/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


#ifndef __AWS_CRYPTO__H__
#define __AWS_CRYPTO__H__

#include "FreeRTOS.h"

/**
 * @brief Commonly used buffer sizes for storing cryptographic hash computation
 * results.
 */
#define cryptoSHA1_DIGEST_BYTES      20
#define cryptoSHA256_DIGEST_BYTES    32

/**
 * @brief Configures crypto library heap callouts so that the FreeRTOS heap is
 * used instead of the C runtime heap. Skipping this call is likely to cause
 * runtime memory corruption in the application.
 */
void CRYPTO_ConfigureHeap( void );

/**
 * @brief Library-independent cryptographic algorithm identifiers.
 */
#define cryptoHASH_ALGORITHM_SHA1           1
#define cryptoHASH_ALGORITHM_SHA256         2
#define cryptoASYMMETRIC_ALGORITHM_RSA      1
#define cryptoASYMMETRIC_ALGORITHM_ECDSA    2

/**
 * @brief Initializes digital signature verification.
 *
 * @param[out] ppvContext Opaque context structure.
 * @param[in] xAsymmetricAlgorithm Cryptographic public key cryptosystem.
 * @param[in] xHashAlgorithm Cryptographic hash algorithm that was used for signing.
 *
 * @return pdTRUE if initialization succeeds, or pdFALSE otherwise.
 */
BaseType_t CRYPTO_SignatureVerificationStart( void ** ppvContext,
                                              BaseType_t xAsymmetricAlgorithm,
                                              BaseType_t xHashAlgorithm );

/**
 * @brief Updates a cryptographic hash computation with the specified byte array.
 *
 * @param[in] pvContext Opaque context structure.
 * @param[in] pucData Byte array that was signed.
 * @param[in] xDataLength Length in bytes of data that was signed.
 */
void CRYPTO_SignatureVerificationUpdate( void * pvContext,
                                         const uint8_t * pucData,
                                         size_t xDataLength );

/**
 * @brief Verifies a digital signature computation using the public key from the
 * specified certificate.
 *
 * @param[in] pvContext Opaque context structure.
 * @param[in] pucSignerCertificate Base64 and DER encoded X.509 certificate of the
 * signer.
 * @param[in] xSignerCertificateLength Length in bytes of the certificate.
 * @param[in] pucSignature Digital signature result to verify.
 * @param[in] xSignatureLength in bytes of digital signature result.
 *
 * @return pdTRUE if the signature is correct or pdFALSE if the signature is invalid.
 */
BaseType_t CRYPTO_SignatureVerificationFinal( void * pvContext,
                                              char * pcSignerCertificate,
                                              size_t xSignerCertificateLength,
                                              uint8_t * pucSignature,
                                              size_t xSignatureLength );

#endif /* ifndef __AWS_CRYPTO__H__ */
