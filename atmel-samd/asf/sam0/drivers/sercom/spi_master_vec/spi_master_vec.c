/**
 * \file
 *
 * \brief SERCOM SPI master with vectored I/O driver implementation
 *
 * Copyright (C) 2013-2016 Atmel Corporation. All rights reserved.
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

#include "spi_master_vec.h"
#include <sercom_interrupt.h>
#include <system.h>
#include <system_interrupt.h>

/**
 * \ingroup asfdoc_sam0_sercom_spi_master_vec_group
 *
 * @{
 */

/**
 * \name Internal functions
 * @{
 */

/** \cond INTERNAL */

static void _spi_master_vec_int_handler(uint8_t sercom_index);

/**
 * \brief Wait for SERCOM SPI to synchronize
 *
 * \param[in] sercom_spi SERCOM SPI to check for synchronization.
 *
 * \note The implementation of this function depends on the SERCOM revision.
 */
static inline void _spi_master_vec_wait_for_sync(SercomSpi *const sercom_spi)
{
#if defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1)
	while (sercom_spi->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY) {
		/* Intentionally left empty */
	}
#elif defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2)
	while (sercom_spi->SYNCBUSY.reg) {
		/* Intentionally left empty */
	}
#else
#  error Unknown SERCOM SYNCBUSY scheme!
#endif
}

/**
 * \brief Pin MUX configuration helper
 *
 * \param[in] pinmux Pin MUX setting to apply. Special values:
 * \arg \c PINMUX_UNUSED to do nothing.
 * \arg \c PINMUX_DEFAULT to use default pin MUX for the SERCOM pad.
 * \param[in] sercom,padnum SERCOM pad specification, for \ref PINMUX_DEFAULT.
 */
static inline void _spi_master_vec_pinmux_helper(uint32_t pinmux,
		Sercom *const sercom, uint8_t padnum)
{
	struct system_pinmux_config pin_conf;

	if (pinmux == PINMUX_DEFAULT) {
		pinmux = _sercom_get_default_pad(sercom, padnum);
	}

	if (pinmux == PINMUX_UNUSED) {
		return;
	}

	system_pinmux_get_config_defaults(&pin_conf);
	pin_conf.mux_position = pinmux & 0xFFFF;
	system_pinmux_pin_set_config(pinmux >> 16, &pin_conf);
};

/** \endcond */

/** @} */

/**
 * \brief Initialize hardware and driver instance
 *
 * This function configures the clock system for the specified SERCOM module,
 * sets up the related pins and their MUX, initializes the SERCOM in SPI master
 * mode, and prepares the driver instance for operation.
 *
 * \pre \ref system_init() must have been called prior to this function.
 *
 * The SERCOM SPI module is left disabled after initialization, and must be
 * enabled with \ref spi_master_vec_enable() before a transfer can be done.
 *
 * \param[out] module Driver instance to initialize.
 * \param[in,out] sercom SERCOM module to initialize and associate driver
 * instance with.
 * \param[in] config Driver configuration to use.
 *
 * \return Status of initialization.
 * \retval STATUS_OK if initialization succeeded.
 * \retval STATUS_ERR_INVALID_ARG if driver has been misconfigured.
 */
enum status_code spi_master_vec_init(struct spi_master_vec_module *const module,
		Sercom *const sercom, const struct spi_master_vec_config *const config)
{
	Assert(module);
	Assert(sercom);
	Assert(config);

	enum status_code status;
	SercomSpi *const spi_hw = &(sercom->SPI);
	struct system_gclk_chan_config gclk_chan_conf;
	uint16_t tmp_baud;
	uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom *)spi_hw);
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
	uint32_t pm_index = sercom_index + MCLK_APBCMASK_SERCOM0_Pos;
#else
	uint32_t pm_index = sercom_index + PM_APBCMASK_SERCOM0_Pos;
