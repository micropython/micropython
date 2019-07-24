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
 * @file aws_helper_secure_connect.h
 * @brief Header file for helper_secure_connect.
 */

#ifndef _AWS_HELPER_SECURE_CONNECT_H_
#define _AWS_HELPER_SECURE_CONNECT_H_
#include "aws_greengrass_discovery.h"

/*
 * @brief Start a secure connection on the desired port.
 *
 * @param [in] pxHostParams : Host parameters used to connect to the host.
 *
 *
 * @param [in] ulReceiveTimeOut : Receive Timeout in millisecond.
 *
 * @param [in] ulSendTimeOut : Send Timeout in millisecond
 * 
 * @param [out] pvTLSCtx: Opaque context.
 *
 * @param [out] xSocket : Returned socket type.
 *
 * @return If the connection was successful then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_SecureConnect_Connect( const GGD_HostAddressData_t * pxHostAddressData,
                                      Socket_t * pxSocket,
                                      uint32_t ulReceiveTimeOut,
                                      uint32_t ulSendTimeOut );

/*
 * @briefstop a secure connection with host.
 *
 * @param [in] pxSocket : Socket to disconnect from.Sets to SOCKETS_INVALID_SOCKET.
 */
void GGD_SecureConnect_Disconnect( Socket_t * pxSocket );

/*
 * @brief Send data to the host
 *
 * NOTE:  The memory for the page file is allocated by the user.
 *
 * @param [in] ulPayloadSize: Data to be sent.
 *   Could be an HTTP command!
 *   eg: "GET /greengrass/discover/thing/-your thing name-".
 *
 * @param [in] ulPayloadSize: Data size in bytes.
 *
 * @param [in] xSocket: Socket.
 *
 * @return pdPASS if GET was sent successfully
 * returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_SecureConnect_Send( const char * pcPayload,
                                   const uint32_t ulPayloadSize,
                                   const Socket_t xSocket );

/*
 * @brief Get the response from the host.
 *
 * NOTE:  The memory for the file is allocated by the user.
 *
 * @param [in] pucPayload: Pointer to an empty file type in memory.
 *             The received file will be returned at this address.
 *
 * @param [in] ulPayloadSize: Data size in bytes.
 *
 * @param [in] xSocket: Socket.
 *
 * @param [out] ulDataRecvSize: The size of the databuffer received.
 *
 * @return If the JSON file was received successfully then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 *
 *
 */
BaseType_t GGD_SecureConnect_Read( char * pcBuffer,
                                   const uint32_t ulBufferSize,
                                   const Socket_t xSocket,
                                   uint32_t * pulDataRecvSize );

#endif /* _AWS_HELPER_SECURE_CONNECT_H_ */
