/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

#define ALL_UARTS 0xFFFF

// arrays use 0 based numbering: UART1 is stored at index 0
STATIC bool reserved_uart[MAX_UART];
STATIC bool never_reset_uart[MAX_UART];
int errflag; // Used to restart read halts

STATIC void uart_clock_enable(uint16_t mask);
STATIC void uart_clock_disable(uint16_t mask);
STATIC void uart_assign_irq(busio_uart_obj_t *self, USART_TypeDef *USARTx);

STATIC USART_TypeDef *assign_uart_or_throw(busio_uart_obj_t *self, bool pin_eval,
    int periph_index, bool uart_taken) {
    if (pin_eval) {
        // assign a root pointer pointer for IRQ
        MP_STATE_PORT(cpy_uart_obj_all)[periph_index] = self;
        return mcu_uart_banks[periph_index];
    } else {
        if (uart_taken) {
            mp_raise_ValueError(translate("Hardware in use, try alternative pins"));
        } else {
            mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_UART);
        }
    }
}

void uart_reset(void) {
    uint16_t never_reset_mask = 0x00;
    for (uint8_t i = 0; i < MAX_UART; i++) {
        if (!never_reset_uart[i]) {
            reserved_uart[i] = false;
            MP_STATE_PORT(cpy_uart_obj_all)[i] = NULL;
        } else {
            never_reset_mask |= 1 << i;
        }
    }
    uart_clock_disable(ALL_UARTS & ~(never_reset_mask));
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    // match pins to UART objects
    USART_TypeDef *USARTx;

    uint8_t tx_len = MP_ARRAY_SIZE(mcu_uart_tx_list);
    uint8_t rx_len = MP_ARRAY_SIZE(mcu_uart_rx_list);
    bool uart_taken = false;
    uint8_t periph_index = 0; // origin 0 corrected

    if ((rts != NULL) || (cts != NULL) || (rs485_dir != NULL) || (rs485_invert == true)) {
        mp_raise_ValueError(translate("RTS/CTS/RS485 Not yet supported on this device"));
    }

    // Can have both pins, or either
    if ((tx != NULL) && (rx != NULL)) {
        // normal find loop if both pins exist
        for (uint i = 0; i < tx_len; i++) {
            if (mcu_uart_tx_list[i].pin == tx) {
                // rx
                for (uint j = 0; j < rx_len; j++) {
                    if (mcu_uart_rx_list[j].pin == rx
                        && mcu_uart_rx_list[j].periph_index == mcu_uart_tx_list[i].periph_index) {
                        // keep looking if the UART is taken, edge case
                        if (reserved_uart[mcu_uart_tx_list[i].periph_index - 1]) {
                            uart_taken = true;
                            continue;
                        }
                        // store pins if not
                        self->tx = &mcu_uart_tx_list[i];
                        self->rx = &mcu_uart_rx_list[j];
                        break;
                    }
                }
                if (self->tx != NULL) {
                    break;
                }
            }
        }
        periph_index = self->tx->periph_index - 1;
        USARTx = assign_uart_or_throw(self, (self->tx != NULL && self->rx != NULL),
            periph_index, uart_taken);
    } else if (tx == NULL) {
        // If there is no tx, run only rx
        for (uint i = 0; i < rx_len; i++) {
            if (mcu_uart_rx_list[i].pin == rx) {
                // keep looking if the UART is taken, edge case
                if (reserved_uart[mcu_uart_rx_list[i].periph_index - 1]) {
                    uart_taken = true;
                    continue;
                }
                // store pins if not
                self->rx = &mcu_uart_rx_list[i];
                break;
            }
        }
        periph_index = self->rx->periph_index - 1;
        USARTx = assign_uart_or_throw(self, (self->rx != NULL),
            periph_index, uart_taken);
    } else if (rx == NULL) {
        // If there is no rx, run only tx
        for (uint i = 0; i < tx_len; i++) {
            if (mcu_uart_tx_list[i].pin == tx) {
                // keep looking if the UART is taken, edge case
                if (reserved_uart[mcu_uart_tx_list[i].periph_index - 1]) {
                    uart_taken = true;
                    continue;
                }
                // store pins if not
                self->tx = &mcu_uart_tx_list[i];
                break;
            }
        }
        periph_index = self->tx->periph_index - 1;
        USARTx = assign_uart_or_throw(self, (self->tx != NULL),
            periph_index, uart_taken);
    } else {
        // both pins cannot be empty
        mp_raise_ValueError(translate("Supply at least one UART pin"));
    }

    // Other errors
    if (receiver_buffer_size == 0) {
        mp_raise_ValueError(translate("Invalid buffer size"));
    }
    if (bits != 8 && bits != 9) {
        mp_raise_ValueError(translate("Invalid word/bit length"));
    }
    if (USARTx == NULL) {  // this can only be hit if the periph file is wrong
        mp_raise_ValueError(translate("Internal define error"));
    }

    // GPIO Init
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (self->tx != NULL) {
        GPIO_InitStruct.Pin = pin_mask(tx->number);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = self->tx->altfn_index;
        HAL_GPIO_Init(pin_port(tx->port), &GPIO_InitStruct);
    }
    if (self->rx != NULL) {
        GPIO_InitStruct.Pin = pin_mask(rx->number);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = self->rx->altfn_index;
        HAL_GPIO_Init(pin_port(rx->port), &GPIO_InitStruct);
    }

    // reserve uart and enable the peripheral
    reserved_uart[periph_index] = true;
    uart_clock_enable(1 << (periph_index));
    uart_assign_irq(self, USARTx);

    self->handle.Instance = USARTx;
    self->handle.Init.BaudRate = baudrate;
    self->handle.Init.WordLength = (bits == 9) ? UART_WORDLENGTH_9B : UART_WORDLENGTH_8B;
    self->handle.Init.StopBits = (stop > 1) ? UART_STOPBITS_2 : UART_STOPBITS_1;
    self->handle.Init.Parity = (parity == BUSIO_UART_PARITY_ODD) ? UART_PARITY_ODD :
        (parity == BUSIO_UART_PARITY_EVEN) ? UART_PARITY_EVEN :
        UART_PARITY_NONE;
    self->handle.Init.Mode = (self->tx != NULL && self->rx != NULL) ? UART_MODE_TX_RX :
        (self->tx != NULL) ? UART_MODE_TX :
        UART_MODE_RX;
    self->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    self->handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("UART Init Error"));

    }

    // Init buffer for rx and claim pins
    if (self->rx != NULL) {
        if (receiver_buffer != NULL) {
            self->ringbuf = (ringbuf_t) { receiver_buffer, receiver_buffer_size };
        } else {
            if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size, true)) {
                mp_raise_ValueError(translate("UART Buffer allocation error"));
            }
        }
        common_hal_mcu_pin_claim(rx);
    }
    if (self->tx != NULL) {
        common_hal_mcu_pin_claim(tx);
    }
    self->baudrate = baudrate;
    self->timeout_ms = timeout * 1000;
    self->sigint_enabled = sigint_enabled;

    // start the interrupt series
    if ((HAL_UART_GetState(&self->handle) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX) {
        mp_raise_ValueError(translate("Could not start interrupt, RX busy"));
    }

    // start the receive interrupt chain
    HAL_NVIC_DisableIRQ(self->irq); // prevent handle lock contention
    HAL_UART_Receive_IT(&self->handle, &self->rx_char, 1);
    HAL_NVIC_SetPriority(self->irq, UART_IRQPRI, UART_IRQSUB_PRI);
    HAL_NVIC_EnableIRQ(self->irq);

    errflag = HAL_OK;
}

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_uart_banks); i++) {
        if (mcu_uart_banks[i] == self->handle.Instance) {
            never_reset_uart[i] = true;
            never_reset_pin_number(self->tx->pin->port, self->tx->pin->number);
            never_reset_pin_number(self->rx->pin->port, self->rx->pin->number);
            break;
        }
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->tx->pin == NULL && self->rx->pin == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }

    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_uart_banks); i++) {
        if (mcu_uart_banks[i] == self->handle.Instance) {
            reserved_uart[i] = false;
            never_reset_uart[i] = false;
            break;
        }
    }

    if (self->tx) {
        reset_pin_number(self->tx->pin->port,self->tx->pin->number);
        self->tx = NULL;
    }
    if (self->rx) {
        reset_pin_number(self->rx->pin->port,self->rx->pin->number);
        self->rx = NULL;
    }

    ringbuf_free(&self->ringbuf);
}

