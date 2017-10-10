
/**
* \file
*
* \brief Non-Volatile Memory Controller
*
* Copyright (C) 2015-2017 Atmel Corporation. All rights reserved.
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
* DAMAGES (INCLUDING, BUT NOT LIMIT ED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/

#include <hpl_flash.h>
#include <hpl_user_area.h>
#include <string.h>
#include <utils_assert.h>
#include <utils.h>
#include <hpl_nvmctrl_config.h>

#define NVM_MEMORY ((volatile uint16_t *)FLASH_ADDR)

/**
 * \brief NVM configuration type
 */
struct nvm_configuration {
	hri_nvmctrl_ctrlb_reg_t ctrlb; /*!< Control B Register */
};

/**
 * \brief Array of NVM configurations
 */
static struct nvm_configuration _nvm
    = {(CONF_NVM_CACHE << NVMCTRL_CTRLB_CACHEDIS_Pos) | (CONF_NVM_READ_MODE << NVMCTRL_CTRLB_READMODE_Pos)
       | (CONF_NVM_SLEEPPRM << NVMCTRL_CTRLB_SLEEPPRM_Pos)};

/*!< Pointer to hpl device */
static struct _flash_device *_nvm_dev = NULL;

static void _flash_erase_row(void *const hw, const uint32_t dst_addr);
static void _flash_program(void *const hw, const uint32_t dst_addr, const uint8_t *buffer, const uint16_t size);

/**
 * \brief Initialize NVM
 */
int32_t _flash_init(struct _flash_device *const device, void *const hw)
{
	ASSERT(device && (hw == NVMCTRL));
	uint32_t ctrlb;

	device->hw = hw;
	ctrlb      = _nvm.ctrlb & ~(NVMCTRL_CTRLB_RWS_Msk | NVMCTRL_CTRLB_MANW);
	ctrlb |= hri_nvmctrl_get_CTRLB_reg(device->hw, NVMCTRL_CTRLB_RWS_Msk | NVMCTRL_CTRLB_MANW);
	hri_nvmctrl_write_CTRLB_reg(device->hw, ctrlb);

	_nvm_dev = device;
	NVIC_DisableIRQ(NVMCTRL_IRQn);
	NVIC_ClearPendingIRQ(NVMCTRL_IRQn);
	NVIC_EnableIRQ(NVMCTRL_IRQn);
	return ERR_NONE;
}

/**
 * \brief De-initialize NVM
 */
void _flash_deinit(struct _flash_device *const device)
{
	device->hw = NULL;
	NVIC_DisableIRQ(NVMCTRL_IRQn);
}

/**
 * \brief Get the flash page size.
 */
uint32_t _flash_get_page_size(struct _flash_device *const device)
{
	(void)device;
	return (uint32_t)NVMCTRL_PAGE_SIZE;
}

/**
 * \brief Get the numbers of flash page.
 */
uint32_t _flash_get_total_pages(struct _flash_device *const device)
{
	(void)device;
	return (uint32_t)FLASH_NB_OF_PAGES;
}

/**
 * \brief Get the number of wait states for read and write operations.
 */
uint8_t _flash_get_wait_state(struct _flash_device *const device)
{
	return hri_nvmctrl_get_CTRLB_reg(device->hw, NVMCTRL_CTRLB_RWS_Msk);
}

/**
 * \brief Set the number of wait states for read and write operations.
 */
void _flash_set_wait_state(struct _flash_device *const device, uint8_t state)
{
	hri_nvmctrl_write_CTRLB_RWS_bf(device->hw, state);
}

/**
 * \brief Reads a number of bytes to a page in the internal Flash.
 */
