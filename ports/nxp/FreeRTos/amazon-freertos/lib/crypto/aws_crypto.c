/*
 * Amazon FreeRTOS Crypto V1.0.3
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
#include "aws_crypto.h"

/* mbedTLS includes. */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/platform.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"

/* C runtime includes. */
#include <string.h>

/**
 * @brief Internal signature verification context structure
 */
typedef struct SignatureVerificationState
{
    BaseType_t xAsymmetricAlgorithm;
    BaseType_t xHashAlgorithm;
    mbedtls_sha1_context xSHA1Context;
    mbedtls_sha256_context xSHA256Context;
} SignatureVerificationState_t, * SignatureVerificationStatePtr_t;

/*
 * Helper routines
 */

/**
 * @brief Implements libc calloc semantics using the FreeRTOS heap
 */
static void * prvCalloc( size_t xNmemb,
                         size_t xSize )
{
    void * pvNew = pvPortMalloc( xNmemb * xSize );

    if( NULL != pvNew )
    {
        memset( pvNew, 0, xNmemb * xSize );
    }

    return pvNew;
}

/**
 * @brief Verifies a cryptographic signature based on the signer
 * certificate, hash algorithm, and the data that was signed.
 */
static BaseType_t prvVerifySignature( char * pcSignerCertificate,
                                      size_t xSignerCertificateLength,
                                      BaseType_t xHashAlgorithm,
                                      uint8_t * pucHash,
                                      size_t xHashLength,
                                      uint8_t * pucSignature,
                                      size_t xSignatureLength )
{
    BaseType_t xResult = pdTRUE;
    mbedtls_x509_crt xCertCtx;
    mbedtls_md_type_t xMbedHashAlg = MBEDTLS_MD_SHA256;


    memset( &xCertCtx, 0, sizeof( mbedtls_x509_crt ) );

    /*
     * Map the hash algorithm
     */
    if( cryptoHASH_ALGORITHM_SHA1 == xHashAlgorithm )
    {
        xMbedHashAlg = MBEDTLS_MD_SHA1;
    }

    /*
     * Decode and create a certificate context
     */
    mbedtls_x509_crt_init( &xCertCtx );

    if( 0 != mbedtls_x509_crt_parse(
            &xCertCtx, ( const unsigned char * ) pcSignerCertificate, xSignerCertificateLength ) )
    {
        xResult = pdFALSE;
    }

    /*
     * Verify the signature using the public key from the decoded certificate
     */
    if( pdTRUE == xResult )
    {
        if( 0 != mbedtls_pk_verify(
                &xCertCtx.pk,
                xMbedHashAlg,
                pucHash,
                xHashLength,
                pucSignature,
                xSignatureLength ) )
        {
            xResult = pdFALSE;
        }
    }

    /*
     * Clean-up
     */
    mbedtls_x509_crt_free( &xCertCtx );

    return xResult;
}

/*
 * Interface routines
 */

/**
 * @brief Overrides CRT heap callouts to use FreeRTOS instead
 */
void CRYPTO_ConfigureHeap( void )
{
    /*
     * Ensure that the FreeRTOS heap is used
     */
    mbedtls_platform_set_calloc_free( prvCalloc, vPortFree ); /*lint !e534 This function always return 0. */
}

/**
 * @brief Creates signature verification context.
 */
