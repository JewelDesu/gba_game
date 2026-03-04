#include "gm_intro.h"

#include "bn_core.h"
#include "bn_regular_bg_position_hbe_ptr.h"
#include "bn_display.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_math.h"


#include "bn_regular_bg_items_intro_bg.h"



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


        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);

        regular_bg.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed layer_2 = 0;  
        bn::fixed layer_3 = 0;

        int timer = 0;

        while(!(bn::keypad::any_pressed() && timer > 60))
        {
            ++timer;
            layer_1 = loop_intro(layer_1, 0.2);
            layer_2 = loop_intro(layer_2, 0.3);
            layer_3 = loop_intro(layer_3, 0.5);


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
                    horizontal_deltas[index] = 0;
                }
            }

            horizontal_hbe.reload_deltas_ref();
            
            bn::core::update();
        }

        return Scene::TITLE;
    }
} 
