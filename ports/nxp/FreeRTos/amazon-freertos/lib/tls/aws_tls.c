/*
 * Amazon FreeRTOS TLS V1.1.3
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_tls.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"
#include "task.h"
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

#ifdef TGT_A71CH
#  include "ax_mbedtls.h"
#  include "pkcs11_se.h"
#endif /* TGT_A71CH */

/* mbedTLS includes. */
#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/debug.h"
#ifdef MBEDTLS_DEBUG_C
    #define tlsDEBUG_VERBOSE    4
#endif

/* C runtime includes. */
#include <string.h>
#include <time.h>
#include <stdio.h>

/**
 * @brief Internal context structure.
 *
 * @param[in] pcDestination Server location, can be a DNS name or IP address.
 * @param[in] pcServerCertificate Server X.509 certificate in PEM format to trust.
 * @param[in] ulServerCertificateLength Length in bytes of the server certificate.
 * @param[in] xNetworkRecv Callback for receiving data on an open TCP socket.
 * @param[in] xNetworkSend Callback for sending data on an open TCP socket.
 * @param[in] pvCallerContext Opaque pointer provided by caller for above callbacks.
 * @param[out] xTLSCHandshakeSuccessful Indicates whether TLS handshake was successfully completed.
 * @param[out] xMbedSslCtx Connection context for mbedTLS.
 * @param[out] xMbedSslConfig Configuration context for mbedTLS.
 * @param[out] xMbedX509CA Server certificate context for mbedTLS.
 * @param[out] xMbedX509Cli Client certificate context for mbedTLS.
 * @param[out] mbedPkAltCtx RSA crypto implementation context for mbedTLS.
 * @param[out] xP11FunctionList PKCS#11 function list structure.
 * @param[out] xP11Session PKCS#11 session context.
 * @param[out] xP11PrivateKey PKCS#11 private key context.
 */
typedef struct TLSContext
{
    const char * pcDestination;
    const char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    const char ** ppcAlpnProtocols;
    uint32_t ulAlpnProtocolsCount;

    NetworkRecv_t xNetworkRecv;
    NetworkSend_t xNetworkSend;
    void * pvCallerContext;
    BaseType_t xTLSHandshakeSuccessful;

    /* mbedTLS. */
    mbedtls_ssl_context xMbedSslCtx;
    mbedtls_ssl_config xMbedSslConfig;
    mbedtls_x509_crt xMbedX509CA;
    mbedtls_x509_crt xMbedX509Cli;
    mbedtls_pk_context xMbedPkCtx;
    mbedtls_pk_info_t xMbedPkInfo;

    /* PKCS#11. */
    CK_FUNCTION_LIST_PTR xP11FunctionList;
    CK_SESSION_HANDLE xP11Session;
    CK_OBJECT_HANDLE xP11PrivateKey;
} TLSContext_t;


#define TLS_PRINT( X )    vLoggingPrintf X

/*
 * Helper routines.
 */

/**
 * @brief TLS internal context rundown helper routine.
 *
 * @param[in] pvContext Caller context.
 */
static void prvFreeContext( TLSContext_t * pxCtx )
{
    if( NULL != pxCtx )
    {
        /* Cleanup mbedTLS. */
        mbedtls_ssl_close_notify( &pxCtx->xMbedSslCtx ); /*lint !e534 The error is already taken care of inside mbedtls_ssl_close_notify*/
        mbedtls_ssl_free( &pxCtx->xMbedSslCtx );
        mbedtls_ssl_config_free( &pxCtx->xMbedSslConfig );

        /* Cleanup PKCS#11. */
        if( ( NULL != pxCtx->xP11FunctionList ) &&
            ( NULL != pxCtx->xP11FunctionList->C_CloseSession ) )
        {
            pxCtx->xP11FunctionList->C_CloseSession( pxCtx->xP11Session ); /*lint !e534 This function always return CKR_OK. */
        }

        pxCtx->xTLSHandshakeSuccessful = pdFALSE;
    }
}

