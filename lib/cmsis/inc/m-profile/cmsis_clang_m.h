/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
 * CMSIS-Core(M) Compiler LLVM/Clang Header File
 */

#ifndef __CMSIS_CLANG_M_H
#define __CMSIS_CLANG_M_H

#pragma clang system_header   /* treat file as system include file */

#ifndef __CMSIS_CLANG_H
  #error "This file must not be included directly"
#endif

#if (__ARM_ACLE >= 200)
  #include <arm_acle.h>
#else
  #error Compiler must support ACLE V2.0
#endif /* (__ARM_ACLE >= 200) */

/* Fallback for __has_builtin */
#ifndef __has_builtin
  #define __has_builtin(x) (0)
#endif


/* #########################  Startup and Lowlevel Init  ######################## */
#ifndef __PROGRAM_START
#define __PROGRAM_START           _start
#endif

#ifndef __INITIAL_SP
#define __INITIAL_SP              __stack
#endif

#ifndef __STACK_LIMIT
#define __STACK_LIMIT             __stack_limit
#endif

#ifndef __VECTOR_TABLE
#define __VECTOR_TABLE            __Vectors
#endif

#ifndef __VECTOR_TABLE_ATTRIBUTE
#define __VECTOR_TABLE_ATTRIBUTE  __attribute__((used, section(".vectors")))
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
#ifndef __STACK_SEAL
#define __STACK_SEAL              __stack_seal
#endif

#ifndef __TZ_STACK_SEAL_SIZE
#define __TZ_STACK_SEAL_SIZE      8U
#endif

#ifndef __TZ_STACK_SEAL_VALUE
#define __TZ_STACK_SEAL_VALUE     0xFEF5EDA5FEF5EDA5ULL
#endif

__STATIC_FORCEINLINE void __TZ_set_STACKSEAL_S (uint32_t* stackTop)
{
  *((uint64_t *)stackTop) = __TZ_STACK_SEAL_VALUE;
}
#endif


#if (__ARM_ARCH_ISA_THUMB >= 2)
/**
  \brief   STRT Unprivileged (8 bit)
  \details Executes a Unprivileged STRT instruction for 8 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRBT(uint8_t value, volatile uint8_t *ptr)
{
  __ASM volatile ("strbt %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}


/**
  \brief   STRT Unprivileged (16 bit)
  \details Executes a Unprivileged STRT instruction for 16 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRHT(uint16_t value, volatile uint16_t *ptr)
{
  __ASM volatile ("strht %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}


/**
  \brief   STRT Unprivileged (32 bit)
  \details Executes a Unprivileged STRT instruction for 32 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRT(uint32_t value, volatile uint32_t *ptr)
{
  __ASM volatile ("strt %1, %0" : "=Q" (*ptr) : "r" (value) );
}
#endif /* (__ARM_ARCH_ISA_THUMB >= 2) */


/* ###########################  Core Function Access  ########################### */
/** \ingroup  CMSIS_Core_FunctionInterface
    \defgroup CMSIS_Core_RegAccFunctions CMSIS Core Register Access Functions
  @{
 */

