/*!
    \file    gd32f4xx_dac.h
    \brief   definitions for the DAC

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

#ifndef GD32F4XX_DAC_H
#define GD32F4XX_DAC_H

#include "gd32f4xx.h"

/* DACx(x=0) definitions */
#define DAC0                              (DAC_BASE)

/* registers definitions */
#define DAC_CTL0(dacx)                    REG32((dacx) + 0x00000000U)          /*!< DACx control register 0 */
#define DAC_SWT(dacx)                     REG32((dacx) + 0x00000004U)          /*!< DACx software trigger register */
#define DAC_OUT0_R12DH(dacx)              REG32((dacx) + 0x00000008U)          /*!< DACx_OUT0 12-bit right-aligned data holding register */
#define DAC_OUT0_L12DH(dacx)              REG32((dacx) + 0x0000000CU)          /*!< DACx_OUT0 12-bit left-aligned data holding register */
#define DAC_OUT0_R8DH(dacx)               REG32((dacx) + 0x00000010U)          /*!< DACx_OUT0 8-bit right-aligned data holding register */
#define DAC_OUT1_R12DH(dacx)              REG32((dacx) + 0x00000014U)          /*!< DACx_OUT1 12-bit right-aligned data holding register */
#define DAC_OUT1_L12DH(dacx)              REG32((dacx) + 0x00000018U)          /*!< DACx_OUT1 12-bit left-aligned data holding register */
#define DAC_OUT1_R8DH(dacx)               REG32((dacx) + 0x0000001CU)          /*!< DACx_OUT1 8-bit right-aligned data holding register */
#define DACC_R12DH(dacx)                  REG32((dacx) + 0x00000020U)          /*!< DACx concurrent mode 12-bit right-aligned data holding register */
#define DACC_L12DH(dacx)                  REG32((dacx) + 0x00000024U)          /*!< DACx concurrent mode 12-bit left-aligned data holding register */
#define DACC_R8DH(dacx)                   REG32((dacx) + 0x00000028U)          /*!< DACx concurrent mode 8-bit right-aligned data holding register */
#define DAC_OUT0_DO(dacx)                 REG32((dacx) + 0x0000002CU)          /*!< DACx_OUT0 data output register */
#define DAC_OUT1_DO(dacx)                 REG32((dacx) + 0x00000030U)          /*!< DACx_OUT1 data output register */
#define DAC_STAT0(dacx)                   REG32((dacx) + 0x00000034U)          /*!< DACx status register 0 */

/* bits definitions */
/* DAC_CTL0 */
#define DAC_CTL0_DEN0                     BIT(0)                               /*!< DACx_OUT0 enable */
#define DAC_CTL0_DBOFF0                   BIT(1)                               /*!< DACx_OUT0 output buffer turn off */
#define DAC_CTL0_DTEN0                    BIT(2)                               /*!< DACx_OUT0 trigger enable */
#define DAC_CTL0_DTSEL0                   BITS(3,5)                            /*!< DACx_OUT0 trigger selection */
#define DAC_CTL0_DWM0                     BITS(6,7)                            /*!< DACx_OUT0 noise wave mode */
#define DAC_CTL0_DWBW0                    BITS(8,11)                           /*!< DACx_OUT0 noise wave bit width */
#define DAC_CTL0_DDMAEN0                  BIT(12)                              /*!< DACx_OUT0 DMA enable */
#define DAC_CTL0_DDUDRIE0                 BIT(13)                              /*!< DACx_OUT0 DMA underrun interrupt enable */

