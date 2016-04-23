#ifndef _INTERRUPT_API_H
#define _INTERRUPT_API_H


#include "interrupts_hal.h"
#include "interrupts_irq.h"

bool Interrupt_attachInterrupt(uint16_t pin, void (*isr)(void), InterruptMode mode);
void Interrupt_detachInterrupt(uint16_t pin);
void Interrupt_enableAllInterrupts(void);
void Interrupt_disableAllInterrupts(void);
bool Interrupt_attachSystemInterrupt(hal_irq_t irq, void (*isr)(void));
bool Interrupt_detachSystemInterrupt(hal_irq_t irq);


#endif
