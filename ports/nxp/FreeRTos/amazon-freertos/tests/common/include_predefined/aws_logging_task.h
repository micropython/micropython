/*
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
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
