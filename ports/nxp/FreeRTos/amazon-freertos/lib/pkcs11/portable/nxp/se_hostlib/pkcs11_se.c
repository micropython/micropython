/*
 * Amazon FreeRTOS PKCS#11 for A71CH V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2018 NXP
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
 * @file pkcs11.c
 * @brief Windows simulator PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

#ifdef TGT_A71CH
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "aws_clientcredential.h"

/*Other includes */
#include "HLSEAPI.h"
#include "ax_mbedtls.h"
/* Flash write */
#include "mflash_file.h"
#include "pkcs11_se.h"
/* C runtime includes. */
#include <stdio.h>
#include <string.h>



#define pkcs11SLOT_ID                         1
#define pkcs11OBJECT_HANDLE_PUBLIC_KEY        1
#define pkcs11OBJECT_HANDLE_PRIVATE_KEY       2
#define pkcs11OBJECT_HANDLE_CERTIFICATE       3




typedef int( *pfnMbedTlsSign )(
    void *ctx,
    mbedtls_md_type_t md_alg,
    const unsigned char *hash,
    size_t hash_len,
    unsigned char *sig,
    size_t *sig_len,
    int( *f_rng )( void *, unsigned char *, size_t ),
    void *p_rng );



/**
 * @brief Key structure.
 */
typedef struct P11Key
{
    mbedtls_pk_context xMbedPkCtx;
    mbedtls_x509_crt xMbedX509Cli;
    mbedtls_pk_info_t xMbedPkInfo;
    pfnMbedTlsSign pfnSavedMbedSign;
    void *pvSavedMbedPkCtx;
    uint16_t pem_len_client;
    uint16_t pem_len_client_ca;
    unsigned char * certificate_buf; //[2500];
} P11Key_t, * P11KeyPtr_t;

/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    P11KeyPtr_t pxCurrentKey;
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_MECHANISM_TYPE xOperationInProgress;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    CK_OBJECT_CLASS xFindObjectClass;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
    mbedtls_pk_context xPublicKey;
    mbedtls_sha256_context xSHA256Context;
} P11Session_t, * P11SessionPtr_t;



/*-----------------------------------------------------------*/
static void prvFreeKey( P11KeyPtr_t pxKey );
static int awsRetreieve_certificate(unsigned char *pbuf, uint8_t index, uint16_t *outbuflen);
static int convertdertopemformat(const unsigned char * int_ca_cert_der, int len_der, unsigned char * int_ca_cert_pem, int dst_buf_len, U16 *outlength);
/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
static P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}


/*-----------------------------------------------------------*/

/**
 * @brief Initializes a key structure.
 */
