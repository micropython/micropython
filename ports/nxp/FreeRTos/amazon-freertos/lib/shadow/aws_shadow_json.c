/*
 * Amazon FreeRTOS Shadow V1.0.5
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

/**
 * @file aws_shadow_json.c
 * @brief JSON wrapper for shadow API.
 */

/* C library includes. */
#include <string.h>
#include <stdlib.h>

/* AWS includes. */
#include "aws_shadow_json.h"

/* Other includes. */
#include "jsmn.h"

/* Sockets configuration includes. */
#include "aws_shadow_config.h"
#include "aws_shadow_config_defaults.h"

/* The JSON keys to search for when looking for the error code and message,
 * and client token, respectively. */
#define shadowJSON_ERROR_CODE       "code"
#define shadowJSON_ERROR_MESSAGE    "message"
#define shadowJSON_CLIENT_TOKEN     "clientToken"

/**
 * @brief Given a JSON key, get its value. Does not work on arrays or objects.  Returns
 * length of value and sets ppcValue to the start of the value.  Returns 0 on
 * error (key-value does not exist).
 */
static uint16_t prvGetJSONValue( const char ** ppcValue,
                                 const char * const pcKey,
                                 const char * const pcDoc,
                                 jsmntok_t * pxJSMNTokens,
                                 int16_t sTokensParsed );

/**
 * @brief Wrapper for jsmn functions. Returns negative values (see jsmn error codes) on
 * error.  Returns the number of tokens parsed on success.
 */
static int16_t prvParseJSON( const char * const pcDoc,
                             uint32_t ulDocLength,
                             jsmntok_t * pxJSMNTokens );

/*-----------------------------------------------------------*/

