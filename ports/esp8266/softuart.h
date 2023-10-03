#ifndef SOFTUART_H_
#define SOFTUART_H_

#include "user_interface.h"

#define SOFTUART_MAX_RX_BUFF 256

#define SOFTUART_GPIO_COUNT 16

typedef struct {
    uint8_t gpio_id;
    uint32_t gpio_mux_name;
    uint8_t gpio_func;
} softuart_pin_t;

typedef struct {
    char receive_buffer[SOFTUART_MAX_RX_BUFF];
    uint8_t receive_buffer_tail;
    uint8_t receive_buffer_head;
    uint8_t buffer_overflow;
} softuart_buffer_t;

typedef struct {
    softuart_pin_t pin_rx;
    softuart_pin_t pin_tx;
    // optional rs485 tx enable pin (high -> tx enabled)
    uint8_t pin_rs485_tx_enable;
    // wether or not this softuart is rs485 and controlls rs485 tx enable pin
    uint8_t is_rs485;
    volatile softuart_buffer_t buffer;
    uint32_t bit_time;
    uint32_t elapsed;
} Softuart;


BOOL Softuart_Available(Softuart *s);
uint32_t Softuart_Flush(Softuart *s);
BOOL Softuart_rxWait(Softuart *s, uint32_t timeout_us);
void Softuart_Intr_Handler(void *p);  // void* for type compatibility with etshal.h: void ets_isr_attach(int irq_no, void (*handler)(void *), void *arg);
uint8_t Softuart_IsGpioValid(uint8_t gpio_id);
void Softuart_SetPinRx(Softuart *s, uint8_t gpio_id);
void Softuart_SetPinTx(Softuart *s, uint8_t gpio_id);
void Softuart_EnableRs485(Softuart *s, uint8_t gpio_id);
void Softuart_Init(Softuart *s, uint32_t baudrate);
void Softuart_Putchar(Softuart *s, char data);
void Softuart_Puts(Softuart *s, const char *c);
uint8_t Softuart_Read(Softuart *s);
uint8_t Softuart_Readline(Softuart *s, char *Buffer, uint8_t MaxLen);
// check status of rx/tx
int Softuart_rx_any(Softuart *s);
int Softuart_tx_any_room(Softuart *s);

// define mapping from pin to functio mode
typedef struct {
    uint32_t gpio_mux_name;
    uint8_t gpio_func;
} softuart_reg_t;

#endif /* SOFTUART_H_ */
