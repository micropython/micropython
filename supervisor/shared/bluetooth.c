/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Scott Shawcroft for Adafruit Industries
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

#if !CIRCUITPY_BLE_FILE_SERVICE

void supervisor_bluetooth_init(void) {
}

void supervisor_start_bluetooth(void) {
}

void supervisor_bluetooth_background(void) {
}

#else

#include <string.h>

#include "supervisor/shared/bluetooth.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#if defined(CIRCUITPY_BOOT_BUTTON)
#include "shared-bindings/digitalio/DigitalInOut.h"
#endif
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "shared-module/storage/__init__.h"

#include "bluetooth/ble_drv.h"

#include "common-hal/_bleio/__init__.h"

#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include "py/mpstate.h"

bleio_service_obj_t supervisor_ble_service;
bleio_uuid_obj_t supervisor_ble_service_uuid;
bleio_characteristic_obj_t supervisor_ble_version_characteristic;
bleio_uuid_obj_t supervisor_ble_version_uuid;
bleio_characteristic_obj_t supervisor_ble_transfer_characteristic;
bleio_uuid_obj_t supervisor_ble_transfer_uuid;

// This is the base UUID for the file transfer service.
const uint8_t file_transfer_base_uuid[16] = {0x72, 0x65, 0x66, 0x73, 0x6e, 0x61, 0x72, 0x54, 0x65, 0x6c, 0x69, 0x46, 0x00, 0x00, 0xaf, 0xad };
// This standard advertisement advertises the CircuitPython editing service and a CIRCUITPY short name.
const uint8_t public_advertising_data[] = { 0x02, 0x01, 0x06, // 0-2 Flags
                                            0x02, 0x0a, 0xd8,  // 3-5 TX power level -40
                                            0x03, 0x02, 0xbb, 0xfe,  // 6 - 9 Incomplete service list (File Transfer service)
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
// This scan response advertises the full CIRCUITPYXXXX device name.
uint8_t circuitpython_scan_response_data[15] = {0x0e, 0x09, 0x43, 0x49, 0x52, 0x43, 0x55, 0x49, 0x54, 0x50, 0x59, 0x00, 0x00, 0x00, 0x00};
mp_obj_list_t service_list;
mp_obj_t service_list_items[1];
mp_obj_list_t characteristic_list;
mp_obj_t characteristic_list_items[2];
// 2 * 10 ringbuf packets, 512 for a disk sector and 12 for the file transfer write header.
#define PACKET_BUFFER_SIZE (2 * 10 + 512 + 12)
// uint32_t so its aligned
uint32_t _buffer[PACKET_BUFFER_SIZE / 4 + 1];
uint32_t _outgoing1[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
uint32_t _outgoing2[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
ble_drv_evt_handler_entry_t static_handler_entry;
bleio_packet_buffer_obj_t _transfer_packet_buffer;
bool boot_in_discovery_mode = false;
bool advertising = false;

STATIC void supervisor_bluetooth_start_advertising(void) {
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (is_connected) {
        return;
    }
    bool bonded = common_hal_bleio_adapter_is_bonded_to_central(&common_hal_bleio_adapter_obj);
    #if CIRCUITPY_USB
    // Don't advertise when we have USB instead of BLE.
    if (!bonded && !boot_in_discovery_mode) {
        // mp_printf(&mp_plat_print, "skipping advertising\n");
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

    // ble_mode = 1;

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

void supervisor_start_bluetooth(void) {
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);

    supervisor_ble_service_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_service_uuid, 0xfebb, NULL);

    // We know we'll only be 1 characteristic so we can statically allocate it.
    characteristic_list.base.type = &mp_type_list;
    characteristic_list.alloc = sizeof(characteristic_list_items) / sizeof(characteristic_list_items[0]);
    characteristic_list.len = 0;
    characteristic_list.items = characteristic_list_items;
    mp_seq_clear(characteristic_list.items, 0, characteristic_list.alloc, sizeof(*characteristic_list.items));

    _common_hal_bleio_service_construct(&supervisor_ble_service, &supervisor_ble_service_uuid, false /* is secondary */, &characteristic_list);

    // Version number
    supervisor_ble_version_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_version_uuid, 0x0100, file_transfer_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_version_characteristic,
        &supervisor_ble_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_version_uuid,
        CHAR_PROP_READ,
        SECURITY_MODE_OPEN,
        SECURITY_MODE_NO_ACCESS,
        4,                                       // max length
        true,                                       // fixed length
        NULL);                                       // no initial value

    uint32_t version = 1;
    mp_buffer_info_t bufinfo;
    bufinfo.buf = &version;
    bufinfo.len = sizeof(version);
    common_hal_bleio_characteristic_set_value(&supervisor_ble_version_characteristic, &bufinfo);

    // Active filename.
    supervisor_ble_transfer_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_transfer_uuid, 0x0200, file_transfer_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_transfer_characteristic,
        &supervisor_ble_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_transfer_uuid,
        CHAR_PROP_READ | CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_NOTIFY,
        SECURITY_MODE_ENC_NO_MITM,
        SECURITY_MODE_ENC_NO_MITM,
        BLE_GATTS_VAR_ATTR_LEN_MAX,                  // max length
        false,                                       // fixed length
        NULL);                                       // no initial value

    _common_hal_bleio_packet_buffer_construct(
        &_transfer_packet_buffer, &supervisor_ble_transfer_characteristic,
        _buffer, PACKET_BUFFER_SIZE,
        _outgoing1, _outgoing2, BLE_GATTS_VAR_ATTR_LEN_MAX,
        &static_handler_entry);

    // Kick off advertisments
    supervisor_bluetooth_background();
}

#define COMMAND_SIZE 1024

#define ANY_COMMAND 0x00
#define THIS_COMMAND 0x01
#define READ 0x10
#define READ_DATA 0x11
#define READ_PACING 0x12
#define WRITE 0x20
#define WRITE_PACING 0x21
#define WRITE_DATA 0x22
#define DELETE 0x30
#define DELETE_STATUS 0x31
#define MKDIR 0x40
#define MKDIR_STATUS 0x41
#define LISTDIR 0x50
#define LISTDIR_ENTRY 0x51

#define STATUS_OK 0x01
#define STATUS_ERROR 0x02
#define STATUS_ERROR_NO_FILE 0x03
#define STATUS_ERROR_PROTOCOL 0x04

// Used by read and write.
FIL active_file;

struct read_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t chunk_offset;
    uint32_t chunk_size;
    uint8_t path[];
};

