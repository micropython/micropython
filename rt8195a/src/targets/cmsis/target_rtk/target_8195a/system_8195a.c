/**************************************************************************//**
 * @file     system_ARMCM3.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM3 Device Series
 * @version  V1.08
 * @date     23. November 2012
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2012 ARM LIMITED

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


#include "basic_types.h"
#include "rtl8195a.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HSI             ( 8000000UL)
#define __XTAL            ( 5000000UL)    /* Oscillator frequency             */

//#define __SYSTEM_CLOCK    (5*__XTAL)
#define __SYSTEM_CLOCK    (200000000UL/6*5)

extern unsigned int rand_x;
extern u32 HalGetCpuClk(VOID);


u32 Rand2(void)
{
    static unsigned int y = 362436;
    static unsigned int z = 521288629;
    static unsigned int c = 7654321;

    unsigned long long t, a= 698769069;

    rand_x = 69069 * rand_x + 12345;
    y ^= (y << 13); y ^= (y >> 17); y ^= (y << 5);
    t = a * z + c; c = (t >> 32); z = t;

    return rand_x + y + z;
}

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __SYSTEM_CLOCK;/*!< System Clock Frequency (Core Clock)*/


u32
SystemGetCpuClk(void)
{
    return HalGetCpuClk();
}

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
//  SystemCoreClock = __SYSTEM_CLOCK;
  
    SystemCoreClock = SystemGetCpuClk();
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
    // TODO: Hardware initial
#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

  //SystemCoreClock = __SYSTEM_CLOCK;
  //SystemCoreClock = HalGetCpuClk();
    SystemCoreClockUpdate();
}
