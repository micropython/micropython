/**
 * \file
 *
 * \brief SAM SUPC
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

#ifdef _SAMD51_SUPC_COMPONENT_
#ifndef _HRI_SUPC_D51_H_INCLUDED_
#define _HRI_SUPC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SUPC_CRITICAL_SECTIONS)
#define SUPC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SUPC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SUPC_CRITICAL_SECTION_ENTER()
#define SUPC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_supc_bbps_reg_t;
typedef uint32_t hri_supc_bkin_reg_t;
typedef uint32_t hri_supc_bkout_reg_t;
typedef uint32_t hri_supc_bod12_reg_t;
typedef uint32_t hri_supc_bod33_reg_t;
typedef uint32_t hri_supc_intenset_reg_t;
typedef uint32_t hri_supc_intflag_reg_t;
typedef uint32_t hri_supc_status_reg_t;
typedef uint32_t hri_supc_vref_reg_t;
typedef uint32_t hri_supc_vreg_reg_t;

static inline void hri_supc_set_INTEN_BOD33RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD33RDY;
}

static inline bool hri_supc_get_INTEN_BOD33RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_BOD33RDY) >> SUPC_INTENSET_BOD33RDY_Pos;
}

static inline void hri_supc_write_INTEN_BOD33RDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD33RDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD33RDY;
	}
}

static inline void hri_supc_clear_INTEN_BOD33RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD33RDY;
}

static inline void hri_supc_set_INTEN_BOD33DET_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD33DET;
}

static inline bool hri_supc_get_INTEN_BOD33DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_BOD33DET) >> SUPC_INTENSET_BOD33DET_Pos;
}

static inline void hri_supc_write_INTEN_BOD33DET_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD33DET;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD33DET;
	}
}

static inline void hri_supc_clear_INTEN_BOD33DET_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD33DET;
}

static inline void hri_supc_set_INTEN_B33SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_B33SRDY;
}

static inline bool hri_supc_get_INTEN_B33SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_B33SRDY) >> SUPC_INTENSET_B33SRDY_Pos;
}

static inline void hri_supc_write_INTEN_B33SRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_B33SRDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_B33SRDY;
	}
}

static inline void hri_supc_clear_INTEN_B33SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_B33SRDY;
}

static inline void hri_supc_set_INTEN_BOD12RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD12RDY;
}

static inline bool hri_supc_get_INTEN_BOD12RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_BOD12RDY) >> SUPC_INTENSET_BOD12RDY_Pos;
}

static inline void hri_supc_write_INTEN_BOD12RDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD12RDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD12RDY;
	}
}

static inline void hri_supc_clear_INTEN_BOD12RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD12RDY;
}

static inline void hri_supc_set_INTEN_BOD12DET_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD12DET;
}

static inline bool hri_supc_get_INTEN_BOD12DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_BOD12DET) >> SUPC_INTENSET_BOD12DET_Pos;
}

static inline void hri_supc_write_INTEN_BOD12DET_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD12DET;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_BOD12DET;
	}
}

static inline void hri_supc_clear_INTEN_BOD12DET_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_BOD12DET;
}

static inline void hri_supc_set_INTEN_B12SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_B12SRDY;
}

static inline bool hri_supc_get_INTEN_B12SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_B12SRDY) >> SUPC_INTENSET_B12SRDY_Pos;
}

static inline void hri_supc_write_INTEN_B12SRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_B12SRDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_B12SRDY;
	}
}

static inline void hri_supc_clear_INTEN_B12SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_B12SRDY;
}

static inline void hri_supc_set_INTEN_VREGRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_VREGRDY;
}

static inline bool hri_supc_get_INTEN_VREGRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_VREGRDY) >> SUPC_INTENSET_VREGRDY_Pos;
}

static inline void hri_supc_write_INTEN_VREGRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_VREGRDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_VREGRDY;
	}
}

static inline void hri_supc_clear_INTEN_VREGRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_VREGRDY;
}

static inline void hri_supc_set_INTEN_VCORERDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_VCORERDY;
}

static inline bool hri_supc_get_INTEN_VCORERDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTENSET.reg & SUPC_INTENSET_VCORERDY) >> SUPC_INTENSET_VCORERDY_Pos;
}

static inline void hri_supc_write_INTEN_VCORERDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_VCORERDY;
	} else {
		((Supc *)hw)->INTENSET.reg = SUPC_INTENSET_VCORERDY;
	}
}

static inline void hri_supc_clear_INTEN_VCORERDY_bit(const void *const hw)
{
	((Supc *)hw)->INTENCLR.reg = SUPC_INTENSET_VCORERDY;
}

static inline void hri_supc_set_INTEN_reg(const void *const hw, hri_supc_intenset_reg_t mask)
{
	((Supc *)hw)->INTENSET.reg = mask;
}

static inline hri_supc_intenset_reg_t hri_supc_get_INTEN_reg(const void *const hw, hri_supc_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_supc_intenset_reg_t hri_supc_read_INTEN_reg(const void *const hw)
{
	return ((Supc *)hw)->INTENSET.reg;
}

static inline void hri_supc_write_INTEN_reg(const void *const hw, hri_supc_intenset_reg_t data)
{
	((Supc *)hw)->INTENSET.reg = data;
	((Supc *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_supc_clear_INTEN_reg(const void *const hw, hri_supc_intenset_reg_t mask)
{
	((Supc *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_supc_get_INTFLAG_BOD33RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD33RDY) >> SUPC_INTFLAG_BOD33RDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_BOD33RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD33RDY;
}

static inline bool hri_supc_get_INTFLAG_BOD33DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD33DET) >> SUPC_INTFLAG_BOD33DET_Pos;
}

static inline void hri_supc_clear_INTFLAG_BOD33DET_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD33DET;
}

static inline bool hri_supc_get_INTFLAG_B33SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_B33SRDY) >> SUPC_INTFLAG_B33SRDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_B33SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_B33SRDY;
}

static inline bool hri_supc_get_INTFLAG_BOD12RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD12RDY) >> SUPC_INTFLAG_BOD12RDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_BOD12RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD12RDY;
}

static inline bool hri_supc_get_INTFLAG_BOD12DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD12DET) >> SUPC_INTFLAG_BOD12DET_Pos;
}

static inline void hri_supc_clear_INTFLAG_BOD12DET_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD12DET;
}

static inline bool hri_supc_get_INTFLAG_B12SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_B12SRDY) >> SUPC_INTFLAG_B12SRDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_B12SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_B12SRDY;
}

static inline bool hri_supc_get_INTFLAG_VREGRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_VREGRDY) >> SUPC_INTFLAG_VREGRDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_VREGRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_VREGRDY;
}

static inline bool hri_supc_get_INTFLAG_VCORERDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_VCORERDY) >> SUPC_INTFLAG_VCORERDY_Pos;
}

static inline void hri_supc_clear_INTFLAG_VCORERDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_VCORERDY;
}

static inline bool hri_supc_get_interrupt_BOD33RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD33RDY) >> SUPC_INTFLAG_BOD33RDY_Pos;
}

static inline void hri_supc_clear_interrupt_BOD33RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD33RDY;
}

static inline bool hri_supc_get_interrupt_BOD33DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD33DET) >> SUPC_INTFLAG_BOD33DET_Pos;
}

static inline void hri_supc_clear_interrupt_BOD33DET_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD33DET;
}

static inline bool hri_supc_get_interrupt_B33SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_B33SRDY) >> SUPC_INTFLAG_B33SRDY_Pos;
}

static inline void hri_supc_clear_interrupt_B33SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_B33SRDY;
}

static inline bool hri_supc_get_interrupt_BOD12RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD12RDY) >> SUPC_INTFLAG_BOD12RDY_Pos;
}

static inline void hri_supc_clear_interrupt_BOD12RDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD12RDY;
}

static inline bool hri_supc_get_interrupt_BOD12DET_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_BOD12DET) >> SUPC_INTFLAG_BOD12DET_Pos;
}

static inline void hri_supc_clear_interrupt_BOD12DET_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_BOD12DET;
}

static inline bool hri_supc_get_interrupt_B12SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_B12SRDY) >> SUPC_INTFLAG_B12SRDY_Pos;
}

static inline void hri_supc_clear_interrupt_B12SRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_B12SRDY;
}

static inline bool hri_supc_get_interrupt_VREGRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_VREGRDY) >> SUPC_INTFLAG_VREGRDY_Pos;
}

static inline void hri_supc_clear_interrupt_VREGRDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_VREGRDY;
}

static inline bool hri_supc_get_interrupt_VCORERDY_bit(const void *const hw)
{
	return (((Supc *)hw)->INTFLAG.reg & SUPC_INTFLAG_VCORERDY) >> SUPC_INTFLAG_VCORERDY_Pos;
}

static inline void hri_supc_clear_interrupt_VCORERDY_bit(const void *const hw)
{
	((Supc *)hw)->INTFLAG.reg = SUPC_INTFLAG_VCORERDY;
}

static inline hri_supc_intflag_reg_t hri_supc_get_INTFLAG_reg(const void *const hw, hri_supc_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_supc_intflag_reg_t hri_supc_read_INTFLAG_reg(const void *const hw)
{
	return ((Supc *)hw)->INTFLAG.reg;
}

static inline void hri_supc_clear_INTFLAG_reg(const void *const hw, hri_supc_intflag_reg_t mask)
{
	((Supc *)hw)->INTFLAG.reg = mask;
}

static inline void hri_supc_set_BOD33_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD33_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_ENABLE) >> SUPC_BOD33_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD33_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_ENABLE;
	tmp |= value << SUPC_BOD33_ENABLE_Pos;
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD33_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD33_STDBYCFG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_STDBYCFG) >> SUPC_BOD33_STDBYCFG_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD33_STDBYCFG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_STDBYCFG;
	tmp |= value << SUPC_BOD33_STDBYCFG_Pos;
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD33_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_RUNSTDBY) >> SUPC_BOD33_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD33_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_RUNSTDBY;
	tmp |= value << SUPC_BOD33_RUNSTDBY_Pos;
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD33_RUNHIB_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_RUNHIB;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD33_RUNHIB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_RUNHIB) >> SUPC_BOD33_RUNHIB_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD33_RUNHIB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_RUNHIB;
	tmp |= value << SUPC_BOD33_RUNHIB_Pos;
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_RUNHIB_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_RUNHIB;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_RUNHIB_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_RUNHIB;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD33_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD33_RUNBKUP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_RUNBKUP) >> SUPC_BOD33_RUNBKUP_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD33_RUNBKUP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_RUNBKUP;
	tmp |= value << SUPC_BOD33_RUNBKUP_Pos;
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD33_ACTION_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_ACTION_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_ACTION(mask)) >> SUPC_BOD33_ACTION_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD33_ACTION_bf(const void *const hw, hri_supc_bod33_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_ACTION_Msk;
	tmp |= SUPC_BOD33_ACTION(data);
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_ACTION_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_ACTION_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_ACTION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_ACTION_Msk) >> SUPC_BOD33_ACTION_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD33_HYST_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_HYST(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_HYST_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_HYST(mask)) >> SUPC_BOD33_HYST_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD33_HYST_bf(const void *const hw, hri_supc_bod33_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_HYST_Msk;
	tmp |= SUPC_BOD33_HYST(data);
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_HYST_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_HYST(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_HYST_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_HYST(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_HYST_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_HYST_Msk) >> SUPC_BOD33_HYST_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD33_PSEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_PSEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_PSEL(mask)) >> SUPC_BOD33_PSEL_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD33_PSEL_bf(const void *const hw, hri_supc_bod33_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_PSEL_Msk;
	tmp |= SUPC_BOD33_PSEL(data);
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_PSEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_PSEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_PSEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_PSEL_Msk) >> SUPC_BOD33_PSEL_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD33_LEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_LEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_LEVEL(mask)) >> SUPC_BOD33_LEVEL_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD33_LEVEL_bf(const void *const hw, hri_supc_bod33_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_LEVEL_Msk;
	tmp |= SUPC_BOD33_LEVEL(data);
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_LEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_LEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_LEVEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_LEVEL_Msk) >> SUPC_BOD33_LEVEL_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD33_VBATLEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= SUPC_BOD33_VBATLEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_VBATLEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_VBATLEVEL(mask)) >> SUPC_BOD33_VBATLEVEL_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD33_VBATLEVEL_bf(const void *const hw, hri_supc_bod33_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= ~SUPC_BOD33_VBATLEVEL_Msk;
	tmp |= SUPC_BOD33_VBATLEVEL(data);
	((Supc *)hw)->BOD33.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_VBATLEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~SUPC_BOD33_VBATLEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_VBATLEVEL_bf(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= SUPC_BOD33_VBATLEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_VBATLEVEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp = (tmp & SUPC_BOD33_VBATLEVEL_Msk) >> SUPC_BOD33_VBATLEVEL_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD33_reg(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_get_BOD33_reg(const void *const hw, hri_supc_bod33_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD33.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_BOD33_reg(const void *const hw, hri_supc_bod33_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD33_reg(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD33_reg(const void *const hw, hri_supc_bod33_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD33.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod33_reg_t hri_supc_read_BOD33_reg(const void *const hw)
{
	return ((Supc *)hw)->BOD33.reg;
}

static inline void hri_supc_set_BOD12_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD12_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_ENABLE) >> SUPC_BOD12_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD12_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_ENABLE;
	tmp |= value << SUPC_BOD12_ENABLE_Pos;
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD12_HYST_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_HYST;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD12_HYST_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_HYST) >> SUPC_BOD12_HYST_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD12_HYST_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_HYST;
	tmp |= value << SUPC_BOD12_HYST_Pos;
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_HYST_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_HYST;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_HYST_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_HYST;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD12_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD12_STDBYCFG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_STDBYCFG) >> SUPC_BOD12_STDBYCFG_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD12_STDBYCFG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_STDBYCFG;
	tmp |= value << SUPC_BOD12_STDBYCFG_Pos;
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_STDBYCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_STDBYCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD12_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD12_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_RUNSTDBY) >> SUPC_BOD12_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD12_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_RUNSTDBY;
	tmp |= value << SUPC_BOD12_RUNSTDBY_Pos;
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD12_ACTCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_ACTCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BOD12_ACTCFG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_ACTCFG) >> SUPC_BOD12_ACTCFG_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BOD12_ACTCFG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_ACTCFG;
	tmp |= value << SUPC_BOD12_ACTCFG_Pos;
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_ACTCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_ACTCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_ACTCFG_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_ACTCFG;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BOD12_ACTION_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_get_BOD12_ACTION_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_ACTION(mask)) >> SUPC_BOD12_ACTION_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD12_ACTION_bf(const void *const hw, hri_supc_bod12_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_ACTION_Msk;
	tmp |= SUPC_BOD12_ACTION(data);
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_ACTION_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_ACTION_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_ACTION(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_read_BOD12_ACTION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_ACTION_Msk) >> SUPC_BOD12_ACTION_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD12_PSEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_get_BOD12_PSEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_PSEL(mask)) >> SUPC_BOD12_PSEL_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD12_PSEL_bf(const void *const hw, hri_supc_bod12_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_PSEL_Msk;
	tmp |= SUPC_BOD12_PSEL(data);
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_PSEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_PSEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_PSEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_read_BOD12_PSEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_PSEL_Msk) >> SUPC_BOD12_PSEL_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD12_LEVEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= SUPC_BOD12_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_get_BOD12_LEVEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_LEVEL(mask)) >> SUPC_BOD12_LEVEL_Pos;
	return tmp;
}

static inline void hri_supc_write_BOD12_LEVEL_bf(const void *const hw, hri_supc_bod12_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= ~SUPC_BOD12_LEVEL_Msk;
	tmp |= SUPC_BOD12_LEVEL(data);
	((Supc *)hw)->BOD12.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_LEVEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~SUPC_BOD12_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_LEVEL_bf(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= SUPC_BOD12_LEVEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_read_BOD12_LEVEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp = (tmp & SUPC_BOD12_LEVEL_Msk) >> SUPC_BOD12_LEVEL_Pos;
	return tmp;
}

static inline void hri_supc_set_BOD12_reg(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_get_BOD12_reg(const void *const hw, hri_supc_bod12_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BOD12.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_BOD12_reg(const void *const hw, hri_supc_bod12_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BOD12_reg(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BOD12_reg(const void *const hw, hri_supc_bod12_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BOD12.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bod12_reg_t hri_supc_read_BOD12_reg(const void *const hw)
{
	return ((Supc *)hw)->BOD12.reg;
}

static inline void hri_supc_set_VREG_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= SUPC_VREG_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREG_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_ENABLE) >> SUPC_VREG_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREG_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= ~SUPC_VREG_ENABLE;
	tmp |= value << SUPC_VREG_ENABLE_Pos;
	((Supc *)hw)->VREG.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~SUPC_VREG_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_ENABLE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= SUPC_VREG_ENABLE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREG_SEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= SUPC_VREG_SEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREG_SEL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_SEL) >> SUPC_VREG_SEL_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREG_SEL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= ~SUPC_VREG_SEL;
	tmp |= value << SUPC_VREG_SEL_Pos;
	((Supc *)hw)->VREG.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_SEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~SUPC_VREG_SEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_SEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= SUPC_VREG_SEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREG_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= SUPC_VREG_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREG_RUNBKUP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_RUNBKUP) >> SUPC_VREG_RUNBKUP_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREG_RUNBKUP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= ~SUPC_VREG_RUNBKUP;
	tmp |= value << SUPC_VREG_RUNBKUP_Pos;
	((Supc *)hw)->VREG.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~SUPC_VREG_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_RUNBKUP_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= SUPC_VREG_RUNBKUP;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREG_VSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= SUPC_VREG_VSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREG_VSEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_VSEN) >> SUPC_VREG_VSEN_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREG_VSEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= ~SUPC_VREG_VSEN;
	tmp |= value << SUPC_VREG_VSEN_Pos;
	((Supc *)hw)->VREG.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_VSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~SUPC_VREG_VSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_VSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= SUPC_VREG_VSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREG_VSPER_bf(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= SUPC_VREG_VSPER(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vreg_reg_t hri_supc_get_VREG_VSPER_bf(const void *const hw, hri_supc_vreg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_VSPER(mask)) >> SUPC_VREG_VSPER_Pos;
	return tmp;
}

static inline void hri_supc_write_VREG_VSPER_bf(const void *const hw, hri_supc_vreg_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= ~SUPC_VREG_VSPER_Msk;
	tmp |= SUPC_VREG_VSPER(data);
	((Supc *)hw)->VREG.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_VSPER_bf(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~SUPC_VREG_VSPER(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_VSPER_bf(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= SUPC_VREG_VSPER(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vreg_reg_t hri_supc_read_VREG_VSPER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp = (tmp & SUPC_VREG_VSPER_Msk) >> SUPC_VREG_VSPER_Pos;
	return tmp;
}

static inline void hri_supc_set_VREG_reg(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vreg_reg_t hri_supc_get_VREG_reg(const void *const hw, hri_supc_vreg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_VREG_reg(const void *const hw, hri_supc_vreg_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREG_reg(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREG_reg(const void *const hw, hri_supc_vreg_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREG.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vreg_reg_t hri_supc_read_VREG_reg(const void *const hw)
{
	return ((Supc *)hw)->VREG.reg;
}

static inline void hri_supc_set_VREF_TSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_TSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREF_TSEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_TSEN) >> SUPC_VREF_TSEN_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREF_TSEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_TSEN;
	tmp |= value << SUPC_VREF_TSEN_Pos;
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_TSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_TSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_TSEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_TSEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREF_VREFOE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_VREFOE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREF_VREFOE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_VREFOE) >> SUPC_VREF_VREFOE_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREF_VREFOE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_VREFOE;
	tmp |= value << SUPC_VREF_VREFOE_Pos;
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_VREFOE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_VREFOE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_VREFOE_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_VREFOE;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREF_TSSEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_TSSEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREF_TSSEL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_TSSEL) >> SUPC_VREF_TSSEL_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREF_TSSEL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_TSSEL;
	tmp |= value << SUPC_VREF_TSSEL_Pos;
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_TSSEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_TSSEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_TSSEL_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_TSSEL;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREF_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREF_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_RUNSTDBY) >> SUPC_VREF_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREF_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_RUNSTDBY;
	tmp |= value << SUPC_VREF_RUNSTDBY_Pos;
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_RUNSTDBY_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_RUNSTDBY;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREF_ONDEMAND_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_ONDEMAND;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_VREF_ONDEMAND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_ONDEMAND) >> SUPC_VREF_ONDEMAND_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_VREF_ONDEMAND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_ONDEMAND;
	tmp |= value << SUPC_VREF_ONDEMAND_Pos;
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_ONDEMAND_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_ONDEMAND;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_ONDEMAND_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_ONDEMAND;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_VREF_SEL_bf(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= SUPC_VREF_SEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vref_reg_t hri_supc_get_VREF_SEL_bf(const void *const hw, hri_supc_vref_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_SEL(mask)) >> SUPC_VREF_SEL_Pos;
	return tmp;
}

static inline void hri_supc_write_VREF_SEL_bf(const void *const hw, hri_supc_vref_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= ~SUPC_VREF_SEL_Msk;
	tmp |= SUPC_VREF_SEL(data);
	((Supc *)hw)->VREF.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_SEL_bf(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~SUPC_VREF_SEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_SEL_bf(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= SUPC_VREF_SEL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vref_reg_t hri_supc_read_VREF_SEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp = (tmp & SUPC_VREF_SEL_Msk) >> SUPC_VREF_SEL_Pos;
	return tmp;
}

static inline void hri_supc_set_VREF_reg(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vref_reg_t hri_supc_get_VREF_reg(const void *const hw, hri_supc_vref_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->VREF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_VREF_reg(const void *const hw, hri_supc_vref_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_VREF_reg(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_VREF_reg(const void *const hw, hri_supc_vref_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->VREF.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_vref_reg_t hri_supc_read_VREF_reg(const void *const hw)
{
	return ((Supc *)hw)->VREF.reg;
}

static inline void hri_supc_set_BBPS_CONF_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg |= SUPC_BBPS_CONF;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BBPS_CONF_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BBPS.reg;
	tmp = (tmp & SUPC_BBPS_CONF) >> SUPC_BBPS_CONF_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BBPS_CONF_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BBPS.reg;
	tmp &= ~SUPC_BBPS_CONF;
	tmp |= value << SUPC_BBPS_CONF_Pos;
	((Supc *)hw)->BBPS.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BBPS_CONF_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg &= ~SUPC_BBPS_CONF;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BBPS_CONF_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg ^= SUPC_BBPS_CONF;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BBPS_WAKEEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg |= SUPC_BBPS_WAKEEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_supc_get_BBPS_WAKEEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BBPS.reg;
	tmp = (tmp & SUPC_BBPS_WAKEEN) >> SUPC_BBPS_WAKEEN_Pos;
	return (bool)tmp;
}

static inline void hri_supc_write_BBPS_WAKEEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BBPS.reg;
	tmp &= ~SUPC_BBPS_WAKEEN;
	tmp |= value << SUPC_BBPS_WAKEEN_Pos;
	((Supc *)hw)->BBPS.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BBPS_WAKEEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg &= ~SUPC_BBPS_WAKEEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BBPS_WAKEEN_bit(const void *const hw)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg ^= SUPC_BBPS_WAKEEN;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_set_BBPS_reg(const void *const hw, hri_supc_bbps_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bbps_reg_t hri_supc_get_BBPS_reg(const void *const hw, hri_supc_bbps_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BBPS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_BBPS_reg(const void *const hw, hri_supc_bbps_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BBPS_reg(const void *const hw, hri_supc_bbps_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BBPS_reg(const void *const hw, hri_supc_bbps_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BBPS.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bbps_reg_t hri_supc_read_BBPS_reg(const void *const hw)
{
	return ((Supc *)hw)->BBPS.reg;
}

static inline void hri_supc_set_BKOUT_EN_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg |= SUPC_BKOUT_EN(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_get_BKOUT_EN_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_EN(mask)) >> SUPC_BKOUT_EN_Pos;
	return tmp;
}

static inline void hri_supc_write_BKOUT_EN_bf(const void *const hw, hri_supc_bkout_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp &= ~SUPC_BKOUT_EN_Msk;
	tmp |= SUPC_BKOUT_EN(data);
	((Supc *)hw)->BKOUT.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BKOUT_EN_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg &= ~SUPC_BKOUT_EN(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BKOUT_EN_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg ^= SUPC_BKOUT_EN(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_read_BKOUT_EN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_EN_Msk) >> SUPC_BKOUT_EN_Pos;
	return tmp;
}

static inline void hri_supc_set_BKOUT_CLR_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg |= SUPC_BKOUT_CLR(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_get_BKOUT_CLR_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_CLR(mask)) >> SUPC_BKOUT_CLR_Pos;
	return tmp;
}

static inline void hri_supc_write_BKOUT_CLR_bf(const void *const hw, hri_supc_bkout_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp &= ~SUPC_BKOUT_CLR_Msk;
	tmp |= SUPC_BKOUT_CLR(data);
	((Supc *)hw)->BKOUT.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BKOUT_CLR_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg &= ~SUPC_BKOUT_CLR(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BKOUT_CLR_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg ^= SUPC_BKOUT_CLR(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_read_BKOUT_CLR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_CLR_Msk) >> SUPC_BKOUT_CLR_Pos;
	return tmp;
}

static inline void hri_supc_set_BKOUT_SET_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg |= SUPC_BKOUT_SET(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_get_BKOUT_SET_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_SET(mask)) >> SUPC_BKOUT_SET_Pos;
	return tmp;
}

static inline void hri_supc_write_BKOUT_SET_bf(const void *const hw, hri_supc_bkout_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp &= ~SUPC_BKOUT_SET_Msk;
	tmp |= SUPC_BKOUT_SET(data);
	((Supc *)hw)->BKOUT.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BKOUT_SET_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg &= ~SUPC_BKOUT_SET(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BKOUT_SET_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg ^= SUPC_BKOUT_SET(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_read_BKOUT_SET_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_SET_Msk) >> SUPC_BKOUT_SET_Pos;
	return tmp;
}

static inline void hri_supc_set_BKOUT_RTCTGL_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg |= SUPC_BKOUT_RTCTGL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_get_BKOUT_RTCTGL_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_RTCTGL(mask)) >> SUPC_BKOUT_RTCTGL_Pos;
	return tmp;
}

static inline void hri_supc_write_BKOUT_RTCTGL_bf(const void *const hw, hri_supc_bkout_reg_t data)
{
	uint32_t tmp;
	SUPC_CRITICAL_SECTION_ENTER();
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp &= ~SUPC_BKOUT_RTCTGL_Msk;
	tmp |= SUPC_BKOUT_RTCTGL(data);
	((Supc *)hw)->BKOUT.reg = tmp;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BKOUT_RTCTGL_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg &= ~SUPC_BKOUT_RTCTGL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BKOUT_RTCTGL_bf(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg ^= SUPC_BKOUT_RTCTGL(mask);
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_read_BKOUT_RTCTGL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp = (tmp & SUPC_BKOUT_RTCTGL_Msk) >> SUPC_BKOUT_RTCTGL_Pos;
	return tmp;
}

static inline void hri_supc_set_BKOUT_reg(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg |= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_get_BKOUT_reg(const void *const hw, hri_supc_bkout_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKOUT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_supc_write_BKOUT_reg(const void *const hw, hri_supc_bkout_reg_t data)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg = data;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_clear_BKOUT_reg(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg &= ~mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_supc_toggle_BKOUT_reg(const void *const hw, hri_supc_bkout_reg_t mask)
{
	SUPC_CRITICAL_SECTION_ENTER();
	((Supc *)hw)->BKOUT.reg ^= mask;
	SUPC_CRITICAL_SECTION_LEAVE();
}

static inline hri_supc_bkout_reg_t hri_supc_read_BKOUT_reg(const void *const hw)
{
	return ((Supc *)hw)->BKOUT.reg;
}

static inline bool hri_supc_get_STATUS_BOD33RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_BOD33RDY) >> SUPC_STATUS_BOD33RDY_Pos;
}

static inline bool hri_supc_get_STATUS_BOD33DET_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_BOD33DET) >> SUPC_STATUS_BOD33DET_Pos;
}

static inline bool hri_supc_get_STATUS_B33SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_B33SRDY) >> SUPC_STATUS_B33SRDY_Pos;
}

static inline bool hri_supc_get_STATUS_BOD12RDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_BOD12RDY) >> SUPC_STATUS_BOD12RDY_Pos;
}

static inline bool hri_supc_get_STATUS_BOD12DET_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_BOD12DET) >> SUPC_STATUS_BOD12DET_Pos;
}

static inline bool hri_supc_get_STATUS_B12SRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_B12SRDY) >> SUPC_STATUS_B12SRDY_Pos;
}

static inline bool hri_supc_get_STATUS_VREGRDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_VREGRDY) >> SUPC_STATUS_VREGRDY_Pos;
}

static inline bool hri_supc_get_STATUS_VCORERDY_bit(const void *const hw)
{
	return (((Supc *)hw)->STATUS.reg & SUPC_STATUS_VCORERDY) >> SUPC_STATUS_VCORERDY_Pos;
}

static inline hri_supc_status_reg_t hri_supc_get_STATUS_reg(const void *const hw, hri_supc_status_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_supc_status_reg_t hri_supc_read_STATUS_reg(const void *const hw)
{
	return ((Supc *)hw)->STATUS.reg;
}

static inline hri_supc_bkin_reg_t hri_supc_get_BKIN_BKIN_bf(const void *const hw, hri_supc_bkin_reg_t mask)
{
	return (((Supc *)hw)->BKIN.reg & SUPC_BKIN_BKIN(mask)) >> SUPC_BKIN_BKIN_Pos;
}

static inline hri_supc_bkin_reg_t hri_supc_read_BKIN_BKIN_bf(const void *const hw)
{
	return (((Supc *)hw)->BKIN.reg & SUPC_BKIN_BKIN_Msk) >> SUPC_BKIN_BKIN_Pos;
}

static inline hri_supc_bkin_reg_t hri_supc_get_BKIN_reg(const void *const hw, hri_supc_bkin_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Supc *)hw)->BKIN.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_supc_bkin_reg_t hri_supc_read_BKIN_reg(const void *const hw)
{
	return ((Supc *)hw)->BKIN.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SUPC_D51_H_INCLUDED */
#endif /* _SAMD51_SUPC_COMPONENT_ */
