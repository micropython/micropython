/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/mphal.h"
#include "py/ringbuf.h"
#include "supervisor/port.h"
#include "supervisor/serial.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "em_cmu.h"

#define CONSOLE_RCV_BUFFER_SIZE 4096

#define EUSART_VCOM_TX_PORT gpioPortA
#define EUSART_VCOM_TX_PIN 5

#define EUSART_VCOM_RX_PORT gpioPortA
#define EUSART_VCOM_RX_PIN 6

STATIC ringbuf_t con_uart_rx_ringbuf;
STATIC byte con_uart_rx_buf[CONSOLE_RCV_BUFFER_SIZE];
STATIC volatile uint8_t received_data;

// USART0 RX interrupt handler , put characters to ring buffer one by one
void EUSART0_RX_IRQHandler(void) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    received_data = EUSART0->RXDATA;
    if (1 != ringbuf_put_n(&con_uart_rx_ringbuf, (uint8_t *)&received_data, 1)) {
        mp_raise_OverflowError_varg(MP_ERROR_TEXT("Console UART RX buffer overflow"));
    }

    CORE_EXIT_ATOMIC();
    port_wake_main_task();

    if (received_data == CHAR_CTRL_C &&
        mp_interrupt_char == CHAR_CTRL_C) {
        ringbuf_clear(&con_uart_rx_ringbuf);
        mp_sched_keyboard_interrupt();
    }
    EUSART_IntClear(EUSART0, EUSART_IF_RXFL);
}

// Configure EUSART0 for REPL
void port_serial_early_init(void) {

    // Enable clock to GPIO and EUSART1
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_EUSART0, true);

    // Configure the EUSART TX pin to the board controller as an output
    GPIO_PinModeSet(EUSART_VCOM_TX_PORT, EUSART_VCOM_TX_PIN, gpioModePushPull, 0);

    // Configure the EUSART RX pin to the board controller as an input
    GPIO_PinModeSet(EUSART_VCOM_RX_PORT, EUSART_VCOM_RX_PIN, gpioModeInput, 0);

    // Route EUSART0 TX and RX to the board controller TX and RX pins
    GPIO->EUSARTROUTE[0].TXROUTE = (EUSART_VCOM_TX_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
        | (EUSART_VCOM_TX_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[0].RXROUTE = (EUSART_VCOM_RX_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
        | (EUSART_VCOM_RX_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

    // Enable RX and TX signals now that they have been routed
    GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN;

    // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
    EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;

    // Configure and enable EUSART0 for high-frequency (EM0/1) operation
    EUSART_UartInitHf(EUSART0, &init);


    // Claim and never reset UART console pin
    common_hal_mcu_pin_claim(&pin_PA5);
    common_hal_mcu_pin_claim(&pin_PA6);
    common_hal_never_reset_pin(&pin_PA5);
    common_hal_never_reset_pin(&pin_PA6);
}

// Enable EUSART0 interrupt, init ring buffer
void port_serial_init(void) {
    ringbuf_init(&con_uart_rx_ringbuf,
        con_uart_rx_buf,
        CONSOLE_RCV_BUFFER_SIZE);

    received_data = 0;
    // Enable NVIC USART sources
    NVIC_ClearPendingIRQ(EUSART0_RX_IRQn);
    NVIC_EnableIRQ(EUSART0_RX_IRQn);
    NVIC_SetPriority(EUSART0_RX_IRQn, 3);
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);
}

bool port_serial_connected(void) {
    return true;
}

// Get a characters from ring buffer
char port_serial_read(void) {
    int data;

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    data = ringbuf_get(&con_uart_rx_ringbuf);

    CORE_EXIT_ATOMIC();

    return (char)data;
}

// Checking ring buffer haves bytes available or not
bool port_serial_bytes_available(void) {
    return ringbuf_num_filled(&con_uart_rx_ringbuf) > 0 ? true : false;
}

// Send n bytes data to serial by EUSART0
void port_serial_write_substring(const char *text, uint32_t len) {
    char *p_text = (char *)text;
    while (len--) {
        EUSART_Tx(EUSART0, *p_text);
        p_text++;
    }
}
