/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2018 Damien P. George
 */

MEMORY
{
    ROM : ORIGIN = 0x00000000, LENGTH = 1M
    RAM : ORIGIN = 0x20000000, LENGTH = 128K
}

REGION_ALIAS("ISR", ROM)
REGION_ALIAS("TEXT", ROM)
REGION_ALIAS("DATA", RAM)
REGION_ALIAS("BSS", RAM)

_estack = ORIGIN(RAM) + LENGTH(RAM);

INCLUDE "mcu/arm/cortex-m.ld"
