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

#ifdef _SAMD51_PAC_COMPONENT_
#ifndef _HRI_PAC_D51_H_INCLUDED_
#define _HRI_PAC_D51_H_INCLUDED_

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

typedef uint32_t hri_pac_intflaga_reg_t;
typedef uint32_t hri_pac_intflagahb_reg_t;
typedef uint32_t hri_pac_intflagb_reg_t;
typedef uint32_t hri_pac_intflagc_reg_t;
typedef uint32_t hri_pac_intflagd_reg_t;
typedef uint32_t hri_pac_statusa_reg_t;
typedef uint32_t hri_pac_statusb_reg_t;
typedef uint32_t hri_pac_statusc_reg_t;
typedef uint32_t hri_pac_statusd_reg_t;
typedef uint32_t hri_pac_wrctrl_reg_t;
typedef uint8_t  hri_pac_evctrl_reg_t;
typedef uint8_t  hri_pac_intenset_reg_t;

static inline void hri_pac_set_INTEN_ERR_bit(const void *const hw)
{
	((Pac *)hw)->INTENSET.reg = PAC_INTENSET_ERR;
}

static inline bool hri_pac_get_INTEN_ERR_bit(const void *const hw)
{
	return (((Pac *)hw)->INTENSET.reg & PAC_INTENSET_ERR) >> PAC_INTENSET_ERR_Pos;
}

static inline void hri_pac_write_INTEN_ERR_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pac *)hw)->INTENCLR.reg = PAC_INTENSET_ERR;
	} else {
		((Pac *)hw)->INTENSET.reg = PAC_INTENSET_ERR;
	}
}

static inline void hri_pac_clear_INTEN_ERR_bit(const void *const hw)
{
	((Pac *)hw)->INTENCLR.reg = PAC_INTENSET_ERR;
}

static inline void hri_pac_set_INTEN_reg(const void *const hw, hri_pac_intenset_reg_t mask)
{
	((Pac *)hw)->INTENSET.reg = mask;
}

static inline hri_pac_intenset_reg_t hri_pac_get_INTEN_reg(const void *const hw, hri_pac_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intenset_reg_t hri_pac_read_INTEN_reg(const void *const hw)
{
	return ((Pac *)hw)->INTENSET.reg;
}

static inline void hri_pac_write_INTEN_reg(const void *const hw, hri_pac_intenset_reg_t data)
{
	((Pac *)hw)->INTENSET.reg = data;
	((Pac *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_pac_clear_INTEN_reg(const void *const hw, hri_pac_intenset_reg_t mask)
{
	((Pac *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_pac_get_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_FLASH) >> PAC_INTFLAGAHB_FLASH_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_FLASH_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_FLASH;
}

static inline bool hri_pac_get_INTFLAGAHB_FLASH_ALT_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_FLASH_ALT) >> PAC_INTFLAGAHB_FLASH_ALT_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_FLASH_ALT_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_FLASH_ALT;
}

static inline bool hri_pac_get_INTFLAGAHB_SEEPROM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_SEEPROM) >> PAC_INTFLAGAHB_SEEPROM_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_SEEPROM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_SEEPROM;
}

static inline bool hri_pac_get_INTFLAGAHB_RAMCM4S_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_RAMCM4S) >> PAC_INTFLAGAHB_RAMCM4S_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_RAMCM4S_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_RAMCM4S;
}

static inline bool hri_pac_get_INTFLAGAHB_RAMPPPDSU_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_RAMPPPDSU) >> PAC_INTFLAGAHB_RAMPPPDSU_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_RAMPPPDSU_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_RAMPPPDSU;
}

static inline bool hri_pac_get_INTFLAGAHB_RAMDMAWR_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_RAMDMAWR) >> PAC_INTFLAGAHB_RAMDMAWR_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_RAMDMAWR_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_RAMDMAWR;
}

static inline bool hri_pac_get_INTFLAGAHB_RAMDMACICM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_RAMDMACICM) >> PAC_INTFLAGAHB_RAMDMACICM_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_RAMDMACICM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_RAMDMACICM;
}

