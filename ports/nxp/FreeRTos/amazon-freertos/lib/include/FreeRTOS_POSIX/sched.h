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
 * @file sched.h
 * @brief Execution scheduling.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sched.h.html
 */

#ifndef _FREERTOS_POSIX_SCHED_H_
#define _FREERTOS_POSIX_SCHED_H_

/**
 * @defgroup Scheduling policies.
 */
/**@{ */
#define SCHED_FIFO        0 /**< First in-first out (FIFO) scheduling policy. */
#define SCHED_RR          1 /**< Round robin scheduling policy. */
#define SCHED_SPORADIC    2 /**< Sporadic server scheduling policy. */
#define SCHED_OTHER       3 /**< Another scheduling policy. */
/**@} */

/**
 * @brief Scheduling parameters required for implementation of each supported
 * scheduling policy.
 */
struct sched_param
{
    int sched_priority; /**< Process or thread execution scheduling priority. */
};

/**
 * @brief Get priority limit.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/sched_get_priority_max.html
 *
 * @note policy is ignored.
 */
int sched_get_priority_max( int policy );

/**
 * @brief Yield the processor.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/sched_yield.html
 */
int sched_yield( void );

#endif /* ifndef _FREERTOS_POSIX_SCHED_H_ */
