/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CMP_H_
#define _FSL_CMP_H_

#include "fsl_common.h"

/*!
 * @addtogroup cmp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief CMP driver version 2.0.1. */
#define FSL_CMP_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*!
 * @brief Interrupt enable/disable mask.
 */
enum _cmp_interrupt_enable
{
    kCMP_OutputRisingInterruptEnable  = CMP_SCR_IER_MASK, /*!< Comparator interrupt enable rising. */
    kCMP_OutputFallingInterruptEnable = CMP_SCR_IEF_MASK, /*!< Comparator interrupt enable falling. */
};

/*!
 * @brief Status flags' mask.
 */
enum _cmp_status_flags
{
    kCMP_OutputRisingEventFlag  = CMP_SCR_CFR_MASK,  /*!< Rising-edge on the comparison output has occurred. */
    kCMP_OutputFallingEventFlag = CMP_SCR_CFF_MASK,  /*!< Falling-edge on the comparison output has occurred. */
    kCMP_OutputAssertEventFlag  = CMP_SCR_COUT_MASK, /*!< Return the current value of the analog comparator output. */
};

/*!
 * @brief CMP Hysteresis mode.
 */
typedef enum _cmp_hysteresis_mode
{
    kCMP_HysteresisLevel0 = 0U, /*!< Hysteresis level 0. */
    kCMP_HysteresisLevel1 = 1U, /*!< Hysteresis level 1. */
    kCMP_HysteresisLevel2 = 2U, /*!< Hysteresis level 2. */
    kCMP_HysteresisLevel3 = 3U, /*!< Hysteresis level 3. */
} cmp_hysteresis_mode_t;

/*!
 * @brief CMP Voltage Reference source.
 */
typedef enum _cmp_reference_voltage_source
{
    kCMP_VrefSourceVin1 = 0U, /*!< Vin1 is selected as a resistor ladder network supply reference Vin. */
    kCMP_VrefSourceVin2 = 1U, /*!< Vin2 is selected as a resistor ladder network supply reference Vin. */
} cmp_reference_voltage_source_t;

/*!
 * @brief Configures the comparator.
 */
typedef struct _cmp_config
{
    bool enableCmp;                       /*!< Enable the CMP module. */
    cmp_hysteresis_mode_t hysteresisMode; /*!< CMP Hysteresis mode. */
    bool enableHighSpeed;                 /*!< Enable High-speed (HS) comparison mode. */
    bool enableInvertOutput;              /*!< Enable the inverted comparator output. */
    bool useUnfilteredOutput;             /*!< Set the compare output(COUT) to equal COUTA(true) or COUT(false). */
    bool enablePinOut;                    /*!< The comparator output is available on the associated pin. */
#if defined(FSL_FEATURE_CMP_HAS_TRIGGER_MODE) && FSL_FEATURE_CMP_HAS_TRIGGER_MODE
    bool enableTriggerMode; /*!< Enable the trigger mode. */
#endif                      /* FSL_FEATURE_CMP_HAS_TRIGGER_MODE */
} cmp_config_t;

/*!
 * @brief Configures the filter.
 */
typedef struct _cmp_filter_config
{
#if defined(FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT) && FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT
    bool enableSample;    /*!< Using the external SAMPLE as a sampling clock input or using a divided bus clock. */
#endif                    /* FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT */
    uint8_t filterCount;  /*!< Filter Sample Count. Available range is 1-7; 0 disables the filter.*/
    uint8_t filterPeriod; /*!< Filter Sample Period. The divider to the bus clock. Available range is 0-255. */
} cmp_filter_config_t;

/*!
 * @brief Configures the internal DAC.
 */
typedef struct _cmp_dac_config
{
    cmp_reference_voltage_source_t referenceVoltageSource; /*!< Supply voltage reference source. */
    uint8_t DACValue; /*!< Value for the DAC Output Voltage. Available range is 0-63.*/
} cmp_dac_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name Initialization
 * @{
 */

/*!
 * @brief Initializes the CMP.
 *
 * This function initializes the CMP module. The operations included are as follows.
 * - Enabling the clock for CMP module.
 * - Configuring the comparator.
 * - Enabling the CMP module.
 * Note that for some devices, multiple CMP instances share the same clock gate. In this case, to enable the clock for
 * any instance enables all CMPs. See the appropriate MCU reference manual for the clock assignment of the CMP.
 *
 * @param base   CMP peripheral base address.
 * @param config Pointer to the configuration structure.
 */
void CMP_Init(CMP_Type *base, const cmp_config_t *config);

/*!
 * @brief De-initializes the CMP module.
 *
 * This function de-initializes the CMP module. The operations included are as follows.
 * - Disabling the CMP module.
 * - Disabling the clock for CMP module.
 *
 * This function disables the clock for the CMP.
 * Note that for some devices, multiple CMP instances share the same clock gate. In this case, before disabling the
 * clock for the CMP, ensure that all the CMP instances are not used.
 *
 * @param base CMP peripheral base address.
 */
void CMP_Deinit(CMP_Type *base);

/*!
 * @brief Enables/disables the CMP module.
 *
 * @param base CMP peripheral base address.
 * @param enable Enables or disables the module.
 */
static inline void CMP_Enable(CMP_Type *base, bool enable)
{
    if (enable)
    {
        base->CR1 |= CMP_CR1_EN_MASK;
    }
    else
    {
        base->CR1 &= ~(uint8_t)CMP_CR1_EN_MASK;
    }
}

/*!
 * @brief Initializes the CMP user configuration structure.
 *
 * This function initializes the user configuration structure to these default values.
 * @code
 *   config->enableCmp           = true;
 *   config->hysteresisMode      = kCMP_HysteresisLevel0;
 *   config->enableHighSpeed     = false;
 *   config->enableInvertOutput  = false;
 *   config->useUnfilteredOutput = false;
 *   config->enablePinOut        = false;
 *   config->enableTriggerMode   = false;
 * @endcode
 * @param config Pointer to the configuration structure.
 */
void CMP_GetDefaultConfig(cmp_config_t *config);

/*!
 * @brief  Sets the input channels for the comparator.
 *
 * This function sets the input channels for the comparator.
 * Note that two input channels cannot be set the same way in the application. When the user selects the same input
 * from the analog mux to the positive and negative port, the comparator is disabled automatically.
 *
 * @param  base            CMP peripheral base address.
 * @param  positiveChannel Positive side input channel number. Available range is 0-7.
 * @param  negativeChannel Negative side input channel number. Available range is 0-7.
 */
void CMP_SetInputChannels(CMP_Type *base, uint8_t positiveChannel, uint8_t negativeChannel);

/* @} */

/*!
 * @name Advanced Features
 * @{
 */

#if defined(FSL_FEATURE_CMP_HAS_DMA) && FSL_FEATURE_CMP_HAS_DMA
/*!
 * @brief Enables/disables the DMA request for rising/falling events.
 *
 * This function enables/disables the DMA request for rising/falling events. Either event triggers the generation of
 * the DMA request from CMP if the DMA feature is enabled. Both events are ignored for generating the DMA request from
 * the CMP
 * if the DMA is disabled.
 *
 * @param base CMP peripheral base address.
 * @param enable Enables or disables the feature.
 */
void CMP_EnableDMA(CMP_Type *base, bool enable);
#endif /* FSL_FEATURE_CMP_HAS_DMA */

#if defined(FSL_FEATURE_CMP_HAS_WINDOW_MODE) && FSL_FEATURE_CMP_HAS_WINDOW_MODE
/*!
 * @brief Enables/disables the window mode.
 *
 * @param base CMP peripheral base address.
 * @param enable Enables or disables the feature.
 */
static inline void CMP_EnableWindowMode(CMP_Type *base, bool enable)
{
    if (enable)
    {
        base->CR1 |= CMP_CR1_WE_MASK;
    }
    else
    {
        base->CR1 &= ~CMP_CR1_WE_MASK;
    }
}
#endif /* FSL_FEATURE_CMP_HAS_WINDOW_MODE */

#if defined(FSL_FEATURE_CMP_HAS_PASS_THROUGH_MODE) && FSL_FEATURE_CMP_HAS_PASS_THROUGH_MODE
/*!
 * @brief Enables/disables the pass through mode.
 *
 * @param base CMP peripheral base address.
 * @param enable Enables or disables the feature.
 */
static inline void CMP_EnablePassThroughMode(CMP_Type *base, bool enable)
{
    if (enable)
    {
        base->MUXCR |= CMP_MUXCR_PSTM_MASK;
    }
    else
    {
        base->MUXCR &= ~CMP_MUXCR_PSTM_MASK;
    }
}
#endif /* FSL_FEATURE_CMP_HAS_PASS_THROUGH_MODE */

/*!
 * @brief  Configures the filter.
 *
 * @param  base   CMP peripheral base address.
 * @param  config Pointer to the configuration structure.
 */
void CMP_SetFilterConfig(CMP_Type *base, const cmp_filter_config_t *config);

/*!
 * @brief Configures the internal DAC.
 *
 * @param base   CMP peripheral base address.
 * @param config Pointer to the configuration structure. "NULL" disables the feature.
 */
void CMP_SetDACConfig(CMP_Type *base, const cmp_dac_config_t *config);

/*!
 * @brief Enables the interrupts.
 *
 * @param base    CMP peripheral base address.
 * @param mask    Mask value for interrupts. See "_cmp_interrupt_enable".
 */
void CMP_EnableInterrupts(CMP_Type *base, uint32_t mask);

/*!
 * @brief Disables the interrupts.
 *
 * @param base    CMP peripheral base address.
 * @param mask    Mask value for interrupts. See "_cmp_interrupt_enable".
 */
void CMP_DisableInterrupts(CMP_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Results
 * @{
 */

/*!
 * @brief  Gets the status flags.
 *
 * @param  base     CMP peripheral base address.
 *
 * @return          Mask value for the asserted flags. See "_cmp_status_flags".
 */
uint32_t CMP_GetStatusFlags(CMP_Type *base);

/*!
 * @brief Clears the status flags.
 *
 * @param base     CMP peripheral base address.
 * @param mask     Mask value for the flags. See "_cmp_status_flags".
 */
void CMP_ClearStatusFlags(CMP_Type *base, uint32_t mask);

/* @} */
#if defined(__cplusplus)
}
#endif
/*!
 * @}
 */
#endif /* _FSL_CMP_H_ */
