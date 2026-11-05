/*!
    \file    gd32f4xx_iref.h
    \brief   definitions for the IREF

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

#ifndef GD32F4XX_IREF_H
#define GD32F4XX_IREF_H

#include "gd32f4xx.h"

/* IREF definitions */
#define IREF                            IREF_BASE              /*!< IREF base address */

/* registers definitions */
#define IREF_CTL                        REG32(IREF + 0x300U)   /*!< IREF control register */

/* bits definitions */
/* IREF_CTL */
#define IREF_CTL_CSDT                   BITS(0,5)              /*!< current step data */
#define IREF_CTL_SCMOD                  BIT(7)                 /*!< sink current mode */
#define IREF_CTL_CPT                    BITS(8,12)             /*!< current precision trim */
#define IREF_CTL_SSEL                   BIT(14)                /*!< step selection */ 
#define IREF_CTL_CREN                   BIT(15)                /*!< current reference enable */

/* constants definitions */
/* IREF current precision trim */
#define CTL_CPT(regval)                 (BITS(8,12) & ((uint32_t)(regval) << 8))
#define IREF_CUR_PRECISION_TRIM_0       CTL_CPT(0)             /*!< IREF current precision trim 0 */
#define IREF_CUR_PRECISION_TRIM_1       CTL_CPT(1)             /*!< IREF current precision trim 1 */
#define IREF_CUR_PRECISION_TRIM_2       CTL_CPT(2)             /*!< IREF current precision trim 2 */
#define IREF_CUR_PRECISION_TRIM_3       CTL_CPT(3)             /*!< IREF current precision trim 3 */
#define IREF_CUR_PRECISION_TRIM_4       CTL_CPT(4)             /*!< IREF current precision trim 4 */
#define IREF_CUR_PRECISION_TRIM_5       CTL_CPT(5)             /*!< IREF current precision trim 5 */
#define IREF_CUR_PRECISION_TRIM_6       CTL_CPT(6)             /*!< IREF current precision trim 6 */
#define IREF_CUR_PRECISION_TRIM_7       CTL_CPT(7)             /*!< IREF current precision trim 7 */
#define IREF_CUR_PRECISION_TRIM_8       CTL_CPT(8)             /*!< IREF current precision trim 8 */
#define IREF_CUR_PRECISION_TRIM_9       CTL_CPT(9)             /*!< IREF current precision trim 9 */
#define IREF_CUR_PRECISION_TRIM_10      CTL_CPT(10)            /*!< IREF current precision trim 10 */
#define IREF_CUR_PRECISION_TRIM_11      CTL_CPT(11)            /*!< IREF current precision trim 11 */
#define IREF_CUR_PRECISION_TRIM_12      CTL_CPT(12)            /*!< IREF current precision trim 12 */
#define IREF_CUR_PRECISION_TRIM_13      CTL_CPT(13)            /*!< IREF current precision trim 13 */
#define IREF_CUR_PRECISION_TRIM_14      CTL_CPT(14)            /*!< IREF current precision trim 14 */
#define IREF_CUR_PRECISION_TRIM_15      CTL_CPT(15)            /*!< IREF current precision trim 15 */
#define IREF_CUR_PRECISION_TRIM_16      CTL_CPT(16)            /*!< IREF current precision trim 16 */
#define IREF_CUR_PRECISION_TRIM_17      CTL_CPT(17)            /*!< IREF current precision trim 17 */
#define IREF_CUR_PRECISION_TRIM_18      CTL_CPT(18)            /*!< IREF current precision trim 18 */
#define IREF_CUR_PRECISION_TRIM_19      CTL_CPT(19)            /*!< IREF current precision trim 19 */
#define IREF_CUR_PRECISION_TRIM_20      CTL_CPT(20)            /*!< IREF current precision trim 20 */
#define IREF_CUR_PRECISION_TRIM_21      CTL_CPT(21)            /*!< IREF current precision trim 21 */
#define IREF_CUR_PRECISION_TRIM_22      CTL_CPT(22)            /*!< IREF current precision trim 22 */
#define IREF_CUR_PRECISION_TRIM_23      CTL_CPT(23)            /*!< IREF current precision trim 23 */
#define IREF_CUR_PRECISION_TRIM_24      CTL_CPT(24)            /*!< IREF current precision trim 24 */
#define IREF_CUR_PRECISION_TRIM_25      CTL_CPT(25)            /*!< IREF current precision trim 25 */
#define IREF_CUR_PRECISION_TRIM_26      CTL_CPT(26)            /*!< IREF current precision trim 26 */
#define IREF_CUR_PRECISION_TRIM_27      CTL_CPT(27)            /*!< IREF current precision trim 27 */
#define IREF_CUR_PRECISION_TRIM_28      CTL_CPT(28)            /*!< IREF current precision trim 28 */
#define IREF_CUR_PRECISION_TRIM_29      CTL_CPT(29)            /*!< IREF current precision trim 29 */
#define IREF_CUR_PRECISION_TRIM_30      CTL_CPT(30)            /*!< IREF current precision trim 30 */
#define IREF_CUR_PRECISION_TRIM_31      CTL_CPT(31)            /*!< IREF current precision trim 31 */

