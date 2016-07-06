/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    templates/chconf.h
 * @brief   Configuration file template.
 * @details A copy of this file must be placed in each project directory, it
 *          contains the application specific kernel settings.
 *
 * @addtogroup config
 * @details Kernel related settings and hooks.
 * @{
 */

#ifndef _CHCONF_H_
#define _CHCONF_H_

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System tick frequency.
 * @details Frequency of the system timer that drives the system ticks. This
 *          setting also defines the system tick time unit.
 */
#if !defined(CH_FREQUENCY) || defined(__DOXYGEN__)
#define CH_FREQUENCY                    1000
#endif

/**
 * @brief   Round robin interval.
 * @details This constant is the number of system ticks allowed for the
 *          threads before preemption occurs. Setting this value to zero
 *          disables the preemption for threads with equal priority and the
 *          round robin becomes cooperative. Note that higher priority
 *          threads can still preempt, the kernel is always preemptive.
 *
 * @note    Disabling the round robin preemption makes the kernel more compact
 *          and generally faster.
 */
#if !defined(CH_TIME_QUANTUM) || defined(__DOXYGEN__)
#define CH_TIME_QUANTUM                 20
#endif

/**
 * @brief   Managed RAM size.
 * @details Size of the RAM area to be managed by the OS. If set to zero
 *          then the whole available RAM is used. The core memory is made
 *          available to the heap allocator and/or can be used directly through
 *          the simplified core memory allocator.
 *
 * @note    In order to let the OS manage the whole RAM the linker script must
 *          provide the @p __heap_base__ and @p __heap_end__ symbols.
 * @note    Requires @p CH_USE_MEMCORE.
 */
#if !defined(CH_MEMCORE_SIZE) || defined(__DOXYGEN__)
#define CH_MEMCORE_SIZE                 0
#endif

/**
 * @brief   Idle thread automatic spawn suppression.
 * @details When this option is activated the function @p chSysInit()
 *          does not spawn the idle thread automatically. The application has
 *          then the responsibility to do one of the following:
 *          - Spawn a custom idle thread at priority @p IDLEPRIO.
 *          - Change the main() thread priority to @p IDLEPRIO then enter
 *            an endless loop. In this scenario the @p main() thread acts as
 *            the idle thread.
 *          .
 * @note    Unless an idle thread is spawned the @p main() thread must not
 *          enter a sleep state.
 */
#if !defined(CH_NO_IDLE_THREAD) || defined(__DOXYGEN__)
#define CH_NO_IDLE_THREAD               FALSE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Performance options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   OS optimization.
 * @details If enabled then time efficient rather than space efficient code
 *          is used when two possible implementations exist.
 *
 * @note    This is not related to the compiler optimization options.
 * @note    The default is @p TRUE.
 */
#if !defined(CH_OPTIMIZE_SPEED) || defined(__DOXYGEN__)
#define CH_OPTIMIZE_SPEED               TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Subsystem options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Threads registry APIs.
 * @details If enabled then the registry APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_REGISTRY) || defined(__DOXYGEN__)
#define CH_USE_REGISTRY                 TRUE
#endif

/**
 * @brief   Threads synchronization APIs.
 * @details If enabled then the @p chThdWait() function is included in
 *          the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_WAITEXIT) || defined(__DOXYGEN__)
#define CH_USE_WAITEXIT                 TRUE
#endif

/**
 * @brief   Semaphores APIs.
 * @details If enabled then the Semaphores APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_SEMAPHORES) || defined(__DOXYGEN__)
#define CH_USE_SEMAPHORES               TRUE
#endif

/**
 * @brief   Semaphores queuing mode.
 * @details If enabled then the threads are enqueued on semaphores by
 *          priority rather than in FIFO order.
 *
 * @note    The default is @p FALSE. Enable this if you have special requirements.
 * @note    Requires @p CH_USE_SEMAPHORES.
 */
#if !defined(CH_USE_SEMAPHORES_PRIORITY) || defined(__DOXYGEN__)
#define CH_USE_SEMAPHORES_PRIORITY      FALSE
#endif

/**
 * @brief   Atomic semaphore API.
 * @details If enabled then the semaphores the @p chSemSignalWait() API
 *          is included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_SEMAPHORES.
 */
#if !defined(CH_USE_SEMSW) || defined(__DOXYGEN__)
#define CH_USE_SEMSW                    TRUE
#endif

/**
 * @brief   Mutexes APIs.
 * @details If enabled then the mutexes APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_MUTEXES) || defined(__DOXYGEN__)
#define CH_USE_MUTEXES                  TRUE
#endif

/**
 * @brief   Conditional Variables APIs.
 * @details If enabled then the conditional variables APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_MUTEXES.
 */
