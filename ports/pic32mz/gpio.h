#ifndef MICROPY_INCLUDED_PIC32_GPIO_H
#define MICROPY_INCLUDED_PIC32_GPIO_H

#include <stdint.h>
#include <stdbool.h>

#define PIC32_MAX_PORT_PIN (16U)
#define GPIO_MAP_REG_MAX   (PIC32_MAX_PORT_PIN * 4U)
#define INPUT_PIN_NC       (0xFFU)

typedef enum
{
    #if (PIC32_PIN_COUNT != 64)
    PIC32_PORTA,
    #endif
    PIC32_PORTB,
    PIC32_PORTC,
    PIC32_PORTD,
    PIC32_PORTE,
    PIC32_PORTF,
    PIC32_PORTG,
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    PIC32_PORTH,
    PIC32_PORTJ,
    #endif
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 124))
    PIC32_PORTK,
    #endif
    PIC32_PORT_MAX,
}pic32_gpio_port_t;

enum
{
    #if (PIC32_PIN_COUNT != 64)
    PIC32_BASE_PORTA = PIC32_PORTA * PIC32_MAX_PORT_PIN,
    #endif
    PIC32_BASE_PORTB = PIC32_PORTB * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTC = PIC32_PORTC * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTD = PIC32_PORTD * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTE = PIC32_PORTE * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTF = PIC32_PORTF * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTG = PIC32_PORTG * PIC32_MAX_PORT_PIN,
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    PIC32_BASE_PORTH = PIC32_PORTH * PIC32_MAX_PORT_PIN,
    PIC32_BASE_PORTJ = PIC32_PORTJ * PIC32_MAX_PORT_PIN,
    #endif
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100) && \
    (PIC32_PIN_COUNT != 124))
    PIC32_BASE_PORTK = PIC32_PORTK * PIC32_MAX_PORT_PIN,
    #endif
};

enum
{
    PIC32_GPIO_OUTPUT,
    PIC32_GPIO_INPUT,
};


