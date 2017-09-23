/**************************************************************************//**
 * @file     core_cmInstr.h
 * @brief    CMSIS Cortex-M Core Instruction Access Header File
 * @version  V4.00
 * @date     28. August 2014
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2009 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef __CORE_CMINSTR_H
#define __CORE_CMINSTR_H


/* ##########################  Core Instruction Access  ######################### */
/** \defgroup CMSIS_Core_InstructionInterface CMSIS Core Instruction Interface
  Access to dedicated instructions
  @{
*/

#if   defined ( __CC_ARM ) /*------------------RealView Compiler -----------------*/
/* ARM armcc specific functions */

#if (__ARMCC_VERSION < 400677)
  #error "Please use ARM Compiler Toolchain V4.0.677 or later!"
#endif


/** \brief  No Operation

    No Operation does nothing. This instruction can be used for code alignment purposes.
 */
#define __NOP                             __nop


/** \brief  Wait For Interrupt

    Wait For Interrupt is a hint instruction that suspends execution
    until one of a number of events occurs.
 */
#define __WFI                             __wfi


/** \brief  Wait For Event

    Wait For Event is a hint instruction that permits the processor to enter
    a low-power state until one of a number of events occurs.
 */
#define __WFE                             __wfe


/** \brief  Send Event

    Send Event is a hint instruction. It causes an event to be signaled to the CPU.
 */
#define __SEV                             __sev


/** \brief  Instruction Synchronization Barrier

    Instruction Synchronization Barrier flushes the pipeline in the processor,
    so that all instructions following the ISB are fetched from cache or
    memory, after the instruction has been completed.
 */
#define __ISB()                           __isb(0xF)


/** \brief  Data Synchronization Barrier

    This function acts as a special kind of Data Memory Barrier.
    It completes when all explicit memory accesses before this instruction complete.
 */
#define __DSB()                           __dsb(0xF)


/** \brief  Data Memory Barrier

    This function ensures the apparent order of the explicit memory operations before
    and after the instruction, without ensuring their completion.
 */
#define __DMB()                           __dmb(0xF)


/** \brief  Reverse byte order (32 bit)

    This function reverses the byte order in integer value.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
#define __REV                             __rev


/** \brief  Reverse byte order (16 bit)

    This function reverses the byte order in two unsigned short values.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
#ifndef __NO_EMBEDDED_ASM
__attribute__((section(".rev16_text"))) __STATIC_INLINE __ASM uint32_t __REV16(uint32_t value)
{
  rev16 r0, r0
  bx lr
}
#endif

/** \brief  Reverse byte order in signed short value

    This function reverses the byte order in a signed short value with sign extension to integer.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
#ifndef __NO_EMBEDDED_ASM
__attribute__((section(".revsh_text"))) __STATIC_INLINE __ASM int32_t __REVSH(int32_t value)
{
  revsh r0, r0
  bx lr
}
#endif


/** \brief  Rotate Right in unsigned value (32 bit)

    This function Rotate Right (immediate) provides the value of the contents of a register rotated by a variable number of bits.

    \param [in]    value  Value to rotate
    \param [in]    value  Number of Bits to rotate
    \return               Rotated value
 */
#define __ROR                             __ror


/** \brief  Breakpoint

    This function causes the processor to enter Debug state.
    Debug tools can use this to investigate system state when the instruction at a particular address is reached.

    \param [in]    value  is ignored by the processor.
                   If required, a debugger can use it to store additional information about the breakpoint.
 */
#define __BKPT(value)                       __breakpoint(value)


#if       (__CORTEX_M >= 0x03) || (__CORTEX_SC >= 300)

/** \brief  Reverse bit order of value

    This function reverses the bit order of the given value.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
#define __RBIT                            __rbit


/** \brief  LDR Exclusive (8 bit)

    This function executes a exclusive LDR instruction for 8 bit value.

    \param [in]    ptr  Pointer to data
    \return             value of type uint8_t at (*ptr)
 */
