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
#include "pin.h"

mp_obj_t pyb_gpio(uint n_args, mp_obj_t *args) {
    //assert(1 <= n_args && n_args <= 2);

    const pin_obj_t *pin = pin_map_user_obj(args[0]);
    GPIO_TypeDef *port = pin->gpio;
    uint16_t pin_mask = pin->pin_mask;

    if (n_args == 1) {
        // get pin
        if ((port->IDR & pin_mask) != (uint32_t)Bit_RESET) {
            return MP_OBJ_NEW_SMALL_INT(1);
        } else {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
    } else {
        // set pin
        if (mp_obj_is_true(args[1])) {
            // set pin high
            port->BSRRL = pin_mask;
        } else {
            // set pin low
            port->BSRRH = pin_mask;
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_obj, 1, 2, pyb_gpio);

mp_obj_t pyb_gpio_input(mp_obj_t arg_pin, mp_obj_t arg_mode) {
    const pin_obj_t *pin = pin_map_user_obj(arg_pin);
    GPIO_TypeDef *port = pin->gpio;
    uint16_t pin_mask = pin->pin_mask;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pin_mask;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = mp_obj_get_int(arg_mode);
    GPIO_Init(port, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_input_obj, pyb_gpio_input);

mp_obj_t pyb_gpio_output(mp_obj_t arg_pin, mp_obj_t arg_mode) {
    const pin_obj_t *pin = pin_map_user_obj(arg_pin);
    GPIO_TypeDef *port = pin->gpio;
    uint16_t pin_mask = pin->pin_mask;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pin_mask;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_OType = mp_obj_get_int(arg_mode);
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(port, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_output_obj, pyb_gpio_output);
