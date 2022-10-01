/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "py/mphal.h"

#include "components/driver/include/driver/gpio.h"
#include "components/hal/include/hal/gpio_hal.h"

STATIC uint64_t _never_reset_pin_mask;
STATIC uint64_t _preserved_pin_mask;
STATIC uint64_t _in_use_pin_mask;

// Bit mask of all pins that should never EVER be reset.
// Typically these are SPI flash and PSRAM control pins, and communication pins.
// "Reset forbidden" is stronger than "never reset" below, which may only be temporary.
static const uint64_t pin_mask_reset_forbidden =
    #if defined(CONFIG_IDF_TARGET_ESP32)
    // Never ever reset serial pins for bootloader and possibly USB-serial converter.
    GPIO_SEL_1 |          // TXD0
    GPIO_SEL_3 |          // RXD0
    // SPI flash and PSRAM pins are protected at runtime in supervisor/port.c.
    #endif // ESP32

    #if defined(CONFIG_IDF_TARGET_ESP32C3)
    // Never ever reset pins used to communicate with SPI flash.
    GPIO_SEL_11 |         // VDD_SPI
    GPIO_SEL_12 |         // SPIHD
    GPIO_SEL_13 |         // SPIWP
    GPIO_SEL_14 |         // SPICS0
    GPIO_SEL_15 |         // SPICLK
    GPIO_SEL_16 |         // SPID
    GPIO_SEL_17 |         // SPIQ
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    // Never ever reset serial/JTAG communication pins.
    GPIO_SEL_18 |         // USB D-
    GPIO_SEL_19 |         // USB D+
    #endif
    #endif // ESP32C3

    #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // Never ever reset pins used to communicate with SPI flash and PSRAM.
    GPIO_SEL_19 |         // USB D-
    GPIO_SEL_20 |         // USB D+
    #if defined(CONFIG_ESP32_SPIRAM_SUPPORT) || defined(CONFIG_ESP32S2_SPIRAM_SUPPORT) || defined(CONFIG_ESP32S3_SPIRAM_SUPPORT)
    // Note ESP32-C3 does not have SPIRAM support.
    // Board uses PSRAM, and needs another chip select.
    GPIO_SEL_26 |         // SPICS1
    #endif
    GPIO_SEL_27 |         // SPIHD
    GPIO_SEL_28 |         // SPIWP
    GPIO_SEL_29 |         // SPICS0
    GPIO_SEL_30 |         // SPICLK
    GPIO_SEL_31 |         // SPIHD
    GPIO_SEL_32 |         // SPIQ
    #if defined(CONFIG_SPIRAM_MODE_OCT)
    // Never reset octal SPI flash pins DQ4-DQ7 and DQS/DM.
    GPIO_SEL_33 |         // SPIIO4
    GPIO_SEL_34 |         // SPIIO5
    GPIO_SEL_35 |         // SPIIO6
    GPIO_SEL_36 |         // SPIIO7
    GPIO_SEL_37 |         // SPIDQS
    #endif
    #if CIRCUITPY_USB
    // Never ever reset USB pins.
    GPIO_SEL_19 |         // USB D-
    GPIO_SEL_20 |         // USB D+
    #endif
    #endif // ESP32S2, ESP32S3

    0;                    // Terminate last "|".



void never_reset_pin_number(gpio_num_t pin_number) {
    // Some CircuitPython APIs deal in uint8_t pin numbers, but NO_PIN is -1.
    // Also allow pin 255 to be treated as NO_PIN to avoid crashes
    if (pin_number == NO_PIN || pin_number == (uint8_t)NO_PIN) {
        return;
    }
    _never_reset_pin_mask |= PIN_BIT(pin_number);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pin_number(pin->number);
}

MP_WEAK bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    return false;
}

STATIC bool _reset_forbidden(gpio_num_t pin_number) {
    return pin_mask_reset_forbidden & PIN_BIT(pin_number);
}

STATIC bool _never_reset(gpio_num_t pin_number) {
    return _never_reset_pin_mask & PIN_BIT(pin_number);
}

