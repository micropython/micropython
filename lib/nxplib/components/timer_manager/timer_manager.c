/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "generic_list.h"
#include "timer.h"
#include "timer_manager.h"
/*
 * The OSA_USED macro can only be defined when the OSA component is used.
 * If the source code of the OSA component does not exist, the OSA_USED cannot be defined.
 * OR, If OSA component is not added into project event the OSA source code exists, the OSA_USED
 * also cannot be defined.
 * The source code path of the OSA component is <MCUXpresso_SDK>/components/osa.
 *
 */
#if defined(OSA_USED)
#include "fsl_os_abstraction.h"
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
#include "common_task.h"
#endif
#endif
#include "panic.h"

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/
#define mTmrDummyEvent_c (1 << 16)

/**@brief Timer status. */
typedef enum _timer_state
{
    kTimerStateFree_c     = 0x00, /**< The timer free status. */
    kTimerStateActive_c   = 0x20, /**< The timer active status. */
    kTimerStateReady_c    = 0x40, /**< The timer ready status. */
    kTimerStateInactive_c = 0x80, /**< The timer inactive status. */
    kTimerStateMask_c     = 0xE0, /**< The timer status mask all. */
    kTimerModeMask_c      = 0x1F, /**< The timer mode mask all. */
} timer_state_t;

/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/

/*! @brief Timer handle structure for timer manager. */
typedef struct _timer_handle_struct_t
{
    list_element_t link;               /*!< LIST_ element of the link */
    uint64_t timeoutInUs;              /*!< Time out of the timer, should be microseconds */
    uint64_t remainingUs;              /*!< Remaining of the timer, should be microseconds */
    timer_callback_t pfCallBack;       /*!< Callback function of the timer */
    void *param;                       /*!< Parameter of callback function of the timer */
    hal_timer_handle_t halTimerHandle; /*!< Handle of the hal timer */
    uint8_t tmrStatus;                 /*!< Timer status */
} timer_handle_struct_t;

/*! @brief State structure for timer manager. */
typedef struct _timermanager_state
{
    uint32_t mUsInTimerInterval;                   /*!< Timer intervl in microseconds */
    uint32_t previousTimeInUs;                     /*!< Previous timer count in microseconds */
    list_label_t timerHead;                        /*!< Timer list head */
    uint8_t halTimerHandle[HAL_TIMER_HANDLE_SIZE]; /*!< Timer handle buffer */
#if defined(OSA_USED)
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
    common_task_message_t mTimerCommontaskMsg; /*!< Timer common_task message */
#else
    uint8_t halTimerTaskEventHandle[OSA_EVENT_HANDLE_SIZE]; /*!< Timer task event handle buffer */
    uint8_t timerTaskHandle[OSA_TASK_HANDLE_SIZE];          /*!< Timer task id */
#endif
#endif
    volatile uint8_t numberOfActiveTimers;         /*!< Number of active Timers*/
    volatile uint8_t numberOfLowPowerActiveTimers; /*!< Number of low power active Timers */
    volatile uint8_t timerHardwareIsRunning;       /*!< Hardware timer is runnig */
} timermanager_state_t;

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
#if defined(OSA_USED)
extern const uint8_t gUseRtos_c;
#endif
/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/

/*! -------------------------------------------------------------------------
 * \brief Function called by driver ISR on channel match in interrupt context.
 *---------------------------------------------------------------------------*/
static void HAL_TIMER_Callback(void *param);

/*! -------------------------------------------------------------------------
 * \brief     Timer thread.
 *            Called by the kernel when the timer ISR posts a timer event.
 * \param[in] param - User parameter to timer thread; not used.
 *---------------------------------------------------------------------------*/
static void TimerManagerTask(void *param);

static void TimerEnable(timer_handle_t timerHandle);

/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/
static timermanager_state_t s_timermanager = {0};
/*****************************************************************************
******************************************************************************
* Private API macro define
******************************************************************************
*****************************************************************************/

#define IncrementActiveTimerNumber(type)                                                  \
    (((type)&kTimerModeLowPowerTimer) ? (++s_timermanager.numberOfLowPowerActiveTimers) : \
                                        (++s_timermanager.numberOfActiveTimers))
