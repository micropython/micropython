/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sai.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.sai"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/
enum _sai_transfer_state
{
    kSAI_Busy = 0x0U, /*!< SAI is busy */
    kSAI_Idle,        /*!< Transfer is done. */
    kSAI_Error        /*!< Transfer error occurred. */
};

/*! @brief Typedef for sai tx interrupt handler. */
typedef void (*sai_tx_isr_t)(I2S_Type *base, sai_handle_t *saiHandle);

/*! @brief Typedef for sai rx interrupt handler. */
typedef void (*sai_rx_isr_t)(I2S_Type *base, sai_handle_t *saiHandle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if ((defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)) || \
     (defined(FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV) && (FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV)))

/*!
 * @brief Set the master clock divider.
 *
 * This API will compute the master clock divider according to master clock frequency and master
 * clock source clock source frequency.
 *
 * @param base SAI base pointer.
 * @param mclk_Hz Mater clock frequency in Hz.
 * @param mclkSrcClock_Hz Master clock source frequency in Hz.
 */
static void SAI_SetMasterClockDivider(I2S_Type *base, uint32_t mclk_Hz, uint32_t mclkSrcClock_Hz);
#endif /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */

/*!
 * @brief Get the instance number for SAI.
 *
 * @param base SAI base pointer.
 */
static uint32_t SAI_GetInstance(I2S_Type *base);

/*!
 * @brief sends a piece of data in non-blocking way.
 *
 * @param base SAI base pointer
 * @param channel start channel number.
 * @param channelMask enabled channels mask.
 * @param endChannel end channel numbers.
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be written.
 * @param size Bytes to be written.
 */
static void SAI_WriteNonBlocking(I2S_Type *base,
                                 uint32_t channel,
                                 uint32_t channelMask,
                                 uint32_t endChannel,
                                 uint32_t bitWidth,
                                 uint8_t *buffer,
                                 uint32_t size);

/*!
 * @brief Receive a piece of data in non-blocking way.
 *
 * @param base SAI base pointer
 * @param channel start channel number.
 * @param channelMask enabled channels mask.
 * @param endChannel end channel numbers.
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be read.
 * @param size Bytes to be read.
 */
static void SAI_ReadNonBlocking(I2S_Type *base,
                                uint32_t channel,
                                uint32_t channelMask,
                                uint32_t endChannel,
                                uint32_t bitWidth,
                                uint8_t *buffer,
                                uint32_t size);

/*!
 * @brief Get classic I2S mode configurations.
 *
 * @param config transceiver configurations
 * @param bitWidth audio data bitWidth.
 * @param mode audio data channel
 * @param saiChannelMask channel mask value to enable
 */
static void SAI_GetCommonConfig(sai_transceiver_t *config,
                                sai_word_width_t bitWidth,
                                sai_mono_stereo_t mode,
                                uint32_t saiChannelMask);
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Base pointer array */
static I2S_Type *const s_saiBases[] = I2S_BASE_PTRS;
/*!@brief SAI handle pointer */
static sai_handle_t *s_saiHandle[ARRAY_SIZE(s_saiBases)][2];
/* IRQ number array */
static const IRQn_Type s_saiTxIRQ[] = I2S_TX_IRQS;
static const IRQn_Type s_saiRxIRQ[] = I2S_RX_IRQS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Clock name array */
static const clock_ip_name_t s_saiClock[] = SAI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
/*! @brief Pointer to tx IRQ handler for each instance. */
static sai_tx_isr_t s_saiTxIsr;
/*! @brief Pointer to tx IRQ handler for each instance. */
static sai_rx_isr_t s_saiRxIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
#if ((defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)) || \
     (defined(FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV) && (FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV)))
static void SAI_SetMasterClockDivider(I2S_Type *base, uint32_t mclk_Hz, uint32_t mclkSrcClock_Hz)
{
    assert(mclk_Hz <= mclkSrcClock_Hz);

    uint32_t sourceFreq = mclkSrcClock_Hz / 100U; /*In order to prevent overflow */
    uint32_t targetFreq = mclk_Hz / 100U;         /*In order to prevent overflow */

#if FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV
    uint32_t postDivider = sourceFreq / targetFreq;

    /* if source equal to target, then disable divider */
    if (postDivider == 1U)
    {
        base->MCR &= ~I2S_MCR_DIVEN_MASK;
    }
    else
    {
        base->MCR = (base->MCR & (~I2S_MCR_DIV_MASK)) | I2S_MCR_DIV(postDivider / 2U - 1U) | I2S_MCR_DIVEN_MASK;
    }
#endif
#if FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER
    uint16_t fract, divide;
    uint32_t remaind           = 0;
    uint32_t current_remainder = 0xFFFFFFFFU;
    uint16_t current_fract     = 0;
    uint16_t current_divide    = 0;
    uint32_t mul_freq          = 0;
    uint32_t max_fract         = 256;

    /* Compute the max fract number */
    max_fract = targetFreq * 4096 / sourceFreq + 1;
    if (max_fract > 256)
    {
        max_fract = 256;
    }

    /* Looking for the closet frequency */
    for (fract = 1; fract < max_fract; fract++)
    {
        mul_freq = sourceFreq * fract;
        remaind  = mul_freq % targetFreq;
        divide   = mul_freq / targetFreq;

        /* Find the exactly frequency */
        if (remaind == 0)
        {
            current_fract  = fract;
            current_divide = mul_freq / targetFreq;
            break;
        }

        /* Closer to next one, set the closest to next data */
        if (remaind > mclk_Hz / 2)
        {
            remaind = targetFreq - remaind;
            divide += 1;
        }

        /* Update the closest div and fract */
        if (remaind < current_remainder)
        {
            current_fract     = fract;
            current_divide    = divide;
            current_remainder = remaind;
        }
    }

    /* Fill the computed fract and divider to registers */
    base->MDR = I2S_MDR_DIVIDE(current_divide - 1) | I2S_MDR_FRACT(current_fract - 1);

    /* Waiting for the divider updated */
    while (base->MCR & I2S_MCR_DUF_MASK)
    {
    }
#endif
}
#endif /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */

static uint32_t SAI_GetInstance(I2S_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_saiBases); instance++)
    {
        if (s_saiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_saiBases));

    return instance;
}

static void SAI_WriteNonBlocking(I2S_Type *base,
                                 uint32_t channel,
                                 uint32_t channelMask,
                                 uint32_t endChannel,
                                 uint32_t bitWidth,
                                 uint8_t *buffer,
                                 uint32_t size)
{
    uint32_t i = 0;
    uint8_t j = 0, m = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t data        = 0;
    uint32_t temp        = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        for (j = channel; j <= endChannel; j++)
        {
            if ((1U << j) & channelMask)
            {
                for (m = 0; m < bytesPerWord; m++)
                {
                    temp = (uint32_t)(*buffer);
                    data |= (temp << (8U * m));
                    buffer++;
                }
                base->TDR[j] = data;
                data         = 0;
            }
        }
    }
}

static void SAI_ReadNonBlocking(I2S_Type *base,
                                uint32_t channel,
                                uint32_t channelMask,
                                uint32_t endChannel,
                                uint32_t bitWidth,
                                uint8_t *buffer,
                                uint32_t size)
{
    uint32_t i = 0;
    uint8_t j = 0, m = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t data        = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        for (j = channel; j <= endChannel; j++)
        {
            if ((1U << j) & channelMask)
            {
                data = base->RDR[j];
                for (m = 0; m < bytesPerWord; m++)
                {
                    *buffer = (data >> (8U * m)) & 0xFF;
                    buffer++;
                }
            }
        }
    }
}

static void SAI_GetCommonConfig(sai_transceiver_t *config,
                                sai_word_width_t bitWidth,
                                sai_mono_stereo_t mode,
                                uint32_t saiChannelMask)
{
    assert(NULL != config);
    assert(saiChannelMask != 0U);

    memset(config, 0U, sizeof(sai_transceiver_t));

    config->channelMask = saiChannelMask;
    /* sync mode default configurations */
    config->syncMode = kSAI_ModeAsync;

    /* master mode default */
    config->masterSlave = kSAI_Master;

    /* bit default configurations */
    config->bitClock.bclkSrcSwap    = false;
    config->bitClock.bclkInputDelay = false;
    config->bitClock.bclkPolarity   = kSAI_SampleOnRisingEdge;
    config->bitClock.bclkSource     = kSAI_BclkSourceMclkDiv;

    /* frame sync default configurations */
    config->frameSync.frameSyncWidth = bitWidth;
    config->frameSync.frameSyncEarly = true;
#if defined(FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND) && FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND
    config->frameSync.frameSyncGenerateOnDemand = false;
#endif
    config->frameSync.frameSyncPolarity = kSAI_PolarityActiveLow;

    /* serial data default configurations */
#if defined(FSL_FEATURE_SAI_HAS_CHANNEL_MODE) && FSL_FEATURE_SAI_HAS_CHANNEL_MODE
    config->serialData.dataMode = kSAI_DataPinStateOutputZero;
#endif
    config->serialData.dataOrder           = kSAI_DataMSB;
    config->serialData.dataWord0Length     = bitWidth;
    config->serialData.dataWordLength      = bitWidth;
    config->serialData.dataWordNLength     = bitWidth;
    config->serialData.dataFirstBitShifted = bitWidth;
    config->serialData.dataWordNum         = 2U;
    config->serialData.dataMaskedWord      = mode;

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* fifo configurations */
    config->fifo.fifoWatermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    config->fifo.fifoContinueOneError = true;
#endif
}

/*!
 * brief Initializes the SAI Tx peripheral.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_Init
 *
 * Ungates the SAI clock, resets the module, and configures SAI Tx with a configuration structure.
 * The configuration structure can be custom filled or set with default values by
 * SAI_TxGetDefaultConfig().
 *
 * note  This API should be called at the beginning of the application to use
 * the SAI driver. Otherwise, accessing the SAIM module can cause a hard fault
 * because the clock is not enabled.
 *
 * param base SAI base pointer
 * param config SAI configuration structure.
 */
void SAI_TxInit(I2S_Type *base, const sai_config_t *config)
{
    uint32_t val = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the SAI clock */
    CLOCK_EnableClock(s_saiClock[SAI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    /* Master clock source setting */
    val       = (base->MCR & ~I2S_MCR_MICS_MASK);
    base->MCR = (val | I2S_MCR_MICS(config->mclkSource));
#endif

    /* Configure Master clock output enable */
    val       = (base->MCR & ~I2S_MCR_MOE_MASK);
    base->MCR = (val | I2S_MCR_MOE(config->mclkOutputEnable));
#endif /* FSL_FEATURE_SAI_HAS_MCR */

    SAI_TxReset(base);

    /* Configure audio protocol */
    switch (config->protocol)
    {
        case kSAI_BusLeftJustified:
            base->TCR2 |= I2S_TCR2_BCP_MASK;
            base->TCR3 &= ~I2S_TCR3_WDFL_MASK;
            base->TCR4 = I2S_TCR4_MF(1U) | I2S_TCR4_SYWD(31U) | I2S_TCR4_FSE(0U) | I2S_TCR4_FSP(0U) | I2S_TCR4_FRSZ(1U);
            break;

        case kSAI_BusRightJustified:
            base->TCR2 |= I2S_TCR2_BCP_MASK;
            base->TCR3 &= ~I2S_TCR3_WDFL_MASK;
            base->TCR4 = I2S_TCR4_MF(1U) | I2S_TCR4_SYWD(31U) | I2S_TCR4_FSE(0U) | I2S_TCR4_FSP(0U) | I2S_TCR4_FRSZ(1U);
            break;

        case kSAI_BusI2S:
            base->TCR2 |= I2S_TCR2_BCP_MASK;
            base->TCR3 &= ~I2S_TCR3_WDFL_MASK;
            base->TCR4 = I2S_TCR4_MF(1U) | I2S_TCR4_SYWD(31U) | I2S_TCR4_FSE(1U) | I2S_TCR4_FSP(1U) | I2S_TCR4_FRSZ(1U);
            break;

        case kSAI_BusPCMA:
            base->TCR2 &= ~I2S_TCR2_BCP_MASK;
            base->TCR3 &= ~I2S_TCR3_WDFL_MASK;
            base->TCR4 = I2S_TCR4_MF(1U) | I2S_TCR4_SYWD(0U) | I2S_TCR4_FSE(1U) | I2S_TCR4_FSP(0U) | I2S_TCR4_FRSZ(1U);
            break;

        case kSAI_BusPCMB:
            base->TCR2 &= ~I2S_TCR2_BCP_MASK;
            base->TCR3 &= ~I2S_TCR3_WDFL_MASK;
            base->TCR4 = I2S_TCR4_MF(1U) | I2S_TCR4_SYWD(0U) | I2S_TCR4_FSE(0U) | I2S_TCR4_FSP(0U) | I2S_TCR4_FRSZ(1U);
            break;

        default:
            break;
    }

    /* Set master or slave */
    if (config->masterSlave == kSAI_Master)
    {
        base->TCR2 |= I2S_TCR2_BCD_MASK;
        base->TCR4 |= I2S_TCR4_FSD_MASK;

        /* Bit clock source setting */
        val        = base->TCR2 & (~I2S_TCR2_MSEL_MASK);
        base->TCR2 = (val | I2S_TCR2_MSEL(config->bclkSource));
    }
    else
    {
        base->TCR2 &= ~I2S_TCR2_BCD_MASK;
        base->TCR4 &= ~I2S_TCR4_FSD_MASK;
    }

    /* Set Sync mode */
    switch (config->syncMode)
    {
        case kSAI_ModeAsync:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(0U));
            break;
        case kSAI_ModeSync:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(1U));
            /* If sync with Rx, should set Rx to async mode */
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(0U));
            break;
#if defined(FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI) && (FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI)
        case kSAI_ModeSyncWithOtherTx:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(2U));
            break;
        case kSAI_ModeSyncWithOtherRx:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(3U));
            break;
