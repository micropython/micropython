/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/safe_mode.h"

#include "mphalport.h"

#include "shared-bindings/digitalio/DigitalInOut.h"

#include "supervisor/serial.h"
#include "supervisor/shared/rgb_led_colors.h"
#include "supervisor/shared/rgb_led_status.h"
#include "supervisor/shared/translate.h"

#define SAFE_MODE_DATA_GUARD 0xad0000af
#define SAFE_MODE_DATA_GUARD_MASK 0xff0000ff

static safe_mode_t current_safe_mode;

safe_mode_t wait_for_safe_mode_reset(void) {
    uint32_t reset_state = port_get_saved_word();
    safe_mode_t safe_mode = NO_SAFE_MODE;
    if ((reset_state & SAFE_MODE_DATA_GUARD_MASK) == SAFE_MODE_DATA_GUARD) {
        safe_mode = (reset_state & ~SAFE_MODE_DATA_GUARD_MASK) >> 8;
    }
    if (safe_mode != NO_SAFE_MODE) {
        port_set_saved_word(SAFE_MODE_DATA_GUARD);
        current_safe_mode = safe_mode;
        return safe_mode;
    }
    port_set_saved_word(SAFE_MODE_DATA_GUARD | (MANUAL_SAFE_MODE << 8));
    // Wait for a while to allow for reset.
    temp_status_color(SAFE_MODE);
    #ifdef MICROPY_HW_LED_STATUS
    digitalio_digitalinout_obj_t status_led;
    common_hal_digitalio_digitalinout_construct(&status_led, MICROPY_HW_LED_STATUS);
    common_hal_digitalio_digitalinout_switch_to_output(&status_led, true, DRIVE_MODE_PUSH_PULL);
    #endif
    uint64_t start_ticks = ticks_ms;
    uint64_t diff = 0;
    while (diff < 700) {
        #ifdef MICROPY_HW_LED_STATUS
        // Blink on for 100, off for 100, on for 100, off for 100 and on for 200
        common_hal_digitalio_digitalinout_set_value(&status_led, diff > 100 && diff / 100 != 2 && diff / 100 != 4);
        #endif
        diff = ticks_ms - start_ticks;
    }
    #ifdef MICROPY_HW_LED_STATUS
    common_hal_digitalio_digitalinout_deinit(&status_led);
    #endif
    clear_temp_status();
    port_set_saved_word(SAFE_MODE_DATA_GUARD);
    return NO_SAFE_MODE;
}

// Don't inline this so it's easy to break on it from GDB.
void __attribute__((noinline,)) reset_into_safe_mode(safe_mode_t reason) {
    if (current_safe_mode > BROWNOUT && reason > BROWNOUT) {
        while (true) {
            // This very bad because it means running in safe mode didn't save us. Only ignore brownout
            // because it may be due to a switch bouncing.
        }
    }

    port_set_saved_word(SAFE_MODE_DATA_GUARD | (reason << 8));
    reset_cpu();
}

void print_safe_mode_message(safe_mode_t reason) {
    // Output a user safe mode string if its set.
    #ifdef BOARD_USER_SAFE_MODE
    if (reason == USER_SAFE_MODE) {
        serial_write("\r\n");
        serial_write_compressed(translate("You requested starting safe mode by "));
        serial_write(BOARD_USER_SAFE_MODE_ACTION);
        serial_write("\r\n");
        serial_write_compressed(translate("To exit, please reset the board without "));
        serial_write(BOARD_USER_SAFE_MODE_ACTION);
        serial_write("\r\n");
    } else
    #endif
    if (reason != NO_SAFE_MODE) {
        serial_write("\r\n");
        serial_write_compressed(translate("You are running in safe mode which means something unanticipated happened.\n"));
        if (reason == HARD_CRASH || reason == MICROPY_NLR_JUMP_FAIL || reason == MICROPY_FATAL_ERROR || reason == GC_ALLOC_OUTSIDE_VM) {
            serial_write_compressed(translate("Looks like our core CircuitPython code crashed hard. Whoops!\nPlease file an issue at https://github.com/adafruit/circuitpython/issues\n with the contents of your CIRCUITPY drive and this message:\n"));
            if (reason == HARD_CRASH) {
                serial_write_compressed(translate("Crash into the HardFault_Handler.\n"));
            } else if (reason == MICROPY_NLR_JUMP_FAIL) {
                serial_write_compressed(translate("MicroPython NLR jump failed. Likely memory corruption.\n"));
            } else if (reason == MICROPY_FATAL_ERROR) {
                serial_write_compressed(translate("MicroPython fatal error.\n"));
            } else if (reason == GC_ALLOC_OUTSIDE_VM) {
                serial_write_compressed(translate("Attempted heap allocation when MicroPython VM not running.\n"));
            }
        } else if (reason == BROWNOUT) {
            serial_write_compressed(translate("The microcontroller's power dipped. Please make sure your power supply provides\nenough power for the whole circuit and press reset (after ejecting CIRCUITPY).\n"));
        } else if (reason == HEAP_OVERWRITTEN) {
            serial_write_compressed(translate("The CircuitPython heap was corrupted because the stack was too small.\nPlease increase stack size limits and press reset (after ejecting CIRCUITPY).\nIf you didn't change the stack, then file an issue here with the contents of your CIRCUITPY drive:\n"));
            serial_write("https://github.com/adafruit/circuitpython/issues\r\n");
        } else if (reason == MANUAL_SAFE_MODE) {
            serial_write_compressed(translate("The reset button was pressed while booting CircuitPython. Press again to exit safe mode.\n"));
        }
    }
}