#define DAC_CTL0_DEN1                     BIT(16)                              /*!< DACx_OUT1 enable */ 
#define DAC_CTL0_DBOFF1                   BIT(17)                              /*!< DACx_OUT1 output buffer turn off */
#define DAC_CTL0_DTEN1                    BIT(18)                              /*!< DACx_OUT1 trigger enable */
#define DAC_CTL0_DTSEL1                   BITS(19,21)                          /*!< DACx_OUT1 trigger selection */
#define DAC_CTL0_DWM1                     BITS(22,23)                          /*!< DACx_OUT1 noise wave mode */
#define DAC_CTL0_DWBW1                    BITS(24,27)                          /*!< DACx_OUT1 noise wave bit width */
#define DAC_CTL0_DDMAEN1                  BIT(28)                              /*!< DACx_OUT1 DMA enable */
#define DAC_CTL0_DDUDRIE1                 BIT(29)                              /*!< DACx_OUT1 DMA underrun interrupt enable */

/* DAC_SWT */
#define DAC_SWT_SWTR0                     BIT(0)                               /*!< DACx_OUT0 software trigger */
#define DAC_SWT_SWTR1                     BIT(1)                               /*!< DACx_OUT1 software trigger */

/* DAC0_OUT0_R12DH */
#define DAC_OUT0_DH_R12                   BITS(0,11)                           /*!< DACx_OUT0 12-bit right-aligned data */

/* DAC0_OUT0_L12DH */
#define DAC_OUT0_DH_L12                   BITS(4,15)                           /*!< DACx_OUT0 12-bit left-aligned data */

/* DAC0_OUT0_R8DH */
#define DAC_OUT0_DH_R8                    BITS(0,7)                            /*!< DACx_OUT0 8-bit right-aligned data */

/* DAC1_OUT1_R12DH */
#define DAC_OUT1_DH_R12                   BITS(0,11)                           /*!< DACx_OUT1 12-bit right-aligned data */

/* DAC1_OUT1_L12DH */
#define DAC_OUT1_DH_L12                   BITS(4,15)                           /*!< DACx_OUT1 12-bit left-aligned data */

/* DAC1_OUT1_R8DH */
#define DAC_OUT1_DH_R8                    BITS(0,7)                            /*!< DACx_OUT1 8-bit right-aligned data */

/* DACC_R12DH */
#define DACC_OUT0_DH_R12                  BITS(0,11)                           /*!< DAC concurrent mode DACx_OUT0 12-bit right-aligned data */
#define DACC_OUT1_DH_R12                  BITS(16,27)                          /*!< DAC concurrent mode DACx_OUT1 12-bit right-aligned data */

/* DACC_L12DH */
#define DACC_OUT0_DH_L12                  BITS(4,15)                           /*!< DAC concurrent mode DACx_OUT0 12-bit left-aligned data */
#define DACC_OUT1_DH_L12                  BITS(20,31)                          /*!< DAC concurrent mode DACx_OUT1 12-bit left-aligned data */

/* DACC_R8DH */
#define DACC_OUT0_DH_R8                   BITS(0,7)                            /*!< DAC concurrent mode DACx_OUT0 8-bit right-aligned data */
#define DACC_OUT1_DH_R8                   BITS(8,15)                           /*!< DAC concurrent mode DACx_OUT1 8-bit right-aligned data */

/* DAC0_OUT0_DO */
#define DAC_OUT0_DO_BITS                  BITS(0,11)                           /*!< DACx_OUT0 12-bit output data */

/* DAC1_OUT1_DO */
#define DAC_OUT1_DO_BITS                  BITS(0,11)                           /*!< DACx_OUT1 12-bit output data */

/* DAC_STAT0 */
#define DAC_STAT0_DDUDR0                  BIT(13)                              /*!< DACx_OUT0 DMA underrun flag */
#define DAC_STAT0_DDUDR1                  BIT(29)                              /*!< DACx_OUT1 DMA underrun flag */

