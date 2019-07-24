/*
 * Amazon FreeRTOS Greengrass Discovery V1.0.3
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
 * @file aws_helper_secure_connect.c
 * @brief Secure connection wrapper
 *
 * API used by green grass discovery to help establishing
 * secure connect with a given host
 */

/* SOCKETS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Helper interface includes. */
#include "aws_helper_secure_connect.h"
#include "aws_ggd_config.h"
#include "aws_ggd_config_defaults.h"

/* Standard includes. */
#include <string.h>

#define helperMAX_IP_ADDRESS_OCTETS    4u

/**
 * @brief This function return non 0 if it is an IP and 0 if it isn't
 */
static uint32_t prvIsIPaddress( const char * pcIPAddress );

/*-----------------------------------------------------------*/

BaseType_t GGD_SecureConnect_Connect( const GGD_HostAddressData_t * pxHostAddressData,
                                      Socket_t * pxSocket,
                                      uint32_t ulReceiveTimeOut,
                                      uint32_t ulSendTimeOut )
{
    const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( ulReceiveTimeOut );
    const TickType_t xSendTimeOut = pdMS_TO_TICKS( ulSendTimeOut );
    SocketsSockaddr_t xServerAddress;
    size_t xURLLength;
    BaseType_t xIsIPAddress;
    BaseType_t xStatus;

    configASSERT( pxHostAddressData != NULL );
    configASSERT( pxSocket != NULL );

    /* Calculate the length of the supplied URL. */
    xURLLength = strlen( pxHostAddressData->pcHostAddress );

    /* Ensure that the length of the specified URL is
     * within the permitted limits. */
    if( xURLLength <= ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        /* Create the socket. */
        *pxSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                    SOCKETS_SOCK_STREAM,
                                    SOCKETS_IPPROTO_TCP );

        if( *pxSocket == SOCKETS_INVALID_SOCKET )
        {
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }

        if( xStatus == pdPASS )
        {
            if( prvIsIPaddress( pxHostAddressData->pcHostAddress ) == ( uint32_t ) 0 )
            {
                xIsIPAddress = pdFALSE;
            }
            else
            {
                xIsIPAddress = pdTRUE;
            }

            xServerAddress.ucLength = sizeof( SocketsSockaddr_t );
            xServerAddress.usPort = SOCKETS_htons( pxHostAddressData->usPort );
            xServerAddress.ulAddress =
                SOCKETS_GetHostByName( pxHostAddressData->pcHostAddress );
            xServerAddress.ucSocketDomain = SOCKETS_AF_INET;

            /* Set send timeout for the socket. */
            ( void ) SOCKETS_SetSockOpt( *pxSocket,
                                         0,
                                         SOCKETS_SO_SNDTIMEO,
                                         &xSendTimeOut,
                                         sizeof( xSendTimeOut ) );

            /* Set receive timeout for the socket. */
            ( void ) SOCKETS_SetSockOpt( *pxSocket,
                                         0,
                                         SOCKETS_SO_RCVTIMEO,
                                         &xReceiveTimeOut,
                                         sizeof( xReceiveTimeOut ) );

            /* Set secure connection. */
            ( void ) SOCKETS_SetSockOpt( *pxSocket,
                                         0,
                                         SOCKETS_SO_REQUIRE_TLS,
                                         NULL,
                                         ( size_t ) 0 );

            if( pxHostAddressData->pcCertificate != NULL )
            {
                if( SOCKETS_SetSockOpt( *pxSocket,
                                        0,
                                        SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                        pxHostAddressData->pcCertificate,
                                        ( size_t ) pxHostAddressData->ulCertificateSize )
                    != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }
            }

            if( xIsIPAddress == pdFALSE )
            {
                if( SOCKETS_SetSockOpt( *pxSocket,
                                        0,
                                        SOCKETS_SO_SERVER_NAME_INDICATION,
                                        pxHostAddressData->pcHostAddress,
                                        ( size_t ) 1 + xURLLength )
                    != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }
            }

            /* Establish the TCP connection. */
            if( pdPASS == xStatus )
            {
                if( ( SOCKETS_Connect( *pxSocket,
                                       &xServerAddress,
                                       ( uint32_t ) sizeof( xServerAddress ) )
                      != SOCKETS_ERROR_NONE ) )
                {
                    GGD_SecureConnect_Disconnect( pxSocket );
                    xStatus = pdFAIL;
                }
            }
        }
    }
    else
    {
        ggdconfigPRINT( "Malformed URL\r\n" );
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

void GGD_SecureConnect_Disconnect( Socket_t * pxSocket )
{
    const TickType_t xShortDelay = pdMS_TO_TICKS( 10 );
    TickType_t xTicksToWait = xShortDelay * ( TickType_t ) 100;
    TimeOut_t xTimeOut;
    char cBuf; /*lint !e971 can use char without signed/unsigned. */

    configASSERT( pxSocket != NULL );
    configASSERT( *pxSocket != SOCKETS_INVALID_SOCKET );
    ggdconfigPRINT( "About to close socket.\r\n" );

    /* Initialize xTimeOut.  This records the time at which this function was
     * entered. */
    vTaskSetTimeOutState( &xTimeOut );

    /* Shutdown the connection. */
    ( void ) SOCKETS_Shutdown( *pxSocket, ( uint32_t ) SOCKETS_SHUT_RDWR );

    /* Wait for the socket to disconnect gracefully (indicated by a
     * SOCKETS_EINVAL error) before closing the socket. */
    while( SOCKETS_Recv( *pxSocket, &cBuf, sizeof( cBuf ), ( uint32_t ) 0 ) >= 0 )
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
    ( void ) SOCKETS_Close( *pxSocket );
    *pxSocket = SOCKETS_INVALID_SOCKET;
    ggdconfigPRINT( "Socket closed.\r\n" );

    #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
        {
            configPRINTF( (
                              "Stack high watermark for discovery helper task: %u.\r\n",
                              uxTaskGetStackHighWaterMark( NULL ) ) );
        }
    #endif
}
/*-----------------------------------------------------------*/


BaseType_t GGD_SecureConnect_Send( const char * pcPayload, /*lint !e971 can use char without signed/unsigned. */
                                   const uint32_t ulPayloadSize,
                                   const Socket_t xSocket )
{
    BaseType_t xStatus = pdFAIL;
    uint32_t ulBytesSent = 0;
    int32_t lSendRetVal;

    configASSERT( pcPayload != NULL );

    /* Keep re-trying until timeout or any error
     * other than SOCKETS_EWOULDBLOCK occurs. */

    if( ulPayloadSize > 0 )
    {
        while( ulBytesSent < ulPayloadSize )
        {
            /* Try sending the remaining data. */
            lSendRetVal = SOCKETS_Send( xSocket,
                                        ( const unsigned char * ) &pcPayload[ ulBytesSent ],
                                        ( size_t ) ( ulPayloadSize - ulBytesSent ),
                                        ( uint32_t ) 0 );

            /* A negative return value from SOCKETS_Send
             * means some error occurred. */
            if( lSendRetVal < 0 )
            {
                break;
            }
            else
            {
                /* Update the count of sent bytes. */
                ulBytesSent += ( uint32_t ) lSendRetVal;
            }
        }

        if( ulBytesSent == ulPayloadSize )
        {
            xStatus = pdPASS;
        }
        else
        {
            ggdconfigPRINT( "SecureConnect - error sending secure data to network\r\n" );
            xStatus = pdFAIL;
        }
    }
    else
    {
        xStatus = pdPASS;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

BaseType_t GGD_SecureConnect_Read( char * pcBuffer, /*lint !e971 can use char without signed/unsigned. */
                                   const uint32_t ulBufferSize,
                                   const Socket_t xSocket,
                                   uint32_t * pulDataRecvSize )
{
    int32_t lTmpStatus = 0;
    BaseType_t xStatus = pdFAIL;
    uint16_t usNbRetry;

    configASSERT( pulDataRecvSize != NULL );
    configASSERT( pcBuffer != NULL );

    for( usNbRetry = 0; usNbRetry < ( uint16_t ) ggdconfigTCP_RECEIVE_RETRY; usNbRetry++ )
    {
        lTmpStatus = SOCKETS_Recv( xSocket, ( unsigned char * ) pcBuffer,
                                   ( uint32_t ) ulBufferSize,
                                   ( uint32_t ) 0 );

        /* Check if it is a Timeout. */
        if( lTmpStatus != 0 )
        {
            if( lTmpStatus < 0 )
            {
                ggdconfigPRINT( "SecureConnect - recv error, %d\r\n", lTmpStatus );

                xStatus = pdFAIL;
            }
            else
            {
                xStatus = pdPASS;
            }

            /* If it is not a timeout, break. */
            break;
        }
        else
        {
            /* It is a timeout, retry. */
            ggdconfigPRINT( "SecureConnect - recv Timeout\r\n" );
        }
    }

    if( usNbRetry == ( uint16_t ) ggdconfigTCP_RECEIVE_RETRY )
    {
        ggdconfigPRINT( "SecureConnect - recv number of Timeout exceeded\r\n" );
        xStatus = pdFAIL;
    }

    *pulDataRecvSize = ( uint32_t ) lTmpStatus;

    return xStatus;
}
/*-----------------------------------------------------------*/


static uint32_t prvIsIPaddress( const char * pcIPAddress )
{
    const uint32_t ulDecimalBase = 10u;
    uint8_t ucOctet[ helperMAX_IP_ADDRESS_OCTETS ];
    const char * pcPointerOnEntering;
    uint32_t ulReturn = 0UL, ulValue;
    UBaseType_t uxOctetNumber;
    BaseType_t xResult = pdPASS;

    for( uxOctetNumber = 0u; uxOctetNumber < helperMAX_IP_ADDRESS_OCTETS; uxOctetNumber++ )
    {
        ulValue = 0UL;
        pcPointerOnEntering = pcIPAddress;

        while( ( *pcIPAddress >= '0' ) && ( *pcIPAddress <= '9' ) )
        {
            /* Move previous read characters into the next decimal
             * position. */
            ulValue *= ulDecimalBase;

            /* Add the binary value of the ascii character. */
            ulValue += ( ( uint32_t ) ( *pcIPAddress ) - ( uint32_t ) '0' );

            /* Move to next character in the string. */
            pcIPAddress++;
        }

        /* Check characters were read. */
        if( pcIPAddress == pcPointerOnEntering )
        {
            xResult = pdFAIL;
        }

        /* Check the value fits in an 8-bit number. */
        if( ulValue > 0xffUL )
        {
            xResult = pdFAIL;
        }
        else
        {
            ucOctet[ uxOctetNumber ] = ( uint8_t ) ulValue;

            /* Check the next character is as expected. */
            if( uxOctetNumber < ( helperMAX_IP_ADDRESS_OCTETS - 1u ) )
            {
                if( *pcIPAddress != '.' )
                {
                    xResult = pdFAIL;
                }
                else
                {
                    /* Move past the dot. */
                    pcIPAddress++;
                }
            }
        }

        if( xResult == pdFAIL )
        {
            /* No point going on. */
            break;
        }
    }

    if( *pcIPAddress != ( char ) 0 )
    {
        /* Expected the end of the string. */
        xResult = pdFAIL;
    }

    if( uxOctetNumber != helperMAX_IP_ADDRESS_OCTETS )
    {
        /* Didn't read enough octets. */
        xResult = pdFAIL;
    }

    if( xResult == pdPASS )
    {
        ulReturn = SOCKETS_inet_addr_quick( ucOctet[ 0 ], ucOctet[ 1 ], ucOctet[ 2 ], ucOctet[ 3 ] );
    }

    return ulReturn;
}
