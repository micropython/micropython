/*
 * Amazon FreeRTOS V1.1.4
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

#ifndef AWS_LOGGING_TASK_H
#define AWS_LOGGING_TASK_H

/*
 * Called once to create the logging task and queue.  Must be called before any
 * calls to vLoggingPrintf().
 */
BaseType_t xLoggingTaskInitialize( uint16_t usStackSize,
                                   UBaseType_t uxPriority,
                                   UBaseType_t uxQueueLength );

/*
 * Uses the same semantics as printf().  How the print is performed depends on
 * which files are being built.  Some vLoggingPrintf() implementations will
 * output directly, others will use a logging task to allow log message to be
 * output in the background should the output device be too slow for output to
 * be performed inline.
 */
void vLoggingPrintf( const char * pcFormat,
                     ... );

#endif /* AWS_LOGGING_TASK_H */
