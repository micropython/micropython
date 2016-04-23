#include "spark_wiring_usartserial.h"

#ifdef __cplusplus
extern "C" {
#endif

void usartserial1_begin(unsigned long baudrate)
{
    Serial1.begin(baudrate);
}

void usartserial1_end(void)
{
    Serial1.end();
}

int usartserial1_availableForWrite(void)
{
    return Serial1.availableForWrite();
}

int usartserial1_available(void)
{
    return Serial1.available();
}

void usartserial1_halfduplex(bool enable)
{
    Serial1.halfduplex(enable);
}

int usartserial1_peek(void)
{
    return Serial1.peek();
}

unsigned char usartserial1_read(void)
{
    return Serial1.read();
}

void usartserial1_flush(void)
{
    return Serial1.flush();
}

void usartserial1_putc(unsigned char c)
{
    Serial1.write(c);
}

bool usartserial1_isenable(void)
{
    return Serial1.isEnabled();
}



void usartserial2_begin(unsigned long baudrate)
{
    Serial2.begin(baudrate);
}

void usartserial2_end(void)
{
    Serial2.end();
}

int usartserial2_availableForWrite(void)
{
    return Serial2.availableForWrite();
}

int usartserial2_available(void)
{
    return Serial2.available();
}

void usartserial2_halfduplex(bool enable)
{
    Serial2.halfduplex(enable);
}

int usartserial2_peek(void)
{
    return Serial2.peek();
}

unsigned char usartserial2_read(void)
{
    return Serial2.read();
}

void usartserial2_flush(void)
{
    return Serial2.flush();
}

void usartserial2_putc(unsigned char c)
{
    Serial2.write(c);
}

bool usartserial2_isenable(void)
{
    return Serial2.isEnabled();
}

#ifdef __cplusplus
}
#endif
