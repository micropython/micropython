/*
 * Amazon FreeRTOS MQTT Agent V1.1.3
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
 * @file aws_mqtt_agent.c
 * @brief MQTT Agent implementation.
 */

/* MQTT agent includes. */
#include "aws_mqtt_agent.h"
#include "aws_mqtt_agent_config.h"
#include "aws_mqtt_agent_config_defaults.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Buffer Pool includes. */
#include "aws_bufferpool.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Secure sockets include. */
#include "aws_secure_sockets.h"

/* Standard includes. */
#include <string.h>

/**
 * @brief The length of the command queue used to send commands from application
 * tasks to the MQTT task.
 *
 * The queue can have a maximum of mqttconfigMAX_PARALLEL_OPS parallel operations
 * for each broker connection at any one time. The socket wake callback will only
 * post to the queue if the queue is empty, so there is no need to leave space for
 * that.
 */
#define mqttCOMMAND_QUEUE_LENGTH    ( ( UBaseType_t ) ( mqttconfigMAX_BROKERS * mqttconfigMAX_PARALLEL_OPS ) )

/**
 * @defgroup MessageIdentifer Macros related to message identifier.
 *
 * The message identifier is used to match Connect, Publish, Subscribe etc.
 * requests from an application task to the MQTT task with the response sent
 * from the MQTT task back to the application task. It must only use the top
 * 16-bits of a 32-bit value as the bottom 16-bits pass additional information,
 * for example bit 0 is set to 1 to indicate success (pdPASS) and 0 to indicate
 * a failure (pdFAIL). The value contained in the top 16 bits is also passed as
 * MQTT packet identifier to the MQTT Core Library. The Core library returns this
 * value with ACK or timeout to inform which packet the ACK or timeout response
 * corresponds to.
 */
/** @{ */
#define mqttMESSAGE_IDENTIFIER_MIN     ( 0x00010000UL )
#define mqttMESSAGE_IDENTIFIER_MAX     ( 0xFFFF0000UL )
#define mqttMESSAGE_IDENTIFIER_MASK    ( 0xFFFF0000UL )

/**
 * @brief Extracts the message identifier from the 32-bit notification value.
 *
 * @param[in] x The 32-bit notification value.
 */
#define mqttMESSAGE_IDENTIFIER_EXTRACT( x )    ( x >> 16 )
/** @} */

/**
 * @defgroup Notification Macros related to notification code and status.
 *
 * The notification value sent from the MQTT task to the application task contains
 * a code  and status ( pdPASS/pdFAIL ) about the requested operation. Bit 0 contains
 * the status of the requested operation and is 1 in case of success ( pdPASS ) and 0
 * in case of failure ( pdFAIL ). Bit 1 to Bit 15 contain one of the MQTTNotifyCodes_t
 * value and provide more information about the requested operation. The below macros
 * help to extract the status and code from the received notification value.
 * @see MQTTNotifyCodes_t.
 */
/** @{ */
#define mqttNOTIFICATION_CODE_MASK      ( 0x0000FFFEUL )
#define mqttNOTIFICATION_STATUS_MASK    ( 0x00000001UL )
/** @} */

/**
 * @defgroup ConnectionFlags Flags to identify different properties of a connection.
 */
/** @{ */
#define mqttCONNECTION_SECURED    ( ( UBaseType_t ) 1 << ( UBaseType_t ) 0 )
/** @} */

/**
 * @brief Encodes the broker number returned to the user.
 *
 * Adding one ensures that the user does not ever get a
 * NULL handle.
 */
#define mqttENCODE_BROKER_NUMBER( xBrokerNumber )    ( ( UBaseType_t ) xBrokerNumber + ( UBaseType_t ) 1 )

/**
 * @brief Decodes the broker number supplied by the user.
 *
 * Subtracting one ensures that we index the correct MQTTBrokerConnection_t
 * in the xMQTTConnections array since we added one to the index before giving
 * it to the user as an opaque handle.
 */
#define mqttDECODE_BROKER_NUMBER( xBrokerNumber )    ( ( UBaseType_t ) xBrokerNumber - ( UBaseType_t ) 1 )
/*-----------------------------------------------------------*/

/**
 * @brief Type of the message sent from an application task to the MQTT task.
 */
typedef enum
{
    eMQTTServiceSocket = 0,  /**< See if any of the active connections need servicing. */
    eMQTTConnectRequest,     /**< Initiate a connection to an MQTT broker. */
    eMQTTDisconnectRequest,  /**< Disconnect the connection to an MQTT broker. */
    eMQTTSubscribeRequest,   /**< Initiate a subscribe to a topic.  _TODO_ Currently limited to one topic per subscribe message. */
    eMQTTUnsubscribeRequest, /**< Initiate unsubscribe from a topic.  _TODO_ Currently limited to one topic per unsubscribe message. */
    eMQTTPublishRequest      /**< Initiate a publish to a topic.  _TODO_ Currently limited to one topic per publish message. */
} MQTTAction_t;

/**
 * @brief Status codes sent in notifications passed from the MQTT task to application tasks.
 *
 * The last bit of the status code is used to indicate pass (bit set) or fail (bit clear),
 * so the status codes defined below must have bit 0 clear.
 */
typedef enum
{
    eMQTTCONNCouldNotBeSent = 2,          /**< CONNECT message could not be sent. */
    eMQTTCONNACKConnectionAccepted = 4,   /**< CONNACK received and the broker has accepted the connection. */
    eMQTTCONNACKConnectionRejected = 6,   /**< CONNACK received but the broker has rejected the connection. */
    eMQTTDISCONNSent = 8,                 /**< DISCONNECT message sent successfully. */
    eMQTTDISCONNCouldNotBeSent = 10,      /**< DISCONNECT message could not be sent. */
    eMQTTPUBCouldNotBeSent = 12,          /**< PUBLISH message could not be sent. */
    eMQTTPUBSent = 14,                    /**< PUBLISH message sent successfully. */
    eMQTTPUBACKReceived = 16,             /**< PUBACK received. */
    eMQTTSUBCouldNotBeSent = 18,          /**< SUBSCRIBE message could not be sent. */
    eMQTTSUBACKSubscriptionAccepted = 20, /**< SUBACK received and the broker has accepted the subscription. */
    eMQTTSUBACKSubscriptionRejected = 22, /**< SUBACK received but the broker has rejected the subscription. */
    eMQTTUNSUBCouldNotBeSent = 24,        /**< UNSUBSCRIBE message could not be sent. */
    eMQTTUNSUBACKReceived = 26,           /**< UNSUBACK received. */
    eMQTTBufferAdded = 28,                /**< Provided buffer was successfully added to the MQTT core library. */
    eMQTTBufferCouldNotBeAdded = 30,      /**< Provided buffer could not be added to the MQTT library. */
    eMQTTOperationTimedOut = 32,          /**< The requested operation could not be completed within the specified time. */
    eMQTTClientGotDisconnected = 34       /**< The MQTT client got disconnect in the middle of an operation. */
} MQTTNotifyCodes_t;

/**
 * @brief Stores the information required to send a pass/fail notification
 * to whichever task initiated the operation.
 */
typedef struct MQTTNotificationData
{
    TaskHandle_t xTaskToNotify;   /**< The handle of the task to notify. */
    uint32_t ulMessageIdentifier; /**< Used to match a request going from application task to MQTT task with response going the other way. */
} MQTTNotificationData_t;

/**
 * @brief Contents of the message sent from an application task to the MQTT task to
 * initiate an MQTT operation.
 *
 * Application task creates an object of this type and posts it to the command queue
 * in order to initiate an MQTT operation.
 */
typedef struct MQTTEventData
{
    UBaseType_t uxBrokerNumber;               /**< The broker this command is for, indexed from 0. */
    MQTTAction_t xEventType;                  /**< The operation to initiate. */
    MQTTNotificationData_t xNotificationData; /**< Information used to notify the task that initiated the operation when the operation is complete. */
    TimeOut_t xEventCreationTimestamp;        /**< Timestamp when this event was created. */
    TickType_t xTicksToWait;                  /**< Time in tick counts after which the operation should fail. */
    /* Only one of the following is relevant based on the value of xEventType. */
    union
    {
        const MQTTAgentConnectParams_t * pxConnectParams;         /**< Connect Parameters. */
        const MQTTAgentSubscribeParams_t * pxSubscribeParams;     /**< Subscribe Parameters. */
        const MQTTAgentUnsubscribeParams_t * pxUnsubscribeParams; /**< Unsubscribe Parameters. */
        const MQTTAgentPublishParams_t * pxPublishParams;         /**< Publish Parameters. */
    } u;
} MQTTEventData_t;

/**
 * @brief Contains the state of a connection to MQTT broker.
 *
 * The MQTT task can connect to maximum mqttconfigMAX_BROKERS brokers at any
 * one time. The state of each connection is contained in a
 * MQTTBrokerConnection_t structure.
 */
typedef struct MQTTBrokerConnection
{
    Socket_t xSocket;                                                   /**< TCP socket connected to the broker. */
    MQTTContext_t xMQTTContext;                                         /**< MQTT Core library context. */
    MQTTNotificationData_t xWaitingTasks[ mqttconfigMAX_PARALLEL_OPS ]; /**< Notification data to notify tasks which have sent commands to MQTT command queue and are waiting for results. */
    void * pvUserData;                                                  /**< User data to be supplied back in the callback as it is. */
    MQTTAgentCallback_t pxCallback;                                     /**< The callback to notify user of various events including the Publish messages received from the broker. */
    UBaseType_t uxFlags;                                                /**< Various properties of the connection - secured etc. */
    BaseType_t xConnectionInUse;                                        /**< Tracks whether or not the connection is in use. It is accessed from application tasks (prvGetFreeConnection and prvReturnConnection) and hence should be accessed in critical section. */
    uint8_t ucRxBuffer[ mqttconfigRX_BUFFER_SIZE ];                     /**< Buffers incoming messages. */
} MQTTBrokerConnection_t;
/*-----------------------------------------------------------*/

/**
 * @brief The user name used by all the MQTT connections. AWS IoT uses the user
 * name for collecting anonymous metrics on SDK usage. Metrics default to on.
 */
