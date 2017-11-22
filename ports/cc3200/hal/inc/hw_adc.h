//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __HW_ADC_H__
#define __HW_ADC_H__

//*****************************************************************************
//
// The following are defines for the ADC register offsets.
//
//*****************************************************************************
#define ADC_O_ADC_CTRL          0x00000000  // ADC control register.
#define ADC_O_adc_ch0_gain      0x00000004  // Channel 0 gain setting
#define ADC_O_adc_ch1_gain      0x00000008  // Channel 1 gain setting
#define ADC_O_adc_ch2_gain      0x0000000C  // Channel 2 gain setting
#define ADC_O_adc_ch3_gain      0x00000010  // Channel 3 gain setting
#define ADC_O_adc_ch4_gain      0x00000014  // Channel 4 gain setting
#define ADC_O_adc_ch5_gain      0x00000018  // Channel 5 gain setting
#define ADC_O_adc_ch6_gain      0x0000001C  // Channel 6 gain setting
#define ADC_O_adc_ch7_gain      0x00000020  // Channel 7 gain setting
#define ADC_O_adc_ch0_irq_en    0x00000024  // Channel 0 interrupt enable
                                            // register
#define ADC_O_adc_ch1_irq_en    0x00000028  // Channel 1 interrupt enable
                                            // register
#define ADC_O_adc_ch2_irq_en    0x0000002C  // Channel 2 interrupt enable
                                            // register
#define ADC_O_adc_ch3_irq_en    0x00000030  // Channel 3 interrupt enable
                                            // register
#define ADC_O_adc_ch4_irq_en    0x00000034  // Channel 4 interrupt enable
                                            // register
#define ADC_O_adc_ch5_irq_en    0x00000038  // Channel 5 interrupt enable
                                            // register
#define ADC_O_adc_ch6_irq_en    0x0000003C  // Channel 6 interrupt enable
                                            // register
#define ADC_O_adc_ch7_irq_en    0x00000040  // Channel 7 interrupt enable
                                            // register
#define ADC_O_adc_ch0_irq_status \
                                0x00000044  // Channel 0 interrupt status
                                            // register

#define ADC_O_adc_ch1_irq_status \
                                0x00000048  // Channel 1 interrupt status
                                            // register

#define ADC_O_adc_ch2_irq_status \
                                0x0000004C

#define ADC_O_adc_ch3_irq_status \
                                0x00000050  // Channel 3 interrupt status
                                            // register

#define ADC_O_adc_ch4_irq_status \
                                0x00000054  // Channel 4 interrupt status
                                            // register

#define ADC_O_adc_ch5_irq_status \
                                0x00000058

#define ADC_O_adc_ch6_irq_status \
                                0x0000005C  // Channel 6 interrupt status
                                            // register

#define ADC_O_adc_ch7_irq_status \
                                0x00000060  // Channel 7 interrupt status
                                            // register

#define ADC_O_adc_dma_mode_en   0x00000064  // DMA mode enable register
#define ADC_O_adc_timer_configuration \
                                0x00000068  // ADC timer configuration register

#define ADC_O_adc_timer_current_count \
                                0x00000070  // ADC timer current count register

#define ADC_O_channel0FIFODATA  0x00000074  // CH0 FIFO DATA register
#define ADC_O_channel1FIFODATA  0x00000078  // CH1 FIFO DATA register
#define ADC_O_channel2FIFODATA  0x0000007C  // CH2 FIFO DATA register
#define ADC_O_channel3FIFODATA  0x00000080  // CH3 FIFO DATA register
#define ADC_O_channel4FIFODATA  0x00000084  // CH4 FIFO DATA register
#define ADC_O_channel5FIFODATA  0x00000088  // CH5 FIFO DATA register
#define ADC_O_channel6FIFODATA  0x0000008C  // CH6 FIFO DATA register
#define ADC_O_channel7FIFODATA  0x00000090  // CH7 FIFO DATA register
#define ADC_O_adc_ch0_fifo_lvl  0x00000094  // channel 0 FIFO Level register
#define ADC_O_adc_ch1_fifo_lvl  0x00000098  // Channel 1 interrupt status
                                            // register
#define ADC_O_adc_ch2_fifo_lvl  0x0000009C
#define ADC_O_adc_ch3_fifo_lvl  0x000000A0  // Channel 3 interrupt status
                                            // register
#define ADC_O_adc_ch4_fifo_lvl  0x000000A4  // Channel 4 interrupt status
                                            // register
#define ADC_O_adc_ch5_fifo_lvl  0x000000A8
#define ADC_O_adc_ch6_fifo_lvl  0x000000AC  // Channel 6 interrupt status
                                            // register
#define ADC_O_adc_ch7_fifo_lvl  0x000000B0  // Channel 7 interrupt status
                                            // register

#define ADC_O_ADC_CH_ENABLE     0x000000B8

