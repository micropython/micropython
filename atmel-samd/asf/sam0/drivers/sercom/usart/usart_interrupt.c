/**
 * \file
 *
 * \brief SAM SERCOM USART Asynchronous Driver
 *
 * Copyright (C) 2012-2016 Atmel Corporation. All rights reserved.
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

#include "usart_interrupt.h"

/**
 * \internal
 * Asynchronous write of a buffer with a given length
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[in]  tx_data  Pointer to data to be transmitted
 * \param[in]  length   Length of data buffer
 *
 */
enum status_code _usart_write_buffer(
		struct usart_module *const module,
		uint8_t *tx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);
	Assert(tx_data);

	/* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	system_interrupt_enter_critical_section();

	/* Check if the USART transmitter is busy */
	if (module->remaining_tx_buffer_length > 0) {
		system_interrupt_leave_critical_section();
		return STATUS_BUSY;
	}

	/* Write parameters to the device instance */
	module->remaining_tx_buffer_length = length;

	system_interrupt_leave_critical_section();

	module->tx_buffer_ptr              = tx_data;
	module->tx_status                  = STATUS_BUSY;

	/* Enable the Data Register Empty Interrupt */
	usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_DRE;

	return STATUS_OK;
}

/**
 * \internal
 * Asynchronous read of a buffer with a given length
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[in]  rx_data  Pointer to data to be received
 * \param[in]  length   Length of data buffer
 *
 */
enum status_code _usart_read_buffer(
		struct usart_module *const module,
		uint8_t *rx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);
	Assert(rx_data);

	/* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	system_interrupt_enter_critical_section();

	/* Check if the USART receiver is busy */
	if (module->remaining_rx_buffer_length > 0) {
		system_interrupt_leave_critical_section();
		return STATUS_BUSY;
	}

	/* Set length for the buffer and the pointer, and let
	 * the interrupt handler do the rest */
	module->remaining_rx_buffer_length = length;

	system_interrupt_leave_critical_section();

	module->rx_buffer_ptr              = rx_data;
	module->rx_status                  = STATUS_BUSY;

	/* Enable the RX Complete Interrupt */
	usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;

#ifdef FEATURE_USART_LIN_SLAVE
	/* Enable the break character is received Interrupt */
	if(module->lin_slave_enabled) {
		usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXBRK;
	}
#endif

#ifdef FEATURE_USART_START_FRAME_DECTION
	/* Enable a start condition is detected Interrupt */
	if(module->start_frame_detection_enabled) {
		usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXS;
	}
#endif

	return STATUS_OK;
}

/**
 * \brief Registers a callback
 *
 * Registers a callback function, which is implemented by the user.
 *
 * \note The callback must be enabled by \ref usart_enable_callback
 *       in order for the interrupt handler to call it when the conditions for
 *       the callback type are met.
 *
 * \param[in]  module         Pointer to USART software instance struct
 * \param[in]  callback_func  Pointer to callback function
 * \param[in]  callback_type  Callback type given by an enum
 *
 */
void usart_register_callback(
		struct usart_module *const module,
		usart_callback_t callback_func,
		enum usart_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(callback_func);

	/* Register callback function */
	module->callback[callback_type] = callback_func;

	/* Set the bit corresponding to the callback_type */
	module->callback_reg_mask |= (1 << callback_type);
}

/**
 * \brief Unregisters a callback
 *
 * Unregisters a callback function, which is implemented by the user.
 *
 * \param[in,out]  module         Pointer to USART software instance struct
 * \param[in]      callback_type  Callback type given by an enum
 *
 */
void usart_unregister_callback(
		struct usart_module *const module,
		enum usart_callback callback_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Unregister callback function */
	module->callback[callback_type] = NULL;

	/* Clear the bit corresponding to the callback_type */
	module->callback_reg_mask &= ~(1 << callback_type);
}

/**
 * \brief Asynchronous write a single char
 *
 * Sets up the driver to write the data given. If registered and enabled,
 * a callback function will be called when the transmit is completed.
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[in]  tx_data  Data to transfer
 *
 * \returns Status of the operation.
 * \retval STATUS_OK         If operation was completed
 * \retval STATUS_BUSY       If operation was not completed, due to the
 *                           USART module being busy
 * \retval STATUS_ERR_DENIED If the transmitter is not enabled
 */
