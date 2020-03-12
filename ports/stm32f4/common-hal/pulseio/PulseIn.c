/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "common-hal/pulseio/PulseIn.h"
#include <stdint.h>
#include <string.h>
#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "tick.h"

#include "stm32f4xx_hal.h"

#define STM32_GPIO_PORT_SIZE 16

static pulseio_pulsein_obj_t* _objs[STM32_GPIO_PORT_SIZE];

static void assign_EXTI_Interrupt(pulseio_pulsein_obj_t* self, uint8_t num);

static void pulsein_handler(uint8_t num) {
    // Interrupt register must be cleared manually
    EXTI->PR = 1 << num;

    // Grab the current time first.
    uint32_t current_us;
    uint64_t current_ms;
    current_tick(&current_ms, &current_us);

    // current_tick gives us the remaining us until the next tick but we want the number since the last ms.
    current_us = 1000 - current_us;

    pulseio_pulsein_obj_t* self = _objs[num];
    if ( !self ) return;

    if (self->first_edge) {
        // first pulse is opposite state from idle
        bool state = HAL_GPIO_ReadPin(pin_port(self->pin->port), pin_mask(self->pin->number));
        if ( self->idle_state != state ) {
            self->first_edge = false;
        }
    } else {
        uint32_t ms_diff = current_ms - self->last_ms;
        uint16_t us_diff = current_us - self->last_us;
        uint32_t total_diff = us_diff;

        if (self->last_us > current_us) {
            total_diff = 1000 + current_us - self->last_us;
            if (ms_diff > 1) {
                total_diff += (ms_diff - 1) * 1000;
            }
        } else {
            total_diff += ms_diff * 1000;
        }
        uint16_t duration = 0xffff;
        if (total_diff < duration) {
            duration = total_diff;
        }

        uint16_t i = (self->start + self->len) % self->maxlen;
        self->buffer[i] = duration;
        if (self->len < self->maxlen) {
            self->len++;
        } else {
            self->start++;
        }
    }

    self->last_ms = current_ms;
    self->last_us = current_us;
}

void pulsein_reset(void) {
    // Disable all active interrupts and clear array
    for (uint i = 0; i < STM32_GPIO_PORT_SIZE; i++) {
        if (_objs[i] != NULL) {
            HAL_NVIC_DisableIRQ(_objs[i]->irq);
        }
    }
    memset(_objs, 0, sizeof(_objs));
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self, const mcu_pin_obj_t* pin,
                                             uint16_t maxlen, bool idle_state) {
    // STM32 has one shared EXTI for each pin number, 0-15
    uint8_t p_num = pin->number;
    if(_objs[p_num]) {
        mp_raise_ValueError(translate("Pin number already reserved by EXTI"));
    }
    _objs[p_num] = self;

    // Allocate pulse buffer
    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), 
                          maxlen * sizeof(uint16_t));
    }
    
    // Set internal variables
    self->pin = pin;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->first_edge = true;
    self->paused = false;
    self->last_us = 0;
    self->last_ms = 0;

    // EXTI pins can also be read as an input
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    // Interrupt starts immediately
    assign_EXTI_Interrupt(self, pin->number);
    HAL_NVIC_EnableIRQ(self->irq);
    claim_pin(pin);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return (self->pin == NULL);
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    //Remove pulsein slot from shared array
    HAL_NVIC_DisableIRQ(self->irq);
    _objs[self->pin->number] = NULL;
    reset_pin_number(self->pin->port, self->pin->number);
    self->pin = NULL;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    HAL_NVIC_DisableIRQ(self->irq);
    self->paused = true;
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self, uint16_t trigger_duration) {
    // Make sure we're paused.
    if ( !self->paused ) {
        common_hal_pulseio_pulsein_pause(self);
    }

    // Send the trigger pulse.
    if (trigger_duration > 0) {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = pin_mask(self->pin->number);
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(pin_port(self->pin->port), &GPIO_InitStruct);

        HAL_GPIO_WritePin(pin_port(self->pin->port), pin_mask(self->pin->number), !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        HAL_GPIO_WritePin(pin_port(self->pin->port), pin_mask(self->pin->number), self->idle_state);

        GPIO_InitStruct.Pin = pin_mask(self->pin->number);
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(pin_port(self->pin->port), &GPIO_InitStruct);
    }

    self->first_edge = true;
    self->paused = false;
    self->last_ms = 0;
    self->last_us = 0;

    HAL_NVIC_EnableIRQ(self->irq);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t* self) {
    HAL_NVIC_DisableIRQ(self->irq);
    self->start = 0;
    self->len = 0;
    HAL_NVIC_EnableIRQ(self->irq);
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t* self, int16_t index) {
    HAL_NVIC_DisableIRQ(self->irq);
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        HAL_NVIC_EnableIRQ(self->irq);
        mp_raise_IndexError(translate("index out of range"));
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    HAL_NVIC_EnableIRQ(self->irq);
    return value;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t* self) {
    if (self->len == 0) {
        mp_raise_IndexError(translate("pop from an empty PulseIn"));
    }
    HAL_NVIC_DisableIRQ(self->irq);
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    HAL_NVIC_EnableIRQ(self->irq);

    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t* self) {
    return self->maxlen;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t* self) {
    return self->paused;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t* self) {
    return self->len;
}

static void assign_EXTI_Interrupt(pulseio_pulsein_obj_t* self, uint8_t num) {
    if (num == 0) {
        self->irq = EXTI0_IRQn;
    } else if (num == 1) {
        self->irq = EXTI1_IRQn;
    } else if (num == 2) {
        self->irq = EXTI2_IRQn;
    } else if (num == 3) {
        self->irq = EXTI3_IRQn;
    } else if (num == 4) {
        self->irq = EXTI4_IRQn;
    } else if (num >= 5 && num <= 9 ) {
        self->irq = EXTI9_5_IRQn;
    } else if (num >= 10 && num <= 15) {
        self->irq = EXTI15_10_IRQn;
    }
}

void EXTI0_IRQHandler(void)
{
    pulsein_handler(0);
}
void EXTI1_IRQHandler(void)
{
    pulsein_handler(1);
}
void EXTI2_IRQHandler(void)
{
    pulsein_handler(2);
}
void EXTI3_IRQHandler(void)
{
    pulsein_handler(3);
}
void EXTI4_IRQHandler(void)
{
    pulsein_handler(4);
}

void EXTI9_5_IRQHandler(void)
{
    uint32_t pending = EXTI->PR;
    for (uint i = 5; i <= 9; i++) {
        if(pending & (1 << i)) {
            pulsein_handler(i);
        }
    }
}

void EXTI15_10_IRQHandler(void)
{
    uint32_t pending = EXTI->PR;
    for (uint i = 10; i <= 15; i++) {
        if(pending & (1 << i)) {
            pulsein_handler(i);
        }
    }
}
