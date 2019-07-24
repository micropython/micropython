/*
 * Amazon FreeRTOS Shadow Demo V1.4.6
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
 * @file aws_shadow_lightbulb_on_off.c
 * @brief A simple shadow example.
 *
 * The simple Shadow lightbulb example to illustrate how client application and
 * things communicate with the Shadow service.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* MQTT include. */
#include "aws_mqtt_agent.h"

/* Demo configurations. */
#include "aws_demo_config.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"

/* Required for shadow APIs. */
#include "aws_shadow.h"

/* Required for shadow demo. */
#include "jsmn.h"

/* Task names. */
#define shadowDemoCHAR_TASK_NAME           "Shd-IOT-%d"
#define shadowDemoUPDATE_TASK_NAME         "ShDemoUpdt"

/* Format string and length of the initial reported state, "red". */
#define shadowDemoINITIAL_REPORT_FORMAT    "{\"%s\": \"red\"}"
#define shadowDemoINITIAL_REPORT_BUFFER_LENGTH \
    ( sizeof( shadowDemoCHAR_TASK_NAME ) + sizeof( shadowDemoINITIAL_REPORT_FORMAT ) )

/* JSON formats used in the Shadow tasks. Note the inclusion of the "clientToken"
 * key, which is REQUIRED by the Shadow API. The "clientToken" may be anything, as
 * long as it's unique. This demo uses "token-" suffixed with the RTOS tick count
 * at the time the JSON document is generated. */
#define shadowDemoREPORT_JSON       \
    "{"                             \
    "\"state\":{"                   \
    "\"reported\":"                 \
    "%.*s"                          \
    "},"                            \
    "\"clientToken\": \"token-%d\"" \
    "}"

#define shadowDemoDESIRED_JSON      \
    "{"                             \
    "\"state\":{"                   \
    "\"desired\":{"                 \
    "\"%s\":\"%s\""                 \
    "}"                             \
    "},"                            \
    "\"clientToken\": \"token-%d\"" \
    "}"

/* Maximum amount of time a Shadow function call may block. */
#define shadowDemoTIMEOUT                    pdMS_TO_TICKS( 30000UL )

/* Max size for the name of tasks generated for Shadow. */
#define shadowDemoCHAR_TASK_NAME_MAX_SIZE    15

/* How often the "desired" state of the bulb should be changed. */
#define shadowDemoSEND_UPDATE_MS             pdMS_TO_TICKS( ( 5UL * 1000UL ) )

/* Name of the thing. */
#define shadowDemoTHING_NAME                 clientcredentialIOT_THING_NAME

/* Maximum size of update JSON documents. */
#define shadowDemoBUFFER_LENGTH              128

/* Stack size for task that handles shadow delta and updates. */
#define shadowDemoUPDATE_TASK_STACK_SIZE     ( ( uint16_t ) configMINIMAL_STACK_SIZE * ( uint16_t ) 5 )

/* Maximum number of jsmn tokens. */
#define shadowDemoMAX_TOKENS                 40

/* Queue configuration parameters. */
#define shadowDemoSEND_QUEUE_WAIT_TICKS      3000
#define shadowDemoRECV_QUEUE_WAIT_TICKS      500
#define shadowDemoUPDATE_QUEUE_LENGTH        democonfigSHADOW_DEMO_NUM_TASKS * 2

/* The maximum amount of time tasks will wait for their updates to process.
 * Tasks should not continue executing until their updates have processed.*/
#define shadowDemoNOTIFY_WAIT_MS             pdMS_TO_TICKS( ( 100000UL ) )

/* An element of the Shadow update queue. */
typedef struct
{
    TaskHandle_t xTaskToNotify;
    uint32_t ulDataLength;
    char pcUpdateBuffer[ shadowDemoBUFFER_LENGTH ];
} ShadowQueueData_t;

/* The parameters of the Shadow tasks. */
typedef struct
{
    TaskHandle_t xTaskHandle;
    char cTaskName[ shadowDemoCHAR_TASK_NAME_MAX_SIZE ];
} ShadowTaskParam_t;