/**
  \brief   Get Control Register
  \details Returns the content of the Control Register.
  \return               Control Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Control Register (non-secure)
  \details Returns the content of the non-secure Control Register when in secure mode.
  \return               non-secure Control Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_CONTROL_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, control_ns" : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Control Register
  \details Writes the given value to the Control Register.
  \param [in]    control  Control Register value to set
 */
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control)
{
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
  __ISB();
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Control Register (non-secure)
  \details Writes the given value to the non-secure Control Register when in secure state.
  \param [in]    control  Control Register value to set
 */
__STATIC_FORCEINLINE void __TZ_set_CONTROL_NS(uint32_t control)
{
  __ASM volatile ("MSR control_ns, %0" : : "r" (control) : "memory");
  __ISB();
}
#endif


/**
  \brief   Get IPSR Register
  \details Returns the content of the IPSR Register.
  \return               IPSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_IPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return (result);
}


/**
  \brief   Get APSR Register
  \details Returns the content of the APSR Register.
  \return               APSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_APSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return (result);
}


/**
  \brief   Get xPSR Register
  \details Returns the content of the xPSR Register.
  \return               xPSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_xPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return (result);
}


/**
  \brief   Get Process Stack Pointer
  \details Returns the current value of the Process Stack Pointer (PSP).
  \return               PSP Register value
 */
__STATIC_FORCEINLINE uint32_t __get_PSP(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, psp"  : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Process Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Process Stack Pointer (PSP) when in secure state.
  \return               PSP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PSP_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, psp_ns"  : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Process Stack Pointer
  \details Assigns the given value to the Process Stack Pointer (PSP).
  \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_PSP(uint32_t topOfProcStack)
{
  __ASM volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Process Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Process Stack Pointer (PSP) when in secure state.
  \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_PSP_NS(uint32_t topOfProcStack)
{
  __ASM volatile ("MSR psp_ns, %0" : : "r" (topOfProcStack) : );
}
#endif


/**
  \brief   Get Main Stack Pointer
  \details Returns the current value of the Main Stack Pointer (MSP).
  \return               MSP Register value
 */
__STATIC_FORCEINLINE uint32_t __get_MSP(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, msp" : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Main Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Main Stack Pointer (MSP) when in secure state.
  \return               MSP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_MSP_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, msp_ns" : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Main Stack Pointer
  \details Assigns the given value to the Main Stack Pointer (MSP).
  \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Main Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Main Stack Pointer (MSP) when in secure state.
  \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_MSP_NS(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp_ns, %0" : : "r" (topOfMainStack) : );
}
#endif


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Stack Pointer (SP) when in secure state.
  \return               SP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_SP_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, sp_ns" : "=r" (result) );
  return (result);
}


/**
  \brief   Set Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Stack Pointer (SP) when in secure state.
  \param [in]    topOfStack  Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_SP_NS(uint32_t topOfStack)
{
  __ASM volatile ("MSR sp_ns, %0" : : "r" (topOfStack) : );
}
#endif


/**
  \brief   Get Priority Mask
  \details Returns the current state of the priority mask bit from the Priority Mask Register.
  \return               Priority Mask value
 */
__STATIC_FORCEINLINE uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Priority Mask (non-secure)
  \details Returns the current state of the non-secure priority mask bit from the Priority Mask Register when in secure state.
  \return               Priority Mask value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PRIMASK_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, primask_ns" : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
__STATIC_FORCEINLINE void __set_PRIMASK(uint32_t priMask)
{
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Priority Mask (non-secure)
  \details Assigns the given value to the non-secure Priority Mask Register when in secure state.
  \param [in]    priMask  Priority Mask
 */
__STATIC_FORCEINLINE void __TZ_set_PRIMASK_NS(uint32_t priMask)
{
  __ASM volatile ("MSR primask_ns, %0" : : "r" (priMask) : "memory");
}
#endif


#if (__ARM_ARCH_ISA_THUMB >= 2)
/**
  \brief   Get Base Priority
  \details Returns the current value of the Base Priority register.
  \return               Base Priority register value
 */
__STATIC_FORCEINLINE uint32_t __get_BASEPRI(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, basepri" : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Base Priority (non-secure)
  \details Returns the current value of the non-secure Base Priority register when in secure state.
  \return               Base Priority register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_BASEPRI_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, basepri_ns" : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Base Priority
  \details Assigns the given value to the Base Priority register.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __set_BASEPRI(uint32_t basePri)
{
  __ASM volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Base Priority (non-secure)
  \details Assigns the given value to the non-secure Base Priority register when in secure state.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __TZ_set_BASEPRI_NS(uint32_t basePri)
{
  __ASM volatile ("MSR basepri_ns, %0" : : "r" (basePri) : "memory");
}
#endif


/**
  \brief   Set Base Priority with condition
  \details Assigns the given value to the Base Priority register only if BASEPRI masking is disabled,
           or the new value increases the BASEPRI priority level.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __set_BASEPRI_MAX(uint32_t basePri)
{
  __ASM volatile ("MSR basepri_max, %0" : : "r" (basePri) : "memory");
}


/**
  \brief   Get Fault Mask
  \details Returns the current value of the Fault Mask register.
  \return               Fault Mask register value
 */
__STATIC_FORCEINLINE uint32_t __get_FAULTMASK(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, faultmask" : "=r" (result) );
  return (result);
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Fault Mask (non-secure)
  \details Returns the current value of the non-secure Fault Mask register when in secure state.
  \return               Fault Mask register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_FAULTMASK_NS(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, faultmask_ns" : "=r" (result) );
  return (result);
}
#endif


/**
  \brief   Set Fault Mask
  \details Assigns the given value to the Fault Mask register.
  \param [in]    faultMask  Fault Mask value to set
 */
__STATIC_FORCEINLINE void __set_FAULTMASK(uint32_t faultMask)
{
  __ASM volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Fault Mask (non-secure)
  \details Assigns the given value to the non-secure Fault Mask register when in secure state.
  \param [in]    faultMask  Fault Mask value to set
 */
__STATIC_FORCEINLINE void __TZ_set_FAULTMASK_NS(uint32_t faultMask)
{
  __ASM volatile ("MSR faultmask_ns, %0" : : "r" (faultMask) : "memory");
}
#endif

#endif /* (__ARM_ARCH_ISA_THUMB >= 2) */


#if (__ARM_ARCH >= 8)
/**
  \brief   Get Process Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always in non-secure
  mode.

  \details Returns the current value of the Process Stack Pointer Limit (PSPLIM).
  \return               PSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __get_PSPLIM(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  /* without main extensions, the non-secure PSPLIM is RAZ/WI */
  return (0U);
#else
  uint32_t result;
  __ASM volatile ("MRS %0, psplim"  : "=r" (result) );
  return (result);
#endif
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Process Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always.

  \details Returns the current value of the non-secure Process Stack Pointer Limit (PSPLIM) when in secure state.
  \return               PSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PSPLIM_NS(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)))
  /* without main extensions, the non-secure PSPLIM is RAZ/WI */
  return (0U);
#else
  uint32_t result;
  __ASM volatile ("MRS %0, psplim_ns"  : "=r" (result) );
  return (result);
#endif
}
#endif


/**
  \brief   Set Process Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored in non-secure
  mode.

  \details Assigns the given value to the Process Stack Pointer Limit (PSPLIM).
  \param [in]    ProcStackPtrLimit  Process Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __set_PSPLIM(uint32_t ProcStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  /* without main extensions, the non-secure PSPLIM is RAZ/WI */
  (void)ProcStackPtrLimit;
#else
  __ASM volatile ("MSR psplim, %0" : : "r" (ProcStackPtrLimit));
#endif
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Process Stack Pointer (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored.

  \details Assigns the given value to the non-secure Process Stack Pointer Limit (PSPLIM) when in secure state.
  \param [in]    ProcStackPtrLimit  Process Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __TZ_set_PSPLIM_NS(uint32_t ProcStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)))
  /* without main extensions, the non-secure PSPLIM is RAZ/WI */
  (void)ProcStackPtrLimit;
#else
  __ASM volatile ("MSR psplim_ns, %0\n" : : "r" (ProcStackPtrLimit));
#endif
}
#endif


/**
  \brief   Get Main Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always.

  \details Returns the current value of the Main Stack Pointer Limit (MSPLIM).
  \return               MSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __get_MSPLIM(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  /* without main extensions, the non-secure MSPLIM is RAZ/WI */
  return (0U);
#else
  uint32_t result;
  __ASM volatile ("MRS %0, msplim" : "=r" (result) );
  return (result);
#endif
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Get Main Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always.

  \details Returns the current value of the non-secure Main Stack Pointer Limit(MSPLIM) when in secure state.
  \return               MSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_MSPLIM_NS(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)))
  /* without main extensions, the non-secure MSPLIM is RAZ/WI */
  return (0U);
#else
  uint32_t result;
  __ASM volatile ("MRS %0, msplim_ns" : "=r" (result) );
  return (result);
#endif
}
#endif


