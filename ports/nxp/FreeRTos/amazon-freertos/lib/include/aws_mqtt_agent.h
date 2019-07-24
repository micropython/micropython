/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2018 NXP
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
 * @file aws_mqtt_agent.h
 * @brief MQTT Agent Interface.
 */

#ifndef _AWS_MQTT_AGENT_H_
#define _AWS_MQTT_AGENT_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT lib includes. */
#include "aws_mqtt_lib.h"

/* Library initialization definition include */
#include "aws_lib_init.h"

/**
 * @brief Opaque handle to represent an MQTT client.
 *
 * The MQTT library is capable of creating multiple MQTT clients, maximum number of which
 * is controlled by mqttconfigMAX_BROKERS macro. Each client is identified by an opaque
 * handle which is returned by the MQTT_AGENT_Create API call and later used in all
 * the subsequent API calls.
 */
typedef void * MQTTAgentHandle_t;

/**
 * @brief Return codes.
 *
 * Each API returns a value of this type.
 */
typedef enum
{
    eMQTTAgentSuccess,              /**< The operation was successful. */
    eMQTTAgentFailure,              /**< The operation failed. */
    eMQTTAgentTimeout,              /**< The operation timed out. */
    eMQTTAgentAPICalledFromCallback /**< The MQTT agent APIs must not be called from MQTT callbacks as callbacks run
                                     *   in the context of MQTT agent task and therefore can result in deadlock. This
                                     *   error code is returned if any MQTT agent API is invoked from any callback. */
} MQTTAgentReturnCode_t;

/**
 * @brief Various events reported by the library in the callback.
 *
 * The user can register an optional callback with the MQTT library to
 * get notified of various events including Publish messages received
 * from the broker. This enum identifies the event received in the
 * callback.
 */
typedef enum
{
    eMQTTAgentPublish,   /**< A Publish message was received from the broker. */
    eMQTTAgentDisconnect /**< The connection to the broker got disconnected. */
} MQTTAgentEvent_t;

/**
 * @brief Passed by the library in the callback to inform the user of various events.
 *
 * If the user has registered a callback to get notified of various events, a pointer
 * to this structure is passed in the callback function.
 * @see MQTTAgentEvent_t.
 */
typedef struct MQTTAgentCallbackParams
{
    MQTTAgentEvent_t xMQTTEvent; /**< Type of the event received. */
    /* This union is here for future support. */
    union
    {
        MQTTPublishData_t xPublishData; /**< Publish data. Meaningful only in case of eMQTTAgentPublish event. */
    } u;
} MQTTAgentCallbackParams_t;

/**
 * @brief Signature of the callback registered by the user to get notified of various events.
 *
 * The user can register an optional callback to get notified of various events.
 *
 * @param[in] pvUserData The user data as provided in the connect parameters while connecting.
 * @param[in] pxCallbackParams The event and related data.
 *
 * @return The return value is ignored in all other cases except publish (i.e. eMQTTAgentPublish
 * event):
 * 1. If pdTRUE is returned - The ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) lies with the user.
 * 2. If pdFALSE is returned - The ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) remains with the library and it is recycled as soon as
 * the callback returns.<br>
 * The user should take the ownership of the buffer containing the received message from the
 * broker by returning pdTRUE from the callback if the user wants to use the buffer after
 * the callback is over. The user should return the buffer whenever done by calling the
 * MQTT_AGENT_ReturnBuffer API.
 *
 * @see MQTTAgentCallbackParams_t.
 */
typedef BaseType_t ( * MQTTAgentCallback_t ) ( void * pvUserData,
                                               const MQTTAgentCallbackParams_t * const pxCallbackParams );

/**
* @brief Flags for the MQTT agent connect params.
*/
#define mqttagentURL_IS_IP_ADDRESS          0x00000001 /**< Set this bit in xFlags if the provided URL is an IP address. */
#define mqttagentREQUIRE_TLS                0x00000002 /**< Set this bit in xFlags to use TLS. */
#define mqttagentUSE_AWS_IOT_ALPN_443       0x00000004 /**< Set this bit in xFlags to use AWS IoT support for MQTT over TLS port 443. */

/**
 * @brief Parameters passed to the MQTT_AGENT_Connect API.
 */
