// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// PSA transparent driver for SHA-256 on the RP2350 SHA-256 block. hashlib, hmac
// and TLS reach it through psa_hash_*(); the dispatch tries it before the builtin
// driver. It keeps an mbedtls_sha256_context in the builtin operation struct and
// uses the builtin mbedtls_sha256_*() functions for anything done in software.
//
// The block holds one running hash and its state cannot be written, only read, so:
//  - Only one context at a time owns the block. It takes the block when it first
//    has a whole 64-byte block to hash, which is the only time its state still
//    equals the initial value that START loads.
//  - Reading the block's state after a whole block gives the chaining state, which
//    is exactly what the software code keeps in ctx->state. So an owner can always
//    be moved to software: for a digest, a clone, or when another context takes
//    the block.
//  - Partial blocks stay in ctx->buffer and ctx->total is kept as the software code
//    keeps it, so a moved context carries on in software with no other fix-up.
//
// While a context owns the block its ctx->state is not needed, so it holds a
// signature instead: the context's own address and an ownership generation. An
// operation that is freed without psa_hash_abort() (for example a hashlib object
// that is garbage collected) leaves a dangling owner pointer. Before writing to the
// old owner, check that the signature is still there; if it is not, the memory has
// been reused and the old owner is simply dropped.

#include "tf_psa_crypto_common.h"
#include "mbedtls/private/sha256.h"

#include <string.h>

#include "rp2_sha256_block.h"
#include "rp2_sha256_psa.h"

#define SHA256_BLOCK_SIZE (64)
#define SHA256_DIGEST_SIZE (32)
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

static void release(void) {
    owner = NULL;
    rp2_sha256_block_unlock();
}

// Leave the block. A live owner first gets its state back, so it can go on in
// software; a dead one is just dropped.
static void evict_owner(void) {
    if (owner == NULL) {
        return;
    }
    if (is_owner(owner)) {
        rp2_sha256_block_read(owner->state);
    }
    release();
}

static void release_if_owner(const mbedtls_sha256_context *ctx) {
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
    if (!rp2_sha256_block_try_lock()) {
        return false;
    }
    owner = ctx;
    owner_gen++;
    ctx->state[0] = SIG_MAGIC0;
    ctx->state[1] = (uint32_t)(uintptr_t)ctx;
    ctx->state[2] = owner_gen;
    ctx->state[3] = ~owner_gen ^ SIG_MAGIC1;
    rp2_sha256_block_start();
    return true;
}

static void ctx_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen) {
    if (ilen == 0) {
        return;
    }
    size_t left = ctx->total[0] & 0x3F;
    if (!is_owner(ctx)) {
        if (ctx == owner) {
            // Stale pointer to memory that was reused for this context.
            release();
        }
        bool fresh = ctx->total[1] == 0 && ctx->total[0] < SHA256_BLOCK_SIZE;
        if (!fresh || left + ilen < SHA256_BLOCK_SIZE || !try_acquire(ctx)) {
            // Software. Can't fail for SHA-256.
            mbedtls_sha256_update(ctx, input, ilen);
            return;
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
        rp2_sha256_block_write(ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }
    while (ilen >= SHA256_BLOCK_SIZE) {
        rp2_sha256_block_write(input);
        input += SHA256_BLOCK_SIZE;
        ilen -= SHA256_BLOCK_SIZE;
    }
    if (ilen > 0) {
        memcpy(ctx->buffer + left, input, ilen);
    }
}

static void ctx_finish(mbedtls_sha256_context *ctx, unsigned char *output) {
    if (is_owner(ctx)) {
        // Padding is one or two blocks; do it in software from the block's state.
        rp2_sha256_block_read(ctx->state);
        release();
    } else {
        release_if_owner(ctx);
    }
    mbedtls_sha256_finish(ctx, output);
}

psa_status_t rp2_sha256_psa_hash_setup(mbedtls_psa_hash_operation_t *op, psa_algorithm_t alg) {
    if (alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    mbedtls_sha256_context *ctx = &op->ctx.sha256;
    release_if_owner(ctx);
    op->alg = alg;
    mbedtls_sha256_init(ctx);
    mbedtls_sha256_starts(ctx, 0);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_update(mbedtls_psa_hash_operation_t *op,
    const uint8_t *input, size_t input_length) {
    if (op->alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_BAD_STATE;
    }
    ctx_update(&op->ctx.sha256, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_finish(mbedtls_psa_hash_operation_t *op,
    uint8_t *hash, size_t hash_size, size_t *hash_length) {
    *hash_length = 0;
    if (op->alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_BAD_STATE;
    }
    if (hash_size < SHA256_DIGEST_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ctx_finish(&op->ctx.sha256, hash);
    *hash_length = SHA256_DIGEST_SIZE;
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_abort(mbedtls_psa_hash_operation_t *op) {
    if (op->alg == PSA_ALG_SHA_256) {
        release_if_owner(&op->ctx.sha256);
        mbedtls_sha256_free(&op->ctx.sha256);
    }
    op->alg = 0;
    return PSA_SUCCESS;
}

psa_status_t rp2_sha256_psa_hash_clone(const mbedtls_psa_hash_operation_t *source,
    mbedtls_psa_hash_operation_t *target) {
    if (source->alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_BAD_STATE;
    }
    const mbedtls_sha256_context *src = &source->ctx.sha256;
    mbedtls_sha256_context *dst = &target->ctx.sha256;
    if (dst == src) {
        return PSA_SUCCESS;
    }
    release_if_owner(dst);
    target->alg = source->alg;
    mbedtls_sha256_clone(dst, src);
    if (is_owner(src)) {
        // The copy goes on in software from the block's current state.
        rp2_sha256_block_read(dst->state);
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
    mbedtls_psa_hash_operation_t op;
    memset(&op, 0, sizeof(op));
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