/**
  \brief   Set Main Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored.

  \details Assigns the given value to the Main Stack Pointer Limit (MSPLIM).
  \param [in]    MainStackPtrLimit  Main Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __set_MSPLIM(uint32_t MainStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  /* without main extensions, the non-secure MSPLIM is RAZ/WI */
  (void)MainStackPtrLimit;
#else
  __ASM volatile ("MSR msplim, %0" : : "r" (MainStackPtrLimit));
#endif
}


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
/**
  \brief   Set Main Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored.

  \details Assigns the given value to the non-secure Main Stack Pointer Limit (MSPLIM) when in secure state.
  \param [in]    MainStackPtrLimit  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_MSPLIM_NS(uint32_t MainStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)))
  /* without main extensions, the non-secure MSPLIM is RAZ/WI */
  (void)MainStackPtrLimit;
#else
  __ASM volatile ("MSR msplim_ns, %0" : : "r" (MainStackPtrLimit));
#endif
}
#endif

#endif /* (__ARM_ARCH >= 8) */
/** @} end of CMSIS_Core_RegAccFunctions */


/* ###################  Compiler specific Intrinsics  ########################### */
/** \defgroup CMSIS_SIMD_intrinsics CMSIS SIMD Intrinsics
  Access to dedicated SIMD instructions
  @{
*/