static inline bool hri_pac_get_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_HPB0) >> PAC_INTFLAGAHB_HPB0_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_HPB0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_HPB0;
}

static inline bool hri_pac_get_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_HPB1) >> PAC_INTFLAGAHB_HPB1_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_HPB1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_HPB1;
}

static inline bool hri_pac_get_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_HPB2) >> PAC_INTFLAGAHB_HPB2_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_HPB2_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_HPB2;
}

static inline bool hri_pac_get_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_HPB3) >> PAC_INTFLAGAHB_HPB3_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_HPB3_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_HPB3;
}

static inline bool hri_pac_get_INTFLAGAHB_PUKCC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_PUKCC) >> PAC_INTFLAGAHB_PUKCC_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_PUKCC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_PUKCC;
}

static inline bool hri_pac_get_INTFLAGAHB_SDHC0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_SDHC0) >> PAC_INTFLAGAHB_SDHC0_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_SDHC0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_SDHC0;
}

static inline bool hri_pac_get_INTFLAGAHB_SDHC1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_SDHC1) >> PAC_INTFLAGAHB_SDHC1_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_SDHC1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_SDHC1;
}

static inline bool hri_pac_get_INTFLAGAHB_QSPI_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_QSPI) >> PAC_INTFLAGAHB_QSPI_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_QSPI_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_QSPI;
}

static inline bool hri_pac_get_INTFLAGAHB_BKUPRAM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGAHB.reg & PAC_INTFLAGAHB_BKUPRAM) >> PAC_INTFLAGAHB_BKUPRAM_Pos;
}

static inline void hri_pac_clear_INTFLAGAHB_BKUPRAM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGAHB.reg = PAC_INTFLAGAHB_BKUPRAM;
}

static inline hri_pac_intflagahb_reg_t hri_pac_get_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGAHB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intflagahb_reg_t hri_pac_read_INTFLAGAHB_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGAHB.reg;
}

static inline void hri_pac_clear_INTFLAGAHB_reg(const void *const hw, hri_pac_intflagahb_reg_t mask)
{
	((Pac *)hw)->INTFLAGAHB.reg = mask;
}

static inline bool hri_pac_get_INTFLAGA_PAC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_PAC) >> PAC_INTFLAGA_PAC_Pos;
}

static inline void hri_pac_clear_INTFLAGA_PAC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_PAC;
}

static inline bool hri_pac_get_INTFLAGA_PM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_PM) >> PAC_INTFLAGA_PM_Pos;
}

static inline void hri_pac_clear_INTFLAGA_PM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_PM;
}

static inline bool hri_pac_get_INTFLAGA_MCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_MCLK) >> PAC_INTFLAGA_MCLK_Pos;
}

static inline void hri_pac_clear_INTFLAGA_MCLK_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_MCLK;
}

static inline bool hri_pac_get_INTFLAGA_RSTC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_RSTC) >> PAC_INTFLAGA_RSTC_Pos;
}

static inline void hri_pac_clear_INTFLAGA_RSTC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_RSTC;
}

static inline bool hri_pac_get_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_OSCCTRL) >> PAC_INTFLAGA_OSCCTRL_Pos;
}

static inline void hri_pac_clear_INTFLAGA_OSCCTRL_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_OSCCTRL;
}

static inline bool hri_pac_get_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_OSC32KCTRL) >> PAC_INTFLAGA_OSC32KCTRL_Pos;
}

static inline void hri_pac_clear_INTFLAGA_OSC32KCTRL_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_OSC32KCTRL;
}

static inline bool hri_pac_get_INTFLAGA_SUPC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_SUPC) >> PAC_INTFLAGA_SUPC_Pos;
}

static inline void hri_pac_clear_INTFLAGA_SUPC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_SUPC;
}

static inline bool hri_pac_get_INTFLAGA_GCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_GCLK) >> PAC_INTFLAGA_GCLK_Pos;
}

static inline void hri_pac_clear_INTFLAGA_GCLK_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_GCLK;
}

static inline bool hri_pac_get_INTFLAGA_WDT_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_WDT) >> PAC_INTFLAGA_WDT_Pos;
}

static inline void hri_pac_clear_INTFLAGA_WDT_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_WDT;
}

