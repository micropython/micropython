#include "Types.h"
#include "TimerConfigurator.h"
#include "TimerInterruptConfigurator.h"

void Timer_EnablePeripheralClocks(void)
{
  AT91C_BASE_PMC->PMC_PCER = TIMER0_CLOCK_ENABLE | PIOB_CLOCK_ENABLE;
}

void Timer_Reset(void)
{
  uint32 dummy;
  AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS;
  AT91C_BASE_TC0->TC_IDR = 0xffffffff;
  dummy = AT91C_BASE_TC0->TC_SR;
  dummy = dummy;
}

void Timer_ConfigureMode(void)
{
  AT91C_BASE_TC0->TC_CMR = 0x000CC004; // ACPC=toggle TIOA on RC compare; mode=WAVE; WAVE_SEL=UP w/auto-trigger on RC compare; clock=MCK/1024
}

void Timer_ConfigurePeriod(void)
{
  AT91C_BASE_TC0->TC_RC = 469; // 10ms period for timer clock source of MCK/1024 with MCK=48054857
}

void Timer_EnableOutputPin(void)
{
  AT91C_BASE_PIOB->PIO_PDR = TIOA0_PIN_MASK;
}

void Timer_Enable(void)
{
  AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKEN;
}

void Timer_ConfigureInterruptHandler(void)
{
  Timer_DisableInterrupt();
  Timer_ResetSystemTime();
  Timer_ConfigureInterrupt();
  Timer_EnableInterrupt();
}

void Timer_Start(void)
{
  AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG;
}