#endif /* FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI */
        default:
            break;
    }

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    SAI_TxSetFIFOErrorContinue(base, true);
#endif /* FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR */
}

/*!
 * brief Initializes the SAI Rx peripheral.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_Init
 *
 * Ungates the SAI clock, resets the module, and configures the SAI Rx with a configuration structure.
 * The configuration structure can be custom filled or set with default values by
 * SAI_RxGetDefaultConfig().
 *
 * note  This API should be called at the beginning of the application to use
 * the SAI driver. Otherwise, accessing the SAI module can cause a hard fault
 * because the clock is not enabled.
 *
 * param base SAI base pointer
 * param config SAI configuration structure.
 */
void SAI_RxInit(I2S_Type *base, const sai_config_t *config)
{
    uint32_t val = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable SAI clock first. */
    CLOCK_EnableClock(s_saiClock[SAI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    /* Master clock source setting */
    val       = (base->MCR & ~I2S_MCR_MICS_MASK);
    base->MCR = (val | I2S_MCR_MICS(config->mclkSource));
#endif

    /* Configure Master clock output enable */
    val       = (base->MCR & ~I2S_MCR_MOE_MASK);
    base->MCR = (val | I2S_MCR_MOE(config->mclkOutputEnable));
#endif /* FSL_FEATURE_SAI_HAS_MCR */

    SAI_RxReset(base);

    /* Configure audio protocol */
    switch (config->protocol)
    {
        case kSAI_BusLeftJustified:
            base->RCR2 |= I2S_RCR2_BCP_MASK;
            base->RCR3 &= ~I2S_RCR3_WDFL_MASK;
            base->RCR4 = I2S_RCR4_MF(1U) | I2S_RCR4_SYWD(31U) | I2S_RCR4_FSE(0U) | I2S_RCR4_FSP(0U) | I2S_RCR4_FRSZ(1U);
            break;

        case kSAI_BusRightJustified:
            base->RCR2 |= I2S_RCR2_BCP_MASK;
            base->RCR3 &= ~I2S_RCR3_WDFL_MASK;
            base->RCR4 = I2S_RCR4_MF(1U) | I2S_RCR4_SYWD(31U) | I2S_RCR4_FSE(0U) | I2S_RCR4_FSP(0U) | I2S_RCR4_FRSZ(1U);
            break;

        case kSAI_BusI2S:
            base->RCR2 |= I2S_RCR2_BCP_MASK;
            base->RCR3 &= ~I2S_RCR3_WDFL_MASK;
            base->RCR4 = I2S_RCR4_MF(1U) | I2S_RCR4_SYWD(31U) | I2S_RCR4_FSE(1U) | I2S_RCR4_FSP(1U) | I2S_RCR4_FRSZ(1U);
            break;

        case kSAI_BusPCMA:
            base->RCR2 &= ~I2S_RCR2_BCP_MASK;
            base->RCR3 &= ~I2S_RCR3_WDFL_MASK;
            base->RCR4 = I2S_RCR4_MF(1U) | I2S_RCR4_SYWD(0U) | I2S_RCR4_FSE(1U) | I2S_RCR4_FSP(0U) | I2S_RCR4_FRSZ(1U);
            break;

        case kSAI_BusPCMB:
            base->RCR2 &= ~I2S_RCR2_BCP_MASK;
            base->RCR3 &= ~I2S_RCR3_WDFL_MASK;
            base->RCR4 = I2S_RCR4_MF(1U) | I2S_RCR4_SYWD(0U) | I2S_RCR4_FSE(0U) | I2S_RCR4_FSP(0U) | I2S_RCR4_FRSZ(1U);
            break;

        default:
            break;
    }

    /* Set master or slave */
    if (config->masterSlave == kSAI_Master)
    {
        base->RCR2 |= I2S_RCR2_BCD_MASK;
        base->RCR4 |= I2S_RCR4_FSD_MASK;

        /* Bit clock source setting */
        val        = base->RCR2 & (~I2S_RCR2_MSEL_MASK);
        base->RCR2 = (val | I2S_RCR2_MSEL(config->bclkSource));
    }
    else
    {
        base->RCR2 &= ~I2S_RCR2_BCD_MASK;
        base->RCR4 &= ~I2S_RCR4_FSD_MASK;
    }

    /* Set Sync mode */
    switch (config->syncMode)
    {
        case kSAI_ModeAsync:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(0U));
            break;
        case kSAI_ModeSync:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(1U));
            /* If sync with Tx, should set Tx to async mode */
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(0U));
            break;
#if defined(FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI) && (FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI)
        case kSAI_ModeSyncWithOtherTx:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(2U));
            break;
        case kSAI_ModeSyncWithOtherRx:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(3U));
            break;
#endif /* FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI */
        default:
            break;
    }

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    SAI_RxSetFIFOErrorContinue(base, true);
#endif /* FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR */
}

/*!
 * brief Initializes the SAI peripheral.
 *
 * This API gates the SAI clock. The SAI module can't operate unless SAI_Init is called to enable the clock.
 *
 * param base SAI base pointer
 */
