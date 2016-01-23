/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_HAL_CM.H
 *      Purpose: Hardware Abstraction Layer for Cortex-M definitions
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

/* Definitions */
#define INITIAL_xPSR    0x01000000
#define DEMCR_TRCENA    0x01000000
#define ITM_ITMENA      0x00000001
#define MAGIC_WORD      0xE25A2EA5

#if defined (__CC_ARM)          /* ARM Compiler */

#if ((__TARGET_ARCH_7_M || __TARGET_ARCH_7E_M) && !NO_EXCLUSIVE_ACCESS)
 #define __USE_EXCLUSIVE_ACCESS
#else
 #undef  __USE_EXCLUSIVE_ACCESS
#endif

#elif defined (__GNUC__)        /* GNU Compiler */

#undef  __USE_EXCLUSIVE_ACCESS

#if defined (__CORTEX_M0) || defined (__CORTEX_M0PLUS)
#define __TARGET_ARCH_6S_M 1
#else
#define __TARGET_ARCH_6S_M 0
#endif

#if defined (__VFP_FP__) && !defined(__SOFTFP__)
#define __TARGET_FPU_VFP 1
#else
#define __TARGET_FPU_VFP 0
#endif

#define __inline inline
#define __weak   __attribute__((weak))

#ifndef __CMSIS_GENERIC

__attribute__((always_inline)) static inline void __enable_irq(void)
{
  __asm volatile ("cpsie i");
}

__attribute__((always_inline)) static inline U32 __disable_irq(void)
{
  U32 result;

  __asm volatile ("mrs %0, primask" : "=r" (result));
  __asm volatile ("cpsid i");
  return(result & 1);
}

#endif

__attribute__(( always_inline)) static inline U8 __clz(U32 value)
{
  U8 result;

  __asm volatile ("clz %0, %1" : "=r" (result) : "r" (value));
  return(result);
}

#elif defined (__ICCARM__)      /* IAR Compiler */

#undef  __USE_EXCLUSIVE_ACCESS

#if (__CORE__ == __ARM6M__)
#define __TARGET_ARCH_6S_M 1
#else
#define __TARGET_ARCH_6S_M 0
#endif

#if defined __ARMVFP__
#define __TARGET_FPU_VFP 1
#else
#define __TARGET_FPU_VFP 0
#endif

#define __inline inline

#ifndef __CMSIS_GENERIC

static inline void __enable_irq(void)
{
  __asm volatile ("cpsie i");
}

static inline U32 __disable_irq(void)
{
  U32 result;

  __asm volatile ("mrs %0, primask" : "=r" (result));
  __asm volatile ("cpsid i");
  return(result & 1);
}

#endif

static inline U8 __clz(U32 value)
{
  U8 result;

  __asm volatile ("clz %0, %1" : "=r" (result) : "r" (value));
  return(result);
}

#endif

/* NVIC registers */
#define NVIC_ST_CTRL    (*((volatile U32 *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile U32 *)0xE000E014))
#define NVIC_ST_CURRENT (*((volatile U32 *)0xE000E018))
#define NVIC_ISER         ((volatile U32 *)0xE000E100)
#define NVIC_ICER         ((volatile U32 *)0xE000E180)
#if (__TARGET_ARCH_6S_M)
#define NVIC_IP           ((volatile U32 *)0xE000E400)
#else
#define NVIC_IP           ((volatile U8  *)0xE000E400)
#endif
#define NVIC_INT_CTRL   (*((volatile U32 *)0xE000ED04))
#define NVIC_AIR_CTRL   (*((volatile U32 *)0xE000ED0C))
#define NVIC_SYS_PRI2   (*((volatile U32 *)0xE000ED1C))
#define NVIC_SYS_PRI3   (*((volatile U32 *)0xE000ED20))

#define OS_PEND_IRQ()   NVIC_INT_CTRL  = (1<<28)
#define OS_PENDING      ((NVIC_INT_CTRL >> 26) & (1<<2 | 1))
#define OS_UNPEND(fl)   NVIC_INT_CTRL  = (*fl = OS_PENDING) << 25
#define OS_PEND(fl,p)   NVIC_INT_CTRL  = (fl | p<<2) << 26
#define OS_LOCK()       NVIC_ST_CTRL   =  0x0005
#define OS_UNLOCK()     NVIC_ST_CTRL   =  0x0007

#define OS_X_PENDING    ((NVIC_INT_CTRL >> 28) & 1)
#define OS_X_UNPEND(fl) NVIC_INT_CTRL  = (*fl = OS_X_PENDING) << 27
#define OS_X_PEND(fl,p) NVIC_INT_CTRL  = (fl | p) << 28
#if (__TARGET_ARCH_6S_M)
#define OS_X_INIT(n)    NVIC_IP[n>>2] |= 0xFF << (8*(n & 0x03)); \
                        NVIC_ISER[n>>5] = 1 << (n & 0x1F)
