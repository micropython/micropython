
/**
 * \file
 *
 * \brief Non-Volatile Memory Controller
 *
 * Copyright (C) 2016 -2017 Atmel Corporation. All rights reserved.
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

#define NVM_MEMORY ((volatile uint32_t *)FLASH_ADDR)
#define NVMCTRL_BLOCK_PAGES (NVMCTRL_BLOCK_SIZE / NVMCTRL_PAGE_SIZE)
#define NVMCTRL_REGIONS_NUM 32
#define NVMCTRL_INTFLAG_ERR                                                                                            \
	(NVMCTRL_INTFLAG_ADDRE | NVMCTRL_INTFLAG_PROGE | NVMCTRL_INTFLAG_LOCKE | NVMCTRL_INTFLAG_ECCSE                     \
	 | NVMCTRL_INTFLAG_NVME                                                                                            \
	 | NVMCTRL_INTFLAG_SEESOVF)
/**
 * \brief NVM configuration type
 */
struct nvm_configuration {
	hri_nvmctrl_ctrlb_reg_t ctrla; /*!< Control B Register */
};

/**
 * \brief Array of NVM configurations
 */
static struct nvm_configuration _nvm
    = {(CONF_NVM_CACHE0 << NVMCTRL_CTRLA_CACHEDIS0_Pos) | (CONF_NVM_CACHE1 << NVMCTRL_CTRLA_CACHEDIS1_Pos)
       | (NVMCTRL_CTRLA_PRM(CONF_NVM_SLEEPPRM))};

/*!< Pointer to hpl device */
static struct _flash_device *_nvm_dev = NULL;

static void _flash_erase_block(void *const hw, const uint32_t dst_addr);
static void _flash_program(void *const hw, const uint32_t dst_addr, const uint8_t *buffer, const uint16_t size);

/**
 * \brief Initialize NVM
 */
int32_t _flash_init(struct _flash_device *const device, void *const hw)
{
	uint32_t ctrla;

	ASSERT(device && (hw == NVMCTRL));

	device->hw = hw;
	ctrla      = hri_nvmctrl_read_CTRLA_reg(hw);
	ctrla &= ~(NVMCTRL_CTRLA_CACHEDIS0 | NVMCTRL_CTRLA_CACHEDIS1 | NVMCTRL_CTRLA_PRM_Msk);
	ctrla |= _nvm.ctrla;
	hri_nvmctrl_write_CTRLA_reg(hw, ctrla);

	_nvm_dev = device;
	NVIC_DisableIRQ(NVMCTRL_0_IRQn);
	NVIC_DisableIRQ(NVMCTRL_1_IRQn);
	NVIC_ClearPendingIRQ(NVMCTRL_0_IRQn);
	NVIC_ClearPendingIRQ(NVMCTRL_1_IRQn);
	NVIC_EnableIRQ(NVMCTRL_0_IRQn);
	NVIC_EnableIRQ(NVMCTRL_1_IRQn);

	return ERR_NONE;
}

/**
 * \brief De-initialize NVM
 */
void _flash_deinit(struct _flash_device *const device)
{
	device->hw = NULL;
	NVIC_DisableIRQ(NVMCTRL_0_IRQn);
	NVIC_DisableIRQ(NVMCTRL_1_IRQn);
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
	return (uint32_t)hri_nvmctrl_read_PARAM_NVMP_bf(device->hw);
}

/**
 * \brief Get the number of wait states for read and write operations.
 */
uint8_t _flash_get_wait_state(struct _flash_device *const device)
{
	return hri_nvmctrl_get_CTRLA_reg(device->hw, NVMCTRL_CTRLA_RWS_Msk);
}

/**
 * \brief Set the number of wait states for read and write operations.
 */
void _flash_set_wait_state(struct _flash_device *const device, uint8_t state)
{
	hri_nvmctrl_write_CTRLA_RWS_bf(device->hw, state);
}

/**
 * \brief Reads a number of bytes to a page in the internal Flash.
 */
