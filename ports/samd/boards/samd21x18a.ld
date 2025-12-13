/*
    GNU linker script for SAMD21
*/

/*
_codesize and _vfsromsize are defined in mpconfigmcu.mk or mpconfigboard.mk
as MICROPY_HW_CODESIZE and MICROPY_HW_VFSROMSIZE and are set in Makefile.
*/

_flashsize = 256K;  /* The physical flash size */
_bootloader = 8K;   /* Must match the ORIGIN value of FLASH */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00002000, LENGTH = _codesize
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 32K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 8K;

/*
The VfsROM file system is placed at the end of the flash.
For device with SPI flash the number for _sflash_fs will be 0.
That does not matter since these devices do not use the MCU flash file
system.
*/

_oflash_fs = ORIGIN(FLASH) + _codesize;
_sflash_fs = _flashsize - _codesize - _bootloader - _vfsromsize;

_oflash_vfsrom = ORIGIN(FLASH) + _flashsize - _vfsromsize - _bootloader;
_sflash_vfsrom = _vfsromsize;

_sheap = _ebss;
_eheap = _sstack;
