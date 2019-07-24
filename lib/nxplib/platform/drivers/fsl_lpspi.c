/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpspi"
#endif

/*!
 * @brief Default watermark values.
 *
 * The default watermarks are set to zero.
 */
enum _lpspi_default_watermarks
{
    kLpspiDefaultTxWatermark = 0,
    kLpspiDefaultRxWatermark = 0,
};

/*! @brief Typedef for master interrupt handler. */
typedef void (*lpspi_master_isr_t)(LPSPI_Type *base, lpspi_master_handle_t *handle);

/*! @brief Typedef for slave interrupt handler. */
typedef void (*lpspi_slave_isr_t)(LPSPI_Type *base, lpspi_slave_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get instance number for LPSPI module.
 *
 * @param base LPSPI peripheral base address.
 * @return Return the value of LPSPI instance.
 */
uint32_t LPSPI_GetInstance(LPSPI_Type *base);

/*!
 * @brief Configures the LPSPI peripheral chip select polarity.
 *
 * This function  takes in the desired peripheral chip select (Pcs) and it's corresponding desired polarity and
 * configures the Pcs signal to operate with the desired characteristic.
 *
 * @param base LPSPI peripheral address.
 * @param pcs The particular peripheral chip select (parameter value is of type lpspi_which_pcs_t) for which we wish to
 *            apply the active high or active low characteristic.
 * @param activeLowOrHigh The setting for either "active high, inactive low (0)"  or "active low, inactive high(1)" of
 *                        type lpspi_pcs_polarity_config_t.
 */
static void LPSPI_SetOnePcsPolarity(LPSPI_Type *base,
                                    lpspi_which_pcs_t pcs,
                                    lpspi_pcs_polarity_config_t activeLowOrHigh);

/*!
 * @brief Combine the write data for 1 byte to 4 bytes.
 * This is not a public API.
 */
static uint32_t LPSPI_CombineWriteData(uint8_t *txData, uint32_t bytesEachWrite, bool isByteSwap);

/*!
 * @brief Separate the read data for 1 byte to 4 bytes.
 * This is not a public API.
 */
static void LPSPI_SeparateReadData(uint8_t *rxData, uint32_t readData, uint32_t bytesEachRead, bool isByteSwap);

/*!
 * @brief Master fill up the TX FIFO with data.
 * This is not a public API.
 */
static void LPSPI_MasterTransferFillUpTxFifo(LPSPI_Type *base, lpspi_master_handle_t *handle);

/*!
 * @brief Master finish up a transfer.
 * It would call back if there is callback function and set the state to idle.
 * This is not a public API.
 */
static void LPSPI_MasterTransferComplete(LPSPI_Type *base, lpspi_master_handle_t *handle);

/*!
 * @brief Slave fill up the TX FIFO with data.
 * This is not a public API.
 */
static void LPSPI_SlaveTransferFillUpTxFifo(LPSPI_Type *base, lpspi_slave_handle_t *handle);

/*!
 * @brief Slave finish up a transfer.
 * It would call back if there is callback function and set the state to idle.
 * This is not a public API.
 */
static void LPSPI_SlaveTransferComplete(LPSPI_Type *base, lpspi_slave_handle_t *handle);

/*!
 * @brief LPSPI common interrupt handler.
 *
 * @param handle pointer to s_lpspiHandle which stores the transfer state.
 */
static void LPSPI_CommonIRQHandler(LPSPI_Type *base, void *param);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Defines constant value arrays for the baud rate pre-scalar and scalar divider values.*/
static const uint8_t s_baudratePrescaler[] = {1, 2, 4, 8, 16, 32, 64, 128};

/*! @brief Pointers to lpspi bases for each instance. */
static LPSPI_Type *const s_lpspiBases[] = LPSPI_BASE_PTRS;

/*! @brief Pointers to lpspi IRQ number for each instance. */
static const IRQn_Type s_lpspiIRQ[] = LPSPI_IRQS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to lpspi clocks for each instance. */
static const clock_ip_name_t s_lpspiClocks[] = LPSPI_CLOCKS;

#if defined(LPSPI_PERIPH_CLOCKS)
static const clock_ip_name_t s_LpspiPeriphClocks[] = LPSPI_PERIPH_CLOCKS;
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to lpspi handles for each instance. */
static void *s_lpspiHandle[ARRAY_SIZE(s_lpspiBases)] = {NULL};

/*! @brief Pointer to master IRQ handler for each instance. */
static lpspi_master_isr_t s_lpspiMasterIsr;
/*! @brief Pointer to slave IRQ handler for each instance. */
static lpspi_slave_isr_t s_lpspiSlaveIsr;
/* @brief Dummy data for each instance. This data is used when user's tx buffer is NULL*/
volatile uint8_t g_lpspiDummyData[ARRAY_SIZE(s_lpspiBases)] = {0};
/**********************************************************************************************************************
 * Code
 *********************************************************************************************************************/
uint32_t LPSPI_GetInstance(LPSPI_Type *base)
{
    uint8_t instance = 0;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_lpspiBases); instance++)
    {
        if (s_lpspiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_lpspiBases));

    return instance;
}

/*!
 * brief Set up the dummy data.
 *
 * param base LPSPI peripheral address.
 * param dummyData Data to be transferred when tx buffer is NULL.
 * Note:
 *      This API has no effect when LPSPI in slave interrupt mode, because driver
 *      will set the TXMSK bit to 1 if txData is NULL, no data is loaded from transmit
 *      FIFO and output pin is tristated.
 */
void LPSPI_SetDummyData(LPSPI_Type *base, uint8_t dummyData)
{
    uint32_t instance          = LPSPI_GetInstance(base);
    g_lpspiDummyData[instance] = dummyData;
}

/*!
 * brief Initializes the LPSPI master.
 *
 * param base LPSPI peripheral address.
 * param masterConfig Pointer to structure lpspi_master_config_t.
 * param srcClock_Hz Module source input clock in Hertz
 */
void LPSPI_MasterInit(LPSPI_Type *base, const lpspi_master_config_t *masterConfig, uint32_t srcClock_Hz)
{
    assert(masterConfig);

    uint32_t tcrPrescaleValue = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)

    uint32_t instance = LPSPI_GetInstance(base);
    /* Enable LPSPI clock */
    CLOCK_EnableClock(s_lpspiClocks[instance]);

#if defined(LPSPI_PERIPH_CLOCKS)
    CLOCK_EnableClock(s_LpspiPeriphClocks[instance]);
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Set LPSPI to master */
    LPSPI_SetMasterSlaveMode(base, kLPSPI_Master);

    /* Set specific PCS to active high or low */
    LPSPI_SetOnePcsPolarity(base, masterConfig->whichPcs, masterConfig->pcsActiveHighOrLow);

    /* Set Configuration Register 1 related setting.*/
    base->CFGR1 = (base->CFGR1 & ~(LPSPI_CFGR1_OUTCFG_MASK | LPSPI_CFGR1_PINCFG_MASK | LPSPI_CFGR1_NOSTALL_MASK)) |
                  LPSPI_CFGR1_OUTCFG(masterConfig->dataOutConfig) | LPSPI_CFGR1_PINCFG(masterConfig->pinCfg) |
                  LPSPI_CFGR1_NOSTALL(0);

    /* Set baudrate and delay times*/
    LPSPI_MasterSetBaudRate(base, masterConfig->baudRate, srcClock_Hz, &tcrPrescaleValue);

    /* Set default watermarks */
    LPSPI_SetFifoWatermarks(base, kLpspiDefaultTxWatermark, kLpspiDefaultRxWatermark);

    /* Set Transmit Command Register*/
    base->TCR = LPSPI_TCR_CPOL(masterConfig->cpol) | LPSPI_TCR_CPHA(masterConfig->cpha) |
                LPSPI_TCR_LSBF(masterConfig->direction) | LPSPI_TCR_FRAMESZ(masterConfig->bitsPerFrame - 1) |
                LPSPI_TCR_PRESCALE(tcrPrescaleValue) | LPSPI_TCR_PCS(masterConfig->whichPcs);

    LPSPI_Enable(base, true);

    LPSPI_MasterSetDelayTimes(base, masterConfig->pcsToSckDelayInNanoSec, kLPSPI_PcsToSck, srcClock_Hz);
    LPSPI_MasterSetDelayTimes(base, masterConfig->lastSckToPcsDelayInNanoSec, kLPSPI_LastSckToPcs, srcClock_Hz);
    LPSPI_MasterSetDelayTimes(base, masterConfig->betweenTransferDelayInNanoSec, kLPSPI_BetweenTransfer, srcClock_Hz);

    LPSPI_SetDummyData(base, LPSPI_DUMMY_DATA);
}

