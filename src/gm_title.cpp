#include "gm_title.h"

#include "bn_core.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_regular_bg_position_hbe_ptr.h"
#include "bn_affine_bg_attributes_hbe_ptr.h"

#include "gm_scene.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_sprite_items_apple_cat.h"
#include "bn_sprite_items_title_start_1.h"
#include "bn_sprite_items_title_start_2.h"
#include "bn_sprite_items_title_start_3.h"
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"

namespace gm
{

    bn::fixed loop(bn::fixed number, bn::fixed increment){
    number += increment;
    if(number > 256){
        return 0;
    } else {
        return number;
    }
    }

    Scene Title::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);

        bn::fixed_point init_pos = bn::fixed_point(-70, 44);

        bn::sprite_ptr apple_cat = bn::sprite_items::apple_cat.create_sprite(init_pos.x(), init_pos.y());

        bn::sprite_ptr start1 = bn::sprite_items::title_start_1.create_sprite(-16,30);
        bn::sprite_ptr start2 = bn::sprite_items::title_start_2.create_sprite(0,30);
        bn::sprite_ptr start3 = bn::sprite_items::title_start_3.create_sprite(16,30);
        
        bn::sprite_ptr title1 = bn::sprite_items::title_1.create_sprite(-31,-30);
        bn::sprite_ptr title2 = bn::sprite_items::title_2.create_sprite(31,-30);


        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);

        apple_cat.set_camera(camera);
        regular_bg.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed layer_2 = 0;
        bn::fixed layer_3 = 0;
        bn::fixed layer_4 = 0;

        bn::fixed start_y = 20;
        bn::fixed start_amp = 0;

        int timer = 0;

        while(!(bn::keypad::any_pressed() && timer > 60))
        {

            ++timer;
            layer_1 = loop(layer_1, 0.3);
            layer_2 = loop(layer_2, 0.2);
            layer_3 = loop(layer_3, 0.1);
            layer_4 = loop(layer_4, 0.5);

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            start1.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);
            start2.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);
            start3.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);


            for(int index = 0, limit = 160; index < limit; ++index)
            {
                horizontal_deltas[index] = layer_4;
            }

            horizontal_hbe.reload_deltas_ref();

            //_action.update();
            
            bn::core::update();
        }

        return Scene::MENU;
        
        //bg.set_blending_enabled(true);
    }
    
    
}