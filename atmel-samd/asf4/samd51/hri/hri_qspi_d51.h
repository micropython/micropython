/**
 * \file
 *
 * \brief SAM QSPI
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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
 */

#ifdef _SAMD51_QSPI_COMPONENT_
#ifndef _HRI_QSPI_D51_H_INCLUDED_
#define _HRI_QSPI_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_QSPI_CRITICAL_SECTIONS)
#define QSPI_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define QSPI_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define QSPI_CRITICAL_SECTION_ENTER()
#define QSPI_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_qspi_baud_reg_t;
typedef uint32_t hri_qspi_ctrla_reg_t;
typedef uint32_t hri_qspi_ctrlb_reg_t;
typedef uint32_t hri_qspi_instraddr_reg_t;
typedef uint32_t hri_qspi_instrctrl_reg_t;
typedef uint32_t hri_qspi_instrframe_reg_t;
typedef uint32_t hri_qspi_intenset_reg_t;
typedef uint32_t hri_qspi_intflag_reg_t;
typedef uint32_t hri_qspi_rxdata_reg_t;
typedef uint32_t hri_qspi_scrambctrl_reg_t;
typedef uint32_t hri_qspi_scrambkey_reg_t;
typedef uint32_t hri_qspi_status_reg_t;
typedef uint32_t hri_qspi_txdata_reg_t;

static inline void hri_qspi_set_INTEN_RXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_RXC;
}

static inline bool hri_qspi_get_INTEN_RXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_RXC) >> QSPI_INTENSET_RXC_Pos;
}

static inline void hri_qspi_write_INTEN_RXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_RXC;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_RXC;
	}
}

static inline void hri_qspi_clear_INTEN_RXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_RXC;
}

static inline void hri_qspi_set_INTEN_DRE_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_DRE;
}

static inline bool hri_qspi_get_INTEN_DRE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_DRE) >> QSPI_INTENSET_DRE_Pos;
}

static inline void hri_qspi_write_INTEN_DRE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_DRE;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_DRE;
	}
}

static inline void hri_qspi_clear_INTEN_DRE_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_DRE;
}

static inline void hri_qspi_set_INTEN_TXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_TXC;
}

static inline bool hri_qspi_get_INTEN_TXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_TXC) >> QSPI_INTENSET_TXC_Pos;
}

static inline void hri_qspi_write_INTEN_TXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_TXC;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_TXC;
	}
}

static inline void hri_qspi_clear_INTEN_TXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_TXC;
}

static inline void hri_qspi_set_INTEN_ERROR_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_ERROR;
}

static inline bool hri_qspi_get_INTEN_ERROR_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_ERROR) >> QSPI_INTENSET_ERROR_Pos;
}

static inline void hri_qspi_write_INTEN_ERROR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_ERROR;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_ERROR;
	}
}

static inline void hri_qspi_clear_INTEN_ERROR_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_ERROR;
}

static inline void hri_qspi_set_INTEN_CSRISE_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_CSRISE;
}

static inline bool hri_qspi_get_INTEN_CSRISE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_CSRISE) >> QSPI_INTENSET_CSRISE_Pos;
}

static inline void hri_qspi_write_INTEN_CSRISE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_CSRISE;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_CSRISE;
	}
}

static inline void hri_qspi_clear_INTEN_CSRISE_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_CSRISE;
}

static inline void hri_qspi_set_INTEN_INSTREND_bit(const void *const hw)
{
	((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_INSTREND;
}

static inline bool hri_qspi_get_INTEN_INSTREND_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTENSET.reg & QSPI_INTENSET_INSTREND) >> QSPI_INTENSET_INSTREND_Pos;
}

static inline void hri_qspi_write_INTEN_INSTREND_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_INSTREND;
	} else {
		((Qspi *)hw)->INTENSET.reg = QSPI_INTENSET_INSTREND;
	}
}

static inline void hri_qspi_clear_INTEN_INSTREND_bit(const void *const hw)
{
	((Qspi *)hw)->INTENCLR.reg = QSPI_INTENSET_INSTREND;
}

static inline void hri_qspi_set_INTEN_reg(const void *const hw, hri_qspi_intenset_reg_t mask)
{
	((Qspi *)hw)->INTENSET.reg = mask;
}

