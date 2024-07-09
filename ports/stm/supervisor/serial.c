// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include <string.h>
#include "supervisor/shared/serial.h"
#if CPY_STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4/gpio.h"
// TODO: Switch this to using DEBUG_UART.

UART_HandleTypeDef huart2;
#endif

void port_serial_init(void) {
    #if CPY_STM32F4
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) == HAL_OK) {
        stm32f4_peripherals_status_led(1, 1);
    }
    #endif
}

bool port_serial_connected(void) {
    return true;
}

char port_serial_read(void) {
    #if CPY_STM32F4
    uint8_t data;
    HAL_UART_Receive(&huart2, &data, 1, 500);
    return data;
    #else
    return -1;
    #endif
}

// There is no easy way to find the number of pending characters, so just say there's 1.
uint32_t port_serial_bytes_available(void) {
    #if CPY_STM32F4
    return __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) ? 1 : 0;
    #else
    return 0;
    #endif
}

void port_serial_write_substring(const char *text, uint32_t len) {
    #if CPY_STM32F4
    HAL_UART_Transmit(&huart2, (uint8_t *)text, len, 5000);
    #endif
}
