#include <stdint.h>


extern uint32_t global_tick;

// system tick is based on timer 1
void systick_timer_handler(void)
{
  uint32_t ms_tick = global_tick + 1;
  global_tick = ms_tick;
}

uint32_t mp_hal_ticks_ms(void)
{
  return global_tick;
}

uint32_t mp_hal_ticks_us(void)
{
  // disable timer 1 interrupt
  IEC0bits.T1IE = 0;
  uint32_t counter = TMR1;
  uint32_t milliseconds = global_tick;
}