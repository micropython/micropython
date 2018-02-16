/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "samd21_pins.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"

#ifdef SAMD21
#include "hpl/gclk/hpl_gclk_base.h"
#endif

#include "tick.h"

static pulseio_pulsein_obj_t *active_pulseins[EIC_EXTINT_NUM];
static uint64_t last_ms[EIC_EXTINT_NUM];
static uint16_t last_us[EIC_EXTINT_NUM];

bool eic_get_enable(void) {
    #ifdef SAMD51
    return EIC->CTRLA.bit.ENABLE;
    #endif
    #ifdef SAMD21
    return EIC->CTRL.bit.ENABLE;
    #endif
}

void eic_set_enable(bool value) {
    #ifdef SAMD51
    EIC->CTRLA.bit.ENABLE = value;
    while (EIC->SYNCBUSY.bit.ENABLE != 0) {}
    // This won't actually block long enough in Rev A of SAMD51 and will miss edges in the first
    // three cycles of the peripheral clock. See the errata for details. It shouldn't impact us.
    #endif
    #ifdef SAMD21
    EIC->CTRL.bit.ENABLE = value;
    while (EIC->STATUS.bit.SYNCBUSY != 0) {}
    #endif
}

void eic_reset(void) {
    #ifdef SAMD51
    EIC->CTRLA.bit.SWRST = true;
    while (EIC->SYNCBUSY.bit.SWRST != 0) {}
    // This won't actually block long enough in Rev A of SAMD51 and will miss edges in the first
    // three cycles of the peripheral clock. See the errata for details. It shouldn't impact us.
    #endif
    #ifdef SAMD21
    EIC->CTRL.bit.SWRST = true;
    while (EIC->STATUS.bit.SYNCBUSY != 0) {}
    #endif
}

void pulsein_reset(void) {
    for (int i = 0; i < EIC_EXTINT_NUM; i++) {
        active_pulseins[i] = NULL;
        last_ms[i] = 0;
        last_us[i] = 0;
        #ifdef SAMD51
        NVIC_DisableIRQ(EIC_0_IRQn + i);
        NVIC_ClearPendingIRQ(EIC_0_IRQn + i);
        #endif
    }
    eic_reset();
    #ifdef SAMD21
    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
    #endif
}

static void pulsein_set_config(pulseio_pulsein_obj_t* self, bool first_edge) {
    uint8_t sense_setting = EIC_CONFIG_FILTEN0;
    if (!first_edge) {
        sense_setting |= EIC_CONFIG_SENSE0_BOTH_Val;
    } else if (self->idle_state) {
        sense_setting |= EIC_CONFIG_SENSE0_FALL_Val;
    } else {
        sense_setting |= EIC_CONFIG_SENSE0_RISE_Val;
    }
    eic_set_enable(false);
    uint8_t config_index = self->channel / 8;
    uint8_t position = (self->channel % 8) * 4;
    uint32_t masked_value = EIC->CONFIG[config_index].reg & ~(0xf << position);
    EIC->CONFIG[config_index].reg = masked_value | (sense_setting << position);
    eic_set_enable(true);
}

