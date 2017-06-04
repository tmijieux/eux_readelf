#ifndef EUX_ERROR_H
#define EUX_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include "color.h"

#define eux_fatal_error(msg, ...)                                       \
    do {                                                                \
        fprintf(stderr, "%sfatal error:%s ", s_eux_color_error, s_eux_color_reset); \
        fprintf(stderr, (msg), ## __VA_ARGS__);                         \
        exit(EXIT_FAILURE);                                             \
    } while(0)                                                          \

#define eux_error(msg, ...)                                             \
    do {                                                                \
        fprintf(stderr, "%serror:%s ", s_eux_color_error, s_eux_color_reset); \
        fprintf(stderr, (msg), ## __VA_ARGS__);                         \
    } while(0)                                                          \

#endif // EUX_ERROR_H