static inline bool hri_pac_get_INTFLAGA_RTC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_RTC) >> PAC_INTFLAGA_RTC_Pos;
}

static inline void hri_pac_clear_INTFLAGA_RTC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_RTC;
}

static inline bool hri_pac_get_INTFLAGA_EIC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_EIC) >> PAC_INTFLAGA_EIC_Pos;
}

static inline void hri_pac_clear_INTFLAGA_EIC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_EIC;
}

static inline bool hri_pac_get_INTFLAGA_FREQM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_FREQM) >> PAC_INTFLAGA_FREQM_Pos;
}

static inline void hri_pac_clear_INTFLAGA_FREQM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_FREQM;
}

static inline bool hri_pac_get_INTFLAGA_SERCOM0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_SERCOM0) >> PAC_INTFLAGA_SERCOM0_Pos;
}

static inline void hri_pac_clear_INTFLAGA_SERCOM0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_SERCOM0;
}

static inline bool hri_pac_get_INTFLAGA_SERCOM1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_SERCOM1) >> PAC_INTFLAGA_SERCOM1_Pos;
}

static inline void hri_pac_clear_INTFLAGA_SERCOM1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_SERCOM1;
}

static inline bool hri_pac_get_INTFLAGA_TC0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_TC0) >> PAC_INTFLAGA_TC0_Pos;
}

static inline void hri_pac_clear_INTFLAGA_TC0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_TC0;
}

static inline bool hri_pac_get_INTFLAGA_TC1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGA.reg & PAC_INTFLAGA_TC1) >> PAC_INTFLAGA_TC1_Pos;
}

static inline void hri_pac_clear_INTFLAGA_TC1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGA.reg = PAC_INTFLAGA_TC1;
}

static inline hri_pac_intflaga_reg_t hri_pac_get_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intflaga_reg_t hri_pac_read_INTFLAGA_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGA.reg;
}

static inline void hri_pac_clear_INTFLAGA_reg(const void *const hw, hri_pac_intflaga_reg_t mask)
{
	((Pac *)hw)->INTFLAGA.reg = mask;
}

static inline bool hri_pac_get_INTFLAGB_USB_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_USB) >> PAC_INTFLAGB_USB_Pos;
}

static inline void hri_pac_clear_INTFLAGB_USB_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_USB;
}

static inline bool hri_pac_get_INTFLAGB_DSU_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_DSU) >> PAC_INTFLAGB_DSU_Pos;
}

static inline void hri_pac_clear_INTFLAGB_DSU_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_DSU;
}

static inline bool hri_pac_get_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_NVMCTRL) >> PAC_INTFLAGB_NVMCTRL_Pos;
}

static inline void hri_pac_clear_INTFLAGB_NVMCTRL_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_NVMCTRL;
}

static inline bool hri_pac_get_INTFLAGB_CMCC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_CMCC) >> PAC_INTFLAGB_CMCC_Pos;
}

static inline void hri_pac_clear_INTFLAGB_CMCC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_CMCC;
}

static inline bool hri_pac_get_INTFLAGB_PORT_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_PORT) >> PAC_INTFLAGB_PORT_Pos;
}

static inline void hri_pac_clear_INTFLAGB_PORT_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_PORT;
}

static inline bool hri_pac_get_INTFLAGB_DMAC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_DMAC) >> PAC_INTFLAGB_DMAC_Pos;
}

static inline void hri_pac_clear_INTFLAGB_DMAC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_DMAC;
}

static inline bool hri_pac_get_INTFLAGB_HMATRIX_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_HMATRIX) >> PAC_INTFLAGB_HMATRIX_Pos;
}

static inline void hri_pac_clear_INTFLAGB_HMATRIX_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_HMATRIX;
}

static inline bool hri_pac_get_INTFLAGB_EVSYS_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_EVSYS) >> PAC_INTFLAGB_EVSYS_Pos;
}

static inline void hri_pac_clear_INTFLAGB_EVSYS_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_EVSYS;
}

static inline bool hri_pac_get_INTFLAGB_SERCOM2_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_SERCOM2) >> PAC_INTFLAGB_SERCOM2_Pos;
}

