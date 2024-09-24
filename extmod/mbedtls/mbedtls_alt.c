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
#include <string.h>
#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"

extern int mbedtls_ecdsa_sign_alt(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
    const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);


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

    mbedtls_mpi r, s;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char buf[MBEDTLS_ECDSA_MAX_LEN] = { 0 };
    unsigned char *p = buf + sizeof(buf);

    if (f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    // Check if curve is supported for ECDSA.
    if (!mbedtls_ecdsa_can_do(ctx->grp.id) || ctx->grp.N.p == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    // Try signing with the alternative function first. If the provided key can't be
    // used by the alternative function, then fallback to the default implementation.
    ret = mbedtls_ecdsa_sign_alt(&ctx->grp, &r, &s, &ctx->d, hash, hlen, f_rng, p_rng);
    if (ret == MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED) {
        // Fallback to the default mbedtls implementation.
        MBEDTLS_MPI_CHK(mbedtls_ecdsa_sign(&ctx->grp, &r, &s, &ctx->d, hash, hlen, f_rng, p_rng));
    } else if (ret != 0) {
        // Failed to sign with the alternative function for another reason.
        goto cleanup;
    }

    size_t len = 0;
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, &s));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, &r));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf,
        MBEDTLS_ASN1_CONSTRUCTED |
        MBEDTLS_ASN1_SEQUENCE));

    if (len > sig_size) {
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    memcpy(sig, p, len);
    *slen = len;

    return 0;


cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return ret;
}
#endif
