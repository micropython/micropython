/*
 * Amazon FreeRTOS Crypto AFQP V1.1.4
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

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Crypto includes. */
#include "aws_crypto.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

TEST_GROUP( Full_CRYPTO );

TEST_SETUP( Full_CRYPTO )
{
}

TEST_TEAR_DOWN( Full_CRYPTO )
{
}

TEST_GROUP_RUNNER( Full_CRYPTO )
{
    RUN_TEST_CASE( Full_CRYPTO, VerifySignatureTestVectors );
}

TEST( Full_CRYPTO, VerifySignatureTestVectors )
{
    BaseType_t xResult = pdFALSE;
    void * pvSignatureVerificationContext = NULL;
    char cSignerCertificateRSA[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDtzCCAp+gAwIBAgIJAMAJ51I17/tXMA0GCSqGSIb3DQEBCwUAMHIxCzAJBgNV\n"
        "BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTEMMAoGA1UECgwD\n"
        "QVdTMQwwCgYDVQQLDANJb1QxDDAKBgNVBAMMA0RhbjEaMBgGCSqGSIb3DQEJARYL\n"
        "ZGFuQGZvby5jb20wHhcNMTcwNzMxMjI0OTE3WhcNMTgwNzMxMjI0OTE3WjByMQsw\n"
        "CQYDVQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDDAKBgNV\n"
        "BAoMA0FXUzEMMAoGA1UECwwDSW9UMQwwCgYDVQQDDANEYW4xGjAYBgkqhkiG9w0B\n"
        "CQEWC2RhbkBmb28uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"
        "rQB70DjusUYtNRgGfnrt9bzVT6K50TEwC22FTTmMFzc2TayGZLyXvVEvpaT3RSQp\n"
        "WPXtSN6PI9P/r9t1281UqfeBkt/0Mg5p1Yqd7XhXdziREi/pcKrj/umk1AsrgL7t\n"
        "9XuwIFX5hluej6MyDDAuhtPFAdu7WGAdZr+aBrZyohAgfFcBSNs9/bLoU36FxoWM\n"
        "0iByBztnEdy3k+b+JqwAUOxYLnZTSD28eO4AwXj1WZ2VSLBs+DeDL6SAjRy7v7DF\n"
        "+vrqw82OoYmSk5nzLBYf16H8cEW/zVeWlP+Liw0WTJNGGp2YCwlNx20ywHiNBZDd\n"
        "f/Djw7GsObG5oRs7939kJwIDAQABo1AwTjAdBgNVHQ4EFgQUokZzCOJv6EW6dUHn\n"
        "oDAAQG5BKYcwHwYDVR0jBBgwFoAUokZzCOJv6EW6dUHnoDAAQG5BKYcwDAYDVR0T\n"
        "BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAXO/ggGB8ykOozOs6mciY2kH1l6yI\n"
        "ldnvXn6ilsfICWxj0Xvd+HoDe64ycUmcLUmN2U4NVD42+l9AoZJ6KaewTSiqWo15\n"
        "cMxUs+huysXYS3Kynv/TUTvhqh8yia1mkTFetfDWzgjVsaV0vb9KJcziQXIzAaLc\n"
        "O7WjvCW90YZpxlAwlJgtLfCpD0T1V4vzgT7w2sDMmE8637+3/eDNtP+U/CK+eo0w\n"
        "fjBdyj+3TOG3F6/X4+SFrk+64++CoWuD/y9yJ8FBpPEv2jW2EDWL6quWyqLs1Qye\n"
        "KiOc4Xj4X034Bm9JF5tnaj51igCWrXBcxUHrh2xfkMuQvKyIu6Bkz0IbgQ==\n"
        "-----END CERTIFICATE-----\n";
    uint8_t ucRSA_SHA1Signature[] =
    {
        0x07, 0x77, 0x7C, 0xDD, 0x50, 0xB3, 0xC6, 0x6B, 0x76, 0x1D, 0xC4, 0x99, 0x3C, 0xED, 0xFE, 0x27,
        0xBE, 0x6E, 0x76, 0xD3, 0xBF, 0x2E, 0x2E, 0x28, 0xFD, 0x99, 0x9E, 0xF2, 0xE6, 0x3B, 0x0B, 0x39,
        0x55, 0xB0, 0xE3, 0x67, 0x24, 0x39, 0x24, 0x28, 0x1D, 0xF2, 0x7A, 0x16, 0xA6, 0x17, 0xE3, 0x42,
        0x46, 0x90, 0x09, 0xB6, 0x17, 0x55, 0xB0, 0xA1, 0xA8, 0x55, 0xDC, 0x0C, 0x20, 0x0F, 0xC2, 0x66,
        0x36, 0xE0, 0xCD, 0xAF, 0x31, 0x52, 0x83, 0x7B, 0x12, 0x58, 0x93, 0x77, 0x66, 0x5A, 0x4A, 0x1C,
        0xD9, 0x42, 0xAD, 0x5D, 0xA9, 0xF2, 0x2E, 0x5C, 0xBB, 0x14, 0x82, 0x9F, 0xED, 0x10, 0x8A, 0xF4,
        0xD3, 0xC8, 0x06, 0xAB, 0xFA, 0x03, 0x65, 0xA8, 0xC5, 0x96, 0xEE, 0xC4, 0xC6, 0x6E, 0x31, 0x1E,
        0xA8, 0x23, 0xF9, 0xBF, 0x7C, 0xA7, 0xED, 0xED, 0xA4, 0xCD, 0xD5, 0xD6, 0x5D, 0xE8, 0xCF, 0xB9,
        0xFA, 0x5F, 0xA9, 0xB5, 0x1F, 0xF1, 0x8A, 0xB7, 0x1A, 0x12, 0xCD, 0x67, 0xE8, 0x5C, 0x89, 0x6B,
        0xD7, 0xDB, 0xD3, 0xD0, 0xBA, 0x01, 0xFA, 0x4E, 0x32, 0xF4, 0x08, 0xB8, 0x3B, 0xC6, 0x5E, 0xE4,
        0xF8, 0x9E, 0x06, 0x0B, 0x84, 0x5D, 0x0D, 0xF3, 0x34, 0x4F, 0x17, 0xFE, 0x6F, 0xF6, 0x4F, 0xAC,
        0x87, 0xE5, 0x7E, 0x72, 0x23, 0x09, 0x29, 0x69, 0x74, 0xAA, 0x79, 0xA6, 0xBA, 0xCC, 0x46, 0x20,
        0xAC, 0xAE, 0xB9, 0xC4, 0xAE, 0x5C, 0x20, 0x5E, 0x47, 0x02, 0x8B, 0x39, 0x56, 0x5F, 0xC0, 0xA3,
        0xEF, 0x82, 0xC5, 0x83, 0x6D, 0x3E, 0x99, 0x05, 0xF0, 0x36, 0x4A, 0x2F, 0xA0, 0x95, 0xC1, 0x0E,
        0x6C, 0xBD, 0x3C, 0x33, 0xD3, 0x6F, 0x01, 0x61, 0x51, 0x05, 0x6A, 0x00, 0x82, 0x3D, 0x1F, 0xEF,
        0xB8, 0xC1, 0x88, 0xBB, 0xB2, 0xE6, 0x37, 0xBC, 0xA6, 0x5C, 0x4B, 0xAC, 0x02, 0x6E, 0xA3, 0x26
    };
    uint8_t ucRSA_SHA256Signature[] =
    {
        0x06, 0x5C, 0x35, 0x49, 0x43, 0x22, 0xDF, 0x12, 0xAE, 0xEE, 0x65, 0x4B, 0x1D, 0x5A, 0x1A, 0x57,
        0xA7, 0xA9, 0x8D, 0x9B, 0xCB, 0x76, 0xC4, 0xF9, 0x36, 0x22, 0xF6, 0xB3, 0x4B, 0x40, 0x3E, 0xF7,
        0x5D, 0x9C, 0x33, 0x5E, 0xEB, 0xCC, 0xAB, 0x27, 0xFC, 0x0C, 0x65, 0x72, 0x2E, 0x7D, 0x1D, 0x14,
        0x36, 0xAF, 0xAF, 0xA9, 0x3E, 0x62, 0x4F, 0xFA, 0x9A, 0x5C, 0x52, 0xF2, 0x62, 0x17, 0x9F, 0xC0,
        0xE0, 0x22, 0xEC, 0xD1, 0x5F, 0x34, 0x7B, 0x94, 0x2B, 0x0F, 0x4D, 0xD0, 0x65, 0x08, 0x0E, 0x4E,
        0xCB, 0xE9, 0x7F, 0x76, 0x0C, 0x4D, 0x5E, 0xD3, 0xF7, 0x9F, 0xEF, 0xD7, 0x28, 0x93, 0x41, 0x34,
        0xF3, 0x9C, 0x94, 0xF4, 0x46, 0x5D, 0x38, 0xF4, 0xAF, 0x23, 0x8A, 0x47, 0x34, 0xDD, 0x33, 0xB3,
        0x47, 0x5C, 0x8D, 0xD5, 0x10, 0x61, 0x8E, 0x2C, 0x3E, 0x77, 0xA0, 0xBB, 0x7B, 0xE7, 0x4D, 0x9A,
        0x6E, 0x65, 0xA2, 0x07, 0x5B, 0xB7, 0xC9, 0x85, 0xC3, 0x96, 0x98, 0xA9, 0x1E, 0x90, 0xB0, 0x04,
        0x07, 0xE7, 0x8C, 0xB0, 0x8B, 0x25, 0x66, 0x9F, 0xDB, 0xB6, 0x11, 0x25, 0x60, 0xA1, 0x46, 0x75,
        0xEB, 0x48, 0x65, 0xE4, 0x5D, 0xB8, 0xD3, 0x42, 0x58, 0xC9, 0xB5, 0x01, 0x8E, 0xFF, 0x35, 0xA5,
        0x67, 0x45, 0xE8, 0x62, 0xBD, 0xB5, 0xA0, 0x9E, 0x87, 0xA4, 0xE9, 0xDD, 0xC8, 0xE6, 0x39, 0xA8,
        0x97, 0x2B, 0xF1, 0x04, 0x99, 0x8F, 0xB6, 0xD7, 0xD6, 0xFE, 0xAF, 0x35, 0xC2, 0xC1, 0x62, 0x7C,
        0x1C, 0x33, 0x55, 0x87, 0xA9, 0xE9, 0xBC, 0xEF, 0x5E, 0xBC, 0xB7, 0x79, 0xCF, 0xF5, 0xB5, 0xCB,
        0x4A, 0xC8, 0xD9, 0xD0, 0xA2, 0xE9, 0x47, 0x72, 0x04, 0x23, 0xD1, 0x90, 0x1C, 0x61, 0x3B, 0x60,
        0x9A, 0xFC, 0xAC, 0x4D, 0x35, 0xE2, 0xE3, 0xA6, 0x90, 0x3A, 0x3E, 0xFA, 0x92, 0x0F, 0xA4, 0xAC
    };
    char cSignerCertificateECDSA[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIICKzCCAdGgAwIBAgIJAKNGg1OpqFRbMAoGCCqGSM49BAMCMHIxCzAJBgNVBAYT\n"
        "AlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTEMMAoGA1UECgwDQVdT\n"
        "MQwwCgYDVQQLDANJb1QxDDAKBgNVBAMMA0RhbjEaMBgGCSqGSIb3DQEJARYLZGFu\n"
        "QGZvby5jb20wHhcNMTcwODAxMTUzOTQ4WhcNMTgwODAxMTUzOTQ4WjByMQswCQYD\n"
        "VQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDDAKBgNVBAoM\n"
        "A0FXUzEMMAoGA1UECwwDSW9UMQwwCgYDVQQDDANEYW4xGjAYBgkqhkiG9w0BCQEW\n"
        "C2RhbkBmb28uY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEQXHTh/4Bglwa\n"
        "P9Eb4UzekSAbdO7pjTOxiHcySJbF77HwB54VNpURb4Ezdbinq/i/4ZWAgrtXZqAH\n"
        "3SRhMnIOuKNQME4wHQYDVR0OBBYEFJDG0d5hX9C14PmtSq3pC0cfTjVyMB8GA1Ud\n"
        "IwQYMBaAFJDG0d5hX9C14PmtSq3pC0cfTjVyMAwGA1UdEwQFMAMBAf8wCgYIKoZI\n"
        "zj0EAwIDSAAwRQIgfqoTxQqp0eW5rEOZt36vcdVC989DLAMfrdEo49IxjxACIQDX\n"
        "iR2uXx4o5BNFKfk+aD60EEtFV9tdLvxMYNJy9ftnsg==\n"
        "-----END CERTIFICATE-----\n";
    uint8_t ucECDSA_SHA256Signature[] =
    {
        0x30, 0x45, 0x02, 0x20, 0x5C, 0xF5, 0x58, 0x76, 0x9F, 0xFC, 0x7E, 0xDE, 0x34, 0xAC, 0x72, 0xB2,
        0x1A, 0x8B, 0xF9, 0x63, 0xBB, 0x72, 0x3A, 0x08, 0xCA, 0x70, 0x16, 0xE0, 0x9D, 0x6F, 0xBD, 0x03,
        0xEA, 0x22, 0x61, 0x2F, 0x02, 0x21, 0x00, 0xCD, 0x68, 0xB8, 0x49, 0x81, 0x88, 0x3C, 0xD3, 0xE2,
        0x2D, 0x15, 0x30, 0xB2, 0xCF, 0xF0, 0x6B, 0x3C, 0xB9, 0x8A, 0x92, 0xE8, 0x70, 0x5F, 0x50, 0xD6,
        0x00, 0xC0, 0xDF, 0x6E, 0x3A, 0xF5, 0x27
    };

#define TEST_DATA_TO_SIGN_BYTES    1024
    uint8_t ucDataToSign[ TEST_DATA_TO_SIGN_BYTES ] = { 0 };

    /** \brief Verify an RSA-SHA256 signature test vector.
     *  @{
     */

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_RSA,
        cryptoHASH_ALGORITHM_SHA256 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateRSA,
        sizeof( cSignerCertificateRSA ),
        ucRSA_SHA256Signature,
        sizeof( ucRSA_SHA256Signature ) );
    TEST_ASSERT_TRUE( xResult );

    /* Flip the bits of first byte, this should fail the verification. */
    ucRSA_SHA256Signature[ 0 ] = ~ucRSA_SHA256Signature[ 0 ];

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_RSA,
        cryptoHASH_ALGORITHM_SHA256 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateRSA,
        sizeof( cSignerCertificateRSA ),
        ucRSA_SHA256Signature,
        sizeof( ucRSA_SHA256Signature ) );
    TEST_ASSERT_FALSE( xResult );
    /** @}*/

    /** \brief Verify an RSA-SHA1 signature test vector.
     *  @{
     */

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_RSA,
        cryptoHASH_ALGORITHM_SHA1 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateRSA,
        sizeof( cSignerCertificateRSA ),
        ucRSA_SHA1Signature,
        sizeof( ucRSA_SHA1Signature ) );
    TEST_ASSERT_TRUE( xResult );

    /* Flip the bits of first byte, this should fail the verification. */
    ucRSA_SHA1Signature[ 0 ] = ~ucRSA_SHA1Signature[ 0 ];

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_RSA,
        cryptoHASH_ALGORITHM_SHA1 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateRSA,
        sizeof( cSignerCertificateRSA ),
        ucRSA_SHA1Signature,
        sizeof( ucRSA_SHA1Signature ) );
    TEST_ASSERT_FALSE( xResult );
    /** @}*/

    /** \brief Verify an ECDSA signature test vector.
     *  @{
     */

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_ECDSA,
        cryptoHASH_ALGORITHM_SHA256 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateECDSA,
        sizeof( cSignerCertificateECDSA ),
        ucECDSA_SHA256Signature,
        sizeof( ucECDSA_SHA256Signature ) );
    TEST_ASSERT_TRUE( xResult );

    /* Flip the bits of first byte, this should fail the verification. */
    ucECDSA_SHA256Signature[ 0 ] = ~ucECDSA_SHA256Signature[ 0 ];

    xResult = CRYPTO_SignatureVerificationStart(
        &pvSignatureVerificationContext,
        cryptoASYMMETRIC_ALGORITHM_ECDSA,
        cryptoHASH_ALGORITHM_SHA256 );
    TEST_ASSERT_TRUE( xResult );

    CRYPTO_SignatureVerificationUpdate(
        pvSignatureVerificationContext,
        ucDataToSign,
        sizeof( ucDataToSign ) );

    xResult = CRYPTO_SignatureVerificationFinal(
        pvSignatureVerificationContext,
        cSignerCertificateECDSA,
        sizeof( cSignerCertificateECDSA ),
        ucECDSA_SHA256Signature,
        sizeof( ucECDSA_SHA256Signature ) );
    TEST_ASSERT_FALSE( xResult );
    /** @}*/
}
