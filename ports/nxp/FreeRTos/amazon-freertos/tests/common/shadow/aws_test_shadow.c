/*
 * Amazon FreeRTOS Shadow Test V1.1.4
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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* AWS includes. */
#include "aws_clientcredential.h"
#include "aws_shadow.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

#define shadowCLIENT_ID        clientcredentialIOT_THING_NAME
#define shadowTHING_NAME       clientcredentialIOT_THING_NAME
#define shadowTIMEOUT          pdMS_TO_TICKS( 10000UL )
/* Client token can be anything, as long as it's unique. */
#define shadowCLIENT_TOKEN     "aws-test-shadow-client-token"

#define shadowBUFFER_LENGTH    512
static char pcUpdateBuffer[ shadowBUFFER_LENGTH ];

/* Delay between test loops. */
#define shadowtestLOOP_DELAY    ( ( TickType_t ) 150 / portTICK_PERIOD_MS )

/* notification from callbacks to task*/
static SemaphoreHandle_t xShadowUpdateSemaphore;

/* Generate initial shadow document */
static uint32_t prvGenerateShadowJSON( void );

/* Called when shadow service received UPDATE request and updated thing shadow*/
BaseType_t prvTestUpdatedCallback( void * pvUserData,
                                   const char * const pcThingName,
                                   const char * const pcDeltaDocument,
                                   uint32_t ulDocumentLength,
                                   MQTTBufferHandle_t xBuffer );

/* Called when delta is reported in shadow document*/
BaseType_t prvTestDeltaCallback( const char * const pcThingName,
                                 const char * const pcDeltaDocument,
                                 uint32_t ulDocumentLength,
                                 MQTTBufferHandle_t xBuffer );

/* Called when shadow document is deleted*/
BaseType_t prvTestDeleteCallback( const char * const pcThingName,
                                  const char * const pcDeltaDocument,
                                  uint32_t ulDocumentLength,
                                  MQTTBufferHandle_t xBuffer );


TEST_GROUP( Full_Shadow );

TEST_SETUP( Full_Shadow )
{
}

TEST_TEAR_DOWN( Full_Shadow )
{
}

TEST_GROUP_RUNNER( Full_Shadow )
{
    RUN_TEST_CASE( Full_Shadow, ClientCreateDelete );
    RUN_TEST_CASE( Full_Shadow, ClientConnectDisconnect );
    RUN_TEST_CASE( Full_Shadow, CreateShadowDocument );
    RUN_TEST_CASE( Full_Shadow, DeleteShadowDocument );
    RUN_TEST_CASE( Full_Shadow, UpdateCallback );
}

/* Generate initial shadow document */
static uint32_t prvGenerateShadowJSON()
{
    /*simple shadow document for test */

    return ( uint32_t ) snprintf( pcUpdateBuffer, shadowBUFFER_LENGTH,
                                  "{"
                                  "\"state\":{"
                                  "\"desired\":{"
                                  "\"power\":\"on\""
                                  "},"
                                  "\"reported\":{"
                                  "\"power\":\"on\""
                                  "}"
                                  "},"
                                  "\"clientToken\": \"" shadowCLIENT_TOKEN "\""
                                                                           "}" );
}

/* Called when shadow service received UPDATE request and updated thing shadow*/
BaseType_t prvTestUpdatedCallback( void * pvUserData,
                                   const char * const pcThingName,
                                   const char * const pcDeltaDocument,
                                   uint32_t ulDocumentLength,
                                   MQTTBufferHandle_t xBuffer )
{
    ( void ) pvUserData;
    ( void ) pcThingName;
    ( void ) pcDeltaDocument;
    ( void ) ulDocumentLength;
    ( void ) xBuffer;

    /* notify testing task . */
    ( void ) xSemaphoreGive( xShadowUpdateSemaphore );

    return pdFALSE;
}

/* Called when delta is reported in shadow document*/
BaseType_t prvTestDeltaCallback( const char * const pcThingName,
                                 const char * const pcDeltaDocument,
                                 uint32_t ulDocumentLength,
                                 MQTTBufferHandle_t xBuffer )
{
    ( void ) pcThingName;
    ( void ) pcDeltaDocument;
    ( void ) ulDocumentLength;
    ( void ) xBuffer;


    return pdFALSE;
}

