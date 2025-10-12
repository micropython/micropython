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

#ifndef MICROPY_INCLUDED_ZEPHYR_DYNAMIC_PINCTRL_BUILDER_API_H
#define MICROPY_INCLUDED_ZEPHYR_DYNAMIC_PINCTRL_BUILDER_API_H

#include <pinctrl_soc.h>

/* The pinctrl builder must also define a Pinmux object type, very often it is uint32_t and that is provided for everyone.
 * machine_pin_pinmux_type represents a pin configuration when fetched/used in the DTS tree.
 * In contrast, pinctrl_soc_pin_t represents the pinctrl driver's input and a more complete pin state.
 *
 * For make_pinmux you want to copy what's in include/zephyr/dt-bindings/pinctrl/
 * For make_pinctr, get_pin, and get_function,
 * you want to copy what's in pinctrl_soc.h in the soc folder of your platform.
 */

typedef struct _dynamic_pinctrl_builder_api_t {
	/* Generate final struct/mask/etc that the pinctrl driver uses
	 * pinmux: dts pinmux value
	 * pu, pd: pull up and pull down on/off
	 * out/in: directions on/off
	 * schmitt: schmitt trigger on/off
	 * slew: slew rate, usually platform dependant
	 * drive: pin drive strength in uA
	 * Settings the platform doesnt support may be ignored.
	 */
	pinctrl_soc_pin_t (*make_pinctrl)(const mp_obj_t pinmux, const bool pu, const bool pd, const bool schmitt, const bool out, const bool in, const uint32_t slew, const uint32_t in_drive);
	#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL_GPIO_PREVIOUS_UPDATE) || defined(CONFIG_MICROPY_DYNAMIC_PINCTRL_GPIO_PREVIOUS_DIRECT)
	/* Set values inside pinctrl so it can be applied to disable what it was previously used for */
	pinctrl_soc_pin_t (*set_pinctrl_off)(const pinctrl_soc_pin_t pinctrl);
	#endif
	#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_PINMUX_TYPE_UINT32_T
	/* Create dts pinmux value from raw configuration.
	 * pin: The pin
	 * function: Platform specific, may be only a function ID, or more like signals or modes
	 */
	uint32_t (*make_pinmux)(const uint32_t pin, const uint32_t function);
	/* Extract pin from dts pinmux value */
	uint32_t (*get_pin)(const uint32_t pinmux);
	/* Extract function ID from dts pinmux value */
	uint32_t (*get_function)(const uint32_t pinmux);
	#endif
} dynamic_pinctrl_builder_api_t;

/* The API instance */
extern const dynamic_pinctrl_builder_api_t dynamic_pinctrl_builder_api;
/* The Pinmux type*/
extern const mp_obj_type_t machine_pin_pinmux_type;

#endif /* MICROPY_INCLUDED_ZEPHYR_DYNAMIC_PINCTRL_BUILDER_API_H */
