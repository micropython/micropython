/**
 * \file
 *
 * \brief SAM EIC
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

#ifdef _SAMD21_EIC_COMPONENT_
#ifndef _HRI_EIC_D21_H_INCLUDED_
#define _HRI_EIC_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_EIC_CRITICAL_SECTIONS)
#define EIC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define EIC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define EIC_CRITICAL_SECTION_ENTER()
#define EIC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_eic_config_reg_t;
typedef uint32_t hri_eic_evctrl_reg_t;
typedef uint32_t hri_eic_intenset_reg_t;
typedef uint32_t hri_eic_intflag_reg_t;
typedef uint32_t hri_eic_wakeup_reg_t;
typedef uint8_t  hri_eic_ctrl_reg_t;
typedef uint8_t  hri_eic_nmictrl_reg_t;
typedef uint8_t  hri_eic_nmiflag_reg_t;
typedef uint8_t  hri_eic_status_reg_t;

static inline void hri_eic_wait_for_sync(const void *const hw)
{
	while (((const Eic *)hw)->STATUS.bit.SYNCBUSY)
		;
}

static inline bool hri_eic_is_syncing(const void *const hw)
{
	return ((const Eic *)hw)->STATUS.bit.SYNCBUSY;
}

static inline void hri_eic_set_INTEN_EXTINT0_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT0;
}

static inline bool hri_eic_get_INTEN_EXTINT0_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT0) >> EIC_INTENSET_EXTINT0_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT0;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT0;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT0_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT0;
}

static inline void hri_eic_set_INTEN_EXTINT1_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT1;
}

static inline bool hri_eic_get_INTEN_EXTINT1_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT1) >> EIC_INTENSET_EXTINT1_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT1;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT1;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT1_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT1;
}

static inline void hri_eic_set_INTEN_EXTINT2_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT2;
}

static inline bool hri_eic_get_INTEN_EXTINT2_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT2) >> EIC_INTENSET_EXTINT2_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT2_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT2;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT2;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT2_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT2;
}

static inline void hri_eic_set_INTEN_EXTINT3_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT3;
}

static inline bool hri_eic_get_INTEN_EXTINT3_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT3) >> EIC_INTENSET_EXTINT3_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT3_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT3;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT3;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT3_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT3;
}

static inline void hri_eic_set_INTEN_EXTINT4_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT4;
}

static inline bool hri_eic_get_INTEN_EXTINT4_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT4) >> EIC_INTENSET_EXTINT4_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT4_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT4;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT4;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT4_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT4;
}

static inline void hri_eic_set_INTEN_EXTINT5_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT5;
}

static inline bool hri_eic_get_INTEN_EXTINT5_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT5) >> EIC_INTENSET_EXTINT5_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT5_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT5;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT5;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT5_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT5;
}

static inline void hri_eic_set_INTEN_EXTINT6_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT6;
}

static inline bool hri_eic_get_INTEN_EXTINT6_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT6) >> EIC_INTENSET_EXTINT6_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT6_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT6;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT6;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT6_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT6;
}

static inline void hri_eic_set_INTEN_EXTINT7_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT7;
}

static inline bool hri_eic_get_INTEN_EXTINT7_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT7) >> EIC_INTENSET_EXTINT7_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT7_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT7;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT7;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT7_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT7;
}

static inline void hri_eic_set_INTEN_EXTINT8_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT8;
}

static inline bool hri_eic_get_INTEN_EXTINT8_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT8) >> EIC_INTENSET_EXTINT8_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT8_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT8;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT8;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT8_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT8;
}

static inline void hri_eic_set_INTEN_EXTINT9_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT9;
}

static inline bool hri_eic_get_INTEN_EXTINT9_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT9) >> EIC_INTENSET_EXTINT9_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT9_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT9;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT9;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT9_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT9;
}

static inline void hri_eic_set_INTEN_EXTINT10_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT10;
}

static inline bool hri_eic_get_INTEN_EXTINT10_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT10) >> EIC_INTENSET_EXTINT10_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT10_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT10;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT10;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT10_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT10;
}

static inline void hri_eic_set_INTEN_EXTINT11_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT11;
}

static inline bool hri_eic_get_INTEN_EXTINT11_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT11) >> EIC_INTENSET_EXTINT11_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT11_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT11;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT11;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT11_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT11;
}

static inline void hri_eic_set_INTEN_EXTINT12_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT12;
}

static inline bool hri_eic_get_INTEN_EXTINT12_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT12) >> EIC_INTENSET_EXTINT12_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT12_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT12;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT12;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT12_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT12;
}

static inline void hri_eic_set_INTEN_EXTINT13_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT13;
}

static inline bool hri_eic_get_INTEN_EXTINT13_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT13) >> EIC_INTENSET_EXTINT13_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT13_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT13;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT13;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT13_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT13;
}

static inline void hri_eic_set_INTEN_EXTINT14_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT14;
}

static inline bool hri_eic_get_INTEN_EXTINT14_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT14) >> EIC_INTENSET_EXTINT14_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT14_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT14;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT14;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT14_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT14;
}

static inline void hri_eic_set_INTEN_EXTINT15_bit(const void *const hw)
{
	((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT15;
}

static inline bool hri_eic_get_INTEN_EXTINT15_bit(const void *const hw)
{
	return (((Eic *)hw)->INTENSET.reg & EIC_INTENSET_EXTINT15) >> EIC_INTENSET_EXTINT15_Pos;
}

static inline void hri_eic_write_INTEN_EXTINT15_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT15;
	} else {
		((Eic *)hw)->INTENSET.reg = EIC_INTENSET_EXTINT15;
	}
}

static inline void hri_eic_clear_INTEN_EXTINT15_bit(const void *const hw)
{
	((Eic *)hw)->INTENCLR.reg = EIC_INTENSET_EXTINT15;
}

static inline void hri_eic_set_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENSET.reg = mask;
}

static inline hri_eic_intenset_reg_t hri_eic_get_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_intenset_reg_t hri_eic_read_INTEN_reg(const void *const hw)
{
	return ((Eic *)hw)->INTENSET.reg;
}

static inline void hri_eic_write_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t data)
{
	((Eic *)hw)->INTENSET.reg = data;
	((Eic *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_eic_clear_INTEN_reg(const void *const hw, hri_eic_intenset_reg_t mask)
{
	((Eic *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_eic_get_NMIFLAG_NMI_bit(const void *const hw)
{
	return (((Eic *)hw)->NMIFLAG.reg & EIC_NMIFLAG_NMI) >> EIC_NMIFLAG_NMI_Pos;
}

static inline void hri_eic_clear_NMIFLAG_NMI_bit(const void *const hw)
{
	((Eic *)hw)->NMIFLAG.reg = EIC_NMIFLAG_NMI;
}

static inline hri_eic_nmiflag_reg_t hri_eic_get_NMIFLAG_reg(const void *const hw, hri_eic_nmiflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMIFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_nmiflag_reg_t hri_eic_read_NMIFLAG_reg(const void *const hw)
{
	return ((Eic *)hw)->NMIFLAG.reg;
}

static inline void hri_eic_clear_NMIFLAG_reg(const void *const hw, hri_eic_nmiflag_reg_t mask)
{
	((Eic *)hw)->NMIFLAG.reg = mask;
}

static inline bool hri_eic_get_INTFLAG_EXTINT0_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT0) >> EIC_INTFLAG_EXTINT0_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT0_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT0;
}

static inline bool hri_eic_get_INTFLAG_EXTINT1_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT1) >> EIC_INTFLAG_EXTINT1_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT1_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT1;
}

static inline bool hri_eic_get_INTFLAG_EXTINT2_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT2) >> EIC_INTFLAG_EXTINT2_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT2_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT2;
}

static inline bool hri_eic_get_INTFLAG_EXTINT3_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT3) >> EIC_INTFLAG_EXTINT3_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT3_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT3;
}

static inline bool hri_eic_get_INTFLAG_EXTINT4_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT4) >> EIC_INTFLAG_EXTINT4_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT4_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT4;
}

static inline bool hri_eic_get_INTFLAG_EXTINT5_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT5) >> EIC_INTFLAG_EXTINT5_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT5_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT5;
}

static inline bool hri_eic_get_INTFLAG_EXTINT6_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT6) >> EIC_INTFLAG_EXTINT6_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT6_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT6;
}

static inline bool hri_eic_get_INTFLAG_EXTINT7_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT7) >> EIC_INTFLAG_EXTINT7_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT7_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT7;
}

static inline bool hri_eic_get_INTFLAG_EXTINT8_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT8) >> EIC_INTFLAG_EXTINT8_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT8_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT8;
}

static inline bool hri_eic_get_INTFLAG_EXTINT9_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT9) >> EIC_INTFLAG_EXTINT9_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT9_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT9;
}

static inline bool hri_eic_get_INTFLAG_EXTINT10_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT10) >> EIC_INTFLAG_EXTINT10_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT10_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT10;
}

static inline bool hri_eic_get_INTFLAG_EXTINT11_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT11) >> EIC_INTFLAG_EXTINT11_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT11_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT11;
}

static inline bool hri_eic_get_INTFLAG_EXTINT12_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT12) >> EIC_INTFLAG_EXTINT12_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT12_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT12;
}

static inline bool hri_eic_get_INTFLAG_EXTINT13_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT13) >> EIC_INTFLAG_EXTINT13_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT13_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT13;
}

static inline bool hri_eic_get_INTFLAG_EXTINT14_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT14) >> EIC_INTFLAG_EXTINT14_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT14_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT14;
}

static inline bool hri_eic_get_INTFLAG_EXTINT15_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT15) >> EIC_INTFLAG_EXTINT15_Pos;
}

static inline void hri_eic_clear_INTFLAG_EXTINT15_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT15;
}

static inline bool hri_eic_get_interrupt_EXTINT0_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT0) >> EIC_INTFLAG_EXTINT0_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT0_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT0;
}

static inline bool hri_eic_get_interrupt_EXTINT1_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT1) >> EIC_INTFLAG_EXTINT1_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT1_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT1;
}

static inline bool hri_eic_get_interrupt_EXTINT2_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT2) >> EIC_INTFLAG_EXTINT2_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT2_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT2;
}

static inline bool hri_eic_get_interrupt_EXTINT3_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT3) >> EIC_INTFLAG_EXTINT3_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT3_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT3;
}

static inline bool hri_eic_get_interrupt_EXTINT4_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT4) >> EIC_INTFLAG_EXTINT4_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT4_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT4;
}

static inline bool hri_eic_get_interrupt_EXTINT5_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT5) >> EIC_INTFLAG_EXTINT5_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT5_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT5;
}

static inline bool hri_eic_get_interrupt_EXTINT6_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT6) >> EIC_INTFLAG_EXTINT6_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT6_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT6;
}

static inline bool hri_eic_get_interrupt_EXTINT7_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT7) >> EIC_INTFLAG_EXTINT7_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT7_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT7;
}

static inline bool hri_eic_get_interrupt_EXTINT8_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT8) >> EIC_INTFLAG_EXTINT8_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT8_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT8;
}

static inline bool hri_eic_get_interrupt_EXTINT9_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT9) >> EIC_INTFLAG_EXTINT9_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT9_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT9;
}

static inline bool hri_eic_get_interrupt_EXTINT10_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT10) >> EIC_INTFLAG_EXTINT10_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT10_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT10;
}

static inline bool hri_eic_get_interrupt_EXTINT11_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT11) >> EIC_INTFLAG_EXTINT11_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT11_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT11;
}

static inline bool hri_eic_get_interrupt_EXTINT12_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT12) >> EIC_INTFLAG_EXTINT12_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT12_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT12;
}

static inline bool hri_eic_get_interrupt_EXTINT13_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT13) >> EIC_INTFLAG_EXTINT13_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT13_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT13;
}

static inline bool hri_eic_get_interrupt_EXTINT14_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT14) >> EIC_INTFLAG_EXTINT14_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT14_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT14;
}

static inline bool hri_eic_get_interrupt_EXTINT15_bit(const void *const hw)
{
	return (((Eic *)hw)->INTFLAG.reg & EIC_INTFLAG_EXTINT15) >> EIC_INTFLAG_EXTINT15_Pos;
}

static inline void hri_eic_clear_interrupt_EXTINT15_bit(const void *const hw)
{
	((Eic *)hw)->INTFLAG.reg = EIC_INTFLAG_EXTINT15;
}

static inline hri_eic_intflag_reg_t hri_eic_get_INTFLAG_reg(const void *const hw, hri_eic_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_intflag_reg_t hri_eic_read_INTFLAG_reg(const void *const hw)
{
	return ((Eic *)hw)->INTFLAG.reg;
}

static inline void hri_eic_clear_INTFLAG_reg(const void *const hw, hri_eic_intflag_reg_t mask)
{
	((Eic *)hw)->INTFLAG.reg = mask;
}

static inline void hri_eic_set_CTRL_SWRST_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw);
	((Eic *)hw)->CTRL.reg |= EIC_CTRL_SWRST;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CTRL_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_eic_wait_for_sync(hw);
	tmp = ((Eic *)hw)->CTRL.reg;
	tmp = (tmp & EIC_CTRL_SWRST) >> EIC_CTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_eic_set_CTRL_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw);
	((Eic *)hw)->CTRL.reg |= EIC_CTRL_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_eic_wait_for_sync(hw);
	tmp = ((Eic *)hw)->CTRL.reg;
	tmp = (tmp & EIC_CTRL_ENABLE) >> EIC_CTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw);
	tmp = ((Eic *)hw)->CTRL.reg;
	tmp &= ~EIC_CTRL_ENABLE;
	tmp |= value << EIC_CTRL_ENABLE_Pos;
	((Eic *)hw)->CTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CTRL_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw);
	((Eic *)hw)->CTRL.reg &= ~EIC_CTRL_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	hri_eic_wait_for_sync(hw);
	((Eic *)hw)->CTRL.reg ^= EIC_CTRL_ENABLE;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CTRL_reg(const void *const hw, hri_eic_ctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRL.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_ctrl_reg_t hri_eic_get_CTRL_reg(const void *const hw, hri_eic_ctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_CTRL_reg(const void *const hw, hri_eic_ctrl_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRL.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CTRL_reg(const void *const hw, hri_eic_ctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRL.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CTRL_reg(const void *const hw, hri_eic_ctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CTRL.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_ctrl_reg_t hri_eic_read_CTRL_reg(const void *const hw)
{
	return ((Eic *)hw)->CTRL.reg;
}

static inline void hri_eic_set_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMIFILTEN) >> EIC_NMICTRL_NMIFILTEN_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_NMICTRL_NMIFILTEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= ~EIC_NMICTRL_NMIFILTEN;
	tmp |= value << EIC_NMICTRL_NMIFILTEN_Pos;
	((Eic *)hw)->NMICTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_NMIFILTEN_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= EIC_NMICTRL_NMIFILTEN;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_get_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMISENSE(mask)) >> EIC_NMICTRL_NMISENSE_Pos;
	return tmp;
}

static inline void hri_eic_write_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t data)
{
	uint8_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= ~EIC_NMICTRL_NMISENSE_Msk;
	tmp |= EIC_NMICTRL_NMISENSE(data);
	((Eic *)hw)->NMICTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_NMISENSE_bf(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= EIC_NMICTRL_NMISENSE(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_read_NMICTRL_NMISENSE_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp = (tmp & EIC_NMICTRL_NMISENSE_Msk) >> EIC_NMICTRL_NMISENSE_Pos;
	return tmp;
}

static inline void hri_eic_set_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_get_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->NMICTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_NMICTRL_reg(const void *const hw, hri_eic_nmictrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->NMICTRL.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_nmictrl_reg_t hri_eic_read_NMICTRL_reg(const void *const hw)
{
	return ((Eic *)hw)->NMICTRL.reg;
}

static inline void hri_eic_set_EVCTRL_EXTINTEO0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO0) >> EIC_EVCTRL_EXTINTEO0_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO0;
	tmp |= value << EIC_EVCTRL_EXTINTEO0_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO1) >> EIC_EVCTRL_EXTINTEO1_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO1;
	tmp |= value << EIC_EVCTRL_EXTINTEO1_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO2) >> EIC_EVCTRL_EXTINTEO2_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO2;
	tmp |= value << EIC_EVCTRL_EXTINTEO2_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO3) >> EIC_EVCTRL_EXTINTEO3_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO3;
	tmp |= value << EIC_EVCTRL_EXTINTEO3_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO4) >> EIC_EVCTRL_EXTINTEO4_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO4;
	tmp |= value << EIC_EVCTRL_EXTINTEO4_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO5) >> EIC_EVCTRL_EXTINTEO5_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO5;
	tmp |= value << EIC_EVCTRL_EXTINTEO5_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO6) >> EIC_EVCTRL_EXTINTEO6_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO6;
	tmp |= value << EIC_EVCTRL_EXTINTEO6_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO7) >> EIC_EVCTRL_EXTINTEO7_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO7;
	tmp |= value << EIC_EVCTRL_EXTINTEO7_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO8) >> EIC_EVCTRL_EXTINTEO8_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO8;
	tmp |= value << EIC_EVCTRL_EXTINTEO8_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO9) >> EIC_EVCTRL_EXTINTEO9_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO9;
	tmp |= value << EIC_EVCTRL_EXTINTEO9_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO10) >> EIC_EVCTRL_EXTINTEO10_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO10;
	tmp |= value << EIC_EVCTRL_EXTINTEO10_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO11) >> EIC_EVCTRL_EXTINTEO11_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO11;
	tmp |= value << EIC_EVCTRL_EXTINTEO11_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO12) >> EIC_EVCTRL_EXTINTEO12_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO12;
	tmp |= value << EIC_EVCTRL_EXTINTEO12_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO13) >> EIC_EVCTRL_EXTINTEO13_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO13;
	tmp |= value << EIC_EVCTRL_EXTINTEO13_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO14) >> EIC_EVCTRL_EXTINTEO14_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO14;
	tmp |= value << EIC_EVCTRL_EXTINTEO14_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_EXTINTEO15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_EVCTRL_EXTINTEO15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp = (tmp & EIC_EVCTRL_EXTINTEO15) >> EIC_EVCTRL_EXTINTEO15_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_EVCTRL_EXTINTEO15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= ~EIC_EVCTRL_EXTINTEO15;
	tmp |= value << EIC_EVCTRL_EXTINTEO15_Pos;
	((Eic *)hw)->EVCTRL.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_EXTINTEO15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~EIC_EVCTRL_EXTINTEO15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_EXTINTEO15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= EIC_EVCTRL_EXTINTEO15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_get_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_EVCTRL_reg(const void *const hw, hri_eic_evctrl_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->EVCTRL.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_evctrl_reg_t hri_eic_read_EVCTRL_reg(const void *const hw)
{
	return ((Eic *)hw)->EVCTRL.reg;
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN0) >> EIC_WAKEUP_WAKEUPEN0_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN0;
	tmp |= value << EIC_WAKEUP_WAKEUPEN0_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN0_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN1) >> EIC_WAKEUP_WAKEUPEN1_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN1;
	tmp |= value << EIC_WAKEUP_WAKEUPEN1_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN1_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN2) >> EIC_WAKEUP_WAKEUPEN2_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN2;
	tmp |= value << EIC_WAKEUP_WAKEUPEN2_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN2_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN3) >> EIC_WAKEUP_WAKEUPEN3_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN3;
	tmp |= value << EIC_WAKEUP_WAKEUPEN3_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN3_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN4) >> EIC_WAKEUP_WAKEUPEN4_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN4;
	tmp |= value << EIC_WAKEUP_WAKEUPEN4_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN4_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN5) >> EIC_WAKEUP_WAKEUPEN5_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN5;
	tmp |= value << EIC_WAKEUP_WAKEUPEN5_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN5_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN6) >> EIC_WAKEUP_WAKEUPEN6_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN6;
	tmp |= value << EIC_WAKEUP_WAKEUPEN6_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN6_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN7) >> EIC_WAKEUP_WAKEUPEN7_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN7;
	tmp |= value << EIC_WAKEUP_WAKEUPEN7_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN7_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN8) >> EIC_WAKEUP_WAKEUPEN8_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN8;
	tmp |= value << EIC_WAKEUP_WAKEUPEN8_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN8_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN8;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN9) >> EIC_WAKEUP_WAKEUPEN9_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN9;
	tmp |= value << EIC_WAKEUP_WAKEUPEN9_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN9_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN9;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN10) >> EIC_WAKEUP_WAKEUPEN10_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN10;
	tmp |= value << EIC_WAKEUP_WAKEUPEN10_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN10_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN10;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN11) >> EIC_WAKEUP_WAKEUPEN11_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN11;
	tmp |= value << EIC_WAKEUP_WAKEUPEN11_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN11_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN11;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN12) >> EIC_WAKEUP_WAKEUPEN12_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN12;
	tmp |= value << EIC_WAKEUP_WAKEUPEN12_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN12_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN12;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN13) >> EIC_WAKEUP_WAKEUPEN13_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN13;
	tmp |= value << EIC_WAKEUP_WAKEUPEN13_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN13_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN13;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN14) >> EIC_WAKEUP_WAKEUPEN14_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN14;
	tmp |= value << EIC_WAKEUP_WAKEUPEN14_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN14_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN14;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_WAKEUPEN15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_WAKEUP_WAKEUPEN15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp = (tmp & EIC_WAKEUP_WAKEUPEN15) >> EIC_WAKEUP_WAKEUPEN15_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_WAKEUP_WAKEUPEN15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= ~EIC_WAKEUP_WAKEUPEN15;
	tmp |= value << EIC_WAKEUP_WAKEUPEN15_Pos;
	((Eic *)hw)->WAKEUP.reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_WAKEUPEN15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_WAKEUPEN15_bit(const void *const hw)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= EIC_WAKEUP_WAKEUPEN15;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_WAKEUP_reg(const void *const hw, hri_eic_wakeup_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_wakeup_reg_t hri_eic_get_WAKEUP_reg(const void *const hw, hri_eic_wakeup_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->WAKEUP.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_WAKEUP_reg(const void *const hw, hri_eic_wakeup_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_WAKEUP_reg(const void *const hw, hri_eic_wakeup_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_WAKEUP_reg(const void *const hw, hri_eic_wakeup_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->WAKEUP.reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_wakeup_reg_t hri_eic_read_WAKEUP_reg(const void *const hw)
{
	return ((Eic *)hw)->WAKEUP.reg;
}

static inline void hri_eic_set_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN0) >> EIC_CONFIG_FILTEN0_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN0;
	tmp |= value << EIC_CONFIG_FILTEN0_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN0_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN0;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN1) >> EIC_CONFIG_FILTEN1_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN1;
	tmp |= value << EIC_CONFIG_FILTEN1_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN1_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN1;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN2) >> EIC_CONFIG_FILTEN2_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN2;
	tmp |= value << EIC_CONFIG_FILTEN2_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN2_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN2;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN3) >> EIC_CONFIG_FILTEN3_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN3;
	tmp |= value << EIC_CONFIG_FILTEN3_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN3_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN3;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN4) >> EIC_CONFIG_FILTEN4_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN4;
	tmp |= value << EIC_CONFIG_FILTEN4_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN4_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN4;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN5) >> EIC_CONFIG_FILTEN5_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN5;
	tmp |= value << EIC_CONFIG_FILTEN5_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN5_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN5;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN6) >> EIC_CONFIG_FILTEN6_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN6;
	tmp |= value << EIC_CONFIG_FILTEN6_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN6_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN6;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_eic_get_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_FILTEN7) >> EIC_CONFIG_FILTEN7_Pos;
	return (bool)tmp;
}

static inline void hri_eic_write_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_FILTEN7;
	tmp |= value << EIC_CONFIG_FILTEN7_Pos;
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_FILTEN7_bit(const void *const hw, uint8_t index)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_FILTEN7;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_set_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE0_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE0(mask)) >> EIC_CONFIG_SENSE0_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE0_Msk;
	tmp |= EIC_CONFIG_SENSE0(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE0_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE0(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE0_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE0_Msk) >> EIC_CONFIG_SENSE0_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE1_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE1(mask)) >> EIC_CONFIG_SENSE1_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE1_Msk;
	tmp |= EIC_CONFIG_SENSE1(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE1_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE1(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE1_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE1_Msk) >> EIC_CONFIG_SENSE1_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE2_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE2(mask)) >> EIC_CONFIG_SENSE2_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE2_Msk;
	tmp |= EIC_CONFIG_SENSE2(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE2_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE2(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE2_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE2_Msk) >> EIC_CONFIG_SENSE2_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE3_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE3(mask)) >> EIC_CONFIG_SENSE3_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE3_Msk;
	tmp |= EIC_CONFIG_SENSE3(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE3_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE3(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE3_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE3_Msk) >> EIC_CONFIG_SENSE3_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE4_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE4(mask)) >> EIC_CONFIG_SENSE4_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE4_Msk;
	tmp |= EIC_CONFIG_SENSE4(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE4_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE4(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE4_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE4_Msk) >> EIC_CONFIG_SENSE4_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE5_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE5(mask)) >> EIC_CONFIG_SENSE5_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE5_Msk;
	tmp |= EIC_CONFIG_SENSE5(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE5_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE5(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE5_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE5_Msk) >> EIC_CONFIG_SENSE5_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE6_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE6(mask)) >> EIC_CONFIG_SENSE6_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE6_Msk;
	tmp |= EIC_CONFIG_SENSE6(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE6_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE6(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE6_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE6_Msk) >> EIC_CONFIG_SENSE6_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_SENSE7_bf(const void *const hw, uint8_t index,
                                                                hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE7(mask)) >> EIC_CONFIG_SENSE7_Pos;
	return tmp;
}

static inline void hri_eic_write_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	uint32_t tmp;
	EIC_CRITICAL_SECTION_ENTER();
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= ~EIC_CONFIG_SENSE7_Msk;
	tmp |= EIC_CONFIG_SENSE7(data);
	((Eic *)hw)->CONFIG[index].reg = tmp;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_SENSE7_bf(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= EIC_CONFIG_SENSE7(mask);
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_SENSE7_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp = (tmp & EIC_CONFIG_SENSE7_Msk) >> EIC_CONFIG_SENSE7_Pos;
	return tmp;
}

static inline void hri_eic_set_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg |= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_get_CONFIG_reg(const void *const hw, uint8_t index,
                                                          hri_eic_config_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Eic *)hw)->CONFIG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_eic_write_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t data)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg = data;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_clear_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg &= ~mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_eic_toggle_CONFIG_reg(const void *const hw, uint8_t index, hri_eic_config_reg_t mask)
{
	EIC_CRITICAL_SECTION_ENTER();
	((Eic *)hw)->CONFIG[index].reg ^= mask;
	EIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_eic_config_reg_t hri_eic_read_CONFIG_reg(const void *const hw, uint8_t index)
{
	return ((Eic *)hw)->CONFIG[index].reg;
}

static inline bool hri_eic_get_STATUS_SYNCBUSY_bit(const void *const hw)
{
	return (((Eic *)hw)->STATUS.reg & EIC_STATUS_SYNCBUSY) >> EIC_STATUS_SYNCBUSY_Pos;
}

static inline hri_eic_status_reg_t hri_eic_get_STATUS_reg(const void *const hw, hri_eic_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Eic *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_eic_status_reg_t hri_eic_read_STATUS_reg(const void *const hw)
{
	return ((Eic *)hw)->STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_EIC_D21_H_INCLUDED */
#endif /* _SAMD21_EIC_COMPONENT_ */
