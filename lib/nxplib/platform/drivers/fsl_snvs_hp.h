/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2017-2018, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SNVS_HP_H_
#define _FSL_SNVS_HP_H_

#include "fsl_common.h"

/*!
 * @addtogroup snvs_hp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SNVS_HP_DRIVER_VERSION (MAKE_VERSION(2, 1, 2)) /*!< Version 2.1.2 */
/*@}*/

/*! @brief List of SNVS interrupts */
typedef enum _snvs_hp_interrupts
{
    kSNVS_RTC_AlarmInterrupt    = SNVS_HPCR_HPTA_EN_MASK, /*!< RTC time alarm */
    kSNVS_RTC_PeriodicInterrupt = SNVS_HPCR_PI_EN_MASK,   /*!< RTC periodic interrupt */
} snvs_hp_interrupts_t;

/*! @brief List of SNVS flags */
typedef enum _snvs_hp_status_flags
{
    kSNVS_RTC_AlarmInterruptFlag    = SNVS_HPSR_HPTA_MASK,          /*!< RTC time alarm flag */
    kSNVS_RTC_PeriodicInterruptFlag = SNVS_HPSR_PI_MASK,            /*!< RTC periodic interrupt flag */
    kSNVS_ZMK_ZeroFlag              = (int)SNVS_HPSR_ZMK_ZERO_MASK, /*!< The ZMK is zero */
    kSNVS_OTPMK_ZeroFlag            = SNVS_HPSR_OTPMK_ZERO_MASK,    /*!< The OTPMK is zero */
} snvs_hp_status_flags_t;

/*! @brief List of SNVS security violation flags */
typedef enum _snvs_hp_sv_status_flags
{
    kSNVS_LP_ViolationFlag = SNVS_HPSVSR_SW_LPSV_MASK,           /*!< Low Power section Security Violation */
    kSNVS_ZMK_EccFailFlag  = SNVS_HPSVSR_ZMK_ECC_FAIL_MASK,      /*!< Zeroizable Master Key Error Correcting Code Check
                                                                   Failure */
    kSNVS_LP_SoftwareViolationFlag   = SNVS_HPSVSR_SW_LPSV_MASK, /*!< LP Software Security Violation */
    kSNVS_FatalSoftwareViolationFlag = SNVS_HPSVSR_SW_FSV_MASK,  /*!< Software Fatal Security Violation */
    kSNVS_SoftwareViolationFlag      = SNVS_HPSVSR_SW_SV_MASK,   /*!< Software Security Violation */
    kSNVS_Violation0Flag             = SNVS_HPSVSR_SV0_MASK,     /*!< Security Violation 0 */
    kSNVS_Violation1Flag             = SNVS_HPSVSR_SV1_MASK,     /*!< Security Violation 1 */
    kSNVS_Violation2Flag             = SNVS_HPSVSR_SV2_MASK,     /*!< Security Violation 2 */
    kSNVS_Violation3Flag             = SNVS_HPSVSR_SV3_MASK,     /*!< Security Violation 3 */
    kSNVS_Violation4Flag             = SNVS_HPSVSR_SV4_MASK,     /*!< Security Violation 4 */
    kSNVS_Violation5Flag             = SNVS_HPSVSR_SV5_MASK,     /*!< Security Violation 5 */
} snvs_hp_sv_status_flags_t;

/*!
 * @brief Macro to make security violation flag
 *
 * Macro help to make security violation flag kSNVS_Violation0Flag to kSNVS_Violation5Flag,
 * For example, SNVS_MAKE_HP_SV_FLAG(0) is kSNVS_Violation0Flag.
 */
#define SNVS_MAKE_HP_SV_FLAG(x) (1U << (SNVS_HPSVSR_SV0_SHIFT + (x)))

/*! @brief Structure is used to hold the date and time */
typedef struct _snvs_hp_rtc_datetime
{
    uint16_t year;  /*!< Range from 1970 to 2099.*/
    uint8_t month;  /*!< Range from 1 to 12.*/
    uint8_t day;    /*!< Range from 1 to 31 (depending on month).*/
    uint8_t hour;   /*!< Range from 0 to 23.*/
    uint8_t minute; /*!< Range from 0 to 59.*/
    uint8_t second; /*!< Range from 0 to 59.*/
} snvs_hp_rtc_datetime_t;

/*!
 * @brief SNVS config structure
 *
 * This structure holds the configuration settings for the SNVS peripheral. To initialize this
 * structure to reasonable defaults, call the SNVS_GetDefaultConfig() function and pass a
 * pointer to your config structure instance.
 *
 * The config struct can be made const so it resides in flash
 */
