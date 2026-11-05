// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/alarm/__init__.h"

#include "py/mphal.h"

#include "driver/gpio.h"
#include "soc/gpio_periph.h"

static uint64_t _preserved_pin_mask;
static uint64_t _in_use_pin_mask;

#define GPIO_SEL_0              (BIT(0))                    /*!< Pin 0 selected */
#define GPIO_SEL_1              (BIT(1))                    /*!< Pin 1 selected */
#define GPIO_SEL_2              (BIT(2))                    /*!< Pin 2 selected */
#define GPIO_SEL_3              (BIT(3))                    /*!< Pin 3 selected */
#define GPIO_SEL_4              (BIT(4))                    /*!< Pin 4 selected */
#define GPIO_SEL_5              (BIT(5))                    /*!< Pin 5 selected */
#define GPIO_SEL_6              (BIT(6))                    /*!< Pin 6 selected */
#define GPIO_SEL_7              (BIT(7))                    /*!< Pin 7 selected */
#define GPIO_SEL_8              (BIT(8))                    /*!< Pin 8 selected */
#define GPIO_SEL_9              (BIT(9))                    /*!< Pin 9 selected */
#define GPIO_SEL_10             (BIT(10))                   /*!< Pin 10 selected */
#define GPIO_SEL_11             (BIT(11))                   /*!< Pin 11 selected */
#define GPIO_SEL_12             (BIT(12))                   /*!< Pin 12 selected */
#define GPIO_SEL_13             (BIT(13))                   /*!< Pin 13 selected */
#define GPIO_SEL_14             (BIT(14))                   /*!< Pin 14 selected */
#define GPIO_SEL_15             (BIT(15))                   /*!< Pin 15 selected */
#define GPIO_SEL_16             (BIT(16))                   /*!< Pin 16 selected */
#define GPIO_SEL_17             (BIT(17))                   /*!< Pin 17 selected */
#define GPIO_SEL_18             (BIT(18))                   /*!< Pin 18 selected */
#define GPIO_SEL_19             (BIT(19))                   /*!< Pin 19 selected */
#define GPIO_SEL_20             (BIT(20))                   /*!< Pin 20 selected */
#if SOC_GPIO_PIN_COUNT > 21
#define GPIO_SEL_21             (BIT(21))                   /*!< Pin 21 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 22
#define GPIO_SEL_22             (BIT(22))                   /*!< Pin 22 selected */
#define GPIO_SEL_23             (BIT(23))                   /*!< Pin 23 selected */
#define GPIO_SEL_24             (BIT(24))                   /*!< Pin 24 selected */
#define GPIO_SEL_25             (BIT(25))                   /*!< Pin 25 selected */
#define GPIO_SEL_26             (BIT(26))                   /*!< Pin 26 selected */
#define GPIO_SEL_27             (BIT(27))                   /*!< Pin 27 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 28
#define GPIO_SEL_28             (BIT(28))                   /*!< Pin 28 selected */
#define GPIO_SEL_29             (BIT(29))                   /*!< Pin 29 selected */
#define GPIO_SEL_30             (BIT(30))                   /*!< Pin 30 selected */
#define GPIO_SEL_31             (BIT(31))                   /*!< Pin 31 selected */
#define GPIO_SEL_32             ((uint64_t)PIN_BIT(32))     /*!< Pin 32 selected */
#define GPIO_SEL_33             ((uint64_t)PIN_BIT(33))     /*!< Pin 33 selected */
#define GPIO_SEL_34             ((uint64_t)PIN_BIT(34))     /*!< Pin 34 selected */
#define GPIO_SEL_35             ((uint64_t)PIN_BIT(35))     /*!< Pin 35 selected */
#define GPIO_SEL_36             ((uint64_t)PIN_BIT(36))     /*!< Pin 36 selected */
#define GPIO_SEL_37             ((uint64_t)PIN_BIT(37))     /*!< Pin 37 selected */
#define GPIO_SEL_38             ((uint64_t)PIN_BIT(38))     /*!< Pin 38 selected */
#define GPIO_SEL_39             ((uint64_t)PIN_BIT(39))     /*!< Pin 39 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 40
#define GPIO_SEL_40             ((uint64_t)PIN_BIT(40))     /*!< Pin 40 selected */
#define GPIO_SEL_41             ((uint64_t)PIN_BIT(41))     /*!< Pin 41 selected */
#define GPIO_SEL_42             ((uint64_t)PIN_BIT(42))     /*!< Pin 42 selected */
#define GPIO_SEL_43             ((uint64_t)PIN_BIT(43))     /*!< Pin 43 selected */
#define GPIO_SEL_44             ((uint64_t)PIN_BIT(44))     /*!< Pin 44 selected */
#define GPIO_SEL_45             ((uint64_t)PIN_BIT(45))     /*!< Pin 45 selected */
#define GPIO_SEL_46             ((uint64_t)PIN_BIT(46))     /*!< Pin 46 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 47
#define GPIO_SEL_47             ((uint64_t)PIN_BIT(47))     /*!< Pin 47 selected */
#define GPIO_SEL_48             ((uint64_t)PIN_BIT(48))     /*!< Pin 48 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 49
#define GPIO_SEL_49             ((uint64_t)PIN_BIT(49))     /*!< Pin 49 selected */
#define GPIO_SEL_50             ((uint64_t)PIN_BIT(50))     /*!< Pin 50 selected */
#define GPIO_SEL_51             ((uint64_t)PIN_BIT(51))     /*!< Pin 51 selected */
#define GPIO_SEL_52             ((uint64_t)PIN_BIT(52))     /*!< Pin 52 selected */
#define GPIO_SEL_53             ((uint64_t)PIN_BIT(53))     /*!< Pin 53 selected */
#define GPIO_SEL_54             ((uint64_t)PIN_BIT(54))     /*!< Pin 54 selected */
#define GPIO_SEL_55             ((uint64_t)PIN_BIT(55))     /*!< Pin 55 selected */
#endif
#if SOC_GPIO_PIN_COUNT > 56
#define GPIO_SEL_56             ((uint64_t)PIN_BIT(56))     /*!< Pin 56 selected */
#define GPIO_SEL_57             ((uint64_t)PIN_BIT(57))     /*!< Pin 57 selected */
#define GPIO_SEL_58             ((uint64_t)PIN_BIT(58))     /*!< Pin 58 selected */
#define GPIO_SEL_59             ((uint64_t)PIN_BIT(59))     /*!< Pin 59 selected */
#define GPIO_SEL_60             ((uint64_t)PIN_BIT(60))     /*!< Pin 60 selected */
#define GPIO_SEL_61             ((uint64_t)PIN_BIT(61))     /*!< Pin 61 selected */
#endif

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

    #if defined(CONFIG_IDF_TARGET_ESP32C2)
    // Never ever reset pins used to communicate with SPI flash.
    GPIO_SEL_11 |         // VDD_SPI
    GPIO_SEL_12 |         // SPIHD
    GPIO_SEL_13 |         // SPIWP
    GPIO_SEL_14 |         // SPICS0
    GPIO_SEL_15 |         // SPICLK
    GPIO_SEL_16 |         // SPID
    GPIO_SEL_17 |         // SPIQ
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_19 |
    GPIO_SEL_20 |
    #endif
    #endif // ESP32C2

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
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_20 |
    GPIO_SEL_21 |
    #endif
    #endif // ESP32C3

    #if defined(CONFIG_IDF_TARGET_ESP32C5)
    // Never ever reset pins used to communicate with SPI flash.
    // ports/espressif/esp-idf/components/esp_hal_gpspi/esp32c5/include/soc/spi_pins.h
    GPIO_SEL_16 |         // MSPI_IOMUX_PIN_NUM_CS0 (flash)
    GPIO_SEL_17 |         // MSPI_IOMUX_PIN_NUM_MISO
    GPIO_SEL_18 |         // MSPI_IOMUX_PIN_NUM_WP
    GPIO_SEL_19 |         // VDD_SPI (powers the in-module flash)
    GPIO_SEL_20 |         // MSPI_IOMUX_PIN_NUM_HD
    GPIO_SEL_21 |         // MSPI_IOMUX_PIN_NUM_CLK
    GPIO_SEL_22 |         // MSPI_IOMUX_PIN_NUM_MOSI
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    // Never ever reset serial/JTAG communication pins. On C5, gpio_ll_func_sel()
    // on either clears USB_SERIAL_JTAG.conf0.usb_pad_enable, killing the USB pads.
    GPIO_SEL_13 |         // USB D- (USB_INT_PHY0_DM_GPIO_NUM)
    GPIO_SEL_14 |         // USB D+ (USB_INT_PHY0_DP_GPIO_NUM)
    #endif
    #if defined(CONFIG_SPIRAM)
    GPIO_SEL_15 |         // MSPI_IOMUX_PIN_NUM_CS1 (PSRAM); keep if PSRAM in use
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_11 |
    GPIO_SEL_12 |
    #endif
    #endif // ESP32C5

    #if defined(CONFIG_IDF_TARGET_ESP32C6)
    // Never ever reset pins used to communicate with SPI flash.
    GPIO_SEL_24 |         // SPICS0
    GPIO_SEL_25 |         // SPIQ
    GPIO_SEL_26 |         // SPIWP
    GPIO_SEL_28 |         // SPIHD
    GPIO_SEL_29 |         // SPICLK
    GPIO_SEL_30 |         // SPID
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    // Never ever reset serial/JTAG communication pins.
    GPIO_SEL_12 |         // USB D-
    GPIO_SEL_13 |         // USB D+
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_16 |
    GPIO_SEL_17 |
    #endif
    #endif // ESP32C6

    #if defined(CONFIG_IDF_TARGET_ESP32C61)
    // Never ever reset pins used to communicate with SPI flash.
    GPIO_SEL_15 |        // SPICS0  (flash CS#)
    GPIO_SEL_16 |        // SPIQ    (MISO/SIO1)
    GPIO_SEL_17 |        // SPIWP   (WP#/SIO2)
    GPIO_SEL_19 |        // SPIHD   (HOLD#/SIO3)
    GPIO_SEL_20 |        // SPICLK  (CLK)
    GPIO_SEL_21 |        // SPID    (MOSI/SIO0)
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    // Never ever reset serial/JTAG communication pins.
    GPIO_SEL_12 |         // USB D-
    GPIO_SEL_13 |         // USB D+
    #endif
    #if defined(CONFIG_SPIRAM)
    GPIO_SEL_14 |        // SPICS1 (PSRAM CS#); keep if PSRAM in use
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_10 |
    GPIO_SEL_11 |
    #endif
    #endif // ESP32C6

    #if defined(CONFIG_IDF_TARGET_ESP32H2)
    // Never ever reset pins used to communicate with the in-package SPI flash.
    GPIO_SEL_15 |
    GPIO_SEL_16 |
    GPIO_SEL_17 |
    GPIO_SEL_18 |
    GPIO_SEL_19 |
    GPIO_SEL_20 |
    GPIO_SEL_21 |
    // It isn't clear what these are used for but they aren't broken out for
    // user use.
    GPIO_SEL_6 |
    GPIO_SEL_7 |
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    // Never ever reset serial/JTAG communication pins.
    GPIO_SEL_26 |         // USB D-
    GPIO_SEL_27 |         // USB D+
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_23 |
    GPIO_SEL_24 |
    #endif
    #endif // ESP32H2

    #if defined(CONFIG_IDF_TARGET_ESP32P4)
    // SPI flash is on dedicated pins.

    // USB is on the FS OTG, whose PHY is on 26 and 27 by default and on 24 and 25 when swapped.
    #if CIRCUITPY_USB_DEVICE_INSTANCE == 0
    #if CIRCUITPY_ESP32P4_SWAP_LSFS == 1
    GPIO_SEL_24 |         // USB D-
    GPIO_SEL_25 |         // USB D+
    #else
    GPIO_SEL_26 |         // USB D-
    GPIO_SEL_27 |         // USB D+
    #endif
    #endif
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG || (defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG) && CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    // Never ever reset serial/JTAG communication pins.
    #if CIRCUITPY_ESP32P4_SWAP_LSFS == 1
    GPIO_SEL_26 |         // USB D-
    GPIO_SEL_27 |         // USB D+
    #else
    GPIO_SEL_24 |         // USB D-
    GPIO_SEL_25 |         // USB D+
    #endif
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_37 |
    GPIO_SEL_38 |
    #endif
    #endif // ESP32P4

    #if defined(CONFIG_IDF_TARGET_ESP32S31)
    // Never ever reset pins used to communicate with SPI flash.
    // esp-idf/components/soc/esp32s31/register/soc/iomux_mspi_pin_reg.h
    GPIO_SEL_26 |         // MSPI_IOMUX_PIN_NUM_CS0
    GPIO_SEL_27 |         // MSPI_IOMUX_PIN_NUM_MISO
    GPIO_SEL_28 |         // MSPI_IOMUX_PIN_NUM_WP
    GPIO_SEL_30 |         // MSPI_IOMUX_PIN_NUM_HD
    GPIO_SEL_31 |         // MSPI_IOMUX_PIN_NUM_CLK
    GPIO_SEL_32 |         // MSPI_IOMUX_PIN_NUM_MOSI
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG || (defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG) && CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    // Never ever reset serial/JTAG communication pins.
    GPIO_SEL_33 |         // USB D-
    GPIO_SEL_34 |         // USB D+
    #endif
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Never reset debug UART/console pins.
    GPIO_SEL_58 |
    GPIO_SEL_59 |
    #endif
    #endif // ESP32S31

    #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    #if CIRCUITPY_USB_DEVICE
    // Never ever reset USB pins.
    GPIO_SEL_19 |         // USB D-
    GPIO_SEL_20 |         // USB D+
    #endif
    // Never ever reset pins used to communicate with SPI flash and PSRAM.
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
    #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) && CONFIG_ESP_CONSOLE_UART_DEFAULT && CONFIG_ESP_CONSOLE_UART_NUM == 0
    // Don't reset/use the IDF UART console.
    GPIO_SEL_43 | // UART TX
    GPIO_SEL_44 | // UART RX
    #endif
    #endif // ESP32S2, ESP32S3

    0;                    // Terminate last "|".



