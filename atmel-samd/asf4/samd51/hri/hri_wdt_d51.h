/**
 * \file
 *
 * \brief SAM WDT
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

#ifdef _SAMD51_WDT_COMPONENT_
#ifndef _HRI_WDT_D51_H_INCLUDED_
#define _HRI_WDT_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_WDT_CRITICAL_SECTIONS)
#define WDT_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define WDT_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define WDT_CRITICAL_SECTION_ENTER()
#define WDT_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_wdt_syncbusy_reg_t;
typedef uint8_t  hri_wdt_clear_reg_t;
typedef uint8_t  hri_wdt_config_reg_t;
typedef uint8_t  hri_wdt_ctrla_reg_t;
typedef uint8_t  hri_wdt_ewctrl_reg_t;
typedef uint8_t  hri_wdt_intenset_reg_t;
typedef uint8_t  hri_wdt_intflag_reg_t;

static inline void hri_wdt_wait_for_sync(const void *const hw, hri_wdt_syncbusy_reg_t reg)
{
	while (((Wdt *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_wdt_is_syncing(const void *const hw, hri_wdt_syncbusy_reg_t reg)
{
	return ((Wdt *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_wdt_set_INTEN_EW_bit(const void *const hw)
{
	((Wdt *)hw)->INTENSET.reg = WDT_INTENSET_EW;
}

static inline bool hri_wdt_get_INTEN_EW_bit(const void *const hw)
{
	return (((Wdt *)hw)->INTENSET.reg & WDT_INTENSET_EW) >> WDT_INTENSET_EW_Pos;
}

static inline void hri_wdt_write_INTEN_EW_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Wdt *)hw)->INTENCLR.reg = WDT_INTENSET_EW;
	} else {
		((Wdt *)hw)->INTENSET.reg = WDT_INTENSET_EW;
	}
}

static inline void hri_wdt_clear_INTEN_EW_bit(const void *const hw)
{
	((Wdt *)hw)->INTENCLR.reg = WDT_INTENSET_EW;
}

static inline void hri_wdt_set_INTEN_reg(const void *const hw, hri_wdt_intenset_reg_t mask)
{
	((Wdt *)hw)->INTENSET.reg = mask;
}

static inline hri_wdt_intenset_reg_t hri_wdt_get_INTEN_reg(const void *const hw, hri_wdt_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_wdt_intenset_reg_t hri_wdt_read_INTEN_reg(const void *const hw)
{
	return ((Wdt *)hw)->INTENSET.reg;
}

static inline void hri_wdt_write_INTEN_reg(const void *const hw, hri_wdt_intenset_reg_t data)
{
	((Wdt *)hw)->INTENSET.reg = data;
	((Wdt *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_wdt_clear_INTEN_reg(const void *const hw, hri_wdt_intenset_reg_t mask)
{
	((Wdt *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_wdt_get_INTFLAG_EW_bit(const void *const hw)
{
	return (((Wdt *)hw)->INTFLAG.reg & WDT_INTFLAG_EW) >> WDT_INTFLAG_EW_Pos;
}

static inline void hri_wdt_clear_INTFLAG_EW_bit(const void *const hw)
{
	((Wdt *)hw)->INTFLAG.reg = WDT_INTFLAG_EW;
}

static inline bool hri_wdt_get_interrupt_EW_bit(const void *const hw)
{
	return (((Wdt *)hw)->INTFLAG.reg & WDT_INTFLAG_EW) >> WDT_INTFLAG_EW_Pos;
}

static inline void hri_wdt_clear_interrupt_EW_bit(const void *const hw)
{
	((Wdt *)hw)->INTFLAG.reg = WDT_INTFLAG_EW;
}

static inline hri_wdt_intflag_reg_t hri_wdt_get_INTFLAG_reg(const void *const hw, hri_wdt_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_wdt_intflag_reg_t hri_wdt_read_INTFLAG_reg(const void *const hw)
{
	return ((Wdt *)hw)->INTFLAG.reg;
}

static inline void hri_wdt_clear_INTFLAG_reg(const void *const hw, hri_wdt_intflag_reg_t mask)
{
	((Wdt *)hw)->INTFLAG.reg = mask;
}

static inline void hri_wdt_write_CLEAR_reg(const void *const hw, hri_wdt_clear_reg_t data)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CLEAR.reg = data;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRLA_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg |= WDT_CTRLA_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp = (tmp & WDT_CTRLA_ENABLE) >> WDT_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp &= ~WDT_CTRLA_ENABLE;
	tmp |= value << WDT_CTRLA_ENABLE_Pos;
	((Wdt *)hw)->CTRLA.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg &= ~WDT_CTRLA_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg ^= WDT_CTRLA_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRLA_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg |= WDT_CTRLA_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRLA_WEN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp = (tmp & WDT_CTRLA_WEN) >> WDT_CTRLA_WEN_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRLA_WEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp &= ~WDT_CTRLA_WEN;
	tmp |= value << WDT_CTRLA_WEN_Pos;
	((Wdt *)hw)->CTRLA.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRLA_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg &= ~WDT_CTRLA_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRLA_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg ^= WDT_CTRLA_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRLA_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg |= WDT_CTRLA_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRLA_ALWAYSON_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp = (tmp & WDT_CTRLA_ALWAYSON) >> WDT_CTRLA_ALWAYSON_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRLA_ALWAYSON_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp &= ~WDT_CTRLA_ALWAYSON;
	tmp |= value << WDT_CTRLA_ALWAYSON_Pos;
	((Wdt *)hw)->CTRLA.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRLA_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg &= ~WDT_CTRLA_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRLA_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw, WDT_SYNCBUSY_ENABLE | WDT_SYNCBUSY_WEN | WDT_SYNCBUSY_ALWAYSON);
	((Wdt *)hw)->CTRLA.reg ^= WDT_CTRLA_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRLA_reg(const void *const hw, hri_wdt_ctrla_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRLA.reg |= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ctrla_reg_t hri_wdt_get_CTRLA_reg(const void *const hw, hri_wdt_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_wdt_write_CTRLA_reg(const void *const hw, hri_wdt_ctrla_reg_t data)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRLA.reg = data;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRLA_reg(const void *const hw, hri_wdt_ctrla_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRLA.reg &= ~mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRLA_reg(const void *const hw, hri_wdt_ctrla_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRLA.reg ^= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ctrla_reg_t hri_wdt_read_CTRLA_reg(const void *const hw)
{
	return ((Wdt *)hw)->CTRLA.reg;
}

static inline void hri_wdt_set_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg |= WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_get_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_PER(mask)) >> WDT_CONFIG_PER_Pos;
	return tmp;
}

static inline void hri_wdt_write_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t data)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp &= ~WDT_CONFIG_PER_Msk;
	tmp |= WDT_CONFIG_PER(data);
	((Wdt *)hw)->CONFIG.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg &= ~WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg ^= WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_read_CONFIG_PER_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_PER_Msk) >> WDT_CONFIG_PER_Pos;
	return tmp;
}

static inline void hri_wdt_set_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg |= WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_get_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_WINDOW(mask)) >> WDT_CONFIG_WINDOW_Pos;
	return tmp;
}

static inline void hri_wdt_write_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t data)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp &= ~WDT_CONFIG_WINDOW_Msk;
	tmp |= WDT_CONFIG_WINDOW(data);
	((Wdt *)hw)->CONFIG.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg &= ~WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg ^= WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_read_CONFIG_WINDOW_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_WINDOW_Msk) >> WDT_CONFIG_WINDOW_Pos;
	return tmp;
}

static inline void hri_wdt_set_CONFIG_reg(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg |= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_get_CONFIG_reg(const void *const hw, hri_wdt_config_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_wdt_write_CONFIG_reg(const void *const hw, hri_wdt_config_reg_t data)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg = data;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CONFIG_reg(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg &= ~mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CONFIG_reg(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CONFIG.reg ^= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_read_CONFIG_reg(const void *const hw)
{
	return ((Wdt *)hw)->CONFIG.reg;
}

static inline void hri_wdt_set_EWCTRL_EWOFFSET_bf(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg |= WDT_EWCTRL_EWOFFSET(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ewctrl_reg_t hri_wdt_get_EWCTRL_EWOFFSET_bf(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->EWCTRL.reg;
	tmp = (tmp & WDT_EWCTRL_EWOFFSET(mask)) >> WDT_EWCTRL_EWOFFSET_Pos;
	return tmp;
}

static inline void hri_wdt_write_EWCTRL_EWOFFSET_bf(const void *const hw, hri_wdt_ewctrl_reg_t data)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	tmp = ((Wdt *)hw)->EWCTRL.reg;
	tmp &= ~WDT_EWCTRL_EWOFFSET_Msk;
	tmp |= WDT_EWCTRL_EWOFFSET(data);
	((Wdt *)hw)->EWCTRL.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_EWCTRL_EWOFFSET_bf(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg &= ~WDT_EWCTRL_EWOFFSET(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_EWCTRL_EWOFFSET_bf(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg ^= WDT_EWCTRL_EWOFFSET(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ewctrl_reg_t hri_wdt_read_EWCTRL_EWOFFSET_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->EWCTRL.reg;
	tmp = (tmp & WDT_EWCTRL_EWOFFSET_Msk) >> WDT_EWCTRL_EWOFFSET_Pos;
	return tmp;
}

static inline void hri_wdt_set_EWCTRL_reg(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg |= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ewctrl_reg_t hri_wdt_get_EWCTRL_reg(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->EWCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_wdt_write_EWCTRL_reg(const void *const hw, hri_wdt_ewctrl_reg_t data)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg = data;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_EWCTRL_reg(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg &= ~mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_EWCTRL_reg(const void *const hw, hri_wdt_ewctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->EWCTRL.reg ^= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ewctrl_reg_t hri_wdt_read_EWCTRL_reg(const void *const hw)
{
	return ((Wdt *)hw)->EWCTRL.reg;
}

static inline bool hri_wdt_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Wdt *)hw)->SYNCBUSY.reg & WDT_SYNCBUSY_ENABLE) >> WDT_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_wdt_get_SYNCBUSY_WEN_bit(const void *const hw)
{
	return (((Wdt *)hw)->SYNCBUSY.reg & WDT_SYNCBUSY_WEN) >> WDT_SYNCBUSY_WEN_Pos;
}

static inline bool hri_wdt_get_SYNCBUSY_ALWAYSON_bit(const void *const hw)
{
	return (((Wdt *)hw)->SYNCBUSY.reg & WDT_SYNCBUSY_ALWAYSON) >> WDT_SYNCBUSY_ALWAYSON_Pos;
}

static inline bool hri_wdt_get_SYNCBUSY_CLEAR_bit(const void *const hw)
{
	return (((Wdt *)hw)->SYNCBUSY.reg & WDT_SYNCBUSY_CLEAR) >> WDT_SYNCBUSY_CLEAR_Pos;
}

static inline hri_wdt_syncbusy_reg_t hri_wdt_get_SYNCBUSY_reg(const void *const hw, hri_wdt_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Wdt *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_wdt_syncbusy_reg_t hri_wdt_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Wdt *)hw)->SYNCBUSY.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_WDT_D51_H_INCLUDED */
#endif /* _SAMD51_WDT_COMPONENT_ */
