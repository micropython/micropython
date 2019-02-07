/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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

// This file contains pin definitions that are specific to the sc5xx port.
// This file should only ever be #included by pin.h and not directly.

enum {
  PORT_A,
  PORT_B,
  PORT_C,
  PORT_D,
  PORT_E,
  PORT_F,
  PORT_G,
};

#define AF_FN_SPI 0
#define AF_FN_MSI 0
#define AF_FN_UART 0
#define AF_FN_CAN 0

// AF4 is input tap, should be considered as AF0
#define GPIO_AF0_SPI1 0
#define GPIO_AF0_SPI2 0
#define GPIO_AF4_SPI1 0
#define GPIO_AF4_SPI2 0

#define SPI0 (void *)pREG_SPI0_CTL
#define SPI1 (void *)pREG_SPI1_CTL
#define SPI2 (void *)pREG_SPI2_CTL

enum {
  AF_PIN_TYPE_UART_TX = 0,
  AF_PIN_TYPE_UART_RX,
  AF_PIN_TYPE_UART_CTS,
  AF_PIN_TYPE_UART_RTS,

  AF_PIN_TYPE_SPI_MOSI = 0,
  AF_PIN_TYPE_SPI_MISO,
  AF_PIN_TYPE_SPI_SCK,
  AF_PIN_TYPE_SPI_SS,

  AF_PIN_TYPE_MSI_CLK = 0,
  AF_PIN_TYPE_MSI_CMD,
  AF_PIN_TYPE_MSI_D0,
  AF_PIN_TYPE_MSI_D1,
  AF_PIN_TYPE_MSI_D2,
  AF_PIN_TYPE_MSI_D3,

  AF_PIN_TYPE_CAN_TX = 0,
  AF_PIN_TYPE_CAN_RX,
};
