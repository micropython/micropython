/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "py/obj.h"
#include "lib/protomatter/src/core.h"
#include "shared-bindings/busio/I2C.h"

extern const mp_obj_type_t is31fl3741_is31fl3741_type;
typedef struct {
    mp_obj_base_t base;
    mp_obj_t framebuffer;
    mp_buffer_info_t bufinfo;
    uint16_t bufsize, width, height, scale_width, scale_height;
    busio_i2c_obj_t *i2c;
    busio_i2c_obj_t inline_i2c;
    uint8_t device_address;
    uint8_t bit_depth;
    bool paused;
    bool scale;
    bool auto_gamma;
} is31fl3741_is31fl3741_obj_t;

static const uint16_t glassesmatrix_ledmap[18 * 5 * 3] = {
    65535, 65535, 65535, // (0,0) (clipped, corner)
    10,    8,     9,     // (0,1) / right ring pixel 20
    13,    11,    12,    // (0,2) / 19
    16,    14,    15,    // (0,3) / 18
    4,     2,     3,     // (0,4) / 17
    217,   215,   216,   // (1,0) / right ring pixel #21
    220,   218,   219,   // (1,1)
    223,   221,   222,   // (1,2)
    226,   224,   225,   // (1,3)
    214,   212,   213,   // (1,4)
    187,   185,   186,   // (2,0)
    190,   188,   189,   // (2,1)
    193,   191,   192,   // (2,2)
    196,   194,   195,   // (2,3)
    184,   182,   183,   // (2,4)
    37,    35,    36,    // (3,0)
    40,    38,    39,    // (3,1)
    43,    41,    42,    // (3,2)
    46,    44,    45,    // (3,3)
    34,    32,    33,    // (3,4)
    67,    65,    66,    // (4,0)
    70,    68,    69,    // (4,1)
    73,    71,    72,    // (4,2)
    76,    74,    75,    // (4,3)
    64,    62,    63,    // (4,4)
    97,    95,    96,    // (5,0)
    100,   98,    99,    // (5,1)
    103,   101,   102,   // (5,2)
    106,   104,   105,   // (5,3)
    94,    92,    93,    // (5,4)
    127,   125,   126,   // (6,0) / right ring pixel 3
    130,   128,   129,   // (6,1)
    133,   131,   132,   // (6,2)
    136,   134,   135,   // (6,3)
    124,   122,   123,   // (6,4)
    157,   155,   156,   // (7,0)
    160,   158,   159,   // (7,1)
    163,   161,   162,   // (7,2) / right ring pixel 5
    166,   164,   165,   // (7,3) / 6
    244,   242,   243,   // (7,4) / 7
    247,   245,   246,   // (8,0)
    250,   248,   249,   // (8,1)
    253,   251,   252,   // (8,2)
    256,   254,   255,   // (8,3)
    65535, 65535, 65535, // (8,4) (clipped, nose bridge)
    345,   347,   346,   // (9,0)
    342,   344,   343,   // (9,1)
    267,   269,   268,   // (9,2)
    263,   265,   264,   // (9,3)
    65535, 65535, 65535, // (9,4) (clipped, nose bridge)
    336,   338,   337,   // (10,0)
    333,   335,   334,   // (10,1)
    237,   239,   238,   // (10,2) / left ring pixel 19
    233,   235,   234,   // (10,3) / 18
    348,   262,   349,   // (10,4) / 17
    327,   329,   328,   // (11,0) / left ring pixel 21
    324,   326,   325,   // (11,1)
    207,   209,   208,   // (11,2)
    203,   205,   204,   // (11,3)
    330,   202,   331,   // (11,4)
    318,   320,   319,   // (12,0)
    315,   317,   316,   // (12,1)
    177,   179,   178,   // (12,2)
    173,   175,   174,   // (12,3)
    321,   172,   322,   // (12,4)
    309,   311,   310,   // (13,0)
    306,   308,   307,   // (13,1)
    147,   149,   148,   // (13,2)
    143,   145,   144,   // (13,3)
    312,   142,   313,   // (13,4)
    300,   302,   301,   // (14,0)
    297,   299,   298,   // (14,1)
    117,   119,   118,   // (14,2)
    113,   115,   114,   // (14,3)
    303,   112,   304,   // (14,4)
    291,   293,   292,   // (15,0)
    288,   290,   289,   // (15,1)
    87,    89,    88,    // (15,2)
    83,    85,    84,    // (15,3)
    294,   82,    295,   // (15,4)
    282,   284,   283,   // (16,0) / left ring pixel 3
    279,   281,   280,   // (16,1)
    57,    59,    58,    // (16,2)
    53,    55,    54,    // (16,3)
    285,   52,    286,   // (16,4)
    65535, 65535, 65535, // (17,0) (clipped, corner)
    270,   272,   271,   // (17,1) / left ring pixel 4
    27,    29,    28,    // (17,2) / 5
    23,    25,    24,    // (17,3) / 6
    276,   22,    277,   // (17,4) / 7
};

// Gamma correction table
static const uint8_t IS31GammaTable[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,
    3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,
    6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,
    11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,
    17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  34,  34,  35,
    36,  37,  38,  38,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,  80,  81,
    82,  84,  85,  86,  88,  89,  90,  92,  93,  94,  96,  97,  99,  100, 102,
    103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
    127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
    154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
    255
};
