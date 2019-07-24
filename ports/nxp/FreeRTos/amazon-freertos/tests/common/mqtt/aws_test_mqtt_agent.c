/*
 * Amazon FreeRTOS MQTT AFQP V1.1.4
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
 * @file aws_test_mqtt_agent.c
 * @brief Tests for MQTT Agent.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Amazon FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "aws_mqtt_agent.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "aws_clientcredential.h"

/* Unity framework includes. */
#include "unity_fixture.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* MQTT agent connection timeout. */
#define mqttagenttestTIMEOUT       pdMS_TO_TICKS( 10000UL )

/* Topic name for ping-pong testing. */
#define mqttagenttestTOPIC_NAME    ( ( const uint8_t * ) "freertos/tests/echo" )

#define mqttagenttestMESSAGE       "Hello from the test."
#define mqttagenttestFAILUREPRINTF( x )    vLoggingPrintf x

/* The parameters below are definable so the test can run on most target. */
/* Size in bytes of the messages published.  */
#ifndef  mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE
    #define mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE    ( 256 )
#endif

/* Number of Receive Tasks that are created. */
#ifndef  mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS
    #define mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS    ( 2 )
#endif

/* Number of Transmit Tasks that are created. */
#ifndef  mqttagenttestMULTI_TASK_TEST_NUM_TX_TASKS
    #define mqttagenttestMULTI_TASK_TEST_NUM_TX_TASKS    ( 2 )
#endif

/* Total number of tasks. */
#define mqttagenttestMULTI_TASK_TEST_NUM_TASKS    ( mqttagenttestMULTI_TASK_TEST_NUM_TX_TASKS + mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS )



/* TCP Echo Client tasks multitask example parameters. */
#define  mqttagenttestMULTI_TASK_TEST_TASKS_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#define  mqttagenttestMULTI_TASK_TEST_TASKS_PRIORITY      ( tskIDLE_PRIORITY )


/* Timeout for sync between Transmit and Receive Tasks. */
#ifndef  mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT
    #define  mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT    ( 40000 )
#endif

/* This represents the time to wait to complete the multitask test.
 * It will vary from one platform to another. */
#ifndef mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT
    #define mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT    ( 400000 )
#endif


/* Event mask used for the sync group. It represents all Tasks.*/
#define mqttagenttestMULTI_TASK_TEST_EVENT_MASK                ( ( 1 << mqttagenttestMULTI_TASK_TEST_NUM_TASKS ) - 1 )
/* Event mask used for the sync group. It represents all Transmit Tasks. */
#define mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK             ( ( ( 1 << mqttagenttestMULTI_TASK_TEST_NUM_TASKS ) - 1 ) - ( ( 1 << mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS ) - 1 ) )

#define  mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS       ( pdMS_TO_TICKS( mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT ) )
#define mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT_TICKS    ( pdMS_TO_TICKS( mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT ) )

/* Receive task priority is changed on bit 0, while Transmit is changed on bit 1.
 * Loop needs to be at least 4 to cover every cases. */
#define mqttagenttestMULTI_TASK_TEST_MAX_LOOP                  ( 4 )

#define mqttagenttestMULTI_TASK_TEST_LOW_PRIORITY              tskIDLE_PRIORITY
#define mqttagenttestMULTI_TASK_TEST_HIGH_PRIORITY             ( configMAX_PRIORITIES - 1 )

/* Receive task toggle priority on bit 0 while Transmit does on bit 1. This allows covering all possible scenario of low/high priority. */
#define mqttagenttestMULTI_TASK_TEST_RX_LOWP_SWITCH_MASK       ( 0 )
#define mqttagenttestMULTI_TASK_TEST_TX_LOWP_SWITCH_MASK       ( 1 )

/* Every Receive task will create one client. */
#define mqttagenttestMULTI_TASK_TEST_CLIENT_ID                 "ClientId%d"
#define mqttagenttestMULTI_TASK_TEST_MAX_CLIENT_ID_SIZE        ( 14 )

/* Number of publishing made by the Transmit task every loop. */
#define mqttagenttestMULTI_TASK_TEST_PUBLISH_PER_LOOP          ( 10 )

/* Topic name for ping-pong testing. */
#define mqttagenttestMULTI_TASK_TEST_TOPIC_NAME                ( ( const uint8_t * ) "freertos/tests/multiTask/%d" )
#define mqttagenttestMULTI_TASK_TEST_MAX_TOPIC_NAME_SIZE       ( 30 )


