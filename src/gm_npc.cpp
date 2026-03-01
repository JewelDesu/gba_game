#include "gm_npc.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"

#include "gm_npc_type.h"

#include "bn_sprite_items_apple.h"
#include "bn_sprite_items_banana.h"

namespace gm
{
    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jumpy = 4;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;


    Npc::Npc(bn::fixed_point pos, bn::camera_ptr& camera, NPC_TYPE type, bn::sprite_text_generator& text_generator) :
    _type(type),
    _pos(pos),
    _camera(camera),
    _text_generator(text_generator)
    {
        _text_generator.set_bg_priority(0);

        if(_type == NPC_TYPE::APPLECAT)
        {
            _sprite = bn::sprite_items::apple.create_sprite(_pos.x(), _pos.y());

            _action = bn::create_sprite_animate_action_forever(
                    _sprite.value(), 20, bn::sprite_items::apple.tiles_item(), 0, 1, 2, 3);
            
            lines = bn::span(applecat_lines);
        }
        else if(_type == NPC_TYPE::BANANACAT)
        {
            _sprite = bn::sprite_items::banana.create_sprite(_pos.x(), _pos.y());

            _action = bn::create_sprite_animate_action_forever(
                    _sprite.value(), 20, bn::sprite_items::banana.tiles_item(), 0, 1, 2, 3);
            
            lines = bn::span(bananacat_lines);
        }

        if(_sprite.has_value())
        {
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(2);
            _sprite.value().set_z_order(2);
        }
    }

    void Npc::set_level(gm::Level level)
    {
        _level = level;
    }

    void Npc::set_map(bn::affine_bg_ptr map)
    {
        _map = map;
        if(_map.has_value())
        {
            _map_cells = _map.value().map().cells_ref().value();
        }
    }

    bool Npc::check_ground_collision()
    {
        if(!_map.has_value() || !_level.has_value() || !_map_cells.has_value())
        {
            return false;
        }

        bn::fixed left = _pos.x() - _hitbox.width() / 2 + _hitbox.x();
        bn::fixed right = _pos.x() + _hitbox.width() / 2 + _hitbox.x();
        bn::fixed bottom = _pos.y() + _hitbox.height() / 2 + _hitbox.y();

        int map_size = _map.value().dimensions().width();
        bn::vector<int, 32> floor_tiles_list = _level.value().floor_tiles();

        auto check_cell = [&](bn::fixed x, bn::fixed y) -> bool {
            int cell_index = (y.safe_division(8).right_shift_integer() * map_size / 8 + x.safe_division(8).integer());

            if(cell_index < 0 || cell_index >= _map_cells.value().size())
                return false;
            
            int tile = _map_cells.value().at(cell_index);

            for(int i = 0; i < floor_tiles_list.size(); ++i)
            {
                if(floor_tiles_list.at(i) == tile)
                    return true;
            }
            return false;
        };

        return check_cell(left, bottom) || check_cell(right, bottom);
    }

    bool Npc::check_wall_collision(bn::fixed new_x)
    {
        if(!_map.has_value() || !_level.has_value() || !_map_cells.has_value())
        {
            return false;
        }

        bn::fixed half_width = _hitbox.width() / 2;
        bn::fixed left = new_x - half_width + _hitbox.x();
        bn::fixed right = new_x + half_width + _hitbox.x();
        bn::fixed top = _pos.y() - _hitbox.height() / 2 + _hitbox.y();
        bn::fixed bottom = _pos.y() + _hitbox.height() / 2 + _hitbox.y();

        int map_size = _map.value().dimensions().width();
        bn::vector<int, 32> wall_tiles_list = _level.value().wall_tiles();

        auto check_cell = [&](bn::fixed x, bn::fixed y) -> bool {
            int cell_index = (y.safe_division(8).right_shift_integer() * map_size / 8 + x.safe_division(8).integer());

            if(cell_index < 0 || cell_index >= _map_cells.value().size())
                return false;
            
            int tile = _map_cells.value().at(cell_index);

            for(int i = 0; i < wall_tiles_list.size(); ++i)
            {
                if(wall_tiles_list.at(i) == tile)
                    return true;
            }
            return false;
        };

        return check_cell(left, top) || check_cell(left, bottom) || 
            check_cell(right, top) || check_cell(right, bottom);
    }