size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx == NULL) {
        mp_raise_ValueError(translate("No RX pin"));
    }

    uint64_t start_ticks = supervisor_ticks_ms64();

    // Wait for all bytes received or timeout, same as nrf
    while ((ringbuf_num_filled(&self->ringbuf) < len) && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;
        // restart if it failed in the callback
        if (errflag != HAL_OK) {
            errflag = HAL_UART_Receive_IT(&self->handle, &self->rx_char, 1);
        }
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }

    // Halt reception
    HAL_NVIC_DisableIRQ(self->irq);
    // Copy as much received data as available, up to len bytes.
    size_t rx_bytes = ringbuf_get_n(&self->ringbuf, data, len);
    HAL_NVIC_EnableIRQ(self->irq);

    if (rx_bytes == 0) {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }
    return rx_bytes;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx == NULL) {
        mp_raise_ValueError(translate("No TX pin"));
    }
    bool write_err = false; // write error shouldn't disable interrupts

    HAL_NVIC_DisableIRQ(self->irq);
    HAL_StatusTypeDef ret = HAL_UART_Transmit(&self->handle, (uint8_t *)data, len, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        write_err = true;
    }
    HAL_UART_Receive_IT(&self->handle, &self->rx_char, 1);
    HAL_NVIC_EnableIRQ(self->irq);

    if (write_err) {
        mp_raise_ValueError(translate("UART write error"));
    }
    return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *handle) {
    for (int i = 0; i < 7; i++) {
        // get context pointer and cast it as struct pointer
        busio_uart_obj_t *context = (busio_uart_obj_t *)MP_STATE_PORT(cpy_uart_obj_all)[i];
        if (handle == &context->handle) {
            // check if transaction is ongoing
            if ((HAL_UART_GetState(handle) & HAL_UART_STATE_BUSY_RX) == HAL_UART_STATE_BUSY_RX) {
                return;
            }
            ringbuf_put_n(&context->ringbuf, &context->rx_char, 1);
            errflag = HAL_UART_Receive_IT(handle, &context->rx_char, 1);
            if (context->sigint_enabled) {
                if (context->rx_char == CHAR_CTRL_C) {
                    common_hal_busio_uart_clear_rx_buffer(context);
                    mp_sched_keyboard_interrupt();
                }
            }

            return;
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle) {
    if (__HAL_UART_GET_FLAG(UartHandle, UART_FLAG_PE) != RESET) {
        __HAL_UART_CLEAR_PEFLAG(UartHandle);
    } else if (__HAL_UART_GET_FLAG(UartHandle, UART_FLAG_FE) != RESET) {
        __HAL_UART_CLEAR_FEFLAG(UartHandle);
    } else if (__HAL_UART_GET_FLAG(UartHandle, UART_FLAG_NE) != RESET) {
        __HAL_UART_CLEAR_NEFLAG(UartHandle);
    } else if (__HAL_UART_GET_FLAG(UartHandle, UART_FLAG_ORE) != RESET) {
        __HAL_UART_CLEAR_OREFLAG(UartHandle);
    }
    // restart serial read after an error
    for (int i = 0; i < 7; i++) {
        busio_uart_obj_t *context = (busio_uart_obj_t *)MP_STATE_PORT(cpy_uart_obj_all)[i];
        if (UartHandle == &context->handle) {
            HAL_UART_Receive_IT(UartHandle, &context->rx_char, 1);
            return;
        }
    }

}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    // Don't reset if it's the same value
    if (baudrate == self->baudrate) {
        return;
    }

    // Otherwise de-init and set new rate
    if (HAL_UART_DeInit(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("UART De-init error"));
    }
    self->handle.Init.BaudRate = baudrate;
    if (HAL_UART_Init(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("UART Re-init error"));
    }

    self->baudrate = baudrate;
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    // Halt reception
    HAL_NVIC_DisableIRQ(self->irq);
    ringbuf_clear(&self->ringbuf);
    HAL_NVIC_EnableIRQ(self->irq);
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return __HAL_UART_GET_FLAG(&self->handle, UART_FLAG_TXE);
}

STATIC void call_hal_irq(int uart_num) {
    // Create casted context pointer
    busio_uart_obj_t *context = (busio_uart_obj_t *)MP_STATE_PORT(cpy_uart_obj_all)[uart_num - 1];
    if (context != NULL) {
        HAL_NVIC_ClearPendingIRQ(context->irq);
        HAL_UART_IRQHandler(&context->handle);
    }
}

// UART/USART IRQ handlers
void USART1_IRQHandler(void) {
    call_hal_irq(1);
}

void USART2_IRQHandler(void) {
    call_hal_irq(2);
}

void USART3_IRQHandler(void) {
    call_hal_irq(3);
}

void UART4_IRQHandler(void) {
    call_hal_irq(4);
}

void UART5_IRQHandler(void) {
    call_hal_irq(5);
}

void USART6_IRQHandler(void) {
    call_hal_irq(6);
}

STATIC void uart_clock_enable(uint16_t mask) {
    #ifdef USART1
    if (mask & (1 << 0)) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_ENABLE();
    }
    #endif
    #ifdef USART2
    if (mask & (1 << 1)) {
        __HAL_RCC_USART2_FORCE_RESET();
        __HAL_RCC_USART2_RELEASE_RESET();
        __HAL_RCC_USART2_CLK_ENABLE();
    }
    #endif
    #ifdef USART3
    if (mask & (1 << 2)) {
        __HAL_RCC_USART3_FORCE_RESET();
        __HAL_RCC_USART3_RELEASE_RESET();
        __HAL_RCC_USART3_CLK_ENABLE();
    }
    #endif
    #ifdef UART4
    if (mask & (1 << 3)) {
        __HAL_RCC_UART4_FORCE_RESET();
        __HAL_RCC_UART4_RELEASE_RESET();
        __HAL_RCC_UART4_CLK_ENABLE();
    }
    #endif
    #ifdef UART5
    if (mask & (1 << 4)) {
        __HAL_RCC_UART5_FORCE_RESET();
        __HAL_RCC_UART5_RELEASE_RESET();
        __HAL_RCC_UART5_CLK_ENABLE();
    }
    #endif
    #ifdef USART6
    if (mask & (1 << 5)) {
        __HAL_RCC_USART6_FORCE_RESET();
        __HAL_RCC_USART6_RELEASE_RESET();
        __HAL_RCC_USART6_CLK_ENABLE();
    }
    #endif
    #ifdef UART7
    if (mask & (1 << 6)) {
        __HAL_RCC_UART7_FORCE_RESET();
        __HAL_RCC_UART7_RELEASE_RESET();
        __HAL_RCC_UART7_CLK_ENABLE();
    }
    #endif
    #ifdef UART8
    if (mask & (1 << 7)) {
        __HAL_RCC_UART8_FORCE_RESET();
        __HAL_RCC_UART8_RELEASE_RESET();
        __HAL_RCC_UART8_CLK_ENABLE();
    }
    #endif
    #ifdef UART9
    if (mask & (1 << 8)) {
        __HAL_RCC_UART9_FORCE_RESET();
        __HAL_RCC_UART9_RELEASE_RESET();
        __HAL_RCC_UART9_CLK_ENABLE();
    }
    #endif
    #ifdef UART10
    if (mask & (1 << 9)) {
        __HAL_RCC_UART10_FORCE_RESET();
        __HAL_RCC_UART10_RELEASE_RESET();
        __HAL_RCC_UART10_CLK_ENABLE();
    }
    #endif
}