struct read_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t total_length;
    uint32_t data_size;
    uint8_t data[];
};

struct read_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t chunk_size;
};

uint8_t _process_read(const uint8_t *raw_buf, size_t command_len) {
    struct read_command *command = (struct read_command *)raw_buf;
    size_t header_size = 12;
    size_t response_size = 16;
    uint8_t data_buffer[response_size];
    struct read_data response;
    response.command = READ_DATA;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - response_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, response_size, NULL, 0);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }

    char *path = (char *)((uint8_t *)command) + header_size;
    path[command->path_length] = '\0';

    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    FRESULT result = f_open(fs, &active_file, path, FA_READ);
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, response_size, NULL, 0);
        return ANY_COMMAND;
    }
    uint32_t total_length = f_size(&active_file);
    // Write out the response header.
    uint32_t offset = command->chunk_offset;
    uint32_t chunk_size = command->chunk_size;
    chunk_size = MIN(chunk_size, total_length - offset);
    response.chunk_offset = offset;
    response.total_length = total_length;
    response.data_size = chunk_size;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, response_size, NULL, 0);
    f_lseek(&active_file, offset);
    // Write out the chunk contents. We can do this in small pieces because PacketBuffer
    // will assemble them into larger packets of its own.
    size_t chunk_end = offset + chunk_size;
    while (offset < chunk_end) {
        size_t quantity_read;
        size_t read_amount = MIN(response_size, chunk_end - offset);
        f_read(&active_file, data_buffer, read_amount, &quantity_read);
        offset += quantity_read;
        // TODO: Do something if the read fails
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, data_buffer, quantity_read, NULL, 0);
    }
    if (offset >= total_length) {
        f_close(&active_file);
        return ANY_COMMAND;
    }
    return READ_PACING;
}

uint8_t _process_read_pacing(const uint8_t *command, size_t command_len) {
    size_t response_size = 4 * sizeof(uint32_t);
    uint32_t response[response_size / sizeof(uint32_t)];
    uint8_t *response_bytes = (uint8_t *)response;
    response_bytes[0] = READ_DATA;
    response_bytes[1] = STATUS_OK;
    uint32_t offset = ((uint32_t *)command)[1];
    uint32_t chunk_size = ((uint32_t *)command)[2];
    uint32_t total_length = f_size(&active_file);
    // Write out the response header.
    chunk_size = MIN(chunk_size, total_length - offset);
    response[1] = offset;
    response[2] = total_length;
    response[3] = chunk_size;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, response_size, NULL, 0);
    f_lseek(&active_file, offset);
    // Write out the chunk contents. We can do this in small pieces because PacketBuffer
    // will assemble them into larger packets of its own.
    size_t chunk_offset = 0;
    while (chunk_offset < chunk_size) {
        size_t quantity_read;
        size_t read_size = MIN(chunk_size - chunk_offset, response_size);
        FRESULT result = f_read(&active_file, response, read_size, &quantity_read);
        if (quantity_read == 0 || result != FR_OK) {
            // TODO: If we can't read everything, then the file must have been shortened. Maybe we
            // should return 0s to pad it out.
            break;
        }
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, quantity_read, NULL, 0);
        chunk_offset += quantity_read;
    }
    if ((offset + chunk_size) >= total_length) {
        f_close(&active_file);
        return ANY_COMMAND;
    }
    return READ_PACING;
}

