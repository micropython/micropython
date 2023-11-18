/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

STATIC I2CSPM_Init_TypeDef i2cspm_init;
STATIC bool in_used = false;
STATIC bool never_reset = false;

// Reser I2C peripheral
void i2c_reset(void) {
    if ((!never_reset) && in_used) {
        I2C_Reset(DEFAULT_I2C_PERIPHERAL);
        in_used = false;
    }
}

// Construct I2C protocol, this function init i2c peripheral
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout) {

    if ((scl != NULL) && (sda != NULL)) {
        if (scl->function_list[ DEFAULT_I2C_PERIPHERAL == I2C1?
                                FN_I2C1_SCL : FN_I2C0_SCL] == 1 &&
            scl->function_list[DEFAULT_I2C_PERIPHERAL == I2C1?
                               FN_I2C1_SDA : FN_I2C0_SDA] == 1) {
            self->scl = scl;
            self->sda = sda;
            self->has_lock = false;
            i2cspm_init.sclPort = self->scl->port;
            i2cspm_init.sclPin = self->scl->number;
            i2cspm_init.sdaPort = self->sda->port;
            i2cspm_init.sdaPin = self->sda->number;
            i2cspm_init.port = DEFAULT_I2C_PERIPHERAL;
            i2cspm_init.i2cRefFreq = 0;
            i2cspm_init.i2cMaxFreq = I2C_FREQ_STANDARD_MAX;
            i2cspm_init.i2cClhr = i2cClockHLRStandard;

            self->i2cspm = i2cspm_init.port;
            I2CSPM_Init(&i2cspm_init);
            common_hal_mcu_pin_claim(scl);
            common_hal_mcu_pin_claim(sda);
            in_used = true;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Hardware in use, try alternative pins"));
        }
    } else {
        raise_ValueError_invalid_pins();
    }
}

// Never reset I2C obj when reload
void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset = true;
    common_hal_never_reset_pin(self->sda);
    common_hal_never_reset_pin(self->scl);
}

// Check I2C status, deinited or not
bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda == NULL;
}

// Deinit i2c obj, reset I2C pin
void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    I2C_Reset(self->i2cspm);
    common_hal_reset_pin(self->sda);
    common_hal_reset_pin(self->scl);
    self->sda = NULL;
    self->scl = NULL;
    self->i2cspm = NULL;
    in_used = false;
}

// Probe device in I2C bus
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef ret;
    uint8_t data = 0;

    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_READ;

    seq.buf[0].data = &data;
    seq.buf[0].len = 1;

    ret = I2CSPM_Transfer(self->i2cspm, &seq);
    if (ret != i2cTransferDone) {
        return false;
    }
    return true;
}

// Lock I2C bus
bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;

    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }

    return grabbed_lock;
}

// Check I2C lock status
bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

// Unlock I2C bus
void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

// Write data to the device selected by address
uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {

    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef ret;

    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_WRITE;

    seq.buf[0].data = (uint8_t *)data;
    seq.buf[0].len = len;

    ret = I2CSPM_Transfer(self->i2cspm, &seq);
    if (ret != i2cTransferDone) {
        return MP_EIO;
    }
    return 0;
}

// Read into buffer from the device selected by address
uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self,
    uint16_t addr,
    uint8_t *data, size_t len) {

    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef ret;

    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_READ;

    seq.buf[0].data = data;
    seq.buf[0].len = len;

    ret = I2CSPM_Transfer(self->i2cspm, &seq);
    if (ret != i2cTransferDone) {
        return MP_EIO;
    }
    return 0;
}

// Write the bytes from out_data to the device selected by address,
uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len,
    uint8_t *in_data, size_t in_len) {

    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef ret;

    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_WRITE_READ;
    // Select command to issue
    seq.buf[0].data = out_data;
    seq.buf[0].len = out_len;
    // Select location/length of data to be read
    seq.buf[1].data = in_data;
    seq.buf[1].len = in_len;

    ret = I2CSPM_Transfer(self->i2cspm, &seq);
    if (ret != i2cTransferDone) {
        return MP_EIO;
    }
    return 0;
}
