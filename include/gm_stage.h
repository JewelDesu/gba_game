#ifndef GM_STAGE_H
#define GM_STAGE_H

#include "gm_player.h"
#include "gm_scene.h"
#include "bn_fixed_point.h"



namespace gm
{
    class Stage
    {
        public:
            Stage(Player& player);
            Scene execute(bn::fixed_point spawn);

        private:
        Player* _player;
    };
}

#endif