#if !defined(CH_USE_CONDVARS) || defined(__DOXYGEN__)
#define CH_USE_CONDVARS                 TRUE
#endif

/**
 * @brief   Conditional Variables APIs with timeout.
 * @details If enabled then the conditional variables APIs with timeout
 *          specification are included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_CONDVARS.
 */
#if !defined(CH_USE_CONDVARS_TIMEOUT) || defined(__DOXYGEN__)
#define CH_USE_CONDVARS_TIMEOUT         TRUE
#endif

/**
 * @brief   Events Flags APIs.
 * @details If enabled then the event flags APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_EVENTS) || defined(__DOXYGEN__)
#define CH_USE_EVENTS                   TRUE
#endif

/**
 * @brief   Events Flags APIs with timeout.
 * @details If enabled then the events APIs with timeout specification
 *          are included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_EVENTS.
 */
#if !defined(CH_USE_EVENTS_TIMEOUT) || defined(__DOXYGEN__)
#define CH_USE_EVENTS_TIMEOUT           TRUE
#endif

/**
 * @brief   Synchronous Messages APIs.
 * @details If enabled then the synchronous messages APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_MESSAGES) || defined(__DOXYGEN__)
#define CH_USE_MESSAGES                 TRUE
#endif

/**
 * @brief   Synchronous Messages queuing mode.
 * @details If enabled then messages are served by priority rather than in
 *          FIFO order.
 *
 * @note    The default is @p FALSE. Enable this if you have special requirements.
 * @note    Requires @p CH_USE_MESSAGES.
 */
#if !defined(CH_USE_MESSAGES_PRIORITY) || defined(__DOXYGEN__)
#define CH_USE_MESSAGES_PRIORITY        FALSE
#endif

/**
 * @brief   Mailboxes APIs.
 * @details If enabled then the asynchronous messages (mailboxes) APIs are
 *          included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_SEMAPHORES.
 */
#if !defined(CH_USE_MAILBOXES) || defined(__DOXYGEN__)
#define CH_USE_MAILBOXES                TRUE
#endif

/**
 * @brief   I/O Queues APIs.
 * @details If enabled then the I/O queues APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_QUEUES) || defined(__DOXYGEN__)
#define CH_USE_QUEUES                   TRUE
#endif

/**
 * @brief   Core Memory Manager APIs.
 * @details If enabled then the core memory manager APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_MEMCORE) || defined(__DOXYGEN__)
#define CH_USE_MEMCORE                  TRUE
#endif

/**
 * @brief   Heap Allocator APIs.
 * @details If enabled then the memory heap allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_MEMCORE and either @p CH_USE_MUTEXES or
 *          @p CH_USE_SEMAPHORES.
 * @note    Mutexes are recommended.
 */
#if !defined(CH_USE_HEAP) || defined(__DOXYGEN__)
#define CH_USE_HEAP                     TRUE
#endif

/**
 * @brief   C-runtime allocator.
 * @details If enabled the the heap allocator APIs just wrap the C-runtime
 *          @p malloc() and @p free() functions.
 *
 * @note    The default is @p FALSE.
 * @note    Requires @p CH_USE_HEAP.
 * @note    The C-runtime may or may not require @p CH_USE_MEMCORE, see the
 *          appropriate documentation.
 */
#if !defined(CH_USE_MALLOC_HEAP) || defined(__DOXYGEN__)
#define CH_USE_MALLOC_HEAP              FALSE
#endif

/**
 * @brief   Memory Pools Allocator APIs.
 * @details If enabled then the memory pools allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_USE_MEMPOOLS) || defined(__DOXYGEN__)
#define CH_USE_MEMPOOLS                 TRUE
#endif

/**
 * @brief   Dynamic Threads APIs.
 * @details If enabled then the dynamic threads creation APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_USE_WAITEXIT.
 * @note    Requires @p CH_USE_HEAP and/or @p CH_USE_MEMPOOLS.
 */
#if !defined(CH_USE_DYNAMIC) || defined(__DOXYGEN__)
#define CH_USE_DYNAMIC                  TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Debug options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Debug option, system state check.
 * @details If enabled the correct call protocol for system APIs is checked
 *          at runtime.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_SYSTEM_STATE_CHECK) || defined(__DOXYGEN__)
#define CH_DBG_SYSTEM_STATE_CHECK       FALSE
#endif

