/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2019 Damien P. George
 */

#include "py/mphal.h"
#include "storage.h"

void WeAct_Core_early_init(void) {
    // Turn off the USB switch.
    mp_hal_pin_output(pyb_pin_OTG_FS_POWER);
    mp_hal_pin_low(pyb_pin_OTG_FS_POWER);

    // Explicitly init SPI2 because it's not enabled as a block device
    spi_bdev_ioctl(&spi_bdev2, BDEV_IOCTL_INIT, (uint32_t)&spiflash2_config);
}