#if ( mqttconfigENABLE_METRICS == 1 )
    #ifndef configPLATFORM_NAME
        #define configPLATFORM_NAME    "Unknown"
    #endif

    static const char cUserName[] = "?SDK=AmazonFreeRTOS"
                                    "&Version="tskKERNEL_VERSION_NUMBER
                                    "&Platform="configPLATFORM_NAME;

#else
    static const char cUserName[] = "";
#endif

/**
 * @brief The length of the above username.
 */
static const uint16_t usUserNameLength = ( uint16_t ) ( sizeof( cUserName ) - 1U );

/**
 * @brief State of connections to MQTT brokers.
 *
 * Each MQTTBrokerConnection_t structure maintains the state of one connection
 * to an MQTT broker. The maximum number of simultaneous connections is set by
 * mqttconfigMAX_BROKERS.
 */
static MQTTBrokerConnection_t xMQTTConnections[ mqttconfigMAX_BROKERS ];

/**
 * @brief Handle of the command queue used to pass commands from application
 * tasks to the MQTT task.
 */
static QueueHandle_t xCommandQueue = NULL;

/**
 * @brief Handle of the MQTT task.
 */
static TaskHandle_t xMQTTTaskHandle = NULL;

/**
 * @brief Used to match commands sent to the MQTT task to replies coming from the
 * MQTT task.
 */
static uint32_t ulQueueMessageIdentifier = 0;
/*-----------------------------------------------------------*/

/**
 * @brief The callback registered with the core MQTT library to transmit bytes over wire.
 *
 * The MQTT core library calls this function whenever it needs to transmit any bytes.
 * @param[in] pvSendContext The send context is broker number in our case.
 *
 * @param[in] pucData The data to transmit.
 * @param[in] ulDataLength Length of the data.
 *
 * @return The number of actually transmitted bytes. Can be less than ulDataLength if
 * transmission fails for some reason.
 */
static uint32_t prvMQTTSendCallback( void * pvSendContext,
                                     const uint8_t * const pucData,
                                     uint32_t ulDataLength );

/**
 * @brief The callback registered with the core MQTT library to receive various MQTT events.
 *
 * The core MQTT library notifies about various events (acknowledgments, publishes, unexpected packets etc.)
 * in this callback. Each MQTT core library operation requires a packet identifier which is sent back
 * with ACK or timeout.
 *
 * Whenever we initiate an MQTT operation for which we expect an ACK, we store the MQTTNotificationData_t
 * for the task that initiates the operation. The notification data contains a message identifier, top 16 bits
 * of which are used as packet identifier. Upon receiving ACK or timeout, we match the packet identifier with
 * the top 16 bits of message identifier in MQTTNotificationData_t to figure out the task which initiated the
 * operation. The task is then notified of the result.
 *
 * @param[in] pvCallbackContext The callback context is broker number in our case.
 * @param[in] pxParams The event type and related data.
 *
 * @return The return value is significant only in the case of publish messages. We return eMQTTTrue
 * if the user wants to take the ownership of the buffer (indicated by the user by returning pdTRUE
 * from the callback), otherwise we return eMQTTFalse. For all other messages than publish, we return
 * eMQTTFalse (which is ignored by the Core library anyways).
 */