static CK_RV prvInitializeKey( P11SessionPtr_t pxSessionObj,
    const char * pcEncodedKey,
    const uint32_t ulEncodedKeyLength,
    const char * pcEncodedCertificate,
    const uint32_t ulEncodedCertificateLength )
{
    CK_RV xResult = CKR_OK;

    /*
     * Create the key structure, but allow an existing one to be used.
     */
    if( NULL == pxSessionObj->pxCurrentKey )
    {
        if( NULL == ( pxSessionObj->pxCurrentKey = ( P11KeyPtr_t )pvPortMalloc(
            sizeof( P11Key_t ) ) ) ) /*lint !e9087 Allow casting void* to other types. */
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( CKR_OK == xResult )
    {
        pxSessionObj->pxCurrentKey->pem_len_client = 0;
        pxSessionObj->pxCurrentKey->pem_len_client_ca = 0;
        pxSessionObj->pxCurrentKey->certificate_buf = NULL;

        xResult = (CK_RV)(ax_mbedtls_associate_keypair(
            0, &(pxSessionObj->pxCurrentKey->xMbedPkCtx)));

        if (xResult != CKR_OK)
        {
            xResult = CKR_FUNCTION_FAILED;
        }

        pxSessionObj->pxCurrentKey->pvSavedMbedPkCtx = pxSessionObj->pxCurrentKey->xMbedPkCtx.pk_ctx;
    }
    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Load the default key and certificate from storage.
 */
static CK_RV prvLoadAndInitializeDefaultCertificateAndKey( P11SessionPtr_t pxSession )
{
    CK_RV xResult = 0;
    uint8_t * pucCertificateData = NULL;
    uint32_t ulCertificateDataLength = 0;
    uint8_t * pucKeyData = NULL;
    uint32_t ulKeyDataLength = 0;
    int ret = 0;
    uint8_t  index;
    unsigned char * aclient_cer; //[500];
    unsigned char * aintca_cer; //[1500];
    uint16_t client_crt_len = 500; //sizeof(aclient_cer);
    uint16_t intca_crt_len = 1000; //sizeof(aintca_cer);

    if( NULL == (aclient_cer = (unsigned char *)pvPortMalloc(client_crt_len)))    {
        xResult = CKR_HOST_MEMORY;
    }
    if( NULL == (aintca_cer = (unsigned char *)pvPortMalloc(intca_crt_len)))    {
        xResult = CKR_HOST_MEMORY;
    }


    if (CKR_OK == xResult)
    {
    /* Attach the certificate and key to the session. */
        xResult = prvInitializeKey( pxSession,
                                ( const char * ) pucKeyData,
                                ulKeyDataLength,
                                ( const char * ) pucCertificateData,
                                ulCertificateDataLength );
    }

    if (CKR_OK == xResult)
    {
        mbedtls_x509_crt_init( &pxSession->pxCurrentKey->xMbedX509Cli );

        index = 0;
        ret = awsRetreieve_certificate(aclient_cer, index, &client_crt_len);
        if (ret != 0)    {
            xResult = CKR_FUNCTION_FAILED;
            /*failure */
        }

        index = 1;
        ret = awsRetreieve_certificate(aintca_cer, index, &intca_crt_len);
        if ( ret == 0 ){
            xResult = CKR_OK;
        }
        else {
            // greceful exit.
            xResult = CKR_OK;
            intca_crt_len = 0; // Not Using intca_crt_len
        }

    }
   if (CKR_OK == xResult)
   {
	   if( NULL == (pxSession->pxCurrentKey->certificate_buf = (unsigned char *)pvPortMalloc(2000)))    {
	          xResult = CKR_HOST_MEMORY;
	      }

        ret = convertdertopemformat(aclient_cer, client_crt_len, \
                pxSession->pxCurrentKey->certificate_buf, \
                sizeof(pxSession->pxCurrentKey->certificate_buf), \
                &(pxSession->pxCurrentKey->pem_len_client));

        if (ret == 0 && intca_crt_len != 0)
        {
            (pxSession->pxCurrentKey->pem_len_client) -= 1;
            ret = convertdertopemformat(aintca_cer, intca_crt_len, \
                    &(pxSession->pxCurrentKey->certificate_buf[pxSession->pxCurrentKey->pem_len_client]),\
                    sizeof(pxSession->pxCurrentKey->certificate_buf)-(pxSession->pxCurrentKey->pem_len_client), \
                    &(pxSession->pxCurrentKey->pem_len_client_ca));
        }

        if (ret != 0)        {
            xResult = CKR_FUNCTION_FAILED;
            /*failure */
        }


        /* Stir the random pot. */
        mbedtls_entropy_update_manual( &pxSession->xMbedEntropyContext,
                                       pucKeyData,
                                       ulKeyDataLength );
        mbedtls_entropy_update_manual( &pxSession->xMbedEntropyContext,
                                       pucCertificateData,
                                       ulCertificateDataLength );
   }

    if ( NULL != aclient_cer )    {
        vPortFree(aclient_cer);
    }

    if ( NULL != aintca_cer )    {
        vPortFree(aintca_cer);
    }
    /* certificate_buf Freed later */

    return xResult;
}

/*-----------------------------------------------------------*/


/*
 * PKCS#11 module implementation.
 */
/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList_sm =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize_sm,
	C_Finalize_sm,
    NULL,
    C_GetFunctionList_sm,
    C_GetSlotList,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_OpenSession_sm,
	C_CloseSession_sm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_CreateObject,
    NULL,
    C_DestroyObject,
    NULL,
    C_GetAttributeValue_sm,
    NULL,
    C_FindObjectsInit_sm,
    C_FindObjects_sm,
    C_FindObjectsFinal_sm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_SignInit,
    C_Sign,
    NULL,
    NULL,
    NULL,
    NULL,
    C_VerifyInit,
    C_Verify,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_GenerateRandom,
    NULL,
    NULL,
    NULL
};


 /**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList_sm )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   /*lint !e9072 It's OK to have different parameter name. */
    *ppxFunctionList = &prvP11FunctionList_sm;

    return CKR_OK;
}


