/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#include "mboot_buffer.h"
#include "mboot_memory.h"
#include "mboot_upgrade.h"
#include "mboot_validate.h"
#include "flash.h"

// --------------------------------------------------------------------+
// Type Definitions
// --------------------------------------------------------------------+
typedef struct _upgrade_session_t
{
    // Available memory range for session
    uintptr_t mem_start_addr;
    uintptr_t mem_end_addr;

    // Current addresses of write and erase pointer
    uintptr_t write_addr;
    uintptr_t erase_addr;

    // Number of blocks received for sessions
    uint32_t n_blocks;

    // Number of bytes written to flash by session
    uint32_t n_bytes_written;

    // Session block download function
    mboot_upgrade_status_t (*write_data_handler)();

    // Session status
    status_t status;
    mboot_upgrade_status_t upgrade_status;
    mboot_validate_status_t verification_status;
} upgrade_session_t;

// --------------------------------------------------------------------+
// Local Variables
// --------------------------------------------------------------------+
static fw_header_t image_header = {};
static upgrade_session_t session;

// --------------------------------------------------------------------+
// External Variables
// --------------------------------------------------------------------+
extern uintptr_t __firmware_start;
extern uintptr_t __firmware_end;
extern uintptr_t __flash_start;

// --------------------------------------------------------------------+
// Local Function Declarations
// --------------------------------------------------------------------+
static mboot_upgrade_status_t write_initial_block_handler();
static mboot_upgrade_status_t write_block_handler();
static mboot_upgrade_status_t write_block(uint8_t *data, uint32_t length, uintptr_t *eaddr, uintptr_t *waddr);

// --------------------------------------------------------------------+
// Global Function Definitions
// --------------------------------------------------------------------+

// Todo: Add configuration structure
mboot_upgrade_status_t mboot_upgrade_init() {
    flash_init();

    // Initialize firmware upgrade session
    session.n_blocks = 0UL;
    session.n_bytes_written = 0UL;
    session.status = kStatus_Success;

    session.mem_start_addr = ((uintptr_t)MEM_GET_SYMBOL_VALUE(__firmware_start)) - ((uintptr_t)MEM_GET_SYMBOL_VALUE(__flash_start));
    session.mem_end_addr = ((uintptr_t)MEM_GET_SYMBOL_VALUE(__firmware_end)) - ((uintptr_t)MEM_GET_SYMBOL_VALUE(__flash_start));
    session.write_addr = session.mem_start_addr;
    session.erase_addr = session.mem_start_addr;

    session.upgrade_status = BU_STATUS_UNKNOWN_ERROR;
    session.verification_status = FWV_STATUS_UNKNOWN_ERROR;

    // Initialize session write data handler
    session.write_data_handler = write_initial_block_handler;

    // Initialize session data buffer
    mboot_buffer_init();
    mboot_buffer_reset();

    return BU_STATUS_OK;
}

mboot_upgrade_status_t mboot_upgrade_deinit() {
    // De-initialize firmware upgrade session
    session.n_blocks = 0UL;
    session.n_bytes_written = 0UL;
    session.status = kStatus_Success;

    session.mem_start_addr = 0UL;
    session.mem_end_addr = 0UL;
    session.write_addr = session.mem_start_addr;
    session.erase_addr = session.mem_start_addr;

    session.upgrade_status = BU_STATUS_UNKNOWN_ERROR;
    session.verification_status = FWV_STATUS_UNKNOWN_ERROR;

    // De-initialize session write data handler
    session.write_data_handler = write_initial_block_handler;

    // De-initialize session data buffer
    mboot_buffer_reset();

    return BU_STATUS_OK;
}

// TODO: Return error when more data is 'written' than available
mboot_upgrade_status_t mboot_upgrade_write_data(uint16_t block_num, const uint8_t *data, uint16_t block_length) {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;
    session.n_blocks += 1UL;

    do {
        // Append data to buffer when not full
        if (!mboot_buffer_is_full()) {
            uint32_t remaining_data = mboot_buffer_append(data, block_length);
            // Update data buffer pointer to point to remaining data and update remaining block length
            data += block_length - remaining_data;
            block_length = remaining_data;
            // ---
            ret_status = BU_STATUS_OK;
        }

        // Write data to flash when buffer full
        if (mboot_buffer_is_full()) {
            ret_status = session.write_data_handler();

            if (ret_status != BU_STATUS_OK) {
                break;
            }
        }
    } while (block_length != 0);

    session.upgrade_status = ret_status;
    return ret_status;
}

