#ifndef GM_BULLET_EXTRA_H
#define GM_BULLET_EXTRA_H

#include "bn_math.h"
#include "bn_fixed_point.h"

namespace gm
{
    [[nodiscard]] constexpr bn::fixed_point unit_vector(bn::fixed x, bn::fixed y)
    {
        bn::fixed magnitude = bn::sqrt((x * x) + (y * y));
        return bn::fixed_point(x, y) / magnitude;
    }

    [[nodiscard]] constexpr bn::fixed_point direction_vector(bn::fixed x, bn::fixed y, bn::fixed speed)
    {
        return unit_vector(x, y) * speed;
    }
}

#endif