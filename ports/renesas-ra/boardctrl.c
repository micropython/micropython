/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2020 Damien P. George
 * Copyright (c) 2022 Renesas Electronics Corporation
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
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "boardctrl.h"
#include "led.h"
#include "usrsw.h"

static void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(RA_LED1, 1);
        mp_hal_delay_ms(250);
        led_state(RA_LED1, 0);
        mp_hal_delay_ms(250);
    }
}

#if !MICROPY_HW_USES_BOOTLOADER
static uint update_reset_mode(uint reset_mode) {
    #if MICROPY_HW_HAS_SWITCH
    bool press_status;

    if (switch_get()) {
        press_status = true;
        mp_printf(&mp_plat_print, "\nEntering select boot mode\n");
        mp_printf(&mp_plat_print, "Normal: release switch after LED1 flashes 4 times or more.\n");
        mp_printf(&mp_plat_print, "Safe: release switch after LED1 flashes 2 times.\n");
        mp_printf(&mp_plat_print, "Factory filesystem: release switch after LED1 flashes 3 times.\n");
        // For boards with only a single LED, we'll flash that LED the
        // appropriate number of times, with a pause between each one
        for (uint i = 0; i < 100; i++) {
            led_state(RA_LED1, 1);
            mp_hal_delay_ms(1000);
            led_state(RA_LED1, 0);
            for (uint i = 0; i < 50; i++) {
                mp_hal_delay_ms(20);
                if (!switch_get()) {
                    press_status = false;
                    break;
                }
            }
            if (press_status == false) {
                break;
            }
            ++reset_mode;
        }
        if (reset_mode > BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
            reset_mode = BOARDCTRL_RESET_MODE_NORMAL;
        }
        #if 0   // if selected mode is displayed, LED1 flash is unnecessary.
        // Flash the selected reset mode twice.
        //   We'll flash LED selected number of times, and wait for 2 secs, then flash again:
        //     BOARDCTRL_RESET_MODE_NORMAL = 1,
        //     BOARDCTRL_RESET_MODE_SAFE_MODE = 2,
        //     BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM = 3,
        mp_hal_delay_ms(1000);
        for (uint i = 0; i < 2; i++) {
            for (uint j = 0; j < reset_mode; j++) {
                led_state(RA_LED1, 1);
                mp_hal_delay_ms(1000);
                led_state(RA_LED1, 0);
                mp_hal_delay_ms(1000);
            }
            mp_hal_delay_ms(2000);
        }
        #endif
        if (reset_mode == BOARDCTRL_RESET_MODE_SAFE_MODE) {
            mp_printf(&mp_plat_print, "\nBoot with safe mode\n");
        } else if (reset_mode == BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM) {
            mp_printf(&mp_plat_print, "\nBoot with factory filesystem mode\n");
        } else {
            mp_printf(&mp_plat_print, "\nBoot with Normal mode\n");
        }
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
