/**
 * \file
 *
 * \brief SAM OSC32KCTRL
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

#ifdef _SAMD51_OSC32KCTRL_COMPONENT_
#ifndef _HRI_OSC32KCTRL_D51_H_INCLUDED_
#define _HRI_OSC32KCTRL_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_OSC32KCTRL_CRITICAL_SECTIONS)
#define OSC32KCTRL_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define OSC32KCTRL_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define OSC32KCTRL_CRITICAL_SECTION_ENTER()
#define OSC32KCTRL_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_osc32kctrl_xosc32k_reg_t;
typedef uint32_t hri_osc32kctrl_intenset_reg_t;
typedef uint32_t hri_osc32kctrl_intflag_reg_t;
typedef uint32_t hri_osc32kctrl_osculp32k_reg_t;
typedef uint32_t hri_osc32kctrl_status_reg_t;
typedef uint8_t  hri_osc32kctrl_cfdctrl_reg_t;
typedef uint8_t  hri_osc32kctrl_evctrl_reg_t;
typedef uint8_t  hri_osc32kctrl_rtcctrl_reg_t;

static inline void hri_osc32kctrl_set_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTENSET.reg = OSC32KCTRL_INTENSET_XOSC32KRDY;
}

static inline bool hri_osc32kctrl_get_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTENSET.reg & OSC32KCTRL_INTENSET_XOSC32KRDY) >> OSC32KCTRL_INTENSET_XOSC32KRDY_Pos;
}

static inline void hri_osc32kctrl_write_INTEN_XOSC32KRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Osc32kctrl *)hw)->INTENCLR.reg = OSC32KCTRL_INTENSET_XOSC32KRDY;
	} else {
		((Osc32kctrl *)hw)->INTENSET.reg = OSC32KCTRL_INTENSET_XOSC32KRDY;
	}
}

static inline void hri_osc32kctrl_clear_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTENCLR.reg = OSC32KCTRL_INTENSET_XOSC32KRDY;
}

static inline void hri_osc32kctrl_set_INTEN_XOSC32KFAIL_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTENSET.reg = OSC32KCTRL_INTENSET_XOSC32KFAIL;
}

static inline bool hri_osc32kctrl_get_INTEN_XOSC32KFAIL_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTENSET.reg & OSC32KCTRL_INTENSET_XOSC32KFAIL) >> OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos;
}

static inline void hri_osc32kctrl_write_INTEN_XOSC32KFAIL_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Osc32kctrl *)hw)->INTENCLR.reg = OSC32KCTRL_INTENSET_XOSC32KFAIL;
	} else {
		((Osc32kctrl *)hw)->INTENSET.reg = OSC32KCTRL_INTENSET_XOSC32KFAIL;
	}
}

static inline void hri_osc32kctrl_clear_INTEN_XOSC32KFAIL_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTENCLR.reg = OSC32KCTRL_INTENSET_XOSC32KFAIL;
}

static inline void hri_osc32kctrl_set_INTEN_reg(const void *const hw, hri_osc32kctrl_intenset_reg_t mask)
{
	((Osc32kctrl *)hw)->INTENSET.reg = mask;
}

static inline hri_osc32kctrl_intenset_reg_t hri_osc32kctrl_get_INTEN_reg(const void *const             hw,
                                                                         hri_osc32kctrl_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_osc32kctrl_intenset_reg_t hri_osc32kctrl_read_INTEN_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->INTENSET.reg;
}

static inline void hri_osc32kctrl_write_INTEN_reg(const void *const hw, hri_osc32kctrl_intenset_reg_t data)
{
	((Osc32kctrl *)hw)->INTENSET.reg = data;
	((Osc32kctrl *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_osc32kctrl_clear_INTEN_reg(const void *const hw, hri_osc32kctrl_intenset_reg_t mask)
{
	((Osc32kctrl *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_osc32kctrl_get_INTFLAG_XOSC32KRDY_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTFLAG.reg & OSC32KCTRL_INTFLAG_XOSC32KRDY) >> OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos;
}

static inline void hri_osc32kctrl_clear_INTFLAG_XOSC32KRDY_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KRDY;
}

static inline bool hri_osc32kctrl_get_INTFLAG_XOSC32KFAIL_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTFLAG.reg & OSC32KCTRL_INTFLAG_XOSC32KFAIL) >> OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos;
}

static inline void hri_osc32kctrl_clear_INTFLAG_XOSC32KFAIL_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KFAIL;
}

static inline bool hri_osc32kctrl_get_interrupt_XOSC32KRDY_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTFLAG.reg & OSC32KCTRL_INTFLAG_XOSC32KRDY) >> OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos;
}

static inline void hri_osc32kctrl_clear_interrupt_XOSC32KRDY_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KRDY;
}

static inline bool hri_osc32kctrl_get_interrupt_XOSC32KFAIL_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->INTFLAG.reg & OSC32KCTRL_INTFLAG_XOSC32KFAIL) >> OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos;
}

static inline void hri_osc32kctrl_clear_interrupt_XOSC32KFAIL_bit(const void *const hw)
{
	((Osc32kctrl *)hw)->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KFAIL;
}

static inline hri_osc32kctrl_intflag_reg_t hri_osc32kctrl_get_INTFLAG_reg(const void *const            hw,
                                                                          hri_osc32kctrl_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_osc32kctrl_intflag_reg_t hri_osc32kctrl_read_INTFLAG_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->INTFLAG.reg;
}

static inline void hri_osc32kctrl_clear_INTFLAG_reg(const void *const hw, hri_osc32kctrl_intflag_reg_t mask)
{
	((Osc32kctrl *)hw)->INTFLAG.reg = mask;
}

static inline void hri_osc32kctrl_set_RTCCTRL_RTCSEL_bf(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg |= OSC32KCTRL_RTCCTRL_RTCSEL(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_rtcctrl_reg_t hri_osc32kctrl_get_RTCCTRL_RTCSEL_bf(const void *const            hw,
                                                                                hri_osc32kctrl_rtcctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->RTCCTRL.reg;
	tmp = (tmp & OSC32KCTRL_RTCCTRL_RTCSEL(mask)) >> OSC32KCTRL_RTCCTRL_RTCSEL_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_write_RTCCTRL_RTCSEL_bf(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t data)
{
	uint8_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->RTCCTRL.reg;
	tmp &= ~OSC32KCTRL_RTCCTRL_RTCSEL_Msk;
	tmp |= OSC32KCTRL_RTCCTRL_RTCSEL(data);
	((Osc32kctrl *)hw)->RTCCTRL.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_RTCCTRL_RTCSEL_bf(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg &= ~OSC32KCTRL_RTCCTRL_RTCSEL(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_RTCCTRL_RTCSEL_bf(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg ^= OSC32KCTRL_RTCCTRL_RTCSEL(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_rtcctrl_reg_t hri_osc32kctrl_read_RTCCTRL_RTCSEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->RTCCTRL.reg;
	tmp = (tmp & OSC32KCTRL_RTCCTRL_RTCSEL_Msk) >> OSC32KCTRL_RTCCTRL_RTCSEL_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_set_RTCCTRL_reg(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg |= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_rtcctrl_reg_t hri_osc32kctrl_get_RTCCTRL_reg(const void *const            hw,
                                                                          hri_osc32kctrl_rtcctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->RTCCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_osc32kctrl_write_RTCCTRL_reg(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t data)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg = data;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_RTCCTRL_reg(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg &= ~mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_RTCCTRL_reg(const void *const hw, hri_osc32kctrl_rtcctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->RTCCTRL.reg ^= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_rtcctrl_reg_t hri_osc32kctrl_read_RTCCTRL_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->RTCCTRL.reg;
}

static inline void hri_osc32kctrl_set_XOSC32K_ENABLE_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ENABLE;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_ENABLE) >> OSC32KCTRL_XOSC32K_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_ENABLE;
	tmp |= value << OSC32KCTRL_XOSC32K_ENABLE_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_ENABLE_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_ENABLE;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_ENABLE_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_ENABLE;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_XTALEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_XTALEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_XTALEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_XTALEN) >> OSC32KCTRL_XOSC32K_XTALEN_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_XTALEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_XTALEN;
	tmp |= value << OSC32KCTRL_XOSC32K_XTALEN_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_XTALEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_XTALEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_XTALEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_XTALEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_EN32K_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_EN32K) >> OSC32KCTRL_XOSC32K_EN32K_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_EN32K_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_EN32K;
	tmp |= value << OSC32KCTRL_XOSC32K_EN32K_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_EN1K_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_EN1K) >> OSC32KCTRL_XOSC32K_EN1K_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_EN1K_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_EN1K;
	tmp |= value << OSC32KCTRL_XOSC32K_EN1K_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_RUNSTDBY;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_RUNSTDBY) >> OSC32KCTRL_XOSC32K_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_RUNSTDBY;
	tmp |= value << OSC32KCTRL_XOSC32K_RUNSTDBY_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_RUNSTDBY;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_RUNSTDBY;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ONDEMAND;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_ONDEMAND) >> OSC32KCTRL_XOSC32K_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_ONDEMAND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_ONDEMAND;
	tmp |= value << OSC32KCTRL_XOSC32K_ONDEMAND_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_ONDEMAND;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_ONDEMAND;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_WRTLOCK) >> OSC32KCTRL_XOSC32K_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_WRTLOCK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_WRTLOCK;
	tmp |= value << OSC32KCTRL_XOSC32K_WRTLOCK_Pos;
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_XOSC32K_STARTUP_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_STARTUP(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_get_XOSC32K_STARTUP_bf(const void *const            hw,
                                                                                 hri_osc32kctrl_xosc32k_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_STARTUP(mask)) >> OSC32KCTRL_XOSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_STARTUP_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t data)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_STARTUP_Msk;
	tmp |= OSC32KCTRL_XOSC32K_STARTUP(data);
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_STARTUP_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_STARTUP(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_STARTUP_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_STARTUP(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_read_XOSC32K_STARTUP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_STARTUP_Msk) >> OSC32KCTRL_XOSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_set_XOSC32K_CGM_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= OSC32KCTRL_XOSC32K_CGM(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_get_XOSC32K_CGM_bf(const void *const            hw,
                                                                             hri_osc32kctrl_xosc32k_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_CGM(mask)) >> OSC32KCTRL_XOSC32K_CGM_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_CGM_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t data)
{
	uint16_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= ~OSC32KCTRL_XOSC32K_CGM_Msk;
	tmp |= OSC32KCTRL_XOSC32K_CGM(data);
	((Osc32kctrl *)hw)->XOSC32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_CGM_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_CGM(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_CGM_bf(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= OSC32KCTRL_XOSC32K_CGM(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_read_XOSC32K_CGM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & OSC32KCTRL_XOSC32K_CGM_Msk) >> OSC32KCTRL_XOSC32K_CGM_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_set_XOSC32K_reg(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg |= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_get_XOSC32K_reg(const void *const            hw,
                                                                          hri_osc32kctrl_xosc32k_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Osc32kctrl *)hw)->XOSC32K.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_osc32kctrl_write_XOSC32K_reg(const void *const hw, hri_osc32kctrl_xosc32k_reg_t data)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg = data;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_XOSC32K_reg(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg &= ~mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_XOSC32K_reg(const void *const hw, hri_osc32kctrl_xosc32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->XOSC32K.reg ^= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_xosc32k_reg_t hri_osc32kctrl_read_XOSC32K_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->XOSC32K.reg;
}

static inline void hri_osc32kctrl_set_CFDCTRL_CFDEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg |= OSC32KCTRL_CFDCTRL_CFDEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_CFDCTRL_CFDEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp = (tmp & OSC32KCTRL_CFDCTRL_CFDEN) >> OSC32KCTRL_CFDCTRL_CFDEN_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_CFDCTRL_CFDEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp &= ~OSC32KCTRL_CFDCTRL_CFDEN;
	tmp |= value << OSC32KCTRL_CFDCTRL_CFDEN_Pos;
	((Osc32kctrl *)hw)->CFDCTRL.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_CFDCTRL_CFDEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg &= ~OSC32KCTRL_CFDCTRL_CFDEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_CFDCTRL_CFDEN_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg ^= OSC32KCTRL_CFDCTRL_CFDEN;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_CFDCTRL_SWBACK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg |= OSC32KCTRL_CFDCTRL_SWBACK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_CFDCTRL_SWBACK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp = (tmp & OSC32KCTRL_CFDCTRL_SWBACK) >> OSC32KCTRL_CFDCTRL_SWBACK_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_CFDCTRL_SWBACK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp &= ~OSC32KCTRL_CFDCTRL_SWBACK;
	tmp |= value << OSC32KCTRL_CFDCTRL_SWBACK_Pos;
	((Osc32kctrl *)hw)->CFDCTRL.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_CFDCTRL_SWBACK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg &= ~OSC32KCTRL_CFDCTRL_SWBACK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_CFDCTRL_SWBACK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg ^= OSC32KCTRL_CFDCTRL_SWBACK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_CFDCTRL_CFDPRESC_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg |= OSC32KCTRL_CFDCTRL_CFDPRESC;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_CFDCTRL_CFDPRESC_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp = (tmp & OSC32KCTRL_CFDCTRL_CFDPRESC) >> OSC32KCTRL_CFDCTRL_CFDPRESC_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_CFDCTRL_CFDPRESC_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp &= ~OSC32KCTRL_CFDCTRL_CFDPRESC;
	tmp |= value << OSC32KCTRL_CFDCTRL_CFDPRESC_Pos;
	((Osc32kctrl *)hw)->CFDCTRL.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_CFDCTRL_CFDPRESC_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg &= ~OSC32KCTRL_CFDCTRL_CFDPRESC;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_CFDCTRL_CFDPRESC_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg ^= OSC32KCTRL_CFDCTRL_CFDPRESC;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_CFDCTRL_reg(const void *const hw, hri_osc32kctrl_cfdctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg |= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_cfdctrl_reg_t hri_osc32kctrl_get_CFDCTRL_reg(const void *const            hw,
                                                                          hri_osc32kctrl_cfdctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->CFDCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_osc32kctrl_write_CFDCTRL_reg(const void *const hw, hri_osc32kctrl_cfdctrl_reg_t data)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg = data;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_CFDCTRL_reg(const void *const hw, hri_osc32kctrl_cfdctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg &= ~mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_CFDCTRL_reg(const void *const hw, hri_osc32kctrl_cfdctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->CFDCTRL.reg ^= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_cfdctrl_reg_t hri_osc32kctrl_read_CFDCTRL_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->CFDCTRL.reg;
}

static inline void hri_osc32kctrl_set_EVCTRL_CFDEO_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg |= OSC32KCTRL_EVCTRL_CFDEO;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_EVCTRL_CFDEO_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->EVCTRL.reg;
	tmp = (tmp & OSC32KCTRL_EVCTRL_CFDEO) >> OSC32KCTRL_EVCTRL_CFDEO_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_EVCTRL_CFDEO_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->EVCTRL.reg;
	tmp &= ~OSC32KCTRL_EVCTRL_CFDEO;
	tmp |= value << OSC32KCTRL_EVCTRL_CFDEO_Pos;
	((Osc32kctrl *)hw)->EVCTRL.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_EVCTRL_CFDEO_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg &= ~OSC32KCTRL_EVCTRL_CFDEO;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_EVCTRL_CFDEO_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg ^= OSC32KCTRL_EVCTRL_CFDEO;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_EVCTRL_reg(const void *const hw, hri_osc32kctrl_evctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg |= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_evctrl_reg_t hri_osc32kctrl_get_EVCTRL_reg(const void *const           hw,
                                                                        hri_osc32kctrl_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Osc32kctrl *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_osc32kctrl_write_EVCTRL_reg(const void *const hw, hri_osc32kctrl_evctrl_reg_t data)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg = data;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_EVCTRL_reg(const void *const hw, hri_osc32kctrl_evctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg &= ~mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_EVCTRL_reg(const void *const hw, hri_osc32kctrl_evctrl_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->EVCTRL.reg ^= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_evctrl_reg_t hri_osc32kctrl_read_EVCTRL_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->EVCTRL.reg;
}

static inline void hri_osc32kctrl_set_OSCULP32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_OSCULP32K_EN32K_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & OSC32KCTRL_OSCULP32K_EN32K) >> OSC32KCTRL_OSCULP32K_EN32K_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_OSCULP32K_EN32K_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp &= ~OSC32KCTRL_OSCULP32K_EN32K;
	tmp |= value << OSC32KCTRL_OSCULP32K_EN32K_Pos;
	((Osc32kctrl *)hw)->OSCULP32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_OSCULP32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg &= ~OSC32KCTRL_OSCULP32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_OSCULP32K_EN32K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg ^= OSC32KCTRL_OSCULP32K_EN32K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_OSCULP32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_OSCULP32K_EN1K_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & OSC32KCTRL_OSCULP32K_EN1K) >> OSC32KCTRL_OSCULP32K_EN1K_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_OSCULP32K_EN1K_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp &= ~OSC32KCTRL_OSCULP32K_EN1K;
	tmp |= value << OSC32KCTRL_OSCULP32K_EN1K_Pos;
	((Osc32kctrl *)hw)->OSCULP32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_OSCULP32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg &= ~OSC32KCTRL_OSCULP32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_OSCULP32K_EN1K_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg ^= OSC32KCTRL_OSCULP32K_EN1K;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_osc32kctrl_get_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & OSC32KCTRL_OSCULP32K_WRTLOCK) >> OSC32KCTRL_OSCULP32K_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_osc32kctrl_write_OSCULP32K_WRTLOCK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp &= ~OSC32KCTRL_OSCULP32K_WRTLOCK;
	tmp |= value << OSC32KCTRL_OSCULP32K_WRTLOCK_Pos;
	((Osc32kctrl *)hw)->OSCULP32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg &= ~OSC32KCTRL_OSCULP32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg ^= OSC32KCTRL_OSCULP32K_WRTLOCK;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_set_OSCULP32K_CALIB_bf(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_CALIB(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_osculp32k_reg_t hri_osc32kctrl_get_OSCULP32K_CALIB_bf(const void *const              hw,
                                                                                   hri_osc32kctrl_osculp32k_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & OSC32KCTRL_OSCULP32K_CALIB(mask)) >> OSC32KCTRL_OSCULP32K_CALIB_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_write_OSCULP32K_CALIB_bf(const void *const hw, hri_osc32kctrl_osculp32k_reg_t data)
{
	uint32_t tmp;
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp &= ~OSC32KCTRL_OSCULP32K_CALIB_Msk;
	tmp |= OSC32KCTRL_OSCULP32K_CALIB(data);
	((Osc32kctrl *)hw)->OSCULP32K.reg = tmp;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_OSCULP32K_CALIB_bf(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg &= ~OSC32KCTRL_OSCULP32K_CALIB(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_OSCULP32K_CALIB_bf(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg ^= OSC32KCTRL_OSCULP32K_CALIB(mask);
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_osculp32k_reg_t hri_osc32kctrl_read_OSCULP32K_CALIB_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & OSC32KCTRL_OSCULP32K_CALIB_Msk) >> OSC32KCTRL_OSCULP32K_CALIB_Pos;
	return tmp;
}

static inline void hri_osc32kctrl_set_OSCULP32K_reg(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg |= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_osculp32k_reg_t hri_osc32kctrl_get_OSCULP32K_reg(const void *const              hw,
                                                                              hri_osc32kctrl_osculp32k_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->OSCULP32K.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_osc32kctrl_write_OSCULP32K_reg(const void *const hw, hri_osc32kctrl_osculp32k_reg_t data)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg = data;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_clear_OSCULP32K_reg(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg &= ~mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_osc32kctrl_toggle_OSCULP32K_reg(const void *const hw, hri_osc32kctrl_osculp32k_reg_t mask)
{
	OSC32KCTRL_CRITICAL_SECTION_ENTER();
	((Osc32kctrl *)hw)->OSCULP32K.reg ^= mask;
	OSC32KCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_osc32kctrl_osculp32k_reg_t hri_osc32kctrl_read_OSCULP32K_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->OSCULP32K.reg;
}

static inline bool hri_osc32kctrl_get_STATUS_XOSC32KRDY_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) >> OSC32KCTRL_STATUS_XOSC32KRDY_Pos;
}

static inline bool hri_osc32kctrl_get_STATUS_XOSC32KFAIL_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KFAIL) >> OSC32KCTRL_STATUS_XOSC32KFAIL_Pos;
}

static inline bool hri_osc32kctrl_get_STATUS_XOSC32KSW_bit(const void *const hw)
{
	return (((Osc32kctrl *)hw)->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KSW) >> OSC32KCTRL_STATUS_XOSC32KSW_Pos;
}

static inline hri_osc32kctrl_status_reg_t hri_osc32kctrl_get_STATUS_reg(const void *const           hw,
                                                                        hri_osc32kctrl_status_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Osc32kctrl *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_osc32kctrl_status_reg_t hri_osc32kctrl_read_STATUS_reg(const void *const hw)
{
	return ((Osc32kctrl *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_OSC32KCTRL_D51_H_INCLUDED */
#endif /* _SAMD51_OSC32KCTRL_COMPONENT_ */