#endif
	uint32_t gclk_index = sercom_index + SERCOM0_GCLK_ID_CORE;
	uint32_t gclk_hz;

	module->sercom = sercom;

	/* Enable clock for the module interface */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << pm_index);

	/* Set up the GCLK for the module */
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = config->gclk_generator;
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
	sercom_set_gclk_generator(config->gclk_generator, false);

	_spi_master_vec_wait_for_sync(spi_hw);

	/* Set up the SERCOM SPI module as master */
	spi_hw->CTRLA.reg = SERCOM_SPI_CTRLA_MODE(0x3);
	spi_hw->CTRLA.reg |= (uint32_t)config->mux_setting
			| config->transfer_mode
			| config->data_order
			| ((config->run_in_standby || system_is_debugger_present()) ?
					SERCOM_SPI_CTRLA_RUNSTDBY : 0);

	/* Get baud value from configured baudrate and internal clock rate */
	gclk_hz = system_gclk_chan_get_hz(gclk_index);
	status = _sercom_get_sync_baud_val(config->baudrate, gclk_hz, &tmp_baud);

	if (status != STATUS_OK) {
		/* Baud rate calculation error! */
		return STATUS_ERR_INVALID_ARG;
	}

	spi_hw->BAUD.reg = (uint8_t)tmp_baud;

	/* Configure the pin multiplexers */
	_spi_master_vec_pinmux_helper(config->pinmux_pad0, sercom, 0);
	_spi_master_vec_pinmux_helper(config->pinmux_pad3, sercom, 3);

	/* SERCOM PAD1 and PAD2 are used for slave SS.
	 * This is a SPI master driver, so control of slave SS must be left to
	 * the PORT module, i.e., peripheral MUX should not be set for that pin.
	 * DOPO controls which PAD is used for slave SS:
	 * If DOPO is odd, SERCOM_PAD1 is SS: SERCOM_PAD2 can be MUXed.
	 * If DOPO is even, SERCOM_PAD2 is SS: SERCOM_PAD1 can be MUXed.
	 */
	if (config->mux_setting & (1 << SERCOM_SPI_CTRLA_DOPO_Pos)) {
		_spi_master_vec_pinmux_helper(config->pinmux_pad2, sercom, 2);
	} else {
		_spi_master_vec_pinmux_helper(config->pinmux_pad1, sercom, 1);
	}

	/* Initialize our instance and register interrupt handler + data */
	module->rx_bufdesc_ptr = NULL;
	module->tx_bufdesc_ptr = NULL;
	module->direction = SPI_MASTER_VEC_DIRECTION_IDLE;
	module->status = STATUS_OK;
#ifdef CONF_SPI_MASTER_VEC_OS_SUPPORT
	CONF_SPI_MASTER_VEC_CREATE_SEMAPHORE(module->busy_semaphore);
#endif

	_sercom_set_handler(sercom_index, _spi_master_vec_int_handler);
	_sercom_instances[sercom_index] = module;

	return STATUS_OK;
}

/**
 * \brief Enable the SERCOM SPI module
 *
 * This function must be called after \ref spi_master_vec_init() before a
 * transfer can be started.
 *
 * \param[in,out] module Driver instance to operate on.
 */
void spi_master_vec_enable(const struct spi_master_vec_module *const module)
{
	Assert(module);
	Assert(module->sercom);

	SercomSpi *const spi_hw = &(module->sercom->SPI);

	spi_hw->INTENCLR.reg = SERCOM_SPI_INTFLAG_DRE | SERCOM_SPI_INTFLAG_RXC
			| SERCOM_SPI_INTFLAG_TXC;

	_spi_master_vec_wait_for_sync(spi_hw);

	spi_hw->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;

	system_interrupt_enable(_sercom_get_interrupt_vector(module->sercom));
}

/**
 * \brief Disable the SERCOM SPI module
 *
 * \param[in,out] module Driver instance to operate on.
 */
void spi_master_vec_disable(struct spi_master_vec_module *const module)
{
	Assert(module);
	Assert(module->sercom);

	SercomSpi *const spi_hw = &(module->sercom->SPI);

	system_interrupt_disable(_sercom_get_interrupt_vector(module->sercom));

	_spi_master_vec_wait_for_sync(spi_hw);

	spi_hw->INTENCLR.reg = SERCOM_SPI_INTENCLR_MASK;
	spi_hw->INTFLAG.reg = SERCOM_SPI_INTFLAG_MASK;
	spi_hw->CTRLB.reg = 0;
	spi_hw->CTRLA.reg &= ~SERCOM_SPI_CTRLA_ENABLE;
	module->rx_bufdesc_ptr = NULL;
	module->tx_bufdesc_ptr = NULL;
	module->direction = SPI_MASTER_VEC_DIRECTION_IDLE;
	module->status = STATUS_OK;
}

