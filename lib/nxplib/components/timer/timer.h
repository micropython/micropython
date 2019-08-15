/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TIMER_H_
#define _TIMER_H_

/*!
 * @addtogroup Timer_Adapter
 * @{
 */

/*!
 * @brief The timer adapter component
 *
 * The timer adapter is built based on the timer SDK driver provided by the NXP
 * MCUXpresso SDK. The timer adapter could provide high accuracy timer for user.
 * Since callback function would be handled in ISR, and timer clock use high
 * accuracy clock, user can get accuracy millisecond timer.
 *
 * The timer adapter would be used with different HW timer modules like FTM, PIT, LPTMR.
 * But at the same time, only one HW timer module could be used. On different platforms, different
 * HW timer module would be used. For the platforms which have multiple HW timer modules,
 * one HW timer module would be selected as the default, but it is easy to change the default
 * HW timer module to another. Just two steps to switch the HW timer module:
 * 1.Remove the default HW timer module source file from the project
 * 2.Add the expected HW timer module source file to the project.
 * For example, in platform FRDM-K64F, there are two HW timer modules available, FTM and PIT.
 * FTM is used as the default HW timer, so ftm_adapter.c and timer.h is included in the project by
 * default. If PIT is expected to be used as the HW timer, ftm_adapter.c need to be removed from the
 * project and pit_adapter.c should be included in the project
 */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
***********************************************************************************/

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
/*! @brief HAL timer callback function. */
typedef void (*hal_timer_callback_t)(void* param);

/*! @brief HAL timer status. */
typedef enum _hal_timer_status
{
    kStatus_HAL_TimerSuccess = kStatus_Success,                           /*!< Success */
    kStatus_HAL_TimerNotSupport = MAKE_STATUS(kStatusGroup_HAL_TIMER, 1), /*!< Not Support */
    kStatus_HAL_TimerIsUsed = MAKE_STATUS(kStatusGroup_HAL_TIMER, 2),     /*!< timer is used  */
    kStatus_HAL_TimerInvalid = MAKE_STATUS(kStatusGroup_HAL_TIMER, 3),    /*!< timer is invalid  */
    kStatus_HAL_TimerOutOfRanger = MAKE_STATUS(kStatusGroup_HAL_TIMER, 4), /*!< timer is Out Of Ranger */
} hal_timer_status_t;

/*! @brief HAL timer configuration structure for HAL timer setting. */
typedef struct _hal_timer_config
{
    uint32_t timeout;                     /*!< Timeout of the timer, should use microseconds, for example: if set timeout to 1000, mean 1000 microseconds
                                               interval would generate timer timeout interrupt*/
    uint32_t srcClock_Hz;                 /*!< Source clock of the timer */
    uint8_t  instance;                    /*!< Hardware timer module instance, for example: if you want use FTM0,then the instance is configured to 0, if
                                               you want use FTM2 hardware timer, then configure the instance to 2, detail information please refer to the
                                               SOC corresponding RM.Invalid instance value will cause initialization failure. */
} hal_timer_config_t;

#define HAL_TIMER_HANDLE_SIZE                (20U)

/*! @brief HAL timer handle. */
typedef void* hal_timer_handle_t;

#if defined(__GIC_PRIO_BITS)
#define HAL_TIMER_ISR_PRIORITY (25U)
#else
#if defined(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
#define HAL_TIMER_ISR_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
#else
/* The default value 3 is used to support different ARM Core, such as CM0P, CM4, CM7, and CM33, etc.
 * The minimum number of priority bits implemented in the NVIC is 2 on these SOCs. The value of mininum
 * priority is 3 (2^2 - 1). So, the default value is 3.
 */
