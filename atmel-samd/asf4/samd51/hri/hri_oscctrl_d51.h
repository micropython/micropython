/**
 * \file
 *
 * \brief SAM OSCCTRL
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

#ifdef _SAMD51_OSCCTRL_COMPONENT_
#ifndef _HRI_OSCCTRL_D51_H_INCLUDED_
#define _HRI_OSCCTRL_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_OSCCTRL_CRITICAL_SECTIONS)
#define OSCCTRL_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define OSCCTRL_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define OSCCTRL_CRITICAL_SECTION_ENTER()
#define OSCCTRL_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_oscctrl_dfllmul_reg_t;
typedef uint32_t hri_oscctrl_dfllval_reg_t;
typedef uint32_t hri_oscctrl_dpllctrlb_reg_t;
typedef uint32_t hri_oscctrl_dpllratio_reg_t;
typedef uint32_t hri_oscctrl_dpllstatus_reg_t;
typedef uint32_t hri_oscctrl_dpllsyncbusy_reg_t;
typedef uint32_t hri_oscctrl_intenset_reg_t;
typedef uint32_t hri_oscctrl_intflag_reg_t;
typedef uint32_t hri_oscctrl_status_reg_t;
typedef uint32_t hri_oscctrl_xoscctrl_reg_t;
typedef uint32_t hri_oscctrldpll_dpllctrlb_reg_t;
typedef uint32_t hri_oscctrldpll_dpllratio_reg_t;
typedef uint32_t hri_oscctrldpll_dpllstatus_reg_t;
typedef uint32_t hri_oscctrldpll_dpllsyncbusy_reg_t;
typedef uint8_t  hri_oscctrl_dfllctrla_reg_t;
typedef uint8_t  hri_oscctrl_dfllctrlb_reg_t;
typedef uint8_t  hri_oscctrl_dfllsync_reg_t;
typedef uint8_t  hri_oscctrl_dpllctrla_reg_t;
typedef uint8_t  hri_oscctrl_evctrl_reg_t;
typedef uint8_t  hri_oscctrldpll_dpllctrla_reg_t;

static inline void hri_oscctrldpll_set_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_ENABLE) >> OSCCTRL_DPLLCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_ENABLE;
	tmp |= value << OSCCTRL_DPLLCTRLA_ENABLE_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_RUNSTDBY) >> OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_RUNSTDBY;
	tmp |= value << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_ONDEMAND) >> OSCCTRL_DPLLCTRLA_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLA_ONDEMAND_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_ONDEMAND;
	tmp |= value << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLA_reg(const void *const hw, hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrla_reg_t hri_oscctrldpll_get_DPLLCTRLA_reg(const void *const           hw,
                                                                            hri_oscctrl_dpllctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLA_reg(const void *const hw, hri_oscctrl_dpllctrla_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLA_reg(const void *const hw, hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLA_reg(const void *const hw, hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLA.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrla_reg_t hri_oscctrldpll_read_DPLLCTRLA_reg(const void *const hw)
{
	return ((OscctrlDpll *)hw)->DPLLCTRLA.reg;
}

static inline void hri_oscctrldpll_set_DPLLRATIO_LDR_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg |= OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_get_DPLLRATIO_LDR_bf(const void *const           hw,
                                                                               hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDR(mask)) >> OSCCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLRATIO_LDR_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp &= ~OSCCTRL_DPLLRATIO_LDR_Msk;
	tmp |= OSCCTRL_DPLLRATIO_LDR(data);
	((OscctrlDpll *)hw)->DPLLRATIO.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLRATIO_LDR_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg &= ~OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLRATIO_LDR_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg ^= OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_read_DPLLRATIO_LDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDR_Msk) >> OSCCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg |= OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_get_DPLLRATIO_LDRFRAC_bf(const void *const           hw,
                                                                                   hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDRFRAC(mask)) >> OSCCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp &= ~OSCCTRL_DPLLRATIO_LDRFRAC_Msk;
	tmp |= OSCCTRL_DPLLRATIO_LDRFRAC(data);
	((OscctrlDpll *)hw)->DPLLRATIO.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg &= ~OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg ^= OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_read_DPLLRATIO_LDRFRAC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDRFRAC_Msk) >> OSCCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLRATIO_reg(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_get_DPLLRATIO_reg(const void *const           hw,
                                                                            hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLRATIO.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLRATIO_reg(const void *const hw, hri_oscctrl_dpllratio_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLRATIO_reg(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLRATIO_reg(const void *const hw, hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLRATIO.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrldpll_read_DPLLRATIO_reg(const void *const hw)
{
	return ((OscctrlDpll *)hw)->DPLLRATIO.reg;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_WUF_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLB_WUF_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_WUF) >> OSCCTRL_DPLLCTRLB_WUF_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_WUF_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_WUF;
	tmp |= value << OSCCTRL_DPLLCTRLB_WUF_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_WUF_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_WUF_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LBYPASS) >> OSCCTRL_DPLLCTRLB_LBYPASS_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_LBYPASS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_LBYPASS;
	tmp |= value << OSCCTRL_DPLLCTRLB_LBYPASS_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_DCOEN_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrldpll_get_DPLLCTRLB_DCOEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOEN) >> OSCCTRL_DPLLCTRLB_DCOEN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_DCOEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DCOEN;
	tmp |= value << OSCCTRL_DPLLCTRLB_DCOEN_Pos;
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_DCOEN_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_DCOEN_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_FILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_FILTER_bf(const void *const           hw,
                                                                                  hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_FILTER(mask)) >> OSCCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_FILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_FILTER_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_FILTER(data);
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_FILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_FILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_FILTER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_FILTER_Msk) >> OSCCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_REFCLK_bf(const void *const           hw,
                                                                                  hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_REFCLK(mask)) >> OSCCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_REFCLK_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_REFCLK(data);
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_REFCLK_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_REFCLK_Msk) >> OSCCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_LTIME_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_LTIME_bf(const void *const           hw,
                                                                                 hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LTIME(mask)) >> OSCCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_LTIME_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_LTIME_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_LTIME(data);
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_LTIME_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_LTIME_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_LTIME_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LTIME_Msk) >> OSCCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_DCOFILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_DCOFILTER_bf(const void *const           hw,
                                                                                     hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOFILTER(mask)) >> OSCCTRL_DPLLCTRLB_DCOFILTER_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_DCOFILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DCOFILTER_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_DCOFILTER(data);
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_DCOFILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_DCOFILTER_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_DCOFILTER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOFILTER_Msk) >> OSCCTRL_DPLLCTRLB_DCOFILTER_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_DIV_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_DIV_bf(const void *const           hw,
                                                                               hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DIV(mask)) >> OSCCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_DIV_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DIV_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_DIV(data);
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_DIV_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_DIV_bf(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_DIV_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DIV_Msk) >> OSCCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_oscctrldpll_set_DPLLCTRLB_reg(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_get_DPLLCTRLB_reg(const void *const           hw,
                                                                            hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrldpll_write_DPLLCTRLB_reg(const void *const hw, hri_oscctrl_dpllctrlb_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_clear_DPLLCTRLB_reg(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrldpll_toggle_DPLLCTRLB_reg(const void *const hw, hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((OscctrlDpll *)hw)->DPLLCTRLB.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrldpll_read_DPLLCTRLB_reg(const void *const hw)
{
	return ((OscctrlDpll *)hw)->DPLLCTRLB.reg;
}

static inline bool hri_oscctrldpll_get_DPLLSYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((OscctrlDpll *)hw)->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_ENABLE) >> OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos;
}

static inline bool hri_oscctrldpll_get_DPLLSYNCBUSY_DPLLRATIO_bit(const void *const hw)
{
	return (((OscctrlDpll *)hw)->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO)
	       >> OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos;
}

static inline hri_oscctrl_dpllsyncbusy_reg_t hri_oscctrldpll_get_DPLLSYNCBUSY_reg(const void *const              hw,
                                                                                  hri_oscctrl_dpllsyncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLSYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_dpllsyncbusy_reg_t hri_oscctrldpll_read_DPLLSYNCBUSY_reg(const void *const hw)
{
	return ((OscctrlDpll *)hw)->DPLLSYNCBUSY.reg;
}

static inline bool hri_oscctrldpll_get_DPLLSTATUS_LOCK_bit(const void *const hw)
{
	return (((OscctrlDpll *)hw)->DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_LOCK) >> OSCCTRL_DPLLSTATUS_LOCK_Pos;
}

static inline bool hri_oscctrldpll_get_DPLLSTATUS_CLKRDY_bit(const void *const hw)
{
	return (((OscctrlDpll *)hw)->DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_CLKRDY) >> OSCCTRL_DPLLSTATUS_CLKRDY_Pos;
}

static inline hri_oscctrl_dpllstatus_reg_t hri_oscctrldpll_get_DPLLSTATUS_reg(const void *const            hw,
                                                                              hri_oscctrl_dpllstatus_reg_t mask)
{
	uint32_t tmp;
	tmp = ((OscctrlDpll *)hw)->DPLLSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_dpllstatus_reg_t hri_oscctrldpll_read_DPLLSTATUS_reg(const void *const hw)
{
	return ((OscctrlDpll *)hw)->DPLLSTATUS.reg;
}

static inline void hri_oscctrl_set_DPLLCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_ENABLE) >> OSCCTRL_DPLLCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_ENABLE;
	tmp |= value << OSCCTRL_DPLLCTRLA_ENABLE_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLA_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_RUNSTDBY) >> OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_RUNSTDBY;
	tmp |= value << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLA_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg |= OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLA_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLA_ONDEMAND) >> OSCCTRL_DPLLCTRLA_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLA_ONDEMAND_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp &= ~OSCCTRL_DPLLCTRLA_ONDEMAND;
	tmp |= value << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLA_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg &= ~OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLA_ONDEMAND_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg ^= OSCCTRL_DPLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrla_reg_t hri_oscctrl_get_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_oscctrl_dpllctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllctrla_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_oscctrl_dpllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrla_reg_t hri_oscctrl_read_DPLLCTRLA_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLA.reg;
}

static inline void hri_oscctrl_set_DPLLRATIO_LDR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg |= OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t
hri_oscctrl_get_DPLLRATIO_LDR_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDR(mask)) >> OSCCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLRATIO_LDR_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_oscctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp &= ~OSCCTRL_DPLLRATIO_LDR_Msk;
	tmp |= OSCCTRL_DPLLRATIO_LDR(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLRATIO_LDR_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg &= ~OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLRATIO_LDR_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg ^= OSCCTRL_DPLLRATIO_LDR(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrl_read_DPLLRATIO_LDR_bf(const void *const hw,
                                                                            uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDR_Msk) >> OSCCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLRATIO_LDRFRAC_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg |= OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t
hri_oscctrl_get_DPLLRATIO_LDRFRAC_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDRFRAC(mask)) >> OSCCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLRATIO_LDRFRAC_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_oscctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp &= ~OSCCTRL_DPLLRATIO_LDRFRAC_Msk;
	tmp |= OSCCTRL_DPLLRATIO_LDRFRAC(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLRATIO_LDRFRAC_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg &= ~OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLRATIO_LDRFRAC_bf(const void *const hw, uint8_t submodule_index,
                                                           hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg ^= OSCCTRL_DPLLRATIO_LDRFRAC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrl_read_DPLLRATIO_LDRFRAC_bf(const void *const hw,
                                                                                uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp = (tmp & OSCCTRL_DPLLRATIO_LDRFRAC_Msk) >> OSCCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrl_get_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_oscctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllratio_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_oscctrl_dpllratio_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllratio_reg_t hri_oscctrl_read_DPLLRATIO_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Oscctrl *)hw)->Dpll[submodule_index].DPLLRATIO.reg;
}

static inline void hri_oscctrl_set_DPLLCTRLB_WUF_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLB_WUF_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_WUF) >> OSCCTRL_DPLLCTRLB_WUF_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_WUF_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_WUF;
	tmp |= value << OSCCTRL_DPLLCTRLB_WUF_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_WUF_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_WUF_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_WUF;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLB_LBYPASS_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLB_LBYPASS_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LBYPASS) >> OSCCTRL_DPLLCTRLB_LBYPASS_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_LBYPASS_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_LBYPASS;
	tmp |= value << OSCCTRL_DPLLCTRLB_LBYPASS_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_LBYPASS_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_LBYPASS_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_LBYPASS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLB_DCOEN_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DPLLCTRLB_DCOEN_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOEN) >> OSCCTRL_DPLLCTRLB_DCOEN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_DCOEN_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DCOEN;
	tmp |= value << OSCCTRL_DPLLCTRLB_DCOEN_Pos;
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_DCOEN_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_DCOEN_bit(const void *const hw, uint8_t submodule_index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DCOEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DPLLCTRLB_FILTER_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t
hri_oscctrl_get_DPLLCTRLB_FILTER_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_FILTER(mask)) >> OSCCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_FILTER_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_FILTER_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_FILTER(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_FILTER_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_FILTER_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_FILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_FILTER_bf(const void *const hw,
                                                                               uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_FILTER_Msk) >> OSCCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLCTRLB_REFCLK_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t
hri_oscctrl_get_DPLLCTRLB_REFCLK_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_REFCLK(mask)) >> OSCCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_REFCLK_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_REFCLK_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_REFCLK(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_REFCLK_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_REFCLK_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_REFCLK(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_REFCLK_bf(const void *const hw,
                                                                               uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_REFCLK_Msk) >> OSCCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLCTRLB_LTIME_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t
hri_oscctrl_get_DPLLCTRLB_LTIME_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LTIME(mask)) >> OSCCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_LTIME_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_LTIME_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_LTIME(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_LTIME_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_LTIME_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_LTIME(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_LTIME_bf(const void *const hw,
                                                                              uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_LTIME_Msk) >> OSCCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLCTRLB_DCOFILTER_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t
hri_oscctrl_get_DPLLCTRLB_DCOFILTER_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOFILTER(mask)) >> OSCCTRL_DPLLCTRLB_DCOFILTER_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_DCOFILTER_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DCOFILTER_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_DCOFILTER(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_DCOFILTER_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_DCOFILTER_bf(const void *const hw, uint8_t submodule_index,
                                                             hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DCOFILTER(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_DCOFILTER_bf(const void *const hw,
                                                                                  uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DCOFILTER_Msk) >> OSCCTRL_DPLLCTRLB_DCOFILTER_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLCTRLB_DIV_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t
hri_oscctrl_get_DPLLCTRLB_DIV_bf(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DIV(mask)) >> OSCCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_DIV_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_oscctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= ~OSCCTRL_DPLLCTRLB_DIV_Msk;
	tmp |= OSCCTRL_DPLLCTRLB_DIV(data);
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_DIV_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_DIV_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= OSCCTRL_DPLLCTRLB_DIV(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_DIV_bf(const void *const hw,
                                                                            uint8_t           submodule_index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DPLLCTRLB_DIV_Msk) >> OSCCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_get_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_oscctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllctrlb_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_oscctrl_dpllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dpllctrlb_reg_t hri_oscctrl_read_DPLLCTRLB_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Oscctrl *)hw)->Dpll[submodule_index].DPLLCTRLB.reg;
}

static inline bool hri_oscctrl_get_DPLLSYNCBUSY_ENABLE_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Oscctrl *)hw)->Dpll[submodule_index].DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_ENABLE)
	       >> OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos;
}

static inline bool hri_oscctrl_get_DPLLSYNCBUSY_DPLLRATIO_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Oscctrl *)hw)->Dpll[submodule_index].DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO)
	       >> OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos;
}

static inline hri_oscctrl_dpllsyncbusy_reg_t
hri_oscctrl_get_DPLLSYNCBUSY_reg(const void *const hw, uint8_t submodule_index, hri_oscctrl_dpllsyncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLSYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_dpllsyncbusy_reg_t hri_oscctrl_read_DPLLSYNCBUSY_reg(const void *const hw,
                                                                               uint8_t           submodule_index)
{
	return ((Oscctrl *)hw)->Dpll[submodule_index].DPLLSYNCBUSY.reg;
}

static inline bool hri_oscctrl_get_DPLLSTATUS_LOCK_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Oscctrl *)hw)->Dpll[submodule_index].DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_LOCK)
	       >> OSCCTRL_DPLLSTATUS_LOCK_Pos;
}

static inline bool hri_oscctrl_get_DPLLSTATUS_CLKRDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Oscctrl *)hw)->Dpll[submodule_index].DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_CLKRDY)
	       >> OSCCTRL_DPLLSTATUS_CLKRDY_Pos;
}

static inline hri_oscctrl_dpllstatus_reg_t hri_oscctrl_get_DPLLSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                          hri_oscctrl_dpllstatus_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->Dpll[submodule_index].DPLLSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_dpllstatus_reg_t hri_oscctrl_read_DPLLSTATUS_reg(const void *const hw,
                                                                           uint8_t           submodule_index)
{
	return ((Oscctrl *)hw)->Dpll[submodule_index].DPLLSTATUS.reg;
}

static inline void hri_oscctrl_set_INTEN_XOSCRDY0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCRDY0;
}

static inline bool hri_oscctrl_get_INTEN_XOSCRDY0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_XOSCRDY0) >> OSCCTRL_INTENSET_XOSCRDY0_Pos;
}

static inline void hri_oscctrl_write_INTEN_XOSCRDY0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCRDY0;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCRDY0;
	}
}

static inline void hri_oscctrl_clear_INTEN_XOSCRDY0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCRDY0;
}

static inline void hri_oscctrl_set_INTEN_XOSCRDY1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCRDY1;
}

static inline bool hri_oscctrl_get_INTEN_XOSCRDY1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_XOSCRDY1) >> OSCCTRL_INTENSET_XOSCRDY1_Pos;
}

static inline void hri_oscctrl_write_INTEN_XOSCRDY1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCRDY1;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCRDY1;
	}
}

static inline void hri_oscctrl_clear_INTEN_XOSCRDY1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCRDY1;
}

static inline void hri_oscctrl_set_INTEN_XOSCFAIL0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCFAIL0;
}

static inline bool hri_oscctrl_get_INTEN_XOSCFAIL0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_XOSCFAIL0) >> OSCCTRL_INTENSET_XOSCFAIL0_Pos;
}

static inline void hri_oscctrl_write_INTEN_XOSCFAIL0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCFAIL0;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCFAIL0;
	}
}

static inline void hri_oscctrl_clear_INTEN_XOSCFAIL0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCFAIL0;
}

static inline void hri_oscctrl_set_INTEN_XOSCFAIL1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCFAIL1;
}

static inline bool hri_oscctrl_get_INTEN_XOSCFAIL1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_XOSCFAIL1) >> OSCCTRL_INTENSET_XOSCFAIL1_Pos;
}

static inline void hri_oscctrl_write_INTEN_XOSCFAIL1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCFAIL1;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_XOSCFAIL1;
	}
}

static inline void hri_oscctrl_clear_INTEN_XOSCFAIL1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_XOSCFAIL1;
}

static inline void hri_oscctrl_set_INTEN_DFLLRDY_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLRDY;
}

static inline bool hri_oscctrl_get_INTEN_DFLLRDY_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DFLLRDY) >> OSCCTRL_INTENSET_DFLLRDY_Pos;
}

static inline void hri_oscctrl_write_INTEN_DFLLRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLRDY;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLRDY;
	}
}

static inline void hri_oscctrl_clear_INTEN_DFLLRDY_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLRDY;
}

static inline void hri_oscctrl_set_INTEN_DFLLOOB_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLOOB;
}

static inline bool hri_oscctrl_get_INTEN_DFLLOOB_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DFLLOOB) >> OSCCTRL_INTENSET_DFLLOOB_Pos;
}

static inline void hri_oscctrl_write_INTEN_DFLLOOB_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLOOB;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLOOB;
	}
}

static inline void hri_oscctrl_clear_INTEN_DFLLOOB_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLOOB;
}

static inline void hri_oscctrl_set_INTEN_DFLLLCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLLCKF;
}

static inline bool hri_oscctrl_get_INTEN_DFLLLCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DFLLLCKF) >> OSCCTRL_INTENSET_DFLLLCKF_Pos;
}

static inline void hri_oscctrl_write_INTEN_DFLLLCKF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLLCKF;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLLCKF;
	}
}

static inline void hri_oscctrl_clear_INTEN_DFLLLCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLLCKF;
}

static inline void hri_oscctrl_set_INTEN_DFLLLCKC_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLLCKC;
}

static inline bool hri_oscctrl_get_INTEN_DFLLLCKC_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DFLLLCKC) >> OSCCTRL_INTENSET_DFLLLCKC_Pos;
}

static inline void hri_oscctrl_write_INTEN_DFLLLCKC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLLCKC;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLLCKC;
	}
}

static inline void hri_oscctrl_clear_INTEN_DFLLLCKC_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLLCKC;
}

static inline void hri_oscctrl_set_INTEN_DFLLRCS_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLRCS;
}

static inline bool hri_oscctrl_get_INTEN_DFLLRCS_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DFLLRCS) >> OSCCTRL_INTENSET_DFLLRCS_Pos;
}

static inline void hri_oscctrl_write_INTEN_DFLLRCS_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLRCS;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DFLLRCS;
	}
}

static inline void hri_oscctrl_clear_INTEN_DFLLRCS_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DFLLRCS;
}

static inline void hri_oscctrl_set_INTEN_DPLL0LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LCKR;
}

static inline bool hri_oscctrl_get_INTEN_DPLL0LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL0LCKR) >> OSCCTRL_INTENSET_DPLL0LCKR_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL0LCKR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LCKR;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LCKR;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL0LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LCKR;
}

static inline void hri_oscctrl_set_INTEN_DPLL0LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LCKF;
}

static inline bool hri_oscctrl_get_INTEN_DPLL0LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL0LCKF) >> OSCCTRL_INTENSET_DPLL0LCKF_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL0LCKF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LCKF;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LCKF;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL0LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LCKF;
}

static inline void hri_oscctrl_set_INTEN_DPLL0LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LTO;
}

static inline bool hri_oscctrl_get_INTEN_DPLL0LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL0LTO) >> OSCCTRL_INTENSET_DPLL0LTO_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL0LTO_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LTO;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LTO;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL0LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LTO;
}

static inline void hri_oscctrl_set_INTEN_DPLL0LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LDRTO;
}

static inline bool hri_oscctrl_get_INTEN_DPLL0LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL0LDRTO) >> OSCCTRL_INTENSET_DPLL0LDRTO_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL0LDRTO_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LDRTO;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL0LDRTO;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL0LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL0LDRTO;
}

static inline void hri_oscctrl_set_INTEN_DPLL1LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LCKR;
}

static inline bool hri_oscctrl_get_INTEN_DPLL1LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL1LCKR) >> OSCCTRL_INTENSET_DPLL1LCKR_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL1LCKR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LCKR;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LCKR;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL1LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LCKR;
}

static inline void hri_oscctrl_set_INTEN_DPLL1LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LCKF;
}

static inline bool hri_oscctrl_get_INTEN_DPLL1LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL1LCKF) >> OSCCTRL_INTENSET_DPLL1LCKF_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL1LCKF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LCKF;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LCKF;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL1LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LCKF;
}

static inline void hri_oscctrl_set_INTEN_DPLL1LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LTO;
}

static inline bool hri_oscctrl_get_INTEN_DPLL1LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL1LTO) >> OSCCTRL_INTENSET_DPLL1LTO_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL1LTO_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LTO;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LTO;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL1LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LTO;
}

static inline void hri_oscctrl_set_INTEN_DPLL1LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LDRTO;
}

static inline bool hri_oscctrl_get_INTEN_DPLL1LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTENSET.reg & OSCCTRL_INTENSET_DPLL1LDRTO) >> OSCCTRL_INTENSET_DPLL1LDRTO_Pos;
}

static inline void hri_oscctrl_write_INTEN_DPLL1LDRTO_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LDRTO;
	} else {
		((Oscctrl *)hw)->INTENSET.reg = OSCCTRL_INTENSET_DPLL1LDRTO;
	}
}

static inline void hri_oscctrl_clear_INTEN_DPLL1LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTENCLR.reg = OSCCTRL_INTENSET_DPLL1LDRTO;
}

static inline void hri_oscctrl_set_INTEN_reg(const void *const hw, hri_oscctrl_intenset_reg_t mask)
{
	((Oscctrl *)hw)->INTENSET.reg = mask;
}

static inline hri_oscctrl_intenset_reg_t hri_oscctrl_get_INTEN_reg(const void *const          hw,
                                                                   hri_oscctrl_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_intenset_reg_t hri_oscctrl_read_INTEN_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->INTENSET.reg;
}

static inline void hri_oscctrl_write_INTEN_reg(const void *const hw, hri_oscctrl_intenset_reg_t data)
{
	((Oscctrl *)hw)->INTENSET.reg = data;
	((Oscctrl *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_oscctrl_clear_INTEN_reg(const void *const hw, hri_oscctrl_intenset_reg_t mask)
{
	((Oscctrl *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_oscctrl_get_INTFLAG_XOSCRDY0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCRDY0) >> OSCCTRL_INTFLAG_XOSCRDY0_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_XOSCRDY0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCRDY0;
}

static inline bool hri_oscctrl_get_INTFLAG_XOSCRDY1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCRDY1) >> OSCCTRL_INTFLAG_XOSCRDY1_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_XOSCRDY1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCRDY1;
}

static inline bool hri_oscctrl_get_INTFLAG_XOSCFAIL0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCFAIL0) >> OSCCTRL_INTFLAG_XOSCFAIL0_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_XOSCFAIL0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCFAIL0;
}

static inline bool hri_oscctrl_get_INTFLAG_XOSCFAIL1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCFAIL1) >> OSCCTRL_INTFLAG_XOSCFAIL1_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_XOSCFAIL1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCFAIL1;
}

static inline bool hri_oscctrl_get_INTFLAG_DFLLRDY_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) >> OSCCTRL_INTFLAG_DFLLRDY_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DFLLRDY_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRDY;
}

static inline bool hri_oscctrl_get_INTFLAG_DFLLOOB_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLOOB) >> OSCCTRL_INTFLAG_DFLLOOB_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DFLLOOB_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLOOB;
}

static inline bool hri_oscctrl_get_INTFLAG_DFLLLCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLLCKF) >> OSCCTRL_INTFLAG_DFLLLCKF_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DFLLLCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLLCKF;
}

static inline bool hri_oscctrl_get_INTFLAG_DFLLLCKC_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLLCKC) >> OSCCTRL_INTFLAG_DFLLLCKC_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DFLLLCKC_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLLCKC;
}

static inline bool hri_oscctrl_get_INTFLAG_DFLLRCS_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRCS) >> OSCCTRL_INTFLAG_DFLLRCS_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DFLLRCS_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRCS;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL0LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LCKR) >> OSCCTRL_INTFLAG_DPLL0LCKR_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL0LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LCKR;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL0LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LCKF) >> OSCCTRL_INTFLAG_DPLL0LCKF_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL0LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LCKF;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL0LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LTO) >> OSCCTRL_INTFLAG_DPLL0LTO_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL0LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LTO;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL0LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LDRTO) >> OSCCTRL_INTFLAG_DPLL0LDRTO_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL0LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LDRTO;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL1LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LCKR) >> OSCCTRL_INTFLAG_DPLL1LCKR_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL1LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LCKR;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL1LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LCKF) >> OSCCTRL_INTFLAG_DPLL1LCKF_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL1LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LCKF;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL1LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LTO) >> OSCCTRL_INTFLAG_DPLL1LTO_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL1LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LTO;
}

static inline bool hri_oscctrl_get_INTFLAG_DPLL1LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LDRTO) >> OSCCTRL_INTFLAG_DPLL1LDRTO_Pos;
}

static inline void hri_oscctrl_clear_INTFLAG_DPLL1LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LDRTO;
}

static inline bool hri_oscctrl_get_interrupt_XOSCRDY0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCRDY0) >> OSCCTRL_INTFLAG_XOSCRDY0_Pos;
}

static inline void hri_oscctrl_clear_interrupt_XOSCRDY0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCRDY0;
}

static inline bool hri_oscctrl_get_interrupt_XOSCRDY1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCRDY1) >> OSCCTRL_INTFLAG_XOSCRDY1_Pos;
}

static inline void hri_oscctrl_clear_interrupt_XOSCRDY1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCRDY1;
}

static inline bool hri_oscctrl_get_interrupt_XOSCFAIL0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCFAIL0) >> OSCCTRL_INTFLAG_XOSCFAIL0_Pos;
}

static inline void hri_oscctrl_clear_interrupt_XOSCFAIL0_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCFAIL0;
}

static inline bool hri_oscctrl_get_interrupt_XOSCFAIL1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_XOSCFAIL1) >> OSCCTRL_INTFLAG_XOSCFAIL1_Pos;
}

static inline void hri_oscctrl_clear_interrupt_XOSCFAIL1_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_XOSCFAIL1;
}

static inline bool hri_oscctrl_get_interrupt_DFLLRDY_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) >> OSCCTRL_INTFLAG_DFLLRDY_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DFLLRDY_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRDY;
}

static inline bool hri_oscctrl_get_interrupt_DFLLOOB_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLOOB) >> OSCCTRL_INTFLAG_DFLLOOB_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DFLLOOB_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLOOB;
}

static inline bool hri_oscctrl_get_interrupt_DFLLLCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLLCKF) >> OSCCTRL_INTFLAG_DFLLLCKF_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DFLLLCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLLCKF;
}

static inline bool hri_oscctrl_get_interrupt_DFLLLCKC_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLLCKC) >> OSCCTRL_INTFLAG_DFLLLCKC_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DFLLLCKC_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLLCKC;
}

static inline bool hri_oscctrl_get_interrupt_DFLLRCS_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRCS) >> OSCCTRL_INTFLAG_DFLLRCS_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DFLLRCS_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRCS;
}

static inline bool hri_oscctrl_get_interrupt_DPLL0LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LCKR) >> OSCCTRL_INTFLAG_DPLL0LCKR_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL0LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LCKR;
}

static inline bool hri_oscctrl_get_interrupt_DPLL0LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LCKF) >> OSCCTRL_INTFLAG_DPLL0LCKF_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL0LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LCKF;
}

static inline bool hri_oscctrl_get_interrupt_DPLL0LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LTO) >> OSCCTRL_INTFLAG_DPLL0LTO_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL0LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LTO;
}

static inline bool hri_oscctrl_get_interrupt_DPLL0LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL0LDRTO) >> OSCCTRL_INTFLAG_DPLL0LDRTO_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL0LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL0LDRTO;
}

static inline bool hri_oscctrl_get_interrupt_DPLL1LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LCKR) >> OSCCTRL_INTFLAG_DPLL1LCKR_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL1LCKR_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LCKR;
}

static inline bool hri_oscctrl_get_interrupt_DPLL1LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LCKF) >> OSCCTRL_INTFLAG_DPLL1LCKF_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL1LCKF_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LCKF;
}

static inline bool hri_oscctrl_get_interrupt_DPLL1LTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LTO) >> OSCCTRL_INTFLAG_DPLL1LTO_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL1LTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LTO;
}

static inline bool hri_oscctrl_get_interrupt_DPLL1LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->INTFLAG.reg & OSCCTRL_INTFLAG_DPLL1LDRTO) >> OSCCTRL_INTFLAG_DPLL1LDRTO_Pos;
}

static inline void hri_oscctrl_clear_interrupt_DPLL1LDRTO_bit(const void *const hw)
{
	((Oscctrl *)hw)->INTFLAG.reg = OSCCTRL_INTFLAG_DPLL1LDRTO;
}

static inline hri_oscctrl_intflag_reg_t hri_oscctrl_get_INTFLAG_reg(const void *const         hw,
                                                                    hri_oscctrl_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_intflag_reg_t hri_oscctrl_read_INTFLAG_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->INTFLAG.reg;
}

static inline void hri_oscctrl_clear_INTFLAG_reg(const void *const hw, hri_oscctrl_intflag_reg_t mask)
{
	((Oscctrl *)hw)->INTFLAG.reg = mask;
}

static inline void hri_oscctrl_set_EVCTRL_CFDEO0_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg |= OSCCTRL_EVCTRL_CFDEO0;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_EVCTRL_CFDEO0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->EVCTRL.reg;
	tmp = (tmp & OSCCTRL_EVCTRL_CFDEO0) >> OSCCTRL_EVCTRL_CFDEO0_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_EVCTRL_CFDEO0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->EVCTRL.reg;
	tmp &= ~OSCCTRL_EVCTRL_CFDEO0;
	tmp |= value << OSCCTRL_EVCTRL_CFDEO0_Pos;
	((Oscctrl *)hw)->EVCTRL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_EVCTRL_CFDEO0_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg &= ~OSCCTRL_EVCTRL_CFDEO0;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_EVCTRL_CFDEO0_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg ^= OSCCTRL_EVCTRL_CFDEO0;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_EVCTRL_CFDEO1_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg |= OSCCTRL_EVCTRL_CFDEO1;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_EVCTRL_CFDEO1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->EVCTRL.reg;
	tmp = (tmp & OSCCTRL_EVCTRL_CFDEO1) >> OSCCTRL_EVCTRL_CFDEO1_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_EVCTRL_CFDEO1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->EVCTRL.reg;
	tmp &= ~OSCCTRL_EVCTRL_CFDEO1;
	tmp |= value << OSCCTRL_EVCTRL_CFDEO1_Pos;
	((Oscctrl *)hw)->EVCTRL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_EVCTRL_CFDEO1_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg &= ~OSCCTRL_EVCTRL_CFDEO1;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_EVCTRL_CFDEO1_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg ^= OSCCTRL_EVCTRL_CFDEO1;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_EVCTRL_reg(const void *const hw, hri_oscctrl_evctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_evctrl_reg_t hri_oscctrl_get_EVCTRL_reg(const void *const hw, hri_oscctrl_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_EVCTRL_reg(const void *const hw, hri_oscctrl_evctrl_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_EVCTRL_reg(const void *const hw, hri_oscctrl_evctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_EVCTRL_reg(const void *const hw, hri_oscctrl_evctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->EVCTRL.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_evctrl_reg_t hri_oscctrl_read_EVCTRL_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->EVCTRL.reg;
}

static inline void hri_oscctrl_set_XOSCCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_ENABLE) >> OSCCTRL_XOSCCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_ENABLE_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_ENABLE;
	tmp |= value << OSCCTRL_XOSCCTRL_ENABLE_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_ENABLE_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_XTALEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_XTALEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_XTALEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_XTALEN) >> OSCCTRL_XOSCCTRL_XTALEN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_XTALEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_XTALEN;
	tmp |= value << OSCCTRL_XOSCCTRL_XTALEN_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_XTALEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_XTALEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_XTALEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_XTALEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_RUNSTDBY) >> OSCCTRL_XOSCCTRL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_RUNSTDBY;
	tmp |= value << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_RUNSTDBY_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_ONDEMAND_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_ONDEMAND_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_ONDEMAND) >> OSCCTRL_XOSCCTRL_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_ONDEMAND_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_ONDEMAND;
	tmp |= value << OSCCTRL_XOSCCTRL_ONDEMAND_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_ONDEMAND_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_ONDEMAND_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_LOWBUFGAIN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_LOWBUFGAIN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_LOWBUFGAIN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_LOWBUFGAIN) >> OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_LOWBUFGAIN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_LOWBUFGAIN;
	tmp |= value << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_LOWBUFGAIN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_LOWBUFGAIN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_LOWBUFGAIN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_LOWBUFGAIN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_ENALC_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_ENALC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_ENALC_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_ENALC) >> OSCCTRL_XOSCCTRL_ENALC_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_ENALC_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_ENALC;
	tmp |= value << OSCCTRL_XOSCCTRL_ENALC_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_ENALC_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_ENALC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_ENALC_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_ENALC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_CFDEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_CFDEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_CFDEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_CFDEN) >> OSCCTRL_XOSCCTRL_CFDEN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_CFDEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_CFDEN;
	tmp |= value << OSCCTRL_XOSCCTRL_CFDEN_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_CFDEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_CFDEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_CFDEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_CFDEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_SWBEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_SWBEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_XOSCCTRL_SWBEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_SWBEN) >> OSCCTRL_XOSCCTRL_SWBEN_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_SWBEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_SWBEN;
	tmp |= value << OSCCTRL_XOSCCTRL_SWBEN_Pos;
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_SWBEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_SWBEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_SWBEN_bit(const void *const hw, uint8_t index)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_SWBEN;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index,
                                                     hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_IPTAT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_get_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index,
                                                                           hri_oscctrl_xoscctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_IPTAT(mask)) >> OSCCTRL_XOSCCTRL_IPTAT_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index,
                                                       hri_oscctrl_xoscctrl_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_IPTAT_Msk;
	tmp |= OSCCTRL_XOSCCTRL_IPTAT(data);
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index,
                                                       hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_IPTAT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index,
                                                        hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_IPTAT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_read_XOSCCTRL_IPTAT_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_IPTAT_Msk) >> OSCCTRL_XOSCCTRL_IPTAT_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index,
                                                     hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_IMULT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_get_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index,
                                                                           hri_oscctrl_xoscctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_IMULT(mask)) >> OSCCTRL_XOSCCTRL_IMULT_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index,
                                                       hri_oscctrl_xoscctrl_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_IMULT_Msk;
	tmp |= OSCCTRL_XOSCCTRL_IMULT(data);
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index,
                                                       hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_IMULT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index,
                                                        hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_IMULT(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_read_XOSCCTRL_IMULT_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_IMULT_Msk) >> OSCCTRL_XOSCCTRL_IMULT_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index,
                                                       hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_STARTUP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_get_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index,
                                                                             hri_oscctrl_xoscctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_STARTUP(mask)) >> OSCCTRL_XOSCCTRL_STARTUP_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index,
                                                         hri_oscctrl_xoscctrl_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_STARTUP_Msk;
	tmp |= OSCCTRL_XOSCCTRL_STARTUP(data);
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index,
                                                         hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_STARTUP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index,
                                                          hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_STARTUP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_read_XOSCCTRL_STARTUP_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_STARTUP_Msk) >> OSCCTRL_XOSCCTRL_STARTUP_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index,
                                                        hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= OSCCTRL_XOSCCTRL_CFDPRESC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_get_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index,
                                                                              hri_oscctrl_xoscctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_CFDPRESC(mask)) >> OSCCTRL_XOSCCTRL_CFDPRESC_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index,
                                                          hri_oscctrl_xoscctrl_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= ~OSCCTRL_XOSCCTRL_CFDPRESC_Msk;
	tmp |= OSCCTRL_XOSCCTRL_CFDPRESC(data);
	((Oscctrl *)hw)->XOSCCTRL[index].reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index,
                                                          hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~OSCCTRL_XOSCCTRL_CFDPRESC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index,
                                                           hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= OSCCTRL_XOSCCTRL_CFDPRESC(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_read_XOSCCTRL_CFDPRESC_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp = (tmp & OSCCTRL_XOSCCTRL_CFDPRESC_Msk) >> OSCCTRL_XOSCCTRL_CFDPRESC_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_XOSCCTRL_reg(const void *const hw, uint8_t index, hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_get_XOSCCTRL_reg(const void *const hw, uint8_t index,
                                                                      hri_oscctrl_xoscctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->XOSCCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_XOSCCTRL_reg(const void *const hw, uint8_t index, hri_oscctrl_xoscctrl_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_XOSCCTRL_reg(const void *const hw, uint8_t index, hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_XOSCCTRL_reg(const void *const hw, uint8_t index, hri_oscctrl_xoscctrl_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->XOSCCTRL[index].reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_xoscctrl_reg_t hri_oscctrl_read_XOSCCTRL_reg(const void *const hw, uint8_t index)
{
	return ((Oscctrl *)hw)->XOSCCTRL[index].reg;
}

static inline void hri_oscctrl_set_DFLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLA_ENABLE) >> OSCCTRL_DFLLCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp &= ~OSCCTRL_DFLLCTRLA_ENABLE;
	tmp |= value << OSCCTRL_DFLLCTRLA_ENABLE_Pos;
	((Oscctrl *)hw)->DFLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg &= ~OSCCTRL_DFLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLA_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg ^= OSCCTRL_DFLLCTRLA_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLA_RUNSTDBY) >> OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp &= ~OSCCTRL_DFLLCTRLA_RUNSTDBY;
	tmp |= value << OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos;
	((Oscctrl *)hw)->DFLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg &= ~OSCCTRL_DFLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg ^= OSCCTRL_DFLLCTRLA_RUNSTDBY;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLA_ONDEMAND) >> OSCCTRL_DFLLCTRLA_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLA_ONDEMAND_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp &= ~OSCCTRL_DFLLCTRLA_ONDEMAND;
	tmp |= value << OSCCTRL_DFLLCTRLA_ONDEMAND_Pos;
	((Oscctrl *)hw)->DFLLCTRLA.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg &= ~OSCCTRL_DFLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg ^= OSCCTRL_DFLLCTRLA_ONDEMAND;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLA_reg(const void *const hw, hri_oscctrl_dfllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllctrla_reg_t hri_oscctrl_get_DFLLCTRLA_reg(const void *const           hw,
                                                                        hri_oscctrl_dfllctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLA_reg(const void *const hw, hri_oscctrl_dfllctrla_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLA_reg(const void *const hw, hri_oscctrl_dfllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLA_reg(const void *const hw, hri_oscctrl_dfllctrla_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLA.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllctrla_reg_t hri_oscctrl_read_DFLLCTRLA_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->DFLLCTRLA.reg;
}

static inline void hri_oscctrl_set_DFLLCTRLB_MODE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_MODE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_MODE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_MODE) >> OSCCTRL_DFLLCTRLB_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_MODE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_MODE;
	tmp |= value << OSCCTRL_DFLLCTRLB_MODE_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_MODE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_MODE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_MODE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_MODE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_STABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_STABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_STABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_STABLE) >> OSCCTRL_DFLLCTRLB_STABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_STABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_STABLE;
	tmp |= value << OSCCTRL_DFLLCTRLB_STABLE_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_STABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_STABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_STABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_STABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_LLAW_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_LLAW;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_LLAW_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_LLAW) >> OSCCTRL_DFLLCTRLB_LLAW_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_LLAW_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_LLAW;
	tmp |= value << OSCCTRL_DFLLCTRLB_LLAW_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_LLAW_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_LLAW;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_LLAW_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_LLAW;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_USBCRM_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_USBCRM;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_USBCRM_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_USBCRM) >> OSCCTRL_DFLLCTRLB_USBCRM_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_USBCRM_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_USBCRM;
	tmp |= value << OSCCTRL_DFLLCTRLB_USBCRM_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_USBCRM_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_USBCRM;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_USBCRM_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_USBCRM;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_CCDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_CCDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_CCDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_CCDIS) >> OSCCTRL_DFLLCTRLB_CCDIS_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_CCDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_CCDIS;
	tmp |= value << OSCCTRL_DFLLCTRLB_CCDIS_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_CCDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_CCDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_CCDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_CCDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_QLDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_QLDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_QLDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_QLDIS) >> OSCCTRL_DFLLCTRLB_QLDIS_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_QLDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_QLDIS;
	tmp |= value << OSCCTRL_DFLLCTRLB_QLDIS_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_QLDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_QLDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_QLDIS_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_QLDIS;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_BPLCKC_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_BPLCKC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_BPLCKC_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_BPLCKC) >> OSCCTRL_DFLLCTRLB_BPLCKC_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_BPLCKC_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_BPLCKC;
	tmp |= value << OSCCTRL_DFLLCTRLB_BPLCKC_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_BPLCKC_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_BPLCKC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_BPLCKC_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_BPLCKC;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_WAITLOCK_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= OSCCTRL_DFLLCTRLB_WAITLOCK;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLCTRLB_WAITLOCK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp = (tmp & OSCCTRL_DFLLCTRLB_WAITLOCK) >> OSCCTRL_DFLLCTRLB_WAITLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_WAITLOCK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= ~OSCCTRL_DFLLCTRLB_WAITLOCK;
	tmp |= value << OSCCTRL_DFLLCTRLB_WAITLOCK_Pos;
	((Oscctrl *)hw)->DFLLCTRLB.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_WAITLOCK_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~OSCCTRL_DFLLCTRLB_WAITLOCK;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_WAITLOCK_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= OSCCTRL_DFLLCTRLB_WAITLOCK;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLCTRLB_reg(const void *const hw, hri_oscctrl_dfllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllctrlb_reg_t hri_oscctrl_get_DFLLCTRLB_reg(const void *const           hw,
                                                                        hri_oscctrl_dfllctrlb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLCTRLB_reg(const void *const hw, hri_oscctrl_dfllctrlb_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLCTRLB_reg(const void *const hw, hri_oscctrl_dfllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLCTRLB_reg(const void *const hw, hri_oscctrl_dfllctrlb_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLCTRLB.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllctrlb_reg_t hri_oscctrl_read_DFLLCTRLB_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->DFLLCTRLB.reg;
}

static inline void hri_oscctrl_set_DFLLVAL_FINE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg |= OSCCTRL_DFLLVAL_FINE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_get_DFLLVAL_FINE_bf(const void *const         hw,
                                                                        hri_oscctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_FINE(mask)) >> OSCCTRL_DFLLVAL_FINE_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLVAL_FINE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp &= ~OSCCTRL_DFLLVAL_FINE_Msk;
	tmp |= OSCCTRL_DFLLVAL_FINE(data);
	((Oscctrl *)hw)->DFLLVAL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLVAL_FINE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg &= ~OSCCTRL_DFLLVAL_FINE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLVAL_FINE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg ^= OSCCTRL_DFLLVAL_FINE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_read_DFLLVAL_FINE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_FINE_Msk) >> OSCCTRL_DFLLVAL_FINE_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLVAL_COARSE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg |= OSCCTRL_DFLLVAL_COARSE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_get_DFLLVAL_COARSE_bf(const void *const         hw,
                                                                          hri_oscctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_COARSE(mask)) >> OSCCTRL_DFLLVAL_COARSE_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLVAL_COARSE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp &= ~OSCCTRL_DFLLVAL_COARSE_Msk;
	tmp |= OSCCTRL_DFLLVAL_COARSE(data);
	((Oscctrl *)hw)->DFLLVAL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLVAL_COARSE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg &= ~OSCCTRL_DFLLVAL_COARSE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLVAL_COARSE_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg ^= OSCCTRL_DFLLVAL_COARSE(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_read_DFLLVAL_COARSE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_COARSE_Msk) >> OSCCTRL_DFLLVAL_COARSE_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLVAL_DIFF_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg |= OSCCTRL_DFLLVAL_DIFF(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_get_DFLLVAL_DIFF_bf(const void *const         hw,
                                                                        hri_oscctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_DIFF(mask)) >> OSCCTRL_DFLLVAL_DIFF_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLVAL_DIFF_bf(const void *const hw, hri_oscctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp &= ~OSCCTRL_DFLLVAL_DIFF_Msk;
	tmp |= OSCCTRL_DFLLVAL_DIFF(data);
	((Oscctrl *)hw)->DFLLVAL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLVAL_DIFF_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg &= ~OSCCTRL_DFLLVAL_DIFF(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLVAL_DIFF_bf(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg ^= OSCCTRL_DFLLVAL_DIFF(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_read_DFLLVAL_DIFF_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & OSCCTRL_DFLLVAL_DIFF_Msk) >> OSCCTRL_DFLLVAL_DIFF_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLVAL_reg(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_get_DFLLVAL_reg(const void *const         hw,
                                                                    hri_oscctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLVAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLVAL_reg(const void *const hw, hri_oscctrl_dfllval_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLVAL_reg(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLVAL_reg(const void *const hw, hri_oscctrl_dfllval_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLVAL.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllval_reg_t hri_oscctrl_read_DFLLVAL_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->DFLLVAL.reg;
}

static inline void hri_oscctrl_set_DFLLMUL_MUL_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg |= OSCCTRL_DFLLMUL_MUL(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_get_DFLLMUL_MUL_bf(const void *const         hw,
                                                                       hri_oscctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_MUL(mask)) >> OSCCTRL_DFLLMUL_MUL_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLMUL_MUL_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp &= ~OSCCTRL_DFLLMUL_MUL_Msk;
	tmp |= OSCCTRL_DFLLMUL_MUL(data);
	((Oscctrl *)hw)->DFLLMUL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLMUL_MUL_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg &= ~OSCCTRL_DFLLMUL_MUL(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLMUL_MUL_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg ^= OSCCTRL_DFLLMUL_MUL(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_read_DFLLMUL_MUL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_MUL_Msk) >> OSCCTRL_DFLLMUL_MUL_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLMUL_FSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg |= OSCCTRL_DFLLMUL_FSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_get_DFLLMUL_FSTEP_bf(const void *const         hw,
                                                                         hri_oscctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_FSTEP(mask)) >> OSCCTRL_DFLLMUL_FSTEP_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLMUL_FSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp &= ~OSCCTRL_DFLLMUL_FSTEP_Msk;
	tmp |= OSCCTRL_DFLLMUL_FSTEP(data);
	((Oscctrl *)hw)->DFLLMUL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLMUL_FSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg &= ~OSCCTRL_DFLLMUL_FSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLMUL_FSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg ^= OSCCTRL_DFLLMUL_FSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_read_DFLLMUL_FSTEP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_FSTEP_Msk) >> OSCCTRL_DFLLMUL_FSTEP_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLMUL_CSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg |= OSCCTRL_DFLLMUL_CSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_get_DFLLMUL_CSTEP_bf(const void *const         hw,
                                                                         hri_oscctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_CSTEP(mask)) >> OSCCTRL_DFLLMUL_CSTEP_Pos;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLMUL_CSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp &= ~OSCCTRL_DFLLMUL_CSTEP_Msk;
	tmp |= OSCCTRL_DFLLMUL_CSTEP(data);
	((Oscctrl *)hw)->DFLLMUL.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLMUL_CSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg &= ~OSCCTRL_DFLLMUL_CSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLMUL_CSTEP_bf(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg ^= OSCCTRL_DFLLMUL_CSTEP(mask);
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_read_DFLLMUL_CSTEP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & OSCCTRL_DFLLMUL_CSTEP_Msk) >> OSCCTRL_DFLLMUL_CSTEP_Pos;
	return tmp;
}

static inline void hri_oscctrl_set_DFLLMUL_reg(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_get_DFLLMUL_reg(const void *const         hw,
                                                                    hri_oscctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLMUL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLMUL_reg(const void *const hw, hri_oscctrl_dfllmul_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLMUL_reg(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLMUL_reg(const void *const hw, hri_oscctrl_dfllmul_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLMUL.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllmul_reg_t hri_oscctrl_read_DFLLMUL_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->DFLLMUL.reg;
}

static inline void hri_oscctrl_set_DFLLSYNC_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg |= OSCCTRL_DFLLSYNC_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLSYNC_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp = (tmp & OSCCTRL_DFLLSYNC_ENABLE) >> OSCCTRL_DFLLSYNC_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLSYNC_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp &= ~OSCCTRL_DFLLSYNC_ENABLE;
	tmp |= value << OSCCTRL_DFLLSYNC_ENABLE_Pos;
	((Oscctrl *)hw)->DFLLSYNC.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLSYNC_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg &= ~OSCCTRL_DFLLSYNC_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLSYNC_ENABLE_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg ^= OSCCTRL_DFLLSYNC_ENABLE;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLSYNC_DFLLCTRLB_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg |= OSCCTRL_DFLLSYNC_DFLLCTRLB;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLSYNC_DFLLCTRLB_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp = (tmp & OSCCTRL_DFLLSYNC_DFLLCTRLB) >> OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLSYNC_DFLLCTRLB_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp &= ~OSCCTRL_DFLLSYNC_DFLLCTRLB;
	tmp |= value << OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos;
	((Oscctrl *)hw)->DFLLSYNC.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLSYNC_DFLLCTRLB_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg &= ~OSCCTRL_DFLLSYNC_DFLLCTRLB;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLSYNC_DFLLCTRLB_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg ^= OSCCTRL_DFLLSYNC_DFLLCTRLB;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLSYNC_DFLLVAL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg |= OSCCTRL_DFLLSYNC_DFLLVAL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLSYNC_DFLLVAL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp = (tmp & OSCCTRL_DFLLSYNC_DFLLVAL) >> OSCCTRL_DFLLSYNC_DFLLVAL_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLSYNC_DFLLVAL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp &= ~OSCCTRL_DFLLSYNC_DFLLVAL;
	tmp |= value << OSCCTRL_DFLLSYNC_DFLLVAL_Pos;
	((Oscctrl *)hw)->DFLLSYNC.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLSYNC_DFLLVAL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg &= ~OSCCTRL_DFLLSYNC_DFLLVAL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLSYNC_DFLLVAL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg ^= OSCCTRL_DFLLSYNC_DFLLVAL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLSYNC_DFLLMUL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg |= OSCCTRL_DFLLSYNC_DFLLMUL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_oscctrl_get_DFLLSYNC_DFLLMUL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp = (tmp & OSCCTRL_DFLLSYNC_DFLLMUL) >> OSCCTRL_DFLLSYNC_DFLLMUL_Pos;
	return (bool)tmp;
}

static inline void hri_oscctrl_write_DFLLSYNC_DFLLMUL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	OSCCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp &= ~OSCCTRL_DFLLSYNC_DFLLMUL;
	tmp |= value << OSCCTRL_DFLLSYNC_DFLLMUL_Pos;
	((Oscctrl *)hw)->DFLLSYNC.reg = tmp;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLSYNC_DFLLMUL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg &= ~OSCCTRL_DFLLSYNC_DFLLMUL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLSYNC_DFLLMUL_bit(const void *const hw)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg ^= OSCCTRL_DFLLSYNC_DFLLMUL;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_set_DFLLSYNC_reg(const void *const hw, hri_oscctrl_dfllsync_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg |= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllsync_reg_t hri_oscctrl_get_DFLLSYNC_reg(const void *const          hw,
                                                                      hri_oscctrl_dfllsync_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Oscctrl *)hw)->DFLLSYNC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_oscctrl_write_DFLLSYNC_reg(const void *const hw, hri_oscctrl_dfllsync_reg_t data)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg = data;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_clear_DFLLSYNC_reg(const void *const hw, hri_oscctrl_dfllsync_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg &= ~mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_oscctrl_toggle_DFLLSYNC_reg(const void *const hw, hri_oscctrl_dfllsync_reg_t mask)
{
	OSCCTRL_CRITICAL_SECTION_ENTER();
	((Oscctrl *)hw)->DFLLSYNC.reg ^= mask;
	OSCCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_oscctrl_dfllsync_reg_t hri_oscctrl_read_DFLLSYNC_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->DFLLSYNC.reg;
}

static inline bool hri_oscctrl_get_STATUS_XOSCRDY0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCRDY0) >> OSCCTRL_STATUS_XOSCRDY0_Pos;
}

static inline bool hri_oscctrl_get_STATUS_XOSCRDY1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCRDY1) >> OSCCTRL_STATUS_XOSCRDY1_Pos;
}

static inline bool hri_oscctrl_get_STATUS_XOSCFAIL0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCFAIL0) >> OSCCTRL_STATUS_XOSCFAIL0_Pos;
}

static inline bool hri_oscctrl_get_STATUS_XOSCFAIL1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCFAIL1) >> OSCCTRL_STATUS_XOSCFAIL1_Pos;
}

static inline bool hri_oscctrl_get_STATUS_XOSCCKSW0_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCCKSW0) >> OSCCTRL_STATUS_XOSCCKSW0_Pos;
}

static inline bool hri_oscctrl_get_STATUS_XOSCCKSW1_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_XOSCCKSW1) >> OSCCTRL_STATUS_XOSCCKSW1_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DFLLRDY_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DFLLRDY) >> OSCCTRL_STATUS_DFLLRDY_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DFLLOOB_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DFLLOOB) >> OSCCTRL_STATUS_DFLLOOB_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DFLLLCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DFLLLCKF) >> OSCCTRL_STATUS_DFLLLCKF_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DFLLLCKC_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DFLLLCKC) >> OSCCTRL_STATUS_DFLLLCKC_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DFLLRCS_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DFLLRCS) >> OSCCTRL_STATUS_DFLLRCS_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL0LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL0LCKR) >> OSCCTRL_STATUS_DPLL0LCKR_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL0LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL0LCKF) >> OSCCTRL_STATUS_DPLL0LCKF_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL0TO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL0TO) >> OSCCTRL_STATUS_DPLL0TO_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL0LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL0LDRTO) >> OSCCTRL_STATUS_DPLL0LDRTO_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL1LCKR_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL1LCKR) >> OSCCTRL_STATUS_DPLL1LCKR_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL1LCKF_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL1LCKF) >> OSCCTRL_STATUS_DPLL1LCKF_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL1TO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL1TO) >> OSCCTRL_STATUS_DPLL1TO_Pos;
}

static inline bool hri_oscctrl_get_STATUS_DPLL1LDRTO_bit(const void *const hw)
{
	return (((Oscctrl *)hw)->STATUS.reg & OSCCTRL_STATUS_DPLL1LDRTO) >> OSCCTRL_STATUS_DPLL1LDRTO_Pos;
}

static inline hri_oscctrl_status_reg_t hri_oscctrl_get_STATUS_reg(const void *const hw, hri_oscctrl_status_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Oscctrl *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_oscctrl_status_reg_t hri_oscctrl_read_STATUS_reg(const void *const hw)
{
	return ((Oscctrl *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_OSCCTRL_D51_H_INCLUDED */
#endif /* _SAMD51_OSCCTRL_COMPONENT_ */
