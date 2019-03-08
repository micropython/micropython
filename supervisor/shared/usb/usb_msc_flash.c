/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "tusb.h"
// // #include "supervisor/flash.h"

// For updating fatfs's cache
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/diskio.h"
#include "lib/oofatfs/ff.h"
#include "py/mpstate.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/autoreload.h"

#define MSC_FLASH_BLOCK_SIZE    512

static bool ejected[1];

// The root FS is always at the end of the list.
static fs_user_mount_t* get_vfs(int lun) {
    // TODO(tannewt): Return the mount which matches the lun where 0 is the end
    // and is counted in reverse.
    if (lun > 0) {
        return NULL;
    }
    mp_vfs_mount_t* current_mount = MP_STATE_VM(vfs_mount_table);
    if (current_mount == NULL) {
        return NULL;
    }
    while (current_mount->next != NULL) {
        current_mount = current_mount->next;
    }
    return current_mount->obj;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 have their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, const uint8_t scsi_cmd[16], void* buffer, uint16_t bufsize) {
    const void* response = NULL;
    int32_t resplen = 0;

    switch ( scsi_cmd[0] ) {
        case SCSI_CMD_TEST_UNIT_READY:
            // Command that host uses to check our readiness before sending other commands
            resplen = 0;
            if (lun > 1) {
                resplen = -1;
            } else {
                fs_user_mount_t* current_mount = get_vfs(lun);
                if (current_mount == NULL) {
                    resplen = -1;
                }
                if (ejected[lun]) {
                    resplen = -1;
                }
            }
        break;

        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Host is about to read/write etc ... better not to disconnect disk
            resplen = 0;
        break;

        case SCSI_CMD_START_STOP_UNIT:
        {
            // Host try to eject/safe remove/poweroff us. We could safely disconnect with disk storage, or go into lower power
            const scsi_start_stop_unit_t* start_stop = (const scsi_start_stop_unit_t*) scsi_cmd;
            // Start bit = 0 : low power mode, if load_eject = 1 : unmount disk storage as well
            // Start bit = 1 : Ready mode, if load_eject = 1 : mount disk storage
            resplen = 0;
            if (start_stop->load_eject == 1) {
                if (lun > 1) {
                    resplen = -1;
                } else {
                    fs_user_mount_t* current_mount = get_vfs(lun);
                    if (current_mount == NULL) {
                        resplen = -1;
                    }
                    if (disk_ioctl(current_mount, CTRL_SYNC, NULL) != RES_OK) {
                        resplen = -1;
                    } else {
                        ejected[lun] = true;
                    }
                }
            }
        }
        break;

        default:
          // Set Sense = Invalid Command Operation
          tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

          // negative means error -> tinyusb could stall and/or response with failed status
          resplen = -1;
        break;
    }

    // return len must not larger than bufsize
    if ( resplen > bufsize ) {
        resplen = bufsize;
    }

    // copy response to stack's buffer if any
    if ( response && resplen ) {
        memcpy(buffer, response, resplen);
    }

    return resplen;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
    fs_user_mount_t * vfs = get_vfs(lun);
    disk_ioctl(vfs, GET_SECTOR_COUNT, block_count);
    disk_ioctl(vfs, GET_SECTOR_SIZE, block_size);
}

bool tud_msc_is_writable_cb(uint8_t lun) {
    if (lun > 1) {
        return false;
    }

    fs_user_mount_t* vfs = get_vfs(lun);
    if (vfs == NULL) {
        return false;
    }
    if (vfs->writeblocks[0] == MP_OBJ_NULL || !filesystem_is_writable_by_usb(vfs)) {
        return false;
    }
    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb (uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
    (void) lun;
    (void) offset;

    const uint32_t block_count = bufsize / MSC_FLASH_BLOCK_SIZE;

    fs_user_mount_t * vfs = get_vfs(lun);
    disk_read(vfs, buffer, lba, block_count);

    return block_count * MSC_FLASH_BLOCK_SIZE;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb (uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    (void) lun;
    (void) offset;

    const uint32_t block_count = bufsize / MSC_FLASH_BLOCK_SIZE;

    fs_user_mount_t * vfs = get_vfs(lun);
    disk_write(vfs, buffer, lba, block_count);
    // Since by getting here we assume the mount is read-only to
    // MicroPython let's update the cached FatFs sector if it's the one
    // we just wrote.
    #if _MAX_SS != _MIN_SS
    if (vfs->ssize == MSC_FLASH_BLOCK_SIZE) {
    #else
    // The compiler can optimize this away.
    if (_MAX_SS == FILESYSTEM_BLOCK_SIZE) {
    #endif
        if (lba == vfs->fatfs.winsect && lba > 0) {
            memcpy(vfs->fatfs.win,
                   buffer + MSC_FLASH_BLOCK_SIZE * (vfs->fatfs.winsect - lba),
                   MSC_FLASH_BLOCK_SIZE);
        }
    }

    return block_count * MSC_FLASH_BLOCK_SIZE;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void tud_msc_write10_complete_cb (uint8_t lun) {
    (void) lun;

    // This write is complete, start the autoreload clock.
    autoreload_start();
}