/*!
 * brief Sets the lpspi_master_config_t structure to default values.
 *
 * This API initializes the configuration structure  for LPSPI_MasterInit().
 * The initialized structure can remain unchanged in LPSPI_MasterInit(), or can be modified
 * before calling the LPSPI_MasterInit().
 * Example:
 * code
 *  lpspi_master_config_t  masterConfig;
 *  LPSPI_MasterGetDefaultConfig(&masterConfig);
 * endcode
 * param masterConfig pointer to lpspi_master_config_t structure
 */
void LPSPI_MasterGetDefaultConfig(lpspi_master_config_t *masterConfig)
{
    assert(masterConfig);

    /* Initializes the configure structure to zero. */
    memset(masterConfig, 0, sizeof(*masterConfig));

    masterConfig->baudRate     = 500000;
    masterConfig->bitsPerFrame = 8;
    masterConfig->cpol         = kLPSPI_ClockPolarityActiveHigh;
    masterConfig->cpha         = kLPSPI_ClockPhaseFirstEdge;
    masterConfig->direction    = kLPSPI_MsbFirst;

    masterConfig->pcsToSckDelayInNanoSec        = 1000000000 / masterConfig->baudRate * 2;
    masterConfig->lastSckToPcsDelayInNanoSec    = 1000000000 / masterConfig->baudRate * 2;
    masterConfig->betweenTransferDelayInNanoSec = 1000000000 / masterConfig->baudRate * 2;

    masterConfig->whichPcs           = kLPSPI_Pcs0;
    masterConfig->pcsActiveHighOrLow = kLPSPI_PcsActiveLow;

    masterConfig->pinCfg        = kLPSPI_SdiInSdoOut;
    masterConfig->dataOutConfig = kLpspiDataOutRetained;
}

/*!
 * brief LPSPI slave configuration.
 *
 * param base LPSPI peripheral address.
 * param slaveConfig Pointer to a structure lpspi_slave_config_t.
 */
void LPSPI_SlaveInit(LPSPI_Type *base, const lpspi_slave_config_t *slaveConfig)
{
    assert(slaveConfig);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)

    uint32_t instance = LPSPI_GetInstance(base);
    /* Enable LPSPI clock */
    CLOCK_EnableClock(s_lpspiClocks[instance]);

#if defined(LPSPI_PERIPH_CLOCKS)
    CLOCK_EnableClock(s_LpspiPeriphClocks[instance]);
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    LPSPI_SetMasterSlaveMode(base, kLPSPI_Slave);

    LPSPI_SetOnePcsPolarity(base, slaveConfig->whichPcs, slaveConfig->pcsActiveHighOrLow);

    base->CFGR1 = (base->CFGR1 & ~(LPSPI_CFGR1_OUTCFG_MASK | LPSPI_CFGR1_PINCFG_MASK)) |
                  LPSPI_CFGR1_OUTCFG(slaveConfig->dataOutConfig) | LPSPI_CFGR1_PINCFG(slaveConfig->pinCfg);

    LPSPI_SetFifoWatermarks(base, kLpspiDefaultTxWatermark, kLpspiDefaultRxWatermark);

    base->TCR = LPSPI_TCR_CPOL(slaveConfig->cpol) | LPSPI_TCR_CPHA(slaveConfig->cpha) |
                LPSPI_TCR_LSBF(slaveConfig->direction) | LPSPI_TCR_FRAMESZ(slaveConfig->bitsPerFrame - 1);

    /* This operation will set the dummy data for edma transfer, no effect in interrupt way. */
    LPSPI_SetDummyData(base, LPSPI_DUMMY_DATA);

    LPSPI_Enable(base, true);
}

/*!
 * brief Sets the lpspi_slave_config_t structure to default values.
 *
 * This API initializes the configuration structure for LPSPI_SlaveInit().
 * The initialized structure can remain unchanged in LPSPI_SlaveInit() or can be modified
 * before calling the LPSPI_SlaveInit().
 * Example:
 * code
 *  lpspi_slave_config_t  slaveConfig;
 *  LPSPI_SlaveGetDefaultConfig(&slaveConfig);
 * endcode
 * param slaveConfig pointer to lpspi_slave_config_t structure.
 */
void LPSPI_SlaveGetDefaultConfig(lpspi_slave_config_t *slaveConfig)
{
    assert(slaveConfig);

    /* Initializes the configure structure to zero. */
    memset(slaveConfig, 0, sizeof(*slaveConfig));

    slaveConfig->bitsPerFrame = 8;                              /*!< Bits per frame, minimum 8, maximum 4096.*/
    slaveConfig->cpol         = kLPSPI_ClockPolarityActiveHigh; /*!< Clock polarity. */
    slaveConfig->cpha         = kLPSPI_ClockPhaseFirstEdge;     /*!< Clock phase. */
    slaveConfig->direction    = kLPSPI_MsbFirst;                /*!< MSB or LSB data shift direction. */

    slaveConfig->whichPcs           = kLPSPI_Pcs0;         /*!< Desired Peripheral Chip Select (pcs) */
    slaveConfig->pcsActiveHighOrLow = kLPSPI_PcsActiveLow; /*!< Desired PCS active high or low */

    slaveConfig->pinCfg        = kLPSPI_SdiInSdoOut;
    slaveConfig->dataOutConfig = kLpspiDataOutRetained;
}

/*!
 * brief Restores the LPSPI peripheral to reset state. Note that this function
 * sets all registers to reset state. As a result, the LPSPI module can't work after calling
 * this API.
 * param base LPSPI peripheral address.
 */
void LPSPI_Reset(LPSPI_Type *base)
{
    /* Reset all internal logic and registers, except the Control Register. Remains set until cleared by software.*/
    base->CR |= LPSPI_CR_RST_MASK;

    /* Software reset doesn't reset the CR, so manual reset the FIFOs */
    base->CR |= LPSPI_CR_RRF_MASK | LPSPI_CR_RTF_MASK;

    /* Master logic is not reset and module is disabled.*/
    base->CR = 0x00U;
}

/*!
 * brief De-initializes the LPSPI peripheral. Call this API to disable the LPSPI clock.
 * param base LPSPI peripheral address.
 */
