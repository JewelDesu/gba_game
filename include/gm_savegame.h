#ifndef GM__SAVEGAME_H
#define GM__SAVEGAME_H

#include "bn_core.h"
#include "bn_array.h"
#include "bn_fixed_point.h"
#include "gm_scene.h"
#include "gm_player_type.h"


namespace gm
{
    struct savegame
    {
        int return_value = 666;
        Scene stage_name = Scene::SELECT;
        //Scene stage_name = Scene::THIRD;
        Scene used_objects[16];
        uint8_t _obj_count = 0;
        uint8_t health = 10;
        uint8_t max_health = 10;
        bn::fixed_point _pos = bn::fixed_point(175, 391);
        //bn::fixed_point _pos = bn::fixed_point(169, 631);
        //bool (Scene obj);
        bool used_obj(Scene obj);
        void use_obj(Scene obj);
        bool valid();
        int points = 0;
        PLAYER_TYPE player_type = PLAYER_TYPE::BANANA_CAT;

    };
    
}


#endif