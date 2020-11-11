#include "board.h"
#include "sysclk.h"
#include "interrupt.h"
#include "gpio.h"
#include "uart.h"

void mpu_init(void) {
    sysclk_init();
    interrupt_init();
    gpio_init();

    // i use rs485 convert, RS_pin need to be set.
    gpio_state_set(RS_PIN, true);

    uart_init(UART_ATTACHED_TO_REPL, NO_PARITY_8_BIT_DATA, ONE_STOP_BIT, REPL_BAUD_RATE);

}

void led_init(void) {
    // the pins are already set as output in pic32_config, as a demo.
    gpio_output_set(RED_LED);
    gpio_output_set(ORANGE_LED);
}

void led_state(int led, int state) {
    gpio_state_set(led, state);
}

void led_toggle(int led) {
    gpio_state_toggle(led);
}

int uart_rx_any(void) {
    return uart_rx_ready(PIC32_UART_4);
}

int uart_rx_char(void) {
    return uart_read_char(PIC32_UART_4);
}

void uart_tx_char(int chr) {
    uart_send_char(PIC32_UART_4, chr);
}