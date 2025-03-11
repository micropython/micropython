/* GNU linker script for ESP8266 with 2M or more flash, and includes a ROMFS partition

   Flash layout:
    0x40200000 36k  header + iram/dram init
    0x40209000 668k firmware (irom0)
    0x402c0000 320k ROMFS
    0x40300000 1M+  filesystem (not memory mapped)
*/

MEMORY
{
    dport0_0_seg : org = 0x3ff00000, len = 16
    dram0_0_seg :  org = 0x3ffe8000, len = 80K
    iram1_0_seg :  org = 0x40100000, len = 32K
    irom0_0_seg :  org = 0x40209000, len = 1M - 36K - 320K
    FLASH_ROMFS :  org = 0x402b0000, len = 320K
}

/* define ROMFS extents */
_micropy_hw_romfs_part0_start = ORIGIN(FLASH_ROMFS);
_micropy_hw_romfs_part0_size = LENGTH(FLASH_ROMFS);

/* define common sections and symbols */
INCLUDE boards/esp8266_common.ld
