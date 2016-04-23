
#ifndef __SPARK_WIRING_HCISERIAL_H
#define __SPARK_WIRING_HCISERIAL_H

#if PLATFORM_ID == 88 // Duo

#include "spark_wiring_stream.h"
#include "hci_usart_hal.h"
#include "spark_wiring_platform.h"
#include "spark_wiring_usartserial.h"


class HCIUsart
{
private:
    HAL_HCI_USART_Serial _serial;
public:
    HCIUsart(HAL_HCI_USART_Serial serial, HCI_USART_Ring_Buffer *rx_buffer, HCI_USART_Ring_Buffer *tx_buffer);

    void registerReceiveHandler(ReceiveHandler_t handler);
    int  downloadFirmware(void);

    void begin(unsigned long baudrate);
    void end(void);

    int available(void);
    int read(void);
    size_t write(uint8_t dat);
    size_t write(const uint8_t *buf, uint16_t len);

    void restartTransmit(void);
    void setRTS(uint8_t value);
    uint8_t checkCTS(void);
};

#if Wiring_Serial6
extern HCIUsart BTUsart;
#endif

#endif

#endif
