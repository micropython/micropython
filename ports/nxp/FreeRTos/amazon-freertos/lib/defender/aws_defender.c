/*
 * Amazon FreeRTOS Device Defender Agent V1.0.2
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

#include "aws_cbor.h"
#include "aws_cbor_alloc.h"
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"

#include "FreeRTOS.h"
#include "aws_clientcredential.h"
#include "aws_mqtt_agent.h"
#include "task.h"

typedef enum
{
    eDefenderFalse = 0,
    eDefenderTrue = 1,
} DEFENDERBool_t;

/* Marks that the agent has been asked to stop by the application. */
static DEFENDERBool_t xDefenderKill;
/* Endpoint used for connecting to the AWS IoT Device Defender service. */
static char const * const pcDefenderMQTTEndpoint =
    clientcredentialMQTT_BROKER_ENDPOINT;
/* Status of last report sent. */
static DefenderReportStatus_t eDefenderReportStatus;
/* Set of agent states. */
static DefenderState_t ( * DEFENDER_States[ eDefenderStateCount ] )( void ) = { 0 };
/* The current state. */
static DefenderState_t eDefenderState;
/* Period between reports. */
static uint32_t ulDelayPeriodSec = 300;
/* Handle for the MQTT agent. */
static MQTTAgentHandle_t xDefenderMQTTAgent;
/* Handle for the agent task. */
static TaskHandle_t xDefenderTaskHandle = NULL;
/* Timeout period for MQTT connections. */
static TickType_t xMQTTTimeoutPeriodTicks = pdMS_TO_TICKS( 10U * 1000U );

/**
 * @brief      Publishes metrics report to service
 *
 * @param[in]  xReport  The metrics report
 *
 * @return     Returns true if error occurred, false (0) on success
 */
static DEFENDERBool_t prvPublishCborToDevDef( CBORHandle_t xReport );

/**
 * @brief      Subscribes to the report accept topic
 *
 * The DD service sends a messsage on the accept topic when a report is received
 * and accepted.
 *
 * @return     Returns true if error occurred, false (0) on success
 */
static DEFENDERBool_t prvSubscribeToAcceptCbor( void );

/**
 * @brief      Subscribes to the report reject topic
 *
 * The DD service sends a messsage on the reject topic when a report is received
 * and rejected.
 *
 * @return     Returns true if error occurred, false (0) on success
 */
static DEFENDERBool_t prvSubscribeToRejectCbor( void );

/**
 * @brief      Device Defender Agent Task
 *
 * @param      params  Required for a FreeRTOS task, but is ignored by the
 *     agent.
 */
static void prvAgentLoop( void * params );

/**
 * @brief      Report success when message recieved on the accept topic
 *
 * @param      pxPvPublishCallbackContext  Not used
 * @param      pxPublishData               Not used
 *
 * @return     Always returns false
 */
static MQTTBool_t xAcceptCallback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData );

/**
 * @brief      Report failure when message recieved on the reject topic
 *
 * @param      pxPvPublishCallbackContext  Not used
 * @param      pxPublishData               Not used
 *
 * @return     Always returns false
 */
static MQTTBool_t prvRejectCallback( void * pxPvPublishCallbackContext,
                                     MQTTPublishData_t const * pxPublishData );

static DefenderState_t prvStateInit( void );
static DefenderState_t prvStateNewMQTT( void );
static DefenderState_t prvStateConnectMqtt( void );
static DefenderState_t prvStateDisconnectMqtt( void );
static DefenderState_t prvStateSubscribe( void );
static DefenderState_t prvStateCreateReport( void );
static DefenderState_t prvStateDeleteMqtt( void );
static DefenderState_t prvStateSleep( void );

DefenderState_t StateGet( void )
{
    return eDefenderState;
}

char * StateAsString( DefenderState_t eState )
{
    switch( eState )
    {
        case eDefenderStateInit:
            return "eDefenderStateInit";

        case eDefenderStateStarted:
            return "eDefenderStateStarted";

        case eDefenderStateNewMqttFailed:
            return "eDefenderStateNewMqttFailed";

        case eDefenderStateNewMqttSuccess:
            return "eDefenderStateNewMqttSuccess";

        case eDefenderStateConnectMqttFailed:
            return "eDefenderStateConnectMqttFailed";

        case eDefenderStateConnectMqttSuccess:
            return "eDefenderStateConnectMqttSuccess";

        case eDefenderStateSubscribeMqttFailed:
            return "eDefenderStateSubscribeMqttFailed";

        case eDefenderStateSubscribeMqttSuccess:
            return "eDefenderStateSubscribeMqttSuccess";

        case eDefenderStateSubmitReportFailed:
            return "eDefenderStateSubmitReportFailed";

        case eDefenderStateSubmitReportSuccess:
            return "eDefenderStateSubmitReportSuccess";

        case eDefenderStateDisconnectFailed:
            return "eDefenderStateDisconnectFailed";

        case eDefenderStateDisconnected:
            return "eDefenderStateDisconnected";

        case eDefenderStateDeleteFailed:
            return "eDefenderStateDeleteFailed";

        case eDefenderStateSleep:
            return "eDefenderStateSleep";

        case eDefenderStateCount:
            return "eDefenderStateCount";
    }

    return "Invalid State Value";
}

