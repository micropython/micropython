/**
 * \file
 *
 * \brief SAM Digital-to-Analog Interrupt Driver
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
#include "dac.h"
#include "dac_callback.h"

struct dac_module *_dac_instances[DAC_INST_NUM];

/**
 * \brief Convert a specific number digital data to analog through DAC.
 *
 * This function will perform a conversion of specific number of digital data.
 * The conversion should be event-triggered, the data will be written to DATABUF
 * and transferred to the DATA register and converted when a Start Conversion
 * Event is issued.
 * Conversion data must be right or left adjusted according to configuration
 * settings.
 * \note To be event triggered, the enable_start_on_event must be
 * enabled in the configuration.
 *
 * \param[in] module_inst      Pointer to the DAC software device struct
 * \param[in] channel          DAC channel to write to
 * \param[in] buffer             Pointer to the digital data write buffer to be converted
 * \param[in] length             Size of the write buffer
 *
 * \return Status of the operation.
 * \retval STATUS_OK           If the data was written
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 * \retval STATUS_BUSY      The DAC is busy to accept new job
 */
enum status_code dac_chan_write_buffer_job(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		uint16_t *buffer,
		uint32_t length)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(buffer);

	UNUSED(channel);

	Dac *const dac_module = module_inst->hw;

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if(module_inst->remaining_conversions != 0 ||
			module_inst->job_status == STATUS_BUSY){
		return STATUS_BUSY;
	}

	/* Wait until the synchronization is complete */
	while (dac_is_syncing(module_inst)) {
	};

	module_inst->job_status = STATUS_BUSY;

	module_inst->remaining_conversions = length;
	module_inst->job_buffer = buffer;
	module_inst->transferred_conversions = 0;

	/* Enable interrupt */
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_DAC);
	dac_module->INTFLAG.reg = DAC_INTFLAG_UNDERRUN | DAC_INTFLAG_EMPTY;
	dac_module->INTENSET.reg = DAC_INTENSET_UNDERRUN | DAC_INTENSET_EMPTY;

	return STATUS_OK;
}

/**
 * \brief Convert one digital data job.
 *
 * This function will perform a conversion of specfic number of digital data.
 * The conversion is event-triggered, the data will be written to DATABUF
 * and transferred to the DATA register and converted when a Start Conversion
 * Event is issued.
 * Conversion data must be right or left adjusted according to configuration
 * settings.
 * \note To be event triggered, the enable_start_on_event must be
 * enabled in the configuration.
 *
 * \param[in] module_inst      Pointer to the DAC software device struct
 * \param[in] channel          DAC channel to write to
 * \param[in] data             Digital data to be converted
 *
 * \return Status of the operation.
 * \retval STATUS_OK           If the data was written
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 * \retval STATUS_BUSY      The DAC is busy to accept new job
 */
enum status_code dac_chan_write_job(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		uint16_t data)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	UNUSED(channel);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if(module_inst->remaining_conversions != 0 ||
			module_inst->job_status == STATUS_BUSY){
		return STATUS_BUSY;
	}

	dac_chan_write_buffer_job(module_inst, channel, &data, 1);

	return STATUS_OK;
}

