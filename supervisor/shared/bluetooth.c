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

#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

#include "common-hal/_bleio/__init__.h"

#include "supervisor/shared/autoreload.h"

#include "py/mpstate.h"

bleio_service_obj_t supervisor_ble_service;
bleio_uuid_obj_t supervisor_ble_service_uuid;
bleio_characteristic_obj_t supervisor_ble_version_characteristic;
bleio_uuid_obj_t supervisor_ble_version_uuid;
bleio_characteristic_obj_t supervisor_ble_filename_characteristic;
bleio_uuid_obj_t supervisor_ble_filename_uuid;
bleio_characteristic_obj_t supervisor_ble_length_characteristic;
bleio_uuid_obj_t supervisor_ble_length_uuid;
bleio_characteristic_obj_t supervisor_ble_contents_characteristic;
bleio_uuid_obj_t supervisor_ble_contents_uuid;

// This is the base UUID for CircuitPython services and characteristics.
const uint8_t circuitpython_base_uuid[16] = {0x6e, 0x68, 0x74, 0x79, 0x50, 0x74, 0x69, 0x75, 0x63, 0x72, 0x69, 0x43, 0x00, 0x00, 0xaf, 0xad };
// This standard advertisement advertises the CircuitPython editing service and a CIRCUITPY short name.
uint8_t circuitpython_advertising_data[] = { 0x02, 0x01, 0x06, 0x02, 0x0a, 0x00, 0x11, 0x07, 0x6e, 0x68, 0x74, 0x79, 0x50, 0x74, 0x69, 0x75, 0x63, 0x72, 0x69, 0x43, 0x00, 0x01, 0xaf, 0xad, 0x06, 0x08, 0x43, 0x49, 0x52, 0x43, 0x55 };
// This scan response advertises the full CIRCUITPYXXXX device name.
uint8_t circuitpython_scan_response_data[15] = {0x0e, 0x09, 0x43, 0x49, 0x52, 0x43, 0x55, 0x49, 0x54, 0x50, 0x59, 0x00, 0x00, 0x00, 0x00};
mp_obj_list_t service_list;
mp_obj_t service_list_items[1];
mp_obj_list_t characteristic_list;
mp_obj_t characteristic_list_items[4];

void supervisor_bluetooth_start_advertising(void) {
    #if !CIRCUITPY_BLE_FILE_SERVICE
    return;
    #endif
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (is_connected) {
        return;
    }
    // TODO: switch to Adafruit short UUID for the advertisement and add manufacturing data to distinguish ourselves from arduino.
    _common_hal_bleio_adapter_start_advertising(&common_hal_bleio_adapter_obj,
                                                   true,
                                                   false, 0,
                                                   1.0,
                                                   circuitpython_advertising_data,
                                                   sizeof(circuitpython_advertising_data),
                                                   circuitpython_scan_response_data,
                                                   sizeof(circuitpython_scan_response_data));
}

void supervisor_start_bluetooth(void) {
    #if !CIRCUITPY_BLE_FILE_SERVICE
    return;
    #endif

    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);

    supervisor_ble_service_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_service_uuid, 0x0100, circuitpython_base_uuid);

    // We know we'll only be 1 characteristic so we can statically allocate it.
    characteristic_list.base.type = &mp_type_list;
    characteristic_list.alloc = sizeof(characteristic_list_items) / sizeof(characteristic_list_items[0]);
    characteristic_list.len = 0;
    characteristic_list.items = characteristic_list_items;
    mp_seq_clear(characteristic_list.items, 0, characteristic_list.alloc, sizeof(*characteristic_list.items));

    _common_hal_bleio_service_construct(&supervisor_ble_service, &supervisor_ble_service_uuid, false /* is secondary */, &characteristic_list);

    // File length
    supervisor_ble_version_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_version_uuid, 0x0203, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_version_characteristic,
                                              &supervisor_ble_service,
                                              0, // handle (for remote only)
                                              &supervisor_ble_version_uuid,
                                              CHAR_PROP_READ,
                                              SECURITY_MODE_OPEN,
                                              SECURITY_MODE_NO_ACCESS,
                                              4, // max length
                                              true, // fixed length
                                              NULL); // no initial value

    uint32_t version = 1;
    mp_buffer_info_t bufinfo;
    bufinfo.buf = &version;
    bufinfo.len = sizeof(version);
    common_hal_bleio_characteristic_set_value(&supervisor_ble_version_characteristic, &bufinfo);

    // Active filename.
    supervisor_ble_filename_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_filename_uuid, 0x0200, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_filename_characteristic,
                                              &supervisor_ble_service,
                                              0, // handle (for remote only)
                                              &supervisor_ble_filename_uuid,
                                              CHAR_PROP_READ | CHAR_PROP_WRITE,
                                              SECURITY_MODE_OPEN,
                                              SECURITY_MODE_OPEN,
                                              500, // max length
                                              false, // fixed length
                                              NULL); // no initial value

    char code_py[] = "/code.py";
    bufinfo.buf = code_py;
    bufinfo.len = sizeof(code_py);
    common_hal_bleio_characteristic_set_value(&supervisor_ble_filename_characteristic, &bufinfo);

    // File length
    supervisor_ble_length_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_length_uuid, 0x0202, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_length_characteristic,
                                              &supervisor_ble_service,
                                              0, // handle (for remote only)
                                              &supervisor_ble_length_uuid,
                                              CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                                              SECURITY_MODE_OPEN,
                                              SECURITY_MODE_NO_ACCESS,
                                              4, // max length
                                              true, // fixed length
                                              NULL); // no initial value

    // File actions
    supervisor_ble_contents_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_contents_uuid, 0x0201, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_contents_characteristic,
                                              &supervisor_ble_service,
                                              0, // handle (for remote only)
                                              &supervisor_ble_contents_uuid,
                                              CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE,
                                              SECURITY_MODE_OPEN,
                                              SECURITY_MODE_OPEN,
                                              500, // max length
                                              false, // fixed length
                                              NULL); // no initial value

    supervisor_bluetooth_start_advertising();
    vm_used_ble = false;
}

