/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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
#include <stdbool.h>

#include "shared-bindings/sdioio/SDCard.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "supervisor/board.h"
#include "supervisor/shared/translate.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

STATIC bool reserved_sdio[MP_ARRAY_SIZE(mcu_sdio_banks)];
STATIC bool never_reset_sdio[MP_ARRAY_SIZE(mcu_sdio_banks)];

STATIC const mcu_periph_obj_t *find_pin_function(const mcu_periph_obj_t *table, size_t sz, const mcu_pin_obj_t *pin, int periph_index) {
    for (size_t i = 0; i < sz; i++, table++) {
        if (periph_index == table->periph_index && pin == table->pin) {
            return table;
        }
    }
    return NULL;
}

// match pins to SDIO objects
STATIC int check_pins(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, mcu_pin_obj_t **data) {
    bool sdio_taken = false;

    const uint8_t sdio_clock_len = MP_ARRAY_SIZE(mcu_sdio_clock_list);
    const uint8_t sdio_command_len = MP_ARRAY_SIZE(mcu_sdio_command_list);
    const uint8_t sdio_data0_len = MP_ARRAY_SIZE(mcu_sdio_data0_list);
    const uint8_t sdio_data1_len = MP_ARRAY_SIZE(mcu_sdio_data1_list);
    const uint8_t sdio_data2_len = MP_ARRAY_SIZE(mcu_sdio_data2_list);
    const uint8_t sdio_data3_len = MP_ARRAY_SIZE(mcu_sdio_data3_list);


    // Loop over each possibility for clock.  Check whether all other pins can
    // be used on the same peripheral
    for (uint i = 0; i < sdio_clock_len; i++) {
        const mcu_periph_obj_t *mcu_sdio_clock = &mcu_sdio_clock_list[i];
        if (mcu_sdio_clock->pin != clock) {
            continue;
        }

        int periph_index = mcu_sdio_clock->periph_index;

        const mcu_periph_obj_t *mcu_sdio_command = NULL;
        if (!(mcu_sdio_command = find_pin_function(mcu_sdio_command_list, sdio_command_len, command, periph_index))) {
            continue;
        }

        const mcu_periph_obj_t *mcu_sdio_data0 = NULL;
        if (!(mcu_sdio_data0 = find_pin_function(mcu_sdio_data0_list, sdio_data0_len, data[0], periph_index))) {
            continue;
        }

        const mcu_periph_obj_t *mcu_sdio_data1 = NULL;
        if (num_data > 1 && !(mcu_sdio_data1 = find_pin_function(mcu_sdio_data1_list, sdio_data1_len, data[1], periph_index))) {
            continue;
        }

        const mcu_periph_obj_t *mcu_sdio_data2 = NULL;
        if (num_data > 2 && !(mcu_sdio_data2 = find_pin_function(mcu_sdio_data2_list, sdio_data2_len, data[2], periph_index))) {
            continue;
        }

        const mcu_periph_obj_t *mcu_sdio_data3 = NULL;
        if (num_data > 3 && !(mcu_sdio_data3 = find_pin_function(mcu_sdio_data3_list, sdio_data3_len, data[3], periph_index))) {
            continue;
        }

        if (reserved_sdio[periph_index - 1]) {
            sdio_taken = true;
            continue;
        }

        self->clock = mcu_sdio_clock;
        self->command = mcu_sdio_command;
        self->data[0] = mcu_sdio_data0;
        self->data[1] = mcu_sdio_data1;
        self->data[2] = mcu_sdio_data2;
        self->data[3] = mcu_sdio_data3;

        return periph_index;
    }

    if (sdio_taken) {
        mp_raise_ValueError(translate("Hardware busy, try alternative pins"));
    } else {
        mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_SDIO);
    }
}