static inline hri_qspi_intenset_reg_t hri_qspi_get_INTEN_reg(const void *const hw, hri_qspi_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_qspi_intenset_reg_t hri_qspi_read_INTEN_reg(const void *const hw)
{
	return ((Qspi *)hw)->INTENSET.reg;
}

static inline void hri_qspi_write_INTEN_reg(const void *const hw, hri_qspi_intenset_reg_t data)
{
	((Qspi *)hw)->INTENSET.reg = data;
	((Qspi *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_qspi_clear_INTEN_reg(const void *const hw, hri_qspi_intenset_reg_t mask)
{
	((Qspi *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_qspi_get_INTFLAG_RXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_RXC) >> QSPI_INTFLAG_RXC_Pos;
}

static inline void hri_qspi_clear_INTFLAG_RXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_RXC;
}

static inline bool hri_qspi_get_INTFLAG_DRE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_DRE) >> QSPI_INTFLAG_DRE_Pos;
}

static inline void hri_qspi_clear_INTFLAG_DRE_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_DRE;
}

static inline bool hri_qspi_get_INTFLAG_TXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_TXC) >> QSPI_INTFLAG_TXC_Pos;
}

static inline void hri_qspi_clear_INTFLAG_TXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_TXC;
}

static inline bool hri_qspi_get_INTFLAG_ERROR_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_ERROR) >> QSPI_INTFLAG_ERROR_Pos;
}

static inline void hri_qspi_clear_INTFLAG_ERROR_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_ERROR;
}

static inline bool hri_qspi_get_INTFLAG_CSRISE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_CSRISE) >> QSPI_INTFLAG_CSRISE_Pos;
}

static inline void hri_qspi_clear_INTFLAG_CSRISE_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_CSRISE;
}

static inline bool hri_qspi_get_INTFLAG_INSTREND_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_INSTREND) >> QSPI_INTFLAG_INSTREND_Pos;
}

static inline void hri_qspi_clear_INTFLAG_INSTREND_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_INSTREND;
}

static inline bool hri_qspi_get_interrupt_RXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_RXC) >> QSPI_INTFLAG_RXC_Pos;
}

static inline void hri_qspi_clear_interrupt_RXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_RXC;
}

static inline bool hri_qspi_get_interrupt_DRE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_DRE) >> QSPI_INTFLAG_DRE_Pos;
}

static inline void hri_qspi_clear_interrupt_DRE_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_DRE;
}

static inline bool hri_qspi_get_interrupt_TXC_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_TXC) >> QSPI_INTFLAG_TXC_Pos;
}

static inline void hri_qspi_clear_interrupt_TXC_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_TXC;
}

static inline bool hri_qspi_get_interrupt_ERROR_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_ERROR) >> QSPI_INTFLAG_ERROR_Pos;
}

static inline void hri_qspi_clear_interrupt_ERROR_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_ERROR;
}

static inline bool hri_qspi_get_interrupt_CSRISE_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_CSRISE) >> QSPI_INTFLAG_CSRISE_Pos;
}

static inline void hri_qspi_clear_interrupt_CSRISE_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_CSRISE;
}

static inline bool hri_qspi_get_interrupt_INSTREND_bit(const void *const hw)
{
	return (((Qspi *)hw)->INTFLAG.reg & QSPI_INTFLAG_INSTREND) >> QSPI_INTFLAG_INSTREND_Pos;
}

static inline void hri_qspi_clear_interrupt_INSTREND_bit(const void *const hw)
{
	((Qspi *)hw)->INTFLAG.reg = QSPI_INTFLAG_INSTREND;
}

