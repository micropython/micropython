/*
 * Amazon FreeRTOS
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
 * @file aws_mqtt_lib.h
 * @brief MQTT Core Library interface.
 */

#ifndef _AWS_MQTT_LIB_H_
#define _AWS_MQTT_LIB_H_

/* This ifndef enables the core MQTT library to be used without
 * providing MQTTConfig.h. All the config values in this case are
 * taken from MQTTConfigDefaults.h. */
#ifndef mqttDO_NOT_USE_CUSTOM_CONFIG
    #include "aws_mqtt_config.h"
#endif
#include "aws_mqtt_config_defaults.h"

/* MQTT buffer includes. */
#include "aws_mqtt_buffer.h"

/**
 * @defgroup FixedHeaderSize Macros to define MQTT fixed header size.
 *
 * Each MQTT message starts with a fixed header. The first byte contains the
 * message type and related flags. The next 1-4 bytes contain the length of
 * the packet excluding the length of fixed header (namely "Remaining Length").
 * "Remaining Length" field can span up to 4 bytes, so the maximum length of
 * MQTT fixed header can be 5 bytes and minimum length can be 2 bytes.
 */
/** @{ */
#define mqttREMAINING_LENGTH_MAX_BYTES    4 /**< Remaining Length field can span maximum 4 bytes. */
#define mqttREMAINING_LENGTH_MIN_BYTES    1 /**< Remaining Length field must be of at least 1 byte. */
#define mqttFIXED_HEADER_MAX_SIZE         ( 1 + mqttREMAINING_LENGTH_MAX_BYTES )
#define mqttFIXED_HEADER_MIN_SIZE         ( 1 + mqttREMAINING_LENGTH_MIN_BYTES )
/** @} */

/**
 * @brief Boolean type.
 */
typedef enum
{
    eMQTTFalse = 0, /**< Boolean False. */
    eMQTTTrue = 1   /**< Boolean True. */
} MQTTBool_t;

/**
 * @brief Return codes from various APIs.
 */
typedef enum
{
    eMQTTSuccess = 0,                /**< The operation was successful. */
    eMQTTFailure,                    /**< The operation failed. */
    eMQTTClientAlreadyConnected,     /**< Connection attempt while the client is already connected. */
    eMQTTClientConnectionInProgress, /**< Connection attempt while connection is already in progress. */
    eMQTTClientNotConnected,         /**< Client is not connected. */
    eMQTTNoFreeBuffer,               /**< No free buffer is available for the operation. */
    eMQTTSendFailed,                 /**< The registered send callback failed to transmit data. */
    eMQTTMalformedPacketReceived,    /**< A malformed packet was received. Client has been disconnected. The user must re-connect before carrying out any other operation. */
    eMQTTSubscriptionManagerFull     /**< No space left in subscription manager to store any more subscriptions. */
} MQTTReturnCode_t;

/**
 * @brief The next expected byte.
 *
 * This enum helps in keeping track of the next expected byte in
 * order to decode the incoming bytes.
 */
typedef enum
{
    eMQTTRxNextBytePacketType,   /**< Next received byte will be the first byte of fixed header containing packet type and related flags. */
    eMQTTRxNextBytePacketLength, /**< Next received bytes will contain the encoded "Remaining Length". */
    eMQTTRxNextByteMessage       /**< The fixed header has been received, now receiving the rest of the message. */
} MQTTRxNextByte_t;

/**
 * @brief The action taken on the message being received.
 *
 * If a large enough buffer is available to store the message, it
 * is stored, otherwise it is dropped.
 */
typedef enum
{
    eMQTTRxMessageStore, /**< The message being received is being stored. */
    eMQTTRxMessageDrop   /**< The message being received is being dropped. */
} MQTTRxMessageAction_t;

/**
 * @brief Various events reported by the library in the user supplied callback.
 *
 * The user can register an optional callback with the library to get
 * notified of various events. One of the callback parameter is of type
 * MQTTEventType_t and specifies the event.
 */
