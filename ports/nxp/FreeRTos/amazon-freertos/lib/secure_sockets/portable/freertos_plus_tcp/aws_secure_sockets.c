/*
 * Amazon FreeRTOS Secure Sockets for FreeRTOS+TCP V1.1.5
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "aws_secure_sockets.h"
#include "aws_tls.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_crypto.h"

/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xSocket;
    char * pcDestination;
    void * pvTLSContext;
    BaseType_t xRequireTLS;
    BaseType_t xSendFlags;
    BaseType_t xRecvFlags;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    char ** ppcAlpnProtocols;
    uint32_t ulAlpnProtocolsCount;
    BaseType_t xConnectAttempted;
} SSOCKETContext_t, * SSOCKETContextPtr_t;

/*
 * Helper routines.
 */

/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

    return FreeRTOS_send( pxContext->xSocket, pucData, xDataLength, pxContext->xSendFlags );
}
/*-----------------------------------------------------------*/

/*
 * @brief Network receive callback.
 */
static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

    return FreeRTOS_recv( pxContext->xSocket, pucReceiveBuffer, xReceiveLength, pxContext->xRecvFlags );
}
/*-----------------------------------------------------------*/

/*
 * Interface routines.
 */

int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */
    uint32_t ulProtocol;
    int32_t lReturn;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) && ( NULL != pxContext ) )
    {
        /* Clean-up destination string. */
        if( NULL != pxContext->pcDestination )
        {
            vPortFree( pxContext->pcDestination );
        }

        /* Clean-up server certificate. */
        if( NULL != pxContext->pcServerCertificate )
        {
            vPortFree( pxContext->pcServerCertificate );
        }

        /* Clean-up application protocol array. */
        if( NULL != pxContext->ppcAlpnProtocols )
        {
            for( ulProtocol = 0;
                 ulProtocol < pxContext->ulAlpnProtocolsCount;
                 ulProtocol++ )
            {
                if( NULL != pxContext->ppcAlpnProtocols[ ulProtocol ] )
                {
                    vPortFree( pxContext->ppcAlpnProtocols[ ulProtocol ] );
                }
            }

            vPortFree( pxContext->ppcAlpnProtocols );
        }

        /* Clean-up TLS context. */
        if( pdTRUE == pxContext->xRequireTLS )
        {
            TLS_Cleanup( pxContext->pvTLSContext );
        }

        /* Close the underlying socket handle. */
        ( void ) FreeRTOS_closesocket( pxContext->xSocket );

        /* Free the context. */
        vPortFree( pxContext );
        lReturn = SOCKETS_ERROR_NONE;
    }
    else
    {
        lReturn = SOCKETS_EINVAL;
    }

    return lReturn;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */
    TLSParams_t xTLSParams = { 0 };
    struct freertos_sockaddr xTempAddress = { 0 };

    if( ( pxContext != SOCKETS_INVALID_SOCKET ) && ( pxAddress != NULL ) )
    {
        /* A connection was attempted. If this function fails, then the socket is invalid and the user
         * must call SOCKETS_Close(), on this socket, and SOCKETS_Socket() to get a new socket. */
        pxContext->xConnectAttempted = pdTRUE;

        /* Connect the wrapped socket. */
        xTempAddress.sin_addr = pxAddress->ulAddress;
        xTempAddress.sin_family = pxAddress->ucSocketDomain;
        xTempAddress.sin_len = ( uint8_t ) sizeof( xTempAddress );
        xTempAddress.sin_port = pxAddress->usPort;
        lStatus = FreeRTOS_connect( pxContext->xSocket, &xTempAddress, xAddressLength );

        /* Negotiate TLS if requested. */
        if( ( SOCKETS_ERROR_NONE == lStatus ) && ( pdTRUE == pxContext->xRequireTLS ) )
        {
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxContext->pcDestination;
            xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
            xTLSParams.ppcAlpnProtocols = ( const char ** ) pxContext->ppcAlpnProtocols;
            xTLSParams.ulAlpnProtocolsCount = pxContext->ulAlpnProtocolsCount;
            xTLSParams.pvCallerContext = pxContext;
            xTLSParams.pxNetworkRecv = prvNetworkRecv;
            xTLSParams.pxNetworkSend = prvNetworkSend;
            lStatus = TLS_Init( &pxContext->pvTLSContext, &xTLSParams );

            if( SOCKETS_ERROR_NONE == lStatus )
            {
                lStatus = TLS_Connect( pxContext->pvTLSContext );

                if( lStatus < 0 )
                {
                    lStatus = SOCKETS_TLS_HANDSHAKE_ERROR;
                }
            }
        }
    }
    else
    {
        lStatus = SOCKETS_SOCKET_ERROR;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    return FreeRTOS_gethostbyname( pcHostName );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        pxContext->xRecvFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Receive through TLS pipe, if negotiated. */
            lStatus = TLS_Recv( pxContext->pvTLSContext, pvBuffer, xBufferLength );
        }
        else
        {
            /* Receive unencrypted. */
            lStatus = prvNetworkRecv( pxContext, pvBuffer, xBufferLength );
        }
    }
    else
    {
        lStatus = SOCKETS_EINVAL;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        pxContext->xSendFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Send through TLS pipe, if negotiated. */
            lStatus = TLS_Send( pxContext->pvTLSContext, pvBuffer, xDataLength );
        }
        else
        {
            /* Send unencrypted. */
            lStatus = prvNetworkSend( pxContext, pvBuffer, xDataLength );
        }
    }
    else
    {
        lStatus = SOCKETS_EINVAL;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    TickType_t xTimeout;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */
    char ** ppcAlpnIn = ( char ** ) pvOptionValue;
    size_t xLength = 0;
    uint32_t ulProtocol;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) && ( xSocket != NULL ) )
    {
        switch( lOptionName )
        {
            case SOCKETS_SO_SERVER_NAME_INDICATION:

                /* Do not set the SNI options if the socket is possibly already connected. */
                if( pxContext->xConnectAttempted == pdTRUE )
                {
                    lStatus = SOCKETS_EISCONN;
                }

                /* Non-NULL destination string indicates that SNI extension should
                 * be used during TLS negotiation. */
                else if( NULL == ( pxContext->pcDestination =
                                       ( char * ) pvPortMalloc( 1U + xOptionLength ) ) )
                {
                    lStatus = SOCKETS_ENOMEM;
                }
                else
                {
                    memcpy( pxContext->pcDestination, pvOptionValue, xOptionLength );
                    pxContext->pcDestination[ xOptionLength ] = '\0';
                }

                break;

            case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

                /* Do not set the trusted server certificate if the socket is possibly already connected. */
                if( pxContext->xConnectAttempted == pdTRUE )
                {
                    lStatus = SOCKETS_EISCONN;
                }

                /* Non-NULL server certificate field indicates that the default trust
                 * list should not be used. */
                else if( NULL == ( pxContext->pcServerCertificate =
                                       ( char * ) pvPortMalloc( xOptionLength ) ) )
                {
                    lStatus = SOCKETS_ENOMEM;
                }
                else
                {
                    memcpy( pxContext->pcServerCertificate, pvOptionValue, xOptionLength );
                    pxContext->ulServerCertificateLength = xOptionLength;
                }

                break;

            case SOCKETS_SO_REQUIRE_TLS:

                /* Do not set the TLS option if the socket is possibly already connected. */
                if( pxContext->xConnectAttempted == pdTRUE )
                {
                    lStatus = SOCKETS_EISCONN;
                }
                else
                {
                    pxContext->xRequireTLS = pdTRUE;
                }

                break;

            case SOCKETS_SO_ALPN_PROTOCOLS:

                /* Do not set the ALPN option if the socket is already connected. */
                if( pxContext->xConnectAttempted == pdTRUE )
                {
                    lStatus = SOCKETS_EISCONN;
                    break;
                }

                /* Allocate a sufficiently long array of pointers. */
                pxContext->ulAlpnProtocolsCount = 1 + xOptionLength;

                if( NULL == ( pxContext->ppcAlpnProtocols =
                                  ( char ** ) pvPortMalloc( pxContext->ulAlpnProtocolsCount * sizeof( char * ) ) ) )
                {
                    lStatus = SOCKETS_ENOMEM;
                }
                else
                {
                    /* Zero out the pointers. */
                    memset( pxContext->ppcAlpnProtocols,
                            0,
                            pxContext->ulAlpnProtocolsCount * sizeof( char * ) );
                }

                /* Copy each protocol string. */
                for( ulProtocol = 0;
                     ( ulProtocol < pxContext->ulAlpnProtocolsCount - 1 ) &&
                     ( pdFREERTOS_ERRNO_NONE == lStatus );
                     ulProtocol++ )
                {
                    xLength = strlen( ppcAlpnIn[ ulProtocol ] );

                    if( NULL == ( pxContext->ppcAlpnProtocols[ ulProtocol ] =
                                      ( char * ) pvPortMalloc( 1 + xLength ) ) )
                    {
                        lStatus = SOCKETS_ENOMEM;
                    }
                    else
                    {
                        memcpy( pxContext->ppcAlpnProtocols[ ulProtocol ],
                                ppcAlpnIn[ ulProtocol ],
                                xLength );
                        pxContext->ppcAlpnProtocols[ ulProtocol ][ xLength ] = '\0';
                    }
                }

                break;

            case SOCKETS_SO_NONBLOCK:
                xTimeout = 0;

                /* Non-blocking connect is not supported.  Socket may be set to nonblocking
                 * only after a connection is made. */
                if( pdTRUE == pxContext->xConnectAttempted )
                {
                    lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                                   lLevel,
                                                   SOCKETS_SO_RCVTIMEO,
                                                   &xTimeout,
                                                   sizeof( xTimeout ) );

                    if( lStatus == SOCKETS_ERROR_NONE )
                    {
                        lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                                       lLevel,
                                                       SOCKETS_SO_SNDTIMEO,
                                                       &xTimeout,
                                                       sizeof( xTimeout ) );
                    }
                }
                else
                {
                    lStatus = SOCKETS_EISCONN;
                }

                break;

            case SOCKETS_SO_RCVTIMEO:
            case SOCKETS_SO_SNDTIMEO:
                /* Comply with Berkeley standard - a 0 timeout is wait forever. */
                xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t */

                if( xTimeout == 0U )
                {
                    xTimeout = portMAX_DELAY;
                }

                lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                               lLevel,
                                               lOptionName,
                                               &xTimeout,
                                               xOptionLength );
                break;

            default:
                lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                               lLevel,
                                               lOptionName,
                                               pvOptionValue,
                                               xOptionLength );
                break;
        }
    }
    else
    {
        lStatus = SOCKETS_EINVAL;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    int32_t lReturn;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) && ( xSocket != NULL ) )
    {
        lReturn = FreeRTOS_shutdown( pxContext->xSocket, ( BaseType_t ) ulHow );
    }
    else
    {
        lReturn = SOCKETS_EINVAL;
    }

    return lReturn;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    SSOCKETContextPtr_t pxContext = NULL;
    Socket_t xSocket;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

    /* Create the wrapped socket. */
    xSocket = FreeRTOS_socket( lDomain, lType, lProtocol );

    if( xSocket != FREERTOS_INVALID_SOCKET )
    {
        /* Allocate the internal context structure. */
        if( NULL == ( pxContext = pvPortMalloc( sizeof( SSOCKETContext_t ) ) ) )
        {
            /* Need to close socket. */
            ( void ) FreeRTOS_closesocket( xSocket );
            pxContext = SOCKETS_INVALID_SOCKET;
        }
        else
        {
            memset( pxContext, 0, sizeof( SSOCKETContext_t ) );
            pxContext->xSocket = xSocket;
        }
    }
    else
    {
        pxContext = SOCKETS_INVALID_SOCKET;
    }

    return pxContext;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    /* Empty initialization for this port. */
    return pdPASS;
}
/*-----------------------------------------------------------*/

