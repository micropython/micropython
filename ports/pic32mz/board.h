#ifndef MICROPY_INCLUDED_PIC32BIT_BOARD_H
#define MICROPY_INCLUDED_PIC32BIT_BOARD_H

#define RED_LED    (13)
#define ORANGE_LED (12)
#define RS_PIN     (10)

#define SYS_FREQ 200000000 // 200MHz

extern int _stack;
extern int _heap;
// Heap start / end definitions
#define PIC32_HEAP_START &_heap
#define PIC32_HEAP_END &_stack - 16*1024 /* Stack is tunable */

void mpu_init(void);

void led_init(void);
void led_state(int led, int state);
void led_toggle(int led);

void uart_init(void);
int uart_rx_any(void);
int uart_rx_char(void);
void uart_tx_char(int chr);
void print_str(char *s);
void set_rs_pin(void);
void clear_rs_pin(void);
#endif // MICROPY_INCLUDED_PIC32BIT_BOARD_H