/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CM_LIB.H
 *      Purpose: RTX Kernel System Configuration
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
//NeoJou
//#include "mbed_error.h"

#if   defined (__CC_ARM)
#pragma O3
#define __USED __attribute__((used))
#elif defined (__GNUC__)
#pragma GCC optimize ("O3")
#define __USED __attribute__((used))
#elif defined (__ICCARM__)
#define __USED __root
#endif


/*----------------------------------------------------------------------------
 *      Definitions
 *---------------------------------------------------------------------------*/

#define _declare_box(pool,size,cnt)  uint32_t pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt) uint64_t pool[(((size)+7)/8)*(cnt) + 2]

#define OS_TCB_SIZE     48
#define OS_TMR_SIZE     8

#if defined (__CC_ARM) && !defined (__MICROLIB)

typedef void    *OS_ID;
typedef uint32_t OS_TID;
typedef uint32_t OS_MUT[3];
typedef uint32_t OS_RESULT;

#define runtask_id()    rt_tsk_self()
#define mutex_init(m)   rt_mut_init(m)
#define mutex_wait(m)   os_mut_wait(m,0xFFFF)
#define mutex_rel(m)    os_mut_release(m)

extern OS_TID    rt_tsk_self    (void);
extern void      rt_mut_init    (OS_ID mutex);
extern OS_RESULT rt_mut_release (OS_ID mutex);
extern OS_RESULT rt_mut_wait    (OS_ID mutex, uint16_t timeout);

#define os_mut_wait(mutex,timeout) _os_mut_wait((uint32_t)rt_mut_wait,mutex,timeout)
#define os_mut_release(mutex)      _os_mut_release((uint32_t)rt_mut_release,mutex)

OS_RESULT _os_mut_release (uint32_t p, OS_ID mutex)                   __svc_indirect(0);
OS_RESULT _os_mut_wait    (uint32_t p, OS_ID mutex, uint16_t timeout) __svc_indirect(0);

#endif


/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

#if (OS_TIMERS != 0)
#define OS_TASK_CNT (OS_TASKCNT + 1)
#else
#define OS_TASK_CNT  OS_TASKCNT
#endif

uint16_t const os_maxtaskrun = OS_TASK_CNT;
uint32_t const os_rrobin     = (OS_ROBIN << 16) | OS_ROBINTOUT;
uint32_t const os_trv        = OS_TRV;
uint8_t  const os_flags      = OS_RUNPRIV;

/* Export following defines to uVision debugger. */
__USED uint32_t const os_clockrate = OS_TICK;
__USED uint32_t const os_timernum  = 0;

/* Stack for the os_idle_demon */
unsigned int idle_task_stack[OS_IDLESTKSIZE];
unsigned short const idle_task_stack_size = OS_IDLESTKSIZE;

#ifndef OS_FIFOSZ
 #define OS_FIFOSZ      16
#endif

/* Fifo Queue buffer for ISR requests.*/
uint32_t       os_fifo[OS_FIFOSZ*2+1];
uint8_t  const os_fifo_size = OS_FIFOSZ;

/* An array of Active task pointers. */
void *os_active_TCB[OS_TASK_CNT];

/* User Timers Resources */
#if (OS_TIMERS != 0)
extern void osTimerThread (void const *argument);
osThreadDef(osTimerThread, (osPriority)(OS_TIMERPRIO-3), 4*OS_TIMERSTKSZ);
osThreadId osThreadId_osTimerThread;
osMessageQDef(osTimerMessageQ, OS_TIMERCBQS, void *);
osMessageQId osMessageQId_osTimerMessageQ;
#else
osThreadDef_t os_thread_def_osTimerThread = { NULL };
osThreadId osThreadId_osTimerThread;
osMessageQDef(osTimerMessageQ, 0, void *);
osMessageQId osMessageQId_osTimerMessageQ;
#endif


/*----------------------------------------------------------------------------
 *      RTX Optimizations (empty functions)
 *---------------------------------------------------------------------------*/

#if OS_ROBIN == 0
 void rt_init_robin (void) {;}
 void rt_chk_robin  (void) {;}
#endif

#if OS_STKCHECK == 0
 void rt_stk_check  (void) {;}
#endif


/*----------------------------------------------------------------------------
 *      Standard Library multithreading interface
 *---------------------------------------------------------------------------*/

#if defined (__CC_ARM) && !defined (__MICROLIB)
 static OS_MUT   std_libmutex[OS_MUTEXCNT];
 static uint32_t nr_mutex;

 /*--------------------------- _mutex_initialize -----------------------------*/

