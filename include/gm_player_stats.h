#ifndef GM_PLAYER_STATS_H
#define GM_PLAYER_STATS_H

#include "bn_sprite_ptr.h"

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
}



#endif