void LPSPI_Deinit(LPSPI_Type *base)
{
    /* Reset to default value */
    LPSPI_Reset(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)

    uint32_t instance = LPSPI_GetInstance(base);
    /* Enable LPSPI clock */
    CLOCK_DisableClock(s_lpspiClocks[instance]);

#if defined(LPSPI_PERIPH_CLOCKS)
    CLOCK_DisableClock(s_LpspiPeriphClocks[instance]);
#endif

#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

static void LPSPI_SetOnePcsPolarity(LPSPI_Type *base,
                                    lpspi_which_pcs_t pcs,
                                    lpspi_pcs_polarity_config_t activeLowOrHigh)
{
    uint32_t cfgr1Value = 0;
    /* Clear the PCS polarity bit */
    cfgr1Value = base->CFGR1 & ~(1U << (LPSPI_CFGR1_PCSPOL_SHIFT + pcs));

    /* Configure the PCS polarity bit according to the activeLowOrHigh setting */
    base->CFGR1 = cfgr1Value | ((uint32_t)activeLowOrHigh << (LPSPI_CFGR1_PCSPOL_SHIFT + pcs));
}

/*!
 * brief Sets the LPSPI baud rate in bits per second.
 *
 * This function takes in the desired bitsPerSec (baud rate) and calculates the nearest
 * possible baud rate without exceeding the desired baud rate and returns the
 * calculated baud rate in bits-per-second. It requires the caller to provide
 * the frequency of the module source clock (in Hertz). Note that the baud rate
 * does not go into effect until the Transmit Control Register (TCR) is programmed
 * with the prescale value. Hence, this function returns the prescale tcrPrescaleValue
 * parameter for later programming in the TCR.  The higher level
 * peripheral driver should alert the user of an out of range baud rate input.
 *
 * Note that the LPSPI module must first be disabled before configuring this.
 * Note that the LPSPI module must be configured for master mode before configuring this.
 *
 * param base LPSPI peripheral address.
 * param baudRate_Bps The desired baud rate in bits per second.
 * param srcClock_Hz Module source input clock in Hertz.
 * param tcrPrescaleValue The TCR prescale value needed to program the TCR.
 * return  The actual calculated baud rate. This function may also return a "0" if the
 *          LPSPI is not configured for master mode or if the LPSPI module is not disabled.
 */

uint32_t LPSPI_MasterSetBaudRate(LPSPI_Type *base,
                                 uint32_t baudRate_Bps,
                                 uint32_t srcClock_Hz,
                                 uint32_t *tcrPrescaleValue)
{
    assert(tcrPrescaleValue);

    /* For master mode configuration only, if slave mode detected, return 0.
     * Also, the LPSPI module needs to be disabled first, if enabled, return 0
     */
    if ((!LPSPI_IsMaster(base)) || (base->CR & LPSPI_CR_MEN_MASK))
    {
        return 0;
    }

    uint32_t prescaler, bestPrescaler;
    uint32_t scaler, bestScaler;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t desiredBaudrate = baudRate_Bps;

    /* find combination of prescaler and scaler resulting in baudrate closest to the
     * requested value
     */
    min_diff = 0xFFFFFFFFU;

    /* Set to maximum divisor value bit settings so that if baud rate passed in is less
     * than the minimum possible baud rate, then the SPI will be configured to the lowest
     * possible baud rate
     */
    bestPrescaler = 7;
    bestScaler    = 255;

    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler < 8) && min_diff; prescaler++)
    {
        for (scaler = 0; (scaler < 256) && min_diff; scaler++)
        {
            realBaudrate = (srcClock_Hz / (s_baudratePrescaler[prescaler] * (scaler + 2U)));

            /* calculate the baud rate difference based on the conditional statement
             * that states that the calculated baud rate must not exceed the desired baud rate
             */
            if (desiredBaudrate >= realBaudrate)
            {
                diff = desiredBaudrate - realBaudrate;
                if (min_diff > diff)
                {
                    /* a better match found */
                    min_diff      = diff;
                    bestPrescaler = prescaler;
                    bestScaler    = scaler;
                    bestBaudrate  = realBaudrate;
                }
            }
        }
    }

    /* Write the best baud rate scalar to the CCR.
     * Note, no need to check for error since we've already checked to make sure the module is
     * disabled and in master mode. Also, there is a limit on the maximum divider so we will not
     * exceed this.
     */
    base->CCR = (base->CCR & ~LPSPI_CCR_SCKDIV_MASK) | LPSPI_CCR_SCKDIV(bestScaler);

    /* return the best prescaler value for user to use later */
    *tcrPrescaleValue = bestPrescaler;

    /* return the actual calculated baud rate */
    return bestBaudrate;
}

/*!
 * brief Manually configures a specific LPSPI delay parameter (module must be disabled to
 *        change the delay values).
 *
 * This function configures the following:
 * SCK to PCS delay, or
 * PCS to SCK delay, or
 * The configurations must occur between the transfer delay.
 *
 * The delay names are available in type lpspi_delay_type_t.
 *
 * The user passes the desired delay along with the delay value.
 * This allows the user to directly set the delay values if they have
 * pre-calculated them or if they simply wish to manually increment the value.
 *
 * Note that the LPSPI module must first be disabled before configuring this.
 * Note that the LPSPI module must be configured for master mode before configuring this.
 *
 * param base LPSPI peripheral address.
 * param scaler The 8-bit delay value 0x00 to 0xFF (255).
 * param whichDelay The desired delay to configure, must be of type lpspi_delay_type_t.
 */
void LPSPI_MasterSetDelayScaler(LPSPI_Type *base, uint32_t scaler, lpspi_delay_type_t whichDelay)
{
    /*These settings are only relevant in master mode */
    switch (whichDelay)
    {
        case kLPSPI_PcsToSck:
            base->CCR = (base->CCR & (~LPSPI_CCR_PCSSCK_MASK)) | LPSPI_CCR_PCSSCK(scaler);

            break;
        case kLPSPI_LastSckToPcs:
            base->CCR = (base->CCR & (~LPSPI_CCR_SCKPCS_MASK)) | LPSPI_CCR_SCKPCS(scaler);

            break;
        case kLPSPI_BetweenTransfer:
            base->CCR = (base->CCR & (~LPSPI_CCR_DBT_MASK)) | LPSPI_CCR_DBT(scaler);

            break;
        default:
            assert(false);
            break;
    }
}

/*!
 * brief Calculates the delay based on the desired delay input in nanoseconds (module must be
 *        disabled to change the delay values).
 *
 * This function calculates the values for the following:
 * SCK to PCS delay, or
 * PCS to SCK delay, or
 * The configurations must occur between the transfer delay.
 *
 * The delay names are available in type lpspi_delay_type_t.
 *
 * The user passes the desired delay and the desired delay value in
 * nano-seconds.  The function calculates the value needed for the desired delay parameter
 * and returns the actual calculated delay because an exact delay match may not be possible. In this
 * case, the closest match is calculated without going below the desired delay value input.
 * It is possible to input a very large delay value that exceeds the capability of the part, in
 * which case the maximum supported delay is returned. It is up to the higher level
 * peripheral driver to alert the user of an out of range delay input.
 *
 * Note that the LPSPI module must be configured for master mode before configuring this. And note that
 * the delayTime = LPSPI_clockSource / (PRESCALE * Delay_scaler).
 *
 * param base LPSPI peripheral address.
 * param delayTimeInNanoSec The desired delay value in nano-seconds.
 * param whichDelay The desired delay to configuration, which must be of type lpspi_delay_type_t.
 * param srcClock_Hz  Module source input clock in Hertz.
 * return actual Calculated delay value in nano-seconds.
 */
uint32_t LPSPI_MasterSetDelayTimes(LPSPI_Type *base,
                                   uint32_t delayTimeInNanoSec,
                                   lpspi_delay_type_t whichDelay,
                                   uint32_t srcClock_Hz)
{
    uint64_t realDelay, bestDelay;
    uint32_t scaler, bestScaler;
    uint32_t diff, min_diff;
    uint64_t initialDelayNanoSec;
    uint32_t clockDividedPrescaler;

    /* For delay between transfer, an additional scaler value is needed */
    uint32_t additionalScaler = 0;

    /*As the RM note, the LPSPI baud rate clock is itself divided by the PRESCALE setting, which can vary between
     * transfers.*/
    clockDividedPrescaler =
        srcClock_Hz / s_baudratePrescaler[(base->TCR & LPSPI_TCR_PRESCALE_MASK) >> LPSPI_TCR_PRESCALE_SHIFT];

    /* Find combination of prescaler and scaler resulting in the delay closest to the requested value.*/
    min_diff = 0xFFFFFFFFU;

    /* Initialize scaler to max value to generate the max delay */
    bestScaler = 0xFFU;

    /* Calculate the initial (min) delay and maximum possible delay based on the specific delay as
     * the delay divisors are slightly different based on which delay we are configuring.
     */
    if (whichDelay == kLPSPI_BetweenTransfer)
    {
        /* First calculate the initial, default delay, note min delay is 2 clock cycles. Due to large size of
         calculated values (uint64_t), we need to break up the calculation into several steps to ensure
         accurate calculated results
         */
        initialDelayNanoSec = 1000000000U;
        initialDelayNanoSec *= 2U;
        initialDelayNanoSec /= clockDividedPrescaler;

        /* Calculate the maximum delay */
        bestDelay = 1000000000U;
        bestDelay *= 257U; /* based on DBT+2, or 255 + 2 */
        bestDelay /= clockDividedPrescaler;

        additionalScaler = 1U;
    }
    else
    {
        /* First calculate the initial, default delay, min delay is 1 clock cycle. Due to large size of calculated
        values (uint64_t), we need to break up the calculation into several steps to ensure accurate calculated
        results.
        */
        initialDelayNanoSec = 1000000000U;
        initialDelayNanoSec /= clockDividedPrescaler;

        /* Calculate the maximum delay */
        bestDelay = 1000000000U;
        bestDelay *= 256U; /* based on SCKPCS+1 or PCSSCK+1, or 255 + 1 */
        bestDelay /= clockDividedPrescaler;

        additionalScaler = 0;
    }

    /* If the initial, default delay is already greater than the desired delay, then
     * set the delay to their initial value (0) and return the delay. In other words,
     * there is no way to decrease the delay value further.
     */
    if (initialDelayNanoSec >= delayTimeInNanoSec)
    {
        LPSPI_MasterSetDelayScaler(base, 0, whichDelay);
        return initialDelayNanoSec;
    }

    /* If min_diff = 0, the exit for loop */
    for (scaler = 0; (scaler < 256U) && min_diff; scaler++)
    {
        /* Calculate the real delay value as we cycle through the scaler values.
        Due to large size of calculated values (uint64_t), we need to break up the
        calculation into several steps to ensure accurate calculated results
        */
        realDelay = 1000000000U;
        realDelay *= (scaler + 1 + additionalScaler);
        realDelay /= clockDividedPrescaler;

        /* calculate the delay difference based on the conditional statement
         * that states that the calculated delay must not be less then the desired delay
         */
        if (realDelay >= delayTimeInNanoSec)
        {
            diff = realDelay - delayTimeInNanoSec;
            if (min_diff > diff)
            {
                /* a better match found */
                min_diff   = diff;
                bestScaler = scaler;
                bestDelay  = realDelay;
            }
        }
    }

    /* write the best scaler value for the delay */
    LPSPI_MasterSetDelayScaler(base, bestScaler, whichDelay);

    /* return the actual calculated delay value (in ns) */
    return bestDelay;
}

