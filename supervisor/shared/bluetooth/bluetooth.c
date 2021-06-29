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

#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/tick.h"

#include "py/mpstate.h"

#if CIRCUITPY_BLE_FILE_SERVICE
#include "supervisor/shared/bluetooth/file_transfer.h"
#include "bluetooth/ble_drv.h"
#endif

#if CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/serial.h"
#include "bluetooth/ble_drv.h"
#endif

// This standard advertisement advertises the CircuitPython editing service and a CIRCUITPY short name.
const uint8_t public_advertising_data[] = { 0x02, 0x01, 0x06, // 0-2 Flags
                                            0x02, 0x0a, 0xd8,  // 3-5 TX power level -40
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
// This scan response advertises the full CIRCPYXXXX device name.
uint8_t circuitpython_scan_response_data[] = {
    0x0a, 0x09, 0x43, 0x49, 0x52, 0x50, 0x59, 0x00, 0x00, 0x00, 0x00,
    #if CIRCUITPY_SERIAL_BLE
    0x11, 0x06, 0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e,
    #endif
};

bool boot_in_discovery_mode = false;
bool advertising = false;

STATIC void supervisor_bluetooth_start_advertising(void) {
    #if !CIRCUITPY_BLE_FILE_SERVICE && !CIRCUITPY_SERIAL_BLE
    return;
    #else
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
    // Advertise with less power when doing so publicly to reduce who can hear us. This will make it
    // harder for someone with bad intentions to pair from a distance.
    if (!bonded) {
        tx_power = -40;
        adv = public_advertising_data;
        adv_len = sizeof(public_advertising_data);
        scan_response = circuitpython_scan_response_data;
        scan_response_len = sizeof(circuitpython_scan_response_data);
    }
    uint32_t status = _common_hal_bleio_adapter_start_advertising(&common_hal_bleio_adapter_obj,
        true,
        bonded, // Advertise anonymously if we are bonded
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
    #endif
}

#define BLE_DISCOVERY_DATA_GUARD 0xbb0000bb
#define BLE_DISCOVERY_DATA_GUARD_MASK 0xff0000ff

void supervisor_bluetooth_init(void) {
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
    while (diff < 1000) {
        #ifdef CIRCUITPY_STATUS_LED
        // Blink on for 100, off for 100, on for 100, off for 100 and on for 200
        bool led_on = ble_mode != 0 || (diff % 150) <= 75;
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
}

STATIC bool was_connected;
void supervisor_bluetooth_background(void) {
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (was_connected && !is_connected) {
        #if CIRCUITPY_BLE_FILE_SERVICE
        supervisor_bluetooth_file_transfer_disconnected();
        #endif
    }
    was_connected = is_connected;
    if (!is_connected) {
        supervisor_bluetooth_start_advertising();
        return;
    }

    #if CIRCUITPY_BLE_FILE_SERVICE
    supervisor_bluetooth_file_transfer_background();
    #endif
}

void supervisor_start_bluetooth(void) {
    #if !CIRCUITPY_BLE_FILE_SERVICE && !CIRCUITPY_SERIAL_BLE
    return;
    #endif

    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);

    #if CIRCUITPY_BLE_FILE_SERVICE
    supervisor_start_bluetooth_file_transfer();
    #endif

    #if CIRCUITPY_SERIAL_BLE
    supervisor_start_bluetooth_serial();
    #endif

    // Kick off advertisments
    supervisor_bluetooth_background();
}
