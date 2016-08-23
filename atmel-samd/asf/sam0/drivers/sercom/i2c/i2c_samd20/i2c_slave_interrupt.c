/**
 * \file
 *
 * \brief SAM D20 I2C Slave Interrupt Driver
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include "i2c_slave_interrupt.h"

/**
 * \brief Enables sending of NACK on address match
 *
 * Enables sending of NACK on address match, thus discarding
 * any incoming transaction.
 *
 * \param[in,out] module  Pointer to software module structure
 */
void i2c_slave_enable_nack_on_address(
		struct i2c_slave_module *const module)
{
	/* Sanity check arguments. */
	Assert(module);

	module->nack_on_address = true;
}

/**
 * \brief Disables sending NACK on address match
 *
 * Disables sending of NACK on address match, thus
 * acknowledging incoming transactions.
 *
 * \param[in,out] module  Pointer to software module structure
 */
void i2c_slave_disable_nack_on_address(
		struct i2c_slave_module *const module)
{
	/* Sanity check arguments. */
	Assert(module);

	module->nack_on_address = false;
}

/**
 * \internal
 * Reads next data. Used by interrupt handler to get next data byte from master.
 *
 * \param[in,out] module  Pointer to software module structure
 */
static void _i2c_slave_read(
		struct i2c_slave_module *const module)
{
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Read byte from master and put in buffer. */
	*(module->buffer++) = i2c_hw->DATA.reg;

	/*Decrement remaining buffer length */
	module->buffer_remaining--;
}

/**
 * \internal
 * Writes next data. Used by interrupt handler to send next data byte to master.
 *
 * \param[in,out] module  Pointer to software module structure
 */
static void _i2c_slave_write(
		struct i2c_slave_module *const module)
{
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Write byte from buffer to master */
	i2c_hw->DATA.reg = *(module->buffer++);

	/*Decrement remaining buffer length */
	module->buffer_remaining--;
}

/**
 * \brief Registers callback for the specified callback type
 *
 * Associates the given callback function with the
 * specified callback type. To enable the callback, the
 * \ref i2c_slave_enable_callback function must be used.
 *
 * \param[in,out] module         Pointer to the software module struct
 * \param[in]     callback       Pointer to the function desired for the
 *                               specified callback
 * \param[in]     callback_type  Callback type to register
 */
void i2c_slave_register_callback(
		struct i2c_slave_module *const module,
		i2c_slave_callback_t callback,
		enum i2c_slave_callback callback_type)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);
	Assert(callback);

	/* Register callback. */
	module->callbacks[callback_type] = callback;

	/* Set corresponding bit to set callback as initiated. */
	module->registered_callback |= (1 << callback_type);
}

/**
 * \brief Unregisters callback for the specified callback type
 *
 * Removes the currently registered callback for the given callback
 * type.
 *
 * \param[in,out]  module         Pointer to the software module struct
 * \param[in]      callback_type  Callback type to unregister
 */
void i2c_slave_unregister_callback(
		struct i2c_slave_module *const module,
		enum i2c_slave_callback callback_type)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	/* Register callback. */
	module->callbacks[callback_type] = NULL;

	/* Set corresponding bit to set callback as initiated. */
	module->registered_callback &= ~(1 << callback_type);
}

/**
 * \brief Initiates a reads packet operation
 *
 * Reads a data packet from the master. A write request must be initiated by
 * the master before the packet can be read.
 *
 * The \ref I2C_SLAVE_CALLBACK_WRITE_REQUEST callback can be used to call this
 * function.
 *
 * \param[in,out] module  Pointer to software module struct
 * \param[in,out] packet  Pointer to I<SUP>2</SUP>C packet to transfer
 *
 * \return Status of starting asynchronously reading I<SUP>2</SUP>C packet.
 * \retval STATUS_OK    If reading was started successfully
 * \retval STATUS_BUSY  If module is currently busy with another transfer
 */
enum status_code i2c_slave_read_packet_job(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);
	Assert(packet);

	/* Check if the I2C module is busy doing async operation. */
	if (module->buffer_remaining > 0) {
		return STATUS_BUSY;
	}

	/* Save packet to software module. */
	module->buffer           = packet->data;
	module->buffer_remaining = packet->data_length;
	module->buffer_length    = packet->data_length;
	module->status           = STATUS_BUSY;

	/* Enable interrupts */
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);
	i2c_hw->INTENSET.reg = SERCOM_I2CS_INTFLAG_AMATCH |
			SERCOM_I2CS_INTFLAG_DRDY | SERCOM_I2CS_INTFLAG_PREC;

	/* Read will begin when master initiates the transfer */
	return STATUS_OK;
}