// Used by write and write data to know when the write is complete.
size_t total_write_length;

struct write_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t offset;
    uint32_t total_length;
    uint8_t path[];
};

struct write_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint32_t data_size;
    uint8_t data[];
};

struct write_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint32_t free_space;
};

uint8_t _process_write(const uint8_t *raw_buf, size_t command_len) {
    struct write_command *command = (struct write_command *)raw_buf;
    size_t header_size = 12;
    struct write_pacing response;
    response.command = WRITE_PACING;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }
    total_write_length = command->total_length;

    char *path = (char *)command->path;
    path[command->path_length] = '\0';

    // Check to see if USB has already been mounted. If not, then we "eject" from USB until we're done.
    #if CIRCUITPY_USB_MSC
    if (storage_usb_enabled() && !usb_msc_lock()) {
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        return ANY_COMMAND;
    }
    #endif

    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    FRESULT result = f_open(fs, &active_file, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        return ANY_COMMAND;
    }
    // Write out the pacing response.

    // Align the next chunk to a sector boundary.
    uint32_t offset = command->offset;
    size_t chunk_size = MIN(total_write_length - offset, 512 - (offset % 512));
    response.offset = offset;
    response.free_space = chunk_size;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
    return WRITE_DATA;
}

uint8_t _process_write_data(const uint8_t *raw_buf, size_t command_len) {
    struct write_data *command = (struct write_data *)raw_buf;
    size_t header_size = 12;
    struct write_pacing response;
    response.command = WRITE_PACING;
    response.status = STATUS_OK;
    if (command->data_size > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->data_size) {
        return THIS_COMMAND;
    }
    uint32_t offset = command->offset;
    f_lseek(&active_file, offset);
    UINT actual;
    f_write(&active_file, command->data, command->data_size, &actual);
    if (actual < command->data_size) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        return ANY_COMMAND;
    }
    offset += command->data_size;
    // Align the next chunk to a sector boundary.
    size_t chunk_size = MIN(total_write_length - offset, 512);
    response.offset = offset;
    response.free_space = chunk_size;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
    if (total_write_length == offset) {
        f_truncate(&active_file);
        f_close(&active_file);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        // Trigger an autoreload
        autoreload_now();
        return ANY_COMMAND;
    }
    return WRITE_DATA;
}

struct delete_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

struct delete_response {
    uint8_t command;
    uint8_t status;
};

uint8_t _process_delete(const uint8_t *raw_buf, size_t command_len) {
    const struct delete_command *command = (struct delete_command *)raw_buf;
    size_t header_size = 4;
    struct delete_response response;
    response.command = DELETE_STATUS;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct delete_response), NULL, 0);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)((uint8_t *)command) + header_size;
    path[command->path_length] = '\0';
    FRESULT result = f_unlink(fs, path);
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
    }
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct delete_response), NULL, 0);
    return ANY_COMMAND;
}

struct mkdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

struct mkdir_response {
    uint8_t command;
    uint8_t status;
};

uint8_t _process_mkdir(const uint8_t *raw_buf, size_t command_len) {
    const struct mkdir_command *command = (struct mkdir_command *)raw_buf;
    size_t header_size = 4;
    struct mkdir_response response;
    response.command = MKDIR_STATUS;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct mkdir_response), NULL, 0);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)((uint8_t *)command) + header_size;
    // TODO: Check that the final character is a `/`
    path[command->path_length - 1] = '\0';
    FRESULT result = f_mkdir(fs, path);
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
    }
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct mkdir_response), NULL, 0);
    return ANY_COMMAND;
}

struct listdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

struct listdir_entry {
    uint8_t command;
    uint8_t status;
    uint16_t path_length;
    uint32_t entry_number;
    uint32_t entry_count;
    uint32_t flags;
    uint32_t file_size;
    uint8_t path[];
};

