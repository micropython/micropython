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
 * @file aws_ota_agent_test_access_define.h
 * @brief Function wrappers that access private methods in aws_ota_agent.c.
 *
 * Needed for testing private functions.
 */

#ifndef _AWS_OTA_AGENT_TEST_ACCESS_DEFINE_H_
#define _AWS_OTA_AGENT_TEST_ACCESS_DEFINE_H_

/*-----------------------------------------------------------*/

IngestResult_t TEST_OTA_prvIngestDataBlock( OTA_FileContext_t * C,
                                            const char * pacRawMsg,
                                            u32 iMsgSize,
                                            OTA_Err_t * pxCloseResult )
{
    return prvIngestDataBlock( C, pacRawMsg, iMsgSize, pxCloseResult );
}


/*-----------------------------------------------------------*/

OTA_FileContext_t * TEST_OTA_prvParseJobDoc( const char * pacRawMsg,
                                             u32 iMsgLen )
{
    return prvParseJobDoc( pacRawMsg, iMsgLen );
}

/*-----------------------------------------------------------*/

bool_t TEST_OTA_prvOTA_Close( OTA_FileContext_t * const C )
{
    return prvOTA_Close( C );
}

/*-----------------------------------------------------------*/

DocParseErr_t TEST_OTA_prvParseJSONbyModel( const char * pcJSON,
                                            uint32_t ulMsgLen,
                                            JSON_DocModel_t * pxDocModel )
{
    return prvParseJSONbyModel( pcJSON, ulMsgLen, pxDocModel );
}

#endif /* _AWS_OTA_AGENT_TEST_ACCESS_DEFINE_H_ */
