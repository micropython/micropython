/*
 * Amazon FreeRTOS mbedTLS-based PKCS#11 V1.0.6
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
 * @file pkcs11.c
 * @brief mbedTLS PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_pkcs11_config.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "threading_alt.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

typedef int ( * pfnMbedTlsSign )( void * ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char * hash,
                                  size_t hash_len,
                                  unsigned char * sig,
                                  size_t * sig_len,
                                  int ( *f_rng )( void *, unsigned char *, size_t ),
                                  void * p_rng );

#define PKCS11_PRINT( X )    vLoggingPrintf X

#define pkcs11NO_OPERATION            ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFF )

/* The size of the buffer malloc'ed for the exported public key in C_GenerateKeyPair */
#define pkcs11KEY_GEN_MAX_DER_SIZE    200

/* PKCS#11 Object */
typedef struct P11Struct_t
{
    CK_BBOOL xIsInitialized;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
} P11Struct_t, * P11Context_t;

static P11Struct_t xP11Context;


/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_MECHANISM_TYPE xOperationInProgress;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    uint8_t * xFindObjectLabel;
    uint8_t xFindObjectLabelLength;
    SemaphoreHandle_t xVerifyMutex; /* Protects the verification key from being modified while in use. */
    mbedtls_pk_context xVerifyKey;
    SemaphoreHandle_t xSignMutex;   /* Protects the signing key from being modified while in use. */
    mbedtls_pk_context xSignKey;
    mbedtls_sha256_context xSHA256Context;
} P11Session_t, * P11SessionPtr_t;

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID               1

#define pkcs11SUPPORTED_KEY_BITS    2048


/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT             3

#define pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX     0
#define pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX    1


/*-----------------------------------------------------------*/
/*------------ Port Specific File Access API ----------------*/
/*--------- See aws_pkcs11_pal.c for definitions ------------*/
/*-----------------------------------------------------------*/

/**
 *  @brief Save an object to storage.
 */
extern CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                               uint8_t * pucData,
                                               uint32_t ulDataSize );

/**
 *   @brief Look up an object handle using it's label.
 */
extern CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                               uint8_t usLength );

/**
 *   @brief Get the value of an object.
 *   @note  Buffers may be allocated by this call, and should be
 *          freed up by calling PKCS11_PAL_GetObjectValueCleanup().
 */
extern BaseType_t PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                             uint8_t ** ppucData,
                                             uint32_t * pulDataSize,
                                             CK_BBOOL * xIsPrivate );

/**
 *  @brief Free the buffer allocated in PKCS11_PAL_GetObjectValue() (see PAL).
 */
extern void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucBuffer,
                                              uint32_t ulBufferSize );

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/
/*--------- mbedTLS threading functions for FreeRTOS --------*/
/*--------------- See MBEDTLS_THREADING_ALT -----------------*/
/*-----------------------------------------------------------*/

/**
 * @brief Implementation of mbedtls_mutex_init for thread-safety.
 *
 */
void aws_mbedtls_mutex_init( mbedtls_threading_mutex_t * mutex )
{
    if( mutex->is_valid == 0 )
    {
        mutex->mutex = xSemaphoreCreateMutex();

        if( mutex->mutex != NULL )
        {
            mutex->is_valid = 1;
        }
        else
        {
            PKCS11_PRINT( ( "Failed to initialize mbedTLS mutex.\r\n" ) );
        }
    }
}

/**
 * @brief Implementation of mbedtls_mutex_free for thread-safety.
 *
 */
void aws_mbedtls_mutex_free( mbedtls_threading_mutex_t * mutex )
{
    if( mutex->is_valid == 1 )
    {
        vSemaphoreDelete( mutex->mutex );
        mutex->is_valid = 0;
    }
}

/**
 * @brief Implementation of mbedtls_mutex_lock for thread-safety.
 *
 * @return 0 if successful, MBEDTLS_ERR_THREADING_MUTEX_ERROR if timeout,
 * MBEDTLS_ERR_THREADING_BAD_INPUT_DATA if the mutex is not valid.
 */
