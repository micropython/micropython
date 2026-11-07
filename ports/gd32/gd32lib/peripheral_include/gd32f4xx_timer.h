/*!
    \file    gd32f4xx_timer.h
    \brief   definitions for the TIMER

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

#ifndef GD32F4XX_TIMER_H
#define GD32F4XX_TIMER_H

#include "gd32f4xx.h"

/* TIMERx(x=0..13) definitions */
#define TIMER0                           (TIMER_BASE + 0x00010000U)
#define TIMER1                           (TIMER_BASE + 0x00000000U)
#define TIMER2                           (TIMER_BASE + 0x00000400U)
#define TIMER3                           (TIMER_BASE + 0x00000800U)
#define TIMER4                           (TIMER_BASE + 0x00000C00U)
#define TIMER5                           (TIMER_BASE + 0x00001000U)
#define TIMER6                           (TIMER_BASE + 0x00001400U)
#define TIMER7                           (TIMER_BASE + 0x00010400U)
#define TIMER8                           (TIMER_BASE + 0x00014000U)
#define TIMER9                           (TIMER_BASE + 0x00014400U)
#define TIMER10                          (TIMER_BASE + 0x00014800U)
#define TIMER11                          (TIMER_BASE + 0x00001800U)
#define TIMER12                          (TIMER_BASE + 0x00001C00U)
#define TIMER13                          (TIMER_BASE + 0x00002000U)

/* registers definitions */
#define TIMER_CTL0(timerx)               REG32((timerx) + 0x00U)           /*!< TIMER control register 0 */
#define TIMER_CTL1(timerx)               REG32((timerx) + 0x04U)           /*!< TIMER control register 1 */
#define TIMER_SMCFG(timerx)              REG32((timerx) + 0x08U)           /*!< TIMER slave mode configuration register */
#define TIMER_DMAINTEN(timerx)           REG32((timerx) + 0x0CU)           /*!< TIMER DMA and interrupt enable register */
#define TIMER_INTF(timerx)               REG32((timerx) + 0x10U)           /*!< TIMER interrupt flag register */
#define TIMER_SWEVG(timerx)              REG32((timerx) + 0x14U)           /*!< TIMER software event generation register */
#define TIMER_CHCTL0(timerx)             REG32((timerx) + 0x18U)           /*!< TIMER channel control register 0 */
#define TIMER_CHCTL1(timerx)             REG32((timerx) + 0x1CU)           /*!< TIMER channel control register 1 */
#define TIMER_CHCTL2(timerx)             REG32((timerx) + 0x20U)           /*!< TIMER channel control register 2 */
#define TIMER_CNT(timerx)                REG32((timerx) + 0x24U)           /*!< TIMER counter register */
#define TIMER_PSC(timerx)                REG32((timerx) + 0x28U)           /*!< TIMER prescaler register */
#define TIMER_CAR(timerx)                REG32((timerx) + 0x2CU)           /*!< TIMER counter auto reload register */
#define TIMER_CREP(timerx)               REG32((timerx) + 0x30U)           /*!< TIMER counter repetition register */
#define TIMER_CH0CV(timerx)              REG32((timerx) + 0x34U)           /*!< TIMER channel 0 capture/compare value register */
#define TIMER_CH1CV(timerx)              REG32((timerx) + 0x38U)           /*!< TIMER channel 1 capture/compare value register */
#define TIMER_CH2CV(timerx)              REG32((timerx) + 0x3CU)           /*!< TIMER channel 2 capture/compare value register */
#define TIMER_CH3CV(timerx)              REG32((timerx) + 0x40U)           /*!< TIMER channel 3 capture/compare value register */
#define TIMER_CCHP(timerx)               REG32((timerx) + 0x44U)           /*!< TIMER complementary channel protection register */
#define TIMER_DMACFG(timerx)             REG32((timerx) + 0x48U)           /*!< TIMER DMA configuration register */
#define TIMER_DMATB(timerx)              REG32((timerx) + 0x4CU)           /*!< TIMER DMA transfer buffer register */
#define TIMER_IRMP(timerx)               REG32((timerx) + 0x50U)           /*!< TIMER channel input remap register */
#define TIMER_CFG(timerx)                REG32((timerx) + 0xFCU)           /*!< TIMER configuration register */

/* bits definitions */
/* TIMER_CTL0 */
#define TIMER_CTL0_CEN                   BIT(0)              /*!< TIMER counter enable */
#define TIMER_CTL0_UPDIS                 BIT(1)              /*!< update disable */
#define TIMER_CTL0_UPS                   BIT(2)              /*!< update source */
#define TIMER_CTL0_SPM                   BIT(3)              /*!< single pulse mode */
#define TIMER_CTL0_DIR                   BIT(4)              /*!< timer counter direction */
#define TIMER_CTL0_CAM                   BITS(5,6)           /*!< center-aligned mode selection */
#define TIMER_CTL0_ARSE                  BIT(7)              /*!< auto-reload shadow enable */
#define TIMER_CTL0_CKDIV                 BITS(8,9)           /*!< clock division */

/* TIMER_CTL1 */
#define TIMER_CTL1_CCSE                  BIT(0)              /*!< commutation control shadow enable */
#define TIMER_CTL1_CCUC                  BIT(2)              /*!< commutation control shadow register update control */
#define TIMER_CTL1_DMAS                  BIT(3)              /*!< DMA request source selection */
#define TIMER_CTL1_MMC                   BITS(4,6)           /*!< master mode control */
#define TIMER_CTL1_TI0S                  BIT(7)              /*!< channel 0 trigger input selection(hall mode selection) */
#define TIMER_CTL1_ISO0                  BIT(8)              /*!< idle state of channel 0 output */
#define TIMER_CTL1_ISO0N                 BIT(9)              /*!< idle state of channel 0 complementary output */
#define TIMER_CTL1_ISO1                  BIT(10)             /*!< idle state of channel 1 output */
#define TIMER_CTL1_ISO1N                 BIT(11)             /*!< idle state of channel 1 complementary output */
#define TIMER_CTL1_ISO2                  BIT(12)             /*!< idle state of channel 2 output */
#define TIMER_CTL1_ISO2N                 BIT(13)             /*!< idle state of channel 2 complementary output */
#define TIMER_CTL1_ISO3                  BIT(14)             /*!< idle state of channel 3 output */

/* TIMER_SMCFG */
#define TIMER_SMCFG_SMC                  BITS(0,2)           /*!< slave mode control */
#define TIMER_SMCFG_TRGS                 BITS(4,6)           /*!< trigger selection */
#define TIMER_SMCFG_MSM                  BIT(7)              /*!< master-slave mode */
#define TIMER_SMCFG_ETFC                 BITS(8,11)          /*!< external trigger filter control */
#define TIMER_SMCFG_ETPSC                BITS(12,13)         /*!< external trigger prescaler */
#define TIMER_SMCFG_SMC1                 BIT(14)             /*!< part of SMC for enable external clock mode 1 */
#define TIMER_SMCFG_ETP                  BIT(15)             /*!< external trigger polarity */
 
