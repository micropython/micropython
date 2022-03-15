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

#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "shared/timeutils/timeutils.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-module/storage/__init__.h"

#include "bluetooth/ble_drv.h"

#include "common-hal/_bleio/__init__.h"

#include "supervisor/fatfs_port.h"
#include "supervisor/shared/reload.h"
#include "supervisor/shared/bluetooth/file_transfer.h"
#include "supervisor/shared/bluetooth/file_transfer_protocol.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include "py/mpstate.h"
#include "py/stackctrl.h"

STATIC bleio_service_obj_t supervisor_ble_service;
STATIC bleio_uuid_obj_t supervisor_ble_service_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_version_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_version_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_transfer_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_transfer_uuid;

// This is the base UUID for the file transfer service.
const uint8_t file_transfer_base_uuid[16] = {0x72, 0x65, 0x66, 0x73, 0x6e, 0x61, 0x72, 0x54, 0x65, 0x6c, 0x69, 0x46, 0x00, 0x00, 0xaf, 0xad };

STATIC mp_obj_list_t characteristic_list;
STATIC mp_obj_t characteristic_list_items[2];
// 2 * 10 ringbuf packets, 512 for a disk sector and 12 for the file transfer write header.
#define PACKET_BUFFER_SIZE (2 * 10 + 512 + 12)
// uint32_t so its aligned
STATIC uint32_t _buffer[PACKET_BUFFER_SIZE / 4 + 1];
STATIC uint32_t _outgoing1[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC uint32_t _outgoing2[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC ble_drv_evt_handler_entry_t static_handler_entry;
STATIC bleio_packet_buffer_obj_t _transfer_packet_buffer;

void supervisor_start_bluetooth_file_transfer(void) {
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
        NULL,                                       // no initial value
        NULL); // no description

    uint32_t version = 4;
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
        NULL,                                       // no initial valuen
        NULL);

    _common_hal_bleio_packet_buffer_construct(
        &_transfer_packet_buffer, &supervisor_ble_transfer_characteristic,
        _buffer, PACKET_BUFFER_SIZE,
        _outgoing1, _outgoing2, BLE_GATTS_VAR_ATTR_LEN_MAX,
        &static_handler_entry);
}

#define COMMAND_SIZE 1024

#define ANY_COMMAND 0x00
#define THIS_COMMAND 0x01

// FATFS has a two second timestamp resolution but the BLE API allows for nanosecond resolution.
// This function truncates the time the time to a resolution storable by FATFS and fills in the
// FATFS encoded version into fattime.
STATIC uint64_t truncate_time(uint64_t input_time, DWORD *fattime) {
    timeutils_struct_time_t tm;
    uint64_t seconds_since_epoch = timeutils_seconds_since_epoch_from_nanoseconds_since_1970(input_time);
    timeutils_seconds_since_epoch_to_struct_time(seconds_since_epoch, &tm);
    uint64_t truncated_time = timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970((seconds_since_epoch / 2) * 2 * 1000000000);

    *fattime = ((tm.tm_year - 1980) << 25) | (tm.tm_mon << 21) | (tm.tm_mday << 16) |
        (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
    return truncated_time;
}

// Used by read and write.
STATIC FIL active_file;
STATIC uint8_t _process_read(const uint8_t *raw_buf, size_t command_len) {
    struct read_command *command = (struct read_command *)raw_buf;
    size_t header_size = sizeof(struct read_command);
    size_t response_size = sizeof(struct read_data);
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

STATIC uint8_t _process_read_pacing(const uint8_t *raw_buf, size_t command_len) {
    struct read_pacing *command = (struct read_pacing *)raw_buf;
    struct read_data response;
    response.command = READ_DATA;
    response.status = STATUS_OK;
    size_t response_size = sizeof(struct read_data);

    uint32_t total_length = f_size(&active_file);
    // Write out the response header.
    uint32_t chunk_size = MIN(command->chunk_size, total_length - command->chunk_offset);
    response.chunk_offset = command->chunk_offset;
    response.total_length = total_length;
    response.data_size = chunk_size;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, response_size, NULL, 0);
    f_lseek(&active_file, command->chunk_offset);
    // Write out the chunk contents. We can do this in small pieces because PacketBuffer
    // will assemble them into larger packets of its own.
    size_t chunk_offset = 0;
    uint8_t data[20];
    while (chunk_offset < chunk_size) {
        size_t quantity_read;
        size_t read_size = MIN(chunk_size - chunk_offset, sizeof(data));
        FRESULT result = f_read(&active_file, &data, read_size, &quantity_read);
        if (quantity_read == 0 || result != FR_OK) {
            // TODO: If we can't read everything, then the file must have been shortened. Maybe we
            // should return 0s to pad it out.
            break;
        }
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&data, quantity_read, NULL, 0);
        chunk_offset += quantity_read;
    }
    if ((chunk_offset + chunk_size) >= total_length) {
        f_close(&active_file);
        return ANY_COMMAND;
    }
    return READ_PACING;
}