int _mutex_initialize (OS_ID *mutex) {
  /* Allocate and initialize a system mutex. */

  if (nr_mutex >= OS_MUTEXCNT) {
    /* If you are here, you need to increase the number OS_MUTEXCNT. */
    error("Not enough stdlib mutexes\n");
  }
  *mutex = &std_libmutex[nr_mutex++];
  mutex_init (*mutex);
  return (1);
}


/*--------------------------- _mutex_acquire --------------------------------*/

__attribute__((used)) void _mutex_acquire (OS_ID *mutex) {
  /* Acquire a system mutex, lock stdlib resources. */
  if (runtask_id ()) {
    /* RTX running, acquire a mutex. */
    mutex_wait (*mutex);
  }
}


/*--------------------------- _mutex_release --------------------------------*/

__attribute__((used)) void _mutex_release (OS_ID *mutex) {
  /* Release a system mutex, unlock stdlib resources. */
  if (runtask_id ()) {
    /* RTX running, release a mutex. */
    mutex_rel (*mutex);
  }
}

#endif


/*----------------------------------------------------------------------------
 *      RTX Startup
 *---------------------------------------------------------------------------*/

/* Main Thread definition */
extern int main (void);
osThreadDef_t os_thread_def_main = {(os_pthread)main, osPriorityNormal, 0, NULL};

// This define should be probably moved to the CMSIS layer
//NeoJou
#if 0
#if   defined(TARGET_LPC1768)
#define INITIAL_SP            (0x10008000UL)

#elif defined(TARGET_LPC11U24)
#define INITIAL_SP            (0x10002000UL)

#elif defined(TARGET_LPC11U35_401) || defined(TARGET_LPC11U35_501) || defined(TARGET_LPCCAPPUCCINO)
#define INITIAL_SP            (0x10002000UL)

#elif defined(TARGET_LPC1114)
#define INITIAL_SP            (0x10001000UL)

#elif defined(TARGET_LPC812)
#define INITIAL_SP            (0x10001000UL)

#elif defined(TARGET_LPC824)
#define INITIAL_SP            (0x10002000UL)

#elif defined(TARGET_KL25Z)
#define INITIAL_SP            (0x20003000UL)

#elif defined(TARGET_K64F)
#define INITIAL_SP            (0x20030000UL)

#elif defined(TARGET_KL46Z)
#define INITIAL_SP            (0x20006000UL)

#elif defined(TARGET_KL43Z)
#define INITIAL_SP            (0x20006000UL)

#elif defined(TARGET_KL05Z)
#define INITIAL_SP            (0x20000C00UL)

#elif defined(TARGET_LPC4088)
#define INITIAL_SP            (0x10010000UL)

#elif defined(TARGET_LPC4337)
#define INITIAL_SP            (0x10008000UL)

#elif defined(TARGET_LPC1347)
#define INITIAL_SP            (0x10002000UL)

#elif defined(TARGET_STM32F100RB) || defined(TARGET_STM32F051R8)
#define INITIAL_SP            (0x20002000UL)

#elif defined(TARGET_DISCO_F303VC)
#define INITIAL_SP            (0x2000A000UL)

#elif defined(TARGET_STM32F407) || defined(TARGET_F407VG)
#define INITIAL_SP            (0x20020000UL)

#elif defined(TARGET_STM32F401RE)
#define INITIAL_SP            (0x20018000UL)

#elif defined(TARGET_LPC1549)
#define INITIAL_SP            (0x02009000UL)

#elif defined(TARGET_LPC11U68)
#define INITIAL_SP            (0x10004000UL)

#elif defined(TARGET_NRF51822)
#define INITIAL_SP            (0x20004000UL)

#elif defined(TARGET_STM32F411RE)
#define INITIAL_SP            (0x20020000UL)

#elif defined(TARGET_STM32F103RB)
#define INITIAL_SP            (0x20005000UL)

#elif defined(TARGET_STM32F302R8)
#define INITIAL_SP            (0x20004000UL)

#elif  defined(TARGET_STM32F334R8)
#define INITIAL_SP            (0x20003000UL)

#elif  defined(TARGET_STM32F334C8)
#define INITIAL_SP            (0x20003000UL)

#elif  defined(TARGET_RTL8195A)
extern unsigned char          __StackTop[];
#define INITIAL_SP            (__StackTop)
//#define INITIAL_SP            (0x1FFFFFFFUL)

#else
#error "no target defined"

