/*
 * Amazon FreeRTOS V1.4.6
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


/*
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Standard includes. */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Sanity check all the definitions required by this file are set. */
#ifndef configPRINT_STRING
    #error configPRINT_STRING( x ) must be defined in FreeRTOSConfig.h to use this logging file.  Set configPRINT_STRING( x ) to a function that outputs a string, where X is the string.  For example, #define configPRINT_STRING( x ) MyUARTWriteString( X )
#endif

#ifndef configLOGGING_MAX_MESSAGE_LENGTH
    #error configLOGGING_MAX_MESSAGE_LENGTH must be defined in FreeRTOSConfig.h to use this logging file.  configLOGGING_MAX_MESSAGE_LENGTH sets the size of the buffer into which formatted text is written, so also sets the maximum log message length.
#endif

#ifndef configLOGGING_INCLUDE_TIME_AND_TASK_NAME
    #error configLOGGING_INCLUDE_TIME_AND_TASK_NAME must be defined in FreeRTOSConfig.h to use this logging file.  Set configLOGGING_INCLUDE_TIME_AND_TASK_NAME to 1 to prepend a time stamp, message number and the name of the calling task to each logged message.  Otherwise set to 0.
#endif

/* A block time of 0 just means don't block. */
#define loggingDONT_BLOCK 0

/*-----------------------------------------------------------*/

/*
 * The task that actually performs the print output.  Using a separate task
 * enables the use of slow output, such as as a UART, without the task that is
 * outputting the log message having to wait for the message to be completely
 * written.  Using a separate task also serialises access to the output port.
 *
 * The structure of this task is very simple; it blocks on a queue to wait for
 * a pointer to a string, sending any received strings to a macro that performs
 * the actual output.  The macro is port specific, so implemented outside of
 * this file.  This version uses dynamic memory, so the buffer that contained
 * the log message is freed after it has been output.
 */
static void prvLoggingTask( void * pvParameters );

/*-----------------------------------------------------------*/

/*
 * The queue used to pass pointers to log messages from the task that created
 * the message to the task that will performs the output.
 */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

BaseType_t xLoggingTaskInitialize( uint16_t usStackSize, UBaseType_t uxPriority, UBaseType_t uxQueueLength )
{
    BaseType_t xReturn = pdFAIL;

    /* Ensure the logging task has not been created already. */
    if( xQueue == NULL )
    {
        /* Create the queue used to pass pointers to strings to the logging task. */
        xQueue = xQueueCreate( uxQueueLength, sizeof( char ** ) );

        if( xQueue != NULL )
        {
            if( xTaskCreate( prvLoggingTask, "Logging", usStackSize, NULL, uxPriority, NULL ) == pdPASS )
            {
                xReturn = pdPASS;
            }
            else
            {
                /* Could not create the task, so delete the queue again. */
                vQueueDelete( xQueue );
            }
        }
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvLoggingTask( void *pvParameters )
{
    char *pcReceivedString = NULL;

    for( ;; )
    {
        /* Block to wait for the next string to print. */
        if( xQueueReceive( xQueue, &pcReceivedString, portMAX_DELAY ) == pdPASS )
        {
            configPRINT_STRING( pcReceivedString );
            vPortFree( ( void * ) pcReceivedString );
        }
    }
}
/*-----------------------------------------------------------*/

/*!
 * \brief Formats a string to be printed and sends it
 * to the print queue.
 *
 * Appends the message number, time (in ticks), and task
 * that called vLoggingPrintf to the beginning of each
 * print statement.
 *
 */
void vLoggingPrintf( const char *pcFormat, ... )
{
    size_t xLength = 0;
    int32_t xLength2 = 0;
    va_list args;
    char *pcPrintString = NULL;

    /* The queue is created by xLoggingTaskInitialize().  Check
    xLoggingTaskInitialize() has been called. */
    configASSERT( xQueue );

    /* Allocate a buffer to hold the log message. */
    pcPrintString = pvPortMalloc( configLOGGING_MAX_MESSAGE_LENGTH );

    if( pcPrintString != NULL )
    {
        /* There are a variable number of parameters. */
        va_start( args, pcFormat );

        if( strcmp( pcFormat, "\n" ) != 0 )
        {
            #if( configLOGGING_INCLUDE_TIME_AND_TASK_NAME == 1 )
            {
                const char *pcTaskName;
                const char *pcNoTask = "None";
                static BaseType_t xMessageNumber = 0;

                /* Add a time stamp and the name of the calling task to the
                start of the log. */
                if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
                {
                    pcTaskName = pcTaskGetName( NULL );
                }
                else
                {
                    pcTaskName = pcNoTask;
                }

                xLength = snprintf( pcPrintString, configLOGGING_MAX_MESSAGE_LENGTH, "%lu %lu [%s] ",
                    xMessageNumber++,
                    ( unsigned long ) xTaskGetTickCount(),
                    pcTaskName );
            }
            #else
            {
                xLength = 0;
            }
            #endif
        }

        xLength2 = vsnprintf( pcPrintString + xLength, configLOGGING_MAX_MESSAGE_LENGTH - xLength, pcFormat, args );

        if( xLength2 <  0 )
        {
            /* vsnprintf() failed. Restore the terminating NULL
             * character of the first part. Note that the first
             * part of the buffer may be empty if the value of
             * configLOGGING_INCLUDE_TIME_AND_TASK_NAME is not
             * 1 and as a result, the whole buffer may be empty.
             * That's the reason we have a check for xLength > 0
             * before sending the buffer to the logging task. */
            xLength2 = 0;
            pcPrintString[ xLength ] = '\0';
        }

        xLength += ( size_t ) xLength2;
        va_end( args );

        /* Only send the buffer to the logging task if it is
         * not empty. */
        if( xLength > 0 )
        {
            /* Send the string to the logging task for IO. */
            if( xQueueSend( xQueue, &pcPrintString, loggingDONT_BLOCK ) != pdPASS )
            {
                /* The buffer was not sent so must be freed again. */
                vPortFree( ( void * ) pcPrintString );
            }
        }
        else
        {
            /* The buffer was not sent, so it must be
             * freed. */
            vPortFree( ( void * ) pcPrintString );
        }
    }
}
/*-----------------------------------------------------------*/

void vLoggingPrint( const char * pcMessage )
{
    char * pcPrintString = NULL;
    size_t xLength = 0;

    /* The queue is created by xLoggingTaskInitialize().  Check
     * xLoggingTaskInitialize() has been called. */
    configASSERT( xQueue );

    xLength = strlen( pcMessage ) + 1;
    pcPrintString = pvPortMalloc( xLength );

    if( pcPrintString != NULL )
    {
        strncpy( pcPrintString, pcMessage, xLength );

        /* Send the string to the logging task for IO. */
        if( xQueueSend( xQueue, &pcPrintString, loggingDONT_BLOCK ) != pdPASS )
        {
            /* The buffer was not sent so must be freed again. */
            vPortFree( ( void * ) pcPrintString );
        }
    }
}