static inline void hri_pac_clear_INTFLAGB_SERCOM2_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_SERCOM2;
}

static inline bool hri_pac_get_INTFLAGB_SERCOM3_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_SERCOM3) >> PAC_INTFLAGB_SERCOM3_Pos;
}

static inline void hri_pac_clear_INTFLAGB_SERCOM3_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_SERCOM3;
}

static inline bool hri_pac_get_INTFLAGB_TCC0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_TCC0) >> PAC_INTFLAGB_TCC0_Pos;
}

static inline void hri_pac_clear_INTFLAGB_TCC0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_TCC0;
}

static inline bool hri_pac_get_INTFLAGB_TCC1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_TCC1) >> PAC_INTFLAGB_TCC1_Pos;
}

static inline void hri_pac_clear_INTFLAGB_TCC1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_TCC1;
}

static inline bool hri_pac_get_INTFLAGB_TC2_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_TC2) >> PAC_INTFLAGB_TC2_Pos;
}

static inline void hri_pac_clear_INTFLAGB_TC2_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_TC2;
}

static inline bool hri_pac_get_INTFLAGB_TC3_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_TC3) >> PAC_INTFLAGB_TC3_Pos;
}

static inline void hri_pac_clear_INTFLAGB_TC3_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_TC3;
}

static inline bool hri_pac_get_INTFLAGB_TAL_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_TAL) >> PAC_INTFLAGB_TAL_Pos;
}

static inline void hri_pac_clear_INTFLAGB_TAL_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_TAL;
}

static inline bool hri_pac_get_INTFLAGB_RAMECC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGB.reg & PAC_INTFLAGB_RAMECC) >> PAC_INTFLAGB_RAMECC_Pos;
}

static inline void hri_pac_clear_INTFLAGB_RAMECC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGB.reg = PAC_INTFLAGB_RAMECC;
}

static inline hri_pac_intflagb_reg_t hri_pac_get_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intflagb_reg_t hri_pac_read_INTFLAGB_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGB.reg;
}

static inline void hri_pac_clear_INTFLAGB_reg(const void *const hw, hri_pac_intflagb_reg_t mask)
{
	((Pac *)hw)->INTFLAGB.reg = mask;
}

static inline bool hri_pac_get_INTFLAGC_TCC2_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_TCC2) >> PAC_INTFLAGC_TCC2_Pos;
}

static inline void hri_pac_clear_INTFLAGC_TCC2_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_TCC2;
}

static inline bool hri_pac_get_INTFLAGC_TCC3_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_TCC3) >> PAC_INTFLAGC_TCC3_Pos;
}

static inline void hri_pac_clear_INTFLAGC_TCC3_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_TCC3;
}

static inline bool hri_pac_get_INTFLAGC_TC4_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_TC4) >> PAC_INTFLAGC_TC4_Pos;
}

static inline void hri_pac_clear_INTFLAGC_TC4_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_TC4;
}

static inline bool hri_pac_get_INTFLAGC_TC5_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_TC5) >> PAC_INTFLAGC_TC5_Pos;
}

static inline void hri_pac_clear_INTFLAGC_TC5_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_TC5;
}

static inline bool hri_pac_get_INTFLAGC_PDEC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_PDEC) >> PAC_INTFLAGC_PDEC_Pos;
}

static inline void hri_pac_clear_INTFLAGC_PDEC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_PDEC;
}

static inline bool hri_pac_get_INTFLAGC_AC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_AC) >> PAC_INTFLAGC_AC_Pos;
}

static inline void hri_pac_clear_INTFLAGC_AC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_AC;
}

static inline bool hri_pac_get_INTFLAGC_AES_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_AES) >> PAC_INTFLAGC_AES_Pos;
}

static inline void hri_pac_clear_INTFLAGC_AES_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_AES;
}

static inline bool hri_pac_get_INTFLAGC_TRNG_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_TRNG) >> PAC_INTFLAGC_TRNG_Pos;
}

static inline void hri_pac_clear_INTFLAGC_TRNG_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_TRNG;
}

