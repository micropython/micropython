/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_snvs_lp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.snvs_lp"
#endif

#define SECONDS_IN_A_DAY (86400U)
#define SECONDS_IN_A_HOUR (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR (365U)
#define YEAR_RANGE_START (1970U)
#define YEAR_RANGE_END (2099U)

#define SNVS_DEFAULT_PGD_VALUE (0x41736166U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Checks whether the date and time passed in is valid
 *
 * @param datetime Pointer to structure where the date and time details are stored
 *
 * @return Returns false if the date & time details are out of range; true if in range
 */
static bool SNVS_LP_CheckDatetimeFormat(const snvs_lp_srtc_datetime_t *datetime);

/*!
 * @brief Converts time data from datetime to seconds
 *
 * @param datetime Pointer to datetime structure where the date and time details are stored
 *
 * @return The result of the conversion in seconds
 */
static uint32_t SNVS_LP_ConvertDatetimeToSeconds(const snvs_lp_srtc_datetime_t *datetime);

/*!
 * @brief Converts time data from seconds to a datetime structure
 *
 * @param seconds  Seconds value that needs to be converted to datetime format
 * @param datetime Pointer to the datetime structure where the result of the conversion is stored
 */
static void SNVS_LP_ConvertSecondsToDatetime(uint32_t seconds, snvs_lp_srtc_datetime_t *datetime);

/*!
 * @brief Returns SRTC time in seconds.
 *
 * This function is used internally to get actual SRTC time in seconds.
 *
 * @param base SNVS peripheral base address
 *
 * @return SRTC time in seconds
 */
static uint32_t SNVS_LP_SRTC_GetSeconds(SNVS_Type *base);

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
/*!
 * @brief Get the SNVS instance from peripheral base address.
 *
 * @param base SNVS peripheral base address.
 *
 * @return SNVS instance.
 */
static uint32_t SNVS_LP_GetInstance(SNVS_Type *base);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
/*! @brief Pointer to snvs_lp clock. */
const clock_ip_name_t s_snvsLpClock[] = SNVS_LP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static bool SNVS_LP_CheckDatetimeFormat(const snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    /* Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Check year, month, hour, minute, seconds */
    if ((datetime->year < YEAR_RANGE_START) || (datetime->year > YEAR_RANGE_END) || (datetime->month > 12U) ||
        (datetime->month < 1U) || (datetime->hour >= 24U) || (datetime->minute >= 60U) || (datetime->second >= 60U))
    {
        /* If not correct then error*/
        return false;
    }

    /* Adjust the days in February for a leap year */
    if ((((datetime->year & 3U) == 0) && (datetime->year % 100 != 0)) || (datetime->year % 400 == 0))
    {
        daysPerMonth[2] = 29U;
    }

    /* Check the validity of the day */
    if ((datetime->day > daysPerMonth[datetime->month]) || (datetime->day < 1U))
    {
        return false;
    }

    return true;
}

static uint32_t SNVS_LP_ConvertDatetimeToSeconds(const snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    /* Number of days from begin of the non Leap-year*/
    /* Number of days from begin of the non Leap-year*/
    uint16_t monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    uint32_t seconds;

    /* Compute number of days from 1970 till given year*/
    seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    seconds += monthDays[datetime->month];
    /* Add days in given month. We subtract the current day as it is
     * represented in the hours, minutes and seconds field*/
    seconds += (datetime->day - 1);
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) +
              (datetime->minute * SECONDS_IN_A_MINUTE) + datetime->second;

    return seconds;
}

