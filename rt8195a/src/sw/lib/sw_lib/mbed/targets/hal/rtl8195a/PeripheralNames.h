/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_PERIPHERALNAMES_H
#define MBED_PERIPHERALNAMES_H

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef enum {
    UART_1 = (int)USART1_BASE,
    UART_2 = (int)USART2_BASE,
    UART_3 = (int)USART3_BASE,
    UART_4 = (int)UART4_BASE,
    UART_5 = (int)UART5_BASE,
    UART_6 = (int)USART6_BASE
} UARTName;

typedef enum {
    ADC0_0 = 0,
    ADC0_1,
    ADC0_2,
    ADC0_3,
    ADC0_4,
    ADC0_5,
    ADC0_6,
    ADC0_7,
    ADC0_8,
    ADC0_9,
    ADC0_10,
    ADC0_11,
    ADC0_12,
    ADC0_13,
    ADC0_14,
    ADC0_15
} ADCName;

typedef enum {
    DAC_0 = 0,
    DAC_1
} DACName;

typedef enum {
    SPI_1 = (int)SPI1_BASE,
    SPI_2 = (int)SPI2_BASE,
    SPI_3 = (int)SPI3_BASE,
} SPIName;

typedef enum {
    I2C_1 = (int)I2C1_BASE,
    I2C_2 = (int)I2C2_BASE,
    I2C_3 = (int)I2C3_BASE
} I2CName;

typedef enum {
    PWM_1 = 1,
    PWM_2,
    PWM_3,
    PWM_4,
    PWM_5,
    PWM_6
} PWMName;

typedef enum {
     CAN_1 = (int)CAN1_BASE,
     CAN_2 = (int)CAN2_BASE
} CANName;
#endif

#define STDIO_UART_TX  PA_6
#define STDIO_UART_RX  PA_7
#define STDIO_UART     UART0

#ifdef __cplusplus
}
#endif

#endif