/**
 * @brief Initialize the Cryptoki module for use.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Initialize_sm )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );
    uint32_t status = CKR_GENERAL_ERROR;

    status = CKR_OK;

    return status;
}



/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize_sm )( CK_VOID_PTR pvReserved )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvReserved );

    return CKR_OK;
}





/**
 * @brief Query the value of the specified cryptographic object attribute.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue_sm )( CK_SESSION_HANDLE xSession,
    CK_OBJECT_HANDLE xObject,
    CK_ATTRIBUTE_PTR pxTemplate,
    CK_ULONG ulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_VOID_PTR pvAttr = NULL;
    CK_ULONG ulAttrLength = 0;
    mbedtls_pk_type_t xMbedPkType;
    CK_ULONG xP11KeyType, iAttrib, xKeyBitLen;

    ( void )( xObject );

    /*
     * Enumerate the requested attributes.
     */

    for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
    {
        /*
         * Get the attribute data and size.
         */

        switch( pxTemplate[ iAttrib ].type )
        {
        case CKA_KEY_TYPE:

            /*
             * Map the private key type between APIs.
             */
            xMbedPkType = mbedtls_pk_get_type( &pxSession->pxCurrentKey->xMbedPkCtx );

            switch( xMbedPkType )
            {
            case MBEDTLS_PK_RSA:
            case MBEDTLS_PK_RSA_ALT:
            case MBEDTLS_PK_RSASSA_PSS:
                xP11KeyType = CKK_RSA;
                break;

            case MBEDTLS_PK_ECKEY:
            case MBEDTLS_PK_ECKEY_DH:
                xP11KeyType = CKK_EC;
                break;

            case MBEDTLS_PK_ECDSA:
                xP11KeyType = CKK_ECDSA;
                break;

            default:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
            }

            ulAttrLength = sizeof( xP11KeyType );
            pvAttr = &xP11KeyType;
            break;

        case CKA_VALUE:

            /*
             * Assume that the query is for the encoded client certificate.
             */


            ulAttrLength = pxSession->pxCurrentKey->pem_len_client + pxSession->pxCurrentKey->pem_len_client_ca;
            pvAttr = ( CK_VOID_PTR )(pxSession->pxCurrentKey->certificate_buf);

            break;

        case CKA_MODULUS_BITS:
        case CKA_PRIME_BITS:

            /*
             * Key strength size query, handled the same for RSA or ECDSA
             * in this port.
             */
            xKeyBitLen = mbedtls_pk_get_bitlen(
                &pxSession->pxCurrentKey->xMbedPkCtx );
            ulAttrLength = sizeof( xKeyBitLen );
            pvAttr = &xKeyBitLen;
            break;

        case CKA_VENDOR_DEFINED:

            /*
             * Return the key context for application-layer use.
             */
            ulAttrLength = sizeof( pxSession->pxCurrentKey->xMbedPkCtx );
            pvAttr = &pxSession->pxCurrentKey->xMbedPkCtx;
            break;

        default:
            xResult = CKR_ATTRIBUTE_TYPE_INVALID;
            break;
        }

        if( CKR_OK == xResult )
        {
            /*
             * Copy out the data and size.
             */

            if( NULL != pxTemplate[ iAttrib ].pValue )
            {
                if( pxTemplate[ iAttrib ].ulValueLen < ulAttrLength )
                {
                    xResult = CKR_BUFFER_TOO_SMALL;
                }
                else
                {
                    memcpy( pxTemplate[ iAttrib ].pValue, pvAttr, ulAttrLength );

                }
            }

            pxTemplate[ iAttrib ].ulValueLen = ulAttrLength;
        }
    }

    return xResult;
}



