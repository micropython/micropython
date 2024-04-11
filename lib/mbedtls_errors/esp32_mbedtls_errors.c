/*
 *  Error message information
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

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

#if defined(MBEDTLS_AES_C)
#include "mbedtls/aes.h"
#endif

#if defined(MBEDTLS_ARIA_C)
#include "mbedtls/aria.h"
#endif

#if defined(MBEDTLS_ASN1_PARSE_C)
#include "mbedtls/asn1.h"
#endif

#if defined(MBEDTLS_BASE64_C)
#include "mbedtls/base64.h"
#endif

#if defined(MBEDTLS_BIGNUM_C)
#include "mbedtls/bignum.h"
#endif

#if defined(MBEDTLS_CAMELLIA_C)
#include "mbedtls/camellia.h"
#endif

#if defined(MBEDTLS_CCM_C)
#include "mbedtls/ccm.h"
#endif

#if defined(MBEDTLS_CHACHA20_C)
#include "mbedtls/chacha20.h"
#endif

#if defined(MBEDTLS_CHACHAPOLY_C)
#include "mbedtls/chachapoly.h"
#endif

#if defined(MBEDTLS_CIPHER_C)
#include "mbedtls/cipher.h"
#endif

#if defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/ctr_drbg.h"
#endif

#if defined(MBEDTLS_DES_C)
#include "mbedtls/des.h"
#endif

#if defined(MBEDTLS_DHM_C)
#include "mbedtls/dhm.h"
#endif

#if defined(MBEDTLS_ECP_C)
#include "mbedtls/ecp.h"
#endif

#if defined(MBEDTLS_ENTROPY_C)
#include "mbedtls/entropy.h"
#endif

#if defined(MBEDTLS_ERROR_C)
#include "mbedtls/error.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#if defined(MBEDTLS_GCM_C)
#include "mbedtls/gcm.h"
#endif

#if defined(MBEDTLS_HKDF_C)
#include "mbedtls/hkdf.h"
#endif

#if defined(MBEDTLS_HMAC_DRBG_C)
#include "mbedtls/hmac_drbg.h"
#endif

#if defined(MBEDTLS_LMS_C)
#include "mbedtls/lms.h"
#endif

#if defined(MBEDTLS_MD_C)
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_NET_C)
#include "mbedtls/net_sockets.h"
#endif

#if defined(MBEDTLS_OID_C)
#include "mbedtls/oid.h"
#endif

#if defined(MBEDTLS_PEM_PARSE_C) || defined(MBEDTLS_PEM_WRITE_C)
#include "mbedtls/pem.h"
#endif

#if defined(MBEDTLS_PK_C)
#include "mbedtls/pk.h"
#endif

#if defined(MBEDTLS_PKCS12_C)
#include "mbedtls/pkcs12.h"
#endif

#if defined(MBEDTLS_PKCS5_C)
#include "mbedtls/pkcs5.h"
#endif

#if defined(MBEDTLS_PKCS7_C)
#include "mbedtls/pkcs7.h"
#endif

#if defined(MBEDTLS_POLY1305_C)
#include "mbedtls/poly1305.h"
#endif

#if defined(MBEDTLS_RSA_C)
#include "mbedtls/rsa.h"
#endif

#if defined(MBEDTLS_SHA1_C)
#include "mbedtls/sha1.h"
#endif

#if defined(MBEDTLS_SHA256_C)
#include "mbedtls/sha256.h"
#endif

#if defined(MBEDTLS_SHA512_C)
#include "mbedtls/sha512.h"
#endif

#if defined(MBEDTLS_SSL_TLS_C)
#include "mbedtls/ssl.h"
#endif

#if defined(MBEDTLS_THREADING_C)
#include "mbedtls/threading.h"
#endif

#if defined(MBEDTLS_X509_USE_C) || defined(MBEDTLS_X509_CREATE_C)
#include "mbedtls/x509.h"
#endif


// Error code table type
struct ssl_errs {
    int16_t errnum;
    const char *errstr;
};

// Table of high level error codes
static const struct ssl_errs mbedtls_high_level_error_tab[] = {
// BEGIN generated code
#if defined(MBEDTLS_CIPHER_C)
        { -(MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE), "CIPHER_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA), "CIPHER_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_CIPHER_ALLOC_FAILED), "CIPHER_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_CIPHER_INVALID_PADDING), "CIPHER_INVALID_PADDING" },
        { -(MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED), "CIPHER_FULL_BLOCK_EXPECTED" },
        { -(MBEDTLS_ERR_CIPHER_AUTH_FAILED), "CIPHER_AUTH_FAILED" },
        { -(MBEDTLS_ERR_CIPHER_INVALID_CONTEXT), "CIPHER_INVALID_CONTEXT" },
#endif /* MBEDTLS_CIPHER_C */

