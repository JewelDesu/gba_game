#include "bn_core.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "gm_scene.h"
#include "gm_title.h"
#include "gm_menu.h"
#include "gm_small_sprite_font.h"



int main()
{
    bn::core::init();
    gm::Scene scene = gm::Scene::TITLE;

    bn::sprite_text_generator small_text_generator(gm::small_sprite_font);
    small_text_generator.set_bg_priority(1);
   
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
        bn::core::update();
    }
}
