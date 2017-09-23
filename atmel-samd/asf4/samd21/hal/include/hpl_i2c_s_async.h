/**
 * \file
 *
 * \brief I2C Slave Hardware Proxy Layer(HPL) declaration.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
#ifndef _HPL_I2C_S_ASYNC_H_INCLUDED
#define _HPL_I2C_S_ASYNC_H_INCLUDED

#include "hpl_i2c_s_sync.h"
#include "hpl_irq.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief i2c callback types
 */
enum _i2c_s_async_callback_type { I2C_S_DEVICE_ERROR, I2C_S_DEVICE_TX, I2C_S_DEVICE_RX_COMPLETE };

/**
 * \brief Forward declaration of I2C Slave device
 */
struct _i2c_s_async_device;

/**
 * \brief i2c slave callback function type
 */
typedef void (*_i2c_s_async_cb_t)(struct _i2c_s_async_device *device);

/**
 * \brief i2c slave callback pointers structure
 */
struct _i2c_s_async_callback {
	void (*error)(struct _i2c_s_async_device *const device);
	void (*tx)(struct _i2c_s_async_device *const device);
	void (*rx_done)(struct _i2c_s_async_device *const device, const uint8_t data);
};

/**
 * \brief i2c slave device structure
 */
struct _i2c_s_async_device {
	void *                       hw;
	struct _i2c_s_async_callback cb;
	struct _irq_descriptor       irq;
};

/**
 * \name HPL functions
 */

/**
 * \brief Initialize asynchronous I2C slave
 *
 * This function does low level I2C configuration.
 *
 * \param[in] device The pointer to i2c interrupt device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_init(struct _i2c_s_async_device *const device, void *const hw);

/**
 * \brief Deinitialize asynchronous I2C in interrupt mode
 *
 * \param[in] device The pointer to i2c device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_deinit(struct _i2c_s_async_device *const device);

/**
 * \brief Enable I2C module
 *
 * This function does low level I2C enable.
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_enable(struct _i2c_s_async_device *const device);

/**
 * \brief Disable I2C module
 *
 * This function does low level I2C disable.
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_disable(struct _i2c_s_async_device *const device);

/**
 * \brief Check if 10-bit addressing mode is on
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Cheking status
 * \retval 1 10-bit addressing mode is on
 * \retval 0 10-bit addressing mode is off
 */
int32_t _i2c_s_async_is_10bit_addressing_on(const struct _i2c_s_async_device *const device);

/**
 * \brief Set I2C slave address
 *
 * \param[in] device The pointer to i2c slave device structure
 * \param[in] address Address to set
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_set_address(struct _i2c_s_async_device *const device, const uint16_t address);

/**
 * \brief Write a byte to the given I2C instance
 *
 * \param[in] device The pointer to i2c slave device structure
 * \param[in] data Data to write
 */
void _i2c_s_async_write_byte(struct _i2c_s_async_device *const device, const uint8_t data);

/**
 * \brief Retrieve I2C slave status
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 *\return I2C slave status
 */
i2c_s_status_t _i2c_s_async_get_status(const struct _i2c_s_async_device *const device);

/**
 * \brief Abort data transmission
 *
 * \param[in] device The pointer to i2c device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_async_abort_transmission(const struct _i2c_s_async_device *const device);

/**
 * \brief Enable/disable I2C slave interrupt
 *
 * param[in] device The pointer to I2C slave device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] disable Enable or disable
 */
int32_t _i2c_s_async_set_irq_state(struct _i2c_s_async_device *const device, const enum _i2c_s_async_callback_type type,
                                   const bool disable);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_I2C_S_ASYNC_H_INCLUDED */
