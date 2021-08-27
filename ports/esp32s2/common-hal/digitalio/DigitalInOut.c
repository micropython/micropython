/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2020 Scott Shawcroft for Adafruit Industries
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
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "components/driver/include/driver/gpio.h"

#include "components/hal/include/hal/gpio_hal.h"

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;

    gpio_config_t config;
    config.pin_bit_mask = 1ull << pin->number;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    if (gpio_config(&config) != ESP_OK) {
        return DIGITALINOUT_PIN_BUSY;
    }

    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

void common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    gpio_set_direction(self->pin->number, GPIO_MODE_DEF_INPUT);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    common_hal_digitalio_digitalinout_set_value(self, value);
    return common_hal_digitalio_digitalinout_set_drive_mode(self, drive_mode);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    uint32_t iomux = READ_PERI_REG(GPIO_PIN_MUX_REG[self->pin->number]);
    if ((iomux & FUN_IE) != 0) {
        return DIRECTION_INPUT;
    }
    return DIRECTION_OUTPUT;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    self->output_value = value;
    gpio_set_level(self->pin->number, value);
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_INPUT) {
        return gpio_get_level(self->pin->number) == 1;
    }
    return self->output_value;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    gpio_num_t number = self->pin->number;
    gpio_mode_t mode;
    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        mode = GPIO_MODE_DEF_OD;
    } else {
        mode = GPIO_MODE_DEF_OUTPUT;
    }
    esp_err_t result = gpio_set_direction(number, mode);
    if (result != ESP_OK) {
        return DIGITALINOUT_INPUT_ONLY;
    }
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    if (GPIO_HAL_GET_HW(GPIO_PORT_0)->pin[self->pin->number].pad_driver == 1) {
        return DRIVE_MODE_OPEN_DRAIN;
    }
    return DRIVE_MODE_PUSH_PULL;
}

void common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    gpio_num_t number = self->pin->number;
    gpio_pullup_dis(number);
    gpio_pulldown_dis(number);
    if (pull == PULL_UP) {
        gpio_pullup_en(number);
    } else if (pull == PULL_DOWN) {
        gpio_pulldown_en(number);
    }
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    gpio_num_t gpio_num = self->pin->number;
    if (REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU) == 1) {
        return PULL_UP;
    } else if (REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD) == 1) {
        return PULL_DOWN;
    }
    return PULL_NONE;
}