/* Shadow demo tasks. */
static void prvShadowInitTask( void * pvParameters );
static void prvChangeDesiredTask( void * pvParameters );
static void prvUpdateQueueTask( void * pvParameters );

/* Creates Shadow client and connects to MQTT broker. */
static ShadowReturnCode_t prvShadowClientCreateConnect( void );

/* Called when there's a difference between "reported" and "desired" in Shadow document. */
static BaseType_t prvDeltaCallback( void * pvUserData,
                                    const char * const pcThingName,
                                    const char * const pcDeltaDocument,
                                    uint32_t ulDocumentLength,
                                    MQTTBufferHandle_t xBuffer );

/* JSON functions. */
static uint32_t prvGenerateDesiredJSON( ShadowQueueData_t * const pxShadowQueueData,
                                        const char * const pcTaskName,
                                        uint8_t ucBulbState );
static uint32_t prvGenerateReportedJSON( ShadowQueueData_t * const pxShadowQueueData,
                                         const char * const pcReportedData,
                                         uint32_t ulReportedDataLength );
static BaseType_t prvIsStringEqual( const char * const pcJson,     /*lint !e971 can use char without signed/unsigned. */
                                    const jsmntok_t * const pxTok,
                                    const char * const pcString ); /*lint !e971 can use char without signed/unsigned. */

/* The update queue's handle, data structure, and memory. */
static QueueHandle_t xUpdateQueue = NULL;
static StaticQueue_t xStaticQueue;
static uint8_t ucQueueStorageArea[ shadowDemoUPDATE_QUEUE_LENGTH * sizeof( ShadowQueueData_t ) ];

/* The handle of the Shadow client shared across all tasks. */
static ShadowClientHandle_t xClientHandle;
/* Memory allocated to store the Shadow task params. */
static ShadowTaskParam_t xShadowTaskParamBuffer[ democonfigSHADOW_DEMO_NUM_TASKS ];

/*-----------------------------------------------------------*/

static uint32_t prvGenerateDesiredJSON( ShadowQueueData_t * const pxShadowQueueData,
                                        const char * const pcTaskName,
                                        uint8_t ucBulbState )
{
    /* Map cBulbState to strings. */
    static const char * const pColors[ 2 ] = { "green", "red" };

    /* Generate JSON. */
    return ( uint32_t ) snprintf( ( char * ) pxShadowQueueData->pcUpdateBuffer, shadowDemoBUFFER_LENGTH,
                                  shadowDemoDESIRED_JSON,
                                  pcTaskName,
                                  pColors[ ucBulbState ],
                                  ( int ) xTaskGetTickCount() );
}
/*-----------------------------------------------------------*/

static uint32_t prvGenerateReportedJSON( ShadowQueueData_t * const pxShadowQueueData,
                                         const char * const pcReportedData,
                                         uint32_t ulReportedDataLength )
{
    return ( uint32_t ) snprintf( ( char * ) ( pxShadowQueueData->pcUpdateBuffer ),
                                  shadowDemoBUFFER_LENGTH,
                                  shadowDemoREPORT_JSON,
                                  ( int ) ulReportedDataLength,
                                  pcReportedData,
                                  ( int ) xTaskGetTickCount() );
}

/*-----------------------------------------------------------*/

