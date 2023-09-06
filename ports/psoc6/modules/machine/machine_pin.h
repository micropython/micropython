#ifndef MICROPY_INCLUDED_MACHINE_PIN_H
#define MICROPY_INCLUDED_MACHINE_PIN_H

#include <stdint.h>

uint8_t gpio_get_value(uint32_t pin);
void gpio_set_value(uint32_t pin);
void gpio_clear_value(uint32_t pin);

#endif // MICROPY_INCLUDED_MACHINE_PIN_H