void _flash_read(struct _flash_device *const device, const uint32_t src_addr, uint8_t *buffer, uint32_t length)
{
	uint32_t nvm_address = src_addr / 2;
	uint32_t i;
	uint16_t data;

	/* Check if the module is busy */
	while (!hri_nvmctrl_get_interrupt_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	/* Clear flags */
	hri_nvmctrl_clear_STATUS_reg(device->hw, NVMCTRL_STATUS_MASK);

	/* Check whether byte address is word-aligned*/
	if (src_addr % 2) {
		data      = NVM_MEMORY[nvm_address++];
		buffer[0] = data >> 8;
		i         = 1;
	} else {
		i = 0;
	}

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment */
	while (i < length) {
		data      = NVM_MEMORY[nvm_address++];
		buffer[i] = (data & 0xFF);
		if (i < (length - 1)) {
			buffer[i + 1] = (data >> 8);
		}
		i += 2;
	}
}

/**
 * \brief Writes a number of bytes to a page in the internal Flash.
 */
void _flash_write(struct _flash_device *const device, const uint32_t dst_addr, const uint8_t *buffer, uint32_t length)
{
	uint8_t  tmp_buffer[NVMCTRL_ROW_PAGES][NVMCTRL_PAGE_SIZE];
	uint32_t row_start_addr, row_end_addr;
	uint32_t i, j, k;
	uint32_t wr_start_addr = dst_addr;

	do {
		row_start_addr = wr_start_addr & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);
		row_end_addr   = row_start_addr + NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE - 1;

		/* store the erase data into temp buffer before write */
		for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
			_flash_read(device, row_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
		}

		/* temp buffer update */
		j = (wr_start_addr - row_start_addr) / NVMCTRL_PAGE_SIZE;
		k = wr_start_addr - row_start_addr - j * NVMCTRL_PAGE_SIZE;
		while ((wr_start_addr <= row_end_addr) && (length > 0)) {
			tmp_buffer[j][k] = *buffer;
			k                = (k + 1) % NVMCTRL_PAGE_SIZE;
			if (0 == k) {
				j++;
			}
			wr_start_addr++;
			buffer++;
			length--;
		}

		/* erase row before write */
		_flash_erase_row(device->hw, row_start_addr);

		/* write buffer to flash */
		for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
			_flash_program(device->hw, row_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
		}

	} while (row_end_addr < (wr_start_addr + length - 1));
}

/**
 * \brief Appends a number of bytes in the internal Flash.
 */
void _flash_append(struct _flash_device *const device, const uint32_t dst_addr, const uint8_t *buffer, uint32_t length)
{
	uint32_t page_start_addr = dst_addr & ~(NVMCTRL_PAGE_SIZE - 1);
	uint32_t size;
	uint32_t offset = 0;

	if (dst_addr != page_start_addr) {
		/* Need to write some data to the end of a page */
		size = min(length, NVMCTRL_PAGE_SIZE - (dst_addr - page_start_addr));
		_flash_program(device->hw, dst_addr, buffer, size);
		page_start_addr += NVMCTRL_PAGE_SIZE;
		offset += size;
	}

	while (offset < length) {
		size = min(length - offset, NVMCTRL_PAGE_SIZE);
		_flash_program(device->hw, page_start_addr, buffer + offset, size);
		page_start_addr += NVMCTRL_PAGE_SIZE;
		offset += size;
	}
}

/**
 * \brief Execute erase in the internal flash
 */
void _flash_erase(struct _flash_device *const device, uint32_t dst_addr, uint32_t page_nums)
{
	uint8_t  tmp_buffer[NVMCTRL_PAGE_SIZE];
	uint32_t row_start_addr;
	uint32_t i;

	row_start_addr = dst_addr & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);

	memset(tmp_buffer, 0xFF, NVMCTRL_PAGE_SIZE);

	/* when address is not aligned with row start address */
	if (dst_addr != row_start_addr) {
		row_start_addr += NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;
		for (i = 0; i < NVMCTRL_ROW_PAGES - 1; i++) {
			_flash_write(device, dst_addr, tmp_buffer, NVMCTRL_PAGE_SIZE);
			if (--page_nums == 0) {
				return;
			}
			dst_addr += NVMCTRL_PAGE_SIZE;
			if (dst_addr == row_start_addr) {
				break;
			}
		}
	}

	while (page_nums >= NVMCTRL_ROW_PAGES) {
		_flash_erase_row(device->hw, row_start_addr);
		row_start_addr += NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;
		page_nums -= NVMCTRL_ROW_PAGES;
	}

	if (page_nums != 0) {
		for (i = 0; i < page_nums; i++) {
			_flash_write(device, row_start_addr, tmp_buffer, NVMCTRL_PAGE_SIZE);
			row_start_addr += NVMCTRL_PAGE_SIZE;
		}
	}
}

