// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "samd/pins.h"

#define PORT_COUNT (PORT_BITS / 32 + 1)

#ifdef SAM_D5X_E5X
#define SWD_MUX GPIO_PIN_FUNCTION_H
#endif
#ifdef SAMD21
#define SWD_MUX GPIO_PIN_FUNCTION_G
#endif

static uint32_t claimed_pins[PORT_COUNT];

void reset_pin_number(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return;
    }

    claimed_pins[GPIO_PORT(pin_number)] &= ~(1 << GPIO_PIN(pin_number));

    if (pin_number == PIN_PA30
        #ifdef SAM_D5X_E5X
        ) {
        #endif
        #ifdef SAMD21
        || pin_number == PIN_PA31) {
        #endif
        gpio_set_pin_function(pin_number, SWD_MUX);
    } else {
        gpio_set_pin_direction(pin_number, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_OFF);
    }

    #ifdef SPEAKER_ENABLE_PIN
    if (pin_number == SPEAKER_ENABLE_PIN->number) {
        gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(SPEAKER_ENABLE_PIN->number, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(SPEAKER_ENABLE_PIN->number, false);
    }
    #endif
}


void claim_pin_number(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return;
    }

    claimed_pins[GPIO_PORT(pin_number)] |= 1 << GPIO_PIN(pin_number);
}

void claim_pin(const mcu_pin_obj_t* pin) {
    claim_pin_number(pin->number);
}

bool pin_number_is_free(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return false;
    }

    // SWD pins are in use while a debugger is attached.
    if ((pin_number == PIN_PA30 || pin_number == PIN_PA31) && DSU->STATUSB.bit.DBGPRES == 1) {
        return false;
    }

    return (claimed_pins[GPIO_PORT(pin_number)] & (1 << GPIO_PIN(pin_number))) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t* pin) {
    return pin->number;
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}

mcu_pin_function_t *mcu_find_pin_function(mcu_pin_function_t *table, const mcu_pin_obj_t *pin, int instance, uint16_t name) {
    if (!pin) {
        return NULL;
    }

    for(; table->obj; table++) {
        if (instance != -1 && instance != table->instance) {
            continue;
        }
        if (pin == table->obj) {
            return table;
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q pin"), name);
}
