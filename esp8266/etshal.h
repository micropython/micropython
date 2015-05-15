#ifndef _INCLUDED_ETSHAL_H_
#define _INCLUDED_ETSHAL_H_

void ets_isr_unmask();
void ets_install_putc1();
void ets_isr_attach();
void uart_div_modify();

#endif // _INCLUDED_ETSHAL_H_
