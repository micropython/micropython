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

#ifndef _FREERTOS_POSIX_INTERNAL_TYPES_H_
#define _FREERTOS_POSIX_INTERNAL_TYPES_H_

#include "FreeRTOS_POSIX_internal.h"

/*
 * sys/types.h defines a POSIX type when posixconfigENABLE_PTHREAD_<TYPE>_T
 * is not defined AND when posixconfigENABLE_PTHREAD_<TYPE>_T is set to 1.
 * FreeRTOS_POSIX_internal.h defines internal type ONLY when
 * posixconfigENABLE_PTHREAD_<TYPE>_T is set to 1.
 * #else part below is to have a type defined, so the code compiles, when
 * posixconfigENABLE_PTHREAD_<TYPE>_T is not defined.
 */
#if posixconfigENABLE_PTHREAD_MUTEX_T == 1
    typedef pthread_mutex_internal_t PthreadMutexType_t;
#else
    typedef void    * PthreadMutexType_t;
#endif

#if posixconfigENABLE_PTHREAD_COND_T == 1
    typedef pthread_cond_internal_t PthreadCondType_t;
#else
    typedef void    * PthreadCondType_t;
#endif

#if posixconfigENABLE_SEM_T == 1
    typedef sem_internal_t PosixSemType_t;
#else
    typedef void    * PosixSemType_t;
#endif

#if posixconfigENABLE_PTHREAD_MUTEXATTR_T == 1
    typedef struct pthread_mutexattr {
        uint32_t        ulpthreadMutexAttrStorage;
    } PthreadMutexAttrType_t;
#else
    typedef void    * PthreadMutexAttrType_t;
#endif

#if posixconfigENABLE_PTHREAD_ATTR_T == 1
    typedef struct pthread_attr {
        uint32_t ulpthreadAttrStorage;
    } PthreadAttrType_t;
#else
    typedef void    * PthreadAttrType_t;
#endif

#if posixconfigENABLE_PTHREAD_BARRIER_T == 1
    typedef pthread_barrier_internal_t PthreadBarrierType_t;
#else
    typedef void    * PthreadBarrierType_t;
#endif

#endif /* _FREERTOS_POSIX_INTERNAL_TYPES_H_ */