static MQTTBool_t prvMQTTEventCallback( void * pvCallbackContext,
                                        const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Returns the current tick count.
 *
 * This function is also registered with the MQTT Core library and gets called by the Core
 * library whenever it needs the current tick count.
 *
 * @param[out] pxCurrentTickCount The output parameter to receive the current tick count.
 */
static void prvMQTTGetTicks( uint64_t * pxCurrentTickCount );

/**
 * @brief Gets an unused connection.
 *
 * This functions iterates over xMQTTConnections array and tries to find a free connection (i.e. one with
 * mqttCONNECTION_IN_USE bit in uxFlags cleared). If it finds one, it returns its index in the xMQTTConnections
 * array otherwise it returns -1 to indicate failure.
 *
 * @return The index of a free connection in xMQTTConnections array if one is available, -1 otherwise.
 */
static BaseType_t prvGetFreeConnection( void );

/**
 * @brief Returns the connection to the unused pool by marking it "not in use".
 *
 * @param[in] uxBrokerNumber Index of the connection in the xMQTTConnections array.
 */
static void prvReturnConnection( UBaseType_t uxBrokerNumber );

/**
 * @brief Stores the notification data in one of the available buffers in MQTTBrokerConnection_t.
 *
 * Finds an empty buffer from xWaitingTasks in MQTTBrokerConnection_t to store the notification data
 * (empty buffer is identified by the NULL value of xTaskToNotify). If it finds a buffer, it stores the
 * notification data from the event data and returns the pointer to the buffer. If it fails to find an
 * empty buffer, it returns NULL.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the MQTT broker connection the event is for.
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 *
 * @return Pointer to the buffer used to store notification data or NULL if all buffers are in use.
 */
static MQTTNotificationData_t * prvStoreNotificationData( MQTTBrokerConnection_t * const pxConnection,
                                                          const MQTTEventData_t * const pxEventData );

/**
 * @brief Retrieves the notification data matching the given packet identifier.
 *
 * Whenever we receive a message from the MQTT Core library, we need to check if any task is waiting
 * for it and accordingly notify it. This functions iterates over all the waiting tasks ( i.e. xWaitingTasks )
 * in MQTTBrokerConnection_t and tries to find a notification data having message identifier with top 16
 * bits matching the received packet identifier from the Core library.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t
 * @param[in] usPacketIdentifier The packet identifier.
 *
 * @return Pointer to the buffer containing the notification data if a task is waiting, NULL otherwise.
 */
static MQTTNotificationData_t * prvRetrieveNotificationData( MQTTBrokerConnection_t * const pxConnection,
                                                             uint16_t usPacketIdentifier );

/**
 * @brief Sets up the connection as per the parameters in event data.
 *
 * Establishes the TCP connection with the broker and sets up the appropriate options
 * for the socket.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 *
 * @return pdPASS if connection succeeds, pdFAIL otherwise.
 */
static BaseType_t prvSetupConnection( const MQTTEventData_t * const pxEventData );

/**
 * @brief Gracefully terminates the connection.
 *
 * Cleanup the TLS context. Shuts the socket for both read and write and then tries
 * to read all the available data (bounded by a certain timeout) after which it closes
 * the socket.
 *
 * @param[in] pxConnection MQTTBrokerConnection_t structure specifying the connection to terminate.
 */
static void prvGracefulSocketClose( MQTTBrokerConnection_t * const pxConnection );

/**
 * @brief The callback registered with the socket to get notified of the available data to read on the socket.
 *
 * This function just posts a eMQTTServiceSocket request to the MQTT command queue to
 * unblock the MQTT task in order to ensure that the available data is read and processed.
 *
 * @param[in] pxSocket The socket on which the data is available for reading.
 */
static void prvMQTTClientSocketWakeupCallback( Socket_t pxSocket );

/**
 * @brief Notifies the application task about the received CONNACK message.
 *
 * Retrieves the notification data corresponding to the task which initiated the Connect operation. If
 * there is a task waiting for the CONNACK, notifies the task otherwise silently ignores the CONNACK.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which CONNACK is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedCONNACK( MQTTBrokerConnection_t * const pxConnection,
                                       const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the application task about the received SUBACK message.
 *
 * Retrieves the notification data corresponding to the task which initiated the Subscribe operation.
 * If there is a task waiting for the SUBACK, notifies the task otherwise silently ignores the SUBACK.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which SUBACK is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedSUBACK( MQTTBrokerConnection_t * const pxConnection,
                                      const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the application task about the received UNSUBACK message.
 *
 * Retrieves the notification data corresponding to the task which initiated the Unsubscribe operation.
 * If there is a task waiting for the UNSUBACK, notifies the task otherwise silently ignores the UNSUBACK.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which UNSUBACK is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedUNSUBACK( MQTTBrokerConnection_t * const pxConnection,
                                        const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the application task about the received PUBACK message.
 *
 * Retrieves the notification data corresponding to the task which initiated the Publish operation.
 * If there is a task waiting for the PUBACK, notifies the task otherwise silently ignores the PUBACK.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which PUBACK is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedPUBACK( MQTTBrokerConnection_t * const pxConnection,
                                      const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the user about the received Publish message.
 *
 * If the user has registered a callback, invokes the callback to inform the user about the Publish message
 * otherwise silently ignores the message.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which Publish is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 *
 * @return The value returned by the callback if the user has registered one, pdFALSE otherwise.
 */
static BaseType_t prvProcessReceivedPublish( MQTTBrokerConnection_t * const pxConnection,
                                             const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the application task about the timeout.
 *
 * Retrieves the notification data corresponding to the task which initiated the operation. If there is a
 * task waiting for ACK matching the packet identifier supplied by the Core library, notifies the task
 * otherwise silently ignores the event.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which Timeout event is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedTimeout( MQTTBrokerConnection_t * const pxConnection,
                                       const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the user about the disconnect and unblocks all the waiting tasks.
 *
 * If the user has registered a callback, invokes the callback to inform the user about the disconnect
 * event. Also closes the socket connection.
 *
 * @param[in] pxConnection The MQTTBrokerConnection_t corresponding to the connection on which Disconnect event is received.
 * @param[in] pxParams The parameters received in the callback form the MQTT Core library containing relevant data.
 */
static void prvProcessReceivedDisconnect( MQTTBrokerConnection_t * const pxConnection,
                                          const MQTTEventCallbackParams_t * const pxParams );

/**
 * @brief Notifies the task by encoding the packet identifier, notification code and status in the notification value.
 *
 * If notification data is not NULL, notification value is encoded as follows: Most significant 16
 * bits contain the packet identifier same as set by the application task while posting to the command
 * queue. Next 15 bits contain the status code. Last bit contains the status - 1 for pdPASS and 0
 * for pdFAIL.
 *
 * @param[in] pxNotificationData Notification data containing the information about the task to be notified.
 * @param[in] eNotificationCode Notification code about the result of the operation.
 * @param[in] uxStatus Status of the operation (pdPASS/pdFAIL).
 */
static void prvNotifyRequestingTask( MQTTNotificationData_t * const pxNotificationData,
                                     MQTTNotifyCodes_t xNotificationCode,
                                     UBaseType_t uxStatus );

/**
 * @brief Called on each iteration of the MQTT task to service connected sockets.
 *
 * For all the connected sockets, it reads the available data and passes it to the
 * MQTT Core library. It also invokes the MQTT_Periodic function of the core library
 * to ensure regular timeout and keep alive processing.
 *
 * @return Time in ticks when the next invocation of MQTT_Periodic is required.
 */
static TickType_t prvManageConnections( void );

/**
 * @brief Initiates the MQTT Connect operation.
 *
 * First, it stores the notification data corresponding to the task which initiated the
 * operation in one of the available buffers in MQTTBrokerConnection_t. If it fails to
 * find a free buffer, it fails immediately and notifies the application task. Upon
 * successfully storing the notification data, it sets up the connect parameters and
 * calls the MQTT_Connect function of the core MQTT library. If that fails, it informs
 * the application task and frees the buffer used to store the notification data.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 */
static void prvInitiateMQTTConnect( MQTTEventData_t * const pxEventData );

/**
 * @brief Initiates the MQTT Disconnect operation.
 *
 * Invokes the MQTT_Disconnect function of the core MQTT library and informs the application
 * task about the result.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 */
static void prvInitiateMQTTDisconnect( MQTTEventData_t * const pxEventData );

/**
 * @brief Initiates the MQTT Subscribe operation.
 *
 * First, it stores the notification data corresponding to the task which initiated the
 * operation in one of the available buffers in MQTTBrokerConnection_t. If it fails to
 * find a free buffer, it fails immediately and notifies the application task. Upon
 * successfully storing the notification data, it sets up the subscribe parameters and
 * calls the MQTT_Subscribe function of the core MQTT library. If that fails, it informs
 * the application task and frees the buffer used to store the notification data.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 */
static void prvInitiateMQTTSubscribe( MQTTEventData_t * const pxEventData );

/**
 * @brief Initiates the MQTT Unsubscribe operation.
 *
 * First, it stores the notification data corresponding to the task which initiated the
 * operation in one of the available buffers in MQTTBrokerConnection_t. If it fails to
 * find a free buffer, it fails immediately and notifies the application task. Upon
 * successfully storing the notification data, it sets up the unsubscribe parameters and
 * calls the MQTT_Unsubscribe function of the core MQTT library. If that fails, it informs
 * the application task and frees the buffer used to store the notification data.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 */
static void prvInitiateMQTTUnSubscribe( MQTTEventData_t * const pxEventData );

/**
 * @brief Initiates the MQTT Publish operation.
 *
 * In case of QoS0, it sets up the publish parameters and calls the MQTT_Publish function
 * of the core MQTT library. It informs the success or failure to the application task
 * immediately. In non QoS0 case, first it stores the notification data corresponding to
 * the task which initiated the operation in one of the available buffers in
 * MQTTBrokerConnection_t. If it fails to find a free buffer, it fails immediately and
 * notifies the application task. Upon successfully storing the notification data, it
 * sets up the publish parameters and calls the MQTT_Publish function of the core MQTT
 * library. If that fails, it informs the application task and frees the buffer used to
 * store the notification data.
 *
 * @param[in] pxEventData The event data as posted by application task to the command queue.
 */
static void prvInitiateMQTTPublish( MQTTEventData_t * const pxEventData );

/*
 * @brief Posts the event to the command queue and waits for the notification from the MQTT task.
 *
 * This function interfaces application tasks to the MQTT task. Connect, Publish, Subscribe
 * and other requests are packed up into a structure then send over a command queue
 * to the MQTT task. After posting to the command queue, it puts the calling task in
 * blocked state (bounded by a certain timeout) waiting for the notification from the
 * MQTT task. Upon receiving the notification, decodes the notification value and returns
 * eMQTTAgentSuccess if the operation succeeded, eMQTTAgentFailure if the operation
 * failed or eMQTTAgentTimeout if the operation timed out.
 * NOTE that this function sets the packet identifier field of notification data in
 * pxEventData before posting it to the command queue.
 *
 * @param[in] pxEventData The Event to be sent to the command queue.
 *
 * @return eMQTTAgentSuccess if the command sent to the MQTT task was successful,
 * eMQTTAgentTimeout if it timed out, eMQTTAgentFailure to indicate any other
 * failure.
 */
static MQTTAgentReturnCode_t prvSendCommandToMQTTTask( MQTTEventData_t * pxEventData );

/**
 * @brief Implements the task that manages the MQTT protocol.
 *
 * This function reads messages from the command queue and processes them.
 * It wakes up periodically and calls prvManageConnections() in order to
 * ensure regular timeout and keep alive processing by the MQTT Core library.
 *
 * @param[in] pvParameters The parameters as specified when creating the task, NULL in this case.
 */
static void prvMQTTTask( void * pvParameters );
/*-----------------------------------------------------------*/

static uint32_t prvMQTTSendCallback( void * pvSendContext,
                                     const uint8_t * const pucData,
                                     uint32_t ulDataLength )
{
    MQTTBrokerConnection_t * pxConnection;
    UBaseType_t uxBrokerNumber = ( UBaseType_t ) pvSendContext; /*lint !e923 The cast is ok as we passed the index of the client before. */
    int32_t lSendRetVal;
    uint32_t ulBytesSent = 0;
    TimeOut_t xTimestamp;
    TickType_t xTicksToWait = pdMS_TO_TICKS( mqttconfigTCP_SEND_TIMEOUT_MS );

    /* Broker number must be valid. */
    configASSERT( uxBrokerNumber < ( UBaseType_t ) mqttconfigMAX_BROKERS );

    /* Record the timestamp when this function was called. */
    vTaskSetTimeOutState( &( xTimestamp ) );

    /* Get the actual connection to the broker. */
    pxConnection = &( xMQTTConnections[ uxBrokerNumber ] );

    /* Keep re-trying until timeout or any error
     * other than SOCKETS_EWOULDBLOCK occurs. */
    while( ulBytesSent < ulDataLength )
    {
        /* Check for timeout and if timeout has occurred, stop retrying. */
        if( xTaskCheckForTimeOut( &( xTimestamp ), &( xTicksToWait ) ) == pdTRUE )
        {
            break;
        }

        /* Try sending the remaining data. */
        lSendRetVal = SOCKETS_Send( pxConnection->xSocket,
                                    &( pucData[ ulBytesSent ] ),               /* Only send the remaining data. */
                                    ( size_t ) ( ulDataLength - ulBytesSent ), /* Only send the remaining data. */
                                    0 );

        /* A negative return value from SOCKETS_Send
         * means some error occurred. */
        if( lSendRetVal < 0 )
        {
            /* Since the socket is non-blocking, send can return
             * SOCKETS_EWOULDBLOCK, in which case we retry again until
             * timeout. In case of any other error, we stop re-trying. */
            if( lSendRetVal != SOCKETS_EWOULDBLOCK )
            {
                break;
            }
        }
        else
        {
            /* Update the count of sent bytes. */
            ulBytesSent += ( uint32_t ) lSendRetVal;
        }
    }

    return ulBytesSent;
}
/*-----------------------------------------------------------*/
static MQTTBool_t prvMQTTEventCallback( void * pvCallbackContext,
                                        const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTBool_t xReturn = eMQTTFalse;
    MQTTBrokerConnection_t * pxConnection;
    UBaseType_t uxBrokerNumber = ( UBaseType_t ) pvCallbackContext; /*lint !e923 The cast is ok as we passed the index of the client before. */

    /* Broker number must be valid. */
    configASSERT( uxBrokerNumber < ( UBaseType_t ) mqttconfigMAX_BROKERS );

    /* Get the actual connection to the broker. */
    pxConnection = &( xMQTTConnections[ uxBrokerNumber ] );

    switch( pxParams->xEventType )
    {
        case eMQTTConnACK:
            prvProcessReceivedCONNACK( pxConnection, pxParams );
            break;

        case eMQTTSubACK:
            prvProcessReceivedSUBACK( pxConnection, pxParams );
            break;

        case eMQTTUnSubACK:
            prvProcessReceivedUNSUBACK( pxConnection, pxParams );
            break;

        case eMQTTPubACK:
            prvProcessReceivedPUBACK( pxConnection, pxParams );
            break;

        case eMQTTPublish:

            /* Inform the core library if the user wants to take
             * the ownership of the provided buffer. */
            if( prvProcessReceivedPublish( pxConnection, pxParams ) == pdTRUE )
            {
                xReturn = eMQTTTrue;
            }

            break;

        case eMQTTTimeout:
            prvProcessReceivedTimeout( pxConnection, pxParams );
            break;

        case eMQTTClientDisconnected:
            prvProcessReceivedDisconnect( pxConnection, pxParams );
            break;

        case eMQTTPacketDropped:
            mqttconfigDEBUG_LOG( ( "[WARN] MQTT Agent dropped a packet. No buffer available.\r\n" ) );
            mqttconfigDEBUG_LOG( ( "Consider adjusting parameters in aws_bufferpool_config.h.\r\n" ) );
            break;

        default:
            /* _TODO_ Handle the remaining events. */
            break;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvMQTTGetTicks( uint64_t * pxCurrentTickCount )
{
    TimeOut_t xTimeout;

    /* Get the current time-stamp and pack it in uint64_t. */
    vTaskSetTimeOutState( &xTimeout );

    /* _TODO_ - Might need to change for port. */
    *pxCurrentTickCount = ( uint64_t ) xTimeout.xOverflowCount;
    *pxCurrentTickCount <<= 32;
    *pxCurrentTickCount |= xTimeout.xTimeOnEntering;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGetFreeConnection( void )
{
    BaseType_t x;

    /* Iterate over all connections to check if a free connection is available.
     * Since this function is called from application tasks, many tasks can be
     * touching the xConnectionInUse simultaneously and therefore it has to be
     * in critical section. */
    taskENTER_CRITICAL();

    for( x = 0; x < mqttconfigMAX_BROKERS; x++ )
    {
        /* If we find a free connection... */
        if( xMQTTConnections[ x ].xConnectionInUse == pdFALSE )
        {
            /* ...mark the connection "in use" and stop. */
            xMQTTConnections[ x ].xConnectionInUse = pdTRUE;
            break;
        }
    }

    taskEXIT_CRITICAL();

    /* If we could not find a free connection, return -1 to indicate failure. */
    if( x == mqttconfigMAX_BROKERS )
    {
        x = -1;
    }

    return x;
}
/*-----------------------------------------------------------*/

static void prvReturnConnection( UBaseType_t uxBrokerNumber )
{
    configASSERT( uxBrokerNumber < ( UBaseType_t ) mqttconfigMAX_BROKERS );

    /* Since this function is called from application tasks, many tasks can
     * be touching the xConnectionInUse simultaneously and therefore it has to
     * be in critical section. */
    taskENTER_CRITICAL();
    /* Mark the connection as "not in use". */
    xMQTTConnections[ uxBrokerNumber ].xConnectionInUse = pdFALSE;
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static MQTTNotificationData_t * prvStoreNotificationData( MQTTBrokerConnection_t * const pxConnection,
                                                          const MQTTEventData_t * const pxEventData )
{
    UBaseType_t x;
    MQTTNotificationData_t * pxNotificationData = NULL;

    /* Iterate over all the buffers to find an unused one. Unused
    * buffer is identified by the NULL value of xTaskToNotify. */
    for( x = 0; x < ( UBaseType_t ) mqttconfigMAX_PARALLEL_OPS; x++ )
    {
        if( pxConnection->xWaitingTasks[ x ].xTaskToNotify == NULL )
        {
            /* We found one unused buffer - copy the notification data
             * and return. */
            pxNotificationData = &( pxConnection->xWaitingTasks[ x ] );
            memcpy( pxNotificationData, &( pxEventData->xNotificationData ), sizeof( MQTTNotificationData_t ) );
            break;
        }
    }

    return pxNotificationData;
}
/*-----------------------------------------------------------*/

static MQTTNotificationData_t * prvRetrieveNotificationData( MQTTBrokerConnection_t * const pxConnection,
                                                             uint16_t usPacketIdentifier )
{
    UBaseType_t x;
    MQTTNotificationData_t * pxNotificationData = NULL;

    /* Iterate over all the buffers to see if there is one matching the
     * packet identifier. Note that the packet identifier constitutes of the
     * top 16 bits of the message identifier stored in the notification data. */
    for( x = 0; x < ( UBaseType_t ) mqttconfigMAX_PARALLEL_OPS; x++ )
    {
        if( ( pxConnection->xWaitingTasks[ x ].xTaskToNotify != NULL ) &&
            ( usPacketIdentifier == ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxConnection->xWaitingTasks[ x ].ulMessageIdentifier ) ) ) )
        {
            /* We found the notification data, return it. */
            pxNotificationData = &( pxConnection->xWaitingTasks[ x ] );
            break;
        }
    }

    return pxNotificationData;
}
/*-----------------------------------------------------------*/

static BaseType_t prvSetupConnection( const MQTTEventData_t * const pxEventData )
{
    SocketsSockaddr_t xMQTTServerAddress = { 0 };
    BaseType_t xStatus = pdPASS;
    size_t xURLLength;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );
    char * ppcAlpns[] = { socketsAWS_IOT_ALPN_MQTT };

    /* Should not get here if the socket used to communicate with the
     * broker is already connected. */
    configASSERT( pxConnection->xSocket == SOCKETS_INVALID_SOCKET );

    /* Calculate the length of the supplied URL. */
    xURLLength = strlen( pxEventData->u.pxConnectParams->pcURL );

    /* Ensure that the length of the specified URL is
     * within the permitted limits. */
    if( xURLLength <= ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        if( ( pxEventData->u.pxConnectParams->xFlags & mqttagentUSE_AWS_IOT_ALPN_443 ) != 0 )
        {
            /* The AWS IoT ALPN feature implies the default TLS server port. */
            xMQTTServerAddress.usPort = SOCKETS_htons( securesocketsDEFAULT_TLS_DESTINATION_PORT );
        }
        else
        {
            xMQTTServerAddress.usPort = SOCKETS_htons( pxEventData->u.pxConnectParams->usPort );
        }

        xMQTTServerAddress.ulAddress = SOCKETS_GetHostByName( pxEventData->u.pxConnectParams->pcURL );
        xMQTTServerAddress.ucSocketDomain = SOCKETS_AF_INET;

        /* Create the socket. */
        pxConnection->xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );

        if( pxConnection->xSocket != SOCKETS_INVALID_SOCKET )
        {
            /* Set a callback function that will unblock the MQTT task when data
             * is received on a socket. */
            ( void ) SOCKETS_SetSockOpt( pxConnection->xSocket,
                                         0,                                            /* Level - Unused. */
                                         SOCKETS_SO_WAKEUP_CALLBACK,
                                         ( void * ) prvMQTTClientSocketWakeupCallback, /*lint !e9087 !e9074 The cast is ok as we are setting the callback here. */
                                         sizeof( &( prvMQTTClientSocketWakeupCallback ) ) );

            /* Set secure socket option if it is a secured connection. */
            if( ( pxConnection->uxFlags & mqttCONNECTION_SECURED ) == mqttCONNECTION_SECURED )
            {
                if( SOCKETS_SetSockOpt( pxConnection->xSocket,
                                        0, /* Level - Unused. */
                                        SOCKETS_SO_REQUIRE_TLS,
                                        NULL,
                                        0 ) != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }

                /* If a certificate is supplied, set it. */
                if( ( xStatus == pdPASS ) &&
                    ( pxEventData->u.pxConnectParams->pcCertificate != NULL ) )
                {
                    if( SOCKETS_SetSockOpt( pxConnection->xSocket,
                                            0, /* Level - Unused. */
                                            SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                            pxEventData->u.pxConnectParams->pcCertificate,
                                            pxEventData->u.pxConnectParams->ulCertificateSize ) != SOCKETS_ERROR_NONE )
                    {
                        xStatus = pdFAIL;
                    }
                }

                /* Use SNI if the provided URL is not IP address. */
                if( ( xStatus == pdPASS ) &&
                    ( ( pxEventData->u.pxConnectParams->xFlags & mqttagentURL_IS_IP_ADDRESS ) == 0 ) &&
                    ( pxEventData->u.pxConnectParams->xURLIsIPAddress == pdFALSE ) )
                {
                    if( SOCKETS_SetSockOpt( pxConnection->xSocket,
                                            0, /* Level - Unused. */
                                            SOCKETS_SO_SERVER_NAME_INDICATION,
                                            pxEventData->u.pxConnectParams->pcURL,
                                            ( size_t ) 1 + xURLLength ) != SOCKETS_ERROR_NONE )
                    {
                        xStatus = pdFAIL;
                    }
                }

                /* Negotiate ALPN if requested. */
                if( ( xStatus == pdPASS ) &&
                    ( ( ( pxEventData->u.pxConnectParams->xFlags & mqttagentUSE_AWS_IOT_ALPN_443 ) ) != 0 ) )
                {
                    if( SOCKETS_SetSockOpt( pxConnection->xSocket,
                                            0, /* Level - Unused. */
                                            SOCKETS_SO_ALPN_PROTOCOLS,
                                            ppcAlpns,
                                            sizeof( ppcAlpns ) / sizeof( ppcAlpns[ 0 ] ) ) != SOCKETS_ERROR_NONE )
                    {
                        xStatus = pdFAIL;
                    }
                }
            }

            /* Establish the connection. */
            if( xStatus == pdPASS )
            {
                if( SOCKETS_Connect( pxConnection->xSocket, &xMQTTServerAddress, sizeof( xMQTTServerAddress ) ) != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }
            }

            if( xStatus == pdPASS )
            {
                /* Do not block now onwards. */
                ( void ) SOCKETS_SetSockOpt( pxConnection->xSocket,
                                             0 /* Unused. */,
                                             SOCKETS_SO_NONBLOCK,
                                             NULL /* Unused. */,
                                             0 /* Unused. */ );
            }
            else
            {
                /* Connection Failed. */
                prvGracefulSocketClose( pxConnection );
            }
        }
        else
        {
            xStatus = pdFAIL;
        }
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "Malformed URL %s.\r\n", pxEventData->u.pxConnectParams->pcURL ) );
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static void prvGracefulSocketClose( MQTTBrokerConnection_t * const pxConnection )
{
    const TickType_t xShortDelay = pdMS_TO_TICKS( 10 );
    TickType_t xTicksToWait = xShortDelay * ( TickType_t ) 100;
    TimeOut_t xTimeOut;

    mqttconfigDEBUG_LOG( ( "About to close socket.\r\n" ) );

    /* Initialize xTimeOut.  This records the time at which this function was
     * entered. */
    vTaskSetTimeOutState( &xTimeOut );

    /* Shutdown the connection. */
    ( void ) SOCKETS_Shutdown( pxConnection->xSocket, SOCKETS_SHUT_RDWR );

    /* Wait for the socket to disconnect gracefully (indicated by a
     * SOCKETS_ERRNO_EINVAL error) before closing the socket. */
    while( SOCKETS_Recv( pxConnection->xSocket, pxConnection->ucRxBuffer, sizeof( pxConnection->ucRxBuffer ), 0 ) >= 0 )
    {
        vTaskDelay( xShortDelay );

        if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
        {
            /* Timed out before the wanted number of bytes were available, exit
             * the loop. */
            break;
        }
    }

    /* Close the socket. */
    ( void ) SOCKETS_Close( pxConnection->xSocket );
    pxConnection->xSocket = SOCKETS_INVALID_SOCKET;
    mqttconfigDEBUG_LOG( ( "Socket closed.\r\n" ) );

    #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
        {
            /* Print the stack high watermark for the MQTT task. */
            mqttconfigDEBUG_LOG( ( "Stack high watermark for MQTT task: %u\r\n", uxTaskGetStackHighWaterMark( NULL ) ) );
        }
    #endif
}
/*-----------------------------------------------------------*/

static void prvMQTTClientSocketWakeupCallback( Socket_t pxSocket )
{
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 20 );
    MQTTEventData_t xEventData;

    /* Just to avoid compiler warnings.  The socket is not used but the function
     * prototype cannot be changed because this is a callback function. */
    ( void ) pxSocket;

    /* Should not be possible to get here without the task having been
     * created! */
    configASSERT( xMQTTTaskHandle );

    /* A socket used by the MQTT task may need attention.  Send an event
     * to the MQTT task to make sure the task is not blocked on xCommandQueue.
     * There is only any need to do this if there are no messages already in the
     * queue, as if there are, the task won't block anyway. */
    if( uxQueueMessagesWaiting( xCommandQueue ) == ( UBaseType_t ) 0 )
    {
        /* The eMQTTServiceSocket event is not handled directly, it is only used
         * to unblock the MQTT task, so only the xEventType needs to be set. */
        memset( &xEventData, 0x00, sizeof( MQTTEventData_t ) );
        xEventData.xEventType = eMQTTServiceSocket;
        mqttconfigDEBUG_LOG( ( "Socket sending wakeup to MQTT task.\r\n" ) );
        ( void ) xQueueSendToBack( xCommandQueue, &xEventData, xTicksToWait );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedCONNACK( MQTTBrokerConnection_t * const pxConnection,
                                       const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTNotificationData_t * pxNotificationData;

    /* Retrieve the notification data for the task which initiated the Connect operation.*/
    pxNotificationData = prvRetrieveNotificationData( pxConnection, pxParams->u.xMQTTConnACKData.usPacketIdentifier );

    /* If there is no task waiting for it, ignore it. */
    if( pxNotificationData != NULL )
    {
        if( pxParams->u.xMQTTConnACKData.xConnACKReturnCode == eMQTTConnACKConnectionAccepted )
        {
            mqttconfigDEBUG_LOG( ( "MQTT Connect was accepted. Connection established.\r\n" ) );
            prvNotifyRequestingTask( pxNotificationData, eMQTTCONNACKConnectionAccepted, pdPASS );
        }
        else
        {
            /* We must not unblock the task which initiated the connect
             * operation from here. The reason is that TCP socket is
             * still connected and if the task re-tries the connect
             * operation it will attempt to call connect on an already
             * connected socket. If the broker rejects the connection,
             * we get a disconnect from the core library also which is
             * when we unblock the task. This ensures the connect is not
             * re-tried until the socket is closed. */
            mqttconfigDEBUG_LOG( ( "MQTT Connect was rejected with error %d.\r\n", pxParams->u.xMQTTConnACKData.xConnACKReturnCode ) );
        }
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedSUBACK( MQTTBrokerConnection_t * const pxConnection,
                                      const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTNotificationData_t * pxNotificationData;

    /* Retrieve the notification data for the task which initiated the Subscribe operation.*/
    pxNotificationData = prvRetrieveNotificationData( pxConnection, pxParams->u.xMQTTSubACKData.usPacketIdentifier );

    /* If there is no task waiting for it, ignore it. */
    if( pxNotificationData != NULL )
    {
        if( pxParams->u.xMQTTSubACKData.xSubACKReturnCode != eMQTTSubACKFailure )
        {
            mqttconfigDEBUG_LOG( ( "MQTT Subscribe was accepted. Subscribed.\r\n" ) );
            prvNotifyRequestingTask( pxNotificationData, eMQTTSUBACKSubscriptionAccepted, pdPASS );
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "MQTT Subscribe was rejected.\r\n" ) );
            prvNotifyRequestingTask( pxNotificationData, eMQTTSUBACKSubscriptionRejected, pdFAIL );
        }
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedUNSUBACK( MQTTBrokerConnection_t * const pxConnection,
                                        const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTNotificationData_t * pxNotificationData;

    /* Retrieve the notification data for the task which initiated the Unsubscribe operation.*/
    pxNotificationData = prvRetrieveNotificationData( pxConnection, pxParams->u.xMQTTUnSubACKData.usPacketIdentifier );

    /* If there is no task waiting for it, ignore it. */
    if( pxNotificationData != NULL )
    {
        /* Otherwise inform the task. */
        mqttconfigDEBUG_LOG( ( "MQTT Unsubscribe was successful.\r\n" ) );
        prvNotifyRequestingTask( pxNotificationData, eMQTTUNSUBACKReceived, pdPASS );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedPUBACK( MQTTBrokerConnection_t * const pxConnection,
                                      const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTNotificationData_t * pxNotificationData;

    /* Retrieve the notification data for the task which initiated the Publish operation.*/
    pxNotificationData = prvRetrieveNotificationData( pxConnection, pxParams->u.xMQTTPubACKData.usPacketIdentifier );

    /* If there is no task waiting for it, ignore it. */
    if( pxNotificationData != NULL )
    {
        /* Otherwise inform the task. */
        mqttconfigDEBUG_LOG( ( "MQTT Publish was successful.\r\n" ) );
        prvNotifyRequestingTask( pxNotificationData, eMQTTPUBACKReceived, pdPASS );
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvProcessReceivedPublish( MQTTBrokerConnection_t * const pxConnection,
                                             const MQTTEventCallbackParams_t * const pxParams )
{
    BaseType_t xReturn = pdFALSE;
    MQTTAgentCallbackParams_t xCallbackParams;

    /* Either the user has not enabled subscription management, or the
     * publish message was received on a topic for which the user has not
     * registered any topic specific callback. If the user has registered
     * a generic callback, invoke it otherwise the user is not interested
     * in the messages and therefore ignore it. */
    if( pxConnection->pxCallback != NULL )
    {
        xCallbackParams.xMQTTEvent = eMQTTAgentPublish;
        xCallbackParams.u.xPublishData = pxParams->u.xPublishData;

        xReturn = pxConnection->pxCallback( pxConnection->pvUserData, &( xCallbackParams ) );
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedTimeout( MQTTBrokerConnection_t * const pxConnection,
                                       const MQTTEventCallbackParams_t * const pxParams )
{
    MQTTNotificationData_t * pxNotificationData;

    /* Try to see if there is a task waiting for the operation which just timed out. */
    pxNotificationData = prvRetrieveNotificationData( pxConnection, pxParams->u.xTimeoutData.usPacketIdentifier );

    /* If there is no task waiting, just ignore. Otherwise
     * inform the task about the timeout. */
    if( pxNotificationData != NULL )
    {
        mqttconfigDEBUG_LOG( ( "MQTT Timeout.\r\n" ) );
        prvNotifyRequestingTask( pxNotificationData, eMQTTOperationTimedOut, pdFAIL );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedDisconnect( MQTTBrokerConnection_t * const pxConnection,
                                          const MQTTEventCallbackParams_t * const pxParams )
{
    UBaseType_t x;
    MQTTAgentCallbackParams_t xCallbackParams;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pxParams;

    /* Only process the disconnect event if the client is connected. */
    if( pxConnection->xSocket != SOCKETS_INVALID_SOCKET )
    {
        /* Inform the user about the disconnect, if a callback is registered. */
        if( pxConnection->pxCallback != NULL )
        {
            xCallbackParams.xMQTTEvent = eMQTTAgentDisconnect;
            ( void ) pxConnection->pxCallback( pxConnection->pvUserData, &( xCallbackParams ) );
        }

        /* Close the connection. */
        prvGracefulSocketClose( pxConnection );
    }

    /* Unblock all the tasks which are waiting for ACKs because
     * we won't get any ACK after getting disconnected. Note that
     * if a user calls MQTT_AGENT_Disconnect, we do not store
     * the notification data (see prvInitiateMQTTDisconnect) and
     * therefore we won't unblock the task from here. Instead the
     * task calling MQTT_AGENT_Disconnect gets unblocked from
     * prvInitiateMQTTDisconnect and therefore receives a success
     * return code. */
    for( x = 0; x < ( UBaseType_t ) mqttconfigMAX_PARALLEL_OPS; x++ )
    {
        if( pxConnection->xWaitingTasks[ x ].xTaskToNotify != NULL )
        {
            prvNotifyRequestingTask( &( pxConnection->xWaitingTasks[ x ] ),
                                     eMQTTClientGotDisconnected,
                                     pdFAIL );
        }
    }
}
/*-----------------------------------------------------------*/

static void prvNotifyRequestingTask( MQTTNotificationData_t * const pxNotificationData,
                                     MQTTNotifyCodes_t xNotificationCode,
                                     UBaseType_t uxStatus )
{
    if( pxNotificationData->xTaskToNotify != NULL )
    {
        /* ulMessageIdentifier only uses the top 16-bits.  The status code uses
         * the low 16-bits, of which the least significant bit is used to indicate
         * if the operation passed or failed. */
        mqttconfigDEBUG_LOG( ( "Notifying task.\r\n" ) );

        /* Encode the notification code and status in the notification value. */
        pxNotificationData->ulMessageIdentifier |= ( UBaseType_t ) xNotificationCode;
        pxNotificationData->ulMessageIdentifier |= uxStatus;

        /* Notify the task. */
        ( void ) xTaskNotify( pxNotificationData->xTaskToNotify, pxNotificationData->ulMessageIdentifier, eSetValueWithoutOverwrite );

        /* Free up the buffer for further use. */
        pxNotificationData->xTaskToNotify = NULL;
    }
}
/*-----------------------------------------------------------*/

static TickType_t prvManageConnections( void )
{
    UBaseType_t uxBrokerNumber;
    MQTTBrokerConnection_t * pxConnection;
    BaseType_t xAnyConnectedClient = pdFALSE;
    int32_t lBytesReceived;
    TickType_t xNextMQTTPeriodicInvokeTicks, xNextTimeoutTicks = portMAX_DELAY;
    uint64_t xTickCount = 0;

    /* For each broker the MQTT task might be connected to. */
    for( uxBrokerNumber = 0; uxBrokerNumber < ( UBaseType_t ) mqttconfigMAX_BROKERS; uxBrokerNumber++ )
    {
        pxConnection = &( xMQTTConnections[ uxBrokerNumber ] );

        /* Process only the connected clients. */
        if( pxConnection->xSocket != SOCKETS_INVALID_SOCKET )
        {
            /* Read data from the socket. */
            lBytesReceived = SOCKETS_Recv( pxConnection->xSocket, pxConnection->ucRxBuffer, mqttconfigRX_BUFFER_SIZE, 0 );

            /* If data was read, pass it to the MQTT Core library. */
            if( lBytesReceived > 0 )
            {
                ( void ) MQTT_ParseReceivedData( &( pxConnection->xMQTTContext ), pxConnection->ucRxBuffer, ( size_t ) lBytesReceived );

                /* Some data was received on this socket and we do not
                 * know if there is more data available. Therefore we
                 * set xNextTimeoutTicks to zero which ensures that we
                 * do not block on the command queue and try to read
                 * again from this socket on the next invocation of
                 * prvManageConnections. This way we ensure that we keep
                 * processing commands received on the command queue
                 * between calls to SOCKETS_Recv. As a result, a socket
                 * receiving lots of data continuously does not starve
                 * the command processing. */
                xNextTimeoutTicks = 0;
            }
            else if( lBytesReceived < 0 )
            {
                /* A negative return value from SOCKETS_Recv indicates error.
                 * Since the socket is marked non-blocking, read can potentially
                 * return SOCKETS_EWOULDBLOCK in which case we will re-try to
                 * read on the next execution of this function. In case of any
                 * other error, we disconnect. */
                if( lBytesReceived != SOCKETS_EWOULDBLOCK )
                {
                    /* Disconnect from the broker. Note that the socket close
                     * and cleanup will happen in the disconnect callback
                     * ( prvProcessReceivedDisconnect function ) from the core
                     * MQTT library. */
                    ( void ) MQTT_Disconnect( &( pxConnection->xMQTTContext ) );
                }
            }
            else
            {
                /* If no data was received on this socket, we continue
                 * to call MQTT_Periodic and calculate xNextTimeoutTicks
                 * accordingly. */
            }
        }

        /* Is the client connected? */
        if( ( xAnyConnectedClient == pdFALSE ) && ( pxConnection->xSocket != SOCKETS_INVALID_SOCKET ) )
        {
            xAnyConnectedClient = pdTRUE;
        }

        /* Get the current tick count. */
        prvMQTTGetTicks( &xTickCount );

        /* Invoke MQTT_Periodic. */
        xNextMQTTPeriodicInvokeTicks = ( TickType_t ) MQTT_Periodic( &( pxConnection->xMQTTContext ), xTickCount );

        /* Update the next timeout value. */
        xNextTimeoutTicks = configMIN( xNextTimeoutTicks, xNextMQTTPeriodicInvokeTicks );
    }

    /* The MQTT task must not block for more than mqttconfigMQTT_TASK_MAX_BLOCK_TICKS
     * ticks if any client is connected. */
    if( xAnyConnectedClient == pdTRUE )
    {
        xNextTimeoutTicks = configMIN( xNextTimeoutTicks, ( TickType_t ) mqttconfigMQTT_TASK_MAX_BLOCK_TICKS );
    }

    /* The return value indicates when the MQTT task should wake up next. */
    return xNextTimeoutTicks;
}
/*-----------------------------------------------------------*/

static void prvInitiateMQTTConnect( MQTTEventData_t * const pxEventData )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData;
    MQTTConnectParams_t xConnectParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* Store notification data. */
    pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );

    /* If a free buffer was not available to store the  notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. */
    if( pxNotificationData != NULL )
    {
        /* Store the user data and callback. */
        pxConnection->pvUserData = pxEventData->u.pxConnectParams->pvUserData;
        pxConnection->pxCallback = pxEventData->u.pxConnectParams->pxCallback;

        /* Check if the connection is to be secured. */
        if( ( pxEventData->u.pxConnectParams->xSecuredConnection == pdFALSE ) &&
            ( ( pxEventData->u.pxConnectParams->xFlags & mqttagentREQUIRE_TLS ) == 0 ) )
        {
            pxConnection->uxFlags &= ~mqttCONNECTION_SECURED;
        }
        else
        {
            pxConnection->uxFlags |= mqttCONNECTION_SECURED;
        }

        /* Establish the TCP connection. */
        xStatus = prvSetupConnection( pxEventData );

        if( xStatus == pdPASS )
        {
            #if ( mqttconfigENABLE_METRICS == 1 )
                mqttconfigDEBUG_LOG( ( "Anonymous metrics will be collected. Recompile with"
                                "mqttconfigENABLE_METRICS set to 0 to disable.\r\n" ) );
            #endif

            /* Setup connect parameters and call the Core library connect function. */
            xConnectParams.pucClientId = pxEventData->u.pxConnectParams->pucClientId;
            xConnectParams.usClientIdLength = pxEventData->u.pxConnectParams->usClientIdLength;

            if(pxEventData->u.pxConnectParams->uUsernamelength == 0)
            {
                xConnectParams.pucUserName = ( const uint8_t * ) cUserName;
                xConnectParams.usUserNameLength = usUserNameLength;
            }
            else
            {
                xConnectParams.pucUserName = ( const uint8_t * ) pxEventData->u.pxConnectParams->cUserName;
                xConnectParams.usUserNameLength = pxEventData->u.pxConnectParams->uUsernamelength;
            }

            xConnectParams.pucPassword = pxEventData->u.pxConnectParams->p_password;
            xConnectParams.usPasswordLength = pxEventData->u.pxConnectParams->passwordlength;
            xConnectParams.usKeepAliveIntervalSeconds = mqttconfigKEEP_ALIVE_INTERVAL_SECONDS;
            xConnectParams.ulKeepAliveActualIntervalTicks = mqttconfigKEEP_ALIVE_ACTUAL_INTERVAL_TICKS;
            xConnectParams.ulPingRequestTimeoutTicks = mqttconfigKEEP_ALIVE_TIMEOUT_TICKS;
            xConnectParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
            xConnectParams.ulTimeoutTicks = pxEventData->xTicksToWait;

            if( MQTT_Connect( &( pxConnection->xMQTTContext ), &( xConnectParams ) ) != eMQTTSuccess )
            {
                mqttconfigDEBUG_LOG( ( "MQTT_Connect failed!\r\n" ) );

                /* The TCP connection was successful but we failed to send
                 * the MQTT Connect message. This could happen because of
                 * multiple reasons like a free buffer from the buffer pool
                 * was not available to construct the MQTT Connect message
                 * or the network send failed. The TCP Connection must be
                 * closed in this case to avoid leaking sockets. */
                prvGracefulSocketClose( pxConnection );

                /* Set the status to fail. */
                xStatus = pdFAIL;
            }
        }
    }

    if( xStatus == pdFAIL )
    {
        /* The Connect request was never sent.  Inform the task that initiated
         * the Connect operation. */
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTCONNCouldNotBeSent, pdFAIL );

        /* If a buffer was used to store notification data, return it. */
        if( pxNotificationData != NULL )
        {
            pxNotificationData->xTaskToNotify = NULL;
        }
    }
}
/*-----------------------------------------------------------*/

static void prvInitiateMQTTDisconnect( MQTTEventData_t * const pxEventData )
{
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* There is no need to store notification data as we will
     * not get any ACK. Try to send MQTT Disconnect message and
     * inform the application task about the result. Note that
     * the socket is closed when we receive disconnect event from
     * the MQTT core library in the registered callback (see
     * prvProcessReceivedDisconnect). */
    if( MQTT_Disconnect( &( pxConnection->xMQTTContext ) ) == eMQTTSuccess )
    {
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTDISCONNSent, pdPASS );
    }
    else
    {
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTDISCONNCouldNotBeSent, pdFAIL );
    }
}
/*-----------------------------------------------------------*/

static void prvInitiateMQTTSubscribe( MQTTEventData_t * const pxEventData )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData;
    MQTTSubscribeParams_t xSubscribeParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* Store notification data. */
    pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );

    /* If a free buffer was not available to store the	notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. */
    if( pxNotificationData != NULL )
    {
        /* Setup subscribe parameters and call the Core library subscribe function. */
        xSubscribeParams.pucTopic = pxEventData->u.pxSubscribeParams->pucTopic;
        xSubscribeParams.usTopicLength = pxEventData->u.pxSubscribeParams->usTopicLength;
        xSubscribeParams.xQos = pxEventData->u.pxSubscribeParams->xQoS;
        xSubscribeParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
        xSubscribeParams.ulTimeoutTicks = pxEventData->xTicksToWait;
        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
            xSubscribeParams.pvPublishCallbackContext = pxEventData->u.pxSubscribeParams->pvPublishCallbackContext;
            xSubscribeParams.pxPublishCallback = pxEventData->u.pxSubscribeParams->pxPublishCallback;
        #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

        if( MQTT_Subscribe( &( pxConnection->xMQTTContext ), &( xSubscribeParams ) ) == eMQTTSuccess )
        {
            xStatus = pdPASS;
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "MQTT_Subscribe failed!\r\n" ) );
        }
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "Could not get a buffer to store notification data. Too many parallel tasks!\r\n" ) );
    }

    if( xStatus == pdFAIL )
    {
        /* The Subscribe request was never sent.  Inform the task that initiated
         * the Subscribe operation. */
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTSUBCouldNotBeSent, pdFAIL );

        /* If a buffer was used to store notification data, return it. */
        if( pxNotificationData != NULL )
        {
            pxNotificationData->xTaskToNotify = NULL;
        }
    }
}
/*-----------------------------------------------------------*/

static void prvInitiateMQTTUnSubscribe( MQTTEventData_t * const pxEventData )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData;
    MQTTUnsubscribeParams_t xUnsubscribeParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* Store notification data. */
    pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );

    /* If a free buffer was not available to store the	notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. */
    if( pxNotificationData != NULL )
    {
        /* Setup unsubscribe parameters and call the Core library unsubscribe
         * function. */
        xUnsubscribeParams.pucTopic = pxEventData->u.pxUnsubscribeParams->pucTopic;
        xUnsubscribeParams.usTopicLength = pxEventData->u.pxUnsubscribeParams->usTopicLength;
        xUnsubscribeParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
        xUnsubscribeParams.ulTimeoutTicks = pxEventData->xTicksToWait;

        if( MQTT_Unsubscribe( &( pxConnection->xMQTTContext ), &( xUnsubscribeParams ) ) == eMQTTSuccess )
        {
            xStatus = pdPASS;
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "MQTT_Unsubscribe failed!\r\n" ) );
        }
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "Could not get a buffer to store notification data. Too many parallel tasks!\r\n" ) );
    }

    if( xStatus == pdFAIL )
    {
        /* The Unsubscribe request was never sent.  Inform the task that initiated
         * the Unsubscribe operation. */
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTUNSUBCouldNotBeSent, pdFAIL );

        /* If a buffer was used to store notification data, return it. */
        if( pxNotificationData != NULL )
        {
            pxNotificationData->xTaskToNotify = NULL;
        }
    }
}
/*-----------------------------------------------------------*/