/* TIMER_DMAINTEN */
#define TIMER_DMAINTEN_UPIE              BIT(0)              /*!< update interrupt enable */
#define TIMER_DMAINTEN_CH0IE             BIT(1)              /*!< channel 0 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH1IE             BIT(2)              /*!< channel 1 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH2IE             BIT(3)              /*!< channel 2 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CH3IE             BIT(4)              /*!< channel 3 capture/compare interrupt enable */
#define TIMER_DMAINTEN_CMTIE             BIT(5)              /*!< commutation interrupt request enable */
#define TIMER_DMAINTEN_TRGIE             BIT(6)              /*!< trigger interrupt enable */
#define TIMER_DMAINTEN_BRKIE             BIT(7)              /*!< break interrupt enable */
#define TIMER_DMAINTEN_UPDEN             BIT(8)              /*!< update DMA request enable */
#define TIMER_DMAINTEN_CH0DEN            BIT(9)              /*!< channel 0 DMA request enable */
#define TIMER_DMAINTEN_CH1DEN            BIT(10)             /*!< channel 1 DMA request enable */
#define TIMER_DMAINTEN_CH2DEN            BIT(11)             /*!< channel 2 DMA request enable */
#define TIMER_DMAINTEN_CH3DEN            BIT(12)             /*!< channel 3 DMA request enable */
#define TIMER_DMAINTEN_CMTDEN            BIT(13)             /*!< commutation DMA request enable */
#define TIMER_DMAINTEN_TRGDEN            BIT(14)             /*!< trigger DMA request enable */

/* TIMER_INTF */
#define TIMER_INTF_UPIF                  BIT(0)              /*!< update interrupt flag */
#define TIMER_INTF_CH0IF                 BIT(1)              /*!< channel 0 capture/compare interrupt flag */
#define TIMER_INTF_CH1IF                 BIT(2)              /*!< channel 1 capture/compare interrupt flag */
#define TIMER_INTF_CH2IF                 BIT(3)              /*!< channel 2 capture/compare interrupt flag */
#define TIMER_INTF_CH3IF                 BIT(4)              /*!< channel 3 capture/compare interrupt flag */
#define TIMER_INTF_CMTIF                 BIT(5)              /*!< channel commutation interrupt flag */
#define TIMER_INTF_TRGIF                 BIT(6)              /*!< trigger interrupt flag */
#define TIMER_INTF_BRKIF                 BIT(7)              /*!< break interrupt flag */
#define TIMER_INTF_CH0OF                 BIT(9)              /*!< channel 0 overcapture flag */
#define TIMER_INTF_CH1OF                 BIT(10)             /*!< channel 1 overcapture flag */
#define TIMER_INTF_CH2OF                 BIT(11)             /*!< channel 2 overcapture flag */
#define TIMER_INTF_CH3OF                 BIT(12)             /*!< channel 3 overcapture flag */

/* TIMER_SWEVG */
#define TIMER_SWEVG_UPG                  BIT(0)              /*!< update event generate */
#define TIMER_SWEVG_CH0G                 BIT(1)              /*!< channel 0 capture or compare event generation */
#define TIMER_SWEVG_CH1G                 BIT(2)              /*!< channel 1 capture or compare event generation */
#define TIMER_SWEVG_CH2G                 BIT(3)              /*!< channel 2 capture or compare event generation */
#define TIMER_SWEVG_CH3G                 BIT(4)              /*!< channel 3 capture or compare event generation */
#define TIMER_SWEVG_CMTG                 BIT(5)              /*!< channel commutation event generation */
#define TIMER_SWEVG_TRGG                 BIT(6)              /*!< trigger event generation */
#define TIMER_SWEVG_BRKG                 BIT(7)              /*!< break event generation */

/* TIMER_CHCTL0 */
/* output compare mode */
#define TIMER_CHCTL0_CH0MS               BITS(0,1)           /*!< channel 0 mode selection */
#define TIMER_CHCTL0_CH0COMFEN           BIT(2)              /*!< channel 0 output compare fast enable */
#define TIMER_CHCTL0_CH0COMSEN           BIT(3)              /*!< channel 0 output compare shadow enable */
#define TIMER_CHCTL0_CH0COMCTL           BITS(4,6)           /*!< channel 0 output compare mode */
#define TIMER_CHCTL0_CH0COMCEN           BIT(7)              /*!< channel 0 output compare clear enable */
#define TIMER_CHCTL0_CH1MS               BITS(8,9)           /*!< channel 1 mode selection */
#define TIMER_CHCTL0_CH1COMFEN           BIT(10)             /*!< channel 1 output compare fast enable */
#define TIMER_CHCTL0_CH1COMSEN           BIT(11)             /*!< channel 1 output compare shadow enable */
#define TIMER_CHCTL0_CH1COMCTL           BITS(12,14)         /*!< channel 1 output compare mode */
#define TIMER_CHCTL0_CH1COMCEN           BIT(15)             /*!< channel 1 output compare clear enable */
/* input capture mode */
#define TIMER_CHCTL0_CH0CAPPSC           BITS(2,3)           /*!< channel 0 input capture prescaler */
#define TIMER_CHCTL0_CH0CAPFLT           BITS(4,7)           /*!< channel 0 input capture filter control */
#define TIMER_CHCTL0_CH1CAPPSC           BITS(10,11)         /*!< channel 1 input capture prescaler */
#define TIMER_CHCTL0_CH1CAPFLT           BITS(12,15)         /*!< channel 1 input capture filter control */

/* TIMER_CHCTL1 */
/* output compare mode */
#define TIMER_CHCTL1_CH2MS               BITS(0,1)           /*!< channel 2 mode selection */
#define TIMER_CHCTL1_CH2COMFEN           BIT(2)              /*!< channel 2 output compare fast enable */
#define TIMER_CHCTL1_CH2COMSEN           BIT(3)              /*!< channel 2 output compare shadow enable */
#define TIMER_CHCTL1_CH2COMCTL           BITS(4,6)           /*!< channel 2 output compare mode */
#define TIMER_CHCTL1_CH2COMCEN           BIT(7)              /*!< channel 2 output compare clear enable */
#define TIMER_CHCTL1_CH3MS               BITS(8,9)           /*!< channel 3 mode selection */
#define TIMER_CHCTL1_CH3COMFEN           BIT(10)             /*!< channel 3 output compare fast enable */
#define TIMER_CHCTL1_CH3COMSEN           BIT(11)             /*!< channel 3 output compare shadow enable */
#define TIMER_CHCTL1_CH3COMCTL           BITS(12,14)         /*!< channel 3 output compare mode */
#define TIMER_CHCTL1_CH3COMCEN           BIT(15)             /*!< channel 3 output compare clear enable */
/* input capture mode */
#define TIMER_CHCTL1_CH2CAPPSC           BITS(2,3)           /*!< channel 2 input capture prescaler */
#define TIMER_CHCTL1_CH2CAPFLT           BITS(4,7)           /*!< channel 2 input capture filter control */
#define TIMER_CHCTL1_CH3CAPPSC           BITS(10,11)         /*!< channel 3 input capture prescaler */
#define TIMER_CHCTL1_CH3CAPFLT           BITS(12,15)         /*!< channel 3 input capture filter control */

/* TIMER_CHCTL2 */
#define TIMER_CHCTL2_CH0EN               BIT(0)              /*!< channel 0 capture/compare function enable */
#define TIMER_CHCTL2_CH0P                BIT(1)              /*!< channel 0 capture/compare function polarity */
#define TIMER_CHCTL2_CH0NEN              BIT(2)              /*!< channel 0 complementary output enable */
#define TIMER_CHCTL2_CH0NP               BIT(3)              /*!< channel 0 complementary output polarity */
#define TIMER_CHCTL2_CH1EN               BIT(4)              /*!< channel 1 capture/compare function enable  */
#define TIMER_CHCTL2_CH1P                BIT(5)              /*!< channel 1 capture/compare function polarity */
#define TIMER_CHCTL2_CH1NEN              BIT(6)              /*!< channel 1 complementary output enable */
#define TIMER_CHCTL2_CH1NP               BIT(7)              /*!< channel 1 complementary output polarity */
#define TIMER_CHCTL2_CH2EN               BIT(8)              /*!< channel 2 capture/compare function enable  */
#define TIMER_CHCTL2_CH2P                BIT(9)              /*!< channel 2 capture/compare function polarity */
#define TIMER_CHCTL2_CH2NEN              BIT(10)             /*!< channel 2 complementary output enable */
#define TIMER_CHCTL2_CH2NP               BIT(11)             /*!< channel 2 complementary output polarity */
#define TIMER_CHCTL2_CH3EN               BIT(12)             /*!< channel 3 capture/compare function enable  */
#define TIMER_CHCTL2_CH3P                BIT(13)             /*!< channel 3 capture/compare function polarity */

