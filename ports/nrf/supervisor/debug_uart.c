/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jun2Sak
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

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#ifdef NRF_DEBUG_PRINT

#define DEBUG_UART_TXPIN 26
#define DEBUG_UART_RXPIN 15

#include "nrfx.h"
#include "nrf_uart.h"
#include "nrf_gpio.h"
#include "nrf_rtc.h"
#include "nrfx_uarte.h"
#include "nrfx_rtc.h"
#include "supervisor/serial.h" // dbg_printf()
#include "shared-bindings/microcontroller/Processor.h"
#include "common-hal/alarm/__init__.h"

extern const nrfx_rtc_t rtc_instance;    // port.c
extern uint32_t reset_reason_saved;

const nrfx_uarte_t _dbg_uart_inst = NRFX_UARTE_INSTANCE(1);
static int _dbg_uart_initialized = 0;
#define DBG_PBUF_LEN 80
static char _dbg_pbuf[DBG_PBUF_LEN + 1];

void _debug_uart_uninit(void) {
    nrf_gpio_cfg(DEBUG_UART_TXPIN,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrfx_uarte_uninit(&_dbg_uart_inst);
}

void _debug_uart_init(void) {
    // if (_dbg_uart_initialized) return;
    nrfx_uarte_config_t config = {
        .pseltxd = DEBUG_UART_TXPIN,
        .pselrxd = DEBUG_UART_RXPIN,
        .pselcts = NRF_UARTE_PSEL_DISCONNECTED,
        .pselrts = NRF_UARTE_PSEL_DISCONNECTED,
        .p_context = NULL,
        .baudrate = NRF_UART_BAUDRATE_115200,
        .interrupt_priority = 7,
        .hal_cfg = {
            .hwfc = NRF_UARTE_HWFC_DISABLED,
            .parity = NRF_UARTE_PARITY_EXCLUDED
        }
    };
    nrfx_uarte_init(&_dbg_uart_inst, &config, NULL);
    // drive config
    nrf_gpio_cfg(config.pseltxd,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_PULLUP,  // orig=NOPULL
        NRF_GPIO_PIN_H0H1,   // orig=S0S1
        NRF_GPIO_PIN_NOSENSE);
    _dbg_uart_initialized = 1;
    return;
}

void _debug_print_substr(const char *text, uint32_t length) {
    char *data = (char *)text;
    int siz;
    while (length != 0) {
        if (length <= DBG_PBUF_LEN) {
            siz = length;
        } else {
            siz = DBG_PBUF_LEN;
        }
        memcpy(_dbg_pbuf, data, siz);
        _dbg_pbuf[siz] = 0;
        nrfx_uarte_tx(&_dbg_uart_inst, (uint8_t const *)_dbg_pbuf, siz);
        data += siz;
        length -= siz;
    }
}

void _debug_uart_deinit(void) {
    nrfx_uarte_uninit(&_dbg_uart_inst);
}

int dbg_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vprintf(fmt, ap);
    va_end(ap);
    return ret;
}

void dbg_dump_RTCreg(void) {
    dbg_printf("\r\nRTC2\r\n");
    NRF_RTC_Type *r = rtc_instance.p_reg;
    dbg_printf("PRESCALER=%08X, ", (int)r->PRESCALER);
    dbg_printf("COUNTER=%08X  ", (int)r->COUNTER);
    dbg_printf("INTENSET=%08X ", (int)r->INTENSET);
    dbg_printf("EVTENSET=%08X\r\n", (int)r->EVTENSET);
    dbg_printf("EVENTS_COMPARE[0..3]=%X,%X,%X,%X ", (int)r->EVENTS_COMPARE[0], (int)r->EVENTS_COMPARE[1], (int)r->EVENTS_COMPARE[2], (int)r->EVENTS_COMPARE[3]);
    dbg_printf("CC[0..3]=%08X,%08X,%08X,%08X\r\n", (int)r->CC[0], (int)r->CC[1], (int)r->CC[2], (int)r->CC[3]);
}

int dbg_check_RTCprescaler(void) {
    NRF_RTC_Type *r = rtc_instance.p_reg;
    if ((int)r->PRESCALER == 0) {
        dbg_printf("****** PRESCALER == 0\r\n");
        return -1;
    }
    return 0;
}

void dbg_dump_RAMreg(void) {
    int i;
    for (i = 0; i <= 8; ++i) {
        dbg_printf(" RAM%d:%08X", i, (int)(NRF_POWER->RAM[i].POWER));
        if (i == 4) {
            dbg_printf("\r\n");
        }
    }
    dbg_printf("\r\n");
}

