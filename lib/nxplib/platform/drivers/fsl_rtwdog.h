/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_RTWDOG_H_
#define _FSL_RTWDOG_H_

#include "fsl_common.h"

/*!
 * @addtogroup rtwdog
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/
/*! @name Unlock sequence */
/*@{*/
#define WDOG_FIRST_WORD_OF_UNLOCK (RTWDOG_UPDATE_KEY & 0xFFFFU)           /*!< First word of unlock sequence */
#define WDOG_SECOND_WORD_OF_UNLOCK ((RTWDOG_UPDATE_KEY >> 16U) & 0xFFFFU) /*!< Second word of unlock sequence */
/*@}*/

/*! @name Refresh sequence */
/*@{*/
#define WDOG_FIRST_WORD_OF_REFRESH (RTWDOG_REFRESH_KEY & 0xFFFFU)           /*!< First word of refresh sequence */
#define WDOG_SECOND_WORD_OF_REFRESH ((RTWDOG_REFRESH_KEY >> 16U) & 0xFFFFU) /*!< Second word of refresh sequence */
/*@}*/
/*! @name Driver version */
/*@{*/
/*! @brief RTWDOG driver version 2.1.0. */
#define FSL_RTWDOG_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

/*! @brief Describes RTWDOG clock source. */
typedef enum _rtwdog_clock_source
{
    kRTWDOG_ClockSource0 = 0U, /*!< Clock source 0 */
    kRTWDOG_ClockSource1 = 1U, /*!< Clock source 1 */
    kRTWDOG_ClockSource2 = 2U, /*!< Clock source 2 */
    kRTWDOG_ClockSource3 = 3U, /*!< Clock source 3 */
} rtwdog_clock_source_t;

/*! @brief Describes the selection of the clock prescaler. */
typedef enum _rtwdog_clock_prescaler
{
    kRTWDOG_ClockPrescalerDivide1   = 0x0U, /*!< Divided by 1 */
    kRTWDOG_ClockPrescalerDivide256 = 0x1U, /*!< Divided by 256 */
} rtwdog_clock_prescaler_t;

/*! @brief Defines RTWDOG work mode. */
typedef struct _rtwdog_work_mode
{
    bool enableWait;  /*!< Enables or disables RTWDOG in wait mode */
    bool enableStop;  /*!< Enables or disables RTWDOG in stop mode */
    bool enableDebug; /*!< Enables or disables RTWDOG in debug mode */
} rtwdog_work_mode_t;

/*! @brief Describes RTWDOG test mode. */
typedef enum _rtwdog_test_mode
{
    kRTWDOG_TestModeDisabled = 0U, /*!< Test Mode disabled */
    kRTWDOG_UserModeEnabled  = 1U, /*!< User Mode enabled */
    kRTWDOG_LowByteTest      = 2U, /*!< Test Mode enabled, only low byte is used */
    kRTWDOG_HighByteTest     = 3U, /*!< Test Mode enabled, only high byte is used */
} rtwdog_test_mode_t;

/*! @brief Describes RTWDOG configuration structure. */
typedef struct _rtwdog_config
{
    bool enableRtwdog;                  /*!< Enables or disables RTWDOG */
    rtwdog_clock_source_t clockSource;  /*!< Clock source select */
    rtwdog_clock_prescaler_t prescaler; /*!< Clock prescaler value */
    rtwdog_work_mode_t workMode;        /*!< Configures RTWDOG work mode in debug stop and wait mode */
    rtwdog_test_mode_t testMode;        /*!< Configures RTWDOG test mode */
    bool enableUpdate;                  /*!< Update write-once register enable */
    bool enableInterrupt;               /*!< Enables or disables RTWDOG interrupt */
    bool enableWindowMode;              /*!< Enables or disables RTWDOG window mode */
    uint16_t windowValue;               /*!< Window value */
    uint16_t timeoutValue;              /*!< Timeout value */
} rtwdog_config_t;

/*!
 * @brief RTWDOG interrupt configuration structure.
 *
 * This structure contains the settings for all of the RTWDOG interrupt configurations.
 */
enum _rtwdog_interrupt_enable_t
{
    kRTWDOG_InterruptEnable = RTWDOG_CS_INT_MASK, /*!< Interrupt is generated before forcing a reset */
};

/*!
 * @brief RTWDOG status flags.
 *
 * This structure contains the RTWDOG status flags for use in the RTWDOG functions.
 */
