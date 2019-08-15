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
#include "fsl_gpt.h"

#define gHalTimerInstanceNum_c (FSL_FEATURE_SOC_GPT_COUNT)
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
    GPT_Type *mGPTBase[]                     = GPT_BASE_PTRS;
    hal_timer_handle_struct_t *halTimerState = (hal_timer_handle_struct_t *)g_timerHandle[instance];

    GPT_ClearStatusFlags(mGPTBase[halTimerState->instance], kGPT_OutputCompare1Flag);
    if (halTimerState->callback != NULL)
        halTimerState->callback(halTimerState->callbackParam);
}

void GPT0_IRQHandler(void)
{
    HAL_TimerInterruptHandle(0);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPT1_IRQHandler(void)
{
    HAL_TimerInterruptHandle(1);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPT2_IRQHandler(void)
{
    HAL_TimerInterruptHandle(2);
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
    gpt_config_t gptConfig;
    GPT_Type *instanceList[] = GPT_BASE_PTRS;
    IRQn_Type instanceIrq[]  = GPT_IRQS;
    IRQn_Type irqId;
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;

    assert(sizeof(hal_timer_handle_struct_t) == HAL_TIMER_HANDLE_SIZE);
    assert(halTimerConfig);
    assert(halTimerHandle);
    if (halTimerConfig->instance >= gHalTimerInstanceNum_c)
    {
        return kStatus_HAL_TimerOutOfRanger;
    }
    halTimerState->timeout       = halTimerConfig->timeout;
    halTimerState->instance      = halTimerConfig->instance;
    halTimerState->timerClock_Hz = halTimerConfig->srcClock_Hz;
    irqId                        = instanceIrq[halTimerState->instance];
    GPT_GetDefaultConfig(&gptConfig);

    /* Initialize GPT module */
    GPT_Init(instanceList[halTimerState->instance], &gptConfig);
    /* Set both GPT modules to 1 second duration */
    GPT_SetOutputCompareValue(instanceList[halTimerState->instance], kGPT_OutputCompare_Channel1,
                              USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz));
    /* Enable GPT Output Compare1 interrupt */
    GPT_EnableInterrupts(instanceList[halTimerState->instance], kGPT_OutputCompare1InterruptEnable);

    g_timerHandle[halTimerState->instance] = halTimerHandle;
    NVIC_SetPriority((IRQn_Type)irqId, HAL_TIMER_ISR_PRIORITY);
    EnableIRQ(irqId);
    return kStatus_HAL_TimerSuccess;
}

/*************************************************************************************/
void HAL_TimerDeinit(hal_timer_handle_t halTimerHandle)
{
    GPT_Type *mGPTBase[] = GPT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    g_timerHandle[halTimerState->instance]   = NULL;
    GPT_Deinit(mGPTBase[halTimerState->instance]);
}

/*************************************************************************************/
void HAL_TimerEnable(hal_timer_handle_t halTimerHandle)
{
    GPT_Type *mGPTBase[] = GPT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    GPT_StartTimer(mGPTBase[halTimerState->instance]);
}

/*************************************************************************************/
void HAL_TimerDisable(hal_timer_handle_t halTimerHandle)
{
    GPT_Type *mGPTBase[] = GPT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    GPT_StopTimer(mGPTBase[halTimerState->instance]);
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
    GPT_Type *mGPTBase[] = GPT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    return COUNT_TO_USEC(GPT_GetCurrentTimerCount(mGPTBase[halTimerState->instance]), halTimerState->timerClock_Hz);
}

hal_timer_status_t HAL_TimerUpdateTimeout(hal_timer_handle_t halTimerHandle, uint32_t timeout)
{
    uint32_t tickCount;
    GPT_Type *mGPTBase[] = GPT_BASE_PTRS;
    assert(halTimerHandle);
    hal_timer_handle_struct_t *halTimerState = halTimerHandle;
    halTimerState->timeout                   = timeout;
    tickCount                                = USEC_TO_COUNT(halTimerState->timeout, halTimerState->timerClock_Hz);
    if ((tickCount < 1) || (tickCount > 0xfff0))
        return kStatus_HAL_TimerOutOfRanger;
    GPT_SetOutputCompareValue(mGPTBase[halTimerState->instance], kGPT_OutputCompare_Channel1, tickCount);
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
