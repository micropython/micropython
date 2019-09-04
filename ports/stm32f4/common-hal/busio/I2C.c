/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "stm32f4xx_hal.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/translate.h"

I2C_HandleTypeDef hi2c2;

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
        const mcu_pin_obj_t* scl, const mcu_pin_obj_t* sda, uint32_t frequency, uint32_t timeout) {
    //TODO: Rework this entire section to use LL so we can properly assign pins
    //      This will also be bundled with MSP removal
    hi2c2.Instance = I2C1;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_Init(&hi2c2) != HAL_OK) {
        mp_raise_RuntimeError(translate("I2C Init Error"));
    } else {
        mp_printf(&mp_plat_print, "I2C INIT OK");
    }

    self->sda_pin = sda->number;
    self->scl_pin = scl->number;

    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NO_PIN;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    HAL_I2C_MspDeInit(&hi2c2);

    self->sda_pin = NO_PIN;
    self->scl_pin = NO_PIN;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr<<1),2,2) == HAL_OK;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;

    //Critical section code that may be required at some point. 
    // uint32_t store_primask = __get_PRIMASK();
    // __disable_irq();
    // __DMB();

        if (!self->has_lock) {
            grabbed_lock = true;
            self->has_lock = true;
        }

    // __DMB();
    // __set_PRIMASK(store_primask);

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
    return HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)(addr<<1), (uint8_t *)data, (uint16_t)len, 2) == HAL_OK ? 0 : MP_EIO;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
        uint8_t *data, size_t len) {
    return HAL_I2C_Master_Receive(&hi2c2, (uint16_t)(addr<<1), data, (uint16_t)len, 2) == HAL_OK ? 0 : MP_EIO;
}
