/**
 * \file
 *
 * \brief SAM SERCOM
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

#ifdef _SAMD51_SERCOM_COMPONENT_
#ifndef _HRI_SERCOM_D51_H_INCLUDED_
#define _HRI_SERCOM_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SERCOM_CRITICAL_SECTIONS)
#define SERCOM_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SERCOM_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SERCOM_CRITICAL_SECTION_ENTER()
#define SERCOM_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_sercomi2cm_status_reg_t;
typedef uint16_t hri_sercomi2cs_length_reg_t;
typedef uint16_t hri_sercomi2cs_status_reg_t;
typedef uint16_t hri_sercomspi_length_reg_t;
typedef uint16_t hri_sercomspi_status_reg_t;
typedef uint16_t hri_sercomusart_baud_reg_t;
typedef uint16_t hri_sercomusart_length_reg_t;
typedef uint16_t hri_sercomusart_status_reg_t;
typedef uint32_t hri_sercomi2cm_addr_reg_t;
typedef uint32_t hri_sercomi2cm_baud_reg_t;
typedef uint32_t hri_sercomi2cm_ctrla_reg_t;
typedef uint32_t hri_sercomi2cm_ctrlb_reg_t;
typedef uint32_t hri_sercomi2cm_ctrlc_reg_t;
typedef uint32_t hri_sercomi2cm_data_reg_t;
typedef uint32_t hri_sercomi2cm_syncbusy_reg_t;
typedef uint32_t hri_sercomi2cs_addr_reg_t;
typedef uint32_t hri_sercomi2cs_ctrla_reg_t;
typedef uint32_t hri_sercomi2cs_ctrlb_reg_t;
typedef uint32_t hri_sercomi2cs_ctrlc_reg_t;
typedef uint32_t hri_sercomi2cs_data_reg_t;
typedef uint32_t hri_sercomi2cs_syncbusy_reg_t;
typedef uint32_t hri_sercomspi_addr_reg_t;
typedef uint32_t hri_sercomspi_ctrla_reg_t;
typedef uint32_t hri_sercomspi_ctrlb_reg_t;
typedef uint32_t hri_sercomspi_ctrlc_reg_t;
typedef uint32_t hri_sercomspi_data_reg_t;
typedef uint32_t hri_sercomspi_syncbusy_reg_t;
typedef uint32_t hri_sercomusart_ctrla_reg_t;
typedef uint32_t hri_sercomusart_ctrlb_reg_t;
typedef uint32_t hri_sercomusart_ctrlc_reg_t;
typedef uint32_t hri_sercomusart_data_reg_t;
typedef uint32_t hri_sercomusart_syncbusy_reg_t;
typedef uint8_t  hri_sercomi2cm_dbgctrl_reg_t;
typedef uint8_t  hri_sercomi2cm_intenset_reg_t;
typedef uint8_t  hri_sercomi2cm_intflag_reg_t;
typedef uint8_t  hri_sercomi2cs_intenset_reg_t;
typedef uint8_t  hri_sercomi2cs_intflag_reg_t;
typedef uint8_t  hri_sercomspi_baud_reg_t;
typedef uint8_t  hri_sercomspi_dbgctrl_reg_t;
typedef uint8_t  hri_sercomspi_intenset_reg_t;
typedef uint8_t  hri_sercomspi_intflag_reg_t;
typedef uint8_t  hri_sercomusart_dbgctrl_reg_t;
typedef uint8_t  hri_sercomusart_intenset_reg_t;
typedef uint8_t  hri_sercomusart_intflag_reg_t;
typedef uint8_t  hri_sercomusart_rxerrcnt_reg_t;
typedef uint8_t  hri_sercomusart_rxpl_reg_t;

static inline void hri_sercomi2cm_wait_for_sync(const void *const hw, hri_sercomi2cm_syncbusy_reg_t reg)
{
	while (((Sercom *)hw)->I2CM.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_sercomi2cm_is_syncing(const void *const hw, hri_sercomi2cm_syncbusy_reg_t reg)
{
	return ((Sercom *)hw)->I2CM.SYNCBUSY.reg & reg;
}

static inline void hri_sercomi2cs_wait_for_sync(const void *const hw, hri_sercomi2cs_syncbusy_reg_t reg)
{
	while (((Sercom *)hw)->I2CS.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_sercomi2cs_is_syncing(const void *const hw, hri_sercomi2cs_syncbusy_reg_t reg)
{
	return ((Sercom *)hw)->I2CS.SYNCBUSY.reg & reg;
}

static inline void hri_sercomspi_wait_for_sync(const void *const hw, hri_sercomspi_syncbusy_reg_t reg)
{
	while (((Sercom *)hw)->SPI.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_sercomspi_is_syncing(const void *const hw, hri_sercomspi_syncbusy_reg_t reg)
{
	return ((Sercom *)hw)->SPI.SYNCBUSY.reg & reg;
}

static inline void hri_sercomusart_wait_for_sync(const void *const hw, hri_sercomusart_syncbusy_reg_t reg)
{
	while (((Sercom *)hw)->USART.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_sercomusart_is_syncing(const void *const hw, hri_sercomusart_syncbusy_reg_t reg)
{
	return ((Sercom *)hw)->USART.SYNCBUSY.reg & reg;
}

static inline void hri_sercomspi_set_INTEN_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_DRE;
}

static inline bool hri_sercomspi_get_INTEN_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTENSET.reg & SERCOM_SPI_INTENSET_DRE) >> SERCOM_SPI_INTENSET_DRE_Pos;
}

static inline void hri_sercomspi_write_INTEN_DRE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_DRE;
	} else {
		((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_DRE;
	}
}

static inline void hri_sercomspi_clear_INTEN_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_DRE;
}

static inline void hri_sercomspi_set_INTEN_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_TXC;
}

static inline bool hri_sercomspi_get_INTEN_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTENSET.reg & SERCOM_SPI_INTENSET_TXC) >> SERCOM_SPI_INTENSET_TXC_Pos;
}

static inline void hri_sercomspi_write_INTEN_TXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_TXC;
	} else {
		((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_TXC;
	}
}

static inline void hri_sercomspi_clear_INTEN_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_TXC;
}

static inline void hri_sercomspi_set_INTEN_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_RXC;
}

static inline bool hri_sercomspi_get_INTEN_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTENSET.reg & SERCOM_SPI_INTENSET_RXC) >> SERCOM_SPI_INTENSET_RXC_Pos;
}

static inline void hri_sercomspi_write_INTEN_RXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_RXC;
	} else {
		((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_RXC;
	}
}

static inline void hri_sercomspi_clear_INTEN_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_RXC;
}

static inline void hri_sercomspi_set_INTEN_SSL_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_SSL;
}

static inline bool hri_sercomspi_get_INTEN_SSL_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTENSET.reg & SERCOM_SPI_INTENSET_SSL) >> SERCOM_SPI_INTENSET_SSL_Pos;
}

static inline void hri_sercomspi_write_INTEN_SSL_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_SSL;
	} else {
		((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_SSL;
	}
}

static inline void hri_sercomspi_clear_INTEN_SSL_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_SSL;
}

static inline void hri_sercomspi_set_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_ERROR;
}

static inline bool hri_sercomspi_get_INTEN_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTENSET.reg & SERCOM_SPI_INTENSET_ERROR) >> SERCOM_SPI_INTENSET_ERROR_Pos;
}

static inline void hri_sercomspi_write_INTEN_ERROR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_ERROR;
	} else {
		((Sercom *)hw)->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_ERROR;
	}
}

static inline void hri_sercomspi_clear_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = SERCOM_SPI_INTENSET_ERROR;
}

static inline void hri_sercomspi_set_INTEN_reg(const void *const hw, hri_sercomspi_intenset_reg_t mask)
{
	((Sercom *)hw)->SPI.INTENSET.reg = mask;
}

static inline hri_sercomspi_intenset_reg_t hri_sercomspi_get_INTEN_reg(const void *const            hw,
                                                                       hri_sercomspi_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomspi_intenset_reg_t hri_sercomspi_read_INTEN_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.INTENSET.reg;
}

static inline void hri_sercomspi_write_INTEN_reg(const void *const hw, hri_sercomspi_intenset_reg_t data)
{
	((Sercom *)hw)->SPI.INTENSET.reg = data;
	((Sercom *)hw)->SPI.INTENCLR.reg = ~data;
}

static inline void hri_sercomspi_clear_INTEN_reg(const void *const hw, hri_sercomspi_intenset_reg_t mask)
{
	((Sercom *)hw)->SPI.INTENCLR.reg = mask;
}

static inline bool hri_sercomspi_get_INTFLAG_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE) >> SERCOM_SPI_INTFLAG_DRE_Pos;
}

static inline void hri_sercomspi_clear_INTFLAG_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_DRE;
}

static inline bool hri_sercomspi_get_INTFLAG_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) >> SERCOM_SPI_INTFLAG_TXC_Pos;
}

static inline void hri_sercomspi_clear_INTFLAG_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_TXC;
}

static inline bool hri_sercomspi_get_INTFLAG_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC) >> SERCOM_SPI_INTFLAG_RXC_Pos;
}

static inline void hri_sercomspi_clear_INTFLAG_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_RXC;
}

static inline bool hri_sercomspi_get_INTFLAG_SSL_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_SSL) >> SERCOM_SPI_INTFLAG_SSL_Pos;
}

static inline void hri_sercomspi_clear_INTFLAG_SSL_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_SSL;
}

static inline bool hri_sercomspi_get_INTFLAG_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_ERROR) >> SERCOM_SPI_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomspi_clear_INTFLAG_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_ERROR;
}

static inline bool hri_sercomspi_get_interrupt_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE) >> SERCOM_SPI_INTFLAG_DRE_Pos;
}

static inline void hri_sercomspi_clear_interrupt_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_DRE;
}

static inline bool hri_sercomspi_get_interrupt_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) >> SERCOM_SPI_INTFLAG_TXC_Pos;
}

static inline void hri_sercomspi_clear_interrupt_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_TXC;
}

static inline bool hri_sercomspi_get_interrupt_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC) >> SERCOM_SPI_INTFLAG_RXC_Pos;
}

static inline void hri_sercomspi_clear_interrupt_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_RXC;
}

static inline bool hri_sercomspi_get_interrupt_SSL_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_SSL) >> SERCOM_SPI_INTFLAG_SSL_Pos;
}

static inline void hri_sercomspi_clear_interrupt_SSL_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_SSL;
}

static inline bool hri_sercomspi_get_interrupt_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_ERROR) >> SERCOM_SPI_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomspi_clear_interrupt_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_ERROR;
}

static inline hri_sercomspi_intflag_reg_t hri_sercomspi_get_INTFLAG_reg(const void *const           hw,
                                                                        hri_sercomspi_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomspi_intflag_reg_t hri_sercomspi_read_INTFLAG_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.INTFLAG.reg;
}

static inline void hri_sercomspi_clear_INTFLAG_reg(const void *const hw, hri_sercomspi_intflag_reg_t mask)
{
	((Sercom *)hw)->SPI.INTFLAG.reg = mask;
}

static inline void hri_sercomspi_set_CTRLA_SWRST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_SWRST) >> SERCOM_SPI_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_set_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_ENABLE) >> SERCOM_SPI_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_ENABLE;
	tmp |= value << SERCOM_SPI_CTRLA_ENABLE_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_RUNSTDBY) >> SERCOM_SPI_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_RUNSTDBY;
	tmp |= value << SERCOM_SPI_CTRLA_RUNSTDBY_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_IBON_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_IBON) >> SERCOM_SPI_CTRLA_IBON_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_IBON_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_IBON;
	tmp |= value << SERCOM_SPI_CTRLA_IBON_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_CPHA_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_CPHA;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_CPHA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_CPHA) >> SERCOM_SPI_CTRLA_CPHA_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_CPHA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_CPHA;
	tmp |= value << SERCOM_SPI_CTRLA_CPHA_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_CPHA_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_CPHA;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_CPHA_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_CPHA;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_CPOL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_CPOL) >> SERCOM_SPI_CTRLA_CPOL_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_CPOL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_CPOL;
	tmp |= value << SERCOM_SPI_CTRLA_CPOL_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLA_DORD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_DORD) >> SERCOM_SPI_CTRLA_DORD_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLA_DORD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_DORD;
	tmp |= value << SERCOM_SPI_CTRLA_DORD_Pos;
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLA_MODE_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_get_CTRLA_MODE_bf(const void *const         hw,
                                                                        hri_sercomspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_MODE(mask)) >> SERCOM_SPI_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLA_MODE_bf(const void *const hw, hri_sercomspi_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_MODE_Msk;
	tmp |= SERCOM_SPI_CTRLA_MODE(data);
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_MODE_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_MODE_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_MODE_Msk) >> SERCOM_SPI_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLA_DOPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_DOPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_get_CTRLA_DOPO_bf(const void *const         hw,
                                                                        hri_sercomspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_DOPO(mask)) >> SERCOM_SPI_CTRLA_DOPO_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLA_DOPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_DOPO_Msk;
	tmp |= SERCOM_SPI_CTRLA_DOPO(data);
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_DOPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_DOPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_DOPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_DOPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_read_CTRLA_DOPO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_DOPO_Msk) >> SERCOM_SPI_CTRLA_DOPO_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLA_DIPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_DIPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_get_CTRLA_DIPO_bf(const void *const         hw,
                                                                        hri_sercomspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_DIPO(mask)) >> SERCOM_SPI_CTRLA_DIPO_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLA_DIPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_DIPO_Msk;
	tmp |= SERCOM_SPI_CTRLA_DIPO(data);
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_DIPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_DIPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_DIPO_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_DIPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_read_CTRLA_DIPO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_DIPO_Msk) >> SERCOM_SPI_CTRLA_DIPO_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLA_FORM_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_get_CTRLA_FORM_bf(const void *const         hw,
                                                                        hri_sercomspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_FORM(mask)) >> SERCOM_SPI_CTRLA_FORM_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLA_FORM_bf(const void *const hw, hri_sercomspi_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= ~SERCOM_SPI_CTRLA_FORM_Msk;
	tmp |= SERCOM_SPI_CTRLA_FORM(data);
	((Sercom *)hw)->SPI.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_FORM_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_FORM_bf(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);
	((Sercom *)hw)->SPI.CTRLA.reg ^= SERCOM_SPI_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_read_CTRLA_FORM_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp = (tmp & SERCOM_SPI_CTRLA_FORM_Msk) >> SERCOM_SPI_CTRLA_FORM_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLA_reg(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLA.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_get_CTRLA_reg(const void *const         hw,
                                                                    hri_sercomspi_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLA_reg(const void *const hw, hri_sercomspi_ctrla_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLA_reg(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLA.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLA_reg(const void *const hw, hri_sercomspi_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLA.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrla_reg_t hri_sercomspi_read_CTRLA_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.CTRLA.reg;
}

static inline void hri_sercomspi_set_CTRLB_PLOADEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_PLOADEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLB_PLOADEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_PLOADEN) >> SERCOM_SPI_CTRLB_PLOADEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLB_PLOADEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_PLOADEN;
	tmp |= value << SERCOM_SPI_CTRLB_PLOADEN_Pos;
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_PLOADEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_PLOADEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_PLOADEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_PLOADEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLB_SSDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_SSDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLB_SSDE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_SSDE) >> SERCOM_SPI_CTRLB_SSDE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLB_SSDE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_SSDE;
	tmp |= value << SERCOM_SPI_CTRLB_SSDE_Pos;
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_SSDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_SSDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_SSDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_SSDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLB_MSSEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_MSSEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLB_MSSEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_MSSEN) >> SERCOM_SPI_CTRLB_MSSEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLB_MSSEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_MSSEN;
	tmp |= value << SERCOM_SPI_CTRLB_MSSEN_Pos;
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_MSSEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_MSSEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_MSSEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_MSSEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLB_RXEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_RXEN) >> SERCOM_SPI_CTRLB_RXEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLB_RXEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_RXEN;
	tmp |= value << SERCOM_SPI_CTRLB_RXEN_Pos;
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_get_CTRLB_CHSIZE_bf(const void *const         hw,
                                                                          hri_sercomspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_CHSIZE(mask)) >> SERCOM_SPI_CTRLB_CHSIZE_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_CHSIZE_Msk;
	tmp |= SERCOM_SPI_CTRLB_CHSIZE(data);
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_read_CTRLB_CHSIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_CHSIZE_Msk) >> SERCOM_SPI_CTRLB_CHSIZE_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLB_AMODE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= SERCOM_SPI_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_get_CTRLB_AMODE_bf(const void *const         hw,
                                                                         hri_sercomspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_AMODE(mask)) >> SERCOM_SPI_CTRLB_AMODE_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLB_AMODE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= ~SERCOM_SPI_CTRLB_AMODE_Msk;
	tmp |= SERCOM_SPI_CTRLB_AMODE(data);
	((Sercom *)hw)->SPI.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_AMODE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~SERCOM_SPI_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_AMODE_bf(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= SERCOM_SPI_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_read_CTRLB_AMODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp = (tmp & SERCOM_SPI_CTRLB_AMODE_Msk) >> SERCOM_SPI_CTRLB_AMODE_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLB_reg(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_get_CTRLB_reg(const void *const         hw,
                                                                    hri_sercomspi_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLB_reg(const void *const hw, hri_sercomspi_ctrlb_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLB_reg(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLB_reg(const void *const hw, hri_sercomspi_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLB.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlb_reg_t hri_sercomspi_read_CTRLB_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.CTRLB.reg;
}

static inline void hri_sercomspi_set_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg |= SERCOM_SPI_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_CTRLC_DATA32B_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp = (tmp & SERCOM_SPI_CTRLC_DATA32B) >> SERCOM_SPI_CTRLC_DATA32B_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_CTRLC_DATA32B_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp &= ~SERCOM_SPI_CTRLC_DATA32B;
	tmp |= value << SERCOM_SPI_CTRLC_DATA32B_Pos;
	((Sercom *)hw)->SPI.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg &= ~SERCOM_SPI_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg ^= SERCOM_SPI_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_CTRLC_ICSPACE_bf(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg |= SERCOM_SPI_CTRLC_ICSPACE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlc_reg_t hri_sercomspi_get_CTRLC_ICSPACE_bf(const void *const         hw,
                                                                           hri_sercomspi_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp = (tmp & SERCOM_SPI_CTRLC_ICSPACE(mask)) >> SERCOM_SPI_CTRLC_ICSPACE_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLC_ICSPACE_bf(const void *const hw, hri_sercomspi_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp &= ~SERCOM_SPI_CTRLC_ICSPACE_Msk;
	tmp |= SERCOM_SPI_CTRLC_ICSPACE(data);
	((Sercom *)hw)->SPI.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLC_ICSPACE_bf(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg &= ~SERCOM_SPI_CTRLC_ICSPACE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLC_ICSPACE_bf(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg ^= SERCOM_SPI_CTRLC_ICSPACE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlc_reg_t hri_sercomspi_read_CTRLC_ICSPACE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp = (tmp & SERCOM_SPI_CTRLC_ICSPACE_Msk) >> SERCOM_SPI_CTRLC_ICSPACE_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_CTRLC_reg(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlc_reg_t hri_sercomspi_get_CTRLC_reg(const void *const         hw,
                                                                    hri_sercomspi_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.CTRLC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_CTRLC_reg(const void *const hw, hri_sercomspi_ctrlc_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_CTRLC_reg(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_CTRLC_reg(const void *const hw, hri_sercomspi_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.CTRLC.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_ctrlc_reg_t hri_sercomspi_read_CTRLC_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.CTRLC.reg;
}

static inline void hri_sercomspi_set_BAUD_BAUD_bf(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg |= SERCOM_SPI_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_baud_reg_t hri_sercomspi_get_BAUD_BAUD_bf(const void *const        hw,
                                                                      hri_sercomspi_baud_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.BAUD.reg;
	tmp = (tmp & SERCOM_SPI_BAUD_BAUD(mask)) >> SERCOM_SPI_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_BAUD_BAUD_bf(const void *const hw, hri_sercomspi_baud_reg_t data)
{
	uint8_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.BAUD.reg;
	tmp &= ~SERCOM_SPI_BAUD_BAUD_Msk;
	tmp |= SERCOM_SPI_BAUD_BAUD(data);
	((Sercom *)hw)->SPI.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_BAUD_BAUD_bf(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg &= ~SERCOM_SPI_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_BAUD_BAUD_bf(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg ^= SERCOM_SPI_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_baud_reg_t hri_sercomspi_read_BAUD_BAUD_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.BAUD.reg;
	tmp = (tmp & SERCOM_SPI_BAUD_BAUD_Msk) >> SERCOM_SPI_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_BAUD_reg(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_baud_reg_t hri_sercomspi_get_BAUD_reg(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_BAUD_reg(const void *const hw, hri_sercomspi_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_BAUD_reg(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_BAUD_reg(const void *const hw, hri_sercomspi_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_baud_reg_t hri_sercomspi_read_BAUD_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.BAUD.reg;
}

static inline void hri_sercomspi_set_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg |= SERCOM_SPI_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_LENGTH_LENEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp = (tmp & SERCOM_SPI_LENGTH_LENEN) >> SERCOM_SPI_LENGTH_LENEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_LENGTH_LENEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp &= ~SERCOM_SPI_LENGTH_LENEN;
	tmp |= value << SERCOM_SPI_LENGTH_LENEN_Pos;
	((Sercom *)hw)->SPI.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg &= ~SERCOM_SPI_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg ^= SERCOM_SPI_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_LENGTH_LEN_bf(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg |= SERCOM_SPI_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_length_reg_t hri_sercomspi_get_LENGTH_LEN_bf(const void *const          hw,
                                                                         hri_sercomspi_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp = (tmp & SERCOM_SPI_LENGTH_LEN(mask)) >> SERCOM_SPI_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_LENGTH_LEN_bf(const void *const hw, hri_sercomspi_length_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp &= ~SERCOM_SPI_LENGTH_LEN_Msk;
	tmp |= SERCOM_SPI_LENGTH_LEN(data);
	((Sercom *)hw)->SPI.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_LENGTH_LEN_bf(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg &= ~SERCOM_SPI_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_LENGTH_LEN_bf(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg ^= SERCOM_SPI_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_length_reg_t hri_sercomspi_read_LENGTH_LEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp = (tmp & SERCOM_SPI_LENGTH_LEN_Msk) >> SERCOM_SPI_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_LENGTH_reg(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_length_reg_t hri_sercomspi_get_LENGTH_reg(const void *const          hw,
                                                                      hri_sercomspi_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->SPI.LENGTH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_LENGTH_reg(const void *const hw, hri_sercomspi_length_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_LENGTH_reg(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_LENGTH_reg(const void *const hw, hri_sercomspi_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.LENGTH.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_length_reg_t hri_sercomspi_read_LENGTH_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.LENGTH.reg;
}

static inline void hri_sercomspi_set_ADDR_ADDR_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg |= SERCOM_SPI_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_get_ADDR_ADDR_bf(const void *const        hw,
                                                                      hri_sercomspi_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp = (tmp & SERCOM_SPI_ADDR_ADDR(mask)) >> SERCOM_SPI_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_ADDR_ADDR_bf(const void *const hw, hri_sercomspi_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp &= ~SERCOM_SPI_ADDR_ADDR_Msk;
	tmp |= SERCOM_SPI_ADDR_ADDR(data);
	((Sercom *)hw)->SPI.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_ADDR_ADDR_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg &= ~SERCOM_SPI_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_ADDR_ADDR_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg ^= SERCOM_SPI_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp = (tmp & SERCOM_SPI_ADDR_ADDR_Msk) >> SERCOM_SPI_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg |= SERCOM_SPI_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_get_ADDR_ADDRMASK_bf(const void *const        hw,
                                                                          hri_sercomspi_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp = (tmp & SERCOM_SPI_ADDR_ADDRMASK(mask)) >> SERCOM_SPI_ADDR_ADDRMASK_Pos;
	return tmp;
}

static inline void hri_sercomspi_write_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomspi_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp &= ~SERCOM_SPI_ADDR_ADDRMASK_Msk;
	tmp |= SERCOM_SPI_ADDR_ADDRMASK(data);
	((Sercom *)hw)->SPI.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg &= ~SERCOM_SPI_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg ^= SERCOM_SPI_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_read_ADDR_ADDRMASK_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp = (tmp & SERCOM_SPI_ADDR_ADDRMASK_Msk) >> SERCOM_SPI_ADDR_ADDRMASK_Pos;
	return tmp;
}

static inline void hri_sercomspi_set_ADDR_reg(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_get_ADDR_reg(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_ADDR_reg(const void *const hw, hri_sercomspi_addr_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_ADDR_reg(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_ADDR_reg(const void *const hw, hri_sercomspi_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.ADDR.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_addr_reg_t hri_sercomspi_read_ADDR_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.ADDR.reg;
}

static inline hri_sercomspi_data_reg_t hri_sercomspi_read_DATA_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.DATA.reg;
}

static inline void hri_sercomspi_write_DATA_reg(const void *const hw, hri_sercomspi_data_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DATA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg |= SERCOM_SPI_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.DBGCTRL.reg;
	tmp = (tmp & SERCOM_SPI_DBGCTRL_DBGSTOP) >> SERCOM_SPI_DBGCTRL_DBGSTOP_Pos;
	return (bool)tmp;
}

static inline void hri_sercomspi_write_DBGCTRL_DBGSTOP_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->SPI.DBGCTRL.reg;
	tmp &= ~SERCOM_SPI_DBGCTRL_DBGSTOP;
	tmp |= value << SERCOM_SPI_DBGCTRL_DBGSTOP_Pos;
	((Sercom *)hw)->SPI.DBGCTRL.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg &= ~SERCOM_SPI_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg ^= SERCOM_SPI_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_set_DBGCTRL_reg(const void *const hw, hri_sercomspi_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_dbgctrl_reg_t hri_sercomspi_get_DBGCTRL_reg(const void *const           hw,
                                                                        hri_sercomspi_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->SPI.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_write_DBGCTRL_reg(const void *const hw, hri_sercomspi_dbgctrl_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_clear_DBGCTRL_reg(const void *const hw, hri_sercomspi_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomspi_toggle_DBGCTRL_reg(const void *const hw, hri_sercomspi_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.DBGCTRL.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_dbgctrl_reg_t hri_sercomspi_read_DBGCTRL_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.DBGCTRL.reg;
}

static inline bool hri_sercomspi_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_SWRST) >> SERCOM_SPI_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_sercomspi_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) >> SERCOM_SPI_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_sercomspi_get_SYNCBUSY_CTRLB_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_CTRLB) >> SERCOM_SPI_SYNCBUSY_CTRLB_Pos;
}

static inline bool hri_sercomspi_get_SYNCBUSY_LENGTH_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_LENGTH) >> SERCOM_SPI_SYNCBUSY_LENGTH_Pos;
}

static inline hri_sercomspi_syncbusy_reg_t hri_sercomspi_get_SYNCBUSY_reg(const void *const            hw,
                                                                          hri_sercomspi_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->SPI.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomspi_syncbusy_reg_t hri_sercomspi_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.SYNCBUSY.reg;
}

static inline bool hri_sercomspi_get_STATUS_BUFOVF_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.STATUS.reg & SERCOM_SPI_STATUS_BUFOVF) >> SERCOM_SPI_STATUS_BUFOVF_Pos;
}

static inline void hri_sercomspi_clear_STATUS_BUFOVF_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.STATUS.reg = SERCOM_SPI_STATUS_BUFOVF;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomspi_get_STATUS_LENERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->SPI.STATUS.reg & SERCOM_SPI_STATUS_LENERR) >> SERCOM_SPI_STATUS_LENERR_Pos;
}

static inline void hri_sercomspi_clear_STATUS_LENERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.STATUS.reg = SERCOM_SPI_STATUS_LENERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_status_reg_t hri_sercomspi_get_STATUS_reg(const void *const          hw,
                                                                      hri_sercomspi_status_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->SPI.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomspi_clear_STATUS_reg(const void *const hw, hri_sercomspi_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->SPI.STATUS.reg = mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomspi_status_reg_t hri_sercomspi_read_STATUS_reg(const void *const hw)
{
	return ((Sercom *)hw)->SPI.STATUS.reg;
}

static inline void hri_sercomi2cm_set_INTEN_MB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_MB;
}

static inline bool hri_sercomi2cm_get_INTEN_MB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTENSET.reg & SERCOM_I2CM_INTENSET_MB) >> SERCOM_I2CM_INTENSET_MB_Pos;
}

static inline void hri_sercomi2cm_write_INTEN_MB_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_MB;
	} else {
		((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_MB;
	}
}

static inline void hri_sercomi2cm_clear_INTEN_MB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_MB;
}

static inline void hri_sercomi2cm_set_INTEN_SB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_SB;
}

static inline bool hri_sercomi2cm_get_INTEN_SB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTENSET.reg & SERCOM_I2CM_INTENSET_SB) >> SERCOM_I2CM_INTENSET_SB_Pos;
}

static inline void hri_sercomi2cm_write_INTEN_SB_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_SB;
	} else {
		((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_SB;
	}
}

static inline void hri_sercomi2cm_clear_INTEN_SB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_SB;
}

static inline void hri_sercomi2cm_set_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_ERROR;
}

static inline bool hri_sercomi2cm_get_INTEN_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTENSET.reg & SERCOM_I2CM_INTENSET_ERROR) >> SERCOM_I2CM_INTENSET_ERROR_Pos;
}

static inline void hri_sercomi2cm_write_INTEN_ERROR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_ERROR;
	} else {
		((Sercom *)hw)->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_ERROR;
	}
}

static inline void hri_sercomi2cm_clear_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_ERROR;
}

static inline void hri_sercomi2cm_set_INTEN_reg(const void *const hw, hri_sercomi2cm_intenset_reg_t mask)
{
	((Sercom *)hw)->I2CM.INTENSET.reg = mask;
}

static inline hri_sercomi2cm_intenset_reg_t hri_sercomi2cm_get_INTEN_reg(const void *const             hw,
                                                                         hri_sercomi2cm_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CM.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cm_intenset_reg_t hri_sercomi2cm_read_INTEN_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.INTENSET.reg;
}

static inline void hri_sercomi2cm_write_INTEN_reg(const void *const hw, hri_sercomi2cm_intenset_reg_t data)
{
	((Sercom *)hw)->I2CM.INTENSET.reg = data;
	((Sercom *)hw)->I2CM.INTENCLR.reg = ~data;
}

static inline void hri_sercomi2cm_clear_INTEN_reg(const void *const hw, hri_sercomi2cm_intenset_reg_t mask)
{
	((Sercom *)hw)->I2CM.INTENCLR.reg = mask;
}

static inline bool hri_sercomi2cm_get_INTFLAG_MB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) >> SERCOM_I2CM_INTFLAG_MB_Pos;
}

static inline void hri_sercomi2cm_clear_INTFLAG_MB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MB;
}

static inline bool hri_sercomi2cm_get_INTFLAG_SB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) >> SERCOM_I2CM_INTFLAG_SB_Pos;
}

static inline void hri_sercomi2cm_clear_INTFLAG_SB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
}

static inline bool hri_sercomi2cm_get_INTFLAG_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_ERROR) >> SERCOM_I2CM_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomi2cm_clear_INTFLAG_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_ERROR;
}

static inline bool hri_sercomi2cm_get_interrupt_MB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) >> SERCOM_I2CM_INTFLAG_MB_Pos;
}

static inline void hri_sercomi2cm_clear_interrupt_MB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MB;
}

static inline bool hri_sercomi2cm_get_interrupt_SB_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) >> SERCOM_I2CM_INTFLAG_SB_Pos;
}

static inline void hri_sercomi2cm_clear_interrupt_SB_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
}

static inline bool hri_sercomi2cm_get_interrupt_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_ERROR) >> SERCOM_I2CM_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomi2cm_clear_interrupt_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_ERROR;
}

static inline hri_sercomi2cm_intflag_reg_t hri_sercomi2cm_get_INTFLAG_reg(const void *const            hw,
                                                                          hri_sercomi2cm_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CM.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cm_intflag_reg_t hri_sercomi2cm_read_INTFLAG_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.INTFLAG.reg;
}

static inline void hri_sercomi2cm_clear_INTFLAG_reg(const void *const hw, hri_sercomi2cm_intflag_reg_t mask)
{
	((Sercom *)hw)->I2CM.INTFLAG.reg = mask;
}

static inline void hri_sercomi2cm_set_STATUS_BUSERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_BUSERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSERR) >> SERCOM_I2CM_STATUS_BUSERR_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_BUSERR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_BUSERR;
	tmp |= value << SERCOM_I2CM_STATUS_BUSERR_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_BUSERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_BUSERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_BUSERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_BUSERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_ARBLOST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_ARBLOST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_ARBLOST_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) >> SERCOM_I2CM_STATUS_ARBLOST_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_ARBLOST_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_ARBLOST;
	tmp |= value << SERCOM_I2CM_STATUS_ARBLOST_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_ARBLOST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_ARBLOST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_ARBLOST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_ARBLOST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_RXNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_RXNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_RXNACK_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) >> SERCOM_I2CM_STATUS_RXNACK_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_RXNACK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_RXNACK;
	tmp |= value << SERCOM_I2CM_STATUS_RXNACK_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_RXNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_RXNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_RXNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_RXNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_LOWTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_LOWTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_LOWTOUT_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_LOWTOUT) >> SERCOM_I2CM_STATUS_LOWTOUT_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_LOWTOUT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_LOWTOUT;
	tmp |= value << SERCOM_I2CM_STATUS_LOWTOUT_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_LOWTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_LOWTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_LOWTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_LOWTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_CLKHOLD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_CLKHOLD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_CLKHOLD_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_CLKHOLD) >> SERCOM_I2CM_STATUS_CLKHOLD_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_CLKHOLD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_CLKHOLD;
	tmp |= value << SERCOM_I2CM_STATUS_CLKHOLD_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_CLKHOLD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_CLKHOLD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_CLKHOLD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_CLKHOLD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_MEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_MEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_MEXTTOUT_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_MEXTTOUT) >> SERCOM_I2CM_STATUS_MEXTTOUT_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_MEXTTOUT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_MEXTTOUT;
	tmp |= value << SERCOM_I2CM_STATUS_MEXTTOUT_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_MEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_MEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_MEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_MEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_SEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_SEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_SEXTTOUT_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SEXTTOUT) >> SERCOM_I2CM_STATUS_SEXTTOUT_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_SEXTTOUT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_SEXTTOUT;
	tmp |= value << SERCOM_I2CM_STATUS_SEXTTOUT_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_SEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_SEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_SEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_SEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_STATUS_LENERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_LENERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_STATUS_LENERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_LENERR) >> SERCOM_I2CM_STATUS_LENERR_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_LENERR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_LENERR;
	tmp |= value << SERCOM_I2CM_STATUS_LENERR_Pos;
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_LENERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_LENERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_LENERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_LENERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_status_reg_t hri_sercomi2cm_get_STATUS_BUSSTATE_bf(const void *const           hw,
                                                                                hri_sercomi2cm_status_reg_t mask)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(mask)) >> SERCOM_I2CM_STATUS_BUSSTATE_Pos;
}

static inline void hri_sercomi2cm_set_STATUS_BUSSTATE_bf(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_status_reg_t hri_sercomi2cm_read_STATUS_BUSSTATE_bf(const void *const hw)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	return (((Sercom *)hw)->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk) >> SERCOM_I2CM_STATUS_BUSSTATE_Pos;
}

static inline void hri_sercomi2cm_write_STATUS_BUSSTATE_bf(const void *const hw, hri_sercomi2cm_status_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= ~SERCOM_I2CM_STATUS_BUSSTATE_Msk;
	tmp |= SERCOM_I2CM_STATUS_BUSSTATE(data);
	((Sercom *)hw)->I2CM.STATUS.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_STATUS_BUSSTATE_bf(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg ^= SERCOM_I2CM_STATUS_BUSSTATE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_STATUS_BUSSTATE_bf(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_status_reg_t hri_sercomi2cm_get_STATUS_reg(const void *const           hw,
                                                                        hri_sercomi2cm_status_reg_t mask)
{
	uint16_t tmp;
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_set_STATUS_reg(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg |= mask;
}

static inline hri_sercomi2cm_status_reg_t hri_sercomi2cm_read_STATUS_reg(const void *const hw)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	return ((Sercom *)hw)->I2CM.STATUS.reg;
}

static inline void hri_sercomi2cm_write_STATUS_reg(const void *const hw, hri_sercomi2cm_status_reg_t data)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg = data;
}

static inline void hri_sercomi2cm_toggle_STATUS_reg(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg ^= mask;
}

static inline void hri_sercomi2cm_clear_STATUS_reg(const void *const hw, hri_sercomi2cm_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.STATUS.reg = mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_SWRST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SWRST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SWRST) >> SERCOM_I2CM_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_set_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_ENABLE) >> SERCOM_I2CM_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_ENABLE;
	tmp |= value << SERCOM_I2CM_CTRLA_ENABLE_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_RUNSTDBY) >> SERCOM_I2CM_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_RUNSTDBY;
	tmp |= value << SERCOM_I2CM_CTRLA_RUNSTDBY_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_PINOUT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_PINOUT) >> SERCOM_I2CM_CTRLA_PINOUT_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_PINOUT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_PINOUT;
	tmp |= value << SERCOM_I2CM_CTRLA_PINOUT_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_MEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_MEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_MEXTTOEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_MEXTTOEN) >> SERCOM_I2CM_CTRLA_MEXTTOEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_MEXTTOEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_MEXTTOEN;
	tmp |= value << SERCOM_I2CM_CTRLA_MEXTTOEN_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_MEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_MEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_MEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_MEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SEXTTOEN) >> SERCOM_I2CM_CTRLA_SEXTTOEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_SEXTTOEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_SEXTTOEN;
	tmp |= value << SERCOM_I2CM_CTRLA_SEXTTOEN_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_SCLSM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SCLSM) >> SERCOM_I2CM_CTRLA_SCLSM_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_SCLSM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_SCLSM;
	tmp |= value << SERCOM_I2CM_CTRLA_SCLSM_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_LOWTOUTEN) >> SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_LOWTOUTEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_LOWTOUTEN;
	tmp |= value << SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos;
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_get_CTRLA_MODE_bf(const void *const          hw,
                                                                          hri_sercomi2cm_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_MODE(mask)) >> SERCOM_I2CM_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_MODE_Msk;
	tmp |= SERCOM_I2CM_CTRLA_MODE(data);
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_MODE_Msk) >> SERCOM_I2CM_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_get_CTRLA_SDAHOLD_bf(const void *const          hw,
                                                                             hri_sercomi2cm_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SDAHOLD(mask)) >> SERCOM_I2CM_CTRLA_SDAHOLD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_SDAHOLD_Msk;
	tmp |= SERCOM_I2CM_CTRLA_SDAHOLD(data);
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_read_CTRLA_SDAHOLD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SDAHOLD_Msk) >> SERCOM_I2CM_CTRLA_SDAHOLD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_get_CTRLA_SPEED_bf(const void *const          hw,
                                                                           hri_sercomi2cm_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SPEED(mask)) >> SERCOM_I2CM_CTRLA_SPEED_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_SPEED_Msk;
	tmp |= SERCOM_I2CM_CTRLA_SPEED(data);
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_read_CTRLA_SPEED_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_SPEED_Msk) >> SERCOM_I2CM_CTRLA_SPEED_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_CTRLA_INACTOUT_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_INACTOUT(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_get_CTRLA_INACTOUT_bf(const void *const          hw,
                                                                              hri_sercomi2cm_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_INACTOUT(mask)) >> SERCOM_I2CM_CTRLA_INACTOUT_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_INACTOUT_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= ~SERCOM_I2CM_CTRLA_INACTOUT_Msk;
	tmp |= SERCOM_I2CM_CTRLA_INACTOUT(data);
	((Sercom *)hw)->I2CM.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_INACTOUT_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~SERCOM_I2CM_CTRLA_INACTOUT(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_INACTOUT_bf(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CM.CTRLA.reg ^= SERCOM_I2CM_CTRLA_INACTOUT(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_read_CTRLA_INACTOUT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLA_INACTOUT_Msk) >> SERCOM_I2CM_CTRLA_INACTOUT_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_CTRLA_reg(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLA.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_get_CTRLA_reg(const void *const          hw,
                                                                      hri_sercomi2cm_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLA_reg(const void *const hw, hri_sercomi2cm_ctrla_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLA_reg(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLA.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLA_reg(const void *const hw, hri_sercomi2cm_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLA.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrla_reg_t hri_sercomi2cm_read_CTRLA_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.CTRLA.reg;
}

static inline void hri_sercomi2cm_set_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLB_SMEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLB_SMEN) >> SERCOM_I2CM_CTRLB_SMEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLB_SMEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp &= ~SERCOM_I2CM_CTRLB_SMEN;
	tmp |= value << SERCOM_I2CM_CTRLB_SMEN_Pos;
	((Sercom *)hw)->I2CM.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg ^= SERCOM_I2CM_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLB_QCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_QCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLB_QCEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLB_QCEN) >> SERCOM_I2CM_CTRLB_QCEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLB_QCEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp &= ~SERCOM_I2CM_CTRLB_QCEN;
	tmp |= value << SERCOM_I2CM_CTRLB_QCEN_Pos;
	((Sercom *)hw)->I2CM.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLB_QCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_QCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLB_QCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg ^= SERCOM_I2CM_CTRLB_QCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLB_ACKACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLB_ACKACT) >> SERCOM_I2CM_CTRLB_ACKACT_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLB_ACKACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp &= ~SERCOM_I2CM_CTRLB_ACKACT;
	tmp |= value << SERCOM_I2CM_CTRLB_ACKACT_Pos;
	((Sercom *)hw)->I2CM.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg ^= SERCOM_I2CM_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlb_reg_t hri_sercomi2cm_get_CTRLB_CMD_bf(const void *const          hw,
                                                                         hri_sercomi2cm_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLB_CMD(mask)) >> SERCOM_I2CM_CTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cm_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp &= ~SERCOM_I2CM_CTRLB_CMD_Msk;
	tmp |= SERCOM_I2CM_CTRLB_CMD(data);
	((Sercom *)hw)->I2CM.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.CTRLB.reg ^= SERCOM_I2CM_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlb_reg_t hri_sercomi2cm_read_CTRLB_CMD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLB_CMD_Msk) >> SERCOM_I2CM_CTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_CTRLB_reg(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLB.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlb_reg_t hri_sercomi2cm_get_CTRLB_reg(const void *const          hw,
                                                                      hri_sercomi2cm_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLB_reg(const void *const hw, hri_sercomi2cm_ctrlb_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLB.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLB_reg(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLB.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLB_reg(const void *const hw, hri_sercomi2cm_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLB.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlb_reg_t hri_sercomi2cm_read_CTRLB_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.CTRLB.reg;
}

static inline void hri_sercomi2cm_set_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg |= SERCOM_I2CM_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_CTRLC_DATA32B_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLC.reg;
	tmp = (tmp & SERCOM_I2CM_CTRLC_DATA32B) >> SERCOM_I2CM_CTRLC_DATA32B_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_CTRLC_DATA32B_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.CTRLC.reg;
	tmp &= ~SERCOM_I2CM_CTRLC_DATA32B;
	tmp |= value << SERCOM_I2CM_CTRLC_DATA32B_Pos;
	((Sercom *)hw)->I2CM.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg &= ~SERCOM_I2CM_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg ^= SERCOM_I2CM_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_CTRLC_reg(const void *const hw, hri_sercomi2cm_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlc_reg_t hri_sercomi2cm_get_CTRLC_reg(const void *const          hw,
                                                                      hri_sercomi2cm_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.CTRLC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_CTRLC_reg(const void *const hw, hri_sercomi2cm_ctrlc_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_CTRLC_reg(const void *const hw, hri_sercomi2cm_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_CTRLC_reg(const void *const hw, hri_sercomi2cm_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.CTRLC.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_ctrlc_reg_t hri_sercomi2cm_read_CTRLC_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.CTRLC.reg;
}

static inline void hri_sercomi2cm_set_BAUD_BAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg |= SERCOM_I2CM_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_get_BAUD_BAUD_bf(const void *const         hw,
                                                                        hri_sercomi2cm_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_BAUD(mask)) >> SERCOM_I2CM_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_BAUD_BAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp &= ~SERCOM_I2CM_BAUD_BAUD_Msk;
	tmp |= SERCOM_I2CM_BAUD_BAUD(data);
	((Sercom *)hw)->I2CM.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_BAUD_BAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg &= ~SERCOM_I2CM_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_BAUD_BAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg ^= SERCOM_I2CM_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_read_BAUD_BAUD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_BAUD_Msk) >> SERCOM_I2CM_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_BAUD_BAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg |= SERCOM_I2CM_BAUD_BAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_get_BAUD_BAUDLOW_bf(const void *const         hw,
                                                                           hri_sercomi2cm_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_BAUDLOW(mask)) >> SERCOM_I2CM_BAUD_BAUDLOW_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_BAUD_BAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp &= ~SERCOM_I2CM_BAUD_BAUDLOW_Msk;
	tmp |= SERCOM_I2CM_BAUD_BAUDLOW(data);
	((Sercom *)hw)->I2CM.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_BAUD_BAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg &= ~SERCOM_I2CM_BAUD_BAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_BAUD_BAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg ^= SERCOM_I2CM_BAUD_BAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_read_BAUD_BAUDLOW_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_BAUDLOW_Msk) >> SERCOM_I2CM_BAUD_BAUDLOW_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_BAUD_HSBAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg |= SERCOM_I2CM_BAUD_HSBAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_get_BAUD_HSBAUD_bf(const void *const         hw,
                                                                          hri_sercomi2cm_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_HSBAUD(mask)) >> SERCOM_I2CM_BAUD_HSBAUD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_BAUD_HSBAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp &= ~SERCOM_I2CM_BAUD_HSBAUD_Msk;
	tmp |= SERCOM_I2CM_BAUD_HSBAUD(data);
	((Sercom *)hw)->I2CM.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_BAUD_HSBAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg &= ~SERCOM_I2CM_BAUD_HSBAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_BAUD_HSBAUD_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg ^= SERCOM_I2CM_BAUD_HSBAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_read_BAUD_HSBAUD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_HSBAUD_Msk) >> SERCOM_I2CM_BAUD_HSBAUD_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_BAUD_HSBAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg |= SERCOM_I2CM_BAUD_HSBAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_get_BAUD_HSBAUDLOW_bf(const void *const         hw,
                                                                             hri_sercomi2cm_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_HSBAUDLOW(mask)) >> SERCOM_I2CM_BAUD_HSBAUDLOW_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_BAUD_HSBAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp &= ~SERCOM_I2CM_BAUD_HSBAUDLOW_Msk;
	tmp |= SERCOM_I2CM_BAUD_HSBAUDLOW(data);
	((Sercom *)hw)->I2CM.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_BAUD_HSBAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg &= ~SERCOM_I2CM_BAUD_HSBAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_BAUD_HSBAUDLOW_bf(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg ^= SERCOM_I2CM_BAUD_HSBAUDLOW(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_read_BAUD_HSBAUDLOW_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp = (tmp & SERCOM_I2CM_BAUD_HSBAUDLOW_Msk) >> SERCOM_I2CM_BAUD_HSBAUDLOW_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_BAUD_reg(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_get_BAUD_reg(const void *const         hw,
                                                                    hri_sercomi2cm_baud_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_BAUD_reg(const void *const hw, hri_sercomi2cm_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_BAUD_reg(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_BAUD_reg(const void *const hw, hri_sercomi2cm_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_baud_reg_t hri_sercomi2cm_read_BAUD_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.BAUD.reg;
}

static inline void hri_sercomi2cm_set_ADDR_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg |= SERCOM_I2CM_ADDR_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_ADDR_LENEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_LENEN) >> SERCOM_I2CM_ADDR_LENEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_ADDR_LENEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= ~SERCOM_I2CM_ADDR_LENEN;
	tmp |= value << SERCOM_I2CM_ADDR_LENEN_Pos;
	((Sercom *)hw)->I2CM.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg &= ~SERCOM_I2CM_ADDR_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg ^= SERCOM_I2CM_ADDR_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_ADDR_HS_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg |= SERCOM_I2CM_ADDR_HS;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_ADDR_HS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_HS) >> SERCOM_I2CM_ADDR_HS_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_ADDR_HS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= ~SERCOM_I2CM_ADDR_HS;
	tmp |= value << SERCOM_I2CM_ADDR_HS_Pos;
	((Sercom *)hw)->I2CM.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_HS_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg &= ~SERCOM_I2CM_ADDR_HS;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_HS_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg ^= SERCOM_I2CM_ADDR_HS;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg |= SERCOM_I2CM_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_ADDR_TENBITEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_TENBITEN) >> SERCOM_I2CM_ADDR_TENBITEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_ADDR_TENBITEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= ~SERCOM_I2CM_ADDR_TENBITEN;
	tmp |= value << SERCOM_I2CM_ADDR_TENBITEN_Pos;
	((Sercom *)hw)->I2CM.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg &= ~SERCOM_I2CM_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg ^= SERCOM_I2CM_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg |= SERCOM_I2CM_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_get_ADDR_ADDR_bf(const void *const         hw,
                                                                        hri_sercomi2cm_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_ADDR(mask)) >> SERCOM_I2CM_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cm_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= ~SERCOM_I2CM_ADDR_ADDR_Msk;
	tmp |= SERCOM_I2CM_ADDR_ADDR(data);
	((Sercom *)hw)->I2CM.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg &= ~SERCOM_I2CM_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg ^= SERCOM_I2CM_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_ADDR_Msk) >> SERCOM_I2CM_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_ADDR_LEN_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg |= SERCOM_I2CM_ADDR_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_get_ADDR_LEN_bf(const void *const         hw,
                                                                       hri_sercomi2cm_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_LEN(mask)) >> SERCOM_I2CM_ADDR_LEN_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_ADDR_LEN_bf(const void *const hw, hri_sercomi2cm_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= ~SERCOM_I2CM_ADDR_LEN_Msk;
	tmp |= SERCOM_I2CM_ADDR_LEN(data);
	((Sercom *)hw)->I2CM.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_LEN_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg &= ~SERCOM_I2CM_ADDR_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_LEN_bf(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	((Sercom *)hw)->I2CM.ADDR.reg ^= SERCOM_I2CM_ADDR_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_read_ADDR_LEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp = (tmp & SERCOM_I2CM_ADDR_LEN_Msk) >> SERCOM_I2CM_ADDR_LEN_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_set_ADDR_reg(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.ADDR.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_get_ADDR_reg(const void *const         hw,
                                                                    hri_sercomi2cm_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_ADDR_reg(const void *const hw, hri_sercomi2cm_addr_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.ADDR.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_ADDR_reg(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.ADDR.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_ADDR_reg(const void *const hw, hri_sercomi2cm_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.ADDR.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_addr_reg_t hri_sercomi2cm_read_ADDR_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.ADDR.reg;
}

static inline hri_sercomi2cm_data_reg_t hri_sercomi2cm_read_DATA_DATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.DATA.reg;
	tmp = (tmp & SERCOM_I2CM_DATA_DATA_Msk) >> SERCOM_I2CM_DATA_DATA_Pos;
	return tmp;
}

static inline void hri_sercomi2cm_write_DATA_DATA_bf(const void *const hw, hri_sercomi2cm_data_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SYSOP);
	tmp = ((Sercom *)hw)->I2CM.DATA.reg;
	tmp &= ~SERCOM_I2CM_DATA_DATA_Msk;
	tmp |= SERCOM_I2CM_DATA_DATA(data);
	((Sercom *)hw)->I2CM.DATA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_data_reg_t hri_sercomi2cm_read_DATA_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.DATA.reg;
}

static inline void hri_sercomi2cm_write_DATA_reg(const void *const hw, hri_sercomi2cm_data_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DATA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg |= SERCOM_I2CM_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cm_get_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CM.DBGCTRL.reg;
	tmp = (tmp & SERCOM_I2CM_DBGCTRL_DBGSTOP) >> SERCOM_I2CM_DBGCTRL_DBGSTOP_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cm_write_DBGCTRL_DBGSTOP_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CM.DBGCTRL.reg;
	tmp &= ~SERCOM_I2CM_DBGCTRL_DBGSTOP;
	tmp |= value << SERCOM_I2CM_DBGCTRL_DBGSTOP_Pos;
	((Sercom *)hw)->I2CM.DBGCTRL.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg &= ~SERCOM_I2CM_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg ^= SERCOM_I2CM_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_set_DBGCTRL_reg(const void *const hw, hri_sercomi2cm_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_dbgctrl_reg_t hri_sercomi2cm_get_DBGCTRL_reg(const void *const            hw,
                                                                          hri_sercomi2cm_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CM.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cm_write_DBGCTRL_reg(const void *const hw, hri_sercomi2cm_dbgctrl_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_clear_DBGCTRL_reg(const void *const hw, hri_sercomi2cm_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cm_toggle_DBGCTRL_reg(const void *const hw, hri_sercomi2cm_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CM.DBGCTRL.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cm_dbgctrl_reg_t hri_sercomi2cm_read_DBGCTRL_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.DBGCTRL.reg;
}

static inline bool hri_sercomi2cm_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SWRST) >> SERCOM_I2CM_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_sercomi2cm_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE) >> SERCOM_I2CM_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_sercomi2cm_get_SYNCBUSY_SYSOP_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SYSOP) >> SERCOM_I2CM_SYNCBUSY_SYSOP_Pos;
}

static inline bool hri_sercomi2cm_get_SYNCBUSY_LENGTH_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_LENGTH) >> SERCOM_I2CM_SYNCBUSY_LENGTH_Pos;
}

static inline hri_sercomi2cm_syncbusy_reg_t hri_sercomi2cm_get_SYNCBUSY_reg(const void *const             hw,
                                                                            hri_sercomi2cm_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CM.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cm_syncbusy_reg_t hri_sercomi2cm_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CM.SYNCBUSY.reg;
}

static inline void hri_sercomusart_set_BAUD_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_BAUD_bf(const void *const          hw,
                                                                          hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_BAUD(mask)) >> SERCOM_USART_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_BAUD_Msk;
	tmp |= SERCOM_USART_BAUD_BAUD(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_BAUD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_BAUD_Msk) >> SERCOM_USART_BAUD_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_FRAC_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_FRAC_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRAC_BAUD_bf(const void *const          hw,
                                                                               hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRAC_BAUD(mask)) >> SERCOM_USART_BAUD_FRAC_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRAC_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_FRAC_BAUD_Msk;
	tmp |= SERCOM_USART_BAUD_FRAC_BAUD(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRAC_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_FRAC_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRAC_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_FRAC_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRAC_BAUD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRAC_BAUD_Msk) >> SERCOM_USART_BAUD_FRAC_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_FRAC_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_FRAC_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRAC_FP_bf(const void *const          hw,
                                                                             hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRAC_FP(mask)) >> SERCOM_USART_BAUD_FRAC_FP_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRAC_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_FRAC_FP_Msk;
	tmp |= SERCOM_USART_BAUD_FRAC_FP(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRAC_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_FRAC_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRAC_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_FRAC_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRAC_FP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRAC_FP_Msk) >> SERCOM_USART_BAUD_FRAC_FP_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_FRACFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_FRACFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRACFP_BAUD_bf(const void *const          hw,
                                                                                 hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRACFP_BAUD(mask)) >> SERCOM_USART_BAUD_FRACFP_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRACFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_FRACFP_BAUD_Msk;
	tmp |= SERCOM_USART_BAUD_FRACFP_BAUD(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRACFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_FRACFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRACFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_FRACFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRACFP_BAUD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRACFP_BAUD_Msk) >> SERCOM_USART_BAUD_FRACFP_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_FRACFP_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_FRACFP_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRACFP_FP_bf(const void *const          hw,
                                                                               hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRACFP_FP(mask)) >> SERCOM_USART_BAUD_FRACFP_FP_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRACFP_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_FRACFP_FP_Msk;
	tmp |= SERCOM_USART_BAUD_FRACFP_FP(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRACFP_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_FRACFP_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRACFP_FP_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_FRACFP_FP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRACFP_FP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_FRACFP_FP_Msk) >> SERCOM_USART_BAUD_FRACFP_FP_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_USARTFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= SERCOM_USART_BAUD_USARTFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_USARTFP_BAUD_bf(const void *const          hw,
                                                                                  hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_USARTFP_BAUD(mask)) >> SERCOM_USART_BAUD_USARTFP_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_USARTFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= ~SERCOM_USART_BAUD_USARTFP_BAUD_Msk;
	tmp |= SERCOM_USART_BAUD_USARTFP_BAUD(data);
	((Sercom *)hw)->USART.BAUD.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_USARTFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~SERCOM_USART_BAUD_USARTFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_USARTFP_BAUD_bf(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= SERCOM_USART_BAUD_USARTFP_BAUD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_USARTFP_BAUD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp = (tmp & SERCOM_USART_BAUD_USARTFP_BAUD_Msk) >> SERCOM_USART_BAUD_USARTFP_BAUD_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_BAUD_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_reg(const void *const          hw,
                                                                      hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_reg(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.BAUD.reg;
}

static inline void hri_sercomusart_set_BAUD_FRAC_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRAC_reg(const void *const          hw,
                                                                           hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRAC_reg(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRAC_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRAC_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRAC_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.BAUD.reg;
}

static inline void hri_sercomusart_set_BAUD_FRACFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_FRACFP_reg(const void *const          hw,
                                                                             hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_FRACFP_reg(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_FRACFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_FRACFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_FRACFP_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.BAUD.reg;
}

static inline void hri_sercomusart_set_BAUD_USARTFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_get_BAUD_USARTFP_reg(const void *const          hw,
                                                                              hri_sercomusart_baud_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.BAUD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_BAUD_USARTFP_reg(const void *const hw, hri_sercomusart_baud_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_BAUD_USARTFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_BAUD_USARTFP_reg(const void *const hw, hri_sercomusart_baud_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.BAUD.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_baud_reg_t hri_sercomusart_read_BAUD_USARTFP_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.BAUD.reg;
}

static inline void hri_sercomusart_set_INTEN_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
}

static inline bool hri_sercomusart_get_INTEN_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_DRE) >> SERCOM_USART_INTENSET_DRE_Pos;
}

static inline void hri_sercomusart_write_INTEN_DRE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_DRE;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
	}
}

static inline void hri_sercomusart_clear_INTEN_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_DRE;
}

static inline void hri_sercomusart_set_INTEN_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_TXC;
}

static inline bool hri_sercomusart_get_INTEN_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_TXC) >> SERCOM_USART_INTENSET_TXC_Pos;
}

static inline void hri_sercomusart_write_INTEN_TXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_TXC;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_TXC;
	}
}

static inline void hri_sercomusart_clear_INTEN_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_TXC;
}

static inline void hri_sercomusart_set_INTEN_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
}

static inline bool hri_sercomusart_get_INTEN_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_RXC) >> SERCOM_USART_INTENSET_RXC_Pos;
}

static inline void hri_sercomusart_write_INTEN_RXC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
	}
}

static inline void hri_sercomusart_clear_INTEN_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
}

static inline void hri_sercomusart_set_INTEN_RXS_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXS;
}

static inline bool hri_sercomusart_get_INTEN_RXS_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_RXS) >> SERCOM_USART_INTENSET_RXS_Pos;
}

static inline void hri_sercomusart_write_INTEN_RXS_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXS;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXS;
	}
}

static inline void hri_sercomusart_clear_INTEN_RXS_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXS;
}

static inline void hri_sercomusart_set_INTEN_CTSIC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_CTSIC;
}

static inline bool hri_sercomusart_get_INTEN_CTSIC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_CTSIC) >> SERCOM_USART_INTENSET_CTSIC_Pos;
}

static inline void hri_sercomusart_write_INTEN_CTSIC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_CTSIC;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_CTSIC;
	}
}

static inline void hri_sercomusart_clear_INTEN_CTSIC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_CTSIC;
}

static inline void hri_sercomusart_set_INTEN_RXBRK_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXBRK;
}

static inline bool hri_sercomusart_get_INTEN_RXBRK_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_RXBRK) >> SERCOM_USART_INTENSET_RXBRK_Pos;
}

static inline void hri_sercomusart_write_INTEN_RXBRK_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXBRK;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXBRK;
	}
}

static inline void hri_sercomusart_clear_INTEN_RXBRK_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXBRK;
}

static inline void hri_sercomusart_set_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_ERROR;
}

static inline bool hri_sercomusart_get_INTEN_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTENSET.reg & SERCOM_USART_INTENSET_ERROR) >> SERCOM_USART_INTENSET_ERROR_Pos;
}

static inline void hri_sercomusart_write_INTEN_ERROR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_ERROR;
	} else {
		((Sercom *)hw)->USART.INTENSET.reg = SERCOM_USART_INTENSET_ERROR;
	}
}

static inline void hri_sercomusart_clear_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTENCLR.reg = SERCOM_USART_INTENSET_ERROR;
}

static inline void hri_sercomusart_set_INTEN_reg(const void *const hw, hri_sercomusart_intenset_reg_t mask)
{
	((Sercom *)hw)->USART.INTENSET.reg = mask;
}

static inline hri_sercomusart_intenset_reg_t hri_sercomusart_get_INTEN_reg(const void *const              hw,
                                                                           hri_sercomusart_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomusart_intenset_reg_t hri_sercomusart_read_INTEN_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.INTENSET.reg;
}

static inline void hri_sercomusart_write_INTEN_reg(const void *const hw, hri_sercomusart_intenset_reg_t data)
{
	((Sercom *)hw)->USART.INTENSET.reg = data;
	((Sercom *)hw)->USART.INTENCLR.reg = ~data;
}

static inline void hri_sercomusart_clear_INTEN_reg(const void *const hw, hri_sercomusart_intenset_reg_t mask)
{
	((Sercom *)hw)->USART.INTENCLR.reg = mask;
}

static inline bool hri_sercomusart_get_INTFLAG_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE) >> SERCOM_USART_INTFLAG_DRE_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_DRE;
}

static inline bool hri_sercomusart_get_INTFLAG_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) >> SERCOM_USART_INTFLAG_TXC_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;
}

static inline bool hri_sercomusart_get_INTFLAG_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) >> SERCOM_USART_INTFLAG_RXC_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXC;
}

static inline bool hri_sercomusart_get_INTFLAG_RXS_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXS) >> SERCOM_USART_INTFLAG_RXS_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_RXS_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXS;
}

static inline bool hri_sercomusart_get_INTFLAG_CTSIC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_CTSIC) >> SERCOM_USART_INTFLAG_CTSIC_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_CTSIC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_CTSIC;
}

static inline bool hri_sercomusart_get_INTFLAG_RXBRK_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXBRK) >> SERCOM_USART_INTFLAG_RXBRK_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_RXBRK_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;
}

static inline bool hri_sercomusart_get_INTFLAG_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) >> SERCOM_USART_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomusart_clear_INTFLAG_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
}

static inline bool hri_sercomusart_get_interrupt_DRE_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE) >> SERCOM_USART_INTFLAG_DRE_Pos;
}

static inline void hri_sercomusart_clear_interrupt_DRE_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_DRE;
}

static inline bool hri_sercomusart_get_interrupt_TXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) >> SERCOM_USART_INTFLAG_TXC_Pos;
}

static inline void hri_sercomusart_clear_interrupt_TXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;
}

static inline bool hri_sercomusart_get_interrupt_RXC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) >> SERCOM_USART_INTFLAG_RXC_Pos;
}

static inline void hri_sercomusart_clear_interrupt_RXC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXC;
}

static inline bool hri_sercomusart_get_interrupt_RXS_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXS) >> SERCOM_USART_INTFLAG_RXS_Pos;
}

static inline void hri_sercomusart_clear_interrupt_RXS_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXS;
}

static inline bool hri_sercomusart_get_interrupt_CTSIC_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_CTSIC) >> SERCOM_USART_INTFLAG_CTSIC_Pos;
}

static inline void hri_sercomusart_clear_interrupt_CTSIC_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_CTSIC;
}

static inline bool hri_sercomusart_get_interrupt_RXBRK_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXBRK) >> SERCOM_USART_INTFLAG_RXBRK_Pos;
}

static inline void hri_sercomusart_clear_interrupt_RXBRK_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;
}

static inline bool hri_sercomusart_get_interrupt_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_ERROR) >> SERCOM_USART_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomusart_clear_interrupt_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
}

static inline hri_sercomusart_intflag_reg_t hri_sercomusart_get_INTFLAG_reg(const void *const             hw,
                                                                            hri_sercomusart_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomusart_intflag_reg_t hri_sercomusart_read_INTFLAG_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.INTFLAG.reg;
}

static inline void hri_sercomusart_clear_INTFLAG_reg(const void *const hw, hri_sercomusart_intflag_reg_t mask)
{
	((Sercom *)hw)->USART.INTFLAG.reg = mask;
}

static inline void hri_sercomusart_set_CTRLA_SWRST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_SWRST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_SWRST) >> SERCOM_USART_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_set_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST | SERCOM_USART_SYNCBUSY_ENABLE);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST | SERCOM_USART_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_ENABLE) >> SERCOM_USART_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST | SERCOM_USART_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_ENABLE;
	tmp |= value << SERCOM_USART_CTRLA_ENABLE_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST | SERCOM_USART_SYNCBUSY_ENABLE);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST | SERCOM_USART_SYNCBUSY_ENABLE);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_RUNSTDBY) >> SERCOM_USART_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_RUNSTDBY;
	tmp |= value << SERCOM_USART_CTRLA_RUNSTDBY_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_IBON_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_IBON) >> SERCOM_USART_CTRLA_IBON_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_IBON_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_IBON;
	tmp |= value << SERCOM_USART_CTRLA_IBON_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_IBON_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_IBON;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_TXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_TXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_TXINV_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_TXINV) >> SERCOM_USART_CTRLA_TXINV_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_TXINV_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_TXINV;
	tmp |= value << SERCOM_USART_CTRLA_TXINV_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_TXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_TXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_TXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_TXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_RXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_RXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_RXINV_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_RXINV) >> SERCOM_USART_CTRLA_RXINV_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_RXINV_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_RXINV;
	tmp |= value << SERCOM_USART_CTRLA_RXINV_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_RXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_RXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_RXINV_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_RXINV;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_CMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_CMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_CMODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_CMODE) >> SERCOM_USART_CTRLA_CMODE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_CMODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_CMODE;
	tmp |= value << SERCOM_USART_CTRLA_CMODE_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_CMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_CMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_CMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_CMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_CPOL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_CPOL) >> SERCOM_USART_CTRLA_CPOL_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_CPOL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_CPOL;
	tmp |= value << SERCOM_USART_CTRLA_CPOL_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_CPOL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_CPOL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLA_DORD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_DORD) >> SERCOM_USART_CTRLA_DORD_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLA_DORD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_DORD;
	tmp |= value << SERCOM_USART_CTRLA_DORD_Pos;
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_DORD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_DORD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLA_MODE_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_MODE_bf(const void *const           hw,
                                                                            hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_MODE(mask)) >> SERCOM_USART_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_MODE_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_MODE_Msk;
	tmp |= SERCOM_USART_CTRLA_MODE(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_MODE_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_MODE_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_MODE_Msk) >> SERCOM_USART_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_SAMPR_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_SAMPR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_SAMPR_bf(const void *const           hw,
                                                                             hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_SAMPR(mask)) >> SERCOM_USART_CTRLA_SAMPR_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_SAMPR_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_SAMPR_Msk;
	tmp |= SERCOM_USART_CTRLA_SAMPR(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_SAMPR_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_SAMPR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_SAMPR_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_SAMPR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_SAMPR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_SAMPR_Msk) >> SERCOM_USART_CTRLA_SAMPR_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_TXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_TXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_TXPO_bf(const void *const           hw,
                                                                            hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_TXPO(mask)) >> SERCOM_USART_CTRLA_TXPO_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_TXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_TXPO_Msk;
	tmp |= SERCOM_USART_CTRLA_TXPO(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_TXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_TXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_TXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_TXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_TXPO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_TXPO_Msk) >> SERCOM_USART_CTRLA_TXPO_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_RXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_RXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_RXPO_bf(const void *const           hw,
                                                                            hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_RXPO(mask)) >> SERCOM_USART_CTRLA_RXPO_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_RXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_RXPO_Msk;
	tmp |= SERCOM_USART_CTRLA_RXPO(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_RXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_RXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_RXPO_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_RXPO(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_RXPO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_RXPO_Msk) >> SERCOM_USART_CTRLA_RXPO_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_SAMPA_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_SAMPA(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_SAMPA_bf(const void *const           hw,
                                                                             hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_SAMPA(mask)) >> SERCOM_USART_CTRLA_SAMPA_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_SAMPA_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_SAMPA_Msk;
	tmp |= SERCOM_USART_CTRLA_SAMPA(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_SAMPA_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_SAMPA(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_SAMPA_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_SAMPA(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_SAMPA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_SAMPA_Msk) >> SERCOM_USART_CTRLA_SAMPA_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_FORM_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg |= SERCOM_USART_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_FORM_bf(const void *const           hw,
                                                                            hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_FORM(mask)) >> SERCOM_USART_CTRLA_FORM_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_FORM_bf(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= ~SERCOM_USART_CTRLA_FORM_Msk;
	tmp |= SERCOM_USART_CTRLA_FORM(data);
	((Sercom *)hw)->USART.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_FORM_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_FORM_bf(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_MASK);
	((Sercom *)hw)->USART.CTRLA.reg ^= SERCOM_USART_CTRLA_FORM(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_FORM_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp = (tmp & SERCOM_USART_CTRLA_FORM_Msk) >> SERCOM_USART_CTRLA_FORM_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLA_reg(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLA.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_get_CTRLA_reg(const void *const           hw,
                                                                        hri_sercomusart_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLA_reg(const void *const hw, hri_sercomusart_ctrla_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLA_reg(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLA.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLA_reg(const void *const hw, hri_sercomusart_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLA.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrla_reg_t hri_sercomusart_read_CTRLA_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.CTRLA.reg;
}

static inline void hri_sercomusart_set_CTRLB_SBMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_SBMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_SBMODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_SBMODE) >> SERCOM_USART_CTRLB_SBMODE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_SBMODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_SBMODE;
	tmp |= value << SERCOM_USART_CTRLB_SBMODE_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_SBMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_SBMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_SBMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_SBMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_COLDEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_COLDEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_COLDEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_COLDEN) >> SERCOM_USART_CTRLB_COLDEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_COLDEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_COLDEN;
	tmp |= value << SERCOM_USART_CTRLB_COLDEN_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_COLDEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_COLDEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_COLDEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_COLDEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_SFDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_SFDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_SFDE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_SFDE) >> SERCOM_USART_CTRLB_SFDE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_SFDE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_SFDE;
	tmp |= value << SERCOM_USART_CTRLB_SFDE_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_SFDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_SFDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_SFDE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_SFDE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_ENC_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_ENC;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_ENC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_ENC) >> SERCOM_USART_CTRLB_ENC_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_ENC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_ENC;
	tmp |= value << SERCOM_USART_CTRLB_ENC_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_ENC_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_ENC;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_ENC_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_ENC;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_PMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_PMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_PMODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_PMODE) >> SERCOM_USART_CTRLB_PMODE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_PMODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_PMODE;
	tmp |= value << SERCOM_USART_CTRLB_PMODE_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_PMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_PMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_PMODE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_PMODE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_TXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_TXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_TXEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_TXEN) >> SERCOM_USART_CTRLB_TXEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_TXEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_TXEN;
	tmp |= value << SERCOM_USART_CTRLB_TXEN_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_TXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_TXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_TXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_TXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLB_RXEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_RXEN) >> SERCOM_USART_CTRLB_RXEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLB_RXEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_RXEN;
	tmp |= value << SERCOM_USART_CTRLB_RXEN_Pos;
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_RXEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_RXEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_get_CTRLB_CHSIZE_bf(const void *const           hw,
                                                                              hri_sercomusart_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_CHSIZE(mask)) >> SERCOM_USART_CTRLB_CHSIZE_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_CHSIZE_Msk;
	tmp |= SERCOM_USART_CTRLB_CHSIZE(data);
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_CHSIZE_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_CHSIZE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_read_CTRLB_CHSIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_CHSIZE_Msk) >> SERCOM_USART_CTRLB_CHSIZE_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLB_LINCMD_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= SERCOM_USART_CTRLB_LINCMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_get_CTRLB_LINCMD_bf(const void *const           hw,
                                                                              hri_sercomusart_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_LINCMD(mask)) >> SERCOM_USART_CTRLB_LINCMD_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLB_LINCMD_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= ~SERCOM_USART_CTRLB_LINCMD_Msk;
	tmp |= SERCOM_USART_CTRLB_LINCMD(data);
	((Sercom *)hw)->USART.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_LINCMD_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_LINCMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_LINCMD_bf(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= SERCOM_USART_CTRLB_LINCMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_read_CTRLB_LINCMD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp = (tmp & SERCOM_USART_CTRLB_LINCMD_Msk) >> SERCOM_USART_CTRLB_LINCMD_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLB_reg(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_get_CTRLB_reg(const void *const           hw,
                                                                        hri_sercomusart_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLB_reg(const void *const hw, hri_sercomusart_ctrlb_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLB_reg(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLB_reg(const void *const hw, hri_sercomusart_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLB.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlb_reg_t hri_sercomusart_read_CTRLB_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.CTRLB.reg;
}

static inline void hri_sercomusart_set_CTRLC_INACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_INACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLC_INACK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_INACK) >> SERCOM_USART_CTRLC_INACK_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLC_INACK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_INACK;
	tmp |= value << SERCOM_USART_CTRLC_INACK_Pos;
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_INACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_INACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_INACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_INACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLC_DSNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_DSNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_CTRLC_DSNACK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_DSNACK) >> SERCOM_USART_CTRLC_DSNACK_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_CTRLC_DSNACK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_DSNACK;
	tmp |= value << SERCOM_USART_CTRLC_DSNACK_Pos;
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_DSNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_DSNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_DSNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_DSNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_CTRLC_GTIME_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_GTIME(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_GTIME_bf(const void *const           hw,
                                                                             hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_GTIME(mask)) >> SERCOM_USART_CTRLC_GTIME_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_GTIME_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_GTIME_Msk;
	tmp |= SERCOM_USART_CTRLC_GTIME(data);
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_GTIME_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_GTIME(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_GTIME_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_GTIME(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_GTIME_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_GTIME_Msk) >> SERCOM_USART_CTRLC_GTIME_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLC_BRKLEN_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_BRKLEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_BRKLEN_bf(const void *const           hw,
                                                                              hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_BRKLEN(mask)) >> SERCOM_USART_CTRLC_BRKLEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_BRKLEN_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_BRKLEN_Msk;
	tmp |= SERCOM_USART_CTRLC_BRKLEN(data);
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_BRKLEN_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_BRKLEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_BRKLEN_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_BRKLEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_BRKLEN_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_BRKLEN_Msk) >> SERCOM_USART_CTRLC_BRKLEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLC_HDRDLY_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_HDRDLY(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_HDRDLY_bf(const void *const           hw,
                                                                              hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_HDRDLY(mask)) >> SERCOM_USART_CTRLC_HDRDLY_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_HDRDLY_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_HDRDLY_Msk;
	tmp |= SERCOM_USART_CTRLC_HDRDLY(data);
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_HDRDLY_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_HDRDLY(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_HDRDLY_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_HDRDLY(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_HDRDLY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_HDRDLY_Msk) >> SERCOM_USART_CTRLC_HDRDLY_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLC_MAXITER_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_MAXITER(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_MAXITER_bf(const void *const           hw,
                                                                               hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_MAXITER(mask)) >> SERCOM_USART_CTRLC_MAXITER_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_MAXITER_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_MAXITER_Msk;
	tmp |= SERCOM_USART_CTRLC_MAXITER(data);
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_MAXITER_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_MAXITER(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_MAXITER_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_MAXITER(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_MAXITER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_MAXITER_Msk) >> SERCOM_USART_CTRLC_MAXITER_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLC_DATA32B_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= SERCOM_USART_CTRLC_DATA32B(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_DATA32B_bf(const void *const           hw,
                                                                               hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_DATA32B(mask)) >> SERCOM_USART_CTRLC_DATA32B_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_DATA32B_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= ~SERCOM_USART_CTRLC_DATA32B_Msk;
	tmp |= SERCOM_USART_CTRLC_DATA32B(data);
	((Sercom *)hw)->USART.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_DATA32B_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~SERCOM_USART_CTRLC_DATA32B(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_DATA32B_bf(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= SERCOM_USART_CTRLC_DATA32B(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_DATA32B_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp = (tmp & SERCOM_USART_CTRLC_DATA32B_Msk) >> SERCOM_USART_CTRLC_DATA32B_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_CTRLC_reg(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_get_CTRLC_reg(const void *const           hw,
                                                                        hri_sercomusart_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.CTRLC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_CTRLC_reg(const void *const hw, hri_sercomusart_ctrlc_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_CTRLC_reg(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_CTRLC_reg(const void *const hw, hri_sercomusart_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.CTRLC.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_ctrlc_reg_t hri_sercomusart_read_CTRLC_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.CTRLC.reg;
}

static inline void hri_sercomusart_set_RXPL_RXPL_bf(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg |= SERCOM_USART_RXPL_RXPL(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_rxpl_reg_t hri_sercomusart_get_RXPL_RXPL_bf(const void *const          hw,
                                                                          hri_sercomusart_rxpl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.RXPL.reg;
	tmp = (tmp & SERCOM_USART_RXPL_RXPL(mask)) >> SERCOM_USART_RXPL_RXPL_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_RXPL_RXPL_bf(const void *const hw, hri_sercomusart_rxpl_reg_t data)
{
	uint8_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.RXPL.reg;
	tmp &= ~SERCOM_USART_RXPL_RXPL_Msk;
	tmp |= SERCOM_USART_RXPL_RXPL(data);
	((Sercom *)hw)->USART.RXPL.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_RXPL_RXPL_bf(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg &= ~SERCOM_USART_RXPL_RXPL(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_RXPL_RXPL_bf(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg ^= SERCOM_USART_RXPL_RXPL(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_rxpl_reg_t hri_sercomusart_read_RXPL_RXPL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.RXPL.reg;
	tmp = (tmp & SERCOM_USART_RXPL_RXPL_Msk) >> SERCOM_USART_RXPL_RXPL_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_RXPL_reg(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_rxpl_reg_t hri_sercomusart_get_RXPL_reg(const void *const          hw,
                                                                      hri_sercomusart_rxpl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.RXPL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_RXPL_reg(const void *const hw, hri_sercomusart_rxpl_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_RXPL_reg(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_RXPL_reg(const void *const hw, hri_sercomusart_rxpl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.RXPL.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_rxpl_reg_t hri_sercomusart_read_RXPL_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.RXPL.reg;
}

static inline void hri_sercomusart_set_LENGTH_LEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg |= SERCOM_USART_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_get_LENGTH_LEN_bf(const void *const            hw,
                                                                             hri_sercomusart_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp = (tmp & SERCOM_USART_LENGTH_LEN(mask)) >> SERCOM_USART_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_LENGTH_LEN_bf(const void *const hw, hri_sercomusart_length_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp &= ~SERCOM_USART_LENGTH_LEN_Msk;
	tmp |= SERCOM_USART_LENGTH_LEN(data);
	((Sercom *)hw)->USART.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_LENGTH_LEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg &= ~SERCOM_USART_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_LENGTH_LEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg ^= SERCOM_USART_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_read_LENGTH_LEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp = (tmp & SERCOM_USART_LENGTH_LEN_Msk) >> SERCOM_USART_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_LENGTH_LENEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg |= SERCOM_USART_LENGTH_LENEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_get_LENGTH_LENEN_bf(const void *const            hw,
                                                                               hri_sercomusart_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp = (tmp & SERCOM_USART_LENGTH_LENEN(mask)) >> SERCOM_USART_LENGTH_LENEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_LENGTH_LENEN_bf(const void *const hw, hri_sercomusart_length_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp &= ~SERCOM_USART_LENGTH_LENEN_Msk;
	tmp |= SERCOM_USART_LENGTH_LENEN(data);
	((Sercom *)hw)->USART.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_LENGTH_LENEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg &= ~SERCOM_USART_LENGTH_LENEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_LENGTH_LENEN_bf(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg ^= SERCOM_USART_LENGTH_LENEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_read_LENGTH_LENEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp = (tmp & SERCOM_USART_LENGTH_LENEN_Msk) >> SERCOM_USART_LENGTH_LENEN_Pos;
	return tmp;
}

static inline void hri_sercomusart_set_LENGTH_reg(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_get_LENGTH_reg(const void *const            hw,
                                                                          hri_sercomusart_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.LENGTH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_LENGTH_reg(const void *const hw, hri_sercomusart_length_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_LENGTH_reg(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_LENGTH_reg(const void *const hw, hri_sercomusart_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.LENGTH.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_length_reg_t hri_sercomusart_read_LENGTH_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.LENGTH.reg;
}

static inline hri_sercomusart_data_reg_t hri_sercomusart_read_DATA_DATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.DATA.reg;
	tmp = (tmp & SERCOM_USART_DATA_DATA_Msk) >> SERCOM_USART_DATA_DATA_Pos;
	return tmp;
}

static inline void hri_sercomusart_write_DATA_DATA_bf(const void *const hw, hri_sercomusart_data_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.DATA.reg;
	tmp &= ~SERCOM_USART_DATA_DATA_Msk;
	tmp |= SERCOM_USART_DATA_DATA(data);
	((Sercom *)hw)->USART.DATA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_data_reg_t hri_sercomusart_read_DATA_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.DATA.reg;
}

static inline void hri_sercomusart_write_DATA_reg(const void *const hw, hri_sercomusart_data_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DATA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg |= SERCOM_USART_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.DBGCTRL.reg;
	tmp = (tmp & SERCOM_USART_DBGCTRL_DBGSTOP) >> SERCOM_USART_DBGCTRL_DBGSTOP_Pos;
	return (bool)tmp;
}

static inline void hri_sercomusart_write_DBGCTRL_DBGSTOP_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->USART.DBGCTRL.reg;
	tmp &= ~SERCOM_USART_DBGCTRL_DBGSTOP;
	tmp |= value << SERCOM_USART_DBGCTRL_DBGSTOP_Pos;
	((Sercom *)hw)->USART.DBGCTRL.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg &= ~SERCOM_USART_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_DBGCTRL_DBGSTOP_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg ^= SERCOM_USART_DBGCTRL_DBGSTOP;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_set_DBGCTRL_reg(const void *const hw, hri_sercomusart_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_dbgctrl_reg_t hri_sercomusart_get_DBGCTRL_reg(const void *const             hw,
                                                                            hri_sercomusart_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_write_DBGCTRL_reg(const void *const hw, hri_sercomusart_dbgctrl_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_clear_DBGCTRL_reg(const void *const hw, hri_sercomusart_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomusart_toggle_DBGCTRL_reg(const void *const hw, hri_sercomusart_dbgctrl_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.DBGCTRL.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_dbgctrl_reg_t hri_sercomusart_read_DBGCTRL_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.DBGCTRL.reg;
}

static inline bool hri_sercomusart_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST) >> SERCOM_USART_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_sercomusart_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE) >> SERCOM_USART_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_sercomusart_get_SYNCBUSY_CTRLB_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB) >> SERCOM_USART_SYNCBUSY_CTRLB_Pos;
}

static inline bool hri_sercomusart_get_SYNCBUSY_RXERRCNT_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_RXERRCNT) >> SERCOM_USART_SYNCBUSY_RXERRCNT_Pos;
}

static inline bool hri_sercomusart_get_SYNCBUSY_LENGTH_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_LENGTH) >> SERCOM_USART_SYNCBUSY_LENGTH_Pos;
}

static inline hri_sercomusart_syncbusy_reg_t hri_sercomusart_get_SYNCBUSY_reg(const void *const              hw,
                                                                              hri_sercomusart_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->USART.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomusart_syncbusy_reg_t hri_sercomusart_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.SYNCBUSY.reg;
}

static inline hri_sercomusart_rxerrcnt_reg_t hri_sercomusart_get_RXERRCNT_reg(const void *const              hw,
                                                                              hri_sercomusart_rxerrcnt_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->USART.RXERRCNT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomusart_rxerrcnt_reg_t hri_sercomusart_read_RXERRCNT_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.RXERRCNT.reg;
}

static inline bool hri_sercomusart_get_STATUS_PERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_PERR) >> SERCOM_USART_STATUS_PERR_Pos;
}

static inline void hri_sercomusart_clear_STATUS_PERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_PERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_FERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_FERR) >> SERCOM_USART_STATUS_FERR_Pos;
}

static inline void hri_sercomusart_clear_STATUS_FERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_FERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_BUFOVF_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_BUFOVF) >> SERCOM_USART_STATUS_BUFOVF_Pos;
}

static inline void hri_sercomusart_clear_STATUS_BUFOVF_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_BUFOVF;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_CTS_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_CTS) >> SERCOM_USART_STATUS_CTS_Pos;
}

static inline void hri_sercomusart_clear_STATUS_CTS_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_CTS;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_ISF_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_ISF) >> SERCOM_USART_STATUS_ISF_Pos;
}

static inline void hri_sercomusart_clear_STATUS_ISF_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_ISF;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_COLL_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_COLL) >> SERCOM_USART_STATUS_COLL_Pos;
}

static inline void hri_sercomusart_clear_STATUS_COLL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_COLL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_TXE_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_TXE) >> SERCOM_USART_STATUS_TXE_Pos;
}

static inline void hri_sercomusart_clear_STATUS_TXE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_TXE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomusart_get_STATUS_ITER_bit(const void *const hw)
{
	return (((Sercom *)hw)->USART.STATUS.reg & SERCOM_USART_STATUS_ITER) >> SERCOM_USART_STATUS_ITER_Pos;
}

static inline void hri_sercomusart_clear_STATUS_ITER_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = SERCOM_USART_STATUS_ITER;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_status_reg_t hri_sercomusart_get_STATUS_reg(const void *const            hw,
                                                                          hri_sercomusart_status_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->USART.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomusart_clear_STATUS_reg(const void *const hw, hri_sercomusart_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->USART.STATUS.reg = mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomusart_status_reg_t hri_sercomusart_read_STATUS_reg(const void *const hw)
{
	return ((Sercom *)hw)->USART.STATUS.reg;
}

static inline void hri_sercomi2cs_set_INTEN_PREC_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_PREC;
}

static inline bool hri_sercomi2cs_get_INTEN_PREC_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTENSET.reg & SERCOM_I2CS_INTENSET_PREC) >> SERCOM_I2CS_INTENSET_PREC_Pos;
}

static inline void hri_sercomi2cs_write_INTEN_PREC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_PREC;
	} else {
		((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_PREC;
	}
}

static inline void hri_sercomi2cs_clear_INTEN_PREC_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_PREC;
}

static inline void hri_sercomi2cs_set_INTEN_AMATCH_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_AMATCH;
}

static inline bool hri_sercomi2cs_get_INTEN_AMATCH_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTENSET.reg & SERCOM_I2CS_INTENSET_AMATCH) >> SERCOM_I2CS_INTENSET_AMATCH_Pos;
}

static inline void hri_sercomi2cs_write_INTEN_AMATCH_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_AMATCH;
	} else {
		((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_AMATCH;
	}
}

static inline void hri_sercomi2cs_clear_INTEN_AMATCH_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_AMATCH;
}

static inline void hri_sercomi2cs_set_INTEN_DRDY_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_DRDY;
}

static inline bool hri_sercomi2cs_get_INTEN_DRDY_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTENSET.reg & SERCOM_I2CS_INTENSET_DRDY) >> SERCOM_I2CS_INTENSET_DRDY_Pos;
}

static inline void hri_sercomi2cs_write_INTEN_DRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_DRDY;
	} else {
		((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_DRDY;
	}
}

static inline void hri_sercomi2cs_clear_INTEN_DRDY_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_DRDY;
}

static inline void hri_sercomi2cs_set_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_ERROR;
}

static inline bool hri_sercomi2cs_get_INTEN_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTENSET.reg & SERCOM_I2CS_INTENSET_ERROR) >> SERCOM_I2CS_INTENSET_ERROR_Pos;
}

static inline void hri_sercomi2cs_write_INTEN_ERROR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_ERROR;
	} else {
		((Sercom *)hw)->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_ERROR;
	}
}

static inline void hri_sercomi2cs_clear_INTEN_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTENCLR.reg = SERCOM_I2CS_INTENSET_ERROR;
}

static inline void hri_sercomi2cs_set_INTEN_reg(const void *const hw, hri_sercomi2cs_intenset_reg_t mask)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = mask;
}

static inline hri_sercomi2cs_intenset_reg_t hri_sercomi2cs_get_INTEN_reg(const void *const             hw,
                                                                         hri_sercomi2cs_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CS.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cs_intenset_reg_t hri_sercomi2cs_read_INTEN_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.INTENSET.reg;
}

static inline void hri_sercomi2cs_write_INTEN_reg(const void *const hw, hri_sercomi2cs_intenset_reg_t data)
{
	((Sercom *)hw)->I2CS.INTENSET.reg = data;
	((Sercom *)hw)->I2CS.INTENCLR.reg = ~data;
}

static inline void hri_sercomi2cs_clear_INTEN_reg(const void *const hw, hri_sercomi2cs_intenset_reg_t mask)
{
	((Sercom *)hw)->I2CS.INTENCLR.reg = mask;
}

static inline bool hri_sercomi2cs_get_INTFLAG_PREC_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC) >> SERCOM_I2CS_INTFLAG_PREC_Pos;
}

static inline void hri_sercomi2cs_clear_INTFLAG_PREC_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
}

static inline bool hri_sercomi2cs_get_INTFLAG_AMATCH_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH) >> SERCOM_I2CS_INTFLAG_AMATCH_Pos;
}

static inline void hri_sercomi2cs_clear_INTFLAG_AMATCH_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
}

static inline bool hri_sercomi2cs_get_INTFLAG_DRDY_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_DRDY) >> SERCOM_I2CS_INTFLAG_DRDY_Pos;
}

static inline void hri_sercomi2cs_clear_INTFLAG_DRDY_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_DRDY;
}

static inline bool hri_sercomi2cs_get_INTFLAG_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_ERROR) >> SERCOM_I2CS_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomi2cs_clear_INTFLAG_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR;
}

static inline bool hri_sercomi2cs_get_interrupt_PREC_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_PREC) >> SERCOM_I2CS_INTFLAG_PREC_Pos;
}

static inline void hri_sercomi2cs_clear_interrupt_PREC_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
}

static inline bool hri_sercomi2cs_get_interrupt_AMATCH_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_AMATCH) >> SERCOM_I2CS_INTFLAG_AMATCH_Pos;
}

static inline void hri_sercomi2cs_clear_interrupt_AMATCH_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
}

static inline bool hri_sercomi2cs_get_interrupt_DRDY_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_DRDY) >> SERCOM_I2CS_INTFLAG_DRDY_Pos;
}

static inline void hri_sercomi2cs_clear_interrupt_DRDY_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_DRDY;
}

static inline bool hri_sercomi2cs_get_interrupt_ERROR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.INTFLAG.reg & SERCOM_I2CS_INTFLAG_ERROR) >> SERCOM_I2CS_INTFLAG_ERROR_Pos;
}

static inline void hri_sercomi2cs_clear_interrupt_ERROR_bit(const void *const hw)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR;
}

static inline hri_sercomi2cs_intflag_reg_t hri_sercomi2cs_get_INTFLAG_reg(const void *const            hw,
                                                                          hri_sercomi2cs_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sercom *)hw)->I2CS.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cs_intflag_reg_t hri_sercomi2cs_read_INTFLAG_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.INTFLAG.reg;
}

static inline void hri_sercomi2cs_clear_INTFLAG_reg(const void *const hw, hri_sercomi2cs_intflag_reg_t mask)
{
	((Sercom *)hw)->I2CS.INTFLAG.reg = mask;
}

static inline void hri_sercomi2cs_set_CTRLA_SWRST_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_SWRST;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SWRST) >> SERCOM_I2CS_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_set_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_ENABLE) >> SERCOM_I2CS_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_ENABLE;
	tmp |= value << SERCOM_I2CS_CTRLA_ENABLE_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_ENABLE;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_RUNSTDBY) >> SERCOM_I2CS_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_RUNSTDBY;
	tmp |= value << SERCOM_I2CS_CTRLA_RUNSTDBY_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_RUNSTDBY;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_PINOUT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_PINOUT) >> SERCOM_I2CS_CTRLA_PINOUT_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_PINOUT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_PINOUT;
	tmp |= value << SERCOM_I2CS_CTRLA_PINOUT_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_PINOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_PINOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SEXTTOEN) >> SERCOM_I2CS_CTRLA_SEXTTOEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_SEXTTOEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_SEXTTOEN;
	tmp |= value << SERCOM_I2CS_CTRLA_SEXTTOEN_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_SEXTTOEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_SEXTTOEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_SCLSM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SCLSM) >> SERCOM_I2CS_CTRLA_SCLSM_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_SCLSM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_SCLSM;
	tmp |= value << SERCOM_I2CS_CTRLA_SCLSM_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_SCLSM_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_SCLSM;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_LOWTOUTEN) >> SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_LOWTOUTEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_LOWTOUTEN;
	tmp |= value << SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos;
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_LOWTOUTEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_LOWTOUTEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_get_CTRLA_MODE_bf(const void *const          hw,
                                                                          hri_sercomi2cs_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_MODE(mask)) >> SERCOM_I2CS_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_MODE_Msk;
	tmp |= SERCOM_I2CS_CTRLA_MODE(data);
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_MODE_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_MODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_read_CTRLA_MODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_MODE_Msk) >> SERCOM_I2CS_CTRLA_MODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_get_CTRLA_SDAHOLD_bf(const void *const          hw,
                                                                             hri_sercomi2cs_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SDAHOLD(mask)) >> SERCOM_I2CS_CTRLA_SDAHOLD_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_SDAHOLD_Msk;
	tmp |= SERCOM_I2CS_CTRLA_SDAHOLD(data);
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_SDAHOLD_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_SDAHOLD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_read_CTRLA_SDAHOLD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SDAHOLD_Msk) >> SERCOM_I2CS_CTRLA_SDAHOLD_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_get_CTRLA_SPEED_bf(const void *const          hw,
                                                                           hri_sercomi2cs_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SPEED(mask)) >> SERCOM_I2CS_CTRLA_SPEED_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= ~SERCOM_I2CS_CTRLA_SPEED_Msk;
	tmp |= SERCOM_I2CS_CTRLA_SPEED(data);
	((Sercom *)hw)->I2CS.CTRLA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~SERCOM_I2CS_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_SPEED_bf(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_SYNCBUSY_MASK);
	((Sercom *)hw)->I2CS.CTRLA.reg ^= SERCOM_I2CS_CTRLA_SPEED(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_read_CTRLA_SPEED_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLA_SPEED_Msk) >> SERCOM_I2CS_CTRLA_SPEED_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLA_reg(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLA.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_get_CTRLA_reg(const void *const          hw,
                                                                      hri_sercomi2cs_ctrla_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLA_reg(const void *const hw, hri_sercomi2cs_ctrla_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLA_reg(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLA.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLA_reg(const void *const hw, hri_sercomi2cs_ctrla_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLA.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrla_reg_t hri_sercomi2cs_read_CTRLA_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.CTRLA.reg;
}

static inline void hri_sercomi2cs_set_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLB_SMEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_SMEN) >> SERCOM_I2CS_CTRLB_SMEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_SMEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_SMEN;
	tmp |= value << SERCOM_I2CS_CTRLB_SMEN_Pos;
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_SMEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_SMEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLB_GCMD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_GCMD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLB_GCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_GCMD) >> SERCOM_I2CS_CTRLB_GCMD_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_GCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_GCMD;
	tmp |= value << SERCOM_I2CS_CTRLB_GCMD_Pos;
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_GCMD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_GCMD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_GCMD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_GCMD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLB_AACKEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_AACKEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLB_AACKEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_AACKEN) >> SERCOM_I2CS_CTRLB_AACKEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_AACKEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_AACKEN;
	tmp |= value << SERCOM_I2CS_CTRLB_AACKEN_Pos;
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_AACKEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_AACKEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_AACKEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_AACKEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLB_ACKACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_ACKACT) >> SERCOM_I2CS_CTRLB_ACKACT_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_ACKACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_ACKACT;
	tmp |= value << SERCOM_I2CS_CTRLB_ACKACT_Pos;
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_ACKACT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_ACKACT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLB_AMODE_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_get_CTRLB_AMODE_bf(const void *const          hw,
                                                                           hri_sercomi2cs_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_AMODE(mask)) >> SERCOM_I2CS_CTRLB_AMODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_AMODE_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_AMODE_Msk;
	tmp |= SERCOM_I2CS_CTRLB_AMODE(data);
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_AMODE_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_AMODE_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_AMODE(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_read_CTRLB_AMODE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_AMODE_Msk) >> SERCOM_I2CS_CTRLB_AMODE_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= SERCOM_I2CS_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_get_CTRLB_CMD_bf(const void *const          hw,
                                                                         hri_sercomi2cs_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_CMD(mask)) >> SERCOM_I2CS_CTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= ~SERCOM_I2CS_CTRLB_CMD_Msk;
	tmp |= SERCOM_I2CS_CTRLB_CMD(data);
	((Sercom *)hw)->I2CS.CTRLB.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~SERCOM_I2CS_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_CMD_bf(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= SERCOM_I2CS_CTRLB_CMD(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_read_CTRLB_CMD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLB_CMD_Msk) >> SERCOM_I2CS_CTRLB_CMD_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLB_reg(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_get_CTRLB_reg(const void *const          hw,
                                                                      hri_sercomi2cs_ctrlb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLB_reg(const void *const hw, hri_sercomi2cs_ctrlb_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLB_reg(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLB_reg(const void *const hw, hri_sercomi2cs_ctrlb_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLB.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlb_reg_t hri_sercomi2cs_read_CTRLB_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.CTRLB.reg;
}

static inline void hri_sercomi2cs_set_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg |= SERCOM_I2CS_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_CTRLC_DATA32B_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLC_DATA32B) >> SERCOM_I2CS_CTRLC_DATA32B_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_CTRLC_DATA32B_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp &= ~SERCOM_I2CS_CTRLC_DATA32B;
	tmp |= value << SERCOM_I2CS_CTRLC_DATA32B_Pos;
	((Sercom *)hw)->I2CS.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg &= ~SERCOM_I2CS_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLC_DATA32B_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg ^= SERCOM_I2CS_CTRLC_DATA32B;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_CTRLC_SDASETUP_bf(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg |= SERCOM_I2CS_CTRLC_SDASETUP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlc_reg_t hri_sercomi2cs_get_CTRLC_SDASETUP_bf(const void *const          hw,
                                                                              hri_sercomi2cs_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLC_SDASETUP(mask)) >> SERCOM_I2CS_CTRLC_SDASETUP_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLC_SDASETUP_bf(const void *const hw, hri_sercomi2cs_ctrlc_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp &= ~SERCOM_I2CS_CTRLC_SDASETUP_Msk;
	tmp |= SERCOM_I2CS_CTRLC_SDASETUP(data);
	((Sercom *)hw)->I2CS.CTRLC.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLC_SDASETUP_bf(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg &= ~SERCOM_I2CS_CTRLC_SDASETUP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLC_SDASETUP_bf(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg ^= SERCOM_I2CS_CTRLC_SDASETUP(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlc_reg_t hri_sercomi2cs_read_CTRLC_SDASETUP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp = (tmp & SERCOM_I2CS_CTRLC_SDASETUP_Msk) >> SERCOM_I2CS_CTRLC_SDASETUP_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_CTRLC_reg(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlc_reg_t hri_sercomi2cs_get_CTRLC_reg(const void *const          hw,
                                                                      hri_sercomi2cs_ctrlc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.CTRLC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_write_CTRLC_reg(const void *const hw, hri_sercomi2cs_ctrlc_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_CTRLC_reg(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_CTRLC_reg(const void *const hw, hri_sercomi2cs_ctrlc_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.CTRLC.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_ctrlc_reg_t hri_sercomi2cs_read_CTRLC_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.CTRLC.reg;
}

static inline void hri_sercomi2cs_set_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg |= SERCOM_I2CS_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_LENGTH_LENEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp = (tmp & SERCOM_I2CS_LENGTH_LENEN) >> SERCOM_I2CS_LENGTH_LENEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_LENGTH_LENEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp &= ~SERCOM_I2CS_LENGTH_LENEN;
	tmp |= value << SERCOM_I2CS_LENGTH_LENEN_Pos;
	((Sercom *)hw)->I2CS.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg &= ~SERCOM_I2CS_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_LENGTH_LENEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg ^= SERCOM_I2CS_LENGTH_LENEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_LENGTH_LEN_bf(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg |= SERCOM_I2CS_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_length_reg_t hri_sercomi2cs_get_LENGTH_LEN_bf(const void *const           hw,
                                                                           hri_sercomi2cs_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp = (tmp & SERCOM_I2CS_LENGTH_LEN(mask)) >> SERCOM_I2CS_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_LENGTH_LEN_bf(const void *const hw, hri_sercomi2cs_length_reg_t data)
{
	uint16_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp &= ~SERCOM_I2CS_LENGTH_LEN_Msk;
	tmp |= SERCOM_I2CS_LENGTH_LEN(data);
	((Sercom *)hw)->I2CS.LENGTH.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_LENGTH_LEN_bf(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg &= ~SERCOM_I2CS_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_LENGTH_LEN_bf(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg ^= SERCOM_I2CS_LENGTH_LEN(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_length_reg_t hri_sercomi2cs_read_LENGTH_LEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp = (tmp & SERCOM_I2CS_LENGTH_LEN_Msk) >> SERCOM_I2CS_LENGTH_LEN_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_LENGTH_reg(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_length_reg_t hri_sercomi2cs_get_LENGTH_reg(const void *const           hw,
                                                                        hri_sercomi2cs_length_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->I2CS.LENGTH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_write_LENGTH_reg(const void *const hw, hri_sercomi2cs_length_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_LENGTH_reg(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_LENGTH_reg(const void *const hw, hri_sercomi2cs_length_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.LENGTH.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_length_reg_t hri_sercomi2cs_read_LENGTH_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.LENGTH.reg;
}

static inline void hri_sercomi2cs_set_ADDR_GENCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg |= SERCOM_I2CS_ADDR_GENCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_ADDR_GENCEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_GENCEN) >> SERCOM_I2CS_ADDR_GENCEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_ADDR_GENCEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp &= ~SERCOM_I2CS_ADDR_GENCEN;
	tmp |= value << SERCOM_I2CS_ADDR_GENCEN_Pos;
	((Sercom *)hw)->I2CS.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_ADDR_GENCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg &= ~SERCOM_I2CS_ADDR_GENCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_ADDR_GENCEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg ^= SERCOM_I2CS_ADDR_GENCEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg |= SERCOM_I2CS_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_ADDR_TENBITEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_TENBITEN) >> SERCOM_I2CS_ADDR_TENBITEN_Pos;
	return (bool)tmp;
}

static inline void hri_sercomi2cs_write_ADDR_TENBITEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp &= ~SERCOM_I2CS_ADDR_TENBITEN;
	tmp |= value << SERCOM_I2CS_ADDR_TENBITEN_Pos;
	((Sercom *)hw)->I2CS.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg &= ~SERCOM_I2CS_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_ADDR_TENBITEN_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg ^= SERCOM_I2CS_ADDR_TENBITEN;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_set_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg |= SERCOM_I2CS_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_get_ADDR_ADDR_bf(const void *const         hw,
                                                                        hri_sercomi2cs_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_ADDR(mask)) >> SERCOM_I2CS_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cs_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp &= ~SERCOM_I2CS_ADDR_ADDR_Msk;
	tmp |= SERCOM_I2CS_ADDR_ADDR(data);
	((Sercom *)hw)->I2CS.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg &= ~SERCOM_I2CS_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_ADDR_ADDR_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg ^= SERCOM_I2CS_ADDR_ADDR(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_ADDR_Msk) >> SERCOM_I2CS_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg |= SERCOM_I2CS_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_get_ADDR_ADDRMASK_bf(const void *const         hw,
                                                                            hri_sercomi2cs_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_ADDRMASK(mask)) >> SERCOM_I2CS_ADDR_ADDRMASK_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomi2cs_addr_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp &= ~SERCOM_I2CS_ADDR_ADDRMASK_Msk;
	tmp |= SERCOM_I2CS_ADDR_ADDRMASK(data);
	((Sercom *)hw)->I2CS.ADDR.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg &= ~SERCOM_I2CS_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_ADDR_ADDRMASK_bf(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg ^= SERCOM_I2CS_ADDR_ADDRMASK(mask);
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_read_ADDR_ADDRMASK_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp = (tmp & SERCOM_I2CS_ADDR_ADDRMASK_Msk) >> SERCOM_I2CS_ADDR_ADDRMASK_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_set_ADDR_reg(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg |= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_get_ADDR_reg(const void *const         hw,
                                                                    hri_sercomi2cs_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_write_ADDR_reg(const void *const hw, hri_sercomi2cs_addr_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_clear_ADDR_reg(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg &= ~mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sercomi2cs_toggle_ADDR_reg(const void *const hw, hri_sercomi2cs_addr_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.ADDR.reg ^= mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_addr_reg_t hri_sercomi2cs_read_ADDR_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.ADDR.reg;
}

static inline hri_sercomi2cs_data_reg_t hri_sercomi2cs_read_DATA_DATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.DATA.reg;
	tmp = (tmp & SERCOM_I2CS_DATA_DATA_Msk) >> SERCOM_I2CS_DATA_DATA_Pos;
	return tmp;
}

static inline void hri_sercomi2cs_write_DATA_DATA_bf(const void *const hw, hri_sercomi2cs_data_reg_t data)
{
	uint32_t tmp;
	SERCOM_CRITICAL_SECTION_ENTER();
	tmp = ((Sercom *)hw)->I2CS.DATA.reg;
	tmp &= ~SERCOM_I2CS_DATA_DATA_Msk;
	tmp |= SERCOM_I2CS_DATA_DATA(data);
	((Sercom *)hw)->I2CS.DATA.reg = tmp;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_data_reg_t hri_sercomi2cs_read_DATA_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.DATA.reg;
}

static inline void hri_sercomi2cs_write_DATA_reg(const void *const hw, hri_sercomi2cs_data_reg_t data)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.DATA.reg = data;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.SYNCBUSY.reg & SERCOM_I2CS_SYNCBUSY_SWRST) >> SERCOM_I2CS_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_sercomi2cs_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.SYNCBUSY.reg & SERCOM_I2CS_SYNCBUSY_ENABLE) >> SERCOM_I2CS_SYNCBUSY_ENABLE_Pos;
}

static inline bool hri_sercomi2cs_get_SYNCBUSY_LENGTH_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.SYNCBUSY.reg & SERCOM_I2CS_SYNCBUSY_LENGTH) >> SERCOM_I2CS_SYNCBUSY_LENGTH_Pos;
}

static inline hri_sercomi2cs_syncbusy_reg_t hri_sercomi2cs_get_SYNCBUSY_reg(const void *const             hw,
                                                                            hri_sercomi2cs_syncbusy_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sercom *)hw)->I2CS.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sercomi2cs_syncbusy_reg_t hri_sercomi2cs_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.SYNCBUSY.reg;
}

static inline bool hri_sercomi2cs_get_STATUS_BUSERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_BUSERR) >> SERCOM_I2CS_STATUS_BUSERR_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_BUSERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_BUSERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_COLL_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_COLL) >> SERCOM_I2CS_STATUS_COLL_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_COLL_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_COLL;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_RXNACK_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_RXNACK) >> SERCOM_I2CS_STATUS_RXNACK_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_RXNACK_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_RXNACK;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_DIR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_DIR) >> SERCOM_I2CS_STATUS_DIR_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_DIR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_DIR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_SR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_SR) >> SERCOM_I2CS_STATUS_SR_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_SR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_SR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_LOWTOUT_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_LOWTOUT) >> SERCOM_I2CS_STATUS_LOWTOUT_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_LOWTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_LOWTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_CLKHOLD_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_CLKHOLD) >> SERCOM_I2CS_STATUS_CLKHOLD_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_CLKHOLD_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_CLKHOLD;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_SEXTTOUT_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_SEXTTOUT) >> SERCOM_I2CS_STATUS_SEXTTOUT_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_SEXTTOUT_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_SEXTTOUT;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_HS_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_HS) >> SERCOM_I2CS_STATUS_HS_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_HS_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_HS;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sercomi2cs_get_STATUS_LENERR_bit(const void *const hw)
{
	return (((Sercom *)hw)->I2CS.STATUS.reg & SERCOM_I2CS_STATUS_LENERR) >> SERCOM_I2CS_STATUS_LENERR_Pos;
}

static inline void hri_sercomi2cs_clear_STATUS_LENERR_bit(const void *const hw)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_LENERR;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_status_reg_t hri_sercomi2cs_get_STATUS_reg(const void *const           hw,
                                                                        hri_sercomi2cs_status_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sercom *)hw)->I2CS.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sercomi2cs_clear_STATUS_reg(const void *const hw, hri_sercomi2cs_status_reg_t mask)
{
	SERCOM_CRITICAL_SECTION_ENTER();
	((Sercom *)hw)->I2CS.STATUS.reg = mask;
	SERCOM_CRITICAL_SECTION_LEAVE();
}

static inline hri_sercomi2cs_status_reg_t hri_sercomi2cs_read_STATUS_reg(const void *const hw)
{
	return ((Sercom *)hw)->I2CS.STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SERCOM_D51_H_INCLUDED */
#endif /* _SAMD51_SERCOM_COMPONENT_ */
