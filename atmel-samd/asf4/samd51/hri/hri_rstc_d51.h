/**
 * \file
 *
 * \brief SAM RSTC
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

#ifdef _SAMD51_RSTC_COMPONENT_
#ifndef _HRI_RSTC_D51_H_INCLUDED_
#define _HRI_RSTC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_RSTC_CRITICAL_SECTIONS)
#define RSTC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define RSTC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define RSTC_CRITICAL_SECTION_ENTER()
#define RSTC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint8_t hri_rstc_bkupexit_reg_t;
typedef uint8_t hri_rstc_rcause_reg_t;

static inline bool hri_rstc_get_RCAUSE_POR_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_POR) >> RSTC_RCAUSE_POR_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BODCORE_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BODCORE) >> RSTC_RCAUSE_BODCORE_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BODVDD_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BODVDD) >> RSTC_RCAUSE_BODVDD_Pos;
}

static inline bool hri_rstc_get_RCAUSE_NVM_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_NVM) >> RSTC_RCAUSE_NVM_Pos;
}

static inline bool hri_rstc_get_RCAUSE_EXT_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_EXT) >> RSTC_RCAUSE_EXT_Pos;
}

static inline bool hri_rstc_get_RCAUSE_WDT_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_WDT) >> RSTC_RCAUSE_WDT_Pos;
}

static inline bool hri_rstc_get_RCAUSE_SYST_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_SYST) >> RSTC_RCAUSE_SYST_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BACKUP_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BACKUP) >> RSTC_RCAUSE_BACKUP_Pos;
}

static inline hri_rstc_rcause_reg_t hri_rstc_get_RCAUSE_reg(const void *const hw, hri_rstc_rcause_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->RCAUSE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rstc_rcause_reg_t hri_rstc_read_RCAUSE_reg(const void *const hw)
{
	return ((Rstc *)hw)->RCAUSE.reg;
}

static inline bool hri_rstc_get_BKUPEXIT_RTC_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_RTC) >> RSTC_BKUPEXIT_RTC_Pos;
}

static inline bool hri_rstc_get_BKUPEXIT_BBPS_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_BBPS) >> RSTC_BKUPEXIT_BBPS_Pos;
}

static inline bool hri_rstc_get_BKUPEXIT_HIB_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_HIB) >> RSTC_BKUPEXIT_HIB_Pos;
}

static inline hri_rstc_bkupexit_reg_t hri_rstc_get_BKUPEXIT_reg(const void *const hw, hri_rstc_bkupexit_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->BKUPEXIT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rstc_bkupexit_reg_t hri_rstc_read_BKUPEXIT_reg(const void *const hw)
{
	return ((Rstc *)hw)->BKUPEXIT.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_RSTC_D51_H_INCLUDED */
#endif /* _SAMD51_RSTC_COMPONENT_ */
