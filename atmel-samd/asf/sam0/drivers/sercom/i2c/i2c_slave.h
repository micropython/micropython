/**
 * \file
 *
 * \brief SAM SERCOM I2C Slave Driver
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

#ifndef I2C_SLAVE_H_INCLUDED
#define I2C_SLAVE_H_INCLUDED

#include "i2c_common.h"
#include <sercom.h>
#include <pinmux.h>

#if I2C_SLAVE_CALLBACK_MODE == true
#  include <sercom_interrupt.h>
#endif

#ifndef PINMUX_DEFAULT
#  define PINMUX_DEFAULT 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_sercom_i2c_group
 *
 * @{
 *
 */

/**
 * \name I2C Slave Status Flags
 *
 * I<SUP>2</SUP>C slave status flags, returned by \ref i2c_slave_get_status() and cleared
 * by \ref i2c_slave_clear_status().
 * @{
 */

/** Address Match.
 * \note Should only be cleared internally by driver.
 */
#define I2C_SLAVE_STATUS_ADDRESS_MATCH     (1UL << 0)
/** Data Ready. */
#define I2C_SLAVE_STATUS_DATA_READY        (1UL << 1)
/** Stop Received. */
#define I2C_SLAVE_STATUS_STOP_RECEIVED     (1UL << 2)
/** Clock Hold.
 * \note Cannot be cleared, only valid when I2C_SLAVE_STATUS_ADDRESS_MATCH is
 * set.
 */
#define I2C_SLAVE_STATUS_CLOCK_HOLD        (1UL << 3)
/** SCL Low Timeout. */
#define I2C_SLAVE_STATUS_SCL_LOW_TIMEOUT   (1UL << 4)
/** Repeated Start.
 * \note Cannot be cleared, only valid when I2C_SLAVE_STATUS_ADDRESS_MATCH is
 * set.
 */
#define I2C_SLAVE_STATUS_REPEATED_START    (1UL << 5)
/** Received not acknowledge.
 * \note Cannot be cleared.
 */
#define I2C_SLAVE_STATUS_RECEIVED_NACK     (1UL << 6)
/** Transmit Collision. */
#define I2C_SLAVE_STATUS_COLLISION         (1UL << 7)
/** Bus error. */
#define I2C_SLAVE_STATUS_BUS_ERROR         (1UL << 8)

/** @} */

/**
 * \brief I<SUP>2</SUP>C slave packet for read/write
 *
 * Structure to be used when transferring I<SUP>2</SUP>C slave packets.
 */
struct i2c_slave_packet {
	/** Length of data array */
	uint16_t data_length;
	/** Data array containing all data to be transferred */
	uint8_t *data;
};

#if I2C_SLAVE_CALLBACK_MODE == true
 /**
 * \brief Callback types
 *
 * The available callback types for the I<SUP>2</SUP>C slave.
 */
enum i2c_slave_callback {
	/** Callback for packet write complete */
	I2C_SLAVE_CALLBACK_WRITE_COMPLETE,
	/** Callback for packet read complete */
	I2C_SLAVE_CALLBACK_READ_COMPLETE,
	/**
	 * Callback for read request from master - can be used to
	 * issue a write
	 */
	I2C_SLAVE_CALLBACK_READ_REQUEST,
	/**
	 * Callback for write request from master - can be used to issue a read
	 */
	I2C_SLAVE_CALLBACK_WRITE_REQUEST,
	/** Callback for error */
	I2C_SLAVE_CALLBACK_ERROR,
	/**
	 * Callback for error in last transfer. Discovered on a new address
	 * interrupt.
	 */
	I2C_SLAVE_CALLBACK_ERROR_LAST_TRANSFER,
#  if !defined(__DOXYGEN__)
	/** Total number of callbacks */
	_I2C_SLAVE_CALLBACK_N,
#  endif
};

#  if !defined(__DOXYGEN__)
/** Software module prototype */
struct i2c_slave_module;

/** Callback type */
typedef void (*i2c_slave_callback_t)(
		struct i2c_slave_module *const module);
#  endif
#endif

/**
 * \brief Enum for the possible SDA hold times with respect to the negative
 * edge of SCL
 *
 * Enum for the possible SDA hold times with respect to the negative edge
 * of SCL.
 */
