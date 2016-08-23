/**
 * \file
 *
 * \brief SAM Serial Peripheral Interface Driver
 *
 * Copyright (c) 2013-2016 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "spi_interrupt.h"

/**
 * \internal
 *
 * Dummy byte to send when reading in master mode.
 */
uint16_t dummy_write;

/**
 * \internal
 * Starts transceive of buffers with a given length
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[in]  rx_data  Pointer to data to be received
 * \param[in]  tx_data  Pointer to data to be transmitted
 * \param[in]  length   Length of data buffer
 *
 */
static void _spi_transceive_buffer(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length)
{
	Assert(module);
	Assert(tx_data);

	/* Write parameters to the device instance */
	module->remaining_tx_buffer_length = length;
	module->remaining_rx_buffer_length = length;
	module->rx_buffer_ptr = rx_data;
	module->tx_buffer_ptr = tx_data;
	module->status = STATUS_BUSY;

	module->dir = SPI_DIRECTION_BOTH;

	/* Get a pointer to the hardware module instance */
	SercomSpi *const hw = &(module->hw->SPI);

	/* Enable the Data Register Empty and RX Complete Interrupt */
	hw->INTENSET.reg = (SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY |
			SPI_INTERRUPT_FLAG_RX_COMPLETE);

#  if CONF_SPI_SLAVE_ENABLE == true
	if (module->mode == SPI_MODE_SLAVE) {
		/* Clear TXC flag if set */
		hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
		/* Enable transmit complete interrupt for slave */
		hw->INTENSET.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
	}
#  endif
}

/**
 * \internal
 * Starts write of a buffer with a given length
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[in]  tx_data  Pointer to data to be transmitted
 * \param[in]  length   Length of data buffer
 *
 */
static void _spi_write_buffer(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint16_t length)
{
	Assert(module);
	Assert(tx_data);

	/* Write parameters to the device instance */
	module->remaining_tx_buffer_length = length;
	module->remaining_dummy_buffer_length = length;
	module->tx_buffer_ptr = tx_data;
	module->status = STATUS_BUSY;

	module->dir = SPI_DIRECTION_WRITE;

	/* Get a pointer to the hardware module instance */
	SercomSpi *const hw = &(module->hw->SPI);

#  if CONF_SPI_SLAVE_ENABLE == true
	if (module->mode == SPI_MODE_SLAVE) {
		/* Clear TXC flag if set */
		hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
		/* Enable transmit complete interrupt for slave */
		hw->INTENSET.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
	}
#  endif

	if (module->receiver_enabled) {
		/* Enable the Data Register Empty and RX Complete interrupt */
		hw->INTENSET.reg = (SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY |
				SPI_INTERRUPT_FLAG_RX_COMPLETE);
	} else {
		/* Enable the Data Register Empty interrupt */
		hw->INTENSET.reg = SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;
	}
}

/**
 * \internal
 * Setup SPI to read a buffer with a given length
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[in]  rx_data  Pointer to data to be received
 * \param[in]  length   Length of data buffer
 *
 */
static void _spi_read_buffer(
		struct spi_module *const module,
		uint8_t *rx_data,
		uint16_t length)
{
	Assert(module);
	Assert(rx_data);

	uint8_t tmp_intenset = 0;

	/* Set length for the buffer and the pointer, and let
	 * the interrupt handler do the rest */
	module->remaining_rx_buffer_length = length;
	module->remaining_dummy_buffer_length = length;
	module->rx_buffer_ptr = rx_data;
	module->status = STATUS_BUSY;

	module->dir = SPI_DIRECTION_READ;

	/* Get a pointer to the hardware module instance */
	SercomSpi *const hw = &(module->hw->SPI);

	/* Enable the RX Complete Interrupt */
	tmp_intenset = SPI_INTERRUPT_FLAG_RX_COMPLETE;

#  if CONF_SPI_MASTER_ENABLE == true
	if (module->mode == SPI_MODE_MASTER && module->dir == SPI_DIRECTION_READ) {
		/* Enable Data Register Empty interrupt for master */
		tmp_intenset |= SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;
	}
#  endif
#  if CONF_SPI_SLAVE_ENABLE == true
	if (module->mode == SPI_MODE_SLAVE) {
		/* Clear TXC flag if set */
		hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
		/* Enable transmit complete interrupt for slave */
		tmp_intenset |= SPI_INTERRUPT_FLAG_TX_COMPLETE;

		/* Workaround for SSL flag enable */
#ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
		/* Clear SSL flag if set */
		hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW;
		/* Enable Slave Select Low Interrupt for slave */
		tmp_intenset |= SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW;
#endif
	}
#  endif

	/* Enable all interrupts simultaneously */
	hw->INTENSET.reg = tmp_intenset;
}

