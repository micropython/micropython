/**
 * \file
 *
 * \brief I/O SPI related functionality implementation.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "hal_spi_m_sync.h"
#include <utils_assert.h>
#include <utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Driver version
 */
#define SPI_M_SYNC_DRIVER_VERSION 0x00000001u

#define SPI_DEACTIVATE_NEXT 0x8000

static int32_t _spi_m_sync_io_write(struct io_descriptor *const io, const uint8_t *const buf, const uint16_t length);
static int32_t _spi_m_sync_io_read(struct io_descriptor *const io, uint8_t *const buf, const uint16_t length);

int32_t spi_m_sync_init(struct spi_m_sync_descriptor *spi, void *const hw)
{
	int32_t rc = 0;

	ASSERT(spi && hw);

	spi->dev.prvt = (void *)hw;

	rc = _spi_m_sync_init(&spi->dev, hw);
	if (rc < 0) {
		return rc;
	}

	spi->flags    = SPI_DEACTIVATE_NEXT;
	spi->io.read  = _spi_m_sync_io_read;
	spi->io.write = _spi_m_sync_io_write;

	return ERR_NONE;
}

void spi_m_sync_deinit(struct spi_m_sync_descriptor *spi)
{
	ASSERT(spi);

	_spi_m_sync_deinit(&spi->dev);
}

void spi_m_sync_enable(struct spi_m_sync_descriptor *spi)
{
	ASSERT(spi);

	_spi_m_sync_enable(&spi->dev);
}

void spi_m_sync_disable(struct spi_m_sync_descriptor *spi)
{
	ASSERT(spi);

	_spi_m_sync_disable(&spi->dev);
}

int32_t spi_m_sync_set_baudrate(struct spi_m_sync_descriptor *spi, const uint32_t baud_val)
{
	ASSERT(spi);

	return _spi_m_sync_set_baudrate(&spi->dev, baud_val);
}

int32_t spi_m_sync_set_mode(struct spi_m_sync_descriptor *spi, const enum spi_transfer_mode mode)
{
	ASSERT(spi);

	return _spi_m_sync_set_mode(&spi->dev, mode);
}

int32_t spi_m_sync_set_char_size(struct spi_m_sync_descriptor *spi, const enum spi_char_size char_size)
{
	ASSERT(spi);

	return _spi_m_sync_set_char_size(&spi->dev, char_size);
}

int32_t spi_m_sync_set_data_order(struct spi_m_sync_descriptor *spi, const enum spi_data_order dord)
{
	ASSERT(spi);

	return _spi_m_sync_set_data_order(&spi->dev, dord);
}

/** \brief Do SPI read in polling way
 *  For SPI master, activate CS, do send 0xFFs and read data, deactivate CS.
 *
 *  It blocks until all data read or error.
 *
 *  \param[in, out] spi Pointer to the HAL SPI instance.
 *  \param[out] buf Pointer to the buffer to store read data.
 *  \param[in] size Size of the data in number of characters.
 *  \return Operation status.
 *  \retval size Success.
 *  \retval >=0 Time out, with number of characters read.
 */
static int32_t _spi_m_sync_io_read(struct io_descriptor *io, uint8_t *buf, const uint16_t length)
{
	ASSERT(io);

	struct spi_m_sync_descriptor *spi = CONTAINER_OF(io, struct spi_m_sync_descriptor, io);
	struct spi_xfer               xfer;

	xfer.rxbuf = buf;
	xfer.txbuf = 0;
	xfer.size  = length;

	return spi_m_sync_transfer(spi, &xfer);
}

/** \brief Do SPI data write in polling way
 *  For SPI master, activate CS, do buffer send and deactivate CS. The data back
 *  is discarded.
 *
 *  The data read back is discarded.
 *
 *  It blocks until all data sent or error.
 *
 *  \param[in, out] spi Pointer to the HAL SPI instance.
 *  \param[in] p_xfer Pointer to the transfer information (\ref spi_transfer).
 *  \return Operation status.
 *  \retval size Success.
 *  \retval >=0 Timeout, with number of characters transferred.
 */
static int32_t _spi_m_sync_io_write(struct io_descriptor *const io, const uint8_t *const buf, const uint16_t length)
{
	ASSERT(io);

	struct spi_m_sync_descriptor *spi = CONTAINER_OF(io, struct spi_m_sync_descriptor, io);
	struct spi_xfer               xfer;

	xfer.rxbuf = 0;
	xfer.txbuf = (uint8_t *)buf;
	xfer.size  = length;

	return spi_m_sync_transfer(spi, &xfer);
}

int32_t spi_m_sync_transfer(struct spi_m_sync_descriptor *spi, const struct spi_xfer *p_xfer)
{
	struct spi_msg msg;

	ASSERT(spi && p_xfer);

	msg.txbuf = p_xfer->txbuf;
	msg.rxbuf = p_xfer->rxbuf;
	msg.size  = p_xfer->size;

	return _spi_m_sync_trans(&spi->dev, &msg);
}

int32_t spi_m_sync_get_io_descriptor(struct spi_m_sync_descriptor *const spi, struct io_descriptor **io)
{
	ASSERT(spi && io);
	*io = &spi->io;
	return 0;
}

uint32_t spi_m_sync_get_version(void)
{
	return SPI_M_SYNC_DRIVER_VERSION;
}

#ifdef __cplusplus
}
#endif
