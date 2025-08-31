#ifndef GM_PLAYER_H
#define GM_PLAYER_H

#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_cell.h"
#include "bn_sprite_animate_actions.h"
#include "bn_span.h"

#include "gm_level.h"

#include "bn_sprite_items_cat_sprite.h"
namespace gm
{
    class Player
    {
        public:
            Player(bn::sprite_ptr sprite);

            [[nodiscard]] bn::fixed_point pos();

            void move_left();
            void move_right();
            void update_pos(bn::affine_bg_ptr map, gm::Level level);
            void spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map);
            void reset();

        private:
            bn::sprite_ptr _sprite;
            bn::fixed _dx;
            bn::fixed _dy;
            bn::fixed_point _pos;
            bn::optional<bn::camera_ptr> _camera;
            bn::optional<bn::affine_bg_ptr> _map;

            bn::optional<bn::span<const bn::affine_bg_map_cell>> _map_cells;
            

            bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 30, bn::sprite_items::cat_sprite.tiles_item(), 0,1,0,1,0,1,0,1,0,1);
            bn::optional<bn::sprite_animate_action<4>> _spin_action;

            void _update_camera(int lerp);
    };
}

#endif