/**
 * \brief Registers a SPI callback function
 *
 * Registers a callback function which is implemented by the user.
 *
 * \note The callback must be enabled by \ref spi_enable_callback, in order
 *       for the interrupt handler to call it when the conditions for the
 *       callback type are met.
 *
 * \param[in]  module         Pointer to USART software instance struct
 * \param[in]  callback_func  Pointer to callback function
 * \param[in]  callback_type  Callback type given by an enum
 *
 */
void spi_register_callback(
		struct spi_module *const module,
		spi_callback_t callback_func,
		enum spi_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(callback_func);

	/* Register callback function */
	module->callback[callback_type] = callback_func;

	/* Set the bit corresponding to the callback_type */
	module->registered_callback |= (1 << callback_type);
}

/**
 * \brief Unregisters a SPI callback function
 *
 * Unregisters a callback function which is implemented by the user.
 *
 * \param[in] module         Pointer to SPI software instance struct
 * \param[in] callback_type  Callback type given by an enum
 *
 */
void spi_unregister_callback(
		struct spi_module *const module,
		enum spi_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Unregister callback function */
	module->callback[callback_type] = NULL;

	/* Clear the bit corresponding to the callback_type */
	module->registered_callback &= ~(1 << callback_type);
}

/**
 * \brief Asynchronous buffer write
 *
 * Sets up the driver to write to the SPI from a given buffer. If registered
 * and enabled, a callback function will be called when the write is finished.
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[out] tx_data  Pointer to data buffer to receive
 * \param[in]  length   Data buffer length
 *
 * \returns Status of the write request operation.
 * \retval STATUS_OK               If the operation completed successfully
 * \retval STATUS_ERR_BUSY         If the SPI was already busy with a write
 *                                 operation
 * \retval STATUS_ERR_INVALID_ARG  If requested write length was zero
 */
