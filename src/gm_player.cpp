#include "gm_player.h"

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_size.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_log.h"
#include "bn_fixed_rect.h"
#include "bn_sram.h"
#include "bn_regular_bg_ptr.h"

#include "gm_level.h"
#include "gm_extra.h"
#include "gm_hitbox.h"
#include "gm_enemy.h"
#include "gm_enemy_type.h"
#include "gm_coin.h"

#include "bn_sprite_items_banana.h"
#include "bn_sprite_items_apple.h"
#include "bn_sprite_items_explosion.h"
#include "bn_affine_bg_items_bgg.h"
#include "bn_regular_bg_items_dialogbox.h"
#include "bn_sound_items.h"

namespace gm
{
    enum directions{up, down, left, right, down_bad};


    [[nodiscard]] int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        if(map_size <= 0) 
        {
            return 0;
        }
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool contains_cell(int tile, bn::vector<int, 32> tiles)
    {
        for(int index = 0; index < tiles.size(); ++index)
        {
            if(tiles.at(index) == tile)
            {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool check_collisions_map(bn::fixed_point pos, directions direction,Hitbox hitbox, bn::affine_bg_ptr& map, gm::Level level, bn::span<const bn::affine_bg_map_cell> cells)
    {
        bn::fixed l = pos.x() - hitbox.width() / 2 + hitbox.x();
        bn::fixed r = pos.x() + hitbox.width() / 2 + hitbox.x();
        bn::fixed u = pos.y() - hitbox.height() / 2 + hitbox.y();
        bn::fixed d = pos.y() + hitbox.height() / 2 + hitbox.y();
        
        bn::vector<int, 32> tiles;
        if(direction == down){
            tiles = level.floor_tiles();
        } else if(direction == left || direction == right){
            tiles = level.wall_tiles();
        } else if(direction == up){
            tiles = level.ceiling_tiles();
        } else if(direction == down_bad){
            tiles = level.obsticle_tiles();
        }

        if(contains_cell(get_map_cell(l, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles) ||
        contains_cell(get_map_cell(r, u, map, cells), tiles) ||
        contains_cell(get_map_cell(r, d, map, cells), tiles)){
            return true;
        } else {
            return false;
        }
    }


    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jumpy = 4;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;
    constexpr const bn::fixed wall_slide_speed = 0.35;
    constexpr const bn::fixed lower = 2;
    constexpr int shoot_frames = 5;
    constexpr bn::fixed_size dimensions(16, 16);

    Player::Player(bn::sprite_ptr sprite, PLAYER_TYPE type) :
    _sprite(sprite),
    _type(type),
    _camera(bn::camera_ptr::create(0,0)),
    _map(bn::affine_bg_items::bgg.create_bg(0, 0)),
    _healthbar(gm::Healthbar()),
    _dialogbox(bn::regular_bg_items::dialogbox.create_bg(0,0))
    {
        _map.value().set_visible(false);
        _sprite.put_above();
        _sprite.set_visible(false);
        _healthbar.set_visible(false);
        _facing_left = false;
        _dialogbox.set_priority(1);
        _dialogbox.put_above();
        _dialogbox.set_visible(false);
    }

    int Player::hp()
    {
        return _healthbar.hp();
    }

    void Player::set_hp(int hp)
    {
        _healthbar.set_hp(hp);
    }

    void Player::set_healthbar_visibility(bool is_visible)
    {
        _healthbar.set_visible(is_visible);
    }

    void Player::set_points_visibility(bool is_visable)
    {
        _points.set_visable(is_visable);
    }

    int Player::points()
    {
        return _points.point();
    }

    void Player::set_point(int point)
    {
        _points.set_points(point);
    }

    void Player::spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map, bn::vector<Enemy,16>& enemies, bn::vector<Coin,16>& coins)
    {
        _pos = pos;
        _camera = camera;
        _map = map;
        _map_cells = map.map().cells_ref().value();
        _sprite.set_visible(true);
        _enemies = &enemies;
        _coins = &coins;
        _healthbar.set_visible(false);
        _healthbar.set_hp(globals::savegame.health);
        _points.set_def_points(globals::savegame.points);
        set_point(0);
        reset();
    }

    void Player::delete_data()
    {
        //_sprite.set_visible(false);
        //set_healthbar_visibility(false);
        _camera.reset();
        _map.reset();
        _map_cells.reset();
        _enemies.reset();
        _coins.reset();
    }

     void Player::reset()
    {
        _sprite.set_camera(_camera);
        _sprite.set_bg_priority(1);
        _sprite.put_above();
        _update_camera(1);
        _sprite.set_horizontal_flip(false);
        _dy = 0;
        _dx = 0;
        _falling = false;
        _running = false;
        _grounded = false;
        _jumping = false;
        _wallslide = false;
        _jumps = true;
        _attacking = false;
        _facing_left = false;
        //_melee = 0;
        _weapon_swap = false;
        _invulnerable = false;
        _in_dialog = false;
        //_dialogbox.set_camera(_camera);
    }

    void Player::hide(){
        _healthbar.set_visible(false);
        _sprite.set_visible(false);
        set_points_visibility(false);
    }

    bn::fixed_point Player::pos()
    {
        return _pos;
    }

    void Player::set_dialog(bool is_in_dialog)
    {
        _in_dialog = is_in_dialog;
        _dialogbox.set_visible(_in_dialog);
    }

    bool Player::is_in_dialog()
    {
        return _in_dialog;
    }

    void Player::death()
    {

        if(! _explosion_sprite.has_value())
        {
            _explosion_sprite = bn::sprite_items::explosion.create_sprite(_pos.x(), _pos.y());
            if(_camera.has_value()) _explosion_sprite->set_camera(_camera.value());
            _explosion_sprite->set_bg_priority(0);
            _explosion_sprite->set_visible(false);
        }

        _explosion_sprite->set_position(_pos);
        _explosion_sprite->set_visible(true);

        _explosion = bn::create_sprite_animate_action_once(
                        _explosion_sprite.value(), 12, bn::sprite_items::explosion.tiles_item(),
                        0,1,2,3,4,5,6,7,8,9);

        while(! _explosion.value().done())
        {
            _explosion.value().update();
            bn::core::update();
        }

        _explosion.reset();
        _explosion_sprite.reset();
        delete_data();
    }
    
    void Player::colide_with_enemy(Enemy_Gun& enemy_gun,bn::fixed_rect new_body_rect)
    {
        Hitbox collide_hitobx = Hitbox(_pos.x(),_pos.y()+2, 8, 12);
        for(int i = 0; i < _enemies.value()->size(); i++)
        {
            if (_enemies.value()->at(i).is_hit(collide_hitobx) || enemy_gun.check_player(new_body_rect))
            {
                if(!_invulnerable && _enemies.value()->at(i).hp() > 0)
                {
                    BN_LOG("Enemy hit player!");
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 0.3;
                    BN_LOG("player health: ", _healthbar.hp());
                    if(_dx < 0)
                    {
                        _dx += 6;
                    }
                    else if(_dx > 0)
                    {
                        _dx -= 6;
                    }
                }
            }
        }
    }

    void Player::collide_with_coin(bn::fixed_rect new_body_rect)
    {
        Hitbox collide_hitobx = Hitbox(_pos.x(),_pos.y()+2, 8, 12);

        for(int i = 0; i < _coins.value()->size(); i++)
        {
            if (_coins.value()->at(i)._check_player(new_body_rect) && !_coins.value()->at(i).is_set_collected())
            {
                BN_LOG("Player touched coin");
                int award = _coins.value()->at(i).points();
                set_point(award);
                _coins.value()->at(i).set_collected(false);
            }
        }
    }
    /*
        bool Player::check_enemy_bullet(bn::fixed_point pos)
    {

            Hitbox attack_hitbox = Hitbox(pos.x(),pos.y(), 10, 10);

            if(_sprite.horizontal_flip())
            {
                attack_hitbox.set_x(pos.x() + 8);
            }
            else
            {
                attack_hitbox.set_x(pos.x() - 8);
            }
            if(!_invulnerable && _enemies.value()->at(i).is_hit(attack_hitbox))
            {
                BN_LOG("check_enemy_bullet ");
                if(!_invulnerable && _enemies.value()->at(i).hp() > 0)
                {
                    BN_LOG("Enemy hit player!");
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 0.3;
                    BN_LOG("player health: ", _healthbar.hp());
                    if(_dx < 0)
                    {
                        _dx += 6;
                    }
                    else if(_dx > 0)
                    {
                        _dx -= 6;
                    }
                }
                    return true;
            }
                
            return false;
    }
    */


    void Player::move_left()
    {
        _sprite.set_horizontal_flip(true);
        _running = true;
        _dx -= acc;
        _facing_left = true;
    }

    bool Player::facing_left()
    {
        if(_facing_left )
        {
            return true;
        }
        else
        {
            return false;
            _facing_left = false;
        }
    }

    void Player::move_right()
    {
        _sprite.set_horizontal_flip(false);
        _running = true;
        _dx += acc;
        _facing_left = false;
    }

    void Player::move_down(bn::affine_bg_ptr map, gm::Level level)
    {
        if(_grounded)
        {
            _grounded = false;
            _dy += lower;
        }
    }

    void Player::jump(bn::affine_bg_ptr map, gm::Level level)
    {
        if(!_in_dialog)
        {
            if(_grounded && !_in_dialog)
            {
                _dy -= jumpy;
                _grounded = false;
                bn::sound_items::jump.play();
            }
        }
    }
    void Player::wall_jump(bn::affine_bg_ptr map, gm::Level level)
    {
        if(_wallslide)
        {
            _dy -= jumpy;
            _grounded = false;
            _jumps = false;
            bn::sound_items::jump.play();
        }
    }

    void Player::attack()
    {
        _attacking = true;
        bn::sound_items::bamboo_swish.play();
    }
    void Player::shoot()
    {
        _shooting = true;
        
    }


    void Player::attacking()
    {
        if(_attacking)
        {
            
            Hitbox attack_hitbox = Hitbox(_pos.x(),_pos.y(), 20, 20);

            if(_sprite.horizontal_flip())
            {
                attack_hitbox.set_x(_pos.x() - 8);
            }
            else
            {
                attack_hitbox.set_x(_pos.x() + 8);
            }

            for(int i = 0; i < _enemies.value()->size(); i++)
            {
                if(_enemies.value()->at(i).is_vulnerable() && _enemies.value()->at(i).is_hit(attack_hitbox))
                {
                    int point = 0;
                    if(_sprite.horizontal_flip())
                    {
                        point = _enemies.value()->at(i).damage_from_left(1);
                    }
                    else
                    {
                        point = _enemies.value()->at(i).damage_from_right(1);
                    }

                    if(point > 0)
                    {
                        set_point(point);
                    }
                }
            }
        }
    }

    void Player::collide_with_objects(bn::affine_bg_ptr map, gm::Level level)
    {
        bool touching_left = false;
        bool touching_right = false;
        // if falling
        if(_dy > 0)
        {
            _falling = true;
            _grounded = false;
            _jumping = false;
            
            // clamp max fall speed
            if(_dy > max_dy){
                _dy = max_dy;
            }

            if(check_collisions_map(_pos, down, _hitbox_fall, map, level, _map_cells.value()))
            {
                _grounded = true;
                _falling = false;
                _jumps = true;
                _dy = 0;
                if(_pos.y() >= 0)
                {
                    _pos.set_y(_pos.y() - modulo(_pos.y(), 8));
                }

                //todo if they pressed jump a few milliseconds before hitting the ground then jump now
            }
            else if(check_collisions_map(_pos, down_bad, _hitbox_fall, map, level, _map_cells.value()))
            {
                    BN_LOG("Player touched obsticle falling");
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 3;
                    if(_dx < 0)
                    {
                        _dx -= 2;
                    }
                    else if(_dx > 0)
                    {
                        _dx += 2;
                    }
            }

        }
        else if(_dy < 0 ) // jumping
        {
            _jumping = true;
            _falling = false;
            if(check_collisions_map(_pos, up, _hitbox_jump, map, level, _map_cells.value()))
            {
                _dy = 0;
            }
        }
        if(_dx > 0) // moving right
        {
            if(check_collisions_map(_pos, right,_hitbox_right, map, level, _map_cells.value())){
                _dx = 0;
                touching_right = true;
            }
            else if(check_collisions_map(_pos, down_bad, _hitbox_fall, map, level, _map_cells.value()))
            {
                    BN_LOG("Player touched obsticle moving right");
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 3;
                    if(_dx > 0)
                        _dx = -2;
                    else if(_dx < 0)
                        _dx = 2;
            }
        } 
        else if (_dx < 0) // moving left
        {
            if(check_collisions_map(_pos, left, _hitbox_left, map, level, _map_cells.value())){
                _dx = 0;
                touching_left = true;
            }
            else if(check_collisions_map(_pos, down_bad, _hitbox_fall, map, level, _map_cells.value()))
            {
                    BN_LOG("Player touched obsticle moving left");
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 3;
                    _jumps = false;
                    if(_dx < 0)
                    {
                        _dx = -2;
                    }
                    else if(_dx > 0)
                    {
                        _dx = 2;
                    }
            }
        }

        //wall sliding
        _wallslide = false;
        if(!_grounded && _falling)
        {
            if(bn::keypad::left_held() && touching_left)
            {
                _falling = false;
                _wallslide = true;
            }
            else if(bn::keypad::right_held() && touching_right)
            {
                _falling = false;
                _wallslide = true;
            }

            if(_wallslide)
            {

                if(_dy > wall_slide_speed){
                    _dy = wall_slide_speed;
                }
            }
        }
    }


    bool Player::check_player_bullet(bn::fixed_point pos)
    {

            Hitbox attack_hitbox = Hitbox(pos.x(),pos.y(), 10, 10);

            if(_sprite.horizontal_flip())
            {
                attack_hitbox.set_x(pos.x() + 8);
            }
            else
            {
                attack_hitbox.set_x(pos.x() - 8);
            }

            for(int i = 0;i < _enemies.value()->size(); i++)
            {
            if(_enemies.value()->at(i).is_vulnerable() && _enemies.value()->at(i).is_hit(attack_hitbox))
                {
                    BN_LOG("Player::check_player_bullet ", i);
                    int point = 0;
                    if(_sprite.horizontal_flip())
                    {
                        point = _enemies.value()->at(i).damage_from_left(1);
                    }
                    else
                    {
                        point = _enemies.value()->at(i).damage_from_right(1);
                    }

                    if(point > 0)
                    {
                        set_point(point);
                    }

                    return true;
                }
            }
            return false;
    }
    


    bool Player::check_wall_bullet(bn::fixed_point pos, bn::fixed_point velocity, bn::affine_bg_ptr map, Level level)
    {
        BN_LOG("Gun::update_projectiles wall check");
        directions dir;
        if(bn::abs(velocity.x()) > bn::abs(velocity.y())) {
            dir = (velocity.x() < 0) ? directions::left : directions::right;
        }
        BN_LOG("Gun::update_projectiles direction", dir);

        if(dir == left)
        {        
            bool result = check_collisions_map(pos, left, _bullet_hitbox_left, map, level, _map_cells.value());
            BN_LOG("check_collisions_map result: ", result);
            return result;
        }
        else
        {
            bool result = check_collisions_map(pos, right, _bullet_hitbox_right, map, level, _map_cells.value());        
            BN_LOG("check_collisions_map result: ", result);
            return result;
        }
    }

    void Player::apply_animation_state()
    {
        switch (_type)
        {
            case PLAYER_TYPE::BANANA_CAT:
                apply_animation_state_banana();
                break;
            case PLAYER_TYPE::APPLE_CAT:
                apply_animation_state_apple();
                break;
            default:
                break;
        }
    }

    void Player::apply_animation_state_banana()
    {
        if(_attacking && _action.done()){
            _attacking = false;
        }

        _sprite.set_vertical_scale(1);
        if(_attacking)
        {
            if(_action.graphics_indexes().front() != 13)
            {
                _action = bn::create_sprite_animate_action_once(
                    _sprite, 1, bn::sprite_items::banana.tiles_item(), 13,13,13,13,14,14);
            }
        }
        else if(_jumping)
        {
            if(_action.graphics_indexes().front() != 10)
            {
            _action = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::banana.tiles_item(), 10,11,12,10,11,12);
            }
        }
        else if(_falling)
        {
            _action = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::banana.tiles_item(), 12,12,12,12,12,12);
        }
        else if(_wallslide)
        {
            _action = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::banana.tiles_item(), 2,2,2,2,2,2);
        }
        else if(_running)
        {
            if(_action.graphics_indexes().front() != 8)
            {
                _action = bn::create_sprite_animate_action_forever(
                    _sprite, 2.5, bn::sprite_items::banana.tiles_item(), 8,9,4,5,6,7);
            }
        }
        else 
        {
            if(_action.graphics_indexes().front() != 3)
            {
                _action = bn::create_sprite_animate_action_forever(
                    _sprite, 15, bn::sprite_items::banana.tiles_item(), 3,0,1,2,3,0,1,2);
            }
        }

        _action.update();
    }

