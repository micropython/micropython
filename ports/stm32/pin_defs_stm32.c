#include "py/obj.h"
#include "pin.h"

// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD,
// GPIO_MODE_AF_PP, GPIO_MODE_AF_OD, or GPIO_MODE_ANALOG.

uint32_t pin_get_mode(const machine_pin_obj_t *pin) {
    GPIO_TypeDef *gpio = pin->gpio;
    uint32_t mode = (gpio->MODER >> (pin->pin * 2)) & 3;
    if (mode == GPIO_MODE_OUTPUT_PP || mode == GPIO_MODE_AF_PP) {
        if (gpio->OTYPER & pin->pin_mask) {
            mode |= 1 << 4;     // Converts from xxx_PP to xxx_OD
        }
    }
    return mode;
}

// Returns the pin pullup/pulldown. The value returned by this macro should
// be one of GPIO_NOPULL, GPIO_PULLUP, or GPIO_PULLDOWN.

uint32_t pin_get_pull(const machine_pin_obj_t *pin) {
    return (pin->gpio->PUPDR >> (pin->pin * 2)) & 3;
}

// Returns the af (alternate function) index currently set for a pin.

uint32_t pin_get_af(const machine_pin_obj_t *pin) {
    return (pin->gpio->AFR[pin->pin >> 3] >> ((pin->pin & 7) * 4)) & 0xf;
}
