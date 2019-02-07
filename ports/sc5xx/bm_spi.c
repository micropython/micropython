/*
 * Copyright (c) 2018 Analog Devices, Inc.  All rights reserved.
 *
 * Bare-Metal ("BM") device driver file for SPI
 *
 */

#include "bm_spi.h"

#define SPI_SIMPLE_MAX_SCLK0_FREQ   (112500000)
#define SPI_SIMPLE_MIN_SCLK0_FREQ    (10000000)

/**
 * @brief      Initialize an instance of the SPI driver
 *
 * @param      device           A pointer to the device driver instance
 * @param[in]  spi_mode         The spi mode (e.g. SPI_MODE_0
 * @param[in]  spi_select_line  The spi select line (if using hardware SPI SELECT)
 *                              Set to NULL to manually manage the SPI select line
 * @param[in]  transmit_len     The transmit length (e.g. SPI_WORDLEN_8BIT)
 * @param[in]  f_sclk0_freq     The f sclk0 frequency to derive SPI clock speed
 * @param[in]  device_num       The device number
 *
 * @return     BM_SPI_RESULT enumeration / result of operation
 */
BM_SPI_RESULT spi_initialize(BM_SPI *device,
                             BM_SPI_MODE spi_mode,
                             BM_SPI_SELECT_PIN spi_select_line,
                             BM_SPI_TRANSFER_LEN transmit_len,
                             uint32_t f_sclk0_freq,
                             BM_SPI_PERIPHERAL_NUMBER device_num) {

    device->device_num = device_num;

    // Set f_sclk0 frequency which is used to set SPI SCLK freq
    if (f_sclk0_freq > SPI_SIMPLE_MAX_SCLK0_FREQ) {
        return SPI_SIMPLE_INVALID_SCLK0_FREQ;
    }
    else if (f_sclk0_freq < SPI_SIMPLE_MIN_SCLK0_FREQ) {
        return SPI_SIMPLE_INVALID_SCLK0_FREQ;
    }
    device->_f_sclk0 = f_sclk0_freq;

    // Set up control register pointers
    if (device_num == BM_SPI0) {
        device->pREG_SPI_SLVSEL = pREG_SPI0_SLVSEL;
        device->pREG_SPI_CTL    = pREG_SPI0_CTL;
        device->pREG_SPI_CLK    = pREG_SPI0_CLK;
        device->pREG_SPI_TFIFO  = pREG_SPI0_TFIFO;
        device->pREG_SPI_RFIFO  = (volatile uint32_t *)pREG_SPI0_RFIFO;
        device->pREG_SPI_TXCTL  = pREG_SPI0_TXCTL;
        device->pREG_SPI_RXCTL  = pREG_SPI0_RXCTL;
        device->pREG_SPI_STAT   = pREG_SPI0_STAT;
    }
    else if (device_num == BM_SPI1) {
        device->pREG_SPI_SLVSEL = pREG_SPI1_SLVSEL;
        device->pREG_SPI_CTL    = pREG_SPI1_CTL;
        device->pREG_SPI_CLK    = pREG_SPI1_CLK;
        device->pREG_SPI_TFIFO  = pREG_SPI1_TFIFO;
        device->pREG_SPI_RFIFO  = (volatile uint32_t *)pREG_SPI1_RFIFO;
        device->pREG_SPI_TXCTL  = pREG_SPI1_TXCTL;
        device->pREG_SPI_RXCTL  = pREG_SPI1_RXCTL;
        device->pREG_SPI_STAT   = pREG_SPI1_STAT;
    }
    else if (device_num == BM_SPI2) {
        device->pREG_SPI_SLVSEL = pREG_SPI2_SLVSEL;
        device->pREG_SPI_CTL    = pREG_SPI2_CTL;
        device->pREG_SPI_CLK    = pREG_SPI2_CLK;
        device->pREG_SPI_TFIFO  = pREG_SPI2_TFIFO;
        device->pREG_SPI_RFIFO  = (volatile uint32_t *)pREG_SPI2_RFIFO;
        device->pREG_SPI_TXCTL  = pREG_SPI2_TXCTL;
        device->pREG_SPI_RXCTL  = pREG_SPI2_RXCTL;
        device->pREG_SPI_STAT   = pREG_SPI2_STAT;
    }

    // Configure the SPI port
    if (spi_select_line != SPI_SSEL_MANUAL) {
        *device->pREG_SPI_CTL =  BITM_SPI_CTL_MSTR |     // SPI is the master device
                                BITM_SPI_CTL_EMISO |     // Enable MISO
                                BITM_SPI_CTL_ASSEL |     // Use SPI port select lines
                                (uint8_t)transmit_len << BITP_SPI_CTL_SIZE |   //< Set transmit size
                                0;
    }
    else {
        *device->pREG_SPI_CTL =  BITM_SPI_CTL_MSTR |     // SPI is the master device
                                BITM_SPI_CTL_EMISO |     // Enable MISO
                                (uint8_t)transmit_len << BITP_SPI_CTL_SIZE |   // Set transmit size
                                0;
    }

    // Configure SPI Mode
    if (spi_mode == SPI_MODE_2 || spi_mode == SPI_MODE_3) {
        *device->pREG_SPI_CTL |= BITM_SPI_CTL_CPOL;
    }
    if (spi_mode == SPI_MODE_1 || spi_mode == SPI_MODE_3) {
        *device->pREG_SPI_CTL |= BITM_SPI_CTL_CPHA;
    }

    // Set the corresponding SPI slave select register
    if (spi_select_line != SPI_SSEL_MANUAL) {
        *device->pREG_SPI_SLVSEL = 0xFE00;
        *device->pREG_SPI_SLVSEL |= (1 << (uint8_t)spi_select_line);
    }
    // Otherwise, the GPIO of slave select should be taken care by the user

    // Setup transfer register
    *device->pREG_SPI_RXCTL =   BITM_SPI_RXCTL_RTI |
                              BITM_SPI_RXCTL_REN |
                              0;

    *device->pREG_SPI_TXCTL =   BITM_SPI_TXCTL_TTI |
                              BITM_SPI_TXCTL_TEN |
                              0;

    //< Enable the SPI port
    *device->pREG_SPI_CTL |= BITM_SPI_CTL_EN;

    return SPI_SIMPLE_SUCCESS;
}

