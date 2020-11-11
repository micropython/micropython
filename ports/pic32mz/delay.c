#include "delay.h"
#include "sysclk.h"

// @brief Read current cp0 timer, used to create delays
static uint32_t readcoretimer(void)
{
  volatile uint32_t timer;
  
  asm volatile("mfc0   %0, $9" : "=r"(timer));
  
  return timer;
}

// @brief Set current cp0 timer count
static void setcoretime(volatile uint32_t count)
{
  asm volatile("mtc0    %0, $9" : :"r"(count));
}

void delay_us(uint32_t us)
{
  us *= SYS_FREQ / 1000000 / 2; //Core timer updates every 2 ticks
  setcoretime(0);
  while (us > readcoretimer());
}

void delay_ms(uint32_t ms)
{
  delay_us(ms * 1000);
}