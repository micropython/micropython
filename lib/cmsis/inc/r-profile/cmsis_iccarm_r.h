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
 * CMSIS-Core(R) Compiler ICCARM (IAR Compiler for Arm) Header File
 */

#ifndef __CMSIS_ICCARM_R_H
#define __CMSIS_ICCARM_R_H

#pragma system_include   /* treat file as system include file */

#ifndef __CMSIS_ICCARM_H
  #error "This file must not be included directly"
#endif

#define __get_CPSR() (__arm_rsr("CPSR"))
#define __get_mode() (__get_CPSR() & 0x1FU)

#define __set_CPSR(VALUE) (__arm_wsr("CPSR", (VALUE)))
#define __set_mode(VALUE) (__arm_wsr("CPSR_c", (VALUE)))

#if (defined (__ARM_FP)      && (__ARM_FP >= 1))
  #define __get_FPEXC() (__arm_rsr("FPEXC"))
  #define __set_FPEXC(VALUE) (__arm_wsr("FPEXC", VALUE))
#else
  #define __get_FPEXC()             ( 0 )
  #define __set_FPEXC(VALUE)        ((void)VALUE)
#endif

#define __get_CP(cp, op1, RT, CRn, CRm, op2)                                   \
  ((RT) = __arm_rsr("p" #cp ":" #op1 ":c" #CRn ":c" #CRm ":" #op2))

#define __set_CP(cp, op1, RT, CRn, CRm, op2)                                   \
  (__arm_wsr("p" #cp ":" #op1 ":c" #CRn ":c" #CRm ":" #op2, (RT)))

#define __get_CP64(cp, op1, Rt, CRm)                                           \
  __ASM volatile("MRRC p" #cp ", " #op1 ", %Q0, %R0, c" #CRm                   \
                 : "=r"(Rt)                                                    \
                 :                                                             \
                 : "memory")

#define __set_CP64(cp, op1, Rt, CRm)                                           \
  __ASM volatile("MCRR p" #cp ", " #op1 ", %Q0, %R0, c" #CRm                   \
                 :                                                             \
                 : "r"(Rt)                                                     \
                 : "memory")


__IAR_FT uint32_t __get_SP_usr(void) {
  uint32_t cpsr;
  uint32_t result;
  __ASM volatile("MRS     %0, cpsr   \n"
                 "CPS     #0x1F      \n" // no effect in USR mode
                 "MOV     %1, sp     \n"
                 "MSR     cpsr_c, %0 \n" // no effect in USR mode
                 "ISB"
                 : "+r"(cpsr), "=r"(result)
                 : 
                 : "memory");
  return result;
}

__IAR_FT void __set_SP_usr(uint32_t topOfProcStack) {
  uint32_t cpsr;
  __ASM volatile("MRS     %0, cpsr   \n"
                 "CPS     #0x1F      \n" // no effect in USR mode
                 "MOV     sp, %1     \n"
                 "MSR     cpsr_c, %2 \n" // no effect in USR mode
                 "ISB"
                 : "+r"(cpsr)
                 : "r"(topOfProcStack)
                 : "memory");
}

#define __get_mode() (__get_CPSR() & 0x1FU)

__STATIC_INLINE
void __FPU_Enable(void) {
  __ASM volatile(
      // Permit access to VFP/NEON, registers by modifying CPACR
      "        MRC     p15,0,R1,c1,c0,2  \n"
      "        ORR     R1,R1,#0x00F00000 \n"
      "        MCR     p15,0,R1,c1,c0,2  \n"

      // Ensure that subsequent instructions occur in the context of VFP/NEON
      // access permitted
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

#ifdef __ARM_ADVANCED_SIMD__
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
      "        MOV32   R2,#0x00086060    \n" // Mask off all bits that do not
                                             // have to be preserved.
                                             // Non-preserved bits can/should be
                                             // zero.
      "        AND     R1,R1,R2          \n"
      "        VMSR    FPSCR,R1          \n"
      :
      :
      : "cc", "r1", "r2");
}

#undef __IAR_FT
#undef __ICCARM_V8

#pragma diag_default = Pe940
#pragma diag_default = Pe177
#endif /* __CMSIS_ARMCLANG_R_H */
