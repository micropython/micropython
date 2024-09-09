#ifndef MICROPY_INCLUDED_MBEDTLS_CONFIG_BOARD_H
#define MICROPY_INCLUDED_MBEDTLS_CONFIG_BOARD_H

#define MBEDTLS_ECP_NIST_OPTIM

#include "ports/stm32/mbedtls/mbedtls_config_port.h"

#if MICROPY_PY_PLUGNTRUST
// The SE05x library uses kSE05x_ECSignatureAlgo_SHA_256 by default in
// its mbedtls hooks, so we must force SHA256 otherwise signing fails.
#undef MBEDTLS_SHA384_C
#undef MBEDTLS_SHA512_C

#define MBEDTLS_ECDSA_SIGN_ALT
// Only available in the NXP mini package.
// #define MBEDTLS_ECDSA_VERIFY_ALT

// Enable alternate signing on SE05x.
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#endif

#endif /* MICROPY_INCLUDED_MBEDTLS_CONFIG_BOARD_H */
