/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2022 Damien P. George
 * Copyright (c) 2022 David Lechner <david@pybricks.com>
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

#include "py/mphal.h"
#include "irq.h"

void board_init(void) {
    if (query_irq() == IRQ_STATE_DISABLED) {
        enable_irq(IRQ_STATE_ENABLED);
    }

    // Enable 3V3 for all ports
    mp_hal_pin_output(pyb_pin_PORT_3V3_EN);
    mp_hal_pin_high(pyb_pin_PORT_3V3_EN);

    // Port A
    // Enable RX/TX buffer
    mp_hal_pin_output(pyb_pin_PORTA_EN);
    mp_hal_pin_low(pyb_pin_PORTA_EN);

    // Port B
    // Enable RX/TX buffer
    mp_hal_pin_output(pyb_pin_PORTB_EN);
    mp_hal_pin_low(pyb_pin_PORTB_EN);
}

#if BUILDING_MBOOT

#include "drivers/memory/spiflash.h"
#include "mboot/mboot.h"
#include "boardctrl.h"
#include "adc.h"
#include "hub_display.h"

#define RESET_MODE_NUM_STATES (4)
#define RESET_MODE_TIMEOUT_CYCLES (8)

// Location and value for the SPI flash update key.  If this key exists at the defined
// location then mboot will attempt to do a filesystem-load update of the main firmware.
// This makes the update robust to power failures: if the update does not complete then
// it will be restarted the next time it powers up.  Only when it fully completes will
// this key be erased, and then the application can run.
#define SPIFLASH_UPDATE_KEY_ADDR (1020 * 1024)
#define SPIFLASH_UPDATE_KEY_VALUE (0x12345678)

static void board_led_pattern(int reset_mode, uint16_t brightness) {
    switch (reset_mode) {
        case BOARDCTRL_RESET_MODE_NORMAL:
            // set status light to red
            hub_display_set(3, brightness);
            hub_display_set(4, 0);
            hub_display_set(5, 0);
            break;
        case BOARDCTRL_RESET_MODE_SAFE_MODE:
            // set status light to green
            hub_display_set(3, 0);
            hub_display_set(4, brightness);
            hub_display_set(5, 0);
            break;
        case BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM:
            // set status light to blue
            hub_display_set(3, 0);
            hub_display_set(4, 0);
            hub_display_set(5, brightness);
            break;
        case BOARDCTRL_RESET_MODE_BOOTLOADER:
            // set status light to white
            hub_display_set(3, brightness);
            hub_display_set(4, brightness);
            hub_display_set(5, brightness);
            break;
    }

    hub_display_update();
}

static void board_battery_init(void) {
    mp_hal_pin_config(pyb_pin_BAT_VMON_ADC, MP_HAL_PIN_MODE_ADC, MP_HAL_PIN_PULL_NONE, 0);
    adc_config(ADC1, 12);
}

// returns true if the battery is pressed, otherwise false
static int board_battery_state(void) {
    uint16_t value = adc_config_and_read_u16(ADC1, 6, ADC_SAMPLETIME_15CYCLES);
    // If battery voltage is above USB voltage, then we consider the battery
    // to be present.
    return value > 41100; // 41100 is approx 5.5V
}

static void board_button_init(void) {
    mp_hal_pin_input(pyb_pin_BUTTON);
}

// returns true if the button is pressed, otherwise false
static int board_button_state(void) {
    // button is active low
    return !mp_hal_pin_read(pyb_pin_BUTTON);
}

void board_mboot_cleanup(int reset_mode) {
    board_led_pattern(0, 0);
    hub_display_off();
}

void board_mboot_led_init(void) {
    hub_display_on();
}

void board_mboot_led_state(int led, int state) {
    if (state) {
        hub_display_set(3 + led, 0x7fff);
    } else {
        hub_display_set(3 + led, 0);
    }

    hub_display_update();
}

int board_mboot_get_reset_mode(uint32_t *initial_r0) {
    board_battery_init();
    board_button_init();
    int reset_mode = BOARDCTRL_RESET_MODE_NORMAL;

    if (board_battery_state()) {
        // Battery is present, check flash for update key and start an update if the key exists.
        // Otherwise, boot normally.

        // Initialise the external SPI flash.
        MBOOT_SPIFLASH_SPIFLASH->config = MBOOT_SPIFLASH_CONFIG;
        mp_spiflash_init(MBOOT_SPIFLASH_SPIFLASH);

        // Read in the key.
        uint32_t buf;
        mp_spiflash_read(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR, 4, (uint8_t *)&buf);

        if (buf == SPIFLASH_UPDATE_KEY_VALUE) {
            // The key has the correct value, so read in the FS-load elements and enter the bootloader.
            mp_spiflash_read(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR + 4, ELEM_DATA_SIZE, ELEM_DATA_START);
            *initial_r0 = MBOOT_INITIAL_R0_KEY_FSLOAD;
            reset_mode = BOARDCTRL_RESET_MODE_BOOTLOADER;
        }
    } else {
        // Battery is not present. Cycle through reset modes until button is pressed.
        systick_init();
        hub_display_on();
        reset_mode = 0;
        for (int i = 0; i < (RESET_MODE_NUM_STATES * RESET_MODE_TIMEOUT_CYCLES + 1) * 64; i++) {
            if (i % 64 == 0) {
                if (++reset_mode > RESET_MODE_NUM_STATES) {
                    reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
                }
                board_led_pattern(reset_mode, 0x7fff);
            }

            if (board_button_state()) {
                break;
            }

            mp_hal_delay_ms(19);
        }

        // Flash the selected reset mode.
        for (int i = 0; i < 6; i++) {
            board_led_pattern(reset_mode, 0x0fff);
            mp_hal_delay_ms(50);
            board_led_pattern(reset_mode, 0x7fff);
            mp_hal_delay_ms(50);
        }

        mp_hal_delay_ms(300);
    }

    board_led_pattern(0, 0);
    return reset_mode;
}

void board_mboot_state_change(int state, uint32_t arg) {
    if (state == MBOOT_STATE_FSLOAD_START) {
        // The FS-load update is about to start.  Program the update key and FS-load elements
        // into the flash so they can be retrieved if there is a power failure during the update.
        mp_spiflash_erase_block(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR);
        mp_spiflash_write(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR + 4, ELEM_DATA_SIZE, ELEM_DATA_START);
        uint32_t key = SPIFLASH_UPDATE_KEY_VALUE;
        mp_spiflash_write(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR, 4, (const uint8_t *)&key);
    } else if (state == MBOOT_STATE_FSLOAD_END) {
        // The FS-load update completed (either with success or failure), so erase the
        // update key and write the result of the FS-load operation into flash.
        mp_spiflash_erase_block(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR);
        mp_spiflash_write(MBOOT_SPIFLASH_SPIFLASH, SPIFLASH_UPDATE_KEY_ADDR + 4, 4, (const uint8_t *)&arg);
    }

    mboot_state_change_default(state, arg);
}

#endif
