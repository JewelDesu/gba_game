#ifndef GM_MINNIE_H
#define GM_MINNIE_H

#include "gm_player.h"
#include "gm_scene.h"
#include "bn_fixed_point.h"

#include "gm_enemy_gun.h"

namespace gm
{
    class Minnie
    {
        public:
            Minnie(Player& player);
            Scene execute(bn::fixed_point spawn);

        private:
        Player* _player;
        Enemy_Gun enemy_bullets;
    };
}

#endif