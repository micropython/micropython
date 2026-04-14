/*
 * Copyright (c) 2009-2017 ARM Limited. All rights reserved.
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
 * CMSIS-Core(A) Compiler Specific Macros, Functions, Instructions
 */

#ifndef __CMSIS_CP15_H
#define __CMSIS_CP15_H

#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header    /* treat file as system include file */
#endif

/** \brief  Get ACTLR
    \return               Auxiliary Control register value
 */
__STATIC_FORCEINLINE uint32_t __get_ACTLR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 1, 0, 1);
  return(result);
}

/** \brief  Set ACTLR
    \param [in]    actlr  Auxiliary Control value to set
 */
__STATIC_FORCEINLINE void __set_ACTLR(uint32_t actlr)
{
  __set_CP(15, 0, actlr, 1, 0, 1);
}

/** \brief  Get CPACR
    \return               Coprocessor Access Control register value
 */
__STATIC_FORCEINLINE uint32_t __get_CPACR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 1, 0, 2);
  return result;
}

/** \brief  Set CPACR
    \param [in]    cpacr  Coprocessor Access Control value to set
 */
__STATIC_FORCEINLINE void __set_CPACR(uint32_t cpacr)
{
  __set_CP(15, 0, cpacr, 1, 0, 2);
}

/** \brief  Get DFSR
    \return               Data Fault Status Register value
 */
__STATIC_FORCEINLINE uint32_t __get_DFSR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 5, 0, 0);
  return result;
}

/** \brief  Set DFSR
    \param [in]    dfsr  Data Fault Status value to set
 */
__STATIC_FORCEINLINE void __set_DFSR(uint32_t dfsr)
{
  __set_CP(15, 0, dfsr, 5, 0, 0);
}

/** \brief  Get IFSR
    \return               Instruction Fault Status Register value
 */
__STATIC_FORCEINLINE uint32_t __get_IFSR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 5, 0, 1);
  return result;
}

/** \brief  Set IFSR
    \param [in]    ifsr  Instruction Fault Status value to set
 */
__STATIC_FORCEINLINE void __set_IFSR(uint32_t ifsr)
{
  __set_CP(15, 0, ifsr, 5, 0, 1);
}

/** \brief  Get ISR
    \return               Interrupt Status Register value
 */
__STATIC_FORCEINLINE uint32_t __get_ISR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 12, 1, 0);
  return result;
}

/** \brief  Get CBAR
    \return               Configuration Base Address register value
 */
__STATIC_FORCEINLINE uint32_t __get_CBAR(void)
{
  uint32_t result;
  __get_CP(15, 4, result, 15, 0, 0);
  return result;
}

/** \brief  Get TTBR0

    This function returns the value of the Translation Table Base Register 0.

    \return               Translation Table Base Register 0 value
 */
__STATIC_FORCEINLINE uint32_t __get_TTBR0(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 2, 0, 0);
  return result;
}

/** \brief  Set TTBR0

    This function assigns the given value to the Translation Table Base Register 0.

    \param [in]    ttbr0  Translation Table Base Register 0 value to set
 */
__STATIC_FORCEINLINE void __set_TTBR0(uint32_t ttbr0)
{
  __set_CP(15, 0, ttbr0, 2, 0, 0);
}

/** \brief  Get DACR

    This function returns the value of the Domain Access Control Register.

    \return               Domain Access Control Register value
 */
__STATIC_FORCEINLINE uint32_t __get_DACR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 3, 0, 0);
  return result;
}

/** \brief  Set DACR

    This function assigns the given value to the Domain Access Control Register.

    \param [in]    dacr   Domain Access Control Register value to set
 */
__STATIC_FORCEINLINE void __set_DACR(uint32_t dacr)
{
  __set_CP(15, 0, dacr, 3, 0, 0);
}

/** \brief  Set SCTLR

    This function assigns the given value to the System Control Register.

    \param [in]    sctlr  System Control Register value to set
 */
__STATIC_FORCEINLINE void __set_SCTLR(uint32_t sctlr)
{
  __set_CP(15, 0, sctlr, 1, 0, 0);
}

/** \brief  Get SCTLR
    \return               System Control Register value
 */
__STATIC_FORCEINLINE uint32_t __get_SCTLR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 1, 0, 0);
  return result;
}

/** \brief  Get MPIDR

    This function returns the value of the Multiprocessor Affinity Register.

    \return               Multiprocessor Affinity Register value
 */
__STATIC_FORCEINLINE uint32_t __get_MPIDR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 0, 0, 5);
  return result;
}

