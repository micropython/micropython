// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include <stdbool.h>

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

// I2C timing specs for the H7 and F7
// Configured for maximum possible clock settings for the family
#if (CPY_STM32F7)
#ifndef CPY_I2CFAST_TIMINGR
#define CPY_I2CFAST_TIMINGR     0x6000030D
#endif
#ifndef CPY_I2CSTANDARD_TIMINGR
#define CPY_I2CSTANDARD_TIMINGR 0x20404768
#endif
#elif (CPY_STM32H7)
#ifndef CPY_I2CFAST_TIMINGR
#define CPY_I2CFAST_TIMINGR     0x00B03FDB
#endif
#ifndef CPY_I2CSTANDARD_TIMINGR
#define CPY_I2CSTANDARD_TIMINGR 0x307075B1
#endif
#elif (CPY_STM32L4)
#ifndef CPY_I2CFAST_TIMINGR
#define CPY_I2CFAST_TIMINGR     0x00B03FDB
#endif
#ifndef CPY_I2CSTANDARD_TIMINGR
#define CPY_I2CSTANDARD_TIMINGR 0x307075B1
#endif
#endif

// Arrays use 0 based numbering: I2C1 is stored at index 0
#define MAX_I2C 4

static bool reserved_i2c[MAX_I2C];
static bool never_reset_i2c[MAX_I2C];

#define ALL_CLOCKS 0xFF
static void i2c_clock_enable(uint8_t mask);
static void i2c_clock_disable(uint8_t mask);
static void i2c_assign_irq(busio_i2c_obj_t *self, I2C_TypeDef *I2Cx);

void i2c_reset(void) {
    uint16_t never_reset_mask = 0x00;
    for (int i = 0; i < MAX_I2C; i++) {
        if (!never_reset_i2c[i]) {
            reserved_i2c[i] = false;
        } else {
            never_reset_mask |= 1 << i;
        }
    }
    i2c_clock_disable(ALL_CLOCKS & ~(never_reset_mask));
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    // Match pins to I2C objects
    I2C_TypeDef *I2Cx;
    uint8_t sda_len = MP_ARRAY_SIZE(mcu_i2c_sda_list);
    uint8_t scl_len = MP_ARRAY_SIZE(mcu_i2c_scl_list);
    bool i2c_taken = false;

    for (uint i = 0; i < sda_len; i++) {
        if (mcu_i2c_sda_list[i].pin == sda) {
            for (uint j = 0; j < scl_len; j++) {
                if ((mcu_i2c_scl_list[j].pin == scl)
                    && (mcu_i2c_scl_list[j].periph_index == mcu_i2c_sda_list[i].periph_index)) {
                    // Keep looking if the I2C is taken, could be another SCL that works
                    if (reserved_i2c[mcu_i2c_scl_list[j].periph_index - 1]) {
                        i2c_taken = true;
                        continue;
                    }
                    self->scl = &mcu_i2c_scl_list[j];
                    self->sda = &mcu_i2c_sda_list[i];
                    break;
                }
            }
            if (self->scl != NULL) {
                // Multi-level break to pick lowest peripheral
                break;
            }
        }
    }

    // Handle typedef selection, errors
    if (self->sda != NULL && self->scl != NULL) {
        I2Cx = mcu_i2c_banks[self->sda->periph_index - 1];
    } else {
        if (i2c_taken) {
            mp_raise_ValueError(MP_ERROR_TEXT("Hardware in use, try alternative pins"));
        } else {
            raise_ValueError_invalid_pins();
        }
    }

    // Start GPIO for each pin
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

    // Note: due to I2C soft reboot issue, do not relocate clock init.
    i2c_clock_enable(1 << (self->sda->periph_index - 1));
    reserved_i2c[self->sda->periph_index - 1] = true;

    // Create root pointer and assign IRQ
    MP_STATE_PORT(cpy_i2c_obj_all)[self->sda->periph_index - 1] = self;
    i2c_assign_irq(self, I2Cx);

    // Handle the HAL handle differences
    #if (CPY_STM32H7 || CPY_STM32F7 || CPY_STM32L4)
    if (frequency == 400000) {
        self->handle.Init.Timing = CPY_I2CFAST_TIMINGR;
    } else if (frequency == 100000) {
        self->handle.Init.Timing = CPY_I2CSTANDARD_TIMINGR;
    } else {
        mp_arg_error_invalid(MP_QSTR_frequency);
    }
    #else
    self->handle.Init.ClockSpeed = frequency;
    self->handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    #endif

    self->handle.Instance = I2Cx;
    self->handle.Init.OwnAddress1 = 0;
    self->handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    self->handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    self->handle.Init.OwnAddress2 = 0;
    self->handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    self->handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    self->handle.State = HAL_I2C_STATE_RESET;
    if (HAL_I2C_Init(&(self->handle)) != HAL_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("I2C init error"));
    }
    common_hal_mcu_pin_claim(sda);
    common_hal_mcu_pin_claim(scl);

    self->frame_in_prog = false;

    // start the receive interrupt chain
    HAL_NVIC_DisableIRQ(self->irq); // prevent handle lock contention
    HAL_NVIC_SetPriority(self->irq, 1, 0);
    HAL_NVIC_EnableIRQ(self->irq);
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_i2c_banks); i++) {
        if (self->handle.Instance == mcu_i2c_banks[i]) {
            never_reset_i2c[i] = true;

            never_reset_pin_number(self->scl->pin->port, self->scl->pin->number);
            never_reset_pin_number(self->sda->pin->port, self->sda->pin->number);
            break;
        }
    }
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    i2c_clock_disable(1 << (self->sda->periph_index - 1));
    reserved_i2c[self->sda->periph_index - 1] = false;
    never_reset_i2c[self->sda->periph_index - 1] = false;

    reset_pin_number(self->sda->pin->port, self->sda->pin->number);
    reset_pin_number(self->scl->pin->port, self->scl->pin->number);
    self->sda = NULL;
    self->scl = NULL;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return HAL_I2C_IsDeviceReady(&(self->handle), (uint16_t)(addr << 1), 2, 2) == HAL_OK;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;

    // Critical section code that may be required at some point.
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

