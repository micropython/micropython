/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    RAM : ORIGIN = 0x00000000, LENGTH = 4M
}

#include "cortex-m.ld"
