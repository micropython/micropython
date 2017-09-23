/**
 * \file
 *
 * \brief SAM DAC
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

#ifdef _SAMD51_DAC_COMPONENT_
#ifndef _HRI_DAC_D51_H_INCLUDED_
#define _HRI_DAC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_DAC_CRITICAL_SECTIONS)
#define DAC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define DAC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define DAC_CRITICAL_SECTION_ENTER()
#define DAC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_dac_dacctrl_reg_t;
typedef uint16_t hri_dac_data_reg_t;
typedef uint16_t hri_dac_databuf_reg_t;
typedef uint16_t hri_dac_result_reg_t;
typedef uint32_t hri_dac_syncbusy_reg_t;
typedef uint8_t  hri_dac_ctrla_reg_t;
typedef uint8_t  hri_dac_ctrlb_reg_t;
typedef uint8_t  hri_dac_dbgctrl_reg_t;
typedef uint8_t  hri_dac_evctrl_reg_t;
typedef uint8_t  hri_dac_intenset_reg_t;
typedef uint8_t  hri_dac_intflag_reg_t;
typedef uint8_t  hri_dac_status_reg_t;

static inline void hri_dac_wait_for_sync(const void *const hw, hri_dac_syncbusy_reg_t reg)
{
	while (((Dac *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_dac_is_syncing(const void *const hw, hri_dac_syncbusy_reg_t reg)
{
	return ((Dac *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_dac_set_INTEN_UNDERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_UNDERRUN0;
}

static inline bool hri_dac_get_INTEN_UNDERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_UNDERRUN0) >> DAC_INTENSET_UNDERRUN0_Pos;
}

static inline void hri_dac_write_INTEN_UNDERRUN0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_UNDERRUN0;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_UNDERRUN0;
	}
}

static inline void hri_dac_clear_INTEN_UNDERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_UNDERRUN0;
}

static inline void hri_dac_set_INTEN_UNDERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_UNDERRUN1;
}

static inline bool hri_dac_get_INTEN_UNDERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_UNDERRUN1) >> DAC_INTENSET_UNDERRUN1_Pos;
}

static inline void hri_dac_write_INTEN_UNDERRUN1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_UNDERRUN1;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_UNDERRUN1;
	}
}

static inline void hri_dac_clear_INTEN_UNDERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_UNDERRUN1;
}

static inline void hri_dac_set_INTEN_EMPTY0_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_EMPTY0;
}

static inline bool hri_dac_get_INTEN_EMPTY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_EMPTY0) >> DAC_INTENSET_EMPTY0_Pos;
}

static inline void hri_dac_write_INTEN_EMPTY0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_EMPTY0;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_EMPTY0;
	}
}

static inline void hri_dac_clear_INTEN_EMPTY0_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_EMPTY0;
}

static inline void hri_dac_set_INTEN_EMPTY1_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_EMPTY1;
}

static inline bool hri_dac_get_INTEN_EMPTY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_EMPTY1) >> DAC_INTENSET_EMPTY1_Pos;
}

static inline void hri_dac_write_INTEN_EMPTY1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_EMPTY1;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_EMPTY1;
	}
}

static inline void hri_dac_clear_INTEN_EMPTY1_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_EMPTY1;
}

static inline void hri_dac_set_INTEN_RESRDY0_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_RESRDY0;
}

static inline bool hri_dac_get_INTEN_RESRDY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_RESRDY0) >> DAC_INTENSET_RESRDY0_Pos;
}

static inline void hri_dac_write_INTEN_RESRDY0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_RESRDY0;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_RESRDY0;
	}
}

static inline void hri_dac_clear_INTEN_RESRDY0_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_RESRDY0;
}

static inline void hri_dac_set_INTEN_RESRDY1_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_RESRDY1;
}

static inline bool hri_dac_get_INTEN_RESRDY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_RESRDY1) >> DAC_INTENSET_RESRDY1_Pos;
}

static inline void hri_dac_write_INTEN_RESRDY1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_RESRDY1;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_RESRDY1;
	}
}

static inline void hri_dac_clear_INTEN_RESRDY1_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_RESRDY1;
}

static inline void hri_dac_set_INTEN_OVERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_OVERRUN0;
}

static inline bool hri_dac_get_INTEN_OVERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_OVERRUN0) >> DAC_INTENSET_OVERRUN0_Pos;
}

static inline void hri_dac_write_INTEN_OVERRUN0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_OVERRUN0;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_OVERRUN0;
	}
}

static inline void hri_dac_clear_INTEN_OVERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_OVERRUN0;
}

static inline void hri_dac_set_INTEN_OVERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTENSET.reg = DAC_INTENSET_OVERRUN1;
}

static inline bool hri_dac_get_INTEN_OVERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTENSET.reg & DAC_INTENSET_OVERRUN1) >> DAC_INTENSET_OVERRUN1_Pos;
}

static inline void hri_dac_write_INTEN_OVERRUN1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_OVERRUN1;
	} else {
		((Dac *)hw)->INTENSET.reg = DAC_INTENSET_OVERRUN1;
	}
}

static inline void hri_dac_clear_INTEN_OVERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTENCLR.reg = DAC_INTENSET_OVERRUN1;
}

static inline void hri_dac_set_INTEN_reg(const void *const hw, hri_dac_intenset_reg_t mask)
{
	((Dac *)hw)->INTENSET.reg = mask;
}

static inline hri_dac_intenset_reg_t hri_dac_get_INTEN_reg(const void *const hw, hri_dac_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dac_intenset_reg_t hri_dac_read_INTEN_reg(const void *const hw)
{
	return ((Dac *)hw)->INTENSET.reg;
}

static inline void hri_dac_write_INTEN_reg(const void *const hw, hri_dac_intenset_reg_t data)
{
	((Dac *)hw)->INTENSET.reg = data;
	((Dac *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_dac_clear_INTEN_reg(const void *const hw, hri_dac_intenset_reg_t mask)
{
	((Dac *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_dac_get_INTFLAG_UNDERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_UNDERRUN0) >> DAC_INTFLAG_UNDERRUN0_Pos;
}

static inline void hri_dac_clear_INTFLAG_UNDERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_UNDERRUN0;
}

static inline bool hri_dac_get_INTFLAG_UNDERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_UNDERRUN1) >> DAC_INTFLAG_UNDERRUN1_Pos;
}

static inline void hri_dac_clear_INTFLAG_UNDERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_UNDERRUN1;
}

static inline bool hri_dac_get_INTFLAG_EMPTY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_EMPTY0) >> DAC_INTFLAG_EMPTY0_Pos;
}

static inline void hri_dac_clear_INTFLAG_EMPTY0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_EMPTY0;
}

static inline bool hri_dac_get_INTFLAG_EMPTY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_EMPTY1) >> DAC_INTFLAG_EMPTY1_Pos;
}

static inline void hri_dac_clear_INTFLAG_EMPTY1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_EMPTY1;
}

static inline bool hri_dac_get_INTFLAG_RESRDY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_RESRDY0) >> DAC_INTFLAG_RESRDY0_Pos;
}

static inline void hri_dac_clear_INTFLAG_RESRDY0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_RESRDY0;
}

static inline bool hri_dac_get_INTFLAG_RESRDY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_RESRDY1) >> DAC_INTFLAG_RESRDY1_Pos;
}

static inline void hri_dac_clear_INTFLAG_RESRDY1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_RESRDY1;
}

static inline bool hri_dac_get_INTFLAG_OVERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_OVERRUN0) >> DAC_INTFLAG_OVERRUN0_Pos;
}

static inline void hri_dac_clear_INTFLAG_OVERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_OVERRUN0;
}

static inline bool hri_dac_get_INTFLAG_OVERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_OVERRUN1) >> DAC_INTFLAG_OVERRUN1_Pos;
}

static inline void hri_dac_clear_INTFLAG_OVERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_OVERRUN1;
}

static inline bool hri_dac_get_interrupt_UNDERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_UNDERRUN0) >> DAC_INTFLAG_UNDERRUN0_Pos;
}

static inline void hri_dac_clear_interrupt_UNDERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_UNDERRUN0;
}

static inline bool hri_dac_get_interrupt_UNDERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_UNDERRUN1) >> DAC_INTFLAG_UNDERRUN1_Pos;
}

static inline void hri_dac_clear_interrupt_UNDERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_UNDERRUN1;
}

static inline bool hri_dac_get_interrupt_EMPTY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_EMPTY0) >> DAC_INTFLAG_EMPTY0_Pos;
}

static inline void hri_dac_clear_interrupt_EMPTY0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_EMPTY0;
}

static inline bool hri_dac_get_interrupt_EMPTY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_EMPTY1) >> DAC_INTFLAG_EMPTY1_Pos;
}

static inline void hri_dac_clear_interrupt_EMPTY1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_EMPTY1;
}

static inline bool hri_dac_get_interrupt_RESRDY0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_RESRDY0) >> DAC_INTFLAG_RESRDY0_Pos;
}

static inline void hri_dac_clear_interrupt_RESRDY0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_RESRDY0;
}

static inline bool hri_dac_get_interrupt_RESRDY1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_RESRDY1) >> DAC_INTFLAG_RESRDY1_Pos;
}

static inline void hri_dac_clear_interrupt_RESRDY1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_RESRDY1;
}

static inline bool hri_dac_get_interrupt_OVERRUN0_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_OVERRUN0) >> DAC_INTFLAG_OVERRUN0_Pos;
}

static inline void hri_dac_clear_interrupt_OVERRUN0_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_OVERRUN0;
}

static inline bool hri_dac_get_interrupt_OVERRUN1_bit(const void *const hw)
{
	return (((Dac *)hw)->INTFLAG.reg & DAC_INTFLAG_OVERRUN1) >> DAC_INTFLAG_OVERRUN1_Pos;
}

static inline void hri_dac_clear_interrupt_OVERRUN1_bit(const void *const hw)
{
	((Dac *)hw)->INTFLAG.reg = DAC_INTFLAG_OVERRUN1;
}

static inline hri_dac_intflag_reg_t hri_dac_get_INTFLAG_reg(const void *const hw, hri_dac_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dac_intflag_reg_t hri_dac_read_INTFLAG_reg(const void *const hw)
{
	return ((Dac *)hw)->INTFLAG.reg;
}

static inline void hri_dac_clear_INTFLAG_reg(const void *const hw, hri_dac_intflag_reg_t mask)
{
	((Dac *)hw)->INTFLAG.reg = mask;
}

static inline void hri_dac_write_DATA_reg(const void *const hw, uint8_t index, hri_dac_data_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DATA[index].reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_write_DATABUF_reg(const void *const hw, uint8_t index, hri_dac_databuf_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DATABUF[index].reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_CTRLA_SWRST_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST);
	((Dac *)hw)->CTRLA.reg |= DAC_CTRLA_SWRST;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST);
	tmp = ((Dac *)hw)->CTRLA.reg;
	tmp = (tmp & DAC_CTRLA_SWRST) >> DAC_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_dac_set_CTRLA_ENABLE_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST | DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->CTRLA.reg |= DAC_CTRLA_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST | DAC_SYNCBUSY_ENABLE);
	tmp = ((Dac *)hw)->CTRLA.reg;
	tmp = (tmp & DAC_CTRLA_ENABLE) >> DAC_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST | DAC_SYNCBUSY_ENABLE);
	tmp = ((Dac *)hw)->CTRLA.reg;
	tmp &= ~DAC_CTRLA_ENABLE;
	tmp |= value << DAC_CTRLA_ENABLE_Pos;
	((Dac *)hw)->CTRLA.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST | DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->CTRLA.reg &= ~DAC_CTRLA_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST | DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->CTRLA.reg ^= DAC_CTRLA_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_CTRLA_reg(const void *const hw, hri_dac_ctrla_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLA.reg |= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrla_reg_t hri_dac_get_CTRLA_reg(const void *const hw, hri_dac_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dac_write_CTRLA_reg(const void *const hw, hri_dac_ctrla_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLA.reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_CTRLA_reg(const void *const hw, hri_dac_ctrla_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLA.reg &= ~mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_CTRLA_reg(const void *const hw, hri_dac_ctrla_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLA.reg ^= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrla_reg_t hri_dac_read_CTRLA_reg(const void *const hw)
{
	return ((Dac *)hw)->CTRLA.reg;
}

static inline void hri_dac_set_CTRLB_DIFF_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg |= DAC_CTRLB_DIFF;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_CTRLB_DIFF_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp = (tmp & DAC_CTRLB_DIFF) >> DAC_CTRLB_DIFF_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_CTRLB_DIFF_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp &= ~DAC_CTRLB_DIFF;
	tmp |= value << DAC_CTRLB_DIFF_Pos;
	((Dac *)hw)->CTRLB.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_CTRLB_DIFF_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg &= ~DAC_CTRLB_DIFF;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_CTRLB_DIFF_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg ^= DAC_CTRLB_DIFF;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_CTRLB_REFSEL_bf(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg |= DAC_CTRLB_REFSEL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrlb_reg_t hri_dac_get_CTRLB_REFSEL_bf(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp = (tmp & DAC_CTRLB_REFSEL(mask)) >> DAC_CTRLB_REFSEL_Pos;
	return tmp;
}

static inline void hri_dac_write_CTRLB_REFSEL_bf(const void *const hw, hri_dac_ctrlb_reg_t data)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp &= ~DAC_CTRLB_REFSEL_Msk;
	tmp |= DAC_CTRLB_REFSEL(data);
	((Dac *)hw)->CTRLB.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_CTRLB_REFSEL_bf(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg &= ~DAC_CTRLB_REFSEL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_CTRLB_REFSEL_bf(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg ^= DAC_CTRLB_REFSEL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrlb_reg_t hri_dac_read_CTRLB_REFSEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp = (tmp & DAC_CTRLB_REFSEL_Msk) >> DAC_CTRLB_REFSEL_Pos;
	return tmp;
}

static inline void hri_dac_set_CTRLB_reg(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg |= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrlb_reg_t hri_dac_get_CTRLB_reg(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dac_write_CTRLB_reg(const void *const hw, hri_dac_ctrlb_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_CTRLB_reg(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg &= ~mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_CTRLB_reg(const void *const hw, hri_dac_ctrlb_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->CTRLB.reg ^= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_ctrlb_reg_t hri_dac_read_CTRLB_reg(const void *const hw)
{
	return ((Dac *)hw)->CTRLB.reg;
}

static inline void hri_dac_set_EVCTRL_STARTEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_STARTEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_STARTEI0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_STARTEI0) >> DAC_EVCTRL_STARTEI0_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_STARTEI0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_STARTEI0;
	tmp |= value << DAC_EVCTRL_STARTEI0_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_STARTEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_STARTEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_STARTEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_STARTEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_STARTEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_STARTEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_STARTEI1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_STARTEI1) >> DAC_EVCTRL_STARTEI1_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_STARTEI1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_STARTEI1;
	tmp |= value << DAC_EVCTRL_STARTEI1_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_STARTEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_STARTEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_STARTEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_STARTEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_EMPTYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_EMPTYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_EMPTYEO0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_EMPTYEO0) >> DAC_EVCTRL_EMPTYEO0_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_EMPTYEO0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_EMPTYEO0;
	tmp |= value << DAC_EVCTRL_EMPTYEO0_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_EMPTYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_EMPTYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_EMPTYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_EMPTYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_EMPTYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_EMPTYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_EMPTYEO1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_EMPTYEO1) >> DAC_EVCTRL_EMPTYEO1_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_EMPTYEO1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_EMPTYEO1;
	tmp |= value << DAC_EVCTRL_EMPTYEO1_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_EMPTYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_EMPTYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_EMPTYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_EMPTYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_INVEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_INVEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_INVEI0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_INVEI0) >> DAC_EVCTRL_INVEI0_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_INVEI0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_INVEI0;
	tmp |= value << DAC_EVCTRL_INVEI0_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_INVEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_INVEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_INVEI0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_INVEI0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_INVEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_INVEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_INVEI1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_INVEI1) >> DAC_EVCTRL_INVEI1_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_INVEI1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_INVEI1;
	tmp |= value << DAC_EVCTRL_INVEI1_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_INVEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_INVEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_INVEI1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_INVEI1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_RESRDYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_RESRDYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_RESRDYEO0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_RESRDYEO0) >> DAC_EVCTRL_RESRDYEO0_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_RESRDYEO0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_RESRDYEO0;
	tmp |= value << DAC_EVCTRL_RESRDYEO0_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_RESRDYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_RESRDYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_RESRDYEO0_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_RESRDYEO0;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_RESRDYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= DAC_EVCTRL_RESRDYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_EVCTRL_RESRDYEO1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp = (tmp & DAC_EVCTRL_RESRDYEO1) >> DAC_EVCTRL_RESRDYEO1_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_EVCTRL_RESRDYEO1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= ~DAC_EVCTRL_RESRDYEO1;
	tmp |= value << DAC_EVCTRL_RESRDYEO1_Pos;
	((Dac *)hw)->EVCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_RESRDYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~DAC_EVCTRL_RESRDYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_RESRDYEO1_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= DAC_EVCTRL_RESRDYEO1;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_EVCTRL_reg(const void *const hw, hri_dac_evctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg |= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_evctrl_reg_t hri_dac_get_EVCTRL_reg(const void *const hw, hri_dac_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dac_write_EVCTRL_reg(const void *const hw, hri_dac_evctrl_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_EVCTRL_reg(const void *const hw, hri_dac_evctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg &= ~mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_EVCTRL_reg(const void *const hw, hri_dac_evctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->EVCTRL.reg ^= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_evctrl_reg_t hri_dac_read_EVCTRL_reg(const void *const hw)
{
	return ((Dac *)hw)->EVCTRL.reg;
}

static inline void hri_dac_set_DACCTRL_LEFTADJ_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_LEFTADJ;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DACCTRL_LEFTADJ_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_LEFTADJ) >> DAC_DACCTRL_LEFTADJ_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DACCTRL_LEFTADJ_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_LEFTADJ;
	tmp |= value << DAC_DACCTRL_LEFTADJ_Pos;
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_LEFTADJ_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_LEFTADJ;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_LEFTADJ_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_LEFTADJ;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DACCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DACCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_ENABLE);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_ENABLE) >> DAC_DACCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DACCTRL_ENABLE_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_ENABLE);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_ENABLE;
	tmp |= value << DAC_DACCTRL_ENABLE_Pos;
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_ENABLE);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_ENABLE;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DACCTRL_FEXT_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_FEXT;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DACCTRL_FEXT_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_FEXT) >> DAC_DACCTRL_FEXT_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DACCTRL_FEXT_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_FEXT;
	tmp |= value << DAC_DACCTRL_FEXT_Pos;
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_FEXT_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_FEXT;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_FEXT_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_FEXT;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DACCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_RUNSTDBY;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DACCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_RUNSTDBY) >> DAC_DACCTRL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DACCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_RUNSTDBY;
	tmp |= value << DAC_DACCTRL_RUNSTDBY_Pos;
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_RUNSTDBY;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_RUNSTDBY;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DACCTRL_DITHER_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_DITHER;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DACCTRL_DITHER_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_DITHER) >> DAC_DACCTRL_DITHER_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DACCTRL_DITHER_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_DITHER;
	tmp |= value << DAC_DACCTRL_DITHER_Pos;
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_DITHER_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_DITHER;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_DITHER_bit(const void *const hw, uint8_t index)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_DITHER;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_CCTRL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_get_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index,
                                                                 hri_dac_dacctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_CCTRL(mask)) >> DAC_DACCTRL_CCTRL_Pos;
	return tmp;
}

static inline void hri_dac_write_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t data)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_CCTRL_Msk;
	tmp |= DAC_DACCTRL_CCTRL(data);
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_CCTRL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_CCTRL(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_read_DACCTRL_CCTRL_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_CCTRL_Msk) >> DAC_DACCTRL_CCTRL_Pos;
	return tmp;
}

static inline void hri_dac_set_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_REFRESH(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_get_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index,
                                                                   hri_dac_dacctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_REFRESH(mask)) >> DAC_DACCTRL_REFRESH_Pos;
	return tmp;
}

static inline void hri_dac_write_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t data)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_REFRESH_Msk;
	tmp |= DAC_DACCTRL_REFRESH(data);
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_REFRESH(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_REFRESH(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_read_DACCTRL_REFRESH_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_REFRESH_Msk) >> DAC_DACCTRL_REFRESH_Pos;
	return tmp;
}

static inline void hri_dac_set_DACCTRL_OSR_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg |= DAC_DACCTRL_OSR(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_get_DACCTRL_OSR_bf(const void *const hw, uint8_t index,
                                                               hri_dac_dacctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_OSR(mask)) >> DAC_DACCTRL_OSR_Pos;
	return tmp;
}

static inline void hri_dac_write_DACCTRL_OSR_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t data)
{
	uint16_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= ~DAC_DACCTRL_OSR_Msk;
	tmp |= DAC_DACCTRL_OSR(data);
	((Dac *)hw)->DACCTRL[index].reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_OSR_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg &= ~DAC_DACCTRL_OSR(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_OSR_bf(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_MASK);
	((Dac *)hw)->DACCTRL[index].reg ^= DAC_DACCTRL_OSR(mask);
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_read_DACCTRL_OSR_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp = (tmp & DAC_DACCTRL_OSR_Msk) >> DAC_DACCTRL_OSR_Pos;
	return tmp;
}

static inline void hri_dac_set_DACCTRL_reg(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DACCTRL[index].reg |= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_get_DACCTRL_reg(const void *const hw, uint8_t index,
                                                            hri_dac_dacctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->DACCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dac_write_DACCTRL_reg(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DACCTRL[index].reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DACCTRL_reg(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DACCTRL[index].reg &= ~mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DACCTRL_reg(const void *const hw, uint8_t index, hri_dac_dacctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DACCTRL[index].reg ^= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dacctrl_reg_t hri_dac_read_DACCTRL_reg(const void *const hw, uint8_t index)
{
	return ((Dac *)hw)->DACCTRL[index].reg;
}

static inline void hri_dac_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg |= DAC_DBGCTRL_DBGRUN;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dac_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->DBGCTRL.reg;
	tmp = (tmp & DAC_DBGCTRL_DBGRUN) >> DAC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_dac_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dac *)hw)->DBGCTRL.reg;
	tmp &= ~DAC_DBGCTRL_DBGRUN;
	tmp |= value << DAC_DBGCTRL_DBGRUN_Pos;
	((Dac *)hw)->DBGCTRL.reg = tmp;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg &= ~DAC_DBGCTRL_DBGRUN;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg ^= DAC_DBGCTRL_DBGRUN;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_set_DBGCTRL_reg(const void *const hw, hri_dac_dbgctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg |= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dbgctrl_reg_t hri_dac_get_DBGCTRL_reg(const void *const hw, hri_dac_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dac_write_DBGCTRL_reg(const void *const hw, hri_dac_dbgctrl_reg_t data)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg = data;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_clear_DBGCTRL_reg(const void *const hw, hri_dac_dbgctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg &= ~mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dac_toggle_DBGCTRL_reg(const void *const hw, hri_dac_dbgctrl_reg_t mask)
{
	DAC_CRITICAL_SECTION_ENTER();
	((Dac *)hw)->DBGCTRL.reg ^= mask;
	DAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dac_dbgctrl_reg_t hri_dac_read_DBGCTRL_reg(const void *const hw)
{
	return ((Dac *)hw)->DBGCTRL.reg;
}

static inline bool hri_dac_get_STATUS_READY0_bit(const void *const hw)
{
	return (((Dac *)hw)->STATUS.reg & DAC_STATUS_READY0) >> DAC_STATUS_READY0_Pos;
}

static inline bool hri_dac_get_STATUS_READY1_bit(const void *const hw)
{
	return (((Dac *)hw)->STATUS.reg & DAC_STATUS_READY1) >> DAC_STATUS_READY1_Pos;
}

static inline bool hri_dac_get_STATUS_EOC0_bit(const void *const hw)
{
	return (((Dac *)hw)->STATUS.reg & DAC_STATUS_EOC0) >> DAC_STATUS_EOC0_Pos;
}

static inline bool hri_dac_get_STATUS_EOC1_bit(const void *const hw)
{
	return (((Dac *)hw)->STATUS.reg & DAC_STATUS_EOC1) >> DAC_STATUS_EOC1_Pos;
}

static inline hri_dac_status_reg_t hri_dac_get_STATUS_reg(const void *const hw, hri_dac_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dac *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dac_status_reg_t hri_dac_read_STATUS_reg(const void *const hw)
{
	return ((Dac *)hw)->STATUS.reg;
}

static inline bool hri_dac_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_SWRST) >> DAC_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_dac_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_ENABLE) >> DAC_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_dac_get_SYNCBUSY_DATA0_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_DATA0) >> DAC_SYNCBUSY_DATA0_Pos;
}

static inline bool hri_dac_get_SYNCBUSY_DATA1_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_DATA1) >> DAC_SYNCBUSY_DATA1_Pos;
}

static inline bool hri_dac_get_SYNCBUSY_DATABUF0_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_DATABUF0) >> DAC_SYNCBUSY_DATABUF0_Pos;
}

static inline bool hri_dac_get_SYNCBUSY_DATABUF1_bit(const void *const hw)
{
	return (((Dac *)hw)->SYNCBUSY.reg & DAC_SYNCBUSY_DATABUF1) >> DAC_SYNCBUSY_DATABUF1_Pos;
}

static inline hri_dac_syncbusy_reg_t hri_dac_get_SYNCBUSY_reg(const void *const hw, hri_dac_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dac *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dac_syncbusy_reg_t hri_dac_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Dac *)hw)->SYNCBUSY.reg;
}

static inline hri_dac_result_reg_t hri_dac_get_RESULT_RESULT_bf(const void *const hw, uint8_t index,
                                                                hri_dac_result_reg_t mask)
{
	return (((Dac *)hw)->RESULT[index].reg & DAC_RESULT_RESULT(mask)) >> DAC_RESULT_RESULT_Pos;
}

static inline hri_dac_result_reg_t hri_dac_read_RESULT_RESULT_bf(const void *const hw, uint8_t index)
{
	return (((Dac *)hw)->RESULT[index].reg & DAC_RESULT_RESULT_Msk) >> DAC_RESULT_RESULT_Pos;
}

static inline hri_dac_result_reg_t hri_dac_get_RESULT_reg(const void *const hw, uint8_t index,
                                                          hri_dac_result_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dac *)hw)->RESULT[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dac_result_reg_t hri_dac_read_RESULT_reg(const void *const hw, uint8_t index)
{
	return ((Dac *)hw)->RESULT[index].reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_DAC_D51_H_INCLUDED */
#endif /* _SAMD51_DAC_COMPONENT_ */
