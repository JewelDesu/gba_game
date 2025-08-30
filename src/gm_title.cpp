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

#include "gm_scene.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_sprite_items_apple_cat.h"
#include "bn_sprite_items_banana_cat.h"
#include "bn_sprite_items_title_start_1.h"
#include "bn_sprite_items_title_start_2.h"
#include "bn_sprite_items_title_start_3.h"
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"

namespace gm
{

    bn::fixed loop(bn::fixed number, bn::fixed increment){
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

void Title::fade_out()
    {
        bn::blending::set_fade_alpha(0);
        bn::blending::set_fade_color(bn::blending::fade_color_type::WHITE);
        _fade_action = bn::blending_fade_alpha_to_action(15, 1);
        while(!_fade_action.value().done()){
            _fade_action.value().update();
            bn::core::update();
        }
        bn::blending::set_fade_alpha(1);
    }

    Scene Title::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);

        bn::fixed_point init_pos1 = bn::fixed_point(-70, 44);
        bn::fixed_point init_pos2 = bn::fixed_point(70, 44);

        bn::sprite_ptr apple_cat = bn::sprite_items::apple_cat.create_sprite(init_pos1.x(), init_pos1.y());
        bn::sprite_ptr banana_cat = bn::sprite_items::banana_cat.create_sprite(init_pos2.x(), init_pos2.y());

        
        bn::sprite_ptr title1 = bn::sprite_items::title_1.create_sprite(-31,-30);
        bn::sprite_ptr title2 = bn::sprite_items::title_2.create_sprite(31,-30);


        _text_generator->set_left_alignment();
        _text_generator->generate(-38, 55, "PRESS START", labels);




        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::regular_bg_position_hbe_ptr horizontal_hbe =
                bn::regular_bg_position_hbe_ptr::create_horizontal(regular_bg, horizontal_deltas);

        apple_cat.set_camera(camera);
        banana_cat.set_camera(camera);
        regular_bg.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed start_y = -30;
        bn::fixed start_amp = 0;

        int timer = 0;


        while(!(bn::keypad::any_pressed() && timer > 60))
        {

            ++timer;
            layer_1 = loop(layer_1, 0.5);

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            title1.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);
            title2.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);

            for(bn::sprite_ptr& sprite : labels)
            {
                sprite.set_visible(true);
            }

            for(int index = 0, limit = 160; index < limit; ++index)
            {
                horizontal_deltas[index] = layer_1;
            }

            horizontal_hbe.reload_deltas_ref();

            //_action.update();
            
            bn::core::update();
        }
        fade_out();    
        return Scene::MENU;
        
        //bg.set_blending_enabled(true);
    }
    
    
}