/**
 * \brief Reset the SERCOM SPI module
 *
 * This function will disable and reset the SPI module to its power on default
 * values.
 *
 * \param[in,out] module Pointer to a driver instance.
 */
void spi_master_vec_reset(struct spi_master_vec_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->sercom);

	SercomSpi *const spi_hw = &(module->sercom->SPI);

	/* Disable the module */
	spi_master_vec_disable(module);

	_spi_master_vec_wait_for_sync(spi_hw);

	/* Software reset the module */
	spi_hw->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;

#ifdef CONF_SPI_MASTER_VEC_OS_SUPPORT
	CONF_SPI_MASTER_VEC_DELETE_SEMAPHORE(module->busy_semaphore);
#endif
}

/**
 * \brief Start vectored I/O transfer
 *
 * This function initiates a uni- or bidirectional SPI transfer from/to any
 * number of data buffers. The transfer is interrupt-driven and will run in the
 * background, after this function has returned.
 *
 * The buffers to transmit from or receive into must be described in arrays of
 * buffer descriptors. These arrays \e must end with descriptors that specify
 * zero buffer length. The first descriptor in an array can \e not specify zero
 * length. The number of bytes to transmit and to receive do not have to be
 * equal.
 *
 * If the address for a receive buffer is set to \c NULL, the received bytes
 * corresponding to that buffer descriptor will be discarded. This is useful if
 * slave is already set up to transfer a number of bytes, but the master has no
 * available buffer to receive them into. As an example, to receive the two
 * first bytes and discard the 128 following, the buffer descriptors could be:
\code
	struct spi_master_vec_bufdesc rx_buffers[3] = {
		// Read two status bytes
		{.data = status_buffer, .length = 2},
		// Discard 128 data bytes
		{.data = NULL, .length = 128},
		// End of reception
		{.length = 0},
	};
\endcode
 *
 * To initiate a unidirectional transfer, pass \c NULL as the address of either
 * buffer descriptor array, like this:
\code
	// Transmit some buffers
	spi_master_vec_transceive_buffer_job(&module, tx_buffers, NULL);

	// Receive some buffers
	spi_master_vec_transceive_buffer_job(&module, NULL, rx_buffers);
\endcode
 *
 * \pre \ref spi_master_vec_init() and \ref spi_master_vec_enable() must have
 * been called before this function.
 *
 * \param[in,out] module Driver instance to operate on.
 * \param[in] tx_bufdescs address of buffer descriptor array for bytes to
 * transmit.
 * \arg \c NULL if the transfer is a simplex read.
 * \param[in,out] rx_bufdescs address of buffer descriptor array for storing
 * received bytes.
 * \arg \c NULL if the transfer is a simplex write.
 *
 * \return Status of transfer start.
 * \retval STATUS_OK if transfer was started.
 * \retval STATUS_BUSY if a transfer is already on-going.
 */
enum status_code spi_master_vec_transceive_buffer_job(
		struct spi_master_vec_module *const module,
		struct spi_master_vec_bufdesc tx_bufdescs[],
		struct spi_master_vec_bufdesc rx_bufdescs[])
{
	Assert(module);
	Assert(module->sercom);
	Assert(tx_bufdescs || rx_bufdescs);

	SercomSpi *const spi_hw = &(module->sercom->SPI);
	uint32_t tmp_ctrlb;
	uint8_t tmp_intenset;

	system_interrupt_enter_critical_section();
	if (module->status == STATUS_BUSY) {
		system_interrupt_leave_critical_section();
		return STATUS_BUSY;
	} else {
		module->status = STATUS_BUSY;
		system_interrupt_leave_critical_section();
	}

#ifdef CONF_SPI_MASTER_VEC_OS_SUPPORT
	CONF_SPI_MASTER_VEC_TAKE_SEMAPHORE(module->busy_semaphore);
#endif

