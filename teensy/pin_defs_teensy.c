#include <stdint.h>
#include <mk20dx128.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"

// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD,
// GPIO_MODE_AF_PP, GPIO_MODE_AF_OD, or GPIO_MODE_ANALOG.

uint32_t pin_get_mode(const pin_obj_t *pin) {
    if (pin->gpio == NULL) {
        // Analog only pin
        return GPIO_MODE_ANALOG;
    }
    volatile uint32_t *port_pcr = GPIO_PIN_TO_PORT_PCR(pin->gpio, pin->pin);
    uint32_t pcr = *port_pcr;
    uint32_t af = (pcr & PORT_PCR_MUX_MASK) >> 8;
    if (af == 0) {
        return GPIO_MODE_ANALOG;
    }
    if (af == 1) {
        if (pin->gpio->PDDR & (1 << pin->pin)) {
            if (pcr & PORT_PCR_ODE) {
                return GPIO_MODE_OUTPUT_OD;
            }
            return GPIO_MODE_OUTPUT_PP;
        }
        return GPIO_MODE_INPUT;
    }

    if (pcr & PORT_PCR_ODE) {
        return GPIO_MODE_AF_OD;
    }
    return GPIO_MODE_AF_PP;
}

// Returns the pin pullup/pulldown. The value returned by this macro should
// be one of GPIO_NOPULL, GPIO_PULLUP, or GPIO_PULLDOWN.

uint32_t pin_get_pull(const pin_obj_t *pin) {
    if (pin->gpio == NULL) {
        // Analog only pin
        return GPIO_NOPULL;
    }
    volatile uint32_t *port_pcr = GPIO_PIN_TO_PORT_PCR(pin->gpio, pin->pin); 

    uint32_t pcr = *port_pcr;
    uint32_t af = (pcr & PORT_PCR_MUX_MASK) >> 8;

    // pull is only valid for digital modes (hence the af > 0 test)

    if (af > 0 && (pcr & PORT_PCR_PE) != 0) {
        if (pcr & PORT_PCR_PS) {
            return GPIO_PULLUP;
        }
        return GPIO_PULLDOWN;
    }
    return GPIO_NOPULL;
}

// Returns the af (alternate function) index currently set for a pin.

uint32_t pin_get_af(const pin_obj_t *pin) {
    if (pin->gpio == NULL) {
        // Analog only pin
        return 0;
    }
    volatile uint32_t *port_pcr = GPIO_PIN_TO_PORT_PCR(pin->gpio, pin->pin);
    return (*port_pcr & PORT_PCR_MUX_MASK) >> 8;
}