#define __LDREXB(ptr)                     ((uint8_t ) __ldrex(ptr))


/** \brief  LDR Exclusive (16 bit)

    This function executes a exclusive LDR instruction for 16 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint16_t at (*ptr)
 */
#define __LDREXH(ptr)                     ((uint16_t) __ldrex(ptr))


/** \brief  LDR Exclusive (32 bit)

    This function executes a exclusive LDR instruction for 32 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint32_t at (*ptr)
 */
#define __LDREXW(ptr)                     ((uint32_t ) __ldrex(ptr))


/** \brief  STR Exclusive (8 bit)

    This function executes a exclusive STR instruction for 8 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
#define __STREXB(value, ptr)              __strex(value, ptr)


/** \brief  STR Exclusive (16 bit)

    This function executes a exclusive STR instruction for 16 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
#define __STREXH(value, ptr)              __strex(value, ptr)


/** \brief  STR Exclusive (32 bit)

    This function executes a exclusive STR instruction for 32 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
#define __STREXW(value, ptr)              __strex(value, ptr)


/** \brief  Remove the exclusive lock

    This function removes the exclusive lock which is created by LDREX.

 */
#define __CLREX                           __clrex


/** \brief  Signed Saturate

    This function saturates a signed value.

    \param [in]  value  Value to be saturated
    \param [in]    sat  Bit position to saturate to (1..32)
    \return             Saturated value
 */
#define __SSAT                            __ssat


/** \brief  Unsigned Saturate

    This function saturates an unsigned value.

    \param [in]  value  Value to be saturated
    \param [in]    sat  Bit position to saturate to (0..31)
    \return             Saturated value
 */
#define __USAT                            __usat


/** \brief  Count leading zeros

    This function counts the number of leading zeros of a data value.

    \param [in]  value  Value to count the leading zeros
    \return             number of leading zeros in value
 */
#define __CLZ                             __clz


/** \brief  Rotate Right with Extend (32 bit)

    This function moves each bit of a bitstring right by one bit. The carry input is shifted in at the left end of the bitstring.

    \param [in]    value  Value to rotate
    \return               Rotated value
 */
#ifndef __NO_EMBEDDED_ASM
__attribute__((section(".rrx_text"))) __STATIC_INLINE __ASM uint32_t __RRX(uint32_t value)
{
  rrx r0, r0
  bx lr
}
#endif


/** \brief  LDRT Unprivileged (8 bit)

    This function executes a Unprivileged LDRT instruction for 8 bit value.

    \param [in]    ptr  Pointer to data
    \return             value of type uint8_t at (*ptr)
 */
#define __LDRBT(ptr)                      ((uint8_t )  __ldrt(ptr))


/** \brief  LDRT Unprivileged (16 bit)

    This function executes a Unprivileged LDRT instruction for 16 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint16_t at (*ptr)
 */
#define __LDRHT(ptr)                      ((uint16_t)  __ldrt(ptr))


/** \brief  LDRT Unprivileged (32 bit)

    This function executes a Unprivileged LDRT instruction for 32 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint32_t at (*ptr)
 */
#define __LDRT(ptr)                       ((uint32_t ) __ldrt(ptr))


/** \brief  STRT Unprivileged (8 bit)

    This function executes a Unprivileged STRT instruction for 8 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
#define __STRBT(value, ptr)               __strt(value, ptr)


/** \brief  STRT Unprivileged (16 bit)

    This function executes a Unprivileged STRT instruction for 16 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
#define __STRHT(value, ptr)               __strt(value, ptr)


/** \brief  STRT Unprivileged (32 bit)

    This function executes a Unprivileged STRT instruction for 32 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
#define __STRT(value, ptr)                __strt(value, ptr)

#endif /* (__CORTEX_M >= 0x03) || (__CORTEX_SC >= 300) */


