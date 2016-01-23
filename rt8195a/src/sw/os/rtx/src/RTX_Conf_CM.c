/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_Conf_CM.C
 *      Purpose: Configuration of CMSIS RTX Kernel for Cortex-M
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"


/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Thread Configuration
// =======================
//
//   <o>Number of concurrent running threads <0-250>
//   <i> Defines max. number of threads that will run at the same time.
//       counting "main", but not counting "osTimerThread"
//   <i> Default: 6
#ifndef OS_TASKCNT
#if 0
#  if   defined(TARGET_LPC1768) || defined(TARGET_LPC2368)   || defined(TARGET_LPC4088) || defined(TARGET_LPC4337) || defined(TARGET_LPC1347) || defined(TARGET_K64F) || defined(TARGET_STM32F401RE)\
	 || defined(TARGET_KL46Z) || defined(TARGET_KL43Z)  || defined(TARGET_STM32F407) || defined(TARGET_F407VG)  || defined(TARGET_STM32F303VC) || defined(TARGET_LPC1549) || defined(TARGET_LPC11U68) || defined(TARGET_NRF51822) || defined(TARGET_STM32F411RE) || defined(TARGET_RTL8195A)
#    define OS_TASKCNT         14
#  elif defined(TARGET_LPC11U24) || defined(TARGET_LPC11U35_401)  || defined(TARGET_LPC11U35_501) || defined(TARGET_LPCCAPPUCCINO) || defined(TARGET_LPC1114) \
	 || defined(TARGET_LPC812)   || defined(TARGET_KL25Z)         || defined(TARGET_KL05Z)        || defined(TARGET_STM32F100RB)  || defined(TARGET_STM32F051R8) \
	 || defined(TARGET_STM32F103RB) || defined(TARGET_LPC824) || defined(TARGET_STM32F302R8) || defined(TARGET_STM32F334R8) || defined(TARGET_STM32F334C8)
#    define OS_TASKCNT         6
#  else
#    error "no target defined"
#  endif

#else // if 0

#define OS_TASKCNT         14

#endif
#endif

//   <o>Scheduler (+ interrupts) stack size [bytes] <64-4096:8><#/4>
#ifndef OS_SCHEDULERSTKSIZE
#if 0

#  if   defined(TARGET_LPC1768) || defined(TARGET_LPC2368)   || defined(TARGET_LPC4088) || defined(TARGET_LPC4337) || defined(TARGET_LPC1347)  || defined(TARGET_K64F) || defined(TARGET_STM32F401RE)\
	 || defined(TARGET_KL46Z) || defined(TARGET_KL43Z) || defined(TARGET_STM32F407) || defined(TARGET_F407VG)  || defined(TARGET_STM32F303VC) || defined(TARGET_LPC1549) || defined(TARGET_LPC11U68) || defined(TARGET_NRF51822) || defined(TARGET_STM32F411RE) || defined(TARGET_RTL8195A)
#      define OS_SCHEDULERSTKSIZE    256
#  elif defined(TARGET_LPC11U24) || defined(TARGET_LPC11U35_401)  || defined(TARGET_LPC11U35_501) || defined(TARGET_LPCCAPPUCCINO)  || defined(TARGET_LPC1114) \
	 || defined(TARGET_LPC812)   || defined(TARGET_KL25Z)         || defined(TARGET_KL05Z)        || defined(TARGET_STM32F100RB)  || defined(TARGET_STM32F051R8) \
	 || defined(TARGET_STM32F103RB) || defined(TARGET_LPC824) || defined(TARGET_STM32F302R8) || defined(TARGET_STM32F334R8) || defined(TARGET_STM32F334C8)
#      define OS_SCHEDULERSTKSIZE    128
#  else
#    error "no target defined"
#  endif

#else // if 0

#define OS_SCHEDULERSTKSIZE    256

#endif

#endif

//   <o>Idle stack size [bytes] <64-4096:8><#/4>
//   <i> Defines default stack size for the Idle thread.
#ifndef OS_IDLESTKSIZE
 #define OS_IDLESTKSIZE         128
#endif

//   <o>Timer Thread stack size [bytes] <64-4096:8><#/4>
//   <i> Defines stack size for Timer thread.
//   <i> Default: 200
#ifndef OS_TIMERSTKSZ
 #define OS_TIMERSTKSZ  WORDS_STACK_SIZE
#endif

// <q>Check for stack overflow
// <i> Includes the stack checking code for stack overflow.
// <i> Note that additional code reduces the Kernel performance.
#ifndef OS_STKCHECK
 #define OS_STKCHECK    1
#endif

// <o>Processor mode for thread execution
//   <0=> Unprivileged mode
//   <1=> Privileged mode
// <i> Default: Privileged mode
#ifndef OS_RUNPRIV
 #define OS_RUNPRIV     1
#endif

// </h>
// <h>SysTick Timer Configuration
// ==============================
//
//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Defines the timer clock value.
//   <i> Default: 6000000  (6MHz)
#ifndef OS_CLOCK

#if 0

#  if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
#    define OS_CLOCK       96000000