int aws_mbedtls_mutex_lock( mbedtls_threading_mutex_t * mutex )
{
    int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

    if( mutex->is_valid == 1 )
    {
        if( xSemaphoreTake( mutex->mutex, portMAX_DELAY ) )
        {
            ret = 0;
        }
        else
        {
            ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
            PKCS11_PRINT( ( "Failed to obtain mbedTLS mutex.\r\n" ) );
        }
    }

    return ret;
}

/**
 * @brief Implementation of mbedtls_mutex_unlock for thread-safety.
 *
 * @return 0 if successful, MBEDTLS_ERR_THREADING_MUTEX_ERROR if timeout,
 * MBEDTLS_ERR_THREADING_BAD_INPUT_DATA if the mutex is not valid.
 */
int aws_mbedtls_mutex_unlock( mbedtls_threading_mutex_t * mutex )
{
    int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

    if( mutex->is_valid == 1 )
    {
        if( xSemaphoreGive( mutex->mutex ) )
        {
            ret = 0;
        }
        else
        {
            ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
            PKCS11_PRINT( ( "Failed to unlock mbedTLS mutex.\r\n" ) );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}


/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    NULL, /*C_GetSlotInfo*/
    NULL, /*C_GetTokenInfo*/
    NULL, /*C_GetMechanismList*/
    NULL, /*C_GetMechansimInfo */
    NULL, /*C_InitToken*/
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL, /*C_CloseAllSessions*/
    NULL, /*C_GetSessionInfo*/
    NULL, /*C_GetOperationState*/
    NULL, /*C_SetOperationState*/
    NULL, /*C_Login*/
    NULL, /*C_Logout*/
    C_CreateObject,
    NULL, /*C_CopyObject*/
    C_DestroyObject,
    NULL, /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL, /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL, /*C_EncryptInit*/
    NULL, /*C_Encrypt*/
    NULL, /*C_EncryptUpdate*/
    NULL, /*C_EncryptFinal*/
    NULL, /*C_DecryptInit*/
    NULL, /*C_Decrypt*/
    NULL, /*C_DecryptUpdate*/
    NULL, /*C_DecryptFinal*/
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    NULL, /*C_SignUpdate*/
    NULL, /*C_SignFinal*/
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    NULL, /*C_VerifyUpdate*/
    NULL, /*C_VerifyFinal*/
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

/*-----------------------------------------------------------*/

CK_RV prvMbedTLS_Initialize( void )
{
    CK_RV xResult = CKR_OK;

    if( xP11Context.xIsInitialized == CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        /* Ensure that the FreeRTOS heap is used. */
        CRYPTO_ConfigureHeap();

        /* Configure mbedtls to use FreeRTOS mutexes. */
        mbedtls_threading_set_alt( aws_mbedtls_mutex_init,
                                   aws_mbedtls_mutex_free,
                                   aws_mbedtls_mutex_lock,
                                   aws_mbedtls_mutex_unlock );

        /* Initialze the entropy source and DRBG for the PKCS#11 module */
        mbedtls_entropy_init( &xP11Context.xMbedEntropyContext );
        mbedtls_ctr_drbg_init( &xP11Context.xMbedDrbgCtx );

        if( 0 != mbedtls_ctr_drbg_seed( &xP11Context.xMbedDrbgCtx,
                                        mbedtls_entropy_func,
                                        &xP11Context.xMbedEntropyContext,
                                        NULL,
                                        0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            xP11Context.xIsInitialized = CK_TRUE;
        }
    }

    return xResult;
}

/**
 * @brief Initialize the Cryptoki module for use.
 */
#if !defined( pkcs11configC_INITIALIZE_ALT )
    CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
    { /*lint !e9072 It's OK to have different parameter name. */
        ( void ) ( pvInitArgs );

        return prvMbedTLS_Initialize();
    }
#endif /* if !defined( pkcs11configC_INITIALIZE_ALT ) */

/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{
    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL != pvReserved )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xP11Context.xIsInitialized == CK_FALSE )
    {
        xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        if( NULL != &xP11Context.xMbedEntropyContext )
        {
            mbedtls_entropy_free( &xP11Context.xMbedEntropyContext );
        }

        if( NULL != &xP11Context.xMbedDrbgCtx )
        {
            mbedtls_ctr_drbg_free( &xP11Context.xMbedDrbgCtx );
        }

        xP11Context.xIsInitialized = CK_FALSE;
    }

    return xResult;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL == ppxFunctionList )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        *ppxFunctionList = &prvP11FunctionList;
    }

    return xResult;
}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    /* Since the mbedTLS implementation of PKCS#11 does not depend
     * on a physical token, this parameter is ignored. */
    ( void ) ( xTokenPresent );

    if( NULL == pulCount )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( NULL == pxSlotList )
    {
        *pulCount = 1;
    }
    else
    {
        if( 0u == *pulCount )
        {
            xResult = CKR_BUFFER_TOO_SMALL;
        }
        else
        {
            pxSlotList[ 0 ] = pkcs11SLOT_ID;
            *pulCount = 1;
        }
    }

    return xResult;
}

