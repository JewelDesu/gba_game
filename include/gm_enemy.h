#ifndef GM_ENEMY_H
#define GM_ENEMY_H

#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_cell.h"
#include "bn_sprite_animate_actions.h"
#include "bn_span.h"

#include "gm_enemy_type.h"
#include "gm_level.h"
#include "gm_hitbox.h"

namespace gm
{
    class Enemy
    {
        public:




        private:
        bn::fixed _dx;
        bn::fixed _dy;
        bn::fixed_point _pos;
        bn::optional<bn::camera_ptr> _camera;
        bn::optional<bn::affine_bg_ptr> _map;
        bn::optional<bn::span<const bn::affine_bg_map_cell>> _map_cells;

        int _dir;
        bool _running = false;
        bool _grounded = false;
        bool _falling = false;
        bool _jumping = false;
        bool _wallslide = false;
        bool _jumps = true;

        bn::optional<bn::sprite_ptr> _spite;
        bn::fixed_point _target = bn::fixed_point(0,0);
    }
}

#endif