#if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
#define     __SADD8                 __sadd8
#define     __QADD8                 __qadd8
#define     __SHADD8                __shadd8
#define     __UADD8                 __uadd8
#define     __UQADD8                __uqadd8
#define     __UHADD8                __uhadd8
#define     __SSUB8                 __ssub8
#define     __QSUB8                 __qsub8
#define     __SHSUB8                __shsub8
#define     __USUB8                 __usub8
#define     __UQSUB8                __uqsub8
#define     __UHSUB8                __uhsub8
#define     __SADD16                __sadd16
#define     __QADD16                __qadd16
#define     __SHADD16               __shadd16
#define     __UADD16                __uadd16
#define     __UQADD16               __uqadd16
#define     __UHADD16               __uhadd16
#define     __SSUB16                __ssub16
#define     __QSUB16                __qsub16
#define     __SHSUB16               __shsub16
#define     __USUB16                __usub16
#define     __UQSUB16               __uqsub16
#define     __UHSUB16               __uhsub16
#define     __SASX                  __sasx
#define     __QASX                  __qasx
#define     __SHASX                 __shasx
#define     __UASX                  __uasx
#define     __UQASX                 __uqasx
#define     __UHASX                 __uhasx
#define     __SSAX                  __ssax
#define     __QSAX                  __qsax
#define     __SHSAX                 __shsax
#define     __USAX                  __usax
#define     __UQSAX                 __uqsax
#define     __UHSAX                 __uhsax
#define     __USAD8                 __usad8
#define     __USADA8                __usada8
#define     __SSAT16                __ssat16
#define     __USAT16                __usat16
#define     __UXTB16                __uxtb16
#define     __UXTAB16               __uxtab16
#define     __SXTB16                __sxtb16
#define     __SXTAB16               __sxtab16
#define     __SMUAD                 __smuad
#define     __SMUADX                __smuadx
#define     __SMLAD                 __smlad
#define     __SMLADX                __smladx
#define     __SMLALD                __smlald
#define     __SMLALDX               __smlaldx
#define     __SMUSD                 __smusd
#define     __SMUSDX                __smusdx
#define     __SMLSD                 __smlsd
#define     __SMLSDX                __smlsdx
#define     __SMLSLD                __smlsld
#define     __SMLSLDX               __smlsldx
#define     __SEL                   __sel
#define     __QADD                  __qadd
#define     __QSUB                  __qsub

#define __PKHBT(ARG1,ARG2,ARG3) \
__extension__ \
({                          \
  uint32_t __RES, __ARG1 = (ARG1), __ARG2 = (ARG2); \
  __ASM ("pkhbt %0, %1, %2, lsl %3" : "=r" (__RES) :  "r" (__ARG1), "r" (__ARG2), "I" (ARG3)  ); \
  __RES; \
 })

#define __PKHTB(ARG1,ARG2,ARG3) \
__extension__ \
({                          \
  uint32_t __RES, __ARG1 = (ARG1), __ARG2 = (ARG2); \
  if (ARG3 == 0) \
    __ASM ("pkhtb %0, %1, %2" : "=r" (__RES) :  "r" (__ARG1), "r" (__ARG2)  ); \
  else \
    __ASM ("pkhtb %0, %1, %2, asr %3" : "=r" (__RES) :  "r" (__ARG1), "r" (__ARG2), "I" (ARG3)  ); \
  __RES; \
 })

#define __SXTB16_RORn(ARG1, ARG2)        __SXTB16(__ROR(ARG1, ARG2))

#define __SXTAB16_RORn(ARG1, ARG2, ARG3) __SXTAB16(ARG1, __ROR(ARG2, ARG3))

__STATIC_FORCEINLINE int32_t __SMMLA (int32_t op1, int32_t op2, int32_t op3)
{
  int32_t result;

  __ASM volatile ("smmla %0, %1, %2, %3" : "=r" (result): "r"  (op1), "r" (op2), "r" (op3) );
  return (result);
}
#endif /* (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)) */
/** @} end of group CMSIS_SIMD_intrinsics */


#endif /* __CMSIS_CLANG_M_H */