/**
 * \brief Execute lock in the internal flash
 */
int32_t _flash_lock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums)
{
	uint32_t region_pages;
	uint32_t row_start_addr;

	region_pages   = (uint32_t)NVMCTRL_FLASH_SIZE / (16 * NVMCTRL_PAGE_SIZE);
	row_start_addr = dst_addr & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);

	if ((page_nums != region_pages) || (dst_addr != row_start_addr)) {
		return ERR_INVALID_ARG;
	}

	while (!hri_nvmctrl_get_interrupt_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	/* Clear flags */
	hri_nvmctrl_clear_STATUS_reg(device->hw, NVMCTRL_STATUS_MASK);

	hri_nvmctrl_write_ADDR_reg(device->hw, dst_addr / 2);
	hri_nvmctrl_write_CTRLA_reg(device->hw, NVMCTRL_CTRLA_CMD_LR | NVMCTRL_CTRLA_CMDEX_KEY);

	return (int32_t)NVMCTRL_FLASH_SIZE / (16 * NVMCTRL_PAGE_SIZE);
}

/**
 * \brief Execute unlock in the internal flash
 */
int32_t _flash_unlock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums)
{
	uint32_t region_pages;
	uint32_t row_start_addr;

	region_pages   = (uint32_t)NVMCTRL_FLASH_SIZE / (16 * NVMCTRL_PAGE_SIZE);
	row_start_addr = dst_addr & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);

	if ((page_nums != region_pages) || (dst_addr != row_start_addr)) {
		return ERR_INVALID_ARG;
	}

	while (!hri_nvmctrl_get_interrupt_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	/* Clear flags */
	hri_nvmctrl_clear_STATUS_reg(device->hw, NVMCTRL_STATUS_MASK);

	hri_nvmctrl_write_ADDR_reg(device->hw, dst_addr / 2);
	hri_nvmctrl_write_CTRLA_reg(device->hw, NVMCTRL_CTRLA_CMD_UR | NVMCTRL_CTRLA_CMDEX_KEY);

	return (int32_t)NVMCTRL_FLASH_SIZE / (16 * NVMCTRL_PAGE_SIZE);
}

/**
 * \brief check whether the region which is pointed by address
 */
bool _flash_is_locked(struct _flash_device *const device, const uint32_t dst_addr)
{
	uint16_t region_id;

	/* Get region for given page */
	region_id = dst_addr / (NVMCTRL_FLASH_SIZE / 16);

	return !(hri_nvmctrl_get_LOCK_reg(device->hw, 1 << region_id));
}

/**
 * \brief Enable/disable Flash interrupt
 */
void _flash_set_irq_state(struct _flash_device *const device, const enum _flash_cb_type type, const bool state)
{
	ASSERT(device);

	if (FLASH_DEVICE_CB_READY == type) {
		hri_nvmctrl_write_INTEN_READY_bit(device->hw, state);
	} else if (FLASH_DEVICE_CB_ERROR == type) {
		hri_nvmctrl_write_INTEN_ERROR_bit(device->hw, state);
	}
}

/**
 * \internal   erase a row in flash
 * \param[in]  hw            The pointer to hardware instance
 * \param[in]  dst_addr      Destination page address to erase
 */
static void _flash_erase_row(void *const hw, const uint32_t dst_addr)
{
	while (!hri_nvmctrl_get_interrupt_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* Clear flags */
	hri_nvmctrl_clear_STATUS_reg(hw, NVMCTRL_STATUS_MASK);

	/* Set address and command */
	hri_nvmctrl_write_ADDR_reg(hw, dst_addr / 2);
	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY);
}

/**
 * \internal   write a page in flash
 * \param[in]  hw            The pointer to hardware instance
 * \param[in]  dst_addr      Destination page address to write
 * \param[in]  buffer        Pointer to buffer where the data to
 *                           write is stored
 * \param[in] size           The size of data to write to a page
 */
