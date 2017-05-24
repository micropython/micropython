/**
 * \file
 *
 * \brief SAM I2S - Inter-IC Sound Controller
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "i2s.h"

/**
 * \brief Initializes a hardware I<SUP>2</SUP>S module instance
 *
 * Enables the clock and initialize the I<SUP>2</SUP>S module.
 *
 * \param[in,out] module_inst  Pointer to the software module instance struct
 * \param[in]     hw           Pointer to the TCC hardware module
 *
 * \return Status of the initialization procedure.
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_BUSY         Hardware module was busy when the
 *                             initialization procedure was attempted
 * \retval STATUS_ERR_DENIED   Hardware module was already enabled
 */
enum status_code i2s_init(
		struct i2s_module *const module_inst,
		I2s *hw)
{
	Assert(module_inst);
	Assert(hw);

	/* Enable the user interface clock in the PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_I2S);

	/* Status check */
	uint32_t ctrla;
	ctrla = module_inst->hw->CTRLA.reg;
	if (ctrla & I2S_CTRLA_ENABLE) {
		if (ctrla & (I2S_CTRLA_SEREN1 |
				I2S_CTRLA_SEREN0 | I2S_CTRLA_CKEN1 | I2S_CTRLA_CKEN0)) {
			return STATUS_BUSY;
		} else {
			return STATUS_ERR_DENIED;
		}
	}

	/* Initialize module */
	module_inst->hw = hw;

	/* Initialize serializers */
#if I2S_CALLBACK_MODE == true
	int i, j;
	for (i = 0; i < 2; i ++) {
		for (j = 0; j < I2S_SERIALIZER_CALLBACK_N; j ++) {
			module_inst->serializer[i].callback[j] = NULL;
		}
		module_inst->serializer[i].registered_callback_mask = 0;
		module_inst->serializer[i].enabled_callback_mask = 0;

		module_inst->serializer[i].job_buffer = NULL;
		module_inst->serializer[i].job_status = STATUS_OK;
		module_inst->serializer[i].requested_words = 0;
		module_inst->serializer[i].transferred_words = 0;

		module_inst->serializer[i].mode = I2S_SERIALIZER_RECEIVE;
		module_inst->serializer[i].data_size = I2S_DATA_SIZE_32BIT;
	}

	_i2s_instances[0] = module_inst;

	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_I2S);
#endif

	return STATUS_OK;
}


/**
 * \brief Configure specified I<SUP>2</SUP>S clock unit
 *
 * Enables the clock and initialize the clock unit, based on the given
 * configurations.
 *
 * \param[in,out] module_inst  Pointer to the software module instance struct
 * \param[in]     clock_unit   I<SUP>2</SUP>S clock unit to initialize and configure
 * \param[in]     config       Pointer to the I<SUP>2</SUP>S clock unit configuration
 *                             options struct
 *
 * \return Status of the configuration procedure.
 *
 * \retval STATUS_OK              The module was initialized successfully
 * \retval STATUS_BUSY            Hardware module was busy when the
 *                                configuration procedure was attempted
 * \retval STATUS_ERR_DENIED      Hardware module was already enabled
 * \retval STATUS_ERR_INVALID_ARG Invalid divider value or
 *                                MCK direction setting conflict
 */
