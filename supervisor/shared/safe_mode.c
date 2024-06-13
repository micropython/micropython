// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/safe_mode.h"

#include "mphalport.h"

#if defined(CIRCUITPY_BOOT_BUTTON)
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/time/__init__.h"
#endif
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "supervisor/linker.h"
#include "supervisor/shared/rgb_led_colors.h"
#include "supervisor/shared/serial.h"
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
        // Init the boot button every time in case it is used for LEDs.
        #ifdef CIRCUITPY_BOOT_BUTTON
        digitalio_digitalinout_obj_t boot_button;
        common_hal_digitalio_digitalinout_construct(&boot_button, CIRCUITPY_BOOT_BUTTON);
        common_hal_digitalio_digitalinout_switch_to_input(&boot_button, PULL_UP);
        common_hal_time_delay_ms(1);
        bool button_pressed = !common_hal_digitalio_digitalinout_get_value(&boot_button);
        common_hal_digitalio_digitalinout_deinit(&boot_button);
        if (button_pressed) {
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
void __attribute__((noinline, )) PLACE_IN_ITCM(reset_into_safe_mode)(safe_mode_t reason) {
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

    serial_write_compressed(MP_ERROR_TEXT("\nYou are in safe mode because:\n"));

    mp_rom_error_text_t message = NULL;

    // First check for safe mode reasons that do not necessarily reflect bugs.

    switch (reason) {
        case SAFE_MODE_BROWNOUT:
            message = MP_ERROR_TEXT("Power dipped. Make sure you are providing enough power.");
            break;
        case SAFE_MODE_USER:
            #if defined(BOARD_USER_SAFE_MODE_ACTION)
            message = BOARD_USER_SAFE_MODE_ACTION;
            #elif defined(CIRCUITPY_BOOT_BUTTON)
            message = MP_ERROR_TEXT("You pressed the BOOT button at start up");
            #else
            message = MP_ERROR_TEXT("You pressed the reset button during boot.");
            #endif
            break;
        case SAFE_MODE_NO_CIRCUITPY:
            message = MP_ERROR_TEXT("CIRCUITPY drive could not be found or created.");
            break;
        case SAFE_MODE_PROGRAMMATIC:
            message = MP_ERROR_TEXT("The `microcontroller` module was used to boot into safe mode.");
            break;
        #if CIRCUITPY_SAFEMODE_PY
        case SAFE_MODE_SAFEMODE_PY_ERROR:
            message = MP_ERROR_TEXT("Error in safemode.py.");
            break;
        #endif
        case SAFE_MODE_STACK_OVERFLOW:
            message = MP_ERROR_TEXT("Stack overflow. Increase stack size.");
            break;
        case SAFE_MODE_USB_TOO_MANY_ENDPOINTS:
            message = MP_ERROR_TEXT("USB devices need more endpoints than are available.");
            break;
        case SAFE_MODE_USB_TOO_MANY_INTERFACE_NAMES:
            message = MP_ERROR_TEXT("USB devices specify too many interface names.");
            break;
        case SAFE_MODE_USB_BOOT_DEVICE_NOT_INTERFACE_ZERO:
            message = MP_ERROR_TEXT("Boot device must be first (interface #0).");
            break;
        case SAFE_MODE_WATCHDOG:
            message = MP_ERROR_TEXT("Internal watchdog timer expired.");
            break;
        default:
            break;
    }

    if (message) {
        // Non-crash safe mode.
        serial_write_compressed(message);
    } else {
        // Something worse happened.
        serial_write_compressed(MP_ERROR_TEXT("CircuitPython core code crashed hard. Whoops!\n"));
        switch (reason) {
            case SAFE_MODE_GC_ALLOC_OUTSIDE_VM:
                message = MP_ERROR_TEXT("Heap allocation when VM not running.");
                break;
            case SAFE_MODE_FLASH_WRITE_FAIL:
                message = MP_ERROR_TEXT("Failed to write internal flash.");
                break;
            case SAFE_MODE_HARD_FAULT:
                message = MP_ERROR_TEXT("Hard fault: memory access or instruction error.");
                break;
            case SAFE_MODE_INTERRUPT_ERROR:
                message = MP_ERROR_TEXT("Interrupt error.");
                break;
            case SAFE_MODE_NLR_JUMP_FAIL:
                message = MP_ERROR_TEXT("NLR jump failed. Likely memory corruption.");
                break;
            case SAFE_MODE_NO_HEAP:
                message = MP_ERROR_TEXT("Unable to allocate to the heap.");
                break;
            case SAFE_MODE_SDK_FATAL_ERROR:
                message = MP_ERROR_TEXT("Third-party firmware fatal error.");
                break;
            default:
                message = MP_ERROR_TEXT("Unknown reason.");
                break;
        }
        serial_write_compressed(message);
        serial_write_compressed(MP_ERROR_TEXT("\nPlease file an issue with your program at github.com/adafruit/circuitpython/issues."));
    }

    // Always tell user how to get out of safe mode.
    serial_write_compressed(MP_ERROR_TEXT("\nPress reset to exit safe mode.\n"));
}
