/*
 * Amazon FreeRTOS+POSIX V1.0.2
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

/**
 * @file FreeRTOS_POSIX_timer.c
 * @brief Implementation of timer functions in time.h
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/signal.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/utils.h"

/* FreeRTOS timer include. */
#include "timers.h"

/**
 * @defgroup Timespec zero check macros.
 */
/**@{ */
#define TIMESPEC_IS_ZERO( xTimespec )        ( xTimespec.tv_sec == 0 && xTimespec.tv_nsec == 0 ) /**< Check for 0. */
#define TIMESPEC_IS_NOT_ZERO( xTimespec )    ( !( TIMESPEC_IS_ZERO( xTimespec ) ) )              /**< Check for not 0. */
/**@} */

/**
 * @brief Internal timer structure.
 */
typedef struct timer_internal
{
    StaticTimer_t xTimerBuffer;  /**< Memory that holds the FreeRTOS timer. */
    struct sigevent xTimerEvent; /**< What to do when this timer expires. */
    TickType_t xTimerPeriod;     /**< Period of this timer. */
} timer_internal_t;

/*-----------------------------------------------------------*/

void prvTimerCallback( TimerHandle_t xTimerHandle )
{
    timer_internal_t * pxTimer = ( timer_internal_t * ) pvTimerGetTimerID( xTimerHandle );
    pthread_t xTimerNotificationThread;

    /* The value of the timer ID, set in timer_create, should not be NULL. */
    configASSERT( pxTimer != NULL );

    /* A value of SIGEV_SIGNAL isn't supported and should not have been successfully
     * set. */
    configASSERT( pxTimer->xTimerEvent.sigev_notify != SIGEV_SIGNAL );

    /* Update the timer period, which may need to be set to an it_interval
     * argument. This call should not block. */
    if( pxTimer->xTimerPeriod > 0 )
    {
        xTimerChangePeriod( xTimerHandle, pxTimer->xTimerPeriod, 0 );
    }

    /* Create the timer notification thread if requested. */
    if( pxTimer->xTimerEvent.sigev_notify == SIGEV_THREAD )
    {
        /* if the user has provided thread attributes, create a thread
         * with the provided attributes. Otherwise dispatch callback directly */
        if( pxTimer->xTimerEvent.sigev_notify_attributes == NULL )
        {
            ( *pxTimer->xTimerEvent.sigev_notify_function )( pxTimer->xTimerEvent.sigev_value );
        }
        else
        {
            ( void ) pthread_create( &xTimerNotificationThread,
                                     pxTimer->xTimerEvent.sigev_notify_attributes,
                                     ( void * ( * )( void * ) )pxTimer->xTimerEvent.sigev_notify_function,
                                     pxTimer->xTimerEvent.sigev_value.sival_ptr );
        }
    }
}

/*-----------------------------------------------------------*/

