#include "gm_death.h"

#include "bn_core.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_animate_actions.h"
#include "bn_camera_ptr.h"

#include "gm_small_sprite_font.h"
#include "bn_regular_bg_items_bg.h"

#include "bn_sprite_items_banana_cry.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "bn_sound_items.h"


namespace gm
{
    Death::Death(Player& player, bn::sprite_text_generator& text_generator):
    _player(&player),
    _text_generator(&text_generator)
    {

    }

    Scene Death::execute()
    {
        _player->set_hp(10);
        //_player->set_healthbar_visibility(false);

        bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);

        //bn::sound_items::banana_cat_cry.play();
        //bn::music::set_volume(0.6);

        bn::sprite_ptr _sprite = bn::sprite_items::banana_cry.create_sprite(0,-10);
        _sprite.set_horizontal_scale(2);
        _sprite.set_vertical_scale(2);

        bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 15, bn::sprite_items::banana_cry.tiles_item(), 0,1,2,3,0,1,2,3,0,1);

        bn::camera_ptr camera = bn::camera_ptr::create(0,0);
        
        _sprite.set_camera(camera);

        
        int time = 0;
        bool text_made = false;
        while(time < 120 || !bn::keypad::any_pressed())
        {
            ++time;
            if(time > 180 && !text_made){
                _text_generator->generate(-38, 55, "You have died", labels);
                text_made = true;
            }

            _action.update();
            bn::core::update();
        }

        return Scene::TITLE;
    }
}