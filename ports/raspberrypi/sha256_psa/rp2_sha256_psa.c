// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// PSA transparent driver for SHA-256 on the RP2350 SHA-256 block. The context holds
// an mbedtls_sha256_context, and the builtin mbedtls_sha256_*() functions do
// anything done in software.
//
// The block holds one running hash, and its state can be read but not written:
//  - One context at a time owns the block. It takes the block when it first has a
//    whole 64-byte block to hash, while its state still equals the initial value.
//  - After a whole block, the block's state is the chaining state the software code
//    keeps in ctx->state. So an owner can always move to software: for a digest, a
//    clone, or when another context takes the block.
//  - Partial blocks stay in ctx->buffer and ctx->total is kept as the software code
//    keeps it, so a moved context goes on in software unchanged.
//
// While a context owns the block, its ctx->state holds a signature instead: its own
// address and an ownership generation. An operation freed without
// psa_hash_abort() (a hashlib object that is garbage collected) leaves a dangling
// owner pointer, so the signature is checked before writing to the old owner. If
// it is gone, the memory has been reused and the old owner is dropped.

#include "tf_psa_crypto_common.h"
#include "mbedtls/private/sha256.h"

#include <string.h>

#include "hardware/sha256.h"
#include "pico/bootrom/lock.h"

#include "rp2_sha256_psa.h"

#define SHA256_BLOCK_LENGTH PSA_HASH_BLOCK_LENGTH(PSA_ALG_SHA_256)
#define SIGNATURE_MAGIC0 (0x53324852u)
#define SIGNATURE_MAGIC1 (0x3532ac1du)

static mbedtls_sha256_context *owner;
static uint32_t owner_generation;

static void block_start(void) {
    // Big-endian message words from a byte buffer. START loads the initial value.
    sha256_set_bswap(true);
    sha256_err_not_ready_clear();
    sha256_start();
}

static void block_write(const uint8_t *data) {
    // Ready drops after the 16th word, while the block is digested.
    sha256_wait_ready_blocking();
    for (size_t i = 0; i < SHA256_BLOCK_LENGTH; i += 4) {
        uint32_t word;
        // data may be unaligned.
        memcpy(&word, data + i, sizeof(word));
        sha256_put_word(word);
    }
}

static void block_read(uint32_t state[8]) {
    sha256_wait_valid_blocking();
    for (size_t i = 0; i < 8; i++) {
        state[i] = sha256_hw->sum[i];
    }
}

static bool is_owner(const mbedtls_sha256_context *ctx) {
    return ctx == owner &&
           ctx->state[0] == SIGNATURE_MAGIC0 &&
           ctx->state[1] == (uint32_t)(uintptr_t)ctx &&
           ctx->state[2] == owner_generation &&
           ctx->state[3] == (~owner_generation ^ SIGNATURE_MAGIC1);
}

static void release(void) {
    owner = NULL;
    bootrom_release_lock(BOOTROM_LOCK_SHA_256);
}

// Leave the block. A live owner first gets its state back, so it can go on in
// software; a dead one is just dropped.
static void evict_owner(void) {
    if (owner == NULL) {
        return;
    }
    if (is_owner(owner)) {
        block_read(owner->state);
    }
    release();
}

static void release_if_owner(const mbedtls_sha256_context *ctx) {
    if (ctx == owner) {
        // ctx is being reset or freed, so its state is not needed.
        release();
    }
}

static bool try_acquire(mbedtls_sha256_context *ctx) {
    evict_owner();
    if (!bootrom_try_acquire_lock(BOOTROM_LOCK_SHA_256)) {
        return false;
    }
    owner = ctx;
    owner_generation++;
    ctx->state[0] = SIGNATURE_MAGIC0;
    ctx->state[1] = (uint32_t)(uintptr_t)ctx;
    ctx->state[2] = owner_generation;
    ctx->state[3] = ~owner_generation ^ SIGNATURE_MAGIC1;
    block_start();
    return true;
}

