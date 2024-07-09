// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George, Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/bitbangio/I2C.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

static void delay(bitbangio_i2c_obj_t *self) {
    // We need to use an accurate delay to get acceptable I2C
    // speeds (eg 1us should be not much more than 1us).
    common_hal_mcu_delay_us(self->us_delay);
}

static void scl_low(bitbangio_i2c_obj_t *self) {
    common_hal_digitalio_digitalinout_set_value(&self->scl, false);
}

static void scl_release(bitbangio_i2c_obj_t *self) {
    common_hal_digitalio_digitalinout_set_value(&self->scl, true);
    uint32_t count = self->us_timeout;
    delay(self);
    // For clock stretching, wait for the SCL pin to be released, with timeout.
    common_hal_digitalio_digitalinout_switch_to_input(&self->scl, PULL_UP);
    for (; !common_hal_digitalio_digitalinout_get_value(&self->scl) && count; --count) {
        common_hal_mcu_delay_us(1);
    }
    common_hal_digitalio_digitalinout_switch_to_output(&self->scl, true, DRIVE_MODE_OPEN_DRAIN);
    // raise exception on timeout
    if (count == 0) {
        mp_raise_msg_varg(&mp_type_TimeoutError, MP_ERROR_TEXT("%q too long"), MP_QSTR_timeout);
    }
}

static void sda_low(bitbangio_i2c_obj_t *self) {
    common_hal_digitalio_digitalinout_set_value(&self->sda, false);
}

static void sda_release(bitbangio_i2c_obj_t *self) {
    common_hal_digitalio_digitalinout_set_value(&self->sda, true);
}

static bool sda_read(bitbangio_i2c_obj_t *self) {
    common_hal_digitalio_digitalinout_switch_to_input(&self->sda, PULL_UP);
    bool value = common_hal_digitalio_digitalinout_get_value(&self->sda);
    common_hal_digitalio_digitalinout_switch_to_output(&self->sda, true, DRIVE_MODE_OPEN_DRAIN);
    return value;
}

static void start(bitbangio_i2c_obj_t *self) {
    sda_release(self);
    delay(self);
    scl_release(self);
    sda_low(self);
    delay(self);
}

static void stop(bitbangio_i2c_obj_t *self) {
    delay(self);
    sda_low(self);
    delay(self);
    scl_release(self);
    sda_release(self);
    delay(self);
}

static int write_byte(bitbangio_i2c_obj_t *self, uint8_t val) {
    delay(self);
    scl_low(self);

    for (int i = 7; i >= 0; i--) {
        if ((val >> i) & 1) {
            sda_release(self);
        } else {
            sda_low(self);
        }
        delay(self);
        scl_release(self);
        scl_low(self);
    }

    sda_release(self);
    delay(self);
    scl_release(self);

    int ret = sda_read(self);
    delay(self);
    scl_low(self);

    return !ret;
}

static bool read_byte(bitbangio_i2c_obj_t *self, uint8_t *val, bool ack) {
    delay(self);
    scl_low(self);
    delay(self);

    uint8_t data = 0;
    for (int i = 7; i >= 0; i--) {
        scl_release(self);
        data = (data << 1) | sda_read(self);
        scl_low(self);
        delay(self);
    }
    *val = data;

    // send ack/nack bit
    if (ack) {
        sda_low(self);
    }
    delay(self);
    scl_release(self);
    scl_low(self);
    sda_release(self);

    return true;
}

void shared_module_bitbangio_i2c_construct(bitbangio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency,
    uint32_t us_timeout) {

    self->us_timeout = us_timeout;
    self->us_delay = 500000 / frequency;
    if (self->us_delay == 0) {
        self->us_delay = 1;
    }
    digitalinout_result_t result = common_hal_digitalio_digitalinout_construct(&self->scl, scl);
    if (result != DIGITALINOUT_OK) {
        return;
    }
    result = common_hal_digitalio_digitalinout_construct(&self->sda, sda);
    if (result != DIGITALINOUT_OK) {
        common_hal_digitalio_digitalinout_deinit(&self->scl);
        return;
    }
    common_hal_digitalio_digitalinout_switch_to_output(&self->scl, true, DRIVE_MODE_OPEN_DRAIN);
    common_hal_digitalio_digitalinout_switch_to_output(&self->sda, true, DRIVE_MODE_OPEN_DRAIN);

    stop(self);
}

bool shared_module_bitbangio_i2c_deinited(bitbangio_i2c_obj_t *self) {
    // If one is deinited, both will be.
    return common_hal_digitalio_digitalinout_deinited(&self->scl);
}

void shared_module_bitbangio_i2c_deinit(bitbangio_i2c_obj_t *self) {
    if (shared_module_bitbangio_i2c_deinited(self)) {
        return;
    }
    common_hal_digitalio_digitalinout_deinit(&self->scl);
    common_hal_digitalio_digitalinout_deinit(&self->sda);
}

bool shared_module_bitbangio_i2c_try_lock(bitbangio_i2c_obj_t *self) {
    bool success = false;
    common_hal_mcu_disable_interrupts();
    if (!self->locked) {
        self->locked = true;
        success = true;
    }
    common_hal_mcu_enable_interrupts();
    return success;
}

bool shared_module_bitbangio_i2c_has_lock(bitbangio_i2c_obj_t *self) {
    return self->locked;
}

void shared_module_bitbangio_i2c_unlock(bitbangio_i2c_obj_t *self) {
    self->locked = false;
}

bool shared_module_bitbangio_i2c_probe(bitbangio_i2c_obj_t *self, uint8_t addr) {
    start(self);
    bool ok = write_byte(self, addr << 1);
    stop(self);
    return ok;
}

uint8_t shared_module_bitbangio_i2c_write(bitbangio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    // start the I2C transaction
    start(self);
    uint8_t status = 0;
    if (!write_byte(self, addr << 1)) {
        status = MP_ENODEV;
    }

    if (status == 0) {
        for (uint32_t i = 0; i < len; i++) {
            if (!write_byte(self, data[i])) {
                status = MP_EIO;
                break;
            }
        }
    }

    if (transmit_stop_bit) {
        stop(self);
    }
    return status;
}

uint8_t shared_module_bitbangio_i2c_read(bitbangio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    // start the I2C transaction
    start(self);
    uint8_t status = 0;
    if (!write_byte(self, (addr << 1) | 1)) {
        status = MP_ENODEV;
    }

    if (status == 0) {
        for (uint32_t i = 0; i < len; i++) {
            if (!read_byte(self, data + i, i < len - 1)) {
                status = MP_EIO;
                break;
            }
        }
    }

    stop(self);
    return status;
}