enum i2c_slave_sda_hold_time {
	/** SDA hold time disabled */
	I2C_SLAVE_SDA_HOLD_TIME_DISABLED =
			((SERCOM_I2CS_CTRLA_SDAHOLD_Msk & ((0) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos))),
	/** SDA hold time 50ns - 100ns */
	I2C_SLAVE_SDA_HOLD_TIME_50NS_100NS =
			((SERCOM_I2CS_CTRLA_SDAHOLD_Msk & ((1) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos))),
	/** SDA hold time 300ns - 600ns */
	I2C_SLAVE_SDA_HOLD_TIME_300NS_600NS =
			((SERCOM_I2CS_CTRLA_SDAHOLD_Msk & ((2) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos))),
	/** SDA hold time 400ns - 800ns */
	I2C_SLAVE_SDA_HOLD_TIME_400NS_800NS =
			((SERCOM_I2CS_CTRLA_SDAHOLD_Msk & ((3) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos))),
};

/**
 * \brief Enum for the possible address modes
 *
 * Enum for the possible address modes.
 */
enum i2c_slave_address_mode {
	/** Address match on address_mask used as a mask to address */
	I2C_SLAVE_ADDRESS_MODE_MASK = SERCOM_I2CS_CTRLB_AMODE(0),
	/** Address math on both address and address_mask */
	I2C_SLAVE_ADDRESS_MODE_TWO_ADDRESSES = SERCOM_I2CS_CTRLB_AMODE(1),
	/**
	 * Address match on range of addresses between and including address and
	 * address_mask
	 */
	I2C_SLAVE_ADDRESS_MODE_RANGE = SERCOM_I2CS_CTRLB_AMODE(2),
};

/**
 * \brief Enum for the direction of a request
 *
 * Enum for the direction of a request.
 */
enum i2c_slave_direction {
	/** Read */
	I2C_SLAVE_DIRECTION_READ,
	/** Write */
	I2C_SLAVE_DIRECTION_WRITE,
	/** No direction */
	I2C_SLAVE_DIRECTION_NONE,
};

#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
/**
 * \brief Enum for the transfer speed
 *
 * Enum for the transfer speed.
 */
enum i2c_slave_transfer_speed {
	/** Standard-mode (Sm) up to 100KHz and Fast-mode (Fm) up to 400KHz */
	I2C_SLAVE_SPEED_STANDARD_AND_FAST = SERCOM_I2CS_CTRLA_SPEED(0),
	/** Fast-mode Plus (Fm+) up to 1MHz */
	I2C_SLAVE_SPEED_FAST_MODE_PLUS = SERCOM_I2CS_CTRLA_SPEED(1),
	/** High-speed mode (Hs-mode) up to 3.4MHz */
	I2C_SLAVE_SPEED_HIGH_SPEED = SERCOM_I2CS_CTRLA_SPEED(2),
};
#endif

/**
 * \brief SERCOM I<SUP>2</SUP>C slave driver software device instance structure.
 *
 * SERCOM I<SUP>2</SUP>C slave driver software instance structure, used to
 * retain software state information of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct i2c_slave_module {
#if !defined(__DOXYGEN__)
	/** Hardware instance initialized for the struct */
	Sercom *hw;
	/** Module lock */
	volatile bool locked;
	/** Timeout value for polled functions */
	uint16_t buffer_timeout;
#  ifdef FEATURE_I2C_10_BIT_ADDRESS
	/** Using 10-bit addressing for the slave */
	bool ten_bit_address;
#  endif
#  if I2C_SLAVE_CALLBACK_MODE == true
	/** Nack on address match */
	bool nack_on_address;
	/** Pointers to callback functions */
	volatile i2c_slave_callback_t callbacks[_I2C_SLAVE_CALLBACK_N];
	/** Mask for registered callbacks */
	volatile uint8_t registered_callback;
	/** Mask for enabled callbacks */
	volatile uint8_t enabled_callback;
	/** The total number of bytes to transfer */
	volatile uint16_t buffer_length;
	/**
	 * Counter used for bytes left to send in write and to count number of
	 * obtained bytes in read
	 */
	uint16_t buffer_remaining;
	/** Data buffer for packet write and read */
	volatile uint8_t *buffer;
	/** Save direction of request from master. 1 = read, 0 = write. */
	volatile enum i2c_transfer_direction transfer_direction;
	/** Status for status read back in error callback */
	volatile enum status_code status;
#  endif
#endif
};

/**
 * \brief Configuration structure for the I<SUP>2</SUP>C slave device
 *
 * This is the configuration structure for the I<SUP>2</SUP>C slave device. It is used
 * as an argument for \ref i2c_slave_init to provide the desired
 * configurations for the module. The structure should be initialized using the
 * \ref i2c_slave_get_config_defaults.
 */