enum status_code i2s_clock_unit_set_config(
		struct i2s_module *const module_inst,
		const enum i2s_clock_unit clock_unit,
		const struct i2s_clock_unit_config *config)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(clock_unit < I2S_CLOCK_UNIT_N);
	Assert(config);

	/* Status check */
	uint32_t ctrla, syncbusy;
	syncbusy = module_inst->hw->SYNCBUSY.reg;
	ctrla = module_inst->hw->CTRLA.reg;

	/* Busy ? */
	if (syncbusy & (I2S_SYNCBUSY_CKEN0 << clock_unit)) {
		return STATUS_BUSY;
	}
	/* Already enabled ? */
	if (ctrla & (I2S_CTRLA_CKEN0 << clock_unit)) {
		return STATUS_ERR_DENIED;
	}
	/* Parameter check */
	if (config->clock.mck_src && config->clock.mck_out_enable) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Initialize Clock Unit */
	uint32_t clkctrl =
		(config->clock.mck_out_invert ? I2S_CLKCTRL_MCKOUTINV : 0) |
		(config->clock.sck_out_invert ? I2S_CLKCTRL_SCKOUTINV : 0) |
		(config->frame.frame_sync.invert_out ? I2S_CLKCTRL_FSOUTINV : 0) |
		(config->clock.mck_out_enable ? I2S_CLKCTRL_MCKEN : 0) |
		(config->clock.mck_src ? I2S_CLKCTRL_MCKSEL : 0) |
		(config->clock.sck_src ? I2S_CLKCTRL_SCKSEL : 0) |
		(config->frame.frame_sync.invert_use ? I2S_CLKCTRL_FSINV : 0) |
		(config->frame.frame_sync.source ? I2S_CLKCTRL_FSSEL : 0) |
		(config->frame.data_delay ? I2S_CLKCTRL_BITDELAY : 0);

	uint8_t div_val = config->clock.mck_out_div;
	if ((div_val > 0x21) || (div_val == 0)) {
		return STATUS_ERR_INVALID_ARG;
	} else {
		div_val --;
	}
	clkctrl |= I2S_CLKCTRL_MCKOUTDIV(div_val);

	div_val = config->clock.sck_div;
	if ((div_val > 0x21) || (div_val == 0)) {
		return STATUS_ERR_INVALID_ARG;
	} else {
		div_val --;
	}
	clkctrl |= I2S_CLKCTRL_MCKDIV(div_val);

	uint8_t number_slots = config->frame.number_slots;
	if (number_slots > 8) {
		return STATUS_ERR_INVALID_ARG;
	} else if (number_slots > 0) {
		number_slots --;
	}
	clkctrl |=
			I2S_CLKCTRL_NBSLOTS(number_slots) |
			I2S_CLKCTRL_FSWIDTH(config->frame.frame_sync.width) |
			I2S_CLKCTRL_SLOTSIZE(config->frame.slot_size);

	/* Write clock unit configurations */
	module_inst->hw->CLKCTRL[clock_unit].reg = clkctrl;

	/* Select general clock source */
	const uint8_t i2s_gclk_ids[2] = {I2S_GCLK_ID_0, I2S_GCLK_ID_1};
	struct system_gclk_chan_config gclk_chan_config;
	system_gclk_chan_get_config_defaults(&gclk_chan_config);
	gclk_chan_config.source_generator = config->clock.gclk_src;
	system_gclk_chan_set_config(i2s_gclk_ids[clock_unit], &gclk_chan_config);
	system_gclk_chan_enable(i2s_gclk_ids[clock_unit]);

	/* Initialize pins */
	struct system_pinmux_config pin_config;
	system_pinmux_get_config_defaults(&pin_config);
	if (config->mck_pin.enable) {
		pin_config.mux_position = config->mck_pin.mux;
		system_pinmux_pin_set_config(config->mck_pin.gpio, &pin_config);
	}
	if (config->sck_pin.enable) {
		pin_config.mux_position = config->sck_pin.mux;
		system_pinmux_pin_set_config(config->sck_pin.gpio, &pin_config);
	}
	if (config->fs_pin.enable) {
		pin_config.mux_position = config->fs_pin.mux;
		system_pinmux_pin_set_config(config->fs_pin.gpio, &pin_config);
	}

	return STATUS_OK;
}


/**
 * \brief Configure specified I<SUP>2</SUP>S serializer
 *
 * Enables the clock and initialize the serializer, based on the given
 * configurations.
 *
 * \param[in,out] module_inst  Pointer to the software module instance struct
 * \param[in]     serializer   I<SUP>2</SUP>S serializer to initialize and configure
 * \param[in]     config       Pointer to the I<SUP>2</SUP>S serializer configuration
 *                             options struct
 *
 * \return Status of the configuration procedure.
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_BUSY         Hardware module was busy when the
 *                             configuration procedure was attempted
 * \retval STATUS_ERR_DENIED   Hardware module was already enabled
 */
