/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/bluetooth/bluetooth.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#if defined(CIRCUITPY_BOOT_BUTTON)
#include "shared-bindings/digitalio/DigitalInOut.h"
#endif
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "shared-module/storage/__init__.h"

#include "common-hal/_bleio/__init__.h"

#include "supervisor/shared/serial.h"
#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/translate/translate.h"

#include "py/mpstate.h"

#if CIRCUITPY_BLE_FILE_SERVICE
#include "supervisor/shared/bluetooth/file_transfer.h"
#include "bluetooth/ble_drv.h"
#endif

#if CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/serial.h"
#include "bluetooth/ble_drv.h"
#endif

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

// This standard advertisement advertises the CircuitPython editing service and a CIRCUITPY short name.
const uint8_t public_advertising_data[] = { 0x02, 0x01, 0x06, // 0-2 Flags
                                            0x02, 0x0a, 0xec,  // 3-5 TX power level -20
                                            #if CIRCUITPY_BLE_FILE_SERVICE
                                            0x03, 0x02, 0xbb, 0xfe,  // 6 - 9 Incomplete service list (File Transfer service)
                                            #endif
                                            0x0e, 0xff, 0x22, 0x08,  // 10 - 13 Adafruit Manufacturer Data
                                            0x0a, 0x04, 0x00,        // 14 - 16 Creator ID / Creation ID
                                            CIRCUITPY_CREATOR_ID & 0xff,              // 17 - 20 Creator ID
                                            (CIRCUITPY_CREATOR_ID >> 8) & 0xff,
                                            (CIRCUITPY_CREATOR_ID >> 16) & 0xff,
                                            (CIRCUITPY_CREATOR_ID >> 24) & 0xff,
                                            CIRCUITPY_CREATION_ID & 0xff,              // 21 - 24 Creation ID
                                            (CIRCUITPY_CREATION_ID >> 8) & 0xff,
                                            (CIRCUITPY_CREATION_ID >> 16) & 0xff,
                                            (CIRCUITPY_CREATION_ID >> 24) & 0xff,
                                            0x05, 0x08, 0x43, 0x49, 0x52, 0x43  // 25 - 31 - Short name
};
const uint8_t private_advertising_data[] = { 0x02, 0x01, 0x06, // 0-2 Flags
                                             0x02, 0x0a, 0x00 // 3-5 TX power level 0
};
// This scan response advertises the full device name (if it fits.)
uint8_t circuitpython_scan_response_data[31];

#if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
STATIC bool boot_in_discovery_mode = false;
STATIC bool advertising = false;
STATIC bool _private_advertising = false;
STATIC bool ble_started = false;

#define WORKFLOW_UNSET 0
#define WORKFLOW_ENABLED 1
#define WORKFLOW_DISABLED 2

STATIC uint8_t workflow_state = WORKFLOW_UNSET;
STATIC bool was_connected = false;

#if CIRCUITPY_STATUS_BAR
// To detect when the title bar changes.
STATIC bool _last_connected = false;
STATIC bool _last_advertising = false;
#endif

#if CIRCUITPY_STATUS_BAR
// Title bar status
bool supervisor_bluetooth_status_dirty(void) {
    return _last_advertising != advertising ||
           _last_connected != was_connected;
}
#endif

#if CIRCUITPY_STATUS_BAR
void supervisor_bluetooth_status(void) {
    serial_write("BLE:");
    if (advertising) {
        if (_private_advertising) {
            serial_write_compressed(MP_ERROR_TEXT("Reconnecting"));
        } else {
            const char *name = (char *)circuitpython_scan_response_data + 2;
            int len = MIN(strlen(name), sizeof(circuitpython_scan_response_data) - 2);
            serial_write_substring(name, len);
        }
    } else if (was_connected) {
        serial_write_compressed(MP_ERROR_TEXT("Ok"));
    } else {
        serial_write_compressed(MP_ERROR_TEXT("Off"));
    }

    _last_connected = was_connected;
    _last_advertising = advertising;
}
#endif

STATIC void supervisor_bluetooth_start_advertising(void) {
    if (workflow_state != WORKFLOW_ENABLED) {
        return;
    }
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (is_connected) {
        return;
    }
    bool bonded = common_hal_bleio_adapter_is_bonded_to_central(&common_hal_bleio_adapter_obj);
    #if CIRCUITPY_USB
    // Don't advertise when we have USB instead of BLE.
    if (!bonded && !boot_in_discovery_mode) {
        return;
    }
    #endif
    uint32_t timeout = 0;
    float interval = 0.1f;
    int tx_power = 0;
    const uint8_t *adv = private_advertising_data;
    size_t adv_len = sizeof(private_advertising_data);
    const uint8_t *scan_response = NULL;
    size_t scan_response_len = 0;
    _private_advertising = true;
    // Advertise with less power when doing so publicly to reduce who can hear us. This will make it
    // harder for someone with bad intentions to pair from a distance.
    if (!bonded || boot_in_discovery_mode) {
        tx_power = -20;
        adv = public_advertising_data;
        adv_len = sizeof(public_advertising_data);
        scan_response = circuitpython_scan_response_data;
        scan_response_len = sizeof(circuitpython_scan_response_data);
        uint16_t max_name_len = sizeof(circuitpython_scan_response_data) - 2;
        uint16_t name_len = bleio_adapter_get_name((char *)circuitpython_scan_response_data + 2,
            max_name_len);
        if (name_len > max_name_len) {
            circuitpython_scan_response_data[0] = max_name_len + 1;
            circuitpython_scan_response_data[1] = 0x8;
        } else {
            circuitpython_scan_response_data[0] = name_len + 1;
            circuitpython_scan_response_data[1] = 0x9;
        }
        scan_response_len = circuitpython_scan_response_data[0] + 1;
        _private_advertising = false;
    }
    uint32_t status = _common_hal_bleio_adapter_start_advertising(&common_hal_bleio_adapter_obj,
        true,
        _private_advertising, // Advertise anonymously if we are privately advertising
        timeout,
        interval,
        adv,
        adv_len,
        scan_response,
        scan_response_len,
        tx_power,
        NULL);
    // This may fail if we are already advertising.
    advertising = status == NRF_SUCCESS;
}