static inline bool hri_pac_get_INTFLAGC_ICM_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_ICM) >> PAC_INTFLAGC_ICM_Pos;
}

static inline void hri_pac_clear_INTFLAGC_ICM_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_ICM;
}

static inline bool hri_pac_get_INTFLAGC_PUKCC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_PUKCC) >> PAC_INTFLAGC_PUKCC_Pos;
}

static inline void hri_pac_clear_INTFLAGC_PUKCC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_PUKCC;
}

static inline bool hri_pac_get_INTFLAGC_QSPI_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_QSPI) >> PAC_INTFLAGC_QSPI_Pos;
}

static inline void hri_pac_clear_INTFLAGC_QSPI_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_QSPI;
}

static inline bool hri_pac_get_INTFLAGC_CCL_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGC.reg & PAC_INTFLAGC_CCL) >> PAC_INTFLAGC_CCL_Pos;
}

static inline void hri_pac_clear_INTFLAGC_CCL_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGC.reg = PAC_INTFLAGC_CCL;
}

static inline hri_pac_intflagc_reg_t hri_pac_get_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGC.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intflagc_reg_t hri_pac_read_INTFLAGC_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGC.reg;
}

static inline void hri_pac_clear_INTFLAGC_reg(const void *const hw, hri_pac_intflagc_reg_t mask)
{
	((Pac *)hw)->INTFLAGC.reg = mask;
}

static inline bool hri_pac_get_INTFLAGD_SERCOM4_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_SERCOM4) >> PAC_INTFLAGD_SERCOM4_Pos;
}

static inline void hri_pac_clear_INTFLAGD_SERCOM4_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_SERCOM4;
}

static inline bool hri_pac_get_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_SERCOM5) >> PAC_INTFLAGD_SERCOM5_Pos;
}

static inline void hri_pac_clear_INTFLAGD_SERCOM5_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_SERCOM5;
}

static inline bool hri_pac_get_INTFLAGD_SERCOM6_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_SERCOM6) >> PAC_INTFLAGD_SERCOM6_Pos;
}

static inline void hri_pac_clear_INTFLAGD_SERCOM6_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_SERCOM6;
}

static inline bool hri_pac_get_INTFLAGD_SERCOM7_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_SERCOM7) >> PAC_INTFLAGD_SERCOM7_Pos;
}

static inline void hri_pac_clear_INTFLAGD_SERCOM7_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_SERCOM7;
}

static inline bool hri_pac_get_INTFLAGD_TCC4_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_TCC4) >> PAC_INTFLAGD_TCC4_Pos;
}

static inline void hri_pac_clear_INTFLAGD_TCC4_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_TCC4;
}

static inline bool hri_pac_get_INTFLAGD_TC6_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_TC6) >> PAC_INTFLAGD_TC6_Pos;
}

static inline void hri_pac_clear_INTFLAGD_TC6_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_TC6;
}

static inline bool hri_pac_get_INTFLAGD_TC7_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_TC7) >> PAC_INTFLAGD_TC7_Pos;
}

static inline void hri_pac_clear_INTFLAGD_TC7_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_TC7;
}

static inline bool hri_pac_get_INTFLAGD_ADC0_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_ADC0) >> PAC_INTFLAGD_ADC0_Pos;
}

static inline void hri_pac_clear_INTFLAGD_ADC0_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_ADC0;
}

static inline bool hri_pac_get_INTFLAGD_ADC1_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_ADC1) >> PAC_INTFLAGD_ADC1_Pos;
}

static inline void hri_pac_clear_INTFLAGD_ADC1_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_ADC1;
}

static inline bool hri_pac_get_INTFLAGD_DAC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_DAC) >> PAC_INTFLAGD_DAC_Pos;
}

static inline void hri_pac_clear_INTFLAGD_DAC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_DAC;
}

static inline bool hri_pac_get_INTFLAGD_I2S_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_I2S) >> PAC_INTFLAGD_I2S_Pos;
}

static inline void hri_pac_clear_INTFLAGD_I2S_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_I2S;
}

