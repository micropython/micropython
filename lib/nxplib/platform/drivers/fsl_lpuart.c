/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpuart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpuart"
#endif

/* LPUART transfer state. */
enum _lpuart_transfer_states
{
    kLPUART_TxIdle, /*!< TX idle. */
    kLPUART_TxBusy, /*!< TX busy. */
    kLPUART_RxIdle, /*!< RX idle. */
    kLPUART_RxBusy  /*!< RX busy. */
};

/* Typedef for interrupt handler. */
typedef void (*lpuart_isr_t)(LPUART_Type *base, lpuart_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Check whether the RX ring buffer is full.
 *
 * @userData handle LPUART handle pointer.
 * @retval true  RX ring buffer is full.
 * @retval false RX ring buffer is not full.
 */
static bool LPUART_TransferIsRxRingBufferFull(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief Write to TX register using non-blocking method.
 *
 * This function writes data to the TX register directly, upper layer must make
 * sure the TX register is empty or TX FIFO has empty room before calling this function.
 *
 * @note This function does not check whether all the data has been sent out to bus,
 * so before disable TX, check kLPUART_TransmissionCompleteFlag to ensure the TX is
 * finished.
 *
 * @param base LPUART peripheral base address.
 * @param data Start address of the data to write.
 * @param length Size of the buffer to be sent.
 */
static void LPUART_WriteNonBlocking(LPUART_Type *base, const uint8_t *data, size_t length);

/*!
 * @brief Read RX register using non-blocking method.
 *
 * This function reads data from the TX register directly, upper layer must make
 * sure the RX register is full or TX FIFO has data before calling this function.
 *
 * @param base LPUART peripheral base address.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 */
static void LPUART_ReadNonBlocking(LPUART_Type *base, uint8_t *data, size_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of LPUART peripheral base address. */
static LPUART_Type *const s_lpuartBases[] = LPUART_BASE_PTRS;
/* Array of LPUART handle. */
static lpuart_handle_t *s_lpuartHandle[ARRAY_SIZE(s_lpuartBases)];
/* Array of LPUART IRQ number. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
static const IRQn_Type s_lpuartRxIRQ[] = LPUART_RX_IRQS;
static const IRQn_Type s_lpuartTxIRQ[] = LPUART_TX_IRQS;
#else
static const IRQn_Type s_lpuartIRQ[] = LPUART_RX_TX_IRQS;
#endif
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of LPUART clock name. */
static const clock_ip_name_t s_lpuartClock[] = LPUART_CLOCKS;

#if defined(LPUART_PERIPH_CLOCKS)
/* Array of LPUART functional clock name. */
static const clock_ip_name_t s_lpuartPeriphClocks[] = LPUART_PERIPH_CLOCKS;
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* LPUART ISR for transactional APIs. */
static lpuart_isr_t s_lpuartIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Get the LPUART instance from peripheral base address.
 *
 * param base LPUART peripheral base address.
 * return LPUART instance.
 */
uint32_t LPUART_GetInstance(LPUART_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_lpuartBases); instance++)
    {
        if (s_lpuartBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_lpuartBases));

    return instance;
}

/*!
 * brief Get the length of received data in RX ring buffer.
 *
 * userData handle LPUART handle pointer.
 * return Length of received data in RX ring buffer.
 */
size_t LPUART_TransferGetRxRingBufferLength(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    size_t size;
    size_t tmpRxRingBufferSize   = handle->rxRingBufferSize;
    uint16_t tmpRxRingBufferTail = handle->rxRingBufferTail;
    uint16_t tmpRxRingBufferHead = handle->rxRingBufferHead;

    if (tmpRxRingBufferTail > tmpRxRingBufferHead)
    {
        size = ((size_t)tmpRxRingBufferHead + tmpRxRingBufferSize - (size_t)tmpRxRingBufferTail);
    }
    else
    {
        size = ((size_t)tmpRxRingBufferHead - (size_t)tmpRxRingBufferTail);
    }

    return size;
}

static bool LPUART_TransferIsRxRingBufferFull(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    bool full;

    if (LPUART_TransferGetRxRingBufferLength(base, handle) == (handle->rxRingBufferSize - 1U))
    {
        full = true;
    }
    else
    {
        full = false;
    }
    return full;
}

static void LPUART_WriteNonBlocking(LPUART_Type *base, const uint8_t *data, size_t length)
{
    assert(NULL != data);

    size_t i;

    /* The Non Blocking write data API assume user have ensured there is enough space in
    peripheral to write. */
    for (i = 0; i < length; i++)
    {
        base->DATA = data[i];
    }
}

static void LPUART_ReadNonBlocking(LPUART_Type *base, uint8_t *data, size_t length)
{
    assert(NULL != data);

    size_t i;
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    uint32_t ctrl = base->CTRL;
    bool isSevenDataBits =
        ((ctrl & LPUART_CTRL_M7_MASK) || ((!(ctrl & LPUART_CTRL_M_MASK)) && (ctrl & LPUART_CTRL_PE_MASK)));
#endif

    /* The Non Blocking read data API assume user have ensured there is enough space in
    peripheral to write. */
    for (i = 0; i < length; i++)
    {
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
        if (isSevenDataBits)
        {
            data[i] = (base->DATA & 0x7F);
        }
        else
        {
            data[i] = base->DATA;
        }
#else
        data[i] = (uint8_t)(base->DATA);
#endif
    }
}

/*!
 * brief Initializes an LPUART instance with the user configuration structure and the peripheral clock.
 *
 * This function configures the LPUART module with user-defined settings. Call the LPUART_GetDefaultConfig() function
 * to configure the configuration structure and get the default configuration.
 * The example below shows how to use this API to configure the LPUART.
 * code
 *  lpuart_config_t lpuartConfig;
 *  lpuartConfig.baudRate_Bps = 115200U;
 *  lpuartConfig.parityMode = kLPUART_ParityDisabled;
 *  lpuartConfig.dataBitsCount = kLPUART_EightDataBits;
 *  lpuartConfig.isMsb = false;
 *  lpuartConfig.stopBitCount = kLPUART_OneStopBit;
 *  lpuartConfig.txFifoWatermark = 0;
 *  lpuartConfig.rxFifoWatermark = 1;
 *  LPUART_Init(LPUART1, &lpuartConfig, 20000000U);
 * endcode
 *
 * param base LPUART peripheral base address.
 * param config Pointer to a user-defined configuration structure.
 * param srcClock_Hz LPUART clock source frequency in HZ.
 * retval kStatus_LPUART_BaudrateNotSupport Baudrate is not support in current clock source.
 * retval kStatus_Success LPUART initialize succeed
 */
status_t LPUART_Init(LPUART_Type *base, const lpuart_config_t *config, uint32_t srcClock_Hz)
{
    assert(NULL != config);
    assert(0U < config->baudRate_Bps);
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    assert((uint8_t)FSL_FEATURE_LPUART_FIFO_SIZEn(base) >= config->txFifoWatermark);
    assert((uint8_t)FSL_FEATURE_LPUART_FIFO_SIZEn(base) >= config->rxFifoWatermark);
#endif

    uint32_t temp;
    uint16_t sbr, sbrTemp;
    uint32_t osr, osrTemp, tempDiff, calculatedBaud, baudDiff;

    /* This LPUART instantiation uses a slightly different baud rate calculation
     * The idea is to use the best OSR (over-sampling rate) possible
     * Note, OSR is typically hard-set to 16 in other LPUART instantiations
     * loop to find the best OSR value possible, one that generates minimum baudDiff
     * iterate through the rest of the supported values of OSR */

    baudDiff = config->baudRate_Bps;
    osr      = 0;
    sbr      = 0;
    for (osrTemp = 4U; osrTemp <= 32U; osrTemp++)
    {
        /* calculate the temporary sbr value   */
        sbrTemp = (uint16_t)((srcClock_Hz / (config->baudRate_Bps * osrTemp)));
        /*set sbrTemp to 1 if the sourceClockInHz can not satisfy the desired baud rate*/
        if (sbrTemp == 0U)
        {
            sbrTemp = 1U;
        }
        /* Calculate the baud rate based on the temporary OSR and SBR values */
        calculatedBaud = (srcClock_Hz / (osrTemp * sbrTemp));

        tempDiff = calculatedBaud - config->baudRate_Bps;

        /* Select the better value between srb and (sbr + 1) */
        if (tempDiff > (config->baudRate_Bps - (srcClock_Hz / (osrTemp * ((uint32_t)sbrTemp + 1U)))))
        {
            tempDiff = config->baudRate_Bps - (srcClock_Hz / (osrTemp * ((uint32_t)sbrTemp + 1U)));
            sbrTemp++;
        }

        if (tempDiff <= baudDiff)
        {
            baudDiff = tempDiff;
            osr      = osrTemp; /* update and store the best OSR value calculated */
            sbr      = sbrTemp; /* update store the best SBR value calculated */
        }
    }

    /* Check to see if actual baud rate is within 3% of desired baud rate
     * based on the best calculate OSR value */
    if (baudDiff > ((config->baudRate_Bps / 100U) * 3U))
    {
        /* Unacceptable baud rate difference of more than 3%*/
        return kStatus_LPUART_BaudrateNotSupport;
    }

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)

    uint32_t instance = LPUART_GetInstance(base);

    /* Enable lpuart clock */
    CLOCK_EnableClock(s_lpuartClock[instance]);
#if defined(LPUART_PERIPH_CLOCKS)
    CLOCK_EnableClock(s_lpuartPeriphClocks[instance]);
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if defined(FSL_FEATURE_LPUART_HAS_GLOBAL) && FSL_FEATURE_LPUART_HAS_GLOBAL
    /*Reset all internal logic and registers, except the Global Register */
    LPUART_SoftwareReset(base);
#else
    /* Disable LPUART TX RX before setting. */
    base->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
#endif

    temp = base->BAUD;

    /* Acceptable baud rate, check if OSR is between 4x and 7x oversampling.
     * If so, then "BOTHEDGE" sampling must be turned on */
    if ((osr > 3U) && (osr < 8U))
    {
        temp |= LPUART_BAUD_BOTHEDGE_MASK;
    }

    /* program the osr value (bit value is one less than actual value) */
    temp &= ~LPUART_BAUD_OSR_MASK;
    temp |= LPUART_BAUD_OSR(osr - 1U);

    /* write the sbr value to the BAUD registers */
    temp &= ~LPUART_BAUD_SBR_MASK;
    base->BAUD = temp | LPUART_BAUD_SBR(sbr);

    /* Set bit count and parity mode. */
    base->BAUD &= ~LPUART_BAUD_M10_MASK;

    temp = base->CTRL & ~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK | LPUART_CTRL_M_MASK | LPUART_CTRL_ILT_MASK |
                          LPUART_CTRL_IDLECFG_MASK);

    temp |=
        (uint8_t)config->parityMode | LPUART_CTRL_IDLECFG(config->rxIdleConfig) | LPUART_CTRL_ILT(config->rxIdleType);

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    if (kLPUART_SevenDataBits == config->dataBitsCount)
    {
        if (kLPUART_ParityDisabled != config->parityMode)
        {
            temp &= ~LPUART_CTRL_M7_MASK; /* Seven data bits and one parity bit */
        }
        else
        {
            temp |= LPUART_CTRL_M7_MASK;
        }
    }
    else
