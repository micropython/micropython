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
 * @file aws_ota_agent_test_access_declare.h
 * @brief Declarations of functions that access private methods in aws_ota_agent.c.
 *
 * Needed for testing private functions.
 */

#ifndef _AWS_OTA_AGENT_TEST_ACCESS_DECLARE_H_
#define _AWS_OTA_AGENT_TEST_ACCESS_DECLARE_H_

#include "aws_ota_types.h"
#include "aws_ota_agent.h"
#include "aws_ota_agent_internal.h"

IngestResult_t TEST_OTA_prvIngestDataBlock( OTA_FileContext_t * C,
                                            const char * pacRawMsg,
                                            u32 iMsgSize,
                                            OTA_Err_t * pxCloseResult );

OTA_FileContext_t * TEST_OTA_prvParseJobDoc( const char * pacRawMsg,
                                             u32 iMsgLen );

bool_t TEST_OTA_prvOTA_Close( OTA_FileContext_t * const C );

DocParseErr_t TEST_OTA_prvParseJSONbyModel( const char * pcJSON,
                                            uint32_t ulMsgLen,
                                            JSON_DocModel_t * pxDocModel );

#endif /* ifndef _AWS_OTA_AGENT_TEST_ACCESS_DECLARE_H_ */
