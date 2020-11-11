#include "gpio.h"
#include "helpers.h"
#include "pic32_config.h"
#include "error.h"
#include <xc.h>

#if (PIC32_PIN_COUNT == 64)
#define PIC32_PORT_COUNT 6
#elif (PIC32_PIN_COUNT == 100)
#define PIC32_PORT_COUNT 7
#elif (PIC32_PIN_COUNT == 124)
#define PIC32_PORT_COUNT 9
#elif (PIC32_PIN_COUNT == 144)
#define PIC32_PORT_COUNT 10
#else
  #error "Pin Count Inavailable in this series"
#endif

#define is_regindex_valid(x) if (x > GPIO_MAP_REG_MAX) return (ERRMAX)

static const uint32_t ports_direction[] =
{
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RA0 << 0) |
    (GPIO_RA1 << 1) |
    (GPIO_RA2 << 2) |
    (GPIO_RA3 << 3) |
    (GPIO_RA4 << 4) |
    (GPIO_RA5 << 5) |
    (GPIO_RA6 << 6) |
    (GPIO_RA7 << 7) |
    (GPIO_RA9 << 9) |
    (GPIO_RA10 << 10) |
    (GPIO_RA14 << 14) |
    (GPIO_RA15 << 15),
    #endif

    (GPIO_RB0 << 0) |
    (GPIO_RB1 << 1) |
    (GPIO_RB2 << 2) |
    (GPIO_RB3 << 3) |
    (GPIO_RB4 << 4) |
    (GPIO_RB5 << 5) |
    (GPIO_RB6 << 6) |
    (GPIO_RB7 << 7) |
    (GPIO_RB8 << 8) |
    (GPIO_RB9 << 9) |
    (GPIO_RB10 << 10) |
    (GPIO_RB11 << 11) |
    (GPIO_RB12 << 12) |
    (GPIO_RB13 << 13) |
    (GPIO_RB14 << 14) |
    (GPIO_RB15 << 15),

    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RC1 << 1) |
    (GPIO_RC2 << 2) |
    (GPIO_RC3 << 3) |
    (GPIO_RC4 << 4) |
    #endif
    (GPIO_RC12 << 12) |
    (GPIO_RC13 << 13) |
    (GPIO_RC14 << 14) |
    (GPIO_RC15 << 15),

    (GPIO_RD0 << 0) |
    (GPIO_RD1 << 1) |
    (GPIO_RD2 << 2) |
    (GPIO_RD3 << 3) |
    (GPIO_RD4 << 4) |
    (GPIO_RD5 << 5) |
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    (GPIO_RD6 << 6) |
    (GPIO_RD7 << 7) |
    #endif
    (GPIO_RD9 << 9) |
    (GPIO_RD10 << 10) |
    (GPIO_RD11 << 11) |
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RD12 << 12) |
    (GPIO_RD13 << 13) |
    (GPIO_RD14 << 14) |
    (GPIO_RD15 << 15),
    #endif

    (GPIO_RE0 << 0) |
    (GPIO_RE1 << 1) |
    (GPIO_RE2 << 2) |
    (GPIO_RE3 << 3) |
    (GPIO_RE4 << 4) |
    (GPIO_RE5 << 5) |
    (GPIO_RE6 << 6) |
    (GPIO_RE7 << 7) |
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RE8 << 8) |
    (GPIO_RE9 << 9),
    #endif

    (GPIO_RF0 << 0) |
    (GPIO_RF1 << 1) |
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RF2 << 2) |
    #endif
    (GPIO_RF3 << 3) |
    (GPIO_RF4 << 4) |
    (GPIO_RF5 << 5) |
    (GPIO_RF8 << 8) |
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RF12 << 12) |
    (GPIO_RF13 << 13),
    #endif

    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RG0 << 0) |
    (GPIO_RG1 << 1) |
    #endif
    (GPIO_RG6 << 6) |
    (GPIO_RG7 << 7) |
    (GPIO_RG8 << 8) |
    (GPIO_RG9 << 9) |
    #if (PIC32_PIN_COUNT != 64)
    (GPIO_RG12 << 12) |
    (GPIO_RG13 << 13) |
    (GPIO_RG14 << 14) |
    (GPIO_RG15 << 15),
    #endif

    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    (GPIO_RH0 << 0) |
    (GPIO_RH1 << 1) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RH2 << 2) |
    (GPIO_RH3 << 3) |
    #endif
    (GPIO_RH4 << 4) |
    (GPIO_RH5 << 5) |
    (GPIO_RH6 << 6) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RH7 << 7) |
    #endif
    (GPIO_RH8 << 8) |
    (GPIO_RH9 << 9) |
    (GPIO_RH10 << 10) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RH11 << 11) |
    #endif
    (GPIO_RH12 << 12) |
    (GPIO_RH13 << 13) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RH14 << 14) |
    (GPIO_RH15 << 15),
    #endif

    (GPIO_RJ0 << 0) |
    (GPIO_RJ1 << 1) |
    (GPIO_RJ2 << 2) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RJ3 << 3) |
    #endif
    (GPIO_RJ4 << 4) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RJ5 << 5) |
    (GPIO_RJ6 << 6) |
    (GPIO_RJ7 << 7) |
    #endif
    (GPIO_RJ8 << 8) |
    (GPIO_RJ9 << 9) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RJ10 << 10) |
    #endif
    (GPIO_RJ11 << 11) |
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RJ12 << 12) |
    (GPIO_RJ13 << 13) |
    (GPIO_RJ14 << 14) |
    (GPIO_RJ15 << 15),
    #endif
    #if (PIC32_PIN_COUNT != 124)
    (GPIO_RK0 << 0) |
    (GPIO_RK1 << 1) |
    (GPIO_RK2 << 2) |
    (GPIO_RK3 << 3) |
    (GPIO_RK4 << 4) |
    (GPIO_RK5 << 5) |
    (GPIO_RK6 << 6) |
    (GPIO_RK7 << 7)
    #endif // 124
    #endif // 64 && 100
}; // ports_direction

