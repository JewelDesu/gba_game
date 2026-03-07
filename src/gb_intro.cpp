#include "gm_intro.h"

#include "bn_core.h"
#include "bn_regular_bg_position_hbe_ptr.h"
#include "bn_display.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"



#include "bn_regular_bg_items_intro_bg.h"
#include "bn_sprite_items_banana.h"
#include "bn_sprite_items_apple.h"



namespace gm
{
    bn::fixed loop_intro(bn::fixed number, bn::fixed increment)
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

    Intro::Intro ()
    {

    }

    Scene Intro::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::intro_bg.create_bg(0, 0);
        bn::fixed_point init_pos = bn::fixed_point(0, 0);
        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::sprite_ptr banana1 = bn::sprite_items::banana.create_sprite(29, init_pos.y()+45);
        bn::sprite_ptr apple1 = bn::sprite_items::apple.create_sprite(40, init_pos.y()+59);

        banana1.set_horizontal_flip(true);
        apple1.set_horizontal_flip(true);

        bn::sprite_animate_action<6> action1 = bn::create_sprite_animate_action_forever(
                        banana1, 4, bn::sprite_items::banana.tiles_item(), 8,9,4,5,6,7);
        
        bn::sprite_animate_action<6> action2 = bn::create_sprite_animate_action_forever(
                        apple1, 4, bn::sprite_items::apple.tiles_item(), 8,9,4,5,6,7);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);


        banana1.set_camera(camera);
        apple1.set_camera(camera);

        regular_bg.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed layer_2 = 0;  
        bn::fixed layer_3 = 0;
        bn::fixed layer_4 = 0;


        int timer = 0;

        while(!(bn::keypad::any_pressed() && timer > 60))
        {
            ++timer;
            layer_1 = loop_intro(layer_1, 0.2);
            layer_2 = loop_intro(layer_2, 0.3);
            layer_3 = loop_intro(layer_3, 0.5);
            layer_4 = loop_intro(layer_4, 0.1);

            // banana1.set_x(banana1.x() + 1.5);
            // apple1.set_x(apple1.x() + 1.2);


            for(int index = 0, limit = 160; index < limit; ++index)
            {
                if( index > 105)
                {
                    horizontal_deltas[index] = layer_1;
                } 
                else if(index > 90)
                {
                    horizontal_deltas[index] = layer_2;
                }
                else if(index > 0)
                {
                    horizontal_deltas[index] = layer_3;
                }
                else 
                {
                    horizontal_deltas[index] = layer_4;
                }
            }

            horizontal_hbe.reload_deltas_ref();
            action1.update();
            action2.update();
            
            bn::core::update();
        }

        return Scene::TITLE;
    }
} 