#elif defined ( __GNUC__ ) /*------------------ GNU Compiler ---------------------*/
/* GNU gcc specific functions */

/* Define macros for porting to both thumb1 and thumb2.
 * For thumb1, use low register (r0-r7), specified by constrant "l"
 * Otherwise, use general registers, specified by constrant "r" */
#if defined (__thumb__) && !defined (__thumb2__)
#define __CMSIS_GCC_OUT_REG(r) "=l" (r)
#define __CMSIS_GCC_USE_REG(r) "l" (r)
#else
#define __CMSIS_GCC_OUT_REG(r) "=r" (r)
#define __CMSIS_GCC_USE_REG(r) "r" (r)
#endif

/** \brief  No Operation

    No Operation does nothing. This instruction can be used for code alignment purposes.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __NOP(void)
{
  __ASM volatile ("nop");
}


/** \brief  Wait For Interrupt

    Wait For Interrupt is a hint instruction that suspends execution
    until one of a number of events occurs.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __WFI(void)
{
  __ASM volatile ("wfi");
}


/** \brief  Wait For Event

    Wait For Event is a hint instruction that permits the processor to enter
    a low-power state until one of a number of events occurs.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __WFE(void)
{
  __ASM volatile ("wfe");
}


/** \brief  Send Event

    Send Event is a hint instruction. It causes an event to be signaled to the CPU.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __SEV(void)
{
  __ASM volatile ("sev");
}


/** \brief  Instruction Synchronization Barrier

    Instruction Synchronization Barrier flushes the pipeline in the processor,
    so that all instructions following the ISB are fetched from cache or
    memory, after the instruction has been completed.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __ISB(void)
{
  __ASM volatile ("isb");
}


/** \brief  Data Synchronization Barrier

    This function acts as a special kind of Data Memory Barrier.
    It completes when all explicit memory accesses before this instruction complete.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __DSB(void)
{
  __ASM volatile ("dsb");
}


/** \brief  Data Memory Barrier

    This function ensures the apparent order of the explicit memory operations before
    and after the instruction, without ensuring their completion.
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __DMB(void)
{
  __ASM volatile ("dmb");
}


/** \brief  Reverse byte order (32 bit)

    This function reverses the byte order in integer value.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;

  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}


/** \brief  Reverse byte order (16 bit)

    This function reverses the byte order in two unsigned short values.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
  uint32_t result;

  __ASM volatile ("rev16 %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
}


/** \brief  Reverse byte order in signed short value

    This function reverses the byte order in a signed short value with sign extension to integer.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE int32_t __REVSH(int32_t value)
{
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  return (short)__builtin_bswap16(value);
#else
  uint32_t result;

  __ASM volatile ("revsh %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}


/** \brief  Rotate Right in unsigned value (32 bit)

    This function Rotate Right (immediate) provides the value of the contents of a register rotated by a variable number of bits.

    \param [in]    value  Value to rotate
    \param [in]    value  Number of Bits to rotate
    \return               Rotated value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  return (op1 >> op2) | (op1 << (32 - op2)); 
}


/** \brief  Breakpoint

    This function causes the processor to enter Debug state.
    Debug tools can use this to investigate system state when the instruction at a particular address is reached.

    \param [in]    value  is ignored by the processor.
                   If required, a debugger can use it to store additional information about the breakpoint.
 */
#define __BKPT(value)                       __ASM volatile ("bkpt "#value)


#if       (__CORTEX_M >= 0x03) || (__CORTEX_SC >= 300)