enum status_code spi_write_buffer_job(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint16_t length)
{
	Assert(module);
	Assert(tx_data);

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Check if the SPI is busy transmitting or slave waiting for TXC*/
	if (module->status == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	/* Issue internal write */
	_spi_write_buffer(module, tx_data, length);

	return STATUS_OK;
}

/**
 * \brief Asynchronous buffer read
 *
 * Sets up the driver to read from the SPI to a given buffer. If registered
 * and enabled, a callback function will be called when the read is finished.
 *
 * \note If address matching is enabled for the slave, the first character
 *       received and placed in the RX buffer will be the address.
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[out] rx_data  Pointer to data buffer to receive
 * \param[in]  length   Data buffer length
 * \param[in]  dummy    Dummy character to send when reading in master mode
 *
 * \returns Status of the operation.
 * \retval  STATUS_OK               If the operation completed successfully
 * \retval  STATUS_ERR_BUSY         If the SPI was already busy with a read
 *                                  operation
 * \retval  STATUS_ERR_DENIED       If the receiver is not enabled
 * \retval  STATUS_ERR_INVALID_ARG  If requested read length was zero
 */
enum status_code spi_read_buffer_job(
		struct spi_module *const module,
		uint8_t *rx_data,
		uint16_t length,
		uint16_t dummy)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(rx_data);

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Check if the SPI is busy transmitting or slave waiting for TXC*/
	if (module->status == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	dummy_write = dummy;
	/* Issue internal read */
	_spi_read_buffer(module, rx_data, length);
	return STATUS_OK;
}

/**
 * \brief Asynchronous buffer write and read
 *
 * Sets up the driver to write and read to and from given buffers. If registered
 * and enabled, a callback function will be called when the transfer is finished.
 *
 * \note If address matching is enabled for the slave, the first character
 *       received and placed in the RX buffer will be the address.
 *
 * \param[in]  module   Pointer to SPI software instance struct
 * \param[in] tx_data   Pointer to data buffer to send
 * \param[out] rx_data  Pointer to data buffer to receive
 * \param[in]  length   Data buffer length
 *
 * \returns Status of the operation.
 * \retval  STATUS_OK               If the operation completed successfully
 * \retval  STATUS_ERR_BUSY         If the SPI was already busy with a read
 *                                  operation
 * \retval  STATUS_ERR_DENIED       If the receiver is not enabled
 * \retval  STATUS_ERR_INVALID_ARG  If requested read length was zero
 */
enum status_code spi_transceive_buffer_job(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(rx_data);

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Check if the SPI is busy transmitting or slave waiting for TXC*/
	if (module->status == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	/* Issue internal transceive */
	_spi_transceive_buffer(module, tx_data, rx_data, length);

	return STATUS_OK;
}
/**
 * \brief Aborts an ongoing job
 *
 * This function will abort the specified job type.
 *
 * \param[in]  module    Pointer to SPI software instance struct
 */
void spi_abort_job(
		struct spi_module *const module)
{
	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw
		= &(module->hw->SPI);

	/* Abort ongoing job */

	/* Disable interrupts */
	spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE |
			SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY |
			SPI_INTERRUPT_FLAG_TX_COMPLETE;

	module->status = STATUS_ABORTED;
	module->remaining_rx_buffer_length = 0;
	module->remaining_dummy_buffer_length = 0;
	module->remaining_tx_buffer_length = 0;

	module->dir = SPI_DIRECTION_IDLE;
}

#  if CONF_SPI_SLAVE_ENABLE == true || CONF_SPI_MASTER_ENABLE == true
/**
 * \internal
 * Writes a character from the TX buffer to the Data register.
 *
 * \param[in,out]  module  Pointer to SPI software instance struct
 */
static void _spi_write(
		struct spi_module *const module)
{
	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw = &(module->hw->SPI);

	/* Write value will be at least 8-bits long */
	uint16_t data_to_send = *(module->tx_buffer_ptr);
	/* Increment 8-bit pointer */
	(module->tx_buffer_ptr)++;

	if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
		data_to_send |= ((*(module->tx_buffer_ptr)) << 8);
		/* Increment 8-bit pointer */
		(module->tx_buffer_ptr)++;
	}

	/* Write the data to send*/
	spi_hw->DATA.reg = data_to_send & SERCOM_SPI_DATA_MASK;

	/* Decrement remaining buffer length */
	(module->remaining_tx_buffer_length)--;
}
#  endif

#  if CONF_SPI_MASTER_ENABLE == true
/**
 * \internal
 * Writes a dummy character to the Data register.
 *
 * \param[in,out]  module  Pointer to SPI software instance struct
 */
static void _spi_write_dummy(
		struct spi_module *const module)
{
	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw = &(module->hw->SPI);

	/* Write dummy byte */
	spi_hw->DATA.reg = dummy_write;

	/* Decrement remaining dummy buffer length */
	module->remaining_dummy_buffer_length--;
}
#  endif

/**
 * \internal
 * Writes a dummy character from the to the Data register.
 *
 * \param[in,out]  module  Pointer to SPI software instance struct
 */
static void _spi_read_dummy(
		struct spi_module *const module)
{
	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw = &(module->hw->SPI);
	uint16_t flush = 0;

	/* Read dummy byte */
	flush = spi_hw->DATA.reg;
	UNUSED(flush);

	/* Decrement remaining dummy buffer length */
	module->remaining_dummy_buffer_length--;
}

/**
 * \internal
 * Reads a character from the Data register to the RX buffer.
 *
 * \param[in,out]  module  Pointer to SPI software instance struct
 */
static void _spi_read(
		struct spi_module *const module)
{
	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw = &(module->hw->SPI);

	uint16_t received_data = (spi_hw->DATA.reg & SERCOM_SPI_DATA_MASK);

