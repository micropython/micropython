#ifndef _INCLUDED_ETSHAL_H_
#define _INCLUDED_ETSHAL_H_

void ets_isr_unmask();
void ets_install_putc1();
void ets_isr_attach();
void uart_div_modify();
uint32_t ets_get_cpu_frequency();

#endif // _INCLUDED_ETSHAL_H_
