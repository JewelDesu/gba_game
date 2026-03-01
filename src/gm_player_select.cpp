#include "gm_player_select.h"

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
#include "bn_sprite_animate_actions.h"
#include "bn_sram.h"
#include "bn_vector.h"
#include "bn_audio.h"

#include "gm_scene.h"
#include "gm_savegame.h"
#include "gm_player_stats.h"
#include "gm_globals.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_sprite_items_apple.h"
#include "bn_sprite_items_banana.h"
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"
#include "bn_sprite_items_cursor.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"


namespace gm
{

    bn::fixed loop2(bn::fixed number, bn::fixed increment)
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

    Player_select::Player_select (Player& player, bn::sprite_text_generator& text_generator): 
        _player(&player),
        _text_generator(&text_generator)
    {

    }

    Scene Player_select::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);
        //bn::regular_bg_ptr regular_bg = bn::regular_bg_items::house_bg.create_bg(0, 0);


        //bn::sprite_ptr apple_cat = bn::sprite_items::apple.create_sprite(init_pos1.x(), init_pos1.y());
        //bn::sprite_ptr banana_cat = bn::sprite_items::banana.create_sprite(init_pos2.x(), init_pos2.y());

        bn::fixed_point init_pos1 = bn::fixed_point(-45, 0);
        bn::fixed_point init_pos2 = bn::fixed_point(45, 0);

        bn::sprite_ptr banana1 = bn::sprite_items::banana.create_sprite(init_pos1.x(), init_pos1.y()+35);
        banana1.set_horizontal_scale(2);
        banana1.set_vertical_scale(2);

        bn::sprite_ptr apple1 = bn::sprite_items::apple.create_sprite(init_pos2.x(), init_pos2.y()+35);
        apple1.set_horizontal_scale(2);
        apple1.set_vertical_scale(2);


        bn::sprite_animate_action<10> _action_banana = bn::create_sprite_animate_action_forever(
                banana1, 10, bn::sprite_items::banana.tiles_item(), 0,1,2,3,0,1,2,3,0,1);
        bn::sprite_animate_action<10> _action_apple = bn::create_sprite_animate_action_forever(
                apple1, 10, bn::sprite_items::apple.tiles_item(), 0,1,2,3,0,1,2,3,0,1);

        bn::music_items::happy_banana_ii.play();

        _text_generator->set_left_alignment();
        _text_generator->generate(-38, -15, "CAT SELECT", labels);




        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);

        
        banana1.set_camera(camera);
        apple1.set_camera(camera);
        regular_bg.set_camera(camera);

        int timer = 0;
        bn::fixed start_y = 50;
        bn::fixed start_x = -70;
        bn::fixed start_amp = 0;


        gm::gui gui;
        bool gamestart = false;


        bn::vector<bn::string_view,4> options;

        options.push_back("BANANA CAT");
        options.push_back("APPLE CAT");

        _text_generator->generate(-75 , 15, options[0], labels);
        _text_generator->generate(15 , 15, options[1], labels);
    

        bn::optional<bn::sprite_ptr> cursor = options.size() > 0 ? bn::optional<bn::sprite_ptr>(
            bn::sprite_items::cursor.create_sprite(bn::fixed_point(-70, 50))): bn::optional<bn::sprite_ptr>();

        cursor->set_horizontal_scale(2);
        cursor->set_vertical_scale(2);

            
        if(cursor)
        {
            cursor->set_bg_priority(1);
        }

        int menu_ind = 0;
        int menu_var = 0;

        while(!(gamestart && timer > 60))
        {
            ++timer;

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            _action_banana.update();
            _action_apple.update();

            for(bn::sprite_ptr& sprite : labels)
            {
                sprite.set_visible(true);
            }

            horizontal_hbe.reload_deltas_ref();

            cursor->set_x(start_x + bn::degrees_lut_sin(start_amp*1)*2);

            if(cursor)
            {
                if(bn::keypad::right_pressed())
                {
                    menu_ind++;
                    menu_var = 1;
                    if(menu_ind >= options.size())
                    {
                        menu_ind = 0;
                        menu_var = -1;
                    }
                    if(menu_ind == 1)
                    {
                        start_x = 23 * menu_var + menu_ind * (-8);
                        cursor->set_position(start_x, cursor->y());
                    }
                    else
                        start_x = 70 * menu_var + menu_ind * (-8);
                        cursor->set_position(start_x, cursor->y());
                    BN_LOG("right_pressed: ", menu_ind, "  ", menu_var);
                    BN_LOG("set_position: ", cursor->x(), "  ",  cursor->y());
                }
                if(bn::keypad::left_pressed())
                {
                    menu_ind--;
                    menu_var = 1;
                    if(menu_ind < 0)
                    {
                        menu_ind = options.size()-1;
                        menu_var = -1;
                    }
                    if(menu_ind == 1)
                    {
                        start_x = -7 * menu_var + menu_ind * (8);
                        cursor->set_position(start_x, cursor->y());
                    }
                    else
                        start_x = -70 * menu_var + menu_ind * (-8);
                        cursor->set_position(start_x, cursor->y());
                    BN_LOG("left_pressed: ", menu_ind);
                    BN_LOG("set_position: ", cursor->x(), "  ",  cursor->y());
                }
            }
            if(bn::keypad::a_pressed())
            {
                if(menu_ind == 0)
                {
                    _player -> set_player(globals::savegame.player_type);
                    gm::globals::savegame = gm::savegame();
                    gamestart = true;
                    //BN_LOG("Game loaded with player position: ", gm::savegame()._pos().y());
                }
                else if(menu_ind == 1)
                {
                    globals::savegame.player_type = PLAYER_TYPE::APPLE_CAT;
                    _player -> set_player(globals::savegame.player_type);
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

        return Scene::STAGE;

        
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