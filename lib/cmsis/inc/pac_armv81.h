/******************************************************************************
 * @file     pac_armv81.h
 * @brief    CMSIS PAC key functions for Armv8.1-M PAC extension
 * @version  V1.0.0
 * @date     23. March 2022
 ******************************************************************************/
/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header    /* treat file as system include file */
#endif

#ifndef PAC_ARMV81_H
#define PAC_ARMV81_H


/* ###################  PAC Key functions  ########################### */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_PacKeyFunctions PAC Key functions
  \brief    Functions that access the PAC keys.
  @{
 */

#if (defined (__ARM_FEATURE_PAUTH) && (__ARM_FEATURE_PAUTH == 1))

/**
  \brief   read the PAC key used for privileged mode
  \details Reads the PAC key stored in the PAC_KEY_P registers.
  \param [out]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __get_PAC_KEY_P (uint32_t* pPacKey) {
  __ASM volatile (
  "mrs   r1, pac_key_p_0\n"
  "str   r1,[%0,#0]\n"
  "mrs   r1, pac_key_p_1\n"
  "str   r1,[%0,#4]\n"
  "mrs   r1, pac_key_p_2\n"
  "str   r1,[%0,#8]\n"
  "mrs   r1, pac_key_p_3\n"
  "str   r1,[%0,#12]\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   write the PAC key used for privileged mode
  \details writes the given PAC key to the PAC_KEY_P registers.
  \param [in]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __set_PAC_KEY_P (uint32_t* pPacKey) {
  __ASM volatile (
  "ldr   r1,[%0,#0]\n"
  "msr   pac_key_p_0, r1\n"
  "ldr   r1,[%0,#4]\n"
  "msr   pac_key_p_1, r1\n"
  "ldr   r1,[%0,#8]\n"
  "msr   pac_key_p_2, r1\n"
  "ldr   r1,[%0,#12]\n"
  "msr   pac_key_p_3, r1\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   read the PAC key used for unprivileged mode
  \details Reads the PAC key stored in the PAC_KEY_U registers.
  \param [out]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __get_PAC_KEY_U (uint32_t* pPacKey) {
  __ASM volatile (
  "mrs   r1, pac_key_u_0\n"
  "str   r1,[%0,#0]\n"
  "mrs   r1, pac_key_u_1\n"
  "str   r1,[%0,#4]\n"
  "mrs   r1, pac_key_u_2\n"
  "str   r1,[%0,#8]\n"
  "mrs   r1, pac_key_u_3\n"
  "str   r1,[%0,#12]\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   write the PAC key used for unprivileged mode
  \details writes the given PAC key to the PAC_KEY_U registers.
  \param [in]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __set_PAC_KEY_U (uint32_t* pPacKey) {
  __ASM volatile (
  "ldr   r1,[%0,#0]\n"
  "msr   pac_key_u_0, r1\n"
  "ldr   r1,[%0,#4]\n"
  "msr   pac_key_u_1, r1\n"
  "ldr   r1,[%0,#8]\n"
  "msr   pac_key_u_2, r1\n"
  "ldr   r1,[%0,#12]\n"
  "msr   pac_key_u_3, r1\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))

/**
  \brief   read the PAC key used for privileged mode (non-secure)
  \details Reads the PAC key stored in the non-secure PAC_KEY_P registers when in secure mode.
  \param [out]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __TZ_get_PAC_KEY_P_NS (uint32_t* pPacKey) {
  __ASM volatile (
  "mrs   r1, pac_key_p_0_ns\n"
  "str   r1,[%0,#0]\n"
  "mrs   r1, pac_key_p_1_ns\n"
  "str   r1,[%0,#4]\n"
  "mrs   r1, pac_key_p_2_ns\n"
  "str   r1,[%0,#8]\n"
  "mrs   r1, pac_key_p_3_ns\n"
  "str   r1,[%0,#12]\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   write the PAC key used for privileged mode (non-secure)
  \details writes the given PAC key to the non-secure PAC_KEY_P registers when in secure mode.
  \param [in]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __TZ_set_PAC_KEY_P_NS (uint32_t* pPacKey) {
  __ASM volatile (
  "ldr   r1,[%0,#0]\n"
  "msr   pac_key_p_0_ns, r1\n"
  "ldr   r1,[%0,#4]\n"
  "msr   pac_key_p_1_ns, r1\n"
  "ldr   r1,[%0,#8]\n"
  "msr   pac_key_p_2_ns, r1\n"
  "ldr   r1,[%0,#12]\n"
  "msr   pac_key_p_3_ns, r1\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   read the PAC key used for unprivileged mode (non-secure)
  \details Reads the PAC key stored in the non-secure PAC_KEY_U registers when in secure mode.
  \param [out]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __TZ_get_PAC_KEY_U_NS (uint32_t* pPacKey) {
  __ASM volatile (
  "mrs   r1, pac_key_u_0_ns\n"
  "str   r1,[%0,#0]\n"
  "mrs   r1, pac_key_u_1_ns\n"
  "str   r1,[%0,#4]\n"
  "mrs   r1, pac_key_u_2_ns\n"
  "str   r1,[%0,#8]\n"
  "mrs   r1, pac_key_u_3_ns\n"
  "str   r1,[%0,#12]\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

/**
  \brief   write the PAC key used for unprivileged mode (non-secure)
  \details writes the given PAC key to the non-secure PAC_KEY_U registers when in secure mode.
  \param [in]    pPacKey  128bit PAC key
 */
__STATIC_FORCEINLINE void __TZ_set_PAC_KEY_U_NS (uint32_t* pPacKey) {
  __ASM volatile (
  "ldr   r1,[%0,#0]\n"
  "msr   pac_key_u_0_ns, r1\n"
  "ldr   r1,[%0,#4]\n"
  "msr   pac_key_u_1_ns, r1\n"
  "ldr   r1,[%0,#8]\n"
  "msr   pac_key_u_2_ns, r1\n"
  "ldr   r1,[%0,#12]\n"
  "msr   pac_key_u_3_ns, r1\n"
  : : "r" (pPacKey) : "memory", "r1"
  );
}

#endif /* (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3)) */

#endif /* (defined (__ARM_FEATURE_PAUTH) && (__ARM_FEATURE_PAUTH == 1)) */

/*@} end of CMSIS_Core_PacKeyFunctions */


#endif /* PAC_ARMV81_H */