void dbg_dump_GPIOregs(void) {
    int i, port, col;

    NRF_GPIO_Type *gpio[] = { NRF_P0, NRF_P1 };
    const char cnf_pull_chr[] = "-D*U";  // pull down, pull up
    const char cnf_sense_chr[] = "-?HL"; // sense high, sense low
    for (port = 0, col = 0; port <= 1; ++port) {
        for (i = 0; i < 32; ++i) {
            uint32_t cnf = gpio[port]->PIN_CNF[i];
            if (cnf != 0x0002) { // changed from default value
                dbg_printf("[%d_%02d]:%c%c%c%d%c ", port, i,
                    (cnf & 1) ? 'O' : 'I', // output, input
                    (cnf & 2) ? 'd' : 'c', // disconnected, connected
                    cnf_pull_chr[(cnf >> 2) & 3],
                    (int)((cnf >> 8) & 7), // drive config 0-7
                    cnf_sense_chr[(cnf >> 16) & 3]);
                if (++col >= 6) {
                    dbg_printf("\r\n");
                    col = 0;
                }
            }
        }
    }
    if (col > 0) {
        dbg_printf("\r\n");
    }

    dbg_printf("GPIOTE\r\n");
    NRF_GPIOTE_Type const *reg = NRF_GPIOTE;
    const char config_mode_chr[] = "-E-T"; // event, task
    const char config_pol_chr[] = "-HLT"; // low-to-Hi, hi-to-Low, Toggle
    const char config_outinit_chr[] = "01"; // initial value is 0 or 1
    for (i = 0, col = 0; i < 8; ++i) {
        uint32_t conf = reg->CONFIG[i];
        if (conf != 0) { // changed from default value
            dbg_printf("CONFIG[%d]:%d_%02d,%c%c%c ", i,
                (int)((conf >> 13) & 1), (int)((conf >> 8) & 0x1F),
                config_mode_chr[conf & 3],
                config_pol_chr[(conf >> 16) & 3],
                (conf & 3) == 3 ?
                config_outinit_chr[(conf >> 20) & 1] : '-');
            if (++col >= 4) {
                dbg_printf("\r\n");
                col = 0;
            }
        }
    }
    if (col > 0) {
        dbg_printf("\r\n");
    }
    for (i = 0; i < 8; ++i) {
        dbg_printf("EVENTS_IN[%d]:%X ", i, (int)(reg->EVENTS_IN[i]));
        if ((i & 3) == 3) {
            dbg_printf("\r\n");
        }
    }
    dbg_printf("EVENTS_PORT:%X INTENSET:%08X\r\n",
        (int)(reg->EVENTS_PORT), (int)(reg->INTENSET));
}

void dbg_dumpQSPIreg(void) {
    uint32_t r;
    dbg_printf("QSPI\r\n");
    r = NRF_QSPI->IFCONFIG0;
    dbg_printf("IFCONFIG0 READ=%ld write=%ld ADDR=%ld DPM=%ld PPSIZE=%ld\r\n",
        r & 7, (r >> 3) & 7, (r >> 6) & 1, (r >> 7) & 1, (r >> 12) & 1);
    r = NRF_QSPI->IFCONFIG1;
    dbg_printf("IFCONFIG1 SCKDELAY=%ld SPIMODE=%ld SCKFREQ=%ld\r\n",
        r & 0xFF, (r >> 25) & 1, (r >> 28) & 0xF);
    r = NRF_QSPI->STATUS;
    dbg_printf("STATUS    DPM=%ld READY=%ld SREG=0x%02lX\r\n",
        (r >> 2) & 1, (r >> 3) & 1, (r >> 24) & 0xFF);
    r = NRF_QSPI->DPMDUR;
    dbg_printf("DPMDUR    ENTER=%ld EXIT=%ld\r\n", r & 0xFFFF, (r >> 16) & 0xFFFF);
}

void dbg_dump_reset_reason(void) {
    int reset_reason = (int)common_hal_mcu_processor_get_reset_reason();
    const char *rr_str[] = {
        "POWER_ON", "BROWNOUT", "SOFTWARE", "DEEPSLEEPALARM",
        "RESET_PIN", "WATCHDOG", "UNKNOWN"
    };
    dbg_printf("reset_reason=%s\r\n", rr_str[reset_reason]);
}

#else /*!NRF_DEBUG_PRINT*/
int dbg_printf(const char *fmt, ...) {
    return 0;
}
#endif /*!NRF_DEBUG_PRINT*/
