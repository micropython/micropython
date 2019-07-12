/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_STM32_DMA_H
#define MICROPY_INCLUDED_STM32_DMA_H

typedef struct _dma_descr_t dma_descr_t;


extern const dma_descr_t dma_I2C_1_RX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_SPI_3_RX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_SPI_3_TX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_SDIO_0;

// TODO: 需要定义 各种外设 的 通道值供 dma_descr_t.sub_instance 使用， 范围[0,0x7f]
// 按 channel 定义所有可使用DMA的外设通道
#define _DMA1_CH1_PREFIX      (0x00000000)
#define _DMA1_CH2_PREFIX      (0x10000000)
#define _DMA1_CH3_PREFIX      (0x20000000)
#define _DMA1_CH4_PREFIX      (0x30000000)
#define _DMA1_CH5_PREFIX      (0x40000000)
#define _DMA1_CH6_PREFIX      (0x50000000)
#define _DMA1_CH7_PREFIX      (0x60000000)

#define _DMA2_CH1_PREFIX      (0x70000000)
#define _DMA2_CH2_PREFIX      (0x80000000)
#define _DMA2_CH3_PREFIX      (0x90000000)
#define _DMA2_CH4_PREFIX      (0xa0000000)
#define _DMA2_CH5_PREFIX      (0xb0000000)

// DMA1 Channel1
#define MP_DMA1_CH1_ADC1      (_DMA1_CH1_PREFIX | 0x00000001)
#define MP_DMA1_CH1_TIM2_CH3  (_DMA1_CH1_PREFIX | 0x00000002)
#define MP_DMA1_CH1_TIM4_CH1  (_DMA1_CH1_PREFIX | 0x00000003)

// DMA1 Channel2
#define MP_DMA1_CH2_SPI1_RX   (_DMA1_CH2_PREFIX | 0x00000001)
#define MP_DMA1_CH2_USART3_TX (_DMA1_CH2_PREFIX | 0x00000002)
#define MP_DMA1_CH2_TIM1_CH1  (_DMA1_CH2_PREFIX | 0x00000003)
#define MP_DMA1_CH2_TIM2_UP   (_DMA1_CH2_PREFIX | 0x00000004)
#define MP_DMA1_CH2_TIM3_CH3  (_DMA1_CH2_PREFIX | 0x00000005)

// DMA1 Channel3
#define MP_DMA1_CH3_SPI1_TX   (_DMA1_CH3_PREFIX | 0x00000001)
#define MP_DMA1_CH3_USART3_RX (_DMA1_CH3_PREFIX | 0x00000002)
#define MP_DMA1_CH3_TIM3_CH4  (_DMA1_CH3_PREFIX | 0x00000003)
#define MP_DMA1_CH3_TIM3_UP   (MP_DMA1_CH3_TIM3_CH4)