#define DecrementActiveTimerNumber(type)                                                  \
    (((type)&kTimerModeLowPowerTimer) ? (--s_timermanager.numberOfLowPowerActiveTimers) : \
                                        (--s_timermanager.numberOfActiveTimers))

/*
 * \brief Detect if the timer is a low-power timer
 */
#define IsLowPowerTimer(type) ((type)&kTimerModeLowPowerTimer)

#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))

#else
/*
 * \brief Defines the timer thread's stack
 */
OSA_TASK_DEFINE(TimerManagerTask, TM_TASK_PRIORITY, 1, TM_TASK_STACK_SIZE, false);

#endif

/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/
/*!-------------------------------------------------------------------------
 * \brief     Returns the timer status
 * \param[in] timerHandle - the handle of timer
 * \return    see definition of uint8_t
 *---------------------------------------------------------------------------*/
static uint8_t TimerGetTimerStatus(timer_handle_t timerHandle)
{
    timer_handle_struct_t *timer = (timer_handle_struct_t *)timerHandle;
    return timer->tmrStatus & (uint8_t)kTimerStateMask_c;
}

/*! -------------------------------------------------------------------------
 * \brief     Set the timer status
 * \param[in] timerHandle - the handle of timer
 * \param[in] status - the status of the timer
 *---------------------------------------------------------------------------*/
static void TimerSetTimerStatus(timer_handle_t timerHandle, uint8_t status)
{
    timer_handle_struct_t *timer = (timer_handle_struct_t *)timerHandle;
    timer->tmrStatus &= (uint8_t)(~kTimerStateMask_c);
    timer->tmrStatus |= status;
}

/*! -------------------------------------------------------------------------
 * \brief     Returns the timer type
 * \param[in] timerHandle - the handle of timer
 * \return    see definition of uint8_t
 *---------------------------------------------------------------------------*/
static uint8_t TimerGetTimerType(timer_handle_t timerHandle)
{
    timer_handle_struct_t *timer = (timer_handle_struct_t *)timerHandle;
    return timer->tmrStatus & (uint8_t)kTimerModeMask_c;
}

/*! -------------------------------------------------------------------------
 * \brief     Set the timer type
 * \param[in] timerHandle - the handle of timer
 * \param[in] type - timer type
 *---------------------------------------------------------------------------*/
static void TimerSetTimerType(timer_handle_t timerHandle, uint8_t type)
{
    timer_handle_struct_t *timer = (timer_handle_struct_t *)timerHandle;
    timer->tmrStatus &= (uint8_t)(~kTimerModeMask_c);
    timer->tmrStatus |= type;
}

/*! -------------------------------------------------------------------------
 * \brief     Set the timer free
 * \param[in] timerHandle - the handle of timer
 * \param[in] type - timer type
 *---------------------------------------------------------------------------*/
static void TimerMarkTimerFree(timer_handle_t timerHandle)
{
    timer_handle_struct_t *timer = (timer_handle_struct_t *)timerHandle;
    timer->tmrStatus             = 0;
}

/*! -------------------------------------------------------------------------
 * \brief  Notify Timer task to run.
 * \return
 *---------------------------------------------------------------------------*/
static void NotifyTimersTask(void)
{
#if defined(OSA_USED)
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
    s_timermanager.mTimerCommontaskMsg.callback = TimerManagerTask;
    COMMON_TASK_post_message(&s_timermanager.mTimerCommontaskMsg);
#else
    (void)OSA_EventSet((osa_event_handle_t)s_timermanager.halTimerTaskEventHandle, mTmrDummyEvent_c);
#endif
#else
    TimerManagerTask(NULL);
#endif
}

/*! -------------------------------------------------------------------------
 * \brief Function called by driver ISR on channel match in interrupt context.
 *---------------------------------------------------------------------------*/