void SAI_Init(I2S_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the SAI clock */
    CLOCK_EnableClock(s_saiClock[SAI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief De-initializes the SAI peripheral.
 *
 * This API gates the SAI clock. The SAI module can't operate unless SAI_TxInit
 * or SAI_RxInit is called to enable the clock.
 *
 * param base SAI base pointer
 */
void SAI_Deinit(I2S_Type *base)
{
    SAI_TxEnable(base, false);
    SAI_RxEnable(base, false);
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(s_saiClock[SAI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief  Sets the SAI Tx configuration structure to default values.
 *
 * deprecated Do not use this function.  It has been superceded by @ref
 * SAI_GetClassicI2SConfig/SAI_GetLeftJustifiedConfig/SAI_GetRightJustifiedConfig/SAI_GetDSPConfig/SAI_GetTDMConfig
 *
 * This API initializes the configuration structure for use in SAI_TxConfig().
 * The initialized structure can remain unchanged in SAI_TxConfig(), or it can be modified
 *  before calling SAI_TxConfig().
 * This is an example.
   code
   sai_config_t config;
   SAI_TxGetDefaultConfig(&config);
   endcode
 *
 * param config pointer to master configuration structure
 */
void SAI_TxGetDefaultConfig(sai_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->bclkSource  = kSAI_BclkSourceMclkDiv;
    config->masterSlave = kSAI_Master;
#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    config->mclkOutputEnable = true;
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    config->mclkSource = kSAI_MclkSourceSysclk;
#endif
#endif /* FSL_FEATURE_SAI_HAS_MCR */
    config->protocol = kSAI_BusI2S;
    config->syncMode = kSAI_ModeAsync;
}

/*!
 * brief  Sets the SAI Rx configuration structure to default values.
 *
 * deprecated Do not use this function.  It has been superceded by @ref
 * SAI_GetClassicI2SConfig/SAI_GetLeftJustifiedConfig/SAI_GetRightJustifiedConfig/SAI_GetDSPConfig/SAI_GetTDMConfig
 *
 * This API initializes the configuration structure for use in SAI_RxConfig().
 * The initialized structure can remain unchanged in SAI_RxConfig() or it can be modified
 *  before calling SAI_RxConfig().
 * This is an example.
   code
   sai_config_t config;
   SAI_RxGetDefaultConfig(&config);
   endcode
 *
 * param config pointer to master configuration structure
 */
void SAI_RxGetDefaultConfig(sai_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->bclkSource  = kSAI_BclkSourceMclkDiv;
    config->masterSlave = kSAI_Master;
#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    config->mclkOutputEnable = true;
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    config->mclkSource = kSAI_MclkSourceSysclk;
#endif
#endif /* FSL_FEATURE_SAI_HAS_MCR */
    config->protocol = kSAI_BusI2S;
    config->syncMode = kSAI_ModeSync;
}

/*!
 * brief Resets the SAI Tx.
 *
 * This function enables the software reset and FIFO reset of SAI Tx. After reset, clear the reset bit.
 *
 * param base SAI base pointer
 */
void SAI_TxReset(I2S_Type *base)
{
    /* Set the software reset and FIFO reset to clear internal state */
    base->TCSR = I2S_TCSR_SR_MASK | I2S_TCSR_FR_MASK;

    /* Clear software reset bit, this should be done by software */
    base->TCSR &= ~I2S_TCSR_SR_MASK;

    /* Reset all Tx register values */
    base->TCR2 = 0;
    base->TCR3 = 0;
    base->TCR4 = 0;
    base->TCR5 = 0;
    base->TMR  = 0;
}

/*!
 * brief Resets the SAI Rx.
 *
 * This function enables the software reset and FIFO reset of SAI Rx. After reset, clear the reset bit.
 *
 * param base SAI base pointer
 */
void SAI_RxReset(I2S_Type *base)
{
    /* Set the software reset and FIFO reset to clear internal state */
    base->RCSR = I2S_RCSR_SR_MASK | I2S_RCSR_FR_MASK;

    /* Clear software reset bit, this should be done by software */
    base->RCSR &= ~I2S_RCSR_SR_MASK;

    /* Reset all Rx register values */
    base->RCR2 = 0;
    base->RCR3 = 0;
    base->RCR4 = 0;
    base->RCR5 = 0;
    base->RMR  = 0;
}

/*!
 * brief Enables/disables the SAI Tx.
 *
 * param base SAI base pointer
 * param enable True means enable SAI Tx, false means disable.
 */
void SAI_TxEnable(I2S_Type *base, bool enable)
{
    if (enable)
    {
        /* If clock is sync with Rx, should enable RE bit. */
        if (((base->TCR2 & I2S_TCR2_SYNC_MASK) >> I2S_TCR2_SYNC_SHIFT) == 0x1U)
        {
            base->RCSR = ((base->RCSR & 0xFFE3FFFFU) | I2S_RCSR_RE_MASK);
        }
        base->TCSR = ((base->TCSR & 0xFFE3FFFFU) | I2S_TCSR_TE_MASK);
        /* Also need to clear the FIFO error flag before start */
        SAI_TxClearStatusFlags(base, kSAI_FIFOErrorFlag);
    }
    else
    {
        /* If RE not sync with TE, than disable TE, otherwise, shall not disable TE */
        if (((base->RCR2 & I2S_RCR2_SYNC_MASK) >> I2S_RCR2_SYNC_SHIFT) != 0x1U)
        {
            /* Should not close RE even sync with Rx */
            base->TCSR = ((base->TCSR & 0xFFE3FFFFU) & (~I2S_TCSR_TE_MASK));
        }
    }
}

/*!
 * brief Enables/disables the SAI Rx.
 *
 * param base SAI base pointer
 * param enable True means enable SAI Rx, false means disable.
 */
void SAI_RxEnable(I2S_Type *base, bool enable)
{
    if (enable)
    {
        /* If clock is sync with Tx, should enable TE bit. */
        if (((base->RCR2 & I2S_RCR2_SYNC_MASK) >> I2S_RCR2_SYNC_SHIFT) == 0x1U)
        {
            base->TCSR = ((base->TCSR & 0xFFE3FFFFU) | I2S_TCSR_TE_MASK);
        }
        base->RCSR = ((base->RCSR & 0xFFE3FFFFU) | I2S_RCSR_RE_MASK);
        /* Also need to clear the FIFO error flag before start */
        SAI_RxClearStatusFlags(base, kSAI_FIFOErrorFlag);
    }
    else
    {
        /* While TX is not sync with RX, close RX */
        if (((base->TCR2 & I2S_TCR2_SYNC_MASK) >> I2S_TCR2_SYNC_SHIFT) != 0x1U)
        {
            base->RCSR = ((base->RCSR & 0xFFE3FFFFU) & (~I2S_RCSR_RE_MASK));
        }
    }
}

/*!
 * brief Do software reset or FIFO reset .
 *
 * FIFO reset means clear all the data in the FIFO, and make the FIFO pointer both to 0.
 * Software reset means clear the Tx internal logic, including the bit clock, frame count etc. But software
 * reset will not clear any configuration registers like TCR1~TCR5.
 * This function will also clear all the error flags such as FIFO error, sync error etc.
 *
 * param base SAI base pointer
 * param type Reset type, FIFO reset or software reset
 */
void SAI_TxSoftwareReset(I2S_Type *base, sai_reset_type_t type)
{
    base->TCSR |= (uint32_t)type;

    /* Clear the software reset */
    base->TCSR &= ~I2S_TCSR_SR_MASK;
}

/*!
 * brief Do software reset or FIFO reset .
 *
 * FIFO reset means clear all the data in the FIFO, and make the FIFO pointer both to 0.
 * Software reset means clear the Rx internal logic, including the bit clock, frame count etc. But software
 * reset will not clear any configuration registers like RCR1~RCR5.
 * This function will also clear all the error flags such as FIFO error, sync error etc.
 *
 * param base SAI base pointer
 * param type Reset type, FIFO reset or software reset
 */
void SAI_RxSoftwareReset(I2S_Type *base, sai_reset_type_t type)
{
    base->RCSR |= (uint32_t)type;

    /* Clear the software reset */
    base->RCSR &= ~I2S_RCSR_SR_MASK;
}

/*!
 * brief Set the Tx channel FIFO enable mask.
 *
 * param base SAI base pointer
 * param mask Channel enable mask, 0 means all channel FIFO disabled, 1 means channel 0 enabled,
 * 3 means both channel 0 and channel 1 enabled.
 */
void SAI_TxSetChannelFIFOMask(I2S_Type *base, uint8_t mask)
{
    base->TCR3 &= ~I2S_TCR3_TCE_MASK;
    base->TCR3 |= I2S_TCR3_TCE(mask);
}

/*!
 * brief Set the Rx channel FIFO enable mask.
 *
 * param base SAI base pointer
 * param mask Channel enable mask, 0 means all channel FIFO disabled, 1 means channel 0 enabled,
 * 3 means both channel 0 and channel 1 enabled.
 */
void SAI_RxSetChannelFIFOMask(I2S_Type *base, uint8_t mask)
{
    base->RCR3 &= ~I2S_RCR3_RCE_MASK;
    base->RCR3 |= I2S_RCR3_RCE(mask);
}

/*!
 * brief Set the Tx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_TxSetDataOrder(I2S_Type *base, sai_data_order_t order)
{
    uint32_t val = (base->TCR4) & (~I2S_TCR4_MF_MASK);

    val |= I2S_TCR4_MF(order);
    base->TCR4 = val;
}

/*!
 * brief Set the Rx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_RxSetDataOrder(I2S_Type *base, sai_data_order_t order)
{
    uint32_t val = (base->RCR4) & (~I2S_RCR4_MF_MASK);

    val |= I2S_RCR4_MF(order);
    base->RCR4 = val;
}

/*!
 * brief Set the Tx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_TxSetBitClockPolarity(I2S_Type *base, sai_clock_polarity_t polarity)
{
    uint32_t val = (base->TCR2) & (~I2S_TCR2_BCP_MASK);

    val |= I2S_TCR2_BCP(polarity);
    base->TCR2 = val;
}

/*!
 * brief Set the Rx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_RxSetBitClockPolarity(I2S_Type *base, sai_clock_polarity_t polarity)
{
    uint32_t val = (base->RCR2) & (~I2S_RCR2_BCP_MASK);

    val |= I2S_RCR2_BCP(polarity);
    base->RCR2 = val;
}

/*!
 * brief Set the Tx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_TxSetFrameSyncPolarity(I2S_Type *base, sai_clock_polarity_t polarity)
{
    uint32_t val = (base->TCR4) & (~I2S_TCR4_FSP_MASK);

    val |= I2S_TCR4_FSP(polarity);
    base->TCR4 = val;
}

/*!
 * brief Set the Rx data order.
 *
 * param base SAI base pointer
 * param order Data order MSB or LSB
 */
void SAI_RxSetFrameSyncPolarity(I2S_Type *base, sai_clock_polarity_t polarity)
{
    uint32_t val = (base->RCR4) & (~I2S_RCR4_FSP_MASK);

    val |= I2S_RCR4_FSP(polarity);
    base->RCR4 = val;
}

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
/*!
 * brief Set Tx FIFO packing feature.
 *
 * param base SAI base pointer.
 * param pack FIFO pack type. It is element of sai_fifo_packing_t.
 */
void SAI_TxSetFIFOPacking(I2S_Type *base, sai_fifo_packing_t pack)
{
    uint32_t val = base->TCR4;

    val &= ~I2S_TCR4_FPACK_MASK;
    val |= I2S_TCR4_FPACK(pack);
    base->TCR4 = val;
}

/*!
 * brief Set Rx FIFO packing feature.
 *
 * param base SAI base pointer.
 * param pack FIFO pack type. It is element of sai_fifo_packing_t.
 */
void SAI_RxSetFIFOPacking(I2S_Type *base, sai_fifo_packing_t pack)
{
    uint32_t val = base->RCR4;

    val &= ~I2S_RCR4_FPACK_MASK;
    val |= I2S_RCR4_FPACK(pack);
    base->RCR4 = val;
}
#endif /* FSL_FEATURE_SAI_HAS_FIFO_PACKING */

/*!
 * brief Transmitter bit clock rate configurations.
 *
 * param base SAI base pointer.
 * param sourceClockHz, bit clock source frequency.
 * param sampleRate audio data sample rate.
 * param bitWidth, audio data bitWidth.
 * param channelNumbers, audio channel numbers.
 */
void SAI_TxSetBitClockRate(
    I2S_Type *base, uint32_t sourceClockHz, uint32_t sampleRate, uint32_t bitWidth, uint32_t channelNumbers)
{
    uint32_t tcr2         = base->TCR2;
    uint32_t bitClockDiv  = 0;
    uint32_t bitClockFreq = sampleRate * bitWidth * channelNumbers;

    assert(sourceClockHz >= bitClockFreq);

    tcr2 &= ~I2S_TCR2_DIV_MASK;
    /* need to check the divided bclk, if bigger than target, then divider need to re-calculate. */
    bitClockDiv = sourceClockHz / bitClockFreq;
    /* for the condition where the source clock is smaller than target bclk */
    if (bitClockDiv == 0U)
    {
        bitClockDiv++;
    }
    /* recheck the divider if properly or not, to make sure output blck not bigger than target*/
    if ((sourceClockHz / bitClockDiv) > bitClockFreq)
    {
        bitClockDiv++;
    }

#if defined(FSL_FEATURE_SAI_HAS_BCLK_BYPASS) && (FSL_FEATURE_SAI_HAS_BCLK_BYPASS)
    /* if bclk same with MCLK, bypass the divider */
    if (bitClockDiv == 1U)
    {
        tcr2 |= I2S_TCR2_BYP_MASK;
    }
    else
#endif
    {
        tcr2 |= I2S_TCR2_DIV(bitClockDiv / 2U - 1U);
    }

    base->TCR2 = tcr2;
}

/*!
 * brief Receiver bit clock rate configurations.
 *
 * param base SAI base pointer.
 * param sourceClockHz, bit clock source frequency.
 * param sampleRate audio data sample rate.
 * param bitWidth, audio data bitWidth.
 * param channelNumbers, audio channel numbers.
 */
void SAI_RxSetBitClockRate(
    I2S_Type *base, uint32_t sourceClockHz, uint32_t sampleRate, uint32_t bitWidth, uint32_t channelNumbers)
{
    uint32_t rcr2         = base->RCR2;
    uint32_t bitClockDiv  = 0;
    uint32_t bitClockFreq = sampleRate * bitWidth * channelNumbers;

    assert(sourceClockHz >= bitClockFreq);

    rcr2 &= ~I2S_RCR2_DIV_MASK;
    /* need to check the divided bclk, if bigger than target, then divider need to re-calculate. */
    bitClockDiv = sourceClockHz / bitClockFreq;
    /* for the condition where the source clock is smaller than target bclk */
    if (bitClockDiv == 0U)
    {
        bitClockDiv++;
    }
    /* recheck the divider if properly or not, to make sure output blck not bigger than target*/
    if ((sourceClockHz / bitClockDiv) > bitClockFreq)
    {
        bitClockDiv++;
    }

#if defined(FSL_FEATURE_SAI_HAS_BCLK_BYPASS) && (FSL_FEATURE_SAI_HAS_BCLK_BYPASS)
    /* if bclk same with MCLK, bypass the divider */
    if (bitClockDiv == 1U)
    {
        rcr2 |= I2S_RCR2_BYP_MASK;
    }
    else
#endif
    {
        rcr2 |= I2S_RCR2_DIV(bitClockDiv / 2U - 1U);
    }

    base->RCR2 = rcr2;
}

/*!
 * brief Transmitter Bit clock configurations.
 *
 * param base SAI base pointer.
 * param masterSlave master or slave.
 * param config bit clock other configurations, can be NULL in slave mode.
 */
void SAI_TxSetBitclockConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_bit_clock_t *config)
{
    uint32_t tcr2 = base->TCR2;

    if ((masterSlave == kSAI_Master) || (masterSlave == kSAI_Bclk_Master_FrameSync_Slave))
    {
        assert(config != NULL);

        tcr2 &= ~(I2S_TCR2_BCD_MASK | I2S_TCR2_BCP_MASK | I2S_TCR2_BCI_MASK | I2S_TCR2_BCS_MASK | I2S_TCR2_MSEL_MASK);
        tcr2 |= I2S_TCR2_BCD(1U) | I2S_TCR2_BCP(config->bclkPolarity) | I2S_TCR2_BCI(config->bclkInputDelay) |
                I2S_TCR2_BCS(config->bclkSrcSwap) | I2S_TCR2_MSEL(config->bclkSource);
    }
    else
    {
        tcr2 &= ~(I2S_TCR2_BCD_MASK);
    }

    base->TCR2 = tcr2;
}

/*!
 * brief Receiver Bit clock configurations.
 *
 * param base SAI base pointer.
 * param masterSlave master or slave.
 * param config bit clock other configurations, can be NULL in slave mode.
 */
void SAI_RxSetBitclockConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_bit_clock_t *config)
{
    uint32_t rcr2 = base->RCR2;

    if ((masterSlave == kSAI_Master) || (masterSlave == kSAI_Bclk_Master_FrameSync_Slave))
    {
        assert(config != NULL);

        rcr2 &= ~(I2S_RCR2_BCD_MASK | I2S_RCR2_BCP_MASK | I2S_RCR2_BCI_MASK | I2S_RCR2_BCS_MASK | I2S_RCR2_MSEL_MASK);
        rcr2 |= I2S_RCR2_BCD(1U) | I2S_RCR2_BCP(config->bclkPolarity) | I2S_RCR2_BCI(config->bclkInputDelay) |
                I2S_RCR2_BCS(config->bclkSrcSwap) | I2S_RCR2_MSEL(config->bclkSource);
    }
    else
    {
        rcr2 &= ~(I2S_RCR2_BCD_MASK);
    }

    base->RCR2 = rcr2;
}

#if (defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)) || \
    (defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER))
/*!
 * brief Master clock configurations.
 *
 * param base SAI base pointer.
 * param config master clock configurations.
 */
void SAI_SetMasterClockConfig(I2S_Type *base, sai_master_clock_t *config)
{
    assert(config != NULL);

#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    uint32_t val = 0;
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    /* Master clock source setting */
    val       = (base->MCR & ~I2S_MCR_MICS_MASK);
    base->MCR = (val | I2S_MCR_MICS(config->mclkSource));
#endif

    /* Configure Master clock output enable */
    val       = (base->MCR & ~I2S_MCR_MOE_MASK);
    base->MCR = (val | I2S_MCR_MOE(config->mclkOutputEnable));
#endif /* FSL_FEATURE_SAI_HAS_MCR */

#if ((defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)) || \
     (defined(FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV) && (FSL_FEATURE_SAI_HAS_MCR_MCLK_POST_DIV)))
    /* Check if master clock divider enabled, then set master clock divider */
    if (config->mclkOutputEnable)
    {
        SAI_SetMasterClockDivider(base, config->mclkHz, config->mclkSourceClkHz);
    }
#endif /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */
}
#endif

/*!
 * brief SAI transmitter fifo configurations.
 *
 * param base SAI base pointer.
 * param config fifo configurations.
 */
void SAI_TxSetFifoConfig(I2S_Type *base, sai_fifo_t *config)
{
    assert(config != NULL);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    assert(config->fifoWatermark <= (I2S_TCR1_TFW_MASK >> I2S_TCR1_TFW_SHIFT));
#endif

    uint32_t tcr4 = base->TCR4;

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE
    tcr4 &= ~I2S_TCR4_FCOMB_MASK;
    tcr4 |= I2S_TCR4_FCOMB(config->fifoCombine);
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    tcr4 &= ~I2S_TCR4_FCONT_MASK;
    /* ERR05144: not set FCONT = 1 when TMR > 0, the transmit shift register may not load correctly that will cause TX
     * not work */
    if (base->TMR == 0U)
    {
        tcr4 |= I2S_TCR4_FCONT(config->fifoContinueOneError);
    }
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
    tcr4 &= ~I2S_TCR4_FPACK_MASK;
    tcr4 |= I2S_TCR4_FPACK(config->fifoPacking);
#endif

    base->TCR4 = tcr4;

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    base->TCR1 = (base->TCR1 & (~I2S_TCR1_TFW_MASK)) | I2S_TCR1_TFW(config->fifoWatermark);
#endif
}

/*!
 * brief SAI receiver fifo configurations.
 *
 * param base SAI base pointer.
 * param config fifo configurations.
 */
void SAI_RxSetFifoConfig(I2S_Type *base, sai_fifo_t *config)
{
    assert(config != NULL);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    assert(config->fifoWatermark <= (I2S_TCR1_TFW_MASK >> I2S_TCR1_TFW_SHIFT));
#endif
    uint32_t rcr4 = base->RCR4;

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE
    rcr4 &= ~I2S_RCR4_FCOMB_MASK;
    rcr4 |= I2S_RCR4_FCOMB(config->fifoCombine);
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    rcr4 &= ~I2S_RCR4_FCONT_MASK;
    rcr4 |= I2S_RCR4_FCONT(config->fifoContinueOneError);
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
    rcr4 &= ~I2S_RCR4_FPACK_MASK;
    rcr4 |= I2S_RCR4_FPACK(config->fifoPacking);
#endif

    base->RCR4 = rcr4;

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    base->RCR1 = (base->RCR1 & (~I2S_RCR1_RFW_MASK)) | I2S_RCR1_RFW(config->fifoWatermark);
#endif
}

/*!
 * brief SAI transmitter Frame sync configurations.
 *
 * param base SAI base pointer.
 * param masterSlave master or slave.
 * param config frame sync configurations, can be NULL in slave mode.
 */
void SAI_TxSetFrameSyncConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_frame_sync_t *config)
{
    uint32_t tcr4 = base->TCR4;

    if ((masterSlave == kSAI_Master) || (masterSlave == kSAI_Bclk_Slave_FrameSync_Master))
    {
        assert(config != NULL);
        assert((config->frameSyncWidth - 1U) <= (I2S_TCR4_SYWD_MASK >> I2S_TCR4_SYWD_SHIFT));

        tcr4 &= ~(I2S_TCR4_FSE_MASK | I2S_TCR4_FSP_MASK | I2S_TCR4_FSD_MASK | I2S_TCR4_SYWD_MASK);

#if defined(FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND) && FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND
        tcr4 &= ~I2S_TCR4_ONDEM_MASK;
        tcr4 |= I2S_TCR4_ONDEM(config->frameSyncGenerateOnDemand);
#endif

        tcr4 |= I2S_TCR4_FSE(config->frameSyncEarly) | I2S_TCR4_FSP(config->frameSyncPolarity) | I2S_TCR4_FSD(1U) |
                I2S_TCR4_SYWD(config->frameSyncWidth - 1U);
    }
    else
    {
        tcr4 &= ~I2S_TCR4_FSD_MASK;
    }

    base->TCR4 = tcr4;
}

/*!
 * brief SAI receiver Frame sync configurations.
 *
 * param base SAI base pointer.
 * param masterSlave master or slave.
 * param config frame sync configurations, can be NULL in slave mode.
 */
void SAI_RxSetFrameSyncConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_frame_sync_t *config)
{
    uint32_t rcr4 = base->RCR4;

    if ((masterSlave == kSAI_Master) || (masterSlave == kSAI_Bclk_Slave_FrameSync_Master))
    {
        assert(config != NULL);
        assert((config->frameSyncWidth - 1U) <= (I2S_RCR4_SYWD_MASK >> I2S_RCR4_SYWD_SHIFT));

        rcr4 &= ~(I2S_RCR4_FSE_MASK | I2S_RCR4_FSP_MASK | I2S_RCR4_FSD_MASK | I2S_RCR4_SYWD_MASK);

#if defined(FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND) && FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND
        rcr4 &= ~I2S_RCR4_ONDEM_MASK;
        rcr4 |= I2S_RCR4_ONDEM(config->frameSyncGenerateOnDemand);
#endif

        rcr4 |= I2S_RCR4_FSE(config->frameSyncEarly) | I2S_RCR4_FSP(config->frameSyncPolarity) | I2S_RCR4_FSD(1U) |
                I2S_RCR4_SYWD(config->frameSyncWidth - 1U);
    }
    else
    {
        rcr4 &= ~I2S_RCR4_FSD_MASK;
    }

    base->RCR4 = rcr4;
}

