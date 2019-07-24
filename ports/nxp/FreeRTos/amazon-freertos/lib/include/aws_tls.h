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


#ifndef __AWS__TLS__H__
#define __AWS__TLS__H__

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include aws_tls.h"
#endif

/**
 * @brief Defines callback type for receiving bytes from the network.
 *
 * @param[in] pvCallerContext Opaque context handle provided by caller.
 * @param[out] pucReceiveBuffer Buffer to fill with received data.
 * @param[in] xReceiveLength Length of previous parameter in bytes.
 *
 * @return The number of bytes actually read.
 */
typedef BaseType_t ( * NetworkRecv_t ) ( void * pvCallerContext,
                                         unsigned char * pucReceiveBuffer,
                                         size_t xReceiveLength );

/**
 * @brief Defines callback type for sending bytes to the network.
 *
 * @param[in] pvCallerContext Opaque context handle provided by caller.
 * @param[out] pucReceiveBuffer Buffer of data to send.
 * @param[in] xReceiveLength Length of previous parameter in bytes.
 *
 * @return The number of bytes actually sent.
 */
typedef BaseType_t ( * NetworkSend_t ) ( void * pvCallerContext,
                                         const unsigned char * pucData,
                                         size_t xDataLength );

/**
 * @brief Defines parameter structure for initializing the TLS interface.
 *
 * @param[in] ulSize Size of the structure in bytes.
 * @param[in] pcDestination Network name of the TLS server.
 * @param[in] pcServerCertificate PEM encoded server certificate to trust.
 * @param[in] ulServerCertificateLength Length in bytes of the encoded server
 * certificate. The length must include the null terminator.
 * @param[in] pxNetworkRecv Caller-defined network receive function pointer.
 * @param[in] pxNetworkSend Caller-defined network send function pointer.
 * @param[in] pvCallerContext Caller-defined context handle to be used with callback
 * functions.
 */
typedef struct xTLS_PARAMS
{
    uint32_t ulSize;
    const char * pcDestination;
    const char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    const char ** ppcAlpnProtocols;
    uint32_t ulAlpnProtocolsCount;

    NetworkRecv_t pxNetworkRecv;
    NetworkSend_t pxNetworkSend;
    void * pvCallerContext;
} TLSParams_t;

/**
 * @brief Initializes the TLS context.
 *
 * @param[out] ppvContext Opaque context handle returned by the TLS library.
 * @param[in] pxParams Connection parameters specified by caller.
 *
 * @return Zero on success. Error return codes have the high bit set.
 */
BaseType_t TLS_Init( void ** ppvContext,
                     TLSParams_t * pxParams );

/**
 * @brief Negotiates TLS and connects to the server.
 *
 * @param pvContext Opaque context handle for TLS library.
 *
 * @return Zero on success. Error return codes have the high bit set.
 */
BaseType_t TLS_Connect( void * pvContext );

/**
 * @brief Reads the requested number of bytes from the secure connection
 *
 * @param pvContext Opaque context handle for TLS library.
 * @param pucReadBuffer Byte array for storing (decrypted) data read from the
 * network.
 * @param xReadLength Length in bytes of read buffer.
 *
 * @return Number of bytes read. Error return codes have the high bit set.
 */
BaseType_t TLS_Recv( void * pvContext,
                     unsigned char * pucReadBuffer,
                     size_t xReadLength );

/**
 * @brief Writes the requested number of bytes to the secure connection.
 *
 * @param pvContext Opaque context handle for TLS library.
 * @param pucMsg Byte array of data to be encrypted and then sent to the network.
 * @param xMsgLength Length in bytes of write buffer.
 *
 * @return Number of bytes read. Error return codes have the high bit set.
 */
BaseType_t TLS_Send( void * pvContext,
                     const unsigned char * pucMsg,
                     size_t xMsgLength );

/**
 * @brief Frees resources consumed by the TLS context.
 *
 * @param pvContext Opaque context handle for TLS library.
 */
void TLS_Cleanup( void * pvContext );

#endif /* ifndef __AWS__TLS__H__ */
