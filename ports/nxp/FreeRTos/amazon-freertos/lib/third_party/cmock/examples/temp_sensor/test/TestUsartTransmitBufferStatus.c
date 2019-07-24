#include "unity.h"
#include "Types.h"
#include "UsartTransmitBufferStatus.h"

AT91S_USART Usart0Peripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void testReadyToTransmitShouldReturnStatusPerTransmitBufferReadyStatus(void)
{
  AT91C_BASE_US0->US_CSR = 0;
  TEST_ASSERT(!Usart_ReadyToTransmit());
  
  AT91C_BASE_US0->US_CSR = AT91C_US_TXRDY;
  TEST_ASSERT(Usart_ReadyToTransmit());
}
