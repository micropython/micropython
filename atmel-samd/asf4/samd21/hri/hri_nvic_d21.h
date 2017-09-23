/**
 * \file
 *
 * \brief SAM NVIC
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

#ifdef _SAMD21_NVIC_COMPONENT_
#ifndef _HRI_NVIC_D21_H_INCLUDED_
#define _HRI_NVIC_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_NVIC_CRITICAL_SECTIONS)
#define NVIC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define NVIC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define NVIC_CRITICAL_SECTION_ENTER()
#define NVIC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_nvic_nvicicer_reg_t;
typedef uint32_t hri_nvic_nvicicpr_reg_t;
typedef uint32_t hri_nvic_nvicipr0_reg_t;
typedef uint32_t hri_nvic_nvicipr1_reg_t;
typedef uint32_t hri_nvic_nvicipr2_reg_t;
typedef uint32_t hri_nvic_nvicipr3_reg_t;
typedef uint32_t hri_nvic_nvicipr4_reg_t;
typedef uint32_t hri_nvic_nvicipr5_reg_t;
typedef uint32_t hri_nvic_nvicipr6_reg_t;
typedef uint32_t hri_nvic_nvicipr7_reg_t;
typedef uint32_t hri_nvic_nviciser_reg_t;
typedef uint32_t hri_nvic_nvicispr_reg_t;

static inline void hri_nvic_set_NVICISER_SETENA_bf(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg |= NVIC_NVICISER_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser_reg_t hri_nvic_get_NVICISER_SETENA_bf(const void *const       hw,
                                                                      hri_nvic_nviciser_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER.reg;
	tmp = (tmp & NVIC_NVICISER_SETENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISER_SETENA_bf(const void *const hw, hri_nvic_nviciser_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISER.reg;
	tmp &= ~NVIC_NVICISER_SETENA_Msk;
	tmp |= NVIC_NVICISER_SETENA(data);
	((Nvic *)hw)->NVICISER.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER_SETENA_bf(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg &= ~NVIC_NVICISER_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER_SETENA_bf(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg ^= NVIC_NVICISER_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser_reg_t hri_nvic_read_NVICISER_SETENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER.reg;
	tmp = (tmp & NVIC_NVICISER_SETENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISER_reg(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser_reg_t hri_nvic_get_NVICISER_reg(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISER_reg(const void *const hw, hri_nvic_nviciser_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER_reg(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER_reg(const void *const hw, hri_nvic_nviciser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser_reg_t hri_nvic_read_NVICISER_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISER.reg;
}

static inline void hri_nvic_set_NVICICER_CLRENA_bf(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg |= NVIC_NVICICER_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer_reg_t hri_nvic_get_NVICICER_CLRENA_bf(const void *const       hw,
                                                                      hri_nvic_nvicicer_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER.reg;
	tmp = (tmp & NVIC_NVICICER_CLRENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICER_CLRENA_bf(const void *const hw, hri_nvic_nvicicer_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICER.reg;
	tmp &= ~NVIC_NVICICER_CLRENA_Msk;
	tmp |= NVIC_NVICICER_CLRENA(data);
	((Nvic *)hw)->NVICICER.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER_CLRENA_bf(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg &= ~NVIC_NVICICER_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER_CLRENA_bf(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg ^= NVIC_NVICICER_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer_reg_t hri_nvic_read_NVICICER_CLRENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER.reg;
	tmp = (tmp & NVIC_NVICICER_CLRENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICER_reg(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer_reg_t hri_nvic_get_NVICICER_reg(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICER_reg(const void *const hw, hri_nvic_nvicicer_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER_reg(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER_reg(const void *const hw, hri_nvic_nvicicer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer_reg_t hri_nvic_read_NVICICER_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICER.reg;
}

static inline void hri_nvic_set_NVICISPR_SETPEND_bf(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg |= NVIC_NVICISPR_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr_reg_t hri_nvic_get_NVICISPR_SETPEND_bf(const void *const       hw,
                                                                       hri_nvic_nvicispr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR.reg;
	tmp = (tmp & NVIC_NVICISPR_SETPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR_SETPEND_bf(const void *const hw, hri_nvic_nvicispr_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISPR.reg;
	tmp &= ~NVIC_NVICISPR_SETPEND_Msk;
	tmp |= NVIC_NVICISPR_SETPEND(data);
	((Nvic *)hw)->NVICISPR.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR_SETPEND_bf(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg &= ~NVIC_NVICISPR_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR_SETPEND_bf(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg ^= NVIC_NVICISPR_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr_reg_t hri_nvic_read_NVICISPR_SETPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR.reg;
	tmp = (tmp & NVIC_NVICISPR_SETPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISPR_reg(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr_reg_t hri_nvic_get_NVICISPR_reg(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR_reg(const void *const hw, hri_nvic_nvicispr_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR_reg(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR_reg(const void *const hw, hri_nvic_nvicispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr_reg_t hri_nvic_read_NVICISPR_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISPR.reg;
}

static inline void hri_nvic_set_NVICICPR_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg |= NVIC_NVICICPR_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr_reg_t hri_nvic_get_NVICICPR_CLRPEND_bf(const void *const       hw,
                                                                       hri_nvic_nvicicpr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR.reg;
	tmp = (tmp & NVIC_NVICICPR_CLRPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICPR.reg;
	tmp &= ~NVIC_NVICICPR_CLRPEND_Msk;
	tmp |= NVIC_NVICICPR_CLRPEND(data);
	((Nvic *)hw)->NVICICPR.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg &= ~NVIC_NVICICPR_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg ^= NVIC_NVICICPR_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr_reg_t hri_nvic_read_NVICICPR_CLRPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR.reg;
	tmp = (tmp & NVIC_NVICICPR_CLRPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICPR_reg(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr_reg_t hri_nvic_get_NVICICPR_reg(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR_reg(const void *const hw, hri_nvic_nvicicpr_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR_reg(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR_reg(const void *const hw, hri_nvic_nvicicpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr_reg_t hri_nvic_read_NVICICPR_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICPR.reg;
}

static inline void hri_nvic_set_NVICIPR0_PRI0_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg |= NVIC_NVICIPR0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_get_NVICIPR0_PRI0_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI0(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR0_PRI0_bf(const void *const hw, hri_nvic_nvicipr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp &= ~NVIC_NVICIPR0_PRI0_Msk;
	tmp |= NVIC_NVICIPR0_PRI0(data);
	((Nvic *)hw)->NVICIPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR0_PRI0_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg &= ~NVIC_NVICIPR0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR0_PRI0_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg ^= NVIC_NVICIPR0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_read_NVICIPR0_PRI0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI0_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR0_PRI1_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg |= NVIC_NVICIPR0_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_get_NVICIPR0_PRI1_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI1(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR0_PRI1_bf(const void *const hw, hri_nvic_nvicipr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp &= ~NVIC_NVICIPR0_PRI1_Msk;
	tmp |= NVIC_NVICIPR0_PRI1(data);
	((Nvic *)hw)->NVICIPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR0_PRI1_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg &= ~NVIC_NVICIPR0_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR0_PRI1_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg ^= NVIC_NVICIPR0_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_read_NVICIPR0_PRI1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI1_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR0_PRI2_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg |= NVIC_NVICIPR0_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_get_NVICIPR0_PRI2_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI2(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR0_PRI2_bf(const void *const hw, hri_nvic_nvicipr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp &= ~NVIC_NVICIPR0_PRI2_Msk;
	tmp |= NVIC_NVICIPR0_PRI2(data);
	((Nvic *)hw)->NVICIPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR0_PRI2_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg &= ~NVIC_NVICIPR0_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR0_PRI2_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg ^= NVIC_NVICIPR0_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_read_NVICIPR0_PRI2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI2_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR0_PRI3_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg |= NVIC_NVICIPR0_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_get_NVICIPR0_PRI3_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI3(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR0_PRI3_bf(const void *const hw, hri_nvic_nvicipr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp &= ~NVIC_NVICIPR0_PRI3_Msk;
	tmp |= NVIC_NVICIPR0_PRI3(data);
	((Nvic *)hw)->NVICIPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR0_PRI3_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg &= ~NVIC_NVICIPR0_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR0_PRI3_bf(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg ^= NVIC_NVICIPR0_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_read_NVICIPR0_PRI3_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp = (tmp & NVIC_NVICIPR0_PRI3_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR0_reg(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_get_NVICIPR0_reg(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR0_reg(const void *const hw, hri_nvic_nvicipr0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR0_reg(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR0_reg(const void *const hw, hri_nvic_nvicipr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr0_reg_t hri_nvic_read_NVICIPR0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR0.reg;
}

static inline void hri_nvic_set_NVICIPR1_PRI4_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg |= NVIC_NVICIPR1_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_get_NVICIPR1_PRI4_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI4(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR1_PRI4_bf(const void *const hw, hri_nvic_nvicipr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp &= ~NVIC_NVICIPR1_PRI4_Msk;
	tmp |= NVIC_NVICIPR1_PRI4(data);
	((Nvic *)hw)->NVICIPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR1_PRI4_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg &= ~NVIC_NVICIPR1_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR1_PRI4_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg ^= NVIC_NVICIPR1_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_read_NVICIPR1_PRI4_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI4_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR1_PRI5_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg |= NVIC_NVICIPR1_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_get_NVICIPR1_PRI5_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI5(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR1_PRI5_bf(const void *const hw, hri_nvic_nvicipr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp &= ~NVIC_NVICIPR1_PRI5_Msk;
	tmp |= NVIC_NVICIPR1_PRI5(data);
	((Nvic *)hw)->NVICIPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR1_PRI5_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg &= ~NVIC_NVICIPR1_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR1_PRI5_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg ^= NVIC_NVICIPR1_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_read_NVICIPR1_PRI5_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI5_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR1_PRI6_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg |= NVIC_NVICIPR1_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_get_NVICIPR1_PRI6_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI6(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR1_PRI6_bf(const void *const hw, hri_nvic_nvicipr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp &= ~NVIC_NVICIPR1_PRI6_Msk;
	tmp |= NVIC_NVICIPR1_PRI6(data);
	((Nvic *)hw)->NVICIPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR1_PRI6_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg &= ~NVIC_NVICIPR1_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR1_PRI6_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg ^= NVIC_NVICIPR1_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_read_NVICIPR1_PRI6_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI6_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR1_PRI7_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg |= NVIC_NVICIPR1_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_get_NVICIPR1_PRI7_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI7(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR1_PRI7_bf(const void *const hw, hri_nvic_nvicipr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp &= ~NVIC_NVICIPR1_PRI7_Msk;
	tmp |= NVIC_NVICIPR1_PRI7(data);
	((Nvic *)hw)->NVICIPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR1_PRI7_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg &= ~NVIC_NVICIPR1_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR1_PRI7_bf(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg ^= NVIC_NVICIPR1_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_read_NVICIPR1_PRI7_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp = (tmp & NVIC_NVICIPR1_PRI7_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR1_reg(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_get_NVICIPR1_reg(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR1_reg(const void *const hw, hri_nvic_nvicipr1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR1_reg(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR1_reg(const void *const hw, hri_nvic_nvicipr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr1_reg_t hri_nvic_read_NVICIPR1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR1.reg;
}

static inline void hri_nvic_set_NVICIPR2_PRI8_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg |= NVIC_NVICIPR2_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_get_NVICIPR2_PRI8_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI8(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR2_PRI8_bf(const void *const hw, hri_nvic_nvicipr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp &= ~NVIC_NVICIPR2_PRI8_Msk;
	tmp |= NVIC_NVICIPR2_PRI8(data);
	((Nvic *)hw)->NVICIPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR2_PRI8_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg &= ~NVIC_NVICIPR2_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR2_PRI8_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg ^= NVIC_NVICIPR2_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_read_NVICIPR2_PRI8_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI8_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR2_PRI9_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg |= NVIC_NVICIPR2_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_get_NVICIPR2_PRI9_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI9(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR2_PRI9_bf(const void *const hw, hri_nvic_nvicipr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp &= ~NVIC_NVICIPR2_PRI9_Msk;
	tmp |= NVIC_NVICIPR2_PRI9(data);
	((Nvic *)hw)->NVICIPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR2_PRI9_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg &= ~NVIC_NVICIPR2_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR2_PRI9_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg ^= NVIC_NVICIPR2_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_read_NVICIPR2_PRI9_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI9_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR2_PRI10_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg |= NVIC_NVICIPR2_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_get_NVICIPR2_PRI10_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI10(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR2_PRI10_bf(const void *const hw, hri_nvic_nvicipr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp &= ~NVIC_NVICIPR2_PRI10_Msk;
	tmp |= NVIC_NVICIPR2_PRI10(data);
	((Nvic *)hw)->NVICIPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR2_PRI10_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg &= ~NVIC_NVICIPR2_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR2_PRI10_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg ^= NVIC_NVICIPR2_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_read_NVICIPR2_PRI10_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI10_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR2_PRI11_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg |= NVIC_NVICIPR2_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_get_NVICIPR2_PRI11_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI11(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR2_PRI11_bf(const void *const hw, hri_nvic_nvicipr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp &= ~NVIC_NVICIPR2_PRI11_Msk;
	tmp |= NVIC_NVICIPR2_PRI11(data);
	((Nvic *)hw)->NVICIPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR2_PRI11_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg &= ~NVIC_NVICIPR2_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR2_PRI11_bf(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg ^= NVIC_NVICIPR2_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_read_NVICIPR2_PRI11_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp = (tmp & NVIC_NVICIPR2_PRI11_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR2_reg(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_get_NVICIPR2_reg(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR2_reg(const void *const hw, hri_nvic_nvicipr2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR2_reg(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR2_reg(const void *const hw, hri_nvic_nvicipr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr2_reg_t hri_nvic_read_NVICIPR2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR2.reg;
}

static inline void hri_nvic_set_NVICIPR3_PRI12_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg |= NVIC_NVICIPR3_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_get_NVICIPR3_PRI12_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI12(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR3_PRI12_bf(const void *const hw, hri_nvic_nvicipr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp &= ~NVIC_NVICIPR3_PRI12_Msk;
	tmp |= NVIC_NVICIPR3_PRI12(data);
	((Nvic *)hw)->NVICIPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR3_PRI12_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg &= ~NVIC_NVICIPR3_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR3_PRI12_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg ^= NVIC_NVICIPR3_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_read_NVICIPR3_PRI12_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI12_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR3_PRI13_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg |= NVIC_NVICIPR3_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_get_NVICIPR3_PRI13_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI13(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR3_PRI13_bf(const void *const hw, hri_nvic_nvicipr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp &= ~NVIC_NVICIPR3_PRI13_Msk;
	tmp |= NVIC_NVICIPR3_PRI13(data);
	((Nvic *)hw)->NVICIPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR3_PRI13_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg &= ~NVIC_NVICIPR3_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR3_PRI13_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg ^= NVIC_NVICIPR3_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_read_NVICIPR3_PRI13_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI13_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR3_PRI14_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg |= NVIC_NVICIPR3_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_get_NVICIPR3_PRI14_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI14(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR3_PRI14_bf(const void *const hw, hri_nvic_nvicipr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp &= ~NVIC_NVICIPR3_PRI14_Msk;
	tmp |= NVIC_NVICIPR3_PRI14(data);
	((Nvic *)hw)->NVICIPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR3_PRI14_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg &= ~NVIC_NVICIPR3_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR3_PRI14_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg ^= NVIC_NVICIPR3_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_read_NVICIPR3_PRI14_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI14_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR3_PRI15_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg |= NVIC_NVICIPR3_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_get_NVICIPR3_PRI15_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI15(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR3_PRI15_bf(const void *const hw, hri_nvic_nvicipr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp &= ~NVIC_NVICIPR3_PRI15_Msk;
	tmp |= NVIC_NVICIPR3_PRI15(data);
	((Nvic *)hw)->NVICIPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR3_PRI15_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg &= ~NVIC_NVICIPR3_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR3_PRI15_bf(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg ^= NVIC_NVICIPR3_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_read_NVICIPR3_PRI15_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp = (tmp & NVIC_NVICIPR3_PRI15_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR3_reg(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_get_NVICIPR3_reg(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR3_reg(const void *const hw, hri_nvic_nvicipr3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR3_reg(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR3_reg(const void *const hw, hri_nvic_nvicipr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr3_reg_t hri_nvic_read_NVICIPR3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR3.reg;
}

static inline void hri_nvic_set_NVICIPR4_PRI16_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg |= NVIC_NVICIPR4_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_get_NVICIPR4_PRI16_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI16(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR4_PRI16_bf(const void *const hw, hri_nvic_nvicipr4_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp &= ~NVIC_NVICIPR4_PRI16_Msk;
	tmp |= NVIC_NVICIPR4_PRI16(data);
	((Nvic *)hw)->NVICIPR4.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR4_PRI16_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg &= ~NVIC_NVICIPR4_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR4_PRI16_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg ^= NVIC_NVICIPR4_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_read_NVICIPR4_PRI16_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI16_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR4_PRI17_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg |= NVIC_NVICIPR4_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_get_NVICIPR4_PRI17_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI17(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR4_PRI17_bf(const void *const hw, hri_nvic_nvicipr4_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp &= ~NVIC_NVICIPR4_PRI17_Msk;
	tmp |= NVIC_NVICIPR4_PRI17(data);
	((Nvic *)hw)->NVICIPR4.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR4_PRI17_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg &= ~NVIC_NVICIPR4_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR4_PRI17_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg ^= NVIC_NVICIPR4_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_read_NVICIPR4_PRI17_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI17_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR4_PRI18_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg |= NVIC_NVICIPR4_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_get_NVICIPR4_PRI18_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI18(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR4_PRI18_bf(const void *const hw, hri_nvic_nvicipr4_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp &= ~NVIC_NVICIPR4_PRI18_Msk;
	tmp |= NVIC_NVICIPR4_PRI18(data);
	((Nvic *)hw)->NVICIPR4.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR4_PRI18_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg &= ~NVIC_NVICIPR4_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR4_PRI18_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg ^= NVIC_NVICIPR4_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_read_NVICIPR4_PRI18_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI18_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR4_PRI19_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg |= NVIC_NVICIPR4_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_get_NVICIPR4_PRI19_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI19(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR4_PRI19_bf(const void *const hw, hri_nvic_nvicipr4_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp &= ~NVIC_NVICIPR4_PRI19_Msk;
	tmp |= NVIC_NVICIPR4_PRI19(data);
	((Nvic *)hw)->NVICIPR4.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR4_PRI19_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg &= ~NVIC_NVICIPR4_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR4_PRI19_bf(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg ^= NVIC_NVICIPR4_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_read_NVICIPR4_PRI19_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp = (tmp & NVIC_NVICIPR4_PRI19_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR4_reg(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_get_NVICIPR4_reg(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR4.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR4_reg(const void *const hw, hri_nvic_nvicipr4_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR4_reg(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR4_reg(const void *const hw, hri_nvic_nvicipr4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR4.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr4_reg_t hri_nvic_read_NVICIPR4_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR4.reg;
}

static inline void hri_nvic_set_NVICIPR5_PRI20_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg |= NVIC_NVICIPR5_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_get_NVICIPR5_PRI20_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI20(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR5_PRI20_bf(const void *const hw, hri_nvic_nvicipr5_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp &= ~NVIC_NVICIPR5_PRI20_Msk;
	tmp |= NVIC_NVICIPR5_PRI20(data);
	((Nvic *)hw)->NVICIPR5.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR5_PRI20_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg &= ~NVIC_NVICIPR5_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR5_PRI20_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg ^= NVIC_NVICIPR5_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_read_NVICIPR5_PRI20_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI20_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR5_PRI21_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg |= NVIC_NVICIPR5_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_get_NVICIPR5_PRI21_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI21(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR5_PRI21_bf(const void *const hw, hri_nvic_nvicipr5_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp &= ~NVIC_NVICIPR5_PRI21_Msk;
	tmp |= NVIC_NVICIPR5_PRI21(data);
	((Nvic *)hw)->NVICIPR5.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR5_PRI21_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg &= ~NVIC_NVICIPR5_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR5_PRI21_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg ^= NVIC_NVICIPR5_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_read_NVICIPR5_PRI21_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI21_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR5_PRI22_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg |= NVIC_NVICIPR5_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_get_NVICIPR5_PRI22_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI22(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR5_PRI22_bf(const void *const hw, hri_nvic_nvicipr5_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp &= ~NVIC_NVICIPR5_PRI22_Msk;
	tmp |= NVIC_NVICIPR5_PRI22(data);
	((Nvic *)hw)->NVICIPR5.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR5_PRI22_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg &= ~NVIC_NVICIPR5_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR5_PRI22_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg ^= NVIC_NVICIPR5_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_read_NVICIPR5_PRI22_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI22_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR5_PRI23_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg |= NVIC_NVICIPR5_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_get_NVICIPR5_PRI23_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI23(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR5_PRI23_bf(const void *const hw, hri_nvic_nvicipr5_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp &= ~NVIC_NVICIPR5_PRI23_Msk;
	tmp |= NVIC_NVICIPR5_PRI23(data);
	((Nvic *)hw)->NVICIPR5.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR5_PRI23_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg &= ~NVIC_NVICIPR5_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR5_PRI23_bf(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg ^= NVIC_NVICIPR5_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_read_NVICIPR5_PRI23_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp = (tmp & NVIC_NVICIPR5_PRI23_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR5_reg(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_get_NVICIPR5_reg(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR5.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR5_reg(const void *const hw, hri_nvic_nvicipr5_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR5_reg(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR5_reg(const void *const hw, hri_nvic_nvicipr5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR5.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr5_reg_t hri_nvic_read_NVICIPR5_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR5.reg;
}

static inline void hri_nvic_set_NVICIPR6_PRI24_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg |= NVIC_NVICIPR6_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_get_NVICIPR6_PRI24_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI24(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR6_PRI24_bf(const void *const hw, hri_nvic_nvicipr6_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp &= ~NVIC_NVICIPR6_PRI24_Msk;
	tmp |= NVIC_NVICIPR6_PRI24(data);
	((Nvic *)hw)->NVICIPR6.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR6_PRI24_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg &= ~NVIC_NVICIPR6_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR6_PRI24_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg ^= NVIC_NVICIPR6_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_read_NVICIPR6_PRI24_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI24_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR6_PRI25_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg |= NVIC_NVICIPR6_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_get_NVICIPR6_PRI25_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI25(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR6_PRI25_bf(const void *const hw, hri_nvic_nvicipr6_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp &= ~NVIC_NVICIPR6_PRI25_Msk;
	tmp |= NVIC_NVICIPR6_PRI25(data);
	((Nvic *)hw)->NVICIPR6.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR6_PRI25_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg &= ~NVIC_NVICIPR6_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR6_PRI25_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg ^= NVIC_NVICIPR6_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_read_NVICIPR6_PRI25_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI25_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR6_PRI26_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg |= NVIC_NVICIPR6_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_get_NVICIPR6_PRI26_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI26(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR6_PRI26_bf(const void *const hw, hri_nvic_nvicipr6_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp &= ~NVIC_NVICIPR6_PRI26_Msk;
	tmp |= NVIC_NVICIPR6_PRI26(data);
	((Nvic *)hw)->NVICIPR6.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR6_PRI26_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg &= ~NVIC_NVICIPR6_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR6_PRI26_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg ^= NVIC_NVICIPR6_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_read_NVICIPR6_PRI26_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI26_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR6_PRI27_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg |= NVIC_NVICIPR6_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_get_NVICIPR6_PRI27_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI27(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR6_PRI27_bf(const void *const hw, hri_nvic_nvicipr6_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp &= ~NVIC_NVICIPR6_PRI27_Msk;
	tmp |= NVIC_NVICIPR6_PRI27(data);
	((Nvic *)hw)->NVICIPR6.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR6_PRI27_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg &= ~NVIC_NVICIPR6_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR6_PRI27_bf(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg ^= NVIC_NVICIPR6_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_read_NVICIPR6_PRI27_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp = (tmp & NVIC_NVICIPR6_PRI27_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR6_reg(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_get_NVICIPR6_reg(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR6.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR6_reg(const void *const hw, hri_nvic_nvicipr6_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR6_reg(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR6_reg(const void *const hw, hri_nvic_nvicipr6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR6.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr6_reg_t hri_nvic_read_NVICIPR6_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR6.reg;
}

static inline void hri_nvic_set_NVICIPR7_PRI28_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg |= NVIC_NVICIPR7_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_get_NVICIPR7_PRI28_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI28(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR7_PRI28_bf(const void *const hw, hri_nvic_nvicipr7_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp &= ~NVIC_NVICIPR7_PRI28_Msk;
	tmp |= NVIC_NVICIPR7_PRI28(data);
	((Nvic *)hw)->NVICIPR7.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR7_PRI28_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg &= ~NVIC_NVICIPR7_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR7_PRI28_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg ^= NVIC_NVICIPR7_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_read_NVICIPR7_PRI28_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI28_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR7_PRI29_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg |= NVIC_NVICIPR7_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_get_NVICIPR7_PRI29_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI29(mask)) >> 8;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR7_PRI29_bf(const void *const hw, hri_nvic_nvicipr7_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp &= ~NVIC_NVICIPR7_PRI29_Msk;
	tmp |= NVIC_NVICIPR7_PRI29(data);
	((Nvic *)hw)->NVICIPR7.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR7_PRI29_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg &= ~NVIC_NVICIPR7_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR7_PRI29_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg ^= NVIC_NVICIPR7_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_read_NVICIPR7_PRI29_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI29_Msk) >> 8;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR7_PRI30_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg |= NVIC_NVICIPR7_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_get_NVICIPR7_PRI30_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI30(mask)) >> 16;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR7_PRI30_bf(const void *const hw, hri_nvic_nvicipr7_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp &= ~NVIC_NVICIPR7_PRI30_Msk;
	tmp |= NVIC_NVICIPR7_PRI30(data);
	((Nvic *)hw)->NVICIPR7.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR7_PRI30_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg &= ~NVIC_NVICIPR7_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR7_PRI30_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg ^= NVIC_NVICIPR7_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_read_NVICIPR7_PRI30_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI30_Msk) >> 16;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR7_PRI31_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg |= NVIC_NVICIPR7_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_get_NVICIPR7_PRI31_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI31(mask)) >> 24;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR7_PRI31_bf(const void *const hw, hri_nvic_nvicipr7_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp &= ~NVIC_NVICIPR7_PRI31_Msk;
	tmp |= NVIC_NVICIPR7_PRI31(data);
	((Nvic *)hw)->NVICIPR7.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR7_PRI31_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg &= ~NVIC_NVICIPR7_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR7_PRI31_bf(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg ^= NVIC_NVICIPR7_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_read_NVICIPR7_PRI31_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp = (tmp & NVIC_NVICIPR7_PRI31_Msk) >> 24;
	return tmp;
}

static inline void hri_nvic_set_NVICIPR7_reg(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_get_NVICIPR7_reg(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIPR7.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIPR7_reg(const void *const hw, hri_nvic_nvicipr7_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIPR7_reg(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIPR7_reg(const void *const hw, hri_nvic_nvicipr7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIPR7.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicipr7_reg_t hri_nvic_read_NVICIPR7_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIPR7.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_NVIC_D21_H_INCLUDED */
#endif /* _SAMD21_NVIC_COMPONENT_ */
