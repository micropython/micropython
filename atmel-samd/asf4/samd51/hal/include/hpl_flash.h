/**
 * \file
 *
 * \brief FLASH related functionality declaration.
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
#ifndef _HPL_FLASH_H_INCLUDED
#define _HPL_FLASH_H_INCLUDED

/**
 * \addtogroup hpl__flash__group FLASH HPL APIs
 *
 */

/**@{*/

#include <compiler.h>
#include "hpl_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief FLASH device structure
 *
 * The FLASH device structure forward declaration.
 */
struct _flash_device;

/** The callback types */
enum _flash_cb_type { FLASH_DEVICE_CB_READY, FLASH_DEVICE_CB_ERROR, FLASH_DEVICE_CB_N };

/**
 * \brief FLASH interrupt handlers structure
 */
struct _flash_callback {
	/** Ready to accept new command handler */
	void (*ready_cb)(struct _flash_device *device);
	/** Error handler */
	void (*error_cb)(struct _flash_device *device);
};

/**
 * \brief FLASH descriptor device structure.
 */
struct _flash_device {
	struct _flash_callback flash_cb; /*!< Interrupt handers  */
	struct _irq_descriptor irq;      /*!< Interrupt descriptor */
	void *                 hw;       /*!< Hardware module instance handler */
};

/**
 * \brief Initialize FLASH.
 *
 * This function does low level FLASH configuration.
 *
 * \param[in] device The pointer to FLASH device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialize status.
 */
int32_t _flash_init(struct _flash_device *const device, void *const hw);

/**
 * \brief Deinitialize FLASH.
 *
 * \param[in] device The pointer to FLASH device instance
 */
void _flash_deinit(struct _flash_device *const device);

/**
 * \brief Reads a number of bytes in the internal Flash.
 *
 * \param[in] device         The pointer to FLASH device instance
 * \param[in]  src_addr      Source bytes address to read from flash
 * \param[out] buffer        Pointer to a buffer where the content
 *                           of the read page will be stored
 * \param[in]  length        Number of bytes to read
 */
void _flash_read(struct _flash_device *const device, const uint32_t src_addr, uint8_t *buffer, uint32_t length);

/**
 * \brief Writes a number of bytes in the internal Flash.
 *
 * \param[in] device         The pointer to FLASH device instance
 * \param[in]  dst_addr      Destination bytes address to write into flash
 * \param[in]  buffer        Pointer to buffer where the data to
 *                           write is stored
 * \param[in]  length        Number of bytes to write
 */
void _flash_write(struct _flash_device *const device, const uint32_t dst_addr, const uint8_t *buffer, uint32_t length);

/**
 * \brief Appends a number of bytes in the internal Flash.
 *
 * \param[in] device         The pointer to FLASH device instance
 * \param[in]  dst_addr      Destination bytes address to write into flash
 * \param[in]  buffer        Pointer to buffer with data to write to flash
 * \param[in]  length        Number of bytes to write
 */
void _flash_append(struct _flash_device *const device, const uint32_t dst_addr, const uint8_t *buffer, uint32_t length);

/** \brief Execute lock in the internal flash
 *  \param[in] device         The pointer to FLASH device instance
 *  \param[in]  dst_addr      Destination bytes address aligned with page
 *                            start to be locked
 *  \param[in]  page_nums     Number of pages to be locked
 *
 *  \return Real locked numbers of pages.
 */
int32_t _flash_lock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums);

/** \brief Execute unlock in the internal flash
 *  \param[in] device         The pointer to FLASH device instance
 *  \param[in]  dst_addr      Destination bytes address aligned with page
 *                            start to be unlocked
 *  \param[in]  page_nums     Number of pages to be unlocked
 *
 *  \return Real unlocked numbers of pages.
 */
int32_t _flash_unlock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums);

/** \brief check whether the region which is pointed by address
 *         is locked
 *  \param[in] device         The pointer to FLASH device instance
 *  \param[in]  dst_addr      Destination bytes address to check
 *
 *  \return The lock status of assigned address.
 */
bool _flash_is_locked(struct _flash_device *const device, const uint32_t dst_addr);

/** \brief Execute erase in the internal flash
 *  \param[in] device         The pointer to FLASH device instance
 *  \param[in]  dst_addr      Destination bytes address aligned with page
 *                            start to be erased
 *  \param[in]  page_nums     Number of pages to be erased
 */
void _flash_erase(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums);

/**
 * \brief Get the flash page size.
 *
 * \param[in] device         The pointer to FLASH device instance
 *
 * \return The flash page size
 */
uint32_t _flash_get_page_size(struct _flash_device *const device);

/**
 * \brief Get the numbers of flash page.
 *
 * \param[in] device         The pointer to FLASH device instance
 *
 * \return The flash total page numbers
 */
uint32_t _flash_get_total_pages(struct _flash_device *const device);

/**
 * \brief Get the number of wait states for read and write operations.
 *
 * \param[in] device         The pointer to FLASH device instance
 *
 * \return The number of wait states for read and write operations
 */
uint8_t _flash_get_wait_state(struct _flash_device *const device);

/**
 * \brief Set the number of wait states for read and write operations.
 *
 * \param[in] device         The pointer to FLASH device instance
 * \param[in] state The number of wait states
 *
 */
void _flash_set_wait_state(struct _flash_device *const device, uint8_t state);

/**
 * \brief Enable/disable Flash interrupt
 *
 * param[in] device The pointer to Flash device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _flash_set_irq_state(struct _flash_device *const device, const enum _flash_cb_type type, const bool state);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _HPL_FLASH_H_INCLUDED */
