/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdint.h>
#include <string.h>

#include "shared-bindings/paralleldisplay/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "common-hal/audiobusio/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/runtime.h"

#include "driver/gpio.h"

/*
 *  Current pin limitations for ESP32-S2 ParallelBus:
 *   - data0 pin must be byte aligned
 */

void common_hal_paralleldisplay_parallelbus_construct_nonsequential(paralleldisplay_parallelbus_obj_t *self,
    uint8_t n_pins, const mcu_pin_obj_t **data_pins,
    const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {

    if (n_pins != 8 && n_pins != 16) {
        mp_raise_ValueError_varg(translate("Number of data_pins must be 8 or 16, not %d"), n_pins);
    }

    for (uint8_t i = 0; i < n_pins; i++) {
        if (!common_hal_mcu_pin_is_free(data_pins[i])) {
            mp_raise_ValueError_varg(translate("Bus pin %d is already in use"), i);
        }
    }

    // This will throw if unsuccessful.  Everything following is guaranteed to succeed.
    port_i2s_allocate_i2s0();

    i2s_lcd_config_t config = {
        .data_width = n_pins,
        .pin_num_cs = common_hal_mcu_pin_number(chip_select),
        .pin_num_wr = common_hal_mcu_pin_number(write),   // write strobe
        .pin_num_rs = common_hal_mcu_pin_number(command), // The "register select" pin is called "command" by CircuitPython
        .clk_freq = frequency,
        .i2s_port = 0,
        .swap_data = false,
        .buffer_size = 512,
    };

    for (uint8_t i = 0; i < n_pins; i++) {
        common_hal_never_reset_pin(data_pins[i]);
        config.pin_data_num[i] = common_hal_mcu_pin_number(data_pins[i]);
    }

    if (read != NULL) {
        common_hal_never_reset_pin(read);
        gpio_config_t read_config = {
            .pin_bit_mask = 1ull << read->number,
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&read_config);
        self->read_pin_number = read->number;
        gpio_set_level(read->number, true);
    }

    self->reset_pin_number = NO_PIN;
    if (reset != NULL) {
        common_hal_never_reset_pin(reset);
        self->reset_pin_number = reset->number;
    }

    common_hal_never_reset_pin(chip_select);
    common_hal_never_reset_pin(command);
    common_hal_never_reset_pin(write);

    self->config = config;
    self->handle = i2s_lcd_driver_init(&config);

    if (!self->handle) {
        port_i2s_reset_instance(0);
        mp_raise_RuntimeError(translate("Internal error"));
    }
}


void common_hal_paralleldisplay_parallelbus_construct(paralleldisplay_parallelbus_obj_t *self,
    const mcu_pin_obj_t *data0, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {
    char buf[7];
    const mcu_pin_obj_t *data_pins[8];
    for (int i = 0; i < 8; i++) {
        snprintf(buf, sizeof(buf), "GPIO%d", data0->number + i);
        data_pins[i] = validate_obj_is_free_pin(
            mp_obj_dict_get(MP_OBJ_FROM_PTR(&mcu_pin_globals), mp_obj_new_str(buf, strlen(buf))), MP_QSTR_pin);
    }
    common_hal_paralleldisplay_parallelbus_construct_nonsequential(self, 8, data_pins, command, chip_select, write, read, reset, frequency);
}


void common_hal_paralleldisplay_parallelbus_deinit(paralleldisplay_parallelbus_obj_t *self) {
    if (!self->handle) {
        return;
    }
    i2s_lcd_driver_deinit(self->handle);
    self->handle = NULL;
    /* SNIP - same as from SAMD and NRF ports */
    for (uint8_t i = 0; i < self->config.data_width; i++) {
        reset_pin_number(self->config.pin_data_num[i]);
    }

    reset_pin_number(self->config.pin_num_cs);
    reset_pin_number(self->config.pin_num_wr);
    reset_pin_number(self->config.pin_num_rs);
    reset_pin_number(self->read_pin_number);
    reset_pin_number(self->reset_pin_number);

    port_i2s_reset_instance(0);
}

bool common_hal_paralleldisplay_parallelbus_reset(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    paralleldisplay_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset_pin_number == NO_PIN) {
        return false;
    }

    gpio_set_level(self->reset_pin_number, false);
    common_hal_mcu_delay_us(4);
    gpio_set_level(self->reset_pin_number, true);
    return true;

}

bool common_hal_paralleldisplay_parallelbus_bus_free(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    return true;
}

bool common_hal_paralleldisplay_parallelbus_begin_transaction(mp_obj_t obj) {
    paralleldisplay_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    bool lock_acquired = false;
    i2s_lcd_acquire_nonblocking(self->handle, 1, &lock_acquired);
    if (lock_acquired) {
        gpio_set_level(self->config.pin_num_cs, false);
    }
    return lock_acquired;
}

void common_hal_paralleldisplay_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    paralleldisplay_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (data_length) {
        gpio_set_level(self->config.pin_num_rs, byte_type == DISPLAY_DATA);
        i2s_lcd_write(self->handle, data, data_length);
    }
}

void common_hal_paralleldisplay_parallelbus_end_transaction(mp_obj_t obj) {
    paralleldisplay_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    i2s_lcd_release(self->handle);
    gpio_set_level(self->config.pin_num_cs, true);
}