static void prvInitiateMQTTPublish( MQTTEventData_t * const pxEventData )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData = NULL;
    MQTTPublishParams_t xPublishParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* No need to store  notification data in case of QoS0 because
     * there will not be any ACK. */
    if( pxEventData->u.pxPublishParams->xQoS != eMQTTQoS0 )
    {
        pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );
    }

    /* If a free buffer was not available to store the notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. We don't store notification data in case of QoS0, so
     * proceed anyways. */
    if( ( pxNotificationData != NULL ) || ( pxEventData->u.pxPublishParams->xQoS == eMQTTQoS0 ) )
    {
        /* Setup publish parameters and call the Core library publish function. */
        xPublishParams.pucTopic = pxEventData->u.pxPublishParams->pucTopic;
        xPublishParams.usTopicLength = pxEventData->u.pxPublishParams->usTopicLength;
        xPublishParams.xQos = pxEventData->u.pxPublishParams->xQoS;
        xPublishParams.pvData = pxEventData->u.pxPublishParams->pvData;
        xPublishParams.ulDataLength = pxEventData->u.pxPublishParams->ulDataLength;
        xPublishParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
        xPublishParams.ulTimeoutTicks = pxEventData->xTicksToWait;

        if( MQTT_Publish( &( pxConnection->xMQTTContext ), &( xPublishParams ) ) == eMQTTSuccess )
        {
            xStatus = pdPASS;
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "MQTT_Publish failed!\r\n" ) );
        }
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "Could not get a buffer to store notification data. Too many parallel tasks!\r\n" ) );
    }

    /* In case of QoS0 successful publish, inform and unblock the task that
     * initiated the publish operation as no PUBACK is expected. */
    if( ( xStatus == pdPASS ) && ( pxEventData->u.pxPublishParams->xQoS == eMQTTQoS0 ) )
    {
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTPUBSent, pdPASS );
    }

    if( xStatus == pdFAIL )
    {
        /* The Publish was not successful.  Inform the task that initiated
         * the Publish operation. */
        prvNotifyRequestingTask( &( pxEventData->xNotificationData ), eMQTTPUBCouldNotBeSent, pdFAIL );

        /* If a buffer was used to store notification data, return it. */
        if( pxNotificationData != NULL )
        {
            pxNotificationData->xTaskToNotify = NULL;
        }
    }
}
/*-----------------------------------------------------------*/

