/* Dynamic Pinctrl builder functions file.
 * for mp_dynamic_pinctrl_make_pinmux you want to copy what's in include/zephyr/dt-bindings/pinctrl/
 * for mp_dynamic_pinctrl_make_pinctrl_soc_pin_t, mp_dynamic_pinctrl_get_pin,
 * mp_dynamic_pinctrl_get_function, you want to copy what's in
 * pinctrl_soc.h in the soc folder of your platform
 */
static inline int mp_dynamic_pinctrl_make_pinmux(int pin, int function) {
    return RP2XXX_PINMUX(pin, function);
}

static inline int mp_dynamic_pinctrl_get_pin(uint32_t pinmux) {
    return RP2_GET_PIN_NUM(pinmux);
}

static inline int mp_dynamic_pinctrl_get_function(uint32_t pinmux) {
    return RP2_GET_PIN_ALT_FUNC(pinmux);
}

static inline pinctrl_soc_pin_t mp_dynamic_pinctrl_make_pinctrl_soc_pin_t(uint32_t pinmux,
    bool pu, bool pd, bool schmitt, bool out, bool in, uint32_t slew, uint32_t drive) {
    pinctrl_soc_pin_t pinctrl = {
        .pin_num = RP2_GET_PIN_NUM(pinmux),
        .alt_func = RP2_GET_PIN_ALT_FUNC(pinmux),
        .drive_strength = drive,
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
    return pinctrl;
}
