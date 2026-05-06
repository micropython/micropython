/*
 * Copyright (c) 2020-2021 Arm Limited. All rights reserved.
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
 * CMSIS-Core(M) Level 1 Cache API for Armv7-M and later
 */

#ifndef ARM_ARMV7M_CACHEL1_H
#define ARM_ARMV7M_CACHEL1_H

#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header    /* treat file as system include file */
#endif

/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_CacheFunctions Cache Functions
  \brief    Functions that configure Instruction and Data cache.
  @{
 */

/* Cache Size ID Register Macros */
#define CCSIDR_WAYS(x)         (((x) & SCB_CCSIDR_ASSOCIATIVITY_Msk) >> SCB_CCSIDR_ASSOCIATIVITY_Pos)
#define CCSIDR_SETS(x)         (((x) & SCB_CCSIDR_NUMSETS_Msk      ) >> SCB_CCSIDR_NUMSETS_Pos      )

#ifndef __SCB_DCACHE_LINE_SIZE
#define __SCB_DCACHE_LINE_SIZE  32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
#endif

#ifndef __SCB_ICACHE_LINE_SIZE
#define __SCB_ICACHE_LINE_SIZE  32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
#endif

/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
__STATIC_FORCEINLINE void SCB_EnableICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    if (SCB->CCR & SCB_CCR_IC_Msk) return;  /* return if ICache is already enabled */

    __DSB();
    __ISB();
    SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB->CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
__STATIC_FORCEINLINE void SCB_DisableICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    SCB->CCR &= ~(uint32_t)SCB_CCR_IC_Msk;  /* disable I-Cache */
    SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
__STATIC_FORCEINLINE void SCB_InvalidateICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    SCB->ICIALLU = 0UL;
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   I-Cache Invalidate by address
  \details Invalidates I-Cache for the given address.
           I-Cache is invalidated starting from a 32 byte aligned address in 32 byte granularity.
           I-Cache memory blocks which are part of given address + given size are invalidated.
  \param[in]   addr    address
  \param[in]   isize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE void SCB_InvalidateICache_by_Addr (volatile void *addr, int32_t isize)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    if ( isize > 0 ) {
       int32_t op_size = isize + (((uint32_t)addr) & (__SCB_ICACHE_LINE_SIZE - 1U));
      uint32_t op_addr = (uint32_t)addr /* & ~(__SCB_ICACHE_LINE_SIZE - 1U) */;

      __DSB();

      do {
        SCB->ICIMVAU = op_addr;             /* register accepts only 32byte aligned values, only bits 31..5 are valid */
        op_addr += __SCB_ICACHE_LINE_SIZE;
        op_size -= __SCB_ICACHE_LINE_SIZE;
      } while ( op_size > 0 );

      __DSB();
      __ISB();
    }
  #endif
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  */
__STATIC_FORCEINLINE void SCB_EnableDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    if (SCB->CCR & SCB_CCR_DC_Msk) return;  /* return if DCache is already enabled */

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    __DSB();

    SCB->CCR |=  (uint32_t)SCB_CCR_DC_Msk;  /* enable D-Cache */

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  */
__STATIC_FORCEINLINE void SCB_DisableDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    struct {
      uint32_t ccsidr;
      uint32_t sets;
      uint32_t ways;
    } locals
    #if ((defined(__GNUC__) || defined(__clang__)) && !defined(__OPTIMIZE__))
       __ALIGNED(__SCB_DCACHE_LINE_SIZE)
    #endif
    ;

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    SCB->CCR &= ~(uint32_t)SCB_CCR_DC_Msk;  /* disable D-Cache */
    __DSB();

    #if !defined(__OPTIMIZE__)
      /*
       * For the endless loop issue with no optimization builds.
       * More details, see https://github.com/ARM-software/CMSIS_5/issues/620
       *
       * The issue only happens when local variables are in stack. If
       * local variables are saved in general purpose register, then the function
       * is OK.
       *
       * When local variables are in stack, after disabling the cache, flush the
       * local variables cache line for data consistency.
       */
      /* Clean and invalidate the local variable cache. */
    #if defined(__ICCARM__)
    /* As we can't align the stack to the cache line size, invalidate each of the variables */
      SCB->DCCIMVAC = (uint32_t)&locals.sets;
      SCB->DCCIMVAC = (uint32_t)&locals.ways;
      SCB->DCCIMVAC = (uint32_t)&locals.ccsidr;
    #else
      SCB->DCCIMVAC = (uint32_t)&locals;
    #endif
      __DSB();
      __ISB();
    #endif

    locals.ccsidr = SCB->CCSIDR;
                                            /* clean & invalidate D-Cache */
    locals.sets = (uint32_t)(CCSIDR_SETS(locals.ccsidr));
    do {
      locals.ways = (uint32_t)(CCSIDR_WAYS(locals.ccsidr));
      do {
        SCB->DCCISW = (((locals.sets << SCB_DCCISW_SET_Pos) & SCB_DCCISW_SET_Msk) |
                       ((locals.ways << SCB_DCCISW_WAY_Pos) & SCB_DCCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (locals.ways-- != 0U);
    } while(locals.sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  */
__STATIC_FORCEINLINE void SCB_InvalidateDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  */
__STATIC_FORCEINLINE void SCB_CleanDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* clean D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCCSW = (((sets << SCB_DCCSW_SET_Pos) & SCB_DCCSW_SET_Msk) |
                      ((ways << SCB_DCCSW_WAY_Pos) & SCB_DCCSW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  */
__STATIC_FORCEINLINE void SCB_CleanInvalidateDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* clean & invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCCISW = (((sets << SCB_DCCISW_SET_Pos) & SCB_DCCISW_SET_Msk) |
                       ((ways << SCB_DCCISW_WAY_Pos) & SCB_DCCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address.
           D-Cache is invalidated starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are invalidated.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE void SCB_InvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (__SCB_DCACHE_LINE_SIZE - 1U));
      uint32_t op_addr = (uint32_t)addr /* & ~(__SCB_DCACHE_LINE_SIZE - 1U) */;

      __DSB();

      do {
        SCB->DCIMVAC = op_addr;             /* register accepts only 32byte aligned values, only bits 31..5 are valid */
        op_addr += __SCB_DCACHE_LINE_SIZE;
        op_size -= __SCB_DCACHE_LINE_SIZE;
      } while ( op_size > 0 );

      __DSB();
      __ISB();
    }
  #endif
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
           D-Cache is cleaned starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are cleaned.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE void SCB_CleanDCache_by_Addr (volatile void *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (__SCB_DCACHE_LINE_SIZE - 1U));
      uint32_t op_addr = (uint32_t)addr /* & ~(__SCB_DCACHE_LINE_SIZE - 1U) */;

      __DSB();

      do {
        SCB->DCCMVAC = op_addr;             /* register accepts only 32byte aligned values, only bits 31..5 are valid */
        op_addr += __SCB_DCACHE_LINE_SIZE;
        op_size -= __SCB_DCACHE_LINE_SIZE;
      } while ( op_size > 0 );

      __DSB();
      __ISB();
    }
  #endif
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
           D-Cache is cleaned and invalidated starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are cleaned and invalidated.
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE void SCB_CleanInvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (__SCB_DCACHE_LINE_SIZE - 1U));
      uint32_t op_addr = (uint32_t)addr /* & ~(__SCB_DCACHE_LINE_SIZE - 1U) */;

      __DSB();

      do {
        SCB->DCCIMVAC = op_addr;            /* register accepts only 32byte aligned values, only bits 31..5 are valid */
        op_addr +=          __SCB_DCACHE_LINE_SIZE;
        op_size -=          __SCB_DCACHE_LINE_SIZE;
      } while ( op_size > 0 );

      __DSB();
      __ISB();
    }
  #endif
}

/*@} end of CMSIS_Core_CacheFunctions */

#endif /* ARM_ARMV7M_CACHEL1_H */