/*!
 * brief SAI transmitter Serial data configurations.
 *
 * param base SAI base pointer.
 * param config serial data configurations.
 */
void SAI_TxSetSerialDataConfig(I2S_Type *base, sai_serial_data_t *config)
{
    assert(config != NULL);

    uint32_t tcr4 = base->TCR4;

    base->TCR5 = I2S_TCR5_WNW(config->dataWordNLength - 1U) | I2S_TCR5_W0W(config->dataWord0Length - 1U) |
                 I2S_TCR5_FBT(config->dataFirstBitShifted - 1U);
    base->TMR = config->dataMaskedWord;
#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    /* ERR05144: not set FCONT = 1 when TMR > 0, the transmit shift register may not load correctly that will cause TX
     * not work */
    if (config->dataMaskedWord > 0U)
    {
        tcr4 &= ~I2S_TCR4_FCONT_MASK;
    }
#endif
    tcr4 &= ~(I2S_TCR4_FRSZ_MASK | I2S_TCR4_MF_MASK);
    tcr4 |= I2S_TCR4_FRSZ(config->dataWordNum - 1U) | I2S_TCR4_MF(config->dataOrder);

#if defined(FSL_FEATURE_SAI_HAS_CHANNEL_MODE) && FSL_FEATURE_SAI_HAS_CHANNEL_MODE
    tcr4 &= ~I2S_TCR4_CHMOD_MASK;
    tcr4 |= I2S_TCR4_CHMOD(config->dataMode);
#endif

    base->TCR4 = tcr4;
}

/*!
 * @brief SAI receiver Serial data configurations.
 *
 * @param base SAI base pointer.
 * @param config serial data configurations.
 */
void SAI_RxSetSerialDataConfig(I2S_Type *base, sai_serial_data_t *config)
{
    assert(config != NULL);

    uint32_t rcr4 = base->RCR4;

    base->RCR5 = I2S_RCR5_WNW(config->dataWordNLength - 1U) | I2S_RCR5_W0W(config->dataWord0Length - 1U) |
                 I2S_RCR5_FBT(config->dataFirstBitShifted - 1U);
    base->RMR = config->dataMaskedWord;

    rcr4 &= ~(I2S_RCR4_FRSZ_MASK | I2S_RCR4_MF_MASK);
    rcr4 |= I2S_RCR4_FRSZ(config->dataWordNum - 1u) | I2S_RCR4_MF(config->dataOrder);

    base->RCR4 = rcr4;
}

/*!
 * brief SAI transmitter configurations.
 *
 * param base SAI base pointer.
 * param config transmitter configurations.
 */
void SAI_TxSetConfig(I2S_Type *base, sai_transceiver_t *config)
{
    assert(config != NULL);
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t val = 0U, i = 0U;
    uint32_t channelNums = 0U;
    /* reset transmitter */
    SAI_TxReset(base);

    /* if channel mask is not set, then format->channel must be set,
     use it to get channel mask value */
    if (config->channelMask == 0U)
    {
        config->channelMask = 1U << config->startChannel;
    }

    for (i = 0U; i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base); i++)
    {
        if (((uint32_t)1 << i) & config->channelMask)
        {
            /* get start channel number when channelNums = 0 only */
            if (channelNums == 0U)
            {
                config->startChannel = i;
            }
            channelNums++;
            config->endChannel = i;
        }
    }
    assert(channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));
    config->channelNums = channelNums;
#if defined(FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE) && (FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE)
    /* make sure combine mode disabled while multipe channel is used */
    if (config->channelNums > 1U)
    {
        base->TCR4 &= ~I2S_TCR4_FCOMB_MASK;
    }
#endif

    /* Set data channel */
    base->TCR3 &= ~I2S_TCR3_TCE_MASK;
    base->TCR3 |= I2S_TCR3_TCE(config->channelMask);

    /* Set Sync mode */
    switch (config->syncMode)
    {
        case kSAI_ModeAsync:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(0U));
            break;
        case kSAI_ModeSync:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(1U));
            /* If sync with Rx, should set Rx to async mode */
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(0U));
            break;
#if defined(FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI) && (FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI)
        case kSAI_ModeSyncWithOtherTx:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(2U));
            break;
        case kSAI_ModeSyncWithOtherRx:
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(3U));
            break;
#endif /* FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI */
        default:
            break;
    }

    /* bit clock configurations */
    SAI_TxSetBitclockConfig(base, config->masterSlave, &config->bitClock);
    /* serial data configurations */
    SAI_TxSetSerialDataConfig(base, &config->serialData);
    /* frame sync configurations */
    SAI_TxSetFrameSyncConfig(base, config->masterSlave, &config->frameSync);
    /* fifo configurations */
    SAI_TxSetFifoConfig(base, &config->fifo);
}

/*!
 * brief SAI transmitter transfer configurations.
 *
 * This function initializes the TX, include bit clock, frame sync, master clock, serial data and fifo configurations.
 *
 * param base SAI base pointer.
 * param handle SAI handle pointer.
 * param config tranmitter configurations.
 */
void SAI_TransferTxSetConfig(I2S_Type *base, sai_handle_t *handle, sai_transceiver_t *config)
{
    assert(handle != NULL);
    assert(config != NULL);

    handle->bitWidth = config->frameSync.frameSyncWidth;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->watermark = config->fifo.fifoWatermark;
#endif

    /* transmitter configurations */
    SAI_TxSetConfig(base, config);

    handle->channel = config->startChannel;
    /* used for multi channel */
    handle->channelMask = config->channelMask;
    handle->channelNums = config->channelNums;
    handle->endChannel  = config->endChannel;
}

/*!
 * brief SAI receiver configurations.
 *
 * param base SAI base pointer.
 * param config transmitter configurations.
 */
void SAI_RxSetConfig(I2S_Type *base, sai_transceiver_t *config)
{
    assert(config != NULL);
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t val = 0U, i = 0U;
    uint32_t channelNums = 0U;

    /* reset receiver */
    SAI_RxReset(base);

    /* if channel mask is not set, then format->channel must be set,
     use it to get channel mask value */
    if (config->channelMask == 0U)
    {
        config->channelMask = 1U << config->startChannel;
    }

    for (i = 0U; i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base); i++)
    {
        if (((uint32_t)1 << i) & config->channelMask)
        {
            /* get start channel number when channelNums = 0 only */
            if (channelNums == 0U)
            {
                config->startChannel = i;
            }
            channelNums++;
            config->endChannel = i;
        }
    }
    assert(channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));
    config->channelNums = channelNums;
#if defined(FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE) && (FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE)
    /* make sure combine mode disabled while multipe channel is used */
    if (config->channelNums > 1U)
    {
        base->RCR4 &= ~I2S_RCR4_FCOMB_MASK;
    }
#endif

    /* Set data channel */
    base->RCR3 &= ~I2S_RCR3_RCE_MASK;
    base->RCR3 |= I2S_RCR3_RCE(config->channelMask);

    /* Set Sync mode */
    switch (config->syncMode)
    {
        case kSAI_ModeAsync:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(0U));
            break;
        case kSAI_ModeSync:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(1U));
            /* If sync with Tx, should set Tx to async mode */
            val = base->TCR2;
            val &= ~I2S_TCR2_SYNC_MASK;
            base->TCR2 = (val | I2S_TCR2_SYNC(0U));
            break;
