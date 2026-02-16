#ifndef GM_PLAYER_STATS_H
#define GM_PLAYER_STATS_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

namespace gm
{
    enum class Weapon
    {
        MELEE,
        RANGED
    };
    class Healthbar
    {
        public:
            Healthbar();
            int hp();
            void set_hp(int hp);
            void set_visible(bool is_visable);
            void set_weapon(Weapon weapon);


        private:    
            bn::sprite_ptr _sprites;
            
            bn::sprite_ptr _weapon_sprites;
            int _hp = 10;
            bool _is_visible;
            Weapon _weapon = Weapon::MELEE;
            
            
        
    };

    class gui
    {
        public:
            gui();
            void textbox(const bn::string_view& text);
            int textbox(const bn::string_view& text, bn::vector<bn::string_view,4> options, bn::fixed _x_pos, bn::fixed _y_pos, bool textbox);
        
    };

    class points
    {
        public:
            points();
            int point();
            void set_points(int points);
            void update();
            void set_visable(bool visible);
            void set_def_points(int points);

        private:
            bn::vector<bn::sprite_ptr,32> point_sprites;
            int _points = 0;
    };
}



#endif