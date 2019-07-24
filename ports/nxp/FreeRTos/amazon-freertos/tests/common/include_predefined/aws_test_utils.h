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

#ifndef _AWS_TEST_UTILS_H_
#define _AWS_TEST_UTILS_H_

/**
 * @brief      Retry expression or statement with exponential backoff
 *
 * @param      xCommand            The expression or statement that should be
 *                                 retried
 * @param      xSuccessStatus      The success status where a xCommand need not
 *                                 be retried any more
 * @param      ulStartingPeriodMs  The initial delay period in milliseconds
 * @param      lRetries            The number of times to retry.  xCommand will
 *                                 be tried once, and then retried n times.
 *
 * @code
 *  int a = 0;
 *  RETRY_EXPONENTIAL( printf( "foo\n" ), 4, 150, 8 );
 *  RETRY_EXPONENTIAL( a = printf( "bar\n" ), 0, 250, 8 );
 *  RETRY_EXPONENTIAL( a = printf( "bar\n" ), 0, 250, 8 );
 *  RETRY_EXPONENTIAL( a = connect_to_server(), CONNECTION_SUCCESS, 250, 8 );
 *  RETRY_EXPONENTIAL( a++, 10, 250, 8 );
 * @endcode
 *
 * @return     None
 */
#define RETRY_EXPONENTIAL(                                               \
        xCommand, xSuccessStatus, ulStartingPeriodMs, lRetries )         \
    {                                                                    \
        int32_t lRetried = 0;                                            \
        uint32_t ulPeriodMs = ulStartingPeriodMs;                        \
        int32_t lStatus;                                                 \
        for( ; lRetried <= lRetries; lRetried++ ) {                      \
            if( lRetried ) {                                             \
                configPRINTF( ( "retrying \"%s\", %d of %d, in %d ms\n", \
                                # xCommand, lRetried,                    \
                                lRetries, ulPeriodMs ) );                \
                vTaskDelay( pdMS_TO_TICKS( ulPeriodMs ) );               \
                ulPeriodMs *= 2;                                         \
            }                                                            \
            lStatus = xCommand;                                          \
            if( xSuccessStatus == lStatus ) {                            \
                break;                                                   \
            }                                                            \
            configPRINTF( ( "expected %d, got %d\n",                     \
                            xSuccessStatus, lStatus ) );                 \
        }                                                                \
    }

/**
 * @brief      Returns the file name at the end of a windows path
 *
 * @param      full_path  The full path
 *
 * @return     file name
 */
#define WIN_FILENAME( full_path ) \
    ( strrchr( full_path, '\\' ) ? strrchr( full_path, '\\' ) + 1 : full_path )

/**
 * @brief      Returns the file name at the end of a linux path
 *
 * @param      full_path  The full path
 *
 * @return     file name
 */
#define NIX_FILENAME( full_path ) \
    ( strrchr( full_path, '/' ) ? strrchr( full_path, '/' ) + 1 : full_path )

/**
 * The name of the current file, stripped of the path
 */
#define __FILENAME__    WIN_FILENAME( NIX_FILENAME( __FILE__ ) )


#endif /* end of include guard: _AWS_TEST_UTILS_H_ */
