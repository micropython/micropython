/*
Amazon FreeRTOS OTA Update Demo V1.4.6
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/


/**
 * @file aws_ota_update_demo.c
 * @brief A simple OTA update example.
 *
 * This example initializes the OTA agent to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MQTT include. */
#include "aws_mqtt_agent.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"

/* Amazon FreeRTOS OTA agent includes. */
#include "aws_ota_agent.h"

/* Required for demo task stack and priority */
#include "aws_ota_update_demo.h"
#include "aws_demo_config.h"
#include "aws_application_version.h"

static void App_OTACompleteCallback(OTA_JobEvent_t eEvent );

/*-----------------------------------------------------------*/

#define myappONE_SECOND_DELAY_IN_TICKS      pdMS_TO_TICKS( 1000UL )     /* One second delay value for calls to vTaskDelay(). */
#define myappMAX_AWS_CONNECT_WAIT_IN_TICKS  pdMS_TO_TICKS( 60000UL )    /* Wait a maximum of 60 seconds to connect to the AWS IoT broker. */
#define myappMAX_AWS_DISCONNECT_WAIT_IN_TICKS pdMS_TO_TICKS( 60000UL )  /* Wait a maximum of 60 seconds to disconnect from the AWS IoT broker. */

static const char *pcStateStr[eOTA_NumAgentStates] =
{
     "Not Ready",
     "Ready",
     "Active",
     "Shutting down"
};

void vOTAUpdateDemoTask( void * pvParameters )
{
    MQTTAgentConnectParams_t xConnectParams;
    MQTTAgentHandle_t xMQTTClientHandle;
    OTA_State_t eState;

/* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

	configPRINTF ( ("OTA demo version %u.%u.%u\r\n",
		xAppFirmwareVersion.u.x.ucMajor,
		xAppFirmwareVersion.u.x.ucMinor,
		xAppFirmwareVersion.u.x.usBuild ) );
    configPRINTF( ( "Creating MQTT Client...\r\n" ) );

    /* Create the MQTT Client. */
    if( MQTT_AGENT_Create( &( xMQTTClientHandle ) ) == eMQTTAgentSuccess )
    {
        for ( ; ; )
        {
            configPRINTF( ( "Connecting to broker...\r\n" ) );
            memset( &xConnectParams, 0, sizeof( xConnectParams ) );
            xConnectParams.pucClientId = ( const uint8_t * ) ( clientcredentialIOT_THING_NAME );
            xConnectParams.usClientIdLength = sizeof( clientcredentialIOT_THING_NAME ) - 1; /* Length doesn't include trailing 0. */
            xConnectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
            xConnectParams.usPort = clientcredentialMQTT_BROKER_PORT;
            xConnectParams.pcCertificate = NULL;
            xConnectParams.ulCertificateSize = 0;
            xConnectParams.pvUserData = NULL;
            xConnectParams.pxCallback = NULL;
            xConnectParams.xFlags = democonfigMQTT_AGENT_CONNECT_FLAGS;

            /* Connect to the broker. */
            if( MQTT_AGENT_Connect( xMQTTClientHandle, &( xConnectParams ), myappMAX_AWS_CONNECT_WAIT_IN_TICKS ) == eMQTTAgentSuccess )
            {
                configPRINTF( ( "Connected to broker.\r\n" ) );
                OTA_AgentInit( xMQTTClientHandle, ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ), App_OTACompleteCallback, ( TickType_t ) ~0 );

                while( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_NotReady )
                {
                    /* Wait forever for OTA traffic but allow other tasks to run and output statistics only once per second. */
                    vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS );
                    configPRINTF( ( "State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n", pcStateStr[eState],
                            OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() ) );
                }
                if ( MQTT_AGENT_Disconnect( xMQTTClientHandle, myappMAX_AWS_DISCONNECT_WAIT_IN_TICKS ) == eMQTTAgentSuccess )
                {
                    configPRINTF( ( "Disconnected from MQTT broker\r\n" ) );
                }
                else
                {
                    configPRINTF( ( "Error trying to disconnect from MQTT broker\r\n" ) );
                }
            }
            else
            {
                configPRINTF( ( "ERROR:  MQTT_AGENT_Connect() Failed.\r\n" ) );
            }
            /* After failure to connect or a disconnect, wait an arbitrary one second before retry. */
            vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS );
        }
    }
    else
    {
        configPRINTF( ( "Failed to create MQTT client.\r\n" ) );
    }

    /* All done.  FreeRTOS does not allow a task to run off the end of its
     * implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}


/* The OTA agent has completed the update job or determined that we're in
 * self test mode. If it was accepted, we want to activate the new image.
 * This typically means we should reset the device to run the new firmware.
 * If now is not a good time to reset the device, it may be activated later
 * by your user code. If the update was rejected, just return without doing
 * anything and we'll wait for another job. If it reported that we should
 * start test mode, normally we would perform some kind of system checks to
 * make sure our new firmware does the basic things we think it should do
 * but we'll just go ahead and set the image as accepted for demo purposes.
 * The accept function varies depending on your platform. Refer to the OTA
 * PAL implementation for your platform in aws_ota_pal.c to see what it
 * does for you.
 */

static void App_OTACompleteCallback( OTA_JobEvent_t eEvent )
{
	OTA_Err_t xErr = kOTA_Err_Uninitialized;
	
    if ( eEvent == eOTA_JobEvent_Activate )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" ) );
        OTA_ActivateNewImage();
    }
    else if (eEvent == eOTA_JobEvent_Fail)
    {
        configPRINTF( ( "Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" ) );
        /* Nothing special to do. The OTA agent handles it. */
    }
	else if (eEvent == eOTA_JobEvent_StartTest)
	{
		/* This demo just accepts the image since it was a good OTA update and networking
		 * and services are all working (or we wouldn't have made it this far). If this
		 * were some custom device that wants to test other things before calling it OK,
		 * this would be the place to kick off those tests before calling OTA_SetImageState()
		 * with the final result of either accepted or rejected. */
        configPRINTF( ( "Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n" ) );
	xErr = OTA_SetImageState (eOTA_ImageState_Accepted);
        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( " Error! Failed to set image state as accepted.\r\n" );    
        }
	}
}


/*-----------------------------------------------------------*/

void vStartOTAUpdateDemoTask( void )
{
    xTaskCreate( vOTAUpdateDemoTask,
                 "OTA",
                 democonfigOTA_UPDATE_TASK_STACK_SIZE,
                 NULL,
                 democonfigOTA_UPDATE_TASK_TASK_PRIORITY,
                 NULL );
}
