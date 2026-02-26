#include "gm_title.h"

#include "bn_core.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_display.h"
#include "bn_vector.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_regular_bg_position_hbe_ptr.h"
#include "bn_affine_bg_attributes_hbe_ptr.h"
#include "bn_sram.h"
#include "bn_vector.h"
#include "bn_audio.h"
#include "bn_optional.h"

#include "gm_scene.h"
#include "gm_savegame.h"
#include "gm_player_stats.h"
#include "gm_globals.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_sprite_items_apple_cat.h"
#include "bn_sprite_items_banana_cat.h"
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"
#include "bn_sprite_items_cursor.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"


namespace gm
{

    bn::fixed loop(bn::fixed number, bn::fixed increment)
    {
        number += increment;
        if(number > 256)
        {
            return 0;
        }
        else 
        {
            return number;
        }
    }

    Title::Title (bn::sprite_text_generator& text_generator)
        : _text_generator(&text_generator)
    {

    }

    Scene Title::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);
        //bn::regular_bg_ptr regular_bg = bn::regular_bg_items::house_bg.create_bg(0, 0);

        bn::fixed_point init_pos1 = bn::fixed_point(-70, 44);
        bn::fixed_point init_pos2 = bn::fixed_point(70, 44);

        bn::optional<bn::sprite_ptr> apple_cat = bn::sprite_items::apple_cat.create_sprite(init_pos1.x(), init_pos1.y());
        bn::optional<bn::sprite_ptr> banana_cat = bn::sprite_items::banana_cat.create_sprite(init_pos2.x(), init_pos2.y());

        
        bn::optional<bn::sprite_ptr> title1 = bn::sprite_items::title_1.create_sprite(-31,-30);
        bn::optional<bn::sprite_ptr> title2 = bn::sprite_items::title_2.create_sprite(31,-30);

        bn::music_items::happy_banana_ii.play();

        //_text_generator->set_left_alignment();
        //_text_generator->generate(-38, 55, "PRESS START", labels);




        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);

        
        apple_cat -> set_camera(camera);
        banana_cat -> set_camera(camera);
        regular_bg.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed start_y = -30;
        bn::fixed start_x = -38 + 2;
        bn::fixed start_amp = 0;

        int timer = 0;

        gm::savegame load_save;
        bn::sram::read(load_save);

        bool valid_save = load_save.valid();
        gm::gui gui;
        bool gamestart = false;


        bn::vector<bn::string_view,4> options;
        if(valid_save)
         {
            options.push_back("Load save");
        }
         options.push_back("Start new game");

        for(int i = 0, limit = options.size(); i < limit; i++)
        {
            _text_generator->generate(-38 + 8, 55 + 10 * i, options[i], labels);
        }

        bn::optional<bn::sprite_ptr> cursor = options.size() > 0 ? bn::optional<bn::sprite_ptr>(
            bn::sprite_items::cursor.create_sprite(bn::fixed_point(-38 + 2, 55))): bn::optional<bn::sprite_ptr>();
        if(cursor)
        {
            cursor->set_bg_priority(1);
        }

        int menu_ind = 0;

        while(!(gamestart && timer > 60))
        {
            ++timer;
            layer_1 = loop(layer_1, 0.5);

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            title1 -> set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);
            title2 -> set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);

            for(bn::sprite_ptr& sprite : labels)
            {
                sprite.set_visible(true);
            }

            for(int index = 0, limit = 160; index < limit; ++index)
            {
                horizontal_deltas[index] = layer_1;
            }

            horizontal_hbe.reload_deltas_ref();

            cursor->set_x(start_x + bn::degrees_lut_sin(start_amp*1)*2);

            if(cursor)
            {
                if(bn::keypad::down_pressed())
                {
                    menu_ind++;
                    if(menu_ind >= options.size())
                    {
                        menu_ind = 0;
                    }
                    cursor->set_position(cursor->x(), 55 + menu_ind * 10);
                    BN_LOG("down_pressed: ", menu_ind);
                }
                if(bn::keypad::up_pressed())
                {
                    menu_ind--;
                    if(menu_ind < 0)
                    {
                        menu_ind = options.size()-1;
                    }
                    cursor->set_position(cursor->x(), 55 + menu_ind * 10);
                    BN_LOG("up_pressed: ", menu_ind);
                }
            }
            if(bn::keypad::a_pressed())
            {
                if(menu_ind == 0 && valid_save)
                {
                    gm::globals::savegame = load_save;
                    gamestart = true;
                    //BN_LOG("Game loaded with player position: ", gm::savegame()._pos().y());
                }
                else if(menu_ind == 1)
                {
                    gm::globals::savegame = gm::savegame();
                    gamestart = true;
                }
                else
                {
                    gamestart = true;
                }
            }

            bn::core::update();
        }

        bn::music_volume_to_action(60, 0);

        return globals::savegame.stage_name;

        
        // while(!(bn::keypad::any_pressed() && timer > 60))
        // {

        //     ++timer;
        //     layer_1 = loop(layer_1, 0.5);

        //     start_amp += 6;
        //     if(start_amp >= 360){
        //         start_amp = 0;
        //     }

        //     title1.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);
        //     title2.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);

        //     for(bn::sprite_ptr& sprite : labels)
        //     {
        //         sprite.set_visible(true);
        //     }

        //     for(int index = 0, limit = 160; index < limit; ++index)
        //     {
        //         horizontal_deltas[index] = layer_1;
        //     }

        //     horizontal_hbe.reload_deltas_ref();

        //     //_action.update();
            
        //     bn::core::update();
        // }

        // return Scene::STAGE;
        
        //bg.set_blending_enabled(true);
    }
    
    
}




            // bn::vector<bn::string_view,4> options;
            // if(valid_save)
            // {
            //     options.push_back("Load save");
            // }
            // options.push_back("Start new game");

            // int option = gui.textbox(" ", options, -38, 55, false);
            
            // if(option == 0 && valid_save)
            // {
            //     gm::globals::savegame = load_save;
            //     gamestart = true;
            //     //BN_LOG("Game loaded with player position: ", gm::savegame()._pos().y());
            // }
            // else if(option == 1)
            // {
            //     options.clear();
            //     options.push_back("No");
            //     options.push_back("Yes");
            //     int option2 = gui.textbox("Starting new game.", options, -38, 55, false);

            //     if(option2 == 1)
            //     {
            //         gm::globals::savegame = gm::savegame();
            //         gamestart = true;
            //     }
            // }
            // else
            // {
            //     gamestart = true;
            // }