/* Called when shadow document is deleted*/
BaseType_t prvTestDeleteCallback( const char * const pcThingName,
                                  const char * const pcDeltaDocument,
                                  uint32_t ulDocumentLength,
                                  MQTTBufferHandle_t xBuffer )
{
    ( void ) pcThingName;
    ( void ) pcDeltaDocument;
    ( void ) ulDocumentLength;
    ( void ) xBuffer;

    return pdFALSE;
}

/* helper functions for setting MQTT params. */
void TEST_SHADOW_Connect_Helper( MQTTAgentConnectParams_t * xConnectParams,
                                 ShadowClientHandle_t * pxShadowClientHandle )
{
    xConnectParams->pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
    xConnectParams->xURLIsIPAddress = pdFALSE; /* Deprecated. */
    xConnectParams->usPort = clientcredentialMQTT_BROKER_PORT;

    xConnectParams->xSecuredConnection = pdFALSE; /* Deprecated. */
    xConnectParams->xFlags = mqttagentREQUIRE_TLS;
    xConnectParams->pcCertificate = NULL;
    xConnectParams->ulCertificateSize = 0;
    xConnectParams->pxCallback = NULL;
    xConnectParams->pvUserData = pxShadowClientHandle;

    xConnectParams->pucClientId = ( uint8_t * ) shadowCLIENT_ID;
    xConnectParams->usClientIdLength = ( uint16_t ) ( strlen( ( const char * ) shadowCLIENT_ID ) );
}