static inline hri_qspi_intflag_reg_t hri_qspi_get_INTFLAG_reg(const void *const hw, hri_qspi_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_qspi_intflag_reg_t hri_qspi_read_INTFLAG_reg(const void *const hw)
{
	return ((Qspi *)hw)->INTFLAG.reg;
}

static inline void hri_qspi_clear_INTFLAG_reg(const void *const hw, hri_qspi_intflag_reg_t mask)
{
	((Qspi *)hw)->INTFLAG.reg = mask;
}

static inline void hri_qspi_write_TXDATA_reg(const void *const hw, hri_qspi_txdata_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->TXDATA.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_write_SCRAMBKEY_reg(const void *const hw, hri_qspi_scrambkey_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBKEY.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLA_SWRST_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg |= QSPI_CTRLA_SWRST;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp = (tmp & QSPI_CTRLA_SWRST) >> QSPI_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_set_CTRLA_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg |= QSPI_CTRLA_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp = (tmp & QSPI_CTRLA_ENABLE) >> QSPI_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp &= ~QSPI_CTRLA_ENABLE;
	tmp |= value << QSPI_CTRLA_ENABLE_Pos;
	((Qspi *)hw)->CTRLA.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg &= ~QSPI_CTRLA_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg ^= QSPI_CTRLA_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLA_LASTXFER_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg |= QSPI_CTRLA_LASTXFER;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLA_LASTXFER_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp = (tmp & QSPI_CTRLA_LASTXFER) >> QSPI_CTRLA_LASTXFER_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLA_LASTXFER_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp &= ~QSPI_CTRLA_LASTXFER;
	tmp |= value << QSPI_CTRLA_LASTXFER_Pos;
	((Qspi *)hw)->CTRLA.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLA_LASTXFER_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg &= ~QSPI_CTRLA_LASTXFER;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLA_LASTXFER_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg ^= QSPI_CTRLA_LASTXFER;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLA_reg(const void *const hw, hri_qspi_ctrla_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrla_reg_t hri_qspi_get_CTRLA_reg(const void *const hw, hri_qspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_CTRLA_reg(const void *const hw, hri_qspi_ctrla_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLA_reg(const void *const hw, hri_qspi_ctrla_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLA_reg(const void *const hw, hri_qspi_ctrla_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLA.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrla_reg_t hri_qspi_read_CTRLA_reg(const void *const hw)
{
	return ((Qspi *)hw)->CTRLA.reg;
}

static inline void hri_qspi_set_CTRLB_MODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_MODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLB_MODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_MODE) >> QSPI_CTRLB_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLB_MODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_MODE;
	tmp |= value << QSPI_CTRLB_MODE_Pos;
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_MODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_MODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_MODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_MODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLB_LOOPEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_LOOPEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLB_LOOPEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_LOOPEN) >> QSPI_CTRLB_LOOPEN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLB_LOOPEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_LOOPEN;
	tmp |= value << QSPI_CTRLB_LOOPEN_Pos;
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_LOOPEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_LOOPEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_LOOPEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_LOOPEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLB_WDRBT_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_WDRBT;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLB_WDRBT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_WDRBT) >> QSPI_CTRLB_WDRBT_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLB_WDRBT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_WDRBT;
	tmp |= value << QSPI_CTRLB_WDRBT_Pos;
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_WDRBT_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_WDRBT;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_WDRBT_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_WDRBT;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLB_SMEMREG_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_SMEMREG;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_CTRLB_SMEMREG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_SMEMREG) >> QSPI_CTRLB_SMEMREG_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_CTRLB_SMEMREG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_SMEMREG;
	tmp |= value << QSPI_CTRLB_SMEMREG_Pos;
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_SMEMREG_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_SMEMREG;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_SMEMREG_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_SMEMREG;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_CTRLB_CSMODE_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_CSMODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_get_CTRLB_CSMODE_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_CSMODE(mask)) >> QSPI_CTRLB_CSMODE_Pos;
	return tmp;
}

static inline void hri_qspi_write_CTRLB_CSMODE_bf(const void *const hw, hri_qspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_CSMODE_Msk;
	tmp |= QSPI_CTRLB_CSMODE(data);
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_CSMODE_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_CSMODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_CSMODE_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_CSMODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_read_CTRLB_CSMODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_CSMODE_Msk) >> QSPI_CTRLB_CSMODE_Pos;
	return tmp;
}

static inline void hri_qspi_set_CTRLB_DATALEN_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_DATALEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_get_CTRLB_DATALEN_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DATALEN(mask)) >> QSPI_CTRLB_DATALEN_Pos;
	return tmp;
}

