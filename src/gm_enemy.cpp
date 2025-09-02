
#include "gm_enemy.h"

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_size.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_log.h"

#include "gm_enemy_type.h"
#include "gm_level.h"

#include "bn_sprite_items_goomba.h"

namespace gm
{
    enum directions{up, down, left, right};


    [[nodiscard]] int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool contains_cell(int tile, bn::vector<int, 32> tiles)
    {
        for(int index = 0; index < tiles.size(); ++index)
        {
            if(tiles.at(index) == tile)
            {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool check_collisions_map(bn::fixed_point pos, directions direction,Hitbox hitbox, bn::affine_bg_ptr& map, gm::Level level, bn::span<const bn::affine_bg_map_cell> cells)
    {
        bn::fixed l = pos.x() - hitbox.width() / 2 + hitbox.x();
        bn::fixed r = pos.x() + hitbox.width() / 2 + hitbox.x();
        bn::fixed u = pos.y() - hitbox.height() / 2 + hitbox.y();
        bn::fixed d = pos.y() + hitbox.height() / 2 + hitbox.y();
        
        bn::vector<int, 32> tiles;
        if(direction == down){
            tiles = level.floor_tiles();
        } else if(direction == left || direction == right){
            tiles = level.wall_tiles();
        } else if(direction == up){
            tiles = level.ceiling_tiles();
        }

        if(contains_cell(get_map_cell(l, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles) ||
        contains_cell(get_map_cell(r, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles)){
            return true;
        } else {
            return false;
        }
    }

    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jumpy = 4;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Enemy::Enemy(bn::camera_ptr camera, bn::affine_bg_ptr map, int pos_x, int pos_y, ENEMY_TYPE type):
    _camera(camera),
    _map(map),
    _pos(pos_x, pos_y),
    _type(type),
    _level(Level(map));
    {
        _map_cells = map.map().cells_ref().value();

        if(_type == ENEMY_TYPE::GOOMBA)
        {
            _sprite = bn::sprite_items::bat_sprite.create_sprite(_pos.pos_x(), _pos.pos_y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                _sprite.value(), 6, bn::sprite_items::goomba.tiles_item(), 0,1,0,1);
        }
        _sprite.value().spawn_enemy(true);
    }

    void Enemy::spawn_enemy(bool spawned)
    {
        _sprite.value().spawn_enemy(spawned);
    }

    bn::fixed_point Enemy::pos()
    {
        return _pos;
    }

    void Enemy::set_pos(bn::fixed_point pos)
    {
        _pos = pos;
    }

    void Enemy::update_pos_enemy(bn::fixed_point player_pos)
    {
        _sprite.value().spawn_enemy(true);

        _dy += gravity;
        _dx = 0;
        _dir = -_dir;
        _direction_timer = 0;
        _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());

        if((_action.value().current_index() == 1 || _action.value().current_index() == 3)  && !_invulnerable && _grounded)
        {
            _dx += _dir*acc;
        }
    }
}