	module->tx_bufdesc_ptr = tx_bufdescs;
	module->rx_bufdesc_ptr = rx_bufdescs;

	if (tx_bufdescs && rx_bufdescs) {
		Assert(tx_bufdescs[0].length);
		Assert(rx_bufdescs[0].length);

		module->direction = SPI_MASTER_VEC_DIRECTION_BOTH;
		module->tx_length = tx_bufdescs[0].length;
		module->tx_head_ptr = tx_bufdescs[0].data;
		module->rx_length = rx_bufdescs[0].length;
		module->rx_head_ptr = rx_bufdescs[0].data;
		module->tx_lead_on_rx = 0;
		tmp_ctrlb = SERCOM_SPI_CTRLB_RXEN;
		tmp_intenset = SERCOM_SPI_INTFLAG_DRE | SERCOM_SPI_INTFLAG_RXC;
	} else {
		if (tx_bufdescs) {
			Assert(tx_bufdescs[0].length);

			module->direction = SPI_MASTER_VEC_DIRECTION_WRITE;
			module->tx_length = tx_bufdescs[0].length;
			module->tx_head_ptr = tx_bufdescs[0].data;
			tmp_ctrlb = 0;
			tmp_intenset = SERCOM_SPI_INTFLAG_DRE;
		} else {
			Assert(rx_bufdescs[0].length);

			module->direction = SPI_MASTER_VEC_DIRECTION_READ;
			module->rx_length = rx_bufdescs[0].length;
			module->rx_head_ptr = rx_bufdescs[0].data;
			module->tx_lead_on_rx = 0;
			tmp_ctrlb = SERCOM_SPI_CTRLB_RXEN;
			tmp_intenset = SERCOM_SPI_INTFLAG_DRE | SERCOM_SPI_INTFLAG_RXC;
		}
	}

	/* Ensure the SERCOM is sync'ed before writing these registers */
	_spi_master_vec_wait_for_sync(spi_hw);

	spi_hw->CTRLB.reg = tmp_ctrlb;
	spi_hw->INTENSET.reg = tmp_intenset;

	return STATUS_OK;
}

/**
 * \brief Interrupt handler
 *
 * \param[in] sercom_index SERCOM instance number passed from the master SERCOM
 * driver.
 */