static inline bool hri_pac_get_INTFLAGD_PCC_bit(const void *const hw)
{
	return (((Pac *)hw)->INTFLAGD.reg & PAC_INTFLAGD_PCC) >> PAC_INTFLAGD_PCC_Pos;
}

static inline void hri_pac_clear_INTFLAGD_PCC_bit(const void *const hw)
{
	((Pac *)hw)->INTFLAGD.reg = PAC_INTFLAGD_PCC;
}

static inline hri_pac_intflagd_reg_t hri_pac_get_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->INTFLAGD.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_intflagd_reg_t hri_pac_read_INTFLAGD_reg(const void *const hw)
{
	return ((Pac *)hw)->INTFLAGD.reg;
}

static inline void hri_pac_clear_INTFLAGD_reg(const void *const hw, hri_pac_intflagd_reg_t mask)
{
	((Pac *)hw)->INTFLAGD.reg = mask;
}

static inline void hri_pac_set_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_PERID(mask)) >> PAC_WRCTRL_PERID_Pos;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= ~PAC_WRCTRL_PERID_Msk;
	tmp |= PAC_WRCTRL_PERID(data);
	((Pac *)hw)->WRCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_PERID_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= PAC_WRCTRL_PERID(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_PERID_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_PERID_Msk) >> PAC_WRCTRL_PERID_Pos;
	return tmp;
}

static inline void hri_pac_set_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_KEY(mask)) >> PAC_WRCTRL_KEY_Pos;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	uint32_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= ~PAC_WRCTRL_KEY_Msk;
	tmp |= PAC_WRCTRL_KEY(data);
	((Pac *)hw)->WRCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_KEY_bf(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= PAC_WRCTRL_KEY(mask);
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_KEY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp = (tmp & PAC_WRCTRL_KEY_Msk) >> PAC_WRCTRL_KEY_Pos;
	return tmp;
}

static inline void hri_pac_set_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_get_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->WRCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_WRCTRL_reg(const void *const hw, hri_pac_wrctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->WRCTRL.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_wrctrl_reg_t hri_pac_read_WRCTRL_reg(const void *const hw)
{
	return ((Pac *)hw)->WRCTRL.reg;
}

static inline void hri_pac_set_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg |= PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pac_get_EVCTRL_ERREO_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp = (tmp & PAC_EVCTRL_ERREO) >> PAC_EVCTRL_ERREO_Pos;
	return (bool)tmp;
}

static inline void hri_pac_write_EVCTRL_ERREO_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PAC_CRITICAL_SECTION_ENTER();
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp &= ~PAC_EVCTRL_ERREO;
	tmp |= value << PAC_EVCTRL_ERREO_Pos;
	((Pac *)hw)->EVCTRL.reg = tmp;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg &= ~PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_EVCTRL_ERREO_bit(const void *const hw)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg ^= PAC_EVCTRL_ERREO;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_set_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg |= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_evctrl_reg_t hri_pac_get_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pac *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pac_write_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t data)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg = data;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_clear_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg &= ~mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pac_toggle_EVCTRL_reg(const void *const hw, hri_pac_evctrl_reg_t mask)
{
	PAC_CRITICAL_SECTION_ENTER();
	((Pac *)hw)->EVCTRL.reg ^= mask;
	PAC_CRITICAL_SECTION_LEAVE();
}

static inline hri_pac_evctrl_reg_t hri_pac_read_EVCTRL_reg(const void *const hw)
{
	return ((Pac *)hw)->EVCTRL.reg;
}

static inline bool hri_pac_get_STATUSA_PAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_PAC) >> PAC_STATUSA_PAC_Pos;
}

static inline bool hri_pac_get_STATUSA_PM_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_PM) >> PAC_STATUSA_PM_Pos;
}

static inline bool hri_pac_get_STATUSA_MCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_MCLK) >> PAC_STATUSA_MCLK_Pos;
}

static inline bool hri_pac_get_STATUSA_RSTC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_RSTC) >> PAC_STATUSA_RSTC_Pos;
}

static inline bool hri_pac_get_STATUSA_OSCCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_OSCCTRL) >> PAC_STATUSA_OSCCTRL_Pos;
}

