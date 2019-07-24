/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_event.h"
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
static void SDMMCHOST_NotifyCardInsertStatus(bool inserted, const sdmmchost_detect_card_t *cd);

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
                                               sdhc_handle_t *handle,
                                               status_t status,
                                               void *userData);

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
sdhc_handle_t s_sdhcHandle;
static uint32_t s_sdhcAdmaTable[SDHC_ADMA_TABLE_WORDS];
volatile bool g_sdhcTransferSuccessFlag = true;
/*! @brief Card detect flag. */
static volatile bool s_sdInsertedFlag = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_NotifyCardInsertStatus(bool inserted, const sdmmchost_detect_card_t *cd)
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
    SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_DISABLE(base);
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

static void SDMMCHOST_TransferCompleteCallback(SDMMCHOST_TYPE *base,
                                               sdhc_handle_t *handle,
                                               status_t status,
                                               void *userData)
{
    if (status == kStatus_Success)
    {
        g_sdhcTransferSuccessFlag = true;
    }
    else
    {
        g_sdhcTransferSuccessFlag = false;
    }

    SDMMCEVENT_Notify(kSDMMCEVENT_TransferComplete);
}

static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content)
{
    status_t error = kStatus_Success;

    /* make sure transfer complete event is cleared */
    SDMMCEVENT_Delete(kSDMMCEVENT_TransferComplete);

    do
    {
        error = SDHC_TransferNonBlocking(base, &s_sdhcHandle, s_sdhcAdmaTable, SDHC_ADMA_TABLE_WORDS, content);
    } while (error == kStatus_SDHC_BusyTransferring);

    if ((error != kStatus_Success) ||
        (false == SDMMCEVENT_Wait(kSDMMCEVENT_TransferComplete, SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT)) ||
        (!g_sdhcTransferSuccessFlag))
    {
        error = kStatus_Fail;
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(base);
    }

    return error;
}

void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = SDHC_GetPresentStatusFlags(base);
    /* check command inhibit status flag */
    if ((status & kSDHC_CommandInhibitFlag) != 0U)
    {
        /* reset command line */
        SDHC_Reset(base, kSDHC_ResetCommand, 100U);
    }
    /* check data inhibit status flag */
    if ((status & kSDHC_DataInhibitFlag) != 0U)
    {
        /* reset data line */
        SDHC_Reset(base, kSDHC_ResetData, 100U);
    }
}

static status_t SDMMCHOST_CardDetectInit(SDMMCHOST_TYPE *base, const sdmmchost_detect_card_t *cd)
{
    sdmmchost_detect_card_type_t cdType = kSDMMCHOST_DetectCardByGpioCD;
    bool cardInsert                     = false;

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
        /* Card detection pin will generate interrupt on either eage */
        PORT_SetPinInterruptConfig(BOARD_SDHC_CD_PORT_BASE, BOARD_SDHC_CD_GPIO_PIN, kPORT_InterruptEitherEdge);
        /* set IRQ priority */
        SDMMCHOST_SET_IRQ_PRIORITY(SDMMCHOST_CARD_DETECT_IRQ, 6U);
        /* Open card detection pin NVIC. */
        SDMMCHOST_ENABLE_IRQ(SDMMCHOST_CARD_DETECT_IRQ);
        /* check card detect status */
        if (GPIO_PinRead(BOARD_SDHC_CD_GPIO_BASE, BOARD_SDHC_CD_GPIO_PIN) == SDMMCHOST_CARD_INSERT_CD_LEVEL)
        {
            cardInsert = true;
        }
    }
    else if (cdType == kSDMMCHOST_DetectCardByHostDATA3)
    {
        /* enable card detect through DATA3 */
        SDMMCHOST_CARD_DETECT_DATA3_ENABLE(base, true);
        /* enable card detect interrupt */
        SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base);
        SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_ENABLE(base);

        if (SDMMCHOST_CARD_DETECT_INSERT_STATUS(base))
        {
            cardInsert = true;
        }
    }
    else
    {
        /* SDHC do not support detect card through CD */
        return kStatus_Fail;
    }

    /* notify application about the card insertion status */
    SDMMCHOST_NotifyCardInsertStatus(cardInsert, cd);

    return kStatus_Success;
}

static void SDMMCHOST_CardDetectDeinit(void)
{
    SDMMCEVENT_Delete(kSDMMCEVENT_CardDetect);
}

void SDMMCHOST_Delay(uint32_t milliseconds)
{
    SDMMCEVENT_Delay(milliseconds);
}

void SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER(void)
{
    if (PORT_GetPinsInterruptFlags(BOARD_SDHC_CD_PORT_BASE) == (1U << BOARD_SDHC_CD_GPIO_PIN))
    {
        SDMMCHOST_NotifyCardInsertStatus(
            GPIO_PinRead(BOARD_SDHC_CD_GPIO_BASE, BOARD_SDHC_CD_GPIO_PIN) == SDMMCHOST_CARD_INSERT_CD_LEVEL,
            ((sdmmhostcard_usr_param_t *)s_sdhcHandle.userData)->cd);
    }
    /* Clear interrupt flag.*/
    PORT_ClearPinsInterruptFlags(BOARD_SDHC_CD_PORT_BASE, ~0U);
    SDMMCEVENT_Notify(kSDMMCEVENT_CardDetect);
}

status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase,
                                        const sdmmchost_detect_card_t *cd,
                                        bool waitCardStatus)
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
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(1000U);
    }
}

status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *userData)
{
    sdhc_transfer_callback_t sdhcCallback = {0};
    sdhc_host_t *sdhcHost                 = (sdhc_host_t *)host;
    /* init event timer */
    SDMMCEVENT_InitTimer();

    /* Initializes SDHC. */
    sdhcHost->config.cardDetectDat3      = false;
    sdhcHost->config.endianMode          = SDHC_ENDIAN_MODE;
    sdhcHost->config.dmaMode             = SDHC_DMA_MODE;
    sdhcHost->config.readWatermarkLevel  = SDHC_READ_WATERMARK_LEVEL;
    sdhcHost->config.writeWatermarkLevel = SDHC_WRITE_WATERMARK_LEVEL;
    SDHC_Init(sdhcHost->base, &(sdhcHost->config));

    /* Create handle for SDHC driver */
    sdhcCallback.TransferComplete = SDMMCHOST_TransferCompleteCallback;
    sdhcCallback.CardInserted     = SDMMCHOST_DetectCardInsertByHost;
    sdhcCallback.CardRemoved      = SDMMCHOST_DetectCardRemoveByHost;
    SDHC_TransferCreateHandle(sdhcHost->base, &s_sdhcHandle, &sdhcCallback, userData);

    /* Create transfer complete event. */
    if (false == SDMMCEVENT_Create(kSDMMCEVENT_TransferComplete))
    {
        return kStatus_Fail;
    }

    /* Define transfer function. */
    sdhcHost->transfer = SDMMCHOST_TransferFunction;
    /* card detect init */
    SDMMCHOST_CardDetectInit(sdhcHost->base, (userData == NULL) ? NULL : (((sdmmhostcard_usr_param_t *)userData)->cd));

    return kStatus_Success;
}

void SDMMCHOST_Reset(SDMMCHOST_TYPE *base)
{
    /* reserved for future */
}

void SDMMCHOST_Deinit(void *host)
{
    sdhc_host_t *sdhcHost = (sdhc_host_t *)host;
    SDHC_Deinit(sdhcHost->base);
    SDMMCHOST_CardDetectDeinit();
    SDMMCEVENT_Delete(kSDMMCEVENT_TransferComplete);
}
