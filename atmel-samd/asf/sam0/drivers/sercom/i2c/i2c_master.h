/**
 * \file
 *
 * \brief SAM SERCOM I2C Master Driver
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

#ifndef I2C_MASTER_H_INCLUDED
#define I2C_MASTER_H_INCLUDED

#include "i2c_common.h"
#include <sercom.h>
#include <pinmux.h>

#if I2C_MASTER_CALLBACK_MODE == true
#  include <sercom_interrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PINMUX_DEFAULT
#  define PINMUX_DEFAULT 0
#endif

/**
 * \addtogroup asfdoc_sam0_sercom_i2c_group
 *
 * @{
 */

/**
 * \brief I<SUP>2</SUP>C master packet for read/write
 *
 * Structure to be used when transferring I<SUP>2</SUP>C master packets.
 */
struct i2c_master_packet {
	/** Address to slave device  */
	uint16_t address;
	/** Length of data array */
	uint16_t data_length;
	/** Data array containing all data to be transferred */
	uint8_t *data;
	/** Use 10-bit addressing. Set to false if the feature is not supported by the device  */
	bool ten_bit_address;
	/** Use high speed transfer. Set to false if the feature is not supported by the device */
	bool high_speed;
	/** High speed mode master code (0000 1XXX), valid when high_speed is true */
	uint8_t hs_master_code;
};

/** \brief Interrupt flags
 *
 * Flags used when reading or setting interrupt flags.
 */
enum i2c_master_interrupt_flag {
	/** Interrupt flag used for write */
	I2C_MASTER_INTERRUPT_WRITE = 0,
	/** Interrupt flag used for read */
	I2C_MASTER_INTERRUPT_READ  = 1,
};

/**
 * \brief Values for hold time after start bit.
 *
 * Values for the possible I<SUP>2</SUP>C master mode SDA internal hold times after start
 * bit has been sent.
 */
enum i2c_master_start_hold_time {
	/** Internal SDA hold time disabled */
	I2C_MASTER_START_HOLD_TIME_DISABLED = SERCOM_I2CM_CTRLA_SDAHOLD(0),
	/** Internal SDA hold time 50ns - 100ns */
	I2C_MASTER_START_HOLD_TIME_50NS_100NS = SERCOM_I2CM_CTRLA_SDAHOLD(1),
	/** Internal SDA hold time 300ns - 600ns */
	I2C_MASTER_START_HOLD_TIME_300NS_600NS = SERCOM_I2CM_CTRLA_SDAHOLD(2),
	/** Internal SDA hold time 400ns - 800ns */
	I2C_MASTER_START_HOLD_TIME_400NS_800NS = SERCOM_I2CM_CTRLA_SDAHOLD(3),
};

/**
 * \brief Values for inactive bus time-out.
 *
 * If the inactive bus time-out is enabled and the bus is inactive for
 * longer than the time-out setting, the bus state logic will be set to idle.
 */
enum i2c_master_inactive_timeout {
	/** Inactive bus time-out disabled */
	I2C_MASTER_INACTIVE_TIMEOUT_DISABLED = SERCOM_I2CM_CTRLA_INACTOUT(0),
	/** Inactive bus time-out 5-6 SCL cycle time-out */
	I2C_MASTER_INACTIVE_TIMEOUT_55US = SERCOM_I2CM_CTRLA_INACTOUT(1),
	/** Inactive bus time-out 10-11 SCL cycle time-out */
	I2C_MASTER_INACTIVE_TIMEOUT_105US = SERCOM_I2CM_CTRLA_INACTOUT(2),
	/** Inactive bus time-out 20-21 SCL cycle time-out */
	I2C_MASTER_INACTIVE_TIMEOUT_205US = SERCOM_I2CM_CTRLA_INACTOUT(3),
};

/**
 * \brief I<SUP>2</SUP>C frequencies
 *
 * Values for I<SUP>2</SUP>C speeds supported by the module. The driver
 * will also support setting any other value, in which case set
 * the value in the \ref i2c_master_config at desired value divided by 1000.
 *
 * Example: If 10KHz operation is required, give baud_rate in the configuration
 * structure the value 10.
 */
enum i2c_master_baud_rate {
	/** Baud rate at 100KHz (Standard-mode) */
	I2C_MASTER_BAUD_RATE_100KHZ = 100,
	/** Baud rate at 400KHz (Fast-mode) */
	I2C_MASTER_BAUD_RATE_400KHZ = 400,
#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
	/** Baud rate at 1MHz (Fast-mode Plus) */
	I2C_MASTER_BAUD_RATE_1000KHZ = 1000,
	/** Baud rate at 3.4MHz (High-speed mode) */
	I2C_MASTER_BAUD_RATE_3400KHZ = 3400,
#endif
};

