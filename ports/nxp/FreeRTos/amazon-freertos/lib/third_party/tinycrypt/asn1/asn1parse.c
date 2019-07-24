/*
 *  Generic ASN.1 parsing
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */


#include "asn1.h"
#include "stdint.h"
#include "string.h"

#include <tinycrypt/ecc.h>

/*
 * ASN.1 DER decoding routines
 */
int mbedtls_asn1_get_len( unsigned char **p,
                  const unsigned char *end,
                  size_t *len )
{
    if( ( end - *p ) < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    if( ( **p & 0x80 ) == 0 )
        *len = *(*p)++;
    else
    {
        switch( **p & 0x7F )
        {
        case 1:
            if( ( end - *p ) < 2 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = (*p)[1];
            (*p) += 2;
            break;

        case 2:
            if( ( end - *p ) < 3 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (size_t)(*p)[1] << 8 ) | (*p)[2];
            (*p) += 3;
            break;

        case 3:
            if( ( end - *p ) < 4 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (size_t)(*p)[1] << 16 ) |
                   ( (size_t)(*p)[2] << 8  ) | (*p)[3];
            (*p) += 4;
            break;

        case 4:
            if( ( end - *p ) < 5 )
                return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

            *len = ( (size_t)(*p)[1] << 24 ) | ( (size_t)(*p)[2] << 16 ) |
                   ( (size_t)(*p)[3] << 8  ) |           (*p)[4];
            (*p) += 5;
            break;

        default:
            return( MBEDTLS_ERR_ASN1_INVALID_LENGTH );
        }
    }

    if( *len > (size_t) ( end - *p ) )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    return( 0 );
}

int mbedtls_asn1_get_tag( unsigned char **p,
                  const unsigned char *end,
                  size_t *len, int tag )
{
    if( ( end - *p ) < 1 )
        return( MBEDTLS_ERR_ASN1_OUT_OF_DATA );

    if( **p != tag )
        return( MBEDTLS_ERR_ASN1_UNEXPECTED_TAG );

    (*p)++;

    return( mbedtls_asn1_get_len( p, end, len ) );
}