    void Npc::update_follow(bn::fixed_point target_pos)
    {
        if(!_is_following || !_sprite.has_value() || !_level.has_value())
        {
            return;
        }

        bn::fixed distance = target_pos.x() - _pos.x();
        
        // Only follow if within reasonable distance
        if(bn::abs(distance) > max_follow_distance || bn::abs(distance) < min_follow_distance)
        {
            _dx *= friction;
            _dx = 0; // Stop moving if too far
            return;
        }

        // Horizontal movement - try to match target's horizontal position
        if(distance > 5)
        {
            _dx += acc;
            _sprite.value().set_horizontal_flip(false);
        }
        else if(distance < -5)
        {
            _dx -= acc;
            _sprite.value().set_horizontal_flip(true);
        }
        else
        {
            _dx *= friction;
        }

        // Check if NPC should jump to follow player
        _grounded = check_ground_collision();
        
        if(_grounded && target_pos.y() < _pos.y() - 20)
        {
            // Jump if player is above
            _dy = -jumpy;
        }

        // Apply gravity
        _dy += gravity;
        if(_dy > max_dy)
        {
            _dy = max_dy;
        }

        // Update position with collision checking
        bn::fixed new_x = _pos.x() + _dx;
        
        if(!check_wall_collision(new_x))
        {
            _pos.set_x(new_x);
        }
        else
        {
            _dx = 0;
        }

        _pos.set_y(_pos.y() + _dy);

        // Check ground collision and stop falling
        if(check_ground_collision() && _dy > 0)
        {
            _dy = 0;
            _grounded = true;
        }
        else
        {
            _grounded = false;
        }
    }

    void Npc::move_towards_target(bn::fixed_point target_pos)
    {
        update_follow(target_pos);
    }

    void Npc::update()
    {
        if(_action.has_value())
        {
            _action.value().update();
        }

        if(_is_talking)
        {
           if(current_char == lines.at(current_line).size() * 2)
           {
                if(bn::keypad::up_pressed() || bn::keypad::a_pressed() || bn::keypad::a_held() || bn::keypad::up_held())
                {
                    if(current_line == lines.size()-1)
                    {
                        _is_talking = false;
                        current_chars = "";
                        current_char = 0;
                        current_line = 0;
                        spoken_before = true;
                    } 
                    else 
                    {
                        current_line += 1;
                        current_char = 0;
                    }
                } 
                else if(bn::keypad::start_pressed())
                {
                    _is_talking = false;
                    current_chars = "";
                    current_char = 0;
                    current_line = 0;
                    spoken_before = true;
                }
            } 
            else 
            {
                if(bn::keypad::start_pressed())
                {
                    _is_talking = false;
                    current_chars = "";
                    current_char = 0;
                    current_line = 0;
                    spoken_before = true;
                }
                current_chars = lines.at(current_line).substr(0,(current_char/2)+1);
                if(bn::keypad::a_held() || bn::keypad::up_held())
                {
                    current_char+=2;
                } 
                else 
                {
                    ++current_char;
                }
                
            }
            _text_generator.set_left_alignment();
            text_sprites.clear();
            _text_generator.generate(-115, text_limit_y, current_chars, text_sprites);
        } 
        else if(near_player && !finished) 
        {
            _text_generator.set_center_alignment();
            text_sprites.clear();
            _text_generator.generate(0, text_limit_y, "press 'up' to interact", text_sprites);
        } 
        else 
        {
            text_sprites.clear(); 
        }

        if(_action.has_value())
        {
            _action.value().update();
        }

        if(!_is_talking && _sprite.has_value())
        {
            _sprite -> set_position(_pos);
        }

    }

    bool Npc::finished_talking()
    {
        return spoken_before;
    }

    bool Npc::check_trigger(bn::fixed_point pos)
    {
        if(!finished)
        {
            if(bn::abs(_pos.x() - pos.x()) < 20)
            {
                if(bn::abs(_pos.y() - pos.y()) < 20)
                {
                    near_player = true;
                    return true;
                }
            }
            near_player = false;
        }
        return false;
    }

    void Npc::talk()
    {
        _is_talking = true;
    }

    bool Npc::is_talking()
    {
        return _is_talking;
    }

    void Npc::set_following(bool should_follow) 
    {
        _is_following = should_follow;
    }

    bool Npc::is_following() const 
    {
        return _is_following;
    }

    // void Npc::update_follow(bn::fixed_point target_pos)
    // {
    //     if(_is_following && _sprite.has_value())
    //     {
    //         bn::fixed distance = bn::abs(target_pos.x() - _pos.x());
            
    //         // Only follow if within follow distance range
    //         if(distance > min_follow_distance && distance < max_follow_distance)
    //         {
    //             move_towards_target(target_pos);
    //         }
    //     }
    // }

    // void Npc::move_towards_target(bn::fixed_point target_pos)
    // {
    //     // Calculate direction to move
    //     if(target_pos.x() < _pos.x())
    //     {
    //         _pos.set_x(_pos.x() - follow_speed);
    //         _sprite -> set_horizontal_flip(true);
    //     }
    //     else if(target_pos.x() > _pos.x())
    //     {
    //         _pos.set_x(_pos.x() + follow_speed);
    //         _sprite -> set_horizontal_flip(false);
    //     }

    //     // Update sprite position
    //     _sprite -> set_position(_pos);
    // }
}
