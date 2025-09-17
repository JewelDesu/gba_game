#include "gm_player_stats.h"

#include "bn_log.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_health.h"
#include "bn_sprite_items_weapon2.h"

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
        _sprites.set_item(bn::sprite_items::health, 10-hp);
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


}