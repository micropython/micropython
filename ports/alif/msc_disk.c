/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2024 OpenMV LLC.
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
#include "py/misc.h"
#include "ospi_flash.h"
#include "tusb.h"

#if CFG_TUD_MSC

#if MICROPY_FATFS_MAX_SS != MICROPY_HW_FLASH_BLOCK_SIZE_BYTES
#error MICROPY_FATFS_MAX_SS must be the same size as MICROPY_HW_FLASH_BLOCK_SIZE_BYTES
#endif

#define BLOCK_SIZE          (MICROPY_HW_FLASH_BLOCK_SIZE_BYTES)
#define BLOCK_COUNT         (MICROPY_HW_FLASH_STORAGE_FS_BYTES / BLOCK_SIZE)
#define FLASH_BASE_ADDR     (MICROPY_HW_FLASH_STORAGE_BASE_ADDR)

static bool ejected = false;

// Invoked on SCSI_CMD_INQUIRY.
// Fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively.
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    memcpy(vendor_id, MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING), 8));
    memcpy(product_id, MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING), 16));
    memcpy(product_rev, MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING), 4));
}

// Invoked on Test-Unit-Ready command.
// Return true allowing host to read/write this LUN (e.g SD card inserted).
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    if (ejected) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
        return false;
    }
    return true;
}

// Invoked on SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size.
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
    *block_size = BLOCK_SIZE;
    *block_count = BLOCK_COUNT;
}

// Invoked on Start-Stop-Unit command:
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    if (load_eject) {
        if (start) {
            // load disk storage
            ejected = false;
        } else {
            // unload disk storage
            ejected = true;
        }
    }
    return true;
}

// Callback invoked on READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of read bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
    uint32_t addr = FLASH_BASE_ADDR + lba * BLOCK_SIZE;
    uint32_t block_count = bufsize / BLOCK_SIZE;
    int ret = ospi_flash_read(addr, block_count * BLOCK_SIZE, buffer);
    if (ret < 0) {
        return ret;
    }
    return block_count * BLOCK_SIZE;
}

// Callback invoked on WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes.
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
    if (bufsize < BLOCK_SIZE) {
        // Workaround for issue with TinyUSB passing in a small buffer.
        uint32_t addr = FLASH_BASE_ADDR + lba * BLOCK_SIZE + offset;
        if (offset == 0) {
            int ret = ospi_flash_erase_sector(addr);
            if (ret < 0) {
                return ret;
            }
        }
        int ret = ospi_flash_write(addr, bufsize, buffer);
        if (ret < 0) {
            return ret;
        }
        return bufsize;
    }

    uint32_t addr = FLASH_BASE_ADDR + lba * BLOCK_SIZE;
    uint32_t block_count = bufsize / BLOCK_SIZE;
    for (uint32_t block = 0; block < block_count; ++block) {
        int ret = ospi_flash_erase_sector(addr);
        if (ret < 0) {
            return ret;
        }
        ret = ospi_flash_write(addr, BLOCK_SIZE, buffer);
        if (ret < 0) {
            return ret;
        }
        addr += BLOCK_SIZE;
        buffer += BLOCK_SIZE;
    }
    return block_count * BLOCK_SIZE;
}

// Callback invoked on a SCSI command that's not handled by TinyUSB.
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize) {
    int32_t resplen = 0;
    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Sync the logical unit if needed.
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

#endif
