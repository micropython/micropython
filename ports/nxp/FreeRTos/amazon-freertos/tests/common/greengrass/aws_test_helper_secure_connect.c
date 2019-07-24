/*
 * Amazon FreeRTOS Greegrass Discovery Test V1.1.4
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
#include <string.h>
#include "aws_helper_secure_connect.h"
#include "unity.h"
#include "unity_fixture.h"
#include "aws_ggd_config.h"
#include "aws_ggd_config_defaults.h"

TEST_GROUP( Full_GGD_Helper );

TEST_SETUP( Full_GGD_Helper )
{
}

TEST_TEAR_DOWN( Full_GGD_Helper )
{
}


TEST_GROUP_RUNNER( Full_GGD_Helper )
{
    RUN_TEST_CASE( Full_GGD_Helper, SecureConnect_Connect_Disconnect );
    RUN_TEST_CASE( Full_GGD_Helper, SecureConnect_Send );
}

TEST( Full_GGD_Helper, SecureConnect_Connect_Disconnect )
{
    GGD_HostAddressData_t xHostAddressData = { 0 };
    Socket_t xSocket;
    BaseType_t xStatus;

    xHostAddressData.pcCertificate = NULL;
    xHostAddressData.ulCertificateSize = 0;
    xHostAddressData.pcHostAddress = clientcredentialMQTT_BROKER_ENDPOINT;
    xHostAddressData.usPort = clientcredentialMQTT_BROKER_PORT;

    if( TEST_PROTECT() )
    {
        /** @brief Check ideal case, everything passes.
         *  @{
         */

        xStatus = GGD_SecureConnect_Connect( &xHostAddressData,
                                             &xSocket,
                                             ggdconfigTCP_RECEIVE_TIMEOUT_MS,
                                             ggdconfigTCP_SEND_TIMEOUT_MS );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );

        GGD_SecureConnect_Disconnect( &xSocket );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers.
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_SecureConnect_Connect( &xHostAddressData,
                                             NULL,
                                             ggdconfigTCP_RECEIVE_TIMEOUT_MS,
                                             ggdconfigTCP_SEND_TIMEOUT_MS );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_SecureConnect_Connect( NULL,
                                             &xSocket,
                                             ggdconfigTCP_RECEIVE_TIMEOUT_MS,
                                             ggdconfigTCP_SEND_TIMEOUT_MS );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( Full_GGD_Helper, SecureConnect_Send )
{
#define PAYLOAD    "Hello"
    char cPayload[] = PAYLOAD;
    Socket_t xSocket;
    BaseType_t xStatus;
    uint32_t ulPayLoadSize = strlen( PAYLOAD );
    GGD_HostAddressData_t xHostAddressData = { 0 };

    xHostAddressData.pcCertificate = NULL;
    xHostAddressData.ulCertificateSize = 0;
    xHostAddressData.pcHostAddress = clientcredentialMQTT_BROKER_ENDPOINT;
    xHostAddressData.usPort = clientcredentialMQTT_BROKER_PORT;

    if( TEST_PROTECT() )
    {
        /** @brief Check ideal case 1 character sent correctly.
         *  @{
         */
        xStatus = GGD_SecureConnect_Connect( &xHostAddressData,
                                             &xSocket,
                                             ggdconfigTCP_RECEIVE_TIMEOUT_MS,
                                             ggdconfigTCP_SEND_TIMEOUT_MS );
        xStatus = GGD_SecureConnect_Send( cPayload,
                                          ulPayLoadSize,
                                          xSocket );

        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        GGD_SecureConnect_Disconnect( &xSocket );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers.
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_SecureConnect_Send( NULL,
                                          ulPayLoadSize,
                                          xSocket );
    }

    /** @}*/
}