#else
#define OS_X_INIT(n)    NVIC_IP[n] = 0xFF; \
                        NVIC_ISER[n>>5] = 1 << (n & 0x1F)
#endif
#define OS_X_LOCK(n)    NVIC_ICER[n>>5] = 1 << (n & 0x1F)
#define OS_X_UNLOCK(n)  NVIC_ISER[n>>5] = 1 << (n & 0x1F)

/* Core Debug registers */
#define DEMCR           (*((volatile U32 *)0xE000EDFC))

/* ITM registers */
#define ITM_CONTROL     (*((volatile U32 *)0xE0000E80))
#define ITM_ENABLE      (*((volatile U32 *)0xE0000E00))
#define ITM_PORT30_U32  (*((volatile U32 *)0xE0000078))
#define ITM_PORT31_U32  (*((volatile U32 *)0xE000007C))
#define ITM_PORT31_U16  (*((volatile U16 *)0xE000007C))
#define ITM_PORT31_U8   (*((volatile U8  *)0xE000007C))

/* Variables */
extern BIT dbg_msg;

/* Functions */
#ifdef __USE_EXCLUSIVE_ACCESS
 #define rt_inc(p)     while(__strex((__ldrex(p)+1),p))
 #define rt_dec(p)     while(__strex((__ldrex(p)-1),p))
#else
 #define rt_inc(p)     __disable_irq();(*p)++;__enable_irq();
 #define rt_dec(p)     __disable_irq();(*p)--;__enable_irq();
#endif

__inline static U32 rt_inc_qi (U32 size, U8 *count, U8 *first) {
  U32 cnt,c2;
#ifdef __USE_EXCLUSIVE_ACCESS
  do {
    if ((cnt = __ldrex(count)) == size) {
      __clrex();
      return (cnt); }
  } while (__strex(cnt+1, count));
  do {
    c2 = (cnt = __ldrex(first)) + 1;
    if (c2 == size) c2 = 0;
  } while (__strex(c2, first));
#else
  __disable_irq();
  if ((cnt = *count) < size) {
    *count = cnt+1;
    c2 = (cnt = *first) + 1;
    if (c2 == size) c2 = 0;
    *first = c2;
  }
  __enable_irq ();
#endif
  return (cnt);
}

__inline static void rt_systick_init (void) {
  NVIC_ST_RELOAD  = os_trv;
  NVIC_ST_CURRENT = 0;
  NVIC_ST_CTRL    = 0x0007;
  NVIC_SYS_PRI3  |= 0xFF000000;
}

__inline static void rt_svc_init (void) {
#if !(__TARGET_ARCH_6S_M)
  int sh,prigroup;
#endif
  NVIC_SYS_PRI3 |= 0x00FF0000;
#if (__TARGET_ARCH_6S_M)
  NVIC_SYS_PRI2 |= (NVIC_SYS_PRI3<<(8+1)) & 0xFC000000;
#else
  sh       = 8 - __clz (~((NVIC_SYS_PRI3 << 8) & 0xFF000000));
  prigroup = ((NVIC_AIR_CTRL >> 8) & 0x07);
  if (prigroup >= sh) {
    sh = prigroup + 1;
  }
  NVIC_SYS_PRI2 = ((0xFEFFFFFF << sh) & 0xFF000000) | (NVIC_SYS_PRI2 & 0x00FFFFFF);
#endif
}

extern void rt_set_PSP (U32 stack);
extern U32  rt_get_PSP (void);
extern void os_set_env (void);
extern void *_alloc_box (void *box_mem);
extern int  _free_box (void *box_mem, void *box);

extern void rt_init_stack (P_TCB p_TCB, FUNCP task_body);
extern void rt_ret_val  (P_TCB p_TCB, U32 v0);
extern void rt_ret_val2 (P_TCB p_TCB, U32 v0, U32 v1);

extern void dbg_init (void);
extern void dbg_task_notify (P_TCB p_tcb, BOOL create);
extern void dbg_task_switch (U32 task_id);

#ifdef DBG_MSG
#define DBG_INIT() dbg_init()
#define DBG_TASK_NOTIFY(p_tcb,create) if (dbg_msg) dbg_task_notify(p_tcb,create)
#define DBG_TASK_SWITCH(task_id)      if (dbg_msg && (os_tsk.new_tsk != os_tsk.run)) \
                                                   dbg_task_switch(task_id)
#else
#define DBG_INIT()
#define DBG_TASK_NOTIFY(p_tcb,create)
#define DBG_TASK_SWITCH(task_id)
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

