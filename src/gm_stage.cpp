#include "gm_stage.h"


#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"



#include "gm_level.h"

#include "bn_affine_bg_items_house_2x.h"
#include "bn_regular_bg_items_house_bg.h"


namespace gm
{
    Stage::Stage(Player& player):
    _player(&player)
    {

    }

    Scene Stage::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());


        bn::regular_bg_ptr map_bg = bn::regular_bg_items::house_bg.create_bg(512, 512);
        bn::affine_bg_ptr map = bn::affine_bg_items::house_2x.create_bg(512, 512);
        map_bg.set_priority(2);
        map.set_priority(1);

        gm::Level level = gm::Level(map);

        map.set_camera(camera);
        map_bg.set_camera(camera);


        _player->spawn(spawn_location, camera, map);

        while(true)
        {
            _player->update_pos(map,level);

            bn::core::update();
        }
    }
}