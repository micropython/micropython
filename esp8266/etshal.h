#ifndef _INCLUDED_ETSHAL_H_
#define _INCLUDED_ETSHAL_H_

#include <os_type.h>

// see http://esp8266-re.foogod.com/wiki/Random_Number_Generator
#define WDEV_HWRNG ((volatile uint32_t*)0x3ff20e44)

void ets_delay_us();
void ets_intr_lock(void);
void ets_intr_unlock(void);
void ets_isr_mask(uint32_t mask);
void ets_isr_unmask(uint32_t mask);
void ets_isr_attach(int irq_no, void (*handler)(void *), void *arg);
void ets_install_putc1();
void uart_div_modify();
void ets_set_idle_cb(void (*handler)(void *), void *arg);

void ets_timer_arm_new(os_timer_t *tim, uint32_t millis, bool repeat, bool is_milli_timer);
void ets_timer_setfn(os_timer_t *tim, ETSTimerFunc callback, void *cb_data);
void ets_timer_disarm(os_timer_t *tim);

// These prototypes are for recent SDKs with "malloc tracking"
void *pvPortMalloc(unsigned sz, const char *fname, int line);
void vPortFree(void *p, const char *fname, int line);

#endif // _INCLUDED_ETSHAL_H_
