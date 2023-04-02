/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2020 Damien P. George
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

#include <string.h>

#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "boardctrl.h"
#include "powerctrl.h"
#include "led.h"
#include "usrsw.h"

NORETURN void boardctrl_fatal_error(const char *msg) {
    for (volatile uint delay = 0; delay < 10000000; delay++) {
    }
    led_state(1, 1);
    led_state(2, 1);
    led_state(3, 1);
    led_state(4, 1);
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
    for (uint i = 0;;) {
        led_toggle(((i++) & 3) + 1);
        for (volatile uint delay = 0; delay < 10000000; delay++) {
        }
        if (i >= 16) {
            // to conserve power
            __WFI();
        }
    }
}

STATIC void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_RED, 1);
        led_state(PYB_LED_GREEN, 0);
        mp_hal_delay_ms(250);
        led_state(PYB_LED_RED, 0);
        led_state(PYB_LED_GREEN, 1);
        mp_hal_delay_ms(250);
    }
    led_state(PYB_LED_GREEN, 0);
}

#if MICROPY_HW_USES_BOOTLOADER
void boardctrl_maybe_enter_mboot(size_t n_args, const void *args_in) {
    const mp_obj_t *args = args_in;

    if (n_args == 0 || !mp_obj_is_true(args[0])) {
        // By default, with no args given, we enter the custom bootloader (mboot)
        powerctrl_enter_bootloader(0x70ad0000, MBOOT_VTOR);
    }

    if (n_args == 1 && mp_obj_is_str_or_bytes(args[0])) {
        // With a string/bytes given, pass its data to the custom bootloader
        size_t len;
        const char *data = mp_obj_str_get_data(args[0], &len);
        void *mboot_region = (void *)*((volatile uint32_t *)MBOOT_VTOR);
        memmove(mboot_region, data, len);
        powerctrl_enter_bootloader(0x70ad0080, MBOOT_VTOR);
    }
}
#endif

#if !MICROPY_HW_USES_BOOTLOADER
STATIC uint update_reset_mode(uint reset_mode) {
    // Note: Must use HAL_Delay here as MicroPython is not yet initialised
    // and mp_hal_delay_ms will attempt to invoke the scheduler.

    #if MICROPY_HW_HAS_SWITCH
    if (switch_get()) {

        // The original method used on the pyboard is appropriate if you have 2
        // or more LEDs.
        #if defined(MICROPY_HW_LED2)
        for (uint i = 0; i < 100; i++) {
            led_state(2, reset_mode & 1);
            led_state(3, reset_mode & 2);
            led_state(4, reset_mode & 4);
            for (uint j = 0; j < 30; ++j) {
                HAL_Delay(20);
                if (!switch_get()) {
                    goto select_mode;
                }
            }
            if (++reset_mode > BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
                reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
            }
        }
    select_mode:
        // flash the selected reset mode
        for (uint i = 0; i < 6; i++) {
            led_state(2, 0);
            led_state(3, 0);
            led_state(4, 0);
            HAL_Delay(50);
            led_state(2, reset_mode & 1);
            led_state(3, reset_mode & 2);
            led_state(4, reset_mode & 4);
            HAL_Delay(50);
        }
        HAL_Delay(400);

        #elif defined(MICROPY_HW_LED1)

        // For boards with only a single LED, we'll flash that LED the
        // appropriate number of times, with a pause between each one
        for (uint i = 0; i < 10; i++) {
            led_state(1, 0);
            for (uint j = 0; j < reset_mode; j++) {
                if (!switch_get()) {
                    break;
                }
                led_state(1, 1);
                HAL_Delay(100);
                led_state(1, 0);
                HAL_Delay(200);
            }
            HAL_Delay(400);
            if (!switch_get()) {
                break;
            }
            if (++reset_mode > BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
                reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
            }
        }
        // Flash the selected reset mode
        for (uint i = 0; i < 2; i++) {
            for (uint j = 0; j < reset_mode; j++) {
                led_state(1, 1);
                HAL_Delay(100);
                led_state(1, 0);
                HAL_Delay(200);
            }
            HAL_Delay(400);
        }
        #else
        #error Need a reset mode update method
        #endif
    }
    #endif
    return reset_mode;
}
#endif

void boardctrl_before_soft_reset_loop(boardctrl_state_t *state) {
    #if !MICROPY_HW_USES_BOOTLOADER
    // Update the reset_mode via the default
    // method which uses the board switch/button and LEDs.
    state->reset_mode = update_reset_mode(BOARDCTRL_RESET_MODE_NORMAL);
    #endif
}

void boardctrl_top_soft_reset_loop(boardctrl_state_t *state) {
    // Turn on a single LED to indicate start up.
    #if defined(MICROPY_HW_LED2)
    led_state(1, 0);
    led_state(2, 1);
    #else
    led_state(1, 1);
    led_state(2, 0);
    #endif
    led_state(3, 0);
    led_state(4, 0);
}

int boardctrl_run_boot_py(boardctrl_state_t *state) {
    bool run_boot_py = state->reset_mode != BOARDCTRL_RESET_MODE_SAFE_MODE;

    if (run_boot_py) {
        // Run boot.py, if it exists.
        const char *boot_py = "boot.py";
        int ret = pyexec_file_if_exists(boot_py);

        // Take action based on the execution result.
        if (ret & PYEXEC_FORCED_EXIT) {
            return BOARDCTRL_GOTO_SOFT_RESET_EXIT;
        }
        if (!ret) {
            // There was an error, prevent main.py from running and flash LEDs.
            state->reset_mode = BOARDCTRL_RESET_MODE_SAFE_MODE;
            flash_error(4);
        }
    }

    // Turn boot-up LEDs off

    #if !defined(MICROPY_HW_LED2)
    // If there is only one LED on the board then it's used to signal boot-up
    // and so we turn it off here.  Otherwise LED(1) is used to indicate dirty
    // flash cache and so we shouldn't change its state.
    led_state(1, 0);
    #endif
    led_state(2, 0);
    led_state(3, 0);
    led_state(4, 0);

    return BOARDCTRL_CONTINUE;
}

int boardctrl_run_main_py(boardctrl_state_t *state) {
    bool run_main_py = state->reset_mode != BOARDCTRL_RESET_MODE_SAFE_MODE
        && pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL;

    if (run_main_py) {
        // Run main.py (or what it was configured to be), if it exists.
        const char *main_py;
        if (MP_STATE_PORT(pyb_config_main) == MP_OBJ_NULL) {
            main_py = "main.py";
        } else {
            main_py = mp_obj_str_get_str(MP_STATE_PORT(pyb_config_main));
        }
        int ret = pyexec_file_if_exists(main_py);

        // Take action based on the execution result.
        if (ret & PYEXEC_FORCED_EXIT) {
            return BOARDCTRL_GOTO_SOFT_RESET_EXIT;
        }
        if (!ret) {
            flash_error(3);
        }
    }

    return BOARDCTRL_CONTINUE;
}

void boardctrl_start_soft_reset(boardctrl_state_t *state) {
    state->log_soft_reset = true;
}

void boardctrl_end_soft_reset(boardctrl_state_t *state) {
    // Set reset_mode to normal boot.
    state->reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
}