// Used by write and write data to know when the write is complete.
STATIC size_t total_write_length;
STATIC uint64_t _truncated_time;

// Returns true if usb is active and replies with an error if so. If not, it grabs
// the USB mass storage lock and returns false. Make sure to release the lock with
// usb_msc_unlock() when the transaction is complete.
STATIC bool _usb_active(void *response, size_t response_size) {
    // Check to see if USB has already been mounted. If not, then we "eject" from USB until we're done.
    #if CIRCUITPY_USB && CIRCUITPY_USB_MSC
    if (storage_usb_enabled() && !usb_msc_lock()) {
        // Status is always the second byte of the response.
        ((uint8_t *)response)[1] = STATUS_ERROR_READONLY;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)response, response_size, NULL, 0);
        return true;
    }
    #endif
    return false;
}

STATIC uint8_t _process_write(const uint8_t *raw_buf, size_t command_len) {
    struct write_command *command = (struct write_command *)raw_buf;
    size_t header_size = sizeof(struct write_command);
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
    if (_usb_active(&response, sizeof(struct write_pacing))) {
        return ANY_COMMAND;
    }

    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    DWORD fattime;
    _truncated_time = truncate_time(command->modification_time, &fattime);
    override_fattime(fattime);
    FRESULT result = f_open(fs, &active_file, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        override_fattime(0);
        return ANY_COMMAND;
    }
    // Write out the pacing response.

    // Align the next chunk to a sector boundary.
    uint32_t offset = command->offset;
    size_t chunk_size = MIN(total_write_length - offset, 512 - (offset % 512));
    // Special case when truncating the file. (Deleting stuff off the end.)
    if (chunk_size == 0) {
        f_lseek(&active_file, offset);
        f_truncate(&active_file);
        f_close(&active_file);
        override_fattime(0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
    }
    response.offset = offset;
    response.free_space = chunk_size;
    response.truncated_time = _truncated_time;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
    if (chunk_size == 0) {
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        autoreload_trigger();
        return ANY_COMMAND;
    }

    return WRITE_DATA;
}

STATIC uint8_t _process_write_data(const uint8_t *raw_buf, size_t command_len) {
    struct write_data *command = (struct write_data *)raw_buf;
    size_t header_size = sizeof(struct write_data);
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
        override_fattime(0);
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
        override_fattime(0);
        return ANY_COMMAND;
    }
    offset += command->data_size;
    // Align the next chunk to a sector boundary.
    size_t chunk_size = MIN(total_write_length - offset, 512);
    response.offset = offset;
    response.free_space = chunk_size;
    response.truncated_time = _truncated_time;
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct write_pacing), NULL, 0);
    if (total_write_length == offset) {
        f_truncate(&active_file);
        f_close(&active_file);
        override_fattime(0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        autoreload_trigger();
        return ANY_COMMAND;
    }
    return WRITE_DATA;
}

STATIC FRESULT _delete_directory_contents(FATFS *fs, const TCHAR *path) {
    FF_DIR dir;
    FRESULT res = f_opendir(fs, &dir, path);
    FILINFO file_info;
    // Check the stack since we're putting paths on it.
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        return FR_INT_ERR;
    }
    while (res == FR_OK) {
        res = f_readdir(&dir, &file_info);
        if (res != FR_OK || file_info.fname[0] == '\0') {
            break;
        }
        size_t pathlen = strlen(path);
        size_t fnlen = strlen(file_info.fname);
        TCHAR full_path[pathlen + 1 + fnlen];
        memcpy(full_path, path, pathlen);
        full_path[pathlen] = '/';
        size_t full_pathlen = pathlen + 1 + fnlen;
        memcpy(full_path + pathlen + 1, file_info.fname, fnlen);
        full_path[full_pathlen] = '\0';
        if ((file_info.fattrib & AM_DIR) != 0) {
            res = _delete_directory_contents(fs, full_path);
        }
        if (res != FR_OK) {
            break;
        }
        res = f_unlink(fs, full_path);
    }
    f_closedir(&dir);
    return res;
}