/**
 * @brief Network send callback shim.
 *
 * @param[in] pvContext Caller context.
 * @param[in] pucData Byte buffer to send.
 * @param[in] xDataLength Length of byte buffer to send.
 *
 * @return Number of bytes sent, or a negative value on error.
 */
static int prvNetworkSend( void * pvContext,
                           const unsigned char * pucData,
                           size_t xDataLength )
{
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pxCtx->xNetworkSend( pxCtx->pvCallerContext, pucData, xDataLength );
}

/**
 * @brief Network receive callback shim.
 *
 * @param[in] pvContext Caller context.
 * @param[out] pucReceiveBuffer Byte buffer to receive into.
 * @param[in] xReceiveLength Length of byte buffer for receive.
 *
 * @return Number of bytes received, or a negative value on error.
 */
static int prvNetworkRecv( void * pvContext,
                           unsigned char * pucReceiveBuffer,
                           size_t xReceiveLength )
{
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pxCtx->xNetworkRecv( pxCtx->pvCallerContext, pucReceiveBuffer, xReceiveLength );
}

/**
 * @brief Callback that wraps PKCS#11 for pseudo-random number generation.
 *
 * @param[in] pvCtx Caller context.
 * @param[in] pucRandom Byte array to fill with random data.
 * @param[in] xRandomLength Length of byte array.
 *
 * @return Zero on success.
 */
static int prvGenerateRandomBytes( void * pvCtx,
                                   unsigned char * pucRandom,
                                   size_t xRandomLength )
{
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvCtx; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pxCtx->xP11FunctionList->C_GenerateRandom( pxCtx->xP11Session, pucRandom, xRandomLength );
}

/**
 * @brief Callback that enforces a worst-case expiration check on TLS server
 * certificates.
 *
 * @param[in] pvCtx Caller context.
 * @param[in] pxCertificate Certificate to check.
 * @param[in] lPathCount Location of this certificate in the chain.
 * @param[in] pulFlags Verification status flags.
 *
 * @return Zero on success.
 */
