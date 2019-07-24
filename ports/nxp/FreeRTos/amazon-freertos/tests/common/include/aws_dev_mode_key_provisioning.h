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

#ifndef _AWS_DEV_MODE_KEY_PROVISIONING_H_
#define _AWS_DEV_MODE_KEY_PROVISIONING_H_

#include "aws_pkcs11.h"

typedef struct ProvisioningParams_t
{
    uint32_t ulClientPrivateKeyType;
    uint8_t * pcClientPrivateKey;
    uint32_t ulClientPrivateKeyLength;
    uint8_t * pcClientCertificate;
    uint32_t ulClientCertificateLength;
} ProvisioningParams_t;

void vDevModeKeyProvisioning( void );

void vAlternateKeyProvisioning( ProvisioningParams_t * xParams );

CK_RV xInitializePkcsSession( CK_FUNCTION_LIST_PTR * ppxFunctionList,
                              CK_SLOT_ID * pxSlotId,
                              CK_SESSION_HANDLE * pxSession );

CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR xObjectHandle );


#endif /* _AWS_DEV_MODE_KEY_PROVISIONING_H_ */
