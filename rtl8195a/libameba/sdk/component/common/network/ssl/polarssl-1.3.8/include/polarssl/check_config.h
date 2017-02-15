/**
 * \file check_config.h
 *
 * \brief Consistency checks for configuration options
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * It is recommended to include this file from your config.h
 * in order to catch dependency issues early.
 */

#ifndef POLARSSL_CHECK_CONFIG_H
#define POLARSSL_CHECK_CONFIG_H

#if defined(POLARSSL_AESNI_C) && !defined(POLARSSL_HAVE_ASM)
#error "POLARSSL_AESNI_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_CERTS_C) && !defined(POLARSSL_PEM_PARSE_C)
#error "POLARSSL_CERTS_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_CTR_DRBG_C) && !defined(POLARSSL_AES_C)
#error "POLARSSL_CTR_DRBG_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_DHM_C) && !defined(POLARSSL_BIGNUM_C)
#error "POLARSSL_DHM_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_ECDH_C) && !defined(POLARSSL_ECP_C)
#error "POLARSSL_ECDH_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_ECDSA_C) &&            \
    ( !defined(POLARSSL_ECP_C) ||           \
      !defined(POLARSSL_ASN1_PARSE_C) ||    \
      !defined(POLARSSL_ASN1_WRITE_C) )
#error "POLARSSL_ECDSA_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_ECDSA_DETERMINISTIC) && !defined(POLARSSL_HMAC_DRBG_C)
#error "POLARSSL_ECDSA_DETERMINISTIC defined, but not all prerequisites"
#endif

#if defined(POLARSSL_ECP_C) && ( !defined(POLARSSL_BIGNUM_C) || (   \
    !defined(POLARSSL_ECP_DP_SECP192R1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP224R1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP256R1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP384R1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP521R1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_BP256R1_ENABLED)   &&                  \
    !defined(POLARSSL_ECP_DP_BP384R1_ENABLED)   &&                  \
    !defined(POLARSSL_ECP_DP_BP512R1_ENABLED)   &&                  \
    !defined(POLARSSL_ECP_DP_SECP192K1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP224K1_ENABLED) &&                  \
    !defined(POLARSSL_ECP_DP_SECP256K1_ENABLED) ) )
#error "POLARSSL_ECP_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_ENTROPY_C) && (!defined(POLARSSL_SHA512_C) &&      \
                                    !defined(POLARSSL_SHA256_C))
#error "POLARSSL_ENTROPY_C defined, but not all prerequisites"
#endif
#if defined(POLARSSL_ENTROPY_C) && defined(POLARSSL_SHA512_C) &&         \
    defined(CTR_DRBG_ENTROPY_LEN) && (CTR_DRBG_ENTROPY_LEN > 64)
#error "CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(POLARSSL_ENTROPY_C) &&                                            \
    ( !defined(POLARSSL_SHA512_C) || defined(POLARSSL_ENTROPY_FORCE_SHA256) ) \
    && defined(CTR_DRBG_ENTROPY_LEN) && (CTR_DRBG_ENTROPY_LEN > 32)
#error "CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(POLARSSL_ENTROPY_C) && \
    defined(POLARSSL_ENTROPY_FORCE_SHA256) && !defined(POLARSSL_SHA256_C)
#error "POLARSSL_ENTROPY_FORCE_SHA256 defined, but not all prerequisites"
#endif

#if defined(POLARSSL_GCM_C) && (                                        \
        !defined(POLARSSL_AES_C) && !defined(POLARSSL_CAMELLIA_C) )
#error "POLARSSL_GCM_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_HAVEGE_C) && !defined(POLARSSL_TIMING_C)
#error "POLARSSL_HAVEGE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_HMAC_DRBG) && !defined(POLARSSL_MD_C)
#error "POLARSSL_HMAC_DRBG_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED) &&                 \
    ( !defined(POLARSSL_ECDH_C) || !defined(POLARSSL_X509_CRT_PARSE_C) )
#error "POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED) &&                 \
    ( !defined(POLARSSL_ECDH_C) || !defined(POLARSSL_X509_CRT_PARSE_C) )
#error "POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED) && !defined(POLARSSL_DHM_C)
#error "POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED) &&                     \
    !defined(POLARSSL_ECDH_C)
#error "POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED) &&                   \
    ( !defined(POLARSSL_DHM_C) || !defined(POLARSSL_RSA_C) ||           \
      !defined(POLARSSL_X509_CRT_PARSE_C) || !defined(POLARSSL_PKCS1_V15) )