static void DEFENDER_StateFunctionSet( void )
{
    /* clang-format off */
    DEFENDER_States[ eDefenderStateInit ] = prvStateInit;

    DEFENDER_States[ eDefenderStateStarted ] = prvStateNewMQTT;
    DEFENDER_States[ eDefenderStateNewMqttFailed ] = prvStateSleep;
    DEFENDER_States[ eDefenderStateNewMqttSuccess ] = prvStateConnectMqtt;
    DEFENDER_States[ eDefenderStateConnectMqttFailed ] = prvStateDeleteMqtt;
    DEFENDER_States[ eDefenderStateConnectMqttSuccess ] = prvStateSubscribe;
    DEFENDER_States[ eDefenderStateSubscribeMqttFailed ] = prvStateDisconnectMqtt;
    DEFENDER_States[ eDefenderStateSubscribeMqttSuccess ] = prvStateCreateReport;
    DEFENDER_States[ eDefenderStateSubmitReportFailed ] = prvStateDisconnectMqtt;
    DEFENDER_States[ eDefenderStateSubmitReportSuccess ] = prvStateDisconnectMqtt;
    DEFENDER_States[ eDefenderStateDisconnectFailed ] = prvStateDisconnectMqtt;
    DEFENDER_States[ eDefenderStateDisconnected ] = prvStateDeleteMqtt;
    DEFENDER_States[ eDefenderStateDeleteFailed ] = prvStateDeleteMqtt;
    DEFENDER_States[ eDefenderStateSleep ] = prvStateSleep;
    /* clang-format on */
}

static DefenderState_t prvStateInit( void )
{
    return eDefenderStateStarted;
}

static DefenderState_t prvStateNewMQTT( void )
{
    MQTTAgentReturnCode_t xCreateResult =
        MQTT_AGENT_Create( &xDefenderMQTTAgent );

    if( eMQTTAgentSuccess != xCreateResult )
    {
        return eDefenderStateNewMqttFailed;
    }

    return eDefenderStateNewMqttSuccess;
}

static DefenderState_t prvStateConnectMqtt( void )
{
    /*Assign MQTT connection parameters*/
    MQTTAgentConnectParams_t xDefenderConParams =
    {
        .pcURL              = NULL,
        .xFlags             = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress    = pdFALSE,
        .usPort             = clientcredentialMQTT_BROKER_PORT,
        .pucClientId        = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength   = 0,
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    MQTTAgentReturnCode_t xConnectResult = 0;

    /* Initialize non-static field values. */
    xDefenderConParams.pcURL = pcDefenderMQTTEndpoint;
    xDefenderConParams.usClientIdLength =
        ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );

    /*Connect to the endpoint*/
    xConnectResult = MQTT_AGENT_Connect( xDefenderMQTTAgent,
                                         &xDefenderConParams,
                                         xMQTTTimeoutPeriodTicks );

    if( eMQTTAgentSuccess != xConnectResult )
    {
        return eDefenderStateConnectMqttFailed;
    }

    return eDefenderStateConnectMqttSuccess;
}

static DefenderState_t prvStateSubscribe( void )
{
    DEFENDERBool_t xError = eDefenderFalse;

    xError |= prvSubscribeToAcceptCbor();
    xError |= prvSubscribeToRejectCbor();

    if( xError )
    {
        return eDefenderStateSubscribeMqttFailed;
    }

    return eDefenderStateSubscribeMqttSuccess;
}

static DEFENDERBool_t prvSubscribeToAcceptCbor( void )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/"
                         clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/accepted";
    MQTTAgentSubscribeParams_t xSubParams =
    {
        .pucTopic                 = NULL,
        .usTopicLength            = 0,
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = xAcceptCallback,
    };
    MQTTAgentReturnCode_t xSubResult = 0;

    /* Initialize non-static field values. */
    xSubParams.pucTopic = pucTopic;
    xSubParams.usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic );

    xSubResult = MQTT_AGENT_Subscribe( xDefenderMQTTAgent,
                                       &xSubParams,
                                       xMQTTTimeoutPeriodTicks );

    DEFENDERBool_t xError = eMQTTAgentSuccess != xSubResult;

    return xError;
}

