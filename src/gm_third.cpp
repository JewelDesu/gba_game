#include "gm_third.h"


#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_unique_ptr.h"
#include "bn_math.h"
#include "bn_keypad.h"

#include "gm_level.h"
#include "gm_gun.h"
#include "gm_enemy_gun.h"
#include "gm_npc.h"
#include "gm_npc_type.h"

#include "bn_affine_bg_items_thirdstage.h"
#include "bn_regular_bg_items_grasslands.h"
#include "bn_regular_bg_items_cloud_cover_1.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "bn_sound_items.h"
#include "gm_small_sprite_font.h"


namespace gm
{
    Third::Third(Player& player):
    _player(&player)
    {

    }

    Scene Third::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());
        bn::music_items::cats_at_play.play();
        bn::music::set_volume(0.3);

        bn::sprite_text_generator text_generator(small_sprite_font);

        bn::affine_bg_ptr map = bn::affine_bg_items::thirdstage.create_bg(512, 512); 
        //bn::regular_bg_ptr map_bg = bn::regular_bg_items::grasslands.create_bg(512, 512);
        //map_bg.set_priority(3);
        map.set_priority(2);
        gm::Level level = gm::Level(map);

        map.set_camera(camera);

        bn::unique_ptr<bn::vector<Enemy, 16>> enemies(new bn::vector<Enemy, 16>());
        enemies->push_back(Enemy(camera, map, 447, 519, ENEMY_TYPE::GLORP_CRAFT, 25));


        bn::vector<Coin, 16> coins = {};
        coins.push_back(Coin(camera, 333, 599, COIN_TYPE::BANANA));


        _player -> spawn(spawn_location, camera, map, *enemies, coins);
        _player -> set_healthbar_visibility(true);
        //_player -> set_points_visibility(true);
        Gun bullets;
        globals::savegame.stage_name = Scene::THIRD;

        while(true)
        {
            if(_player -> hp() < 1)
            {
                //bn::sound_items::banana_cat_cry2.play();
                bn::sound_items::death_pollo.play();
                _player -> death();
                return Scene::DEATH;
            }
            if(gm::globals::menu)
            {
                _player -> delete_data();
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

            if(_player -> pos().y() > 1000)
            {
                _player -> set_hp(0);
            }

            _player -> update_pos(map, level, bullets, camera, *enemies,enemy_bullets);
            _player -> apply_animation_state();
            
            bn::core::update();
        }
    }
}