/**
 * \brief Initiates a write packet operation
 *
 * Writes a data packet to the master. A read request must be initiated by
 * the master before the packet can be written.
 *
 * The \ref I2C_SLAVE_CALLBACK_READ_REQUEST callback can be used to call this
 * function.
 *
 * \param[in,out] module  Pointer to software module struct
 * \param[in,out] packet  Pointer to I<SUP>2</SUP>C packet to transfer
 *
 * \return Status of starting writing I<SUP>2</SUP>C packet.
 * \retval STATUS_OK   If writing was started successfully
 * \retval STATUS_BUSY If module is currently busy with another transfer
 */
enum status_code i2c_slave_write_packet_job(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);
	Assert(packet);

	/* Check if the I2C module is busy doing async operation. */
	if (module->buffer_remaining > 0) {
		return STATUS_BUSY;
	}

	/* Save packet to software module. */
	module->buffer           = packet->data;
	module->buffer_remaining = packet->data_length;
	module->buffer_length    = packet->data_length;
	module->status           = STATUS_BUSY;

	/* Enable interrupts */
	SercomI2cs *const i2c_hw = &(module->hw->I2CS);
	i2c_hw->INTENSET.reg = SERCOM_I2CS_INTFLAG_AMATCH |
			SERCOM_I2CS_INTFLAG_DRDY | SERCOM_I2CS_INTFLAG_PREC;

	return STATUS_OK;
}

/**
 * \internal Interrupt handler for I<SUP>2</SUP>C slave
 *
 * \param[in] instance Sercom instance that triggered the interrupt
 */
