#include "gm_player_stats.h"

#include "bn_core.h"
#include "bn_log.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_sprite_text_generator.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_string.h"
#include "bn_math.h"


#include "bn_sprite_items_health.h"
#include "bn_sprite_items_weapon2.h"
#include "bn_regular_bg_items_textbox.h"
#include "bn_sprite_items_cursor.h"
#include "gm_small_sprite_font.h"

namespace gm
{

    //const constexpr int x = -108;
    //const constexpr int y = -72;
    //const constexpr int _weaponx = -112;
    //const constexpr int _weapon_y = -74;
    //const constexpr int img = 16;

    
    const constexpr int x = -96;
    const constexpr int y = -64;
    const constexpr int _weaponx = -104;
    const constexpr int _weapon_y = -68;
    const constexpr int points_x = 119;
    const constexpr int points_y = -74;

    void _set_visible(bool visible, bn::ivector<bn::sprite_ptr>& sprites)
    {
        for(bn::sprite_ptr& sprite : sprites)
        {
            sprite.set_visible(visible);
        }
    }
    
    Weapon _weapon;

    Healthbar::Healthbar():
    _sprites (bn::sprite_items::health.create_sprite(x,y)),
    _weapon_sprites (bn::sprite_items::weapon2.create_sprite(_weaponx,_weapon_y))

    {
        _sprites.set_bg_priority(0);
        _weapon_sprites.set_bg_priority(0);
    }
    
    int Healthbar::hp()
    {
        return _hp;
    }

    void Healthbar::set_hp(int hp)
    {
        _hp = hp;
        _sprites.set_item(bn::sprite_items::health, 10 - hp);
    }

    void Healthbar::set_weapon(Weapon weapon)
    {
        _weapon = weapon;
        BN_LOG("void Healthbar::set_weapon()");
        if(_weapon == gm::Weapon::MELEE)
        {
            _weapon_sprites.set_item(bn::sprite_items::weapon2, 0);
        }
        else
            _weapon_sprites.set_item(bn::sprite_items::weapon2, 1);
        
    }

    void Healthbar::set_visible(bool is_visable)
    {
        _sprites.set_scale(2);
        _weapon_sprites.set_scale(2);
        _sprites.set_visible(is_visable);
        _weapon_sprites.set_visible(is_visable);
    }

    gui::gui()
    {

    }

    int gui::textbox(const bn::string_view& text, bn::vector<bn::string_view,4> options, bn::fixed _x_pos, bn::fixed _y_pos, bool textbox)
    {
        bn::regular_bg_ptr _textbox = bn::regular_bg_items::textbox.create_bg(0,0);
        _textbox.set_priority(1);
        if(textbox)
            _textbox.set_visible(true);
        else
            _textbox.set_visible(false);
        bn::vector<bn::sprite_ptr,32> text_sprites;
        bn::sprite_text_generator small_text_generator(gm::small_sprite_font);
        small_text_generator.set_bg_priority(1);
        small_text_generator.set_left_alignment();
        bn::core::update();

        bn::fixed _y = 0;
        bn::fixed start_y = _y_pos;
        bn::fixed start_x = _x_pos + 2;
        bn::fixed start_amp = 0;


        for(int i = 1, limit = text.length(); i <= limit; i++)
        {
            text_sprites.clear();
            //bn::fixed _y_pos=35;

            int start=0;
            for(int j=0; j<i; j++)
            {
                if(text.at(j) == '\n')
                {
                    int len = j - start;
                    if(len > 0)
                    {
                        bn::string_view sub_text = text.substr(start, j - start + 1);
                        small_text_generator.generate(_x_pos - 4, _y_pos, sub_text, text_sprites);
                    }
                }
                if(j == i - 1 && j - start > 0)
                {
                    bn::string_view sub_text = text.substr(start, j - start + 1);
                    small_text_generator.generate(_x_pos - 4, _y_pos, sub_text, text_sprites);
                }
            }
            char last_char = text.at(i -1);
            if(last_char != ' ' && last_char != '\n')
            {

            }

            for(int j=0; j<3; j++)
            {
                if(bn::keypad::a_pressed() || bn::keypad::b_pressed() || bn::keypad::start_pressed())
                {
                    i = limit - 1;
                }
                bn::core::update();
            }
            _y = _y_pos + 10;
        }

        for(int i = 0, limit = options.size(); i < limit; i++)
        {
            small_text_generator.generate(_x_pos + 8, _y + 10 * i, options[i], text_sprites);
        }

        bn::optional<bn::sprite_ptr> cursor = options.size() > 0 ? bn::optional<bn::sprite_ptr>(
            bn::sprite_items::cursor.create_sprite(bn::fixed_point(_x_pos - 2, _y))): bn::optional<bn::sprite_ptr>();
        
        if(cursor)
        {
            cursor->set_bg_priority(1);
        }
        int ind = 0;

        bn::core::update();

        while(!bn::keypad::a_pressed() && !bn::keypad::start_pressed())
        {
            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            //cursor->set_x(start_x + bn::degrees_lut_sin(start_amp*1)*2);

            if(cursor)
            {
                if(bn::keypad::down_pressed())
                {
                    ind++;
                    if(ind >= options.size())
                    {
                        ind = 0;
                    }
                    cursor->set_position(cursor->x(), _y + ind * 10);
                }
                if(bn::keypad::up_pressed())
                {
                    ind--;
                    if(ind < 0)
                    {
                        ind = options.size()-1;
                    }
                    cursor->set_position(cursor->x(), _y + ind * 10);
                }
            }
            bn::core::update();
        }

        bn::core::update();
        _textbox.set_visible(false);
        return ind;
    }

    void gui::textbox(const bn::string_view& text)
    {
        textbox(text, bn::vector<bn::string_view,4>(), -113, 35, true);
    }


    points::points()
    {

    }

    int points::point()
    {
        return _points;
    }
            
    void points::set_points(int points)
    {
        point_sprites.clear(); 
        _points += points;
        update();
    }

    void points::set_def_points(int points)
    {
        point_sprites.clear(); 
        _points = points;
    }

    void points::set_visable(bool visible)
    {
        _set_visible(visible, point_sprites);
    }
    void points::update()
    {
        int points = _points;
        bool visible = true;
        bn::sprite_text_generator small_text_generator(gm::small_sprite_font);
        small_text_generator.set_bg_priority(1);
        small_text_generator.set_right_alignment();

        bn::string<8> text = bn::to_string<8>(points);
        point_sprites.clear();
        small_text_generator.generate(points_x, points_y, text, point_sprites);
        _set_visible(visible, point_sprites);
    }
}