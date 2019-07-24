/*
 * Amazon FreeRTOS Demo Bootloader V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_boot_loader.c
 * @brief Application loader implementation.
 */

/* Bootloader includes.*/
#include "aws_boot_types.h"
#include "aws_boot_config.h"
#include "aws_boot_log.h"
#include "aws_boot_partition.h"
#include "aws_boot_loader.h"

/**
 * @brief Bootloader data.
 */

/* Bootloader state, start in initialization state.*/
static BOOTState_t xBootState = eBootStateInit;

/* Application descriptor of image to be executed.*/
static const BOOTImageDescriptor_t * pxAppDescriptorExec;

/* Default application execution address.*/
static const void * pvDefaultExecAddress;

/**
 * @brief Bootloader status function prototype.
 *
 * @param[in] void.
 *
 * @return Returns the new state of bootloader.
 */
typedef BOOTState_t (* BootStateFunction_t)( void );

/* Bootloader Initialize. */
static BOOTState_t prvBOOT_Init( void );

/* Validate application images. */
static BOOTState_t prvBOOT_ValidateImages( void );

/* Start execution of selected image. */
static BOOTState_t prvBOOT_ExecuteImage( void );

/* Start default image execution. */
static BOOTState_t prvBOOT_ExecuteDefault( void );

/* Error state. */
static BOOTState_t prvBOOT_Error( void );

/**
 * @brief Bootloader function table.
 * This defines the function table to handle all bootloader states.
 */
static const BootStateFunction_t apfBootStateFunctionTable[] =
{
    prvBOOT_Init,           /**< Initialize the bootloader.*/
    prvBOOT_ValidateImages, /**< Validate the application images.*/
    prvBOOT_ExecuteImage,   /**< Start executiong the selected image.*/
    prvBOOT_ExecuteDefault, /**< Start executiong the default image.*/
    prvBOOT_Error           /**< Handle error state.*/
};

/**
 * @brief private function prototypes.
 */

/**
 * @brief Validate an application image.
 * Validates the application image present in application image slot
 * pointed by the application descriptor.
 * The image is validated in following steps -
 * - Image signature is verified
 * - Image flags are validated
 * - HWID is verified
 * - Address entries are verified
 * - Crypto signature is verified
 * @param[in] pxAppDescriptor ptr to the application descriptor of the image to
 * be validated.
 * @return pdPASS if validation successful, pdFAIL otherwise
 */
static BaseType_t prvValidateImage( const BOOTImageDescriptor_t * pxAppDescriptor );

/**
 * @brief Invalidate an application image.
 * If the application image fails validation it should be invalidated to avoid
 * further use by erasing the header. If bootconfigENABLE_ERASE_INVALID is set
 * the flash area is completely erased.It is recommended to erase entire image
 * if it fails any validations.
 * @param[in] pxAppDescriptor ptr to the application descriptor of the image to
 * be invalidated.
 * @return pdPASS if invalidation successful, pdFAIL otherwise
 */
static BaseType_t prvInvalidateImage( const BOOTImageDescriptor_t * pxAppDescriptor );

/**
 * @brief Launch application into execution.
 * This launches application into execution from the address passed by calling
 * platform specific launch function.
 * @param[in] pvLaunchAddress - Application execution address
 * @return void - It should never return.
 */
static void prvLaunchApplication( const BOOTImageDescriptor_t * pxAppDesc );

/**
 * @brief Bootloader Tasks.
 * This execute bootloader tasks as per bootloader state.
 */
void BOOTLOADER_Tasks( void )
{
    /**
     *  Basic check.
     */
    if( xBootState >= ( sizeof( apfBootStateFunctionTable ) / sizeof( *apfBootStateFunctionTable ) ) )
    {
        xBootState = eBootStateError;
    }

    /**
     *  Dispatch.
     */
    xBootState = ( *apfBootStateFunctionTable[ xBootState ] )();
}

/*-----------------------------------------------------------*/

