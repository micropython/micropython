/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SNVS_LP_H_
#define _FSL_SNVS_LP_H_

#include "fsl_common.h"

/*!
 * @addtogroup snvs_lp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SNVS_LP_DRIVER_VERSION (MAKE_VERSION(2, 1, 0)) /*!< Version 2.1.0 */
/*@}*/

#define SNVS_ZMK_REG_COUNT 8 /* 8 Zeroizable Master Key registers. */

/*! @brief List of SNVS_LP interrupts */
typedef enum _snvs_lp_srtc_interrupts
{
    kSNVS_SRTC_AlarmInterrupt = SNVS_LPCR_LPTA_EN_MASK, /*!< SRTC time alarm.*/
} snvs_lp_srtc_interrupts_t;

/*! @brief List of SNVS_LP flags */
typedef enum _snvs_lp_srtc_status_flags
{
    kSNVS_SRTC_AlarmInterruptFlag = SNVS_LPSR_LPTA_MASK, /*!< SRTC time alarm flag */
} snvs_lp_srtc_status_flags_t;

/*! @brief List of SNVS_LP external tampers */
typedef enum _snvs_lp_external_tamper
{
    kSNVS_ExternalTamper1 = 1U,
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
    kSNVS_ExternalTamper2  = 2U,
    kSNVS_ExternalTamper3  = 3U,
    kSNVS_ExternalTamper4  = 4U,
    kSNVS_ExternalTamper5  = 5U,
    kSNVS_ExternalTamper6  = 6U,
    kSNVS_ExternalTamper7  = 7U,
    kSNVS_ExternalTamper8  = 8U,
    kSNVS_ExternalTamper9  = 9U,
    kSNVS_ExternalTamper10 = 10U
#endif
} snvs_lp_external_tamper_t;

/* define max possible tamper present */
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
#define SNVS_LP_MAX_TAMPER kSNVS_ExternalTamper10
#else
#define SNVS_LP_MAX_TAMPER kSNVS_ExternalTamper1
#endif

/*! @brief List of SNVS_LP external tampers status */
typedef enum _snvs_lp_external_tamper_status
{
    kSNVS_TamperNotDetected = 0U,
    kSNVS_TamperDetected    = 1U
} snvs_lp_external_tamper_status_t;

/*! @brief SNVS_LP external tamper polarity */
typedef enum _snvs_lp_external_tamper_polarity
{
    kSNVS_ExternalTamperActiveLow  = 0U,
    kSNVS_ExternalTamperActiveHigh = 1U
} snvs_lp_external_tamper_polarity_t;

/*! @brief Structure is used to hold the date and time */
typedef struct _snvs_lp_srtc_datetime
{
    uint16_t year;  /*!< Range from 1970 to 2099.*/
    uint8_t month;  /*!< Range from 1 to 12.*/
    uint8_t day;    /*!< Range from 1 to 31 (depending on month).*/
    uint8_t hour;   /*!< Range from 0 to 23.*/
    uint8_t minute; /*!< Range from 0 to 59.*/
    uint8_t second; /*!< Range from 0 to 59.*/
} snvs_lp_srtc_datetime_t;

/*!
 * @brief SNVS_LP config structure
 *
 * This structure holds the configuration settings for the SNVS_LP peripheral. To initialize this
 * structure to reasonable defaults, call the SNVS_LP_GetDefaultConfig() function and pass a
 * pointer to your config structure instance.
 *
 * The config struct can be made const so it resides in flash
 */
typedef struct _snvs_lp_srtc_config
{
    bool srtcCalEnable;    /*!< true: SRTC calibration mechanism is enabled;
                                false: No calibration is used */
    uint32_t srtcCalValue; /*!< Defines signed calibration value for SRTC;
                                This is a 5-bit 2's complement value, range from -16 to +15 */
} snvs_lp_srtc_config_t;

/*!
 * @brief SNVS_LP Zeroizable Master Key programming mode.
 */