static BaseType_t prvIsStringEqual( const char * const pcJson,    /*lint !e971 can use char without signed/unsigned. */
                                    const jsmntok_t * const pxTok,
                                    const char * const pcString ) /*lint !e971 can use char without signed/unsigned. */
{
    uint32_t ulStringSize = ( uint32_t ) pxTok->end - ( uint32_t ) pxTok->start;
    BaseType_t xStatus = pdFALSE;

    if( pxTok->type == JSMN_STRING )
    {
        if( ( pcString[ ulStringSize ] == 0 ) &&
            ( strncmp( pcJson + pxTok->start, pcString, ulStringSize ) == 0 ) )
        {
            xStatus = pdTRUE;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvDeltaCallback( void * pvUserData,
                                    const char * const pcThingName,
                                    const char * const pcDeltaDocument,
                                    uint32_t ulDocumentLength,
                                    MQTTBufferHandle_t xBuffer )
{
    int32_t lNbTokens;
    uint16_t usTokenIndex;
    uint32_t ulStringSize;
    jsmn_parser xJSMNParser;
    ShadowQueueData_t xShadowQueueData;
    jsmntok_t pxJSMNTokens[ shadowDemoMAX_TOKENS ];

    /* Silence compiler warnings about unused variables. */
    ( void ) pvUserData;
    ( void ) xBuffer;
    ( void ) pcThingName;

    jsmn_init( &xJSMNParser );
    memset( &xShadowQueueData, 0x00, sizeof( ShadowQueueData_t ) );

    lNbTokens = ( int32_t ) jsmn_parse( &xJSMNParser,
                                        pcDeltaDocument,
                                        ( size_t ) ulDocumentLength,
                                        pxJSMNTokens,
                                        ( unsigned int ) shadowDemoMAX_TOKENS );

    if( lNbTokens > 0 )
    {
        /* Find the new reported state. */
        for( usTokenIndex = 0; usTokenIndex < ( uint16_t ) lNbTokens; usTokenIndex++ )
        {
            if( prvIsStringEqual( pcDeltaDocument, &pxJSMNTokens[ usTokenIndex ], "state" ) == pdTRUE )
            {
                ulStringSize = pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].end
                               - pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].start;
                configASSERT( ulStringSize < shadowDemoBUFFER_LENGTH );

                /* Generate a new JSON document with new reported state. */
                xShadowQueueData.ulDataLength = prvGenerateReportedJSON( &xShadowQueueData,
                                                                         &pcDeltaDocument[ pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].start ],
                                                                         ulStringSize );

                /* Add new reported state to update queue. */
                if( xQueueSendToBack( xUpdateQueue, &xShadowQueueData, shadowDemoSEND_QUEUE_WAIT_TICKS ) == pdTRUE )
                {
                    configPRINTF( ( "Successfully added new reported state to update queue.\r\n" ) );
                }
                else
                {
                    configPRINTF( ( "Update queue full, deferring reported state update.\r\n" ) );
                }

                break;
            }
        }
    }

    return pdFALSE;
}

/*-----------------------------------------------------------*/