STATIC void uart_clock_disable(uint16_t mask) {
    #ifdef USART1
    if (mask & (1 << 0)) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_DISABLE();
    }
    #endif
    #ifdef USART2
    if (mask & (1 << 1)) {
        __HAL_RCC_USART2_FORCE_RESET();
        __HAL_RCC_USART2_RELEASE_RESET();
        __HAL_RCC_USART2_CLK_DISABLE();
    }
    #endif
    #ifdef USART3
    if (mask & (1 << 2)) {
        __HAL_RCC_USART3_FORCE_RESET();
        __HAL_RCC_USART3_RELEASE_RESET();
        __HAL_RCC_USART3_CLK_DISABLE();
    }
    #endif
    #ifdef UART4
    if (mask & (1 << 3)) {
        __HAL_RCC_UART4_FORCE_RESET();
        __HAL_RCC_UART4_RELEASE_RESET();
        __HAL_RCC_UART4_CLK_DISABLE();
    }
    #endif
    #ifdef UART5
    if (mask & (1 << 4)) {
        __HAL_RCC_UART5_FORCE_RESET();
        __HAL_RCC_UART5_RELEASE_RESET();
        __HAL_RCC_UART5_CLK_DISABLE();
    }
    #endif
    #ifdef USART6
    if (mask & (1 << 5)) {
        __HAL_RCC_USART6_FORCE_RESET();
        __HAL_RCC_USART6_RELEASE_RESET();
        __HAL_RCC_USART6_CLK_DISABLE();
    }
    #endif
    #ifdef UART7
    if (mask & (1 << 6)) {
        __HAL_RCC_UART7_FORCE_RESET();
        __HAL_RCC_UART7_RELEASE_RESET();
        __HAL_RCC_UART7_CLK_DISABLE();
    }
    #endif
    #ifdef UART8
    if (mask & (1 << 7)) {
        __HAL_RCC_UART8_FORCE_RESET();
        __HAL_RCC_UART8_RELEASE_RESET();
        __HAL_RCC_UART8_CLK_DISABLE();
    }
    #endif
    #ifdef UART9
    if (mask & (1 << 8)) {
        __HAL_RCC_UART9_FORCE_RESET();
        __HAL_RCC_UART9_RELEASE_RESET();
        __HAL_RCC_UART9_CLK_DISABLE();
    }
    #endif
    #ifdef UART10
    if (mask & (1 << 9)) {
        __HAL_RCC_UART10_FORCE_RESET();
        __HAL_RCC_UART10_RELEASE_RESET();
        __HAL_RCC_UART10_CLK_DISABLE();
    }
    #endif
}

