/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "timer.h"
#include "fsl_device_registers.h"
#include "fsl_ctimer.h"

#define gHalTimerInstanceNum_c (FSL_FEATURE_SOC_CTIMER_COUNT)
#define gStackTimerChannel_c (kCTIMER_Match_0)
typedef struct _hal_timer_handle_struct_t
{
    uint32_t timeout;
    uint32_t timerClock_Hz;
    hal_timer_callback_t callback;
    void *callbackParam;
    uint8_t instance;
} hal_timer_handle_struct_t;
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static hal_timer_handle_t g_timerHandle[gHalTimerInstanceNum_c];
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static void HAL_TimerInterruptHandle(uint8_t instance)
{
    hal_timer_handle_struct_t *halTimerState = (hal_timer_handle_struct_t *)g_timerHandle[instance];

    if (halTimerState->callback != NULL)
        halTimerState->callback(halTimerState->callbackParam);
}

void ctimer0_match0_callback(uint32_t flags)
{
    HAL_TimerInterruptHandle(0);
}

void ctimer1_match0_callback(uint32_t flags)
{
    HAL_TimerInterruptHandle(1);
}
ctimer_callback_t ctimer_callback_table[] = {ctimer0_match0_callback, ctimer1_match0_callback};

static hal_timer_status_t HAL_CTimerConfigTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout)
{
    CTIMER_Type *mCtimerBase[] = CTIMER_BASE_PTRS;
    ctimer_match_config_t mCtimerMatchConfig;
    ctimer_config_t config;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    halTimerState->timeout                   = timeout;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(mCtimerBase[halTimerState->instance], &config);
    CTIMER_StopTimer(mCtimerBase[halTimerState->instance]);

    /* Configuration 0 */
    mCtimerMatchConfig.enableCounterReset = true;
    mCtimerMatchConfig.enableCounterStop  = false;
    mCtimerMatchConfig.outControl         = kCTIMER_Output_NoAction;
    mCtimerMatchConfig.outPinInitState    = false;
    mCtimerMatchConfig.enableInterrupt    = true;
    mCtimerMatchConfig.matchValue         = USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz);
    if ((mCtimerMatchConfig.matchValue < 1) || (mCtimerMatchConfig.matchValue > 0xFFFFFFF0))
        return kStatus_HAL_TimerOutOfRanger;
    /* Configure channel to Software compare; output pin not used */
    CTIMER_RegisterCallBack(mCtimerBase[halTimerState->instance], &ctimer_callback_table[halTimerState->instance],
                            kCTIMER_SingleCallback);
    CTIMER_SetupMatch(mCtimerBase[halTimerState->instance], (ctimer_match_t)gStackTimerChannel_c, &mCtimerMatchConfig);
    return kStatus_HAL_TimerSuccess;
}

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
hal_timer_status_t HAL_TimerInit(hal_timer_handle_t halTimerHandle, hal_timer_config_t *halTimerConfig)
{
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    ctimer_config_t config;
    assert(sizeof(hal_timer_handle_struct_t) == HAL_TIMER_HANDLE_SIZE);
    assert(halTimerConfig);
    assert(halTimerHandle);
    if (halTimerConfig->instance >= gHalTimerInstanceNum_c)
    {
        return kStatus_HAL_TimerOutOfRanger;
    }
    CTIMER_GetDefaultConfig(&config);
    halTimerState->timeout                 = halTimerConfig->timeout;
    halTimerState->instance                = halTimerConfig->instance;
    halTimerState->timerClock_Hz           = halTimerConfig->srcClock_Hz / (config.prescale + 1);
    g_timerHandle[halTimerState->instance] = halTimerHandle;
    return HAL_CTimerConfigTimeout(halTimerHandle, halTimerState->timeout);
}
/*************************************************************************************/
void HAL_TimerDeinit(hal_timer_handle_t halTimerHandle)
{
    CTIMER_Type *mCtimerBase[] = CTIMER_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    g_timerHandle[halTimerState->instance]   = NULL;
    CTIMER_Deinit(mCtimerBase[halTimerState->instance]);
}
/*************************************************************************************/
void HAL_TimerEnable(hal_timer_handle_t halTimerHandle)
{
    CTIMER_Type *mCtimerBase[] = CTIMER_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    CTIMER_StartTimer(mCtimerBase[halTimerState->instance]);
}

/*************************************************************************************/
void HAL_TimerDisable(hal_timer_handle_t halTimerHandle)
{
    CTIMER_Type *mCtimerBase[] = CTIMER_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    CTIMER_StopTimer(mCtimerBase[halTimerState->instance]);
}

/*************************************************************************************/
void HAL_TimerInstallCallback(hal_timer_handle_t halTimerHandle, hal_timer_callback_t callback, void *callbackParam)
{
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    halTimerState->callback                  = callback;
    halTimerState->callbackParam             = callbackParam;
}

uint32_t HAL_TimerGetMaxTimeout(hal_timer_handle_t halTimerHandle)
{
    uint32_t reserveCount;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    reserveCount                             = MSEC_TO_COUNT(4, halTimerState->timerClock_Hz);
    if (reserveCount < MSEC_TO_COUNT(1, halTimerState->timerClock_Hz))
        return 1000;
    return COUNT_TO_USEC(0xFFFFFFFF - reserveCount, halTimerState->timerClock_Hz);
}
uint32_t HAL_TimerGetCurrentTimerCount(hal_timer_handle_t halTimerHandle)
{
    CTIMER_Type *mCtimerBase[] = CTIMER_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    return COUNT_TO_USEC(mCtimerBase[halTimerState->instance]->TC, halTimerState->timerClock_Hz);
}

hal_timer_status_t HAL_TimerUpdateTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout)
{
    return HAL_CTimerConfigTimeout(halTimerHandle, timeout);
}

void HAL_TimerExitLowpower(hal_timer_handle_t halTimerHandle)
{
    assert(halTimerHandle);
}

void HAL_TimerEnterLowpower(hal_timer_handle_t halTimerHandle)
{
    assert(halTimerHandle);
}