static inline bool hri_pac_get_STATUSA_OSC32KCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_OSC32KCTRL) >> PAC_STATUSA_OSC32KCTRL_Pos;
}

static inline bool hri_pac_get_STATUSA_SUPC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_SUPC) >> PAC_STATUSA_SUPC_Pos;
}

static inline bool hri_pac_get_STATUSA_GCLK_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_GCLK) >> PAC_STATUSA_GCLK_Pos;
}

static inline bool hri_pac_get_STATUSA_WDT_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_WDT) >> PAC_STATUSA_WDT_Pos;
}

static inline bool hri_pac_get_STATUSA_RTC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_RTC) >> PAC_STATUSA_RTC_Pos;
}

static inline bool hri_pac_get_STATUSA_EIC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_EIC) >> PAC_STATUSA_EIC_Pos;
}

static inline bool hri_pac_get_STATUSA_FREQM_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_FREQM) >> PAC_STATUSA_FREQM_Pos;
}

static inline bool hri_pac_get_STATUSA_SERCOM0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_SERCOM0) >> PAC_STATUSA_SERCOM0_Pos;
}

static inline bool hri_pac_get_STATUSA_SERCOM1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_SERCOM1) >> PAC_STATUSA_SERCOM1_Pos;
}

static inline bool hri_pac_get_STATUSA_TC0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_TC0) >> PAC_STATUSA_TC0_Pos;
}

static inline bool hri_pac_get_STATUSA_TC1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSA.reg & PAC_STATUSA_TC1) >> PAC_STATUSA_TC1_Pos;
}

static inline hri_pac_statusa_reg_t hri_pac_get_STATUSA_reg(const void *const hw, hri_pac_statusa_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSA.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusa_reg_t hri_pac_read_STATUSA_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSA.reg;
}

static inline bool hri_pac_get_STATUSB_USB_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_USB) >> PAC_STATUSB_USB_Pos;
}

static inline bool hri_pac_get_STATUSB_DSU_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_DSU) >> PAC_STATUSB_DSU_Pos;
}

static inline bool hri_pac_get_STATUSB_NVMCTRL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_NVMCTRL) >> PAC_STATUSB_NVMCTRL_Pos;
}

static inline bool hri_pac_get_STATUSB_CMCC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_CMCC) >> PAC_STATUSB_CMCC_Pos;
}

static inline bool hri_pac_get_STATUSB_PORT_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_PORT) >> PAC_STATUSB_PORT_Pos;
}

static inline bool hri_pac_get_STATUSB_DMAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_DMAC) >> PAC_STATUSB_DMAC_Pos;
}

static inline bool hri_pac_get_STATUSB_HMATRIX_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_HMATRIX) >> PAC_STATUSB_HMATRIX_Pos;
}

static inline bool hri_pac_get_STATUSB_EVSYS_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_EVSYS) >> PAC_STATUSB_EVSYS_Pos;
}

static inline bool hri_pac_get_STATUSB_SERCOM2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_SERCOM2) >> PAC_STATUSB_SERCOM2_Pos;
}

static inline bool hri_pac_get_STATUSB_SERCOM3_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_SERCOM3) >> PAC_STATUSB_SERCOM3_Pos;
}

static inline bool hri_pac_get_STATUSB_TCC0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_TCC0) >> PAC_STATUSB_TCC0_Pos;
}

static inline bool hri_pac_get_STATUSB_TCC1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_TCC1) >> PAC_STATUSB_TCC1_Pos;
}

static inline bool hri_pac_get_STATUSB_TC2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_TC2) >> PAC_STATUSB_TC2_Pos;
}

static inline bool hri_pac_get_STATUSB_TC3_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_TC3) >> PAC_STATUSB_TC3_Pos;
}

static inline bool hri_pac_get_STATUSB_TAL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_TAL) >> PAC_STATUSB_TAL_Pos;
}

static inline bool hri_pac_get_STATUSB_RAMECC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSB.reg & PAC_STATUSB_RAMECC) >> PAC_STATUSB_RAMECC_Pos;
}

static inline hri_pac_statusb_reg_t hri_pac_get_STATUSB_reg(const void *const hw, hri_pac_statusb_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusb_reg_t hri_pac_read_STATUSB_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSB.reg;
}

