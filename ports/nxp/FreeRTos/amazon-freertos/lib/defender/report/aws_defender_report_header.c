/*
 * Amazon FreeRTOS Device Defender Agent V1.0.2
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
#include "aws_defender_internals.h"
#include "FreeRTOS.h"

static int32_t lReportId;
const char * pcDEFENDER_METRICS_VERSION = "1.0";

/*
 * Returns a non-crypto, but random number for initializing the report ID
 * This helps to prevent duplicate report IDs from the same device after POR
 */
static uint32_t prvDEFENDER_ReportIdInit( void )
{
    uint32_t ulId = configRAND32();

    ulId >>= 2; /*Strip off top two bits to avoid overflow*/
    return ulId;
}

CBORHandle_t GetHeader( void )
{
    lReportId = lReportId == 0 ? prvDEFENDER_ReportIdInit() : lReportId;

    CBORHandle_t xHeader = CBOR_New( 0 );

    CBOR_AssignKeyWithInt( xHeader, DEFENDER_REPORT_ID_TAG, ++lReportId );
    CBOR_AssignKeyWithString(
        xHeader, DEFENDER_VERSION_TAG, pcDEFENDER_METRICS_VERSION );

    return xHeader;
}

int32_t GetLastReportId( void )
{
    return lReportId;
}
