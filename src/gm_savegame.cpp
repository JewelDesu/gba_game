
#include "gm_savegame.h"

namespace gm
{
    bool savegame::used_obj(Scene obj)
    {
        for(int i = 0, limit = _obj_count; i < limit; i++)
        {
            if(obj == used_objects[i])
            {
                return true;
            }
        }
        return false;
    }

    void savegame::use_obj(Scene obj)
    {
        if(!used_obj(obj))
        {
            BN_ASSERT(_obj_count<16,"object overflow");
            _obj_count++;
            used_objects[_obj_count-1] = obj;
        }
    }

    bool savegame::valid()
    {
        return return_value == 666;
    }
}