typedef enum
{
    eMQTTPublish,            /**< Publish message received from the broker. */
    eMQTTConnACK,            /**< CONNACK received. */
    eMQTTUnexpectedConnACK,  /**< Unexpected CONNACK received. */
    eMQTTPubACK,             /**< PUBACK received. */
    eMQTTUnexpectedPubACK,   /**< Unexpected PUBACK received. */
    eMQTTSubACK,             /**< SUBACK received. */
    eMQTTUnexpectedSubACK,   /**< Unexpected SUBACK received. */
    eMQTTUnSubACK,           /**< UNSUBACK received. */
    eMQTTUnexpectedUnSubACK, /**< Unexpected UNSUBACK received. */
    eMQTTClientDisconnected, /**< Client has been disconnected. The user must re-connect before carrying out any other operation. */
    eMQTTPacketDropped,      /**< A packet was dropped because a large enough buffer was not available to store it. */
    eMQTTTimeout,            /**< Timeout detected - An expected ACK was not received within the specified time. */
    eMQTTPingTimeout         /**< A PINGRESP was not received within the expected time. */
} MQTTEventType_t;

/**
 * @brief The reason of disconnect.
 *
 * Whenever the client is disconnected, the library sends the reason of
 * disconnect in the user supplied callback.
 */
typedef enum
{
    eMQTTDisconnectReasonMalformedPacket,         /**< The client was disconnected because a malformed packet was received. */
    eMQTTDisconnectReasonBrokerRefusedConnection, /**< The client was disconnected because broker refused the connection request. */
    eMQTTDisconnectReasonUserRequest,             /**< The client was disconnected on user request. */
    eMQTTDisconnectReasonConnectTimeout           /**< The client was disconnected because an expected CONNACK was not received. */
} MQTTDisconnectReason_t;

/**
 * @brief Quality of Service (QoS).
 */
typedef enum
{
    eMQTTQoS0 = 0, /**< Quality of Service 0 - Fire and Forget. No ACK. */
    eMQTTQoS1 = 1, /**< Quality of Service 1 - Wait till ACK or Timeout. */
    eMQTTQoS2 = 2  /**< Quality of Service 2 - Not supported. */
} MQTTQoS_t;

/**
 * @brief The connection state.
 */
typedef enum
{
    eMQTTNotConnected,         /**< Not connected. */
    eMQTTConnectionInProgress, /**< Connect has been sent and waiting for CONNACK. */
    eMQTTConnected             /**< Connected. */
} MQTTConnectionState_t;

/**
 * @brief Return codes sent by the broker in the CONNACK message.
 *
 * The values in the following enum exactly correspond to the ones
 * received in the CONNACK packet and must not be changed.
 */
typedef enum
{
    eMQTTConnACKConnectionAccepted = 0,          /**< Connection accepted by the broker. */
    eMQTTConnACKUnacceptableProtocolVersion = 1, /**< Broker does not support the requested protocol version. */
    eMQTTConnACKIdentifierRejected = 2,          /**< Packet identifier rejected. */
    eMQTTConnACKServerUnavailable = 3,           /**< Broker unavailable. */
    eMQTTConnACKBadUsernameOrPassword = 4,       /**< Bad username or password. */
    eMQTTConnACKUnauthorized = 5                 /**< Unauthorized. */
} MQTTConnACKReturnCode_t;

/**
 * @brief Return codes sent by the broker in the SUBACK message.
 *
 * The values in the following enum exactly correspond to the ones
 * received in the SUBACK packet and must not be changed.
 */
typedef enum
{
    eMQTTSubACKSuccessQos0 = 0, /**< Subscription accepted. Granted QoS0. */
    eMQTTSubACKSuccessQos1 = 1, /**< Subscription accepted. Granted QoS1. */
    eMQTTSubACKSuccessQos2 = 2, /**< Subscription accepted. Granted QoS2. */
    eMQTTSubACKFailure = 128    /**< Subscription rejected. */
} MQTTSubACKReturnCode_t;

