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
 * CMSIS-Core(M) Compiler ICCARM (IAR Compiler for Arm) Header File
 */

#ifndef __CMSIS_ICCARM_M_H__
#define __CMSIS_ICCARM_M_H__

#ifndef __ICCARM__
  #error This file should only be compiled by ICCARM
#endif

#pragma system_include

/* Define compiler macros for CPU architecture, used in CMSIS 5.
 */
#if __ARM_ARCH_6M__ || __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ || __ARM_ARCH_8M_BASE__ || __ARM_ARCH_8M_MAIN__ || __ARM_ARCH_8_1M_MAIN__
/* Macros already defined */
#else
  #if defined(__ARM8M_MAINLINE__) || defined(__ARM8EM_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__ 1
  #elif defined(__ARM8M_BASELINE__)
    #define __ARM_ARCH_8M_BASE__ 1
  #elif defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M'
    #if __ARM_ARCH == 6
      #define __ARM_ARCH_6M__ 1
    #elif __ARM_ARCH == 7
      #if __ARM_FEATURE_DSP
        #define __ARM_ARCH_7EM__ 1
      #else
        #define __ARM_ARCH_7M__ 1
      #endif
    #elif __ARM_ARCH == 801
      #define __ARM_ARCH_8_1M_MAIN__ 1
    #endif /* __ARM_ARCH */
  #endif /* __ARM_ARCH_PROFILE == 'M' */
#endif

/* Alternativ core deduction for older ICCARM's */
#if !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) && !defined(__ARM_ARCH_7EM__) && \
    !defined(__ARM_ARCH_8M_BASE__) && !defined(__ARM_ARCH_8M_MAIN__) && !defined(__ARM_ARCH_8_1M_MAIN__)
  #if defined(__ARM6M__) && (__CORE__ == __ARM6M__)
    #define __ARM_ARCH_6M__ 1
  #elif defined(__ARM7M__) && (__CORE__ == __ARM7M__)
    #define __ARM_ARCH_7M__ 1
  #elif defined(__ARM7EM__) && (__CORE__ == __ARM7EM__)
    #define __ARM_ARCH_7EM__  1
  #elif defined(__ARM8M_BASELINE__) && (__CORE == __ARM8M_BASELINE__)
    #define __ARM_ARCH_8M_BASE__ 1
  #elif defined(__ARM8M_MAINLINE__) && (__CORE == __ARM8M_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__ 1
  #elif defined(__ARM8EM_MAINLINE__) && (__CORE == __ARM8EM_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__ 1
  #elif defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M' && __ARM_ARCH == 801
    #define __ARM_ARCH_8_1M_MAIN__ 1
  #else
    #error "Unknown target."
  #endif
#endif



#if defined(__ARM_ARCH_6M__) && __ARM_ARCH_6M__==1
  #define __IAR_M0_FAMILY  1
#elif defined(__ARM_ARCH_8M_BASE__) && __ARM_ARCH_8M_BASE__==1
  #define __IAR_M0_FAMILY  1
#else
  #define __IAR_M0_FAMILY  0
#endif

#ifndef __PROGRAM_START
#define __PROGRAM_START           __iar_program_start
#endif

#ifndef __INITIAL_SP
#define __INITIAL_SP              CSTACK$$Limit
#endif

#ifndef __STACK_LIMIT
#define __STACK_LIMIT             CSTACK$$Base
#endif

#ifndef __VECTOR_TABLE
#define __VECTOR_TABLE            __vector_table
#endif

#ifndef __VECTOR_TABLE_ATTRIBUTE
#define __VECTOR_TABLE_ATTRIBUTE  @".intvec"
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#ifndef __STACK_SEAL
#define __STACK_SEAL              STACKSEAL$$Base
#endif

#ifndef __TZ_STACK_SEAL_SIZE
#define __TZ_STACK_SEAL_SIZE      8U
#endif

#ifndef __TZ_STACK_SEAL_VALUE
#define __TZ_STACK_SEAL_VALUE     0xFEF5EDA5FEF5EDA5ULL
#endif

__STATIC_FORCEINLINE void __TZ_set_STACKSEAL_S (uint32_t* stackTop) {
  *((uint64_t *)stackTop) = __TZ_STACK_SEAL_VALUE;
}
#endif

#define __get_APSR()                (__arm_rsr("APSR"))
#define __get_BASEPRI()             (__arm_rsr("BASEPRI"))
#define __get_CONTROL()             (__arm_rsr("CONTROL"))
#define __get_FAULTMASK()           (__arm_rsr("FAULTMASK"))

