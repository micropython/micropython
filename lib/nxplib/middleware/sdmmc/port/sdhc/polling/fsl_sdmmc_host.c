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
 * @brief SDMMCHOST transfer function.
 * @param base host base address.
 * @param content transfer configurations.
 */
static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content);

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
static uint32_t s_sdhcAdmaTable[SDHC_ADMA_TABLE_WORDS];
/*! @brief Card detect flag. */
static volatile bool s_sdInsertedFlag = false;
extern volatile uint32_t s_timeMilliseconds;
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

static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER *content)
{
    status_t error = kStatus_Success;

    if (kStatus_Success != SDHC_TransferBlocking(base, s_sdhcAdmaTable, SDHC_ADMA_TABLE_WORDS, content))
    {
        error = kStatus_Fail;
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(base);
    }

    return error;
}

void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE *base)
{
    uint32_t status = false;
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
    bool cardInsert                     = true;

    if (cd != NULL)
    {
        cdType = cd->cdType;
    }

    if (cdType == kSDMMCHOST_DetectCardByGpioCD)
    {
        if (GPIO_PinRead(BOARD_SDHC_CD_GPIO_BASE, BOARD_SDHC_CD_GPIO_PIN) == SDMMCHOST_CARD_INSERT_CD_LEVEL)
        {
            cardInsert = true;
        }
    }
    else if (cdType == kSDMMCHOST_DetectCardByHostDATA3)
    {
        /* enable card detect through DATA3 */
        SDMMCHOST_CARD_DETECT_DATA3_ENABLE(base, true);
        /* enable card detect status */
        SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base);
        if (SDMMCHOST_CARD_DETECT_INSERT_STATUS(base))
        {
            s_sdInsertedFlag = true;
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
    s_sdInsertedFlag = false;
}

void SDMMCHOST_Delay(uint32_t milliseconds)
{
    SDMMCEVENT_Delay(milliseconds);
}

status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE *base, const sdmmchost_detect_card_t *cd, bool waitCardStatus)
{
    sdmmchost_detect_card_type_t cdType = kSDMMCHOST_DetectCardByGpioCD;

    if (cd != NULL)
    {
        cdType = cd->cdType;
    }

    if (waitCardStatus != s_sdInsertedFlag)
    {
        /* Wait card inserted. */
        do
        {
            if (cdType != kSDMMCHOST_DetectCardByGpioCD)
            {
                if (SDMMCHOST_CARD_DETECT_INSERT_STATUS(base))
                {
                    s_sdInsertedFlag = true;
                }
            }
            else
            {
                if (GPIO_PinRead(BOARD_SDHC_CD_GPIO_BASE, BOARD_SDHC_CD_GPIO_PIN) == SDMMCHOST_CARD_INSERT_CD_LEVEL)
                {
                    s_sdInsertedFlag = true;
                }
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
    sdhc_host_t *sdhcHost = (sdhc_host_t *)host;

    /* init event timer */
    SDMMCEVENT_InitTimer();

    /* Initializes SDHC. */
    sdhcHost->config.cardDetectDat3      = false;
    sdhcHost->config.endianMode          = SDHC_ENDIAN_MODE;
    sdhcHost->config.dmaMode             = SDHC_DMA_MODE;
    sdhcHost->config.readWatermarkLevel  = SDHC_READ_WATERMARK_LEVEL;
    sdhcHost->config.writeWatermarkLevel = SDHC_WRITE_WATERMARK_LEVEL;
    SDHC_Init(sdhcHost->base, &(sdhcHost->config));

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
}
