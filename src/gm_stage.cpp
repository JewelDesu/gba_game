#include "gm_stage.h"


#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_unique_ptr.h"
#include "bn_math.h"


#include "gm_level.h"
#include "gm_gun.h"
#include "gm_enemy_gun.h"

#include "bn_affine_bg_items_stage.h"
#include "bn_affine_bg_items_test.h"
#include "bn_regular_bg_items_grasslands.h"
#include "bn_regular_bg_items_cloud_cover_1.h"

namespace gm
{
    Stage::Stage(Player& player):
    _player(&player)
    {

    }

    Scene Stage::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());


        bn::affine_bg_ptr map = bn::affine_bg_items::stage.create_bg(512, 512);
        //bn::affine_bg_ptr map = bn::affine_bg_items::test.create_bg(512, 512);
        
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::grasslands.create_bg(512, 512);
        bn::regular_bg_ptr map_bg2 = bn::regular_bg_items::cloud_cover_1.create_bg(512, 512);
        map_bg.set_priority(3);
        map_bg2.set_priority(2);
        map.set_priority(1);
        

        gm::Level level = gm::Level(map);

        //map_bg.set_camera(camera);
        //map_bg2.set_camera(camera);
        map.set_camera(camera);
        

        bn::vector<Enemy, 16> enemies = {};
        enemies.push_back(Enemy(camera, map, 523, 351, ENEMY_TYPE::GOOMBA, 5));
        enemies.push_back(Enemy(camera, map, 216, 367, ENEMY_TYPE::SLIME, 5));
        enemies.push_back(Enemy(camera, map, 870, 990, ENEMY_TYPE::GOOMBA_BOSS, 5));


        _player->spawn(spawn_location, camera, map, enemies);
        _player->set_healthbar_visibility(true);
        Gun bullets;
        

        while(true)
        {
            for(Enemy& enemy : enemies)
            {
                if(bn::abs(enemy.pos().x() - camera.x()) < 200 && bn::abs(enemy.pos().y() - camera.y()) < 100)
                {
                    enemy.update_pos_enemy(_player->pos(), enemy_bullets);
                }
                else 
                {
                    enemy.set_visible(false);
                }
            }

            map_bg2.set_x(map_bg2.x() - 0.5);

            _player->update_pos(map, level, bullets, camera, enemies,enemy_bullets);
            _player->apply_animation_state();
            //bullets.shoot(camera, *_player, enemies);
            bn::core::update();
        }
    }
}