typedef enum _snvs_lp_zmk_program_mode
{
    kSNVS_ZMKSoftwareProgram, /*!< Software programming mode. */
    kSNVS_ZMKHardwareProgram, /*!< Hardware programming mode. */
} snvs_lp_zmk_program_mode_t;

/*!
 * @brief SNVS_LP Master Key mode.
 */
typedef enum _snvs_lp_master_key_mode
{
    kSNVS_OTPMK = 0, /*!< One Time Programmable Master Key. */
    kSNVS_ZMK   = 2, /*!< Zeroizable Master Key. */
    kSNVS_CMK   = 3, /*!< Combined Master Key, it is XOR of OPTMK and ZMK. */
} snvs_lp_master_key_mode_t;

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
 * @brief Ungates the SNVS clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application using the SNVS driver.
 *
 * @param base   SNVS peripheral base address
 * @param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_Init(SNVS_Type *base);

/*!
 * @brief Deinit the SNVS LP section.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_LP_Deinit(SNVS_Type *base);

/*! @}*/

/*!
 * @brief Ungates the SNVS clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application using the SNVS driver.
 *
 * @param base   SNVS peripheral base address
 * @param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_SRTC_Init(SNVS_Type *base, const snvs_lp_srtc_config_t *config);

/*!
 * @brief Stops the SRTC timer.
 *
 * @param base SNVS peripheral base address
 */
void SNVS_LP_SRTC_Deinit(SNVS_Type *base);

/*!
 * @brief Fills in the SNVS_LP config struct with the default settings.
 *
 * The default values are as follows.
 * @code
 *    config->srtccalenable = false;
 *    config->srtccalvalue = 0U;
 * @endcode
 * @param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_SRTC_GetDefaultConfig(snvs_lp_srtc_config_t *config);

/*!
 * @name Secure RTC (SRTC) current Time & Alarm
 * @{
 */

/*!
 * @brief Sets the SNVS SRTC date and time according to the given time structure.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the date and time details are stored.
 *
 * @return kStatus_Success: Success in setting the time and starting the SNVS SRTC
 *         kStatus_InvalidArgument: Error because the datetime format is incorrect
 */
status_t SNVS_LP_SRTC_SetDatetime(SNVS_Type *base, const snvs_lp_srtc_datetime_t *datetime);

/*!
 * @brief Gets the SNVS SRTC time and stores it in the given time structure.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the date and time details are stored.
 */
void SNVS_LP_SRTC_GetDatetime(SNVS_Type *base, snvs_lp_srtc_datetime_t *datetime);

/*!
 * @brief Sets the SNVS SRTC alarm time.
 *
 * The function sets the SRTC alarm. It also checks whether the specified alarm
 * time is greater than the present time. If not, the function does not set the alarm
 * and returns an error.
 * Please note, that SRTC alarm has limited resolution because only 32 most
 * significant bits of SRTC counter are compared to SRTC Alarm register.
 * If the alarm time is beyond SRTC resolution, the function does not set the alarm
 * and returns an error.
 *
 * @param base      SNVS peripheral base address
 * @param alarmTime Pointer to the structure where the alarm time is stored.
 *
 * @return kStatus_Success: success in setting the SNVS SRTC alarm
 *         kStatus_InvalidArgument: Error because the alarm datetime format is incorrect
 *         kStatus_Fail: Error because the alarm time has already passed or is beyond resolution
 */
status_t SNVS_LP_SRTC_SetAlarm(SNVS_Type *base, const snvs_lp_srtc_datetime_t *alarmTime);

/*!
 * @brief Returns the SNVS SRTC alarm time.
 *
 * @param base     SNVS peripheral base address
 * @param datetime Pointer to the structure where the alarm date and time details are stored.
 */
void SNVS_LP_SRTC_GetAlarm(SNVS_Type *base, snvs_lp_srtc_datetime_t *datetime);

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
static inline void SNVS_LP_SRTC_EnableInterrupts(SNVS_Type *base, uint32_t mask)
{
    base->LPCR |= mask;
}

