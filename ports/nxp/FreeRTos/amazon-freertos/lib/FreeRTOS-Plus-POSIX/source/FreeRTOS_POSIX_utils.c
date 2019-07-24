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
 * @file FreeRTOS_POSIX_utils.c
 * @brief Implementation of utility functions in utils.h
 */

/* C standard library includes. */
#include <stddef.h>
#include <limits.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/utils.h"

/*-----------------------------------------------------------*/

size_t UTILS_strnlen( const char * const pcString,
                      size_t xMaxLength )
{
    const char * pcCharPointer = pcString;
    size_t xLength = 0;

    if( pcString != NULL )
    {
        while( ( *pcCharPointer != '\0' ) && ( xLength < xMaxLength ) )
        {
            xLength++;
            pcCharPointer++;
        }
    }

    return xLength;
}

/*-----------------------------------------------------------*/

int UTILS_AbsoluteTimespecToDeltaTicks( const struct timespec * const pxAbsoluteTime,
                                   const struct timespec * const pxCurrentTime,
                                   TickType_t * const pxResult )
{
    int iStatus = 0;
    struct timespec xDifference = { 0 };

    /* Check parameters. */
    if( ( pxAbsoluteTime == NULL ) || ( pxCurrentTime == NULL ) || ( pxResult == NULL ) )
    {
        iStatus = EINVAL;
    }

    /* Calculate the difference between the current time and absolute time. */
    if( iStatus == 0 )
    {
        iStatus = UTILS_TimespecSubtract( pxAbsoluteTime, pxCurrentTime, &xDifference );
        if( iStatus == 1 )
        {
            /* pxAbsoluteTime was in the past. */
            iStatus = ETIMEDOUT;
        }
        else if( iStatus == -1 )
        {
            /* error */
            iStatus = EINVAL;
        }
    }

    /* Convert the time difference to ticks. */
    if( iStatus == 0 )
    {
        iStatus = UTILS_TimespecToTicks( &xDifference, pxResult );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int UTILS_TimespecToTicks( const struct timespec * const pxTimespec,
                           TickType_t * const pxResult )
{
    int iStatus = 0;
    int64_t llTotalTicks = 0;
    long lNanoseconds = 0;

    /* Check parameters. */
    if( ( pxTimespec == NULL ) || ( pxResult == NULL ) )
    {
        iStatus = EINVAL;
    }
    else if( ( iStatus == 0 ) && ( UTILS_ValidateTimespec( pxTimespec ) == false ) )
    {
        iStatus = EINVAL;
    }

    if( iStatus == 0 )
    {
        /* Convert timespec.tv_sec to ticks. */
        llTotalTicks = ( int64_t ) configTICK_RATE_HZ * ( pxTimespec->tv_sec );

        /* Convert timespec.tv_nsec to ticks. This value does not have to be checked
         * for overflow because a valid timespec has 0 <= tv_nsec < 1000000000 and
         * NANOSECONDS_PER_TICK > 1. */
        lNanoseconds = pxTimespec->tv_nsec / ( long ) NANOSECONDS_PER_TICK +                  /* Whole nanoseconds. */
                       ( long ) ( pxTimespec->tv_nsec % ( long ) NANOSECONDS_PER_TICK != 0 ); /* Add 1 to round up if needed. */

        /* Add the nanoseconds to the total ticks. */
        llTotalTicks += ( int64_t ) lNanoseconds;

        /* Check for overflow */
        if( llTotalTicks < 0 )
        {
            iStatus = EINVAL;
        }
        else
        {
            /* check if TickType_t is 32 bit or 64 bit */
            uint32_t ulTickTypeSize = sizeof( TickType_t );
            /* check for downcast overflow */
            if ( ulTickTypeSize == sizeof( uint32_t ) )
            {
                if ( llTotalTicks > UINT_MAX )
                {
                    iStatus = EINVAL;
                }
            }
        }

        /* Write result. */
        *pxResult = ( TickType_t ) llTotalTicks;
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

void UTILS_NanosecondsToTimespec( int64_t llSource,
                                  struct timespec * const pxDestination )
{
    long lCarrySec = 0;

    /* Convert to timespec. */
    pxDestination->tv_sec = ( time_t ) ( llSource / NANOSECONDS_PER_SECOND );
    pxDestination->tv_nsec = ( long ) ( llSource % NANOSECONDS_PER_SECOND );

    /* Subtract from tv_sec if tv_nsec < 0. */
    if( pxDestination->tv_nsec < 0L )
    {
        /* Compute the number of seconds to carry. */
        lCarrySec = ( pxDestination->tv_nsec / ( long ) NANOSECONDS_PER_SECOND ) + 1L;

        pxDestination->tv_sec -= ( time_t ) ( lCarrySec );
        pxDestination->tv_nsec += lCarrySec * ( long ) NANOSECONDS_PER_SECOND;
    }
}

/*-----------------------------------------------------------*/

int UTILS_TimespecAdd( const struct timespec * const x,
                       const struct timespec * const y,
                       struct timespec * const pxResult )
{
    int64_t llPartialSec = 0;
    int iStatus = 0;

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) || ( y == NULL ) )
    {
        iStatus = -1;
    }

    if( iStatus == 0 )
    {
        /* Perform addition. */
        pxResult->tv_nsec = x->tv_nsec + y->tv_nsec;

        /* check for overflow in case nsec value was invalid */
        if( pxResult->tv_nsec < 0 )
        {
            iStatus = 1;
        }
        else
        {
            llPartialSec = ( pxResult->tv_nsec ) / NANOSECONDS_PER_SECOND;
            pxResult->tv_nsec = ( pxResult->tv_nsec ) % NANOSECONDS_PER_SECOND;
            pxResult->tv_sec = x->tv_sec + y->tv_sec + llPartialSec;

            /* check for overflow */
            if(  pxResult->tv_sec < 0 )
            {
                iStatus = 1;
            }
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int UTILS_TimespecAddNanoseconds( const struct timespec * const x,
                                  int64_t llNanoseconds,
                                  struct timespec * const pxResult )
{
    int64_t llTotalNSec = 0;
    int iStatus = 0;

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) )
    {
        iStatus =  -1;
    }

    if( iStatus == 0 )
    {
        /* add nano seconds */
        llTotalNSec = x->tv_nsec + llNanoseconds;

        /* check for nano seconds overflow */
        if ( llTotalNSec < 0 )
        {
            iStatus = 1;
        }
        else
        {

            pxResult->tv_nsec =  llTotalNSec % NANOSECONDS_PER_SECOND;
            pxResult->tv_sec =  x->tv_sec + ( llTotalNSec / NANOSECONDS_PER_SECOND );

            /* check for seconds overflow */
            if(  pxResult->tv_sec < 0 )
            {
                iStatus = 1;
            }
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int UTILS_TimespecSubtract( const struct timespec * const x,
                            const struct timespec * const y,
                            struct timespec * const pxResult )
{

    int iCompareResult = 0;
    int iStatus = 0;

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) || ( y == NULL ) )
    {
        iStatus = -1;
    }

    if( iStatus == 0 )
    {
        iCompareResult = UTILS_TimespecCompare( x, y );

        /* if x < y then result would be negative, return 1 */
        if( iCompareResult == -1 )
        {
            iStatus = 1;
        }
        else if( iCompareResult == 0 )
        {
            /* if times are the same return zero */
            pxResult->tv_sec = 0;
            pxResult->tv_nsec = 0;
        }
        else
        {
            /* If x > y Perform subtraction. */
            pxResult->tv_sec = x->tv_sec - y->tv_sec;
            pxResult->tv_nsec = x->tv_nsec - y->tv_nsec;

            /* check if nano seconds value needs to borrow */
            if( pxResult->tv_nsec < 0 )
            {
                /* Based on comparison, tv_sec > 0 */
                pxResult->tv_sec--;
                pxResult->tv_nsec += (long) NANOSECONDS_PER_SECOND;
            }

            /* if nano second is negative after borrow, it is an overflow error */
            if( pxResult->tv_nsec < 0 )
            {
                iStatus = -1;
            }
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int UTILS_TimespecCompare( const struct timespec * const x,
                               const struct timespec * const y)
{
    int iStatus = 0;
    /* Check parameters */
    if( ( x == NULL ) && ( y == NULL ) )
    {
        iStatus = 0;
    }
    else if( y == NULL )
    {
        iStatus = 1;
    }
    else if( x == NULL )
    {
        iStatus = -1;
    }
    else if( x->tv_sec > y->tv_sec )
    {
        iStatus = 1;
    }
    else if( x->tv_sec < y->tv_sec )
    {
        iStatus = -1;
    }
    else
    {
        /* seconds are equal compare nano seconds */
        if( x->tv_nsec > y->tv_nsec )
        {
            iStatus = 1;
        }
        else if(x->tv_nsec < y->tv_nsec)
        {
            iStatus = -1;
        }
        else
        {
            iStatus = 0;
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

bool UTILS_ValidateTimespec( const struct timespec * const pxTimespec )
{
    bool xReturn = false;

    if( pxTimespec != NULL )
    {
        /* Verify 0 <= tv_nsec < 1000000000. */
        if( ( pxTimespec->tv_nsec >= 0 ) &&
            ( pxTimespec->tv_nsec < NANOSECONDS_PER_SECOND ) )
        {
            xReturn = true;
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