/* Default connection parameters. */
static const MQTTAgentConnectParams_t xDefaultConnectParameters =
{
    clientcredentialMQTT_BROKER_ENDPOINT,
    mqttagentREQUIRE_TLS,
    pdFALSE,
    clientcredentialMQTT_BROKER_PORT,
    ( const uint8_t * ) clientcredentialIOT_THING_NAME,
    0,
    pdTRUE,
    NULL,
    NULL,
    NULL,
    0
};

/* Parameters used in Muti task call back function. Data is checked and status is passed down. */
typedef struct
{
    SemaphoreHandle_t xSemaphore;
    BaseType_t xStatus;
} MQTTtestAgentCbParam_t;


/* Task parameters for multitask test. */
typedef struct
{
    uint16_t usTaskTag;
    BaseType_t xStatus;
    TaskHandle_t xTaskHandle;
    uint8_t * pucTransmittedData;
    SemaphoreHandle_t xSemaphore;
    BaseType_t xTaskCreated;
} MQTTtestAgentTaskParams_t;


/* Parameters for the Receive tasks. They are bundles into one structure to improve readability. */
typedef struct
{
    MQTTAgentHandle_t xMQTTHandle;
    EventGroupHandle_t xSyncEventRx;
    uint8_t cTopic[ mqttagenttestMULTI_TASK_TEST_MAX_TOPIC_NAME_SIZE ];
    uint16_t usTopicLength;
    uint8_t cClientID[ mqttagenttestMULTI_TASK_TEST_MAX_CLIENT_ID_SIZE ];
    uint16_t usClientIDLength;
} MQTTtestAgentMultiTestRxParam_t;
/* Bundle of global static used by Receive tasks. Note that Transmit task will also use the mqtt handle and the sync semaphore. */
static MQTTtestAgentMultiTestRxParam_t MQTTtestAgentMultiTestRxParam[ mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS ];

static void prvMultiTaskTest_Rx_Task( void * pvParameters );
static void prvMultiTaskTest_Tx_Task( void * pvParameters );

/* The event group used to wait for the multitaks test completion.*/
static EventGroupHandle_t xSyncEventGroup = NULL;

/**
 * @brief Callback for MQTT subscription.
 */
static MQTTBool_t prvMultiTaskTestMQTTCallback( void * pvUserData,
                                                const MQTTPublishData_t * const pxPublishParameters )
{
    MQTTtestAgentCbParam_t * pxMQTTtestAgentCbParam;
    BaseType_t xDataIndex;

    pxMQTTtestAgentCbParam = ( MQTTtestAgentCbParam_t * ) pvUserData;

    pxMQTTtestAgentCbParam->xStatus = pdPASS;

    if( pxPublishParameters->ulDataLength != mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE )
    {
        pxMQTTtestAgentCbParam->xStatus = pdFAIL;
    }
    else
    {
        for( xDataIndex = 0; xDataIndex < mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE; xDataIndex++ )
        {
            if( ( ( uint8_t * ) ( pxPublishParameters->pvData ) )[ xDataIndex ] != xDataIndex )
            {
                pxMQTTtestAgentCbParam->xStatus = pdFAIL;
                break;
            }
        }
    }

    /* Give the semaphore to signal message receipt. */
    xSemaphoreGive( pxMQTTtestAgentCbParam->xSemaphore );

    return eMQTTFalse;
}

/**
 * @brief Callback for MQTT subscription.
 */
static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters )
{
    /* Give the semaphore to signal message receipt. */
    xSemaphoreGive( ( SemaphoreHandle_t ) pvUserData );

    return eMQTTFalse;
}

/*-----------------------------------------------------------*/


/**
 * @brief Test helper routine for MQTT connect, subcribe, publish, and
 * tear-down.
 */