/* TIMER_CNT */
#define TIMER_CNT_CNT16                  BITS(0,15)          /*!< 16 bit timer counter */
#define TIMER_CNT_CNT32                  BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) timer counter */

/* TIMER_PSC */
#define TIMER_PSC_PSC                    BITS(0,15)          /*!< prescaler value of the counter clock */

/* TIMER_CAR */
#define TIMER_CAR_CARL16                 BITS(0,15)          /*!< 16 bit counter auto reload value */
#define TIMER_CAR_CARL32                 BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) counter auto reload value */

/* TIMER_CREP */
#define TIMER_CREP_CREP                  BITS(0,7)           /*!< counter repetition value */

/* TIMER_CH0CV */
#define TIMER_CH0CV_CH0VAL16             BITS(0,15)          /*!< 16 bit capture/compare value of channel 0 */
#define TIMER_CH0CV_CH0VAL32             BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) capture/compare value of channel 0 */

/* TIMER_CH1CV */
#define TIMER_CH1CV_CH1VAL16             BITS(0,15)          /*!< 16 bit capture/compare value of channel 1 */
#define TIMER_CH1CV_CH1VAL32             BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) capture/compare value of channel 1 */

/* TIMER_CH2CV */
#define TIMER_CH2CV_CH2VAL16             BITS(0,15)          /*!< 16 bit capture/compare value of channel 2 */
#define TIMER_CH2CV_CH2VAL32             BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) capture/compare value of channel 2 */

/* TIMER_CH3CV */
#define TIMER_CH3CV_CH3VAL16             BITS(0,15)          /*!< 16 bit capture/compare value of channel 3 */
#define TIMER_CH3CV_CH3VAL32             BITS(0,31)          /*!< 32 bit(TIMER1,TIMER4) capture/compare value of channel 3 */

/* TIMER_CCHP */
#define TIMER_CCHP_DTCFG                 BITS(0,7)           /*!< dead time configure */
#define TIMER_CCHP_PROT                  BITS(8,9)           /*!< complementary register protect control */
#define TIMER_CCHP_IOS                   BIT(10)             /*!< idle mode off-state configure */
#define TIMER_CCHP_ROS                   BIT(11)             /*!< run mode off-state configure */
#define TIMER_CCHP_BRKEN                 BIT(12)             /*!< break enable */
#define TIMER_CCHP_BRKP                  BIT(13)             /*!< break polarity */
#define TIMER_CCHP_OAEN                  BIT(14)             /*!< output automatic enable */
#define TIMER_CCHP_POEN                  BIT(15)             /*!< primary output enable */

/* TIMER_DMACFG */
#define TIMER_DMACFG_DMATA               BITS(0,4)           /*!< DMA transfer access start address */
#define TIMER_DMACFG_DMATC               BITS(8,12)          /*!< DMA transfer count */

/* TIMER_DMATB */
#define TIMER_DMATB_DMATB                BITS(0,15)          /*!< DMA transfer buffer address */

/* TIMER_IRMP */
#define TIMER1_IRMP_ITI1_RMP             BITS(10,11)         /*!< TIMER1 internal trigger input 1 remap */
#define TIMER4_IRMP_CI3_RMP              BITS(6,7)           /*!< TIMER4 channel 3 input remap */
#define TIMER10_IRMP_ITI1_RMP            BITS(0,1)           /*!< TIMER10 internal trigger input 1 remap */

/* TIMER_CFG */
#define TIMER_CFG_OUTSEL                 BIT(0)              /*!< the output value selection */
#define TIMER_CFG_CHVSEL                 BIT(1)              /*!< write CHxVAL register selection */

/* constants definitions */
/* TIMER init parameter struct definitions*/
typedef struct
{ 
    uint16_t prescaler;                         /*!< prescaler value */
    uint16_t alignedmode;                       /*!< aligned mode */
    uint16_t counterdirection;                  /*!< counter direction */
    uint16_t clockdivision;                     /*!< clock division value */
    uint32_t period;                            /*!< period value */
    uint8_t  repetitioncounter;                 /*!< the counter repetition value */
}timer_parameter_struct;

/* break parameter struct definitions*/
typedef struct
{ 
    uint16_t runoffstate;                       /*!< run mode off-state */
    uint16_t ideloffstate;                      /*!< idle mode off-state */
    uint16_t deadtime;                          /*!< dead time */
    uint16_t breakpolarity;                     /*!< break polarity */
    uint16_t outputautostate;                   /*!< output automatic enable */
    uint16_t protectmode;                       /*!< complementary register protect control */
    uint16_t breakstate;                        /*!< break enable */
}timer_break_parameter_struct;

/* channel output parameter struct definitions */
typedef struct
{ 
    uint16_t outputstate;                       /*!< channel output state */
    uint16_t outputnstate;                      /*!< channel complementary output state */
    uint16_t ocpolarity;                        /*!< channel output polarity */
    uint16_t ocnpolarity;                       /*!< channel complementary output polarity */
    uint16_t ocidlestate;                       /*!< idle state of channel output */
    uint16_t ocnidlestate;                      /*!< idle state of channel complementary output */
}timer_oc_parameter_struct;

/* channel input parameter struct definitions */
typedef struct
{ 
    uint16_t icpolarity;                        /*!< channel input polarity */
    uint16_t icselection;                       /*!< channel input mode selection */
    uint16_t icprescaler;                       /*!< channel input capture prescaler */
    uint16_t icfilter;                          /*!< channel input capture filter control */
}timer_ic_parameter_struct;

/* TIMER interrupt enable or disable */
#define TIMER_INT_UP                        TIMER_DMAINTEN_UPIE                     /*!< update interrupt */
#define TIMER_INT_CH0                       TIMER_DMAINTEN_CH0IE                    /*!< channel 0 interrupt */
#define TIMER_INT_CH1                       TIMER_DMAINTEN_CH1IE                    /*!< channel 1 interrupt */
#define TIMER_INT_CH2                       TIMER_DMAINTEN_CH2IE                    /*!< channel 2 interrupt */
#define TIMER_INT_CH3                       TIMER_DMAINTEN_CH3IE                    /*!< channel 3 interrupt */
#define TIMER_INT_CMT                       TIMER_DMAINTEN_CMTIE                    /*!< channel commutation interrupt flag */
#define TIMER_INT_TRG                       TIMER_DMAINTEN_TRGIE                    /*!< trigger interrupt */
#define TIMER_INT_BRK                       TIMER_DMAINTEN_BRKIE                    /*!< break interrupt */

/* TIMER flag */
#define TIMER_FLAG_UP                       TIMER_INTF_UPIF                         /*!< update flag */
#define TIMER_FLAG_CH0                      TIMER_INTF_CH0IF                        /*!< channel 0 flag */
#define TIMER_FLAG_CH1                      TIMER_INTF_CH1IF                        /*!< channel 1 flag */
#define TIMER_FLAG_CH2                      TIMER_INTF_CH2IF                        /*!< channel 2 flag */
#define TIMER_FLAG_CH3                      TIMER_INTF_CH3IF                        /*!< channel 3 flag */
#define TIMER_FLAG_CMT                      TIMER_INTF_CMTIF                        /*!< channel commutation flag */
#define TIMER_FLAG_TRG                      TIMER_INTF_TRGIF                        /*!< trigger flag */
#define TIMER_FLAG_BRK                      TIMER_INTF_BRKIF                        /*!< break flag */
#define TIMER_FLAG_CH0O                     TIMER_INTF_CH0OF                        /*!< channel 0 overcapture flag */
#define TIMER_FLAG_CH1O                     TIMER_INTF_CH1OF                        /*!< channel 1 overcapture flag */
#define TIMER_FLAG_CH2O                     TIMER_INTF_CH2OF                        /*!< channel 2 overcapture flag */
#define TIMER_FLAG_CH3O                     TIMER_INTF_CH3OF                        /*!< channel 3 overcapture flag */