/*Transactional APIs -- Master*/

/*!
 * brief Initializes the LPSPI master handle.
 *
 * This function initializes the LPSPI handle, which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.

 * param base LPSPI peripheral address.
 * param handle LPSPI handle pointer to lpspi_master_handle_t.
 * param callback DSPI callback.
 * param userData callback function parameter.
 */
void LPSPI_MasterTransferCreateHandle(LPSPI_Type *base,
                                      lpspi_master_handle_t *handle,
                                      lpspi_master_transfer_callback_t callback,
                                      void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_lpspiHandle[LPSPI_GetInstance(base)] = handle;

    /* Set irq handler. */
    s_lpspiMasterIsr = LPSPI_MasterTransferHandleIRQ;

    handle->callback = callback;
    handle->userData = userData;
}

/*!
 * brief Check the argument for transfer .
 *
 * param transfer the transfer struct to be used.
 * param bitPerFrame The bit size of one frame.
 * param bytePerFrame The byte size of one frame.
 * return Return true for right and false for wrong.
 */
bool LPSPI_CheckTransferArgument(lpspi_transfer_t *transfer, uint32_t bitsPerFrame, uint32_t bytesPerFrame)
{
    assert(transfer);

    /* If the transfer count is zero, then return immediately.*/
    if (transfer->dataSize == 0)
    {
        return false;
    }

    /* If both send buffer and receive buffer is null */
    if ((!(transfer->txData)) && (!(transfer->rxData)))
    {
        return false;
    }

    /*The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4 .
     *For bytesPerFrame greater than 4 situation:
     *the transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not integer multiples of 4 ,
     *otherwise , the transfer data size can be integer multiples of bytesPerFrame.
     */
    if (bytesPerFrame <= 4)
    {
        if ((transfer->dataSize % bytesPerFrame) != 0)
        {
            return false;
        }
    }
    else
    {
        if ((bytesPerFrame % 4U) != 0)
        {
            if (transfer->dataSize != bytesPerFrame)
            {
                return false;
            }
        }
        else
        {
            if ((transfer->dataSize % bytesPerFrame) != 0)
            {
                return false;
            }
        }
    }

    return true;
}

/*!
 * brief LPSPI master transfer data using a polling method.
 *
 * This function transfers data using a  polling method. This is a blocking function, which does not return until all
 * transfers have been
 * completed.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not integer multiples of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * param base LPSPI peripheral address.
 * param transfer pointer to lpspi_transfer_t structure.
 * return status of status_t.
 */
status_t LPSPI_MasterTransferBlocking(LPSPI_Type *base, lpspi_transfer_t *transfer)
{
    assert(transfer);

    uint32_t bitsPerFrame  = ((base->TCR & LPSPI_TCR_FRAMESZ_MASK) >> LPSPI_TCR_FRAMESZ_SHIFT) + 1;
    uint32_t bytesPerFrame = (bitsPerFrame + 7) / 8;
    uint32_t temp          = 0U;
    uint8_t dummyData      = g_lpspiDummyData[LPSPI_GetInstance(base)];

    if (!LPSPI_CheckTransferArgument(transfer, bitsPerFrame, bytesPerFrame))
    {
        return kStatus_InvalidArgument;
    }

    /* Check that LPSPI is not busy.*/
    if (LPSPI_GetStatusFlags(base) & kLPSPI_ModuleBusyFlag)
    {
        return kStatus_LPSPI_Busy;
    }

    uint8_t *txData               = transfer->txData;
    uint8_t *rxData               = transfer->rxData;
    uint32_t txRemainingByteCount = transfer->dataSize;
    uint32_t rxRemainingByteCount = transfer->dataSize;

    uint8_t bytesEachWrite;
    uint8_t bytesEachRead;

    uint32_t readData = 0;
    uint32_t wordToSend =
        ((uint32_t)dummyData) | ((uint32_t)dummyData << 8) | ((uint32_t)dummyData << 16) | ((uint32_t)dummyData << 24);

    /*The TX and RX FIFO sizes are always the same*/
    uint32_t fifoSize = LPSPI_GetRxFifoSize(base);

    uint32_t whichPcs = (transfer->configFlags & LPSPI_MASTER_PCS_MASK) >> LPSPI_MASTER_PCS_SHIFT;

    bool isPcsContinuous = (bool)(transfer->configFlags & kLPSPI_MasterPcsContinuous);
    bool isRxMask        = false;
    bool isByteSwap      = (bool)(transfer->configFlags & kLPSPI_MasterByteSwap);

    LPSPI_FlushFifo(base, true, true);
    LPSPI_ClearStatusFlags(base, kLPSPI_AllStatusFlag);

    if (!rxData)
    {
        isRxMask = true;
    }

    LPSPI_Enable(base, false);
    base->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    /* Check if using 3-wire mode and the txData is NULL, set the output pin to tristated. */
    temp = base->CFGR1;
    temp &= LPSPI_CFGR1_PINCFG_MASK;
    if ((temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdiInSdiOut)) || (temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdoInSdoOut)))
    {
        if (!txData)
        {
            base->CFGR1 |= LPSPI_CFGR1_OUTCFG_MASK;
        }
        /* The 3-wire mode can't send and receive data at the same time. */
        if ((txData) && (rxData))
        {
            return kStatus_InvalidArgument;
        }
    }
    LPSPI_Enable(base, true);

    base->TCR =
        (base->TCR & ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_PCS_MASK)) |
        LPSPI_TCR_CONT(isPcsContinuous) | LPSPI_TCR_CONTC(0) | LPSPI_TCR_RXMSK(isRxMask) | LPSPI_TCR_PCS(whichPcs);

    if (bytesPerFrame <= 4)
    {
        bytesEachWrite = bytesPerFrame;
        bytesEachRead  = bytesPerFrame;
    }
    else
    {
        bytesEachWrite = 4;
        bytesEachRead  = 4;
    }

    /*Write the TX data until txRemainingByteCount is equal to 0 */
    while (txRemainingByteCount > 0)
    {
        if (txRemainingByteCount < bytesEachWrite)
        {
            bytesEachWrite = txRemainingByteCount;
        }

        /*Wait until TX FIFO is not full*/
        while (LPSPI_GetTxFifoCount(base) == fifoSize)
        {
        }

        if (txData)
        {
            wordToSend = LPSPI_CombineWriteData(txData, bytesEachWrite, isByteSwap);
            txData += bytesEachWrite;
        }

        LPSPI_WriteData(base, wordToSend);
        txRemainingByteCount -= bytesEachWrite;

        /*Check whether there is RX data in RX FIFO . Read out the RX data so that the RX FIFO would not overrun.*/
        if (rxData)
        {
            while (LPSPI_GetRxFifoCount(base))
            {
                readData = LPSPI_ReadData(base);
                if (rxRemainingByteCount < bytesEachRead)
                {
                    bytesEachRead = rxRemainingByteCount;
                }

                LPSPI_SeparateReadData(rxData, readData, bytesEachRead, isByteSwap);
                rxData += bytesEachRead;

                rxRemainingByteCount -= bytesEachRead;
            }
        }
    }

    /* After write all the data in TX FIFO , should write the TCR_CONTC to 0 to de-assert the PCS. Note that TCR
     * register also use the TX FIFO.
     */
    while ((LPSPI_GetTxFifoCount(base) == fifoSize))
    {
    }
    base->TCR = (base->TCR & ~(LPSPI_TCR_CONTC_MASK));

    /*Read out the RX data in FIFO*/
    if (rxData)
    {
        while (rxRemainingByteCount > 0)
        {
            while (LPSPI_GetRxFifoCount(base))
            {
                readData = LPSPI_ReadData(base);

                if (rxRemainingByteCount < bytesEachRead)
                {
                    bytesEachRead = rxRemainingByteCount;
                }

                LPSPI_SeparateReadData(rxData, readData, bytesEachRead, isByteSwap);
                rxData += bytesEachRead;

                rxRemainingByteCount -= bytesEachRead;
            }
        }
    }
    else
    {
        /* If no RX buffer, then transfer is not complete until transfer complete flag sets */
        while (!(LPSPI_GetStatusFlags(base) & kLPSPI_TransferCompleteFlag))
        {
        }
    }

    return kStatus_Success;
}