/** \brief  Get VBAR

    This function returns the value of the Vector Base Address Register.

    \return               Vector Base Address Register
 */
__STATIC_FORCEINLINE uint32_t __get_VBAR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 12, 0, 0);
  return result;
}

/** \brief  Set VBAR

    This function assigns the given value to the Vector Base Address Register.

    \param [in]    vbar  Vector Base Address Register value to set
 */
__STATIC_FORCEINLINE void __set_VBAR(uint32_t vbar)
{
  __set_CP(15, 0, vbar, 12, 0, 0);
}

/** \brief  Get MVBAR

    This function returns the value of the Monitor Vector Base Address Register.

    \return               Monitor Vector Base Address Register
 */
__STATIC_FORCEINLINE uint32_t __get_MVBAR(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 12, 0, 1);
  return result;
}

/** \brief  Set MVBAR

    This function assigns the given value to the Monitor Vector Base Address Register.

    \param [in]    mvbar  Monitor Vector Base Address Register value to set
 */
__STATIC_FORCEINLINE void __set_MVBAR(uint32_t mvbar)
{
  __set_CP(15, 0, mvbar, 12, 0, 1);
}

#if (defined(__TIM_PRESENT) && (__TIM_PRESENT == 1U)) || \
    defined(DOXYGEN)

/** \brief  Set CNTFRQ

  This function assigns the given value to PL1 Physical Timer Counter Frequency Register (CNTFRQ).

  \param [in]    value  CNTFRQ Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTFRQ(uint32_t value)
{
  __set_CP(15, 0, value, 14, 0, 0);
}

/** \brief  Get CNTFRQ

    This function returns the value of the PL1 Physical Timer Counter Frequency Register (CNTFRQ).

    \return               CNTFRQ Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CNTFRQ(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 0 , 0);
  return result;
}

/** \brief  Set CNTP_TVAL

  This function assigns the given value to PL1 Physical Timer Value Register (CNTP_TVAL).

  \param [in]    value  CNTP_TVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_TVAL(uint32_t value)
{
  __set_CP(15, 0, value, 14, 2, 0);
}

/** \brief  Get CNTP_TVAL

    This function returns the value of the PL1 Physical Timer Value Register (CNTP_TVAL).

    \return               CNTP_TVAL Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CNTP_TVAL(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 2, 0);
  return result;
}

/** \brief  Get CNTPCT

    This function returns the value of the 64 bits PL1 Physical Count Register (CNTPCT).

    \return               CNTPCT Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTPCT(void)
{
  uint64_t result;
  __get_CP64(15, 0, result, 14);
  return result;
}

/** \brief  Set CNTP_CVAL

  This function assigns the given value to 64bits PL1 Physical Timer CompareValue Register (CNTP_CVAL).

  \param [in]    value  CNTP_CVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_CVAL(uint64_t value)
{
  __set_CP64(15, 2, value, 14);
}

/** \brief  Get CNTP_CVAL

    This function returns the value of the 64 bits PL1 Physical Timer CompareValue Register (CNTP_CVAL).

    \return               CNTP_CVAL Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTP_CVAL(void)
{
  uint64_t result;
  __get_CP64(15, 2, result, 14);
  return result;
}

/** \brief  Set CNTP_CTL

  This function assigns the given value to PL1 Physical Timer Control Register (CNTP_CTL).

  \param [in]    value  CNTP_CTL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_CTL(uint32_t value)
{
  __set_CP(15, 0, value, 14, 2, 1);
}

/** \brief  Get CNTP_CTL register
    \return               CNTP_CTL Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CNTP_CTL(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 2, 1);
  return result;
}

/******************************* VIRTUAL TIMER *******************************/
/** see [ARM DDI 0406C.d] :
    . §B4.1.31 "CNTV_CTL, Counter-timer Virtual Timer Control register"
    . §B4.1.32 "CNTV_CVAL, Counter-timer Virtual Timer CompareValue register"
    . §B4.1.33 "CNTV_TVAL, Counter-timer Virtual Timer TimerValue register"
    . §B4.1.34 "CNTVCT, Counter-timer Virtual Count register"
**/
/** \brief  Set CNTV_TVAL
  This function assigns the given value to VL1 Virtual Timer Value Register (CNTV_TVAL).
  \param [in]    value  CNTV_TVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTV_TVAL(uint32_t value)
{
  __set_CP(15, 0, value, 14, 3, 0);
}

/** \brief  Get CNTV_TVAL
    This function returns the value of the VL1 Virtual Timer Value Register (CNTV_TVAL).
    \return               CNTV_TVAL Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CNTV_TVAL(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 3, 0);
  return result;
}

/** \brief  Get CNTVCT
    This function returns the value of the 64 bits VL1 Virtual Count Register (CNTVCT).
    \return               CNTVCT Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTVCT(void)
{
  uint64_t result;
  __get_CP64(15, 1, result, 14);
  return result;
}

/** \brief  Set CNTV_CVAL
  This function assigns the given value to 64bits VL1 Virtual Timer CompareValue Register (CNTV_CVAL).
  \param [in]    value  CNTV_CVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTV_CVAL(uint64_t value)
{
  __set_CP64(15, 3, value, 14);
}

/** \brief  Get CNTV_CVAL
    This function returns the value of the 64 bits VL1 Virtual Timer CompareValue Register (CNTV_CVAL).
    \return               CNTV_CVAL Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTV_CVAL(void)
{
  uint64_t result;
  __get_CP64(15, 3, result, 14);
  return result;
}

/** \brief  Set CNTV_CTL
  This function assigns the given value to VL1 Virtual Timer Control Register (CNTV_CTL).
  \param [in]    value  CNTV_CTL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTV_CTL(uint32_t value)
{
  __set_CP(15, 0, value, 14, 3, 1);
}

/** \brief  Get CNTV_CTL register
    \return               CNTV_CTL Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CNTV_CTL(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 3, 1);
  return result;
}

/***************************** VIRTUAL TIMER END *****************************/
#endif

