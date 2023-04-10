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

#include "supervisor/linker.h"
#include "supervisor/serial.h"
#include "supervisor/shared/rgb_led_colors.h"
#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/tick.h"

#define SAFE_MODE_DATA_GUARD 0xad0000af
#define SAFE_MODE_DATA_GUARD_MASK 0xff0000ff

static safe_mode_t _safe_mode;

safe_mode_t get_safe_mode(void) {
    return _safe_mode;
}

void set_safe_mode(safe_mode_t safe_mode) {
    _safe_mode = safe_mode;
}

safe_mode_t wait_for_safe_mode_reset(void) {
    uint32_t reset_state = port_get_saved_word();
    safe_mode_t safe_mode = SAFE_MODE_NONE;
    if ((reset_state & SAFE_MODE_DATA_GUARD_MASK) == SAFE_MODE_DATA_GUARD) {
        safe_mode = (reset_state & ~SAFE_MODE_DATA_GUARD_MASK) >> 8;
    }
    if (safe_mode != SAFE_MODE_NONE) {
        port_set_saved_word(SAFE_MODE_DATA_GUARD);
        _safe_mode = safe_mode;
        return safe_mode;
    } else {
        _safe_mode = SAFE_MODE_NONE;
    }

    const mcu_reset_reason_t reset_reason = common_hal_mcu_processor_get_reset_reason();
    if (reset_reason != RESET_REASON_POWER_ON &&
        reset_reason != RESET_REASON_RESET_PIN &&
        reset_reason != RESET_REASON_UNKNOWN &&
        reset_reason != RESET_REASON_SOFTWARE) {
        return SAFE_MODE_NONE;
    }
    #if CIRCUITPY_SKIP_SAFE_MODE_WAIT
    return SAFE_MODE_NONE;
    #endif
    port_set_saved_word(SAFE_MODE_DATA_GUARD | (SAFE_MODE_USER << 8));
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
        return SAFE_MODE_USER;
    }
    // Restore the original state of the saved word if no reset occurred during our wait period.
    port_set_saved_word(reset_state);
    return SAFE_MODE_NONE;
}

void PLACE_IN_ITCM(safe_mode_on_next_reset)(safe_mode_t reason) {
    port_set_saved_word(SAFE_MODE_DATA_GUARD | (reason << 8));
}

// Don't inline this so it's easy to break on it from GDB.
void __attribute__((noinline,)) PLACE_IN_ITCM(reset_into_safe_mode)(safe_mode_t reason) {
    if (_safe_mode > SAFE_MODE_BROWNOUT && reason > SAFE_MODE_BROWNOUT) {
        while (true) {
            // This very bad because it means running in safe mode didn't save us. Only ignore brownout
            // because it may be due to a switch bouncing.
        }
    }

    safe_mode_on_next_reset(reason);
    reset_cpu();
}



void print_safe_mode_message(safe_mode_t reason) {
    if (reason == SAFE_MODE_NONE) {
        return;
    }

    serial_write_compressed(translate("\nYou are in safe mode because:\n"));

    const compressed_string_t *message = NULL;

    // First check for safe mode reasons that do not necessarily reflect bugs.

    switch (reason) {
        case SAFE_MODE_BROWNOUT:
            message = translate("The power dipped. Make sure you are providing enough power.");
            break;
        case SAFE_MODE_USER:
            #if defined(BOARD_USER_SAFE_MODE_ACTION)
            message = BOARD_USER_SAFE_MODE_ACTION;
            #elif defined(CIRCUITPY_BOOT_BUTTON)
            message = translate("You pressed the BOOT button at start up");
            #else
            message = translate("You pressed the reset button during boot.");
            #endif
            break;
        case SAFE_MODE_NO_CIRCUITPY:
            message = translate("CIRCUITPY drive could not be found or created.");
            break;
        case SAFE_MODE_PROGRAMMATIC:
            message = translate("The `microcontroller` module was used to boot into safe mode.");
            break;
        #if CIRCUITPY_SAFEMODE_PY
        case SAFE_MODE_SAFEMODE_PY_ERROR:
            message = translate("Error in safemode.py.");
            break;
        #endif
        case SAFE_MODE_STACK_OVERFLOW:
            message = translate("Heap was corrupted because the stack was too small. Increase stack size.");
            break;
        case SAFE_MODE_USB_TOO_MANY_ENDPOINTS:
            message = translate("USB devices need more endpoints than are available.");
            break;
        case SAFE_MODE_USB_TOO_MANY_INTERFACE_NAMES:
            message = translate("USB devices specify too many interface names.");
            break;
        case SAFE_MODE_USB_BOOT_DEVICE_NOT_INTERFACE_ZERO:
            message = translate("Boot device must be first (interface #0).");
            break;
        case SAFE_MODE_WATCHDOG:
            message = translate("Internal watchdog timer expired.");
            break;
        default:
            break;
    }

    if (message) {
        // Non-crash safe mode.
        serial_write_compressed(message);
    } else {
        // Something worse happened.
        serial_write_compressed(translate("CircuitPython core code crashed hard. Whoops!\n"));
        switch (reason) {
            case SAFE_MODE_GC_ALLOC_OUTSIDE_VM:
                message = translate("Heap allocation when VM not running.");
                break;
            case SAFE_MODE_FLASH_WRITE_FAIL:
                message = translate("Failed to write internal flash.");
                break;
            case SAFE_MODE_HARD_FAULT:
                message = translate("Fault detected by hardware.");
                break;
            case SAFE_MODE_INTERRUPT_ERROR:
                message = translate("Interrupt error.");
                break;
            case SAFE_MODE_NLR_JUMP_FAIL:
                message = translate("NLR jump failed. Likely memory corruption.");
                break;
            case SAFE_MODE_NO_HEAP:
                message = translate("Unable to allocate the heap.");
                break;
            case SAFE_MODE_SDK_FATAL_ERROR:
                message = translate("Third-party firmware fatal error.");
                break;
            default:
                message = translate("Unknown reason.");
                break;
        }
        serial_write_compressed(message);
        serial_write_compressed(translate("\nPlease file an issue with your program at https://github.com/adafruit/circuitpython/issues."));
    }

    // Always tell user how to get out of safe mode.
    serial_write_compressed(translate("\nPress reset to exit safe mode.\n"));
}
