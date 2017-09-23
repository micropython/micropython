/**
 * \file
 *
 * \brief SAM EIC
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

#ifdef _SAMD51_EIC_COMPONENT_
#ifndef _HRI_EIC_D51_H_INCLUDED_
#define _HRI_EIC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_EIC_CRITICAL_SECTIONS)
#define EIC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define EIC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define EIC_CRITICAL_SECTION_ENTER()
#define EIC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_eic_nmiflag_reg_t;
typedef uint32_t hri_eic_asynch_reg_t;
typedef uint32_t hri_eic_config_reg_t;
typedef uint32_t hri_eic_debouncen_reg_t;
typedef uint32_t hri_eic_dprescaler_reg_t;
typedef uint32_t hri_eic_evctrl_reg_t;
typedef uint32_t hri_eic_intenset_reg_t;
typedef uint32_t hri_eic_intflag_reg_t;
typedef uint32_t hri_eic_pinstate_reg_t;
typedef uint32_t hri_eic_syncbusy_reg_t;
typedef uint8_t  hri_eic_ctrla_reg_t;
typedef uint8_t  hri_eic_nmictrl_reg_t;

static inline void hri_eic_wait_for_sync(const void *const hw, hri_eic_syncbusy_reg_t reg)
{
	while (((Eic *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_eic_is_syncing(const void *const hw, hri_eic_syncbusy_reg_t reg)
{
	return ((Eic *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_eic_set_INTEN_EXTINT_bf(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT(mask);
}

static inline hri_eic_intenset_reg_t hri_eic_get_INTEN_EXTINT_bf(const void *const hw, hri_eic_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTENSET.reg;
	tmp = (tmp & EIC_INTENSET_EXTINT(mask)) >> EIC_INTENSET_EXTINT_Pos;
	return tmp;
}

static inline hri_eic_intenset_reg_t hri_eic_read_INTEN_EXTINT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTENSET.reg;
	tmp = (tmp & EIC_INTENSET_EXTINT_Msk) >> EIC_INTENSET_EXTINT_Pos;
	return tmp;
}

static inline void hri_eic_write_INTEN_EXTINT_bf(const void *const hw, hri_eic_intenset_reg_t data)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT(data);
	((Eic *)hw)->INTENCLR.reg = ~EIC_INTENSET_EXTINT(data);
}

static inline void hri_eic_clear_INTEN_EXTINT_bf(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT(mask);
}

static inline void hri_eic_set_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENSET.reg = mask;
}

static inline hri_eic_intenset_reg_t hri_eic_get_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_intenset_reg_t hri_eic_read_INTEN_reg(const void *const hw)
{
	return ((Eic *)hw)->INTENSET.reg;
}

static inline void hri_eic_write_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t data)
{
	((Eic *)hw)->INTENSET.reg = data;
	((Eic *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_eic_clear_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_eic_get_NMIFLAG_NMI_bit(const void *const hw)
{
	return (((Eic *)hw)->NMIFLAG.reg & EIC_NMIFLAG_NMI) >> EIC_NMIFLAG_NMI_Pos;
}

static inline void hri_eic_clear_NMIFLAG_NMI_bit(const void *const hw)
{
	((Eic *)hw)->NMIFLAG.reg = EIC_NMIFLAG_NMI;
}

static inline hri_eic_nmiflag_reg_t hri_eic_get_NMIFLAG_reg(const void *const hw, hri_eic_nmiflag_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Eic *)hw)->NMIFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_nmiflag_reg_t hri_eic_read_NMIFLAG_reg(const void *const hw)
{
	return ((Eic *)hw)->NMIFLAG.reg;
}

static inline void hri_eic_clear_NMIFLAG_reg(const void *const hw, hri_eic_nmiflag_reg_t mask)
{
	((Eic *)hw)->NMIFLAG.reg = mask;
}

static inline hri_eic_intflag_reg_t hri_eic_get_INTFLAG_reg(const void *const hw, hri_eic_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_intflag_reg_t hri_eic_read_INTFLAG_reg(const void *const hw)
{
	return ((Eic *)hw)->INTFLAG.reg;
}

static inline void hri_eic_clear_INTFLAG_reg(const void *const hw, hri_eic_intflag_reg_t mask)
{
	((Eic *)hw)->INTFLAG.reg = mask;
}

static inline void hri_eic_set_CTRLA_SWRST_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST);
	((Eic *)hw)->CTRLA.reg |= EIC_CTRLA_SWRST;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST);
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp = (tmp & EIC_CTRLA_SWRST) >> EIC_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_eic_set_CTRLA_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST | EIC_SYNCBUSY_ENABLE);
	((Eic *)hw)->CTRLA.reg |= EIC_CTRLA_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST | EIC_SYNCBUSY_ENABLE);
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp = (tmp & EIC_CTRLA_ENABLE) >> EIC_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST | EIC_SYNCBUSY_ENABLE);
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp &= ~EIC_CTRLA_ENABLE;
	tmp |= value << EIC_CTRLA_ENABLE_Pos;
	((Eic *)hw)->CTRLA.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST | EIC_SYNCBUSY_ENABLE);
	((Eic *)hw)->CTRLA.reg &= ~EIC_CTRLA_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_SWRST | EIC_SYNCBUSY_ENABLE);
	((Eic *)hw)->CTRLA.reg ^= EIC_CTRLA_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CTRLA_CKSEL_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_MASK);
	((Eic *)hw)->CTRLA.reg |= EIC_CTRLA_CKSEL;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CTRLA_CKSEL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp = (tmp & EIC_CTRLA_CKSEL) >> EIC_CTRLA_CKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CTRLA_CKSEL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_MASK);
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp &= ~EIC_CTRLA_CKSEL;
	tmp |= value << EIC_CTRLA_CKSEL_Pos;
	((Eic *)hw)->CTRLA.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CTRLA_CKSEL_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_MASK);
	((Eic *)hw)->CTRLA.reg &= ~EIC_CTRLA_CKSEL;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CTRLA_CKSEL_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw, EIC_SYNCBUSY_MASK);
	((Eic *)hw)->CTRLA.reg ^= EIC_CTRLA_CKSEL;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CTRLA_reg(const void *const hw, hri_eic_ctrla_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRLA.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_ctrla_reg_t hri_eic_get_CTRLA_reg(const void *const hw, hri_eic_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_CTRLA_reg(const void *const hw, hri_eic_ctrla_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRLA.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CTRLA_reg(const void *const hw, hri_eic_ctrla_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRLA.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CTRLA_reg(const void *const hw, hri_eic_ctrla_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRLA.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_ctrla_reg_t hri_eic_read_CTRLA_reg(const void *const hw)
{
	return ((Eic *)hw)->CTRLA.reg;
}

static inline void hri_eic_set_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMIFILTEN) >> EIC_NMICTRL_NMIFILTEN_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_NMICTRL_NMIFILTEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= ~EIC_NMICTRL_NMIFILTEN;
	tmp |= value << EIC_NMICTRL_NMIFILTEN_Pos;
	((Eic *)hw)->NMICTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_NMICTRL_NMIASYNCH_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= EIC_NMICTRL_NMIASYNCH;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_NMICTRL_NMIASYNCH_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMIASYNCH) >> EIC_NMICTRL_NMIASYNCH_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_NMICTRL_NMIASYNCH_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= ~EIC_NMICTRL_NMIASYNCH;
	tmp |= value << EIC_NMICTRL_NMIASYNCH_Pos;
	((Eic *)hw)->NMICTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_NMIASYNCH_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~EIC_NMICTRL_NMIASYNCH;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_NMIASYNCH_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= EIC_NMICTRL_NMIASYNCH;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_get_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMISENSE(mask)) >> EIC_NMICTRL_NMISENSE_Pos;
	return tmp;
}

static inline void hri_eic_write_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t data)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= ~EIC_NMICTRL_NMISENSE_Msk;
	tmp |= EIC_NMICTRL_NMISENSE(data);
	((Eic *)hw)->NMICTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_read_NMICTRL_NMISENSE_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMISENSE_Msk) >> EIC_NMICTRL_NMISENSE_Pos;
	return tmp;
}

static inline void hri_eic_set_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_get_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_read_NMICTRL_reg(const void *const hw)
{
	return ((Eic *)hw)->NMICTRL.reg;
}

static inline void hri_eic_set_EVCTRL_EXTINTEO_bf(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_get_EVCTRL_EXTINTEO_bf(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO(mask)) >> EIC_EVCTRL_EXTINTEO_Pos;
	return tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO_bf(const void *const hw, hri_eic_evctrl_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO_Msk;
	tmp |= EIC_EVCTRL_EXTINTEO(data);
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO_bf(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO_bf(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_read_EVCTRL_EXTINTEO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO_Msk) >> EIC_EVCTRL_EXTINTEO_Pos;
	return tmp;
}

static inline void hri_eic_set_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_get_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_read_EVCTRL_reg(const void *const hw)
{
	return ((Eic *)hw)->EVCTRL.reg;
}

static inline void hri_eic_set_ASYNCH_ASYNCH_bf(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg |= EIC_ASYNCH_ASYNCH(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_asynch_reg_t hri_eic_get_ASYNCH_ASYNCH_bf(const void *const hw, hri_eic_asynch_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->ASYNCH.reg;
	tmp = (tmp & EIC_ASYNCH_ASYNCH(mask)) >> EIC_ASYNCH_ASYNCH_Pos;
	return tmp;
}

static inline void hri_eic_write_ASYNCH_ASYNCH_bf(const void *const hw, hri_eic_asynch_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->ASYNCH.reg;
	tmp &= ~EIC_ASYNCH_ASYNCH_Msk;
	tmp |= EIC_ASYNCH_ASYNCH(data);
	((Eic *)hw)->ASYNCH.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_ASYNCH_ASYNCH_bf(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg &= ~EIC_ASYNCH_ASYNCH(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_ASYNCH_ASYNCH_bf(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg ^= EIC_ASYNCH_ASYNCH(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_asynch_reg_t hri_eic_read_ASYNCH_ASYNCH_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->ASYNCH.reg;
	tmp = (tmp & EIC_ASYNCH_ASYNCH_Msk) >> EIC_ASYNCH_ASYNCH_Pos;
	return tmp;
}

static inline void hri_eic_set_ASYNCH_reg(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_asynch_reg_t hri_eic_get_ASYNCH_reg(const void *const hw, hri_eic_asynch_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->ASYNCH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_ASYNCH_reg(const void *const hw, hri_eic_asynch_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_ASYNCH_reg(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_ASYNCH_reg(const void *const hw, hri_eic_asynch_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->ASYNCH.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_asynch_reg_t hri_eic_read_ASYNCH_reg(const void *const hw)
{
	return ((Eic *)hw)->ASYNCH.reg;
}

static inline void hri_eic_set_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN0) >> EIC_CONFIG_FILTEN0_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN0;
	tmp |= value << EIC_CONFIG_FILTEN0_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN1) >> EIC_CONFIG_FILTEN1_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN1;
	tmp |= value << EIC_CONFIG_FILTEN1_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN2) >> EIC_CONFIG_FILTEN2_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN2;
	tmp |= value << EIC_CONFIG_FILTEN2_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN3) >> EIC_CONFIG_FILTEN3_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN3;
	tmp |= value << EIC_CONFIG_FILTEN3_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN4) >> EIC_CONFIG_FILTEN4_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN4;
	tmp |= value << EIC_CONFIG_FILTEN4_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN5) >> EIC_CONFIG_FILTEN5_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN5;
	tmp |= value << EIC_CONFIG_FILTEN5_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN6) >> EIC_CONFIG_FILTEN6_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN6;
	tmp |= value << EIC_CONFIG_FILTEN6_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN7) >> EIC_CONFIG_FILTEN7_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN7;
	tmp |= value << EIC_CONFIG_FILTEN7_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE0_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE0(mask)) >> EIC_CONFIG_SENSE0_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE0_Msk;
	tmp |= EIC_CONFIG_SENSE0(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE0_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE0_Msk) >> EIC_CONFIG_SENSE0_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE1_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE1(mask)) >> EIC_CONFIG_SENSE1_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE1_Msk;
	tmp |= EIC_CONFIG_SENSE1(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE1_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE1_Msk) >> EIC_CONFIG_SENSE1_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE2_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE2(mask)) >> EIC_CONFIG_SENSE2_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE2_Msk;
	tmp |= EIC_CONFIG_SENSE2(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE2_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE2_Msk) >> EIC_CONFIG_SENSE2_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE3_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE3(mask)) >> EIC_CONFIG_SENSE3_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE3_Msk;
	tmp |= EIC_CONFIG_SENSE3(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE3_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE3_Msk) >> EIC_CONFIG_SENSE3_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE4_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE4(mask)) >> EIC_CONFIG_SENSE4_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE4_Msk;
	tmp |= EIC_CONFIG_SENSE4(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE4_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE4_Msk) >> EIC_CONFIG_SENSE4_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE5_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE5(mask)) >> EIC_CONFIG_SENSE5_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE5_Msk;
	tmp |= EIC_CONFIG_SENSE5(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE5_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE5_Msk) >> EIC_CONFIG_SENSE5_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE6_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE6(mask)) >> EIC_CONFIG_SENSE6_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE6_Msk;
	tmp |= EIC_CONFIG_SENSE6(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE6_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE6_Msk) >> EIC_CONFIG_SENSE6_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE7_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE7(mask)) >> EIC_CONFIG_SENSE7_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE7_Msk;
	tmp |= EIC_CONFIG_SENSE7(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE7_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE7_Msk) >> EIC_CONFIG_SENSE7_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_reg(const void *const hw, uint8_t index,
                                                          hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_reg(const void *const hw, uint8_t index)
{
	return ((Eic *)hw)->CONFIG[index].reg;
}

static inline void hri_eic_set_DEBOUNCEN_DEBOUNCEN_bf(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg |= EIC_DEBOUNCEN_DEBOUNCEN(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_debouncen_reg_t hri_eic_get_DEBOUNCEN_DEBOUNCEN_bf(const void *const       hw,
                                                                         hri_eic_debouncen_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DEBOUNCEN.reg;
	tmp = (tmp & EIC_DEBOUNCEN_DEBOUNCEN(mask)) >> EIC_DEBOUNCEN_DEBOUNCEN_Pos;
	return tmp;
}

static inline void hri_eic_write_DEBOUNCEN_DEBOUNCEN_bf(const void *const hw, hri_eic_debouncen_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DEBOUNCEN.reg;
	tmp &= ~EIC_DEBOUNCEN_DEBOUNCEN_Msk;
	tmp |= EIC_DEBOUNCEN_DEBOUNCEN(data);
	((Eic *)hw)->DEBOUNCEN.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DEBOUNCEN_DEBOUNCEN_bf(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg &= ~EIC_DEBOUNCEN_DEBOUNCEN(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DEBOUNCEN_DEBOUNCEN_bf(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg ^= EIC_DEBOUNCEN_DEBOUNCEN(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_debouncen_reg_t hri_eic_read_DEBOUNCEN_DEBOUNCEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DEBOUNCEN.reg;
	tmp = (tmp & EIC_DEBOUNCEN_DEBOUNCEN_Msk) >> EIC_DEBOUNCEN_DEBOUNCEN_Pos;
	return tmp;
}

static inline void hri_eic_set_DEBOUNCEN_reg(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_debouncen_reg_t hri_eic_get_DEBOUNCEN_reg(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DEBOUNCEN.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_DEBOUNCEN_reg(const void *const hw, hri_eic_debouncen_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DEBOUNCEN_reg(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DEBOUNCEN_reg(const void *const hw, hri_eic_debouncen_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DEBOUNCEN.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_debouncen_reg_t hri_eic_read_DEBOUNCEN_reg(const void *const hw)
{
	return ((Eic *)hw)->DEBOUNCEN.reg;
}

static inline void hri_eic_set_DPRESCALER_STATES0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= EIC_DPRESCALER_STATES0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_DPRESCALER_STATES0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_STATES0) >> EIC_DPRESCALER_STATES0_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_DPRESCALER_STATES0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= ~EIC_DPRESCALER_STATES0;
	tmp |= value << EIC_DPRESCALER_STATES0_Pos;
	((Eic *)hw)->DPRESCALER.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_STATES0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~EIC_DPRESCALER_STATES0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_STATES0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= EIC_DPRESCALER_STATES0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_DPRESCALER_STATES1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= EIC_DPRESCALER_STATES1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_DPRESCALER_STATES1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_STATES1) >> EIC_DPRESCALER_STATES1_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_DPRESCALER_STATES1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= ~EIC_DPRESCALER_STATES1;
	tmp |= value << EIC_DPRESCALER_STATES1_Pos;
	((Eic *)hw)->DPRESCALER.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_STATES1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~EIC_DPRESCALER_STATES1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_STATES1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= EIC_DPRESCALER_STATES1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_DPRESCALER_TICKON_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= EIC_DPRESCALER_TICKON;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_DPRESCALER_TICKON_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_TICKON) >> EIC_DPRESCALER_TICKON_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_DPRESCALER_TICKON_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= ~EIC_DPRESCALER_TICKON;
	tmp |= value << EIC_DPRESCALER_TICKON_Pos;
	((Eic *)hw)->DPRESCALER.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_TICKON_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~EIC_DPRESCALER_TICKON;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_TICKON_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= EIC_DPRESCALER_TICKON;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_DPRESCALER_PRESCALER0_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= EIC_DPRESCALER_PRESCALER0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_get_DPRESCALER_PRESCALER0_bf(const void *const        hw,
                                                                            hri_eic_dprescaler_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_PRESCALER0(mask)) >> EIC_DPRESCALER_PRESCALER0_Pos;
	return tmp;
}

static inline void hri_eic_write_DPRESCALER_PRESCALER0_bf(const void *const hw, hri_eic_dprescaler_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= ~EIC_DPRESCALER_PRESCALER0_Msk;
	tmp |= EIC_DPRESCALER_PRESCALER0(data);
	((Eic *)hw)->DPRESCALER.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_PRESCALER0_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~EIC_DPRESCALER_PRESCALER0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_PRESCALER0_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= EIC_DPRESCALER_PRESCALER0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_read_DPRESCALER_PRESCALER0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_PRESCALER0_Msk) >> EIC_DPRESCALER_PRESCALER0_Pos;
	return tmp;
}

static inline void hri_eic_set_DPRESCALER_PRESCALER1_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= EIC_DPRESCALER_PRESCALER1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_get_DPRESCALER_PRESCALER1_bf(const void *const        hw,
                                                                            hri_eic_dprescaler_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_PRESCALER1(mask)) >> EIC_DPRESCALER_PRESCALER1_Pos;
	return tmp;
}

static inline void hri_eic_write_DPRESCALER_PRESCALER1_bf(const void *const hw, hri_eic_dprescaler_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= ~EIC_DPRESCALER_PRESCALER1_Msk;
	tmp |= EIC_DPRESCALER_PRESCALER1(data);
	((Eic *)hw)->DPRESCALER.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_PRESCALER1_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~EIC_DPRESCALER_PRESCALER1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_PRESCALER1_bf(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= EIC_DPRESCALER_PRESCALER1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_read_DPRESCALER_PRESCALER1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp = (tmp & EIC_DPRESCALER_PRESCALER1_Msk) >> EIC_DPRESCALER_PRESCALER1_Pos;
	return tmp;
}

static inline void hri_eic_set_DPRESCALER_reg(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_get_DPRESCALER_reg(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->DPRESCALER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_DPRESCALER_reg(const void *const hw, hri_eic_dprescaler_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_DPRESCALER_reg(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_DPRESCALER_reg(const void *const hw, hri_eic_dprescaler_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->DPRESCALER.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_dprescaler_reg_t hri_eic_read_DPRESCALER_reg(const void *const hw)
{
	return ((Eic *)hw)->DPRESCALER.reg;
}

static inline bool hri_eic_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Eic *)hw)->SYNCBUSY.reg & EIC_SYNCBUSY_SWRST) >> EIC_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_eic_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Eic *)hw)->SYNCBUSY.reg & EIC_SYNCBUSY_ENABLE) >> EIC_SYNCBUSY_ENABLE_Pos;
}

static inline hri_eic_syncbusy_reg_t hri_eic_get_SYNCBUSY_reg(const void *const hw, hri_eic_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_syncbusy_reg_t hri_eic_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Eic *)hw)->SYNCBUSY.reg;
}

static inline hri_eic_pinstate_reg_t hri_eic_get_PINSTATE_PINSTATE_bf(const void *const hw, hri_eic_pinstate_reg_t mask)
{
	return (((Eic *)hw)->PINSTATE.reg & EIC_PINSTATE_PINSTATE(mask)) >> EIC_PINSTATE_PINSTATE_Pos;
}

static inline hri_eic_pinstate_reg_t hri_eic_read_PINSTATE_PINSTATE_bf(const void *const hw)
{
	return (((Eic *)hw)->PINSTATE.reg & EIC_PINSTATE_PINSTATE_Msk) >> EIC_PINSTATE_PINSTATE_Pos;
}

static inline hri_eic_pinstate_reg_t hri_eic_get_PINSTATE_reg(const void *const hw, hri_eic_pinstate_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->PINSTATE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_pinstate_reg_t hri_eic_read_PINSTATE_reg(const void *const hw)
{
	return ((Eic *)hw)->PINSTATE.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_EIC_D51_H_INCLUDED */
#endif /* _SAMD51_EIC_COMPONENT_ */
