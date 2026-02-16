#ifndef GM_LOADING_H
#define GM_LOADING_H

#include "gm_scene.h"
#include "gm_player.h"

namespace gm
{
    class Loading
    {       
        public:
            void execute(Scene next_scene);

        private:
            Player* _player;
    };
}

#endif