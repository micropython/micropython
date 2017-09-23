/**
 * \file
 *
 * \brief SAM GCLK
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

#ifdef _SAMD51_GCLK_COMPONENT_
#ifndef _HRI_GCLK_D51_H_INCLUDED_
#define _HRI_GCLK_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_GCLK_CRITICAL_SECTIONS)
#define GCLK_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define GCLK_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define GCLK_CRITICAL_SECTION_ENTER()
#define GCLK_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_gclk_genctrl_reg_t;
typedef uint32_t hri_gclk_pchctrl_reg_t;
typedef uint32_t hri_gclk_syncbusy_reg_t;
typedef uint8_t  hri_gclk_ctrla_reg_t;

static inline void hri_gclk_wait_for_sync(const void *const hw, hri_gclk_syncbusy_reg_t reg)
{
	while (((Gclk *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_gclk_is_syncing(const void *const hw, hri_gclk_syncbusy_reg_t reg)
{
	return ((Gclk *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_gclk_set_CTRLA_SWRST_bit(const void *const hw)
{
	GCLK_CRITICAL_SECTION_ENTER();
	hri_gclk_wait_for_sync(hw, GCLK_SYNCBUSY_SWRST);
	((Gclk *)hw)->CTRLA.reg |= GCLK_CTRLA_SWRST;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_gclk_wait_for_sync(hw, GCLK_SYNCBUSY_SWRST);
	tmp = ((Gclk *)hw)->CTRLA.reg;
	tmp = (tmp & GCLK_CTRLA_SWRST) >> GCLK_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_set_CTRLA_reg(const void *const hw, hri_gclk_ctrla_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->CTRLA.reg |= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_ctrla_reg_t hri_gclk_get_CTRLA_reg(const void *const hw, hri_gclk_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Gclk *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_gclk_write_CTRLA_reg(const void *const hw, hri_gclk_ctrla_reg_t data)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->CTRLA.reg = data;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_CTRLA_reg(const void *const hw, hri_gclk_ctrla_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->CTRLA.reg &= ~mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_CTRLA_reg(const void *const hw, hri_gclk_ctrla_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->CTRLA.reg ^= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_ctrla_reg_t hri_gclk_read_CTRLA_reg(const void *const hw)
{
	return ((Gclk *)hw)->CTRLA.reg;
}

static inline void hri_gclk_set_GENCTRL_GENEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_GENEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_GENEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_GENEN) >> GCLK_GENCTRL_GENEN_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_GENEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_GENEN;
	tmp |= value << GCLK_GENCTRL_GENEN_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_GENEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_GENEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_GENEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_GENEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_IDC_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_IDC;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_IDC_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_IDC) >> GCLK_GENCTRL_IDC_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_IDC_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_IDC;
	tmp |= value << GCLK_GENCTRL_IDC_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_IDC_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_IDC;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_IDC_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_IDC;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_OOV_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_OOV;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_OOV_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_OOV) >> GCLK_GENCTRL_OOV_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_OOV_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_OOV;
	tmp |= value << GCLK_GENCTRL_OOV_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_OOV_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_OOV;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_OOV_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_OOV;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_OE_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_OE;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_OE_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_OE) >> GCLK_GENCTRL_OE_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_OE_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_OE;
	tmp |= value << GCLK_GENCTRL_OE_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_OE_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_OE;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_OE_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_OE;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_DIVSEL_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_DIVSEL;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_DIVSEL_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_DIVSEL) >> GCLK_GENCTRL_DIVSEL_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_DIVSEL_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_DIVSEL;
	tmp |= value << GCLK_GENCTRL_DIVSEL_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_DIVSEL_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_DIVSEL;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_DIVSEL_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_DIVSEL;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_RUNSTDBY;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_GENCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_RUNSTDBY) >> GCLK_GENCTRL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_GENCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_RUNSTDBY;
	tmp |= value << GCLK_GENCTRL_RUNSTDBY_Pos;
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_RUNSTDBY;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_RUNSTDBY;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_GENCTRL_SRC_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_SRC(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_get_GENCTRL_SRC_bf(const void *const hw, uint8_t index,
                                                                 hri_gclk_genctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_SRC(mask)) >> GCLK_GENCTRL_SRC_Pos;
	return tmp;
}

static inline void hri_gclk_write_GENCTRL_SRC_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t data)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_SRC_Msk;
	tmp |= GCLK_GENCTRL_SRC(data);
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_SRC_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_SRC(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_SRC_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_SRC(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_read_GENCTRL_SRC_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_SRC_Msk) >> GCLK_GENCTRL_SRC_Pos;
	return tmp;
}

static inline void hri_gclk_set_GENCTRL_DIV_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= GCLK_GENCTRL_DIV(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_get_GENCTRL_DIV_bf(const void *const hw, uint8_t index,
                                                                 hri_gclk_genctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_DIV(mask)) >> GCLK_GENCTRL_DIV_Pos;
	return tmp;
}

static inline void hri_gclk_write_GENCTRL_DIV_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t data)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= ~GCLK_GENCTRL_DIV_Msk;
	tmp |= GCLK_GENCTRL_DIV(data);
	((Gclk *)hw)->GENCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_DIV_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~GCLK_GENCTRL_DIV(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_DIV_bf(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= GCLK_GENCTRL_DIV(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_read_GENCTRL_DIV_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp = (tmp & GCLK_GENCTRL_DIV_Msk) >> GCLK_GENCTRL_DIV_Pos;
	return tmp;
}

static inline void hri_gclk_set_GENCTRL_reg(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg |= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_get_GENCTRL_reg(const void *const hw, uint8_t index,
                                                              hri_gclk_genctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->GENCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_gclk_write_GENCTRL_reg(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t data)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg = data;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_GENCTRL_reg(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg &= ~mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_GENCTRL_reg(const void *const hw, uint8_t index, hri_gclk_genctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->GENCTRL[index].reg ^= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_genctrl_reg_t hri_gclk_read_GENCTRL_reg(const void *const hw, uint8_t index)
{
	return ((Gclk *)hw)->GENCTRL[index].reg;
}

static inline void hri_gclk_set_PCHCTRL_CHEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg |= GCLK_PCHCTRL_CHEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_PCHCTRL_CHEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp = (tmp & GCLK_PCHCTRL_CHEN) >> GCLK_PCHCTRL_CHEN_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_PCHCTRL_CHEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp &= ~GCLK_PCHCTRL_CHEN;
	tmp |= value << GCLK_PCHCTRL_CHEN_Pos;
	((Gclk *)hw)->PCHCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_PCHCTRL_CHEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg &= ~GCLK_PCHCTRL_CHEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_PCHCTRL_CHEN_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg ^= GCLK_PCHCTRL_CHEN;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_PCHCTRL_WRTLOCK_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg |= GCLK_PCHCTRL_WRTLOCK;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_gclk_get_PCHCTRL_WRTLOCK_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp = (tmp & GCLK_PCHCTRL_WRTLOCK) >> GCLK_PCHCTRL_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_gclk_write_PCHCTRL_WRTLOCK_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp &= ~GCLK_PCHCTRL_WRTLOCK;
	tmp |= value << GCLK_PCHCTRL_WRTLOCK_Pos;
	((Gclk *)hw)->PCHCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_PCHCTRL_WRTLOCK_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg &= ~GCLK_PCHCTRL_WRTLOCK;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_PCHCTRL_WRTLOCK_bit(const void *const hw, uint8_t index)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg ^= GCLK_PCHCTRL_WRTLOCK;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_set_PCHCTRL_GEN_bf(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg |= GCLK_PCHCTRL_GEN(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_pchctrl_reg_t hri_gclk_get_PCHCTRL_GEN_bf(const void *const hw, uint8_t index,
                                                                 hri_gclk_pchctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp = (tmp & GCLK_PCHCTRL_GEN(mask)) >> GCLK_PCHCTRL_GEN_Pos;
	return tmp;
}

static inline void hri_gclk_write_PCHCTRL_GEN_bf(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t data)
{
	uint32_t tmp;
	GCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp &= ~GCLK_PCHCTRL_GEN_Msk;
	tmp |= GCLK_PCHCTRL_GEN(data);
	((Gclk *)hw)->PCHCTRL[index].reg = tmp;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_PCHCTRL_GEN_bf(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg &= ~GCLK_PCHCTRL_GEN(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_PCHCTRL_GEN_bf(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg ^= GCLK_PCHCTRL_GEN(mask);
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_pchctrl_reg_t hri_gclk_read_PCHCTRL_GEN_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp = (tmp & GCLK_PCHCTRL_GEN_Msk) >> GCLK_PCHCTRL_GEN_Pos;
	return tmp;
}

static inline void hri_gclk_set_PCHCTRL_reg(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg |= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_pchctrl_reg_t hri_gclk_get_PCHCTRL_reg(const void *const hw, uint8_t index,
                                                              hri_gclk_pchctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->PCHCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_gclk_write_PCHCTRL_reg(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t data)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg = data;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_clear_PCHCTRL_reg(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg &= ~mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_gclk_toggle_PCHCTRL_reg(const void *const hw, uint8_t index, hri_gclk_pchctrl_reg_t mask)
{
	GCLK_CRITICAL_SECTION_ENTER();
	((Gclk *)hw)->PCHCTRL[index].reg ^= mask;
	GCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_gclk_pchctrl_reg_t hri_gclk_read_PCHCTRL_reg(const void *const hw, uint8_t index)
{
	return ((Gclk *)hw)->PCHCTRL[index].reg;
}

static inline bool hri_gclk_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) >> GCLK_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL0_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0) >> GCLK_SYNCBUSY_GENCTRL0_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL1_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL1) >> GCLK_SYNCBUSY_GENCTRL1_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL2_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL2) >> GCLK_SYNCBUSY_GENCTRL2_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL3_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL3) >> GCLK_SYNCBUSY_GENCTRL3_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL4_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL4) >> GCLK_SYNCBUSY_GENCTRL4_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL5_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL5) >> GCLK_SYNCBUSY_GENCTRL5_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL6_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL6) >> GCLK_SYNCBUSY_GENCTRL6_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL7_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL7) >> GCLK_SYNCBUSY_GENCTRL7_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL8_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL8) >> GCLK_SYNCBUSY_GENCTRL8_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL9_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL9) >> GCLK_SYNCBUSY_GENCTRL9_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL10_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL10) >> GCLK_SYNCBUSY_GENCTRL10_Pos;
}

static inline bool hri_gclk_get_SYNCBUSY_GENCTRL11_bit(const void *const hw)
{
	return (((Gclk *)hw)->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL11) >> GCLK_SYNCBUSY_GENCTRL11_Pos;
}

static inline hri_gclk_syncbusy_reg_t hri_gclk_get_SYNCBUSY_reg(const void *const hw, hri_gclk_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Gclk *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_gclk_syncbusy_reg_t hri_gclk_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Gclk *)hw)->SYNCBUSY.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_GCLK_D51_H_INCLUDED */
#endif /* _SAMD51_GCLK_COMPONENT_ */