static BOOTState_t prvBOOT_Init( void )
{
    DEFINE_BOOT_METHOD_NAME( "prvBOOT_Init" );

    BOOTState_t xReturnState = eBootStateError;

    BOOT_LOG_L1( "\nBootloader version %02d.", BOOTLOADER_VERSION_MAJOR );
    BOOT_LOG_L1( "%02d.", BOOTLOADER_VERSION_MINOR );
    BOOT_LOG_L1( "%02d\r\n", BOOTLOADER_VERSION_BUILD );

    /**
     * Initialize application descriptor for image to be executed.
     */
    pxAppDescriptorExec = NULL;

    /**
     * Initialize  watchdog.
     */
    #if ( bootconfigENABLE_WATCHDOG_TIMER == 1 )
        {
            if( pdTRUE == BOOT_PAL_WatchdogInit() )
            {
                BOOT_LOG_L1( "[%s] Watchdog timer initialized.\r\n", BOOT_METHOD_NAME );
                xReturnState = eBootStateValidation;
            }
            else
            {
                BOOT_LOG_L1( "[%s] Watchdog timer initialization failed.\r\n", BOOT_METHOD_NAME );
                xReturnState = eBootStateError;
            }
        }
    #else /* if ( bootconfigENABLE_WATCHDOG_TIMER == 1 ) */
        {
            BOOT_LOG_L1( "BOOT_Init : Watchdog config disabled.\r\n" );
        }
    #endif /* if ( bootconfigENABLE_WATCHDOG_TIMER == 1 ) */

    /**
     * Initialize the crypto library.
     */
    #if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 )
        if( pdTRUE == BOOT_CRYPTO_Init() )
        {
            BOOT_LOG_L1( "[%s] Crypto initialized.\r\n", BOOT_METHOD_NAME );
            xReturnState = eBootStateValidation;
        }
        else
        {
            BOOT_LOG_L1( "[%s] Crypto initialization failed.\r\n", BOOT_METHOD_NAME );
            xReturnState = eBootStateError;
        }
    #else /* if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 ) */
        {
            BOOT_LOG_L1( "BOOT_ValidateImages: Crypto signature verification is disabled.\r\n" );
        }
    #endif /* if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 ) */

    /**
     * Return next state.
     */
    return xReturnState;
}

/*-----------------------------------------------------------*/

static BOOTState_t prvBOOT_ValidateImages( void )
{
    DEFINE_BOOT_METHOD_NAME( "prvBOOT_ValidateImages" );

    BOOTState_t xReturnState = eBootStateError;
    uint32_t ulSeqNumber = 0;
    uint8_t ucIndex = 0;

    /* Partition Info*/
    BOOTPartition_Info_t xPartitionInfo;
    memset( &xPartitionInfo, 0x00, sizeof( xPartitionInfo ) );

    /**
     * Read partition table and get application image descriptors for
     * OTA image slots.
     */
    if( pdTRUE == BOOT_FLASH_ReadPartitionTable( &xPartitionInfo ) )
    {
        /**
         * Validate the application images and find out the newest valid
         * image to boot.
         */
        for( ucIndex = 0; ucIndex < xPartitionInfo.ucNumOfApps; ucIndex++ )
        {
            if( pdPASS == prvValidateImage( xPartitionInfo.paxOTAAppDescriptor[ ucIndex ] ) )
            {
                if( ulSeqNumber < xPartitionInfo.paxOTAAppDescriptor[ ucIndex ]->ulSequenceNum )
                {
                    ulSeqNumber = xPartitionInfo.paxOTAAppDescriptor[ ucIndex ]->ulSequenceNum;
                    pxAppDescriptorExec = xPartitionInfo.paxOTAAppDescriptor[ ucIndex ];
                }
            }
            else
            {
                /**
                 * The image failed validation so invalidate it by erasing header
                 * and erasing bank if full erase is set in config.
                 */
                BOOT_LOG_L1( "[%s] Validation failed for image at 0x%08x \r\n", BOOT_METHOD_NAME, xPartitionInfo.paxOTAAppDescriptor[ ucIndex ]);

                if( pdPASS != prvInvalidateImage( xPartitionInfo.paxOTAAppDescriptor[ ucIndex ] ) )
                {
                    BOOT_LOG_L1( "[%s] Invalidation failed.\r\n", BOOT_METHOD_NAME );
                }
            }
        }
    }
    else
    {
        BOOT_LOG_L1( "[%s] Partition table error.\r\n", BOOT_METHOD_NAME );
    }

    /**
     *  check if we have an image to execute in OTA partitions.
     */
    if( pxAppDescriptorExec != NULL )
    {
        BOOT_LOG_L1( "\n[%s] Booting image with sequence number %d at 0x%08x \r\n", BOOT_METHOD_NAME, ulSeqNumber, pxAppDescriptorExec );

        /* We have an image to execute. */
        xReturnState = eBootStateExecuteImage;
    }
    else
    {
        /**
         * Valid images are not present in any OTA partitions so execute the
         * default image. This will only execute the default application image
         * and will not update any image header flags.
         */
        #if ( bootconfigENABLE_DEFAULT_START == 1 )
            {
                if( xPartitionInfo.pvDefaultAppExecAddress != NULL )
                {
                    BOOT_LOG_L1( "\n[%s] Booting default image. \r\n", BOOT_METHOD_NAME );
                    pvDefaultExecAddress = xPartitionInfo.pvDefaultAppExecAddress;

                    /* Execute default image. */
                    xReturnState = eBootStateExecuteDefault;
                }
                else
                {
                    BOOT_LOG_L1( "\n[%s] Default image not found.\r\n", BOOT_METHOD_NAME );
                    xReturnState = eBootStateError;
                }
            }
        #else /* if ( bootconfigENABLE_DEFAULT_START == 1 ) */
            {
                BOOT_LOG_L1( "BOOT_ValidateImages : Default image execution disabled.\r\n", BOOT_METHOD_NAME );
            }
        #endif /* if ( bootconfigENABLE_DEFAULT_START == 1 ) */
    }

    return xReturnState;
}