static int prvCheckCertificate( void * pvCtx,
                                mbedtls_x509_crt * pxCertificate,
                                int lPathCount,
                                uint32_t * pulFlags )
{
    int lCompilationYear = 0;

#define tlsCOMPILER_DATE_STRING_MONTH_LENGTH    4
    char cCompilationMonth[ tlsCOMPILER_DATE_STRING_MONTH_LENGTH ];
    int lCompilationMonth = 0;
    int lCompilationDay = 0;
    const char cMonths[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    /* Unreferenced parameters. */
    ( void ) ( pvCtx );
    ( void ) ( lPathCount );

    /* Parse the date string fields. */
    sscanf( __DATE__,
            "%3s %d %d",
            cCompilationMonth,
            &lCompilationDay,
            &lCompilationYear );
    cCompilationMonth[ tlsCOMPILER_DATE_STRING_MONTH_LENGTH - 1 ] = '\0';

    /* Check for server expiration. First check the year. */
    if( pxCertificate->valid_to.year < lCompilationYear )
    {
        *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
    }
    else if( pxCertificate->valid_to.year == lCompilationYear )
    {
        /* Convert the month. */
        lCompilationMonth =
            ( ( strstr( cMonths, cCompilationMonth ) - cMonths ) /
              ( tlsCOMPILER_DATE_STRING_MONTH_LENGTH - 1 ) ) + 1;

        /* Check the month. */
        if( pxCertificate->valid_to.mon < lCompilationMonth )
        {
            *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
        }
        else if( pxCertificate->valid_to.mon == lCompilationMonth )
        {
            /* Check the day. */
            if( pxCertificate->valid_to.day < lCompilationDay )
            {
                *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
            }
        }
    }

    return 0;
}


/**
 * @brief Sign a cryptographic hash with the private key.
 *
 * @param[in] pvContext Crypto context.
 * @param[in] xMdAlg Unused.
 * @param[in] pucHash Length in bytes of hash to be signed.
 * @param[in] uiHashLen Byte array of hash to be signed.
 * @param[out] pucSig RSA signature bytes.
 * @param[in] pxSigLen Length in bytes of signature buffer.
 * @param[in] piRng Unused.
 * @param[in] pvRng Unused.
 *
 * @return Zero on success.
 */
static int prvPrivateKeySigningCallback( void * pvContext,
                                         mbedtls_md_type_t xMdAlg,
                                         const unsigned char * pucHash,
                                         size_t xHashLen,
                                         unsigned char * pucSig,
                                         size_t * pxSigLen,
                                         int ( *piRng )( void *, unsigned char *, size_t ), /*lint !e955 This parameter is unused. */
                                         void * pvRng )
{
    BaseType_t xResult = 0;
    TLSContext_t * pxSession = ( TLSContext_t * ) pvContext;
    CK_MECHANISM xMech = { 0 };

    /* Unreferenced parameters. */
    ( void ) ( piRng );
    ( void ) ( pvRng );
    ( void ) ( xMdAlg );

    /* Use the PKCS#11 module to sign. */
    xMech.mechanism = CKM_SHA256;

    xResult = ( BaseType_t ) C_SignInit( pxSession->xP11Session,
                                         &xMech,
                                         pxSession->xP11PrivateKey );

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) C_Sign( ( CK_SESSION_HANDLE ) pxSession->xP11Session,
                                         ( CK_BYTE_PTR ) pucHash, /*lint !e9005 The interfaces are from 3rdparty libraries, we are not suppose to change them. */
                                         ( CK_ULONG ) xHashLen,
                                         pucSig,
                                         ( CK_ULONG_PTR ) pxSigLen );
    }

    if( xResult != 0 )
    {
        configPRINTF( ( "Failure in signing callback: %d \r\n", xResult ) );
    }

    return xResult;
}

/**
 * @brief Helper for setting up potentially hardware-based cryptographic context
 * for the client TLS certificate and private key.
 *
 * @param Caller context.
 *
 * @return Zero on success.
 */
