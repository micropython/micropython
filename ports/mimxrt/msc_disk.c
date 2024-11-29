/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2024-2025 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
 *
 */
#include "tusb.h"
#if CFG_TUD_MSC
#include "flash.h"
#include BOARD_FLASH_OPS_HEADER_H
#include "stdlib.h"
#include "modmimxrt.h"
#if MICROPY_PY_MACHINE_SDCARD
#include "sdcard.h"

#ifndef MICROPY_HW_SDCARD_SDMMC
#define MICROPY_HW_SDCARD_SDMMC (1)
#endif

#define MSC_SDCARD_INDEX    (MICROPY_HW_SDCARD_SDMMC - 1)
#endif

// This implementation does Not support Flash sector caching.
// MICROPY_FATFS_MAX_SS  must be identical to SECTOR_SIZE_BYTES
#define BLOCK_SIZE          (SECTOR_SIZE_BYTES)
#define BLOCK_COUNT         (MICROPY_HW_FLASH_STORAGE_BYTES / BLOCK_SIZE)
#define FLASH_BASE_ADDR     (MICROPY_HW_FLASH_STORAGE_BASE)

static bool msc_ejected = false;

const mp_obj_type_t *mimxrt_msc_medium = NULL;

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    memcpy(vendor_id, MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING), 8));
    memcpy(product_id, MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING), 16));
    memcpy(product_rev, MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING, MIN(strlen(MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING), 4));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    if (msc_ejected || mimxrt_msc_medium == NULL) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
        return false;
    }
    return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
    if (mimxrt_msc_medium == &mimxrt_flash_type) {
        *block_size = BLOCK_SIZE;
        *block_count = BLOCK_COUNT;
    #if MICROPY_PY_MACHINE_SDCARD
    } else if (mimxrt_msc_medium == &machine_sdcard_type) {
        mimxrt_sdcard_obj_t *card = &mimxrt_sdcard_objs[MSC_SDCARD_INDEX];
        *block_size = card->block_len;
        *block_count = card->block_count;
    #endif
    }
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    if (load_eject) {
        if (start) {
            // load disk storage
            msc_ejected = false;
        } else {
            // unload disk storage
            msc_ejected = true;
        }
    }
    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
    if (mimxrt_msc_medium == &mimxrt_flash_type) {
        flash_read_block(FLASH_BASE_ADDR + lba * BLOCK_SIZE, buffer, bufsize);
    #if MICROPY_PY_MACHINE_SDCARD
    } else if (mimxrt_msc_medium == &machine_sdcard_type) {
        mimxrt_sdcard_obj_t *card = &mimxrt_sdcard_objs[MSC_SDCARD_INDEX];
        sdcard_read(card, buffer, lba, bufsize / card->block_len);
    #endif
    }
    return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
    if (mimxrt_msc_medium == &mimxrt_flash_type) {
        // Erase count sectors starting at lba
        for (int n = 0; n < (bufsize / BLOCK_SIZE); n++) {
            flash_erase_sector(FLASH_BASE_ADDR + (lba + n) * BLOCK_SIZE);
        }
        flash_write_block(FLASH_BASE_ADDR + lba * BLOCK_SIZE, buffer, bufsize);
    #if MICROPY_PY_MACHINE_SDCARD
    } else if (mimxrt_msc_medium == &machine_sdcard_type) {
        mimxrt_sdcard_obj_t *card = &mimxrt_sdcard_objs[MSC_SDCARD_INDEX];
        sdcard_write(card, buffer, lba, bufsize / card->block_len);

    #endif
    }
    return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
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
