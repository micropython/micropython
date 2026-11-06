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

static uint32_t never_reset_pins[PORT_COUNT];
static uint32_t claimed_pins[PORT_COUNT];

void reset_all_pins(void) {
    // Keep claims only on pins that keep their configuration.
    for (uint32_t i = 0; i < PORT_COUNT; i++) {
        claimed_pins[i] &= never_reset_pins[i];
    }

    uint32_t pin_mask[PORT_COUNT] = PORT_OUT_IMPLEMENTED;

    // Do not full reset USB lines.
    #if CIRCUITPY_USB_DEVICE
    pin_mask[0] &= ~(PORT_PA24 | PORT_PA25);
    #endif

    // Do not reset SWD when a debugger is present.
    if (DSU->STATUSB.bit.DBGPRES == 1) {
        pin_mask[0] &= ~(PORT_PA30 | PORT_PA31);
    }

    for (uint32_t i = 0; i < PORT_COUNT; i++) {
        pin_mask[i] &= ~never_reset_pins[i];
    }

    gpio_set_port_direction(GPIO_PORTA, pin_mask[0], GPIO_DIRECTION_OFF);
    gpio_set_port_direction(GPIO_PORTB, pin_mask[1], GPIO_DIRECTION_OFF);
    #if PORT_BITS > 64
    gpio_set_port_direction(GPIO_PORTC, pin_mask[2], GPIO_DIRECTION_OFF);
    #endif
    #if PORT_BITS > 96
    gpio_set_port_direction(GPIO_PORTD, pin_mask[3], GPIO_DIRECTION_OFF);
    #endif

    // Configure SWD. SWDIO will be automatically switched on PA31 when a signal is input on
    // SWCLK.
    #ifdef SAM_D5X_E5X
    gpio_set_pin_function(PIN_PA30, MUX_PA30H_CM4_SWCLK);
    #endif
    #ifdef SAMD21
    gpio_set_pin_function(PIN_PA30, GPIO_PIN_FUNCTION_G);
    gpio_set_pin_function(PIN_PA31, GPIO_PIN_FUNCTION_G);
    #endif

    // After configuring SWD because it may be shared.
    #ifdef SPEAKER_ENABLE_PIN
    gpio_set_pin_function(SPEAKER_ENABLE_PIN->number, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(SPEAKER_ENABLE_PIN->number, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(SPEAKER_ENABLE_PIN->number, false);
    #endif
}

void never_reset_pin_number(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return;
    }

    never_reset_pins[GPIO_PORT(pin_number)] |= 1 << GPIO_PIN(pin_number);
}

void reset_pin_number(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return;
    }

    never_reset_pins[GPIO_PORT(pin_number)] &= ~(1 << GPIO_PIN(pin_number));
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

void common_hal_never_reset_pin(const mcu_pin_obj_t* pin) {
    never_reset_pin_number(pin->number);
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
