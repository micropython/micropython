/**
 * \file
 *
 * \brief SAM EVSYS
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

#ifdef _SAMD21_EVSYS_COMPONENT_
#ifndef _HRI_EVSYS_D21_H_INCLUDED_
#define _HRI_EVSYS_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_EVSYS_CRITICAL_SECTIONS)
#define EVSYS_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define EVSYS_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define EVSYS_CRITICAL_SECTION_ENTER()
#define EVSYS_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_evsys_user_reg_t;
typedef uint32_t hri_evsys_channel_reg_t;
typedef uint32_t hri_evsys_chstatus_reg_t;
typedef uint32_t hri_evsys_intenset_reg_t;
typedef uint32_t hri_evsys_intflag_reg_t;
typedef uint8_t  hri_evsys_ctrl_reg_t;

static inline void hri_evsys_set_INTEN_OVR0_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR0;
}

static inline bool hri_evsys_get_INTEN_OVR0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR0) >> EVSYS_INTENSET_OVR0_Pos;
}

static inline void hri_evsys_write_INTEN_OVR0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR0;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR0;
	}
}

static inline void hri_evsys_clear_INTEN_OVR0_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR0;
}

static inline void hri_evsys_set_INTEN_OVR1_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR1;
}

static inline bool hri_evsys_get_INTEN_OVR1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR1) >> EVSYS_INTENSET_OVR1_Pos;
}

static inline void hri_evsys_write_INTEN_OVR1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR1;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR1;
	}
}

static inline void hri_evsys_clear_INTEN_OVR1_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR1;
}

static inline void hri_evsys_set_INTEN_OVR2_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR2;
}

static inline bool hri_evsys_get_INTEN_OVR2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR2) >> EVSYS_INTENSET_OVR2_Pos;
}

static inline void hri_evsys_write_INTEN_OVR2_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR2;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR2;
	}
}

static inline void hri_evsys_clear_INTEN_OVR2_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR2;
}

static inline void hri_evsys_set_INTEN_OVR3_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR3;
}

static inline bool hri_evsys_get_INTEN_OVR3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR3) >> EVSYS_INTENSET_OVR3_Pos;
}

static inline void hri_evsys_write_INTEN_OVR3_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR3;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR3;
	}
}

static inline void hri_evsys_clear_INTEN_OVR3_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR3;
}

static inline void hri_evsys_set_INTEN_OVR4_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR4;
}

static inline bool hri_evsys_get_INTEN_OVR4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR4) >> EVSYS_INTENSET_OVR4_Pos;
}

static inline void hri_evsys_write_INTEN_OVR4_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR4;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR4;
	}
}

static inline void hri_evsys_clear_INTEN_OVR4_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR4;
}

static inline void hri_evsys_set_INTEN_OVR5_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR5;
}

static inline bool hri_evsys_get_INTEN_OVR5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR5) >> EVSYS_INTENSET_OVR5_Pos;
}

static inline void hri_evsys_write_INTEN_OVR5_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR5;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR5;
	}
}

static inline void hri_evsys_clear_INTEN_OVR5_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR5;
}

static inline void hri_evsys_set_INTEN_OVR6_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR6;
}

static inline bool hri_evsys_get_INTEN_OVR6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR6) >> EVSYS_INTENSET_OVR6_Pos;
}

static inline void hri_evsys_write_INTEN_OVR6_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR6;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR6;
	}
}

static inline void hri_evsys_clear_INTEN_OVR6_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR6;
}

static inline void hri_evsys_set_INTEN_OVR7_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR7;
}

static inline bool hri_evsys_get_INTEN_OVR7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR7) >> EVSYS_INTENSET_OVR7_Pos;
}

static inline void hri_evsys_write_INTEN_OVR7_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR7;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR7;
	}
}

static inline void hri_evsys_clear_INTEN_OVR7_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR7;
}

static inline void hri_evsys_set_INTEN_EVD0_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD0;
}

static inline bool hri_evsys_get_INTEN_EVD0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD0) >> EVSYS_INTENSET_EVD0_Pos;
}

static inline void hri_evsys_write_INTEN_EVD0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD0;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD0;
	}
}

static inline void hri_evsys_clear_INTEN_EVD0_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD0;
}

static inline void hri_evsys_set_INTEN_EVD1_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD1;
}

static inline bool hri_evsys_get_INTEN_EVD1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD1) >> EVSYS_INTENSET_EVD1_Pos;
}

static inline void hri_evsys_write_INTEN_EVD1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD1;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD1;
	}
}

static inline void hri_evsys_clear_INTEN_EVD1_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD1;
}

static inline void hri_evsys_set_INTEN_EVD2_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD2;
}

static inline bool hri_evsys_get_INTEN_EVD2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD2) >> EVSYS_INTENSET_EVD2_Pos;
}

static inline void hri_evsys_write_INTEN_EVD2_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD2;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD2;
	}
}

static inline void hri_evsys_clear_INTEN_EVD2_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD2;
}

static inline void hri_evsys_set_INTEN_EVD3_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD3;
}

static inline bool hri_evsys_get_INTEN_EVD3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD3) >> EVSYS_INTENSET_EVD3_Pos;
}

static inline void hri_evsys_write_INTEN_EVD3_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD3;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD3;
	}
}

static inline void hri_evsys_clear_INTEN_EVD3_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD3;
}

static inline void hri_evsys_set_INTEN_EVD4_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD4;
}

static inline bool hri_evsys_get_INTEN_EVD4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD4) >> EVSYS_INTENSET_EVD4_Pos;
}

static inline void hri_evsys_write_INTEN_EVD4_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD4;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD4;
	}
}

static inline void hri_evsys_clear_INTEN_EVD4_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD4;
}

static inline void hri_evsys_set_INTEN_EVD5_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD5;
}

static inline bool hri_evsys_get_INTEN_EVD5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD5) >> EVSYS_INTENSET_EVD5_Pos;
}

static inline void hri_evsys_write_INTEN_EVD5_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD5;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD5;
	}
}

static inline void hri_evsys_clear_INTEN_EVD5_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD5;
}

static inline void hri_evsys_set_INTEN_EVD6_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD6;
}

static inline bool hri_evsys_get_INTEN_EVD6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD6) >> EVSYS_INTENSET_EVD6_Pos;
}

static inline void hri_evsys_write_INTEN_EVD6_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD6;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD6;
	}
}

static inline void hri_evsys_clear_INTEN_EVD6_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD6;
}

static inline void hri_evsys_set_INTEN_EVD7_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD7;
}

static inline bool hri_evsys_get_INTEN_EVD7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD7) >> EVSYS_INTENSET_EVD7_Pos;
}

static inline void hri_evsys_write_INTEN_EVD7_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD7;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD7;
	}
}

static inline void hri_evsys_clear_INTEN_EVD7_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD7;
}

static inline void hri_evsys_set_INTEN_OVR8_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR8;
}

static inline bool hri_evsys_get_INTEN_OVR8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR8) >> EVSYS_INTENSET_OVR8_Pos;
}

static inline void hri_evsys_write_INTEN_OVR8_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR8;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR8;
	}
}

static inline void hri_evsys_clear_INTEN_OVR8_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR8;
}

static inline void hri_evsys_set_INTEN_OVR9_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR9;
}

static inline bool hri_evsys_get_INTEN_OVR9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR9) >> EVSYS_INTENSET_OVR9_Pos;
}

static inline void hri_evsys_write_INTEN_OVR9_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR9;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR9;
	}
}

static inline void hri_evsys_clear_INTEN_OVR9_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR9;
}

static inline void hri_evsys_set_INTEN_OVR10_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR10;
}

static inline bool hri_evsys_get_INTEN_OVR10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR10) >> EVSYS_INTENSET_OVR10_Pos;
}

static inline void hri_evsys_write_INTEN_OVR10_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR10;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR10;
	}
}

static inline void hri_evsys_clear_INTEN_OVR10_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR10;
}

static inline void hri_evsys_set_INTEN_OVR11_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR11;
}

static inline bool hri_evsys_get_INTEN_OVR11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_OVR11) >> EVSYS_INTENSET_OVR11_Pos;
}

static inline void hri_evsys_write_INTEN_OVR11_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR11;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_OVR11;
	}
}

static inline void hri_evsys_clear_INTEN_OVR11_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_OVR11;
}

static inline void hri_evsys_set_INTEN_EVD8_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD8;
}

static inline bool hri_evsys_get_INTEN_EVD8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD8) >> EVSYS_INTENSET_EVD8_Pos;
}

static inline void hri_evsys_write_INTEN_EVD8_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD8;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD8;
	}
}

static inline void hri_evsys_clear_INTEN_EVD8_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD8;
}

static inline void hri_evsys_set_INTEN_EVD9_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD9;
}

static inline bool hri_evsys_get_INTEN_EVD9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD9) >> EVSYS_INTENSET_EVD9_Pos;
}

static inline void hri_evsys_write_INTEN_EVD9_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD9;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD9;
	}
}

static inline void hri_evsys_clear_INTEN_EVD9_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD9;
}

static inline void hri_evsys_set_INTEN_EVD10_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD10;
}

static inline bool hri_evsys_get_INTEN_EVD10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD10) >> EVSYS_INTENSET_EVD10_Pos;
}

static inline void hri_evsys_write_INTEN_EVD10_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD10;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD10;
	}
}

static inline void hri_evsys_clear_INTEN_EVD10_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD10;
}

static inline void hri_evsys_set_INTEN_EVD11_bit(const void *const hw)
{
	((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD11;
}

static inline bool hri_evsys_get_INTEN_EVD11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTENSET.reg & EVSYS_INTENSET_EVD11) >> EVSYS_INTENSET_EVD11_Pos;
}

static inline void hri_evsys_write_INTEN_EVD11_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD11;
	} else {
		((Evsys *)hw)->INTENSET.reg = EVSYS_INTENSET_EVD11;
	}
}

static inline void hri_evsys_clear_INTEN_EVD11_bit(const void *const hw)
{
	((Evsys *)hw)->INTENCLR.reg = EVSYS_INTENSET_EVD11;
}

static inline void hri_evsys_set_INTEN_reg(const void *const hw, hri_evsys_intenset_reg_t mask)
{
	((Evsys *)hw)->INTENSET.reg = mask;
}

static inline hri_evsys_intenset_reg_t hri_evsys_get_INTEN_reg(const void *const hw, hri_evsys_intenset_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_intenset_reg_t hri_evsys_read_INTEN_reg(const void *const hw)
{
	return ((Evsys *)hw)->INTENSET.reg;
}

static inline void hri_evsys_write_INTEN_reg(const void *const hw, hri_evsys_intenset_reg_t data)
{
	((Evsys *)hw)->INTENSET.reg = data;
	((Evsys *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_evsys_clear_INTEN_reg(const void *const hw, hri_evsys_intenset_reg_t mask)
{
	((Evsys *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_evsys_get_INTFLAG_OVR0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR0) >> EVSYS_INTFLAG_OVR0_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR0_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR0;
}

static inline bool hri_evsys_get_INTFLAG_OVR1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR1) >> EVSYS_INTFLAG_OVR1_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR1_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR1;
}

static inline bool hri_evsys_get_INTFLAG_OVR2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR2) >> EVSYS_INTFLAG_OVR2_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR2_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR2;
}

static inline bool hri_evsys_get_INTFLAG_OVR3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR3) >> EVSYS_INTFLAG_OVR3_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR3_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR3;
}

static inline bool hri_evsys_get_INTFLAG_OVR4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR4) >> EVSYS_INTFLAG_OVR4_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR4_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR4;
}

static inline bool hri_evsys_get_INTFLAG_OVR5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR5) >> EVSYS_INTFLAG_OVR5_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR5_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR5;
}

static inline bool hri_evsys_get_INTFLAG_OVR6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR6) >> EVSYS_INTFLAG_OVR6_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR6_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR6;
}

static inline bool hri_evsys_get_INTFLAG_OVR7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR7) >> EVSYS_INTFLAG_OVR7_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR7_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR7;
}

static inline bool hri_evsys_get_INTFLAG_EVD0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD0) >> EVSYS_INTFLAG_EVD0_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD0_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD0;
}

static inline bool hri_evsys_get_INTFLAG_EVD1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD1) >> EVSYS_INTFLAG_EVD1_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD1_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD1;
}

static inline bool hri_evsys_get_INTFLAG_EVD2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD2) >> EVSYS_INTFLAG_EVD2_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD2_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD2;
}

static inline bool hri_evsys_get_INTFLAG_EVD3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD3) >> EVSYS_INTFLAG_EVD3_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD3_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD3;
}

static inline bool hri_evsys_get_INTFLAG_EVD4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD4) >> EVSYS_INTFLAG_EVD4_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD4_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD4;
}

static inline bool hri_evsys_get_INTFLAG_EVD5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD5) >> EVSYS_INTFLAG_EVD5_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD5_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD5;
}

static inline bool hri_evsys_get_INTFLAG_EVD6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD6) >> EVSYS_INTFLAG_EVD6_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD6_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD6;
}

static inline bool hri_evsys_get_INTFLAG_EVD7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD7) >> EVSYS_INTFLAG_EVD7_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD7_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD7;
}

static inline bool hri_evsys_get_INTFLAG_OVR8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR8) >> EVSYS_INTFLAG_OVR8_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR8_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR8;
}

static inline bool hri_evsys_get_INTFLAG_OVR9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR9) >> EVSYS_INTFLAG_OVR9_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR9_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR9;
}

static inline bool hri_evsys_get_INTFLAG_OVR10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR10) >> EVSYS_INTFLAG_OVR10_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR10_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR10;
}

static inline bool hri_evsys_get_INTFLAG_OVR11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR11) >> EVSYS_INTFLAG_OVR11_Pos;
}

static inline void hri_evsys_clear_INTFLAG_OVR11_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR11;
}

static inline bool hri_evsys_get_INTFLAG_EVD8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD8) >> EVSYS_INTFLAG_EVD8_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD8_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD8;
}

static inline bool hri_evsys_get_INTFLAG_EVD9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD9) >> EVSYS_INTFLAG_EVD9_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD9_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD9;
}

static inline bool hri_evsys_get_INTFLAG_EVD10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD10) >> EVSYS_INTFLAG_EVD10_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD10_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD10;
}

static inline bool hri_evsys_get_INTFLAG_EVD11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD11) >> EVSYS_INTFLAG_EVD11_Pos;
}

static inline void hri_evsys_clear_INTFLAG_EVD11_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD11;
}

static inline bool hri_evsys_get_interrupt_OVR0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR0) >> EVSYS_INTFLAG_OVR0_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR0_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR0;
}

static inline bool hri_evsys_get_interrupt_OVR1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR1) >> EVSYS_INTFLAG_OVR1_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR1_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR1;
}

static inline bool hri_evsys_get_interrupt_OVR2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR2) >> EVSYS_INTFLAG_OVR2_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR2_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR2;
}

static inline bool hri_evsys_get_interrupt_OVR3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR3) >> EVSYS_INTFLAG_OVR3_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR3_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR3;
}

static inline bool hri_evsys_get_interrupt_OVR4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR4) >> EVSYS_INTFLAG_OVR4_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR4_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR4;
}

static inline bool hri_evsys_get_interrupt_OVR5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR5) >> EVSYS_INTFLAG_OVR5_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR5_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR5;
}

static inline bool hri_evsys_get_interrupt_OVR6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR6) >> EVSYS_INTFLAG_OVR6_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR6_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR6;
}

static inline bool hri_evsys_get_interrupt_OVR7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR7) >> EVSYS_INTFLAG_OVR7_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR7_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR7;
}

static inline bool hri_evsys_get_interrupt_EVD0_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD0) >> EVSYS_INTFLAG_EVD0_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD0_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD0;
}

static inline bool hri_evsys_get_interrupt_EVD1_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD1) >> EVSYS_INTFLAG_EVD1_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD1_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD1;
}

static inline bool hri_evsys_get_interrupt_EVD2_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD2) >> EVSYS_INTFLAG_EVD2_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD2_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD2;
}

static inline bool hri_evsys_get_interrupt_EVD3_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD3) >> EVSYS_INTFLAG_EVD3_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD3_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD3;
}

static inline bool hri_evsys_get_interrupt_EVD4_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD4) >> EVSYS_INTFLAG_EVD4_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD4_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD4;
}

static inline bool hri_evsys_get_interrupt_EVD5_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD5) >> EVSYS_INTFLAG_EVD5_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD5_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD5;
}

static inline bool hri_evsys_get_interrupt_EVD6_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD6) >> EVSYS_INTFLAG_EVD6_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD6_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD6;
}

static inline bool hri_evsys_get_interrupt_EVD7_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD7) >> EVSYS_INTFLAG_EVD7_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD7_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD7;
}

static inline bool hri_evsys_get_interrupt_OVR8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR8) >> EVSYS_INTFLAG_OVR8_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR8_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR8;
}

static inline bool hri_evsys_get_interrupt_OVR9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR9) >> EVSYS_INTFLAG_OVR9_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR9_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR9;
}

static inline bool hri_evsys_get_interrupt_OVR10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR10) >> EVSYS_INTFLAG_OVR10_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR10_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR10;
}

static inline bool hri_evsys_get_interrupt_OVR11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_OVR11) >> EVSYS_INTFLAG_OVR11_Pos;
}

static inline void hri_evsys_clear_interrupt_OVR11_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_OVR11;
}

static inline bool hri_evsys_get_interrupt_EVD8_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD8) >> EVSYS_INTFLAG_EVD8_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD8_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD8;
}

static inline bool hri_evsys_get_interrupt_EVD9_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD9) >> EVSYS_INTFLAG_EVD9_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD9_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD9;
}

static inline bool hri_evsys_get_interrupt_EVD10_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD10) >> EVSYS_INTFLAG_EVD10_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD10_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD10;
}

static inline bool hri_evsys_get_interrupt_EVD11_bit(const void *const hw)
{
	return (((Evsys *)hw)->INTFLAG.reg & EVSYS_INTFLAG_EVD11) >> EVSYS_INTFLAG_EVD11_Pos;
}

static inline void hri_evsys_clear_interrupt_EVD11_bit(const void *const hw)
{
	((Evsys *)hw)->INTFLAG.reg = EVSYS_INTFLAG_EVD11;
}

static inline hri_evsys_intflag_reg_t hri_evsys_get_INTFLAG_reg(const void *const hw, hri_evsys_intflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_intflag_reg_t hri_evsys_read_INTFLAG_reg(const void *const hw)
{
	return ((Evsys *)hw)->INTFLAG.reg;
}

static inline void hri_evsys_clear_INTFLAG_reg(const void *const hw, hri_evsys_intflag_reg_t mask)
{
	((Evsys *)hw)->INTFLAG.reg = mask;
}

static inline void hri_evsys_write_CTRL_reg(const void *const hw, hri_evsys_ctrl_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CTRL.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_CHANNEL_SWEVT_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_SWEVT;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_evsys_get_CHANNEL_SWEVT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_SWEVT) >> EVSYS_CHANNEL_SWEVT_Pos;
	return (bool)tmp;
}

static inline void hri_evsys_write_CHANNEL_SWEVT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_SWEVT;
	tmp |= value << EVSYS_CHANNEL_SWEVT_Pos;
	((Evsys *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_SWEVT_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_SWEVT;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_SWEVT_bit(const void *const hw)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_SWEVT;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_set_CHANNEL_CHANNEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_CHANNEL_bf(const void *const       hw,
                                                                       hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_CHANNEL(mask)) >> EVSYS_CHANNEL_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_CHANNEL_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_CHANNEL_Msk;
	tmp |= EVSYS_CHANNEL_CHANNEL(data);
	((Evsys *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_CHANNEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_CHANNEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_CHANNEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_CHANNEL_Msk) >> EVSYS_CHANNEL_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN(mask)) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EVGEN_Msk;
	tmp |= EVSYS_CHANNEL_EVGEN(data);
	((Evsys *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_EVGEN_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_EVGEN(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_EVGEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EVGEN_Msk) >> EVSYS_CHANNEL_EVGEN_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH(mask)) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_PATH_Msk;
	tmp |= EVSYS_CHANNEL_PATH(data);
	((Evsys *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_PATH_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_PATH(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_PATH_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_PATH_Msk) >> EVSYS_CHANNEL_PATH_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_EDGSEL_bf(const void *const       hw,
                                                                      hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL(mask)) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t data)
{
	uint32_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= ~EVSYS_CHANNEL_EDGSEL_Msk;
	tmp |= EVSYS_CHANNEL_EDGSEL(data);
	((Evsys *)hw)->CHANNEL.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_EDGSEL_bf(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= EVSYS_CHANNEL_EDGSEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_EDGSEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp = (tmp & EVSYS_CHANNEL_EDGSEL_Msk) >> EVSYS_CHANNEL_EDGSEL_Pos;
	return tmp;
}

static inline void hri_evsys_set_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_get_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHANNEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_CHANNEL_reg(const void *const hw, hri_evsys_channel_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->CHANNEL.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_channel_reg_t hri_evsys_read_CHANNEL_reg(const void *const hw)
{
	return ((Evsys *)hw)->CHANNEL.reg;
}

static inline void hri_evsys_set_USER_USER_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg |= EVSYS_USER_USER(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_get_USER_USER_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->USER.reg;
	tmp = (tmp & EVSYS_USER_USER(mask)) >> EVSYS_USER_USER_Pos;
	return tmp;
}

static inline void hri_evsys_write_USER_USER_bf(const void *const hw, hri_evsys_user_reg_t data)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->USER.reg;
	tmp &= ~EVSYS_USER_USER_Msk;
	tmp |= EVSYS_USER_USER(data);
	((Evsys *)hw)->USER.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_USER_USER_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg &= ~EVSYS_USER_USER(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_USER_USER_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg ^= EVSYS_USER_USER(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_read_USER_USER_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->USER.reg;
	tmp = (tmp & EVSYS_USER_USER_Msk) >> EVSYS_USER_USER_Pos;
	return tmp;
}

static inline void hri_evsys_set_USER_CHANNEL_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg |= EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_get_USER_CHANNEL_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->USER.reg;
	tmp = (tmp & EVSYS_USER_CHANNEL(mask)) >> EVSYS_USER_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_write_USER_CHANNEL_bf(const void *const hw, hri_evsys_user_reg_t data)
{
	uint16_t tmp;
	EVSYS_CRITICAL_SECTION_ENTER();
	tmp = ((Evsys *)hw)->USER.reg;
	tmp &= ~EVSYS_USER_CHANNEL_Msk;
	tmp |= EVSYS_USER_CHANNEL(data);
	((Evsys *)hw)->USER.reg = tmp;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_USER_CHANNEL_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg &= ~EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_USER_CHANNEL_bf(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg ^= EVSYS_USER_CHANNEL(mask);
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_read_USER_CHANNEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->USER.reg;
	tmp = (tmp & EVSYS_USER_CHANNEL_Msk) >> EVSYS_USER_CHANNEL_Pos;
	return tmp;
}

static inline void hri_evsys_set_USER_reg(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg |= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_get_USER_reg(const void *const hw, hri_evsys_user_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Evsys *)hw)->USER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_evsys_write_USER_reg(const void *const hw, hri_evsys_user_reg_t data)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg = data;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_clear_USER_reg(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg &= ~mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline void hri_evsys_toggle_USER_reg(const void *const hw, hri_evsys_user_reg_t mask)
{
	EVSYS_CRITICAL_SECTION_ENTER();
	((Evsys *)hw)->USER.reg ^= mask;
	EVSYS_CRITICAL_SECTION_LEAVE();
}

static inline hri_evsys_user_reg_t hri_evsys_read_USER_reg(const void *const hw)
{
	return ((Evsys *)hw)->USER.reg;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY0_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY0) >> EVSYS_CHSTATUS_USRRDY0_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY1_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY1) >> EVSYS_CHSTATUS_USRRDY1_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY2_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY2) >> EVSYS_CHSTATUS_USRRDY2_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY3_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY3) >> EVSYS_CHSTATUS_USRRDY3_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY4_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY4) >> EVSYS_CHSTATUS_USRRDY4_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY5_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY5) >> EVSYS_CHSTATUS_USRRDY5_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY6_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY6) >> EVSYS_CHSTATUS_USRRDY6_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY7_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY7) >> EVSYS_CHSTATUS_USRRDY7_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY0_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY0) >> EVSYS_CHSTATUS_CHBUSY0_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY1_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY1) >> EVSYS_CHSTATUS_CHBUSY1_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY2_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY2) >> EVSYS_CHSTATUS_CHBUSY2_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY3_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY3) >> EVSYS_CHSTATUS_CHBUSY3_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY4_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY4) >> EVSYS_CHSTATUS_CHBUSY4_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY5_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY5) >> EVSYS_CHSTATUS_CHBUSY5_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY6_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY6) >> EVSYS_CHSTATUS_CHBUSY6_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY7_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY7) >> EVSYS_CHSTATUS_CHBUSY7_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY8_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY8) >> EVSYS_CHSTATUS_USRRDY8_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY9_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY9) >> EVSYS_CHSTATUS_USRRDY9_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY10_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY10) >> EVSYS_CHSTATUS_USRRDY10_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_USRRDY11_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_USRRDY11) >> EVSYS_CHSTATUS_USRRDY11_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY8_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY8) >> EVSYS_CHSTATUS_CHBUSY8_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY9_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY9) >> EVSYS_CHSTATUS_CHBUSY9_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY10_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY10) >> EVSYS_CHSTATUS_CHBUSY10_Pos;
}

static inline bool hri_evsys_get_CHSTATUS_CHBUSY11_bit(const void *const hw)
{
	return (((Evsys *)hw)->CHSTATUS.reg & EVSYS_CHSTATUS_CHBUSY11) >> EVSYS_CHSTATUS_CHBUSY11_Pos;
}

static inline hri_evsys_chstatus_reg_t hri_evsys_get_CHSTATUS_reg(const void *const hw, hri_evsys_chstatus_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Evsys *)hw)->CHSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_evsys_chstatus_reg_t hri_evsys_read_CHSTATUS_reg(const void *const hw)
{
	return ((Evsys *)hw)->CHSTATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_EVSYS_D21_H_INCLUDED */
#endif /* _SAMD21_EVSYS_COMPONENT_ */
