/**
 * \file
 *
 * \brief Instance description for DAC
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

#ifndef _SAMD21_DAC_INSTANCE_
#define _SAMD21_DAC_INSTANCE_

/* ========== Register definition for DAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DAC_CTRLA              (0x42004800) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (0x42004801) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (0x42004802) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (0x42004804) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (0x42004805) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (0x42004806) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (0x42004807) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (0x42004808) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (0x4200480C) /**< \brief (DAC) Data Buffer */
#else
#define REG_DAC_CTRLA              (*(RwReg8 *)0x42004800UL) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (*(RwReg8 *)0x42004801UL) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (*(RwReg8 *)0x42004802UL) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (*(RwReg8 *)0x42004804UL) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (*(RwReg8 *)0x42004805UL) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (*(RwReg8 *)0x42004806UL) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (*(RoReg8 *)0x42004807UL) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (*(RwReg16*)0x42004808UL) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (*(RwReg16*)0x4200480CUL) /**< \brief (DAC) Data Buffer */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DAC peripheral ========== */
#define DAC_DMAC_ID_EMPTY           40       // Index of DMAC EMPTY trigger
#define DAC_GCLK_ID                 33       // Index of Generic Clock

#endif /* _SAMD21_DAC_INSTANCE_ */