static BaseType_t prvCreateClientAndConnectToBroker( BaseType_t xUseAlpn )
{
    MQTTAgentReturnCode_t xReturned = eMQTTAgentFailure;
    StaticSemaphore_t xSemaphore = { 0 };
    MQTTAgentHandle_t xMQTTHandle = NULL;
    MQTTAgentSubscribeParams_t xSubscribeParams;
    MQTTAgentPublishParams_t xPublishParameters;
    BaseType_t xMQTTAgentCreated = pdFALSE;
    MQTTAgentConnectParams_t xConnectParameters;

    memcpy( &xConnectParameters, &xDefaultConnectParameters, sizeof( MQTTAgentConnectParams_t ) );

    /* Initialize the semaphore as unavailable. */
    TEST_ASSERT_NOT_NULL( xSemaphoreCreateCountingStatic( 1, 0, &xSemaphore ) );

    /* Fill in the MQTTAgentConnectParams_t member that is not const. */
    xConnectParameters.usClientIdLength = ( uint16_t ) strlen(
        ( char * ) xConnectParameters.pucClientId );

    /* Switch ports if requested. */
    if( pdTRUE == xUseAlpn )
    {
        xConnectParameters.xFlags |= mqttagentUSE_AWS_IOT_ALPN_443;
    }

    if( TEST_PROTECT() )
    {
        /* The MQTT client object must be created before it can be used. */
        xReturned = MQTT_AGENT_Create( &xMQTTHandle );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );
        xMQTTAgentCreated = pdTRUE;

        /* Connect to the broker. */
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        mqttagenttestTIMEOUT );
        TEST_ASSERT_EQUAL_INT_MESSAGE( xReturned, eMQTTAgentSuccess, "Failed to connect to the MQTT broker with MQTT_AGENT_Connect()." );

        /* Setup subscribe parameters to subscribe to echo topic. */
        xSubscribeParams.pucTopic = mqttagenttestTOPIC_NAME;
        xSubscribeParams.pvPublishCallbackContext = &xSemaphore;
        xSubscribeParams.pxPublishCallback = prvMQTTCallback;
        xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) mqttagenttestTOPIC_NAME );
        xSubscribeParams.xQoS = eMQTTQoS1;

        /* Subscribe to the topic. */
        xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
                                          &xSubscribeParams,
                                          mqttagenttestTIMEOUT );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );

        /* Setup the publish parameters. */
        memset( &( xPublishParameters ), 0x00, sizeof( xPublishParameters ) );
        xPublishParameters.pucTopic = mqttagenttestTOPIC_NAME;
        xPublishParameters.pvData = mqttagenttestMESSAGE;
        xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) mqttagenttestTOPIC_NAME );
        xPublishParameters.ulDataLength = ( uint32_t ) strlen( mqttagenttestMESSAGE );
        xPublishParameters.xQoS = eMQTTQoS1;

        /* Publish the message. */
        xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                        &( xPublishParameters ),
                                        mqttagenttestTIMEOUT );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );

        /* Take the semaphore to ensure the message is Received. */
        if( pdFALSE == xSemaphoreTake( ( QueueHandle_t ) &( xSemaphore ), mqttagenttestTIMEOUT ) )
        {
            TEST_FAIL();
        }

        /* Disconnect the client. */
        xReturned = MQTT_AGENT_Disconnect( xMQTTHandle, mqttagenttestTIMEOUT );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );
    }
    else
    {
        TEST_FAIL();
    }

    /*Don't forget to reset the flag, since connect parameters are global, all test afterwards would use ALPN. */
    xConnectParameters.xFlags &= ~mqttagentUSE_AWS_IOT_ALPN_443;

    if( xMQTTAgentCreated == pdTRUE )
    {
        /* Delete the MQTT client. */
        xReturned = MQTT_AGENT_Delete( xMQTTHandle );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );
    }

    return eMQTTAgentSuccess == xReturned;
}
/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( Full_MQTT_Agent );
TEST_GROUP( Full_MQTT_Agent_ALPN );
TEST_GROUP( Full_MQTT_Agent_Stress_Tests );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( Full_MQTT_Agent )
{
}
TEST_SETUP( Full_MQTT_Agent_Stress_Tests )
{
}
TEST_SETUP( Full_MQTT_Agent_ALPN )
{
}
/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( Full_MQTT_Agent )
{
}
TEST_TEAR_DOWN( Full_MQTT_Agent_Stress_Tests )
{
}
TEST_TEAR_DOWN( Full_MQTT_Agent_ALPN )
{
}
/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( Full_MQTT_Agent )
{
    RUN_TEST_CASE( Full_MQTT_Agent, AFQP_MQTT_Agent_SubscribePublishDefaultPort );
    RUN_TEST_CASE( Full_MQTT_Agent, AFQP_MQTT_Agent_InvalidCredentials );
}
TEST_GROUP_RUNNER( Full_MQTT_Agent_Stress_Tests )
{
    RUN_TEST_CASE( Full_MQTT_Agent_Stress_Tests, MQTT_Agent_MultiTaskTest );
}
TEST_GROUP_RUNNER( Full_MQTT_Agent_ALPN )
{
    RUN_TEST_CASE( Full_MQTT_Agent_ALPN, MQTT_Agent_SubscribePublishAlpn );
}
/*-----------------------------------------------------------*/

/* Test for ping-ponging a message using the default AWS IoT port for MQTT. */
TEST( Full_MQTT_Agent, AFQP_MQTT_Agent_SubscribePublishDefaultPort )
{
    prvCreateClientAndConnectToBroker( pdFALSE );
}