void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, mcu_pin_obj_t **data, uint32_t frequency) {

    int periph_index = check_pins(self, clock, command, num_data, data);
    SDIO_TypeDef *SDIOx = mcu_sdio_banks[periph_index - 1];

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure data pins */
    for (int i = 0; i < num_data; i++) {
        GPIO_InitStruct.Pin = pin_mask(data[i]->number);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Alternate = self->data[i]->altfn_index;
        HAL_GPIO_Init(pin_port(data[i]->port), &GPIO_InitStruct);
    }

    /* Configure command pin */
    GPIO_InitStruct.Alternate = self->command->altfn_index;
    GPIO_InitStruct.Pin = pin_mask(command->number);
    HAL_GPIO_Init(pin_port(command->port), &GPIO_InitStruct);

    /* Configure clock */
    GPIO_InitStruct.Alternate = self->clock->altfn_index;
    GPIO_InitStruct.Pin = pin_mask(clock->number);
    HAL_GPIO_Init(pin_port(clock->port), &GPIO_InitStruct);

    __HAL_RCC_SDIO_CLK_ENABLE();

    self->handle.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;
    self->handle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    self->handle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    self->handle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    self->handle.Init.BusWide = SDIO_BUS_WIDE_1B;
    self->handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    self->handle.Instance = SDIOx;

    HAL_StatusTypeDef r = HAL_SD_Init(&self->handle);
    if (r != HAL_OK) {
        mp_raise_ValueError_varg(translate("SDIO Init Error %d"), (int)r);
    }

    HAL_SD_CardInfoTypeDef info;
    r = HAL_SD_GetCardInfo(&self->handle, &info);
    if (r != HAL_OK) {
        mp_raise_ValueError_varg(translate("SDIO GetCardInfo Error %d"), (int)r);
    }

    self->num_data = 1;
    if (num_data == 4) {
        if ((r = HAL_SD_ConfigWideBusOperation(&self->handle, SDIO_BUS_WIDE_4B)) == HAL_SD_ERROR_NONE) {
            self->handle.Init.BusWide = SDIO_BUS_WIDE_4B;
            self->num_data = 4;
        } else {
        }
    }

    self->capacity = info.BlockNbr * (info.BlockSize / 512);
    self->frequency = 25000000;

    reserved_sdio[periph_index - 1] = true;

    common_hal_mcu_pin_claim(clock);
    common_hal_mcu_pin_claim(command);
    for (int i = 0; i < num_data; i++) {
        common_hal_mcu_pin_claim(data[i]);
    }

    return;
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

STATIC void check_whole_block(mp_buffer_info_t *bufinfo) {
    if (bufinfo->len % 512) {
        mp_raise_ValueError(translate("Buffer must be a multiple of 512 bytes"));
    }
}

STATIC void wait_write_complete(sdioio_sdcard_obj_t *self) {
    if (self->state_programming) {
        HAL_SD_CardStateTypedef st = HAL_SD_CARD_PROGRAMMING;
        // This waits up to 60s for programming to complete.  This seems like
        // an extremely long time, but this is the timeout that micropython's
        // implementation uses
        for (int i = 0; i < 60000 && st == HAL_SD_CARD_PROGRAMMING; i++) {
            st = HAL_SD_GetCardState(&self->handle);
            HAL_Delay(1);
        };
        self->state_programming = false;
    }
}

STATIC void check_for_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        raise_deinited_error();
    }
}

int common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    wait_write_complete(self);
    self->state_programming = true;
    common_hal_mcu_disable_interrupts();
    HAL_StatusTypeDef r = HAL_SD_WriteBlocks(&self->handle, bufinfo->buf, start_block, bufinfo->len / 512, 1000);
    common_hal_mcu_enable_interrupts();
    if (r != HAL_OK) {
        return -EIO;
    }
    return 0;
}

int common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    check_for_deinit(self);
    check_whole_block(bufinfo);
    wait_write_complete(self);
    common_hal_mcu_disable_interrupts();
    HAL_StatusTypeDef r = HAL_SD_ReadBlocks(&self->handle, bufinfo->buf, start_block, bufinfo->len / 512, 1000);
    common_hal_mcu_enable_interrupts();
    if (r != HAL_OK) {
        return -EIO;
    }
    return 0;
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    check_for_deinit(self);
    return true;
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->command == NULL;
}

STATIC void never_reset_mcu_periph(const mcu_periph_obj_t *periph) {
    if (periph) {
        never_reset_pin_number(periph->pin->port,periph->pin->number);
    }
}

STATIC void reset_mcu_periph(const mcu_periph_obj_t *periph) {
    if (periph) {
        reset_pin_number(periph->pin->port,periph->pin->number);
    }
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }

    reserved_sdio[self->command->periph_index - 1] = false;
    never_reset_sdio[self->command->periph_index - 1] = false;

    reset_mcu_periph(self->command);
    self->command = NULL;

    reset_mcu_periph(self->clock);
    self->command = NULL;

    for (size_t i = 0; i < MP_ARRAY_SIZE(self->data); i++) {
        reset_mcu_periph(self->data[i]);
        self->data[i] = NULL;
    }
}

void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }

    if (never_reset_sdio[self->command->periph_index] - 1) {
        return;
    }

    never_reset_sdio[self->command->periph_index - 1] = true;

    never_reset_mcu_periph(self->command);
    never_reset_mcu_periph(self->clock);

    for (size_t i = 0; i < MP_ARRAY_SIZE(self->data); i++) {
        never_reset_mcu_periph(self->data[i]);
    }
}

void sdioio_reset() {
    for (size_t i = 0; i < MP_ARRAY_SIZE(reserved_sdio); i++) {
        if (!never_reset_sdio[i]) {
            reserved_sdio[i] = false;
        }
    }
}
