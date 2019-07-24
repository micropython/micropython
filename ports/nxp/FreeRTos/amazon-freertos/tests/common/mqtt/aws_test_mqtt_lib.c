/*
 * Amazon FreeRTOS MQTT AFQP V1.1.4
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

/**
 * @file aws_test_mqtt_lib.c
 * @brief Tests for MQTT Core Library.
 */

/* Standard includes. */
#include <string.h>

/* Unity framework includes. */
#include "unity_fixture.h"

/* MQTT Lib includes. */
#include "aws_mqtt_lib.h"
#include "aws_mqtt_lib_test_access_declare.h"
#include "aws_mqtt_agent_config.h"

/* Bufferpool includes. */
#include "aws_bufferpool.h"

/**
 * @brief The callback context registered with the MQTT Core library.
 *
 * The same context is returned in the MQTT event callback. This specific
 * value is used to check that the library indeed passes the same context
 * back.
 */
#define testmqttlibCALLBACK_CONTEXT           ( void * ) ( 0x5a5a5a5a )

/**
 * @brief The send context registered with the MQTT Code library.
 *
 * The same context us returned in the network send callback. This specific
 * value is used to check that the library indeed passes the same context
 * back.
 */
#define testmqttlibSEND_CONTEXT               ( void * ) ( 0x0a0a0a0a )

/**
 * @brief Packet ID of the connect message.
 */
#define testmqttlibCONNECT_PACKET_ID          ( 1 )

/**
 * @brief Number of ticks after which an operation should timeout.
 */
#define testmqttlibOPERATION_TIMEOUT_TICKS    ( 1000 )

/**
 * @brief MQTT Control packet types.
 */
#define mqttCONTROL_CONNACK                   ( ( uint8_t ) 2 << ( uint8_t ) 4 )

/**
 * @brief MQTT Control packet flags.
 */
#define mqttFLAGS_CONNACK                     ( ( uint8_t ) 0 ) /**< Reserved. */
/*-----------------------------------------------------------*/

/**
 * @brief Tracks the number of times the registered callback is invoked for
 * various events during the execution of a test.
 */
typedef struct CallbackCounter
{
    uint32_t ulConnACK;           /**< Number of times the callback is invoked for CONNACK message. */
    uint32_t ulUnexpectedConnACK; /**< Number of times the callback is invoked for unexpected CONNACK messages. */
    uint32_t ulDisconnect;        /**< Number of times the callback is invoked for disconnect message. */
    uint32_t ulUnidentified;      /**< Number of times the callback is invoked for un-handled events. */
} CallbackCounter_t;
/*-----------------------------------------------------------*/

/**
 * @breif MQTT context used by all the tests.
 */
static MQTTContext_t xMQTTContext;

/**
 * @brief Callback counter used by all the tests.
 */
static CallbackCounter_t xCallbackCounter;
/*-----------------------------------------------------------*/

/**
 * @brief The MQTT event callback registered with the MQTT library.
 *
 * In this case, we just update the callback counters for checking later after
 * the test finishes.
 *
 * @param[in] pvCallbackContext The callback context as supplied in Init parameters.
 * @param[in] pxParams The event and related data.
 *
 * @return The return value is ignored in all other cases except publish (i.e. eMQTTPublish
 * event):
 * 1. If eMQTTTrue is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) lies with the user.
 * 2. If eMQTTFalse is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) remains with the library and it is recycled as soon as the callback
 * returns.
 */
