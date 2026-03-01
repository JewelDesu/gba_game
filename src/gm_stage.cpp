#include "gm_stage.h"


#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_unique_ptr.h"
#include "bn_math.h"
#include "bn_keypad.h"
#include "bn_log.h"


#include "gm_level.h"
#include "gm_gun.h"
#include "gm_enemy_gun.h"
#include "gm_npc.h"
#include "gm_npc_type.h"

#include "bn_affine_bg_items_stage.h"
#include "bn_affine_bg_items_water2.h"
#include "bn_regular_bg_items_grasslands.h"
#include "bn_regular_bg_items_cloud_cover_1.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "bn_sound_items.h"
#include "gm_small_sprite_font.h"


namespace gm
{
    Stage::Stage(Player& player):
    _player(&player)
    {

    }

    Scene Stage::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());
        bn::music_items::haplev.play();
        bn::music::set_volume(0.3);

        bn::sprite_text_generator text_generator(small_sprite_font);

        bn::affine_bg_ptr map = bn::affine_bg_items::water2.create_bg(512, 512);
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::grasslands.create_bg(512, 512);
        map_bg.set_priority(3);
        map.set_priority(2);
        gm::Level level = gm::Level(map);

        map.set_camera(camera);

        Npc applecat = Npc(bn::fixed_point(270, 383), camera, NPC_TYPE::APPLECAT, text_generator);

        bn::unique_ptr<bn::vector<Enemy, 16>> enemies(new bn::vector<Enemy, 16>());
        enemies -> push_back(Enemy(camera, map, 216, 367, ENEMY_TYPE::SLIME, 5));
        enemies -> push_back(Enemy(camera, map, 642, 268, ENEMY_TYPE::SLIME, 5));
        enemies -> push_back(Enemy(camera, map, 425, 599, ENEMY_TYPE::SLIME, 5));
        enemies -> push_back(Enemy(camera, map, 277, 671, ENEMY_TYPE::SLIME, 5));
        enemies -> push_back(Enemy(camera, map, 230, 791, ENEMY_TYPE::SLIME, 5));
        enemies -> push_back(Enemy(camera, map, 446, 911, ENEMY_TYPE::SLIME, 5));

        enemies -> push_back(Enemy(camera, map, 523, 351, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 868, 351, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 729, 638, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 293, 711, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 490, 831, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 240, 927, ENEMY_TYPE::GOOMBA, 5));
        enemies -> push_back(Enemy(camera, map, 903, 159, ENEMY_TYPE::GOOMBA, 5));

        //enemies->push_back(Enemy(camera, map, 870, 990, ENEMY_TYPE::GOOMBA_BOSS, 5));
        enemies->push_back(Enemy(camera, map, 870, 960, ENEMY_TYPE::GOOMBA_BOSS, 5));


        Enemy& boss = (*enemies)[13];


        bn::vector<Coin, 16> coins = {};
        coins.push_back(Coin(camera, 250, 389, COIN_TYPE::BANANA));
        coins.push_back(Coin(camera, 898, 253, COIN_TYPE::BANANA));
        coins.push_back(Coin(camera, 246, 621, COIN_TYPE::BANANA));
        coins.push_back(Coin(camera, 663, 965, COIN_TYPE::BANANA));

        coins.push_back(Coin(camera, 250, 287, COIN_TYPE::BANANA_GEM));
        coins.push_back(Coin(camera, 590, 831, COIN_TYPE::BANANA_GEM));
        coins.push_back(Coin(camera, 79, 927, COIN_TYPE::BANANA_GEM));
        coins.push_back(Coin(camera, 750, 151, COIN_TYPE::BANANA_GEM));

        _player -> spawn(spawn_location, camera, map, *enemies, coins);
        _player -> set_healthbar_visibility(true);
        //_player -> set_points_visibility(true);
        Gun bullets;
        globals::savegame.stage_name = Scene::STAGE;


        while(true)
        {
            if(_player -> hp() < 1)
            {
                //bn::sound_items::banana_cat_cry2.play();
                bn::sound_items::death_pollo.play();
                _player->death();
                return Scene::DEATH;
            }
            
            if(gm::globals::menu)
            {
                _player->delete_data();
                gm::globals::menu = false;
                return Scene::TITLE;
            }
            
            for(Enemy& enemy : *enemies)
            {
                enemy.update_cooldowns();
                if(bn::abs(enemy.pos().x() - camera.x()) < 210 && bn::abs(enemy.pos().y() - camera.y()) < 120)
                {
                    enemy.update_pos_enemy(_player->pos(), enemy_bullets);
                }
                else 
                {
                    enemy.set_visible(false);
                }
            }

            for(Coin& coin : coins)
            {
                coin.update();
            }

            if(applecat.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed())
                {
                    _player -> set_dialog(true);
                    applecat.talk();
                }
                else if(!applecat.is_talking())
                {
                    _player -> set_dialog(false);
                }
            } 
            else 
            {
                _player -> set_dialog(false);
            }

            applecat.update();

            //map_bg2.value().set_x(map_bg2.value().x() - 0.5);

            _player -> update_pos(map, level, bullets, camera, *enemies,enemy_bullets);
            _player -> apply_animation_state();
            //bullets.shoot(camera, *_player, enemies);

            if(boss.hp() < 1){
                globals::savegame.health = _player->hp();
                globals::savegame.points = _player->points();
                globals::savegame._pos = bn::fixed_point(50, 943);

                _player -> delete_data();
                _player -> set_points_visibility(false);
                //_player -> set_player(gm::PLAYER_TYPE::APPLE_CAT);

                return Scene::MINNIE;
            }
            
            bn::core::update();
        }
    }
}