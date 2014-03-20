#include <stdio.h>
#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "led.h"
#include "pin.h"
#include "build/pins.h"

static const pin_obj_t *gLed[] = {
    &PYB_LED1,
#if defined(PYB_LED2)
    &PYB_LED2,
#if defined(PYB_LED3)
    &PYB_LED3,
#if defined(PYB_LED4)
    &PYB_LED4,
#endif
#endif
#endif
};
#define NUM_LEDS (sizeof(gLed) / sizeof(gLed[0]))

void led_init(void) {
    /* GPIO structure */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure I/O speed, mode, output type and pull */
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Mode = PYB_OTYPE;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    /* Turn off LEDs and initialize */
    for (int led = 0; led < NUM_LEDS; led++) {
        PYB_LED_OFF(gLed[led]); 
        GPIO_InitStructure.Pin = gLed[led]->pin_mask;
        HAL_GPIO_Init(gLed[led]->gpio, &GPIO_InitStructure);
    }
}

void led_state(pyb_led_t led, int state) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }
    const pin_obj_t *led_pin = gLed[led - 1];
//printf("led_state(%d,%d)\n", led, state);
    if (state == 0) {
        // turn LED off
        PYB_LED_OFF(led_pin);
    } else {
        // turn LED on
        PYB_LED_ON(led_pin);
    }
}

void led_toggle(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }
    const pin_obj_t *led_pin = gLed[led - 1];
    GPIO_TypeDef *gpio = led_pin->gpio;

    // We don't know if we're turning the LED on or off, but we don't really
    // care. Just invert the state.
    if (gpio->ODR & led_pin->pin_mask) {
        // pin is high, make it low
        gpio->BSRRH = led_pin->pin_mask;
    } else {
        // pin is low, make it high
        gpio->BSRRL = led_pin->pin_mask;
    }
}

void led_debug(int n, int delay) {
    led_state(1, n & 1);
    led_state(2, n & 2);
    led_state(3, n & 4);
    led_state(4, n & 8);
    HAL_Delay(delay);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    uint led_id;
} pyb_led_obj_t;

void led_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
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

mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_toggle(self->led_id);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
static MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);

static const mp_method_t led_methods[] = {
    { "on", &led_obj_on_obj },
    { "off", &led_obj_off_obj },
    { "toggle", &led_obj_toggle_obj },
    { NULL, NULL },
};

static const mp_obj_type_t led_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Led,
    .print = led_obj_print,
    .methods = led_methods,
};

static mp_obj_t pyb_Led(mp_obj_t led_id) {
    pyb_led_obj_t *o = m_new_obj(pyb_led_obj_t);
    o->base.type = &led_obj_type;
    o->led_id = mp_obj_get_int(led_id);
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_Led_obj, pyb_Led);