/*!
 * brief LPSPI master transfer data using an interrupt method.
 *
 * This function transfers data using an interrupt method. This is a non-blocking function, which returns right away.
 * When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not integer multiples of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 * param transfer pointer to lpspi_transfer_t structure.
 * return status of status_t.
 */
status_t LPSPI_MasterTransferNonBlocking(LPSPI_Type *base, lpspi_master_handle_t *handle, lpspi_transfer_t *transfer)
{
    assert(handle);
    assert(transfer);

    uint32_t bitsPerFrame  = ((base->TCR & LPSPI_TCR_FRAMESZ_MASK) >> LPSPI_TCR_FRAMESZ_SHIFT) + 1;
    uint32_t bytesPerFrame = (bitsPerFrame + 7) / 8;
    uint32_t temp          = 0U;
    uint8_t dummyData      = g_lpspiDummyData[LPSPI_GetInstance(base)];

    if (!LPSPI_CheckTransferArgument(transfer, bitsPerFrame, bytesPerFrame))
    {
        return kStatus_InvalidArgument;
    }

    /* Check that we're not busy.*/
    if (handle->state == kLPSPI_Busy)
    {
        return kStatus_LPSPI_Busy;
    }

    handle->state = kLPSPI_Busy;

    bool isRxMask = false;

    uint8_t txWatermark;

    uint32_t whichPcs = (transfer->configFlags & LPSPI_MASTER_PCS_MASK) >> LPSPI_MASTER_PCS_SHIFT;

    handle->txData               = transfer->txData;
    handle->rxData               = transfer->rxData;
    handle->txRemainingByteCount = transfer->dataSize;
    handle->rxRemainingByteCount = transfer->dataSize;
    handle->totalByteCount       = transfer->dataSize;

    handle->writeTcrInIsr = false;

    handle->writeRegRemainingTimes = (transfer->dataSize / bytesPerFrame) * ((bytesPerFrame + 3) / 4);
    handle->readRegRemainingTimes  = handle->writeRegRemainingTimes;

    handle->txBuffIfNull =
        ((uint32_t)dummyData) | ((uint32_t)dummyData << 8) | ((uint32_t)dummyData << 16) | ((uint32_t)dummyData << 24);

    /*The TX and RX FIFO sizes are always the same*/
    handle->fifoSize = LPSPI_GetRxFifoSize(base);

    handle->isPcsContinuous = (bool)(transfer->configFlags & kLPSPI_MasterPcsContinuous);
    handle->isByteSwap      = (bool)(transfer->configFlags & kLPSPI_MasterByteSwap);

    /*Set the RX and TX watermarks to reduce the ISR times.*/
    if (handle->fifoSize > 1)
    {
        txWatermark         = 1;
        handle->rxWatermark = handle->fifoSize - 2;
    }
    else
    {
        txWatermark         = 0;
        handle->rxWatermark = 0;
    }

    LPSPI_SetFifoWatermarks(base, txWatermark, handle->rxWatermark);

    LPSPI_Enable(base, false);
    /*Transfers will stall when transmit FIFO is empty or receive FIFO is full. */
    base->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    /* Check if using 3-wire mode and the txData is NULL, set the output pin to tristated. */
    temp = base->CFGR1;
    temp &= LPSPI_CFGR1_PINCFG_MASK;
    if ((temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdiInSdiOut)) || (temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdoInSdoOut)))
    {
        if (!handle->txData)
        {
            base->CFGR1 |= LPSPI_CFGR1_OUTCFG_MASK;
        }
        /* The 3-wire mode can't send and receive data at the same time. */
        if ((handle->txData) && (handle->rxData))
        {
            return kStatus_InvalidArgument;
        }
    }
    LPSPI_Enable(base, true);

    /*Flush FIFO , clear status , disable all the inerrupts.*/
    LPSPI_FlushFifo(base, true, true);
    LPSPI_ClearStatusFlags(base, kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    /* If there is not rxData , can mask the receive data (receive data is not stored in receive FIFO).
     * For master transfer , we'd better not masked the transmit data in TCR since the transfer flow is hard to
     * controlled by software.*/
    if (handle->rxData == NULL)
    {
        isRxMask                     = true;
        handle->rxRemainingByteCount = 0;
    }

    base->TCR =
        (base->TCR & ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_PCS_MASK)) |
        LPSPI_TCR_CONT(handle->isPcsContinuous) | LPSPI_TCR_CONTC(0) | LPSPI_TCR_RXMSK(isRxMask) |
        LPSPI_TCR_PCS(whichPcs);

    /*Calculate the bytes for write/read the TX/RX register each time*/
    if (bytesPerFrame <= 4)
    {
        handle->bytesEachWrite = bytesPerFrame;
        handle->bytesEachRead  = bytesPerFrame;
    }
    else
    {
        handle->bytesEachWrite = 4;
        handle->bytesEachRead  = 4;
    }

    /* Enable the NVIC for LPSPI peripheral. Note that below code is useless if the LPSPI interrupt is in INTMUX ,
     * and you should also enable the INTMUX interupt in your application.
     */
    EnableIRQ(s_lpspiIRQ[LPSPI_GetInstance(base)]);

    /*TCR is also shared the FIFO , so wait for TCR written.*/
    while (LPSPI_GetTxFifoCount(base) != 0)
    {
    }
    /*Fill up the TX data in FIFO */
    LPSPI_MasterTransferFillUpTxFifo(base, handle);

    /* Since SPI is a synchronous interface, we only need to enable the RX interrupt if there is RX data.
     * The IRQ handler will get the status of RX and TX interrupt flags.
     */
    if (handle->rxData)
    {
        /*Set rxWatermark to (readRegRemainingTimes-1) if readRegRemainingTimes less than rxWatermark. Otherwise there
         *is not RX interrupt for the last datas because the RX count is not greater than rxWatermark.
         */
        if ((handle->readRegRemainingTimes) <= handle->rxWatermark)
        {
            base->FCR = (base->FCR & (~LPSPI_FCR_RXWATER_MASK)) | LPSPI_FCR_RXWATER(handle->readRegRemainingTimes - 1);
        }

        LPSPI_EnableInterrupts(base, kLPSPI_RxInterruptEnable);
    }
    else
    {
        LPSPI_EnableInterrupts(base, kLPSPI_TxInterruptEnable);
    }

    return kStatus_Success;
}

static void LPSPI_MasterTransferFillUpTxFifo(LPSPI_Type *base, lpspi_master_handle_t *handle)
{
    assert(handle);

    uint32_t wordToSend = 0;

    /* Make sure the difference in remaining TX and RX byte counts does not exceed FIFO depth
     * and that the number of TX FIFO entries does not exceed the FIFO depth.
     * But no need to make the protection if there is no rxData.
     */
    while ((LPSPI_GetTxFifoCount(base) < (handle->fifoSize)) &&
           (((handle->readRegRemainingTimes - handle->writeRegRemainingTimes) < handle->fifoSize) ||
            (handle->rxData == NULL)))
    {
        if (handle->txRemainingByteCount < handle->bytesEachWrite)
        {
            handle->bytesEachWrite = handle->txRemainingByteCount;
        }

        if (handle->txData)
        {
            wordToSend = LPSPI_CombineWriteData(handle->txData, handle->bytesEachWrite, handle->isByteSwap);
            handle->txData += handle->bytesEachWrite;
        }
        else
        {
            wordToSend = handle->txBuffIfNull;
        }

        /*Write the word to TX register*/
        LPSPI_WriteData(base, wordToSend);

        /*Decrease the write TX register times.*/
        --handle->writeRegRemainingTimes;

        /*Decrease the remaining TX byte count.*/
        handle->txRemainingByteCount -= handle->bytesEachWrite;

        if (handle->txRemainingByteCount == 0)
        {
            /* If PCS is continuous, update TCR to de-assert PCS */
            if (handle->isPcsContinuous)
            {
                /* Only write to the TCR if the FIFO has room */
                if ((LPSPI_GetTxFifoCount(base) < (handle->fifoSize)))
                {
                    base->TCR             = (base->TCR & ~(LPSPI_TCR_CONTC_MASK));
                    handle->writeTcrInIsr = false;
                }
                /* Else, set a global flag to tell the ISR to do write to the TCR */
                else
                {
                    handle->writeTcrInIsr = true;
                }
            }
            break;
        }
    }
}

