/**
 * \file
 *
 * \brief I2C Master Hardware Proxy Layer(HPL) declaration.
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
#ifndef _HPL_I2C_M_ASYNC_H_INCLUDED
#define _HPL_I2C_M_ASYNC_H_INCLUDED

#include "hpl_i2c_m_sync.h"
#include "hpl_irq.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief i2c master callback names
 */
enum _i2c_m_async_callback_type {
	I2C_M_ASYNC_DEVICE_ERROR,
	I2C_M_ASYNC_DEVICE_TX_COMPLETE,
	I2C_M_ASYNC_DEVICE_RX_COMPLETE
};

struct _i2c_m_async_device;

typedef void (*_i2c_complete_cb_t)(struct _i2c_m_async_device *i2c_dev);
typedef void (*_i2c_error_cb_t)(struct _i2c_m_async_device *i2c_dev, int32_t errcode);

/**
 * \brief i2c callback pointers structure
 */
struct _i2c_m_async_callback {
	_i2c_error_cb_t    error;
	_i2c_complete_cb_t tx_complete;
	_i2c_complete_cb_t rx_complete;
};

/**
 * \brief i2c device structure
 */
struct _i2c_m_async_device {
	struct _i2c_m_service        service;
	void *                       hw;
	struct _i2c_m_async_callback cb;
	struct _irq_descriptor       irq;
};

/**
 * \name HPL functions
 */

/**
 * \brief Initialize I2C in interrupt mode
 *
 * This function does low level I2C configuration.
 *
 * \param[in] i2c_dev The pointer to i2c interrupt device structure
 * \param[in] hw The pointer to hardware instance
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_init(struct _i2c_m_async_device *const i2c_dev, void *const hw);

/**
 * \brief Deinitialize I2C in interrupt mode
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_deinit(struct _i2c_m_async_device *const i2c_dev);

/**
 * \brief Enable I2C module
 *
 * This function does low level I2C enable.
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_enable(struct _i2c_m_async_device *const i2c_dev);

/**
 * \brief Disable I2C module
 *
 * This function does low level I2C disable.
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_disable(struct _i2c_m_async_device *const i2c_dev);

/**
 * \brief Transfer data by I2C
 *
 * This function does low level I2C data transfer.
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 * \param[in] msg The pointer to i2c msg structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_transfer(struct _i2c_m_async_device *const i2c_dev, struct _i2c_m_msg *msg);

/**
 * \brief Set baud rate of I2C
 *
 * This function does low level I2C set baud rate.
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 * \param[in] clkrate The clock rate(KHz) input to i2c module
 * \param[in] baudrate The demand baud rate(KHz) of i2c module
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_set_baudrate(struct _i2c_m_async_device *const i2c_dev, uint32_t clkrate, uint32_t baudrate);

/**
 * \brief Register callback to I2C
 *
 * This function does low level I2C callback register.
 *
 * \param[in] i2c_dev The pointer to i2c device structure
 * \param[in] cb_type The callback type request
 * \param[in] func The callback function pointer
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_m_async_register_callback(struct _i2c_m_async_device *i2c_dev, enum _i2c_m_async_callback_type cb_type,
                                       FUNC_PTR func);

/**
 * \brief Generate stop condition on the I2C bus
 *
 * This function will generate a stop condition on the I2C bus
 *
 * \param[in] i2c_m_async_descriptor An i2c descriptor which is used to communicate through I2C
 *
 * \return Operation status
 * \retval 0 Operation executed successfully
 * \retval <0 Operation failed
 */
int32_t _i2c_m_async_send_stop(struct _i2c_m_async_device *const i2c_dev);

/**
 * \brief Returns the number of bytes left or not used in the I2C message buffer
 *
 * This function will return the number of bytes left (not written to the bus) or still free
 * (not received from the bus) in the message buffer, depending on direction of transmission.
 *
 * \param[in] i2c_m_async_descriptor An i2c descriptor which is used to communicate through I2C
 *
 * \return Number of bytes or error code
 * \retval >0 Positive number indicating bytes left
 * \retval 0  Buffer is full/empty depending on direction
 * \retval <0 Error code
 */
int32_t _i2c_m_async_get_bytes_left(struct _i2c_m_async_device *const i2c_dev);

/**
 * \brief Enable/disable I2C master interrupt
 *
 * param[in] device The pointer to I2C master device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _i2c_m_async_set_irq_state(struct _i2c_m_async_device *const device, const enum _i2c_m_async_callback_type type,
                                const bool state);

#ifdef __cplusplus
}
#endif

#endif
