/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// This file contains pin definitions that are specific to the stm32 port.
// This file should only ever be #included by pin.h and not directly.

enum {
  PORT_A,
  PORT_B,
  PORT_C,
  PORT_D,
  PORT_E,
  PORT_F,
  PORT_G,
  PORT_H,
  PORT_I,
  PORT_J,
  PORT_K,
};

enum {
  AF_FN_TIM,
  AF_FN_I2C,
  AF_FN_USART,
  AF_FN_UART = AF_FN_USART,
  AF_FN_SPI,
  AF_FN_I2S,
  AF_FN_SDMMC,
};

enum {
  AF_PIN_TYPE_TIM_CH1 = 0,
  AF_PIN_TYPE_TIM_CH2,
  AF_PIN_TYPE_TIM_CH3,
  AF_PIN_TYPE_TIM_CH4,
  AF_PIN_TYPE_TIM_CH1N,
  AF_PIN_TYPE_TIM_CH2N,
  AF_PIN_TYPE_TIM_CH3N,
  AF_PIN_TYPE_TIM_CH1_ETR,
  AF_PIN_TYPE_TIM_ETR,
  AF_PIN_TYPE_TIM_BKIN,

  AF_PIN_TYPE_I2C_SDA = 0,
  AF_PIN_TYPE_I2C_SCL,

  AF_PIN_TYPE_USART_TX = 0,
  AF_PIN_TYPE_USART_RX,
  AF_PIN_TYPE_USART_CTS,
  AF_PIN_TYPE_USART_RTS,
  AF_PIN_TYPE_USART_CK,
  AF_PIN_TYPE_UART_TX  = AF_PIN_TYPE_USART_TX,
  AF_PIN_TYPE_UART_RX  = AF_PIN_TYPE_USART_RX,
  AF_PIN_TYPE_UART_CTS = AF_PIN_TYPE_USART_CTS,
  AF_PIN_TYPE_UART_RTS = AF_PIN_TYPE_USART_RTS,

  AF_PIN_TYPE_SPI_MOSI = 0,
  AF_PIN_TYPE_SPI_MISO,
  AF_PIN_TYPE_SPI_SCK,
  AF_PIN_TYPE_SPI_NSS,

  AF_PIN_TYPE_I2S_CK = 0,
  AF_PIN_TYPE_I2S_MCK,
  AF_PIN_TYPE_I2S_SD,
  AF_PIN_TYPE_I2S_WS,
  AF_PIN_TYPE_I2S_EXTSD,

  AF_PIN_TYPE_SDMMC_CK = 0,
  AF_PIN_TYPE_SDMMC_CMD,
  AF_PIN_TYPE_SDMMC_D0,
  AF_PIN_TYPE_SDMMC_D1,
  AF_PIN_TYPE_SDMMC_D2,
  AF_PIN_TYPE_SDMMC_D3,
};

// The HAL uses a slightly different naming than we chose, so we provide
// some #defines to massage things. Also I2S and SPI share the same
// peripheral.

#define GPIO_AF5_I2S2   GPIO_AF5_SPI2
#define GPIO_AF5_I2S3   GPIO_AF5_I2S3ext
#define GPIO_AF6_I2S2   GPIO_AF6_I2S2ext
#define GPIO_AF6_I2S3   GPIO_AF6_SPI3
#define GPIO_AF7_I2S2   GPIO_AF7_SPI2
#define GPIO_AF7_I2S3   GPIO_AF7_I2S3ext

#define I2S2  SPI2
#define I2S3  SPI3

enum {
  PIN_ADC1  = (1 << 0),
  PIN_ADC2  = (1 << 1),
  PIN_ADC3  = (1 << 2),
};

typedef GPIO_TypeDef pin_gpio_t;

