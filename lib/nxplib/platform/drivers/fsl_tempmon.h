/*
 * Copyright  2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_TEMPMON_H_
#define _FSL_TEMPMON_H_

#include "fsl_common.h"

/*!
 * @addtogroup tempmon
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_TEMPMON_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*! @brief TEMPMON temperature structure. */
typedef struct _tempmon_config
{
    uint16_t frequency;      /*!< The temperature measure frequency.*/
    uint32_t highAlarmTemp;  /*!< The high alarm temperature.*/
    uint32_t panicAlarmTemp; /*!< The panic alarm temperature.*/
    uint32_t lowAlarmTemp;   /*!< The low alarm temperature.*/
} tempmon_config_t;

/*! @brief TEMPMON alarm mode. */
typedef enum _tempmon_alarm_mode
{
    kTEMPMON_HighAlarmMode  = 0U, /*!< The high alarm temperature interrupt mode.*/
    kTEMPMON_PanicAlarmMode = 1U, /*!< The panic alarm temperature interrupt mode.*/
    kTEMPMON_LowAlarmMode   = 2U, /*!< The low alarm temperature interrupt mode.*/
} tempmon_alarm_mode;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the TEMPMON module.
 *
 * @param base TEMPMON base pointer
 * @param config Pointer to configuration structure.
 */
void TEMPMON_Init(TEMPMON_Type *base, const tempmon_config_t *config);

/*!
 * @brief Deinitializes the TEMPMON module.
 *
 * @param base TEMPMON base pointer
 */
void TEMPMON_Deinit(TEMPMON_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the TEMPMON configuration structure to a default value. The default
 * values are:
 *   tempmonConfig->frequency = 0x02U;
 *   tempmonConfig->highAlarmTemp = 44U;
 *   tempmonConfig->panicAlarmTemp = 90U;
 *   tempmonConfig->lowAlarmTemp = 39U;
 *
 * @param config Pointer to a configuration structure.
 */
void TEMPMON_GetDefaultConfig(tempmon_config_t *config);

/*!
 * @brief start the temperature measurement process.
 *
 * @param base TEMPMON base pointer.
 */
static inline void TEMPMON_StartMeasure(TEMPMON_Type *base)
{
    base->TEMPSENSE0 |= TEMPMON_TEMPSENSE0_MEASURE_TEMP_MASK;
}

/*!
 * @brief stop the measurement process.
 *
 * @param base TEMPMON base pointer
 */
static inline void TEMPMON_StopMeasure(TEMPMON_Type *base)
{
    base->TEMPSENSE0 &= ~TEMPMON_TEMPSENSE0_MEASURE_TEMP_MASK;
}

/*!
 * @brief Get current temperature with the fused temperature calibration data.
 *
 * @param base TEMPMON base pointer
 * @return current temperature with degrees Celsius.
 */
float TEMPMON_GetCurrentTemperature(TEMPMON_Type *base);

/*!
 * @brief Set the temperature count (raw sensor output) that will generate an alarm interrupt.
 *
 * @param base TEMPMON base pointer
 * @param tempVal The alarm temperature with degrees Celsius
 * @param alarmMode The alarm mode.
 */
void TEMPMON_SetTempAlarm(TEMPMON_Type *base, uint32_t tempVal, tempmon_alarm_mode alarmMode);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_TEMPMON_H_ */
