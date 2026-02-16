#ifndef GM_DEATH_H
#define GM_DEATH_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_animate_actions.h"

#include "gm_scene.h"
#include "gm_player.h"

namespace bn
{
    class sprite_text_generator;
}
namespace gm
{
    class Death
    {
        public:
        Death(Player& player, bn::sprite_text_generator& text_generator);
        Scene execute();



        private:
        Player* _player;
        bn::sprite_text_generator* _text_generator;
        bn::vector<bn::sprite_ptr, 16> labels;
    };
}


#endif