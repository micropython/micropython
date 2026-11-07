/*!
    \file    gd32f4xx_fwdgt.h
    \brief   definitions for the FWDGT

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

#ifndef GD32F4XX_FWDGT_H
#define GD32F4XX_FWDGT_H

#include "gd32f4xx.h"

/* FWDGT definitions */
#define FWDGT                       FWDGT_BASE                      /*!< FWDGT base address */

/* registers definitions */
#define FWDGT_CTL                   REG32((FWDGT) + 0x00U)          /*!< FWDGT control register */
#define FWDGT_PSC                   REG32((FWDGT) + 0x04U)          /*!< FWDGT prescaler register */
#define FWDGT_RLD                   REG32((FWDGT) + 0x08U)          /*!< FWDGT reload register */
#define FWDGT_STAT                  REG32((FWDGT) + 0x0CU)          /*!< FWDGT status register */

/* bits definitions */
/* FWDGT_CTL */
#define FWDGT_CTL_CMD               BITS(0,15)                      /*!< FWDGT command value */

/* FWDGT_PSC */
#define FWDGT_PSC_PSC               BITS(0,2)                       /*!< FWDGT prescaler divider value */

/* FWDGT_RLD */
#define FWDGT_RLD_RLD               BITS(0,11)                      /*!< FWDGT counter reload value */

/* FWDGT_STAT */
#define FWDGT_STAT_PUD              BIT(0)                          /*!< FWDGT prescaler divider value update */
#define FWDGT_STAT_RUD              BIT(1)                          /*!< FWDGT counter reload value update */

/* constants definitions */
/* psc register value */
#define PSC_PSC(regval)             (BITS(0,2) & ((uint32_t)(regval) << 0))
#define FWDGT_PSC_DIV4              ((uint8_t)PSC_PSC(0))           /*!< FWDGT prescaler set to 4 */
#define FWDGT_PSC_DIV8              ((uint8_t)PSC_PSC(1))           /*!< FWDGT prescaler set to 8 */
#define FWDGT_PSC_DIV16             ((uint8_t)PSC_PSC(2))           /*!< FWDGT prescaler set to 16 */
#define FWDGT_PSC_DIV32             ((uint8_t)PSC_PSC(3))           /*!< FWDGT prescaler set to 32 */
#define FWDGT_PSC_DIV64             ((uint8_t)PSC_PSC(4))           /*!< FWDGT prescaler set to 64 */
#define FWDGT_PSC_DIV128            ((uint8_t)PSC_PSC(5))           /*!< FWDGT prescaler set to 128 */
#define FWDGT_PSC_DIV256            ((uint8_t)PSC_PSC(6))           /*!< FWDGT prescaler set to 256 */

/* control value */
#define FWDGT_WRITEACCESS_ENABLE    ((uint16_t)0x5555U)             /*!< FWDGT_CTL bits write access enable value */
#define FWDGT_WRITEACCESS_DISABLE   ((uint16_t)0x0000U)             /*!< FWDGT_CTL bits write access disable value */
#define FWDGT_KEY_RELOAD            ((uint16_t)0xAAAAU)             /*!< FWDGT_CTL bits fwdgt counter reload value */
#define FWDGT_KEY_ENABLE            ((uint16_t)0xCCCCU)             /*!< FWDGT_CTL bits fwdgt counter enable value */

/* FWDGT timeout value */
#define FWDGT_PSC_TIMEOUT           ((uint32_t)0x000FFFFFU)         /*!< FWDGT_PSC register write operation state flag timeout */
#define FWDGT_RLD_TIMEOUT           ((uint32_t)0x000FFFFFU)         /*!< FWDGT_RLD register write operation state flag timeout */

/* FWDGT flag definitions */
#define FWDGT_FLAG_PUD              FWDGT_STAT_PUD                  /*!< FWDGT prescaler divider value update flag */
#define FWDGT_FLAG_RUD              FWDGT_STAT_RUD                  /*!< FWDGT counter reload value update flag */

/* write value to FWDGT_RLD_RLD bit field */
#define RLD_RLD(regval)             (BITS(0,11) & ((uint32_t)(regval) << 0))

/* function declarations */
/* enable write access to FWDGT_PSC and FWDGT_RLD */
void fwdgt_write_enable(void);
/* disable write access to FWDGT_PSC and FWDGT_RLD */
void fwdgt_write_disable(void);
/* start the free watchdog timer counter */
void fwdgt_enable(void);

/* configure the free watchdog timer counter prescaler value */
ErrStatus fwdgt_prescaler_value_config(uint16_t prescaler_value);
/* configure the free watchdog timer counter reload value */
ErrStatus fwdgt_reload_value_config(uint16_t reload_value);
/* reload the counter of FWDGT */
void fwdgt_counter_reload(void);
/* configure counter reload value, and prescaler divider value */
ErrStatus fwdgt_config(uint16_t reload_value, uint8_t prescaler_value);

/* get flag state of FWDGT */
FlagStatus fwdgt_flag_get(uint16_t flag);

#endif /* GD32F4XX_FWDGT_H */
