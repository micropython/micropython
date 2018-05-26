/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/rotaryio/IncrementalEncoder.h"

#include <stdint.h>

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "mpconfigport.h"
#include "peripherals/pins.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"

#ifdef SAMD21
#include "hpl/gclk/hpl_gclk_base.h"
#endif

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t* self,
        const mcu_pin_obj_t* pin, uint16_t maxlen, bool idle_state) {
//     if (!pin->has_extint) {
//         mp_raise_RuntimeError("No hardware support on pin");
//     }
//     uint32_t mask = 1 << pin->extint_channel;
//     if (active_incrementalencoders[pin->extint_channel] != NULL ||
//         (eic_get_enable() == 1 &&
// #ifdef SAMD51
//             ((EIC->INTENSET.bit.EXTINT & mask) != 0 ||
//              (EIC->EVCTRL.bit.EXTINTEO & mask) != 0))) {
// #endif
// #ifdef SAMD21
//             ((EIC->INTENSET.vec.EXTINT & mask) != 0 ||
//              (EIC->EVCTRL.vec.EXTINTEO & mask) != 0))) {
// #endif
//         mp_raise_RuntimeError("EXTINT channel already in use");
//     }
//
//     self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
//     if (self->buffer == NULL) {
//         mp_raise_msg_varg(&mp_type_MemoryError, "Failed to allocate RX buffer of %d bytes", maxlen * sizeof(uint16_t));
//     }
//     self->channel = pin->extint_channel;
//     self->pin = pin->pin;
//     self->maxlen = maxlen;
//     self->idle_state = idle_state;
//     self->start = 0;
//     self->len = 0;
//     self->first_edge = true;
//
//     active_incrementalencoders[pin->extint_channel] = self;
//
//     // Check to see if the EIC is enabled and start it up if its not.'
//     // SAMD51 EIC can only be clocked up to 100mhz so we use the 48mhz clock.
//     if (eic_get_enable() == 0) {
//         #ifdef SAMD51
//         MCLK->APBAMASK.bit.EIC_ = true;
//         hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID,
//                                    GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));
//         #endif
//
//         #ifdef SAMD21
//         PM->APBAMASK.bit.EIC_ = true;
//         _gclk_enable_channel(EIC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
//         #endif
//
//
//         #ifdef SAMD21
//         NVIC_DisableIRQ(EIC_IRQn);
//         NVIC_ClearPendingIRQ(EIC_IRQn);
//         NVIC_EnableIRQ(EIC_IRQn);
//         #endif
//     }
//
//     gpio_set_pin_function(pin->pin, GPIO_PIN_FUNCTION_A);
//
//     #ifdef SAMD51
//     NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
//     NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
//     NVIC_EnableIRQ(EIC_0_IRQn + self->channel);
//     #endif
//
//     // Set config will enable the EIC.
//     incrementalencoder_set_config(self, true);
//     EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t* self) {
    //return self->pin == NO_PIN;
    return true;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t* self) {
    // if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
    //     return;
    // }
    // uint32_t mask = 1 << self->channel;
    // EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
    // #ifdef SAMD51
    // NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    // NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    // #endif
    // active_incrementalencoders[self->channel] = NULL;
    // reset_pin(self->pin);
    // self->pin = NO_PIN;
    //
    // bool all_null = true;
    // for (uint8_t i = 0; all_null && i < 16; i++) {
    //     all_null = all_null && active_incrementalencoders[i] == NULL;
    // }
    // #ifdef SAMD21
    // if (all_null && EIC->INTENSET.reg == 0) {
    //     NVIC_DisableIRQ(EIC_IRQn);
    //     NVIC_ClearPendingIRQ(EIC_IRQn);
    // }
    // #endif
    // // Test if all channels are null and deinit everything if they are.
    // if (all_null && EIC->EVCTRL.reg == 0 && EIC->INTENSET.reg == 0) {
    //     eic_set_enable(false);
    //     #ifdef SAMD51
    //     MCLK->APBAMASK.bit.EIC_ = false;
    //     hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, 0);
    //     #endif
    //
    //     #ifdef SAMD21
    //     PM->APBAMASK.bit.EIC_ = false;
    //     hri_gclk_write_CLKCTRL_reg(GCLK, GCLK_CLKCTRL_ID(EIC_GCLK_ID));
    //     #endif
    // }
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t* self) {
    return 0;
}