/**
 * @brief Begin an enumeration sequence for the objects of the specified type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit_sm )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    ( void ) ( ulCount );

    /*
     * Allow filtering on a single object class attribute.
     */

    pxSession->xFindObjectInit = CK_TRUE;
    pxSession->xFindObjectComplete = CK_FALSE;
    memcpy( &pxSession->xFindObjectClass,
            pxTemplate[ 0 ].pValue,
            sizeof( CK_OBJECT_CLASS ) );

    return CKR_OK;
}

/**
 * @brief Query the objects of the requested type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects_sm )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    BaseType_t xDone = pdFALSE;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */

    if( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) && ( 0u == ulMaxObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) && ( ( CK_BBOOL ) CK_TRUE == pxSession->xFindObjectComplete ) )
    {
        *pulObjectCount = 0;
        xResult = CKR_OK;
        xDone = pdTRUE;
    }

    /*
     * Load the default private key and certificate.
     */

    if( ( pdFALSE == xDone ) && ( NULL == pxSession->pxCurrentKey ) )
    {
        if( CKR_OK != ( xResult = prvLoadAndInitializeDefaultCertificateAndKey( pxSession ) ) )
        {
            xDone = pdTRUE;
        }
    }

    if( pdFALSE == xDone )
    {
        /*
         * Return object handles based on find type.
         */

        switch( pxSession->xFindObjectClass )
        {
            case CKO_PRIVATE_KEY:
                *pxObject = pkcs11OBJECT_HANDLE_PRIVATE_KEY;
                *pulObjectCount = 1;
                break;

            case CKO_PUBLIC_KEY:
                *pxObject = pkcs11OBJECT_HANDLE_PUBLIC_KEY;
                *pulObjectCount = 1;
                break;

            case CKO_CERTIFICATE:
                *pxObject = pkcs11OBJECT_HANDLE_CERTIFICATE;
                *pulObjectCount = 1;
                break;

            default:
                *pxObject = 0;
                *pulObjectCount = 0;
                break;
        }

        pxSession->xFindObjectComplete = CK_TRUE;
    }

    return xResult;
}

/**
 * @brief Terminate object enumeration.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal_sm )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */

    if( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }
    else
    {
        /*
         * Clean-up find objects state.
         */

        pxSession->xFindObjectInit = CK_FALSE;
        pxSession->xFindObjectComplete = CK_FALSE;
        pxSession->xFindObjectClass = 0;
    }

    return xResult;
}






/**
 * @brief Start a session for a cryptographic command sequence.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession_sm )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = NULL;

    ( void ) ( xSlotID );
    ( void ) ( pvApplication );
    ( void ) ( xNotify );

    /*
     * Make space for the context.
     */
    if( NULL == ( pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( P11Session_t ) ) ) ) /*lint !e9087 Allow casting void* to other types. */
    {
        xResult = CKR_HOST_MEMORY;
    }

    /*
     * Assume that there's no performance tradeoff in loading the default key
     * now, since that's the principal use case for opening a session in this
     * provider anyway. This way, the private key can be used for seeding the RNG,
     * especially if there's no hardware-based alternative.
     */

    if( CKR_OK == xResult )
    {
        memset( pxSessionObj, 0, sizeof( P11Session_t ) );
    }

    /*
     * Initialize RNG.
     */

    if( CKR_OK == xResult )
    {
        memset( &pxSessionObj->xMbedEntropyContext,
                0,
                sizeof( pxSessionObj->xMbedEntropyContext ) );

        mbedtls_entropy_init( &pxSessionObj->xMbedEntropyContext );
    }

    if( CKR_OK == xResult )
    {
        mbedtls_ctr_drbg_init( &pxSessionObj->xMbedDrbgCtx );

        if( 0 != mbedtls_ctr_drbg_seed( &pxSessionObj->xMbedDrbgCtx,
                                        mbedtls_entropy_func,
                                        &pxSessionObj->xMbedEntropyContext,
                                        NULL,
                                        0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( CKR_OK == xResult )
    {
        /*
         * Assign the session.
         */

        pxSessionObj->ulState =
            0u != ( xFlags & CKF_RW_SESSION ) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
        pxSessionObj->xOpened = CK_TRUE;

        /*
         * Return the session.
         */

        *pxSession = ( CK_SESSION_HANDLE ) pxSessionObj; /*lint !e923 Allow casting pointer to integer type for handle. */
    }

    return xResult;
}


/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession_sm )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL != pxSession )
    {
        /*
         * Tear down the session.
         */
        if ( NULL != pxSession->pxCurrentKey->certificate_buf )        {
           vPortFree(pxSession->pxCurrentKey->certificate_buf);
        }


        if( NULL != pxSession->pxCurrentKey )        {
            prvFreeKey( pxSession->pxCurrentKey );
        }

        mbedtls_ctr_drbg_free( &pxSession->xMbedDrbgCtx );
        vPortFree( pxSession );
    }
    return xResult;
}