enum _rtwdog_status_flags_t
{
    kRTWDOG_RunningFlag   = RTWDOG_CS_EN_MASK,  /*!< Running flag, set when RTWDOG is enabled */
    kRTWDOG_InterruptFlag = RTWDOG_CS_FLG_MASK, /*!< Interrupt flag, set when interrupt occurs */
};

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name RTWDOG Initialization and De-initialization
 * @{
 */

/*!
 * @brief Initializes the RTWDOG configuration structure.
 *
 * This function initializes the RTWDOG configuration structure to default values. The default
 * values are:
 * @code
 *   rtwdogConfig->enableRtwdog = true;
 *   rtwdogConfig->clockSource = kRTWDOG_ClockSource1;
 *   rtwdogConfig->prescaler = kRTWDOG_ClockPrescalerDivide1;
 *   rtwdogConfig->workMode.enableWait = true;
 *   rtwdogConfig->workMode.enableStop = false;
 *   rtwdogConfig->workMode.enableDebug = false;
 *   rtwdogConfig->testMode = kRTWDOG_TestModeDisabled;
 *   rtwdogConfig->enableUpdate = true;
 *   rtwdogConfig->enableInterrupt = false;
 *   rtwdogConfig->enableWindowMode = false;
 *   rtwdogConfig->windowValue = 0U;
 *   rtwdogConfig->timeoutValue = 0xFFFFU;
 * @endcode
 *
 * @param config Pointer to the RTWDOG configuration structure.
 * @see rtwdog_config_t
 */
void RTWDOG_GetDefaultConfig(rtwdog_config_t *config);

/*!
 * @brief Initializes the RTWDOG module.
 *
 * This function initializes the RTWDOG.
 * To reconfigure the RTWDOG without forcing a reset first, enableUpdate must be set to true
 * in the configuration.
 *
 * Example:
 * @code
 *   rtwdog_config_t config;
 *   RTWDOG_GetDefaultConfig(&config);
 *   config.timeoutValue = 0x7ffU;
 *   config.enableUpdate = true;
 *   RTWDOG_Init(wdog_base,&config);
 * @endcode
 *
 * @param base   RTWDOG peripheral base address.
 * @param config The configuration of the RTWDOG.
 */
AT_QUICKACCESS_SECTION_CODE(void RTWDOG_Init(RTWDOG_Type *base, const rtwdog_config_t *config));

/*!
 * @brief De-initializes the RTWDOG module.
 *
 * This function shuts down the RTWDOG.
 * Ensure that the WDOG_CS.UPDATE is 1, which means that the register update is enabled.
 *
 * @param base   RTWDOG peripheral base address.
 */
void RTWDOG_Deinit(RTWDOG_Type *base);

/* @} */

/*!
 * @name RTWDOG functional Operation
 * @{
 */

/*!
 * @brief Enables the RTWDOG module.
 *
 * This function writes a value into the WDOG_CS register to enable the RTWDOG.
 * The WDOG_CS register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address.
 */
static inline void RTWDOG_Enable(RTWDOG_Type *base)
{
    base->CS |= RTWDOG_CS_EN_MASK;
}

/*!
 * @brief Disables the RTWDOG module.
 *
 * This function writes a value into the WDOG_CS register to disable the RTWDOG.
 * The WDOG_CS register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address
 */
static inline void RTWDOG_Disable(RTWDOG_Type *base)
{
    base->CS &= ~RTWDOG_CS_EN_MASK;
}

/*!
 * @brief Enables the RTWDOG interrupt.
 *
 * This function writes a value into the WDOG_CS register to enable the RTWDOG interrupt.
 * The WDOG_CS register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address.
 * @param mask The interrupts to enable.
 *        The parameter can be a combination of the following source if defined:
 *        @arg kRTWDOG_InterruptEnable
 */
static inline void RTWDOG_EnableInterrupts(RTWDOG_Type *base, uint32_t mask)
{
    base->CS |= mask;
}

/*!
 * @brief Disables the RTWDOG interrupt.
 *
 * This function writes a value into the WDOG_CS register to disable the RTWDOG interrupt.
 * The WDOG_CS register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address.
 * @param mask The interrupts to disabled.
 *        The parameter can be a combination of the following source if defined:
 *        @arg kRTWDOG_InterruptEnable
 */
static inline void RTWDOG_DisableInterrupts(RTWDOG_Type *base, uint32_t mask)
{
    base->CS &= ~mask;
}

/*!
 * @brief Gets the RTWDOG all status flags.
 *
 * This function gets all status flags.
 *
 * Example to get the running flag:
 * @code
 *   uint32_t status;
 *   status = RTWDOG_GetStatusFlags(wdog_base) & kRTWDOG_RunningFlag;
 * @endcode
 * @param base        RTWDOG peripheral base address
 * @return            State of the status flag: asserted (true) or not-asserted (false). @see _rtwdog_status_flags_t
 *                    - true: related status flag has been set.
 *                    - false: related status flag is not set.
 */
static inline uint32_t RTWDOG_GetStatusFlags(RTWDOG_Type *base)
{
    return (base->CS & (RTWDOG_CS_EN_MASK | RTWDOG_CS_FLG_MASK));
}

/*!
 * @brief Enables/disables the window mode.
 *
 * @param base   RTWDOG peripheral base address.
 * @param enable Enables(true) or disables(false) the feature.
 */
static inline void RTWDOG_EnableWindowMode(RTWDOG_Type *base, bool enable)
{
    if (enable)
    {
        base->CS |= RTWDOG_CS_WIN_MASK;
    }
    else
    {
        base->CS &= ~RTWDOG_CS_WIN_MASK;
    }
}

/*!
 * @brief Converts raw count value to millisecond.
 *
 * Note that if the clock frequency is too high the timeout period can be less than 1 ms.
 * In this case this api will return 0 value.
 *
 * @param base          RTWDOG peripheral base address.
 * @param count         Raw count value.
 # @param clockFreqInHz The frequency of the clock source RTWDOG uses.
 */
static inline uint32_t RTWDOG_CountToMesec(RTWDOG_Type *base, uint32_t count, uint32_t clockFreqInHz)
{
    if ((base->CS & RTWDOG_CS_PRES_MASK) >> RTWDOG_CS_PRES_SHIFT)
    {
        clockFreqInHz /= 256;
    }
    return count * 1000U / clockFreqInHz;
}

/*!
 * @brief Clears the RTWDOG flag.
 *
 * This function clears the RTWDOG status flag.
 *
 * Example to clear an interrupt flag:
 * @code
 *   RTWDOG_ClearStatusFlags(wdog_base,kRTWDOG_InterruptFlag);
 * @endcode
 * @param base        RTWDOG peripheral base address.
 * @param mask        The status flags to clear.
 *                    The parameter can be any combination of the following values:
 *                    @arg kRTWDOG_InterruptFlag
 */
void RTWDOG_ClearStatusFlags(RTWDOG_Type *base, uint32_t mask);

/*!
 * @brief Sets the RTWDOG timeout value.
 *
 * This function writes a timeout value into the WDOG_TOVAL register.
 * The WDOG_TOVAL register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address
 * @param timeoutCount RTWDOG timeout value, count of RTWDOG clock ticks.
 */
static inline void RTWDOG_SetTimeoutValue(RTWDOG_Type *base, uint16_t timeoutCount)
{
    base->TOVAL = timeoutCount;
}

/*!
 * @brief Sets the RTWDOG window value.
 *
 * This function writes a window value into the WDOG_WIN register.
 * The WDOG_WIN register is a write-once register. Ensure that the WCT window is still open and
 * this register has not been written in this WCT while the function is called.
 *
 * @param base RTWDOG peripheral base address.
 * @param windowValue RTWDOG window value.
 */
static inline void RTWDOG_SetWindowValue(RTWDOG_Type *base, uint16_t windowValue)
{
    base->WIN = windowValue;
}

/*!
 * @brief Unlocks the RTWDOG register written.
 *
 * This function unlocks the RTWDOG register written.
 *
 * Before starting the unlock sequence and following the configuration, disable the global interrupts.
 * Otherwise, an interrupt could effectively invalidate the unlock sequence and the WCT may expire.
 * After the configuration finishes, re-enable the global interrupts.
 *
 * @param base RTWDOG peripheral base address
 */
static inline void RTWDOG_Unlock(RTWDOG_Type *base)
{
    if ((base->CS) & RTWDOG_CS_CMD32EN_MASK)
    {
        base->CNT = RTWDOG_UPDATE_KEY;
    }
    else
    {
        base->CNT = WDOG_FIRST_WORD_OF_UNLOCK;
        base->CNT = WDOG_SECOND_WORD_OF_UNLOCK;
    }
    while ((base->CS & RTWDOG_CS_ULK_MASK) == 0)
    {
    }
}

/*!
 * @brief Refreshes the RTWDOG timer.
 *
 * This function feeds the RTWDOG.
 * This function should be called before the Watchdog timer is in timeout. Otherwise, a reset is asserted.
 *
 * @param base RTWDOG peripheral base address
 */
static inline void RTWDOG_Refresh(RTWDOG_Type *base)
{
    uint32_t primaskValue = 0U;
    primaskValue          = DisableGlobalIRQ();
    if ((base->CS) & RTWDOG_CS_CMD32EN_MASK)
    {
        base->CNT = RTWDOG_REFRESH_KEY;
    }
    else
    {
        base->CNT = WDOG_FIRST_WORD_OF_REFRESH;
        base->CNT = WDOG_SECOND_WORD_OF_REFRESH;
    }
    EnableGlobalIRQ(primaskValue);
}

/*!
 * @brief Gets the RTWDOG counter value.
 *
 * This function gets the RTWDOG counter value.
 *
 * @param base RTWDOG peripheral base address.
 * @return     Current RTWDOG counter value.
 */
static inline uint16_t RTWDOG_GetCounterValue(RTWDOG_Type *base)
{
    return base->CNT;
}

/*@}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* _FSL_RTWDOG_H_ */
