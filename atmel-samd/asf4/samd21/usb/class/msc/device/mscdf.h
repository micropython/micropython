/**
 * \file
 *
 * \brief USB Device Stack MSC Function Definition.
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef USBDF_MSC_H_
#define USBDF_MSC_H_

#include "usbdc.h"
#include "usb_protocol_msc.h"
#include "spc_protocol.h"
#include "sbc_protocol.h"

/** MSC Class Callback Type */
enum mscdf_cb_type {
	MSCDF_CB_INQUIRY_DISK,
	MSCDF_CB_GET_DISK_CAPACITY,
	MSCDF_CB_START_READ_DISK,
	MSCDF_CB_START_WRITE_DISK,
	MSCDF_CB_EJECT_DISK,
	MSCDF_CB_TEST_DISK_READY,
	MSCDF_CB_XFER_BLOCKS_DONE
};

/** MSC Inquiry Disk Callback. */
typedef uint8_t *(*mscdf_inquiry_disk_t)(uint8_t);

/** MSC Get Disk Capacity Callback. */
typedef uint8_t *(*mscdf_get_disk_capacity_t)(uint8_t);

/** MSC Read Data From Disk Callback. */
typedef int32_t (*mscdf_start_read_disk_t)(uint8_t, uint32_t, uint32_t);

/** MSC Write Data To Disk Callback. */
typedef int32_t (*mscdf_start_write_disk_t)(uint8_t, uint32_t, uint32_t);

/** MSC Eject Disk Callback. */
typedef int32_t (*mscdf_eject_disk_t)(uint8_t);

/** MSC Test Disk Ready Callback. */
typedef int32_t (*mscdf_test_disk_ready_t)(uint8_t);

/** MSC Tansfer Block Done Callback. */
typedef int32_t (*mscdf_xfer_blocks_done_t)(uint8_t);

/**
 * \brief Initialize the USB MSC Function Driver
 * \param[in] max_lun max logic unit support
 * \return Operation status.
 */
int32_t mscdf_init(uint8_t max_lun);

/**
 * \brief Deinitialize the USB MSC Function Driver
 * \return Operation status.
 */
int32_t mscdf_deinit(void);

/**
 * \brief USB MSC Function Register Callback
 * \param[in] cb_type Callback type of MSC Function
 * \param[in] func Pointer to callback function
 * \return Operation status.
 */
int32_t mscdf_register_callback(enum mscdf_cb_type cb_type, FUNC_PTR func);

/**
 * \brief Check whether MSC Function is enabled
 * \return Operation status.
 * \return true MSC Function is enabled
 * \return false MSC Function is disabled
 */
bool mscdf_is_enabled(void);

/**
 * \brief USB MSC multi-blocks data transfer between USB and Memory.
 * \param[in] rd true read disk command, false write disk command.
 * \param[in] blk_addr transfer block address.
 * \param[in] blk_cnt transfer block count. It is regarded as disk
 *            writing done when input blk_cnt as zero.
 * \return Operation status.
 */
int32_t mscdf_xfer_blocks(bool rd, uint8_t *blk_addr, uint32_t blk_cnt);

/**
 * \brief Return version
 */
uint32_t mscdf_get_version(void);

#endif /* USBDF_MSC_H_ */