enum status_code usart_write_job(
		struct usart_module *const module,
		const uint16_t *tx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(tx_data);


	/* Check that the transmitter is enabled */
	if (!(module->transmitter_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Call internal write buffer function with length 1 */
	return _usart_write_buffer(module, (uint8_t *)tx_data, 1);
}

/**
 * \brief Asynchronous read a single char
 *
 * Sets up the driver to read data from the USART module to the data
 * pointer given. If registered and enabled, a callback will be called
 * when the receiving is completed.
 *
 * \param[in]   module   Pointer to USART software instance struct
 * \param[out]  rx_data  Pointer to where received data should be put
 *
 * \returns Status of the operation.
 * \retval  STATUS_OK    If operation was completed
 * \retval  STATUS_BUSY  If operation was not completed
 */
enum status_code usart_read_job(
		struct usart_module *const module,
		uint16_t *const rx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(rx_data);

	/* Call internal read buffer function with length 1 */
	return _usart_read_buffer(module, (uint8_t *)rx_data, 1);
}

/**
 * \brief Asynchronous buffer write
 *
 * Sets up the driver to write a given buffer over the USART. If registered and
 * enabled, a callback function will be called.
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[in]  tx_data  Pointer do data buffer to transmit
 * \param[in]  length   Length of the data to transmit
 *
 * \note If using 9-bit data, the array that *tx_data point to should be defined 
 *       as uint16_t array and should be casted to uint8_t* pointer. Because it 
 *       is an address pointer, the highest byte is not discarded. For example:
 *   \code
          #define TX_LEN 3
          uint16_t tx_buf[TX_LEN] = {0x0111, 0x0022, 0x0133};
          usart_write_buffer_job(&module, (uint8_t*)tx_buf, TX_LEN);
    \endcode
 *
 * \returns Status of the operation.
 * \retval STATUS_OK              If operation was completed successfully.
 * \retval STATUS_BUSY            If operation was not completed, due to the
 *                                USART module being busy
 * \retval STATUS_ERR_INVALID_ARG If operation was not completed, due to invalid
 *                                arguments
 * \retval STATUS_ERR_DENIED      If the transmitter is not enabled
 */
enum status_code usart_write_buffer_job(
		struct usart_module *const module,
		uint8_t *tx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(tx_data);

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Check that the transmitter is enabled */
	if (!(module->transmitter_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Issue internal asynchronous write */
	return _usart_write_buffer(module, tx_data, length);
}

/**
 * \brief Asynchronous buffer read
 *
 * Sets up the driver to read from the USART to a given buffer. If registered
 * and enabled, a callback function will be called.
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[out] rx_data  Pointer to data buffer to receive
 * \param[in]  length   Data buffer length
 *
 * \note If using 9-bit data, the array that *rx_data point to should be defined
 *       as uint16_t array and should be casted to uint8_t* pointer. Because it 
 *       is an address pointer, the highest byte is not discarded. For example:
 *   \code
           #define RX_LEN 3
           uint16_t rx_buf[RX_LEN] = {0x0,};
           usart_read_buffer_job(&module, (uint8_t*)rx_buf, RX_LEN);
    \endcode
 *
 * \returns Status of the operation.
 * \retval STATUS_OK              If operation was completed
 * \retval STATUS_BUSY            If operation was not completed, due to the
 *                                USART module being busy
 * \retval STATUS_ERR_INVALID_ARG If operation was not completed, due to invalid
 *                                arguments
 * \retval STATUS_ERR_DENIED      If the transmitter is not enabled
 */
enum status_code usart_read_buffer_job(
		struct usart_module *const module,
		uint8_t *rx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(rx_data);

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Check that the receiver is enabled */
	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Issue internal asynchronous read */
	return _usart_read_buffer(module, rx_data, length);
}

/**
 * \brief Cancels ongoing read/write operation
 *
 * Cancels the ongoing read/write operation modifying parameters in the
 * USART software struct.
 *
 * \param[in]  module            Pointer to USART software instance struct
 * \param[in]  transceiver_type  Transfer type to cancel
 */
void usart_abort_job(
		struct usart_module *const module,
		enum usart_transceiver_type transceiver_type)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	/* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	switch(transceiver_type) {
		case USART_TRANSCEIVER_RX:
			/* Clear the interrupt flag in order to prevent the receive
			 * complete callback to fire */
			usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXC;

			/* Clear the software reception buffer */
			module->remaining_rx_buffer_length = 0;

			break;

		case USART_TRANSCEIVER_TX:
			/* Clear the interrupt flag in order to prevent the receive
			 * complete callback to fire */
			usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;

			/* Clear the software reception buffer */
			module->remaining_tx_buffer_length = 0;

			break;
	}
}

/**
 * \brief Get status from the ongoing or last asynchronous transfer operation
 *
 * Returns the error from a given ongoing or last asynchronous transfer operation.
 * Either from a read or write transfer.
 *
 * \param[in]  module            Pointer to USART software instance struct
 * \param[in]  transceiver_type  Transfer type to check
  *
 * \return Status of the given job.
 * \retval STATUS_OK               No error occurred during the last transfer
 * \retval STATUS_BUSY             A transfer is ongoing
 * \retval STATUS_ERR_BAD_DATA     The last operation was aborted due to a
 *                                 parity error. The transfer could be affected
 *                                 by external noise
 * \retval STATUS_ERR_BAD_FORMAT   The last operation was aborted due to a
 *                                 frame error
 * \retval STATUS_ERR_OVERFLOW     The last operation was aborted due to a
 *                                 buffer overflow
 * \retval STATUS_ERR_INVALID_ARG  An invalid transceiver enum given
 */
enum status_code usart_get_job_status(
		struct usart_module *const module,
		enum usart_transceiver_type transceiver_type)
{
	/* Sanity check arguments */
	Assert(module);

	/* Variable for status code */
	enum status_code status_code;

	switch(transceiver_type) {
	case USART_TRANSCEIVER_RX:
			status_code = module->rx_status;
			break;

	case USART_TRANSCEIVER_TX:
			status_code = module->tx_status;
			break;

	default:
			status_code = STATUS_ERR_INVALID_ARG;
			break;
	}

	return status_code;
}

/**
 * \internal
 * Handles interrupts as they occur, and it will run callback functions
 * which are registered and enabled.
 *
 * \param[in]  instance  ID of the SERCOM instance calling the interrupt
 *                       handler.
 */
void _usart_interrupt_handler(
		uint8_t instance)
{
	/* Temporary variables */
	uint16_t interrupt_status;
	uint16_t callback_status;
	uint8_t error_code;


	/* Get device instance from the look-up table */
	struct usart_module *module
		= (struct usart_module *)_sercom_instances[instance];

	/* Pointer to the hardware module instance */
	SercomUsart *const usart_hw
		= &(module->hw->USART);

	/* Wait for the synchronization to complete */
	_usart_wait_for_sync(module);

	/* Read and mask interrupt flag register */
	interrupt_status = usart_hw->INTFLAG.reg;
	interrupt_status &= usart_hw->INTENSET.reg;
	callback_status = module->callback_reg_mask &
			module->callback_enable_mask;

	/* Check if a DATA READY interrupt has occurred,
	 * and if there is more to transfer */
	if (interrupt_status & SERCOM_USART_INTFLAG_DRE) {
		if (module->remaining_tx_buffer_length) {
			/* Write value will be at least 8-bits long */
			uint16_t data_to_send = *(module->tx_buffer_ptr);
			/* Increment 8-bit pointer */
			(module->tx_buffer_ptr)++;

			if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
				data_to_send |= (*(module->tx_buffer_ptr) << 8);
				/* Increment 8-bit pointer */
				(module->tx_buffer_ptr)++;
			}
			/* Write the data to send */
			usart_hw->DATA.reg = (data_to_send & SERCOM_USART_DATA_MASK);

			if (--(module->remaining_tx_buffer_length) == 0) {
				/* Disable the Data Register Empty Interrupt */
				usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
				/* Enable Transmission Complete interrupt */
				usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_TXC;

			}
		} else {
			usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
		}
	}

	/* Check if the Transmission Complete interrupt has occurred and
	 * that the transmit buffer is empty */
	if (interrupt_status & SERCOM_USART_INTFLAG_TXC) {

		/* Disable TX Complete Interrupt, and set STATUS_OK */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_TXC;
		module->tx_status = STATUS_OK;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_BUFFER_TRANSMITTED)) {
			(*(module->callback[USART_CALLBACK_BUFFER_TRANSMITTED]))(module);
		}
	}

	/* Check if the Receive Complete interrupt has occurred, and that
	 * there's more data to receive */
	if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {

		if (module->remaining_rx_buffer_length) {
			/* Read out the status code and mask away all but the 4 LSBs*/
			error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
#if !SAMD20
			/* CTS status should not be considered as an error */
			if(error_code & SERCOM_USART_STATUS_CTS) {
				error_code &= ~SERCOM_USART_STATUS_CTS;
			}
#endif
#ifdef FEATURE_USART_LIN_MASTER
			/* TXE status should not be considered as an error */
			if(error_code & SERCOM_USART_STATUS_TXE) {
				error_code &= ~SERCOM_USART_STATUS_TXE;
			}
#endif
			/* Check if an error has occurred during the receiving */
			if (error_code) {
				/* Check which error occurred */
				if (error_code & SERCOM_USART_STATUS_FERR) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_BAD_FORMAT;
					usart_hw->STATUS.reg = SERCOM_USART_STATUS_FERR;
				} else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_OVERFLOW;
					usart_hw->STATUS.reg = SERCOM_USART_STATUS_BUFOVF;
				} else if (error_code & SERCOM_USART_STATUS_PERR) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_BAD_DATA;
					usart_hw->STATUS.reg = SERCOM_USART_STATUS_PERR;
				}
