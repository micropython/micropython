// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// SHA-256 on the RP2350 SHA-256 block, under the mbedtls_sha256_*() API. The
// Makefile renames the builtin functions to mbedtls_sha256_sw_*() and these take
// their place, so hashlib, hmac and TLS all come through here.
//
// The block holds one running hash and its state cannot be written, only read, so:
//  - Only one context at a time owns the block. It takes the block when it first
//    has a whole 64-byte block to hash, which is the only time its state still
//    equals the initial value that START loads.
//  - Reading SUM0..7 after a whole block gives the chaining state, which is exactly
//    what the software code keeps in ctx->state. So an owner can always be moved to
//    software: for a digest, a clone, or when another context takes the block.
//  - Partial blocks stay in ctx->buffer and ctx->total is kept as the software code
//    keeps it, so a moved context carries on in software with no other fix-up.
//  - SHA-224 starts from a different initial value, so it always uses software.
//
// While a context owns the block its ctx->state is not needed, so it holds a
// signature instead: the context's own address and an ownership generation. A
// context that is freed without mbedtls_sha256_free() (for example a hashlib object
// that is garbage collected) leaves a dangling owner pointer. Before writing to the
// old owner, check that the signature is still there; if it is not, the memory has
// been reused and the old owner is simply dropped.
//
// The SHA-256 bootrom lock is held while a context owns the block.

#include "tf_psa_crypto_common.h"
#include "mbedtls/private/sha256.h"

#include <string.h>

#include "hardware/structs/sha256.h"
#include "pico/bootrom/lock.h"

#include "sha256_hw.h"

// The builtin implementation, renamed by the Makefile.
void mbedtls_sha256_sw_init(mbedtls_sha256_context *ctx);
void mbedtls_sha256_sw_free(mbedtls_sha256_context *ctx);
void mbedtls_sha256_sw_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src);
int mbedtls_sha256_sw_starts(mbedtls_sha256_context *ctx, int is224);
int mbedtls_sha256_sw_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen);
int mbedtls_sha256_sw_finish(mbedtls_sha256_context *ctx, unsigned char *output);

#define SHA256_BLOCK_SIZE (64)
#define SIG_MAGIC0 (0x53324852u)
#define SIG_MAGIC1 (0x3532ac1du)

static mbedtls_sha256_context *owner;
static uint32_t owner_gen;

static bool is_owner(const mbedtls_sha256_context *ctx) {
    return ctx == owner &&
           ctx->state[0] == SIG_MAGIC0 &&
           ctx->state[1] == (uint32_t)(uintptr_t)ctx &&
           ctx->state[2] == owner_gen &&
           ctx->state[3] == (~owner_gen ^ SIG_MAGIC1);
}

// Copy the chaining state of the block into dst->state. Doesn't change ownership.
static void read_state(mbedtls_sha256_context *dst) {
    while (!(sha256_hw->csr & SHA256_CSR_SUM_VLD_BITS)) {
    }
    for (size_t i = 0; i < 8; i++) {
        dst->state[i] = sha256_hw->sum[i];
    }
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
        read_state(owner);
    }
    release();
}

static void release_if_owner(const mbedtls_sha256_context *ctx) {
    // mbedtls allows free(NULL); without this check a NULL ctx would match a NULL owner.
    if (ctx != NULL && ctx == owner) {
        // ctx is being reset or freed, so its state is not needed.
        release();
    }
}

static bool try_acquire(mbedtls_sha256_context *ctx) {
    if (owner != NULL) {
        // Take the block from the previous owner, which lets go of the lock.
        evict_owner();
    }
    if (!bootrom_try_acquire_lock(BOOTROM_LOCK_SHA_256)) {
        return false;
    }
    owner = ctx;
    owner_gen++;
    ctx->state[0] = SIG_MAGIC0;
    ctx->state[1] = (uint32_t)(uintptr_t)ctx;
    ctx->state[2] = owner_gen;
    ctx->state[3] = ~owner_gen ^ SIG_MAGIC1;

    // Big-endian message words from a byte buffer, like pico_sha256 with
    // SHA256_BIG_ENDIAN. START loads the SHA-256 initial value.
    hw_set_bits(&sha256_hw->csr, SHA256_CSR_BSWAP_BITS);
    hw_clear_bits(&sha256_hw->csr, SHA256_CSR_ERR_WDATA_NOT_RDY_BITS);
    hw_set_bits(&sha256_hw->csr, SHA256_CSR_START_BITS);
    return true;
}

static void write_block(const unsigned char *data) {
    // WDATA_RDY only drops after the 16th word, while the block is digested.
    while (!(sha256_hw->csr & SHA256_CSR_WDATA_RDY_BITS)) {
    }
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; i += 4) {
        uint32_t word;
        // data may be unaligned.
        memcpy(&word, data + i, sizeof(word));
        sha256_hw->wdata = word;
    }
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx) {
    release_if_owner(ctx);
    mbedtls_sha256_sw_init(ctx);
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx) {
    release_if_owner(ctx);
    mbedtls_sha256_sw_free(ctx);
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src) {
    if (dst == src) {
        return;
    }
    release_if_owner(dst);
    mbedtls_sha256_sw_clone(dst, src);
    if (is_owner(src)) {
        // The copy goes on in software from the block's current state.
        read_state(dst);
    }
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224) {
    release_if_owner(ctx);
    return mbedtls_sha256_sw_starts(ctx, is224);
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen) {
    if (ilen == 0) {
        return 0;
    }
    size_t left = ctx->total[0] & 0x3F;
    if (!is_owner(ctx)) {
        if (ctx == owner) {
            // Stale pointer to memory that was reused for this context.
            release();
        }
        bool fresh = ctx->total[1] == 0 && ctx->total[0] < SHA256_BLOCK_SIZE;
        #if defined(MBEDTLS_SHA224_C)
        fresh = fresh && ctx->is224 == 0;
        #endif
        if (!fresh || left + ilen < SHA256_BLOCK_SIZE || !try_acquire(ctx)) {
            return mbedtls_sha256_sw_update(ctx, input, ilen);
        }
    }

    // Same buffering and length counting as the builtin mbedtls_sha256_update().
    size_t fill = SHA256_BLOCK_SIZE - left;
    ctx->total[0] += ilen;
    if (ctx->total[0] < ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy(ctx->buffer + left, input, fill);
        write_block(ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }
    while (ilen >= SHA256_BLOCK_SIZE) {
        write_block(input);
        input += SHA256_BLOCK_SIZE;
        ilen -= SHA256_BLOCK_SIZE;
    }
    if (ilen > 0) {
        memcpy(ctx->buffer + left, input, ilen);
    }
    return 0;
}

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output) {
    if (is_owner(ctx)) {
        // Padding is one or two blocks; do it in software from the block's state.
        read_state(ctx);
        release();
    } else {
        release_if_owner(ctx);
    }
    return mbedtls_sha256_sw_finish(ctx, output);
}

void rp2_sha256_hw_reset(void) {
    // Only release the lock if an owner holds it.
    if (owner != NULL) {
        release();
    }
}
