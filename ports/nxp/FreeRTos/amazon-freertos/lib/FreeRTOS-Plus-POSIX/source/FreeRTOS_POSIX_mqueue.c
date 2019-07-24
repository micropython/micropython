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
 * @file FreeRTOS_POSIX_mqueue.c
 * @brief Implementation of message queue functions in mqueue.h
 */

/* C standard library includes. */
#include <string.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/utils.h"

/**
 * @brief Element of the FreeRTOS queues that store mq data.
 */
typedef struct QueueElement
{
    char * pcData;    /**< Data in queue. Type char* to match msg_ptr. */
    size_t xDataSize; /**< Size of data pointed by pcData. */
} QueueElement_t;

/**
 * @brief Data structure of an mq.
 *
 * FreeRTOS isn't guaranteed to have a file-like abstraction, so message
 * queues in this implementation are stored as a linked list (in RAM).
 */
typedef struct QueueListElement
{
    Link_t xLink;              /**< Pointer to the next element in the list. */
    QueueHandle_t xQueue;      /**< FreeRTOS queue handle. */
    size_t xOpenDescriptors;   /**< Number of threads that have opened this queue. */
    char * pcName;             /**< Null-terminated queue name. */
    struct mq_attr xAttr;      /**< Queue attibutes. */
    BaseType_t xPendingUnlink; /**< If pdTRUE, this queue will be unlinked once all descriptors close. */
} QueueListElement_t;

/*-----------------------------------------------------------*/

/**
 * @brief Convert an absolute timespec into a tick timeout, taking into account
 * queue flags.
 *
 * @param[in] lMessageQueueFlags Message queue flags to consider.
 * @param[in] pxAbsoluteTimeout The absolute timespec to convert.
 * @param[out] pxTimeoutTicks Output parameter of the timeout in ticks.
 *
 * @return 0 if successful; EINVAL if pxAbsoluteTimeout is invalid, or ETIMEDOUT
 * if pxAbsoluteTimeout is in the past.
 */
static int prvCalculateTickTimeout( long lMessageQueueFlags,
                                    const struct timespec * const pxAbsoluteTimeout,
                                    TickType_t * pxTimeoutTicks );

/**
 * @brief Add a new queue to the queue list.
 *
 * @param[out] ppxMessageQueue Pointer to new queue.
 * @param[in] pxAttr mq_attr of the new queue.
 * @param[in] pcName Name of new queue.
 * @param[in] xNameLength Length of pcName.
 *
 * @return pdTRUE if the queue is found; pdFALSE otherwise.
 */
static BaseType_t prvCreateNewMessageQueue( QueueListElement_t ** ppxMessageQueue,
                                            const struct mq_attr * const pxAttr,
                                            const char * const pcName,
                                            size_t xNameLength );

/**
 * @brief Free all the resources used by a message queue.
 *
 * @param[out] pxMessageQueue Pointer to queue to free.
 *
 * @return nothing
 */
static void prvDeleteMessageQueue( const QueueListElement_t * const pxMessageQueue );

/**
 * @brief Attempt to find the queue identified by pcName or xMqId in the queue list.
 *
 * Matches queues by pcName first; if pcName is NULL, matches by xMqId.
 * @param[out] ppxQueueListElement Output parameter set when queue is found.
 * @param[in] pcName A queue name to match.
 * @param[in] xMessageQueueDescriptor A queue descriptor to match.
 *
 * @return pdTRUE if the queue is found; pdFALSE otherwise.
 */
static BaseType_t prvFindQueueInList( QueueListElement_t ** const ppxQueueListElement,
                                      const char * const pcName,
                                      mqd_t xMessageQueueDescriptor );

/**
 * @brief Initialize the queue list.
 *
 * Performs initialization of the queue list mutex and queue list head.
 *
 * @return nothing
 */
static void prvInitializeQueueList( void );

/**
 * @brief Checks that pcName is a valid name for a message queue.
 *
 * Also outputs the length of pcName.
 * @param[in] pcName The name to check.
 * @param[out] pxNameLength Output parameter for name length.
 *
 * @return pdTRUE if the name is valid; pdFALSE otherwise.
 */
static BaseType_t prvValidateQueueName( const char * const pcName,
                                        size_t * pxNameLength );

/**
 * @brief Guards access to the list of message queues.
 */
static StaticSemaphore_t xQueueListMutex = { { 0 }, .u = { 0 } };

