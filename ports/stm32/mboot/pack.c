/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "dfu.h"
#include "gzstream.h"
#include "mboot.h"
#include "pack.h"

#if MBOOT_ENABLE_PACKING

// Keys provided externally by the board, will be built into mboot flash.
#include MBOOT_PACK_KEYS_FILE

// Encrypted dfu files using gzip require a decompress buffer. Larger can be faster.
// This setting is independent to the incoming encrypted/signed/compressed DFU file.
#ifndef MBOOT_PACK_GZIP_BUFFER_SIZE
#define MBOOT_PACK_GZIP_BUFFER_SIZE (2048)
#endif

// State to manage automatic flash erasure.
static uint32_t erased_base_addr;
static uint32_t erased_top_addr;

// DFU chunk buffer, used to cache incoming blocks of data from USB.
static uint32_t firmware_chunk_base_addr;
static mboot_pack_chunk_buf_t firmware_chunk_buf;

// Temporary buffer for decrypted data.
static uint8_t decrypted_buf[MBOOT_PACK_DFU_CHUNK_BUF_SIZE] __attribute__((aligned(8)));

// Temporary buffer for uncompressing.
static uint8_t uncompressed_buf[MBOOT_PACK_GZIP_BUFFER_SIZE] __attribute__((aligned(8)));

// Buffer to hold the start of the firmware, which is only written once the
// entire firmware is validated.  This is 8 bytes due to STM32WB MCUs requiring
// that a double-word write to flash can only be done once (due to ECC).
static uint8_t firmware_head[8];

void mboot_pack_init(void) {
    erased_base_addr = 0;
    erased_top_addr = 0;
    firmware_chunk_base_addr = 0;
}

// In encrypted mode the erase is automatically managed.
// Note: this scheme requires blocks be written in sequence, which is the case.
static int mboot_pack_erase(uint32_t addr, size_t len) {
    while (!(erased_base_addr <= addr && addr + len <= erased_top_addr)) {
        uint32_t erase;
        if (erased_base_addr <= addr && addr < erased_top_addr) {
            erase = erased_top_addr;
        } else {
            erase = addr;
            erased_base_addr = addr;
        }
        uint32_t next_addr;
        int ret = hw_page_erase(erase, &next_addr);
        if (ret != 0) {
            return ret;
        }
        erased_top_addr = next_addr;
    }
    return 0;
}

// Commit an unencrypted and uncompressed chunk of firmware to the flash.
static int mboot_pack_commit_chunk(uint32_t addr, uint8_t *data, size_t len) {
    // Erase any required sectors before writing.
    int ret = mboot_pack_erase(addr, len);
    if (ret != 0) {
        return ret;
    }

    if (addr == APPLICATION_ADDR) {
        // Don't write the very start of the firmware, just copy it into a temporary buffer.
        // It will be written only if the full firmware passes the checksum/signature.
        memcpy(firmware_head, data, sizeof(firmware_head));
        addr += sizeof(firmware_head);
        data += sizeof(firmware_head);
        len -= sizeof(firmware_head);
    }

    // Commit this piece of the firmware.
    return hw_write(addr, data, len);
}

// Handle a chunk with the full firmware signature.
static int mboot_pack_handle_full_sig(void) {
    if (firmware_chunk_buf.header.length < hydro_sign_BYTES) {
        return -1;
    }

    uint8_t *full_sig = &firmware_chunk_buf.data[firmware_chunk_buf.header.length - hydro_sign_BYTES];
    uint32_t *region_data = (uint32_t *)&firmware_chunk_buf.data[0];
    size_t num_regions = (full_sig - (uint8_t *)region_data) / sizeof(uint32_t) / 2;

    uint8_t *buf = decrypted_buf;
    const size_t buf_alloc = sizeof(decrypted_buf);

    // Compute the signature of the full firmware.
    hydro_sign_state sign_state;
    hydro_sign_init(&sign_state, MBOOT_PACK_HYDRO_CONTEXT);
    for (size_t region = 0; region < num_regions; ++region) {
        uint32_t addr = region_data[2 * region];
        uint32_t len = region_data[2 * region + 1];
        while (len) {
            uint32_t l = len <= buf_alloc ? len : buf_alloc;
            hw_read(addr, l, buf);
            if (addr == APPLICATION_ADDR) {
                // The start of the firmware was not yet written to flash so copy
                // it out of the temporary buffer to compute the full signature.
                memcpy(buf, firmware_head, sizeof(firmware_head));
            }
            int ret = hydro_sign_update(&sign_state, buf, l);
            if (ret != 0) {
                return -1;
            }
            addr += l;
            len -= l;
        }
    }

    // Verify the signature of the full firmware.
    int ret = hydro_sign_final_verify(&sign_state, full_sig, mboot_pack_sign_public_key);
    if (ret != 0) {
        dfu_context.status = DFU_STATUS_ERROR_VERIFY;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_SIG_IDX;
        return -1;
    }

    // Full firmware passed the signature check.
    // Write the start of the firmware so it boots.
    return hw_write(APPLICATION_ADDR, firmware_head, sizeof(firmware_head));
}

