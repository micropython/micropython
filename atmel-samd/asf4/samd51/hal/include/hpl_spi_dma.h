/**
 * \file
 *
 * \brief Common SPI DMA related functionality declaration.
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_SPI_DMA_H_INCLUDED
#define _HPL_SPI_DMA_H_INCLUDED

#include <hpl_irq.h>
#include <hpl_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The callback types */
enum _spi_dma_dev_cb_type {
	/** Callback type for DMA transmit. */
	SPI_DEV_CB_DMA_TX,
	/** Callback type for DMA receive. */
	SPI_DEV_CB_DMA_RX,
	/** Callback type for DMA error. */
	SPI_DEV_CB_DMA_ERROR,
	/** Number of callbacks. */
	SPI_DEV_CB_DMA_N
};

struct _spi_dma_dev;

/**
 *  \brief The prototype for callback on SPI DMA.
 */
typedef void (*_spi_dma_cb_t)(struct _dma_resource *resource);

/**
 *  \brief The callbacks offered by SPI driver
 */
struct _spi_dma_dev_callbacks {
	_spi_dma_cb_t tx;
	_spi_dma_cb_t rx;
	_spi_dma_cb_t error;
};

/** SPI driver to support DMA HAL */
struct _spi_dma_dev {
	/** Pointer to the hardware base or private data for special device. */
	void *prvt;
	/** Pointer to callback functions */
	struct _spi_dma_dev_callbacks callbacks;
	/** IRQ instance for SPI device. */
	struct _irq_descriptor irq;
	/** DMA resource */
	struct _dma_resource *resource;
};

#ifdef __cplusplus
}
#endif

#endif /* ifndef _HPL_SPI_DMA_H_INCLUDED */
