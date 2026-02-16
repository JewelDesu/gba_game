#include "gm_enemy_gun.h"

#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_builder.h"
#include "bn_log.h"
#include "bn_fixed_point.h"


#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_boss_bullet.h"
#include "bn_sprite_items_bullet_blow.h"

#include "gm_bullet_extra.h"
#include "gm_bullet_type.h"


namespace gm 
    {
        constexpr bn::fixed_size bullet_dimensions(6, 6);

        Enemy_Gun::Enemy_Gun():
        _palette(bn::sprite_items::boss_bullet.palette_item().create_palette()),
        _tiles({ bn::sprite_items::boss_bullet.tiles_item().create_tiles(0),
                bn::sprite_items::boss_bullet.tiles_item().create_tiles(1)})
        {

        }
        void Enemy_Gun::enemy_shoot(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, BulletPattern pattern, bn::fixed speed)
        {
            switch(pattern)
            {
                case BulletPattern::DIRECT:
                    enemy_spawn_projectile(camera, player, enemy, speed);
                    break;

                case BulletPattern::SPREAD:
                    spawn_spread(camera, player, enemy, 4, 20, speed);
                    break;

                case BulletPattern::CIRCLE:
                    spawn_circle(camera, enemy, 9, speed);
                    break;

                case BulletPattern::RAIN:
                    spawn_rain(camera, enemy, 6, 1);
                    break;

                case BulletPattern::RANDOM:
                    spawn_random(camera, enemy, 8, 1);
                    break;
            }

           // BN_LOG("Enemy_Gun::enemy_shoot called with pattern: ", int(pattern));
        }

        void Enemy_Gun::enemy_update_projectiles(const bn::camera_ptr& camera, bn::fixed_point player, bn::affine_bg_ptr map, Level level, Enemy& enemy, bool _dead)
        {
            
            const int max_distance = 60;

            for(int i = 0;i < _projectiles.size();)
            {
                Projectile& projectile = _projectiles[i];
                bn::sprite_move_by_action& sprite_move_action = projectile.sprite_move_action;
                bn::fixed_point position = sprite_move_action.sprite().position();
                //projectile.sprite_move_action.set_position(position);

                bn::fixed_rect player_rect(player, bn::fixed_size(16, 16));
                bn::fixed_rect bullet_rect(position, bn::fixed_size(8, 8));

                projectile._distance_traveled += bn::abs(projectile._velocity.x()).integer() + bn::abs(projectile._velocity.y()).integer();
                if(projectile._distance_traveled > max_distance )
                {
                    _projectiles.erase(_projectiles.begin() + i);
                   // BN_LOG("Gun::update_projectiles Erased");
                }
                else if(enemy.check_wall_bullet(position, projectile._velocity, map, level))
                {
                    _projectiles.erase(_projectiles.begin() + i);
                   // BN_LOG("Gun::update_projectiles wall hit");
                }
                else if(_dead)
                {
                    _projectiles.erase(_projectiles.begin() + i);
                }
                else
                {
                    sprite_move_action.update();
                    
                    ++i;
                    //BN_LOG("Gun::update_projectiles updated");
                }
            }
        }

        void Enemy_Gun::enemy_spawn_projectile(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy ,bn::fixed speed)
        {
           // BN_LOG("Enemy_Gun::enemy_spawn_projectile");
           // BN_LOG("Enemy position: ", enemy.x(), ", ", enemy.y());
           // BN_LOG("Player position: ", player.x(), ", ", player.y());

            if(_projectiles.full())
                return;
            
            bn::fixed_point bullet_velocity = bn::fixed_point(1, 0);

            bn::sprite_builder builder(bn::sprite_items::bullet.shape_size(),_tiles[0],_palette);
            builder.set_position(enemy);
            builder.set_camera(camera);
            builder.set_bg_priority(1);
            //BN_LOG("Gun::spawn_projectile end");
            bn::fixed_point distance = player - enemy;
            bn::fixed_point delta_position = direction_vector(distance.x(), distance.y(), 1);
            //bn::fixed speed = 1;
            bn::fixed_point velocity = delta_position * speed;
            _projectiles.push_back({ bn::sprite_move_by_action(builder.release_build(), velocity),bullet_velocity });
            //BN_LOG("Enemy bullet spawned, count: ", _projectiles.size());
        }

        bool Enemy_Gun::check_player(const bn::fixed_rect& player_rect)
        {
            for(int i = 0;i < _projectiles.size(); ++i)
            {
                Projectile& projectile = _projectiles[i];

                const bn::fixed_point& bullet_position = projectile.sprite_move_action.sprite().position();
                bn::fixed_rect bullet_rect(bullet_position, bullet_dimensions);

                if(bullet_rect.intersects(player_rect))
                {
                    return true;
                }
                
            }

            return false;
        }

        void Enemy_Gun::spawn_single(const bn::camera_ptr& camera, bn::fixed_point& enemy, const bn::fixed_point& direction)
        {
            if(_projectiles.full())
                return;

            bn::sprite_builder builder(bn::sprite_items::bullet.shape_size(), _tiles[0], _palette);
            builder.set_position(enemy);
            builder.set_camera(camera);

            bn::fixed_point distance = enemy;
            bn::fixed_point delta_position = direction_vector(distance.x(), distance.y(), 1);
            bn::fixed speed = 1;
            bn::fixed_point bullet_velocity = direction * speed;
            
            _projectiles.push_back({ bn::sprite_move_by_action(builder.release_build(), bullet_velocity), direction});
        }

        void Enemy_Gun::spawn_spread(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, int count, bn::fixed angle_step, bn::fixed speed)
        {
            bn::fixed_point base_dir = direction_vector(player.x() - enemy.x(), player.y() - enemy.y(), 1);
            bn::fixed_point distance = player - enemy;
            bn::fixed_point bullet_velocity = bn::fixed_point(1, 0);
            bn::fixed base_angle = bn::degrees_atan2(distance.y().integer(), distance.x().integer());
            int half = count / 2;
            for(int i = -half; i <= half; ++i)
            {
                bn::fixed angle = base_angle + angle_step * i;
                bn::fixed_point dir(bn::degrees_cos(angle), bn::degrees_sin(angle));
                //bn::fixed speed = 0.6;
                bn::fixed_point velocity = dir * speed;
                
                bn::sprite_builder builder(bn::sprite_items::boss_bullet.shape_size(), _tiles[0], _palette);
                builder.set_position(enemy);
                builder.set_camera(camera);
                builder.set_bg_priority(1);
                _projectiles.push_back({ bn::sprite_move_by_action(builder.release_build(), velocity), bullet_velocity });
            }
        }

        void Enemy_Gun::spawn_circle(const bn::camera_ptr& camera, bn::fixed_point& enemy, int count, bn::fixed speed)
        {
            if(_projectiles.full())
                return;

            bn::fixed step = -210 / count;
            bn::fixed_point bullet_velocity = bn::fixed_point(1, 0);

            for(int i = 0; i < count; i++)
            {
                bn::fixed angle = i * step;

                bn::fixed_point dir(bn::degrees_cos(angle), bn::degrees_sin(angle));
                //bn::fixed speed = 0.8;
                bn::fixed_point velocity = dir * speed;

                bn::sprite_builder builder(bn::sprite_items::bullet.shape_size(), _tiles[0], _palette);
                builder.set_position(enemy);
                builder.set_camera(camera);
                builder.set_bg_priority(1);
                _projectiles.push_back({ bn::sprite_move_by_action(builder.release_build(), velocity), bullet_velocity });
            }
        }

        void Enemy_Gun::spawn_rain(const bn::camera_ptr& camera, bn::fixed_point& enemy, int count, bn::fixed speed)
        {
            for(int i = 0; i < count; ++i)
            {
                bn::fixed offset_x = (i - count / 2) * 16;
                bn::fixed_point start(enemy.x() + offset_x, enemy.y() - 30);
                bn::fixed_point dir(0, speed);
                spawn_single(camera, start, dir);
            }
        }

        void Enemy_Gun::spawn_random(const bn::camera_ptr& camera, bn::fixed_point& enemy, int count, bn::fixed speed)
        {

            for(int i = 0; i < count; ++i)
            {
                int pi = 3.14159;
                int angle_raw = _random.get() % 360;
                bn::fixed angle = bn::fixed(angle_raw) * (pi / 180);
                bn::fixed_point dir(bn::degrees_cos(angle) * speed, bn::degrees_sin(angle) * speed);
                spawn_single(camera, enemy, dir);
            }
        }

        // void Enemy_gun::dash(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy)
        // {
        //     bool position_status = false;
        //     bool right = !Enemy::side();
        //     int x = 9;

        //     while(!position_status)
        //     {
        //         if(right)
        //         {
        //             position_status = Enemy::_fall_check(enemy.x() + x*8, enemy.y());
        //         }
        //         else
        //         {
        //             position_status = Enemy::_fall_check(enemy.x() - x*8, enemy.y());
        //         }
        //         if(!position_status)
        //         {
        //             --x;
        //             if(x < 9)
        //             {
        //                 position_status = true;
        //             }
        //         }
        //     }

        //     if(right)
        //     {
        //         enemy.set_x(enemy.x() + x*8)
        //     }
        //     else 
        //     {
        //         enemy.set_x(enemy.x() -  x*8)
        //     }
        // }
    }