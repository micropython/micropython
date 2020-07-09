#include <bsd_os.h>
#include <nrf_errno.h>
#include <bsd_platform.h>
#include <bsd_limits.h>

#include <nrf.h>
#include "nrf_gpio.h"
#include "errno.h"
#include "py/mphal.h"

#define BSD_OS_TRACE_ENABLED 0

#if BSD_OS_TRACE_ENABLED

#include <nrfx_uarte.h>

#define BSD_OS_TRACE_IRQ          EGU2_IRQn
#define BSD_OS_TRACE_IRQ_PRIORITY 6
#define BSD_OS_TRACE_IRQ_HANDLER  EGU2_IRQHandler

#define BSD_OS_TRACE_UART_IRQ_PRIORITY 3
#define BSD_OS_TRACE_UART_FLOW_CONTROL 0
#define BSD_OS_TRACE_UART_INSTANCE     3
#define BSD_OS_TRACE_UART_IRQ_PRIORITY 3

#define BSD_OS_TRACE_PIN_RX  0
#define BSD_OS_TRACE_PIN_TX  1
#define BSD_OS_TRACE_PIN_RTS 14
#define BSD_OS_TRACE_PIN_CTS 15

static nrfx_uarte_t m_uarte_instance = NRFX_UARTE_INSTANCE(BSD_OS_TRACE_UART_INSTANCE);

#endif // BSD_OS_TRACE_ENABLED

typedef struct {
    uint32_t context;
    mp_uint_t prev_time;
} bsd_os_timer_t;

static bsd_os_timer_t timers[8];

void read_task_create(void) {
    // The read task is achieved using SW interrupt.
    NVIC_SetPriority(BSD_APPLICATION_IRQ, BSD_APPLICATION_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(BSD_APPLICATION_IRQ);
    NVIC_EnableIRQ(BSD_APPLICATION_IRQ);
}

#if BSD_OS_TRACE_ENABLED
void trace_uart_init(void) {
    const nrfx_uarte_config_t config = {
        .pseltxd = BSD_OS_TRACE_PIN_TX,
        .pselrxd = BSD_OS_TRACE_PIN_RX,
        .pselcts = BSD_OS_TRACE_PIN_CTS,
        .pselrts = BSD_OS_TRACE_PIN_RTS,
        .hwfc = BSD_OS_TRACE_UART_FLOW_CONTROL,
        .parity = NRF_UARTE_PARITY_EXCLUDED,
        .baudrate = NRF_UARTE_BAUDRATE_1000000,
        .interrupt_priority = BSD_OS_TRACE_UART_IRQ_PRIORITY,
        .p_context = NULL,
    };

    nrfx_uarte_init(&m_uarte_instance, &config, NULL);
}

void trace_task_create(void) {
    NVIC_SetPriority(BSD_OS_TRACE_IRQ, BSD_OS_TRACE_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(BSD_OS_TRACE_IRQ);
    NVIC_EnableIRQ(BSD_OS_TRACE_IRQ);
}
#endif
void bsd_os_init(void) {
    read_task_create();
    #if BSD_OS_TRACE_ENABLED
    trace_uart_init();
    trace_task_create();
    #endif
}

int32_t locate_timer(uint32_t context) {
    for (int i = 0; i < 8; i++)
    {
        if (timers[i].context == context) {
            return i;
        }
    }
    return -1;
}

int32_t locate_free(void) {
    for (int i = 0; i < 8; i++)
    {
        if (timers[i].context == 0) {
            return i;
        }
    }
    return -1;
}

int32_t bsd_os_timedwait(uint32_t context, int32_t *timeout) {
    if (*timeout == BSD_OS_FOREVER) {
        return 0;
    }

    if (*timeout == 0) {
        return NRF_ETIMEDOUT;
    }

    mp_uint_t tick = mp_hal_ticks_ms();
    // Locate timer or free spot.
    int32_t timer = locate_timer(context);
    if (timer == -1) {
        timer = locate_free();

        if (timer == -1) {
            return NRF_ENOMEM;
        }

        timers[timer].context = context;
        timers[timer].prev_time = tick;
        // First time, only allocate.
        return 0;
    }
    mp_uint_t diff = tick - timers[timer].prev_time;

    if (diff > *timeout) {

        timers[timer].context = 0;
        timers[timer].prev_time = 0;
        return NRF_ETIMEDOUT;
    }
    return 0;
}

int nrf_errno;

void bsd_os_errno_set(int errno_val) {
    nrf_errno = errno_val;
}

void bsd_os_application_irq_set(void) {
    NVIC_SetPendingIRQ(BSD_APPLICATION_IRQ);
}


void bsd_os_application_irq_clear(void) {
    NVIC_ClearPendingIRQ(BSD_APPLICATION_IRQ);
}

void BSD_APPLICATION_IRQ_HANDLER(void) {
    bsd_os_application_irq_handler();
}

void bsd_os_trace_irq_set(void) {
    #if BSD_OS_TRACE_ENABLED
    NVIC_SetPendingIRQ(BSD_OS_TRACE_IRQ);
    #else
    bsd_os_trace_irq_handler();
    #endif
}

void bsd_os_trace_irq_clear(void) {
    #if BSD_OS_TRACE_ENABLED
    NVIC_ClearPendingIRQ(BSD_OS_TRACE_IRQ);
    #endif
}

#if BSD_OS_TRACE_ENABLED
void BSD_OS_TRACE_IRQ_HANDLER(void) {
    bsd_os_trace_irq_handler();
}
#endif

int32_t bsd_os_trace_put(const uint8_t *const p_buffer, uint32_t buf_len) {
    #if BSD_OS_TRACE_ENABLED
    uint32_t remaining_bytes = buf_len;

    while (remaining_bytes) {
        uint8_t transfered_len = MIN(remaining_bytes, UINT8_MAX);
        uint32_t index = buf_len - remaining_bytes;

        nrfx_uarte_tx(&m_uarte_instance, &p_buffer[index], transfered_len);
        remaining_bytes -= transfered_len;
    }
    #endif
    return 0;
}
