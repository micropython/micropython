#include <stdio.h>
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "led.h"

/* LED numbers, used internally */
#define PYB_LED_1   (0)
#define PYB_LED_2   (1)
#define PYB_LED_3   (2)
#define PYB_LED_4   (3)

#if defined(PYBOARD)
    #define PYB_LED1_PORT   (GPIOA)
    #define PYB_LED1_PIN    (GPIO_Pin_8)

    #define PYB_LED2_PORT   (GPIOA)
    #define PYB_LED2_PIN    (GPIO_Pin_10)

    #define PYB_LED3_PORT   (GPIOC)
    #define PYB_LED3_PIN    (GPIO_Pin_4)

    #define PYB_LED4_PORT   (GPIOC)
    #define PYB_LED4_PIN    (GPIO_Pin_5)

    #define PYB_OTYPE       (GPIO_OType_OD)

    #define PYB_LED_ON(port, pin)  (port->BSRRH = pin)
    #define PYB_LED_OFF(port, pin) (port->BSRRL = pin)

#elif defined(STM32F4DISC)
    #define PYB_LED1_PORT   (GPIOD)
    #define PYB_LED1_PIN    (GPIO_Pin_14)

    #define PYB_LED2_PORT   (GPIOD)
    #define PYB_LED2_PIN    (GPIO_Pin_12)

    #define PYB_LED3_PORT   (GPIOD)
    #define PYB_LED3_PIN    (GPIO_Pin_15)

    #define PYB_LED4_PORT   (GPIOD)
    #define PYB_LED4_PIN    (GPIO_Pin_13)

    #define PYB_OTYPE       (GPIO_OType_PP)

    #define PYB_LED_ON(port, pin)  (port->BSRRL = pin)
    #define PYB_LED_OFF(port, pin) (port->BSRRH = pin)
#endif

void led_init(void) {
    /* GPIO structure */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure I/O speed, mode, output type and pull */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = PYB_OTYPE;

    /* Turn off LEDs */
    PYB_LED_OFF(PYB_LED1_PORT, PYB_LED1_PIN);
    PYB_LED_OFF(PYB_LED2_PORT, PYB_LED2_PIN);
    PYB_LED_OFF(PYB_LED3_PORT, PYB_LED1_PIN);
    PYB_LED_OFF(PYB_LED4_PORT, PYB_LED2_PIN);

    /* Initialize LEDs */
    GPIO_InitStructure.GPIO_Pin = PYB_LED1_PIN;
    GPIO_Init(PYB_LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PYB_LED2_PIN;
    GPIO_Init(PYB_LED2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PYB_LED3_PIN;
    GPIO_Init(PYB_LED3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PYB_LED4_PIN;
    GPIO_Init(PYB_LED4_PORT, &GPIO_InitStructure);
}

void led_state(pyb_led_t led, int state) {
    GPIO_TypeDef *port;
    uint32_t pin;

    switch (led) {
        case PYB_LED_1:
            pin  = PYB_LED1_PIN;
            port = PYB_LED1_PORT;
            break;
        case PYB_LED_2:
            pin  = PYB_LED2_PIN;
            port = PYB_LED2_PORT;
            break;
        case PYB_LED_3:
            pin  = PYB_LED3_PIN;
            port = PYB_LED3_PORT;
            break;
        case PYB_LED_4:
            pin  = PYB_LED4_PIN;
            port = PYB_LED4_PORT;
            break;
        default:
            return;
    }

    if (state == 0) {
        // turn LED off
        PYB_LED_OFF(port, pin);
    } else {
        // turn LED on
        PYB_LED_ON(port, pin);
    }
}

void led_toggle(pyb_led_t led) {
    GPIO_TypeDef *port;
    uint32_t pin;

    switch (led) {
        case PYB_LED_1:
            pin  = PYB_LED1_PIN;
            port = PYB_LED1_PORT;
            break;
        case PYB_LED_2:
            pin  = PYB_LED2_PIN;
            port = PYB_LED2_PORT;
            break;
        case PYB_LED_3:
            pin  = PYB_LED3_PIN;
            port = PYB_LED3_PORT;
            break;
        case PYB_LED_4:
            pin  = PYB_LED4_PIN;
            port = PYB_LED4_PORT;
            break;
        default:
            return;
    }

    if (!(port->ODR & pin)) {
        // turn LED off
        PYB_LED_OFF(port, pin);
    } else {
        // turn LED on (output low)
        PYB_LED_ON(port, pin);
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
    led_state(self->led_id, 1);
    return mp_const_none;
}

mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self->led_id, 0);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);

static const mp_method_t led_obj_type_methods[] = {
	{ "on", &led_obj_on_obj },
	{ "off", &led_obj_off_obj },
	{ NULL, NULL },
};
static const mp_obj_type_t led_obj_type = {
    { &mp_const_type },
    "Led",
    .print = led_obj_print,
    .methods = led_obj_type_methods,
};

mp_obj_t pyb_Led(mp_obj_t led_id) {
    pyb_led_obj_t *o = m_new_obj(pyb_led_obj_t);
    o->base.type = &led_obj_type;
    o->led_id = mp_obj_get_int(led_id);
    return o;
}
