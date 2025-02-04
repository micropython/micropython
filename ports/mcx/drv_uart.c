/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "board.h"

/* SDK drivers */
#include "fsl_lpuart.h"

#if defined(MICROPY_HW_HAS_LPFLEXCOMM)
#include "fsl_lpflexcomm.h"
#endif

#include "drv_uart.h"

static void drv_uart_irq_handler(void *handle);

int drv_uart_init(drv_uart_t *uart, uint8_t id, drv_uart_config_t *cfg) {
    uart->instance = MCX_BoardGetUARTInstance(id);
    if (uart->instance == NULL) {
        return -EINVAL;
    }

    uart->id = id;

    int uart_inputfreq = MCX_BoardConfigureUARTClock(id);
    if (uart_inputfreq <= 0) {
        return -EINVAL;
    }

    lpuart_config_t uart_cfg;
    LPUART_GetDefaultConfig(&uart_cfg);

    uart_cfg.baudRate_Bps = cfg->baud_rate;
    uart_cfg.enableRx = true;
    uart_cfg.enableTx = true;

    #if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    if (cfg->data_bits == DRV_Uart7DataBits) {
        uart_cfg.dataBitsCount = kLPUART_SevenDataBits;
    }
    #endif
    if (cfg->data_bits == DRV_Uart8DataBits) {
        uart_cfg.dataBitsCount = kLPUART_EightDataBits;
    }

    switch (cfg->parity) {
        case DRV_UartNoParity:
            uart_cfg.parityMode = kLPUART_ParityDisabled;
            break;
        case DRV_UartEvenParity:
            uart_cfg.parityMode = kLPUART_ParityEven;
            break;
        case DRV_UartOddParity:
            uart_cfg.parityMode = kLPUART_ParityOdd;
            break;
        default:
            break;
    }

    /* TODO: Flow control settings. */

    LPUART_Type *lpuart = uart->instance;

    if (LPUART_Init(lpuart, &uart_cfg, uart_inputfreq) != kStatus_Success) {
        return -EIO;
    }

    #if defined(MICROPY_HW_HAS_LPFLEXCOMM)
    /* For now, SDK drivers does not handle LPUART using P2/P3 correctly, initialize the pin functions manually. */
    if (cfg->i2c_shared) {
        LP_FLEXCOMM_Init(id, LP_FLEXCOMM_PERIPH_LPI2CAndLPUART);
    }
    #endif

    uart->status = DRV_UartStatusIdle;

    return 0;
}

int drv_uart_deinit(drv_uart_t *uart) {
    int ret = 0;
    LPUART_Type *lpuart = uart->instance;

    LPUART_EnableTx(lpuart, false);
    LPUART_EnableRx(lpuart, false);

    LPUART_DisableInterrupts(lpuart, kLPUART_RxDataRegFullInterruptEnable);

    lpuart->FIFO |= LPUART_FIFO_TXFLUSH_MASK | LPUART_FIFO_RXFLUSH_MASK;

    return ret;
}

int drv_uart_write(drv_uart_t *uart, const uint8_t *data, int len, mp_uint_t timeout) {
    int ret = 0;
    LPUART_Type *lpuart = uart->instance;

    /* TX can be used when async RX is enabled */

    if (uart->status == DRV_UartStatusBusy) {
        return -EBUSY;
    }

    /* Flush TX FIFO */
    lpuart->FIFO |= LPUART_FIFO_TXFLUSH_MASK;

    LPUART_EnableTx(lpuart, true);

    mp_uint_t time_start = mp_hal_ticks_ms();

    int tx_count;
    for (tx_count = 0; tx_count < len; tx_count++) {
        /* Wait for room in TX FIFO */
        while (LPUART_GetTxFifoCount(lpuart) >= FSL_FEATURE_LPUART_FIFO_SIZEn(uart->id) - 1) {
            if (timeout && (mp_hal_ticks_ms() - time_start > timeout)) {
                ret = -ETIMEDOUT;
                goto timeout_exit;
            }

            MICROPY_EVENT_POLL_HOOK
        }

        LPUART_WriteByte(lpuart, data[tx_count]);
    }

    /* Wait for all items in FIFO has been sent... */
    while (LPUART_GetTxFifoCount(lpuart) > 0) {
        if (timeout && (mp_hal_ticks_ms() - time_start > timeout)) {
            ret = -ETIMEDOUT;
            goto timeout_exit;
        }
    }

    ret = tx_count;

timeout_exit:

    LPUART_EnableTx(lpuart, false);

    return ret;
}

