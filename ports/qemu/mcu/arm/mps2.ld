/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    RAM : ORIGIN = 0x00000000, LENGTH = 4M
}

REGION_ALIAS("ISR", RAM)
REGION_ALIAS("TEXT", RAM)
REGION_ALIAS("DATA", RAM)
REGION_ALIAS("BSS", RAM)

_estack = ORIGIN(RAM) + LENGTH(RAM);

INCLUDE "mcu/arm/cortex-m.ld"