static void _flash_program(void *const hw, const uint32_t dst_addr, const uint8_t *buffer, const uint16_t size)
{
	ASSERT(!(dst_addr % 2));

	uint32_t nvm_address = dst_addr / 2;
	uint16_t i, data;

	while (!hri_nvmctrl_get_interrupt_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY);

	while (!hri_nvmctrl_get_interrupt_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* Clear flags */
	hri_nvmctrl_clear_STATUS_reg(hw, NVMCTRL_STATUS_MASK);

	for (i = 0; i < size; i += 2) {
		data = buffer[i];
		if (i < NVMCTRL_PAGE_SIZE - 1) {
			data |= (buffer[i + 1] << 8);
		}
		NVM_MEMORY[nvm_address++] = data;
	}

	while (!hri_nvmctrl_get_interrupt_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_ADDR_reg(hw, dst_addr / 2);
	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_WP | NVMCTRL_CTRLA_CMDEX_KEY);
}

/**
 * \internal NVM interrupt handler
 */
void NVMCTRL_Handler(void)
{
	void *const hw = _nvm_dev->hw;

	if (hri_nvmctrl_get_interrupt_READY_bit(hw)) {
		hri_nvmctrl_clear_interrupt_READY_bit(hw);
		if (NULL != _nvm_dev->flash_cb.ready_cb) {
			_nvm_dev->flash_cb.ready_cb(_nvm_dev);
		}
	} else if (hri_nvmctrl_get_interrupt_ERROR_bit(hw)) {
		hri_nvmctrl_clear_interrupt_ERROR_bit(hw);
		if (NULL != _nvm_dev->flash_cb.error_cb) {
			_nvm_dev->flash_cb.error_cb(_nvm_dev);
		}
	}
}

/*
The NVM User Row contains calibration data that are automatically read at device
power on.
The NVM User Row can be read at address 0x804000.
*/
#ifndef _NVM_USER_ROW_BASE
#define _NVM_USER_ROW_BASE 0x804000
#endif
#define _NVM_USER_ROW_N_BITS 64
#define _NVM_USER_ROW_N_BYTES (_NVM_USER_ROW_N_BITS / 8)
#define _NVM_USER_ROW_END (((uint8_t *)_NVM_USER_ROW_BASE) + _NVM_USER_ROW_N_BYTES - 1)
#define _IS_NVM_USER_ROW(b)                                                                                            \
	(((uint8_t *)(b) >= (uint8_t *)(_NVM_USER_ROW_BASE)) && ((uint8_t *)(b) <= (uint8_t *)(_NVM_USER_ROW_END)))
#define _IN_NVM_USER_ROW(b, o) (((uint8_t *)(b) + (o)) <= (uint8_t *)(_NVM_USER_ROW_END))

/*
The NVM Software Calibration Area can be read at address 0x806020.
The NVM Software Calibration Area can not be written.
*/
#ifndef _NVM_SW_CALIB_AREA_BASE
#define _NVM_SW_CALIB_AREA_BASE 0x806020
#endif
#define _NVM_SW_CALIB_AREA_N_BITS 128
#define _NVM_SW_CALIB_AREA_N_BYTES (_NVM_SW_CALIB_AREA_N_BITS / 8)
#define _NVM_SW_CALIB_AREA_END (((uint8_t *)_NVM_SW_CALIB_AREA_BASE) + _NVM_SW_CALIB_AREA_N_BYTES - 1)
#define _IS_NVM_SW_CALIB_AREA(b)                                                                                       \
	(((uint8_t *)(b) >= (uint8_t *)_NVM_SW_CALIB_AREA_BASE) && ((uint8_t *)(b) <= (uint8_t *)_NVM_SW_CALIB_AREA_END))
#define _IN_NVM_SW_CALIB_AREA(b, o) (((uint8_t *)(b) + (o)) <= (uint8_t *)(_NVM_SW_CALIB_AREA_END))

/**
 * \internal Read left aligned data bits
 * \param[in] base       Base address for the data
 * \param[in] bit_offset Offset for the bitfield start
 * \param[in] n_bits     Number of bits in the bitfield
 */
static inline uint32_t _user_area_read_l32_bits(const volatile uint32_t *base, const uint32_t bit_offset,
                                                const uint8_t n_bits)
{
	return base[bit_offset >> 5] & ((1 << n_bits) - 1);
}

/**
 * \internal Read right aligned data bits
 * \param[in] base       Base address for the data
 * \param[in] bit_offset Offset for the bitfield start
 * \param[in] n_bits     Number of bits in the bitfield
 */
static inline uint32_t _user_area_read_r32_bits(const volatile uint32_t *base, const uint32_t bit_offset,
                                                const uint8_t n_bits)
{
	return (base[bit_offset >> 5] >> (bit_offset & 0x1F)) & ((1 << n_bits) - 1);
}

int32_t _user_area_read(const void *base, const uint32_t offset, uint8_t *buf, uint32_t size)
{
	ASSERT(buf);

	/** Parameter check. */
	if (_IS_NVM_USER_ROW(base)) {
		if (!_IN_NVM_USER_ROW(base, offset)) {
			return ERR_BAD_ADDRESS;
		}
		/* Cut off if request too many bytes */
		if (!_IN_NVM_USER_ROW(base, offset + size - 1)) {
			return ERR_INVALID_ARG;
		}
	} else if (_IS_NVM_SW_CALIB_AREA(base)) {
		if (!_IN_NVM_SW_CALIB_AREA(base, offset)) {
			return ERR_BAD_ADDRESS;
		}
		/* Cut off if request too many bytes */
		if (!_IN_NVM_SW_CALIB_AREA(base, offset + size - 1)) {
			return ERR_INVALID_ARG;
		}
	} else {
		return ERR_UNSUPPORTED_OP;
	}

	/* Copy data */
	memcpy(buf, ((uint8_t *)base) + offset, size);
	return ERR_NONE;
}

uint32_t _user_area_read_bits(const void *base, const uint32_t bit_offset, const uint8_t n_bits)
{
	volatile uint32_t *mem_base = (volatile uint32_t *)base;
	uint32_t           l_off, l_bits;
	uint32_t           r_off, r_bits;

	/** Parameter check. */
	if (_IS_NVM_USER_ROW(base)) {
		ASSERT(_IN_NVM_USER_ROW(base, bit_offset >> 3) && _IN_NVM_USER_ROW(base, (bit_offset + n_bits - 1) >> 3));
	} else if (_IS_NVM_SW_CALIB_AREA(base)) {
		ASSERT(_IN_NVM_SW_CALIB_AREA(base, bit_offset >> 3)
		       && _IN_NVM_SW_CALIB_AREA(base, (bit_offset + n_bits - 1) >> 3));
	} else {
		ASSERT(false);
	}

	/* Since the bitfield can cross 32-bits boundaries,
	 * left and right bits are read from 32-bit aligned address
	 * and then combined together. */
	l_off  = bit_offset & (~(32 - 1));
	r_off  = l_off + 32;
	l_bits = 32 - (bit_offset & (32 - 1));
	if (n_bits > l_bits) {
		r_bits = n_bits - l_bits;
	} else {
		l_bits = n_bits;
		r_bits = 0;
	}
	return _user_area_read_r32_bits(mem_base, bit_offset, l_bits)
	       + (_user_area_read_l32_bits(mem_base, r_off, r_bits) << l_bits);
}

/** \internal Write 64-bit user row
 *  \param[in] _row Pointer to 64-bit user row data.
 */
static int32_t _user_row_write_exec(const uint32_t *_row)
{
	Nvmctrl *hw    = NVMCTRL;
	uint32_t ctrlb = hri_nvmctrl_read_CTRLB_reg(NVMCTRL);

	/* Denie if Security Bit is set */
	if (hri_nvmctrl_get_STATUS_reg(hw, NVMCTRL_STATUS_SB)) {
		return ERR_DENIED;
	}

	/* Do Save */

	/* - Prepare. */
	while (!hri_nvmctrl_get_INTFLAG_reg(hw, NVMCTRL_INTFLAG_READY)) {
		/* Wait until this module isn't busy */
	}
	hri_nvmctrl_clear_STATUS_reg(hw, NVMCTRL_STATUS_MASK);
	hri_nvmctrl_set_CTRLB_MANW_bit(hw);

	/* - Erase AUX row. */
	hri_nvmctrl_write_ADDR_reg(hw, (hri_nvmctrl_addr_reg_t)_NVM_USER_ROW_BASE);
	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_EAR | NVMCTRL_CTRLA_CMDEX_KEY);
	while (!hri_nvmctrl_get_INTFLAG_reg(hw, NVMCTRL_INTFLAG_READY)) {
		/* Wait until this module isn't busy */
	}

	/* - Page buffer clear & write. */
	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY);
	while (!hri_nvmctrl_get_INTFLAG_reg(hw, NVMCTRL_INTFLAG_READY)) {
		/* Wait until this module isn't busy */
	}
	*((uint32_t *)NVMCTRL_AUX0_ADDRESS)       = _row[0];
	*(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1) = _row[1];

	/* - Write AUX row. */
	hri_nvmctrl_write_ADDR_reg(hw, (hri_nvmctrl_addr_reg_t)_NVM_USER_ROW_BASE);
	hri_nvmctrl_write_CTRLA_reg(hw, NVMCTRL_CTRLA_CMD_WAP | NVMCTRL_CTRLA_CMDEX_KEY);
	while (!hri_nvmctrl_get_INTFLAG_reg(hw, NVMCTRL_INTFLAG_READY)) {
		/* Wait until this module isn't busy */
	}

	/* Restore CTRLB */
	hri_nvmctrl_write_CTRLB_reg(NVMCTRL, ctrlb);

	return ERR_NONE;
}