STATIC uint8_t _process_delete(const uint8_t *raw_buf, size_t command_len) {
    const struct delete_command *command = (struct delete_command *)raw_buf;
    size_t header_size = sizeof(struct delete_command);
    struct delete_status response;
    response.command = DELETE_STATUS;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct delete_status), NULL, 0);
        return ANY_COMMAND;
    }
    if (_usb_active(&response, sizeof(struct delete_status))) {
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)((uint8_t *)command) + header_size;
    path[command->path_length] = '\0';
    FILINFO file;
    FRESULT result = f_stat(fs, path, &file);
    if (result == FR_OK) {
        if ((file.fattrib & AM_DIR) != 0) {
            result = _delete_directory_contents(fs, path);
        }
        if (result == FR_OK) {
            result = f_unlink(fs, path);
        }
    }
    #if CIRCUITPY_USB_MSC
    usb_msc_unlock();
    #endif
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
    }
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct delete_status), NULL, 0);
    if (result == FR_OK) {
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        autoreload_trigger();
    }
    return ANY_COMMAND;
}

// NULL-terminate the path and remove any trailing /. Older versions of the
// protocol require it but newer ones do not.
STATIC void _terminate_path(char *path, size_t path_length) {
    // -1 because fatfs doesn't want a trailing /
    if (path[path_length - 1] == '/') {
        path[path_length - 1] = '\0';
    } else {
        path[path_length] = '\0';
    }
}

STATIC uint8_t _process_mkdir(const uint8_t *raw_buf, size_t command_len) {
    const struct mkdir_command *command = (struct mkdir_command *)raw_buf;
    size_t header_size = sizeof(struct mkdir_command);
    struct mkdir_status response;
    response.command = MKDIR_STATUS;
    response.status = STATUS_OK;
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct mkdir_status), NULL, 0);
        return ANY_COMMAND;
    }
    if (_usb_active(&response, sizeof(struct mkdir_status))) {
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)command->path;
    _terminate_path(path, command->path_length);

    DWORD fattime;
    response.truncated_time = truncate_time(command->modification_time, &fattime);
    override_fattime(fattime);
    FRESULT result = f_mkdir(fs, path);
    override_fattime(0);
    #if CIRCUITPY_USB_MSC
    usb_msc_unlock();
    #endif
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
    }
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct mkdir_status), NULL, 0);
    if (result == FR_OK) {
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        autoreload_trigger();
    }
    return ANY_COMMAND;
}

STATIC void send_listdir_entry_header(const struct listdir_entry *entry, mp_int_t max_packet_size) {
    mp_int_t response_size = sizeof(struct listdir_entry);
    if (max_packet_size >= response_size) {
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, response_size, NULL, 0);
        return;
    }
    // Split into 16 + 12 size packets to fit into 20 byte minimum packet size.
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)entry, 16, NULL, 0);
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, ((const uint8_t *)entry) + 16, response_size - 16, NULL, 0);
}

STATIC uint8_t _process_listdir(uint8_t *raw_buf, size_t command_len) {
    const struct listdir_command *command = (struct listdir_command *)raw_buf;
    struct listdir_entry *entry = (struct listdir_entry *)raw_buf;
    size_t header_size = sizeof(struct listdir_command);
    mp_int_t max_packet_size = common_hal_bleio_packet_buffer_get_outgoing_packet_length(&_transfer_packet_buffer);
    if (max_packet_size < 0) {
        // -1 means we're disconnected
        return ANY_COMMAND;
    }
    // We reuse the command buffer so that we can produce long packets without
    // making the stack large.
    if (command->path_length > (COMMAND_SIZE - header_size - 1)) { // -1 for the null we'll write
        // TODO: throw away any more packets of path.
        entry->command = LISTDIR_ENTRY;
        entry->status = STATUS_ERROR;
        send_listdir_entry_header(entry, max_packet_size);
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + command->path_length) {
        return THIS_COMMAND;
    }

    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *path = (char *)&command->path;
    _terminate_path(path, command->path_length);
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
        send_listdir_entry_header(entry, max_packet_size);
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
        uint64_t truncated_time = timeutils_mktime(1980 + (file_info.fdate >> 9),
            (file_info.fdate >> 5) & 0xf,
            file_info.fdate & 0x1f,
            file_info.ftime >> 11,
            (file_info.ftime >> 5) & 0x1f,
            (file_info.ftime & 0x1f) * 2) * 1000000000ULL;
        entry->truncated_time = truncated_time;
        if ((file_info.fattrib & AM_DIR) != 0) {
            entry->flags = 1; // Directory
            entry->file_size = 0;
        } else {
            entry->flags = 0;
            entry->file_size = file_info.fsize;
        }

        size_t name_length = strlen(file_info.fname);
        entry->path_length = name_length;
        send_listdir_entry_header(entry, max_packet_size);
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
    send_listdir_entry_header(entry, max_packet_size);
    return ANY_COMMAND;
}