#endif
    {
        if (kLPUART_ParityDisabled != config->parityMode)
        {
            temp |= LPUART_CTRL_M_MASK; /* Eight data bits and one parity bit */
        }
    }

    base->CTRL = temp;

#if defined(FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT
    /* set stop bit per char */
    temp       = base->BAUD & ~LPUART_BAUD_SBNS_MASK;
    base->BAUD = temp | LPUART_BAUD_SBNS((uint8_t)config->stopBitCount);
#endif

#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    /* Set tx/rx WATER watermark
       Note:
       Take care of the RX FIFO, RX interrupt request only assert when received bytes
       equal or more than RX water mark, there is potential issue if RX water
       mark larger than 1.
       For example, if RX FIFO water mark is 2, upper layer needs 5 bytes and
       5 bytes are received. the last byte will be saved in FIFO but not trigger
       RX interrupt because the water mark is 2.
     */
    base->WATER = (((uint32_t)(config->rxFifoWatermark) << 16U) | config->txFifoWatermark);

    /* Enable tx/rx FIFO */
    base->FIFO |= (LPUART_FIFO_TXFE_MASK | LPUART_FIFO_RXFE_MASK);

    /* Flush FIFO */
    base->FIFO |= (LPUART_FIFO_TXFLUSH_MASK | LPUART_FIFO_RXFLUSH_MASK);
#endif

    /* Clear all status flags */
    temp = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);

#if defined(FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT
    temp |= LPUART_STAT_LBKDIF_MASK;
#endif

#if defined(FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING) && FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING
    temp |= (LPUART_STAT_MA1F_MASK | LPUART_STAT_MA2F_MASK);
#endif

#if defined(FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT) && FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT
    /* Set the CTS configuration/TX CTS source. */
    base->MODIR |= LPUART_MODIR_TXCTSC(config->txCtsConfig) | LPUART_MODIR_TXCTSSRC(config->txCtsSource);
    if (true == config->enableRxRTS)
    {
        /* Enable the receiver RTS(request-to-send) function. */
        base->MODIR |= LPUART_MODIR_RXRTSE_MASK;
    }
    if (true == config->enableTxCTS)
    {
        /* Enable the CTS(clear-to-send) function. */
        base->MODIR |= LPUART_MODIR_TXCTSE_MASK;
    }
#endif

    /* Set data bits order. */
    if (true == config->isMsb)
    {
        temp |= LPUART_STAT_MSBF_MASK;
    }
    else
    {
        temp &= ~LPUART_STAT_MSBF_MASK;
    }

    base->STAT |= temp;

    /* Enable TX/RX base on configure structure. */
    temp = base->CTRL;
    if (true == config->enableTx)
    {
        temp |= LPUART_CTRL_TE_MASK;
    }

    if (true == config->enableRx)
    {
        temp |= LPUART_CTRL_RE_MASK;
    }

    base->CTRL = temp;

    return kStatus_Success;
}
/*!
 * brief Deinitializes a LPUART instance.
 *
 * This function waits for transmit to complete, disables TX and RX, and disables the LPUART clock.
 *
 * param base LPUART peripheral base address.
 */
