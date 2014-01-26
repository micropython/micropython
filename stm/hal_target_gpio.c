#include <string.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

#include "gpio_api.h"
#include "hal.h"
#include "hal_gpio.h"

const static hal_int_const target_gpio_const[] = {
  { "DIR_INPUT",        0 },
  { "DIR_OUTPUT",       1 },

  { "PUD_NONE",         0 },
  { "PUD_UP",           1 },
  { "PUD_DOWN",         2 },
  { "PUD_OPEN_DRAIN",   3 },

  { "SPEED_LOW",        0 },
  { "SPEED_MEDIUM",     1 },
  { "SPEED_FAST",       2 },
  { "SPEED_HIGH",       3 },

  { NULL, 0 }
};

static mp_obj_t gpio_obj_speed(mp_obj_t self_in, mp_obj_t speed_obj) {
    hal_gpio_obj_t *self = self_in;
    int speed = mp_obj_get_int(speed_obj) & 3;

    uint32_t port_index = (uint32_t) self->pin >> 4;
    GPIO_TypeDef *port_reg = (GPIO_TypeDef *) (GPIOA_BASE + (port_index << 10));

    int pin_shift = (self->pin & 0x0f) << 1;
    port_reg->OSPEEDR = (port_reg->OSPEEDR & ~(3 << pin_shift)) | (speed << pin_shift);

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_2(gpio_obj_speed_obj,  gpio_obj_speed);

void hal_target_init_Gpio_class(mp_obj_t gpio_class_type) {
    for (const hal_int_const *c = target_gpio_const; c->const_name != NULL; c++) {
        rt_store_attr(gpio_class_type, qstr_from_str(c->const_name), MP_OBJ_NEW_SMALL_INT(c->const_val));
    }
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("speed"), (mp_obj_t)&gpio_obj_speed_obj);
}