#error "POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED) &&                 \
    ( !defined(POLARSSL_ECDH_C) || !defined(POLARSSL_RSA_C) ||          \
      !defined(POLARSSL_X509_CRT_PARSE_C) || !defined(POLARSSL_PKCS1_V15) )
#error "POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) &&                 \
    ( !defined(POLARSSL_ECDH_C) || !defined(POLARSSL_ECDSA_C) ||          \
      !defined(POLARSSL_X509_CRT_PARSE_C) )
#error "POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED) &&                   \
    ( !defined(POLARSSL_RSA_C) || !defined(POLARSSL_X509_CRT_PARSE_C) ||\
      !defined(POLARSSL_PKCS1_V15) )
#error "POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_KEY_EXCHANGE_RSA_ENABLED) &&                       \
    ( !defined(POLARSSL_RSA_C) || !defined(POLARSSL_X509_CRT_PARSE_C) ||\
      !defined(POLARSSL_PKCS1_V15) )
#error "POLARSSL_KEY_EXCHANGE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(POLARSSL_MEMORY_BUFFER_ALLOC_C) &&                          \
    ( !defined(POLARSSL_PLATFORM_C) || !defined(POLARSSL_PLATFORM_MEMORY) )
#error "POLARSSL_MEMORY_BUFFER_ALLOC_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PADLOCK_C) && !defined(POLARSSL_HAVE_ASM)
#error "POLARSSL_PADLOCK_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PBKDF2_C) && !defined(POLARSSL_MD_C)
#error "POLARSSL_PBKDF2_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PEM_PARSE_C) && !defined(POLARSSL_BASE64_C)
#error "POLARSSL_PEM_PARSE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PEM_WRITE_C) && !defined(POLARSSL_BASE64_C)
#error "POLARSSL_PEM_WRITE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PK_PARSE_C) && !defined(POLARSSL_PK_C)
#error "POLARSSL_PK_PARSE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PK_WRITE_C) && !defined(POLARSSL_PK_C)
#error "POLARSSL_PK_WRITE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_PKCS11_C) && !defined(POLARSSL_PK_C)
#error "POLARSSL_PKCS11_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_RSA_C) && ( !defined(POLARSSL_BIGNUM_C) ||         \
    !defined(POLARSSL_OID_C) )
#error "POLARSSL_RSA_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_RSASSA_PSS_SUPPORT) &&                        \
    ( !defined(POLARSSL_RSA_C) || !defined(POLARSSL_PKCS1_V21) )
#error "POLARSSL_X509_RSASSA_PSS_SUPPORT defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_PROTO_SSL3) && ( !defined(POLARSSL_MD5_C) ||     \
    !defined(POLARSSL_SHA1_C) )
#error "POLARSSL_SSL_PROTO_SSL3 defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_PROTO_TLS1) && ( !defined(POLARSSL_MD5_C) ||     \
    !defined(POLARSSL_SHA1_C) )
#error "POLARSSL_SSL_PROTO_TLS1 defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_PROTO_TLS1_1) && ( !defined(POLARSSL_MD5_C) ||     \
    !defined(POLARSSL_SHA1_C) )
#error "POLARSSL_SSL_PROTO_TLS1_1 defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_PROTO_TLS1_2) && ( !defined(POLARSSL_SHA1_C) &&     \
    !defined(POLARSSL_SHA256_C) && !defined(POLARSSL_SHA512_C) )
#error "POLARSSL_SSL_PROTO_TLS1_2 defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_CLI_C) && !defined(POLARSSL_SSL_TLS_C)
#error "POLARSSL_SSL_CLI_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_TLS_C) && ( !defined(POLARSSL_CIPHER_C) ||     \
    !defined(POLARSSL_MD_C) )
#error "POLARSSL_SSL_TLS_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_SRV_C) && !defined(POLARSSL_SSL_TLS_C)
#error "POLARSSL_SSL_SRV_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_TLS_C) && (!defined(POLARSSL_SSL_PROTO_SSL3) && \
    !defined(POLARSSL_SSL_PROTO_TLS1) && !defined(POLARSSL_SSL_PROTO_TLS1_1) && \
    !defined(POLARSSL_SSL_PROTO_TLS1_2))
#error "POLARSSL_SSL_TLS_C defined, but no protocols are active"
#endif

