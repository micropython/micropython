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


#ifndef _AWS_PKCS11_H_
#define _AWS_PKCS11_H_


/**
 * @brief Amazon FreeRTOS PKCS#11 Interface.
 * The following definitions are required by the PKCS#11 standard public
 * headers.
 */

#define CK_PTR          *

#ifndef NULL_PTR
    #define NULL_PTR    0
#endif

#define CK_DEFINE_FUNCTION( returnType, name )             returnType name
#define CK_DECLARE_FUNCTION( returnType, name )            returnType name
#define CK_DECLARE_FUNCTION_POINTER( returnType, name )    returnType( CK_PTR name )
#define CK_CALLBACK_FUNCTION( returnType, name )           returnType( CK_PTR name )

/**
 *   @brief Length of a SHA256 digest, in bytes.
 */
#define pcks11SHA256_DIGEST_LENGTH       32

/**
 * @brief Certificate type definitions.
 */
#define pkcs11CERTIFICATE_TYPE_USER      1
#define pkcs11CERTIFICATE_TYPE_ROOT      2

/**
 * @brief Elliptic-curve object identifiers.
 * From https://tools.ietf.org/html/rfc6637#section-11.
 */
#define pkcs11ELLIPTIC_CURVE_NISTP256    "1.2.840.10045.3.1.7"

/* Bring in the public header. */

/* Undefine the macro for Keil Compiler to avoid conflict */
#if defined( __PASTE ) && defined( __CC_ARM )
/* ARM RCVT stdint.h has a duplicate definition with PKCS #11. */
#undef __PASTE
#endif

#include "pkcs11.h"

/* Key Template */
/* The object class must be the first attribute in the array. */
typedef struct PKCS11_KeyTemplate
{
    CK_ATTRIBUTE xObjectClass;
    CK_ATTRIBUTE xKeyType;
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xValue;
} PKCS11_KeyTemplate_t, * PKCS11_KeyTemplatePtr_t;

/* Certificate Template */
/* The object class must be the first attribute in the array. */
typedef struct PKCS11_CertificateTemplate
{
    CK_ATTRIBUTE xObjectClass;
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xValue;
} PKCS11_CertificateTemplate_t, * PKCS11_CertificateTemplatePtr_t;


typedef struct PKCS11_GenerateKeyPublicTemplate
{
    CK_ATTRIBUTE xKeyType;
    CK_ATTRIBUTE xEcParams;
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xValue;
} PKCS11_GenerateKeyPublicTemplate_t, * PKCS11_GenerateKeyPublicTemplatePtr_t;

typedef struct PKCS11_GenerateKeyPrivateTemplate
{
    CK_ATTRIBUTE xLabel;
} PKCS11_GenerateKeyPrivateTemplate_t, * PKCS11_GenerateKeyPrivateTemplatePtr_t;

#define pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS    "Device Priv TLS Key"
#define pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS     "Device Pub TLS Key"
#define pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS    "Device Cert"
#define pkcs11configLABEL_CODE_VERIFICATION_KEY         "Code Verify Key"
#define pkcs11configLABEL_ROOT_CERTIFICATE              "Root Cert"

#define pkcs11INVALID_OBJECT_HANDLE                     0

#endif /* ifndef _AWS_PKCS11_H_ */