static int prvInitializeClientCredential( TLSContext_t * pxCtx )
{
    BaseType_t xResult = 0;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG xCount = 1;
    CK_ATTRIBUTE xTemplate = { 0 };
    CK_OBJECT_HANDLE xCertObj = 0;
    CK_BYTE * pxCertificate = NULL;
    mbedtls_pk_type_t xKeyAlgo = ( mbedtls_pk_type_t ) ~0;
    CK_KEY_TYPE xKeyType = ( CK_KEY_TYPE ) ~0;

    /* Initialize the mbed contexts. */
    mbedtls_x509_crt_init( &pxCtx->xMbedX509Cli );

    /* Get the default private key storage ID. */
    if( CKR_OK == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_GetSlotList( CK_TRUE,
                                                                         &xSlotId,
                                                                         &xCount );
    }

    /* Start a private session with the P#11 module. */
    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_OpenSession( xSlotId,
                                                                         CKF_SERIAL_SESSION,
                                                                         NULL,
                                                                         NULL,
                                                                         &pxCtx->xP11Session );
    }

    /* Get the handle of the device private key. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );
        xTemplate.pValue = &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjectsInit( pxCtx->xP11Session,
                                                                             &xTemplate,
                                                                             1 );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjects( pxCtx->xP11Session,
                                                                         &pxCtx->xP11PrivateKey,
                                                                         1,
                                                                         &xCount );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjectsFinal( pxCtx->xP11Session );
    }

    if( xResult == CKR_OK )
    {
        xTemplate.type = CKA_KEY_TYPE;
        xTemplate.pValue = &xKeyType;
        xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );
        xResult = pxCtx->xP11FunctionList->C_GetAttributeValue( pxCtx->xP11Session,
                                                                pxCtx->xP11PrivateKey,
                                                                &xTemplate,
                                                                1 );
    }

    if( xResult == CKR_OK )
    {
        switch( xKeyType )
        {
            case CKK_RSA:
                xKeyAlgo = MBEDTLS_PK_RSA;
                break;

            case CKK_EC:
                xKeyAlgo = MBEDTLS_PK_ECKEY;
                break;

            default:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }
    }

    if( xResult == CKR_OK )
    {
        memcpy( &pxCtx->xMbedPkInfo, mbedtls_pk_info_from_type( xKeyAlgo ), sizeof( mbedtls_pk_info_t ) );

        pxCtx->xMbedPkInfo.sign_func = prvPrivateKeySigningCallback;
        pxCtx->xMbedPkCtx.pk_info = &pxCtx->xMbedPkInfo;
        pxCtx->xMbedPkCtx.pk_ctx = pxCtx;
    }

    if( 0 == xResult )
    {
        /* Enumerate the first client certificate. */
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS );
        xTemplate.pValue = &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjectsInit( pxCtx->xP11Session,
                                                                             &xTemplate,
                                                                             1 );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjects( pxCtx->xP11Session,
                                                                         &xCertObj,
                                                                         1,
                                                                         &xCount );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_FindObjectsFinal( pxCtx->xP11Session );
    }

    if( 0 == xResult )
    {
        /* Query the device certificate size. */
        xTemplate.type = CKA_VALUE;
        xTemplate.ulValueLen = 0;
        xTemplate.pValue = NULL;
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_GetAttributeValue( pxCtx->xP11Session,
                                                                               xCertObj,
                                                                               &xTemplate,
                                                                               1 );
    }

    if( 0 == xResult )
    {
        /* Create a buffer for the certificate. */
        pxCertificate = ( CK_BYTE_PTR ) pvPortMalloc( xTemplate.ulValueLen ); /*lint !e9079 Allow casting void* to other types. */

        if( NULL == pxCertificate )
        {
            xResult = ( BaseType_t ) CKR_HOST_MEMORY;
        }
    }

    if( 0 == xResult )
    {
        /* Export the certificate. */
        xTemplate.pValue = pxCertificate;
        xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_GetAttributeValue( pxCtx->xP11Session,
                                                                               xCertObj,
                                                                               &xTemplate,
                                                                               1 );
    }

    /* Decode the client certificate. */
    if( 0 == xResult )
    {
        xResult = mbedtls_x509_crt_parse( &pxCtx->xMbedX509Cli,
                                          ( const unsigned char * ) pxCertificate,
                                          xTemplate.ulValueLen );
    }

    /*
     * Add a JITR device issuer certificate, if present.
     */
    if( ( 0 == xResult ) &&
        ( NULL != clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM ) )
    {
        /* Decode the JITR issuer. The device client certificate will get
         * inserted as the first certificate in this chain below. */
        xResult = mbedtls_x509_crt_parse(
            &pxCtx->xMbedX509Cli,
            ( const unsigned char * ) clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM,
            1 + strlen( clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM ) );
    }

    /*
     * Attach the client certificate and private key to the TLS configuration.
     */
    if( 0 == xResult )
    {
        xResult = mbedtls_ssl_conf_own_cert( &pxCtx->xMbedSslConfig,
                                             &pxCtx->xMbedX509Cli,
                                             &pxCtx->xMbedPkCtx );
    }

    if( NULL != pxCertificate )
    {
        vPortFree( pxCertificate );
    }

    return xResult;
}

/*
 * Interface routines.
 */

