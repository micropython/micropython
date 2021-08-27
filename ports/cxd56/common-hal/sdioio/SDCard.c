/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <fcntl.h>
#include <unistd.h>
#include <arch/chip/pin.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/sdioio/SDCard.h"
#include "shared-bindings/util.h"

#define DATA_PINS_NUM 4

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, mcu_pin_obj_t **data, uint32_t frequency) {
    struct geometry geo;

    if (clock->number != PIN_SDIO_CLK || command->number != PIN_SDIO_CMD) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    uint8_t data_pins_num = 0;
    for (uint8_t i = 0; i < DATA_PINS_NUM; i++) {
        if (data[i]->number != PIN_SDIO_DATA0 || data[i]->number != PIN_SDIO_DATA1 ||
            data[i]->number != PIN_SDIO_DATA2 || data[i]->number != PIN_SDIO_DATA3) {
            data_pins_num++;
        }
    }

    if (data_pins_num != DATA_PINS_NUM) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    if (open_blockdriver("/dev/mmcsd0", 0, &self->inode) < 0) {
        mp_raise_ValueError(translate("Could not initialize SDCard"));
    }

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

STATIC void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError(translate("Buffer length must be a multiple of 512"));
    }
}

int common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
    check_whole_block(bufinfo);

    return self->inode->u.i_bops->read(self->inode, bufinfo->buf, start_block, bufinfo->len / 512);
}

int common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
    check_whole_block(bufinfo);

    return self->inode->u.i_bops->write(self->inode, bufinfo->buf, start_block, bufinfo->len / 512);
    ;
}

void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self) {
    never_reset_pin_number(self->clock_pin->number);
    never_reset_pin_number(self->command_pin->number);
    for (uint8_t i = 0; i < DATA_PINS_NUM; i++) {
        never_reset_pin_number(self->data_pins[i]->number);
    }
}