/**
 * @brief Start a session for a cryptographic command sequence.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
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

    /* Check arguments. */
    if( NULL == pxSession )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* For legacy reasons, the CKF_SERIAL_SESSION bit MUST always be set. */
    if( 0 == ( CKF_SERIAL_SESSION & xFlags ) )
    {
        xResult = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }

    /*
     * Make space for the context.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( P11Session_t ) ); /*lint !e9087 Allow casting void* to other types. */

        if( NULL == pxSessionObj )
        {
            xResult = CKR_HOST_MEMORY;
        }

        /*
         * Zero out the session structure.
         */
        if( CKR_OK == xResult )
        {
            memset( pxSessionObj, 0, sizeof( P11Session_t ) );
        }

        pxSessionObj->xSignMutex = xSemaphoreCreateMutex();

        if( NULL == pxSessionObj->xSignMutex )
        {
            xResult = CKR_HOST_MEMORY;
        }

        pxSessionObj->xVerifyMutex = xSemaphoreCreateMutex();

        if( NULL == pxSessionObj->xVerifyMutex )
        {
            xResult = CKR_HOST_MEMORY;
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

    /*
     *   Initialize the operation in progress.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj->xOperationInProgress = pkcs11NO_OPERATION;
    }

    if( ( NULL != pxSessionObj ) && ( CKR_OK != xResult ) )
    {
        vPortFree( pxSessionObj );
    }

    return xResult;
}

/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL != pxSession )
    {
        /*
         * Tear down the session.
         */

        if( NULL != pxSession->xSignKey.pk_ctx )
        {
            mbedtls_pk_free( &pxSession->xSignKey );
        }

        if( NULL != pxSession->xSignMutex )
        {
            vSemaphoreDelete( pxSession->xSignMutex );
        }

        /* Free the public key context if it exists. */
        if( NULL != pxSession->xVerifyKey.pk_ctx )
        {
            mbedtls_pk_free( &pxSession->xVerifyKey );
        }

        if( NULL != pxSession->xVerifyMutex )
        {
            vSemaphoreDelete( pxSession->xVerifyMutex );
        }

        if( NULL != &pxSession->xSHA256Context )
        {
            mbedtls_sha256_free( &pxSession->xSHA256Context );
        }

        vPortFree( pxSession );
    }
    else
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    return xResult;
}

