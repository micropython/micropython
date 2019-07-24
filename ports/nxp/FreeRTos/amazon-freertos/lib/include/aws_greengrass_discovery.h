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
 * @file aws_greengrass_discovery.h
 * @brief greengrass discovery API.
 */

#ifndef _AWS_GREENGRASS_DISCOVERY_H_
#define _AWS_GREENGRASS_DISCOVERY_H_
#include "FreeRTOS.h"
#include "aws_clientcredential.h"
#include "aws_secure_sockets.h"

/**
 * @brief Input from user to locate GGC inside JSON file.
 *
 * When using manual selection discovery, the user should fill the host
 * parameters fields. The discovery API will use those to
 * locate the GGC inside the JSON file and fill
 * GGD_HostAddressData_t fields.
 */
typedef struct
{
    const char * pcGroupName;   /**< Contains the Group name of the GGC device. */
    const char * pcCoreAddress; /**< Contains the ARN of the GGC device. */
    uint8_t ucInterface;  /**< Contains the interface # of the GGC device. */
} HostParameters_t;

/**
 * @brief Green Grass Core connection parameters.
 *
 * Green Grass Core connection parameters extracted from the JSON file
 * returned from the local discovery or from the Cloud.
 * This is the return parameter from the discovery.
 */
typedef struct
{
    const char * pcHostAddress; /**< Host address, could be IP or hostname. */
    char * pcCertificate;       /**< Certificate of GGC. */
    uint32_t ulCertificateSize; /**< Certificate size of GGC. */
    uint16_t usPort;            /**< Port to connect to the GGC. */
} GGD_HostAddressData_t;

/*
 * @brief Connect directly to the green grass core.
 *
 * @note: In most case only calling this function is needed!
 * This function will perform in series:
 * 1. GGD_JSONRequest.
 * 2. GGD_GetJSONFileSize.
 * 3. GGD_GetJSONFile.
 * 4. GGD_ConnectToHost with auto slection parameters set to true.
 * The buffer size of pcBuffer need to be big enough to hold the complete
 * JSON file.
 *
 * @param [in] pcBuffer: Memory buffer provided by the user.
 *
 * @param [in] ulBufferSize: Size of the memory buffer.
 *
 * @param [out] pxHostAddressData : host address data
 *
 * @return If connection was successful then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_GetGGCIPandCertificate( char * pcBuffer,
                                       const uint32_t ulBufferSize,
                                       GGD_HostAddressData_t * pxHostAddressData );

/*
 * @brief HTML request to get the JSON file from the could.
 *
 * @note: This call will open a socket. Socket will need to be closed
 * by either calling GGD_JSONRequestGetFile or GGD_JSONRequestAbort
 *
 * @param [out] pxSocket: Socket for the cloud connection.
 *
 * @return If the JSON request was performed successfully
 * then pdPASS is returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_JSONRequestStart( Socket_t * pxSocket );

/*
 * @brief Get the size of the requested JSON file.
 *
 * @note:  Parse the HTML header to get size of the requested
 * JSON file.
 * The JSON file request through "GGD_JSONRequestStart"
 * has to be done prior to call this function.
 *
 * @param [in] pxSocket: Socket for the cloud connection. 
 * Returns SOCKETS_INVALID_SOCKET if connection is closed
 *
 * @param [out] pulJSONFileSize: The size of the requested JSON file.
 *
 * @return If the JSON file size was received successfully
 * then pdPASS is returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_JSONRequestGetSize( Socket_t * pxSocket,
                                   uint32_t * pulJSONFileSize );

/*
 * @brief Get the GreenGrass core JSON file from the cloud.
 *
 * This call will close the socket in parameter. Need previous call from
 *  GGD_JSONRequestStart.
 *
 * @note 1. Parse the HTML response to extract the JSON file.
 * This function allows the JSON file retrieval in chunck.
 * The user can call the function several time with small buffer size.
 * The JSON file will be retrieved bit by bit until
 * xJSONFileRetrieveCompleted is set to true.
 * 2. The JSON file request through "GGD_JSONRequestStart"
 * has to be done prior to call this function.
 * 3. The JSON file size has to be provided so calling
 * GGD_JSONRequestGetSize prior is mandatory.
 *
 * @param [in] pxSocket: Socket for the cloud connection.
 * @warning The socket Will be closed.Set to SOCKETS_INVALID_SOCKET.
 *
 * @param [in] pcBuffer: Memory buffer provided by the user.
 *
 * @param [in] ulBufferSize: Size of the memory buffer provided.
 *
 * @param [in] pulJSONFileSize: Size of JSON file to be retrieved.
 *
 * @param [out] pulByteRead: Must be set to zero by the user
 * prior first calling of GGD_GetJSONFile. Then the number of
 * bytes read from the server will be update with each GGD_GetJSONFile
 * call.
 *
 * @param [out] pxJSONFileRetrieveCompleted: set to pdTRUE when
 * JSON file retrieve is finish.
 *
 * @return If the request check was sucessfully retrieved.
 *         Otherwise pdFAIL is returned.
 * @note The condition for complete JSON retrieval is
 * xJSONFileRetrieveCompleted == pdTRUE and
 * function return pdPASS for every call
 */
