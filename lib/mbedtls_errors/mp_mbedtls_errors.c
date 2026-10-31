/* -*-c-*-
 *  Error message information
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 *  This file originates from Mbed TLS, but has been modified to
 *  reduce code size impact when used with the generation scripts in this
 *  directory.
 *
 *  Modifications in this file:
 *  - Store the strings in arrays rather than large switch statements.
 *  - Rename mbedtls_strerror to __wrap_mbedtls_strerror to avoid conflict
 *    with the in-library version of this symbol.
 */

#include "mbedtls/version.h"

#if defined(MBEDTLS_ERROR_C) || defined(MBEDTLS_ERROR_STRERROR_DUMMY)
#include "mbedtls/error.h"
#include <string.h>
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#define mbedtls_snprintf snprintf
#define mbedtls_time_t   time_t
#endif

#if defined(MBEDTLS_ERROR_C)

#include <stdio.h>

#if defined(MBEDTLS_NET_C)
#include "mbedtls/net_sockets.h"
#endif

#if defined(MBEDTLS_PKCS7_C)
#include "mbedtls/pkcs7.h"
#endif

#if defined(MBEDTLS_SSL_TLS_C)
#include "mbedtls/ssl.h"
#endif

#if defined(MBEDTLS_X509_USE_C) || \
    defined(MBEDTLS_X509_CREATE_C)
#include "mbedtls/x509.h"
#endif

#if defined(MBEDTLS_AES_C)
#include "mbedtls/private/aes.h"
#endif

#if defined(MBEDTLS_ARIA_C)
#include "mbedtls/private/aria.h"
#endif

#if defined(MBEDTLS_BIGNUM_C)
#include "mbedtls/private/bignum.h"
#endif

#if defined(MBEDTLS_CAMELLIA_C)
#include "mbedtls/private/camellia.h"
#endif

#if defined(MBEDTLS_CHACHAPOLY_C)
#include "mbedtls/private/chachapoly.h"
#endif

#if defined(MBEDTLS_CIPHER_C)
#include "mbedtls/private/cipher.h"
#endif

#if defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/private/ctr_drbg.h"
#endif

#if defined(MBEDTLS_ECP_C)
#include "mbedtls/private/ecp.h"
#endif

#if defined(MBEDTLS_ENTROPY_C)
#include "mbedtls/private/entropy.h"
#endif

#if defined(MBEDTLS_HMAC_DRBG_C)
#include "mbedtls/private/hmac_drbg.h"
#endif

#if defined(MBEDTLS_PKCS5_C)
#include "mbedtls/private/pkcs5.h"
#endif

#if defined(MBEDTLS_RSA_C)
#include "mbedtls/private/rsa.h"
#endif


// Error code table type
struct ssl_errs {
    int16_t errnum;
    const char *errstr;
};

// Table of high level error codes
static const struct ssl_errs mbedtls_high_level_error_tab[] = {
// BEGIN generated code
#if defined(MBEDTLS_PKCS7_C)
        { -(MBEDTLS_ERR_PKCS7_INVALID_FORMAT), "PKCS7_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE), "PKCS7_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_VERSION), "PKCS7_INVALID_VERSION" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_CONTENT_INFO), "PKCS7_INVALID_CONTENT_INFO" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_ALG), "PKCS7_INVALID_ALG" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_CERT), "PKCS7_INVALID_CERT" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_SIGNATURE), "PKCS7_INVALID_SIGNATURE" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_SIGNER_INFO), "PKCS7_INVALID_SIGNER_INFO" },
        { -(MBEDTLS_ERR_PKCS7_CERT_DATE_INVALID), "PKCS7_CERT_DATE_INVALID" },
#endif /* MBEDTLS_PKCS7_C */

