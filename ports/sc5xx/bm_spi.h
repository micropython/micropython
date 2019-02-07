/*
 * Copyright (c) 2018 Analog Devices, Inc.  All rights reserved.
 *
 * Bare-Metal ("BM") device driver header file for SPI
 *
 */

#ifndef _BM_SPI_H__
#define _BM_SPI_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// Which hardware peripheral
typedef enum _BM_SPI_PERIPHERAL_NUMBER {
    BM_SPI0 = (0),
    BM_SPI1 = (1),
    BM_SPI2 = (2)
} BM_SPI_PERIPHERAL_NUMBER;

// SPI result
typedef enum _BM_SPI_RESULT {
    SPI_SIMPLE_SUCCESS,                 // The API call is success
    SPI_SIMPLE_INVALID_SCLK0_FREQ,      // Invalid valid for SCLK
    SPI_SIMPLE_ERROR                    // General SPI error
} BM_SPI_RESULT;

// SPI transfer length (bits)
typedef enum _BM_SPI_TRANSFER_LEN {
    SPI_WORDLEN_8BIT  = (0),
    SPI_WORDLEN_16BIT = (1),
    SPI_WORDLEN_32BIT = (2)
} BM_SPI_TRANSFER_LEN;

// SPI mode (see https://en.wikipedia.org/wiki/Serial_Peripheral_Interface for more info)
typedef enum _BM_SPI_MODE {
    SPI_MODE_0 = (0),
    SPI_MODE_1 = (1),
    SPI_MODE_2 = (2),
    SPI_MODE_3 = (3)
} BM_SPI_MODE;

// SPI select pin
typedef enum _BM_SPI_SELECT_PIN {
    SPI_SSEL_MANUAL = (0),
    SPI_SSEL_1 = (1),
    SPI_SSEL_2 = (2),
    SPI_SSEL_3 = (3),
    SPI_SSEL_4 = (4),
    SPI_SSEL_5 = (5),
    SPI_SSEL_6 = (6),
    SPI_SSEL_7 = (7)
} BM_SPI_SELECT_PIN;

// Structure definition for SPI driver
typedef struct _BM_SPI
{
    // Memory mapped control registers for the SPI driver
    volatile uint32_t *pREG_SPI_CTL;
    volatile uint32_t *pREG_SPI_STAT;
    volatile uint32_t *pREG_SPI_SLVSEL;
    volatile uint32_t *pREG_SPI_RFIFO;
    volatile uint32_t *pREG_SPI_RXCTL;
    volatile uint32_t *pREG_SPI_TFIFO;
    volatile uint32_t *pREG_SPI_TXCTL;

    uint32_t _f_sclk0;

    // These are cached parameters, they will not be used by the driver directly.
    BM_SPI_PERIPHERAL_NUMBER device_num;
    BM_SPI_MODE spi_mode;
    BM_SPI_TRANSFER_LEN transmit_len;
    uint32_t clockFreq;

    volatile uint32_t *pREG_SPI_CLK;
    uint32_t _ssel_gpio;
} BM_SPI;

// Initializes SPI port
BM_SPI_RESULT spi_initialize(BM_SPI *device,
                             BM_SPI_MODE spi_mode,
                             BM_SPI_SELECT_PIN spi_select_line,
                             BM_SPI_TRANSFER_LEN transmit_len,
                             uint32_t f_sclk0_freq,
                             BM_SPI_PERIPHERAL_NUMBER device_num);

// Closes SPI device
void spi_end(BM_SPI *device);

// Sets the clock frequency of the SPI port
BM_SPI_RESULT spi_setClock(BM_SPI *device,
                           uint32_t clockFreq);

// Initiates a SPI transfer of a value (8, 16 or 32 bit depending on how SPI is configured)
uint32_t spi_transfer_single(BM_SPI *device,
                      uint32_t value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //_BM_SPI_H__