/* Test for ping-ponging a message using the default AWS IoT port for MQTT. */
TEST( Full_MQTT_Agent, AFQP_MQTT_Agent_InvalidCredentials )
{
    MQTTAgentReturnCode_t xReturned;
    MQTTAgentHandle_t xMQTTHandle = NULL;
    BaseType_t xClientCreated, xClientConnected;
    MQTTAgentConnectParams_t xConnectParameters;

    memcpy( &xConnectParameters, &xDefaultConnectParameters, sizeof( MQTTAgentConnectParams_t ) );

    /* Initialize the semaphore. */
    if( TEST_PROTECT() )
    {
        xClientCreated = pdFALSE;
        /* Arbitrary invalid credentials. */
        xConnectParameters.pcCertificate = "abcd";
        xConnectParameters.ulCertificateSize = ( uint16_t ) strlen( xConnectParameters.pcCertificate );
        /* Fill in the MQTTAgentConnectParams_t member that is not const. */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen(
            ( char * ) xConnectParameters.pucClientId );


        /* The MQTT client object must be created before it can be used. */
        xReturned = MQTT_AGENT_Create( &xMQTTHandle );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );
        xClientCreated = pdTRUE;

        /* Connect to the broker. */
        xClientConnected = pdTRUE;
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        mqttagenttestTIMEOUT );
        TEST_ASSERT_NOT_EQUAL( xReturned, eMQTTAgentSuccess );
        xClientConnected = pdFALSE;
    }

    /* Connect to the broker. */
    if( xClientConnected == pdTRUE )
    {
        /* If enter here, test has already failed. */
        xReturned = MQTT_AGENT_Disconnect( xMQTTHandle,
                                           mqttagenttestTIMEOUT );

        if( xReturned != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Could not disconnect client.\r\n", __FUNCTION__ ) );
        }
    }

    if( xClientCreated == pdTRUE )
    {
        /* Delete the MQTT client. */
        xReturned = MQTT_AGENT_Delete( xMQTTHandle );
        TEST_ASSERT_EQUAL_INT( xReturned, eMQTTAgentSuccess );
    }
}
/*-----------------------------------------------------------*/

/* Test for ping-ponging a message using AWS IoT MQTT broker support for port 443. */
TEST( Full_MQTT_Agent_ALPN, MQTT_Agent_SubscribePublishAlpn )
{
    prvCreateClientAndConnectToBroker( pdTRUE );
}
/*-----------------------------------------------------------*/

/**
 * @brief multitask test for MQTT
 *
 * This test create x number of Receive tasks that will create x different clients and
 * subscribe to x different topic.
 * The Receive tasks will take care of create, connecting, subscribing, disconnecting and deleting clients.
 * Also, y number of Transmit task are going to be created, each tasks are going to publish n number of MQTT messages
 * to each one of the x tasks.
 * Once each y Transmit tasks have submitted n messages to each of the x Receive task, it starts all over again. What varies from cycle to
 * cycle are the priorities. Receive and Transmit tasks will swich priorities in such a way as to cover all possible combination of
 * high and low prioritie.
 *
 * At the end of the test, Tasks do not self delete. They get suspended. The main test function will first wait for every
 * task to complete through Amazon FreeRTOS synchronization mechanism then it will delete all the created tasks.
 *
 * For simplicity sake, memory is not allocated in the Receive or Transmit tasks. The allocation is centralized in one place, the main task.
 */