BaseType_t SHADOW_JSONDocClientTokenMatch( const char * const pcDoc1,
                                           uint32_t ulDoc1Length,
                                           const char * const pcDoc2,
                                           uint32_t ulDoc2Length )
{
    jsmntok_t pxJSMNTokens[ shadowconfigJSON_JSMN_TOKENS ];
    BaseType_t xReturn = pdFAIL;
    uint16_t usClientToken1Length, usClientToken2Length;
    int16_t sNbTokens;
    char * pcClientToken1;
    char * pcClientToken2;

    /* Parse pcDoc1 with jsmn. */
    sNbTokens = prvParseJSON( pcDoc1, ulDoc1Length, pxJSMNTokens );

    if( sNbTokens > 0 )
    {
        /* Attempt to find the "clientToken" string in parsed pcDoc1. */
        usClientToken1Length = prvGetJSONValue( ( const char ** ) &pcClientToken1,
                                                shadowJSON_CLIENT_TOKEN,
                                                pcDoc1,
                                                ( jsmntok_t * ) pxJSMNTokens,
                                                sNbTokens );

        if( usClientToken1Length > ( uint16_t ) 0 )
        {
            /* Parse pcDoc2 with jsmn. */
            sNbTokens = prvParseJSON( pcDoc2, ulDoc2Length, pxJSMNTokens );

            if( sNbTokens > 0 )
            {
                /* If "clientToken" was found in pcDoc1, attempt to find "clientToken" in pcDoc2. */
                usClientToken2Length = prvGetJSONValue( ( const char ** ) &pcClientToken2,
                                                        shadowJSON_CLIENT_TOKEN,
                                                        pcDoc2,
                                                        ( jsmntok_t * ) pxJSMNTokens,
                                                        sNbTokens );

                /* Compare the client tokens. */
                if( usClientToken2Length == usClientToken1Length )
                {
                    if( strncmp( ( const char * ) pcClientToken1,
                                 ( const char * ) pcClientToken2,
                                 ( size_t ) usClientToken1Length ) == 0 )
                    {
                        xReturn = pdPASS;
                    }
                }
            }
        }
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

int16_t SHADOW_JSONGetErrorCodeAndMessage( const char * const pcErrorJSON,
                                           uint32_t ulErrorJSONLength,
                                           char ** ppcErrorMessage,
                                           uint16_t * pusErrorMessageLength )
{
    jsmntok_t pxJSMNTokens[ shadowconfigJSON_JSMN_TOKENS ];
    char * pcErrorCode;
    int16_t sReturn = 0;
    int16_t sTokensParsed;

    sTokensParsed = prvParseJSON( pcErrorJSON, ulErrorJSONLength, pxJSMNTokens );

    if( sTokensParsed > 0 )
    {
        /* Attempt to find the error code. */
        sReturn = ( int16_t ) prvGetJSONValue( ( const char ** ) &pcErrorCode,
                                               shadowJSON_ERROR_CODE,
                                               pcErrorJSON,
                                               pxJSMNTokens,
                                               sTokensParsed );

        if( sReturn > 0 )
        {
            /* Convert the error code to int16_t for return value. */
            sReturn = ( int16_t ) strtol( pcErrorCode, NULL, 0 );

            if( ( ppcErrorMessage != NULL ) && ( pusErrorMessageLength != NULL ) )
            {
                /* Set the pointer to the error message and the error message length. */
                *pusErrorMessageLength = prvGetJSONValue( ( const char ** ) ppcErrorMessage,
                                                          shadowJSON_ERROR_MESSAGE,
                                                          pcErrorJSON,
                                                          pxJSMNTokens,
                                                          sTokensParsed );
            }
        }
    }
    else
    {
        /* On failure, sTokenParsed returns a jsmn error code.  Return this error code. */
        sReturn = sTokensParsed;
    }

    return sReturn;
}
/*-----------------------------------------------------------*/

static int16_t prvParseJSON( const char * const pcDoc,
                             uint32_t ulDocLength,
                             jsmntok_t * pxJSMNTokens )
{
    jsmn_parser xJSMNParser;
    int16_t sReturn;

    jsmn_init( &xJSMNParser );
    sReturn = ( int16_t ) jsmn_parse( &xJSMNParser,
                                      pcDoc,
                                      ulDocLength,
                                      pxJSMNTokens,
                                      shadowconfigJSON_JSMN_TOKENS );

    return sReturn;
}
/*-----------------------------------------------------------*/

static uint16_t prvGetJSONValue( const char ** ppcValue,
                                 const char * const pcKey,
                                 const char * const pcDoc,
                                 jsmntok_t * pxJSMNTokens,
                                 int16_t sTokensParsed )
{
    jsmntok_t * pxJSMNToken;
    uint16_t usReturn = 0;
    int16_t sIterator;
    uint16_t usCompareLength;
    BaseType_t xValueFound = pdFALSE;

    if( ppcValue != NULL )
    {
        /* "clientToken" is usually (but not guaranteed to be) the last key in the Shadow JSON document.
         * Since this function is primarily used to find "clientToken", find "clientToken" faster
         * by starting from the end. */
        for( sIterator = ( sTokensParsed - 1 ); sIterator >= 0; sIterator-- )
        {
            pxJSMNToken = &( pxJSMNTokens[ sIterator ] );
            usCompareLength = configMAX( ( ( uint16_t ) pxJSMNToken->end - ( uint16_t ) pxJSMNToken->start ),
                                         ( ( uint16_t ) strlen( pcKey ) ) );

            /* Compare pcKey with the keys in the JSON document. */
            if( ( ( strncmp( pcKey, pcDoc + pxJSMNToken->start, usCompareLength ) ) == 0 ) && ( pxJSMNToken->size == 1 ) )
            {
                /* The JSON value is the next jsmn token after the key. */
                pxJSMNToken++;

                /* Set the pointer to the value and the value's length. */
                *ppcValue = ( const char * ) ( pcDoc + pxJSMNToken->start );
                usReturn = ( uint16_t ) pxJSMNToken->end - ( uint16_t ) pxJSMNToken->start;
                xValueFound = pdTRUE;
                break;
            }
        }
    }

    if( xValueFound == pdFALSE )
    {
        usReturn = 0;
    }

    return usReturn;
}
/*-----------------------------------------------------------*/
