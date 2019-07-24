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
 * @file asn1utility.h
 * @brief ASN1 utility header.
 */

#ifndef _ASN1UTILITY_H_
#define _ASN1UTILITY_H_

#include <tinycrypt/ecc.h>
#include "aws_boot_types.h"

/**
 * @brief asn1_getBigInteger.
 * Gets big integer from the data block.
 * @param[in] - pucInt - Data
 * @param[out] - ppucStart - returns pointer to next signature component.
 * @param[in] - pucEnd - end of the data
 * @return - pdTRUE On success , pdFALSE
 */
 BaseType_t asn1_getBigInteger( uint8_t * pucInt,
                                uint8_t ** ppucStart,
                                uint8_t * pucEnd );

/**
 * @brief asn1_decodeSignature.
 * Decodes ASN1 encoded signature.
 * @param[in] - pucSignature - ASN1 encoded signature.
 * @param[in] - pucStart - start of the data
 * @param[in] - pucEnd - end of the data
 * @return - 0 On success , pdFALSE otherwise
 */
BaseType_t asn1_decodeSignature( uint8_t * pucSignature,
                                 uint8_t * pucStart,
                                 uint8_t * pucEnd );
						  
#endif