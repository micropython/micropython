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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "timers.h"

#include "peripherals/exti.h"

#include STM32_HAL_H

#define STM32_GPIO_PORT_SIZE 16
STATIC pulseio_pulsein_obj_t *callback_obj_ref[STM32_GPIO_PORT_SIZE];

STATIC TIM_HandleTypeDef tim_handle;
STATIC uint32_t overflow_count = 0;
STATIC uint8_t refcount = 0;

void pulsein_timer_event_handler(void) {
    // Detect TIM Update event
    if (__HAL_TIM_GET_FLAG(&tim_handle, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&tim_handle, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_IT(&tim_handle, TIM_IT_UPDATE);
            overflow_count++;
        }
    }
}

STATIC void pulsein_exti_event_handler(uint8_t num) {
    // Grab the current time first.
    uint32_t current_overflow = overflow_count;
    uint32_t current_count = tim_handle.Instance->CNT;

    // Interrupt register must be cleared manually
    EXTI->PR = 1 << num;

    pulseio_pulsein_obj_t *self = callback_obj_ref[num];
    if (!self) {
        return;
    }

    if (self->first_edge) {
        // first pulse is opposite state from idle
        bool state = HAL_GPIO_ReadPin(pin_port(self->pin->port), pin_mask(self->pin->number));
        if (self->idle_state != state) {
            self->first_edge = false;
        }
    } else {
        uint32_t total_diff = current_count + 0x10000 * (current_overflow - self->last_overflow) - self->last_count;
        // Cap duration at 16 bits.
        uint16_t duration = MIN(0xffff, total_diff);

        uint16_t i = (self->start + self->len) % self->maxlen;
        self->buffer[i] = duration;
        if (self->len < self->maxlen) {
            self->len++;
        } else {
            self->start++;
        }
    }

    self->last_count = current_count;
    self->last_overflow = current_overflow;
}

void pulsein_reset(void) {
    // Disable all active interrupts and clear array
    for (uint i = 0; i < STM32_GPIO_PORT_SIZE; i++) {
        if (callback_obj_ref[i] != NULL) {
            stm_peripherals_exti_disable(callback_obj_ref[i]->pin->number);
        }
    }
    memset(callback_obj_ref, 0, sizeof(callback_obj_ref));

    HAL_TIM_Base_DeInit(&tim_handle);
    tim_clock_disable(stm_peripherals_timer_get_index(tim_handle.Instance));
    memset(&tim_handle, 0, sizeof(tim_handle));
    refcount = 0;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self, const mcu_pin_obj_t *pin,
    uint16_t maxlen, bool idle_state) {
    // STM32 has one shared EXTI for each pin number, 0-15
    if (!stm_peripherals_exti_is_free(pin->number)) {
        mp_raise_RuntimeError(translate("Pin interrupt already in use"));
    }

    // Allocate pulse buffer
    self->buffer = (uint16_t *)m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        // TODO: free the EXTI here?
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
    self->last_count = 0;
    self->last_overflow = 0;

    if (HAL_TIM_Base_GetState(&tim_handle) == HAL_TIM_STATE_RESET) {
        // Find a suitable timer
        TIM_TypeDef *tim_instance = stm_peripherals_find_timer();
        stm_peripherals_timer_reserve(tim_instance);

        // Set ticks to 1us
        uint32_t source = stm_peripherals_timer_get_source_freq(tim_instance);
        uint32_t prescaler = source / 1000000;

        // Enable clocks and IRQ, set callback
        stm_peripherals_timer_preinit(tim_instance, 4, pulsein_timer_event_handler);

        // Set the new period
        tim_handle.Instance = tim_instance;
        tim_handle.Init.Prescaler = prescaler - 1;
        tim_handle.Init.Period = 0x10000 - 1; // 65 ms period (maximum)
        HAL_TIM_Base_Init(&tim_handle);

        // Set registers manually
        tim_handle.Instance->SR = 0; // Prevent the SR from triggering an interrupt
        tim_handle.Instance->CR1 |= TIM_CR1_CEN; // Resume timer
        tim_handle.Instance->CR1 |= TIM_CR1_URS; // Disable non-overflow interrupts
        __HAL_TIM_ENABLE_IT(&tim_handle, TIM_IT_UPDATE);

        overflow_count = 0;
    }
    // Add to active PulseIns
    refcount++;

    if (!stm_peripherals_exti_reserve(pin->number)) {
        mp_raise_RuntimeError(translate("Pin interrupt already in use"));
    }
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    stm_peripherals_exti_set_callback(pulsein_exti_event_handler, pin->number);
    // Store PulseIn object for use in callback
    callback_obj_ref[pin->number] = self;
    // Interrupt starts immediately
    stm_peripherals_exti_enable(pin->number);

    common_hal_mcu_pin_claim(pin);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    // Remove pulsein slot from shared array
    callback_obj_ref[self->pin->number] = NULL;
    stm_peripherals_exti_free(self->pin->number);
    reset_pin_number(self->pin->port, self->pin->number);
    self->pin = NULL;

    refcount--;
    if (refcount == 0) {
        stm_peripherals_timer_free(tim_handle.Instance);
    }
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
    stm_peripherals_exti_disable(self->pin->number);
    self->paused = true;
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self, uint16_t trigger_duration) {
    // Make sure we're paused.
    if (!self->paused) {
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
    self->last_count = 0;
    self->last_overflow = 0;

    stm_peripherals_exti_enable(self->pin->number);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
    stm_peripherals_exti_disable(self->pin->number);
    self->start = 0;
    self->len = 0;
    stm_peripherals_exti_enable(self->pin->number);
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self, int16_t index) {
    stm_peripherals_exti_disable(self->pin->number);
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        stm_peripherals_exti_enable(self->pin->number);
        mp_raise_IndexError_varg(translate("%q index out of range"), MP_QSTR_PulseIn);
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    stm_peripherals_exti_enable(self->pin->number);
    return value;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
    if (self->len == 0) {
        mp_raise_IndexError_varg(translate("pop from empty %q"), MP_QSTR_PulseIn);
    }
    stm_peripherals_exti_disable(self->pin->number);
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    stm_peripherals_exti_enable(self->pin->number);

    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t *self) {
    return self->maxlen;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t *self) {
    return self->paused;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t *self) {
    return self->len;
}
