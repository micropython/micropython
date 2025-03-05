/*
    GNU linker script for SAMD51
*/

/*
_codesize and _vfsromsize are defined in mpconfigmcu.mk or mpconfigboard.mk
as MICROPY_HW_CODESIZE and MICROPY_HW_VFSROMSIZE and are set in Makefile.
*/

_flashsize = 512K;  /* The physical flash size */
_bootloader = 16K;  /* Must match the ORIGIN value of FLASH */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00004000, LENGTH = _codesize
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 192K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 16K;

/*
The VfsROM file system is placed at the end of the flash.
For device with SPI flash the number for _sflash_fs might be 0.
That does not matter since these devices do not use the MCU flash file
system.
*/

_oflash_fs = ORIGIN(FLASH) + _codesize;
_sflash_fs = _flashsize - _codesize - _bootloader - _vfsromsize;

_oflash_vfsrom = ORIGIN(FLASH) + _flashsize - _vfsromsize - _bootloader;
_sflash_vfsrom = _vfsromsize;

_sheap = _ebss;
_eheap = _sstack;
