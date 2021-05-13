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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "hal/include/hal_gpio.h"
#include "hal/include/hal_i2c_m_sync.h"
#include "hal/include/hpl_i2c_m_sync.h"

#include "samd/sercom.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/translate.h"

#include "common-hal/busio/SPI.h" // for never_reset_sercom

// Number of times to try to send packet if failed.
#define ATTEMPTS 2

Sercom *samd_i2c_get_sercom(const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *sercom_index, uint32_t *sda_pinmux, uint32_t *scl_pinmux) {
    *sda_pinmux = 0;
    *scl_pinmux = 0;
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        *sercom_index = sda->sercom[i].index;
        if (*sercom_index >= SERCOM_INST_NUM) {
            continue;
        }
        Sercom *potential_sercom = sercom_insts[*sercom_index];
        if (potential_sercom->I2CM.CTRLA.bit.ENABLE != 0 ||
            sda->sercom[i].pad != 0) {
            continue;
        }
        *sda_pinmux = PINMUX(sda->number, (i == 0) ? MUX_C : MUX_D);
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (*sercom_index == scl->sercom[j].index &&
                scl->sercom[j].pad == 1) {
                *scl_pinmux = PINMUX(scl->number, (j == 0) ? MUX_C : MUX_D);
                return potential_sercom;
            }
        }
    }
    return NULL;
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {
    uint8_t sercom_index;
    uint32_t sda_pinmux, scl_pinmux;
    Sercom *sercom = samd_i2c_get_sercom(scl, sda, &sercom_index, &sda_pinmux, &scl_pinmux);
    if (sercom == NULL) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    // Test that the pins are in a high state. (Hopefully indicating they are pulled up.)
    gpio_set_pin_function(sda->number, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_function(scl->number, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(sda->number, GPIO_DIRECTION_IN);
    gpio_set_pin_direction(scl->number, GPIO_DIRECTION_IN);

    gpio_set_pin_pull_mode(sda->number, GPIO_PULL_DOWN);
    gpio_set_pin_pull_mode(scl->number, GPIO_PULL_DOWN);

    common_hal_mcu_delay_us(10);

    gpio_set_pin_pull_mode(sda->number, GPIO_PULL_OFF);
    gpio_set_pin_pull_mode(scl->number, GPIO_PULL_OFF);

    // We must pull up within 3us to achieve 400khz.
    common_hal_mcu_delay_us(3);

    if (!gpio_get_pin_level(sda->number) || !gpio_get_pin_level(scl->number)) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        mp_raise_RuntimeError(translate("No pull up found on SDA or SCL; check your wiring"));
    }
    #endif

    gpio_set_pin_function(sda->number, sda_pinmux);
    gpio_set_pin_function(scl->number, scl_pinmux);

    // Set up I2C clocks on sercom.
    samd_peripherals_sercom_clock_init(sercom, sercom_index);

    if (i2c_m_sync_init(&self->i2c_desc, sercom) != ERR_NONE) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        mp_raise_OSError(MP_EIO);
    }

    // clkrate is always 0. baud_rate is in kHz.

    // Frequency must be set before the I2C device is enabled.
    if (i2c_m_sync_set_baudrate(&self->i2c_desc, 0, frequency / 1000) != ERR_NONE) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        common_hal_busio_i2c_deinit(self);
        mp_raise_ValueError(translate("Unsupported baudrate"));
    }

    self->sda_pin = sda->number;
    self->scl_pin = scl->number;
    claim_pin(sda);
    claim_pin(scl);

    if (i2c_m_sync_enable(&self->i2c_desc) != ERR_NONE) {
        common_hal_busio_i2c_deinit(self);
        mp_raise_OSError(MP_EIO);
    }
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NO_PIN;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    i2c_m_sync_disable(&self->i2c_desc);
    i2c_m_sync_deinit(&self->i2c_desc);

    reset_pin_number(self->sda_pin);
    reset_pin_number(self->scl_pin);
    self->sda_pin = NO_PIN;
    self->scl_pin = NO_PIN;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    struct io_descriptor *i2c_io;
    i2c_m_sync_get_io_descriptor(&self->i2c_desc, &i2c_io);
    i2c_m_sync_set_slaveaddr(&self->i2c_desc, addr, I2C_M_SEVEN);

    // Write no data when just probing
    return io_write(i2c_io, NULL, 0) == ERR_NONE;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;
    CRITICAL_SECTION_ENTER()
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {

    uint16_t attempts = ATTEMPTS;
    int32_t status;
    do {
        struct _i2c_m_msg msg;
        msg.addr = addr;
        msg.len = len;
        msg.flags = transmit_stop_bit ? I2C_M_STOP : 0;
        msg.buffer = (uint8_t *)data;
        status = _i2c_m_sync_transfer(&self->i2c_desc.device, &msg);

        // Give up after ATTEMPTS tries.
        if (--attempts == 0) {
            break;
        }
    } while (status != I2C_OK);
    if (status == I2C_OK) {
        return 0;
    } else if (status == I2C_ERR_BAD_ADDRESS) {
        return MP_ENODEV;
    }
    return MP_EIO;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {

    uint16_t attempts = ATTEMPTS;
    int32_t status;
    do {
        struct _i2c_m_msg msg;
        msg.addr = addr;
        msg.len = len;
        msg.flags = I2C_M_STOP | I2C_M_RD;
        msg.buffer = data;
        status = _i2c_m_sync_transfer(&self->i2c_desc.device, &msg);

        // Give up after ATTEMPTS tries.
        if (--attempts == 0) {
            break;
        }
    } while (status != I2C_OK);
    if (status == ERR_NONE) {
        return 0;
    } else if (status == I2C_ERR_BAD_ADDRESS) {
        return MP_ENODEV;
    }
    return MP_EIO;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset_sercom(self->i2c_desc.device.hw);

    never_reset_pin_number(self->scl_pin);
    never_reset_pin_number(self->sda_pin);
}
