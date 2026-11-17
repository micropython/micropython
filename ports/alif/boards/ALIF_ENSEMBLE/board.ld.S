#include "mcu/ensemble.ld.S"

/* Define ROMFS partition locations. */
#if CORE_M55_HP
/* The HP core has access to the external OSPI1 flash and MRAM ROMFS partitions. */
_micropy_hw_romfs_part0_start = 0xc1000000;
_micropy_hw_romfs_part0_size = 16M;
_micropy_hw_romfs_part1_start = ORIGIN(MRAM_FS);
_micropy_hw_romfs_part1_size = LENGTH(MRAM_FS);
#else
/* The HP core has access to the MRAM ROMFS partition. */
_micropy_hw_romfs_part0_start = ORIGIN(MRAM_FS);
_micropy_hw_romfs_part0_size = LENGTH(MRAM_FS);
#endif