typedef enum
{
    #if (PIC32_PIN_COUNT != 64)
    pinA0  = PIC32_BASE_PORTA + 0,
    pinA1  = PIC32_BASE_PORTA + 1,
    pinA2  = PIC32_BASE_PORTA + 2,
    pinA3  = PIC32_BASE_PORTA + 3,
    pinA4  = PIC32_BASE_PORTA + 4,
    pinA5  = PIC32_BASE_PORTA + 5,
    pinA6  = PIC32_BASE_PORTA + 6,
    pinA7  = PIC32_BASE_PORTA + 7,
    pinA9  = PIC32_BASE_PORTA + 9,
    pinA10 = PIC32_BASE_PORTA + 10,
    pinA14 = PIC32_BASE_PORTA + 14,
    pinA15 = PIC32_BASE_PORTA + 15,
    #endif

    pinB0  = PIC32_BASE_PORTB + 0,
    pinB1  = PIC32_BASE_PORTB + 1,
    pinB2  = PIC32_BASE_PORTB + 2,
    pinB3  = PIC32_BASE_PORTB + 3,
    pinB4  = PIC32_BASE_PORTB + 4,
    pinB5  = PIC32_BASE_PORTB + 5,
    pinB6  = PIC32_BASE_PORTB + 6,
    pinB7  = PIC32_BASE_PORTB + 7,
    pinB8  = PIC32_BASE_PORTB + 8,
    pinB9  = PIC32_BASE_PORTB + 9,
    pinB10 = PIC32_BASE_PORTB + 10,
    pinB11 = PIC32_BASE_PORTB + 11,
    pinB12 = PIC32_BASE_PORTB + 12,
    pinB13 = PIC32_BASE_PORTB + 13,
    pinB14 = PIC32_BASE_PORTB + 14,
    pinB15 = PIC32_BASE_PORTB + 15,

    #if (PIC32_PIN_COUNT != 64)
    pinC1  = PIC32_BASE_PORTC + 1,
    pinC2  = PIC32_BASE_PORTC + 2,
    pinC3  = PIC32_BASE_PORTC + 3,
    pinC4  = PIC32_BASE_PORTC + 4,
    #endif

    pinC12 = PIC32_BASE_PORTC + 12,
    pinC13 = PIC32_BASE_PORTC + 13,
    pinC14 = PIC32_BASE_PORTC + 14,
    pinC15 = PIC32_BASE_PORTC + 15,

    pinD0  = PIC32_BASE_PORTD + 0,
    pinD1  = PIC32_BASE_PORTD + 1,
    pinD2  = PIC32_BASE_PORTD + 2,
    pinD3  = PIC32_BASE_PORTD + 3,
    pinD4  = PIC32_BASE_PORTD + 4,
    pinD5  = PIC32_BASE_PORTD + 5,

    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    pinD6  = PIC32_BASE_PORTD + 6,
    pinD7  = PIC32_BASE_PORTD + 7,
    #endif

    pinD9  = PIC32_BASE_PORTD + 9,
    pinD10 = PIC32_BASE_PORTD + 10,
    pinD11 = PIC32_BASE_PORTD + 11,

    #if (PIC32_PIN_COUNT != 64)
    pinD12 = PIC32_BASE_PORTD + 12,
    pinD13 = PIC32_BASE_PORTD + 13,
    pinD14 = PIC32_BASE_PORTD + 14,
    pinD15 = PIC32_BASE_PORTD + 15,
    #endif

    pinE0  = PIC32_BASE_PORTE + 0,
    pinE1  = PIC32_BASE_PORTE + 1,
    pinE2  = PIC32_BASE_PORTE + 2,
    pinE3  = PIC32_BASE_PORTE + 3,
    pinE4  = PIC32_BASE_PORTE + 4,
    pinE5  = PIC32_BASE_PORTE + 5,
    pinE6  = PIC32_BASE_PORTE + 6,
    pinE7  = PIC32_BASE_PORTE + 7,

    #if (PIC32_PIN_COUNT != 64)
    pinE8  = PIC32_BASE_PORTE + 8,
    pinE9  = PIC32_BASE_PORTE + 9,
    #endif

    pinF0  = PIC32_BASE_PORTF + 0,
    pinF1  = PIC32_BASE_PORTF + 1,

    #if (PIC32_PIN_COUNT != 64)
    pinF2  = PIC32_BASE_PORTF + 2,
    #endif

    pinF3  = PIC32_BASE_PORTF + 3,
    pinF4  = PIC32_BASE_PORTF + 4,
    pinF5  = PIC32_BASE_PORTF + 5,
    pinF8  = PIC32_BASE_PORTF + 8,

    #if (PIC32_PIN_COUNT != 64)
    pinF12 = PIC32_BASE_PORTF + 12,
    pinF13 = PIC32_BASE_PORTF + 13,
    #endif

    #if (PIC32_PIN_COUNT != 64)
    pinG0  = PIC32_BASE_PORTG + 0,
    pinG1  = PIC32_BASE_PORTG + 1,
    #endif

    pinG6  = PIC32_BASE_PORTG + 6,
    pinG7  = PIC32_BASE_PORTG + 7,
    pinG8  = PIC32_BASE_PORTG + 8,
    pinG9  = PIC32_BASE_PORTG + 9,

    #if (PIC32_PIN_COUNT != 64)
    pinG12 = PIC32_BASE_PORTG + 12,
    pinG13 = PIC32_BASE_PORTG + 13,
    pinG14 = PIC32_BASE_PORTG + 14,
    pinG15 = PIC32_BASE_PORTG + 15,
    #endif

    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    pinH0  = PIC32_BASE_PORTH + 0,
    pinH1  = PIC32_BASE_PORTH + 1,

    #if (PIC32_PIN_COUNT != 124)
    pinH2  = PIC32_BASE_PORTH + 2,
    pinH3  = PIC32_BASE_PORTH + 3,
    #endif

    pinH4  = PIC32_BASE_PORTH + 4,
    pinH5  = PIC32_BASE_PORTH + 5,
    pinH6  = PIC32_BASE_PORTH + 6,

    #if (PIC32_PIN_COUNT != 124)
    pinH7  = PIC32_BASE_PORTH + 7,
    #endif

    pinH8  = PIC32_BASE_PORTH + 8,
    pinH9  = PIC32_BASE_PORTH + 9,
    pinH10 = PIC32_BASE_PORTH + 10,

    #if (PIC32_PIN_COUNT != 124)
    pinH11 = PIC32_BASE_PORTH + 11,
    #endif

    pinH12 = PIC32_BASE_PORTH + 12,
    pinH13 = PIC32_BASE_PORTH + 13,

    #if (PIC32_PIN_COUNT != 124)
    pinH14 = PIC32_BASE_PORTH + 14,
    pinH15 = PIC32_BASE_PORTH + 15,
    #endif

    pinJ0  = PIC32_BASE_PORTJ + 0,
    pinJ1  = PIC32_BASE_PORTJ + 1,
    pinJ2  = PIC32_BASE_PORTJ + 2,

    #if (PIC32_PIN_COUNT != 124)
    pinJ3  = PIC32_BASE_PORTJ + 3,
    #endif

    pinJ4  = PIC32_BASE_PORTJ + 4,

    #if (PIC32_PIN_COUNT != 124)
    pinJ5  = PIC32_BASE_PORTJ + 5,
    pinJ6  = PIC32_BASE_PORTJ + 6,
    pinJ7  = PIC32_BASE_PORTJ + 7,
    #endif

    pinJ8  = PIC32_BASE_PORTJ + 8,
    pinJ9  = PIC32_BASE_PORTJ + 9,

    #if (PIC32_PIN_COUNT != 124)
    pinJ10 = PIC32_BASE_PORTJ + 10,
    #endif

    pinJ11 = PIC32_BASE_PORTJ + 11,

    #if (PIC32_PIN_COUNT != 124)
    pinJ12 = PIC32_BASE_PORTJ + 12,
    pinJ13 = PIC32_BASE_PORTJ + 13,
    pinJ14 = PIC32_BASE_PORTJ + 14,
    pinJ15 = PIC32_BASE_PORTJ + 15,
    #endif

    #if (PIC32_PIN_COUNT != 124)
    pinK0  = PIC32_BASE_PORTK + 0,
    pinK1  = PIC32_BASE_PORTK + 1,
    pinK2  = PIC32_BASE_PORTK + 2,
    pinK3  = PIC32_BASE_PORTK + 3,
    pinK4  = PIC32_BASE_PORTK + 4,
    pinK5  = PIC32_BASE_PORTK + 5,
    pinK6  = PIC32_BASE_PORTK + 6,
    pinK7  = PIC32_BASE_PORTK + 7,
    #endif // 124

    #endif // 64 && 100
}pic32_pin_t;

void gpio_init(void);
bool gpio_state_get(pic32_pin_t nbr);
void gpio_state_set(pic32_pin_t nbr, bool state);
void gpio_input_set(pic32_pin_t nbr);
void gpio_output_set(pic32_pin_t nbr);
void gpio_state_toggle(pic32_pin_t nbr);
int gpio_outfunc_map_set(int func_index, uint8_t value);
int gpio_map_getindex(pic32_pin_t pin);

#endif // MICROPY_INCLUDED_PIC32_GPIO_H