typedef struct _snvs_hp_rtc_config
{
    bool rtcCalEnable;              /*!< true: RTC calibration mechanism is enabled;
                                         false:No calibration is used */
    uint32_t rtcCalValue;           /*!< Defines signed calibration value for nonsecure RTC;
                                         This is a 5-bit 2's complement value, range from -16 to +15 */
    uint32_t periodicInterruptFreq; /*!< Defines frequency of the periodic interrupt;
                                         Range from 0 to 15 */
} snvs_hp_rtc_config_t;

typedef enum _snvs_hp_ssm_state
{
    kSNVS_SSMInit      = 0x00, /*!< Init */
    kSNVS_SSMHardFail  = 0x01, /*!< Hard Fail */
    kSNVS_SSMSoftFail  = 0x03, /*!< Soft Fail */
    kSNVS_SSMInitInter = 0x08, /*!< Init Intermediate (transition state between Init and Check) */
    kSNVS_SSMCheck     = 0x09, /*!< Check */
    kSNVS_SSMNonSecure = 0x0B, /*!< Non-Secure */
    kSNVS_SSMTrusted   = 0x0D, /*!< Trusted */
    kSNVS_SSMSecure    = 0x0F, /*!< Secure */
} snvs_hp_ssm_state_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initialize the SNVS.
 *
 * @note This API should be called at the beginning of the application using the SNVS driver.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_HP_Init(SNVS_Type *base);

/*!
 * @brief Deinitialize the SNVS.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_HP_Deinit(SNVS_Type *base);

/*!
 * @brief Ungates the SNVS clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application using the SNVS driver.
 *
 * @param base   SNVS peripheral base address
 * @param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_HP_RTC_Init(SNVS_Type *base, const snvs_hp_rtc_config_t *config);

/*!
 * @brief Stops the RTC and SRTC timers.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_HP_RTC_Deinit(SNVS_Type *base);

/*!
 * @brief Fills in the SNVS config struct with the default settings.
 *
 * The default values are as follows.
 * @code
 *    config->rtccalenable = false;
 *    config->rtccalvalue = 0U;
 *    config->PIFreq = 0U;
 * @endcode
 * @param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_HP_RTC_GetDefaultConfig(snvs_hp_rtc_config_t *config);

/*! @}*/

/*!
 * @name Non secure RTC current Time & Alarm
 * @{
 */

/*!
 * @brief Sets the SNVS RTC date and time according to the given time structure.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the date and time details are stored.
 *
 * @return kStatus_Success: Success in setting the time and starting the SNVS RTC
 *         kStatus_InvalidArgument: Error because the datetime format is incorrect
 */
status_t SNVS_HP_RTC_SetDatetime(SNVS_Type *base, const snvs_hp_rtc_datetime_t *datetime);

/*!
 * @brief Gets the SNVS RTC time and stores it in the given time structure.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the date and time details are stored.
 */
void SNVS_HP_RTC_GetDatetime(SNVS_Type *base, snvs_hp_rtc_datetime_t *datetime);

/*!
 * @brief Sets the SNVS RTC alarm time.
 *
 * The function sets the RTC alarm. It also checks whether the specified alarm time
 * is greater than the present time. If not, the function does not set the alarm
 * and returns an error.
 *
 * @param base      SNVS peripheral base address
 * @param alarmTime Pointer to the structure where the alarm time is stored.
 *
 * @return kStatus_Success: success in setting the SNVS RTC alarm
 *         kStatus_InvalidArgument: Error because the alarm datetime format is incorrect
 *         kStatus_Fail: Error because the alarm time has already passed
 */
status_t SNVS_HP_RTC_SetAlarm(SNVS_Type *base, const snvs_hp_rtc_datetime_t *alarmTime);

/*!
 * @brief Returns the SNVS RTC alarm time.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the alarm date and time details are stored.
 */
void SNVS_HP_RTC_GetAlarm(SNVS_Type *base, snvs_hp_rtc_datetime_t *datetime);

#if (defined(FSL_FEATURE_SNVS_HAS_SRTC) && (FSL_FEATURE_SNVS_HAS_SRTC > 0))
/*!
 * @brief The function synchronizes RTC counter value with SRTC.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_HP_RTC_TimeSynchronize(SNVS_Type *base);
#endif /* FSL_FEATURE_SNVS_HAS_SRTC */

/*! @}*/

/*!
 * @name Interrupt Interface
 * @{
 */

/*!
 * @brief Enables the selected SNVS interrupts.
 *
 * @param base SNVS peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::snvs_interrupt_enable_t
 */
static inline void SNVS_HP_RTC_EnableInterrupts(SNVS_Type *base, uint32_t mask)
{
    base->HPCR |= mask;
}

/*!
 * @brief Disables the selected SNVS interrupts.
 *
 * @param base SNVS peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::snvs_interrupt_enable_t
 */
