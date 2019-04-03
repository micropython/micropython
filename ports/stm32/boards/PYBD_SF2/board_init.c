#include "py/mphal.h"
#include "storage.h"

void mboot_board_early_init(void) {
    #ifdef pyb_pin_W23
    // Enable 500mA on WBUS-DIP28
    mp_hal_pin_config(pyb_pin_W23, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    #endif
}

void board_early_init(void) {
    #ifdef pyb_pin_EN_3V3
    // Turn on 3.3V
    mp_hal_pin_output(pyb_pin_EN_3V3);
    mp_hal_pin_high(pyb_pin_EN_3V3);
    #endif

    #ifndef MICROPY_HW_BDEV2_IOCTL
    // Init SPI2 if it's not enabled as a block device
    spi_bdev_ioctl(&spi_bdev2, BDEV_IOCTL_INIT, (uint32_t)&spiflash2_config);
    #endif
}
