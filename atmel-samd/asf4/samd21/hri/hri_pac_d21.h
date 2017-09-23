/**
 * \file
 *
 * \brief SAM PAC
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

#ifdef _SAMD21_PAC_COMPONENT_
#ifndef _HRI_PAC_D21_H_INCLUDED_
#define _HRI_PAC_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PAC_CRITICAL_SECTIONS)
#define PAC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define PAC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define PAC_CRITICAL_SECTION_ENTER()
#define PAC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_pac_wpset_reg_t;

static inline void hri_pac_set_WP_WP_bf(const void *const hw, hri_pac_wpset_reg_t mask)
{
	((Pac *)hw)->WPSET.reg = PAC_WPSET_WP(mask);
}

static inline hri_pac_wpset_reg_t hri_pac_get_WP_WP_bf(const void *const hw, hri_pac_wpset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WPSET.reg;
	tmp = (tmp & PAC_WPSET_WP(mask)) >> PAC_WPSET_WP_Pos;
	return tmp;
}

static inline hri_pac_wpset_reg_t hri_pac_read_WP_WP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WPSET.reg;
	tmp = (tmp & PAC_WPSET_WP_Msk) >> PAC_WPSET_WP_Pos;
	return tmp;
}

static inline void hri_pac_write_WP_WP_bf(const void *const hw, hri_pac_wpset_reg_t data)
{
	((Pac *)hw)->WPSET.reg = PAC_WPSET_WP(data);
	((Pac *)hw)->WPCLR.reg = ~PAC_WPSET_WP(data);
}

static inline void hri_pac_clear_WP_WP_bf(const void *const hw, hri_pac_wpset_reg_t mask)
{
	((Pac *)hw)->WPCLR.reg = PAC_WPSET_WP(mask);
}

static inline void hri_pac_set_WP_reg(const void *const hw, hri_pac_wpset_reg_t mask)
{
	((Pac *)hw)->WPSET.reg = mask;
}

static inline hri_pac_wpset_reg_t hri_pac_get_WP_reg(const void *const hw, hri_pac_wpset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WPSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_wpset_reg_t hri_pac_read_WP_reg(const void *const hw)
{
	return ((Pac *)hw)->WPSET.reg;
}

static inline void hri_pac_write_WP_reg(const void *const hw, hri_pac_wpset_reg_t data)
{
	((Pac *)hw)->WPSET.reg = data;
	((Pac *)hw)->WPCLR.reg = ~data;
}

static inline void hri_pac_clear_WP_reg(const void *const hw, hri_pac_wpset_reg_t mask)
{
	((Pac *)hw)->WPCLR.reg = mask;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PAC_D21_H_INCLUDED */
#endif /* _SAMD21_PAC_COMPONENT_ */