// Handle a chunk with firmware data.
static int mboot_pack_handle_firmware(void) {
    const uint8_t *fw_data = &firmware_chunk_buf.data[0];
    const size_t fw_len = firmware_chunk_buf.header.length;

    // Decrypt the chunk.
    if (hydro_secretbox_decrypt(decrypted_buf, fw_data, fw_len, 0, MBOOT_PACK_HYDRO_CONTEXT, mboot_pack_secretbox_key) != 0) {
        dfu_context.status = DFU_STATUS_ERROR_VERIFY;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_SIG_IDX;
        return -1;
    }

    // Use the decrypted message contents going formward.
    size_t len = fw_len - hydro_secretbox_HEADERBYTES;
    uint32_t addr = firmware_chunk_buf.header.address;

    if (firmware_chunk_buf.header.format == MBOOT_PACK_CHUNK_FW_GZIP) {
        // Decompress chunk data.
        gz_stream_init_from_raw_data(decrypted_buf, len);
        for (;;) {
            int read = gz_stream_read(sizeof(uncompressed_buf), uncompressed_buf);
            if (read == 0) {
                return 0; // finished decompressing
            } else if (read < 0) {
                return -1; // error reading
            }
            int ret = mboot_pack_commit_chunk(addr, uncompressed_buf, read);
            if (ret != 0) {
                return ret;
            }
            addr += read;
        }
    } else {
        // Commit chunk data directly.
        return mboot_pack_commit_chunk(addr, decrypted_buf, len);
    }
}

int mboot_pack_write(uint32_t addr, const uint8_t *src8, size_t len) {
    if (addr == APPLICATION_ADDR) {
        // Base address of main firmware, reset any previous state
        firmware_chunk_base_addr = 0;
    }

    if (firmware_chunk_base_addr == 0) {
        // First piece of data starting a new chunk, so set the base address.
        firmware_chunk_base_addr = addr;
    }

    if (addr < firmware_chunk_base_addr) {
        // Address out of range.
        firmware_chunk_base_addr = 0;
        return -1;
    }

    size_t offset = addr - firmware_chunk_base_addr;
    if (offset + len > sizeof(firmware_chunk_buf)) {
        // Address/length out of range.
        firmware_chunk_base_addr = 0;
        return -1;
    }

    // Copy in the new data piece into the chunk buffer.
    memcpy((uint8_t *)&firmware_chunk_buf + offset, src8, len);

    if (offset + len < sizeof(firmware_chunk_buf.header)) {
        // Don't have the header yet.
        return 0;
    }

    if (firmware_chunk_buf.header.header_vers != MBOOT_PACK_HEADER_VERSION) {
        // Chunk header has the wrong version.
        dfu_context.status = DFU_STATUS_ERROR_FILE;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_SIG_IDX;
        return -1;
    }

    if (firmware_chunk_buf.header.address != firmware_chunk_base_addr) {
        // Chunk address doesn't agree with dfu address, abort.
        dfu_context.status = DFU_STATUS_ERROR_ADDRESS;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_SIG_IDX;
        return -1;
    }

    if (offset + len < sizeof(firmware_chunk_buf.header) + firmware_chunk_buf.header.length + sizeof(firmware_chunk_buf.signature)) {
        // Don't have the full chunk yet.
        return 0;
    }

    // Have the full chunk in firmware_chunk_buf, process it now.

    // Reset the chunk base address for the next chunk that comes in.
    firmware_chunk_base_addr = 0;

    // Verify the signature of the chunk.
    const size_t fw_len = firmware_chunk_buf.header.length;
    const uint8_t *sig = &firmware_chunk_buf.data[0] + fw_len;
    if (hydro_sign_verify(sig, &firmware_chunk_buf, sizeof(firmware_chunk_buf.header) + fw_len,
        MBOOT_PACK_HYDRO_CONTEXT, mboot_pack_sign_public_key) != 0) {
        // Signature failed
        dfu_context.status = DFU_STATUS_ERROR_VERIFY;
        dfu_context.error = MBOOT_ERROR_STR_INVALID_SIG_IDX;
        return -1;
    }

    // Signature passed, we have valid chunk.

    if (firmware_chunk_buf.header.format == MBOOT_PACK_CHUNK_META) {
        // Ignore META chunks.
        return 0;
    } else if (firmware_chunk_buf.header.format == MBOOT_PACK_CHUNK_FULL_SIG) {
        return mboot_pack_handle_full_sig();
    } else if (firmware_chunk_buf.header.format == MBOOT_PACK_CHUNK_FW_RAW
        || firmware_chunk_buf.header.format == MBOOT_PACK_CHUNK_FW_GZIP) {
        return mboot_pack_handle_firmware();
    } else {
        // Unsupported contents.
        return -1;
    }
}

#endif // MBOOT_ENABLE_PACKING
