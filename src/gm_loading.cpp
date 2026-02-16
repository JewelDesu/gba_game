#include "gm_loading.h"

#include "bn_core.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_random.h"

#include "gm_scene.h"

#include "bn_sprite_items_banana.h"
#include "bn_regular_bg_items_grasslands_loading.h"


namespace gm
{
    void Loading::execute(Scene next_scene)
    {
        bn::fixed_point init_pos = bn::fixed_point(0, 0);
        
        
        bn::sprite_ptr banana1 = bn::sprite_items::banana.create_sprite(init_pos.x(), init_pos.y()+35);
        banana1.set_horizontal_scale(2);
        banana1.set_vertical_scale(2);
        
        bn::sprite_animate_action<6> action1 = bn::create_sprite_animate_action_forever(
                        banana1, 3, bn::sprite_items::banana.tiles_item(), 8,9,4,5,6,7);



        bn::camera_ptr camera = bn::camera_ptr::create(init_pos.x()+100, init_pos.y());

        
        bn::regular_bg_ptr map = bn::regular_bg_items::grasslands_loading.create_bg(100, 256);
        


        
        banana1.set_camera(camera);

        map.set_camera(camera);
        //_player -> set_points_visibility(false);

        for(int i = 0; i < 160; ++i)
        {
            banana1.set_x(banana1.x() + 1.5);

            action1.update();
            bn::core::update();
        }
    }
}