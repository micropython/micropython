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
#include <stdbool.h>

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "stm32f4xx_hal.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/translate.h"
#include "common-hal/microcontroller/Pin.h"

STATIC bool reserved_i2c[3];

void i2c_reset(void) {
    //Note: I2Cs are also forcibly reset in construct, due to silicon error
    #ifdef I2C1
        reserved_i2c[0] = false;
        __HAL_RCC_I2C1_CLK_DISABLE(); 
    #endif
    #ifdef I2C2
        reserved_i2c[1] = false;
        __HAL_RCC_I2C2_CLK_DISABLE(); 
    #endif
    #ifdef I2C3
        reserved_i2c[3] = false;
        __HAL_RCC_I2C3_CLK_DISABLE(); 
    #endif
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
        const mcu_pin_obj_t* scl, const mcu_pin_obj_t* sda, uint32_t frequency, uint32_t timeout) {

    //match pins to I2C objects
    I2C_TypeDef * I2Cx;

    uint8_t sda_len = sizeof(mcu_i2c_sda_list)/sizeof(*mcu_i2c_sda_list);
    uint8_t scl_len = sizeof(mcu_i2c_scl_list)/sizeof(*mcu_i2c_scl_list);
    for(uint i=0; i<sda_len;i++) {
        if (mcu_i2c_sda_list[i].pin == sda) {
            for(uint j=0; j<scl_len;j++) {
                if ((mcu_i2c_scl_list[j].pin == scl)
                    && (mcu_i2c_scl_list[j].i2c_index == mcu_i2c_sda_list[i].i2c_index)) {
                    self->scl = &mcu_i2c_scl_list[j];
                    self->sda = &mcu_i2c_sda_list[i];
                    break;
                }
            }
        }
    }

    //handle typedef selection, errors
    if(self->sda!=NULL && self->scl!=NULL ) {
        I2Cx = mcu_i2c_banks[self->sda->i2c_index-1];
    } else {
        mp_raise_RuntimeError(translate("Invalid I2C pin selection"));
    }

    if(reserved_i2c[self->sda->i2c_index-1]) {
        mp_raise_RuntimeError(translate("Hardware busy, try alternative pins"));
    }

    //Start GPIO for each pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(sda->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = self->sda->altfn_index; 
    HAL_GPIO_Init(pin_port(sda->port), &GPIO_InitStruct);

    GPIO_InitStruct.Pin = pin_mask(scl->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = self->scl->altfn_index; 
    HAL_GPIO_Init(pin_port(scl->port), &GPIO_InitStruct);

    //Fix for HAL error caused by soft reboot GPIO init SDA pin voltage drop. See Eratta. 
    //Must be in this exact spot or I2C will get stuck in infinite loop. 
    //TODO: See git issue #2172
    #ifdef I2C1
    __HAL_RCC_I2C1_FORCE_RESET();
    HAL_Delay(2);
    __HAL_RCC_I2C1_RELEASE_RESET();
    #endif
    #ifdef I2C2
    __HAL_RCC_I2C2_FORCE_RESET();
    HAL_Delay(2);
    __HAL_RCC_I2C2_RELEASE_RESET();
    #endif
    #ifdef I2C3
    __HAL_RCC_I2C3_FORCE_RESET();
    HAL_Delay(2);
    __HAL_RCC_I2C3_RELEASE_RESET();
    #endif

    //Keep separate so above hack can be cleanly replaced
    #ifdef I2C1
    if(I2Cx==I2C1) { 
        reserved_i2c[0] = true;
        __HAL_RCC_I2C1_CLK_ENABLE();
    } 
    #endif
    #ifdef I2C2
    if(I2Cx==I2C2) {
        reserved_i2c[1] = true;
        __HAL_RCC_I2C2_CLK_ENABLE(); 
    }
    #endif
    #ifdef I2C3
    if(I2Cx==I2C3) {
        reserved_i2c[2] = true;
        __HAL_RCC_I2C3_CLK_ENABLE(); 
    }
    #endif

    self->handle.Instance = I2Cx;
    self->handle.Init.ClockSpeed = 100000;
    self->handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    self->handle.Init.OwnAddress1 = 0;
    self->handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    self->handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    self->handle.Init.OwnAddress2 = 0;
    self->handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    self->handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_Init(&(self->handle)) != HAL_OK) {
        mp_raise_RuntimeError(translate("I2C Init Error"));
    }
    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda->pin == mp_const_none;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    #ifdef I2C1
    if(self->handle.Instance==I2C1) {
        reserved_i2c[0] = 0;
        __HAL_RCC_I2C1_CLK_DISABLE(); 
    }
    #endif
    #ifdef I2C2
    if(self->handle.Instance==I2C2) {
        reserved_i2c[1] = 0;
        __HAL_RCC_I2C2_CLK_DISABLE(); 
    }
    #endif
    #ifdef I2C3
    if(self->handle.Instance==I2C3) {
        reserved_i2c[3] = 0;
        __HAL_RCC_I2C3_CLK_DISABLE(); 
    }
    #endif
    reset_pin_number(self->sda->pin->port,self->sda->pin->number);
    reset_pin_number(self->scl->pin->port,self->scl->pin->number);
    self->sda = mp_const_none;
    self->scl = mp_const_none;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return HAL_I2C_IsDeviceReady(&(self->handle), (uint16_t)(addr<<1),2,2) == HAL_OK;
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
    HAL_StatusTypeDef result = HAL_I2C_Master_Transmit(&(self->handle), (uint16_t)(addr<<1), (uint8_t *)data, (uint16_t)len, 500);
    return result == HAL_OK ? 0 : MP_EIO;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
        uint8_t *data, size_t len) {
    return HAL_I2C_Master_Receive(&(self->handle), (uint16_t)(addr<<1), data, (uint16_t)len, 500) == HAL_OK ? 0 : MP_EIO;
}