enum status_code i2s_serializer_set_config(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const struct i2s_serializer_config *config)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(serializer < I2S_SERIALIZER_N);
	Assert(config);

	/* Status check */
	uint32_t ctrla, syncbusy;
	syncbusy = module_inst->hw->SYNCBUSY.reg;
	ctrla = module_inst->hw->CTRLA.reg;

	/* Busy ? */
	if (syncbusy & ((I2S_SYNCBUSY_SEREN0 | I2S_SYNCBUSY_DATA0) << serializer)) {
		return STATUS_BUSY;
	}
	/* Already enabled ? */
	if (ctrla & (I2S_CTRLA_CKEN0 << serializer)) {
		return STATUS_ERR_DENIED;
	}

	/* Initialize Serializer */
	uint32_t serctrl =
			(config->loop_back ? I2S_SERCTRL_RXLOOP : 0) |
			(config->dma_usage ? I2S_SERCTRL_DMA : 0) |
			(config->mono_mode ? I2S_SERCTRL_MONO : 0) |
			(config->disable_data_slot[7] ? I2S_SERCTRL_SLOTDIS7 : 0) |
			(config->disable_data_slot[6] ? I2S_SERCTRL_SLOTDIS6 : 0) |
			(config->disable_data_slot[5] ? I2S_SERCTRL_SLOTDIS5 : 0) |
			(config->disable_data_slot[4] ? I2S_SERCTRL_SLOTDIS4 : 0) |
			(config->disable_data_slot[3] ? I2S_SERCTRL_SLOTDIS3 : 0) |
			(config->disable_data_slot[2] ? I2S_SERCTRL_SLOTDIS2 : 0) |
			(config->disable_data_slot[1] ? I2S_SERCTRL_SLOTDIS1 : 0) |
			(config->disable_data_slot[0] ? I2S_SERCTRL_SLOTDIS0 : 0) |
			(config->transfer_lsb_first ? I2S_SERCTRL_BITREV : 0) |
			(config->data_adjust_left_in_word ? I2S_SERCTRL_WORDADJ : 0) |
			(config->data_adjust_left_in_slot ? I2S_SERCTRL_SLOTADJ : 0) |
			(config->data_padding ? I2S_SERCTRL_TXSAME : 0);

	if (config->clock_unit < I2S_CLOCK_UNIT_N) {
		serctrl |= (config->clock_unit ? I2S_SERCTRL_CLKSEL : 0);
	} else {
		return STATUS_ERR_INVALID_ARG;
	}

	serctrl |=
			I2S_SERCTRL_SERMODE(config->mode) |
			I2S_SERCTRL_TXDEFAULT(config->line_default_state) |
			I2S_SERCTRL_DATASIZE(config->data_size) |
			I2S_SERCTRL_EXTEND(config->bit_padding);

	/* Write Serializer configuration */
	module_inst->hw->SERCTRL[serializer].reg = serctrl;

	/* Initialize pins */
	struct system_pinmux_config pin_config;
	system_pinmux_get_config_defaults(&pin_config);
	if (config->data_pin.enable) {
		pin_config.mux_position = config->data_pin.mux;
		system_pinmux_pin_set_config(config->data_pin.gpio, &pin_config);
	}

	/* Save configure */
	module_inst->serializer[serializer].mode = config->mode;
	module_inst->serializer[serializer].data_size = config->data_size;

	return STATUS_OK;
}



/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the I<SUP>2</SUP>S software instance struct
 *
 * \return Bitmask of \c I2S_STATUS_* flags.
 *
 * \retval I2S_STATUS_SYNC_BUSY            Module is busy synchronization
 * \retval I2S_STATUS_TRANSMIT_UNDERRUN(x) Serializer x (0~1) is underrun
 * \retval I2S_STATUS_TRANSMIT_READY(x)    Serializer x (0~1) is ready to
 *                                         transmit new data word
 * \retval I2S_STATUS_RECEIVE_OVERRUN(x)   Serializer x (0~1) is overrun
 * \retval I2S_STATUS_RECEIVE_READY(x)     Serializer x (0~1) has data ready to
 *                                         read
 */