/* Test for shadow client create and delete api */
TEST( Full_Shadow, ClientCreateDelete )
{
    /*Init required params and shadow library for test.*/
    ShadowClientHandle_t xShadowClientHandle;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;
    uint32_t i, ulMaxLoopCount = 1;

    if( TEST_PROTECT() )
    {
        xCreateParams.xMQTTClientType = eDedicatedMQTTClient;

        for( i = 0; i < ulMaxLoopCount; i++ )
        {
            /*create the shadow client.*/
            xReturn = SHADOW_ClientCreate( &xShadowClientHandle, &xCreateParams );
            TEST_ASSERT_TRUE( eShadowSuccess == xReturn );

            /* Pause for a short while. */
            vTaskDelay( shadowtestLOOP_DELAY );

            /* if shadow client was created successfully test delete.*/
            xReturn = SHADOW_ClientDelete( xShadowClientHandle );

            TEST_ASSERT_TRUE( eShadowSuccess == xReturn );

            /* Pause for a short while. */
            vTaskDelay( shadowtestLOOP_DELAY );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/* Test for shadow client connect and disconnect api. */
TEST( Full_Shadow, ClientConnectDisconnect )
{
    /*Init required params and shadow library for test.*/
    ShadowClientHandle_t xShadowClientHandle;
    BaseType_t xClientCreated = pdFALSE;
    MQTTAgentConnectParams_t xConnectParams;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;
    uint32_t i, ulMaxLoopCount = 1;

    if( TEST_PROTECT() )
    {
        xCreateParams.xMQTTClientType = eDedicatedMQTTClient;

        /*create shadow client .*/
        xReturn = SHADOW_ClientCreate( &xShadowClientHandle, &xCreateParams );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        xClientCreated = pdTRUE;

        /*set MQTT params.*/
        TEST_SHADOW_Connect_Helper( &xConnectParams, &xShadowClientHandle );

        for( i = 0; i < ulMaxLoopCount; i++ )
        {
            /*try connecting to shadow client.*/
            xReturn = SHADOW_ClientConnect( xShadowClientHandle,
                                            &xConnectParams,
                                            shadowTIMEOUT );
            TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

            /* Pause for a short while to ensure the network is not too
             * congested. */
            vTaskDelay( shadowtestLOOP_DELAY );

            /*If connection is successful try to disconnect from shadow client.*/
            xReturn = SHADOW_ClientDisconnect( xShadowClientHandle );
            TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

            /* Pause for a short while to ensure the network is not too
             * congested. */
            vTaskDelay( shadowtestLOOP_DELAY );
        }
    }
    else
    {
        TEST_FAIL();
    }

    if( xClientCreated )
    {
        /* delete shadow client before returning.*/
        xReturn = SHADOW_ClientDelete( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
}

/* Test for shadow document is created/updated - SHADOW_Update api.*/
TEST( Full_Shadow, CreateShadowDocument )
{
    /*Init required params and shadow library for test.*/
    ShadowClientHandle_t xShadowClientHandle;
    BaseType_t xClientCreated = pdFALSE;
    MQTTAgentConnectParams_t xConnectParams;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;
    uint32_t ulCompareLength;
    ShadowOperationParams_t xOperationParams;

    if( TEST_PROTECT() )
    {
        xCreateParams.xMQTTClientType = eDedicatedMQTTClient;
        xReturn = SHADOW_ClientCreate( &xShadowClientHandle, &xCreateParams );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        xClientCreated = pdTRUE;

        memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
        TEST_SHADOW_Connect_Helper( &xConnectParams, &xShadowClientHandle );
        xReturn = SHADOW_ClientConnect( xShadowClientHandle,
                                        &xConnectParams,
                                        shadowTIMEOUT );

        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

        xOperationParams.pcThingName = shadowTHING_NAME;
        xOperationParams.xQoS = eMQTTQoS0;
        xOperationParams.pcData = NULL;
        /* subscribe to the accepted and rejected topics */
        xOperationParams.ucKeepSubscriptions = pdFALSE;
        xReturn = SHADOW_Delete( xShadowClientHandle,
                                 &xOperationParams,
                                 shadowTIMEOUT );
        TEST_ASSERT_TRUE( ( xReturn == eShadowSuccess ) || ( xReturn == eShadowRejectedNotFound ) );

        /*init shadow operations params. */
        xOperationParams.pcData = pcUpdateBuffer;
        xOperationParams.ulDataLength = ulCompareLength = prvGenerateShadowJSON();

        /* Only compare the state data when checking correctness. */
        ulCompareLength -= sizeof( "\"clientToken\": \"" shadowCLIENT_TOKEN "\"" );

        /* create the shadow document for thing.*/
        xReturn = SHADOW_Update( xShadowClientHandle,
                                 &xOperationParams,
                                 shadowTIMEOUT );

        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        /* get the shadow document from shadow service.*/
        xReturn = SHADOW_Get( xShadowClientHandle,
                              &xOperationParams,
                              shadowTIMEOUT );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

        if( memcmp( pcUpdateBuffer,
                    xOperationParams.pcData,
                    ulCompareLength ) != 0 )
        {
            TEST_FAIL();
        }

        /* Free the buffer used by shadow document. */
        xReturn = SHADOW_ReturnMQTTBuffer( xShadowClientHandle, xOperationParams.xBuffer );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

        /* disconnect shadow client. */
        xReturn = SHADOW_ClientDisconnect( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
    else
    {
        TEST_FAIL();
    }

    if( xClientCreated )
    {
        /* delete shadow client before returning.*/
        xReturn = SHADOW_ClientDelete( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
}

/* Test for shadow document delete api.*/
TEST( Full_Shadow, DeleteShadowDocument )
{
    /*Init required params and shadow library for test.*/
    ShadowClientHandle_t xShadowClientHandle;
    BaseType_t xClientCreated = pdFALSE;
    MQTTAgentConnectParams_t xConnectParams;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;
    ShadowOperationParams_t xOperationParams;

    if( TEST_PROTECT() )
    {
        xCreateParams.xMQTTClientType = eDedicatedMQTTClient;
        xReturn = SHADOW_ClientCreate( &xShadowClientHandle, &xCreateParams );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        xClientCreated = pdTRUE;

        memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
        TEST_SHADOW_Connect_Helper( &xConnectParams, &xShadowClientHandle );
        xReturn = SHADOW_ClientConnect( xShadowClientHandle,
                                        &xConnectParams,
                                        shadowTIMEOUT );

        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

        xOperationParams.pcThingName = shadowTHING_NAME;
        xOperationParams.xQoS = eMQTTQoS0;
        xOperationParams.pcData = pcUpdateBuffer;
        /* subscribe to the accepted and rejected topics */
        xOperationParams.ucKeepSubscriptions = pdFALSE;

        /* generate a simple json document for test. */
        xOperationParams.ulDataLength = prvGenerateShadowJSON();

        /* create the shadow document for thing.*/
        xReturn = SHADOW_Update( xShadowClientHandle,
                                 &xOperationParams,
                                 shadowTIMEOUT );

        /*Delete the shadow document just created. */
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        /* delete the shadow document is same .*/
        xReturn = SHADOW_Delete( xShadowClientHandle,
                                 &xOperationParams,
                                 shadowTIMEOUT );


        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        /* try to get the deleted document .*/
        xReturn = SHADOW_Get( xShadowClientHandle,
                              &xOperationParams,
                              shadowTIMEOUT );

        TEST_ASSERT_TRUE( xOperationParams.pcData == NULL )


        /*disconnect shadow client*/
        xReturn = SHADOW_ClientDisconnect( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
    else
    {
        TEST_FAIL();
    }

    if( xClientCreated )
    {
        /* delete shadow client before returning.*/
        xReturn = SHADOW_ClientDelete( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
}

/* Test for shadow document update callback.*/
TEST( Full_Shadow, UpdateCallback )
{
    /*Init required params and shadow library for test.*/
    ShadowClientHandle_t xShadowClientHandle;
    BaseType_t xClientCreated = pdFALSE;
    BaseType_t xSemaphoreCreated = pdFALSE;
    MQTTAgentConnectParams_t xConnectParams;
    ShadowCallbackParams_t xCallbackParams;
    ShadowCreateParams_t xCreateParams;
    ShadowReturnCode_t xReturn;

    if( TEST_PROTECT() )
    {
        xCreateParams.xMQTTClientType = eDedicatedMQTTClient;
        xShadowUpdateSemaphore = xSemaphoreCreateBinary();
        TEST_ASSERT_TRUE( xShadowUpdateSemaphore != NULL );
        xSemaphoreCreated = pdTRUE;

        xReturn = SHADOW_ClientCreate( &xShadowClientHandle, &xCreateParams );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        xClientCreated = pdTRUE;

        memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
        TEST_SHADOW_Connect_Helper( &xConnectParams, &xShadowClientHandle );
        xReturn = SHADOW_ClientConnect( xShadowClientHandle,
                                        &xConnectParams,
                                        shadowTIMEOUT );

        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        /*Register update callback.*/
        xCallbackParams.pcThingName = shadowTHING_NAME;
        xCallbackParams.xShadowUpdatedCallback = prvTestUpdatedCallback;
        xReturn = SHADOW_RegisterCallbacks( xShadowClientHandle,
                                            &xCallbackParams,
                                            shadowTIMEOUT );

        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
        ShadowOperationParams_t xOperationParams;
        xOperationParams.pcThingName = shadowTHING_NAME;
        xOperationParams.xQoS = eMQTTQoS0;
        xOperationParams.pcData = pcUpdateBuffer;
        /* subscribe to the accepted and rejected topics */
        xOperationParams.ucKeepSubscriptions = pdFALSE;

        /* generate a simple json document for test. */
        xOperationParams.ulDataLength = prvGenerateShadowJSON();

        /* create the shadow document for thing.*/
        xReturn = SHADOW_Update( xShadowClientHandle,
                                 &xOperationParams,
                                 shadowTIMEOUT );


        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );

        /*wait for update callback.*/


        /* See if we can obtain the semaphore.  If the semaphore is not
         * available wait 10 ticks to see if it becomes free. */
        vTaskDelay( ( TickType_t ) 10 );

        if( xSemaphoreTake( xShadowUpdateSemaphore, ( TickType_t ) 10 ) == pdFALSE )
        {
            /* We were not able to obtain the semaphore. */
            TEST_FAIL();
        }

        xReturn = SHADOW_ClientDisconnect( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }
    else
    {
        TEST_FAIL();
    }

    if( xClientCreated )
    {
        /* delete shadow client before returning.*/
        xReturn = SHADOW_ClientDelete( xShadowClientHandle );
        TEST_ASSERT_EQUAL( eShadowSuccess, xReturn );
    }

    if( xSemaphoreCreated )
    {
        vSemaphoreDelete( xShadowUpdateSemaphore );
    }
}
