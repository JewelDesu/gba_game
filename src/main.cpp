#include "bn_core.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_bg.h"
int main()
{
    bn::core::init();
    bn::regular_bg_ptr regular_bg = bn::regular_bg_items::bg.create_bg(0, 0);
    while(true)
    {
        bn::core::update();
    }
}