#ifdef FEATURE_USART_LIN_SLAVE
				else if (error_code & SERCOM_USART_STATUS_ISF) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_PROTOCOL;
					usart_hw->STATUS.reg = SERCOM_USART_STATUS_ISF;
				}
#endif
#ifdef FEATURE_USART_COLLISION_DECTION
				else if (error_code & SERCOM_USART_STATUS_COLL) {
					/* Store the error code and clear flag by writing 1 to it */
					module->rx_status = STATUS_ERR_PACKET_COLLISION;
					usart_hw->STATUS.reg = SERCOM_USART_STATUS_COLL;
				}
#endif

				/* Run callback if registered and enabled */
				if (callback_status
						& (1 << USART_CALLBACK_ERROR)) {
					(*(module->callback[USART_CALLBACK_ERROR]))(module);
				}

			} else {

				/* Read current packet from DATA register,
				 * increment buffer pointer and decrement buffer length */
				uint16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

				/* Read value will be at least 8-bits long */
				*(module->rx_buffer_ptr) = received_data;
				/* Increment 8-bit pointer */
				module->rx_buffer_ptr += 1;

				if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
					/* 9-bit data, write next received byte to the buffer */
					*(module->rx_buffer_ptr) = (received_data >> 8);
					/* Increment 8-bit pointer */
					module->rx_buffer_ptr += 1;
				}

				/* Check if the last character have been received */
				if(--(module->remaining_rx_buffer_length) == 0) {
					/* Disable RX Complete Interrupt,
					 * and set STATUS_OK */
					usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_RXC;
					module->rx_status = STATUS_OK;

					/* Run callback if registered and enabled */
					if (callback_status
							& (1 << USART_CALLBACK_BUFFER_RECEIVED)) {
						(*(module->callback[USART_CALLBACK_BUFFER_RECEIVED]))(module);
					}
				}
			}
		} else {
			/* This should not happen. Disable Receive Complete interrupt. */
			usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_RXC;
		}
	}

#ifdef FEATURE_USART_HARDWARE_FLOW_CONTROL
	if (interrupt_status & SERCOM_USART_INTFLAG_CTSIC) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_CTSIC;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_CTSIC;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_CTS_INPUT_CHANGE)) {
			(*(module->callback[USART_CALLBACK_CTS_INPUT_CHANGE]))(module);
		}
	}
#endif

#ifdef FEATURE_USART_LIN_SLAVE
	if (interrupt_status & SERCOM_USART_INTFLAG_RXBRK) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXBRK;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_BREAK_RECEIVED)) {
			(*(module->callback[USART_CALLBACK_BREAK_RECEIVED]))(module);
		}
	}
#endif

#ifdef FEATURE_USART_START_FRAME_DECTION
	if (interrupt_status & SERCOM_USART_INTFLAG_RXS) {
		/* Disable interrupts */
		usart_hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXS;
		/* Clear interrupt flag */
		usart_hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXS;

		/* Run callback if registered and enabled */
		if (callback_status & (1 << USART_CALLBACK_START_RECEIVED)) {
			(*(module->callback[USART_CALLBACK_START_RECEIVED]))(module);
		}
	}
#endif
}

