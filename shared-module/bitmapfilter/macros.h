// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
// SPDX-FileCopyrightText: Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT


#pragma once

// Triggered by use of IM_MIN(IM_MAX(...)); this is a spurious diagnostic.
#pragma GCC diagnostic ignored "-Wshadow"

#define IM_MAX(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a, b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })

#define IMAGE_RGB565_LINE_LEN_BYTES(bitmap) \
    ((bitmap)->width * 2)

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(bitmap, y) \
    (uint16_t *)(&(bitmap)->data[(bitmap)->stride * (y)])
#define IMAGE_GET_RGB565_PIXEL_FAST(rowptr, x) \
    __builtin_bswap16((rowptr)[(x)])
#define IMAGE_PUT_RGB565_PIXEL_FAST(rowptr, x, val) \
    ((rowptr)[(x)] = __builtin_bswap16((val)))
#define COLOR_R5_G6_B5_TO_RGB565(r, g, b) \
    (((r) << 11) | ((g) << 5) | (b))
#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8)    ((((r8) & 0xF8) << 8) | (((g8) & 0xFC) << 3) | ((b8) >> 3))

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 11) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel >> 8) & 0xF8; \
        __pixel | (__pixel >> 5); \
    })

#define COLOR_RGB565_TO_G6(pixel) (((pixel) >> 5) & 0x3F)
#define COLOR_RGB565_TO_G8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel >> 3) & 0xFC; \
        __pixel | (__pixel >> 6); \
    })

#define COLOR_RGB565_TO_B5(pixel) ((pixel) & 0x1F)
#define COLOR_RGB565_TO_B8(pixel) \
    ({ \
        __typeof__ (pixel) __pixel = (pixel); \
        __pixel = (__pixel << 3) & 0xF8; \
        __pixel | (__pixel >> 5); \
    })
#define COLOR_R5_MAX (0x1F)
#define COLOR_G6_MAX (0x3F)
#define COLOR_B5_MAX (0x1F)
#define COLOR_R8_MIN                            0
#define COLOR_R8_MAX                            255
#define COLOR_G8_MIN                            0
#define COLOR_G8_MAX                            255
#define COLOR_B8_MIN                            0
#define COLOR_B8_MAX                            255

#define COLOR_RGB565_BINARY_MAX (0xffff)
#define COLOR_RGB565_BINARY_MIN (0x0000)

#define COLOR_RGB888_TO_Y(r8, g8, b8) ((((r8) * 38) + ((g8) * 75) + ((b8) * 15)) >> 7) // 0.299R + 0.587G + 0.114B
#define COLOR_RGB565_TO_Y(rgb565) \
    ({ \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565); \
        int g = COLOR_RGB565_TO_G8(__rgb565); \
        int b = COLOR_RGB565_TO_B8(__rgb565); \
        COLOR_RGB888_TO_Y(r, g, b); \
    })

#define COLOR_RGB888_TO_U(r8, g8, b8)           ((((r8) * -21) - ((g8) * 43) + ((b8) * 64)) >> 7) // -0.168736R - 0.331264G + 0.5B
#define COLOR_RGB565_TO_U(rgb565)                \
    ({                                           \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565);    \
        int g = COLOR_RGB565_TO_G8(__rgb565);    \
        int b = COLOR_RGB565_TO_B8(__rgb565);    \
        COLOR_RGB888_TO_U(r, g, b);              \
    })

#define COLOR_RGB888_TO_V(r8, g8, b8)           ((((r8) * 64) - ((g8) * 54) - ((b8) * 10)) >> 7) // 0.5R - 0.418688G - 0.081312B
#define COLOR_RGB565_TO_V(rgb565)                \
    ({                                           \
        __typeof__ (rgb565) __rgb565 = (rgb565); \
        int r = COLOR_RGB565_TO_R8(__rgb565);    \
        int g = COLOR_RGB565_TO_G8(__rgb565);    \
        int b = COLOR_RGB565_TO_B8(__rgb565);    \
        COLOR_RGB888_TO_V(r, g, b);              \
    })

uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v);

// CIRCUITPY-CHANGE (vs openmv): an offset is removed so that the Y value is the
// same as from COLOR_RGB565_TO_Y.
#define COLOR_YUV_TO_RGB565(y, u, v)            imlib_yuv_to_rgb((y), u, v)