static inline void hri_qspi_write_CTRLB_DATALEN_bf(const void *const hw, hri_qspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_DATALEN_Msk;
	tmp |= QSPI_CTRLB_DATALEN(data);
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_DATALEN_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_DATALEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_DATALEN_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_DATALEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_read_CTRLB_DATALEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DATALEN_Msk) >> QSPI_CTRLB_DATALEN_Pos;
	return tmp;
}

static inline void hri_qspi_set_CTRLB_DLYBCT_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_DLYBCT(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_get_CTRLB_DLYBCT_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DLYBCT(mask)) >> QSPI_CTRLB_DLYBCT_Pos;
	return tmp;
}

static inline void hri_qspi_write_CTRLB_DLYBCT_bf(const void *const hw, hri_qspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_DLYBCT_Msk;
	tmp |= QSPI_CTRLB_DLYBCT(data);
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_DLYBCT_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_DLYBCT(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_DLYBCT_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_DLYBCT(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_read_CTRLB_DLYBCT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DLYBCT_Msk) >> QSPI_CTRLB_DLYBCT_Pos;
	return tmp;
}

static inline void hri_qspi_set_CTRLB_DLYCS_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= QSPI_CTRLB_DLYCS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_get_CTRLB_DLYCS_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DLYCS(mask)) >> QSPI_CTRLB_DLYCS_Pos;
	return tmp;
}

