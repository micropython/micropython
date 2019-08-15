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
#include "fsl_mrt.h"

#define gHalTimerInstanceNum_c (FSL_FEATURE_SOC_MRT_COUNT)
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
    MRT_Type *instanceList[] = MRT_BASE_PTRS;

    hal_timer_handle_struct_t *halTimerState = (hal_timer_handle_struct_t *)g_timerHandle[instance];
    /* Clear interrupt flag.*/
    MRT_ClearStatusFlags(instanceList[halTimerState->instance], kMRT_Channel_0, kMRT_TimerInterruptFlag);

    if (halTimerState->callback != NULL)
        halTimerState->callback(halTimerState->callbackParam);
}
void MRT0_IRQHandler(void)
{
    HAL_TimerInterruptHandle(0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
hal_timer_status_t HAL_TimerInit(hal_timer_handle_t halTimerHandle, hal_timer_config_t *halTimerConfig)
{
    MRT_Type *instanceList[]                 = MRT_BASE_PTRS;
    IRQn_Type instanceIrq[]                  = MRT_IRQS;
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;

    assert(sizeof(hal_timer_handle_struct_t) == HAL_TIMER_HANDLE_SIZE);
    assert(halTimerHandle);
    assert(halTimerConfig);
    if (halTimerConfig->instance >= gHalTimerInstanceNum_c)
    {
        return kStatus_HAL_TimerOutOfRanger;
    }
    halTimerState->timeout  = halTimerConfig->timeout;
    halTimerState->instance = halTimerConfig->instance;

    /* Structure of initialize MRT */
    mrt_config_t mrtConfig;

    /* mrtConfig.enableMultiTask = false; */
    MRT_GetDefaultConfig(&mrtConfig);

    /* Init mrt module */
    MRT_Init(instanceList[halTimerState->instance], &mrtConfig);

    /* Setup Channel 0 to be repeated */
    MRT_SetupChannelMode(instanceList[halTimerState->instance], kMRT_Channel_0, kMRT_RepeatMode);

    /* Enable timer interrupts for channel 0 */
    MRT_EnableInterrupts(instanceList[halTimerState->instance], kMRT_Channel_0, kMRT_TimerInterruptEnable);

    halTimerState->timerClock_Hz           = halTimerConfig->srcClock_Hz;
    g_timerHandle[halTimerState->instance] = halTimerHandle;
    /* Enable at the irq */
    NVIC_SetPriority((IRQn_Type)instanceIrq[halTimerState->instance], HAL_TIMER_ISR_PRIORITY);
    EnableIRQ(instanceIrq[halTimerState->instance]);
    return kStatus_HAL_TimerSuccess;
}

void HAL_TimerDeinit(hal_timer_handle_t halTimerHandle)
{
    MRT_Type *instanceList[] = MRT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    g_timerHandle[halTimerState->instance]   = NULL;
    MRT_Deinit(instanceList[halTimerState->instance]);
}
/*************************************************************************************/
void HAL_TimerEnable(hal_timer_handle_t halTimerHandle)
{
    MRT_Type *instanceList[] = MRT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    MRT_StartTimer(instanceList[halTimerState->instance], kMRT_Channel_0,
                   USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz));
}

/*************************************************************************************/
void HAL_TimerDisable(hal_timer_handle_t halTimerHandle)
{
    MRT_Type *instanceList[] = MRT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    /* Stop channel 0 */
    MRT_StopTimer(instanceList[halTimerState->instance], kMRT_Channel_0);
    /* Clear interrupt flag.*/
    MRT_ClearStatusFlags(instanceList[halTimerState->instance], kMRT_Channel_0, kMRT_TimerInterruptFlag);
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
    return COUNT_TO_USEC(0xFFFF - reserveCount, halTimerState->timerClock_Hz);
}
// return micro us
uint32_t HAL_TimerGetCurrentTimerCount(hal_timer_handle_t halTimerHandle)
{
    MRT_Type *instanceList[] = MRT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    return COUNT_TO_USEC(MRT_GetCurrentTimerCount(instanceList[halTimerState->instance], kMRT_Channel_0),
                         halTimerState->timerClock_Hz);
}

hal_timer_status_t HAL_TimerUpdateTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout)
{
    uint32_t tickCount;
    MRT_Type *instanceList[] = MRT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    halTimerState->timeout                   = timeout;
    tickCount                                = USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz);
    if ((tickCount < 1) || (tickCount > 0xfff0))
        return kStatus_HAL_TimerOutOfRanger;
    MRT_StopTimer(instanceList[halTimerState->instance], kMRT_Channel_0);
    return kStatus_HAL_TimerSuccess;
}
void HAL_TimerExitLowpower(hal_timer_handle_t halTimerHandle)
{
    assert(halTimerHandle);
}
void HAL_TimerEnterLowpower(hal_timer_handle_t halTimerHandle)
{
    assert(halTimerHandle);
}