static MQTTBool_t prvMQTTEventCallback( void * pvCallbackContext,
                                        const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief The send callback registered with the MQTT library.
 *
 * This one mimics a successful send by returning ulDataLength
 * indicating that all the data was transmitted successfully.
 *
 * @param[in] pvSendContext The send context as supplied in Init parameters.
 * @param[in] pucData The data to transmit.
 * @param[in] ulDataLength The length of the data.
 *
 * @return The number of bytes actually transmitted.
 */
static uint32_t prvSendCallback( void * pvSendContext,
                                 const uint8_t * const pucData,
                                 uint32_t ulDataLength );

/**
 * @brief The send callback registered with the MQTT library to
 * mimic network send failure..
 *
 * This one mimics a failed send by returning 0 indicating that
 * no data could be transmitted.
 *
 * @param[in] pvSendContext The send context as supplied in Init parameters.
 * @param[in] pucData The data to transmit.
 * @param[in] ulDataLength The length of the data.
 *
 * @return The number of bytes actually transmitted.
 */
static uint32_t prvSendFailedCallback( void * pvSendContext,
                                       const uint8_t * const pucData,
                                       uint32_t ulDataLength );

/**
 * @brief Initializes the global callback counter object.
 */
static void prvInitializeCallbackCounter( void );

/**
 * @brief Initializes the global MQTT context by calling MQTT_Init.
 *
 * @return The return value of MQTT_Init.
 */
static MQTTReturnCode_t prvInitializeMQTTContext( void );

/**
 * @brief Sends MQTT connect message by calling MQTT_Connect.
 *
 * @return The return value of MQTT_Connect.
 */
static MQTTReturnCode_t prvSendMQTTConnect( void );

/**
 * @brief Mimics receiving a CONNACK message by passing a valid CONNACK message
 * to MQTT_ParseReceivedData.
 *
 * @return The return value of MQTT_ParseReceivedData.
 */
static MQTTReturnCode_t prvReceiveMQTTConnACK( void );
/*-----------------------------------------------------------*/

static MQTTBool_t prvMQTTEventCallback( void * pvCallbackContext,
                                        const MQTTEventCallbackParams_t * const pxParams )
{
    /* Ensure that the correct callback context was supplied
     * back by the library. */
    TEST_ASSERT_EQUAL( pvCallbackContext, testmqttlibCALLBACK_CONTEXT );

    switch( pxParams->xEventType )
    {
        case eMQTTConnACK:
            xCallbackCounter.ulConnACK += 1;

            /* Ensure that correct identifier was passed. */
            TEST_ASSERT_EQUAL( pxParams->u.xMQTTConnACKData.usPacketIdentifier, testmqttlibCONNECT_PACKET_ID );

            break;

        case eMQTTClientDisconnected:
            xCallbackCounter.ulDisconnect += 1;

            break;

        case eMQTTUnexpectedConnACK:
            xCallbackCounter.ulUnexpectedConnACK += 1;

            break;

        default:
            xCallbackCounter.ulUnidentified += 1;

            break;
    }

    /* This value is ignored for all events other than the
     * publish messages received from the broker. */
    return eMQTTFalse;
}
/*-----------------------------------------------------------*/

static uint32_t prvSendCallback( void * pvSendContext,
                                 const uint8_t * const pucData,
                                 uint32_t ulDataLength )
{
    /* Ensure that the correct context was supplied by the library. */
    TEST_ASSERT_EQUAL( pvSendContext, testmqttlibSEND_CONTEXT );

    /* Mimic that everything was sent successfully. */
    return ulDataLength;
}
/*-----------------------------------------------------------*/

static uint32_t prvSendFailedCallback( void * pvSendContext,
                                       const uint8_t * const pucData,
                                       uint32_t ulDataLength )
{
    /* Ensure that the correct context was supplied by the library. */
    TEST_ASSERT_EQUAL( pvSendContext, testmqttlibSEND_CONTEXT );

    /* Mimic that no data could be sent. */
    return 0;
}
/*-----------------------------------------------------------*/

static void prvInitializeCallbackCounter( void )
{
    xCallbackCounter.ulConnACK = 0;
    xCallbackCounter.ulUnexpectedConnACK = 0;
    xCallbackCounter.ulDisconnect = 0;
    xCallbackCounter.ulUnidentified = 0;
}
/*-----------------------------------------------------------*/

static MQTTReturnCode_t prvInitializeMQTTContext( void )
{
    MQTTInitParams_t xInitParams = {0};
    MQTTReturnCode_t xReturnCode;

    /* Setup init parameters. */
    xInitParams.pxCallback = &( prvMQTTEventCallback );
    xInitParams.pvCallbackContext = testmqttlibCALLBACK_CONTEXT;
    xInitParams.pvSendContext = testmqttlibSEND_CONTEXT;
    xInitParams.pxMQTTSendFxn = &( prvSendCallback );
    xInitParams.pxGetTicksFxn = NULL;
    xInitParams.xBufferPoolInterface.pxGetBufferFxn = BUFFERPOOL_GetFreeBuffer;
    xInitParams.xBufferPoolInterface.pxReturnBufferFxn = BUFFERPOOL_ReturnBuffer;

    /* Initialize MQTT context. */
    xReturnCode = MQTT_Init( &( xMQTTContext ), &( xInitParams ) );

    /* Return the returned code. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

static MQTTReturnCode_t prvSendMQTTConnect( void )
{
    MQTTConnectParams_t xConnectParams = {0};
    MQTTReturnCode_t xReturnCode;

    /* Setup connect parameters. */
    xConnectParams.pucClientId = ( const uint8_t * ) "client_id";
    xConnectParams.usClientIdLength = ( uint16_t ) ( strlen( ( const char * ) xConnectParams.pucClientId ) );
    xConnectParams.pucUserName = ( const uint8_t * ) "Amazon FreeRTOS";
    xConnectParams.usUserNameLength = ( uint16_t ) ( strlen( ( const char * ) xConnectParams.pucUserName ) );
    xConnectParams.usPacketIdentifier = ( uint16_t ) testmqttlibCONNECT_PACKET_ID;
    xConnectParams.usKeepAliveIntervalSeconds = mqttconfigKEEP_ALIVE_INTERVAL_SECONDS;
    xConnectParams.ulKeepAliveActualIntervalTicks = mqttconfigKEEP_ALIVE_ACTUAL_INTERVAL_TICKS;
    xConnectParams.ulPingRequestTimeoutTicks = mqttconfigKEEP_ALIVE_TIMEOUT_TICKS;
    xConnectParams.ulTimeoutTicks = testmqttlibOPERATION_TIMEOUT_TICKS;

    /* Send MQTT Connect. */
    xReturnCode = MQTT_Connect( &( xMQTTContext ), &( xConnectParams ) );

    /* Return the returned code. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

static MQTTReturnCode_t prvReceiveMQTTConnACK( void )
{
    MQTTReturnCode_t xReturnCode;
    static const uint8_t ucConnACKMessage[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        2,                                       /* Fixed header remaining length - always 2 for CONNACK. */
        0,                                       /* Bit 0 is SP - Session Present. */
        0,                                       /* Return code. */
    };

    /* Receive CONNACK. */
    xReturnCode = MQTT_ParseReceivedData( &( xMQTTContext ), ucConnACKMessage, sizeof( ucConnACKMessage ) );

    /* Return the returned code. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( Full_MQTT );
/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( Full_MQTT )
{
    MQTTReturnCode_t xReturnCode;

    /* Each test starts with a fresh context state. */
    xReturnCode = prvInitializeMQTTContext();

    /* Ensure that the context was initialized successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Reset callback counters before each test. */
    prvInitializeCallbackCounter();
}
/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( Full_MQTT )
{
    /* Each test leaves the context in fresh state. */
    Test_prvResetMQTTContext( &( xMQTTContext ) );
}
/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( Full_MQTT )
{
    RUN_TEST_CASE( Full_MQTT, AFQP_prvGetTopicFilterType_HappyCases );
    RUN_TEST_CASE( Full_MQTT, AFQP_prvGetTopicFilterType_ErrorCases );

    RUN_TEST_CASE( Full_MQTT, AFQP_prvDoesTopicMatchTopicFilter_MatchCases );
    RUN_TEST_CASE( Full_MQTT, AFQP_prvDoesTopicMatchTopicFilter_NotMatchCases );

    /* MQTT_Init tests. */
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Init_HappyCase );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Init_NULLParams );

    /* MQTT_Connect tests. */
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_HappyCase );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_BrokerRejectsConnection );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_ConnACKWithoutConnect );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_ReservedReturnCodeFromBroker );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_ShorterConnACK );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_LongerConnACK );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_NULLParams );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_SecondConnectWhileAlreadyConnected );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_SecondConnectWhileWaitingForConnACK );
    RUN_TEST_CASE( Full_MQTT, AFQP_MQTT_Connect_NetworkSendFailed );
}
/*-----------------------------------------------------------*/

