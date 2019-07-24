/*
 * Amazon FreeRTOS Device Defender Demo V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include "stdio.h"
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "task.h"

/* Defender includes. */
#include "aws_defender.h"

/* Demo for device defender */
static void prvDefenderDemo( void * );

/* Logs the metrics init status */
static void prvLogInitStatus( DefenderErr_t eStartStatus );

/* Logs the status returned from PeriodSet */
static void prvLogPeriodStatus( DefenderErr_t eStartStatus );

/* Logs the result from starting Device Defender */
static void prvLogStartStatus( DefenderErr_t eStartStatus );

/* Monitors the report status */
static void prvMonitorReportStatus( void );

/* Logs the report status */
static void prvLogReportStatus( DefenderReportStatus_t eStartStatus );

void vStartDeviceDefenderDemo( void )
{
    configSTACK_DEPTH_TYPE const xStackDepth = configMINIMAL_STACK_SIZE;

    ( void ) xTaskCreate( prvDefenderDemo,
                          "Defender Demo",
                          xStackDepth,
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL );
}

static void prvDefenderDemo( void * param )
{
    DefenderMetric_t xMetricsList[ 1 ];
    int32_t lReportPeriodSec = 300;
    DefenderErr_t eInitStatus;
    DefenderErr_t ePeriodStatus;
    DefenderErr_t eStartStatus;

    xMetricsList[ 0 ] = xDefenderTCPConnections;

    /* Silence warning about unused param */
    ( void ) param;

    eInitStatus = DEFENDER_MetricsInit( xMetricsList );
    prvLogInitStatus( eInitStatus );

    ePeriodStatus = DEFENDER_ReportPeriodSet( lReportPeriodSec );
    prvLogPeriodStatus( ePeriodStatus );

    eStartStatus = DEFENDER_Start();
    prvLogStartStatus( eStartStatus );

    prvMonitorReportStatus();
    configPRINTF( ( "----Device Defender Demo Complete----.\r\n" ) );
    vTaskDelete( NULL );
}

static void prvLogInitStatus( DefenderErr_t eInitStatus )
{
    switch( eInitStatus )
    {
        case eDefenderErrSuccess:
            configPRINTF( ( "Defender metrics were initialized.\r\n" ) );
            break;

        case eDefenderErrTooManyMetrics:
            configPRINTF( ( "Too many metrics were provided.\r\n" ) );
            break;

        default:
            configPRINTF( ( "Unexpected init status: %d.\r\n", eInitStatus ) );
            break;
    }
}

static void prvLogPeriodStatus( DefenderErr_t ePeriodStatus )
{
    switch( ePeriodStatus )
    {
        case eDefenderErrSuccess:
            configPRINTF( ( "Defender period was set.\r\n" ) );
            break;

        case eDefenderErrPeriodTooShort:
            configPRINTF( ( "Report period was too short (less than 300 sec).\r\n" ) );
            break;

        default:
            configPRINTF( ( "Unexpected period status: %d.\r\n", ePeriodStatus ) );
            break;
    }
}

static void prvLogStartStatus( DefenderErr_t eStartStatus )
{
    switch( eStartStatus )
    {
        case eDefenderErrSuccess:
            configPRINTF( ( "Defender was started.\r\n" ) );
            break;

        case eDefenderErrFailedToCreateTask:
            configPRINTF( ( "Defender task could not be created.\r\n" ) );
            break;

        case eDefenderErrAlreadyStarted:
            configPRINTF( ( "Defender task was started outside of the demo.\r\n" ) );
            break;

        default:
            configPRINTF( ( "Unexpected start status: %d.\r\n", eStartStatus ) );
            break;
    }
}

static void prvMonitorReportStatus( void )
{
    /* Defender is started.  No further action is required by the calling
     * aplication.  However the application can query the report status as
     * demonstrated below. */
    int32_t lTaskDelayPeriodMs = 11;
    DefenderReportStatus_t eReportStatus = DEFENDER_ReportStatusGet();

    while( eDefenderRepInit == eReportStatus )
    {
        /* The first report is sent on start.  Subsequent reports are sent
         * periodically */
        /* Wait for agent to send the first report */
        eReportStatus = DEFENDER_ReportStatusGet();
        vTaskDelay( pdMS_TO_TICKS( lTaskDelayPeriodMs ) );
    }

    /* First report has been sent.  Will wait for a short period to give the
     * service a chance to respond. */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );

    eReportStatus = DEFENDER_ReportStatusGet();
    prvLogReportStatus( eReportStatus );
}

static void prvLogReportStatus( DefenderReportStatus_t eReportStatus )
{
    switch( eReportStatus )
    {
        case eDefenderRepSuccess:
            configPRINTF( ( "Device Defender Acknowledged the report.\r\n" ) );
            break;

        case eDefenderRepInit:
            configPRINTF( ( "Metrics report has not yet been sent.\r\n" ) );
            break;

        case eDefenderRepRejected:
            configPRINTF( ( "Device defender service received and rejected the "
                            "metrics report.\r\n" ) );
            break;

        case eDefenderRepNoAck:
            configPRINTF( ( "Device defender servie did not acknowledge the metrics "
                            "within the expected time.\r\n" ) );
            break;

        case eDefenderRepNotSent:
            configPRINTF( ( "The metrics report was not sent.\r\n" ) );
            break;

        default:
            configPRINTF( ( "Unexpected report status: %d.\r\n", eReportStatus ) );
            break;
    }
}