// DMA1 Channel4
#define MP_DMA1_CH4_SPI2_RX   (_DMA1_CH4_PREFIX | 0x00000001)
#define MP_DMA1_CH4_I2S2_RX   (MP_DMA1_CH4_SPI2_RX)
#define MP_DMA1_CH4_USART1_TX (_DMA1_CH4_PREFIX | 0x00000002)
#define MP_DMA1_CH4_I2C2_TX   (_DMA1_CH4_PREFIX | 0x00000003)
#define MP_DMA1_CH4_TIM1_CH4  (_DMA1_CH4_PREFIX | 0x00000004)
#define MP_DMA1_CH4_TIM1_TRIG (MP_DMA1_CH4_TIM1_CH4)
#define MP_DMA1_CH4_TIM1_COM  (MP_DMA1_CH4_TIM1_CH4)
#define MP_DMA1_CH4_TIM4_CH2  (_DMA1_CH4_PREFIX | 0x00000005)
// DMA1 Channel5
#define MP_DMA1_CH5_SPI2_TX      (_DMA1_CH5_PREFIX | 0x00000001)
#define MP_DMA1_CH5_I2S2_TX      (MP_DMA1_CH5_SPI2_TX)
#define MP_DMA1_CH5_USART1_RX    (_DMA1_CH5_PREFIX | 0x00000002)
#define MP_DMA1_CH5_I2C2_RX      (_DMA1_CH5_PREFIX | 0x00000003)
#define MP_DMA1_CH5_TIM1_UP      (_DMA1_CH5_PREFIX | 0x00000004)
#define MP_DMA1_CH5_TIM2_CH1     (_DMA1_CH5_PREFIX | 0x00000005)
#define MP_DMA1_CH5_TIM4_CH3     (_DMA1_CH5_PREFIX | 0x00000006)
// DMA1 Channel6
#define MP_DMA1_CH6_USART2_RX    (_DMA1_CH6_PREFIX | 0x00000001)
#define MP_DMA1_CH6_I2C1_TX      (_DMA1_CH6_PREFIX | 0x00000002)
#define MP_DMA1_CH6_TIM1_CH3     (_DMA1_CH6_PREFIX | 0x00000003)
#define MP_DMA1_CH6_TIM3_CH1     (_DMA1_CH6_PREFIX | 0x00000004)
#define MP_DMA1_CH6_TIM3_TRIG    (MP_DMA1_CH6_TIM3_CH1)
// DMA1 Channel7
#define MP_DMA1_CH7_USART2_TX    (_DMA1_CH7_PREFIX | 0x00000001)
#define MP_DMA1_CH7_I2C1_RX      (_DMA1_CH7_PREFIX | 0x00000002)
#define MP_DMA1_CH7_TIM2_CH2     (_DMA1_CH7_PREFIX | 0x00000003)
#define MP_DMA1_CH7_TIM2_CH4     (MP_DMA1_CH7_TIM2_CH2)
#define MP_DMA1_CH7_TIM4_UP      (_DMA1_CH7_PREFIX | 0x00000004)
// DMA2 Channel1
#define MP_DMA2_CH1_SPI3_RX      (_DMA2_CH1_PREFIX | 0x00000001)
#define MP_DMA2_CH1_I2S3_RX      (MP_DMA2_CH1_SPI3_RX)
#define MP_DMA2_CH1_TIM5_CH4     (_DMA2_CH1_PREFIX | 0x00000002)
#define MP_DMA2_CH1_TIM5_TRIG    (MP_DMA2_CH1_TIM5_CH4)
#define MP_DMA2_CH1_TIM8_CH3     (_DMA2_CH1_PREFIX | 0x00000003)
#define MP_DMA2_CH1_TIM8_UP      (MP_DMA2_CH1_TIM8_CH3)
// DMA2 Channel2
#define MP_DMA2_CH2_SPI3_TX      (_DMA2_CH2_PREFIX | 0x00000001)
#define MP_DMA2_CH2_I2S3_TX      (MP_DMA2_CH2_SPI3_TX)
#define MP_DMA2_CH2_TIM5_CH3     (_DMA2_CH2_PREFIX | 0x00000002)
#define MP_DMA2_CH2_TIM5_UP      (MP_DMA2_CH2_TIM5_CH3)
#define MP_DMA2_CH2_TIM8_CH4     (_DMA2_CH2_PREFIX | 0x00000003)
#define MP_DMA2_CH2_TIM8_TRIG    (MP_DMA2_CH2_TIM8_CH4)
#define MP_DMA2_CH2_TIM8_COM     (MP_DMA2_CH2_TIM8_CH4)
// DMA2 Channel3
#define MP_DMA2_CH3_UART4_RX     (_DMA2_CH3_PREFIX | 0x00000001)
#define MP_DMA2_CH3_TIM6_UP      (_DMA2_CH3_PREFIX | 0x00000002)
#define MP_DMA2_CH3_DAC_Channel1 (MP_DMA2_CH3_TIM6_UP)
#define MP_DMA2_CH3_TIM8_CH1     (_DMA2_CH3_PREFIX | 0x00000003)
// DMA2 Channel4
#define MP_DMA2_CH4_SDIO         (_DMA2_CH4_PREFIX | 0x00000001)
#define MP_DMA2_CH4_TIM5_CH2     (_DMA2_CH4_PREFIX | 0x00000002)
#define MP_DMA2_CH4_TIM7_UP      (_DMA2_CH4_PREFIX | 0x00000003)
#define MP_DMA2_CH4_DAC_Channel2 (MP_DMA2_CH4_TIM7_UP)
// DMA2 Channel5
#define MP_DMA2_CH5_ADC3         (_DMA2_CH5_PREFIX | 0x00000001)
#define MP_DMA2_CH5_UART4_TX     (_DMA2_CH5_PREFIX | 0x00000002)
#define MP_DMA2_CH5_TIM5_CH1     (_DMA2_CH5_PREFIX | 0x00000003)
#define MP_DMA2_CH5_TIM8_CH2     (_DMA2_CH5_PREFIX | 0x00000004)



void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data);
void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data);
void dma_deinit(const dma_descr_t *dma_descr);
void dma_invalidate_channel(const dma_descr_t *dma_descr);

void dma_nohal_init(const dma_descr_t *descr, uint32_t config);
void dma_nohal_deinit(const dma_descr_t *descr);
void dma_nohal_start(const dma_descr_t *descr, uint32_t src_addr, uint32_t dst_addr, uint16_t len);

#endif // MICROPY_INCLUDED_STM32_DMA_H
