/**
 * \file
 *
 * \brief Instance description for RTC
 *
 * Copyright (c) 2017 Atmel Corporation,
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

#ifndef _SAMD51_RTC_INSTANCE_
#define _SAMD51_RTC_INSTANCE_

/* ========== Register definition for RTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RTC_DBGCTRL            (0x4000240E) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (0x40002414) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (0x40002440) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (0x40002444) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (0x40002448) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (0x4000244C) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_TAMPCTRL           (0x40002460) /**< \brief (RTC) Tamper Control */
#define REG_RTC_TAMPID             (0x40002468) /**< \brief (RTC) Tamper ID */
#define REG_RTC_BKUP0              (0x40002480) /**< \brief (RTC) Backup 0 */
#define REG_RTC_BKUP1              (0x40002484) /**< \brief (RTC) Backup 1 */
#define REG_RTC_BKUP2              (0x40002488) /**< \brief (RTC) Backup 2 */
#define REG_RTC_BKUP3              (0x4000248C) /**< \brief (RTC) Backup 3 */
#define REG_RTC_BKUP4              (0x40002490) /**< \brief (RTC) Backup 4 */
#define REG_RTC_BKUP5              (0x40002494) /**< \brief (RTC) Backup 5 */
#define REG_RTC_BKUP6              (0x40002498) /**< \brief (RTC) Backup 6 */
#define REG_RTC_BKUP7              (0x4000249C) /**< \brief (RTC) Backup 7 */
#define REG_RTC_MODE0_CTRLA        (0x40002400) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_CTRLB        (0x40002402) /**< \brief (RTC) MODE0 Control B */
#define REG_RTC_MODE0_EVCTRL       (0x40002404) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (0x40002408) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (0x4000240A) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (0x4000240C) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (0x40002410) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (0x40002418) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (0x40002420) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE0_COMP1        (0x40002424) /**< \brief (RTC) MODE0 Compare 1 Value */
#define REG_RTC_MODE0_TIMESTAMP    (0x40002464) /**< \brief (RTC) MODE0 Timestamp */
#define REG_RTC_MODE1_CTRLA        (0x40002400) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_CTRLB        (0x40002402) /**< \brief (RTC) MODE1 Control B */
#define REG_RTC_MODE1_EVCTRL       (0x40002404) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (0x40002408) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (0x4000240A) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (0x4000240C) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (0x40002410) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (0x40002418) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (0x4000241C) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (0x40002420) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (0x40002422) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE1_COMP2        (0x40002424) /**< \brief (RTC) MODE1 Compare 2 Value */
#define REG_RTC_MODE1_COMP3        (0x40002426) /**< \brief (RTC) MODE1 Compare 3 Value */
#define REG_RTC_MODE1_TIMESTAMP    (0x40002464) /**< \brief (RTC) MODE1 Timestamp */
#define REG_RTC_MODE2_CTRLA        (0x40002400) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_CTRLB        (0x40002402) /**< \brief (RTC) MODE2 Control B */
#define REG_RTC_MODE2_EVCTRL       (0x40002404) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (0x40002408) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (0x4000240A) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (0x4000240C) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (0x40002410) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (0x40002418) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_TIMESTAMP    (0x40002464) /**< \brief (RTC) MODE2 Timestamp */
#define REG_RTC_MODE2_ALARM_ALARM0 (0x40002420) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (0x40002424) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#define REG_RTC_MODE2_ALARM_ALARM1 (0x40002428) /**< \brief (RTC) MODE2_ALARM Alarm 1 Value */
#define REG_RTC_MODE2_ALARM_MASK1  (0x4000242C) /**< \brief (RTC) MODE2_ALARM Alarm 1 Mask */
#else
#define REG_RTC_DBGCTRL            (*(RwReg8 *)0x4000240EUL) /**< \brief (RTC) Debug Control */
#define REG_RTC_FREQCORR           (*(RwReg8 *)0x40002414UL) /**< \brief (RTC) Frequency Correction */
#define REG_RTC_GP0                (*(RwReg  *)0x40002440UL) /**< \brief (RTC) General Purpose 0 */
#define REG_RTC_GP1                (*(RwReg  *)0x40002444UL) /**< \brief (RTC) General Purpose 1 */
#define REG_RTC_GP2                (*(RwReg  *)0x40002448UL) /**< \brief (RTC) General Purpose 2 */
#define REG_RTC_GP3                (*(RwReg  *)0x4000244CUL) /**< \brief (RTC) General Purpose 3 */
#define REG_RTC_TAMPCTRL           (*(RwReg  *)0x40002460UL) /**< \brief (RTC) Tamper Control */
#define REG_RTC_TAMPID             (*(RwReg  *)0x40002468UL) /**< \brief (RTC) Tamper ID */
#define REG_RTC_BKUP0              (*(RwReg  *)0x40002480UL) /**< \brief (RTC) Backup 0 */
#define REG_RTC_BKUP1              (*(RwReg  *)0x40002484UL) /**< \brief (RTC) Backup 1 */
#define REG_RTC_BKUP2              (*(RwReg  *)0x40002488UL) /**< \brief (RTC) Backup 2 */
#define REG_RTC_BKUP3              (*(RwReg  *)0x4000248CUL) /**< \brief (RTC) Backup 3 */
#define REG_RTC_BKUP4              (*(RwReg  *)0x40002490UL) /**< \brief (RTC) Backup 4 */
#define REG_RTC_BKUP5              (*(RwReg  *)0x40002494UL) /**< \brief (RTC) Backup 5 */
#define REG_RTC_BKUP6              (*(RwReg  *)0x40002498UL) /**< \brief (RTC) Backup 6 */
#define REG_RTC_BKUP7              (*(RwReg  *)0x4000249CUL) /**< \brief (RTC) Backup 7 */
#define REG_RTC_MODE0_CTRLA        (*(RwReg16*)0x40002400UL) /**< \brief (RTC) MODE0 Control A */
#define REG_RTC_MODE0_CTRLB        (*(RwReg16*)0x40002402UL) /**< \brief (RTC) MODE0 Control B */
#define REG_RTC_MODE0_EVCTRL       (*(RwReg  *)0x40002404UL) /**< \brief (RTC) MODE0 Event Control */
#define REG_RTC_MODE0_INTENCLR     (*(RwReg16*)0x40002408UL) /**< \brief (RTC) MODE0 Interrupt Enable Clear */
#define REG_RTC_MODE0_INTENSET     (*(RwReg16*)0x4000240AUL) /**< \brief (RTC) MODE0 Interrupt Enable Set */
#define REG_RTC_MODE0_INTFLAG      (*(RwReg16*)0x4000240CUL) /**< \brief (RTC) MODE0 Interrupt Flag Status and Clear */
#define REG_RTC_MODE0_SYNCBUSY     (*(RoReg  *)0x40002410UL) /**< \brief (RTC) MODE0 Synchronization Busy Status */
#define REG_RTC_MODE0_COUNT        (*(RwReg  *)0x40002418UL) /**< \brief (RTC) MODE0 Counter Value */
#define REG_RTC_MODE0_COMP0        (*(RwReg  *)0x40002420UL) /**< \brief (RTC) MODE0 Compare 0 Value */
#define REG_RTC_MODE0_COMP1        (*(RwReg  *)0x40002424UL) /**< \brief (RTC) MODE0 Compare 1 Value */
#define REG_RTC_MODE0_TIMESTAMP    (*(RoReg  *)0x40002464UL) /**< \brief (RTC) MODE0 Timestamp */
#define REG_RTC_MODE1_CTRLA        (*(RwReg16*)0x40002400UL) /**< \brief (RTC) MODE1 Control A */
#define REG_RTC_MODE1_CTRLB        (*(RwReg16*)0x40002402UL) /**< \brief (RTC) MODE1 Control B */
#define REG_RTC_MODE1_EVCTRL       (*(RwReg  *)0x40002404UL) /**< \brief (RTC) MODE1 Event Control */
#define REG_RTC_MODE1_INTENCLR     (*(RwReg16*)0x40002408UL) /**< \brief (RTC) MODE1 Interrupt Enable Clear */
#define REG_RTC_MODE1_INTENSET     (*(RwReg16*)0x4000240AUL) /**< \brief (RTC) MODE1 Interrupt Enable Set */
#define REG_RTC_MODE1_INTFLAG      (*(RwReg16*)0x4000240CUL) /**< \brief (RTC) MODE1 Interrupt Flag Status and Clear */
#define REG_RTC_MODE1_SYNCBUSY     (*(RoReg  *)0x40002410UL) /**< \brief (RTC) MODE1 Synchronization Busy Status */
#define REG_RTC_MODE1_COUNT        (*(RwReg16*)0x40002418UL) /**< \brief (RTC) MODE1 Counter Value */
#define REG_RTC_MODE1_PER          (*(RwReg16*)0x4000241CUL) /**< \brief (RTC) MODE1 Counter Period */
#define REG_RTC_MODE1_COMP0        (*(RwReg16*)0x40002420UL) /**< \brief (RTC) MODE1 Compare 0 Value */
#define REG_RTC_MODE1_COMP1        (*(RwReg16*)0x40002422UL) /**< \brief (RTC) MODE1 Compare 1 Value */
#define REG_RTC_MODE1_COMP2        (*(RwReg16*)0x40002424UL) /**< \brief (RTC) MODE1 Compare 2 Value */
#define REG_RTC_MODE1_COMP3        (*(RwReg16*)0x40002426UL) /**< \brief (RTC) MODE1 Compare 3 Value */
#define REG_RTC_MODE1_TIMESTAMP    (*(RoReg  *)0x40002464UL) /**< \brief (RTC) MODE1 Timestamp */
#define REG_RTC_MODE2_CTRLA        (*(RwReg16*)0x40002400UL) /**< \brief (RTC) MODE2 Control A */
#define REG_RTC_MODE2_CTRLB        (*(RwReg16*)0x40002402UL) /**< \brief (RTC) MODE2 Control B */
#define REG_RTC_MODE2_EVCTRL       (*(RwReg  *)0x40002404UL) /**< \brief (RTC) MODE2 Event Control */
#define REG_RTC_MODE2_INTENCLR     (*(RwReg16*)0x40002408UL) /**< \brief (RTC) MODE2 Interrupt Enable Clear */
#define REG_RTC_MODE2_INTENSET     (*(RwReg16*)0x4000240AUL) /**< \brief (RTC) MODE2 Interrupt Enable Set */
#define REG_RTC_MODE2_INTFLAG      (*(RwReg16*)0x4000240CUL) /**< \brief (RTC) MODE2 Interrupt Flag Status and Clear */
#define REG_RTC_MODE2_SYNCBUSY     (*(RoReg  *)0x40002410UL) /**< \brief (RTC) MODE2 Synchronization Busy Status */
#define REG_RTC_MODE2_CLOCK        (*(RwReg  *)0x40002418UL) /**< \brief (RTC) MODE2 Clock Value */
#define REG_RTC_MODE2_TIMESTAMP    (*(RoReg  *)0x40002464UL) /**< \brief (RTC) MODE2 Timestamp */
#define REG_RTC_MODE2_ALARM_ALARM0 (*(RwReg  *)0x40002420UL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Value */
#define REG_RTC_MODE2_ALARM_MASK0  (*(RwReg  *)0x40002424UL) /**< \brief (RTC) MODE2_ALARM Alarm 0 Mask */
#define REG_RTC_MODE2_ALARM_ALARM1 (*(RwReg  *)0x40002428UL) /**< \brief (RTC) MODE2_ALARM Alarm 1 Value */
#define REG_RTC_MODE2_ALARM_MASK1  (*(RwReg  *)0x4000242CUL) /**< \brief (RTC) MODE2_ALARM Alarm 1 Mask */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RTC peripheral ========== */
#define RTC_DMAC_ID_TIMESTAMP       1        // DMA RTC timestamp trigger
#define RTC_GPR_NUM                 4        // Number of General-Purpose Registers
#define RTC_NUM_OF_ALARMS           2        // Number of Alarms
#define RTC_NUM_OF_BKREGS           8        // Number of Backup Registers
#define RTC_NUM_OF_COMP16           4        // Number of 16-bit Comparators
#define RTC_NUM_OF_COMP32           2        // Number of 32-bit Comparators
#define RTC_NUM_OF_TAMPERS          5        // Number of Tamper Inputs
#define RTC_PER_NUM                 8        // Number of Periodic Intervals

#endif /* _SAMD51_RTC_INSTANCE_ */
