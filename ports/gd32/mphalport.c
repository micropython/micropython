// mphalport.c - port HAL implementation for GD32
#include <stdint.h>
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "gd32f4xx.h"
#include "mphalport.h"
#include "shared/runtime/softtimer.h"

// REPL UART: USART0 PA9(TX)/PA10(RX) AF7
#define REPL_UART       USART0
#define REPL_UART_RCU   RCU_USART0
#define REPL_GPIO_RCU   RCU_GPIOA
#define REPL_TX_PIN     GPIO_PIN_9
#define REPL_RX_PIN     GPIO_PIN_10
#define REPL_AF         GPIO_AF_7

static int uart_ready = 0;

void uart_repl_init(void) {
    if (uart_ready) {
        return;
    }
    rcu_periph_clock_enable(REPL_GPIO_RCU);
    rcu_periph_clock_enable(REPL_UART_RCU);

    gpio_af_set(GPIOA, REPL_AF, REPL_TX_PIN | REPL_RX_PIN);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, REPL_TX_PIN);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, REPL_TX_PIN);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, REPL_RX_PIN);

    usart_deinit(REPL_UART);
    usart_baudrate_set(REPL_UART, MICROPY_HW_UART_REPL_BAUD);
    usart_word_length_set(REPL_UART, USART_WL_8BIT);
    usart_stop_bit_set(REPL_UART, USART_STB_1BIT);
    usart_parity_config(REPL_UART, USART_PM_NONE);
    usart_hardware_flow_rts_config(REPL_UART, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(REPL_UART, USART_CTS_DISABLE);
    usart_receive_config(REPL_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(REPL_UART, USART_TRANSMIT_ENABLE);
    usart_enable(REPL_UART);
    uart_ready = 1;
}

int mp_hal_stdin_rx_chr(void) {
    uart_repl_init();
    while (RESET == usart_flag_get(REPL_UART, USART_FLAG_RBNE)) {
        ;
    }
    return (int)usart_data_receive(REPL_UART);
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    uart_repl_init();
    for (size_t i = 0; i < len; i++) {
        while (RESET == usart_flag_get(REPL_UART, USART_FLAG_TBE)) {
            ;
        }
        usart_data_transmit(REPL_UART, (uint8_t)str[i]);
    }
    return len;
}

// SysTick
volatile uint32_t systick_ms = 0;

void SysTick_Handler(void) {
    systick_ms++;
    #if MICROPY_PY_MACHINE_TIMER
    // run soft timer callbacks at 1 kHz
    soft_timer_handler();
    #endif
}

mp_uint_t mp_hal_ticks_ms(void) {
    return systick_ms;
}

mp_uint_t mp_hal_ticks_us(void) {
    return systick_ms * 1000;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return DWT->CYCCNT;
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint32_t start = systick_ms;
    while ((systick_ms - start) < ms) {
        __WFI();
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    if (us >= 1000) {
        mp_hal_delay_ms(us / 1000);
        us %= 1000;
    }
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles) {
        ;
    }
}

void mp_hal_set_interrupt_char(int c) {
    (void)c;
}

// ---- pin low-level operations (used by extmod soft protocols) ----
void mp_hal_pin_write_impl_(mp_hal_pin_obj_t pin, int value) {
    uint8_t port = PIN_PORT(pin.pin);
    uint32_t mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_bit_write(pin_port_base(port), mask, value ? SET : RESET);
}

void mp_hal_pin_open_drain_impl(mp_hal_pin_obj_t pin) {
    uint8_t port = PIN_PORT(pin.pin);
    uint32_t mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, mask);
    gpio_output_options_set(pin_port_base(port), GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, mask);
    gpio_bit_set(pin_port_base(port), mask);
}

int mp_hal_pin_read_impl(mp_hal_pin_obj_t pin) {
    uint8_t port = PIN_PORT(pin.pin);
    uint32_t mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    return gpio_input_bit_get(pin_port_base(port), mask) ? 1 : 0;
}

void mp_hal_pin_input_impl(mp_hal_pin_obj_t pin) {
    uint8_t port = PIN_PORT(pin.pin);
    uint32_t mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_INPUT, GPIO_PUPD_NONE, mask);
}

void mp_hal_pin_output_impl(mp_hal_pin_obj_t pin) {
    uint8_t port = PIN_PORT(pin.pin);
    uint32_t mask = (uint32_t)(1U << PIN_NUM(pin.pin));
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, mask);
    gpio_output_options_set(pin_port_base(port), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, mask);
}

// ---- wall-clock time for filesystem timestamps ----
// We only have a millisecond uptime counter plus (optionally) the RTC.
// Report epoch time when the RTC is configured, otherwise a fixed epoch
// offset by uptime so that file mtimes at least increase monotonically.
extern uint64_t machine_rtc_get_epoch_seconds(void) __attribute__((weak));

uint64_t mp_hal_time_ns(void) {
    uint64_t secs;
    if (machine_rtc_get_epoch_seconds != NULL) {
        secs = machine_rtc_get_epoch_seconds();
    } else {
        secs = 1762300000ull;   // fallback epoch (2025-11-05)
    }
    return secs * 1000000000ull + (uint64_t)systick_ms * 1000000ull;
}