/**
 * @brief Represents the type of the topic filter.
 */
typedef enum
{
    eMQTTTopicFilterTypeInvalid = 0, /**< The topic filter is invalid. */
    eMQTTTopicFilterTypeSimple,      /**< The topic filter does not contain any wild cards. */
    eMQTTTopicFilterTypeWildCard     /**< The topic filter contains wild cards. */
} MQTTTopicFilterType_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when a CONNACK message is received.
 */
typedef struct MQTTConnACKData
{
    MQTTConnACKReturnCode_t xConnACKReturnCode; /**< CONNACK return code. @see MQTTConnACKReturnCode_t. */
    uint16_t usPacketIdentifier;                /**< Packet identifier which the user can use to match the CONNACK with the Connect request. */
} MQTTConnACKData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when a SUBACK message is received.
 */
typedef struct MQTTSubACKData
{
    MQTTSubACKReturnCode_t xSubACKReturnCode; /**< SUBACK return code. @see MQTTSubACKReturnCode_t. */
    uint16_t usPacketIdentifier;              /**< Packet identifier which the user can use to match the SUBACK with the Subscribe request. */
} MQTTSubACKData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when an UNSUBACK message is received.
 */
typedef struct MQTTUnSubACKData
{
    uint16_t usPacketIdentifier; /**< Packet identifier which the user can use to match the UNSUBACK with the Unsubscribe request. */
} MQTTUnSubACKData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when a PUBACK message is received.
 */
typedef struct MQTTPubACKData
{
    uint16_t usPacketIdentifier; /**< Packet identifier which the user can use to match the PUBACK with the Publish request. */
} MQTTPubACKData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when a publish message from the broker is received.
 */
typedef struct MQTTPublishData
{
    MQTTQoS_t xQos;             /**< Quality of Service (QoS). */
    const uint8_t * pucTopic;   /**< The topic on which the message is received. */
    uint16_t usTopicLength;     /**< Length of the topic. */
    const void * pvData;        /**< The received message. */
    uint32_t ulDataLength;      /**< Length of the message. */
    MQTTBufferHandle_t xBuffer; /**< The buffer containing the whole MQTT message. Both pcTopic and pvData are pointers to the locations in this buffer. */
} MQTTPublishData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when an operation times out.
 */
typedef struct MQTTTimeoutData
{
    uint16_t usPacketIdentifier; /**< Packet identifier which the user can use to identify which operation timed out. */
} MQTTTimeoutData_t;

/**
 * @brief The data sent by the MQTT library in the user supplied callback
 * when the client is disconnected.
 */
typedef struct MQTTDisconnectData
{
    MQTTDisconnectReason_t xDisconnectReason; /**< The reason of disconnect. @see MQTTDisconnectReason_t. */
} MQTTDisconnectData_t;

/**
 * @brief The parameters sent by the MQTT library in the user supplied callback.
 */
typedef struct MQTTCallbackParams
{
    MQTTEventType_t xEventType; /**< The event type. */

    /* Only one of the following is meaningful
     * based on the value of xEventType. */
    union
    {
        MQTTConnACKData_t xMQTTConnACKData;   /**< CONNACK data. */
        MQTTSubACKData_t xMQTTSubACKData;     /**< SUBACK data. */
        MQTTUnSubACKData_t xMQTTUnSubACKData; /**< UNSUBACK data. */
        MQTTPubACKData_t xMQTTPubACKData;     /**< PUBACK data. */
        MQTTPublishData_t xPublishData;       /**< Publish data. */
        MQTTTimeoutData_t xTimeoutData;       /**< Timeout data. */
        MQTTDisconnectData_t xDisconnectData; /**< Disconnect data. */
    } u;
} MQTTEventCallbackParams_t;

