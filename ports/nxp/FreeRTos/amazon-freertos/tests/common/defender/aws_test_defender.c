/*
 * Amazon FreeRTOS Device Defender Agent V1.1.4
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

#include "FreeRTOS.h"

#include "projdefs.h"
#include "task.h"

#include "aws_cbor.h"
#include "aws_cbor_print.h"
#include "aws_cbor_types.h"
#include "aws_clientcredential.h"
#include "aws_defender_internals.h"
#include "aws_mqtt_agent.h"
#include "unity_fixture.h"

static bool xReportAccepted;
static bool xReportRejected;

TEST_GROUP( Full_DEFENDER );

TEST_SETUP( Full_DEFENDER )
{
    /* Delay to allow the MQTT logs to flush */
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
    xReportAccepted = false;
    xReportRejected = false;
}

TEST_TEAR_DOWN( Full_DEFENDER )
{
    /* Delay to allow the MQTT logs to flush */
    ( void ) DEFENDER_Stop();
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
}

TEST_GROUP_RUNNER( Full_DEFENDER )
{
    /* These test should not require any network connectivity */
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_err_if_already_started );
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_success );
    RUN_TEST_CASE( Full_DEFENDER, Stop_should_return_success_when_started );
    RUN_TEST_CASE( Full_DEFENDER, Stop_should_return_err_when_not_started );

    /* These tests check the connectivity and responses from the service */
    RUN_TEST_CASE( Full_DEFENDER, report_to_echo_server );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_cbor_example_report );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_json_example_report );

    /* This tests that the agent successfully reports to the service */
    RUN_TEST_CASE( Full_DEFENDER, agent_happy_states );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_report_from_agent );
}

/*----------------------------------------------------------------------------*/

TEST( Full_DEFENDER, Start_should_return_err_if_already_started )
{
    ( void ) DEFENDER_Start();
    DefenderErr_t eErr = DEFENDER_Start();
    TEST_ASSERT_EQUAL( eDefenderErrAlreadyStarted, eErr );
}

TEST( Full_DEFENDER, Start_should_return_success )
{
    DefenderErr_t eErr = DEFENDER_Start();

    TEST_ASSERT_EQUAL( eDefenderErrSuccess, eErr );
}

TEST( Full_DEFENDER, Stop_should_return_success_when_started )
{
    ( void ) DEFENDER_Start();
    DefenderErr_t eErr = DEFENDER_Stop();
    TEST_ASSERT_EQUAL( eDefenderErrSuccess, eErr );
}

TEST( Full_DEFENDER, Stop_should_return_err_when_not_started )
{
    DefenderErr_t eErr = DEFENDER_Stop();

    TEST_ASSERT_EQUAL( eDefenderErrNotStarted, eErr );
}

/*----------------------------------------------------------------------------*/

static bool xEchoTriggered;

static CBORHandle_t prvCreateDummyReport( void );
static MQTTAgentHandle_t prvMqttAgentNew( void );
static void prvMqttAgentConnectToEcho( MQTTAgentHandle_t );
static void prvSubscribeToEcho( MQTTAgentHandle_t );
static MQTTBool_t prvEchoCallback( void * pvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData );
static void prvReportPublishToEcho( MQTTAgentHandle_t,
                                    CBORHandle_t );

TEST( Full_DEFENDER, report_to_echo_server )
{
    CBORHandle_t xDefenderReport = prvCreateDummyReport();
    MQTTAgentHandle_t xDefenderMqttAgent = prvMqttAgentNew();

    prvMqttAgentConnectToEcho( xDefenderMqttAgent );
    prvSubscribeToEcho( xDefenderMqttAgent );
    prvReportPublishToEcho( xDefenderMqttAgent, xDefenderReport );

    /* Wait for 1 second while we  */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );
    TEST_ASSERT_TRUE_MESSAGE( xEchoTriggered,
                              "Expected the echo to trigger the callback" )

    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    ( void ) MQTT_AGENT_Disconnect( xDefenderMqttAgent, xTimeout );
    ( void ) MQTT_AGENT_Delete( xDefenderMqttAgent );
    CBOR_Delete( &xDefenderReport );
}