#define HAL_TIMER_ISR_PRIORITY (3U)
#endif
#endif

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
#if defined(__cplusplus)
    extern "C" {
#endif /* _cplusplus */

/*!
 * @brief Initializes the timer adapter module for a timer basic operation.
 *
 * @note This API should be called at the beginning of the application using the timer adapter.
 * For Initializes timer adapter,
 *  @code
 *     uint8_t halTimerHandle[HAL_TIMER_HANDLE_SIZE];
 *   hal_timer_config_t halTimerConfig;
 *   halTimerConfig.timeout = 1000;
 *   halTimerConfig.srcClock_Hz = BOARD_GetTimeSrcClock();
 *   halTimerConfig.instance = 0;
 *   HAL_TimerInit((hal_timer_handle_t)&halTimerHandle, &halTimerConfig);
 *  @endcode
 *
 * @param halTimerHandle     HAL timer adapter handle, the handle buffer with size #HAL_TIMER_HANDLE_SIZE should
 *                           be allocated at upper level
 * @param halTimerConfig     A pointer to the HAL timer configuration structure
 * @retval kStatus_HAL_TimerSuccess The timer adapter module initialization succeed.
 * @retval kStatus_HAL_TimerOutOfRanger The timer adapter instance out of ranger.
 */
hal_timer_status_t HAL_TimerInit(hal_timer_handle_t halTimerHandle, hal_timer_config_t* halTimerConfig);

/*!
 * @brief DeInitilizate the timer adapter module.
 *
 * @note This API should be called when not using the timer adapter anymore.
 *
 * @param halTimerHandle     HAL timer adapter handle
 */
void HAL_TimerDeinit(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Enable the timer adapter module.
 *
 * @note This API should be called when enable the timer adapter.
 *
 * @param halTimerHandle     HAL timer adapter handle
 */
void HAL_TimerEnable(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Disable the timer adapter module.
 *
 * @note This API should be called when disable the timer adapter.
 *
 * @param halTimerHandle     HAL timer adapter handle
 */
void HAL_TimerDisable(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Install the timer adapter module callback function.
 *
 * @note This API should be called to when to install callback function for the timer.Since callback function
 *       would be handled in ISR, and timer clock use high accuracy clock, user can get accuracy millisecond timer.
 *
 * @param halTimerHandle     HAL timer adapter handle
 * @param callback            The installed callback function by upper layer
 * @param callbackParam         The callback function parameter
 */
void HAL_TimerInstallCallback(hal_timer_handle_t halTimerHandle, hal_timer_callback_t callback, void* callbackParam);

/*!
 * @brief Get the timer count of the timer adapter.
 *
 * @note This API should be return the real-time timer counting value in a range from 0 to a
 *        timer period, and return microseconds.
 *
 * @param halTimerHandle     HAL timer adapter handle
 * @retval the real-time timer counting value and return microseconds.
 */
uint32_t HAL_TimerGetCurrentTimerCount(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Update the timeout of the timer adapter to generate timeout interrupt.
 *
 * @note This API should be called when need set the timeout of the timer interrupt..
 *
 * @param halTimerHandle     HAL timer adapter handle
 * @param Timeout            Timeout time, should be used microseconds.
 * @retval kStatus_HAL_TimerSuccess The timer adapter module update timeout succeed.
 * @retval kStatus_HAL_TimerOutOfRanger The timer adapter set the timeout out of ranger.
 */
hal_timer_status_t HAL_TimerUpdateTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout);

/*!
 * @brief Get maximum Timer timeout
 *
 * @note This API should to get maximum Timer timeout value to avoid overflow
 *
 * @param halTimerHandle     HAL timer adapter handle
 * @retval get the real-time timer maximum timeout value and return microseconds.
 */
uint32_t HAL_TimerGetMaxTimeout(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Timer adapter power up function.
 *
 * @note This API should be called by low power module when system exit from sleep mode.
 *
 * @param halTimerHandle     HAL timer adapter handle
 */
void HAL_TimerExitLowpower(hal_timer_handle_t halTimerHandle);

/*!
 * @brief Timer adapter power down function.
 *
 * @note This API should be called by low power module before system enter into sleep mode.
 *
 * @param halTimerHandle     HAL timer adapter handle
 */
void HAL_TimerEnterLowpower(hal_timer_handle_t halTimerHandle);

#if defined(__cplusplus)
}
#endif
/*! @}*/
#endif /* _TIMER_H_ */