static void LPSPI_MasterTransferComplete(LPSPI_Type *base, lpspi_master_handle_t *handle)
{
    assert(handle);

    /* Disable interrupt requests*/
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    handle->state = kLPSPI_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, kStatus_Success, handle->userData);
    }
}

/*!
 * brief Gets the master transfer remaining bytes.
 *
 * This function gets the master transfer remaining bytes.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 * param count Number of bytes transferred so far by the non-blocking transaction.
 * return status of status_t.
 */
status_t LPSPI_MasterTransferGetCount(LPSPI_Type *base, lpspi_master_handle_t *handle, size_t *count)
{
    assert(handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kLPSPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    size_t remainingByte;

    if (handle->rxData)
    {
        remainingByte = handle->rxRemainingByteCount;
    }
    else
    {
        remainingByte = handle->txRemainingByteCount;
    }

    *count = handle->totalByteCount - remainingByte;

    return kStatus_Success;
}

/*!
 * brief LPSPI master abort transfer which uses an interrupt method.
 *
 * This function aborts a transfer which uses an interrupt method.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferAbort(LPSPI_Type *base, lpspi_master_handle_t *handle)
{
    assert(handle);

    /* Disable interrupt requests*/
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    LPSPI_Reset(base);

    handle->state                = kLPSPI_Idle;
    handle->txRemainingByteCount = 0;
    handle->rxRemainingByteCount = 0;
}

/*!
 * brief LPSPI Master IRQ handler function.
 *
 * This function processes the LPSPI transmit and receive IRQ.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferHandleIRQ(LPSPI_Type *base, lpspi_master_handle_t *handle)
{
    assert(handle);

    uint32_t readData;

    if (handle->rxData != NULL)
    {
        if (handle->rxRemainingByteCount)
        {
            /* First, disable the interrupts to avoid potentially triggering another interrupt
             * while reading out the RX FIFO as more data may be coming into the RX FIFO. We'll
             * re-enable the interrupts based on the LPSPI state after reading out the FIFO.
             */
            LPSPI_DisableInterrupts(base, kLPSPI_RxInterruptEnable);

            while ((LPSPI_GetRxFifoCount(base)) && (handle->rxRemainingByteCount))
            {
                /*Read out the data*/
                readData = LPSPI_ReadData(base);

                /*Decrease the read RX register times.*/
                --handle->readRegRemainingTimes;

                if (handle->rxRemainingByteCount < handle->bytesEachRead)
                {
                    handle->bytesEachRead = handle->rxRemainingByteCount;
                }

                LPSPI_SeparateReadData(handle->rxData, readData, handle->bytesEachRead, handle->isByteSwap);
                handle->rxData += handle->bytesEachRead;

                /*Decrease the remaining RX byte count.*/
                handle->rxRemainingByteCount -= handle->bytesEachRead;
            }

            /* Re-enable the interrupts only if rxCount indicates there is more data to receive,
             * else we may get a spurious interrupt.
             * */
            if (handle->rxRemainingByteCount)
            {
                /* Set the TDF and RDF interrupt enables simultaneously to avoid race conditions */
                LPSPI_EnableInterrupts(base, kLPSPI_RxInterruptEnable);
            }
        }

        /*Set rxWatermark to (readRegRemainingTimes-1) if readRegRemainingTimes less than rxWatermark. Otherwise there
         *is not RX interrupt for the last datas because the RX count is not greater than rxWatermark.
         */
        if ((handle->readRegRemainingTimes) <= (handle->rxWatermark))
        {
            base->FCR =
                (base->FCR & (~LPSPI_FCR_RXWATER_MASK)) |
                LPSPI_FCR_RXWATER((handle->readRegRemainingTimes > 1) ? (handle->readRegRemainingTimes - 1U) : (0U));
        }
    }

    if (handle->txRemainingByteCount)
    {
        LPSPI_MasterTransferFillUpTxFifo(base, handle);
    }
    else
    {
        if ((LPSPI_GetTxFifoCount(base) < (handle->fifoSize)))
        {
            if ((handle->isPcsContinuous) && (handle->writeTcrInIsr))
            {
                base->TCR             = (base->TCR & ~(LPSPI_TCR_CONTC_MASK));
                handle->writeTcrInIsr = false;
            }
        }
    }

    if ((handle->txRemainingByteCount == 0) && (handle->rxRemainingByteCount == 0) && (!handle->writeTcrInIsr))
    {
        /* If no RX buffer, then transfer is not complete until transfer complete flag sets */
        if (handle->rxData == NULL)
        {
            if (LPSPI_GetStatusFlags(base) & kLPSPI_TransferCompleteFlag)
            {
                /* Complete the transfer and disable the interrupts */
                LPSPI_MasterTransferComplete(base, handle);
            }
            else
            {
                LPSPI_EnableInterrupts(base, kLPSPI_TransferCompleteInterruptEnable);
                LPSPI_DisableInterrupts(base, kLPSPI_TxInterruptEnable | kLPSPI_RxInterruptEnable);
            }
        }
        else
        {
            /* Complete the transfer and disable the interrupts */
            LPSPI_MasterTransferComplete(base, handle);
        }
    }
}

/*Transactional APIs -- Slave*/
/*!
 * brief Initializes the LPSPI slave handle.
 *
 * This function initializes the LPSPI handle, which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * param base LPSPI peripheral address.
 * param handle LPSPI handle pointer to lpspi_slave_handle_t.
 * param callback DSPI callback.
 * param userData callback function parameter.
 */
void LPSPI_SlaveTransferCreateHandle(LPSPI_Type *base,
                                     lpspi_slave_handle_t *handle,
                                     lpspi_slave_transfer_callback_t callback,
                                     void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_lpspiHandle[LPSPI_GetInstance(base)] = handle;

    /* Set irq handler. */
    s_lpspiSlaveIsr = LPSPI_SlaveTransferHandleIRQ;

    handle->callback = callback;
    handle->userData = userData;
}

/*!
 * brief LPSPI slave transfer data using an interrupt method.
 *
 * This function transfer data using an interrupt method. This is a non-blocking function, which returns right away.
 * When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 * param transfer pointer to lpspi_transfer_t structure.
 * return status of status_t.
 */
