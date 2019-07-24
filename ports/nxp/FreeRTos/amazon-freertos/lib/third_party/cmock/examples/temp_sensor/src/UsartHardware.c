#include "Types.h"
#include "UsartHardware.h"
#include "UsartConfigurator.h"
#include "UsartPutChar.h"

void UsartHardware_Init(uint8 baudRateRegisterSetting)
{
  Usart_ConfigureUsartIO();
  Usart_EnablePeripheralClock();
  Usart_Reset();
  Usart_ConfigureMode();
  Usart_SetBaudRateRegister(baudRateRegisterSetting);
  Usart_Enable();
}

void UsartHardware_TransmitString(char* data)
{
  while(*data != NULL)
  {
    Usart_PutChar(*data++);
  }
}
