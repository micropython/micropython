/*
 * debug functions
 *  (will be removed)
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#ifdef MY_DEBUGUART

#define DEBUG_UART_TXPIN 26
#define DEBUG_UART_RXPIN 15

#include "nrfx.h"
#include "nrf_uart.h"
#include "nrf_gpio.h"
#include "nrf_rtc.h"
#include "nrfx_uarte.h"
#include "nrfx_rtc.h"
#include "supervisor/serial.h" // dbg_printf()

extern const nrfx_rtc_t rtc_instance;    // port.c
extern volatile int rtc_woke_up_counter; // port.c

const nrfx_uarte_t _dbg_uart_inst = NRFX_UARTE_INSTANCE(1);
static int _dbg_uart_initialized = 0;
#define DBG_PBUF_LEN 80
static char _dbg_pbuf[DBG_PBUF_LEN+1];

void _debug_uart_init(void) {
    //if (_dbg_uart_initialized) return;
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
		 NRF_GPIO_PIN_PULLUP, // orig=NOPULL
		 NRF_GPIO_PIN_H0H1,  // orig=S0S1
		 NRF_GPIO_PIN_NOSENSE);
    _dbg_uart_initialized = 1;
    return;
}

void _debug_print_substr(const char* text, uint32_t length) {
    char* data = (char*)text;
    int   siz;
    while(length != 0) {
        if (length <= DBG_PBUF_LEN) {
	  siz = length;
	}
	else {
	  siz = DBG_PBUF_LEN;
	}
	memcpy(_dbg_pbuf, data, siz);
	_dbg_pbuf[siz] = 0;
	nrfx_uarte_tx(&_dbg_uart_inst, (uint8_t const*)_dbg_pbuf, siz);
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


void dbg_dumpRTC(void) {
    dbg_printf("\r\nRTC2\r\n");
    NRF_RTC_Type  *r = rtc_instance.p_reg;
    dbg_printf("PRESCALER=%08X, ", (int)r->PRESCALER);
    dbg_printf("COUNTER=%08X  ", (int)r->COUNTER);
    dbg_printf("INTENSET=%08X ", (int)r->INTENSET);
    dbg_printf("EVTENSET=%08X\r\n", (int)r->EVTENSET);
    dbg_printf("EVENTS_COMPARE[0..3]=%X,%X,%X,%X ", (int)r->EVENTS_COMPARE[0], (int)r->EVENTS_COMPARE[1], (int)r->EVENTS_COMPARE[2], (int)r->EVENTS_COMPARE[3]);
    dbg_printf("CC[0..3]=%08X,%08X,%08X,%08X\r\n", (int)r->CC[0], (int)r->CC[1], (int)r->CC[2], (int)r->CC[3]);
    dbg_printf("woke_up=%d\r\n", rtc_woke_up_counter);
}

#else /*!MY_DEBUGUART*/
int dbg_printf(const char *fmt, ...) {
    return 0;
}
#endif /*!MY_DEBUGUART*/
