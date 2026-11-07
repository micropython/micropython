// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "nrf_gpio.h"
#include "py/mphal.h"

#include "nrf/pins.h"

// Bit mask of claimed pins on each of up to two ports. nrf52832 has one port; nrf52840 has two.
static uint32_t claimed_pins[GPIO_COUNT];

#ifdef SPEAKER_ENABLE_PIN
static void reset_speaker_enable_pin(void) {
    nrf_gpio_cfg(SPEAKER_ENABLE_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(SPEAKER_ENABLE_PIN->number, false);
}
#endif

MP_WEAK bool board_reset_pin_number(uint8_t pin_number) {
    return false;
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }

    // Clear claimed bit.
    claimed_pins[nrf_pin_port(pin_number)] &= ~(1 << nrf_relative_pin_number(pin_number));

    #ifdef SPEAKER_ENABLE_PIN
    if (pin_number == SPEAKER_ENABLE_PIN->number) {
        reset_speaker_enable_pin();
    }
    #endif

    // Allow the board to override the reset state of any pin.
    board_reset_pin_number(pin_number);
}


void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin_number(uint8_t pin_number) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[nrf_pin_port(pin_number)] |= 1 << nrf_relative_pin_number(pin_number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    claim_pin_number(pin->number);
}


bool pin_number_is_free(uint8_t pin_number) {
    return !(claimed_pins[nrf_pin_port(pin_number)] & (1 << nrf_relative_pin_number(pin_number)));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
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

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}
