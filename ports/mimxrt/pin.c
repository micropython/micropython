/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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
#include "pin.h"


void pin_init(void) {
    return;
}

uint32_t pin_get_mode(const machine_pin_obj_t *pin) {
    uint32_t pin_mode = PIN_MODE_ALT;
    uint32_t config_register = *((volatile uint32_t *)pin->configRegister);
    uint8_t af_mode = pin_get_af(pin);

    if (af_mode == PIN_AF_MODE_ALT5) {
        bool open_drain_enabled = (config_register & IOMUXC_SW_PAD_CTL_PAD_ODE_MASK) >> IOMUXC_SW_PAD_CTL_PAD_ODE_SHIFT;

        if (open_drain_enabled) {
            pin_mode = PIN_MODE_OPEN_DRAIN;
        } else {
            // Check pin direction
            if ((pin->gpio->GDIR & (1U << pin->pin)) >> pin->pin) {
                pin_mode = PIN_MODE_OUT;
            } else {
                pin_mode = PIN_MODE_IN;
            }
        }
    }

    return pin_mode;
}

uint32_t pin_get_af(const machine_pin_obj_t *pin) {
    uint32_t mux_register = *((volatile uint32_t *)pin->muxRegister);

    // Read configured AF-Mode of pin
    return (uint32_t)(mux_register & IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK) >> IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT;
}

uint32_t pin_generate_config(const uint32_t pull, const uint32_t mode, const uint32_t drive, uint32_t config_register) {
    uint32_t pad_config = 0x0UL;

    #if defined MIMXRT117x_SERIES

    // Set Pull-up
    if ((config_register >= 0x400E8350 && config_register <= 0x400E83dc) ||   // GPIO_AD_xx
        (config_register >= 0x40C08040 && config_register <= 0x40C0807C)) {   // GPIO_LPSR_xx
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_SRE(0U); // Set slew rate; there is a discrepancy between doc and header file
        if (pull != PIN_PULL_DISABLED) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PUE(1) | // Pull Enabled
                IOMUXC_SW_PAD_CTL_PAD_PUS(pull != PIN_PULL_DOWN_100K); // Up or DOWn
        }
    } else { // GPIO_SD_Bx_xx
        if (pull == PIN_PULL_DISABLED) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PULL(0b11);
        } else if (pull == PIN_PULL_DOWN_100K) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PULL(0b10);
        } else {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PULL(0b01);
        }
    }
    // Set open Drain; different for LPSR GPIO!
    if (config_register >= 0x40C08040 && config_register <= 0x40C0807C) {  // GPIO_LPSR_xx
        if (mode == PIN_MODE_OPEN_DRAIN) {
            pad_config |= 1 << 5;  // Open Drain Enabled, no Macro provided
        }
    } else {
        if (mode == PIN_MODE_OPEN_DRAIN) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b1);  // Open Drain Enabled
        }
    }
    // Set drive strength
    if (mode != PIN_MODE_IN) {
        if (!IS_GPIO_DRIVE(drive)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid drive strength: %d"), drive);
        }
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_DSE(drive >= PIN_DRIVE_4);
    }

    #else

    pad_config |= IOMUXC_SW_PAD_CTL_PAD_SRE(0U);      // Slow Slew Rate
    pad_config |= IOMUXC_SW_PAD_CTL_PAD_SPEED(0b01);      // medium(100MHz)

    if (mode == PIN_MODE_OPEN_DRAIN) {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b1);  // Open Drain Enabled
    } else {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b0);  // Open Drain Disabled
    }

    if (pull == PIN_PULL_DISABLED) {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(0); // Pull/Keeper Disabled
    } else if (pull == PIN_PULL_HOLD) {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(1) | // Pull/Keeper Enabled
            IOMUXC_SW_PAD_CTL_PAD_PUE(0);            // Keeper selected
    } else {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(1) |  // Pull/Keeper Enabled
            IOMUXC_SW_PAD_CTL_PAD_PUE(1) |            // Pull selected
            IOMUXC_SW_PAD_CTL_PAD_PUS(pull);
    }

    if (mode == PIN_MODE_IN) {
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_DSE(0b000) |  // output driver disabled
            IOMUXC_SW_PAD_CTL_PAD_HYS(1U);  // Hysteresis enabled
    } else {

        if (!IS_GPIO_DRIVE(drive)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid drive strength: %d"), drive);
        }

        pad_config |= IOMUXC_SW_PAD_CTL_PAD_DSE(drive) |
            IOMUXC_SW_PAD_CTL_PAD_HYS(0U);  // Hysteresis disabled
    }

    #endif

    return pad_config;
}

const machine_pin_obj_t *pin_find(mp_obj_t user_obj) {
    const machine_pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (mp_obj_is_type(user_obj, &machine_pin_type)) {
        pin_obj = user_obj;
        return pin_obj;
    }

    // If pin is SMALL_INT
    if (mp_obj_is_small_int(user_obj)) {
        uint8_t value = MP_OBJ_SMALL_INT_VALUE(user_obj);
        if (value < num_board_pins) {
            return machine_pin_board_pins[value];
        }
    }

    // See if the pin name matches a board pin
    pin_obj = pin_find_named_pin(&machine_pin_board_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    // See if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(&machine_pin_cpu_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't exist"));
}

const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    mp_map_t *named_map = mp_obj_dict_get_map((mp_obj_t)named_pins);
    mp_map_elem_t *named_elem = mp_map_lookup(named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return named_elem->value;
    }
    return NULL;
}

const machine_pin_af_obj_t *pin_find_af(const machine_pin_obj_t *pin, uint8_t fn) {
    const machine_pin_af_obj_t *af_obj = NULL;

    for (int i = 0; i < pin->af_list_len; ++i) {
        af_obj = &pin->af_list[i];
        if (af_obj->af_mode == fn) {
            return af_obj;
        }
    }

    return NULL;
}

const machine_pin_af_obj_t *pin_find_af_by_base(const machine_pin_obj_t *pin, void *base_ptr[], size_t base_size) {
    const machine_pin_af_obj_t *af_obj = NULL;

    for (int i = 0; i < pin->af_list_len; ++i) {
        af_obj = &pin->af_list[i];
        for (int j = 0; j < base_size; ++j) {
            if (af_obj->instance == base_ptr[j]) {
                return af_obj;
            }
        }
    }

    return NULL;
}

const machine_pin_af_obj_t *pin_find_af_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx) {
    // TODO: Implement pin_find_af_by_index function
    return NULL;
}

const machine_pin_af_obj_t *pin_find_af_by_name(const machine_pin_obj_t *pin, const char *name) {
    // TODO: Implement pin_find_af_by_name function
    return NULL;
}