/* constants definitions */
/* DAC trigger source */
#define CTL0_DTSEL(regval)                (BITS(3,5) & ((uint32_t)(regval) << 3))
#define DAC_TRIGGER_T5_TRGO               CTL0_DTSEL(0)                        /*!< TIMER5 TRGO */
#define DAC_TRIGGER_T7_TRGO               CTL0_DTSEL(1)                        /*!< TIMER7 TRGO */
#define DAC_TRIGGER_T6_TRGO               CTL0_DTSEL(2)                        /*!< TIMER6 TRGO */
#define DAC_TRIGGER_T4_TRGO               CTL0_DTSEL(3)                        /*!< TIMER4 TRGO */
#define DAC_TRIGGER_T1_TRGO               CTL0_DTSEL(4)                        /*!< TIMER1 TRGO */
#define DAC_TRIGGER_T3_TRGO               CTL0_DTSEL(5)                        /*!< TIMER3 TRGO */
#define DAC_TRIGGER_EXTI_9                CTL0_DTSEL(6)                        /*!< EXTI interrupt line9 event */
#define DAC_TRIGGER_SOFTWARE              CTL0_DTSEL(7)                        /*!< software trigger */

/* DAC noise wave mode */
#define CTL0_DWM(regval)                  (BITS(6,7) & ((uint32_t)(regval) << 6))
#define DAC_WAVE_DISABLE                  CTL0_DWM(0)                          /*!< wave disabled */
#define DAC_WAVE_MODE_LFSR                CTL0_DWM(1)                          /*!< LFSR noise mode */
#define DAC_WAVE_MODE_TRIANGLE            CTL0_DWM(2)                          /*!< triangle noise mode */

/* DAC noise wave bit width */
#define DWBW(regval)                      (BITS(8,11) & ((uint32_t)(regval) << 8))
#define DAC_WAVE_BIT_WIDTH_1              DWBW(0)                              /*!< bit width of the wave signal is 1 */
#define DAC_WAVE_BIT_WIDTH_2              DWBW(1)                              /*!< bit width of the wave signal is 2 */
#define DAC_WAVE_BIT_WIDTH_3              DWBW(2)                              /*!< bit width of the wave signal is 3 */
#define DAC_WAVE_BIT_WIDTH_4              DWBW(3)                              /*!< bit width of the wave signal is 4 */
#define DAC_WAVE_BIT_WIDTH_5              DWBW(4)                              /*!< bit width of the wave signal is 5 */
#define DAC_WAVE_BIT_WIDTH_6              DWBW(5)                              /*!< bit width of the wave signal is 6 */
#define DAC_WAVE_BIT_WIDTH_7              DWBW(6)                              /*!< bit width of the wave signal is 7 */
#define DAC_WAVE_BIT_WIDTH_8              DWBW(7)                              /*!< bit width of the wave signal is 8 */
#define DAC_WAVE_BIT_WIDTH_9              DWBW(8)                              /*!< bit width of the wave signal is 9 */
#define DAC_WAVE_BIT_WIDTH_10             DWBW(9)                              /*!< bit width of the wave signal is 10 */
#define DAC_WAVE_BIT_WIDTH_11             DWBW(10)                             /*!< bit width of the wave signal is 11 */
#define DAC_WAVE_BIT_WIDTH_12             DWBW(11)                             /*!< bit width of the wave signal is 12 */

/* unmask LFSR bits in DAC LFSR noise mode */
#define DAC_LFSR_BIT0                     DAC_WAVE_BIT_WIDTH_1                 /*!< unmask the LFSR bit0 */
#define DAC_LFSR_BITS1_0                  DAC_WAVE_BIT_WIDTH_2                 /*!< unmask the LFSR bits[1:0] */
#define DAC_LFSR_BITS2_0                  DAC_WAVE_BIT_WIDTH_3                 /*!< unmask the LFSR bits[2:0] */
#define DAC_LFSR_BITS3_0                  DAC_WAVE_BIT_WIDTH_4                 /*!< unmask the LFSR bits[3:0] */
#define DAC_LFSR_BITS4_0                  DAC_WAVE_BIT_WIDTH_5                 /*!< unmask the LFSR bits[4:0] */
#define DAC_LFSR_BITS5_0                  DAC_WAVE_BIT_WIDTH_6                 /*!< unmask the LFSR bits[5:0] */
#define DAC_LFSR_BITS6_0                  DAC_WAVE_BIT_WIDTH_7                 /*!< unmask the LFSR bits[6:0] */
#define DAC_LFSR_BITS7_0                  DAC_WAVE_BIT_WIDTH_8                 /*!< unmask the LFSR bits[7:0] */
#define DAC_LFSR_BITS8_0                  DAC_WAVE_BIT_WIDTH_9                 /*!< unmask the LFSR bits[8:0] */
#define DAC_LFSR_BITS9_0                  DAC_WAVE_BIT_WIDTH_10                /*!< unmask the LFSR bits[9:0] */
#define DAC_LFSR_BITS10_0                 DAC_WAVE_BIT_WIDTH_11                /*!< unmask the LFSR bits[10:0] */
#define DAC_LFSR_BITS11_0                 DAC_WAVE_BIT_WIDTH_12                /*!< unmask the LFSR bits[11:0] */

