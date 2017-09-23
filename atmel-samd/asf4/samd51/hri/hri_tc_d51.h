/**
 * \file
 *
 * \brief SAM TC
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

#ifdef _SAMD51_TC_COMPONENT_
#ifndef _HRI_TC_D51_H_INCLUDED_
#define _HRI_TC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_TC_CRITICAL_SECTIONS)
#define TC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define TC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define TC_CRITICAL_SECTION_ENTER()
#define TC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_tc_cc16_reg_t;
typedef uint16_t hri_tc_ccbuf16_reg_t;
typedef uint16_t hri_tc_count16_reg_t;
typedef uint16_t hri_tc_evctrl_reg_t;
typedef uint32_t hri_tc_cc32_reg_t;
typedef uint32_t hri_tc_ccbuf32_reg_t;
typedef uint32_t hri_tc_count32_reg_t;
typedef uint32_t hri_tc_ctrla_reg_t;
typedef uint32_t hri_tc_syncbusy_reg_t;
typedef uint8_t  hri_tc_cc8_reg_t;
typedef uint8_t  hri_tc_ccbuf8_reg_t;
typedef uint8_t  hri_tc_count8_reg_t;
typedef uint8_t  hri_tc_ctrlbset_reg_t;
typedef uint8_t  hri_tc_dbgctrl_reg_t;
typedef uint8_t  hri_tc_drvctrl_reg_t;
typedef uint8_t  hri_tc_intenset_reg_t;
typedef uint8_t  hri_tc_intflag_reg_t;
typedef uint8_t  hri_tc_per_reg_t;
typedef uint8_t  hri_tc_perbuf_reg_t;
typedef uint8_t  hri_tc_status_reg_t;
typedef uint8_t  hri_tc_wave_reg_t;

static inline void hri_tc_wait_for_sync(const void *const hw, hri_tc_syncbusy_reg_t reg)
{
	while (((Tc *)hw)->COUNT8.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_tc_is_syncing(const void *const hw, hri_tc_syncbusy_reg_t reg)
{
	return ((Tc *)hw)->COUNT8.SYNCBUSY.reg & reg;
}

static inline void hri_tccount16_set_COUNT_COUNT_bf(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg |= TC_COUNT16_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count16_reg_t hri_tccount16_get_COUNT_COUNT_bf(const void *const hw, hri_tc_count16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.COUNT.reg;
	tmp = (tmp & TC_COUNT16_COUNT_COUNT(mask)) >> TC_COUNT16_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount16_write_COUNT_COUNT_bf(const void *const hw, hri_tc_count16_reg_t data)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT16.COUNT.reg;
	tmp &= ~TC_COUNT16_COUNT_COUNT_Msk;
	tmp |= TC_COUNT16_COUNT_COUNT(data);
	((Tc *)hw)->COUNT16.COUNT.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_COUNT_COUNT_bf(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg &= ~TC_COUNT16_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_COUNT_COUNT_bf(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg ^= TC_COUNT16_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count16_reg_t hri_tccount16_read_COUNT_COUNT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.COUNT.reg;
	tmp = (tmp & TC_COUNT16_COUNT_COUNT_Msk) >> TC_COUNT16_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount16_set_COUNT_reg(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count16_reg_t hri_tccount16_get_COUNT_reg(const void *const hw, hri_tc_count16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount16_write_COUNT_reg(const void *const hw, hri_tc_count16_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_COUNT_reg(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_COUNT_reg(const void *const hw, hri_tc_count16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.COUNT.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count16_reg_t hri_tccount16_read_COUNT_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT16.COUNT.reg;
}

static inline void hri_tccount16_set_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg |= TC_COUNT16_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc16_reg_t hri_tccount16_get_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CC[index].reg;
	tmp = (tmp & TC_COUNT16_CC_CC(mask)) >> TC_COUNT16_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount16_write_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc16_reg_t data)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT16.CC[index].reg;
	tmp &= ~TC_COUNT16_CC_CC_Msk;
	tmp |= TC_COUNT16_CC_CC(data);
	((Tc *)hw)->COUNT16.CC[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg &= ~TC_COUNT16_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg ^= TC_COUNT16_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc16_reg_t hri_tccount16_read_CC_CC_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CC[index].reg;
	tmp = (tmp & TC_COUNT16_CC_CC_Msk) >> TC_COUNT16_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount16_set_CC_reg(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc16_reg_t hri_tccount16_get_CC_reg(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CC[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount16_write_CC_reg(const void *const hw, uint8_t index, hri_tc_cc16_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_CC_reg(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_CC_reg(const void *const hw, uint8_t index, hri_tc_cc16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CC[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc16_reg_t hri_tccount16_read_CC_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT16.CC[index].reg;
}

static inline void hri_tccount16_set_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg |= TC_COUNT16_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf16_reg_t hri_tccount16_get_CCBUF_CCBUF_bf(const void *const hw, uint8_t index,
                                                                    hri_tc_ccbuf16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT16_CCBUF_CCBUF(mask)) >> TC_COUNT16_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount16_write_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t data)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT16.CCBUF[index].reg;
	tmp &= ~TC_COUNT16_CCBUF_CCBUF_Msk;
	tmp |= TC_COUNT16_CCBUF_CCBUF(data);
	((Tc *)hw)->COUNT16.CCBUF[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg &= ~TC_COUNT16_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg ^= TC_COUNT16_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf16_reg_t hri_tccount16_read_CCBUF_CCBUF_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT16_CCBUF_CCBUF_Msk) >> TC_COUNT16_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount16_set_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf16_reg_t hri_tccount16_get_CCBUF_reg(const void *const hw, uint8_t index,
                                                               hri_tc_ccbuf16_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT16.CCBUF[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount16_write_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_clear_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount16_toggle_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf16_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT16.CCBUF[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf16_reg_t hri_tccount16_read_CCBUF_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT16.CCBUF[index].reg;
}

static inline void hri_tccount32_set_COUNT_COUNT_bf(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg |= TC_COUNT32_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count32_reg_t hri_tccount32_get_COUNT_COUNT_bf(const void *const hw, hri_tc_count32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.COUNT.reg;
	tmp = (tmp & TC_COUNT32_COUNT_COUNT(mask)) >> TC_COUNT32_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount32_write_COUNT_COUNT_bf(const void *const hw, hri_tc_count32_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT32.COUNT.reg;
	tmp &= ~TC_COUNT32_COUNT_COUNT_Msk;
	tmp |= TC_COUNT32_COUNT_COUNT(data);
	((Tc *)hw)->COUNT32.COUNT.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_COUNT_COUNT_bf(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg &= ~TC_COUNT32_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_COUNT_COUNT_bf(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg ^= TC_COUNT32_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count32_reg_t hri_tccount32_read_COUNT_COUNT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.COUNT.reg;
	tmp = (tmp & TC_COUNT32_COUNT_COUNT_Msk) >> TC_COUNT32_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount32_set_COUNT_reg(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count32_reg_t hri_tccount32_get_COUNT_reg(const void *const hw, hri_tc_count32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount32_write_COUNT_reg(const void *const hw, hri_tc_count32_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_COUNT_reg(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_COUNT_reg(const void *const hw, hri_tc_count32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.COUNT.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count32_reg_t hri_tccount32_read_COUNT_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT32.COUNT.reg;
}

static inline void hri_tccount32_set_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg |= TC_COUNT32_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc32_reg_t hri_tccount32_get_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CC[index].reg;
	tmp = (tmp & TC_COUNT32_CC_CC(mask)) >> TC_COUNT32_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount32_write_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc32_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT32.CC[index].reg;
	tmp &= ~TC_COUNT32_CC_CC_Msk;
	tmp |= TC_COUNT32_CC_CC(data);
	((Tc *)hw)->COUNT32.CC[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg &= ~TC_COUNT32_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg ^= TC_COUNT32_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc32_reg_t hri_tccount32_read_CC_CC_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CC[index].reg;
	tmp = (tmp & TC_COUNT32_CC_CC_Msk) >> TC_COUNT32_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount32_set_CC_reg(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc32_reg_t hri_tccount32_get_CC_reg(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CC[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount32_write_CC_reg(const void *const hw, uint8_t index, hri_tc_cc32_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_CC_reg(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_CC_reg(const void *const hw, uint8_t index, hri_tc_cc32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CC[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc32_reg_t hri_tccount32_read_CC_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT32.CC[index].reg;
}

static inline void hri_tccount32_set_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg |= TC_COUNT32_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf32_reg_t hri_tccount32_get_CCBUF_CCBUF_bf(const void *const hw, uint8_t index,
                                                                    hri_tc_ccbuf32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT32_CCBUF_CCBUF(mask)) >> TC_COUNT32_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount32_write_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT32.CCBUF[index].reg;
	tmp &= ~TC_COUNT32_CCBUF_CCBUF_Msk;
	tmp |= TC_COUNT32_CCBUF_CCBUF(data);
	((Tc *)hw)->COUNT32.CCBUF[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg &= ~TC_COUNT32_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg ^= TC_COUNT32_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf32_reg_t hri_tccount32_read_CCBUF_CCBUF_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT32_CCBUF_CCBUF_Msk) >> TC_COUNT32_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount32_set_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf32_reg_t hri_tccount32_get_CCBUF_reg(const void *const hw, uint8_t index,
                                                               hri_tc_ccbuf32_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT32.CCBUF[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount32_write_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_clear_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount32_toggle_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf32_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT32.CCBUF[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf32_reg_t hri_tccount32_read_CCBUF_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT32.CCBUF[index].reg;
}

static inline void hri_tc_set_CTRLB_DIR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_DIR;
}

static inline bool hri_tc_get_CTRLB_DIR_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.CTRLBSET.reg & TC_CTRLBSET_DIR) >> TC_CTRLBSET_DIR_Pos;
}

static inline void hri_tc_write_CTRLB_DIR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_DIR;
	} else {
		((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_DIR;
	}
}

static inline void hri_tc_clear_CTRLB_DIR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_DIR;
}

static inline void hri_tc_set_CTRLB_LUPD_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_LUPD;
}

static inline bool hri_tc_get_CTRLB_LUPD_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.CTRLBSET.reg & TC_CTRLBSET_LUPD) >> TC_CTRLBSET_LUPD_Pos;
}

static inline void hri_tc_write_CTRLB_LUPD_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_LUPD;
	} else {
		((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_LUPD;
	}
}

static inline void hri_tc_clear_CTRLB_LUPD_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_LUPD;
}

static inline void hri_tc_set_CTRLB_ONESHOT_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_ONESHOT;
}

static inline bool hri_tc_get_CTRLB_ONESHOT_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.CTRLBSET.reg & TC_CTRLBSET_ONESHOT) >> TC_CTRLBSET_ONESHOT_Pos;
}

static inline void hri_tc_write_CTRLB_ONESHOT_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_ONESHOT;
	} else {
		((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_ONESHOT;
	}
}

static inline void hri_tc_clear_CTRLB_ONESHOT_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_ONESHOT;
}

static inline void hri_tc_set_CTRLB_CMD_bf(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_CMD(mask);
}

static inline hri_tc_ctrlbset_reg_t hri_tc_get_CTRLB_CMD_bf(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLBSET.reg;
	tmp = (tmp & TC_CTRLBSET_CMD(mask)) >> TC_CTRLBSET_CMD_Pos;
	return tmp;
}

static inline hri_tc_ctrlbset_reg_t hri_tc_read_CTRLB_CMD_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLBSET.reg;
	tmp = (tmp & TC_CTRLBSET_CMD_Msk) >> TC_CTRLBSET_CMD_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLB_CMD_bf(const void *const hw, hri_tc_ctrlbset_reg_t data)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = TC_CTRLBSET_CMD(data);
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = ~TC_CTRLBSET_CMD(data);
}

static inline void hri_tc_clear_CTRLB_CMD_bf(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = TC_CTRLBSET_CMD(mask);
}

static inline void hri_tc_set_CTRLB_reg(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = mask;
}

static inline hri_tc_ctrlbset_reg_t hri_tc_get_CTRLB_reg(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLBSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tc_ctrlbset_reg_t hri_tc_read_CTRLB_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.CTRLBSET.reg;
}

static inline void hri_tc_write_CTRLB_reg(const void *const hw, hri_tc_ctrlbset_reg_t data)
{
	((Tc *)hw)->COUNT8.CTRLBSET.reg = data;
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = ~data;
}

static inline void hri_tc_clear_CTRLB_reg(const void *const hw, hri_tc_ctrlbset_reg_t mask)
{
	((Tc *)hw)->COUNT8.CTRLBCLR.reg = mask;
}

static inline void hri_tc_set_INTEN_OVF_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_OVF;
}

static inline bool hri_tc_get_INTEN_OVF_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTENSET.reg & TC_INTENSET_OVF) >> TC_INTENSET_OVF_Pos;
}

static inline void hri_tc_write_INTEN_OVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_OVF;
	} else {
		((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_OVF;
	}
}

static inline void hri_tc_clear_INTEN_OVF_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_OVF;
}

static inline void hri_tc_set_INTEN_ERR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_ERR;
}

static inline bool hri_tc_get_INTEN_ERR_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTENSET.reg & TC_INTENSET_ERR) >> TC_INTENSET_ERR_Pos;
}

static inline void hri_tc_write_INTEN_ERR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_ERR;
	} else {
		((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_ERR;
	}
}

static inline void hri_tc_clear_INTEN_ERR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_ERR;
}

static inline void hri_tc_set_INTEN_MC0_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_MC0;
}

static inline bool hri_tc_get_INTEN_MC0_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTENSET.reg & TC_INTENSET_MC0) >> TC_INTENSET_MC0_Pos;
}

static inline void hri_tc_write_INTEN_MC0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_MC0;
	} else {
		((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_MC0;
	}
}

static inline void hri_tc_clear_INTEN_MC0_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_MC0;
}

static inline void hri_tc_set_INTEN_MC1_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_MC1;
}

static inline bool hri_tc_get_INTEN_MC1_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTENSET.reg & TC_INTENSET_MC1) >> TC_INTENSET_MC1_Pos;
}

static inline void hri_tc_write_INTEN_MC1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_MC1;
	} else {
		((Tc *)hw)->COUNT8.INTENSET.reg = TC_INTENSET_MC1;
	}
}

static inline void hri_tc_clear_INTEN_MC1_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTENCLR.reg = TC_INTENSET_MC1;
}

static inline void hri_tc_set_INTEN_reg(const void *const hw, hri_tc_intenset_reg_t mask)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = mask;
}

static inline hri_tc_intenset_reg_t hri_tc_get_INTEN_reg(const void *const hw, hri_tc_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tc_intenset_reg_t hri_tc_read_INTEN_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.INTENSET.reg;
}

static inline void hri_tc_write_INTEN_reg(const void *const hw, hri_tc_intenset_reg_t data)
{
	((Tc *)hw)->COUNT8.INTENSET.reg = data;
	((Tc *)hw)->COUNT8.INTENCLR.reg = ~data;
}

static inline void hri_tc_clear_INTEN_reg(const void *const hw, hri_tc_intenset_reg_t mask)
{
	((Tc *)hw)->COUNT8.INTENCLR.reg = mask;
}

static inline bool hri_tc_get_INTFLAG_OVF_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_OVF) >> TC_INTFLAG_OVF_Pos;
}

static inline void hri_tc_clear_INTFLAG_OVF_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_OVF;
}

static inline bool hri_tc_get_INTFLAG_ERR_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_ERR) >> TC_INTFLAG_ERR_Pos;
}

static inline void hri_tc_clear_INTFLAG_ERR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_ERR;
}

static inline bool hri_tc_get_INTFLAG_MC0_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_MC0) >> TC_INTFLAG_MC0_Pos;
}

static inline void hri_tc_clear_INTFLAG_MC0_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_MC0;
}

static inline bool hri_tc_get_INTFLAG_MC1_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_MC1) >> TC_INTFLAG_MC1_Pos;
}

static inline void hri_tc_clear_INTFLAG_MC1_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_MC1;
}

static inline bool hri_tc_get_interrupt_OVF_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_OVF) >> TC_INTFLAG_OVF_Pos;
}

static inline void hri_tc_clear_interrupt_OVF_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_OVF;
}

static inline bool hri_tc_get_interrupt_ERR_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_ERR) >> TC_INTFLAG_ERR_Pos;
}

static inline void hri_tc_clear_interrupt_ERR_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_ERR;
}

static inline bool hri_tc_get_interrupt_MC0_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_MC0) >> TC_INTFLAG_MC0_Pos;
}

static inline void hri_tc_clear_interrupt_MC0_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_MC0;
}

static inline bool hri_tc_get_interrupt_MC1_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.INTFLAG.reg & TC_INTFLAG_MC1) >> TC_INTFLAG_MC1_Pos;
}

static inline void hri_tc_clear_interrupt_MC1_bit(const void *const hw)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = TC_INTFLAG_MC1;
}

static inline hri_tc_intflag_reg_t hri_tc_get_INTFLAG_reg(const void *const hw, hri_tc_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tc_intflag_reg_t hri_tc_read_INTFLAG_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.INTFLAG.reg;
}

static inline void hri_tc_clear_INTFLAG_reg(const void *const hw, hri_tc_intflag_reg_t mask)
{
	((Tc *)hw)->COUNT8.INTFLAG.reg = mask;
}

static inline void hri_tc_set_CTRLA_SWRST_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_SWRST;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_SWRST) >> TC_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_tc_set_CTRLA_ENABLE_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_ENABLE) >> TC_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_ENABLE;
	tmp |= value << TC_CTRLA_ENABLE_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_ENABLE;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_RUNSTDBY;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_RUNSTDBY) >> TC_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_RUNSTDBY;
	tmp |= value << TC_CTRLA_RUNSTDBY_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_RUNSTDBY;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_RUNSTDBY;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_ONDEMAND_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_ONDEMAND;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_ONDEMAND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_ONDEMAND) >> TC_CTRLA_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_ONDEMAND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_ONDEMAND;
	tmp |= value << TC_CTRLA_ONDEMAND_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_ONDEMAND_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_ONDEMAND;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_ONDEMAND_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_ONDEMAND;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_ALOCK_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_ALOCK;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_ALOCK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_ALOCK) >> TC_CTRLA_ALOCK_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_ALOCK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_ALOCK;
	tmp |= value << TC_CTRLA_ALOCK_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_ALOCK_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_ALOCK;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_ALOCK_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_ALOCK;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_CAPTEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_CAPTEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_CAPTEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTEN0) >> TC_CTRLA_CAPTEN0_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_CAPTEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_CAPTEN0;
	tmp |= value << TC_CTRLA_CAPTEN0_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_CAPTEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_CAPTEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_CAPTEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_CAPTEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_CAPTEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_CAPTEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_CAPTEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTEN1) >> TC_CTRLA_CAPTEN1_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_CAPTEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_CAPTEN1;
	tmp |= value << TC_CTRLA_CAPTEN1_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_CAPTEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_CAPTEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_CAPTEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_CAPTEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_COPEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_COPEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_COPEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_COPEN0) >> TC_CTRLA_COPEN0_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_COPEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_COPEN0;
	tmp |= value << TC_CTRLA_COPEN0_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_COPEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_COPEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_COPEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_COPEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_COPEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_COPEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_CTRLA_COPEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_COPEN1) >> TC_CTRLA_COPEN1_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_CTRLA_COPEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_COPEN1;
	tmp |= value << TC_CTRLA_COPEN1_Pos;
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_COPEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_COPEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_COPEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_COPEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_CTRLA_MODE_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_MODE(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_MODE_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_MODE(mask)) >> TC_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLA_MODE_bf(const void *const hw, hri_tc_ctrla_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_MODE_Msk;
	tmp |= TC_CTRLA_MODE(data);
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_MODE_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_MODE(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_MODE_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_MODE(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_MODE_Msk) >> TC_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_tc_set_CTRLA_PRESCSYNC_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_PRESCSYNC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_PRESCSYNC_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_PRESCSYNC(mask)) >> TC_CTRLA_PRESCSYNC_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLA_PRESCSYNC_bf(const void *const hw, hri_tc_ctrla_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_PRESCSYNC_Msk;
	tmp |= TC_CTRLA_PRESCSYNC(data);
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_PRESCSYNC_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_PRESCSYNC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_PRESCSYNC_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_PRESCSYNC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_PRESCSYNC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_PRESCSYNC_Msk) >> TC_CTRLA_PRESCSYNC_Pos;
	return tmp;
}

static inline void hri_tc_set_CTRLA_PRESCALER_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_PRESCALER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_PRESCALER_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_PRESCALER(mask)) >> TC_CTRLA_PRESCALER_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLA_PRESCALER_bf(const void *const hw, hri_tc_ctrla_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_PRESCALER_Msk;
	tmp |= TC_CTRLA_PRESCALER(data);
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_PRESCALER_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_PRESCALER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_PRESCALER_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_PRESCALER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_PRESCALER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_PRESCALER_Msk) >> TC_CTRLA_PRESCALER_Pos;
	return tmp;
}

static inline void hri_tc_set_CTRLA_CAPTMODE0_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_CAPTMODE0(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_CAPTMODE0_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTMODE0(mask)) >> TC_CTRLA_CAPTMODE0_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLA_CAPTMODE0_bf(const void *const hw, hri_tc_ctrla_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_CAPTMODE0_Msk;
	tmp |= TC_CTRLA_CAPTMODE0(data);
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_CAPTMODE0_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_CAPTMODE0(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_CAPTMODE0_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_CAPTMODE0(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_CAPTMODE0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTMODE0_Msk) >> TC_CTRLA_CAPTMODE0_Pos;
	return tmp;
}

static inline void hri_tc_set_CTRLA_CAPTMODE1_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg |= TC_CTRLA_CAPTMODE1(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_CAPTMODE1_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTMODE1(mask)) >> TC_CTRLA_CAPTMODE1_Pos;
	return tmp;
}

static inline void hri_tc_write_CTRLA_CAPTMODE1_bf(const void *const hw, hri_tc_ctrla_reg_t data)
{
	uint32_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= ~TC_CTRLA_CAPTMODE1_Msk;
	tmp |= TC_CTRLA_CAPTMODE1(data);
	((Tc *)hw)->COUNT8.CTRLA.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_CAPTMODE1_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~TC_CTRLA_CAPTMODE1(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_CAPTMODE1_bf(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CTRLA.reg ^= TC_CTRLA_CAPTMODE1(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_CAPTMODE1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp = (tmp & TC_CTRLA_CAPTMODE1_Msk) >> TC_CTRLA_CAPTMODE1_Pos;
	return tmp;
}

static inline void hri_tc_set_CTRLA_reg(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CTRLA.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_get_CTRLA_reg(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_CTRLA_reg(const void *const hw, hri_tc_ctrla_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CTRLA.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_CTRLA_reg(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CTRLA.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_CTRLA_reg(const void *const hw, hri_tc_ctrla_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CTRLA.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ctrla_reg_t hri_tc_read_CTRLA_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.CTRLA.reg;
}

static inline void hri_tc_set_EVCTRL_TCINV_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_TCINV;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_EVCTRL_TCINV_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_TCINV) >> TC_EVCTRL_TCINV_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_EVCTRL_TCINV_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_TCINV;
	tmp |= value << TC_EVCTRL_TCINV_Pos;
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_TCINV_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_TCINV;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_TCINV_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_TCINV;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_EVCTRL_TCEI_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_TCEI;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_EVCTRL_TCEI_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_TCEI) >> TC_EVCTRL_TCEI_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_EVCTRL_TCEI_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_TCEI;
	tmp |= value << TC_EVCTRL_TCEI_Pos;
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_TCEI_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_TCEI;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_TCEI_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_TCEI;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_EVCTRL_OVFEO_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_OVFEO;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_EVCTRL_OVFEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_OVFEO) >> TC_EVCTRL_OVFEO_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_EVCTRL_OVFEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_OVFEO;
	tmp |= value << TC_EVCTRL_OVFEO_Pos;
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_OVFEO_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_OVFEO;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_OVFEO_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_OVFEO;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_EVCTRL_MCEO0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_MCEO0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_EVCTRL_MCEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_MCEO0) >> TC_EVCTRL_MCEO0_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_EVCTRL_MCEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_MCEO0;
	tmp |= value << TC_EVCTRL_MCEO0_Pos;
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_MCEO0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_MCEO0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_MCEO0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_MCEO0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_EVCTRL_MCEO1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_MCEO1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_EVCTRL_MCEO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_MCEO1) >> TC_EVCTRL_MCEO1_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_EVCTRL_MCEO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_MCEO1;
	tmp |= value << TC_EVCTRL_MCEO1_Pos;
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_MCEO1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_MCEO1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_MCEO1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_MCEO1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_EVCTRL_EVACT_bf(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= TC_EVCTRL_EVACT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_evctrl_reg_t hri_tc_get_EVCTRL_EVACT_bf(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_EVACT(mask)) >> TC_EVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_tc_write_EVCTRL_EVACT_bf(const void *const hw, hri_tc_evctrl_reg_t data)
{
	uint16_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= ~TC_EVCTRL_EVACT_Msk;
	tmp |= TC_EVCTRL_EVACT(data);
	((Tc *)hw)->COUNT8.EVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_EVACT_bf(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~TC_EVCTRL_EVACT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_EVACT_bf(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= TC_EVCTRL_EVACT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_evctrl_reg_t hri_tc_read_EVCTRL_EVACT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp = (tmp & TC_EVCTRL_EVACT_Msk) >> TC_EVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_tc_set_EVCTRL_reg(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_evctrl_reg_t hri_tc_get_EVCTRL_reg(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tc *)hw)->COUNT8.EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_EVCTRL_reg(const void *const hw, hri_tc_evctrl_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_EVCTRL_reg(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_EVCTRL_reg(const void *const hw, hri_tc_evctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.EVCTRL.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_evctrl_reg_t hri_tc_read_EVCTRL_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.EVCTRL.reg;
}

static inline void hri_tc_set_WAVE_WAVEGEN_bf(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg |= TC_WAVE_WAVEGEN(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_wave_reg_t hri_tc_get_WAVE_WAVEGEN_bf(const void *const hw, hri_tc_wave_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.WAVE.reg;
	tmp = (tmp & TC_WAVE_WAVEGEN(mask)) >> TC_WAVE_WAVEGEN_Pos;
	return tmp;
}

static inline void hri_tc_write_WAVE_WAVEGEN_bf(const void *const hw, hri_tc_wave_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.WAVE.reg;
	tmp &= ~TC_WAVE_WAVEGEN_Msk;
	tmp |= TC_WAVE_WAVEGEN(data);
	((Tc *)hw)->COUNT8.WAVE.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_WAVE_WAVEGEN_bf(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg &= ~TC_WAVE_WAVEGEN(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_WAVE_WAVEGEN_bf(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg ^= TC_WAVE_WAVEGEN(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_wave_reg_t hri_tc_read_WAVE_WAVEGEN_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.WAVE.reg;
	tmp = (tmp & TC_WAVE_WAVEGEN_Msk) >> TC_WAVE_WAVEGEN_Pos;
	return tmp;
}

static inline void hri_tc_set_WAVE_reg(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_wave_reg_t hri_tc_get_WAVE_reg(const void *const hw, hri_tc_wave_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.WAVE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_WAVE_reg(const void *const hw, hri_tc_wave_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_WAVE_reg(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_WAVE_reg(const void *const hw, hri_tc_wave_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.WAVE.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_wave_reg_t hri_tc_read_WAVE_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.WAVE.reg;
}

static inline void hri_tc_set_DRVCTRL_INVEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg |= TC_DRVCTRL_INVEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_DRVCTRL_INVEN0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.DRVCTRL.reg;
	tmp = (tmp & TC_DRVCTRL_INVEN0) >> TC_DRVCTRL_INVEN0_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_DRVCTRL_INVEN0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.DRVCTRL.reg;
	tmp &= ~TC_DRVCTRL_INVEN0;
	tmp |= value << TC_DRVCTRL_INVEN0_Pos;
	((Tc *)hw)->COUNT8.DRVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_DRVCTRL_INVEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg &= ~TC_DRVCTRL_INVEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_DRVCTRL_INVEN0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg ^= TC_DRVCTRL_INVEN0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_DRVCTRL_INVEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg |= TC_DRVCTRL_INVEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_DRVCTRL_INVEN1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.DRVCTRL.reg;
	tmp = (tmp & TC_DRVCTRL_INVEN1) >> TC_DRVCTRL_INVEN1_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_DRVCTRL_INVEN1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.DRVCTRL.reg;
	tmp &= ~TC_DRVCTRL_INVEN1;
	tmp |= value << TC_DRVCTRL_INVEN1_Pos;
	((Tc *)hw)->COUNT8.DRVCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_DRVCTRL_INVEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg &= ~TC_DRVCTRL_INVEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_DRVCTRL_INVEN1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg ^= TC_DRVCTRL_INVEN1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_DRVCTRL_reg(const void *const hw, hri_tc_drvctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_drvctrl_reg_t hri_tc_get_DRVCTRL_reg(const void *const hw, hri_tc_drvctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.DRVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_DRVCTRL_reg(const void *const hw, hri_tc_drvctrl_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_DRVCTRL_reg(const void *const hw, hri_tc_drvctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_DRVCTRL_reg(const void *const hw, hri_tc_drvctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DRVCTRL.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_drvctrl_reg_t hri_tc_read_DRVCTRL_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.DRVCTRL.reg;
}

static inline void hri_tc_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg |= TC_DBGCTRL_DBGRUN;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.DBGCTRL.reg;
	tmp = (tmp & TC_DBGCTRL_DBGRUN) >> TC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_tc_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	tmp = ((Tc *)hw)->COUNT8.DBGCTRL.reg;
	tmp &= ~TC_DBGCTRL_DBGRUN;
	tmp |= value << TC_DBGCTRL_DBGRUN_Pos;
	((Tc *)hw)->COUNT8.DBGCTRL.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg &= ~TC_DBGCTRL_DBGRUN;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg ^= TC_DBGCTRL_DBGRUN;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_set_DBGCTRL_reg(const void *const hw, hri_tc_dbgctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_dbgctrl_reg_t hri_tc_get_DBGCTRL_reg(const void *const hw, hri_tc_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_DBGCTRL_reg(const void *const hw, hri_tc_dbgctrl_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_DBGCTRL_reg(const void *const hw, hri_tc_dbgctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_DBGCTRL_reg(const void *const hw, hri_tc_dbgctrl_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.DBGCTRL.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_dbgctrl_reg_t hri_tc_read_DBGCTRL_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.DBGCTRL.reg;
}

static inline void hri_tccount8_set_COUNT_COUNT_bf(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	((Tc *)hw)->COUNT8.COUNT.reg |= TC_COUNT8_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count8_reg_t hri_tccount8_get_COUNT_COUNT_bf(const void *const hw, hri_tc_count8_reg_t mask)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	tmp = ((Tc *)hw)->COUNT8.COUNT.reg;
	tmp = (tmp & TC_COUNT8_COUNT_COUNT(mask)) >> TC_COUNT8_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount8_write_COUNT_COUNT_bf(const void *const hw, hri_tc_count8_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	tmp = ((Tc *)hw)->COUNT8.COUNT.reg;
	tmp &= ~TC_COUNT8_COUNT_COUNT_Msk;
	tmp |= TC_COUNT8_COUNT_COUNT(data);
	((Tc *)hw)->COUNT8.COUNT.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_COUNT_COUNT_bf(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	((Tc *)hw)->COUNT8.COUNT.reg &= ~TC_COUNT8_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_COUNT_COUNT_bf(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	((Tc *)hw)->COUNT8.COUNT.reg ^= TC_COUNT8_COUNT_COUNT(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count8_reg_t hri_tccount8_read_COUNT_COUNT_bf(const void *const hw)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_COUNT);
	tmp = ((Tc *)hw)->COUNT8.COUNT.reg;
	tmp = (tmp & TC_COUNT8_COUNT_COUNT_Msk) >> TC_COUNT8_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_tccount8_set_COUNT_reg(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.COUNT.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count8_reg_t hri_tccount8_get_COUNT_reg(const void *const hw, hri_tc_count8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount8_write_COUNT_reg(const void *const hw, hri_tc_count8_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.COUNT.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_COUNT_reg(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.COUNT.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_COUNT_reg(const void *const hw, hri_tc_count8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.COUNT.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_count8_reg_t hri_tccount8_read_COUNT_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.COUNT.reg;
}

static inline void hri_tc_set_PER_PER_bf(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	((Tc *)hw)->COUNT8.PER.reg |= TC_COUNT8_PER_PER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_per_reg_t hri_tc_get_PER_PER_bf(const void *const hw, hri_tc_per_reg_t mask)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	tmp = ((Tc *)hw)->COUNT8.PER.reg;
	tmp = (tmp & TC_COUNT8_PER_PER(mask)) >> TC_COUNT8_PER_PER_Pos;
	return tmp;
}

static inline void hri_tc_write_PER_PER_bf(const void *const hw, hri_tc_per_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	tmp = ((Tc *)hw)->COUNT8.PER.reg;
	tmp &= ~TC_COUNT8_PER_PER_Msk;
	tmp |= TC_COUNT8_PER_PER(data);
	((Tc *)hw)->COUNT8.PER.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_PER_PER_bf(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	((Tc *)hw)->COUNT8.PER.reg &= ~TC_COUNT8_PER_PER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_PER_PER_bf(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	((Tc *)hw)->COUNT8.PER.reg ^= TC_COUNT8_PER_PER(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_per_reg_t hri_tc_read_PER_PER_bf(const void *const hw)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_PER);
	tmp = ((Tc *)hw)->COUNT8.PER.reg;
	tmp = (tmp & TC_COUNT8_PER_PER_Msk) >> TC_COUNT8_PER_PER_Pos;
	return tmp;
}

static inline void hri_tc_set_PER_reg(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PER.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_per_reg_t hri_tc_get_PER_reg(const void *const hw, hri_tc_per_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.PER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_PER_reg(const void *const hw, hri_tc_per_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PER.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_PER_reg(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PER.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_PER_reg(const void *const hw, hri_tc_per_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PER.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_per_reg_t hri_tc_read_PER_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.PER.reg;
}

static inline void hri_tccount8_set_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CC[index].reg |= TC_COUNT8_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc8_reg_t hri_tccount8_get_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CC[index].reg;
	tmp = (tmp & TC_COUNT8_CC_CC(mask)) >> TC_COUNT8_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount8_write_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc8_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CC[index].reg;
	tmp &= ~TC_COUNT8_CC_CC_Msk;
	tmp |= TC_COUNT8_CC_CC(data);
	((Tc *)hw)->COUNT8.CC[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CC[index].reg &= ~TC_COUNT8_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_CC_CC_bf(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CC[index].reg ^= TC_COUNT8_CC_CC(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc8_reg_t hri_tccount8_read_CC_CC_bf(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CC[index].reg;
	tmp = (tmp & TC_COUNT8_CC_CC_Msk) >> TC_COUNT8_CC_CC_Pos;
	return tmp;
}

static inline void hri_tccount8_set_CC_reg(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CC[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc8_reg_t hri_tccount8_get_CC_reg(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CC[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount8_write_CC_reg(const void *const hw, uint8_t index, hri_tc_cc8_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CC[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_CC_reg(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CC[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_CC_reg(const void *const hw, uint8_t index, hri_tc_cc8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CC[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_cc8_reg_t hri_tccount8_read_CC_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT8.CC[index].reg;
}

static inline void hri_tc_set_PERBUF_PERBUF_bf(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.PERBUF.reg |= TC_COUNT8_PERBUF_PERBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_perbuf_reg_t hri_tc_get_PERBUF_PERBUF_bf(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.PERBUF.reg;
	tmp = (tmp & TC_COUNT8_PERBUF_PERBUF(mask)) >> TC_COUNT8_PERBUF_PERBUF_Pos;
	return tmp;
}

static inline void hri_tc_write_PERBUF_PERBUF_bf(const void *const hw, hri_tc_perbuf_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.PERBUF.reg;
	tmp &= ~TC_COUNT8_PERBUF_PERBUF_Msk;
	tmp |= TC_COUNT8_PERBUF_PERBUF(data);
	((Tc *)hw)->COUNT8.PERBUF.reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_PERBUF_PERBUF_bf(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.PERBUF.reg &= ~TC_COUNT8_PERBUF_PERBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_PERBUF_PERBUF_bf(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.PERBUF.reg ^= TC_COUNT8_PERBUF_PERBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_perbuf_reg_t hri_tc_read_PERBUF_PERBUF_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.PERBUF.reg;
	tmp = (tmp & TC_COUNT8_PERBUF_PERBUF_Msk) >> TC_COUNT8_PERBUF_PERBUF_Pos;
	return tmp;
}

static inline void hri_tc_set_PERBUF_reg(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PERBUF.reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_perbuf_reg_t hri_tc_get_PERBUF_reg(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.PERBUF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_write_PERBUF_reg(const void *const hw, hri_tc_perbuf_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PERBUF.reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_clear_PERBUF_reg(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PERBUF.reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tc_toggle_PERBUF_reg(const void *const hw, hri_tc_perbuf_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.PERBUF.reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_perbuf_reg_t hri_tc_read_PERBUF_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.PERBUF.reg;
}

static inline void hri_tccount8_set_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CCBUF[index].reg |= TC_COUNT8_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf8_reg_t hri_tccount8_get_CCBUF_CCBUF_bf(const void *const hw, uint8_t index,
                                                                  hri_tc_ccbuf8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT8_CCBUF_CCBUF(mask)) >> TC_COUNT8_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount8_write_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t data)
{
	uint8_t tmp;
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	tmp = ((Tc *)hw)->COUNT8.CCBUF[index].reg;
	tmp &= ~TC_COUNT8_CCBUF_CCBUF_Msk;
	tmp |= TC_COUNT8_CCBUF_CCBUF(data);
	((Tc *)hw)->COUNT8.CCBUF[index].reg = tmp;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CCBUF[index].reg &= ~TC_COUNT8_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_CCBUF_CCBUF_bf(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.CCBUF[index].reg ^= TC_COUNT8_CCBUF_CCBUF(mask);
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf8_reg_t hri_tccount8_read_CCBUF_CCBUF_bf(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CCBUF[index].reg;
	tmp = (tmp & TC_COUNT8_CCBUF_CCBUF_Msk) >> TC_COUNT8_CCBUF_CCBUF_Pos;
	return tmp;
}

static inline void hri_tccount8_set_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CCBUF[index].reg |= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf8_reg_t hri_tccount8_get_CCBUF_reg(const void *const hw, uint8_t index,
                                                             hri_tc_ccbuf8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.CCBUF[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tccount8_write_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t data)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CCBUF[index].reg = data;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_clear_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CCBUF[index].reg &= ~mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tccount8_toggle_CCBUF_reg(const void *const hw, uint8_t index, hri_tc_ccbuf8_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.CCBUF[index].reg ^= mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_ccbuf8_reg_t hri_tccount8_read_CCBUF_reg(const void *const hw, uint8_t index)
{
	return ((Tc *)hw)->COUNT8.CCBUF[index].reg;
}

static inline bool hri_tc_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_SWRST) >> TC_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_ENABLE) >> TC_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_CTRLB_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_CTRLB) >> TC_SYNCBUSY_CTRLB_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_STATUS_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_STATUS) >> TC_SYNCBUSY_STATUS_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_COUNT_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_COUNT) >> TC_SYNCBUSY_COUNT_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_PER_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_PER) >> TC_SYNCBUSY_PER_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_CC0_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_CC0) >> TC_SYNCBUSY_CC0_Pos;
}

static inline bool hri_tc_get_SYNCBUSY_CC1_bit(const void *const hw)
{
	return (((Tc *)hw)->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_CC1) >> TC_SYNCBUSY_CC1_Pos;
}

static inline hri_tc_syncbusy_reg_t hri_tc_get_SYNCBUSY_reg(const void *const hw, hri_tc_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tc *)hw)->COUNT8.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tc_syncbusy_reg_t hri_tc_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.SYNCBUSY.reg;
}

static inline bool hri_tc_get_STATUS_STOP_bit(const void *const hw)
{
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	return (((Tc *)hw)->COUNT8.STATUS.reg & TC_STATUS_STOP) >> TC_STATUS_STOP_Pos;
}

static inline void hri_tc_clear_STATUS_STOP_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.STATUS.reg = TC_STATUS_STOP;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_STATUS_SLAVE_bit(const void *const hw)
{
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	return (((Tc *)hw)->COUNT8.STATUS.reg & TC_STATUS_SLAVE) >> TC_STATUS_SLAVE_Pos;
}

static inline void hri_tc_clear_STATUS_SLAVE_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.STATUS.reg = TC_STATUS_SLAVE;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_STATUS_PERBUFV_bit(const void *const hw)
{
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	return (((Tc *)hw)->COUNT8.STATUS.reg & TC_STATUS_PERBUFV) >> TC_STATUS_PERBUFV_Pos;
}

static inline void hri_tc_clear_STATUS_PERBUFV_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.STATUS.reg = TC_STATUS_PERBUFV;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_STATUS_CCBUFV0_bit(const void *const hw)
{
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	return (((Tc *)hw)->COUNT8.STATUS.reg & TC_STATUS_CCBUFV0) >> TC_STATUS_CCBUFV0_Pos;
}

static inline void hri_tc_clear_STATUS_CCBUFV0_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.STATUS.reg = TC_STATUS_CCBUFV0;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tc_get_STATUS_CCBUFV1_bit(const void *const hw)
{
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	return (((Tc *)hw)->COUNT8.STATUS.reg & TC_STATUS_CCBUFV1) >> TC_STATUS_CCBUFV1_Pos;
}

static inline void hri_tc_clear_STATUS_CCBUFV1_bit(const void *const hw)
{
	TC_CRITICAL_SECTION_ENTER();
	hri_tc_wait_for_sync(hw, TC_SYNCBUSY_MASK);
	((Tc *)hw)->COUNT8.STATUS.reg = TC_STATUS_CCBUFV1;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_status_reg_t hri_tc_get_STATUS_reg(const void *const hw, hri_tc_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tc *)hw)->COUNT8.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tc_clear_STATUS_reg(const void *const hw, hri_tc_status_reg_t mask)
{
	TC_CRITICAL_SECTION_ENTER();
	((Tc *)hw)->COUNT8.STATUS.reg = mask;
	TC_CRITICAL_SECTION_LEAVE();
}

static inline hri_tc_status_reg_t hri_tc_read_STATUS_reg(const void *const hw)
{
	return ((Tc *)hw)->COUNT8.STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_TC_D51_H_INCLUDED */
#endif /* _SAMD51_TC_COMPONENT_ */
