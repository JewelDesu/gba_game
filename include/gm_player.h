#ifndef GM_PLAYER_H
#define GM_PLAYER_H

#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_cell.h"
#include "bn_sprite_animate_actions.h"
#include "bn_span.h"
#include "bn_sprite_palette_actions.h"
#include "bn_fixed_rect.h"
#include "bn_regular_bg_ptr.h"

#include "gm_level.h"
#include "gm_hitbox.h"
#include "gm_enemy.h"
#include "gm_player_stats.h"
#include "gm_gun.h"
#include "gm_enemy_gun.h"
#include "gm_globals.h"
#include "gm_coin.h"
#include "gm_player_type.h"


#include "bn_sprite_items_banana.h"
#include "bn_sprite_items_apple.h"
namespace gm
{
    class Gun;
    class Enemy_gun;
    class Player
    {
        public:
            Player(bn::sprite_ptr sprite, PLAYER_TYPE type);

            [[nodiscard]] bn::fixed_point pos();
            int hp();
            void set_hp(int hp);
            void set_healthbar_visibility(bool is_visible);
            int points();
            void set_points_visibility(bool is_visable);
            void set_point(int point);
            void attack();
            void move_left();
            bool facing_left();
            void move_right();
            void move_down(bn::affine_bg_ptr map, gm::Level level);
            void jump(bn::affine_bg_ptr map, gm::Level level);
            void wall_jump(bn::affine_bg_ptr map, gm::Level level);
            void attacking();
            void spawn_projectile(const bn::camera_ptr& camera, Player player);
            void update(const bn::camera_ptr& camera, Player player, bn::vector<Enemy,16>& enemies);
            void apply_animation_state();
            void apply_animation_state_banana();
            void apply_animation_state_apple();
            void collide_with_objects(bn::affine_bg_ptr map, gm::Level level);
            void colide_with_enemy(Enemy_Gun& enemy_gun,bn::fixed_rect new_body_rect);
            void update_pos(bn::affine_bg_ptr map, gm::Level level, Gun& gun, const bn::camera_ptr& camera, bn::vector<Enemy,16>& enemies, Enemy_Gun& enemy_gun);
            void spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map, bn::vector<Enemy,16>& enemies, bn::vector<Coin,16>& coins);
            void reset();
            void set_dialog(bool is_in_dialog);
            bool is_in_dialog();
            void hide();
            void delete_data();
            bool check_player_bullet(bn::fixed_point pos);
            bool check_wall_bullet(bn::fixed_point pos, bn::fixed_point velocity, bn::affine_bg_ptr map, Level level);
            void collide_with_coin(bn::fixed_rect new_body_rect);
            void show_shoot(bn::color fade_color);
            void shoot();
            bool in_menu();
            Scene to_menu();
            void death();
            void set_player(PLAYER_TYPE type);

        private:
            bn::sprite_ptr _sprite;
            bn::fixed _dx;
            bn::fixed _dy;
            bn::fixed_point _pos;
            bn::optional<bn::camera_ptr> _camera;
            bn::optional<bn::affine_bg_ptr> _map;
            bn::optional<bn::span<const bn::affine_bg_map_cell>> _map_cells;
            bn::optional<bn::vector<Enemy,16>*> _enemies;
            bn::regular_bg_ptr _dialogbox;
            bn::optional<bn::vector<Coin,16>*> _coins;
            
            gm::Hitbox _hitbox_fall = Hitbox(0,16,16,0);
            gm::Hitbox _hitbox_left = Hitbox(-4,0,4,16);
            gm::Hitbox _hitbox_right = Hitbox(4,0,6,16);
            gm::Hitbox _hitbox_jump = Hitbox(0,8,12,8);
            gm::Healthbar _healthbar;
            gm::Hitbox _bullet_hitbox_left = Hitbox(-2,0,0,8);
            gm::Hitbox _bullet_hitbox_right = Hitbox(2,0,0,8);
            gm::points _points;

            bool _running = false;
            bool _grounded = false;
            bool _falling = false;
            bool _jumping = false;
            bool _wallslide = false;
            bool _jumps = false;
            bool _attacking = false;
            bool _invulnerable = false;
            int _invuln_timer = 0;
            bool _facing_left = false;
            bool _shooting = false;
            int _shoot_cooldown = 0;
            int _shoot_delay = 15;
            int _melee = 0;
            bool _weapon_swap = false;
            bool _in_menu = false;
            bool _in_dialog = false;
            Weapon _weapon;
            PLAYER_TYPE _type;

            int _show_shoot_counter = 0;
            bn::optional<bn::sprite_palette_fade_to_action> _weapon_palette_fade_action;

            bn::optional<bn::sprite_animate_action<10>> _explosion;
            bn::optional<bn::sprite_ptr> _explosion_sprite;
            bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 30, bn::sprite_items::banana.tiles_item(), 0,1,2,3,0,1,2,3,0,1);
            bn::sprite_animate_action<10> _action_apple = bn::create_sprite_animate_action_forever(
                        _sprite, 30, bn::sprite_items::apple.tiles_item(), 0,1,2,3,0,1,2,3,0,1);
            bn::optional<bn::sprite_animate_action<4>> _attack_action;

            void _update_camera(int lerp);
    };
}

#endif