// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/sdioio/SDCard.h"
#include "common-hal/sdioio/sdfat_pio/shim.h"

#include "extmod/vfs.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "shared-bindings/util.h"
#include "py/mperrno.h"
#include "py/runtime.h"

// Maximum SD clock the PIO driver is allowed to be asked for. At a typical
// 150 MHz clk_sys the driver tops out near 37.5 MHz (clkDiv == 1); the cap is
// generous and the achieved rate is reported back through the `frequency`
// property.
#define SDIO_MAX_FREQUENCY (50000000)

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency) {
    // The vendored PIO driver only supports 4-bit mode and requires the four
    // data lines to be on consecutive GPIOs (DAT0..DAT3).
    if (num_data != 4) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Number of data_pins must be %d, not %d"), 4, num_data);
    }
    for (size_t i = 1; i < num_data; i++) {
        if (data[i]->number != data[0]->number + i) {
            mp_raise_ValueError(MP_ERROR_TEXT("Data pins must be consecutive"));
        }
    }

    mp_arg_validate_int_max(frequency, SDIO_MAX_FREQUENCY, MP_QSTR_frequency);

    self->num_data = num_data;
    self->clock = clock->number;
    self->command = command->number;
    for (size_t i = 0; i < num_data; i++) {
        self->data[i] = data[i]->number;
    }

    claim_pin(clock);
    claim_pin(command);
    for (size_t i = 0; i < num_data; i++) {
        claim_pin(data[i]);
    }

    sdfat_pio_card_new(&self->card);
    uint32_t actual_frequency = 0;
    bool ok = sdfat_pio_card_begin(&self->card, clock->number, command->number,
        data[0]->number, frequency, &actual_frequency);
    if (!ok) {
        // The driver's error code identifies the SD command/phase that failed
        // (see SdCardInfo.h SD_CARD_ERROR_* codes).
        uint8_t error_code = sdfat_pio_card_error_code(&self->card);
        sdfat_pio_card_end(&self->card);
        sdfat_pio_card_free(&self->card);
        reset_pin_number(self->clock);
        reset_pin_number(self->command);
        for (size_t i = 0; i < num_data; i++) {
            reset_pin_number(self->data[i]);
        }
        self->command = COMMON_HAL_MCU_NO_PIN;
        mp_raise_OSError_msg_varg(
            MP_ERROR_TEXT("SDIO Init Error 0x%02x"), error_code);
    }

    self->frequency = actual_frequency;
    self->capacity = sdfat_pio_card_sector_count(&self->card);
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->capacity;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->num_data;
}

static void check_for_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
}

static void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), 512);
    }
}

// Native function for the VFS blockdev layer. The PIO driver is polling, so
// these block until the transfer completes. Writes leave the multi-block
// transfer open until the next sync.
mp_negative_errno_t sdioio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!sdfat_pio_card_read_sectors(&self->card, start_block, buf, num_blocks)) {
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t sdioio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!sdfat_pio_card_write_sectors(&self->card, start_block, buf, num_blocks)) {
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    uint32_t num_blocks = bufinfo->len / 512;
    return sdioio_sdcard_readblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

mp_negative_errno_t common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    uint32_t num_blocks = bufinfo->len / 512;
    return sdioio_sdcard_writeblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

// Native function for VFS blockdev layer.
bool sdioio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg,
    mp_int_t *out_value) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    *out_value = 0;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            // The driver keeps multi-block writes open between calls, so end
            // the transfer and wait for the card to finish programming.
            return sdfat_pio_card_sync(&self->card);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            *out_value = common_hal_sdioio_sdcard_get_count(self);
            return true;

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = 512;  // SD cards use 512-byte sectors.
            return true;

        default:
            return false;  // Unsupported command.
    }
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    // Only 4-bit mode is supported (see construct); reject a request for any
    // other width. Reconfiguring the clock at runtime is not implemented yet,
    // so the frequency argument is accepted but ignored.
    if (bits != 0 && bits != self->num_data) {
        return false;
    }
    return true;
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->command == COMMON_HAL_MCU_NO_PIN;
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }

    // Commit any open write before the PIO is released.
    sdfat_pio_card_sync(&self->card);
    sdfat_pio_card_end(&self->card);
    sdfat_pio_card_free(&self->card);

    reset_pin_number(self->command);
    self->command = COMMON_HAL_MCU_NO_PIN;
    reset_pin_number(self->clock);
    self->clock = COMMON_HAL_MCU_NO_PIN;
    for (size_t i = 0; i < self->num_data; i++) {
        reset_pin_number(self->data[i]);
        self->data[i] = COMMON_HAL_MCU_NO_PIN;
    }
}