/**
 * @brief Provides import and storage of a single client certificate and
 * associated private key.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    void * pvContext = NULL;
    int32_t lMbedTLSParseResult = ~0;
    PKCS11_KeyTemplatePtr_t pxKeyTemplate = NULL;
    PKCS11_CertificateTemplatePtr_t pxCertificateTemplate = NULL;
    CK_ATTRIBUTE_PTR pxObjectClassAttribute = pxTemplate;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    /*
     * Check parameters.
     */
    if( ( pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT > ulCount ) ||
        ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* The PKCS#11 spec allows attributes to be in any order, but to simplify
     *  code, CKA_CLASS is required to be the first attribute in the template. */
    if( CKR_OK == xResult )
    {
        if( ( pxObjectClassAttribute->type != CKA_CLASS ) ||
            ( pxObjectClassAttribute->ulValueLen != sizeof( CK_OBJECT_CLASS ) ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /*
     * Handle the object by class.
     */

    if( CKR_OK == xResult )
    {
        switch( *( ( uint32_t * ) pxObjectClassAttribute->pValue ) )
        {
            case CKO_CERTIFICATE:

                pxCertificateTemplate = ( PKCS11_CertificateTemplatePtr_t ) pxTemplate;

                /* Validate the attribute count for this object class. */
                if( sizeof( PKCS11_CertificateTemplate_t ) / sizeof( CK_ATTRIBUTE ) != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the attribute template. */
                if( ( CKA_VALUE != pxCertificateTemplate->xValue.type ) ||
                    ( CKA_LABEL != pxCertificateTemplate->xLabel.type ) )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Verify that the given certificate can be parsed. */
                pvContext = pvPortMalloc( sizeof( mbedtls_x509_crt ) );

                if( NULL != pvContext )
                {
                    mbedtls_x509_crt_init( ( mbedtls_x509_crt * ) pvContext );
                    lMbedTLSParseResult = mbedtls_x509_crt_parse( ( mbedtls_x509_crt * ) pvContext,
                                                                  pxCertificateTemplate->xValue.pValue,
                                                                  pxCertificateTemplate->xValue.ulValueLen );
                    mbedtls_x509_crt_free( ( mbedtls_x509_crt * ) pvContext );
                    vPortFree( pvContext );
                }
                else
                {
                    xResult = CKR_HOST_MEMORY;
                    break;
                }

                if( 0 != lMbedTLSParseResult )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Write the certificate to NVM. */
                if( 0 == ( *pxObject = PKCS11_PAL_SaveObject( &pxCertificateTemplate->xLabel,
                                                              pxCertificateTemplate->xValue.pValue,
                                                              pxCertificateTemplate->xValue.ulValueLen ) ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }

                break;

            case CKO_PRIVATE_KEY:
            case CKO_PUBLIC_KEY:

                /* Cast the template for easy field access. */
                pxKeyTemplate = ( PKCS11_KeyTemplatePtr_t ) pxTemplate;

                /* Validate the attribute count for this object class. */
                if( ( sizeof( PKCS11_KeyTemplate_t ) / sizeof( CK_ATTRIBUTE ) ) != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Confirm that the template is formatted as expected for a private key. */
                if( ( CKA_VALUE != pxKeyTemplate->xValue.type ) ||
                    ( CKA_KEY_TYPE != pxKeyTemplate->xKeyType.type ) ||
                    ( CKA_LABEL != pxKeyTemplate->xLabel.type ) )
                {
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
                    break;
                }

                /* Verify that the given key can be parsed. */
                pvContext = pvPortMalloc( sizeof( mbedtls_pk_context ) );

                if( NULL != pvContext )
                {
                    mbedtls_pk_init( ( mbedtls_pk_context * ) pvContext );

                    if( *( uint32_t * ) pxKeyTemplate->xObjectClass.pValue == CKO_PRIVATE_KEY )
                    {
                        lMbedTLSParseResult = mbedtls_pk_parse_key( ( mbedtls_pk_context * ) pvContext,
                                                                    pxKeyTemplate->xValue.pValue,
                                                                    pxKeyTemplate->xValue.ulValueLen,
                                                                    NULL,
                                                                    0 );
                    }
                    else
                    {
                        lMbedTLSParseResult = mbedtls_pk_parse_public_key( ( mbedtls_pk_context * ) pvContext,
                                                                           pxKeyTemplate->xValue.pValue,
                                                                           pxKeyTemplate->xValue.ulValueLen );
                    }

                    mbedtls_pk_free( ( mbedtls_pk_context * ) pvContext );
                    vPortFree( pvContext );
                }
                else
                {
                    xResult = CKR_HOST_MEMORY;
                    break;
                }

                if( 0 != lMbedTLSParseResult )
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                    break;
                }

                /* Save the key to NVM. */
                if( 0 == ( *pxObject = PKCS11_PAL_SaveObject( &pxKeyTemplate->xLabel,
                                                              pxKeyTemplate->xValue.pValue,
                                                              pxKeyTemplate->xValue.ulValueLen ) ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }

                break;

            default:
                xResult = CKR_ARGUMENTS_BAD;
        }
    }

    return xResult;
}

/**
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    /* TODO: Delete objects from NVM. */
    ( void ) xSession;
    ( void ) xObject;
    return CKR_OK;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{
    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_ULONG iAttrib;
    mbedtls_pk_context privateKeyContext;
    mbedtls_pk_type_t xKeyType;
    CK_KEY_TYPE xPkcsKeyType = ( CK_KEY_TYPE ) ~0;

    uint8_t * pxObjectValue = NULL;
    uint32_t ulLength = 0;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        /*
         * Copy the object into a buffer.
         */
        xResult = PKCS11_PAL_GetObjectValue( xObject, &pxObjectValue, &ulLength, &xIsPrivate );
    }

    if( xResult == CKR_OK )
    {
        for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
        {
            switch( pxTemplate[ iAttrib ].type )
            {
                case CKA_VALUE:

                    if( xIsPrivate == CK_TRUE )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = CK_UNAVAILABLE_INFORMATION;
                        xResult = CKR_ATTRIBUTE_SENSITIVE;
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].pValue == NULL )
                        {
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
                        }
                        else if( pxTemplate[ iAttrib ].ulValueLen < ulLength )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, pxObjectValue, ulLength );
                        }
                    }

                    break;

                case CKA_KEY_TYPE:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( mbedtls_pk_type_t );
                    }
                    else if( pxTemplate[ iAttrib ].ulValueLen < sizeof( mbedtls_pk_type_t ) )
                    {
                        xResult = CKR_BUFFER_TOO_SMALL;
                    }
                    else
                    {
                        mbedtls_pk_init( &privateKeyContext );
                        xResult = mbedtls_pk_parse_key( &privateKeyContext, pxObjectValue, ulLength, NULL, 0 );

                        if( 0 != xResult )
                        {
                            xResult = CKR_FUNCTION_FAILED;
                        }
                        else
                        {
                            xKeyType = mbedtls_pk_get_type( &privateKeyContext );

                            switch( xKeyType )
                            {
                                case MBEDTLS_PK_RSA:
                                case MBEDTLS_PK_RSA_ALT:
                                case MBEDTLS_PK_RSASSA_PSS:
                                    xPkcsKeyType = CKK_RSA;
                                    break;

                                case MBEDTLS_PK_ECKEY:
                                case MBEDTLS_PK_ECKEY_DH:
                                    xPkcsKeyType = CKK_EC;
                                    break;

                                case MBEDTLS_PK_ECDSA:
                                    xPkcsKeyType = CKK_ECDSA;
                                    break;

                                default:
                                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                                    break;
                            }

                            memcpy( pxTemplate[ iAttrib ].pValue, &xPkcsKeyType, sizeof( CK_KEY_TYPE ) );
                        }

                        /* Free the mbedTLS structure used to parse the key. */
                        mbedtls_pk_free( &privateKeyContext );
                    }

                    break;

                default:
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
            }
        }

        /* Free the buffer where object was stored. */
        PKCS11_PAL_GetObjectValueCleanup( pxObjectValue, ulLength );
    }

    return xResult;
}