/** \brief  Reverse bit order of value

    This function reverses the bit order of the given value.

    \param [in]    value  Value to reverse
    \return               Reversed value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __RBIT(uint32_t value)
{
  uint32_t result;

   __ASM volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
   return(result);
}


/** \brief  LDR Exclusive (8 bit)

    This function executes a exclusive LDR instruction for 8 bit value.

    \param [in]    ptr  Pointer to data
    \return             value of type uint8_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint8_t __LDREXB(volatile uint8_t *addr)
{
    uint32_t result;

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
   __ASM volatile ("ldrexb %0, %1" : "=r" (result) : "Q" (*addr) );
#else
    /* Prior to GCC 4.8, "Q" will be expanded to [rx, #0] which is not
       accepted by assembler. So has to use following less efficient pattern.
    */
   __ASM volatile ("ldrexb %0, [%1]" : "=r" (result) : "r" (addr) : "memory" );
#endif
   return ((uint8_t) result);    /* Add explicit type cast here */
}


/** \brief  LDR Exclusive (16 bit)

    This function executes a exclusive LDR instruction for 16 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint16_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint16_t __LDREXH(volatile uint16_t *addr)
{
    uint32_t result;

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
   __ASM volatile ("ldrexh %0, %1" : "=r" (result) : "Q" (*addr) );
#else
    /* Prior to GCC 4.8, "Q" will be expanded to [rx, #0] which is not
       accepted by assembler. So has to use following less efficient pattern.
    */
   __ASM volatile ("ldrexh %0, [%1]" : "=r" (result) : "r" (addr) : "memory" );
#endif
   return ((uint16_t) result);    /* Add explicit type cast here */
}


/** \brief  LDR Exclusive (32 bit)

    This function executes a exclusive LDR instruction for 32 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint32_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __LDREXW(volatile uint32_t *addr)
{
    uint32_t result;

   __ASM volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
   return(result);
}


/** \brief  STR Exclusive (8 bit)

    This function executes a exclusive STR instruction for 8 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __STREXB(uint8_t value, volatile uint8_t *addr)
{
   uint32_t result;

   __ASM volatile ("strexb %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" ((uint32_t)value) );
   return(result);
}


/** \brief  STR Exclusive (16 bit)

    This function executes a exclusive STR instruction for 16 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __STREXH(uint16_t value, volatile uint16_t *addr)
{
   uint32_t result;

   __ASM volatile ("strexh %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" ((uint32_t)value) );
   return(result);
}


/** \brief  STR Exclusive (32 bit)

    This function executes a exclusive STR instruction for 32 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __STREXW(uint32_t value, volatile uint32_t *addr)
{
   uint32_t result;

   __ASM volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
   return(result);
}


/** \brief  Remove the exclusive lock

    This function removes the exclusive lock which is created by LDREX.

 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __CLREX(void)
{
  __ASM volatile ("clrex" ::: "memory");
}


/** \brief  Signed Saturate

    This function saturates a signed value.

    \param [in]  value  Value to be saturated
    \param [in]    sat  Bit position to saturate to (1..32)
    \return             Saturated value
 */
