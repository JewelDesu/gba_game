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


#include "bn_sprite_items_cat_sprite.h"
#include "bn_affine_bg_items_house.h"

namespace gm
{
    enum directions{up, down, left, right};

    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jump = 4;
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

     void Player::reset(){
        _sprite.set_camera(_camera);
        _sprite.set_bg_priority(1);
        _sprite.put_above();
        _update_camera(1);
        _sprite.set_horizontal_flip(false);
        _dy = 0;
        _dx = 0;
    }

    void Player::move_left()
    {
        _sprite.set_horizontal_flip(true);
        _dx -= acc;
    }

    void Player::move_right()
    {
        _sprite.set_horizontal_flip(false);
        _dx += acc;
    }

        void Player::_update_camera(int lerp){
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
        if(bn::keypad::left_held())
        {
            move_left();
        }
        else if(bn::keypad::right_held())
        {
            move_right();
        }

        _pos.set_x(_pos.x() + _dx);
        _pos.set_y(_pos.y() + _dy);

        _sprite.set_x(_pos.x());
        _sprite.set_y(_pos.y());
    }
}