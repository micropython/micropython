/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// Entropy source for os.urandom() and the random-module seed.
//
// Preference order: EFI_RNG_PROTOCOL (firmware/hardware RNG) -> RDRAND on x86-64 ->
// a counter-seeded PRNG fallback (xorshift64*, mixing the cycle counter each call).
// The fallback always succeeds, so callers always get bytes even when no hardware
// RNG is present (e.g. QEMU TCG without virtio-rng).

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "py/mphal.h"
#include "efi.h"
#include "uefi_port.h"

// --- EFI_RNG_PROTOCOL (minimal; this is its only user) ---
#define EFI_RNG_PROTOCOL_GUID \
    {0x3152bca5, 0xeade, 0x433d, {0x86, 0x2e, 0xc0, 0x1c, 0xdc, 0x29, 0x1f, 0x44}}
typedef struct _EFI_RNG_PROTOCOL EFI_RNG_PROTOCOL;
struct _EFI_RNG_PROTOCOL {
    void *GetInfo;
    EFI_STATUS(EFIAPI * GetRNG)(EFI_RNG_PROTOCOL * This, EFI_GUID *Algorithm, UINTN Length, UINT8 *Value);
};

static int efi_rng_fill(uint8_t *buf, size_t len) {
    EFI_GUID guid = EFI_RNG_PROTOCOL_GUID;
    EFI_RNG_PROTOCOL *rng;
    if (EFI_ERROR(mp_uefi_st->BootServices->LocateProtocol(&guid, NULL, (void **)&rng))) {
        return 0;
    }
    return !EFI_ERROR(rng->GetRNG(rng, NULL, (UINTN)len, buf));
}

#if defined(__x86_64__)
static int rdrand_fill(uint8_t *buf, size_t len) {
    uint32_t a, b, c, d;
    __asm__ volatile ("cpuid" : "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (1u), "c" (0u));
    if (!(c & (1u << 30))) { // RDRAND not supported
        return 0;
    }
    for (size_t i = 0; i < len;) {
        uint64_t r;
        unsigned char ok;
        for (int tries = 0;; tries++) {
            __asm__ volatile ("rdrand %0; setc %1" : "=r" (r), "=qm" (ok));
            if (ok) {
                break;
            }
            if (tries > 16) {
                return 0;
            }
        }
        size_t n = (len - i < sizeof(r)) ? (len - i) : sizeof(r);
        memcpy(buf + i, &r, n);
        i += n;
    }
    return 1;
}
#endif

// xorshift64* fallback, lazily seeded and continuously re-stirred by the counter.
static uint64_t prng_state;

static void prng_fill(uint8_t *buf, size_t len) {
    if (prng_state == 0) {
        prng_state = ((uint64_t)mp_hal_ticks_cpu() << 1) | 1u;
    }
    for (size_t i = 0; i < len;) {
        prng_state ^= prng_state >> 12;
        prng_state ^= prng_state << 25;
        prng_state ^= prng_state >> 27;
        uint64_t r = (prng_state * 0x2545F4914F6CDD1DULL) ^ (uint64_t)mp_hal_ticks_cpu();
        size_t n = (len - i < sizeof(r)) ? (len - i) : sizeof(r);
        memcpy(buf + i, &r, n);
        i += n;
    }
}

void mp_hal_get_random(size_t n, void *buf) {
    if (efi_rng_fill(buf, n)) {
        return;
    }
    #if defined(__x86_64__)
    if (rdrand_fill(buf, n)) {
        return;
    }
    #endif
    prng_fill(buf, n);
}

// Seed value for random.seed() with no argument (MICROPY_PY_RANDOM_SEED_INIT_FUNC).
unsigned int mp_uefi_random_seed(void) {
    unsigned int s;
    mp_hal_get_random(sizeof(s), &s);
    return s;
}