FIL active_file;
volatile bool new_filename;
volatile bool run_ble_background;
bool was_connected;

void update_file_length(void) {
    int32_t file_length = -1;
    mp_buffer_info_t bufinfo;
    bufinfo.buf = &file_length;
    bufinfo.len = sizeof(file_length);
    if (active_file.obj.fs != 0) {
        file_length = (int32_t) f_size(&active_file);
    }
    common_hal_bleio_characteristic_set_value(&supervisor_ble_length_characteristic, &bufinfo);
}

void open_current_file(void) {
    if (active_file.obj.fs != 0) {
        return;
    }
    uint16_t max_len = supervisor_ble_filename_characteristic.max_length;
    uint8_t path[max_len];
    size_t length = common_hal_bleio_characteristic_get_value(&supervisor_ble_filename_characteristic, path, max_len - 1);
    path[length] = '\0';

    FATFS *fs = &((fs_user_mount_t *) MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    f_open(fs, &active_file, (char*) path, FA_READ | FA_WRITE);

    update_file_length();
}

void close_current_file(void) {
    f_close(&active_file);
}

uint32_t current_command[1024 / sizeof(uint32_t)];
volatile size_t current_offset;

void supervisor_bluetooth_background(void) {
    #if !CIRCUITPY_BLE_FILE_SERVICE
    return;
    #endif
    if (!run_ble_background) {
        return;
    }
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (!was_connected && is_connected) {
        open_current_file();
    } else if (was_connected && !is_connected) {
        close_current_file();
        new_filename = false;
    }
    was_connected = is_connected;
    run_ble_background = false;
    if (!is_connected) {
        supervisor_bluetooth_start_advertising();
        return;
    }
    if (new_filename) {
        close_current_file();
        open_current_file();

        new_filename = false;
        // get length and set the characteristic for it
    }
    uint16_t current_length = ((uint16_t*) current_command)[0];
    if (current_length > 0 && current_length == current_offset) {
        uint16_t command = ((uint16_t *) current_command)[1];

        if (command == 1) {
            uint16_t max_len = 20; //supervisor_ble_contents_characteristic.max_length;
            uint8_t buf[max_len];
            mp_buffer_info_t bufinfo;
            bufinfo.buf = buf;
            f_lseek(&active_file, 0);
            while (f_read(&active_file, buf, max_len, &bufinfo.len) == FR_OK) {
                if (bufinfo.len == 0) {
                    break;
                }
                common_hal_bleio_characteristic_set_value(&supervisor_ble_contents_characteristic, &bufinfo);
            }
        } else if (command == 2) { // patch
            uint32_t offset = current_command[1];
            uint32_t remove_length = current_command[2];
            uint32_t insert_length = current_command[3];
            uint32_t file_length =  (int32_t) f_size(&active_file);
            //uint32_t data_shift_length = fileLength - offset - remove_length;
            int32_t data_shift = insert_length - remove_length;
            uint32_t new_length = file_length + data_shift;

            // TODO: Make these loops smarter to read and write on sector boundaries.
            if (data_shift < 0) {
                for (uint32_t shift_offset = offset + insert_length; shift_offset < new_length; shift_offset++) {
                    uint8_t data;
                    UINT actual;
                    f_lseek(&active_file, shift_offset - data_shift);
                    f_read(&active_file, &data, 1, &actual);
                    f_lseek(&active_file, shift_offset);
                    f_write(&active_file, &data, 1, &actual);
                }
                f_truncate(&active_file);
            } else if (data_shift > 0) {
                f_lseek(&active_file, file_length);
                // Fill end with 0xff so we don't need to erase.
                uint8_t data = 0xff;
                for (size_t i = 0; i < (size_t) data_shift; i++) {
                    UINT actual;
                    f_write(&active_file, &data, 1, &actual);
                }
                for (uint32_t shift_offset = new_length - 1; shift_offset >= offset + insert_length ; shift_offset--) {
                    UINT actual;
                    f_lseek(&active_file, shift_offset - data_shift);
                    f_read(&active_file, &data, 1, &actual);
                    f_lseek(&active_file, shift_offset);
                    f_write(&active_file, &data, 1, &actual);
                }
            }

            f_lseek(&active_file, offset);
            uint8_t* data = (uint8_t *) (current_command + 4);
            UINT written;
            f_write(&active_file, data, insert_length, &written);
            f_sync(&active_file);
            // Notify the new file length.
            update_file_length();

            // Trigger an autoreload
            autoreload_now();
        }
        current_offset = 0;
    }
}

// This happens in an interrupt so we need to be quick.
bool supervisor_bluetooth_hook(ble_evt_t *ble_evt) {
    #if !CIRCUITPY_BLE_FILE_SERVICE
    return false;
    #endif
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
}
