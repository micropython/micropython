/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _PYBGPIO_H_
#define _PYBGPIO_H_

#if defined (MICROPY_PY_GPIO) && MICROPY_PY_GPIO

#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
typedef struct
{
    mp_obj_base_t base;
    GPIO_Type* port;
    uint32_t pin;
}gpio_obj_t;
mp_obj_t pyb_gpio_Toggle(mp_obj_t self_in);
#else
typedef struct 
{
    mp_obj_base_t base;
    hal_gpio_pin_config_t config;
    hal_gpio_handle_t handle;
    mp_obj_t callback;
}gpio_obj_t;
#endif /* MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU */

extern const mp_obj_type_t pyb_gpio_type;

mp_obj_t pyb_gpio_SetOutput(mp_obj_t self_in, mp_obj_t pinValue);
mp_obj_t pyb_gpio_init_helper(gpio_obj_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
#endif /* MICROPY_PY_GPIO */
#endif /* _PYBGPIO_H_ */