void LPUART_Deinit(LPUART_Type *base)
{
    uint32_t temp;

#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    /* Wait tx FIFO send out*/
    while (0U != ((base->WATER & LPUART_WATER_TXCOUNT_MASK) >> LPUART_WATER_TXWATER_SHIFT))
    {
    }
#endif
    /* Wait last char shift out */
    while (0U == (base->STAT & LPUART_STAT_TC_MASK))
    {
    }

    /* Clear all status flags */
    temp = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);

#if defined(FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT
    temp |= LPUART_STAT_LBKDIF_MASK;
#endif

#if defined(FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING) && FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING
    temp |= (LPUART_STAT_MA1F_MASK | LPUART_STAT_MA2F_MASK);
#endif

    base->STAT |= temp;

    /* Disable the module. */
    base->CTRL = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = LPUART_GetInstance(base);

    /* Disable lpuart clock */
    CLOCK_DisableClock(s_lpuartClock[instance]);

#if defined(LPUART_PERIPH_CLOCKS)
    CLOCK_DisableClock(s_lpuartPeriphClocks[instance]);
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Gets the default configuration structure.
 *
 * This function initializes the LPUART configuration structure to a default value. The default
 * values are:
 *   lpuartConfig->baudRate_Bps = 115200U;
 *   lpuartConfig->parityMode = kLPUART_ParityDisabled;
 *   lpuartConfig->dataBitsCount = kLPUART_EightDataBits;
 *   lpuartConfig->isMsb = false;
 *   lpuartConfig->stopBitCount = kLPUART_OneStopBit;
 *   lpuartConfig->txFifoWatermark = 0;
 *   lpuartConfig->rxFifoWatermark = 1;
 *   lpuartConfig->rxIdleType = kLPUART_IdleTypeStartBit;
 *   lpuartConfig->rxIdleConfig = kLPUART_IdleCharacter1;
 *   lpuartConfig->enableTx = false;
 *   lpuartConfig->enableRx = false;
 *
 * param config Pointer to a configuration structure.
 */
void LPUART_GetDefaultConfig(lpuart_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    config->baudRate_Bps  = 115200U;
    config->parityMode    = kLPUART_ParityDisabled;
    config->dataBitsCount = kLPUART_EightDataBits;
    config->isMsb         = false;
#if defined(FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT
    config->stopBitCount = kLPUART_OneStopBit;
#endif
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    config->txFifoWatermark = 0;
    config->rxFifoWatermark = 0;
#endif
#if defined(FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT) && FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT
    config->enableRxRTS = false;
    config->enableTxCTS = false;
    config->txCtsConfig = kLPUART_CtsSampleAtStart;
    config->txCtsSource = kLPUART_CtsSourcePin;
#endif
    config->rxIdleType   = kLPUART_IdleTypeStartBit;
    config->rxIdleConfig = kLPUART_IdleCharacter1;
    config->enableTx     = false;
    config->enableRx     = false;
}

/*!
 * brief Sets the LPUART instance baudrate.
 *
 * This function configures the LPUART module baudrate. This function is used to update
 * the LPUART module baudrate after the LPUART module is initialized by the LPUART_Init.
 * code
 *  LPUART_SetBaudRate(LPUART1, 115200U, 20000000U);
 * endcode
 *
 * param base LPUART peripheral base address.
 * param baudRate_Bps LPUART baudrate to be set.
 * param srcClock_Hz LPUART clock source frequency in HZ.
 * retval kStatus_LPUART_BaudrateNotSupport Baudrate is not supported in the current clock source.
 * retval kStatus_Success Set baudrate succeeded.
 */
status_t LPUART_SetBaudRate(LPUART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
    assert(0U < baudRate_Bps);

    uint32_t temp, oldCtrl;
    uint16_t sbr, sbrTemp;
    uint32_t osr, osrTemp, tempDiff, calculatedBaud, baudDiff;

    /* This LPUART instantiation uses a slightly different baud rate calculation
     * The idea is to use the best OSR (over-sampling rate) possible
     * Note, OSR is typically hard-set to 16 in other LPUART instantiations
     * loop to find the best OSR value possible, one that generates minimum baudDiff
     * iterate through the rest of the supported values of OSR */

    baudDiff = baudRate_Bps;
    osr      = 0;
    sbr      = 0;
    for (osrTemp = 4u; osrTemp <= 32u; osrTemp++)
    {
        /* calculate the temporary sbr value   */
        sbrTemp = (uint16_t)(srcClock_Hz / (baudRate_Bps * osrTemp));
        /*set sbrTemp to 1 if the sourceClockInHz can not satisfy the desired baud rate*/
        if (sbrTemp == 0u)
        {
            sbrTemp = 1u;
        }
        /* Calculate the baud rate based on the temporary OSR and SBR values */
        calculatedBaud = (uint16_t)((srcClock_Hz / (osrTemp * sbrTemp)));

        tempDiff = calculatedBaud - baudRate_Bps;

        /* Select the better value between srb and (sbr + 1) */
        if (tempDiff > (baudRate_Bps - (srcClock_Hz / (osrTemp * ((uint32_t)sbrTemp + 1u)))))
        {
            tempDiff = baudRate_Bps - (srcClock_Hz / (osrTemp * ((uint32_t)sbrTemp + 1u)));
            sbrTemp++;
        }

        if (tempDiff <= baudDiff)
        {
            baudDiff = tempDiff;
            osr      = osrTemp; /* update and store the best OSR value calculated */
            sbr      = sbrTemp; /* update store the best SBR value calculated */
        }
    }

    /* Check to see if actual baud rate is within 3% of desired baud rate
     * based on the best calculate OSR value */
    if (baudDiff < (uint32_t)((baudRate_Bps / 100u) * 3u))
    {
        /* Store CTRL before disable Tx and Rx */
        oldCtrl = base->CTRL;

        /* Disable LPUART TX RX before setting. */
        base->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

        temp = base->BAUD;

        /* Acceptable baud rate, check if OSR is between 4x and 7x oversampling.
         * If so, then "BOTHEDGE" sampling must be turned on */
        if ((osr > 3u) && (osr < 8u))
        {
            temp |= LPUART_BAUD_BOTHEDGE_MASK;
        }

        /* program the osr value (bit value is one less than actual value) */
        temp &= ~LPUART_BAUD_OSR_MASK;
        temp |= LPUART_BAUD_OSR(osr - 1u);

        /* write the sbr value to the BAUD registers */
        temp &= ~LPUART_BAUD_SBR_MASK;
        base->BAUD = temp | LPUART_BAUD_SBR(sbr);

        /* Restore CTRL. */
        base->CTRL = oldCtrl;

        return kStatus_Success;
    }
    else
    {
        /* Unacceptable baud rate difference of more than 3%*/
        return kStatus_LPUART_BaudrateNotSupport;
    }
}

/*!
 * brief Enables LPUART interrupts according to a provided mask.
 *
 * This function enables the LPUART interrupts according to a provided mask. The mask
 * is a logical OR of enumeration members. See the ref _lpuart_interrupt_enable.
 * This examples shows how to enable TX empty interrupt and RX full interrupt:
 * code
 *     LPUART_EnableInterrupts(LPUART1,kLPUART_TxDataRegEmptyInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
 * endcode
 *
 * param base LPUART peripheral base address.
 * param mask The interrupts to enable. Logical OR of ref _uart_interrupt_enable.
 */
