/**
 * \file
 *
 * \brief SAM EVSYS
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

#ifdef _SAMD51_EVSYS_COMPONENT_
#ifndef _HRI_EVSYS_D51_H_INCLUDED_
#define _HRI_EVSYS_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_EVSYS_CRITICAL_SECTIONS)
#define EVSYS_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define EVSYS_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define EVSYS_CRITICAL_SECTION_ENTER()
#define EVSYS_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_evsys_intpend_reg_t;
typedef uint32_t hri_evsys_busych_reg_t;
typedef uint32_t hri_evsys_channel_reg_t;
typedef uint32_t hri_evsys_intstatus_reg_t;
typedef uint32_t hri_evsys_readyusr_reg_t;
typedef uint32_t hri_evsys_swevt_reg_t;
typedef uint32_t hri_evsys_user_reg_t;
typedef uint32_t hri_evsyschannel_channel_reg_t;
typedef uint8_t  hri_evsys_chintenset_reg_t;
typedef uint8_t  hri_evsys_chintflag_reg_t;
typedef uint8_t  hri_evsys_chstatus_reg_t;
typedef uint8_t  hri_evsys_ctrla_reg_t;
typedef uint8_t  hri_evsys_prictrl_reg_t;
typedef uint8_t  hri_evsyschannel_chintenset_reg_t;
typedef uint8_t  hri_evsyschannel_chintflag_reg_t;
typedef uint8_t  hri_evsyschannel_chstatus_reg_t;

static inline void hri_evsys_write_SWEVT_reg(const void *const hw, hri_evsys_swevt_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->SWEVT.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_CTRLA_SWRST_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRLA.reg |= EVSYS_CTRLA_SWRST;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->CTRLA.reg;
	tmp = (tmp & EVSYS_CTRLA_SWRST) >> EVSYS_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_set_CTRLA_reg(const void *const hw, hri_evsys_ctrla_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRLA.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_ctrla_reg_t hri_evsys_get_CTRLA_reg(const void *const hw, hri_evsys_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_CTRLA_reg(const void *const hw, hri_evsys_ctrla_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRLA.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CTRLA_reg(const void *const hw, hri_evsys_ctrla_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRLA.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CTRLA_reg(const void *const hw, hri_evsys_ctrla_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRLA.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_ctrla_reg_t hri_evsys_read_CTRLA_reg(const void *const hw)
{
	return ((Evsys *)hw)->CTRLA.reg;
}

static inline void hri_evsys_set_PRICTRL_RREN_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg |= EVSYS_PRICTRL_RREN;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_PRICTRL_RREN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp = (tmp & EVSYS_PRICTRL_RREN) >> EVSYS_PRICTRL_RREN_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_PRICTRL_RREN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp &= ~EVSYS_PRICTRL_RREN;
	tmp |= value << EVSYS_PRICTRL_RREN_Pos;
	((Evsys *)hw)->PRICTRL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_PRICTRL_RREN_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg &= ~EVSYS_PRICTRL_RREN;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_PRICTRL_RREN_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg ^= EVSYS_PRICTRL_RREN;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_PRICTRL_PRI_bf(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg |= EVSYS_PRICTRL_PRI(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_prictrl_reg_t hri_evsys_get_PRICTRL_PRI_bf(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp = (tmp & EVSYS_PRICTRL_PRI(mask)) >> EVSYS_PRICTRL_PRI_Pos;
	return tmp;
}

static inline void hri_evsys_write_PRICTRL_PRI_bf(const void *const hw, hri_evsys_prictrl_reg_t data)
{
	uint8_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp &= ~EVSYS_PRICTRL_PRI_Msk;
	tmp |= EVSYS_PRICTRL_PRI(data);
	((Evsys *)hw)->PRICTRL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_PRICTRL_PRI_bf(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg &= ~EVSYS_PRICTRL_PRI(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_PRICTRL_PRI_bf(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg ^= EVSYS_PRICTRL_PRI(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_prictrl_reg_t hri_evsys_read_PRICTRL_PRI_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp = (tmp & EVSYS_PRICTRL_PRI_Msk) >> EVSYS_PRICTRL_PRI_Pos;
	return tmp;
}

static inline void hri_evsys_set_PRICTRL_reg(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_prictrl_reg_t hri_evsys_get_PRICTRL_reg(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->PRICTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_PRICTRL_reg(const void *const hw, hri_evsys_prictrl_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_PRICTRL_reg(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_PRICTRL_reg(const void *const hw, hri_evsys_prictrl_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->PRICTRL.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_prictrl_reg_t hri_evsys_read_PRICTRL_reg(const void *const hw)
{
	return ((Evsys *)hw)->PRICTRL.reg;
}

static inline void hri_evsys_set_INTPEND_OVR_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= EVSYS_INTPEND_OVR;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_INTPEND_OVR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_OVR) >> EVSYS_INTPEND_OVR_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_INTPEND_OVR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= ~EVSYS_INTPEND_OVR;
	tmp |= value << EVSYS_INTPEND_OVR_Pos;
	((Evsys *)hw)->INTPEND.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_OVR_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~EVSYS_INTPEND_OVR;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_OVR_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= EVSYS_INTPEND_OVR;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_INTPEND_EVD_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= EVSYS_INTPEND_EVD;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_INTPEND_EVD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_EVD) >> EVSYS_INTPEND_EVD_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_INTPEND_EVD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= ~EVSYS_INTPEND_EVD;
	tmp |= value << EVSYS_INTPEND_EVD_Pos;
	((Evsys *)hw)->INTPEND.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_EVD_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~EVSYS_INTPEND_EVD;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_EVD_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= EVSYS_INTPEND_EVD;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_INTPEND_READY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= EVSYS_INTPEND_READY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_INTPEND_READY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_READY) >> EVSYS_INTPEND_READY_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_INTPEND_READY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= ~EVSYS_INTPEND_READY;
	tmp |= value << EVSYS_INTPEND_READY_Pos;
	((Evsys *)hw)->INTPEND.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_READY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~EVSYS_INTPEND_READY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_READY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= EVSYS_INTPEND_READY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_INTPEND_BUSY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= EVSYS_INTPEND_BUSY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_INTPEND_BUSY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_BUSY) >> EVSYS_INTPEND_BUSY_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_INTPEND_BUSY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= ~EVSYS_INTPEND_BUSY;
	tmp |= value << EVSYS_INTPEND_BUSY_Pos;
	((Evsys *)hw)->INTPEND.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_BUSY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~EVSYS_INTPEND_BUSY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_BUSY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= EVSYS_INTPEND_BUSY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_INTPEND_ID_bf(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= EVSYS_INTPEND_ID(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_intpend_reg_t hri_evsys_get_INTPEND_ID_bf(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_ID(mask)) >> EVSYS_INTPEND_ID_Pos;
	return tmp;
}

static inline void hri_evsys_write_INTPEND_ID_bf(const void *const hw, hri_evsys_intpend_reg_t data)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= ~EVSYS_INTPEND_ID_Msk;
	tmp |= EVSYS_INTPEND_ID(data);
	((Evsys *)hw)->INTPEND.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_ID_bf(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~EVSYS_INTPEND_ID(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_ID_bf(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= EVSYS_INTPEND_ID(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_intpend_reg_t hri_evsys_read_INTPEND_ID_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp = (tmp & EVSYS_INTPEND_ID_Msk) >> EVSYS_INTPEND_ID_Pos;
	return tmp;
}

static inline void hri_evsys_set_INTPEND_reg(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_intpend_reg_t hri_evsys_get_INTPEND_reg(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->INTPEND.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_INTPEND_reg(const void *const hw, hri_evsys_intpend_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_INTPEND_reg(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_INTPEND_reg(const void *const hw, hri_evsys_intpend_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->INTPEND.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_intpend_reg_t hri_evsys_read_INTPEND_reg(const void *const hw)
{
	return ((Evsys *)hw)->INTPEND.reg;
}

static inline void hri_evsys_set_USER_CHANNEL_bf(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg |= EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_get_USER_CHANNEL_bf(const void *const hw, uint8_t index,
                                                                 hri_evsys_user_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->USER[index].reg;
	tmp = (tmp & EVSYS_USER_CHANNEL(mask)) >> EVSYS_USER_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_write_USER_CHANNEL_bf(const void *const hw, uint8_t index, hri_evsys_user_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->USER[index].reg;
	tmp &= ~EVSYS_USER_CHANNEL_Msk;
	tmp |= EVSYS_USER_CHANNEL(data);
	((Evsys *)hw)->USER[index].reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_USER_CHANNEL_bf(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg &= ~EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_USER_CHANNEL_bf(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg ^= EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_read_USER_CHANNEL_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->USER[index].reg;
	tmp = (tmp & EVSYS_USER_CHANNEL_Msk) >> EVSYS_USER_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_set_USER_reg(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_get_USER_reg(const void *const hw, uint8_t index,
                                                          hri_evsys_user_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->USER[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_USER_reg(const void *const hw, uint8_t index, hri_evsys_user_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_USER_reg(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_USER_reg(const void *const hw, uint8_t index, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER[index].reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_read_USER_reg(const void *const hw, uint8_t index)
{
	return ((Evsys *)hw)->USER[index].reg;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT0) >> EVSYS_INTSTATUS_CHINT0_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT1) >> EVSYS_INTSTATUS_CHINT1_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT2) >> EVSYS_INTSTATUS_CHINT2_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT3) >> EVSYS_INTSTATUS_CHINT3_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT4) >> EVSYS_INTSTATUS_CHINT4_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT5) >> EVSYS_INTSTATUS_CHINT5_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT6) >> EVSYS_INTSTATUS_CHINT6_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT7) >> EVSYS_INTSTATUS_CHINT7_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT8) >> EVSYS_INTSTATUS_CHINT8_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT9) >> EVSYS_INTSTATUS_CHINT9_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT10) >> EVSYS_INTSTATUS_CHINT10_Pos;
}

static inline bool hri_evsys_get_INTSTATUS_CHINT11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTSTATUS.reg & EVSYS_INTSTATUS_CHINT11) >> EVSYS_INTSTATUS_CHINT11_Pos;
}

static inline hri_evsys_intstatus_reg_t hri_evsys_get_INTSTATUS_reg(const void *const         hw,
                                                                    hri_evsys_intstatus_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->INTSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_intstatus_reg_t hri_evsys_read_INTSTATUS_reg(const void *const hw)
{
	return ((Evsys *)hw)->INTSTATUS.reg;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH0_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH0) >> EVSYS_BUSYCH_BUSYCH0_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH1_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH1) >> EVSYS_BUSYCH_BUSYCH1_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH2_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH2) >> EVSYS_BUSYCH_BUSYCH2_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH3_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH3) >> EVSYS_BUSYCH_BUSYCH3_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH4_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH4) >> EVSYS_BUSYCH_BUSYCH4_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH5_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH5) >> EVSYS_BUSYCH_BUSYCH5_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH6_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH6) >> EVSYS_BUSYCH_BUSYCH6_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH7_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH7) >> EVSYS_BUSYCH_BUSYCH7_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH8_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH8) >> EVSYS_BUSYCH_BUSYCH8_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH9_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH9) >> EVSYS_BUSYCH_BUSYCH9_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH10_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH10) >> EVSYS_BUSYCH_BUSYCH10_Pos;
}

static inline bool hri_evsys_get_BUSYCH_BUSYCH11_bit(const void *const hw)
{
	return (((Evsys *)hw)->BUSYCH.reg & EVSYS_BUSYCH_BUSYCH11) >> EVSYS_BUSYCH_BUSYCH11_Pos;
}

static inline hri_evsys_busych_reg_t hri_evsys_get_BUSYCH_reg(const void *const hw, hri_evsys_busych_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->BUSYCH.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_busych_reg_t hri_evsys_read_BUSYCH_reg(const void *const hw)
{
	return ((Evsys *)hw)->BUSYCH.reg;
}

static inline bool hri_evsys_get_READYUSR_READYUSR0_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR0) >> EVSYS_READYUSR_READYUSR0_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR1_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR1) >> EVSYS_READYUSR_READYUSR1_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR2_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR2) >> EVSYS_READYUSR_READYUSR2_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR3_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR3) >> EVSYS_READYUSR_READYUSR3_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR4_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR4) >> EVSYS_READYUSR_READYUSR4_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR5_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR5) >> EVSYS_READYUSR_READYUSR5_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR6_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR6) >> EVSYS_READYUSR_READYUSR6_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR7_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR7) >> EVSYS_READYUSR_READYUSR7_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR8_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR8) >> EVSYS_READYUSR_READYUSR8_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR9_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR9) >> EVSYS_READYUSR_READYUSR9_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR10_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR10) >> EVSYS_READYUSR_READYUSR10_Pos;
}

static inline bool hri_evsys_get_READYUSR_READYUSR11_bit(const void *const hw)
{
	return (((Evsys *)hw)->READYUSR.reg & EVSYS_READYUSR_READYUSR11) >> EVSYS_READYUSR_READYUSR11_Pos;
}

static inline hri_evsys_readyusr_reg_t hri_evsys_get_READYUSR_reg(const void *const hw, hri_evsys_readyusr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->READYUSR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_readyusr_reg_t hri_evsys_read_READYUSR_reg(const void *const hw)
{
	return ((Evsys *)hw)->READYUSR.reg;
}

static inline void hri_evsyschannel_set_CHINTEN_OVR_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTENSET.reg = EVSYS_CHINTENSET_OVR;
}

static inline bool hri_evsyschannel_get_CHINTEN_OVR_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTENSET.reg & EVSYS_CHINTENSET_OVR) >> EVSYS_CHINTENSET_OVR_Pos;
}

static inline void hri_evsyschannel_write_CHINTEN_OVR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((EvsysChannel *)hw)->CHINTENCLR.reg = EVSYS_CHINTENSET_OVR;
	} else {
		((EvsysChannel *)hw)->CHINTENSET.reg = EVSYS_CHINTENSET_OVR;
	}
}

static inline void hri_evsyschannel_clear_CHINTEN_OVR_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTENCLR.reg = EVSYS_CHINTENSET_OVR;
}

static inline void hri_evsyschannel_set_CHINTEN_EVD_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTENSET.reg = EVSYS_CHINTENSET_EVD;
}

static inline bool hri_evsyschannel_get_CHINTEN_EVD_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTENSET.reg & EVSYS_CHINTENSET_EVD) >> EVSYS_CHINTENSET_EVD_Pos;
}

static inline void hri_evsyschannel_write_CHINTEN_EVD_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((EvsysChannel *)hw)->CHINTENCLR.reg = EVSYS_CHINTENSET_EVD;
	} else {
		((EvsysChannel *)hw)->CHINTENSET.reg = EVSYS_CHINTENSET_EVD;
	}
}

static inline void hri_evsyschannel_clear_CHINTEN_EVD_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTENCLR.reg = EVSYS_CHINTENSET_EVD;
}

static inline void hri_evsyschannel_set_CHINTEN_reg(const void *const hw, hri_evsys_chintenset_reg_t mask)
{
	((EvsysChannel *)hw)->CHINTENSET.reg = mask;
}

static inline hri_evsys_chintenset_reg_t hri_evsyschannel_get_CHINTEN_reg(const void *const          hw,
                                                                          hri_evsys_chintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((EvsysChannel *)hw)->CHINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chintenset_reg_t hri_evsyschannel_read_CHINTEN_reg(const void *const hw)
{
	return ((EvsysChannel *)hw)->CHINTENSET.reg;
}

static inline void hri_evsyschannel_write_CHINTEN_reg(const void *const hw, hri_evsys_chintenset_reg_t data)
{
	((EvsysChannel *)hw)->CHINTENSET.reg = data;
	((EvsysChannel *)hw)->CHINTENCLR.reg = ~data;
}

static inline void hri_evsyschannel_clear_CHINTEN_reg(const void *const hw, hri_evsys_chintenset_reg_t mask)
{
	((EvsysChannel *)hw)->CHINTENCLR.reg = mask;
}

static inline bool hri_evsyschannel_get_CHINTFLAG_OVR_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTFLAG.reg & EVSYS_CHINTFLAG_OVR) >> EVSYS_CHINTFLAG_OVR_Pos;
}

static inline void hri_evsyschannel_clear_CHINTFLAG_OVR_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTFLAG.reg = EVSYS_CHINTFLAG_OVR;
}

static inline bool hri_evsyschannel_get_CHINTFLAG_EVD_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTFLAG.reg & EVSYS_CHINTFLAG_EVD) >> EVSYS_CHINTFLAG_EVD_Pos;
}

static inline void hri_evsyschannel_clear_CHINTFLAG_EVD_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD;
}

static inline bool hri_evsyschannel_get_interrupt_OVR_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTFLAG.reg & EVSYS_CHINTFLAG_OVR) >> EVSYS_CHINTFLAG_OVR_Pos;
}

static inline void hri_evsyschannel_clear_interrupt_OVR_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTFLAG.reg = EVSYS_CHINTFLAG_OVR;
}

static inline bool hri_evsyschannel_get_interrupt_EVD_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHINTFLAG.reg & EVSYS_CHINTFLAG_EVD) >> EVSYS_CHINTFLAG_EVD_Pos;
}

static inline void hri_evsyschannel_clear_interrupt_EVD_bit(const void *const hw)
{
	((EvsysChannel *)hw)->CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD;
}

static inline hri_evsys_chintflag_reg_t hri_evsyschannel_get_CHINTFLAG_reg(const void *const         hw,
                                                                           hri_evsys_chintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((EvsysChannel *)hw)->CHINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chintflag_reg_t hri_evsyschannel_read_CHINTFLAG_reg(const void *const hw)
{
	return ((EvsysChannel *)hw)->CHINTFLAG.reg;
}

static inline void hri_evsyschannel_clear_CHINTFLAG_reg(const void *const hw, hri_evsys_chintflag_reg_t mask)
{
	((EvsysChannel *)hw)->CHINTFLAG.reg = mask;
}

static inline void hri_evsyschannel_set_CHANNEL_RUNSTDBY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsyschannel_get_CHANNEL_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_RUNSTDBY) >> EVSYS_CHANNEL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_RUNSTDBY;
	tmp |= value << EVSYS_CHANNEL_RUNSTDBY_Pos;
	((EvsysChannel *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_RUNSTDBY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_RUNSTDBY_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_set_CHANNEL_ONDEMAND_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsyschannel_get_CHANNEL_ONDEMAND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_ONDEMAND) >> EVSYS_CHANNEL_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_ONDEMAND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_ONDEMAND;
	tmp |= value << EVSYS_CHANNEL_ONDEMAND_Pos;
	((EvsysChannel *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_ONDEMAND_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_ONDEMAND_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_set_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_get_CHANNEL_EVGEN_bf(const void *const       hw,
                                                                            hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN(mask)) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EVGEN_Msk;
	tmp |= EVSYS_CHANNEL_EVGEN(data);
	((EvsysChannel *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_read_CHANNEL_EVGEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN_Msk) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsyschannel_set_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_get_CHANNEL_PATH_bf(const void *const       hw,
                                                                           hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH(mask)) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_PATH_Msk;
	tmp |= EVSYS_CHANNEL_PATH(data);
	((EvsysChannel *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_read_CHANNEL_PATH_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH_Msk) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsyschannel_set_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_get_CHANNEL_EDGSEL_bf(const void *const       hw,
                                                                             hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL(mask)) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EDGSEL_Msk;
	tmp |= EVSYS_CHANNEL_EDGSEL(data);
	((EvsysChannel *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_read_CHANNEL_EDGSEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL_Msk) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsyschannel_set_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_get_CHANNEL_reg(const void *const       hw,
                                                                       hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((EvsysChannel *)hw)->CHANNEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsyschannel_write_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_clear_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsyschannel_toggle_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((EvsysChannel *)hw)->CHANNEL.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsyschannel_read_CHANNEL_reg(const void *const hw)
{
	return ((EvsysChannel *)hw)->CHANNEL.reg;
}

static inline bool hri_evsyschannel_get_CHSTATUS_RDYUSR_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_RDYUSR) >> EVSYS_CHSTATUS_RDYUSR_Pos;
}

static inline bool hri_evsyschannel_get_CHSTATUS_BUSYCH_bit(const void *const hw)
{
	return (((EvsysChannel *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_BUSYCH) >> EVSYS_CHSTATUS_BUSYCH_Pos;
}

static inline hri_evsys_chstatus_reg_t hri_evsyschannel_get_CHSTATUS_reg(const void *const        hw,
                                                                         hri_evsys_chstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((EvsysChannel *)hw)->CHSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chstatus_reg_t hri_evsyschannel_read_CHSTATUS_reg(const void *const hw)
{
	return ((EvsysChannel *)hw)->CHSTATUS.reg;
}

static inline void hri_evsys_set_CHINTEN_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = EVSYS_CHINTENSET_OVR;
}

static inline bool hri_evsys_get_CHINTEN_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg & EVSYS_CHINTENSET_OVR) >> EVSYS_CHINTENSET_OVR_Pos;
}

static inline void hri_evsys_write_CHINTEN_OVR_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = EVSYS_CHINTENSET_OVR;
	} else {
		((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = EVSYS_CHINTENSET_OVR;
	}
}

static inline void hri_evsys_clear_CHINTEN_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = EVSYS_CHINTENSET_OVR;
}

static inline void hri_evsys_set_CHINTEN_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = EVSYS_CHINTENSET_EVD;
}

static inline bool hri_evsys_get_CHINTEN_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg & EVSYS_CHINTENSET_EVD) >> EVSYS_CHINTENSET_EVD_Pos;
}

static inline void hri_evsys_write_CHINTEN_EVD_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = EVSYS_CHINTENSET_EVD;
	} else {
		((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = EVSYS_CHINTENSET_EVD;
	}
}

static inline void hri_evsys_clear_CHINTEN_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = EVSYS_CHINTENSET_EVD;
}

static inline void hri_evsys_set_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                             hri_evsys_chintenset_reg_t mask)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = mask;
}

static inline hri_evsys_chintenset_reg_t hri_evsys_get_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                                   hri_evsys_chintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chintenset_reg_t hri_evsys_read_CHINTEN_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg;
}

static inline void hri_evsys_write_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                               hri_evsys_chintenset_reg_t data)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENSET.reg = data;
	((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = ~data;
}

static inline void hri_evsys_clear_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                               hri_evsys_chintenset_reg_t mask)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTENCLR.reg = mask;
}

static inline bool hri_evsys_get_CHINTFLAG_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg & EVSYS_CHINTFLAG_OVR) >> EVSYS_CHINTFLAG_OVR_Pos;
}

static inline void hri_evsys_clear_CHINTFLAG_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg = EVSYS_CHINTFLAG_OVR;
}

static inline bool hri_evsys_get_CHINTFLAG_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg & EVSYS_CHINTFLAG_EVD) >> EVSYS_CHINTFLAG_EVD_Pos;
}

static inline void hri_evsys_clear_CHINTFLAG_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD;
}

static inline bool hri_evsys_get_interrupt_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg & EVSYS_CHINTFLAG_OVR) >> EVSYS_CHINTFLAG_OVR_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg = EVSYS_CHINTFLAG_OVR;
}

static inline bool hri_evsys_get_interrupt_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg & EVSYS_CHINTFLAG_EVD) >> EVSYS_CHINTFLAG_EVD_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD_bit(const void *const hw, uint8_t submodule_index)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD;
}

static inline hri_evsys_chintflag_reg_t hri_evsys_get_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                                    hri_evsys_chintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chintflag_reg_t hri_evsys_read_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg;
}

static inline void hri_evsys_clear_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_evsys_chintflag_reg_t mask)
{
	((Evsys *)hw)->Channel[submodule_index].CHINTFLAG.reg = mask;
}

static inline void hri_evsys_set_CHANNEL_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_CHANNEL_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_RUNSTDBY) >> EVSYS_CHANNEL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_CHANNEL_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_RUNSTDBY;
	tmp |= value << EVSYS_CHANNEL_RUNSTDBY_Pos;
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= EVSYS_CHANNEL_RUNSTDBY;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_CHANNEL_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_CHANNEL_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_ONDEMAND) >> EVSYS_CHANNEL_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_CHANNEL_ONDEMAND_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_ONDEMAND;
	tmp |= value << EVSYS_CHANNEL_ONDEMAND_Pos;
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= EVSYS_CHANNEL_ONDEMAND;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN(mask)) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EVGEN_Msk;
	tmp |= EVSYS_CHANNEL_EVGEN(data);
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_EVGEN_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN_Msk) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH(mask)) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_PATH_Msk;
	tmp |= EVSYS_CHANNEL_PATH(data);
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_PATH_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH_Msk) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL(mask)) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EDGSEL_Msk;
	tmp |= EVSYS_CHANNEL_EDGSEL(data);
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_EDGSEL_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL_Msk) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_reg(const void *const hw, uint8_t submodule_index,
                                             hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_evsys_channel_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_reg(const void *const hw, uint8_t submodule_index,
                                                hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Evsys *)hw)->Channel[submodule_index].CHANNEL.reg;
}

static inline bool hri_evsys_get_CHSTATUS_RDYUSR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHSTATUS.reg & EVSYS_CHSTATUS_RDYUSR) >> EVSYS_CHSTATUS_RDYUSR_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_BUSYCH_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Evsys *)hw)->Channel[submodule_index].CHSTATUS.reg & EVSYS_CHSTATUS_BUSYCH) >> EVSYS_CHSTATUS_BUSYCH_Pos;
}

static inline hri_evsys_chstatus_reg_t hri_evsys_get_CHSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                  hri_evsys_chstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Evsys *)hw)->Channel[submodule_index].CHSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chstatus_reg_t hri_evsys_read_CHSTATUS_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Evsys *)hw)->Channel[submodule_index].CHSTATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_EVSYS_D51_H_INCLUDED */
#endif /* _SAMD51_EVSYS_COMPONENT_ */