//******************************************************************************
//
// The following are defines for the bit fields in the ADC_O_ADC_CTRL register.
//
//******************************************************************************
#define ADC_ADC_CTRL_adc_cap_scale \
                                0x00000020  // ADC CAP SCALE.

#define ADC_ADC_CTRL_adc_buf_bypass \
                                0x00000010  // ADC ANA CIO buffer bypass.
                                            // Signal is modelled in ANA TOP.
                                            // When '1': ADC buffer is bypassed.

#define ADC_ADC_CTRL_adc_buf_en 0x00000008  // ADC ANA buffer enable. When 1:
                                            // ADC buffer is enabled.
#define ADC_ADC_CTRL_adc_core_en \
                                0x00000004  // ANA ADC core en. This signal act
                                            // as glbal enable to ADC CIO. When
                                            // 1: ADC core is enabled.

#define ADC_ADC_CTRL_adc_soft_reset \
                                0x00000002  // ADC soft reset. When '1' : reset
                                            // ADC internal logic.

#define ADC_ADC_CTRL_adc_en     0x00000001  // ADC global enable. When set ADC
                                            // module is enabled
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch0_gain register.
//
//******************************************************************************
#define ADC_adc_ch0_gain_adc_channel0_gain_M \
                                0x00000003  // gain setting for ADC channel 0.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch0_gain_adc_channel0_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch1_gain register.
//
//******************************************************************************
#define ADC_adc_ch1_gain_adc_channel1_gain_M \
                                0x00000003  // gain setting for ADC channel 1.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch1_gain_adc_channel1_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch2_gain register.
//
//******************************************************************************
#define ADC_adc_ch2_gain_adc_channel2_gain_M \
                                0x00000003  // gain setting for ADC channel 2.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch2_gain_adc_channel2_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch3_gain register.
//
//******************************************************************************
#define ADC_adc_ch3_gain_adc_channel3_gain_M \
                                0x00000003  // gain setting for ADC channel 3.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch3_gain_adc_channel3_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch4_gain register.
//
//******************************************************************************
#define ADC_adc_ch4_gain_adc_channel4_gain_M \
                                0x00000003  // gain setting for ADC channel 4
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch4_gain_adc_channel4_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch5_gain register.
//
//******************************************************************************
#define ADC_adc_ch5_gain_adc_channel5_gain_M \
                                0x00000003  // gain setting for ADC channel 5.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch5_gain_adc_channel5_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch6_gain register.
//
//******************************************************************************
#define ADC_adc_ch6_gain_adc_channel6_gain_M \
                                0x00000003  // gain setting for ADC channel 6
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch6_gain_adc_channel6_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch7_gain register.
//
//******************************************************************************
#define ADC_adc_ch7_gain_adc_channel7_gain_M \
                                0x00000003  // gain setting for ADC channel 7.
                                            // when "00": 1x when "01: 2x when
                                            // "10":3x when "11" 4x

#define ADC_adc_ch7_gain_adc_channel7_gain_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch0_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch0_irq_en_adc_channel0_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch0_irq_en_adc_channel0_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch1_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch1_irq_en_adc_channel1_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch1_irq_en_adc_channel1_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch2_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch2_irq_en_adc_channel2_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch2_irq_en_adc_channel2_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch3_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch3_irq_en_adc_channel3_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch3_irq_en_adc_channel3_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch4_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch4_irq_en_adc_channel4_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch4_irq_en_adc_channel4_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch5_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch5_irq_en_adc_channel5_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch5_irq_en_adc_channel5_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch6_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch6_irq_en_adc_channel6_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch6_irq_en_adc_channel6_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch7_irq_en register.
//
//******************************************************************************
#define ADC_adc_ch7_irq_en_adc_channel7_irq_en_M \
                                0x0000000F  // interrupt enable register for
                                            // per ADC channel bit 3: when '1'
                                            // -> enable FIFO overflow interrupt
                                            // bit 2: when '1' -> enable FIFO
                                            // underflow interrupt bit 1: when
                                            // "1' -> enable FIFO empty
                                            // interrupt bit 0: when "1" ->
                                            // enable FIFO full interrupt

