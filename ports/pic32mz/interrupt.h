#ifndef MICROPY_INCLUDED_PIC32BIT_INTERRUPT_H
#define MICROPY_INCLUDED_PIC32BIT_INTERRUPT_H

void interrupt_init(void);
uint32_t interrupt_tick_get(void);

#endif