STATIC void uart_assign_irq(busio_uart_obj_t *self, USART_TypeDef *USARTx) {
    #ifdef USART1
    if (USARTx == USART1) {
        self->irq = USART1_IRQn;
    }
    #endif
    #ifdef USART2
    if (USARTx == USART2) {
        self->irq = USART2_IRQn;
    }
    #endif
    #ifdef USART3
    if (USARTx == USART3) {
        self->irq = USART3_IRQn;
    }
    #endif
    #ifdef UART4
    if (USARTx == UART4) {
        self->irq = UART4_IRQn;
    }
    #endif
    #ifdef UART5
    if (USARTx == UART5) {
        self->irq = UART5_IRQn;
    }
    #endif
    #ifdef USART6
    if (USARTx == USART6) {
        self->irq = USART6_IRQn;
    }
    #endif
    #ifdef UART7
    if (USARTx == UART7) {
        self->irq = UART7_IRQn;
    }
    #endif
    #ifdef UART8
    if (USARTx == UART8) {
        self->irq = UART8_IRQn;
    }
    #endif
    #ifdef UART9
    if (USARTx == UART9) {
        self->irq = UART9_IRQn;
    }
    #endif
    #ifdef UART10
    if (USARTx == UART10) {
        self->irq = UART10_IRQn;
    }
    #endif
}