#define ADC_adc_ch7_irq_en_adc_channel7_irq_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch0_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch0_irq_status_adc_channel0_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch0_irq_status_adc_channel0_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch1_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch1_irq_status_adc_channel1_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch1_irq_status_adc_channel1_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch2_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch2_irq_status_adc_channel2_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch2_irq_status_adc_channel2_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch3_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch3_irq_status_adc_channel3_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch3_irq_status_adc_channel3_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch4_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch4_irq_status_adc_channel4_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch4_irq_status_adc_channel4_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch5_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch5_irq_status_adc_channel5_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch5_irq_status_adc_channel5_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch6_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch6_irq_status_adc_channel6_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch6_irq_status_adc_channel6_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch7_irq_status register.
//
//******************************************************************************
#define ADC_adc_ch7_irq_status_adc_channel7_irq_status_M \
                                0x0000000F  // interrupt status register for
                                            // per ADC channel. Interrupt status
                                            // can be cleared on write. bit 3:
                                            // when value '1' is written ->
                                            // would clear FIFO overflow
                                            // interrupt status in the next
                                            // cycle. if same interrupt is set
                                            // in the same cycle then interurpt
                                            // would be set and clear command
                                            // will be ignored. bit 2: when
                                            // value '1' is written -> would
                                            // clear FIFO underflow interrupt
                                            // status in the next cycle. bit 1:
                                            // when value '1' is written ->
                                            // would clear FIFO empty interrupt
                                            // status in the next cycle. bit 0:
                                            // when value '1' is written ->
                                            // would clear FIFO full interrupt
                                            // status in the next cycle.

#define ADC_adc_ch7_irq_status_adc_channel7_irq_status_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_dma_mode_en register.
//
//******************************************************************************
#define ADC_adc_dma_mode_en_DMA_MODEenable_M \
                                0x000000FF  // this register enable DMA mode.
                                            // when '1' respective ADC channel
                                            // is enabled for DMA. When '0' only
                                            // interrupt mode is enabled. Bit 0:
                                            // channel 0 DMA mode enable. Bit 1:
                                            // channel 1 DMA mode enable. Bit 2:
                                            // channel 2 DMA mode enable. Bit 3:
                                            // channel 3 DMA mode enable. bit 4:
                                            // channel 4 DMA mode enable. bit 5:
                                            // channel 5 DMA mode enable. bit 6:
                                            // channel 6 DMA mode enable. bit 7:
                                            // channel 7 DMA mode enable.

#define ADC_adc_dma_mode_en_DMA_MODEenable_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_timer_configuration register.
//
//******************************************************************************
#define ADC_adc_timer_configuration_timeren \
                                0x02000000  // when '1' timer is enabled.

#define ADC_adc_timer_configuration_timerreset \
                                0x01000000  // when '1' reset timer.

#define ADC_adc_timer_configuration_timercount_M \
                                0x00FFFFFF  // Timer count configuration. 17
                                            // bit counter is supported. Other
                                            // MSB's are redundent.

#define ADC_adc_timer_configuration_timercount_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_timer_current_count register.
//
//******************************************************************************
#define ADC_adc_timer_current_count_timercurrentcount_M \
                                0x0001FFFF  // Timer count configuration

#define ADC_adc_timer_current_count_timercurrentcount_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel0FIFODATA register.
//
//******************************************************************************
#define ADC_channel0FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel0FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel1FIFODATA register.
//
//******************************************************************************
#define ADC_channel1FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel1FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel2FIFODATA register.
//
//******************************************************************************
#define ADC_channel2FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel2FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel3FIFODATA register.
//
//******************************************************************************
#define ADC_channel3FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel3FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel4FIFODATA register.
//
//******************************************************************************
#define ADC_channel4FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel4FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel5FIFODATA register.
//
//******************************************************************************
#define ADC_channel5FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel5FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel6FIFODATA register.
//
//******************************************************************************
#define ADC_channel6FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel6FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_channel7FIFODATA register.
//
//******************************************************************************
#define ADC_channel7FIFODATA_FIFO_RD_DATA_M \
                                0xFFFFFFFF  // read to this register would
                                            // return ADC data along with time
                                            // stamp information in following
                                            // format: bits [13:0] : ADC sample
                                            // bits [31:14]: : time stamp per
                                            // ADC sample

#define ADC_channel7FIFODATA_FIFO_RD_DATA_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch0_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch0_fifo_lvl_adc_channel0_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch0_fifo_lvl_adc_channel0_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch1_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch1_fifo_lvl_adc_channel1_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch1_fifo_lvl_adc_channel1_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch2_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch2_fifo_lvl_adc_channel2_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch2_fifo_lvl_adc_channel2_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch3_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch3_fifo_lvl_adc_channel3_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch3_fifo_lvl_adc_channel3_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch4_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch4_fifo_lvl_adc_channel4_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch4_fifo_lvl_adc_channel4_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch5_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch5_fifo_lvl_adc_channel5_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch5_fifo_lvl_adc_channel5_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch6_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch6_fifo_lvl_adc_channel6_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch6_fifo_lvl_adc_channel6_fifo_lvl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// ADC_O_adc_ch7_fifo_lvl register.
//
//******************************************************************************
#define ADC_adc_ch7_fifo_lvl_adc_channel7_fifo_lvl_M \
                                0x00000007  // This register shows current FIFO
                                            // level. FIFO is 4 word wide.
                                            // Possible supported levels are :
                                            // 0x0 to 0x3

#define ADC_adc_ch7_fifo_lvl_adc_channel7_fifo_lvl_S 0



#endif // __HW_ADC_H__
