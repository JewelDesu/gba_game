#ifndef GM_THIRD_H
#define GM_THIRD_H

#include "gm_player.h"
#include "gm_scene.h"
#include "bn_fixed_point.h"

#include "gm_enemy_gun.h"

namespace gm
{
    class Third
    {
        public:
            Third(Player& player);
            Scene execute(bn::fixed_point spawn);

        private:
        Player* _player;
        Enemy_Gun enemy_bullets;
        
    };
}

#endif