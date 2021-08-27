/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

// The HAL is sparse on obtaining register information, so we use the LLs here.
#if (CPY_STM32H7)
#include "stm32h7xx_ll_gpio.h"
#elif (CPY_STM32F7)
#include "stm32f7xx_ll_gpio.h"
#elif (CPY_STM32F4)
#include "stm32f4xx_ll_gpio.h"
#endif

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->port, self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {

    common_hal_mcu_pin_claim(pin);
    self->pin = pin;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(self->pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin_port(self->pin->port), &GPIO_InitStruct);

    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    reset_pin_number(self->pin->port, self->pin->number);
    self->pin = NULL;
}

void common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(self->pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin_port(self->pin->port), &GPIO_InitStruct);

    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {

    common_hal_digitalio_digitalinout_set_drive_mode(self, drive_mode);
    common_hal_digitalio_digitalinout_set_value(self, value);
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {

    return (LL_GPIO_GetPinMode(pin_port(self->pin->port), pin_mask(self->pin->number))
        == LL_GPIO_MODE_INPUT) ? DIRECTION_INPUT : DIRECTION_OUTPUT;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    HAL_GPIO_WritePin(pin_port(self->pin->port), pin_mask(self->pin->number), value);
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    return (LL_GPIO_GetPinMode(pin_port(self->pin->port), pin_mask(self->pin->number)) == LL_GPIO_MODE_INPUT)
        ? HAL_GPIO_ReadPin(pin_port(self->pin->port), pin_mask(self->pin->number))
        : LL_GPIO_IsOutputPinSet(pin_port(self->pin->port), pin_mask(self->pin->number));
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(self->pin->number);
    GPIO_InitStruct.Mode = (drive_mode == DRIVE_MODE_OPEN_DRAIN ?
        GPIO_MODE_OUTPUT_OD : GPIO_MODE_OUTPUT_PP);
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin_port(self->pin->port), &GPIO_InitStruct);
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {

    return LL_GPIO_GetPinOutputType(pin_port(self->pin->port), pin_mask(self->pin->number))
           == LL_GPIO_OUTPUT_OPENDRAIN ? DRIVE_MODE_OPEN_DRAIN : DRIVE_MODE_PUSH_PULL;
}

void common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {

    switch (pull) {
        case PULL_UP:
            LL_GPIO_SetPinPull(pin_port(self->pin->port), pin_mask(self->pin->number),LL_GPIO_PULL_UP);
            break;
        case PULL_DOWN:
            LL_GPIO_SetPinPull(pin_port(self->pin->port), pin_mask(self->pin->number),LL_GPIO_PULL_DOWN);
            break;
        case PULL_NONE:
            LL_GPIO_SetPinPull(pin_port(self->pin->port), pin_mask(self->pin->number),LL_GPIO_PULL_NO);
            break;
        default:
            break;
    }
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {

    switch (LL_GPIO_GetPinPull(pin_port(self->pin->port), pin_mask(self->pin->number))) {
        case LL_GPIO_PULL_UP:
            return PULL_UP;
        case LL_GPIO_PULL_DOWN:
            return PULL_DOWN;
        case LL_GPIO_PULL_NO:
            return PULL_NONE;
        default:
            return PULL_NONE;
    }
}
