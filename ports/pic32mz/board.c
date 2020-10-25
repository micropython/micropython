#include "board.h"
#include <xc.h>

/**
 * @brief Read current cp0 timer, used to create delays
 */
static uint32_t readcoretimer(void)
{
  volatile uint32_t timer;
  
  asm volatile("mfc0   %0, $9" : "=r"(timer));
  
  return timer;
}

/**
 * @brief Set current cp0 timer count
 */
static void setcoretime(volatile uint32_t count)
{
  asm volatile("mtc0    %0, $9" : :"r"(count));
}

/*
 * @brief generate a delay
 */
void _delay_us(unsigned int us)
{
  us *= SYS_FREQ / 1000000 / 2; //Core timer updates every 2 ticks
  setcoretime(0);

  while (us > readcoretimer());
}

/* 
 * @brief Set the clocks for different peripherals, initiate cache
 */
void mpu_init(void)
{
  unsigned int cp0;

  //Unlock sequence
  asm volatile("di"); // disable all interrupts
  SYSKEY = 0xAA996655;
  SYSKEY = 0x556699AA;

  //PB1DIV
  //Peripheral Bus 1 cannot be turned off, so there's no need to turn it on
  PB1DIVbits.PBDIV = 1; //Peripheral Bus 1 Clock Divisor Control (PBCLK1 is SYSCLK / 2)
  
  //PB2DIV
  PB2DIVbits.PBDIV = 1;
  PB2DIVbits.ON = 1; 

  PB3DIVbits.PBDIV = 4;
  PB3DIVbits.ON = 1; 

  PB4DIVbits.PBDIV = 1; 
  PB4DIVbits.ON = 1; 

  PB5DIVbits.PBDIV = 1; 
  PB5DIVbits.ON = 1; 

  PB7DIVbits.PBDIV = 0; 
  PB7DIVbits.ON = 1; 

  PB8DIVbits.PBDIV = 1; 
  PB8DIVbits.ON = 1; 

  // PRECON - Set up prefetch
  PRECONbits.PFMSECEN = 0; // Flash SEC Interrrupt Enable ( Do not generate interrupt when PFMSECbit is set)
  PRECONbits.PREFEN = 0b11; // Predictive prefech Enable (for any address)
  PRECONbits.PFMWS  = 0b010; // PFM Access Time Defined in Terms of SYSCLK Wait States

  // Set up caching -system control coprocessor- register number 16
  cp0 = _mfc0(16, 0);
  cp0 &= ~0x07;
  cp0 |= 0b011; // K0 = Cacheable, non-coherent, write-back, write-allocate
  _mtc0(16, 0, cp0);

  // Lock Sequence
  SYSKEY = 0x33333333;
  asm volatile("ei"); // enable interrupts
}

void led_init(void)
{
  ANSELB = 0; // PIC starts all pins as analog, needs to be deactivated to be used
  TRISBCLR = 1 << 13; //set bit 13 port B to output
  TRISBCLR = 1 << 12; //set bit 12 port B to output
}

void led_state(int led, int state)
{
  if (state == 0)
  {
    LATBSET = 1 << led;
  }
  else
  {
    LATBCLR = 1 << led;
  }
}
void led_toggle(int led)
{
  LATBINV = 1 << led;
}

void uart_init(void)
{
  int pb_uart_clk = SYS_FREQ / 2;
  int baud_rate = 19200;

  // Set rs485 bit to output
  // this pin is special for this board and not for all uart init
  ANSELA = 0;
  TRISACLR = 1 << RS_PIN;
  LATASET = 1 << RS_PIN; // clear rspin for reading
  
  // Set up the UART 4
  // RX to RPF2
  U4RXR = 0b0010;
  TRISBbits.TRISB14 = 1; //set its IO to input

  // TX to RPF3
  RPF12R = 0b0010;

  U4MODE = 0; // Set UART 5 off, prior to setting it up.
  U4MODEbits.BRGH = 0; // standard speed mode, included in the previsous statmem

  // formulat from datasheet.
  U4BRG = pb_uart_clk / ( 16  * baud_rate) - 1; 
  U4STA = 0; // disable Tx Rx, clear all flags
  U4STAbits.UTXEN = 1;
  U4STAbits.URXEN = 1;

  U4MODEbits.PDSEL = 0; // controls how many data bits parity,... we chose default
  U4MODEbits.STSEL = 0; // stop bit we chose 1
  
  U4MODEbits.ON = 1; // Turn ON

  print_str("Hello UART \n");
}

int uart_rx_any(void)
{
  return (int)U4STAbits.URXDA & 0x01;
}

int uart_rx_char(void)
{
  return (int)U4RXREG;
}

void uart_tx_char(int c)
{
  while (U4STAbits.UTXBF);
  U4TXREG = (char)c;
}

void print_str(char *s)
{
  while ( *s != '\0')
  {
    uart_tx_char((int)*s++);
  }
}