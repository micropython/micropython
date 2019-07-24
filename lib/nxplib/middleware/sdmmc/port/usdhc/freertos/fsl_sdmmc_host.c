/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_event.h"
#include "fsl_sdmmc_host.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SDMMCHOST notify card insertion status.
 * @param inserted true is inserted, false is not
 * @param cd card detect descriptor
 */
static void SDMMCHOST_NofiyCardInsertStatus(bool inserted, const sdmmchost_detect_card_t *cd);

/*!
 * @brief SDMMCHOST detect card insert status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardInsertByHost(SDMMCHOST_TYPE *base, void *userData);

/*!
 * @brief SDMMCHOST detect card remove status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardRemoveByHost(SDMMCHOST_TYPE *base, void *userData);

/*!
 * @brief SDMMCHOST transfer function.
 * @param base host base address.
 * @param content transfer configurations.
 */
static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content);

/*!
 * @brief SDMMCHOST transfer complete callback.
 * @param base host base address.
 * @param handle host handle.
 * @param status interrupt status.
 * @param userData user data.
 */
static void SDMMCHOST_TransferCompleteCallback(SDMMCHOST_TYPE *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*!
 * @brief SDMMCHOST re-tuning callback
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_ReTuningCallback(SDMMCHOST_TYPE *base, void *userData);

/*!
 * @brief card detect deinit function.
 */
static void SDMMCHOST_CardDetectDeinit(void);

/*!
 * @brief card detect deinit function.
 * @param host base address.
 * @param host detect card configuration.
 */
static status_t SDMMCHOST_CardDetectInit(SDMMCHOST_TYPE *base, const sdmmchost_detect_card_t *cd);
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* DMA descriptor should allocate at non-cached memory */
AT_NONCACHEABLE_SECTION_ALIGN(uint32_t g_usdhcAdma2Table[USDHC_ADMA_TABLE_WORDS], USDHC_ADMA2_ADDR_ALIGN);

static usdhc_handle_t s_usdhcHandle;
static volatile status_t s_usdhcTransferStatus = kStatus_Success;
static volatile bool s_sdInsertedFlag          = false;
static volatile status_t s_reTuningFlag        = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_NofiyCardInsertStatus(bool inserted, const sdmmchost_detect_card_t *cd)
{
    if (inserted == false)
    {
        s_sdInsertedFlag = false;
        if (cd && (cd->cardRemoved))
        {
            cd->cardRemoved(false, cd->userData);
        }
    }
    else
    {
        s_sdInsertedFlag = true;
        if (cd && (cd->cardInserted))
        {
            cd->cardInserted(true, cd->userData);
        }
    }
}

static void SDMMCHOST_DetectCardInsertByHost(SDMMCHOST_TYPE *base, void *userData)
{
    s_sdInsertedFlag = true;
    SDMMCEVENT_Notify(kSDMMCEVENT_CardDetect);
    /* application callback */
    if (userData && (((sdmmhostcard_usr_param_t *)userData)->cd) &&
        ((sdmmhostcard_usr_param_t *)userData)->cd->cardInserted)
    {
        ((sdmmhostcard_usr_param_t *)userData)
            ->cd->cardInserted(true, ((sdmmhostcard_usr_param_t *)userData)->cd->userData);
    }
}

static void SDMMCHOST_DetectCardRemoveByHost(SDMMCHOST_TYPE *base, void *userData)
{
    s_sdInsertedFlag = false;
    /* application callback */
    if (userData && (((sdmmhostcard_usr_param_t *)userData)->cd) &&
        ((sdmmhostcard_usr_param_t *)userData)->cd->cardRemoved)
    {
        ((sdmmhostcard_usr_param_t *)userData)
            ->cd->cardRemoved(false, ((sdmmhostcard_usr_param_t *)userData)->cd->userData);
    }
}

static void SDMMCHOST_CardInterrupt(SDMMCHOST_TYPE *base, void *userData)
{
    /* application callback */
    if (userData && ((sdmmhostcard_usr_param_t *)userData)->cardInt)
    {
        ((sdmmhostcard_usr_param_t *)userData)
            ->cardInt->cardInterrupt(((sdmmhostcard_usr_param_t *)userData)->cardInt->userData);
    }
}

static void SDMMCHOST_TransferCompleteCallback(SDMMCHOST_TYPE *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData)
{
    /* wait the target status and then notify the transfer complete */
    s_usdhcTransferStatus = status;

    if (!((handle->data) && (handle->data->rxData) && (status == kStatus_USDHC_SendCommandFailed)))
    {
        SDMMCEVENT_Notify(kSDMMCEVENT_TransferComplete);
    }
}

static void SDMMCHOST_ReTuningCallback(SDMMCHOST_TYPE *base, void *userData)
{
    s_reTuningFlag = true;
    SDMMCEVENT_Notify(kSDMMCEVENT_TransferComplete);
}

static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content)
{
    status_t error = kStatus_Success;

    usdhc_adma_config_t dmaConfig;

    if (content->data != NULL)
    {
        memset(&dmaConfig, 0, sizeof(usdhc_adma_config_t));
        /* config adma */
        dmaConfig.dmaMode        = USDHC_DMA_MODE;
        dmaConfig.burstLen       = kUSDHC_EnBurstLenForINCR;
        dmaConfig.admaTable      = g_usdhcAdma2Table;
        dmaConfig.admaTableWords = USDHC_ADMA_TABLE_WORDS;
    }

    do
    {
        error = USDHC_TransferNonBlocking(base, &s_usdhcHandle, &dmaConfig, content);
    } while (error == kStatus_USDHC_BusyTransferring);

    if ((error != kStatus_Success) ||
        (false == SDMMCEVENT_Wait(kSDMMCEVENT_TransferComplete, SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT)) ||
        (s_reTuningFlag) || (s_usdhcTransferStatus != kStatus_Success))
    {
        if (s_reTuningFlag || (error == kStatus_USDHC_ReTuningRequest))
        {
            if (s_reTuningFlag)
            {
                s_reTuningFlag = false;
                error          = kStatus_USDHC_TuningError;
            }
        }
        else
        {
            error = s_usdhcTransferStatus;
            /* host error recovery */
            SDMMCHOST_ErrorRecovery(base);
        }
    }

    return error;
}

