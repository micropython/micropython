/**
 * \file
 *
 * \brief SAM I2S
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

#ifdef _SAMD51_I2S_COMPONENT_
#ifndef _HRI_I2S_D51_H_INCLUDED_
#define _HRI_I2S_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_I2S_CRITICAL_SECTIONS)
#define I2S_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define I2S_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define I2S_CRITICAL_SECTION_ENTER()
#define I2S_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_i2s_intenset_reg_t;
typedef uint16_t hri_i2s_intflag_reg_t;
typedef uint16_t hri_i2s_syncbusy_reg_t;
typedef uint32_t hri_i2s_clkctrl_reg_t;
typedef uint32_t hri_i2s_rxctrl_reg_t;
typedef uint32_t hri_i2s_rxdata_reg_t;
typedef uint32_t hri_i2s_txctrl_reg_t;
typedef uint32_t hri_i2s_txdata_reg_t;
typedef uint8_t  hri_i2s_ctrla_reg_t;

static inline void hri_i2s_wait_for_sync(const void *const hw, hri_i2s_syncbusy_reg_t reg)
{
	while (((I2s *)hw)->SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_i2s_is_syncing(const void *const hw, hri_i2s_syncbusy_reg_t reg)
{
	return ((I2s *)hw)->SYNCBUSY.reg & reg;
}

static inline void hri_i2s_set_INTEN_RXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXRDY0;
}

static inline bool hri_i2s_get_INTEN_RXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_RXRDY0) >> I2S_INTENSET_RXRDY0_Pos;
}

static inline void hri_i2s_write_INTEN_RXRDY0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXRDY0;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXRDY0;
	}
}

static inline void hri_i2s_clear_INTEN_RXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXRDY0;
}

static inline void hri_i2s_set_INTEN_RXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXRDY1;
}

static inline bool hri_i2s_get_INTEN_RXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_RXRDY1) >> I2S_INTENSET_RXRDY1_Pos;
}

static inline void hri_i2s_write_INTEN_RXRDY1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXRDY1;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXRDY1;
	}
}

static inline void hri_i2s_clear_INTEN_RXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXRDY1;
}

static inline void hri_i2s_set_INTEN_RXOR0_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXOR0;
}

static inline bool hri_i2s_get_INTEN_RXOR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_RXOR0) >> I2S_INTENSET_RXOR0_Pos;
}

static inline void hri_i2s_write_INTEN_RXOR0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXOR0;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXOR0;
	}
}

static inline void hri_i2s_clear_INTEN_RXOR0_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXOR0;
}

static inline void hri_i2s_set_INTEN_RXOR1_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXOR1;
}

static inline bool hri_i2s_get_INTEN_RXOR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_RXOR1) >> I2S_INTENSET_RXOR1_Pos;
}

static inline void hri_i2s_write_INTEN_RXOR1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXOR1;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_RXOR1;
	}
}

static inline void hri_i2s_clear_INTEN_RXOR1_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_RXOR1;
}

static inline void hri_i2s_set_INTEN_TXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXRDY0;
}

static inline bool hri_i2s_get_INTEN_TXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_TXRDY0) >> I2S_INTENSET_TXRDY0_Pos;
}

static inline void hri_i2s_write_INTEN_TXRDY0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXRDY0;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXRDY0;
	}
}

static inline void hri_i2s_clear_INTEN_TXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXRDY0;
}

static inline void hri_i2s_set_INTEN_TXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXRDY1;
}

static inline bool hri_i2s_get_INTEN_TXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_TXRDY1) >> I2S_INTENSET_TXRDY1_Pos;
}

static inline void hri_i2s_write_INTEN_TXRDY1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXRDY1;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXRDY1;
	}
}

static inline void hri_i2s_clear_INTEN_TXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXRDY1;
}

static inline void hri_i2s_set_INTEN_TXUR0_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXUR0;
}

static inline bool hri_i2s_get_INTEN_TXUR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_TXUR0) >> I2S_INTENSET_TXUR0_Pos;
}

static inline void hri_i2s_write_INTEN_TXUR0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXUR0;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXUR0;
	}
}

static inline void hri_i2s_clear_INTEN_TXUR0_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXUR0;
}

static inline void hri_i2s_set_INTEN_TXUR1_bit(const void *const hw)
{
	((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXUR1;
}

static inline bool hri_i2s_get_INTEN_TXUR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTENSET.reg & I2S_INTENSET_TXUR1) >> I2S_INTENSET_TXUR1_Pos;
}

static inline void hri_i2s_write_INTEN_TXUR1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXUR1;
	} else {
		((I2s *)hw)->INTENSET.reg = I2S_INTENSET_TXUR1;
	}
}

static inline void hri_i2s_clear_INTEN_TXUR1_bit(const void *const hw)
{
	((I2s *)hw)->INTENCLR.reg = I2S_INTENSET_TXUR1;
}

static inline void hri_i2s_set_INTEN_reg(const void *const hw, hri_i2s_intenset_reg_t mask)
{
	((I2s *)hw)->INTENSET.reg = mask;
}

static inline hri_i2s_intenset_reg_t hri_i2s_get_INTEN_reg(const void *const hw, hri_i2s_intenset_reg_t mask)
{
	uint16_t tmp;
	tmp = ((I2s *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_i2s_intenset_reg_t hri_i2s_read_INTEN_reg(const void *const hw)
{
	return ((I2s *)hw)->INTENSET.reg;
}

static inline void hri_i2s_write_INTEN_reg(const void *const hw, hri_i2s_intenset_reg_t data)
{
	((I2s *)hw)->INTENSET.reg = data;
	((I2s *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_i2s_clear_INTEN_reg(const void *const hw, hri_i2s_intenset_reg_t mask)
{
	((I2s *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_i2s_get_INTFLAG_RXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXRDY0) >> I2S_INTFLAG_RXRDY0_Pos;
}

static inline void hri_i2s_clear_INTFLAG_RXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXRDY0;
}

static inline bool hri_i2s_get_INTFLAG_RXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXRDY1) >> I2S_INTFLAG_RXRDY1_Pos;
}

static inline void hri_i2s_clear_INTFLAG_RXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXRDY1;
}

static inline bool hri_i2s_get_INTFLAG_RXOR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXOR0) >> I2S_INTFLAG_RXOR0_Pos;
}

static inline void hri_i2s_clear_INTFLAG_RXOR0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXOR0;
}

static inline bool hri_i2s_get_INTFLAG_RXOR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXOR1) >> I2S_INTFLAG_RXOR1_Pos;
}

static inline void hri_i2s_clear_INTFLAG_RXOR1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXOR1;
}

static inline bool hri_i2s_get_INTFLAG_TXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXRDY0) >> I2S_INTFLAG_TXRDY0_Pos;
}

static inline void hri_i2s_clear_INTFLAG_TXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXRDY0;
}

static inline bool hri_i2s_get_INTFLAG_TXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXRDY1) >> I2S_INTFLAG_TXRDY1_Pos;
}

static inline void hri_i2s_clear_INTFLAG_TXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXRDY1;
}

static inline bool hri_i2s_get_INTFLAG_TXUR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXUR0) >> I2S_INTFLAG_TXUR0_Pos;
}

static inline void hri_i2s_clear_INTFLAG_TXUR0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXUR0;
}

static inline bool hri_i2s_get_INTFLAG_TXUR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXUR1) >> I2S_INTFLAG_TXUR1_Pos;
}

static inline void hri_i2s_clear_INTFLAG_TXUR1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXUR1;
}

static inline bool hri_i2s_get_interrupt_RXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXRDY0) >> I2S_INTFLAG_RXRDY0_Pos;
}

static inline void hri_i2s_clear_interrupt_RXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXRDY0;
}

static inline bool hri_i2s_get_interrupt_RXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXRDY1) >> I2S_INTFLAG_RXRDY1_Pos;
}

static inline void hri_i2s_clear_interrupt_RXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXRDY1;
}

static inline bool hri_i2s_get_interrupt_RXOR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXOR0) >> I2S_INTFLAG_RXOR0_Pos;
}

static inline void hri_i2s_clear_interrupt_RXOR0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXOR0;
}

static inline bool hri_i2s_get_interrupt_RXOR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_RXOR1) >> I2S_INTFLAG_RXOR1_Pos;
}

static inline void hri_i2s_clear_interrupt_RXOR1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_RXOR1;
}

static inline bool hri_i2s_get_interrupt_TXRDY0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXRDY0) >> I2S_INTFLAG_TXRDY0_Pos;
}

static inline void hri_i2s_clear_interrupt_TXRDY0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXRDY0;
}

static inline bool hri_i2s_get_interrupt_TXRDY1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXRDY1) >> I2S_INTFLAG_TXRDY1_Pos;
}

static inline void hri_i2s_clear_interrupt_TXRDY1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXRDY1;
}

static inline bool hri_i2s_get_interrupt_TXUR0_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXUR0) >> I2S_INTFLAG_TXUR0_Pos;
}

static inline void hri_i2s_clear_interrupt_TXUR0_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXUR0;
}

static inline bool hri_i2s_get_interrupt_TXUR1_bit(const void *const hw)
{
	return (((I2s *)hw)->INTFLAG.reg & I2S_INTFLAG_TXUR1) >> I2S_INTFLAG_TXUR1_Pos;
}

static inline void hri_i2s_clear_interrupt_TXUR1_bit(const void *const hw)
{
	((I2s *)hw)->INTFLAG.reg = I2S_INTFLAG_TXUR1;
}

static inline hri_i2s_intflag_reg_t hri_i2s_get_INTFLAG_reg(const void *const hw, hri_i2s_intflag_reg_t mask)
{
	uint16_t tmp;
	tmp = ((I2s *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_i2s_intflag_reg_t hri_i2s_read_INTFLAG_reg(const void *const hw)
{
	return ((I2s *)hw)->INTFLAG.reg;
}

static inline void hri_i2s_clear_INTFLAG_reg(const void *const hw, hri_i2s_intflag_reg_t mask)
{
	((I2s *)hw)->INTFLAG.reg = mask;
}

static inline void hri_i2s_write_TXDATA_reg(const void *const hw, hri_i2s_txdata_reg_t data)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXDATA.reg = data;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_SWRST_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw, I2S_SYNCBUSY_SWRST);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_SWRST;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw, I2S_SYNCBUSY_SWRST);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_SWRST) >> I2S_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_set_CTRLA_ENABLE_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_ENABLE;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_ENABLE) >> I2S_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= ~I2S_CTRLA_ENABLE;
	tmp |= value << I2S_CTRLA_ENABLE_Pos;
	((I2s *)hw)->CTRLA.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg &= ~I2S_CTRLA_ENABLE;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg ^= I2S_CTRLA_ENABLE;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_CKEN0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_CKEN0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_CKEN0_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_CKEN0) >> I2S_CTRLA_CKEN0_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CTRLA_CKEN0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= ~I2S_CTRLA_CKEN0;
	tmp |= value << I2S_CTRLA_CKEN0_Pos;
	((I2s *)hw)->CTRLA.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_CKEN0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg &= ~I2S_CTRLA_CKEN0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_CKEN0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg ^= I2S_CTRLA_CKEN0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_CKEN1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_CKEN1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_CKEN1_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_CKEN1) >> I2S_CTRLA_CKEN1_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CTRLA_CKEN1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= ~I2S_CTRLA_CKEN1;
	tmp |= value << I2S_CTRLA_CKEN1_Pos;
	((I2s *)hw)->CTRLA.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_CKEN1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg &= ~I2S_CTRLA_CKEN1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_CKEN1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg ^= I2S_CTRLA_CKEN1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_TXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_TXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_TXEN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_TXEN) >> I2S_CTRLA_TXEN_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CTRLA_TXEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= ~I2S_CTRLA_TXEN;
	tmp |= value << I2S_CTRLA_TXEN_Pos;
	((I2s *)hw)->CTRLA.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_TXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg &= ~I2S_CTRLA_TXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_TXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg ^= I2S_CTRLA_TXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_RXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg |= I2S_CTRLA_RXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CTRLA_RXEN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp = (tmp & I2S_CTRLA_RXEN) >> I2S_CTRLA_RXEN_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CTRLA_RXEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= ~I2S_CTRLA_RXEN;
	tmp |= value << I2S_CTRLA_RXEN_Pos;
	((I2s *)hw)->CTRLA.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_RXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg &= ~I2S_CTRLA_RXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_RXEN_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	hri_i2s_wait_for_sync(hw,
	                      I2S_SYNCBUSY_SWRST | I2S_SYNCBUSY_ENABLE | I2S_SYNCBUSY_CKEN0 | I2S_SYNCBUSY_CKEN1
	                          | I2S_SYNCBUSY_TXEN
	                          | I2S_SYNCBUSY_RXEN);
	((I2s *)hw)->CTRLA.reg ^= I2S_CTRLA_RXEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CTRLA_reg(const void *const hw, hri_i2s_ctrla_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CTRLA.reg |= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_ctrla_reg_t hri_i2s_get_CTRLA_reg(const void *const hw, hri_i2s_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((I2s *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_i2s_write_CTRLA_reg(const void *const hw, hri_i2s_ctrla_reg_t data)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CTRLA.reg = data;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CTRLA_reg(const void *const hw, hri_i2s_ctrla_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CTRLA.reg &= ~mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CTRLA_reg(const void *const hw, hri_i2s_ctrla_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CTRLA.reg ^= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_ctrla_reg_t hri_i2s_read_CTRLA_reg(const void *const hw)
{
	return ((I2s *)hw)->CTRLA.reg;
}

static inline void hri_i2s_set_CLKCTRL_BITDELAY_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_BITDELAY;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_BITDELAY_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_BITDELAY) >> I2S_CLKCTRL_BITDELAY_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_BITDELAY_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_BITDELAY;
	tmp |= value << I2S_CLKCTRL_BITDELAY_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_BITDELAY_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_BITDELAY;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_BITDELAY_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_BITDELAY;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_FSSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_FSSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_FSSEL_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_FSSEL) >> I2S_CLKCTRL_FSSEL_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_FSSEL_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_FSSEL;
	tmp |= value << I2S_CLKCTRL_FSSEL_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_FSSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_FSSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_FSSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_FSSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_FSINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_FSINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_FSINV_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_FSINV) >> I2S_CLKCTRL_FSINV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_FSINV_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_FSINV;
	tmp |= value << I2S_CLKCTRL_FSINV_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_FSINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_FSINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_FSINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_FSINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_FSOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_FSOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_FSOUTINV_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_FSOUTINV) >> I2S_CLKCTRL_FSOUTINV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_FSOUTINV_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_FSOUTINV;
	tmp |= value << I2S_CLKCTRL_FSOUTINV_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_FSOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_FSOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_FSOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_FSOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_SCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_SCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_SCKSEL_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_SCKSEL) >> I2S_CLKCTRL_SCKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_SCKSEL_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_SCKSEL;
	tmp |= value << I2S_CLKCTRL_SCKSEL_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_SCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_SCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_SCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_SCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_SCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_SCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_SCKOUTINV_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_SCKOUTINV) >> I2S_CLKCTRL_SCKOUTINV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_SCKOUTINV_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_SCKOUTINV;
	tmp |= value << I2S_CLKCTRL_SCKOUTINV_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_SCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_SCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_SCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_SCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_MCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_MCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_MCKSEL_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKSEL) >> I2S_CLKCTRL_MCKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_MCKSEL_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_MCKSEL;
	tmp |= value << I2S_CLKCTRL_MCKSEL_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_MCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_MCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_MCKSEL_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_MCKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_MCKEN_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_MCKEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_MCKEN_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKEN) >> I2S_CLKCTRL_MCKEN_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_MCKEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_MCKEN;
	tmp |= value << I2S_CLKCTRL_MCKEN_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_MCKEN_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_MCKEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_MCKEN_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_MCKEN;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_MCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_MCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_CLKCTRL_MCKOUTINV_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKOUTINV) >> I2S_CLKCTRL_MCKOUTINV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_CLKCTRL_MCKOUTINV_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_MCKOUTINV;
	tmp |= value << I2S_CLKCTRL_MCKOUTINV_Pos;
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_MCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_MCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_MCKOUTINV_bit(const void *const hw, uint8_t index)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_MCKOUTINV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_SLOTSIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index,
                                                                    hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_SLOTSIZE(mask)) >> I2S_CLKCTRL_SLOTSIZE_Pos;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_SLOTSIZE_Msk;
	tmp |= I2S_CLKCTRL_SLOTSIZE(data);
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_SLOTSIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_SLOTSIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_SLOTSIZE_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_SLOTSIZE_Msk) >> I2S_CLKCTRL_SLOTSIZE_Pos;
	return tmp;
}

static inline void hri_i2s_set_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_NBSLOTS(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index,
                                                                   hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_NBSLOTS(mask)) >> I2S_CLKCTRL_NBSLOTS_Pos;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_NBSLOTS_Msk;
	tmp |= I2S_CLKCTRL_NBSLOTS(data);
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_NBSLOTS(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_NBSLOTS(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_NBSLOTS_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_NBSLOTS_Msk) >> I2S_CLKCTRL_NBSLOTS_Pos;
	return tmp;
}

static inline void hri_i2s_set_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_FSWIDTH(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index,
                                                                   hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_FSWIDTH(mask)) >> I2S_CLKCTRL_FSWIDTH_Pos;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_FSWIDTH_Msk;
	tmp |= I2S_CLKCTRL_FSWIDTH(data);
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_FSWIDTH(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_FSWIDTH(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_FSWIDTH_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_FSWIDTH_Msk) >> I2S_CLKCTRL_FSWIDTH_Pos;
	return tmp;
}

static inline void hri_i2s_set_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_MCKDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index,
                                                                  hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKDIV(mask)) >> I2S_CLKCTRL_MCKDIV_Pos;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_MCKDIV_Msk;
	tmp |= I2S_CLKCTRL_MCKDIV(data);
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_MCKDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_MCKDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_MCKDIV_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKDIV_Msk) >> I2S_CLKCTRL_MCKDIV_Pos;
	return tmp;
}

static inline void hri_i2s_set_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= I2S_CLKCTRL_MCKOUTDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index,
                                                                     hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKOUTDIV(mask)) >> I2S_CLKCTRL_MCKOUTDIV_Pos;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= ~I2S_CLKCTRL_MCKOUTDIV_Msk;
	tmp |= I2S_CLKCTRL_MCKOUTDIV(data);
	((I2s *)hw)->CLKCTRL[index].reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~I2S_CLKCTRL_MCKOUTDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= I2S_CLKCTRL_MCKOUTDIV(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_MCKOUTDIV_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp = (tmp & I2S_CLKCTRL_MCKOUTDIV_Msk) >> I2S_CLKCTRL_MCKOUTDIV_Pos;
	return tmp;
}

static inline void hri_i2s_set_CLKCTRL_reg(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg |= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_get_CLKCTRL_reg(const void *const hw, uint8_t index,
                                                            hri_i2s_clkctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->CLKCTRL[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_i2s_write_CLKCTRL_reg(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t data)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg = data;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_CLKCTRL_reg(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg &= ~mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_CLKCTRL_reg(const void *const hw, uint8_t index, hri_i2s_clkctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->CLKCTRL[index].reg ^= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_clkctrl_reg_t hri_i2s_read_CLKCTRL_reg(const void *const hw, uint8_t index)
{
	return ((I2s *)hw)->CLKCTRL[index].reg;
}

static inline void hri_i2s_set_TXCTRL_TXSAME_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_TXSAME;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_TXSAME_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_TXSAME) >> I2S_TXCTRL_TXSAME_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_TXSAME_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_TXSAME;
	tmp |= value << I2S_TXCTRL_TXSAME_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_TXSAME_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_TXSAME;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_TXSAME_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_TXSAME;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTADJ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTADJ) >> I2S_TXCTRL_SLOTADJ_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTADJ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTADJ;
	tmp |= value << I2S_TXCTRL_SLOTADJ_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_WORDADJ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_WORDADJ) >> I2S_TXCTRL_WORDADJ_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_WORDADJ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_WORDADJ;
	tmp |= value << I2S_TXCTRL_WORDADJ_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_BITREV_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_BITREV) >> I2S_TXCTRL_BITREV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_BITREV_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_BITREV;
	tmp |= value << I2S_TXCTRL_BITREV_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS0) >> I2S_TXCTRL_SLOTDIS0_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS0;
	tmp |= value << I2S_TXCTRL_SLOTDIS0_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS1) >> I2S_TXCTRL_SLOTDIS1_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS1;
	tmp |= value << I2S_TXCTRL_SLOTDIS1_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS2) >> I2S_TXCTRL_SLOTDIS2_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS2;
	tmp |= value << I2S_TXCTRL_SLOTDIS2_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS3) >> I2S_TXCTRL_SLOTDIS3_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS3;
	tmp |= value << I2S_TXCTRL_SLOTDIS3_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS4) >> I2S_TXCTRL_SLOTDIS4_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS4;
	tmp |= value << I2S_TXCTRL_SLOTDIS4_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS5) >> I2S_TXCTRL_SLOTDIS5_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS5;
	tmp |= value << I2S_TXCTRL_SLOTDIS5_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS6) >> I2S_TXCTRL_SLOTDIS6_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS6;
	tmp |= value << I2S_TXCTRL_SLOTDIS6_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_SLOTDIS7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_SLOTDIS7) >> I2S_TXCTRL_SLOTDIS7_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_SLOTDIS7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_SLOTDIS7;
	tmp |= value << I2S_TXCTRL_SLOTDIS7_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_MONO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_MONO) >> I2S_TXCTRL_MONO_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_MONO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_MONO;
	tmp |= value << I2S_TXCTRL_MONO_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_TXCTRL_DMA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_DMA) >> I2S_TXCTRL_DMA_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_TXCTRL_DMA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_DMA;
	tmp |= value << I2S_TXCTRL_DMA_Pos;
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_TXCTRL_TXDEFAULT_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_TXDEFAULT(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_get_TXCTRL_TXDEFAULT_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_TXDEFAULT(mask)) >> I2S_TXCTRL_TXDEFAULT_Pos;
	return tmp;
}

static inline void hri_i2s_write_TXCTRL_TXDEFAULT_bf(const void *const hw, hri_i2s_txctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_TXDEFAULT_Msk;
	tmp |= I2S_TXCTRL_TXDEFAULT(data);
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_TXDEFAULT_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_TXDEFAULT(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_TXDEFAULT_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_TXDEFAULT(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_read_TXCTRL_TXDEFAULT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_TXDEFAULT_Msk) >> I2S_TXCTRL_TXDEFAULT_Pos;
	return tmp;
}

static inline void hri_i2s_set_TXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_get_TXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_DATASIZE(mask)) >> I2S_TXCTRL_DATASIZE_Pos;
	return tmp;
}

static inline void hri_i2s_write_TXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_txctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_DATASIZE_Msk;
	tmp |= I2S_TXCTRL_DATASIZE(data);
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_read_TXCTRL_DATASIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_DATASIZE_Msk) >> I2S_TXCTRL_DATASIZE_Pos;
	return tmp;
}

static inline void hri_i2s_set_TXCTRL_EXTEND_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= I2S_TXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_get_TXCTRL_EXTEND_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_EXTEND(mask)) >> I2S_TXCTRL_EXTEND_Pos;
	return tmp;
}

static inline void hri_i2s_write_TXCTRL_EXTEND_bf(const void *const hw, hri_i2s_txctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= ~I2S_TXCTRL_EXTEND_Msk;
	tmp |= I2S_TXCTRL_EXTEND(data);
	((I2s *)hw)->TXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_EXTEND_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~I2S_TXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_EXTEND_bf(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= I2S_TXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_read_TXCTRL_EXTEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp = (tmp & I2S_TXCTRL_EXTEND_Msk) >> I2S_TXCTRL_EXTEND_Pos;
	return tmp;
}

static inline void hri_i2s_set_TXCTRL_reg(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg |= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_get_TXCTRL_reg(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->TXCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_i2s_write_TXCTRL_reg(const void *const hw, hri_i2s_txctrl_reg_t data)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg = data;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_TXCTRL_reg(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg &= ~mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_TXCTRL_reg(const void *const hw, hri_i2s_txctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->TXCTRL.reg ^= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_txctrl_reg_t hri_i2s_read_TXCTRL_reg(const void *const hw)
{
	return ((I2s *)hw)->TXCTRL.reg;
}

static inline void hri_i2s_set_RXCTRL_CLKSEL_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_CLKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_CLKSEL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_CLKSEL) >> I2S_RXCTRL_CLKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_CLKSEL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_CLKSEL;
	tmp |= value << I2S_RXCTRL_CLKSEL_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_CLKSEL_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_CLKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_CLKSEL_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_CLKSEL;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTADJ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTADJ) >> I2S_RXCTRL_SLOTADJ_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTADJ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTADJ;
	tmp |= value << I2S_RXCTRL_SLOTADJ_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_WORDADJ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_WORDADJ) >> I2S_RXCTRL_WORDADJ_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_WORDADJ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_WORDADJ;
	tmp |= value << I2S_RXCTRL_WORDADJ_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_WORDADJ_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_WORDADJ;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_BITREV_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_BITREV) >> I2S_RXCTRL_BITREV_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_BITREV_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_BITREV;
	tmp |= value << I2S_RXCTRL_BITREV_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_BITREV_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_BITREV;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS0) >> I2S_RXCTRL_SLOTDIS0_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS0;
	tmp |= value << I2S_RXCTRL_SLOTDIS0_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS0_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS0;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS1) >> I2S_RXCTRL_SLOTDIS1_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS1;
	tmp |= value << I2S_RXCTRL_SLOTDIS1_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS1_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS1;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS2) >> I2S_RXCTRL_SLOTDIS2_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS2;
	tmp |= value << I2S_RXCTRL_SLOTDIS2_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS2_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS2;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS3) >> I2S_RXCTRL_SLOTDIS3_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS3;
	tmp |= value << I2S_RXCTRL_SLOTDIS3_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS3_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS3;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS4) >> I2S_RXCTRL_SLOTDIS4_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS4;
	tmp |= value << I2S_RXCTRL_SLOTDIS4_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS4_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS4;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS5) >> I2S_RXCTRL_SLOTDIS5_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS5;
	tmp |= value << I2S_RXCTRL_SLOTDIS5_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS5_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS5;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS6) >> I2S_RXCTRL_SLOTDIS6_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS6;
	tmp |= value << I2S_RXCTRL_SLOTDIS6_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS6_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS6;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_SLOTDIS7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SLOTDIS7) >> I2S_RXCTRL_SLOTDIS7_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_SLOTDIS7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SLOTDIS7;
	tmp |= value << I2S_RXCTRL_SLOTDIS7_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SLOTDIS7_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SLOTDIS7;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_MONO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_MONO) >> I2S_RXCTRL_MONO_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_MONO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_MONO;
	tmp |= value << I2S_RXCTRL_MONO_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_MONO_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_MONO;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_DMA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_DMA) >> I2S_RXCTRL_DMA_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_DMA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_DMA;
	tmp |= value << I2S_RXCTRL_DMA_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_DMA_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_DMA;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_RXLOOP_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_RXLOOP;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_i2s_get_RXCTRL_RXLOOP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_RXLOOP) >> I2S_RXCTRL_RXLOOP_Pos;
	return (bool)tmp;
}

static inline void hri_i2s_write_RXCTRL_RXLOOP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_RXLOOP;
	tmp |= value << I2S_RXCTRL_RXLOOP_Pos;
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_RXLOOP_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_RXLOOP;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_RXLOOP_bit(const void *const hw)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_RXLOOP;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_set_RXCTRL_SERMODE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_SERMODE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_get_RXCTRL_SERMODE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SERMODE(mask)) >> I2S_RXCTRL_SERMODE_Pos;
	return tmp;
}

static inline void hri_i2s_write_RXCTRL_SERMODE_bf(const void *const hw, hri_i2s_rxctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_SERMODE_Msk;
	tmp |= I2S_RXCTRL_SERMODE(data);
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_SERMODE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_SERMODE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_SERMODE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_SERMODE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_read_RXCTRL_SERMODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_SERMODE_Msk) >> I2S_RXCTRL_SERMODE_Pos;
	return tmp;
}

static inline void hri_i2s_set_RXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_get_RXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_DATASIZE(mask)) >> I2S_RXCTRL_DATASIZE_Pos;
	return tmp;
}

static inline void hri_i2s_write_RXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_rxctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_DATASIZE_Msk;
	tmp |= I2S_RXCTRL_DATASIZE(data);
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_DATASIZE_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_DATASIZE(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_read_RXCTRL_DATASIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_DATASIZE_Msk) >> I2S_RXCTRL_DATASIZE_Pos;
	return tmp;
}

static inline void hri_i2s_set_RXCTRL_EXTEND_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= I2S_RXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_get_RXCTRL_EXTEND_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_EXTEND(mask)) >> I2S_RXCTRL_EXTEND_Pos;
	return tmp;
}

static inline void hri_i2s_write_RXCTRL_EXTEND_bf(const void *const hw, hri_i2s_rxctrl_reg_t data)
{
	uint32_t tmp;
	I2S_CRITICAL_SECTION_ENTER();
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= ~I2S_RXCTRL_EXTEND_Msk;
	tmp |= I2S_RXCTRL_EXTEND(data);
	((I2s *)hw)->RXCTRL.reg = tmp;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_EXTEND_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~I2S_RXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_EXTEND_bf(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= I2S_RXCTRL_EXTEND(mask);
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_read_RXCTRL_EXTEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp = (tmp & I2S_RXCTRL_EXTEND_Msk) >> I2S_RXCTRL_EXTEND_Pos;
	return tmp;
}

static inline void hri_i2s_set_RXCTRL_reg(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg |= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_get_RXCTRL_reg(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_i2s_write_RXCTRL_reg(const void *const hw, hri_i2s_rxctrl_reg_t data)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg = data;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_clear_RXCTRL_reg(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg &= ~mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline void hri_i2s_toggle_RXCTRL_reg(const void *const hw, hri_i2s_rxctrl_reg_t mask)
{
	I2S_CRITICAL_SECTION_ENTER();
	((I2s *)hw)->RXCTRL.reg ^= mask;
	I2S_CRITICAL_SECTION_LEAVE();
}

static inline hri_i2s_rxctrl_reg_t hri_i2s_read_RXCTRL_reg(const void *const hw)
{
	return ((I2s *)hw)->RXCTRL.reg;
}

static inline bool hri_i2s_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_SWRST) >> I2S_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) >> I2S_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_CKEN0_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_CKEN0) >> I2S_SYNCBUSY_CKEN0_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_CKEN1_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_CKEN1) >> I2S_SYNCBUSY_CKEN1_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_TXEN_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_TXEN) >> I2S_SYNCBUSY_TXEN_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_RXEN_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_RXEN) >> I2S_SYNCBUSY_RXEN_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_TXDATA_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_TXDATA) >> I2S_SYNCBUSY_TXDATA_Pos;
}

static inline bool hri_i2s_get_SYNCBUSY_RXDATA_bit(const void *const hw)
{
	return (((I2s *)hw)->SYNCBUSY.reg & I2S_SYNCBUSY_RXDATA) >> I2S_SYNCBUSY_RXDATA_Pos;
}

static inline hri_i2s_syncbusy_reg_t hri_i2s_get_SYNCBUSY_reg(const void *const hw, hri_i2s_syncbusy_reg_t mask)
{
	uint16_t tmp;
	tmp = ((I2s *)hw)->SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_i2s_syncbusy_reg_t hri_i2s_read_SYNCBUSY_reg(const void *const hw)
{
	return ((I2s *)hw)->SYNCBUSY.reg;
}

static inline hri_i2s_rxdata_reg_t hri_i2s_get_RXDATA_DATA_bf(const void *const hw, hri_i2s_rxdata_reg_t mask)
{
	hri_i2s_wait_for_sync(hw, I2S_SYNCBUSY_MASK);
	return (((I2s *)hw)->RXDATA.reg & I2S_RXDATA_DATA(mask)) >> I2S_RXDATA_DATA_Pos;
}

static inline hri_i2s_rxdata_reg_t hri_i2s_read_RXDATA_DATA_bf(const void *const hw)
{
	hri_i2s_wait_for_sync(hw, I2S_SYNCBUSY_MASK);
	return (((I2s *)hw)->RXDATA.reg & I2S_RXDATA_DATA_Msk) >> I2S_RXDATA_DATA_Pos;
}

static inline hri_i2s_rxdata_reg_t hri_i2s_get_RXDATA_reg(const void *const hw, hri_i2s_rxdata_reg_t mask)
{
	uint32_t tmp;
	tmp = ((I2s *)hw)->RXDATA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_i2s_rxdata_reg_t hri_i2s_read_RXDATA_reg(const void *const hw)
{
	return ((I2s *)hw)->RXDATA.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_I2S_D51_H_INCLUDED */
#endif /* _SAMD51_I2S_COMPONENT_ */
