#include "gm_gun.h"

#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_builder.h"
#include "bn_log.h"
#include "bn_fixed_point.h"

#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_bullet_blow.h"

#include "gm_player_gun_data.h"

namespace gm 
    {

        [[nodiscard]] bn::sprite_animate_action<4> create_bullet_blow(const bn::fixed_point& pos, const bn::camera_ptr& camera)
        {
            bn::sprite_builder builder(bn::sprite_items::bullet_blow);
            builder.set_z_order(3);
            builder.set_position(pos);
            builder.set_camera(camera);

            return bn::create_sprite_animate_action_once(
                builder.release_build(), 4, bn::sprite_items::bullet_blow.tiles_item(), 0, 1, 2);
        }



        Gun::Gun():
        _palette(bn::sprite_items::bullet.palette_item().create_palette()),
        _tiles({ bn::sprite_items::bullet.tiles_item().create_tiles(0),
                bn::sprite_items::bullet.tiles_item().create_tiles(1)})
        {

        }

        void Gun::shoot(const bn::camera_ptr& camera, Player& player)
        {
            BN_LOG("Gun::shoot called");
            spawn_projectile(camera, player);
        }

        void Gun::update_projectiles(const bn::camera_ptr& camera, Player& player, bn::affine_bg_ptr map, Level level)
        {
            
            const int max_distance = 140;
            Player_bullet_data check_data{ bn::fixed_rect(), 0, camera, player};

            for(int i = 0;i < _projectiles.size();)
            {
                Projectile& projectile = _projectiles[i];
                bn::fixed_point pos = projectile._sprite.position() + projectile._velocity;
                projectile._sprite.set_position(pos);
                projectile._distance_traveled += bn::abs(projectile._velocity.x()).integer() + bn::abs(projectile._velocity.y()).integer();
                check_data.bullet_rect = bn::fixed_rect(pos, bn::fixed_size(9, 9));
                check_data.bullet_damage = 1;
                if(projectile._distance_traveled > max_distance )
                {
                    _projectiles.erase(_projectiles.begin() + i);
                    BN_LOG("Gun::update_projectiles Erased");
                }
                else if(player.check_player_bullet(pos))
                {
                    _projectiles.erase(_projectiles.begin() + i);
                    _explosions = create_bullet_blow(pos, camera);
                    _explosions->update();
                }
                else if(player.check_wall_bullet(pos, projectile._velocity, map, level))
                {
                    _projectiles.erase(_projectiles.begin() + i);
                    BN_LOG("Gun::update_projectiles wall hit");
                    _explosions = create_bullet_blow(pos, camera);
                    _explosions->update();
                }
                else
                {
                    ++i;
                    //BN_LOG("Gun::update_projectiles updated");
                }
            }
            if(_explosions)
            {
                _explosions->update();
                if(_explosions->done())
                {
                    _explosions.reset();
                }
            }
        }

        void Gun::spawn_projectile(const bn::camera_ptr& camera, Player& player)
        {
            BN_LOG("Gun::spawn_projectile called");

            if(_projectiles.full())
                return;

            const bn::fixed_point& bullet_position = player.pos();
            bool facing_left = player.facing_left();
            bn::fixed_point bullet_velocity = facing_left ? bn::fixed_point(-2, 0) : bn::fixed_point(2, 0);
            BN_LOG("Spawning bullet at: ", bullet_position.x(), ", ", bullet_position.y());

            bn::sprite_builder builder(bn::sprite_items::bullet.shape_size(),_tiles[0],_palette);
            builder.set_position(bullet_position.x(),bullet_position.y() + 8 );
            builder.set_horizontal_flip(facing_left);
            builder.set_camera(camera);
            BN_LOG("Gun::spawn_projectile end");
            _projectiles.push_back({ builder.release_build(), bullet_velocity });
        }

    }