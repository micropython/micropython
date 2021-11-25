/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared-bindings/busio/I2C.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "peripherals/broadcom/cpu.h"
#include "peripherals/broadcom/vcmailbox.h"

#define NUM_I2C (2)

STATIC bool never_reset_i2c[NUM_I2C];
STATIC bool in_use_i2c[NUM_I2C];
STATIC BSC0_Type *i2c[2] = {BSC0, BSC1};

void reset_i2c(void) {
    for (size_t i = 0; i < 2; i++) {
        if (never_reset_i2c[i]) {
            continue;
        }
        in_use_i2c[i] = false;
    }
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {
    size_t instance_index = NUM_I2C;
    if ((scl == &pin_GPIO1 || scl == &pin_GPIO29 || scl == &pin_GPIO45) &&
        (sda == &pin_GPIO0 || sda == &pin_GPIO28 || sda == &pin_GPIO44)) {
        instance_index = 0;
    } else if ((scl == &pin_GPIO44 || scl == &pin_GPIO3) &&
               (sda == &pin_GPIO43 || sda == &pin_GPIO2)) {
        instance_index = 1;
    }
    if (instance_index == NUM_I2C) {
        mp_raise_ValueError(translate("Invalid pins"));
    }
    in_use_i2c[instance_index] = true;
    self->index = instance_index;
    self->peripheral = i2c[self->index];
    self->sda_pin = sda;
    self->scl_pin = scl;

    uint32_t source_clock = vcmailbox_get_clock_rate_measured(VCMAILBOX_CLOCK_CORE);
    uint16_t clock_divider = source_clock / frequency;
    self->peripheral->DIV_b.CDIV = clock_divider;

    gpio_set_function(sda->number, GPIO_GPFSEL0_FSEL2_SDA1);
    gpio_set_function(scl->number, GPIO_GPFSEL0_FSEL3_SCL1);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    never_reset_i2c[self->index] = false;

    common_hal_reset_pin(self->sda_pin);
    common_hal_reset_pin(self->scl_pin);
    self->sda_pin = NULL;
    self->scl_pin = NULL;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    uint8_t result = common_hal_busio_i2c_write(self, addr, NULL, 0, true);
    // mp_printf(&mp_plat_print, "result %d %d\n", addr, result);
    return result == 0;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

// Discussion of I2C implementation is here: https://github.com/raspberrypi/linux/issues/254

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    COMPLETE_MEMORY_READS;
    self->peripheral->S_b.DONE = true;
    self->peripheral->A_b.ADDR = addr;
    size_t loop_len = len;
    // Prevent the stop bit by transmitting everything but the last byte. Doing
    // so is left up to the subsequent read.
    if (!transmit_stop_bit) {
        loop_len -= 1;
    }
    self->peripheral->DLEN_b.DLEN = len;
    self->peripheral->C = BSC0_C_ST_Msk | BSC0_C_I2CEN_Msk;
    // Wait for the transaction to start.
    while (self->peripheral->S_b.TA == 0) {
        RUN_BACKGROUND_TASKS;
    }
    for (size_t i = 0; i < loop_len; i++) {
        if (self->peripheral->S_b.ERR) {
            break;
        }
        self->peripheral->FIFO_b.DATA = data[i];
        // Wait for the FIFO to empty enough that we can write more data.
        while (self->peripheral->S_b.TXE == 0) {
            RUN_BACKGROUND_TASKS;
        }
    }
    // Wait for the FIFO to empty completely, not DONE, because we may not complete the
    // transaction with a write.
    while (self->peripheral->S_b.ERR == 0 &&
           ((!transmit_stop_bit && self->peripheral->S_b.TXE == 0) ||
            (transmit_stop_bit && self->peripheral->S_b.TA == 1))) {
        RUN_BACKGROUND_TASKS;
    }
    self->finish_write = false;
    if (self->peripheral->S_b.ERR) {
        // Wait for the transfer to finish.
        while (self->peripheral->S_b.TA == 1) {
            RUN_BACKGROUND_TASKS;
        }
        // Clear the flag by writing it and wait for it to clear.
        self->peripheral->S_b.ERR = true;
        while (self->peripheral->S_b.ERR == 1) {
            RUN_BACKGROUND_TASKS;
        }
        return MP_ENODEV;
    }

    if (loop_len < len) {
        self->finish_write = true;
        self->last_write_data = data[len - 1];
    }
    return 0;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    COMPLETE_MEMORY_READS;
    self->peripheral->A_b.ADDR = addr;
    if (self->finish_write) {
        self->finish_write = false;
        if (self->peripheral->S_b.ERR == 1) {
            return MP_ENODEV;
        }
        self->peripheral->FIFO_b.DATA = self->last_write_data;
    } else {
        self->peripheral->S_b.DONE = true;
    }
    self->peripheral->DLEN_b.DLEN = len;
    self->peripheral->C = BSC0_C_READ_Msk | BSC0_C_ST_Msk | BSC0_C_I2CEN_Msk;
    // Wait for the transaction to start.
    while (self->peripheral->S_b.TA == 0) {
        RUN_BACKGROUND_TASKS;
    }
    for (size_t i = 0; i < len; i++) {
        if (self->peripheral->S_b.ERR) {
            break;
        }
        // Wait for the FIFO to have enough data that we can read it. RXR is low
        // once the transaction is done so we check the done bit too.
        while (!self->peripheral->S_b.RXR && !self->peripheral->S_b.DONE) {
            RUN_BACKGROUND_TASKS;
        }
        data[i] = self->peripheral->FIFO_b.DATA;
    }
    // Wait for the transaction to finish.
    while (!self->peripheral->S_b.DONE && !self->peripheral->S_b.ERR) {
        RUN_BACKGROUND_TASKS;
    }
    if (self->peripheral->S_b.ERR) {
        self->peripheral->S_b.ERR = true;
        while (self->peripheral->S_b.ERR == 1) {
            RUN_BACKGROUND_TASKS;
        }
        return MP_ENODEV;
    }

    return 0;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset_i2c[self->index] = true;

    common_hal_never_reset_pin(self->scl_pin);
    common_hal_never_reset_pin(self->sda_pin);
}
