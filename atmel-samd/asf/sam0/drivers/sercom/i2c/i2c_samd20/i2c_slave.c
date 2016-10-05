/**
 * \file
 *
 * \brief SAM D20 I2C Slave Driver
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

#include "i2c_slave.h"
#if I2C_SLAVE_CALLBACK_MODE == true
#  include "i2c_slave_interrupt.h"
#endif

/**
 * \internal Sets configuration to module
 *
 * \param[out] module  Pointer to software module structure
 * \param[in]  config  Configuration structure with configurations to set
 *
 * \return Status of setting configuration.
 * \retval STATUS_OK                       Module was configured correctly
 * \retval STATUS_ERR_ALREADY_INITIALIZED  If setting other GCLK generator than
 *                                         previously set
 */
static enum status_code _i2c_slave_set_config(
		struct i2c_slave_module *const module,
		const struct i2c_slave_config *const config)
{
	uint32_t tmp_ctrla;

	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);
	Assert(config);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);
	Sercom *const sercom_hw = module->hw;

	module->buffer_timeout = config->buffer_timeout;

	struct system_pinmux_config pin_conf;
	system_pinmux_get_config_defaults(&pin_conf);

	uint32_t pad0 = config->pinmux_pad0;
	uint32_t pad1 = config->pinmux_pad1;

	/* SERCOM PAD0 - SDA */
	if (pad0 == PINMUX_DEFAULT) {
		pad0 = _sercom_get_default_pad(sercom_hw, 0);
	}
	pin_conf.mux_position = pad0 & 0xFFFF;
	pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK;
	system_pinmux_pin_set_config(pad0 >> 16, &pin_conf);

	/* SERCOM PAD1 - SCL */
	if (pad1 == PINMUX_DEFAULT) {
		pad1 = _sercom_get_default_pad(sercom_hw, 1);
	}
	pin_conf.mux_position = pad1 & 0xFFFF;
	pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK;
	system_pinmux_pin_set_config(pad1 >> 16, &pin_conf);

	/* Prepare config to write to register CTRLA */
	if (config->run_in_standby || system_is_debugger_present()) {
		tmp_ctrla = SERCOM_I2CS_CTRLA_RUNSTDBY;
	} else {
		tmp_ctrla = 0;
	}

	tmp_ctrla |= config->sda_hold_time |
			(config->scl_low_timeout << SERCOM_I2CS_CTRLA_LOWTOUT_Pos);

	i2c_hw->CTRLA.reg |= tmp_ctrla;

	/* Set CTRLB configuration */
	i2c_hw->CTRLB.reg = SERCOM_I2CS_CTRLB_SMEN | config->address_mode;

	i2c_hw->ADDR.reg = config->address << SERCOM_I2CS_ADDR_ADDR_Pos |
			config->address_mask << SERCOM_I2CS_ADDR_ADDRMASK_Pos |
			config->enable_general_call_address << SERCOM_I2CS_ADDR_GENCEN_Pos;

	return STATUS_OK;
}

/**
 * \brief Initializes the requested I<SUP>2</SUP>C hardware module
 *
 * Initializes the SERCOM I<SUP>2</SUP>C slave device requested and sets the provided
 * software module struct.  Run this function before any further use of
 * the driver.
 *
 * \param[out] module  Pointer to software module struct
 * \param[in]  hw      Pointer to the hardware instance
 * \param[in]  config  Pointer to the configuration struct
 *
 * \return Status of initialization.
 * \retval STATUS_OK                       Module initiated correctly
 * \retval STATUS_ERR_DENIED               If module is enabled
 * \retval STATUS_BUSY                     If module is busy resetting
 * \retval STATUS_ERR_ALREADY_INITIALIZED  If setting other GCLK generator than
 *                                         previously set
 */
