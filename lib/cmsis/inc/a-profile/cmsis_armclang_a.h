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
 * CMSIS-Core(A) Compiler ARMClang (Arm Compiler 6) Header File
 */

#ifndef __CMSIS_ARMCLANG_A_H
#define __CMSIS_ARMCLANG_A_H

#pragma clang system_header   /* treat file as system include file */

#ifndef __CMSIS_ARMCLANG_H
  #error "This file must not be included directly"
#endif

/**
  \brief   STRT Unprivileged (8 bit)
  \details Executes a Unprivileged STRT instruction for 8 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRBT(uint8_t value, volatile uint8_t *ptr)
{
  __ASM volatile ("strbt %1, %0, #0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}


/**
  \brief   STRT Unprivileged (16 bit)
  \details Executes a Unprivileged STRT instruction for 16 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRHT(uint16_t value, volatile uint16_t *ptr)
{
  __ASM volatile ("strht %1, %0, #0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}


/**
  \brief   STRT Unprivileged (32 bit)
  \details Executes a Unprivileged STRT instruction for 32 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
 */
__STATIC_FORCEINLINE void __STRT(uint32_t value, volatile uint32_t *ptr)
{
  __ASM volatile ("strt %1, %0, #0" : "=Q" (*ptr) : "r" (value) );
}



/* ###################  Compiler specific Intrinsics  ########################### */
/** \defgroup CMSIS_SIMD_intrinsics CMSIS SIMD Intrinsics
  Access to dedicated SIMD instructions
  @{
*/
#if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))

#define     __SADD8                 __builtin_arm_sadd8
#define     __QADD8                 __builtin_arm_qadd8
#define     __SHADD8                __builtin_arm_shadd8
#define     __UADD8                 __builtin_arm_uadd8
#define     __UQADD8                __builtin_arm_uqadd8
#define     __UHADD8                __builtin_arm_uhadd8
#define     __SSUB8                 __builtin_arm_ssub8
#define     __QSUB8                 __builtin_arm_qsub8
#define     __SHSUB8                __builtin_arm_shsub8
#define     __USUB8                 __builtin_arm_usub8
#define     __UQSUB8                __builtin_arm_uqsub8
#define     __UHSUB8                __builtin_arm_uhsub8
#define     __SADD16                __builtin_arm_sadd16
#define     __QADD16                __builtin_arm_qadd16
#define     __SHADD16               __builtin_arm_shadd16
#define     __UADD16                __builtin_arm_uadd16
#define     __UQADD16               __builtin_arm_uqadd16
#define     __UHADD16               __builtin_arm_uhadd16
#define     __SSUB16                __builtin_arm_ssub16
#define     __QSUB16                __builtin_arm_qsub16
#define     __SHSUB16               __builtin_arm_shsub16
#define     __USUB16                __builtin_arm_usub16
#define     __UQSUB16               __builtin_arm_uqsub16
#define     __UHSUB16               __builtin_arm_uhsub16
#define     __SASX                  __builtin_arm_sasx
#define     __QASX                  __builtin_arm_qasx
#define     __SHASX                 __builtin_arm_shasx
#define     __UASX                  __builtin_arm_uasx
#define     __UQASX                 __builtin_arm_uqasx
#define     __UHASX                 __builtin_arm_uhasx
#define     __SSAX                  __builtin_arm_ssax
#define     __QSAX                  __builtin_arm_qsax
#define     __SHSAX                 __builtin_arm_shsax
#define     __USAX                  __builtin_arm_usax
#define     __UQSAX                 __builtin_arm_uqsax
#define     __UHSAX                 __builtin_arm_uhsax
#define     __USAD8                 __builtin_arm_usad8
#define     __USADA8                __builtin_arm_usada8
#define     __SSAT16                __builtin_arm_ssat16
#define     __USAT16                __builtin_arm_usat16
#define     __UXTB16                __builtin_arm_uxtb16
#define     __UXTAB16               __builtin_arm_uxtab16
#define     __SXTB16                __builtin_arm_sxtb16
#define     __SXTAB16               __builtin_arm_sxtab16
#define     __SMUAD                 __builtin_arm_smuad
#define     __SMUADX                __builtin_arm_smuadx
#define     __SMLAD                 __builtin_arm_smlad
#define     __SMLADX                __builtin_arm_smladx
#define     __SMLALD                __builtin_arm_smlald
#define     __SMLALDX               __builtin_arm_smlaldx
#define     __SMUSD                 __builtin_arm_smusd
#define     __SMUSDX                __builtin_arm_smusdx
#define     __SMLSD                 __builtin_arm_smlsd
#define     __SMLSDX                __builtin_arm_smlsdx
#define     __SMLSLD                __builtin_arm_smlsld
#define     __SMLSLDX               __builtin_arm_smlsldx
#define     __SEL                   __builtin_arm_sel
#define     __QADD                  __builtin_arm_qadd
#define     __QSUB                  __builtin_arm_qsub

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

