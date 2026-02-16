
#include "gm_enemy.h"

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
#include "bn_display.h"

#include "gm_enemy_type.h"
#include "gm_hitbox.h"
#include "gm_collision.h"
#include "gm_bullet_extra.h"
#include "gm_enemy_gun.h"

#include "bn_sprite_items_goomba.h"
#include "bn_sprite_items_slime.h"
#include "bn_sprite_items_glorp_craft.h"
#include "bn_sound_items.h"

namespace gm
{
    enum directions{up, down, left, right};


    [[nodiscard]] int _get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool _contains_cell(int tile, bn::vector<int, 32> tiles)
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

    [[nodiscard]] bool _check_collisions_map(bn::fixed_point pos, directions direction,Hitbox hitbox, bn::affine_bg_ptr& map, gm::Level level, bn::span<const bn::affine_bg_map_cell> cells)
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
        }

        if(_contains_cell(_get_map_cell(l, u, map, cells), tiles) ||
        _contains_cell(_get_map_cell(l, d, map, cells), tiles) ||
        _contains_cell(_get_map_cell(r, u, map, cells), tiles) ||
        _contains_cell(_get_map_cell(l, d, map, cells), tiles)){
            return true;
        } else {
            return false;
        }
    }

    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jumpy = 4;
    constexpr const bn::fixed acc = 0.1;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Enemy::Enemy(bn::camera_ptr camera, bn::affine_bg_ptr map, int x, int y, ENEMY_TYPE type, int hp):
    _camera(camera),
    _map(map),
    _pos(x, y),
    _type(type),
    _level(Level(map)),
    _hp(hp)
    {
        _map_cells = map.map().cells_ref().value();
        _dir = 1;
        map_cells = map.map().cells_ref().value();
        if(_type == ENEMY_TYPE::GOOMBA || _type == ENEMY_TYPE::GOOMBA_BOSS)
        {
            _sprite = bn::sprite_items::goomba.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                _sprite.value(), 6, bn::sprite_items::goomba.tiles_item(), 0,1,0,1);
        }
        else if (_type == ENEMY_TYPE::SLIME)
        {
            _sprite = bn::sprite_items::slime.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                             _sprite.value(), 20, bn::sprite_items::slime.tiles_item(), 0,1,0,1);
        } 
        else if (_type == ENEMY_TYPE::GLORP_CRAFT)
        {
            _sprite = bn::sprite_items::glorp_craft.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                             _sprite.value(), 20, bn::sprite_items::glorp_craft.tiles_item(), 0,0);
        } 
        _sprite.value().set_visible(true);
    }

    void Enemy::set_visible(bool spawned)
    {
        _sprite.value().set_visible(spawned);
    }

    
    bool Enemy::is_vulnerable()
    {
        return !_invulnerable; 
    }

    int Enemy::damage_from_left(int damage)
    {
        if(_type != ENEMY_TYPE::GLORP_CRAFT)
        {
            _dy -= 0.4;
            _dx -= 1;
            _dir = 1;
            _direction_timer = 0;
            _grounded = false;
            _sprite.value().set_horizontal_flip(true);
        }
        
        return damage_taken(damage);
    }

    int Enemy::damage_from_right(int damage)
    {
        if(_type != ENEMY_TYPE::GLORP_CRAFT)
        {
            _dy -= 0.4;
            _dx += 1;
            _dir = -1;
            _direction_timer = 0;
            _grounded = false;
            _sprite.value().set_horizontal_flip(false);
        }
        
        return damage_taken(damage);
    }

    int Enemy::damage_taken(int damage)
    {
        if(!_invulnerable)
        {
            _hp -= damage;
            _invulnerable = true;

            if(_hp <= 0)
            {
                int points = 0;
                bn::sound_items::pop1.play();
                if(_type == ENEMY_TYPE::GOOMBA)
                {
                    //_points.set_points(_points.point() + 1);
                    points = 1;
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(),5, bn::sprite_items::goomba.tiles_item(), 2,2,2,2);         
                }
                else if(_type == ENEMY_TYPE::SLIME)
                {
                    //_points.set_points(_points.point() + 1);
                    points = 1;
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(),5, bn::sprite_items::slime.tiles_item(), 2,3,2,3);
                }
                else if(_type == ENEMY_TYPE::GOOMBA_BOSS)
                {
                    //_points.set_points(_points.point() + 10);
                    points = 10;
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(),5, bn::sprite_items::goomba.tiles_item(), 2,2,2,2);
                }
                else if(_type == ENEMY_TYPE::GLORP_CRAFT)
                {
                    //_points.set_points(_points.point() + 10);
                    points = 100000;
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(),1, bn::sprite_items::goomba.tiles_item(), 0,0);
                }
                return points;
            }
        }

        return false;

    }

    bool Enemy::is_hit(Hitbox attack)
    {
        
        //BN_LOG("Enemy pos:", _pos.x().integer(), ",", _pos.y().integer());
        if(!_dead)
        {
            if(_type == ENEMY_TYPE::GOOMBA || _type == ENEMY_TYPE::GOOMBA_BOSS)
            {
                return check_collisions_bb(attack, _pos.x(), _pos.y(), 8, 16);
            } 
            else if(_type == ENEMY_TYPE::GLORP_CRAFT)
            {
                return check_collisions_bb(attack, _pos.x(), _pos.y(),32 ,64);
            }
            else 
            {
                return check_collisions_bb(attack, _pos.x(), _pos.y(), 8, 8);
            }
            
        } 
        else 
        {
            return false;
        }
    }

    bool Enemy::_will_hit_wall()
    {   
        if(_check_collisions_map(_pos, directions::left, Hitbox(-4, 0, 12, 12), _map, _level, _map_cells)){
            return true;
        }
        if(_check_collisions_map(_pos, directions::right, Hitbox(10, 0, 12, 12), _map, _level, _map_cells)){
            return true;
        }
        return false;
    }

    bool Enemy::_will_fall()
    {
        if(_type == ENEMY_TYPE::GOOMBA || _type == ENEMY_TYPE::GOOMBA_BOSS)
        {
            if(_dx < 0){ // left
                if(!_check_collisions_map(_pos, directions::down, Hitbox(-4,16,4,8), _map, _level, _map_cells))
                {
                    return true;
                }
            } 
            else 
            { //right
                    
                if(!_check_collisions_map(_pos, directions::down, Hitbox(4,16,4,8),  _map, _level, _map_cells))
                {
                    return true;
                }
            }
        }
        else
        {
            if(_dx < 0){ // left
                if(!_check_collisions_map(_pos, directions::down, Hitbox(-4,8,4,8), _map, _level, _map_cells))
                {
                    return true;
                }
            } 
            else 
            { //right
                    
                if(!_check_collisions_map(_pos, directions::down, Hitbox(4,8,4,8),  _map, _level, _map_cells))
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool Enemy::_fall_check(bn::fixed x, bn::fixed y)
    {  
        if(_check_collisions_map(bn::fixed_point(x, y), directions::down, Hitbox(0,16,4,9), _map, _level, _map_cells))
        {
            return true;
        } 
        else 
        {
            return false;
        }
    }

    bool Enemy::check_wall_bullet(bn::fixed_point pos, bn::fixed_point velocity, bn::affine_bg_ptr map, Level level)
    {
        //BN_LOG("Gun::update_projectiles wall check");
        directions dir;
        if(bn::abs(velocity.x()) > bn::abs(velocity.y())) {
            dir = (velocity.x() < 0) ? directions::left : directions::right;
        }
        else {
        dir = (velocity.y() < 0) ? directions::up : directions::down;
        }
        //BN_LOG("Gun::update_projectiles direction", dir);

        if(dir == up)
        {        
            bool result = _check_collisions_map(pos, up, _bullet_hitbox_left, map, level, map_cells.value());
            //BN_LOG("check_collisions_map result: ", result);
            return result;
        }
        else
        {
            bool result = _check_collisions_map(pos, down, _bullet_hitbox_right, map, level, map_cells.value());        
            //BN_LOG("check_collisions_map result: ", result);
            return result;
        }
    }

    int Enemy::hp()
    {
        return _hp;
    }

    bn::fixed_point Enemy::pos()
    {
        return _pos;
    }

    void Enemy::set_pos(bn::fixed_point pos)
    {
        _pos = pos;
    }
    ENEMY_TYPE Enemy::type()
    {
        return _type;
    }

    void Enemy::update_cooldowns()
    {
        if(_shoot_cooldown > 0)
        {
            --_shoot_cooldown;
        }
    }

    bool Enemy::side()
    {
        return _sprite.value().horizontal_flip();
    }


    void Enemy::update_pos_enemy(bn::fixed_point player_pos, Enemy_Gun& gun)
    {
        if(!_dead)
        {


            if(!_sprite.value().visible())
            {
                _sprite.value().set_visible(true);
            }

                        if(_invulnerable)
            {
                ++_invuln_timer;
                if(modulo(_invuln_timer/3, 2) == 0)
                {
                    _sprite.value().set_visible(true);
                } 
                else 
                {
                    _sprite.value().set_visible(false);
                }
                if(_invuln_timer > 20)
                {
                    _invulnerable = false;
                    _invuln_timer = 0;
                    _sprite.value().set_visible(true);
                }
            }

            if(_action.has_value() && _action.value().done())
            {
                _sprite.value().set_visible(false);
                _dead = true;
            }

            if(_type != ENEMY_TYPE::GLORP_CRAFT)
            {
                _dy += gravity;
            }

            if(_type == ENEMY_TYPE::GOOMBA || _type == ENEMY_TYPE::SLIME || _type == ENEMY_TYPE::GOOMBA_BOSS)
            {
                if(_grounded && _direction_timer > 30 )
                {
                    if(!_invulnerable && _will_fall() || _will_hit_wall())
                    {
                        _dx = 0;
                        _dir = -_dir;
                        _direction_timer = 0;
                        _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());
                    }
                }
                if(_type == ENEMY_TYPE::SLIME)
                {
                    if((_action.value().current_index() == 1 || _action.value().current_index() == 3 && !_invulnerable && _grounded))
                    {
                        _dx += _dir*acc;
                    }
                }
                else if(_type == ENEMY_TYPE::GOOMBA_BOSS)
                {
                    if(_dash_cooldown > 0)
                    {
                        --_dash_cooldown;
                    }

                    bn::fixed diff_x = player_pos.x() - _pos.x();
                    bn::fixed diff_y = player_pos.y() - _pos.y();


                    bool safe_to_move = !_will_fall() && !_will_hit_wall();

                    if(diff_x < 0 && !_sprite.value().horizontal_flip())
                    {
                        _sprite.value().set_horizontal_flip(true);
                        _dir = -1;
                    }
                    else if(diff_x > 0 && _sprite.value().horizontal_flip())
                    {
                        _sprite.value().set_horizontal_flip(false);
                        _dir = 1;
                    }

                    if(!_is_dashing && _dash_cooldown == 0 && bn::abs(diff_x) > 16 && bn::abs(diff_x) < 160 && bn::abs(diff_y) < 20)
                    {
                        _is_dashing = true;
                        _dash_distance_traveled = 0;
                        _dir = (diff_x < 0) ? -1 : 1;

                        bn::sound_items::pop1.play();
                    }

                    if(_is_dashing)
                    {
                        _dx = _dir * _dash_speed;
                        _dash_distance_traveled += bn::abs(_dx);

                        if(_dash_distance_traveled >= _max_dash_distance ||
                           _will_hit_wall() || _will_fall())
                        {
                            _is_dashing = false;
                            _dash_cooldown = 120;
                            _dx = 0;
                            _dash_distance_traveled = 0;
                            BN_LOG("dash ended");
                        }
                    }
                    else
                    {
                        if(!_invulnerable && _grounded)
                        {
                            _dx += _dir * acc;
                        }

                        if(_shoot_cooldown == 0)
                        {
                            gun.enemy_shoot(_camera,player_pos, _pos , BulletPattern::SPREAD, 1);
                            _shoot_cooldown = _shoot_circle_delay;
                        }
                    }
                    gun.enemy_update_projectiles(_camera, player_pos,_map,_level,*this,_dead);
                }
                else
                {
                    if(player_pos.y() - 8 == _pos.y())
                    {
                        if(player_pos.x() < _pos.x() && bn::abs(player_pos.x() - _pos.x()) > 8 && !_sprite.value().horizontal_flip())
                        {
                            _dx = 0;
                            _dir = -_dir;
                            _direction_timer = 0;
                            _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());
                        } 
                        else if(player_pos.x() > _pos.x() && bn::abs(player_pos.x() - _pos.x()) < 8 && _sprite.value().horizontal_flip())
                        {
                            _dx = 0;
                            _dir = -_dir;
                            _direction_timer = 0;
                            _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());
                        }

                    }
                    if(!_invulnerable && _grounded)
                    {
                        _dx += _dir*acc;
                    } 

                    if(_type == ENEMY_TYPE::GOOMBA && _shoot_cooldown == 0)
                    {
                        gun.enemy_shoot(_camera,player_pos, _pos , BulletPattern::DIRECT, 0.8);
                        _shoot_cooldown = _shoot_circle_delay;
                    }
                    gun.enemy_update_projectiles(_camera, player_pos,_map,_level,*this,_dead);


                }
            }
            else if(_type == ENEMY_TYPE::GLORP_CRAFT)
            {
                if(_dash_cooldown > 0)
                {
                    --_dash_cooldown;
                }

                bn::fixed diff_x = player_pos.x() - _pos.x();
                bn::fixed diff_y = player_pos.y() - _pos.y();

                if(diff_x < 0 && !_sprite.value().horizontal_flip())
                {
                    _sprite.value().set_horizontal_flip(true);
                    _dir = -1;
                }
                else if(diff_x > 0 && _sprite.value().horizontal_flip())
                {
                    _sprite.value().set_horizontal_flip(false);
                    _dir = 1;
                }

                if(!_is_dashing &&
                _dash_cooldown == 0 &&
                bn::abs(diff_x) > 16 && bn::abs(diff_x) < 160 &&
                bn::abs(diff_y) < 40)
                {
                    _is_dashing = true;
                    _dash_distance_traveled = 0;
                    _dir = (diff_x < 0) ? -1 : 1;

                    bn::sound_items::pop1.play();
                }


                if(_is_dashing)
                {
                    _dx = _dir * _dash_speed;
                    _dash_distance_traveled += bn::abs(_dx);

                    if(_dash_distance_traveled >= _max_dash_distance ||
                    _will_hit_wall())
                    {
                        _is_dashing = false;
                        _dash_cooldown = 120;
                        _dx = 0;
                        _dash_distance_traveled = 0;
                    }

                    _dy = 0;
                }
                else
                {
                    
                    if(!_invulnerable)
                    {
                        _dx = diff_x / 100;  
                    }

                    _dy = diff_y / 100;        // smooth flying 

                    if(_dy > 1)  _dy = 1;
                    if(_dy < -1) _dy = -1;

                    if(_shoot_cooldown == 0)
                    {
                        gun.enemy_shoot(_camera, player_pos, _pos, BulletPattern::CIRCLE, 1);
                        _shoot_cooldown = _shoot_circle_delay;
                    }
                }

                gun.enemy_update_projectiles(_camera, player_pos, _map, _level, *this, _dead);
            }



            _dx = _dx * friction;

            if(_dx > 0)
            {
                _sprite.value().set_horizontal_flip(false);
            } 
            else if(_dx < 0)
            {
                _sprite.value().set_horizontal_flip(true);
            }

            if(_dy > 0)
            {
                if(_type == ENEMY_TYPE::GOOMBA)
                {
                    if(_check_collisions_map(_pos, directions::down, Hitbox(0,16,8,0), _map, _level, _map_cells))
                    {
                        _dy = 0;
                        _pos.set_y(_pos.y() - modulo(_pos.y(),8));
                        _grounded = true;
                    } 
                    else 
                    {
                        _grounded = false;
                    }
                }  
                else if(_type == ENEMY_TYPE::GOOMBA_BOSS || _type == ENEMY_TYPE::GLORP_CRAFT)
                {
                    if(_check_collisions_map(_pos, directions::down, Hitbox(0,16,8,0), _map, _level, _map_cells))
                    {
                        _dy = 0;
                        _pos.set_y(_pos.y() - modulo(_pos.y(),8));
                        _grounded = true;
                    } 
                    else 
                    {
                        _grounded = false;
                    }
                } 
                else
                {
                    if(_check_collisions_map(_pos, directions::down, Hitbox(0,8,8,0), _map, _level, _map_cells))
                    {
                        _dy = 0;   
                        _pos.set_y(_pos.y() - modulo(_pos.y(),8));
                        _grounded = true;
                    } 
                    else 
                    {
                        _grounded = false;
                    }
                }
            }

            if(bn::abs(_dx) > 0)
            {
                if(_check_collisions_map(_pos, directions::left, Hitbox(0, 0, 4, 8), _map, _level, _map_cells))
                {
                    _dx = -_dx;
                    // _direction_timer = 0;
                }
            }

            if(_dy > max_dy)
            {
                _dy = max_dy;
            }



            if(_hp >= 0)
            {
                _pos.set_x(_pos.x() + _dx);
                _pos.set_y(_pos.y() + _dy);
                _sprite.value().set_position(_pos);
                _sprite.value().set_y(_sprite.value().y() - 1); 
            }

            if(_action.has_value() && !_action.value().done())
            {
                _action.value().update();
            }  

            if(_direction_timer < 121)
            {
                _direction_timer+=1;
            }


        }
    }
}
