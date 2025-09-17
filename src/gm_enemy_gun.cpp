#include "gm_enemy_gun.h"

#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_builder.h"
#include "bn_log.h"
#include "bn_fixed_point.h"

#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_bullet_blow.h"

#include "gm_bullet_extra.h"


namespace gm 
    {
        constexpr bn::fixed_size bullet_dimensions(6, 6);

        Enemy_Gun::Enemy_Gun():
        _palette(bn::sprite_items::bullet.palette_item().create_palette()),
        _tiles({ bn::sprite_items::bullet.tiles_item().create_tiles(0),
                bn::sprite_items::bullet.tiles_item().create_tiles(1)})
        {

        }

        void Enemy_Gun::enemy_shoot(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, const bn::fixed_point& delta_position)
        {
            enemy_spawn_projectile(camera, player, enemy, delta_position);
            BN_LOG("Enemy_Gun::enemy_shoot called");
        }

        void Enemy_Gun::enemy_update_projectiles(const bn::camera_ptr& camera, bn::fixed_point player, bn::affine_bg_ptr map, Level level, Enemy& enemy, bool _dead)
        {
            
            const int max_distance = 90;

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
                    BN_LOG("Gun::update_projectiles Erased");
                }
                else if(enemy.check_wall_bullet(position, projectile._velocity, map, level))
                {
                    _projectiles.erase(_projectiles.begin() + i);
                    BN_LOG("Gun::update_projectiles wall hit");
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

        void Enemy_Gun::enemy_spawn_projectile(const bn::camera_ptr& camera, bn::fixed_point& player, bn::fixed_point& enemy, const bn::fixed_point& delta_position)
        {
            BN_LOG("Enemy_Gun::enemy_spawn_projectile");
            BN_LOG("Enemy position: ", enemy.x(), ", ", enemy.y());
            BN_LOG("Player position: ", player.x(), ", ", player.y());

            if(_projectiles.full())
                return;

            bn::fixed_point bullet_velocity = bn::fixed_point(1, 0);

            bn::sprite_builder builder(bn::sprite_items::bullet.shape_size(),_tiles[0],_palette);
            builder.set_position(enemy);
            builder.set_camera(camera);
            BN_LOG("Gun::spawn_projectile end");
            bn::fixed_point distance = player - enemy;
            bn::fixed_point delta_position = direction_vector(distance.x(), distance.y(), 1);
            _projectiles.push_back({ bn::sprite_move_by_action(builder.release_build(), delta_position),bullet_velocity });
            BN_LOG("Enemy bullet spawned, count: ", _projectiles.size());
        }

        bool Enemy_Gun::check_hero(const bn::fixed_rect& player_rect)
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
    }