uint32_t volatile *gpio_tris[] =
{
    #if (PIC32_PIN_COUNT != 64)
    &TRISA,
    #endif
    &TRISB,
    &TRISC,
    &TRISD,
    &TRISE,
    &TRISF,
    &TRISG,
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    &TRISH,
    &TRISJ,
    #endif
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 124))
    &TRISK,
    #endif
};

uint32_t volatile *gpio_lat[] =
{
    #if (PIC32_PIN_COUNT != 64)
    &LATA,
    #endif
    &LATB,
    &LATC,
    &LATD,
    &LATE,
    &LATF,
    &LATG,
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    &LATH,
    &LATJ,
    #endif
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 124))
    &LATK,
    #endif
};


uint32_t volatile *gpio_port[] =
{
    #if (PIC32_PIN_COUNT != 64)
    &PORTA,
    #endif
    &PORTB,
    &PORTC,
    &PORTD,
    &PORTE,
    &PORTF,
    &PORTG,
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    &PORTH,
    &PORTJ,
    #endif
    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 124))
    &PORTK,
    #endif
};

static const uint8_t pin_map[] = {
    pinD2,
    pinG8,
    pinF4,
    pinD10,
    pinF1,
    pinB9,
    pinB10,
    pinC14,
    pinB5,
    INPUT_PIN_NC,
    #if (PIC32_PIN_COUNT != 64)
    pinC1,
    pinD14,
    pinG1,
    pinA14,
    #else
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    #endif
    #if ((PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 64))
    pinD6,
    #else
    INPUT_PIN_NC,
    #endif
    INPUT_PIN_NC,

    pinD3,
    pinG7,
    pinF5,
    pinD11,
    pinF0,
    pinB1,
    pinE5,
    pinC13,
    pinB3,
    INPUT_PIN_NC,
    #if (PIC32_PIN_COUNT != 64)
    pinC4,
    pinD15,
    pinG0,
    pinA15,
    #else
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    #endif
    #if ((PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 64))
    pinD7,
    #else
    INPUT_PIN_NC,
    #endif
    INPUT_PIN_NC,

    pinD9,
    pinG6,
    pinB8,
    pinB15,
    pinD4,
    pinB0,
    pinE3,
    pinB7,
    INPUT_PIN_NC,
    #if (PIC32_PIN_COUNT != 64)
    pinF12,
    pinD12,
    pinF8,
    pinC3,
    pinE9,
    #else
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,

    #endif
    INPUT_PIN_NC,
    INPUT_PIN_NC,

    pinD1,
    pinG9,
    pinB14,
    pinD0,
    INPUT_PIN_NC,
    pinB6,
    pinD5,
    pinB2,
    pinF3,
    #if (PIC32_PIN_COUNT != 64)
    pinF13,
    INPUT_PIN_NC,
    pinF2,
    pinC2,
    pinE8,
    #else
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    INPUT_PIN_NC,
    #endif
    INPUT_PIN_NC,
    INPUT_PIN_NC,
};

