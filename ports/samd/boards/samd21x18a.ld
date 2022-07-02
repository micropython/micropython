/*
    GNU linker script for SAMD21
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00002000, LENGTH = 256K - 8K
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 32K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 8K;

_oflash_fs = ORIGIN(FLASH) + 192K - 8K;
_sflash_fs = LENGTH(FLASH) - 192K + 8K - 1;

_sheap = _ebss;
_eheap = _sstack;
