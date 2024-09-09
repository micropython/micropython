#include "mbedtls/mbedtls_config.h"
#ifndef MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED
#define MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED -0x006E
#endif
#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#define mbedtls_ecdsa_can_do mbedtls_ecdsa_can_do_private
