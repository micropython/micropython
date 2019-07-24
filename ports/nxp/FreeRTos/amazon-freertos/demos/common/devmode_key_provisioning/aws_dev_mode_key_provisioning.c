/*
 * Amazon FreeRTOS V1.4.6
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
 * @file aws_dev_mode_key_provisioning.c
 * @brief Simple key provisioning example using pkcs#11
 *
 * A simple example to demonstrate key and certificate provisioning in
 * flash using PKCS#11 interface. This should be replaced
 * by production ready key provisioning mechanism.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* PKCS#11 includes. */
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Client credential includes. */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* mbedTLS includes. */
#include "mbedtls/base64.h"
/*-----------------------------------------------------------*/

/* For convenience and to enable rapid evaluation the keys are stored in const
 * strings, see aws_clientcredential_keys.h.  THIS IS NOT GOOD PRACTICE FOR
 * PRODUCTION SYSTEMS WHICH MUST STORE KEYS SECURELY.  The variables declared
 * here are externed in aws_clientcredential_keys.h for access by other
 * modules. */
const char clientcredentialCLIENT_CERTIFICATE_PEM[] = keyCLIENT_CERTIFICATE_PEM;
const char * clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM = keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
const char clientcredentialCLIENT_PRIVATE_KEY_PEM[] = keyCLIENT_PRIVATE_KEY_PEM;

/*
 * Length of device certificate included from aws_clientcredential_keys.h .
 */
const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH = sizeof( clientcredentialCLIENT_CERTIFICATE_PEM );

/*
 * Length of device private key included from aws_clientcredential_keys.h .
 */
const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH = sizeof( clientcredentialCLIENT_PRIVATE_KEY_PEM );


/* Key provisioning helper defines. */
#define provisioningPRIVATE_KEY_TEMPLATE_COUNT         4
#define provisioningCERTIFICATE_TEMPLATE_COUNT         3
#define provisioningROOT_CERTIFICATE_TEMPLATE_COUNT    3


/*-----------------------------------------------------------*/

CK_RV xInitializePkcsSession( CK_FUNCTION_LIST_PTR * ppxFunctionList,
                              CK_SLOT_ID * pxSlotId,
                              CK_SESSION_HANDLE * pxSession )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList xCkGetFunctionList = NULL;
    CK_ULONG xCount = 1;

    ( void ) ( pxSession );

    /* Ensure that the PKCS#11 module is initialized. */
    if( 0 == xResult )
    {
        xCkGetFunctionList = C_GetFunctionList;
        xResult = xCkGetFunctionList( ppxFunctionList );
    }

    if( 0 == xResult )
    {
        xResult = ( *ppxFunctionList )->C_Initialize( NULL );
    }

    /* Get the default private key storage ID. */
    if( ( 0 == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = ( *ppxFunctionList )->C_GetSlotList( CK_TRUE,
                                                       pxSlotId,
                                                       &xCount );
    }

    if( xResult == CKR_OK )
    {
        xResult = ( *ppxFunctionList )->C_OpenSession( *pxSlotId,
                                                       CKF_SERIAL_SESSION,
                                                       NULL,
                                                       NULL,
                                                       pxSession );
    }

    return xResult;
}

/* @brief Converts PEM documents into DER formatted byte arrays.
 * This is a helper function from mbedTLS util pem2der.c
 * (https://github.com/ARMmbed/mbedtls/blob/development/programs/util/pem2der.c#L75)
 *
 * \param pucInput[in]       Pointer to PEM object
 * \param xLen[in]           Length of PEM object
 * \param pucOutput[out]     Pointer to buffer where DER oboject will be placed
 * \param pxOlen[in/out]     Pointer to length of DER buffer.  This value is updated
 *                          to contain the actual length of the converted DER object.
 *
 * \return 0 if successful.  Negative if conversion failed.  If buffer is not
 * large enough to hold converted object, pxOlen is still updated but -1 is returned.
 *
 */
