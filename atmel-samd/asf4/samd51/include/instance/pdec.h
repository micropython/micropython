/**
 * \file
 *
 * \brief Instance description for PDEC
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_PDEC_INSTANCE_
#define _SAMD51_PDEC_INSTANCE_

/* ========== Register definition for PDEC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PDEC_CTRLA             (0x42001C00) /**< \brief (PDEC) Control A */
#define REG_PDEC_CTRLBCLR          (0x42001C04) /**< \brief (PDEC) Control B Clear */
#define REG_PDEC_CTRLBSET          (0x42001C05) /**< \brief (PDEC) Control B Set */
#define REG_PDEC_EVCTRL            (0x42001C06) /**< \brief (PDEC) Event Control */
#define REG_PDEC_INTENCLR          (0x42001C08) /**< \brief (PDEC) Interrupt Enable Clear */
#define REG_PDEC_INTENSET          (0x42001C09) /**< \brief (PDEC) Interrupt Enable Set */
#define REG_PDEC_INTFLAG           (0x42001C0A) /**< \brief (PDEC) Interrupt Flag Status and Clear */
#define REG_PDEC_STATUS            (0x42001C0C) /**< \brief (PDEC) Status */
#define REG_PDEC_DBGCTRL           (0x42001C0F) /**< \brief (PDEC) Debug Control */
#define REG_PDEC_SYNCBUSY          (0x42001C10) /**< \brief (PDEC) Synchronization Status */
#define REG_PDEC_PRESC             (0x42001C14) /**< \brief (PDEC) Prescaler Value */
#define REG_PDEC_FILTER            (0x42001C15) /**< \brief (PDEC) Filter Value */
#define REG_PDEC_PRESCBUF          (0x42001C18) /**< \brief (PDEC) Prescaler Buffer Value */
#define REG_PDEC_FILTERBUF         (0x42001C19) /**< \brief (PDEC) Filter Buffer Value */
#define REG_PDEC_COUNT             (0x42001C1C) /**< \brief (PDEC) Counter Value */
#define REG_PDEC_CC0               (0x42001C20) /**< \brief (PDEC) Channel 0 Compare Value */
#define REG_PDEC_CC1               (0x42001C24) /**< \brief (PDEC) Channel 1 Compare Value */
#define REG_PDEC_CCBUF0            (0x42001C30) /**< \brief (PDEC) Channel Compare Buffer Value 0 */
#define REG_PDEC_CCBUF1            (0x42001C34) /**< \brief (PDEC) Channel Compare Buffer Value 1 */
#else
#define REG_PDEC_CTRLA             (*(RwReg  *)0x42001C00UL) /**< \brief (PDEC) Control A */
#define REG_PDEC_CTRLBCLR          (*(RwReg8 *)0x42001C04UL) /**< \brief (PDEC) Control B Clear */
#define REG_PDEC_CTRLBSET          (*(RwReg8 *)0x42001C05UL) /**< \brief (PDEC) Control B Set */
#define REG_PDEC_EVCTRL            (*(RwReg16*)0x42001C06UL) /**< \brief (PDEC) Event Control */
#define REG_PDEC_INTENCLR          (*(RwReg8 *)0x42001C08UL) /**< \brief (PDEC) Interrupt Enable Clear */
#define REG_PDEC_INTENSET          (*(RwReg8 *)0x42001C09UL) /**< \brief (PDEC) Interrupt Enable Set */
#define REG_PDEC_INTFLAG           (*(RwReg8 *)0x42001C0AUL) /**< \brief (PDEC) Interrupt Flag Status and Clear */
#define REG_PDEC_STATUS            (*(RwReg16*)0x42001C0CUL) /**< \brief (PDEC) Status */
#define REG_PDEC_DBGCTRL           (*(RwReg8 *)0x42001C0FUL) /**< \brief (PDEC) Debug Control */
#define REG_PDEC_SYNCBUSY          (*(RoReg  *)0x42001C10UL) /**< \brief (PDEC) Synchronization Status */
#define REG_PDEC_PRESC             (*(RwReg8 *)0x42001C14UL) /**< \brief (PDEC) Prescaler Value */
#define REG_PDEC_FILTER            (*(RwReg8 *)0x42001C15UL) /**< \brief (PDEC) Filter Value */
#define REG_PDEC_PRESCBUF          (*(RwReg8 *)0x42001C18UL) /**< \brief (PDEC) Prescaler Buffer Value */
#define REG_PDEC_FILTERBUF         (*(RwReg8 *)0x42001C19UL) /**< \brief (PDEC) Filter Buffer Value */
#define REG_PDEC_COUNT             (*(RwReg  *)0x42001C1CUL) /**< \brief (PDEC) Counter Value */
#define REG_PDEC_CC0               (*(RwReg  *)0x42001C20UL) /**< \brief (PDEC) Channel 0 Compare Value */
#define REG_PDEC_CC1               (*(RwReg  *)0x42001C24UL) /**< \brief (PDEC) Channel 1 Compare Value */
#define REG_PDEC_CCBUF0            (*(RwReg  *)0x42001C30UL) /**< \brief (PDEC) Channel Compare Buffer Value 0 */
#define REG_PDEC_CCBUF1            (*(RwReg  *)0x42001C34UL) /**< \brief (PDEC) Channel Compare Buffer Value 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PDEC peripheral ========== */
#define PDEC_CC_NUM                 2        // Number of Compare Channels units
#define PDEC_GCLK_ID                31      

#endif /* _SAMD51_PDEC_INSTANCE_ */