static void SNVS_LP_ConvertSecondsToDatetime(uint32_t seconds, snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t x;
    uint32_t secondsRemaining, days;
    uint16_t daysInYear;
    /* Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Start with the seconds value that is passed in to be converted to date time format */
    secondsRemaining = seconds;

    /* Calcuate the number of days, we add 1 for the current day which is represented in the
     * hours and seconds field
     */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1;

    /* Update seconds left*/
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

    /* Calculate the datetime hour, minute and second fields */
    datetime->hour   = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60U;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* Calculate year */
    daysInYear     = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while (days > daysInYear)
    {
        /* Decrease day count by a year and increment year by 1 */
        days -= daysInYear;
        datetime->year++;

        /* Adjust the number of days for a leap year */
        if (datetime->year & 3U)
        {
            daysInYear = DAYS_IN_A_YEAR;
        }
        else
        {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

    /* Adjust the days in February for a leap year */
    if (!(datetime->year & 3U))
    {
        daysPerMonth[2] = 29U;
    }

    for (x = 1U; x <= 12U; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->month = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;
}

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
static uint32_t SNVS_LP_GetInstance(SNVS_Type *base)
{
    return 0U;
}
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*!
 * brief Ungates the SNVS clock and configures the peripheral for basic operation.
 *
 * note This API should be called at the beginning of the application using the SNVS driver.
 *
 * param base   SNVS peripheral base address
 * param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_Init(SNVS_Type *base)
{
#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
    uint32_t instance = SNVS_LP_GetInstance(base);
    CLOCK_EnableClock(s_snvsLpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Power glitch detector: set the PGD value and clear the previous status. */
    base->LPPGDR = SNVS_DEFAULT_PGD_VALUE;
    base->LPSR   = SNVS_LPSR_PGD_MASK;
}

/*!
 * brief Deinit the SNVS LP section.
 *
 * param base SNVS peripheral base address
 */
void SNVS_LP_Deinit(SNVS_Type *base)
{
#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
    uint32_t instance = SNVS_LP_GetInstance(base);
    CLOCK_DisableClock(s_snvsLpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Ungates the SNVS clock and configures the peripheral for basic operation.
 *
 * note This API should be called at the beginning of the application using the SNVS driver.
 *
 * param base   SNVS peripheral base address
 * param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_SRTC_Init(SNVS_Type *base, const snvs_lp_srtc_config_t *config)
{
    assert(config);

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
    uint32_t instance = SNVS_LP_GetInstance(base);
    CLOCK_EnableClock(s_snvsLpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    int pin;

    if (config->srtcCalEnable)
    {
        base->LPCR = SNVS_LPCR_LPCALB_VAL_MASK & (config->srtcCalValue << SNVS_LPCR_LPCALB_VAL_SHIFT);
        base->LPCR |= SNVS_LPCR_LPCALB_EN_MASK;
    }

    for (pin = kSNVS_ExternalTamper1; pin <= SNVS_LP_MAX_TAMPER; pin++)
    {
        SNVS_LP_DisableExternalTamper(SNVS, (snvs_lp_external_tamper_t)pin);
        SNVS_LP_ClearExternalTamperStatus(SNVS, (snvs_lp_external_tamper_t)pin);
    }
}

/*!
 * brief Stops the SRTC timer.
 *
 * param base SNVS peripheral base address
 */
void SNVS_LP_SRTC_Deinit(SNVS_Type *base)
{
    base->LPCR &= ~SNVS_LPCR_SRTC_ENV_MASK;

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && \
     defined(SNVS_LP_CLOCKS))
    uint32_t instance = SNVS_LP_GetInstance(base);
    CLOCK_DisableClock(s_snvsLpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Fills in the SNVS_LP config struct with the default settings.
 *
 * The default values are as follows.
 * code
 *    config->srtccalenable = false;
 *    config->srtccalvalue = 0U;
 * endcode
 * param config Pointer to the user's SNVS configuration structure.
 */
void SNVS_LP_SRTC_GetDefaultConfig(snvs_lp_srtc_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->srtcCalEnable = false;
    config->srtcCalValue  = 0U;
}

static uint32_t SNVS_LP_SRTC_GetSeconds(SNVS_Type *base)
{
    uint32_t seconds = 0;
    uint32_t tmp     = 0;

    /* Do consecutive reads until value is correct */
    do
    {
        seconds = tmp;
        tmp     = (base->LPSRTCMR << 17U) | (base->LPSRTCLR >> 15U);
    } while (tmp != seconds);

    return seconds;
}

/*!
 * brief Sets the SNVS SRTC date and time according to the given time structure.
 *
 * param base     SNVS peripheral base address
 * param datetime Pointer to the structure where the date and time details are stored.
 *
 * return kStatus_Success: Success in setting the time and starting the SNVS SRTC
 *         kStatus_InvalidArgument: Error because the datetime format is incorrect
 */
status_t SNVS_LP_SRTC_SetDatetime(SNVS_Type *base, const snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t seconds = 0U;
    uint32_t tmp     = base->LPCR;

    /* disable RTC */
    SNVS_LP_SRTC_StopTimer(base);

    /* Return error if the time provided is not valid */
    if (!(SNVS_LP_CheckDatetimeFormat(datetime)))
    {
        return kStatus_InvalidArgument;
    }

    /* Set time in seconds */
    seconds = SNVS_LP_ConvertDatetimeToSeconds(datetime);

    base->LPSRTCMR = (uint32_t)(seconds >> 17U);
    base->LPSRTCLR = (uint32_t)(seconds << 15U);

    /* reenable SRTC in case that it was enabled before */
    if (tmp & SNVS_LPCR_SRTC_ENV_MASK)
    {
        SNVS_LP_SRTC_StartTimer(base);
    }

    return kStatus_Success;
}

/*!
 * brief Gets the SNVS SRTC time and stores it in the given time structure.
 *
 * param base     SNVS peripheral base address
 * param datetime Pointer to the structure where the date and time details are stored.
 */
void SNVS_LP_SRTC_GetDatetime(SNVS_Type *base, snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    SNVS_LP_ConvertSecondsToDatetime(SNVS_LP_SRTC_GetSeconds(base), datetime);
}

/*!
 * brief Sets the SNVS SRTC alarm time.
 *
 * The function sets the SRTC alarm. It also checks whether the specified alarm
 * time is greater than the present time. If not, the function does not set the alarm
 * and returns an error.
 * Please note, that SRTC alarm has limited resolution because only 32 most
 * significant bits of SRTC counter are compared to SRTC Alarm register.
 * If the alarm time is beyond SRTC resolution, the function does not set the alarm
 * and returns an error.
 *
 * param base      SNVS peripheral base address
 * param alarmTime Pointer to the structure where the alarm time is stored.
 *
 * return kStatus_Success: success in setting the SNVS SRTC alarm
 *         kStatus_InvalidArgument: Error because the alarm datetime format is incorrect
 *         kStatus_Fail: Error because the alarm time has already passed or is beyond resolution
 */
status_t SNVS_LP_SRTC_SetAlarm(SNVS_Type *base, const snvs_lp_srtc_datetime_t *alarmTime)
{
    assert(alarmTime);

    uint32_t alarmSeconds = 0U;
    uint32_t currSeconds  = 0U;
    uint32_t tmp          = base->LPCR;

    /* Return error if the alarm time provided is not valid */
    if (!(SNVS_LP_CheckDatetimeFormat(alarmTime)))
    {
        return kStatus_InvalidArgument;
    }

    alarmSeconds = SNVS_LP_ConvertDatetimeToSeconds(alarmTime);
    currSeconds  = SNVS_LP_SRTC_GetSeconds(base);

    /* Return error if the alarm time has passed */
    if (alarmSeconds <= currSeconds)
    {
        return kStatus_Fail;
    }

    /* disable SRTC alarm interrupt */
    base->LPCR &= ~SNVS_LPCR_LPTA_EN_MASK;
    while (base->LPCR & SNVS_LPCR_LPTA_EN_MASK)
    {
    }

    /* Set alarm in seconds*/
    base->LPTAR = alarmSeconds;

    /* reenable SRTC alarm interrupt in case that it was enabled before */
    base->LPCR = tmp;

    return kStatus_Success;
}

/*!
 * brief Returns the SNVS SRTC alarm time.
 *
 * param base     SNVS peripheral base address
 * param datetime Pointer to the structure where the alarm date and time details are stored.
 */
void SNVS_LP_SRTC_GetAlarm(SNVS_Type *base, snvs_lp_srtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t alarmSeconds = 0U;

    /* Get alarm in seconds  */
    alarmSeconds = base->LPTAR;

    SNVS_LP_ConvertSecondsToDatetime(alarmSeconds, datetime);
}

/*!
 * brief Gets the SNVS status flags.
 *
 * param base SNVS peripheral base address
 *
 * return The status flags. This is the logical OR of members of the
 *         enumeration ::snvs_status_flags_t
 */
uint32_t SNVS_LP_SRTC_GetStatusFlags(SNVS_Type *base)
{
    uint32_t flags = 0U;

    if (base->LPSR & SNVS_LPSR_LPTA_MASK)
    {
        flags |= kSNVS_SRTC_AlarmInterruptFlag;
    }

    return flags;
}

/*!
 * brief Gets the enabled SNVS interrupts.
 *
 * param base SNVS peripheral base address
 *
 * return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::snvs_interrupt_enable_t
 */
uint32_t SNVS_LP_SRTC_GetEnabledInterrupts(SNVS_Type *base)
{
    uint32_t val = 0U;

    if (base->LPCR & SNVS_LPCR_LPTA_EN_MASK)
    {
        val |= kSNVS_SRTC_AlarmInterrupt;
    }

    return val;
}

/*!
 * brief Enables the specified SNVS external tamper.
 *
 * param base SNVS peripheral base address
 * param pin SNVS external tamper pin
 * param polarity Polarity of external tamper
 */
void SNVS_LP_EnableExternalTamper(SNVS_Type *base,
                                  snvs_lp_external_tamper_t pin,
                                  snvs_lp_external_tamper_polarity_t polarity)
{
    switch (pin)
    {
        case (kSNVS_ExternalTamper1):
            base->LPTDCR = (base->LPTDCR & ~(1U << SNVS_LPTDCR_ET1P_SHIFT)) | (polarity << SNVS_LPTDCR_ET1P_SHIFT);
            base->LPTDCR |= SNVS_LPTDCR_ET1_EN_MASK;
            break;
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
        case (kSNVS_ExternalTamper2):
            base->LPTDCR = (base->LPTDCR & ~(1U << SNVS_LPTDCR_ET2P_SHIFT)) | (polarity << SNVS_LPTDCR_ET2P_SHIFT);
            base->LPTDCR |= SNVS_LPTDCR_ET2_EN_MASK;
            break;
        case (kSNVS_ExternalTamper3):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET3P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET3P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET3_EN_MASK;
            break;
        case (kSNVS_ExternalTamper4):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET4P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET4P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET4_EN_MASK;
            break;
        case (kSNVS_ExternalTamper5):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET5P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET5P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET5_EN_MASK;
            break;
        case (kSNVS_ExternalTamper6):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET6P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET6P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET6_EN_MASK;
            break;
        case (kSNVS_ExternalTamper7):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET7P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET7P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET7_EN_MASK;
            break;
        case (kSNVS_ExternalTamper8):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET8P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET8P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET8_EN_MASK;
            break;
        case (kSNVS_ExternalTamper9):
            base->LPTDC2R = (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET9P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET9P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET9_EN_MASK;
            break;
        case (kSNVS_ExternalTamper10):
            base->LPTDC2R =
                (base->LPTDC2R & ~(1U << SNVS_LPTDC2R_ET10P_SHIFT)) | (polarity << SNVS_LPTDC2R_ET10P_SHIFT);
            base->LPTDC2R |= SNVS_LPTDC2R_ET10_EN_MASK;
            break;
#endif
        default:
            break;
    }
}

/*!
 * brief Disables the specified SNVS external tamper.
 *
 * param base SNVS peripheral base address
 * param pin SNVS external tamper pin
 */
void SNVS_LP_DisableExternalTamper(SNVS_Type *base, snvs_lp_external_tamper_t pin)
{
    switch (pin)
    {
        case (kSNVS_ExternalTamper1):
            base->LPTDCR &= ~SNVS_LPTDCR_ET1_EN_MASK;
            break;
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
        case (kSNVS_ExternalTamper2):
            base->LPTDCR &= ~SNVS_LPTDCR_ET2_EN_MASK;
            break;
        case (kSNVS_ExternalTamper3):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET3_EN_MASK;
            break;
        case (kSNVS_ExternalTamper4):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET4_EN_MASK;
            break;
        case (kSNVS_ExternalTamper5):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET5_EN_MASK;
            break;
        case (kSNVS_ExternalTamper6):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET6_EN_MASK;
            break;
        case (kSNVS_ExternalTamper7):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET7_EN_MASK;
            break;
        case (kSNVS_ExternalTamper8):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET8_EN_MASK;
            break;
        case (kSNVS_ExternalTamper9):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET9_EN_MASK;
            break;
        case (kSNVS_ExternalTamper10):
            base->LPTDC2R &= ~SNVS_LPTDC2R_ET10_EN_MASK;
            break;
#endif
        default:
            break;
    }
}

/*!
 * brief Returns status of the specified external tamper.
 *
 * param base SNVS peripheral base address
 * param pin SNVS external tamper pin
 *
 * return The status flag. This is the enumeration ::snvs_external_tamper_status_t
 */
snvs_lp_external_tamper_status_t SNVS_LP_GetExternalTamperStatus(SNVS_Type *base, snvs_lp_external_tamper_t pin)
{
    snvs_lp_external_tamper_status_t status = kSNVS_TamperNotDetected;

    switch (pin)
    {
        case (kSNVS_ExternalTamper1):
            status = (base->LPSR & SNVS_LPSR_ET1D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
        case (kSNVS_ExternalTamper2):
            status = (base->LPSR & SNVS_LPSR_ET2D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper3):
            status = (base->LPTDSR & SNVS_LPTDSR_ET3D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper4):
            status = (base->LPTDSR & SNVS_LPTDSR_ET4D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper5):
            status = (base->LPTDSR & SNVS_LPTDSR_ET5D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper6):
            status = (base->LPTDSR & SNVS_LPTDSR_ET6D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper7):
            status = (base->LPTDSR & SNVS_LPTDSR_ET7D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper8):
            status = (base->LPTDSR & SNVS_LPTDSR_ET8D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper9):
            status = (base->LPTDSR & SNVS_LPTDSR_ET9D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
        case (kSNVS_ExternalTamper10):
            status = (base->LPTDSR & SNVS_LPTDSR_ET10D_MASK) ? kSNVS_TamperDetected : kSNVS_TamperNotDetected;
            break;
#endif
        default:
            break;
    }
    return status;
}

/*!
 * brief Clears status of the specified external tamper.
 *
 * param base SNVS peripheral base address
 * param pin SNVS external tamper pin
 */
void SNVS_LP_ClearExternalTamperStatus(SNVS_Type *base, snvs_lp_external_tamper_t pin)
{
    base->LPSR |= SNVS_LPSR_ET1D_MASK;

    switch (pin)
    {
        case (kSNVS_ExternalTamper1):
            base->LPSR |= SNVS_LPSR_ET1D_MASK;
            break;
#if defined(FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER) && (FSL_FEATURE_SNVS_HAS_MULTIPLE_TAMPER > 1)
        case (kSNVS_ExternalTamper2):
            base->LPSR |= SNVS_LPSR_ET2D_MASK;
            break;
        case (kSNVS_ExternalTamper3):
            base->LPTDSR |= SNVS_LPTDSR_ET3D_MASK;
            break;
        case (kSNVS_ExternalTamper4):
            base->LPTDSR |= SNVS_LPTDSR_ET4D_MASK;
            break;
        case (kSNVS_ExternalTamper5):
            base->LPTDSR |= SNVS_LPTDSR_ET5D_MASK;
            break;
        case (kSNVS_ExternalTamper6):
            base->LPTDSR |= SNVS_LPTDSR_ET6D_MASK;
            break;
        case (kSNVS_ExternalTamper7):
            base->LPTDSR |= SNVS_LPTDSR_ET7D_MASK;
            break;
        case (kSNVS_ExternalTamper8):
            base->LPTDSR |= SNVS_LPTDSR_ET8D_MASK;
            break;
        case (kSNVS_ExternalTamper9):
            base->LPTDSR |= SNVS_LPTDSR_ET9D_MASK;
            break;
        case (kSNVS_ExternalTamper10):
            base->LPTDSR |= SNVS_LPTDSR_ET10D_MASK;
            break;
#endif
        default:
            break;
    }
}

/*!
 * brief Get the current Monotonic Counter.
 *
 * param base SNVS peripheral base address
 * return Current Monotonic Counter value.
 */
uint64_t SNVS_LP_GetMonotonicCounter(SNVS_Type *base)
{
    uint32_t mc_lsb, mc_msb;

    mc_msb = base->LPSMCMR;
    mc_lsb = base->LPSMCLR;

    return ((uint64_t)mc_msb << 32UL) | (uint64_t)mc_lsb;
}

/*!
 * brief Write Zeroizable Master Key (ZMK) to the SNVS registers.
 *
 * param base SNVS peripheral base address
 * param ZMKey The ZMK write to the SNVS register.
 */
void SNVS_LP_WriteZeroizableMasterKey(SNVS_Type *base, uint32_t ZMKey[SNVS_ZMK_REG_COUNT])
{
    uint8_t i = 0;

    for (i = 0; i < SNVS_ZMK_REG_COUNT; i++)
    {
        base->LPZMKR[i] = ZMKey[i];
    }
}
