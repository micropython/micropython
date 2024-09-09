/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "se05x_APDU_apis.h"

#include "sm_port.h"
#include "sm_i2c.h"
#include "sm_timer.h"

extern mp_obj_t mp_plugntrust_i2c_instance(void);

uint32_t sm_initSleep() {
    return 0;
}

void sm_sleep(uint32_t ms) {
    mp_hal_delay_ms(ms);
}

void sm_usleep(uint32_t us) {
    mp_hal_delay_ms(us);
}

i2c_error_t axI2CInit(void **ctx, const char *dev) {
    mp_hal_pin_config(pyb_pin_SE05X_EN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_low(pyb_pin_SE05X_EN);
    mp_hal_delay_ms(10);
    mp_hal_pin_high(pyb_pin_SE05X_EN);
    mp_hal_delay_ms(10);
    *ctx = mp_plugntrust_i2c_instance();
    return I2C_OK;
}

void axI2CTerm(void *ctx, int mode) {
    (void)ctx;
    (void)mode;
    extern Se05xSession_t pSession;
    memset(&pSession, 0, sizeof(pSession));
}

i2c_error_t axI2CWrite(void *ctx, unsigned char bus, unsigned char addr, unsigned char *buf, unsigned short len) {
    if (buf == NULL || len > MAX_APDU_BUFFER) {
        return I2C_FAILED;
    }

    mp_machine_i2c_buf_t i2c_buf = {.len = len, .buf = buf};
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_i2c_type, protocol);

    // Check returned bytes for write.
    if (i2c_p->transfer(ctx, (addr >> 1), 1, &i2c_buf, MP_MACHINE_I2C_FLAG_STOP) != len) {
        return I2C_FAILED;
    }

    return I2C_OK;
}

i2c_error_t axI2CRead(void *ctx, unsigned char bus, unsigned char addr, unsigned char *buf, unsigned short len) {
    if (buf == NULL || len > MAX_APDU_BUFFER) {
        return I2C_FAILED;
    }
    mp_machine_i2c_buf_t i2c_buf = {.len = len, .buf = buf};
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_i2c_type, protocol);

    if (i2c_p->transfer(ctx, (addr >> 1), 1, &i2c_buf, MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP) < 0) {
        return I2C_FAILED;
    }

    return I2C_OK;
}
