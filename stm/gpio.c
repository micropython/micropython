// This is a woefully inadequate set of bindings for GPIO control, and
// needs to be replaced with something much better.

#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_pwr.h>
#include <stm32f4xx_rtc.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rng.h>
#include <usbd_storage_msd.h>
#include <stm_misc.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "misc.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "systick.h"
#include "gpio.h"

void parse_pin_name(mp_obj_t name, GPIO_TypeDef **port, uint *pin) {
    const char *pin_name = mp_obj_str_get_str(name);
    switch (pin_name[0]) {
        case 'A': case 'a': *port = GPIOA; break;
        case 'B': case 'b': *port = GPIOB; break;
        case 'C': case 'c': *port = GPIOC; break;
        case 'D': case 'd': *port = GPIOD; break;
        default: goto pin_error;
    }
    *pin = 0;
    for (const char *s = pin_name + 1; *s; s++) {
        if (!('0' <= *s && *s <= '9')) {
            goto pin_error;
        }
        *pin = 10 * (*pin) + *s - '0';
    }
    if (!(0 <= *pin && *pin <= 15)) {
        goto pin_error;
    }

    return;

pin_error:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s does not exist", pin_name));
}

mp_obj_t pyb_gpio(uint n_args, mp_obj_t *args) {
    //assert(1 <= n_args && n_args <= 2);

    GPIO_TypeDef *port;
    uint pin;
    parse_pin_name(args[0], &port, &pin);

    if (n_args == 1) {
        // get pin
        if ((port->IDR & (1 << pin)) != (uint32_t)Bit_RESET) {
            return MP_OBJ_NEW_SMALL_INT(1);
        } else {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
    } else {
        // set pin
        if (rt_is_true(args[1])) {
            // set pin high
            port->BSRRL = 1 << pin;
        } else {
            // set pin low
            port->BSRRH = 1 << pin;
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_obj, 1, 2, pyb_gpio);

mp_obj_t pyb_gpio_input(mp_obj_t arg_port_pin, mp_obj_t arg_mode) {
    GPIO_TypeDef *port;
    uint pin;
    parse_pin_name(arg_port_pin, &port, &pin);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = 1 << pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = mp_obj_get_int(arg_mode);
    GPIO_Init(port, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_input_obj, pyb_gpio_input);

mp_obj_t pyb_gpio_output(mp_obj_t arg_port_pin, mp_obj_t arg_mode) {
    GPIO_TypeDef *port;
    uint pin;
    parse_pin_name(arg_port_pin, &port, &pin);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = 1 << pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_OType = mp_obj_get_int(arg_mode);
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(port, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_output_obj, pyb_gpio_output);

void gpio_init(mp_obj_t mod) {
    rt_store_attr(mod, MP_QSTR_gpio, (mp_obj_t)&pyb_gpio_obj);
    rt_store_attr(mod, MP_QSTR_gpio_in, (mp_obj_t)&pyb_gpio_input_obj);
    rt_store_attr(mod, MP_QSTR_gpio_out, (mp_obj_t)&pyb_gpio_output_obj);
    rt_store_attr(mod, qstr_from_str("PULL_NONE"), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_NOPULL));
    rt_store_attr(mod, qstr_from_str("PULL_UP"), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_UP));
    rt_store_attr(mod, qstr_from_str("PULL_DOWN"), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_DOWN));
    rt_store_attr(mod, qstr_from_str("PUSH_PULL"), MP_OBJ_NEW_SMALL_INT(GPIO_OType_PP));
    rt_store_attr(mod, qstr_from_str("OPEN_DRAIN"), MP_OBJ_NEW_SMALL_INT(GPIO_OType_OD));
}
