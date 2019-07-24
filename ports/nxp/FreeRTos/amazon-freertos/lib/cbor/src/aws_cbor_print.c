/*
 * Amazon FreeRTOS CBOR Library V1.0.1
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
#include "aws_cbor_internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Appends a key value pair to to the destination string
 *
 * @note str will get reallocated if there is insufficient space for the key.
 *
 * Reads the key value pair from the CBOR data and appends them to the end of
 * the string
 *
 * @param  str       Destination string
 * @param  xCborData CBOR handle with cursor pointed at key to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatKeyValue( char * pcStr,
                                CBORHandle_t xCborData,
                                cbor_ssize_t * pxBuf_len );

/**
 * @brief Append key from cbor data to destination string
 *
 * @note str will get reallocated if there is insufficient space for the key.
 *
 * Reads the key from the CBOR structure and concatonates it to the str.  It
 * will also append a ":".
 *
 * @param  str       Destination string
 * @param  xCborData CBOR handle with cursor pointed at key to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatKey( char * pcStr,
                           CBORHandle_t xCborData,
                           cbor_ssize_t * pxBuf_len );

/**
 * @brief Append value from cbor data to destination string
 *
 * @note str will get reallocated if there is insufficient space for the value.
 *
 * @param  str       Destination string
 * @param  xCborData CBOR handle with cursor pointed at value to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatValue( char * pcStr,
                             CBORHandle_t xCborData,
                             cbor_ssize_t * pxBuf_len );

/**
 * @brief Return string representation of integer at cursor location
 *
 * @note allocates data memory for string return
 *
 * @param  xCborData handle for cbor object
 * @return           String representation of integer
 */
static char * CBOR_IntAsString( CBORHandle_t xCborData );

/**
 * @brief concatenates two strings together
 *
 * @note allocates data memory for string return
 *
 * Concatenates two strings together (dest + src).  If there is insufficient
 * space in the destination, the destination is reallocated with sufficient
 * space.
 *
 * @param  dest Pointer to destination string.
 * @note The dest pointer may be eallocated, so the user must use the returned
 *     pointer
 * @param  src  [description]
 * @param  len  [description]
 * @return      [description]
 */
static char * CBOR_StrCat( char * pcDest,
                           char const * pcSrc,
                           cbor_ssize_t * pxLen );

char * CBOR_AsString( CBORHandle_t xCborData )
{
    CBOR_SetCursor( xCborData, 0 );
    cbor_ssize_t xBuf_len = 16;
    char * pcStr = pxCBOR_malloc( xBuf_len );
    pcStr[ 0 ] = '{';
    pcStr[ 1 ] = 0;
    CBOR_NextKey( xCborData );

    while( !( CBOR_BREAK == *( xCborData->pxCursor ) ) )
    {
        pcStr = CBOR_CatKeyValue( pcStr, xCborData, &xBuf_len );
        CBOR_NextKey( xCborData );

        if( !( CBOR_BREAK == *( xCborData->pxCursor ) ) )
        {
            pcStr = CBOR_StrCat( pcStr, ",", &xBuf_len );
        }
    }

    pcStr = CBOR_StrCat( pcStr, "}", &xBuf_len );

    return pcStr;
}

static char * CBOR_CatKeyValue( char * pcStr,
                                CBORHandle_t xCborData,
                                cbor_ssize_t * pxBuf_len )
{
    pcStr = CBOR_CatKey( pcStr, xCborData, pxBuf_len );
    pcStr = CBOR_CatValue( pcStr, xCborData, pxBuf_len );

    return pcStr;
}

static char * CBOR_CatKey( char * pcStr,
                           CBORHandle_t xCborData,
                           cbor_ssize_t * pxBuf_len )
{
    pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
    char * pcKey = CBOR_ReadString( xCborData );
    pcStr = CBOR_StrCat( pcStr, pcKey, pxBuf_len );
    pxCBOR_free( pcKey );
    pcStr = CBOR_StrCat( pcStr, "\":", pxBuf_len );

    return pcStr;
}

static char * CBOR_CatValue( char * pcStr,
                             CBORHandle_t xCborData,
                             cbor_ssize_t * pxBuf_len )
{
    cbor_byte_t * pxKey_pos = xCborData->pxCursor;

    CBOR_Next( xCborData );
    cbor_byte_t xJump_type = *xCborData->pxCursor;
    cbor_byte_t xMajor_type = ( CBOR_MAJOR_TYPE_MASK & xJump_type );

    switch( xMajor_type )
    {
        case CBOR_POS_INT:
        case CBOR_NEG_INT:
           {
               char * pcVal = CBOR_IntAsString( xCborData );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               break;
           }

        case CBOR_BYTE_STRING:
        case CBOR_STRING:
           {
               pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
               char * pcVal = CBOR_ReadString( xCborData );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
               break;
           }

        case CBOR_MAP:
           {
               CBORHandle_t xInner_map = CBOR_ReadMap( xCborData );
               char * pcVal = CBOR_AsString( xInner_map );
               CBOR_Delete( &xInner_map );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               break;
           }
    }

    xCborData->pxCursor = pxKey_pos;

    return pcStr;
}

static char * CBOR_IntAsString( CBORHandle_t xCborData )
{
    int lVal = CBOR_ReadInt( xCborData );
    int lLen = snprintf( NULL, 0, "%d", lVal ) + 1;
    char * pcStr = pxCBOR_malloc( lLen );

    ( void ) snprintf( pcStr, lLen, "%d", lVal );

    return pcStr;
}

static char * CBOR_StrCat( char * pcDest,
                           char const * pcSrc,
                           cbor_ssize_t * pxLen )
{
    cbor_ssize_t xDest_len = strlen( pcDest );
    cbor_ssize_t xSrc_len = strlen( pcSrc );

    if( *pxLen <= xDest_len + xSrc_len )
    {
        do
        {
            *pxLen *= 3;
            *pxLen /= 2;

            /* Inefficient, but necessary to reallocate in each loop, since some
             * embedded platforms (e.g. FreeRTOS) do not implement realloc. */
            pcDest = pxCBOR_realloc( pcDest, *pxLen );
        } while( *pxLen <= xDest_len + xSrc_len );
    }

    strncat( pcDest, pcSrc, ( *pxLen - xDest_len ) );

    return pcDest;
}
