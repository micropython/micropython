#ifndef MICROPY_INCLUDED_PIC32BIT_BOARD_H
#define MICROPY_INCLUDED_PIC32BIT_BOARD_H

#include "pic32_config.h"
#include "gpio.h"

#define RED_LED    (pinB13)
#define ORANGE_LED (pinB12)
#define RS_PIN     (pinA10)

extern int _stack;
extern int _heap;

// Heap start / end definitions
#define PIC32_HEAP_START &_heap
#define PIC32_HEAP_END &_stack - 16 * 1024

void mpu_init(void);

void led_init(void);
void led_state(int led, int state);
void led_toggle(int led);

int uart_rx_any(void);
int uart_rx_char(void);
void uart_tx_char(int chr);

#endif // MICROPY_INCLUDED_PIC32BIT_BOARD_H
