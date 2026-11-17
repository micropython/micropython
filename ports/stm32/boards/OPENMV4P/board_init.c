#include "py/mphal.h"
#include "storage.h"
#include "sdram.h"

#define OMV_BOOTLOADER_MAGIC_ADDR   (0x2001FFFCU)
#define OMV_BOOTLOADER_MAGIC_VALUE  (0xB00710ADU)

void board_low_power(int mode) {
    switch (mode) {
        case 0:     // Leave stop mode.
            sdram_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            sdram_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            sdram_enter_power_down();
            break;
    }
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
}

void board_enter_bootloader(void) {
    *((uint32_t *)OMV_BOOTLOADER_MAGIC_ADDR) = OMV_BOOTLOADER_MAGIC_VALUE;
    NVIC_SystemReset();
}