TEST( Full_MQTT_Agent_Stress_Tests, MQTT_Agent_MultiTaskTest )
{
    uint16_t usIndex;
    MQTTtestAgentTaskParams_t xTaskParams[ mqttagenttestMULTI_TASK_TEST_NUM_TASKS ];
    uint32_t usEventMask;
    BaseType_t xResult;
    uint8_t * pucTransmittedData;

    xSyncEventGroup = NULL;
    pucTransmittedData = NULL;

    /* Initialize to unallocated. */
    for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
    {
        xTaskParams[ usIndex ].xSemaphore = NULL;
        MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx = NULL;
    }

    if( TEST_PROTECT() )
    {
        /* Create the event group used by the Transmit and Receive tasks to synchronize prior
         * to commencing a cycle using a new socket. */
        xSyncEventGroup = xEventGroupCreate();
        TEST_ASSERT_NOT_NULL_MESSAGE( xSyncEventGroup, "Couldn't create Sync event." );

        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
        {
            MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx = xEventGroupCreate();
            TEST_ASSERT_NOT_NULL_MESSAGE( MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx, "Couldn't create Sync event." );
            /* Flag memory as allocated for later Free. */
        }

        pucTransmittedData = pvPortMalloc( mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE * sizeof( uint8_t ) );
        TEST_ASSERT_NOT_NULL_MESSAGE( pucTransmittedData, "Couldn't allocate data." );

        /* Data sent by Transmit tasks. And a counter to check integrity of the data when Received by Receive task. */
        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE; usIndex++ )
        {
            pucTransmittedData[ usIndex ] = ( uint8_t ) ( usIndex & 0xFF );
        }

        /* Create Receive tasks. */
        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
        {
            xTaskParams[ usIndex ].usTaskTag = usIndex;
            xTaskParams[ usIndex ].xStatus = pdFAIL;
            xTaskParams[ usIndex ].pucTransmittedData = pucTransmittedData;

            /* Each Receive task will Receive mqttagenttestMULTI_TASK_TEST_PUBLISH_PER_LOOP * mqttagenttestMULTI_TASK_TEST_NUM_TX_TASKS messages.
             * It is possible that all of them may be Received before callback can trigger, so make semaphore count enough. */
            xTaskParams[ usIndex ].xSemaphore = xSemaphoreCreateCounting( mqttagenttestMULTI_TASK_TEST_PUBLISH_PER_LOOP * mqttagenttestMULTI_TASK_TEST_NUM_TX_TASKS, 0 );
            TEST_ASSERT_NOT_NULL( xTaskParams[ usIndex ].xSemaphore );

            /* Create topic for each Receive task. */
            MQTTtestAgentMultiTestRxParam[ usIndex ].usTopicLength =
                snprintf( ( char * ) &MQTTtestAgentMultiTestRxParam[ usIndex ].cTopic,
                          mqttagenttestMULTI_TASK_TEST_MAX_TOPIC_NAME_SIZE, /* Limit size, will vary depending of the number of tasks. */
                          ( char * ) mqttagenttestMULTI_TASK_TEST_TOPIC_NAME,
                          usIndex );
            /* Create client ID for each Receive task. */
            MQTTtestAgentMultiTestRxParam[ usIndex ].usClientIDLength =
                snprintf( ( char * ) &MQTTtestAgentMultiTestRxParam[ usIndex ].cClientID,
                          mqttagenttestMULTI_TASK_TEST_MAX_CLIENT_ID_SIZE, /* Limit size, will vary depending of the number of tasks. */
                          ( char * ) mqttagenttestMULTI_TASK_TEST_CLIENT_ID,
                          usIndex );

            /* Create Receive Task. When created, flag it for later deletion. */
            xTaskParams[ usIndex ].xTaskCreated = pdFALSE;
            xResult = xTaskCreate( prvMultiTaskTest_Rx_Task, /* The function that implements the task. */
                                   "ClientTask",             /* Just a text name for the task to aid debugging. */
                                   mqttagenttestMULTI_TASK_TEST_TASKS_STACK_SIZE,
                                   &( xTaskParams[ usIndex ] ),
                                   mqttagenttestMULTI_TASK_TEST_TASKS_PRIORITY,
                                   &( xTaskParams[ usIndex ].xTaskHandle ) );
            TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );
            /* Flag Task as created for later delete. */
            xTaskParams[ usIndex ].xTaskCreated = pdTRUE;
        }

        /* Create Transmit tasks. */
        for( usIndex = mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_TASKS; usIndex++ )
        {
            xTaskParams[ usIndex ].usTaskTag = usIndex;
            xTaskParams[ usIndex ].xStatus = pdFAIL;
            xTaskParams[ usIndex ].pucTransmittedData = pucTransmittedData;

            xTaskParams[ usIndex ].xTaskCreated = pdFALSE;
            xResult = xTaskCreate( prvMultiTaskTest_Tx_Task, /* The function that implements the task. */
                                   "ClientTask",             /* Just a text name for the task to aid debugging. */
                                   mqttagenttestMULTI_TASK_TEST_TASKS_STACK_SIZE,
                                   &( xTaskParams[ usIndex ] ),
                                   mqttagenttestMULTI_TASK_TEST_TASKS_PRIORITY,
                                   &( xTaskParams[ usIndex ].xTaskHandle ) );
            TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );
            /* Flag Task as created for later delete. */
            xTaskParams[ usIndex ].xTaskCreated = pdTRUE;
        }

        /* When all task reaches the sync group, then the test is complete. */
        usEventMask = xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                                       0,
                                       mqttagenttestMULTI_TASK_TEST_EVENT_MASK,
                                       mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT_TICKS );
        TEST_ASSERT_EQUAL_MESSAGE( mqttagenttestMULTI_TASK_TEST_EVENT_MASK, usEventMask, "Timeout waiting for tasks" );

        /* Check which tasks have failed. If one failed, the test is failed. */
        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_TASKS; usIndex++ )
        {
            if( xTaskParams[ usIndex ].xStatus != pdPASS )
            {
                TEST_FAIL_MESSAGE( "Not all tasks completed successfully." );
            }
        }
    }

    /* The code below will delete all the memory that has been successfully allocated. */
    /* For each task not completed, delete the task.  */
    for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_TASKS; usIndex++ )
    {
        /* Check task has been created before deleting it. */
        if( xTaskParams[ usIndex ].xTaskCreated == pdTRUE )
        {
            vTaskDelete( xTaskParams[ usIndex ].xTaskHandle );
        }
    }

    /* Delete global group event. */
    if( xSyncEventGroup != NULL )
    {
        vEventGroupDelete( xSyncEventGroup );
    }

    for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
    {
        if( MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx != NULL )
        {
            vEventGroupDelete( MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx );
        }

        if( xTaskParams[ usIndex ].xSemaphore != NULL )
        {
            vSemaphoreDelete( xTaskParams[ usIndex ].xSemaphore );
        }
    }

    if( pucTransmittedData != NULL )
    {
        vPortFree( pucTransmittedData );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Receive Task
 *
 * The Receive tasks will take care of create, connecting, subscribing, disconnecting and deleting clients.
 * Also, y number of Transmit task are going to be created, each tasks are going to publish n number of MQTT messages
 * to each one of the x tasks.
 *
 * Once the task has connected, it will synch with the Transmit tasks. Since the task will receice y messages (from y Transmit task),
 * it will pend on Receive semaphore y times. Data is a simple counter and is checked in the callback.
 */
static void prvMultiTaskTest_Rx_Task( void * pvParameters )
{
    MQTTAgentSubscribeParams_t xSubscribeParams;
    MQTTAgentUnsubscribeParams_t xUnSubscribeParams;
    BaseType_t xResult, xStatus, xAgentCreated = pdFALSE;
    size_t xRecvLoop, xPubSubIndex;
    MQTTtestAgentTaskParams_t * pxTcptestEchoClientsTaskParams;
    EventGroupHandle_t * pxSyncEventRx;
    MQTTAgentHandle_t * pxMQTTHandle;
    MQTTAgentConnectParams_t xConnectParametersMultiTaskTest;
    MQTTtestAgentCbParam_t MQTTtestAgentCbParam;
    uint16_t usTaskTag;

    /* Copy over default parameters, then change for specific clientID connection. */
    memcpy( &xConnectParametersMultiTaskTest, &xDefaultConnectParameters, sizeof( MQTTAgentConnectParams_t ) );


    pxTcptestEchoClientsTaskParams = ( ( MQTTtestAgentTaskParams_t * ) pvParameters );
    usTaskTag = pxTcptestEchoClientsTaskParams->usTaskTag;
    pxSyncEventRx = &MQTTtestAgentMultiTestRxParam[ usTaskTag ].xSyncEventRx;
    pxMQTTHandle = &MQTTtestAgentMultiTestRxParam[ usTaskTag ].xMQTTHandle;
    xConnectParametersMultiTaskTest.pucClientId = MQTTtestAgentMultiTestRxParam[ usTaskTag ].cClientID;
    xConnectParametersMultiTaskTest.usClientIdLength = MQTTtestAgentMultiTestRxParam[ usTaskTag ].usClientIDLength;

    /* Setup subscribe parameters to subscribe to echo topic. */
    xSubscribeParams.pvPublishCallbackContext = &MQTTtestAgentCbParam;
    xSubscribeParams.pxPublishCallback = prvMultiTaskTestMQTTCallback;
    xSubscribeParams.xQoS = eMQTTQoS1;
    xSubscribeParams.pucTopic = MQTTtestAgentMultiTestRxParam[ usTaskTag ].cTopic;
    xSubscribeParams.usTopicLength = MQTTtestAgentMultiTestRxParam[ usTaskTag ].usTopicLength;

    xUnSubscribeParams.pucTopic = xSubscribeParams.pucTopic;
    xUnSubscribeParams.usTopicLength = xSubscribeParams.usTopicLength;

    MQTTtestAgentCbParam.xSemaphore = pxTcptestEchoClientsTaskParams->xSemaphore;

    /* Default is pass to avoid nesting. */
    xStatus = pdPASS;

    for( xRecvLoop = 0; xRecvLoop < mqttagenttestMULTI_TASK_TEST_MAX_LOOP; xRecvLoop++ )
    {
        if( mqttagenttestMULTI_TASK_TEST_RX_LOWP_SWITCH_MASK & xRecvLoop )
        {
            vTaskPrioritySet( NULL, mqttagenttestMULTI_TASK_TEST_LOW_PRIORITY );
        }
        else
        {
            vTaskPrioritySet( NULL, mqttagenttestMULTI_TASK_TEST_HIGH_PRIORITY );
        }

        /* The MQTT client object must be created before it can be used. */
        xResult = MQTT_AGENT_Create( pxMQTTHandle );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Could not create MQTT client.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        xAgentCreated = pdTRUE;

        /* Connect to the broker. */
        xResult = MQTT_AGENT_Connect( *pxMQTTHandle,
                                      &xConnectParametersMultiTaskTest,
                                      mqttagenttestTIMEOUT );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Could not connect MQTT client.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        /* Subscribe to the topic. */
        xResult = MQTT_AGENT_Subscribe( *pxMQTTHandle,
                                        &xSubscribeParams,
                                        mqttagenttestTIMEOUT );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Could not subscribe to topic.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        if( xEventGroupSync( *pxSyncEventRx, /* The event group used for the rendezvous. */
                             ( 1 << usTaskTag ),
                             ( 1 << usTaskTag ) | mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK,
                             mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS ) != ( ( 1 << usTaskTag ) | mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK ) )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Timed out waiting for Transmit tasks.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        /* Take the semaphore to ensure the message is Received. */
        for( xPubSubIndex = 0; xPubSubIndex < mqttagenttestMULTI_TASK_TEST_PUBLISH_PER_LOOP * mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; xPubSubIndex++ )
        {
            if( pdFALSE == xSemaphoreTake( pxTcptestEchoClientsTaskParams->xSemaphore, mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS ) )
            {
                mqttagenttestFAILUREPRINTF( ( "%s: Timed out waiting for pub from Transmit tasks.\r\n", __FUNCTION__ ) );
                xStatus = pdFAIL;
                break;
            }
            else /* Get status from the callback*/
            {
                if( MQTTtestAgentCbParam.xStatus != pdPASS )
                {
                    mqttagenttestFAILUREPRINTF( ( "%s: Error, corrupted data Received.\r\n", __FUNCTION__ ) );
                    xStatus = pdFAIL;
                    break;
                }
            }
        }

        /*If fail happened in for loop above, break out. */
        if( xStatus == pdFAIL )
        {
            break;
        }

        if( xEventGroupSync( *pxSyncEventRx, /* The event group used for the rendezvous. */
                             ( 1 << usTaskTag ),
                             ( 1 << usTaskTag ) | mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK,
                             mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS ) != ( ( 1 << usTaskTag ) | mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK ) )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Timed out waiting for Transmit tasks.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        /* Unsubscribe to the topic. */
        xResult = MQTT_AGENT_Unsubscribe( *pxMQTTHandle,
                                          &xUnSubscribeParams,
                                          mqttagenttestTIMEOUT );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Failed to unsubscribe.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        /* Disconnect. */
        xResult = MQTT_AGENT_Disconnect( *pxMQTTHandle, mqttagenttestTIMEOUT );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Failed to disconnect.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        xResult = MQTT_AGENT_Delete( *pxMQTTHandle );

        if( xResult != eMQTTAgentSuccess )
        {
            mqttagenttestFAILUREPRINTF( ( "%s: Failed to delete MQTT client.\r\n", __FUNCTION__ ) );
            xStatus = pdFAIL;
            break;
        }

        xAgentCreated = pdFALSE;
    }

    if( xAgentCreated == pdTRUE )
    {
        ( void ) MQTT_AGENT_Delete( *pxMQTTHandle );
    }

    pxTcptestEchoClientsTaskParams->xStatus = xStatus;
    xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                     ( 1 << usTaskTag ),
                     mqttagenttestMULTI_TASK_TEST_EVENT_MASK,
                     mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT_TICKS );
    vTaskSuspend( NULL ); /* Suspend this task. Will be deleted by the mainTask */
}