enum status_code i2c_slave_init(
		struct i2c_slave_module *const module,
		Sercom *const hw,
		const struct i2c_slave_config *const config)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(hw);
	Assert(config);

	/* Initialize software module */
	module->hw = hw;

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Check if module is enabled. */
	if (i2c_hw->CTRLA.reg & SERCOM_I2CS_CTRLA_ENABLE) {
		return STATUS_ERR_DENIED;
	}

	/* Check if reset is in progress. */
	if (i2c_hw->CTRLA.reg & SERCOM_I2CS_CTRLA_SWRST) {
		return STATUS_BUSY;
	}

	uint32_t sercom_index = _sercom_get_sercom_inst_index(module->hw);
	uint32_t pm_index     = sercom_index + PM_APBCMASK_SERCOM0_Pos;
	uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;

	/* Turn on module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << pm_index);

	/* Set up the GCLK for the module */
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = config->generator_source;
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
	sercom_set_gclk_generator(config->generator_source, false);

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Get sercom instance index. */
	uint8_t instance_index = _sercom_get_sercom_inst_index(module->hw);

	/* Save software module in interrupt handler. */
	_sercom_set_handler(instance_index, _i2c_slave_interrupt_handler);

	/* Save software module. */
	_sercom_instances[instance_index] = module;

	/* Initialize values in module. */
	module->registered_callback = 0;
	module->enabled_callback = 0;
	module->buffer_length = 0;
	module->nack_on_address = config->enable_nack_on_address;
#endif

	/* Set SERCOM module to operate in I2C slave mode. */
	i2c_hw->CTRLA.reg = SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;

	/* Set config and return status. */
	return _i2c_slave_set_config(module, config);
}

/**
 * \brief Resets the hardware module
 *
 * This will reset the module to hardware defaults.
 *
 * \param[in,out] module  Pointer to software module structure
 */
void i2c_slave_reset(
		struct i2c_slave_module *const module)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Reset module instance. */
	module->registered_callback = 0;
	module->enabled_callback = 0;
	module->buffer_length = 0;
	module->buffer_remaining = 0;
	module->buffer = NULL;
#endif

	/* Disable module */
	i2c_slave_disable(module);

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Clear all pending interrupts. */
	system_interrupt_enter_critical_section();
	system_interrupt_clear_pending(_sercom_get_interrupt_vector(module->hw));
	system_interrupt_leave_critical_section();
#endif

	/* Wait for sync. */
	_i2c_slave_wait_for_sync(module);

	/* Reset module. */
	i2c_hw->CTRLA.reg = SERCOM_I2CS_CTRLA_SWRST;
}

/**
 * \internal Waits for answer on bus
 *
 * \param[in]  module  Pointer to software module structure
 *
 * \return Status of bus.
 * \retval STATUS_OK           If given response from slave device
 * \retval STATUS_ERR_TIMEOUT  If no response was given within specified timeout
 *                             period
 */
static enum status_code _i2c_slave_wait_for_bus(
		struct i2c_slave_module *const module)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

	/* Wait for reply. */
	uint16_t timeout_counter = 0;
	while ((!(i2c_module->INTFLAG.reg & SERCOM_I2CS_INTFLAG_DRDY)) &&
			(!(i2c_module->INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC)) &&
			(!(i2c_module->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH))) {

		/* Check timeout condition. */
		if (++timeout_counter >= module->buffer_timeout) {
			return STATUS_ERR_TIMEOUT;
		}
	}
	return STATUS_OK;
}

/**
 * \brief Writes a packet to the master
 *
 * Writes a packet to the master. This will wait for the master to issue
 * a request.
 *
 * \param[in]  module  Pointer to software module structure
 * \param[in]  packet  Packet to write to master
 *
 * \return Status of packet write.
 * \retval STATUS_OK                Packet was written successfully
 * \retval STATUS_ERR_DENIED        Start condition not received, another
 *                                  interrupt flag is set
 * \retval STATUS_ERR_IO            There was an error in the previous transfer
 * \retval STATUS_ERR_BAD_FORMAT    Master wants to write data
 * \retval STATUS_ERR_INVALID_ARG   Invalid argument(s) was provided
 * \retval STATUS_ERR_BUSY          The I<SUP>2</SUP>C module is busy with a job
 * \retval STATUS_ERR_ERR_OVERFLOW  Master NACKed before entire packet was
 *                                  transferred
 * \retval STATUS_ERR_TIMEOUT       No response was given within the timeout
 *                                  period
 */
