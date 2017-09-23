/**
 * \file
 *
 * \brief SAM AC
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

#ifdef _SAMD51_AC_COMPONENT_
#ifndef _HRI_AC_D51_H_INCLUDED_
#define _HRI_AC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_AC_CRITICAL_SECTIONS)
#define AC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define AC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define AC_CRITICAL_SECTION_ENTER()
#define AC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_ac_calib_reg_t;
typedef uint16_t hri_ac_evctrl_reg_t;
typedef uint32_t hri_ac_compctrl_reg_t;
typedef uint32_t hri_ac_syncbusy_reg_t;
typedef uint8_t  hri_ac_ctrla_reg_t;
typedef uint8_t  hri_ac_ctrlb_reg_t;
typedef uint8_t  hri_ac_dbgctrl_reg_t;
typedef uint8_t  hri_ac_intenset_reg_t;
typedef uint8_t  hri_ac_intflag_reg_t;
typedef uint8_t  hri_ac_scaler_reg_t;
typedef uint8_t  hri_ac_statusa_reg_t;
typedef uint8_t  hri_ac_statusb_reg_t;
typedef uint8_t  hri_ac_winctrl_reg_t;

static inline void hri_ac_wait_for_sync(const void *const hw, hri_ac_syncbusy_reg_t reg)
{
	while (((Ac *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_ac_is_syncing(const void *const hw, hri_ac_syncbusy_reg_t reg)
{
	return ((Ac *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_ac_set_INTEN_COMP0_bit(const void *const hw)
{
	((Ac *)hw)->INTENSET.reg = AC_INTENSET_COMP0;
}

static inline bool hri_ac_get_INTEN_COMP0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTENSET.reg & AC_INTENSET_COMP0) >> AC_INTENSET_COMP0_Pos;
}

static inline void hri_ac_write_INTEN_COMP0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Ac *)hw)->INTENCLR.reg = AC_INTENSET_COMP0;
	} else {
		((Ac *)hw)->INTENSET.reg = AC_INTENSET_COMP0;
	}
}

static inline void hri_ac_clear_INTEN_COMP0_bit(const void *const hw)
{
	((Ac *)hw)->INTENCLR.reg = AC_INTENSET_COMP0;
}

static inline void hri_ac_set_INTEN_COMP1_bit(const void *const hw)
{
	((Ac *)hw)->INTENSET.reg = AC_INTENSET_COMP1;
}

static inline bool hri_ac_get_INTEN_COMP1_bit(const void *const hw)
{
	return (((Ac *)hw)->INTENSET.reg & AC_INTENSET_COMP1) >> AC_INTENSET_COMP1_Pos;
}

static inline void hri_ac_write_INTEN_COMP1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Ac *)hw)->INTENCLR.reg = AC_INTENSET_COMP1;
	} else {
		((Ac *)hw)->INTENSET.reg = AC_INTENSET_COMP1;
	}
}

static inline void hri_ac_clear_INTEN_COMP1_bit(const void *const hw)
{
	((Ac *)hw)->INTENCLR.reg = AC_INTENSET_COMP1;
}

static inline void hri_ac_set_INTEN_WIN0_bit(const void *const hw)
{
	((Ac *)hw)->INTENSET.reg = AC_INTENSET_WIN0;
}

static inline bool hri_ac_get_INTEN_WIN0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTENSET.reg & AC_INTENSET_WIN0) >> AC_INTENSET_WIN0_Pos;
}

static inline void hri_ac_write_INTEN_WIN0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Ac *)hw)->INTENCLR.reg = AC_INTENSET_WIN0;
	} else {
		((Ac *)hw)->INTENSET.reg = AC_INTENSET_WIN0;
	}
}

static inline void hri_ac_clear_INTEN_WIN0_bit(const void *const hw)
{
	((Ac *)hw)->INTENCLR.reg = AC_INTENSET_WIN0;
}

static inline void hri_ac_set_INTEN_reg(const void *const hw, hri_ac_intenset_reg_t mask)
{
	((Ac *)hw)->INTENSET.reg = mask;
}

static inline hri_ac_intenset_reg_t hri_ac_get_INTEN_reg(const void *const hw, hri_ac_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ac_intenset_reg_t hri_ac_read_INTEN_reg(const void *const hw)
{
	return ((Ac *)hw)->INTENSET.reg;
}

static inline void hri_ac_write_INTEN_reg(const void *const hw, hri_ac_intenset_reg_t data)
{
	((Ac *)hw)->INTENSET.reg = data;
	((Ac *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_ac_clear_INTEN_reg(const void *const hw, hri_ac_intenset_reg_t mask)
{
	((Ac *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_ac_get_INTFLAG_COMP0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_COMP0) >> AC_INTFLAG_COMP0_Pos;
}

static inline void hri_ac_clear_INTFLAG_COMP0_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_COMP0;
}

static inline bool hri_ac_get_INTFLAG_COMP1_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_COMP1) >> AC_INTFLAG_COMP1_Pos;
}

static inline void hri_ac_clear_INTFLAG_COMP1_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_COMP1;
}

static inline bool hri_ac_get_INTFLAG_WIN0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_WIN0) >> AC_INTFLAG_WIN0_Pos;
}

static inline void hri_ac_clear_INTFLAG_WIN0_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_WIN0;
}

static inline bool hri_ac_get_interrupt_COMP0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_COMP0) >> AC_INTFLAG_COMP0_Pos;
}

static inline void hri_ac_clear_interrupt_COMP0_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_COMP0;
}

static inline bool hri_ac_get_interrupt_COMP1_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_COMP1) >> AC_INTFLAG_COMP1_Pos;
}

static inline void hri_ac_clear_interrupt_COMP1_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_COMP1;
}

static inline bool hri_ac_get_interrupt_WIN0_bit(const void *const hw)
{
	return (((Ac *)hw)->INTFLAG.reg & AC_INTFLAG_WIN0) >> AC_INTFLAG_WIN0_Pos;
}

static inline void hri_ac_clear_interrupt_WIN0_bit(const void *const hw)
{
	((Ac *)hw)->INTFLAG.reg = AC_INTFLAG_WIN0;
}

static inline hri_ac_intflag_reg_t hri_ac_get_INTFLAG_reg(const void *const hw, hri_ac_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ac_intflag_reg_t hri_ac_read_INTFLAG_reg(const void *const hw)
{
	return ((Ac *)hw)->INTFLAG.reg;
}

static inline void hri_ac_clear_INTFLAG_reg(const void *const hw, hri_ac_intflag_reg_t mask)
{
	((Ac *)hw)->INTFLAG.reg = mask;
}

static inline void hri_ac_write_CTRLB_reg(const void *const hw, hri_ac_ctrlb_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CTRLB.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_CTRLA_SWRST_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST);
	((Ac *)hw)->CTRLA.reg |= AC_CTRLA_SWRST;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST);
	tmp = ((Ac *)hw)->CTRLA.reg;
	tmp = (tmp & AC_CTRLA_SWRST) >> AC_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_ac_set_CTRLA_ENABLE_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST | AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->CTRLA.reg |= AC_CTRLA_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST | AC_SYNCBUSY_ENABLE);
	tmp = ((Ac *)hw)->CTRLA.reg;
	tmp = (tmp & AC_CTRLA_ENABLE) >> AC_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST | AC_SYNCBUSY_ENABLE);
	tmp = ((Ac *)hw)->CTRLA.reg;
	tmp &= ~AC_CTRLA_ENABLE;
	tmp |= value << AC_CTRLA_ENABLE_Pos;
	((Ac *)hw)->CTRLA.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST | AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->CTRLA.reg &= ~AC_CTRLA_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_SWRST | AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->CTRLA.reg ^= AC_CTRLA_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_CTRLA_reg(const void *const hw, hri_ac_ctrla_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CTRLA.reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_ctrla_reg_t hri_ac_get_CTRLA_reg(const void *const hw, hri_ac_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_CTRLA_reg(const void *const hw, hri_ac_ctrla_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CTRLA.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_CTRLA_reg(const void *const hw, hri_ac_ctrla_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CTRLA.reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_CTRLA_reg(const void *const hw, hri_ac_ctrla_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CTRLA.reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_ctrla_reg_t hri_ac_read_CTRLA_reg(const void *const hw)
{
	return ((Ac *)hw)->CTRLA.reg;
}

static inline void hri_ac_set_EVCTRL_COMPEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_COMPEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_COMPEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_COMPEO0) >> AC_EVCTRL_COMPEO0_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_COMPEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_COMPEO0;
	tmp |= value << AC_EVCTRL_COMPEO0_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_COMPEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_COMPEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_COMPEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_COMPEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_COMPEO1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_COMPEO1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_COMPEO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_COMPEO1) >> AC_EVCTRL_COMPEO1_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_COMPEO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_COMPEO1;
	tmp |= value << AC_EVCTRL_COMPEO1_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_COMPEO1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_COMPEO1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_COMPEO1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_COMPEO1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_WINEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_WINEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_WINEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_WINEO0) >> AC_EVCTRL_WINEO0_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_WINEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_WINEO0;
	tmp |= value << AC_EVCTRL_WINEO0_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_WINEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_WINEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_WINEO0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_WINEO0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_COMPEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_COMPEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_COMPEI0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_COMPEI0) >> AC_EVCTRL_COMPEI0_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_COMPEI0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_COMPEI0;
	tmp |= value << AC_EVCTRL_COMPEI0_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_COMPEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_COMPEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_COMPEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_COMPEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_COMPEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_COMPEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_COMPEI1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_COMPEI1) >> AC_EVCTRL_COMPEI1_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_COMPEI1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_COMPEI1;
	tmp |= value << AC_EVCTRL_COMPEI1_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_COMPEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_COMPEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_COMPEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_COMPEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_INVEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_INVEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_INVEI0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_INVEI0) >> AC_EVCTRL_INVEI0_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_INVEI0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_INVEI0;
	tmp |= value << AC_EVCTRL_INVEI0_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_INVEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_INVEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_INVEI0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_INVEI0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_INVEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= AC_EVCTRL_INVEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_EVCTRL_INVEI1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp = (tmp & AC_EVCTRL_INVEI1) >> AC_EVCTRL_INVEI1_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_EVCTRL_INVEI1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= ~AC_EVCTRL_INVEI1;
	tmp |= value << AC_EVCTRL_INVEI1_Pos;
	((Ac *)hw)->EVCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_INVEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~AC_EVCTRL_INVEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_INVEI1_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= AC_EVCTRL_INVEI1;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_EVCTRL_reg(const void *const hw, hri_ac_evctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_evctrl_reg_t hri_ac_get_EVCTRL_reg(const void *const hw, hri_ac_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_EVCTRL_reg(const void *const hw, hri_ac_evctrl_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_EVCTRL_reg(const void *const hw, hri_ac_evctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_EVCTRL_reg(const void *const hw, hri_ac_evctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->EVCTRL.reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_evctrl_reg_t hri_ac_read_EVCTRL_reg(const void *const hw)
{
	return ((Ac *)hw)->EVCTRL.reg;
}

static inline void hri_ac_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg |= AC_DBGCTRL_DBGRUN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->DBGCTRL.reg;
	tmp = (tmp & AC_DBGCTRL_DBGRUN) >> AC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->DBGCTRL.reg;
	tmp &= ~AC_DBGCTRL_DBGRUN;
	tmp |= value << AC_DBGCTRL_DBGRUN_Pos;
	((Ac *)hw)->DBGCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg &= ~AC_DBGCTRL_DBGRUN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg ^= AC_DBGCTRL_DBGRUN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_DBGCTRL_reg(const void *const hw, hri_ac_dbgctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_dbgctrl_reg_t hri_ac_get_DBGCTRL_reg(const void *const hw, hri_ac_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_DBGCTRL_reg(const void *const hw, hri_ac_dbgctrl_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_DBGCTRL_reg(const void *const hw, hri_ac_dbgctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_DBGCTRL_reg(const void *const hw, hri_ac_dbgctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->DBGCTRL.reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_dbgctrl_reg_t hri_ac_read_DBGCTRL_reg(const void *const hw)
{
	return ((Ac *)hw)->DBGCTRL.reg;
}

static inline void hri_ac_set_WINCTRL_WEN0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg |= AC_WINCTRL_WEN0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_WINCTRL_WEN0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp = (tmp & AC_WINCTRL_WEN0) >> AC_WINCTRL_WEN0_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_WINCTRL_WEN0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp &= ~AC_WINCTRL_WEN0;
	tmp |= value << AC_WINCTRL_WEN0_Pos;
	((Ac *)hw)->WINCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_WINCTRL_WEN0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg &= ~AC_WINCTRL_WEN0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_WINCTRL_WEN0_bit(const void *const hw)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg ^= AC_WINCTRL_WEN0;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_WINCTRL_WINTSEL0_bf(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg |= AC_WINCTRL_WINTSEL0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_winctrl_reg_t hri_ac_get_WINCTRL_WINTSEL0_bf(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp = (tmp & AC_WINCTRL_WINTSEL0(mask)) >> AC_WINCTRL_WINTSEL0_Pos;
	return tmp;
}

static inline void hri_ac_write_WINCTRL_WINTSEL0_bf(const void *const hw, hri_ac_winctrl_reg_t data)
{
	uint8_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp &= ~AC_WINCTRL_WINTSEL0_Msk;
	tmp |= AC_WINCTRL_WINTSEL0(data);
	((Ac *)hw)->WINCTRL.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_WINCTRL_WINTSEL0_bf(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg &= ~AC_WINCTRL_WINTSEL0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_WINCTRL_WINTSEL0_bf(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->WINCTRL.reg ^= AC_WINCTRL_WINTSEL0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_winctrl_reg_t hri_ac_read_WINCTRL_WINTSEL0_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp = (tmp & AC_WINCTRL_WINTSEL0_Msk) >> AC_WINCTRL_WINTSEL0_Pos;
	return tmp;
}

static inline void hri_ac_set_WINCTRL_reg(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->WINCTRL.reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_winctrl_reg_t hri_ac_get_WINCTRL_reg(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->WINCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_WINCTRL_reg(const void *const hw, hri_ac_winctrl_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->WINCTRL.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_WINCTRL_reg(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->WINCTRL.reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_WINCTRL_reg(const void *const hw, hri_ac_winctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->WINCTRL.reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_winctrl_reg_t hri_ac_read_WINCTRL_reg(const void *const hw)
{
	return ((Ac *)hw)->WINCTRL.reg;
}

static inline void hri_ac_set_SCALER_VALUE_bf(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg |= AC_SCALER_VALUE(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_scaler_reg_t hri_ac_get_SCALER_VALUE_bf(const void *const hw, uint8_t index,
                                                             hri_ac_scaler_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->SCALER[index].reg;
	tmp = (tmp & AC_SCALER_VALUE(mask)) >> AC_SCALER_VALUE_Pos;
	return tmp;
}

static inline void hri_ac_write_SCALER_VALUE_bf(const void *const hw, uint8_t index, hri_ac_scaler_reg_t data)
{
	uint8_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->SCALER[index].reg;
	tmp &= ~AC_SCALER_VALUE_Msk;
	tmp |= AC_SCALER_VALUE(data);
	((Ac *)hw)->SCALER[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_SCALER_VALUE_bf(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg &= ~AC_SCALER_VALUE(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_SCALER_VALUE_bf(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg ^= AC_SCALER_VALUE(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_scaler_reg_t hri_ac_read_SCALER_VALUE_bf(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->SCALER[index].reg;
	tmp = (tmp & AC_SCALER_VALUE_Msk) >> AC_SCALER_VALUE_Pos;
	return tmp;
}

static inline void hri_ac_set_SCALER_reg(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_scaler_reg_t hri_ac_get_SCALER_reg(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->SCALER[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_SCALER_reg(const void *const hw, uint8_t index, hri_ac_scaler_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_SCALER_reg(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_SCALER_reg(const void *const hw, uint8_t index, hri_ac_scaler_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->SCALER[index].reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_scaler_reg_t hri_ac_read_SCALER_reg(const void *const hw, uint8_t index)
{
	return ((Ac *)hw)->SCALER[index].reg;
}

static inline void hri_ac_set_COMPCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_COMPCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_ENABLE);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_ENABLE) >> AC_COMPCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_COMPCTRL_ENABLE_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_ENABLE);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_ENABLE;
	tmp |= value << AC_COMPCTRL_ENABLE_Pos;
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_ENABLE);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_ENABLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_COMPCTRL_SINGLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_SINGLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_COMPCTRL_SINGLE_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_SINGLE) >> AC_COMPCTRL_SINGLE_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_COMPCTRL_SINGLE_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_SINGLE;
	tmp |= value << AC_COMPCTRL_SINGLE_Pos;
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_SINGLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_SINGLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_SINGLE_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_SINGLE;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_COMPCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_RUNSTDBY;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_COMPCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_RUNSTDBY) >> AC_COMPCTRL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_COMPCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_RUNSTDBY;
	tmp |= value << AC_COMPCTRL_RUNSTDBY_Pos;
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_RUNSTDBY;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_RUNSTDBY;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_COMPCTRL_SWAP_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_SWAP;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_COMPCTRL_SWAP_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_SWAP) >> AC_COMPCTRL_SWAP_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_COMPCTRL_SWAP_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_SWAP;
	tmp |= value << AC_COMPCTRL_SWAP_Pos;
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_SWAP_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_SWAP;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_SWAP_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_SWAP;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_COMPCTRL_HYSTEN_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_HYSTEN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ac_get_COMPCTRL_HYSTEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_HYSTEN) >> AC_COMPCTRL_HYSTEN_Pos;
	return (bool)tmp;
}

static inline void hri_ac_write_COMPCTRL_HYSTEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_HYSTEN;
	tmp |= value << AC_COMPCTRL_HYSTEN_Pos;
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_HYSTEN_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_HYSTEN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_HYSTEN_bit(const void *const hw, uint8_t index)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_HYSTEN;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_set_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_INTSEL(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index,
                                                                  hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_INTSEL(mask)) >> AC_COMPCTRL_INTSEL_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_INTSEL_Msk;
	tmp |= AC_COMPCTRL_INTSEL(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_INTSEL(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_INTSEL(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_INTSEL_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_INTSEL_Msk) >> AC_COMPCTRL_INTSEL_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_MUXNEG(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index,
                                                                  hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_MUXNEG(mask)) >> AC_COMPCTRL_MUXNEG_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_MUXNEG_Msk;
	tmp |= AC_COMPCTRL_MUXNEG(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_MUXNEG(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_MUXNEG(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_MUXNEG_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_MUXNEG_Msk) >> AC_COMPCTRL_MUXNEG_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_MUXPOS(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index,
                                                                  hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_MUXPOS(mask)) >> AC_COMPCTRL_MUXPOS_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_MUXPOS_Msk;
	tmp |= AC_COMPCTRL_MUXPOS(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_MUXPOS(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_MUXPOS(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_MUXPOS_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_MUXPOS_Msk) >> AC_COMPCTRL_MUXPOS_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_SPEED(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index,
                                                                 hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_SPEED(mask)) >> AC_COMPCTRL_SPEED_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_SPEED_Msk;
	tmp |= AC_COMPCTRL_SPEED(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_SPEED(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_SPEED(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_SPEED_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_SPEED_Msk) >> AC_COMPCTRL_SPEED_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_HYST_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_HYST(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_HYST_bf(const void *const hw, uint8_t index,
                                                                hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_HYST(mask)) >> AC_COMPCTRL_HYST_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_HYST_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_HYST_Msk;
	tmp |= AC_COMPCTRL_HYST(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_HYST_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_HYST(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_HYST_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_HYST(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_HYST_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_HYST_Msk) >> AC_COMPCTRL_HYST_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_FLEN(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index,
                                                                hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_FLEN(mask)) >> AC_COMPCTRL_FLEN_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_FLEN_Msk;
	tmp |= AC_COMPCTRL_FLEN(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_FLEN(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_FLEN(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_FLEN_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_FLEN_Msk) >> AC_COMPCTRL_FLEN_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_OUT_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg |= AC_COMPCTRL_OUT(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_OUT_bf(const void *const hw, uint8_t index,
                                                               hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_OUT(mask)) >> AC_COMPCTRL_OUT_Pos;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_OUT_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	uint32_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= ~AC_COMPCTRL_OUT_Msk;
	tmp |= AC_COMPCTRL_OUT(data);
	((Ac *)hw)->COMPCTRL[index].reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_OUT_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg &= ~AC_COMPCTRL_OUT(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_OUT_bf(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	hri_ac_wait_for_sync(hw, AC_SYNCBUSY_MASK);
	((Ac *)hw)->COMPCTRL[index].reg ^= AC_COMPCTRL_OUT(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_OUT_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp = (tmp & AC_COMPCTRL_OUT_Msk) >> AC_COMPCTRL_OUT_Pos;
	return tmp;
}

static inline void hri_ac_set_COMPCTRL_reg(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->COMPCTRL[index].reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_get_COMPCTRL_reg(const void *const hw, uint8_t index,
                                                            hri_ac_compctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->COMPCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_COMPCTRL_reg(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->COMPCTRL[index].reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_COMPCTRL_reg(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->COMPCTRL[index].reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_COMPCTRL_reg(const void *const hw, uint8_t index, hri_ac_compctrl_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->COMPCTRL[index].reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_compctrl_reg_t hri_ac_read_COMPCTRL_reg(const void *const hw, uint8_t index)
{
	return ((Ac *)hw)->COMPCTRL[index].reg;
}

static inline void hri_ac_set_CALIB_BIAS0_bf(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg |= AC_CALIB_BIAS0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_calib_reg_t hri_ac_get_CALIB_BIAS0_bf(const void *const hw, hri_ac_calib_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->CALIB.reg;
	tmp = (tmp & AC_CALIB_BIAS0(mask)) >> AC_CALIB_BIAS0_Pos;
	return tmp;
}

static inline void hri_ac_write_CALIB_BIAS0_bf(const void *const hw, hri_ac_calib_reg_t data)
{
	uint16_t tmp;
	AC_CRITICAL_SECTION_ENTER();
	tmp = ((Ac *)hw)->CALIB.reg;
	tmp &= ~AC_CALIB_BIAS0_Msk;
	tmp |= AC_CALIB_BIAS0(data);
	((Ac *)hw)->CALIB.reg = tmp;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_CALIB_BIAS0_bf(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg &= ~AC_CALIB_BIAS0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_CALIB_BIAS0_bf(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg ^= AC_CALIB_BIAS0(mask);
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_calib_reg_t hri_ac_read_CALIB_BIAS0_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->CALIB.reg;
	tmp = (tmp & AC_CALIB_BIAS0_Msk) >> AC_CALIB_BIAS0_Pos;
	return tmp;
}

static inline void hri_ac_set_CALIB_reg(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg |= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_calib_reg_t hri_ac_get_CALIB_reg(const void *const hw, hri_ac_calib_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Ac *)hw)->CALIB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ac_write_CALIB_reg(const void *const hw, hri_ac_calib_reg_t data)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg = data;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_clear_CALIB_reg(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg &= ~mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ac_toggle_CALIB_reg(const void *const hw, hri_ac_calib_reg_t mask)
{
	AC_CRITICAL_SECTION_ENTER();
	((Ac *)hw)->CALIB.reg ^= mask;
	AC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ac_calib_reg_t hri_ac_read_CALIB_reg(const void *const hw)
{
	return ((Ac *)hw)->CALIB.reg;
}

static inline bool hri_ac_get_STATUSA_STATE0_bit(const void *const hw)
{
	return (((Ac *)hw)->STATUSA.reg & AC_STATUSA_STATE0) >> AC_STATUSA_STATE0_Pos;
}

static inline bool hri_ac_get_STATUSA_STATE1_bit(const void *const hw)
{
	return (((Ac *)hw)->STATUSA.reg & AC_STATUSA_STATE1) >> AC_STATUSA_STATE1_Pos;
}

static inline hri_ac_statusa_reg_t hri_ac_get_STATUSA_WSTATE0_bf(const void *const hw, hri_ac_statusa_reg_t mask)
{
	return (((Ac *)hw)->STATUSA.reg & AC_STATUSA_WSTATE0(mask)) >> AC_STATUSA_WSTATE0_Pos;
}

static inline hri_ac_statusa_reg_t hri_ac_read_STATUSA_WSTATE0_bf(const void *const hw)
{
	return (((Ac *)hw)->STATUSA.reg & AC_STATUSA_WSTATE0_Msk) >> AC_STATUSA_WSTATE0_Pos;
}

static inline hri_ac_statusa_reg_t hri_ac_get_STATUSA_reg(const void *const hw, hri_ac_statusa_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->STATUSA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ac_statusa_reg_t hri_ac_read_STATUSA_reg(const void *const hw)
{
	return ((Ac *)hw)->STATUSA.reg;
}

static inline bool hri_ac_get_STATUSB_READY0_bit(const void *const hw)
{
	return (((Ac *)hw)->STATUSB.reg & AC_STATUSB_READY0) >> AC_STATUSB_READY0_Pos;
}

static inline bool hri_ac_get_STATUSB_READY1_bit(const void *const hw)
{
	return (((Ac *)hw)->STATUSB.reg & AC_STATUSB_READY1) >> AC_STATUSB_READY1_Pos;
}

static inline hri_ac_statusb_reg_t hri_ac_get_STATUSB_reg(const void *const hw, hri_ac_statusb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ac *)hw)->STATUSB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ac_statusb_reg_t hri_ac_read_STATUSB_reg(const void *const hw)
{
	return ((Ac *)hw)->STATUSB.reg;
}

static inline bool hri_ac_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Ac *)hw)->SYNCBUSY.reg & AC_SYNCBUSY_SWRST) >> AC_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_ac_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Ac *)hw)->SYNCBUSY.reg & AC_SYNCBUSY_ENABLE) >> AC_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_ac_get_SYNCBUSY_WINCTRL_bit(const void *const hw)
{
	return (((Ac *)hw)->SYNCBUSY.reg & AC_SYNCBUSY_WINCTRL) >> AC_SYNCBUSY_WINCTRL_Pos;
}

static inline bool hri_ac_get_SYNCBUSY_COMPCTRL0_bit(const void *const hw)
{
	return (((Ac *)hw)->SYNCBUSY.reg & AC_SYNCBUSY_COMPCTRL0) >> AC_SYNCBUSY_COMPCTRL0_Pos;
}

static inline bool hri_ac_get_SYNCBUSY_COMPCTRL1_bit(const void *const hw)
{
	return (((Ac *)hw)->SYNCBUSY.reg & AC_SYNCBUSY_COMPCTRL1) >> AC_SYNCBUSY_COMPCTRL1_Pos;
}

static inline hri_ac_syncbusy_reg_t hri_ac_get_SYNCBUSY_reg(const void *const hw, hri_ac_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ac *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ac_syncbusy_reg_t hri_ac_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Ac *)hw)->SYNCBUSY.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_AC_D51_H_INCLUDED */
#endif /* _SAMD51_AC_COMPONENT_ */
