/*
 * Amazon FreeRTOS POSIX Test V1.1.4
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
 * @file aws_test_posix_mqueue.c
 * @brief Tests for POSIX message queue functions.
 */

/* C standard library includes. */
#include <string.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/mqueue.h"

/* Test framework includes. */
#include "unity.h"
#include "unity_fixture.h"

/**
 * @defgroup Configuration constants for the message queue tests that aren't
 * platform-specific.
 */
/**@{ */
#define posixtestMQ_INVALID_MQD           ( ( mqd_t ) -1 )                         /**< An invalid message queue descriptor. */
#define posixtestMQ_SMALL_MESSAGE         "Hello"                                  /**< A small test message sent over the mq tests. */
#define posixtestMQ_SMALL_MESSAGE_SIZE    ( sizeof( posixtestMQ_SMALL_MESSAGE ) )  /**< Length (including null-terminator) of posixtestMQ_SMALL_MESSAGE. */
#define posixtestMQ_DEFAULT_NAME          "/myqueue"                               /**< Default name of message queues in this test. */
#define posixtestMQ_DEFAULT_MODE          0600                                     /**< Default mode argument for mq_open. */
/**@} */

/* Default queue attributes used in these tests. */
struct mq_attr xDefaultQueueAttr =
{
    .mq_flags   = 0,
    .mq_maxmsg  = posixconfigMQ_MAX_MESSAGES,
    .mq_msgsize = posixtestMQ_SMALL_MESSAGE_SIZE,
    .mq_curmsgs = 0
};

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_MQUEUE );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_MQUEUE )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_MQUEUE )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_MQUEUE )
{
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_unlink_defaults );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_unlink_attr );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_unlink_twice );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_unlink_two_queues );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_flags );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_open_invalid_params );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_unlink_invalid_params );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_getattr );
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_send_receive );
    /*RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_send_receive_invalidParams ); */
    RUN_TEST_CASE( Full_POSIX_MQUEUE, mq_send_receive_nonblock );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_unlink_defaults )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    char pcNameBuffer[ NAME_MAX + 2 ] = { 0 };

    /* Fill name buffer with non-zero value. */
    memset( pcNameBuffer, 'c', NAME_MAX + 2 );

    /* Add the / and null-terminator to create a valid mq name. */
    pcNameBuffer[ 0 ] = '/';
    pcNameBuffer[ NAME_MAX + 1 ] = '\0';

    if( TEST_PROTECT() )
    {
        /* Create queue with system default attributes. */
        xMqId = mq_open( pcNameBuffer, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Close and unlink queue. */
        iStatus = mq_close( xMqId );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        iStatus = mq_unlink( pcNameBuffer );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    /* Clean up resources in case any assert was triggered. */
    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( pcNameBuffer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_unlink_attr )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    struct mq_attr xRetrievedAttr = { 0 };
    /* Custom message queue attributes. */
    struct mq_attr xQueueAttr =
    {
        .mq_flags   = 0,
        .mq_maxmsg  = posixconfigMQ_MAX_MESSAGES / 2,
        .mq_msgsize = posixconfigMQ_MAX_SIZE / 2,
        .mq_curmsgs = 0
    };

    if( TEST_PROTECT() )
    {
        /* Create queue with custom attributes. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, &xQueueAttr );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Retrieve the attributes. */
        iStatus = mq_getattr( xMqId, &xRetrievedAttr );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Verify that xRetrievedAttr matches xQueueAttr. Ignore flags. */
        TEST_ASSERT_EQUAL_INT( xQueueAttr.mq_maxmsg, xRetrievedAttr.mq_maxmsg );
        TEST_ASSERT_EQUAL_INT( xQueueAttr.mq_msgsize, xRetrievedAttr.mq_msgsize );
        TEST_ASSERT_EQUAL_INT( xQueueAttr.mq_curmsgs, xRetrievedAttr.mq_curmsgs );

        /* Close queue. */
        iStatus = mq_close( xMqId );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Unlink queue. */
        iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    /* Clean up resources in case any assert was triggered. */
    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_unlink_twice )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD, xMqId2 = posixtestMQ_INVALID_MQD;

    if( TEST_PROTECT() )
    {
        /* Try opening the same queue twice. This should succeed and not create a
         * memory leak (checked at the end of tests). */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        xMqId2 = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId2 );

        /* Close both mq descriptors and unlink. */
        iStatus = mq_close( xMqId );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        iStatus = mq_close( xMqId2 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    /* Clean up resources in case any assert was triggered. */
    ( void ) mq_close( xMqId );
    ( void ) mq_close( xMqId2 );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_unlink_two_queues )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD, xMqId2 = posixtestMQ_INVALID_MQD;

    /* Create and unlink 2 different queues. */
    if( TEST_PROTECT() )
    {
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        xMqId2 = mq_open( posixtestMQ_DEFAULT_NAME "2", O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId2 );

        /* Check that the queues are different. */
        TEST_ASSERT_NOT_EQUAL( xMqId, xMqId2 );

        iStatus = mq_close( xMqId );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = mq_close( xMqId2 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME "2" );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    /* Clean up resources in case any assert was triggered. */
    ( void ) mq_close( xMqId );
    ( void ) mq_close( xMqId2 );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME "2" );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_flags )
{
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD,
                   xMqId2 = posixtestMQ_INVALID_MQD,
                   xMqId3 = posixtestMQ_INVALID_MQD;

    if( TEST_PROTECT() )
    {
        /* Try to open a queue that does not exist with no O_CREAT flag. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, 0, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );
        TEST_ASSERT_EQUAL_INT( ENOENT, errno );

        /* Create a message queue with O_EXCL. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT | O_EXCL, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Opening a created queue with O_CREAT should succeed as long as O_EXCL isn't set. */
        xMqId2 = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId2 );
        xMqId3 = mq_open( posixtestMQ_DEFAULT_NAME, 0, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId3 );

        /* Close two descriptors. */
        TEST_ASSERT_EQUAL_INT( 0, mq_close( xMqId ) );
        TEST_ASSERT_EQUAL_INT( 0, mq_close( xMqId2 ) );

        /* Attempting to open an existing mq with O_CREAT and O_EXCL should fail. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT | O_EXCL, posixtestMQ_DEFAULT_MODE, NULL );
        TEST_ASSERT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );
        TEST_ASSERT_EQUAL_INT( EEXIST, errno );
    }

    /* Clean up resources used by test. */
    ( void ) mq_close( xMqId );
    ( void ) mq_close( xMqId2 );
    ( void ) mq_close( xMqId3 );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_open_invalid_params )
{
    mqd_t xMqId = NULL;
    struct mq_attr xQueueAttr = { 0 };
    char pcNameBuffer[ NAME_MAX + 3 ] = { 0 };

    /* Try to create a queue with an invalid name. The name is missing a '/'. */
    xMqId = mq_open( "invalidname", O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
    TEST_ASSERT_EQUAL_INT( posixtestMQ_INVALID_MQD, xMqId );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );

    /* Try to create a queue with an invalid mq_maxmsg attribute. */
    xQueueAttr.mq_maxmsg = -1;
    xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, &xQueueAttr );
    TEST_ASSERT_EQUAL_INT( posixtestMQ_INVALID_MQD, xMqId );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );

    /* Try to create a queue with an invalid mq_msgsize attribute. */
    xQueueAttr.mq_maxmsg = posixconfigMQ_MAX_MESSAGES;
    xQueueAttr.mq_msgsize = -1;
    xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, &xQueueAttr );
    TEST_ASSERT_EQUAL_INT( posixtestMQ_INVALID_MQD, xMqId );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );

    /* Fill name buffer with non-zero value. */
    memset( pcNameBuffer, 'c', NAME_MAX + 3 );

    /* Add the / and null-terminator to create a valid mq name. */
    pcNameBuffer[ 0 ] = '/';
    pcNameBuffer[ NAME_MAX + 2 ] = 0;

    /* Try to create a queue with a name longer than NAME_MAX. */
    xMqId = mq_open( pcNameBuffer, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
    TEST_ASSERT_EQUAL_INT( posixtestMQ_INVALID_MQD, xMqId );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_unlink_invalid_params )
{
    int iStatus = 0;
    mqd_t xMqId = NULL;
    char pcNameBuffer[ NAME_MAX + 3 ] = { 0 };

    /* Fill name buffer with non-zero value. */
    memset( pcNameBuffer, 'c', NAME_MAX + 3 );

    /* Add the / and null-terminator to create a valid mq name. */
    pcNameBuffer[ 0 ] = '/';
    pcNameBuffer[ NAME_MAX + 2 ] = 0;

    /* Try to unlink a message queue whose name is longer than NAME_MAX. */
    iStatus = mq_unlink( pcNameBuffer );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( ENAMETOOLONG, errno );

    /* Try to unlink a queue that doesn't exist. */
    iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( ENOENT, errno );

    /* Create a message queue with no open descriptors. */
    xMqId = mq_open( posixtestMQ_DEFAULT_NAME, O_CREAT, posixtestMQ_DEFAULT_MODE, NULL );
    TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );
    iStatus = mq_close( xMqId );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Try to unlink the same queue twice. The second unlink should fail. */
    iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    iStatus = mq_unlink( posixtestMQ_DEFAULT_NAME );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( ENOENT, errno );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_getattr )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    char pcReturnParam[ posixtestMQ_SMALL_MESSAGE_SIZE ] = { 0 };
    struct mq_attr xQueueAttr = { 0 };

    if( TEST_PROTECT() )
    {
        /* Create queue with custom attributes. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME,
                         O_CREAT | O_RDWR,
                         posixtestMQ_DEFAULT_MODE,
                         &xDefaultQueueAttr );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Send a message. */
        iStatus = mq_send( xMqId,
                           posixtestMQ_SMALL_MESSAGE,
                           posixtestMQ_SMALL_MESSAGE_SIZE,
                           0 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Check that curmsgs is now 1. */
        iStatus = mq_getattr( xMqId, &xQueueAttr );
        TEST_ASSERT_EQUAL_INT( iStatus, 0 );
        TEST_ASSERT_EQUAL_INT( xQueueAttr.mq_curmsgs, 1 );

        /* Receive (remove) the message from the queue. */
        iStatus = ( int ) mq_receive( xMqId,
                                      pcReturnParam,
                                      posixtestMQ_SMALL_MESSAGE_SIZE,
                                      NULL );
        TEST_ASSERT_EQUAL_INT( posixtestMQ_SMALL_MESSAGE_SIZE, iStatus );

        /* Check that curmsgs is now 0. */
        iStatus = mq_getattr( xMqId, &xQueueAttr );
        TEST_ASSERT_EQUAL_INT( iStatus, 0 );
        TEST_ASSERT_EQUAL_INT( xQueueAttr.mq_curmsgs, 0 );
    }

    /* Close and unlink the message queue. */
    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );

    /* Attempting to get attributes of an unlinked mq should fail. */
    iStatus = mq_getattr( xMqId, &xQueueAttr );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( EBADF, errno );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_send_receive )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    char pcReceiveBuffer[ posixtestMQ_SMALL_MESSAGE_SIZE ] = { 0 };

    if( TEST_PROTECT() )
    {
        /* Create queue with default parameters. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME,
                         O_CREAT | O_RDWR,
                         posixtestMQ_DEFAULT_MODE,
                         &xDefaultQueueAttr );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Send a message to the queue. */
        iStatus = mq_send( xMqId, posixtestMQ_SMALL_MESSAGE, posixtestMQ_SMALL_MESSAGE_SIZE, 0 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Receive the sent data and check if for correctness. */
        iStatus = ( int ) mq_receive( xMqId, pcReceiveBuffer, posixtestMQ_SMALL_MESSAGE_SIZE, NULL );
        TEST_ASSERT_EQUAL_INT( posixtestMQ_SMALL_MESSAGE_SIZE, iStatus );
        TEST_ASSERT_EQUAL_STRING( posixtestMQ_SMALL_MESSAGE, pcReceiveBuffer );
    }

    /* Close and unlink the message queue. */
    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_send_receive_invalid_params )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    char pcReceiveBuffer[ posixtestMQ_SMALL_MESSAGE_SIZE ] = { 0 };

    /* Try to receive with a bad queue file descriptor. */
    iStatus = ( int ) mq_receive( xMqId,
                                  pcReceiveBuffer,
                                  posixtestMQ_SMALL_MESSAGE_SIZE,
                                  NULL );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( EBADF, errno );

    /* Try to send with a bad queue file descriptor. */
    iStatus = mq_send( xMqId,
                       posixtestMQ_SMALL_MESSAGE,
                       posixtestMQ_SMALL_MESSAGE_SIZE,
                       0 );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( EBADF, errno );

    if( TEST_PROTECT() )
    {
        /* Create queue with default parameters. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME,
                         O_CREAT | O_RDWR,
                         posixtestMQ_DEFAULT_MODE,
                         &xDefaultQueueAttr );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Attempt to send a message that's larger than mq_msgsize. */
        iStatus = mq_send( xMqId, posixtestMQ_SMALL_MESSAGE, posixtestMQ_SMALL_MESSAGE_SIZE + 1, 0 );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EMSGSIZE, errno );

        /* Send a message to the queue. */
        iStatus = mq_send( xMqId, posixtestMQ_SMALL_MESSAGE, posixtestMQ_SMALL_MESSAGE_SIZE, 0 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Attempt to receive a message with msg_len too small. */
        iStatus = mq_receive( xMqId, pcReceiveBuffer, posixtestMQ_SMALL_MESSAGE_SIZE - 1, NULL );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EMSGSIZE, errno );
    }

    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_MQUEUE, mq_send_receive_nonblock )
{
    int iStatus = 0;
    volatile mqd_t xMqId = posixtestMQ_INVALID_MQD;
    char pcReceiveBuffer[ posixtestMQ_SMALL_MESSAGE_SIZE ] = { 0 };

    if( TEST_PROTECT() )
    {
        /* Create nonblocking queue with default parameters. */
        xMqId = mq_open( posixtestMQ_DEFAULT_NAME,
                         O_CREAT | O_RDWR | O_NONBLOCK,
                         posixtestMQ_DEFAULT_MODE,
                         &xDefaultQueueAttr );
        TEST_ASSERT_NOT_EQUAL( posixtestMQ_INVALID_MQD, xMqId );

        /* Try to receive a message from an empty queue. Queue is nonblocking,
         * so this function should return immediately. */
        iStatus = ( int ) mq_receive( xMqId,
                                      pcReceiveBuffer,
                                      posixtestMQ_SMALL_MESSAGE_SIZE,
                                      NULL );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );

        /* Clear errno. */
        errno = 0;

        /* Fill the mq. */
        do
        {
            iStatus = mq_send( xMqId,
                               posixtestMQ_SMALL_MESSAGE,
                               posixtestMQ_SMALL_MESSAGE_SIZE,
                               0 );
        } while( iStatus != -1 );

        /* The final mq_send in the do-while should have failed with EAGAIN
         * because queue was full. */
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );
    }

    /* Clean up resources used by test. */
    ( void ) mq_close( xMqId );
    ( void ) mq_unlink( posixtestMQ_DEFAULT_NAME );
}

/*-----------------------------------------------------------*/
