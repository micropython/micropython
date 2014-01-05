#include <stdio.h>
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
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

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    uint led_id;
} pyb_led_obj_t;

void led_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    print(env, "<LED %lu>", self->led_id);
}

mp_obj_t led_obj_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    switch (self->led_id) {
        case 1: led_state(PYB_LED_G1, 1); break;
        case 2: led_state(PYB_LED_G2, 1); break;
    }
    return mp_const_none;
}

mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    switch (self->led_id) {
        case 1: led_state(PYB_LED_G1, 0); break;
        case 2: led_state(PYB_LED_G2, 0); break;
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);

static const mp_obj_type_t led_obj_type = {
    { &mp_const_type },
    "Led",
    .print = led_obj_print,
    .methods = {
        { "on", &led_obj_on_obj },
        { "off", &led_obj_off_obj },
        { NULL, NULL },
    }
};

mp_obj_t pyb_Led(mp_obj_t led_id) {
    pyb_led_obj_t *o = m_new_obj(pyb_led_obj_t);
    o->base.type = &led_obj_type;
    o->led_id = mp_obj_get_int(led_id);
    return o;
}