struct i2c_slave_config {
	/** Set to enable the SCL low timeout */
	bool enable_scl_low_timeout;
	/** SDA hold time with respect to the negative edge of SCL */
	enum i2c_slave_sda_hold_time sda_hold_time;
	/** Timeout to wait for master in polled functions */
	uint16_t buffer_timeout;
	/** Addressing mode */
	enum i2c_slave_address_mode address_mode;
	/** Address or upper limit of address range */
	uint16_t address;
	/** Address mask, second address, or lower limit of address range */
	uint16_t address_mask;
#ifdef FEATURE_I2C_10_BIT_ADDRESS
	/** Enable 10-bit addressing */
	bool ten_bit_address;
#endif
	/**
	 * Enable general call address recognition (general call address
	 * is defined as 0000000 with direction bit 0).
	 */
	bool enable_general_call_address;

#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
	/** Transfer speed mode */
	enum i2c_slave_transfer_speed transfer_speed;
#endif

#if I2C_SLAVE_CALLBACK_MODE == true
	/**
	 * Enable NACK on address match (this can be changed after initialization
	 * via the \ref i2c_slave_enable_nack_on_address and
	 * \ref i2c_slave_disable_nack_on_address functions).
	 */
	bool enable_nack_on_address;
#endif
	/** GCLK generator to use as clock source */
	enum gclk_generator generator_source;
	/** Set to keep module active in sleep modes */
	bool run_in_standby;
	/** PAD0 (SDA) pinmux */
	uint32_t pinmux_pad0;
	/** PAD1 (SCL) pinmux */
	uint32_t pinmux_pad1;
	/** Set to enable SCL low time-out */
	bool scl_low_timeout;
#ifdef FEATURE_I2C_SCL_STRETCH_MODE
	/** Set to enable SCL stretch only after ACK bit (required for high speed) */
	bool scl_stretch_only_after_ack_bit;
#endif
#ifdef FEATURE_I2C_SCL_EXTEND_TIMEOUT
	/** Set to enable slave SCL low extend time-out */
	bool slave_scl_low_extend_timeout;
#endif
};


/**
 * \name Lock/Unlock
 * @{
 */

/**
 * \brief Attempt to get lock on driver instance
 *
 * This function checks the instance's lock, which indicates whether or not it
 * is currently in use, and sets the lock if it was not already set.
 *
 * The purpose of this is to enable exclusive access to driver instances, so
 * that, e.g., transactions by different services will not interfere with each
 * other.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 *
 * \retval STATUS_OK If the module was locked
 * \retval STATUS_BUSY If the module was already locked
 */
static inline enum status_code i2c_slave_lock(
		struct i2c_slave_module *const module)
{
	enum status_code status;

	system_interrupt_enter_critical_section();

	if (module->locked) {
		status = STATUS_BUSY;
	} else {
		module->locked = true;
		status = STATUS_OK;
	}

	system_interrupt_leave_critical_section();

	return status;
}

/**
 * \brief Unlock driver instance
 *
 * This function clears the instance lock, indicating that it is available for
 * use.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 *
 * \retval STATUS_OK If the module was locked
 * \retval STATUS_BUSY If the module was already locked
 */
static inline void i2c_slave_unlock(struct i2c_slave_module *const module)
{
	module->locked = false;
}

/** @} */

/**
 * \name Configuration and Initialization
 * @{
 */

/**
 * \brief Returns the synchronization status of the module
 *
 * Returns the synchronization status of the module.
 *
 * \param[out] module  Pointer to software module structure
 *
 * \return Status of the synchronization.
 * \retval true   Module is busy synchronizing
 * \retval false  Module is not synchronizing
 */
static inline bool i2c_slave_is_syncing(
		const struct i2c_slave_module *const module)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	/* Return sync status */
#if defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1)
	return (i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_SYNCBUSY);
#elif defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2)
	return (i2c_hw->SYNCBUSY.reg & SERCOM_I2CS_SYNCBUSY_MASK);
#else
#  error Unknown SERCOM SYNCBUSY scheme!
#endif
}

#if !defined(__DOXYGEN__)
/**
 * \internal Wait for hardware module to sync
 *
 * \param[in]  module  Pointer to software module structure
 */
static void _i2c_slave_wait_for_sync(
		const struct i2c_slave_module *const module)
{
	/* Sanity check */
	Assert(module);

	while (i2c_slave_is_syncing(module)) {
		/* Wait for I<SUP>2</SUP>C module to sync */
	}
}
#endif

