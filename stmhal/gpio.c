// This is a woefully inadequate set of bindings for GPIO control, and
// needs to be replaced with something much better.

#include <stdio.h>
#include <string.h>
#include <stm32f4xx_hal.h>

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
    const pin_obj_t *pin = pin_map_user_obj(args[0]);
    if (n_args == 1) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(HAL_GPIO_ReadPin(pin->gpio, pin->pin_mask));
    }

    // set pin
    HAL_GPIO_WritePin(pin->gpio, pin->pin_mask, mp_obj_is_true(args[1]));
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_obj, 1, 2, pyb_gpio);

mp_obj_t pyb_gpio_input(uint n_args, mp_obj_t *args) {
    const pin_obj_t *pin = pin_map_user_obj(args[0]);

    uint32_t pull = GPIO_NOPULL;
    if (n_args > 1) {
        pull = mp_obj_get_int(args[1]);
    }
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin->pin_mask;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = pull;
    HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_input_obj, 1, 2, pyb_gpio_input);

mp_obj_t pyb_gpio_output(uint n_args, mp_obj_t *args) {
    const pin_obj_t *pin = pin_map_user_obj(args[0]);

    uint32_t mode = GPIO_MODE_OUTPUT_PP;
    if (n_args > 1) {
        mode = mp_obj_get_int(args[1]) == GPIO_MODE_OUTPUT_OD ?
                 GPIO_MODE_OUTPUT_OD: GPIO_MODE_OUTPUT_PP;
    }
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin->pin_mask;
    GPIO_InitStructure.Mode = mode;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_gpio_output_obj, 1, 2, pyb_gpio_output);