uint32_t i2s_get_status(
		const struct i2s_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	uint32_t intflag = module_inst->hw->INTFLAG.reg;
	uint32_t status;
	if (module_inst->hw->SYNCBUSY.reg) {
		status = I2S_STATUS_SYNC_BUSY;
	} else {
		status = 0;
	}
	if (intflag & I2S_INTFLAG_TXUR0) {
		status |= I2S_STATUS_TRANSMIT_UNDERRUN(0);
	}
	if (intflag & I2S_INTFLAG_TXUR1) {
		status |= I2S_STATUS_TRANSMIT_UNDERRUN(1);
	}
	if ((intflag & I2S_INTFLAG_TXRDY0) &&
		!module_inst->hw->SYNCBUSY.bit.DATA0) {
		status |= I2S_STATUS_TRANSMIT_READY(0);
	}
	if ((intflag & I2S_INTFLAG_TXRDY1) &&
		!module_inst->hw->SYNCBUSY.bit.DATA1) {
		status |= I2S_STATUS_TRANSMIT_READY(1);
	}
	if (intflag & I2S_INTFLAG_RXOR0) {
		status |= I2S_STATUS_RECEIVE_OVERRUN(0);
	}
	if (intflag & I2S_INTFLAG_RXOR1) {
		status |= I2S_STATUS_RECEIVE_OVERRUN(1);
	}
	if (intflag & I2S_INTFLAG_RXRDY0) {
		status |= I2S_STATUS_RECEIVE_READY(0);
	}
	if (intflag & I2S_INTFLAG_RXRDY1) {
		status |= I2S_STATUS_RECEIVE_READY(1);
	}
	return status;
}

/**
 * \brief Clears a module status flags.
 *
 * Clears the given status flags of the module.
 *
 * \param[in] module_inst   Pointer to the I<SUP>2</SUP>S software instance struct
 * \param[in] status        Bitmask of \c I2S_STATUS_* flags to clear
 */
void i2s_clear_status(
		const struct i2s_module *const module_inst,
		uint32_t status)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	uint32_t intflag = 0;

	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(0)) {
		intflag = I2S_INTFLAG_TXUR0;
	}
	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(1)) {
		intflag = I2S_INTFLAG_TXUR1;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(0)) {
		intflag = I2S_INTFLAG_TXRDY0;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(1)) {
		intflag = I2S_INTFLAG_TXRDY1;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(0)) {
		intflag = I2S_INTFLAG_RXOR0;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(1)) {
		intflag = I2S_INTFLAG_RXOR1;
	}
	if (status & I2S_STATUS_RECEIVE_READY(0)) {
		intflag = I2S_INTFLAG_RXRDY0;
	}
	if (status & I2S_STATUS_RECEIVE_READY(1)) {
		intflag = I2S_INTFLAG_RXRDY1;
	}
	module_inst->hw->INTFLAG.reg = intflag;
}

/**
 * \brief Enable interrupts on status set
 *
 * Enable the given status interrupt request from the I<SUP>2</SUP>S module.
 *
 * \param[in] module_inst Pointer to the I<SUP>2</SUP>S software instance struct
 * \param[in] status Status interrupts to enable
 *
 * \return Status of enable procedure.
 *
 * \retval STATUS_OK              Interrupt is enabled successfully
 * \retval STATUS_ERR_INVALID_ARG Status with no interrupt is passed
 */
