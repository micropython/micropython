/**
 * \file
 *
 * \brief I/O related functionality declaration.
 *
 * Copyright (C) 2014 - 2016 Atmel Corporation. All rights reserved.
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

#ifndef _HAL_IO_INCLUDED
#define _HAL_IO_INCLUDED

/**
 * \addtogroup doc_driver_hal_helper_io I/O Driver
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief I/O descriptor
 *
 * The I/O descriptor forward declaration.
 */
struct io_descriptor;

/**
 * \brief I/O write function pointer type
 */
typedef int32_t (*io_write_t)(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length);

/**
 * \brief I/O read function pointer type
 */
typedef int32_t (*io_read_t)(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length);

/**
 * \brief I/O descriptor
 */
struct io_descriptor {
	io_write_t write; /*! The write function pointer. */
	io_read_t  read;  /*! The read function pointer. */
};

/**
 * \brief I/O write interface
 *
 * This function writes up to \p length of bytes to a given I/O descriptor.
 * It returns the number of bytes actually write.
 *
 * \param[in] descr  An I/O descriptor to write
 * \param[in] buf    The buffer pointer to story the write data
 * \param[in] length The number of bytes to write
 *
 * \return The number of bytes written
 */
int32_t io_write(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length);

/**
 * \brief I/O read interface
 *
 * This function reads up to \p length bytes from a given I/O descriptor, and
 * stores it in the buffer pointed to by \p buf. It returns the number of bytes
 * actually read.
 *
 * \param[in] descr  An I/O descriptor to read
 * \param[in] buf    The buffer pointer to story the read data
 * \param[in] length The number of bytes to read
 *
 * \return The number of bytes actually read. This number can be less than the
 *         requested length. E.g., in a driver that uses ring buffer for
 *         reception, it may depend on the availability of data in the
 *         ring buffer.
 */
int32_t io_read(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HAL_IO_INCLUDED */