static uint8_t _common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    HAL_StatusTypeDef result;
    if (!transmit_stop_bit) {
        uint32_t xfer_opt;
        if (!self->frame_in_prog) {
            xfer_opt = I2C_FIRST_FRAME;
        } else {
            // handle rare possibility of multiple restart writes in a row
            xfer_opt = I2C_NEXT_FRAME;
        }
        result = HAL_I2C_Master_Seq_Transmit_IT(&(self->handle),
            (uint16_t)(addr << 1), (uint8_t *)data,
            (uint16_t)len, xfer_opt);
        while (HAL_I2C_GetState(&(self->handle)) != HAL_I2C_STATE_READY) {
            RUN_BACKGROUND_TASKS;
        }
        self->frame_in_prog = true;
    } else {
        result = HAL_I2C_Master_Transmit(&(self->handle), (uint16_t)(addr << 1),
            (uint8_t *)data, (uint16_t)len, 500);
    }
    return result == HAL_OK ? 0 : MP_EIO;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {
    return _common_hal_busio_i2c_write(self, addr, data, len, true);
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    if (!self->frame_in_prog) {
        return HAL_I2C_Master_Receive(&(self->handle), (uint16_t)(addr << 1), data, (uint16_t)len, 500)
               == HAL_OK ? 0 : MP_EIO;
    } else {
        HAL_StatusTypeDef result = HAL_I2C_Master_Seq_Receive_IT(&(self->handle),
            (uint16_t)(addr << 1), (uint8_t *)data,
            (uint16_t)len, I2C_LAST_FRAME);
        while (HAL_I2C_GetState(&(self->handle)) != HAL_I2C_STATE_READY) {
            RUN_BACKGROUND_TASKS;
        }
        self->frame_in_prog = false;
        return result;
    }
}

uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    uint8_t result = _common_hal_busio_i2c_write(self, addr, out_data, out_len, false);
    if (result != 0) {
        return result;
    }

    return common_hal_busio_i2c_read(self, addr, in_data, in_len);
}

static void i2c_clock_enable(uint8_t mask) {
    // Note: hard reset required due to soft reboot issue.
    #ifdef I2C1
    if (mask & (1 << 0)) {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_I2C1_FORCE_RESET();
        __HAL_RCC_I2C1_RELEASE_RESET();
    }
    #endif
    #ifdef I2C2
    if (mask & (1 << 1)) {
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_I2C2_FORCE_RESET();
        __HAL_RCC_I2C2_RELEASE_RESET();
    }
    #endif
    #ifdef I2C3
    if (mask & (1 << 2)) {
        __HAL_RCC_I2C3_CLK_ENABLE();
        __HAL_RCC_I2C3_FORCE_RESET();
        __HAL_RCC_I2C3_RELEASE_RESET();
    }
    #endif
    #ifdef I2C4
    if (mask & (1 << 3)) {
        __HAL_RCC_I2C4_CLK_ENABLE();
        __HAL_RCC_I2C4_FORCE_RESET();
        __HAL_RCC_I2C4_RELEASE_RESET();
    }
    #endif
}

static void i2c_clock_disable(uint8_t mask) {
    #ifdef I2C1
    if (mask & (1 << 0)) {
        __HAL_RCC_I2C1_CLK_DISABLE();
    }
    #endif
    #ifdef I2C2
    if (mask & (1 << 1)) {
        __HAL_RCC_I2C2_CLK_DISABLE();
    }
    #endif
    #ifdef I2C3
    if (mask & (1 << 2)) {
        __HAL_RCC_I2C3_CLK_DISABLE();
    }
    #endif
    #ifdef I2C4
    if (mask & (1 << 3)) {
        __HAL_RCC_I2C4_CLK_DISABLE();
    }
    #endif
}

static void i2c_assign_irq(busio_i2c_obj_t *self, I2C_TypeDef *I2Cx) {
    #ifdef I2C1
    if (I2Cx == I2C1) {
        self->irq = I2C1_EV_IRQn;
    }
    #endif
    #ifdef I2C2
    if (I2Cx == I2C2) {
        self->irq = I2C2_EV_IRQn;
    }
    #endif
    #ifdef I2C3
    if (I2Cx == I2C3) {
        self->irq = I2C3_EV_IRQn;
    }
    #endif
    #ifdef I2C4
    if (I2Cx == I2C4) {
        self->irq = I2C4_EV_IRQn;
    }
    #endif
}

static void call_hal_irq(int i2c_num) {
    // Create casted context pointer
    busio_i2c_obj_t *context = (busio_i2c_obj_t *)MP_STATE_PORT(cpy_i2c_obj_all)[i2c_num - 1];
    if (context != NULL) {
        HAL_NVIC_ClearPendingIRQ(context->irq);
        HAL_I2C_EV_IRQHandler(&context->handle);
    }
}

void I2C1_EV_IRQHandler(void) {
    call_hal_irq(1);
}
void I2C2_EV_IRQHandler(void) {
    call_hal_irq(2);
}
void I2C3_EV_IRQHandler(void) {
    call_hal_irq(3);
}
void I2C4_EV_IRQHandler(void) {
    call_hal_irq(4);
}

MP_REGISTER_ROOT_POINTER(void *cpy_i2c_obj_all[MAX_I2C]);