static void pulsein_interrupt_handler(uint8_t channel) {
    // Grab the current time first.
    uint32_t current_us;
    uint64_t current_ms;
    current_tick(&current_ms, &current_us);
    // current_tick gives us the remaining us until the next tick but we want the number since the
    // last ms.
    current_us = 1000 - current_us;
    pulseio_pulsein_obj_t* self = active_pulseins[channel];
    if (self->first_edge) {
        self->first_edge = false;
        pulsein_set_config(self, false);
    } else {
        uint32_t ms_diff = current_ms - last_ms[self->channel];
        uint16_t us_diff = current_us - last_us[self->channel];
        uint32_t total_diff = us_diff;
        if (last_us[self->channel] > current_us) {
            total_diff = 1000 + current_us - last_us[self->channel];
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
    last_ms[self->channel] = current_ms;
    last_us[self->channel] = current_us;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self,
        const mcu_pin_obj_t* pin, uint16_t maxlen, bool idle_state) {
    if (!pin->has_extint) {
        mp_raise_RuntimeError("No hardware support on pin");
    }
    uint32_t mask = 1 << pin->extint_channel;
    if (active_pulseins[pin->extint_channel] != NULL ||
        (eic_get_enable() == 1 &&
#ifdef SAMD51
            ((EIC->INTENSET.bit.EXTINT & mask) != 0 ||
             (EIC->EVCTRL.bit.EXTINTEO & mask) != 0))) {
#endif
#ifdef SAMD21
            ((EIC->INTENSET.vec.EXTINT & mask) != 0 ||
             (EIC->EVCTRL.vec.EXTINTEO & mask) != 0))) {
#endif
        mp_raise_RuntimeError("EXTINT channel already in use");
    }

    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, "Failed to allocate RX buffer of %d bytes", maxlen * sizeof(uint16_t));
    }
    self->channel = pin->extint_channel;
    self->pin = pin->pin;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->first_edge = true;

    active_pulseins[pin->extint_channel] = self;

    // Check to see if the EIC is enabled and start it up if its not.'
    // SAMD51 EIC can only be clocked up to 100mhz so we use the 48mhz clock.
    if (eic_get_enable() == 0) {
        #ifdef SAMD51
        MCLK->APBAMASK.bit.EIC_ = true;
        hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID,
                                   GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));
        #endif

        #ifdef SAMD21
        PM->APBAMASK.bit.EIC_ = true;
        _gclk_enable_channel(EIC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
        #endif


        #ifdef SAMD21
        NVIC_DisableIRQ(EIC_IRQn);
        NVIC_ClearPendingIRQ(EIC_IRQn);
        NVIC_EnableIRQ(EIC_IRQn);
        #endif
    }

    gpio_set_pin_function(pin->pin, GPIO_PIN_FUNCTION_A);

    #ifdef SAMD51
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    NVIC_EnableIRQ(EIC_0_IRQn + self->channel);
    #endif

    // Set config will enable the EIC.
    pulsein_set_config(self, true);
    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    uint32_t mask = 1 << self->channel;
    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
    #ifdef SAMD51
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    #endif
    active_pulseins[self->channel] = NULL;
    reset_pin(self->pin);
    self->pin = NO_PIN;

    bool all_null = true;
    for (uint8_t i = 0; all_null && i < 16; i++) {
        all_null = all_null && active_pulseins[i] == NULL;
    }
    #ifdef SAMD21
    if (all_null && EIC->INTENSET.reg == 0) {
        NVIC_DisableIRQ(EIC_IRQn);
        NVIC_ClearPendingIRQ(EIC_IRQn);
    }
    #endif
    // Test if all channels are null and deinit everything if they are.
    if (all_null && EIC->EVCTRL.reg == 0 && EIC->INTENSET.reg == 0) {
        eic_set_enable(false);
        #ifdef SAMD51
        MCLK->APBAMASK.bit.EIC_ = false;
        hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, 0);
        #endif

        #ifdef SAMD21
        PM->APBAMASK.bit.EIC_ = false;
        hri_gclk_write_CLKCTRL_reg(GCLK, GCLK_CLKCTRL_ID(EIC_GCLK_ID));
        #endif
    }
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    uint32_t mask = 1 << self->channel;
    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self,
        uint16_t trigger_duration) {
    // Make sure we're paused.
    common_hal_pulseio_pulsein_pause(self);

    // Send the trigger pulse.
    if (trigger_duration > 0) {
        gpio_set_pin_pull_mode(self->pin, GPIO_PULL_OFF);
        gpio_set_pin_direction(self->pin, GPIO_DIRECTION_OUT);

        common_hal_mcu_disable_interrupts();
        gpio_set_pin_level(self->pin, !self->idle_state);
        common_hal_mcu_delay_us(trigger_duration);
        gpio_set_pin_level(self->pin, self->idle_state);
        common_hal_mcu_enable_interrupts();
    }

    // Reconfigure the pin and make sure its set to detect the first edge.
    last_ms[self->channel] = 0;
    last_us[self->channel] = 0;
    self->first_edge = true;
    gpio_set_pin_function(self->pin, GPIO_PIN_FUNCTION_A);
    uint32_t mask = 1 << self->channel;
    // Clear previous interrupt state and re-enable it.
    EIC->INTFLAG.reg = mask << EIC_INTFLAG_EXTINT_Pos;
    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;

    pulsein_set_config(self, true);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t* self) {
    common_hal_mcu_disable_interrupts();
    self->start = 0;
    self->len = 0;
    common_hal_mcu_enable_interrupts();
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t* self) {
    if (self->len == 0) {
        mp_raise_IndexError("pop from an empty PulseIn");
    }
    common_hal_mcu_disable_interrupts();
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    common_hal_mcu_enable_interrupts();

    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t* self) {
    return self->maxlen;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t* self) {
    return self->len;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t* self,
        int16_t index) {
    common_hal_mcu_disable_interrupts();
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        common_hal_mcu_enable_interrupts();
        mp_raise_IndexError("index out of range");
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    common_hal_mcu_enable_interrupts();
    return value;
}

void external_interrupt_handler(uint8_t channel) {
    pulsein_interrupt_handler(channel);
    EIC->INTFLAG.reg = (1 << channel) << EIC_INTFLAG_EXTINT_Pos;
}

#ifdef SAMD21
void EIC_Handler(void) {
    for (uint8_t i = 0; i < 16; i++) {
        if ((EIC->INTFLAG.vec.EXTINT & (1 << i)) != 0) {
            external_interrupt_handler(i);
        }
    }
}
#endif

#ifdef SAMD51
void EIC_0_Handler(void) {
    external_interrupt_handler(0);
}
void EIC_1_Handler(void) {
    external_interrupt_handler(1);
}
void EIC_2_Handler(void) {
    external_interrupt_handler(2);
}
void EIC_3_Handler(void) {
    external_interrupt_handler(3);
}
void EIC_4_Handler(void) {
    external_interrupt_handler(4);
}
void EIC_5_Handler(void) {
    external_interrupt_handler(5);
}
void EIC_6_Handler(void) {
    external_interrupt_handler(6);
}
void EIC_7_Handler(void) {
    external_interrupt_handler(7);
}
void EIC_8_Handler(void) {
    external_interrupt_handler(8);
}
void EIC_9_Handler(void) {
    external_interrupt_handler(9);
}
void EIC_10_Handler(void) {
    external_interrupt_handler(10);
}
void EIC_11_Handler(void) {
    external_interrupt_handler(11);
}
void EIC_12_Handler(void) {
    external_interrupt_handler(12);
}
void EIC_13_Handler(void) {
    external_interrupt_handler(13);
}
void EIC_14_Handler(void) {
    external_interrupt_handler(14);
}
void EIC_15_Handler(void) {
    external_interrupt_handler(15);
}
#endif
