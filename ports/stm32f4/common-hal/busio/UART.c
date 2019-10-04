/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

#include "common-hal/microcontroller/Pin.h"
#include "stm32f4xx_hal.h"

#include "tick.h"

STATIC bool reserved_uart[10];

void uart_reset(void) {
    //ugh. reduce this
    #ifdef USART1
        reserved_uart[0] = false;
        __HAL_RCC_USART1_CLK_DISABLE(); 
    #endif
    #ifdef USART2
        reserved_uart[1] = false;
        __HAL_RCC_USART2_CLK_DISABLE(); 
    #endif
    #ifdef USART3
        reserved_uart[2] = false;
        __HAL_RCC_USART3_CLK_DISABLE(); 
    #endif
    #ifdef UART4
        reserved_uart[3] = false;
        __HAL_RCC_UART4_CLK_DISABLE();
    #endif
    #ifdef UART5
        reserved_uart[4] = false;
        __HAL_RCC_UART5_CLK_DISABLE();
    #endif
    #ifdef USART6
        reserved_uart[5] = false;
        __HAL_RCC_USART6_CLK_DISABLE(); 
    #endif
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
        uint8_t bits, uart_parity_t parity, uint8_t stop, mp_float_t timeout,
        uint16_t receiver_buffer_size) {

    //match pins to UART objects
    USART_TypeDef * USARTx;

    uint8_t tx_len = sizeof(mcu_uart_tx_list)/sizeof(*mcu_uart_tx_list);
    uint8_t rx_len = sizeof(mcu_uart_rx_list)/sizeof(*mcu_uart_rx_list);

    bool uart_taken = false;
    //tx
    for(uint i=0; i<tx_len;i++) {
        if (mcu_uart_tx_list[i].pin == tx) {
            //rx
            for(uint j=0; j<rx_len;j++) {
                if (mcu_uart_rx_list[j].pin == rx 
                    && mcu_uart_rx_list[j].uart_index == mcu_uart_tx_list[i].uart_index) {
                    //keep looking if the UART is taken, edge case
                    if(reserved_uart[mcu_uart_tx_list[i].uart_index-1]) {
                        uart_taken = true;
                        continue;
                    }
                    //store pins if not
                    self->tx = &mcu_uart_tx_list[i];
                    self->rx = &mcu_uart_rx_list[j];
                    break;
                }
            }
        }
    }

    //handle typedef selection, errors
    if(self->tx!=NULL && self->rx!=NULL) {
        USARTx = mcu_uart_banks[self->tx->uart_index-1];
    } else {
        if (uart_taken) {
            mp_raise_ValueError(translate("Hardware busy, try alternative pins"));
        } else {
            mp_raise_ValueError(translate("Invalid UART pin selection"));
        }
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(tx->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->tx->altfn_index; 
    HAL_GPIO_Init(pin_port(tx->port), &GPIO_InitStruct);

    GPIO_InitStruct.Pin = pin_mask(rx->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->rx->altfn_index; 
    HAL_GPIO_Init(pin_port(rx->port), &GPIO_InitStruct);

    #ifdef USART1
    if(USARTx==USART1) { 
        reserved_uart[0] = true;
        __HAL_RCC_USART1_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART1");
    } 
    #endif
    #ifdef UART2
    if(USARTx==USART2) { 
        reserved_uart[1] = true;
        __HAL_RCC_USART2_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART2");
    } 
    #endif
    #ifdef USART3
    if(USARTx==USART3) { 
        reserved_uart[2] = true;
        __HAL_RCC_USART3_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART3");
    } 
    #endif
    #ifdef UART4
    if(USARTx==UART4) { 
        reserved_uart[3] = true;
        __HAL_RCC_UART4_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART4");
    } 
    #endif
    #ifdef UART5
    if(USARTx==UART5) { 
        reserved_uart[4] = true;
        __HAL_RCC_UART5_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART5");
    } 
    #endif
    #ifdef USART6
    if(USARTx==USART6) { 
        reserved_uart[5] = true;
        __HAL_RCC_USART6_CLK_ENABLE();
        mp_printf(&mp_plat_print, "USART6");
    } 
    #endif

    self->handle.Instance = USARTx;
    self->handle.Init.BaudRate = 115200;
    self->handle.Init.WordLength = UART_WORDLENGTH_8B;
    self->handle.Init.StopBits = UART_STOPBITS_1;
    self->handle.Init.Parity = UART_PARITY_NONE;
    self->handle.Init.Mode = UART_MODE_TX_RX;
    self->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    self->handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&self->handle) != HAL_OK)
    {
        mp_raise_ValueError(translate("UART Init Error"));
    }
    claim_pin(tx);
    claim_pin(rx);
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (HAL_UART_Receive(&self->handle, data, (uint16_t)len, 500) == HAL_OK) {
        return len;
    } else {
        mp_raise_ValueError(translate("UART read error"));
    }
    return 0;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    //const char aTxBuffer[] = "This is the internal message";

    if (HAL_UART_Transmit(&self->handle, (uint8_t *)data, len, 500) == HAL_OK) {
        return len;
    } else {
        mp_raise_ValueError(translate("UART write error"));
    }
    return 0;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return 0;
}