/**
 * @brief Begin an enumeration sequence for the objects of the specified type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_RV xResult = CKR_OK;


    /*
     * Check parameters.
     */
    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( ulCount != 1 )
    {
        xResult = CKR_ARGUMENTS_BAD;
        configPRINTF( ( "Find objects can only filter by one attribute. " ) );
    }
    else
    {
        /* Copy the label to be looked up into the PKCS#11 session context. */
        if( pxTemplate->type == CKA_LABEL )
        {
            pxSession->xFindObjectInit = CK_TRUE;
            pxSession->xFindObjectComplete = CK_FALSE;

            /* Make sure the reported buffer length is not super huge. */
            if( pxTemplate->ulValueLen < UCHAR_MAX )
            {
                pxSession->xFindObjectLabel = pvPortMalloc( pxTemplate->ulValueLen );

                if( pxSession->xFindObjectLabel != NULL )
                {
                    memcpy( pxSession->xFindObjectLabel,
                            pxTemplate->pValue,
                            pxTemplate->ulValueLen );
                    pxSession->xFindObjectLabelLength = ( uint8_t ) pxTemplate->ulValueLen;
                }
                else
                {
                    xResult = CKR_HOST_MEMORY;
                }
            }
            else
            {
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }
        }
        else
        {
            PKCS11_PRINT( ( "Finding objects is only supported by LABEL.\r\n" ) );
            xResult = CKR_ATTRIBUTE_TYPE_INVALID;
        }
    }

    return xResult;
}