/* TIMER interrupt flag */
#define TIMER_INT_FLAG_UP                   TIMER_INTF_UPIF                         /*!< update interrupt flag */
#define TIMER_INT_FLAG_CH0                  TIMER_INTF_CH0IF                        /*!< channel 0 interrupt flag */
#define TIMER_INT_FLAG_CH1                  TIMER_INTF_CH1IF                        /*!< channel 1 interrupt flag */
#define TIMER_INT_FLAG_CH2                  TIMER_INTF_CH2IF                        /*!< channel 2 interrupt flag */
#define TIMER_INT_FLAG_CH3                  TIMER_INTF_CH3IF                        /*!< channel 3 interrupt flag */
#define TIMER_INT_FLAG_CMT                  TIMER_INTF_CMTIF                        /*!< channel commutation interrupt flag */
#define TIMER_INT_FLAG_TRG                  TIMER_INTF_TRGIF                        /*!< trigger interrupt flag */
#define TIMER_INT_FLAG_BRK                  TIMER_INTF_BRKIF

/* TIMER DMA source enable */
#define TIMER_DMA_UPD                       ((uint16_t)TIMER_DMAINTEN_UPDEN)        /*!< update DMA enable */
#define TIMER_DMA_CH0D                      ((uint16_t)TIMER_DMAINTEN_CH0DEN)       /*!< channel 0 DMA enable */
#define TIMER_DMA_CH1D                      ((uint16_t)TIMER_DMAINTEN_CH1DEN)       /*!< channel 1 DMA enable */
#define TIMER_DMA_CH2D                      ((uint16_t)TIMER_DMAINTEN_CH2DEN)       /*!< channel 2 DMA enable */
#define TIMER_DMA_CH3D                      ((uint16_t)TIMER_DMAINTEN_CH3DEN)       /*!< channel 3 DMA enable */
#define TIMER_DMA_CMTD                      ((uint16_t)TIMER_DMAINTEN_CMTDEN)       /*!< commutation DMA request enable */
#define TIMER_DMA_TRGD                      ((uint16_t)TIMER_DMAINTEN_TRGDEN)       /*!< trigger DMA enable */

/* channel DMA request source selection */ 
#define TIMER_DMAREQUEST_UPDATEEVENT        ((uint8_t)0x00U)                        /*!< DMA request of channel y is sent when update event occurs */
#define TIMER_DMAREQUEST_CHANNELEVENT       ((uint8_t)0x01U)                        /*!< DMA request of channel y is sent when channel y event occurs */

/* DMA access base address */
#define DMACFG_DMATA(regval)                (BITS(0, 4) & ((uint32_t)(regval) << 0U))
#define TIMER_DMACFG_DMATA_CTL0             DMACFG_DMATA(0)                         /*!< DMA transfer address is TIMER_CTL0 */
#define TIMER_DMACFG_DMATA_CTL1             DMACFG_DMATA(1)                         /*!< DMA transfer address is TIMER_CTL1 */
#define TIMER_DMACFG_DMATA_SMCFG            DMACFG_DMATA(2)                         /*!< DMA transfer address is TIMER_SMCFG */
#define TIMER_DMACFG_DMATA_DMAINTEN         DMACFG_DMATA(3)                         /*!< DMA transfer address is TIMER_DMAINTEN */
#define TIMER_DMACFG_DMATA_INTF             DMACFG_DMATA(4)                         /*!< DMA transfer address is TIMER_INTF */
#define TIMER_DMACFG_DMATA_SWEVG            DMACFG_DMATA(5)                         /*!< DMA transfer address is TIMER_SWEVG */
#define TIMER_DMACFG_DMATA_CHCTL0           DMACFG_DMATA(6)                         /*!< DMA transfer address is TIMER_CHCTL0 */
#define TIMER_DMACFG_DMATA_CHCTL1           DMACFG_DMATA(7)                         /*!< DMA transfer address is TIMER_CHCTL1 */
#define TIMER_DMACFG_DMATA_CHCTL2           DMACFG_DMATA(8)                         /*!< DMA transfer address is TIMER_CHCTL2 */
#define TIMER_DMACFG_DMATA_CNT              DMACFG_DMATA(9)                         /*!< DMA transfer address is TIMER_CNT */
#define TIMER_DMACFG_DMATA_PSC              DMACFG_DMATA(10)                        /*!< DMA transfer address is TIMER_PSC */
#define TIMER_DMACFG_DMATA_CAR              DMACFG_DMATA(11)                        /*!< DMA transfer address is TIMER_CAR */
#define TIMER_DMACFG_DMATA_CREP             DMACFG_DMATA(12)                        /*!< DMA transfer address is TIMER_CREP */
#define TIMER_DMACFG_DMATA_CH0CV            DMACFG_DMATA(13)                        /*!< DMA transfer address is TIMER_CH0CV */
#define TIMER_DMACFG_DMATA_CH1CV            DMACFG_DMATA(14)                        /*!< DMA transfer address is TIMER_CH1CV */
#define TIMER_DMACFG_DMATA_CH2CV            DMACFG_DMATA(15)                        /*!< DMA transfer address is TIMER_CH2CV */
#define TIMER_DMACFG_DMATA_CH3CV            DMACFG_DMATA(16)                        /*!< DMA transfer address is TIMER_CH3CV */
#define TIMER_DMACFG_DMATA_CCHP             DMACFG_DMATA(17)                        /*!< DMA transfer address is TIMER_CCHP */
#define TIMER_DMACFG_DMATA_DMACFG           DMACFG_DMATA(18)                        /*!< DMA transfer address is TIMER_DMACFG */
#define TIMER_DMACFG_DMATA_DMATB            DMACFG_DMATA(19)                        /*!< DMA transfer address is TIMER_DMATB */

/* DMA access burst length */
#define DMACFG_DMATC(regval)                (BITS(8, 12) & ((uint32_t)(regval) << 8U))
#define TIMER_DMACFG_DMATC_1TRANSFER        DMACFG_DMATC(0)                         /*!< DMA transfer 1 time */
#define TIMER_DMACFG_DMATC_2TRANSFER        DMACFG_DMATC(1)                         /*!< DMA transfer 2 times */
#define TIMER_DMACFG_DMATC_3TRANSFER        DMACFG_DMATC(2)                         /*!< DMA transfer 3 times */
#define TIMER_DMACFG_DMATC_4TRANSFER        DMACFG_DMATC(3)                         /*!< DMA transfer 4 times */
#define TIMER_DMACFG_DMATC_5TRANSFER        DMACFG_DMATC(4)                         /*!< DMA transfer 5 times */
#define TIMER_DMACFG_DMATC_6TRANSFER        DMACFG_DMATC(5)                         /*!< DMA transfer 6 times */
#define TIMER_DMACFG_DMATC_7TRANSFER        DMACFG_DMATC(6)                         /*!< DMA transfer 7 times */
#define TIMER_DMACFG_DMATC_8TRANSFER        DMACFG_DMATC(7)                         /*!< DMA transfer 8 times */
#define TIMER_DMACFG_DMATC_9TRANSFER        DMACFG_DMATC(8)                         /*!< DMA transfer 9 times */
#define TIMER_DMACFG_DMATC_10TRANSFER       DMACFG_DMATC(9)                         /*!< DMA transfer 10 times */
#define TIMER_DMACFG_DMATC_11TRANSFER       DMACFG_DMATC(10)                        /*!< DMA transfer 11 times */
#define TIMER_DMACFG_DMATC_12TRANSFER       DMACFG_DMATC(11)                        /*!< DMA transfer 12 times */
#define TIMER_DMACFG_DMATC_13TRANSFER       DMACFG_DMATC(12)                        /*!< DMA transfer 13 times */
#define TIMER_DMACFG_DMATC_14TRANSFER       DMACFG_DMATC(13)                        /*!< DMA transfer 14 times */
#define TIMER_DMACFG_DMATC_15TRANSFER       DMACFG_DMATC(14)                        /*!< DMA transfer 15 times */
#define TIMER_DMACFG_DMATC_16TRANSFER       DMACFG_DMATC(15)                        /*!< DMA transfer 16 times */
#define TIMER_DMACFG_DMATC_17TRANSFER       DMACFG_DMATC(16)                        /*!< DMA transfer 17 times */
#define TIMER_DMACFG_DMATC_18TRANSFER       DMACFG_DMATC(17)                        /*!< DMA transfer 18 times */

