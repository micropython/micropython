/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WIZNET_SPI_FUNCS_H_
#define _WIZNET_SPI_FUNCS_H_

#include <stdint.h>
#include "lib/wiznet6k/Ethernet/wizchip_conf.h"

typedef struct wiznet_pio_spi_funcs **wiznet_pio_spi_handle_t;
#if   (_WIZCHIP_ == W6300)
typedef struct wiznet_pio_spi_config
{
    uint16_t clock_div_major;
    uint8_t clock_div_minor;
    uint8_t clock_pin;
    uint8_t data_io0_pin;
    uint8_t data_io1_pin;
    uint8_t data_io2_pin;
    uint8_t data_io3_pin;
    uint8_t cs_pin;
    uint8_t reset_pin;
    uint8_t irq_pin;
} wiznet_pio_spi_config_t;

typedef struct wiznet_pio_spi_funcs {
    void (*close)(wiznet_pio_spi_handle_t funcs);
    void (*set_active)(wiznet_pio_spi_handle_t funcs);
    void (*set_inactive)(void);
    void (*frame_start)(void);
    void (*frame_end)(void);
    void (*read_byte)(uint8_t opcode, uint16_t addr, uint8_t *pBuf, uint16_t len);
    void (*write_byte)(uint8_t opcode, uint16_t addr, uint8_t *pBuf, uint16_t len);
    void (*read_buffer)(uint8_t *pBuf, uint16_t len);
    void (*write_buffer)(uint8_t *pBuf, uint16_t len);
    void (*reset)(wiznet_pio_spi_handle_t funcs);
} wiznet_pio_spi_funcs_t;
#else
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

typedef struct wiznet_pio_spi_funcs {
    void (*close)(wiznet_pio_spi_handle_t funcs);
    void (*set_active)(wiznet_pio_spi_handle_t funcs);
    void (*set_inactive)(void);
    void (*frame_start)(void);
    void (*frame_end)(void);
    uint8_t (*read_byte)(void);
    void (*write_byte)(uint8_t tx_data);
    void (*read_buffer)(uint8_t *pBuf, uint16_t len);
    void (*write_buffer)(uint8_t *pBuf, uint16_t len);
    void (*reset)(wiznet_pio_spi_handle_t funcs);
} wiznet_pio_spi_funcs_t;
#endif


#endif

#ifndef _WIZNET_SPI_PIO_H_
#define _WIZNET_SPI_PIO_H_

wiznet_pio_spi_handle_t wiznet_pio_spi_open(const wiznet_pio_spi_config_t *spi_config);
void wiznet_pio_spi_close(wiznet_pio_spi_handle_t handle);
// static bool wiznet_pio_spi_transfer_impl(spi_pio_state_t *state, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length);
void wiznet_pio_spi_read_buffer(uint8_t *pBuf, uint16_t len);
void wiznet_pio_spi_write_buffer(const uint8_t *pBuf, uint16_t len);

#endif
