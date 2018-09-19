#ifndef MICROPY_INCLUDED_ESP8266_SIGMADELTA_H
#define MICROPY_INCLUDED_ESP8266_SIGMADELTA_H

#include <stdbool.h>
#include <stdint.h>

#define GPIO_SIGMA_DELTA  0x60000368
#define SIGMA_DELTA_TARGET     0
#define SIGMA_DELTA_PRESCALAR  8
#define SIGMA_DELTA_ENABLE    16

void sigmadelta_stop(void);
void sigmadelta_start(void);

void sigmadelta_setup(uint8_t prescaler, uint8_t target);

void sigmadelta_detach(uint8_t pin_id);
void sigmadelta_attach(uint8_t pin_id);

#endif // MICROPY_INCLUDED_ESP8266_SIGMADELTA_H
