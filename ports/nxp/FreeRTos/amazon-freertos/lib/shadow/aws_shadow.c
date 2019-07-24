/*
 * Amazon FreeRTOS Shadow V1.0.5
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
 * @file aws_shadow.c
 * @brief Shadow API. Provide simple function to modify/create/delete Things shadows.
 */

/* C library includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* AWS includes. */
#include "aws_shadow_config.h"
#include "aws_shadow_config_defaults.h"
#include "aws_shadow.h"
#include "aws_shadow_json.h"

/**
 * @brief Format strings for the AWS IoT Shadow MQTT topics.
 * Refer to docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html
 * If the MQTT topics change, update them here. Use %s for 'thingName'.
 *
 * These topics may also be changed to trim Thing Shadow messages.
 * Refer to docs.aws.amazon.com/iot/latest/developerguide/using-device-shadows.html#device-shadow-trim-messages
 */
/** @{ */
#define shadowTOPIC_PREFIX              "$aws/things/%s/shadow/" /* All shadow topics begin with this. */
#define shadowTOPIC_OPERATION_UPDATE    "update"
#define shadowTOPIC_OPERATION_GET       "get"
#define shadowTOPIC_OPERATION_DELETE    "delete"
#define shadowTOPIC_SUFFIX_ACCEPTED     "/accepted" /* All accepted topics end with this. */
#define shadowTOPIC_SUFFIX_REJECTED     "/rejected" /* All rejected topics end with this. */

#define shadowTOPIC_UPDATE              shadowTOPIC_PREFIX shadowTOPIC_OPERATION_UPDATE
#define shadowTOPIC_UPDATE_ACCEPTED     shadowTOPIC_UPDATE shadowTOPIC_SUFFIX_ACCEPTED
#define shadowTOPIC_UPDATE_REJECTED     shadowTOPIC_UPDATE shadowTOPIC_SUFFIX_REJECTED
#define shadowTOPIC_UPDATE_DOCUMENTS    shadowTOPIC_UPDATE "/documents"
#define shadowTOPIC_UPDATE_DELTA        shadowTOPIC_UPDATE "/delta"
#define shadowTOPIC_GET                 shadowTOPIC_PREFIX shadowTOPIC_OPERATION_GET
#define shadowTOPIC_GET_ACCEPTED        shadowTOPIC_GET shadowTOPIC_SUFFIX_ACCEPTED
#define shadowTOPIC_GET_REJECTED        shadowTOPIC_GET shadowTOPIC_SUFFIX_REJECTED
#define shadowTOPIC_DELETE              shadowTOPIC_PREFIX shadowTOPIC_OPERATION_DELETE
#define shadowTOPIC_DELETE_ACCEPTED     shadowTOPIC_DELETE shadowTOPIC_SUFFIX_ACCEPTED
#define shadowTOPIC_DELETE_REJECTED     shadowTOPIC_DELETE shadowTOPIC_SUFFIX_REJECTED
/** @} */

/** Maximum length of a Shadow MQTT topic. 128 is currently the longest Thing
* Name that AWS IoT accepts. shadowTOPIC_UPDATE_DOCUMENTS is currently the
* longest topic. All Shadow MQTT topics should be shorter than this value. */
#define configMAX_THING_NAME_LENGTH    128
#define shadowTOPIC_BUFFER_LENGTH      ( configMAX_THING_NAME_LENGTH + ( int16_t ) sizeof( shadowTOPIC_UPDATE_DOCUMENTS ) )

#if shadowconfigENABLE_DEBUG_LOGS == 1
    #define Shadow_debug_printf( X )    configPRINTF( X )
#else
    #define Shadow_debug_printf( X )
#endif

/**
 * @brief Shadow operation that can be parsed from MQTT topics.
 *
 */
typedef enum ShadowOperationName
{
    eShadowOperationUpdate,
    eShadowOperationGet,
    eShadowOperationDelete,
    eShadowOperationUpdateDocuments,
    eShadowOperationUpdateDelta,
    eShadowOperationDeletedByAnother,
    eShadowOperationOther
} ShadowOperationName_t;

/**
 * @brief Data on the Shadow operation currently in progress.
 *
 */
typedef struct ShadowOperationData
{
    ShadowOperationName_t xOperationInProgress;
    ShadowOperationParams_t * pxOperationParams;
} ShadowOperationData_t;

/**
 * @brief Data on the timeout by which a function needs to complete.
 *
 */
typedef struct TimeOutData
{
    TimeOut_t xTimeOut;
    TickType_t xTicksRemaining;
} TimeOutData_t;

/**
 * @brief Data passed to prvShadowOperation.
 *
 */
typedef struct ShadowOperationCallParams
{
    BaseType_t xShadowClientID;

    ShadowOperationName_t xOperationName;
    const char * pcOperationName;

    /* Operation MQTT topics. */
    const char * pcOperationTopic;
    const char * pcOperationAcceptedTopic;
    const char * pcOperationRejectedTopic;

    /* The message to publish to MQTT topics. */
    const char * pcPublishMessage;
    uint32_t ulPublishMessageLength;

    ShadowOperationParams_t * pxOperationParams;
    TickType_t xTimeoutTicks;
} ShadowOperationCallParams_t;

/**
 * @brief An entry of the callback catalog.
 *
 * The callback catalog is the member of the Shadow Client that stores the
 * callback functions associated with Thing Names.
 */
typedef struct CallbackCatalogEntry
{
    ShadowCallbackParams_t xCallbackInfo;
    BaseType_t xInUse;
} CallbackCatalogEntry_t;

/**
 * @brief The Shadow Client.
 *
 */
typedef struct ShadowClient
{
    /* MQTT Client handle. */
    MQTTAgentHandle_t xMQTTClient;

    /* Shadow Client flags. */
    BaseType_t xInUse;
    BaseType_t xUpdateSubscribed;
    BaseType_t xGetSubscribed;
    BaseType_t xDeleteSubscribed;

    /* Synchronization mechanisms. */
    SemaphoreHandle_t xOperationDataMutex; /* Allows only one in-progress operation. */
    SemaphoreHandle_t xOperationMutex;     /* Allows only one in-progress operation. */
    SemaphoreHandle_t xCallbackSemaphore;  /* Communication with callbacks. */
    StaticSemaphore_t xOperationMutexBuffer;
    StaticSemaphore_t xCallbackSemaphoreBuffer;
    StaticSemaphore_t xOperationDataMutexBuffer;

    /* Data shared between blocking function and MQTT callback. */
    ShadowOperationData_t * pxOperationData;

    /* The callback functions pass data to the API calls by setting xOperationResult.
     * This value is volatile to ensure that the compiler knows the value can change
     * without anything happening in the API call. */
    volatile ShadowReturnCode_t xOperationResult;

    /* Callback catalog stores Thing Names and registered callbacks. */
    CallbackCatalogEntry_t xCallbackCatalog[ shadowconfigMAX_THINGS_WITH_CALLBACKS ];

    /* Stores the topic of the in-progress operation. Some of the Shadow API's
     * static functions depend on this buffer remaining the same throughout a
     * Shadow API operation. Therefore, only prvShadowOperation (and the static
     * functions called by prvShadowOperation) should modify the contents of this
     * buffer. */
    uint8_t ucTopicBuffer[ shadowTOPIC_BUFFER_LENGTH ];
} ShadowClient_t;

/**
 * @brief Searches memory for a not-in-use Shadow Client.
 *
 */
static BaseType_t prvGetFreeShadowClient( void );

/**
 * @brief Returns the slot index of callback catalog for matching thing name, if thing name not found
 * returns the next available unused slot index in catalog.
 *
 */
static BaseType_t prvGetCallbackCatalogEntry( CallbackCatalogEntry_t * const xCallbackCatalog,
                                              const char * const pcThingName );


/**
 * @brief Wrapper function for MQTT API calls; converts MQTTAgentReturnCode_t to
 * ShadowReturnCode_t and optionally prints debug messages.
 *
 */
