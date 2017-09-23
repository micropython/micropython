/**
 * \file
 *
 * \brief SPI related functionality declaration.
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

#ifndef _HAL_SPI_M_SYNC_H_INCLUDED
#define _HAL_SPI_M_SYNC_H_INCLUDED

#include <hal_io.h>
#include <hpl_spi_m_sync.h>

/**
 * \addtogroup doc_driver_hal_spi_master_sync
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief SPI HAL driver struct for polling mode
 *
 */
struct spi_m_sync_descriptor {
	/** SPI device instance */
	struct _spi_sync_dev dev;
	/** I/O read/write */
	struct io_descriptor io;
	/** Flags for HAL driver */
	uint16_t flags;
};

/** \brief Initialize SPI HAL instance and hardware for polling mode
 *
 *  Initialize SPI HAL with polling mode.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] hw Pointer to the hardware base.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_INVALID_DATA Error, initialized.
 */
int32_t spi_m_sync_init(struct spi_m_sync_descriptor *spi, void *const hw);

/** \brief Deinitialize the SPI HAL instance and hardware
 *
 *  Abort transfer, disable and reset SPI, deinit software.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_sync_deinit(struct spi_m_sync_descriptor *spi);

/** \brief Enable SPI
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_sync_enable(struct spi_m_sync_descriptor *spi);

/** \brief Disable SPI
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval <0 Error code.
 */
void spi_m_sync_disable(struct spi_m_sync_descriptor *spi);

/** \brief Set SPI baudrate
 *
 *  Works if SPI is initialized as master, it sets the baudrate.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] baud_val The target baudrate value
 *                  (see "baudrate calculation" for calculating the value).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy
 *  \retval ERR_INVALID_ARG The baudrate is not supported.
 */
int32_t spi_m_sync_set_baudrate(struct spi_m_sync_descriptor *spi, const uint32_t baud_val);

/** \brief Set SPI mode
 *
 *  Set the SPI transfer mode (\ref spi_transfer_mode),
 *  which controls the clock polarity and clock phase:
 *  - Mode 0: leading edge is rising edge, data sample on leading edge.
 *  - Mode 1: leading edge is rising edge, data sample on trailing edge.
 *  - Mode 2: leading edge is falling edge, data sample on leading edge.
 *  - Mode 3: leading edge is falling edge, data sample on trailing edge.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] mode The mode (0~3).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy
 *  \retval ERR_INVALID_ARG The mode is not supported.
 */
int32_t spi_m_sync_set_mode(struct spi_m_sync_descriptor *spi, const enum spi_transfer_mode mode);

/** \brief Set SPI transfer character size in number of bits
 *
 *  The character size (\ref spi_char_size) influence the way the data is
 *  sent/received.
 *  For char size <= 8-bit, data is stored byte by byte.
 *  For char size between 9-bit ~ 16-bit, data is stored in 2-byte length.
 *  Note that the default and recommended char size is 8-bit since it's
 *  supported by all system.
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] char_size The char size (~16, recommended 8).
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy
 *  \retval ERR_INVALID_ARG The char size is not supported.
 */
int32_t spi_m_sync_set_char_size(struct spi_m_sync_descriptor *spi, const enum spi_char_size char_size);

/** \brief Set SPI transfer data order
 *
 *  \param[in] spi Pointer to the HAL SPI instance.
 *  \param[in] dord The data order: send LSB/MSB first.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy
 *  \retval ERR_INVALID_ARG The data order is not supported.
 */
int32_t spi_m_sync_set_data_order(struct spi_m_sync_descriptor *spi, const enum spi_data_order dord);

/** \brief Perform the SPI data transfer (TX and RX) in polling way
 *
 *  Activate CS, do TX and RX and deactivate CS. It blocks.
 *
 *  \param[in, out] spi Pointer to the HAL SPI instance.
 *  \param[in] xfer Pointer to the transfer information (\ref spi_xfer).
 *
 *  \retval size Success.
 *  \retval >=0 Timeout, with number of characters transferred.
 *  \retval ERR_BUSY SPI is busy
 */
int32_t spi_m_sync_transfer(struct spi_m_sync_descriptor *spi, const struct spi_xfer *xfer);

/**
 * \brief Return the I/O descriptor for this SPI instance
 *
 * This function will return an I/O instance for this SPI driver instance.
 *
 * \param[in] spi An SPI master descriptor, which is used to communicate through
 *                SPI
 * \param[in, out] io A pointer to an I/O descriptor pointer type
 *
 * \retval ERR_NONE
 */
int32_t spi_m_sync_get_io_descriptor(struct spi_m_sync_descriptor *const spi, struct io_descriptor **io);

/** \brief Retrieve the current driver version
 *
 *  \return Current driver version.
 */
uint32_t spi_m_sync_get_version(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ifndef _HAL_SPI_M_SYNC_H_INCLUDED */
