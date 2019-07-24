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
 * @file aws_greengrass_lib_test_access_declare.h
 * @brief Declaration of functions that access private methods in aws_greengrass_discovery.c.
 *
 * Needed for testing private functions.
 */

#ifndef _AWS_GREENGRASS_DISCOVERY_TEST_ACCESS_DECLARE_H_
#define _AWS_GREENGRASS_DISCOVERY_TEST_ACCESS_DECLARE_H_

#include "jsmn.h"
BaseType_t test_prvCheckForContentLengthString( uint8_t * pucIndex,
                                                const char cNewChar );
BaseType_t test_prvGGDJsoneq( const char * pcJson, /*lint !e971 can use char without signed/unsigned. */
                              const jsmntok_t * const pxTok,
                              const char * pcString );
void test_prvCheckMatch( const char * pcJSONFile, /*lint !e971 can use char without signed/unsigned. */
                         const jsmntok_t * pxTok,
                         const uint32_t ulTokenIndex,
                         BaseType_t * pxMatch,
                         const char * pcMatchCategory,   /*lint !e971 can use char without signuint32_t ulNbTokensed/unsigned. */
                         const char * pcMatchString,     /*lint !e971 can use char without signed/unsigned. */
                         const BaseType_t xAutoSelectFlag );
BaseType_t test_prvGGDGetCertificate( char * pcJSONFile, /*lint !e971 can use char without signed/unsigned. */
                                      const HostParameters_t * pxHostParameters,
                                      const BaseType_t xAutoSelectFlag,
                                      const jsmntok_t * pxTok,
                                      const uint32_t ulNbTokens,
                                      GGD_HostAddressData_t * pxHostAddressData );
BaseType_t test_prvGGDGetIPOnInterface( char * pcJSONFile, /*lint !e971 can use char without signed/unsigned. */
                                        const uint8_t ucTargetInterface,
                                        const jsmntok_t * pxTok,
                                        const uint32_t ulNbTokens,
                                        GGD_HostAddressData_t * pxHostAddressData,
                                        uint32_t * pulTokenIndex,
                                        uint8_t * pusCurrentInterface );
BaseType_t test_prvGGDGetCore( const char * pcJSONFile, /*lint !e971 can use char without signed/unsigned. */
                               const HostParameters_t * const pxHostParameters,
                               const BaseType_t xAutoSelectFlag,
                               const jsmntok_t * pxTok,
                               const uint32_t ulNbTokens,
                               uint32_t * pulTokenIndex );
BaseType_t test_prvIsIPvalid( const char * pcIP,
                              uint32_t ucIPlength );

#endif /* _AWS_GREENGRASS_DISCOVERY_TEST_ACCESS_DECLARE_H_ */