#  elif defined(TARGET_LPC1347) || defined(TARGET_STM32F303VC) || defined(TARGET_LPC1549) || defined(TARGET_STM32F302R8) || defined(TARGET_STM32F334R8) || defined(TARGET_STM32F334C8)
#    define OS_CLOCK       72000000

#  elif defined(TARGET_LPC11U24) || defined(TARGET_LPC11U35_401)  || defined(TARGET_LPC11U35_501) || defined(TARGET_LPCCAPPUCCINO)  || defined(TARGET_LPC1114) || defined(TARGET_KL25Z) || defined(TARGET_KL05Z) || defined(TARGET_KL46Z) || defined(TARGET_KL43Z) || defined(TARGET_STM32F051R8) || defined(TARGET_LPC11U68)
#    define OS_CLOCK       48000000

#  elif defined(TARGET_LPC812)
#    define OS_CLOCK       36000000

#  elif defined(TARGET_LPC824)
#    define OS_CLOCK       30000000

#  elif  defined(TARGET_STM32F100RB)
#    define OS_CLOCK       24000000

#  elif defined(TARGET_LPC4088) || defined(TARGET_K64F)
#    define OS_CLOCK       120000000

#  elif defined(TARGET_LPC4337)
#    define OS_CLOCK       204000000

#  elif defined(TARGET_STM32F407) || defined(TARGET_F407VG)
#    define OS_CLOCK       168000000

#  elif defined(TARGET_NRF51822)
#    define OS_CLOCK       16000000

#  elif defined(TARGET_STM32F401RE)
#    define OS_CLOCK       84000000

#  elif defined(TARGET_STM32F411RE)
#     define OS_CLOCK      100000000

#elif defined(TARGET_STM32F103RB)
#    define OS_CLOCK       72000000

#elif defined(TARGET_RTL8195A)
#    define OS_CLOCK       (200000000UL/6*5)

#  else
#    error "no target defined"
#  endif

#else // if 0

#define OS_CLOCK       (200000000UL/6*5)


#endif

#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Defines the timer tick value.
//   <i> Default: 1000  (1ms)
#ifndef OS_TICK
 #define OS_TICK        1000
#endif

// </h>

// <h>System Configuration
// =======================
//
// <e>Round-Robin Thread switching
// ===============================
//
// <i> Enables Round-Robin Thread switching.
#ifndef OS_ROBIN
 #define OS_ROBIN       1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Defines how long a thread will execute before a thread switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
 #define OS_ROBINTOUT   5
#endif

// </e>

// <e>User Timers
// ==============
//   <i> Enables user Timers
#ifndef OS_TIMERS
 #define OS_TIMERS      1
#endif

//   <o>Timer Thread Priority
//                        <1=> Low
//                        <2=> Below Normal
//                        <3=> Normal
//                        <4=> Above Normal
//                        <5=> High
//                        <6=> Realtime (highest)
//   <i> Defines priority for Timer Thread
//   <i> Default: High
#ifndef OS_TIMERPRIO
 #define OS_TIMERPRIO   5
#endif

//   <o>Timer Callback Queue size <1-32>
//   <i> Number of concurrent active timer callback functions.
//   <i> Default: 4
#ifndef OS_TIMERCBQSZ
 #define OS_TIMERCBQS   4
#endif

// </e>

//   <o>ISR FIFO Queue size<4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the interrupt handler.
//   <i> Default: 16 entries
#ifndef OS_FIFOSZ
 #define OS_FIFOSZ      16
#endif

// </h>

//------------- <<< end of configuration section >>> -----------------------

// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect
//  the arm standard runtime library. For microlib they are not used.
#ifndef OS_MUTEXCNT
 #define OS_MUTEXCNT    12
#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/

#define OS_TRV          ((uint32_t)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)


/*----------------------------------------------------------------------------
 *      OS Idle daemon
 *---------------------------------------------------------------------------*/
void os_idle_demon (void) {
  /* The idle demon is a system thread, running when no other thread is      */
  /* ready to run.                                                           */

  /* Sleep: ideally, we should put the chip to sleep.
     Unfortunately, this usually requires disconnecting the interface chip (debugger).
     This can be done, but it would break the local file system.
  */
  for (;;) {
      // sleep();
  }
}

/*----------------------------------------------------------------------------
 *      RTX Errors
 *---------------------------------------------------------------------------*/
//NeoJou
#define _LONG_CALL_     __attribute__ ((long_call))

extern _LONG_CALL_ uint32_t DiagPrintf(const char *fmt, ...);

extern void mbed_die(void);

void mbed_die(void)
{
	DiagPrintf("mbed_die\r\n");
	for(;;);
}



void os_error (uint32_t err_code) {
    /* This function is called when a runtime error is detected. Parameter     */
    /* 'err_code' holds the runtime error code (defined in RTX_Conf.h).      */
	DiagPrintf("os_error : 0x%x\n", err_code);
    mbed_die();
}

void sysThreadError(osStatus status) {
    if (status != osOK) {
		DiagPrintf("sysThreadError : 0x%x\n", status);
        mbed_die();
    }
}

/*----------------------------------------------------------------------------
 *      RTX Configuration Functions
 *---------------------------------------------------------------------------*/

#include "RTX_CM_lib.h"

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

