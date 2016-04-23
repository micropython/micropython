#include "spark_wiring_interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif


bool Interrupt_attachInterrupt(uint16_t pin, void (*isr)(void), InterruptMode mode)
{
    return attachInterrupt(pin, isr, mode);
}

void Interrupt_detachInterrupt(uint16_t pin)
{
    detachInterrupt(pin);
}

void Interrupt_enableAllInterrupts(void)
{
    interrupts();
}

void Interrupt_disableAllInterrupts(void)
{
    noInterrupts();
}

bool Interrupt_attachSystemInterrupt(hal_irq_t irq, void (*isr)(void))
{
    wiring_interrupt_handler_t fn = wiring_interrupt_handler_t(isr);
    return attachSystemInterrupt(irq, fn);
}

bool Interrupt_detachSystemInterrupt(hal_irq_t irq)
{
    return detachSystemInterrupt(irq);
}


#ifdef __cplusplus
}
#endif
