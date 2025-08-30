#ifndef GM_TITLE_H
#define GM_TITLE_H

#include "gm_scene.h"
#include "bn_vector.h"
#include "bn_blending_actions.h"
#include "bn_sprite_position_hbe_ptr.h"

namespace bn
{
    class sprite_text_generator;
}
namespace gm
{
    class Title
    {
        public:
            Title(bn::sprite_text_generator& text_generator);
            Scene execute();

        private:
            bn::sprite_text_generator* _text_generator;
            bn::vector<bn::sprite_ptr, 16> labels;
            bn::optional<bn::blending_fade_alpha_to_action> _fade_action;
            void fade_out();
    };
}





#endif