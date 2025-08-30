#include "gm_menu.h"

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
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"

namespace gm
{

    Scene Menu::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);

        bn::sprite_ptr title1 = bn::sprite_items::title_1.create_sprite(-31,-30);
        bn::sprite_ptr title2 = bn::sprite_items::title_2.create_sprite(31,-30);


        bn::camera_ptr camera = bn::camera_ptr::create(0,0);


        regular_bg.set_camera(camera);

        bn::fixed start_y = -30;
        bn::fixed start_amp = 0;

        int timer = 0;


        while(!(bn::keypad::any_pressed() && timer > 60))
        {

            ++timer;

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            title1.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);
            title2.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*6);

            //_action.update();
            
            bn::core::update();
        }
        return Scene::TITLE;
    }
    
    
}