#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
/**
 * \brief Enum for the transfer speed
 *
 * Enum for the transfer speed.
 */
enum i2c_master_transfer_speed {
	/** Standard-mode (Sm) up to 100KHz and Fast-mode (Fm) up to 400KHz */
	I2C_MASTER_SPEED_STANDARD_AND_FAST = SERCOM_I2CM_CTRLA_SPEED(0),
	/** Fast-mode Plus (Fm+) up to 1MHz */
	I2C_MASTER_SPEED_FAST_MODE_PLUS = SERCOM_I2CM_CTRLA_SPEED(1),
	/** High-speed mode (Hs-mode) up to 3.4MHz */
	I2C_MASTER_SPEED_HIGH_SPEED = SERCOM_I2CM_CTRLA_SPEED(2),
};
#endif

#if I2C_MASTER_CALLBACK_MODE == true
/**
 * \brief Callback types
 *
 * The available callback types for the I<SUP>2</SUP>C master module.
 */
enum i2c_master_callback {
	/** Callback for packet write complete */
	I2C_MASTER_CALLBACK_WRITE_COMPLETE = 0,
	/** Callback for packet read complete */
	I2C_MASTER_CALLBACK_READ_COMPLETE  = 1,
	/** Callback for error */
	I2C_MASTER_CALLBACK_ERROR          = 2,
#  if !defined(__DOXYGEN__)
	/** Total number of callbacks */
	_I2C_MASTER_CALLBACK_N             = 3,
#  endif
};

#  if !defined(__DOXYGEN__)
/* Prototype for software module */
struct i2c_master_module;

typedef void (*i2c_master_callback_t)(
		struct i2c_master_module *const module);
#  endif
#endif

/**
 * \brief SERCOM I<SUP>2</SUP>C Master driver software device instance structure.
 *
 * SERCOM I<SUP>2</SUP>C Master driver software instance structure, used to
 * retain software state information of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct i2c_master_module {
#if !defined(__DOXYGEN__)
	/** Hardware instance initialized for the struct */
	Sercom *hw;
	/** Module lock */
	volatile bool locked;
	/** Unknown bus state timeout */
	uint16_t unknown_bus_state_timeout;
	/** Buffer write timeout value */
	uint16_t buffer_timeout;
	/** If true, stop condition will be sent after a read/write */
	bool send_stop;
	/** If true, nack signal will be sent after a read/write */
	bool send_nack;
#  if I2C_MASTER_CALLBACK_MODE == true
	/** Pointers to callback functions */
	volatile i2c_master_callback_t callbacks[_I2C_MASTER_CALLBACK_N];
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
	volatile uint16_t buffer_remaining;
	/** Data buffer for packet write and read */
	volatile uint8_t *buffer;
	/** Save direction of async request. 1 = read, 0 = write */
	volatile enum i2c_transfer_direction transfer_direction;
	/** Status for status read back in error callback */
	volatile enum status_code status;
#  endif
#endif
};

/**
 * \brief Configuration structure for the I<SUP>2</SUP>C Master device
 *
 * This is the configuration structure for the I<SUP>2</SUP>C Master device. It
 * is used as an argument for \ref i2c_master_init to provide the desired
 * configurations for the module. The structure should be initialized using the
 * \ref i2c_master_get_config_defaults.
 */
