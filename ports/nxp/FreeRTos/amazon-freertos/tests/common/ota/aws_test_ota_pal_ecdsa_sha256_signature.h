/*
 * Amazon FreeRTOS OTA AFQP V1.1.4
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

#ifndef _AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_
#define _AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_
#include <stdint.h>
#include "aws_test_ota_signature_methods.h"

/**
 * @brief Invalid signature for OTA PAL testing.
 */
static const uint8_t ucInvalidSignature[] =
{
    0x30, 0x44, 0x02, 0x20, 0x75, 0xde, 0xa8, 0x1f, 0xca, 0xec, 0xff, 0x16,
    0xbb, 0x38, 0x4b, 0xe3, 0x14, 0xe7, 0xfb, 0x68, 0xf5, 0x3e, 0x86, 0xa2,
    0x71, 0xba, 0x9e, 0x5e, 0x50, 0xbf, 0xb2, 0x7a, 0x9e, 0x00, 0xc6, 0x4d,
    0x02, 0x20, 0x19, 0x72, 0x42, 0x85, 0x2a, 0xac, 0xdf, 0x5a, 0x5e, 0xfa,
    0xad, 0x49, 0x17, 0x5b, 0xce, 0x5b, 0x65, 0x75, 0x08, 0x47, 0x3e, 0x55,
    0xf9, 0x0e, 0xdf, 0x9e, 0x8c, 0xdc, 0x95, 0xdf, 0x63, 0xd2
};
static const int ucInvalidSignatureLength = 70;

/**
 * @brief Valid signature matching the test block in the OTA PAL tests.
 */
static const uint8_t ucValidSignature[] =
{
    0x30, 0x44, 0x02, 0x20, 0x15, 0x6a, 0x68, 0x98, 0xf0, 0x4e, 0x1e, 0x12,
    0x4c, 0xc4, 0xf1, 0x05, 0x22, 0x36, 0xfd, 0xb4, 0xe5, 0x5d, 0x83, 0x08,
    0x2a, 0xf3, 0xa6, 0x7d, 0x32, 0x6b, 0xff, 0x85, 0x27, 0x14, 0x9b, 0xbf,
    0x02, 0x20, 0x26, 0x7d, 0x5f, 0x4d, 0x12, 0xab, 0xec, 0x17, 0xd8, 0x45,
    0xc6, 0x3d, 0x8e, 0xd8, 0x8d, 0x3f, 0x28, 0x26, 0xfd, 0xce, 0x32, 0x34,
    0x17, 0x05, 0x47, 0xb2, 0xf6, 0x84, 0xd5, 0x68, 0x3e, 0x36
};
static const int ucValidSignatureLength = 70;

/**
 * @brief The type of signature method this file defines for the valid signature.
 */
#define otatestSIG_METHOD   otatestSIG_SHA256_ECDSA

#endif /* ifndef _AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_ */
