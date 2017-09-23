/**
 * \file
 *
 * \brief SAM SYSCTRL
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

#ifdef _SAMD21_SYSCTRL_COMPONENT_
#ifndef _HRI_SYSCTRL_D21_H_INCLUDED_
#define _HRI_SYSCTRL_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SYSCTRL_CRITICAL_SECTIONS)
#define SYSCTRL_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SYSCTRL_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SYSCTRL_CRITICAL_SECTION_ENTER()
#define SYSCTRL_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_sysctrl_dfllctrl_reg_t;
typedef uint16_t hri_sysctrl_vreg_reg_t;
typedef uint16_t hri_sysctrl_xosc32k_reg_t;
typedef uint16_t hri_sysctrl_xosc_reg_t;
typedef uint32_t hri_sysctrl_bod33_reg_t;
typedef uint32_t hri_sysctrl_dfllmul_reg_t;
typedef uint32_t hri_sysctrl_dfllval_reg_t;
typedef uint32_t hri_sysctrl_dpllctrlb_reg_t;
typedef uint32_t hri_sysctrl_dpllratio_reg_t;
typedef uint32_t hri_sysctrl_intenset_reg_t;
typedef uint32_t hri_sysctrl_intflag_reg_t;
typedef uint32_t hri_sysctrl_osc32k_reg_t;
typedef uint32_t hri_sysctrl_osc8m_reg_t;
typedef uint32_t hri_sysctrl_pclksr_reg_t;
typedef uint32_t hri_sysctrl_vref_reg_t;
typedef uint8_t  hri_sysctrl_dfllsync_reg_t;
typedef uint8_t  hri_sysctrl_dpllctrla_reg_t;
typedef uint8_t  hri_sysctrl_dpllstatus_reg_t;
typedef uint8_t  hri_sysctrl_osculp32k_reg_t;

static inline void hri_sysctrl_set_INTEN_XOSCRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_XOSCRDY;
}

static inline bool hri_sysctrl_get_INTEN_XOSCRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_XOSCRDY) >> SYSCTRL_INTENSET_XOSCRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_XOSCRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_XOSCRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_XOSCRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_XOSCRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_XOSCRDY;
}

static inline void hri_sysctrl_set_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_XOSC32KRDY;
}

static inline bool hri_sysctrl_get_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_XOSC32KRDY) >> SYSCTRL_INTENSET_XOSC32KRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_XOSC32KRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_XOSC32KRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_XOSC32KRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_XOSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_XOSC32KRDY;
}

static inline void hri_sysctrl_set_INTEN_OSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_OSC32KRDY;
}

static inline bool hri_sysctrl_get_INTEN_OSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_OSC32KRDY) >> SYSCTRL_INTENSET_OSC32KRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_OSC32KRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_OSC32KRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_OSC32KRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_OSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_OSC32KRDY;
}

static inline void hri_sysctrl_set_INTEN_OSC8MRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_OSC8MRDY;
}

static inline bool hri_sysctrl_get_INTEN_OSC8MRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_OSC8MRDY) >> SYSCTRL_INTENSET_OSC8MRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_OSC8MRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_OSC8MRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_OSC8MRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_OSC8MRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_OSC8MRDY;
}

static inline void hri_sysctrl_set_INTEN_DFLLRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLRDY;
}

static inline bool hri_sysctrl_get_INTEN_DFLLRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DFLLRDY) >> SYSCTRL_INTENSET_DFLLRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_DFLLRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_DFLLRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLRDY;
}

static inline void hri_sysctrl_set_INTEN_DFLLOOB_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLOOB;
}

static inline bool hri_sysctrl_get_INTEN_DFLLOOB_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DFLLOOB) >> SYSCTRL_INTENSET_DFLLOOB_Pos;
}

static inline void hri_sysctrl_write_INTEN_DFLLOOB_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLOOB;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLOOB;
	}
}

static inline void hri_sysctrl_clear_INTEN_DFLLOOB_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLOOB;
}

static inline void hri_sysctrl_set_INTEN_DFLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLLCKF;
}

static inline bool hri_sysctrl_get_INTEN_DFLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DFLLLCKF) >> SYSCTRL_INTENSET_DFLLLCKF_Pos;
}

static inline void hri_sysctrl_write_INTEN_DFLLLCKF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLLCKF;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLLCKF;
	}
}

static inline void hri_sysctrl_clear_INTEN_DFLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLLCKF;
}

static inline void hri_sysctrl_set_INTEN_DFLLLCKC_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLLCKC;
}

static inline bool hri_sysctrl_get_INTEN_DFLLLCKC_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DFLLLCKC) >> SYSCTRL_INTENSET_DFLLLCKC_Pos;
}

static inline void hri_sysctrl_write_INTEN_DFLLLCKC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLLCKC;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLLCKC;
	}
}

static inline void hri_sysctrl_clear_INTEN_DFLLLCKC_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLLCKC;
}

static inline void hri_sysctrl_set_INTEN_DFLLRCS_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLRCS;
}

static inline bool hri_sysctrl_get_INTEN_DFLLRCS_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DFLLRCS) >> SYSCTRL_INTENSET_DFLLRCS_Pos;
}

static inline void hri_sysctrl_write_INTEN_DFLLRCS_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLRCS;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DFLLRCS;
	}
}

static inline void hri_sysctrl_clear_INTEN_DFLLRCS_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DFLLRCS;
}

static inline void hri_sysctrl_set_INTEN_BOD33RDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_BOD33RDY;
}

static inline bool hri_sysctrl_get_INTEN_BOD33RDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_BOD33RDY) >> SYSCTRL_INTENSET_BOD33RDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_BOD33RDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_BOD33RDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_BOD33RDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_BOD33RDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_BOD33RDY;
}

static inline void hri_sysctrl_set_INTEN_BOD33DET_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_BOD33DET;
}

static inline bool hri_sysctrl_get_INTEN_BOD33DET_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_BOD33DET) >> SYSCTRL_INTENSET_BOD33DET_Pos;
}

static inline void hri_sysctrl_write_INTEN_BOD33DET_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_BOD33DET;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_BOD33DET;
	}
}

static inline void hri_sysctrl_clear_INTEN_BOD33DET_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_BOD33DET;
}

static inline void hri_sysctrl_set_INTEN_B33SRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_B33SRDY;
}

static inline bool hri_sysctrl_get_INTEN_B33SRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_B33SRDY) >> SYSCTRL_INTENSET_B33SRDY_Pos;
}

static inline void hri_sysctrl_write_INTEN_B33SRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_B33SRDY;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_B33SRDY;
	}
}

static inline void hri_sysctrl_clear_INTEN_B33SRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_B33SRDY;
}

static inline void hri_sysctrl_set_INTEN_DPLLLCKR_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLCKR;
}

static inline bool hri_sysctrl_get_INTEN_DPLLLCKR_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DPLLLCKR) >> SYSCTRL_INTENSET_DPLLLCKR_Pos;
}

static inline void hri_sysctrl_write_INTEN_DPLLLCKR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLCKR;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLCKR;
	}
}

static inline void hri_sysctrl_clear_INTEN_DPLLLCKR_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLCKR;
}

static inline void hri_sysctrl_set_INTEN_DPLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLCKF;
}

static inline bool hri_sysctrl_get_INTEN_DPLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DPLLLCKF) >> SYSCTRL_INTENSET_DPLLLCKF_Pos;
}

static inline void hri_sysctrl_write_INTEN_DPLLLCKF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLCKF;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLCKF;
	}
}

static inline void hri_sysctrl_clear_INTEN_DPLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLCKF;
}

static inline void hri_sysctrl_set_INTEN_DPLLLTO_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLTO;
}

static inline bool hri_sysctrl_get_INTEN_DPLLLTO_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTENSET.reg & SYSCTRL_INTENSET_DPLLLTO) >> SYSCTRL_INTENSET_DPLLLTO_Pos;
}

static inline void hri_sysctrl_write_INTEN_DPLLLTO_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLTO;
	} else {
		((Sysctrl *)hw)->INTENSET.reg = SYSCTRL_INTENSET_DPLLLTO;
	}
}

static inline void hri_sysctrl_clear_INTEN_DPLLLTO_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTENCLR.reg = SYSCTRL_INTENSET_DPLLLTO;
}

static inline void hri_sysctrl_set_INTEN_reg(const void *const hw, hri_sysctrl_intenset_reg_t mask)
{
	((Sysctrl *)hw)->INTENSET.reg = mask;
}

static inline hri_sysctrl_intenset_reg_t hri_sysctrl_get_INTEN_reg(const void *const          hw,
                                                                   hri_sysctrl_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sysctrl_intenset_reg_t hri_sysctrl_read_INTEN_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->INTENSET.reg;
}

static inline void hri_sysctrl_write_INTEN_reg(const void *const hw, hri_sysctrl_intenset_reg_t data)
{
	((Sysctrl *)hw)->INTENSET.reg = data;
	((Sysctrl *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_sysctrl_clear_INTEN_reg(const void *const hw, hri_sysctrl_intenset_reg_t mask)
{
	((Sysctrl *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_sysctrl_get_INTFLAG_XOSCRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_XOSCRDY) >> SYSCTRL_INTFLAG_XOSCRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_XOSCRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_XOSCRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_XOSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_XOSC32KRDY) >> SYSCTRL_INTFLAG_XOSC32KRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_XOSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_XOSC32KRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_OSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_OSC32KRDY) >> SYSCTRL_INTFLAG_OSC32KRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_OSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_OSC32KRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_OSC8MRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_OSC8MRDY) >> SYSCTRL_INTFLAG_OSC8MRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_OSC8MRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_OSC8MRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_DFLLRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLRDY) >> SYSCTRL_INTFLAG_DFLLRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DFLLRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_DFLLOOB_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLOOB) >> SYSCTRL_INTFLAG_DFLLOOB_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DFLLOOB_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLOOB;
}

static inline bool hri_sysctrl_get_INTFLAG_DFLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLLCKF) >> SYSCTRL_INTFLAG_DFLLLCKF_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DFLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLLCKF;
}

static inline bool hri_sysctrl_get_INTFLAG_DFLLLCKC_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLLCKC) >> SYSCTRL_INTFLAG_DFLLLCKC_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DFLLLCKC_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLLCKC;
}

static inline bool hri_sysctrl_get_INTFLAG_DFLLRCS_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLRCS) >> SYSCTRL_INTFLAG_DFLLRCS_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DFLLRCS_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLRCS;
}

static inline bool hri_sysctrl_get_INTFLAG_BOD33RDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33RDY) >> SYSCTRL_INTFLAG_BOD33RDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_BOD33RDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY;
}

static inline bool hri_sysctrl_get_INTFLAG_BOD33DET_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) >> SYSCTRL_INTFLAG_BOD33DET_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_BOD33DET_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33DET;
}

static inline bool hri_sysctrl_get_INTFLAG_B33SRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_B33SRDY) >> SYSCTRL_INTFLAG_B33SRDY_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_B33SRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_B33SRDY;
}

static inline bool hri_sysctrl_get_INTFLAG_DPLLLCKR_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLCKR) >> SYSCTRL_INTFLAG_DPLLLCKR_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DPLLLCKR_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLCKR;
}

static inline bool hri_sysctrl_get_INTFLAG_DPLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLCKF) >> SYSCTRL_INTFLAG_DPLLLCKF_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DPLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLCKF;
}

static inline bool hri_sysctrl_get_INTFLAG_DPLLLTO_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLTO) >> SYSCTRL_INTFLAG_DPLLLTO_Pos;
}

static inline void hri_sysctrl_clear_INTFLAG_DPLLLTO_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLTO;
}

static inline bool hri_sysctrl_get_interrupt_XOSCRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_XOSCRDY) >> SYSCTRL_INTFLAG_XOSCRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_XOSCRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_XOSCRDY;
}

static inline bool hri_sysctrl_get_interrupt_XOSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_XOSC32KRDY) >> SYSCTRL_INTFLAG_XOSC32KRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_XOSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_XOSC32KRDY;
}

static inline bool hri_sysctrl_get_interrupt_OSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_OSC32KRDY) >> SYSCTRL_INTFLAG_OSC32KRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_OSC32KRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_OSC32KRDY;
}

static inline bool hri_sysctrl_get_interrupt_OSC8MRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_OSC8MRDY) >> SYSCTRL_INTFLAG_OSC8MRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_OSC8MRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_OSC8MRDY;
}

static inline bool hri_sysctrl_get_interrupt_DFLLRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLRDY) >> SYSCTRL_INTFLAG_DFLLRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DFLLRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLRDY;
}

static inline bool hri_sysctrl_get_interrupt_DFLLOOB_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLOOB) >> SYSCTRL_INTFLAG_DFLLOOB_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DFLLOOB_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLOOB;
}

static inline bool hri_sysctrl_get_interrupt_DFLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLLCKF) >> SYSCTRL_INTFLAG_DFLLLCKF_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DFLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLLCKF;
}

static inline bool hri_sysctrl_get_interrupt_DFLLLCKC_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLLCKC) >> SYSCTRL_INTFLAG_DFLLLCKC_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DFLLLCKC_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLLCKC;
}

static inline bool hri_sysctrl_get_interrupt_DFLLRCS_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DFLLRCS) >> SYSCTRL_INTFLAG_DFLLRCS_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DFLLRCS_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DFLLRCS;
}

static inline bool hri_sysctrl_get_interrupt_BOD33RDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33RDY) >> SYSCTRL_INTFLAG_BOD33RDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_BOD33RDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY;
}

static inline bool hri_sysctrl_get_interrupt_BOD33DET_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) >> SYSCTRL_INTFLAG_BOD33DET_Pos;
}

static inline void hri_sysctrl_clear_interrupt_BOD33DET_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33DET;
}

static inline bool hri_sysctrl_get_interrupt_B33SRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_B33SRDY) >> SYSCTRL_INTFLAG_B33SRDY_Pos;
}

static inline void hri_sysctrl_clear_interrupt_B33SRDY_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_B33SRDY;
}

static inline bool hri_sysctrl_get_interrupt_DPLLLCKR_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLCKR) >> SYSCTRL_INTFLAG_DPLLLCKR_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DPLLLCKR_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLCKR;
}

static inline bool hri_sysctrl_get_interrupt_DPLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLCKF) >> SYSCTRL_INTFLAG_DPLLLCKF_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DPLLLCKF_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLCKF;
}

static inline bool hri_sysctrl_get_interrupt_DPLLLTO_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->INTFLAG.reg & SYSCTRL_INTFLAG_DPLLLTO) >> SYSCTRL_INTFLAG_DPLLLTO_Pos;
}

static inline void hri_sysctrl_clear_interrupt_DPLLLTO_bit(const void *const hw)
{
	((Sysctrl *)hw)->INTFLAG.reg = SYSCTRL_INTFLAG_DPLLLTO;
}

static inline hri_sysctrl_intflag_reg_t hri_sysctrl_get_INTFLAG_reg(const void *const         hw,
                                                                    hri_sysctrl_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sysctrl_intflag_reg_t hri_sysctrl_read_INTFLAG_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->INTFLAG.reg;
}

static inline void hri_sysctrl_clear_INTFLAG_reg(const void *const hw, hri_sysctrl_intflag_reg_t mask)
{
	((Sysctrl *)hw)->INTFLAG.reg = mask;
}

static inline void hri_sysctrl_set_XOSC_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_ENABLE) >> SYSCTRL_XOSC_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_ENABLE;
	tmp |= value << SYSCTRL_XOSC_ENABLE_Pos;
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC_XTALEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_XTALEN) >> SYSCTRL_XOSC_XTALEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC_XTALEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_XTALEN;
	tmp |= value << SYSCTRL_XOSC_XTALEN_Pos;
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC_RUNSTDBY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_RUNSTDBY) >> SYSCTRL_XOSC_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_RUNSTDBY;
	tmp |= value << SYSCTRL_XOSC_RUNSTDBY_Pos;
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC_ONDEMAND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_ONDEMAND) >> SYSCTRL_XOSC_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC_ONDEMAND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_ONDEMAND;
	tmp |= value << SYSCTRL_XOSC_ONDEMAND_Pos;
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC_AMPGC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_AMPGC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC_AMPGC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_AMPGC) >> SYSCTRL_XOSC_AMPGC_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC_AMPGC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_AMPGC;
	tmp |= value << SYSCTRL_XOSC_AMPGC_Pos;
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_AMPGC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_AMPGC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_AMPGC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_AMPGC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC_GAIN_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_GAIN(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_get_XOSC_GAIN_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_GAIN(mask)) >> SYSCTRL_XOSC_GAIN_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_XOSC_GAIN_bf(const void *const hw, hri_sysctrl_xosc_reg_t data)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_GAIN_Msk;
	tmp |= SYSCTRL_XOSC_GAIN(data);
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_GAIN_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_GAIN(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_GAIN_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_GAIN(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_read_XOSC_GAIN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_GAIN_Msk) >> SYSCTRL_XOSC_GAIN_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_XOSC_STARTUP_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= SYSCTRL_XOSC_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_get_XOSC_STARTUP_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_STARTUP(mask)) >> SYSCTRL_XOSC_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_XOSC_STARTUP_bf(const void *const hw, hri_sysctrl_xosc_reg_t data)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= ~SYSCTRL_XOSC_STARTUP_Msk;
	tmp |= SYSCTRL_XOSC_STARTUP(data);
	((Sysctrl *)hw)->XOSC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_STARTUP_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~SYSCTRL_XOSC_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_STARTUP_bf(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= SYSCTRL_XOSC_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_read_XOSC_STARTUP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp = (tmp & SYSCTRL_XOSC_STARTUP_Msk) >> SYSCTRL_XOSC_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_XOSC_reg(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_get_XOSC_reg(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_XOSC_reg(const void *const hw, hri_sysctrl_xosc_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC_reg(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC_reg(const void *const hw, hri_sysctrl_xosc_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc_reg_t hri_sysctrl_read_XOSC_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->XOSC.reg;
}

static inline void hri_sysctrl_set_XOSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_ENABLE) >> SYSCTRL_XOSC32K_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_ENABLE;
	tmp |= value << SYSCTRL_XOSC32K_ENABLE_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_XTALEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_XTALEN) >> SYSCTRL_XOSC32K_XTALEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_XTALEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_XTALEN;
	tmp |= value << SYSCTRL_XOSC32K_XTALEN_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_XTALEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_XTALEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_EN32K_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_EN32K) >> SYSCTRL_XOSC32K_EN32K_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_EN32K_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_EN32K;
	tmp |= value << SYSCTRL_XOSC32K_EN32K_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_EN1K_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_EN1K) >> SYSCTRL_XOSC32K_EN1K_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_EN1K_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_EN1K;
	tmp |= value << SYSCTRL_XOSC32K_EN1K_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_AAMPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_AAMPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_AAMPEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_AAMPEN) >> SYSCTRL_XOSC32K_AAMPEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_AAMPEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_AAMPEN;
	tmp |= value << SYSCTRL_XOSC32K_AAMPEN_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_AAMPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_AAMPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_AAMPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_AAMPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_RUNSTDBY) >> SYSCTRL_XOSC32K_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_RUNSTDBY;
	tmp |= value << SYSCTRL_XOSC32K_RUNSTDBY_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_ONDEMAND) >> SYSCTRL_XOSC32K_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_ONDEMAND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_ONDEMAND;
	tmp |= value << SYSCTRL_XOSC32K_ONDEMAND_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_WRTLOCK) >> SYSCTRL_XOSC32K_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_XOSC32K_WRTLOCK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_WRTLOCK;
	tmp |= value << SYSCTRL_XOSC32K_WRTLOCK_Pos;
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_XOSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= SYSCTRL_XOSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc32k_reg_t hri_sysctrl_get_XOSC32K_STARTUP_bf(const void *const         hw,
                                                                           hri_sysctrl_xosc32k_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_STARTUP(mask)) >> SYSCTRL_XOSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_XOSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_xosc32k_reg_t data)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= ~SYSCTRL_XOSC32K_STARTUP_Msk;
	tmp |= SYSCTRL_XOSC32K_STARTUP(data);
	((Sysctrl *)hw)->XOSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~SYSCTRL_XOSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= SYSCTRL_XOSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc32k_reg_t hri_sysctrl_read_XOSC32K_STARTUP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp = (tmp & SYSCTRL_XOSC32K_STARTUP_Msk) >> SYSCTRL_XOSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_XOSC32K_reg(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc32k_reg_t hri_sysctrl_get_XOSC32K_reg(const void *const         hw,
                                                                    hri_sysctrl_xosc32k_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->XOSC32K.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_XOSC32K_reg(const void *const hw, hri_sysctrl_xosc32k_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_XOSC32K_reg(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_XOSC32K_reg(const void *const hw, hri_sysctrl_xosc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->XOSC32K.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_xosc32k_reg_t hri_sysctrl_read_XOSC32K_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->XOSC32K.reg;
}

static inline void hri_sysctrl_set_OSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_ENABLE) >> SYSCTRL_OSC32K_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_ENABLE;
	tmp |= value << SYSCTRL_OSC32K_ENABLE_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_EN32K_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_EN32K) >> SYSCTRL_OSC32K_EN32K_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_EN32K_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_EN32K;
	tmp |= value << SYSCTRL_OSC32K_EN32K_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_EN32K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_EN32K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_EN1K_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_EN1K) >> SYSCTRL_OSC32K_EN1K_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_EN1K_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_EN1K;
	tmp |= value << SYSCTRL_OSC32K_EN1K_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_EN1K_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_EN1K;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_RUNSTDBY) >> SYSCTRL_OSC32K_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_RUNSTDBY;
	tmp |= value << SYSCTRL_OSC32K_RUNSTDBY_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_ONDEMAND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_ONDEMAND) >> SYSCTRL_OSC32K_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_ONDEMAND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_ONDEMAND;
	tmp |= value << SYSCTRL_OSC32K_ONDEMAND_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC32K_WRTLOCK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_WRTLOCK) >> SYSCTRL_OSC32K_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC32K_WRTLOCK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_WRTLOCK;
	tmp |= value << SYSCTRL_OSC32K_WRTLOCK_Pos;
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_get_OSC32K_STARTUP_bf(const void *const        hw,
                                                                         hri_sysctrl_osc32k_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_STARTUP(mask)) >> SYSCTRL_OSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_osc32k_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_STARTUP_Msk;
	tmp |= SYSCTRL_OSC32K_STARTUP(data);
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_STARTUP_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_STARTUP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_read_OSC32K_STARTUP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_STARTUP_Msk) >> SYSCTRL_OSC32K_STARTUP_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSC32K_CALIB_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= SYSCTRL_OSC32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_get_OSC32K_CALIB_bf(const void *const        hw,
                                                                       hri_sysctrl_osc32k_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_CALIB(mask)) >> SYSCTRL_OSC32K_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSC32K_CALIB_bf(const void *const hw, hri_sysctrl_osc32k_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= ~SYSCTRL_OSC32K_CALIB_Msk;
	tmp |= SYSCTRL_OSC32K_CALIB(data);
	((Sysctrl *)hw)->OSC32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_CALIB_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~SYSCTRL_OSC32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_CALIB_bf(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= SYSCTRL_OSC32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_read_OSC32K_CALIB_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp = (tmp & SYSCTRL_OSC32K_CALIB_Msk) >> SYSCTRL_OSC32K_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSC32K_reg(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_get_OSC32K_reg(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC32K.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_OSC32K_reg(const void *const hw, hri_sysctrl_osc32k_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC32K_reg(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC32K_reg(const void *const hw, hri_sysctrl_osc32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC32K.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc32k_reg_t hri_sysctrl_read_OSC32K_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->OSC32K.reg;
}

static inline void hri_sysctrl_set_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg |= SYSCTRL_OSCULP32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & SYSCTRL_OSCULP32K_WRTLOCK) >> SYSCTRL_OSCULP32K_WRTLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSCULP32K_WRTLOCK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp &= ~SYSCTRL_OSCULP32K_WRTLOCK;
	tmp |= value << SYSCTRL_OSCULP32K_WRTLOCK_Pos;
	((Sysctrl *)hw)->OSCULP32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg &= ~SYSCTRL_OSCULP32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSCULP32K_WRTLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg ^= SYSCTRL_OSCULP32K_WRTLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSCULP32K_CALIB_bf(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg |= SYSCTRL_OSCULP32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osculp32k_reg_t hri_sysctrl_get_OSCULP32K_CALIB_bf(const void *const           hw,
                                                                             hri_sysctrl_osculp32k_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & SYSCTRL_OSCULP32K_CALIB(mask)) >> SYSCTRL_OSCULP32K_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSCULP32K_CALIB_bf(const void *const hw, hri_sysctrl_osculp32k_reg_t data)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp &= ~SYSCTRL_OSCULP32K_CALIB_Msk;
	tmp |= SYSCTRL_OSCULP32K_CALIB(data);
	((Sysctrl *)hw)->OSCULP32K.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSCULP32K_CALIB_bf(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg &= ~SYSCTRL_OSCULP32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSCULP32K_CALIB_bf(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg ^= SYSCTRL_OSCULP32K_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osculp32k_reg_t hri_sysctrl_read_OSCULP32K_CALIB_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp = (tmp & SYSCTRL_OSCULP32K_CALIB_Msk) >> SYSCTRL_OSCULP32K_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSCULP32K_reg(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osculp32k_reg_t hri_sysctrl_get_OSCULP32K_reg(const void *const           hw,
                                                                        hri_sysctrl_osculp32k_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->OSCULP32K.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_OSCULP32K_reg(const void *const hw, hri_sysctrl_osculp32k_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSCULP32K_reg(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSCULP32K_reg(const void *const hw, hri_sysctrl_osculp32k_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSCULP32K.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osculp32k_reg_t hri_sysctrl_read_OSCULP32K_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->OSCULP32K.reg;
}

static inline void hri_sysctrl_set_OSC8M_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC8M_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_ENABLE) >> SYSCTRL_OSC8M_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC8M_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_ENABLE;
	tmp |= value << SYSCTRL_OSC8M_ENABLE_Pos;
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC8M_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC8M_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_RUNSTDBY) >> SYSCTRL_OSC8M_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC8M_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_RUNSTDBY;
	tmp |= value << SYSCTRL_OSC8M_RUNSTDBY_Pos;
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC8M_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_OSC8M_ONDEMAND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_ONDEMAND) >> SYSCTRL_OSC8M_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_OSC8M_ONDEMAND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_ONDEMAND;
	tmp |= value << SYSCTRL_OSC8M_ONDEMAND_Pos;
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_OSC8M_PRESC_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_PRESC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_get_OSC8M_PRESC_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_PRESC(mask)) >> SYSCTRL_OSC8M_PRESC_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSC8M_PRESC_bf(const void *const hw, hri_sysctrl_osc8m_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_PRESC_Msk;
	tmp |= SYSCTRL_OSC8M_PRESC(data);
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_PRESC_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_PRESC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_PRESC_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_PRESC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_read_OSC8M_PRESC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_PRESC_Msk) >> SYSCTRL_OSC8M_PRESC_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSC8M_CALIB_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_get_OSC8M_CALIB_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_CALIB(mask)) >> SYSCTRL_OSC8M_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSC8M_CALIB_bf(const void *const hw, hri_sysctrl_osc8m_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_CALIB_Msk;
	tmp |= SYSCTRL_OSC8M_CALIB(data);
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_CALIB_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_CALIB_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_read_OSC8M_CALIB_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_CALIB_Msk) >> SYSCTRL_OSC8M_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSC8M_FRANGE_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= SYSCTRL_OSC8M_FRANGE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_get_OSC8M_FRANGE_bf(const void *const       hw,
                                                                      hri_sysctrl_osc8m_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_FRANGE(mask)) >> SYSCTRL_OSC8M_FRANGE_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_OSC8M_FRANGE_bf(const void *const hw, hri_sysctrl_osc8m_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= ~SYSCTRL_OSC8M_FRANGE_Msk;
	tmp |= SYSCTRL_OSC8M_FRANGE(data);
	((Sysctrl *)hw)->OSC8M.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_FRANGE_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~SYSCTRL_OSC8M_FRANGE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_FRANGE_bf(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= SYSCTRL_OSC8M_FRANGE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_read_OSC8M_FRANGE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp = (tmp & SYSCTRL_OSC8M_FRANGE_Msk) >> SYSCTRL_OSC8M_FRANGE_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_OSC8M_reg(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_get_OSC8M_reg(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->OSC8M.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_OSC8M_reg(const void *const hw, hri_sysctrl_osc8m_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_OSC8M_reg(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_OSC8M_reg(const void *const hw, hri_sysctrl_osc8m_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->OSC8M.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_osc8m_reg_t hri_sysctrl_read_OSC8M_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->OSC8M.reg;
}

static inline void hri_sysctrl_set_DFLLCTRL_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_ENABLE) >> SYSCTRL_DFLLCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_ENABLE;
	tmp |= value << SYSCTRL_DFLLCTRL_ENABLE_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_MODE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_MODE) >> SYSCTRL_DFLLCTRL_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_MODE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_MODE;
	tmp |= value << SYSCTRL_DFLLCTRL_MODE_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_STABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_STABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_STABLE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_STABLE) >> SYSCTRL_DFLLCTRL_STABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_STABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_STABLE;
	tmp |= value << SYSCTRL_DFLLCTRL_STABLE_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_STABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_STABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_STABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_STABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_LLAW_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_LLAW;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_LLAW_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_LLAW) >> SYSCTRL_DFLLCTRL_LLAW_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_LLAW_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_LLAW;
	tmp |= value << SYSCTRL_DFLLCTRL_LLAW_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_LLAW_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_LLAW;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_LLAW_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_LLAW;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_USBCRM_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_USBCRM;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_USBCRM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_USBCRM) >> SYSCTRL_DFLLCTRL_USBCRM_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_USBCRM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_USBCRM;
	tmp |= value << SYSCTRL_DFLLCTRL_USBCRM_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_USBCRM_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_USBCRM;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_USBCRM_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_USBCRM;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_RUNSTDBY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_RUNSTDBY) >> SYSCTRL_DFLLCTRL_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_RUNSTDBY;
	tmp |= value << SYSCTRL_DFLLCTRL_RUNSTDBY_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_ONDEMAND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_ONDEMAND) >> SYSCTRL_DFLLCTRL_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_ONDEMAND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_ONDEMAND;
	tmp |= value << SYSCTRL_DFLLCTRL_ONDEMAND_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_CCDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_CCDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_CCDIS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_CCDIS) >> SYSCTRL_DFLLCTRL_CCDIS_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_CCDIS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_CCDIS;
	tmp |= value << SYSCTRL_DFLLCTRL_CCDIS_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_CCDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_CCDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_CCDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_CCDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_QLDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_QLDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_QLDIS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_QLDIS) >> SYSCTRL_DFLLCTRL_QLDIS_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_QLDIS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_QLDIS;
	tmp |= value << SYSCTRL_DFLLCTRL_QLDIS_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_QLDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_QLDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_QLDIS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_QLDIS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_BPLCKC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_BPLCKC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_BPLCKC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_BPLCKC) >> SYSCTRL_DFLLCTRL_BPLCKC_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_BPLCKC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_BPLCKC;
	tmp |= value << SYSCTRL_DFLLCTRL_BPLCKC_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_BPLCKC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_BPLCKC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_BPLCKC_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_BPLCKC;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_WAITLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_WAITLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLCTRL_WAITLOCK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp = (tmp & SYSCTRL_DFLLCTRL_WAITLOCK) >> SYSCTRL_DFLLCTRL_WAITLOCK_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_WAITLOCK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= ~SYSCTRL_DFLLCTRL_WAITLOCK;
	tmp |= value << SYSCTRL_DFLLCTRL_WAITLOCK_Pos;
	((Sysctrl *)hw)->DFLLCTRL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_WAITLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_WAITLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_WAITLOCK_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= SYSCTRL_DFLLCTRL_WAITLOCK;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLCTRL_reg(const void *const hw, hri_sysctrl_dfllctrl_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllctrl_reg_t hri_sysctrl_get_DFLLCTRL_reg(const void *const          hw,
                                                                      hri_sysctrl_dfllctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLCTRL_reg(const void *const hw, hri_sysctrl_dfllctrl_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLCTRL_reg(const void *const hw, hri_sysctrl_dfllctrl_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLCTRL_reg(const void *const hw, hri_sysctrl_dfllctrl_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLCTRL.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllctrl_reg_t hri_sysctrl_read_DFLLCTRL_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DFLLCTRL.reg;
}

static inline void hri_sysctrl_set_DFLLVAL_FINE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg |= SYSCTRL_DFLLVAL_FINE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_get_DFLLVAL_FINE_bf(const void *const         hw,
                                                                        hri_sysctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_FINE(mask)) >> SYSCTRL_DFLLVAL_FINE_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLVAL_FINE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp &= ~SYSCTRL_DFLLVAL_FINE_Msk;
	tmp |= SYSCTRL_DFLLVAL_FINE(data);
	((Sysctrl *)hw)->DFLLVAL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLVAL_FINE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg &= ~SYSCTRL_DFLLVAL_FINE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLVAL_FINE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg ^= SYSCTRL_DFLLVAL_FINE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_read_DFLLVAL_FINE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_FINE_Msk) >> SYSCTRL_DFLLVAL_FINE_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLVAL_COARSE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg |= SYSCTRL_DFLLVAL_COARSE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_get_DFLLVAL_COARSE_bf(const void *const         hw,
                                                                          hri_sysctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_COARSE(mask)) >> SYSCTRL_DFLLVAL_COARSE_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLVAL_COARSE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp &= ~SYSCTRL_DFLLVAL_COARSE_Msk;
	tmp |= SYSCTRL_DFLLVAL_COARSE(data);
	((Sysctrl *)hw)->DFLLVAL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLVAL_COARSE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg &= ~SYSCTRL_DFLLVAL_COARSE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLVAL_COARSE_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg ^= SYSCTRL_DFLLVAL_COARSE(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_read_DFLLVAL_COARSE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_COARSE_Msk) >> SYSCTRL_DFLLVAL_COARSE_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLVAL_DIFF_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg |= SYSCTRL_DFLLVAL_DIFF(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_get_DFLLVAL_DIFF_bf(const void *const         hw,
                                                                        hri_sysctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_DIFF(mask)) >> SYSCTRL_DFLLVAL_DIFF_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLVAL_DIFF_bf(const void *const hw, hri_sysctrl_dfllval_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp &= ~SYSCTRL_DFLLVAL_DIFF_Msk;
	tmp |= SYSCTRL_DFLLVAL_DIFF(data);
	((Sysctrl *)hw)->DFLLVAL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLVAL_DIFF_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg &= ~SYSCTRL_DFLLVAL_DIFF(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLVAL_DIFF_bf(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg ^= SYSCTRL_DFLLVAL_DIFF(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_read_DFLLVAL_DIFF_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp = (tmp & SYSCTRL_DFLLVAL_DIFF_Msk) >> SYSCTRL_DFLLVAL_DIFF_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLVAL_reg(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_get_DFLLVAL_reg(const void *const         hw,
                                                                    hri_sysctrl_dfllval_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLVAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLVAL_reg(const void *const hw, hri_sysctrl_dfllval_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLVAL_reg(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLVAL_reg(const void *const hw, hri_sysctrl_dfllval_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLVAL.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllval_reg_t hri_sysctrl_read_DFLLVAL_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DFLLVAL.reg;
}

static inline void hri_sysctrl_set_DFLLMUL_MUL_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg |= SYSCTRL_DFLLMUL_MUL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_get_DFLLMUL_MUL_bf(const void *const         hw,
                                                                       hri_sysctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_MUL(mask)) >> SYSCTRL_DFLLMUL_MUL_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLMUL_MUL_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp &= ~SYSCTRL_DFLLMUL_MUL_Msk;
	tmp |= SYSCTRL_DFLLMUL_MUL(data);
	((Sysctrl *)hw)->DFLLMUL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLMUL_MUL_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg &= ~SYSCTRL_DFLLMUL_MUL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLMUL_MUL_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg ^= SYSCTRL_DFLLMUL_MUL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_read_DFLLMUL_MUL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_MUL_Msk) >> SYSCTRL_DFLLMUL_MUL_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLMUL_FSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg |= SYSCTRL_DFLLMUL_FSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_get_DFLLMUL_FSTEP_bf(const void *const         hw,
                                                                         hri_sysctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_FSTEP(mask)) >> SYSCTRL_DFLLMUL_FSTEP_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLMUL_FSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp &= ~SYSCTRL_DFLLMUL_FSTEP_Msk;
	tmp |= SYSCTRL_DFLLMUL_FSTEP(data);
	((Sysctrl *)hw)->DFLLMUL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLMUL_FSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg &= ~SYSCTRL_DFLLMUL_FSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLMUL_FSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg ^= SYSCTRL_DFLLMUL_FSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_read_DFLLMUL_FSTEP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_FSTEP_Msk) >> SYSCTRL_DFLLMUL_FSTEP_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLMUL_CSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg |= SYSCTRL_DFLLMUL_CSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_get_DFLLMUL_CSTEP_bf(const void *const         hw,
                                                                         hri_sysctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_CSTEP(mask)) >> SYSCTRL_DFLLMUL_CSTEP_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLMUL_CSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp &= ~SYSCTRL_DFLLMUL_CSTEP_Msk;
	tmp |= SYSCTRL_DFLLMUL_CSTEP(data);
	((Sysctrl *)hw)->DFLLMUL.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLMUL_CSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg &= ~SYSCTRL_DFLLMUL_CSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLMUL_CSTEP_bf(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg ^= SYSCTRL_DFLLMUL_CSTEP(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_read_DFLLMUL_CSTEP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp = (tmp & SYSCTRL_DFLLMUL_CSTEP_Msk) >> SYSCTRL_DFLLMUL_CSTEP_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DFLLMUL_reg(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_get_DFLLMUL_reg(const void *const         hw,
                                                                    hri_sysctrl_dfllmul_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLMUL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLMUL_reg(const void *const hw, hri_sysctrl_dfllmul_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLMUL_reg(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLMUL_reg(const void *const hw, hri_sysctrl_dfllmul_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLMUL.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllmul_reg_t hri_sysctrl_read_DFLLMUL_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DFLLMUL.reg;
}

static inline void hri_sysctrl_set_DFLLSYNC_READREQ_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg |= SYSCTRL_DFLLSYNC_READREQ;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DFLLSYNC_READREQ_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLSYNC.reg;
	tmp = (tmp & SYSCTRL_DFLLSYNC_READREQ) >> SYSCTRL_DFLLSYNC_READREQ_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DFLLSYNC_READREQ_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DFLLSYNC.reg;
	tmp &= ~SYSCTRL_DFLLSYNC_READREQ;
	tmp |= value << SYSCTRL_DFLLSYNC_READREQ_Pos;
	((Sysctrl *)hw)->DFLLSYNC.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLSYNC_READREQ_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg &= ~SYSCTRL_DFLLSYNC_READREQ;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLSYNC_READREQ_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg ^= SYSCTRL_DFLLSYNC_READREQ;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DFLLSYNC_reg(const void *const hw, hri_sysctrl_dfllsync_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllsync_reg_t hri_sysctrl_get_DFLLSYNC_reg(const void *const          hw,
                                                                      hri_sysctrl_dfllsync_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DFLLSYNC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DFLLSYNC_reg(const void *const hw, hri_sysctrl_dfllsync_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DFLLSYNC_reg(const void *const hw, hri_sysctrl_dfllsync_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DFLLSYNC_reg(const void *const hw, hri_sysctrl_dfllsync_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DFLLSYNC.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dfllsync_reg_t hri_sysctrl_read_DFLLSYNC_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DFLLSYNC.reg;
}

static inline void hri_sysctrl_set_BOD33_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_BOD33_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_ENABLE) >> SYSCTRL_BOD33_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_BOD33_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_ENABLE;
	tmp |= value << SYSCTRL_BOD33_ENABLE_Pos;
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_BOD33_HYST_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_HYST;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_BOD33_HYST_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_HYST) >> SYSCTRL_BOD33_HYST_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_BOD33_HYST_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_HYST;
	tmp |= value << SYSCTRL_BOD33_HYST_Pos;
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_HYST_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_HYST;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_HYST_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_HYST;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_BOD33_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_RUNSTDBY) >> SYSCTRL_BOD33_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_BOD33_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_RUNSTDBY;
	tmp |= value << SYSCTRL_BOD33_RUNSTDBY_Pos;
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_BOD33_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_BOD33_MODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_MODE) >> SYSCTRL_BOD33_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_BOD33_MODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_MODE;
	tmp |= value << SYSCTRL_BOD33_MODE_Pos;
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_MODE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_MODE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_BOD33_CEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_CEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_BOD33_CEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_CEN) >> SYSCTRL_BOD33_CEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_BOD33_CEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_CEN;
	tmp |= value << SYSCTRL_BOD33_CEN_Pos;
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_CEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_CEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_CEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_CEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_BOD33_ACTION_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_ACTION(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_get_BOD33_ACTION_bf(const void *const       hw,
                                                                      hri_sysctrl_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_ACTION(mask)) >> SYSCTRL_BOD33_ACTION_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_BOD33_ACTION_bf(const void *const hw, hri_sysctrl_bod33_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_ACTION_Msk;
	tmp |= SYSCTRL_BOD33_ACTION(data);
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_ACTION_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_ACTION(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_ACTION_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_ACTION(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_read_BOD33_ACTION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_ACTION_Msk) >> SYSCTRL_BOD33_ACTION_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_BOD33_PSEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_PSEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_get_BOD33_PSEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_PSEL(mask)) >> SYSCTRL_BOD33_PSEL_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_BOD33_PSEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_PSEL_Msk;
	tmp |= SYSCTRL_BOD33_PSEL(data);
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_PSEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_PSEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_PSEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_PSEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_read_BOD33_PSEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_PSEL_Msk) >> SYSCTRL_BOD33_PSEL_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_BOD33_LEVEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= SYSCTRL_BOD33_LEVEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_get_BOD33_LEVEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_LEVEL(mask)) >> SYSCTRL_BOD33_LEVEL_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_BOD33_LEVEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= ~SYSCTRL_BOD33_LEVEL_Msk;
	tmp |= SYSCTRL_BOD33_LEVEL(data);
	((Sysctrl *)hw)->BOD33.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_LEVEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~SYSCTRL_BOD33_LEVEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_LEVEL_bf(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= SYSCTRL_BOD33_LEVEL(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_read_BOD33_LEVEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp = (tmp & SYSCTRL_BOD33_LEVEL_Msk) >> SYSCTRL_BOD33_LEVEL_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_BOD33_reg(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_get_BOD33_reg(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->BOD33.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_BOD33_reg(const void *const hw, hri_sysctrl_bod33_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_BOD33_reg(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_BOD33_reg(const void *const hw, hri_sysctrl_bod33_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->BOD33.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_bod33_reg_t hri_sysctrl_read_BOD33_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->BOD33.reg;
}

static inline void hri_sysctrl_set_VREG_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg |= SYSCTRL_VREG_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_VREG_RUNSTDBY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->VREG.reg;
	tmp = (tmp & SYSCTRL_VREG_RUNSTDBY) >> SYSCTRL_VREG_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_VREG_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->VREG.reg;
	tmp &= ~SYSCTRL_VREG_RUNSTDBY;
	tmp |= value << SYSCTRL_VREG_RUNSTDBY_Pos;
	((Sysctrl *)hw)->VREG.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREG_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg &= ~SYSCTRL_VREG_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREG_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg ^= SYSCTRL_VREG_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_VREG_FORCELDO_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg |= SYSCTRL_VREG_FORCELDO;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_VREG_FORCELDO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->VREG.reg;
	tmp = (tmp & SYSCTRL_VREG_FORCELDO) >> SYSCTRL_VREG_FORCELDO_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_VREG_FORCELDO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->VREG.reg;
	tmp &= ~SYSCTRL_VREG_FORCELDO;
	tmp |= value << SYSCTRL_VREG_FORCELDO_Pos;
	((Sysctrl *)hw)->VREG.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREG_FORCELDO_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg &= ~SYSCTRL_VREG_FORCELDO;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREG_FORCELDO_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg ^= SYSCTRL_VREG_FORCELDO;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_VREG_reg(const void *const hw, hri_sysctrl_vreg_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vreg_reg_t hri_sysctrl_get_VREG_reg(const void *const hw, hri_sysctrl_vreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sysctrl *)hw)->VREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_VREG_reg(const void *const hw, hri_sysctrl_vreg_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREG_reg(const void *const hw, hri_sysctrl_vreg_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREG_reg(const void *const hw, hri_sysctrl_vreg_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREG.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vreg_reg_t hri_sysctrl_read_VREG_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->VREG.reg;
}

static inline void hri_sysctrl_set_VREF_TSEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg |= SYSCTRL_VREF_TSEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_VREF_TSEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp = (tmp & SYSCTRL_VREF_TSEN) >> SYSCTRL_VREF_TSEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_VREF_TSEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp &= ~SYSCTRL_VREF_TSEN;
	tmp |= value << SYSCTRL_VREF_TSEN_Pos;
	((Sysctrl *)hw)->VREF.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREF_TSEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg &= ~SYSCTRL_VREF_TSEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREF_TSEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg ^= SYSCTRL_VREF_TSEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_VREF_BGOUTEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_VREF_BGOUTEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp = (tmp & SYSCTRL_VREF_BGOUTEN) >> SYSCTRL_VREF_BGOUTEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_VREF_BGOUTEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp &= ~SYSCTRL_VREF_BGOUTEN;
	tmp |= value << SYSCTRL_VREF_BGOUTEN_Pos;
	((Sysctrl *)hw)->VREF.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREF_BGOUTEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREF_BGOUTEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg ^= SYSCTRL_VREF_BGOUTEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_VREF_CALIB_bf(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg |= SYSCTRL_VREF_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vref_reg_t hri_sysctrl_get_VREF_CALIB_bf(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp = (tmp & SYSCTRL_VREF_CALIB(mask)) >> SYSCTRL_VREF_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_VREF_CALIB_bf(const void *const hw, hri_sysctrl_vref_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp &= ~SYSCTRL_VREF_CALIB_Msk;
	tmp |= SYSCTRL_VREF_CALIB(data);
	((Sysctrl *)hw)->VREF.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREF_CALIB_bf(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg &= ~SYSCTRL_VREF_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREF_CALIB_bf(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg ^= SYSCTRL_VREF_CALIB(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vref_reg_t hri_sysctrl_read_VREF_CALIB_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp = (tmp & SYSCTRL_VREF_CALIB_Msk) >> SYSCTRL_VREF_CALIB_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_VREF_reg(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vref_reg_t hri_sysctrl_get_VREF_reg(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->VREF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_VREF_reg(const void *const hw, hri_sysctrl_vref_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_VREF_reg(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_VREF_reg(const void *const hw, hri_sysctrl_vref_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->VREF.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_vref_reg_t hri_sysctrl_read_VREF_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->VREF.reg;
}

static inline void hri_sysctrl_set_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg |= SYSCTRL_DPLLCTRLA_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLA_ENABLE) >> SYSCTRL_DPLLCTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp &= ~SYSCTRL_DPLLCTRLA_ENABLE;
	tmp |= value << SYSCTRL_DPLLCTRLA_ENABLE_Pos;
	((Sysctrl *)hw)->DPLLCTRLA.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg &= ~SYSCTRL_DPLLCTRLA_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLA_ENABLE_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg ^= SYSCTRL_DPLLCTRLA_ENABLE;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg |= SYSCTRL_DPLLCTRLA_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLA_RUNSTDBY) >> SYSCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp &= ~SYSCTRL_DPLLCTRLA_RUNSTDBY;
	tmp |= value << SYSCTRL_DPLLCTRLA_RUNSTDBY_Pos;
	((Sysctrl *)hw)->DPLLCTRLA.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg &= ~SYSCTRL_DPLLCTRLA_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLA_RUNSTDBY_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg ^= SYSCTRL_DPLLCTRLA_RUNSTDBY;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg |= SYSCTRL_DPLLCTRLA_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLA_ONDEMAND) >> SYSCTRL_DPLLCTRLA_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLA_ONDEMAND_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp &= ~SYSCTRL_DPLLCTRLA_ONDEMAND;
	tmp |= value << SYSCTRL_DPLLCTRLA_ONDEMAND_Pos;
	((Sysctrl *)hw)->DPLLCTRLA.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg &= ~SYSCTRL_DPLLCTRLA_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLA_ONDEMAND_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg ^= SYSCTRL_DPLLCTRLA_ONDEMAND;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLA_reg(const void *const hw, hri_sysctrl_dpllctrla_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrla_reg_t hri_sysctrl_get_DPLLCTRLA_reg(const void *const           hw,
                                                                        hri_sysctrl_dpllctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLA_reg(const void *const hw, hri_sysctrl_dpllctrla_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLA_reg(const void *const hw, hri_sysctrl_dpllctrla_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLA_reg(const void *const hw, hri_sysctrl_dpllctrla_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLA.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrla_reg_t hri_sysctrl_read_DPLLCTRLA_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DPLLCTRLA.reg;
}

static inline void hri_sysctrl_set_DPLLRATIO_LDR_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg |= SYSCTRL_DPLLRATIO_LDR(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_get_DPLLRATIO_LDR_bf(const void *const           hw,
                                                                           hri_sysctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp = (tmp & SYSCTRL_DPLLRATIO_LDR(mask)) >> SYSCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLRATIO_LDR_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp &= ~SYSCTRL_DPLLRATIO_LDR_Msk;
	tmp |= SYSCTRL_DPLLRATIO_LDR(data);
	((Sysctrl *)hw)->DPLLRATIO.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLRATIO_LDR_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg &= ~SYSCTRL_DPLLRATIO_LDR(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLRATIO_LDR_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg ^= SYSCTRL_DPLLRATIO_LDR(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_read_DPLLRATIO_LDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp = (tmp & SYSCTRL_DPLLRATIO_LDR_Msk) >> SYSCTRL_DPLLRATIO_LDR_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg |= SYSCTRL_DPLLRATIO_LDRFRAC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_get_DPLLRATIO_LDRFRAC_bf(const void *const           hw,
                                                                               hri_sysctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp = (tmp & SYSCTRL_DPLLRATIO_LDRFRAC(mask)) >> SYSCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp &= ~SYSCTRL_DPLLRATIO_LDRFRAC_Msk;
	tmp |= SYSCTRL_DPLLRATIO_LDRFRAC(data);
	((Sysctrl *)hw)->DPLLRATIO.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg &= ~SYSCTRL_DPLLRATIO_LDRFRAC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLRATIO_LDRFRAC_bf(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg ^= SYSCTRL_DPLLRATIO_LDRFRAC(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_read_DPLLRATIO_LDRFRAC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp = (tmp & SYSCTRL_DPLLRATIO_LDRFRAC_Msk) >> SYSCTRL_DPLLRATIO_LDRFRAC_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLRATIO_reg(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_get_DPLLRATIO_reg(const void *const           hw,
                                                                        hri_sysctrl_dpllratio_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLRATIO.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLRATIO_reg(const void *const hw, hri_sysctrl_dpllratio_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLRATIO_reg(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLRATIO_reg(const void *const hw, hri_sysctrl_dpllratio_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLRATIO.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllratio_reg_t hri_sysctrl_read_DPLLRATIO_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DPLLRATIO.reg;
}

static inline void hri_sysctrl_set_DPLLCTRLB_LPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_LPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLB_LPEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_LPEN) >> SYSCTRL_DPLLCTRLB_LPEN_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_LPEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_LPEN;
	tmp |= value << SYSCTRL_DPLLCTRLB_LPEN_Pos;
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_LPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_LPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_LPEN_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_LPEN;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLB_WUF_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_WUF;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLB_WUF_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_WUF) >> SYSCTRL_DPLLCTRLB_WUF_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_WUF_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_WUF;
	tmp |= value << SYSCTRL_DPLLCTRLB_WUF_Pos;
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_WUF_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_WUF;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_WUF_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_WUF;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_LBYPASS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sysctrl_get_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_LBYPASS) >> SYSCTRL_DPLLCTRLB_LBYPASS_Pos;
	return (bool)tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_LBYPASS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_LBYPASS;
	tmp |= value << SYSCTRL_DPLLCTRLB_LBYPASS_Pos;
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_LBYPASS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_LBYPASS_bit(const void *const hw)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_LBYPASS;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_set_DPLLCTRLB_FILTER_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_FILTER(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_get_DPLLCTRLB_FILTER_bf(const void *const           hw,
                                                                              hri_sysctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_FILTER(mask)) >> SYSCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_FILTER_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_FILTER_Msk;
	tmp |= SYSCTRL_DPLLCTRLB_FILTER(data);
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_FILTER_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_FILTER(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_FILTER_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_FILTER(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_read_DPLLCTRLB_FILTER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_FILTER_Msk) >> SYSCTRL_DPLLCTRLB_FILTER_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_REFCLK(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_get_DPLLCTRLB_REFCLK_bf(const void *const           hw,
                                                                              hri_sysctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_REFCLK(mask)) >> SYSCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_REFCLK_Msk;
	tmp |= SYSCTRL_DPLLCTRLB_REFCLK(data);
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_REFCLK(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_REFCLK_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_REFCLK(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_read_DPLLCTRLB_REFCLK_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_REFCLK_Msk) >> SYSCTRL_DPLLCTRLB_REFCLK_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLCTRLB_LTIME_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_LTIME(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_get_DPLLCTRLB_LTIME_bf(const void *const           hw,
                                                                             hri_sysctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_LTIME(mask)) >> SYSCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_LTIME_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_LTIME_Msk;
	tmp |= SYSCTRL_DPLLCTRLB_LTIME(data);
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_LTIME_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_LTIME(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_LTIME_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_LTIME(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_read_DPLLCTRLB_LTIME_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_LTIME_Msk) >> SYSCTRL_DPLLCTRLB_LTIME_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLCTRLB_DIV_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= SYSCTRL_DPLLCTRLB_DIV(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_get_DPLLCTRLB_DIV_bf(const void *const           hw,
                                                                           hri_sysctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_DIV(mask)) >> SYSCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_DIV_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t data)
{
	uint32_t tmp;
	SYSCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= ~SYSCTRL_DPLLCTRLB_DIV_Msk;
	tmp |= SYSCTRL_DPLLCTRLB_DIV(data);
	((Sysctrl *)hw)->DPLLCTRLB.reg = tmp;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_DIV_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~SYSCTRL_DPLLCTRLB_DIV(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_DIV_bf(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= SYSCTRL_DPLLCTRLB_DIV(mask);
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_read_DPLLCTRLB_DIV_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp = (tmp & SYSCTRL_DPLLCTRLB_DIV_Msk) >> SYSCTRL_DPLLCTRLB_DIV_Pos;
	return tmp;
}

static inline void hri_sysctrl_set_DPLLCTRLB_reg(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg |= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_get_DPLLCTRLB_reg(const void *const           hw,
                                                                        hri_sysctrl_dpllctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLCTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sysctrl_write_DPLLCTRLB_reg(const void *const hw, hri_sysctrl_dpllctrlb_reg_t data)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg = data;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_clear_DPLLCTRLB_reg(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg &= ~mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sysctrl_toggle_DPLLCTRLB_reg(const void *const hw, hri_sysctrl_dpllctrlb_reg_t mask)
{
	SYSCTRL_CRITICAL_SECTION_ENTER();
	((Sysctrl *)hw)->DPLLCTRLB.reg ^= mask;
	SYSCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_sysctrl_dpllctrlb_reg_t hri_sysctrl_read_DPLLCTRLB_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DPLLCTRLB.reg;
}

static inline bool hri_sysctrl_get_PCLKSR_XOSCRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_XOSCRDY) >> SYSCTRL_PCLKSR_XOSCRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_XOSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_XOSC32KRDY) >> SYSCTRL_PCLKSR_XOSC32KRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_OSC32KRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY) >> SYSCTRL_PCLKSR_OSC32KRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_OSC8MRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY) >> SYSCTRL_PCLKSR_OSC8MRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DFLLRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) >> SYSCTRL_PCLKSR_DFLLRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DFLLOOB_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLOOB) >> SYSCTRL_PCLKSR_DFLLOOB_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DFLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKF) >> SYSCTRL_PCLKSR_DFLLLCKF_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DFLLLCKC_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKC) >> SYSCTRL_PCLKSR_DFLLLCKC_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DFLLRCS_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRCS) >> SYSCTRL_PCLKSR_DFLLRCS_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_BOD33RDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_BOD33RDY) >> SYSCTRL_PCLKSR_BOD33RDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_BOD33DET_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_BOD33DET) >> SYSCTRL_PCLKSR_BOD33DET_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_B33SRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_B33SRDY) >> SYSCTRL_PCLKSR_B33SRDY_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DPLLLCKR_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DPLLLCKR) >> SYSCTRL_PCLKSR_DPLLLCKR_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DPLLLCKF_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DPLLLCKF) >> SYSCTRL_PCLKSR_DPLLLCKF_Pos;
}

static inline bool hri_sysctrl_get_PCLKSR_DPLLLTO_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->PCLKSR.reg & SYSCTRL_PCLKSR_DPLLLTO) >> SYSCTRL_PCLKSR_DPLLLTO_Pos;
}

static inline hri_sysctrl_pclksr_reg_t hri_sysctrl_get_PCLKSR_reg(const void *const hw, hri_sysctrl_pclksr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sysctrl *)hw)->PCLKSR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sysctrl_pclksr_reg_t hri_sysctrl_read_PCLKSR_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->PCLKSR.reg;
}

static inline bool hri_sysctrl_get_DPLLSTATUS_LOCK_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->DPLLSTATUS.reg & SYSCTRL_DPLLSTATUS_LOCK) >> SYSCTRL_DPLLSTATUS_LOCK_Pos;
}

static inline bool hri_sysctrl_get_DPLLSTATUS_CLKRDY_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->DPLLSTATUS.reg & SYSCTRL_DPLLSTATUS_CLKRDY) >> SYSCTRL_DPLLSTATUS_CLKRDY_Pos;
}

static inline bool hri_sysctrl_get_DPLLSTATUS_ENABLE_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->DPLLSTATUS.reg & SYSCTRL_DPLLSTATUS_ENABLE) >> SYSCTRL_DPLLSTATUS_ENABLE_Pos;
}

static inline bool hri_sysctrl_get_DPLLSTATUS_DIV_bit(const void *const hw)
{
	return (((Sysctrl *)hw)->DPLLSTATUS.reg & SYSCTRL_DPLLSTATUS_DIV) >> SYSCTRL_DPLLSTATUS_DIV_Pos;
}

static inline hri_sysctrl_dpllstatus_reg_t hri_sysctrl_get_DPLLSTATUS_reg(const void *const            hw,
                                                                          hri_sysctrl_dpllstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sysctrl *)hw)->DPLLSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sysctrl_dpllstatus_reg_t hri_sysctrl_read_DPLLSTATUS_reg(const void *const hw)
{
	return ((Sysctrl *)hw)->DPLLSTATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SYSCTRL_D21_H_INCLUDED */
#endif /* _SAMD21_SYSCTRL_COMPONENT_ */
