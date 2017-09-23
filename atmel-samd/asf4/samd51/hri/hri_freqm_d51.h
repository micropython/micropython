/**
 * \file
 *
 * \brief SAM FREQM
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

#ifdef _SAMD51_FREQM_COMPONENT_
#ifndef _HRI_FREQM_D51_H_INCLUDED_
#define _HRI_FREQM_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_FREQM_CRITICAL_SECTIONS)
#define FREQM_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define FREQM_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define FREQM_CRITICAL_SECTION_ENTER()
#define FREQM_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_freqm_cfga_reg_t;
typedef uint32_t hri_freqm_syncbusy_reg_t;
typedef uint32_t hri_freqm_value_reg_t;
typedef uint8_t  hri_freqm_ctrla_reg_t;
typedef uint8_t  hri_freqm_ctrlb_reg_t;
typedef uint8_t  hri_freqm_intenset_reg_t;
typedef uint8_t  hri_freqm_intflag_reg_t;
typedef uint8_t  hri_freqm_status_reg_t;

static inline void hri_freqm_wait_for_sync(const void *const hw, hri_freqm_syncbusy_reg_t reg)
{
	while (((Freqm *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_freqm_is_syncing(const void *const hw, hri_freqm_syncbusy_reg_t reg)
{
	return ((Freqm *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_freqm_set_INTEN_DONE_bit(const void *const hw)
{
	((Freqm *)hw)->INTENSET.reg = FREQM_INTENSET_DONE;
}

static inline bool hri_freqm_get_INTEN_DONE_bit(const void *const hw)
{
	return (((Freqm *)hw)->INTENSET.reg & FREQM_INTENSET_DONE) >> FREQM_INTENSET_DONE_Pos;
}

static inline void hri_freqm_write_INTEN_DONE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Freqm *)hw)->INTENCLR.reg = FREQM_INTENSET_DONE;
	} else {
		((Freqm *)hw)->INTENSET.reg = FREQM_INTENSET_DONE;
	}
}

static inline void hri_freqm_clear_INTEN_DONE_bit(const void *const hw)
{
	((Freqm *)hw)->INTENCLR.reg = FREQM_INTENSET_DONE;
}

static inline void hri_freqm_set_INTEN_reg(const void *const hw, hri_freqm_intenset_reg_t mask)
{
	((Freqm *)hw)->INTENSET.reg = mask;
}

static inline hri_freqm_intenset_reg_t hri_freqm_get_INTEN_reg(const void *const hw, hri_freqm_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Freqm *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_freqm_intenset_reg_t hri_freqm_read_INTEN_reg(const void *const hw)
{
	return ((Freqm *)hw)->INTENSET.reg;
}

static inline void hri_freqm_write_INTEN_reg(const void *const hw, hri_freqm_intenset_reg_t data)
{
	((Freqm *)hw)->INTENSET.reg = data;
	((Freqm *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_freqm_clear_INTEN_reg(const void *const hw, hri_freqm_intenset_reg_t mask)
{
	((Freqm *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_freqm_get_INTFLAG_DONE_bit(const void *const hw)
{
	return (((Freqm *)hw)->INTFLAG.reg & FREQM_INTFLAG_DONE) >> FREQM_INTFLAG_DONE_Pos;
}

static inline void hri_freqm_clear_INTFLAG_DONE_bit(const void *const hw)
{
	((Freqm *)hw)->INTFLAG.reg = FREQM_INTFLAG_DONE;
}

static inline bool hri_freqm_get_interrupt_DONE_bit(const void *const hw)
{
	return (((Freqm *)hw)->INTFLAG.reg & FREQM_INTFLAG_DONE) >> FREQM_INTFLAG_DONE_Pos;
}

static inline void hri_freqm_clear_interrupt_DONE_bit(const void *const hw)
{
	((Freqm *)hw)->INTFLAG.reg = FREQM_INTFLAG_DONE;
}

static inline hri_freqm_intflag_reg_t hri_freqm_get_INTFLAG_reg(const void *const hw, hri_freqm_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Freqm *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_freqm_intflag_reg_t hri_freqm_read_INTFLAG_reg(const void *const hw)
{
	return ((Freqm *)hw)->INTFLAG.reg;
}

static inline void hri_freqm_clear_INTFLAG_reg(const void *const hw, hri_freqm_intflag_reg_t mask)
{
	((Freqm *)hw)->INTFLAG.reg = mask;
}

static inline void hri_freqm_write_CTRLB_reg(const void *const hw, hri_freqm_ctrlb_reg_t data)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CTRLB.reg = data;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_set_CTRLA_SWRST_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST);
	((Freqm *)hw)->CTRLA.reg |= FREQM_CTRLA_SWRST;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_freqm_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST);
	tmp = ((Freqm *)hw)->CTRLA.reg;
	tmp = (tmp & FREQM_CTRLA_SWRST) >> FREQM_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_freqm_set_CTRLA_ENABLE_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST | FREQM_SYNCBUSY_ENABLE);
	((Freqm *)hw)->CTRLA.reg |= FREQM_CTRLA_ENABLE;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_freqm_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST | FREQM_SYNCBUSY_ENABLE);
	tmp = ((Freqm *)hw)->CTRLA.reg;
	tmp = (tmp & FREQM_CTRLA_ENABLE) >> FREQM_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_freqm_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	FREQM_CRITICAL_SECTION_ENTER();
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST | FREQM_SYNCBUSY_ENABLE);
	tmp = ((Freqm *)hw)->CTRLA.reg;
	tmp &= ~FREQM_CTRLA_ENABLE;
	tmp |= value << FREQM_CTRLA_ENABLE_Pos;
	((Freqm *)hw)->CTRLA.reg = tmp;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST | FREQM_SYNCBUSY_ENABLE);
	((Freqm *)hw)->CTRLA.reg &= ~FREQM_CTRLA_ENABLE;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	hri_freqm_wait_for_sync(hw, FREQM_SYNCBUSY_SWRST | FREQM_SYNCBUSY_ENABLE);
	((Freqm *)hw)->CTRLA.reg ^= FREQM_CTRLA_ENABLE;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_set_CTRLA_reg(const void *const hw, hri_freqm_ctrla_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CTRLA.reg |= mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_ctrla_reg_t hri_freqm_get_CTRLA_reg(const void *const hw, hri_freqm_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Freqm *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_freqm_write_CTRLA_reg(const void *const hw, hri_freqm_ctrla_reg_t data)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CTRLA.reg = data;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_clear_CTRLA_reg(const void *const hw, hri_freqm_ctrla_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CTRLA.reg &= ~mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_toggle_CTRLA_reg(const void *const hw, hri_freqm_ctrla_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CTRLA.reg ^= mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_ctrla_reg_t hri_freqm_read_CTRLA_reg(const void *const hw)
{
	return ((Freqm *)hw)->CTRLA.reg;
}

static inline void hri_freqm_set_CFGA_REFNUM_bf(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg |= FREQM_CFGA_REFNUM(mask);
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_cfga_reg_t hri_freqm_get_CFGA_REFNUM_bf(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Freqm *)hw)->CFGA.reg;
	tmp = (tmp & FREQM_CFGA_REFNUM(mask)) >> FREQM_CFGA_REFNUM_Pos;
	return tmp;
}

static inline void hri_freqm_write_CFGA_REFNUM_bf(const void *const hw, hri_freqm_cfga_reg_t data)
{
	uint16_t tmp;
	FREQM_CRITICAL_SECTION_ENTER();
	tmp = ((Freqm *)hw)->CFGA.reg;
	tmp &= ~FREQM_CFGA_REFNUM_Msk;
	tmp |= FREQM_CFGA_REFNUM(data);
	((Freqm *)hw)->CFGA.reg = tmp;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_clear_CFGA_REFNUM_bf(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg &= ~FREQM_CFGA_REFNUM(mask);
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_toggle_CFGA_REFNUM_bf(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg ^= FREQM_CFGA_REFNUM(mask);
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_cfga_reg_t hri_freqm_read_CFGA_REFNUM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Freqm *)hw)->CFGA.reg;
	tmp = (tmp & FREQM_CFGA_REFNUM_Msk) >> FREQM_CFGA_REFNUM_Pos;
	return tmp;
}

static inline void hri_freqm_set_CFGA_reg(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg |= mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_cfga_reg_t hri_freqm_get_CFGA_reg(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Freqm *)hw)->CFGA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_freqm_write_CFGA_reg(const void *const hw, hri_freqm_cfga_reg_t data)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg = data;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_clear_CFGA_reg(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg &= ~mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_freqm_toggle_CFGA_reg(const void *const hw, hri_freqm_cfga_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->CFGA.reg ^= mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_cfga_reg_t hri_freqm_read_CFGA_reg(const void *const hw)
{
	return ((Freqm *)hw)->CFGA.reg;
}

static inline bool hri_freqm_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Freqm *)hw)->SYNCBUSY.reg & FREQM_SYNCBUSY_SWRST) >> FREQM_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_freqm_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Freqm *)hw)->SYNCBUSY.reg & FREQM_SYNCBUSY_ENABLE) >> FREQM_SYNCBUSY_ENABLE_Pos;
}

static inline hri_freqm_syncbusy_reg_t hri_freqm_get_SYNCBUSY_reg(const void *const hw, hri_freqm_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Freqm *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_freqm_syncbusy_reg_t hri_freqm_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Freqm *)hw)->SYNCBUSY.reg;
}

static inline hri_freqm_value_reg_t hri_freqm_get_VALUE_VALUE_bf(const void *const hw, hri_freqm_value_reg_t mask)
{
	return (((Freqm *)hw)->VALUE.reg & FREQM_VALUE_VALUE(mask)) >> FREQM_VALUE_VALUE_Pos;
}

static inline hri_freqm_value_reg_t hri_freqm_read_VALUE_VALUE_bf(const void *const hw)
{
	return (((Freqm *)hw)->VALUE.reg & FREQM_VALUE_VALUE_Msk) >> FREQM_VALUE_VALUE_Pos;
}

static inline hri_freqm_value_reg_t hri_freqm_get_VALUE_reg(const void *const hw, hri_freqm_value_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Freqm *)hw)->VALUE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_freqm_value_reg_t hri_freqm_read_VALUE_reg(const void *const hw)
{
	return ((Freqm *)hw)->VALUE.reg;
}

static inline bool hri_freqm_get_STATUS_BUSY_bit(const void *const hw)
{
	return (((Freqm *)hw)->STATUS.reg & FREQM_STATUS_BUSY) >> FREQM_STATUS_BUSY_Pos;
}

static inline void hri_freqm_clear_STATUS_BUSY_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->STATUS.reg = FREQM_STATUS_BUSY;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_freqm_get_STATUS_OVF_bit(const void *const hw)
{
	return (((Freqm *)hw)->STATUS.reg & FREQM_STATUS_OVF) >> FREQM_STATUS_OVF_Pos;
}

static inline void hri_freqm_clear_STATUS_OVF_bit(const void *const hw)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->STATUS.reg = FREQM_STATUS_OVF;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_status_reg_t hri_freqm_get_STATUS_reg(const void *const hw, hri_freqm_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Freqm *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_freqm_clear_STATUS_reg(const void *const hw, hri_freqm_status_reg_t mask)
{
	FREQM_CRITICAL_SECTION_ENTER();
	((Freqm *)hw)->STATUS.reg = mask;
	FREQM_CRITICAL_SECTION_LEAVE();
}

static inline hri_freqm_status_reg_t hri_freqm_read_STATUS_reg(const void *const hw)
{
	return ((Freqm *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_FREQM_D51_H_INCLUDED */
#endif /* _SAMD51_FREQM_COMPONENT_ */
