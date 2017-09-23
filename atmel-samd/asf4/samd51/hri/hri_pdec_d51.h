/**
 * \file
 *
 * \brief SAM PDEC
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

#ifdef _SAMD51_PDEC_COMPONENT_
#ifndef _HRI_PDEC_D51_H_INCLUDED_
#define _HRI_PDEC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PDEC_CRITICAL_SECTIONS)
#define PDEC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define PDEC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define PDEC_CRITICAL_SECTION_ENTER()
#define PDEC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_pdec_evctrl_reg_t;
typedef uint16_t hri_pdec_status_reg_t;
typedef uint32_t hri_pdec_cc_reg_t;
typedef uint32_t hri_pdec_ccbuf_reg_t;
typedef uint32_t hri_pdec_count_reg_t;
typedef uint32_t hri_pdec_ctrla_reg_t;
typedef uint32_t hri_pdec_syncbusy_reg_t;
typedef uint8_t  hri_pdec_ctrlbset_reg_t;
typedef uint8_t  hri_pdec_dbgctrl_reg_t;
typedef uint8_t  hri_pdec_filter_reg_t;
typedef uint8_t  hri_pdec_filterbuf_reg_t;
typedef uint8_t  hri_pdec_intenset_reg_t;
typedef uint8_t  hri_pdec_intflag_reg_t;
typedef uint8_t  hri_pdec_presc_reg_t;
typedef uint8_t  hri_pdec_prescbuf_reg_t;

static inline void hri_pdec_wait_for_sync(const void *const hw, hri_pdec_syncbusy_reg_t reg)
{
	while (((Pdec *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_pdec_is_syncing(const void *const hw, hri_pdec_syncbusy_reg_t reg)
{
	return ((Pdec *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_pdec_set_CTRLB_LUPD_bit(const void *const hw)
{
	((Pdec *)hw)->CTRLBSET.reg = PDEC_CTRLBSET_LUPD;
}

static inline bool hri_pdec_get_CTRLB_LUPD_bit(const void *const hw)
{
	return (((Pdec *)hw)->CTRLBSET.reg & PDEC_CTRLBSET_LUPD) >> PDEC_CTRLBSET_LUPD_Pos;
}

static inline void hri_pdec_write_CTRLB_LUPD_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->CTRLBCLR.reg = PDEC_CTRLBSET_LUPD;
	} else {
		((Pdec *)hw)->CTRLBSET.reg = PDEC_CTRLBSET_LUPD;
	}
}

static inline void hri_pdec_clear_CTRLB_LUPD_bit(const void *const hw)
{
	((Pdec *)hw)->CTRLBCLR.reg = PDEC_CTRLBSET_LUPD;
}

static inline void hri_pdec_set_CTRLB_CMD_bf(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	((Pdec *)hw)->CTRLBSET.reg = PDEC_CTRLBSET_CMD(mask);
}

static inline hri_pdec_ctrlbset_reg_t hri_pdec_get_CTRLB_CMD_bf(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->CTRLBSET.reg;
	tmp = (tmp & PDEC_CTRLBSET_CMD(mask)) >> PDEC_CTRLBSET_CMD_Pos;
	return tmp;
}

static inline hri_pdec_ctrlbset_reg_t hri_pdec_read_CTRLB_CMD_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->CTRLBSET.reg;
	tmp = (tmp & PDEC_CTRLBSET_CMD_Msk) >> PDEC_CTRLBSET_CMD_Pos;
	return tmp;
}

static inline void hri_pdec_write_CTRLB_CMD_bf(const void *const hw, hri_pdec_ctrlbset_reg_t data)
{
	((Pdec *)hw)->CTRLBSET.reg = PDEC_CTRLBSET_CMD(data);
	((Pdec *)hw)->CTRLBCLR.reg = ~PDEC_CTRLBSET_CMD(data);
}

static inline void hri_pdec_clear_CTRLB_CMD_bf(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	((Pdec *)hw)->CTRLBCLR.reg = PDEC_CTRLBSET_CMD(mask);
}

static inline void hri_pdec_set_CTRLB_reg(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	((Pdec *)hw)->CTRLBSET.reg = mask;
}

static inline hri_pdec_ctrlbset_reg_t hri_pdec_get_CTRLB_reg(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->CTRLBSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pdec_ctrlbset_reg_t hri_pdec_read_CTRLB_reg(const void *const hw)
{
	return ((Pdec *)hw)->CTRLBSET.reg;
}

static inline void hri_pdec_write_CTRLB_reg(const void *const hw, hri_pdec_ctrlbset_reg_t data)
{
	((Pdec *)hw)->CTRLBSET.reg = data;
	((Pdec *)hw)->CTRLBCLR.reg = ~data;
}

static inline void hri_pdec_clear_CTRLB_reg(const void *const hw, hri_pdec_ctrlbset_reg_t mask)
{
	((Pdec *)hw)->CTRLBCLR.reg = mask;
}

static inline void hri_pdec_set_INTEN_OVF_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_OVF;
}

static inline bool hri_pdec_get_INTEN_OVF_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_OVF) >> PDEC_INTENSET_OVF_Pos;
}

static inline void hri_pdec_write_INTEN_OVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_OVF;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_OVF;
	}
}

static inline void hri_pdec_clear_INTEN_OVF_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_OVF;
}

static inline void hri_pdec_set_INTEN_ERR_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_ERR;
}

static inline bool hri_pdec_get_INTEN_ERR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_ERR) >> PDEC_INTENSET_ERR_Pos;
}

static inline void hri_pdec_write_INTEN_ERR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_ERR;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_ERR;
	}
}

static inline void hri_pdec_clear_INTEN_ERR_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_ERR;
}

static inline void hri_pdec_set_INTEN_DIR_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_DIR;
}

static inline bool hri_pdec_get_INTEN_DIR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_DIR) >> PDEC_INTENSET_DIR_Pos;
}

static inline void hri_pdec_write_INTEN_DIR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_DIR;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_DIR;
	}
}

static inline void hri_pdec_clear_INTEN_DIR_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_DIR;
}

static inline void hri_pdec_set_INTEN_VLC_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_VLC;
}

static inline bool hri_pdec_get_INTEN_VLC_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_VLC) >> PDEC_INTENSET_VLC_Pos;
}

static inline void hri_pdec_write_INTEN_VLC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_VLC;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_VLC;
	}
}

static inline void hri_pdec_clear_INTEN_VLC_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_VLC;
}

static inline void hri_pdec_set_INTEN_MC0_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_MC0;
}

static inline bool hri_pdec_get_INTEN_MC0_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_MC0) >> PDEC_INTENSET_MC0_Pos;
}

static inline void hri_pdec_write_INTEN_MC0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_MC0;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_MC0;
	}
}

static inline void hri_pdec_clear_INTEN_MC0_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_MC0;
}

static inline void hri_pdec_set_INTEN_MC1_bit(const void *const hw)
{
	((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_MC1;
}

static inline bool hri_pdec_get_INTEN_MC1_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTENSET.reg & PDEC_INTENSET_MC1) >> PDEC_INTENSET_MC1_Pos;
}

static inline void hri_pdec_write_INTEN_MC1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_MC1;
	} else {
		((Pdec *)hw)->INTENSET.reg = PDEC_INTENSET_MC1;
	}
}

static inline void hri_pdec_clear_INTEN_MC1_bit(const void *const hw)
{
	((Pdec *)hw)->INTENCLR.reg = PDEC_INTENSET_MC1;
}

static inline void hri_pdec_set_INTEN_reg(const void *const hw, hri_pdec_intenset_reg_t mask)
{
	((Pdec *)hw)->INTENSET.reg = mask;
}

static inline hri_pdec_intenset_reg_t hri_pdec_get_INTEN_reg(const void *const hw, hri_pdec_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pdec_intenset_reg_t hri_pdec_read_INTEN_reg(const void *const hw)
{
	return ((Pdec *)hw)->INTENSET.reg;
}

static inline void hri_pdec_write_INTEN_reg(const void *const hw, hri_pdec_intenset_reg_t data)
{
	((Pdec *)hw)->INTENSET.reg = data;
	((Pdec *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_pdec_clear_INTEN_reg(const void *const hw, hri_pdec_intenset_reg_t mask)
{
	((Pdec *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_pdec_get_INTFLAG_OVF_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_OVF) >> PDEC_INTFLAG_OVF_Pos;
}

static inline void hri_pdec_clear_INTFLAG_OVF_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_OVF;
}

static inline bool hri_pdec_get_INTFLAG_ERR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_ERR) >> PDEC_INTFLAG_ERR_Pos;
}

static inline void hri_pdec_clear_INTFLAG_ERR_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_ERR;
}

static inline bool hri_pdec_get_INTFLAG_DIR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_DIR) >> PDEC_INTFLAG_DIR_Pos;
}

static inline void hri_pdec_clear_INTFLAG_DIR_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_DIR;
}

static inline bool hri_pdec_get_INTFLAG_VLC_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_VLC) >> PDEC_INTFLAG_VLC_Pos;
}

static inline void hri_pdec_clear_INTFLAG_VLC_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_VLC;
}

static inline bool hri_pdec_get_INTFLAG_MC0_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_MC0) >> PDEC_INTFLAG_MC0_Pos;
}

static inline void hri_pdec_clear_INTFLAG_MC0_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_MC0;
}

static inline bool hri_pdec_get_INTFLAG_MC1_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_MC1) >> PDEC_INTFLAG_MC1_Pos;
}

static inline void hri_pdec_clear_INTFLAG_MC1_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_MC1;
}

static inline bool hri_pdec_get_interrupt_OVF_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_OVF) >> PDEC_INTFLAG_OVF_Pos;
}

static inline void hri_pdec_clear_interrupt_OVF_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_OVF;
}

static inline bool hri_pdec_get_interrupt_ERR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_ERR) >> PDEC_INTFLAG_ERR_Pos;
}

static inline void hri_pdec_clear_interrupt_ERR_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_ERR;
}

static inline bool hri_pdec_get_interrupt_DIR_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_DIR) >> PDEC_INTFLAG_DIR_Pos;
}

static inline void hri_pdec_clear_interrupt_DIR_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_DIR;
}

static inline bool hri_pdec_get_interrupt_VLC_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_VLC) >> PDEC_INTFLAG_VLC_Pos;
}

static inline void hri_pdec_clear_interrupt_VLC_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_VLC;
}

static inline bool hri_pdec_get_interrupt_MC0_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_MC0) >> PDEC_INTFLAG_MC0_Pos;
}

static inline void hri_pdec_clear_interrupt_MC0_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_MC0;
}

static inline bool hri_pdec_get_interrupt_MC1_bit(const void *const hw)
{
	return (((Pdec *)hw)->INTFLAG.reg & PDEC_INTFLAG_MC1) >> PDEC_INTFLAG_MC1_Pos;
}

static inline void hri_pdec_clear_interrupt_MC1_bit(const void *const hw)
{
	((Pdec *)hw)->INTFLAG.reg = PDEC_INTFLAG_MC1;
}

static inline hri_pdec_intflag_reg_t hri_pdec_get_INTFLAG_reg(const void *const hw, hri_pdec_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pdec_intflag_reg_t hri_pdec_read_INTFLAG_reg(const void *const hw)
{
	return ((Pdec *)hw)->INTFLAG.reg;
}

static inline void hri_pdec_clear_INTFLAG_reg(const void *const hw, hri_pdec_intflag_reg_t mask)
{
	((Pdec *)hw)->INTFLAG.reg = mask;
}

static inline void hri_pdec_set_CTRLA_SWRST_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_SWRST;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_SWRST) >> PDEC_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_set_CTRLA_ENABLE_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST | PDEC_SYNCBUSY_ENABLE);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_ENABLE;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST | PDEC_SYNCBUSY_ENABLE);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_ENABLE) >> PDEC_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST | PDEC_SYNCBUSY_ENABLE);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_ENABLE;
	tmp |= value << PDEC_CTRLA_ENABLE_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST | PDEC_SYNCBUSY_ENABLE);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_ENABLE;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_SWRST | PDEC_SYNCBUSY_ENABLE);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_ENABLE;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_RUNSTDBY;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_RUNSTDBY) >> PDEC_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_RUNSTDBY;
	tmp |= value << PDEC_CTRLA_RUNSTDBY_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_RUNSTDBY;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_RUNSTDBY;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_ALOCK_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_ALOCK;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_ALOCK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_ALOCK) >> PDEC_CTRLA_ALOCK_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_ALOCK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_ALOCK;
	tmp |= value << PDEC_CTRLA_ALOCK_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_ALOCK_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_ALOCK;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_ALOCK_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_ALOCK;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_SWAP_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_SWAP;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_SWAP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_SWAP) >> PDEC_CTRLA_SWAP_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_SWAP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_SWAP;
	tmp |= value << PDEC_CTRLA_SWAP_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_SWAP_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_SWAP;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_SWAP_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_SWAP;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PEREN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PEREN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PEREN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PEREN) >> PDEC_CTRLA_PEREN_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PEREN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PEREN;
	tmp |= value << PDEC_CTRLA_PEREN_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PEREN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PEREN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PEREN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PEREN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINEN0) >> PDEC_CTRLA_PINEN0_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINEN0;
	tmp |= value << PDEC_CTRLA_PINEN0_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINEN1) >> PDEC_CTRLA_PINEN1_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINEN1;
	tmp |= value << PDEC_CTRLA_PINEN1_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINEN2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINEN2) >> PDEC_CTRLA_PINEN2_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINEN2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINEN2;
	tmp |= value << PDEC_CTRLA_PINEN2_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINVEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINVEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINVEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINVEN0) >> PDEC_CTRLA_PINVEN0_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINVEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINVEN0;
	tmp |= value << PDEC_CTRLA_PINVEN0_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINVEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINVEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINVEN0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINVEN0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINVEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINVEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINVEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINVEN1) >> PDEC_CTRLA_PINVEN1_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINVEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINVEN1;
	tmp |= value << PDEC_CTRLA_PINVEN1_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINVEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINVEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINVEN1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINVEN1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_PINVEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_PINVEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_CTRLA_PINVEN2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_PINVEN2) >> PDEC_CTRLA_PINVEN2_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_CTRLA_PINVEN2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_PINVEN2;
	tmp |= value << PDEC_CTRLA_PINVEN2_Pos;
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_PINVEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_PINVEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_PINVEN2_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_PINVEN2;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_CTRLA_MODE_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_MODE(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_get_CTRLA_MODE_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_MODE(mask)) >> PDEC_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_pdec_write_CTRLA_MODE_bf(const void *const hw, hri_pdec_ctrla_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_MODE_Msk;
	tmp |= PDEC_CTRLA_MODE(data);
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_MODE_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_MODE(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_MODE_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_MODE(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_MODE_Msk) >> PDEC_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_pdec_set_CTRLA_CONF_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_CONF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_get_CTRLA_CONF_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_CONF(mask)) >> PDEC_CTRLA_CONF_Pos;
	return tmp;
}

static inline void hri_pdec_write_CTRLA_CONF_bf(const void *const hw, hri_pdec_ctrla_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_CONF_Msk;
	tmp |= PDEC_CTRLA_CONF(data);
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_CONF_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_CONF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_CONF_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_CONF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_read_CTRLA_CONF_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_CONF_Msk) >> PDEC_CTRLA_CONF_Pos;
	return tmp;
}

static inline void hri_pdec_set_CTRLA_ANGULAR_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_ANGULAR(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_get_CTRLA_ANGULAR_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_ANGULAR(mask)) >> PDEC_CTRLA_ANGULAR_Pos;
	return tmp;
}

static inline void hri_pdec_write_CTRLA_ANGULAR_bf(const void *const hw, hri_pdec_ctrla_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_ANGULAR_Msk;
	tmp |= PDEC_CTRLA_ANGULAR(data);
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_ANGULAR_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_ANGULAR(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_ANGULAR_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_ANGULAR(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_read_CTRLA_ANGULAR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_ANGULAR_Msk) >> PDEC_CTRLA_ANGULAR_Pos;
	return tmp;
}

static inline void hri_pdec_set_CTRLA_MAXCMP_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg |= PDEC_CTRLA_MAXCMP(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_get_CTRLA_MAXCMP_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_MAXCMP(mask)) >> PDEC_CTRLA_MAXCMP_Pos;
	return tmp;
}

static inline void hri_pdec_write_CTRLA_MAXCMP_bf(const void *const hw, hri_pdec_ctrla_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= ~PDEC_CTRLA_MAXCMP_Msk;
	tmp |= PDEC_CTRLA_MAXCMP(data);
	((Pdec *)hw)->CTRLA.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_MAXCMP_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg &= ~PDEC_CTRLA_MAXCMP(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_MAXCMP_bf(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CTRLA.reg ^= PDEC_CTRLA_MAXCMP(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_read_CTRLA_MAXCMP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp = (tmp & PDEC_CTRLA_MAXCMP_Msk) >> PDEC_CTRLA_MAXCMP_Pos;
	return tmp;
}

static inline void hri_pdec_set_CTRLA_reg(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CTRLA.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_get_CTRLA_reg(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_CTRLA_reg(const void *const hw, hri_pdec_ctrla_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CTRLA.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CTRLA_reg(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CTRLA.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CTRLA_reg(const void *const hw, hri_pdec_ctrla_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CTRLA.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ctrla_reg_t hri_pdec_read_CTRLA_reg(const void *const hw)
{
	return ((Pdec *)hw)->CTRLA.reg;
}

static inline void hri_pdec_set_EVCTRL_OVFEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_OVFEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_OVFEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_OVFEO) >> PDEC_EVCTRL_OVFEO_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_OVFEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_OVFEO;
	tmp |= value << PDEC_EVCTRL_OVFEO_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_OVFEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_OVFEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_OVFEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_OVFEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_ERREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_ERREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_ERREO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_ERREO) >> PDEC_EVCTRL_ERREO_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_ERREO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_ERREO;
	tmp |= value << PDEC_EVCTRL_ERREO_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_ERREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_ERREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_ERREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_ERREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_DIREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_DIREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_DIREO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_DIREO) >> PDEC_EVCTRL_DIREO_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_DIREO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_DIREO;
	tmp |= value << PDEC_EVCTRL_DIREO_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_DIREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_DIREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_DIREO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_DIREO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_VLCEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_VLCEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_VLCEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_VLCEO) >> PDEC_EVCTRL_VLCEO_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_VLCEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_VLCEO;
	tmp |= value << PDEC_EVCTRL_VLCEO_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_VLCEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_VLCEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_VLCEO_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_VLCEO;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_MCEO0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_MCEO0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_MCEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_MCEO0) >> PDEC_EVCTRL_MCEO0_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_MCEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_MCEO0;
	tmp |= value << PDEC_EVCTRL_MCEO0_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_MCEO0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_MCEO0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_MCEO0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_MCEO0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_MCEO1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_MCEO1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_EVCTRL_MCEO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_MCEO1) >> PDEC_EVCTRL_MCEO1_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_EVCTRL_MCEO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_MCEO1;
	tmp |= value << PDEC_EVCTRL_MCEO1_Pos;
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_MCEO1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_MCEO1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_MCEO1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_MCEO1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_EVCTRL_EVACT_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_EVACT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_get_EVCTRL_EVACT_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVACT(mask)) >> PDEC_EVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_pdec_write_EVCTRL_EVACT_bf(const void *const hw, hri_pdec_evctrl_reg_t data)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_EVACT_Msk;
	tmp |= PDEC_EVCTRL_EVACT(data);
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_EVACT_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_EVACT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_EVACT_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_EVACT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_read_EVCTRL_EVACT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVACT_Msk) >> PDEC_EVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_pdec_set_EVCTRL_EVINV_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_EVINV(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_get_EVCTRL_EVINV_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVINV(mask)) >> PDEC_EVCTRL_EVINV_Pos;
	return tmp;
}

static inline void hri_pdec_write_EVCTRL_EVINV_bf(const void *const hw, hri_pdec_evctrl_reg_t data)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_EVINV_Msk;
	tmp |= PDEC_EVCTRL_EVINV(data);
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_EVINV_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_EVINV(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_EVINV_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_EVINV(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_read_EVCTRL_EVINV_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVINV_Msk) >> PDEC_EVCTRL_EVINV_Pos;
	return tmp;
}

static inline void hri_pdec_set_EVCTRL_EVEI_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= PDEC_EVCTRL_EVEI(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_get_EVCTRL_EVEI_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVEI(mask)) >> PDEC_EVCTRL_EVEI_Pos;
	return tmp;
}

static inline void hri_pdec_write_EVCTRL_EVEI_bf(const void *const hw, hri_pdec_evctrl_reg_t data)
{
	uint16_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= ~PDEC_EVCTRL_EVEI_Msk;
	tmp |= PDEC_EVCTRL_EVEI(data);
	((Pdec *)hw)->EVCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_EVEI_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~PDEC_EVCTRL_EVEI(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_EVEI_bf(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= PDEC_EVCTRL_EVEI(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_read_EVCTRL_EVEI_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp = (tmp & PDEC_EVCTRL_EVEI_Msk) >> PDEC_EVCTRL_EVEI_Pos;
	return tmp;
}

static inline void hri_pdec_set_EVCTRL_reg(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_get_EVCTRL_reg(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_EVCTRL_reg(const void *const hw, hri_pdec_evctrl_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_EVCTRL_reg(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_EVCTRL_reg(const void *const hw, hri_pdec_evctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->EVCTRL.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_evctrl_reg_t hri_pdec_read_EVCTRL_reg(const void *const hw)
{
	return ((Pdec *)hw)->EVCTRL.reg;
}

static inline void hri_pdec_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg |= PDEC_DBGCTRL_DBGRUN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->DBGCTRL.reg;
	tmp = (tmp & PDEC_DBGCTRL_DBGRUN) >> PDEC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_pdec_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	tmp = ((Pdec *)hw)->DBGCTRL.reg;
	tmp &= ~PDEC_DBGCTRL_DBGRUN;
	tmp |= value << PDEC_DBGCTRL_DBGRUN_Pos;
	((Pdec *)hw)->DBGCTRL.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg &= ~PDEC_DBGCTRL_DBGRUN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg ^= PDEC_DBGCTRL_DBGRUN;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_set_DBGCTRL_reg(const void *const hw, hri_pdec_dbgctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_dbgctrl_reg_t hri_pdec_get_DBGCTRL_reg(const void *const hw, hri_pdec_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_DBGCTRL_reg(const void *const hw, hri_pdec_dbgctrl_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_DBGCTRL_reg(const void *const hw, hri_pdec_dbgctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_DBGCTRL_reg(const void *const hw, hri_pdec_dbgctrl_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->DBGCTRL.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_dbgctrl_reg_t hri_pdec_read_DBGCTRL_reg(const void *const hw)
{
	return ((Pdec *)hw)->DBGCTRL.reg;
}

static inline void hri_pdec_set_PRESC_PRESC_bf(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	((Pdec *)hw)->PRESC.reg |= PDEC_PRESC_PRESC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_presc_reg_t hri_pdec_get_PRESC_PRESC_bf(const void *const hw, hri_pdec_presc_reg_t mask)
{
	uint8_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	tmp = ((Pdec *)hw)->PRESC.reg;
	tmp = (tmp & PDEC_PRESC_PRESC(mask)) >> PDEC_PRESC_PRESC_Pos;
	return tmp;
}

static inline void hri_pdec_write_PRESC_PRESC_bf(const void *const hw, hri_pdec_presc_reg_t data)
{
	uint8_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	tmp = ((Pdec *)hw)->PRESC.reg;
	tmp &= ~PDEC_PRESC_PRESC_Msk;
	tmp |= PDEC_PRESC_PRESC(data);
	((Pdec *)hw)->PRESC.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_PRESC_PRESC_bf(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	((Pdec *)hw)->PRESC.reg &= ~PDEC_PRESC_PRESC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_PRESC_PRESC_bf(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	((Pdec *)hw)->PRESC.reg ^= PDEC_PRESC_PRESC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_presc_reg_t hri_pdec_read_PRESC_PRESC_bf(const void *const hw)
{
	uint8_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_PRESC);
	tmp = ((Pdec *)hw)->PRESC.reg;
	tmp = (tmp & PDEC_PRESC_PRESC_Msk) >> PDEC_PRESC_PRESC_Pos;
	return tmp;
}

static inline void hri_pdec_set_PRESC_reg(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESC.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_presc_reg_t hri_pdec_get_PRESC_reg(const void *const hw, hri_pdec_presc_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->PRESC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_PRESC_reg(const void *const hw, hri_pdec_presc_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESC.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_PRESC_reg(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESC.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_PRESC_reg(const void *const hw, hri_pdec_presc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESC.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_presc_reg_t hri_pdec_read_PRESC_reg(const void *const hw)
{
	return ((Pdec *)hw)->PRESC.reg;
}

static inline void hri_pdec_set_FILTER_FILTER_bf(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	((Pdec *)hw)->FILTER.reg |= PDEC_FILTER_FILTER(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filter_reg_t hri_pdec_get_FILTER_FILTER_bf(const void *const hw, hri_pdec_filter_reg_t mask)
{
	uint8_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	tmp = ((Pdec *)hw)->FILTER.reg;
	tmp = (tmp & PDEC_FILTER_FILTER(mask)) >> PDEC_FILTER_FILTER_Pos;
	return tmp;
}

static inline void hri_pdec_write_FILTER_FILTER_bf(const void *const hw, hri_pdec_filter_reg_t data)
{
	uint8_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	tmp = ((Pdec *)hw)->FILTER.reg;
	tmp &= ~PDEC_FILTER_FILTER_Msk;
	tmp |= PDEC_FILTER_FILTER(data);
	((Pdec *)hw)->FILTER.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_FILTER_FILTER_bf(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	((Pdec *)hw)->FILTER.reg &= ~PDEC_FILTER_FILTER(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_FILTER_FILTER_bf(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	((Pdec *)hw)->FILTER.reg ^= PDEC_FILTER_FILTER(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filter_reg_t hri_pdec_read_FILTER_FILTER_bf(const void *const hw)
{
	uint8_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_FILTER);
	tmp = ((Pdec *)hw)->FILTER.reg;
	tmp = (tmp & PDEC_FILTER_FILTER_Msk) >> PDEC_FILTER_FILTER_Pos;
	return tmp;
}

static inline void hri_pdec_set_FILTER_reg(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTER.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filter_reg_t hri_pdec_get_FILTER_reg(const void *const hw, hri_pdec_filter_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->FILTER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_FILTER_reg(const void *const hw, hri_pdec_filter_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTER.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_FILTER_reg(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTER.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_FILTER_reg(const void *const hw, hri_pdec_filter_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTER.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filter_reg_t hri_pdec_read_FILTER_reg(const void *const hw)
{
	return ((Pdec *)hw)->FILTER.reg;
}

static inline void hri_pdec_set_PRESCBUF_PRESCBUF_bf(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->PRESCBUF.reg |= PDEC_PRESCBUF_PRESCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_prescbuf_reg_t hri_pdec_get_PRESCBUF_PRESCBUF_bf(const void *const       hw,
                                                                        hri_pdec_prescbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->PRESCBUF.reg;
	tmp = (tmp & PDEC_PRESCBUF_PRESCBUF(mask)) >> PDEC_PRESCBUF_PRESCBUF_Pos;
	return tmp;
}

static inline void hri_pdec_write_PRESCBUF_PRESCBUF_bf(const void *const hw, hri_pdec_prescbuf_reg_t data)
{
	uint8_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->PRESCBUF.reg;
	tmp &= ~PDEC_PRESCBUF_PRESCBUF_Msk;
	tmp |= PDEC_PRESCBUF_PRESCBUF(data);
	((Pdec *)hw)->PRESCBUF.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_PRESCBUF_PRESCBUF_bf(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->PRESCBUF.reg &= ~PDEC_PRESCBUF_PRESCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_PRESCBUF_PRESCBUF_bf(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->PRESCBUF.reg ^= PDEC_PRESCBUF_PRESCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_prescbuf_reg_t hri_pdec_read_PRESCBUF_PRESCBUF_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->PRESCBUF.reg;
	tmp = (tmp & PDEC_PRESCBUF_PRESCBUF_Msk) >> PDEC_PRESCBUF_PRESCBUF_Pos;
	return tmp;
}

static inline void hri_pdec_set_PRESCBUF_reg(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESCBUF.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_prescbuf_reg_t hri_pdec_get_PRESCBUF_reg(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->PRESCBUF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_PRESCBUF_reg(const void *const hw, hri_pdec_prescbuf_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESCBUF.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_PRESCBUF_reg(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESCBUF.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_PRESCBUF_reg(const void *const hw, hri_pdec_prescbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->PRESCBUF.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_prescbuf_reg_t hri_pdec_read_PRESCBUF_reg(const void *const hw)
{
	return ((Pdec *)hw)->PRESCBUF.reg;
}

static inline void hri_pdec_set_FILTERBUF_FILTERBUF_bf(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->FILTERBUF.reg |= PDEC_FILTERBUF_FILTERBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filterbuf_reg_t hri_pdec_get_FILTERBUF_FILTERBUF_bf(const void *const        hw,
                                                                           hri_pdec_filterbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->FILTERBUF.reg;
	tmp = (tmp & PDEC_FILTERBUF_FILTERBUF(mask)) >> PDEC_FILTERBUF_FILTERBUF_Pos;
	return tmp;
}

static inline void hri_pdec_write_FILTERBUF_FILTERBUF_bf(const void *const hw, hri_pdec_filterbuf_reg_t data)
{
	uint8_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->FILTERBUF.reg;
	tmp &= ~PDEC_FILTERBUF_FILTERBUF_Msk;
	tmp |= PDEC_FILTERBUF_FILTERBUF(data);
	((Pdec *)hw)->FILTERBUF.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_FILTERBUF_FILTERBUF_bf(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->FILTERBUF.reg &= ~PDEC_FILTERBUF_FILTERBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_FILTERBUF_FILTERBUF_bf(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->FILTERBUF.reg ^= PDEC_FILTERBUF_FILTERBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filterbuf_reg_t hri_pdec_read_FILTERBUF_FILTERBUF_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->FILTERBUF.reg;
	tmp = (tmp & PDEC_FILTERBUF_FILTERBUF_Msk) >> PDEC_FILTERBUF_FILTERBUF_Pos;
	return tmp;
}

static inline void hri_pdec_set_FILTERBUF_reg(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTERBUF.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filterbuf_reg_t hri_pdec_get_FILTERBUF_reg(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pdec *)hw)->FILTERBUF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_FILTERBUF_reg(const void *const hw, hri_pdec_filterbuf_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTERBUF.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_FILTERBUF_reg(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTERBUF.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_FILTERBUF_reg(const void *const hw, hri_pdec_filterbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->FILTERBUF.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_filterbuf_reg_t hri_pdec_read_FILTERBUF_reg(const void *const hw)
{
	return ((Pdec *)hw)->FILTERBUF.reg;
}

static inline void hri_pdec_set_COUNT_COUNT_bf(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	((Pdec *)hw)->COUNT.reg |= PDEC_COUNT_COUNT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_count_reg_t hri_pdec_get_COUNT_COUNT_bf(const void *const hw, hri_pdec_count_reg_t mask)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	tmp = ((Pdec *)hw)->COUNT.reg;
	tmp = (tmp & PDEC_COUNT_COUNT(mask)) >> PDEC_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_pdec_write_COUNT_COUNT_bf(const void *const hw, hri_pdec_count_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	tmp = ((Pdec *)hw)->COUNT.reg;
	tmp &= ~PDEC_COUNT_COUNT_Msk;
	tmp |= PDEC_COUNT_COUNT(data);
	((Pdec *)hw)->COUNT.reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_COUNT_COUNT_bf(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	((Pdec *)hw)->COUNT.reg &= ~PDEC_COUNT_COUNT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_COUNT_COUNT_bf(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	((Pdec *)hw)->COUNT.reg ^= PDEC_COUNT_COUNT(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_count_reg_t hri_pdec_read_COUNT_COUNT_bf(const void *const hw)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_COUNT);
	tmp = ((Pdec *)hw)->COUNT.reg;
	tmp = (tmp & PDEC_COUNT_COUNT_Msk) >> PDEC_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_pdec_set_COUNT_reg(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->COUNT.reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_count_reg_t hri_pdec_get_COUNT_reg(const void *const hw, hri_pdec_count_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_COUNT_reg(const void *const hw, hri_pdec_count_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->COUNT.reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_COUNT_reg(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->COUNT.reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_COUNT_reg(const void *const hw, hri_pdec_count_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->COUNT.reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_count_reg_t hri_pdec_read_COUNT_reg(const void *const hw)
{
	return ((Pdec *)hw)->COUNT.reg;
}

static inline void hri_pdec_set_CC_CC_bf(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CC[index].reg |= PDEC_CC_CC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_cc_reg_t hri_pdec_get_CC_CC_bf(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CC[index].reg;
	tmp = (tmp & PDEC_CC_CC(mask)) >> PDEC_CC_CC_Pos;
	return tmp;
}

static inline void hri_pdec_write_CC_CC_bf(const void *const hw, uint8_t index, hri_pdec_cc_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CC[index].reg;
	tmp &= ~PDEC_CC_CC_Msk;
	tmp |= PDEC_CC_CC(data);
	((Pdec *)hw)->CC[index].reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CC_CC_bf(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CC[index].reg &= ~PDEC_CC_CC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CC_CC_bf(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CC[index].reg ^= PDEC_CC_CC(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_cc_reg_t hri_pdec_read_CC_CC_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CC[index].reg;
	tmp = (tmp & PDEC_CC_CC_Msk) >> PDEC_CC_CC_Pos;
	return tmp;
}

static inline void hri_pdec_set_CC_reg(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CC[index].reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_cc_reg_t hri_pdec_get_CC_reg(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CC[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_CC_reg(const void *const hw, uint8_t index, hri_pdec_cc_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CC[index].reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CC_reg(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CC[index].reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CC_reg(const void *const hw, uint8_t index, hri_pdec_cc_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CC[index].reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_cc_reg_t hri_pdec_read_CC_reg(const void *const hw, uint8_t index)
{
	return ((Pdec *)hw)->CC[index].reg;
}

static inline void hri_pdec_set_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CCBUF[index].reg |= PDEC_CCBUF_CCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ccbuf_reg_t hri_pdec_get_CCBUF_CCBUF_bf(const void *const hw, uint8_t index,
                                                               hri_pdec_ccbuf_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CCBUF[index].reg;
	tmp = (tmp & PDEC_CCBUF_CCBUF(mask)) >> PDEC_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_pdec_write_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t data)
{
	uint32_t tmp;
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	tmp = ((Pdec *)hw)->CCBUF[index].reg;
	tmp &= ~PDEC_CCBUF_CCBUF_Msk;
	tmp |= PDEC_CCBUF_CCBUF(data);
	((Pdec *)hw)->CCBUF[index].reg = tmp;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CCBUF[index].reg &= ~PDEC_CCBUF_CCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->CCBUF[index].reg ^= PDEC_CCBUF_CCBUF(mask);
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ccbuf_reg_t hri_pdec_read_CCBUF_CCBUF_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CCBUF[index].reg;
	tmp = (tmp & PDEC_CCBUF_CCBUF_Msk) >> PDEC_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_pdec_set_CCBUF_reg(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CCBUF[index].reg |= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ccbuf_reg_t hri_pdec_get_CCBUF_reg(const void *const hw, uint8_t index,
                                                          hri_pdec_ccbuf_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->CCBUF[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_write_CCBUF_reg(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t data)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CCBUF[index].reg = data;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_clear_CCBUF_reg(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CCBUF[index].reg &= ~mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pdec_toggle_CCBUF_reg(const void *const hw, uint8_t index, hri_pdec_ccbuf_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->CCBUF[index].reg ^= mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_ccbuf_reg_t hri_pdec_read_CCBUF_reg(const void *const hw, uint8_t index)
{
	return ((Pdec *)hw)->CCBUF[index].reg;
}

static inline bool hri_pdec_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_SWRST) >> PDEC_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_ENABLE) >> PDEC_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_CTRLB_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_CTRLB) >> PDEC_SYNCBUSY_CTRLB_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_STATUS_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_STATUS) >> PDEC_SYNCBUSY_STATUS_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_PRESC_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_PRESC) >> PDEC_SYNCBUSY_PRESC_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_FILTER_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_FILTER) >> PDEC_SYNCBUSY_FILTER_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_COUNT_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_COUNT) >> PDEC_SYNCBUSY_COUNT_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_CC0_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_CC0) >> PDEC_SYNCBUSY_CC0_Pos;
}

static inline bool hri_pdec_get_SYNCBUSY_CC1_bit(const void *const hw)
{
	return (((Pdec *)hw)->SYNCBUSY.reg & PDEC_SYNCBUSY_CC1) >> PDEC_SYNCBUSY_CC1_Pos;
}

static inline hri_pdec_syncbusy_reg_t hri_pdec_get_SYNCBUSY_reg(const void *const hw, hri_pdec_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pdec *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pdec_syncbusy_reg_t hri_pdec_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Pdec *)hw)->SYNCBUSY.reg;
}

static inline bool hri_pdec_get_STATUS_QERR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_QERR) >> PDEC_STATUS_QERR_Pos;
}

static inline void hri_pdec_clear_STATUS_QERR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_QERR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_IDXERR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_IDXERR) >> PDEC_STATUS_IDXERR_Pos;
}

static inline void hri_pdec_clear_STATUS_IDXERR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_IDXERR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_MPERR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_MPERR) >> PDEC_STATUS_MPERR_Pos;
}

static inline void hri_pdec_clear_STATUS_MPERR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_MPERR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_WINERR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_WINERR) >> PDEC_STATUS_WINERR_Pos;
}

static inline void hri_pdec_clear_STATUS_WINERR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_WINERR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_HERR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_HERR) >> PDEC_STATUS_HERR_Pos;
}

static inline void hri_pdec_clear_STATUS_HERR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_HERR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_STOP_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_STOP) >> PDEC_STATUS_STOP_Pos;
}

static inline void hri_pdec_clear_STATUS_STOP_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_STOP;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_DIR_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_DIR) >> PDEC_STATUS_DIR_Pos;
}

static inline void hri_pdec_clear_STATUS_DIR_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_DIR;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_PRESCBUFV_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_PRESCBUFV) >> PDEC_STATUS_PRESCBUFV_Pos;
}

static inline void hri_pdec_clear_STATUS_PRESCBUFV_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_PRESCBUFV;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_FILTERBUFV_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_FILTERBUFV) >> PDEC_STATUS_FILTERBUFV_Pos;
}

static inline void hri_pdec_clear_STATUS_FILTERBUFV_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_FILTERBUFV;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_CCBUFV0_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_CCBUFV0) >> PDEC_STATUS_CCBUFV0_Pos;
}

static inline void hri_pdec_clear_STATUS_CCBUFV0_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_CCBUFV0;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pdec_get_STATUS_CCBUFV1_bit(const void *const hw)
{
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	return (((Pdec *)hw)->STATUS.reg & PDEC_STATUS_CCBUFV1) >> PDEC_STATUS_CCBUFV1_Pos;
}

static inline void hri_pdec_clear_STATUS_CCBUFV1_bit(const void *const hw)
{
	PDEC_CRITICAL_SECTION_ENTER();
	hri_pdec_wait_for_sync(hw, PDEC_SYNCBUSY_MASK);
	((Pdec *)hw)->STATUS.reg = PDEC_STATUS_CCBUFV1;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_status_reg_t hri_pdec_get_STATUS_reg(const void *const hw, hri_pdec_status_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pdec *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pdec_clear_STATUS_reg(const void *const hw, hri_pdec_status_reg_t mask)
{
	PDEC_CRITICAL_SECTION_ENTER();
	((Pdec *)hw)->STATUS.reg = mask;
	PDEC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pdec_status_reg_t hri_pdec_read_STATUS_reg(const void *const hw)
{
	return ((Pdec *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PDEC_D51_H_INCLUDED */
#endif /* _SAMD51_PDEC_COMPONENT_ */
