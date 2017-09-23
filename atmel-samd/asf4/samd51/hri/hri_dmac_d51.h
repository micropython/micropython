/**
 * \file
 *
 * \brief SAM DMAC
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

#ifdef _SAMD51_DMAC_COMPONENT_
#ifndef _HRI_DMAC_D51_H_INCLUDED_
#define _HRI_DMAC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_DMAC_CRITICAL_SECTIONS)
#define DMAC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define DMAC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define DMAC_CRITICAL_SECTION_ENTER()
#define DMAC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_dmac_crcctrl_reg_t;
typedef uint16_t hri_dmac_ctrl_reg_t;
typedef uint16_t hri_dmac_intpend_reg_t;
typedef uint16_t hri_dmacdescriptor_btcnt_reg_t;
typedef uint16_t hri_dmacdescriptor_btctrl_reg_t;
typedef uint32_t hri_dmac_active_reg_t;
typedef uint32_t hri_dmac_baseaddr_reg_t;
typedef uint32_t hri_dmac_busych_reg_t;
typedef uint32_t hri_dmac_chctrla_reg_t;
typedef uint32_t hri_dmac_crcchksum_reg_t;
typedef uint32_t hri_dmac_crcdatain_reg_t;
typedef uint32_t hri_dmac_intstatus_reg_t;
typedef uint32_t hri_dmac_pendch_reg_t;
typedef uint32_t hri_dmac_prictrl0_reg_t;
typedef uint32_t hri_dmac_swtrigctrl_reg_t;
typedef uint32_t hri_dmac_wrbaddr_reg_t;
typedef uint32_t hri_dmacchannel_chctrla_reg_t;
typedef uint32_t hri_dmacdescriptor_descaddr_reg_t;
typedef uint32_t hri_dmacdescriptor_dstaddr_reg_t;
typedef uint32_t hri_dmacdescriptor_srcaddr_reg_t;
typedef uint8_t  hri_dmac_chctrlb_reg_t;
typedef uint8_t  hri_dmac_chevctrl_reg_t;
typedef uint8_t  hri_dmac_chintenset_reg_t;
typedef uint8_t  hri_dmac_chintflag_reg_t;
typedef uint8_t  hri_dmac_chprilvl_reg_t;
typedef uint8_t  hri_dmac_chstatus_reg_t;
typedef uint8_t  hri_dmac_crcstatus_reg_t;
typedef uint8_t  hri_dmac_dbgctrl_reg_t;
typedef uint8_t  hri_dmacchannel_chctrlb_reg_t;
typedef uint8_t  hri_dmacchannel_chevctrl_reg_t;
typedef uint8_t  hri_dmacchannel_chintenset_reg_t;
typedef uint8_t  hri_dmacchannel_chintflag_reg_t;
typedef uint8_t  hri_dmacchannel_chprilvl_reg_t;
typedef uint8_t  hri_dmacchannel_chstatus_reg_t;

static inline void hri_dmac_set_CTRL_SWRST_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_SWRST;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_SWRST_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_SWRST) >> DMAC_CTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_set_CTRL_DMAENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_DMAENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_DMAENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_DMAENABLE) >> DMAC_CTRL_DMAENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CTRL_DMAENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= ~DMAC_CTRL_DMAENABLE;
	tmp |= value << DMAC_CTRL_DMAENABLE_Pos;
	((Dmac *)hw)->CTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_DMAENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~DMAC_CTRL_DMAENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_DMAENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= DMAC_CTRL_DMAENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CTRL_LVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_LVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_LVLEN0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_LVLEN0) >> DMAC_CTRL_LVLEN0_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CTRL_LVLEN0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= ~DMAC_CTRL_LVLEN0;
	tmp |= value << DMAC_CTRL_LVLEN0_Pos;
	((Dmac *)hw)->CTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_LVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~DMAC_CTRL_LVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_LVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= DMAC_CTRL_LVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CTRL_LVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_LVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_LVLEN1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_LVLEN1) >> DMAC_CTRL_LVLEN1_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CTRL_LVLEN1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= ~DMAC_CTRL_LVLEN1;
	tmp |= value << DMAC_CTRL_LVLEN1_Pos;
	((Dmac *)hw)->CTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_LVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~DMAC_CTRL_LVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_LVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= DMAC_CTRL_LVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CTRL_LVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_LVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_LVLEN2_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_LVLEN2) >> DMAC_CTRL_LVLEN2_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CTRL_LVLEN2_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= ~DMAC_CTRL_LVLEN2;
	tmp |= value << DMAC_CTRL_LVLEN2_Pos;
	((Dmac *)hw)->CTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_LVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~DMAC_CTRL_LVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_LVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= DMAC_CTRL_LVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CTRL_LVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= DMAC_CTRL_LVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CTRL_LVLEN3_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp = (tmp & DMAC_CTRL_LVLEN3) >> DMAC_CTRL_LVLEN3_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CTRL_LVLEN3_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= ~DMAC_CTRL_LVLEN3;
	tmp |= value << DMAC_CTRL_LVLEN3_Pos;
	((Dmac *)hw)->CTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_LVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~DMAC_CTRL_LVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_LVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= DMAC_CTRL_LVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CTRL_reg(const void *const hw, hri_dmac_ctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_ctrl_reg_t hri_dmac_get_CTRL_reg(const void *const hw, hri_dmac_ctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CTRL_reg(const void *const hw, hri_dmac_ctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CTRL_reg(const void *const hw, hri_dmac_ctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CTRL_reg(const void *const hw, hri_dmac_ctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_ctrl_reg_t hri_dmac_read_CTRL_reg(const void *const hw)
{
	return ((Dmac *)hw)->CTRL.reg;
}

static inline void hri_dmac_set_CRCCTRL_CRCBEATSIZE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg |= DMAC_CRCCTRL_CRCBEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_get_CRCCTRL_CRCBEATSIZE_bf(const void *const      hw,
                                                                         hri_dmac_crcctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCBEATSIZE(mask)) >> DMAC_CRCCTRL_CRCBEATSIZE_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCCTRL_CRCBEATSIZE_bf(const void *const hw, hri_dmac_crcctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp &= ~DMAC_CRCCTRL_CRCBEATSIZE_Msk;
	tmp |= DMAC_CRCCTRL_CRCBEATSIZE(data);
	((Dmac *)hw)->CRCCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCTRL_CRCBEATSIZE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg &= ~DMAC_CRCCTRL_CRCBEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCTRL_CRCBEATSIZE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg ^= DMAC_CRCCTRL_CRCBEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_read_CRCCTRL_CRCBEATSIZE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCBEATSIZE_Msk) >> DMAC_CRCCTRL_CRCBEATSIZE_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCCTRL_CRCPOLY_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg |= DMAC_CRCCTRL_CRCPOLY(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_get_CRCCTRL_CRCPOLY_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCPOLY(mask)) >> DMAC_CRCCTRL_CRCPOLY_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCCTRL_CRCPOLY_bf(const void *const hw, hri_dmac_crcctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp &= ~DMAC_CRCCTRL_CRCPOLY_Msk;
	tmp |= DMAC_CRCCTRL_CRCPOLY(data);
	((Dmac *)hw)->CRCCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCTRL_CRCPOLY_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg &= ~DMAC_CRCCTRL_CRCPOLY(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCTRL_CRCPOLY_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg ^= DMAC_CRCCTRL_CRCPOLY(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_read_CRCCTRL_CRCPOLY_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCPOLY_Msk) >> DMAC_CRCCTRL_CRCPOLY_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCCTRL_CRCSRC_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg |= DMAC_CRCCTRL_CRCSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_get_CRCCTRL_CRCSRC_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCSRC(mask)) >> DMAC_CRCCTRL_CRCSRC_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCCTRL_CRCSRC_bf(const void *const hw, hri_dmac_crcctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp &= ~DMAC_CRCCTRL_CRCSRC_Msk;
	tmp |= DMAC_CRCCTRL_CRCSRC(data);
	((Dmac *)hw)->CRCCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCTRL_CRCSRC_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg &= ~DMAC_CRCCTRL_CRCSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCTRL_CRCSRC_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg ^= DMAC_CRCCTRL_CRCSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_read_CRCCTRL_CRCSRC_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCSRC_Msk) >> DMAC_CRCCTRL_CRCSRC_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCCTRL_CRCMODE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg |= DMAC_CRCCTRL_CRCMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_get_CRCCTRL_CRCMODE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCMODE(mask)) >> DMAC_CRCCTRL_CRCMODE_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCCTRL_CRCMODE_bf(const void *const hw, hri_dmac_crcctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp &= ~DMAC_CRCCTRL_CRCMODE_Msk;
	tmp |= DMAC_CRCCTRL_CRCMODE(data);
	((Dmac *)hw)->CRCCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCTRL_CRCMODE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg &= ~DMAC_CRCCTRL_CRCMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCTRL_CRCMODE_bf(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg ^= DMAC_CRCCTRL_CRCMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_read_CRCCTRL_CRCMODE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp = (tmp & DMAC_CRCCTRL_CRCMODE_Msk) >> DMAC_CRCCTRL_CRCMODE_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCCTRL_reg(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_get_CRCCTRL_reg(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->CRCCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CRCCTRL_reg(const void *const hw, hri_dmac_crcctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCTRL_reg(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCTRL_reg(const void *const hw, hri_dmac_crcctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcctrl_reg_t hri_dmac_read_CRCCTRL_reg(const void *const hw)
{
	return ((Dmac *)hw)->CRCCTRL.reg;
}

static inline void hri_dmac_set_CRCDATAIN_CRCDATAIN_bf(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg |= DMAC_CRCDATAIN_CRCDATAIN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcdatain_reg_t hri_dmac_get_CRCDATAIN_CRCDATAIN_bf(const void *const        hw,
                                                                           hri_dmac_crcdatain_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCDATAIN.reg;
	tmp = (tmp & DMAC_CRCDATAIN_CRCDATAIN(mask)) >> DMAC_CRCDATAIN_CRCDATAIN_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCDATAIN_CRCDATAIN_bf(const void *const hw, hri_dmac_crcdatain_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCDATAIN.reg;
	tmp &= ~DMAC_CRCDATAIN_CRCDATAIN_Msk;
	tmp |= DMAC_CRCDATAIN_CRCDATAIN(data);
	((Dmac *)hw)->CRCDATAIN.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCDATAIN_CRCDATAIN_bf(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg &= ~DMAC_CRCDATAIN_CRCDATAIN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCDATAIN_CRCDATAIN_bf(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg ^= DMAC_CRCDATAIN_CRCDATAIN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcdatain_reg_t hri_dmac_read_CRCDATAIN_CRCDATAIN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCDATAIN.reg;
	tmp = (tmp & DMAC_CRCDATAIN_CRCDATAIN_Msk) >> DMAC_CRCDATAIN_CRCDATAIN_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCDATAIN_reg(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcdatain_reg_t hri_dmac_get_CRCDATAIN_reg(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCDATAIN.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CRCDATAIN_reg(const void *const hw, hri_dmac_crcdatain_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCDATAIN_reg(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCDATAIN_reg(const void *const hw, hri_dmac_crcdatain_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCDATAIN.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcdatain_reg_t hri_dmac_read_CRCDATAIN_reg(const void *const hw)
{
	return ((Dmac *)hw)->CRCDATAIN.reg;
}

static inline void hri_dmac_set_CRCCHKSUM_CRCCHKSUM_bf(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg |= DMAC_CRCCHKSUM_CRCCHKSUM(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcchksum_reg_t hri_dmac_get_CRCCHKSUM_CRCCHKSUM_bf(const void *const        hw,
                                                                           hri_dmac_crcchksum_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCCHKSUM.reg;
	tmp = (tmp & DMAC_CRCCHKSUM_CRCCHKSUM(mask)) >> DMAC_CRCCHKSUM_CRCCHKSUM_Pos;
	return tmp;
}

static inline void hri_dmac_write_CRCCHKSUM_CRCCHKSUM_bf(const void *const hw, hri_dmac_crcchksum_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->CRCCHKSUM.reg;
	tmp &= ~DMAC_CRCCHKSUM_CRCCHKSUM_Msk;
	tmp |= DMAC_CRCCHKSUM_CRCCHKSUM(data);
	((Dmac *)hw)->CRCCHKSUM.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCHKSUM_CRCCHKSUM_bf(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg &= ~DMAC_CRCCHKSUM_CRCCHKSUM(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCHKSUM_CRCCHKSUM_bf(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg ^= DMAC_CRCCHKSUM_CRCCHKSUM(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcchksum_reg_t hri_dmac_read_CRCCHKSUM_CRCCHKSUM_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCCHKSUM.reg;
	tmp = (tmp & DMAC_CRCCHKSUM_CRCCHKSUM_Msk) >> DMAC_CRCCHKSUM_CRCCHKSUM_Pos;
	return tmp;
}

static inline void hri_dmac_set_CRCCHKSUM_reg(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcchksum_reg_t hri_dmac_get_CRCCHKSUM_reg(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->CRCCHKSUM.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CRCCHKSUM_reg(const void *const hw, hri_dmac_crcchksum_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CRCCHKSUM_reg(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CRCCHKSUM_reg(const void *const hw, hri_dmac_crcchksum_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCCHKSUM.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcchksum_reg_t hri_dmac_read_CRCCHKSUM_reg(const void *const hw)
{
	return ((Dmac *)hw)->CRCCHKSUM.reg;
}

static inline void hri_dmac_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg |= DMAC_DBGCTRL_DBGRUN;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->DBGCTRL.reg;
	tmp = (tmp & DMAC_DBGCTRL_DBGRUN) >> DMAC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->DBGCTRL.reg;
	tmp &= ~DMAC_DBGCTRL_DBGRUN;
	tmp |= value << DMAC_DBGCTRL_DBGRUN_Pos;
	((Dmac *)hw)->DBGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg &= ~DMAC_DBGCTRL_DBGRUN;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg ^= DMAC_DBGCTRL_DBGRUN;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_DBGCTRL_reg(const void *const hw, hri_dmac_dbgctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_dbgctrl_reg_t hri_dmac_get_DBGCTRL_reg(const void *const hw, hri_dmac_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_DBGCTRL_reg(const void *const hw, hri_dmac_dbgctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_DBGCTRL_reg(const void *const hw, hri_dmac_dbgctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_DBGCTRL_reg(const void *const hw, hri_dmac_dbgctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->DBGCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_dbgctrl_reg_t hri_dmac_read_DBGCTRL_reg(const void *const hw)
{
	return ((Dmac *)hw)->DBGCTRL.reg;
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG0) >> DMAC_SWTRIGCTRL_SWTRIG0_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG0;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG0_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG1) >> DMAC_SWTRIGCTRL_SWTRIG1_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG1;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG1_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG2) >> DMAC_SWTRIGCTRL_SWTRIG2_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG2;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG2_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG3) >> DMAC_SWTRIGCTRL_SWTRIG3_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG3;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG3_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG4_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG4;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG4) >> DMAC_SWTRIGCTRL_SWTRIG4_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG4;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG4_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG4_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG4;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG4_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG4;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG5_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG5;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG5) >> DMAC_SWTRIGCTRL_SWTRIG5_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG5;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG5_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG5_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG5;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG5_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG5;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG6_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG6;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG6) >> DMAC_SWTRIGCTRL_SWTRIG6_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG6;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG6_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG6_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG6;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG6_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG6;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG7_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG7;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG7) >> DMAC_SWTRIGCTRL_SWTRIG7_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG7;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG7_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG7_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG7;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG7_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG7;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG8_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG8;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG8) >> DMAC_SWTRIGCTRL_SWTRIG8_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG8;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG8_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG8_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG8;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG8_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG8;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG9_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG9;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG9) >> DMAC_SWTRIGCTRL_SWTRIG9_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG9;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG9_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG9_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG9;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG9_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG9;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG10_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG10;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG10) >> DMAC_SWTRIGCTRL_SWTRIG10_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG10;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG10_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG10_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG10;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG10_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG10;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG11_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG11;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG11) >> DMAC_SWTRIGCTRL_SWTRIG11_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG11;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG11_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG11_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG11;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG11_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG11;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG12_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG12;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG12) >> DMAC_SWTRIGCTRL_SWTRIG12_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG12;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG12_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG12_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG12;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG12_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG12;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG13_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG13;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG13) >> DMAC_SWTRIGCTRL_SWTRIG13_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG13;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG13_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG13_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG13;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG13_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG13;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG14_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG14;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG14) >> DMAC_SWTRIGCTRL_SWTRIG14_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG14;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG14_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG14_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG14;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG14_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG14;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG15_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG15;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG15) >> DMAC_SWTRIGCTRL_SWTRIG15_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG15;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG15_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG15_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG15;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG15_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG15;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG16_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG16;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG16_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG16) >> DMAC_SWTRIGCTRL_SWTRIG16_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG16_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG16;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG16_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG16_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG16;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG16_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG16;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG17_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG17;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG17_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG17) >> DMAC_SWTRIGCTRL_SWTRIG17_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG17_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG17;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG17_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG17_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG17;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG17_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG17;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG18_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG18;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG18_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG18) >> DMAC_SWTRIGCTRL_SWTRIG18_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG18_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG18;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG18_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG18_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG18;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG18_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG18;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG19_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG19;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG19_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG19) >> DMAC_SWTRIGCTRL_SWTRIG19_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG19_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG19;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG19_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG19_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG19;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG19_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG19;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG20_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG20;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG20_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG20) >> DMAC_SWTRIGCTRL_SWTRIG20_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG20_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG20;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG20_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG20_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG20;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG20_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG20;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG21_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG21;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG21_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG21) >> DMAC_SWTRIGCTRL_SWTRIG21_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG21_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG21;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG21_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG21_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG21;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG21_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG21;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG22_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG22;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG22_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG22) >> DMAC_SWTRIGCTRL_SWTRIG22_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG22_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG22;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG22_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG22_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG22;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG22_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG22;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG23_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG23;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG23_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG23) >> DMAC_SWTRIGCTRL_SWTRIG23_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG23_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG23;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG23_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG23_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG23;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG23_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG23;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG24_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG24;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG24_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG24) >> DMAC_SWTRIGCTRL_SWTRIG24_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG24_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG24;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG24_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG24_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG24;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG24_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG24;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG25_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG25;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG25_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG25) >> DMAC_SWTRIGCTRL_SWTRIG25_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG25_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG25;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG25_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG25_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG25;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG25_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG25;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG26_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG26;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG26_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG26) >> DMAC_SWTRIGCTRL_SWTRIG26_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG26_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG26;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG26_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG26_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG26;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG26_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG26;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG27_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG27;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG27_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG27) >> DMAC_SWTRIGCTRL_SWTRIG27_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG27_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG27;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG27_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG27_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG27;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG27_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG27;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG28_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG28;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG28_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG28) >> DMAC_SWTRIGCTRL_SWTRIG28_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG28_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG28;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG28_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG28_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG28;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG28_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG28;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG29_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG29;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG29_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG29) >> DMAC_SWTRIGCTRL_SWTRIG29_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG29_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG29;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG29_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG29_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG29;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG29_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG29;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG30_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG30;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG30_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG30) >> DMAC_SWTRIGCTRL_SWTRIG30_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG30_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG30;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG30_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG30_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG30;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG30_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG30;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_SWTRIG31_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= DMAC_SWTRIGCTRL_SWTRIG31;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_SWTRIGCTRL_SWTRIG31_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp = (tmp & DMAC_SWTRIGCTRL_SWTRIG31) >> DMAC_SWTRIGCTRL_SWTRIG31_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_SWTRIG31_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= ~DMAC_SWTRIGCTRL_SWTRIG31;
	tmp |= value << DMAC_SWTRIGCTRL_SWTRIG31_Pos;
	((Dmac *)hw)->SWTRIGCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_SWTRIG31_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~DMAC_SWTRIGCTRL_SWTRIG31;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_SWTRIG31_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= DMAC_SWTRIGCTRL_SWTRIG31;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_SWTRIGCTRL_reg(const void *const hw, hri_dmac_swtrigctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_swtrigctrl_reg_t hri_dmac_get_SWTRIGCTRL_reg(const void *const         hw,
                                                                    hri_dmac_swtrigctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->SWTRIGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_SWTRIGCTRL_reg(const void *const hw, hri_dmac_swtrigctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_SWTRIGCTRL_reg(const void *const hw, hri_dmac_swtrigctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_SWTRIGCTRL_reg(const void *const hw, hri_dmac_swtrigctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->SWTRIGCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_swtrigctrl_reg_t hri_dmac_read_SWTRIGCTRL_reg(const void *const hw)
{
	return ((Dmac *)hw)->SWTRIGCTRL.reg;
}

static inline void hri_dmac_set_PRICTRL0_RRLVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_RRLVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_PRICTRL0_RRLVLEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_RRLVLEN0) >> DMAC_PRICTRL0_RRLVLEN0_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_PRICTRL0_RRLVLEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_RRLVLEN0;
	tmp |= value << DMAC_PRICTRL0_RRLVLEN0_Pos;
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_RRLVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_RRLVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_RRLVLEN0_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_RRLVLEN0;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_PRICTRL0_RRLVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_RRLVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_PRICTRL0_RRLVLEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_RRLVLEN1) >> DMAC_PRICTRL0_RRLVLEN1_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_PRICTRL0_RRLVLEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_RRLVLEN1;
	tmp |= value << DMAC_PRICTRL0_RRLVLEN1_Pos;
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_RRLVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_RRLVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_RRLVLEN1_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_RRLVLEN1;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_PRICTRL0_RRLVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_RRLVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_PRICTRL0_RRLVLEN2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_RRLVLEN2) >> DMAC_PRICTRL0_RRLVLEN2_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_PRICTRL0_RRLVLEN2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_RRLVLEN2;
	tmp |= value << DMAC_PRICTRL0_RRLVLEN2_Pos;
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_RRLVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_RRLVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_RRLVLEN2_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_RRLVLEN2;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_PRICTRL0_RRLVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_RRLVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_PRICTRL0_RRLVLEN3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_RRLVLEN3) >> DMAC_PRICTRL0_RRLVLEN3_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_PRICTRL0_RRLVLEN3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_RRLVLEN3;
	tmp |= value << DMAC_PRICTRL0_RRLVLEN3_Pos;
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_RRLVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_RRLVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_RRLVLEN3_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_RRLVLEN3;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_PRICTRL0_LVLPRI0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_LVLPRI0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_LVLPRI0_bf(const void *const       hw,
                                                                       hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI0(mask)) >> DMAC_PRICTRL0_LVLPRI0_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_LVLPRI0_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_LVLPRI0_Msk;
	tmp |= DMAC_PRICTRL0_LVLPRI0(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_LVLPRI0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_LVLPRI0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_LVLPRI0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_LVLPRI0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_LVLPRI0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI0_Msk) >> DMAC_PRICTRL0_LVLPRI0_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_QOS0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_QOS0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_QOS0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS0(mask)) >> DMAC_PRICTRL0_QOS0_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_QOS0_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_QOS0_Msk;
	tmp |= DMAC_PRICTRL0_QOS0(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_QOS0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_QOS0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_QOS0_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_QOS0(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_QOS0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS0_Msk) >> DMAC_PRICTRL0_QOS0_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_LVLPRI1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_LVLPRI1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_LVLPRI1_bf(const void *const       hw,
                                                                       hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI1(mask)) >> DMAC_PRICTRL0_LVLPRI1_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_LVLPRI1_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_LVLPRI1_Msk;
	tmp |= DMAC_PRICTRL0_LVLPRI1(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_LVLPRI1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_LVLPRI1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_LVLPRI1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_LVLPRI1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_LVLPRI1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI1_Msk) >> DMAC_PRICTRL0_LVLPRI1_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_QOS1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_QOS1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_QOS1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS1(mask)) >> DMAC_PRICTRL0_QOS1_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_QOS1_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_QOS1_Msk;
	tmp |= DMAC_PRICTRL0_QOS1(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_QOS1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_QOS1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_QOS1_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_QOS1(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_QOS1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS1_Msk) >> DMAC_PRICTRL0_QOS1_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_LVLPRI2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_LVLPRI2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_LVLPRI2_bf(const void *const       hw,
                                                                       hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI2(mask)) >> DMAC_PRICTRL0_LVLPRI2_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_LVLPRI2_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_LVLPRI2_Msk;
	tmp |= DMAC_PRICTRL0_LVLPRI2(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_LVLPRI2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_LVLPRI2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_LVLPRI2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_LVLPRI2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_LVLPRI2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI2_Msk) >> DMAC_PRICTRL0_LVLPRI2_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_QOS2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_QOS2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_QOS2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS2(mask)) >> DMAC_PRICTRL0_QOS2_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_QOS2_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_QOS2_Msk;
	tmp |= DMAC_PRICTRL0_QOS2(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_QOS2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_QOS2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_QOS2_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_QOS2(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_QOS2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS2_Msk) >> DMAC_PRICTRL0_QOS2_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_LVLPRI3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_LVLPRI3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_LVLPRI3_bf(const void *const       hw,
                                                                       hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI3(mask)) >> DMAC_PRICTRL0_LVLPRI3_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_LVLPRI3_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_LVLPRI3_Msk;
	tmp |= DMAC_PRICTRL0_LVLPRI3(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_LVLPRI3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_LVLPRI3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_LVLPRI3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_LVLPRI3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_LVLPRI3_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_LVLPRI3_Msk) >> DMAC_PRICTRL0_LVLPRI3_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_QOS3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= DMAC_PRICTRL0_QOS3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_QOS3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS3(mask)) >> DMAC_PRICTRL0_QOS3_Pos;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_QOS3_bf(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= ~DMAC_PRICTRL0_QOS3_Msk;
	tmp |= DMAC_PRICTRL0_QOS3(data);
	((Dmac *)hw)->PRICTRL0.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_QOS3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~DMAC_PRICTRL0_QOS3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_QOS3_bf(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= DMAC_PRICTRL0_QOS3(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_QOS3_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp = (tmp & DMAC_PRICTRL0_QOS3_Msk) >> DMAC_PRICTRL0_QOS3_Pos;
	return tmp;
}

static inline void hri_dmac_set_PRICTRL0_reg(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_get_PRICTRL0_reg(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PRICTRL0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_PRICTRL0_reg(const void *const hw, hri_dmac_prictrl0_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_PRICTRL0_reg(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_PRICTRL0_reg(const void *const hw, hri_dmac_prictrl0_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->PRICTRL0.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_prictrl0_reg_t hri_dmac_read_PRICTRL0_reg(const void *const hw)
{
	return ((Dmac *)hw)->PRICTRL0.reg;
}

static inline void hri_dmac_set_INTPEND_TERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_TERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_TERR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_TERR) >> DMAC_INTPEND_TERR_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_TERR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_TERR;
	tmp |= value << DMAC_INTPEND_TERR_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_TERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_TERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_TERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_TERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_TCMPL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_TCMPL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_TCMPL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_TCMPL) >> DMAC_INTPEND_TCMPL_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_TCMPL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_TCMPL;
	tmp |= value << DMAC_INTPEND_TCMPL_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_TCMPL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_TCMPL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_TCMPL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_TCMPL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_SUSP_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_SUSP;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_SUSP_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_SUSP) >> DMAC_INTPEND_SUSP_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_SUSP_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_SUSP;
	tmp |= value << DMAC_INTPEND_SUSP_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_SUSP_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_SUSP;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_SUSP_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_SUSP;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_CRCERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_CRCERR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_CRCERR) >> DMAC_INTPEND_CRCERR_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_CRCERR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_CRCERR;
	tmp |= value << DMAC_INTPEND_CRCERR_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_CRCERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_CRCERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_FERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_FERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_FERR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_FERR) >> DMAC_INTPEND_FERR_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_FERR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_FERR;
	tmp |= value << DMAC_INTPEND_FERR_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_FERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_FERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_FERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_FERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_BUSY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_BUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_BUSY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_BUSY) >> DMAC_INTPEND_BUSY_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_BUSY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_BUSY;
	tmp |= value << DMAC_INTPEND_BUSY_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_BUSY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_BUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_BUSY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_BUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_PEND_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_PEND;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_INTPEND_PEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_PEND) >> DMAC_INTPEND_PEND_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_INTPEND_PEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_PEND;
	tmp |= value << DMAC_INTPEND_PEND_Pos;
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_PEND_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_PEND;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_PEND_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_PEND;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_INTPEND_ID_bf(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= DMAC_INTPEND_ID(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_intpend_reg_t hri_dmac_get_INTPEND_ID_bf(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_ID(mask)) >> DMAC_INTPEND_ID_Pos;
	return tmp;
}

static inline void hri_dmac_write_INTPEND_ID_bf(const void *const hw, hri_dmac_intpend_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= ~DMAC_INTPEND_ID_Msk;
	tmp |= DMAC_INTPEND_ID(data);
	((Dmac *)hw)->INTPEND.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_ID_bf(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~DMAC_INTPEND_ID(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_ID_bf(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= DMAC_INTPEND_ID(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_intpend_reg_t hri_dmac_read_INTPEND_ID_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp = (tmp & DMAC_INTPEND_ID_Msk) >> DMAC_INTPEND_ID_Pos;
	return tmp;
}

static inline void hri_dmac_set_INTPEND_reg(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_intpend_reg_t hri_dmac_get_INTPEND_reg(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Dmac *)hw)->INTPEND.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_INTPEND_reg(const void *const hw, hri_dmac_intpend_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_INTPEND_reg(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_INTPEND_reg(const void *const hw, hri_dmac_intpend_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->INTPEND.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_intpend_reg_t hri_dmac_read_INTPEND_reg(const void *const hw)
{
	return ((Dmac *)hw)->INTPEND.reg;
}

static inline void hri_dmac_set_BASEADDR_BASEADDR_bf(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg |= DMAC_BASEADDR_BASEADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_baseaddr_reg_t hri_dmac_get_BASEADDR_BASEADDR_bf(const void *const       hw,
                                                                        hri_dmac_baseaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->BASEADDR.reg;
	tmp = (tmp & DMAC_BASEADDR_BASEADDR(mask)) >> DMAC_BASEADDR_BASEADDR_Pos;
	return tmp;
}

static inline void hri_dmac_write_BASEADDR_BASEADDR_bf(const void *const hw, hri_dmac_baseaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->BASEADDR.reg;
	tmp &= ~DMAC_BASEADDR_BASEADDR_Msk;
	tmp |= DMAC_BASEADDR_BASEADDR(data);
	((Dmac *)hw)->BASEADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_BASEADDR_BASEADDR_bf(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg &= ~DMAC_BASEADDR_BASEADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_BASEADDR_BASEADDR_bf(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg ^= DMAC_BASEADDR_BASEADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_baseaddr_reg_t hri_dmac_read_BASEADDR_BASEADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->BASEADDR.reg;
	tmp = (tmp & DMAC_BASEADDR_BASEADDR_Msk) >> DMAC_BASEADDR_BASEADDR_Pos;
	return tmp;
}

static inline void hri_dmac_set_BASEADDR_reg(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_baseaddr_reg_t hri_dmac_get_BASEADDR_reg(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->BASEADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_BASEADDR_reg(const void *const hw, hri_dmac_baseaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_BASEADDR_reg(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_BASEADDR_reg(const void *const hw, hri_dmac_baseaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->BASEADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_baseaddr_reg_t hri_dmac_read_BASEADDR_reg(const void *const hw)
{
	return ((Dmac *)hw)->BASEADDR.reg;
}

static inline void hri_dmac_set_WRBADDR_WRBADDR_bf(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg |= DMAC_WRBADDR_WRBADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_wrbaddr_reg_t hri_dmac_get_WRBADDR_WRBADDR_bf(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->WRBADDR.reg;
	tmp = (tmp & DMAC_WRBADDR_WRBADDR(mask)) >> DMAC_WRBADDR_WRBADDR_Pos;
	return tmp;
}

static inline void hri_dmac_write_WRBADDR_WRBADDR_bf(const void *const hw, hri_dmac_wrbaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->WRBADDR.reg;
	tmp &= ~DMAC_WRBADDR_WRBADDR_Msk;
	tmp |= DMAC_WRBADDR_WRBADDR(data);
	((Dmac *)hw)->WRBADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_WRBADDR_WRBADDR_bf(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg &= ~DMAC_WRBADDR_WRBADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_WRBADDR_WRBADDR_bf(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg ^= DMAC_WRBADDR_WRBADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_wrbaddr_reg_t hri_dmac_read_WRBADDR_WRBADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->WRBADDR.reg;
	tmp = (tmp & DMAC_WRBADDR_WRBADDR_Msk) >> DMAC_WRBADDR_WRBADDR_Pos;
	return tmp;
}

static inline void hri_dmac_set_WRBADDR_reg(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_wrbaddr_reg_t hri_dmac_get_WRBADDR_reg(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->WRBADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_WRBADDR_reg(const void *const hw, hri_dmac_wrbaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_WRBADDR_reg(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_WRBADDR_reg(const void *const hw, hri_dmac_wrbaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->WRBADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_wrbaddr_reg_t hri_dmac_read_WRBADDR_reg(const void *const hw)
{
	return ((Dmac *)hw)->WRBADDR.reg;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT0_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT0) >> DMAC_INTSTATUS_CHINT0_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT1_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT1) >> DMAC_INTSTATUS_CHINT1_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT2_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT2) >> DMAC_INTSTATUS_CHINT2_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT3_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT3) >> DMAC_INTSTATUS_CHINT3_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT4_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT4) >> DMAC_INTSTATUS_CHINT4_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT5_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT5) >> DMAC_INTSTATUS_CHINT5_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT6_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT6) >> DMAC_INTSTATUS_CHINT6_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT7_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT7) >> DMAC_INTSTATUS_CHINT7_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT8_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT8) >> DMAC_INTSTATUS_CHINT8_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT9_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT9) >> DMAC_INTSTATUS_CHINT9_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT10_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT10) >> DMAC_INTSTATUS_CHINT10_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT11_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT11) >> DMAC_INTSTATUS_CHINT11_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT12_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT12) >> DMAC_INTSTATUS_CHINT12_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT13_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT13) >> DMAC_INTSTATUS_CHINT13_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT14_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT14) >> DMAC_INTSTATUS_CHINT14_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT15_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT15) >> DMAC_INTSTATUS_CHINT15_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT16_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT16) >> DMAC_INTSTATUS_CHINT16_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT17_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT17) >> DMAC_INTSTATUS_CHINT17_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT18_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT18) >> DMAC_INTSTATUS_CHINT18_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT19_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT19) >> DMAC_INTSTATUS_CHINT19_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT20_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT20) >> DMAC_INTSTATUS_CHINT20_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT21_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT21) >> DMAC_INTSTATUS_CHINT21_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT22_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT22) >> DMAC_INTSTATUS_CHINT22_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT23_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT23) >> DMAC_INTSTATUS_CHINT23_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT24_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT24) >> DMAC_INTSTATUS_CHINT24_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT25_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT25) >> DMAC_INTSTATUS_CHINT25_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT26_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT26) >> DMAC_INTSTATUS_CHINT26_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT27_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT27) >> DMAC_INTSTATUS_CHINT27_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT28_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT28) >> DMAC_INTSTATUS_CHINT28_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT29_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT29) >> DMAC_INTSTATUS_CHINT29_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT30_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT30) >> DMAC_INTSTATUS_CHINT30_Pos;
}

static inline bool hri_dmac_get_INTSTATUS_CHINT31_bit(const void *const hw)
{
	return (((Dmac *)hw)->INTSTATUS.reg & DMAC_INTSTATUS_CHINT31) >> DMAC_INTSTATUS_CHINT31_Pos;
}

static inline hri_dmac_intstatus_reg_t hri_dmac_get_INTSTATUS_reg(const void *const hw, hri_dmac_intstatus_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->INTSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_intstatus_reg_t hri_dmac_read_INTSTATUS_reg(const void *const hw)
{
	return ((Dmac *)hw)->INTSTATUS.reg;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH0_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH0) >> DMAC_BUSYCH_BUSYCH0_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH1_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH1) >> DMAC_BUSYCH_BUSYCH1_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH2_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH2) >> DMAC_BUSYCH_BUSYCH2_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH3_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH3) >> DMAC_BUSYCH_BUSYCH3_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH4_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH4) >> DMAC_BUSYCH_BUSYCH4_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH5_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH5) >> DMAC_BUSYCH_BUSYCH5_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH6_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH6) >> DMAC_BUSYCH_BUSYCH6_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH7_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH7) >> DMAC_BUSYCH_BUSYCH7_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH8_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH8) >> DMAC_BUSYCH_BUSYCH8_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH9_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH9) >> DMAC_BUSYCH_BUSYCH9_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH10_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH10) >> DMAC_BUSYCH_BUSYCH10_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH11_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH11) >> DMAC_BUSYCH_BUSYCH11_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH12_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH12) >> DMAC_BUSYCH_BUSYCH12_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH13_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH13) >> DMAC_BUSYCH_BUSYCH13_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH14_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH14) >> DMAC_BUSYCH_BUSYCH14_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH15_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH15) >> DMAC_BUSYCH_BUSYCH15_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH16_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH16) >> DMAC_BUSYCH_BUSYCH16_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH17_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH17) >> DMAC_BUSYCH_BUSYCH17_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH18_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH18) >> DMAC_BUSYCH_BUSYCH18_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH19_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH19) >> DMAC_BUSYCH_BUSYCH19_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH20_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH20) >> DMAC_BUSYCH_BUSYCH20_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH21_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH21) >> DMAC_BUSYCH_BUSYCH21_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH22_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH22) >> DMAC_BUSYCH_BUSYCH22_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH23_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH23) >> DMAC_BUSYCH_BUSYCH23_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH24_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH24) >> DMAC_BUSYCH_BUSYCH24_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH25_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH25) >> DMAC_BUSYCH_BUSYCH25_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH26_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH26) >> DMAC_BUSYCH_BUSYCH26_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH27_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH27) >> DMAC_BUSYCH_BUSYCH27_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH28_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH28) >> DMAC_BUSYCH_BUSYCH28_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH29_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH29) >> DMAC_BUSYCH_BUSYCH29_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH30_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH30) >> DMAC_BUSYCH_BUSYCH30_Pos;
}

static inline bool hri_dmac_get_BUSYCH_BUSYCH31_bit(const void *const hw)
{
	return (((Dmac *)hw)->BUSYCH.reg & DMAC_BUSYCH_BUSYCH31) >> DMAC_BUSYCH_BUSYCH31_Pos;
}

static inline hri_dmac_busych_reg_t hri_dmac_get_BUSYCH_reg(const void *const hw, hri_dmac_busych_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->BUSYCH.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_busych_reg_t hri_dmac_read_BUSYCH_reg(const void *const hw)
{
	return ((Dmac *)hw)->BUSYCH.reg;
}

static inline bool hri_dmac_get_PENDCH_PENDCH0_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH0) >> DMAC_PENDCH_PENDCH0_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH1_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH1) >> DMAC_PENDCH_PENDCH1_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH2_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH2) >> DMAC_PENDCH_PENDCH2_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH3_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH3) >> DMAC_PENDCH_PENDCH3_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH4_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH4) >> DMAC_PENDCH_PENDCH4_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH5_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH5) >> DMAC_PENDCH_PENDCH5_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH6_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH6) >> DMAC_PENDCH_PENDCH6_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH7_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH7) >> DMAC_PENDCH_PENDCH7_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH8_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH8) >> DMAC_PENDCH_PENDCH8_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH9_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH9) >> DMAC_PENDCH_PENDCH9_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH10_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH10) >> DMAC_PENDCH_PENDCH10_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH11_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH11) >> DMAC_PENDCH_PENDCH11_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH12_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH12) >> DMAC_PENDCH_PENDCH12_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH13_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH13) >> DMAC_PENDCH_PENDCH13_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH14_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH14) >> DMAC_PENDCH_PENDCH14_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH15_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH15) >> DMAC_PENDCH_PENDCH15_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH16_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH16) >> DMAC_PENDCH_PENDCH16_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH17_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH17) >> DMAC_PENDCH_PENDCH17_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH18_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH18) >> DMAC_PENDCH_PENDCH18_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH19_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH19) >> DMAC_PENDCH_PENDCH19_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH20_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH20) >> DMAC_PENDCH_PENDCH20_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH21_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH21) >> DMAC_PENDCH_PENDCH21_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH22_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH22) >> DMAC_PENDCH_PENDCH22_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH23_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH23) >> DMAC_PENDCH_PENDCH23_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH24_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH24) >> DMAC_PENDCH_PENDCH24_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH25_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH25) >> DMAC_PENDCH_PENDCH25_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH26_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH26) >> DMAC_PENDCH_PENDCH26_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH27_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH27) >> DMAC_PENDCH_PENDCH27_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH28_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH28) >> DMAC_PENDCH_PENDCH28_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH29_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH29) >> DMAC_PENDCH_PENDCH29_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH30_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH30) >> DMAC_PENDCH_PENDCH30_Pos;
}

static inline bool hri_dmac_get_PENDCH_PENDCH31_bit(const void *const hw)
{
	return (((Dmac *)hw)->PENDCH.reg & DMAC_PENDCH_PENDCH31) >> DMAC_PENDCH_PENDCH31_Pos;
}

static inline hri_dmac_pendch_reg_t hri_dmac_get_PENDCH_reg(const void *const hw, hri_dmac_pendch_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->PENDCH.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_pendch_reg_t hri_dmac_read_PENDCH_reg(const void *const hw)
{
	return ((Dmac *)hw)->PENDCH.reg;
}

static inline bool hri_dmac_get_ACTIVE_LVLEX0_bit(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_LVLEX0) >> DMAC_ACTIVE_LVLEX0_Pos;
}

static inline bool hri_dmac_get_ACTIVE_LVLEX1_bit(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_LVLEX1) >> DMAC_ACTIVE_LVLEX1_Pos;
}

static inline bool hri_dmac_get_ACTIVE_LVLEX2_bit(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_LVLEX2) >> DMAC_ACTIVE_LVLEX2_Pos;
}

static inline bool hri_dmac_get_ACTIVE_LVLEX3_bit(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_LVLEX3) >> DMAC_ACTIVE_LVLEX3_Pos;
}

static inline bool hri_dmac_get_ACTIVE_ABUSY_bit(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_ABUSY) >> DMAC_ACTIVE_ABUSY_Pos;
}

static inline hri_dmac_active_reg_t hri_dmac_get_ACTIVE_ID_bf(const void *const hw, hri_dmac_active_reg_t mask)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_ID(mask)) >> DMAC_ACTIVE_ID_Pos;
}

static inline hri_dmac_active_reg_t hri_dmac_read_ACTIVE_ID_bf(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_ID_Msk) >> DMAC_ACTIVE_ID_Pos;
}

static inline hri_dmac_active_reg_t hri_dmac_get_ACTIVE_BTCNT_bf(const void *const hw, hri_dmac_active_reg_t mask)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_BTCNT(mask)) >> DMAC_ACTIVE_BTCNT_Pos;
}

static inline hri_dmac_active_reg_t hri_dmac_read_ACTIVE_BTCNT_bf(const void *const hw)
{
	return (((Dmac *)hw)->ACTIVE.reg & DMAC_ACTIVE_BTCNT_Msk) >> DMAC_ACTIVE_BTCNT_Pos;
}

static inline hri_dmac_active_reg_t hri_dmac_get_ACTIVE_reg(const void *const hw, hri_dmac_active_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->ACTIVE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_active_reg_t hri_dmac_read_ACTIVE_reg(const void *const hw)
{
	return ((Dmac *)hw)->ACTIVE.reg;
}

static inline bool hri_dmac_get_CRCSTATUS_CRCBUSY_bit(const void *const hw)
{
	return (((Dmac *)hw)->CRCSTATUS.reg & DMAC_CRCSTATUS_CRCBUSY) >> DMAC_CRCSTATUS_CRCBUSY_Pos;
}

static inline void hri_dmac_clear_CRCSTATUS_CRCBUSY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCSTATUS.reg = DMAC_CRCSTATUS_CRCBUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CRCSTATUS_CRCZERO_bit(const void *const hw)
{
	return (((Dmac *)hw)->CRCSTATUS.reg & DMAC_CRCSTATUS_CRCZERO) >> DMAC_CRCSTATUS_CRCZERO_Pos;
}

static inline void hri_dmac_clear_CRCSTATUS_CRCZERO_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCSTATUS.reg = DMAC_CRCSTATUS_CRCZERO;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CRCSTATUS_CRCERR_bit(const void *const hw)
{
	return (((Dmac *)hw)->CRCSTATUS.reg & DMAC_CRCSTATUS_CRCERR) >> DMAC_CRCSTATUS_CRCERR_Pos;
}

static inline void hri_dmac_clear_CRCSTATUS_CRCERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCSTATUS.reg = DMAC_CRCSTATUS_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcstatus_reg_t hri_dmac_get_CRCSTATUS_reg(const void *const hw, hri_dmac_crcstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->CRCSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_clear_CRCSTATUS_reg(const void *const hw, hri_dmac_crcstatus_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->CRCSTATUS.reg = mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_crcstatus_reg_t hri_dmac_read_CRCSTATUS_reg(const void *const hw)
{
	return ((Dmac *)hw)->CRCSTATUS.reg;
}

static inline void hri_dmacdescriptor_set_DSTADDR_CRC_CHKINIT_bf(const void *const                hw,
                                                                 hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg |= DMAC_DSTADDR_CRC_CHKINIT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t
hri_dmacdescriptor_get_DSTADDR_CRC_CHKINIT_bf(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp = (tmp & DMAC_DSTADDR_CRC_CHKINIT(mask)) >> DMAC_DSTADDR_CRC_CHKINIT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DSTADDR_CRC_CHKINIT_bf(const void *const                hw,
                                                                   hri_dmacdescriptor_dstaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp &= ~DMAC_DSTADDR_CRC_CHKINIT_Msk;
	tmp |= DMAC_DSTADDR_CRC_CHKINIT(data);
	((DmacDescriptor *)hw)->DSTADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DSTADDR_CRC_CHKINIT_bf(const void *const                hw,
                                                                   hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg &= ~DMAC_DSTADDR_CRC_CHKINIT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DSTADDR_CRC_CHKINIT_bf(const void *const                hw,
                                                                    hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg ^= DMAC_DSTADDR_CRC_CHKINIT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t hri_dmacdescriptor_read_DSTADDR_CRC_CHKINIT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp = (tmp & DMAC_DSTADDR_CRC_CHKINIT_Msk) >> DMAC_DSTADDR_CRC_CHKINIT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_DSTADDR_DSTADDR_bf(const void *const                hw,
                                                             hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg |= DMAC_DSTADDR_DSTADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t
hri_dmacdescriptor_get_DSTADDR_DSTADDR_bf(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp = (tmp & DMAC_DSTADDR_DSTADDR(mask)) >> DMAC_DSTADDR_DSTADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DSTADDR_DSTADDR_bf(const void *const                hw,
                                                               hri_dmacdescriptor_dstaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp &= ~DMAC_DSTADDR_DSTADDR_Msk;
	tmp |= DMAC_DSTADDR_DSTADDR(data);
	((DmacDescriptor *)hw)->DSTADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DSTADDR_DSTADDR_bf(const void *const                hw,
                                                               hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg &= ~DMAC_DSTADDR_DSTADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DSTADDR_DSTADDR_bf(const void *const                hw,
                                                                hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg ^= DMAC_DSTADDR_DSTADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t hri_dmacdescriptor_read_DSTADDR_DSTADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp = (tmp & DMAC_DSTADDR_DSTADDR_Msk) >> DMAC_DSTADDR_DSTADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_DSTADDR_CRC_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t
hri_dmacdescriptor_get_DSTADDR_CRC_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DSTADDR_CRC_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DSTADDR_CRC_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DSTADDR_CRC_reg(const void *const                hw,
                                                             hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t hri_dmacdescriptor_read_DSTADDR_CRC_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->DSTADDR.reg;
}

static inline void hri_dmacdescriptor_set_DSTADDR_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t hri_dmacdescriptor_get_DSTADDR_reg(const void *const                hw,
                                                                                  hri_dmacdescriptor_dstaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DSTADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DSTADDR_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DSTADDR_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DSTADDR_reg(const void *const hw, hri_dmacdescriptor_dstaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DSTADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_dstaddr_reg_t hri_dmacdescriptor_read_DSTADDR_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->DSTADDR.reg;
}

static inline void hri_dmacdescriptor_set_BTCTRL_VALID_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_VALID;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacdescriptor_get_BTCTRL_VALID_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_VALID) >> DMAC_BTCTRL_VALID_Pos;
	return (bool)tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_VALID_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_VALID;
	tmp |= value << DMAC_BTCTRL_VALID_Pos;
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_VALID_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_VALID;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_VALID_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_VALID;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_set_BTCTRL_SRCINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_SRCINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacdescriptor_get_BTCTRL_SRCINC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_SRCINC) >> DMAC_BTCTRL_SRCINC_Pos;
	return (bool)tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_SRCINC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_SRCINC;
	tmp |= value << DMAC_BTCTRL_SRCINC_Pos;
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_SRCINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_SRCINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_SRCINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_SRCINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_set_BTCTRL_DSTINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_DSTINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacdescriptor_get_BTCTRL_DSTINC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_DSTINC) >> DMAC_BTCTRL_DSTINC_Pos;
	return (bool)tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_DSTINC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_DSTINC;
	tmp |= value << DMAC_BTCTRL_DSTINC_Pos;
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_DSTINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_DSTINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_DSTINC_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_DSTINC;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_set_BTCTRL_STEPSEL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_STEPSEL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacdescriptor_get_BTCTRL_STEPSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_STEPSEL) >> DMAC_BTCTRL_STEPSEL_Pos;
	return (bool)tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_STEPSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_STEPSEL;
	tmp |= value << DMAC_BTCTRL_STEPSEL_Pos;
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_STEPSEL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_STEPSEL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_STEPSEL_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_STEPSEL;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_set_BTCTRL_EVOSEL_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_EVOSEL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t
hri_dmacdescriptor_get_BTCTRL_EVOSEL_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_EVOSEL(mask)) >> DMAC_BTCTRL_EVOSEL_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_EVOSEL_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_EVOSEL_Msk;
	tmp |= DMAC_BTCTRL_EVOSEL(data);
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_EVOSEL_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_EVOSEL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_EVOSEL_bf(const void *const               hw,
                                                              hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_EVOSEL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_read_BTCTRL_EVOSEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_EVOSEL_Msk) >> DMAC_BTCTRL_EVOSEL_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_BTCTRL_BLOCKACT_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_BLOCKACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t
hri_dmacdescriptor_get_BTCTRL_BLOCKACT_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_BLOCKACT(mask)) >> DMAC_BTCTRL_BLOCKACT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_BLOCKACT_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_BLOCKACT_Msk;
	tmp |= DMAC_BTCTRL_BLOCKACT(data);
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_BLOCKACT_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_BLOCKACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_BLOCKACT_bf(const void *const               hw,
                                                                hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_BLOCKACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_read_BTCTRL_BLOCKACT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_BLOCKACT_Msk) >> DMAC_BTCTRL_BLOCKACT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_BTCTRL_BEATSIZE_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_BEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t
hri_dmacdescriptor_get_BTCTRL_BEATSIZE_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_BEATSIZE(mask)) >> DMAC_BTCTRL_BEATSIZE_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_BEATSIZE_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_BEATSIZE_Msk;
	tmp |= DMAC_BTCTRL_BEATSIZE(data);
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_BEATSIZE_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_BEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_BEATSIZE_bf(const void *const               hw,
                                                                hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_BEATSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_read_BTCTRL_BEATSIZE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_BEATSIZE_Msk) >> DMAC_BTCTRL_BEATSIZE_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_BTCTRL_STEPSIZE_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= DMAC_BTCTRL_STEPSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t
hri_dmacdescriptor_get_BTCTRL_STEPSIZE_bf(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_STEPSIZE(mask)) >> DMAC_BTCTRL_STEPSIZE_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_STEPSIZE_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= ~DMAC_BTCTRL_STEPSIZE_Msk;
	tmp |= DMAC_BTCTRL_STEPSIZE(data);
	((DmacDescriptor *)hw)->BTCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_STEPSIZE_bf(const void *const               hw,
                                                               hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~DMAC_BTCTRL_STEPSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_STEPSIZE_bf(const void *const               hw,
                                                                hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= DMAC_BTCTRL_STEPSIZE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_read_BTCTRL_STEPSIZE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp = (tmp & DMAC_BTCTRL_STEPSIZE_Msk) >> DMAC_BTCTRL_STEPSIZE_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_BTCTRL_reg(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_get_BTCTRL_reg(const void *const               hw,
                                                                                hri_dmacdescriptor_btctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCTRL_reg(const void *const hw, hri_dmacdescriptor_btctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCTRL_reg(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCTRL_reg(const void *const hw, hri_dmacdescriptor_btctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btctrl_reg_t hri_dmacdescriptor_read_BTCTRL_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->BTCTRL.reg;
}

static inline void hri_dmacdescriptor_set_BTCNT_BTCNT_bf(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg |= DMAC_BTCNT_BTCNT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btcnt_reg_t hri_dmacdescriptor_get_BTCNT_BTCNT_bf(const void *const              hw,
                                                                                   hri_dmacdescriptor_btcnt_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCNT.reg;
	tmp = (tmp & DMAC_BTCNT_BTCNT(mask)) >> DMAC_BTCNT_BTCNT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCNT_BTCNT_bf(const void *const hw, hri_dmacdescriptor_btcnt_reg_t data)
{
	uint16_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->BTCNT.reg;
	tmp &= ~DMAC_BTCNT_BTCNT_Msk;
	tmp |= DMAC_BTCNT_BTCNT(data);
	((DmacDescriptor *)hw)->BTCNT.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCNT_BTCNT_bf(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg &= ~DMAC_BTCNT_BTCNT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCNT_BTCNT_bf(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg ^= DMAC_BTCNT_BTCNT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btcnt_reg_t hri_dmacdescriptor_read_BTCNT_BTCNT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCNT.reg;
	tmp = (tmp & DMAC_BTCNT_BTCNT_Msk) >> DMAC_BTCNT_BTCNT_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_BTCNT_reg(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btcnt_reg_t hri_dmacdescriptor_get_BTCNT_reg(const void *const              hw,
                                                                              hri_dmacdescriptor_btcnt_reg_t mask)
{
	uint16_t tmp;
	tmp = ((DmacDescriptor *)hw)->BTCNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_BTCNT_reg(const void *const hw, hri_dmacdescriptor_btcnt_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_BTCNT_reg(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_BTCNT_reg(const void *const hw, hri_dmacdescriptor_btcnt_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->BTCNT.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_btcnt_reg_t hri_dmacdescriptor_read_BTCNT_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->BTCNT.reg;
}

static inline void hri_dmacdescriptor_set_SRCADDR_SRCADDR_bf(const void *const                hw,
                                                             hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg |= DMAC_SRCADDR_SRCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_srcaddr_reg_t
hri_dmacdescriptor_get_SRCADDR_SRCADDR_bf(const void *const hw, hri_dmacdescriptor_srcaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->SRCADDR.reg;
	tmp = (tmp & DMAC_SRCADDR_SRCADDR(mask)) >> DMAC_SRCADDR_SRCADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_SRCADDR_SRCADDR_bf(const void *const                hw,
                                                               hri_dmacdescriptor_srcaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->SRCADDR.reg;
	tmp &= ~DMAC_SRCADDR_SRCADDR_Msk;
	tmp |= DMAC_SRCADDR_SRCADDR(data);
	((DmacDescriptor *)hw)->SRCADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_SRCADDR_SRCADDR_bf(const void *const                hw,
                                                               hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg &= ~DMAC_SRCADDR_SRCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_SRCADDR_SRCADDR_bf(const void *const                hw,
                                                                hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg ^= DMAC_SRCADDR_SRCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_srcaddr_reg_t hri_dmacdescriptor_read_SRCADDR_SRCADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->SRCADDR.reg;
	tmp = (tmp & DMAC_SRCADDR_SRCADDR_Msk) >> DMAC_SRCADDR_SRCADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_SRCADDR_reg(const void *const hw, hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_srcaddr_reg_t hri_dmacdescriptor_get_SRCADDR_reg(const void *const                hw,
                                                                                  hri_dmacdescriptor_srcaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->SRCADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_SRCADDR_reg(const void *const hw, hri_dmacdescriptor_srcaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_SRCADDR_reg(const void *const hw, hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_SRCADDR_reg(const void *const hw, hri_dmacdescriptor_srcaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->SRCADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_srcaddr_reg_t hri_dmacdescriptor_read_SRCADDR_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->SRCADDR.reg;
}

static inline void hri_dmacdescriptor_set_DESCADDR_DESCADDR_bf(const void *const                 hw,
                                                               hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg |= DMAC_DESCADDR_DESCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_descaddr_reg_t
hri_dmacdescriptor_get_DESCADDR_DESCADDR_bf(const void *const hw, hri_dmacdescriptor_descaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DESCADDR.reg;
	tmp = (tmp & DMAC_DESCADDR_DESCADDR(mask)) >> DMAC_DESCADDR_DESCADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DESCADDR_DESCADDR_bf(const void *const                 hw,
                                                                 hri_dmacdescriptor_descaddr_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacDescriptor *)hw)->DESCADDR.reg;
	tmp &= ~DMAC_DESCADDR_DESCADDR_Msk;
	tmp |= DMAC_DESCADDR_DESCADDR(data);
	((DmacDescriptor *)hw)->DESCADDR.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DESCADDR_DESCADDR_bf(const void *const                 hw,
                                                                 hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg &= ~DMAC_DESCADDR_DESCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DESCADDR_DESCADDR_bf(const void *const                 hw,
                                                                  hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg ^= DMAC_DESCADDR_DESCADDR(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_descaddr_reg_t hri_dmacdescriptor_read_DESCADDR_DESCADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DESCADDR.reg;
	tmp = (tmp & DMAC_DESCADDR_DESCADDR_Msk) >> DMAC_DESCADDR_DESCADDR_Pos;
	return tmp;
}

static inline void hri_dmacdescriptor_set_DESCADDR_reg(const void *const hw, hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_descaddr_reg_t
hri_dmacdescriptor_get_DESCADDR_reg(const void *const hw, hri_dmacdescriptor_descaddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacDescriptor *)hw)->DESCADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacdescriptor_write_DESCADDR_reg(const void *const hw, hri_dmacdescriptor_descaddr_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_clear_DESCADDR_reg(const void *const hw, hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacdescriptor_toggle_DESCADDR_reg(const void *const hw, hri_dmacdescriptor_descaddr_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacDescriptor *)hw)->DESCADDR.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmacdescriptor_descaddr_reg_t hri_dmacdescriptor_read_DESCADDR_reg(const void *const hw)
{
	return ((DmacDescriptor *)hw)->DESCADDR.reg;
}

static inline void hri_dmacchannel_set_CHINTEN_TERR_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_TERR;
}

static inline bool hri_dmacchannel_get_CHINTEN_TERR_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTENSET.reg & DMAC_CHINTENSET_TERR) >> DMAC_CHINTENSET_TERR_Pos;
}

static inline void hri_dmacchannel_write_CHINTEN_TERR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_TERR;
	} else {
		((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_TERR;
	}
}

static inline void hri_dmacchannel_clear_CHINTEN_TERR_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_TERR;
}

static inline void hri_dmacchannel_set_CHINTEN_TCMPL_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
}

static inline bool hri_dmacchannel_get_CHINTEN_TCMPL_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTENSET.reg & DMAC_CHINTENSET_TCMPL) >> DMAC_CHINTENSET_TCMPL_Pos;
}

static inline void hri_dmacchannel_write_CHINTEN_TCMPL_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL;
	} else {
		((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
	}
}

static inline void hri_dmacchannel_clear_CHINTEN_TCMPL_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL;
}

static inline void hri_dmacchannel_set_CHINTEN_SUSP_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_SUSP;
}

static inline bool hri_dmacchannel_get_CHINTEN_SUSP_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTENSET.reg & DMAC_CHINTENSET_SUSP) >> DMAC_CHINTENSET_SUSP_Pos;
}

static inline void hri_dmacchannel_write_CHINTEN_SUSP_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_SUSP;
	} else {
		((DmacChannel *)hw)->CHINTENSET.reg = DMAC_CHINTENSET_SUSP;
	}
}

static inline void hri_dmacchannel_clear_CHINTEN_SUSP_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTENCLR.reg = DMAC_CHINTENSET_SUSP;
}

static inline void hri_dmacchannel_set_CHINTEN_reg(const void *const hw, hri_dmac_chintenset_reg_t mask)
{
	((DmacChannel *)hw)->CHINTENSET.reg = mask;
}

static inline hri_dmac_chintenset_reg_t hri_dmacchannel_get_CHINTEN_reg(const void *const         hw,
                                                                        hri_dmac_chintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_chintenset_reg_t hri_dmacchannel_read_CHINTEN_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHINTENSET.reg;
}

static inline void hri_dmacchannel_write_CHINTEN_reg(const void *const hw, hri_dmac_chintenset_reg_t data)
{
	((DmacChannel *)hw)->CHINTENSET.reg = data;
	((DmacChannel *)hw)->CHINTENCLR.reg = ~data;
}

static inline void hri_dmacchannel_clear_CHINTEN_reg(const void *const hw, hri_dmac_chintenset_reg_t mask)
{
	((DmacChannel *)hw)->CHINTENCLR.reg = mask;
}

static inline bool hri_dmacchannel_get_CHINTFLAG_TERR_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_TERR) >> DMAC_CHINTFLAG_TERR_Pos;
}

static inline void hri_dmacchannel_clear_CHINTFLAG_TERR_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_TERR;
}

static inline bool hri_dmacchannel_get_CHINTFLAG_TCMPL_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL) >> DMAC_CHINTFLAG_TCMPL_Pos;
}

static inline void hri_dmacchannel_clear_CHINTFLAG_TCMPL_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
}

static inline bool hri_dmacchannel_get_CHINTFLAG_SUSP_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_SUSP) >> DMAC_CHINTFLAG_SUSP_Pos;
}

static inline void hri_dmacchannel_clear_CHINTFLAG_SUSP_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_SUSP;
}

static inline bool hri_dmacchannel_get_interrupt_TERR_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_TERR) >> DMAC_CHINTFLAG_TERR_Pos;
}

static inline void hri_dmacchannel_clear_interrupt_TERR_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_TERR;
}

static inline bool hri_dmacchannel_get_interrupt_TCMPL_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL) >> DMAC_CHINTFLAG_TCMPL_Pos;
}

static inline void hri_dmacchannel_clear_interrupt_TCMPL_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
}

static inline bool hri_dmacchannel_get_interrupt_SUSP_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHINTFLAG.reg & DMAC_CHINTFLAG_SUSP) >> DMAC_CHINTFLAG_SUSP_Pos;
}

static inline void hri_dmacchannel_clear_interrupt_SUSP_bit(const void *const hw)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = DMAC_CHINTFLAG_SUSP;
}

static inline hri_dmac_chintflag_reg_t hri_dmacchannel_get_CHINTFLAG_reg(const void *const        hw,
                                                                         hri_dmac_chintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_chintflag_reg_t hri_dmacchannel_read_CHINTFLAG_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHINTFLAG.reg;
}

static inline void hri_dmacchannel_clear_CHINTFLAG_reg(const void *const hw, hri_dmac_chintflag_reg_t mask)
{
	((DmacChannel *)hw)->CHINTFLAG.reg = mask;
}

static inline void hri_dmacchannel_set_CHCTRLA_SWRST_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_SWRST;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHCTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_SWRST) >> DMAC_CHCTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_dmacchannel_set_CHCTRLA_ENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHCTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_ENABLE) >> DMAC_CHCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_ENABLE;
	tmp |= value << DMAC_CHCTRLA_ENABLE_Pos;
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_ENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_ENABLE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_set_CHCTRLA_RUNSTDBY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHCTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_RUNSTDBY) >> DMAC_CHCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_RUNSTDBY;
	tmp |= value << DMAC_CHCTRLA_RUNSTDBY_Pos;
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_RUNSTDBY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_RUNSTDBY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_set_CHCTRLA_TRIGSRC_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_get_CHCTRLA_TRIGSRC_bf(const void *const      hw,
                                                                            hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGSRC(mask)) >> DMAC_CHCTRLA_TRIGSRC_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_TRIGSRC_bf(const void *const hw, hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_TRIGSRC_Msk;
	tmp |= DMAC_CHCTRLA_TRIGSRC(data);
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_TRIGSRC_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_TRIGSRC_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_read_CHCTRLA_TRIGSRC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGSRC_Msk) >> DMAC_CHCTRLA_TRIGSRC_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHCTRLA_TRIGACT_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_get_CHCTRLA_TRIGACT_bf(const void *const      hw,
                                                                            hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGACT(mask)) >> DMAC_CHCTRLA_TRIGACT_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_TRIGACT_bf(const void *const hw, hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_TRIGACT_Msk;
	tmp |= DMAC_CHCTRLA_TRIGACT(data);
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_TRIGACT_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_TRIGACT_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_read_CHCTRLA_TRIGACT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGACT_Msk) >> DMAC_CHCTRLA_TRIGACT_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHCTRLA_BURSTLEN_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_get_CHCTRLA_BURSTLEN_bf(const void *const      hw,
                                                                             hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_BURSTLEN(mask)) >> DMAC_CHCTRLA_BURSTLEN_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_BURSTLEN_bf(const void *const hw, hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_BURSTLEN_Msk;
	tmp |= DMAC_CHCTRLA_BURSTLEN(data);
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_BURSTLEN_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_BURSTLEN_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_read_CHCTRLA_BURSTLEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_BURSTLEN_Msk) >> DMAC_CHCTRLA_BURSTLEN_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHCTRLA_THRESHOLD_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_get_CHCTRLA_THRESHOLD_bf(const void *const      hw,
                                                                              hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_THRESHOLD(mask)) >> DMAC_CHCTRLA_THRESHOLD_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_THRESHOLD_bf(const void *const hw, hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_THRESHOLD_Msk;
	tmp |= DMAC_CHCTRLA_THRESHOLD(data);
	((DmacChannel *)hw)->CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_THRESHOLD_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_THRESHOLD_bf(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_read_CHCTRLA_THRESHOLD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_THRESHOLD_Msk) >> DMAC_CHCTRLA_THRESHOLD_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHCTRLA_reg(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_get_CHCTRLA_reg(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLA_reg(const void *const hw, hri_dmac_chctrla_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLA_reg(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLA_reg(const void *const hw, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLA.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmacchannel_read_CHCTRLA_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHCTRLA.reg;
}

static inline void hri_dmacchannel_set_CHCTRLB_CMD_bf(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg |= DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmacchannel_get_CHCTRLB_CMD_bf(const void *const      hw,
                                                                        hri_dmac_chctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLB.reg;
	tmp = (tmp & DMAC_CHCTRLB_CMD(mask)) >> DMAC_CHCTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLB_CMD_bf(const void *const hw, hri_dmac_chctrlb_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHCTRLB.reg;
	tmp &= ~DMAC_CHCTRLB_CMD_Msk;
	tmp |= DMAC_CHCTRLB_CMD(data);
	((DmacChannel *)hw)->CHCTRLB.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLB_CMD_bf(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg &= ~DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLB_CMD_bf(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg ^= DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmacchannel_read_CHCTRLB_CMD_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLB.reg;
	tmp = (tmp & DMAC_CHCTRLB_CMD_Msk) >> DMAC_CHCTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHCTRLB_reg(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmacchannel_get_CHCTRLB_reg(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacchannel_write_CHCTRLB_reg(const void *const hw, hri_dmac_chctrlb_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHCTRLB_reg(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHCTRLB_reg(const void *const hw, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHCTRLB.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmacchannel_read_CHCTRLB_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHCTRLB.reg;
}

static inline void hri_dmacchannel_set_CHPRILVL_PRILVL_bf(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg |= DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmacchannel_get_CHPRILVL_PRILVL_bf(const void *const       hw,
                                                                             hri_dmac_chprilvl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHPRILVL.reg;
	tmp = (tmp & DMAC_CHPRILVL_PRILVL(mask)) >> DMAC_CHPRILVL_PRILVL_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHPRILVL_PRILVL_bf(const void *const hw, hri_dmac_chprilvl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHPRILVL.reg;
	tmp &= ~DMAC_CHPRILVL_PRILVL_Msk;
	tmp |= DMAC_CHPRILVL_PRILVL(data);
	((DmacChannel *)hw)->CHPRILVL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHPRILVL_PRILVL_bf(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg &= ~DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHPRILVL_PRILVL_bf(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg ^= DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmacchannel_read_CHPRILVL_PRILVL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHPRILVL.reg;
	tmp = (tmp & DMAC_CHPRILVL_PRILVL_Msk) >> DMAC_CHPRILVL_PRILVL_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHPRILVL_reg(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmacchannel_get_CHPRILVL_reg(const void *const       hw,
                                                                       hri_dmac_chprilvl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHPRILVL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacchannel_write_CHPRILVL_reg(const void *const hw, hri_dmac_chprilvl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHPRILVL_reg(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHPRILVL_reg(const void *const hw, hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHPRILVL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmacchannel_read_CHPRILVL_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHPRILVL.reg;
}

static inline void hri_dmacchannel_set_CHEVCTRL_EVIE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg |= DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHEVCTRL_EVIE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVIE) >> DMAC_CHEVCTRL_EVIE_Pos;
	return (bool)tmp;
}

static inline void hri_dmacchannel_write_CHEVCTRL_EVIE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVIE;
	tmp |= value << DMAC_CHEVCTRL_EVIE_Pos;
	((DmacChannel *)hw)->CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHEVCTRL_EVIE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHEVCTRL_EVIE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_set_CHEVCTRL_EVOE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg |= DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHEVCTRL_EVOE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOE) >> DMAC_CHEVCTRL_EVOE_Pos;
	return (bool)tmp;
}

static inline void hri_dmacchannel_write_CHEVCTRL_EVOE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVOE;
	tmp |= value << DMAC_CHEVCTRL_EVOE_Pos;
	((DmacChannel *)hw)->CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHEVCTRL_EVOE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHEVCTRL_EVOE_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_set_CHEVCTRL_EVACT_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg |= DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_get_CHEVCTRL_EVACT_bf(const void *const       hw,
                                                                            hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVACT(mask)) >> DMAC_CHEVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHEVCTRL_EVACT_bf(const void *const hw, hri_dmac_chevctrl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVACT_Msk;
	tmp |= DMAC_CHEVCTRL_EVACT(data);
	((DmacChannel *)hw)->CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHEVCTRL_EVACT_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHEVCTRL_EVACT_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_read_CHEVCTRL_EVACT_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVACT_Msk) >> DMAC_CHEVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHEVCTRL_EVOMODE_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg |= DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_get_CHEVCTRL_EVOMODE_bf(const void *const       hw,
                                                                              hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOMODE(mask)) >> DMAC_CHEVCTRL_EVOMODE_Pos;
	return tmp;
}

static inline void hri_dmacchannel_write_CHEVCTRL_EVOMODE_bf(const void *const hw, hri_dmac_chevctrl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVOMODE_Msk;
	tmp |= DMAC_CHEVCTRL_EVOMODE(data);
	((DmacChannel *)hw)->CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHEVCTRL_EVOMODE_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHEVCTRL_EVOMODE_bf(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_read_CHEVCTRL_EVOMODE_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOMODE_Msk) >> DMAC_CHEVCTRL_EVOMODE_Pos;
	return tmp;
}

static inline void hri_dmacchannel_set_CHEVCTRL_reg(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_get_CHEVCTRL_reg(const void *const       hw,
                                                                       hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHEVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacchannel_write_CHEVCTRL_reg(const void *const hw, hri_dmac_chevctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_clear_CHEVCTRL_reg(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmacchannel_toggle_CHEVCTRL_reg(const void *const hw, hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHEVCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmacchannel_read_CHEVCTRL_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHEVCTRL.reg;
}

static inline bool hri_dmacchannel_get_CHSTATUS_PEND_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHSTATUS.reg & DMAC_CHSTATUS_PEND) >> DMAC_CHSTATUS_PEND_Pos;
}

static inline void hri_dmacchannel_clear_CHSTATUS_PEND_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHSTATUS.reg = DMAC_CHSTATUS_PEND;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHSTATUS_BUSY_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHSTATUS.reg & DMAC_CHSTATUS_BUSY) >> DMAC_CHSTATUS_BUSY_Pos;
}

static inline void hri_dmacchannel_clear_CHSTATUS_BUSY_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHSTATUS.reg = DMAC_CHSTATUS_BUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHSTATUS_FERR_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHSTATUS.reg & DMAC_CHSTATUS_FERR) >> DMAC_CHSTATUS_FERR_Pos;
}

static inline void hri_dmacchannel_clear_CHSTATUS_FERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHSTATUS.reg = DMAC_CHSTATUS_FERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmacchannel_get_CHSTATUS_CRCERR_bit(const void *const hw)
{
	return (((DmacChannel *)hw)->CHSTATUS.reg & DMAC_CHSTATUS_CRCERR) >> DMAC_CHSTATUS_CRCERR_Pos;
}

static inline void hri_dmacchannel_clear_CHSTATUS_CRCERR_bit(const void *const hw)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHSTATUS.reg = DMAC_CHSTATUS_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chstatus_reg_t hri_dmacchannel_get_CHSTATUS_reg(const void *const       hw,
                                                                       hri_dmac_chstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((DmacChannel *)hw)->CHSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmacchannel_clear_CHSTATUS_reg(const void *const hw, hri_dmac_chstatus_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((DmacChannel *)hw)->CHSTATUS.reg = mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chstatus_reg_t hri_dmacchannel_read_CHSTATUS_reg(const void *const hw)
{
	return ((DmacChannel *)hw)->CHSTATUS.reg;
}

static inline void hri_dmac_set_CHINTEN_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_TERR;
}

static inline bool hri_dmac_get_CHINTEN_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg & DMAC_CHINTENSET_TERR) >> DMAC_CHINTENSET_TERR_Pos;
}

static inline void hri_dmac_write_CHINTEN_TERR_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_TERR;
	} else {
		((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_TERR;
	}
}

static inline void hri_dmac_clear_CHINTEN_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_TERR;
}

static inline void hri_dmac_set_CHINTEN_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
}

static inline bool hri_dmac_get_CHINTEN_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg & DMAC_CHINTENSET_TCMPL) >> DMAC_CHINTENSET_TCMPL_Pos;
}

static inline void hri_dmac_write_CHINTEN_TCMPL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL;
	} else {
		((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
	}
}

static inline void hri_dmac_clear_CHINTEN_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL;
}

static inline void hri_dmac_set_CHINTEN_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;
}

static inline bool hri_dmac_get_CHINTEN_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg & DMAC_CHINTENSET_SUSP) >> DMAC_CHINTENSET_SUSP_Pos;
}

static inline void hri_dmac_write_CHINTEN_SUSP_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_SUSP;
	} else {
		((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;
	}
}

static inline void hri_dmac_clear_CHINTEN_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = DMAC_CHINTENSET_SUSP;
}

static inline void hri_dmac_set_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                            hri_dmac_chintenset_reg_t mask)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = mask;
}

static inline hri_dmac_chintenset_reg_t hri_dmac_get_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                                 hri_dmac_chintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_chintenset_reg_t hri_dmac_read_CHINTEN_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg;
}

static inline void hri_dmac_write_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chintenset_reg_t data)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENSET.reg = data;
	((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = ~data;
}

static inline void hri_dmac_clear_CHINTEN_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chintenset_reg_t mask)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTENCLR.reg = mask;
}

static inline bool hri_dmac_get_CHINTFLAG_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_TERR) >> DMAC_CHINTFLAG_TERR_Pos;
}

static inline void hri_dmac_clear_CHINTFLAG_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_TERR;
}

static inline bool hri_dmac_get_CHINTFLAG_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL) >> DMAC_CHINTFLAG_TCMPL_Pos;
}

static inline void hri_dmac_clear_CHINTFLAG_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
}

static inline bool hri_dmac_get_CHINTFLAG_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_SUSP) >> DMAC_CHINTFLAG_SUSP_Pos;
}

static inline void hri_dmac_clear_CHINTFLAG_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_SUSP;
}

static inline bool hri_dmac_get_interrupt_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_TERR) >> DMAC_CHINTFLAG_TERR_Pos;
}

static inline void hri_dmac_clear_interrupt_TERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_TERR;
}

static inline bool hri_dmac_get_interrupt_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL) >> DMAC_CHINTFLAG_TCMPL_Pos;
}

static inline void hri_dmac_clear_interrupt_TCMPL_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
}

static inline bool hri_dmac_get_interrupt_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg & DMAC_CHINTFLAG_SUSP) >> DMAC_CHINTFLAG_SUSP_Pos;
}

static inline void hri_dmac_clear_interrupt_SUSP_bit(const void *const hw, uint8_t submodule_index)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = DMAC_CHINTFLAG_SUSP;
}

static inline hri_dmac_chintflag_reg_t hri_dmac_get_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                                  hri_dmac_chintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dmac_chintflag_reg_t hri_dmac_read_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg;
}

static inline void hri_dmac_clear_CHINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                hri_dmac_chintflag_reg_t mask)
{
	((Dmac *)hw)->Channel[submodule_index].CHINTFLAG.reg = mask;
}

static inline void hri_dmac_set_CHCTRLA_SWRST_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_SWRST;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHCTRLA_SWRST_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_SWRST) >> DMAC_CHCTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_set_CHCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_ENABLE) >> DMAC_CHCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CHCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_ENABLE;
	tmp |= value << DMAC_CHCTRLA_ENABLE_Pos;
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_ENABLE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CHCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_RUNSTDBY) >> DMAC_CHCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CHCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_RUNSTDBY;
	tmp |= value << DMAC_CHCTRLA_RUNSTDBY_Pos;
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_RUNSTDBY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_get_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGSRC(mask)) >> DMAC_CHCTRLA_TRIGSRC_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_TRIGSRC_Msk;
	tmp |= DMAC_CHCTRLA_TRIGSRC(data);
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_TRIGSRC(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_read_CHCTRLA_TRIGSRC_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGSRC_Msk) >> DMAC_CHCTRLA_TRIGSRC_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_get_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGACT(mask)) >> DMAC_CHCTRLA_TRIGACT_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_TRIGACT_Msk;
	tmp |= DMAC_CHCTRLA_TRIGACT(data);
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_TRIGACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_read_CHCTRLA_TRIGACT_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_TRIGACT_Msk) >> DMAC_CHCTRLA_TRIGACT_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_get_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_BURSTLEN(mask)) >> DMAC_CHCTRLA_BURSTLEN_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_BURSTLEN_Msk;
	tmp |= DMAC_CHCTRLA_BURSTLEN(data);
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_BURSTLEN(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_read_CHCTRLA_BURSTLEN_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_BURSTLEN_Msk) >> DMAC_CHCTRLA_BURSTLEN_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_get_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_THRESHOLD(mask)) >> DMAC_CHCTRLA_THRESHOLD_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_dmac_chctrla_reg_t data)
{
	uint32_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= ~DMAC_CHCTRLA_THRESHOLD_Msk;
	tmp |= DMAC_CHCTRLA_THRESHOLD(data);
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= DMAC_CHCTRLA_THRESHOLD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_read_CHCTRLA_THRESHOLD_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp = (tmp & DMAC_CHCTRLA_THRESHOLD_Msk) >> DMAC_CHCTRLA_THRESHOLD_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHCTRLA_reg(const void *const hw, uint8_t submodule_index, hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_get_CHCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_dmac_chctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chctrla_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chctrla_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrla_reg_t hri_dmac_read_CHCTRLA_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHCTRLA.reg;
}

static inline void hri_dmac_set_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg |= DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmac_get_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_dmac_chctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg;
	tmp = (tmp & DMAC_CHCTRLB_CMD(mask)) >> DMAC_CHCTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_dmac_chctrlb_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg;
	tmp &= ~DMAC_CHCTRLB_CMD_Msk;
	tmp |= DMAC_CHCTRLB_CMD(data);
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg &= ~DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg ^= DMAC_CHCTRLB_CMD(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmac_read_CHCTRLB_CMD_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg;
	tmp = (tmp & DMAC_CHCTRLB_CMD_Msk) >> DMAC_CHCTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHCTRLB_reg(const void *const hw, uint8_t submodule_index, hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmac_get_CHCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_dmac_chctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CHCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chctrlb_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                              hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chctrlb_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chctrlb_reg_t hri_dmac_read_CHCTRLB_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHCTRLB.reg;
}

static inline void hri_dmac_set_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg |= DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmac_get_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_dmac_chprilvl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg;
	tmp = (tmp & DMAC_CHPRILVL_PRILVL(mask)) >> DMAC_CHPRILVL_PRILVL_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chprilvl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg;
	tmp &= ~DMAC_CHPRILVL_PRILVL_Msk;
	tmp |= DMAC_CHPRILVL_PRILVL(data);
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg &= ~DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg ^= DMAC_CHPRILVL_PRILVL(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmac_read_CHPRILVL_PRILVL_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg;
	tmp = (tmp & DMAC_CHPRILVL_PRILVL_Msk) >> DMAC_CHPRILVL_PRILVL_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHPRILVL_reg(const void *const hw, uint8_t submodule_index,
                                             hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmac_get_CHPRILVL_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_dmac_chprilvl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CHPRILVL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chprilvl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHPRILVL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHPRILVL_reg(const void *const hw, uint8_t submodule_index,
                                                hri_dmac_chprilvl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chprilvl_reg_t hri_dmac_read_CHPRILVL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHPRILVL.reg;
}

static inline void hri_dmac_set_CHEVCTRL_EVIE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg |= DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHEVCTRL_EVIE_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVIE) >> DMAC_CHEVCTRL_EVIE_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CHEVCTRL_EVIE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVIE;
	tmp |= value << DMAC_CHEVCTRL_EVIE_Pos;
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHEVCTRL_EVIE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHEVCTRL_EVIE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVIE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CHEVCTRL_EVOE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg |= DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHEVCTRL_EVOE_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOE) >> DMAC_CHEVCTRL_EVOE_Pos;
	return (bool)tmp;
}

static inline void hri_dmac_write_CHEVCTRL_EVOE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVOE;
	tmp |= value << DMAC_CHEVCTRL_EVOE_Pos;
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHEVCTRL_EVOE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHEVCTRL_EVOE_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVOE;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_set_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg |= DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_get_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVACT(mask)) >> DMAC_CHEVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_dmac_chevctrl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVACT_Msk;
	tmp |= DMAC_CHEVCTRL_EVACT(data);
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVACT(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_read_CHEVCTRL_EVACT_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVACT_Msk) >> DMAC_CHEVCTRL_EVACT_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg |= DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_get_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOMODE(mask)) >> DMAC_CHEVCTRL_EVOMODE_Pos;
	return tmp;
}

static inline void hri_dmac_write_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chevctrl_reg_t data)
{
	uint8_t tmp;
	DMAC_CRITICAL_SECTION_ENTER();
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp &= ~DMAC_CHEVCTRL_EVOMODE_Msk;
	tmp |= DMAC_CHEVCTRL_EVOMODE(data);
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg = tmp;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg &= ~DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg ^= DMAC_CHEVCTRL_EVOMODE(mask);
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_read_CHEVCTRL_EVOMODE_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp = (tmp & DMAC_CHEVCTRL_EVOMODE_Msk) >> DMAC_CHEVCTRL_EVOMODE_Pos;
	return tmp;
}

static inline void hri_dmac_set_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                             hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg |= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_get_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_dmac_chevctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_write_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chevctrl_reg_t data)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg = data;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_clear_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg &= ~mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dmac_toggle_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                                hri_dmac_chevctrl_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg ^= mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chevctrl_reg_t hri_dmac_read_CHEVCTRL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHEVCTRL.reg;
}

static inline bool hri_dmac_get_CHSTATUS_PEND_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg & DMAC_CHSTATUS_PEND) >> DMAC_CHSTATUS_PEND_Pos;
}

static inline void hri_dmac_clear_CHSTATUS_PEND_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg = DMAC_CHSTATUS_PEND;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHSTATUS_BUSY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg & DMAC_CHSTATUS_BUSY) >> DMAC_CHSTATUS_BUSY_Pos;
}

static inline void hri_dmac_clear_CHSTATUS_BUSY_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg = DMAC_CHSTATUS_BUSY;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHSTATUS_FERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg & DMAC_CHSTATUS_FERR) >> DMAC_CHSTATUS_FERR_Pos;
}

static inline void hri_dmac_clear_CHSTATUS_FERR_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg = DMAC_CHSTATUS_FERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dmac_get_CHSTATUS_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg & DMAC_CHSTATUS_CRCERR) >> DMAC_CHSTATUS_CRCERR_Pos;
}

static inline void hri_dmac_clear_CHSTATUS_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg = DMAC_CHSTATUS_CRCERR;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chstatus_reg_t hri_dmac_get_CHSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_dmac_chstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dmac_clear_CHSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                               hri_dmac_chstatus_reg_t mask)
{
	DMAC_CRITICAL_SECTION_ENTER();
	((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg = mask;
	DMAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_dmac_chstatus_reg_t hri_dmac_read_CHSTATUS_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Dmac *)hw)->Channel[submodule_index].CHSTATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_DMAC_D51_H_INCLUDED */
#endif /* _SAMD51_DMAC_COMPONENT_ */