int convert_pem_to_der( const unsigned char * pucInput,
                        size_t xLen,
                        unsigned char * pucOutput,
                        size_t * pxOlen )
{
    int lRet;
    const unsigned char * pucS1;
    const unsigned char * pucS2;
    const unsigned char * pucEnd = pucInput + xLen;
    size_t xOtherLen = 0;

    pucS1 = ( unsigned char * ) strstr( ( const char * ) pucInput, "-----BEGIN" );

    if( pucS1 == NULL )
    {
        return( -1 );
    }

    pucS2 = ( unsigned char * ) strstr( ( const char * ) pucInput, "-----END" );

    if( pucS2 == NULL )
    {
        return( -1 );
    }

    pucS1 += 10;

    while( pucS1 < pucEnd && *pucS1 != '-' )
    {
        pucS1++;
    }

    while( pucS1 < pucEnd && *pucS1 == '-' )
    {
        pucS1++;
    }

    if( *pucS1 == '\r' )
    {
        pucS1++;
    }

    if( *pucS1 == '\n' )
    {
        pucS1++;
    }

    if( ( pucS2 <= pucS1 ) || ( pucS2 > pucEnd ) )
    {
        return( -1 );
    }

    lRet = mbedtls_base64_decode( NULL, 0, &xOtherLen, ( const unsigned char * ) pucS1, pucS2 - pucS1 );

    if( lRet == MBEDTLS_ERR_BASE64_INVALID_CHARACTER )
    {
        return( lRet );
    }

    if( xOtherLen > *pxOlen )
    {
        return( -1 );
    }

    if( ( lRet = mbedtls_base64_decode( pucOutput, xOtherLen, &xOtherLen, ( const unsigned char * ) pucS1,
                                        pucS2 - pucS1 ) ) != 0 )
    {
        return( lRet );
    }

    *pxOlen = xOtherLen;

    return( 0 );
}

CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR pxObjectHandle )
{
    PKCS11_CertificateTemplate_t xCertificateTemplate;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_RV xResult;
    uint8_t * pucDerObject = NULL;
    int32_t lConversionReturn = 0;
    size_t xDerLen = 0;


    /* Initialize the client certificate template. */
    xCertificateTemplate.xObjectClass.type = CKA_CLASS;
    xCertificateTemplate.xObjectClass.pValue = &xCertificateClass;
    xCertificateTemplate.xObjectClass.ulValueLen = sizeof( xCertificateClass );
    xCertificateTemplate.xValue.type = CKA_VALUE;
    xCertificateTemplate.xValue.pValue = ( CK_VOID_PTR ) pucCertificate;
    xCertificateTemplate.xValue.ulValueLen = ( CK_ULONG ) xCertificateLength;
    xCertificateTemplate.xLabel.type = CKA_LABEL;
    xCertificateTemplate.xLabel.pValue = ( CK_VOID_PTR ) pucLabel;
    xCertificateTemplate.xLabel.ulValueLen = sizeof( pucLabel );

    xResult = C_GetFunctionList( &pxFunctionList );

    if( xResult == CKR_OK )
    {
        /* Convert the certificate to DER format if it was in PEM. */
        /* The DER key should be about 3/4 the size of the PEM key, so mallocing the PEM key size is sufficient. */
        pucDerObject = pvPortMalloc( xCertificateTemplate.xValue.ulValueLen );
        lConversionReturn = 0;
        xDerLen = xCertificateTemplate.xValue.ulValueLen;

        if( pucDerObject != NULL )
        {
            lConversionReturn = convert_pem_to_der( xCertificateTemplate.xValue.pValue,
                                                    xCertificateTemplate.xValue.ulValueLen,
                                                    pucDerObject,
                                                    &xDerLen );
        }
        else
        {
            xResult = CKR_DEVICE_MEMORY;
        }

        if( lConversionReturn == 0 )
        {
            /* Set the template pointers to refer to the DER converted objects. */
            xCertificateTemplate.xValue.pValue = pucDerObject;
            xCertificateTemplate.xValue.ulValueLen = xDerLen;
        }
        else
        {
            configPRINTF( ( "ERROR: Failed to provision certificate %d \r\n", lConversionReturn ) );
        }

        /* Create an object using the encoded client certificate. */
        if( CKR_OK == xResult )
        {
            configPRINTF( ( "Write certificate...\r\n" ) );

            xResult = pxFunctionList->C_CreateObject( xSession,
                                                      ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                                                      sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                      pxObjectHandle );
        }

        if( xResult != CKR_OK )
        {
            configPRINTF( ( "ERROR: Failed to provision certificate %d \r\n", xResult ) );
        }

        if( pucDerObject != NULL )
        {
            vPortFree( pucDerObject );
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV xProvisionDevice( CK_SESSION_HANDLE xSession,
                        ProvisioningParams_t * pxParams )
{
    CK_RV xResult;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_KEY_TYPE xPrivateKeyType = ( CK_KEY_TYPE ) pxParams->ulClientPrivateKeyType;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_OBJECT_HANDLE xObject = 0;
    uint8_t * pucDerObject = NULL;
    int32_t lConversionReturn = 0;
    size_t xDerLen = 0;

    PKCS11_KeyTemplate_t xPrivateKeyTemplate;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the device private key template. */
    xPrivateKeyTemplate.xObjectClass.type = CKA_CLASS;
    xPrivateKeyTemplate.xObjectClass.pValue = &xPrivateKeyClass;
    xPrivateKeyTemplate.xObjectClass.ulValueLen = sizeof( xPrivateKeyClass );
    xPrivateKeyTemplate.xKeyType.type = CKA_KEY_TYPE;
    xPrivateKeyTemplate.xKeyType.pValue = &xPrivateKeyType;
    xPrivateKeyTemplate.xKeyType.ulValueLen = sizeof( xPrivateKeyType );
    xPrivateKeyTemplate.xValue.type = CKA_VALUE;
    xPrivateKeyTemplate.xValue.pValue = ( CK_VOID_PTR ) pxParams->pcClientPrivateKey;
    xPrivateKeyTemplate.xValue.ulValueLen = ( CK_ULONG ) pxParams->ulClientPrivateKeyLength;
    xPrivateKeyTemplate.xLabel.type = CKA_LABEL;
    xPrivateKeyTemplate.xLabel.pValue = ( CK_VOID_PTR ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    xPrivateKeyTemplate.xLabel.ulValueLen = ( CK_ULONG ) sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );


    xResult = xProvisionCertificate( xSession,
                                     pxParams->pcClientCertificate,
                                     pxParams->ulClientCertificateLength,
                                     ( uint8_t * ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                     &xObject );

    /* Create an object using the encoded private key. */
    if( CKR_OK == xResult )
    {
        /* Convert the private to DER format if it was in PEM. */
        /* The DER key should be about 3/4 the size of the PEM key, so mallocing the PEM key size is sufficient. */
        pucDerObject = pvPortMalloc( xPrivateKeyTemplate.xValue.ulValueLen );
        lConversionReturn = 0;
        xDerLen = xPrivateKeyTemplate.xValue.ulValueLen;

        if( pucDerObject != NULL )
        {
            lConversionReturn = convert_pem_to_der( xPrivateKeyTemplate.xValue.pValue,
                                                    xPrivateKeyTemplate.xValue.ulValueLen,
                                                    pucDerObject,
                                                    &xDerLen );

            if( lConversionReturn != 0 )
            {
                configPRINTF( ( "ERROR: Failed to parse private key %d \r\n", lConversionReturn ) );
            }
        }
        else
        {
            xResult = CKR_DEVICE_MEMORY;
        }

        if( lConversionReturn == 0 )
        {
            /* Set the template pointers to refer to the DER converted objects. */
            xPrivateKeyTemplate.xValue.pValue = pucDerObject;
            xPrivateKeyTemplate.xValue.ulValueLen = xDerLen;
        }

        if( xResult == CKR_OK )
        {
            configPRINTF( ( "Write device private key...\r\n" ) );

            xResult = pxFunctionList->C_CreateObject( xSession,
                                                      ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                                                      sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                      &xObject );
        }

        if( pucDerObject != NULL )
        {
            vPortFree( pucDerObject );
        }
    }

    if( xResult != CKR_OK )
    {
        configPRINTF( ( "ERROR: Failed to provision private key %d \r\n", xResult ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void vAlternateKeyProvisioning( ProvisioningParams_t * xParams )
{
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_SESSION_HANDLE xSession = 0;

    /* Initialize the PKCS Module */
    xResult = xInitializePkcsSession( &pxFunctionList,
                                      &xSlotId,
                                      &xSession );

    if( xResult == CKR_OK )
    {
        xResult = xProvisionDevice( xSession, xParams );
    }

    pxFunctionList->C_CloseSession( xSession );
}
/*-----------------------------------------------------------*/

void vDevModeKeyProvisioning( void )
{
    ProvisioningParams_t xParams;

    xParams.ulClientPrivateKeyType = CKK_RSA;
    xParams.pcClientPrivateKey = ( uint8_t * ) clientcredentialCLIENT_PRIVATE_KEY_PEM;
    xParams.ulClientPrivateKeyLength = clientcredentialCLIENT_PRIVATE_KEY_LENGTH;
    xParams.pcClientCertificate = ( uint8_t * ) clientcredentialCLIENT_CERTIFICATE_PEM;
    xParams.ulClientCertificateLength = clientcredentialCLIENT_CERTIFICATE_LENGTH;

    vAlternateKeyProvisioning( &xParams );
}


/*-----------------------------------------------------------*/