enum status_code i2c_slave_write_packet_wait(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);
	Assert(packet);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	uint16_t length = packet->data_length;

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Check if the module is busy with a job or AMATCH is enabled */
	if (module->buffer_remaining > 0 ||
			(i2c_hw->INTENSET.reg & SERCOM_I2CS_INTFLAG_AMATCH)) {
		return STATUS_BUSY;
	}
#endif

	enum status_code status;
	/* Wait for master to send address packet */
	status = _i2c_slave_wait_for_bus(module);

	if (status != STATUS_OK) {
			/* Timeout, return */
			return status;
	}
	if (!(i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH)) {
		/* Not address interrupt, something is wrong */
		return STATUS_ERR_DENIED;
	}

	/* Check if there was an error in last transfer */
	if (i2c_hw->STATUS.reg & (SERCOM_I2CS_STATUS_BUSERR |
			SERCOM_I2CS_STATUS_COLL | SERCOM_I2CS_STATUS_LOWTOUT)) {
		return STATUS_ERR_IO;
	}

	/* Check direction */
	if (!(i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_DIR)) {
		/* Write request from master, send NACK and return, workaround 13574*/
		_i2c_slave_set_ctrlb_ackact(module, false);
		_i2c_slave_set_ctrlb_cmd3(module);
		return STATUS_ERR_BAD_FORMAT;
	}

	/* Read request from master, ACK address, workaround 13574 */
	_i2c_slave_set_ctrlb_ackact(module, true);
	_i2c_slave_set_ctrlb_cmd3(module);

	uint16_t i = 0;

	/* Wait for data interrupt */
	status = _i2c_slave_wait_for_bus(module);
	if (status != STATUS_OK) {
			/* Timeout, return */
			return status;
	}

	while (length--) {
		/* Write data */
		_i2c_slave_wait_for_sync(module);
		i2c_hw->DATA.reg = packet->data[i++];

		/* Wait for response from master */
		status = _i2c_slave_wait_for_bus(module);

		if (status != STATUS_OK) {
			/* Timeout, return */
			return status;
		}

		if (i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_RXNACK &&
				length !=0) {
			/* NACK from master, abort */
			/* Release line */
			i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(0x02);

			return STATUS_ERR_OVERFLOW;
			/* Workaround: PIF will probably not be set, ignore */
		}
		/* ACK from master, continue writing */
	}

	/* Release line */
	i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(0x02);

	return STATUS_OK;
}

/**
 * \brief Reads a packet from the master
 *
 * Reads a packet from the master. This will wait for the master to issue a
 * request.
 *
 * \param[in]  module  Pointer to software module structure
 * \param[out] packet  Packet to read from master
 *
 * \return Status of packet read.
 * \retval STATUS_OK                Packet was read successfully
 * \retval STATUS_ABORTED           Master sent stop condition or repeated
 *                                  start before specified length of bytes
 *                                  was received
 * \retval STATUS_ERR_IO            There was an error in the previous transfer
 * \retval STATUS_ERR_DENIED        Start condition not received, another
 *                                  interrupt flag is set
 * \retval STATUS_ERR_INVALID_ARG   Invalid argument(s) was provided
 * \retval STATUS_ERR_BUSY          The I<SUP>2</SUP>C module is busy with a job
 * \retval STATUS_ERR_BAD_FORMAT    Master wants to read data
 * \retval STATUS_ERR_ERR_OVERFLOW  Last byte received overflows buffer
 */
enum status_code i2c_slave_read_packet_wait(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);
	Assert(packet);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	uint16_t length = packet->data_length;

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Check if the module is busy with a job or AMATCH is enabled */
	if (module->buffer_remaining > 0 ||
			(i2c_hw->INTENSET.reg & SERCOM_I2CS_INTFLAG_AMATCH)) {
		return STATUS_BUSY;
	}