void _i2c_slave_interrupt_handler(
		uint8_t instance)
{
	/* Get software module for callback handling. */
	struct i2c_slave_module *module =
			(struct i2c_slave_module*)_sercom_instances[instance];

	Assert(module);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Combine callback registered and enabled masks. */
	uint8_t callback_mask =
			module->enabled_callback & module->registered_callback;


	if (i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH) {
	/* Address match */
		/* Check if last transfer is done - repeated start */
		if (module->buffer_length != module->buffer_remaining &&
				module->transfer_direction == I2C_TRANSFER_WRITE) {

			module->status = STATUS_OK;
			module->buffer_length = 0;
			module->buffer_remaining = 0;

			if ((callback_mask & (1 << I2C_SLAVE_CALLBACK_READ_COMPLETE))) {
				module->callbacks[I2C_SLAVE_CALLBACK_READ_COMPLETE](module);
			}
		} else if (module->buffer_length != module->buffer_remaining &&
				module->transfer_direction == I2C_TRANSFER_READ) {
			module->status = STATUS_OK;
			module->buffer_length = 0;
			module->buffer_remaining = 0;

			if ((callback_mask & (1 << I2C_SLAVE_CALLBACK_WRITE_COMPLETE))) {
				module->callbacks[I2C_SLAVE_CALLBACK_WRITE_COMPLETE](module);
			}
		}

		if (i2c_hw->STATUS.reg & (SERCOM_I2CS_STATUS_BUSERR |
				SERCOM_I2CS_STATUS_COLL | SERCOM_I2CS_STATUS_LOWTOUT)) {
			/* An error occurred in last packet transfer */
			module->status = STATUS_ERR_IO;

			if ((callback_mask & (1 << I2C_SLAVE_CALLBACK_ERROR_LAST_TRANSFER))) {
				module->callbacks[I2C_SLAVE_CALLBACK_ERROR_LAST_TRANSFER](module);
			}
		}
		if (module->nack_on_address) {
			/* NACK address, workaround 13574 */
			_i2c_slave_set_ctrlb_ackact(module, false);
		} else if (i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_DIR) {
			/* Set transfer direction in module instance */
			module->transfer_direction = I2C_TRANSFER_READ;

			/* Read request from master */
			if (callback_mask & (1 << I2C_SLAVE_CALLBACK_READ_REQUEST)) {
				module->callbacks[I2C_SLAVE_CALLBACK_READ_REQUEST](module);
			}

			if (module->buffer_length == 0) {
				/* Data buffer not set up, NACK address, workaround 13574*/
				_i2c_slave_set_ctrlb_ackact(module, false);
			} else {
				/* ACK address, workaround 13574 */
				_i2c_slave_set_ctrlb_ackact(module, true);
			}
		} else {
			/* Set transfer direction in dev inst */
			module->transfer_direction = I2C_TRANSFER_WRITE;

			/* Write request from master */
			if (callback_mask & (1 << I2C_SLAVE_CALLBACK_WRITE_REQUEST)) {
				module->callbacks[I2C_SLAVE_CALLBACK_WRITE_REQUEST](module);
			}

			if (module->buffer_length == 0) {
				/* Data buffer not set up, NACK address, workaround 13574 */
				_i2c_slave_set_ctrlb_ackact(module, false);
			} else {
				/* ACK address, workaround 13574 */
				_i2c_slave_set_ctrlb_ackact(module, true);
			}
		}

		/* ACK or NACK address, Workaround 13574 */
		_i2c_slave_set_ctrlb_cmd3(module);

		/* ACK next incoming packet, workaround 13574 */
		_i2c_slave_set_ctrlb_ackact(module, true);

	} else if (i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC) {
		/* Stop condition on bus - current transfer done */

		/* Clear Stop interrupt */
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;

		/* Disable interrupts */
		i2c_hw->INTENCLR.reg = SERCOM_I2CS_INTFLAG_PREC | SERCOM_I2CS_INTFLAG_DRDY;

		if (!((module->enabled_callback & (1 << I2C_SLAVE_CALLBACK_READ_REQUEST))
				|| (module->enabled_callback == (1 << I2C_SLAVE_CALLBACK_WRITE_REQUEST)))) {
			/* Disable address match if read/write request is not enabled */
			i2c_hw->INTENCLR.reg = SERCOM_I2CS_INTFLAG_AMATCH;
		}

		if (!(module->status == STATUS_ERR_OVERFLOW || module->status == STATUS_ERR_IO)) {
			module->status = STATUS_OK;
			module->buffer_length = 0;
			module->buffer_remaining = 0;

			/* Call appropriate callback if enabled and registered */
			if ((callback_mask & (1 << I2C_SLAVE_CALLBACK_READ_COMPLETE))
					&& (module->transfer_direction == I2C_TRANSFER_WRITE)) {
				/* Read from master complete */
				module->callbacks[I2C_SLAVE_CALLBACK_READ_COMPLETE](module);
			} else if ((callback_mask & (1 << I2C_SLAVE_CALLBACK_WRITE_COMPLETE))
					&& (module->transfer_direction == I2C_TRANSFER_READ)) {
				/* Write to master complete */
				module->callbacks[I2C_SLAVE_CALLBACK_WRITE_COMPLETE](module);
			}
					}
	} else if (i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_DRDY) {
		/* Check if buffer is full, or NACK from master */
		if (module->buffer_remaining <= 0 ||
				(module->transfer_direction == I2C_TRANSFER_READ &&
				(module->buffer_length > module->buffer_remaining) &&
				(i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_RXNACK))) {

			module->buffer_remaining = 0;
			module->buffer_length = 0;

			if (module->transfer_direction == I2C_TRANSFER_WRITE) {
				/* Buffer is full, send NACK, workaround 13574 */
				_i2c_slave_set_ctrlb_ackact(module, false);
				i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(0x2);

				/* Set status, new character in DATA register will overflow
				 * buffer */
				module->status = STATUS_ERR_OVERFLOW;

				if (callback_mask & (1 << I2C_SLAVE_CALLBACK_ERROR)) {
					/* Read complete */
					module->callbacks[I2C_SLAVE_CALLBACK_ERROR](module);
				}
			} else {
				/* Release SCL and wait for new start condition */
				_i2c_slave_set_ctrlb_ackact(module, false);
				i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(0x2);

				/* Transfer successful */
				module->status = STATUS_OK;

				/* Disable interrupts */
				i2c_hw->INTENCLR.reg = SERCOM_I2CS_INTFLAG_DRDY;
			}

		/* Continue buffer write/read */
		} else if (module->buffer_length > 0 && module->buffer_remaining > 0) {
			/* Call function based on transfer direction */
			if (module->transfer_direction == I2C_TRANSFER_WRITE) {
				_i2c_slave_read(module);
			} else {
				_i2c_slave_write(module);
			}
		}
	}
}
