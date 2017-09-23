/**
 * \file
 *
 * \brief Instance description for RTC
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_RTC_INSTANCE_
#define _SAMD21_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RTC_READREQ            (0x40001402) /**< \brief (RTC) Read Request */
#define REG_RTC_STATUS             (0x4000140A) /**< \brief (RTC) Status */
#define REG_RTC_DBGCTRL            (0x4000140B) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (0x4000140C) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_MODE0_CTRL         (0x40001400) /**< \brief (RTC) MODE0 Control */
#define REG_RTC_MODE0_EVCTRL       (0x40001404) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (0x40001406) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (0x40001407) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (0x40001408) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_COUNT        (0x40001410) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (0x40001418) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRL         (0x40001400) /**< \brief (RTC) MODE1 Control */
#define REG_RTC_MODE1_EVCTRL       (0x40001404) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (0x40001406) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (0x40001407) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (0x40001408) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_COUNT        (0x40001410) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (0x40001414) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (0x40001418) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (0x4000141A) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRL         (0x40001400) /**< \brief (RTC) MODE2 Control */
#define REG_RTC_MODE2_EVCTRL       (0x40001404) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (0x40001406) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (0x40001407) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (0x40001408) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_CLOCK        (0x40001410) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (0x40001418) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (0x4000141C) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#else
#define REG_RTC_READREQ            (*(RwReg16*)0x40001402UL) /**< \brief (RTC) Read Request */
#define REG_RTC_STATUS             (*(RwReg8 *)0x4000140AUL) /**< \brief (RTC) Status */
#define REG_RTC_DBGCTRL            (*(RwReg8 *)0x4000140BUL) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (*(RwReg8 *)0x4000140CUL) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_MODE0_CTRL         (*(RwReg16*)0x40001400UL) /**< \brief (RTC) MODE0 Control */
#define REG_RTC_MODE0_EVCTRL       (*(RwReg16*)0x40001404UL) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (*(RwReg8 *)0x40001406UL) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (*(RwReg8 *)0x40001407UL) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (*(RwReg8 *)0x40001408UL) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_COUNT        (*(RwReg  *)0x40001410UL) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (*(RwReg  *)0x40001418UL) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE1_CTRL         (*(RwReg16*)0x40001400UL) /**< \brief (RTC) MODE1 Control */
#define REG_RTC_MODE1_EVCTRL       (*(RwReg16*)0x40001404UL) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (*(RwReg8 *)0x40001406UL) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (*(RwReg8 *)0x40001407UL) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (*(RwReg8 *)0x40001408UL) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_COUNT        (*(RwReg16*)0x40001410UL) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (*(RwReg16*)0x40001414UL) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (*(RwReg16*)0x40001418UL) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (*(RwReg16*)0x4000141AUL) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE2_CTRL         (*(RwReg16*)0x40001400UL) /**< \brief (RTC) MODE2 Control */
#define REG_RTC_MODE2_EVCTRL       (*(RwReg16*)0x40001404UL) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (*(RwReg8 *)0x40001406UL) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (*(RwReg8 *)0x40001407UL) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (*(RwReg8 *)0x40001408UL) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_CLOCK        (*(RwReg  *)0x40001410UL) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_ALARM_ALARM0 (*(RwReg  *)0x40001418UL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (*(RwReg  *)0x4000141CUL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RTC peripheral ========== */
#define RTC_ALARM_NUM               1        // Number of Alarms
#define RTC_COMP16_NUM              2        // Number of 16-bit Comparators
#define RTC_COMP32_NUM              1        // Number of 32-bit Comparators
#define RTC_GCLK_ID                 4        // Index of Generic Clock
#define RTC_NUM_OF_ALARMS           1        // Number of Alarms (obsolete)
#define RTC_NUM_OF_COMP16           2        // Number of 16-bit Comparators (obsolete)
#define RTC_NUM_OF_COMP32           1        // Number of 32-bit Comparators (obsolete)

#endif /* _SAMD21_RTC_INSTANCE_ */