/*!
 * @brief Disables the selected SNVS interrupts.
 *
 * @param base SNVS peripheral base address
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::snvs_interrupt_enable_t
 */
static inline void SNVS_LP_SRTC_DisableInterrupts(SNVS_Type *base, uint32_t mask)
{
    base->LPCR &= ~mask;
}

/*!
 * @brief Gets the enabled SNVS interrupts.
 *
 * @param base SNVS peripheral base address
 *
 * @return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::snvs_interrupt_enable_t
 */
uint32_t SNVS_LP_SRTC_GetEnabledInterrupts(SNVS_Type *base);

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
uint32_t SNVS_LP_SRTC_GetStatusFlags(SNVS_Type *base);

/*!
 * @brief  Clears the SNVS status flags.
 *
 * @param base SNVS peripheral base address
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::snvs_status_flags_t
 */
static inline void SNVS_LP_SRTC_ClearStatusFlags(SNVS_Type *base, uint32_t mask)
{
    base->LPSR |= mask;
}

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the SNVS SRTC time counter.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_LP_SRTC_StartTimer(SNVS_Type *base)
{
    base->LPCR |= SNVS_LPCR_SRTC_ENV_MASK;
    while (!(base->LPCR & SNVS_LPCR_SRTC_ENV_MASK))
    {
    }
}

/*!
 * @brief Stops the SNVS SRTC time counter.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_LP_SRTC_StopTimer(SNVS_Type *base)
{
    base->LPCR &= ~SNVS_LPCR_SRTC_ENV_MASK;
    while (base->LPCR & SNVS_LPCR_SRTC_ENV_MASK)
    {
    }
}

/*! @}*/

/*!
 * @name External tampering
 * @{
 */

/*!
 * @brief Enables the specified SNVS external tamper.
 *
 * @param base SNVS peripheral base address
 * @param pin SNVS external tamper pin
 * @param polarity Polarity of external tamper
 */
void SNVS_LP_EnableExternalTamper(SNVS_Type *base,
                                  snvs_lp_external_tamper_t pin,
                                  snvs_lp_external_tamper_polarity_t polarity);

/*!
 * @brief Disables the specified SNVS external tamper.
 *
 * @param base SNVS peripheral base address
 * @param pin SNVS external tamper pin
 */
void SNVS_LP_DisableExternalTamper(SNVS_Type *base, snvs_lp_external_tamper_t pin);

/*!
 * @brief Returns status of the specified external tamper.
 *
 * @param base SNVS peripheral base address
 * @param pin SNVS external tamper pin
 *
 * @return The status flag. This is the enumeration ::snvs_external_tamper_status_t
 */
snvs_lp_external_tamper_status_t SNVS_LP_GetExternalTamperStatus(SNVS_Type *base, snvs_lp_external_tamper_t pin);

/*!
 * @brief Clears status of the specified external tamper.
 *
 * @param base SNVS peripheral base address
 * @param pin SNVS external tamper pin
 */
void SNVS_LP_ClearExternalTamperStatus(SNVS_Type *base, snvs_lp_external_tamper_t pin);

/*! @}*/

/*!
 * @name Monotonic Counter (MC)
 * @{
 */

/*!
 * @brief Enable or disable the Monotonic Counter.
 *
 * @param base SNVS peripheral base address
 * @param enable Pass true to enable, false to disable.
 */
static inline void SNVS_LP_EnableMonotonicCounter(SNVS_Type *base, bool enable)
{
    if (enable)
    {
        base->LPCR |= SNVS_LPCR_MC_ENV_MASK;
    }
    else
    {
        base->LPCR &= (~SNVS_LPCR_MC_ENV_MASK);
    }
}

/*!
 * @brief Get the current Monotonic Counter.
 *
 * @param base SNVS peripheral base address
 * @return Current Monotonic Counter value.
 */
uint64_t SNVS_LP_GetMonotonicCounter(SNVS_Type *base);

/*!
 * @brief Increase the Monotonic Counter.
 *
 * Increase the Monotonic Counter by 1.
 *
 * @param base SNVS peripheral base address
 */