/* TIMER software event generation source */
#define TIMER_EVENT_SRC_UPG                 ((uint16_t)0x0001U)                     /*!< update event generation */
#define TIMER_EVENT_SRC_CH0G                ((uint16_t)0x0002U)                     /*!< channel 0 capture or compare event generation */
#define TIMER_EVENT_SRC_CH1G                ((uint16_t)0x0004U)                     /*!< channel 1 capture or compare event generation */
#define TIMER_EVENT_SRC_CH2G                ((uint16_t)0x0008U)                     /*!< channel 2 capture or compare event generation */
#define TIMER_EVENT_SRC_CH3G                ((uint16_t)0x0010U)                     /*!< channel 3 capture or compare event generation */
#define TIMER_EVENT_SRC_CMTG                ((uint16_t)0x0020U)                     /*!< channel commutation event generation */
#define TIMER_EVENT_SRC_TRGG                ((uint16_t)0x0040U)                     /*!< trigger event generation */
#define TIMER_EVENT_SRC_BRKG                ((uint16_t)0x0080U)                     /*!< break event generation */

/* center-aligned mode selection */
#define CTL0_CAM(regval)                    ((uint16_t)(BITS(5, 6) & ((uint32_t)(regval) << 5U)))
#define TIMER_COUNTER_EDGE                  CTL0_CAM(0)                             /*!< edge-aligned mode */
#define TIMER_COUNTER_CENTER_DOWN           CTL0_CAM(1)                             /*!< center-aligned and counting down assert mode */
#define TIMER_COUNTER_CENTER_UP             CTL0_CAM(2)                             /*!< center-aligned and counting up assert mode */
#define TIMER_COUNTER_CENTER_BOTH           CTL0_CAM(3)                             /*!< center-aligned and counting up/down assert mode */

/* TIMER prescaler reload mode */
#define TIMER_PSC_RELOAD_NOW                ((uint32_t)0x00000000U)                 /*!< the prescaler is loaded right now */
#define TIMER_PSC_RELOAD_UPDATE             ((uint32_t)0x00000001U)                 /*!< the prescaler is loaded at the next update event */

/* count direction */
#define TIMER_COUNTER_UP                    ((uint16_t)0x0000U)                     /*!< counter up direction */
#define TIMER_COUNTER_DOWN                  ((uint16_t)TIMER_CTL0_DIR)              /*!< counter down direction */

/* specify division ratio between TIMER clock and dead-time and sampling clock */
#define CTL0_CKDIV(regval)                  ((uint16_t)(BITS(8, 9) & ((uint32_t)(regval) << 8U)))
#define TIMER_CKDIV_DIV1                    CTL0_CKDIV(0)                           /*!< clock division value is 1,fDTS=fTIMER_CK */
#define TIMER_CKDIV_DIV2                    CTL0_CKDIV(1)                           /*!< clock division value is 2,fDTS= fTIMER_CK/2 */
#define TIMER_CKDIV_DIV4                    CTL0_CKDIV(2)                           /*!< clock division value is 4, fDTS= fTIMER_CK/4 */

/* single pulse mode */
#define TIMER_SP_MODE_SINGLE                ((uint32_t)0x00000000U)                 /*!< single pulse mode */
#define TIMER_SP_MODE_REPETITIVE            ((uint32_t)0x00000001U)                 /*!< repetitive pulse mode */

/* update source */
#define TIMER_UPDATE_SRC_REGULAR            ((uint32_t)0x00000000U)                 /*!< update generate only by counter overflow/underflow */
#define TIMER_UPDATE_SRC_GLOBAL             ((uint32_t)0x00000001U)                 /*!< update generate by setting of UPG bit or the counter overflow/underflow,or the slave mode controller trigger */

/* run mode off-state configure */
#define TIMER_ROS_STATE_ENABLE              ((uint16_t)TIMER_CCHP_ROS)              /*!< when POEN bit is set, the channel output signals (CHx_O/CHx_ON) are enabled, with relationship to CHxEN/CHxNEN bits */
#define TIMER_ROS_STATE_DISABLE             ((uint16_t)0x0000U)                     /*!< when POEN bit is set, the channel output signals (CHx_O/CHx_ON) are disabled */

/* idle mode off-state configure */                                                 
#define TIMER_IOS_STATE_ENABLE              ((uint16_t)TIMER_CCHP_IOS)              /*!< when POEN bit is reset, the channel output signals (CHx_O/CHx_ON) are enabled, with relationship to CHxEN/CHxNEN bits */
#define TIMER_IOS_STATE_DISABLE             ((uint16_t)0x0000U)                     /*!< when POEN bit is reset, the channel output signals (CHx_O/CHx_ON) are disabled */

/* break input polarity */
#define TIMER_BREAK_POLARITY_LOW            ((uint16_t)0x0000U)                     /*!< break input polarity is low */
#define TIMER_BREAK_POLARITY_HIGH           ((uint16_t)TIMER_CCHP_BRKP)             /*!< break input polarity is high */

/* output automatic enable */
#define TIMER_OUTAUTO_ENABLE                ((uint16_t)TIMER_CCHP_OAEN)             /*!< output automatic enable */
#define TIMER_OUTAUTO_DISABLE               ((uint16_t)0x0000U)                     /*!< output automatic disable */

/* complementary register protect control */
#define CCHP_PROT(regval)                   ((uint16_t)(BITS(8, 9) & ((uint32_t)(regval) << 8U)))
#define TIMER_CCHP_PROT_OFF                 CCHP_PROT(0)                            /*!< protect disable */
#define TIMER_CCHP_PROT_0                   CCHP_PROT(1)                            /*!< PROT mode 0 */
#define TIMER_CCHP_PROT_1                   CCHP_PROT(2)                            /*!< PROT mode 1 */
#define TIMER_CCHP_PROT_2                   CCHP_PROT(3)                            /*!< PROT mode 2 */

/* break input enable */
#define TIMER_BREAK_ENABLE                  ((uint16_t)TIMER_CCHP_BRKEN)            /*!< break input enable */
#define TIMER_BREAK_DISABLE                 ((uint16_t)0x0000U)                     /*!< break input disable */

/* TIMER channel n(n=0,1,2,3) */
#define TIMER_CH_0                          ((uint16_t)0x0000U)                     /*!< TIMER channel 0(TIMERx(x=0..4,7..13)) */
#define TIMER_CH_1                          ((uint16_t)0x0001U)                     /*!< TIMER channel 1(TIMERx(x=0..4,7,8,11)) */
#define TIMER_CH_2                          ((uint16_t)0x0002U)                     /*!< TIMER channel 2(TIMERx(x=0..4,7)) */
#define TIMER_CH_3                          ((uint16_t)0x0003U)                     /*!< TIMER channel 3(TIMERx(x=0..4,7)) */