/**
 * @brief Head of the linked list of queues.
 */
static Link_t xQueueListHead = { 0 };

/*-----------------------------------------------------------*/

static int prvCalculateTickTimeout( long lMessageQueueFlags,
                                    const struct timespec * const pxAbsoluteTimeout,
                                    TickType_t * pxTimeoutTicks )
{
    int iStatus = 0;

    /* Check for nonblocking queue. */
    if( lMessageQueueFlags & O_NONBLOCK )
    {
        /* No additional checks are done for nonblocking queues. Timeout is 0. */
        *pxTimeoutTicks = 0;
    }
    else
    {
        /* No absolute timeout given. Block forever. */
        if( pxAbsoluteTimeout == NULL )
        {
            *pxTimeoutTicks = portMAX_DELAY;
        }
        else
        {
            struct timespec xCurrentTime = { 0 };

            /* Check that the given timespec is valid. */
            if( UTILS_ValidateTimespec( pxAbsoluteTimeout ) == false )
            {
                iStatus = EINVAL;
            }

            /* Get current time */
            if( ( iStatus == 0 ) && ( clock_gettime( CLOCK_REALTIME, &xCurrentTime ) != 0 ) )
            {
                iStatus = EINVAL;
            }

            /* Convert absolute timespec to ticks. */
            if( ( iStatus == 0 ) &&
                ( UTILS_AbsoluteTimespecToDeltaTicks( pxAbsoluteTimeout, &xCurrentTime, pxTimeoutTicks ) != 0 ) )
            {
                iStatus = ETIMEDOUT;
            }
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCreateNewMessageQueue( QueueListElement_t ** ppxMessageQueue,
                                            const struct mq_attr * const pxAttr,
                                            const char * const pcName,
                                            size_t xNameLength )
{
    BaseType_t xStatus = pdTRUE;

    /* Allocate space for a new queue element. */
    *ppxMessageQueue = pvPortMalloc( sizeof( QueueListElement_t ) );

    /* Check that memory allocation succeeded. */
    if( *ppxMessageQueue == NULL )
    {
        xStatus = pdFALSE;
    }

    /* Create the FreeRTOS queue. */
    if( xStatus == pdTRUE )
    {
        ( *ppxMessageQueue )->xQueue =
            xQueueCreate( pxAttr->mq_maxmsg, sizeof( QueueElement_t ) );

        /* Check that queue creation succeeded. */
        if( ( *ppxMessageQueue )->xQueue == NULL )
        {
            vPortFree( *ppxMessageQueue );
            xStatus = pdFALSE;
        }
    }

    if( xStatus == pdTRUE )
    {
        /* Allocate space for the queue name plus null-terminator. */
        ( *ppxMessageQueue )->pcName = pvPortMalloc( xNameLength + 1 );

        /* Check that memory was successfully allocated for queue name. */
        if( ( *ppxMessageQueue )->pcName == NULL )
        {
            vQueueDelete( ( *ppxMessageQueue )->xQueue );
            vPortFree( *ppxMessageQueue );
            xStatus = pdFALSE;
        }
        else
        {
            /* Copy queue name. Copying xNameLength+1 will cause strncpy to add
             * the null-terminator. */
            ( void ) strncpy( ( *ppxMessageQueue )->pcName, pcName, xNameLength + 1 );
        }
    }

    if( xStatus == pdTRUE )
    {
        /* Copy attributes. */
        ( *ppxMessageQueue )->xAttr = *pxAttr;

        /* A newly-created queue will have 1 open descriptor for it. */
        ( *ppxMessageQueue )->xOpenDescriptors = 1;

        /* A newly-created queue will not be pending unlink. */
        ( *ppxMessageQueue )->xPendingUnlink = pdFALSE;

        /* Add the new queue to the list. */
        listADD( &xQueueListHead, &( *ppxMessageQueue )->xLink );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvDeleteMessageQueue( const QueueListElement_t * const pxMessageQueue )
{
    QueueElement_t xQueueElement = { 0 };

    /* Free all data in the queue. It's assumed that no more data will be added
     * to the queue, so xQueueReceive does not block. */
    while( xQueueReceive( pxMessageQueue->xQueue,
                          ( void * ) &xQueueElement,
                          0 ) == pdTRUE )
    {
        vPortFree( xQueueElement.pcData );
    }

    /* Free memory used by this message queue. */
    vQueueDelete( pxMessageQueue->xQueue );
    vPortFree( ( void * ) pxMessageQueue->pcName );
    vPortFree( ( void * ) pxMessageQueue );
}

/*-----------------------------------------------------------*/

static BaseType_t prvFindQueueInList( QueueListElement_t ** const ppxQueueListElement,
                                      const char * const pcName,
                                      mqd_t xMessageQueueDescriptor )
{
    Link_t * pxQueueListLink = NULL;
    QueueListElement_t * pxMessageQueue = NULL;
    BaseType_t xQueueFound = pdFALSE;

    /* Iterate through the list of queues. */
    listFOR_EACH( pxQueueListLink, &xQueueListHead )
    {
        pxMessageQueue = listCONTAINER( pxQueueListLink, QueueListElement_t, xLink );

        /* Match by name first if provided. */
        if( ( pcName != NULL ) && ( strcmp( pxMessageQueue->pcName, pcName ) == 0 ) )
        {
            xQueueFound = pdTRUE;
            break;
        }
        /* If name doesn't match, match by descriptor. */
        else
        {
            if( ( mqd_t ) pxMessageQueue == xMessageQueueDescriptor )
            {
                xQueueFound = pdTRUE;
                break;
            }
        }
    }

    /* If the queue was found, set the output parameter. */
    if( ( xQueueFound == pdTRUE ) && ( ppxQueueListElement != NULL ) )
    {
        *ppxQueueListElement = pxMessageQueue;
    }

    return xQueueFound;
}

/*-----------------------------------------------------------*/

static void prvInitializeQueueList( void )
{
    /* Keep track of whether the queue list has been initialized. */
    static BaseType_t xQueueListInitialized = pdFALSE;

    /* Check if queue list needs to be initialized. */
    if( xQueueListInitialized == pdFALSE )
    {
        /* Initialization must be in a critical section to prevent two threads
         * from initializing at the same time. */
        taskENTER_CRITICAL();

        /* Check again that queue list is still uninitialized, i.e. it wasn't
         * initialized while this function was waiting to enter the critical
         * section. */
        if( xQueueListInitialized == pdFALSE )
        {
            /* Initialize the queue list mutex and list head. */
            ( void ) xSemaphoreCreateMutexStatic( &xQueueListMutex );
            listINIT_HEAD( &xQueueListHead );
            xQueueListInitialized = pdTRUE;
        }

        /* Exit the critical section. */
        taskEXIT_CRITICAL();
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvValidateQueueName( const char * const pcName,
                                        size_t * pxNameLength )
{
    BaseType_t xStatus = pdTRUE;
    size_t xNameLength = 0;

    /* All message queue names must start with '/'. */
    if( pcName[ 0 ] != '/' )
    {
        xStatus = pdFALSE;
    }
    else
    {
        /* Get the length of pcName, excluding the first '/' and null-terminator. */
        xNameLength = UTILS_strnlen( pcName, NAME_MAX + 2 );

        if( xNameLength == NAME_MAX + 2 )
        {
            /* Name too long. */
            xStatus = pdFALSE;
        }
        else
        {
            /* Name length passes, set output parameter. */
            *pxNameLength = xNameLength;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

int mq_close( mqd_t mqdes )
{
    int iStatus = 0;
    QueueListElement_t * pxMessageQueue = ( QueueListElement_t * ) mqdes;
    BaseType_t xQueueRemoved = pdFALSE;

    /* Initialize the queue list, if needed. */
    prvInitializeQueueList();

    /* Lock the mutex that guards access to the queue list. This call will
     * never fail because it blocks forever. */
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

    /* Attempt to find the message queue based on the given descriptor. */
    if( prvFindQueueInList( NULL, NULL, mqdes ) == pdTRUE )
    {
        /* Decrement the number of open descriptors. */
        if(pxMessageQueue->xOpenDescriptors > 0)
        {
            pxMessageQueue->xOpenDescriptors--;
        }

        /* Check if the queue has any more open descriptors. */
        if( pxMessageQueue->xOpenDescriptors == 0 )
        {
            /* If no open descriptors remain and mq_unlink has already been called,
             * remove the queue. */
            if( pxMessageQueue->xPendingUnlink == pdTRUE )
            {
                listREMOVE( &pxMessageQueue->xLink );

                /* Set the flag to delete the queue. Deleting the queue is deferred
                 * until xQueueListMutex is released. */
                xQueueRemoved = pdTRUE;
            }
            /* Otherwise, wait for the call to mq_unlink. */
            else
            {
                pxMessageQueue->xPendingUnlink = pdTRUE;
            }
        }
    }
    else
    {
        /* Queue not found; bad descriptor. */
        errno = EBADF;
        iStatus = -1;
    }

    /* Release the mutex protecting the queue list. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );

    /* Delete all resources used by the queue if needed. */
    if( xQueueRemoved == pdTRUE )
    {
        prvDeleteMessageQueue( pxMessageQueue );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int mq_getattr( mqd_t mqdes,
                struct mq_attr * mqstat )
{
    int iStatus = 0;
    QueueListElement_t * pxMessageQueue = ( QueueListElement_t * ) mqdes;

    /* Lock the mutex that guards access to the queue list. This call will
     * never fail because it blocks forever. */
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

    /* Find the mq referenced by mqdes. */
    if( prvFindQueueInList( NULL, NULL, mqdes ) == pdTRUE )
    {
        /* Update the number of messages in the queue and copy the attributes
         * into mqstat. */
        pxMessageQueue->xAttr.mq_curmsgs = ( long ) uxQueueMessagesWaiting( pxMessageQueue->xQueue );
        *mqstat = pxMessageQueue->xAttr;
    }
    else
    {
        /* Queue not found; bad descriptor. */
        errno = EBADF;
        iStatus = -1;
    }

    /* Release the mutex protecting the queue list. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );

    return iStatus;
}

/*-----------------------------------------------------------*/

mqd_t mq_open( const char * name,
               int oflag,
               mode_t mode,
               struct mq_attr * attr )
{
    mqd_t xMessageQueue = NULL;
    size_t xNameLength = 0;

    /* Default mq_attr. */
    struct mq_attr xQueueCreationAttr =
    {
        .mq_flags   =                          0,
        .mq_maxmsg  = posixconfigMQ_MAX_MESSAGES,
        .mq_msgsize = posixconfigMQ_MAX_SIZE,
        .mq_curmsgs = 0
    };

    /* Silence warnings about unused parameters. */
    ( void ) mode;

    /* Initialize the queue list, if needed. */
    prvInitializeQueueList();

    /* Check queue name. */
    if( prvValidateQueueName( name, &xNameLength ) == pdFALSE )
    {
        /* Invalid name. */
        errno = EINVAL;
        xMessageQueue = ( mqd_t ) -1;
    }

    /* Check attributes, if given. */
    if( xMessageQueue == NULL )
    {
        if( ( attr != NULL ) && ( ( attr->mq_maxmsg < 0 ) || ( attr->mq_msgsize < 0 ) ) )
        {
            /* Invalid mq_attr.mq_maxmsg or mq_attr.mq_msgsize. */
            errno = EINVAL;
            xMessageQueue = ( mqd_t ) -1;
        }
    }

    if( xMessageQueue == NULL )
    {
        /* Lock the mutex that guards access to the queue list. This call will
         * never fail because it blocks forever. */
        ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

        /* Search the queue list to check if the queue exists. */
        if( prvFindQueueInList( ( QueueListElement_t ** ) &xMessageQueue,
                                name,
                                ( mqd_t ) NULL ) == pdTRUE )
        {
            /* If the mq exists, check that this function wasn't called with
             * O_CREAT and O_EXCL. */
            if( ( oflag & O_EXCL ) && ( oflag & O_CREAT ) )
            {
                errno = EEXIST;
                xMessageQueue = ( mqd_t ) -1;
            }
            else
            {
                /* Check if the mq has been unlinked and is pending removal. */
                if( ( ( QueueListElement_t * ) xMessageQueue )->xPendingUnlink == pdTRUE )
                {
                    /* Queue pending deletion. Don't allow it to be re-opened. */
                    errno = EINVAL;
                    xMessageQueue = ( mqd_t ) -1;
                }
                else
                {
                    /* Increase count of open file descriptors for queue. */
                    ( ( QueueListElement_t * ) xMessageQueue )->xOpenDescriptors++;
                }
            }
        }
        /* Queue does not exist. */
        else
        {
            /* Only create the new queue if O_CREAT was specified. */
            if( oflag & O_CREAT )
            {
                /* Copy attributes if provided. */
                if( attr != NULL )
                {
                    xQueueCreationAttr = *attr;
                }

                /* Copy oflags. */
                xQueueCreationAttr.mq_flags = ( long ) oflag;

                /* Create the new message queue. */
                if( prvCreateNewMessageQueue( ( QueueListElement_t ** ) &xMessageQueue,
                                              &xQueueCreationAttr,
                                              name,
                                              xNameLength ) == pdFALSE )
                {
                    errno = ENOSPC;
                    xMessageQueue = ( mqd_t ) -1;
                }
            }
            else
            {
                errno = ENOENT;
                xMessageQueue = ( mqd_t ) -1;
            }
        }

        /* Release the mutex protecting the queue list. */
        ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );
    }

    return xMessageQueue;
}

/*-----------------------------------------------------------*/

ssize_t mq_receive( mqd_t mqdes,
                    char * msg_ptr,
                    size_t msg_len,
                    unsigned int * msg_prio )
{
    return mq_timedreceive( mqdes, msg_ptr, msg_len, msg_prio, NULL );
}

/*-----------------------------------------------------------*/

int mq_send( mqd_t mqdes,
             const char * msg_ptr,
             size_t msg_len,
             unsigned msg_prio )
{
    return mq_timedsend( mqdes, msg_ptr, msg_len, msg_prio, NULL );
}

/*-----------------------------------------------------------*/

ssize_t mq_timedreceive( mqd_t mqdes,
                         char * msg_ptr,
                         size_t msg_len,
                         unsigned * msg_prio,
                         const struct timespec * abstime )
{
    ssize_t xStatus = 0;
    int iCalculateTimeoutReturn = 0;
    TickType_t xTimeoutTicks = 0;
    QueueListElement_t * pxMessageQueue = ( QueueListElement_t * ) mqdes;
    QueueElement_t xReceiveData = { 0 };

    /* Silence warnings about unused parameters. */
    ( void ) msg_prio;

    /* Lock the mutex that guards access to the queue list. This call will
     * never fail because it blocks forever. */
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

    /* Find the mq referenced by mqdes. */
    if( prvFindQueueInList( NULL, NULL, mqdes ) == pdFALSE )
    {
        /* Queue not found; bad descriptor. */
        errno = EBADF;
        xStatus = -1;
    }

    /* Verify that msg_len is large enough. */
    if( xStatus == 0 )
    {
        if( msg_len < ( size_t ) pxMessageQueue->xAttr.mq_msgsize )
        {
            /* msg_len too small. */
            errno = EMSGSIZE;
            xStatus = -1;
        }
    }

    if( xStatus == 0 )
    {
        /* Convert abstime to a tick timeout. */
        iCalculateTimeoutReturn = prvCalculateTickTimeout( pxMessageQueue->xAttr.mq_flags,
                                                           abstime,
                                                           &xTimeoutTicks );

        if( iCalculateTimeoutReturn != 0 )
        {
            errno = iCalculateTimeoutReturn;
            xStatus = -1;
        }
    }

    /* Release the mutex protecting the queue list. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );

    if( xStatus == 0 )
    {
        /* Receive data from the FreeRTOS queue. */
        if( xQueueReceive( pxMessageQueue->xQueue,
                           &xReceiveData,
                           xTimeoutTicks ) == pdFALSE )
        {
            /* If queue receive fails, set the appropriate errno. */
            if( pxMessageQueue->xAttr.mq_flags & O_NONBLOCK )
            {
                /* Set errno to EAGAIN for nonblocking mq. */
                errno = EAGAIN;
            }
            else
            {
                /* Otherwise, set errno to ETIMEDOUT. */
                errno = ETIMEDOUT;
            }

            xStatus = -1;
        }
    }

    if( xStatus == 0 )
    {
        /* Get the length of data for return value. */
        xStatus = ( ssize_t ) xReceiveData.xDataSize;

        /* Copy received data into given buffer, then free it. */
        ( void ) memcpy( msg_ptr, xReceiveData.pcData, xReceiveData.xDataSize );
        vPortFree( xReceiveData.pcData );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

int mq_timedsend( mqd_t mqdes,
                  const char * msg_ptr,
                  size_t msg_len,
                  unsigned int msg_prio,
                  const struct timespec * abstime )
{
    int iStatus = 0, iCalculateTimeoutReturn = 0;
    TickType_t xTimeoutTicks = 0;
    QueueListElement_t * pxMessageQueue = ( QueueListElement_t * ) mqdes;
    QueueElement_t xSendData = { 0 };

    /* Silence warnings about unused parameters. */
    ( void ) msg_prio;

    /* Lock the mutex that guards access to the queue list. This call will
     * never fail because it blocks forever. */
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

    /* Find the mq referenced by mqdes. */
    if( prvFindQueueInList( NULL, NULL, mqdes ) == pdFALSE )
    {
        /* Queue not found; bad descriptor. */
        errno = EBADF;
        iStatus = -1;
    }

    /* Verify that mq_msgsize is large enough. */
    if( iStatus == 0 )
    {
        if( msg_len > ( size_t ) pxMessageQueue->xAttr.mq_msgsize )
        {
            /* msg_len too large. */
            errno = EMSGSIZE;
            iStatus = -1;
        }
    }

    if( iStatus == 0 )
    {
        /* Convert abstime to a tick timeout. */
        iCalculateTimeoutReturn = prvCalculateTickTimeout( pxMessageQueue->xAttr.mq_flags,
                                                           abstime,
                                                           &xTimeoutTicks );

        if( iCalculateTimeoutReturn != 0 )
        {
            errno = iCalculateTimeoutReturn;
            iStatus = -1;
        }
    }

    /* Release the mutex protecting the queue list. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );

    /* Allocate memory for the message. */
    if( iStatus == 0 )
    {
        xSendData.xDataSize = msg_len;
        xSendData.pcData = pvPortMalloc( msg_len );

        /* Check that memory allocation succeeded. */
        if( xSendData.pcData == NULL )
        {
            /* msg_len too large. */
            errno = EMSGSIZE;
            iStatus = -1;
        }
        else
        {
            /* Copy the data to send. */
            ( void ) memcpy( xSendData.pcData, msg_ptr, msg_len );
        }
    }

    if( iStatus == 0 )
    {
        /* Send data to the FreeRTOS queue. */
        if( xQueueSend( pxMessageQueue->xQueue,
                        &xSendData,
                        xTimeoutTicks ) == pdFALSE )
        {
            /* If queue send fails, set the appropriate errno. */
            if( pxMessageQueue->xAttr.mq_flags & O_NONBLOCK )
            {
                /* Set errno to EAGAIN for nonblocking mq. */
                errno = EAGAIN;
            }
            else
            {
                /* Otherwise, set errno to ETIMEDOUT. */
                errno = ETIMEDOUT;
            }

            /* Free the allocated queue data. */
            vPortFree( xSendData.pcData );

            iStatus = -1;
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int mq_unlink( const char * name )
{
    int iStatus = 0;
    size_t xNameSize = 0;
    BaseType_t xQueueRemoved = pdFALSE;
    QueueListElement_t * pxMessageQueue = NULL;

    /* Initialize the queue list, if needed. */
    prvInitializeQueueList();

    /* Check queue name. */
    if( prvValidateQueueName( name, &xNameSize ) == pdFALSE )
    {
        /* Error with mq name. */
        errno = ENAMETOOLONG;
        iStatus = -1;
    }

    if( iStatus == 0 )
    {
        /* Lock the mutex that guards access to the queue list. This call will
         * never fail because it blocks forever. */
        ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &xQueueListMutex, portMAX_DELAY );

        /* Check if the named queue exists. */
        if( prvFindQueueInList( &pxMessageQueue, name, ( mqd_t ) NULL ) == pdTRUE )
        {
            /* If the queue exists and there are no open descriptors to it,
             * remove it from the list. */
            if( pxMessageQueue->xOpenDescriptors == 0 )
            {
                listREMOVE( &pxMessageQueue->xLink );

                /* Set the flag to delete the queue. Deleting the queue is deferred
                 * until xQueueListMutex is released. */
                xQueueRemoved = pdTRUE;
            }
            else
            {
                /* If the queue has open descriptors, set the pending unlink flag
                 * so that mq_close will free its resources. */
                pxMessageQueue->xPendingUnlink = pdTRUE;
            }
        }
        else
        {
            /* The named message queue doesn't exist. */
            errno = ENOENT;
            iStatus = -1;
        }

        /* Release the mutex protecting the queue list. */
        ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xQueueListMutex );
    }

    /* Delete all resources used by the queue if needed. */
    if( xQueueRemoved == pdTRUE )
    {
        prvDeleteMessageQueue( pxMessageQueue );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/