#endif

	enum status_code status;

	/* Wait for master to send address packet */
	status = _i2c_slave_wait_for_bus(module);
	if (status != STATUS_OK) {
		/* Timeout, return */
		return status;
	}

	if (!(i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH)) {
		/* Not address interrupt, something is wrong */
		return STATUS_ERR_DENIED;
	}

	/* Check if there was an error in the last transfer */
	if (i2c_hw->STATUS.reg & (SERCOM_I2CS_STATUS_BUSERR |
			SERCOM_I2CS_STATUS_COLL | SERCOM_I2CS_STATUS_LOWTOUT)) {
		return STATUS_ERR_IO;
	}
	/* Check direction */
	if ((i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_DIR)) {
		/* Read request from master, send NACK and return, workaround 13574 */
		_i2c_slave_set_ctrlb_ackact(module, false);
		_i2c_slave_set_ctrlb_cmd3(module);
		return STATUS_ERR_BAD_FORMAT;
	}

	/* Write request from master, ACK address, workaround 13574 */
	_i2c_slave_set_ctrlb_ackact(module, true);
	_i2c_slave_set_ctrlb_cmd3(module);

	uint16_t i = 0;
	while (length--) {

		/* Wait for next byte or stop condition */
		status = _i2c_slave_wait_for_bus(module);
		if (status != STATUS_OK) {
			/* Timeout, return */
			return status;
		}

		if ((i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC) ||
				i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH) {
			/* Master sent stop condition, or repeated start, read done */
			/* Clear stop flag */
			i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
			return STATUS_ABORTED;
		}

		/* Read data */
		_i2c_slave_wait_for_sync(module);
		packet->data[i++] = i2c_hw->DATA.reg;

	}

	/* Packet read done, wait for packet to NACK, Stop or repeated start */
	status = _i2c_slave_wait_for_bus(module);

	if (i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_DRDY) {
		/* Buffer is full, send NACK */
		_i2c_slave_set_ctrlb_ackact(module, false);
		i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(0x2);
	}
	if (i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC) {
		/* Clear stop flag */
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
	}
	return STATUS_OK;
}

/**
 * \brief Waits for a start condition on the bus
 *
 * \note This function is only available for 7-bit slave addressing.
 *
 * Waits for the master to issue a start condition on the bus.
 * \note This function does not check for errors in the last transfer,
 * this will be discovered when reading or writing.
 *
 * \param[in]  module  Pointer to software module structure
 *
 * \return Direction of the current transfer, when in slave mode.
 * \retval I2C_SLAVE_DIRECTION_NONE   No request from master within timeout
 *                                    period
 * \retval I2C_SLAVE_DIRECTION_READ   Write request from master
 * \retval I2C_SLAVE_DIRECTION_WRITE  Read request from master
 */
enum i2c_slave_direction i2c_slave_get_direction_wait(
		struct i2c_slave_module *const module)
{
	/* Sanity check arguments. */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	enum status_code status;

	/* Wait for address interrupt */
	status = _i2c_slave_wait_for_bus(module);

	if (status != STATUS_OK) {
		/* Timeout, return */
		return I2C_SLAVE_DIRECTION_NONE;
	}

	if (!(i2c_hw->INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH)) {
		/* Not address interrupt, something is wrong */
		return I2C_SLAVE_DIRECTION_NONE;
	}

	/* Check direction */
	if ((i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_DIR)) {
		/* Read request from master */
		return I2C_SLAVE_DIRECTION_WRITE;
	} else {
		/* Write request from master */
		return I2C_SLAVE_DIRECTION_READ;
	}
}

/**
 * \brief Retrieves the current module status
 *
 * Checks the status of the module and returns it as a bitmask of status
 * flags.
 *
 * \param[in] module      Pointer to the I<SUP>2</SUP>C slave software device struct
 *
 * \return Bitmask of status flags.
 *
 * \retval I2C_SLAVE_STATUS_ADDRESS_MATCH   A valid address has been received
 * \retval I2C_SLAVE_STATUS_DATA_READY      A I<SUP>2</SUP>C slave byte transmission is
 *                                          successfully completed
 * \retval I2C_SLAVE_STATUS_STOP_RECEIVED   A stop condition is detected for a
 *                                          transaction being processed
 * \retval I2C_SLAVE_STATUS_CLOCK_HOLD      The slave is holding the SCL line
 *                                          low
 * \retval I2C_SLAVE_STATUS_SCL_LOW_TIMEOUT An SCL low time-out has occurred
 * \retval I2C_SLAVE_STATUS_REPEATED_START  Indicates a repeated start, only
 *                                          valid if \ref
 *                                          I2C_SLAVE_STATUS_ADDRESS_MATCH is
 *                                          set
 * \retval I2C_SLAVE_STATUS_RECEIVED_NACK   The last data packet sent was not
 *                                          acknowledged
 * \retval I2C_SLAVE_STATUS_COLLISION       The I<SUP>2</SUP>C slave was not able to
 *                                          transmit a high data or NACK bit
 * \retval I2C_SLAVE_STATUS_BUS_ERROR       An illegal bus condition has
 *                                          occurred on the bus
 */