/**
 * @brief Valid topic filter test cases for the function prvGetTopicFilterType.
 */
TEST( Full_MQTT, AFQP_prvGetTopicFilterType_HappyCases )
{
    MQTTTopicFilterType_t xTopicFilterType;

    /* Simple topic filter without any separators. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "aws", ( uint16_t ) strlen( "aws" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeSimple, xTopicFilterType );

    /* Simple topic filter with separators. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "aws/iot", ( uint16_t ) strlen( "aws/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeSimple, xTopicFilterType );

    /* '+' wild card topic filters. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "+", ( uint16_t ) strlen( "+" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "a/b/+", ( uint16_t ) strlen( "a/b/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "a/b/+/d", ( uint16_t ) strlen( "a/b/+/d" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "+/a/b", ( uint16_t ) strlen( "+/a/b" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    /* '#' wild card topic filters. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "#", ( uint16_t ) strlen( "#" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "a/b/#", ( uint16_t ) strlen( "a/b/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );

    /* Mixed wild card topic filters. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "a/+/b/#", ( uint16_t ) strlen( "a/+/b/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeWildCard, xTopicFilterType );
}
/*-----------------------------------------------------------*/

/**
 * @brief Invalid topic filter test cases for the function prvGetTopicFilterType.
 */
TEST( Full_MQTT, AFQP_prvGetTopicFilterType_ErrorCases )
{
    MQTTTopicFilterType_t xTopicFilterType;

    /* Multiple '#' characters. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "##", ( uint16_t ) strlen( "##" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* '#' not preceded by '/'. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/iot#", ( uint16_t ) strlen( "/aws/iot#" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* '#' not the last character. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/#/iot", ( uint16_t ) strlen( "/aws/#/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* Multiple '#' at the same topic level. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/##/iot", ( uint16_t ) strlen( "/aws/##/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* Multiple consecutive '+'. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "++", ( uint16_t ) strlen( "++" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* '+' not preceded by '/'. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/iot+", ( uint16_t ) strlen( "/aws/iot+" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* Multiple consecutive '+' in between. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/++/iot", ( uint16_t ) strlen( "/aws/++/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* Invalid mix of '#' and '+'. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/#+/iot", ( uint16_t ) strlen( "/aws/#+/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );

    /* Invalid mix of '#' and '+'. */
    xTopicFilterType = Test_prvGetTopicFilterType( ( const uint8_t * ) "/aws/+#/iot", ( uint16_t ) strlen( "/aws/+#/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTopicFilterTypeInvalid, xTopicFilterType );
}
/*-----------------------------------------------------------*/

/**
 * @brief Tests for prvDoesTopicMatchTopicFilter where the topic matches the given filter..
 */
TEST( Full_MQTT, AFQP_prvDoesTopicMatchTopicFilter_MatchCases )
{
    MQTTBool_t xTopicMatchesTopicFilter;

    /* Simple topic filter with no separator. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ),
                                                                  ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* Simple topic filter with separators. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ),
                                                                  ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* Empty topic level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws//iot",
                                                                  ( uint16_t ) strlen( "aws//iot" ),
                                                                  ( const uint8_t * ) "aws//iot",
                                                                  ( uint16_t ) strlen( "aws//iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' at the end. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ),
                                                                  ( const uint8_t * ) "aws/+",
                                                                  ( uint16_t ) strlen( "aws/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' in between. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "aws/+/shadow",
                                                                  ( uint16_t ) strlen( "aws/+/shadow" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* More than one '+'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "aws/+/+",
                                                                  ( uint16_t ) strlen( "aws/+/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '#' matches anything. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "#",
                                                                  ( uint16_t ) strlen( "#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '#' matches anything starting with separator. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "/aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "/aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "#",
                                                                  ( uint16_t ) strlen( "#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '#' starting with '/'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "/aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "/aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "/#",
                                                                  ( uint16_t ) strlen( "/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '#' matches one level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "aws/iot/#",
                                                                  ( uint16_t ) strlen( "aws/iot/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '#' matches more than one level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow/thing",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow/thing" ),
                                                                  ( const uint8_t * ) "aws/iot/#",
                                                                  ( uint16_t ) strlen( "aws/iot/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* "aws/#" should match "aws" as '#' includes parent level - MQTT protocol spec.*/
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ),
                                                                  ( const uint8_t * ) "aws/#",
                                                                  ( uint16_t ) strlen( "aws/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* "aws/+" should match "aws/" - MQTT protocol spec.*/
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/",
                                                                  ( uint16_t ) strlen( "aws/" ),
                                                                  ( const uint8_t * ) "aws/+",
                                                                  ( uint16_t ) strlen( "aws/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' matches empty topic level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "/aws",
                                                                  ( uint16_t ) strlen( "/aws" ),
                                                                  ( const uint8_t * ) "+/+",
                                                                  ( uint16_t ) strlen( "+/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' matches empty topic level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws//iot",
                                                                  ( uint16_t ) strlen( "aws//iot" ),
                                                                  ( const uint8_t * ) "aws/+/iot",
                                                                  ( uint16_t ) strlen( "aws/+/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' matches empty topic level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws///iot",
                                                                  ( uint16_t ) strlen( "aws///iot" ),
                                                                  ( const uint8_t * ) "aws/+/+/iot",
                                                                  ( uint16_t ) strlen( "aws/+/+/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* Empty level with '+'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws//iot",
                                                                  ( uint16_t ) strlen( "aws//iot" ),
                                                                  ( const uint8_t * ) "aws//+",
                                                                  ( uint16_t ) strlen( "aws//+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* '+' only at one level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "/aws",
                                                                  ( uint16_t ) strlen( "/aws" ),
                                                                  ( const uint8_t * ) "/+",
                                                                  ( uint16_t ) strlen( "/+" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );

    /* Mix of '+' and '#'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow/thing/temp",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow/thing/temp" ),
                                                                  ( const uint8_t * ) "aws/+/shadow/#",
                                                                  ( uint16_t ) strlen( "aws/+/shadow/#" ) );
    TEST_ASSERT_EQUAL( eMQTTTrue, xTopicMatchesTopicFilter );
}
/*-----------------------------------------------------------*/

/**
 * @brief Tests for prvDoesTopicMatchTopicFilter where the topic does not match the given filter.
 */
TEST( Full_MQTT, AFQP_prvDoesTopicMatchTopicFilter_NotMatchCases )
{
    MQTTBool_t xTopicMatchesTopicFilter;

    /* Basic mismatch. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ),
                                                                  ( const uint8_t * ) "iot",
                                                                  ( uint16_t ) strlen( "iot" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* Case mismatch. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "AWS",
                                                                  ( uint16_t ) strlen( "AWS" ),
                                                                  ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* Substrings should not match. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ),
                                                                  ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* Substrings should not match. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ),
                                                                  ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* "aws/+" should not match "aws" - MQTT protocol spec. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws",
                                                                  ( uint16_t ) strlen( "aws" ),
                                                                  ( const uint8_t * ) "aws/+",
                                                                  ( uint16_t ) strlen( "aws/+" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '+' should not match more than one level. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "aws/+",
                                                                  ( uint16_t ) strlen( "aws/+" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '+' with other mismatch. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "aws/+/thing",
                                                                  ( uint16_t ) strlen( "aws/+/thing" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '#' with other mismatch. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "iot/#",
                                                                  ( uint16_t ) strlen( "iot/#" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '+' and '#' with other mismatch. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot/shadow",
                                                                  ( uint16_t ) strlen( "aws/iot/shadow" ),
                                                                  ( const uint8_t * ) "iot/+/#",
                                                                  ( uint16_t ) strlen( "iot/+/#" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '+' with missing '/'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "/aws",
                                                                  ( uint16_t ) strlen( "/aws" ),
                                                                  ( const uint8_t * ) "+",
                                                                  ( uint16_t ) strlen( "+" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );

    /* '#' with missing '/'. */
    xTopicMatchesTopicFilter = Test_prvDoesTopicMatchTopicFilter( ( const uint8_t * ) "aws/iot",
                                                                  ( uint16_t ) strlen( "aws/iot" ),
                                                                  ( const uint8_t * ) "/#",
                                                                  ( uint16_t ) strlen( "/#" ) );
    TEST_ASSERT_EQUAL( eMQTTFalse, xTopicMatchesTopicFilter );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT context initialization happy case.
 */
TEST( Full_MQTT, AFQP_MQTT_Init_HappyCase )
{
    MQTTReturnCode_t xReturnCode;

    /* Initialize the MQTT context. */
    xReturnCode = prvInitializeMQTTContext();

    /* Ensure that the context was initialized successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT context initialization NULL value for required parameter.
 */
TEST( Full_MQTT, AFQP_MQTT_Init_NULLParams )
{
    MQTTInitParams_t xInitParams = {0};

    /* Setup init parameters. */
    xInitParams.pxCallback = &( prvMQTTEventCallback );
    xInitParams.pvCallbackContext = testmqttlibCALLBACK_CONTEXT;
    xInitParams.pvSendContext = testmqttlibSEND_CONTEXT;
    xInitParams.pxMQTTSendFxn = NULL; /* This is a required callback and setting it to NULL will fire assert. */
    xInitParams.pxGetTicksFxn = NULL;
    xInitParams.xBufferPoolInterface.pxGetBufferFxn = BUFFERPOOL_GetFreeBuffer;
    xInitParams.xBufferPoolInterface.pxReturnBufferFxn = BUFFERPOOL_ReturnBuffer;

    if( TEST_PROTECT() )
    {
        /* Initialize the MQTT context. */
        ( void ) MQTT_Init( &( xMQTTContext ), &( xInitParams ) );

        /* Since the above call results in assert being fired,
         * this line must never get executed. */
        TEST_FAIL();
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect happy case.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_HappyCase )
{
    MQTTReturnCode_t xReturnCode;

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = prvReceiveMQTTConnACK();

    /* CONNACK must have been received successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "connected". */
    TEST_ASSERT_EQUAL( eMQTTConnected, xMQTTContext.xConnectionState );

    /* Callback must have been invoked for CONNACK. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulConnACK );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Connection request rejected by the broker.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_BrokerRejectsConnection )
{
    MQTTReturnCode_t xReturnCode;
    static const uint8_t ucConnACKMessage[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        2,                                       /* Fixed header remaining length - always 2 for CONNACK. */
        0,                                       /* Bit 0 is SP - Session Present. */
        1                                        /* Return code - 1 means unacceptable protocol version. */
    };

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = MQTT_ParseReceivedData( &( xMQTTContext ), ucConnACKMessage, sizeof( ucConnACKMessage ) );

    /* MQTT_ParseReceivedData must return success. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "not-connected", since
     * the broker rejected the connection request. */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* Callback must have been invoked for CONNACK. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulConnACK );

    /* Callback must have been invoked for Disconnect because
     * the broker rejected the connection request. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulDisconnect );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Verify CONNACK is not processed when not connected.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_ConnACKWithoutConnect )
{
    MQTTReturnCode_t xReturnCode;

    /* Receive CONNACK without sending connect. */
    xReturnCode = prvReceiveMQTTConnACK();

    /* The returned code must be that the client is not connected. */
    TEST_ASSERT_EQUAL( eMQTTClientNotConnected, xReturnCode );

    /* Connection state must be "not-connected", since
     * the connect was never sent. */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* ConnACK is not processed without a connection, so there is no unexpectedConnACK. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnexpectedConnACK );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Broker sends invalid return code in CONNACK.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_ReservedReturnCodeFromBroker )
{
    MQTTReturnCode_t xReturnCode;
    static const uint8_t ucConnACKMessage[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        2,                                       /* Fixed header remaining length - always 2 for CONNACK. */
        0,                                       /* Bit 0 is SP - Session Present. */
        128                                      /* Return code - Values above 5 are reserved and must not be returned by the broker. So this is a malformed CONNACK. */
    };

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = MQTT_ParseReceivedData( &( xMQTTContext ), ucConnACKMessage, sizeof( ucConnACKMessage ) );

    /* MQTT_ParseReceivedData must return success. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Callback must have been invoked for disconnect since
     * since a malformed packet must result in disconnect. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulDisconnect );

    /* Connection state must be "not-connected". */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - CONNACK with invalid length.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_ShorterConnACK )
{
    MQTTReturnCode_t xReturnCode;
    static const uint8_t ucConnACKMessage[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        1,                                       /* Fixed header remaining length - must always be 2 for CONNACK. So this is a malformed CONNACK. */
        0                                        /* Bit 0 is SP - Session Present. */
    };

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = MQTT_ParseReceivedData( &( xMQTTContext ), ucConnACKMessage, sizeof( ucConnACKMessage ) );

    /* MQTT_ParseReceivedData must return success. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Callback must have been invoked for disconnect since
     * since a malformed packet must result in disconnect. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulDisconnect );

    /* Connection state must be "not-connected". */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - CONNACK with invalid length.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_LongerConnACK )
{
    MQTTReturnCode_t xReturnCode;
    static const uint8_t ucConnACKMessage[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        3,                                       /* Fixed header remaining length - must always be 2 for CONNACK. So this is a malformed CONNACK. */
        0,                                       /* Bit 0 is SP - Session Present. */
        0,                                       /* Return code. */
        1                                        /* Bogus value. */
    };

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = MQTT_ParseReceivedData( &( xMQTTContext ), ucConnACKMessage, sizeof( ucConnACKMessage ) );

    /* MQTT_ParseReceivedData must return success. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Callback must have been invoked for disconnect since
     * since a malformed packet must result in disconnect. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulDisconnect );

    /* Connection state must be "not-connected". */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - NULL value for required parameters.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_NULLParams )
{
    if( TEST_PROTECT() )
    {
        /* Call MQTT_Connect with NULL parameters which will
         * fire assert. */
        ( void ) MQTT_Connect( &( xMQTTContext ), NULL );

        /* Since the above call results in assert being fired,
         * this line must never get executed. */
        TEST_FAIL();
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Send connect message while already connected.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_SecondConnectWhileAlreadyConnected )
{
    MQTTReturnCode_t xReturnCode;

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Receive CONNACK. */
    xReturnCode = prvReceiveMQTTConnACK();

    /* CONNACK must have been received successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "connected". */
    TEST_ASSERT_EQUAL( eMQTTConnected, xMQTTContext.xConnectionState );

    /* Callback must have been invoked for CONNACK. */
    TEST_ASSERT_EQUAL( 1, xCallbackCounter.ulConnACK );

    /* Send second connect message when already connected. */
    xReturnCode = prvSendMQTTConnect();

    /* Ensure that we get the correct error code. */
    TEST_ASSERT_EQUAL( eMQTTClientAlreadyConnected, xReturnCode );

    /* No other callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Send second connect message while waiting for CONNACK.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_SecondConnectWhileWaitingForConnACK )
{
    MQTTReturnCode_t xReturnCode;

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have been sent successfully. */
    TEST_ASSERT_EQUAL( eMQTTSuccess, xReturnCode );

    /* Connection state must be "in-progress". */
    TEST_ASSERT_EQUAL( eMQTTConnectionInProgress, xMQTTContext.xConnectionState );

    /* Send the second MQTT Connect message while
     * we are waiting for CONNACK. */
    xReturnCode = prvSendMQTTConnect();

    /* Ensure that we get the correct error code. */
    TEST_ASSERT_EQUAL( eMQTTClientConnectionInProgress, xReturnCode );

    /* No other callback must be invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/

/**
 * @brief MQTT connect - Network send failure.
 */
TEST( Full_MQTT, AFQP_MQTT_Connect_NetworkSendFailed )
{
    MQTTReturnCode_t xReturnCode;

    /* Change the send callback in the MQTT context. */
    xMQTTContext.pxMQTTSendFxn = &( prvSendFailedCallback );

    /* Send MQTT Connect message. */
    xReturnCode = prvSendMQTTConnect();

    /* Connect must have failed. */
    TEST_ASSERT_EQUAL( eMQTTSendFailed, xReturnCode );

    /* Connection state must be "not-connected". */
    TEST_ASSERT_EQUAL( eMQTTNotConnected, xMQTTContext.xConnectionState );

    /* No callback must have been invoked. */
    TEST_ASSERT_EQUAL( 0, xCallbackCounter.ulUnidentified );
}
/*-----------------------------------------------------------*/
