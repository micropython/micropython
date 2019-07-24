/*
 * Amazon FreeRTOS Secure Sockets for NXP54018_IoT_Module V1.0.0 Beta 4
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

/*
 * Copyright (C) NXP 2017.
 */


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_secure_sockets.h"
#include "aws_tls.h"
#include "task.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Third-party wifi driver include. */
#include "qcom_api.h"
#include "aws_wifi.h"
#include "custom_stack_offload.h"
#include "atheros_stack_offload.h"


/**
 * @brief Flag indicating that socket send operations are not permitted.
 *
 * If a WR shutdown in SOCKETS_Shutdown() is invoked, this flag is
 * set in the socket's xShutdownFlags member.
 */
#define nxpsecuresocketsSOCKET_WRITE_CLOSED_FLAG    ( 1UL << 2 )

/**
 * @brief Flag indicating that socket receive operations are not permitted.
 *
 * If a RD shutdown in SOCKETS_Shutdown() is invoked, this flag is
 * set in the socket's xShutdownFlags member.
 */
#define nxpsecuresocketsSOCKET_READ_CLOSED_FLAG     ( 1UL << 1 )

/**
 * @brief Delay used between network select attempts when effecting a receive timeout.
 *
 * Timeouts are mocked in the secure sockets layer, and this constant sets the
 * sleep time between each read attempt during the receive timeout period.
 */
#define nxpsecuresocketsFIVE_MILLISECONDS           ( pdMS_TO_TICKS( 5 ) )

/**
 * @brief The timeout supplied to the t_select function.
 *
 * Receive timeout are emulated in secure sockets layer and therefore we
 * do not want the Inventek module to block. Setting to zero means
 * no timeout, so one is the smallest value we can set it to.
 */
#define nxpsecuresocketsONE_MILLISECOND             ( 1 )
#define nxpsecuresocketsSOCKET_CONNECTED_FLAG       ( 1 )