static inline bool hri_pac_get_STATUSC_TCC2_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TCC2) >> PAC_STATUSC_TCC2_Pos;
}

static inline bool hri_pac_get_STATUSC_TCC3_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TCC3) >> PAC_STATUSC_TCC3_Pos;
}

static inline bool hri_pac_get_STATUSC_TC4_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC4) >> PAC_STATUSC_TC4_Pos;
}

static inline bool hri_pac_get_STATUSC_TC5_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TC5) >> PAC_STATUSC_TC5_Pos;
}

static inline bool hri_pac_get_STATUSC_PDEC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_PDEC) >> PAC_STATUSC_PDEC_Pos;
}

static inline bool hri_pac_get_STATUSC_AC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_AC) >> PAC_STATUSC_AC_Pos;
}

static inline bool hri_pac_get_STATUSC_AES_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_AES) >> PAC_STATUSC_AES_Pos;
}

static inline bool hri_pac_get_STATUSC_TRNG_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_TRNG) >> PAC_STATUSC_TRNG_Pos;
}

static inline bool hri_pac_get_STATUSC_ICM_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_ICM) >> PAC_STATUSC_ICM_Pos;
}

static inline bool hri_pac_get_STATUSC_PUKCC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_PUKCC) >> PAC_STATUSC_PUKCC_Pos;
}

static inline bool hri_pac_get_STATUSC_QSPI_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_QSPI) >> PAC_STATUSC_QSPI_Pos;
}

static inline bool hri_pac_get_STATUSC_CCL_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSC.reg & PAC_STATUSC_CCL) >> PAC_STATUSC_CCL_Pos;
}

static inline hri_pac_statusc_reg_t hri_pac_get_STATUSC_reg(const void *const hw, hri_pac_statusc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSC.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusc_reg_t hri_pac_read_STATUSC_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSC.reg;
}

static inline bool hri_pac_get_STATUSD_SERCOM4_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_SERCOM4) >> PAC_STATUSD_SERCOM4_Pos;
}

static inline bool hri_pac_get_STATUSD_SERCOM5_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_SERCOM5) >> PAC_STATUSD_SERCOM5_Pos;
}

static inline bool hri_pac_get_STATUSD_SERCOM6_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_SERCOM6) >> PAC_STATUSD_SERCOM6_Pos;
}

static inline bool hri_pac_get_STATUSD_SERCOM7_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_SERCOM7) >> PAC_STATUSD_SERCOM7_Pos;
}

static inline bool hri_pac_get_STATUSD_TCC4_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_TCC4) >> PAC_STATUSD_TCC4_Pos;
}

static inline bool hri_pac_get_STATUSD_TC6_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_TC6) >> PAC_STATUSD_TC6_Pos;
}

static inline bool hri_pac_get_STATUSD_TC7_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_TC7) >> PAC_STATUSD_TC7_Pos;
}

static inline bool hri_pac_get_STATUSD_ADC0_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_ADC0) >> PAC_STATUSD_ADC0_Pos;
}

static inline bool hri_pac_get_STATUSD_ADC1_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_ADC1) >> PAC_STATUSD_ADC1_Pos;
}

static inline bool hri_pac_get_STATUSD_DAC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_DAC) >> PAC_STATUSD_DAC_Pos;
}

static inline bool hri_pac_get_STATUSD_I2S_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_I2S) >> PAC_STATUSD_I2S_Pos;
}

static inline bool hri_pac_get_STATUSD_PCC_bit(const void *const hw)
{
	return (((Pac *)hw)->STATUSD.reg & PAC_STATUSD_PCC) >> PAC_STATUSD_PCC_Pos;
}

static inline hri_pac_statusd_reg_t hri_pac_get_STATUSD_reg(const void *const hw, hri_pac_statusd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pac *)hw)->STATUSD.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pac_statusd_reg_t hri_pac_read_STATUSD_reg(const void *const hw)
{
	return ((Pac *)hw)->STATUSD.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PAC_D51_H_INCLUDED */
#endif /* _SAMD51_PAC_COMPONENT_ */