mboot_upgrade_status_t mboot_upgrade_finalize() {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;

    do {
        // Pad buffer
        mboot_buffer_pad();

        ret_status = session.write_data_handler();
        if (ret_status != BU_STATUS_OK) {
            break;
        }
    } while (session.write_addr < session.erase_addr);

    session.upgrade_status = ret_status;
    return ret_status;
}

mboot_upgrade_status_t mboot_upgrade_validate() {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;
    uintptr_t start_addr = ((uintptr_t)MEM_GET_SYMBOL_VALUE(__firmware_start)) + sizeof(fw_header_t);

    do {
        mboot_validate_status_t fw_status = mboot_validate_firmware(&image_header, start_addr);
        session.verification_status = fw_status;

        // Check if downloaded firmware is valid
        if (fw_status != FWV_STATUS_OK) {
            ret_status = BU_STATUS_FIRMWARE_INVALID;
            break;
        }

        // Write image header to flash to validate downloaded firmware
        session.status = flash_write_block(session.mem_start_addr,
            (uint8_t *)&image_header,
            sizeof(image_header));
        if (session.status != kStatus_Success) {
            ret_status = BU_STATUS_FLASH_WRITE_ERROR;
            break;
        }

        // Validation successfull
        ret_status = BU_STATUS_OK;
    } while (false);

    session.upgrade_status = ret_status;
    return ret_status;
}

// --------------------------------------------------------------------+
// Local Function Definitions
// --------------------------------------------------------------------+
static mboot_upgrade_status_t write_initial_block_handler() {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;
    uint8_t *data = mboot_buffer_data_ptr();
    uint32_t length = mboot_buffer_len();

    do {
        // Check received image header before writing any data to flash
        //  in order to prevent erasing a valid firmware if user tries to
        //  download an incompatible firmware image.
        if (mboot_validate_header((fw_header_t *)data) != FWV_STATUS_OK) {
            ret_status = BU_STATUS_RECEIVED_IMAGE_HEADER_INVALID;
            break;
        }

        // Store image header for later use in validation step of
        //  downloaded firmware.
        memcpy(&image_header, (void *)data, sizeof(fw_header_t));

        // Increment data pointer to offset image header.
        data = (data + sizeof(fw_header_t));
        // Offset write address to skip image header when writing initial block.
        session.write_addr += sizeof(fw_header_t);

        ret_status = write_block(data, length - sizeof(fw_header_t),
            &session.erase_addr,
            &session.write_addr);
        if (ret_status != BU_STATUS_OK) {
            break;
        }

        // Swap session write handler function if
        //  initial block has been successfully written to flash memory.
        session.write_data_handler = write_block_handler;
    } while (false);

    mboot_buffer_reset();

    return ret_status;
}

static mboot_upgrade_status_t write_block_handler() {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;
    uint8_t *data = mboot_buffer_data_ptr();
    uint32_t length = mboot_buffer_len();

    ret_status = write_block(data, length,
        &session.erase_addr,
        &session.write_addr);

    mboot_buffer_reset();

    return ret_status;
}

static mboot_upgrade_status_t write_block(uint8_t *data, uint32_t length, uintptr_t *eaddr, uintptr_t *waddr) {
    mboot_upgrade_status_t ret_status = BU_STATUS_UNKNOWN_ERROR;

    do {
        // Erase block of flash only if write address plus data length
        //  exceeds already erased area
        if (*eaddr < (*waddr + length)) {
            do {
                session.status = flash_erase_block(*eaddr);

                if (session.status == kStatus_Success) {
                    *eaddr += BLOCK_SIZE_BYTES;
                    ret_status = BU_STATUS_OK;
                } else {
                    ret_status = BU_STATUS_FLASH_ERASE_ERROR;
                    break; // Break write operation in case of erase error
                }
            } while (*eaddr < (*waddr + length));

            if (ret_status != BU_STATUS_OK) {
                break; // Break write operation in case of erase error
            }
        }

        // Write block of data to flash
        session.status = flash_write_block(*waddr, data, length);

        if (session.status == kStatus_Success) {
            *waddr += length;
            session.n_bytes_written += length;
            ret_status = BU_STATUS_OK;
        } else {
            ret_status = BU_STATUS_FLASH_WRITE_ERROR;
        }
    } while (false);

    return ret_status;
}