BaseType_t TLS_Init( void ** ppvContext,
                     TLSParams_t * pxParams )
{
    BaseType_t xResult = 0;
    TLSContext_t * pxCtx = NULL;
    CK_C_GetFunctionList xCkGetFunctionList = NULL;

    /* Allocate an internal context. */
    pxCtx = ( TLSContext_t * ) pvPortMalloc( sizeof( TLSContext_t ) ); /*lint !e9087 !e9079 Allow casting void* to other types. */

    if( NULL != pxCtx )
    {
        memset( pxCtx, 0, sizeof( TLSContext_t ) );
        *ppvContext = pxCtx;

        /* Initialize the context. */
        pxCtx->pcDestination = pxParams->pcDestination;
        pxCtx->pcServerCertificate = pxParams->pcServerCertificate;
        pxCtx->ulServerCertificateLength = pxParams->ulServerCertificateLength;
        pxCtx->ppcAlpnProtocols = pxParams->ppcAlpnProtocols;
        pxCtx->ulAlpnProtocolsCount = pxParams->ulAlpnProtocolsCount;
        pxCtx->xNetworkRecv = pxParams->pxNetworkRecv;
        pxCtx->xNetworkSend = pxParams->pxNetworkSend;
        pxCtx->pvCallerContext = pxParams->pvCallerContext;

        /* Get the function pointer list for the PKCS#11 module. */
#ifdef TGT_A71CH
        /* different storage and retreival of Keys and certificates with A71CH */
        xCkGetFunctionList = C_GetFunctionList_sm;
#else
        xCkGetFunctionList = C_GetFunctionList;
#endif /* TGT_A71CH */
        xResult = ( BaseType_t ) xCkGetFunctionList( &pxCtx->xP11FunctionList );

        /* Ensure that the PKCS #11 module is initialized. */
        if( 0 == xResult )
        {
            xResult = ( BaseType_t ) pxCtx->xP11FunctionList->C_Initialize( NULL );

            /* It is ok if the module was previously initialized. */
            if( xResult == CKR_CRYPTOKI_ALREADY_INITIALIZED )
            {
                xResult = CKR_OK;
            }
        }
    }
    else
    {
        xResult = ( BaseType_t ) CKR_HOST_MEMORY;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

#ifdef MBEDTLS_DEBUG_C
    static void prvTlsDebugPrint( void * ctx,
                                  int lLevel,
                                  const char * pcFile,
                                  int lLine,
                                  const char * pcStr )
    {
        /* Unused parameters. */
        ( void ) ctx;
        ( void ) pcFile;
        ( void ) lLine;

        /* Send the debug string to the portable logger. */
        vLoggingPrintf( "mbedTLS: |%d| %s", lLevel, pcStr );
    }
#endif /* ifdef MBEDTLS_DEBUG_C */

/*-----------------------------------------------------------*/

BaseType_t TLS_Connect( void * pvContext )
{
    BaseType_t xResult = 0;
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    /* Initialize mbedTLS structures. */
    mbedtls_ssl_init( &pxCtx->xMbedSslCtx );
    mbedtls_ssl_config_init( &pxCtx->xMbedSslConfig );
    mbedtls_x509_crt_init( &pxCtx->xMbedX509CA );

    /* Decode the root certificate: either the default or the override. */
    if( NULL != pxCtx->pcServerCertificate )
    {
        xResult = mbedtls_x509_crt_parse( &pxCtx->xMbedX509CA,
                                          ( const unsigned char * ) pxCtx->pcServerCertificate,
                                          pxCtx->ulServerCertificateLength );

        if( 0 != xResult )
        {
            TLS_PRINT( ( "ERROR: Failed to parse custom server certificates %d \r\n", xResult ) );
        }
    }
    else
    {
        xResult = mbedtls_x509_crt_parse( &pxCtx->xMbedX509CA,
                                          ( const unsigned char * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM,
                                          tlsVERISIGN_ROOT_CERTIFICATE_LENGTH );

        if(( 0 == xResult ) && (tlsATS1_ROOT_CERTIFICATE_LENGTH > 1))
        {
            xResult = mbedtls_x509_crt_parse( &pxCtx->xMbedX509CA,
                                              ( const unsigned char * ) tlsATS1_ROOT_CERTIFICATE_PEM,
                                              tlsATS1_ROOT_CERTIFICATE_LENGTH );
            if ( 0 == xResult )
            {
                xResult = mbedtls_x509_crt_parse( &pxCtx->xMbedX509CA,
                                                  ( const unsigned char * ) tlsSTARFIELD_ROOT_CERTIFICATE_PEM,
                                                  tlsSTARFIELD_ROOT_CERTIFICATE_LENGTH );
            }
        }

        if( 0 != xResult )
        {
            /* Default root certificates should be in aws_default_root_certificate.h */
            TLS_PRINT( ( "ERROR: Failed to parse default server certificates %d \r\n", xResult ) );
        }
    }

    /* Start with protocol defaults. */
    if( 0 == xResult )
    {
        xResult = mbedtls_ssl_config_defaults( &pxCtx->xMbedSslConfig,
                                               MBEDTLS_SSL_IS_CLIENT,
                                               MBEDTLS_SSL_TRANSPORT_STREAM,
                                               MBEDTLS_SSL_PRESET_DEFAULT );
    }

    if( 0 == xResult )
    {
        /* Use a callback for additional server certificate validation. */
        mbedtls_ssl_conf_verify( &pxCtx->xMbedSslConfig,
                                 &prvCheckCertificate,
                                 pxCtx );

        /* Server certificate validation is mandatory. */
        mbedtls_ssl_conf_authmode( &pxCtx->xMbedSslConfig, MBEDTLS_SSL_VERIFY_REQUIRED );

        /* Set the RNG callback. */
        mbedtls_ssl_conf_rng( &pxCtx->xMbedSslConfig, &prvGenerateRandomBytes, pxCtx ); /*lint !e546 Nothing wrong here. */

        /* Set issuer certificate. */
        mbedtls_ssl_conf_ca_chain( &pxCtx->xMbedSslConfig, &pxCtx->xMbedX509CA, NULL );

        /* Configure the SSL context for the device credentials. */
        xResult = prvInitializeClientCredential( pxCtx );
    }

    if( ( 0 == xResult ) && ( NULL != pxCtx->ppcAlpnProtocols ) )
    {
        /* Include an application protocol list in the TLS ClientHello
         * message. */
        xResult = mbedtls_ssl_conf_alpn_protocols(
            &pxCtx->xMbedSslConfig,
            pxCtx->ppcAlpnProtocols );
    }

    #ifdef MBEDTLS_DEBUG_C

        /* If mbedTLS is being compiled with debug support, assume that the
         * runtime configuration should use verbose output. */
        mbedtls_ssl_conf_dbg( &pxCtx->xMbedSslConfig, prvTlsDebugPrint, NULL );
        mbedtls_debug_set_threshold( tlsDEBUG_VERBOSE );
    #endif

    if( 0 == xResult )
    {
        /* Set the resulting protocol configuration. */
        xResult = mbedtls_ssl_setup( &pxCtx->xMbedSslCtx, &pxCtx->xMbedSslConfig );
    }

#ifdef TGT_A71CH
    if( 0 == xResult )
    {
        if ( clientcredentialCLIENT_PRIVATE_KEY_LENGTH == 1) {
            xResult = ax_mbedtls_associate_ecdhctx(clientcredentialCLIENT_PRIVATE_KEY_PEM[0], pxCtx->xMbedSslCtx.handshake );
        }
    }
#endif /* TGT_A71CH */

    /* Set the hostname, if requested. */
    if( ( 0 == xResult ) && ( NULL != pxCtx->pcDestination ) )
    {
        xResult = mbedtls_ssl_set_hostname( &pxCtx->xMbedSslCtx, pxCtx->pcDestination );
    }

    /* Set the socket callbacks. */
    if( 0 == xResult )
    {
        mbedtls_ssl_set_bio( &pxCtx->xMbedSslCtx,
                             pxCtx,
                             prvNetworkSend,
                             prvNetworkRecv,
                             NULL );

        /* Negotiate. */
        while( 0 != ( xResult = mbedtls_ssl_handshake( &pxCtx->xMbedSslCtx ) ) )
        {
            if( ( MBEDTLS_ERR_SSL_WANT_READ != xResult ) &&
                ( MBEDTLS_ERR_SSL_WANT_WRITE != xResult ) )
            {
                /* There was an unexpected error. Per mbedTLS API documentation,
                 * ensure that upstream clean-up code doesn't accidentally use
                 * a context that failed the handshake. */
                prvFreeContext( pxCtx );
                TLS_PRINT( ( "ERROR: Handshake failed with error code %d \r\n", xResult ) );
                break;
            }
        }
    }

    /* Keep track of successful completion of the handshake. */
    if( 0 == xResult )
    {
        pxCtx->xTLSHandshakeSuccessful = pdTRUE;
    }

    /* Free up allocated memory. */
    mbedtls_x509_crt_free( &pxCtx->xMbedX509CA );
    mbedtls_x509_crt_free( &pxCtx->xMbedX509Cli );

    return xResult;
}

/*-----------------------------------------------------------*/

BaseType_t TLS_Recv( void * pvContext,
                     unsigned char * pucReadBuffer,
                     size_t xReadLength )
{
    BaseType_t xResult = 0;
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */
    size_t xRead = 0;

    if( ( NULL != pxCtx ) && ( pdTRUE == pxCtx->xTLSHandshakeSuccessful ) )
    {
        while( xRead < xReadLength )
        {
            xResult = mbedtls_ssl_read( &pxCtx->xMbedSslCtx,
                                        pucReadBuffer + xRead,
                                        xReadLength - xRead );

            if( 0 < xResult )
            {
                /* Got data, so update the tally and keep looping. */
                xRead += ( size_t ) xResult;
            }
            else if( 0 == xResult )
            {
                /* No data received (and no error). The secure sockets
                 * API supports non-blocking read, so stop the loop but don't
                 * flag an error. */
                break;
            }
            else if( MBEDTLS_ERR_SSL_WANT_READ != xResult )
            {
                /* Hard error: invalidate the context and stop. */
                prvFreeContext( pxCtx );
                break;
            }
        }
    }
    else
    {
        xResult = MBEDTLS_ERR_SSL_INTERNAL_ERROR;
    }

    if( 0 <= xResult )
    {
        xResult = ( BaseType_t ) xRead;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

BaseType_t TLS_Send( void * pvContext,
                     const unsigned char * pucMsg,
                     size_t xMsgLength )
{
    BaseType_t xResult = 0;
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */
    size_t xWritten = 0;

    if( ( NULL != pxCtx ) && ( pdTRUE == pxCtx->xTLSHandshakeSuccessful ) )
    {
        while( xWritten < xMsgLength )
        {
            xResult = mbedtls_ssl_write( &pxCtx->xMbedSslCtx,
                                         pucMsg + xWritten,
                                         xMsgLength - xWritten );

            if( 0 < xResult )
            {
                /* Sent data, so update the tally and keep looping. */
                xWritten += ( size_t ) xResult;
            }
            else if( 0 == xResult )
            {
                /* No data sent (and no error). The secure sockets
                 * API supports non-blocking send, so stop the loop but don't
                 * flag an error. */
                break;
            }
            else if( MBEDTLS_ERR_SSL_WANT_WRITE != xResult )
            {
                /* Hard error: invalidate the context and stop. */
                prvFreeContext( pxCtx );
                break;
            }
        }
    }
    else
    {
        xResult = MBEDTLS_ERR_SSL_INTERNAL_ERROR;
    }

    if( 0 <= xResult )
    {
        xResult = ( BaseType_t ) xWritten;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void TLS_Cleanup( void * pvContext )
{
    TLSContext_t * pxCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    if( NULL != pxCtx )
    {
        if( pdTRUE == pxCtx->xTLSHandshakeSuccessful )
        {
            prvFreeContext( pxCtx );
        }

        /* Free memory. */
        vPortFree( pxCtx );
    }
}