/*-----------------------------------------------------------*/

static BOOTState_t prvBOOT_ExecuteImage( void )
{
    DEFINE_BOOT_METHOD_NAME( "prvBOOT_ExecuteImage" );

    BaseType_t xReturn = pdFALSE;
    uint8_t ucImageFlags = 0;

    BOOTImageDescriptor_t xCopyDescriptor;
    memset( &xCopyDescriptor, 0x00, sizeof( xCopyDescriptor ) );

    if( pxAppDescriptorExec == NULL )
    {
        return eBootStateError;
    }

    xCopyDescriptor = *pxAppDescriptorExec;
    ucImageFlags = pxAppDescriptorExec->xImageHeader.ucImageFlags;

    if( ucImageFlags == ( uint8_t )eBootImageFlagValid )
    {
        /* This is a valid image. */
        xReturn = pdTRUE;
    }
    else if( ucImageFlags == (uint8_t) eBootImageFlagNew )
    {
        /* This is the first launch of this image, set the status to commit pending. */
        xCopyDescriptor.xImageHeader.ucImageFlags = eBootImageFlagCommitPending;

        BOOT_LOG_L1( "[%s] Updating the flags: 0x%02x, to: 0x%02x, for image at 0x%08x\r\n",
                     BOOT_METHOD_NAME,
                     ucImageFlags,
                     xCopyDescriptor.xImageHeader.ucImageFlags,
                     pxAppDescriptorExec );

        if( BOOT_FLASH_Write( pxAppDescriptorExec->xImageHeader.ulAlign,
                              xCopyDescriptor.xImageHeader.ulAlign,
                              sizeof( xCopyDescriptor ) ) )
        {
            BOOT_LOG_L1( "[%s] Updated the image flags: 0x%08x\r\n",
                         BOOT_METHOD_NAME,
                         pxAppDescriptorExec );
            xReturn = pdTRUE;
        }
        else
        {
            BOOT_LOG_L1( "[%s] Failed to update the image flags: 0x%08x\r\n",
                         BOOT_METHOD_NAME,
                         pxAppDescriptorExec );
            xReturn = pdFALSE;
        }

        /**
         * Enable watchdog.
         */
        #if ( bootconfigENABLE_WATCHDOG_TIMER == 1 )
            {
                if( pdTRUE == BOOT_PAL_WatchdogEnable() )
                {
                    BOOT_LOG_L1( "[%s] Watchdog timer started.\r\n", BOOT_METHOD_NAME );
                    xReturn = pdTRUE;
                }
                else
                {
                    BOOT_LOG_L1( "[%s] Watchdog timer start failed.\r\n", BOOT_METHOD_NAME );
                    xReturn = pdFALSE;
                }
            }
        #endif /* if ( bootconfigENABLE_WATCHDOG_TIMER == 1 ) */
    }

    if( xReturn == pdTRUE )
    {
        /* Launch, never returns from here. */
        BOOT_PAL_LaunchApplicationDesc( pxAppDescriptorExec );
    }
    else
    {
        return eBootStateError;
    }
}