/**
 * @brief Query the objects of the requested type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
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
    if( ( NULL == pxObject ) ||
        ( NULL == pulObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) &&
        ( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit ) )
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

    if( ( pdFALSE == xDone ) )
    {
        *pxObject = PKCS11_PAL_FindObject( pxSession->xFindObjectLabel, pxSession->xFindObjectLabelLength );

        if( *pxObject != 0 ) /* 0 is always an invalid handle. */
        {
            *pulObjectCount = 1;
            xResult = CKR_OK;
        }
        else
        {
            PKCS11_PRINT( ( "ERROR: Object with label %s not found. \r\n", ( char * ) pxSession->xFindObjectLabel ) );
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}

/**
 * @brief Terminate object enumeration.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
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
        vPortFree( pxSession->xFindObjectLabel );
        pxSession->xFindObjectLabelLength = 0;
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pMechanism )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( pMechanism->mechanism != CKM_SHA256 )
    {
        xResult = CKR_MECHANISM_INVALID;
    }

    /*
     * Initialize the requested hash type
     */
    if( xResult == CKR_OK )
    {
        mbedtls_sha256_init( &pxSession->xSHA256Context );

        if( 0 != mbedtls_sha256_starts_ret( &pxSession->xSHA256Context, 0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            pxSession->xOperationInProgress = pMechanism->mechanism;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestUpdate )( CK_SESSION_HANDLE xSession,
                                             CK_BYTE_PTR pPart,
                                             CK_ULONG ulPartLen )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }
    else if( pxSession->xOperationInProgress != CKM_SHA256 )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        if( 0 != mbedtls_sha256_update_ret( &pxSession->xSHA256Context, pPart, ulPartLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
            pxSession->xOperationInProgress = pkcs11NO_OPERATION;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestFinal )( CK_SESSION_HANDLE xSession,
                                            CK_BYTE_PTR pDigest,
                                            CK_ULONG_PTR pulDigestLen )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }
    else if( pxSession->xOperationInProgress != CKM_SHA256 )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        pxSession->xOperationInProgress = pkcs11NO_OPERATION;
    }

    if( xResult == CKR_OK )
    {
        if( pDigest == NULL )
        {
            /* Supply the required buffer size. */
            *pulDigestLen = pcks11SHA256_DIGEST_LENGTH;
        }
        else
        {
            if( *pulDigestLen < pcks11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_BUFFER_TOO_SMALL;
            }
            else
            {
                if( 0 != mbedtls_sha256_finish_ret( &pxSession->xSHA256Context, pDigest ) )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }

                pxSession->xOperationInProgress = pkcs11NO_OPERATION;
            }
        }
    }

    return xResult;
}

/**
 * @brief Begin a digital signature generation session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = CKR_OK;
    CK_BBOOL xIsPrivate;

    /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    uint8_t * keyData = NULL;
    uint32_t ulKeyDataLength = 0;

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        xResult = PKCS11_PAL_GetObjectValue( xKey, &keyData, &ulKeyDataLength, &xIsPrivate );

        if( xIsPrivate != CK_TRUE )
        {
            xResult = CKR_KEY_TYPE_INCONSISTENT;
        }

        if( xResult == CKR_OK )
        {
            if( pdTRUE == xSemaphoreTake( pxSession->xSignMutex, portMAX_DELAY ) )
            {
                /* Free the private key context if it exists.
                * TODO: Check if the key is the same as was used previously. */
                if ( NULL != pxSession->xSignKey.pk_ctx )
                {
                    mbedtls_pk_free( &pxSession->xSignKey );
                }

                mbedtls_pk_init( &pxSession->xSignKey );

                if( 0 == mbedtls_pk_parse_key( &pxSession->xSignKey, keyData, ulKeyDataLength, NULL, 0 ) )
                {
                    /* TODO: Check the mechanism.  Note: Currently, mechanism is being set to CKM_SHA256, rather than
                     * CKM_RSA_PKCS
                     * CKM_SHA256_RSA_PKCS
                     * CKM_ECDSA
                     * Calling function does not know whether key is RSA or ECDSA.
                     * xKeyType = mbedtls_pk_get_type( &pxSession->xSignKey );
                     */
                }
                else
                {
                    xResult = CKR_KEY_HANDLE_INVALID;
                }

                xSemaphoreGive( pxSession->xSignMutex );
            }
            else
            {
                xResult = CKR_CANT_LOCK;
            }
        }

        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
    }

    return xResult;
}

