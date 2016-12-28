#include "spark_wiring_spi.h"

#ifdef __cplusplus
extern "C" {
#endif


void spi_begin(void)
{
    SPI.begin();
}

void spi_end(void)
{
    SPI.end();
}

void spi_setBitOrder(uint8_t bitOrder)
{
    SPI.setBitOrder(bitOrder);
}

void spi_setDataMode(uint8_t mode)
{
    SPI.setDataMode(mode);
}

void spi_setClockDividerReference(unsigned value, unsigned scale)
{
    SPI.setClockDividerReference(value, scale);
}

void spi_setClockDivider(uint8_t divider)
{
    SPI.setClockDivider(divider);
}

void spi_setClockSpeed(unsigned value, unsigned value_scale)
{
    SPI.setClockSpeed(value, value_scale);
}

void spi_computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t *divider, unsigned *clock)
{
	uint8_t _divider;
	unsigned _clock;
	uint8_t& div = _divider;
	unsigned& clk = _clock;

    SPI.computeClockDivider(reference, targetSpeed, div, clk);

    *divider = _divider;
    *clock = _clock;
}

uint8_t spi_transfer(uint8_t data)
{
    return SPI.transfer(data);
}

void spi_transferBytes(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback)
{
    return SPI.transfer(tx_buffer, rx_buffer, length, user_callback);
}

bool spi_isEnabled(void)
{
    return SPI.isEnabled();
}




void spi1_begin(void)
{
    SPI1.begin();
}

void spi1_end(void)
{
    SPI1.end();
}

void spi1_setBitOrder(uint8_t bitOrder)
{
    SPI1.setBitOrder(bitOrder);
}

void spi1_setDataMode(uint8_t mode)
{
    SPI1.setDataMode(mode);
}

void spi1_setClockDividerReference(unsigned value, unsigned scale)
{
    SPI1.setClockDividerReference(value, scale);
}

void spi1_setClockDivider(uint8_t divider)
{
    SPI1.setClockDivider(divider);
}

void spi1_setClockSpeed(unsigned value, unsigned value_scale)
{
    SPI1.setClockSpeed(value, value_scale);
}

void spi1_computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t *divider, unsigned *clock)
{
	uint8_t _divider;
	unsigned _clock;
	uint8_t& div = _divider;
	unsigned& clk = _clock;

    SPI1.computeClockDivider(reference, targetSpeed, div, clk);

    *divider = _divider;
    *clock = _clock;
}

uint8_t spi1_transfer(uint8_t data)
{
    return SPI1.transfer(data);
}

void spi1_transferBytes(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback)
{
    return SPI1.transfer(tx_buffer, rx_buffer, length, user_callback);
}

bool spi1_isEnabled(void)
{
    return SPI1.isEnabled();
}


#ifdef __cplusplus
}
#endif
