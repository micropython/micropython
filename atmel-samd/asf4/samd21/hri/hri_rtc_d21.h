/**
 * \file
 *
 * \brief SAM RTC
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

#ifdef _SAMD21_RTC_COMPONENT_
#ifndef _HRI_RTC_D21_H_INCLUDED_
#define _HRI_RTC_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_RTC_CRITICAL_SECTIONS)
#define RTC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define RTC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define RTC_CRITICAL_SECTION_ENTER()
#define RTC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_rtcmode0_ctrl_reg_t;
typedef uint16_t hri_rtcmode0_evctrl_reg_t;
typedef uint16_t hri_rtcmode0_readreq_reg_t;
typedef uint16_t hri_rtcmode1_comp_reg_t;
typedef uint16_t hri_rtcmode1_count_reg_t;
typedef uint16_t hri_rtcmode1_ctrl_reg_t;
typedef uint16_t hri_rtcmode1_evctrl_reg_t;
typedef uint16_t hri_rtcmode1_per_reg_t;
typedef uint16_t hri_rtcmode1_readreq_reg_t;
typedef uint16_t hri_rtcmode2_ctrl_reg_t;
typedef uint16_t hri_rtcmode2_evctrl_reg_t;
typedef uint16_t hri_rtcmode2_readreq_reg_t;
typedef uint32_t hri_rtcalarm_alarm_reg_t;
typedef uint32_t hri_rtcmode0_comp_reg_t;
typedef uint32_t hri_rtcmode0_count_reg_t;
typedef uint32_t hri_rtcmode2_alarm_reg_t;
typedef uint32_t hri_rtcmode2_clock_reg_t;
typedef uint8_t  hri_rtcalarm_mask_reg_t;
typedef uint8_t  hri_rtcmode0_dbgctrl_reg_t;
typedef uint8_t  hri_rtcmode0_freqcorr_reg_t;
typedef uint8_t  hri_rtcmode0_intenset_reg_t;
typedef uint8_t  hri_rtcmode0_intflag_reg_t;
typedef uint8_t  hri_rtcmode0_status_reg_t;
typedef uint8_t  hri_rtcmode1_dbgctrl_reg_t;
typedef uint8_t  hri_rtcmode1_freqcorr_reg_t;
typedef uint8_t  hri_rtcmode1_intenset_reg_t;
typedef uint8_t  hri_rtcmode1_intflag_reg_t;
typedef uint8_t  hri_rtcmode1_status_reg_t;
typedef uint8_t  hri_rtcmode2_dbgctrl_reg_t;
typedef uint8_t  hri_rtcmode2_freqcorr_reg_t;
typedef uint8_t  hri_rtcmode2_intenset_reg_t;
typedef uint8_t  hri_rtcmode2_intflag_reg_t;
typedef uint8_t  hri_rtcmode2_mask_reg_t;
typedef uint8_t  hri_rtcmode2_status_reg_t;

static inline void hri_rtcmode0_wait_for_sync(const void *const hw)
{
	while (((const Rtc *)hw)->MODE0.STATUS.bit.SYNCBUSY)
		;
}

static inline bool hri_rtcmode0_is_syncing(const void *const hw)
{
	return ((const Rtc *)hw)->MODE0.STATUS.bit.SYNCBUSY;
}

static inline void hri_rtcmode1_wait_for_sync(const void *const hw)
{
	while (((const Rtc *)hw)->MODE1.STATUS.bit.SYNCBUSY)
		;
}

static inline bool hri_rtcmode1_is_syncing(const void *const hw)
{
	return ((const Rtc *)hw)->MODE1.STATUS.bit.SYNCBUSY;
}

static inline void hri_rtcmode2_wait_for_sync(const void *const hw)
{
	while (((const Rtc *)hw)->MODE2.STATUS.bit.SYNCBUSY)
		;
}

static inline bool hri_rtcmode2_is_syncing(const void *const hw)
{
	return ((const Rtc *)hw)->MODE2.STATUS.bit.SYNCBUSY;
}

static inline void hri_rtcalarm_set_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                                        hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_SECOND(mask)) >> RTC_MODE2_ALARM_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_SECOND_Msk;
	tmp |= RTC_MODE2_ALARM_SECOND(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_SECOND_Msk) >> RTC_MODE2_ALARM_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                                        hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MINUTE(mask)) >> RTC_MODE2_ALARM_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_MINUTE_Msk;
	tmp |= RTC_MODE2_ALARM_MINUTE(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MINUTE_Msk) >> RTC_MODE2_ALARM_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_HOUR(mask)) >> RTC_MODE2_ALARM_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_HOUR_Msk;
	tmp |= RTC_MODE2_ALARM_HOUR(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_HOUR_Msk) >> RTC_MODE2_ALARM_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_DAY(mask)) >> RTC_MODE2_ALARM_DAY_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_DAY_Msk;
	tmp |= RTC_MODE2_ALARM_DAY(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_DAY_Msk) >> RTC_MODE2_ALARM_DAY_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MONTH(mask)) >> RTC_MODE2_ALARM_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_MONTH_Msk;
	tmp |= RTC_MODE2_ALARM_MONTH(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MONTH_Msk) >> RTC_MODE2_ALARM_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_YEAR(mask)) >> RTC_MODE2_ALARM_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_YEAR_Msk;
	tmp |= RTC_MODE2_ALARM_YEAR(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_YEAR_Msk) >> RTC_MODE2_ALARM_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                              hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_get_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                                  hri_rtcalarm_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcalarm_write_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcalarm_alarm_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_rtcalarm_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_alarm_reg_t hri_rtcalarm_read_ALARM_reg(const void *const hw, uint8_t submodule_index)
{
	return ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].ALARM.reg;
}

static inline void hri_rtcalarm_set_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg |= RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_mask_reg_t hri_rtcalarm_get_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_rtcalarm_mask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg;
	tmp = (tmp & RTC_MODE2_MASK_SEL(mask)) >> RTC_MODE2_MASK_SEL_Pos;
	return tmp;
}

static inline void hri_rtcalarm_write_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcalarm_mask_reg_t data)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg;
	tmp &= ~RTC_MODE2_MASK_SEL_Msk;
	tmp |= RTC_MODE2_MASK_SEL(data);
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg &= ~RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg ^= RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_mask_reg_t hri_rtcalarm_read_MASK_SEL_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg;
	tmp = (tmp & RTC_MODE2_MASK_SEL_Msk) >> RTC_MODE2_MASK_SEL_Pos;
	return tmp;
}

static inline void hri_rtcalarm_set_MASK_reg(const void *const hw, uint8_t submodule_index,
                                             hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_mask_reg_t hri_rtcalarm_get_MASK_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_rtcalarm_mask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcalarm_write_MASK_reg(const void *const hw, uint8_t submodule_index,
                                               hri_rtcalarm_mask_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_clear_MASK_reg(const void *const hw, uint8_t submodule_index,
                                               hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcalarm_toggle_MASK_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcalarm_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcalarm_mask_reg_t hri_rtcalarm_read_MASK_reg(const void *const hw, uint8_t submodule_index)
{
	return ((RtcMode2 *)hw)->Mode2Alarm[submodule_index].MASK.reg;
}

static inline void hri_rtcmode2_set_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                                        hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_SECOND(mask)) >> RTC_MODE2_ALARM_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_SECOND_Msk;
	tmp |= RTC_MODE2_ALARM_SECOND(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_SECOND_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_SECOND_Msk) >> RTC_MODE2_ALARM_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                                        hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MINUTE(mask)) >> RTC_MODE2_ALARM_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_MINUTE_Msk;
	tmp |= RTC_MODE2_ALARM_MINUTE(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_MINUTE_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MINUTE_Msk) >> RTC_MODE2_ALARM_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_HOUR(mask)) >> RTC_MODE2_ALARM_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_HOUR_Msk;
	tmp |= RTC_MODE2_ALARM_HOUR(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_HOUR_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_HOUR_Msk) >> RTC_MODE2_ALARM_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_DAY(mask)) >> RTC_MODE2_ALARM_DAY_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_DAY_Msk;
	tmp |= RTC_MODE2_ALARM_DAY(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_DAY_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_DAY_Msk) >> RTC_MODE2_ALARM_DAY_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MONTH(mask)) >> RTC_MODE2_ALARM_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_MONTH_Msk;
	tmp |= RTC_MODE2_ALARM_MONTH(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_MONTH_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_MONTH_Msk) >> RTC_MODE2_ALARM_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_YEAR(mask)) >> RTC_MODE2_ALARM_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= ~RTC_MODE2_ALARM_YEAR_Msk;
	tmp |= RTC_MODE2_ALARM_YEAR(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= RTC_MODE2_ALARM_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_YEAR_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp = (tmp & RTC_MODE2_ALARM_YEAR_Msk) >> RTC_MODE2_ALARM_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                              hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_get_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                                  hri_rtcmode2_alarm_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcmode2_alarm_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_ALARM_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_rtcmode2_alarm_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_alarm_reg_t hri_rtcmode2_read_ALARM_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].ALARM.reg;
}

static inline void hri_rtcmode2_set_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg |= RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_mask_reg_t hri_rtcmode2_get_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_rtcmode2_mask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg;
	tmp = (tmp & RTC_MODE2_MASK_SEL(mask)) >> RTC_MODE2_MASK_SEL_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcmode2_mask_reg_t data)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg;
	tmp &= ~RTC_MODE2_MASK_SEL_Msk;
	tmp |= RTC_MODE2_MASK_SEL(data);
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg &= ~RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_MASK_SEL_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg ^= RTC_MODE2_MASK_SEL(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_mask_reg_t hri_rtcmode2_read_MASK_SEL_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg;
	tmp = (tmp & RTC_MODE2_MASK_SEL_Msk) >> RTC_MODE2_MASK_SEL_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_MASK_reg(const void *const hw, uint8_t submodule_index,
                                             hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_mask_reg_t hri_rtcmode2_get_MASK_reg(const void *const hw, uint8_t submodule_index,
                                                                hri_rtcmode2_mask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_MASK_reg(const void *const hw, uint8_t submodule_index,
                                               hri_rtcmode2_mask_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_MASK_reg(const void *const hw, uint8_t submodule_index,
                                               hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_MASK_reg(const void *const hw, uint8_t submodule_index,
                                                hri_rtcmode2_mask_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_mask_reg_t hri_rtcmode2_read_MASK_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Rtc *)hw)->MODE2.Mode2Alarm[submodule_index].MASK.reg;
}

static inline void hri_rtcmode2_set_INTEN_ALARM0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_ALARM0;
}

static inline bool hri_rtcmode2_get_INTEN_ALARM0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTENSET.reg & RTC_MODE2_INTENSET_ALARM0) >> RTC_MODE2_INTENSET_ALARM0_Pos;
}

static inline void hri_rtcmode2_write_INTEN_ALARM0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_ALARM0;
	} else {
		((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_ALARM0;
	}
}

static inline void hri_rtcmode2_clear_INTEN_ALARM0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_ALARM0;
}

static inline void hri_rtcmode2_set_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_SYNCRDY;
}

static inline bool hri_rtcmode2_get_INTEN_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTENSET.reg & RTC_MODE2_INTENSET_SYNCRDY) >> RTC_MODE2_INTENSET_SYNCRDY_Pos;
}

static inline void hri_rtcmode2_write_INTEN_SYNCRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_SYNCRDY;
	} else {
		((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_SYNCRDY;
	}
}

static inline void hri_rtcmode2_clear_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_SYNCRDY;
}

static inline void hri_rtcmode2_set_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_OVF;
}

static inline bool hri_rtcmode2_get_INTEN_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTENSET.reg & RTC_MODE2_INTENSET_OVF) >> RTC_MODE2_INTENSET_OVF_Pos;
}

static inline void hri_rtcmode2_write_INTEN_OVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_OVF;
	} else {
		((Rtc *)hw)->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_OVF;
	}
}

static inline void hri_rtcmode2_clear_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTENCLR.reg = RTC_MODE2_INTENSET_OVF;
}

static inline void hri_rtcmode2_set_INTEN_reg(const void *const hw, hri_rtcmode2_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE2.INTENSET.reg = mask;
}

static inline hri_rtcmode2_intenset_reg_t hri_rtcmode2_get_INTEN_reg(const void *const           hw,
                                                                     hri_rtcmode2_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode2_intenset_reg_t hri_rtcmode2_read_INTEN_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.INTENSET.reg;
}

static inline void hri_rtcmode2_write_INTEN_reg(const void *const hw, hri_rtcmode2_intenset_reg_t data)
{
	((Rtc *)hw)->MODE2.INTENSET.reg = data;
	((Rtc *)hw)->MODE2.INTENCLR.reg = ~data;
}

static inline void hri_rtcmode2_clear_INTEN_reg(const void *const hw, hri_rtcmode2_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE2.INTENCLR.reg = mask;
}

static inline bool hri_rtcmode2_get_INTFLAG_ALARM0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM0) >> RTC_MODE2_INTFLAG_ALARM0_Pos;
}

static inline void hri_rtcmode2_clear_INTFLAG_ALARM0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
}

static inline bool hri_rtcmode2_get_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_SYNCRDY) >> RTC_MODE2_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode2_clear_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode2_get_INTFLAG_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_OVF) >> RTC_MODE2_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode2_clear_INTFLAG_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_OVF;
}

static inline bool hri_rtcmode2_get_interrupt_ALARM0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM0) >> RTC_MODE2_INTFLAG_ALARM0_Pos;
}

static inline void hri_rtcmode2_clear_interrupt_ALARM0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
}

static inline bool hri_rtcmode2_get_interrupt_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_SYNCRDY) >> RTC_MODE2_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode2_clear_interrupt_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode2_get_interrupt_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_OVF) >> RTC_MODE2_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode2_clear_interrupt_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_OVF;
}

static inline hri_rtcmode2_intflag_reg_t hri_rtcmode2_get_INTFLAG_reg(const void *const          hw,
                                                                      hri_rtcmode2_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode2_intflag_reg_t hri_rtcmode2_read_INTFLAG_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.INTFLAG.reg;
}

static inline void hri_rtcmode2_clear_INTFLAG_reg(const void *const hw, hri_rtcmode2_intflag_reg_t mask)
{
	((Rtc *)hw)->MODE2.INTFLAG.reg = mask;
}

static inline void hri_rtcmode2_set_CTRL_SWRST_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_SWRST;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_CTRL_SWRST_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_SWRST) >> RTC_MODE2_CTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_set_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_ENABLE) >> RTC_MODE2_CTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= ~RTC_MODE2_CTRL_ENABLE;
	tmp |= value << RTC_MODE2_CTRL_ENABLE_Pos;
	((Rtc *)hw)->MODE2.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_CTRL_CLKREP_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_CLKREP;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_CTRL_CLKREP_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_CLKREP) >> RTC_MODE2_CTRL_CLKREP_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_CTRL_CLKREP_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= ~RTC_MODE2_CTRL_CLKREP;
	tmp |= value << RTC_MODE2_CTRL_CLKREP_Pos;
	((Rtc *)hw)->MODE2.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_CLKREP_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_CLKREP;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_CLKREP_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_CLKREP;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_CTRL_MATCHCLR_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_MATCHCLR) >> RTC_MODE2_CTRL_MATCHCLR_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_CTRL_MATCHCLR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= ~RTC_MODE2_CTRL_MATCHCLR;
	tmp |= value << RTC_MODE2_CTRL_MATCHCLR_Pos;
	((Rtc *)hw)->MODE2.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_CTRL_MODE_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_get_CTRL_MODE_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_MODE(mask)) >> RTC_MODE2_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CTRL_MODE_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= ~RTC_MODE2_CTRL_MODE_Msk;
	tmp |= RTC_MODE2_CTRL_MODE(data);
	((Rtc *)hw)->MODE2.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_MODE_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_MODE_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_read_CTRL_MODE_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_MODE_Msk) >> RTC_MODE2_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg |= RTC_MODE2_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_get_CTRL_PRESCALER_bf(const void *const       hw,
                                                                         hri_rtcmode2_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_PRESCALER(mask)) >> RTC_MODE2_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= ~RTC_MODE2_CTRL_PRESCALER_Msk;
	tmp |= RTC_MODE2_CTRL_PRESCALER(data);
	((Rtc *)hw)->MODE2.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CTRL.reg ^= RTC_MODE2_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_read_CTRL_PRESCALER_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp = (tmp & RTC_MODE2_CTRL_PRESCALER_Msk) >> RTC_MODE2_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CTRL_reg(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_get_CTRL_reg(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_CTRL_reg(const void *const hw, hri_rtcmode2_ctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CTRL_reg(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CTRL_reg(const void *const hw, hri_rtcmode2_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_ctrl_reg_t hri_rtcmode2_read_CTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.CTRL.reg;
}

static inline void hri_rtcmode2_set_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg |= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_READREQ_RCONT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RCONT) >> RTC_READREQ_RCONT_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_READREQ_RCONT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp &= ~RTC_READREQ_RCONT;
	tmp |= value << RTC_READREQ_RCONT_Pos;
	((Rtc *)hw)->MODE2.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg &= ~RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg ^= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg |= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_READREQ_RREQ_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RREQ) >> RTC_READREQ_RREQ_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_READREQ_RREQ_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp &= ~RTC_READREQ_RREQ;
	tmp |= value << RTC_READREQ_RREQ_Pos;
	((Rtc *)hw)->MODE2.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg &= ~RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg ^= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_READREQ_ADDR_bf(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg |= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_readreq_reg_t hri_rtcmode2_get_READREQ_ADDR_bf(const void *const          hw,
                                                                          hri_rtcmode2_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR(mask)) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_READREQ_ADDR_bf(const void *const hw, hri_rtcmode2_readreq_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp &= ~RTC_READREQ_ADDR_Msk;
	tmp |= RTC_READREQ_ADDR(data);
	((Rtc *)hw)->MODE2.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_READREQ_ADDR_bf(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg &= ~RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_READREQ_ADDR_bf(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg ^= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_readreq_reg_t hri_rtcmode2_read_READREQ_ADDR_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR_Msk) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_READREQ_reg(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_readreq_reg_t hri_rtcmode2_get_READREQ_reg(const void *const          hw,
                                                                      hri_rtcmode2_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.READREQ.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_READREQ_reg(const void *const hw, hri_rtcmode2_readreq_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_READREQ_reg(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_READREQ_reg(const void *const hw, hri_rtcmode2_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.READREQ.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_readreq_reg_t hri_rtcmode2_read_READREQ_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.READREQ.reg;
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO0) >> RTC_MODE2_EVCTRL_PEREO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO0;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO0_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO1) >> RTC_MODE2_EVCTRL_PEREO1_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO1;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO1_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO2_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO2) >> RTC_MODE2_EVCTRL_PEREO2_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO2_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO2;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO2_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO3_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO3) >> RTC_MODE2_EVCTRL_PEREO3_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO3_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO3;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO3_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO4_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO4) >> RTC_MODE2_EVCTRL_PEREO4_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO4_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO4;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO4_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO5_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO5) >> RTC_MODE2_EVCTRL_PEREO5_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO5_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO5;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO5_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO6_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO6) >> RTC_MODE2_EVCTRL_PEREO6_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO6_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO6;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO6_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_PEREO7_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_PEREO7) >> RTC_MODE2_EVCTRL_PEREO7_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_PEREO7_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_PEREO7;
	tmp |= value << RTC_MODE2_EVCTRL_PEREO7_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_ALARMEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_ALARMEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_ALARMEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_ALARMEO0) >> RTC_MODE2_EVCTRL_ALARMEO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_ALARMEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_ALARMEO0;
	tmp |= value << RTC_MODE2_EVCTRL_ALARMEO0_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_ALARMEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_ALARMEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_ALARMEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_ALARMEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= RTC_MODE2_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_EVCTRL_OVFEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp = (tmp & RTC_MODE2_EVCTRL_OVFEO) >> RTC_MODE2_EVCTRL_OVFEO_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_OVFEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= ~RTC_MODE2_EVCTRL_OVFEO;
	tmp |= value << RTC_MODE2_EVCTRL_OVFEO_Pos;
	((Rtc *)hw)->MODE2.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~RTC_MODE2_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= RTC_MODE2_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_EVCTRL_reg(const void *const hw, hri_rtcmode2_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_evctrl_reg_t hri_rtcmode2_get_EVCTRL_reg(const void *const         hw,
                                                                    hri_rtcmode2_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE2.EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_EVCTRL_reg(const void *const hw, hri_rtcmode2_evctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_EVCTRL_reg(const void *const hw, hri_rtcmode2_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_EVCTRL_reg(const void *const hw, hri_rtcmode2_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.EVCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_evctrl_reg_t hri_rtcmode2_read_EVCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.EVCTRL.reg;
}

static inline void hri_rtcmode2_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg |= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.DBGCTRL.reg;
	tmp = (tmp & RTC_DBGCTRL_DBGRUN) >> RTC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE2.DBGCTRL.reg;
	tmp &= ~RTC_DBGCTRL_DBGRUN;
	tmp |= value << RTC_DBGCTRL_DBGRUN_Pos;
	((Rtc *)hw)->MODE2.DBGCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg &= ~RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg ^= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_DBGCTRL_reg(const void *const hw, hri_rtcmode2_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_dbgctrl_reg_t hri_rtcmode2_get_DBGCTRL_reg(const void *const          hw,
                                                                      hri_rtcmode2_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_DBGCTRL_reg(const void *const hw, hri_rtcmode2_dbgctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_DBGCTRL_reg(const void *const hw, hri_rtcmode2_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_DBGCTRL_reg(const void *const hw, hri_rtcmode2_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.DBGCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_dbgctrl_reg_t hri_rtcmode2_read_DBGCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.DBGCTRL.reg;
}

static inline void hri_rtcmode2_set_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg |= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode2_get_FREQCORR_SIGN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_SIGN) >> RTC_FREQCORR_SIGN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode2_write_FREQCORR_SIGN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_SIGN;
	tmp |= value << RTC_FREQCORR_SIGN_Pos;
	((Rtc *)hw)->MODE2.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg &= ~RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg ^= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_set_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg |= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_freqcorr_reg_t hri_rtcmode2_get_FREQCORR_VALUE_bf(const void *const           hw,
                                                                             hri_rtcmode2_freqcorr_reg_t mask)
{
	uint8_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE(mask)) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode2_freqcorr_reg_t data)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_VALUE_Msk;
	tmp |= RTC_FREQCORR_VALUE(data);
	((Rtc *)hw)->MODE2.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg &= ~RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.FREQCORR.reg ^= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_freqcorr_reg_t hri_rtcmode2_read_FREQCORR_VALUE_bf(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE_Msk) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_FREQCORR_reg(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.FREQCORR.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_freqcorr_reg_t hri_rtcmode2_get_FREQCORR_reg(const void *const           hw,
                                                                        hri_rtcmode2_freqcorr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.FREQCORR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_FREQCORR_reg(const void *const hw, hri_rtcmode2_freqcorr_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.FREQCORR.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_FREQCORR_reg(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.FREQCORR.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_FREQCORR_reg(const void *const hw, hri_rtcmode2_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.FREQCORR.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_freqcorr_reg_t hri_rtcmode2_read_FREQCORR_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.FREQCORR.reg;
}

static inline void hri_rtcmode2_set_CLOCK_SECOND_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_SECOND_bf(const void *const        hw,
                                                                        hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_SECOND(mask)) >> RTC_MODE2_CLOCK_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_SECOND_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_SECOND_Msk;
	tmp |= RTC_MODE2_CLOCK_SECOND(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_SECOND_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_SECOND_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_SECOND(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_SECOND_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_SECOND_Msk) >> RTC_MODE2_CLOCK_SECOND_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_MINUTE_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_MINUTE_bf(const void *const        hw,
                                                                        hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_MINUTE(mask)) >> RTC_MODE2_CLOCK_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_MINUTE_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_MINUTE_Msk;
	tmp |= RTC_MODE2_CLOCK_MINUTE(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_MINUTE_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_MINUTE_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_MINUTE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_MINUTE_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_MINUTE_Msk) >> RTC_MODE2_CLOCK_MINUTE_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_HOUR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_HOUR_bf(const void *const        hw,
                                                                      hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_HOUR(mask)) >> RTC_MODE2_CLOCK_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_HOUR_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_HOUR_Msk;
	tmp |= RTC_MODE2_CLOCK_HOUR(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_HOUR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_HOUR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_HOUR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_HOUR_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_HOUR_Msk) >> RTC_MODE2_CLOCK_HOUR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_DAY_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_DAY_bf(const void *const        hw,
                                                                     hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_DAY(mask)) >> RTC_MODE2_CLOCK_DAY_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_DAY_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_DAY_Msk;
	tmp |= RTC_MODE2_CLOCK_DAY(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_DAY_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_DAY_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_DAY(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_DAY_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_DAY_Msk) >> RTC_MODE2_CLOCK_DAY_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_MONTH_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_MONTH_bf(const void *const        hw,
                                                                       hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_MONTH(mask)) >> RTC_MODE2_CLOCK_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_MONTH_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_MONTH_Msk;
	tmp |= RTC_MODE2_CLOCK_MONTH(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_MONTH_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_MONTH_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_MONTH(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_MONTH_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_MONTH_Msk) >> RTC_MODE2_CLOCK_MONTH_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_YEAR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg |= RTC_MODE2_CLOCK_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_YEAR_bf(const void *const        hw,
                                                                      hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_YEAR(mask)) >> RTC_MODE2_CLOCK_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_YEAR_bf(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= ~RTC_MODE2_CLOCK_YEAR_Msk;
	tmp |= RTC_MODE2_CLOCK_YEAR(data);
	((Rtc *)hw)->MODE2.CLOCK.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_YEAR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~RTC_MODE2_CLOCK_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_YEAR_bf(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode2_wait_for_sync(hw);
	((Rtc *)hw)->MODE2.CLOCK.reg ^= RTC_MODE2_CLOCK_YEAR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_YEAR_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode2_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp = (tmp & RTC_MODE2_CLOCK_YEAR_Msk) >> RTC_MODE2_CLOCK_YEAR_Pos;
	return tmp;
}

static inline void hri_rtcmode2_set_CLOCK_reg(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CLOCK.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_get_CLOCK_reg(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE2.CLOCK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_write_CLOCK_reg(const void *const hw, hri_rtcmode2_clock_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CLOCK.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_clear_CLOCK_reg(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CLOCK.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode2_toggle_CLOCK_reg(const void *const hw, hri_rtcmode2_clock_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.CLOCK.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_clock_reg_t hri_rtcmode2_read_CLOCK_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.CLOCK.reg;
}

static inline bool hri_rtcmode2_get_STATUS_SYNCBUSY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) >> RTC_STATUS_SYNCBUSY_Pos;
}

static inline void hri_rtcmode2_clear_STATUS_SYNCBUSY_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.STATUS.reg = RTC_STATUS_SYNCBUSY;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_status_reg_t hri_rtcmode2_get_STATUS_reg(const void *const         hw,
                                                                    hri_rtcmode2_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE2.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode2_clear_STATUS_reg(const void *const hw, hri_rtcmode2_status_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE2.STATUS.reg = mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode2_status_reg_t hri_rtcmode2_read_STATUS_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE2.STATUS.reg;
}

static inline void hri_rtcmode0_set_INTEN_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
}

static inline bool hri_rtcmode0_get_INTEN_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTENSET.reg & RTC_MODE0_INTENSET_CMP0) >> RTC_MODE0_INTENSET_CMP0_Pos;
}

static inline void hri_rtcmode0_write_INTEN_CMP0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_CMP0;
	} else {
		((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
	}
}

static inline void hri_rtcmode0_clear_INTEN_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_CMP0;
}

static inline void hri_rtcmode0_set_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_SYNCRDY;
}

static inline bool hri_rtcmode0_get_INTEN_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTENSET.reg & RTC_MODE0_INTENSET_SYNCRDY) >> RTC_MODE0_INTENSET_SYNCRDY_Pos;
}

static inline void hri_rtcmode0_write_INTEN_SYNCRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_SYNCRDY;
	} else {
		((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_SYNCRDY;
	}
}

static inline void hri_rtcmode0_clear_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_SYNCRDY;
}

static inline void hri_rtcmode0_set_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_OVF;
}

static inline bool hri_rtcmode0_get_INTEN_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTENSET.reg & RTC_MODE0_INTENSET_OVF) >> RTC_MODE0_INTENSET_OVF_Pos;
}

static inline void hri_rtcmode0_write_INTEN_OVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_OVF;
	} else {
		((Rtc *)hw)->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_OVF;
	}
}

static inline void hri_rtcmode0_clear_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTENCLR.reg = RTC_MODE0_INTENSET_OVF;
}

static inline void hri_rtcmode0_set_INTEN_reg(const void *const hw, hri_rtcmode0_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE0.INTENSET.reg = mask;
}

static inline hri_rtcmode0_intenset_reg_t hri_rtcmode0_get_INTEN_reg(const void *const           hw,
                                                                     hri_rtcmode0_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode0_intenset_reg_t hri_rtcmode0_read_INTEN_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.INTENSET.reg;
}

static inline void hri_rtcmode0_write_INTEN_reg(const void *const hw, hri_rtcmode0_intenset_reg_t data)
{
	((Rtc *)hw)->MODE0.INTENSET.reg = data;
	((Rtc *)hw)->MODE0.INTENCLR.reg = ~data;
}

static inline void hri_rtcmode0_clear_INTEN_reg(const void *const hw, hri_rtcmode0_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE0.INTENCLR.reg = mask;
}

static inline bool hri_rtcmode0_get_INTFLAG_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_CMP0) >> RTC_MODE0_INTFLAG_CMP0_Pos;
}

static inline void hri_rtcmode0_clear_INTFLAG_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
}

static inline bool hri_rtcmode0_get_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_SYNCRDY) >> RTC_MODE0_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode0_clear_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode0_get_INTFLAG_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_OVF) >> RTC_MODE0_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode0_clear_INTFLAG_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
}

static inline bool hri_rtcmode0_get_interrupt_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_CMP0) >> RTC_MODE0_INTFLAG_CMP0_Pos;
}

static inline void hri_rtcmode0_clear_interrupt_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
}

static inline bool hri_rtcmode0_get_interrupt_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_SYNCRDY) >> RTC_MODE0_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode0_clear_interrupt_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode0_get_interrupt_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_OVF) >> RTC_MODE0_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode0_clear_interrupt_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
}

static inline hri_rtcmode0_intflag_reg_t hri_rtcmode0_get_INTFLAG_reg(const void *const          hw,
                                                                      hri_rtcmode0_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode0_intflag_reg_t hri_rtcmode0_read_INTFLAG_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.INTFLAG.reg;
}

static inline void hri_rtcmode0_clear_INTFLAG_reg(const void *const hw, hri_rtcmode0_intflag_reg_t mask)
{
	((Rtc *)hw)->MODE0.INTFLAG.reg = mask;
}

static inline void hri_rtcmode0_set_CTRL_SWRST_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_CTRL_SWRST_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_SWRST) >> RTC_MODE0_CTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_set_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_ENABLE) >> RTC_MODE0_CTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp &= ~RTC_MODE0_CTRL_ENABLE;
	tmp |= value << RTC_MODE0_CTRL_ENABLE_Pos;
	((Rtc *)hw)->MODE0.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg ^= RTC_MODE0_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_CTRL_MATCHCLR_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_MATCHCLR) >> RTC_MODE0_CTRL_MATCHCLR_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_CTRL_MATCHCLR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp &= ~RTC_MODE0_CTRL_MATCHCLR;
	tmp |= value << RTC_MODE0_CTRL_MATCHCLR_Pos;
	((Rtc *)hw)->MODE0.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_CTRL_MATCHCLR_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg ^= RTC_MODE0_CTRL_MATCHCLR;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_CTRL_MODE_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg |= RTC_MODE0_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_get_CTRL_MODE_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_MODE(mask)) >> RTC_MODE0_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_CTRL_MODE_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp &= ~RTC_MODE0_CTRL_MODE_Msk;
	tmp |= RTC_MODE0_CTRL_MODE(data);
	((Rtc *)hw)->MODE0.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_CTRL_MODE_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_CTRL_MODE_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg ^= RTC_MODE0_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_read_CTRL_MODE_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_MODE_Msk) >> RTC_MODE0_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg |= RTC_MODE0_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_get_CTRL_PRESCALER_bf(const void *const       hw,
                                                                         hri_rtcmode0_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_PRESCALER(mask)) >> RTC_MODE0_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp &= ~RTC_MODE0_CTRL_PRESCALER_Msk;
	tmp |= RTC_MODE0_CTRL_PRESCALER(data);
	((Rtc *)hw)->MODE0.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.CTRL.reg ^= RTC_MODE0_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_read_CTRL_PRESCALER_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp = (tmp & RTC_MODE0_CTRL_PRESCALER_Msk) >> RTC_MODE0_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_CTRL_reg(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.CTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_get_CTRL_reg(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_CTRL_reg(const void *const hw, hri_rtcmode0_ctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.CTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_CTRL_reg(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.CTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_CTRL_reg(const void *const hw, hri_rtcmode0_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.CTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_ctrl_reg_t hri_rtcmode0_read_CTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.CTRL.reg;
}

static inline void hri_rtcmode0_set_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg |= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_READREQ_RCONT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RCONT) >> RTC_READREQ_RCONT_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_READREQ_RCONT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp &= ~RTC_READREQ_RCONT;
	tmp |= value << RTC_READREQ_RCONT_Pos;
	((Rtc *)hw)->MODE0.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg &= ~RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg ^= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg |= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_READREQ_RREQ_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RREQ) >> RTC_READREQ_RREQ_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_READREQ_RREQ_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp &= ~RTC_READREQ_RREQ;
	tmp |= value << RTC_READREQ_RREQ_Pos;
	((Rtc *)hw)->MODE0.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg &= ~RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg ^= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_READREQ_ADDR_bf(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg |= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_readreq_reg_t hri_rtcmode0_get_READREQ_ADDR_bf(const void *const          hw,
                                                                          hri_rtcmode0_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR(mask)) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_READREQ_ADDR_bf(const void *const hw, hri_rtcmode0_readreq_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp &= ~RTC_READREQ_ADDR_Msk;
	tmp |= RTC_READREQ_ADDR(data);
	((Rtc *)hw)->MODE0.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_READREQ_ADDR_bf(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg &= ~RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_READREQ_ADDR_bf(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg ^= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_readreq_reg_t hri_rtcmode0_read_READREQ_ADDR_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR_Msk) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_READREQ_reg(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_readreq_reg_t hri_rtcmode0_get_READREQ_reg(const void *const          hw,
                                                                      hri_rtcmode0_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.READREQ.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_READREQ_reg(const void *const hw, hri_rtcmode0_readreq_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_READREQ_reg(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_READREQ_reg(const void *const hw, hri_rtcmode0_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.READREQ.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_readreq_reg_t hri_rtcmode0_read_READREQ_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.READREQ.reg;
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO0) >> RTC_MODE0_EVCTRL_PEREO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO0;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO0_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO1) >> RTC_MODE0_EVCTRL_PEREO1_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO1;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO1_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO2_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO2) >> RTC_MODE0_EVCTRL_PEREO2_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO2_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO2;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO2_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO3_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO3) >> RTC_MODE0_EVCTRL_PEREO3_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO3_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO3;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO3_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO4_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO4) >> RTC_MODE0_EVCTRL_PEREO4_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO4_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO4;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO4_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO5_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO5) >> RTC_MODE0_EVCTRL_PEREO5_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO5_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO5;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO5_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO6_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO6) >> RTC_MODE0_EVCTRL_PEREO6_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO6_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO6;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO6_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_PEREO7_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_PEREO7) >> RTC_MODE0_EVCTRL_PEREO7_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_PEREO7_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_PEREO7;
	tmp |= value << RTC_MODE0_EVCTRL_PEREO7_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_CMPEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_CMPEO0) >> RTC_MODE0_EVCTRL_CMPEO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_CMPEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_CMPEO0;
	tmp |= value << RTC_MODE0_EVCTRL_CMPEO0_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= RTC_MODE0_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_EVCTRL_OVFEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp = (tmp & RTC_MODE0_EVCTRL_OVFEO) >> RTC_MODE0_EVCTRL_OVFEO_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_OVFEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= ~RTC_MODE0_EVCTRL_OVFEO;
	tmp |= value << RTC_MODE0_EVCTRL_OVFEO_Pos;
	((Rtc *)hw)->MODE0.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~RTC_MODE0_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= RTC_MODE0_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_EVCTRL_reg(const void *const hw, hri_rtcmode0_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_evctrl_reg_t hri_rtcmode0_get_EVCTRL_reg(const void *const         hw,
                                                                    hri_rtcmode0_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE0.EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_EVCTRL_reg(const void *const hw, hri_rtcmode0_evctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_EVCTRL_reg(const void *const hw, hri_rtcmode0_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_EVCTRL_reg(const void *const hw, hri_rtcmode0_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.EVCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_evctrl_reg_t hri_rtcmode0_read_EVCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.EVCTRL.reg;
}

static inline void hri_rtcmode0_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg |= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.DBGCTRL.reg;
	tmp = (tmp & RTC_DBGCTRL_DBGRUN) >> RTC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE0.DBGCTRL.reg;
	tmp &= ~RTC_DBGCTRL_DBGRUN;
	tmp |= value << RTC_DBGCTRL_DBGRUN_Pos;
	((Rtc *)hw)->MODE0.DBGCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg &= ~RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg ^= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_DBGCTRL_reg(const void *const hw, hri_rtcmode0_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_dbgctrl_reg_t hri_rtcmode0_get_DBGCTRL_reg(const void *const          hw,
                                                                      hri_rtcmode0_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_DBGCTRL_reg(const void *const hw, hri_rtcmode0_dbgctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_DBGCTRL_reg(const void *const hw, hri_rtcmode0_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_DBGCTRL_reg(const void *const hw, hri_rtcmode0_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.DBGCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_dbgctrl_reg_t hri_rtcmode0_read_DBGCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.DBGCTRL.reg;
}

static inline void hri_rtcmode0_set_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg |= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode0_get_FREQCORR_SIGN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_SIGN) >> RTC_FREQCORR_SIGN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode0_write_FREQCORR_SIGN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_SIGN;
	tmp |= value << RTC_FREQCORR_SIGN_Pos;
	((Rtc *)hw)->MODE0.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg &= ~RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg ^= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_set_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg |= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_freqcorr_reg_t hri_rtcmode0_get_FREQCORR_VALUE_bf(const void *const           hw,
                                                                             hri_rtcmode0_freqcorr_reg_t mask)
{
	uint8_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE(mask)) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode0_freqcorr_reg_t data)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_VALUE_Msk;
	tmp |= RTC_FREQCORR_VALUE(data);
	((Rtc *)hw)->MODE0.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg &= ~RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.FREQCORR.reg ^= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_freqcorr_reg_t hri_rtcmode0_read_FREQCORR_VALUE_bf(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE_Msk) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_FREQCORR_reg(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.FREQCORR.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_freqcorr_reg_t hri_rtcmode0_get_FREQCORR_reg(const void *const           hw,
                                                                        hri_rtcmode0_freqcorr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.FREQCORR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_FREQCORR_reg(const void *const hw, hri_rtcmode0_freqcorr_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.FREQCORR.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_FREQCORR_reg(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.FREQCORR.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_FREQCORR_reg(const void *const hw, hri_rtcmode0_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.FREQCORR.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_freqcorr_reg_t hri_rtcmode0_read_FREQCORR_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.FREQCORR.reg;
}

static inline void hri_rtcmode0_set_COUNT_COUNT_bf(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COUNT.reg |= RTC_MODE0_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_count_reg_t hri_rtcmode0_get_COUNT_COUNT_bf(const void *const        hw,
                                                                       hri_rtcmode0_count_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COUNT.reg;
	tmp = (tmp & RTC_MODE0_COUNT_COUNT(mask)) >> RTC_MODE0_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_COUNT_COUNT_bf(const void *const hw, hri_rtcmode0_count_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COUNT.reg;
	tmp &= ~RTC_MODE0_COUNT_COUNT_Msk;
	tmp |= RTC_MODE0_COUNT_COUNT(data);
	((Rtc *)hw)->MODE0.COUNT.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_COUNT_COUNT_bf(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COUNT.reg &= ~RTC_MODE0_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_COUNT_COUNT_bf(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COUNT.reg ^= RTC_MODE0_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_count_reg_t hri_rtcmode0_read_COUNT_COUNT_bf(const void *const hw)
{
	uint32_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COUNT.reg;
	tmp = (tmp & RTC_MODE0_COUNT_COUNT_Msk) >> RTC_MODE0_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_COUNT_reg(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COUNT.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_count_reg_t hri_rtcmode0_get_COUNT_reg(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE0.COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_COUNT_reg(const void *const hw, hri_rtcmode0_count_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COUNT.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_COUNT_reg(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COUNT.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_COUNT_reg(const void *const hw, hri_rtcmode0_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COUNT.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_count_reg_t hri_rtcmode0_read_COUNT_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.COUNT.reg;
}

static inline void hri_rtcmode0_set_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COMP[index].reg |= RTC_MODE0_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_comp_reg_t hri_rtcmode0_get_COMP_COMP_bf(const void *const hw, uint8_t index,
                                                                    hri_rtcmode0_comp_reg_t mask)
{
	uint32_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COMP[index].reg;
	tmp = (tmp & RTC_MODE0_COMP_COMP(mask)) >> RTC_MODE0_COMP_COMP_Pos;
	return tmp;
}

static inline void hri_rtcmode0_write_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t data)
{
	uint32_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COMP[index].reg;
	tmp &= ~RTC_MODE0_COMP_COMP_Msk;
	tmp |= RTC_MODE0_COMP_COMP(data);
	((Rtc *)hw)->MODE0.COMP[index].reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COMP[index].reg &= ~RTC_MODE0_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode0_wait_for_sync(hw);
	((Rtc *)hw)->MODE0.COMP[index].reg ^= RTC_MODE0_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_comp_reg_t hri_rtcmode0_read_COMP_COMP_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	hri_rtcmode0_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE0.COMP[index].reg;
	tmp = (tmp & RTC_MODE0_COMP_COMP_Msk) >> RTC_MODE0_COMP_COMP_Pos;
	return tmp;
}

static inline void hri_rtcmode0_set_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COMP[index].reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_comp_reg_t hri_rtcmode0_get_COMP_reg(const void *const hw, uint8_t index,
                                                                hri_rtcmode0_comp_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Rtc *)hw)->MODE0.COMP[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_write_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COMP[index].reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_clear_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COMP[index].reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode0_toggle_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode0_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.COMP[index].reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_comp_reg_t hri_rtcmode0_read_COMP_reg(const void *const hw, uint8_t index)
{
	return ((Rtc *)hw)->MODE0.COMP[index].reg;
}

static inline bool hri_rtcmode0_get_STATUS_SYNCBUSY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE0.STATUS.reg & RTC_STATUS_SYNCBUSY) >> RTC_STATUS_SYNCBUSY_Pos;
}

static inline void hri_rtcmode0_clear_STATUS_SYNCBUSY_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.STATUS.reg = RTC_STATUS_SYNCBUSY;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_status_reg_t hri_rtcmode0_get_STATUS_reg(const void *const         hw,
                                                                    hri_rtcmode0_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE0.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode0_clear_STATUS_reg(const void *const hw, hri_rtcmode0_status_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE0.STATUS.reg = mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode0_status_reg_t hri_rtcmode0_read_STATUS_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE0.STATUS.reg;
}

static inline void hri_rtcmode1_set_INTEN_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_CMP0;
}

static inline bool hri_rtcmode1_get_INTEN_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTENSET.reg & RTC_MODE1_INTENSET_CMP0) >> RTC_MODE1_INTENSET_CMP0_Pos;
}

static inline void hri_rtcmode1_write_INTEN_CMP0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_CMP0;
	} else {
		((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_CMP0;
	}
}

static inline void hri_rtcmode1_clear_INTEN_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_CMP0;
}

static inline void hri_rtcmode1_set_INTEN_CMP1_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_CMP1;
}

static inline bool hri_rtcmode1_get_INTEN_CMP1_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTENSET.reg & RTC_MODE1_INTENSET_CMP1) >> RTC_MODE1_INTENSET_CMP1_Pos;
}

static inline void hri_rtcmode1_write_INTEN_CMP1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_CMP1;
	} else {
		((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_CMP1;
	}
}

static inline void hri_rtcmode1_clear_INTEN_CMP1_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_CMP1;
}

static inline void hri_rtcmode1_set_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_SYNCRDY;
}

static inline bool hri_rtcmode1_get_INTEN_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTENSET.reg & RTC_MODE1_INTENSET_SYNCRDY) >> RTC_MODE1_INTENSET_SYNCRDY_Pos;
}

static inline void hri_rtcmode1_write_INTEN_SYNCRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_SYNCRDY;
	} else {
		((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_SYNCRDY;
	}
}

static inline void hri_rtcmode1_clear_INTEN_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_SYNCRDY;
}

static inline void hri_rtcmode1_set_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_OVF;
}

static inline bool hri_rtcmode1_get_INTEN_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTENSET.reg & RTC_MODE1_INTENSET_OVF) >> RTC_MODE1_INTENSET_OVF_Pos;
}

static inline void hri_rtcmode1_write_INTEN_OVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_OVF;
	} else {
		((Rtc *)hw)->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_OVF;
	}
}

static inline void hri_rtcmode1_clear_INTEN_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTENCLR.reg = RTC_MODE1_INTENSET_OVF;
}

static inline void hri_rtcmode1_set_INTEN_reg(const void *const hw, hri_rtcmode1_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = mask;
}

static inline hri_rtcmode1_intenset_reg_t hri_rtcmode1_get_INTEN_reg(const void *const           hw,
                                                                     hri_rtcmode1_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode1_intenset_reg_t hri_rtcmode1_read_INTEN_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.INTENSET.reg;
}

static inline void hri_rtcmode1_write_INTEN_reg(const void *const hw, hri_rtcmode1_intenset_reg_t data)
{
	((Rtc *)hw)->MODE1.INTENSET.reg = data;
	((Rtc *)hw)->MODE1.INTENCLR.reg = ~data;
}

static inline void hri_rtcmode1_clear_INTEN_reg(const void *const hw, hri_rtcmode1_intenset_reg_t mask)
{
	((Rtc *)hw)->MODE1.INTENCLR.reg = mask;
}

static inline bool hri_rtcmode1_get_INTFLAG_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_CMP0) >> RTC_MODE1_INTFLAG_CMP0_Pos;
}

static inline void hri_rtcmode1_clear_INTFLAG_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_CMP0;
}

static inline bool hri_rtcmode1_get_INTFLAG_CMP1_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_CMP1) >> RTC_MODE1_INTFLAG_CMP1_Pos;
}

static inline void hri_rtcmode1_clear_INTFLAG_CMP1_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_CMP1;
}

static inline bool hri_rtcmode1_get_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_SYNCRDY) >> RTC_MODE1_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode1_clear_INTFLAG_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode1_get_INTFLAG_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_OVF) >> RTC_MODE1_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode1_clear_INTFLAG_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_OVF;
}

static inline bool hri_rtcmode1_get_interrupt_CMP0_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_CMP0) >> RTC_MODE1_INTFLAG_CMP0_Pos;
}

static inline void hri_rtcmode1_clear_interrupt_CMP0_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_CMP0;
}

static inline bool hri_rtcmode1_get_interrupt_CMP1_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_CMP1) >> RTC_MODE1_INTFLAG_CMP1_Pos;
}

static inline void hri_rtcmode1_clear_interrupt_CMP1_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_CMP1;
}

static inline bool hri_rtcmode1_get_interrupt_SYNCRDY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_SYNCRDY) >> RTC_MODE1_INTFLAG_SYNCRDY_Pos;
}

static inline void hri_rtcmode1_clear_interrupt_SYNCRDY_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_SYNCRDY;
}

static inline bool hri_rtcmode1_get_interrupt_OVF_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_OVF) >> RTC_MODE1_INTFLAG_OVF_Pos;
}

static inline void hri_rtcmode1_clear_interrupt_OVF_bit(const void *const hw)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_OVF;
}

static inline hri_rtcmode1_intflag_reg_t hri_rtcmode1_get_INTFLAG_reg(const void *const          hw,
                                                                      hri_rtcmode1_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rtcmode1_intflag_reg_t hri_rtcmode1_read_INTFLAG_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.INTFLAG.reg;
}

static inline void hri_rtcmode1_clear_INTFLAG_reg(const void *const hw, hri_rtcmode1_intflag_reg_t mask)
{
	((Rtc *)hw)->MODE1.INTFLAG.reg = mask;
}

static inline void hri_rtcmode1_set_CTRL_SWRST_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg |= RTC_MODE1_CTRL_SWRST;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_CTRL_SWRST_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_SWRST) >> RTC_MODE1_CTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_set_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg |= RTC_MODE1_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_ENABLE) >> RTC_MODE1_CTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp &= ~RTC_MODE1_CTRL_ENABLE;
	tmp |= value << RTC_MODE1_CTRL_ENABLE_Pos;
	((Rtc *)hw)->MODE1.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg &= ~RTC_MODE1_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg ^= RTC_MODE1_CTRL_ENABLE;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_CTRL_MODE_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg |= RTC_MODE1_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_get_CTRL_MODE_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_MODE(mask)) >> RTC_MODE1_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_CTRL_MODE_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp &= ~RTC_MODE1_CTRL_MODE_Msk;
	tmp |= RTC_MODE1_CTRL_MODE(data);
	((Rtc *)hw)->MODE1.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_CTRL_MODE_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg &= ~RTC_MODE1_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_CTRL_MODE_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg ^= RTC_MODE1_CTRL_MODE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_read_CTRL_MODE_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_MODE_Msk) >> RTC_MODE1_CTRL_MODE_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg |= RTC_MODE1_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_get_CTRL_PRESCALER_bf(const void *const       hw,
                                                                         hri_rtcmode1_ctrl_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_PRESCALER(mask)) >> RTC_MODE1_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp &= ~RTC_MODE1_CTRL_PRESCALER_Msk;
	tmp |= RTC_MODE1_CTRL_PRESCALER(data);
	((Rtc *)hw)->MODE1.CTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg &= ~RTC_MODE1_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_CTRL_PRESCALER_bf(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.CTRL.reg ^= RTC_MODE1_CTRL_PRESCALER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_read_CTRL_PRESCALER_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp = (tmp & RTC_MODE1_CTRL_PRESCALER_Msk) >> RTC_MODE1_CTRL_PRESCALER_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_CTRL_reg(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.CTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_get_CTRL_reg(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_CTRL_reg(const void *const hw, hri_rtcmode1_ctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.CTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_CTRL_reg(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.CTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_CTRL_reg(const void *const hw, hri_rtcmode1_ctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.CTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_ctrl_reg_t hri_rtcmode1_read_CTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.CTRL.reg;
}

static inline void hri_rtcmode1_set_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg |= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_READREQ_RCONT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RCONT) >> RTC_READREQ_RCONT_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_READREQ_RCONT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp &= ~RTC_READREQ_RCONT;
	tmp |= value << RTC_READREQ_RCONT_Pos;
	((Rtc *)hw)->MODE1.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg &= ~RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_READREQ_RCONT_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg ^= RTC_READREQ_RCONT;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg |= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_READREQ_RREQ_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp = (tmp & RTC_READREQ_RREQ) >> RTC_READREQ_RREQ_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_READREQ_RREQ_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp &= ~RTC_READREQ_RREQ;
	tmp |= value << RTC_READREQ_RREQ_Pos;
	((Rtc *)hw)->MODE1.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg &= ~RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_READREQ_RREQ_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg ^= RTC_READREQ_RREQ;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_READREQ_ADDR_bf(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg |= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_readreq_reg_t hri_rtcmode1_get_READREQ_ADDR_bf(const void *const          hw,
                                                                          hri_rtcmode1_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR(mask)) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_READREQ_ADDR_bf(const void *const hw, hri_rtcmode1_readreq_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp &= ~RTC_READREQ_ADDR_Msk;
	tmp |= RTC_READREQ_ADDR(data);
	((Rtc *)hw)->MODE1.READREQ.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_READREQ_ADDR_bf(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg &= ~RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_READREQ_ADDR_bf(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg ^= RTC_READREQ_ADDR(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_readreq_reg_t hri_rtcmode1_read_READREQ_ADDR_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp = (tmp & RTC_READREQ_ADDR_Msk) >> RTC_READREQ_ADDR_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_READREQ_reg(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_readreq_reg_t hri_rtcmode1_get_READREQ_reg(const void *const          hw,
                                                                      hri_rtcmode1_readreq_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.READREQ.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_READREQ_reg(const void *const hw, hri_rtcmode1_readreq_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_READREQ_reg(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_READREQ_reg(const void *const hw, hri_rtcmode1_readreq_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.READREQ.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_readreq_reg_t hri_rtcmode1_read_READREQ_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.READREQ.reg;
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO0) >> RTC_MODE1_EVCTRL_PEREO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO0;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO0_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO1) >> RTC_MODE1_EVCTRL_PEREO1_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO1;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO1_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO2_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO2) >> RTC_MODE1_EVCTRL_PEREO2_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO2_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO2;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO2_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO2_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO2;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO3_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO3) >> RTC_MODE1_EVCTRL_PEREO3_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO3_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO3;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO3_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO3_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO3;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO4_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO4) >> RTC_MODE1_EVCTRL_PEREO4_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO4_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO4;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO4_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO4_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO4;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO5_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO5) >> RTC_MODE1_EVCTRL_PEREO5_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO5_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO5;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO5_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO5_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO5;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO6_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO6) >> RTC_MODE1_EVCTRL_PEREO6_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO6_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO6;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO6_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO6_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO6;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_PEREO7_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_PEREO7) >> RTC_MODE1_EVCTRL_PEREO7_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_PEREO7_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_PEREO7;
	tmp |= value << RTC_MODE1_EVCTRL_PEREO7_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_PEREO7_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_PEREO7;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_CMPEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_CMPEO0) >> RTC_MODE1_EVCTRL_CMPEO0_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_CMPEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_CMPEO0;
	tmp |= value << RTC_MODE1_EVCTRL_CMPEO0_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_CMPEO0_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_CMPEO0;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_CMPEO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_CMPEO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_CMPEO1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_CMPEO1) >> RTC_MODE1_EVCTRL_CMPEO1_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_CMPEO1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_CMPEO1;
	tmp |= value << RTC_MODE1_EVCTRL_CMPEO1_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_CMPEO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_CMPEO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_CMPEO1_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_CMPEO1;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= RTC_MODE1_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_EVCTRL_OVFEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp = (tmp & RTC_MODE1_EVCTRL_OVFEO) >> RTC_MODE1_EVCTRL_OVFEO_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_OVFEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= ~RTC_MODE1_EVCTRL_OVFEO;
	tmp |= value << RTC_MODE1_EVCTRL_OVFEO_Pos;
	((Rtc *)hw)->MODE1.EVCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~RTC_MODE1_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_OVFEO_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= RTC_MODE1_EVCTRL_OVFEO;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_EVCTRL_reg(const void *const hw, hri_rtcmode1_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_evctrl_reg_t hri_rtcmode1_get_EVCTRL_reg(const void *const         hw,
                                                                    hri_rtcmode1_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_EVCTRL_reg(const void *const hw, hri_rtcmode1_evctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_EVCTRL_reg(const void *const hw, hri_rtcmode1_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_EVCTRL_reg(const void *const hw, hri_rtcmode1_evctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.EVCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_evctrl_reg_t hri_rtcmode1_read_EVCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.EVCTRL.reg;
}

static inline void hri_rtcmode1_set_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg |= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.DBGCTRL.reg;
	tmp = (tmp & RTC_DBGCTRL_DBGRUN) >> RTC_DBGCTRL_DBGRUN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_DBGCTRL_DBGRUN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rtc *)hw)->MODE1.DBGCTRL.reg;
	tmp &= ~RTC_DBGCTRL_DBGRUN;
	tmp |= value << RTC_DBGCTRL_DBGRUN_Pos;
	((Rtc *)hw)->MODE1.DBGCTRL.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg &= ~RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_DBGCTRL_DBGRUN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg ^= RTC_DBGCTRL_DBGRUN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_DBGCTRL_reg(const void *const hw, hri_rtcmode1_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_dbgctrl_reg_t hri_rtcmode1_get_DBGCTRL_reg(const void *const          hw,
                                                                      hri_rtcmode1_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_DBGCTRL_reg(const void *const hw, hri_rtcmode1_dbgctrl_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_DBGCTRL_reg(const void *const hw, hri_rtcmode1_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_DBGCTRL_reg(const void *const hw, hri_rtcmode1_dbgctrl_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.DBGCTRL.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_dbgctrl_reg_t hri_rtcmode1_read_DBGCTRL_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.DBGCTRL.reg;
}

static inline void hri_rtcmode1_set_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg |= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_rtcmode1_get_FREQCORR_SIGN_bit(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_SIGN) >> RTC_FREQCORR_SIGN_Pos;
	return (bool)tmp;
}

static inline void hri_rtcmode1_write_FREQCORR_SIGN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_SIGN;
	tmp |= value << RTC_FREQCORR_SIGN_Pos;
	((Rtc *)hw)->MODE1.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg &= ~RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_FREQCORR_SIGN_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg ^= RTC_FREQCORR_SIGN;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_set_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg |= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_freqcorr_reg_t hri_rtcmode1_get_FREQCORR_VALUE_bf(const void *const           hw,
                                                                             hri_rtcmode1_freqcorr_reg_t mask)
{
	uint8_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE(mask)) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode1_freqcorr_reg_t data)
{
	uint8_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp &= ~RTC_FREQCORR_VALUE_Msk;
	tmp |= RTC_FREQCORR_VALUE(data);
	((Rtc *)hw)->MODE1.FREQCORR.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg &= ~RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_FREQCORR_VALUE_bf(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.FREQCORR.reg ^= RTC_FREQCORR_VALUE(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_freqcorr_reg_t hri_rtcmode1_read_FREQCORR_VALUE_bf(const void *const hw)
{
	uint8_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp = (tmp & RTC_FREQCORR_VALUE_Msk) >> RTC_FREQCORR_VALUE_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_FREQCORR_reg(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.FREQCORR.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_freqcorr_reg_t hri_rtcmode1_get_FREQCORR_reg(const void *const           hw,
                                                                        hri_rtcmode1_freqcorr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.FREQCORR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_FREQCORR_reg(const void *const hw, hri_rtcmode1_freqcorr_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.FREQCORR.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_FREQCORR_reg(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.FREQCORR.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_FREQCORR_reg(const void *const hw, hri_rtcmode1_freqcorr_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.FREQCORR.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_freqcorr_reg_t hri_rtcmode1_read_FREQCORR_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.FREQCORR.reg;
}

static inline void hri_rtcmode1_set_COUNT_COUNT_bf(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COUNT.reg |= RTC_MODE1_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_count_reg_t hri_rtcmode1_get_COUNT_COUNT_bf(const void *const        hw,
                                                                       hri_rtcmode1_count_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COUNT.reg;
	tmp = (tmp & RTC_MODE1_COUNT_COUNT(mask)) >> RTC_MODE1_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_COUNT_COUNT_bf(const void *const hw, hri_rtcmode1_count_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COUNT.reg;
	tmp &= ~RTC_MODE1_COUNT_COUNT_Msk;
	tmp |= RTC_MODE1_COUNT_COUNT(data);
	((Rtc *)hw)->MODE1.COUNT.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_COUNT_COUNT_bf(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COUNT.reg &= ~RTC_MODE1_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_COUNT_COUNT_bf(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COUNT.reg ^= RTC_MODE1_COUNT_COUNT(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_count_reg_t hri_rtcmode1_read_COUNT_COUNT_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COUNT.reg;
	tmp = (tmp & RTC_MODE1_COUNT_COUNT_Msk) >> RTC_MODE1_COUNT_COUNT_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_COUNT_reg(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COUNT.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_count_reg_t hri_rtcmode1_get_COUNT_reg(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.COUNT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_COUNT_reg(const void *const hw, hri_rtcmode1_count_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COUNT.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_COUNT_reg(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COUNT.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_COUNT_reg(const void *const hw, hri_rtcmode1_count_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COUNT.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_count_reg_t hri_rtcmode1_read_COUNT_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.COUNT.reg;
}

static inline void hri_rtcmode1_set_PER_PER_bf(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.PER.reg |= RTC_MODE1_PER_PER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_per_reg_t hri_rtcmode1_get_PER_PER_bf(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.PER.reg;
	tmp = (tmp & RTC_MODE1_PER_PER(mask)) >> RTC_MODE1_PER_PER_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_PER_PER_bf(const void *const hw, hri_rtcmode1_per_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.PER.reg;
	tmp &= ~RTC_MODE1_PER_PER_Msk;
	tmp |= RTC_MODE1_PER_PER(data);
	((Rtc *)hw)->MODE1.PER.reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_PER_PER_bf(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.PER.reg &= ~RTC_MODE1_PER_PER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_PER_PER_bf(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.PER.reg ^= RTC_MODE1_PER_PER(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_per_reg_t hri_rtcmode1_read_PER_PER_bf(const void *const hw)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.PER.reg;
	tmp = (tmp & RTC_MODE1_PER_PER_Msk) >> RTC_MODE1_PER_PER_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_PER_reg(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.PER.reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_per_reg_t hri_rtcmode1_get_PER_reg(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.PER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_PER_reg(const void *const hw, hri_rtcmode1_per_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.PER.reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_PER_reg(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.PER.reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_PER_reg(const void *const hw, hri_rtcmode1_per_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.PER.reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_per_reg_t hri_rtcmode1_read_PER_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.PER.reg;
}

static inline void hri_rtcmode1_set_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COMP[index].reg |= RTC_MODE1_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_comp_reg_t hri_rtcmode1_get_COMP_COMP_bf(const void *const hw, uint8_t index,
                                                                    hri_rtcmode1_comp_reg_t mask)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COMP[index].reg;
	tmp = (tmp & RTC_MODE1_COMP_COMP(mask)) >> RTC_MODE1_COMP_COMP_Pos;
	return tmp;
}

static inline void hri_rtcmode1_write_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t data)
{
	uint16_t tmp;
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COMP[index].reg;
	tmp &= ~RTC_MODE1_COMP_COMP_Msk;
	tmp |= RTC_MODE1_COMP_COMP(data);
	((Rtc *)hw)->MODE1.COMP[index].reg = tmp;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COMP[index].reg &= ~RTC_MODE1_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_COMP_COMP_bf(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	hri_rtcmode1_wait_for_sync(hw);
	((Rtc *)hw)->MODE1.COMP[index].reg ^= RTC_MODE1_COMP_COMP(mask);
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_comp_reg_t hri_rtcmode1_read_COMP_COMP_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	hri_rtcmode1_wait_for_sync(hw);
	tmp = ((Rtc *)hw)->MODE1.COMP[index].reg;
	tmp = (tmp & RTC_MODE1_COMP_COMP_Msk) >> RTC_MODE1_COMP_COMP_Pos;
	return tmp;
}

static inline void hri_rtcmode1_set_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COMP[index].reg |= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_comp_reg_t hri_rtcmode1_get_COMP_reg(const void *const hw, uint8_t index,
                                                                hri_rtcmode1_comp_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rtc *)hw)->MODE1.COMP[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_write_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t data)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COMP[index].reg = data;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_clear_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COMP[index].reg &= ~mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rtcmode1_toggle_COMP_reg(const void *const hw, uint8_t index, hri_rtcmode1_comp_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.COMP[index].reg ^= mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_comp_reg_t hri_rtcmode1_read_COMP_reg(const void *const hw, uint8_t index)
{
	return ((Rtc *)hw)->MODE1.COMP[index].reg;
}

static inline bool hri_rtcmode1_get_STATUS_SYNCBUSY_bit(const void *const hw)
{
	return (((Rtc *)hw)->MODE1.STATUS.reg & RTC_STATUS_SYNCBUSY) >> RTC_STATUS_SYNCBUSY_Pos;
}

static inline void hri_rtcmode1_clear_STATUS_SYNCBUSY_bit(const void *const hw)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.STATUS.reg = RTC_STATUS_SYNCBUSY;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_status_reg_t hri_rtcmode1_get_STATUS_reg(const void *const         hw,
                                                                    hri_rtcmode1_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rtc *)hw)->MODE1.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rtcmode1_clear_STATUS_reg(const void *const hw, hri_rtcmode1_status_reg_t mask)
{
	RTC_CRITICAL_SECTION_ENTER();
	((Rtc *)hw)->MODE1.STATUS.reg = mask;
	RTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rtcmode1_status_reg_t hri_rtcmode1_read_STATUS_reg(const void *const hw)
{
	return ((Rtc *)hw)->MODE1.STATUS.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_RTC_D21_H_INCLUDED */
#endif /* _SAMD21_RTC_COMPONENT_ */