#endif  // CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE

#define BLE_DISCOVERY_DATA_GUARD 0xbb0000bb
#define BLE_DISCOVERY_DATA_GUARD_MASK 0xff0000ff

void supervisor_bluetooth_init(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    uint32_t reset_state = port_get_saved_word();
    uint32_t ble_mode = 0;
    if ((reset_state & BLE_DISCOVERY_DATA_GUARD_MASK) == BLE_DISCOVERY_DATA_GUARD) {
        ble_mode = (reset_state & ~BLE_DISCOVERY_DATA_GUARD_MASK) >> 8;
    }
    const mcu_reset_reason_t reset_reason = common_hal_mcu_processor_get_reset_reason();
    boot_in_discovery_mode = false;
    if (reset_reason != RESET_REASON_POWER_ON &&
        reset_reason != RESET_REASON_RESET_PIN &&
        reset_reason != RESET_REASON_UNKNOWN &&
        reset_reason != RESET_REASON_SOFTWARE) {
        return;
    }

    if (ble_mode == 0) {
        port_set_saved_word(BLE_DISCOVERY_DATA_GUARD | (0x01 << 8));
    }
    // Wait for a while to allow for reset.

    #ifdef CIRCUITPY_BOOT_BUTTON
    digitalio_digitalinout_obj_t boot_button;
    common_hal_digitalio_digitalinout_construct(&boot_button, CIRCUITPY_BOOT_BUTTON);
    common_hal_digitalio_digitalinout_switch_to_input(&boot_button, PULL_UP);
    #endif
    #if CIRCUITPY_STATUS_LED
    status_led_init();
    #endif
    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t diff = 0;
    if (ble_mode != 0) {
        #ifdef CIRCUITPY_STATUS_LED
        new_status_color(0x0000ff);
        #endif
        common_hal_bleio_adapter_erase_bonding(&common_hal_bleio_adapter_obj);
        boot_in_discovery_mode = true;
        reset_state = 0x0;
    }
    #if !CIRCUITPY_USB
    // Boot into discovery if USB isn't available and we aren't bonded already.
    // Checking here allows us to have the status LED solidly on even if no button was
    // pressed.
    bool bonded = common_hal_bleio_adapter_is_bonded_to_central(&common_hal_bleio_adapter_obj);
    if (!bonded) {
        boot_in_discovery_mode = true;
    }
    #endif
    while (diff < 1000) {
        #ifdef CIRCUITPY_STATUS_LED
        // Blink on for 50 and off for 100
        bool led_on = boot_in_discovery_mode || (diff % 150) <= 50;
        if (led_on) {
            new_status_color(0x0000ff);
        } else {
            new_status_color(BLACK);
        }
        #endif
        #ifdef CIRCUITPY_BOOT_BUTTON
        if (!common_hal_digitalio_digitalinout_get_value(&boot_button)) {
            boot_in_discovery_mode = true;
            break;
        }
        #endif
        diff = supervisor_ticks_ms64() - start_ticks;
    }
    #if CIRCUITPY_STATUS_LED
    new_status_color(BLACK);
    status_led_deinit();
    #endif
    port_set_saved_word(reset_state);
    #endif
}

void supervisor_bluetooth_background(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    if (!ble_started) {
        return;
    }
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (was_connected && !is_connected) {
        #if CIRCUITPY_BLE_FILE_SERVICE
        supervisor_bluetooth_file_transfer_disconnected();
        #endif
    }

    #if CIRCUITPY_STATUS_BAR
    if (was_connected != is_connected) {
        supervisor_status_bar_request_update(false);
    }
    #endif

    was_connected = is_connected;
    if (!is_connected) {
        supervisor_bluetooth_start_advertising();
        return;
    } else {
        advertising = false;
    }

    #if CIRCUITPY_BLE_FILE_SERVICE
    supervisor_bluetooth_file_transfer_background();
    #endif
    #endif
}

void supervisor_start_bluetooth(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE

    if (workflow_state != WORKFLOW_ENABLED || ble_started) {
        return;
    }

    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);

    #if CIRCUITPY_BLE_FILE_SERVICE
    supervisor_start_bluetooth_file_transfer();
    #endif

    #if CIRCUITPY_SERIAL_BLE
    supervisor_start_bluetooth_serial();
    #endif

    // Mark as started so that the background call does something.
    ble_started = true;

    // Kick off advertisements
    supervisor_bluetooth_background();

    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_request_update(false);
    #endif

    #endif
}

void supervisor_stop_bluetooth(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE

    if (!ble_started && workflow_state != WORKFLOW_ENABLED) {
        return;
    }

    ble_started = false;

    #if CIRCUITPY_SERIAL_BLE
    supervisor_stop_bluetooth_serial();
    #endif

    #endif
}

void supervisor_bluetooth_enable_workflow(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    if (workflow_state == WORKFLOW_DISABLED) {
        return;
    }
    workflow_state = WORKFLOW_ENABLED;
    #endif
}

void supervisor_bluetooth_disable_workflow(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    workflow_state = WORKFLOW_DISABLED;
    #endif
}

bool supervisor_bluetooth_workflow_is_enabled(void) {
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    if (workflow_state == 1) {
        return true;
    }
    #endif
    return false;
}
