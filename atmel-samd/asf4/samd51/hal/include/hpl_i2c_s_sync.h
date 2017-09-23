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
#ifndef _HPL_I2C_S_SYNC_H_INCLUDED
#define _HPL_I2C_S_SYNC_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief I2C Slave status type
 */
typedef uint32_t i2c_s_status_t;

/**
 * \brief i2c slave device structure
 */
struct _i2c_s_sync_device {
	void *hw;
};

#include <compiler.h>

/**
 * \name HPL functions
 */

/**
 * \brief Initialize synchronous I2C slave
 *
 * This function does low level I2C configuration.
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_sync_init(struct _i2c_s_sync_device *const device, void *const hw);

/**
 * \brief Deinitialize synchronous I2C slave
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_sync_deinit(struct _i2c_s_sync_device *const device);

/**
 * \brief Enable I2C module
 *
 * This function does low level I2C enable.
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_sync_enable(struct _i2c_s_sync_device *const device);

/**
 * \brief Disable I2C module
 *
 * This function does low level I2C disable.
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_sync_disable(struct _i2c_s_sync_device *const device);

/**
 * \brief Check if 10-bit addressing mode is on
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Cheking status
 * \retval 1 10-bit addressing mode is on
 * \retval 0 10-bit addressing mode is off
 */
int32_t _i2c_s_sync_is_10bit_addressing_on(const struct _i2c_s_sync_device *const device);

/**
 * \brief Set I2C slave address
 *
 * \param[in] device The pointer to i2c slave device structure
 * \param[in] address Address to set
 *
 * \return Return 0 for success and negative value for error
 */
int32_t _i2c_s_sync_set_address(struct _i2c_s_sync_device *const device, const uint16_t address);

/**
 * \brief Write a byte to the given I2C instance
 *
 * \param[in] device The pointer to i2c slave device structure
 * \param[in] data Data to write
 */
void _i2c_s_sync_write_byte(struct _i2c_s_sync_device *const device, const uint8_t data);

/**
 * \brief Retrieve I2C slave status
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 *\return I2C slave status
 */
i2c_s_status_t _i2c_s_sync_get_status(const struct _i2c_s_sync_device *const device);

/**
 * \brief Read a byte from the given I2C instance
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Data received via I2C interface.
 */
uint8_t _i2c_s_sync_read_byte(const struct _i2c_s_sync_device *const device);

/**
 * \brief Check if I2C is ready to send next byte
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Status of the ready check.
 * \retval true if the I2C is ready to send next byte
 * \retval false if the I2C is not ready to send next byte
 */
bool _i2c_s_sync_is_byte_sent(const struct _i2c_s_sync_device *const device);

/**
 * \brief Check if there is data received by I2C
 *
 * \param[in] device The pointer to i2c slave device structure
 *
 * \return Status of the data received check.
 * \retval true if the I2C has received a byte
 * \retval false if the I2C has not received a byte
 */
bool _i2c_s_sync_is_byte_received(const struct _i2c_s_sync_device *const device);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_I2C_S_SYNC_H_INCLUDED */
