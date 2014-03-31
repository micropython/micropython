#include <stdio.h>
#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "usrsw.h"

#include "exti.h"
#include "gpio.h"
#include "pin.h"
#include "build/pins.h"

// Usage Model:
//
// pyb.switch() returns True if the user switch is pressed, False otherwise.
//
// pyb.switch(callback) will register a callback to be called when the user
//                      switch is pressed.
//
// pyb.switch(None) will remove the callback.
//
// Example:
//
// def switch_pressed():
//     print("User Switch pressed")
//
// pyb.switch(switch_pressed)

static mp_obj_t switch_user_callback_obj;

static mp_obj_t switch_callback(mp_obj_t line) {
    if (switch_user_callback_obj != mp_const_none) {
        mp_call_function_0(switch_user_callback_obj);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(switch_callback_obj, switch_callback);

// this function inits the switch GPIO so that it can be used
void switch_init0(void) {
    GPIO_InitTypeDef init;
    init.Pin = MICROPY_HW_USRSW_PIN.pin_mask;
    init.Mode = GPIO_MODE_INPUT;
    init.Pull = MICROPY_HW_USRSW_PULL;
    init.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(MICROPY_HW_USRSW_PIN.gpio, &init);
}

// this function inits the callback and EXTI function of the switch
void switch_init(void) {
    switch_user_callback_obj = mp_const_none;
    exti_register((mp_obj_t)&MICROPY_HW_USRSW_PIN,
                  MP_OBJ_NEW_SMALL_INT(MICROPY_HW_USRSW_EXTI_MODE),
                  MP_OBJ_NEW_SMALL_INT(MICROPY_HW_USRSW_PULL),
                  (mp_obj_t)&switch_callback_obj,
                  NULL);
}

int switch_get(void) {
    int val = ((MICROPY_HW_USRSW_PIN.gpio->IDR & MICROPY_HW_USRSW_PIN.pin_mask) != 0);
    return val == MICROPY_HW_USRSW_PRESSED;
}

/******************************************************************************/
/* Micro Python bindings                                                      */

static mp_obj_t pyb_switch(uint n_args, mp_obj_t *args) {
    if (n_args == 0) {
        return switch_get() ? mp_const_true : mp_const_false;
    }
    switch_user_callback_obj = args[0];
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_switch_obj, 0, 1, pyb_switch);