int timer_create( clockid_t clockid,
                  struct sigevent * evp,
                  timer_t * timerid )
{
    int iStatus = 0;
    timer_internal_t * pxTimer = NULL;

    /* Silence warnings about unused parameters. */
    ( void ) clockid;

    /* POSIX specifies that when evp is NULL, the behavior shall be as is
     * sigev_notify is SIGEV_SIGNAL. SIGEV_SIGNAL is currently not supported. */
    if( ( evp == NULL ) || ( evp->sigev_notify == SIGEV_SIGNAL ) )
    {
        errno = ENOTSUP;
        iStatus = -1;
    }

    /* Allocate memory for a new timer object. */
    if( iStatus == 0 )
    {
        pxTimer = pvPortMalloc( sizeof( timer_internal_t ) );

        if( pxTimer == NULL )
        {
            errno = EAGAIN;
            iStatus = -1;
        }
    }

    if( iStatus == 0 )
    {
        /* Copy the event notification structure and set the current timer period. */
        pxTimer->xTimerEvent = *evp;
        pxTimer->xTimerPeriod = 0;

        /* Create a new FreeRTOS timer. This call will not fail because the
         * memory for it has already been allocated, so the output parameter is
         * also set. */
        *timerid = ( timer_t ) xTimerCreateStatic( posixconfigTIMER_NAME,    /* Timer name. */
                                                   portMAX_DELAY,            /* Initial timer period. Timers are created disarmed. */
                                                   pdFALSE,                  /* Don't auto-reload timer. */
                                                   ( void * ) pxTimer,       /* Timer id. */
                                                   prvTimerCallback,         /* Timer expiration callback. */
                                                   &pxTimer->xTimerBuffer ); /* Pre-allocated memory for timer. */
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int timer_delete( timer_t timerid )
{
    TimerHandle_t xTimerHandle = timerid;
    timer_internal_t * pxTimer = ( timer_internal_t * ) pvTimerGetTimerID( xTimerHandle );

    /* The value of the timer ID, set in timer_create, should not be NULL. */
    configASSERT( pxTimer != NULL );

    /* Stop the FreeRTOS timer. Because the timer is statically allocated, no call
     * to xTimerDelete is necessary. The timer is stopped so that it's not referenced
     * anywhere. xTimerStop will not fail when it has unlimited block time. */
    ( void ) xTimerStop( xTimerHandle, portMAX_DELAY );

    /* Wait until the timer stop command is processed. */
    while( xTimerIsTimerActive( xTimerHandle ) == pdTRUE )
    {
        vTaskDelay( 1 );
    }

    /* Free the memory in use by the timer. */
    vPortFree( pxTimer );

    return 0;
}

/*-----------------------------------------------------------*/

int timer_getoverrun( timer_t timerid )
{
    /* Silence warnings about unused parameters. */
    ( void ) timerid;

    return 0;
}

/*-----------------------------------------------------------*/

int timer_settime( timer_t timerid,
                   int flags,
                   const struct itimerspec * value,
                   struct itimerspec * ovalue )
{
    int iStatus = 0;
    TimerHandle_t xTimerHandle = timerid;
    timer_internal_t * pxTimer = ( timer_internal_t * ) pvTimerGetTimerID( xTimerHandle );
    TickType_t xNextTimerExpiration = 0, xTimerExpirationPeriod = 0;

    /* Validate the value argument, but only if the timer isn't being disarmed. */
    if( TIMESPEC_IS_NOT_ZERO( value->it_value ) )
    {
        if( ( UTILS_ValidateTimespec( &value->it_interval ) == false ) ||
            ( UTILS_ValidateTimespec( &value->it_value ) == false ) )
        {
            errno = EINVAL;
            iStatus = -1;
        }
    }

    /* Set ovalue, if given. */
    if( ovalue != NULL )
    {
        ( void ) timer_gettime( timerid, ovalue );
    }

    /* Stop the timer if it's currently active. */
    if( ( iStatus == 0 ) && xTimerIsTimerActive( xTimerHandle ) )
    {
        ( void ) xTimerStop( xTimerHandle, portMAX_DELAY );
    }

    /* Only restart the timer if it_value is not zero. */
    if( ( iStatus == 0 ) && TIMESPEC_IS_NOT_ZERO( value->it_value ) )
    {
        /* Convert it_interval to ticks, but only if it_interval is not 0. If
         * it_interval is 0, then the timer is not periodic. */
        if( TIMESPEC_IS_NOT_ZERO( value->it_interval ) )
        {
            ( void ) UTILS_TimespecToTicks( &value->it_interval, &xTimerExpirationPeriod );
        }

        /* Set the new timer period. A non-periodic timer will have its period set
         * to portMAX_DELAY. */
        pxTimer->xTimerPeriod = xTimerExpirationPeriod;

        /* Convert it_value to ticks, but only if it_value is not 0. If it_value
         * is 0, then the timer will remain disarmed. */
        if( TIMESPEC_IS_NOT_ZERO( value->it_value ) )
        {
            /* Absolute timeout. */
            if( ( flags & TIMER_ABSTIME ) == TIMER_ABSTIME )
            {
                struct timespec xCurrentTime = { 0 };

                /* Get current time */
                if( clock_gettime( CLOCK_REALTIME, &xCurrentTime ) != 0 )
                {
                    iStatus = EINVAL;
                }
                else
                {
                    iStatus = UTILS_AbsoluteTimespecToDeltaTicks( &value->it_value, &xCurrentTime, &xNextTimerExpiration );
                }

                /* Make sure xNextTimerExpiration is zero in case we got negative time difference */
                if( iStatus != 0 )
                {
                    xNextTimerExpiration = 0;

                    if ( iStatus == ETIMEDOUT )
                    {
                        /* Set Status to 0 as absolute time is past is treated as expiry but not an error */
                        iStatus = 0;
                    }
                }
            }
            /* Relative timeout. */
            else
            {
                ( void ) UTILS_TimespecToTicks( &value->it_value, &xNextTimerExpiration );
            }
        }

        /* If xNextTimerExpiration is still 0, that means that it_value specified
         * an absolute timeout in the past. Per POSIX spec, a notification should be
         * triggered immediately. */
        if( xNextTimerExpiration == 0 )
        {
            prvTimerCallback( xTimerHandle );
        }
        else
        {
            /* Set the timer to expire at the it_value, then start it. */
            ( void ) xTimerChangePeriod( xTimerHandle, xNextTimerExpiration, portMAX_DELAY );
            ( void ) xTimerStart( xTimerHandle, xNextTimerExpiration );
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int timer_gettime( timer_t timerid,
                   struct itimerspec * value )
{
    TimerHandle_t xTimerHandle = timerid;
    timer_internal_t * pxTimer = ( timer_internal_t * ) pvTimerGetTimerID( xTimerHandle );
    TickType_t xNextExpirationTime = xTimerGetExpiryTime( xTimerHandle ) - xTaskGetTickCount(),
               xTimerExpirationPeriod = pxTimer->xTimerPeriod;

    /* Set it_value only if the timer is armed. Otherwise, set it to 0. */
    if( xTimerIsTimerActive( xTimerHandle ) != pdFALSE )
    {
        value->it_value.tv_sec = ( time_t ) ( xNextExpirationTime / configTICK_RATE_HZ );
        value->it_value.tv_nsec = ( long ) ( ( xNextExpirationTime % configTICK_RATE_HZ ) * NANOSECONDS_PER_TICK );
    }
    else
    {
        value->it_value.tv_sec = 0;
        value->it_value.tv_nsec = 0;
    }

    /* Set it_interval only if the timer is periodic. Otherwise, set it to 0. */
    if( xTimerExpirationPeriod != portMAX_DELAY )
    {
        value->it_interval.tv_sec = ( time_t ) ( xTimerExpirationPeriod / configTICK_RATE_HZ );
        value->it_interval.tv_nsec = ( long ) ( ( xTimerExpirationPeriod % configTICK_RATE_HZ ) * NANOSECONDS_PER_TICK );
    }
    else
    {
        value->it_interval.tv_sec = 0;
        value->it_interval.tv_nsec = 0;
    }

    return 0;
}

/*-----------------------------------------------------------*/
