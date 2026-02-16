#ifndef GM_NPC_H
#define GM_NPC_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_text_generator.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_cell.h"

#include "gm_npc_type.h"
#include "gm_level.h"
#include "gm_hitbox.h"

namespace gm
{
    class Npc
    {


        public:
            Npc(bn::fixed_point pos, bn::camera_ptr& camera, NPC_TYPE type, bn::sprite_text_generator& text_generator);
            void update();
            bool finished_talking();
            bool check_trigger(bn::fixed_point pos);
            void talk();
            bool is_talking();
            void set_following(bool should_follow);
            void update_follow(bn::fixed_point target_pos);
            bool is_following() const;
            void move_towards_target(bn::fixed_point target_pos);


            void set_level(gm::Level level);
            void set_map(bn::affine_bg_ptr map);
            bool check_ground_collision();
            bool check_wall_collision(bn::fixed new_x);

        private:
            NPC_TYPE _type;
            bn::fixed_point _pos;
            bn::camera_ptr& _camera;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<4>> _action;
            bn::optional<gm::Level> _level;
            bn::fixed _dx = 0;
            bn::fixed _dy = 0;
            bn::optional<bn::affine_bg_ptr> _map;
            bn::optional<bn::span<const bn::affine_bg_map_cell>> _map_cells;

            bool _is_talking = false;
            bool near_player = false;
            bool finished = false;
            bool spoken_before = false;
            bool _is_following = false;

            Hitbox _hitbox = Hitbox(0, 0, 16, 16);

            
            bool _running = false;
            bool _grounded = false;
            bool _falling = false;
            bool _jumping = false;


            bn::vector<bn::sprite_ptr, 32> text_sprites;
            bn::sprite_text_generator& _text_generator;

            const bn::fixed text_y = 27;
            const bn::fixed text_limit_y = (bn::display::height() / 2) - text_y;
            bn::fixed follow_speed = 0.5;
            bn::fixed max_follow_distance = 100;
            bn::fixed min_follow_distance = 20;

            bn::span<bn::string_view> lines;

            int current_line = 0;
            int current_char = 0;
            bn::string_view current_chars = "";

            bn::string_view applecat_lines[1] = {
                "Hiiiiiii Im apple cat :D"
            };

            bn::string_view bananacat_lines[1] = {
                "Hiiiiiii Im banana cat :D"
            };

    };
}

#endif