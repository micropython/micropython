#include "spark_wiring_usbserial.h"

#ifdef __cplusplus
extern "C" {
#endif

void usbserial_begin(unsigned long baudrate)
{
    Serial.begin(baudrate);
}

void usbserial_end(void)
{
    Serial.end();
}

int usbserial_available(void)
{
    return Serial.available();    
}

unsigned char usbserial_read(void)
{
    return Serial.read();    
}

int usbserial_peek(void)
{
    return Serial.peek();
}

void usbserial_flush(void)
{
    Serial.flush();
}

void usbserial_putc(unsigned char c)
{
    Serial.write(c);
}

#ifdef __cplusplus
}
#endif