int drv_uart_send_break(drv_uart_t *uart) {
    /* TODO: Implement this */

    return 0;
}

int drv_uart_read(drv_uart_t *uart, uint8_t *data, int len, mp_uint_t timeout) {
    int ret = 0;
    LPUART_Type *lpuart = uart->instance;

    if (uart->status != DRV_UartStatusIdle) {
        return -EBUSY;
    }

    uart->status = DRV_UartStatusBusy;

    lpuart->FIFO |= LPUART_FIFO_RXFLUSH_MASK;

    LPUART_EnableRx(lpuart, true);

    mp_uint_t time_start = mp_hal_ticks_ms();

    int rx_count;
    for (rx_count = 0; rx_count < len; rx_count++) {
        while (LPUART_GetRxFifoCount(lpuart) == 0) {
            if (timeout && (mp_hal_ticks_ms() - time_start > timeout)) {
                ret = rx_count;
                goto timeout_exit;
            }

            MICROPY_EVENT_POLL_HOOK
        }

        data[rx_count] = LPUART_ReadByte(lpuart);
    }

    ret = rx_count;

timeout_exit:
    LPUART_EnableRx(lpuart, false);

    uart->status = DRV_UartStatusIdle;

    return ret;
}

int drv_uart_async_callback(drv_uart_t *uart, drv_uart_async_fn_t fn) {
    if (uart->status != DRV_UartStatusIdle) {
        return -EBUSY;
    }

    uart->async_fn = fn;

    return 0;
}

int drv_uart_async_start(drv_uart_t *uart, drv_uart_async_fn_t fn, void *param) {
    if (uart->status != DRV_UartStatusIdle) {
        return -EBUSY;
    }

    LPUART_Type *lpuart = uart->instance;

    uart->async_fn = fn;
    uart->async_fn_param = param;

    uart->status = DRV_UartStatusAsyncRx;

    /* Flush FIFO */
    lpuart->FIFO |= LPUART_FIFO_RXFLUSH_MASK;

    /* Set watermark to 0 to generate a interrupt whenever there's data in FIFO. */
    LPUART_SetRxFifoWatermark(lpuart, 0U);

    /* RXFE interrupt */
    LPUART_EnableInterrupts(lpuart, kLPUART_RxDataRegFullInterruptEnable);

    MCX_BoardConfigureUARTISR(uart->id, drv_uart_irq_handler, uart);

    LPUART_EnableRx(lpuart, true);

    return 0;
}

int drv_uart_async_read(drv_uart_t *uart, uint8_t *data, uint8_t len) {
    LPUART_Type *lpuart = uart->instance;

    for (uint8_t i = 0; i < len; i++) {
        data[i] = lpuart->DATA;
    }

    return len;
}

int drv_uart_async_cancel(drv_uart_t *uart) {
    drv_uart_status_t status = uart->status;
    LPUART_Type *lpuart = uart->instance;

    if (status == DRV_UartStatusBusy) {
        return -EBUSY;
    }

    if (status == DRV_UartStatusIdle) {
        return -EINVAL;
    }

    if (status == DRV_UartStatusAsyncRx) {
        LPUART_EnableRx(lpuart, false);
        lpuart->FIFO |= LPUART_FIFO_RXFLUSH_MASK;

        LPUART_DisableInterrupts(lpuart, kLPUART_RxDataRegFullInterruptEnable);
        MCX_BoardConfigureUARTISR(uart->id, NULL, NULL);
    }

    return 0;
}

static void drv_uart_irq_handler(void *handle) {
    drv_uart_t *uart = (drv_uart_t *)handle;
    LPUART_Type *lpuart = uart->instance;

    uint32_t ip_status = LPUART_GetStatusFlags(lpuart);

    if (ip_status & kLPUART_RxDataRegFullFlag) {
        if (uart->status == DRV_UartStatusAsyncRx) {
            /* Interrupt is only active for Async RX mode. */
            uint8_t lvl = LPUART_GetRxFifoCount(lpuart);

            uart->async_fn(uart, uart->async_fn_param, lvl);
        }
    }

    LPUART_ClearStatusFlags(lpuart, ip_status);
}
