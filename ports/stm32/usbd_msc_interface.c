/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#include <stdint.h>

#include "usbd_cdc_msc_hid.h"
#include "usbd_msc_interface.h"

#include "extmod/vfs.h"
#include "storage.h"
#include "sdcard.h"

#if MICROPY_HW_USB_MSC

// This flag is needed to support removal of the medium, so that the USB drive
// can be unmounted and won't be remounted automatically.
#define FLAGS_STARTED (0x01)

#define FLAGS_READONLY (0x02)

STATIC const void *usbd_msc_lu_data[USBD_MSC_MAX_LUN];
STATIC uint8_t usbd_msc_lu_num;
STATIC uint16_t usbd_msc_lu_flags;

static inline void lu_flag_set(uint8_t lun, uint8_t flag) {
    usbd_msc_lu_flags |= flag << (lun * 2);
}

static inline void lu_flag_clr(uint8_t lun, uint8_t flag) {
    usbd_msc_lu_flags &= ~(flag << (lun * 2));
}

static inline bool lu_flag_is_set(uint8_t lun, uint8_t flag) {
    return usbd_msc_lu_flags & (flag << (lun * 2));
}

// Sent in response to MODE SENSE(6) command
const uint8_t USBD_MSC_Mode_Sense6_Data[4] = {
    0x03, // mode data length
    0x00, // medium type
    0x00, // bit 7: write protect
    0x00, // block descriptor length
};

// Sent in response to MODE SENSE(10) command
const uint8_t USBD_MSC_Mode_Sense10_Data[8] = {
    0x00, 0x06, // mode data length
    0x00, // medium type
    0x00, // bit 7: write protect
    0x00,
    0x00,
    0x00, 0x00, // block descriptor length
};

STATIC const uint8_t usbd_msc_vpd00[6] = {
    0x00, // peripheral qualifier; peripheral device type
    0x00, // page code
    0x00, // reserved
    2, // page length (additional bytes beyond this entry)
    0x00, // page 0x00 supported
    0x83, // page 0x83 supported
};

STATIC const uint8_t usbd_msc_vpd83[4] = {
    0x00, // peripheral qualifier; peripheral device type
    0x83, // page code
    0x00, 0x00, // page length (additional bytes beyond this entry)
};

STATIC const int8_t usbd_msc_inquiry_data[36] = {
    0x00, // peripheral qualifier; peripheral device type
    0x80, // 0x00 for a fixed drive, 0x80 for a removable drive
    0x02, // version
    0x02, // response data format
    (STANDARD_INQUIRY_DATA_LEN - 5), // additional length
    0x00, // various flags
    0x00, // various flags
    0x00, // various flags
    'M', 'i', 'c', 'r', 'o', 'P', 'y', ' ', // Manufacturer : 8 bytes
    'p', 'y', 'b', 'o', 'a', 'r', 'd', ' ', // Product      : 16 Bytes
    'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
    '1', '.', '0', '0',                     // Version      : 4 Bytes
};

// Set the logical units that will be exposed over MSC
void usbd_msc_init_lu(size_t lu_n, const void *lu_data) {
    usbd_msc_lu_num = MIN(lu_n, USBD_MSC_MAX_LUN);
    memcpy(usbd_msc_lu_data, lu_data, sizeof(void *) * usbd_msc_lu_num);
    usbd_msc_lu_flags = 0;
}

// Helper function to perform an ioctl on a logical unit
STATIC int lu_ioctl(uint8_t lun, int op, uint32_t *data) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    const void *lu = usbd_msc_lu_data[lun];

    if (lu == &pyb_flash_type) {
        switch (op) {
            case MP_BLOCKDEV_IOCTL_INIT:
                storage_init();
                *data = 0;
                return 0;
            case MP_BLOCKDEV_IOCTL_SYNC:
                storage_flush();
                return 0;
            case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
                *data = storage_get_block_size();
                return 0;
            case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
                *data = storage_get_block_count();
                return 0;
            default:
                return -1;
        }
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (lu == &pyb_sdcard_type
               #if MICROPY_HW_ENABLE_MMCARD
               || lu == &pyb_mmcard_type
               #endif
               ) {
        switch (op) {
            case MP_BLOCKDEV_IOCTL_INIT:
                if (!sdcard_power_on()) {
                    return -1;
                }
                *data = 0;
                return 0;
            case MP_BLOCKDEV_IOCTL_SYNC:
                return 0;
            case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
                *data = SDCARD_BLOCK_SIZE;
                return 0;
            case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
                *data = sdcard_get_capacity_in_bytes() / (uint64_t)SDCARD_BLOCK_SIZE;
                return 0;
            default:
                return -1;
        }
    #endif
    } else {
        return -1;
    }
}

// Initialise all logical units (it's only ever called once, with lun_in=0)
STATIC int8_t usbd_msc_Init(uint8_t lun_in) {
    if (lun_in != 0) {
        return 0;
    }
    for (int lun = 0; lun < usbd_msc_lu_num; ++lun) {
        uint32_t data = 0;
        int res = lu_ioctl(lun, MP_BLOCKDEV_IOCTL_INIT, &data);
        if (res != 0) {
            lu_flag_clr(lun, FLAGS_STARTED);
        } else {
            lu_flag_set(lun, FLAGS_STARTED);
            if (data) {
                lu_flag_set(lun, FLAGS_READONLY);
            }
        }
    }
    return 0;
}