static void HAL_TIMER_Callback(void *param)
{
    list_element_handle_t list_element;
    list_handle_t timerHandle = &s_timermanager.timerHead;
    assert(timerHandle);

    if (s_timermanager.numberOfLowPowerActiveTimers || s_timermanager.numberOfActiveTimers)
    {
        list_element = LIST_GetHead(timerHandle);
        while (list_element != NULL)
        {
            timer_handle_struct_t *th = (timer_handle_struct_t *)list_element;
            if ((timer_state_t)TimerGetTimerStatus(th) == kTimerStateActive_c)
            {
                if (th->remainingUs > s_timermanager.mUsInTimerInterval)
                    th->remainingUs = th->remainingUs - s_timermanager.mUsInTimerInterval;
                else
                    th->remainingUs = 0;
            }
            list_element = LIST_GetNext(list_element);
        }
    }
    s_timermanager.previousTimeInUs = HAL_TimerGetCurrentTimerCount((hal_timer_handle_t)s_timermanager.halTimerHandle);
    NotifyTimersTask();
}

/*! -------------------------------------------------------------------------
 * \brief     TimerManager task.
 *            Called by the kernel when the timer ISR posts a timer event.
 * \param[in] param
 *---------------------------------------------------------------------------*/
static void TimerManagerTask(void *param)
{
    uint8_t timerType;
    list_element_handle_t list_element;
    timer_state_t state;
    static uint32_t mpevUsInTimerInterval = 0;

    list_handle_t timerHandle = &s_timermanager.timerHead;
#if defined(OSA_USED)
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
    {
#else
    osa_event_flags_t ev = 0;
    do
    {
        if (KOSA_StatusSuccess == OSA_EventWait((osa_event_handle_t)s_timermanager.halTimerTaskEventHandle,
                                                osaEventFlagsAll_c, false, osaWaitForever_c, &ev))
        {
#endif
#endif

        uint32_t regPrimask = DisableGlobalIRQ();
        ;
        s_timermanager.mUsInTimerInterval = HAL_TimerGetMaxTimeout((hal_timer_handle_t)s_timermanager.halTimerHandle);
        list_element                      = LIST_GetHead(timerHandle);
        while (list_element != NULL)
        {
            timer_handle_struct_t *th = (timer_handle_struct_t *)list_element;
            timerType                 = TimerGetTimerType(th);
            state                     = (timer_state_t)TimerGetTimerStatus(th);
            if (state == kTimerStateReady_c)
            {
                TimerSetTimerStatus(th, kTimerStateActive_c);
                if (s_timermanager.mUsInTimerInterval > th->timeoutInUs)
                {
                    s_timermanager.mUsInTimerInterval = th->timeoutInUs;
                }
            }
            if (state == kTimerStateActive_c)
            {
                /* This timer is active. Decrement it's countdown.. */
                if (th->remainingUs <= 0)
                {
                    /* If this is an interval timer, restart it. Otherwise, mark it as inactive. */
                    if (timerType & (kTimerModeSingleShot | kTimerModeSetMinuteTimer | kTimerModeSetSecondTimer))
                    {
                        th->remainingUs = 0;
                        (void)TM_Stop(th);
                        state = (timer_state_t)TimerGetTimerStatus(th);
                    }
                    else
                    {
                        th->remainingUs = th->timeoutInUs;
                    }
                    /* This timer has expired. */
                    /*Call callback if it is not NULL*/
                    EnableGlobalIRQ(regPrimask);
                    ;
                    if (th->pfCallBack)
                    {
                        th->pfCallBack(th->param);
                    }
                    regPrimask = DisableGlobalIRQ();
                    ;
                }
                if ((state == kTimerStateActive_c) && (s_timermanager.mUsInTimerInterval > th->remainingUs))
                {
                    s_timermanager.mUsInTimerInterval = th->remainingUs;
                }
            }
            else
            {
                /* Ignore any timer that is not active. */
            }
            list_element = LIST_GetNext(list_element);
        }

        if (s_timermanager.numberOfActiveTimers || s_timermanager.numberOfLowPowerActiveTimers)
        {
            if (s_timermanager.mUsInTimerInterval != mpevUsInTimerInterval)
            {
                HAL_TimerDisable((hal_timer_handle_t)s_timermanager.halTimerHandle);
                HAL_TimerUpdateTimeout((hal_timer_handle_t)s_timermanager.halTimerHandle,
                                       s_timermanager.mUsInTimerInterval);
                HAL_TimerEnable((hal_timer_handle_t)s_timermanager.halTimerHandle);
                mpevUsInTimerInterval = s_timermanager.mUsInTimerInterval;
            }
            s_timermanager.timerHardwareIsRunning = true;
        }
        else
        {
            if (s_timermanager.timerHardwareIsRunning)
            {
                HAL_TimerDisable((hal_timer_handle_t)s_timermanager.halTimerHandle);
                s_timermanager.timerHardwareIsRunning = false;
                s_timermanager.mUsInTimerInterval     = 0;
            }
        }
        EnableGlobalIRQ(regPrimask);
        ;
#if defined(OSA_USED)
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
    }
#else
        }
    } while (gUseRtos_c);
#endif
#endif
}
/*! -------------------------------------------------------------------------
 * \brief     Enable the specified timer
 * \param[in] timerHandle - the handle of the timer
 *---------------------------------------------------------------------------*/