MP_WEAK bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    return false;
}

static bool _reset_forbidden(gpio_num_t pin_number) {
    return pin_mask_reset_forbidden & PIN_BIT(pin_number);
}

static void _reset_pin(gpio_num_t pin_number) {
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
        #if defined(SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP) && !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
        // Allow pin holds to work during deep sleep. This increases power consumption noticeably
        // during deep sleep, so enable holds only if we actually are holding some pins.
        // 270uA or so extra current is consumed even with no pins held.
        gpio_deep_sleep_hold_en();
        #endif
    }
}

void common_hal_alarm_clear_pin_preservations(void) {
    // Release any actual holds, not just the tracking mask. Without this the
    // pins would stay held after a fake deep sleep ends.
    uint64_t mask = _preserved_pin_mask;
    for (int i = 0; i < 64; i++, mask >>= 1) {
        if ((mask & 1) && GPIO_IS_VALID_OUTPUT_GPIO(i)) {
            gpio_hold_dis(i);
        }
    }
    _preserved_pin_mask = 0;
}


// Mark pin as free and return it to a quiescent state.
void reset_pin_number(gpio_num_t pin_number) {
    // Some CircuitPython APIs deal in uint8_t pin numbers, but NO_PIN is -1.
    // Also allow pin 255 to be treated as NO_PIN to avoid crashes
    if (pin_number == NO_PIN || pin_number == (uint8_t)NO_PIN) {
        return;
    }
    _in_use_pin_mask &= ~PIN_BIT(pin_number);

    _reset_pin(pin_number);
}

void reset_pin_mask(uint64_t mask) {
    for (int i = 0; i < 64; i++, mask >>= 1) {
        if (mask & 1) {
            reset_pin_number(i);
        }
    }
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

void disable_deep_sleep_pin_hold(void) {
    // preserve_pin_number() turns on the chip-wide deep sleep hold on chips that
    // can't hold single pins in deep sleep. It stays on until turned off here.
    #if defined(SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP) && !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
    gpio_deep_sleep_hold_dis();
    #endif
}

void claim_reset_forbidden_pins(void) {
    _in_use_pin_mask |= pin_mask_reset_forbidden;
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
    return GPIO_IS_VALID_GPIO(pin_number) && !(_in_use_pin_mask & PIN_BIT(pin_number));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin ? pin->number : NO_PIN;
}

void config_pin_as_output_with_level(gpio_num_t pin_number, bool level) {
    gpio_config_t cfg = {
        .pin_bit_mask = BIT64(pin_number),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);

    gpio_set_level(pin_number, level);
}