status_t LPSPI_SlaveTransferNonBlocking(LPSPI_Type *base, lpspi_slave_handle_t *handle, lpspi_transfer_t *transfer)
{
    assert(handle);
    assert(transfer);

    uint32_t bitsPerFrame  = ((base->TCR & LPSPI_TCR_FRAMESZ_MASK) >> LPSPI_TCR_FRAMESZ_SHIFT) + 1;
    uint32_t bytesPerFrame = (bitsPerFrame + 7) / 8;
    uint32_t temp          = 0U;

    if (!LPSPI_CheckTransferArgument(transfer, bitsPerFrame, bytesPerFrame))
    {
        return kStatus_InvalidArgument;
    }

    /* Check that we're not busy.*/
    if (handle->state == kLPSPI_Busy)
    {
        return kStatus_LPSPI_Busy;
    }
    handle->state = kLPSPI_Busy;

    bool isRxMask = false;
    bool isTxMask = false;

    uint32_t whichPcs = (transfer->configFlags & LPSPI_SLAVE_PCS_MASK) >> LPSPI_SLAVE_PCS_SHIFT;

    handle->txData               = transfer->txData;
    handle->rxData               = transfer->rxData;
    handle->txRemainingByteCount = transfer->dataSize;
    handle->rxRemainingByteCount = transfer->dataSize;
    handle->totalByteCount       = transfer->dataSize;

    handle->writeRegRemainingTimes = (transfer->dataSize / bytesPerFrame) * ((bytesPerFrame + 3) / 4);
    handle->readRegRemainingTimes  = handle->writeRegRemainingTimes;

    /*The TX and RX FIFO sizes are always the same*/
    handle->fifoSize = LPSPI_GetRxFifoSize(base);

    handle->isByteSwap = (bool)(transfer->configFlags & kLPSPI_SlaveByteSwap);

    /*Set the RX and TX watermarks to reduce the ISR times.*/
    uint8_t txWatermark;
    if (handle->fifoSize > 1)
    {
        txWatermark         = 1;
        handle->rxWatermark = handle->fifoSize - 2;
    }
    else
    {
        txWatermark         = 0;
        handle->rxWatermark = 0;
    }
    LPSPI_SetFifoWatermarks(base, txWatermark, handle->rxWatermark);

    /* Check if using 3-wire mode and the txData is NULL, set the output pin to tristated. */
    temp = base->CFGR1;
    temp &= LPSPI_CFGR1_PINCFG_MASK;
    if ((temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdiInSdiOut)) || (temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdoInSdoOut)))
    {
        if (!handle->txData)
        {
            LPSPI_Enable(base, false);
            base->CFGR1 |= LPSPI_CFGR1_OUTCFG_MASK;
            LPSPI_Enable(base, true);
        }
        /* The 3-wire mode can't send and receive data at the same time. */
        if ((handle->txData) && (handle->rxData))
        {
            return kStatus_InvalidArgument;
        }
    }

    /*Flush FIFO , clear status , disable all the inerrupts.*/
    LPSPI_FlushFifo(base, true, true);
    LPSPI_ClearStatusFlags(base, kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    /*If there is not rxData , can mask the receive data (receive data is not stored in receive FIFO).*/
    if (handle->rxData == NULL)
    {
        isRxMask                     = true;
        handle->rxRemainingByteCount = 0;
    }

    /*If there is not txData , can mask the transmit data (no data is loaded from transmit FIFO and output pin
     * is tristated).
     */
    if (handle->txData == NULL)
    {
        isTxMask                     = true;
        handle->txRemainingByteCount = 0;
    }

    base->TCR = (base->TCR & ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_RXMSK_MASK |
                               LPSPI_TCR_TXMSK_MASK | LPSPI_TCR_PCS_MASK)) |
                LPSPI_TCR_CONT(0) | LPSPI_TCR_CONTC(0) | LPSPI_TCR_RXMSK(isRxMask) | LPSPI_TCR_TXMSK(isTxMask) |
                LPSPI_TCR_PCS(whichPcs);

    /*Calculate the bytes for write/read the TX/RX register each time*/
    if (bytesPerFrame <= 4)
    {
        handle->bytesEachWrite = bytesPerFrame;
        handle->bytesEachRead  = bytesPerFrame;
    }
    else
    {
        handle->bytesEachWrite = 4;
        handle->bytesEachRead  = 4;
    }

    /* Enable the NVIC for LPSPI peripheral. Note that below code is useless if the LPSPI interrupt is in INTMUX ,
     * and you should also enable the INTMUX interupt in your application.
     */
    EnableIRQ(s_lpspiIRQ[LPSPI_GetInstance(base)]);

    /*TCR is also shared the FIFO , so wait for TCR written.*/
    while (LPSPI_GetTxFifoCount(base) != 0)
    {
    }

    /*Fill up the TX data in FIFO */
    if (handle->txData)
    {
        LPSPI_SlaveTransferFillUpTxFifo(base, handle);
    }

    /* Since SPI is a synchronous interface, we only need to enable the RX interrupt if there is RX data.
     * The IRQ handler will get the status of RX and TX interrupt flags.
     */
    if (handle->rxData)
    {
        /*Set rxWatermark to (readRegRemainingTimes-1) if readRegRemainingTimes less than rxWatermark. Otherwise there
         *is not RX interrupt for the last datas because the RX count is not greater than rxWatermark.
         */
        if ((handle->readRegRemainingTimes) <= handle->rxWatermark)
        {
            base->FCR = (base->FCR & (~LPSPI_FCR_RXWATER_MASK)) | LPSPI_FCR_RXWATER(handle->readRegRemainingTimes - 1);
        }

        LPSPI_EnableInterrupts(base, kLPSPI_RxInterruptEnable);
    }
    else
    {
        LPSPI_EnableInterrupts(base, kLPSPI_TxInterruptEnable);
    }

    if (handle->rxData)
    {
        /* RX FIFO overflow request enable */
        LPSPI_EnableInterrupts(base, kLPSPI_ReceiveErrorInterruptEnable);
    }
    if (handle->txData)
    {
        /* TX FIFO underflow request enable */
        LPSPI_EnableInterrupts(base, kLPSPI_TransmitErrorInterruptEnable);
    }

    return kStatus_Success;
}

static void LPSPI_SlaveTransferFillUpTxFifo(LPSPI_Type *base, lpspi_slave_handle_t *handle)
{
    assert(handle);

    uint32_t wordToSend = 0;

    while (LPSPI_GetTxFifoCount(base) < (handle->fifoSize))
    {
        if (handle->txRemainingByteCount < handle->bytesEachWrite)
        {
            handle->bytesEachWrite = handle->txRemainingByteCount;
        }

        wordToSend = LPSPI_CombineWriteData(handle->txData, handle->bytesEachWrite, handle->isByteSwap);
        handle->txData += handle->bytesEachWrite;

        /*Decrease the remaining TX byte count.*/
        handle->txRemainingByteCount -= handle->bytesEachWrite;

        /*Write the word to TX register*/
        LPSPI_WriteData(base, wordToSend);

        if (handle->txRemainingByteCount == 0)
        {
            break;
        }
    }
}

static void LPSPI_SlaveTransferComplete(LPSPI_Type *base, lpspi_slave_handle_t *handle)
{
    assert(handle);

    status_t status = 0;

    /* Disable interrupt requests*/
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    if (handle->state == kLPSPI_Error)
    {
        status = kStatus_LPSPI_Error;
    }
    else
    {
        status = kStatus_Success;
    }

    handle->state = kLPSPI_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

/*!
 * brief Gets the slave transfer remaining bytes.
 *
 * This function gets the slave transfer remaining bytes.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 * param count Number of bytes transferred so far by the non-blocking transaction.
 * return status of status_t.
 */
status_t LPSPI_SlaveTransferGetCount(LPSPI_Type *base, lpspi_slave_handle_t *handle, size_t *count)
{
    assert(handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kLPSPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    size_t remainingByte;

    if (handle->rxData)
    {
        remainingByte = handle->rxRemainingByteCount;
    }
    else
    {
        remainingByte = handle->txRemainingByteCount;
    }

    *count = handle->totalByteCount - remainingByte;

    return kStatus_Success;
}

/*!
 * brief LPSPI slave aborts a transfer which uses an interrupt method.
 *
 * This function aborts a transfer which uses an interrupt method.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferAbort(LPSPI_Type *base, lpspi_slave_handle_t *handle)
{
    assert(handle);

    /* Disable interrupt requests*/
    LPSPI_DisableInterrupts(base, kLPSPI_TxInterruptEnable | kLPSPI_RxInterruptEnable);

    LPSPI_Reset(base);

    handle->state                = kLPSPI_Idle;
    handle->txRemainingByteCount = 0;
    handle->rxRemainingByteCount = 0;
}

/*!
 * brief LPSPI Slave IRQ handler function.
 *
 * This function processes the LPSPI transmit and receives an IRQ.
 *
 * param base LPSPI peripheral address.
 * param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferHandleIRQ(LPSPI_Type *base, lpspi_slave_handle_t *handle)
{
    assert(handle);

    uint32_t readData;   /* variable to store word read from RX FIFO */
    uint32_t wordToSend; /* variable to store word to write to TX FIFO */

    if (handle->rxData != NULL)
    {
        if (handle->rxRemainingByteCount > 0)
        {
            while (LPSPI_GetRxFifoCount(base))
            {
                /*Read out the data*/
                readData = LPSPI_ReadData(base);

                /*Decrease the read RX register times.*/
                --handle->readRegRemainingTimes;

                if (handle->rxRemainingByteCount < handle->bytesEachRead)
                {
                    handle->bytesEachRead = handle->rxRemainingByteCount;
                }

                LPSPI_SeparateReadData(handle->rxData, readData, handle->bytesEachRead, handle->isByteSwap);
                handle->rxData += handle->bytesEachRead;

                /*Decrease the remaining RX byte count.*/
                handle->rxRemainingByteCount -= handle->bytesEachRead;

                if ((handle->txRemainingByteCount > 0) && (handle->txData != NULL))
                {
                    if (handle->txRemainingByteCount < handle->bytesEachWrite)
                    {
                        handle->bytesEachWrite = handle->txRemainingByteCount;
                    }

                    wordToSend = LPSPI_CombineWriteData(handle->txData, handle->bytesEachWrite, handle->isByteSwap);
                    handle->txData += handle->bytesEachWrite;

                    /*Decrease the remaining TX byte count.*/
                    handle->txRemainingByteCount -= handle->bytesEachWrite;

                    /*Write the word to TX register*/
                    LPSPI_WriteData(base, wordToSend);
                }

                if (handle->rxRemainingByteCount == 0)
                {
                    break;
                }
            }
        }

        /*Set rxWatermark to (readRegRemainingTimes-1) if readRegRemainingTimes less than rxWatermark. Otherwise there
         *is not RX interrupt for the last datas because the RX count is not greater than rxWatermark.
         */
        if ((handle->readRegRemainingTimes) <= (handle->rxWatermark))
        {
            base->FCR =
                (base->FCR & (~LPSPI_FCR_RXWATER_MASK)) |
                LPSPI_FCR_RXWATER((handle->readRegRemainingTimes > 1) ? (handle->readRegRemainingTimes - 1U) : (0U));
        }
    }
    else if ((handle->txRemainingByteCount) && (handle->txData != NULL))
    {
        LPSPI_SlaveTransferFillUpTxFifo(base, handle);
    }
    else
    {
        __NOP();
    }

    if ((handle->txRemainingByteCount == 0) && (handle->rxRemainingByteCount == 0))
    {
        /* If no RX buffer, then transfer is not complete until transfer complete flag sets and the TX FIFO empty*/
        if (handle->rxData == NULL)
        {
            if ((LPSPI_GetStatusFlags(base) & kLPSPI_FrameCompleteFlag) && (LPSPI_GetTxFifoCount(base) == 0))
            {
                /* Complete the transfer and disable the interrupts */
                LPSPI_SlaveTransferComplete(base, handle);
            }
            else
            {
                LPSPI_ClearStatusFlags(base, kLPSPI_FrameCompleteFlag);
                LPSPI_EnableInterrupts(base, kLPSPI_FrameCompleteInterruptEnable);
                LPSPI_DisableInterrupts(base, kLPSPI_TxInterruptEnable | kLPSPI_RxInterruptEnable);
            }
        }
        else
        {
            /* Complete the transfer and disable the interrupts */
            LPSPI_SlaveTransferComplete(base, handle);
        }
    }

    /* Catch tx fifo underflow conditions, service only if tx under flow interrupt enabled */
    if ((LPSPI_GetStatusFlags(base) & kLPSPI_TransmitErrorFlag) && (base->IER & LPSPI_IER_TEIE_MASK))
    {
        LPSPI_ClearStatusFlags(base, kLPSPI_TransmitErrorFlag);
        /* Change state to error and clear flag */
        if (handle->txData)
        {
            handle->state = kLPSPI_Error;
        }
        handle->errorCount++;
    }
    /* Catch rx fifo overflow conditions, service only if rx over flow interrupt enabled */
    if ((LPSPI_GetStatusFlags(base) & kLPSPI_ReceiveErrorFlag) && (base->IER & LPSPI_IER_REIE_MASK))
    {
        LPSPI_ClearStatusFlags(base, kLPSPI_ReceiveErrorFlag);
        /* Change state to error and clear flag */
        if (handle->txData)
        {
            handle->state = kLPSPI_Error;
        }
        handle->errorCount++;
    }
}

