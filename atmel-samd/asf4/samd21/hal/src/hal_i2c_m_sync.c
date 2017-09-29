/**
 * \file
 *
 * \brief I/O I2C related functionality implementation.
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
#include <hal_i2c_m_sync.h>
#include <utils.h>
#include <utils_assert.h>

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

/**
 * \brief Sync version of I2C I/O read
 */
static int32_t i2c_m_sync_read(struct io_descriptor *io, uint8_t *buf, const uint16_t n)
{
	struct i2c_m_sync_desc *i2c = CONTAINER_OF(io, struct i2c_m_sync_desc, io);
	struct _i2c_m_msg       msg;
	int32_t                 ret;

	msg.addr   = i2c->slave_addr;
	msg.len    = n;
	msg.flags  = I2C_M_STOP | I2C_M_RD;
	msg.buffer = buf;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret) {
		return ret;
	}

	return n;
}

/**
 * \brief Sync version of I2C I/O write
 */
static int32_t i2c_m_sync_write(struct io_descriptor *io, const uint8_t *buf, const uint16_t n)
{
	struct i2c_m_sync_desc *i2c = CONTAINER_OF(io, struct i2c_m_sync_desc, io);
	struct _i2c_m_msg       msg;
	int32_t                 ret;

	msg.addr   = i2c->slave_addr;
	msg.len    = n;
	msg.flags  = I2C_M_STOP;
	msg.buffer = (uint8_t *)buf;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret) {
		return ret;
	}

	return n;
}

/**
 * \brief Sync version of i2c initialize
 */
int32_t i2c_m_sync_init(struct i2c_m_sync_desc *i2c, void *hw)
{
	int32_t init_status;
	ASSERT(i2c);

	init_status = _i2c_m_sync_init(&i2c->device, hw);
	if (init_status) {
		return init_status;
	}

	/* Init I/O */
	i2c->io.read  = i2c_m_sync_read;
	i2c->io.write = i2c_m_sync_write;

	return ERR_NONE;
}

/**
 * \brief deinitialize
 */
int32_t i2c_m_sync_deinit(struct i2c_m_sync_desc *i2c)
{
	int32_t status;
	ASSERT(i2c);

	status = _i2c_m_sync_deinit(&i2c->device);
	if (status) {
		return status;
	}

	i2c->io.read  = NULL;
	i2c->io.write = NULL;

	return ERR_NONE;
}

/**
 * \brief Sync version of i2c enable
 */
int32_t i2c_m_sync_enable(struct i2c_m_sync_desc *i2c)
{
	return _i2c_m_sync_enable(&i2c->device);
}

/**
 * \brief Sync version of i2c disable
 */
int32_t i2c_m_sync_disable(struct i2c_m_sync_desc *i2c)
{
	return _i2c_m_sync_disable(&i2c->device);
}

/**
 * \brief Sync version of i2c set slave address
 */
int32_t i2c_m_sync_set_slaveaddr(struct i2c_m_sync_desc *i2c, int16_t addr, int32_t addr_len)
{
	return i2c->slave_addr = (addr & 0x3ff) | (addr_len & I2C_M_TEN);
}

/**
 * \brief Sync version of i2c set baudrate
 */
int32_t i2c_m_sync_set_baudrate(struct i2c_m_sync_desc *i2c, uint32_t clkrate, uint32_t baudrate)
{
	return _i2c_m_sync_set_baudrate(&i2c->device, clkrate, baudrate);
}

/**
 * \brief Sync version of i2c write command
 */
int32_t i2c_m_sync_cmd_write(struct i2c_m_sync_desc *i2c, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	struct _i2c_m_msg msg;
	int32_t           ret;

	msg.addr   = i2c->slave_addr;
	msg.len    = 1;
	msg.flags  = 0;
	msg.buffer = &reg;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret != 0) {
		/* error occurred */
		return ret;
	}

	msg.flags  = I2C_M_STOP;
	msg.buffer = buffer;
	msg.len    = length;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret != 0) {
		/* error occurred */
		return ret;
	}

	return ERR_NONE;
}

/**
 * \brief Sync version of i2c read command
 */
int32_t i2c_m_sync_cmd_read(struct i2c_m_sync_desc *i2c, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	struct _i2c_m_msg msg;
	int32_t           ret;

	msg.addr   = i2c->slave_addr;
	msg.len    = 1;
	msg.flags  = 0;
	msg.buffer = &reg;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret != 0) {
		/* error occurred */
		return ret;
	}

	msg.flags  = I2C_M_STOP | I2C_M_RD;
	msg.buffer = buffer;
	msg.len    = length;

	ret = _i2c_m_sync_transfer(&i2c->device, &msg);

	if (ret != 0) {
		/* error occurred */
		return ret;
	}

	return ERR_NONE;
}

/**
 * \brief Sync version of i2c transfer command
 */
int32_t i2c_m_sync_transfer(struct i2c_m_sync_desc *const i2c, struct _i2c_m_msg *msg)
{
	return _i2c_m_sync_transfer(&i2c->device, msg);
}

/**
 * \brief Sync version of i2c send stop condition command
 */
int32_t i2c_m_sync_send_stop(struct i2c_m_sync_desc *const i2c)
{
	return _i2c_m_sync_send_stop(&i2c->device);
}

/**
 * \brief Retrieve I/O descriptor
 */
int32_t i2c_m_sync_get_io_descriptor(struct i2c_m_sync_desc *const i2c, struct io_descriptor **io)
{
	*io = &i2c->io;
	return ERR_NONE;
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t i2c_m_sync_get_version(void)
{
	return DRIVER_VERSION;
}