/**
 * @brief Signature of the user supplied callback to get notified of various events.
 *
 * The user registers this optional callback to get notified of the various events
 * including publish messages received from the broker.
 *
 * @param[in] pvCallbackContext The callback context as supplied by the user in Init parameters.
 * @param[in] pxParams The event and related data.
 *
 * @return The return value is ignored in all other cases except publish (i.e. eMQTTPublish
 * event):
 * 1. If eMQTTTrue is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) lies with the user.
 * 2. If eMQTTFalse is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) remains with the library and it is recycled as soon as the callback
 * returns.<br>
 * The user should take the ownership of the buffer containing the received message from the
 * broker by returning eMQTTTrue from the callback if the user wants to use the buffer after
 * the callback is over. The user should return the buffer whenever done by calling the
 * MQTT_ReturnBuffer API.
 */
typedef MQTTBool_t ( * MQTTEventCallback_t ) ( void * pvCallbackContext,
                                               const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Signature of the user supplied topic specific publish callback which gets called
 * whenever a publish message is received on the topic this callback is registered for.
 *
 * The user can choose to register this optional topic specific callback while subscribing to
 * a topic. Whenever a publish message is received on the topic, this callback is invoked. If
 * the user chooses not to enable subscription management or chooses not to register a topic
 * specific callback, the generic callback supplied during Init is invoked.
 *
 * @param[in] pvPublishCallbackContext The callback context as supplied by the user in the
 * subscribe parameters.
 * @param[in] pxPublishData The publish data.
 *
 * @return The return value is interpreted as follows:
 * 1. If eMQTTTrue is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) lies with the user.
 * 2. If eMQTTFalse is returned - the ownership of the buffer passed in the callback (xBuffer
 * in MQTTPublishData_t) remains with the library and it is recycled as soon as the callback
 * returns.<br>
 * The user should take the ownership of the buffer containing the received message from the
 * broker by returning eMQTTTrue from the callback if the user wants to use the buffer after
 * the callback is over. The user should return the buffer whenever done by calling the
 * MQTT_ReturnBuffer API.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    typedef MQTTBool_t ( * MQTTPublishCallback_t ) ( void * pvPublishCallbackContext,
                                                     const MQTTPublishData_t * const pxPublishData );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Signature of the user supplied callback to transmit data.
 *
 * The user must register this callback so that the library can transmit the data
 * including keep alive messages when it has to.
 *
 * @param[in] pvSendContext The send context as supplied by the user in Init parameters.
 * @param[in] pucData The data to transmit.
 * @param[in] ulDataLength The length of the data.
 *
 * @return The number of bytes actually transmitted.
 */
typedef uint32_t ( * MQTTSend_t ) ( void * pvSendContext,
                                    const uint8_t * const pucData,
                                    uint32_t ulDataLength );

/**
 * @brief Signature of the callback to get the current tick count.
 *
 * This callback is optional but registering this callback provides more accurate timeout
 * processing. If the user chooses not to register this callback, each timeout processing
 * starts from the moment of next invocation of MQTT_Periodic.
 *
 * @param[out] pxCurrentTickCount The output parameter to receive the current tick count.
 */
typedef void ( * MQTTGetTicks_t ) ( uint64_t * pxCurrentTickCount );

/**
 * @brief Signature of the callback supplied by the user as part
 * of MQTTBufferPoolInterface_t to get a free buffer.
 *
 * The user must register this callback so that the library can
 * obtain a buffer whenever needed. It is okay for the callback
 * to return a buffer larger than the requested length and
 * pulBufferLength must always be updated to the actual length
 * of the returned buffer.
 *
 * @param[in, out] pulBufferLength The library sets it to the desired
 * length of the buffer. It must be updated to the actual length of
 * the returned buffer.
 *
 * @return The pointer to the buffer if one is available, NULL otherwise.
 */
