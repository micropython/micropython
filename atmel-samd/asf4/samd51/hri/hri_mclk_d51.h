/**
 * \file
 *
 * \brief SAM MCLK
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

#ifdef _SAMD51_MCLK_COMPONENT_
#ifndef _HRI_MCLK_D51_H_INCLUDED_
#define _HRI_MCLK_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_MCLK_CRITICAL_SECTIONS)
#define MCLK_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define MCLK_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define MCLK_CRITICAL_SECTION_ENTER()
#define MCLK_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_mclk_ahbmask_reg_t;
typedef uint32_t hri_mclk_apbamask_reg_t;
typedef uint32_t hri_mclk_apbbmask_reg_t;
typedef uint32_t hri_mclk_apbcmask_reg_t;
typedef uint32_t hri_mclk_apbdmask_reg_t;
typedef uint8_t  hri_mclk_cpudiv_reg_t;
typedef uint8_t  hri_mclk_hsdiv_reg_t;
typedef uint8_t  hri_mclk_intenset_reg_t;
typedef uint8_t  hri_mclk_intflag_reg_t;

static inline void hri_mclk_set_INTEN_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTENSET.reg = MCLK_INTENSET_CKRDY;
}

static inline bool hri_mclk_get_INTEN_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTENSET.reg & MCLK_INTENSET_CKRDY) >> MCLK_INTENSET_CKRDY_Pos;
}

static inline void hri_mclk_write_INTEN_CKRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Mclk *)hw)->INTENCLR.reg = MCLK_INTENSET_CKRDY;
	} else {
		((Mclk *)hw)->INTENSET.reg = MCLK_INTENSET_CKRDY;
	}
}

static inline void hri_mclk_clear_INTEN_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTENCLR.reg = MCLK_INTENSET_CKRDY;
}

static inline void hri_mclk_set_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	((Mclk *)hw)->INTENSET.reg = mask;
}

static inline hri_mclk_intenset_reg_t hri_mclk_get_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mclk_intenset_reg_t hri_mclk_read_INTEN_reg(const void *const hw)
{
	return ((Mclk *)hw)->INTENSET.reg;
}

static inline void hri_mclk_write_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t data)
{
	((Mclk *)hw)->INTENSET.reg = data;
	((Mclk *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_mclk_clear_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	((Mclk *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_mclk_get_INTFLAG_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTFLAG.reg & MCLK_INTFLAG_CKRDY) >> MCLK_INTFLAG_CKRDY_Pos;
}

static inline void hri_mclk_clear_INTFLAG_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTFLAG.reg = MCLK_INTFLAG_CKRDY;
}

static inline bool hri_mclk_get_interrupt_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTFLAG.reg & MCLK_INTFLAG_CKRDY) >> MCLK_INTFLAG_CKRDY_Pos;
}

static inline void hri_mclk_clear_interrupt_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTFLAG.reg = MCLK_INTFLAG_CKRDY;
}

static inline hri_mclk_intflag_reg_t hri_mclk_get_INTFLAG_reg(const void *const hw, hri_mclk_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mclk_intflag_reg_t hri_mclk_read_INTFLAG_reg(const void *const hw)
{
	return ((Mclk *)hw)->INTFLAG.reg;
}

static inline void hri_mclk_clear_INTFLAG_reg(const void *const hw, hri_mclk_intflag_reg_t mask)
{
	((Mclk *)hw)->INTFLAG.reg = mask;
}

static inline void hri_mclk_set_CPUDIV_DIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg |= MCLK_CPUDIV_DIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_get_CPUDIV_DIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp = (tmp & MCLK_CPUDIV_DIV(mask)) >> MCLK_CPUDIV_DIV_Pos;
	return tmp;
}

static inline void hri_mclk_write_CPUDIV_DIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t data)
{
	uint8_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp &= ~MCLK_CPUDIV_DIV_Msk;
	tmp |= MCLK_CPUDIV_DIV(data);
	((Mclk *)hw)->CPUDIV.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_CPUDIV_DIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg &= ~MCLK_CPUDIV_DIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_CPUDIV_DIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg ^= MCLK_CPUDIV_DIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_read_CPUDIV_DIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp = (tmp & MCLK_CPUDIV_DIV_Msk) >> MCLK_CPUDIV_DIV_Pos;
	return tmp;
}

static inline void hri_mclk_set_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_get_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_read_CPUDIV_reg(const void *const hw)
{
	return ((Mclk *)hw)->CPUDIV.reg;
}

static inline void hri_mclk_set_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB0) >> MCLK_AHBMASK_HPB0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB0;
	tmp |= value << MCLK_AHBMASK_HPB0_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB1) >> MCLK_AHBMASK_HPB1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB1;
	tmp |= value << MCLK_AHBMASK_HPB1_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB2) >> MCLK_AHBMASK_HPB2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB2;
	tmp |= value << MCLK_AHBMASK_HPB2_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB3) >> MCLK_AHBMASK_HPB3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB3;
	tmp |= value << MCLK_AHBMASK_HPB3_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_DSU) >> MCLK_AHBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_DSU;
	tmp |= value << MCLK_AHBMASK_DSU_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HMATRIX_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HMATRIX) >> MCLK_AHBMASK_HMATRIX_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HMATRIX_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HMATRIX;
	tmp |= value << MCLK_AHBMASK_HMATRIX_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_NVMCTRL) >> MCLK_AHBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_NVMCTRL;
	tmp |= value << MCLK_AHBMASK_NVMCTRL_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HSRAM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HSRAM) >> MCLK_AHBMASK_HSRAM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HSRAM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HSRAM;
	tmp |= value << MCLK_AHBMASK_HSRAM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_CMCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_CMCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_CMCC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_CMCC) >> MCLK_AHBMASK_CMCC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_CMCC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_CMCC;
	tmp |= value << MCLK_AHBMASK_CMCC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_CMCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_CMCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_CMCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_CMCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_DMAC) >> MCLK_AHBMASK_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_DMAC;
	tmp |= value << MCLK_AHBMASK_DMAC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_USB) >> MCLK_AHBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_USB;
	tmp |= value << MCLK_AHBMASK_USB_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_BKUPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_BKUPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_BKUPRAM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_BKUPRAM) >> MCLK_AHBMASK_BKUPRAM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_BKUPRAM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_BKUPRAM;
	tmp |= value << MCLK_AHBMASK_BKUPRAM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_BKUPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_BKUPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_BKUPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_BKUPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_PAC) >> MCLK_AHBMASK_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_PAC;
	tmp |= value << MCLK_AHBMASK_PAC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_QSPI_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_QSPI) >> MCLK_AHBMASK_QSPI_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_QSPI_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_QSPI;
	tmp |= value << MCLK_AHBMASK_QSPI_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_SDHC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_SDHC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_SDHC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_SDHC0) >> MCLK_AHBMASK_SDHC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_SDHC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_SDHC0;
	tmp |= value << MCLK_AHBMASK_SDHC0_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_SDHC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_SDHC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_SDHC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_SDHC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_SDHC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_SDHC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_SDHC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_SDHC1) >> MCLK_AHBMASK_SDHC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_SDHC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_SDHC1;
	tmp |= value << MCLK_AHBMASK_SDHC1_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_SDHC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_SDHC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_SDHC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_SDHC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_ICM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_ICM) >> MCLK_AHBMASK_ICM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_ICM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_ICM;
	tmp |= value << MCLK_AHBMASK_ICM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_PUKCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_PUKCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_PUKCC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_PUKCC) >> MCLK_AHBMASK_PUKCC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_PUKCC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_PUKCC;
	tmp |= value << MCLK_AHBMASK_PUKCC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_PUKCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_PUKCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_PUKCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_PUKCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_QSPI_2X_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_QSPI_2X;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_QSPI_2X_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_QSPI_2X) >> MCLK_AHBMASK_QSPI_2X_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_QSPI_2X_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_QSPI_2X;
	tmp |= value << MCLK_AHBMASK_QSPI_2X_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_QSPI_2X_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_QSPI_2X;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_QSPI_2X_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_QSPI_2X;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_NVMCTRL_SMEEPROM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_NVMCTRL_SMEEPROM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_NVMCTRL_SMEEPROM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_NVMCTRL_SMEEPROM) >> MCLK_AHBMASK_NVMCTRL_SMEEPROM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_NVMCTRL_SMEEPROM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_NVMCTRL_SMEEPROM;
	tmp |= value << MCLK_AHBMASK_NVMCTRL_SMEEPROM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_NVMCTRL_SMEEPROM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_NVMCTRL_SMEEPROM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_NVMCTRL_SMEEPROM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_NVMCTRL_SMEEPROM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_NVMCTRL_CACHE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_NVMCTRL_CACHE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_NVMCTRL_CACHE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_NVMCTRL_CACHE) >> MCLK_AHBMASK_NVMCTRL_CACHE_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_NVMCTRL_CACHE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_NVMCTRL_CACHE;
	tmp |= value << MCLK_AHBMASK_NVMCTRL_CACHE_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_NVMCTRL_CACHE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_NVMCTRL_CACHE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_NVMCTRL_CACHE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_NVMCTRL_CACHE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ahbmask_reg_t hri_mclk_get_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ahbmask_reg_t hri_mclk_read_AHBMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->AHBMASK.reg;
}

static inline void hri_mclk_set_APBAMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_PAC) >> MCLK_APBAMASK_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_PAC;
	tmp |= value << MCLK_APBAMASK_PAC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_PM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_PM) >> MCLK_APBAMASK_PM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_PM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_PM;
	tmp |= value << MCLK_APBAMASK_PM_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_MCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_MCLK) >> MCLK_APBAMASK_MCLK_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_MCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_MCLK;
	tmp |= value << MCLK_APBAMASK_MCLK_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_RSTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_RSTC) >> MCLK_APBAMASK_RSTC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_RSTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_RSTC;
	tmp |= value << MCLK_APBAMASK_RSTC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_OSCCTRL) >> MCLK_APBAMASK_OSCCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_OSCCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_OSCCTRL;
	tmp |= value << MCLK_APBAMASK_OSCCTRL_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_OSC32KCTRL) >> MCLK_APBAMASK_OSC32KCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_OSC32KCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_OSC32KCTRL;
	tmp |= value << MCLK_APBAMASK_OSC32KCTRL_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_SUPC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_SUPC) >> MCLK_APBAMASK_SUPC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_SUPC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_SUPC;
	tmp |= value << MCLK_APBAMASK_SUPC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_GCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_GCLK) >> MCLK_APBAMASK_GCLK_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_GCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_GCLK;
	tmp |= value << MCLK_APBAMASK_GCLK_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_WDT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_WDT) >> MCLK_APBAMASK_WDT_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_WDT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_WDT;
	tmp |= value << MCLK_APBAMASK_WDT_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_RTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_RTC) >> MCLK_APBAMASK_RTC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_RTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_RTC;
	tmp |= value << MCLK_APBAMASK_RTC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_EIC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_EIC) >> MCLK_APBAMASK_EIC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_EIC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_EIC;
	tmp |= value << MCLK_APBAMASK_EIC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_FREQM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_FREQM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_FREQM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_FREQM) >> MCLK_APBAMASK_FREQM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_FREQM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_FREQM;
	tmp |= value << MCLK_APBAMASK_FREQM_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_FREQM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_FREQM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_FREQM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_FREQM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_SERCOM0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_SERCOM0) >> MCLK_APBAMASK_SERCOM0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_SERCOM0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_SERCOM0;
	tmp |= value << MCLK_APBAMASK_SERCOM0_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_SERCOM1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_SERCOM1) >> MCLK_APBAMASK_SERCOM1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_SERCOM1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_SERCOM1;
	tmp |= value << MCLK_APBAMASK_SERCOM1_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_TC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_TC0) >> MCLK_APBAMASK_TC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_TC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_TC0;
	tmp |= value << MCLK_APBAMASK_TC0_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_TC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_TC1) >> MCLK_APBAMASK_TC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_TC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_TC1;
	tmp |= value << MCLK_APBAMASK_TC1_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbamask_reg_t hri_mclk_get_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbamask_reg_t hri_mclk_read_APBAMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBAMASK.reg;
}

static inline void hri_mclk_set_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_USB) >> MCLK_APBBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_USB;
	tmp |= value << MCLK_APBBMASK_USB_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_DSU) >> MCLK_APBBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_DSU;
	tmp |= value << MCLK_APBBMASK_DSU_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_NVMCTRL) >> MCLK_APBBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_NVMCTRL;
	tmp |= value << MCLK_APBBMASK_NVMCTRL_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_PORT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_PORT) >> MCLK_APBBMASK_PORT_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_PORT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_PORT;
	tmp |= value << MCLK_APBBMASK_PORT_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_HMATRIX_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_HMATRIX) >> MCLK_APBBMASK_HMATRIX_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_HMATRIX_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_HMATRIX;
	tmp |= value << MCLK_APBBMASK_HMATRIX_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_HMATRIX_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_HMATRIX;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_EVSYS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_EVSYS) >> MCLK_APBBMASK_EVSYS_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_EVSYS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_EVSYS;
	tmp |= value << MCLK_APBBMASK_EVSYS_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_SERCOM2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_SERCOM2) >> MCLK_APBBMASK_SERCOM2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_SERCOM2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_SERCOM2;
	tmp |= value << MCLK_APBBMASK_SERCOM2_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_SERCOM3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_SERCOM3) >> MCLK_APBBMASK_SERCOM3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_SERCOM3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_SERCOM3;
	tmp |= value << MCLK_APBBMASK_SERCOM3_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_TCC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_TCC0) >> MCLK_APBBMASK_TCC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_TCC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_TCC0;
	tmp |= value << MCLK_APBBMASK_TCC0_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_TCC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_TCC1) >> MCLK_APBBMASK_TCC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_TCC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_TCC1;
	tmp |= value << MCLK_APBBMASK_TCC1_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_TC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_TC2) >> MCLK_APBBMASK_TC2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_TC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_TC2;
	tmp |= value << MCLK_APBBMASK_TC2_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_TC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_TC3) >> MCLK_APBBMASK_TC3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_TC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_TC3;
	tmp |= value << MCLK_APBBMASK_TC3_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_TAL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_TAL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_TAL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_TAL) >> MCLK_APBBMASK_TAL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_TAL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_TAL;
	tmp |= value << MCLK_APBBMASK_TAL_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_TAL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_TAL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_TAL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_TAL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_RAMECC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_RAMECC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_RAMECC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_RAMECC) >> MCLK_APBBMASK_RAMECC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_RAMECC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_RAMECC;
	tmp |= value << MCLK_APBBMASK_RAMECC_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_RAMECC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_RAMECC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_RAMECC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_RAMECC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbbmask_reg_t hri_mclk_get_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbbmask_reg_t hri_mclk_read_APBBMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBBMASK.reg;
}

static inline void hri_mclk_set_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TCC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TCC2) >> MCLK_APBCMASK_TCC2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TCC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TCC2;
	tmp |= value << MCLK_APBCMASK_TCC2_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TCC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TCC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TCC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TCC3) >> MCLK_APBCMASK_TCC3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TCC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TCC3;
	tmp |= value << MCLK_APBCMASK_TCC3_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TCC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TCC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TCC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TCC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC4) >> MCLK_APBCMASK_TC4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC4;
	tmp |= value << MCLK_APBCMASK_TC4_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC5) >> MCLK_APBCMASK_TC5_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC5;
	tmp |= value << MCLK_APBCMASK_TC5_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_PDEC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_PDEC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_PDEC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_PDEC) >> MCLK_APBCMASK_PDEC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_PDEC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_PDEC;
	tmp |= value << MCLK_APBCMASK_PDEC_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_PDEC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_PDEC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_PDEC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_PDEC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_AC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_AC) >> MCLK_APBCMASK_AC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_AC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_AC;
	tmp |= value << MCLK_APBCMASK_AC_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_AES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_AES) >> MCLK_APBCMASK_AES_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_AES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_AES;
	tmp |= value << MCLK_APBCMASK_AES_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TRNG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TRNG) >> MCLK_APBCMASK_TRNG_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TRNG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TRNG;
	tmp |= value << MCLK_APBCMASK_TRNG_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_ICM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_ICM) >> MCLK_APBCMASK_ICM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_ICM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_ICM;
	tmp |= value << MCLK_APBCMASK_ICM_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_ICM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_ICM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_QSPI_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_QSPI) >> MCLK_APBCMASK_QSPI_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_QSPI_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_QSPI;
	tmp |= value << MCLK_APBCMASK_QSPI_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_QSPI_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_QSPI;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_CCL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_CCL) >> MCLK_APBCMASK_CCL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_CCL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_CCL;
	tmp |= value << MCLK_APBCMASK_CCL_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbcmask_reg_t hri_mclk_get_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbcmask_reg_t hri_mclk_read_APBCMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBCMASK.reg;
}

static inline void hri_mclk_set_APBDMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_SERCOM4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_SERCOM4) >> MCLK_APBDMASK_SERCOM4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_SERCOM4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_SERCOM4;
	tmp |= value << MCLK_APBDMASK_SERCOM4_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_SERCOM5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_SERCOM5) >> MCLK_APBDMASK_SERCOM5_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_SERCOM5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_SERCOM5;
	tmp |= value << MCLK_APBDMASK_SERCOM5_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_SERCOM6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_SERCOM6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_SERCOM6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_SERCOM6) >> MCLK_APBDMASK_SERCOM6_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_SERCOM6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_SERCOM6;
	tmp |= value << MCLK_APBDMASK_SERCOM6_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_SERCOM6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_SERCOM6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_SERCOM6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_SERCOM7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_SERCOM7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_SERCOM7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_SERCOM7) >> MCLK_APBDMASK_SERCOM7_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_SERCOM7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_SERCOM7;
	tmp |= value << MCLK_APBDMASK_SERCOM7_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_SERCOM7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_SERCOM7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_SERCOM7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_TCC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_TCC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_TCC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_TCC4) >> MCLK_APBDMASK_TCC4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_TCC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_TCC4;
	tmp |= value << MCLK_APBDMASK_TCC4_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_TCC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_TCC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_TCC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_TCC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_TC6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_TC6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_TC6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_TC6) >> MCLK_APBDMASK_TC6_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_TC6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_TC6;
	tmp |= value << MCLK_APBDMASK_TC6_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_TC6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_TC6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_TC6_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_TC6;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_TC7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_TC7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_TC7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_TC7) >> MCLK_APBDMASK_TC7_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_TC7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_TC7;
	tmp |= value << MCLK_APBDMASK_TC7_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_TC7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_TC7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_TC7_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_TC7;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_ADC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_ADC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_ADC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_ADC0) >> MCLK_APBDMASK_ADC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_ADC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_ADC0;
	tmp |= value << MCLK_APBDMASK_ADC0_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_ADC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_ADC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_ADC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_ADC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_ADC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_ADC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_ADC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_ADC1) >> MCLK_APBDMASK_ADC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_ADC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_ADC1;
	tmp |= value << MCLK_APBDMASK_ADC1_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_ADC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_ADC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_ADC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_ADC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_DAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_DAC) >> MCLK_APBDMASK_DAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_DAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_DAC;
	tmp |= value << MCLK_APBDMASK_DAC_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_I2S_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_I2S;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_I2S_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_I2S) >> MCLK_APBDMASK_I2S_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_I2S_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_I2S;
	tmp |= value << MCLK_APBDMASK_I2S_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_I2S_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_I2S;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_I2S_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_I2S;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_PCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_PCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_PCC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_PCC) >> MCLK_APBDMASK_PCC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_PCC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_PCC;
	tmp |= value << MCLK_APBDMASK_PCC_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_PCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_PCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_PCC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_PCC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbdmask_reg_t hri_mclk_get_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbdmask_reg_t hri_mclk_read_APBDMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBDMASK.reg;
}

static inline hri_mclk_hsdiv_reg_t hri_mclk_get_HSDIV_DIV_bf(const void *const hw, hri_mclk_hsdiv_reg_t mask)
{
	return (((Mclk *)hw)->HSDIV.reg & MCLK_HSDIV_DIV(mask)) >> MCLK_HSDIV_DIV_Pos;
}

static inline hri_mclk_hsdiv_reg_t hri_mclk_read_HSDIV_DIV_bf(const void *const hw)
{
	return (((Mclk *)hw)->HSDIV.reg & MCLK_HSDIV_DIV_Msk) >> MCLK_HSDIV_DIV_Pos;
}

static inline hri_mclk_hsdiv_reg_t hri_mclk_get_HSDIV_reg(const void *const hw, hri_mclk_hsdiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->HSDIV.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mclk_hsdiv_reg_t hri_mclk_read_HSDIV_reg(const void *const hw)
{
	return ((Mclk *)hw)->HSDIV.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_MCLK_D51_H_INCLUDED */
#endif /* _SAMD51_MCLK_COMPONENT_ */