	/* Read value will be at least 8-bits long */
	*(module->rx_buffer_ptr) = received_data;
	/* Increment 8-bit pointer */
	module->rx_buffer_ptr += 1;

	if(module->character_size == SPI_CHARACTER_SIZE_9BIT) {
		/* 9-bit data, write next received byte to the buffer */
		*(module->rx_buffer_ptr) = (received_data >> 8);
		/* Increment 8-bit pointer */
		module->rx_buffer_ptr += 1;
	}

	/* Decrement length of the remaining buffer */
	module->remaining_rx_buffer_length--;
}

/**
 * \internal
 *
 * Handles interrupts as they occur, and it will run callback functions
 * which are registered and enabled.
 *
 * \note This function will be called by the Sercom_Handler, and should
 *       not be called directly from any application code.
 *
 * \param[in]  instance  ID of the SERCOM instance calling the interrupt
 *                       handler.
 */
void _spi_interrupt_handler(
		uint8_t instance)
{
	/* Get device instance from the look-up table */
	struct spi_module *module
		= (struct spi_module *)_sercom_instances[instance];

	/* Pointer to the hardware module instance */
	SercomSpi *const spi_hw = &(module->hw->SPI);

	/* Combine callback registered and enabled masks. */
	uint8_t callback_mask =
			module->enabled_callback & module->registered_callback;

	/* Read and mask interrupt flag register */
	uint16_t interrupt_status = spi_hw->INTFLAG.reg;
	interrupt_status &= spi_hw->INTENSET.reg;

	/* Data register empty interrupt */
	if (interrupt_status & SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY) {
#  if CONF_SPI_MASTER_ENABLE == true
		if ((module->mode == SPI_MODE_MASTER) &&
			(module->dir == SPI_DIRECTION_READ)) {
			/* Send dummy byte when reading in master mode */
			_spi_write_dummy(module);
			if (module->remaining_dummy_buffer_length == 0) {
				/* Disable the Data Register Empty Interrupt */
				spi_hw->INTENCLR.reg
						= SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;
			}
		}
#  endif

		if (0
#  if CONF_SPI_MASTER_ENABLE == true
		|| ((module->mode == SPI_MODE_MASTER) &&
			(module->dir != SPI_DIRECTION_READ))
#  endif
#  if CONF_SPI_SLAVE_ENABLE == true
		|| ((module->mode == SPI_MODE_SLAVE) &&
			(module->dir != SPI_DIRECTION_READ))
#  endif
		) {
			/* Write next byte from buffer */
			_spi_write(module);
			if (module->remaining_tx_buffer_length == 0) {
				/* Disable the Data Register Empty Interrupt */
				spi_hw->INTENCLR.reg
						= SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;

				if (module->dir == SPI_DIRECTION_WRITE &&
						!(module->receiver_enabled)) {
					/* Enable the Data Register transmit complete Interrupt */
					spi_hw->INTENSET.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
				}
			}
		}
	}

	/* Receive complete interrupt*/
	if (interrupt_status & SPI_INTERRUPT_FLAG_RX_COMPLETE) {
		/* Check for overflow */
		if (spi_hw->STATUS.reg & SERCOM_SPI_STATUS_BUFOVF) {
			if (module->dir != SPI_DIRECTION_WRITE) {
				/* Store the error code */
				module->status = STATUS_ERR_OVERFLOW;

				/* End transaction */
				module->dir = SPI_DIRECTION_IDLE;

				spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE |
						SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;
				/* Run callback if registered and enabled */
				if (callback_mask & (1 << SPI_CALLBACK_ERROR)) {
					(module->callback[SPI_CALLBACK_ERROR])(module);
				}
			}
			/* Flush */
			uint16_t flush = spi_hw->DATA.reg;
			UNUSED(flush);
			/* Clear overflow flag */
			spi_hw->STATUS.reg = SERCOM_SPI_STATUS_BUFOVF;
		} else {
			if (module->dir == SPI_DIRECTION_WRITE) {
				/* Flush receive buffer when writing */
				_spi_read_dummy(module);
				if (module->remaining_dummy_buffer_length == 0) {
					spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE;
					module->status = STATUS_OK;
					module->dir = SPI_DIRECTION_IDLE;
					/* Run callback if registered and enabled */
					if (callback_mask &
							(1 << SPI_CALLBACK_BUFFER_TRANSMITTED)){
						(module->callback[SPI_CALLBACK_BUFFER_TRANSMITTED])(module);
					}
				}
			} else {
				/* Read data register */
				_spi_read(module);

				/* Check if the last character have been received */
				if (module->remaining_rx_buffer_length == 0) {
					module->status = STATUS_OK;
					/* Disable RX Complete Interrupt and set status */
					spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE;
					if(module->dir == SPI_DIRECTION_BOTH) {
						if (callback_mask & (1 << SPI_CALLBACK_BUFFER_TRANSCEIVED)) {
							(module->callback[SPI_CALLBACK_BUFFER_TRANSCEIVED])(module);
						}
					} else if (module->dir == SPI_DIRECTION_READ) {
						if (callback_mask & (1 << SPI_CALLBACK_BUFFER_RECEIVED)) {
							(module->callback[SPI_CALLBACK_BUFFER_RECEIVED])(module);
						}
					}
				}
			}
		}
	}

	/* Transmit complete */
	if (interrupt_status & SPI_INTERRUPT_FLAG_TX_COMPLETE) {
#  if CONF_SPI_SLAVE_ENABLE == true
		if (module->mode == SPI_MODE_SLAVE) {
			/* Transaction ended by master */

			/* Disable interrupts */
			spi_hw->INTENCLR.reg =
					SPI_INTERRUPT_FLAG_TX_COMPLETE |
					SPI_INTERRUPT_FLAG_RX_COMPLETE |
					SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY;
			/* Clear interrupt flag */
			spi_hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;


			/* Reset all status information */
			module->dir = SPI_DIRECTION_IDLE;
			module->remaining_tx_buffer_length = 0;
			module->remaining_rx_buffer_length = 0;
			module->status = STATUS_OK;

			if (callback_mask &
					(1 << SPI_CALLBACK_SLAVE_TRANSMISSION_COMPLETE)) {
			(module->callback[SPI_CALLBACK_SLAVE_TRANSMISSION_COMPLETE])
					(module);
			}

		}
#  endif
#  if CONF_SPI_MASTER_ENABLE == true
		if ((module->mode == SPI_MODE_MASTER) &&
			(module->dir == SPI_DIRECTION_WRITE) && !(module->receiver_enabled)) {
		  	/* Clear interrupt flag */
		 	spi_hw->INTENCLR.reg
					= SPI_INTERRUPT_FLAG_TX_COMPLETE;
			/* Buffer sent with receiver disabled */
			module->dir = SPI_DIRECTION_IDLE;
			module->status = STATUS_OK;
			/* Run callback if registered and enabled */
			if (callback_mask & (1 << SPI_CALLBACK_BUFFER_TRANSMITTED)){
				(module->callback[SPI_CALLBACK_BUFFER_TRANSMITTED])
						(module);
			}
		}
#endif
	}

#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
#  if CONF_SPI_SLAVE_ENABLE == true
		/* When a high to low transition is detected on the _SS pin in slave mode */
		if (interrupt_status & SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW) {
			if (module->mode == SPI_MODE_SLAVE) {
				/* Disable interrupts */
				spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW;
				/* Clear interrupt flag */
				spi_hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW;

				if (callback_mask & (1 << SPI_CALLBACK_SLAVE_SELECT_LOW)) {
					(module->callback[SPI_CALLBACK_SLAVE_SELECT_LOW])(module);
				}
			}
		}
#  endif
#  endif

#  ifdef FEATURE_SPI_ERROR_INTERRUPT
	/* When combined error happen */
	if (interrupt_status & SPI_INTERRUPT_FLAG_COMBINED_ERROR) {
		/* Disable interrupts */
		spi_hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_COMBINED_ERROR;
		/* Clear interrupt flag */
		spi_hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_COMBINED_ERROR;

		if (callback_mask & (1 << SPI_CALLBACK_COMBINED_ERROR)) {
			(module->callback[SPI_CALLBACK_COMBINED_ERROR])(module);
		}
	}
#  endif
}
