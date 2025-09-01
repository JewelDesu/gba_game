#include "gm_level.h"


#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_vector.h"



namespace gm
{
    Level::Level(bn::affine_bg_ptr bg)
    {
        bool map_state = false;
        int ind = 0;
        int empty = 0;
        _floor_tiles = {};
        _wall_tiles = {};
        _ceiling_tiles = {};

        bn::span<const bn::affine_bg_map_cell> cells = bg.map().cells_ref().value();

        while(!map_state)
        {
            if(cells.at(ind) == 0)
            {
                ++empty;
                if(empty > 2)
                {
                    map_state = true;
                }
            }
            else{
                if(empty == 0)
                {
                    _floor_tiles.push_back(cells.at(ind));
                } 
                else if(empty == 1)
                {
                    _wall_tiles.push_back(cells.at(ind));
                }
                else if(empty == 2)
                {
                    _ceiling_tiles.push_back(cells.at(ind));
                }
            }
            ++ind;
        }
    }

    bn::vector<int,64> Level::floor_tiles()
    {
        return _floor_tiles;
    }
    bn::vector<int,64> Level::wall_tiles()
    {
        return _wall_tiles;
    }
    bn::vector<int,64> Level::ceiling_tiles()
    {
        return _ceiling_tiles;
    }
}