typedef struct MQTTAgentConnectParams
{
    const char * pcURL;             /**< The URL of the MQTT broker to connect to. */
    BaseType_t xFlags;              /**< Flags to control the behavior of MQTT connect. */
    BaseType_t xURLIsIPAddress;     /**< Deprecated. Set the mqttagentURL_IS_IP_ADDRESS bit in xFlags instead. */
    uint16_t usPort;                /**< Port number at which MQTT broker is listening. This field is ignored if the mqttagentUSE_AWS_IOT_ALPN_443 flag is set. */
    const uint8_t * pucClientId;    /**< Client Identifier of the MQTT client. It should be unique per broker. */
    uint16_t usClientIdLength;      /**< The length of the client Id. */
    BaseType_t xSecuredConnection;  /**< Deprecated. Set the mqttagentREQUIRE_TLS bit in xFlags instead. */
    void * pvUserData;              /**< User data supplied back as it is in the callback. Can be NULL. */
    MQTTAgentCallback_t pxCallback; /**< Callback used to report various events. In addition to other events, this callback is invoked for the publish
                                     *   messages received on the topics for which the user has not registered any subscription callback. Can be NULL. */
    char * pcCertificate;           /**< Certificate used for secure connection. Can be NULL. If it is NULL, the one specified in the aws_credential_keys.h is used. */
    uint32_t ulCertificateSize;     /**< Size of certificate used for secure connection. */
    char * cUserName;               /**< UserName, From Application Layer, if used if during MQTT Connect */
    uint32_t uUsernamelength;       /**< Length of UserName */
    const char *p_password;         /**< Password, if used during MQTT Connect */
    uint32_t passwordlength;        /**< Length of Password */
} MQTTAgentConnectParams_t;

/**
 * @brief Parameters passed to the MQTT_AGENT_Subscribe API.
 */
typedef struct MQTTAgentSubscribeParams
{
    const uint8_t * pucTopic;                    /**< The topic to subscribe to. This can be a topic filter containing wild cards as permitted by the MQTT protocol. */
    uint16_t usTopicLength;                      /**< The length of the topic. */
    MQTTQoS_t xQoS;                              /**< Requested Quality of Service. */
    #if( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        void * pvPublishCallbackContext;         /**< Passed as it is in the publish callback. Can be NULL. */
        MQTTPublishCallback_t pxPublishCallback; /**< Callback function to be called whenever a publish message is received on this topic or on a topic which matches this
                                                  *   topic filter. If a publish message is received on a topic which matches more than one topic filters, the order in which
                                                  *   the callbacks are invoked is undefined. This can be NULL if the user does not want to register a topic specific callback,
                                                  *   in which case the generic callback ( if registered during connect ) is invoked. */
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
} MQTTAgentSubscribeParams_t;

/**
 * @brief Parameters passed to the MQTT_AGENT_Unsubscribe API.
 */
typedef struct MQTTAgentUnsubscribeParams
{
    const uint8_t * pucTopic; /**< The topic to unsubscribe from. */
    uint16_t usTopicLength;   /**< The length of the topic. */
} MQTTAgentUnsubscribeParams_t;

/**
 * @brief Parameters passed to the MQTT_AGENT_Publish API.
 */
typedef struct MQTTAgentPublishParams
{
    const uint8_t * pucTopic; /**< The topic string on which the message should be published. */
    uint16_t usTopicLength;   /**< The length of the topic. */
    MQTTQoS_t xQoS;           /**< Quality of Service (QoS). */
    const void * pvData;      /**< The data to publish. This data is copied into the MQTT buffers and therefore the user can free the buffer after the MQTT_AGENT_Publish call returns. */
    uint32_t ulDataLength;    /**< Length of the data. */
} MQTTAgentPublishParams_t;

/**
 * @brief MQTT library Init function.
 *
 * This function does general initialization and setup. It must be called once
 * and only once before calling any other function.
 *
 * @return pdPASS if everything succeeds, pdFAIL otherwise.
 */
lib_initDECLARE_LIB_INIT( MQTT_AGENT_Init );