/* channel enable state*/
#define TIMER_CCX_ENABLE                    ((uint32_t)0x00000001U)                 /*!< channel enable */
#define TIMER_CCX_DISABLE                   ((uint32_t)0x00000000U)                 /*!< channel disable */

/* channel complementary output enable state*/
#define TIMER_CCXN_ENABLE                   ((uint16_t)0x0004U)                     /*!< channel complementary enable */
#define TIMER_CCXN_DISABLE                  ((uint16_t)0x0000U)                     /*!< channel complementary disable */

/* channel output polarity */
#define TIMER_OC_POLARITY_HIGH              ((uint16_t)0x0000U)                     /*!< channel output polarity is high */
#define TIMER_OC_POLARITY_LOW               ((uint16_t)0x0002U)                     /*!< channel output polarity is low */

/* channel complementary output polarity */
#define TIMER_OCN_POLARITY_HIGH             ((uint16_t)0x0000U)                     /*!< channel complementary output polarity is high */
#define TIMER_OCN_POLARITY_LOW              ((uint16_t)0x0008U)                     /*!< channel complementary output polarity is low */

/* idle state of channel output */ 
#define TIMER_OC_IDLE_STATE_HIGH            ((uint16_t)0x0100)                      /*!< idle state of channel output is high */
#define TIMER_OC_IDLE_STATE_LOW             ((uint16_t)0x0000)                      /*!< idle state of channel output is low */

/* idle state of channel complementary output */ 
#define TIMER_OCN_IDLE_STATE_HIGH           ((uint16_t)0x0200U)                     /*!< idle state of channel complementary output is high */
#define TIMER_OCN_IDLE_STATE_LOW            ((uint16_t)0x0000U)                     /*!< idle state of channel complementary output is low */

/* channel output compare mode */
#define TIMER_OC_MODE_TIMING                ((uint16_t)0x0000U)                     /*!< timing mode */
#define TIMER_OC_MODE_ACTIVE                ((uint16_t)0x0010U)                     /*!< active mode */
#define TIMER_OC_MODE_INACTIVE              ((uint16_t)0x0020U)                     /*!< inactive mode */
#define TIMER_OC_MODE_TOGGLE                ((uint16_t)0x0030U)                     /*!< toggle mode */
#define TIMER_OC_MODE_LOW                   ((uint16_t)0x0040U)                     /*!< force low mode */
#define TIMER_OC_MODE_HIGH                  ((uint16_t)0x0050U)                     /*!< force high mode */
#define TIMER_OC_MODE_PWM0                  ((uint16_t)0x0060U)                     /*!< PWM0 mode */
#define TIMER_OC_MODE_PWM1                  ((uint16_t)0x0070U)                     /*!< PWM1 mode*/

/* channel output compare shadow enable */
#define TIMER_OC_SHADOW_ENABLE              ((uint16_t)0x0008U)                     /*!< channel output shadow state enable */
#define TIMER_OC_SHADOW_DISABLE             ((uint16_t)0x0000U)                     /*!< channel output shadow state disable */

/* channel output compare fast enable */
#define TIMER_OC_FAST_ENABLE                ((uint16_t)0x0004)                      /*!< channel output fast function enable */
#define TIMER_OC_FAST_DISABLE               ((uint16_t)0x0000)                      /*!< channel output fast function disable */

/* channel output compare clear enable */
#define TIMER_OC_CLEAR_ENABLE               ((uint16_t)0x0080U)                     /*!< channel output clear function enable */
#define TIMER_OC_CLEAR_DISABLE              ((uint16_t)0x0000U)                     /*!< channel output clear function disable */

/* channel control shadow register update control */ 
#define TIMER_UPDATECTL_CCU                 ((uint32_t)0x00000000U)                 /*!< the shadow registers are updated when CMTG bit is set */
#define TIMER_UPDATECTL_CCUTRI              ((uint32_t)0x00000001U)                        /*!< the shadow registers update by when CMTG bit is set or an rising edge of TRGI occurs */

/* channel input capture polarity */
#define TIMER_IC_POLARITY_RISING            ((uint16_t)0x0000U)                     /*!< input capture rising edge */
#define TIMER_IC_POLARITY_FALLING           ((uint16_t)0x0002U)                     /*!< input capture falling edge */
#define TIMER_IC_POLARITY_BOTH_EDGE         ((uint16_t)0x000AU)                     /*!< input capture both edge */

/* TIMER input capture selection */
#define TIMER_IC_SELECTION_DIRECTTI         ((uint16_t)0x0001U)                     /*!< channel y is configured as input and icy is mapped on CIy */
#define TIMER_IC_SELECTION_INDIRECTTI       ((uint16_t)0x0002U)                     /*!< channel y is configured as input and icy is mapped on opposite input */
#define TIMER_IC_SELECTION_ITS              ((uint16_t)0x0003U)                     /*!< channel y is configured as input and icy is mapped on ITS */

/* channel input capture prescaler */
#define TIMER_IC_PSC_DIV1                   ((uint16_t)0x0000U)                     /*!< no prescaler */
#define TIMER_IC_PSC_DIV2                   ((uint16_t)0x0004U)                     /*!< divided by 2 */
#define TIMER_IC_PSC_DIV4                   ((uint16_t)0x0008U)                     /*!< divided by 4*/
#define TIMER_IC_PSC_DIV8                   ((uint16_t)0x000CU)                     /*!< divided by 8 */

/* trigger selection */
#define SMCFG_TRGSEL(regval)                 (BITS(4, 6) & ((uint32_t)(regval) << 4U))
#define TIMER_SMCFG_TRGSEL_ITI0              SMCFG_TRGSEL(0)                        /*!< internal trigger 0 */
#define TIMER_SMCFG_TRGSEL_ITI1              SMCFG_TRGSEL(1)                        /*!< internal trigger 1 */
#define TIMER_SMCFG_TRGSEL_ITI2              SMCFG_TRGSEL(2)                        /*!< internal trigger 2 */
#define TIMER_SMCFG_TRGSEL_ITI3              SMCFG_TRGSEL(3)                        /*!< internal trigger 3 */
#define TIMER_SMCFG_TRGSEL_CI0F_ED           SMCFG_TRGSEL(4)                        /*!< TI0 Edge Detector */
#define TIMER_SMCFG_TRGSEL_CI0FE0            SMCFG_TRGSEL(5)                        /*!< filtered TIMER input 0 */
#define TIMER_SMCFG_TRGSEL_CI1FE1            SMCFG_TRGSEL(6)                        /*!< filtered TIMER input 1 */
#define TIMER_SMCFG_TRGSEL_ETIFP             SMCFG_TRGSEL(7)                        /*!< external trigger */

/* master mode control */
#define CTL1_MMC(regval)                    (BITS(4, 6) & ((uint32_t)(regval) << 4U))
#define TIMER_TRI_OUT_SRC_RESET             CTL1_MMC(0)                             /*!< the UPG bit as trigger output */
#define TIMER_TRI_OUT_SRC_ENABLE            CTL1_MMC(1)                             /*!< the counter enable signal TIMER_CTL0_CEN as trigger output */
#define TIMER_TRI_OUT_SRC_UPDATE            CTL1_MMC(2)                             /*!< update event as trigger output */
#define TIMER_TRI_OUT_SRC_CH0               CTL1_MMC(3)                             /*!< a capture or a compare match occurred in channal0 as trigger output TRGO */
#define TIMER_TRI_OUT_SRC_O0CPRE            CTL1_MMC(4)                             /*!< O0CPRE as trigger output */
#define TIMER_TRI_OUT_SRC_O1CPRE            CTL1_MMC(5)                             /*!< O1CPRE as trigger output */
#define TIMER_TRI_OUT_SRC_O2CPRE            CTL1_MMC(6)                             /*!< O2CPRE as trigger output */
#define TIMER_TRI_OUT_SRC_O3CPRE            CTL1_MMC(7)                             /*!< O3CPRE as trigger output */