    void Player::apply_animation_state_apple()
    {
        if(_attacking && _action_apple.done()){
            _attacking = false;
        }

        _sprite.set_vertical_scale(1);
        if(_attacking)
        {
            if(_action_apple.graphics_indexes().front() != 13)
            {
                _action_apple = bn::create_sprite_animate_action_once(
                    _sprite, 1, bn::sprite_items::apple.tiles_item(), 13,13,13,13,14,14);
            }
        }
        else if(_jumping)
        {
            if(_action_apple.graphics_indexes().front() != 10)
            {
            _action_apple = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::apple.tiles_item(), 10,11,12,10,11,12);
            }
        }
        else if(_falling)
        {
            _action_apple = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::apple.tiles_item(), 12,12,12,12,12,12);
        }
        else if(_wallslide)
        {
            _action_apple = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::apple.tiles_item(), 2,2,2,2,2,2);
        }
        else if(_running)
        {
            if(_action_apple.graphics_indexes().front() != 8)
            {
                _action_apple = bn::create_sprite_animate_action_forever(
                    _sprite, 2.5, bn::sprite_items::apple.tiles_item(), 8,9,4,5,6,7);
            }
        }
        else 
        {
            if(_action_apple.graphics_indexes().front() != 3)
            {
                _action_apple = bn::create_sprite_animate_action_forever(
                    _sprite, 15, bn::sprite_items::apple.tiles_item(), 3,0,1,2,3,0,1,2);
            }
        }

        _action_apple.update();
    }

    void Player::_update_camera(int lerp)
    {
        // update camera
        if(_pos.x() < 122+30)
        {
            _camera.value().set_x(_camera.value().x()+ (122-_camera.value().x()) /lerp);
        } else if (_pos.x() > 922-30){
            _camera.value().set_x(_camera.value().x()+ (922-20-_camera.value().x()) /lerp);
        }
        else
        {
            if(_sprite.horizontal_flip()){
                _camera.value().set_x(_camera.value().x()+ (_pos.x() - 30-_camera.value().x() + _dx*8) /lerp);
            } else {
                _camera.value().set_x(_camera.value().x()+ (_pos.x() +30 -_camera.value().x() + _dx*8) /lerp);
            }            
        }

        if(_pos.y() > 942){
            _camera.value().set_y(_camera.value().y()+(942-_camera.value().y()) /lerp);
        } else if(_pos.y() < 90){
            _camera.value().set_y(_camera.value().y()+(90-_camera.value().y()) /lerp);
        } else {
            _camera.value().set_y(_camera.value().y()+ (_pos.y()-10-_camera.value().y()) /lerp);
        }
    }

    bool Player::in_menu()
    {
        if (_in_menu)
            return true;
        return false;
    }

    Scene Player::to_menu()
    {
        return Scene::TITLE;
    }

    void Player::set_player(PLAYER_TYPE type)
    {
        _type = type;
    }

    void Player::update_pos(bn::affine_bg_ptr map, gm::Level level, Gun& gun, const bn::camera_ptr& camera, bn::vector<Enemy,16>& enemies, Enemy_Gun& enemy_gun)
    {
        bn::fixed_point body_position = _pos;
        bn::fixed_rect new_body_rect(body_position, dimensions);

        _update_camera(30 - bn::abs(_dx.integer())*5);
        _dx = _dx * friction;
        _dy+= gravity;

        if(_shoot_cooldown > 0)
        {
            --_shoot_cooldown;
        }


        if(bn::keypad::left_held() && !_in_dialog)
        {
            move_left();
        }
        else if(bn::keypad::right_held()  && !_in_dialog)
        {
            move_right();
        }
        else if(_running & ! _jumping) 
        {
            _running = false;
        } 

        if(bn::keypad::a_pressed()  && !_in_dialog)
        {
            if(_wallslide && _jumps)
            {
                wall_jump(map, level);
            }
            else
                jump(map, level);
            
        }

        if(bn::keypad::down_pressed())
        {
            move_down(map, level);
        }

        if(bn::keypad::b_pressed()  && !_in_dialog)
        {
            
            if(_melee == 0)
            {
                attack();
            }
            else
            {
                if(_shoot_cooldown == 0)
                {
                    gun.shoot(camera, *this);
                    _shoot_cooldown = _shoot_delay;
                }
            }

            
        }
        gun.update_projectiles(camera, *this,map,level);

        if(bn::keypad::l_pressed())
        {
            BN_LOG("Player position: ", pos().x(), ", ", pos().y());
            BN_LOG("bn::keypad::left_pressed() ", _melee);
            if(_melee == 0)
            {
                _melee = 1;
                _weapon = Weapon::RANGED;
            }
            else
            {
                _melee -= 1;
                _weapon = Weapon::MELEE;
            }
            _healthbar.set_weapon(_weapon);
        }

         if(bn::keypad::r_pressed())
        {
             BN_LOG("Player position: ", pos().x(), ", ", pos().y());
            BN_LOG("bn::keypad::left_pressed() ", _melee);
            if(_melee == 0)
            {
                _melee = 1;
                _type = PLAYER_TYPE::APPLE_CAT;
            }
            else
            {
                _melee -= 1;
                _type = PLAYER_TYPE::BANANA_CAT;
            }
        }
        
        attacking();

        if(bn::keypad::start_pressed() && !_in_dialog)
        {
            _in_menu = true;
            while(_in_menu)
            {
                bn::vector<bn::string_view,4> options;
                options.push_back("Continue");
                options.push_back("Save Game");
                options.push_back("Return to title screen");
                int select = globals::gui->textbox("Game is paused", options, -113, 35, true);
                if (select == 1)
                {
                    globals::savegame.health = hp();
                    globals::savegame._pos = pos();
                    bn::sram::write(globals::savegame);
                    globals::gui->textbox("Saved Game");
                    BN_LOG("Game saved with player health: ", hp());
                    BN_LOG("Game saved with player position: ", pos().x(), ", ", pos().y());
                    _in_menu = false;
                }
                else if(select == 2)
                {
                    globals::menu = true;
                    _in_menu = false;
                }
                else if(select == 0)
                {
                    _in_menu = false;
                }

            }
        }

        if(_in_dialog)
        {
            _dialogbox.set_position(0,0);
        }

        collide_with_objects(map, level);
        colide_with_enemy(enemy_gun,new_body_rect);
        collide_with_coin(new_body_rect);
        

        if(_invulnerable)
        {
            ++_invuln_timer;
            if(modulo(_invuln_timer/5, 2) == 0)
            {
                _sprite.set_visible(true);
            } 
            else 
            {
                _sprite.set_visible(false);
            }
            if(_invuln_timer > 120)
            {
                _invulnerable = false;
                _invuln_timer = 0;
                _sprite.set_visible(true);
            }
        }

        _pos.set_x(_pos.x() + _dx);
        _pos.set_y(_pos.y() + _dy);

        if(_pos.x() > 1016){
            _pos.set_x(1016);
        } else if(_pos.x() < 4){
            _pos.set_x(4);
        }

        _sprite.set_x(_pos.x());
        _sprite.set_y(_pos.y());
    }
}