#include "unity.h"
#include "Types.h"
#include "UsartPutChar.h"
#include "MockUsartTransmitBufferStatus.h"

AT91S_USART Usart0Peripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void testPutCharShouldWriteDesiredCharacterToUsartTransmitBuffer(void)
{
  AT91C_BASE_US0->US_THR = 0;
  
  Usart_ReadyToTransmit_ExpectAndReturn(TRUE);
  Usart_PutChar('x');
  TEST_ASSERT_EQUAL('x', AT91C_BASE_US0->US_THR);
  
  Usart_ReadyToTransmit_ExpectAndReturn(TRUE);
  Usart_PutChar('1');
  TEST_ASSERT_EQUAL('1', AT91C_BASE_US0->US_THR);
  
  Usart_ReadyToTransmit_ExpectAndReturn(TRUE);
  Usart_PutChar(':');
  TEST_ASSERT_EQUAL(':', AT91C_BASE_US0->US_THR);
}

void testPutCharShouldWaitUntilReadyToTransmitBeforeLoadingTransmitBufffer(void)
{
  AT91C_BASE_US0->US_THR = 0;
  
  Usart_ReadyToTransmit_ExpectAndReturn(FALSE);
  Usart_ReadyToTransmit_ExpectAndReturn(FALSE);
  Usart_ReadyToTransmit_ExpectAndReturn(FALSE);
  Usart_ReadyToTransmit_ExpectAndReturn(TRUE);
  Usart_PutChar('x');
  TEST_ASSERT_EQUAL('x', AT91C_BASE_US0->US_THR);
}
