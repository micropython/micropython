/*
 * Copyright (c) 2017-2024 IAR Systems
 * Copyright (c) 2017-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * CMSIS-Core(Generic) Compiler ICCARM (IAR Compiler for Arm) Header File
 */

#ifndef __CMSIS_ICCARM_H
#define __CMSIS_ICCARM_H

#pragma system_include

#if (__VER__ >= 8000000)
  #define __ICCARM_V8 1
#else
  #define __ICCARM_V8 0
#endif

#define __IAR_FT _Pragma("inline=forced") __intrinsic

#ifndef __ASM
  #define __ASM __asm
#endif

#ifndef __INLINE
  #define __INLINE inline
#endif

#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE       static inline
#endif

#ifndef   __FORCEINLINE
  #define __FORCEINLINE         _Pragma("inline=forced")
#endif

#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE  __FORCEINLINE __STATIC_INLINE
#endif

#ifndef   __NO_RETURN
  #if defined(__cplusplus) && __cplusplus >= 201103L
    #define __NO_RETURN [[noreturn]]
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define __NO_RETURN _Noreturn
  #else
    #define __NO_RETURN _Pragma("object_attribute=__noreturn")
  #endif
#endif

#ifndef   CMSIS_DEPRECATED
  #define CMSIS_DEPRECATED      __attribute__((deprecated))
#endif

#ifndef   __USED
  #if __ICCARM_V8
    #define __USED __attribute__((used))
  #else
    #define __USED _Pragma("__root")
  #endif
#endif

#undef __WEAK                           /* undo the definition from DLib_Defaults.h */
#ifndef   __WEAK
  #if __ICCARM_V8
    #define __WEAK __attribute__((weak))
  #else
    #define __WEAK _Pragma("__weak")
  #endif
#endif

#ifndef   __PACKED
  #if __ICCARM_V8
    #define __PACKED __attribute__((packed, aligned(1)))
  #else
    /* Needs IAR language extensions */
    #define __PACKED __packed
  #endif
#endif

#ifndef   __PACKED_STRUCT
  #if __ICCARM_V8
    #define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
  #else
    /* Needs IAR language extensions */
    #define __PACKED_STRUCT __packed struct
  #endif
#endif

#ifndef   __PACKED_UNION
  #if __ICCARM_V8
    #define __PACKED_UNION union __attribute__((packed, aligned(1)))
  #else
    /* Needs IAR language extensions */
    #define __PACKED_UNION __packed union
  #endif
#endif

#ifndef __UNALIGNED_UINT16_READ
#pragma language=save
#pragma language=extended
__IAR_FT uint16_t __iar_uint16_read(void const *ptr)
{
  return *(__packed uint16_t*)(ptr);
}
#pragma language=restore
#define __UNALIGNED_UINT16_READ(PTR) __iar_uint16_read(PTR)
#endif


#ifndef __UNALIGNED_UINT16_WRITE
#pragma language=save
#pragma language=extended
__IAR_FT void __iar_uint16_write(void const *ptr, uint16_t val)
{
  *(__packed uint16_t*)(ptr) = val;;
}
#pragma language=restore
#define __UNALIGNED_UINT16_WRITE(PTR,VAL) __iar_uint16_write(PTR,VAL)
#endif

#ifndef __UNALIGNED_UINT32_READ
#pragma language=save
#pragma language=extended
__IAR_FT uint32_t __iar_uint32_read(void const *ptr)
{
  return *(__packed uint32_t*)(ptr);
}
#pragma language=restore
#define __UNALIGNED_UINT32_READ(PTR) __iar_uint32_read(PTR)
#endif

#ifndef __UNALIGNED_UINT32_WRITE
#pragma language=save
#pragma language=extended
__IAR_FT void __iar_uint32_write(void const *ptr, uint32_t val)
{
  *(__packed uint32_t*)(ptr) = val;;
}
#pragma language=restore
#define __UNALIGNED_UINT32_WRITE(PTR,VAL) __iar_uint32_write(PTR,VAL)
#endif

#ifndef __ALIGNED
  #if __ICCARM_V8
    #define __ALIGNED(x) __attribute__((aligned(x)))
  #elif (__VER__ >= 7080000)
    /* Needs IAR language extensions */
    #define __ALIGNED(x) __attribute__((aligned(x)))
  #else
    #warning No compiler specific solution for __ALIGNED.__ALIGNED is ignored.
    #define __ALIGNED(x)
  #endif
#endif

