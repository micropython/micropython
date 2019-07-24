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


/**
 * @file aws_shadow_json.h
 * @brief Shadow JSON utility functions.
 */

#ifndef _AWS_SHADOW_JSON_H_
#define _AWS_SHADOW_JSON_H_

#include "FreeRTOS.h"

/**
 * @brief Check if the client tokens in pcDoc1 and pcDoc2 match.
 *
 * @param[in] pcDoc1, pcDoc2 JSON strings
 * @param[in] ulDoc1Length, ulDoc2Length the lengths of pcDoc1 and pcDoc2,
 *     respectively
 * @return pdTRUE if the client tokens in pcDoc1 and pcDoc2 match; pdFALSE
 *     if the client tokens don't match; jsmn error (see jsmn.h) if jsmn
 *     fails to parse either string.
 */
BaseType_t SHADOW_JSONDocClientTokenMatch( const char * const pcDoc1,
                                           uint32_t ulDoc1Length,
                                           const char * const pcDoc2,
                                           uint32_t ulDoc2Length );

/**
 * @brief Extracts the error code and message from a Shadow error JSON string.
 *
 * @param[in] pcErrorJSON a Shadow error JSON string
 * @param[in] ulErrorJSONLength the length of pcErrorJSON
 * @param[out] ppcErrorMessage set to the location of the error message in pcErrorJSON.
 *     Pass NULL to ignore error message.
 * @param[out] pusErrorMessageLength set to the size of the error message
 *     Pass NULL to ignore error message.
 * @return a positive code corresponding to an error reason on success; jsmn error
 *     (see jsmn.h) if jsmn fails to parse pcErrorJSON; 0 for bad pointer arguments
 */
int16_t SHADOW_JSONGetErrorCodeAndMessage( const char * const pcErrorJSON,
                                           uint32_t ulErrorJSONLength,
                                           char ** ppcErrorMessage,
                                           uint16_t * pusErrorMessageLength );

#endif /* _AWS_SHADOW_JSON_H_ */
