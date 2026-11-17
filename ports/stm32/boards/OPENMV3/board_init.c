#include "py/mphal.h"
#define OMV_BOOTLOADER_MAGIC_ADDR   (0x2001FFFCU)
#define OMV_BOOTLOADER_MAGIC_VALUE  (0xB00710ADU)

void board_enter_bootloader(void) {
    *((uint32_t *)OMV_BOOTLOADER_MAGIC_ADDR) = OMV_BOOTLOADER_MAGIC_VALUE;
    NVIC_SystemReset();
}
