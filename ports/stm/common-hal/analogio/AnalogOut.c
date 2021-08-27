/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019, Lucian Copeland for Adafruit Industries
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

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

#include "common-hal/microcontroller/Pin.h"

#include "stm32f4xx_hal.h"

// DAC is shared between both channels.
#if HAS_DAC
DAC_HandleTypeDef handle;
#endif

STATIC bool dac_on[2];

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    #if !(HAS_DAC)
    mp_raise_ValueError(translate("No DAC on chip"));
    #else
    if (pin == &pin_PA04) {
        self->channel = DAC_CHANNEL_1;
        self->dac_index = 0;
    } else if (pin == &pin_PA05) {
        self->channel = DAC_CHANNEL_2;
        self->dac_index = 1;
    } else {
        mp_raise_ValueError(translate("Invalid DAC pin supplied"));
    }

    // Only init if the shared DAC is empty or reset
    if (handle.Instance == NULL || handle.State == HAL_DAC_STATE_RESET) {
        __HAL_RCC_DAC_CLK_ENABLE();
        handle.Instance = DAC;
        if (HAL_DAC_Init(&handle) != HAL_OK) {
            mp_raise_ValueError(translate("DAC Device Init Error"));
        }
    }

    // init channel specific pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    self->ch_handle.DAC_Trigger = DAC_TRIGGER_NONE;
    self->ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&handle, &self->ch_handle, self->channel) != HAL_OK) {
        mp_raise_ValueError(translate("DAC Channel Init Error"));
    }

    dac_on[self->dac_index] = true;
    self->pin = pin;
    common_hal_mcu_pin_claim(pin);
    #endif
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return !dac_on[self->dac_index];
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    #if HAS_DAC
    reset_pin_number(self->pin->port,self->pin->number);
    self->pin = NULL;
    dac_on[self->dac_index] = false;

    // turn off the DAC if both channels are off
    if (dac_on[0] == false && dac_on[1] == false) {
        __HAL_RCC_DAC_CLK_DISABLE();
        HAL_DAC_DeInit(&handle);
    }
    #endif
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    #if HAS_DAC
    HAL_DAC_SetValue(&handle, self->channel, DAC_ALIGN_12B_R, value >> 4);
    HAL_DAC_Start(&handle, self->channel);
    #endif
}

void analogout_reset(void) {
    #if HAS_DAC
    __HAL_RCC_DAC_CLK_DISABLE();
    HAL_DAC_DeInit(&handle);
    #endif
}
