#ifndef GM_EXTRA_H
#define GM_EXTRA_H

#include "bn_fixed_point.h"

namespace gm
{
    [[nodiscard]] constexpr bn::fixed modulo(bn::fixed a, bn::fixed m)
    {
        return a - m * ((a/m).integer());
    }
}

#endif