static void TimerEnable(timer_handle_t timerHandle)
{
    assert(timerHandle);
    uint32_t regPrimask = DisableGlobalIRQ();
    ;

    if (TimerGetTimerStatus(timerHandle) == kTimerStateInactive_c)
    {
        IncrementActiveTimerNumber(TimerGetTimerType(timerHandle));
        TimerSetTimerStatus(timerHandle, kTimerStateReady_c);
        NotifyTimersTask();
    }
    EnableGlobalIRQ(regPrimask);
    ;
}

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/
/*!
 * @brief Initializes timer manager module with the user configuration structure.
 *
 *
 * @param timerConfig              Pointer to user-defined timer configuration structure.
 * @retval kStatus_TimerSuccess      Timer manager initialization succeed.
 * @retval kStatus_TimerError      An error occurred.
 */
timer_status_t TM_Init(timer_config_t *timerConfig)
{
    static uint8_t initialized = false;
    hal_timer_config_t halTimerConfig;
    hal_timer_handle_t halTimerHandle = &s_timermanager.halTimerHandle[0];
    hal_timer_status_t status;
    assert(timerConfig);
    /* Check if TMR is already initialized */
    if (!initialized)
    {
        LIST_Init((&s_timermanager.timerHead), 0);
        halTimerConfig.timeout     = 1000;
        halTimerConfig.srcClock_Hz = timerConfig->srcClock_Hz;
        halTimerConfig.instance    = timerConfig->instance;
        status                     = HAL_TimerInit(halTimerHandle, &halTimerConfig);
        assert(kStatus_HAL_TimerSuccess == status);
        (void)status;

        HAL_TimerInstallCallback(halTimerHandle, HAL_TIMER_Callback, NULL);
        s_timermanager.mUsInTimerInterval = halTimerConfig.timeout;
#if defined(OSA_USED)
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
        COMMON_TASK_init();
#else
        if (KOSA_StatusSuccess != OSA_EventCreate((osa_event_handle_t)s_timermanager.halTimerTaskEventHandle, true))
        {
            panic(0, (uint32_t)TM_Init, 0, 0);
        }
        else
        {
            if (KOSA_StatusSuccess !=
                OSA_TaskCreate((osa_task_handle_t)s_timermanager.timerTaskHandle, OSA_TASK(TimerManagerTask), NULL))
            {
                panic(0, (uint32_t)TM_Init, 0, 0);
            }
        }
#endif
#endif
        initialized = true;
    }
    return kStatus_TimerSuccess;
}

/*!
 * @brief Deinitialize timer manager module.
 *
 */
void TM_Deinit()
{
#if (defined(TM_COMMON_TASK_ENABLE) && (TM_COMMON_TASK_ENABLE > 0U))
#else
    OSA_EventDestroy((osa_event_handle_t)s_timermanager.halTimerTaskEventHandle);
    OSA_TaskDestroy((osa_task_handle_t)s_timermanager.timerTaskHandle);

#endif
    HAL_TimerDeinit((hal_timer_handle_t)s_timermanager.halTimerHandle);
    memset(&s_timermanager, 0x0, sizeof(s_timermanager));
}

