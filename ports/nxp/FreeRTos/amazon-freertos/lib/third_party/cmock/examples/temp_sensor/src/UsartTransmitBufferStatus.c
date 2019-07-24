#include "Types.h"
#include "UsartTransmitBufferStatus.h"

bool Usart_ReadyToTransmit(void)
{
  return (AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY) > 0;
}