static ShadowReturnCode_t prvConvertMQTTReturnCode( MQTTAgentReturnCode_t xMQTTReturn,
                                                    ShadowClientHandle_t xShadowClientHandle,
                                                    const char * const pcDebugMessageSubject );


/**
 * @brief Function to register a callback by subscribing to Shadow MQTT topics or removes a callback
 * Function by unsubscribing to Shadow MQTT topics
 *
 */
static ShadowReturnCode_t prvRegisterCallback( BaseType_t xShadowClientID,
                                               const void ** const ppvOldCallback,
                                               const void ** const ppvNewCallback,
                                               const char * const pcThingName,
                                               const uint8_t * const pucTopicFormat,
                                               TickType_t xTimeoutTicks );

/**
 * @brief Subscribes to an accepted and rejected topic
 *
 */
static ShadowReturnCode_t prvShadowSubscribeToAcceptedRejected( BaseType_t
                                                                xShadowClientID,
                                                                const char * const pcThingName,
                                                                const char * const pcAcceptedTopic,
                                                                const char * const pcRejectedTopic,
                                                                TimeOutData_t * const pxTimeOutData );

/**
 * @brief Unsubscribe to an accepted and rejected topic.
 *
 */
static ShadowReturnCode_t prvShadowUnsubscribeFromAcceptedRejected( BaseType_t xShadowClientID,
                                                                    const char * const pcThingName,
                                                                    const char * const pcAcceptedTopic,
                                                                    const char * const pcRejectedTopic,
                                                                    TimeOutData_t * const pxTimeOutData );

/**
 * @brief Universal MQTT callback; parses topics for Thing Name and operation matches.
 *
 */
static BaseType_t prvShadowMQTTCallback( void * pvUserData,
                                         const MQTTAgentCallbackParams_t * const pxCallbackParams );

/**
 * @brief Parses "accepted" or "rejected" from MQTT topic.
 *
 */
static ShadowReturnCode_t prvParseShadowOperationStatus( const uint8_t * const
                                                         pucTopic,
                                                         uint16_t usTopicLength );

/**
 * @briefMatch topic with registered callback and return reference to catalog entry found or NULL in not found.
 */
static const CallbackCatalogEntry_t * prvMatchCallbackTopic( const ShadowClient_t *
                                                             const pxShadowClient,
                                                             const uint8_t * const pucTopic,
                                                             uint16_t usTopicLength,
                                                             ShadowOperationName_t * const pxOperationName );

/**
 * @brief Update callback for Shadow Operations.
 */
static void prvShadowUpdateCallback( BaseType_t xShadowClientID,
                                     ShadowReturnCode_t xResult,
                                     const ShadowOperationParams_t * const pxParams,
                                     const char * const pcData,
                                     uint32_t ulDataLength );

/**
 * @brief Get callback for shadow operations
 */
static void prvShadowGetCallback( BaseType_t xShadowClientID,
                                  ShadowReturnCode_t xResult,
                                  ShadowOperationParams_t * const pxParams,
                                  const char * const pcData,
                                  uint32_t ulDataLength,
                                  MQTTBufferHandle_t xBuffer );

/**
 * @briefDelete callback for shadow operations
 */
static void prvShadowDeleteCallback( BaseType_t xShadowClientID,
                                     ShadowReturnCode_t xResult,
                                     const char * const pcData,
                                     uint32_t ulDataLength );

/**
 * @brief Handles error codes and messages in callbacks.
 */
static ShadowReturnCode_t prvGetErrorCodeAndMessage( const char * const pcData,
                                                     uint32_t ulDataLength,
                                                     BaseType_t xShadowClientID,
                                                     const char * const pcOperationName );

/**
 * @briefShadow Operation common code.
 */
static ShadowReturnCode_t prvShadowOperation( ShadowOperationCallParams_t * pxParams );

static void prvSetSubscribedFlag( ShadowClient_t * const pxShadowClient,
                                  ShadowOperationName_t xOperationName,
                                  BaseType_t xValue );

static uint8_t prvGetSubscribedFlag( const ShadowClient_t * const pxShadowClient,
                                     ShadowOperationName_t xOperationName );

/**
 * @brief Memory allocated to store Shadow Clients.
 */
static ShadowClient_t xShadowClients[ shadowconfigMAX_CLIENTS ];

/**
 * @brief Custom prvCreateTopic function since prvCreateTopic is not MISRA 2012 compliant (rule 21.6) .
 */
static uint16_t prvCreateTopic( char * pcTopicString,
                                const uint16_t usBufferLength,
                                const char * pcTopicFormat,
                                const char * pcThingName );

/*-----------------------------------------------------------*/