///@cond INTERNAL
/**
 * \internal Workaround for errata 13574
 * Instead set ACK/NACK of CTRLB
 *
 * This errata exist in part revisions of SAM D20/D21/D09
 * D10/D11/L21/L22/DA1/C20/C21/R30, but workaround can be works in all
 * revision of those device. As this function operation
 * should be use less cpu time as possible, so caller
 * function can ignore to check revision number, and use
 * this workaround in all revision of those device.
 *
 * \param[in,out] module  Pointer to software module structure
 * \param[in] send_ack true send ACK, false send NACK
 */
static inline void _i2c_slave_set_ctrlb_ackact(
		struct i2c_slave_module *const module,
		bool send_ack)
{
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

#if (SAMD20 || SAMD21 || SAMD09 || SAMD10 || SAMD11 || SAML21 || SAMDA1 ||  \
		SAML22 || SAMC20 || SAMC21 || SAMR30)
	/* Workaround, Following two write are atomic */
	system_interrupt_enter_critical_section();
	i2c_hw->STATUS.reg = 0;

	if (send_ack == true) {
		i2c_hw->CTRLB.reg = 0;
	}
	else {
		i2c_hw->CTRLB.reg = SERCOM_I2CS_CTRLB_ACKACT;
	}
	system_interrupt_leave_critical_section();
#else
	/* Normal operation */
	if (send_ack == true) {
		i2c_hw->CTRLB.reg &= ~SERCOM_I2CS_CTRLB_ACKACT;
	}
	else {
		i2c_hw->CTRLB.reg |= SERCOM_I2CS_CTRLB_ACKACT;
	}
#endif
	return;
}

/**
 * \internal Workaround for SAM0 errata 13574,
 * instead Set CMD3 of CTRLB
 *
 * This errata exist in part revisions of SAM D20/D21/D09
 * D10/D11/L21/DA1/C20/C21/R30, but workaround can be works in all
 * revision of those device. As this function operation
 * should be use less cpu time as possible, so caller
 * function can ignore to check revision number, and use
 * this workaround in all revision of those device.
 *
 * \param[in,out] module  Pointer to software module structure
 */
static inline void _i2c_slave_set_ctrlb_cmd3(
		struct i2c_slave_module *const module)
{
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

#if (SAMD20 || SAMD21 || SAMD09 || SAMD10 || SAMD11 || SAML21 || SAML22 || \
		SAMDA1 || SAMC20 || SAMC21 || SAMR30)
	/* Workaround */
	/*
	 * Below code instead i2c_hw->CTRLB.reg = SERCOM_I2CS_CTRLB_CMD(0x3);
	 * CMD=0x3 clears all interrupts, so to keep the result similar
	 * PREC is cleared if it was set
	 */
	if (i2c_hw->INTFLAG.bit.PREC) {
		i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
	}
	i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
#else
	/* Normal operation */
	i2c_hw->CTRLB.reg = SERCOM_I2CS_CTRLB_CMD(0x3);
#endif
	return;
}
///@endcond

/**
 * \brief Gets the I<SUP>2</SUP>C slave default configurations
 *
 * This will initialize the configuration structure to known default values.
 *
 * The default configuration is as follows:
 * - Disable SCL low timeout
 * - 300ns - 600ns SDA hold time
 * - Buffer timeout = 65535
 * - Address with mask
 * - Address = 0
 * - Address mask = 0 (one single address)
 * - General call address disabled
 * - Address nack disabled if the interrupt driver is used
 * - GCLK generator 0
 * - Do not run in standby
 * - PINMUX_DEFAULT for SERCOM pads
 *
 * Those default configuration only available if the device supports it:
 * - Not using 10-bit addressing
 * - Standard-mode and Fast-mode transfer speed
 * - SCL stretch disabled
 * - Slave SCL low extend time-out disabled
 *
 * \param[out] config  Pointer to configuration structure to be initialized
 */
