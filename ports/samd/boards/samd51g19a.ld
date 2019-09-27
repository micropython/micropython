/*
    GNU linker script for SAMD51
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00004000, LENGTH = 512K - 16K
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 192K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 16K;

_sheap = _ebss;
_eheap = _sstack;