uint8_t _process_listdir(uint8_t *raw_buf, size_t command_len) {
    const struct listdir_command *command = (struct listdir_command *)raw_buf;
    struct listdir_entry *entry = (struct listdir_entry *)raw_buf;
    size_t header_size = 4;
    size_t response_size = 5 * sizeof(uint32_t);
    // We reuse the command buffer so that we can produce long packets without
    // making the stack large.
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        entry->command = LISTDIR_ENTRY;
        entry->status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, response_size, NULL, 0);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }

    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)command->path;
    // -1 because fatfs doesn't want a trailing /
    path[command->path_length - 1] = '\0';
    // mp_printf(&mp_plat_print, "list %s\n", path);
    FF_DIR dir;
    FRESULT res = f_opendir(fs, &dir, path);

    entry->command = LISTDIR_ENTRY;
    entry->status = STATUS_OK;
    entry->path_length = 0;
    entry->entry_number = 0;
    entry->entry_count = 0;
    entry->flags = 0;

    if (res != FR_OK) {
        entry->status = STATUS_ERROR_NO_FILE;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, response_size, NULL, 0);
        return ANY_COMMAND;
    }
    FILINFO file_info;
    res = f_readdir(&dir, &file_info);
    char *fn = file_info.fname;
    size_t total_entries = 0;
    while (res == FR_OK && fn[0] != 0) {
        res = f_readdir(&dir, &file_info);
        total_entries += 1;
    }
    // Rewind the directory.
    f_readdir(&dir, NULL);
    entry->entry_count = total_entries;
    for (size_t i = 0; i < total_entries; i++) {
        res = f_readdir(&dir, &file_info);
        entry->entry_number = i;
        if ((file_info.fattrib & AM_DIR) != 0) {
            entry->flags = 1; // Directory
            entry->file_size = 0;
        } else {
            entry->flags = 0;
            entry->file_size = file_info.fsize;
        }

        size_t name_length = strlen(file_info.fname);
        entry->path_length = name_length;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, response_size, NULL, 0);
        size_t fn_offset = 0;
        while (fn_offset < name_length) {
            size_t fn_size = MIN(name_length - fn_offset, 4);
            common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, ((uint8_t *)file_info.fname) + fn_offset, fn_size, NULL, 0);
            fn_offset += fn_size;
        }
    }
    f_closedir(&dir);
    entry->path_length = 0;
    entry->entry_number = entry->entry_count;
    entry->flags = 0;
    entry->file_size = 0;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, response_size, NULL, 0);
    return ANY_COMMAND;
}

// Background state that must live across background calls. After the _process
// helpers to force them to not use them.
uint8_t current_command[COMMAND_SIZE] __attribute__ ((aligned(4)));
volatile size_t current_offset;
uint8_t next_command;
bool was_connected;
void supervisor_bluetooth_background(void) {
    bool is_connected = common_hal_bleio_adapter_get_connected(&common_hal_bleio_adapter_obj);
    if (was_connected && !is_connected) {
        f_close(&active_file);
    }
    was_connected = is_connected;
    if (!is_connected) {
        next_command = 0;
        supervisor_bluetooth_start_advertising();
        return;
    }

    mp_int_t size = 1;
    while (size > 0) {
        size = common_hal_bleio_packet_buffer_readinto(&_transfer_packet_buffer, current_command + current_offset, COMMAND_SIZE - current_offset);

        if (size == 0) {
            break;
        }
        // TODO: If size < 0 return an error.
        current_offset += size;
        // mp_printf(&mp_plat_print, "buffer[:%d]:", current_offset);
        // for (size_t i = 0; i < current_offset; i++) {
        //     mp_printf(&mp_plat_print, " (%x %c)", current_command[i], current_command[i]);
        // }
        // mp_printf(&mp_plat_print, "\n");
        uint8_t current_state = current_command[0];
        // mp_printf(&mp_plat_print, "current command 0x%02x\n", current_state);
        // Check for protocol error.
        if (next_command != ANY_COMMAND && next_command != THIS_COMMAND && ((current_state & 0xf) != 0) && current_state != next_command) {
            uint8_t response[2];
            response[0] = next_command;
            response[1] = STATUS_ERROR_PROTOCOL;
            common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, response, 2, NULL, 0);
            break;
        }
        switch (current_state) {
            case READ:
                next_command = _process_read(current_command, current_offset);
                break;
            case READ_PACING:
                next_command = _process_read_pacing(current_command, current_offset);
                break;
            case WRITE:
                next_command = _process_write(current_command, current_offset);
                break;
            case WRITE_DATA:
                next_command = _process_write_data(current_command, current_offset);
                break;
            case DELETE:
                next_command = _process_delete(current_command, current_offset);
                break;
            case MKDIR:
                next_command = _process_mkdir(current_command, current_offset);
                break;
            case LISTDIR:
                next_command = _process_listdir(current_command, current_offset);
                break;
        }
        // Preserve the offset if we are waiting for more from this command.
        if (next_command != THIS_COMMAND) {
            current_offset = 0;
        }
    }
}

#endif // #else
