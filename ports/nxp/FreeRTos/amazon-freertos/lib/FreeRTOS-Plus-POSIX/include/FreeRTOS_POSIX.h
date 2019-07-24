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
 * @file FreeRTOS_POSIX.h
 * @brief FreeRTOS+POSIX header.
 *
 * This file must be included before all other FreeRTOS+POSIX includes.
 */

#ifndef _FREERTOS_POSIX_H_
#define _FREERTOS_POSIX_H_

/* FreeRTOS+POSIX platform-specific configuration headers. */
#include "FreeRTOS_POSIX_portable.h"
#include "portable/FreeRTOS_POSIX_portable_default.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"

/* FreeRTOS+POSIX data types and internal structs. */
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX_internal.h"

#endif /* _FREERTOS_POSIX_H_ */