void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = USDHC_GetPresentStatusFlags(base);
    /* check command inhibit status flag */
    if ((status & kUSDHC_CommandInhibitFlag) != 0U)
    {
        /* reset command line */
        USDHC_Reset(base, kUSDHC_ResetCommand, 100U);
    }
    /* check data inhibit status flag */
    if ((status & kUSDHC_DataInhibitFlag) != 0U)
    {
        /* reset data line */
        USDHC_Reset(base, kUSDHC_ResetData, 100U);
    }
}

static status_t SDMMCHOST_CardDetectInit(SDMMCHOST_TYPE *base, const sdmmchost_detect_card_t *cd)
{
    sdmmchost_detect_card_type_t cdType = kSDMMCHOST_DetectCardByGpioCD;
    bool cardInserted                   = false;

    if (cd != NULL)
    {
        cdType = cd->cdType;
    }

    if (!SDMMCEVENT_Create(kSDMMCEVENT_CardDetect))
    {
        return kStatus_Fail;
    }

    if (cdType == kSDMMCHOST_DetectCardByGpioCD)
    {
        SDMMCHOST_CARD_DETECT_GPIO_INIT();

/* set IRQ priority */
#if defined(__CORTEX_M)
        SDMMCHOST_SET_IRQ_PRIORITY(SDMMCHOST_CARD_DETECT_GPIO_IRQ, 6U);
#else
        SDMMCHOST_SET_IRQ_PRIORITY(SDMMCHOST_CARD_DETECT_GPIO_IRQ, 26U);
#endif
        /* Open card detection pin NVIC. */
        SDMMCHOST_ENABLE_IRQ(SDMMCHOST_CARD_DETECT_GPIO_IRQ);
        /* detect card insert status */
        if (SDMMCHOST_CARD_DETECT_GPIO_STATUS() == SDMMCHOST_CARD_INSERT_CD_LEVEL)
        {
            cardInserted = true;
        }
    }
    else
    {
        /* enable card detect through DATA3 */
        if (cdType == kSDMMCHOST_DetectCardByHostDATA3)
        {
            SDMMCHOST_CARD_DETECT_DATA3_ENABLE(base, true);
        }
        /* enable card detect interrupt */
        SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base);
        SDMMCHOST_CARD_DETECT_REMOVE_ENABLE(base);
        SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_ENABLE(base);
        SDMMCHOST_CARD_DETECT_REMOVE_INTERRUPT_ENABLE(base);
        /* check if card is inserted */
        if (SDMMCHOST_CARD_DETECT_INSERT_STATUS(base))
        {
            cardInserted = true;
        }
    }

    /* notify application about the card insertion status */
    SDMMCHOST_NofiyCardInsertStatus(cardInserted, cd);

    return kStatus_Success;
}

static void SDMMCHOST_CardDetectDeinit(void)
{
    SDMMCEVENT_Delete(kSDMMCEVENT_CardDetect);
    s_sdInsertedFlag = false;
}

void SDMMCHOST_Delay(uint32_t milliseconds)
{
    SDMMCEVENT_Delay(milliseconds);
}

void SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER(void)
{
    if (SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_STATUS() & (1U << BOARD_USDHC_CD_GPIO_PIN))
    {
        SDMMCHOST_NofiyCardInsertStatus((SDMMCHOST_CARD_DETECT_GPIO_STATUS() == SDMMCHOST_CARD_INSERT_CD_LEVEL),
                                        ((sdmmhostcard_usr_param_t *)s_usdhcHandle.userData)->cd);
    }
    /* Clear interrupt flag.*/
    SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_STATUS_CLEAR(~0U);
    SDMMCEVENT_Notify(kSDMMCEVENT_CardDetect);
}