/*!
 * @brief Power up timer manager module.
 *
 */
void TM_ExitLowpower()
{
#if (defined(TM_ENABLE_LOW_POWER_TIMER) && (TM_ENABLE_LOW_POWER_TIMER > 0U))
    HAL_TimerExitLowpower((hal_timer_handle_t)s_timermanager.halTimerHandle);
#endif
}

/*!
 * @brief Power down timer manager module.
 *
 */
void TM_EnterLowpower()
{
#if (defined(TM_ENABLE_LOW_POWER_TIMER) && (TM_ENABLE_LOW_POWER_TIMER > 0U))
    HAL_TimerEnterLowpower((hal_timer_handle_t)s_timermanager.halTimerHandle);
#endif
}

/*!
 * @brief Open a timer with user handle.
 *
 * @param timerHandle              Pointer to point to a memory space of size #TIMER_HANDLE_SIZE allocated by the
 * caller.
 * @retval kStatus_TimerSuccess    Timer open succeed.
 * @retval kStatus_TimerError      An error occurred.
 */
timer_status_t TM_Open(timer_handle_t timerHandle)
{
    timer_handle_struct_t *timerState = timerHandle;
    assert(sizeof(timer_handle_struct_t) == TIMER_HANDLE_SIZE);
    assert(timerHandle);

    TimerSetTimerStatus(timerState, kTimerStateInactive_c);
    timerState->halTimerHandle = (hal_timer_handle_t)s_timermanager.halTimerHandle;
    LIST_AddTail(&s_timermanager.timerHead, (list_element_handle_t) & (timerState->link));
    return kStatus_TimerSuccess;
}

/*!
 * @brief Close a timer with user handle.
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval kStatus_TimerSuccess    Timer close succeed.
 * @retval kStatus_TimerError      An error occurred.
 */
timer_status_t TM_Close(timer_handle_t timerHandle)
{
    timer_status_t status;
    assert(timerHandle);

    status = TM_Stop(timerHandle);

    assert(kStatus_TimerSuccess == status);
    (void)status;

    TimerMarkTimerFree(timerHandle);
    LIST_RemoveElement(timerHandle);

    return kStatus_TimerSuccess;
}

/*!
 * @brief   Check if all timers except the LP timers are OFF
 *
 *
 * @retval return 1 there are no active non-low power timers, 0 otherwise.
 */

uint8_t TM_AreAllTimersOff(void)
{
    return !s_timermanager.numberOfActiveTimers;
}

/*!
 * @brief  Check if a specified timer is active
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval return 1 if timer is active, return 0 if timer is not active.
 */
uint8_t TM_IsTimerActive(timer_handle_t timerHandle)
{
    assert(timerHandle);
    return TimerGetTimerStatus(timerHandle) == kTimerStateActive_c;
}

/*!
 * @brief  Check if a specified timer is ready
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval return 1 if timer is ready, return 0 if timer is not ready.
 */
uint8_t TM_IsTimerReady(timer_handle_t timerHandle)
{
    assert(timerHandle);
    return TimerGetTimerStatus(timerHandle) == kTimerStateReady_c;
}

/*!
 * @brief  Install a specified timer callback
 *
 * @param timerHandle - the handle of the timer
 * @param callback - callback function
 * @param callbackParam - parameter to callback function
 *
 * @retval kStatus_TimerSuccess    Timer install callback succeed.
 *
 */
timer_status_t TM_InstallCallback(timer_handle_t timerHandle, timer_callback_t callback, void *callbackParam)
{
    timer_handle_struct_t *th = timerHandle;

    assert(timerHandle);
    th->pfCallBack = callback;
    th->param      = callbackParam;

    return kStatus_TimerSuccess;
}

/*!
 * @brief  Start a specified timer
 *
 * @param timerHandle - the handle of the timer
 * @param timerType - the type of the timer
 * @param timerTimout - time expressed in millisecond units
 *
 * @retval kStatus_TimerSuccess    Timer start succeed.
 * @retval kStatus_TimerError      An error occurred.
 */
