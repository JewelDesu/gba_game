#ifndef GM_PLAYER_GUN_DATA
#define GM_PLAYER_GUN_DATA

#include "bn_fixed_rect.h"

namespace bn
{
    class camera_ptr;
}

namespace gm
{

class Player;
class objects;

class Player_bullet_data
{

public:
    bn::fixed_rect bullet_rect;
    int bullet_damage;
    const bn::camera_ptr& camera_ref;
    Player& player_ref;
};

}

#endif