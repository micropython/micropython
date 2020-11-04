#include "sysclk.h"
#include <xc.h>

uint32_t sysclk_sysfreq_get(void)
{
  return (SYS_FREQ);
}

uint32_t sysclk_timerfreq_get(void)
{
  return (SYS_FREQ / (PERPHERAL3_CLK_DIV + 1));
}

uint32_t sysclk_uartfreq_get(void)
{
  return (SYS_FREQ / (PERPHERAL2_CLK_DIV + 1));
}

// Set the clocks for system and peripherals
void sysclk_init(void)
{
  uint32_t cp0;

  asm volatile("di");

  //Unlock sequence
  SYSKEY = 0xAA996655;
  SYSKEY = 0x556699AA;

  PB1DIVbits.PBDIV = PERPHERAL1_CLK_DIV; 
  
  PB2DIVbits.PBDIV = PERPHERAL2_CLK_DIV; 
  PB2DIVbits.ON = 1; 

  PB3DIVbits.PBDIV = PERPHERAL3_CLK_DIV;
  PB3DIVbits.ON = 1; 

  PB4DIVbits.PBDIV = PERPHERAL4_CLK_DIV; 
  PB4DIVbits.ON = 1; 

  PB5DIVbits.PBDIV = PERPHERAL5_CLK_DIV; 
  PB5DIVbits.ON = 1; 

  PB7DIVbits.PBDIV = PERPHERAL7_CLK_DIV; 
  PB7DIVbits.ON = 1; 

  PB8DIVbits.PBDIV = PERPHERAL8_CLK_DIV; 
  PB8DIVbits.ON = 1; 

  // PRECON - Set up prefetch
  PRECONbits.PFMSECEN = 0; // Do not generate interrupt when PFMSECbit is set
  PRECONbits.PREFEN = 0b11; // Predictive prefech Enable (for any address)
  PRECONbits.PFMWS  = 0b010; // PFM Access Time Defined in Terms of SYSCLK Wait States

  // Set up caching
  cp0 = _mfc0(16, 0);
  cp0 &= ~0x07;
  cp0 |= 0b011; // K0 = Cacheable, non-coherent, write-back, write-allocate
  _mtc0(16, 0, cp0);

  // Lock Sequence
  SYSKEY = 0x33333333;

  asm volatile("ei");
}