BaseType_t GGD_JSONRequestGetFile( Socket_t * pxSocket,
                                   char * pcBuffer,
                                   const uint32_t ulBufferSize,
                                   uint32_t * pulByteRead,
                                   BaseType_t * pxJSONFileRetrieveCompleted,
                                   const uint32_t pulJSONFileSize );

/*
 * @brief Need to be called if GGD_JSONRequestGetFile cannot be called.
 *
 * @param [in] pxSocket: Socket to close the cloud connection.Set to SOCKETS_INVALID_SOCKET.
 *  
 */
void GGD_JSONRequestAbort( Socket_t * pxSocket );

/*
 * @brief  Get host IP and certificate
 *
 * Get host IP and certificate with the JSON file given the greengrass group 
 * and cloud core address the user wants to connect.
 *
 * @note:  The JSON file contains the certificate that is going to be used to
 * connect with the core. For memory savings, the JSON file certificate is
 * going to be editing inline to connect with the host.
 * Thus, afterwards, the JSON will become unusable.
 * This function will also change the default certificate being used.
 * The default certificate is the one specified in client credential.
 * After calling this function, it will be one of the certificates provided in the
 * JSON file.
 * @note that if xAutoSelectFlag is set to false, the user will have to provide the
 * pxHostParameters. If xAutoSelectFlag is set to true, then pxHostParameters
 * are not used (can be set to NULL) and the IP and certificate returned will be of
 * the first established connection.
 * The user would also need to specify the ggdconfigCORE_NETWORK_INTERFACE (starting from 1)
 * in aws_ggd_config.h
 *
 * @param [out] pxHostAddressData : host address data
 *
 * @param [in] pucJSON_File: Pointer to the JSON file. WARNING, will be modified to
 *             format the certificate.
 *
 * @param [in] ulJSON_File_Size: Size in byte of the array.
 *
 * @param [in] pxHostParameters: Contains the group name, cloud address of the desired
 * core to connect to and interface to use (WIFI, ETH0 etc...). 
 * @warning: Cannot be NULL if xAutoSelectFlag is set to pdFALSE
 *
 * @param [in] xAutoSelectFlag: The user can opt for the auto select option.
 *             Then pxHostParameters are not used. Can be set to NULL.
 *
 * @return successfull pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 */
BaseType_t GGD_GetIPandCertificateFromJSON( char * pcJSONFile,
                                            const uint32_t ulJSONFileSize,
                                            const HostParameters_t * pxHostParameters,
                                            GGD_HostAddressData_t * pxHostAddressData,
                                            const BaseType_t xAutoSelectFlag );
#endif /* _AWS_GREENGRASS_DISCOVERY_H_ */
