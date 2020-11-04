#include "board.h"
#include "sysclk.h"
#include "interrupt.h"

#include <xc.h>

void mpu_init(void)
{
  sysclk_init();
  interrupt_init();
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