#if defined(MBEDTLS_SSL_TLS_C)
        { -(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS), "SSL_CRYPTO_IN_PROGRESS" },
        { -(MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE), "SSL_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_SSL_INVALID_MAC), "SSL_INVALID_MAC" },
        { -(MBEDTLS_ERR_SSL_INVALID_RECORD), "SSL_INVALID_RECORD" },
        { -(MBEDTLS_ERR_SSL_CONN_EOF), "SSL_CONN_EOF" },
        { -(MBEDTLS_ERR_SSL_DECODE_ERROR), "SSL_DECODE_ERROR" },
        { -(MBEDTLS_ERR_SSL_NO_RNG), "SSL_NO_RNG" },
        { -(MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE), "SSL_NO_CLIENT_CERTIFICATE" },
        { -(MBEDTLS_ERR_SSL_UNSUPPORTED_EXTENSION), "SSL_UNSUPPORTED_EXTENSION" },
        { -(MBEDTLS_ERR_SSL_NO_APPLICATION_PROTOCOL), "SSL_NO_APPLICATION_PROTOCOL" },
        { -(MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED), "SSL_PRIVATE_KEY_REQUIRED" },
        { -(MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED), "SSL_CA_CHAIN_REQUIRED" },
        { -(MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE), "SSL_UNEXPECTED_MESSAGE" },
        { -(MBEDTLS_ERR_SSL_UNRECOGNIZED_NAME), "SSL_UNRECOGNIZED_NAME" },
        { -(MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY), "SSL_PEER_CLOSE_NOTIFY" },
        { -(MBEDTLS_ERR_SSL_BAD_CERTIFICATE), "SSL_BAD_CERTIFICATE" },
        { -(MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET), "SSL_RECEIVED_NEW_SESSION_TICKET" },
        { -(MBEDTLS_ERR_SSL_CANNOT_READ_EARLY_DATA), "SSL_CANNOT_READ_EARLY_DATA" },
        { -(MBEDTLS_ERR_SSL_RECEIVED_EARLY_DATA), "SSL_RECEIVED_EARLY_DATA" },
        { -(MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA), "SSL_CANNOT_WRITE_EARLY_DATA" },
        { -(MBEDTLS_ERR_SSL_CACHE_ENTRY_NOT_FOUND), "SSL_CACHE_ENTRY_NOT_FOUND" },
        { -(MBEDTLS_ERR_SSL_HW_ACCEL_FAILED), "SSL_HW_ACCEL_FAILED" },
        { -(MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH), "SSL_HW_ACCEL_FALLTHROUGH" },
        { -(MBEDTLS_ERR_SSL_BAD_PROTOCOL_VERSION), "SSL_BAD_PROTOCOL_VERSION" },
        { -(MBEDTLS_ERR_SSL_HANDSHAKE_FAILURE), "SSL_HANDSHAKE_FAILURE" },
        { -(MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED), "SSL_SESSION_TICKET_EXPIRED" },
        { -(MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH), "SSL_PK_TYPE_MISMATCH" },
        { -(MBEDTLS_ERR_SSL_UNKNOWN_IDENTITY), "SSL_UNKNOWN_IDENTITY" },
        { -(MBEDTLS_ERR_SSL_INTERNAL_ERROR), "SSL_INTERNAL_ERROR" },
        { -(MBEDTLS_ERR_SSL_COUNTER_WRAPPING), "SSL_COUNTER_WRAPPING" },
        { -(MBEDTLS_ERR_SSL_WAITING_SERVER_HELLO_RENEGO), "SSL_WAITING_SERVER_HELLO_RENEGO" },
        { -(MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED), "SSL_HELLO_VERIFY_REQUIRED" },
        { -(MBEDTLS_ERR_SSL_WANT_READ), "SSL_WANT_READ" },
        { -(MBEDTLS_ERR_SSL_WANT_WRITE), "SSL_WANT_WRITE" },
        { -(MBEDTLS_ERR_SSL_TIMEOUT), "SSL_TIMEOUT" },
        { -(MBEDTLS_ERR_SSL_CLIENT_RECONNECT), "SSL_CLIENT_RECONNECT" },
        { -(MBEDTLS_ERR_SSL_UNEXPECTED_RECORD), "SSL_UNEXPECTED_RECORD" },
        { -(MBEDTLS_ERR_SSL_NON_FATAL), "SSL_NON_FATAL" },
        { -(MBEDTLS_ERR_SSL_ILLEGAL_PARAMETER), "SSL_ILLEGAL_PARAMETER" },
        { -(MBEDTLS_ERR_SSL_CONTINUE_PROCESSING), "SSL_CONTINUE_PROCESSING" },
        { -(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS), "SSL_ASYNC_IN_PROGRESS" },
        { -(MBEDTLS_ERR_SSL_EARLY_MESSAGE), "SSL_EARLY_MESSAGE" },
        { -(MBEDTLS_ERR_SSL_UNEXPECTED_CID), "SSL_UNEXPECTED_CID" },
        { -(MBEDTLS_ERR_SSL_VERSION_MISMATCH), "SSL_VERSION_MISMATCH" },
        { -(MBEDTLS_ERR_SSL_BAD_CONFIG), "SSL_BAD_CONFIG" },
        { -(MBEDTLS_ERR_SSL_CERTIFICATE_VERIFICATION_WITHOUT_HOSTNAME), "SSL_CERTIFICATE_VERIFICATION_WITHOUT_HOSTNAME" },
#endif /* MBEDTLS_SSL_TLS_C */

#if defined(MBEDTLS_X509_USE_C) || \
    defined(MBEDTLS_X509_CREATE_C)
        { -(MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE), "X509_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_X509_UNKNOWN_OID), "X509_UNKNOWN_OID" },
        { -(MBEDTLS_ERR_X509_INVALID_FORMAT), "X509_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_X509_INVALID_VERSION), "X509_INVALID_VERSION" },
        { -(MBEDTLS_ERR_X509_INVALID_SERIAL), "X509_INVALID_SERIAL" },
        { -(MBEDTLS_ERR_X509_INVALID_ALG), "X509_INVALID_ALG" },
        { -(MBEDTLS_ERR_X509_INVALID_NAME), "X509_INVALID_NAME" },
        { -(MBEDTLS_ERR_X509_INVALID_DATE), "X509_INVALID_DATE" },
        { -(MBEDTLS_ERR_X509_INVALID_SIGNATURE), "X509_INVALID_SIGNATURE" },
        { -(MBEDTLS_ERR_X509_INVALID_EXTENSIONS), "X509_INVALID_EXTENSIONS" },
        { -(MBEDTLS_ERR_X509_UNKNOWN_VERSION), "X509_UNKNOWN_VERSION" },
        { -(MBEDTLS_ERR_X509_UNKNOWN_SIG_ALG), "X509_UNKNOWN_SIG_ALG" },
        { -(MBEDTLS_ERR_X509_SIG_MISMATCH), "X509_SIG_MISMATCH" },
        { -(MBEDTLS_ERR_X509_CERT_VERIFY_FAILED), "X509_CERT_VERIFY_FAILED" },
        { -(MBEDTLS_ERR_X509_CERT_UNKNOWN_FORMAT), "X509_CERT_UNKNOWN_FORMAT" },
        { -(MBEDTLS_ERR_X509_BAD_INPUT_DATA), "X509_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_X509_FILE_IO_ERROR), "X509_FILE_IO_ERROR" },
        { -(MBEDTLS_ERR_X509_FATAL_ERROR), "X509_FATAL_ERROR" },
#endif /* MBEDTLS_X509_USE_C || 
          MBEDTLS_X509_CREATE_C */

#if defined(MBEDTLS_CIPHER_C)
        { -(MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE), "CIPHER_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED), "CIPHER_FULL_BLOCK_EXPECTED" },
        { -(MBEDTLS_ERR_CIPHER_INVALID_CONTEXT), "CIPHER_INVALID_CONTEXT" },
#endif /* MBEDTLS_CIPHER_C */

#if defined(MBEDTLS_ECP_C)
        { -(MBEDTLS_ERR_ECP_INVALID_KEY), "ECP_INVALID_KEY" },
#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_PKCS5_C)
        { -(MBEDTLS_ERR_PKCS5_INVALID_FORMAT), "PKCS5_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PKCS5_FEATURE_UNAVAILABLE), "PKCS5_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PKCS5_PASSWORD_MISMATCH), "PKCS5_PASSWORD_MISMATCH" },
#endif /* MBEDTLS_PKCS5_C */

#if defined(MBEDTLS_RSA_C)
        { -(MBEDTLS_ERR_RSA_KEY_GEN_FAILED), "RSA_KEY_GEN_FAILED" },
        { -(MBEDTLS_ERR_RSA_KEY_CHECK_FAILED), "RSA_KEY_CHECK_FAILED" },
        { -(MBEDTLS_ERR_RSA_PUBLIC_FAILED), "RSA_PUBLIC_FAILED" },
        { -(MBEDTLS_ERR_RSA_PRIVATE_FAILED), "RSA_PRIVATE_FAILED" },
        { -(MBEDTLS_ERR_RSA_RNG_FAILED), "RSA_RNG_FAILED" },
#endif /* MBEDTLS_RSA_C */
// END generated code
};

static const struct ssl_errs mbedtls_low_level_error_tab[] = {
// Low level error codes
//
// BEGIN generated code
#if defined(MBEDTLS_NET_C)
    { -(MBEDTLS_ERR_NET_SOCKET_FAILED), "NET_SOCKET_FAILED" },
    { -(MBEDTLS_ERR_NET_CONNECT_FAILED), "NET_CONNECT_FAILED" },
    { -(MBEDTLS_ERR_NET_BIND_FAILED), "NET_BIND_FAILED" },
    { -(MBEDTLS_ERR_NET_LISTEN_FAILED), "NET_LISTEN_FAILED" },
    { -(MBEDTLS_ERR_NET_ACCEPT_FAILED), "NET_ACCEPT_FAILED" },
    { -(MBEDTLS_ERR_NET_RECV_FAILED), "NET_RECV_FAILED" },
    { -(MBEDTLS_ERR_NET_SEND_FAILED), "NET_SEND_FAILED" },
    { -(MBEDTLS_ERR_NET_CONN_RESET), "NET_CONN_RESET" },
    { -(MBEDTLS_ERR_NET_UNKNOWN_HOST), "NET_UNKNOWN_HOST" },
    { -(MBEDTLS_ERR_NET_INVALID_CONTEXT), "NET_INVALID_CONTEXT" },
    { -(MBEDTLS_ERR_NET_POLL_FAILED), "NET_POLL_FAILED" },
    { -(MBEDTLS_ERR_NET_BAD_INPUT_DATA), "NET_BAD_INPUT_DATA" },
#endif /* MBEDTLS_NET_C */

#if defined(MBEDTLS_AES_C)
    { -(MBEDTLS_ERR_AES_INVALID_KEY_LENGTH), "AES_INVALID_KEY_LENGTH" },
    { -(MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH), "AES_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_AES_C */

#if defined(MBEDTLS_ARIA_C)
    { -(MBEDTLS_ERR_ARIA_INVALID_INPUT_LENGTH), "ARIA_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_ARIA_C */

#if defined(MBEDTLS_BIGNUM_C)
    { -(MBEDTLS_ERR_MPI_FILE_IO_ERROR), "MPI_FILE_IO_ERROR" },
    { -(MBEDTLS_ERR_MPI_INVALID_CHARACTER), "MPI_INVALID_CHARACTER" },
    { -(MBEDTLS_ERR_MPI_NEGATIVE_VALUE), "MPI_NEGATIVE_VALUE" },
    { -(MBEDTLS_ERR_MPI_DIVISION_BY_ZERO), "MPI_DIVISION_BY_ZERO" },
    { -(MBEDTLS_ERR_MPI_NOT_ACCEPTABLE), "MPI_NOT_ACCEPTABLE" },
#endif /* MBEDTLS_BIGNUM_C */

#if defined(MBEDTLS_CAMELLIA_C)
    { -(MBEDTLS_ERR_CAMELLIA_INVALID_INPUT_LENGTH), "CAMELLIA_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_CAMELLIA_C */

#if defined(MBEDTLS_CHACHAPOLY_C)
    { -(MBEDTLS_ERR_CHACHAPOLY_BAD_STATE), "CHACHAPOLY_BAD_STATE" },
#endif /* MBEDTLS_CHACHAPOLY_C */

#if defined(MBEDTLS_CTR_DRBG_C)
    { -(MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED), "CTR_DRBG_ENTROPY_SOURCE_FAILED" },
    { -(MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG), "CTR_DRBG_REQUEST_TOO_BIG" },
    { -(MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG), "CTR_DRBG_INPUT_TOO_BIG" },
    { -(MBEDTLS_ERR_CTR_DRBG_FILE_IO_ERROR), "CTR_DRBG_FILE_IO_ERROR" },
#endif /* MBEDTLS_CTR_DRBG_C */

#if defined(MBEDTLS_ENTROPY_C)
    { -(MBEDTLS_ERR_ENTROPY_MAX_SOURCES), "ENTROPY_MAX_SOURCES" },
    { -(MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED), "ENTROPY_NO_SOURCES_DEFINED" },
    { -(MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE), "ENTROPY_NO_STRONG_SOURCE" },
    { -(MBEDTLS_ERR_ENTROPY_FILE_IO_ERROR), "ENTROPY_FILE_IO_ERROR" },
#endif /* MBEDTLS_ENTROPY_C */

#if defined(MBEDTLS_HMAC_DRBG_C)
    { -(MBEDTLS_ERR_HMAC_DRBG_REQUEST_TOO_BIG), "HMAC_DRBG_REQUEST_TOO_BIG" },
    { -(MBEDTLS_ERR_HMAC_DRBG_INPUT_TOO_BIG), "HMAC_DRBG_INPUT_TOO_BIG" },
    { -(MBEDTLS_ERR_HMAC_DRBG_FILE_IO_ERROR), "HMAC_DRBG_FILE_IO_ERROR" },
    { -(MBEDTLS_ERR_HMAC_DRBG_ENTROPY_SOURCE_FAILED), "HMAC_DRBG_ENTROPY_SOURCE_FAILED" },
#endif /* MBEDTLS_HMAC_DRBG_C */
// END generated code
};

static const char *mbedtls_err_prefix = "MBEDTLS_ERR_";
#define MBEDTLS_ERR_PREFIX_LEN ( sizeof("MBEDTLS_ERR_")-1 )

// copy error text into buffer, ensure null termination, return strlen of result
static size_t mbedtls_err_to_str(int err, const struct ssl_errs tab[], int tab_len, char *buf, size_t buflen) {
    if (buflen == 0) return 0;

    // prefix for all error names
    strncpy(buf, mbedtls_err_prefix, buflen);
    if (buflen <= MBEDTLS_ERR_PREFIX_LEN+1) {
        buf[buflen-1] = 0;
        return buflen-1;
    }

    // append error name from table
    for (int i = 0; i < tab_len; i++) {
        if (tab[i].errnum == err) {
            strncpy(buf+MBEDTLS_ERR_PREFIX_LEN, tab[i].errstr, buflen-MBEDTLS_ERR_PREFIX_LEN);
            buf[buflen-1] = 0;
            return strlen(buf);
        }
    }

    mbedtls_snprintf(buf+MBEDTLS_ERR_PREFIX_LEN, buflen-MBEDTLS_ERR_PREFIX_LEN, "UNKNOWN (0x%04X)",
            err);
    return strlen(buf);
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

void __wrap_mbedtls_strerror(int ret, char *buf, size_t buflen) {
    int use_ret;

    if (buflen == 0) return;

    buf[buflen-1] = 0;

    if (ret < 0) ret = -ret;

    //
    // High-level error codes
    //
    uint8_t got_hl = (ret & 0xFF80) != 0;
    if (got_hl) {
        use_ret = ret & 0xFF80;

        // special case, don't try to translate low level code
#if defined(MBEDTLS_SSL_TLS_C)
        if (use_ret == -(MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE)) {
            strncpy(buf, "MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE", buflen);
            buf[buflen-1] = 0;
            return;
        }
#endif

        size_t len = mbedtls_err_to_str(use_ret, mbedtls_high_level_error_tab,
                ARRAY_SIZE(mbedtls_high_level_error_tab), buf, buflen);

        buf += len;
        buflen -= len;
        if (buflen == 0) return;
    }

    //
    // Low-level error codes
    //
    use_ret = ret & ~0xFF80;

    if (use_ret == 0) return;

    // If high level code is present, make a concatenation between both error strings.
    if (got_hl) {
        if (buflen < 2) return;
        *buf++ = '+';
        buflen--;
    }

    mbedtls_err_to_str(use_ret, mbedtls_low_level_error_tab,
            ARRAY_SIZE(mbedtls_low_level_error_tab), buf, buflen);
}

#else /* MBEDTLS_ERROR_C */

#if defined(MBEDTLS_ERROR_STRERROR_DUMMY)

/*
 * Provide an non-function in case MBEDTLS_ERROR_C is not defined
 */
void __wrap_mbedtls_strerror( int ret, char *buf, size_t buflen )
{
    ((void) ret);

    if( buflen > 0 )
        buf[0] = '\0';
}

#endif /* MBEDTLS_ERROR_STRERROR_DUMMY */

#endif /* MBEDTLS_ERROR_C */
