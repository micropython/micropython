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
#ifndef AWS_DEFENDER_H
#define AWS_DEFENDER_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Pointer to a Defender metric structure
 * @note Calling application should not access the contents of the structure
 * @see DEFENDER_MetricsInit
 */
typedef struct DefenderMetric_s * DefenderMetric_t;

/** Status returned from public functions */
typedef enum
{
    /** Function was successful */
    eDefenderErrSuccess = 0,

    /** More metrics were provided than are available for use and/or storage.
     * Double check that the metrics list is correct. */
    eDefenderErrTooManyMetrics,

    /** Request reporting period was too short.  The period should not be less
     * than 300 seconds. */
    eDefenderErrPeriodTooShort,

    /** A task could not be created, and therefore the device defender agent
     * failed to start. */
    eDefenderErrFailedToCreateTask,

    /** The agent is already running.  The function that emitted this error
     * cannot be called while the agent is running (e.g. DEFENDER_Start) */
    eDefenderErrAlreadyStarted,

    /** The agent is not running.  The function that emitted this error cannot
     * be called when the agent is not running (e.g. DEFENDER_Stop) */
    eDefenderErrNotStarted,

    /** An otherwise unspecified error has occurred.   */
    eDefenderErrOther,
} DefenderErr_t;

/** Indicates the status of the last report sent */
typedef enum
{
    /** Last report was successfully sent and acknowledged */
    eDefenderRepSuccess = 0,

    /** Device defender has been started but no report has been sent */
    eDefenderRepInit,

    /** Last report was rejected */
    eDefenderRepRejected,

    /** Last report was not acknowledged */
    eDefenderRepNoAck,

    /** Last report failed to send, likely due to a connectivity issue */
    eDefenderRepNotSent,
} DefenderReportStatus_t;

/** Maximum number of reportable metrics */
#define DEFENDER_MAX_METRICS_COUNT    ( 1 )

/** Provides a count of established tcp connections */
extern DefenderMetric_t xDefenderTCPConnections;

/**
 * @param pxMetricsList List of the metrics to put in the report
 * @return DefenderErr_t
 */
#define DEFENDER_MetricsInit( pxMetricsList ) \
    DEFENDER_MetricsInitFunc(                 \
        pxMetricsList, sizeof( pxMetricsList ) / sizeof( DefenderMetric_t ) )

/**
 * @note Prefer using DEFENDER_MetricsInit where possible.
 * @brief Initializes the metrics list
 * @return DefenderErr_t
 */
DefenderErr_t DEFENDER_MetricsInitFunc( DefenderMetric_t * pxMetricsList,
                                        int32_t lMetricsLength );

/**
 * @brief Set the reporting period in seconds
 *
 * Device defender provides metric reports on an interval.  If the device is
 * awake, and the interval has elapsed, the device will report the metrics.
 *
 * @param  periodSec Reporting period in seconds
 * @return DefenderErr_t
 */
DefenderErr_t DEFENDER_ReportPeriodSet( uint32_t ulPeriodSec );

/**
 * @brief      Set the timeout period for various connection actions
 *
 * @param[in]  ulTimeoutMs  The timeout in milliseconds
 *
 * @return     DefenderErr_t
 */
DefenderErr_t DEFENDER_ConnectionTimeoutSet( uint32_t ulTimeoutMs );

/**
 * @brief Starts the defender agent
 * @return DefenderErr_t
 */
DefenderErr_t DEFENDER_Start( void );

/**
 * @brief Stops the defender agent
 * @return DefenderErr_t
 */
DefenderErr_t DEFENDER_Stop( void );

/**
 * @return Status of most recent report
 * @return DefenderReportStatus_t
 */
DefenderReportStatus_t DEFENDER_ReportStatusGet( void );

/**
 * @brief Returns the last Report ID used.
 */
int32_t GetLastReportId( void );

/**
 * @brief Returns the name of the given enum value
 *
 * @return Pointer to a string literal
 */
char const * DEFENDER_ErrAsString( DefenderErr_t /*eErrNum*/ );

/**
 * @brief Returns the name of the given enum value
 *
 * @return Pointer to a string literal
 */
char const *
    DEFENDER_ReportStatusAsString( DefenderReportStatus_t /*eStatusNum*/ );

#endif /* end of include guard: AWS_DEFENDER_H */
