#ifndef GM_LEVEL_H
#define GM_LEVEL_H

#include "bn_affine_bg_ptr.h"
#include "bn_vector.h"


namespace gm
{
    class Level
    {
        public:
        Level (bn::affine_bg_ptr bg);

        [[nodiscard]] bn::vector<int,64> floor_tiles();
        [[nodiscard]] bn::vector<int,64> wall_tiles();
        [[nodiscard]] bn::vector<int,64> ceiling_tiles();

        private:
            bn::vector<int,64> _floor_tiles;
            bn::vector<int,64> _wall_tiles;
            bn::vector<int,64> _ceiling_tiles;
    };
}

#endif