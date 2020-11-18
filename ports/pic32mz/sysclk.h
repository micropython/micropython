#ifndef PIC32_INCLUDED_SYSCLK_H
#define PIC32_INCLUDED_SYSCLK_H

#include <stdint.h>

#define PERPHERAL1_CLK_DIV (1)
#define PERPHERAL2_CLK_DIV (1)
#define PERPHERAL3_CLK_DIV (4)
#define PERPHERAL4_CLK_DIV (1)
#define PERPHERAL5_CLK_DIV (1)
#define PERPHERAL7_CLK_DIV (0)
#define PERPHERAL8_CLK_DIV (1)

#define SYS_FREQ 200000000 // 200MHz

uint32_t sysclk_sysfreq_get(void);
uint32_t sysclk_timerfreq_get(void);
uint32_t sysclk_uartfreq_get(void);
void sysclk_init(void);

#endif // PIC32_INCLUDED_SYSCLK_H
