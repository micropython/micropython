#ifndef _INCLUDED_ETSHAL_H_
#define _INCLUDED_ETSHAL_H_

#include <os_type.h>

void ets_isr_unmask();
void ets_install_putc1();
void ets_isr_attach();
void uart_div_modify();

void ets_timer_arm_new(os_timer_t *tim, uint32_t millis, bool repeat, bool is_milli_timer);
void ets_timer_setfn(os_timer_t *tim, ETSTimerFunc callback, void *cb_data);
void ets_timer_disarm(os_timer_t *tim);

#endif // _INCLUDED_ETSHAL_H_
