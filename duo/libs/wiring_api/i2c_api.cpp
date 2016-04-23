#include "spark_wiring_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


void i2c_setSpeed(uint32_t clockSpeed)
{
    Wire.setSpeed(clockSpeed);
}

void i2c_enableDMAMode(bool enableDMAMode)
{
    Wire.enableDMAMode(enableDMAMode);
}

void i2c_stretchClock(bool stretch)
{
    Wire.stretchClock(stretch);
}

void i2c_begin(void)
{
    Wire.begin();
}

void i2c_beginTransmission(uint8_t address)
{
    Wire.beginTransmission(address);
}

void i2c_end(void)
{
    Wire.end();
}

uint8_t i2c_endTransmission(uint8_t sendStop)
{
    return Wire.endTransmission(sendStop);
}

uint8_t i2c_requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return Wire.requestFrom(address, quantity, sendStop);
}

size_t i2c_writeOneByte(uint8_t data)
{
    return Wire.write(data);
}

size_t i2c_writeBytes(uint8_t *data, size_t len)
{
    return Wire.write((const uint8_t *)data, len);
}

int i2c_available(void)
{
    return Wire.available();
}

int i2c_read(void)
{
    return Wire.read();
}

int i2c_peek(void)
{
    return Wire.peek();
}

void i2c_flush(void)
{
    Wire.flush();
}

bool i2c_isEnabled(void)
{
    return Wire.isEnabled();
}

void i2c_onReceive( void (*function)(int) )
{
    Wire.onReceive(function);
}

void i2c_onRequest( void (*function)(void) )
{
    Wire.onRequest(function);
}


#ifdef __cplusplus
}
#endif