STATIC uint8_t _process_move(const uint8_t *raw_buf, size_t command_len) {
    const struct move_command *command = (struct move_command *)raw_buf;
    size_t header_size = sizeof(struct move_command);
    struct move_status response;
    response.command = MOVE_STATUS;
    response.status = STATUS_OK;
    // +2 for null terminators.
    uint32_t total_path_length = command->old_path_length + command->new_path_length + 1;
    if (total_path_length > (COMMAND_SIZE - header_size - 1)) {
        // TODO: throw away any more packets of path.
        response.status = STATUS_ERROR;
        common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct move_status), NULL, 0);
        return ANY_COMMAND;
    }
    if (_usb_active(&response, sizeof(struct move_status))) {
        return ANY_COMMAND;
    }
    // We need to receive another packet to have the full path.
    if (command_len < header_size + total_path_length) {
        return THIS_COMMAND;
    }
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    char *old_path = (char *)command->paths;
    old_path[command->old_path_length] = '\0';

    char *new_path = old_path + command->old_path_length + 1;
    new_path[command->new_path_length] = '\0';

    // mp_printf(&mp_plat_print, "move %s to %s\n", old_path, new_path);

    FRESULT result = f_rename(fs, old_path, new_path);
    #if CIRCUITPY_USB_MSC
    usb_msc_unlock();
    #endif
    if (result != FR_OK) {
        response.status = STATUS_ERROR;
    }
    common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, (const uint8_t *)&response, sizeof(struct move_status), NULL, 0);
    if (result == FR_OK) {
        // Don't reload until everything is written out of the packet buffer.
        common_hal_bleio_packet_buffer_flush(&_transfer_packet_buffer);
        autoreload_trigger();
    }
    return ANY_COMMAND;
}

// Background state that must live across background calls. After the _process
// helpers to force them to not use them.
STATIC uint8_t current_command[COMMAND_SIZE] __attribute__ ((aligned(4)));
STATIC volatile size_t current_offset;
STATIC uint8_t next_command;
STATIC bool running = false;
void supervisor_bluetooth_file_transfer_background(void) {
    if (running) {
        return;
    }
    running = true;
    mp_int_t size = 1;
    while (size > 0) {
        size = common_hal_bleio_packet_buffer_readinto(&_transfer_packet_buffer, current_command + current_offset, COMMAND_SIZE - current_offset);

        if (size == 0) {
            break;
        }
        autoreload_suspend(AUTORELOAD_SUSPEND_BLE);
        // TODO: If size < 0 return an error.
        current_offset += size;
        #if CIRCUITPY_VERBOSE_BLE
        mp_printf(&mp_plat_print, "buffer[:%d]:", current_offset);
        for (size_t i = 0; i < current_offset; i++) {
            mp_printf(&mp_plat_print, " (%x %c)", current_command[i], current_command[i]);
        }
        mp_printf(&mp_plat_print, "\n");
        #endif
        uint8_t current_state = current_command[0];
        // mp_printf(&mp_plat_print, "current command 0x%02x\n", current_state);
        // Check for protocol error.
        if (next_command != ANY_COMMAND && next_command != THIS_COMMAND && ((current_state & 0xf) != 0) && current_state != next_command) {
            uint8_t response[2];
            response[0] = next_command;
            response[1] = STATUS_ERROR_PROTOCOL;
            common_hal_bleio_packet_buffer_write(&_transfer_packet_buffer, response, 2, NULL, 0);
            autoreload_resume(AUTORELOAD_SUSPEND_BLE);
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
            case MOVE:
                next_command = _process_move(current_command, current_offset);
                break;
        }
        // Preserve the offset if we are waiting for more from this command.
        if (next_command != THIS_COMMAND) {
            current_offset = 0;
        }
        if (next_command == ANY_COMMAND) {
            autoreload_resume(AUTORELOAD_SUSPEND_BLE);
        }
    }
    running = false;
}

void supervisor_bluetooth_file_transfer_disconnected(void) {
    next_command = ANY_COMMAND;
    current_offset = 0;
    f_close(&active_file);
    autoreload_resume(AUTORELOAD_SUSPEND_BLE);
}
