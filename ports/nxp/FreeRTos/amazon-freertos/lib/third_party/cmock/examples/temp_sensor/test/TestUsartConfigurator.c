#include "unity.h"
#include "Types.h"
#include "UsartConfigurator.h"

AT91S_PIO PioAPeripheral;
AT91S_PMC PmcPeripheral;
AT91S_USART Usart0Peripheral;

void setUp(void)
{
}

void tearDown(void)
{
}

void testConfigureUsartIOShouldConfigureUsartTxPinfForPeripheralIO(void)
{
  AT91C_BASE_PIOA->PIO_ASR = 0;
  AT91C_BASE_PIOA->PIO_BSR = 0xffffffff;
  AT91C_BASE_PIOA->PIO_PDR = 0;
  Usart_ConfigureUsartIO();
  TEST_ASSERT_EQUAL(USART0_TX_PIN, AT91C_BASE_PIOA->PIO_ASR);
  TEST_ASSERT_EQUAL(0, AT91C_BASE_PIOA->PIO_BSR);
  TEST_ASSERT_EQUAL(USART0_TX_PIN, AT91C_BASE_PIOA->PIO_PDR);
}

void testEnablePeripheralClockShouldEnableClockToUsartPeripheral(void)
{
  AT91C_BASE_PMC->PMC_PCER = 0;
  Usart_EnablePeripheralClock();
  TEST_ASSERT_EQUAL(((uint32)1) << USART0_CLOCK_ENABLE, AT91C_BASE_PMC->PMC_PCER);
}

void testResetShouldDisableAllUsartInterrupts(void)
{
  AT91C_BASE_US0->US_IDR = 0;
  Usart_Reset();
  TEST_ASSERT_EQUAL(0xffffffff, AT91C_BASE_US0->US_IDR);
}

void testResetShouldResetUsartTransmitterAndReceiver(void)
{
  AT91C_BASE_US0->US_CR = 0;
  Usart_Reset();
  TEST_ASSERT_EQUAL(AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS, AT91C_BASE_US0->US_CR);
}

void testConfigureModeShouldSetUsartModeToAsynchronous(void)
{
  uint32 asyncMode =  (AT91C_US_USMODE_NORMAL |
                        AT91C_US_NBSTOP_1_BIT |
                        AT91C_US_PAR_NONE |
                        AT91C_US_CHRL_8_BITS |
                        AT91C_US_CLKS_CLOCK);

  AT91C_BASE_US0->US_MR = ~asyncMode;
  Usart_ConfigureMode();
  TEST_ASSERT_EQUAL(asyncMode, AT91C_BASE_US0->US_MR);
}

void testSetBaudRateRegisterShouldSetUsartBaudRateRegisterToValuePassedAsParameter(void)
{
  AT91C_BASE_US0->US_BRGR = 0;
  Usart_SetBaudRateRegister(3);
  TEST_ASSERT_EQUAL(3, AT91C_BASE_US0->US_BRGR);
  Usart_SetBaudRateRegister(251);
  TEST_ASSERT_EQUAL(251, AT91C_BASE_US0->US_BRGR);
}


void testEnableShouldEnableUsart0Transmitter(void)
{
  AT91C_BASE_US0->US_CR = 0;
  Usart_Enable();
  TEST_ASSERT_EQUAL(AT91C_US_TXEN, AT91C_BASE_US0->US_CR);
}
