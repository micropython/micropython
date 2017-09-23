/**
 * \file
 *
 * \brief SPI Master DMA related functionality declaration.
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

#ifndef _HPL_SPI_M_DMA_H_INCLUDED
#define _HPL_SPI_M_DMA_H_INCLUDED

#include <hpl_spi.h>
#include <hpl_spi_dma.h>

/**
 * \addtogroup hpl_spi HPL SPI
 *
 *
 *@{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Uses common SPI dma device driver. */
#define _spi_m_dma_dev _spi_dma_dev

#define _spi_m_dma_dev_cb_type _spi_dma_dev_cb_type

/**
 *  \brief Initialize SPI for access with interrupts
 *  It will load default hardware configuration and software struct.
 *  \param[in, out] dev Pointer to the SPI device instance.
 *  \param[in] hw Pointer to the hardware base.
 *  \return Operation status.
 *  \retval ERR_INVALID_ARG Input parameter problem.
 *  \retval ERR_BUSY SPI hardware not ready (resetting).
 *  \retval ERR_DENIED SPI has been enabled.
 *  \retval 0 ERR_NONE is operation done successfully.
 */
int32_t _spi_m_dma_init(struct _spi_m_dma_dev *dev, void *const hw);

/**
 *  \brief Initialize SPI for access with interrupts
 *  Disable, reset the hardware and the software struct.
 *  \param[in, out] dev Pointer to the SPI device instance.
 *  \return Operation status.
 *  \retval 0 ERR_NONE is operation done successfully.
 */
int32_t _spi_m_dma_deinit(struct _spi_m_dma_dev *dev);

/**
 *  \brief Enable SPI for access with interrupts
 *  Enable the SPI and enable callback generation of receive and error
 *  interrupts.
 *  \param[in] dev Pointer to the SPI device instance.
 *  \return Operation status.
 *  \retval ERR_INVALID_ARG Input parameter problem.
 *  \retval ERR_BUSY SPI hardware not ready (resetting).
 *  \retval 0 ERR_NONE is operation done successfully.
 */
int32_t _spi_m_dma_enable(struct _spi_m_dma_dev *dev);

/**
 *  \brief Disable SPI for access without interrupts
 *  Disable SPI and interrupts. Deactivate all CS pins if works as master.
 *  \param[in] dev Pointer to the SPI device instance.
 *  \return Operation status.
 *  \retval 0 ERR_NONE is operation done successfully.
 */
int32_t _spi_m_dma_disable(struct _spi_m_dma_dev *dev);

/**
 *  \brief Set SPI transfer mode
 *  Set SPI transfer mode (\ref spi_transfer_mode),
 *  which controls clock polarity and clock phase.
 *  Mode 0: leading edge is rising edge, data sample on leading edge.
 *  Mode 1: leading edge is rising edge, data sample on trailing edge.
 *  Mode 2: leading edge is falling edge, data sample on leading edge.
 *  Mode 3: leading edge is falling edge, data sample on trailing edge.
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] mode The SPI transfer mode.
 *  \return Operation status.
 *  \retval ERR_BUSY SPI is not ready to accept new setting.
 *  \retval 0 ERR_NONE is operation done successfully.
 */
int32_t _spi_m_dma_set_mode(struct _spi_m_dma_dev *dev, const enum spi_transfer_mode mode);

/**
 *  \brief Set SPI baudrate
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] baud_val The SPI baudrate value, see \ref _spi_calc_baud_val() on
 *                  how it's generated.
 *  \return Operation status.
 *  \retval ERR_BUSY SPI is not ready to accept new setting.
 *  \retval 0 Operation done successfully.
 */
int32_t _spi_m_dma_set_baudrate(struct _spi_m_dma_dev *dev, const uint32_t baud_val);

/**
 *  \brief Set SPI baudrate
 *  \param[in, out] dev Pointer to the SPI device instance.
 *  \param[in] char_size The character size, see \ref spi_char_size.
 *  \return Operation status.
 *  \retval ERR_INVALID_ARG The character size is not supported.
 *  \retval ERR_BUSY SPI is not ready to accept new setting.
 *  \retval 0 Operation done successfully.
 */
int32_t _spi_m_dma_set_char_size(struct _spi_m_dma_dev *dev, const enum spi_char_size char_size);

/**
 *  \brief Set SPI data order
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] dord SPI data order (LSB/MSB first).
 *  \return Operation status.
 *  \retval ERR_INVALID_ARG The character size is not supported.
 *  \retval ERR_BUSY SPI is not ready to accept new setting.
 *  \retval 0 Operation done successfully.
 */
int32_t _spi_m_dma_set_data_order(struct _spi_m_dma_dev *dev, const enum spi_data_order dord);

/**
 *  \brief Register the SPI device callback
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] cb_type The callback type.
 *  \param[in] func The callback function to register. NULL to disable callback.
 *  \return Always 0.
 */
void _spi_m_dma_register_callback(struct _spi_m_dma_dev *dev, enum _spi_dma_dev_cb_type, _spi_dma_cb_t func);

/** \brief Do SPI data transfer (TX & RX) with DMA
 *  Log the TX & RX buffers and transfer them in background. It never blocks.
 *
 *  \param[in] dev Pointer to the SPI device instance.
 *  \param[in] txbuf Pointer to the transfer information (\ref spi_transfer).
 *  \param[out] rxbuf Pointer to the receiver information (\ref spi_receive).
 *  \param[in] length spi transfer data length.
 *
 *  \return Operation status.
 *  \retval ERR_NONE Success.
 *  \retval ERR_BUSY Busy.
 */
int32_t _spi_m_dma_transfer(struct _spi_m_dma_dev *dev, uint8_t const *txbuf, uint8_t *const rxbuf,
                            const uint16_t length);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif /* ifndef _HPL_SPI_M_DMA_H_INCLUDED */