static MQTTAgentReturnCode_t prvSendCommandToMQTTTask( MQTTEventData_t * pxEventData )
{
    BaseType_t xReturn;
    MQTTAgentReturnCode_t xReturnCode = eMQTTAgentFailure;
    uint32_t ulReceivedMessageIdentifier;

    /* Should not try to send commands until after the MQTT task has been
     * initialized, in which case the command queue will have been created. */
    configASSERT( xCommandQueue );

    /* Setup notification data. */
    pxEventData->xNotificationData.xTaskToNotify = xTaskGetCurrentTaskHandle();

    /* Commands must not be sent from the MQTT task itself (which could be
     * the case if a command is sent from a callback function).  Otherwise
     * there is the possibility that the task could end up waiting for itself
     * resulting in deadlock. */
    if( pxEventData->xNotificationData.xTaskToNotify != xMQTTTaskHandle )
    {
        taskENTER_CRITICAL();
        {
            /* The message identifier is used to know which message is being
             * acknowledged.  A critical region is used as a single message identifier
             * variable is used by all connections. The identifier uses the top 16-bits
             * of the 32-bit word, leaving the lowest 16-bits free for use by the MQTT
             * task to return a status code. */
            pxEventData->xNotificationData.ulMessageIdentifier = ulQueueMessageIdentifier;
            ulQueueMessageIdentifier += mqttMESSAGE_IDENTIFIER_MIN;

            if( ulQueueMessageIdentifier >= mqttMESSAGE_IDENTIFIER_MAX )
            {
                ulQueueMessageIdentifier = mqttMESSAGE_IDENTIFIER_MIN;
            }
        }
        taskEXIT_CRITICAL();

        /* Record the time at which this event is created. */
        vTaskSetTimeOutState( &( pxEventData->xEventCreationTimestamp ) );

        /* The calling task is going to wait for a notification, so clear the
         * notifications state first.  This is probably not necessary as the task will
         * wait for a particular notification value, but is for maximum robustness. */
        ( void ) xTaskNotifyStateClear( NULL );

        /* The MQTT protocol is running in a separate task, to which commands
         * are sent on a queue, and a signal is sent back using a task
         * notification. */
        mqttconfigDEBUG_LOG( ( "Sending command to MQTT task.\r\n" ) );
        xReturn = xQueueSendToBack( xCommandQueue, pxEventData, pxEventData->xTicksToWait );

        if( xReturn != pdFALSE )
        {
            /* Ensure ulReceivedMessageIdentifier does not accidentally equal
             * xEventData.ulMessageIdentifier as it will be checked to see if the
             * message being acknowledged is that just sent. */
            ulReceivedMessageIdentifier = ~( pxEventData->xNotificationData.ulMessageIdentifier );

            for( ; ; )
            {
                /* The message identifier is used as the notification value so it is
                 * known which message is being acknowledged (in case the message is
                 * one this task previously didn't wait long enough to receive). We
                 * block here forever and rely on the notification from the MQTT task
                 * to unblock us. Return value is ignored because in case of portMAX_DELAY
                 * the function will return only when a notification is received. */
                ( void ) xTaskNotifyWait( 0UL, 0UL, &ulReceivedMessageIdentifier, portMAX_DELAY );

                if( pxEventData->xNotificationData.ulMessageIdentifier == ( ulReceivedMessageIdentifier & mqttMESSAGE_IDENTIFIER_MASK ) )
                {
                    /* A reply to the message was received.  The low 16-bits
                     * contain a status code, of which the least significant it
                     * is 1 (pdPASS) if the status code indicates a pass, and
                     * 0 (pdFAIL) if the status code indicates a fail. */

                    if( ( ulReceivedMessageIdentifier & mqttNOTIFICATION_STATUS_MASK ) != ( uint32_t ) pdPASS )
                    {
                        /* A reply to the message was received, but the operation
                         * failed. Check if the failure reason was timeout. */
                        if( ( ulReceivedMessageIdentifier & mqttNOTIFICATION_CODE_MASK ) == ( uint32_t ) eMQTTOperationTimedOut )
                        {
                            xReturnCode = eMQTTAgentTimeout;
                        }

                        mqttconfigDEBUG_LOG( ( "Command sent to MQTT task failed.\r\n" ) );
                    }
                    else
                    {
                        /* A reply to the message was received and the operation
                         * passed. */
                        mqttconfigDEBUG_LOG( ( "Command sent to MQTT task passed.\r\n" ) );
                        xReturnCode = eMQTTAgentSuccess;
                    }

                    break;
                }
                else
                {
                    /* Don't know what the notification was from, keep waiting. */
                    mqttconfigDEBUG_LOG( ( "Unexpected notification received.\r\n" ) );
                }
            }
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "Attempt to write to the MQTT command queue failed.\r\n" ) );
        }
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "MQTT Agent API called from MQTT task ( possibly from callback ) !!.\r\n" ) );
        xReturnCode = eMQTTAgentAPICalledFromCallback;
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

