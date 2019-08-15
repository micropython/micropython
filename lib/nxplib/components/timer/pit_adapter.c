/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_device_registers.h"
#include "timer.h"
#include "fsl_pit.h"

#define gHalTimerInstanceNum_c FSL_FEATURE_SOC_PIT_COUNT

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
    PIT_Type *instanceList[]                 = PIT_BASE_PTRS;
    hal_timer_handle_struct_t *halTimerState = (hal_timer_handle_struct_t *)g_timerHandle[instance];

    PIT_ClearStatusFlags(instanceList[halTimerState->instance], kPIT_Chnl_0, PIT_TFLG_TIF_MASK);
    if (halTimerState->callback != NULL)
        halTimerState->callback(halTimerState->callbackParam);
}

#if (defined(FSL_FEATURE_SOC_PIT_COUNT) && (FSL_FEATURE_SOC_PIT_COUNT > 0U))
void PIT_IRQHandler(void)
{
    HAL_TimerInterruptHandle(0);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void PIT0_IRQHandler(void)
{
    HAL_TimerInterruptHandle(0);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* FSL_FEATURE_SOC_PIT_COUNT */

#if (defined(FSL_FEATURE_SOC_PIT_COUNT) && (FSL_FEATURE_SOC_PIT_COUNT > 1U))
void PIT1_IRQHandler(void)
{
    HAL_TimerInterruptHandle(1);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* FSL_FEATURE_SOC_PIT_COUNT */

#if (defined(FSL_FEATURE_SOC_PIT_COUNT) && (FSL_FEATURE_SOC_PIT_COUNT > 2U))
void PIT2_IRQHandler(void)
{
    HAL_TimerInterruptHandle(2);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* FSL_FEATURE_SOC_PIT_COUNT */

#if (defined(FSL_FEATURE_SOC_PIT_COUNT) && (FSL_FEATURE_SOC_PIT_COUNT > 3U))
void PIT3_IRQHandler(void)
{
    HAL_TimerInterruptHandle(3);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif /* FSL_FEATURE_SOC_PIT_COUNT */
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
hal_timer_status_t HAL_TimerInit(hal_timer_handle_t halTimerHandle, hal_timer_config_t *halTimerConfig)
{
    PIT_Type *instanceList[]                             = PIT_BASE_PTRS;
    IRQn_Type instanceIrq[][FSL_FEATURE_PIT_TIMER_COUNT] = PIT_IRQS;
    IRQn_Type irqId;
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    /* Structure of initialize PIT */
    pit_config_t pitConfig;

    assert(sizeof(hal_timer_handle_struct_t) == HAL_TIMER_HANDLE_SIZE);
    assert(halTimerConfig);
    assert(halTimerHandle);
    if (halTimerConfig->instance >= gHalTimerInstanceNum_c)
    {
        return kStatus_HAL_TimerOutOfRanger;
    }
    halTimerState->timeout  = halTimerConfig->timeout;
    halTimerState->instance = halTimerConfig->instance;
    irqId                   = instanceIrq[0][halTimerState->instance];
    /*
     * pitConfig.enableRunInDebug = false;
     */
    PIT_GetDefaultConfig(&pitConfig);
    assert(instanceList[halTimerState->instance]);
    /* Init pit module */
    PIT_Init(instanceList[halTimerState->instance], &pitConfig);
    halTimerState->timerClock_Hz = halTimerConfig->srcClock_Hz;
    /* Set timer period for channel 0 */
    PIT_SetTimerPeriod(instanceList[halTimerState->instance], kPIT_Chnl_0,
                       USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz));
    /* Enable timer interrupts for channel 0 */
    PIT_EnableInterrupts(instanceList[halTimerState->instance], kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    g_timerHandle[halTimerState->instance] = halTimerHandle;

    NVIC_SetPriority((IRQn_Type)irqId, HAL_TIMER_ISR_PRIORITY);
    EnableIRQ(irqId);
    return kStatus_HAL_TimerSuccess;
}

/*************************************************************************************/
void HAL_TimerDeinit(hal_timer_handle_t halTimerHandle)
{
    PIT_Type *instanceList[] = PIT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    g_timerHandle[halTimerState->instance]   = NULL;
    PIT_Deinit(instanceList[halTimerState->instance]);
}
/*************************************************************************************/
void HAL_TimerEnable(hal_timer_handle_t halTimerHandle)
{
    PIT_Type *instanceList[] = PIT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    PIT_StartTimer(instanceList[halTimerState->instance], kPIT_Chnl_0);
}

/*************************************************************************************/
void HAL_TimerDisable(hal_timer_handle_t halTimerHandle)
{
    PIT_Type *instanceList[] = PIT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    PIT_StopTimer(instanceList[halTimerState->instance], kPIT_Chnl_0);
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

// return micro us
uint32_t HAL_TimerGetCurrentTimerCount(hal_timer_handle_t halTimerHandle)
{
    PIT_Type *instanceList[] = PIT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    return COUNT_TO_USEC(PIT_GetCurrentTimerCount(instanceList[halTimerState->instance], kPIT_Chnl_0),
                         halTimerState->timerClock_Hz);
}

hal_timer_status_t HAL_TimerUpdateTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout)
{
    uint32_t tickCount;
    PIT_Type *instanceList[] = PIT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    halTimerState->timeout                   = timeout;
    tickCount                                = USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz);
    if ((tickCount < 1) || (tickCount > 0xfffffff0))
        return kStatus_HAL_TimerOutOfRanger;
    PIT_SetTimerPeriod(instanceList[halTimerState->instance], kPIT_Chnl_0, tickCount);
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