/* slave mode control */
#define SMCFG_SMC(regval)                   (BITS(0, 2) & ((uint32_t)(regval) << 0U)) 
#define TIMER_SLAVE_MODE_DISABLE            SMCFG_SMC(0)                            /*!< slave mode disable */
#define TIMER_QUAD_DECODER_MODE0            SMCFG_SMC(1)                            /*!< quadrature decoder mode 0 */
#define TIMER_QUAD_DECODER_MODE1            SMCFG_SMC(2)                            /*!< quadrature decoder mode 1 */
#define TIMER_QUAD_DECODER_MODE2            SMCFG_SMC(3)                            /*!< quadrature decoder mode 2 */
#define TIMER_SLAVE_MODE_RESTART            SMCFG_SMC(4)                            /*!< restart mode */
#define TIMER_SLAVE_MODE_PAUSE              SMCFG_SMC(5)                            /*!< pause mode */
#define TIMER_SLAVE_MODE_EVENT              SMCFG_SMC(6)                            /*!< event mode */
#define TIMER_SLAVE_MODE_EXTERNAL0          SMCFG_SMC(7)                            /*!< external clock mode 0 */

/* master slave mode selection */ 
#define TIMER_MASTER_SLAVE_MODE_ENABLE      ((uint32_t)0x00000000U)                 /*!< master slave mode enable */
#define TIMER_MASTER_SLAVE_MODE_DISABLE     ((uint32_t)0x00000001U)                 /*!< master slave mode disable */

/* external trigger prescaler */
#define SMCFG_ETPSC(regval)                 (BITS(12, 13) & ((uint32_t)(regval) << 12U))
#define TIMER_EXT_TRI_PSC_OFF               SMCFG_ETPSC(0)                          /*!< no divided */
#define TIMER_EXT_TRI_PSC_DIV2              SMCFG_ETPSC(1)                          /*!< divided by 2 */
#define TIMER_EXT_TRI_PSC_DIV4              SMCFG_ETPSC(2)                          /*!< divided by 4 */
#define TIMER_EXT_TRI_PSC_DIV8              SMCFG_ETPSC(3)                          /*!< divided by 8 */

/* external trigger polarity */
#define TIMER_ETP_FALLING                   TIMER_SMCFG_ETP                         /*!< active low or falling edge active */
#define TIMER_ETP_RISING                    ((uint32_t)0x00000000U)                 /*!< active high or rising edge active */

/* channel 0 trigger input selection */ 
#define TIMER_HALLINTERFACE_ENABLE          ((uint32_t)0x00000000U)                 /*!< TIMER hall sensor mode enable */
#define TIMER_HALLINTERFACE_DISABLE         ((uint32_t)0x00000001U)                 /*!< TIMER hall sensor mode disable */

/* timer1 internal trigger input1 remap */
#define TIMER1_IRMP(regval)                 (BITS(10, 11) & ((uint32_t)(regval) << 10U))       
#define TIMER1_ITI1_RMP_TIMER7_TRGO         TIMER1_IRMP(0)                          /*!< timer1 internal trigger input 1 remap to TIMER7_TRGO */
#define TIMER1_ITI1_RMP_ETHERNET_PTP        TIMER1_IRMP(1)                          /*!< timer1 internal trigger input 1 remap to ethernet PTP */
#define TIMER1_ITI1_RMP_USB_FS_SOF          TIMER1_IRMP(2)                          /*!< timer1 internal trigger input 1 remap to USB FS SOF */
#define TIMER1_ITI1_RMP_USB_HS_SOF          TIMER1_IRMP(3)                          /*!< timer1 internal trigger input 1 remap to USB HS SOF */

/* timer4 channel 3 input remap */
#define TIMER4_IRMP(regval)                 (BITS(6, 7) & ((uint32_t)(regval) << 6U))          
#define TIMER4_CI3_RMP_GPIO                 TIMER4_IRMP(0)                          /*!< timer4 channel 3 input remap to GPIO pin */
#define TIMER4_CI3_RMP_IRC32K               TIMER4_IRMP(1)                          /*!< timer4 channel 3 input remap to IRC32K */
#define TIMER4_CI3_RMP_LXTAL                TIMER4_IRMP(2)                          /*!< timer4 channel 3 input remap to  LXTAL */
#define TIMER4_CI3_RMP_RTC_WAKEUP_INT       TIMER4_IRMP(3)                          /*!< timer4 channel 3 input remap to RTC wakeup interrupt */

/* timer10 internal trigger input1 remap */
#define TIMER10_IRMP(regval)                (BITS(0, 1) & ((uint32_t)(regval) << 0U))
#define TIMER10_ITI1_RMP_GPIO               TIMER10_IRMP(0)                         /*!< timer10 internal trigger input1 remap based on GPIO setting */
#define TIMER10_ITI1_RMP_RTC_HXTAL_DIV      TIMER10_IRMP(2)                         /*!< timer10 internal trigger input1 remap  HXTAL _DIV(clock used for RTC which is HXTAL clock divided by RTCDIV bits in RCU_CFG0 register) */

/* timerx(x=0,1,2,13,14,15,16) write cc register selection */
#define TIMER_CHVSEL_ENABLE                  ((uint16_t)0x0002U)                    /*!< write CHxVAL register selection enable  */
#define TIMER_CHVSEL_DISABLE                 ((uint16_t)0x0000U)                    /*!< write CHxVAL register selection disable */

/* the output value selection */
#define TIMER_OUTSEL_ENABLE                 ((uint16_t)0x0001U)                     /*!< output value selection enable */
#define TIMER_OUTSEL_DISABLE                ((uint16_t)0x0000U)                     /*!< output value selection disable */

/* function declarations */
/* TIMER timebase*/
/* deinit a TIMER */
void timer_deinit(uint32_t timer_periph);
/* initialize TIMER init parameter struct */
void timer_struct_para_init(timer_parameter_struct* initpara);
/* initialize TIMER counter */
void timer_init(uint32_t timer_periph, timer_parameter_struct* initpara);
/* enable a TIMER */
void timer_enable(uint32_t timer_periph);
/* disable a TIMER */
void timer_disable(uint32_t timer_periph);
/* enable the auto reload shadow function */
void timer_auto_reload_shadow_enable(uint32_t timer_periph);
/* disable the auto reload shadow function */
void timer_auto_reload_shadow_disable(uint32_t timer_periph);
/* enable the update event */
void timer_update_event_enable(uint32_t timer_periph);
/* disable the update event */
void timer_update_event_disable(uint32_t timer_periph);
/* set TIMER counter alignment mode */
void timer_counter_alignment(uint32_t timer_periph, uint16_t aligned);
/* set TIMER counter up direction */
void timer_counter_up_direction(uint32_t timer_periph);
/* set TIMER counter down direction */
void timer_counter_down_direction(uint32_t timer_periph);
/* configure TIMER prescaler */
void timer_prescaler_config(uint32_t timer_periph, uint16_t prescaler, uint8_t pscreload);
/* configure TIMER repetition register value */
void timer_repetition_value_config(uint32_t timer_periph, uint16_t repetition);
/* configure TIMER autoreload register value */
void timer_autoreload_value_config(uint32_t timer_periph,uint32_t autoreload);
/* configure TIMER counter register value */
void timer_counter_value_config(uint32_t timer_periph , uint32_t counter);
/* read TIMER counter value */
uint32_t timer_counter_read(uint32_t timer_periph);
/* read TIMER prescaler value */
uint16_t timer_prescaler_read(uint32_t timer_periph);
/* configure TIMER single pulse mode */
void timer_single_pulse_mode_config(uint32_t timer_periph, uint32_t spmode);
/* configure TIMER update source */
void timer_update_source_config(uint32_t timer_periph, uint32_t update);