status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE *base, const sdmmchost_detect_card_t *cd, bool waitCardStatus)
{
    uint32_t timeout = SDMMCHOST_CARD_DETECT_TIMEOUT;

    if (cd != NULL)
    {
        timeout = cd->cdTimeOut_ms;
    }

    if (waitCardStatus != s_sdInsertedFlag)
    {
        /* Wait card inserted. */
        do
        {
            if (!SDMMCEVENT_Wait(kSDMMCEVENT_CardDetect, timeout))
            {
                return kStatus_Fail;
            }
        } while (waitCardStatus != s_sdInsertedFlag);
    }

    return kStatus_Success;
}

bool SDMMCHOST_IsCardPresent(void)
{
    return s_sdInsertedFlag;
}

void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    if (pwr != NULL)
    {
        pwr->powerOff();
        SDMMCHOST_Delay(pwr->powerOffDelay_ms);
    }
    else
    {
        /* only SD card need card detect*/
        SDMMCHOST_ENABLE_SD_POWER(false);
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(500U);
    }
}

void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    /* use user define the power on function  */
    if (pwr != NULL)
    {
        pwr->powerOn();
        SDMMCHOST_Delay(pwr->powerOnDelay_ms);
    }
    else
    {
        /* card power on */
        SDMMCHOST_ENABLE_SD_POWER(true);
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(500U);
    }
}

status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *userData)
{
    usdhc_host_t *usdhcHost            = (usdhc_host_t *)host;
    usdhc_transfer_callback_t callback = {
        .TransferComplete = SDMMCHOST_TransferCompleteCallback,
        .ReTuning         = SDMMCHOST_ReTuningCallback,
        .CardInserted     = SDMMCHOST_DetectCardInsertByHost,
        .CardRemoved      = SDMMCHOST_DetectCardRemoveByHost,
        .SdioInterrupt    = SDMMCHOST_CardInterrupt,
        .BlockGap         = NULL,
    };
    /* init card power control */
    SDMMCHOST_INIT_SD_POWER();
    SDMMCHOST_INIT_MMC_POWER();

    /* Initializes USDHC. */
    usdhcHost->config.dataTimeout         = USDHC_DATA_TIMEOUT;
    usdhcHost->config.endianMode          = USDHC_ENDIAN_MODE;
    usdhcHost->config.readWatermarkLevel  = USDHC_READ_WATERMARK_LEVEL;
    usdhcHost->config.writeWatermarkLevel = USDHC_WRITE_WATERMARK_LEVEL;
    usdhcHost->config.readBurstLen        = USDHC_READ_BURST_LEN;
    usdhcHost->config.writeBurstLen       = USDHC_WRITE_BURST_LEN;

    USDHC_Init(usdhcHost->base, &(usdhcHost->config));

    /* disable the card insert/remove interrupt, due to use GPIO interrupt detect card */
    USDHC_DisableInterruptSignal(usdhcHost->base, kUSDHC_CardRemovalFlag | kUSDHC_CardInsertionFlag);
    /* create interrupt handler */
    USDHC_TransferCreateHandle(usdhcHost->base, &s_usdhcHandle, &callback, userData);

    if (false == SDMMCEVENT_Create(kSDMMCEVENT_TransferComplete))
    {
        return kStatus_Fail;
    }

    /* Define transfer function. */
    usdhcHost->transfer = SDMMCHOST_TransferFunction;
    /* card detect init */
    SDMMCHOST_CardDetectInit(usdhcHost->base, (userData == NULL) ? NULL : (((sdmmhostcard_usr_param_t *)userData)->cd));

    return kStatus_Success;
}

void SDMMCHOST_Reset(SDMMCHOST_TYPE *base)
{
    /* voltage switch to normal but not 1.8V */
    SDMMCHOST_SWITCH_VOLTAGE180V(base, false);
    /* Disable DDR mode */
    SDMMCHOST_ENABLE_DDR_MODE(base, false, 0U);
    /* disable tuning */
    SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, false);
    /* Disable HS400 mode */
    SDMMCHOST_ENABLE_HS400_MODE(base, false);
    /* Disable DLL */
    SDMMCHOST_ENABLE_STROBE_DLL(base, false);
}

void SDMMCHOST_Deinit(void *host)
{
    usdhc_host_t *usdhcHost = (usdhc_host_t *)host;
    SDMMCHOST_Reset(usdhcHost->base);
    USDHC_Deinit(usdhcHost->base);
    SDMMCEVENT_Delete(kSDMMCEVENT_TransferComplete);
    SDMMCHOST_CardDetectDeinit();
}