/**
 * @brief      Disables the SPI port
 *
 * @param      device  A pointer to the device driver instance
 */
void spi_end(BM_SPI *device) {

    *device->pREG_SPI_CTL = 0;
}

/**
 * @brief      Sets the clock frequency for SPI transactions
 *
 * @param      device     A pointer to the device driver instance
 * @param[in]  clockFreq  The clock frequency
 *
 * @return     The result of the operation - fails with an invalid clock value
 */
BM_SPI_RESULT spi_setClock(BM_SPI *device,
                           uint32_t clockFreq) {

    if (clockFreq == 0 || clockFreq > 20000000) {
        return SPI_SIMPLE_INVALID_SCLK0_FREQ;
    }

    *device->pREG_SPI_CLK = device->_f_sclk0 / ((float)(clockFreq + 1));

    return SPI_SIMPLE_SUCCESS;
}

/**
 * @brief     Performs a simultaneous read and write on MOSI and MISO
 *
 * This implementation does not rely on the default slave select functionality
 * within the SPI peripheral due to timing issues encountered with this
 * functionality during testing.
 *
 * @param      device  A pointer to the device driver instance
 * @param[in]  value   The value
 *
 * @return     The received word during a SPI transaction
 */
uint32_t spi_transfer_single(BM_SPI *device,
                      uint32_t value) {

    uint32_t rx_value;

    // Write value to
    *device->pREG_SPI_TFIFO = value;

    // Wait for transfer to complete
    while (*device->pREG_SPI_STAT & BITM_SPI_STAT_RFE) {}

    rx_value = *device->pREG_SPI_RFIFO;

    return rx_value;
}

