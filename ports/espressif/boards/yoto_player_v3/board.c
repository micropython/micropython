// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "board.h"

#include "supervisor/board.h"

#include "extmod/vfs_fat.h"

#include "py/mpstate.h"

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/i2cioexpander/IOExpander.h"
#include "shared-bindings/i2cioexpander/IOPin.h"
#include "shared-bindings/sdioio/SDCard.h"

#include "supervisor/filesystem.h"

static sdioio_sdcard_obj_t sdmmc;
static mp_vfs_mount_t _sdcard_vfs;
static fs_user_mount_t _sdcard_usermount;
static i2cioexpander_ioexpander_obj_t ioexpander0;  // First chip (p0/p1)
static i2cioexpander_ioexpander_obj_t ioexpander1;  // Second chip (p2/p3)

void board_init(void) {
    // Wait for everything to start
    mp_hal_delay_ms(300);

    // Initialize the board's peripherals here.
    busio_i2c_obj_t *i2c = MP_OBJ_TO_PTR(common_hal_board_create_i2c(0));
    // The IO expander pins stay in use after code.py ends, so keep the I2C pins
    // from being reset with the others.
    common_hal_busio_i2c_never_reset(i2c);

    // Initialize the IOExpanders
    // V3/V3-E uses two PI4IOE5V6416 chips (16 pins each)
    // First chip (0x20): p0 (pins 0-7), p1 (pins 8-15)
    ioexpander0.base.type = &i2cioexpander_ioexpander_type;
    common_hal_i2cioexpander_ioexpander_construct(
        &ioexpander0,
        MP_OBJ_FROM_PTR(i2c),
        0x20,  // I2C address for first chip
        16,    // Number of pins
        2,     // Output register
        0,     // Input register
        6);    // Direction register

    // Second chip (0x21): p2 (pins 0-7), p3 (pins 8-15)
    ioexpander1.base.type = &i2cioexpander_ioexpander_type;
    common_hal_i2cioexpander_ioexpander_construct(
        &ioexpander1,
        MP_OBJ_FROM_PTR(i2c),
        0x21,  // I2C address for second chip
        16,    // Number of pins
        2,     // Output register
        0,     // Input register
        6);    // Direction register

    board_set(MP_QSTR_IOEXPANDER0, MP_OBJ_FROM_PTR(&ioexpander0));
    board_set(MP_QSTR_IOEXPANDER1, MP_OBJ_FROM_PTR(&ioexpander1));

    // Battery and charging (from first chip - p0/p1)
    board_set(MP_QSTR_BATTERY_ALERT, ioexpander0.pins->items[6]);      // IOX.0.6
    board_set(MP_QSTR_QI_STATUS, ioexpander0.pins->items[7]);          // IOX.0.7
    board_set(MP_QSTR_QI_I2C_INT, ioexpander0.pins->items[0]);         // IOX.0.0
    board_set(MP_QSTR_USB_STATUS, ioexpander0.pins->items[8 + 0]);     // IOX.1.0
    board_set(MP_QSTR_CHARGE_STATUS, ioexpander0.pins->items[8 + 4]);  // IOX.1.4

    // Buttons (from first chip - p0/p1)
    board_set(MP_QSTR_POWER_BUTTON, ioexpander0.pins->items[8 + 3]);   // IOX.1.3
    board_set(MP_QSTR_ENC1_BUTTON, ioexpander0.pins->items[5]);        // IOX.0.5
    board_set(MP_QSTR_ENC2_BUTTON, ioexpander0.pins->items[4]);        // IOX.0.4

    // Audio (from first chip - p0/p1 and second chip - p2/p3)
    board_set(MP_QSTR_HEADPHONE_DETECT, ioexpander0.pins->items[8 + 1]); // IOX.1.1
    board_set(MP_QSTR_PACTRL, ioexpander1.pins->items[4]);             // IOX.2.4

    // Display - V3/V3-E uses ht16d35x with 4 CS lines (from second chip - p2)
    board_set(MP_QSTR_DISPLAY_CS0, ioexpander1.pins->items[0]);        // IOX.2.0
    board_set(MP_QSTR_DISPLAY_CS1, ioexpander1.pins->items[1]);        // IOX.2.1
    board_set(MP_QSTR_DISPLAY_CS2, ioexpander1.pins->items[2]);        // IOX.2.2
    board_set(MP_QSTR_DISPLAY_CS3, ioexpander1.pins->items[3]);        // IOX.2.3

    // Power control (from second chip - p2/p3)
    board_set(MP_QSTR_LEVEL_CONVERTER, ioexpander1.pins->items[8 + 0]); // IOX.3.0
    board_set(MP_QSTR_LEVEL_POWER_ENABLE, ioexpander1.pins->items[5]); // IOX.2.5
    board_set(MP_QSTR_LEVEL_VINHOLD, ioexpander1.pins->items[8 + 1]);  // IOX.3.1
    board_set(MP_QSTR_LEVEL_VOUTEN, ioexpander1.pins->items[8 + 3]);   // IOX.3.3

    // Sensors (from first chip - p0/p1)
    board_set(MP_QSTR_TILT, ioexpander0.pins->items[8 + 2]);           // IOX.1.2
    board_set(MP_QSTR_RTC_INT, ioexpander0.pins->items[1]);            // IOX.0.1

    // Qi charging control (V3-E, from second chip - p2/p3)
    board_set(MP_QSTR_QI_CHARGE_ENABLE, ioexpander1.pins->items[6]);   // IOX.2.6
    board_set(MP_QSTR_USB_CHARGE_ENABLE, ioexpander1.pins->items[7]);  // IOX.2.7
    board_set(MP_QSTR_QI_ENABLE_5W, ioexpander1.pins->items[8 + 5]);   // IOX.3.5

    // Output pin 3 high. Not clear why.
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander0.pins->items[3]), true, DRIVE_MODE_PUSH_PULL);

    // Initialize output pins
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[0]), true, DRIVE_MODE_PUSH_PULL);        // DISPLAY_CS0 (IOX.2.0)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[1]), true, DRIVE_MODE_PUSH_PULL);        // DISPLAY_CS1 (IOX.2.1)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[2]), true, DRIVE_MODE_PUSH_PULL);        // DISPLAY_CS2 (IOX.2.2)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[3]), true, DRIVE_MODE_PUSH_PULL);        // DISPLAY_CS3 (IOX.2.3)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[4]), true, DRIVE_MODE_PUSH_PULL);      // PACTRL (IOX.2.4)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[5]), false, DRIVE_MODE_PUSH_PULL);      // LEVEL_POWER_ENABLE (IOX.2.5)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[6]), true, DRIVE_MODE_PUSH_PULL);       // QI_CHARGE_ENABLE
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[7]), false, DRIVE_MODE_PUSH_PULL); // USB_CHARGE_ENABLE
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[8 + 0]), true, DRIVE_MODE_PUSH_PULL);  // LEVEL_CONVERTER (IOX.3.0)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[8 + 1]), true, DRIVE_MODE_PUSH_PULL);      // VINHOLD (IOX.3.1)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[8 + 3]), true, DRIVE_MODE_PUSH_PULL);      // VOUTEN (IOX.3.3)
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander1.pins->items[8 + 5]), false, DRIVE_MODE_PUSH_PULL);      // QI_ENABLE_5W (IOX.3.5)

    // Initialize SD card
    // V3/V3-E uses single-line SD (sd1 mode), not 4-line (sd4 mode) like Mini
    sdmmc.base.type = &sdioio_SDCard_type;
    const mcu_pin_obj_t *data_pins[1] = {MP_ROM_PTR(&pin_GPIO2)};
    common_hal_sdioio_sdcard_construct(&sdmmc, MP_ROM_PTR(&pin_GPIO14), MP_ROM_PTR(&pin_GPIO15), 1, data_pins, 25 * 1000000);

    fs_user_mount_t *vfs = &_sdcard_usermount;
    vfs->base.type = &mp_fat_vfs_type;
    vfs->fatfs.drv = vfs;

    // Initialise underlying block device
    vfs->blockdev.block_size = FF_MIN_SS; // default, will be populated by call to MP_BLOCKDEV_IOCTL_BLOCK_SIZE
    mp_vfs_blockdev_init(&vfs->blockdev, &sdmmc);

    // mount the block device so the VFS methods can be used
    FRESULT res = f_mount(&vfs->fatfs);
    if (res != FR_OK) {
        common_hal_sdioio_sdcard_deinit(&sdmmc);
        return;
    }
    common_hal_sdioio_sdcard_never_reset(&sdmmc);

    filesystem_set_concurrent_write_protection((supervisor_vfs_t *)vfs, true);
    filesystem_set_writable_by_usb((supervisor_vfs_t *)vfs, false);

    mp_vfs_mount_t *sdcard_vfs = &_sdcard_vfs;
    sdcard_vfs->str = "/sd";
    sdcard_vfs->len = 3;
    sdcard_vfs->obj = MP_OBJ_FROM_PTR(&_sdcard_usermount);
    sdcard_vfs->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = sdcard_vfs;
}
