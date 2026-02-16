#include "gm_coin.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "gm_hitbox.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"

#include "gm_coin_type.h"
#include "gm_hitbox.h"

#include "bn_sprite_items_banana_coin.h"
#include "bn_sprite_items_banana_gem.h"


namespace gm
{
    constexpr bn::fixed_size coin_dimensions(6, 6);

    Coin::Coin(bn::camera_ptr camera, int x, int y, COIN_TYPE type) :
    _camera(camera),
    _pos(x, y), 
    _type(type)
    {
        if(_type == COIN_TYPE::BANANA)
        {
            _sprite = bn::sprite_items::banana_coin.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
        }
        else if(_type == COIN_TYPE::BANANA_GEM)
        {
            _sprite = bn::sprite_items::banana_gem.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                _sprite.value(), 20, bn::sprite_items::banana_gem.tiles_item(), 0,1,2,0,1,2);
        }

        _sprite.value().set_visible(true);
    }

    bool Coin::check_trigger(bn::fixed_point pos)
    {
        if(bn::abs(_pos.x() - pos.x()) < 1)
        {
            if(bn::abs(_pos.y() - pos.y()) < 1)
            {
                on_player = true;
                return true;
            }
        }
        on_player = false;
        return false;
    }

    void Coin::set_collected(bool spawned)
    {
        _collected = true;
        _sprite.value().set_visible(spawned);
    }

    int Coin::is_set_collected()
    {
        if(!_collected)
        {
            //return check_collisions_bb(attack, _pos.x(), _pos.y(), 2, 2);
            return false;
        }      
        else 
        {
            return true;
        }
    }

    int Coin::set_points()
    {
        if(_type == COIN_TYPE::BANANA)
        {
            return 10;
        }
    }

    int Coin::points() const
    {
        if(_type == COIN_TYPE::BANANA) return 10;
        else if(_type == COIN_TYPE::BANANA_GEM) return 100;
        return 0;
    }

    bool Coin::_check_player(const bn::fixed_rect& player_rect)
        {

            //const bn::fixed_point& coin_position = _sprite.sprite().position();
            bn::fixed_rect coin_rect(_pos, coin_dimensions);

            if(coin_rect.intersects(player_rect))
            {
                return true;
            }
                

            return false;
        }

    void Coin::update()
    {

        if(!_collected)
        {
            if(_action.has_value() && !_action.value().done())
            {
                _action.value().update();
            } 
        }
        
    }
}
