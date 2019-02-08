/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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
#include "modmachine.h"
#include "i2c.h"
#include "bm_twi.h"

#if MICROPY_HW_ENABLE_HW_I2C

#define I2C_POLL_TIMEOUT_MS (50)

i2c_t I2CHandle0;
i2c_t I2CHandle1;
i2c_t I2CHandle2;

void i2c_init0(void) {
    twi_initialize(&I2CHandle0, 0x01, machine_get_fsclk0(), 0);
    twi_initialize(&I2CHandle1, 0x01, machine_get_fsclk0(), 1);
    twi_initialize(&I2CHandle2, 0x01, machine_get_fsclk0(), 2);
}

int i2c_init(i2c_t *i2c, uint32_t freq) {
    // On SC5XX, I2C pins are not multiplexed, there is no need to configure pins
    return (twi_set_clock(i2c, freq) == 0);
}

int i2c_readfrom(i2c_t *i2c, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    int ret;
    i2c -> address = addr;
    ret = twi_read_block_r(i2c, dest, len, !stop);
    return (ret != TWI_SIMPLE_SUCCESS);
}

int i2c_writeto(i2c_t *i2c, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    int ret;
    i2c -> address = addr;
    ret = twi_write_block_r(i2c, (uint8_t *)src, len, !stop);
    return (ret != TWI_SIMPLE_SUCCESS);
}

#endif // MICROPY_HW_ENABLE_HW_I2C