static uint32_t LPSPI_CombineWriteData(uint8_t *txData, uint32_t bytesEachWrite, bool isByteSwap)
{
    assert(txData);

    uint32_t wordToSend = 0;

    switch (bytesEachWrite)
    {
        case 1:
            wordToSend = *txData;
            ++txData;
            break;

        case 2:
            if (!isByteSwap)
            {
                wordToSend = *txData;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 8U;
                ++txData;
            }
            else
            {
                wordToSend = (unsigned)(*txData) << 8U;
                ++txData;
                wordToSend |= *txData;
                ++txData;
            }

            break;

        case 3:
            if (!isByteSwap)
            {
                wordToSend = *txData;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 8U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 16U;
                ++txData;
            }
            else
            {
                wordToSend = (unsigned)(*txData) << 16U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 8U;
                ++txData;
                wordToSend |= *txData;
                ++txData;
            }
            break;

        case 4:
            if (!isByteSwap)
            {
                wordToSend = *txData;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 8U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 16U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 24U;
                ++txData;
            }
            else
            {
                wordToSend = (unsigned)(*txData) << 24U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 16U;
                ++txData;
                wordToSend |= (unsigned)(*txData) << 8U;
                ++txData;
                wordToSend |= *txData;
                ++txData;
            }
            break;

        default:
            assert(false);
            break;
    }
    return wordToSend;
}

static void LPSPI_SeparateReadData(uint8_t *rxData, uint32_t readData, uint32_t bytesEachRead, bool isByteSwap)
{
    assert(rxData);

    switch (bytesEachRead)
    {
        case 1:
            *rxData = readData;
            ++rxData;
            break;

        case 2:
            if (!isByteSwap)
            {
                *rxData = readData;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
            }
            else
            {
                *rxData = readData >> 8;
                ++rxData;
                *rxData = readData;
                ++rxData;
            }
            break;

        case 3:
            if (!isByteSwap)
            {
                *rxData = readData;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
                *rxData = readData >> 16;
                ++rxData;
            }
            else
            {
                *rxData = readData >> 16;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
                *rxData = readData;
                ++rxData;
            }
            break;

        case 4:
            if (!isByteSwap)
            {
                *rxData = readData;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
                *rxData = readData >> 16;
                ++rxData;
                *rxData = readData >> 24;
                ++rxData;
            }
            else
            {
                *rxData = readData >> 24;
                ++rxData;
                *rxData = readData >> 16;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
                *rxData = readData;
                ++rxData;
            }
            break;

        default:
            assert(false);
            break;
    }
}

static void LPSPI_CommonIRQHandler(LPSPI_Type *base, void *param)
{
    if (LPSPI_IsMaster(base))
    {
        s_lpspiMasterIsr(base, (lpspi_master_handle_t *)param);
    }
    else
    {
        s_lpspiSlaveIsr(base, (lpspi_slave_handle_t *)param);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#if defined(LPSPI0)
void LPSPI0_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[0]);
    LPSPI_CommonIRQHandler(LPSPI0, s_lpspiHandle[0]);
}
#endif

#if defined(LPSPI1)
void LPSPI1_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[1]);
    LPSPI_CommonIRQHandler(LPSPI1, s_lpspiHandle[1]);
}
#endif

#if defined(LPSPI2)
void LPSPI2_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[2]);
    LPSPI_CommonIRQHandler(LPSPI2, s_lpspiHandle[2]);
}
#endif

#if defined(LPSPI3)
void LPSPI3_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[3]);
    LPSPI_CommonIRQHandler(LPSPI3, s_lpspiHandle[3]);
}
#endif

#if defined(LPSPI4)
void LPSPI4_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[4]);
    LPSPI_CommonIRQHandler(LPSPI4, s_lpspiHandle[4]);
}
#endif

#if defined(LPSPI5)
void LPSPI5_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[5]);
    LPSPI_CommonIRQHandler(LPSPI5, s_lpspiHandle[5]);
}
#endif

#if defined(DMA__LPSPI0)
void DMA_SPI0_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI0)]);
    LPSPI_CommonIRQHandler(DMA__LPSPI0, s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI0)]);
}
#endif

#if defined(DMA__LPSPI1)
void DMA_SPI1_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI1)]);
    LPSPI_CommonIRQHandler(DMA__LPSPI1, s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI1)]);
}
#endif
#if defined(DMA__LPSPI2)
void DMA_SPI2_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI2)]);
    LPSPI_CommonIRQHandler(DMA__LPSPI2, s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI2)]);
}
#endif

#if defined(DMA__LPSPI3)
void DMA_SPI3_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI3)]);
    LPSPI_CommonIRQHandler(DMA__LPSPI3, s_lpspiHandle[LPSPI_GetInstance(DMA__LPSPI3)]);
}
#endif

#if defined(ADMA__LPSPI0)
void ADMA_SPI0_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI0)]);
    LPSPI_CommonIRQHandler(ADMA__LPSPI0, s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI0)]);
}
#endif

#if defined(ADMA__LPSPI1)
void ADMA_SPI1_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI1)]);
    LPSPI_CommonIRQHandler(ADMA__LPSPI1, s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI1)]);
}
#endif
#if defined(ADMA__LPSPI2)
void ADMA_SPI2_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI2)]);
    LPSPI_CommonIRQHandler(ADMA__LPSPI2, s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI2)]);
}
#endif

#if defined(ADMA__LPSPI3)
void ADMA_SPI3_INT_DriverIRQHandler(void)
{
    assert(s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI3)]);
    LPSPI_CommonIRQHandler(ADMA__LPSPI3, s_lpspiHandle[LPSPI_GetInstance(ADMA__LPSPI3)]);
}
#endif