// Process SCSI INQUIRY command for the logical unit
STATIC int usbd_msc_Inquiry(uint8_t lun, const uint8_t *params, uint8_t *data_out) {
    if (params[1] & 1) {
        // EVPD set - return vital product data parameters
        uint8_t page_code = params[2];
        switch (page_code) {
            case 0x00: // Supported VPD pages
                memcpy(data_out, usbd_msc_vpd00, sizeof(usbd_msc_vpd00));
                return sizeof(usbd_msc_vpd00);
            case 0x83: // Device identification
                memcpy(data_out, usbd_msc_vpd83, sizeof(usbd_msc_vpd83));
                return sizeof(usbd_msc_vpd83);
            default: // Unsupported
                return -1;
        }
    }

    // A standard inquiry

    if (lun >= usbd_msc_lu_num) {
        return -1;
    }

    uint8_t alloc_len = params[3] << 8 | params[4];
    int len = MIN(sizeof(usbd_msc_inquiry_data), alloc_len);
    memcpy(data_out, usbd_msc_inquiry_data, len);

    #if MICROPY_HW_ENABLE_SDCARD
    const void *lu = usbd_msc_lu_data[lun];
    if (len == sizeof(usbd_msc_inquiry_data)) {
        if (lu == &pyb_sdcard_type) {
            memcpy(data_out + 24, "SDCard", sizeof("SDCard") - 1);
        }
        #if MICROPY_HW_ENABLE_MMCARD
        else if (lu == &pyb_mmcard_type) {
            memcpy(data_out + 24, "MMCard", sizeof("MMCard") - 1);
        }
        #endif
    }
    #endif

    return len;
}

// Get storage capacity of a logical unit
STATIC int8_t usbd_msc_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
    uint32_t block_size_u32 = 0;
    int res = lu_ioctl(lun, MP_BLOCKDEV_IOCTL_BLOCK_SIZE, &block_size_u32);
    if (res != 0) {
        return -1;
    }
    *block_size = block_size_u32;
    return lu_ioctl(lun, MP_BLOCKDEV_IOCTL_BLOCK_COUNT, block_num);
}

// Check if a logical unit is ready
STATIC int8_t usbd_msc_IsReady(uint8_t lun) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    return lu_flag_is_set(lun, FLAGS_STARTED) ? 0 : -1;
}

// Check if a logical unit is write protected
STATIC int8_t usbd_msc_IsWriteProtected(uint8_t lun) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    return lu_flag_is_set(lun, FLAGS_READONLY) ? 1 : 0;
}

// Start or stop a logical unit
STATIC int8_t usbd_msc_StartStopUnit(uint8_t lun, uint8_t started) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    if (started) {
        lu_flag_set(lun, FLAGS_STARTED);
    } else {
        lu_flag_clr(lun, FLAGS_STARTED);
    }
    return 0;
}

// Prepare a logical unit for possible removal
STATIC int8_t usbd_msc_PreventAllowMediumRemoval(uint8_t lun, uint8_t param) {
    uint32_t dummy;
    // Sync the logical unit so the device can be unplugged/turned off
    return lu_ioctl(lun, MP_BLOCKDEV_IOCTL_SYNC, &dummy);
}

// Read data from a logical unit
STATIC int8_t usbd_msc_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    const void *lu = usbd_msc_lu_data[lun];

    if (lu == &pyb_flash_type) {
        storage_read_blocks(buf, blk_addr, blk_len);
        return 0;
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (lu == &pyb_sdcard_type
               #if MICROPY_HW_ENABLE_MMCARD
               || lu == &pyb_mmcard_type
               #endif
               ) {
        if (sdcard_read_blocks(buf, blk_addr, blk_len) == 0) {
            return 0;
        }
    #endif
    }
    return -1;
}

// Write data to a logical unit
STATIC int8_t usbd_msc_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (lun >= usbd_msc_lu_num) {
        return -1;
    }
    const void *lu = usbd_msc_lu_data[lun];

    if (lu == &pyb_flash_type) {
        storage_write_blocks(buf, blk_addr, blk_len);
        return 0;
    #if MICROPY_HW_ENABLE_SDCARD
    } else if (lu == &pyb_sdcard_type
               #if MICROPY_HW_ENABLE_MMCARD
               || lu == &pyb_mmcard_type
               #endif
               ) {
        if (sdcard_write_blocks(buf, blk_addr, blk_len) == 0) {
            return 0;
        }
    #endif
    }
    return -1;
}

// Get the number of attached logical units
STATIC int8_t usbd_msc_GetMaxLun(void) {
    return usbd_msc_lu_num - 1;
}

// Table of operations for the SCSI layer to call
const USBD_StorageTypeDef usbd_msc_fops = {
    usbd_msc_Init,
    usbd_msc_Inquiry,
    usbd_msc_GetCapacity,
    usbd_msc_IsReady,
    usbd_msc_IsWriteProtected,
    usbd_msc_StartStopUnit,
    usbd_msc_PreventAllowMediumRemoval,
    usbd_msc_Read,
    usbd_msc_Write,
    usbd_msc_GetMaxLun,
};

#endif // MICROPY_HW_USB_MSC
