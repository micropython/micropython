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
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/i2cioexpander/IOExpander.h"
#include "shared-bindings/i2cioexpander/IOPin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#include "supervisor/filesystem.h"

static sdioio_sdcard_obj_t sdmmc;
static mp_vfs_mount_t _sdcard_vfs;
static fs_user_mount_t _sdcard_usermount;
static i2cioexpander_ioexpander_obj_t ioexpander;

#define DELAY 0x80

// This is a GC9306.
uint8_t display_init_sequence[] = {
    0xfe, 0,
    0xef, 0,

    // // sw reset
    // 0x01, 0 | DELAY, 150,
    // normal display mode on
    // 0x13, 0,
    // display and color format settings
    0x36, 1, 0x48, // Memory access control. mini does 0x48, not 0, 2, 3, 4 or 6
    0x3A, 1 | DELAY,  0x55, 10, // COLMOD. mini does 0x06
    0xa4, 2, 0x44, 0x44, // power control 7
    0xa5, 2, 0x42, 0x42,
    0xaa, 2, 0x88, 0x88,
    0xae, 1, 0x2b,
    0xe8, 2, 0x22, 0x0b, // frame rate
    0xe3, 2, 0x01, 0x10,
    0xff, 1, 0x61,
    0xac, 1, 0x00,
    0xad, 1, 0x33,
    0xaf, 1, 0x77,
    0xa6, 2, 0x1c, 0x1c, // power control 2
    0xa7, 2, 0x1c, 0x1c, // power control 3
    0xa8, 2, 0x10, 0x10, // power control 4
    0xa9, 2, 0x0d, 0x0d, // power control 5
    0xf0, 6, 0x02, 0x01, 0x00, 0x00, 0x00, 0x05, // Gamma settings
    0xf1, 6, 0x01, 0x02, 0x00, 0x06, 0x10, 0x0e,
    0xf2, 6, 0x03, 0x11, 0x28, 0x02, 0x00, 0x48,
    0xf3, 6, 0x0c, 0x11, 0x30, 0x00, 0x00, 0x46,
    0xf4, 6, 0x05, 0x1f, 0x1f, 0x36, 0x30, 0x0f,
    0xf5, 6, 0x04, 0x1d, 0x1a, 0x38, 0x3f, 0x0f, // Last gamma setting
    0x35, 1, 0x00,
    0x44, 2, 0x00, 0x0a, // set tear scan line
    0x21, 0, // display inversion on
    // sleep out
    0x11, 0 | DELAY, 255,

    // display on
    0x29, 0 | DELAY, 255,
};

void board_init(void) {
    // Wait for everything to start
    mp_hal_delay_ms(300);

    // Initialize the board's peripherals here.
    busio_i2c_obj_t *i2c = MP_OBJ_TO_PTR(common_hal_board_create_i2c(0));
    // The IO expander pins stay in use after code.py ends, so keep the I2C pins
    // from being reset with the others.
    common_hal_busio_i2c_never_reset(i2c);

    // Initialize the IOExpander
    ioexpander.base.type = &i2cioexpander_ioexpander_type;
    common_hal_i2cioexpander_ioexpander_construct(
        &ioexpander,
        MP_OBJ_FROM_PTR(i2c),
        0x20,  // I2C address
        16,    // Number of pins
        2,     // Output register
        0,     // Input register
        6);    // Direction register

    board_set(MP_QSTR_IOEXPANDER, MP_OBJ_FROM_PTR(&ioexpander));
    board_set(MP_QSTR_PLUG_STATUS, ioexpander.pins->items[8 + 5]);
    board_set(MP_QSTR_CHARGE_STATUS, ioexpander.pins->items[8 + 7]);
    board_set(MP_QSTR_POWER_BUTTON, ioexpander.pins->items[8 + 3]);
    board_set(MP_QSTR_ENC1_BUTTON, ioexpander.pins->items[5]);
    board_set(MP_QSTR_ENC2_BUTTON, ioexpander.pins->items[4]);
    board_set(MP_QSTR_HEADPHONE_DETECT, ioexpander.pins->items[8 + 1]);
    board_set(MP_QSTR_PACTRL, ioexpander.pins->items[6]);
    board_set(MP_QSTR_DISPLAY_CS, ioexpander.pins->items[0]);
    board_set(MP_QSTR_DISPLAY_DC, ioexpander.pins->items[1]);
    board_set(MP_QSTR_DISPLAY_RESET, ioexpander.pins->items[2]);

    board_set(MP_QSTR_LEVEL_CONVERTER, ioexpander.pins->items[3]);
    board_set(MP_QSTR_LEVEL_POWER_ENABLE, ioexpander.pins->items[8 + 4]);
    board_set(MP_QSTR_LEVEL_VINHOLD, ioexpander.pins->items[8 + 6]);

    board_set(MP_QSTR_TILT, ioexpander.pins->items[8 + 2]);

    // Only on some variants
    board_set(MP_QSTR_RTC_INT, ioexpander.pins->items[7]);

    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander.pins->items[2]), true, DRIVE_MODE_PUSH_PULL);
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander.pins->items[3]), true, DRIVE_MODE_PUSH_PULL);
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander.pins->items[6]), true, DRIVE_MODE_PUSH_PULL);
    common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander.pins->items[14]), true, DRIVE_MODE_PUSH_PULL);

    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        ioexpander.pins->items[1],    // DC
        ioexpander.pins->items[0],     // CS
        ioexpander.pins->items[2],    // RST
        25000000,       // baudrate
        0,              // polarity
        0               // phase
        );
    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width (after rotation)
        240,            // height (after rotation)
        0,             // column start
        0,             // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        true,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO26,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );

    digitalinout_result_t sd_enable_res = common_hal_i2cioexpander_iopin_switch_to_output(MP_OBJ_TO_PTR(ioexpander.pins->items[12]), false, DRIVE_MODE_PUSH_PULL);
    if (sd_enable_res != DIGITALINOUT_OK) {
        mp_printf(&mp_plat_print, "Failed to initialize IOExpander. Skipping SD card\n");
        return;
    }

    sdmmc.base.type = &sdioio_SDCard_type;
    const mcu_pin_obj_t *data_pins[4] = {MP_ROM_PTR(&pin_GPIO2), MP_ROM_PTR(&pin_GPIO4), MP_ROM_PTR(&pin_GPIO12), MP_ROM_PTR(&pin_GPIO13)};
    common_hal_sdioio_sdcard_construct(&sdmmc, MP_ROM_PTR(&pin_GPIO14), MP_ROM_PTR(&pin_GPIO15), 4, data_pins, 25 * 1000000);

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