void LPUART_EnableInterrupts(LPUART_Type *base, uint32_t mask)
{
    base->BAUD |= ((mask << 8U) & (LPUART_BAUD_LBKDIE_MASK | LPUART_BAUD_RXEDGIE_MASK));
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    base->FIFO = (base->FIFO & ~(LPUART_FIFO_TXOF_MASK | LPUART_FIFO_RXUF_MASK)) |
                 ((mask << 8U) & (LPUART_FIFO_TXOFE_MASK | LPUART_FIFO_RXUFE_MASK));
#endif
    mask &= 0xFFFFFF00U;
    base->CTRL |= mask;
}

/*!
 * brief Disables  LPUART interrupts according to a provided mask.
 *
 * This function disables the LPUART interrupts according to a provided mask. The mask
 * is a logical OR of enumeration members. See ref _lpuart_interrupt_enable.
 * This example shows how to disable the TX empty interrupt and RX full interrupt:
 * code
 *     LPUART_DisableInterrupts(LPUART1,kLPUART_TxDataRegEmptyInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
 * endcode
 *
 * param base LPUART peripheral base address.
 * param mask The interrupts to disable. Logical OR of ref _lpuart_interrupt_enable.
 */
void LPUART_DisableInterrupts(LPUART_Type *base, uint32_t mask)
{
    base->BAUD &= ~((mask << 8U) & (LPUART_BAUD_LBKDIE_MASK | LPUART_BAUD_RXEDGIE_MASK));
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    base->FIFO = (base->FIFO & ~(LPUART_FIFO_TXOF_MASK | LPUART_FIFO_RXUF_MASK)) &
                 ~((mask << 8U) & (LPUART_FIFO_TXOFE_MASK | LPUART_FIFO_RXUFE_MASK));
#endif
    mask &= 0xFFFFFF00U;
    base->CTRL &= ~mask;
}

/*!
 * brief Gets enabled LPUART interrupts.
 *
 * This function gets the enabled LPUART interrupts. The enabled interrupts are returned
 * as the logical OR value of the enumerators ref _lpuart_interrupt_enable. To check
 * a specific interrupt enable status, compare the return value with enumerators
 * in ref _lpuart_interrupt_enable.
 * For example, to check whether the TX empty interrupt is enabled:
 * code
 *     uint32_t enabledInterrupts = LPUART_GetEnabledInterrupts(LPUART1);
 *
 *     if (kLPUART_TxDataRegEmptyInterruptEnable & enabledInterrupts)
 *     {
 *         ...
 *     }
 * endcode
 *
 * param base LPUART peripheral base address.
 * return LPUART interrupt flags which are logical OR of the enumerators in ref _lpuart_interrupt_enable.
 */
uint32_t LPUART_GetEnabledInterrupts(LPUART_Type *base)
{
    uint32_t temp;
    temp = (base->BAUD & (LPUART_BAUD_LBKDIE_MASK | LPUART_BAUD_RXEDGIE_MASK)) >> 8U;
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    temp |= (base->FIFO & (LPUART_FIFO_TXOFE_MASK | LPUART_FIFO_RXUFE_MASK)) >> 8U;
#endif
    temp |= (uint32_t)(base->CTRL & 0xFF0C000u);

    return temp;
}

/*!
 * brief Gets LPUART status flags.
 *
 * This function gets all LPUART status flags. The flags are returned as the logical
 * OR value of the enumerators ref _lpuart_flags. To check for a specific status,
 * compare the return value with enumerators in the ref _lpuart_flags.
 * For example, to check whether the TX is empty:
 * code
 *     if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1))
 *     {
 *         ...
 *     }
 * endcode
 *
 * param base LPUART peripheral base address.
 * return LPUART status flags which are ORed by the enumerators in the _lpuart_flags.
 */
uint32_t LPUART_GetStatusFlags(LPUART_Type *base)
{
    uint32_t temp;
    temp = base->STAT;
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    temp |= (base->FIFO &
             (LPUART_FIFO_TXEMPT_MASK | LPUART_FIFO_RXEMPT_MASK | LPUART_FIFO_TXOF_MASK | LPUART_FIFO_RXUF_MASK)) >>
            16U;
#endif
    return temp;
}

/*!
 * brief Clears status flags with a provided mask.
 *
 * This function clears LPUART status flags with a provided mask. Automatically cleared flags
 * can't be cleared by this function.
 * Flags that can only cleared or set by hardware are:
 *    kLPUART_TxDataRegEmptyFlag, kLPUART_TransmissionCompleteFlag, kLPUART_RxDataRegFullFlag,
 *    kLPUART_RxActiveFlag, kLPUART_NoiseErrorInRxDataRegFlag, kLPUART_ParityErrorInRxDataRegFlag,
 *    kLPUART_TxFifoEmptyFlag,kLPUART_RxFifoEmptyFlag
 * Note: This API should be called when the Tx/Rx is idle, otherwise it takes no effects.
 *
 * param base LPUART peripheral base address.
 * param mask the status flags to be cleared. The user can use the enumerators in the
 *  _lpuart_status_flag_t to do the OR operation and get the mask.
 * return 0 succeed, others failed.
 * retval kStatus_LPUART_FlagCannotClearManually The flag can't be cleared by this function but
 *         it is cleared automatically by hardware.
 * retval kStatus_Success Status in the mask are cleared.
 */
