#include "color.h"

#define EUX_COLOR_TO_STRING(name_capital_, name_, value_)               \
    static const char * const s_eux_color_##name_capital_ = "\033["#value_"m";

#define EUX_COLOR_TO_PTR_DEFINE(name_capital_, name_, value_)   \
    const char *s_eux_color_##name_ = "\033["#value_"m";

EUX_COLOR_LIST(EUX_COLOR_TO_STRING)
EUX_COLOR_LIST(EUX_COLOR_TO_PTR_DEFINE)

#define EUX_COLOR_ENABLE(name_capital_, name_, value_)  \
    s_eux_color_##name_ = s_eux_color_##name_capital_;

#define EUX_COLOR_DISABLE(name_capital_, name_, value_) \
    s_eux_color_##name_ = "";

void eux_set_output_colored(bool enable_color)
{
    if (enable_color)
    { EUX_COLOR_LIST(EUX_COLOR_ENABLE) }
    else
    { EUX_COLOR_LIST(EUX_COLOR_DISABLE) }
}