static CBORHandle_t prvCreateDummyReport( void )
{
    /* Create example report */
    CBORHandle_t xDefenderReport = CBOR_New( 0 );

    TEST_ASSERT_NOT_NULL_MESSAGE( xDefenderReport,
                                  "Failed to create new pcReport" );
    CBORHandle_t xHeader = CBOR_New( 0 );

    if( NULL == xHeader )
    {
        CBOR_Delete( &xDefenderReport );
        TEST_FAIL_MESSAGE( "Failed to create new xHeader" );
    }
    else
    {
        /* Using configRAND32 instead of ipconfigRAND32 */
        /* Including xHeader with ipconfigRAND32 causes redefinition errors */
        /* Cryptographically secure rand is not important here */
        uint32_t ulReportId = configRAND32() && ( 0x3FFFFFFF );
        CBOR_AppendKeyWithInt( xHeader, "report_id", ulReportId );
        CBOR_AppendKeyWithString( xHeader, "version", "1.0" );
        CBOR_AppendKeyWithMap( xDefenderReport, "header", xHeader );
    }

    CBOR_Delete( &xHeader );

    CBORHandle_t xMetrics = CBOR_New( 0 );
    {
        CBORHandle_t xTcpPorts = CBOR_New( 0 );
        {
            CBOR_AppendKeyWithInt( xTcpPorts, "total", 0 );
            CBOR_AppendKeyWithMap( xMetrics, "listening_tcp_ports", xTcpPorts );
        }
        CBOR_Delete( &xTcpPorts );

        CBORHandle_t xUdpPorts = CBOR_New( 0 );
        {
            CBOR_AppendKeyWithInt( xUdpPorts, "total", 0 );
            CBOR_AppendKeyWithMap( xMetrics, "listening_udp_ports", xUdpPorts );
        }
        CBOR_Delete( &xUdpPorts );

        CBORHandle_t xTcpConnections = CBOR_New( 0 );
        {
            CBORHandle_t xEstConnections = CBOR_New( 0 );
            {
                CBOR_AppendKeyWithInt( xEstConnections, "total", 2 );
                CBOR_AppendKeyWithMap( xTcpConnections,
                                       "established_connections",
                                       xEstConnections );
            }
            CBOR_AppendKeyWithMap( xMetrics, "tcp_connections",
                                   xTcpConnections );
        }
        CBOR_Delete( &xTcpConnections );

        CBOR_AppendKeyWithMap( xDefenderReport, "metrics", xMetrics );
    }
    CBOR_Delete( &xMetrics );
    cborError_t eErr = CBOR_CheckError( xDefenderReport );

    if( eErr )
    {
        CBOR_Delete( &xDefenderReport );
        TEST_ASSERT_EQUAL_MESSAGE( eCborErrNoError, eErr,
                                   "Failed to create pcReport" );
    }

    return xDefenderReport;
}

static MQTTAgentHandle_t prvMqttAgentNew( void )
{
    MQTTAgentHandle_t xNewMqttAgent = 0;
    MQTTAgentReturnCode_t eCreateResult = MQTT_AGENT_Create( &xNewMqttAgent );

    TEST_ASSERT_EQUAL_MESSAGE( eMQTTAgentSuccess, eCreateResult,
                               "Failed to create agent" );

    return xNewMqttAgent;
}

static void prvMqttAgentConnectToEcho( MQTTAgentHandle_t pxMQTTAgent )
{
    MQTTAgentConnectParams_t xDefenderConParams =
    {
        .pcURL              = clientcredentialMQTT_BROKER_ENDPOINT,
        .xFlags             = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress    = pdFALSE,
        .usPort             = clientcredentialMQTT_BROKER_PORT,
        .pucClientId        = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength   = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME ),
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eConnectResult =
        MQTT_AGENT_Connect( pxMQTTAgent, &xDefenderConParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eConnectResult,
                         "Failed to connect agent to echo broker" );
}

