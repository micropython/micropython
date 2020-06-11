/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_DFU_H
#define MICROPY_INCLUDED_STM32_MBOOT_DFU_H

#include <stdint.h>

// DFU spec: https://www.usb.org/sites/default/files/DFU_1.1.pdf

#define DFU_XFER_SIZE (2048)

// The DFU standard allows for error messages to be sent (via str index) in GETSTATUS responses
#define MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER_IDX 0x10
#define MBOOT_ERROR_STR_OVERWRITE_BOOTLOADER "Can't overwrite mboot"

#define MBOOT_ERROR_STR_INVALID_ADDRESS_IDX 0x11
#define MBOOT_ERROR_STR_INVALID_ADDRESS "Address out of range"

// DFU class requests
enum {
    DFU_DETACH = 0,
    DFU_DNLOAD = 1,
    DFU_UPLOAD = 2,
    DFU_GETSTATUS = 3,
    DFU_CLRSTATUS = 4,
    DFU_GETSTATE = 5,
    DFU_ABORT = 6,
};

// DFU States
typedef enum {
    DFU_STATE_IDLE = 2,
    DFU_STATE_BUSY = 4,
    DFU_STATE_DNLOAD_IDLE = 5,
    DFU_STATE_MANIFEST = 7,
    DFU_STATE_UPLOAD_IDLE = 9,
    DFU_STATE_ERROR = 0xa,
} dfu_state_t;

typedef enum {
    DFU_CMD_NONE = 0,
    DFU_CMD_EXIT = 1,
    DFU_CMD_UPLOAD = 7,
    DFU_CMD_DNLOAD = 8,
} dfu_cmd_t;

// Error status flags
typedef enum {
    DFU_STATUS_OK = 0x00,  // No error condition is present.
    DFU_STATUS_ERROR_TARGET = 0x01,  // File is not targeted for use by this device.
    DFU_STATUS_ERROR_FILE = 0x02,  // File is for this device but fails some vendor-specific verification test.
    DFU_STATUS_ERROR_WRITE = 0x03,  // Device is unable to write memory.
    DFU_STATUS_ERROR_ERASE = 0x04,  // Memory erase function failed.
    DFU_STATUS_ERROR_CHECK_ERASED = 0x05,  // Memory erase check failed.
    DFU_STATUS_ERROR_PROG = 0x06,  // Program memory function failed.
    DFU_STATUS_ERROR_VERIFY = 0x07,  // Programmed memory failed verification.
    DFU_STATUS_ERROR_ADDRESS = 0x08,  // Cannot program memory due to received address that is out of range.
    DFU_STATUS_ERROR_NOTDONE = 0x09,  // Received DFU_DNLOAD with wLength = 0, but device does not think it has all of the data yet.
    DFU_STATUS_ERROR_FIRMWARE = 0x0A,  // Device's firmware is corrupt. It cannot return to run-time (non-DFU) operations.
    DFU_STATUS_ERROR_VENDOR = 0x0B,  // iString indicates a vendor-specific error.
    DFU_STATUS_ERROR_USBR = 0x0C,  // Device detected unexpected USB reset signaling.
    DFU_STATUS_ERROR_POR = 0x0D,  // Device detected unexpected power on reset.
    DFU_STATUS_ERROR_UNKNOWN = 0x0E,  // Something went wrong, but the device does not know what it was.
    DFU_STATUS_ERROR_STALLEDPKT = 0x0F,  // Device stalled an unexpected request.
} dfu_status_t;

typedef struct _dfu_state_t {
    dfu_state_t state;
    dfu_cmd_t cmd;
    dfu_status_t status;
    uint8_t error;
    uint16_t wBlockNum;
    uint16_t wLength;
    uint32_t addr;
    uint8_t buf[DFU_XFER_SIZE] __attribute__((aligned(4)));
} dfu_context_t;

static dfu_context_t dfu_context SECTION_NOZERO_BSS;

#endif // MICROPY_INCLUDED_STM32_MBOOT_DFU_H