int32_t _user_area_write(void *base, const uint32_t offset, const uint8_t *buf, const uint32_t size)
{
	uint32_t _row[2]; /* Copy of user row. */

	/** Parameter check. */
	if (_IS_NVM_USER_ROW(base)) {
		if (!_IN_NVM_USER_ROW(base, offset)) {
			return ERR_BAD_ADDRESS;
		} else if (!_IN_NVM_USER_ROW(base, offset + size - 1)) {
			return ERR_INVALID_ARG;
		}
	} else if (_IS_NVM_SW_CALIB_AREA(base)) {
		return ERR_DENIED;
	} else {
		return ERR_UNSUPPORTED_OP;
	}

	memcpy(_row, base, 8);                       /* Store previous data. */
	memcpy((uint8_t *)_row + offset, buf, size); /* Modify with buf data. */

	return _user_row_write_exec(_row);
}

int32_t _user_area_write_bits(void *base, const uint32_t bit_offset, const uint32_t bits, const uint8_t n_bits)
{
	uint32_t _row[2]; /* Copy of user row. */
	uint32_t l_off, l_bits;
	uint32_t r_off, r_bits;

	/** Parameter check. */
	if (_IS_NVM_USER_ROW(base)) {
		if (!_IN_NVM_USER_ROW(base, bit_offset >> 3)) {
			return ERR_BAD_ADDRESS;
		} else if (!_IN_NVM_USER_ROW(base, (bit_offset + n_bits - 1) >> 3)) {
			return ERR_INVALID_ARG;
		}
	} else if (_IS_NVM_SW_CALIB_AREA(base)) {
		return ERR_DENIED;
	} else {
		return ERR_UNSUPPORTED_OP;
	}

	/* Since the bitfield can cross 32-bits boundaries,
	 * left and right bits are splitted for 32-bit aligned address
	 * and then saved. */
	l_off  = bit_offset & (~(32 - 1));
	r_off  = l_off + 32;
	l_bits = 32 - (bit_offset & (32 - 1));
	if (n_bits > l_bits) {
		r_bits = n_bits - l_bits;
	} else {
		l_bits = n_bits;
		r_bits = 0;
	}

	memcpy(_row, base, 8); /* Store previous data. */
	if (l_bits) {
		uint32_t l_mask = ((1 << l_bits) - 1) << (bit_offset & (32 - 1));
		_row[bit_offset >> 5] &= ~l_mask;
		_row[bit_offset >> 5] |= (bits << (bit_offset & (32 - 1))) & l_mask;
	}
	if (r_bits) {
		uint32_t r_mask = (1 << r_bits) - 1;
		_row[r_off >> 5] &= ~r_mask;
		_row[r_off >> 5] |= bits >> l_bits;
	}
	return _user_row_write_exec(_row);
}