#if defined(FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI) && (FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI)
        case kSAI_ModeSyncWithOtherTx:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(2U));
            break;
        case kSAI_ModeSyncWithOtherRx:
            val = base->RCR2;
            val &= ~I2S_RCR2_SYNC_MASK;
            base->RCR2 = (val | I2S_RCR2_SYNC(3U));
            break;
#endif /* FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI */
        default:
            break;
    }

    /* bit clock configurations */
    SAI_RxSetBitclockConfig(base, config->masterSlave, &config->bitClock);
    /* serial data configurations */
    SAI_RxSetSerialDataConfig(base, &config->serialData);
    /* frame sync configurations */
    SAI_RxSetFrameSyncConfig(base, config->masterSlave, &config->frameSync);
    /* fifo configurations */
    SAI_RxSetFifoConfig(base, &config->fifo);
}

/*!
 * brief SAI receiver transfer configurations.
 *
 * This function initializes the TX, include bit clock, frame sync, master clock, serial data and fifo configurations.
 *
 * param base SAI base pointer.
 * param handle SAI handle pointer.
 * param config tranmitter configurations.
 */
void SAI_TransferRxSetConfig(I2S_Type *base, sai_handle_t *handle, sai_transceiver_t *config)
{
    assert(handle != NULL);
    assert(config != NULL);

    handle->bitWidth = config->frameSync.frameSyncWidth;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->watermark = config->fifo.fifoWatermark;
#endif

    /* receiver configurations */
    SAI_RxSetConfig(base, config);

    handle->channel = config->startChannel;
    /* used for multi channel */
    handle->channelMask = config->channelMask;
    handle->channelNums = config->channelNums;
    handle->endChannel  = config->endChannel;
}

/*!
 * brief Get classic I2S mode configurations.
 *
 * param config transceiver configurations.
 * param bitWidth audio data bitWidth.
 * param mode audio data channel.
 * param saiChannelMask channel mask value to enable.
 */
void SAI_GetClassicI2SConfig(sai_transceiver_t *config,
                             sai_word_width_t bitWidth,
                             sai_mono_stereo_t mode,
                             uint32_t saiChannelMask)
{
    SAI_GetCommonConfig(config, bitWidth, mode, saiChannelMask);
}

/*!
 * brief Get left justified mode configurations.
 *
 * param config transceiver configurations.
 * param bitWidth audio data bitWidth.
 * param mode audio data channel.
 * param saiChannelMask channel mask value to enable.
 */
void SAI_GetLeftJustifiedConfig(sai_transceiver_t *config,
                                sai_word_width_t bitWidth,
                                sai_mono_stereo_t mode,
                                uint32_t saiChannelMask)
{
    assert(NULL != config);
    assert(saiChannelMask != 0U);

    SAI_GetCommonConfig(config, bitWidth, mode, saiChannelMask);

    config->frameSync.frameSyncEarly    = false;
    config->frameSync.frameSyncPolarity = kSAI_PolarityActiveHigh;
}

/*!
 * brief Get right justified mode configurations.
 *
 * param config transceiver configurations.
 * param bitWidth audio data bitWidth.
 * param mode audio data channel.
 * param saiChannelMask channel mask value to enable.
 */
void SAI_GetRightJustifiedConfig(sai_transceiver_t *config,
                                 sai_word_width_t bitWidth,
                                 sai_mono_stereo_t mode,
                                 uint32_t saiChannelMask)
{
    assert(NULL != config);
    assert(saiChannelMask != 0U);

    SAI_GetCommonConfig(config, bitWidth, mode, saiChannelMask);

    config->frameSync.frameSyncEarly    = false;
    config->frameSync.frameSyncPolarity = kSAI_PolarityActiveHigh;
}

/*!
 * brief Get DSP mode configurations.
 *
 * param config transceiver configurations.
 * param bitWidth audio data bitWidth.
 * param mode audio data channel.
 * param saiChannelMask channel mask value to enable.
 */
void SAI_GetDSPConfig(sai_transceiver_t *config,
                      sai_frame_sync_len_t frameSyncWidth,
                      sai_word_width_t bitWidth,
                      sai_mono_stereo_t mode,
                      uint32_t saiChannelMask)
{
    assert(NULL != config);
    assert(saiChannelMask != 0U);

    SAI_GetCommonConfig(config, bitWidth, mode, saiChannelMask);

    /* frame sync default configurations */
    switch (frameSyncWidth)
    {
        case kSAI_FrameSyncLenOneBitClk:
            config->frameSync.frameSyncWidth = 1U;
            break;
        default:
            assert(false);
    }
    config->frameSync.frameSyncEarly    = false;
    config->frameSync.frameSyncPolarity = kSAI_PolarityActiveHigh;
}

/*!
 * brief Get TDM mode configurations.
 *
 * param config transceiver configurations.
 * param bitWidth audio data bitWidth.
 * param mode audio data channel.
 * param saiChannelMask channel mask value to enable.
 */
void SAI_GetTDMConfig(sai_transceiver_t *config,
                      sai_frame_sync_len_t frameSyncWidth,
                      sai_word_width_t bitWidth,
                      uint32_t dataWordNum,
                      uint32_t saiChannelMask)
{
    assert(NULL != config);
    assert(saiChannelMask != 0U);
    assert(dataWordNum <= 32U);

    SAI_GetCommonConfig(config, bitWidth, kSAI_Stereo, saiChannelMask);

    /* frame sync default configurations */
    switch (frameSyncWidth)
    {
        case kSAI_FrameSyncLenOneBitClk:
            config->frameSync.frameSyncWidth = 1U;
            break;
        case kSAI_FrameSyncLenPerWordWidth:
            break;
        default:
            assert(false);
    }
    config->frameSync.frameSyncEarly    = false;
    config->frameSync.frameSyncPolarity = kSAI_PolarityActiveHigh;
    config->serialData.dataWordNum      = dataWordNum;
}

/*!
 * brief Configures the SAI Tx audio format.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_TxSetConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base SAI base pointer.
 * param format Pointer to the SAI audio data format structure.
 * param mclkSourceClockHz SAI master clock source frequency in Hz.
 * param bclkSourceClockHz SAI bit clock source frequency in Hz. If the bit clock source is a master
 * clock, this value should equal the masterClockHz.
 */
void SAI_TxSetFormat(I2S_Type *base,
                     sai_transfer_format_t *format,
                     uint32_t mclkSourceClockHz,
                     uint32_t bclkSourceClockHz)
{
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t bclk    = 0;
    uint32_t val     = 0;
    uint32_t i       = 0U;
    uint32_t divider = 0U, channelNums = 0U;

    if (format->isFrameSyncCompact)
    {
        bclk = format->sampleRate_Hz * format->bitWidth * (format->stereo == kSAI_Stereo ? 2U : 1U);
        val  = (base->TCR4 & (~I2S_TCR4_SYWD_MASK));
        val |= I2S_TCR4_SYWD(format->bitWidth - 1U);
        base->TCR4 = val;
    }
    else
    {
        bclk = format->sampleRate_Hz * 32U * 2U;
    }

/* Compute the mclk */
#if defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)
    /* Check if master clock divider enabled, then set master clock divider */
    if (base->MCR & I2S_MCR_MOE_MASK)
    {
        SAI_SetMasterClockDivider(base, format->masterClockHz, mclkSourceClockHz);
    }
#endif /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */

    /* Set bclk if needed */
    if (base->TCR2 & I2S_TCR2_BCD_MASK)
    {
        base->TCR2 &= ~I2S_TCR2_DIV_MASK;
        /* need to check the divided bclk, if bigger than target, then divider need to re-calculate. */
        divider = bclkSourceClockHz / bclk;
        /* for the condition where the source clock is smaller than target bclk */
        if (divider == 0U)
        {
            divider++;
        }
        /* recheck the divider if properly or not, to make sure output blck not bigger than target*/
        if ((bclkSourceClockHz / divider) > bclk)
        {
            divider++;
        }

#if defined(FSL_FEATURE_SAI_HAS_BCLK_BYPASS) && (FSL_FEATURE_SAI_HAS_BCLK_BYPASS)
        /* if bclk same with MCLK, bypass the divider */
        if (divider == 1U)
        {
            base->TCR2 |= I2S_TCR2_BYP_MASK;
        }
        else
#endif
        {
            base->TCR2 |= I2S_TCR2_DIV(divider / 2U - 1U);
        }
    }

    /* Set bitWidth */
    val = (format->isFrameSyncCompact) ? (format->bitWidth - 1) : 31U;
    if (format->protocol == kSAI_BusRightJustified)
    {
        base->TCR5 = I2S_TCR5_WNW(val) | I2S_TCR5_W0W(val) | I2S_TCR5_FBT(val);
    }
    else
    {
        if (base->TCR4 & I2S_TCR4_MF_MASK)
        {
            base->TCR5 = I2S_TCR5_WNW(val) | I2S_TCR5_W0W(val) | I2S_TCR5_FBT(format->bitWidth - 1);
        }
        else
        {
            base->TCR5 = I2S_TCR5_WNW(val) | I2S_TCR5_W0W(val) | I2S_TCR5_FBT(0);
        }
    }

    /* Set mono or stereo */
    base->TMR = (uint32_t)format->stereo;

    /* if channel mask is not set, then format->channel must be set,
     use it to get channel mask value */
    if (format->channelMask == 0U)
    {
        format->channelMask = 1U << format->channel;
    }

    /* if channel nums is not set, calculate it here according to channelMask*/
    for (i = 0U; i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base); i++)
    {
        if (((uint32_t)1 << i) & format->channelMask)
        {
            /* geet start channel number when channelNums = 0 only */
            if (channelNums == 0U)
            {
                format->channel = i;
            }
            channelNums++;
            format->endChannel = i;
        }
    }
    format->channelNums = channelNums;
    assert(format->channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));

#if defined(FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE) && (FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE)
    /* make sure combine mode disabled while multipe channel is used */
    if (format->channelNums > 1U)
    {
        base->TCR4 &= ~I2S_TCR4_FCOMB_MASK;
    }
#endif

    /* Set data channel */
    base->TCR3 &= ~I2S_TCR3_TCE_MASK;
    base->TCR3 |= I2S_TCR3_TCE(format->channelMask);

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Set watermark */
    base->TCR1 = format->watermark;
#endif /* FSL_FEATURE_SAI_FIFO_COUNT  */
}

/*!
 * brief Configures the SAI Rx audio format.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_RxSetConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base SAI base pointer.
 * param format Pointer to the SAI audio data format structure.
 * param mclkSourceClockHz SAI master clock source frequency in Hz.
 * param bclkSourceClockHz SAI bit clock source frequency in Hz. If the bit clock source is a master
 * clock, this value should equal the masterClockHz.
 */
void SAI_RxSetFormat(I2S_Type *base,
                     sai_transfer_format_t *format,
                     uint32_t mclkSourceClockHz,
                     uint32_t bclkSourceClockHz)
{
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t bclk    = 0;
    uint32_t val     = 0;
    uint32_t i       = 0U;
    uint32_t divider = 0U, channelNums = 0U;

    if (format->isFrameSyncCompact)
    {
        bclk = format->sampleRate_Hz * format->bitWidth * (format->stereo == kSAI_Stereo ? 2U : 1U);
        val  = (base->RCR4 & (~I2S_RCR4_SYWD_MASK));
        val |= I2S_RCR4_SYWD(format->bitWidth - 1U);
        base->RCR4 = val;
    }
    else
    {
        bclk = format->sampleRate_Hz * 32U * 2U;
    }

/* Compute the mclk */
#if defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)
    /* Check if master clock divider enabled */
    if (base->MCR & I2S_MCR_MOE_MASK)
    {
        SAI_SetMasterClockDivider(base, format->masterClockHz, mclkSourceClockHz);
    }
#endif /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */

    /* Set bclk if needed */
    if (base->RCR2 & I2S_RCR2_BCD_MASK)
    {
        base->RCR2 &= ~I2S_RCR2_DIV_MASK;
        /* need to check the divided bclk, if bigger than target, then divider need to re-calculate. */
        divider = bclkSourceClockHz / bclk;
        /* for the condition where the source clock is smaller than target bclk */
        if (divider == 0U)
        {
            divider++;
        }
        /* recheck the divider if properly or not, to make sure output blck not bigger than target*/
        if ((bclkSourceClockHz / divider) > bclk)
        {
            divider++;
        }
#if defined(FSL_FEATURE_SAI_HAS_BCLK_BYPASS) && (FSL_FEATURE_SAI_HAS_BCLK_BYPASS)
        /* if bclk same with MCLK, bypass the divider */
        if (divider == 1U)
        {
            base->RCR2 |= I2S_RCR2_BYP_MASK;
        }
        else
#endif
        {
            base->RCR2 |= I2S_RCR2_DIV(divider / 2U - 1U);
        }
    }

    /* Set bitWidth */
    val = (format->isFrameSyncCompact) ? (format->bitWidth - 1) : 31U;
    if (format->protocol == kSAI_BusRightJustified)
    {
        base->RCR5 = I2S_RCR5_WNW(val) | I2S_RCR5_W0W(val) | I2S_RCR5_FBT(val);
    }
    else
    {
        if (base->RCR4 & I2S_RCR4_MF_MASK)
        {
            base->RCR5 = I2S_RCR5_WNW(val) | I2S_RCR5_W0W(val) | I2S_RCR5_FBT(format->bitWidth - 1);
        }
        else
        {
            base->RCR5 = I2S_RCR5_WNW(val) | I2S_RCR5_W0W(val) | I2S_RCR5_FBT(0);
        }
    }

    /* Set mono or stereo */
    base->RMR = (uint32_t)format->stereo;

    /* if channel mask is not set, then format->channel must be set,
     use it to get channel mask value */
    if (format->channelMask == 0U)
    {
        format->channelMask = 1U << format->channel;
    }

    /* if channel nums is not set, calculate it here according to channelMask*/
    for (i = 0U; i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base); i++)
    {
        if (((uint32_t)1 << i) & format->channelMask)
        {
            /* geet start channel number when channelNums = 0 only */
            if (channelNums == 0U)
            {
                format->channel = i;
            }
            channelNums++;
            format->endChannel = i;
        }
    }
    format->channelNums = channelNums;
    assert(format->channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));

