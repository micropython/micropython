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

#include "asf/sam0/drivers/port/port.h"

#include "rgb_led_status.h"
#include "samd21_pins.h"

#ifdef MICROPY_HW_NEOPIXEL
bool neopixel_in_use;
#endif
#ifdef MICROPY_HW_APA102_MOSI
bool apa102_sck_in_use;
bool apa102_mosi_in_use;
#endif
#ifdef SPEAKER_ENABLE_PIN
bool speaker_enable_in_use;
#endif

void reset_all_pins(void) {
    struct system_pinmux_config config;
    system_pinmux_get_config_defaults(&config);
    config.powersave = true;

    uint32_t pin_mask[2] = PORT_OUT_IMPLEMENTED;

    // Do not full reset USB or SWD lines.
    pin_mask[0] &= ~(PORT_PA24 | PORT_PA25 | PORT_PA30 | PORT_PA31);

    system_pinmux_group_set_config(&(PORT->Group[0]), pin_mask[0] & ~MICROPY_PORT_A, &config);
    system_pinmux_group_set_config(&(PORT->Group[1]), pin_mask[1] & ~MICROPY_PORT_B, &config);

    // Configure SWD
    system_pinmux_get_config_defaults(&config);
    config.mux_position = 0x6;
    system_pinmux_group_set_config(&(PORT->Group[0]), PORT_PA30 | PORT_PA31, &config);

    #ifdef MICROPY_HW_NEOPIXEL
    neopixel_in_use = false;
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    apa102_sck_in_use = false;
    apa102_mosi_in_use = false;
    #endif

    // After configuring SWD because it may be shared.
    #ifdef SPEAKER_ENABLE_PIN
    speaker_enable_in_use = false;
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(SPEAKER_ENABLE_PIN->pin, &pin_conf);
    port_pin_set_output_level(SPEAKER_ENABLE_PIN->pin, false);
    #endif
}

void reset_pin(uint8_t pin) {
    if (pin >= PORT_BITS) {
        return;
    }

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL->pin) {
        neopixel_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI->pin ||
        pin == MICROPY_HW_APA102_SCK->pin) {
        apa102_mosi_in_use = apa102_mosi_in_use && pin != MICROPY_HW_APA102_MOSI->pin;
        apa102_sck_in_use = apa102_sck_in_use && pin != MICROPY_HW_APA102_SCK->pin;
        if (!apa102_sck_in_use && !apa102_mosi_in_use) {
            rgb_led_status_init();
        }
        return;
    }
    #endif

    struct system_pinmux_config config;
    system_pinmux_get_config_defaults(&config);
    if (pin == PIN_PA30 || pin == PIN_PA31) {
        config.mux_position = 0x6;
    } else {
        config.powersave = true;
    }
    system_pinmux_pin_set_config(pin, &config);

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN->pin) {
        speaker_enable_in_use = false;
        struct port_config pin_conf;
        port_get_config_defaults(&pin_conf);

        pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
        port_pin_set_config(SPEAKER_ENABLE_PIN->pin, &pin_conf);
        port_pin_set_output_level(SPEAKER_ENABLE_PIN->pin, false);
    }
    #endif
}

void claim_pin(const mcu_pin_obj_t* pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = true;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI) {
        apa102_mosi_in_use = true;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        apa102_sck_in_use = true;
    }
    #endif

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        speaker_enable_in_use = true;
    }
    #endif
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        return !neopixel_in_use;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI) {
        return !apa102_mosi_in_use;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        return !apa102_sck_in_use;
    }
    #endif

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        return !speaker_enable_in_use;
    }
    #endif

    PortGroup *const port = system_pinmux_get_group_from_gpio_pin(pin->pin);
    uint32_t pin_index = (pin->pin);
    volatile PORT_PINCFG_Type *state = &port->PINCFG[pin_index];
    volatile PORT_PMUX_Type *pmux = &port->PMUX[pin_index / 2];

    if (pin->pin == PIN_PA30 || pin->pin == PIN_PA31) {
        return state->bit.PMUXEN == 1 && ((pmux->reg >> (4 * pin_index % 2)) & 0xf) == 0x6;
    }

    return state->bit.PMUXEN == 0 && state->bit.INEN == 0 &&
           state->bit.PULLEN == 0 && (port->DIR.reg & (1 << pin_index)) == 0;
}
