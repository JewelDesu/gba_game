#include "bn_core.h"
#include "bn_math.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_text_generator.h"

#include "gm_scene.h"
#include "gm_title.h"
#include "gm_menu.h"
#include "gm_player.h"
#include "gm_stage.h"
#include "gm_level.h"

#include "gm_small_sprite_font.h"
#include "bn_sprite_items_banana.h"


int main()
{
    bn::core::init();
    gm::Scene scene = gm::Scene::TITLE;

    bn::sprite_text_generator small_text_generator(gm::small_sprite_font);
    small_text_generator.set_bg_priority(1);

    bn::sprite_ptr cat_sprite = bn::sprite_items::banana.create_sprite(0,0);
    cat_sprite.set_visible(false);
    gm::Player player = gm::Player(cat_sprite);
   
    while(true)
    {
        if(scene == gm::Scene::TITLE)
        {
            gm::Title title = gm::Title(small_text_generator);
            scene = title.execute();
        }
        else if(scene == gm::Scene::MENU)
        {
            gm::Menu menu = gm::Menu();
            scene = menu.execute();
        }
        else if(scene == gm::Scene::STAGE)
        {
            gm::Stage stage = gm::Stage(player);
            scene = stage.execute(bn::fixed_point(293, 368));
        }
        bn::core::update();
    }
}
