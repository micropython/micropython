/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2022 Damien P. George
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

#include "mboot.h"
#include "ports/stm32/boardctrl.h"

#if MBOOT_ENABLE_DEFAULT_UI

/******************************************************************************/
// LED

#if defined(MBOOT_LED1)
#define LED0 MBOOT_LED1
#elif defined(MICROPY_HW_LED1)
#define LED0 MICROPY_HW_LED1
#endif

#if defined(MBOOT_LED2)
#define LED1 MBOOT_LED2
#elif defined(MICROPY_HW_LED2)
#define LED1 MICROPY_HW_LED2
#endif

#if defined(MBOOT_LED3)
#define LED2 MBOOT_LED3
#elif defined(MICROPY_HW_LED3)
#define LED2 MICROPY_HW_LED3
#endif

#if defined(MBOOT_LED4)
#define LED3 MBOOT_LED4
#elif defined(MICROPY_HW_LED4)
#define LED3 MICROPY_HW_LED4
#endif

// For flashing states: bit 0 is "active", bit 1 is "inactive", bits 2-6 are flash rate.
typedef enum {
    LED0_STATE_OFF = 0,
    LED0_STATE_ON = 1,
    LED0_STATE_SLOW_FLASH = (20 << 2) | 1,
    LED0_STATE_FAST_FLASH = (2 << 2) | 1,
    LED0_STATE_SLOW_INVERTED_FLASH = (20 << 2) | 2,
} led0_state_t;

static led0_state_t led0_cur_state = LED0_STATE_OFF;
static uint32_t led0_ms_interval = 0;
static int led0_toggle_count = 0;

void led_init(void) {
    #if defined(MBOOT_BOARD_LED_INIT)
    // Custom LED init function provided by the board.
    MBOOT_BOARD_LED_INIT();
    #else
    // Init LEDs using GPIO calls.
    mp_hal_pin_output(LED0);
    #ifdef LED1
    mp_hal_pin_output(LED1);
    #endif
    #ifdef LED2
    mp_hal_pin_output(LED2);
    #endif
    #ifdef LED3
    mp_hal_pin_output(LED3);
    #endif
    #endif

    led0_cur_state = LED0_STATE_OFF;
}

static void led_state(uint32_t led, int val) {
    #if defined(MBOOT_BOARD_LED_STATE)
    // Custom LED state function provided by the board.
    return MBOOT_BOARD_LED_STATE(led, val);
    #else
    // Set LEDs using GPIO calls.
    if (val) {
        MICROPY_HW_LED_ON(led);
    } else {
        MICROPY_HW_LED_OFF(led);
    }
    #endif
}

void led_state_all(unsigned int mask) {
    led_state(LED0, mask & MBOOT_LED_STATE_LED0);
    #ifdef LED1
    led_state(LED1, mask & MBOOT_LED_STATE_LED1);
    #endif
    #ifdef LED2
    led_state(LED2, mask & MBOOT_LED_STATE_LED2);
    #endif
    #ifdef LED3
    led_state(LED3, mask & MBOOT_LED_STATE_LED3);
    #endif
}

static void led0_state(led0_state_t state) {
    led0_cur_state = state;
    if (state == LED0_STATE_OFF || state == LED0_STATE_ON) {
        led_state(LED0, state);
    }
}

void mboot_ui_systick(void) {
    if (led0_cur_state != LED0_STATE_OFF && systick_ms - led0_ms_interval > 50) {
        uint8_t rate = (led0_cur_state >> 2) & 0x1f;
        led0_ms_interval += 50;
        if (++led0_toggle_count >= rate) {
            led0_toggle_count = 0;
        }
        led_state(LED0, (led0_cur_state & (led0_toggle_count == 0 ? 1 : 2)));
    }
}

/******************************************************************************/
// User button

#ifdef MICROPY_HW_USRSW_PIN

#define RESET_MODE_NUM_STATES (4)
#define RESET_MODE_TIMEOUT_CYCLES (8)
#ifdef LED2
#ifdef LED3
#define RESET_MODE_LED_STATES 0x8421
#else
#define RESET_MODE_LED_STATES 0x7421
#endif
#else
#define RESET_MODE_LED_STATES 0x3210
#endif

static void usrbtn_init(void) {
    mp_hal_pin_config(MICROPY_HW_USRSW_PIN, MP_HAL_PIN_MODE_INPUT, MICROPY_HW_USRSW_PULL, 0);
}

static int usrbtn_state(void) {
    return mp_hal_pin_read(MICROPY_HW_USRSW_PIN) == MICROPY_HW_USRSW_PRESSED;
}

int mboot_get_reset_mode_default(void) {
    usrbtn_init();
    int reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
    if (usrbtn_state()) {
        // Cycle through reset modes while USR is held
        // Timeout is roughly 20s, where reset_mode=1
        systick_init();
        led_init();
        reset_mode = 0;
        for (int i = 0; i < (RESET_MODE_NUM_STATES * RESET_MODE_TIMEOUT_CYCLES + 1) * 32; i++) {
            if (i % 32 == 0) {
                if (++reset_mode > RESET_MODE_NUM_STATES) {
                    reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
                }
                uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
                led_state_all(l);
            }
            if (!usrbtn_state()) {
                break;
            }
            mp_hal_delay_ms(19);
        }
        // Flash the selected reset mode
        for (int i = 0; i < 6; i++) {
            led_state_all(0);
            mp_hal_delay_ms(50);
            uint8_t l = RESET_MODE_LED_STATES >> ((reset_mode - 1) * 4);
            led_state_all(l);
            mp_hal_delay_ms(50);
        }
        mp_hal_delay_ms(300);
    }
    return reset_mode;
}

#endif

/******************************************************************************/
// State change

void mboot_state_change_default(mboot_state_t state, uint32_t arg) {
    switch (state) {
        case MBOOT_STATE_DFU_START:
            led_state_all(0);
            led0_state(LED0_STATE_SLOW_FLASH);
            break;

        case MBOOT_STATE_DFU_END:
            led_state_all(0);
            break;

        case MBOOT_STATE_FSLOAD_START:
            break;

        case MBOOT_STATE_FSLOAD_END:
            // Flash LEDs based on success/failure of update
            for (int i = 0; i < 4; ++i) {
                if (arg == 0) {
                    led_state_all(MBOOT_LED_STATE_LED0 | MBOOT_LED_STATE_LED1 | MBOOT_LED_STATE_LED2);
                } else {
                    led_state_all(MBOOT_LED_STATE_LED0);
                }
                mp_hal_delay_ms(100);
                led_state_all(0);
                mp_hal_delay_ms(100);
            }
            break;

        case MBOOT_STATE_FSLOAD_PASS_START:
            led_state_all(arg == 0 ? 2 : 4);
            break;

        case MBOOT_STATE_FSLOAD_PROGRESS:
            break;

        case MBOOT_STATE_ERASE_START:
            led0_state(LED0_STATE_ON);
            break;

        case MBOOT_STATE_READ_START:
        case MBOOT_STATE_WRITE_START:
            led0_state(LED0_STATE_FAST_FLASH);
            break;

        case MBOOT_STATE_ERASE_END:
        case MBOOT_STATE_READ_END:
        case MBOOT_STATE_WRITE_END:
            if (arg == 0) {
                led0_state(LED0_STATE_SLOW_FLASH);
            } else {
                led0_state(LED0_STATE_SLOW_INVERTED_FLASH);
            }
            break;
    }
}

#endif // MBOOT_ENABLE_DEFAULT_UI