static inline void hri_qspi_write_CTRLB_DLYCS_bf(const void *const hw, hri_qspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= ~QSPI_CTRLB_DLYCS_Msk;
	tmp |= QSPI_CTRLB_DLYCS(data);
	((Qspi *)hw)->CTRLB.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_DLYCS_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~QSPI_CTRLB_DLYCS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_DLYCS_bf(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= QSPI_CTRLB_DLYCS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_read_CTRLB_DLYCS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp = (tmp & QSPI_CTRLB_DLYCS_Msk) >> QSPI_CTRLB_DLYCS_Pos;
	return tmp;
}

static inline void hri_qspi_set_CTRLB_reg(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_get_CTRLB_reg(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_CTRLB_reg(const void *const hw, hri_qspi_ctrlb_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_CTRLB_reg(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_CTRLB_reg(const void *const hw, hri_qspi_ctrlb_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->CTRLB.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_ctrlb_reg_t hri_qspi_read_CTRLB_reg(const void *const hw)
{
	return ((Qspi *)hw)->CTRLB.reg;
}

static inline void hri_qspi_set_BAUD_CPOL_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg |= QSPI_BAUD_CPOL;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_BAUD_CPOL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_CPOL) >> QSPI_BAUD_CPOL_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_BAUD_CPOL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp &= ~QSPI_BAUD_CPOL;
	tmp |= value << QSPI_BAUD_CPOL_Pos;
	((Qspi *)hw)->BAUD.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_BAUD_CPOL_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg &= ~QSPI_BAUD_CPOL;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_BAUD_CPOL_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg ^= QSPI_BAUD_CPOL;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_BAUD_CPHA_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg |= QSPI_BAUD_CPHA;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_BAUD_CPHA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_CPHA) >> QSPI_BAUD_CPHA_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_BAUD_CPHA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp &= ~QSPI_BAUD_CPHA;
	tmp |= value << QSPI_BAUD_CPHA_Pos;
	((Qspi *)hw)->BAUD.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_BAUD_CPHA_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg &= ~QSPI_BAUD_CPHA;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_BAUD_CPHA_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg ^= QSPI_BAUD_CPHA;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_BAUD_BAUD_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg |= QSPI_BAUD_BAUD(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_get_BAUD_BAUD_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_BAUD(mask)) >> QSPI_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_qspi_write_BAUD_BAUD_bf(const void *const hw, hri_qspi_baud_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp &= ~QSPI_BAUD_BAUD_Msk;
	tmp |= QSPI_BAUD_BAUD(data);
	((Qspi *)hw)->BAUD.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_BAUD_BAUD_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg &= ~QSPI_BAUD_BAUD(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_BAUD_BAUD_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg ^= QSPI_BAUD_BAUD(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_read_BAUD_BAUD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_BAUD_Msk) >> QSPI_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_qspi_set_BAUD_DLYBS_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg |= QSPI_BAUD_DLYBS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_get_BAUD_DLYBS_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_DLYBS(mask)) >> QSPI_BAUD_DLYBS_Pos;
	return tmp;
}

static inline void hri_qspi_write_BAUD_DLYBS_bf(const void *const hw, hri_qspi_baud_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp &= ~QSPI_BAUD_DLYBS_Msk;
	tmp |= QSPI_BAUD_DLYBS(data);
	((Qspi *)hw)->BAUD.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_BAUD_DLYBS_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg &= ~QSPI_BAUD_DLYBS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_BAUD_DLYBS_bf(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg ^= QSPI_BAUD_DLYBS(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_read_BAUD_DLYBS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp = (tmp & QSPI_BAUD_DLYBS_Msk) >> QSPI_BAUD_DLYBS_Pos;
	return tmp;
}

static inline void hri_qspi_set_BAUD_reg(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_get_BAUD_reg(const void *const hw, hri_qspi_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_BAUD_reg(const void *const hw, hri_qspi_baud_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_BAUD_reg(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_BAUD_reg(const void *const hw, hri_qspi_baud_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->BAUD.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_baud_reg_t hri_qspi_read_BAUD_reg(const void *const hw)
{
	return ((Qspi *)hw)->BAUD.reg;
}

static inline void hri_qspi_set_INSTRADDR_ADDR_bf(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg |= QSPI_INSTRADDR_ADDR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instraddr_reg_t hri_qspi_get_INSTRADDR_ADDR_bf(const void *const        hw,
                                                                      hri_qspi_instraddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRADDR.reg;
	tmp = (tmp & QSPI_INSTRADDR_ADDR(mask)) >> QSPI_INSTRADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRADDR_ADDR_bf(const void *const hw, hri_qspi_instraddr_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRADDR.reg;
	tmp &= ~QSPI_INSTRADDR_ADDR_Msk;
	tmp |= QSPI_INSTRADDR_ADDR(data);
	((Qspi *)hw)->INSTRADDR.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRADDR_ADDR_bf(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg &= ~QSPI_INSTRADDR_ADDR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRADDR_ADDR_bf(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg ^= QSPI_INSTRADDR_ADDR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instraddr_reg_t hri_qspi_read_INSTRADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRADDR.reg;
	tmp = (tmp & QSPI_INSTRADDR_ADDR_Msk) >> QSPI_INSTRADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRADDR_reg(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instraddr_reg_t hri_qspi_get_INSTRADDR_reg(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_INSTRADDR_reg(const void *const hw, hri_qspi_instraddr_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRADDR_reg(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRADDR_reg(const void *const hw, hri_qspi_instraddr_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRADDR.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instraddr_reg_t hri_qspi_read_INSTRADDR_reg(const void *const hw)
{
	return ((Qspi *)hw)->INSTRADDR.reg;
}

static inline void hri_qspi_set_INSTRCTRL_INSTR_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg |= QSPI_INSTRCTRL_INSTR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_get_INSTRCTRL_INSTR_bf(const void *const        hw,
                                                                       hri_qspi_instrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp = (tmp & QSPI_INSTRCTRL_INSTR(mask)) >> QSPI_INSTRCTRL_INSTR_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRCTRL_INSTR_bf(const void *const hw, hri_qspi_instrctrl_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp &= ~QSPI_INSTRCTRL_INSTR_Msk;
	tmp |= QSPI_INSTRCTRL_INSTR(data);
	((Qspi *)hw)->INSTRCTRL.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRCTRL_INSTR_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg &= ~QSPI_INSTRCTRL_INSTR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRCTRL_INSTR_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg ^= QSPI_INSTRCTRL_INSTR(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_read_INSTRCTRL_INSTR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp = (tmp & QSPI_INSTRCTRL_INSTR_Msk) >> QSPI_INSTRCTRL_INSTR_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRCTRL_OPTCODE_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg |= QSPI_INSTRCTRL_OPTCODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_get_INSTRCTRL_OPTCODE_bf(const void *const        hw,
                                                                         hri_qspi_instrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp = (tmp & QSPI_INSTRCTRL_OPTCODE(mask)) >> QSPI_INSTRCTRL_OPTCODE_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRCTRL_OPTCODE_bf(const void *const hw, hri_qspi_instrctrl_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp &= ~QSPI_INSTRCTRL_OPTCODE_Msk;
	tmp |= QSPI_INSTRCTRL_OPTCODE(data);
	((Qspi *)hw)->INSTRCTRL.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRCTRL_OPTCODE_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg &= ~QSPI_INSTRCTRL_OPTCODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRCTRL_OPTCODE_bf(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg ^= QSPI_INSTRCTRL_OPTCODE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_read_INSTRCTRL_OPTCODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp = (tmp & QSPI_INSTRCTRL_OPTCODE_Msk) >> QSPI_INSTRCTRL_OPTCODE_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRCTRL_reg(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_get_INSTRCTRL_reg(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_INSTRCTRL_reg(const void *const hw, hri_qspi_instrctrl_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRCTRL_reg(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRCTRL_reg(const void *const hw, hri_qspi_instrctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRCTRL.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrctrl_reg_t hri_qspi_read_INSTRCTRL_reg(const void *const hw)
{
	return ((Qspi *)hw)->INSTRCTRL.reg;
}

static inline void hri_qspi_set_INSTRFRAME_INSTREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_INSTREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_INSTREN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_INSTREN) >> QSPI_INSTRFRAME_INSTREN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_INSTREN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_INSTREN;
	tmp |= value << QSPI_INSTRFRAME_INSTREN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_INSTREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_INSTREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_INSTREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_INSTREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_ADDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_ADDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_ADDREN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_ADDREN) >> QSPI_INSTRFRAME_ADDREN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_ADDREN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_ADDREN;
	tmp |= value << QSPI_INSTRFRAME_ADDREN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_ADDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_ADDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_ADDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_ADDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_OPTCODEEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_OPTCODEEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_OPTCODEEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_OPTCODEEN) >> QSPI_INSTRFRAME_OPTCODEEN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_OPTCODEEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_OPTCODEEN;
	tmp |= value << QSPI_INSTRFRAME_OPTCODEEN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_OPTCODEEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_OPTCODEEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_OPTCODEEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_OPTCODEEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_DATAEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_DATAEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_DATAEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_DATAEN) >> QSPI_INSTRFRAME_DATAEN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_DATAEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_DATAEN;
	tmp |= value << QSPI_INSTRFRAME_DATAEN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_DATAEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_DATAEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_DATAEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_DATAEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_ADDRLEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_ADDRLEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_ADDRLEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_ADDRLEN) >> QSPI_INSTRFRAME_ADDRLEN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_ADDRLEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_ADDRLEN;
	tmp |= value << QSPI_INSTRFRAME_ADDRLEN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_ADDRLEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_ADDRLEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_ADDRLEN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_ADDRLEN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_CRMODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_CRMODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_CRMODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_CRMODE) >> QSPI_INSTRFRAME_CRMODE_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_CRMODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_CRMODE;
	tmp |= value << QSPI_INSTRFRAME_CRMODE_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_CRMODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_CRMODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_CRMODE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_CRMODE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_DDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_DDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_INSTRFRAME_DDREN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_DDREN) >> QSPI_INSTRFRAME_DDREN_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_INSTRFRAME_DDREN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_DDREN;
	tmp |= value << QSPI_INSTRFRAME_DDREN_Pos;
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_DDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_DDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_DDREN_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_DDREN;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_INSTRFRAME_WIDTH_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_WIDTH(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_get_INSTRFRAME_WIDTH_bf(const void *const         hw,
                                                                         hri_qspi_instrframe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_WIDTH(mask)) >> QSPI_INSTRFRAME_WIDTH_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRFRAME_WIDTH_bf(const void *const hw, hri_qspi_instrframe_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_WIDTH_Msk;
	tmp |= QSPI_INSTRFRAME_WIDTH(data);
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_WIDTH_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_WIDTH(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_WIDTH_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_WIDTH(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_read_INSTRFRAME_WIDTH_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_WIDTH_Msk) >> QSPI_INSTRFRAME_WIDTH_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRFRAME_OPTCODELEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_OPTCODELEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_get_INSTRFRAME_OPTCODELEN_bf(const void *const         hw,
                                                                              hri_qspi_instrframe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_OPTCODELEN(mask)) >> QSPI_INSTRFRAME_OPTCODELEN_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRFRAME_OPTCODELEN_bf(const void *const hw, hri_qspi_instrframe_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_OPTCODELEN_Msk;
	tmp |= QSPI_INSTRFRAME_OPTCODELEN(data);
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_OPTCODELEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_OPTCODELEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_OPTCODELEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_OPTCODELEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_read_INSTRFRAME_OPTCODELEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_OPTCODELEN_Msk) >> QSPI_INSTRFRAME_OPTCODELEN_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRFRAME_TFRTYPE_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_TFRTYPE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_get_INSTRFRAME_TFRTYPE_bf(const void *const         hw,
                                                                           hri_qspi_instrframe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_TFRTYPE(mask)) >> QSPI_INSTRFRAME_TFRTYPE_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRFRAME_TFRTYPE_bf(const void *const hw, hri_qspi_instrframe_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_TFRTYPE_Msk;
	tmp |= QSPI_INSTRFRAME_TFRTYPE(data);
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_TFRTYPE_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_TFRTYPE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_TFRTYPE_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_TFRTYPE(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_read_INSTRFRAME_TFRTYPE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_TFRTYPE_Msk) >> QSPI_INSTRFRAME_TFRTYPE_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRFRAME_DUMMYLEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= QSPI_INSTRFRAME_DUMMYLEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_get_INSTRFRAME_DUMMYLEN_bf(const void *const         hw,
                                                                            hri_qspi_instrframe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_DUMMYLEN(mask)) >> QSPI_INSTRFRAME_DUMMYLEN_Pos;
	return tmp;
}

static inline void hri_qspi_write_INSTRFRAME_DUMMYLEN_bf(const void *const hw, hri_qspi_instrframe_reg_t data)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= ~QSPI_INSTRFRAME_DUMMYLEN_Msk;
	tmp |= QSPI_INSTRFRAME_DUMMYLEN(data);
	((Qspi *)hw)->INSTRFRAME.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_DUMMYLEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~QSPI_INSTRFRAME_DUMMYLEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_DUMMYLEN_bf(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= QSPI_INSTRFRAME_DUMMYLEN(mask);
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_read_INSTRFRAME_DUMMYLEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp = (tmp & QSPI_INSTRFRAME_DUMMYLEN_Msk) >> QSPI_INSTRFRAME_DUMMYLEN_Pos;
	return tmp;
}

static inline void hri_qspi_set_INSTRFRAME_reg(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_get_INSTRFRAME_reg(const void *const         hw,
                                                                    hri_qspi_instrframe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->INSTRFRAME.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_INSTRFRAME_reg(const void *const hw, hri_qspi_instrframe_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_INSTRFRAME_reg(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_INSTRFRAME_reg(const void *const hw, hri_qspi_instrframe_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->INSTRFRAME.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_instrframe_reg_t hri_qspi_read_INSTRFRAME_reg(const void *const hw)
{
	return ((Qspi *)hw)->INSTRFRAME.reg;
}

static inline void hri_qspi_set_SCRAMBCTRL_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg |= QSPI_SCRAMBCTRL_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_SCRAMBCTRL_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->SCRAMBCTRL.reg;
	tmp = (tmp & QSPI_SCRAMBCTRL_ENABLE) >> QSPI_SCRAMBCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_SCRAMBCTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->SCRAMBCTRL.reg;
	tmp &= ~QSPI_SCRAMBCTRL_ENABLE;
	tmp |= value << QSPI_SCRAMBCTRL_ENABLE_Pos;
	((Qspi *)hw)->SCRAMBCTRL.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_SCRAMBCTRL_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg &= ~QSPI_SCRAMBCTRL_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_SCRAMBCTRL_ENABLE_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg ^= QSPI_SCRAMBCTRL_ENABLE;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_SCRAMBCTRL_RANDOMDIS_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg |= QSPI_SCRAMBCTRL_RANDOMDIS;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_qspi_get_SCRAMBCTRL_RANDOMDIS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->SCRAMBCTRL.reg;
	tmp = (tmp & QSPI_SCRAMBCTRL_RANDOMDIS) >> QSPI_SCRAMBCTRL_RANDOMDIS_Pos;
	return (bool)tmp;
}

static inline void hri_qspi_write_SCRAMBCTRL_RANDOMDIS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	QSPI_CRITICAL_SECTION_ENTER();
	tmp = ((Qspi *)hw)->SCRAMBCTRL.reg;
	tmp &= ~QSPI_SCRAMBCTRL_RANDOMDIS;
	tmp |= value << QSPI_SCRAMBCTRL_RANDOMDIS_Pos;
	((Qspi *)hw)->SCRAMBCTRL.reg = tmp;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_SCRAMBCTRL_RANDOMDIS_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg &= ~QSPI_SCRAMBCTRL_RANDOMDIS;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_SCRAMBCTRL_RANDOMDIS_bit(const void *const hw)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg ^= QSPI_SCRAMBCTRL_RANDOMDIS;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_set_SCRAMBCTRL_reg(const void *const hw, hri_qspi_scrambctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg |= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_scrambctrl_reg_t hri_qspi_get_SCRAMBCTRL_reg(const void *const         hw,
                                                                    hri_qspi_scrambctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->SCRAMBCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_qspi_write_SCRAMBCTRL_reg(const void *const hw, hri_qspi_scrambctrl_reg_t data)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg = data;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_clear_SCRAMBCTRL_reg(const void *const hw, hri_qspi_scrambctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg &= ~mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline void hri_qspi_toggle_SCRAMBCTRL_reg(const void *const hw, hri_qspi_scrambctrl_reg_t mask)
{
	QSPI_CRITICAL_SECTION_ENTER();
	((Qspi *)hw)->SCRAMBCTRL.reg ^= mask;
	QSPI_CRITICAL_SECTION_LEAVE();
}

static inline hri_qspi_scrambctrl_reg_t hri_qspi_read_SCRAMBCTRL_reg(const void *const hw)
{
	return ((Qspi *)hw)->SCRAMBCTRL.reg;
}

static inline hri_qspi_rxdata_reg_t hri_qspi_get_RXDATA_DATA_bf(const void *const hw, hri_qspi_rxdata_reg_t mask)
{
	return (((Qspi *)hw)->RXDATA.reg & QSPI_RXDATA_DATA(mask)) >> QSPI_RXDATA_DATA_Pos;
}

static inline hri_qspi_rxdata_reg_t hri_qspi_read_RXDATA_DATA_bf(const void *const hw)
{
	return (((Qspi *)hw)->RXDATA.reg & QSPI_RXDATA_DATA_Msk) >> QSPI_RXDATA_DATA_Pos;
}

static inline hri_qspi_rxdata_reg_t hri_qspi_get_RXDATA_reg(const void *const hw, hri_qspi_rxdata_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->RXDATA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_qspi_rxdata_reg_t hri_qspi_read_RXDATA_reg(const void *const hw)
{
	return ((Qspi *)hw)->RXDATA.reg;
}

static inline bool hri_qspi_get_STATUS_ENABLE_bit(const void *const hw)
{
	return (((Qspi *)hw)->STATUS.reg & QSPI_STATUS_ENABLE) >> QSPI_STATUS_ENABLE_Pos;
}

static inline bool hri_qspi_get_STATUS_CSSTATUS_bit(const void *const hw)
{
	return (((Qspi *)hw)->STATUS.reg & QSPI_STATUS_CSSTATUS) >> QSPI_STATUS_CSSTATUS_Pos;
}

static inline hri_qspi_status_reg_t hri_qspi_get_STATUS_reg(const void *const hw, hri_qspi_status_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Qspi *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_qspi_status_reg_t hri_qspi_read_STATUS_reg(const void *const hw)
{
	return ((Qspi *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_QSPI_D51_H_INCLUDED */
#endif /* _SAMD51_QSPI_COMPONENT_ */
