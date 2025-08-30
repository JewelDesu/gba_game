#include "gm_menu.h"

#include "bn_core.h"
#include "bn_regular_bg_ptr.h"

#include "gm_scene.h"

#include "bn_regular_bg_items_bg.h"


namespace gm
{

    Scene Menu::execute()
    {
        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);
        bn::core::update();
        return Scene::TITLE;
    }
    
    
}