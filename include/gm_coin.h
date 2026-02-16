#ifndef GM_COIN_H
#define GM_COIN_H

#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_sprite_animate_actions.h"

#include "gm_coin_type.h"
#include "gm_hitbox.h"


namespace gm
{
    class Coin
    {
        public:
            Coin(bn::camera_ptr camera, int x, int y, COIN_TYPE type);
            bool check_trigger(bn::fixed_point pos);
            void set_collected(bool spawned);
            int is_set_collected();
            int set_points();
            bool _check_player(const bn::fixed_rect& player_rect);
            int points() const;
            void update();

        private:
            COIN_TYPE _type;
            bn::fixed_point _pos;
            bn::camera_ptr& _camera;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<6>> _action;

            bool on_player = false;
            bool _collected = false;



    };
}

#endif