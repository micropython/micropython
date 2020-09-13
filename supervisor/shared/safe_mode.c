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
#include "supervisor/shared/tick.h"

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
    #ifdef CIRCUITPY_BOOT_BUTTON
    digitalio_digitalinout_obj_t boot_button;
    common_hal_digitalio_digitalinout_construct(&boot_button, CIRCUITPY_BOOT_BUTTON);
    common_hal_digitalio_digitalinout_switch_to_input(&boot_button, PULL_UP);
    #endif
    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t diff = 0;
    while (diff < 700) {
        #ifdef MICROPY_HW_LED_STATUS
        // Blink on for 100, off for 100, on for 100, off for 100 and on for 200
        common_hal_digitalio_digitalinout_set_value(&status_led, diff > 100 && diff / 100 != 2 && diff / 100 != 4);
        #endif
	    #ifdef CIRCUITPY_BOOT_BUTTON
        if (!common_hal_digitalio_digitalinout_get_value(&boot_button)) {
           return USER_SAFE_MODE;
        }
        #endif
        diff = supervisor_ticks_ms64() - start_ticks;
    }
    #ifdef MICROPY_HW_LED_STATUS
    common_hal_digitalio_digitalinout_deinit(&status_led);
    #endif
    clear_temp_status();
    port_set_saved_word(SAFE_MODE_DATA_GUARD);
    return NO_SAFE_MODE;
}

void safe_mode_on_next_reset(safe_mode_t reason) {
    port_set_saved_word(SAFE_MODE_DATA_GUARD | (reason << 8));
}

// Don't inline this so it's easy to break on it from GDB.
void __attribute__((noinline,)) reset_into_safe_mode(safe_mode_t reason) {
    if (current_safe_mode > BROWNOUT && reason > BROWNOUT) {
        while (true) {
            // This very bad because it means running in safe mode didn't save us. Only ignore brownout
            // because it may be due to a switch bouncing.
        }
    }

    safe_mode_on_next_reset(reason);
    reset_cpu();
}



#define FILE_AN_ISSUE translate("\nPlease file an issue with the contents of your CIRCUITPY drive at \nhttps://github.com/adafruit/circuitpython/issues\n")

void print_safe_mode_message(safe_mode_t reason) {
    if (reason == NO_SAFE_MODE) {
        return;
    }
    serial_write("\n");

    switch (reason) {
        case USER_SAFE_MODE:
            #ifdef BOARD_USER_SAFE_MODE_ACTION
                // Output a user safe mode string if it's set.
                serial_write_compressed(translate("You requested starting safe mode by "));
                serial_write_compressed(translate(BOARD_USER_SAFE_MODE_ACTION));
                serial_write_compressed(translate("To exit, please reset the board without "));
                serial_write_compressed(translate(BOARD_USER_SAFE_MODE_ACTION));
            #else
                break;
            #endif
            return;
        case MANUAL_SAFE_MODE:
            serial_write_compressed(translate("CircuitPython is in safe mode because you pressed the reset button during boot. Press again to exit safe mode.\n"));
            return;
        case PROGRAMMATIC_SAFE_MODE:
            serial_write_compressed(translate("The `microcontroller` module was used to boot into safe mode. Press reset to exit safe mode.\n"));
            return;
        default:
            break;
    }

    serial_write_compressed(translate("You are in safe mode: something unanticipated happened.\n"));
    switch (reason) {
        case BROWNOUT:
            serial_write_compressed(translate("The microcontroller's power dipped. Make sure your power supply provides\nenough power for the whole circuit and press reset (after ejecting CIRCUITPY).\n"));
            return;
        case HEAP_OVERWRITTEN:
            serial_write_compressed(translate("The CircuitPython heap was corrupted because the stack was too small.\nPlease increase the stack size if you know how, or if not:"));
            serial_write_compressed(FILE_AN_ISSUE);
            return;
        case NO_HEAP:
            serial_write_compressed(translate("CircuitPython was unable to allocate the heap.\n"));
            serial_write_compressed(FILE_AN_ISSUE);
            return;
        default:
            break;
    }

    serial_write_compressed(translate("CircuitPython core code crashed hard. Whoops!\n"));
    switch (reason) {
        case HARD_CRASH:
            serial_write_compressed(translate("Crash into the HardFault_Handler."));
            return;
        case MICROPY_NLR_JUMP_FAIL:
            serial_write_compressed(translate("MicroPython NLR jump failed. Likely memory corruption."));
            return;
        case MICROPY_FATAL_ERROR:
            serial_write_compressed(translate("MicroPython fatal error."));
            break;
        case GC_ALLOC_OUTSIDE_VM:
            serial_write_compressed(translate("Attempted heap allocation when MicroPython VM not running."));
            break;
        #ifdef SOFTDEVICE_PRESENT
        // defined in ports/nrf/bluetooth/bluetooth_common.mk
        // will print "Unknown reason" if somehow encountered on other ports
        case NORDIC_SOFT_DEVICE_ASSERT:
            serial_write_compressed(translate("Nordic Soft Device failure assertion."));
            break;
        #endif
        case FLASH_WRITE_FAIL:
            serial_write_compressed(translate("Failed to write internal flash."));
            break;
        case MEM_MANAGE:
            serial_write_compressed(translate("Invalid memory access."));
            break;
        case WATCHDOG_RESET:
            serial_write_compressed(translate("Watchdog timer expired."));
            break;
        default:
            serial_write_compressed(translate("Unknown reason."));
            break;
    }
    serial_write_compressed(FILE_AN_ISSUE);
}