typedef uint8_t * ( * MQTTGetFreeBuffer_t ) ( uint32_t * pulBufferLength );

/**
 * @brief Signature of the callback supplied by the user as part
 * of MQTTBufferPoolInterface_t to return the buffer obtained using
 * MQTTGetFreeBuffer_t.
 *
 * The user must register this callback so that the library can
 * return the buffer whenever done.
 *
 * @param[in] pucBuffer The buffer to return.
 */
typedef void ( * MQTTReturnBuffer_t ) ( uint8_t * pucBuffer );

/**
 * @brief Represents a subscription entry in the subscription manager.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    typedef struct MQTTSubscription
    {
        uint8_t ucTopicFilter[ mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH ]; /**< The topic filter this subscription entry is for. */
        uint16_t usTopicFilterLength;                                             /**< The length of the topic filter. */
        void * pvPublishCallbackContext;                                          /**< The callback context supplied by the user while subscribing. */
        MQTTPublishCallback_t pxPublishCallback;                                  /**< The callback associated with this subscription. */
        MQTTBool_t xInUse;                                                        /**< Tracks whether the subscription entry is in-use. */
        MQTTTopicFilterType_t xTopicFilterType;                                   /**< The type of the topic filter. */
    } MQTTSubscription_t;

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief The subscription manager used to keep track of user subscriptions
 * and topic specific callbacks.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    typedef struct MQTTSubscriptionManager
    {
        MQTTSubscription_t xSubscriptions[ mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ]; /**< User subscriptions. */
        uint32_t ulInUseSubscriptions;                                                         /**< Number of subscription entries currently in use. */
    } MQTTSubscriptionManager_t;

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief The buffer pool interface supplied by the user.
 *
 * The library uses this interface to get a buffer from the pool
 * of free buffers and return it back to the pool whenever done.
 */
typedef struct MQTTBufferPoolInterface
{
    MQTTGetFreeBuffer_t pxGetBufferFxn;   /**< The function to get a free buffer. @see MQTTGetFreeBuffer_t. */
    MQTTReturnBuffer_t pxReturnBufferFxn; /**< The function to return the buffer. @see MQTTReturnBuffer_t. */
} MQTTBufferPoolInterface_t;

/**
 * @brief Represents the state of the message currently being received.
 */
typedef struct MQTTRxMessageState
{
    MQTTRxNextByte_t xRxNextByte;           /**< The next expected byte type. @see MQTTRxNextByte_t. */
    MQTTRxMessageAction_t xRxMessageAction; /**< Whether the current Rx message is being stored or dropped. Valid only after the fixed header has been received i.e. xRxNextByte is eMQTTRxNextByteMessage. @see MQTTRxMessageAction_t. */
    uint8_t ucRemaingingLengthFieldBytes;   /**< The number of bytes the "Remaining Length" field spans. Valid only after the fixed header has been received i.e. xRxNextByte is eMQTTRxNextByteMessage. */
    uint32_t ulTotalMessageLength;          /**< The total length of the message. Valid only after the fixed header has been received i.e. xRxNextByte is eMQTTRxNextByteMessage. */
} MQTTRxMessageState_t;

/**
 * @brief Represents one connection to an MQTT broker.
 *
 * A context should be initialized using MQTT_Init before it can be used
 * in the subsequent function calls.
 */
