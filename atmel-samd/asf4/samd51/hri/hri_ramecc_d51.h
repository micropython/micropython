/**
 * \file
 *
 * \brief SAM RAMECC
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

#ifdef _SAMD51_RAMECC_COMPONENT_
#ifndef _HRI_RAMECC_D51_H_INCLUDED_
#define _HRI_RAMECC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_RAMECC_CRITICAL_SECTIONS)
#define RAMECC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define RAMECC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define RAMECC_CRITICAL_SECTION_ENTER()
#define RAMECC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_ramecc_erraddr_reg_t;
typedef uint8_t  hri_ramecc_dbgctrl_reg_t;
typedef uint8_t  hri_ramecc_intenset_reg_t;
typedef uint8_t  hri_ramecc_intflag_reg_t;
typedef uint8_t  hri_ramecc_status_reg_t;

static inline void hri_ramecc_set_INTEN_SINGLEE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTENSET.reg = RAMECC_INTENSET_SINGLEE;
}

static inline bool hri_ramecc_get_INTEN_SINGLEE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTENSET.reg & RAMECC_INTENSET_SINGLEE) >> RAMECC_INTENSET_SINGLEE_Pos;
}

static inline void hri_ramecc_write_INTEN_SINGLEE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Ramecc *)hw)->INTENCLR.reg = RAMECC_INTENSET_SINGLEE;
	} else {
		((Ramecc *)hw)->INTENSET.reg = RAMECC_INTENSET_SINGLEE;
	}
}

static inline void hri_ramecc_clear_INTEN_SINGLEE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTENCLR.reg = RAMECC_INTENSET_SINGLEE;
}

static inline void hri_ramecc_set_INTEN_DUALE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTENSET.reg = RAMECC_INTENSET_DUALE;
}

static inline bool hri_ramecc_get_INTEN_DUALE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTENSET.reg & RAMECC_INTENSET_DUALE) >> RAMECC_INTENSET_DUALE_Pos;
}

static inline void hri_ramecc_write_INTEN_DUALE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Ramecc *)hw)->INTENCLR.reg = RAMECC_INTENSET_DUALE;
	} else {
		((Ramecc *)hw)->INTENSET.reg = RAMECC_INTENSET_DUALE;
	}
}

static inline void hri_ramecc_clear_INTEN_DUALE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTENCLR.reg = RAMECC_INTENSET_DUALE;
}

static inline void hri_ramecc_set_INTEN_reg(const void *const hw, hri_ramecc_intenset_reg_t mask)
{
	((Ramecc *)hw)->INTENSET.reg = mask;
}

static inline hri_ramecc_intenset_reg_t hri_ramecc_get_INTEN_reg(const void *const hw, hri_ramecc_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ramecc_intenset_reg_t hri_ramecc_read_INTEN_reg(const void *const hw)
{
	return ((Ramecc *)hw)->INTENSET.reg;
}

static inline void hri_ramecc_write_INTEN_reg(const void *const hw, hri_ramecc_intenset_reg_t data)
{
	((Ramecc *)hw)->INTENSET.reg = data;
	((Ramecc *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_ramecc_clear_INTEN_reg(const void *const hw, hri_ramecc_intenset_reg_t mask)
{
	((Ramecc *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_ramecc_get_INTFLAG_SINGLEE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTFLAG.reg & RAMECC_INTFLAG_SINGLEE) >> RAMECC_INTFLAG_SINGLEE_Pos;
}

static inline void hri_ramecc_clear_INTFLAG_SINGLEE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTFLAG.reg = RAMECC_INTFLAG_SINGLEE;
}

static inline bool hri_ramecc_get_INTFLAG_DUALE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTFLAG.reg & RAMECC_INTFLAG_DUALE) >> RAMECC_INTFLAG_DUALE_Pos;
}

static inline void hri_ramecc_clear_INTFLAG_DUALE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTFLAG.reg = RAMECC_INTFLAG_DUALE;
}

static inline bool hri_ramecc_get_interrupt_SINGLEE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTFLAG.reg & RAMECC_INTFLAG_SINGLEE) >> RAMECC_INTFLAG_SINGLEE_Pos;
}

static inline void hri_ramecc_clear_interrupt_SINGLEE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTFLAG.reg = RAMECC_INTFLAG_SINGLEE;
}

static inline bool hri_ramecc_get_interrupt_DUALE_bit(const void *const hw)
{
	return (((Ramecc *)hw)->INTFLAG.reg & RAMECC_INTFLAG_DUALE) >> RAMECC_INTFLAG_DUALE_Pos;
}

static inline void hri_ramecc_clear_interrupt_DUALE_bit(const void *const hw)
{
	((Ramecc *)hw)->INTFLAG.reg = RAMECC_INTFLAG_DUALE;
}

static inline hri_ramecc_intflag_reg_t hri_ramecc_get_INTFLAG_reg(const void *const hw, hri_ramecc_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ramecc_intflag_reg_t hri_ramecc_read_INTFLAG_reg(const void *const hw)
{
	return ((Ramecc *)hw)->INTFLAG.reg;
}

static inline void hri_ramecc_clear_INTFLAG_reg(const void *const hw, hri_ramecc_intflag_reg_t mask)
{
	((Ramecc *)hw)->INTFLAG.reg = mask;
}

static inline void hri_ramecc_set_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg |= RAMECC_DBGCTRL_ECCDIS;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ramecc_get_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->DBGCTRL.reg;
	tmp = (tmp & RAMECC_DBGCTRL_ECCDIS) >> RAMECC_DBGCTRL_ECCDIS_Pos;
	return (bool)tmp;
}

static inline void hri_ramecc_write_DBGCTRL_ECCDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RAMECC_CRITICAL_SECTION_ENTER();
	tmp = ((Ramecc *)hw)->DBGCTRL.reg;
	tmp &= ~RAMECC_DBGCTRL_ECCDIS;
	tmp |= value << RAMECC_DBGCTRL_ECCDIS_Pos;
	((Ramecc *)hw)->DBGCTRL.reg = tmp;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_clear_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg &= ~RAMECC_DBGCTRL_ECCDIS;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_toggle_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg ^= RAMECC_DBGCTRL_ECCDIS;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_set_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg |= RAMECC_DBGCTRL_ECCELOG;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_ramecc_get_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->DBGCTRL.reg;
	tmp = (tmp & RAMECC_DBGCTRL_ECCELOG) >> RAMECC_DBGCTRL_ECCELOG_Pos;
	return (bool)tmp;
}

static inline void hri_ramecc_write_DBGCTRL_ECCELOG_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RAMECC_CRITICAL_SECTION_ENTER();
	tmp = ((Ramecc *)hw)->DBGCTRL.reg;
	tmp &= ~RAMECC_DBGCTRL_ECCELOG;
	tmp |= value << RAMECC_DBGCTRL_ECCELOG_Pos;
	((Ramecc *)hw)->DBGCTRL.reg = tmp;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_clear_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg &= ~RAMECC_DBGCTRL_ECCELOG;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_toggle_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg ^= RAMECC_DBGCTRL_ECCELOG;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_set_DBGCTRL_reg(const void *const hw, hri_ramecc_dbgctrl_reg_t mask)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg |= mask;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ramecc_dbgctrl_reg_t hri_ramecc_get_DBGCTRL_reg(const void *const hw, hri_ramecc_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_ramecc_write_DBGCTRL_reg(const void *const hw, hri_ramecc_dbgctrl_reg_t data)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg = data;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_clear_DBGCTRL_reg(const void *const hw, hri_ramecc_dbgctrl_reg_t mask)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg &= ~mask;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_ramecc_toggle_DBGCTRL_reg(const void *const hw, hri_ramecc_dbgctrl_reg_t mask)
{
	RAMECC_CRITICAL_SECTION_ENTER();
	((Ramecc *)hw)->DBGCTRL.reg ^= mask;
	RAMECC_CRITICAL_SECTION_LEAVE();
}

static inline hri_ramecc_dbgctrl_reg_t hri_ramecc_read_DBGCTRL_reg(const void *const hw)
{
	return ((Ramecc *)hw)->DBGCTRL.reg;
}

static inline bool hri_ramecc_get_STATUS_ECCDIS_bit(const void *const hw)
{
	return (((Ramecc *)hw)->STATUS.reg & RAMECC_STATUS_ECCDIS) >> RAMECC_STATUS_ECCDIS_Pos;
}

static inline hri_ramecc_status_reg_t hri_ramecc_get_STATUS_reg(const void *const hw, hri_ramecc_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Ramecc *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ramecc_status_reg_t hri_ramecc_read_STATUS_reg(const void *const hw)
{
	return ((Ramecc *)hw)->STATUS.reg;
}

static inline hri_ramecc_erraddr_reg_t hri_ramecc_get_ERRADDR_ERRADDR_bf(const void *const        hw,
                                                                         hri_ramecc_erraddr_reg_t mask)
{
	return (((Ramecc *)hw)->ERRADDR.reg & RAMECC_ERRADDR_ERRADDR(mask)) >> RAMECC_ERRADDR_ERRADDR_Pos;
}

static inline hri_ramecc_erraddr_reg_t hri_ramecc_read_ERRADDR_ERRADDR_bf(const void *const hw)
{
	return (((Ramecc *)hw)->ERRADDR.reg & RAMECC_ERRADDR_ERRADDR_Msk) >> RAMECC_ERRADDR_ERRADDR_Pos;
}

static inline hri_ramecc_erraddr_reg_t hri_ramecc_get_ERRADDR_reg(const void *const hw, hri_ramecc_erraddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Ramecc *)hw)->ERRADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_ramecc_erraddr_reg_t hri_ramecc_read_ERRADDR_reg(const void *const hw)
{
	return ((Ramecc *)hw)->ERRADDR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_RAMECC_D51_H_INCLUDED */
#endif /* _SAMD51_RAMECC_COMPONENT_ */
