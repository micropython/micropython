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
 * @file aws_mqtt_agent_config_defaults.h
 * @brief MQTT agent default config options.
 *
 * Ensures that the config options for MQTT agent are set to sensible
 * default values if the user does not provide one.
 */

#ifndef _AWS_MQTT_AGENT_CONFIG_DEFAULTS_H_
#define _AWS_MQTT_AGENT_CONFIG_DEFAULTS_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Controls whether or not to report usage metrics to the
 * AWS IoT broker.
 *
 * If mqttconfigENABLE_METRICS is set to 1, a string containing
 * metric information will be included in the "username" field of
 * the MQTT connect messages.
 */
#ifndef mqttconfigENABLE_METRICS
    #define mqttconfigENABLE_METRICS    ( 1 )
#endif

/**
 * @brief The maximum time interval in seconds allowed to elapse between 2 consecutive
 * control packets.
 */
#ifndef mqttconfigKEEP_ALIVE_INTERVAL_SECONDS
    #define mqttconfigKEEP_ALIVE_INTERVAL_SECONDS    ( 1200 )
#endif

/**
 * @brief Defines the frequency at which the client should send Keep Alive messages.
 *
 * Even though the maximum time allowed between 2 consecutive control packets
 * is defined by the mqttconfigKEEP_ALIVE_INTERVAL_SECONDS macro, the user
 * can and should send Keep Alive messages at a slightly faster rate to ensure
 * that the connection is not closed by the server because of network delays.
 * This macro defines the interval of inactivity after which a keep alive messages
 * is sent.
 */
#ifndef mqttconfigKEEP_ALIVE_ACTUAL_INTERVAL_TICKS
    #define mqttconfigKEEP_ALIVE_ACTUAL_INTERVAL_TICKS    ( 5000 )
#endif

/**
 * @brief The maximum interval in ticks to wait for PINGRESP.
 *
 * If PINGRESP is not received within this much time after sending PINGREQ,
 * the client assumes that the PINGREQ timed out.
 */
#ifndef mqttconfigKEEP_ALIVE_TIMEOUT_TICKS
    #define mqttconfigKEEP_ALIVE_TIMEOUT_TICKS    ( 1000 )
#endif

/**
 * @brief The maximum time in ticks for which the MQTT task is permitted to block.
 *
 * The MQTT task blocks until the user initiates any action or until it receives
 * any data from the broker. This macro controls the maximum time the MQTT task can
 * block. It should be set to a small number for the platforms which do not have any
 * mechanism to wake up the MQTT task whenever data is received on a connected socket.
 * This ensures that the MQTT task keeps waking up frequently and processes the publish
 * messages received from the broker, if any.
 *
 * If the platform's secure_sockets layer supports SOCKETS_SO_WAKEUP_CALLBACK i.e.
 * the MQTT task can wake up whenever data is received on a connected socket, this
 * value should be set to maximum value:
 * #define  #define mqttconfigMQTT_TASK_MAX_BLOCK_TICKS    ( ~( ( uint32_t ) 0 ) )
 *
 * If the platform's secure_sockets layer does not support SOCKETS_SO_WAKEUP_CALLBACK
 * i.e. the MQTT task cannot wake up whenever data is received on a connected socket,
 * this value should be set to a small number:
 * #define mqttconfigMQTT_TASK_MAX_BLOCK_TICKS             ( 100 )
 */
#ifndef mqttconfigMQTT_TASK_MAX_BLOCK_TICKS
    #error "mqttconfigMQTT_TASK_MAX_BLOCK_TICKS must be defined in aws_mqtt_agent_config.h."
#endif

/**
 * @defgroup MQTTTask MQTT task configuration parameters.
 */
/** @{ */
#ifndef mqttconfigMQTT_TASK_STACK_DEPTH
    #define mqttconfigMQTT_TASK_STACK_DEPTH    ( configMINIMAL_STACK_SIZE * 4 )
#endif

#ifndef mqttconfigMQTT_TASK_PRIORITY
    #define mqttconfigMQTT_TASK_PRIORITY    ( tskIDLE_PRIORITY )
#endif
/** @} */

/**
 * @brief Maximum number of MQTT clients that can exist simultaneously.
 */
#ifndef mqttconfigMAX_BROKERS
    #define mqttconfigMAX_BROKERS    ( 1 )
#endif

/**
 * @brief Maximum number of parallel operations per client.
 */
#ifndef mqttconfigMAX_PARALLEL_OPS
    #define mqttconfigMAX_PARALLEL_OPS    ( 5 )
#endif

/**
 * @brief Time in milliseconds after which the TCP send operation should timeout.
 */
#ifndef mqttconfigTCP_SEND_TIMEOUT_MS
    #define mqttconfigTCP_SEND_TIMEOUT_MS    ( 2000 )
#endif

/**
 * @brief Length of the buffer used to receive data.
 */
#ifndef mqttconfigRX_BUFFER_SIZE
    #define mqttconfigRX_BUFFER_SIZE    ( 1024 )
#endif

/**
 * @defgroup BufferPoolInterface The functions used by the MQTT client to get and return buffers.
 *
 * The MQTT client needs buffers for both transmitting and receiving messages.
 * Whenever it needs a buffer, it invokes mqttconfigGET_FREE_BUFFER_FXN function to get
 * a buffer and after it is done it invokes mqttconfigRETURN_BUFFER_FXN to return the
 * buffer. By default, BUFFERPOOL_GetFreeBuffer and BUFFERPOOL_ReturnBuffer functions are
 * used to get and return buffers from the central buffer pool. The user can change the
 * buffer management functions for MQTT client by defining mqttconfigGET_FREE_BUFFER_FXN
 * and mqttconfigRETURN_BUFFER_FXN macros. The user should implement the two functions
 * having signatures same as BUFFERPOOL_GetFreeBuffer and BUFFERPOOL_ReturnBuffer and then
 * define the macros in BufferPoolConfig.h:
 * @code
 * uint8_t* UserDefined_GetFreeBuffer( uint32_t *pulBufferLength );
 * void UserDefined_ReturnBuffer( uint8_t * const pucBuffer );
 *
 * #define mqttconfigGET_FREE_BUFFER_FXN       UserDefined_GetFreeBuffer
 * #define mqttconfigRETURN_BUFFER_FXN         UserDefined_ReturnBuffer
 * @endcode
 */
/** @{ */
#ifndef mqttconfigGET_FREE_BUFFER_FXN
    #define mqttconfigGET_FREE_BUFFER_FXN    BUFFERPOOL_GetFreeBuffer
#endif

#ifndef mqttconfigRETURN_BUFFER_FXN
    #define mqttconfigRETURN_BUFFER_FXN    BUFFERPOOL_ReturnBuffer
#endif
/** @} */

#endif /* _AWS_MQTT_AGENT_CONFIG_DEFAULTS_H_ */
