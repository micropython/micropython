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

#if defined(CIRCUITPY_BOOT_BUTTON)
#include "shared-bindings/digitalio/DigitalInOut.h"
#endif
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "supervisor/serial.h"
#include "supervisor/shared/rgb_led_colors.h"
#include "supervisor/shared/status_leds.h"
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

    const mcu_reset_reason_t reset_reason = common_hal_mcu_processor_get_reset_reason();
    if (reset_reason != RESET_REASON_POWER_ON &&
        reset_reason != RESET_REASON_RESET_PIN &&
        reset_reason != RESET_REASON_UNKNOWN) {
        return NO_SAFE_MODE;
    }
    port_set_saved_word(SAFE_MODE_DATA_GUARD | (MANUAL_SAFE_MODE << 8));
    // Wait for a while to allow for reset.

    #if CIRCUITPY_STATUS_LED
    status_led_init();
    #endif
    #ifdef CIRCUITPY_BOOT_BUTTON
    digitalio_digitalinout_obj_t boot_button;
    common_hal_digitalio_digitalinout_construct(&boot_button, CIRCUITPY_BOOT_BUTTON);
    common_hal_digitalio_digitalinout_switch_to_input(&boot_button, PULL_UP);
    #endif
    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t diff = 0;
    bool boot_in_safe_mode = false;
    while (diff < 1000) {
        #ifdef CIRCUITPY_STATUS_LED
        // Blink on for 100, off for 100
        bool led_on = (diff % 250) < 125;
        if (led_on) {
            new_status_color(SAFE_MODE);
        } else {
            new_status_color(BLACK);
        }
        #endif
        #ifdef CIRCUITPY_BOOT_BUTTON
        if (!common_hal_digitalio_digitalinout_get_value(&boot_button)) {
            boot_in_safe_mode = true;
            break;
        }
        #endif
        diff = supervisor_ticks_ms64() - start_ticks;
    }
    #if CIRCUITPY_STATUS_LED
    new_status_color(BLACK);
    status_led_deinit();
    #endif
    if (boot_in_safe_mode) {
        return USER_SAFE_MODE;
    }
    // Restore the original state of the saved word if no reset occured during our wait period.
    port_set_saved_word(reset_state);
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



void print_safe_mode_message(safe_mode_t reason) {
    if (reason == NO_SAFE_MODE) {
        return;
    }

    serial_write("\r\n");
    serial_write_compressed(translate("You are in safe mode because:\n"));

    const compressed_string_t *message = NULL;

    // First check for safe mode reasons that do not necessarily reflect bugs.

    switch (reason) {
        case USER_SAFE_MODE:
            #ifdef BOARD_USER_SAFE_MODE_ACTION
            // Output a user safe mode string if it's set.
            serial_write_compressed(translate("You requested starting safe mode by "));
            serial_write_compressed(BOARD_USER_SAFE_MODE_ACTION);
            serial_write_compressed(translate("To exit, please reset the board without "));
            message = BOARD_USER_SAFE_MODE_ACTION;
            #endif
            break;
        case MANUAL_SAFE_MODE:
            message = translate("You pressed the reset button during boot. Press again to exit safe mode.");
            break;
        case PROGRAMMATIC_SAFE_MODE:
            message = translate("The `microcontroller` module was used to boot into safe mode. Press reset to exit safe mode.");
            break;
        case BROWNOUT:
            message = translate("The microcontroller's power dipped. Make sure your power supply provides\nenough power for the whole circuit and press reset (after ejecting CIRCUITPY).");
            break;
        case USB_TOO_MANY_ENDPOINTS:
            message = translate("USB devices need more endpoints than are available.");
            break;
        case USB_TOO_MANY_INTERFACE_NAMES:
            message = translate("USB devices specify too many interface names.");
            break;
        case WATCHDOG_RESET:
            message = translate("Watchdog timer expired.");
            break;
        default:
            break;
    }

    if (message) {
        serial_write_compressed(message);
        serial_write("\r\n");
        return;
    }

    // Something worse happened.

    serial_write_compressed(translate("CircuitPython core code crashed hard. Whoops!\n"));

    switch (reason) {
        case HARD_CRASH:
            message = translate("Crash into the HardFault_Handler.");
            break;
        case MICROPY_NLR_JUMP_FAIL:
            message = translate("NLR jump failed. Likely memory corruption.");
            break;
        case MICROPY_FATAL_ERROR:
            message = translate("Fatal error.");
            break;
        case NO_HEAP:
            message = translate("CircuitPython was unable to allocate the heap.");
            break;
        case HEAP_OVERWRITTEN:
            message = translate("The CircuitPython heap was corrupted because the stack was too small.\nIncrease the stack size if you know how. If not:");
            break;
        case GC_ALLOC_OUTSIDE_VM:
            message = translate("Attempted heap allocation when VM not running.");
            break;
            #ifdef SOFTDEVICE_PRESENT
        // defined in ports/nrf/bluetooth/bluetooth_common.mk
        // will print "Unknown reason" if somehow encountered on other ports
        case NORDIC_SOFT_DEVICE_ASSERT:
            message = translate("Nordic system firmware failure assertion.");
            break;
            #endif
        case FLASH_WRITE_FAIL:
            message = translate("Failed to write internal flash.");
            break;
        case MEM_MANAGE:
            message = translate("Invalid memory access.");
            break;
        default:
            message = translate("Unknown reason.");
            break;
    }
    serial_write_compressed(message);
    serial_write_compressed(translate("\nPlease file an issue with the contents of your CIRCUITPY drive at \nhttps://github.com/adafruit/circuitpython/issues\n"));
}
