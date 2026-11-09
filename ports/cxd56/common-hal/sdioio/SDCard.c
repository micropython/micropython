// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <fcntl.h>
#include <unistd.h>
#include <arch/chip/pin.h>

#include "extmod/vfs.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "shared-bindings/util.h"

#define DATA_PINS_NUM 4

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency) {
    struct geometry geo;

    self->inode = NULL;

    if (clock->number != PIN_SDIO_CLK || command->number != PIN_SDIO_CMD) {
        raise_ValueError_invalid_pins();
    }

    // Only 4-bit mode on the four SDIO data pins, in order.
    static const uint8_t data_pin_numbers[DATA_PINS_NUM] = {
        PIN_SDIO_DATA0, PIN_SDIO_DATA1, PIN_SDIO_DATA2, PIN_SDIO_DATA3,
    };
    if (num_data != DATA_PINS_NUM) {
        raise_ValueError_invalid_pins();
    }
    for (uint8_t i = 0; i < DATA_PINS_NUM; i++) {
        if (data[i]->number != data_pin_numbers[i]) {
            raise_ValueError_invalid_pins();
        }
    }

    struct inode *inode;
    if (open_blockdriver("/dev/mmcsd0", 0, &inode) < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("SDCard init"));
    }
    self->inode = inode;

    self->inode->u.i_bops->geometry(self->inode, &geo);

    claim_pin(clock);
    claim_pin(command);
    self->clock_pin = clock;
    self->command_pin = command;
    for (uint8_t i = 0; i < DATA_PINS_NUM; i++) {
        claim_pin(data[i]);
        self->data_pins[i] = data[i];
    }

    self->count = geo.geo_nsectors;
    self->frequency = frequency;
    self->width = num_data;
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }
    close_blockdriver(self->inode);
    self->inode = NULL;

    reset_pin_number(self->clock_pin->number);
    reset_pin_number(self->command_pin->number);
    for (uint8_t i = 0; i < DATA_PINS_NUM; i++) {
        reset_pin_number(self->data_pins[i]->number);
    }
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->inode == NULL;
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t baudrate, uint8_t width) {
    return true;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->width;
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->count;
}

static void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), 512);
    }
}

// Native function for VFS blockdev layer
mp_negative_errno_t sdioio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int result = self->inode->u.i_bops->read(self->inode, buf, start_block, num_blocks);
    if (result < 0) {
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
    check_whole_block(bufinfo);

    uint32_t num_blocks = bufinfo->len / 512;
    return sdioio_sdcard_readblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

// Native function for VFS blockdev layer
mp_negative_errno_t sdioio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t num_blocks) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int result = self->inode->u.i_bops->write(self->inode, buf, start_block, num_blocks);
    if (result < 0) {
        return -MP_EIO;
    }
    return 0;
}

mp_negative_errno_t common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
    check_whole_block(bufinfo);

    uint32_t num_blocks = bufinfo->len / 512;
    return sdioio_sdcard_writeblocks(MP_OBJ_FROM_PTR(self), bufinfo->buf,
        start_block, num_blocks);
}

// Native function for VFS blockdev layer
bool sdioio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg,
    mp_int_t *out_value) {
    sdioio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    *out_value = 0;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            // SDIO operations are synchronous, no action needed
            return true;

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            *out_value = common_hal_sdioio_sdcard_get_count(self);
            return true;

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = 512;  // SD cards use 512-byte sectors
            return true;

        default:
            return false;  // Unsupported command
    }
}
