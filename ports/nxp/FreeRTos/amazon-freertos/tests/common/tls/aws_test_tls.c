/*
 * Amazon FreeRTOS TLS AFQP V1.1.4
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
#include <string.h>

/* Unity framework includes. */
#include "unity_fixture.h"

/* Secure sockets includes */
#include "aws_secure_sockets.h"

/* Credential includes. */
#include "aws_clientcredential.h"
#include "aws_test_tls.h"

/* Provisioning include. */
#include "aws_dev_mode_key_provisioning.h"
#include "aws_pkcs11.h"


/*
 * Length of elliptic curve credentials included from aws_clientcredential_keys.h.
 */
static const uint32_t tlstestCLIENT_CERTIFICATE_LENGTH_EC = sizeof( tlstestCLIENT_CERTIFICATE_PEM_EC );
static const uint32_t tlstestCLIENT_PRIVATE_KEY_LENGTH_EC = sizeof( tlstestCLIENT_PRIVATE_KEY_PEM_EC );

/*
 * Length of device malformed certificate included from aws_clientcredential_keys.h.
 */
static const uint32_t tlstestCLIENT_CERTIFICATE_PEM_MALFORMED_LENGTH = sizeof( tlstestCLIENT_CERTIFICATE_PEM_MALFORMED );

/*
 * Length of the untrusted credentials included from aws_clientcredential_keys.h.
 */
static const uint32_t tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM_LENGTH = sizeof( tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM );
static const uint32_t tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM_LENGTH = sizeof( tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM );

/*
 * Length of the BYOC credentials included from aws_clientcredential_keys.h.
 */
static const uint32_t tlstestCLIENT_BYOC_CERTIFICATE_PEM_LENGTH = sizeof( tlstestCLIENT_BYOC_CERTIFICATE_PEM );
static const uint32_t tlstestCLIENT_BYOC_PRIVATE_KEY_PEM_LENGTH = sizeof( tlstestCLIENT_BYOC_PRIVATE_KEY_PEM );
/*-----------------------------------------------------------*/

TEST_GROUP( Full_TLS );

TEST_SETUP( Full_TLS )
{
}

TEST_TEAR_DOWN( Full_TLS )
{
}

TEST_GROUP_RUNNER( Full_TLS )
{
    RUN_TEST_CASE( Full_TLS, AFQP_TLS_ConnectEC );
    RUN_TEST_CASE( Full_TLS, AFQP_TLS_ConnectRSA );
    RUN_TEST_CASE( Full_TLS, AFQP_TLS_ConnectMalformedCert );
    RUN_TEST_CASE( Full_TLS, AFQP_TLS_ConnectUntrustedCert );
    RUN_TEST_CASE( Full_TLS, AFQP_TLS_ConnectBYOCCredentials );
}

/*-----------------------------------------------------------*/

static Socket_t prvSecureSocketCreate( void )
{
    BaseType_t xResult;
    Socket_t xSocket;

    xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
    TEST_ASSERT_NOT_EQUAL( xSocket, SOCKETS_INVALID_SOCKET );

    xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_REQUIRE_TLS, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket set sock opt require tls failed" );

    return xSocket;
}
/*-----------------------------------------------------------*/