static inline void i2c_slave_get_config_defaults(
		struct i2c_slave_config *const config)
{
	/*Sanity check argument */
	Assert(config);
	config->enable_scl_low_timeout = false;
	config->sda_hold_time = I2C_SLAVE_SDA_HOLD_TIME_300NS_600NS;
	config->buffer_timeout = 65535;
	config->address_mode = I2C_SLAVE_ADDRESS_MODE_MASK;
	config->address = 0;
	config->address_mask = 0;
#ifdef FEATURE_I2C_10_BIT_ADDRESS
	config->ten_bit_address = false;
#endif
	config->enable_general_call_address = false;
#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
	config->transfer_speed = I2C_SLAVE_SPEED_STANDARD_AND_FAST;
#endif
#if I2C_SLAVE_CALLBACK_MODE == true
	config->enable_nack_on_address = false;
#endif
	config->generator_source = GCLK_GENERATOR_0;
	config->run_in_standby = false;
	config->pinmux_pad0 = PINMUX_DEFAULT;
	config->pinmux_pad1 = PINMUX_DEFAULT;
	config->scl_low_timeout  = false;
#ifdef FEATURE_I2C_SCL_STRETCH_MODE
	config->scl_stretch_only_after_ack_bit = false;
#endif
#ifdef FEATURE_I2C_SCL_EXTEND_TIMEOUT
	config->slave_scl_low_extend_timeout   = false;
#endif
}

enum status_code i2c_slave_init(struct i2c_slave_module *const module,
		Sercom *const hw,
		const struct i2c_slave_config *const config);

/**
 * \brief Enables the I<SUP>2</SUP>C module
 *
 * This will enable the requested I<SUP>2</SUP>C module.
 *
 * \param[in]  module Pointer to the software module struct
 */
static inline void i2c_slave_enable(
		const struct i2c_slave_module *const module)
{
	/* Sanity check of arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Enable global interrupt for module */
	system_interrupt_enable(_sercom_get_interrupt_vector(module->hw));
#endif

	/* Wait for module to sync */
	_i2c_slave_wait_for_sync(module);

	/* Enable module */
	i2c_hw->CTRLA.reg |= SERCOM_I2CS_CTRLA_ENABLE;
}


/**
 * \brief Disables the I<SUP>2</SUP>C module
 *
 * This will disable the I<SUP>2</SUP>C module specified in the provided software module
 * structure.
 *
 * \param[in]  module  Pointer to the software module struct
 */
static inline void i2c_slave_disable(
		const struct i2c_slave_module *const module)
{
	/* Sanity check of arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

#if I2C_SLAVE_CALLBACK_MODE == true
	/* Disable interrupts */
	i2c_hw->INTENCLR.reg = SERCOM_I2CS_INTENSET_PREC |
			SERCOM_I2CS_INTENSET_AMATCH | SERCOM_I2CS_INTENSET_DRDY;

	/* Clear interrupt flags */
	i2c_hw->INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC | SERCOM_I2CS_INTFLAG_AMATCH |
			SERCOM_I2CS_INTFLAG_DRDY;

	/* Disable global interrupt for module */
	system_interrupt_disable(_sercom_get_interrupt_vector(module->hw));
#endif

	/* Wait for module to sync */
	_i2c_slave_wait_for_sync(module);

	/* Disable module */
	i2c_hw->CTRLA.reg &= ~SERCOM_I2CS_CTRLA_ENABLE;
}

void i2c_slave_reset(
		struct i2c_slave_module *const module);

/** @} */

/**
 * \name Read and Write
 * @{
 */

enum status_code i2c_slave_write_packet_wait(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet);
enum status_code i2c_slave_read_packet_wait(
		struct i2c_slave_module *const module,
		struct i2c_slave_packet *const packet);
enum i2c_slave_direction i2c_slave_get_direction_wait(
		struct i2c_slave_module *const module);

/** @} */

/**
 * \name Status Management
 * @{
 */
uint32_t i2c_slave_get_status(
		struct i2c_slave_module *const module);
void i2c_slave_clear_status(
		struct i2c_slave_module *const module,
		uint32_t status_flags);
/** @} */

#ifdef FEATURE_I2C_DMA_SUPPORT
/**
 * \name SERCOM I2C slave with DMA Interfaces
 * @{
 */

/**
 * \brief Read SERCOM I<SUP>2</SUP>C interrupt status.
 *
 * Read I<SUP>2</SUP>C interrupt status for DMA transfer.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 *
 */
static inline uint8_t i2c_slave_dma_read_interrupt_status(struct i2c_slave_module *const module)
{
	return (uint8_t)module->hw->I2CS.INTFLAG.reg;
}

/**
 * \brief Write SERCOM I<SUP>2</SUP>C interrupt status.
 *
 * Write I<SUP>2</SUP>C interrupt status for DMA transfer.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 * \param[in] flag Interrupt flag status
 *
 */
static inline void i2c_slave_dma_write_interrupt_status(struct i2c_slave_module *const module,
		uint8_t flag)
{
	module->hw->I2CS.INTFLAG.reg = flag;
}

/** @} */
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* I2C_SLAVE_H_INCLUDED */
