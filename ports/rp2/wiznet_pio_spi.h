/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WIZNET_PIO_SPI_H_
#define _WIZNET_PIO_SPI_H_

typedef struct wiznet_pio_spi_config {
    uint8_t data_in_pin;
    uint8_t data_out_pin;
    uint8_t cs_pin;
    uint8_t clock_pin;
    uint8_t irq_pin;
    uint8_t reset_pin;
    uint16_t clock_div_major;
    uint8_t clock_div_minor;
    uint8_t spi_hw_instance;
} wiznet_pio_spi_config_t;

typedef struct wiznet_pio_spi_funcs **wiznet_pio_spi_handle_t;

typedef struct wiznet_pio_spi_funcs {
    void (*close)(wiznet_pio_spi_handle_t funcs);
    void (*set_active)(wiznet_pio_spi_handle_t funcs);
    void (*set_inactive)(void);
    void (*frame_start)(void);
    void (*frame_end)(void);
    uint8_t (*read_byte)(void);
    void (*write_byte)(uint8_t tx_data);
    void (*read_buffer)(uint8_t *pBuf, uint16_t len);
    void (*write_buffer)(const uint8_t *pBuf, uint16_t len);
    void (*reset)(wiznet_pio_spi_handle_t funcs);
} wiznet_pio_spi_funcs_t;

wiznet_pio_spi_handle_t wiznet_pio_spi_open(const wiznet_pio_spi_config_t *pio_spi_config);
void wiznet_pio_spi_close(wiznet_pio_spi_handle_t handle);
bool wiznet_pio_spi_transfer(const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length);
void wiznet_pio_spi_read_buffer(uint8_t *pBuf, uint16_t len);
void wiznet_pio_spi_write_buffer(const uint8_t *pBuf, uint16_t len);

#endif