uint32_t i2c_slave_get_status(
		struct i2c_slave_module *const module)
{
	 /* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	uint8_t intflags = i2c_hw->INTFLAG.reg;
	uint8_t status = i2c_hw->STATUS.reg;
	uint32_t status_flags = 0;

	/* Check Address Match flag */
	if (intflags & SERCOM_I2CS_INTFLAG_AMATCH) {
		status_flags |= I2C_SLAVE_STATUS_ADDRESS_MATCH;
	}
	/* Check Data Ready flag */
	if (intflags & SERCOM_I2CS_INTFLAG_DRDY) {
		status_flags |= I2C_SLAVE_STATUS_DATA_READY;
	}
	/* Check Stop flag */
	if (intflags & SERCOM_I2CS_INTFLAG_PREC) {
		status_flags |= I2C_SLAVE_STATUS_STOP_RECEIVED;
	}
	/* Check Clock Hold */
	if (status & SERCOM_I2CS_STATUS_CLKHOLD) {
		status_flags |= I2C_SLAVE_STATUS_CLOCK_HOLD;
	}
	/* Check SCL Low Timeout */
	if (status & SERCOM_I2CS_STATUS_LOWTOUT) {
		status_flags |= I2C_SLAVE_STATUS_SCL_LOW_TIMEOUT;
	}
	/* Check Repeated Start */
	if (status & SERCOM_I2CS_STATUS_SR) {
		status_flags |= I2C_SLAVE_STATUS_REPEATED_START;
	}
	/* Check Received Not Acknowledge */
	if (status & SERCOM_I2CS_STATUS_RXNACK) {
		status_flags |= I2C_SLAVE_STATUS_RECEIVED_NACK;
	}
	/* Check Transmit Collision */
	if (status & SERCOM_I2CS_STATUS_COLL) {
		status_flags |= I2C_SLAVE_STATUS_COLLISION;
	}
	/* Check Bus Error */
	if (status & SERCOM_I2CS_STATUS_BUSERR) {
		status_flags |= I2C_SLAVE_STATUS_BUS_ERROR;
	}

	return status_flags;
}

/**
 * \brief Clears a module status flag
 *
 * Clears the given status flag of the module.
 *
 * \note Not all status flags can be cleared.
 *
 * \param[in] module         Pointer to the I<SUP>2</SUP>C software device struct
 * \param[in] status_flags   Bit mask of status flags to clear
 *
 */
void i2c_slave_clear_status(
		struct i2c_slave_module *const module,
		uint32_t status_flags)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Clear Address Match flag */
	if (status_flags & I2C_SLAVE_STATUS_ADDRESS_MATCH) {
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
	}
	/* Clear Data Ready flag */
	if (status_flags & I2C_SLAVE_STATUS_DATA_READY) {
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_DRDY;
	}
	/* Clear Stop flag */
	if (status_flags & I2C_SLAVE_STATUS_STOP_RECEIVED) {
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
	}
	/* Clear SCL Low Timeout */
	if (status_flags & I2C_SLAVE_STATUS_SCL_LOW_TIMEOUT) {
		i2c_hw->STATUS.reg = SERCOM_I2CS_STATUS_LOWTOUT;
	}
	/* Clear Transmit Collision */
	if (status_flags & I2C_SLAVE_STATUS_COLLISION) {
		i2c_hw->STATUS.reg = SERCOM_I2CS_STATUS_COLL;
	}
	/* Clear Bus Error */
	if (status_flags & I2C_SLAVE_STATUS_BUS_ERROR) {
		i2c_hw->STATUS.reg = SERCOM_I2CS_STATUS_BUSERR;
	}
}