static BaseType_t prvGetFreeShadowClient( void )
{
    BaseType_t xIterator, xReturn = -1;

    taskENTER_CRITICAL();
    {
        for( xIterator = 0; xIterator < shadowconfigMAX_CLIENTS; xIterator++ )
        {
            if( xShadowClients[ xIterator ].xInUse == pdFALSE )
            {
                xShadowClients[ xIterator ].xInUse = pdTRUE;
                xReturn = xIterator;
                break;
            }
        }
    }
    taskEXIT_CRITICAL();

    return xReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGetCallbackCatalogEntry( CallbackCatalogEntry_t * const xCallbackCatalog,
                                              const char * const pcThingName ) /*_RB_ I find the name of this function confusing compared to what it is actually doing. */
{                                                                              /* Todo: sub manager. */
    CallbackCatalogEntry_t * pxCallbackCatalogEntry;
    BaseType_t xIterator, xReturn = -1, xThingNameFound = pdFALSE;
    size_t xThingNameLengh;
    size_t xThingName_cb_Lengh;

    taskENTER_CRITICAL();
    {
        for( xIterator = 0; xIterator < shadowconfigMAX_THINGS_WITH_CALLBACKS; xIterator++ )
        {
            pxCallbackCatalogEntry = &( xCallbackCatalog[ xIterator ] );

            if( pxCallbackCatalogEntry->xInUse == pdFALSE )
            {
                xReturn = xIterator;
            }
            else
            {
                xThingNameLengh = strlen( pxCallbackCatalogEntry->xCallbackInfo.pcThingName );
                xThingName_cb_Lengh = strlen( pcThingName );

                if( xThingNameLengh == xThingName_cb_Lengh )
                {
                    if( strncmp( pcThingName,
                                 pxCallbackCatalogEntry->xCallbackInfo.pcThingName,
                                 xThingNameLengh ) == 0 )
                    {
                        xReturn = xIterator;
                        xThingNameFound = pdTRUE;
                        break;
                    }
                }
            }
        }
    }
    taskEXIT_CRITICAL();

    if( xThingNameFound == pdFALSE )
    {
        xCallbackCatalog[ xReturn ].xInUse = pdTRUE;
        xCallbackCatalog[ xReturn ].xCallbackInfo.pcThingName = pcThingName;
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvConvertMQTTReturnCode( MQTTAgentReturnCode_t xMQTTReturn,
                                                    ShadowClientHandle_t xShadowClientHandle,
                                                    const char * const pcDebugMessageSubject )
{
    ShadowReturnCode_t xReturn = eShadowUnknown;

    switch( xMQTTReturn )
    {
        case eMQTTAgentSuccess:
            Shadow_debug_printf( ( "[Shadow %d] MQTT: %s succeeded.\r\n",
                                   ( BaseType_t ) xShadowClientHandle, /*lint !e923 Safe cast from pointer handle. */
                                   pcDebugMessageSubject ) );
            xReturn = eShadowSuccess;
            break;

        case eMQTTAgentTimeout:
            Shadow_debug_printf( ( "[Shadow %d] MQTT: %s timed out.\r\n",
                                   ( BaseType_t ) xShadowClientHandle, /*lint !e923 Safe cast from pointer handle. */
                                   pcDebugMessageSubject ) );
            xReturn = eShadowTimeout;
            break;

        case eMQTTAgentFailure:
        default:
            Shadow_debug_printf( ( "[Shadow %d] MQTT: %s failed.\r\n",
                                   ( BaseType_t ) xShadowClientHandle, /*lint !e923 Safe cast from pointer handle. */
                                   pcDebugMessageSubject ) );
            xReturn = eShadowFailure;
            break;
    }

    /* Prevent compiler warning in case Shadow_debug_printf() is not defined. */
    ( void ) xShadowClientHandle;
    ( void ) pcDebugMessageSubject;

    return xReturn;
}

/*-----------------------------------------------------------*/

static uint16_t prvCreateTopic( char * pcTopicString,
                                const uint16_t usBufferLength,
                                const char * pcTopicFormat,
                                const char * pcThingName )
{
    uint16_t usTopicFormatIdx;
    uint16_t usThingIdx;
    uint16_t usTopicFormatSize;
    uint16_t usThingNameSize;
    uint16_t usTopicSize;
    uint16_t usTopicIdx = 0;
    char cCurrentChar;
    char cPrevChar = '0';

    usTopicFormatSize = ( uint16_t ) strlen( pcTopicFormat );
    usThingNameSize = ( uint16_t ) strlen( pcThingName );

    /* Buffer length check. */
    if( usTopicFormatSize + usThingNameSize >= usBufferLength )
    {
        return 0;
    }

    /* The topic format string is expected to include a %s. However, the length
     * check above assumes, worst case, that the %s was for some reason missing. */
    usTopicSize = usTopicFormatSize + usThingNameSize - ( uint16_t ) 2;

    for( usTopicFormatIdx = 0;
         usTopicFormatIdx < usTopicFormatSize;
         usTopicFormatIdx++ )
    {
        cCurrentChar = pcTopicFormat[ usTopicFormatIdx ];

        if( ( cPrevChar == '%' ) && ( cCurrentChar == 's' ) )
        {
            for( usThingIdx = 0;
                 usThingIdx < usThingNameSize;
                 usThingIdx++ )
            {
                pcTopicString[ usThingIdx + usTopicFormatIdx - ( uint16_t ) 1 ]
                    = ( char ) pcThingName[ usThingIdx ];
            }

            /* Remove 1 because we backtracked in order to overwrite the %s. */
            usTopicIdx += usThingNameSize - ( uint16_t ) 1;
        }
        else
        {
            pcTopicString[ usTopicIdx ] = cCurrentChar;
            usTopicIdx++;
        }

        cPrevChar = cCurrentChar;
    }

    pcTopicString[ usTopicIdx ] = '\0';

    return usTopicSize;
}

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvRegisterCallback( BaseType_t xShadowClientID,
                                               const void ** const ppvOldCallback,
                                               const void ** const ppvNewCallback,
                                               const char * const pcThingName,
                                               const uint8_t * const pucTopicFormat,
                                               TickType_t xTimeoutTicks )
{
    uint8_t ucTopicString[ shadowTOPIC_BUFFER_LENGTH ];
    ShadowReturnCode_t xReturn = eShadowSuccess;
    MQTTAgentSubscribeParams_t xSubscribeParams;
    MQTTAgentUnsubscribeParams_t xUnsubscribeParams;
    ShadowClient_t * pxShadowClient;
    MQTTAgentReturnCode_t xMQTTReturn;
    uint16_t usTopicLength;

    usTopicLength = prvCreateTopic( ( char * ) ucTopicString, shadowTOPIC_BUFFER_LENGTH,
                                    ( const char * ) pucTopicFormat, pcThingName );
    pxShadowClient = &( xShadowClients[ xShadowClientID ] );

    if( *ppvOldCallback != NULL )
    {
        xUnsubscribeParams.usTopicLength = usTopicLength;
        xUnsubscribeParams.pucTopic = ucTopicString;

        xMQTTReturn = MQTT_AGENT_Unsubscribe( pxShadowClient->xMQTTClient,
                                              &xUnsubscribeParams,
                                              xTimeoutTicks );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Unsubscribe from callback topic" );
    }

    /* Registering a new callback; subscribe to topic. */
    if( *ppvNewCallback != NULL )
    {
        xSubscribeParams.usTopicLength = usTopicLength;
        xSubscribeParams.pucTopic = ucTopicString;

        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
            xSubscribeParams.pvPublishCallbackContext = NULL;
            xSubscribeParams.pxPublishCallback = NULL;
        #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

        /* Shadow service always publishes QoS 1, regardless of the value below. */
        xSubscribeParams.xQoS = eMQTTQoS1;

        xMQTTReturn = MQTT_AGENT_Subscribe( pxShadowClient->xMQTTClient,
                                            &xSubscribeParams,
                                            xTimeoutTicks );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Subscribe to callback topic" );
    }

    /* Change the callback. */
    if( xReturn == eShadowSuccess )
    {
        *ppvOldCallback = ( *ppvNewCallback );
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvShadowSubscribeToAcceptedRejected( BaseType_t
                                                                xShadowClientID,
                                                                const char * const pcThingName,
                                                                const char * const
                                                                pcAcceptedTopic,
                                                                const char * const pcRejectedTopic,
                                                                TimeOutData_t * const pxTimeOutData )
{
    ShadowReturnCode_t xReturn;
    ShadowClient_t * pxShadowClient;
    MQTTAgentSubscribeParams_t xSubscribeParams;
    MQTTAgentUnsubscribeParams_t xUnsubscribeParams;
    MQTTAgentReturnCode_t xMQTTReturn;
    TickType_t xTimeoutTicks;

    pxShadowClient = &( xShadowClients[ xShadowClientID ] );

    /* MQTT subscription parameters. */
    xSubscribeParams.pucTopic = pxShadowClient->ucTopicBuffer;
    /* Shadow service always publishes QoS 1, regardless of the value below. */
    xSubscribeParams.xQoS = eMQTTQoS1;

    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        xSubscribeParams.pvPublishCallbackContext = NULL;
        xSubscribeParams.pxPublishCallback = NULL;
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    /* Fill the accepted topic. */
    xSubscribeParams.usTopicLength = prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                                     shadowTOPIC_BUFFER_LENGTH,
                                                     pcAcceptedTopic,
                                                     pcThingName );

    xMQTTReturn = MQTT_AGENT_Subscribe( pxShadowClient->xMQTTClient,
                                        &xSubscribeParams,
                                        pxTimeOutData->xTicksRemaining );

    xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                        ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                        "Subscribe to accepted topic" );

    if( xReturn == eShadowSuccess )
    {
        /* Fill the rejected topic. */
        xSubscribeParams.usTopicLength = prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                                         shadowTOPIC_BUFFER_LENGTH,
                                                         pcRejectedTopic, pcThingName );

        xMQTTReturn = MQTT_AGENT_Subscribe( pxShadowClient->xMQTTClient,
                                            &xSubscribeParams,
                                            pxTimeOutData->xTicksRemaining );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Subscribe to rejected topic" );

        if( xReturn != eShadowSuccess )
        {
            xUnsubscribeParams.usTopicLength = prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                                               shadowTOPIC_BUFFER_LENGTH,
                                                               pcAcceptedTopic,
                                                               pcThingName );

            xUnsubscribeParams.pucTopic = pxShadowClient->ucTopicBuffer;

            xTimeoutTicks = pdMS_TO_TICKS( shadowconfigCLEANUP_TIME_MS );

            ( void ) MQTT_AGENT_Unsubscribe( pxShadowClient->xMQTTClient,
                                             &xUnsubscribeParams,
                                             xTimeoutTicks );

            ( void ) prvConvertMQTTReturnCode( xMQTTReturn,
                                               ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                               "Cleanup: Unsubscribe from accepted topic" );
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
static ShadowReturnCode_t prvShadowUnsubscribeFromAcceptedRejected( BaseType_t
                                                                    xShadowClientID,
                                                                    const char * const pcThingName,
                                                                    const char * const
                                                                    pcAcceptedTopic,
                                                                    const char * const pcRejectedTopic,
                                                                    TimeOutData_t * const pxTimeOutData )
{
    ShadowReturnCode_t xReturn = eShadowFailure;
    ShadowClient_t * pxShadowClient;
    MQTTAgentUnsubscribeParams_t xUnsubscribeParams;
    MQTTAgentReturnCode_t xMQTTReturn;

    pxShadowClient = &( xShadowClients[ xShadowClientID ] );

    /* MQTT unsubscribe parameters. */
    xUnsubscribeParams.pucTopic = pxShadowClient->ucTopicBuffer;

    if( pcAcceptedTopic != NULL )
    {
        /* Fill the accepted topic. */
        xUnsubscribeParams.usTopicLength = prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                                           shadowTOPIC_BUFFER_LENGTH,
                                                           pcAcceptedTopic,
                                                           pcThingName );

        xMQTTReturn = MQTT_AGENT_Unsubscribe( pxShadowClient->xMQTTClient,
                                              &xUnsubscribeParams,
                                              pxTimeOutData->xTicksRemaining );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Unsubscribe from accepted topic" );
    }

    if( pcRejectedTopic != NULL )
    {
        /* Fill the rejected topic. */
        xUnsubscribeParams.usTopicLength = prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                                           shadowTOPIC_BUFFER_LENGTH,
                                                           pcRejectedTopic,
                                                           pcThingName );

        xMQTTReturn = MQTT_AGENT_Unsubscribe( pxShadowClient->xMQTTClient,
                                              &xUnsubscribeParams,
                                              pxTimeOutData->xTicksRemaining );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Unsubscribe from rejected topic" );
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvShadowMQTTCallback( void * pvUserData,
                                         const MQTTAgentCallbackParams_t * const pxCallbackParams )
{
    BaseType_t xOperationMatched = pdFALSE;
    ShadowClient_t * pxShadowClient;
    const MQTTPublishData_t * pxPublishData;
    ShadowOperationName_t xOperationName;
    ShadowReturnCode_t xResult;
    const CallbackCatalogEntry_t * pxCallbackCatalogEntry;
    BaseType_t xReturn = pdFALSE;
    BaseType_t xCompareLen;
    BaseType_t xShadowClientID;


    xShadowClientID = *( ( BaseType_t * ) pvUserData ); /*lint !e9087 Safe cast from pointer handle. */
    pxShadowClient = &( xShadowClients[ xShadowClientID ] );

    if( pxCallbackParams->xMQTTEvent == eMQTTAgentPublish )
    {
        pxPublishData = ( &( pxCallbackParams->u.xPublishData ) );

        /* If xOperationMutex is locked, the client is waiting on the acceptance
         * or rejection of a publish. Publish results take priority over user notify
         * callbacks. This also means that the client will not be notified of gets or
         * deletes performed by itself in a user notify callback. However, the client
         * will still be notified of updates performed by itself if it has registered
         * a callback for /update/documents or update/delta. */

        if( xSemaphoreTake( pxShadowClient->xOperationDataMutex,
                            portMAX_DELAY ) == pdPASS )
        {
            if( pxShadowClient->pxOperationData != NULL )
            {
                /* Verify Thing Name and operation by comparing the received topic with
                 * the current operation's topic. */

                xCompareLen = ( BaseType_t ) configMIN( ( BaseType_t ) strlen( ( const char * ) pxShadowClient->ucTopicBuffer ),
                                                        ( BaseType_t ) pxPublishData->usTopicLength );

                if( strncmp( ( const char * ) pxPublishData->pucTopic,
                             ( const char * ) pxShadowClient->ucTopicBuffer,
                             ( size_t ) xCompareLen ) == 0 )
                {
                    /* Parse the in-progress operation and result. */
                    xOperationName = ( pxShadowClient->pxOperationData )->xOperationInProgress;
                    xResult = prvParseShadowOperationStatus( pxPublishData->pucTopic,
                                                             pxPublishData->usTopicLength );

                    /* Both an operation and result were identified, and both match
                     * the operation this Shadow Client is waiting on; call the
                     * operation-specific callback. */
                    if( ( xResult != eShadowUnknown ) && ( xOperationName != eShadowOperationOther ) )
                    {
                        xOperationMatched = pdTRUE;

                        switch( xOperationName )
                        {
                            case eShadowOperationUpdate:
                                prvShadowUpdateCallback( xShadowClientID,
                                                         xResult,
                                                         ( pxShadowClient->pxOperationData )->pxOperationParams,
                                                         ( const char * ) pxPublishData->pvData,
                                                         pxPublishData->ulDataLength );
                                break;

                            case eShadowOperationGet:
                                prvShadowGetCallback( xShadowClientID,
                                                      xResult,
                                                      ( pxShadowClient->pxOperationData )->pxOperationParams,
                                                      ( const char * ) pxPublishData->pvData,
                                                      pxPublishData->ulDataLength,
                                                      pxPublishData->xBuffer );

                                /* Only take an MQTT buffer if the Get operation succeeded. */
                                if( xResult == eShadowSuccess )
                                {
                                    xReturn = pdTRUE;
                                }

                                break;

                            case eShadowOperationDelete:
                                prvShadowDeleteCallback( xShadowClientID,
                                                         xResult,
                                                         ( const char * ) pxPublishData->pvData,
                                                         pxPublishData->ulDataLength );
                                break;

                            default:
                                /* Should not fall here. */
                                break;
                        }
                    }
                }
            }

            configASSERT( xSemaphoreGive( pxShadowClient->xOperationDataMutex ) == pdPASS );
        }

        /* If the received topic doesn't match the current operation, it's
         * still possible for it to match a registered callback. */
        if( xOperationMatched == pdFALSE )
        {
            pxCallbackCatalogEntry = prvMatchCallbackTopic( pxShadowClient,
                                                            pxPublishData->pucTopic, pxPublishData->usTopicLength,
                                                            &xOperationName );

            if( pxCallbackCatalogEntry != NULL )
            {
                switch( xOperationName )
                {
                    case eShadowOperationUpdateDocuments:

                        if( pxCallbackCatalogEntry->xCallbackInfo.xShadowUpdatedCallback != NULL )
                        {
                            xReturn = pxCallbackCatalogEntry->xCallbackInfo.xShadowUpdatedCallback( pvUserData,
                                                                                                    pxCallbackCatalogEntry->xCallbackInfo.pcThingName,
                                                                                                    ( const char * ) pxPublishData->pvData,
                                                                                                    pxPublishData->ulDataLength,
                                                                                                    pxPublishData->xBuffer );
                        }

                        break;

                    case eShadowOperationUpdateDelta:

                        if( pxCallbackCatalogEntry->xCallbackInfo.xShadowDeltaCallback != NULL )
                        {
                            xReturn = pxCallbackCatalogEntry->xCallbackInfo.xShadowDeltaCallback( pvUserData,
                                                                                                  pxCallbackCatalogEntry->xCallbackInfo.pcThingName,
                                                                                                  ( const char * ) pxPublishData->pvData,
                                                                                                  pxPublishData->ulDataLength,
                                                                                                  pxPublishData->xBuffer );
                        }

                        break;

                    case eShadowOperationDeletedByAnother:

                        if( pxCallbackCatalogEntry->xCallbackInfo.xShadowDeletedCallback != NULL )
                        {
                            pxCallbackCatalogEntry->xCallbackInfo.xShadowDeletedCallback( pvUserData,
                                                                                          pxCallbackCatalogEntry->xCallbackInfo.pcThingName );
                        }

                        break;

                    default:
                        /* Should not fall here. */
                        break;
                }
            }
        }
    }
    /* The Shadow Client assumes all subscriptions are lost on disconnect. */
    else
    {
        if( pxCallbackParams->xMQTTEvent == eMQTTAgentDisconnect )
        {
            Shadow_debug_printf( ( "[Shadow %d] Warning: got an MQTT disconnect"
                                   " message.\r\n", xShadowClientID ) );

            prvSetSubscribedFlag( pxShadowClient, eShadowOperationUpdate, 0 );
            prvSetSubscribedFlag( pxShadowClient, eShadowOperationGet, 0 );
            prvSetSubscribedFlag( pxShadowClient, eShadowOperationDelete, 0 );

            /*_RB_ TODO below. */
            /* TODO: resubscribe to all callback topics. */
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvParseShadowOperationStatus( const uint8_t * const
                                                         pucTopic,
                                                         uint16_t usTopicLength )
{
    ShadowReturnCode_t xResult = eShadowUnknown;
    const uint8_t * pucTopicStatus;
    size_t xCompareLength;

    xCompareLength = strlen( shadowTOPIC_SUFFIX_ACCEPTED );
    pucTopicStatus = pucTopic + usTopicLength - xCompareLength;

    if( strncmp( ( const char * ) pucTopicStatus,
                 shadowTOPIC_SUFFIX_ACCEPTED,
                 xCompareLength ) == 0 )
    {
        xResult = eShadowSuccess;
    }
    else
    {
        xCompareLength = strlen( shadowTOPIC_SUFFIX_REJECTED );
        pucTopicStatus = pucTopic + usTopicLength - xCompareLength;

        if( strncmp( ( const char * ) pucTopicStatus,
                     shadowTOPIC_SUFFIX_REJECTED,
                     xCompareLength ) == 0 )
        {
            xResult = eShadowFailure;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static const CallbackCatalogEntry_t * prvMatchCallbackTopic( const ShadowClient_t * const pxShadowClient,
                                                             const uint8_t * const pucTopic,
                                                             uint16_t usTopicLength,
                                                             ShadowOperationName_t * const pxOperationName )
{
    const CallbackCatalogEntry_t * pxReturn = NULL;
    BaseType_t xIterator, xTopicFound = pdFALSE;
    uint8_t ucTopicBuffer[ shadowTOPIC_BUFFER_LENGTH ];
    size_t xCompareLength;

    for( xIterator = 0; xIterator < shadowconfigMAX_THINGS_WITH_CALLBACKS; xIterator++ )
    {
        pxReturn = &( pxShadowClient->xCallbackCatalog[ xIterator ] );

        if( pxReturn->xInUse == pdTRUE )
        {
            xCompareLength = prvCreateTopic( ( char * ) ucTopicBuffer,
                                             shadowTOPIC_BUFFER_LENGTH,
                                             shadowTOPIC_PREFIX, pxReturn->xCallbackInfo.pcThingName );

            if( strncmp( ( const char * ) ucTopicBuffer,
                         ( const char * ) pucTopic,
                         xCompareLength ) == 0 )
            {
                xTopicFound = pdTRUE;

                xCompareLength = prvCreateTopic( ( char * ) ucTopicBuffer,
                                                 shadowTOPIC_BUFFER_LENGTH, shadowTOPIC_UPDATE_DOCUMENTS,
                                                 pxReturn->xCallbackInfo.pcThingName );

                xCompareLength = configMAX( xCompareLength, usTopicLength );

                if( pxOperationName != NULL )
                {
                    if( strncmp( ( const char * ) ucTopicBuffer,
                                 ( const char * ) pucTopic,
                                 xCompareLength ) == 0 )
                    {
                        *pxOperationName = eShadowOperationUpdateDocuments;
                    }
                    else
                    {
                        xCompareLength = prvCreateTopic( ( char * ) ucTopicBuffer,
                                                         shadowTOPIC_BUFFER_LENGTH, shadowTOPIC_UPDATE_DELTA,
                                                         pxReturn->xCallbackInfo.pcThingName );

                        xCompareLength = configMAX( xCompareLength, usTopicLength );

                        if( strncmp( ( const char * ) ucTopicBuffer,
                                     ( const char * ) pucTopic,
                                     xCompareLength ) == 0 )
                        {
                            *pxOperationName = eShadowOperationUpdateDelta;
                        }
                        else
                        {
                            xCompareLength = prvCreateTopic( ( char * ) ucTopicBuffer,
                                                             shadowTOPIC_BUFFER_LENGTH,
                                                             shadowTOPIC_DELETE_ACCEPTED,
                                                             pxReturn->xCallbackInfo.pcThingName );

                            xCompareLength = configMAX( xCompareLength, usTopicLength );

                            if( strncmp( ( const char * ) ucTopicBuffer,
                                         ( const char * ) pucTopic,
                                         xCompareLength ) == 0 )
                            {
                                *pxOperationName = eShadowOperationDeletedByAnother;
                            }
                            else
                            {
                                *pxOperationName = eShadowOperationOther;
                            }
                        }
                    }
                }

                break;
            }
        }
    }

    if( xTopicFound == pdFALSE )
    {
        pxReturn = NULL;
    }

    return pxReturn;
}

/*-----------------------------------------------------------*/

static void prvShadowUpdateCallback( BaseType_t xShadowClientID,
                                     ShadowReturnCode_t xResult,
                                     const ShadowOperationParams_t * const pxParams,
                                     const char * const pcData,
                                     uint32_t ulDataLength )
{
    /* Remove warning about unused parameters. */
    ( void ) pxParams;

    ShadowClient_t * pxShadowClient;

    BaseType_t xReturn;

    pxShadowClient = &( xShadowClients[ xShadowClientID ] );

    /* Verify client token match. */
    xReturn = SHADOW_JSONDocClientTokenMatch( pxParams->pcData,
                                              pxParams->ulDataLength,
                                              pcData,
                                              ulDataLength );

    if( xReturn == pdPASS )
    {
        pxShadowClient->xOperationResult = xResult;

        /* For failures, get the code and message. */
        if( xResult == eShadowFailure )
        {
            pxShadowClient->xOperationResult = prvGetErrorCodeAndMessage( pcData,
                                                                          ulDataLength,
                                                                          xShadowClientID,
                                                                          shadowTOPIC_OPERATION_UPDATE );
        }

        configASSERT( xSemaphoreGive( pxShadowClient->xCallbackSemaphore ) == pdPASS );
    }
}

/*-----------------------------------------------------------*/

static void prvShadowGetCallback( BaseType_t xShadowClientID,
                                  ShadowReturnCode_t xResult,
                                  ShadowOperationParams_t * const pxParams,
                                  const char * const pcData,
                                  uint32_t ulDataLength,
                                  MQTTBufferHandle_t xBuffer )
{
    ShadowClient_t * pxShadowClient;

    pxShadowClient = &( xShadowClients[ xShadowClientID ] );
    pxShadowClient->xOperationResult = xResult;

/* For successes, fill the user's buffer with the Shadow document. */
    if( xResult == eShadowSuccess )
    {
        pxParams->pcData = pcData;
        pxParams->ulDataLength = ulDataLength;
        pxParams->xBuffer = xBuffer;
    }
/* For failures , get the code and message. */
    else
    {
        pxShadowClient->xOperationResult = prvGetErrorCodeAndMessage( pcData,
                                                                      ulDataLength,
                                                                      xShadowClientID,
                                                                      shadowTOPIC_OPERATION_GET );
        pxParams->pcData = NULL;
        pxParams->ulDataLength = 0;
    }

    configASSERT( xSemaphoreGive( pxShadowClient->xCallbackSemaphore ) == pdPASS );
}
/*-----------------------------------------------------------*/

static void prvShadowDeleteCallback( BaseType_t xShadowClientID,
                                     ShadowReturnCode_t xResult,
                                     const char * const pcData,
                                     uint32_t ulDataLength )
{
    ShadowClient_t * pxShadowClient;

    pxShadowClient = &( xShadowClients[ xShadowClientID ] );
    pxShadowClient->xOperationResult = xResult;

    if( xResult == eShadowFailure )
    {
        pxShadowClient->xOperationResult = prvGetErrorCodeAndMessage( pcData,
                                                                      ulDataLength,
                                                                      xShadowClientID,
                                                                      shadowTOPIC_OPERATION_DELETE );
    }

    configASSERT( xSemaphoreGive( pxShadowClient->xCallbackSemaphore ) == pdPASS );
}
/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvGetErrorCodeAndMessage( const char * const pcData,
                                                     uint32_t ulDataLength,
                                                     BaseType_t xShadowClientID,
                                                     const char * const pcOperationName )
{
    ShadowReturnCode_t xErrorCode;
    char * pcErrorMessage;
    uint16_t usErrorMessageLength;

    xErrorCode = ( ShadowReturnCode_t ) SHADOW_JSONGetErrorCodeAndMessage( pcData,
                                                                           ulDataLength,
                                                                           &pcErrorMessage,
                                                                           &usErrorMessageLength );

    if( xErrorCode > 0 )
    {
        Shadow_debug_printf( ( "[Shadow %d] %s rejected, code %d: %.*s.\r\n",
                               xShadowClientID, pcOperationName,
                               xErrorCode,
                               usErrorMessageLength,
                               pcErrorMessage ) );
    }
    else
    {
        Shadow_debug_printf( ( "[Shadow %d] JSON parse error while parsing"
                               " error code and message.\r\n", xShadowClientID ) );
    }

    /* Remove compiler warnings in the case that Shadow_debug_printf() is not
     * defined. */
    ( void ) pcOperationName;
    ( void ) xShadowClientID;

    /* SHADOW_JSONGetErrorCodeAndMessage may return 0 for bad pointer arguments.
     * Convert this to a JSON parse error, as 0 is eShadowSuccess. */
    if( xErrorCode == 0 )
    {
        xErrorCode = eShadowJSMNInval;
    }

    return xErrorCode;
}

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvShadowOperation( ShadowOperationCallParams_t * pxParams )
{
    ShadowReturnCode_t xReturn = eShadowFailure;
    MQTTAgentPublishParams_t xPublishParams;
    ShadowClient_t * pxShadowClient;
    TimeOutData_t xTimeOutData;
    ShadowOperationData_t xOperationData;
    MQTTAgentReturnCode_t xMQTTReturn;

    /* Initialize timeout data. */
    xTimeOutData.xTicksRemaining = pxParams->xTimeoutTicks;

    /* Identify the relevant Shadow Client, then lock that client's operation mutex.
     * This allows only one operation to be in progress. */
    pxShadowClient = &( xShadowClients[ ( pxParams->xShadowClientID ) ] );

    if( xSemaphoreTake( pxShadowClient->xOperationMutex,
                        xTimeOutData.xTicksRemaining ) == pdPASS )
    {
        /* Subscribe to accepted/rejected if necessary. */
        if( ( BaseType_t ) prvGetSubscribedFlag( pxShadowClient,
                                                 pxParams->xOperationName ) == pdFALSE )
        {
            xReturn = prvShadowSubscribeToAcceptedRejected( pxParams->xShadowClientID,
                                                            ( pxParams->pxOperationParams )->pcThingName,
                                                            pxParams->pcOperationAcceptedTopic,
                                                            pxParams->pcOperationRejectedTopic,
                                                            &xTimeOutData );
        }
        else
        {
            xReturn = eShadowSuccess;
        }

        if( xReturn == eShadowSuccess )
        {
            /* The subscribe to update/accepted and update/rejected succeeded,
             * so set the appropriate flag. */
            prvSetSubscribedFlag( pxShadowClient, pxParams->xOperationName, 1 );

            /* This is guarded by xOperationMutex and should never fail. */
            configASSERT( xSemaphoreTake( pxShadowClient->xCallbackSemaphore,
                                          xTimeOutData.xTicksRemaining ) == pdPASS );

            /* Fill ucTopicBuffer with the update topic. */
            xPublishParams.usTopicLength =
                prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                shadowTOPIC_BUFFER_LENGTH,
                                pxParams->pcOperationTopic,
                                ( pxParams->pxOperationParams )->pcThingName );

            /* Operation parameters. */
            xPublishParams.pucTopic = pxShadowClient->ucTopicBuffer;
            xPublishParams.pvData = pxParams->pcPublishMessage;
            xPublishParams.ulDataLength = pxParams->ulPublishMessageLength;
            xPublishParams.xQoS = ( pxParams->pxOperationParams )->xQoS;

            /* Data to pass to the callback. */
            xOperationData.xOperationInProgress = pxParams->xOperationName;
            xOperationData.pxOperationParams = pxParams->pxOperationParams;
            pxShadowClient->pxOperationData = &xOperationData;

            xMQTTReturn = MQTT_AGENT_Publish( pxShadowClient->xMQTTClient,
                                              &xPublishParams,
                                              xTimeOutData.xTicksRemaining );

            /* Publish to operation topic. */
            xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                                ( ShadowClientHandle_t ) ( pxParams->xShadowClientID ), /*lint !e923 Safe cast from pointer handle. */
                                                "Publish to operation topic" );

            if( xReturn == eShadowSuccess )
            {
                /* Wait for the semaphore to become available again; it should be
                 * released by the update callback. */
                if( xSemaphoreTake( pxShadowClient->xCallbackSemaphore,
                                    xTimeOutData.xTicksRemaining ) != pdPASS )
                {
                    Shadow_debug_printf( ( "[Shadow %d] Timeout waiting on"
                                           " %s accepted/rejected.\r\n",
                                           pxParams->xShadowClientID,
                                           pxParams->pcOperationName ) );
                    xReturn = eShadowTimeout;
                }
                else
                {
                    /* The update callback reports its status as xOperationResult. */
                    xReturn = pxShadowClient->xOperationResult;
                }
            }

            configASSERT( xSemaphoreGive( pxShadowClient->xCallbackSemaphore ) == pdPASS );
        }

        /* Unsubscribe. */
        if( ( pxParams->pxOperationParams )->ucKeepSubscriptions == ( uint8_t ) 0 )
        {
            xTimeOutData.xTicksRemaining = configMAX( xTimeOutData.xTicksRemaining,
                                                      pdMS_TO_TICKS( shadowconfigCLEANUP_TIME_MS ) );

            /* If the Shadow client is subscribed to delete/accepted for this
             * Thing for a user notify callback, do not unsubscribe; that would
             * break callback notify. */
            if( pxParams->xOperationName == eShadowOperationDelete )
            {
                ( void ) prvCreateTopic( ( char * ) pxShadowClient->ucTopicBuffer,
                                         shadowTOPIC_BUFFER_LENGTH,
                                         shadowTOPIC_DELETE_ACCEPTED,
                                         pxParams->pxOperationParams->pcThingName );

                /* If there's a callback registered for delete/accepted, only
                 * unsubscribe from delete/rejected. */
                if( prvMatchCallbackTopic( pxShadowClient,
                                           pxShadowClient->ucTopicBuffer,
                                           ( uint16_t )
                                           strlen( ( const char * ) pxShadowClient->ucTopicBuffer ),
                                           NULL ) == NULL )
                {
                    if( prvShadowUnsubscribeFromAcceptedRejected( pxParams->xShadowClientID,
                                                                  pxParams->pxOperationParams->pcThingName,
                                                                  NULL,
                                                                  pxParams->pcOperationRejectedTopic,
                                                                  &xTimeOutData ) == eShadowSuccess )
                    {
                        prvSetSubscribedFlag( pxShadowClient,
                                              pxParams->xOperationName,
                                              0 );
                    }
                }
            }
            else
            {
                if( prvShadowUnsubscribeFromAcceptedRejected( pxParams->xShadowClientID,
                                                              pxParams->pxOperationParams->pcThingName,
                                                              pxParams->pcOperationAcceptedTopic,
                                                              pxParams->pcOperationRejectedTopic,
                                                              &xTimeOutData ) == eShadowSuccess )
                {
                    prvSetSubscribedFlag( pxShadowClient,
                                          pxParams->xOperationName,
                                          0 );
                }
            }
        }

        /* Delete this operation's data so that the next operation has a clean Shadow
         * Client, then release the operation mutex. */
        if( xSemaphoreTake( pxShadowClient->xOperationDataMutex,
                            portMAX_DELAY ) == pdPASS )
        {
            pxShadowClient->pxOperationData = NULL;
            configASSERT( xSemaphoreGive( pxShadowClient->xOperationDataMutex ) == pdPASS );
        }
        else
        {
            Shadow_debug_printf( ( "Error while taking mutex\n" ) );
            configASSERT( 0 );
        }

        pxShadowClient->xOperationResult = eShadowSuccess;
        memset( pxShadowClient->ucTopicBuffer, 0, shadowTOPIC_BUFFER_LENGTH );
        configASSERT( xSemaphoreGive( pxShadowClient->xOperationMutex )
                      == pdPASS );
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

static void prvSetSubscribedFlag( ShadowClient_t * const pxShadowClient,
                                  ShadowOperationName_t xOperationName,
                                  BaseType_t xValue )
{
    switch( xOperationName )
    {
        case eShadowOperationUpdate:
            pxShadowClient->xUpdateSubscribed = xValue;
            break;

        case eShadowOperationGet:
            pxShadowClient->xGetSubscribed = xValue;
            break;

        case eShadowOperationDelete:
            pxShadowClient->xDeleteSubscribed = xValue;
            break;

        default:
            /* Should not fall here. */
            break;
    }
}

/*-----------------------------------------------------------*/

static uint8_t prvGetSubscribedFlag( const ShadowClient_t * const pxShadowClient,
                                     ShadowOperationName_t xOperationName )
{
    uint8_t ucReturn = 0;

    switch( xOperationName )
    {
        case eShadowOperationUpdate:
            ucReturn = ( uint8_t ) pxShadowClient->xUpdateSubscribed;
            break;

        case eShadowOperationGet:
            ucReturn = ( uint8_t ) pxShadowClient->xGetSubscribed;
            break;

        case eShadowOperationDelete:
            ucReturn = ( uint8_t ) pxShadowClient->xDeleteSubscribed;
            break;

        default:
            /* Should not fall here. */
            break;
    }

    return ucReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientCreate( ShadowClientHandle_t * pxShadowClientHandle,
                                        const ShadowCreateParams_t * const pxShadowCreateParams )
{
    ShadowClient_t * pxShadowClient;
    BaseType_t xShadowClientID;
    ShadowReturnCode_t xReturn = eShadowFailure;
    MQTTAgentReturnCode_t xMQTTReturn;

    configASSERT( ( pxShadowClientHandle != NULL ) );
    configASSERT( ( pxShadowCreateParams != NULL ) );

    /* For now, only dedicated MQTT clients are supported. Remove this assert
     * once support for shared clients is added. */
    configASSERT( ( pxShadowCreateParams->xMQTTClientType == eDedicatedMQTTClient ) );

    xShadowClientID = prvGetFreeShadowClient();
    configASSERT( xShadowClientID >= 0 );

    if( xShadowClientID >= 0 )
    {
        pxShadowClient = &( xShadowClients[ xShadowClientID ] );

        xMQTTReturn = MQTT_AGENT_Create( &( pxShadowClient->xMQTTClient ) );

        xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                            ( ShadowClientHandle_t ) xShadowClientID, /*lint !e923 Safe cast from pointer handle. */
                                            "Creation of dedicated MQTT client" );

        if( xReturn == eShadowSuccess )
        {
            /* Create synchronization mechanisms; these calls should never fail. */
            pxShadowClient->xCallbackSemaphore = xSemaphoreCreateBinaryStatic( &( pxShadowClient->xCallbackSemaphoreBuffer ) );
            pxShadowClient->xOperationMutex = xSemaphoreCreateMutexStatic( &( pxShadowClient->xOperationMutexBuffer ) );
            pxShadowClient->xOperationDataMutex = xSemaphoreCreateMutexStatic( &( pxShadowClient->xOperationDataMutexBuffer ) );
            configASSERT( xSemaphoreGive( pxShadowClient->xCallbackSemaphore ) == pdPASS );

            /* Set the output parameter. */
            *pxShadowClientHandle = ( ShadowClientHandle_t ) xShadowClientID; /*lint !e923 Safe cast from pointer handle. */
        }
        else
        {
            Shadow_debug_printf( ( "[Shadow Init] Failed to create dedicated"
                                   "client; deleting partially-initialized client.\r\n" ) );

            /* Delete the partially-initialized client. */
            xReturn = SHADOW_ClientDelete( *( pxShadowClientHandle ) );
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientConnect( ShadowClientHandle_t xShadowClientHandle,
                                         MQTTAgentConnectParams_t * const pxConnectParams,
                                         TickType_t xTimeoutTicks )
{
    ShadowClient_t * pxShadowClient;
    ShadowReturnCode_t xReturn;
    MQTTAgentReturnCode_t xMQTTReturn;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) );                       /*lint !e923 Safe cast from pointer handle. */
    configASSERT( xShadowClientHandle == *( ( ShadowClientHandle_t * ) ( pxConnectParams->pvUserData ) ) ); /*lint !e9087 Safe cast from opaque context. */

    pxShadowClient = &( xShadowClients[ ( BaseType_t ) xShadowClientHandle ] );                             /*lint !e923 Safe cast from pointer handle. */
    configASSERT( ( pxShadowClient->xInUse == pdTRUE ) );

    pxConnectParams->pxCallback = prvShadowMQTTCallback;

    xMQTTReturn = MQTT_AGENT_Connect( pxShadowClient->xMQTTClient,
                                      pxConnectParams,
                                      xTimeoutTicks );

    xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                        xShadowClientHandle,
                                        "Connect" );

    pxConnectParams->pxCallback = NULL;

    return xReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientDisconnect( ShadowClientHandle_t xShadowClientHandle )
{
    ShadowClient_t * pxShadowClient;
    MQTTAgentReturnCode_t xMQTTReturn;
    TickType_t xTimeoutTicks;
    ShadowReturnCode_t xReturn;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    pxShadowClient = &( xShadowClients[ ( BaseType_t ) xShadowClientHandle ] );       /*lint !e923 Safe cast from pointer handle. */
    configASSERT( ( pxShadowClient->xInUse == pdTRUE ) );

    xTimeoutTicks = pdMS_TO_TICKS( shadowconfigCLEANUP_TIME_MS );

    xMQTTReturn = MQTT_AGENT_Disconnect( pxShadowClient->xMQTTClient,
                                         xTimeoutTicks );

    xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                        xShadowClientHandle,
                                        "Disconnect" );

    return xReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientDelete( ShadowClientHandle_t xShadowClientHandle )
{
    ShadowClient_t * pxShadowClient;
    ShadowReturnCode_t xReturn = eShadowFailure;
    MQTTAgentReturnCode_t xMQTTReturn;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    pxShadowClient = &( xShadowClients[ ( BaseType_t ) xShadowClientHandle ] );       /*lint !e923 Safe cast from pointer handle. */
    configASSERT( ( pxShadowClient->xInUse == pdTRUE ) );

    xMQTTReturn = MQTT_AGENT_Delete( pxShadowClient->xMQTTClient );

    xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                        xShadowClientHandle,
                                        "MQTT Client delete" );

    if( xReturn == eShadowSuccess )
    {
        taskENTER_CRITICAL();
        memset( pxShadowClient, 0, sizeof( ShadowClient_t ) );
        taskEXIT_CRITICAL();
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Update( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxUpdateParams,
                                  TickType_t xTimeoutTicks )
{
    ShadowOperationCallParams_t xUpdateCallParams;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    configASSERT( ( pxUpdateParams != NULL ) );
    configASSERT( ( pxUpdateParams->pcThingName != NULL ) );
    configASSERT( ( pxUpdateParams->pcData != NULL ) );
    configASSERT( ( pxUpdateParams->xQoS == eMQTTQoS0 ||
                    pxUpdateParams->xQoS == eMQTTQoS1 ) );

    xUpdateCallParams.xShadowClientID = ( BaseType_t ) xShadowClientHandle; /*lint !e923 Safe cast from pointer handle. */
    xUpdateCallParams.xOperationName = eShadowOperationUpdate;

    xUpdateCallParams.pcOperationName = shadowTOPIC_OPERATION_UPDATE;

    xUpdateCallParams.pcOperationTopic = shadowTOPIC_UPDATE;
    xUpdateCallParams.pcOperationAcceptedTopic = shadowTOPIC_UPDATE_ACCEPTED;
    xUpdateCallParams.pcOperationRejectedTopic = shadowTOPIC_UPDATE_REJECTED;

    xUpdateCallParams.pcPublishMessage = pxUpdateParams->pcData;
    xUpdateCallParams.ulPublishMessageLength = pxUpdateParams->ulDataLength;
    xUpdateCallParams.pxOperationParams = ( ShadowOperationParams_t * ) pxUpdateParams;
    xUpdateCallParams.xTimeoutTicks = xTimeoutTicks;

    return prvShadowOperation( &xUpdateCallParams );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Get( ShadowClientHandle_t xShadowClientHandle,
                               ShadowOperationParams_t * const pxGetParams,
                               TickType_t xTimeoutTicks )
{
    ShadowOperationCallParams_t xGetCallParams;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    configASSERT( ( pxGetParams != NULL ) );
    configASSERT( ( pxGetParams->pcThingName != NULL ) );
    configASSERT( ( pxGetParams->xQoS == eMQTTQoS0 ||
                    pxGetParams->xQoS == eMQTTQoS1 ) );

    xGetCallParams.xShadowClientID = ( BaseType_t ) xShadowClientHandle; /*lint !e923 Safe cast from pointer handle. */
    xGetCallParams.xOperationName = eShadowOperationGet;

    xGetCallParams.pcOperationName = shadowTOPIC_OPERATION_GET;

    xGetCallParams.pcOperationTopic = shadowTOPIC_GET;
    xGetCallParams.pcOperationAcceptedTopic = shadowTOPIC_GET_ACCEPTED;
    xGetCallParams.pcOperationRejectedTopic = shadowTOPIC_GET_REJECTED;

    xGetCallParams.pcPublishMessage = "";
    xGetCallParams.ulPublishMessageLength = 0;
    xGetCallParams.pxOperationParams = pxGetParams;
    xGetCallParams.xTimeoutTicks = xTimeoutTicks;

    return prvShadowOperation( &xGetCallParams );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Delete( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxDeleteParams,
                                  TickType_t xTimeoutTicks )
{
    ShadowOperationCallParams_t xDeleteCallParams;
    ShadowReturnCode_t xStatus;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    configASSERT( ( pxDeleteParams != NULL ) );
    configASSERT( ( pxDeleteParams->pcThingName != NULL ) );
    configASSERT( ( pxDeleteParams->xQoS == eMQTTQoS0 ||
                    pxDeleteParams->xQoS == eMQTTQoS1 ) );

    xDeleteCallParams.xShadowClientID = ( BaseType_t ) xShadowClientHandle; /*lint !e923 Safe cast from pointer handle. */
    xDeleteCallParams.xOperationName = eShadowOperationDelete;

    xDeleteCallParams.pcOperationName = shadowTOPIC_OPERATION_DELETE;

    xDeleteCallParams.pcOperationTopic = shadowTOPIC_DELETE;
    xDeleteCallParams.pcOperationAcceptedTopic = shadowTOPIC_DELETE_ACCEPTED;
    xDeleteCallParams.pcOperationRejectedTopic = shadowTOPIC_DELETE_REJECTED;

    xDeleteCallParams.pcPublishMessage = "";
    xDeleteCallParams.ulPublishMessageLength = 0;
    xDeleteCallParams.pxOperationParams = ( ShadowOperationParams_t * ) pxDeleteParams;
    xDeleteCallParams.xTimeoutTicks = xTimeoutTicks;

    xStatus = prvShadowOperation( &xDeleteCallParams );

    return xStatus;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_RegisterCallbacks( ShadowClientHandle_t xShadowClientHandle,
                                             ShadowCallbackParams_t * const pxCallbackParams,
                                             TickType_t xTimeoutTicks )
{
    ShadowClient_t * pxShadowClient;
    CallbackCatalogEntry_t * pxCallbackCatalogEntry;
    ShadowReturnCode_t xReturn = eShadowFailure;
    BaseType_t xCallbackCatalogIndex;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    configASSERT( ( pxCallbackParams != NULL ) );
    configASSERT( ( pxCallbackParams->pcThingName != NULL ) );

    pxShadowClient = &( xShadowClients[ ( BaseType_t ) xShadowClientHandle ] ); /*lint !e923 Safe cast from pointer handle. */
    configASSERT( ( pxShadowClient->xInUse == pdTRUE ) );

    xCallbackCatalogIndex = prvGetCallbackCatalogEntry( pxShadowClient->xCallbackCatalog,
                                                        pxCallbackParams->pcThingName );
    configASSERT( xCallbackCatalogIndex >= 0 );

    /* Initialize timeout data. */

    pxCallbackCatalogEntry = &( pxShadowClient->xCallbackCatalog
                                [ xCallbackCatalogIndex ] );

    /*_RB_ Casting on these calls make the code unreadable.  Types need changing to remove the need for the casts. */
    /* ToDo: sub manager. */
    xReturn = prvRegisterCallback( ( BaseType_t ) xShadowClientHandle,                                                      /*lint !e923 Safe cast from pointer handle. */
                                   ( const void ** ) &( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowUpdatedCallback ), /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. No const is being cast away either.*/
                                   ( const void ** ) &( pxCallbackParams->xShadowUpdatedCallback ),                         /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. */
                                   pxCallbackParams->pcThingName,
                                   ( const uint8_t * ) shadowTOPIC_UPDATE_DOCUMENTS,
                                   xTimeoutTicks );

    if( xReturn == eShadowSuccess )
    {
        xReturn = prvRegisterCallback( ( BaseType_t ) xShadowClientHandle,                                                      /*lint !e923 Safe cast from pointer handle. */
                                       ( const void ** ) &( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowDeletedCallback ), /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. No const is being cast away either.*/
                                       ( const void ** ) &( pxCallbackParams->xShadowDeletedCallback ),                         /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. */
                                       pxCallbackParams->pcThingName,
                                       ( const uint8_t * ) shadowTOPIC_DELETE_ACCEPTED,
                                       xTimeoutTicks );
    }

    if( xReturn == eShadowSuccess )
    {
        xReturn = prvRegisterCallback( ( BaseType_t ) xShadowClientHandle,                                                    /*lint !e923 Safe cast from pointer handle. */
                                       ( const void ** ) &( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowDeltaCallback ), /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. No const is being cast away either.*/
                                       ( const void ** ) &( pxCallbackParams->xShadowDeltaCallback ),                         /*lint !e9087 !e9005 cast is opaque and recast correctly inside the function. */
                                       pxCallbackParams->pcThingName,
                                       ( const uint8_t * ) shadowTOPIC_UPDATE_DELTA,
                                       xTimeoutTicks );
    }

    if( ( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowUpdatedCallback == NULL ) &&
        ( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowDeltaCallback == NULL ) &&
        ( ( pxCallbackCatalogEntry->xCallbackInfo ).xShadowDeletedCallback == NULL ) )
    {
        taskENTER_CRITICAL();
        {
            memset( pxCallbackCatalogEntry,
                    0,
                    sizeof( CallbackCatalogEntry_t ) );
        }
        taskEXIT_CRITICAL();
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ReturnMQTTBuffer( ShadowClientHandle_t xShadowClientHandle,
                                            MQTTBufferHandle_t xBufferHandle )
{
    ShadowClient_t * pxShadowClient;
    MQTTAgentReturnCode_t xMQTTReturn;
    ShadowReturnCode_t xReturn = eShadowFailure;

    configASSERT( ( ( BaseType_t ) xShadowClientHandle >= 0 &&
                    ( BaseType_t ) xShadowClientHandle < shadowconfigMAX_CLIENTS ) ); /*lint !e923 Safe cast from pointer handle. */

    pxShadowClient = &( xShadowClients[ ( BaseType_t ) xShadowClientHandle ] );       /*lint !e923 Safe cast from pointer handle. */
    configASSERT( ( pxShadowClient->xInUse == pdTRUE ) );

    xMQTTReturn = MQTT_AGENT_ReturnBuffer( pxShadowClient->xMQTTClient,
                                           xBufferHandle );

    xReturn = prvConvertMQTTReturnCode( xMQTTReturn,
                                        xShadowClientHandle,
                                        "Return MQTT buffer" );

    return xReturn;
}
