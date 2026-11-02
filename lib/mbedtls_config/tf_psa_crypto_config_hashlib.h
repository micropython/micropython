// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018-2019 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// TF-PSA-Crypto configuration for ports that want hashlib but not ssl, selected with
// TF_PSA_CRYPTO_CONFIG_FILE when CIRCUITPY_HASHLIB_MBEDTLS_ONLY is set.
//
// Only the crypto config is needed here: nothing on this path includes
// mbedtls/build_info.h, so no TLS/X.509 configuration is required.
//
// The PSA core is enabled so that hashlib can use the same psa_hash_*() interface on
// every port. It also means PSA_WANT_* behaves as documented: tf-psa-crypto/build_info.h
// only derives the builtin implementations from PSA_WANT_* when MBEDTLS_PSA_CRYPTO_C is
// set, so without it PSA_WANT_ALG_SHA_256 would silently select nothing.

#pragma once

#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_DEPRECATED_REMOVED

// Everything runs in one address space, so PSA functions don't need to copy their
// buffers before using them. The copies are heap allocations on every call, and web
// workflow hashes (the websocket handshake) while no VM, and so no VM heap, exists.
#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS

#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE

// The digests hashlib exposes.
#define PSA_WANT_ALG_SHA_1
#define PSA_WANT_ALG_SHA_256

// psa_crypto_init() initializes the RNG subsystem even in a build that only hashes, so
// a random generator has to be available. Take it straight from the port TRNG, which
// keeps entropy.c and ctr_drbg.c out of the build; see hashlib_psa_port.c.
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

#define MBEDTLS_SHA256_SMALLER

// Memory allocation hooks
#include <stdlib.h>
#include <stdio.h>
void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr);
#define MBEDTLS_PLATFORM_STD_CALLOC m_tracked_calloc
#define MBEDTLS_PLATFORM_STD_FREE m_tracked_free
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf
