/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_EWM_H_
#define _FSL_EWM_H_

#include "fsl_common.h"

/*!
 * @addtogroup ewm
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief EWM driver version 2.0.1. */
#define FSL_EWM_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*! @brief Describes EWM clock source. */
#if defined(FSL_FEATURE_EWM_HAS_CLOCK_SELECT) && FSL_FEATURE_EWM_HAS_CLOCK_SELECT
typedef enum _ewm_lpo_clock_source
{
    kEWM_LpoClockSource0 = 0U, /*!< EWM clock sourced from lpo_clk[0]*/
    kEWM_LpoClockSource1 = 1U, /*!< EWM clock sourced from lpo_clk[1]*/
    kEWM_LpoClockSource2 = 2U, /*!< EWM clock sourced from lpo_clk[2]*/
    kEWM_LpoClockSource3 = 3U, /*!< EWM clock sourced from lpo_clk[3]*/
} ewm_lpo_clock_source_t;
#endif /* FSL_FEATURE_EWM_HAS_CLOCK_SELECT */

/*!
 * @brief Data structure for EWM configuration.
 *
 * This structure is used to configure the EWM.
 */
typedef struct _ewm_config
{
    bool enableEwm;           /*!< Enable EWM module */
    bool enableEwmInput;      /*!< Enable EWM_in input */
    bool setInputAssertLogic; /*!< EWM_in signal assertion state */
    bool enableInterrupt;     /*!< Enable EWM interrupt */
#if defined(FSL_FEATURE_EWM_HAS_CLOCK_SELECT) && FSL_FEATURE_EWM_HAS_CLOCK_SELECT
    ewm_lpo_clock_source_t clockSource; /*!< Clock source select */
#endif                                  /* FSL_FEATURE_EWM_HAS_CLOCK_SELECT */
#if defined(FSL_FEATURE_EWM_HAS_PRESCALER) && FSL_FEATURE_EWM_HAS_PRESCALER
    uint8_t prescaler;        /*!< Clock prescaler value */
#endif                        /* FSL_FEATURE_EWM_HAS_PRESCALER */
    uint8_t compareLowValue;  /*!< Compare low-register value */
    uint8_t compareHighValue; /*!< Compare high-register value */
} ewm_config_t;

/*!
 * @brief EWM interrupt configuration structure with default settings all disabled.
 *
 * This structure contains the settings for all of EWM interrupt configurations.
 */
enum _ewm_interrupt_enable_t
{
    kEWM_InterruptEnable = EWM_CTRL_INTEN_MASK, /*!< Enable the EWM to generate an interrupt*/
};

/*!
 * @brief EWM status flags.
 *
 * This structure contains the constants for the EWM status flags for use in the EWM functions.
 */
enum _ewm_status_flags_t
{
    kEWM_RunningFlag = EWM_CTRL_EWMEN_MASK, /*!< Running flag, set when EWM is enabled*/
};

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name EWM initialization and de-initialization
 * @{
 */

/*!
 * @brief Initializes the EWM peripheral.
 *
 * This function is used to initialize the EWM. After calling, the EWM
 * runs immediately according to the configuration.
 * Note that, except for the interrupt enable control bit, other control bits and registers are write once after a
 * CPU reset. Modifying them more than once generates a bus transfer error.
 *
 * This is an example.
 * @code
 *   ewm_config_t config;
 *   EWM_GetDefaultConfig(&config);
 *   config.compareHighValue = 0xAAU;
 *   EWM_Init(ewm_base,&config);
 * @endcode
 *
 * @param base EWM peripheral base address
 * @param config The configuration of the EWM
 */
void EWM_Init(EWM_Type *base, const ewm_config_t *config);

/*!
 * @brief Deinitializes the EWM peripheral.
 *
 * This function is used to shut down the EWM.
 *
 * @param base EWM peripheral base address
 */
void EWM_Deinit(EWM_Type *base);

/*!
 * @brief Initializes the EWM configuration structure.
 *
 * This function initializes the EWM configuration structure to default values. The default
 * values are as follows.
 * @code
 *   ewmConfig->enableEwm = true;
 *   ewmConfig->enableEwmInput = false;
 *   ewmConfig->setInputAssertLogic = false;
 *   ewmConfig->enableInterrupt = false;
 *   ewmConfig->ewm_lpo_clock_source_t = kEWM_LpoClockSource0;
 *   ewmConfig->prescaler = 0;
 *   ewmConfig->compareLowValue = 0;
 *   ewmConfig->compareHighValue = 0xFEU;
 * @endcode
 *
 * @param config Pointer to the EWM configuration structure.
 * @see ewm_config_t
 */
void EWM_GetDefaultConfig(ewm_config_t *config);

/* @} */

/*!
 * @name EWM functional Operation
 * @{
 */

/*!
 * @brief Enables the EWM interrupt.
 *
 * This function enables the EWM interrupt.
 *
 * @param base EWM peripheral base address
 * @param mask The interrupts to enable
 *        The parameter can be combination of the following source if defined
 *        @arg kEWM_InterruptEnable
 */
static inline void EWM_EnableInterrupts(EWM_Type *base, uint32_t mask)
{
    base->CTRL |= mask;
}

/*!
 * @brief Disables the EWM interrupt.
 *
 * This function enables the EWM interrupt.
 *
 * @param base EWM peripheral base address
 * @param mask The interrupts to disable
 *        The parameter can be combination of the following source if defined
 *        @arg kEWM_InterruptEnable
 */
static inline void EWM_DisableInterrupts(EWM_Type *base, uint32_t mask)
{
    base->CTRL &= ~mask;
}

/*!
 * @brief Gets all status flags.
 *
 * This function gets all status flags.
 *
 * This is an example for getting the running flag.
 * @code
 *   uint32_t status;
 *   status = EWM_GetStatusFlags(ewm_base) & kEWM_RunningFlag;
 * @endcode
 * @param base        EWM peripheral base address
 * @return            State of the status flag: asserted (true) or not-asserted (false).@see _ewm_status_flags_t
 *                    - True: a related status flag has been set.
 *                    - False: a related status flag is not set.
 */
static inline uint32_t EWM_GetStatusFlags(EWM_Type *base)
{
    return (base->CTRL & EWM_CTRL_EWMEN_MASK);
}

/*!
 * @brief Services the EWM.
 *
 * This function resets the EWM counter to zero.
 *
 * @param base EWM peripheral base address
 */
void EWM_Refresh(EWM_Type *base);

/*@}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* _FSL_EWM_H_ */