/* timer DMA and event*/
/* enable the TIMER DMA */
void timer_dma_enable(uint32_t timer_periph, uint16_t dma);
/* disable the TIMER DMA */
void timer_dma_disable(uint32_t timer_periph, uint16_t dma);
/* channel DMA request source selection */
void timer_channel_dma_request_source_select(uint32_t timer_periph, uint8_t dma_request);
/* configure the TIMER DMA transfer */
void timer_dma_transfer_config(uint32_t timer_periph,uint32_t dma_baseaddr, uint32_t dma_lenth);
/* software generate events */
void timer_event_software_generate(uint32_t timer_periph, uint16_t event);

/* TIMER channel complementary protection */
/* initialize TIMER break parameter struct */
void timer_break_struct_para_init(timer_break_parameter_struct* breakpara);
/* configure TIMER break function */
void timer_break_config(uint32_t timer_periph, timer_break_parameter_struct* breakpara);
/* enable TIMER break function */
void timer_break_enable(uint32_t timer_periph);
/* disable TIMER break function */
void timer_break_disable(uint32_t timer_periph);
/* enable TIMER output automatic function */
void timer_automatic_output_enable(uint32_t timer_periph);
/* disable TIMER output automatic function */
void timer_automatic_output_disable(uint32_t timer_periph);
/* enable or disable TIMER primary output function */
void timer_primary_output_config(uint32_t timer_periph, ControlStatus newvalue);
/* enable or disable channel capture/compare control shadow register */
void timer_channel_control_shadow_config(uint32_t timer_periph, ControlStatus newvalue);
/* configure TIMER channel control shadow register update control */
void timer_channel_control_shadow_update_config(uint32_t timer_periph, uint8_t ccuctl);

/* TIMER channel output */
/* initialize TIMER channel output parameter struct */
void timer_channel_output_struct_para_init(timer_oc_parameter_struct* ocpara);
/* configure TIMER channel output function */
void timer_channel_output_config(uint32_t timer_periph,uint16_t channel, timer_oc_parameter_struct* ocpara);
/* configure TIMER channel output compare mode */
void timer_channel_output_mode_config(uint32_t timer_periph, uint16_t channel,uint16_t ocmode);
/* configure TIMER channel output pulse value */
void timer_channel_output_pulse_value_config(uint32_t timer_periph, uint16_t channel, uint32_t pulse);
/* configure TIMER channel output shadow function */
void timer_channel_output_shadow_config(uint32_t timer_periph, uint16_t channel, uint16_t ocshadow);
/* configure TIMER channel output fast function */
void timer_channel_output_fast_config(uint32_t timer_periph, uint16_t channel, uint16_t ocfast);
/* configure TIMER channel output clear function */
void timer_channel_output_clear_config(uint32_t timer_periph,uint16_t channel,uint16_t occlear);
/* configure TIMER channel output polarity */
void timer_channel_output_polarity_config(uint32_t timer_periph, uint16_t channel, uint16_t ocpolarity);
/* configure TIMER channel complementary output polarity */
void timer_channel_complementary_output_polarity_config(uint32_t timer_periph, uint16_t channel, uint16_t ocnpolarity);
/* configure TIMER channel enable state */
void timer_channel_output_state_config(uint32_t timer_periph, uint16_t channel, uint32_t state);
/* configure TIMER channel complementary output enable state */
void timer_channel_complementary_output_state_config(uint32_t timer_periph, uint16_t channel, uint16_t ocnstate);

/* TIMER channel input */
/* initialize TIMER channel input parameter struct */
void timer_channel_input_struct_para_init(timer_ic_parameter_struct* icpara);
/* configure TIMER input capture parameter */
void timer_input_capture_config(uint32_t timer_periph, uint16_t channel, timer_ic_parameter_struct* icpara);
/* configure TIMER channel input capture prescaler value */
void timer_channel_input_capture_prescaler_config(uint32_t timer_periph, uint16_t channel, uint16_t prescaler);
/* read TIMER channel capture compare register value */
uint32_t timer_channel_capture_value_register_read(uint32_t timer_periph, uint16_t channel);
/* configure TIMER input pwm capture function */
void timer_input_pwm_capture_config(uint32_t timer_periph, uint16_t channel, timer_ic_parameter_struct* icpwm);
/* configure TIMER hall sensor mode */
void timer_hall_mode_config(uint32_t timer_periph, uint32_t hallmode);

/* TIMER master and slave */
/* select TIMER input trigger source */
void timer_input_trigger_source_select(uint32_t timer_periph, uint32_t intrigger);
/* select TIMER master mode output trigger source */
void timer_master_output_trigger_source_select(uint32_t timer_periph, uint32_t outrigger);
/* select TIMER slave mode */
void timer_slave_mode_select(uint32_t timer_periph,uint32_t slavemode);
/* configure TIMER master slave mode */
void timer_master_slave_mode_config(uint32_t timer_periph, uint32_t masterslave);
/* configure TIMER external trigger input */
void timer_external_trigger_config(uint32_t timer_periph, uint32_t extprescaler, uint32_t extpolarity, uint32_t extfilter);
/* configure TIMER quadrature decoder mode */
void timer_quadrature_decoder_mode_config(uint32_t timer_periph, uint32_t decomode, uint16_t ic0polarity, uint16_t ic1polarity);
/* configure TIMER internal clock mode */
void timer_internal_clock_config(uint32_t timer_periph);
/* configure TIMER the internal trigger as external clock input */
void timer_internal_trigger_as_external_clock_config(uint32_t timer_periph, uint32_t intrigger);
/* configure TIMER the external trigger as external clock input */
void timer_external_trigger_as_external_clock_config(uint32_t timer_periph, uint32_t extrigger, uint16_t extpolarity,uint32_t extfilter);
/* configure TIMER the external clock mode 0 */
void timer_external_clock_mode0_config(uint32_t timer_periph, uint32_t extprescaler, uint32_t extpolarity, uint32_t extfilter);
/* configure TIMER the external clock mode 1 */
void timer_external_clock_mode1_config(uint32_t timer_periph, uint32_t extprescaler, uint32_t extpolarity, uint32_t extfilter);
/* disable TIMER the external clock mode 1 */
void timer_external_clock_mode1_disable(uint32_t timer_periph);
/* configure TIMER channel remap function */
void timer_channel_remap_config(uint32_t timer_periph,uint32_t remap);

/* TIMER configure */
/* configure TIMER write CHxVAL register selection */
void timer_write_chxval_register_config(uint32_t timer_periph, uint16_t ccsel);
/* configure TIMER output value selection */
void timer_output_value_selection_config(uint32_t timer_periph, uint16_t outsel);

/* TIMER interrupt and flag*/
/* get TIMER flags */
FlagStatus timer_flag_get(uint32_t timer_periph, uint32_t flag);
/* clear TIMER flags */
void timer_flag_clear(uint32_t timer_periph, uint32_t flag);
/* enable the TIMER interrupt */
void timer_interrupt_enable(uint32_t timer_periph, uint32_t interrupt);
/* disable the TIMER interrupt */
void timer_interrupt_disable(uint32_t timer_periph, uint32_t interrupt);
/* get timer interrupt flag */
FlagStatus timer_interrupt_flag_get(uint32_t timer_periph, uint32_t interrupt);
/* clear TIMER interrupt flag */
void timer_interrupt_flag_clear(uint32_t timer_periph, uint32_t interrupt);

#endif /* GD32F4XX_TIMER_H */