static inline void SNVS_LP_IncreaseMonotonicCounter(SNVS_Type *base)
{
    /* Write to the LPSMCLR or LPSMCLR, the counter increases. */
    *((volatile uint32_t *)(&(base->LPSMCLR))) = 0xFFFFFFFFU;
}

/*! @}*/

/*!
 * @name Zeroizable Master Key (ZMK)
 * @{
 */

/*!
 * @brief Write Zeroizable Master Key (ZMK) to the SNVS registers.
 *
 * @param base SNVS peripheral base address
 * @param ZMKey The ZMK write to the SNVS register.
 */
void SNVS_LP_WriteZeroizableMasterKey(SNVS_Type *base, uint32_t ZMKey[SNVS_ZMK_REG_COUNT]);

/*!
 * @brief Set Zeroizable Master Key valid.
 *
 * This API could only be called when using software programming mode. After writing
 * ZMK using @ref SNVS_LP_WriteZeroizableMasterKey, call this API to make the ZMK
 * valid.
 *
 * @param base SNVS peripheral base address
 * @param valid Pass true to set valid, false to set invalid.
 */
static inline void SNVS_LP_SetZeroizableMasterKeyValid(SNVS_Type *base, bool valid)
{
    if (valid)
    {
        base->LPMKCR |= SNVS_LPMKCR_ZMK_VAL_MASK;
    }
    else
    {
        base->LPMKCR &= (~SNVS_LPMKCR_ZMK_VAL_MASK);
    }
}

/*!
 * @brief Get Zeroizable Master Key valid status.
 *
 * In hardware programming mode, call this API to check whether the ZMK is valid.
 *
 * @param base SNVS peripheral base address
 * @return true if valid, false if invalid.
 */
static inline bool SNVS_LP_GetZeroizableMasterKeyValid(SNVS_Type *base)
{
    return (SNVS_LPMKCR_ZMK_VAL_MASK == (base->LPMKCR & SNVS_LPMKCR_ZMK_VAL_MASK));
}

/*!
 * @brief Set Zeroizable Master Key programming mode.
 *
 * @param base SNVS peripheral base address
 * @param mode ZMK programming mode.
 */
static inline void SNVS_LP_SetZeroizableMasterKeyProgramMode(SNVS_Type *base, snvs_lp_zmk_program_mode_t mode)
{
    if (kSNVS_ZMKSoftwareProgram == mode)
    {
        base->LPMKCR &= (~SNVS_LPMKCR_ZMK_HWP_MASK);
    }
    else
    {
        base->LPMKCR |= SNVS_LPMKCR_ZMK_HWP_MASK;
    }
}

/*!
 * @brief Enable or disable Zeroizable Master Key ECC.
 *
 * @param base SNVS peripheral base address
 * @param enable Pass true to enable, false to disable.
 */
static inline void SNVS_LP_EnableZeroizableMasterKeyECC(SNVS_Type *base, bool enable)
{
    if (enable)
    {
        base->LPMKCR |= SNVS_LPMKCR_ZMK_ECC_EN_MASK;
    }
    else
    {
        base->LPMKCR &= (~SNVS_LPMKCR_ZMK_ECC_EN_MASK);
    }
}

/*!
 * @brief Set SNVS Master Key mode.
 *
 * @param base SNVS peripheral base address
 * @param mode Master Key mode.
 * @note When @ref kSNVS_ZMK or @ref kSNVS_CMK used, the SNVS_HP must be configured
 * to enable the master key selection.
 */
static inline void SNVS_LP_SetMasterKeyMode(SNVS_Type *base, snvs_lp_master_key_mode_t mode)
{
    uint32_t lpmkcr = base->LPMKCR;
    lpmkcr          = (lpmkcr & (~SNVS_LPMKCR_MASTER_KEY_SEL_MASK)) | SNVS_LPMKCR_MASTER_KEY_SEL(mode);
    base->LPMKCR    = lpmkcr;
}

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_SNVS_LP_H_ */