#ifndef   __RESTRICT
  #if __ICCARM_V8
    #define __RESTRICT            __restrict
  #else
    /* Needs IAR language extensions */
    #define __RESTRICT            restrict
  #endif
#endif

#ifndef   __COMPILER_BARRIER
  #define __COMPILER_BARRIER() __ASM volatile("":::"memory")
#endif

#ifndef __NO_INIT
  #define __NO_INIT __attribute__ ((section (".noinit")))
#endif

#ifndef __ALIAS
  #define __ALIAS(x) __attribute__ ((alias(x)))
#endif

#if defined(__CLZ)
  #undef __CLZ
#endif
#if defined(__REVSH)
  #undef __REVSH
#endif
#if defined(__RBIT)
  #undef __RBIT
#endif
#if defined(__SSAT)
  #undef __SSAT
#endif
#if defined(__USAT)
  #undef __USAT
#endif

#include "iccarm_builtin.h"

#define __disable_irq       __iar_builtin_disable_interrupt
#define __enable_irq        __iar_builtin_enable_interrupt
#define __arm_rsr           __iar_builtin_rsr
#define __arm_wsr           __iar_builtin_wsr

#define __NOP     __iar_builtin_no_operation
#define __WFI     __iar_builtin_WFI
#define __WFE     __iar_builtin_WFE
#define __ISB     __iar_builtin_ISB
#define __SEV     __iar_builtin_SEV
#define __DSB     __iar_builtin_DSB
#define __DMB     __iar_builtin_DMB
#define __REV     __iar_builtin_REV
#define __REV16   __iar_builtin_REV16
#define __ROR     __iar_builtin_ROR
#define __RBIT    __iar_builtin_RBIT
#define __CLZ     __iar_builtin_CLZ

__IAR_FT int16_t __REVSH(int16_t val)
{
  return (int16_t) __iar_builtin_REVSH(val);
}


#define __BKPT(value)    __asm volatile ("BKPT     %0" : : "i"(value))

#if (__ARM_FEATURE_LDREX >= 1)

/*
 * __iar_builtin_CLREX can be reordered w.r.t. STREX during high optimizations.
 * As a workaround we use inline assembly and a memory barrier.
 * (IAR issue EWARM-11901)
 * Fixed in EWARM 9.50.i2
 */

__IAR_FT void __CLREX(void) {
  __ASM volatile("CLREX" ::: "memory"); 
}


#define __LDREXB  __iar_builtin_LDREXB
#define __STREXB  __iar_builtin_STREXB

#endif /* (__ARM_FEATURE_LDREX >= 1) */

#if (__ARM_FEATURE_LDREX >= 2)

#define __LDREXH  __iar_builtin_LDREXH
#define __STREXH  __iar_builtin_STREXH

#endif /* (__ARM_FEATURE_LDREX >= 2) */

#if (__ARM_FEATURE_LDREX >= 4)

#define __LDREXW  __iar_builtin_LDREX
#define __STREXW  __iar_builtin_STREX

#endif /* (__ARM_FEATURE_LDREX >= 4) */

#if ((__ARM_FEATURE_SAT    >= 1) && \
     (__ARM_ARCH_ISA_THUMB >= 2)    )
/* __ARM_FEATURE_SAT is wrong for Armv8-M Baseline devices */
/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
#define __SSAT    __iar_builtin_SSAT


/**
  \brief   Unsigned Saturate
  \details Saturates an unsigned value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (0..31)
  \return             Saturated value
*/
#define __USAT    __iar_builtin_USAT

#else /* (__ARM_FEATURE_SAT >= 1) */
/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
__STATIC_FORCEINLINE int32_t __SSAT(int32_t val, uint32_t sat)
{
  if ((sat >= 1U) && (sat <= 32U))
  {
    const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
    const int32_t min = -1 - max ;
    if (val > max)
    {
      return (max);
    }
    else if (val < min)
    {
      return (min);
    }
  }
  return (val);
}


/**
  \brief   Unsigned Saturate
  \details Saturates an unsigned value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (0..31)
  \return             Saturated value
 */
__STATIC_FORCEINLINE uint32_t __USAT(int32_t val, uint32_t sat)
{
  if (sat <= 31U)
  {
    const uint32_t max = ((1U << sat) - 1U);
    if (val > (int32_t)max)
    {
      return (max);
    }
    else if (val < 0)
    {
      return (0U);
    }
  }
  return ((uint32_t)val);
}
#endif /* (__ARM_FEATURE_SAT >= 1) */