BaseType_t CRYPTO_SignatureVerificationStart( void ** ppvContext,
                                              BaseType_t xAsymmetricAlgorithm,
                                              BaseType_t xHashAlgorithm )
{
    BaseType_t xResult = pdTRUE;
    SignatureVerificationStatePtr_t pxCtx = NULL;

    /*
     * Allocate the context
     */
    if( NULL == ( pxCtx = ( SignatureVerificationStatePtr_t ) pvPortMalloc(
                      sizeof( *pxCtx ) ) ) ) /*lint !e9087 Allow casting void* to other types. */
    {
        xResult = pdFALSE;
    }

    if( pdTRUE == xResult )
    {
        *ppvContext = pxCtx;

        /*
         * Store the algorithm identifiers
         */
        pxCtx->xAsymmetricAlgorithm = xAsymmetricAlgorithm;
        pxCtx->xHashAlgorithm = xHashAlgorithm;

        /*
         * Initialize the requested hash type
         */
        if( cryptoHASH_ALGORITHM_SHA1 == pxCtx->xHashAlgorithm )
        {
            mbedtls_sha1_init( &pxCtx->xSHA1Context );
            ( void )mbedtls_sha1_starts_ret( &pxCtx->xSHA1Context );
        }
        else
        {
            mbedtls_sha256_init( &pxCtx->xSHA256Context );
            ( void )mbedtls_sha256_starts_ret( &pxCtx->xSHA256Context, 0 );
        }
    }

    return xResult;
}

/**
 * @brief Adds bytes to an in-progress hash for subsequent signature
 * verification.
 */
void CRYPTO_SignatureVerificationUpdate( void * pvContext,
                                         const uint8_t * pucData,
                                         size_t xDataLength )
{
    SignatureVerificationStatePtr_t pxCtx = ( SignatureVerificationStatePtr_t ) pvContext; /*lint !e9087 Allow casting void* to other types. */

    /*
     * Add the data to the hash of the requested type
     */
    if( cryptoHASH_ALGORITHM_SHA1 == pxCtx->xHashAlgorithm )
    {
        ( void )mbedtls_sha1_update_ret( &pxCtx->xSHA1Context, pucData, xDataLength );
    }
    else
    {
        ( void )mbedtls_sha256_update_ret( &pxCtx->xSHA256Context, pucData, xDataLength );
    }
}

/**
 * @brief Performs signature verification on a cryptographic hash.
 */
BaseType_t CRYPTO_SignatureVerificationFinal( void * pvContext,
                                              char * pcSignerCertificate,
                                              size_t xSignerCertificateLength,
                                              uint8_t * pucSignature,
                                              size_t xSignatureLength )
{
    BaseType_t xResult = pdFALSE;
	if ( pvContext != NULL )
	{
		SignatureVerificationStatePtr_t pxCtx =
			(SignatureVerificationStatePtr_t)pvContext;	/*lint !e9087 Allow casting void* to other types. */
		uint8_t ucSHA1or256[cryptoSHA256_DIGEST_BYTES];	/* Reserve enough space for the larger of SHA1 or SHA256 results. */
		uint8_t * pucHash = NULL;
		size_t xHashLength = 0;

		if ((pcSignerCertificate != NULL) &&
			(pucSignature != NULL) &&
			(xSignerCertificateLength > 0UL) &&
			(xSignatureLength > 0UL))
		{
			/*
			 * Finish the hash
			 */
			if (cryptoHASH_ALGORITHM_SHA1 == pxCtx->xHashAlgorithm)
			{
				(void)mbedtls_sha1_finish_ret (&pxCtx->xSHA1Context, ucSHA1or256);
				pucHash = ucSHA1or256;
				xHashLength = cryptoSHA1_DIGEST_BYTES;
			}
			else
			{
				(void)mbedtls_sha256_finish_ret (&pxCtx->xSHA256Context, ucSHA1or256);
				pucHash = ucSHA1or256;
				xHashLength = cryptoSHA256_DIGEST_BYTES;
			}

			/*
			 * Verify the signature
			 */
			xResult = prvVerifySignature (pcSignerCertificate,
				xSignerCertificateLength,
				pxCtx->xHashAlgorithm,
				pucHash,
				xHashLength,
				pucSignature,
				xSignatureLength);
		}
		else
		{
			/* Allow function to be called with only the context pointer for cleanup after a failure. */
		}
		/*
		 * Clean-up
		 */
		vPortFree (pxCtx);
	}
    return xResult;
}