typedef struct MQTTContext
{
    Link_t xTxBufferListHead;                                   /**< The list of Tx buffers i.e. buffers containing transmitted messages waiting for ACK. */
    MQTTBufferHandle_t xRxBuffer;                               /**< The Rx buffer i.e. the buffer used to store the incoming message. */
    MQTTRxMessageState_t xRxMessageState;                       /**< The state of the message being received currently. */
    uint8_t ucRxFixedHeaderBuffer[ mqttFIXED_HEADER_MAX_SIZE ]; /**< The buffer used to store the fixed header of the incoming message. */
    uint32_t ulRxMessageReceivedLength;                         /**< The length of the message received so far. */
    void * pvCallbackContext;                                   /**< As supplied by the user in Init parameters. */
    MQTTEventCallback_t pxCallback;                             /**< Callback supplied  by the user to get notified of various events. */
    void * pvSendContext;                                       /**< As supplied by the user in Init parameters. */
    MQTTSend_t pxMQTTSendFxn;                                   /**< Callback supplied by the user to transmit data. */
    MQTTGetTicks_t pxGetTicksFxn;                               /**< Callback supplied by the user to get current tick count. */
    MQTTBufferPoolInterface_t xBufferPoolInterface;             /**< The buffer pool interface supplied by the user. @see MQTTBufferPoolInterface_t. */
    MQTTConnectionState_t xConnectionState;                     /**< The current connection state. */
    uint64_t xLastSentMessageTimestamp;                         /**< The timestamp when the last message (or acknowledgment) was sent. */
    uint32_t ulNextPeriodicInvokeTicks;                         /**< The time interval in ticks after which MQTT_Periodic function must be called. */
    uint32_t ulKeepAliveActualIntervalTicks;                    /**< The time interval in ticks after which a keep alive message should be sent. */
    uint32_t ulPingRequestTimeoutTicks;                         /**< The time interval in ticks to wait for PINGRESP after sending PINGREQ. */
    MQTTBool_t xWaitingForPingResp;                             /**< Whether a keep alive message has been sent and we are waiting for response from the broker. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        MQTTSubscriptionManager_t xSubscriptionManager;         /**< The subscription manager used to keep track of user subscriptions and topic specific callbacks.*/
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
} MQTTContext_t;

/**
 * @brief MQTT Init Parameters.
 *
 * Parameters passed to the MQTT_Init API.
 */
typedef struct MQTTInitParams
{
    void * pvCallbackContext;                       /**< Passed as it is in the event callback. */
    MQTTEventCallback_t pxCallback;                 /**< User supplied callback to get notified of various events. Can be NULL. @see MQTTEventCallback_t.*/
    void * pvSendContext;                           /**< Passed as it is in the send callback. */
    MQTTSend_t pxMQTTSendFxn;                       /**< User supplied callback to transmit data. Must not be NULL. @see MQTTSend_t. */
    MQTTGetTicks_t pxGetTicksFxn;                   /**< User supplied callback to get the current tick count. Can be NULL. @see MQTTGetTicks_t. */
    MQTTBufferPoolInterface_t xBufferPoolInterface; /**< User supplied buffer pool interface. @see MQTTBufferPoolInterface_t. */
} MQTTInitParams_t;

/**
 * @brief MQTT Connect Parameters.
 *
 * Parameters passed to the MQTT_Connect API.
 */
typedef struct MQTTConnectParams
{
    uint16_t usKeepAliveIntervalSeconds;     /**< The maximum time interval (in seconds) permitted to elapse between two control packets transmitted. */
    uint32_t ulKeepAliveActualIntervalTicks; /**< The time interval in ticks after which a keep alive message should be sent. Note that keep alive messages are sent only if nothing else is sent. */
    uint32_t ulPingRequestTimeoutTicks;      /**< The time interval in ticks to wait for PINGRESP after sending PINGREQ. */
    const uint8_t * pucClientId;             /**< Client Id identifies the client to the server. Must be unique per broker. */
    uint16_t usClientIdLength;               /**< The length of the client Id. */
    const uint8_t * pucUserName;             /**< The user name. */
    uint16_t usUserNameLength;               /**< The length of the user name. */
    uint16_t usPacketIdentifier;             /**< The same identifier is returned in the callback when corresponding CONNACK is received or the operation times out. */
    uint32_t ulTimeoutTicks;                 /**< The time interval in ticks after which the operation should fail. */
    const char * pucPassword;                /**< The Password. */
    uint16_t usPasswordLength;               /**< The length of the password. */
} MQTTConnectParams_t;