#if (__ARM_ARCH_ISA_THUMB >= 2)

#define __RRX     __iar_builtin_RRX

__IAR_FT uint8_t __LDRBT(volatile uint8_t *addr)
{
  uint32_t res;
  __ASM volatile ("LDRBT %0, [%1]" : "=&r" (res) : "r" (addr) : "memory");
  return ((uint8_t)res);
}

__IAR_FT uint16_t __LDRHT(volatile uint16_t *addr)
{
  uint32_t res;
  __ASM volatile ("LDRHT %0, [%1]" : "=&r" (res) : "r" (addr) : "memory");
  return ((uint16_t)res);
}

__IAR_FT uint32_t __LDRT(volatile uint32_t *addr)
{
  uint32_t res;
  __ASM volatile ("LDRT %0, [%1]" : "=&r" (res) : "r" (addr) : "memory");
  return res;
}

__IAR_FT void __STRBT(uint8_t value, volatile uint8_t *addr)
{
  __ASM volatile ("STRBT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
}

__IAR_FT void __STRHT(uint16_t value, volatile uint16_t *addr)
{
  __ASM volatile ("STRHT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
}

__IAR_FT void __STRT(uint32_t value, volatile uint32_t *addr)
{
  __ASM volatile ("STRT %1, [%0]" : : "r" (addr), "r" (value) : "memory");
}
#endif

#if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
  #define __SADD8   __iar_builtin_SADD8
  #define __QADD8   __iar_builtin_QADD8
  #define __SHADD8  __iar_builtin_SHADD8
  #define __UADD8   __iar_builtin_UADD8
  #define __UQADD8  __iar_builtin_UQADD8
  #define __UHADD8  __iar_builtin_UHADD8
  #define __SSUB8   __iar_builtin_SSUB8
  #define __QSUB8   __iar_builtin_QSUB8
  #define __SHSUB8  __iar_builtin_SHSUB8
  #define __USUB8   __iar_builtin_USUB8
  #define __UQSUB8  __iar_builtin_UQSUB8
  #define __UHSUB8  __iar_builtin_UHSUB8
  #define __SADD16  __iar_builtin_SADD16
  #define __QADD16  __iar_builtin_QADD16
  #define __SHADD16 __iar_builtin_SHADD16
  #define __UADD16  __iar_builtin_UADD16
  #define __UQADD16 __iar_builtin_UQADD16
  #define __UHADD16 __iar_builtin_UHADD16
  #define __SSUB16  __iar_builtin_SSUB16
  #define __QSUB16  __iar_builtin_QSUB16
  #define __SHSUB16 __iar_builtin_SHSUB16
  #define __USUB16  __iar_builtin_USUB16
  #define __UQSUB16 __iar_builtin_UQSUB16
  #define __UHSUB16 __iar_builtin_UHSUB16
  #define __SASX    __iar_builtin_SASX
  #define __QASX    __iar_builtin_QASX
  #define __SHASX   __iar_builtin_SHASX
  #define __UASX    __iar_builtin_UASX
  #define __UQASX   __iar_builtin_UQASX
  #define __UHASX   __iar_builtin_UHASX
  #define __SSAX    __iar_builtin_SSAX
  #define __QSAX    __iar_builtin_QSAX
  #define __SHSAX   __iar_builtin_SHSAX
  #define __USAX    __iar_builtin_USAX
  #define __UQSAX   __iar_builtin_UQSAX
  #define __UHSAX   __iar_builtin_UHSAX
  #define __USAD8   __iar_builtin_USAD8
  #define __USADA8  __iar_builtin_USADA8
  #define __SSAT16  __iar_builtin_SSAT16
  #define __USAT16  __iar_builtin_USAT16
  #define __UXTB16  __iar_builtin_UXTB16
  #define __UXTAB16 __iar_builtin_UXTAB16
  #define __SXTB16  __iar_builtin_SXTB16
  #define __SXTAB16 __iar_builtin_SXTAB16
  #define __SMUAD   __iar_builtin_SMUAD
  #define __SMUADX  __iar_builtin_SMUADX
  #define __SMMLA   __iar_builtin_SMMLA
  #define __SMLAD   __iar_builtin_SMLAD
  #define __SMLADX  __iar_builtin_SMLADX
  #define __SMLALD  __iar_builtin_SMLALD
  #define __SMLALDX __iar_builtin_SMLALDX
  #define __SMUSD   __iar_builtin_SMUSD
  #define __SMUSDX  __iar_builtin_SMUSDX
  #define __SMLSD   __iar_builtin_SMLSD
  #define __SMLSDX  __iar_builtin_SMLSDX
  #define __SMLSLD  __iar_builtin_SMLSLD
  #define __SMLSLDX __iar_builtin_SMLSLDX
  #define __SEL     __iar_builtin_SEL
  #define __QADD    __iar_builtin_QADD
  #define __QSUB    __iar_builtin_QSUB
  #define __PKHBT   __iar_builtin_PKHBT
  #define __PKHTB   __iar_builtin_PKHTB

  /* Note, these are suboptimal but I lack compiler features to express this */

  #define __SXTB16_RORn(ARG1, ARG2) __SXTB16(__ROR(ARG1, ARG2))
  #define __SXTAB16_RORn(ARG1, ARG2, ARG3) __SXTAB16(ARG1, __ROR(ARG2, ARG3))

#endif /* (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)) */

#if (defined (__ARM_FP)      && (__ARM_FP >= 1))
  #define __get_FPSCR()             (__arm_rsr("FPSCR"))
  #define __set_FPSCR(VALUE)        (__arm_wsr("FPSCR", (VALUE)))
#else
  #define __get_FPSCR()             ( 0 )
  #define __set_FPSCR(VALUE)        ((void)VALUE)
#endif

#if (defined(__ARM_ARCH_ISA_THUMB) && __ARM_ARCH_ISA_THUMB >= 2)
// This is not really fault_irq on Cortex-not-M. However 
// there seems to be code that assumes this.
  __IAR_FT void __disable_fault_irq(void)
  {
    __ASM volatile ("CPSID F" ::: "memory");
  }

  __IAR_FT void __enable_fault_irq(void)
  {
    __ASM volatile ("CPSIE F" ::: "memory");
  }
#endif

#if (__ARM_ARCH >= 8)

  __IAR_FT uint8_t __LDAB(volatile uint8_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDAB %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return ((uint8_t)res);
  }

  __IAR_FT uint16_t __LDAH(volatile uint16_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDAH %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return ((uint16_t)res);
  }

  __IAR_FT uint32_t __LDA(volatile uint32_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDA %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return res;
  }

  __IAR_FT void __STLB(uint8_t value, volatile uint8_t *ptr)
  {
    __ASM volatile ("STLB %1, [%0]" :: "r" (ptr), "r" (value) : "memory");
  }

  __IAR_FT void __STLH(uint16_t value, volatile uint16_t *ptr)
  {
    __ASM volatile ("STLH %1, [%0]" :: "r" (ptr), "r" (value) : "memory");
  }

  __IAR_FT void __STL(uint32_t value, volatile uint32_t *ptr)
  {
    __ASM volatile ("STL %1, [%0]" :: "r" (ptr), "r" (value) : "memory");
  }

  __IAR_FT uint8_t __LDAEXB(volatile uint8_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDAEXB %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return ((uint8_t)res);
  }

  __IAR_FT uint16_t __LDAEXH(volatile uint16_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDAEXH %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return ((uint16_t)res);
  }

  __IAR_FT uint32_t __LDAEX(volatile uint32_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("LDAEX %0, [%1]" : "=r" (res) : "r" (ptr) : "memory");
    return res;
  }

  __IAR_FT uint32_t __STLEXB(uint8_t value, volatile uint8_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("STLEXB %0, %2, [%1]" : "=&r" (res) : "r" (ptr), "r" (value) : "memory");
    return res;
  }

  __IAR_FT uint32_t __STLEXH(uint16_t value, volatile uint16_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("STLEXH %0, %2, [%1]" : "=&r" (res) : "r" (ptr), "r" (value) : "memory");
    return res;
  }

  __IAR_FT uint32_t __STLEX(uint32_t value, volatile uint32_t *ptr)
  {
    uint32_t res;
    __ASM volatile ("STLEX %0, %2, [%1]" : "=&r" (res) : "r" (ptr), "r" (value) : "memory");
    return res;
  }

#endif /* __ARM_ARCH >= 8 */

  #if __ARM_ARCH_PROFILE == 'A'
    #include "a-profile/cmsis_iccarm_a.h"
  #elif __ARM_ARCH_PROFILE == 'R'
    #include "r-profile/cmsis_iccarm_r.h"
  #elif __ARM_ARCH_PROFILE == 'M'
    #include "m-profile/cmsis_iccarm_m.h"
  #else
    #error "Unknown Arm architecture profile"
  #endif


#endif 
