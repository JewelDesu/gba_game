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

#include "gm_level.h"
#include "gm_extra.h"
#include "gm_hitbox.h"

#include "bn_sprite_items_banana.h"
#include "bn_affine_bg_items_house.h"

namespace gm
{
    enum directions{up, down, left, right};


    [[nodiscard]] int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool contains_cell(int tile, bn::vector<int, 64> tiles)
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
        
        bn::vector<int, 64> tiles;
        if(direction == down){
            tiles = level.floor_tiles();
        } else if(direction == left || direction == right){
            tiles = level.wall_tiles();
        } else if(direction == up){
            tiles = level.ceiling_tiles();
        }

        if(contains_cell(get_map_cell(l, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles) ||
        contains_cell(get_map_cell(r, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles)){
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

    Player::Player(bn::sprite_ptr sprite) :
    _sprite(sprite),
    _camera(bn::camera_ptr::create(0,0)),
    _map(bn::affine_bg_items::house.create_bg(0, 0))
    {
        _map.value().set_visible(false);
        _sprite.put_above();
        _sprite.set_visible(false);
    }

    void Player::spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map)
    {
        _pos = pos;
        _camera = camera;
        _map = map;
        _map_cells = map.map().cells_ref().value();
        _sprite.set_visible(true);
        reset();
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
    }
    bn::fixed_point Player::pos()
    {
        return _pos;
    }

    void Player::collide_with_objects(bn::affine_bg_ptr map, gm::Level level){
        // if falling
        if(_dy > 0){
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
                _dy = 0;
                _pos.set_y(_pos.y() - modulo(_pos.y(),8));
                //todo if they pressed jump a few milliseconds before hitting the ground then jump now
            }
        }
        else if(_dy < 0) // jumping
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
            }
        } 
        else if (_dx < 0) // moving left
        {
            if(check_collisions_map(_pos, left, _hitbox_left, map, level, _map_cells.value())){
                _dx = 0;
            }
        }
    }

    void Player::move_left()
    {
        _sprite.set_horizontal_flip(true);
        _running = true;
        _dx -= acc;
    }

    void Player::move_right()
    {
        _sprite.set_horizontal_flip(false);
        _running = true;
        _dx += acc;
    }

    void Player::jump(bn::affine_bg_ptr map, gm::Level level)
    {
        if(_grounded)
        {
            _dy -= jumpy;
            _grounded = false;
        }
    }

    void Player::apply_animation_state()
    {
        _sprite.set_vertical_scale(1);
        if(_jumping)
        {
            if(_action.graphics_indexes().front() != 10){
            _action = bn::create_sprite_animate_action_forever(
                _sprite, 6, bn::sprite_items::banana.tiles_item(), 10,11,12);
            }
        }
        else if(_running)
        {
            if(_action.graphics_indexes().front() != 8){
                _action = bn::create_sprite_animate_action_forever(
                    _sprite, 2.5, bn::sprite_items::banana.tiles_item(), 8,9,4,5,6,7);
            }
        }
        else 
        {
            if(_action.graphics_indexes().front() != 3){
                _action = bn::create_sprite_animate_action_forever(
                    _sprite, 15, bn::sprite_items::banana.tiles_item(), 3,0,1,2,3,0,1,2);
            }
        }

        _action.update();
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

    void Player::update_pos(bn::affine_bg_ptr map, gm::Level level)
    {
        _update_camera(30 - bn::abs(_dx.integer())*5);
        _dx = _dx * friction;
        _dy+= gravity;


        if(bn::keypad::left_held())
        {
            move_left();
        }
        else if(bn::keypad::right_held())
        {
            move_right();
        }
        else if(_running & ! _jumping) 
        {
            _running = false;
        } 

        if(bn::keypad::a_pressed())
        {
            jump(map, level);
        }

        collide_with_objects(map, level);

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