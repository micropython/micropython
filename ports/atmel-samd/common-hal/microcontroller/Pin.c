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

#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "samd/pins.h"

#ifdef SPEAKER_ENABLE_PIN
bool speaker_enable_in_use;
#endif

#define PORT_COUNT (PORT_BITS / 32 + 1)

#ifdef SAM_D5X_E5X
#define SWD_MUX GPIO_PIN_FUNCTION_H
#endif
#ifdef SAMD21
#define SWD_MUX GPIO_PIN_FUNCTION_G
#endif

STATIC uint32_t never_reset_pins[PORT_COUNT];

void reset_all_pins(void) {
    uint32_t pin_mask[PORT_COUNT] = PORT_OUT_IMPLEMENTED;

    // Do not full reset USB lines.
    pin_mask[0] &= ~(PORT_PA24 | PORT_PA25);

    // Do not reset SWD when a debugger is present.
    if (DSU->STATUSB.bit.DBGPRES == 1) {
        pin_mask[0] &= ~(PORT_PA30 | PORT_PA31);
    }

    for (uint32_t i = 0; i < PORT_COUNT; i++) {
        pin_mask[i] &= ~never_reset_pins[i];
    }

    gpio_set_port_direction(GPIO_PORTA, pin_mask[0] & ~MICROPY_PORT_A, GPIO_DIRECTION_OFF);
    gpio_set_port_direction(GPIO_PORTB, pin_mask[1] & ~MICROPY_PORT_B, GPIO_DIRECTION_OFF);
    #if PORT_BITS > 64
    gpio_set_port_direction(GPIO_PORTC, pin_mask[2] & ~MICROPY_PORT_C, GPIO_DIRECTION_OFF);
    #endif
    #if PORT_BITS > 96
    gpio_set_port_direction(GPIO_PORTD, pin_mask[3] & ~MICROPY_PORT_D, GPIO_DIRECTION_OFF);
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
    speaker_enable_in_use = false;
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
        speaker_enable_in_use = false;
        gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(SPEAKER_ENABLE_PIN->number, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(SPEAKER_ENABLE_PIN->number, false);
    }
    #endif
}

void common_hal_never_reset_pin(const mcu_pin_obj_t* pin) {
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t* pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t* pin) {
    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        speaker_enable_in_use = true;
    }
    #endif
}

bool pin_number_is_free(uint8_t pin_number) {
    PortGroup *const port = &PORT->Group[(enum gpio_port)GPIO_PORT(pin_number)];
    uint8_t pin_index = GPIO_PIN(pin_number);
    volatile PORT_PINCFG_Type *state = &port->PINCFG[pin_index];
    volatile PORT_PMUX_Type *pmux = &port->PMUX[pin_index / 2];

    if (pin_number == PIN_PA30 || pin_number == PIN_PA31) {
        if (DSU->STATUSB.bit.DBGPRES == 1) {
            return false;
        }
        if (pin_number == PIN_PA30
            #ifdef SAM_D5X_E5X
            ) {
            #endif
            #ifdef SAMD21
            || pin_number == PIN_PA31) {
            #endif) {
            return state->bit.PMUXEN == 1 && ((pmux->reg >> (4 * pin_index % 2)) & 0xf) == SWD_MUX;
        }
    }

    return state->bit.PMUXEN == 0 && state->bit.INEN == 0 &&
           state->bit.PULLEN == 0 && (port->DIR.reg & (1 << pin_index)) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        return !speaker_enable_in_use;
    }
    #endif

    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t* pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t* pin) {
    return claim_pin(pin);
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
    mp_raise_ValueError_varg(translate("%q pin invalid"), name);
}