/* triangle amplitude in DAC triangle noise mode */
#define DAC_TRIANGLE_AMPLITUDE_1          DAC_WAVE_BIT_WIDTH_1                 /*!< triangle amplitude is 1 */
#define DAC_TRIANGLE_AMPLITUDE_3          DAC_WAVE_BIT_WIDTH_2                 /*!< triangle amplitude is 3 */
#define DAC_TRIANGLE_AMPLITUDE_7          DAC_WAVE_BIT_WIDTH_3                 /*!< triangle amplitude is 7 */
#define DAC_TRIANGLE_AMPLITUDE_15         DAC_WAVE_BIT_WIDTH_4                 /*!< triangle amplitude is 15 */
#define DAC_TRIANGLE_AMPLITUDE_31         DAC_WAVE_BIT_WIDTH_5                 /*!< triangle amplitude is 31 */
#define DAC_TRIANGLE_AMPLITUDE_63         DAC_WAVE_BIT_WIDTH_6                 /*!< triangle amplitude is 63 */
#define DAC_TRIANGLE_AMPLITUDE_127        DAC_WAVE_BIT_WIDTH_7                 /*!< triangle amplitude is 127 */
#define DAC_TRIANGLE_AMPLITUDE_255        DAC_WAVE_BIT_WIDTH_8                 /*!< triangle amplitude is 255 */
#define DAC_TRIANGLE_AMPLITUDE_511        DAC_WAVE_BIT_WIDTH_9                 /*!< triangle amplitude is 511 */
#define DAC_TRIANGLE_AMPLITUDE_1023       DAC_WAVE_BIT_WIDTH_10                /*!< triangle amplitude is 1023 */
#define DAC_TRIANGLE_AMPLITUDE_2047       DAC_WAVE_BIT_WIDTH_11                /*!< triangle amplitude is 2047 */
#define DAC_TRIANGLE_AMPLITUDE_4095       DAC_WAVE_BIT_WIDTH_12                /*!< triangle amplitude is 4095 */

/* DAC data alignment */
#define DATA_ALIGN(regval)                (BITS(0,1) & ((uint32_t)(regval) << 0))
#define DAC_ALIGN_12B_R                   DATA_ALIGN(0)                        /*!< 12-bit right-aligned data */
#define DAC_ALIGN_12B_L                   DATA_ALIGN(1)                        /*!< 12-bit left-aligned data */
#define DAC_ALIGN_8B_R                    DATA_ALIGN(2)                        /*!< 8-bit right-aligned data */

/* DAC output channel definitions */
#define DAC_OUT0                          ((uint8_t)0x00U)                     /*!< DACx_OUT0 channel */
#define DAC_OUT1                          ((uint8_t)0x01U)                     /*!< DACx_OUT1 channel */

/* DAC interrupt */
#define DAC_INT_DDUDR0                    DAC_CTL0_DDUDRIE0                    /*!< DACx_OUT0 DMA underrun interrupt */
#define DAC_INT_DDUDR1                    DAC_CTL0_DDUDRIE1                    /*!< DACx_OUT1 DMA underrun interrupt */

