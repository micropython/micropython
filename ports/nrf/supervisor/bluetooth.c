/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/bluetooth.h"
#include "supervisor/bluetooth.h"

// This happens in an interrupt so we need to be quick.
bool supervisor_bluetooth_hook(ble_evt_t *ble_evt) {
#if CIRCUITPY_BLE_FILE_SERVICE
    // Catch writes to filename or contents. Length is read-only.

    bool done = false;
    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            // We run our background task even if it wasn't us connected to because we may want to
            // advertise if the user code stopped advertising.
            run_ble_background = true;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            run_ble_background = true;
            break;
        case BLE_GATTS_EVT_WRITE: {
            // A client wrote to a characteristic.

            ble_gatts_evt_write_t *evt_write = &ble_evt->evt.gatts_evt.params.write;
            // Event handle must match the handle for my characteristic.
            if (evt_write->handle == supervisor_ble_contents_characteristic.handle) {
                // Handle events
                //write_to_ringbuf(self, evt_write->data, evt_write->len);
                // First packet includes a uint16_t le for length at the start.
                uint16_t current_length = ((uint16_t*) current_command)[0];
                memcpy(((uint8_t*) current_command) + current_offset, evt_write->data, evt_write->len);
                current_offset += evt_write->len;
                current_length = ((uint16_t*) current_command)[0];
                if (current_offset == current_length) {
                    run_ble_background = true;
                    done = true;
                }
            } else if (evt_write->handle == supervisor_ble_filename_characteristic.handle) {
                new_filename = true;
                run_ble_background = true;
                done = true;
            } else {
                return done;
            }
            break;
        }

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled peripheral event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
    return done;
#else
    return false;
#endif
}