timer_status_t TM_Start(timer_handle_t timerHandle, timer_mode_t timerType, uint32_t timerTimeout)
{
    timer_status_t status;
    timer_handle_struct_t *th = timerHandle;
    assert(timerHandle);
    /* Stopping an already stopped timer is harmless. */
    status = TM_Stop(timerHandle);
    assert(status == kStatus_TimerSuccess);

    TimerSetTimerType(timerHandle, timerType);

    if (timerType & kTimerModeSetMinuteTimer)
    {
        th->timeoutInUs = 1000 * 1000 * 60 * timerTimeout;
        th->remainingUs = 1000 * 1000 * 60 * timerTimeout;
    }
    else if (timerType & kTimerModeSetSecondTimer)
    {
        th->timeoutInUs = 1000 * 1000 * timerTimeout;
        th->remainingUs = 1000 * 1000 * timerTimeout;
    }
    else
    {
        th->timeoutInUs = 1000 * timerTimeout;
        th->remainingUs = 1000 * timerTimeout;
    }

    /* Enable timer, the timer task will do the rest of the work. */
    TimerEnable(timerHandle);

    return status;
}

/*!
 * @brief  Stop a specified timer
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval kStatus_TimerSuccess    Timer stop succeed.
 * @retval kStatus_TimerError      An error occurred.
 */
timer_status_t TM_Stop(timer_handle_t timerHandle)
{
    timer_status_t status             = kStatus_TimerSuccess;
    timer_handle_struct_t *timerState = timerHandle;
    timer_state_t state;

    /* check if timer is not allocated or if it has an invalid ID (fix@ENGR00323423) */
    if (timerHandle == NULL)
    {
        status = kStatus_TimerInvalidId;
    }
    else
    {
        uint32_t regPrimask = DisableGlobalIRQ();
        ;
        state = (timer_state_t)TimerGetTimerStatus(timerHandle);

        if ((state == kTimerStateActive_c) || (state == kTimerStateReady_c))
        {
            TimerSetTimerStatus(timerHandle, kTimerStateInactive_c);
            DecrementActiveTimerNumber(TimerGetTimerType(timerHandle));
            /* if no sw active timers are enabled, */
            /* call the TimerManagerTask() to countdown the ticks and stop the hw timer*/
            if ((!s_timermanager.numberOfActiveTimers) && (!s_timermanager.numberOfLowPowerActiveTimers))
            {
                if (s_timermanager.timerHardwareIsRunning)
                {
                    HAL_TimerDisable(timerState->halTimerHandle);
                    s_timermanager.timerHardwareIsRunning = false;
                }
            }
        }
        EnableGlobalIRQ(regPrimask);
        ;
    }

    return status;
}

/*!
 * @brief  Returns the remaining time until timeout
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval remaining time in microseconds until first timer timeouts.
 */
uint32_t TM_GetRemainingTime(timer_handle_t timerHandle)
{
    timer_handle_struct_t *timerState = timerHandle;
    assert(timerHandle);
    return (uint32_t)(timerState->remainingUs -
                      (HAL_TimerGetCurrentTimerCount((hal_timer_handle_t)s_timermanager.halTimerHandle) -
                       s_timermanager.previousTimeInUs));
}

/*!
 * @brief Get the first expire time of timer
 *
 * @param timerHandle - the handle of the timer
 *
 * @retval return the first expire time of all timer.
 */
uint32_t TM_GetFirstExpireTime(uint8_t timerType)
{
    uint32_t min = 0xFFFFFFFF;
    uint32_t remainingTime;
    list_element_handle_t list_element;
    list_handle_t timerHandle = &s_timermanager.timerHead;

    list_element = LIST_GetHead(timerHandle);
    while (list_element != NULL)
    {
        timer_handle_struct_t *th = (timer_handle_struct_t *)list_element;

        if (TM_IsTimerActive(th) && ((timerType & TimerGetTimerType(th)) > 0))
        {
            remainingTime = HAL_TimerGetCurrentTimerCount((hal_timer_handle_t)s_timermanager.halTimerHandle);
            if (remainingTime < min)
            {
                min = remainingTime;
            }
        }
        list_element = LIST_GetNext(list_element);
    }
    return min;
}

