/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright The Mbed TLS Contributors
 * Copyright (c) 2024 Damien P. George
 *
 * This file provides default fallback functions for use with alternate
 * cryptography functions implemented in Python.
 */
#if MICROPY_PY_SSL_ECDSA_SIGN_ALT
#if defined(MBEDTLS_ECP_RESTARTABLE) || defined(MBEDTLS_ECDSA_DETERMINISTIC)
#error "MICROPY_PY_SSL_ECDSA_SIGN_ALT cannot be used with MBEDTLS_ECP_RESTARTABLE or MBEDTLS_ECDSA_DETERMINISTIC"
#endif

#include <string.h>
#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"

extern int micropy_mbedtls_ecdsa_sign_alt(const mbedtls_mpi *d, const unsigned char *hash, size_t hlen,
    unsigned char *sig, size_t sig_size, size_t *slen);


// Compute and write signature
// See lib/mbedtls/library/ecdsa.c:688
//
// Note: To avoid duplicating a lot of code, MBEDTLS_ECDSA_SIGN_ALT is not defined,
// which allows the default mbedtls_ecdsa_sign to be used as a fallback function.
// However, mbedtls_ecdsa_sign cannot be wrapped because it is called internally
// within its object file, so we wrap mbedtls_ecdsa_read/write_signature instead.
int __wrap_mbedtls_ecdsa_write_signature(mbedtls_ecdsa_context *ctx,
    mbedtls_md_type_t md_alg,
    const unsigned char *hash, size_t hlen,
    unsigned char *sig, size_t sig_size, size_t *slen,
    int (*f_rng)(void *, unsigned char *, size_t),
    void *p_rng) {

    (void)md_alg;

    if (f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // Check if curve is supported for ECDSA.
    if (!mbedtls_ecdsa_can_do(ctx->grp.id) || ctx->grp.N.p == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // Try signing with the alternative function first.
    int ret = micropy_mbedtls_ecdsa_sign_alt(&ctx->d, hash, hlen, sig, sig_size, slen);

    // Fallback to the default mbedtls implementation if needed.
    if (ret == MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED) {
        mbedtls_mpi r, s;
        mbedtls_mpi_init(&r);
        mbedtls_mpi_init(&s);

        size_t len = 0;
        unsigned char buf[MBEDTLS_ECDSA_MAX_LEN] = { 0 };
        unsigned char *p = buf + sizeof(buf);

        MBEDTLS_MPI_CHK(mbedtls_ecdsa_sign(&ctx->grp, &r, &s, &ctx->d, hash, hlen, f_rng, p_rng));
        MBEDTLS_ASN1_CHK_CLEANUP_ADD(len, mbedtls_asn1_write_mpi(&p, buf, &s));
        MBEDTLS_ASN1_CHK_CLEANUP_ADD(len, mbedtls_asn1_write_mpi(&p, buf, &r));
        MBEDTLS_ASN1_CHK_CLEANUP_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
        MBEDTLS_ASN1_CHK_CLEANUP_ADD(len, mbedtls_asn1_write_tag(&p, buf, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

        if (len > sig_size) {
            ret = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
        } else {
            ret = 0;
            *slen = len;
            memcpy(sig, p, len);
        }

    cleanup:
        mbedtls_mpi_free(&r);
        mbedtls_mpi_free(&s);
    }

    return ret;
}
#endif
