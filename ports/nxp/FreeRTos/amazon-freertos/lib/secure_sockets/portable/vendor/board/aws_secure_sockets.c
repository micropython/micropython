/*
 * Amazon FreeRTOS Secure Socket V1.0.0
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

/**
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* Socket and WiFi interface includes. */
#include "aws_secure_sockets.h"

/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    /* FIX ME. */
    return ( Socket_t ) SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Accept( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t * pxAddressLength )
{
    /* FIX ME. */
    return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    /* FIX ME. */
    return 0;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    /* FIX ME. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

uint32_t ulRand( void )
{
    /* FIX ME. */
    return 0;
}
/*-----------------------------------------------------------*/
