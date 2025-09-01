#include "gm_stage.h"


#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"



#include "gm_level.h"

#include "bn_affine_bg_items_stage.h"


namespace gm
{
    Stage::Stage(Player& player):
    _player(&player)
    {

    }

    Scene Stage::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());


        bn::affine_bg_ptr map = bn::affine_bg_items::stage.create_bg(512, 512);
        map.set_priority(1);

        gm::Level level = gm::Level(map);

        map.set_camera(camera);


        _player->spawn(spawn_location, camera, map);

        while(true)
        {
            _player->update_pos(map,level);
            _player->apply_animation_state();
            bn::core::update();
        }
    }
}