/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    ROM : ORIGIN = 0x00000000, LENGTH = 1M
    RAM : ORIGIN = 0x20000000, LENGTH = 256K
}

#define ISR_SECTION  ROM
#define TEXT_SECTION ROM
#define DATA_SECTION RAM

#include "cortex-m.ld"
