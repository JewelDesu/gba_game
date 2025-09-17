#ifndef GM_GUN_H
#define GM_GUN_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_math.h"
#include "bn_forward_list.h"
#include "bn_affine_bg_ptr.h"

#include "gm_enemy.h"
#include "gm_player.h"
#include "gm_constants.h"


namespace gm
{
    class Player;
    class Gun
    {
        public:
            Gun();
            void spawn_projectile(const bn::camera_ptr& camera, Player& player);
            void update_projectiles(const bn::camera_ptr& camera, Player& player, bn::affine_bg_ptr map, Level level);
            void shoot(const bn::camera_ptr& camera, Player& player);
        private:
            struct Projectile
            {
                bn::sprite_ptr _sprite;
                bn::fixed_point _velocity;
                int _distance_traveled = 0;
            };

            bool _shooting = false;
            bn::vector<Projectile, 32> _projectiles;
            bn::sprite_palette_ptr _palette;
            bn::array<bn::sprite_tiles_ptr, 2> _tiles;
            bn::fixed _dx;
            bn::optional<bn::sprite_animate_action<4>> _explosions;
            

            
            


    };
}

#endif