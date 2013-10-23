#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include "led.h"

#define PYB_LED_R_PORT (GPIOA)
#define PYB_LED_R1_PIN (GPIO_Pin_8)
#define PYB_LED_R2_PIN (GPIO_Pin_10)
#define PYB_LED_G_PORT   (GPIOC)
#define PYB_LED_G1_PIN (GPIO_Pin_4)
#define PYB_LED_G2_PIN (GPIO_Pin_5)

void led_init(void) {
    // set the output high (so LED is off)
    PYB_LED_R_PORT->BSRRL = PYB_LED_R1_PIN;
    PYB_LED_R_PORT->BSRRL = PYB_LED_R2_PIN;
    PYB_LED_G_PORT->BSRRL = PYB_LED_G1_PIN;
    PYB_LED_G_PORT->BSRRL = PYB_LED_G2_PIN;
    // make them open drain outputs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PYB_LED_R1_PIN | PYB_LED_R2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(PYB_LED_R_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = PYB_LED_G1_PIN | PYB_LED_G2_PIN;
    GPIO_Init(PYB_LED_G_PORT, &GPIO_InitStructure);
}

void led_state(pyb_led_t led, int state) {
    GPIO_TypeDef *port;
    uint32_t pin;
    switch (led) {
        case PYB_LED_R1: port = PYB_LED_R_PORT; pin = PYB_LED_R1_PIN; break;
        case PYB_LED_R2: port = PYB_LED_R_PORT; pin = PYB_LED_R2_PIN; break;
        case PYB_LED_G1: port = PYB_LED_G_PORT; pin = PYB_LED_G1_PIN; break;
        case PYB_LED_G2: port = PYB_LED_G_PORT; pin = PYB_LED_G2_PIN; break;
        default: return;
    }
    if (state == 0) {
        // turn LED off (output is high)
        port->BSRRL = pin;
    } else {
        // turn LED on (output is low)
        port->BSRRH = pin;
    }
}

void led_toggle(pyb_led_t led) {
    GPIO_TypeDef *port;
    uint32_t pin;
    switch (led) {
        case PYB_LED_R1: port = PYB_LED_R_PORT; pin = PYB_LED_R1_PIN; break;
        case PYB_LED_R2: port = PYB_LED_R_PORT; pin = PYB_LED_R2_PIN; break;
        case PYB_LED_G1: port = PYB_LED_G_PORT; pin = PYB_LED_G1_PIN; break;
        case PYB_LED_G2: port = PYB_LED_G_PORT; pin = PYB_LED_G2_PIN; break;
        default: return;
    }
    if (!(port->ODR & pin)) {
        // turn LED off (output high)
        port->BSRRL = pin;
    } else {
        // turn LED on (output low)
        port->BSRRH = pin;
    }
}