static void prvMQTTTask( void * pvParameters )
{
    MQTTEventData_t xMQTTCommand;
    TickType_t xNextTimeoutTicks = 0;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    for( ; ; )
    {
        if( xQueueReceive( xCommandQueue, &xMQTTCommand, xNextTimeoutTicks ) != pdFALSE )
        {
            mqttconfigDEBUG_LOG( ( "Received message %x from queue.\r\n", xMQTTCommand.xNotificationData.ulMessageIdentifier ) );

            /* The connection index identifies the broker to communicate with -
             * starting from an index of 0.  Check the index is valid here so
             * functions further down the call tree don't have to.  A check is
             * performed before messages are sent to the command queue anyway. */
            configASSERT( xMQTTCommand.uxBrokerNumber < ( UBaseType_t ) mqttconfigMAX_BROKERS );

            /* Check if the timeout for the event has been reached.
             * It means that the MQTT task picked up this command for
             * processing too late and there is no point in proceeding.
             * Fail the operation with timeout and unblock the waiting
             * task. */
            if( xTaskCheckForTimeOut( &( xMQTTCommand.xEventCreationTimestamp ), &( xMQTTCommand.xTicksToWait ) ) == pdTRUE )
            {
                /* Note that in case of eMQTTServiceSocket event, the
                 * xMQTTCommand.xNotificationData.xTaskToNotify happens to
                 * be NULL and therefore prvNotifyRequestingTask returns
                 * without doing anything. */
                prvNotifyRequestingTask( &( xMQTTCommand.xNotificationData ), eMQTTOperationTimedOut, pdFAIL );
            }
            else
            {
                /* Process the received command. Note that the xTicksToWait
                 * has been updated in the previous call to xTaskCheckForTimeout
                 * to ensure that we block only for the duration specified by the
                 * user. */
                switch( xMQTTCommand.xEventType )
                {
                    case eMQTTConnectRequest:
                        prvInitiateMQTTConnect( &( xMQTTCommand ) );
                        break;

                    case eMQTTDisconnectRequest:
                        prvInitiateMQTTDisconnect( &( xMQTTCommand ) );
                        break;

                    case eMQTTSubscribeRequest:
                        prvInitiateMQTTSubscribe( &( xMQTTCommand ) );
                        break;

                    case eMQTTUnsubscribeRequest:
                        prvInitiateMQTTUnSubscribe( &( xMQTTCommand ) );
                        break;

                    case eMQTTPublishRequest:
                        prvInitiateMQTTPublish( &( xMQTTCommand ) );
                        break;

                    default:
                        /* Anything else is illegal. */
                        mqttconfigDEBUG_LOG( ( "Unknown request received on command queue.\r\n" ) );
                        break;
                }
            }
        }

        /* Process active connections each time the queue unblocks.  It might
         * be that the queue read timed out because a connection needs service. */
        xNextTimeoutTicks = prvManageConnections();
    }
}
/*-----------------------------------------------------------*/