/* IREF current step */
#define CTL_CSDT(regval)                (BITS(0,5) & ((uint32_t)(regval) << 0))
#define IREF_CUR_STEP_DATA_0            CTL_CSDT(0)            /*!< IREF current step data 0 */
#define IREF_CUR_STEP_DATA_1            CTL_CSDT(1)            /*!< IREF current step data 1 */
#define IREF_CUR_STEP_DATA_2            CTL_CSDT(2)            /*!< IREF current step data 2 */
#define IREF_CUR_STEP_DATA_3            CTL_CSDT(3)            /*!< IREF current step data 3 */
#define IREF_CUR_STEP_DATA_4            CTL_CSDT(4)            /*!< IREF current step data 4 */
#define IREF_CUR_STEP_DATA_5            CTL_CSDT(5)            /*!< IREF current step data 5 */
#define IREF_CUR_STEP_DATA_6            CTL_CSDT(6)            /*!< IREF current step data 6 */
#define IREF_CUR_STEP_DATA_7            CTL_CSDT(7)            /*!< IREF current step data 7 */
#define IREF_CUR_STEP_DATA_8            CTL_CSDT(8)            /*!< IREF current step data 8 */
#define IREF_CUR_STEP_DATA_9            CTL_CSDT(9)            /*!< IREF current step data 9 */
#define IREF_CUR_STEP_DATA_10           CTL_CSDT(10)           /*!< IREF current step data 10 */
#define IREF_CUR_STEP_DATA_11           CTL_CSDT(11)           /*!< IREF current step data 11 */
#define IREF_CUR_STEP_DATA_12           CTL_CSDT(12)           /*!< IREF current step data 12 */
#define IREF_CUR_STEP_DATA_13           CTL_CSDT(13)           /*!< IREF current step data 13 */
#define IREF_CUR_STEP_DATA_14           CTL_CSDT(14)           /*!< IREF current step data 14 */
#define IREF_CUR_STEP_DATA_15           CTL_CSDT(15)           /*!< IREF current step data 15 */
#define IREF_CUR_STEP_DATA_16           CTL_CSDT(16)           /*!< IREF current step data 16 */
#define IREF_CUR_STEP_DATA_17           CTL_CSDT(17)           /*!< IREF current step data 17 */
#define IREF_CUR_STEP_DATA_18           CTL_CSDT(18)           /*!< IREF current step data 18 */
#define IREF_CUR_STEP_DATA_19           CTL_CSDT(19)           /*!< IREF current step data 19 */
#define IREF_CUR_STEP_DATA_20           CTL_CSDT(20)           /*!< IREF current step data 20 */
#define IREF_CUR_STEP_DATA_21           CTL_CSDT(21)           /*!< IREF current step data 21 */
#define IREF_CUR_STEP_DATA_22           CTL_CSDT(22)           /*!< IREF current step data 22 */
#define IREF_CUR_STEP_DATA_23           CTL_CSDT(23)           /*!< IREF current step data 23 */
#define IREF_CUR_STEP_DATA_24           CTL_CSDT(24)           /*!< IREF current step data 24 */
#define IREF_CUR_STEP_DATA_25           CTL_CSDT(25)           /*!< IREF current step data 25 */
#define IREF_CUR_STEP_DATA_26           CTL_CSDT(26)           /*!< IREF current step data 26 */
#define IREF_CUR_STEP_DATA_27           CTL_CSDT(27)           /*!< IREF current step data 27 */
#define IREF_CUR_STEP_DATA_28           CTL_CSDT(28)           /*!< IREF current step data 28 */
#define IREF_CUR_STEP_DATA_29           CTL_CSDT(29)           /*!< IREF current step data 29 */
#define IREF_CUR_STEP_DATA_30           CTL_CSDT(30)           /*!< IREF current step data 30 */
#define IREF_CUR_STEP_DATA_31           CTL_CSDT(31)           /*!< IREF current step data 31 */
#define IREF_CUR_STEP_DATA_32           CTL_CSDT(32)           /*!< IREF current step data 32 */
#define IREF_CUR_STEP_DATA_33           CTL_CSDT(33)           /*!< IREF current step data 33 */
#define IREF_CUR_STEP_DATA_34           CTL_CSDT(34)           /*!< IREF current step data 34 */
#define IREF_CUR_STEP_DATA_35           CTL_CSDT(35)           /*!< IREF current step data 35 */
#define IREF_CUR_STEP_DATA_36           CTL_CSDT(36)           /*!< IREF current step data 36 */
#define IREF_CUR_STEP_DATA_37           CTL_CSDT(37)           /*!< IREF current step data 37 */
#define IREF_CUR_STEP_DATA_38           CTL_CSDT(38)           /*!< IREF current step data 38 */
#define IREF_CUR_STEP_DATA_39           CTL_CSDT(39)           /*!< IREF current step data 39 */
#define IREF_CUR_STEP_DATA_40           CTL_CSDT(40)           /*!< IREF current step data 40 */
#define IREF_CUR_STEP_DATA_41           CTL_CSDT(41)           /*!< IREF current step data 41 */
#define IREF_CUR_STEP_DATA_42           CTL_CSDT(42)           /*!< IREF current step data 42 */
#define IREF_CUR_STEP_DATA_43           CTL_CSDT(43)           /*!< IREF current step data 43 */
#define IREF_CUR_STEP_DATA_44           CTL_CSDT(44)           /*!< IREF current step data 44 */
#define IREF_CUR_STEP_DATA_45           CTL_CSDT(45)           /*!< IREF current step data 45 */
#define IREF_CUR_STEP_DATA_46           CTL_CSDT(46)           /*!< IREF current step data 46 */
#define IREF_CUR_STEP_DATA_47           CTL_CSDT(47)           /*!< IREF current step data 47 */
#define IREF_CUR_STEP_DATA_48           CTL_CSDT(48)           /*!< IREF current step data 48 */
#define IREF_CUR_STEP_DATA_49           CTL_CSDT(49)           /*!< IREF current step data 49 */
#define IREF_CUR_STEP_DATA_50           CTL_CSDT(50)           /*!< IREF current step data 50 */
#define IREF_CUR_STEP_DATA_51           CTL_CSDT(51)           /*!< IREF current step data 51 */
#define IREF_CUR_STEP_DATA_52           CTL_CSDT(52)           /*!< IREF current step data 52 */
#define IREF_CUR_STEP_DATA_53           CTL_CSDT(53)           /*!< IREF current step data 53 */
#define IREF_CUR_STEP_DATA_54           CTL_CSDT(54)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_55           CTL_CSDT(55)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_56           CTL_CSDT(56)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_57           CTL_CSDT(57)           /*!< IREF current step data 57 */
#define IREF_CUR_STEP_DATA_58           CTL_CSDT(58)           /*!< IREF current step data 58 */
#define IREF_CUR_STEP_DATA_59           CTL_CSDT(59)           /*!< IREF current step data 59 */
#define IREF_CUR_STEP_DATA_60           CTL_CSDT(60)           /*!< IREF current step data 60 */
#define IREF_CUR_STEP_DATA_61           CTL_CSDT(61)           /*!< IREF current step data 61 */
#define IREF_CUR_STEP_DATA_62           CTL_CSDT(62)           /*!< IREF current step data 62 */
#define IREF_CUR_STEP_DATA_63           CTL_CSDT(63)           /*!< IREF current step data 63 */
 
/* IREF mode selection */
#define IREF_STEP(regval)               (BIT(14) & ((uint32_t)(regval) << 14))
#define IREF_MODE_LOW_POWER             IREF_STEP(0)           /*!< low power, 1uA step */
#define IREF_MODE_HIGH_CURRENT          IREF_STEP(1)           /*!< high current, 8uA step */
 
/* IREF sink current mode*/ 
#define IREF_CURRENT(regval)            (BIT(7) & ((uint32_t)(regval) << 7))
#define IREF_SOURCE_CURRENT             IREF_CURRENT(0)        /*!< IREF source current */
#define IREF_SINK_CURRENT               IREF_CURRENT(1)        /*!< IREF sink current */

/* function declarations */
/* deinitialize IREF */
void iref_deinit(void);
/* enable IREF */
void iref_enable(void);
/* disable IREF */
void iref_disable(void);

/* set IREF mode*/
void iref_mode_set(uint32_t step);
/* set IREF sink current mode*/
void iref_sink_set(uint32_t sinkmode);
/* set IREF current precision trim value */
void iref_precision_trim_value_set(uint32_t precisiontrim);
/* set IREF step data*/
void iref_step_data_config(uint32_t stepdata);

#endif /* GD32F4XX_IREF_H */
