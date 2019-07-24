/*
 * Amazon FreeRTOS MQTT Echo Demo V1.4.6
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
 * @file aws_hello_world.c
 * @brief A simple MQTT double echo example.
 *
 * It creates an MQTT client that both subscribes to and publishes to the
 * same MQTT topic, as a result of which each time the MQTT client publishes
 * a message to the remote MQTT broker, the broker sends the same message back
 * to the client (the first echo).  If the MQTT client has not seen the message
 * before, it appends the string "ACK" to the message before publishing back to
 * the broker (the second echo).
 *
 * The double echo allows a complete round trip to be observed from the AWS IoT
 * console itself.  The user can subscribe to "freertos/demos/echo" topic from
 * the AWS IoT Console and when executing correctly, the user will see 12 pairs
 * of strings, one pair (two strings) every five seconds for a minute.  The first
 * string of each pair takes the form "Hello World n", where 'n' is an monotonically
 * increasing integer.  This is the string originally published by the MQTT client.
 * The second string of each pair takes the form "Hello World n ACK".  This is the
 * string published by the MQTT client after it has received the first string back
 * from the MQTT broker.  The broker also sends the second string back to the
 * client, but the client ignores messages that already contain "ACK", so the
 * back and forth stops there.
 *
 * The demo uses two tasks. The task implemented by
 * prvMQTTConnectAndPublishTask() creates the MQTT client, subscribes to the
 * broker specified by the clientcredentialMQTT_BROKER_ENDPOINT constant,
 * performs the publish operations, and cleans up all the used resources after
 * a minute of operation.  The task implemented by prvMessageEchoingTask()
 * appends "ACK" to strings received from the MQTT broker and publishes them
 * back to the broker.  Strings received from the MQTT broker are passed from
 * the MQTT callback function to prvMessageEchoingTask() over a FreeRTOS message
 * buffer.
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "aws_hello_world.h"

/**
 * @brief MQTT client ID.
 *
 * It must be unique per MQTT broker.
 */
#define echoCLIENT_ID            ( ( const uint8_t * ) "MQTTEcho" )

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define echoTOPIC_NAME           ( ( const uint8_t * ) "freertos/demos/echo" )

/**
 * @brief The string appended to messages that are echoed back to the MQTT broker.
 *
 * It is also used to detect if a received message has already been acknowledged.
 */
#define echoACK_STRING           ( ( const char * ) " ACK" )

/**
 * @brief The length of the ACK string appended to messages that are echoed back
 * to the MQTT broker.
 */
#define echoACK_STRING_LENGTH    4

/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 */
#define echoMAX_DATA_LENGTH      20

/**
 * @brief A block time of 0 simply means "don't block".
 */
#define echoDONT_BLOCK           ( ( TickType_t ) 0 )

/*-----------------------------------------------------------*/

/**
 * @brief Implements the task that connects to and then publishes messages to the
 * MQTT broker.
 *
 * Messages are published every five seconds for a minute.
 *
 * @param[in] pvParameters Parameters passed while creating the task. Unused in our
 * case.
 */
static void prvMQTTConnectAndPublishTask( void * pvParameters );

/**
 * @brief Creates an MQTT client and then connects to the MQTT broker.
 *
 * The MQTT broker end point is set by clientcredentialMQTT_BROKER_ENDPOINT.
 *
 * @return pdPASS if everything is successful, pdFAIL otherwise.
 */
static BaseType_t prvCreateClientAndConnectToBroker( void );

/**
 * @brief Publishes the next message to the echoTOPIC_NAME topic.
 *
 * This is called every five seconds to publish the next message.
 *
 * @param[in] xMessageNumber Appended to the message to make it unique.
 */
static void prvPublishNextMessage( BaseType_t xMessageNumber );

/**
 * @brief The callback registered with the MQTT client to get notified when
 * data is received from the broker.
 *
 * @param[in] pvUserData User data as supplied while registering the callback.
 * @param[in] pxCallbackParams Data received from the broker.
 *
 * @return Indicates whether or not we take the ownership of the buffer containing
 * the MQTT message. We never take the ownership and always return eMQTTFalse.
 */
static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxCallbackParams );

/**
 * @brief Subscribes to the echoTOPIC_NAME topic.
 *
 * @return pdPASS if subscribe operation is successful, pdFALSE otherwise.
 */
