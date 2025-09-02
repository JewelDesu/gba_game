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
#include "gm_hitbox.h"

#include "bn_sprite_items_banana.h"
namespace gm
{
    class Player
    {
        public:
            Player(bn::sprite_ptr sprite);

            [[nodiscard]] bn::fixed_point pos();

            void move_left();
            void move_right();
            void jump(bn::affine_bg_ptr map, gm::Level level);
            void wall_jump(bn::affine_bg_ptr map, gm::Level level);
            void apply_animation_state();
            void collide_with_objects(bn::affine_bg_ptr map, gm::Level level);
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
            
            gm::Hitbox _hitbox_left = Hitbox(-4,0,4,16);
            gm::Hitbox _hitbox_right = Hitbox(4,0,6,16);
            gm::Hitbox _hitbox_fall = Hitbox(0,16,16,0);
            gm::Hitbox _hitbox_jump = Hitbox(0,4,6,4);

            bool _running = false;
            bool _grounded = false;
            bool _falling = false;
            bool _jumping = false;
            bool _wallslide = false;
            bool _jumps = true;

            bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 30, bn::sprite_items::banana.tiles_item(), 0,1,2,3,0,1,2,3);
            bn::optional<bn::sprite_animate_action<4>> _spin_action;

            void _update_camera(int lerp);
    };
}

#endif