status_t LPUART_ClearStatusFlags(LPUART_Type *base, uint32_t mask)
{
    uint32_t temp;
    status_t status;
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    temp = (uint32_t)base->FIFO;
    temp &= (uint32_t)(~(LPUART_FIFO_TXOF_MASK | LPUART_FIFO_RXUF_MASK));
    temp |= (mask << 16U) & (LPUART_FIFO_TXOF_MASK | LPUART_FIFO_RXUF_MASK);
    base->FIFO = temp;
#endif
    temp = (uint32_t)base->STAT;
#if defined(FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT
    temp &= (uint32_t)(~(LPUART_STAT_LBKDIF_MASK));
    temp |= mask & LPUART_STAT_LBKDIF_MASK;
#endif
    temp &= (uint32_t)(~(LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                         LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK));
    temp |= mask & (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
                    LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
#if defined(FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING) && FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING
    temp &= (uint32_t)(~(LPUART_STAT_MA2F_MASK | LPUART_STAT_MA1F_MASK));
    temp |= mask & (LPUART_STAT_MA2F_MASK | LPUART_STAT_MA1F_MASK);
#endif
    base->STAT = temp;
    /* If some flags still pending. */
    if (0U != (mask & LPUART_GetStatusFlags(base)))
    {
        /* Some flags can only clear or set by the hardware itself, these flags are: kLPUART_TxDataRegEmptyFlag,
        kLPUART_TransmissionCompleteFlag, kLPUART_RxDataRegFullFlag, kLPUART_RxActiveFlag,
        kLPUART_NoiseErrorInRxDataRegFlag, kLPUART_ParityErrorInRxDataRegFlag,
        kLPUART_TxFifoEmptyFlag, kLPUART_RxFifoEmptyFlag. */
        status = kStatus_LPUART_FlagCannotClearManually; /* flags can not clear manually */
    }
    else
    {
        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Writes to the transmitter register using a blocking method.
 *
 * This function polls the transmitter register, waits for the register to be empty or  for TX FIFO to have
 * room, and writes data to the transmitter buffer.
 *
 * note This function does not check whether all data has been sent out to the bus.
 * Before disabling the transmitter, check the kLPUART_TransmissionCompleteFlag to ensure that the transmit is
 * finished.
 *
 * param base LPUART peripheral base address.
 * param data Start address of the data to write.
 * param length Size of the data to write.
 */
void LPUART_WriteBlocking(LPUART_Type *base, const uint8_t *data, size_t length)
{
    assert(NULL != data);

    const uint8_t *dataAddress = data;
    size_t transferSize        = length;

    /* This API can only ensure that the data is written into the data buffer but can't
    ensure all data in the data buffer are sent into the transmit shift buffer. */
    while (0U != transferSize)
    {
        while (0U == (base->STAT & LPUART_STAT_TDRE_MASK))
        {
        }
        base->DATA = *(dataAddress);
        dataAddress++;
        transferSize--;
    }
}

/*!
 * brief Reads the receiver data register using a blocking method.
 *
 * This function polls the receiver register, waits for the receiver register full or receiver FIFO
 * has data, and reads data from the TX register.
 *
 * param base LPUART peripheral base address.
 * param data Start address of the buffer to store the received data.
 * param length Size of the buffer.
 * retval kStatus_LPUART_RxHardwareOverrun Receiver overrun happened while receiving data.
 * retval kStatus_LPUART_NoiseError Noise error happened while receiving data.
 * retval kStatus_LPUART_FramingError Framing error happened while receiving data.
 * retval kStatus_LPUART_ParityError Parity error happened while receiving data.
 * retval kStatus_Success Successfully received all data.
 */
status_t LPUART_ReadBlocking(LPUART_Type *base, uint8_t *data, size_t length)
{
    assert(NULL != data);

    uint32_t statusFlag;
    uint8_t *dataAddress = data;

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    uint32_t ctrl = base->CTRL;
    bool isSevenDataBits =
        ((ctrl & LPUART_CTRL_M7_MASK) || ((!(ctrl & LPUART_CTRL_M_MASK)) && (ctrl & LPUART_CTRL_PE_MASK)));
#endif

    while (0U != (length--))
    {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
        while (0U == ((base->WATER & LPUART_WATER_RXCOUNT_MASK) >> LPUART_WATER_RXCOUNT_SHIFT))
#else
        while (0U == (base->STAT & LPUART_STAT_RDRF_MASK))
#endif
        {
            statusFlag = LPUART_GetStatusFlags(base);

            if ((uint32_t)kLPUART_RxOverrunFlag == (statusFlag & (uint32_t)kLPUART_RxOverrunFlag))
            {
                return ((kStatus_Success == LPUART_ClearStatusFlags(base, (uint32_t)kLPUART_RxOverrunFlag)) ?
                            (kStatus_LPUART_RxHardwareOverrun) :
                            (kStatus_LPUART_FlagCannotClearManually));
            }

            if ((uint32_t)kLPUART_NoiseErrorFlag == (statusFlag & (uint32_t)kLPUART_NoiseErrorFlag))
            {
                return ((kStatus_Success == LPUART_ClearStatusFlags(base, (uint32_t)kLPUART_NoiseErrorFlag)) ?
                            (kStatus_LPUART_NoiseError) :
                            (kStatus_LPUART_FlagCannotClearManually));
            }

            if ((uint32_t)kLPUART_FramingErrorFlag == (statusFlag & (uint32_t)kLPUART_FramingErrorFlag))
            {
                return ((kStatus_Success == LPUART_ClearStatusFlags(base, (uint32_t)kLPUART_FramingErrorFlag)) ?
                            (kStatus_LPUART_FramingError) :
                            (kStatus_LPUART_FlagCannotClearManually));
            }

            if ((uint32_t)kLPUART_ParityErrorFlag == (statusFlag & (uint32_t)kLPUART_ParityErrorFlag))
            {
                return ((kStatus_Success == LPUART_ClearStatusFlags(base, (uint32_t)kLPUART_ParityErrorFlag)) ?
                            (kStatus_LPUART_ParityError) :
                            (kStatus_LPUART_FlagCannotClearManually));
            }
        }
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
        if (isSevenDataBits)
        {
            *(dataAddress) = (uint8_t)(base->DATA & 0x7F);
            dataAddress++;
        }
        else
        {
            *(dataAddress) = (uint8_t)base->DATA;
            dataAddress++;
        }
#else
        *(dataAddress) = (uint8_t)base->DATA;
        dataAddress++;
#endif
    }

    return kStatus_Success;
}

/*!
 * brief Initializes the LPUART handle.
 *
 * This function initializes the LPUART handle, which can be used for other LPUART
 * transactional APIs. Usually, for a specified LPUART instance,
 * call this API once to get the initialized handle.
 *
 * The LPUART driver supports the "background" receiving, which means that user can set up
 * an RX ring buffer optionally. Data received is stored into the ring buffer even when the
 * user doesn't call the LPUART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 * The ring buffer is disabled if passing NULL as p ringBuffer.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param callback Callback function.
 * param userData User data.
 */
void LPUART_TransferCreateHandle(LPUART_Type *base,
                                 lpuart_handle_t *handle,
                                 lpuart_transfer_callback_t callback,
                                 void *userData)
{
    assert(NULL != handle);

    uint32_t instance;

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    uint32_t ctrl = base->CTRL;
    bool isSevenDataBits =
        ((ctrl & LPUART_CTRL_M7_MASK) || ((!(ctrl & LPUART_CTRL_M_MASK)) && (ctrl & LPUART_CTRL_PE_MASK)));
#endif

    /* Zero the handle. */
    (void)memset(handle, 0, sizeof(lpuart_handle_t));

    /* Set the TX/RX state. */
    handle->rxState = (uint8_t)kLPUART_RxIdle;
    handle->txState = (uint8_t)kLPUART_TxIdle;

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    /* Initial seven data bits flag */
    handle->isSevenDataBits = isSevenDataBits;
#endif

    /* Get instance from peripheral base address. */
    instance = LPUART_GetInstance(base);

    /* Save the handle in global variables to support the double weak mechanism. */
    s_lpuartHandle[instance] = handle;

    s_lpuartIsr = LPUART_TransferHandleIRQ;

/* Enable interrupt in NVIC. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    (void)EnableIRQ(s_lpuartRxIRQ[instance]);
    (void)EnableIRQ(s_lpuartTxIRQ[instance]);
#else
    (void)EnableIRQ(s_lpuartIRQ[instance]);
#endif
}

/*!
 * brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 *
 * note When using RX ring buffer, one byte is reserved for internal use. In other
 * words, if p ringBufferSize is 32, then only 31 bytes are used for saving data.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * param ringBufferSize size of the ring buffer.
 */
void LPUART_TransferStartRingBuffer(LPUART_Type *base,
                                    lpuart_handle_t *handle,
                                    uint8_t *ringBuffer,
                                    size_t ringBufferSize)
{
    assert(NULL != handle);
    assert(NULL != ringBuffer);

    /* Setup the ring buffer address */
    handle->rxRingBuffer     = ringBuffer;
    handle->rxRingBufferSize = ringBufferSize;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;

    /* Enable the interrupt to accept the data when user need the ring buffer. */
    LPUART_EnableInterrupts(
        base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
}

/*!
 * brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 */
void LPUART_TransferStopRingBuffer(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    if (handle->rxState == (uint8_t)kLPUART_RxIdle)
    {
        LPUART_DisableInterrupts(
            base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
    }

    handle->rxRingBuffer     = NULL;
    handle->rxRingBufferSize = 0U;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;
}

/*!
 * brief Transmits a buffer of data using the interrupt method.
 *
 * This function send data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data written to the transmitter register. When
 * all data is written to the TX register in the ISR, the LPUART driver calls the callback
 * function and passes the ref kStatus_LPUART_TxIdle as status parameter.
 *
 * note The kStatus_LPUART_TxIdle is passed to the upper layer when all data are written
 * to the TX register. However, there is no check to ensure that all the data sent out. Before disabling the TX,
 * check the kLPUART_TransmissionCompleteFlag to ensure that the transmit is finished.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param xfer LPUART transfer structure, see #lpuart_transfer_t.
 * retval kStatus_Success Successfully start the data transmission.
 * retval kStatus_LPUART_TxBusy Previous transmission still not finished, data not all written to the TX register.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_TransferSendNonBlocking(LPUART_Type *base, lpuart_handle_t *handle, lpuart_transfer_t *xfer)
{
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    status_t status;

    /* Return error if current TX busy. */
    if ((uint8_t)kLPUART_TxBusy == handle->txState)
    {
        status = kStatus_LPUART_TxBusy;
    }
    else
    {
        handle->txData        = xfer->data;
        handle->txDataSize    = xfer->dataSize;
        handle->txDataSizeAll = xfer->dataSize;
        handle->txState       = (uint8_t)kLPUART_TxBusy;

        /* Enable transmitter interrupt. */
        LPUART_EnableInterrupts(base, (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt driven data sending. The user can get the remainBtyes to find out
 * how many bytes are not sent out.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 */
void LPUART_TransferAbortSend(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    LPUART_DisableInterrupts(
        base, (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable | (uint32_t)kLPUART_TransmissionCompleteInterruptEnable);

    handle->txDataSize = 0;
    handle->txState    = (uint8_t)kLPUART_TxIdle;
}

/*!
 * brief Gets the number of bytes that have been written to the LPUART transmitter register.
 *
 * This function gets the number of bytes that have been written to LPUART TX
 * register by an interrupt method.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param count Send bytes count.
 * retval kStatus_NoTransferInProgress No send in progress.
 * retval kStatus_InvalidArgument Parameter is invalid.
 * retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetSendCount(LPUART_Type *base, lpuart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if ((uint8_t)kLPUART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll - handle->txDataSize;

    return kStatus_Success;
}

/*!
 * brief Receives a buffer of data using the interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function
 * which returns without waiting to ensure that all data are received.
 * If the RX ring buffer is used and not empty, the data in the ring buffer is copied and
 * the parameter p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in the ring buffer is not enough for read, the receive
 * request is saved by the LPUART driver. When the new data arrives, the receive request
 * is serviced first. When all data is received, the LPUART driver notifies the upper layer
 * through a callback function and passes a status parameter ref kStatus_UART_RxIdle.
 * For example, the upper layer needs 10 bytes but there are only 5 bytes in ring buffer.
 * The 5 bytes are copied to xfer->data, which returns with the
 * parameter p receivedBytes set to 5. For the remaining 5 bytes, the newly arrived data is
 * saved from xfer->data[5]. When 5 bytes are received, the LPUART driver notifies the upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to xfer->data. When all data is received, the upper layer is notified.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param xfer LPUART transfer structure, see #uart_transfer_t.
 * param receivedBytes Bytes received from the ring buffer directly.
 * retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * retval kStatus_LPUART_RxBusy Previous receive request is not finished.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_TransferReceiveNonBlocking(LPUART_Type *base,
                                           lpuart_handle_t *handle,
                                           lpuart_transfer_t *xfer,
                                           size_t *receivedBytes)
{
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    uint32_t i;
    status_t status;
    /* How many bytes to copy from ring buffer to user memory. */
    size_t bytesToCopy = 0U;
    /* How many bytes to receive. */
    size_t bytesToReceive;
    /* How many bytes currently have received. */
    size_t bytesCurrentReceived;

    /* How to get data:
       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
          to lpuart handle, enable interrupt to store received data to xfer->data. When
          all data received, trigger callback.
       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
          If there are enough data in ring buffer, copy them to xfer->data and return.
          If there are not enough data in ring buffer, copy all of them to xfer->data,
          save the xfer->data remained empty space to lpuart handle, receive data
          to this empty space and trigger callback when finished. */

    if ((uint8_t)kLPUART_RxBusy == handle->rxState)
    {
        status = kStatus_LPUART_RxBusy;
    }
    else
    {
        bytesToReceive       = xfer->dataSize;
        bytesCurrentReceived = 0;

        /* If RX ring buffer is used. */
        if (NULL != handle->rxRingBuffer)
        {
            /* Disable LPUART RX IRQ, protect ring buffer. */
            LPUART_DisableInterrupts(base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable);

            /* How many bytes in RX ring buffer currently. */
            bytesToCopy = LPUART_TransferGetRxRingBufferLength(base, handle);

            if (0U != bytesToCopy)
            {
                bytesToCopy = MIN(bytesToReceive, bytesToCopy);

                bytesToReceive -= bytesToCopy;

                /* Copy data from ring buffer to user memory. */
                for (i = 0U; i < bytesToCopy; i++)
                {
                    xfer->data[bytesCurrentReceived] = handle->rxRingBuffer[handle->rxRingBufferTail];
                    bytesCurrentReceived++;

                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
                    if (((uint32_t)handle->rxRingBufferTail + 1U) == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }
            }

            /* If ring buffer does not have enough data, still need to read more data. */
            if (0U != bytesToReceive)
            {
                /* No data in ring buffer, save the request to LPUART handle. */
                handle->rxData        = xfer->data + bytesCurrentReceived;
                handle->rxDataSize    = bytesToReceive;
                handle->rxDataSizeAll = bytesToReceive;
                handle->rxState       = (uint8_t)kLPUART_RxBusy;
            }
            /* Enable LPUART RX IRQ if previously enabled. */
            LPUART_EnableInterrupts(base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable);

            /* Call user callback since all data are received. */
            if (0U == bytesToReceive)
            {
                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_LPUART_RxIdle, handle->userData);
                }
            }
        }
        /* Ring buffer not used. */
        else
        {
            handle->rxData        = xfer->data + bytesCurrentReceived;
            handle->rxDataSize    = bytesToReceive;
            handle->rxDataSizeAll = bytesToReceive;
            handle->rxState       = (uint8_t)kLPUART_RxBusy;

            /* Enable RX interrupt. */
            LPUART_EnableInterrupts(base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                              (uint32_t)kLPUART_RxOverrunInterruptEnable |
                                              (uint32_t)kLPUART_IdleLineInterruptEnable);
        }

        /* Return the how many bytes have read. */
        if (NULL != receivedBytes)
        {
            *receivedBytes = bytesCurrentReceived;
        }

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to find out
 * how many bytes not received yet.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 */
void LPUART_TransferAbortReceive(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    /* Only abort the receive to handle->rxData, the RX ring buffer is still working. */
    if (NULL == handle->rxRingBuffer)
    {
        /* Disable RX interrupt. */
        LPUART_DisableInterrupts(base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                           (uint32_t)kLPUART_RxOverrunInterruptEnable |
                                           (uint32_t)kLPUART_IdleLineInterruptEnable);
    }

    handle->rxDataSize = 0U;
    handle->rxState    = (uint8_t)kLPUART_RxIdle;
}

/*!
 * brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param count Receive bytes count.
 * retval kStatus_NoTransferInProgress No receive in progress.
 * retval kStatus_InvalidArgument Parameter is invalid.
 * retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetReceiveCount(LPUART_Type *base, lpuart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if ((uint8_t)kLPUART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll - handle->rxDataSize;

    return kStatus_Success;
}

/*!
 * brief LPUART IRQ handle function.
 *
 * This function handles the LPUART transmit and receive IRQ request.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 */
void LPUART_TransferHandleIRQ(LPUART_Type *base, lpuart_handle_t *handle)
{
    assert(NULL != handle);

    uint8_t count;
    uint8_t tempCount;
    uint32_t status            = LPUART_GetStatusFlags(base);
    uint32_t enabledInterrupts = LPUART_GetEnabledInterrupts(base);

    /* If RX overrun. */
    if ((uint32_t)kLPUART_RxOverrunFlag == ((uint32_t)kLPUART_RxOverrunFlag & status))
    {
        /* Clear overrun flag, otherwise the RX does not work. */
        base->STAT = ((base->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);

        /* Trigger callback. */
        if (NULL != (handle->callback))
        {
            handle->callback(base, handle, kStatus_LPUART_RxHardwareOverrun, handle->userData);
        }
    }

    /* If IDLE flag is set and the IDLE interrupt is enabled. */
    if ((0U != ((uint32_t)kLPUART_IdleLineFlag & status)) &&
        (0U != ((uint32_t)kLPUART_IdleLineInterruptEnable & enabledInterrupts)))
    {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
        count = ((uint8_t)((base->WATER & LPUART_WATER_RXCOUNT_MASK) >> LPUART_WATER_RXCOUNT_SHIFT));

        while ((0U != handle->rxDataSize) && (0U != count))
        {
            tempCount = (uint8_t)MIN(handle->rxDataSize, count);

            /* Using non block API to read the data from the registers. */
            LPUART_ReadNonBlocking(base, handle->rxData, tempCount);
            handle->rxData += tempCount;
            handle->rxDataSize -= tempCount;
            count -= tempCount;

            /* If rxDataSize is 0, disable idle line interrupt.*/
            if (0U == (handle->rxDataSize))
            {
                handle->rxState = (uint8_t)kLPUART_RxIdle;

                LPUART_DisableInterrupts(
                    base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_LPUART_RxIdle, handle->userData);
                }
            }
        }
#endif
        /* Clear IDLE flag.*/
        base->STAT |= LPUART_STAT_IDLE_MASK;

        /* If rxDataSize is 0, disable idle line interrupt.*/
        if (0U != (handle->rxDataSize))
        {
            LPUART_DisableInterrupts(base, (uint32_t)kLPUART_IdleLineInterruptEnable);
        }
        /* If callback is not NULL and rxDataSize is not 0. */
        if ((0U != handle->rxDataSize) && (NULL != handle->callback))
        {
            handle->callback(base, handle, kStatus_LPUART_IdleLineDetected, handle->userData);
        }
    }
    /* Receive data register full */
    if ((0U != ((uint32_t)kLPUART_RxDataRegFullFlag & status)) &&
        (0U != ((uint32_t)kLPUART_RxDataRegFullInterruptEnable & enabledInterrupts)))
    {
/* Get the size that can be stored into buffer for this interrupt. */
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
        count = ((uint8_t)((base->WATER & LPUART_WATER_RXCOUNT_MASK) >> LPUART_WATER_RXCOUNT_SHIFT));
#else
        count = 1;
#endif

        /* If handle->rxDataSize is not 0, first save data to handle->rxData. */
        while ((0U != handle->rxDataSize) && (0U != count))
        {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
            tempCount = (uint8_t)MIN(handle->rxDataSize, count);
#else
            tempCount = 1;
#endif

            /* Using non block API to read the data from the registers. */
            LPUART_ReadNonBlocking(base, handle->rxData, tempCount);
            handle->rxData += tempCount;
            handle->rxDataSize -= tempCount;
            count -= tempCount;

            /* If all the data required for upper layer is ready, trigger callback. */
            if (0U == handle->rxDataSize)
            {
                handle->rxState = (uint8_t)kLPUART_RxIdle;

                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_LPUART_RxIdle, handle->userData);
                }
            }
        }

        /* If use RX ring buffer, receive data to ring buffer. */
        if (NULL != handle->rxRingBuffer)
        {
            while (0U != count--)
            {
                /* If RX ring buffer is full, trigger callback to notify over run. */
                if (LPUART_TransferIsRxRingBufferFull(base, handle))
                {
                    if (NULL != handle->callback)
                    {
                        handle->callback(base, handle, kStatus_LPUART_RxRingBufferOverrun, handle->userData);
                    }
                }

                /* If ring buffer is still full after callback function, the oldest data is overridden. */
                if (LPUART_TransferIsRxRingBufferFull(base, handle))
                {
                    /* Increase handle->rxRingBufferTail to make room for new data. */
                    if (((uint32_t)handle->rxRingBufferTail + 1U) == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }

/* Read data. */
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
                if (handle->isSevenDataBits)
                {
                    handle->rxRingBuffer[handle->rxRingBufferHead] = (base->DATA & 0x7F);
                }
                else
                {
                    handle->rxRingBuffer[handle->rxRingBufferHead] = base->DATA;
                }
#else
                uint16_t tpmRxRingBufferHead              = handle->rxRingBufferHead;
                uint32_t tpmData                          = base->DATA;
                handle->rxRingBuffer[tpmRxRingBufferHead] = (uint8_t)tpmData;
#endif

                /* Increase handle->rxRingBufferHead. */
                if (((uint32_t)handle->rxRingBufferHead + 1U) == handle->rxRingBufferSize)
                {
                    handle->rxRingBufferHead = 0U;
                }
                else
                {
                    handle->rxRingBufferHead++;
                }
            }
        }
        /* If no receive requst pending, stop RX interrupt. */
        else if (0U == handle->rxDataSize)
        {
            LPUART_DisableInterrupts(
                base, (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
        }
        else
        {
        }
    }

    /* Send data register empty and the interrupt is enabled. */
    if ((0U != ((uint32_t)kLPUART_TxDataRegEmptyFlag & status)) &&
        (0U != ((uint32_t)kLPUART_TxDataRegEmptyInterruptEnable & enabledInterrupts)))
    {
/* Get the bytes that available at this moment. */
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
        count = (uint8_t)FSL_FEATURE_LPUART_FIFO_SIZEn(base) -
                (uint8_t)((base->WATER & LPUART_WATER_TXCOUNT_MASK) >> LPUART_WATER_TXCOUNT_SHIFT);
#else
        count = 1;
#endif

        while ((0U != handle->txDataSize) && (0U != count))
        {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
            tempCount = (uint8_t)MIN(handle->txDataSize, count);
#else
            tempCount = 1;
#endif

            /* Using non block API to write the data to the registers. */
            LPUART_WriteNonBlocking(base, handle->txData, tempCount);
            handle->txData += tempCount;
            handle->txDataSize -= tempCount;
            count -= tempCount;

            /* If all the data are written to data register, notify user with the callback, then TX finished. */
            if (0U == handle->txDataSize)
            {
                handle->txState = (uint8_t)kLPUART_TxIdle;

                /* Disable TX register empty interrupt. */
                base->CTRL = (base->CTRL & ~LPUART_CTRL_TIE_MASK);

                /* Trigger callback. */
                if (NULL != handle->callback)
                {
                    handle->callback(base, handle, kStatus_LPUART_TxIdle, handle->userData);
                }
            }
        }
    }
}

/*!
 * brief LPUART Error IRQ handle function.
 *
 * This function handles the LPUART error IRQ request.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 */
void LPUART_TransferHandleErrorIRQ(LPUART_Type *base, lpuart_handle_t *handle)
{
    /* To be implemented by User. */
}
#if defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_LPUART1_RX_DriverIRQHandler(void)
{
    uint32_t stat = 0U;
    uint32_t ctrl = 0U;

    if (CLOCK_isEnabledClock(s_lpuartClock[0]))
    {
        stat = LPUART0->STAT;
        ctrl = LPUART0->CTRL;
        if ((LPUART_STAT_OR_MASK & stat) || ((LPUART_STAT_RDRF_MASK & stat) && (LPUART_CTRL_RIE_MASK & ctrl)))
        {
            s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
        }
    }
    if (CLOCK_isEnabledClock(s_lpuartClock[1]))
    {
        stat = LPUART1->STAT;
        ctrl = LPUART1->CTRL;
        if ((LPUART_STAT_OR_MASK & stat) || ((LPUART_STAT_RDRF_MASK & stat) && (LPUART_CTRL_RIE_MASK & ctrl)))
        {
            s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART0_LPUART1_TX_DriverIRQHandler(void)
{
    uint32_t stat = 0U;
    uint32_t ctrl = 0U;

    if (CLOCK_isEnabledClock(s_lpuartClock[0]))
    {
        stat = LPUART0->STAT;
        ctrl = LPUART0->CTRL;
        if ((LPUART_STAT_OR_MASK & stat) || ((stat & LPUART_STAT_TDRE_MASK) && (ctrl & LPUART_CTRL_TIE_MASK)))
        {
            s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
        }
    }
    if (CLOCK_isEnabledClock(s_lpuartClock[1]))
    {
        stat = LPUART1->STAT;
        ctrl = LPUART1->CTRL;
        if ((LPUART_STAT_OR_MASK & stat) || ((stat & LPUART_STAT_TDRE_MASK) && (ctrl & LPUART_CTRL_TIE_MASK)))
        {
            s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART0_LPUART1_DriverIRQHandler(void)
{
    uint32_t stat = 0U;
    uint32_t ctrl = 0U;

    if (CLOCK_isEnabledClock(s_lpuartClock[0]))
    {
        stat = LPUART0->STAT;
        ctrl = LPUART0->CTRL;
        if ((0U != (LPUART_STAT_OR_MASK & stat)) ||
            ((0U != (LPUART_STAT_RDRF_MASK & stat)) && (0U != (LPUART_CTRL_RIE_MASK & ctrl))) ||
            ((0U != (stat & LPUART_STAT_TDRE_MASK)) && (0U != (ctrl & LPUART_CTRL_TIE_MASK))))
        {
            s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
        }
    }
    if (CLOCK_isEnabledClock(s_lpuartClock[1]))
    {
        stat = LPUART1->STAT;
        ctrl = LPUART1->CTRL;
        if ((0U != (LPUART_STAT_OR_MASK & stat)) ||
            ((0U != (LPUART_STAT_RDRF_MASK & stat)) && (0U != (LPUART_CTRL_RIE_MASK & ctrl))) ||
            ((0U != (stat & LPUART_STAT_TDRE_MASK)) && (0U != (ctrl & LPUART_CTRL_TIE_MASK))))
        {
            s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART0)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART0_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART0_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART0, s_lpuartHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#endif

#if defined(LPUART1)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART1_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART1_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART1_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART1, s_lpuartHandle[1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#endif

#if defined(LPUART2)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART2_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART2, s_lpuartHandle[2]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART2_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART2, s_lpuartHandle[2]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART2_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART2, s_lpuartHandle[2]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART3)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART3_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART3, s_lpuartHandle[3]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART3_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART3, s_lpuartHandle[3]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART3_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART3, s_lpuartHandle[3]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART4)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART4_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART4, s_lpuartHandle[4]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART4_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART4, s_lpuartHandle[4]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART4_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART4, s_lpuartHandle[4]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART5)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART5_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART5, s_lpuartHandle[5]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART5_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART5, s_lpuartHandle[5]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART5_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART5, s_lpuartHandle[5]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART6)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART6_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART6, s_lpuartHandle[6]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART6_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART6, s_lpuartHandle[6]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART6_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART6, s_lpuartHandle[6]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART7)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART7_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART7, s_lpuartHandle[7]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART7_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART7, s_lpuartHandle[7]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART7_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART7, s_lpuartHandle[7]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART8)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART8_TX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART8, s_lpuartHandle[8]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART8_RX_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART8, s_lpuartHandle[8]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART8_DriverIRQHandler(void)
{
    s_lpuartIsr(LPUART8, s_lpuartHandle[8]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(CM4_0__LPUART)
void M4_0_LPUART_DriverIRQHandler(void)
{
    s_lpuartIsr(CM4_0__LPUART, s_lpuartHandle[LPUART_GetInstance(CM4_0__LPUART)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CM4_1__LPUART)
void M4_1_LPUART_DriverIRQHandler(void)
{
    s_lpuartIsr(CM4_1__LPUART, s_lpuartHandle[LPUART_GetInstance(CM4_1__LPUART)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CM4__LPUART)
void M4_LPUART_DriverIRQHandler(void)
{
    s_lpuartIsr(CM4__LPUART, s_lpuartHandle[LPUART_GetInstance(CM4__LPUART)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART0)
void DMA_UART0_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(DMA__LPUART0, s_lpuartHandle[LPUART_GetInstance(DMA__LPUART0)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART1)
void DMA_UART1_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(DMA__LPUART1, s_lpuartHandle[LPUART_GetInstance(DMA__LPUART1)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART2)
void DMA_UART2_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(DMA__LPUART2, s_lpuartHandle[LPUART_GetInstance(DMA__LPUART2)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART3)
void DMA_UART3_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(DMA__LPUART3, s_lpuartHandle[LPUART_GetInstance(DMA__LPUART3)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART4)
void DMA_UART4_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(DMA__LPUART4, s_lpuartHandle[LPUART_GetInstance(DMA__LPUART4)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART0)
void ADMA_UART0_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(ADMA__LPUART0, s_lpuartHandle[LPUART_GetInstance(ADMA__LPUART0)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART1)
void ADMA_UART1_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(ADMA__LPUART1, s_lpuartHandle[LPUART_GetInstance(ADMA__LPUART1)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART2)
void ADMA_UART2_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(ADMA__LPUART2, s_lpuartHandle[LPUART_GetInstance(ADMA__LPUART2)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART3)
void ADMA_UART3_INT_DriverIRQHandler(void)
{
    s_lpuartIsr(ADMA__LPUART3, s_lpuartHandle[LPUART_GetInstance(ADMA__LPUART3)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
