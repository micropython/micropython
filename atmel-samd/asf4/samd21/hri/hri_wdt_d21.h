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

#ifdef _SAMD21_WDT_COMPONENT_
#ifndef _HRI_WDT_D21_H_INCLUDED_
#define _HRI_WDT_D21_H_INCLUDED_

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

typedef uint8_t hri_wdt_clear_reg_t;
typedef uint8_t hri_wdt_config_reg_t;
typedef uint8_t hri_wdt_ctrl_reg_t;
typedef uint8_t hri_wdt_ewctrl_reg_t;
typedef uint8_t hri_wdt_intenset_reg_t;
typedef uint8_t hri_wdt_intflag_reg_t;
typedef uint8_t hri_wdt_status_reg_t;

static inline void hri_wdt_wait_for_sync(const void *const hw)
{
	while (((const Wdt *)hw)->STATUS.bit.SYNCBUSY)
		;
}

static inline bool hri_wdt_is_syncing(const void *const hw)
{
	return ((const Wdt *)hw)->STATUS.bit.SYNCBUSY;
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

static inline void hri_wdt_set_CTRL_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg |= WDT_CTRL_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp = (tmp & WDT_CTRL_ENABLE) >> WDT_CTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp &= ~WDT_CTRL_ENABLE;
	tmp |= value << WDT_CTRL_ENABLE_Pos;
	((Wdt *)hw)->CTRL.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRL_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg &= ~WDT_CTRL_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg ^= WDT_CTRL_ENABLE;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRL_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg |= WDT_CTRL_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRL_WEN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp = (tmp & WDT_CTRL_WEN) >> WDT_CTRL_WEN_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRL_WEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp &= ~WDT_CTRL_WEN;
	tmp |= value << WDT_CTRL_WEN_Pos;
	((Wdt *)hw)->CTRL.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRL_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg &= ~WDT_CTRL_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRL_WEN_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg ^= WDT_CTRL_WEN;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRL_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg |= WDT_CTRL_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_wdt_get_CTRL_ALWAYSON_bit(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp = (tmp & WDT_CTRL_ALWAYSON) >> WDT_CTRL_ALWAYSON_Pos;
	return (bool)tmp;
}

static inline void hri_wdt_write_CTRL_ALWAYSON_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp &= ~WDT_CTRL_ALWAYSON;
	tmp |= value << WDT_CTRL_ALWAYSON_Pos;
	((Wdt *)hw)->CTRL.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRL_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg &= ~WDT_CTRL_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRL_ALWAYSON_bit(const void *const hw)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CTRL.reg ^= WDT_CTRL_ALWAYSON;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_set_CTRL_reg(const void *const hw, hri_wdt_ctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRL.reg |= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ctrl_reg_t hri_wdt_get_CTRL_reg(const void *const hw, hri_wdt_ctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_wdt_write_CTRL_reg(const void *const hw, hri_wdt_ctrl_reg_t data)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRL.reg = data;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CTRL_reg(const void *const hw, hri_wdt_ctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRL.reg &= ~mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CTRL_reg(const void *const hw, hri_wdt_ctrl_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	((Wdt *)hw)->CTRL.reg ^= mask;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_ctrl_reg_t hri_wdt_read_CTRL_reg(const void *const hw)
{
	return ((Wdt *)hw)->CTRL.reg;
}

static inline void hri_wdt_set_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg |= WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_get_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_PER(mask)) >> WDT_CONFIG_PER_Pos;
	return tmp;
}

static inline void hri_wdt_write_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t data)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp &= ~WDT_CONFIG_PER_Msk;
	tmp |= WDT_CONFIG_PER(data);
	((Wdt *)hw)->CONFIG.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg &= ~WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CONFIG_PER_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg ^= WDT_CONFIG_PER(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_read_CONFIG_PER_bf(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_PER_Msk) >> WDT_CONFIG_PER_Pos;
	return tmp;
}

static inline void hri_wdt_set_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg |= WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_get_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp = (tmp & WDT_CONFIG_WINDOW(mask)) >> WDT_CONFIG_WINDOW_Pos;
	return tmp;
}

static inline void hri_wdt_write_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t data)
{
	uint8_t tmp;
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	tmp = ((Wdt *)hw)->CONFIG.reg;
	tmp &= ~WDT_CONFIG_WINDOW_Msk;
	tmp |= WDT_CONFIG_WINDOW(data);
	((Wdt *)hw)->CONFIG.reg = tmp;
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_clear_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg &= ~WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_wdt_toggle_CONFIG_WINDOW_bf(const void *const hw, hri_wdt_config_reg_t mask)
{
	WDT_CRITICAL_SECTION_ENTER();
	hri_wdt_wait_for_sync(hw);
	((Wdt *)hw)->CONFIG.reg ^= WDT_CONFIG_WINDOW(mask);
	WDT_CRITICAL_SECTION_LEAVE();
}

static inline hri_wdt_config_reg_t hri_wdt_read_CONFIG_WINDOW_bf(const void *const hw)
{
	uint8_t tmp;
	hri_wdt_wait_for_sync(hw);
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

static inline bool hri_wdt_get_STATUS_SYNCBUSY_bit(const void *const hw)
{
	return (((Wdt *)hw)->STATUS.reg & WDT_STATUS_SYNCBUSY) >> WDT_STATUS_SYNCBUSY_Pos;
}

static inline hri_wdt_status_reg_t hri_wdt_get_STATUS_reg(const void *const hw, hri_wdt_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Wdt *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_wdt_status_reg_t hri_wdt_read_STATUS_reg(const void *const hw)
{
	return ((Wdt *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_WDT_D21_H_INCLUDED */
#endif /* _SAMD21_WDT_COMPONENT_ */