#if defined(FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE) && (FSL_FEATURE_SAI_HAS_FIFO_COMBINE_MODE)
    /* make sure combine mode disabled while multipe channel is used */
    if (format->channelNums > 1U)
    {
        base->RCR4 &= ~I2S_RCR4_FCOMB_MASK;
    }
#endif

    /* Set data channel */
    base->RCR3 &= ~I2S_RCR3_RCE_MASK;
    /* enable all the channel */
    base->RCR3 |= I2S_RCR3_RCE(format->channelMask);

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Set watermark */
    base->RCR1 = format->watermark;
#endif /* FSL_FEATURE_SAI_FIFO_COUNT  */
}

/*!
 * brief Sends data using a blocking method.
 *
 * note This function blocks by polling until data is ready to be sent.
 *
 * param base SAI base pointer.
 * param channel Data channel used.
 * param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * param buffer Pointer to the data to be written.
 * param size Bytes to be written.
 */
void SAI_WriteBlocking(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{
    uint32_t i           = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    bytesPerWord = (size_t)((FSL_FEATURE_SAI_FIFO_COUNT - base->TCR1) * bytesPerWord);
#endif

    while (i < size)
    {
        /* Wait until it can write data */
        while (!(base->TCSR & I2S_TCSR_FWF_MASK))
        {
        }

        SAI_WriteNonBlocking(base, channel, 1U << channel, channel, bitWidth, buffer, bytesPerWord);
        buffer += bytesPerWord;
        i += bytesPerWord;
    }

    /* Wait until the last data is sent */
    while (!(base->TCSR & I2S_TCSR_FWF_MASK))
    {
    }
}

/*!
 * brief Sends data to multi channel using a blocking method.
 *
 * note This function blocks by polling until data is ready to be sent.
 *
 * param base SAI base pointer.
 * param channel Data channel used.
 * param channelMask channel mask.
 * param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * param buffer Pointer to the data to be written.
 * param size Bytes to be written.
 */
void SAI_WriteMultiChannelBlocking(
    I2S_Type *base, uint32_t channel, uint32_t channelMask, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t i = 0, j = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t channelNums = 0U, endChannel = 0U;

#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    bytesPerWord = (size_t)((FSL_FEATURE_SAI_FIFO_COUNT - base->TCR1) * bytesPerWord);
#endif

    for (i = 0U; (i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base)); i++)
    {
        if ((1U << i) & (channelMask))
        {
            channelNums++;
            endChannel = i;
        }
    }

    assert(channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));
    bytesPerWord *= channelNums;

    while (j < size)
    {
        /* Wait until it can write data */
        while (!(base->TCSR & I2S_TCSR_FWF_MASK))
        {
        }

        SAI_WriteNonBlocking(base, channel, channelMask, endChannel, bitWidth, buffer, bytesPerWord);
        buffer += bytesPerWord;
        j += bytesPerWord;
    }

    /* Wait until the last data is sent */
    while (!(base->TCSR & I2S_TCSR_FWF_MASK))
    {
    }
}

/*!
 * brief Receives multi channel data using a blocking method.
 *
 * note This function blocks by polling until data is ready to be sent.
 *
 * param base SAI base pointer.
 * param channel Data channel used.
 * param channelMask channel mask.
 * param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * param buffer Pointer to the data to be read.
 * param size Bytes to be read.
 */
void SAI_ReadMultiChannelBlocking(
    I2S_Type *base, uint32_t channel, uint32_t channelMask, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{
    assert(FSL_FEATURE_SAI_CHANNEL_COUNTn(base) != -1);

    uint32_t i = 0, j = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t channelNums = 0U, endChannel = 0U;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    bytesPerWord = (size_t)(base->RCR1 * bytesPerWord);
#endif
    for (i = 0U; (i < FSL_FEATURE_SAI_CHANNEL_COUNTn(base)); i++)
    {
        if ((1U << i) & (channelMask))
        {
            channelNums++;
            endChannel = i;
        }
    }

    assert(channelNums <= FSL_FEATURE_SAI_CHANNEL_COUNTn(base));
    bytesPerWord *= channelNums;

    while (j < size)
    {
        /* Wait until data is received */
        while (!(base->RCSR & I2S_RCSR_FWF_MASK))
        {
        }

        SAI_ReadNonBlocking(base, channel, channelMask, endChannel, bitWidth, buffer, bytesPerWord);
        buffer += bytesPerWord;
        j += bytesPerWord;
    }
}

/*!
 * brief Receives data using a blocking method.
 *
 * note This function blocks by polling until data is ready to be sent.
 *
 * param base SAI base pointer.
 * param channel Data channel used.
 * param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * param buffer Pointer to the data to be read.
 * param size Bytes to be read.
 */
void SAI_ReadBlocking(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{
    uint32_t i           = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    bytesPerWord = (size_t)(base->RCR1 * bytesPerWord);
#endif

    while (i < size)
    {
        /* Wait until data is received */
        while (!(base->RCSR & I2S_RCSR_FWF_MASK))
        {
        }

        SAI_ReadNonBlocking(base, channel, 1U << channel, channel, bitWidth, buffer, bytesPerWord);
        buffer += bytesPerWord;
        i += bytesPerWord;
    }
}

/*!
 * brief Initializes the SAI Tx handle.
 *
 * This function initializes the Tx handle for the SAI Tx transactional APIs. Call
 * this function once to get the handle initialized.
 *
 * param base SAI base pointer
 * param handle SAI handle pointer.
 * param callback Pointer to the user callback function.
 * param userData User parameter passed to the callback function
 */
void SAI_TransferTxCreateHandle(I2S_Type *base, sai_handle_t *handle, sai_transfer_callback_t callback, void *userData)
{
    assert(handle);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    s_saiHandle[SAI_GetInstance(base)][0] = handle;

    handle->callback = callback;
    handle->userData = userData;
    handle->base     = base;

    /* Set the isr pointer */
    s_saiTxIsr = SAI_TransferTxHandleIRQ;

    /* Enable Tx irq */
    EnableIRQ(s_saiTxIRQ[SAI_GetInstance(base)]);
}

/*!
 * brief Initializes the SAI Rx handle.
 *
 * This function initializes the Rx handle for the SAI Rx transactional APIs. Call
 * this function once to get the handle initialized.
 *
 * param base SAI base pointer.
 * param handle SAI handle pointer.
 * param callback Pointer to the user callback function.
 * param userData User parameter passed to the callback function.
 */
void SAI_TransferRxCreateHandle(I2S_Type *base, sai_handle_t *handle, sai_transfer_callback_t callback, void *userData)
{
    assert(handle);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    s_saiHandle[SAI_GetInstance(base)][1] = handle;

    handle->callback = callback;
    handle->userData = userData;
    handle->base     = base;

    /* Set the isr pointer */
    s_saiRxIsr = SAI_TransferRxHandleIRQ;

    /* Enable Rx irq */
    EnableIRQ(s_saiRxIRQ[SAI_GetInstance(base)]);
}

/*!
 * brief Configures the SAI Tx audio format.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_TxSetTransferConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base SAI base pointer.
 * param handle SAI handle pointer.
 * param format Pointer to the SAI audio data format structure.
 * param mclkSourceClockHz SAI master clock source frequency in Hz.
 * param bclkSourceClockHz SAI bit clock source frequency in Hz. If a bit clock source is a master
 * clock, this value should equal the masterClockHz in format.
 * return Status of this function. Return value is the status_t.
 */
status_t SAI_TransferTxSetFormat(I2S_Type *base,
                                 sai_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz)
{
    assert(handle);

    if ((bclkSourceClockHz < format->sampleRate_Hz)
#if defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)
        || (mclkSourceClockHz < format->sampleRate_Hz)
#endif
    )
    {
        return kStatus_InvalidArgument;
    }

    /* Copy format to handle */
    handle->bitWidth = format->bitWidth;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->watermark = format->watermark;
#endif

    SAI_TxSetFormat(base, format, mclkSourceClockHz, bclkSourceClockHz);

    handle->channel = format->channel;
    /* used for multi channel */
    handle->channelMask = format->channelMask;
    handle->channelNums = format->channelNums;
    handle->endChannel  = format->endChannel;

    return kStatus_Success;
}

/*!
 * brief Configures the SAI Rx audio format.
 *
 * deprecated Do not use this function.  It has been superceded by @ref SAI_RxSetTransferConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base SAI base pointer.
 * param handle SAI handle pointer.
 * param format Pointer to the SAI audio data format structure.
 * param mclkSourceClockHz SAI master clock source frequency in Hz.
 * param bclkSourceClockHz SAI bit clock source frequency in Hz. If a bit clock source is a master
 * clock, this value should equal the masterClockHz in format.
 * return Status of this function. Return value is one of status_t.
 */
status_t SAI_TransferRxSetFormat(I2S_Type *base,
                                 sai_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz)
{
    assert(handle);

    if ((bclkSourceClockHz < format->sampleRate_Hz)
#if defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)
        || (mclkSourceClockHz < format->sampleRate_Hz)
#endif
    )
    {
        return kStatus_InvalidArgument;
    }

    /* Copy format to handle */
    handle->bitWidth = format->bitWidth;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    handle->watermark = format->watermark;
#endif

    SAI_RxSetFormat(base, format, mclkSourceClockHz, bclkSourceClockHz);

    handle->channel = format->channel;
    /* used for multi channel */
    handle->channelMask = format->channelMask;
    handle->channelNums = format->channelNums;
    handle->endChannel  = format->endChannel;

    return kStatus_Success;
}

/*!
 * brief Performs an interrupt non-blocking send transfer on SAI.
 *
 * note This API returns immediately after the transfer initiates.
 * Call the SAI_TxGetTransferStatusIRQ to poll the transfer status and check whether
 * the transfer is finished. If the return status is not kStatus_SAI_Busy, the transfer
 * is finished.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * param xfer Pointer to the sai_transfer_t structure.
 * retval kStatus_Success Successfully started the data receive.
 * retval kStatus_SAI_TxBusy Previous receive still not finished.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SAI_TransferSendNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer)
{
    assert(handle);

    /* Check if the queue is full */
    if (handle->saiQueue[handle->queueUser].data)
    {
        return kStatus_SAI_QueueFull;
    }

    /* Add into queue */
    handle->transferSize[handle->queueUser]      = xfer->dataSize;
    handle->saiQueue[handle->queueUser].data     = xfer->data;
    handle->saiQueue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queueUser                            = (handle->queueUser + 1) % SAI_XFER_QUEUE_SIZE;

    /* Set the state to busy */
    handle->state = kSAI_Busy;

    /* Enable interrupt */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Use FIFO request interrupt and fifo error*/
    SAI_TxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFORequestInterruptEnable);
#else
    SAI_TxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFOWarningInterruptEnable);
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    /* Enable Tx transfer */
    SAI_TxEnable(base, true);

    return kStatus_Success;
}

