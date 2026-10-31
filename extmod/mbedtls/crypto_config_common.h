#ifndef MICROPY_INCLUDED_MBEDTLS_CRYPTO_CONFIG_COMMON_H
#define MICROPY_INCLUDED_MBEDTLS_CRYPTO_CONFIG_COMMON_H

// A port may enable this option in their crypto_config_port.h
// to select additional bare-metal configuration.
#if MICROPY_MBEDTLS_CONFIG_BARE_METAL

// Bare-metal crypto lib configuration
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

// Bare-metal memory allocation hooks.
#include <stdlib.h>
#include <stdio.h>
void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr);
#define MBEDTLS_PLATFORM_STD_CALLOC m_tracked_calloc
#define MBEDTLS_PLATFORM_STD_FREE m_tracked_free
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf

// Default mbedtls_ms_time() function name
#ifndef MBEDTLS_PLATFORM_MS_TIME_ALT
#define MBEDTLS_PLATFORM_MS_TIME_ALT mbedtls_ms_time
#endif

// mbedTLS options for bare metal, that apply to the psa crypto library not mbedTLS
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_HAVE_ASM

#endif // MICROPY_MBEDTLS_CONFIG_BARE_METAL

// These mbedTLS options apply to the psa crypto library not mbedTLS, despite the name!
// See mbedtls/tf-psa-crypto/include/crypto_config.h for full list
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_DEPRECATED_REMOVED
#define MBEDTLS_ERROR_C
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_MD_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_HAVE_ECC_KEYS
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_SHA256_SMALLER

// Necessary to allow using legacy AES API in modcryptolib.c, can remove after that file is updated
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

// PSA-specific config options
#define PSA_WANT_ALG_AES                        1
#define PSA_WANT_ALG_CBC_NO_PADDING             1
#define PSA_WANT_ALG_CBC_PKCS7                  1 // TODO: check if we had this one before or not
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA        1
#define PSA_WANT_ALG_ECDH                       1
#define PSA_WANT_ALG_ECDSA                      1
#define PSA_WANT_ALG_GCM                        1
#define PSA_WANT_ALG_MD5                        1
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT         1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN          1
#define PSA_WANT_ALG_SHA_1                      1
#define PSA_WANT_ALG_SHA_224                    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_SHA_384                    1
#define PSA_WANT_ALG_SHA_512                    1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS            1
#define PSA_WANT_ECC_SECP_K1_256                1
#define PSA_WANT_ECC_SECP_R1_256                1
#define PSA_WANT_ECC_SECP_R1_384                1
#define PSA_WANT_ECC_SECP_R1_521                1
#define PSA_WANT_KEY_TYPE_AES 1
#define PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE  1
#define PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY         1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_HMAC                  1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY        1

#endif // MICROPY_INCLUDED_MBEDTLS_CRYPTO_CONFIG_COMMON_H