/**
 * @brief MQTT Subscribe Parameters.
 *
 * Parameters passed to the MQTT_Subscribe API.
 */
typedef struct MQTTSubscribeParams
{
    const uint8_t * pucTopic;                    /**< The topic to subscribe to. This can be a topic filter containing wild cards as permitted by the MQTT protocol. */
    uint16_t usTopicLength;                      /**< The length of the topic. */
    MQTTQoS_t xQos;                              /**< Requested Quality of Service (QoS). */
    uint16_t usPacketIdentifier;                 /**< The same identifier is returned in the callback when corresponding SUBACK is received or the operation times out. */
    uint32_t ulTimeoutTicks;                     /**< The time interval in ticks after which the operation should fail. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        void * pvPublishCallbackContext;         /**< Passed as it is in the publish callback. */
        MQTTPublishCallback_t pxPublishCallback; /**< Callback function to be called whenever a publish message is received on this topic or on a topic which matches this
                                                  *   topic filter. If a publish message is received on a topic which matches more than one topic filters, the order in which
                                                  *   the callbacks are invoked is undefined. This can be NULL if the user does not want to register a topic specific callback,
                                                  *   in which case the generic callback ( if registered during initialization ) is invoked. */
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
} MQTTSubscribeParams_t;

/**
 * @brief MQTT Unsubscribe Parameters.
 *
 * Parameters passed to the MQTT_Unsubscribe API.
 */
typedef struct MQTTUnsubscribeParams
{
    const uint8_t * pucTopic;    /**< The topic to unsubscribe from. */
    uint16_t usTopicLength;      /**< The length of the topic. */
    uint16_t usPacketIdentifier; /**< The same identifier is returned in the callback when corresponding UNSUBACK is received or the operation times out. */
    uint32_t ulTimeoutTicks;     /**< The time interval in ticks after which the operation should fail. */
} MQTTUnsubscribeParams_t;

/**
 * @brief MQTT Publish Parameters.
 *
 * Parameters passed to the MQTT_Publish API.
 */
typedef struct MQTTPublishParams
{
    const uint8_t * pucTopic;    /**< The topic to which the data should be published. */
    uint16_t usTopicLength;      /**< The length of the topic. */
    MQTTQoS_t xQos;              /**< Quality of Service. */
    const void * pvData;         /**< The data to publish. */
    uint32_t ulDataLength;       /**< Length of the data. */
    uint16_t usPacketIdentifier; /**< The same identifier is returned in the callback when corresponding PUBACK is received or the operation times out. */
    uint32_t ulTimeoutTicks;     /**< The time interval in ticks after which the operation should fail. */
} MQTTPublishParams_t;

