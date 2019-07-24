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
 * @file utils.h
 * @brief Utility functions used by FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_UTILS_
#define _FREERTOS_POSIX_UTILS_

/* C standard library includes. */
#include <stdbool.h>
#include <stdint.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX/time.h"

/**
 * @brief Calculates the length of pcString, up to xMaxLength.
 *
 * @param[in] pcString The string to find the length of.
 * @param[in] xMaxLength The limit when searching for the end of pcString.
 *
 * @return 0 if pcString is NULL; otherwise, the length of pcString or xMaxLength,
 * whichever is smaller.
 */
size_t UTILS_strnlen( const char * const pcString,
                      size_t xMaxLength );

/**
 * @brief Calculates the number of ticks between now and a given timespec.
 *
 * @param[in] pxAbsoluteTime A time in the future, specified as seconds and
 * nanoseconds since CLOCK_REALTIME's 0.
 * @param[in] pxCurrentTime current time, specified as seconds and
 * nanoseconds.
 * @param[out] pxResult Where the result of the conversion is stored. The result
 * is rounded up for fractional ticks.
 *
 * @return 0 on success. Otherwise, ETIMEDOUT if pxAbsoluteTime is in the past,
 * or EINVAL for invalid parameters.
 */
int UTILS_AbsoluteTimespecToDeltaTicks( const struct timespec * const pxAbsoluteTime,
                                   const struct timespec * const pxCurrentTime,
                                   TickType_t * const pxResult );

/**
 * @brief Converts a struct timespec to FreeRTOS ticks.
 *
 * @param[in] pxTimespec The timespec to convert.
 * @param[out] Where the result of the conversion is stored. The result is rounded
 * up for fractional ticks.
 *
 * @return 0 on success. Otherwise, EINVAL for invalid parameters.
 */
int UTILS_TimespecToTicks( const struct timespec * const pxTimespec,
                           TickType_t * const pxResult );

/**
 * @brief Converts an integer value to a timespec.
 *
 * @param[in] llSource The value to convert.
 * @param[out] pxDestination Where to store the converted value.
 *
 * @return No return value.
 */
void UTILS_NanosecondsToTimespec( int64_t llSource,
                                  struct timespec * const pxDestination );

/**
 * @brief Calculates pxResult = x + y.
 *
 * @param[in] x The first argument for addition.
 * @param[in] y The second argument for addition.
 * @param[out] pxResult Where the result of the calculation is stored.
 *
 * @return -1 if any argument was NULL; 1 if result is negative (overflow); otherwise, 0.
 */
int UTILS_TimespecAdd( const struct timespec * const x,
                       const struct timespec * const y,
                       struct timespec * const pxResult );

/**
 * @brief Calculates pxResult = x + ( struct timespec ) nanosec.
 *
 * @param[in] x The first argument for addition.
 * @param[in] llNanoseconds The second argument for addition.
 * @param[out] pxResult Where the result of the calculation is stored.
 *
 * @return -1 if pxResult or x was NULL; 1 if result is negative; otherwise, 0.
 */
int UTILS_TimespecAddNanoseconds( const struct timespec * const x,
                                  int64_t llNanoseconds,
                                  struct timespec * const pxResult );

/**
 * @brief Calculates pxResult = x - y. If the result is negative contents of
 * pResult are undefined
 *
 * @param[in] x The first argument for subtraction.
 * @param[in] y The second argument for subtraction.
 * @param[out] pxResult Where the result of the calculation is stored.
 *
 * @return -1 if any argument was NULL; 1 if result is negative; otherwise, 0.
 */
int UTILS_TimespecSubtract( const struct timespec * const x,
                            const struct timespec * const y,
                            struct timespec * const pxResult);

/**
 * @brief Compare  x == y.
 *
 * @param[in] x The first argument for comparison.
 * @param[in] y The second argument for comparison.
 *
 * @return 0 if x == y; 1 if x > y; -1 if x < y or any argument was NULL
 */
int UTILS_TimespecCompare( const struct timespec * const x,
                           const struct timespec * const y);

/**
* @brief Checks that a timespec conforms to POSIX.
*
* A valid timespec must have 0 <= tv_nsec < 1000000000.
*
* @param[in] pxTimespec The timespec to validate.
*
* @return true if the pxTimespec is valid, false otherwise.
*/
bool UTILS_ValidateTimespec(const struct timespec * const pxTimespec);

#endif /* ifndef _FREERTOS_POSIX_UTILS_ */
