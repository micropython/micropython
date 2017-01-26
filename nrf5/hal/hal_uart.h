/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef HAL_UART_H__
#define HAL_UART_H__

#include <stdint.h>
#include <stdbool.h>

#include "nrf.h"

#if NRF51

#define UART_HWCONTROL_NONE                  ((uint32_t)UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos)
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)(UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos)
#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == UART_HWCONTROL_NONE) || \
                               ((CONTROL) == UART_HWCONTROL_RTS_CTS))

#elif NRF52

#define UART_HWCONTROL_NONE                  ((uint32_t)UARTE_CONFIG_HWFC_Disabled << UARTE_CONFIG_HWFC_Pos)
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)(UARTE_CONFIG_HWFC_Enabled << UARTE_CONFIG_HWFC_Pos)
#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == UART_HWCONTROL_NONE) || \
                               ((CONTROL) == UART_HWCONTROL_RTS_CTS))
#else
#error "Device not supported."
#endif
typedef enum {
  HAL_UART_STATE_RESET             = 0x00,    /*!< Peripheral is not yet Initialized                  */
  HAL_UART_STATE_READY             = 0x01,    /*!< Peripheral Initialized and ready for use           */
  HAL_UART_STATE_BUSY              = 0x02,    /*!< an internal process is ongoing                     */
  HAL_UART_STATE_BUSY_TX           = 0x12,    /*!< Data Transmission process is ongoing               */
  HAL_UART_STATE_BUSY_RX           = 0x22,    /*!< Data Reception process is ongoing                  */
  HAL_UART_STATE_BUSY_TX_RX        = 0x32,    /*!< Data Transmission and Reception process is ongoing */
  HAL_UART_STATE_TIMEOUT           = 0x03,    /*!< Timeout state                                      */
  HAL_UART_STATE_ERROR             = 0x04     /*!< Error                                              */
} HAL_UART_StateTypeDef;

typedef enum
{
  HAL_UART_ERROR_NONE      = 0x00,    /*!< No error */
  HAL_UART_ERROR_ORE       = 0x01,    /*!< Overrun error. A start bit is received while the previous data still lies in RXD. (Previous data is lost.) */
  HAL_UART_ERROR_PE        = 0x02,    /*!< Parity error. A character with bad parity is received, if HW parity check is enabled. */
  HAL_UART_ERROR_FE        = 0x04,    /*!< Frame error. A valid stop bit is not detected on the serial data input after all bits in a character have been received. */
  HAL_UART_ERROR_BE        = 0x08,    /*!< Break error. The serial data input is '0' for longer than the length of a data frame. (The data frame length is 10 bits without parity bit, and 11 bits with parity bit.). */
} HAL_UART_ErrorTypeDef;

typedef enum {
    HAL_UART_BAUD_1K2 = 0,        /**< 1200 baud */
    HAL_UART_BAUD_2K4,            /**< 2400 baud */
    HAL_UART_BAUD_4K8,            /**< 4800 baud */
    HAL_UART_BAUD_9K6,            /**< 9600 baud */
    HAL_UART_BAUD_14K4,           /**< 14.4 kbaud */
    HAL_UART_BAUD_19K2,           /**< 19.2 kbaud */
    HAL_UART_BAUD_28K8,           /**< 28.8 kbaud */
    HAL_UART_BAUD_38K4,           /**< 38.4 kbaud */
    HAL_UART_BAUD_57K6,           /**< 57.6 kbaud */
    HAL_UART_BAUD_76K8,           /**< 76.8 kbaud */
    HAL_UART_BAUD_115K2,          /**< 115.2 kbaud */
    HAL_UART_BAUD_230K4,          /**< 230.4 kbaud */
    HAL_UART_BAUD_250K0,          /**< 250.0 kbaud */
    HAL_UART_BAUD_500K0,          /**< 500.0 kbaud */
    HAL_UART_BAUD_1M0             /**< 1 mbaud */
} hal_uart_baudrate_t;

typedef struct {
    uint32_t baud_rate;
    uint32_t flow_control;
} UART_InitTypeDef;

typedef struct
{
    NRF_UART_Type      *instance;          /* UART registers base address        */
    UART_InitTypeDef   init;               /* UART communication parameters      */
    uint8_t            *p_tx_buff;         /* Pointer to UART Tx transfer Buffer */
    uint16_t           tx_xfer_size;       /* UART Tx Transfer size              */
    uint16_t           tx_xfer_count;      /* UART Tx Transfer Counter           */
    uint8_t            *p_rx_buff;         /* Pointer to UART Rx transfer Buffer */
    uint16_t           rx_xfer_size;       /* UART Rx Transfer size              */
    uint16_t           rx_xfer_count;      /* UART Rx Transfer Counter           */
    __IO HAL_UART_StateTypeDef state;      /* UART communication state           */
    __IO HAL_UART_ErrorTypeDef error_code; /* UART Error code                    */

} UART_HandleTypeDef;

typedef struct {
    const pin_obj_t * rx_pin;       /**< RX pin. */
    const pin_obj_t * tx_pin;       /**< TX pin. */
    const pin_obj_t * rts_pin;      /**< RTS pin, only used if flow control is enabled. */
    const pin_obj_t * cts_pin;      /**< CTS pin, only used if flow control is enabled. */
    bool              flow_control; /**< Flow control setting, if flow control is used, the system will use low power UART mode, based on CTS signal. */
    bool              use_parity;   /**< Even parity if TRUE, no parity if FALSE. */
    uint32_t          baud_rate;    /**< Baud rate configuration. */
    uint32_t          irq_priority; /**< UARTE IRQ priority. */
} hal_uart_init_t;


typedef void (*uart_complete_cb)(void);

void nrf_uart_init(hal_uart_init_t const * p_uart_init);

void nrf_uart_char_write(uint8_t ch);

uint8_t nrf_uart_char_read(void);

void nrf_uart_buffer_write(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb);

void nrf_uart_buffer_read(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb);

#endif // UART_H__