static inline void SNVS_HP_RTC_DisableInterrupts(SNVS_Type *base, uint32_t mask)
{
    base->HPCR &= ~mask;
}

/*!
 * @brief Gets the enabled SNVS interrupts.
 *
 * @param base SNVS peripheral base address
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::snvs_interrupt_enable_t
 */
uint32_t SNVS_HP_RTC_GetEnabledInterrupts(SNVS_Type *base);

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the SNVS status flags.
 *
 * @param base SNVS peripheral base address
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::snvs_status_flags_t
 */
uint32_t SNVS_HP_RTC_GetStatusFlags(SNVS_Type *base);

/*!
 * @brief  Clears the SNVS status flags.
 *
 * @param base SNVS peripheral base address
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::snvs_status_flags_t
 */
static inline void SNVS_HP_RTC_ClearStatusFlags(SNVS_Type *base, uint32_t mask)
{
    base->HPSR |= mask;
}

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the SNVS RTC time counter.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_RTC_StartTimer(SNVS_Type *base)
{
    base->HPCR |= SNVS_HPCR_RTC_EN_MASK;
    while (!(base->HPCR & SNVS_HPCR_RTC_EN_MASK))
    {
    }
}

/*!
 * @brief Stops the SNVS RTC time counter.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_RTC_StopTimer(SNVS_Type *base)
{
    base->HPCR &= ~SNVS_HPCR_RTC_EN_MASK;
    while (base->HPCR & SNVS_HPCR_RTC_EN_MASK)
    {
    }
}

/*! @}*/

/*!
 * @brief Enable or disable master key selection.
 *
 * @param base SNVS peripheral base address
 * @param enable Pass true to enable, false to disable.
 */
static inline void SNVS_HP_EnableMasterKeySelection(SNVS_Type *base, bool enable)
{
    if (enable)
    {
        base->HPCOMR |= SNVS_HPCOMR_MKS_EN_MASK;
    }
    else
    {
        base->HPCOMR &= (~SNVS_HPCOMR_MKS_EN_MASK);
    }
}

/*!
 * @brief Trigger to program Zeroizable Master Key.
 *
 * @param base SNVS peripheral base address
 * @param enable Pass true to enable, false to disable.
 */
static inline void SNVS_HP_ProgramZeroizableMasterKey(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_PROG_ZMK_MASK;
}

/*!
 * @brief Trigger SSM State Transition
 *
 * Trigger state transition of the system security monitor (SSM). It results only
 * the following transitions of the SSM:
 *   - Check State -> Non-Secure (when Non-Secure Boot and not in Fab Configuration)
 *   - Check State --> Trusted (when Secure Boot or in Fab Configuration )
 *   - Trusted State --> Secure
 *   - Secure State --> Trusted
 *   - Soft Fail --> Non-Secure
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_ChangeSSMState(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_SSM_ST_MASK;
}

/*!
 * @brief Trigger Software Fatal Security Violation
 *
 * The result SSM state transition is:
 *  - Check State -> Soft Fail
 *  - Non-Secure State -> Soft Fail
 *  - Trusted State -> Soft Fail
 *  - Secure State -> Soft Fail
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_SetSoftwareFatalSecurityViolation(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_SW_FSV_MASK;
}

/*!
 * @brief Trigger Software Security Violation
 *
 * The result SSM state transition is:
 *  - Check -> Non-Secure
 *  - Trusted -> Soft Fail
 *  - Secure -> Soft Fail
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_SetSoftwareSecurityViolation(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_SW_SV_MASK;
}

/*!
 * @brief Get current SSM State
 *
 * @param base SNVS peripheral base address
 * @return Current SSM state
 */
static inline snvs_hp_ssm_state_t SNVS_HP_GetSSMState(SNVS_Type *base)
{
    return (snvs_hp_ssm_state_t)((base->HPSR & SNVS_HPSR_SSM_STATE_MASK) >> SNVS_HPSR_SSM_STATE_SHIFT);
}

/*!
 * @brief Reset the SNVS LP section.
 *
 * Reset the LP section except SRTC and Time alarm.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_ResetLP(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_LP_SWR_MASK;
}

/*!
 * @name High Assurance Counter (HAC)
 * @{
 */

/*!
 * @brief Enable or disable the High Assurance Counter (HAC)
 *
 * @param base SNVS peripheral base address
 * @param enable Pass true to enable, false to disable.
 */
static inline void SNVS_HP_EnableHighAssuranceCounter(SNVS_Type *base, bool enable)
{
    if (enable)
    {
        base->HPCOMR |= SNVS_HPCOMR_HAC_EN_MASK;
    }
    else
    {
        base->HPCOMR &= (~SNVS_HPCOMR_HAC_EN_MASK);
    }
}

