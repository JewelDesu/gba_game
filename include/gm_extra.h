#ifndef GM_EXTRA_H
#define GM_EXTRA_H

#include "bn_fixed_point.h"

namespace gm
{
    [[nodiscard]] constexpr bn::fixed modulo(bn::fixed a, bn::fixed m)
    {
        return a - m * ((a/m).integer());
    }

    [[nodiscard]] constexpr bn::fixed amp (bn::fixed start_amp)
    {
        start_amp += 6;
        if(start_amp >= 360){
            start_amp = 0;
        }

        return start_amp;
    }
}

#endif