#define __get_IPSR()                (__arm_rsr("IPSR"))
#define __get_MSP()                 (__arm_rsr("MSP"))
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  #define __get_MSPLIM()            (0U)
#else
  #define __get_MSPLIM()            (__arm_rsr("MSPLIM"))
#endif
#define __get_PRIMASK()             (__arm_rsr("PRIMASK"))
#define __get_PSP()                 (__arm_rsr("PSP"))

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  #define __get_PSPLIM()            (0U)
#else
  #define __get_PSPLIM()            (__arm_rsr("PSPLIM"))
#endif

#define __get_xPSR()                (__arm_rsr("xPSR"))

#define __set_BASEPRI(VALUE)        (__arm_wsr("BASEPRI", (VALUE)))
#define __set_BASEPRI_MAX(VALUE)    (__arm_wsr("BASEPRI_MAX", (VALUE)))

__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control)
{
__arm_wsr("CONTROL", control);
__iar_builtin_ISB();
}

#define __set_FAULTMASK(VALUE)      (__arm_wsr("FAULTMASK", (VALUE)))
#define __set_MSP(VALUE)            (__arm_wsr("MSP", (VALUE)))

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  #define __set_MSPLIM(VALUE)       ((void)(VALUE))
#else
  #define __set_MSPLIM(VALUE)       (__arm_wsr("MSPLIM", (VALUE)))
#endif
#define __set_PRIMASK(VALUE)        (__arm_wsr("PRIMASK", (VALUE)))
#define __set_PSP(VALUE)            (__arm_wsr("PSP", (VALUE)))
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  #define __set_PSPLIM(VALUE)       ((void)(VALUE))
#else
  #define __set_PSPLIM(VALUE)       (__arm_wsr("PSPLIM", (VALUE)))
#endif

#define __TZ_get_CONTROL_NS()       (__arm_rsr("CONTROL_NS"))

__STATIC_FORCEINLINE void __TZ_set_CONTROL_NS(uint32_t control)
{
__arm_wsr("CONTROL_NS", control);
__iar_builtin_ISB();
}

#define __TZ_get_PSP_NS()           (__arm_rsr("PSP_NS"))
#define __TZ_set_PSP_NS(VALUE)      (__arm_wsr("PSP_NS", (VALUE)))
#define __TZ_get_MSP_NS()           (__arm_rsr("MSP_NS"))
#define __TZ_set_MSP_NS(VALUE)      (__arm_wsr("MSP_NS", (VALUE)))
#define __TZ_get_SP_NS()            (__arm_rsr("SP_NS"))
#define __TZ_set_SP_NS(VALUE)       (__arm_wsr("SP_NS", (VALUE)))
#define __TZ_get_PRIMASK_NS()       (__arm_rsr("PRIMASK_NS"))
#define __TZ_set_PRIMASK_NS(VALUE)  (__arm_wsr("PRIMASK_NS", (VALUE)))
#define __TZ_get_BASEPRI_NS()       (__arm_rsr("BASEPRI_NS"))
#define __TZ_set_BASEPRI_NS(VALUE)  (__arm_wsr("BASEPRI_NS", (VALUE)))
#define __TZ_get_FAULTMASK_NS()     (__arm_rsr("FAULTMASK_NS"))
#define __TZ_set_FAULTMASK_NS(VALUE)(__arm_wsr("FAULTMASK_NS", (VALUE)))

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     !(defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  #define __TZ_get_PSPLIM_NS()      (0U)
  #define __TZ_set_PSPLIM_NS(VALUE) ((void)(VALUE))
#else
  #define __TZ_get_PSPLIM_NS()      (__arm_rsr("PSPLIM_NS"))
  #define __TZ_set_PSPLIM_NS(VALUE) (__arm_wsr("PSPLIM_NS", (VALUE)))
#endif

#define __TZ_get_MSPLIM_NS()        (__arm_rsr("MSPLIM_NS"))
#define __TZ_set_MSPLIM_NS(VALUE)   (__arm_wsr("MSPLIM_NS", (VALUE)))

#undef __IAR_FT
#undef __IAR_M0_FAMILY
#undef __ICCARM_V8

#pragma diag_default=Pe940
#pragma diag_default=Pe177


#endif /* __CMSIS_ICCARM_M_H__ */