BaseType_t MQTT_AGENT_Init( void )
{
    /* The following variables must be static as they hold data that is used as
     * long as the MQTT application is running. */

    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xStaticQueue;

    /* The array to use as the queue's storage area.  This must be at least
     * uxQueueLength * uxItemSize bytes.  Again, must be static. */
    static uint8_t ucQueueStorageArea[ mqttCOMMAND_QUEUE_LENGTH * sizeof( MQTTEventData_t ) ];

    /* The stack used by the MQTT task. */
    static StackType_t xStack[ mqttconfigMQTT_TASK_STACK_DEPTH ];

    /* The variable used to hold the MQTT task's data structures. */
    static StaticTask_t xStaticTask;

    BaseType_t xReturnCode = pdPASS;
    UBaseType_t x, y;

    /* If the command queue is not NULL then the queue and task have already
     * been created. */
    if( xCommandQueue == NULL )
    {
        /* Ensure the connection structures start in a consistent state. */
        memset( xMQTTConnections, 0x00, sizeof( xMQTTConnections ) );

        for( x = 0; x < ( UBaseType_t ) mqttconfigMAX_BROKERS; x++ )
        {
            xMQTTConnections[ x ].xSocket = SOCKETS_INVALID_SOCKET;

            /* Initialize user callback and data. These are provided during connect. */
            xMQTTConnections[ x ].pvUserData = NULL;
            xMQTTConnections[ x ].pxCallback = NULL;

            /* Mark the connection "not in use". */
            xMQTTConnections[ x ].xConnectionInUse = pdFALSE;

            /* Initialize the MQTT Core Library context. */
            MQTTInitParams_t xInitParams;
            xInitParams.pvCallbackContext = ( void * ) x; /*lint !e923 The cast is ok as we are passing the index of the client. */
            xInitParams.pxCallback = prvMQTTEventCallback;
            xInitParams.pvSendContext = ( void * ) x;     /*lint !e923 The cast is ok as we are passing the index of the client. */
            xInitParams.pxMQTTSendFxn = prvMQTTSendCallback;
            xInitParams.pxGetTicksFxn = prvMQTTGetTicks;
            xInitParams.xBufferPoolInterface.pxGetBufferFxn = mqttconfigGET_FREE_BUFFER_FXN;
            xInitParams.xBufferPoolInterface.pxReturnBufferFxn = mqttconfigRETURN_BUFFER_FXN;

            if( MQTT_Init( &xMQTTConnections[ x ].xMQTTContext, &xInitParams ) != eMQTTSuccess )
            {
                xReturnCode = pdFAIL;
                break;
            }

            /* Initialize waiting tasks list. */
            for( y = 0; y < ( UBaseType_t ) mqttconfigMAX_PARALLEL_OPS; y++ )
            {
                xMQTTConnections[ x ].xWaitingTasks[ y ].xTaskToNotify = NULL;
                xMQTTConnections[ x ].xWaitingTasks[ y ].ulMessageIdentifier = 0;
            }
        }

        /* ulQueueMessageIdentifier uses the top 16-bits of a 32-bit value, so
         * initialize it to its start value. */
        ulQueueMessageIdentifier = mqttMESSAGE_IDENTIFIER_MIN;

        /* Don't create the MQTT task until the command queue has been created,
         * as the task itself assumes the queue is valid. */
        xCommandQueue = xQueueCreateStatic( mqttCOMMAND_QUEUE_LENGTH, sizeof( MQTTEventData_t ), ucQueueStorageArea, &xStaticQueue );
        configASSERT( xCommandQueue );

        xMQTTTaskHandle = xTaskCreateStatic( prvMQTTTask, "MQTT", mqttconfigMQTT_TASK_STACK_DEPTH, NULL, mqttconfigMQTT_TASK_PRIORITY, xStack, &xStaticTask );
        configASSERT( xMQTTTaskHandle );
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Create( MQTTAgentHandle_t * const pxMQTTHandle )
{
    BaseType_t xBrokerNumber;
    UBaseType_t xEncodedBrokerNumber;
    MQTTAgentReturnCode_t xReturnCode = eMQTTAgentFailure;

    /* Try to get a free MQTTBrokerConnection_t. */
    xBrokerNumber = prvGetFreeConnection();

    /* If we cannot get a free connection, fail immediately. */
    if( xBrokerNumber >= 0 )
    {
        /* Encode the broker number. */
        xEncodedBrokerNumber = mqttENCODE_BROKER_NUMBER( xBrokerNumber );

        /* Return the encoded broker number as the opaque
         * handle to the user. */
        *pxMQTTHandle = ( MQTTAgentHandle_t ) ( xEncodedBrokerNumber ); /*lint !e923 Opaque pointer. */

        /* The create operation is successful. */
        xReturnCode = eMQTTAgentSuccess;
    }
    else
    {
        mqttconfigDEBUG_LOG( ( "No free MQTTBrokerConnection_t struct available!.\r\n" ) );
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Delete( MQTTAgentHandle_t xMQTTHandle )
{
    const UBaseType_t uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */

    /* Return the connection to the free connection pool. */
    prvReturnConnection( uxBrokerNumber );

    return eMQTTAgentSuccess;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Connect( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentConnectParams_t * const pxConnectParams,
                                          TickType_t xTimeoutTicks )
{
    MQTTEventData_t xEventData;
    MQTTAgentReturnCode_t xReturnCode;
    const UBaseType_t uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */

    /* Setup the event to be sent to the command queue. */
    xEventData.uxBrokerNumber = uxBrokerNumber;
    xEventData.xEventType = eMQTTConnectRequest;
    xEventData.xTicksToWait = xTimeoutTicks;
    xEventData.u.pxConnectParams = pxConnectParams;

    /* Note that the notification data part of xEventData and
     * xEventCreationTimestamp are set in the following call. */
    xReturnCode = prvSendCommandToMQTTTask( &xEventData );

    /* Return the code to the user. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Disconnect( MQTTAgentHandle_t xMQTTHandle,
                                             TickType_t xTimeoutTicks )
{
    MQTTEventData_t xEventData;
    MQTTAgentReturnCode_t xReturnCode;

    /* Setup the event to be sent to the command queue. */
    xEventData.uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */
    xEventData.xEventType = eMQTTDisconnectRequest;
    xEventData.xTicksToWait = xTimeoutTicks;

    /* Note that the notification data part of xEventData and
     * xEventCreationTimestamp are set in the following call. */
    xReturnCode = prvSendCommandToMQTTTask( &xEventData );

    /* Return the code to the user. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Subscribe( MQTTAgentHandle_t xMQTTHandle,
                                            const MQTTAgentSubscribeParams_t * const pxSubscribeParams,
                                            TickType_t xTimeoutTicks )
{
    MQTTEventData_t xEventData;
    MQTTAgentReturnCode_t xReturnCode;

    /* Setup the event to be sent to the command queue. */
    xEventData.uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */
    xEventData.xEventType = eMQTTSubscribeRequest;
    xEventData.xTicksToWait = xTimeoutTicks;
    xEventData.u.pxSubscribeParams = pxSubscribeParams;

    /* Note that the notification data part of xEventData and
     * xEventCreationTimestamp are set in the following call. */
    xReturnCode = prvSendCommandToMQTTTask( &xEventData );

    /* Return the code to the user. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Unsubscribe( MQTTAgentHandle_t xMQTTHandle,
                                              const MQTTAgentUnsubscribeParams_t * const pxUnsubscribeParams,
                                              TickType_t xTimeoutTicks )
{
    MQTTEventData_t xEventData;
    MQTTAgentReturnCode_t xReturnCode;

    /* Setup the event to be sent to the command queue. */
    xEventData.uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */
    xEventData.xEventType = eMQTTUnsubscribeRequest;
    xEventData.xTicksToWait = xTimeoutTicks;
    xEventData.u.pxUnsubscribeParams = pxUnsubscribeParams;

    /* Note that the notification data part of xEventData and
     * xEventCreationTimestamp are set in the following call. */
    xReturnCode = prvSendCommandToMQTTTask( &xEventData );

    /* Return the code to the user. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Publish( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentPublishParams_t * const pxPublishParams,
                                          TickType_t xTimeoutTicks )
{
    MQTTEventData_t xEventData;
    MQTTAgentReturnCode_t xReturnCode;

    /* Setup the event to be sent to the command queue. */
    xEventData.uxBrokerNumber = ( UBaseType_t ) mqttDECODE_BROKER_NUMBER( xMQTTHandle ); /*lint !e923 Opaque pointer. */
    xEventData.xEventType = eMQTTPublishRequest;
    xEventData.xTicksToWait = xTimeoutTicks;
    xEventData.u.pxPublishParams = pxPublishParams;

    /* Note that the notification data part of xEventData and
     * xEventCreationTimestamp are set in the following call. */
    xReturnCode = prvSendCommandToMQTTTask( &xEventData );

    /* Return the code to the user. */
    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_ReturnBuffer( MQTTAgentHandle_t xMQTTHandle,
                                               MQTTBufferHandle_t xBufferHandle )
{
    /* Remove compiler warnings about unused parameters. */
    ( void ) xMQTTHandle;

    /* Return the buffer to the free buffer pool. Since this function
     * gets called from application tasks, we must use thread safe implementation
     * of the buffer pool. */
    mqttconfigRETURN_BUFFER_FXN( mqttbufferGET_RAW_BUFFER_FROM_HANDLE( xBufferHandle ) );

    /* Return success. */
    return eMQTTAgentSuccess;
}
/*-----------------------------------------------------------*/
