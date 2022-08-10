/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/i2ctarget/I2CTarget.h"

#include "py/mperrno.h"
#include "py/runtime.h"

#include "common-hal/i2ctarget/I2CTarget.h"
#include "shared-bindings/microcontroller/Pin.h"

void common_hal_i2ctarget_i2c_target_construct(i2ctarget_i2c_target_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus) {
    // Pins 45 and 46 are "strapping" pins that impact start up behavior. They usually need to
    // be pulled-down so pulling them up for I2C is a bad idea. To make this hard, we don't
    // support I2C on these pins.
    // Also 46 is input-only so it'll never work.
    if (scl->number == 45 || scl->number == 46 || sda->number == 45 || sda->number == 46) {
        raise_ValueError_invalid_pins();
    }

    if (num_addresses > 1) {
        mp_raise_ValueError(translate("Only one address is allowed"));
    }
    self->addresses = addresses;
    self->num_addresses = num_addresses;

    self->sda_pin = sda;
    self->scl_pin = scl;
    self->i2c_num = peripherals_i2c_get_free_num();

    if (self->i2c_num == I2C_NUM_MAX) {
        mp_raise_ValueError(translate("All I2C peripherals are in use"));
    }

    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = self->sda_pin->number,
        .scl_io_num = self->scl_pin->number,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = self->addresses[0],
    };

    // Initialize I2C.
    esp_err_t err = peripherals_i2c_init(self->i2c_num, &i2c_conf);
    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            mp_raise_OSError(MP_EIO);
        } else {
            mp_arg_error_invalid(MP_QSTR_I2CTarget);
        }
    }

    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_i2ctarget_i2c_target_deinited(i2ctarget_i2c_target_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_i2ctarget_i2c_target_deinit(i2ctarget_i2c_target_obj_t *self) {
    if (common_hal_i2ctarget_i2c_target_deinited(self)) {
        return;
    }

    peripherals_i2c_deinit(self->i2c_num);

    common_hal_reset_pin(self->sda_pin);
    common_hal_reset_pin(self->scl_pin);
    self->sda_pin = NULL;
    self->scl_pin = NULL;
}

int common_hal_i2ctarget_i2c_target_is_addressed(i2ctarget_i2c_target_obj_t *self,
    uint8_t *address, bool *is_read, bool *is_restart) {
    *address = self->addresses[0];
    *is_read = true;
    *is_restart = false;
    return 1;
}

int common_hal_i2ctarget_i2c_target_read_byte(i2ctarget_i2c_target_obj_t *self, uint8_t *data) {
    i2c_slave_read_buffer(self->i2c_num, data, 128, 0);
    return 1;
}

int common_hal_i2ctarget_i2c_target_write_byte(i2ctarget_i2c_target_obj_t *self, uint8_t data) {
    i2c_reset_tx_fifo(self->i2c_num);
    i2c_slave_write_buffer(self->i2c_num, &data, 128, 0);
    return 1;
}

void common_hal_i2ctarget_i2c_target_ack(i2ctarget_i2c_target_obj_t *self, bool ack) {

}

void common_hal_i2ctarget_i2c_target_close(i2ctarget_i2c_target_obj_t *self) {

}
