// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// rp2_sha256_block.h on the RP2350 SHA-256 block. The SHA-256 bootrom lock is
// held while a context owns the block.

#include <string.h>

#include "hardware/structs/sha256.h"
#include "hardware/address_mapped.h"
#include "pico/bootrom/lock.h"

#include "rp2_sha256_block.h"

bool rp2_sha256_block_try_lock(void) {
    return bootrom_try_acquire_lock(BOOTROM_LOCK_SHA_256);
}

void rp2_sha256_block_unlock(void) {
    bootrom_release_lock(BOOTROM_LOCK_SHA_256);
}

void rp2_sha256_block_start(void) {
    // Big-endian message words from a byte buffer, like pico_sha256 with
    // SHA256_BIG_ENDIAN. START loads the SHA-256 initial value.
    hw_set_bits(&sha256_hw->csr, SHA256_CSR_BSWAP_BITS);
    hw_clear_bits(&sha256_hw->csr, SHA256_CSR_ERR_WDATA_NOT_RDY_BITS);
    hw_set_bits(&sha256_hw->csr, SHA256_CSR_START_BITS);
}

void rp2_sha256_block_write(const uint8_t *data) {
    // WDATA_RDY only drops after the 16th word, while the block is digested.
    while (!(sha256_hw->csr & SHA256_CSR_WDATA_RDY_BITS)) {
    }
    for (size_t i = 0; i < 64; i += 4) {
        uint32_t word;
        // data may be unaligned.
        memcpy(&word, data + i, sizeof(word));
        sha256_hw->wdata = word;
    }
}

void rp2_sha256_block_read(uint32_t state[8]) {
    while (!(sha256_hw->csr & SHA256_CSR_SUM_VLD_BITS)) {
    }
    for (size_t i = 0; i < 8; i++) {
        state[i] = sha256_hw->sum[i];
    }
}