/**
 * \brief Registers an asynchronous callback function with the driver.
 *
 * Registers an asynchronous callback with the DAC driver, fired when a callback
 * condition occurs.
 *
 * \param[in,out] module_inst  Pointer to the DAC software instance struct
 * \param[in]     callback    Pointer to the callback function to register
 * \param[in]     channel     Logical channel to register callback function
 * \param[in]     type        Type of callback function to register
 *
 * \return Status of the registration operation.
 * \retval STATUS_OK                   The callback was registered successfully
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 */
enum status_code dac_register_callback(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		const dac_callback_t callback,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(callback);

	UNUSED(channel);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if ((uint8_t)type < DAC_CALLBACK_N) {
		module_inst->callback[(uint8_t)type] = callback;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Unregisters an asynchronous callback function with the driver.
 *
 * Unregisters an asynchronous callback with the DAC driver, removing it
 * from the internal callback registration table.
 *
 * \param[in,out] module_inst  Pointer to the DAC software instance struct
 * \param[in]     channel     Logical channel to unregister callback function
 * \param[in]     type        Type of callback function to unregister
 *
 * \return Status of the de-registration operation.
 * \retval STATUS_OK                   The callback was unregistered successfully
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 */
enum status_code dac_unregister_callback(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(module_inst);

	UNUSED(channel);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if ((uint8_t)type < DAC_CALLBACK_N) {
		module_inst->callback[(uint8_t)type] = NULL;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Enables asynchronous callback generation for a given channel and type.
 *
 * Enables asynchronous callbacks for a given logical DAC channel and type. This
 * must be called before a DAC channel will generate callback events.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     channel     Logical channel to enable callback function
 * \param[in]     type        Type of callback function callbacks to enable
 *
 * \return Status of the callback enable operation.
 * \retval STATUS_OK                   The callback was enabled successfully
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 */
enum status_code dac_chan_enable_callback(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(module_inst);

	UNUSED(channel);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	module_inst->callback_enable[type] = true;

	return STATUS_OK;
}

/**
 * \brief Disables asynchronous callback generation for a given channel and type.
 *
 * Disables asynchronous callbacks for a given logical DAC channel and type.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     channel     Logical channel to disable callback function
 * \param[in]     type        Type of callback function callbacks to disable
 *
 * \return Status of the callback disable operation.
 * \retval STATUS_OK                   The callback was disabled successfully
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode
 */
enum status_code dac_chan_disable_callback(
		struct dac_module *const module_inst,
		const enum dac_channel channel,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(module_inst);

	UNUSED(channel);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (module_inst->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	module_inst->callback_enable[type] = false;

	return STATUS_OK;
}

/** \internal
 *  Internal handler for DAC module interrupts.
 *
 * \param[in] instance  DAC instance number
 */
static void _dac_interrupt_handler(const uint8_t instance)
{
	struct dac_module *module = _dac_instances[instance];
	Dac *const dac_hw = module->hw;

	if (dac_hw->INTFLAG.reg & DAC_INTFLAG_UNDERRUN) {
		dac_hw->INTFLAG.reg = DAC_INTFLAG_UNDERRUN;

		if ((module->callback) &&
			 (module->callback_enable[DAC_CALLBACK_DATA_UNDERRUN])){
			module->callback[DAC_CALLBACK_DATA_UNDERRUN](0);
		}
	}

	if (dac_hw->INTFLAG.reg & DAC_INTFLAG_EMPTY) {
		dac_hw->INTFLAG.reg = DAC_INTFLAG_EMPTY;

		/* If in a write buffer job */
		if (module->remaining_conversions) {

			/* Fill the data buffer with next data in write buffer */
			dac_hw->DATABUF.reg =
				module->job_buffer[module->transferred_conversions++];

			/* Write buffer size decrement */
			module->remaining_conversions --;

			/* If in a write buffer job and all the data are converted */
			if (module->remaining_conversions == 0) {
				module->job_status = STATUS_OK;

				/* Disable interrupt */
				dac_hw->INTENCLR.reg = DAC_INTENCLR_EMPTY;
				dac_hw->INTFLAG.reg = DAC_INTFLAG_EMPTY;
				system_interrupt_disable(SYSTEM_INTERRUPT_MODULE_DAC);

				if ((module->callback) &&
					 (module->callback_enable[DAC_CALLBACK_TRANSFER_COMPLETE])) {
					module->callback[DAC_CALLBACK_TRANSFER_COMPLETE](0);
				}
			}
		}

		if ((module->callback) &&
			 (module->callback_enable[DAC_CALLBACK_DATA_EMPTY])) {
			module->callback[DAC_CALLBACK_DATA_EMPTY](0);
		}
	}
}

/** Handler for the DAC hardware module interrupt. */
void DAC_Handler(void)
{
	_dac_interrupt_handler(0);
}

/**
 * \brief Gets the status of a job
 *
 * Gets the status of an ongoing or the last job.
 *
 * \param[in]  module_inst Pointer to the DAC software instance struct
 * \param[in]  channel     Logical channel to enable callback function
 *
 * \return Status of the job.
 */
enum status_code dac_chan_get_job_status(
		struct dac_module *module_inst,
		const enum dac_channel channel)
{
	/* Sanity check arguments */
	Assert(module_inst);

	UNUSED(channel);

	return module_inst->job_status;
}

/**
 * \brief Aborts an ongoing job
 *
 * Aborts an ongoing job.
 *
 * \param[in]  module_inst Pointer to the DAC software instance struct
 * \param[in]  channel     Logical channel to enable callback function
 */
void dac_chan_abort_job(
		struct dac_module *module_inst,
		const enum dac_channel channel)
{
	/* Sanity check arguments */
	Assert(module_inst);

	UNUSED(channel);

	/* Disable interrupt */
	module_inst->hw->INTFLAG.reg = DAC_INTFLAG_UNDERRUN | DAC_INTFLAG_EMPTY;
	module_inst->hw->INTENCLR.reg = DAC_INTENCLR_UNDERRUN | DAC_INTENCLR_EMPTY;

	/* Mark job as aborted */
	module_inst->job_status = STATUS_ABORTED;
	module_inst->remaining_conversions = 0;

}
