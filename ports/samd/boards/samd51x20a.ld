/*
    GNU linker script for SAMD51x20
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00004000, LENGTH = 1024K - 16K
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 256K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 16K;

_oflash_fs = ORIGIN(FLASH) + 384K - 16K;
_sflash_fs = LENGTH(FLASH) - 384K + 16K - 1;

_sheap = _ebss;
_eheap = _sstack;
