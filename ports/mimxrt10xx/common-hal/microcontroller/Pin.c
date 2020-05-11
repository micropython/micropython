/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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
#include "supervisor/shared/rgb_led_status.h"

#ifdef MICROPY_HW_NEOPIXEL
bool neopixel_in_use;
#endif

#define GPIO_PORT_COUNT (IOMUXC_SW_PAD_CTL_PAD_COUNT / 32 + 1)

// GPIO ports are 32 pins wide
STATIC uint32_t claimed_pins[GPIO_PORT_COUNT]; //remove?
STATIC uint32_t never_reset_pins[GPIO_PORT_COUNT];

// There are two numbering systems used here:
// IOMUXC index, used for iterating through pins and accessing reset information, 
// and GPIO port and number, used to store claimed and reset tagging. The two number 
// systems are not related and one cannot determine the other without a pin object
void reset_all_pins(void) {
    for (uint8_t i = 0; i < GPIO_PORT_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < IOMUXC_SW_PAD_CTL_PAD_COUNT; i++) {
        // mp_printf(&mp_plat_print, "CTL MUX i=%d:%x\n",i,IOMUXC->SW_MUX_CTL_PAD[i]);
        // mp_printf(&mp_plat_print, "CTL PAD i=%d:%x\n",i,IOMUXC->SW_PAD_CTL_PAD[i]);
        if(!(never_reset_pins[mcu_pin_list[i].port] & (1 << mcu_pin_list[i].number))) {
            // mp_printf(&mp_plat_print, "SUCCESS\n");
            IOMUXC->SW_MUX_CTL_PAD[i] = mcu_pin_list[i].mux_reset;
            IOMUXC->SW_PAD_CTL_PAD[i] = mcu_pin_list[i].pad_reset;
        }
    }

    #ifdef MICROPY_HW_NEOPIXEL
    neopixel_in_use = false;
    #endif
}

void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    never_reset_pins[pin_port] |= 1 << pin_number;
}

// Since i.MX pins need extra register and reset information to reset properly, 
// resetting pins by number alone has been removed. 
void common_hal_reset_pin(const mcu_pin_obj_t* pin) {
    never_reset_pins[pin->port] &= ~(1 << pin->number);
    claimed_pins[pin->port] &= ~(1 << pin->number);
    *(uint32_t*)pin->mux_reg = pin->mux_reset;
    *(uint32_t*)pin->cfg_reg = pin->pad_reset;

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
}

void common_hal_never_reset_pin(const mcu_pin_obj_t* pin) {
    never_reset_pin_number(pin->port, pin->number);
}

void claim_pin(const mcu_pin_obj_t* pin) {
    claimed_pins[pin->port] |= 1 << pin->number;

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = true;
    }
    #endif
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        return !neopixel_in_use;
    }
    #endif

    return !(claimed_pins[pin->port] & 1<<pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t* pin) {
    return pin->mux_idx; // returns IOMUXC to align with pin table
    // Note: IOMUXC "numbers" do not align cleanly with GPIO values
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t* pin) {
    claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    common_hal_reset_pin(&mcu_pin_list[pin_no]);
}

// TODO: replace use of GPIO pointers in pin struct with this system?
// GPIO_TypeDef * pin_port(uint8_t pin_port) {}
// uint16_t pin_mask(uint8_t pin_number) {}
