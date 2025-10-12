/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

static uint32_t mp_dynamic_pinctrl_make_pinmux(uint32_t pin, uint32_t function) {
    return RP2XXX_PINMUX(pin, function);
}

static uint32_t mp_dynamic_pinctrl_get_pin(uint32_t pinmux) {
    return RP2_GET_PIN_NUM(pinmux);
}

static uint32_t mp_dynamic_pinctrl_get_function(uint32_t pinmux) {
    return RP2_GET_PIN_ALT_FUNC(pinmux);
}

static pinctrl_soc_pin_t mp_dynamic_pinctrl_make_pinctrl(const mp_obj_t pinmux,
    const bool pu, const bool pd, const bool schmitt, const bool out, const bool in,
    const uint32_t slew, const uint32_t in_drive) {
    uint32_t mux = mp_obj_get_uint(pinmux);
    pinctrl_soc_pin_t pinctrl = {
        .pin_num = RP2_GET_PIN_NUM(mux),
        .alt_func = RP2_GET_PIN_ALT_FUNC(mux),
        .drive_strength = 0,
        .slew_rate = slew,
        .pullup = pu,
        .pulldown = pd,
        .input_enable = in,
        .schmitt_enable = schmitt,
        .oe_override = 0,
        .out_override = 0,
        .in_override = 0,
        .irq_override = 0,
    };
    if (in_drive < 2000) {
        pinctrl.drive_strength = 0;
    } else if (in_drive < 4000) {
        pinctrl.drive_strength = 1;
    } else if (in_drive < 8000) {
        pinctrl.drive_strength = 2;
    } else {
        pinctrl.drive_strength = 3;
    }
    return pinctrl;
}

pinctrl_soc_pin_t mp_dynamic_pinctrl_set_pinctrl_off(const pinctrl_soc_pin_t pinctrl) {
    pinctrl_soc_pin_t out_pinctrl = {0};

    out_pinctrl.pin_num = pinctrl.pin_num;
    out_pinctrl.alt_func = RP2_PINCTRL_GPIO_FUNC_SIO;
    return out_pinctrl;
}

const dynamic_pinctrl_builder_api_t dynamic_pinctrl_builder_api = {
    .make_pinmux = mp_dynamic_pinctrl_make_pinmux,
    .get_pin = mp_dynamic_pinctrl_get_pin,
    .get_function = mp_dynamic_pinctrl_get_function,
    .make_pinctrl = mp_dynamic_pinctrl_make_pinctrl,
    .set_pinctrl_off = mp_dynamic_pinctrl_set_pinctrl_off,
};