#define __SSAT(ARG1,ARG2) \
({                          \
  uint32_t __RES, __ARG1 = (ARG1); \
  __ASM ("ssat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) ); \
  __RES; \
 })


/** \brief  Unsigned Saturate

    This function saturates an unsigned value.

    \param [in]  value  Value to be saturated
    \param [in]    sat  Bit position to saturate to (0..31)
    \return             Saturated value
 */
#define __USAT(ARG1,ARG2) \
({                          \
  uint32_t __RES, __ARG1 = (ARG1); \
  __ASM ("usat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) ); \
  __RES; \
 })


/** \brief  Count leading zeros

    This function counts the number of leading zeros of a data value.

    \param [in]  value  Value to count the leading zeros
    \return             number of leading zeros in value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint8_t __CLZ(uint32_t value)
{
  uint32_t result;

  __ASM volatile ("clz %0, %1" : "=r" (result) : "r" (value) );
   return ((uint8_t) result);    /* Add explicit type cast here */
}


/** \brief  Rotate Right with Extend (32 bit)

    This function moves each bit of a bitstring right by one bit. The carry input is shifted in at the left end of the bitstring.

    \param [in]    value  Value to rotate
    \return               Rotated value
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __RRX(uint32_t value)
{
  uint32_t result;

  __ASM volatile ("rrx %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
}


/** \brief  LDRT Unprivileged (8 bit)

    This function executes a Unprivileged LDRT instruction for 8 bit value.

    \param [in]    ptr  Pointer to data
    \return             value of type uint8_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint8_t __LDRBT(volatile uint8_t *addr)
{
    uint32_t result;

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
   __ASM volatile ("ldrbt %0, %1" : "=r" (result) : "Q" (*addr) );
#else
    /* Prior to GCC 4.8, "Q" will be expanded to [rx, #0] which is not
       accepted by assembler. So has to use following less efficient pattern.
    */
   __ASM volatile ("ldrbt %0, [%1]" : "=r" (result) : "r" (addr) : "memory" );
#endif
   return ((uint8_t) result);    /* Add explicit type cast here */
}


/** \brief  LDRT Unprivileged (16 bit)

    This function executes a Unprivileged LDRT instruction for 16 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint16_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint16_t __LDRHT(volatile uint16_t *addr)
{
    uint32_t result;

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
   __ASM volatile ("ldrht %0, %1" : "=r" (result) : "Q" (*addr) );
#else
    /* Prior to GCC 4.8, "Q" will be expanded to [rx, #0] which is not
       accepted by assembler. So has to use following less efficient pattern.
    */
   __ASM volatile ("ldrht %0, [%1]" : "=r" (result) : "r" (addr) : "memory" );
#endif
   return ((uint16_t) result);    /* Add explicit type cast here */
}


/** \brief  LDRT Unprivileged (32 bit)

    This function executes a Unprivileged LDRT instruction for 32 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint32_t at (*ptr)
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __LDRT(volatile uint32_t *addr)
{
    uint32_t result;

   __ASM volatile ("ldrt %0, %1" : "=r" (result) : "Q" (*addr) );
   return(result);
}


/** \brief  STRT Unprivileged (8 bit)

    This function executes a Unprivileged STRT instruction for 8 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __STRBT(uint8_t value, volatile uint8_t *addr)
{
   __ASM volatile ("strbt %1, %0" : "=Q" (*addr) : "r" ((uint32_t)value) );
}


/** \brief  STRT Unprivileged (16 bit)

    This function executes a Unprivileged STRT instruction for 16 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __STRHT(uint16_t value, volatile uint16_t *addr)
{
   __ASM volatile ("strht %1, %0" : "=Q" (*addr) : "r" ((uint32_t)value) );
}


/** \brief  STRT Unprivileged (32 bit)

    This function executes a Unprivileged STRT instruction for 32 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
 */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __STRT(uint32_t value, volatile uint32_t *addr)
{
   __ASM volatile ("strt %1, %0" : "=Q" (*addr) : "r" (value) );
}

#endif /* (__CORTEX_M >= 0x03) || (__CORTEX_SC >= 300) */


#elif defined ( __ICCARM__ ) /*------------------ ICC Compiler -------------------*/
/* IAR iccarm specific functions */
#include <cmsis_iar.h>


#elif defined ( __TMS470__ ) /*---------------- TI CCS Compiler ------------------*/
/* TI CCS specific functions */
#include <cmsis_ccs.h>


#elif defined ( __TASKING__ ) /*------------------ TASKING Compiler --------------*/
/* TASKING carm specific functions */
/*
 * The CMSIS functions have been implemented as intrinsics in the compiler.
 * Please use "carm -?i" to get an up to date list of all intrinsics,
 * Including the CMSIS ones.
 */


#elif defined ( __CSMC__ ) /*------------------ COSMIC Compiler -------------------*/
/* Cosmic specific functions */
#include <cmsis_csm.h>

#endif

/*@}*/ /* end of group CMSIS_Core_InstructionInterface */

#endif /* __CORE_CMINSTR_H */