static void prvSecureSocketClose( Socket_t xSocket )
{
    BaseType_t xResult;

    xResult = SOCKETS_Close( xSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

static void prvConnectWithProvisioning( ProvisioningParams_t * pxProvisioningParams,
                                        BaseType_t xConnectExpectedToSucceed )
{
    const char * pcAWSIoTAddress = clientcredentialMQTT_BROKER_ENDPOINT;
    uint16_t usAWSIoTPort = clientcredentialMQTT_BROKER_PORT;
    SocketsSockaddr_t xMQTTServerAddress = { 0 };
    Socket_t xSocket;
    BaseType_t xSocketOpen = pdFALSE;
    BaseType_t xResult;

    if( TEST_PROTECT() )
    {
        /* Provision the device with the supplied parameters. */
        vAlternateKeyProvisioning( pxProvisioningParams );

        /* Create socket. */
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
        TEST_ASSERT_NOT_EQUAL( xSocket, SOCKETS_INVALID_SOCKET );
        xSocketOpen = pdTRUE;

        /* Mark the socket as secure. */
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_REQUIRE_TLS, NULL, 0 );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket set sock opt require tls failed" );

        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_SERVER_NAME_INDICATION, pcAWSIoTAddress, 1u + strlen( pcAWSIoTAddress ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket set sock opt server name indication failed" );

        /* DNS Lookup. */
        xMQTTServerAddress.ulAddress = SOCKETS_GetHostByName( pcAWSIoTAddress );

        /* Connect. */
        xMQTTServerAddress.usPort = SOCKETS_htons( usAWSIoTPort );
        xMQTTServerAddress.ucSocketDomain = SOCKETS_AF_INET;
        xResult = SOCKETS_Connect( xSocket, &xMQTTServerAddress, sizeof( xMQTTServerAddress ) );

        /* Check that the connect result is as expected. */
        if( xConnectExpectedToSucceed == pdTRUE )
        {
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket connect failed while it was expected to succeed." );

            /* If the connect succeeded, shutdown the socket. */
            xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket disconnect failed." );
        }
        else
        {
            TEST_ASSERT_LESS_THAN_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket connect succeeded while it was expected to fail." );
        }
    }

    /* Make sure to close the socket. */
    if( TEST_PROTECT() )
    {
        if( xSocketOpen == pdTRUE )
        {
            xResult = SOCKETS_Close( xSocket );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
            xSocketOpen = pdFALSE;
        }
    }

    /* Regardless of whatever failed above, re-provision the
     * device with default RSA certs so that subsequent tests
     * are not changed. */
    vDevModeKeyProvisioning();
}
/*-----------------------------------------------------------*/

TEST( Full_TLS, AFQP_TLS_ConnectRSA )
{
    const char * pcAWSIoTAddress = clientcredentialMQTT_BROKER_ENDPOINT;
    uint16_t usAWSIoTPort = clientcredentialMQTT_BROKER_PORT;
    SocketsSockaddr_t xMQTTServerAddress = { 0 };
    Socket_t xSocket;
    BaseType_t xResult;

    xMQTTServerAddress.ulAddress = SOCKETS_GetHostByName( pcAWSIoTAddress );
    xMQTTServerAddress.usPort = SOCKETS_htons( usAWSIoTPort );
    xMQTTServerAddress.ucSocketDomain = SOCKETS_AF_INET;

    xSocket = prvSecureSocketCreate();

    if( TEST_PROTECT() )
    {
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_SERVER_NAME_INDICATION, pcAWSIoTAddress, 1u + strlen( pcAWSIoTAddress ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket set sock opt server name indication failed" );

        xResult = SOCKETS_Connect( xSocket, &xMQTTServerAddress, sizeof( xMQTTServerAddress ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket connect failed" );

        xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket disconnect failed" );
    }

    prvSecureSocketClose( xSocket );
}
/*-----------------------------------------------------------*/

TEST( Full_TLS, AFQP_TLS_ConnectEC )
{
    ProvisioningParams_t xParams;

    /* Provision the device with P-256 elliptic curve key/certs. */
    xParams.ulClientPrivateKeyType = CKK_EC;
    xParams.pcClientPrivateKey = ( uint8_t * ) tlstestCLIENT_PRIVATE_KEY_PEM_EC;
    xParams.ulClientPrivateKeyLength = tlstestCLIENT_PRIVATE_KEY_LENGTH_EC;
    xParams.pcClientCertificate = ( uint8_t * ) tlstestCLIENT_CERTIFICATE_PEM_EC;
    xParams.ulClientCertificateLength = tlstestCLIENT_CERTIFICATE_LENGTH_EC;

    prvConnectWithProvisioning( &( xParams ),
                                pdTRUE /* The connect operation should succeed. */
                                );
}
/*-----------------------------------------------------------*/

TEST( Full_TLS, AFQP_TLS_ConnectMalformedCert )
{
    ProvisioningParams_t xParams;

    /* Provision the device with malformed client credential certificate. */
    xParams.ulClientPrivateKeyType = CKK_RSA;
    xParams.pcClientPrivateKey = ( uint8_t * ) clientcredentialCLIENT_PRIVATE_KEY_PEM;
    xParams.ulClientPrivateKeyLength = clientcredentialCLIENT_PRIVATE_KEY_LENGTH;
    xParams.pcClientCertificate = ( uint8_t * ) tlstestCLIENT_CERTIFICATE_PEM_MALFORMED;
    xParams.ulClientCertificateLength = tlstestCLIENT_CERTIFICATE_PEM_MALFORMED_LENGTH;

    prvConnectWithProvisioning( &( xParams ),
                                pdFALSE /* The connect operation with a malformed certificate should fail. */
                                );
}
/*-----------------------------------------------------------*/

TEST( Full_TLS, AFQP_TLS_ConnectUntrustedCert )
{
    ProvisioningParams_t xParams;

    /* Provision the device with malformed client credential certificate. */
    xParams.ulClientPrivateKeyType = CKK_RSA;
    xParams.pcClientPrivateKey = ( uint8_t * ) tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM;
    xParams.ulClientPrivateKeyLength = tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM_LENGTH;
    xParams.pcClientCertificate = ( uint8_t * ) tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM;
    xParams.ulClientCertificateLength = tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM_LENGTH;

    prvConnectWithProvisioning( &( xParams ),
                                pdFALSE /* The connect operation with an untrusted certificate should fail. */
                                );
}
/*-----------------------------------------------------------*/

TEST( Full_TLS, AFQP_TLS_ConnectBYOCCredentials )
{
    ProvisioningParams_t xParams;

    /* Provision the device with BYOC credentials. */
    xParams.ulClientPrivateKeyType = CKK_EC;
    xParams.pcClientPrivateKey = ( uint8_t * ) tlstestCLIENT_BYOC_PRIVATE_KEY_PEM;
    xParams.ulClientPrivateKeyLength = tlstestCLIENT_BYOC_PRIVATE_KEY_PEM_LENGTH;
    xParams.pcClientCertificate = ( uint8_t * ) tlstestCLIENT_BYOC_CERTIFICATE_PEM;
    xParams.ulClientCertificateLength = tlstestCLIENT_BYOC_CERTIFICATE_PEM_LENGTH;

    prvConnectWithProvisioning( &( xParams ),
                                pdTRUE /* The connect operation should succeed. */
                                );
}
/*-----------------------------------------------------------*/