enum status_code i2s_enable_status_interrupt(
		struct i2s_module *const module_inst,
		uint32_t status)
{
	/* Sanity check arguments */
	Assert(module_inst);

	/* No sync busy interrupt */
	if (status & I2S_STATUS_SYNC_BUSY) {
		return STATUS_ERR_INVALID_ARG;
	}
	Assert(module_inst->hw);

	uint32_t intflag = 0;
	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(0)) {
		intflag = I2S_INTFLAG_TXUR0;
	}
	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(1)) {
		intflag = I2S_INTFLAG_TXUR1;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(0)) {
		intflag = I2S_INTFLAG_TXRDY0;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(1)) {
		intflag = I2S_INTFLAG_TXRDY1;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(0)) {
		intflag = I2S_INTFLAG_RXOR0;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(1)) {
		intflag = I2S_INTFLAG_RXOR1;
	}
	if (status & I2S_STATUS_RECEIVE_READY(0)) {
		intflag = I2S_INTFLAG_RXRDY0;
	}
	if (status & I2S_STATUS_RECEIVE_READY(1)) {
		intflag = I2S_INTFLAG_RXRDY1;
	}
	module_inst->hw->INTENSET.reg = intflag;
	return STATUS_OK;
}

/**
 * \brief Disable interrupts on status set
 *
 * Disable the given status interrupt request from the I<SUP>2</SUP>S module.
 *
 * \param[in] module_inst Pointer to the I<SUP>2</SUP>S software instance struct
 * \param[in] status Status interrupts to disable
 */
void i2s_disable_status_interrupt(
		struct i2s_module *const module_inst,
		uint32_t status)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	uint32_t intflag = 0;
	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(0)) {
		intflag = I2S_INTFLAG_TXUR0;
	}
	if (status & I2S_STATUS_TRANSMIT_UNDERRUN(1)) {
		intflag = I2S_INTFLAG_TXUR1;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(0)) {
		intflag = I2S_INTFLAG_TXRDY0;
	}
	if (status & I2S_STATUS_TRANSMIT_READY(1)) {
		intflag = I2S_INTFLAG_TXRDY1;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(0)) {
		intflag = I2S_INTFLAG_RXOR0;
	}
	if (status & I2S_STATUS_RECEIVE_OVERRUN(1)) {
		intflag = I2S_INTFLAG_RXOR1;
	}
	if (status & I2S_STATUS_RECEIVE_READY(0)) {
		intflag = I2S_INTFLAG_RXRDY0;
	}
	if (status & I2S_STATUS_RECEIVE_READY(1)) {
		intflag = I2S_INTFLAG_RXRDY1;
	}
	module_inst->hw->INTENCLR.reg = intflag;
}


/**
 * \brief Write buffer to the specified Serializer of I<SUP>2</SUP>S module
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    The serializer to write to
 * \param[in]  buffer        The data buffer to write
 * \param[in]  size          Number of data words to write
 *
 * \return Status of the initialization procedure.
 *
 * \retval STATUS_OK               The data was sent successfully
 * \retval STATUS_ERR_DENIED       The module or serializer is disabled
 * \retval STATUS_ERR_INVALID_ARG  An invalid buffer pointer was supplied
 */