/*-----------------------------------------------------------*/

/**
 * @brief Transmit Task
 *
 * y number of Transmit task are going to be created, each tasks are going to publish n number of MQTT messages
 * to each one of the x tasks.
 */
static void prvMultiTaskTest_Tx_Task( void * pvParameters )
{
    size_t xSendLoop;
    BaseType_t xStatus, xReturn;
    size_t xPubSubIndex, usIndex;
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTtestAgentTaskParams_t * xTcptestEchoClientsTaskParams;

    xTcptestEchoClientsTaskParams = ( ( MQTTtestAgentTaskParams_t * ) pvParameters );

    xStatus = pdPASS;

    /* Transmit and Receive loop switch priorities on a different bit, so all combination of high and low priorities will be covered. */
    for( xSendLoop = 1; xSendLoop < mqttagenttestMULTI_TASK_TEST_MAX_LOOP + 1; xSendLoop++ )
    {
        if( mqttagenttestMULTI_TASK_TEST_TX_LOWP_SWITCH_MASK & xSendLoop )
        {
            vTaskPrioritySet( NULL, mqttagenttestMULTI_TASK_TEST_LOW_PRIORITY );
        }
        else
        {
            vTaskPrioritySet( NULL, mqttagenttestMULTI_TASK_TEST_HIGH_PRIORITY );
        }

        /* Wait for the Receive tasks to create and connect the agent. */

        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
        {
            if( xEventGroupSync( MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx,             /* The event group used for the rendezvous. */
                                 ( 1 << xTcptestEchoClientsTaskParams->usTaskTag ),                 /* The bit representing the Transmit task reaching the rendezvous. */
                                 ( mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK | ( 1 << usIndex ) ), /* Also wait for the Receive tasks. */
                                 mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS ) != ( mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK | ( 1 << usIndex ) ) )
            {
                mqttagenttestFAILUREPRINTF( ( "%s: Failed synching with Transmit and Receive tasks.\r\n", __FUNCTION__ ) );
                xStatus = pdFAIL;
                break;
            }
        }

        if( xStatus == pdFAIL )
        {
            break;
        }

        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
        {
            /* Setup the publish parameters. */
            memset( &( xPublishParameters ), 0x00, sizeof( xPublishParameters ) );
            xPublishParameters.pucTopic = MQTTtestAgentMultiTestRxParam[ usIndex ].cTopic;
            xPublishParameters.pvData = xTcptestEchoClientsTaskParams->pucTransmittedData;
            xPublishParameters.usTopicLength = MQTTtestAgentMultiTestRxParam[ usIndex ].usTopicLength;
            xPublishParameters.ulDataLength = ( uint32_t ) mqttagenttestMULTI_TASK_TEST_PUB_DATA_SIZE;
            xPublishParameters.xQoS = eMQTTQoS1;

            /* Publish the message. */
            for( xPubSubIndex = 0; xPubSubIndex < mqttagenttestMULTI_TASK_TEST_PUBLISH_PER_LOOP; xPubSubIndex++ )
            {
                xReturn = MQTT_AGENT_Publish( MQTTtestAgentMultiTestRxParam[ usIndex ].xMQTTHandle,
                                              &( xPublishParameters ),
                                              mqttagenttestTIMEOUT );

                if( xReturn != eMQTTAgentSuccess )
                {
                    mqttagenttestFAILUREPRINTF( ( "%s: Failed publishing to topic.\r\n", __FUNCTION__ ) );
                    xStatus = pdFAIL;
                    break;
                }
            }

            if( xStatus == pdFAIL )
            {
                break;
            }
        }

        if( xStatus == pdFAIL )
        {
            break;
        }

        for( usIndex = 0; usIndex < mqttagenttestMULTI_TASK_TEST_NUM_RX_TASKS; usIndex++ )
        {
            if( xEventGroupSync( MQTTtestAgentMultiTestRxParam[ usIndex ].xSyncEventRx,             /* The event group used for the rendezvous. */
                                 ( 1 << xTcptestEchoClientsTaskParams->usTaskTag ),                 /* The bit representing the Transmit task reaching the rendezvous. */
                                 ( mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK | ( 1 << usIndex ) ), /* Also wait for the Receive tasks. */
                                 mqttagenttestMULTI_TASK_TEST_SYNC_TIMEOUT_TICKS ) != ( mqttagenttestMULTI_TASK_TEST_TX_EVENT_MASK | ( 1 << usIndex ) ) )
            {
                mqttagenttestFAILUREPRINTF( ( "%s: Failed synching with Transmit and Receive tasks.\r\n", __FUNCTION__ ) );
                xStatus = pdFAIL;
                break;
            }
        }
    }

    xTcptestEchoClientsTaskParams->xStatus = xStatus;
    xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                     ( 1 << xTcptestEchoClientsTaskParams->usTaskTag ),
                     mqttagenttestMULTI_TASK_TEST_EVENT_MASK,
                     mqttagenttestMULTI_TASK_TEST_COMPLETE_TIMEOUT_TICKS );
    vTaskSuspend( NULL ); /* Suspend this task. Will be deleted by the mainTask */
}
