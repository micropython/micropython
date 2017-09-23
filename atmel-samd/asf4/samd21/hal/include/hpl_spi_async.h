/**
 * \file
 *
 * \brief Common SPI related functionality declaration.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_SPI_ASYNC_H_INCLUDED
#define _HPL_SPI_ASYNC_H_INCLUDED

#include <hpl_spi.h>
#include <hpl_irq.h>

/**
 * \addtogroup hpl_spi HPL SPI
 *
 *@{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Callbacks the SPI driver must offer in async mode
 */
//@{
/** The callback types */
enum _spi_async_dev_cb_type {
	/** Callback type for transmit, see \ref _spi_async_dev_cb_xfer_t. */
	SPI_DEV_CB_TX,
	/** Callback type for receive, see \ref _spi_async_dev_cb_xfer_t. */
	SPI_DEV_CB_RX,
	/** Callback type for \ref _spi_async_dev_cb_complete_t. */
	SPI_DEV_CB_COMPLETE,
	/** Number of callbacks. */
	SPI_DEV_CB_N
};

struct _spi_async_dev;

/** \brief The prototype for callback on SPI transfer complete.
 *  If status code is zero, it indicates the normal completion, that is,
 *  SS deactivation.
 *  If status code belows zero, it indicates complete.
 */
typedef void (*_spi_async_dev_cb_complete_t)(struct _spi_async_dev *dev, int32_t status);

/** \brief The prototype for callback on SPI transmit/receive event
 *  For TX, the callback is invoked when transmit is done or ready to start
 *  transmit.
 *  For RX, the callback is invoked when receive is done or ready to read data,
 *  see \ref _spi_async_dev_read_one_t on data reading.
 *  Without DMA enabled, the callback is invoked on each character event.
 *  With DMA enabled, the callback is invoked on DMA buffer done.
 */
typedef void (*_spi_async_dev_cb_xfer_t)(struct _spi_async_dev *dev);

/**
 *  \brief The callbacks offered by SPI driver
 */
struct _spi_async_dev_callbacks {
	/** TX callback, see \ref _spi_async_dev_cb_xfer_t. */
	_spi_async_dev_cb_xfer_t tx;
	/** RX callback, see \ref _spi_async_dev_cb_xfer_t. */
	_spi_async_dev_cb_xfer_t rx;
	/** Complete or complete callback, see \ref _spi_async_dev_cb_complete_t. */
	_spi_async_dev_cb_complete_t complete;
};
//@}

/**
 *  \brief SPI async driver
 */
//@{

/** SPI driver to support async HAL */
struct _spi_async_dev {
	/** Pointer to the hardware base or private data for special device. */
	void *prvt;
	/** Data size, number of bytes for each character */
	uint8_t char_size;
	/** Dummy byte used in master mode when reading the slave */
	uint16_t dummy_byte;

	/** \brief Pointer to callback functions, ignored for polling mode
	 *  Pointer to the callback functions so that initialize the driver to
	 *  handle interrupts.
	 */
	struct _spi_async_dev_callbacks callbacks;
	/** IRQ instance for SPI device. */
	struct _irq_descriptor irq;
};
//@}

#ifdef __cplusplus
}
#endif

/**@}*/
#endif /* ifndef _HPL_SPI_ASYNC_H_INCLUDED */
