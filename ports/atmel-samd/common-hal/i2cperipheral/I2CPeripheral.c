/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#include "shared-bindings/i2cperipheral/I2CPeripheral.h"
#include "common-hal/busio/I2C.h"

#include "lib/utils/interrupt_char.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "hal/include/hal_gpio.h"
#include "peripherals/samd/sercom.h"

void common_hal_i2cperipheral_i2c_peripheral_construct(i2cperipheral_i2c_peripheral_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus) {
    uint8_t sercom_index;
    uint32_t sda_pinmux, scl_pinmux;
    Sercom *sercom = samd_i2c_get_sercom(scl, sda, &sercom_index, &sda_pinmux, &scl_pinmux);
    if (sercom == NULL) {
        mp_raise_ValueError(translate("Invalid pins"));
    }
    self->sercom = sercom;

    gpio_set_pin_function(sda->number, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_function(scl->number, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_function(sda->number, sda_pinmux);
    gpio_set_pin_function(scl->number, scl_pinmux);

    self->sda_pin = sda->number;
    self->scl_pin = scl->number;
    claim_pin(sda);
    claim_pin(scl);

    samd_peripherals_sercom_clock_init(sercom, sercom_index);

    #ifdef SAM_D5X_E5X
    sercom->I2CS.CTRLC.bit.SDASETUP = 0x08;
    #endif

    sercom->I2CS.CTRLA.bit.SWRST = 1;
    while (sercom->I2CS.CTRLA.bit.SWRST || sercom->I2CS.SYNCBUSY.bit.SWRST) {
    }

    sercom->I2CS.CTRLB.bit.AACKEN = 0; //  Automatic acknowledge is disabled.

    if (num_addresses == 1) {
        sercom->I2CS.CTRLB.bit.AMODE = 0x0; // MASK
        sercom->I2CS.ADDR.bit.ADDR = addresses[0];
        sercom->I2CS.ADDR.bit.ADDRMASK = 0x00; // Match exact address
    } else if (num_addresses == 2) {
        sercom->I2CS.CTRLB.bit.AMODE = 0x1; // 2_ADDRS
        sercom->I2CS.ADDR.bit.ADDR = addresses[0];
        sercom->I2CS.ADDR.bit.ADDRMASK = addresses[1];
    } else {
        uint32_t combined = 0; // all addresses OR'ed
        uint32_t differ = 0; // bits that differ between addresses
        for (unsigned int i = 0; i < num_addresses; i++) {
            combined |= addresses[i];
            differ |= addresses[0] ^ addresses[i];
        }
        sercom->I2CS.CTRLB.bit.AMODE = 0x0; // MASK
        sercom->I2CS.ADDR.bit.ADDR = combined;
        sercom->I2CS.ADDR.bit.ADDRMASK = differ;
    }
    self->addresses = addresses;
    self->num_addresses = num_addresses;

    if (smbus) {
        sercom->I2CS.CTRLA.bit.LOWTOUTEN = 1; // Errata 12003
        sercom->I2CS.CTRLA.bit.SEXTTOEN = 1; // SCL Low Extend/Cumulative Time-Out 25ms
    }
    sercom->I2CS.CTRLA.bit.SCLSM = 0; // Clock stretch before ack
    sercom->I2CS.CTRLA.bit.MODE = 0x04; // Device mode
    sercom->I2CS.CTRLA.bit.ENABLE = 1;
}

bool common_hal_i2cperipheral_i2c_peripheral_deinited(i2cperipheral_i2c_peripheral_obj_t *self) {
    return self->sda_pin == NO_PIN;
}

void common_hal_i2cperipheral_i2c_peripheral_deinit(i2cperipheral_i2c_peripheral_obj_t *self) {
    if (common_hal_i2cperipheral_i2c_peripheral_deinited(self)) {
        return;
    }

    self->sercom->I2CS.CTRLA.bit.ENABLE = 0;

    reset_pin_number(self->sda_pin);
    reset_pin_number(self->scl_pin);
    self->sda_pin = NO_PIN;
    self->scl_pin = NO_PIN;
}

static int i2c_peripheral_check_error(i2cperipheral_i2c_peripheral_obj_t *self, bool raise) {
    if (!self->sercom->I2CS.INTFLAG.bit.ERROR) {
        return 0;
    }

    int err = MP_EIO;

    if (self->sercom->I2CS.STATUS.bit.LOWTOUT || self->sercom->I2CS.STATUS.bit.SEXTTOUT) {
        err = MP_ETIMEDOUT;
    }

    self->sercom->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR; // Clear flag

    if (raise) {
        mp_raise_OSError(err);
    }
    return -err;
}

int common_hal_i2cperipheral_i2c_peripheral_is_addressed(i2cperipheral_i2c_peripheral_obj_t *self, uint8_t *address, bool *is_read, bool *is_restart) {
    int err = i2c_peripheral_check_error(self, false);
    if (err) {
        return err;
    }

    if (!self->sercom->I2CS.INTFLAG.bit.AMATCH) {
        return 0;
    }

    self->writing = false;

    *address = self->sercom->I2CS.DATA.reg >> 1;
    *is_read = self->sercom->I2CS.STATUS.bit.DIR;
    *is_restart = self->sercom->I2CS.STATUS.bit.SR;

    for (unsigned int i = 0; i < self->num_addresses; i++) {
        if (*address == self->addresses[i]) {
            common_hal_i2cperipheral_i2c_peripheral_ack(self, true);
            return 1;
        }
    }

    // This should clear AMATCH, but it doesn't...
    common_hal_i2cperipheral_i2c_peripheral_ack(self, false);
    return 0;
}

int common_hal_i2cperipheral_i2c_peripheral_read_byte(i2cperipheral_i2c_peripheral_obj_t *self, uint8_t *data) {
    for (int t = 0; t < 100 && !self->sercom->I2CS.INTFLAG.reg; t++) {
        mp_hal_delay_us(10);
    }

    i2c_peripheral_check_error(self, true);

    if (!self->sercom->I2CS.INTFLAG.bit.DRDY ||
        self->sercom->I2CS.INTFLAG.bit.PREC ||
        self->sercom->I2CS.INTFLAG.bit.AMATCH) {
        return 0;
    }

    *data = self->sercom->I2CS.DATA.reg;
    return 1;
}

int common_hal_i2cperipheral_i2c_peripheral_write_byte(i2cperipheral_i2c_peripheral_obj_t *self, uint8_t data) {
    for (int t = 0; !self->sercom->I2CS.INTFLAG.reg && t < 100; t++) {
        mp_hal_delay_us(10);
    }

    i2c_peripheral_check_error(self, true);

    if (self->sercom->I2CS.INTFLAG.bit.PREC) {
        return 0;
    }

    // RXNACK can carry over from the previous transfer
    if (self->writing && self->sercom->I2CS.STATUS.bit.RXNACK) {
        return 0;
    }

    self->writing = true;

    if (!self->sercom->I2CS.INTFLAG.bit.DRDY) {
        return 0;
    }

    self->sercom->I2CS.DATA.bit.DATA = data; // Send data

    return 1;
}

void common_hal_i2cperipheral_i2c_peripheral_ack(i2cperipheral_i2c_peripheral_obj_t *self, bool ack) {
    self->sercom->I2CS.CTRLB.bit.ACKACT = !ack;
    self->sercom->I2CS.CTRLB.bit.CMD = 0x03;
}

void common_hal_i2cperipheral_i2c_peripheral_close(i2cperipheral_i2c_peripheral_obj_t *self) {
    for (int t = 0; !self->sercom->I2CS.INTFLAG.reg && t < 100; t++) {
        mp_hal_delay_us(10);
    }

    if (self->sercom->I2CS.INTFLAG.bit.AMATCH || !self->sercom->I2CS.STATUS.bit.CLKHOLD) {
        return;
    }

    if (!self->sercom->I2CS.STATUS.bit.DIR) {
        common_hal_i2cperipheral_i2c_peripheral_ack(self, false);
    } else {
        int i = 0;
        while (self->sercom->I2CS.INTFLAG.reg == SERCOM_I2CS_INTFLAG_DRDY) {
            if (mp_hal_is_interrupted()) {
                return;
            }

            self->sercom->I2CS.DATA.bit.DATA = 0xff;  // Send dummy byte

            // Wait for a result (if any).
            // test_byte_word.py::TestWord::test_write_seq leaves us with no INTFLAGs set in some of the tests
            for (int t = 0; !self->sercom->I2CS.INTFLAG.reg && t < 100; t++) {
                mp_hal_delay_us(10);
            }

            if (++i > 1000) { // Avoid getting stuck "forever"
                mp_raise_OSError(MP_EIO);
            }
        }
    }

    if (self->sercom->I2CS.INTFLAG.bit.AMATCH) {
        return;
    }

    if (self->sercom->I2CS.STATUS.bit.CLKHOLD) {
        // Unable to release the clock.
        // The device might have to be re-initialized to get unstuck.
        mp_raise_OSError(MP_EIO);
    }
}