/**
 * @brief Cleans up a key structure.
 */
static void prvFreeKey( P11KeyPtr_t pxKey )
{
    if( NULL != pxKey )
    {
        /* Restore the internal key context. */
        pxKey->xMbedPkCtx.pk_ctx = pxKey->pvSavedMbedPkCtx;

        /* Clean-up. */
        mbedtls_pk_free( &pxKey->xMbedPkCtx );
        mbedtls_x509_crt_free( &pxKey->xMbedX509Cli );
        vPortFree( pxKey );
    }
}




static int awsRetreieve_certificate(unsigned char *pbuf, uint8_t index, uint16_t *outbuflen)
{
    int ret = 1;
    HLSE_RET_CODE hlseRc;
    HLSE_OBJECT_HANDLE certHandles[5];
    HLSE_ATTRIBUTE attr;
    U16 certHandlesNum = sizeof(certHandles) / sizeof(HLSE_OBJECT_HANDLE);
    U16 certHandlesNum_copy;
    U8 i = 0;
    uint8_t certPattern[] = {0x30, 0x82};
    hlseRc = HLSE_EnumerateObjects(HLSE_CERTIFICATE, certHandles, &certHandlesNum);
    if ((hlseRc == HLSE_SW_OK))
    {
        certHandlesNum_copy = certHandlesNum;
        while(certHandlesNum_copy)
        {
            if (HLSE_GET_OBJECT_INDEX(certHandles[i]) == index)
            {
                break;
            }
            i++;
            certHandlesNum_copy--;
        }
        /*can not find the index specified if i >= certHandlesNum*/
        if (i < certHandlesNum)
        {
            attr.type = HLSE_ATTR_OBJECT_VALUE;
            attr.value = pbuf;
            attr.valueLen = *outbuflen;
            hlseRc = HLSE_GetObjectAttribute(certHandles[i], &attr);
            if (hlseRc == HLSE_SW_OK)
            {
                if ((pbuf[0] == certPattern[0]) && (pbuf[1] == certPattern[1]))
                {
                    *outbuflen = attr.valueLen;
                    ret = 0;
                }
            }

        }

    }

    return ret;
}
static int convertdertopemformat(const unsigned char * int_ca_cert_der, int len_der, unsigned char * int_ca_cert_pem, int dst_buf_len, U16 *outlength)
{
    int ret;
    size_t len_out;
    const char start[]= "-----BEGIN CERTIFICATE-----\n";
    const char end[]= "\n-----END CERTIFICATE-----\n";

    strcpy((char *)int_ca_cert_pem,start);
    ret = mbedtls_base64_encode(&int_ca_cert_pem[sizeof(start) - 1],dst_buf_len-sizeof(start)-sizeof(end)-1,&len_out,int_ca_cert_der,len_der);
    if (ret == 0)
    {
        strcpy((char *) &int_ca_cert_pem[sizeof(start) - 1 + len_out],end);
        *outlength = len_out + sizeof(start) + sizeof(end) - 1;
    }
    else
    {
        *outlength = 0;
        ret = 1;
    }
    return ret;
}

#ifndef IMX_RT
BaseType_t mflash_save_file(
    char * pcFileName,
    uint8_t * pucData,
    uint32_t ulDataSize )
{
    return pdFALSE;
}

BaseType_t mflash_read_file(
    char * pcFileName,
    uint8_t ** ppucData,
    uint32_t * pulDataSize )
{
    return pdFALSE;
}

bool mflash_is_initialized() {
    return true;
}

BaseType_t mflash_init(
    mflash_file_t *user_file_table,
    bool init_drv
) {
    return pdFALSE; /* must not be called */
}
#endif /*BOARD_NAME */
#endif /* TGT_A71CH */