/**
 * @brief Initializes the given MQTT Context.
 *
 * MQTT context must be initialized first using this API before passing it to
 * the subsequent API calls.
 *
 * @param[in] pxMQTTContext The context to initialize.
 * @param[in] pxInitParams Init parameters.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Init( MQTTContext_t * pxMQTTContext,
                            const MQTTInitParams_t * const pxInitParams );

/**
 * @brief Initiates the Connect operation.
 *
 * Prepares and transmits an MQTT connect message and puts the packet on the
 * waiting ACK list which is removed when the corresponding CONNACK is received
 * or the operation times out.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] pxConnectParams Connect parameters.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Connect( MQTTContext_t * pxMQTTContext,
                               const MQTTConnectParams_t * const pxConnectParams );

/**
 * @brief Disconnects the connection to the broker.
 *
 * Prepares and transmits an MQTT disconnect message and resets the internal
 * state. It invokes the user supplied callback to inform about the disconnect
 * and passes all the user supplied buffers in the callback. The user can choose
 * to take the ownership of all the buffers by returning eMQTTTrue from the callback.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Disconnect( MQTTContext_t * pxMQTTContext );

/**
 * @brief Initiates the Subscribe operation.
 *
 * Prepares and transmits an MQTT subscribe message and puts the packet on the
 * waiting ACK list which is removed when the corresponding SUBACK is received
 * or the operation times out.
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
 * @warning If the user takes the ownership of the MQTT buffer by returning eMQTTTrue
 * from the callback, no further callbacks are invoked. The user should make sure not
 * to take the ownership of the MQTT buffer if they want all the callbacks to get invoked.
 * For example:
 * * Subscriptions: a/b/c, a/b/#, a/b/+
 * * Publish message received on topic: a/b/c --> First the callback corresponding to a/b/c
 *   subscription is invoked. Then the callbacks for topic filters a/b/# and a/b/+ are invoked
 *   in no particular order. If the user decides to take the ownership of the MQTT buffer in
 *   any of the callback by returning eMQTTTrue, no further callbacks are invoked.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] pxSubscribeParams Subscribe parameters.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Subscribe( MQTTContext_t * pxMQTTContext,
                                 const MQTTSubscribeParams_t * const pxSubscribeParams );

/**
 * @brief Initiates the Unsubscribe operation.
 *
 * Prepares and transmits an MQTT unsubscribe message and puts the packet on the
 * waiting ACK list which is removed when the corresponding UNSUBACK is received
 * or the operation times out.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] pxUnsubscribeParams Unsubscribe parameters.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Unsubscribe( MQTTContext_t * pxMQTTContext,
                                   const MQTTUnsubscribeParams_t * const pxUnsubscribeParams );

/**
 * @brief Initiates the Publish operation.
 *
 * Prepares and transmits an MQTT publish message. In non QoS0 case, puts the
 * packet on the waiting ACK list which is removed when the corresponding PUBACK
 * is received or the operation times out.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] pxPublishParams Publish parameters.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_Publish( MQTTContext_t * pxMQTTContext,
                               const MQTTPublishParams_t * const pxPublishParams );

/**
 * @brief Decodes the incoming messages.
 *
 * User provides the incoming bytes by calling this API which are buffered
 * until a complete MQTT message has been received after which the user
 * supplied callback is invoked to inform about the received message.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] pucReceivedData Received bytes.
 * @param[in] xReceivedDataLength Number of received bytes.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_ParseReceivedData( MQTTContext_t * pxMQTTContext,
                                         const uint8_t * pucReceivedData,
                                         size_t xReceivedDataLength );

/**
 * @brief Returns the buffer provided in the publish callback.
 *
 * When a publish message is received from the broker, the buffer containing the message
 * is returned in the user supplied callback (xBuffer in MQTTPublishData_t) and the user
 * can take the ownership by returning eMQTTTrue from the callback. The user should later
 * return the buffer whenever done by calling the MQTT_ReturnBuffer API.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] xBufferHandle The buffer to return.
 *
 * @return eMQTTSuccess if everything succeeds, otherwise an error code explaining the reason of failure.
 */
MQTTReturnCode_t MQTT_ReturnBuffer( MQTTContext_t * pxMQTTContext,
                                    MQTTBufferHandle_t xBufferHandle );

/**
 * @brief Manages timeout and keep alive messages.
 *
 * Iterates the pending ACK list and checks if any of them has expired in which
 * case the user supplied callback is invoked to inform about the timeout. Also,
 * checks if sufficient time has passed since the last transmitted a packet and
 * transmits a keep alive (PINGREQ) message accordingly.
 *
 * @param[in] pxMQTTContext The initialized MQTT context.
 * @param[in] xCurrentTickCount The current tick count value.
 *
 * @return The time interval in ticks after which this function should be called again.
 */
uint32_t MQTT_Periodic( MQTTContext_t * pxMQTTContext,
                        uint64_t xCurrentTickCount );

#endif /* _AWS_MQTT_LIB_H_ */
