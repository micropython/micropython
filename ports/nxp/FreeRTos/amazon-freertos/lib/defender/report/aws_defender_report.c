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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static DefenderMetric_t xMetricsList[ DEFENDER_MAX_METRICS_COUNT ];
static int32_t lMetricsCount;

DefenderErr_t DEFENDER_MetricsInitFunc( DefenderMetric_t * xMetrics,
                                        int32_t lMetricsCountIn )
{
    /*If the metrics list is too long return an error*/
    if( DEFENDER_MAX_METRICS_COUNT < lMetricsCountIn )
    {
        return eDefenderErrTooManyMetrics;
    }

    /*Set the metrics count*/
    lMetricsCount = lMetricsCountIn;

    /*Copy the list of metrics */
    for( int32_t lI = 0; lI < lMetricsCountIn; ++lI )
    {
        xMetricsList[ lI ] = xMetrics[ lI ];
    }

    /*Return success*/
    return eDefenderErrSuccess;
}

CBORHandle_t CreateReport( void )
{
    /*Create new empty report*/
    CBORHandle_t xReport = CBOR_New( 0 );
    /*Create new report header*/
    CBORHandle_t xHeader = GetHeader();

    /*Copy header to the report*/
    CBOR_AppendKeyWithMap( xReport, DEFENDER_HEADER_TAG, xHeader );
    /*Delete the header*/
    CBOR_Delete( &xHeader );
    /*Create new empty metrics document*/
    CBORHandle_t xMetrics = CBOR_New( 0 );

    /*For each metric, append it to the metrics document*/
    for( int32_t lI = 0; lI < lMetricsCount; ++lI )
    {
        /*Update the date for the metric*/
        xMetricsList[ lI ]->UpdateMetric();
        /*Get report from the metric*/
        CBORHandle_t xMetric = xMetricsList[ lI ]->ReportMetric();
        /*Copy the metric to the metrics document*/
        CBOR_AppendMap( xMetrics, xMetric );
        /*Delete the metric document*/
        CBOR_Delete( &xMetric );
    }

    /*Copy the metrics document to the report*/
    CBOR_AppendKeyWithMap( xReport, DEFENDER_METRICS_TAG, xMetrics );
    /*Delete the metrics document*/
    CBOR_Delete( &xMetrics );

    /*If an error occurred */
    if( eCborErrNoError != CBOR_CheckError( xReport ) )
    {
        /*Delete the report*/
        CBOR_Delete( &xReport );
    }

    /*Return the report*/
    return xReport;
}