/*!
 * brief Performs an interrupt non-blocking receive transfer on SAI.
 *
 * note This API returns immediately after the transfer initiates.
 * Call the SAI_RxGetTransferStatusIRQ to poll the transfer status and check whether
 * the transfer is finished. If the return status is not kStatus_SAI_Busy, the transfer
 * is finished.
 *
 * param base SAI base pointer
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * param xfer Pointer to the sai_transfer_t structure.
 * retval kStatus_Success Successfully started the data receive.
 * retval kStatus_SAI_RxBusy Previous receive still not finished.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SAI_TransferReceiveNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer)
{
    assert(handle);

    /* Check if the queue is full */
    if (handle->saiQueue[handle->queueUser].data)
    {
        return kStatus_SAI_QueueFull;
    }

    /* Add into queue */
    handle->transferSize[handle->queueUser]      = xfer->dataSize;
    handle->saiQueue[handle->queueUser].data     = xfer->data;
    handle->saiQueue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queueUser                            = (handle->queueUser + 1) % SAI_XFER_QUEUE_SIZE;

    /* Set state to busy */
    handle->state = kSAI_Busy;

/* Enable interrupt */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Use FIFO request interrupt and fifo error*/
    SAI_RxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFORequestInterruptEnable);
#else
    SAI_RxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFOWarningInterruptEnable);
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    /* Enable Rx transfer */
    SAI_RxEnable(base, true);

    return kStatus_Success;
}

/*!
 * brief Gets a set byte count.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * param count Bytes count sent.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SAI_TransferGetSendCount(I2S_Type *base, sai_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSAI_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] - handle->saiQueue[handle->queueDriver].dataSize);
    }

    return status;
}

/*!
 * brief Gets a received byte count.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * param count Bytes count received.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SAI_TransferGetReceiveCount(I2S_Type *base, sai_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSAI_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] - handle->saiQueue[handle->queueDriver].dataSize);
    }

    return status;
}

/*!
 * brief Aborts the current send.
 *
 * note This API can be called any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 */
void SAI_TransferAbortSend(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    /* Stop Tx transfer and disable interrupt */
    SAI_TxEnable(base, false);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Use FIFO request interrupt and fifo error */
    SAI_TxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFORequestInterruptEnable);
#else
    SAI_TxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFOWarningInterruptEnable);
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    handle->state = kSAI_Idle;

    /* Clear the queue */
    memset(handle->saiQueue, 0, sizeof(sai_transfer_t) * SAI_XFER_QUEUE_SIZE);
    handle->queueDriver = 0;
    handle->queueUser   = 0;
}

/*!
 * brief Aborts the current IRQ receive.
 *
 * note This API can be called when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * param base SAI base pointer
 * param handle Pointer to the sai_handle_t structure which stores the transfer state.
 */
void SAI_TransferAbortReceive(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    /* Stop Tx transfer and disable interrupt */
    SAI_RxEnable(base, false);
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    /* Use FIFO request interrupt and fifo error */
    SAI_RxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFORequestInterruptEnable);
#else
    SAI_RxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFOWarningInterruptEnable);
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    handle->state = kSAI_Idle;

    /* Clear the queue */
    memset(handle->saiQueue, 0, sizeof(sai_transfer_t) * SAI_XFER_QUEUE_SIZE);
    handle->queueDriver = 0;
    handle->queueUser   = 0;
}

/*!
 * brief Terminate all SAI send.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortSend.
 *
 * param base SAI base pointer.
 * param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateSend(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    /* Abort the current transfer */
    SAI_TransferAbortSend(base, handle);

    /* Clear all the internal information */
    memset(handle->saiQueue, 0U, sizeof(handle->saiQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;
}

/*!
 * brief Terminate all SAI receive.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortReceive.
 *
 * param base SAI base pointer.
 * param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateReceive(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    /* Abort the current transfer */
    SAI_TransferAbortReceive(base, handle);

    /* Clear all the internal information */
    memset(handle->saiQueue, 0U, sizeof(handle->saiQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;
}

/*!
 * brief Tx interrupt handler.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure.
 */
void SAI_TransferTxHandleIRQ(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    uint8_t *buffer  = handle->saiQueue[handle->queueDriver].data;
    uint8_t dataSize = (handle->bitWidth / 8U) * handle->channelNums;

    /* Handle Error */
    if (base->TCSR & I2S_TCSR_FEF_MASK)
    {
        /* Clear FIFO error flag to continue transfer */
        SAI_TxClearStatusFlags(base, kSAI_FIFOErrorFlag);

        /* Reset FIFO for safety */
        SAI_TxSoftwareReset(base, kSAI_ResetTypeFIFO);

        /* Call the callback */
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_SAI_TxError, handle->userData);
        }
    }

/* Handle transfer */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if (base->TCSR & I2S_TCSR_FRF_MASK)
    {
        /* Judge if the data need to transmit is less than space */
        uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize),
                           (size_t)((FSL_FEATURE_SAI_FIFO_COUNT - handle->watermark) * dataSize));

        /* Copy the data from sai buffer to FIFO */
        SAI_WriteNonBlocking(base, handle->channel, handle->channelMask, handle->endChannel, handle->bitWidth, buffer,
                             size);

        /* Update the internal counter */
        handle->saiQueue[handle->queueDriver].dataSize -= size;
        handle->saiQueue[handle->queueDriver].data += size;
    }
#else
    if (base->TCSR & I2S_TCSR_FWF_MASK)
    {
        uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), dataSize);

        SAI_WriteNonBlocking(base, handle->channel, handle->channelMask, handle->endChannel, handle->bitWidth, buffer,
                             size);

        /* Update internal counter */
        handle->saiQueue[handle->queueDriver].dataSize -= size;
        handle->saiQueue[handle->queueDriver].data += size;
    }
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    /* If finished a block, call the callback function */
    if (handle->saiQueue[handle->queueDriver].dataSize == 0U)
    {
        memset(&handle->saiQueue[handle->queueDriver], 0, sizeof(sai_transfer_t));
        handle->queueDriver = (handle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_SAI_TxIdle, handle->userData);
        }
    }

    /* If all data finished, just stop the transfer */
    if (handle->saiQueue[handle->queueDriver].data == NULL)
    {
        SAI_TransferAbortSend(base, handle);
    }
}

/*!
 * brief Tx interrupt handler.
 *
 * param base SAI base pointer.
 * param handle Pointer to the sai_handle_t structure.
 */
void SAI_TransferRxHandleIRQ(I2S_Type *base, sai_handle_t *handle)
{
    assert(handle);

    uint8_t *buffer  = handle->saiQueue[handle->queueDriver].data;
    uint8_t dataSize = (handle->bitWidth / 8U) * handle->channelNums;

    /* Handle Error */
    if (base->RCSR & I2S_RCSR_FEF_MASK)
    {
        /* Clear FIFO error flag to continue transfer */
        SAI_RxClearStatusFlags(base, kSAI_FIFOErrorFlag);

        /* Reset FIFO for safety */
        SAI_RxSoftwareReset(base, kSAI_ResetTypeFIFO);

        /* Call the callback */
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_SAI_RxError, handle->userData);
        }
    }

/* Handle transfer */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if (base->RCSR & I2S_RCSR_FRF_MASK)
    {
        /* Judge if the data need to transmit is less than space */
        uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), (handle->watermark * dataSize));

        /* Copy the data from sai buffer to FIFO */
        SAI_ReadNonBlocking(base, handle->channel, handle->channelMask, handle->endChannel, handle->bitWidth, buffer,
                            size);

        /* Update the internal counter */
        handle->saiQueue[handle->queueDriver].dataSize -= size;
        handle->saiQueue[handle->queueDriver].data += size;
    }
#else
    if (base->RCSR & I2S_RCSR_FWF_MASK)
    {
        uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), dataSize);

        SAI_ReadNonBlocking(base, handle->channel, handle->channelMask, handle->endChannel, handle->bitWidth, buffer,
                            size);

        /* Update internal state */
        handle->saiQueue[handle->queueDriver].dataSize -= size;
        handle->saiQueue[handle->queueDriver].data += size;
    }
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

    /* If finished a block, call the callback function */
    if (handle->saiQueue[handle->queueDriver].dataSize == 0U)
    {
        memset(&handle->saiQueue[handle->queueDriver], 0, sizeof(sai_transfer_t));
        handle->queueDriver = (handle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_SAI_RxIdle, handle->userData);
        }
    }

    /* If all data finished, just stop the transfer */
    if (handle->saiQueue[handle->queueDriver].data == NULL)
    {
        SAI_TransferAbortReceive(base, handle);
    }
}

