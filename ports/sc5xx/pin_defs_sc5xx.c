#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"

// Returns the pin mode.
uint32_t pin_get_mode(const pin_obj_t *pin) {
    pin_gpio_t *gpio = pin -> gpio;
    uint32_t pin_number = pin -> pin;
    uint32_t pin_mask = (uint32_t)(1 << pin_number);
    if ((gpio -> FER) & pin_mask)
        return GPIO_MODE_AF_PP;
    else if ((gpio -> DIR) & pin_mask)
        return GPIO_MODE_OUTPUT_PP;
    else
        return GPIO_MODE_INPUT;
}

// Returns the af (alternate function) index currently set for a pin.
uint32_t pin_get_af(const pin_obj_t *pin) {
    pin_gpio_t *gpio = pin -> gpio;
    uint32_t pin_number = pin -> pin;
    uint32_t pin_mask = (uint32_t)(1 << pin_number);

    if ((gpio -> FER) & pin_mask)
        return ((gpio -> MUX) >> (pin_number * 2)) & 0x3;
    else
        return 0;
}