/**
 * @brief Creates a new MQTT client.
 *
 * The MQTT library is capable of creating multiple MQTT clients, maximum number of which
 * is controlled by mqttconfigMAX_BROKERS macro. If mqttconfigMAX_BROKERS clients are already
 * in use, this function will fail immediately. Otherwise a new client is setup and the handle
 * to the created client is returned in the pxMQTTHandle parameter which should be used in all
 * the subsequent API calls. Note that the returned handled is only valid if the return value
 * of the API is eMQTTAgentSuccess.
 *
 * @param[out] pxMQTTHandle Output parameter to return the opaque client handle.
 *
 * @return eMQTTAgentSuccess if a new client is successfully created, otherwise an error code
 * explaining the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Create( MQTTAgentHandle_t * const pxMQTTHandle );

/**
 * @brief Deletes the already created MQTT client.
 *
 * This function just frees up the internal resources and does not disconnect. The user must
 * call MQTT_AGENT_Disconnect API to make sure that the client is disconnected before
 * deleting it.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 *
 * @return eMQTTAgentSuccess if the client is successfully deleted, otherwise an
 * error code explaining the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Delete( MQTTAgentHandle_t xMQTTHandle );

/**
 * @brief Establishes a connection with the MQTT broker.
 *
 * @note This function alters the calling task's notification state and value. If xTimeoutTicks
 * is short the calling task's notification state and value may be updated after MQTT_AGENT_Connect()
 * has returned.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] pxConnectParams Connect parameters.
 * @param[in] xTimeoutTicks Maximum time in ticks after which the operation should fail. Use pdMS_TO_TICKS
 * macro to convert milliseconds to ticks.
 *
 * @return eMQTTAgentSuccess if the connect operation succeeds, otherwise an error code explaining the
 * reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Connect( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentConnectParams_t * const pxConnectParams,
                                          TickType_t xTimeoutTicks );

/**
 * @brief Disconnects the connection with the MQTT broker.
 *
 * @note This function alters the calling task's notification state and value. If xTimeoutTicks
 * is short the calling task's notification state and value may be updated after MQTT_AGENT_Disconnect()
 * has returned.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] xTimeoutTicks Maximum time in ticks after which the operation should fail. Use pdMS_TO_TICKS
 * macro to convert milliseconds to ticks.
 *
 * @return eMQTTAgentSuccess if the disconnect operation succeeds, otherwise an error code explaining
 * the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Disconnect( MQTTAgentHandle_t xMQTTHandle,
                                             TickType_t xTimeoutTicks );

/**
 * @brief Subscribes to a given topic.
 *
 * @note This function alters the calling task's notification state and value. If xTimeoutTicks
 * is short the calling task's notification state and value may be updated after MQTT_AGENT_Subscribe()
 * has returned.
 *
 * Whenever a publish message is received on a topic, the registered callbacks are invoked
 * in the following order:
 * * If we have an exact matching entry in the subscription manager, the corresponding
 *   callback is invoked.
 * * Then the wild card topic filters are checked for match and the corresponding callbacks
 *   are invoked for the ones which match the topic.
 *
 * @note If a publish message is received on a topic which matches more than one topic
 * filters, the order in which the registered callbacks are invoked is undefined.
 *
 * @warning If the user takes the ownership of the MQTT buffer by returning eMQTTTrue from the
 * callback, no further callbacks are invoked. The user should make sure not to take the ownership
 * of the MQTT buffer if they want all the callbacks to get invoked. For example:
 * * Subscriptions: a/b/c, a/b/#, a/b/+
 * * Publish message received on topic: a/b/c --> First the callback corresponding to a/b/c
 *   subscription is invoked. Then the callbacks for topic filters a/b/# and a/b/+ are invoked
 *   in no particular order. If the user decides to take the ownership of the MQTT buffer in
 *   any of the callback by returning eMQTTTrue, no further callbacks are invoked.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] pxSubscribeParams Subscribe parameters.
 * @param[in] xTimeoutTicks Maximum time in ticks after which the operation should fail. Use pdMS_TO_TICKS
 * macro to convert milliseconds to ticks.
 *
 * @return eMQTTAgentSuccess if the subscribe operation succeeds, otherwise an error code explaining
 * the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Subscribe( MQTTAgentHandle_t xMQTTHandle,
                                            const MQTTAgentSubscribeParams_t * const pxSubscribeParams,
                                            TickType_t xTimeoutTicks );

/**
 * @brief Unsubscribes from a given topic.
 *
 * @note This function alters the calling task's notification state and value. If xTimeoutTicks
 * is short the calling task's notification state and value may be updated after MQTT_AGENT_Unsubscribe()
 * has returned.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] pxUnsubscribeParams Unsubscribe parameters.
 * @param[in] xTimeoutTicks Maximum time in ticks after which the operation should fail. Use pdMS_TO_TICKS
 * macro to convert milliseconds to ticks.
 *
 * @return eMQTTAgentSuccess if the unsubscribe operation succeeds, otherwise an error code explaining
 * the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Unsubscribe( MQTTAgentHandle_t xMQTTHandle,
                                              const MQTTAgentUnsubscribeParams_t * const pxUnsubscribeParams,
                                              TickType_t xTimeoutTicks );

/**
 * @brief Publishes a message to a given topic.
 *
 * @note This function alters the calling task's notification state and value. If xTimeoutTicks
 * is short the calling task's notification state and value may be updated after MQTT_AGENT_Publish()
 * has returned.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] pxPublishParams Publish parameters.
 * @param[in] xTimeoutTicks Maximum time in ticks after which the operation should fail. Use pdMS_TO_TICKS
 * macro to convert milliseconds to ticks.
 *
 * @return eMQTTAgentSuccess if the publish operation succeeds, otherwise an error code explaining
 * the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_Publish( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentPublishParams_t * const pxPublishParams,
                                          TickType_t xTimeoutTicks );

/**
 * @brief Returns the buffer provided in the publish callback.
 *
 * When a publish message is received from the broker, the buffer containing the message
 * is returned in the user supplied callback (xBuffer in MQTTPublishData_t) and the user
 * can take the ownership by returning pdTRUE from the callback. The user should later
 * return the buffer whenever done by calling the MQTT_AGENT_ReturnBuffer API.
 *
 * @param[in] xMQTTHandle The opaque handle as returned from MQTT_AGENT_Create.
 * @param[in] xBufferHandle The buffer to return.
 *
 * @return eMQTTAgentSuccess if the return buffer operation succeeds, otherwise an error
 * code explaining the reason of the failure is returned.
 */
MQTTAgentReturnCode_t MQTT_AGENT_ReturnBuffer( MQTTAgentHandle_t xMQTTHandle,
                                               MQTTBufferHandle_t xBufferHandle );

#endif /* _AWS_MQTT_AGENT_H_ */