/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xSocket;
    char * pcDestination;
    void * pvTLSContext;
    BaseType_t xRequireTLS;
    BaseType_t xSendFlags;
    BaseType_t xRecvFlags;
    BaseType_t xShutdownFlags;
    uint32_t ulSendTimeout;
    uint32_t ulRecvTimeout;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    uint32_t ulState;
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
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext;

    /* Do not send data on unconnected socket */
    if( !( pxContext->ulState & nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
    {
        return -1;
    }

    char * sendBuf = custom_alloc( xDataLength );

    if( sendBuf == NULL )
    {
        return -1;
    }

    memcpy( sendBuf, pucData, xDataLength );
    int ret = qcom_send( ( int ) pxContext->xSocket,
                         sendBuf,
                         xDataLength,
                         pxContext->xSendFlags );
    custom_free( sendBuf );

    return ret;
}



/*-----------------------------------------------------------*/


QCA_CONTEXT_STRUCT * wlan_get_context( void );

/*
 * @brief Network receive callback.
 */
static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext;
    TickType_t xTimeOnEntering = xTaskGetTickCount();

    /* Do not receive data on unconnected socket */
    if( !( pxContext->ulState & nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
    {
        return -1;
    }

    QCA_CONTEXT_STRUCT * enetCtx = wlan_get_context();
    A_STATUS xStatus;

    char * buffLoc = NULL;
    int xRetVal = 0;

    for( ; ; )
    {
        /* Check if there is anything to be received on this socket. */
        xStatus = ( A_STATUS ) t_select( enetCtx,
                                         ( uint32_t ) pxContext->xSocket,
                                         nxpsecuresocketsONE_MILLISECOND );

        if( xStatus == A_OK ) /* Data available. */
        {
            xRetVal = qcom_recv( ( int ) pxContext->xSocket, &buffLoc, xReceiveLength, 0 );

            if( xRetVal > 0 ) /* Data received. */
            {
                memcpy( pucReceiveBuffer, buffLoc, xRetVal );
                break;
            }
            else /* Error occured. */
            {
                /*int errno = t_errno( wlan_get_context(), ( uint32_t ) pxContext->xSocket ); */
                xRetVal = SOCKETS_SOCKET_ERROR;
                break;
            }
        }
        else if( xStatus == A_ERROR ) /* A_ERROR is returned from t_select on timeout. */
        {
            if( ( xTaskGetTickCount() - xTimeOnEntering ) < pxContext->ulRecvTimeout )
            {
                vTaskDelay( nxpsecuresocketsFIVE_MILLISECONDS );
            }
            else
            {
                xRetVal = 0;
                break;
            }
        }
        else
        {
            xRetVal = SOCKETS_SOCKET_ERROR;
            break;
        }
    }

    if( buffLoc != NULL )
    {
        zero_copy_free( buffLoc );
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

/*
 * Interface routines.
 */

int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;
    int32_t lStatus = SOCKETS_ERROR_NONE;

    if( ( NULL != pxContext ) && ( SOCKETS_INVALID_SOCKET != pxContext ) )
    {
        pxContext->ulState = 0;

        if( NULL != pxContext->pcDestination )
        {
            vPortFree( pxContext->pcDestination );
        }

        if( NULL != pxContext->pcServerCertificate )
        {
            vPortFree( pxContext->pcServerCertificate );
        }

        if( pdTRUE == pxContext->xRequireTLS )
        {
            TLS_Cleanup( pxContext->pvTLSContext );
        }

        qcom_socket_close( ( int ) pxContext->xSocket );
        vPortFree( pxContext );
    }
    else
    {
        lStatus = SOCKETS_EINVAL;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t xStatus = SOCKETS_ERROR_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;
    TLSParams_t xTLSParams = { 0 };
    SOCKADDR_T xTempAddress = { 0 };

    if( ( SOCKETS_INVALID_SOCKET != pxContext ) && ( NULL != pxAddress ) && WIFI_IsConnected())
    {
        /* Connect the wrapped socket. */


        /* The driver code expects the endianess of the address and port in host order and then swaps before
         * sending the connection data to the firmware. */
        xTempAddress.sin_addr.s_addr = SOCKETS_ntohl( pxAddress->ulAddress );
        xTempAddress.sin_family = pxAddress->ucSocketDomain;
        xTempAddress.sin_port = SOCKETS_ntohs( pxAddress->usPort );
        xStatus = qcom_connect( ( int ) pxContext->xSocket,
                                ( struct sockaddr * ) &xTempAddress,
                                xAddressLength );

        /* Keep socket state - connected */
        if( SOCKETS_ERROR_NONE == xStatus )
        {
            pxContext->ulState |= nxpsecuresocketsSOCKET_CONNECTED_FLAG;
        }

        /* Negotiate TLS if requested. */
        if( ( SOCKETS_ERROR_NONE == xStatus ) && ( pdTRUE == pxContext->xRequireTLS ) )
        {
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxContext->pcDestination;
            xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
            xTLSParams.pvCallerContext = pxContext;
            xTLSParams.pxNetworkRecv = prvNetworkRecv;
            xTLSParams.pxNetworkSend = prvNetworkSend;
            xStatus = TLS_Init( &pxContext->pvTLSContext, &xTLSParams );

            if( SOCKETS_ERROR_NONE == xStatus )
            {
                xStatus = TLS_Connect( pxContext->pvTLSContext );

                /* Report positive error codes as negative number */
                xStatus = xStatus > 0 ? SOCKETS_TLS_HANDSHAKE_ERROR : xStatus;
            }
        }
    }
    else
    {
        xStatus = SOCKETS_SOCKET_ERROR;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/


/* Convert IP address in uint32_t to comma separated bytes. */
#define UINT32_IPADDR_TO_CSV_BYTES( a )                     \
    ( ( ( a ) >> 24 ) & 0xFF ), ( ( ( a ) >> 16 ) & 0xFF ), \
    ( ( ( a ) >> 8 ) & 0xFF ), ( ( a ) & 0xFF )

/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t ulAddr = 0;

    if( strlen( pcHostName ) <= ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        WIFI_GetHostIP( ( char * ) pcHostName, ( uint8_t * ) &ulAddr );
        configPRINTF( ( "Looked up %s as %d.%d.%d.%d\r\n",
                        pcHostName,
                        UINT32_IPADDR_TO_CSV_BYTES( ulAddr ) ) );
    }
    else
    {
        configPRINTF( ( "Malformed URL, Host name: %s is too long.", pcHostName ) );
    }

    /* This api is to return the address in network order. WIFI_GetHostIP returns the host IP
     * in host order.
     */
    ulAddr = SOCKETS_htonl( ulAddr );

    return ulAddr;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;


    if( ( SOCKETS_INVALID_SOCKET != xSocket ) &&
        ( NULL != pvBuffer ) &&
        ( ( nxpsecuresocketsSOCKET_READ_CLOSED_FLAG & pxContext->xShutdownFlags ) == 0UL ) &&
        ( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
        )
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
        lStatus = SOCKETS_SOCKET_ERROR;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    /* The WiFi module refuses to send data if it exceeds this threshold defined in
     * atheros_stack_offload.h */
    uint32_t ulSendMaxLength = IPV4_FRAGMENTATION_THRESHOLD;
    int32_t lWritten = 0, lWrittenPerLoop = 0;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    if( ( SOCKETS_INVALID_SOCKET != pxContext ) &&
        ( NULL != pvBuffer ) &&
        ( ( nxpsecuresocketsSOCKET_WRITE_CLOSED_FLAG & pxContext->xShutdownFlags ) == 0UL ) &&
        ( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
        )
    {
        pxContext->xSendFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* In case of TLS, reserve extra space for SSL meta data (header, maclen, ivlen, ... = 45B) */
            ulSendMaxLength = 1531;

            for(
                uint32_t ulToRemain = xDataLength, ulBufferPos = 0, ulToSend = 0;
                ulToRemain;
                ulBufferPos += ulToSend, ulToRemain -= ulToSend
                )
            {
                ulToSend = ulToRemain > ulSendMaxLength ? ulSendMaxLength : ulToRemain;
                /* Send through TLS pipe, if negotiated. */
                lWrittenPerLoop = TLS_Send( pxContext->pvTLSContext,
                                            &( ( unsigned char const * ) pvBuffer )[ ulBufferPos ],
                                            ulToSend );

                /* Error code < 0. */
                if( lWrittenPerLoop < 0 )
                {
                    /* Set the error code to be returned */
                    lWritten = lWrittenPerLoop;
                    break;
                }

                /* Number of lWritten bytes. */
                lWritten += lWrittenPerLoop;

                if( lWrittenPerLoop != ulToSend )
                {
                    break;
                }
            }
        }
        else
        {
            for(
                uint32_t ulToRemain = xDataLength, ulBufferPos = 0, ulToSend = 0;
                ulToRemain;
                ulBufferPos += ulToSend, ulToRemain -= ulToSend
                )
            {
                ulToSend = ulToRemain > ulSendMaxLength ? ulSendMaxLength : ulToRemain;
                lWrittenPerLoop = prvNetworkSend( pxContext,
                                                  &( ( unsigned char const * ) pvBuffer )[ ulBufferPos ],
                                                  ulToSend );

                /* Error code < 0. */
                if( lWrittenPerLoop < 0 )
                {
                    /* Set the error code to be returned */
                    lWritten = lWrittenPerLoop;
                    break;
                }

                /* Number of lWritten bytes. */
                lWritten += lWrittenPerLoop;

                if( lWrittenPerLoop != ulToSend )
                {
                    break;
                }
            }
        }
    }
    else
    {
        lWritten = SOCKETS_SOCKET_ERROR;
    }

    return lWritten;
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
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    if( ( NULL != pxContext ) && ( SOCKETS_INVALID_SOCKET != pxContext ) )
    {
        switch( lOptionName )
        {
            case SOCKETS_SO_SERVER_NAME_INDICATION:

                /* Secure socket option cannot be used on connected socket */
                if( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
                {
                    lStatus = SOCKETS_SOCKET_ERROR;
                    break;
                }

                /* Non-NULL destination string indicates that SNI extension should
                 * be used during TLS negotiation. */
                if( NULL == ( pxContext->pcDestination =
                                  ( char * ) pvPortMalloc( 1 + xOptionLength ) ) )
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

                /* Secure socket option cannot be used on connected socket */
                if( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
                {
                    lStatus = SOCKETS_SOCKET_ERROR;
                    break;
                }

                /* Non-NULL server certificate field indicates that the default trust
                 * list should not be used. */
                if( NULL == ( pxContext->pcServerCertificate =
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

                /* Secure socket option cannot be used on connected socket */
                if( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
                {
                    lStatus = SOCKETS_SOCKET_ERROR;
                    break;
                }

                pxContext->xRequireTLS = pdTRUE;
                break;

            case SOCKETS_SO_NONBLOCK:

                if( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
                {
                    xTimeout = 0;
                    /* TODO: Investigate the NONBLOCK compile time config. */
                    pxContext->ulSendTimeout = 1;
                    pxContext->ulRecvTimeout = 1;
                }
                else
                {
                    lStatus = SOCKETS_SOCKET_ERROR;
                }

                break;

            case SOCKETS_SO_RCVTIMEO:
                xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t. */

                if( xTimeout == 0U )
                {
                    pxContext->ulRecvTimeout = portMAX_DELAY;
                }
                else
                {
                    pxContext->ulRecvTimeout = xTimeout;
                }

                break;

            case SOCKETS_SO_SNDTIMEO:
                /* Comply with Berkeley standard - a 0 timeout is wait forever. */
                xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t. */

                if( xTimeout == 0U )
                {
                    pxContext->ulSendTimeout = portMAX_DELAY;
                }
                else
                {
                    pxContext->ulSendTimeout = xTimeout;
                }

                break;

            case SOCKETS_SO_ALPN_PROTOCOLS:

                /* Secure socket option cannot be used on connected socket */
                if( pxContext->ulState & ( nxpsecuresocketsSOCKET_CONNECTED_FLAG ) )
                {
                    lStatus = SOCKETS_SOCKET_ERROR;
                    break;
                }

                /* NOT implemented ? */
                break;

            default:
                lStatus = qcom_setsockopt( ( int ) pxContext->xSocket,
                                           lLevel,
                                           lOptionName,
                                           ( void * ) pvOptionValue,
                                           xOptionLength );
                break;
        }
    }
    else
    {
        lStatus = SOCKETS_SOCKET_ERROR;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    SSOCKETContextPtr_t pxSecureSocket = ( SSOCKETContextPtr_t ) xSocket;
    int32_t lRetVal = SOCKETS_SOCKET_ERROR;

    /* Ensure that a valid socket was passed. */
    if( ( SOCKETS_INVALID_SOCKET != pxSecureSocket ) )
    {
        switch( ulHow )
        {
            case SOCKETS_SHUT_RD:
                /* Further receive calls on this socket should return error. */
                pxSecureSocket->xShutdownFlags |= nxpsecuresocketsSOCKET_READ_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            case SOCKETS_SHUT_WR:
                /* Further send calls on this socket should return error. */
                pxSecureSocket->xShutdownFlags |= nxpsecuresocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            case SOCKETS_SHUT_RDWR:
                /* Further send or receive calls on this socket should return error. */
                pxSecureSocket->xShutdownFlags |= nxpsecuresocketsSOCKET_READ_CLOSED_FLAG;
                pxSecureSocket->xShutdownFlags |= nxpsecuresocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            default:
                /* An invalid value was passed for ulHow. */
                lRetVal = SOCKETS_EINVAL;
                break;
        }
    }
    else
    {
        return SOCKETS_EINVAL;
    }

    return lRetVal;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    int32_t xSocket = 0;
    SSOCKETContextPtr_t pxContext = NULL;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

    /* Allocate the internal context structure. */
    pxContext = pvPortMalloc( sizeof( SSOCKETContext_t ) );

    if( pxContext != NULL )
    {
        memset( pxContext, 0, sizeof( SSOCKETContext_t ) );

        /* Create the wrapped socket. */
        xSocket = qcom_socket( ATH_AF_INET,
                               SOCK_STREAM_TYPE,
                               0 ); /*xProtocol*/

        if( xSocket != A_ERROR )
        {
            pxContext->xSocket = ( Socket_t ) xSocket;
            /* Set default timeouts. */
            pxContext->ulRecvTimeout = socketsconfigDEFAULT_RECV_TIMEOUT;
            pxContext->ulSendTimeout = socketsconfigDEFAULT_SEND_TIMEOUT;
        }
        else /* Driver could not allocate socket. */
        {
            lStatus = SOCKETS_SOCKET_ERROR;
            vPortFree( pxContext );
        }
    }
    else /* Malloc failed. */
    {
        lStatus = SOCKETS_ENOMEM;
    }

    if( lStatus != SOCKETS_ERROR_NONE )
    {
        pxContext = SOCKETS_INVALID_SOCKET;
    }

    return pxContext;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    /* Empty initialization for NXP board. */
    return pdPASS;
}
