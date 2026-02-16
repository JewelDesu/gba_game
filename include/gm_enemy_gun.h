#ifndef GM_ENEMY_GUN_H
#define GM_ENEMY_GUN_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_actions.h"
#include "bn_math.h"
#include "bn_forward_list.h"
#include "bn_affine_bg_ptr.h"
#include "bn_fixed_rect.h"
#include "bn_random.h"

#include "gm_enemy.h"
#include "gm_constants.h"
#include "gm_bullet_type.h"



namespace gm
{
    class Enemy;
    class Enemy_Gun
    {
        public:

            Enemy_Gun();
            void enemy_spawn_projectile(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, bn::fixed speed);
            void enemy_update_projectiles(const bn::camera_ptr& camera, bn::fixed_point player, bn::affine_bg_ptr map, Level level, Enemy& enemy, bool _dead);
            void enemy_shoot(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, BulletPattern pattern, bn::fixed speed);
            bool check_player(const bn::fixed_rect& player_rect);
            void spawn_single(const bn::camera_ptr& camera,bn::fixed_point& enemy,const bn::fixed_point& direction);
            void spawn_spread(const bn::camera_ptr& camera,bn::fixed_point& player,bn::fixed_point& enemy,int count,bn::fixed angle_step, bn::fixed speed);
            void spawn_circle(const bn::camera_ptr& camera,bn::fixed_point& enemy,int count,bn::fixed speed);
            void spawn_rain(const bn::camera_ptr& camera,bn::fixed_point& enemy,int count,bn::fixed speed);
            void spawn_random(const bn::camera_ptr& camera,bn::fixed_point& enemy,int count,bn::fixed speed);
            //void dash(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy);
        private:
            struct Projectile
            {
                bn::sprite_move_by_action sprite_move_action;
                bn::fixed_point _velocity;
                int _distance_traveled = 0;
            };

            bool _shooting = false;
            bn::vector<Projectile, 24> _projectiles;
            bn::sprite_palette_ptr _palette;
            bn::array<bn::sprite_tiles_ptr, 2> _tiles;
            bn::fixed _dx;
            bool _dead = false;
            bn::random _random;
            

            
            


    };
}

#endif