__STATIC_FORCEINLINE uint32_t __SXTB16_RORn(uint32_t op1, uint32_t rotate)
{
    uint32_t result;
    if (__builtin_constant_p(rotate) && ((rotate == 8U) || (rotate == 16U) || (rotate == 24U)))
    {
        __ASM volatile("sxtb16 %0, %1, ROR %2" : "=r"(result) : "r"(op1), "i"(rotate));
    }
    else
    {
        result = __SXTB16(__ROR(op1, rotate));
    }
    return result;
}

__STATIC_FORCEINLINE uint32_t __SXTAB16_RORn(uint32_t op1, uint32_t op2, uint32_t rotate)
{
    uint32_t result;
    if (__builtin_constant_p(rotate) && ((rotate == 8U) || (rotate == 16U) || (rotate == 24U)))
    {
        __ASM volatile("sxtab16 %0, %1, %2, ROR %3" : "=r"(result) : "r"(op1), "r"(op2), "i"(rotate));
    }
    else
    {
        result = __SXTAB16(op1, __ROR(op2, rotate));
    }
    return result;
}

__STATIC_FORCEINLINE int32_t __SMMLA (int32_t op1, int32_t op2, int32_t op3)
{
  int32_t result;

  __ASM volatile ("smmla %0, %1, %2, %3" : "=r" (result): "r"  (op1), "r" (op2), "r" (op3) );
  return (result);
}

#endif /* (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)) */
 /** @} end of group CMSIS_SIMD_intrinsics */

/* ###########################  Core Function Access  ########################### */
/** \ingroup  CMSIS_Core_FunctionInterface
    \defgroup CMSIS_Core_RegAccFunctions CMSIS Core Register Access Functions
  @{
 */

/** \brief  Get CPSR Register
    \return               CPSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CPSR(void)
{
  uint32_t result;
  __ASM volatile("MRS %0, cpsr" : "=r" (result) );
  return(result);
}

/** \brief  Set CPSR Register
    \param [in]    cpsr  CPSR value to set
 */
__STATIC_FORCEINLINE void __set_CPSR(uint32_t cpsr)
{
  __ASM volatile ("MSR cpsr, %0" : : "r" (cpsr) : "cc", "memory");
}

/** \brief  Get Mode
    \return                Processor Mode
 */
__STATIC_FORCEINLINE uint32_t __get_mode(void)
{
  return (__get_CPSR() & 0x1FU);
}

/** \brief  Set Mode
    \param [in]    mode  Mode value to set
 */
__STATIC_FORCEINLINE void __set_mode(uint32_t mode)
{
  __ASM volatile("MSR  cpsr_c, %0" : : "r" (mode) : "memory");
}

/** \brief  Get Stack Pointer
    \return Stack Pointer value
 */
__STATIC_FORCEINLINE uint32_t __get_SP(void)
{
  uint32_t result;
  __ASM volatile("MOV  %0, sp" : "=r" (result) : : "memory");
  return result;
}

/** \brief  Set Stack Pointer
    \param [in]    stack  Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_SP(uint32_t stack)
{
  __ASM volatile("MOV  sp, %0" : : "r" (stack) : "memory");
}

/** \brief  Get USR/SYS Stack Pointer
    \return USR/SYS Stack Pointer value
 */
__STATIC_FORCEINLINE uint32_t __get_SP_usr(void)
{
  uint32_t cpsr;
  uint32_t result;
  __ASM volatile(
    "MRS     %0, cpsr   \n"
    "CPS     #0x1F      \n" // no effect in USR mode
    "MOV     %1, sp     \n"
    "MSR     cpsr_c, %0 \n" // no effect in USR mode
    "ISB" :  "=r"(cpsr), "=r"(result) : : "memory"
   );
  return result;
}

/** \brief  Set USR/SYS Stack Pointer
    \param [in]    topOfProcStack  USR/SYS Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_SP_usr(uint32_t topOfProcStack)
{
  uint32_t cpsr;
  __ASM volatile(
    "MRS     %0, cpsr   \n"
    "CPS     #0x1F      \n" // no effect in USR mode
    "MOV     sp, %1     \n"
    "MSR     cpsr_c, %0 \n" // no effect in USR mode
    "ISB" : "=r"(cpsr) : "r" (topOfProcStack) : "memory"
   );
}

/** \brief  Get FPEXC
    \return               Floating Point Exception Control register value
 */