enum status_code i2s_serializer_write_buffer_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer,
		void *buffer, uint32_t size)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(serializer < I2S_SERIALIZER_N);
	Assert(buffer);

	if (size == 0) {
		return STATUS_OK;
	}

	uint8_t data_size = 1; /* number of bytes */
	struct i2s_serializer_module *data_module = (struct i2s_serializer_module *)
			&module_inst->serializer[serializer];

	/* Check buffer */
	switch(data_module->data_size) {
	case I2S_DATA_SIZE_32BIT:
	case I2S_DATA_SIZE_24BIT:
	case I2S_DATA_SIZE_20BIT:
	case I2S_DATA_SIZE_18BIT:
		if ((uint32_t)buffer & 0x3) {
			return STATUS_ERR_INVALID_ARG;
		}
		data_size = 4;
		break;
	case I2S_DATA_SIZE_16BIT:
	case I2S_DATA_SIZE_16BIT_COMPACT:
		if ((uint32_t)buffer & 0x1) {
			return STATUS_ERR_INVALID_ARG;
		}
		data_size = 2;
		break;
	default:
		break;
	}

	/* Check status */
	if (!(module_inst->hw->CTRLA.reg &
		(I2S_CTRLA_ENABLE | (I2S_CTRLA_SEREN0 << serializer)))) {
		return STATUS_ERR_DENIED;
	}

	/* Write */
	uint32_t i;
	uint32_t sync_bit = I2S_SYNCBUSY_DATA0 << serializer;
	uint32_t ready_bit = I2S_INTFLAG_TXRDY0 << serializer;
	if (4 == data_size) {
		uint32_t *p32 = (uint32_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Tx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			module_inst->hw->DATA[serializer].reg = p32[i];
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	} else if (2 == data_size) {
		uint16_t *p16 = (uint16_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Tx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			module_inst->hw->DATA[serializer].reg = p16[i];
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	} else {
		uint8_t *p8 = (uint8_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Tx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			module_inst->hw->DATA[serializer].reg = p8[i];
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	}

	return STATUS_OK;
}

/**
 * \brief Read from the specified Serializer of I<SUP>2</SUP>S module to a buffer
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    The serializer to write to
 * \param[in]  buffer        The buffer to fill read data (NULL to discard)
 * \param[in]  size          Number of data words to read
 *
 * \return Status of the initialization procedure.
 *
 * \retval STATUS_OK               The data was sent successfully
 * \retval STATUS_ERR_DENIED       The module or serializer is disabled
 * \retval STATUS_ERR_INVALID_ARG  An invalid buffer pointer was supplied
 */
enum status_code i2s_serializer_read_buffer_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer,
		void *buffer, uint32_t size)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	if (size == 0) {
		return STATUS_OK;
	}

	uint8_t data_size = 1; /* number of bytes */
	struct i2s_serializer_module *data_module = (struct i2s_serializer_module *)
			&module_inst->serializer[serializer];

	/* Check buffer */
	switch(data_module->data_size) {
		case I2S_DATA_SIZE_32BIT:
		case I2S_DATA_SIZE_24BIT:
		case I2S_DATA_SIZE_20BIT:
		case I2S_DATA_SIZE_18BIT:
		if ((uint32_t)buffer & 0x3) {
			return STATUS_ERR_INVALID_ARG;
		}
		data_size = 4;
		break;
		case I2S_DATA_SIZE_16BIT:
		case I2S_DATA_SIZE_16BIT_COMPACT:
		if ((uint32_t)buffer & 0x1) {
			return STATUS_ERR_INVALID_ARG;
		}
		data_size = 2;
		break;
	default:
		break;
	}

	/* Check status */
	if (!(module_inst->hw->CTRLA.reg &
		(I2S_CTRLA_ENABLE | (I2S_CTRLA_SEREN0 << serializer)))) {
		return STATUS_ERR_DENIED;
	}

	/* Read */
	uint32_t i;
	uint32_t sync_bit = I2S_SYNCBUSY_DATA0 << serializer;
	uint32_t ready_bit = I2S_INTFLAG_RXRDY0 << serializer;
	if (buffer == NULL) {
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Rx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			module_inst->hw->DATA[serializer].reg;
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	}
	else if (4 == data_size) {
		uint32_t *p32 = (uint32_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Rx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			p32[i] = module_inst->hw->DATA[serializer].reg;
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	} else if (2 == data_size) {
		uint16_t *p16 = (uint16_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Rx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			p16[i] = module_inst->hw->DATA[serializer].reg;
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	} else {
		uint8_t *p8 = (uint8_t*)buffer;
		for (i = 0; i < size; i ++) {
			while(!(module_inst->hw->INTFLAG.reg & ready_bit)) {
				/* Wait Tx ready */
			}
			while(module_inst->hw->SYNCBUSY.reg & sync_bit) {
				/* Wait Sync */
			}
			p8[i] = module_inst->hw->DATA[serializer].reg;
			module_inst->hw->INTFLAG.reg = ready_bit;
		}
	}

	return STATUS_OK;
}
