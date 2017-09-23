/**
 * \file
 *
 * \brief Instance description for WDT
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

#ifndef _SAMD21_WDT_INSTANCE_
#define _SAMD21_WDT_INSTANCE_

/* ========== Register definition for WDT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_WDT_CTRL               (0x40001000) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (0x40001001) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (0x40001002) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (0x40001004) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (0x40001005) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (0x40001006) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_STATUS             (0x40001007) /**< \brief (WDT) Status */
#define REG_WDT_CLEAR              (0x40001008) /**< \brief (WDT) Clear */
#else
#define REG_WDT_CTRL               (*(RwReg8 *)0x40001000UL) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (*(RwReg8 *)0x40001001UL) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (*(RwReg8 *)0x40001002UL) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (*(RwReg8 *)0x40001004UL) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (*(RwReg8 *)0x40001005UL) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (*(RwReg8 *)0x40001006UL) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_STATUS             (*(RoReg8 *)0x40001007UL) /**< \brief (WDT) Status */
#define REG_WDT_CLEAR              (*(WoReg8 *)0x40001008UL) /**< \brief (WDT) Clear */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for WDT peripheral ========== */
#define WDT_GCLK_ID                 3        // Index of Generic Clock

#endif /* _SAMD21_WDT_INSTANCE_ */
