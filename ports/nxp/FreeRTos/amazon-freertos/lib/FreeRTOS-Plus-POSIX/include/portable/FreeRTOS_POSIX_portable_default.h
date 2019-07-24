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
 * @file FreeRTOS_POSIX_portable_default.h
 * @brief Defaults for port-specific configuration of FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_PORTABLE_DEFAULT_H_
#define _FREERTOS_POSIX_PORTABLE_DEFAULT_H_

/**
 * @brief The FreeRTOS task name given to pthreads.
 */
#ifndef posixconfigPTHREAD_TASK_NAME
    #define posixconfigPTHREAD_TASK_NAME    "pthread"
#endif

/**
 * @brief the FreeRTOS timer name given to POSIX timers.
 */
#ifndef posixconfigTIMER_NAME
    #define posixconfigTIMER_NAME    "timer"
#endif

/**
 * @defgroup Defaults for POSIX message queue implementation.
 */
/**@{ */
#ifndef posixconfigMQ_MAX_MESSAGES
    #define posixconfigMQ_MAX_MESSAGES    10 /**< Maximum number of messages in an mq at one time. */
#endif

#ifndef posixconfigMQ_MAX_SIZE
    #define posixconfigMQ_MAX_SIZE    128 /**< Maximum size (in bytes) of each message. */
#endif
/**@} */

/**
 * @defgroup POSIX implementation-dependent constants usually defined in limits.h.
 *
 * They are defined here to provide portability between platforms.
 */
/**@{ */
#ifndef PTHREAD_STACK_MIN
    #define PTHREAD_STACK_MIN    configMINIMAL_STACK_SIZE * sizeof( StackType_t ) /**< Minimum size in bytes of thread stack storage. */
#endif
#ifndef NAME_MAX
    #define NAME_MAX             64                                               /**< Maximum number of bytes in a filename (not including terminating null). */
#endif
#ifndef SEM_VALUE_MAX
    #define SEM_VALUE_MAX        0xFFFFU                                          /**< Maximum value of a sem_t. */
#endif
/**@} */

/**
 * @defgroup Enable typedefs of POSIX types.
 *
 * Set these values to 1 or 0 to enable or disable the typedefs, respectively.
 * These typedefs should only be disabled if they conflict with system typedefs.
 */
/**@{ */
#ifndef posixconfigENABLE_CLOCK_T
    #define posixconfigENABLE_CLOCK_T                1 /**< clock_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_CLOCKID_T
    #define posixconfigENABLE_CLOCKID_T              1 /**< clockid_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_MODE_T
    #define posixconfigENABLE_MODE_T                 1 /**< mode_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PID_T
    #define posixconfigENABLE_PID_T                  1 /**< pid_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_ATTR_T
    #define posixconfigENABLE_PTHREAD_ATTR_T         1 /**< pthread_attr_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_COND_T
    #define posixconfigENABLE_PTHREAD_COND_T         1 /**< pthread_cond_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_CONDATTR_T
    #define posixconfigENABLE_PTHREAD_CONDATTR_T     1 /**< pthread_condattr_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_MUTEX_T
    #define posixconfigENABLE_PTHREAD_MUTEX_T        1 /**< pthread_mutex_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_MUTEXATTR_T
    #define posixconfigENABLE_PTHREAD_MUTEXATTR_T    1 /**< pthread_mutexattr_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_T
    #define posixconfigENABLE_PTHREAD_T              1 /**< pthread_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_SSIZE_T
    #define posixconfigENABLE_SSIZE_T                1 /**< ssize_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_TIME_T
    #define posixconfigENABLE_TIME_T                 1 /**< time_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_TIMER_T
    #define posixconfigENABLE_TIMER_T                1 /**< timer_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_USECONDS_T
    #define posixconfigENABLE_USECONDS_T             1 /**< useconds_t in sys/types.h */
#endif
#ifndef posixconfigENABLE_TIMESPEC
    #define posixconfigENABLE_TIMESPEC               1 /**< struct timespec in time.h */
#endif
#ifndef posixconfigENABLE_ITIMERSPEC
    #define posixconfigENABLE_ITIMERSPEC             1 /**< struct itimerspec in time.h */
#endif
#ifndef posixconfigENABLE_TM
    #define posixconfigENABLE_TM                     1 /**< struct tm in time.h */
#endif
#ifndef posixconfigENABLE_SEM_T
    #define posixconfigENABLE_SEM_T                  1 /**< struct sem_t in semaphore.h */
#endif
#ifndef posixconfigENABLE_PTHREAD_BARRIER_T
    #define posixconfigENABLE_PTHREAD_BARRIER_T      1 /**< pthread_barrier_t in sys/types.h */
#endif
/**@} */

#endif /* ifndef _FREERTOS_POSIX_PORTABLE_DEFAULT_H_ */
