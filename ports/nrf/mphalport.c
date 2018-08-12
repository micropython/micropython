/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#include <string.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "uart.h"
#include "nrfx_errors.h"
#include "nrfx_config.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = MP_EIO,
    [HAL_BUSY] = MP_EBUSY,
    [HAL_TIMEOUT] = MP_ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    mp_raise_OSError(mp_hal_status_to_errno_table[status]);
}

#if !MICROPY_KBD_EXCEPTION
void mp_hal_set_interrupt_char(int c) {

}
#endif

#if !MICROPY_PY_BLE_NUS
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (MP_STATE_PORT(board_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(board_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(board_stdio_uart));
        }
    }

    return 0;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (MP_STATE_PORT(board_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(board_stdio_uart), str, len);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    if (MP_STATE_PORT(board_stdio_uart) != NULL) {
        uart_tx_strn_cooked(MP_STATE_PORT(board_stdio_uart), str, len);
    }
}
#endif

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_delay_us(mp_uint_t us)
{
    if (us == 0) {
        return;
    }

    register uint32_t delay __ASM ("r0") = us;
    __ASM volatile (
#ifdef NRF51
            ".syntax unified\n"
#endif
        "1:\n"
        " SUBS %0, %0, #1\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
#ifdef NRF52
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
#endif
        " BNE 1b\n"
        : "+r" (delay));
}

void mp_hal_delay_ms(mp_uint_t ms)
{
    for (mp_uint_t i = 0; i < ms; i++)
    {
        mp_hal_delay_us(999);
    }
}

#if defined(NRFX_LOG_ENABLED) && (NRFX_LOG_ENABLED == 1)

static const char nrfx_error_unknown[1] = "";

static const char nrfx_error_success[]            = "NRFX_SUCCESS";
static const char nrfx_error_internal[]           = "NRFX_ERROR_INTERNAL";
static const char nrfx_error_no_mem[]             = "NRFX_ERROR_NO_MEM";
static const char nrfx_error_not_supported[]      = "NRFX_ERROR_NOT_SUPPORTED";
static const char nrfx_error_invalid_param[]      = "NRFX_ERROR_INVALID_PARAM";
static const char nrfx_error_invalid_state[]      = "NRFX_ERROR_INVALID_STATE";
static const char nrfx_error_invalid_length[]     = "NRFX_ERROR_INVALID_LENGTH";
static const char nrfx_error_timeout[]            = "NRFX_ERROR_TIMEOUT";
static const char nrfx_error_forbidden[]          = "NRFX_ERROR_FORBIDDEN";
static const char nrfx_error_null[]               = "NRFX_ERROR_NULL";
static const char nrfx_error_invalid_addr[]       = "NRFX_ERROR_INVALID_ADDR";
static const char nrfx_error_busy[]               = "NRFX_ERROR_BUSY";
static const char nrfx_error_already_initalized[] = "NRFX_ERROR_ALREADY_INITIALIZED";

static const char * nrfx_error_strings[13] = {
    nrfx_error_success,
    nrfx_error_internal,
    nrfx_error_no_mem,
    nrfx_error_not_supported,
    nrfx_error_invalid_param,
    nrfx_error_invalid_state,
    nrfx_error_invalid_length,
    nrfx_error_timeout,
    nrfx_error_forbidden,
    nrfx_error_null,
    nrfx_error_invalid_addr,
    nrfx_error_busy,
    nrfx_error_already_initalized
};

static const char nrfx_drv_error_twi_err_overrun[] = "NRFX_ERROR_DRV_TWI_ERR_OVERRUN";
static const char nrfx_drv_error_twi_err_anack[]   = "NRFX_ERROR_DRV_TWI_ERR_ANACK";
static const char nrfx_drv_error_twi_err_dnack[]   = "NRFX_ERROR_DRV_TWI_ERR_DNACK";

static const char * nrfx_drv_error_strings[3] = {
    nrfx_drv_error_twi_err_overrun,
    nrfx_drv_error_twi_err_anack,
    nrfx_drv_error_twi_err_dnack
};

const char * nrfx_error_code_lookup(uint32_t err_code) {
    if (err_code >= NRFX_ERROR_BASE_NUM && err_code <= NRFX_ERROR_BASE_NUM + 13) {
        return nrfx_error_strings[err_code - NRFX_ERROR_BASE_NUM];
    } else if (err_code >= NRFX_ERROR_DRIVERS_BASE_NUM && err_code <= NRFX_ERROR_DRIVERS_BASE_NUM + 3) {
        return nrfx_drv_error_strings[err_code - NRFX_ERROR_DRIVERS_BASE_NUM];
    }

    return nrfx_error_unknown;
}

#endif // NRFX_LOG_ENABLED