#endif
#else // if 0
extern unsigned char          __StackTop[];
#define INITIAL_SP            (__StackTop)
#endif

#ifdef __CC_ARM
extern unsigned char     Image$$RW_IRAM1$$ZI$$Limit[];
#define HEAP_START      (Image$$RW_IRAM1$$ZI$$Limit)
#elif defined(__GNUC__)
extern unsigned char     __end__[];
#define HEAP_START      (__end__)
#endif

void set_main_stack(void) {
    // That is the bottom of the main stack block: no collision detection
    os_thread_def_main.stack_pointer = HEAP_START;

    // Leave OS_SCHEDULERSTKSIZE words for the scheduler and interrupts
    os_thread_def_main.stacksize = (INITIAL_SP - (unsigned int)HEAP_START) - (OS_SCHEDULERSTKSIZE * 4);
}

#if defined (__CC_ARM)
#ifdef __MICROLIB
void _main_init (void) __attribute__((section(".ARM.Collect$$$$000000FF")));
void _main_init (void) {
  osKernelInitialize();
  set_main_stack();
  osThreadCreate(&os_thread_def_main, NULL);
  osKernelStart();
  for (;;);
}
#else

/* The single memory model is checking for stack collision at run time, verifing
   that the heap pointer is underneath the stack pointer.

   With the RTOS there is not only one stack above the heap, there are multiple
   stacks and some of them are underneath the heap pointer.
*/
#pragma import(__use_two_region_memory)

__asm void __rt_entry (void) {

  IMPORT  __user_setup_stackheap
  IMPORT  __rt_lib_init
  IMPORT  os_thread_def_main
  IMPORT  osKernelInitialize
  IMPORT  set_main_stack
  IMPORT  osKernelStart
  IMPORT  osThreadCreate
  IMPORT  exit

  BL      __user_setup_stackheap
  MOV     R1,R2
  BL      __rt_lib_init
  BL      osKernelInitialize
  BL      set_main_stack
  LDR     R0,=os_thread_def_main
  MOVS    R1,#0
  BL      osThreadCreate
  BL      osKernelStart
  BL      exit

  ALIGN
}
#endif

#elif defined (__GNUC__)

#ifdef __CS3__

/* CS3 start_c routine.
 *
 * Copyright (c) 2006, 2007 CodeSourcery Inc
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include "cs3.h"

extern void __libc_init_array (void);

__attribute ((noreturn)) void __cs3_start_c (void){
  unsigned regions = __cs3_region_num;
  const struct __cs3_region *rptr = __cs3_regions;

  /* Initialize memory */
  for (regions = __cs3_region_num, rptr = __cs3_regions; regions--; rptr++) {
    long long *src = (long long *)rptr->init;
    long long *dst = (long long *)rptr->data;
    unsigned limit = rptr->init_size;
    unsigned count;

    if (src != dst)
      for (count = 0; count != limit; count += sizeof (long long))
        *dst++ = *src++;
    else
      dst = (long long *)((char *)dst + limit);
    limit = rptr->zero_size;
    for (count = 0; count != limit; count += sizeof (long long))
      *dst++ = 0;
  }

  /* Run initializers.  */
  __libc_init_array ();

  osKernelInitialize();
  set_main_stack();
  osThreadCreate(&os_thread_def_main, NULL);
  osKernelStart();
  for (;;);
}

#else

__attribute__((naked)) void software_init_hook (void) {
  __asm (
    ".syntax unified\n"
    ".thumb\n"
    "movs r0,#0\n"
    "movs r1,#0\n"
    "mov  r4,r0\n"
    "mov  r5,r1\n"
    "ldr  r0,= __libc_fini_array\n"
    "bl   atexit\n"
    "bl   __libc_init_array\n"
    "mov  r0,r4\n"
    "mov  r1,r5\n"
    "bl   osKernelInitialize\n"
    "bl   set_main_stack\n"
    "ldr  r0,=os_thread_def_main\n"
    "movs r1,#0\n"
    "bl   osThreadCreate\n"
    "bl   osKernelStart\n"
    "bl   exit\n"
  );
}

#endif

#elif defined (__ICCARM__)

extern int  __low_level_init(void);
extern void __iar_data_init3(void);
extern void exit(int arg);

__noreturn __stackless void __cmain(void) {
  int a;

  if (__low_level_init() != 0) {
    __iar_data_init3();
  }
  osKernelInitialize();
  osThreadCreate(&os_thread_def_main, NULL);
  a = osKernelStart();
  exit(a);
}

#endif


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/