static void prvSubscribeToEcho( MQTTAgentHandle_t pxMQTTAgent )
{
    uint8_t * pucTopic = ( uint8_t * ) "freertos/tests/echo";
    MQTTAgentSubscribeParams_t xSubscriptionParams =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = prvEchoCallback,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eSubscriptionResult =
        MQTT_AGENT_Subscribe( pxMQTTAgent, &xSubscriptionParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eSubscriptionResult,
                         "Failed to subscribe to pucTopic" );
}

static MQTTBool_t prvEchoCallback( void * pvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData )
{
    xEchoTriggered = true;

    return eMQTTFalse;
}

static void prvReportPublishToEcho( MQTTAgentHandle_t pxMQTTAgent,
                                    CBORHandle_t xReport )
{
    uint8_t * pucTopic = ( uint8_t * ) "freertos/tests/echo";

    MQTTAgentPublishParams_t xPubRecParams =
    {
        .pucTopic      = pucTopic,
        .usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = xReport->pxBufferStart,
        .ulDataLength  = xReport->pxMapEnd - xReport->pxBufferStart,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 1000 );
    MQTTAgentReturnCode_t ePublishResult =
        MQTT_AGENT_Publish( pxMQTTAgent, &xPubRecParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == ePublishResult,
                         "Failed to publish to pucTopic" );
}

/*----------------------------------------------------------------------------*/

static char const * const pcDefenderTestEndpoint =
    clientcredentialMQTT_BROKER_ENDPOINT;
/* "data.gamma.us-east-1.iot.amazonaws.com"; */

static MQTTBool_t testRejectCallback( void * pvPublishCallbackContext,
                                      MQTTPublishData_t const * pxPublishData );

static MQTTBool_t testAcceptCallback( void * pvPublishCallbackContext,
                                      MQTTPublishData_t const * pxPublishData );

static void MqttAgentConnectToDevDef( MQTTAgentHandle_t );
static void SubscribeToAcceptCbor( MQTTAgentHandle_t );
static void SubscribeToRejectCbor( MQTTAgentHandle_t );
static void PublishCborToDevDef( MQTTAgentHandle_t,
                                 CBORHandle_t );

TEST( Full_DEFENDER, endpoint_accepts_cbor_example_report )
{
    CBORHandle_t xDefenderReport = prvCreateDummyReport();
    MQTTAgentHandle_t xDefenderMqttAgent = prvMqttAgentNew();

    MqttAgentConnectToDevDef( xDefenderMqttAgent );
    SubscribeToAcceptCbor( xDefenderMqttAgent );
    SubscribeToRejectCbor( xDefenderMqttAgent );
    PublishCborToDevDef( xDefenderMqttAgent, xDefenderReport );

    /* Wait for 1 second for the response to come back from the service */
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    vTaskDelay( pdMS_TO_TICKS( xTimeout ) );

    /* Clean up resources before checking test assertions */
    ( void ) MQTT_AGENT_Disconnect( xDefenderMqttAgent, xTimeout );
    ( void ) MQTT_AGENT_Delete( xDefenderMqttAgent );
    CBOR_Delete( &xDefenderReport );

    TEST_ASSERT_FALSE_MESSAGE( xReportRejected, "Metrics pcReport was rejected." )
    TEST_ASSERT_TRUE_MESSAGE( xReportAccepted,
                              "Expected the pcReport to be accepted." )
}

static void MqttAgentConnectToDevDef( MQTTAgentHandle_t pxMQTTAgent )
{
    MQTTAgentConnectParams_t xDefenderConParams =
    {
        .pcURL              = pcDefenderTestEndpoint,
        .xFlags             = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress    = pdFALSE,
        .usPort             = clientcredentialMQTT_BROKER_PORT,
        .pucClientId        = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength   = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME ),
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eConnectResult =
        MQTT_AGENT_Connect( pxMQTTAgent, &xDefenderConParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eConnectResult,
                         "Failed to connect agent to broker" );
}

