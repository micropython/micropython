/*
 * Amazon FreeRTOS Demo Bootloader V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_boot_crypto.h
 * @brief Boot crypto header.
 */

#ifndef _AWS_BOOT_CRYPTO_H_
#define _AWS_BOOT_CRYPTO_H_

/* Standard includes.*/
#include <stdint.h>

/* Bootloader includes.*/
#include "aws_boot_types.h"

/* Size of the ECC public key in bytes.*/
#define ECC_PUBKEY_BIT_STRING_SIZE      ( 64U )

/* Number of bytes to represent an component (R,S) of the the curve. */
#define ECC_NUM_BYTES_PER_SIG_COMPONENT (32U)

/* Number of components of signature. */
#define ECC_NUM_SIG_COMPONENTS (2U)

/**
 * Max size of ECC signature.
 */
#define BOOT_ECC_SIGNATURE_SIZE_MAX        ( 72UL )

/**
 * @brief Name of crypto signature.
 */
#define BOOT_CRYPTO_SIGNATURE_NAME         "sig-sha256-ecdsa"

/**
 * @brief Max size of crypto signature name.
 */
#define BOOT_CRYPTO_SIGNATURE_NAME_SIZE    ( 32U )

/**
 * @brief Max size of crypto signature in application trailer.
 */
#define BOOT_CRYPTO_SIGNATURE_SIZE         ( 256U )

/**
 * @brief Boot crypto module initialization.
 * @param[in] None.
 * @return pdTRUE if initialization succeeds, or pdFALSE otherwise.
 */
BaseType_t BOOT_CRYPTO_Init( void );

/**
 * @brief Verifies a cryptographic signature.
 * This verifies the cryptographic signature of the data block.
 * @param[in] pucData - points to start of the data
 * @param[in] ulSize - size of the data
 * @param[in] pucSignature -  points to ASN1 encoded crypto signature
 * @param[in] ulSignatureSize - size of the ASN1 encoded crypto signature
 * @return pdTRUE - if verification succeeds, or pdFALSE if it fails.
 */
BaseType_t BOOT_CRYPTO_Verify( const uint8_t * pucData,
                               uint32_t ulSize,
                               const uint8_t * pucSignature,
                               uint32_t ulSignatureSize );

#endif /* ifndef _AWS_BOOT_CRYPTO_H_ */