/**
 * @brief Digitally sign the indicated cryptographic hash bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );

    if( NULL == pulSignatureLen )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( CKR_OK == xResult )
    {
        if( NULL == pucSignature )
        {
            *pulSignatureLen = pkcs11SUPPORTED_KEY_BITS / 8;
        }
        else
        {
            /*
             * Check algorithm support.
             */
            if( CKR_OK == xResult )
            {
                if( ( CK_ULONG ) cryptoSHA256_DIGEST_BYTES != ulDataLen )
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }
            }

            /*
             * Sign the data.
             */

            if( CKR_OK == xResult )
            {
                if( pdTRUE == xSemaphoreTake( pxSessionObj->xSignMutex, portMAX_DELAY ) )
                {
                    BaseType_t x = mbedtls_pk_sign( &pxSessionObj->xSignKey,
                                                    MBEDTLS_MD_SHA256,
                                                    pucData,
                                                    ulDataLen,
                                                    pucSignature,
                                                    ( size_t * ) pulSignatureLen,
                                                    mbedtls_ctr_drbg_random,
                                                    &xP11Context.xMbedDrbgCtx );

                    if( x != CKR_OK )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }

                    xSemaphoreGive( pxSessionObj->xSignMutex );
                }
                else
                {
                    xResult = CKR_CANT_LOCK;
                }
            }
        }
    }

    return xResult;
}

/**
 * @brief Begin a digital signature verification session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = CKR_OK;
    CK_BBOOL xIsPrivate = CK_TRUE;
    P11SessionPtr_t pxSession;
    uint8_t * keyData = NULL;
    uint32_t ulKeyDataLength = 0;

    /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );

    pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
        xResult = PKCS11_PAL_GetObjectValue( xKey, &keyData, &ulKeyDataLength, &xIsPrivate );
    }

    if( ( xResult == CKR_OK ) && ( xIsPrivate != CK_FALSE ) )
    {
        xResult = CKR_KEY_TYPE_INCONSISTENT;
        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
    }

    if( xResult == CKR_OK )
    {
        if( pdTRUE == xSemaphoreTake( pxSession->xVerifyMutex, portMAX_DELAY ) )
        {
            /* Free the public key context if it exists.
            * TODO: Check if the key is the same as used by last verify operation. */
            if ( NULL != pxSession->xVerifyKey.pk_ctx )
            {
                mbedtls_pk_free( &pxSession->xVerifyKey );
            }

            mbedtls_pk_init( &pxSession->xVerifyKey );

            if( 0 != mbedtls_pk_parse_public_key( &pxSession->xVerifyKey, keyData, ulKeyDataLength ) )
            {
                if( 0 != mbedtls_pk_parse_key( &pxSession->xVerifyKey, keyData, ulKeyDataLength, NULL, 0 ) )
                {
                    xResult = CKR_KEY_HANDLE_INVALID;
                }
            }

            xSemaphoreGive( pxSession->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }

        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
    }

    return xResult;
}