/* DAC interrupt flag */
#define DAC_INT_FLAG_DDUDR0               DAC_STAT0_DDUDR0                     /*!< DACx_OUT0 DMA underrun interrupt flag */
#define DAC_INT_FLAG_DDUDR1               DAC_STAT0_DDUDR1                     /*!< DACx_OUT1 DMA underrun interrupt flag */

/* DAC flags */
#define DAC_FLAG_DDUDR0                   DAC_STAT0_DDUDR0                     /*!< DACx_OUT0 DMA underrun flag */
#define DAC_FLAG_DDUDR1                   DAC_STAT0_DDUDR1                     /*!< DACx_OUT1 DMA underrun flag */

/* function declarations */
/* DAC initialization functions */
/* deinitialize DAC */
void dac_deinit(uint32_t dac_periph);
/* enable DAC */
void dac_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC */
void dac_disable(uint32_t dac_periph, uint8_t dac_out);
/* enable DAC DMA function */
void dac_dma_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC DMA function */
void dac_dma_disable(uint32_t dac_periph, uint8_t dac_out);

/* DAC buffer functions */
/* enable DAC output buffer */
void dac_output_buffer_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC output buffer */
void dac_output_buffer_disable(uint32_t dac_periph, uint8_t dac_out);

/* read and write operation functions */
/* get DAC output value */
uint16_t dac_output_value_get(uint32_t dac_periph, uint8_t dac_out);
/* set DAC data holding register value */
void dac_data_set(uint32_t dac_periph, uint8_t dac_out, uint32_t dac_align, uint16_t data);

/* DAC trigger configuration */
/* enable DAC trigger */
void dac_trigger_enable(uint32_t dac_periph, uint8_t dac_out);
/* disable DAC trigger */
void dac_trigger_disable(uint32_t dac_periph, uint8_t dac_out);
/* configure DAC trigger source */
void dac_trigger_source_config(uint32_t dac_periph, uint8_t dac_out, uint32_t triggersource);
/* enable DAC software trigger */
void dac_software_trigger_enable(uint32_t dac_periph, uint8_t dac_out);

/* DAC wave mode configuration */
/* configure DAC wave mode */
void dac_wave_mode_config(uint32_t dac_periph, uint8_t dac_out, uint32_t wave_mode);
/* configure DAC LFSR noise mode */
void dac_lfsr_noise_config(uint32_t dac_periph, uint8_t dac_out, uint32_t unmask_bits);
/* configure DAC triangle noise mode */
void dac_triangle_noise_config(uint32_t dac_periph, uint8_t dac_out, uint32_t amplitude);

/* DAC concurrent mode configuration */
/* enable DAC concurrent mode */
void dac_concurrent_enable(uint32_t dac_periph);
/* disable DAC concurrent mode */
void dac_concurrent_disable(uint32_t dac_periph);
/* enable DAC concurrent software trigger */
void dac_concurrent_software_trigger_enable(uint32_t dac_periph);
/* enable DAC concurrent buffer function */
void dac_concurrent_output_buffer_enable(uint32_t dac_periph);
/* disable DAC concurrent buffer function */
void dac_concurrent_output_buffer_disable(uint32_t dac_periph);
/* set DAC concurrent mode data holding register value */
void dac_concurrent_data_set(uint32_t dac_periph, uint32_t dac_align, uint16_t data0, uint16_t data1);

/* DAC interrupt and flag functions */
/* get DAC flag */
FlagStatus dac_flag_get(uint32_t dac_periph, uint32_t flag);
/* clear DAC flag */
void dac_flag_clear(uint32_t dac_periph, uint32_t flag);
/* enable DAC interrupt */
void dac_interrupt_enable(uint32_t dac_periph, uint32_t interrupt);
/* disable DAC interrupt */
void dac_interrupt_disable(uint32_t dac_periph, uint32_t interrupt);
/* get DAC interrupt flag */
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph, uint32_t int_flag);
/* clear DAC interrupt flag */
void dac_interrupt_flag_clear(uint32_t dac_periph, uint32_t int_flag);

#endif /* GD32F4XX_DAC_H */