static uint32_t volatile *const output_map_register[] = {
    &RPD2R,
    &RPG8R,
    &RPF4R,
    &RPD10R,
    &RPF1R,
    &RPB9R,
    &RPB10R,
    &RPC14R,
    &RPB5R,
    NULL,
    #if (PIC32_PIN_COUNT != 64)
    &RPC1R,
    &RPD14R,
    &RPG1R,
    &RPA14R,
    #else
    NULL,
    NULL,
    NULL,
    NULL,
    #endif
    #if ((PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 64))
    &RPD6R,
    #else
    NULL,
    #endif
    NULL,

    &RPD3R,
    &RPG7R,
    &RPF5R,
    &RPD11R,
    &RPF0R,
    &RPB1R,
    &RPE5R,
    &RPC13R,
    &RPB3R,
    NULL,
    #if (PIC32_PIN_COUNT != 64)
    &RPC4R,
    &RPD15R,
    &RPG0R,
    &RPA15R,
    #else
    NULL,
    NULL,
    NULL,
    NULL,
    #endif
    #if ((PIC32_PIN_COUNT != 100) && (PIC32_PIN_COUNT != 64))
    &RPD7R,
    #else
    NULL,
    #endif
    NULL,

    &RPD9R,
    &RPG6R,
    &RPB8R,
    &RPB15R,
    &RPD4R,
    &RPB0R,
    &RPE3R,
    &RPB7R,
    NULL,
    #if (PIC32_PIN_COUNT != 64)
    &RPF12R,
    &RPD12R,
    &RPF8R,
    &RPC3R,
    &RPE9R,
    #else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    #endif
    NULL,
    NULL,

    &RPD1R,
    &RPG9R,
    &RPB14R,
    &RPD0R,
    NULL,
    &RPB6R,
    &RPD5R,
    &RPB2R,
    &RPF3R,
    #if (PIC32_PIN_COUNT != 64)
    &RPF13R,
    NULL,
    &RPF2R,
    &RPC2R,
    &RPE8R,
    #else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    #endif
    NULL,
    NULL,
};

static inline pic32_gpio_port_t getportnumber(pic32_pin_t pin) {
    return pin / PIC32_MAX_PORT_PIN;
}

int gpio_outfunc_map_set(int func_index, uint8_t value) {
    is_regindex_valid(func_index);
    *output_map_register[func_index] = value;
    return OK;
}

void gpio_output_set(pic32_pin_t pin) {
    pic32_gpio_port_t port = getportnumber(pin);
    pin = pin - (port * PIC32_MAX_PORT_PIN);

    BIT_CLR(gpio_tris[port], pin);
}
void gpio_input_set(pic32_pin_t pin) {
    pic32_gpio_port_t port = getportnumber(pin);
    pin = pin - (port * PIC32_MAX_PORT_PIN);

    BIT_SET(gpio_tris[port], pin);
}

void gpio_state_set(pic32_pin_t pin, bool state) {
    pic32_gpio_port_t port = getportnumber(pin);
    pin = pin - (port * PIC32_MAX_PORT_PIN);

    if (state) {
        BIT_SET(gpio_lat[port], pin);
    } else {
        BIT_CLR(gpio_lat[port], pin);
    }
}

void gpio_state_toggle(pic32_pin_t pin) {
    pic32_gpio_port_t port = getportnumber(pin);
    pin = pin - (port * PIC32_MAX_PORT_PIN);

    BIT_INV(gpio_lat[port], pin);
}

bool gpio_state_get(pic32_pin_t pin) {
    pic32_gpio_port_t port = getportnumber(pin);
    pin = pin - (port * PIC32_MAX_PORT_PIN);

    return !((*gpio_port[port] & ~(1 << pin)) == 0);
}

int gpio_map_getindex(pic32_pin_t pin) {
    int i;

    for (i = 0; i < GPIO_MAP_REG_MAX; i++)
    {
        if (pin == pin_map[i]) {
            return i;
        }

    }
    return INPUT_PIN_NC;
}

void gpio_init(void) {
    int i;
    #if (PIC32_PIN_COUNT != 64)
    ANSELA = 0;
    #endif

    ANSELB = 0;

    #if (PIC32_PIN_COUNT != 64)
    ANSELC = 0;
    #endif

    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    ANSELD = 0;
    #endif

    ANSELE = 0;
    #if (PIC32_PIN_COUNT != 64)
    ANSELF = 0;
    #endif

    ANSELG = 0;

    #if ((PIC32_PIN_COUNT != 64) && (PIC32_PIN_COUNT != 100))
    ANSELH = 0;
    ANSELJ = 0;
    #endif

    // Direction of gpio
    for (i = 0; i < PIC32_PORT_COUNT; i++)
    {
        *gpio_tris[i] = ports_direction[i];
    }

}