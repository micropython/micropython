/*!
    \file    gd32f4xx_trng.h
    \brief   definitions for the TRNG

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GD32F4XX_TRNG_H
#define GD32F4XX_TRNG_H

#include "gd32f4xx.h"

/* TRNG definitions */
#define TRNG                        TRNG_BASE

/* registers definitions */
#define TRNG_CTL                    REG32(TRNG + 0x00000000U)        /*!< control register */
#define TRNG_STAT                   REG32(TRNG + 0x00000004U)        /*!< status register */
#define TRNG_DATA                   REG32(TRNG + 0x00000008U)        /*!< data register */

/* bits definitions */
/* TRNG_CTL */
#define TRNG_CTL_TRNGEN             BIT(2)                     /*!< TRNG enable bit */
#define TRNG_CTL_TRNGIE             BIT(3)                     /*!< interrupt enable bit */

/* TRNG_STAT */
#define TRNG_STAT_DRDY              BIT(0)                     /*!< random data ready status bit */
#define TRNG_STAT_CECS              BIT(1)                     /*!< clock error current status */
#define TRNG_STAT_SECS              BIT(2)                     /*!< seed error current status */
#define TRNG_STAT_CEIF              BIT(5)                     /*!< clock error interrupt flag */
#define TRNG_STAT_SEIF              BIT(6)                     /*!< seed error interrupt flag */

/* TRNG_DATA */
#define TRNG_DATA_TRNGDATA          BITS(0,31)                 /*!< 32-Bit Random data */

/* constants definitions */
/* TRNG status flag */
typedef enum { 
    TRNG_FLAG_DRDY = TRNG_STAT_DRDY,                           /*!< random Data ready status */
    TRNG_FLAG_CECS = TRNG_STAT_CECS,                           /*!< clock error current status */
    TRNG_FLAG_SECS = TRNG_STAT_SECS                            /*!< seed error current status */
} trng_flag_enum;

/* TRNG inerrupt flag */
typedef enum {
    TRNG_INT_FLAG_CEIF = TRNG_STAT_CEIF,                       /*!< clock error interrupt flag */
    TRNG_INT_FLAG_SEIF = TRNG_STAT_SEIF                        /*!< seed error interrupt flag */
} trng_int_flag_enum;

/* function declarations */
/* initialization functions */
/* reset TRNG */
void trng_deinit(void);
/* enable TRNG */
void trng_enable(void);
/* disable TRNG */
void trng_disable(void);
/* get the true random data */
uint32_t trng_get_true_random_data(void);

/* interrupt & flag functions */
/* enable TRNG interrupt */
void trng_interrupt_enable(void);
/* disable TRNG interrupt  */
void trng_interrupt_disable(void);
/* get TRNG flag status */
FlagStatus trng_flag_get(trng_flag_enum flag);
/* get TRNG interrupt flag status */
FlagStatus trng_interrupt_flag_get(trng_int_flag_enum int_flag);
/* clear TRNG interrupt flag status */
void trng_interrupt_flag_clear(trng_int_flag_enum int_flag);

#endif /* GD32F4XX_TRNG_H */
