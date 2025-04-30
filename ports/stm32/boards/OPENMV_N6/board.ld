/*
    Linker script for OPENMV_N6.
*/

MEMORY
{
    EXT_FLASH_ROMFS (rx) : ORIGIN = 0x70200000, LENGTH = 14M
}

/* ROMFS location */
_micropy_hw_romfs_part0_start = ORIGIN(EXT_FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(EXT_FLASH_ROMFS);
