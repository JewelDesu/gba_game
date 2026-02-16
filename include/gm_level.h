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

        [[nodiscard]] bn::vector<int,32> floor_tiles();
        [[nodiscard]] bn::vector<int,32> wall_tiles();
        [[nodiscard]] bn::vector<int,32> ceiling_tiles();
        [[nodiscard]] bn::vector<int,32> obsticle_tiles();

        private:
            bn::vector<int,32> _floor_tiles;
            bn::vector<int,32> _wall_tiles;
            bn::vector<int,32> _ceiling_tiles;
            bn::vector<int,32> _obsticle_tiles;
    };
}

#endif