void _flash_read(struct _flash_device *const device, const uint32_t src_addr, uint8_t *buffer, uint32_t length)
{
	uint8_t *nvm_addr = (uint8_t *)NVM_MEMORY;
	uint32_t i;

	/* Check if the module is busy */
	while (!hri_nvmctrl_get_STATUS_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	for (i = 0; i < length; i++) {
		buffer[i] = nvm_addr[src_addr + i];
	}
}

/**
 * \brief Writes a number of bytes to a page in the internal Flash.
 */
void _flash_write(struct _flash_device *const device, const uint32_t dst_addr, uint8_t *buffer, uint32_t length)
{
	uint8_t  tmp_buffer[NVMCTRL_BLOCK_PAGES][NVMCTRL_PAGE_SIZE];
	uint32_t block_start_addr, block_end_addr;
	uint32_t i, j, k;
	uint32_t wr_start_addr = dst_addr;

	do {
		block_start_addr = wr_start_addr & ~(NVMCTRL_BLOCK_SIZE - 1);
		block_end_addr   = block_start_addr + NVMCTRL_BLOCK_SIZE - 1;

		/* store the erase data into temp buffer before write */
		for (i = 0; i < NVMCTRL_BLOCK_PAGES; i++) {
			_flash_read(device, block_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
		}

		/* temp buffer update */
		j = (wr_start_addr - block_start_addr) / NVMCTRL_PAGE_SIZE;
		k = wr_start_addr - block_start_addr - j * NVMCTRL_PAGE_SIZE;
		while ((wr_start_addr <= block_end_addr) && (length > 0)) {
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
		_flash_erase_block(device->hw, block_start_addr);

		/* write buffer to flash */
		for (i = 0; i < NVMCTRL_BLOCK_PAGES; i++) {
			_flash_program(device->hw, block_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
		}
	} while (block_end_addr < (wr_start_addr + length - 1));
}

/**
 * \brief Appends a number of bytes in the internal Flash.
 */
void _flash_append(struct _flash_device *const device, const uint32_t dst_addr, uint8_t *buffer, uint32_t length)
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
	uint32_t block_start_addr;
	uint32_t i;

	block_start_addr = dst_addr & ~(NVMCTRL_BLOCK_SIZE - 1);

	memset(tmp_buffer, 0xFF, NVMCTRL_PAGE_SIZE);

	/* when address is not aligned with block start address */
	if (dst_addr != block_start_addr) {
		block_start_addr += NVMCTRL_BLOCK_SIZE;
		for (i = 0; i < NVMCTRL_BLOCK_PAGES - 1; i++) {
			_flash_write(device, dst_addr, tmp_buffer, NVMCTRL_PAGE_SIZE);

			if (--page_nums == 0) {
				return;
			}

			dst_addr += NVMCTRL_PAGE_SIZE;

			if (dst_addr == block_start_addr) {
				break;
			}
		}
	}

	while (page_nums >= NVMCTRL_BLOCK_PAGES) {
		_flash_erase_block(device->hw, block_start_addr);
		block_start_addr += NVMCTRL_PAGE_SIZE;
		page_nums -= NVMCTRL_BLOCK_PAGES;
	}

	if (page_nums != 0) {
		for (i = 0; i < page_nums; i++) {
			_flash_write(device, block_start_addr, tmp_buffer, NVMCTRL_PAGE_SIZE);
			block_start_addr += NVMCTRL_PAGE_SIZE;
		}
	}
}

/**
 * \brief Execute lock in the internal flash
 */
int32_t _flash_lock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums)
{
	uint32_t region_pages;
	uint32_t block_start_addr;

	region_pages     = (uint32_t)FLASH_SIZE / (NVMCTRL_REGIONS_NUM * NVMCTRL_PAGE_SIZE);
	block_start_addr = dst_addr & ~(NVMCTRL_BLOCK_SIZE - 1);

	if ((page_nums != region_pages) || (dst_addr != block_start_addr)) {
		return ERR_INVALID_ARG;
	}

	while (!hri_nvmctrl_get_STATUS_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_ADDR_reg(device->hw, dst_addr);
	hri_nvmctrl_write_CTRLB_reg(device->hw, NVMCTRL_CTRLB_CMD_LR | NVMCTRL_CTRLB_CMDEX_KEY);

	return (int32_t)FLASH_SIZE / (NVMCTRL_REGIONS_NUM * NVMCTRL_PAGE_SIZE);
}

/**
 * \brief Execute unlock in the internal flash
 */
int32_t _flash_unlock(struct _flash_device *const device, const uint32_t dst_addr, uint32_t page_nums)
{
	uint32_t region_pages;
	uint32_t block_start_addr;

	region_pages     = (uint32_t)FLASH_SIZE / (NVMCTRL_REGIONS_NUM * NVMCTRL_PAGE_SIZE);
	block_start_addr = dst_addr & ~(NVMCTRL_BLOCK_SIZE - 1);

	if ((page_nums != region_pages) || (dst_addr != block_start_addr)) {
		return ERR_INVALID_ARG;
	}

	while (!hri_nvmctrl_get_STATUS_READY_bit(device->hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_ADDR_reg(device->hw, dst_addr);
	hri_nvmctrl_write_CTRLB_reg(device->hw, NVMCTRL_CTRLB_CMD_UR | NVMCTRL_CTRLB_CMDEX_KEY);

	return (int32_t)FLASH_SIZE / (NVMCTRL_REGIONS_NUM * NVMCTRL_PAGE_SIZE);
}

/**
 * \brief check whether the region which is pointed by address
 */
bool _flash_is_locked(struct _flash_device *const device, const uint32_t dst_addr)
{
	uint16_t region_id;

	/* Get region for given page */
	region_id = dst_addr / (FLASH_SIZE / NVMCTRL_REGIONS_NUM);

	return !(hri_nvmctrl_get_RUNLOCK_reg(device->hw, 1 << region_id));
}

/**
 * \brief Enable/disable Flash interrupt
 */
void _flash_set_irq_state(struct _flash_device *const device, const enum _flash_cb_type type, const bool state)
{
	ASSERT(device);

	if (FLASH_DEVICE_CB_READY == type) {
		hri_nvmctrl_write_INTEN_DONE_bit(device->hw, state);
	} else if (FLASH_DEVICE_CB_ERROR == type) {
		if (state) {
			hri_nvmctrl_write_INTEN_reg(device->hw, NVMCTRL_INTFLAG_ERR);
		} else {
			hri_nvmctrl_clear_INTEN_reg(device->hw, NVMCTRL_INTFLAG_ERR);
		}
	}
}

/**
 * \internal   erase a row in flash
 * \param[in]  hw            The pointer to hardware instance
 * \param[in]  dst_addr      Destination page address to erase
 */
static void _flash_erase_block(void *const hw, const uint32_t dst_addr)
{
	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* Set address and command */
	hri_nvmctrl_write_ADDR_reg(hw, dst_addr);
	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_EB | NVMCTRL_CTRLB_CMDEX_KEY);
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
	uint32_t *ptr_read    = (uint32_t *)buffer;
	uint32_t  nvm_address = dst_addr / 4;
	uint16_t  i;

	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_PBC | NVMCTRL_CTRLB_CMDEX_KEY);

	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* Writes to the page buffer must be 32 bits, perform manual copy
	 * to ensure alignment */
	for (i = 0; i < size; i += 4) {
		NVM_MEMORY[nvm_address++] = *ptr_read;
		ptr_read++;
	}

	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	hri_nvmctrl_write_ADDR_reg(hw, dst_addr);
	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_WP | NVMCTRL_CTRLB_CMDEX_KEY);
}

/**
 * \internal NVM interrupt handler
 *
 * \param[in] p The pointer to interrupt parameter
 */
static void _nvm_interrupt_handler(struct _flash_device *device)
{
	void *const hw = device->hw;

	if (hri_nvmctrl_get_INTFLAG_DONE_bit(hw)) {
		hri_nvmctrl_clear_INTFLAG_DONE_bit(hw);

		if (NULL != device->flash_cb.ready_cb) {
			device->flash_cb.ready_cb(device);
		}
	} else if (hri_nvmctrl_read_INTFLAG_reg(hw) && ~NVMCTRL_INTFLAG_ERR) {
		hri_nvmctrl_clear_INTFLAG_reg(hw, NVMCTRL_INTFLAG_ERR);

		if (NULL != device->flash_cb.error_cb) {
			device->flash_cb.error_cb(device);
		}
	}
}

/**
 * \internal NVM 0 interrupt handler
 */
void NVMCTRL_0_Handler(void)
{
	_nvm_interrupt_handler(_nvm_dev);
}

/**
 * \internal NVM 1 interrupt handler
 */
void NVMCTRL_1_Handler(void)
{
	_nvm_interrupt_handler(_nvm_dev);
}

/*
   The NVM User Row contains calibration data that are automatically read at device
   power on.
   The NVM User Row can be read at address 0x804000.
 */
#ifndef _NVM_USER_ROW_BASE
#define _NVM_USER_ROW_BASE 0x804000
#endif
#define _NVM_USER_ROW_N_BITS 96
#define _NVM_USER_ROW_N_BYTES (_NVM_USER_ROW_N_BITS / 8)
#define _NVM_USER_ROW_END (((uint8_t *)_NVM_USER_ROW_BASE) + _NVM_USER_ROW_N_BYTES - 1)
#define _IS_NVM_USER_ROW(b)                                                                                            \
	(((uint8_t *)(b) >= (uint8_t *)(_NVM_USER_ROW_BASE)) && ((uint8_t *)(b) <= (uint8_t *)(_NVM_USER_ROW_END)))
#define _IN_NVM_USER_ROW(b, o) (((uint8_t *)(b) + (o)) <= (uint8_t *)(_NVM_USER_ROW_END))

/*
   The NVM Software Calibration Area can be read at address 0x00800080.
   The NVM Software Calibration Area can not be written.
 */
#ifndef _NVM_SW_CALIB_AREA_BASE
#define _NVM_SW_CALIB_AREA_BASE 0x00800080
#endif
#define _NVM_SW_CALIB_AREA_N_BITS 45
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

/** \internal Write 96-bit user row
 *  \param[in] _row Pointer to 96-bit user row data.
 */
static int32_t _user_row_write_exec(const uint32_t *_row)
{
	Nvmctrl *hw    = NVMCTRL;
	uint32_t ctrla = hri_nvmctrl_read_CTRLA_reg(NVMCTRL);

	/* Denied if Security Bit is set */
	if (DSU->STATUSB.bit.PROT) {
		return ERR_DENIED;
	}

	/* Do Save */

	/* - Prepare. */
	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}
	hri_nvmctrl_clear_CTRLA_WMODE_bf(NVMCTRL, NVMCTRL_CTRLA_WMODE_Msk);

	/* - Erase AUX row. */
	hri_nvmctrl_write_ADDR_reg(hw, (hri_nvmctrl_addr_reg_t)_NVM_USER_ROW_BASE);
	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_EP | NVMCTRL_CTRLB_CMDEX_KEY);
	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* - Page buffer clear & write. */
	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_PBC | NVMCTRL_CTRLB_CMDEX_KEY);
	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}
	*((uint32_t *)NVMCTRL_USER)       = _row[0];
	*(((uint32_t *)NVMCTRL_USER) + 1) = _row[1];
	*(((uint32_t *)NVMCTRL_USER) + 2) = _row[2];
	*(((uint32_t *)NVMCTRL_USER) + 3) = 0xFFFFFFFF;

	/* - Write AUX row. */
	hri_nvmctrl_write_ADDR_reg(hw, (hri_nvmctrl_addr_reg_t)_NVM_USER_ROW_BASE);
	hri_nvmctrl_write_CTRLB_reg(hw, NVMCTRL_CTRLB_CMD_WQW | NVMCTRL_CTRLB_CMDEX_KEY);
	while (!hri_nvmctrl_get_STATUS_READY_bit(hw)) {
		/* Wait until this module isn't busy */
	}

	/* Restore CTRLA */
	hri_nvmctrl_write_CTRLA_reg(NVMCTRL, ctrla);

	return ERR_NONE;
}

int32_t _user_area_write(void *base, const uint32_t offset, const uint8_t *buf, const uint32_t size)
{
	uint32_t _row[3]; /* Copy of user row. */

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

	memcpy(_row, base, 12);                      /* Store previous data. */
	memcpy((uint8_t *)_row + offset, buf, size); /* Modify with buf data. */

	return _user_row_write_exec(_row);
}

int32_t _user_area_write_bits(void *base, const uint32_t bit_offset, const uint32_t bits, const uint8_t n_bits)
{
	uint32_t _row[3]; /* Copy of user row. */
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

	memcpy(_row, base, 12); /* Store previous data. */

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
