/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    ITCM : ORIGIN = 0x00000000, LENGTH = 512K
    RAM  : ORIGIN = 0x01000000, LENGTH = 2M
}

#define ISR_SECTION ITCM

#include "cortex-m.ld"
