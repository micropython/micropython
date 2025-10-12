/* Dynamic Pinctrl builder functions file.
 * for mp_dynamic_pinctrl_make_pinmux you want to copy what's in include/zephyr/dt-bindings/pinctrl/
 * for mp_dynamic_pinctrl_make_pinctrl_soc_pin_t, mp_dynamic_pinctrl_get_pin,
 * mp_dynamic_pinctrl_get_function, you want to copy what's in
 * pinctrl_soc.h in the soc folder of your platform
 */

/* What mp_dynamic_pinctrl_get_function would return for GPIO */
#define MICROPY_DYNAMIC_PINCTRL_FUNCTION_GPIO 11

/* Provided drive values */
#define MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE (1)
#define MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_0 0
#define MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_1 1
#define MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_2 2

/* The input for this in arbitrary mode is the same
 * as raw pinmux from bflb-common-pinctrl.h for function.
 */
static inline int mp_dynamic_pinctrl_make_pinmux(int pin, int function) {
    return ((pin & BFLB_PINMUX_PIN_MASK) << BFLB_PINMUX_PIN_POS) | function;
}

static inline int mp_dynamic_pinctrl_get_pin(uint32_t pinmux) {
    return BFLB_PINMUX_GET_PIN(pinmux);
}

static inline int mp_dynamic_pinctrl_get_function(uint32_t pinmux) {
    return BFLB_PINMUX_GET_FUN(pinmux);
}

static inline pinctrl_soc_pin_t mp_dynamic_pinctrl_make_pinctrl_soc_pin_t(uint32_t pinmux,
    bool pu, bool pd, bool schmitt, bool out, bool in, uint32_t slew, uint32_t drive) {
    uint32_t pinctrl = pinmux;
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
    pinctrl |= (drive & BFLB_PINMUX_DRIVER_STRENGTH_MASK) << BFLB_PINMUX_DRIVER_STRENGTH_POS;
    return pinctrl;
}