/*-----------------------------------------------------------*/

static BOOTState_t prvBOOT_ExecuteDefault( void )
{
    if( pvDefaultExecAddress == NULL )
    {
        return eBootStateError;
    }

    /* Launch, never returns from here. */
    BOOT_PAL_LaunchApplication( pvDefaultExecAddress );
}

/*-----------------------------------------------------------*/

static BOOTState_t prvBOOT_Error( void )
{
    DEFINE_BOOT_METHOD_NAME( "prvBOOT_Error" );

    BOOT_LOG_L1( "[%s] No valid image found.\r\n", BOOT_METHOD_NAME );
    
    /* Nothing to Boot, notify error. */
    BOOT_PAL_NotifyBootError();
}

/*-----------------------------------------------------------*/

static BaseType_t prvValidateImage( const BOOTImageDescriptor_t * pxAppDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "prvValidateImage" );

    BaseType_t xReturn = pdFALSE;
    uint8_t * pucTrailerAddress = NULL;
    uint8_t * pucStartAddress = NULL;
    BOOTImageTrailer_t * pxImgTrailer = NULL;
    uint32_t ulSizeApp = 0;

    /* Get the image flags.*/
    uint8_t ucImageFlags = pxAppDescriptor->xImageHeader.ucImageFlags;
    
    /* Get Flash bank area. */
    uint8_t u8FlashAreaID = BOOT_FLASH_GetFlashArea(pxAppDescriptor);
    
    BOOT_LOG_L1( "\n[%s] Validating image at Bank : %d\r\n",
                  BOOT_METHOD_NAME,
                  u8FlashAreaID );

    /**
     *  Check if valid magic code is present for the application image.
     */
    if( memcmp( pxAppDescriptor->xImageHeader.acMagicCode,
                BOOT_MAGIC_CODE,
                BOOT_MAGIC_CODE_SIZE ) != 0 )
    {
        BOOT_LOG_L1( "[%s] No application image or magic code present at: 0x%08x\r\n",
                     BOOT_METHOD_NAME,
                     pxAppDescriptor );
        xReturn = pdFALSE;
    }
    else
    {
        BOOT_LOG_L1( "[%s] Valid magic code at: 0x%08x\r\n",
                     BOOT_METHOD_NAME,
                     pxAppDescriptor );
        xReturn = pdTRUE;
    }

    /**
     *  Validate the image flags.
     */
    if( xReturn == pdTRUE )
    {
        if( ( ucImageFlags != (uint8_t) eBootImageFlagNew ) && ( ucImageFlags != (uint8_t) eBootImageFlagValid ) )
        {
            /* Image probably failed the validation, can not run this image. */
            BOOT_LOG_L1( "[%s] Invalid image flags: 0x%02x at: 0x%08x\r\n",
                         BOOT_METHOD_NAME,
                         ucImageFlags,
                         pxAppDescriptor );
            xReturn = pdFALSE;
        }
        else
        {
            BOOT_LOG_L1( "[%s] Valid image flags: 0x%02x at: 0x%08x\r\n",
                         BOOT_METHOD_NAME,
                         ucImageFlags,
                         pxAppDescriptor );
            xReturn = pdTRUE;
        }
    }

    /**
     *  Check if application image is built for this Hardware.
     */
    #if ( bootconfigENABLE_HWID_VALIDATION == 1 )
        {
            if( xReturn == pdTRUE )
            {
                if( pxAppDescriptor->ulHardwareID == bootconfigHARDWARE_ID )
                {
                    BOOT_LOG_L1( "[%s] Hardware ID is valid.\r\n", BOOT_METHOD_NAME );
                    xReturn = pdTRUE;
                }
                else
                {
                    BOOT_LOG_L1( "[%s] Hardware ID is invalid.\r\n", BOOT_METHOD_NAME );
                    xReturn = pdFALSE;
                }
            }
        }
    #else /* if ( bootconfigENABLE_HWID_VALIDATION == 1 ) */
        {
            BOOT_LOG_L3( "[%s] Hardware ID validation disabled.\r\n", BOOT_METHOD_NAME );
        }
    #endif /* if ( bootconfigENABLE_HWID_VALIDATION == 1 ) */

    /**
     *  Validate the addresses.
     */
    #if ( bootconfigENABLE_ADDRESS_VALIDATION == 1 )
        if( xReturn == pdTRUE )
        {
            if( pdTRUE == BOOT_FLASH_ValidateAddress( pxAppDescriptor ) )
            {
                BOOT_LOG_L1( "[%s] Addresses are valid.\r\n", BOOT_METHOD_NAME );
                xReturn = pdTRUE;
            }
            else
            {
                BOOT_LOG_L1( "[%s]  Addresses are not valid.\r\n", BOOT_METHOD_NAME );
                xReturn = pdFALSE;
            }
        }
    #else /* if ( bootconfigENABLE_ADDRESS_VALIDATION == 1 ) */
        {
            BOOT_LOG_L1( "[%s] Address validation is disabled.\r\n", BOOT_METHOD_NAME );
        }
    #endif /* if ( bootconfigENABLE_ADDRESS_VALIDATION == 1 ) */

    /**
     * Verify the crypto signature of application image.
     */
    #if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 )
        if( xReturn == pdTRUE )
        {
            /* Start address of the application image after header. */
            pucStartAddress = ( uint8_t * ) pxAppDescriptor;
            pucStartAddress = pucStartAddress + sizeof( BOOTImageHeader_t );

            /* Size of the application. */
            ulSizeApp = pxAppDescriptor->pvEndAddress - pxAppDescriptor->pvStartAddress;

            /* Application trailer. */
            pucTrailerAddress = pucStartAddress + ulSizeApp;
            
            /* Align it to BOOT_QUAD_WORD_SIZE. */
            if ( ( (uint32_t) pucTrailerAddress % BOOT_QUAD_WORD_SIZE) != 0)
            {
                pucTrailerAddress += BOOT_QUAD_WORD_SIZE - ( (uint32_t)pucTrailerAddress % BOOT_QUAD_WORD_SIZE );
            }

            pxImgTrailer = ( BOOTImageTrailer_t * ) pucTrailerAddress;

            if( pdTRUE == BOOT_CRYPTO_Verify( pucStartAddress,
                                              ulSizeApp,
                                              pxImgTrailer->aucSignature,
                                              pxImgTrailer->ulSignatureSize ) )
            {
                BOOT_LOG_L1( "[%s] Crypto signature is valid.\r\n", BOOT_METHOD_NAME );
                xReturn = pdTRUE;
            }
            else
            {
                BOOT_LOG_L1( "[%s] Crypto signature is not valid.\r\n", BOOT_METHOD_NAME );
                xReturn = pdFALSE;
            }
        }
    #else /* if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 ) */
        {
            BOOT_LOG_L1( "[%s] Crypto signature verification is disabled.\r\n", BOOT_METHOD_NAME );
        }
    #endif /* if ( bootconfigENABLE_CRYPTO_SIGNATURE_VERIFICATION == 1 ) */

    return xReturn;
}

/*-----------------------------------------------------------*/

static BaseType_t prvInvalidateImage( const BOOTImageDescriptor_t * pxAppDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "prvInvalidateImage" );

    BaseType_t xReturn = pdFALSE;
    
    /* Erase the header first. */
    xReturn = BOOT_FLASH_EraseHeader( pxAppDescriptor );

    #if ( bootconfigENABLE_ERASE_INVALID == 1 )
        {
            if( xReturn == pdTRUE )
            {
                /* Erase the entire bank. */
                xReturn = BOOT_FLASH_EraseBank( pxAppDescriptor );
            }
        }
    #else
        {
            BOOT_LOG_L1( "[%s] Full bank erase is disabled.\r\n", BOOT_METHOD_NAME );
        }
    #endif /* if ( bootconfigENABLE_ERASE_INVALID == 1 ) */

    return pdTRUE;
}
