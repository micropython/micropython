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
 * @file asn1utility.c
 * @brief ASN1 utility implementation.
 */

#include "aws_boot_types.h"
#include "asn1.h"
#include "string.h"
#include <tinycrypt/ecc.h>
#include "asn1utility.h"
#include "aws_boot_crypto.h"

 /*-----------------------------------------------------------*/
 
 BaseType_t asn1_getBigInteger( uint8_t * pucInt,
                                uint8_t ** ppucStart,
                                uint8_t * pucEnd )
{
    BaseType_t xReturn = pdFALSE; 
    size_t xLength;
    
    /* Clear destination buffer for integer component. */
    memset(pucInt, 0, ECC_NUM_BYTES_PER_SIG_COMPONENT );

    /* Get ASN.1 integer tag. */
    if(0 == mbedtls_asn1_get_tag( ppucStart, pucEnd, &xLength, MBEDTLS_ASN1_INTEGER ) )
    {

        if( xLength > ECC_NUM_BYTES_PER_SIG_COMPONENT )
        {
            /* If length is greater than ECC_NUM_BYTES_PER_SIG_COMPONENT we probably have 
             * signed byte so copy last ECC_NUM_BYTES_PER_SIG_COMPONENT. */
            memcpy( pucInt, *ppucStart + xLength - ECC_NUM_BYTES_PER_SIG_COMPONENT, ECC_NUM_BYTES_PER_SIG_COMPONENT );
        }
        else
        {
            /* If length is less than ECC_NUM_BYTES_PER_SIG_COMPONENT copy the length 
             bytes to buffer. */
            memcpy( pucInt + ECC_NUM_BYTES_PER_SIG_COMPONENT - xLength, *ppucStart, xLength );
        }
        
        /* Move the buffer pointer to next element*/
        *ppucStart += xLength;
        
         xReturn = pdTRUE; 
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

BaseType_t asn1_decodeSignature( uint8_t * pucSignature,
                                 uint8_t * pucStart,
                                 uint8_t * pucEnd )
{
    size_t xLength = 0;
    BaseType_t xReturn = pdFALSE; 
        
    /* Get ASN.1 sequence tag. */
    if( 0 == mbedtls_asn1_get_tag( &pucStart, 
                               pucEnd, 
                               &xLength, 
                               MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ))
    {
        /* Sanity check length. */
        if( pucStart + xLength > pucEnd )
        {
            return pdFALSE;
        }
        
        /* Get the first big integer. */
        xReturn = asn1_getBigInteger( pucSignature, &pucStart, pucEnd );

        if(xReturn == pdTRUE)
        {
            /* Get the second big int. */
            xReturn = asn1_getBigInteger( pucSignature + ECC_NUM_BYTES_PER_SIG_COMPONENT, &pucStart, pucEnd );

        }

    }
    
    return xReturn;
}