static MQTTBool_t xAcceptCallback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData )
{
    ( void ) pxPvPublishCallbackContext;
    ( void ) pxPublishData;

    eDefenderReportStatus = eDefenderRepSuccess;

    return eMQTTFalse;
}

static DEFENDERBool_t prvSubscribeToRejectCbor( void )
{
    uint8_t * pucTopic = ( uint8_t * ) "$aws/things/"
                         clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/rejected";
    MQTTAgentSubscribeParams_t xSubParams =
    {
        .pucTopic                 = NULL,
        .usTopicLength            = 0,
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = prvRejectCallback,
    };
    MQTTAgentReturnCode_t xSubResult = 0;

    /* Initialize non-static field values. */
    xSubParams.pucTopic = pucTopic;
    xSubParams.usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic );

    xSubResult = MQTT_AGENT_Subscribe( xDefenderMQTTAgent,
                                       &xSubParams,
                                       xMQTTTimeoutPeriodTicks );

    DEFENDERBool_t xError = eMQTTAgentSuccess != xSubResult;

    return xError;
}

static MQTTBool_t prvRejectCallback( void * pxPvPublishCallbackContext,
                                     MQTTPublishData_t const * pxPublishData )
{
    ( void ) pxPvPublishCallbackContext;
    ( void ) pxPublishData;

    eDefenderReportStatus = eDefenderRepRejected;

    return eMQTTFalse;
}


static DefenderState_t prvStateCreateReport( void )
{
    CBORHandle_t xReport;

    xReport = CreateReport();

    if( NULL == xReport )
    {
        return eDefenderStateSubmitReportFailed;
    }

    DEFENDERBool_t xError = prvPublishCborToDevDef( xReport );
    CBOR_Delete( &xReport );

    /* Wait for ack from service */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );

    if( true == xError )
    {
        return eDefenderStateSubmitReportFailed;
    }

    return eDefenderStateSubmitReportSuccess;
}

static DEFENDERBool_t prvPublishCborToDevDef( CBORHandle_t xReport )
{
    MQTTAgentPublishParams_t xPubRecParams =
    {
        .pucTopic      = NULL,
        .usTopicLength = 0,
        .xQoS          = eMQTTQoS0,
        .pvData        = NULL,
        .ulDataLength  = 0,
    };
    uint8_t * pucTopic = ( uint8_t * )
                         "$aws/things/"
                         clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor";
    uint8_t const * pucBuffer = CBOR_GetRawBuffer( xReport );
    int32_t lBufLen = CBOR_GetBufferSize( xReport );
    MQTTAgentReturnCode_t xPublishResult = 0;

    /* Initialize non-static field values. */
    xPubRecParams.pucTopic = pucTopic;
    xPubRecParams.usTopicLength = ( uint16_t ) strlen( ( char * ) pucTopic );
    xPubRecParams.pvData = pucBuffer;
    xPubRecParams.ulDataLength = lBufLen;

    xPublishResult = MQTT_AGENT_Publish( xDefenderMQTTAgent,
                                         &xPubRecParams,
                                         xMQTTTimeoutPeriodTicks );

    if( eMQTTAgentSuccess != xPublishResult )
    {
        eDefenderReportStatus = eDefenderRepNotSent;

        return true;
    }

    eDefenderReportStatus = eDefenderRepNoAck;

    return eDefenderFalse;
}

static DefenderState_t prvStateDisconnectMqtt( void )
{
    if( eMQTTAgentSuccess
        != MQTT_AGENT_Disconnect(
            xDefenderMQTTAgent, xMQTTTimeoutPeriodTicks ) )
    {
        return eDefenderStateDisconnectFailed;
    }

    return eDefenderStateDisconnected;
}

static DefenderState_t prvStateDeleteMqtt( void )
{
    if( eMQTTAgentSuccess != MQTT_AGENT_Delete( xDefenderMQTTAgent ) )
    {
        return eDefenderStateDeleteFailed;
    }

    return eDefenderStateSleep;
}

