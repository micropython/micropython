/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef RA_RA_SPI_H_
#define RA_RA_SPI_H_

#include <stdint.h>
#include <stdbool.h>

#include "ra_config.h"
#include "ra_gpio.h"

bool ra_af_find_ch(ra_af_pin_t *af_pin, uint32_t size, uint32_t pin, uint8_t *ch);
bool ra_spi_find_af_ch(uint32_t mosi, uint32_t miso, uint32_t sck, uint8_t *ch);

uint8_t ra_spi_write_byte(uint32_t ch, uint8_t b);
void ra_spi_read_bytes8(uint32_t ch, uint8_t *buf, uint32_t count);
void ra_spi_write_bytes8(uint32_t ch, uint8_t *buf, uint32_t count);
void ra_spi_write_bytes16(uint32_t ch, uint16_t *buf, uint32_t count);
void ra_spi_write_bytes32(uint32_t ch, uint32_t *buf, uint32_t count);
void ra_spi_write_bytes(uint32_t ch, uint32_t bits, uint8_t *buf, uint32_t count);
void ra_spi_transfer8(uint32_t ch, uint8_t *dst, uint8_t *src, uint32_t count);
void ra_spi_transfer16(uint32_t ch, uint16_t *dst, uint16_t *src, uint32_t count);
void ra_spi_transfer32(uint32_t ch, uint32_t *dst, uint32_t *src, uint32_t count);
void ra_spi_transfer(uint32_t ch, uint32_t bits, uint8_t *dst, uint8_t *src, uint32_t count, uint32_t timeout);
void ra_spi_start_xfer(uint32_t ch, uint16_t spcmd, uint8_t spbr);
void ra_spi_end_xfer(uint32_t ch);
void ra_spi_get_conf(uint32_t ch, uint16_t *spcmd, uint8_t *spbr);
void ra_spi_init(uint32_t ch, uint32_t mosi, uint32_t miso, uint32_t sck, uint32_t cs, uint32_t baud, uint32_t bits, uint32_t polarity, uint32_t phase, uint32_t firstbit);
void ra_spi_deinit(uint32_t ch, uint32_t cs);

#endif /* RA_RA_SPI_H_ */