/**
 * @brief Verify the digital signature of the specified data using the public
 * key attached to this session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj;

    /*
     * Check parameters.
     */
    if( ( NULL == pucData ) ||
        ( NULL == pucSignature ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        pxSessionObj = prvSessionPointerFromHandle( xSession ); /*lint !e9072 It's OK to have different parameter name. */

        if( pdTRUE == xSemaphoreTake( pxSessionObj->xVerifyMutex, portMAX_DELAY ) )
        {
            /* Verify the signature. If a public key is present, use it. */
            if( NULL != pxSessionObj->xVerifyKey.pk_ctx )
            {
                if( 0 != mbedtls_pk_verify( &pxSessionObj->xVerifyKey,
                                            MBEDTLS_MD_SHA256,
                                            pucData,
                                            ulDataLen,
                                            pucSignature,
                                            ulSignatureLen ) )
                {
                    xResult = CKR_SIGNATURE_INVALID;
                }
            }

            xSemaphoreGive( pxSessionObj->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }

        /* TODO: Deleted else. */
    }

    /* Return the signature verification result. */
    return xResult;
}

/**
 * @brief Generate a new assymetric keyset.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateKeyPair )( CK_SESSION_HANDLE xSession,
                                                CK_MECHANISM_PTR pxMechanism,
                                                CK_ATTRIBUTE_PTR pxPublicKeyTemplate,
                                                CK_ULONG ulPublicKeyAttributeCount,
                                                CK_ATTRIBUTE_PTR pxPrivateKeyTemplate,
                                                CK_ULONG ulPrivateKeyAttributeCount,
                                                CK_OBJECT_HANDLE_PTR pxPublicKey,
                                                CK_OBJECT_HANDLE_PTR pxPrivateKey )
{
    /* Avoid warnings about unused parameters. */
    ( void ) ( pxPublicKey );
    ( void ) ( ulPrivateKeyAttributeCount );
    ( void ) ( ulPublicKeyAttributeCount );
    ( void ) ( xSession );

    PKCS11_GenerateKeyPrivateTemplatePtr_t pxPrivateTemplate = ( PKCS11_GenerateKeyPrivateTemplatePtr_t ) pxPrivateKeyTemplate;
    PKCS11_GenerateKeyPublicTemplatePtr_t pxPublicTemplate = ( PKCS11_GenerateKeyPublicTemplatePtr_t ) pxPublicKeyTemplate;

    CK_RV xResult = CKR_OK;
    uint8_t * pucDerFile = pvPortMalloc( pkcs11KEY_GEN_MAX_DER_SIZE );

    if( pucDerFile == NULL )
    {
        xResult = CKR_HOST_MEMORY;
    }

    if( CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism )
    {
        xResult = CKR_MECHANISM_PARAM_INVALID;
    }

    if( ( pxPrivateTemplate->xLabel.type != CKA_LABEL ) ||
        ( pxPublicTemplate->xLabel.type != CKA_LABEL ) ||
        ( pxPublicTemplate->xEcParams.type != CKA_EC_PARAMS ) )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    if( 0 != strcmp( pkcs11ELLIPTIC_CURVE_NISTP256, pxPublicTemplate->xEcParams.pValue ) )
    {
        xResult = CKR_CURVE_NOT_SUPPORTED;
    }

    mbedtls_pk_context xCtx;
    mbedtls_pk_init( &xCtx );
    xResult = mbedtls_pk_setup( &xCtx, mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY ) );

    if( xResult == 0 )
    {
        /*mbedtls_ecdsa_init( xCtx.pk_ctx ); */

        if( 0 != mbedtls_ecp_gen_key( MBEDTLS_ECP_DP_SECP256R1,
                                      mbedtls_pk_ec( xCtx ),
                                      mbedtls_ctr_drbg_random,
                                      &xP11Context.xMbedDrbgCtx ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = mbedtls_pk_write_key_der( &xCtx, pucDerFile, pkcs11KEY_GEN_MAX_DER_SIZE );
    }

    /* TODO: Write back public key value if CKA_VALUE is provided in template. */
    /* TODO: Think on how to give 2 different labels to private public key for non mbed implementations. */

    if( xResult > 0 )
    {
        *pxPrivateKey = PKCS11_PAL_SaveObject( &pxPrivateTemplate->xLabel, pucDerFile + pkcs11KEY_GEN_MAX_DER_SIZE - xResult, xResult );
        /* FIXME: This is a hack.*/
        *pxPublicKey = *pxPrivateKey + 1;
        xResult = CKR_OK;
    }
    else
    {
        xResult = CKR_GENERAL_ERROR;
    }

    /* Clean up. */
    if( NULL != pucDerFile )
    {
        vPortFree( pucDerFile );
    }

    mbedtls_pk_free( &xCtx );

    return xResult;
}

/**
 * @brief Generate cryptographically random bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{
    CK_RV xResult = CKR_OK;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( ( NULL == pucRandomData ) ||
        ( ulRandomLen == 0 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        if( 0 != mbedtls_ctr_drbg_random( &xP11Context.xMbedDrbgCtx, pucRandomData, ulRandomLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}
