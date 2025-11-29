/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2025 Andrew Leech
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

#include "py/mpconfig.h"

#if MICROPY_HW_USB_MSC && MICROPY_HW_TINYUSB_STACK

#include "py/misc.h"
#include "tusb.h"
#include "storage.h"
#include "sdcard.h"
#include "usb.h"

// Storage medium selection (shared with main.c for compatibility)
pyb_usb_storage_medium_t pyb_usb_storage_medium = PYB_USB_STORAGE_MEDIUM_NONE;

// Ejection state
static bool msc_ejected = false;

// Invoked on SCSI_CMD_INQUIRY.
// Fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively.
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    (void)lun;
    memcpy(vendor_id, MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING), 8));
    memcpy(product_id, MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING), 16));
    memcpy(product_rev, MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING), 4));
}

// Invoked on Test-Unit-Ready command.
// Return true allowing host to read/write this LUN (e.g., SD card inserted).
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    (void)lun;

    if (msc_ejected || pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_NONE) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
        return false;
    }

    #if MICROPY_HW_ENABLE_SDCARD
    if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_SDCARD) {
        if (!sdcard_is_present()) {
            tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
            return false;
        }
    }
    #endif

    return true;
}

// Invoked on SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size.
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
    (void)lun;

    if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_FLASH) {
        *block_size = storage_get_block_size();
        *block_count = storage_get_block_count();
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_SDCARD) {
        *block_size = SDCARD_BLOCK_SIZE;
        *block_count = sdcard_get_capacity_in_bytes() / (uint64_t)SDCARD_BLOCK_SIZE;
    #endif
    } else {
        *block_size = 0;
        *block_count = 0;
    }
}

// Invoked on Start-Stop-Unit command:
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    (void)lun;
    (void)power_condition;

    if (load_eject) {
        if (start) {
            // Load disk storage
            msc_ejected = false;
        } else {
            // Unload disk storage
            msc_ejected = true;
        }
    }
    return true;
}

// Callback invoked on READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of read bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
    (void)lun;
    (void)offset;

    if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_FLASH) {
        uint32_t block_count = bufsize / FLASH_BLOCK_SIZE;
        int ret = storage_read_blocks(buffer, lba, block_count);
        if (ret != 0) {
            return -1;
        }
        return block_count * FLASH_BLOCK_SIZE;
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_SDCARD) {
        uint32_t block_count = bufsize / SDCARD_BLOCK_SIZE;
        if (sdcard_read_blocks(buffer, lba, block_count) != 0) {
            return -1;
        }
        return block_count * SDCARD_BLOCK_SIZE;
    #endif
    }

    return -1;
}

// Callback invoked on WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes.
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
    (void)lun;
    (void)offset;

    if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_FLASH) {
        uint32_t block_count = bufsize / FLASH_BLOCK_SIZE;
        int ret = storage_write_blocks(buffer, lba, block_count);
        if (ret != 0) {
            return -1;
        }
        return block_count * FLASH_BLOCK_SIZE;
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_SDCARD) {
        uint32_t block_count = bufsize / SDCARD_BLOCK_SIZE;
        if (sdcard_write_blocks(buffer, lba, block_count) != 0) {
            return -1;
        }
        return block_count * SDCARD_BLOCK_SIZE;
    #endif
    }

    return -1;
}

// Callback invoked on a SCSI command that's not handled by TinyUSB.
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize) {
    (void)lun;
    (void)buffer;
    (void)bufsize;

    int32_t resplen = 0;
    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Sync the logical unit if needed.
            #if MICROPY_HW_ENABLE_STORAGE
            if (pyb_usb_storage_medium == PYB_USB_STORAGE_MEDIUM_FLASH) {
                storage_flush();
            }
            #endif
            break;

        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }
    return resplen;
}

#endif // MICROPY_HW_USB_MSC && MICROPY_HW_TINYUSB_STACK
