#include "bn_core.h"
#include "bn_regular_bg_ptr.h"

#include "gm_scene.h"
#include "gm_title.h"
#include "gm_menu.h"




int main()
{
    bn::core::init();
    gm::Scene scene = gm::Scene::TITLE;
   
    while(true)
    {
        if(scene == gm::Scene::TITLE)
        {
            gm::Title title = gm::Title();
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
