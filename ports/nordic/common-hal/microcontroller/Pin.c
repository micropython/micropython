/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "nrf_gpio.h"
#include "py/mphal.h"

#include "nrf/pins.h"

#ifdef SPEAKER_ENABLE_PIN
bool speaker_enable_in_use;
#endif

// Bit mask of claimed pins on each of up to two ports. nrf52832 has one port; nrf52840 has two.
STATIC uint32_t claimed_pins[GPIO_COUNT];
STATIC uint32_t never_reset_pins[GPIO_COUNT];

STATIC void reset_speaker_enable_pin(void) {
    #ifdef SPEAKER_ENABLE_PIN
    speaker_enable_in_use = false;
    nrf_gpio_cfg(SPEAKER_ENABLE_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(SPEAKER_ENABLE_PIN->number, false);
    #endif
}

void reset_all_pins(void) {
    for (size_t i = 0; i < GPIO_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }

    for (uint32_t pin = 0; pin < NUMBER_OF_PINS; ++pin) {
        if ((never_reset_pins[nrf_pin_port(pin)] & (1 << nrf_relative_pin_number(pin))) != 0) {
            continue;
        }
        nrf_gpio_cfg_default(pin);
    }

    // After configuring SWD because it may be shared.
    reset_speaker_enable_pin();
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }

    // Clear claimed bit.
    claimed_pins[nrf_pin_port(pin_number)] &= ~(1 << nrf_relative_pin_number(pin_number));
    never_reset_pins[nrf_pin_port(pin_number)] &= ~(1 << nrf_relative_pin_number(pin_number));

    #ifdef SPEAKER_ENABLE_PIN
    if (pin_number == SPEAKER_ENABLE_PIN->number) {
        reset_speaker_enable_pin();
    }
    #endif
}


void never_reset_pin_number(uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    never_reset_pins[nrf_pin_port(pin_number)] |= 1 << nrf_relative_pin_number(pin_number);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[nrf_pin_port(pin->number)] |= 1 << nrf_relative_pin_number(pin->number);

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        speaker_enable_in_use = true;
    }
    #endif
}


bool pin_number_is_free(uint8_t pin_number) {
    return !(claimed_pins[nrf_pin_port(pin_number)] & (1 << nrf_relative_pin_number(pin_number)));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        return !speaker_enable_in_use;
    }
    #endif

    #ifdef NRF52840
    // If NFC pins are enabled for NFC, don't allow them to be used for GPIO.
    if (((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) ==
         (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)) &&
        (pin->number == 9 || pin->number == 10)) {
        return false;
    }
    #endif

    return pin_number_is_free(pin->number);

}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}