static DefenderState_t prvStateSleep( void )
{
    static TickType_t xWakeTick;
    TickType_t const xCurrentTick = xTaskGetTickCount();
    TickType_t const xActivePeriod = ( xCurrentTick - xWakeTick );
    uint32_t const ulPeriodMS = ( ulDelayPeriodSec * 1000 );
    TickType_t const xPeriodTicks = pdMS_TO_TICKS( ulPeriodMS );
    uint32_t ulSleepPeriod = ( xPeriodTicks - xActivePeriod );

    if( ulSleepPeriod > ulDelayPeriodSec )
    {
        ulSleepPeriod = 1;
    }

    vTaskDelay( ulSleepPeriod );

    xWakeTick = xTaskGetTickCount();

    return eDefenderStateStarted;
}

static DefenderState_t DEFENDER_StateFunction( DefenderState_t xCurrentState )
{
    if( xCurrentState == eDefenderStateInit )
    {
        DEFENDER_StateFunctionSet();
    }

    DefenderState_t xNextState = DEFENDER_States[ xCurrentState ]();
    return xNextState;
}

DefenderErr_t DEFENDER_ReportPeriodSet( uint32_t ulPeriodSec )
{
    ulDelayPeriodSec = ulPeriodSec;
    return eDefenderErrSuccess;
}

DefenderErr_t DEFENDER_ConnectionTimeoutSet( uint32_t ulTimeoutMs )
{
    xMQTTTimeoutPeriodTicks = pdMS_TO_TICKS( ulTimeoutMs );
    return eDefenderErrSuccess;
}

DefenderReportStatus_t DEFENDER_ReportStatusGet( void )
{
    DefenderReportStatus_t xReportStatus;

    do
    {
        xReportStatus = eDefenderReportStatus;
    } while( xReportStatus != eDefenderReportStatus );

    return xReportStatus;
}

DefenderErr_t DEFENDER_Start( void )
{
    if( NULL != xDefenderTaskHandle )
    {
        return eDefenderErrAlreadyStarted;
    }

    xDefenderKill = eDefenderFalse;

    /* Returns pdTrue (1) on success. */
    BaseType_t xSuccess =
        xTaskCreate( prvAgentLoop, "DD_Agent", configMINIMAL_STACK_SIZE,
                     NULL, tskIDLE_PRIORITY, &xDefenderTaskHandle );

    if( 1 != xSuccess )
    {
        return eDefenderErrFailedToCreateTask;
    }

    return eDefenderErrSuccess;
}

DefenderErr_t DEFENDER_Stop( void )
{
    if( NULL == xDefenderTaskHandle )
    {
        return eDefenderErrNotStarted;
    }

    xDefenderKill = true;

    return eDefenderErrSuccess;
}

static void prvAgentLoop( void * params )
{
    /* Void pointer is a required parameter for a FreeRTOS task */
    /* Silence warning about unused parameters */
    ( void ) params;

    for( ; ; )
    {
        /* Check if a kill was requested before the report was started */
        if( xDefenderKill )
        {
            break;
            /* TODO: Appropriately clean up resources on kill request */
        }

        eDefenderState = DEFENDER_StateFunction( eDefenderState );

        int32_t lStatePeriodMS = 5;
        vTaskDelay( pdMS_TO_TICKS( lStatePeriodMS ) );
    }

    TaskHandle_t xTaskHandle = xDefenderTaskHandle;
    xDefenderTaskHandle = NULL;
    vTaskDelete( xTaskHandle );
}

char const * DEFENDER_ErrAsString( DefenderErr_t eErrNum )
{
    /* *INDENT-OFF* */
    switch( eErrNum )
    {
        case eDefenderErrSuccess:
            return "eDefenderErrSuccess";
        case eDefenderErrTooManyMetrics:
            return "eDefenderErrTooManyMetrics";
        case eDefenderErrPeriodTooShort:
            return "eDefenderErrPeriodTooShort";
        case eDefenderErrFailedToCreateTask:
            return "eDefenderErrFailedToCreateTask";
        case eDefenderErrAlreadyStarted:
            return "eDefenderErrAlreadyStarted";
        case eDefenderErrNotStarted:
            return "eDefenderErrNotStarted";
        case eDefenderErrOther:
            return "eDefenderErrOther";
    }
    /* *INDENT-ON* */
    return "Invalid value";
}

char const * DEFENDER_ReportStatusAsString( DefenderReportStatus_t eStatusNum )
{
    /* *INDENT-OFF* */
    switch( eStatusNum )
    {
        case eDefenderRepSuccess:
            return "eDefenderRepSuccess";
        case eDefenderRepInit:
            return "eDefenderRepInit";
        case eDefenderRepRejected:
            return "eDefenderRepRejected";
        case eDefenderRepNoAck:
            return "eDefenderRepNoAck";
        case eDefenderRepNotSent:
            return "eDefenderRepNotSent";
    }
    /* *INDENT-ON* */
    return "Invalid value";
}