#if defined(I2S0)
void I2S0_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][1]) && ((I2S0->RCSR & kSAI_FIFORequestFlag) || (I2S0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S0->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][1]) && ((I2S0->RCSR & kSAI_FIFOWarningFlag) || (I2S0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S0->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S0, s_saiHandle[0][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][0]) && ((I2S0->TCSR & kSAI_FIFORequestFlag) || (I2S0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S0->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][0]) && ((I2S0->TCSR & kSAI_FIFOWarningFlag) || (I2S0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S0->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S0, s_saiHandle[0][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S0_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[0][0]);
    s_saiTxIsr(I2S0, s_saiHandle[0][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S0_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[0][1]);
    s_saiRxIsr(I2S0, s_saiHandle[0][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* I2S0*/

#if defined(I2S1)
void I2S1_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((I2S1->RCSR & kSAI_FIFORequestFlag) || (I2S1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S1->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((I2S1->RCSR & kSAI_FIFOWarningFlag) || (I2S1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S1->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S1, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((I2S1->TCSR & kSAI_FIFORequestFlag) || (I2S1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S1->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((I2S1->TCSR & kSAI_FIFOWarningFlag) || (I2S1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S1->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S1, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S1_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[1][0]);
    s_saiTxIsr(I2S1, s_saiHandle[1][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S1_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[1][1]);
    s_saiRxIsr(I2S1, s_saiHandle[1][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* I2S1*/

#if defined(I2S2)
void I2S2_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][1]) && ((I2S2->RCSR & kSAI_FIFORequestFlag) || (I2S2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S2->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][1]) && ((I2S2->RCSR & kSAI_FIFOWarningFlag) || (I2S2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S2->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S2, s_saiHandle[2][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][0]) && ((I2S2->TCSR & kSAI_FIFORequestFlag) || (I2S2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S2->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][0]) && ((I2S2->TCSR & kSAI_FIFOWarningFlag) || (I2S2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S2->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S2, s_saiHandle[2][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S2_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[2][0]);
    s_saiTxIsr(I2S2, s_saiHandle[2][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S2_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[2][1]);
    s_saiRxIsr(I2S2, s_saiHandle[2][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* I2S2*/

#if defined(I2S3)
void I2S3_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][1]) && ((I2S3->RCSR & kSAI_FIFORequestFlag) || (I2S3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S3->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][1]) && ((I2S3->RCSR & kSAI_FIFOWarningFlag) || (I2S3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S3->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S3, s_saiHandle[3][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][0]) && ((I2S3->TCSR & kSAI_FIFORequestFlag) || (I2S3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S3->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][0]) && ((I2S3->TCSR & kSAI_FIFOWarningFlag) || (I2S3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S3->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S3, s_saiHandle[3][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S3_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[3][0]);
    s_saiTxIsr(I2S3, s_saiHandle[3][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S3_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[3][1]);
    s_saiRxIsr(I2S3, s_saiHandle[3][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* I2S3*/

#if defined(I2S4)
void I2S4_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[4][1]) && ((I2S4->RCSR & kSAI_FIFORequestFlag) || (I2S4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S4->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[4][1]) && ((I2S4->RCSR & kSAI_FIFOWarningFlag) || (I2S4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S4->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S4, s_saiHandle[4][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[4][0]) && ((I2S4->TCSR & kSAI_FIFORequestFlag) || (I2S4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S4->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[4][0]) && ((I2S4->TCSR & kSAI_FIFOWarningFlag) || (I2S4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S4->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S4, s_saiHandle[4][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S4_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[4][0]);
    s_saiTxIsr(I2S4, s_saiHandle[4][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S4_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[4][1]);
    s_saiRxIsr(I2S4, s_saiHandle[4][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(FSL_FEATURE_SAI_SAI5_SAI6_SHARE_IRQ) && (FSL_FEATURE_SAI_SAI5_SAI6_SHARE_IRQ) && defined(I2S5) && \
    defined(I2S6)
void I2S56_DriverIRQHandler(void)
{
    /* use index 5 to get handle when I2S5 & I2S6 share IRQ NUMBER */
    I2S_Type *base = s_saiHandle[5][1]->base;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][1]) && base && ((base->RCSR & kSAI_FIFORequestFlag) || (base->RCSR & kSAI_FIFOErrorFlag)) &&
        ((base->RCSR & kSAI_FIFORequestInterruptEnable) || (base->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][1]) && base && ((base->RCSR & kSAI_FIFOWarningFlag) || (base->RCSR & kSAI_FIFOErrorFlag)) &&
        ((base->RCSR & kSAI_FIFOWarningInterruptEnable) || (base->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(base, s_saiHandle[5][1]);
    }

    base = s_saiHandle[5][0]->base;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][0]) && base && ((base->TCSR & kSAI_FIFORequestFlag) || (base->TCSR & kSAI_FIFOErrorFlag)) &&
        ((base->TCSR & kSAI_FIFORequestInterruptEnable) || (base->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][0]) && base && ((base->TCSR & kSAI_FIFOWarningFlag) || (base->TCSR & kSAI_FIFOErrorFlag)) &&
        ((base->TCSR & kSAI_FIFOWarningInterruptEnable) || (base->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(base, s_saiHandle[5][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S56_Tx_DriverIRQHandler(void)
{
    /* use index 5 to get handle when I2S5 & I2S6 share IRQ NUMBER */
    assert(s_saiHandle[5][0]);
    s_saiTxIsr(s_saiHandle[5][0]->base, s_saiHandle[5][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S56_Rx_DriverIRQHandler(void)
{
    /* use index 5 to get handle when I2S5 & I2S6 share IRQ NUMBER */
    assert(s_saiHandle[5][1]);
    s_saiRxIsr(s_saiHandle[5][1]->base, s_saiHandle[5][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#else

#if defined(I2S5)
void I2S5_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][1]) && ((I2S5->RCSR & kSAI_FIFORequestFlag) || (I2S5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S5->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][1]) && ((I2S5->RCSR & kSAI_FIFOWarningFlag) || (I2S5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S5->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S5, s_saiHandle[5][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][0]) && ((I2S5->TCSR & kSAI_FIFORequestFlag) || (I2S5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S5->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][0]) && ((I2S5->TCSR & kSAI_FIFOWarningFlag) || (I2S5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S5->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S5, s_saiHandle[5][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S5_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[5][0]);
    s_saiTxIsr(I2S5, s_saiHandle[5][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S5_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[5][1]);
    s_saiRxIsr(I2S5, s_saiHandle[5][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(I2S6)
void I2S6_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][1]) && ((I2S6->RCSR & kSAI_FIFORequestFlag) || (I2S6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S6->RCSR & kSAI_FIFORequestInterruptEnable) || (I2S6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][1]) && ((I2S6->RCSR & kSAI_FIFOWarningFlag) || (I2S6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S6->RCSR & kSAI_FIFOWarningInterruptEnable) || (I2S6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(I2S6, s_saiHandle[6][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][0]) && ((I2S6->TCSR & kSAI_FIFORequestFlag) || (I2S6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S6->TCSR & kSAI_FIFORequestInterruptEnable) || (I2S6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][0]) && ((I2S6->TCSR & kSAI_FIFOWarningFlag) || (I2S6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((I2S6->TCSR & kSAI_FIFOWarningInterruptEnable) || (I2S6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(I2S6, s_saiHandle[6][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S6_Tx_DriverIRQHandler(void)
{
    assert(s_saiHandle[6][0]);
    s_saiTxIsr(I2S6, s_saiHandle[6][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void I2S6_Rx_DriverIRQHandler(void)
{
    assert(s_saiHandle[6][1]);
    s_saiRxIsr(I2S6, s_saiHandle[6][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(AUDIO__SAI0)
void AUDIO_SAI0_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][1]) &&
        ((AUDIO__SAI0->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI0->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][1]) &&
        ((AUDIO__SAI0->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI0->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI0, s_saiHandle[0][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][0]) &&
        ((AUDIO__SAI0->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI0->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][0]) &&
        ((AUDIO__SAI0->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI0->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI0, s_saiHandle[0][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* AUDIO__SAI0 */

#if defined(AUDIO__SAI1)
void AUDIO_SAI1_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) &&
        ((AUDIO__SAI1->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI1->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) &&
        ((AUDIO__SAI1->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI1->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI1, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) &&
        ((AUDIO__SAI1->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI1->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) &&
        ((AUDIO__SAI1->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI1->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI1, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* AUDIO__SAI1 */

#if defined(AUDIO__SAI2)
void AUDIO_SAI2_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][1]) &&
        ((AUDIO__SAI2->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI2->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][1]) &&
        ((AUDIO__SAI2->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI2->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI2, s_saiHandle[2][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][0]) &&
        ((AUDIO__SAI2->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI2->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][0]) &&
        ((AUDIO__SAI2->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI2->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI2, s_saiHandle[2][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* AUDIO__SAI2 */

#if defined(AUDIO__SAI3)
void AUDIO_SAI3_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][1]) &&
        ((AUDIO__SAI3->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI3->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][1]) &&
        ((AUDIO__SAI3->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI3->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI3, s_saiHandle[3][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][0]) &&
        ((AUDIO__SAI3->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI3->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][0]) &&
        ((AUDIO__SAI3->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI3->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI3, s_saiHandle[3][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(AUDIO__SAI6)
void AUDIO_SAI6_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][1]) &&
        ((AUDIO__SAI6->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI6->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][1]) &&
        ((AUDIO__SAI6->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI6->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI6, s_saiHandle[6][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][0]) &&
        ((AUDIO__SAI6->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI6->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][0]) &&
        ((AUDIO__SAI6->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI6->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI6, s_saiHandle[6][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* AUDIO__SAI6 */

#if defined(AUDIO__SAI7)
void AUDIO_SAI7_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[7][1]) &&
        ((AUDIO__SAI7->RCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI7->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI7->RCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI7->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[7][1]) &&
        ((AUDIO__SAI7->RCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI7->RCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI7->RCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI7->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(AUDIO__SAI7, s_saiHandle[7][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[7][0]) &&
        ((AUDIO__SAI7->TCSR & kSAI_FIFORequestFlag) || (AUDIO__SAI7->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI7->TCSR & kSAI_FIFORequestInterruptEnable) || (AUDIO__SAI7->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[7][0]) &&
        ((AUDIO__SAI7->TCSR & kSAI_FIFOWarningFlag) || (AUDIO__SAI7->TCSR & kSAI_FIFOErrorFlag)) &&
        ((AUDIO__SAI7->TCSR & kSAI_FIFOWarningInterruptEnable) || (AUDIO__SAI7->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI7, s_saiHandle[7][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* AUDIO__SAI7 */

#if defined(ADMA__SAI0)
void ADMA_SAI0_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI0->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI0->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI0->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI0->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI0, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI0->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI0->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI0->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI0->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI0, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI0 */

#if defined(ADMA__SAI1)
void ADMA_SAI1_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI1->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI1->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI1->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI1->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI1, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI1->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI1->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI1->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI1->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI1, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI1 */

#if defined(ADMA__SAI2)
void ADMA_SAI2_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI2->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI2->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI2->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI2->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI2, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI2->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI2->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI2->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI2->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI2, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI2 */

#if defined(ADMA__SAI3)
void ADMA_SAI3_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI3->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI3->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI3->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI3->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI3, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI3->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI3->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI3->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI3->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI3, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI3 */

#if defined(ADMA__SAI4)
void ADMA_SAI4_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI4->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI4->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI4->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI4->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI4, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI4->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI4->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI4->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI4->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI4, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI4 */

#if defined(ADMA__SAI5)
void ADMA_SAI5_INT_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((ADMA__SAI5->RCSR & kSAI_FIFORequestFlag) || (ADMA__SAI5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI5->RCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((ADMA__SAI5->RCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI5->RCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(ADMA__SAI5, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((ADMA__SAI5->TCSR & kSAI_FIFORequestFlag) || (ADMA__SAI5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI5->TCSR & kSAI_FIFORequestInterruptEnable) || (ADMA__SAI5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((ADMA__SAI5->TCSR & kSAI_FIFOWarningFlag) || (ADMA__SAI5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((ADMA__SAI5->TCSR & kSAI_FIFOWarningInterruptEnable) || (ADMA__SAI5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(ADMA__SAI5, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* ADMA__SAI5 */

#if defined(SAI0)
void SAI0_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][1]) && ((SAI0->RCSR & kSAI_FIFORequestFlag) || (SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI0->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][1]) && ((SAI0->RCSR & kSAI_FIFOWarningFlag) || (SAI0->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI0->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI0->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI0, s_saiHandle[0][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[0][0]) && ((SAI0->TCSR & kSAI_FIFORequestFlag) || (SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI0->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[0][0]) && ((SAI0->TCSR & kSAI_FIFOWarningFlag) || (SAI0->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI0->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI0->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(AUDIO__SAI0, s_saiHandle[0][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI0 */

#if defined(SAI1)
void SAI1_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][1]) && ((SAI1->RCSR & kSAI_FIFORequestFlag) || (SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI1->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][1]) && ((SAI1->RCSR & kSAI_FIFOWarningFlag) || (SAI1->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI1->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI1->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI1, s_saiHandle[1][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[1][0]) && ((SAI1->TCSR & kSAI_FIFORequestFlag) || (SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI1->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[1][0]) && ((SAI1->TCSR & kSAI_FIFOWarningFlag) || (SAI1->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI1->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI1->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI1, s_saiHandle[1][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI1 */

#if defined(SAI2)
void SAI2_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][1]) && ((SAI2->RCSR & kSAI_FIFORequestFlag) || (SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI2->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][1]) && ((SAI2->RCSR & kSAI_FIFOWarningFlag) || (SAI2->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI2->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI2->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI2, s_saiHandle[2][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[2][0]) && ((SAI2->TCSR & kSAI_FIFORequestFlag) || (SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI2->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[2][0]) && ((SAI2->TCSR & kSAI_FIFOWarningFlag) || (SAI2->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI2->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI2->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI2, s_saiHandle[2][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI2 */

#if defined(SAI3)
void SAI3_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][1]) && ((SAI3->RCSR & kSAI_FIFORequestFlag) || (SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI3->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][1]) && ((SAI3->RCSR & kSAI_FIFOWarningFlag) || (SAI3->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI3->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI3->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI3, s_saiHandle[3][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[3][0]) && ((SAI3->TCSR & kSAI_FIFORequestFlag) || (SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI3->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[3][0]) && ((SAI3->TCSR & kSAI_FIFOWarningFlag) || (SAI3->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI3->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI3->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI3, s_saiHandle[3][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void SAI3_TX_DriverIRQHandler(void)
{
    assert(s_saiHandle[3][0]);
    s_saiTxIsr(SAI3, s_saiHandle[3][0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void SAI3_RX_DriverIRQHandler(void)
{
    assert(s_saiHandle[3][1]);
    s_saiRxIsr(SAI3, s_saiHandle[3][1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI3 */

#if defined(SAI4)
void SAI4_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[4][1]) && ((SAI4->RCSR & kSAI_FIFORequestFlag) || (SAI4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI4->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[4][1]) && ((SAI4->RCSR & kSAI_FIFOWarningFlag) || (SAI4->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI4->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI4->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI4, s_saiHandle[4][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[4][0]) && ((SAI4->TCSR & kSAI_FIFORequestFlag) || (SAI4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI4->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[4][0]) && ((SAI4->TCSR & kSAI_FIFOWarningFlag) || (SAI4->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI4->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI4->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI4, s_saiHandle[4][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI4 */

#if defined(SAI5)
void SAI5_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][1]) && ((SAI5->RCSR & kSAI_FIFORequestFlag) || (SAI5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI5->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][1]) && ((SAI5->RCSR & kSAI_FIFOWarningFlag) || (SAI5->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI5->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI5->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI5, s_saiHandle[5][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[5][0]) && ((SAI5->TCSR & kSAI_FIFORequestFlag) || (SAI5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI5->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[5][0]) && ((SAI5->TCSR & kSAI_FIFOWarningFlag) || (SAI5->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI5->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI5->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI5, s_saiHandle[5][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI5 */

#if defined(SAI6)
void SAI6_DriverIRQHandler(void)
{
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][1]) && ((SAI6->RCSR & kSAI_FIFORequestFlag) || (SAI6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI6->RCSR & kSAI_FIFORequestInterruptEnable) || (SAI6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][1]) && ((SAI6->RCSR & kSAI_FIFOWarningFlag) || (SAI6->RCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI6->RCSR & kSAI_FIFOWarningInterruptEnable) || (SAI6->RCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiRxIsr(SAI6, s_saiHandle[6][1]);
    }
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    if ((s_saiHandle[6][0]) && ((SAI6->TCSR & kSAI_FIFORequestFlag) || (SAI6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI6->TCSR & kSAI_FIFORequestInterruptEnable) || (SAI6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#else
    if ((s_saiHandle[6][0]) && ((SAI6->TCSR & kSAI_FIFOWarningFlag) || (SAI6->TCSR & kSAI_FIFOErrorFlag)) &&
        ((SAI6->TCSR & kSAI_FIFOWarningInterruptEnable) || (SAI6->TCSR & kSAI_FIFOErrorInterruptEnable)))
#endif
    {
        s_saiTxIsr(SAI6, s_saiHandle[6][0]);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* SAI6 */
