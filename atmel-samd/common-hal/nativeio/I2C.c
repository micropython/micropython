/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

 // This file contains all of the port specific HAL functions for the machine
 // module.

#include "shared-bindings/nativeio/I2C.h"
#include "py/nlr.h"

#include "asf/sam0/drivers/sercom/i2c/i2c_master.h"
#include "samd21_pins.h"

// We use ENABLE registers below we don't want to treat as a macro.
#undef ENABLE

// Number of times to try to send packet if failed.
#define TIMEOUT 1

void common_hal_nativeio_i2c_construct(nativeio_i2c_obj_t *self,
        const mcu_pin_obj_t* scl, const mcu_pin_obj_t* sda, uint32_t frequency) {
    struct i2c_master_config config_i2c_master;
    i2c_master_get_config_defaults(&config_i2c_master);
    // Struct takes the argument in Khz not Hz.
    config_i2c_master.baud_rate = frequency / 1000;
    Sercom* sercom = NULL;
    uint32_t sda_pinmux = 0;
    uint32_t scl_pinmux = 0;
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = sda->sercom[i].sercom;
        if (potential_sercom == NULL ||
            potential_sercom->I2CM.CTRLA.bit.ENABLE != 0 ||
            sda->sercom[i].pad != 0) {
            continue;
        }
        sda_pinmux = PINMUX(sda->pin, (i == 0) ? MUX_C : MUX_D);
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (potential_sercom == scl->sercom[j].sercom &&
                scl->sercom[j].pad == 1) {
                scl_pinmux = PINMUX(scl->pin, (j == 0) ? MUX_C : MUX_D);
                sercom = potential_sercom;
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid pins."));
    }

    config_i2c_master.pinmux_pad0 = sda_pinmux; // SDA
    config_i2c_master.pinmux_pad1 = scl_pinmux; // SCL
    config_i2c_master.buffer_timeout = 10000;

    self->sda_pin = sda->pin;
    self->scl_pin = scl->pin;

    enum status_code status = i2c_master_init(&self->i2c_master_instance,
        sercom, &config_i2c_master);
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus init error"));
    }

    i2c_master_enable(&self->i2c_master_instance);
}

void common_hal_nativeio_i2c_deinit(nativeio_i2c_obj_t *self) {
    i2c_master_disable(&self->i2c_master_instance);
    reset_pin(self->sda_pin);
    reset_pin(self->scl_pin);
}

bool common_hal_nativeio_i2c_probe(nativeio_i2c_obj_t *self, uint8_t addr) {
    uint8_t buf;
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = 0,
        .data        = &buf,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };

    enum status_code status = i2c_master_write_packet_wait(
        &self->i2c_master_instance, &packet);
    return status == STATUS_OK;
}

void common_hal_nativeio_i2c_configure(nativeio_i2c_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    return;
}

bool common_hal_nativeio_i2c_try_lock(nativeio_i2c_obj_t *self) {
    self->has_lock = i2c_master_lock(&self->i2c_master_instance) == STATUS_OK;
    return self->has_lock;
}

bool common_hal_nativeio_i2c_has_lock(nativeio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_nativeio_i2c_unlock(nativeio_i2c_obj_t *self) {
    self->has_lock = false;
    i2c_master_unlock(&self->i2c_master_instance);
}

bool common_hal_nativeio_i2c_write(nativeio_i2c_obj_t *self, uint16_t addr,
        const uint8_t *data, size_t len, bool transmit_stop_bit) {
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = len,
        .data        = (uint8_t *) data,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };

    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        if (transmit_stop_bit) {
            status = i2c_master_write_packet_wait(&self->i2c_master_instance,
                                                  &packet);
        } else {
            status = i2c_master_write_packet_wait_no_stop(
                &self->i2c_master_instance, &packet);
        }
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }
    return status == STATUS_OK;
}

bool common_hal_nativeio_i2c_read(nativeio_i2c_obj_t *self, uint16_t addr,
        uint8_t *data, size_t len) {
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = len,
        .data        = data,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };

    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        status = i2c_master_read_packet_wait(&self->i2c_master_instance,
                                             &packet);
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }
    return status == STATUS_OK;
}