/*!
 * @brief Returns the handle of the timer of the first allocated timer that has the
 *        specified parameter.
 *
 * @param param - specified parameter of timer
 *
 * @retval return the handle of the timer if success.
 */
timer_handle_t TM_GetFirstTimerWithParam(void *param)
{
    list_element_handle_t list_element;
    list_handle_t timerHandle = &s_timermanager.timerHead;

    list_element = LIST_GetHead(timerHandle);
    while (list_element != NULL)
    {
        timer_handle_struct_t *th = (timer_handle_struct_t *)list_element;
        if (th->param == param)
        {
            return th;
        }
        list_element = LIST_GetNext(list_element);
    }
    return NULL;
}

/*!
 * @brief Returns not counted time before entering in sleep,This function is called
 *        by Low Power module;
 *
 * @retval return microseconds that wasn't counted before entering in sleep.
 */
uint32_t TM_NotCountedTimeBeforeSleep(void)
{
    uint32_t timeUs = 0;
#if (defined(TM_ENABLE_LOW_POWER_TIMER) && (TM_ENABLE_LOW_POWER_TIMER > 0U))
    uint32_t currentTimeInUs;

    if (s_timermanager.numberOfLowPowerActiveTimers)
    {
        currentTimeInUs = HAL_TimerGetCurrentTimerCount((hal_timer_handle_t)s_timermanager.halTimerHandle);
        HAL_TimerDisable((hal_timer_handle_t)s_timermanager.halTimerHandle);
        s_timermanager.timerHardwareIsRunning = false;

        /* The hw timer is stopped but keep s_timermanager.timerHardwareIsRunning = TRUE...*/
        /* The Lpm timers are considered as being in running mode, so that  */
        /* not to start the hw timer if a TMR event occurs (this shouldn't happen) */

        timeUs = (uint32_t)(currentTimeInUs - s_timermanager.previousTimeInUs);
        return timeUs;
    }
    return 0;
#endif
}

/*!
 * @brief Sync low power timer in sleep mode,This function is called by Low Power module;
 *
 * @param sleepDurationTmrUs - sleep duration in TMR microseconds
 *
 */
void TM_SyncLpmTimers(uint32_t sleepDurationTmrUs)
{
#if (defined(TM_ENABLE_LOW_POWER_TIMER) && (TM_ENABLE_LOW_POWER_TIMER > 0U))
    list_element_handle_t list_element;
    list_handle_t timerHandle = &s_timermanager.timerHead;
    uint8_t timerType;
    timer_state_t state;

    /* Check if there are low power active timer */
    if (s_timermanager.numberOfLowPowerActiveTimers)
    {
        list_element = LIST_GetHead(timerHandle);
        while (list_element != NULL)
        {
            timer_handle_struct_t *th = (timer_handle_struct_t *)list_element;
            state                     = (timer_state_t)TimerGetTimerStatus(th);
            timerType                 = TimerGetTimerType(th);
            if ((state == kTimerStateActive_c) && (IsLowPowerTimer(timerType)))
            {
                /* Timer expired when MCU was in sleep mode??? */
                if (th->remainingUs > sleepDurationTmrUs)
                {
                    th->remainingUs -= sleepDurationTmrUs;
                }
                else
                {
                    th->remainingUs = 0;
                }
            }
            list_element = LIST_GetNext(list_element);
        }

        HAL_TimerEnable((hal_timer_handle_t)s_timermanager.halTimerHandle);
        s_timermanager.previousTimeInUs =
            HAL_TimerGetCurrentTimerCount((hal_timer_handle_t)s_timermanager.halTimerHandle);
    }

#else
    sleepDurationTmrUs = sleepDurationTmrUs;
#endif /* #if (TM_ENABLE_LOW_POWER_TIMER) */
}

/*!
 * @brief Make timer task ready after wakeup from lowpower mode,This function is called
 *        by Low Power module;
 *
 */
void TM_MakeTimerTaskReady(void)
{
#if (defined(TM_ENABLE_LOW_POWER_TIMER) && (TM_ENABLE_LOW_POWER_TIMER > 0U))
    NotifyTimersTask();
#endif
}