/**
 * @brief   Debug option, parameters checks.
 * @details If enabled then the checks on the API functions input
 *          parameters are activated.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_CHECKS) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_CHECKS            FALSE
#endif

/**
 * @brief   Debug option, consistency checks.
 * @details If enabled then all the assertions in the kernel code are
 *          activated. This includes consistency checks inside the kernel,
 *          runtime anomalies and port-defined checks.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_ASSERTS) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_ASSERTS           FALSE
#endif

/**
 * @brief   Debug option, trace buffer.
 * @details If enabled then the context switch circular trace buffer is
 *          activated.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_TRACE) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_TRACE             FALSE
#endif

/**
 * @brief   Debug option, stack checks.
 * @details If enabled then a runtime stack check is performed.
 *
 * @note    The default is @p FALSE.
 * @note    The stack check is performed in a architecture/port dependent way.
 *          It may not be implemented or some ports.
 * @note    The default failure mode is to halt the system with the global
 *          @p panic_msg variable set to @p NULL.
 */
#if !defined(CH_DBG_ENABLE_STACK_CHECK) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_STACK_CHECK       FALSE
#endif

/**
 * @brief   Debug option, stacks initialization.
 * @details If enabled then the threads working area is filled with a byte
 *          value when a thread is created. This can be useful for the
 *          runtime measurement of the used stack.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_FILL_THREADS) || defined(__DOXYGEN__)
#define CH_DBG_FILL_THREADS             FALSE
#endif

/**
 * @brief   Debug option, threads profiling.
 * @details If enabled then a field is added to the @p Thread structure that
 *          counts the system ticks occurred while executing the thread.
 *
 * @note    The default is @p TRUE.
 * @note    This debug option is defaulted to TRUE because it is required by
 *          some test cases into the test suite.
 */
#if !defined(CH_DBG_THREADS_PROFILING) || defined(__DOXYGEN__)
#define CH_DBG_THREADS_PROFILING        TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Threads descriptor structure extension.
 * @details User fields added to the end of the @p Thread structure.
 */
#if !defined(THREAD_EXT_FIELDS) || defined(__DOXYGEN__)
#define THREAD_EXT_FIELDS                                                   \
  /* Add threads custom fields here.*/
#endif

/**
 * @brief   Threads initialization hook.
 * @details User initialization code added to the @p chThdInit() API.
 *
 * @note    It is invoked from within @p chThdInit() and implicitly from all
 *          the threads creation APIs.
 */
#if !defined(THREAD_EXT_INIT_HOOK) || defined(__DOXYGEN__)
#define THREAD_EXT_INIT_HOOK(tp) {                                          \
  /* Add threads initialization code here.*/                                \
}
#endif

/**
 * @brief   Threads finalization hook.
 * @details User finalization code added to the @p chThdExit() API.
 *
 * @note    It is inserted into lock zone.
 * @note    It is also invoked when the threads simply return in order to
 *          terminate.
 */
#if !defined(THREAD_EXT_EXIT_HOOK) || defined(__DOXYGEN__)
#define THREAD_EXT_EXIT_HOOK(tp) {                                          \
  /* Add threads finalization code here.*/                                  \
}
#endif

/**
 * @brief   Context switch hook.
 * @details This hook is invoked just before switching between threads.
 */
#if !defined(THREAD_CONTEXT_SWITCH_HOOK) || defined(__DOXYGEN__)
#define THREAD_CONTEXT_SWITCH_HOOK(ntp, otp) {                              \
  /* System halt code here.*/                                               \
}
#endif

/**
 * @brief   Idle Loop hook.
 * @details This hook is continuously invoked by the idle thread loop.
 */
#if !defined(IDLE_LOOP_HOOK) || defined(__DOXYGEN__)
#define IDLE_LOOP_HOOK() {                                                  \
  /* Idle loop code here.*/                                                 \
}
#endif

/**
 * @brief   System tick event hook.
 * @details This hook is invoked in the system tick handler immediately
 *          after processing the virtual timers queue.
 */
#if !defined(SYSTEM_TICK_EVENT_HOOK) || defined(__DOXYGEN__)
#define SYSTEM_TICK_EVENT_HOOK() {                                          \
  /* System tick event code here.*/                                         \
}
#endif

/**
 * @brief   System halt hook.
 * @details This hook is invoked in case to a system halting error before
 *          the system is halted.
 */
#if !defined(SYSTEM_HALT_HOOK) || defined(__DOXYGEN__)
#define SYSTEM_HALT_HOOK() {                                                \
  /* System halt code here.*/                                               \
}
#endif

/** @} */

/*===========================================================================*/
/* Port-specific settings (override port settings defaulted in chcore.h).    */
/*===========================================================================*/

/* NOTE: When changing this option you also have to enable or disable the FPU
   in the project options.*/
#define CORTEX_USE_FPU                  FALSE

#endif  /* _CHCONF_H_ */

/** @} */