static BaseType_t prvSubscribe( void );

/*-----------------------------------------------------------*/

/**
 * @brief The FreeRTOS message buffer that is used to send data from the callback
 * function (see prvMQTTCallback() above) to the task that echoes the data back to
 * the broker.
 */
static MessageBufferHandle_t xEchoMessageBuffer = NULL;

/**
 * @ brief The handle of the MQTT client object used by the MQTT echo demo.
 */
static MQTTAgentHandle_t xMQTTHandle = NULL;

/*-----------------------------------------------------------*/

static BaseType_t prvCreateClientAndConnectToBroker( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn = pdFAIL;
    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        democonfigMQTT_AGENT_CONNECT_FLAGS,   /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. Can be overridden by ALPN connection flag. */
        echoCLIENT_ID,                        /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        NULL,                                 /* User data supplied to the callback. Can be NULL. */
        NULL,                                 /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    configASSERT( xMQTTHandle == NULL );

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */
    xReturned = MQTT_AGENT_Create( &xMQTTHandle );

    if( xReturned == eMQTTAgentSuccess )
    {
        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) echoCLIENT_ID );

        /* Connect to the broker. */
        configPRINTF( ( "MQTT echo attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

        if( xReturned != eMQTTAgentSuccess )
        {
            /* Could not connect, so delete the MQTT client. */
            ( void ) MQTT_AGENT_Delete( xMQTTHandle );
            configPRINTF( ( "ERROR:  MQTT echo failed to connect with error %d.\r\n", xReturned ) );
        }
        else
        {
            configPRINTF( ( "MQTT echo connected.\r\n" ) );
            xReturn = pdPASS;
        }
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvPublishNextMessage( BaseType_t xMessageNumber )
{
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTAgentReturnCode_t xReturned;
    char cDataBuffer[ echoMAX_DATA_LENGTH ];

    /* Check this function is not being called before the MQTT client object has
     * been created. */
    configASSERT( xMQTTHandle != NULL );

    /* Create the message that will be published, which is of the form "Hello World n"
     * where n is a monotonically increasing number. Note that snprintf appends
     * terminating null character to the cDataBuffer. */
    ( void ) snprintf( cDataBuffer, echoMAX_DATA_LENGTH, "Hello World %d", ( int ) xMessageNumber );

    /* Setup the publish parameters. */
    memset( &( xPublishParameters ), 0x00, sizeof( xPublishParameters ) );
    xPublishParameters.pucTopic = echoTOPIC_NAME;
    xPublishParameters.pvData = cDataBuffer;
    xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
    xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );
    xPublishParameters.xQoS = eMQTTQoS1;

    /* Publish the message. */
    xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                    &( xPublishParameters ),
                                    democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "Echo successfully published '%s'\r\n", cDataBuffer ) );
    }
    else
    {
        configPRINTF( ( "ERROR:  Echo failed to publish '%s'\r\n", cDataBuffer ) );
    }

    /* Remove compiler warnings in case configPRINTF() is not defined. */
    ( void ) xReturned;
}
/*-----------------------------------------------------------*/

