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
#include "gm_player_stats.h"

namespace gm
{
    class Enemy_Gun;
    class Player_bullet_data;
    class Enemy
    {
        public:
            Enemy(bn::camera_ptr camera, bn::affine_bg_ptr map, int x, int y, ENEMY_TYPE type, int hp);
            int hp();
            void set_visible(bool spawned);
            bn::fixed_point pos();
            void set_pos(bn::fixed_point pos);
            void update_pos_enemy(bn::fixed_point player_pos, Enemy_Gun& gun);
            ENEMY_TYPE type();
            bool _will_hit_wall();
            bool _will_fall();
            bool is_vulnerable();
            int damage_from_right(int damage);
            int damage_from_left(int damage);
            int damage_taken(int damage);
            bool is_hit(Hitbox attack);
            void update_cooldowns();
            bool side();
            bool _fall_check(bn::fixed x, bn::fixed y);

            bool check_wall_bullet(bn::fixed_point pos, bn::fixed_point velocity, bn::affine_bg_ptr map, Level level);
        private:
            bn::fixed _dy = 0;
            bn::fixed _dx = 0;
            bn::fixed_point _pos;
            bn::camera_ptr _camera;
            bn::affine_bg_ptr _map;
            bn::span<const bn::affine_bg_map_cell> _map_cells;
            bn::optional<bn::span<const bn::affine_bg_map_cell>> map_cells;
            ENEMY_TYPE _type;
            Level _level;
            int _dir;
            int _direction_timer = 0;
            bool _running = false;
            bool _grounded = false;
            bool _falling = false;
            bool _jumping = false;
            bool _wallslide = false;
            bool _jumps = false;
            bool _dead = false;
            bool _invulnerable = false;
            int _invuln_timer = 0;
            int _shoot_cooldown = 0;
            int _shoot_delay = 50;
            int _shoot_circle_delay = 120;

            int _hp;

            bool _is_dashing = false;
            bn::fixed _dash_distance_traveled = 0;
            int _dash_cooldown = 0;
            bn::fixed _dash_speed = 3;
            int _max_dash_distance = 84;

            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<4>> _action;
            bn::fixed_point _target = bn::fixed_point(0,0);

            gm::Hitbox _bullet_hitbox_left = Hitbox(-2,2,2,8);
            gm::Hitbox _bullet_hitbox_right = Hitbox(2,2,2,8);

            //gm::points _points;
            
    };
}

#endif