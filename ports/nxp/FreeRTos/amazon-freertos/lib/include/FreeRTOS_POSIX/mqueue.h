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
 * @file mqueue.h
 * @brief Message queues.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html
 */

#ifndef _FREERTOS_POSIX_MQUEUE_H_
#define _FREERTOS_POSIX_MQUEUE_H_

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX/time.h"

/**
 * @brief Message queue descriptor.
 */
typedef void * mqd_t;

/**
 * @brief Message queue attributes.
 */
struct mq_attr
{
    long mq_flags;   /**< Message queue flags. */
    long mq_maxmsg;  /**< Maximum number of messages. */
    long mq_msgsize; /**< Maximum message size. */
    long mq_curmsgs; /**< Number of messages currently queued. */
};

/**
 * @brief Close a message queue.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_close.html
 */
int mq_close( mqd_t mqdes );

/**
 * @brief Get message queue attributes.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_getattr.html
 */
int mq_getattr( mqd_t mqdes,
                struct mq_attr * mqstat );

/**
 * @brief Open a message queue.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_open.html
 *
 * @note Currently, only the following oflags are implemented: O_RDWR, O_CREAT,
 * O_EXCL, and O_NONBLOCK. Also, mode is ignored.
 */
mqd_t mq_open( const char * name,
               int oflag,
               mode_t mode,
               struct mq_attr * attr );

/**
 * @brief Receive a message from a message queue.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_receive.html
 *
 * @note msg_prio is ignored. Messages are not checked for corruption.
 */
ssize_t mq_receive( mqd_t mqdes,
                    char * msg_ptr,
                    size_t msg_len,
                    unsigned int * msg_prio );

/**
 * @brief Send a message to a message queue.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_send.html
 *
 * @note msg_prio is ignored.
 */
int mq_send( mqd_t mqdes,
             const char * msg_ptr,
             size_t msg_len,
             unsigned msg_prio );

/**
 * @brief Receive a message from a message queue with timeout.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedreceive.html
 *
 * @note msg_prio is ignored. Messages are not checked for corruption.
 */
ssize_t mq_timedreceive( mqd_t mqdes,
                         char * msg_ptr,
                         size_t msg_len,
                         unsigned * msg_prio,
                         const struct timespec * abstime );

/**
 * @brief Send a message to a message queue with timeout.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_timedsend.html
 *
 * @note msg_prio is ignored.
 */
int mq_timedsend( mqd_t mqdes,
                  const char * msg_ptr,
                  size_t msg_len,
                  unsigned msg_prio,
                  const struct timespec * abstime );

/**
 * @brief Remove a message queue.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_unlink.html
 */
int mq_unlink( const char * name );

#endif /* ifndef _FREERTOS_POSIX_MQUEUE_H_ */