STATIC bool _preserved_pin(gpio_num_t pin_number) {
    return _preserved_pin_mask & PIN_BIT(pin_number);
}

STATIC void _reset_pin(gpio_num_t pin_number) {
    // Never ever reset pins used for flash, RAM, and basic communication.
    if (_reset_forbidden(pin_number)) {
        return;
    }

    // Disable any existing hold on this pin,
    if (GPIO_IS_VALID_OUTPUT_GPIO(pin_number)) {
        gpio_hold_dis(pin_number);
    }

    // Give the board a chance to reset the pin in a particular way.
    if (espressif_board_reset_pin_number(pin_number)) {
        return;
    }

    bool pull_down = false;

    // Special case the status LED pin.
    #if defined(MICROPY_HW_LED_STATUS) && (!defined(MICROPY_HW_LED_STATUS_INVERTED) || !MICROPY_HW_LED_STATUS_INVERTED)
    pull_down = pull_down || pin_number == MICROPY_HW_LED_STATUS->number;
    #endif

    #ifdef DOUBLE_TAP_PIN
    // Pull the double tap pin down so that resets come back to CircuitPython.
    pull_down = pull_down || pin_number == DOUBLE_TAP_PIN->number;
    #endif

    // This will pull the pin up. For pins needing pull down it shouldn't be a
    // problem for a moment.
    gpio_reset_pin(pin_number);

    if (pull_down) {
        gpio_pullup_dis(pin_number);
        gpio_pulldown_en(pin_number);
    }
}

void preserve_pin_number(gpio_num_t pin_number) {
    if (GPIO_IS_VALID_OUTPUT_GPIO(pin_number)) {
        gpio_hold_en(pin_number);
        _preserved_pin_mask |= PIN_BIT(pin_number);
    }
    if (_preserved_pin_mask) {
        // Allow pin holds to work during deep sleep. This increases power consumption noticeably
        // during deep sleep, so enable holds only if we actually are holding some pins.
        // 270uA or so extra current is consumed even with no pins held.
        gpio_deep_sleep_hold_en();
    }
}

void clear_pin_preservations(void) {
    _preserved_pin_mask = 0;
}


// Mark pin as free and return it to a quiescent state.
void reset_pin_number(gpio_num_t pin_number) {
    // Some CircuitPython APIs deal in uint8_t pin numbers, but NO_PIN is -1.
    // Also allow pin 255 to be treated as NO_PIN to avoid crashes
    if (pin_number == NO_PIN || pin_number == (uint8_t)NO_PIN) {
        return;
    }
    _never_reset_pin_mask &= ~PIN_BIT(pin_number);
    _in_use_pin_mask &= ~PIN_BIT(pin_number);

    _reset_pin(pin_number);
}

void common_hal_mcu_pin_reset_number(uint8_t i) {
    reset_pin_number((gpio_num_t)i);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void reset_all_pins(void) {
    // Undo deep sleep holds in case we woke up from deep sleep.
    // We still need to unhold individual pins, which is done by _reset_pin.
    gpio_deep_sleep_hold_dis();

    for (uint8_t i = 0; i < GPIO_PIN_COUNT; i++) {
        uint32_t iomux_address = GPIO_PIN_MUX_REG[i];
        if (iomux_address == 0 ||
            _never_reset(i) ||
            _preserved_pin(i)) {
            continue;
        }
        _reset_pin(i);
    }
    _in_use_pin_mask = _never_reset_pin_mask;
}

void claim_pin_number(gpio_num_t pin_number) {
    // Some CircuitPython APIs deal in uint8_t pin numbers, but NO_PIN is -1.
    // Also allow pin 255 to be treated as NO_PIN to avoid crashes
    if (pin_number == NO_PIN || pin_number == (uint8_t)NO_PIN) {
        return;
    }
    _in_use_pin_mask |= PIN_BIT(pin_number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    claim_pin_number(pin->number);
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}

bool pin_number_is_free(gpio_num_t pin_number) {
    return !(_in_use_pin_mask & PIN_BIT(pin_number));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin ? pin->number : NO_PIN;
}