#if defined(MBEDTLS_DHM_C)
        { -(MBEDTLS_ERR_DHM_BAD_INPUT_DATA), "DHM_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_DHM_READ_PARAMS_FAILED), "DHM_READ_PARAMS_FAILED" },
        { -(MBEDTLS_ERR_DHM_MAKE_PARAMS_FAILED), "DHM_MAKE_PARAMS_FAILED" },
        { -(MBEDTLS_ERR_DHM_READ_PUBLIC_FAILED), "DHM_READ_PUBLIC_FAILED" },
        { -(MBEDTLS_ERR_DHM_MAKE_PUBLIC_FAILED), "DHM_MAKE_PUBLIC_FAILED" },
        { -(MBEDTLS_ERR_DHM_CALC_SECRET_FAILED), "DHM_CALC_SECRET_FAILED" },
        { -(MBEDTLS_ERR_DHM_INVALID_FORMAT), "DHM_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_DHM_ALLOC_FAILED), "DHM_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_DHM_FILE_IO_ERROR), "DHM_FILE_IO_ERROR" },
        { -(MBEDTLS_ERR_DHM_SET_GROUP_FAILED), "DHM_SET_GROUP_FAILED" },
#endif /* MBEDTLS_DHM_C */

#if defined(MBEDTLS_ECP_C)
        { -(MBEDTLS_ERR_ECP_BAD_INPUT_DATA), "ECP_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL), "ECP_BUFFER_TOO_SMALL" },
        { -(MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE), "ECP_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_ECP_VERIFY_FAILED), "ECP_VERIFY_FAILED" },
        { -(MBEDTLS_ERR_ECP_ALLOC_FAILED), "ECP_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_ECP_RANDOM_FAILED), "ECP_RANDOM_FAILED" },
        { -(MBEDTLS_ERR_ECP_INVALID_KEY), "ECP_INVALID_KEY" },
        { -(MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH), "ECP_SIG_LEN_MISMATCH" },
        { -(MBEDTLS_ERR_ECP_IN_PROGRESS), "ECP_IN_PROGRESS" },
#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_MD_C)
        { -(MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE), "MD_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_MD_BAD_INPUT_DATA), "MD_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_MD_ALLOC_FAILED), "MD_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_MD_FILE_IO_ERROR), "MD_FILE_IO_ERROR" },
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_PEM_PARSE_C) || defined(MBEDTLS_PEM_WRITE_C)
        { -(MBEDTLS_ERR_PEM_NO_HEADER_FOOTER_PRESENT), "PEM_NO_HEADER_FOOTER_PRESENT" },
        { -(MBEDTLS_ERR_PEM_INVALID_DATA), "PEM_INVALID_DATA" },
        { -(MBEDTLS_ERR_PEM_ALLOC_FAILED), "PEM_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_PEM_INVALID_ENC_IV), "PEM_INVALID_ENC_IV" },
        { -(MBEDTLS_ERR_PEM_UNKNOWN_ENC_ALG), "PEM_UNKNOWN_ENC_ALG" },
        { -(MBEDTLS_ERR_PEM_PASSWORD_REQUIRED), "PEM_PASSWORD_REQUIRED" },
        { -(MBEDTLS_ERR_PEM_PASSWORD_MISMATCH), "PEM_PASSWORD_MISMATCH" },
        { -(MBEDTLS_ERR_PEM_FEATURE_UNAVAILABLE), "PEM_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PEM_BAD_INPUT_DATA), "PEM_BAD_INPUT_DATA" },
#endif /* MBEDTLS_PEM_PARSE_C || MBEDTLS_PEM_WRITE_C */

#if defined(MBEDTLS_PK_C)
        { -(MBEDTLS_ERR_PK_ALLOC_FAILED), "PK_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_PK_TYPE_MISMATCH), "PK_TYPE_MISMATCH" },
        { -(MBEDTLS_ERR_PK_BAD_INPUT_DATA), "PK_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_PK_FILE_IO_ERROR), "PK_FILE_IO_ERROR" },
        { -(MBEDTLS_ERR_PK_KEY_INVALID_VERSION), "PK_KEY_INVALID_VERSION" },
        { -(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT), "PK_KEY_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PK_UNKNOWN_PK_ALG), "PK_UNKNOWN_PK_ALG" },
        { -(MBEDTLS_ERR_PK_PASSWORD_REQUIRED), "PK_PASSWORD_REQUIRED" },
        { -(MBEDTLS_ERR_PK_PASSWORD_MISMATCH), "PK_PASSWORD_MISMATCH" },
        { -(MBEDTLS_ERR_PK_INVALID_PUBKEY), "PK_INVALID_PUBKEY" },
        { -(MBEDTLS_ERR_PK_INVALID_ALG), "PK_INVALID_ALG" },
        { -(MBEDTLS_ERR_PK_UNKNOWN_NAMED_CURVE), "PK_UNKNOWN_NAMED_CURVE" },
        { -(MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE), "PK_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PK_SIG_LEN_MISMATCH), "PK_SIG_LEN_MISMATCH" },
        { -(MBEDTLS_ERR_PK_BUFFER_TOO_SMALL), "PK_BUFFER_TOO_SMALL" },
#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_PKCS12_C)
        { -(MBEDTLS_ERR_PKCS12_BAD_INPUT_DATA), "PKCS12_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_PKCS12_FEATURE_UNAVAILABLE), "PKCS12_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PKCS12_PBE_INVALID_FORMAT), "PKCS12_PBE_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PKCS12_PASSWORD_MISMATCH), "PKCS12_PASSWORD_MISMATCH" },
#endif /* MBEDTLS_PKCS12_C */

#if defined(MBEDTLS_PKCS5_C)
        { -(MBEDTLS_ERR_PKCS5_BAD_INPUT_DATA), "PKCS5_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_PKCS5_INVALID_FORMAT), "PKCS5_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PKCS5_FEATURE_UNAVAILABLE), "PKCS5_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PKCS5_PASSWORD_MISMATCH), "PKCS5_PASSWORD_MISMATCH" },
#endif /* MBEDTLS_PKCS5_C */

#if defined(MBEDTLS_PKCS7_C)
        { -(MBEDTLS_ERR_PKCS7_INVALID_FORMAT), "PKCS7_INVALID_FORMAT" },
        { -(MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE), "PKCS7_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_VERSION), "PKCS7_INVALID_VERSION" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_CONTENT_INFO), "PKCS7_INVALID_CONTENT_INFO" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_ALG), "PKCS7_INVALID_ALG" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_CERT), "PKCS7_INVALID_CERT" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_SIGNATURE), "PKCS7_INVALID_SIGNATURE" },
        { -(MBEDTLS_ERR_PKCS7_INVALID_SIGNER_INFO), "PKCS7_INVALID_SIGNER_INFO" },
        { -(MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA), "PKCS7_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_PKCS7_ALLOC_FAILED), "PKCS7_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_PKCS7_VERIFY_FAIL), "PKCS7_VERIFY_FAIL" },
        { -(MBEDTLS_ERR_PKCS7_CERT_DATE_INVALID), "PKCS7_CERT_DATE_INVALID" },
#endif /* MBEDTLS_PKCS7_C */

#if defined(MBEDTLS_RSA_C)
        { -(MBEDTLS_ERR_RSA_BAD_INPUT_DATA), "RSA_BAD_INPUT_DATA" },
        { -(MBEDTLS_ERR_RSA_INVALID_PADDING), "RSA_INVALID_PADDING" },
        { -(MBEDTLS_ERR_RSA_KEY_GEN_FAILED), "RSA_KEY_GEN_FAILED" },
        { -(MBEDTLS_ERR_RSA_KEY_CHECK_FAILED), "RSA_KEY_CHECK_FAILED" },
        { -(MBEDTLS_ERR_RSA_PUBLIC_FAILED), "RSA_PUBLIC_FAILED" },
        { -(MBEDTLS_ERR_RSA_PRIVATE_FAILED), "RSA_PRIVATE_FAILED" },
        { -(MBEDTLS_ERR_RSA_VERIFY_FAILED), "RSA_VERIFY_FAILED" },
        { -(MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE), "RSA_OUTPUT_TOO_LARGE" },
        { -(MBEDTLS_ERR_RSA_RNG_FAILED), "RSA_RNG_FAILED" },
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_SSL_TLS_C)
        { -(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS), "SSL_CRYPTO_IN_PROGRESS" },
        { -(MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE), "SSL_FEATURE_UNAVAILABLE" },
        { -(MBEDTLS_ERR_SSL_BAD_INPUT_DATA), "SSL_BAD_INPUT_DATA" },
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
        { -(MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA), "SSL_CANNOT_WRITE_EARLY_DATA" },
        { -(MBEDTLS_ERR_SSL_ALLOC_FAILED), "SSL_ALLOC_FAILED" },
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
        { -(MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL), "SSL_BUFFER_TOO_SMALL" },
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
#endif /* MBEDTLS_SSL_TLS_C */

#if defined(MBEDTLS_X509_USE_C) || defined(MBEDTLS_X509_CREATE_C)
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
        { -(MBEDTLS_ERR_X509_ALLOC_FAILED), "X509_ALLOC_FAILED" },
        { -(MBEDTLS_ERR_X509_FILE_IO_ERROR), "X509_FILE_IO_ERROR" },
        { -(MBEDTLS_ERR_X509_BUFFER_TOO_SMALL), "X509_BUFFER_TOO_SMALL" },
        { -(MBEDTLS_ERR_X509_FATAL_ERROR), "X509_FATAL_ERROR" },
#endif /* MBEDTLS_X509_USE_C || MBEDTLS_X509_CREATE_C */
// END generated code
};

static const struct ssl_errs mbedtls_low_level_error_tab[] = {
// Low level error codes
//
// BEGIN generated code
#if defined(MBEDTLS_AES_C)
    { -(MBEDTLS_ERR_AES_INVALID_KEY_LENGTH), "AES_INVALID_KEY_LENGTH" },
    { -(MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH), "AES_INVALID_INPUT_LENGTH" },
    { -(MBEDTLS_ERR_AES_BAD_INPUT_DATA), "AES_BAD_INPUT_DATA" },
#endif /* MBEDTLS_AES_C */

#if defined(MBEDTLS_ARIA_C)
    { -(MBEDTLS_ERR_ARIA_BAD_INPUT_DATA), "ARIA_BAD_INPUT_DATA" },
    { -(MBEDTLS_ERR_ARIA_INVALID_INPUT_LENGTH), "ARIA_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_ARIA_C */

#if defined(MBEDTLS_ASN1_PARSE_C)
    { -(MBEDTLS_ERR_ASN1_OUT_OF_DATA), "ASN1_OUT_OF_DATA" },
    { -(MBEDTLS_ERR_ASN1_UNEXPECTED_TAG), "ASN1_UNEXPECTED_TAG" },
    { -(MBEDTLS_ERR_ASN1_INVALID_LENGTH), "ASN1_INVALID_LENGTH" },
    { -(MBEDTLS_ERR_ASN1_LENGTH_MISMATCH), "ASN1_LENGTH_MISMATCH" },
    { -(MBEDTLS_ERR_ASN1_INVALID_DATA), "ASN1_INVALID_DATA" },
    { -(MBEDTLS_ERR_ASN1_ALLOC_FAILED), "ASN1_ALLOC_FAILED" },
    { -(MBEDTLS_ERR_ASN1_BUF_TOO_SMALL), "ASN1_BUF_TOO_SMALL" },
#endif /* MBEDTLS_ASN1_PARSE_C */

#if defined(MBEDTLS_BASE64_C)
    { -(MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL), "BASE64_BUFFER_TOO_SMALL" },
    { -(MBEDTLS_ERR_BASE64_INVALID_CHARACTER), "BASE64_INVALID_CHARACTER" },
#endif /* MBEDTLS_BASE64_C */

#if defined(MBEDTLS_BIGNUM_C)
    { -(MBEDTLS_ERR_MPI_FILE_IO_ERROR), "MPI_FILE_IO_ERROR" },
    { -(MBEDTLS_ERR_MPI_BAD_INPUT_DATA), "MPI_BAD_INPUT_DATA" },
    { -(MBEDTLS_ERR_MPI_INVALID_CHARACTER), "MPI_INVALID_CHARACTER" },
    { -(MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL), "MPI_BUFFER_TOO_SMALL" },
    { -(MBEDTLS_ERR_MPI_NEGATIVE_VALUE), "MPI_NEGATIVE_VALUE" },
    { -(MBEDTLS_ERR_MPI_DIVISION_BY_ZERO), "MPI_DIVISION_BY_ZERO" },
    { -(MBEDTLS_ERR_MPI_NOT_ACCEPTABLE), "MPI_NOT_ACCEPTABLE" },
    { -(MBEDTLS_ERR_MPI_ALLOC_FAILED), "MPI_ALLOC_FAILED" },
#endif /* MBEDTLS_BIGNUM_C */

#if defined(MBEDTLS_CAMELLIA_C)
    { -(MBEDTLS_ERR_CAMELLIA_BAD_INPUT_DATA), "CAMELLIA_BAD_INPUT_DATA" },
    { -(MBEDTLS_ERR_CAMELLIA_INVALID_INPUT_LENGTH), "CAMELLIA_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_CAMELLIA_C */

#if defined(MBEDTLS_CCM_C)
    { -(MBEDTLS_ERR_CCM_BAD_INPUT), "CCM_BAD_INPUT" },
    { -(MBEDTLS_ERR_CCM_AUTH_FAILED), "CCM_AUTH_FAILED" },
#endif /* MBEDTLS_CCM_C */

#if defined(MBEDTLS_CHACHA20_C)
    { -(MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA), "CHACHA20_BAD_INPUT_DATA" },
#endif /* MBEDTLS_CHACHA20_C */

#if defined(MBEDTLS_CHACHAPOLY_C)
    { -(MBEDTLS_ERR_CHACHAPOLY_BAD_STATE), "CHACHAPOLY_BAD_STATE" },
    { -(MBEDTLS_ERR_CHACHAPOLY_AUTH_FAILED), "CHACHAPOLY_AUTH_FAILED" },
#endif /* MBEDTLS_CHACHAPOLY_C */

#if defined(MBEDTLS_CTR_DRBG_C)
    { -(MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED), "CTR_DRBG_ENTROPY_SOURCE_FAILED" },
    { -(MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG), "CTR_DRBG_REQUEST_TOO_BIG" },
    { -(MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG), "CTR_DRBG_INPUT_TOO_BIG" },
    { -(MBEDTLS_ERR_CTR_DRBG_FILE_IO_ERROR), "CTR_DRBG_FILE_IO_ERROR" },
#endif /* MBEDTLS_CTR_DRBG_C */

#if defined(MBEDTLS_DES_C)
    { -(MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH), "DES_INVALID_INPUT_LENGTH" },
#endif /* MBEDTLS_DES_C */

#if defined(MBEDTLS_ENTROPY_C)
    { -(MBEDTLS_ERR_ENTROPY_SOURCE_FAILED), "ENTROPY_SOURCE_FAILED" },
    { -(MBEDTLS_ERR_ENTROPY_MAX_SOURCES), "ENTROPY_MAX_SOURCES" },
    { -(MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED), "ENTROPY_NO_SOURCES_DEFINED" },
    { -(MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE), "ENTROPY_NO_STRONG_SOURCE" },
    { -(MBEDTLS_ERR_ENTROPY_FILE_IO_ERROR), "ENTROPY_FILE_IO_ERROR" },
#endif /* MBEDTLS_ENTROPY_C */

#if defined(MBEDTLS_ERROR_C)
    { -(MBEDTLS_ERR_ERROR_GENERIC_ERROR), "ERROR_GENERIC_ERROR" },
    { -(MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED), "ERROR_CORRUPTION_DETECTED" },
#endif /* MBEDTLS_ERROR_C */

#if defined(MBEDTLS_PLATFORM_C)
    { -(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED), "PLATFORM_HW_ACCEL_FAILED" },
    { -(MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED), "PLATFORM_FEATURE_UNSUPPORTED" },
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_GCM_C)
    { -(MBEDTLS_ERR_GCM_AUTH_FAILED), "GCM_AUTH_FAILED" },
    { -(MBEDTLS_ERR_GCM_BAD_INPUT), "GCM_BAD_INPUT" },
    { -(MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL), "GCM_BUFFER_TOO_SMALL" },
#endif /* MBEDTLS_GCM_C */

#if defined(MBEDTLS_HKDF_C)
    { -(MBEDTLS_ERR_HKDF_BAD_INPUT_DATA), "HKDF_BAD_INPUT_DATA" },
#endif /* MBEDTLS_HKDF_C */

#if defined(MBEDTLS_HMAC_DRBG_C)
    { -(MBEDTLS_ERR_HMAC_DRBG_REQUEST_TOO_BIG), "HMAC_DRBG_REQUEST_TOO_BIG" },
    { -(MBEDTLS_ERR_HMAC_DRBG_INPUT_TOO_BIG), "HMAC_DRBG_INPUT_TOO_BIG" },
    { -(MBEDTLS_ERR_HMAC_DRBG_FILE_IO_ERROR), "HMAC_DRBG_FILE_IO_ERROR" },
    { -(MBEDTLS_ERR_HMAC_DRBG_ENTROPY_SOURCE_FAILED), "HMAC_DRBG_ENTROPY_SOURCE_FAILED" },
#endif /* MBEDTLS_HMAC_DRBG_C */

#if defined(MBEDTLS_LMS_C)
    { -(MBEDTLS_ERR_LMS_BAD_INPUT_DATA), "LMS_BAD_INPUT_DATA" },
    { -(MBEDTLS_ERR_LMS_OUT_OF_PRIVATE_KEYS), "LMS_OUT_OF_PRIVATE_KEYS" },
    { -(MBEDTLS_ERR_LMS_VERIFY_FAILED), "LMS_VERIFY_FAILED" },
    { -(MBEDTLS_ERR_LMS_ALLOC_FAILED), "LMS_ALLOC_FAILED" },
    { -(MBEDTLS_ERR_LMS_BUFFER_TOO_SMALL), "LMS_BUFFER_TOO_SMALL" },
#endif /* MBEDTLS_LMS_C */

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
    { -(MBEDTLS_ERR_NET_BUFFER_TOO_SMALL), "NET_BUFFER_TOO_SMALL" },
    { -(MBEDTLS_ERR_NET_INVALID_CONTEXT), "NET_INVALID_CONTEXT" },
    { -(MBEDTLS_ERR_NET_POLL_FAILED), "NET_POLL_FAILED" },
    { -(MBEDTLS_ERR_NET_BAD_INPUT_DATA), "NET_BAD_INPUT_DATA" },
#endif /* MBEDTLS_NET_C */

#if defined(MBEDTLS_OID_C)
    { -(MBEDTLS_ERR_OID_NOT_FOUND), "OID_NOT_FOUND" },
    { -(MBEDTLS_ERR_OID_BUF_TOO_SMALL), "OID_BUF_TOO_SMALL" },
#endif /* MBEDTLS_OID_C */

#if defined(MBEDTLS_POLY1305_C)
    { -(MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA), "POLY1305_BAD_INPUT_DATA" },
#endif /* MBEDTLS_POLY1305_C */

#if defined(MBEDTLS_SHA1_C)
    { -(MBEDTLS_ERR_SHA1_BAD_INPUT_DATA), "SHA1_BAD_INPUT_DATA" },
#endif /* MBEDTLS_SHA1_C */

#if defined(MBEDTLS_SHA256_C)
    { -(MBEDTLS_ERR_SHA256_BAD_INPUT_DATA), "SHA256_BAD_INPUT_DATA" },
#endif /* MBEDTLS_SHA256_C */

#if defined(MBEDTLS_SHA512_C)
    { -(MBEDTLS_ERR_SHA512_BAD_INPUT_DATA), "SHA512_BAD_INPUT_DATA" },
#endif /* MBEDTLS_SHA512_C */

#if defined(MBEDTLS_THREADING_C)
    { -(MBEDTLS_ERR_THREADING_BAD_INPUT_DATA), "THREADING_BAD_INPUT_DATA" },
    { -(MBEDTLS_ERR_THREADING_MUTEX_ERROR), "THREADING_MUTEX_ERROR" },
#endif /* MBEDTLS_THREADING_C */
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

void mbedtls_strerror(int ret, char *buf, size_t buflen) {
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
void mbedtls_strerror( int ret, char *buf, size_t buflen )
{
    ((void) ret);

    if( buflen > 0 )
        buf[0] = '\0';
}

#endif /* MBEDTLS_ERROR_STRERROR_DUMMY */

#endif /* MBEDTLS_ERROR_C */
