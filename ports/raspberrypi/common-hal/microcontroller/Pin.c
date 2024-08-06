// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

static uint64_t gpio_bank0_pin_claimed;

#if CIRCUITPY_CYW43
#include "bindings/cyw43/__init__.h"
#include "pico/cyw43_arch.h"

bool cyw_ever_init;
static uint32_t cyw_pin_claimed;

void reset_pin_number_cyw(uint8_t pin_no) {
    cyw_pin_claimed &= ~(1LL << pin_no);
}
#endif

static uint64_t never_reset_pins;

void reset_all_pins(void) {
    for (size_t i = 0; i < NUM_BANK0_GPIOS; i++) {
        if ((never_reset_pins & (1LL << i)) != 0) {
            continue;
        }
        reset_pin_number(i);
    }
    #if CIRCUITPY_CYW43
    if (cyw_ever_init) {
        // reset LED and SMPS_MODE to Low; don't touch VBUS_SENSE
        // otherwise it is switched to output mode forever!
        cyw43_arch_gpio_put(0, 0);
        cyw43_arch_gpio_put(1, 0);
    }
    cyw_pin_claimed = 0;
    #endif
}

void never_reset_pin_number(uint8_t pin_number) {
    if (pin_number >= NUM_BANK0_GPIOS) {
        return;
    }

    never_reset_pins |= 1LL << pin_number;
}

// By default, all pins get reset in the same way
MP_WEAK bool board_reset_pin_number(uint8_t pin_number) {
    return false;
}

void reset_pin_number(uint8_t pin_number) {
    if (pin_number >= NUM_BANK0_GPIOS) {
        return;
    }

    gpio_bank0_pin_claimed &= ~(1LL << pin_number);
    never_reset_pins &= ~(1LL << pin_number);

    // Allow the board to override the reset state of any pin
    if (board_reset_pin_number(pin_number)) {
        return;
    }

    // We are very aggressive in shutting down the pad fully. Both pulls are
    // disabled and both buffers are as well.
    gpio_init(pin_number);
    hw_clear_bits(&pads_bank0_hw->io[pin_number], PADS_BANK0_GPIO0_IE_BITS |
        PADS_BANK0_GPIO0_PUE_BITS |
        PADS_BANK0_GPIO0_PDE_BITS);
    hw_set_bits(&pads_bank0_hw->io[pin_number], PADS_BANK0_GPIO0_OD_BITS);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    #if CIRCUITPY_CYW43
    if (pin->base.type == &cyw43_pin_type) {
        reset_pin_number_cyw(pin->number);
        return;
    }
    #endif
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    #if CIRCUITPY_CYW43
    if (pin->base.type == &cyw43_pin_type) {
        cyw_pin_claimed |= (1LL << pin->number);
        return;
    }
    #endif
    if (pin->number >= NUM_BANK0_GPIOS) {
        return;
    }
    gpio_bank0_pin_claimed |= (1LL << pin->number);
}

bool pin_number_is_free(uint8_t pin_number) {
    if (pin_number >= NUM_BANK0_GPIOS) {
        return false;
    }
    return !(gpio_bank0_pin_claimed & (1LL << pin_number));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    #if CIRCUITPY_CYW43
    if (pin->base.type == &cyw43_pin_type) {
        return !(cyw_pin_claimed & (1LL << pin->number));
    }
    #endif
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    return claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}
