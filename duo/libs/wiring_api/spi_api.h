#ifndef _SPI_API_H
#define _SPI_API_H

#include "spi_hal.h"

typedef void (*wiring_spi_dma_transfercomplete_callback_t)(void);


void spi_begin(void);
void spi_end(void);
void spi_setBitOrder(uint8_t bitOrder);
void spi_setDataMode(uint8_t mode);
void spi_setClockDividerReference(unsigned value, unsigned scale);
void spi_setClockDivider(uint8_t divider);
void spi_setClockSpeed(unsigned value, unsigned value_scale);
void spi_computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t *divider, unsigned *clock);
uint8_t spi_transfer(uint8_t data);
void spi_transferBytes(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback);
bool spi_isEnabled(void);



void spi1_begin(void);
void spi1_end(void);
void spi1_setBitOrder(uint8_t bitOrder);
void spi1_setDataMode(uint8_t mode);
void spi1_setClockDividerReference(unsigned value, unsigned scale);
void spi1_setClockDivider(uint8_t divider);
void spi1_setClockSpeed(unsigned value, unsigned value_scale);
void spi1_computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t *divider, unsigned *clock);
uint8_t spi1_transfer(uint8_t data);
void spi1_transferBytes(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback);
bool spi1_isEnabled(void);


#endif
