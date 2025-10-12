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

static uint32_t mp_dynamic_pinctrl_make_pinmux(const uint32_t pin, const uint32_t function) {
    return ((pin & BFLB_PINMUX_PIN_MASK) << BFLB_PINMUX_PIN_POS) | function;
}

static uint32_t mp_dynamic_pinctrl_get_pin(const uint32_t pinmux) {
    return BFLB_PINMUX_GET_PIN(pinmux);
}

static uint32_t mp_dynamic_pinctrl_get_function(const uint32_t pinmux) {
    return BFLB_PINMUX_GET_FUN(pinmux);
}

static pinctrl_soc_pin_t mp_dynamic_pinctrl_make_pinctrl(const mp_obj_t pinmux,
    const bool pu, const bool pd, const bool schmitt, const bool out, const bool in,
    const uint32_t slew, const uint32_t in_drive) {
    uint32_t pinctrl = mp_obj_get_uint(pinmux);
    uint32_t drive = 0;
    if (pu) {
        pinctrl |= 0x1U << BFLB_PINMUX_PULL_UP_POS;
    } else if (pd) {
        pinctrl |= 0x1U << BFLB_PINMUX_PULL_DOWN_POS;
    }
    if (schmitt) {
        pinctrl |= 0x1U << BFLB_PINMUX_SMT_POS;
    }
    if (out && !in) {
        pinctrl |= 0x1U << BFLB_PINMUX_OE_POS;
    }

    #if defined(CONFIG_SOC_SERIES_BL60X) || defined(CONFIG_SOC_SERIES_BL70X)
    if (in_drive < 4000) {
        drive = 0;
    } else if (in_drive < 8000) {
        drive = 1;
    } else if (in_drive < 12000) {
        drive = 2;
    } else {
        drive = 3;
    }
    #elif defined(CONFIG_SOC_SERIES_BL70XL)
    if (in_drive < 3000) {
        drive = 0;
    } else if (in_drive < 9000) {
        drive = 1;
    } else if (in_drive < 16000) {
        drive = 2;
    } else {
        drive = 3;
    }
    #elif defined(CONFIG_SOC_SERIES_BL61X)
    if (in_drive < 3000) {
        drive = 0;
    } else if (in_drive < 10000) {
        drive = 1;
    } else if (in_drive < 18000) {
        drive = 2;
    } else {
        drive = 3;
    }
    #elif defined(CONFIG_SOC_SERIES_BL616CL)
    if (in_drive < 6000) {
        drive = 0;
    } else if (in_drive < 12000) {
        drive = 1;
    } else if (in_drive < 20000) {
        drive = 2;
    } else {
        drive = 3;
    }
    #else
    /* BL808, unspecified */
    if (in_drive < 5000) {
        drive = 0;
    } else if (in_drive < 10000) {
        drive = 1;
    } else if (in_drive < 20000) {
        drive = 2;
    } else {
        drive = 3;
    }
    #endif

    pinctrl |= (drive & BFLB_PINMUX_DRIVER_STRENGTH_MASK) << BFLB_PINMUX_DRIVER_STRENGTH_POS;
    return pinctrl;
}

pinctrl_soc_pin_t mp_dynamic_pinctrl_set_pinctrl_off(const pinctrl_soc_pin_t pinctrl) {
    pinctrl_soc_pin_t pin = BFLB_PINMUX_GET_PIN(pinctrl);
    return ((pin & BFLB_PINMUX_PIN_MASK) << BFLB_PINMUX_PIN_POS)
           | (BFLB_PINMUX_FUN_INST_gpio << BFLB_PINMUX_FUN_INST_POS);
}

const dynamic_pinctrl_builder_api_t dynamic_pinctrl_builder_api = {
    .make_pinmux = mp_dynamic_pinctrl_make_pinmux,
    .get_pin = mp_dynamic_pinctrl_get_pin,
    .get_function = mp_dynamic_pinctrl_get_function,
    .make_pinctrl = mp_dynamic_pinctrl_make_pinctrl,
    .set_pinctrl_off = mp_dynamic_pinctrl_set_pinctrl_off,
};
