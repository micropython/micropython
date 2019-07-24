#include "Types.h"
#include "UsartConfigurator.h"

void Usart_ConfigureUsartIO(void)
{
  AT91C_BASE_PIOA->PIO_ASR = USART0_TX_PIN;
  AT91C_BASE_PIOA->PIO_BSR = 0;
  AT91C_BASE_PIOA->PIO_PDR = USART0_TX_PIN;
}

void Usart_EnablePeripheralClock(void)
{
  AT91C_BASE_PMC->PMC_PCER = ((uint32)1) << USART0_CLOCK_ENABLE;
}

void Usart_Reset(void)
{
  AT91C_BASE_US0->US_IDR = 0xffffffff;
  AT91C_BASE_US0->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
}

void Usart_ConfigureMode(void)
{
  AT91C_BASE_US0->US_MR = AT91C_US_USMODE_NORMAL |
                          AT91C_US_NBSTOP_1_BIT |
                          AT91C_US_PAR_NONE |
                          AT91C_US_CHRL_8_BITS |
                          AT91C_US_CLKS_CLOCK;
}

void Usart_SetBaudRateRegister(uint8 baudRateRegisterSetting)
{
  AT91C_BASE_US0->US_BRGR = baudRateRegisterSetting;
}

void Usart_Enable(void)
{
  AT91C_BASE_US0->US_CR = AT91C_US_TXEN;
}
