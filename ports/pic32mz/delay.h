#ifndef MICROPY_INCLUDED_PIC32BIT_DELAY_H
#define MICROPY_INCLUDED_PIC32BIT_DELAY_H

#include <stdint.h>

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif // MICROPY_INCLUDED_PIC32BIT_DELAY_H