static CK_RV prvSocketsGetCryptoSession( CK_SESSION_HANDLE * pxSession,
                                         CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    static CK_SESSION_HANDLE xPkcs11Session = 0;
    static CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    CK_ULONG ulCount = 1;
    CK_SLOT_ID xSlotId = 0;

    portENTER_CRITICAL();

    if( 0 == xPkcs11Session )
    {
        /* One-time initialization. */

        /* Ensure that the PKCS#11 module is initialized. */
        if( 0 == xResult )
        {
            pxCkGetFunctionList = C_GetFunctionList;
            xResult = pxCkGetFunctionList( &pxPkcs11FunctionList );
        }

        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_Initialize( NULL );
        }

        /* Get the default slot ID. */
        if( ( 0 == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
        {
            xResult = pxPkcs11FunctionList->C_GetSlotList( CK_TRUE,
                                                           &xSlotId,
                                                           &ulCount );
        }

        /* Start a session with the PKCS#11 module. */
        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_OpenSession( xSlotId,
                                                           CKF_SERIAL_SESSION,
                                                           NULL,
                                                           NULL,
                                                           &xPkcs11Session );
        }
    }

    portEXIT_CRITICAL();

    /* Output the shared function pointers and session handle. */
    *ppxFunctionList = pxPkcs11FunctionList;
    *pxSession = xPkcs11Session;

    return xResult;
}
/*-----------------------------------------------------------*/