/** \brief  Set TLBIALL

  TLB Invalidate All
 */
__STATIC_FORCEINLINE void __set_TLBIALL(uint32_t value)
{
  __set_CP(15, 0, value, 8, 7, 0);
}

/** \brief  Set BPIALL.

  Branch Predictor Invalidate All
 */
__STATIC_FORCEINLINE void __set_BPIALL(uint32_t value)
{
  __set_CP(15, 0, value, 7, 5, 6);
}

/** \brief  Set ICIALLU

  Instruction Cache Invalidate All
 */
__STATIC_FORCEINLINE void __set_ICIALLU(uint32_t value)
{
  __set_CP(15, 0, value, 7, 5, 0);
}

/** \brief  Set ICIMVAC

  Instruction Cache Invalidate
 */
__STATIC_FORCEINLINE void __set_ICIMVAC(uint32_t value)
{
  __set_CP(15, 0, value, 7, 5, 1);
}

/** \brief  Set DCCMVAC

  Data cache clean
 */
__STATIC_FORCEINLINE void __set_DCCMVAC(uint32_t value)
{
  __set_CP(15, 0, value, 7, 10, 1);
}

/** \brief  Set DCIMVAC

  Data cache invalidate
 */
__STATIC_FORCEINLINE void __set_DCIMVAC(uint32_t value)
{
  __set_CP(15, 0, value, 7, 6, 1);
}

/** \brief  Set DCCIMVAC

  Data cache clean and invalidate
 */
__STATIC_FORCEINLINE void __set_DCCIMVAC(uint32_t value)
{
  __set_CP(15, 0, value, 7, 14, 1);
}

/** \brief  Set CSSELR
 */
__STATIC_FORCEINLINE void __set_CSSELR(uint32_t value)
{
  __set_CP(15, 2, value, 0, 0, 0);
}

/** \brief  Get CSSELR
    \return CSSELR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CSSELR(void)
{
  uint32_t result;
  __get_CP(15, 2, result, 0, 0, 0);
  return result;
}

/** \brief  Get CCSIDR
    \return CCSIDR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CCSIDR(void)
{
  uint32_t result;
  __get_CP(15, 1, result, 0, 0, 0);
  return result;
}

/** \brief  Get CLIDR
    \return CLIDR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CLIDR(void)
{
  uint32_t result;
  __get_CP(15, 1, result, 0, 0, 1);
  return result;
}

/** \brief  Set DCISW
 */
__STATIC_FORCEINLINE void __set_DCISW(uint32_t value)
{
  __set_CP(15, 0, value, 7, 6, 2);
}

/** \brief  Set DCCSW
 */
__STATIC_FORCEINLINE void __set_DCCSW(uint32_t value)
{
  __set_CP(15, 0, value, 7, 10, 2);
}

/** \brief  Set DCCISW
 */
__STATIC_FORCEINLINE void __set_DCCISW(uint32_t value)
{
  __set_CP(15, 0, value, 7, 14, 2);
}

#endif