/*!
 * @brief Start or stop the High Assurance Counter (HAC)
 *
 * @param base SNVS peripheral base address
 * @param start Pass true to start, false to stop.
 */
static inline void SNVS_HP_StartHighAssuranceCounter(SNVS_Type *base, bool start)
{
    if (start)
    {
        base->HPCOMR &= (~SNVS_HPCOMR_HAC_STOP_MASK);
    }
    else
    {
        base->HPCOMR |= SNVS_HPCOMR_HAC_STOP_MASK;
    }
}

/*!
 * @brief Set the High Assurance Counter (HAC) initialize value.
 *
 * @param base SNVS peripheral base address
 * @param value The initial value to set.
 */
static inline void SNVS_HP_SetHighAssuranceCounterInitialValue(SNVS_Type *base, uint32_t value)
{
    base->HPHACIVR = value;
}

/*!
 * @brief Load the High Assurance Counter (HAC)
 *
 * This function loads the HAC initialize value to counter register.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_LoadHighAssuranceCounter(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_HAC_LOAD_MASK;
}

/*!
 * @brief Get the current High Assurance Counter (HAC) value
 *
 * @param base SNVS peripheral base address
 * @return HAC currnet value.
 */
static inline uint32_t SNVS_HP_GetHighAssuranceCounter(SNVS_Type *base)
{
    return base->HPHACR;
}

/*!
 * @brief Clear the High Assurance Counter (HAC)
 *
 * This function can be called in a functional or soft fail state. When the HAC
 * is enabled:
 *   - If the HAC is cleared in the soft fail state, the SSM transitions to the
 * hard fail state immediately;
 *   - If the HAC is cleared in functional state, the SSM will transition to
 * hard fail immediately after transitioning to soft fail.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_ClearHighAssuranceCounter(SNVS_Type *base)
{
    base->HPCOMR |= SNVS_HPCOMR_HAC_CLEAR_MASK;
}

/*!
 * @brief Lock the High Assurance Counter (HAC)
 *
 * Once locked, the HAC initialize value could not be changed, the HAC enable
 * status could not be changed. This could only be unlocked by system reset.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_HP_LockHighAssuranceCounter(SNVS_Type *base)
{
    base->HPLR |= SNVS_HPLR_HAC_L_MASK;
}

/*! @}*/

/*!
 * @brief Get the SNVS HP status flags.
 *
 * The flags are returned as the OR'ed value of @ref snvs_hp_sgtatus_flags_t.
 *
 * @param base SNVS peripheral base address
 * @return The OR'ed value of status flags.
 */
static inline uint32_t SNVS_HP_GetStatusFlags(SNVS_Type *base)
{
    return base->HPSR;
}

/*!
 * @brief Clear the SNVS HP status flags.
 *
 * The flags to clear are passed in as the OR'ed value of @ref snvs_hp_status_flags_t.
 * Only these flags could be cleared using this API.
 *  - @ref kSNVS_RTC_PeriodicInterruptFlag
 *  - @ref kSNVS_RTC_AlarmInterruptFlag
 *
 * @param base SNVS peripheral base address
 * @param mask OR'ed value of the flags to clear.
 */
static inline void SNVS_HP_ClearStatusFlags(SNVS_Type *base, uint32_t mask)
{
    base->HPSR = mask;
}

/*!
 * @brief Get the SNVS HP security violation status flags.
 *
 * The flags are returned as the OR'ed value of @ref snvs_hp_sv_status_flags_t.
 *
 * @param base SNVS peripheral base address
 * @return The OR'ed value of security violation status flags.
 */
static inline uint32_t SNVS_HP_GetSecurityViolationStatusFlags(SNVS_Type *base)
{
    return base->HPSVSR;
}

/*!
 * @brief Clear the SNVS HP security violation status flags.
 *
 * The flags to clear are passed in as the OR'ed value of @ref snvs_hp_sv_status_flags_t.
 * Only these flags could be cleared using this API.
 *
 *  - @ref kSNVS_ZMK_EccFailFlag
 *  - @ref kSNVS_Violation0Flag
 *  - @ref kSNVS_Violation1Flag
 *  - @ref kSNVS_Violation2Flag
 *  - @ref kSNVS_Violation3Flag
 *  - @ref kSNVS_Violation4Flag
 *  - @ref kSNVS_Violation5Flag
 *
 * @param base SNVS peripheral base address
 * @param mask OR'ed value of the flags to clear.
 */
static inline void SNVS_HP_ClearSecurityViolationStatusFlags(SNVS_Type *base, uint32_t mask)
{
    base->HPSVSR = mask;
}

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_SNVS_HP_H_ */