uint32_t ulRand( void )
{
    CK_RV xResult = 0;
    CK_SESSION_HANDLE xPkcs11Session = 0;
    CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    uint32_t ulRandomValue = 0;

    xResult = prvSocketsGetCryptoSession( &xPkcs11Session,
                                          &pxPkcs11FunctionList );

    if( 0 == xResult )
    {
        /* Request a sequence of cryptographically random byte values using
         * PKCS#11. */
        xResult = pxPkcs11FunctionList->C_GenerateRandom( xPkcs11Session,
                                                          ( CK_BYTE_PTR ) &ulRandomValue,
                                                          sizeof( ulRandomValue ) );
    }

    /* Check if any of the API calls failed. */
    if( 0 != xResult )
    {
        ulRandomValue = 0;
    }

    return ulRandomValue;
}
/*-----------------------------------------------------------*/

/**
 * @brief Generate a TCP Initial Sequence Number that is reasonably difficult
 * to predict, per https://tools.ietf.org/html/rfc6528.
 */
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    CK_RV xResult = 0;
    CK_SESSION_HANDLE xPkcs11Session = 0;
    CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    CK_MECHANISM xMechSha256 = { 0 };
    uint8_t ucSha256Result[ cryptoSHA256_DIGEST_BYTES ];
    CK_ULONG ulLength = sizeof( ucSha256Result );
    uint32_t ulNextSequenceNumber = 0;
    static uint64_t ullKey = 0;

    /* Acquire a crypto session handle. */
    xResult = prvSocketsGetCryptoSession( &xPkcs11Session,
                                          &pxPkcs11FunctionList );

    if( 0 == xResult )
    {
        if( 0 == ullKey )
        {
            /* One-time initialization, per boot, of the random seed. */
            xResult = pxPkcs11FunctionList->C_GenerateRandom( xPkcs11Session,
                                                              ( CK_BYTE_PTR ) &ullKey,
                                                              sizeof( ullKey ) );
        }
    }

    /* Lock the shared crypto session. */
    portENTER_CRITICAL();

    /* Start a hash. */
    if( 0 == xResult )
    {
        xMechSha256.mechanism = CKM_SHA256;
        xResult = pxPkcs11FunctionList->C_DigestInit( xPkcs11Session, &xMechSha256 );
    }

    /* Hash the seed. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ullKey,
                                                        sizeof( ullKey ) );
    }

    /* Hash the source address. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ulSourceAddress,
                                                        sizeof( ulSourceAddress ) );
    }

    /* Hash the source port. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &usSourcePort,
                                                        sizeof( usSourcePort ) );
    }

    /* Hash the destination address. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ulDestinationAddress,
                                                        sizeof( ulDestinationAddress ) );
    }

    /* Hash the destination port. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &usDestinationPort,
                                                        sizeof( usDestinationPort ) );
    }

    /* Get the hash. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestFinal( xPkcs11Session,
                                                       ucSha256Result,
                                                       &ulLength );
    }

    portEXIT_CRITICAL();

    /* Use the first four bytes of the hash result as the starting point for
     * all initial sequence numbers for connections based on the input 4-tuple. */
    if( 0 == xResult )
    {
        memcpy( &ulNextSequenceNumber,
                ucSha256Result,
                sizeof( ulNextSequenceNumber ) );

        /* Add the tick count of four-tick intervals. In theory, per the RFC
         * (see above), this approach still allows server equipment to optimize
         * handling of connections from the same device that haven't fully timed out. */
        ulNextSequenceNumber += xTaskGetTickCount() / 4;
    }

    return ulNextSequenceNumber;
}
/*-----------------------------------------------------------*/