__STATIC_FORCEINLINE uint32_t __get_FPEXC(void)
{
#if (__FPU_PRESENT == 1)
  uint32_t result;
  __ASM volatile("VMRS %0, fpexc" : "=r" (result) : : "memory");
  return(result);
#else
  return(0);
#endif
}

/** \brief  Set FPEXC
    \param [in]    fpexc  Floating Point Exception Control value to set
 */
__STATIC_FORCEINLINE void __set_FPEXC(uint32_t fpexc)
{
#if (__FPU_PRESENT == 1)
  __ASM volatile ("VMSR fpexc, %0" : : "r" (fpexc) : "memory");
#endif
}

/** @} end of CMSIS_Core_RegAccFunctions */


/*
 * Include common core functions to access Coprocessor 15 registers
 */

#define __get_CP(cp, op1, Rt, CRn, CRm, op2) __ASM volatile("MRC p" # cp ", " # op1 ", %0, c" # CRn ", c" # CRm ", " # op2 : "=r" (Rt) : : "memory" )
#define __set_CP(cp, op1, Rt, CRn, CRm, op2) __ASM volatile("MCR p" # cp ", " # op1 ", %0, c" # CRn ", c" # CRm ", " # op2 : : "r" (Rt) : "memory" )
#define __get_CP64(cp, op1, Rt, CRm)         __ASM volatile("MRRC p" # cp ", " # op1 ", %Q0, %R0, c" # CRm  : "=r" (Rt) : : "memory" )
#define __set_CP64(cp, op1, Rt, CRm)         __ASM volatile("MCRR p" # cp ", " # op1 ", %Q0, %R0, c" # CRm  : : "r" (Rt) : "memory" )

#include "cmsis_cp15.h"

/** \brief  Enable Floating Point Unit

  Critical section, called from undef handler, so systick is disabled
 */
__STATIC_INLINE void __FPU_Enable(void)
{
  __ASM volatile(
    // Permit access to VFP/NEON, registers by modifying CPACR
    "        MRC     p15,0,R1,c1,c0,2  \n"
    "        ORR     R1,R1,#0x00F00000 \n"
    "        MCR     p15,0,R1,c1,c0,2  \n"

    // Ensure that subsequent instructions occur in the context of VFP/NEON access permitted
    "        ISB                       \n"

    // Enable VFP/NEON
    "        VMRS    R1,FPEXC          \n"
    "        ORR     R1,R1,#0x40000000 \n"
    "        VMSR    FPEXC,R1          \n"

    // Initialise VFP/NEON registers to 0
    "        MOV     R2,#0             \n"

    // Initialise D16 registers to 0
    "        VMOV    D0, R2,R2         \n"
    "        VMOV    D1, R2,R2         \n"
    "        VMOV    D2, R2,R2         \n"
    "        VMOV    D3, R2,R2         \n"
    "        VMOV    D4, R2,R2         \n"
    "        VMOV    D5, R2,R2         \n"
    "        VMOV    D6, R2,R2         \n"
    "        VMOV    D7, R2,R2         \n"
    "        VMOV    D8, R2,R2         \n"
    "        VMOV    D9, R2,R2         \n"
    "        VMOV    D10,R2,R2         \n"
    "        VMOV    D11,R2,R2         \n"
    "        VMOV    D12,R2,R2         \n"
    "        VMOV    D13,R2,R2         \n"
    "        VMOV    D14,R2,R2         \n"
    "        VMOV    D15,R2,R2         \n"

#if (defined(__ARM_NEON) && (__ARM_NEON == 1))
    // Initialise D32 registers to 0
    "        VMOV    D16,R2,R2         \n"
    "        VMOV    D17,R2,R2         \n"
    "        VMOV    D18,R2,R2         \n"
    "        VMOV    D19,R2,R2         \n"
    "        VMOV    D20,R2,R2         \n"
    "        VMOV    D21,R2,R2         \n"
    "        VMOV    D22,R2,R2         \n"
    "        VMOV    D23,R2,R2         \n"
    "        VMOV    D24,R2,R2         \n"
    "        VMOV    D25,R2,R2         \n"
    "        VMOV    D26,R2,R2         \n"
    "        VMOV    D27,R2,R2         \n"
    "        VMOV    D28,R2,R2         \n"
    "        VMOV    D29,R2,R2         \n"
    "        VMOV    D30,R2,R2         \n"
    "        VMOV    D31,R2,R2         \n"
#endif

  // Initialise FPSCR to a known state
    "        VMRS    R1,FPSCR          \n"
    "        LDR     R2,=0x00086060    \n" //Mask off all bits that do not have to be preserved. Non-preserved bits can/should be zero.
    "        AND     R1,R1,R2          \n"
    "        VMSR    FPSCR,R1            "
    : : : "cc", "r1", "r2"
  );
}

#endif /* __CMSIS_ARMCLANG_A_H */