struct i2c_master_config {
	/** Baud rate (in KHz) for I<SUP>2</SUP>C operations in
	 * standard-mode, Fast-mode, and Fast-mode Plus Transfers,
	 * \ref i2c_master_baud_rate */
	uint32_t baud_rate;
#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
	/** Baud rate (in KHz) for I<SUP>2</SUP>C operations in
	 * High-speed mode, \ref i2c_master_baud_rate */
	uint32_t baud_rate_high_speed;
	/** Transfer speed mode */
	enum i2c_master_transfer_speed transfer_speed;
#endif
	/** GCLK generator to use as clock source */
	enum gclk_generator generator_source;
	/** Bus hold time after start signal on data line */
	enum i2c_master_start_hold_time start_hold_time;
	/** Unknown bus state \ref asfdoc_sam0_sercom_i2c_unknown_bus_timeout "timeout" */
	uint16_t unknown_bus_state_timeout;
	/** Timeout for packet write to wait for slave */
	uint16_t buffer_timeout;
	/** Set to keep module active in sleep modes */
	bool run_in_standby;
	/** PAD0 (SDA) pinmux */
	uint32_t pinmux_pad0;
	/** PAD1 (SCL) pinmux */
	uint32_t pinmux_pad1;
	/** Set to enable SCL low time-out */
	bool scl_low_timeout;
	/** Inactive bus time out */
	enum i2c_master_inactive_timeout inactive_timeout;
#ifdef FEATURE_I2C_SCL_STRETCH_MODE
	/** Set to enable SCL stretch only after ACK bit (required for high speed) */
	bool scl_stretch_only_after_ack_bit;
#endif
#ifdef FEATURE_I2C_SCL_EXTEND_TIMEOUT
	/** Set to enable slave SCL low extend time-out */
	bool slave_scl_low_extend_timeout;
	/** Set to enable maser SCL low extend time-out */
	bool master_scl_low_extend_timeout;
#endif
	/** Get more accurate BAUD, considering rise time(required for standard-mode and Fast-mode) */
	uint16_t sda_scl_rise_time_ns;
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
static inline enum status_code i2c_master_lock(
		struct i2c_master_module *const module)
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
static inline void i2c_master_unlock(struct i2c_master_module *const module)
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
 * \param[in]  module  Pointer to software module structure
 *
 * \return Status of the synchronization.
 * \retval true   Module is busy synchronizing
 * \retval false  Module is not synchronizing
 */
static inline bool i2c_master_is_syncing (
		const struct i2c_master_module *const module)
{
	/* Sanity check */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_hw = &(module->hw->I2CM);

#if defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1)
	return (i2c_hw->STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
#elif defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2)
	return (i2c_hw->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
#else
#  error Unknown SERCOM SYNCBUSY scheme!
#endif
}

#if !defined(__DOXYGEN__)
/**
 * \internal
 * Wait for hardware module to sync
 *
 * \param[in]  module  Pointer to software module structure
 */
static void _i2c_master_wait_for_sync(
		const struct i2c_master_module *const module)
{
	/* Sanity check */
	Assert(module);

	while (i2c_master_is_syncing(module)) {
		/* Wait for I2C module to sync. */
	}
}
#endif

/**
 * \brief Gets the I<SUP>2</SUP>C master default configurations
 *
 * Use to initialize the configuration structure to known default values.
 *
 * The default configuration is as follows:
 * - Baudrate 100KHz
 * - GCLK generator 0
 * - Do not run in standby
 * - Start bit hold time 300ns - 600ns
 * - Buffer timeout = 65535
 * - Unknown bus status timeout = 65535
 * - Do not run in standby
 * - PINMUX_DEFAULT for SERCOM pads
 *
 * Those default configuration only available if the device supports it:
 * - High speed baudrate 3.4MHz
 * - Standard-mode and Fast-mode transfer speed
 * - SCL stretch disabled
 * - Slave SCL low extend time-out disabled
 * - Master SCL low extend time-out disabled
 *
 * \param[out] config  Pointer to configuration structure to be initiated
 */
static inline void i2c_master_get_config_defaults(
		struct i2c_master_config *const config)
{
	/*Sanity check argument */
	Assert(config);
	config->baud_rate        = I2C_MASTER_BAUD_RATE_100KHZ;
#ifdef FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
	config->baud_rate_high_speed = I2C_MASTER_BAUD_RATE_3400KHZ;
	config->transfer_speed       = I2C_MASTER_SPEED_STANDARD_AND_FAST;
#endif
	config->generator_source = GCLK_GENERATOR_0;
	config->run_in_standby   = false;
	config->start_hold_time  = I2C_MASTER_START_HOLD_TIME_300NS_600NS;
	config->buffer_timeout   = 65535;
	config->unknown_bus_state_timeout = 65535;
	config->pinmux_pad0      = PINMUX_DEFAULT;
	config->pinmux_pad1      = PINMUX_DEFAULT;
	config->scl_low_timeout  = false;
	config->inactive_timeout = I2C_MASTER_INACTIVE_TIMEOUT_DISABLED;
#ifdef FEATURE_I2C_SCL_STRETCH_MODE
	config->scl_stretch_only_after_ack_bit = false;
#endif
#ifdef FEATURE_I2C_SCL_EXTEND_TIMEOUT
	config->slave_scl_low_extend_timeout   = false;
	config->master_scl_low_extend_timeout  = false;
#endif
	/* The typical value is 215ns */
	config->sda_scl_rise_time_ns = 215;
}

enum status_code i2c_master_init(
		struct i2c_master_module *const module,
		Sercom *const hw,
		const struct i2c_master_config *const config);

/**
 * \brief Enables the I<SUP>2</SUP>C module
 *
 * Enables the requested I<SUP>2</SUP>C module and set the bus state to IDLE
 * after the specified \ref asfdoc_sam0_sercom_i2c_timeout "timeout" period if no
 * stop bit is detected.
 *
 * \param[in]  module  Pointer to the software module struct
 */
static inline void i2c_master_enable(
		const struct i2c_master_module *const module)
{
	/* Sanity check of arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

	/* Timeout counter used to force bus state */
	uint32_t timeout_counter = 0;

	/* Wait for module to sync */
	_i2c_master_wait_for_sync(module);

	/* Enable module */
	i2c_module->CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;

#if I2C_MASTER_CALLBACK_MODE == true
	/* Enable module interrupts */
	system_interrupt_enable(_sercom_get_interrupt_vector(module->hw));
#endif
	/* Start timeout if bus state is unknown */
	while (!(i2c_module->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1))) {
		timeout_counter++;
		if(timeout_counter >= (module->unknown_bus_state_timeout)) {
			/* Timeout, force bus state to idle */
			i2c_module->STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);
			/* Workaround #1 */
			return;
		}
	}
}

/**
 * \brief Disable the I<SUP>2</SUP>C module
 *
 * Disables the requested I<SUP>2</SUP>C module.
 *
 * \param[in]  module  Pointer to the software module struct
 */
static inline void i2c_master_disable(
		const struct i2c_master_module *const module)
{
	/* Sanity check of arguments */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

#if I2C_MASTER_CALLBACK_MODE == true
		/* Disable module interrupts */
		system_interrupt_disable(_sercom_get_interrupt_vector(module->hw));
#endif

	/* Wait for module to sync */
	_i2c_master_wait_for_sync(module);

	/* Disbale interrupt */
	i2c_module->INTENCLR.reg = SERCOM_I2CM_INTENCLR_MASK;
	/* Clear interrupt flag */
	i2c_module->INTFLAG.reg = SERCOM_I2CM_INTFLAG_MASK;

	/* Disable module */
	i2c_module->CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;

}

void i2c_master_reset(struct i2c_master_module *const module);

/** @} */

/**
* \name Read and Write
* @{
*/

enum status_code i2c_master_read_packet_wait(
		struct i2c_master_module *const module,
		struct i2c_master_packet *const packet);

enum status_code i2c_master_read_packet_wait_no_stop(
		struct i2c_master_module *const module,
		struct i2c_master_packet *const packet);

enum status_code i2c_master_write_packet_wait(
		struct i2c_master_module *const module,
		struct i2c_master_packet *const packet);

enum status_code i2c_master_write_packet_wait_no_stop(
		struct i2c_master_module *const module,
		struct i2c_master_packet *const packet);

void i2c_master_send_stop(struct i2c_master_module *const module);

void i2c_master_send_nack(struct i2c_master_module *const module);

enum status_code i2c_master_read_byte(
		struct i2c_master_module *const module,
		uint8_t *byte);

enum status_code i2c_master_write_byte(
		struct i2c_master_module *const module,
		uint8_t byte);

enum status_code i2c_master_read_packet_wait_no_nack(
		struct i2c_master_module *const module,
		struct i2c_master_packet *const packet);

/** @} */

#ifdef FEATURE_I2C_DMA_SUPPORT
/**
* \name SERCOM I2C Master with DMA Interfaces
* @{
*/

/**
 * \brief Set I<SUP>2</SUP>C for DMA transfer with slave address and transfer size.
 *
 * This function will set the slave address, transfer size and enable the auto transfer
 * mode for DMA.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 * \param[in] addr I<SUP>2</SUP>C slave address
 * \param[in] length I<SUP>2</SUP>C transfer length with DMA
 * \param[in] direction I<SUP>2</SUP>C transfer direction
 *
 */
static inline void i2c_master_dma_set_transfer(struct i2c_master_module *const module,
		uint16_t addr, uint8_t length, enum i2c_transfer_direction direction)
{
	module->hw->I2CM.ADDR.reg =
		SERCOM_I2CM_ADDR_ADDR(addr<<1) |
		SERCOM_I2CM_ADDR_LENEN |
		SERCOM_I2CM_ADDR_LEN(length) |
		direction;
}

/** @} */
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* I2C_MASTER_H_INCLUDED */