static void prvMessageEchoingTask( void * pvParameters )
{
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTAgentReturnCode_t xReturned;
    char cDataBuffer[ echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH ];
    size_t xBytesReceived;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Check this task has not already been created. */
    configASSERT( xMQTTHandle != NULL );
    configASSERT( xEchoMessageBuffer != NULL );

    /* Setup the publish parameters. */
    xPublishParameters.pucTopic = echoTOPIC_NAME;
    xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
    xPublishParameters.pvData = cDataBuffer;
    xPublishParameters.xQoS = eMQTTQoS1;

    for( ; ; )
    {
        /* Each message received on the message buffer has "ACK" appended to it
         * before being published on the same topic.  Wait for the next message. */
        memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
        xBytesReceived = xMessageBufferReceive( xEchoMessageBuffer,
                                                cDataBuffer,
                                                sizeof( cDataBuffer ),
                                                portMAX_DELAY );

        /* Ensure the ACK can be added without overflowing the buffer.
        * Note that xBytesReceived already includes null character as
        * it is written to the message buffer in the MQTT callback. */
        if( xBytesReceived <= ( sizeof( cDataBuffer ) - ( size_t ) echoACK_STRING_LENGTH ) )
        {
            /* Append ACK to the received message. Note that
             * strcat appends terminating null character to the
             * cDataBuffer. */
            strcat( cDataBuffer, echoACK_STRING );
            xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );

            /* Publish the ACK message. */
            xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                            &xPublishParameters,
                                            democonfigMQTT_TIMEOUT );

            if( xReturned == eMQTTAgentSuccess )
            {
                configPRINTF( ( "Message returned with ACK: '%s'\r\n", cDataBuffer ) );
            }
            else
            {
                configPRINTF( ( "ERROR:  Could not return message with ACK: '%s'\r\n", cDataBuffer ) );
            }
        }
        else
        {
            /* cDataBuffer is null terminated as the terminating null
             * character was sent from the MQTT callback. */
            configPRINTF( ( "ERROR:  Buffer is not big enough to return message with ACK: '%s'\r\n", cDataBuffer ) );
        }
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvSubscribe( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn;
    MQTTAgentSubscribeParams_t xSubscribeParams;

    /* Setup subscribe parameters to subscribe to echoTOPIC_NAME topic. */
    xSubscribeParams.pucTopic = echoTOPIC_NAME;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    xSubscribeParams.pxPublishCallback = prvMQTTCallback;
    xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
    xSubscribeParams.xQoS = eMQTTQoS1;

    /* Subscribe to the topic. */
    xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
                                      &xSubscribeParams,
                                      democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "MQTT Echo demo subscribed to %s\r\n", echoTOPIC_NAME ) );
        xReturn = pdPASS;
    }
    else
    {
        configPRINTF( ( "ERROR:  MQTT Echo demo could not subscribe to %s\r\n", echoTOPIC_NAME ) );
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters )
{
    char cBuffer[ echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH ];
    uint32_t ulBytesToCopy = ( echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH - 1 ); /* Bytes to copy initialized to ensure it
                                                                                   * fits in the buffer. One place is left
                                                                                   * for NULL terminator. */

    /* Remove warnings about the unused parameters. */
    ( void ) pvUserData;

    /* Don't expect the callback to be invoked for any other topics. */
    configASSERT( ( size_t ) ( pxPublishParameters->usTopicLength ) == strlen( ( const char * ) echoTOPIC_NAME ) );
    configASSERT( memcmp( pxPublishParameters->pucTopic, echoTOPIC_NAME, ( size_t ) ( pxPublishParameters->usTopicLength ) ) == 0 );

    /* THe ulBytesToCopy has already been initialized to ensure it does not copy
     * more bytes than will fit in the buffer.  Now check it does not copy more
     * bytes than are available. */
    if( pxPublishParameters->ulDataLength <= ulBytesToCopy )
    {
        ulBytesToCopy = pxPublishParameters->ulDataLength;

        /* Set the buffer to zero and copy the data into the buffer to ensure
         * there is a NULL terminator and the buffer can be accessed as a
         * string. */
        memset( cBuffer, 0x00, sizeof( cBuffer ) );
        memcpy( cBuffer, pxPublishParameters->pvData, ( size_t ) ulBytesToCopy );

        /* Only echo the message back if it has not already been echoed.  If the
         * data has already been echoed then it will already contain the echoACK_STRING
         * string. */
        if( strstr( cBuffer, echoACK_STRING ) == NULL )
        {
            /* The string has not been echoed before, so send it to the publish
             * task, which will then echo the data back.  Make sure to send the
             * terminating null character as well so that the received buffer in
             * EchoingTask can be printed as a C string.  THE DATA CANNOT BE ECHOED
             * BACK WITHIN THE CALLBACK AS THE CALLBACK IS EXECUTING WITHINT THE
             * CONTEXT OF THE MQTT TASK.  Calling an MQTT API function here could cause
             * a deadlock. */
            ( void ) xMessageBufferSend( xEchoMessageBuffer, cBuffer, ( size_t ) ulBytesToCopy + ( size_t ) 1, echoDONT_BLOCK );
        }
    }
    else
    {
        configPRINTF( ( "[WARN]: Dropping received message as it does not fit in the buffer.\r\n" ) );
    }

    /* The data was copied into the FreeRTOS message buffer, so the buffer
     * containing the data is no longer required.  Returning eMQTTFalse tells the
     * MQTT agent that the ownership of the buffer containing the message lies with
     * the agent and it is responsible for freeing the buffer. */
    return eMQTTFalse;
}
/*-----------------------------------------------------------*/