#if defined(POLARSSL_SSL_TLS_C) && (defined(POLARSSL_SSL_PROTO_SSL3) && \
    defined(POLARSSL_SSL_PROTO_TLS1_1) && !defined(POLARSSL_SSL_PROTO_TLS1))
#error "Illegal protocol selection"
#endif

#if defined(POLARSSL_SSL_TLS_C) && (defined(POLARSSL_SSL_PROTO_TLS1) && \
    defined(POLARSSL_SSL_PROTO_TLS1_2) && !defined(POLARSSL_SSL_PROTO_TLS1_1))
#error "Illegal protocol selection"
#endif

#if defined(POLARSSL_SSL_TLS_C) && (defined(POLARSSL_SSL_PROTO_SSL3) && \
    defined(POLARSSL_SSL_PROTO_TLS1_2) && (!defined(POLARSSL_SSL_PROTO_TLS1) || \
    !defined(POLARSSL_SSL_PROTO_TLS1_1)))
#error "Illegal protocol selection"
#endif

#if defined(POLARSSL_SSL_SESSION_TICKETS) && defined(POLARSSL_SSL_TLS_C) && \
    ( !defined(POLARSSL_AES_C) || !defined(POLARSSL_SHA256_C) ||            \
      !defined(POLARSSL_CIPHER_MODE_CBC) )
#error "POLARSSL_SSL_SESSION_TICKETS_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION) && \
        !defined(POLARSSL_X509_CRT_PARSE_C)
#error "POLARSSL_SSL_SERVER_NAME_INDICATION defined, but not all prerequisites"
#endif

#if defined(POLARSSL_THREADING_PTHREAD)
#if !defined(POLARSSL_THREADING_C) || defined(POLARSSL_THREADING_IMPL)
#error "POLARSSL_THREADING_PTHREAD defined, but not all prerequisites"
#endif
#define POLARSSL_THREADING_IMPL
#endif

#if defined(POLARSSL_THREADING_ALT)
#if !defined(POLARSSL_THREADING_C) || defined(POLARSSL_THREADING_IMPL)
#error "POLARSSL_THREADING_ALT defined, but not all prerequisites"
#endif
#define POLARSSL_THREADING_IMPL
#endif

#if defined(POLARSSL_THREADING_C) && !defined(POLARSSL_THREADING_IMPL)
#error "POLARSSL_THREADING_C defined, single threading implementation required"
#endif
#undef POLARSSL_THREADING_IMPL

#if defined(POLARSSL_VERSION_FEATURES) && !defined(POLARSSL_VERSION_C)
#error "POLARSSL_VERSION_FEATURES defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_USE_C) && ( !defined(POLARSSL_BIGNUM_C) ||  \
    !defined(POLARSSL_OID_C) || !defined(POLARSSL_ASN1_PARSE_C) ||      \
    !defined(POLARSSL_PK_PARSE_C) )
#error "POLARSSL_X509_USE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CREATE_C) && ( !defined(POLARSSL_BIGNUM_C) ||  \
    !defined(POLARSSL_OID_C) || !defined(POLARSSL_ASN1_WRITE_C) ||       \
    !defined(POLARSSL_PK_WRITE_C) )
#error "POLARSSL_X509_CREATE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C) && ( !defined(POLARSSL_X509_USE_C) )
#error "POLARSSL_X509_CRT_PARSE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CRL_PARSE_C) && ( !defined(POLARSSL_X509_USE_C) )
#error "POLARSSL_X509_CRL_PARSE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CSR_PARSE_C) && ( !defined(POLARSSL_X509_USE_C) )
#error "POLARSSL_X509_CSR_PARSE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CRT_WRITE_C) && ( !defined(POLARSSL_X509_CREATE_C) )
#error "POLARSSL_X509_CRT_WRITE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_X509_CSR_WRITE_C) && ( !defined(POLARSSL_X509_CREATE_C) )
#error "POLARSSL_X509_CSR_WRITE_C defined, but not all prerequisites"
#endif

#if defined(POLARSSL_AES_C) || defined(POLARSSL_AES_C)
#if !defined(RTL_HW_CRYPTO)
#error "POLARSSL_AES_C or POLARSSL_AES_C defined, RTL_HW_CRYPTO required"
#endif
#endif

#if defined(SUPPORT_HW_SW_CRYPTO) && !defined(RTL_HW_CRYPTO)
#error "SUPPORT_HW_SW_CRYPTO defined, RTL_HW_CRYPTO required"
#endif

#endif /* POLARSSL_CHECK_CONFIG_H */