static void ctx_update(mbedtls_sha256_context *ctx, const uint8_t *input, size_t input_length) {
    if (input_length == 0) {
        return;
    }
    size_t left = ctx->total[0] & (SHA256_BLOCK_LENGTH - 1);
    if (!is_owner(ctx)) {
        if (ctx == owner) {
            // Stale pointer to memory that was reused for this context.
            release();
        }
        bool fresh = ctx->total[1] == 0 && ctx->total[0] < SHA256_BLOCK_LENGTH;
        if (!fresh || left + input_length < SHA256_BLOCK_LENGTH || !try_acquire(ctx)) {
            // Software. Can't fail for SHA-256.
            mbedtls_sha256_update(ctx, input, input_length);
            return;
        }
    }

    // Same buffering and length counting as the builtin mbedtls_sha256_update().
    size_t fill = SHA256_BLOCK_LENGTH - left;
    ctx->total[0] += input_length;
    if (ctx->total[0] < input_length) {
        ctx->total[1]++;
    }

    if (left && input_length >= fill) {
        memcpy(ctx->buffer + left, input, fill);
        block_write(ctx->buffer);
        input += fill;
        input_length -= fill;
        left = 0;
    }
    while (input_length >= SHA256_BLOCK_LENGTH) {
        block_write(input);
        input += SHA256_BLOCK_LENGTH;
        input_length -= SHA256_BLOCK_LENGTH;
    }
    if (input_length > 0) {
        memcpy(ctx->buffer + left, input, input_length);
    }
}

static void ctx_finish(mbedtls_sha256_context *ctx, uint8_t *output) {
    if (is_owner(ctx)) {
        // Padding is one or two blocks; do it in software from the block's state.
        block_read(ctx->state);
        release();
    } else {
        release_if_owner(ctx);
    }
    mbedtls_sha256_finish(ctx, output);
}

psa_status_t rp2_sha256_psa_hash_setup(rp2_sha256_psa_hash_operation_t *op, psa_algorithm_t alg) {
    if (alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    release_if_owner(&op->sha256);
    mbedtls_sha256_init(&op->sha256);
    mbedtls_sha256_starts(&op->sha256, 0);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_update(rp2_sha256_psa_hash_operation_t *op,
    const uint8_t *input, size_t input_length) {
    ctx_update(&op->sha256, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_finish(rp2_sha256_psa_hash_operation_t *op,
    uint8_t *hash, size_t hash_size, size_t *hash_length) {
    *hash_length = 0;
    if (hash_size < PSA_HASH_LENGTH(PSA_ALG_SHA_256)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ctx_finish(&op->sha256, hash);
    *hash_length = PSA_HASH_LENGTH(PSA_ALG_SHA_256);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_abort(rp2_sha256_psa_hash_operation_t *op) {
    release_if_owner(&op->sha256);
    mbedtls_sha256_free(&op->sha256);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_clone(const rp2_sha256_psa_hash_operation_t *source,
    rp2_sha256_psa_hash_operation_t *target) {
    mbedtls_sha256_clone(&target->sha256, &source->sha256);
    if (is_owner(&source->sha256)) {
        // The copy goes on in software from the block's current state.
        block_read(target->sha256.state);
    }
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_compute(psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *hash, size_t hash_size, size_t *hash_length) {
    *hash_length = 0;
    if (alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    rp2_sha256_psa_hash_operation_t op;
    rp2_sha256_psa_hash_setup(&op, alg);
    rp2_sha256_psa_hash_update(&op, input, input_length);
    psa_status_t status = rp2_sha256_psa_hash_finish(&op, hash, hash_size, hash_length);
    rp2_sha256_psa_hash_abort(&op);
    return status;
}

void rp2_sha256_psa_reset(void) {
    if (owner != NULL) {
        release();
    }
}