static void _spi_master_vec_int_handler(uint8_t sercom_index)
{
	struct spi_master_vec_module *const module =
			_sercom_instances[sercom_index];
	enum _spi_master_vec_direction dir = module->direction;
	SercomSpi *const spi_hw = &(module->sercom->SPI);
	uint8_t int_status;

	int_status = spi_hw->INTFLAG.reg ;
    int_status &= spi_hw->INTENSET.reg;

	if (int_status & SERCOM_SPI_INTFLAG_DRE) {
		uint_fast8_t tx_lead_on_rx = module->tx_lead_on_rx;

		/* If TX is ahead of RX by 2+ bytes, allow RX to catch up.
		 * Note: will only happen _once_ per READ or BOTH.
		 */
		if ((tx_lead_on_rx >= 2) && (dir != SPI_MASTER_VEC_DIRECTION_WRITE)) {
			Assert((dir == SPI_MASTER_VEC_DIRECTION_READ)
					|| (dir == SPI_MASTER_VEC_DIRECTION_BOTH));
			Assert(int_status & SERCOM_SPI_INTFLAG_RXC);
		/* Otherwise, we can send more bytes */
		} else {
			module->tx_lead_on_rx = ++tx_lead_on_rx;

			/* If doing a READ, just send 0 to trigger the transfer */
			if (dir == SPI_MASTER_VEC_DIRECTION_READ) {
				uint32_t tx_lead_limit;

				spi_hw->DATA.reg = 0;

check_for_read_end:
				/* With current TX'ed bytes, will we get the last RX byte?
				 * If so, we can disable the DRE interrupt to stop transmitting.
				 *
				 * Since a buffer can have minimum 1 byte length, this check is
				 * simplified by first checking if the RX end is so close that
				 * the max. 2 byte lead of TX may actually fill the buffers.
				 */
				tx_lead_limit = (module->rx_bufdesc_ptr + 1)->length;

				if (!tx_lead_limit || !(module->rx_bufdesc_ptr + 2)->length) {
					tx_lead_limit += module->rx_length;

					if (tx_lead_on_rx >= tx_lead_limit) {
						spi_hw->INTENCLR.reg = SERCOM_SPI_INTFLAG_DRE;
					}
				}
			/* For WRITE and BOTH, output current byte */
			} else {
				spi_master_vec_buflen_t tx_length;
				uint8_t *tx_head_ptr;

				tx_head_ptr = module->tx_head_ptr;
				spi_hw->DATA.reg = *(tx_head_ptr++);

				/* Check if this was the last byte to send */
				tx_length = module->tx_length - 1;

				if (tx_length) {
					module->tx_head_ptr = tx_head_ptr;
					module->tx_length = tx_length;
				} else {
				/* Any more buffers left to send, perhaps? */
					tx_length = (++module->tx_bufdesc_ptr)->length;

					if (tx_length) {
						module->tx_head_ptr = module->tx_bufdesc_ptr->data;
						module->tx_length = tx_length;
					} else {
						if (dir == SPI_MASTER_VEC_DIRECTION_WRITE) {
						/* Disable DRE and enable TXC to end WRITE */
							spi_hw->INTENCLR.reg = SERCOM_SPI_INTFLAG_DRE;
							spi_hw->INTENSET.reg = SERCOM_SPI_INTFLAG_TXC;
						} else {
						/* For BOTH, check if we still have bytes to read */
							dir = SPI_MASTER_VEC_DIRECTION_READ;
							module->direction = dir;
							goto check_for_read_end;
						}
					}
				}
			}
		}
	}

	/* For READ and BOTH, store the received byte */
	if (int_status & SERCOM_SPI_INTFLAG_RXC) {
		spi_master_vec_buflen_t rx_length;
		uint8_t *rx_head_ptr;

		rx_head_ptr = module->rx_head_ptr;
		if (rx_head_ptr != NULL) {
			*(rx_head_ptr++) = spi_hw->DATA.reg;
		} else {
			uint8_t dummy;
			dummy = spi_hw->DATA.reg;
			UNUSED(dummy);
		}
		module->tx_lead_on_rx--;

		/* Check if this was the last byte to receive */
		rx_length = module->rx_length - 1;

		if (rx_length) {
			module->rx_head_ptr = rx_head_ptr;
			module->rx_length = rx_length;
		} else {
			/* Any more buffers left to receive into? */
			rx_length = (++module->rx_bufdesc_ptr)->length;

			if (rx_length) {
				module->rx_head_ptr = module->rx_bufdesc_ptr->data;
				module->rx_length = rx_length;
			} else {
				/* Disable the SPI receiver (instant effect) and RX interrupt */
				spi_hw->CTRLB.reg = 0;
				spi_hw->INTENCLR.reg = SERCOM_SPI_INTFLAG_RXC;

				if (dir == SPI_MASTER_VEC_DIRECTION_READ) {
					/* If doing READ, end the transaction here */
					dir = SPI_MASTER_VEC_DIRECTION_IDLE;
					module->direction = dir;
					module->status = STATUS_OK;
#ifdef CONF_SPI_MASTER_VEC_OS_SUPPORT
					CONF_SPI_MASTER_VEC_GIVE_SEMAPHORE_FROM_ISR(module->busy_semaphore);
#endif
				} else {
					/* If doing BOTH, change direction to WRITE */
					dir = SPI_MASTER_VEC_DIRECTION_WRITE;
					module->direction = dir;
				}
			}
		}
	}

	/* For WRITE */
	if (int_status & SERCOM_SPI_INTFLAG_TXC) {
		/* End transaction here, since last byte has been sent */
		spi_hw->INTENCLR.reg = SERCOM_SPI_INTFLAG_TXC;

		dir = SPI_MASTER_VEC_DIRECTION_IDLE;
		module->direction = dir;
		module->status = STATUS_OK;
#ifdef CONF_SPI_MASTER_VEC_OS_SUPPORT
		CONF_SPI_MASTER_VEC_GIVE_SEMAPHORE_FROM_ISR(module->busy_semaphore);
#endif
	}
}

/**
 * @}
 */