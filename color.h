#ifndef EUX_COLOR_H
#define EUX_COLOR_H

#include <stdbool.h>

#define EUX_COLOR_LIST(COLOR)                   \
    COLOR(RED,     red,     91 )                \
    COLOR(BLUE,    blue,    34 )                \
    COLOR(GREEN,   green,   92 )                \
    COLOR(YELLOW,  yellow,  93 )                \
    COLOR(ERROR,   error,   01;31 )             \
    COLOR(WARNING, warning, 01;35 )             \
    COLOR(NOTE,    note,    01;36 )             \
    COLOR(BOLD,    bold,    01 )                \
    COLOR(DEBUG,   debug,   93 )                \
    COLOR(RESET,   reset,   0  )

#define EUX_COLOR_TO_PTR_DECLARE(name_capital_, name_, value_)  \
    extern const char *s_eux_color_##name_;

EUX_COLOR_LIST(EUX_COLOR_TO_PTR_DECLARE)

void eux_set_output_colored(bool enable_color);

#endif // EUX_COLOR_H
