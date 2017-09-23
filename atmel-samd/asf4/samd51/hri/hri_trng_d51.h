/**
 * \file
 *
 * \brief SAM TRNG
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

#ifdef _SAMD51_TRNG_COMPONENT_
#ifndef _HRI_TRNG_D51_H_INCLUDED_
#define _HRI_TRNG_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_TRNG_CRITICAL_SECTIONS)
#define TRNG_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define TRNG_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define TRNG_CRITICAL_SECTION_ENTER()
#define TRNG_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_trng_data_reg_t;
typedef uint8_t  hri_trng_ctrla_reg_t;
typedef uint8_t  hri_trng_evctrl_reg_t;
typedef uint8_t  hri_trng_intenset_reg_t;
typedef uint8_t  hri_trng_intflag_reg_t;

static inline void hri_trng_set_INTEN_DATARDY_bit(const void *const hw)
{
	((Trng *)hw)->INTENSET.reg = TRNG_INTENSET_DATARDY;
}

static inline bool hri_trng_get_INTEN_DATARDY_bit(const void *const hw)
{
	return (((Trng *)hw)->INTENSET.reg & TRNG_INTENSET_DATARDY) >> TRNG_INTENSET_DATARDY_Pos;
}

static inline void hri_trng_write_INTEN_DATARDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Trng *)hw)->INTENCLR.reg = TRNG_INTENSET_DATARDY;
	} else {
		((Trng *)hw)->INTENSET.reg = TRNG_INTENSET_DATARDY;
	}
}

static inline void hri_trng_clear_INTEN_DATARDY_bit(const void *const hw)
{
	((Trng *)hw)->INTENCLR.reg = TRNG_INTENSET_DATARDY;
}

static inline void hri_trng_set_INTEN_reg(const void *const hw, hri_trng_intenset_reg_t mask)
{
	((Trng *)hw)->INTENSET.reg = mask;
}

static inline hri_trng_intenset_reg_t hri_trng_get_INTEN_reg(const void *const hw, hri_trng_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_trng_intenset_reg_t hri_trng_read_INTEN_reg(const void *const hw)
{
	return ((Trng *)hw)->INTENSET.reg;
}

static inline void hri_trng_write_INTEN_reg(const void *const hw, hri_trng_intenset_reg_t data)
{
	((Trng *)hw)->INTENSET.reg = data;
	((Trng *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_trng_clear_INTEN_reg(const void *const hw, hri_trng_intenset_reg_t mask)
{
	((Trng *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_trng_get_INTFLAG_DATARDY_bit(const void *const hw)
{
	return (((Trng *)hw)->INTFLAG.reg & TRNG_INTFLAG_DATARDY) >> TRNG_INTFLAG_DATARDY_Pos;
}

static inline void hri_trng_clear_INTFLAG_DATARDY_bit(const void *const hw)
{
	((Trng *)hw)->INTFLAG.reg = TRNG_INTFLAG_DATARDY;
}

static inline bool hri_trng_get_interrupt_DATARDY_bit(const void *const hw)
{
	return (((Trng *)hw)->INTFLAG.reg & TRNG_INTFLAG_DATARDY) >> TRNG_INTFLAG_DATARDY_Pos;
}

static inline void hri_trng_clear_interrupt_DATARDY_bit(const void *const hw)
{
	((Trng *)hw)->INTFLAG.reg = TRNG_INTFLAG_DATARDY;
}

static inline hri_trng_intflag_reg_t hri_trng_get_INTFLAG_reg(const void *const hw, hri_trng_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_trng_intflag_reg_t hri_trng_read_INTFLAG_reg(const void *const hw)
{
	return ((Trng *)hw)->INTFLAG.reg;
}

static inline void hri_trng_clear_INTFLAG_reg(const void *const hw, hri_trng_intflag_reg_t mask)
{
	((Trng *)hw)->INTFLAG.reg = mask;
}

static inline void hri_trng_set_CTRLA_ENABLE_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg |= TRNG_CTRLA_ENABLE;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_trng_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->CTRLA.reg;
	tmp = (tmp & TRNG_CTRLA_ENABLE) >> TRNG_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_trng_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TRNG_CRITICAL_SECTION_ENTER();
	tmp = ((Trng *)hw)->CTRLA.reg;
	tmp &= ~TRNG_CTRLA_ENABLE;
	tmp |= value << TRNG_CTRLA_ENABLE_Pos;
	((Trng *)hw)->CTRLA.reg = tmp;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg &= ~TRNG_CTRLA_ENABLE;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg ^= TRNG_CTRLA_ENABLE;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg |= TRNG_CTRLA_RUNSTDBY;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_trng_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->CTRLA.reg;
	tmp = (tmp & TRNG_CTRLA_RUNSTDBY) >> TRNG_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_trng_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TRNG_CRITICAL_SECTION_ENTER();
	tmp = ((Trng *)hw)->CTRLA.reg;
	tmp &= ~TRNG_CTRLA_RUNSTDBY;
	tmp |= value << TRNG_CTRLA_RUNSTDBY_Pos;
	((Trng *)hw)->CTRLA.reg = tmp;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg &= ~TRNG_CTRLA_RUNSTDBY;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg ^= TRNG_CTRLA_RUNSTDBY;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_set_CTRLA_reg(const void *const hw, hri_trng_ctrla_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg |= mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline hri_trng_ctrla_reg_t hri_trng_get_CTRLA_reg(const void *const hw, hri_trng_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_trng_write_CTRLA_reg(const void *const hw, hri_trng_ctrla_reg_t data)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg = data;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_clear_CTRLA_reg(const void *const hw, hri_trng_ctrla_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg &= ~mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_toggle_CTRLA_reg(const void *const hw, hri_trng_ctrla_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->CTRLA.reg ^= mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline hri_trng_ctrla_reg_t hri_trng_read_CTRLA_reg(const void *const hw)
{
	return ((Trng *)hw)->CTRLA.reg;
}

static inline void hri_trng_set_EVCTRL_DATARDYEO_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg |= TRNG_EVCTRL_DATARDYEO;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_trng_get_EVCTRL_DATARDYEO_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->EVCTRL.reg;
	tmp = (tmp & TRNG_EVCTRL_DATARDYEO) >> TRNG_EVCTRL_DATARDYEO_Pos;
	return (bool)tmp;
}

static inline void hri_trng_write_EVCTRL_DATARDYEO_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TRNG_CRITICAL_SECTION_ENTER();
	tmp = ((Trng *)hw)->EVCTRL.reg;
	tmp &= ~TRNG_EVCTRL_DATARDYEO;
	tmp |= value << TRNG_EVCTRL_DATARDYEO_Pos;
	((Trng *)hw)->EVCTRL.reg = tmp;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_clear_EVCTRL_DATARDYEO_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg &= ~TRNG_EVCTRL_DATARDYEO;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_toggle_EVCTRL_DATARDYEO_bit(const void *const hw)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg ^= TRNG_EVCTRL_DATARDYEO;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_set_EVCTRL_reg(const void *const hw, hri_trng_evctrl_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg |= mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline hri_trng_evctrl_reg_t hri_trng_get_EVCTRL_reg(const void *const hw, hri_trng_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Trng *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_trng_write_EVCTRL_reg(const void *const hw, hri_trng_evctrl_reg_t data)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg = data;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_clear_EVCTRL_reg(const void *const hw, hri_trng_evctrl_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg &= ~mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline void hri_trng_toggle_EVCTRL_reg(const void *const hw, hri_trng_evctrl_reg_t mask)
{
	TRNG_CRITICAL_SECTION_ENTER();
	((Trng *)hw)->EVCTRL.reg ^= mask;
	TRNG_CRITICAL_SECTION_LEAVE();
}

static inline hri_trng_evctrl_reg_t hri_trng_read_EVCTRL_reg(const void *const hw)
{
	return ((Trng *)hw)->EVCTRL.reg;
}

static inline hri_trng_data_reg_t hri_trng_get_DATA_DATA_bf(const void *const hw, hri_trng_data_reg_t mask)
{
	return (((Trng *)hw)->DATA.reg & TRNG_DATA_DATA(mask)) >> TRNG_DATA_DATA_Pos;
}

static inline hri_trng_data_reg_t hri_trng_read_DATA_DATA_bf(const void *const hw)
{
	return (((Trng *)hw)->DATA.reg & TRNG_DATA_DATA_Msk) >> TRNG_DATA_DATA_Pos;
}

static inline hri_trng_data_reg_t hri_trng_get_DATA_reg(const void *const hw, hri_trng_data_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Trng *)hw)->DATA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_trng_data_reg_t hri_trng_read_DATA_reg(const void *const hw)
{
	return ((Trng *)hw)->DATA.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_TRNG_D51_H_INCLUDED */
#endif /* _SAMD51_TRNG_COMPONENT_ */
