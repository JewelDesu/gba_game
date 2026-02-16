#ifndef GM_PLAYER_SELECT_H
#define GM_PLAYER_SELECT_H

#include "gm_scene.h"
#include "gm_player.h"


#include "bn_vector.h"
#include "bn_blending_actions.h"
#include "bn_sprite_position_hbe_ptr.h"

namespace bn
{
    class sprite_text_generator;
}
namespace gm
{
    class Player_select
    {
        public:
            Player_select(Player& player, bn::sprite_text_generator& text_generator);
            Scene execute();


        private:
            bn::sprite_text_generator* _text_generator;
            bn::vector<bn::sprite_ptr, 16> labels;
            Player* _player;


    };
    
}




#endif