static void SubscribeToAcceptCbor( MQTTAgentHandle_t pxMQTTAgent )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/accepted";
    MQTTAgentSubscribeParams_t xSubscriptionParams =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = testAcceptCallback,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eSubscriptionResult =
        MQTT_AGENT_Subscribe( pxMQTTAgent, &xSubscriptionParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eSubscriptionResult,
                         "Failed to subscribe to pucTopic" );
}

static MQTTBool_t testAcceptCallback( void * pvPublishCallbackContext,
                                      MQTTPublishData_t const * pxPublishData )
{
    xReportAccepted = true;

    return eMQTTFalse;
}

static void SubscribeToRejectCbor( MQTTAgentHandle_t pxMQTTAgent )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/rejected";
    MQTTAgentSubscribeParams_t xSubscriptionParams =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = testRejectCallback,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eSubscriptionResult =
        MQTT_AGENT_Subscribe( pxMQTTAgent, &xSubscriptionParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eSubscriptionResult,
                         "Failed to subscribe to pucTopic" );
}

static MQTTBool_t testRejectCallback( void * pvPublishCallbackContext,
                                      MQTTPublishData_t const * pxPublishData )
{
    xReportRejected = true;

    return eMQTTFalse;
}

static void PublishCborToDevDef( MQTTAgentHandle_t pxMQTTAgent,
                                 CBORHandle_t xReport )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/"
                         clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor";
    uint8_t const * pucBuffer = xReport->pxBufferStart;
    int lBufLen = xReport->pxMapEnd - xReport->pxBufferStart + 1;

    MQTTAgentPublishParams_t xPubRecParams =
    {
        .pucTopic      = pucTopic,
        .usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = pucBuffer,
        .ulDataLength  = lBufLen,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );

    MQTTAgentReturnCode_t ePublishResult =
        MQTT_AGENT_Publish( pxMQTTAgent, &xPubRecParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == ePublishResult,
                         "Failed to publish to pucTopic" );
}

/*----------------------------------------------------------------------------*/

static void SubscribeToAcceptJson( MQTTAgentHandle_t );
static void SubscribeToRejectJson( MQTTAgentHandle_t );
static void PublishJsonToDevDef( MQTTAgentHandle_t,
                                 char const * );

TEST( Full_DEFENDER, endpoint_accepts_json_example_report )
{
    CBORHandle_t xDefenderReport = prvCreateDummyReport();
    char * pcJSONReport = CBOR_AsString( xDefenderReport );

    CBOR_Delete( &xDefenderReport );

    MQTTAgentHandle_t xDefenderMqttAgent = prvMqttAgentNew();
    MqttAgentConnectToDevDef( xDefenderMqttAgent );
    SubscribeToAcceptJson( xDefenderMqttAgent );
    SubscribeToRejectJson( xDefenderMqttAgent );
    PublishJsonToDevDef( xDefenderMqttAgent, pcJSONReport );
    vPortFree( pcJSONReport );

    /* Wait for the service to respond */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );

    ( void ) MQTT_AGENT_Disconnect( xDefenderMqttAgent, xTimeout );
    ( void ) MQTT_AGENT_Delete( xDefenderMqttAgent );

    TEST_ASSERT_FALSE_MESSAGE( xReportRejected,
                               "JSON Metrics pcReport was rejected." )
    TEST_ASSERT_TRUE_MESSAGE( xReportAccepted,
                              "Expected the JSON pcReport to be accepted." )
}

static void SubscribeToAcceptJson( MQTTAgentHandle_t pxMQTTAgent )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/json/accepted";
    MQTTAgentSubscribeParams_t xSubscriptionParams =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = testAcceptCallback,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eSubscriptionResult =
        MQTT_AGENT_Subscribe( pxMQTTAgent, &xSubscriptionParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eSubscriptionResult,
                         "Failed to subscribe to pucTopic" );
}