static void prvMQTTConnectAndPublishTask( void * pvParameters )
{
    BaseType_t xX;
    BaseType_t xReturned;
    const TickType_t xFiveSeconds = pdMS_TO_TICKS( 5000UL );
    const BaseType_t xIterationsInAMinute = 60 / 5;
    TaskHandle_t xEchoingTask = NULL;

    /* Avoid compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Create the MQTT client object and connect it to the MQTT broker. */
    xReturned = prvCreateClientAndConnectToBroker();

    if( xReturned == pdPASS )
    {
        /* Create the task that echoes data received in the callback back to the
         * MQTT broker. */
        xReturned = xTaskCreate( prvMessageEchoingTask,               /* The function that implements the task. */
                                 "Echoing",                           /* Human readable name for the task. */
                                 democonfigMQTT_ECHO_TASK_STACK_SIZE, /* Size of the stack to allocate for the task, in words not bytes! */
                                 NULL,                                /* The task parameter is not used. */
                                 tskIDLE_PRIORITY,                    /* Runs at the lowest priority. */
                                 &( xEchoingTask ) );                 /* The handle is stored so the created task can be deleted again at the end of the demo. */

        if( xReturned != pdPASS )
        {
            /* The task could not be created because there was insufficient FreeRTOS
             * heap available to create the task's data structures and/or stack. */
            configPRINTF( ( "MQTT echoing task could not be created - out of heap space?\r\n" ) );
        }
    }
    else
    {
        configPRINTF( ( "MQTT echo test could not connect to broker.\r\n" ) );
    }

    if( xReturned == pdPASS )
    {
        configPRINTF( ( "MQTT echo test echoing task created.\r\n" ) );

        /* Subscribe to the echo topic. */
        xReturned = prvSubscribe();
    }

    if( xReturned == pdPASS )
    {
        /* MQTT client is now connected to a broker.  Publish a message
         * every five seconds until a minute has elapsed. */
        for( xX = 0; xX < xIterationsInAMinute; xX++ )
        {
            prvPublishNextMessage( xX );

            /* Five seconds delay between publishes. */
            vTaskDelay( xFiveSeconds );
        }
    }

    /* Disconnect the client. */
    ( void ) MQTT_AGENT_Disconnect( xMQTTHandle, democonfigMQTT_TIMEOUT );

    /* End the demo by deleting all created resources. */
    configPRINTF( ( "MQTT echo demo finished.\r\n" ) );
    configPRINTF( ( "----Demo finished----\r\n" ) );
    vMessageBufferDelete( xEchoMessageBuffer );
    vTaskDelete( xEchoingTask );
    vTaskDelete( NULL ); /* Delete this task. */
}
/*-----------------------------------------------------------*/

void vStartMQTTEchoDemo( void )
{
    configPRINTF( ( "Creating MQTT Echo Task...\r\n" ) );

    /* Create the message buffer used to pass strings from the MQTT callback
     * function to the task that echoes the strings back to the broker.  The
     * message buffer will only ever have to hold one message as messages are only
     * published every 5 seconds.  The message buffer requires that there is space
     * for the message length, which is held in a size_t variable. */
    xEchoMessageBuffer = xMessageBufferCreate( ( size_t ) echoMAX_DATA_LENGTH + sizeof( size_t ) );
    configASSERT( xEchoMessageBuffer );

    /* Create the task that publishes messages to the MQTT broker every five
     * seconds.  This task, in turn, creates the task that echoes data received
     * from the broker back to the broker. */
    ( void ) xTaskCreate( prvMQTTConnectAndPublishTask,        /* The function that implements the demo task. */
                          "MQTTEcho",                          /* The name to assign to the task being created. */
                          democonfigMQTT_ECHO_TASK_STACK_SIZE, /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          NULL,                                /* The task parameter is not being used. */
                          democonfigMQTT_ECHO_TASK_PRIORITY,   /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
}
/*-----------------------------------------------------------*/
