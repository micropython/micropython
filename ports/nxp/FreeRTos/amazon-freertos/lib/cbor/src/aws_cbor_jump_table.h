/*
 * Amazon FreeRTOS
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
#ifndef AWS_CBOR_JUMP_TABLE_H
#define AWS_CBOR_JUMP_TABLE_H

/**
 * @file
 * @brief Jump table definitions for typing and navigating CBOR buffers
 */

#define CBOR_BREAK     0xFF

#define CBOR_TYPE_0    ( 0U )
#define CBOR_TYPE_1    ( 1U )
#define CBOR_TYPE_2    ( 2U )
#define CBOR_TYPE_3    ( 3U )
#define CBOR_TYPE_4    ( 4U )
#define CBOR_TYPE_5    ( 5U )
#define CBOR_TYPE_6    ( 6U )
#define CBOR_TYPE_7    ( 7U )

/* TODO:Made shift value unsigned to silence check. */
/* Need to add fix to clang-tidy check to remove false positive, as it is not */
/* the right hand value that is being worked on by the shift operation, and */
/* therefore signed is allowed. */
#define CBOR_MajorType( a )    ( ( a ) << 5U )
#define CBOR_MAJOR_TYPE_MASK         CBOR_MajorType( 0x7U )
#define CBOR_ADDITIONAL_DATA_MASK    ( ~CBOR_MAJOR_TYPE_MASK )

#define CBOR_POS_INT                 CBOR_MajorType( CBOR_TYPE_0 )
#define CBOR_NEG_INT                 CBOR_MajorType( CBOR_TYPE_1 )
#define CBOR_BYTE_STRING             CBOR_MajorType( CBOR_TYPE_2 )
#define CBOR_STRING                  CBOR_MajorType( CBOR_TYPE_3 )
#define CBOR_ARRAY                   CBOR_MajorType( CBOR_TYPE_4 )
#define CBOR_MAP                     CBOR_MajorType( CBOR_TYPE_5 )
#define CBOR_TAG                     CBOR_MajorType( CBOR_TYPE_6 )
#define CBOR_OTHER                   CBOR_MajorType( CBOR_TYPE_7 )

/* integer additional information */
#define CBOR_INT8_FOLLOWS            ( 0x18 )
#define CBOR_INT16_FOLLOWS           ( 0x19 )
#define CBOR_INT32_FOLLOWS           ( 0x1A )

#define CBOR_IsSmallInt( a )    ( CBOR_INT8_FOLLOWS > ( a ) )
#define CBOR_Is8BitInt( a )     ( UINT8_MAX >= ( a ) )
#define CBOR_Is16BitInt( a )    ( UINT16_MAX >= ( a ) )
#define CBOR_Is32BitInt( a )    ( UINT32_MAX >= ( a ) )

#define CBOR_SMALL_INT_SIZE       ( 1 )
#define CBOR_INT8_SIZE            ( 2 )
#define CBOR_INT16_SIZE           ( 3 )
#define CBOR_INT32_SIZE           ( 5 )

/* */
#define CBOR_INDEFINITE_LENGTH    ( 0x1FU )

#define CBOR_MAP_OPEN             ( CBOR_MAP | CBOR_INDEFINITE_LENGTH )

#define CBOR_BYTE_WIDTH           ( 8U )

#endif /* end of include guard: AWS_CBOR_JUMP_TABLE_H */