static void prvUpdateQueueTask( void * pvParameters )
{
    ShadowReturnCode_t xReturn;
    ShadowOperationParams_t xUpdateParams;
    ShadowQueueData_t xShadowQueueData;

    ( void ) pvParameters;

    xUpdateParams.pcThingName = shadowDemoTHING_NAME;
    xUpdateParams.xQoS = eMQTTQoS0;
    xUpdateParams.pcData = xShadowQueueData.pcUpdateBuffer;
    /* Keep subscriptions across multiple calls to SHADOW_Update. */
    xUpdateParams.ucKeepSubscriptions = pdTRUE;

    for( ; ; )
    {
        if( xQueueReceive( xUpdateQueue, &xShadowQueueData, shadowDemoRECV_QUEUE_WAIT_TICKS ) == pdTRUE )
        {
            configPRINTF( ( "Performing Thing Shadow update.\r\n" ) );
            xUpdateParams.ulDataLength = xShadowQueueData.ulDataLength;

            xReturn = SHADOW_Update( xClientHandle, &xUpdateParams, shadowDemoTIMEOUT );

            if( xReturn == eShadowSuccess )
            {
                configPRINTF( ( "Successfully performed update.\r\n" ) );
            }
            else
            {
                configPRINTF( ( "Update failed, returned %d.\r\n", xReturn ) );
            }

            /* Notify tasks that their update was completed. */
            if( xShadowQueueData.xTaskToNotify != NULL )
            {
                xTaskNotifyGive( xShadowQueueData.xTaskToNotify );
            }
        }
    }
}
/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvShadowClientCreateConnect( void )
{
    MQTTAgentConnectParams_t xConnectParams;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;

    xCreateParams.xMQTTClientType = eDedicatedMQTTClient;
    xReturn = SHADOW_ClientCreate( &xClientHandle, &xCreateParams );

    if( xReturn == eShadowSuccess )
    {
        memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
        xConnectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
        xConnectParams.usPort = clientcredentialMQTT_BROKER_PORT;

        xConnectParams.xFlags = democonfigMQTT_AGENT_CONNECT_FLAGS;
        xConnectParams.pcCertificate = NULL;
        xConnectParams.ulCertificateSize = 0;
        xConnectParams.pxCallback = NULL;
        xConnectParams.pvUserData = &xClientHandle;

        xConnectParams.pucClientId = ( const uint8_t * ) ( clientcredentialIOT_THING_NAME );
        xConnectParams.usClientIdLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );
        xReturn = SHADOW_ClientConnect( xClientHandle,
                                        &xConnectParams,
                                        shadowDemoTIMEOUT );

        if( xReturn != eShadowSuccess )
        {
            configPRINTF( ( "Shadow_ClientConnect unsuccessful, returned %d.\r\n", xReturn ) );
        }
    }
    else
    {
        configPRINTF( ( "Shadow_ClientCreate unsuccessful, returned %d.\r\n", xReturn ) );
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvChangeDesiredTask( void * pvParameters )
{
    uint8_t ucBulbState = 0;
    uint32_t ulInitialReportLength;
    TickType_t xLastWakeTime;
    ShadowTaskParam_t * pxShadowTaskParam;
    ShadowQueueData_t xShadowQueueData;
    char pcInitialReportBuffer[ shadowDemoINITIAL_REPORT_BUFFER_LENGTH ];

    /* Initialize parameters. */
    pxShadowTaskParam = ( ShadowTaskParam_t * ) pvParameters; /*lint !e9087 Safe cast from context. */
    memset( &xShadowQueueData, 0x00, sizeof( ShadowQueueData_t ) );
    xShadowQueueData.xTaskToNotify = pxShadowTaskParam->xTaskHandle;

    /* Add the initial state to the update queue, wait for the update to complete. */
    ulInitialReportLength = snprintf( pcInitialReportBuffer,
                                      shadowDemoINITIAL_REPORT_BUFFER_LENGTH,
                                      shadowDemoINITIAL_REPORT_FORMAT,
                                      pxShadowTaskParam->cTaskName );
    xShadowQueueData.ulDataLength = prvGenerateReportedJSON( &xShadowQueueData,
                                                             pcInitialReportBuffer,
                                                             ulInitialReportLength );
    /* The calls below should never fail because the queue should be empty currently. */
    configASSERT( xQueueSendToBack( xUpdateQueue, &xShadowQueueData, shadowDemoSEND_QUEUE_WAIT_TICKS ) == pdTRUE );
    configASSERT( ulTaskNotifyTake( pdTRUE, shadowDemoNOTIFY_WAIT_MS ) == 1 );

    xLastWakeTime = xTaskGetTickCount();

    /* Keep changing the desired state of light bulb periodically. */
    for( ; ; )
    {
        configPRINTF( ( "%s changing desired state.\r\n", pxShadowTaskParam->cTaskName ) );

        /* Toggle the desired state and generate a new JSON document. */
        ucBulbState = !( ucBulbState );
        xShadowQueueData.ulDataLength = prvGenerateDesiredJSON( &xShadowQueueData,
                                                                pxShadowTaskParam->cTaskName,
                                                                ucBulbState );

        /* Add the new desired state to the update queue. */
        if( xQueueSendToBack( xUpdateQueue, &xShadowQueueData, shadowDemoSEND_QUEUE_WAIT_TICKS ) == pdTRUE )
        {
            /* If the new desired state was successfully added, wait for notification that the update completed. */
            configASSERT( ulTaskNotifyTake( pdTRUE, shadowDemoNOTIFY_WAIT_MS ) == 1 );
            configPRINTF( ( "%s done changing desired state.\r\n", pxShadowTaskParam->cTaskName ) );
        }
        else
        {
            configPRINTF( ( "Update queue full, deferring desired state change.\r\n" ) );
        }

        vTaskDelayUntil( &xLastWakeTime, shadowDemoSEND_UPDATE_MS );
    }
}
/*-----------------------------------------------------------*/

static void prvShadowInitTask( void * pvParameters )
{
    uint8_t ucTask;
    ShadowReturnCode_t xReturn;
    ShadowOperationParams_t xOperationParams;
    ShadowCallbackParams_t xCallbackParams;

    ( void ) pvParameters;

    /* Initialize the update queue and Shadow client; set all pending updates to false. */
    xUpdateQueue = xQueueCreateStatic( shadowDemoUPDATE_QUEUE_LENGTH,
                                       sizeof( ShadowQueueData_t ),
                                       ucQueueStorageArea,
                                       &xStaticQueue );
    xReturn = prvShadowClientCreateConnect();

    if( xReturn == eShadowSuccess )
    {
        xOperationParams.pcThingName = shadowDemoTHING_NAME;
        xOperationParams.xQoS = eMQTTQoS0;
        xOperationParams.pcData = NULL;
        /* Don't keep subscriptions, since SHADOW_Delete is only called here once. */
        xOperationParams.ucKeepSubscriptions = pdFALSE;

        /* Delete any previous shadow. */
        xReturn = SHADOW_Delete( xClientHandle,
                                 &xOperationParams,
                                 shadowDemoTIMEOUT );

        /* Atttempting to delete a non-existant shadow returns eShadowRejectedNotFound.
         * Either eShadowSuccess or eShadowRejectedNotFound signify that there's no
         * existing Thing Shadow, so both values are ok. */
        if( ( xReturn == eShadowSuccess ) || ( xReturn == eShadowRejectedNotFound ) )
        {
            /* Register callbacks. This demo doesn't use updated or deleted callbacks, so
             * those members are set to NULL. The callbacks are registered after deleting
             * the Shadow so that any previous Shadow doesn't unintentionally trigger the
             * delta callback.*/
            xCallbackParams.pcThingName = shadowDemoTHING_NAME;
            xCallbackParams.xShadowUpdatedCallback = NULL;
            xCallbackParams.xShadowDeletedCallback = NULL;
            xCallbackParams.xShadowDeltaCallback = prvDeltaCallback;

            xReturn = SHADOW_RegisterCallbacks( xClientHandle,
                                                &xCallbackParams,
                                                shadowDemoTIMEOUT );
        }
    }

    if( xReturn == eShadowSuccess )
    {
        configPRINTF( ( "Shadow client initialized.\r\n" ) );

        /* Create the update task which will process the update queue. */
        ( void ) xTaskCreate( prvUpdateQueueTask,
                              shadowDemoUPDATE_TASK_NAME,
                              shadowDemoUPDATE_TASK_STACK_SIZE,
                              NULL,
                              democonfigSHADOW_DEMO_TASK_PRIORITY,
                              NULL );

        /* Create the Shadow demo tasks which update the "desired" states. */
        for( ucTask = 0; ucTask < ( uint8_t ) democonfigSHADOW_DEMO_NUM_TASKS; ucTask++ )
        {
            ( void ) snprintf( ( char * ) ( &( xShadowTaskParamBuffer[ ucTask ] ) )->cTaskName,
                               shadowDemoCHAR_TASK_NAME_MAX_SIZE,
                               shadowDemoCHAR_TASK_NAME,
                               ucTask );
            ( void ) xTaskCreate( prvChangeDesiredTask,
                                  ( const char * ) ( &( xShadowTaskParamBuffer[ ucTask ] ) )->cTaskName,
                                  democonfigSHADOW_DEMO_TASK_STACK_SIZE,
                                  &( xShadowTaskParamBuffer[ ucTask ] ),
                                  democonfigSHADOW_DEMO_TASK_PRIORITY,
                                  &( ( xShadowTaskParamBuffer[ ucTask ] ).xTaskHandle ) );
        }
    }
    else
    {
        configPRINTF( ( "Failed to initialize Shadow client.\r\n" ) );
    }

    vTaskDelete( NULL );
}

/* Create the shadow demo main task which will act as a client application to
 * request periodic change in state (color) of light bulb.  */
void vStartShadowDemoTasks( void )
{
    ( void ) xTaskCreate( prvShadowInitTask,
                          "MainDemoTask",
                          shadowDemoUPDATE_TASK_STACK_SIZE,
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL );
}
/*-----------------------------------------------------------*/