static void SubscribeToRejectJson( MQTTAgentHandle_t pxMQTTAgent )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/json/rejected";
    MQTTAgentSubscribeParams_t xSubscriptionParams =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = testRejectCallback,
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t eSubscriptionResult =
        MQTT_AGENT_Subscribe( pxMQTTAgent, &xSubscriptionParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == eSubscriptionResult,
                         "Failed to subscribe to pucTopic" );
}

static void PublishJsonToDevDef( MQTTAgentHandle_t pxMQTTAgent,
                                 char const * pcReport )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/"
                         clientcredentialIOT_THING_NAME
                         "/defender/metrics/json";

    MQTTAgentPublishParams_t xPubRecParams =
    {
        .pucTopic      = pucTopic,
        .usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = pcReport,
        .ulDataLength  = ( uint16_t ) strlen( pcReport ),
    };
    TickType_t const xTimeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t ePublishResult =
        MQTT_AGENT_Publish( pxMQTTAgent, &xPubRecParams, xTimeout );

    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == ePublishResult,
                         "Failed to publish JSON pcReport to pucTopic" );
}

/*----------------------------------------------------------------------------*/
#define DEFENDER_AssertStateAndWait( state )                     \
    do {                                                         \
        TEST_ASSERT_EQUAL_STRING( StateAsString( state ),        \
                                  StateAsString( StateGet() ) ); \
        while( state == StateGet() ) {                           \
            ;                                                    \
        }                                                        \
    } while( 0 )

#define DEFENDER_AssertState( state )                 \
    TEST_ASSERT_EQUAL_STRING( StateAsString( state ), \
                              StateAsString( StateGet() ) );

TEST( Full_DEFENDER, agent_happy_states )
{
    ( void ) DEFENDER_MetricsInitFunc( NULL, 0 );

    int lReportPeriod = 5;
    ( void ) DEFENDER_ReportPeriodSet( lReportPeriod );

    DEFENDER_Start();

    while( eDefenderStateInit == StateGet() )
    {
    }

    /* The following asserts the flow of the device defender agent's state
     * machine, assuming no errors occur */
    DEFENDER_AssertStateAndWait( eDefenderStateStarted );
    DEFENDER_AssertStateAndWait( eDefenderStateNewMqttSuccess );
    DEFENDER_AssertStateAndWait( eDefenderStateConnectMqttSuccess );
    DEFENDER_AssertStateAndWait( eDefenderStateSubscribeMqttSuccess );
    DEFENDER_AssertStateAndWait( eDefenderStateSubmitReportSuccess );
    DEFENDER_AssertStateAndWait( eDefenderStateDisconnected );
    DEFENDER_AssertStateAndWait( eDefenderStateSleep );
    DEFENDER_AssertState( eDefenderStateStarted );

    DEFENDER_Stop();
}

TEST( Full_DEFENDER, endpoint_accepts_report_from_agent )
{
    int32_t const lMaxStrLen = 128;
    char cFailStr[ 128 ] = { 0 };

    DefenderMetric_t xMetricsList[] =
    {
        xDefenderTCPConnections,
    };

    ( void ) DEFENDER_MetricsInit( xMetricsList );

    int32_t lReportPeriodSec = 20;
    ( void ) DEFENDER_ReportPeriodSet( lReportPeriodSec );

    DEFENDER_Start();

    while( eDefenderStateSleep != StateGet() )
    {
    }

    DEFENDER_Stop();

    DefenderReportStatus_t eReportStatus = DEFENDER_ReportStatusGet();

    ( void ) snprintf( cFailStr, lMaxStrLen, "Metrics pcReport ID: %d",
                       ( int ) GetLastReportId() );
    TEST_ASSERT_EQUAL_STRING_MESSAGE(
        DEFENDER_ReportStatusAsString( eDefenderRepSuccess ),
        DEFENDER_ReportStatusAsString( eReportStatus ), cFailStr );
}
