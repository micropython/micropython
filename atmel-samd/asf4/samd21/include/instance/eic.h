/**
 * \file
 *
 * \brief Instance description for EIC
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

#ifndef _SAMD21_EIC_INSTANCE_
#define _SAMD21_EIC_INSTANCE_

/* ========== Register definition for EIC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EIC_CTRL               (0x40001800) /**< \brief (EIC) Control */
#define REG_EIC_STATUS             (0x40001801) /**< \brief (EIC) Status */
#define REG_EIC_NMICTRL            (0x40001802) /**< \brief (EIC) Non-Maskable Interrupt Control */
#define REG_EIC_NMIFLAG            (0x40001803) /**< \brief (EIC) Non-Maskable Interrupt Flag Status and Clear */
#define REG_EIC_EVCTRL             (0x40001804) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (0x40001808) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (0x4000180C) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (0x40001810) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_WAKEUP             (0x40001814) /**< \brief (EIC) Wake-Up Enable */
#define REG_EIC_CONFIG0            (0x40001818) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (0x4000181C) /**< \brief (EIC) Configuration 1 */
#else
#define REG_EIC_CTRL               (*(RwReg8 *)0x40001800UL) /**< \brief (EIC) Control */
#define REG_EIC_STATUS             (*(RoReg8 *)0x40001801UL) /**< \brief (EIC) Status */
#define REG_EIC_NMICTRL            (*(RwReg8 *)0x40001802UL) /**< \brief (EIC) Non-Maskable Interrupt Control */
#define REG_EIC_NMIFLAG            (*(RwReg8 *)0x40001803UL) /**< \brief (EIC) Non-Maskable Interrupt Flag Status and Clear */
#define REG_EIC_EVCTRL             (*(RwReg  *)0x40001804UL) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (*(RwReg  *)0x40001808UL) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (*(RwReg  *)0x4000180CUL) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (*(RwReg  *)0x40001810UL) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_WAKEUP             (*(RwReg  *)0x40001814UL) /**< \brief (EIC) Wake-Up Enable */
#define REG_EIC_CONFIG0            (*(RwReg  *)0x40001818UL) /**< \brief (EIC) Configuration 0 */
#define REG_EIC_CONFIG1            (*(RwReg  *)0x4000181CUL) /**< \brief (EIC) Configuration 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EIC peripheral ========== */
#define EIC_CONFIG_NUM              2        // Number of CONFIG registers
#define EIC_GCLK_ID                 5        // Index of Generic Clock

#endif /* _SAMD21_EIC_INSTANCE_ */
