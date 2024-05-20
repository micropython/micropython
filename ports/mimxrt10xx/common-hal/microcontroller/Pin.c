// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "sdk/drivers/igpio/fsl_gpio.h"

#include "py/gc.h"

static bool claimed_pins[PAD_COUNT];
static bool never_reset_pins[PAD_COUNT];

// Default is that no pins are forbidden to reset.
MP_WEAK const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[] = {
    NULL,
};

static bool _reset_forbidden(const mcu_pin_obj_t *pin) {
    const mcu_pin_obj_t **forbidden_pin = &mimxrt10xx_reset_forbidden_pins[0];
    while (*forbidden_pin) {
        if (pin == *forbidden_pin) {
            return true;
        }
        forbidden_pin++;
    }
    return false;
}

// There are two numbering systems used here:
// IOMUXC index, used for iterating through pins and accessing reset information,
// and GPIO port and number, used to store claimed and reset tagging. The two number
// systems are not related and one cannot determine the other without a pin object
void reset_all_pins(void) {
    for (uint8_t i = 0; i < PAD_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < PAD_COUNT; i++) {
        mcu_pin_obj_t *pin = mcu_pin_globals.map.table[i].value;
        if (never_reset_pins[pin->mux_idx]) {
            continue;
        }
        common_hal_reset_pin(pin);
    }
}

MP_WEAK bool mimxrt10xx_board_reset_pin_number(const mcu_pin_obj_t *pin) {
    return false;
}

// Since i.MX pins need extra register and reset information to reset properly,
// resetting pins by number alone has been removed.
void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }

    if (_reset_forbidden(pin)) {
        return;
    }

    // Give the board a chance to reset the pin in a particular way, or not reset it at all.
    if (mimxrt10xx_board_reset_pin_number(pin)) {
        return;
    }

    disable_pin_change_interrupt(pin);
    never_reset_pins[pin->mux_idx] = false;
    claimed_pins[pin->mux_idx] = false;

    // Make sure this pin's GPIO is set to input. Otherwise, output values could interfere
    // with the ADC.
    const gpio_pin_config_t config = { kGPIO_DigitalInput, 0, kGPIO_NoIntmode };
    GPIO_PinInit(pin->gpio, pin->number, &config);

    // This should never be true, but protect against it anyway.
    if (pin->mux_reg == 0) {
        return;
    }
    *(uint32_t *)pin->mux_reg = pin->mux_reset;
    *(uint32_t *)pin->cfg_reg = pin->pad_reset;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pins[pin->mux_idx] = true;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return !claimed_pins[pin->mux_idx];
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->mux_idx; // returns IOMUXC to align with pin table
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claimed_pins[pin->mux_idx] = true;
}

void claim_pin(const mcu_pin_obj_t *pin) {
    common_hal_mcu_pin_claim(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    common_hal_reset_pin((mcu_pin_obj_t *)(mcu_pin_globals.map.table[pin_no].value));
}

const mcu_periph_obj_t *find_pin_function_sz(const mcu_periph_obj_t *list, size_t sz, const mcu_pin_obj_t *pin, int *instance, uint16_t name) {
    if (!pin) {
        return NULL;
    }
    for (size_t i = 0; i < sz; i++) {
        if (*instance != -1 && *instance != list[i].bank_idx) {
            continue;
        }
        if (pin == list[i].pin) {
            *instance = list[i].bank_idx;
            return &list[i];
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q pin"), name);
}
