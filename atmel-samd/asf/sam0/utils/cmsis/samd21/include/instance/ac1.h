/**
 * \file
 *
 * \brief Instance description for AC1
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
 *
 */

#ifndef _SAMD21_AC1_INSTANCE_
#define _SAMD21_AC1_INSTANCE_

/* ========== Register definition for AC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AC1_CTRLA              (0x42005400U) /**< \brief (AC1) Control A */
#define REG_AC1_CTRLB              (0x42005401U) /**< \brief (AC1) Control B */
#define REG_AC1_EVCTRL             (0x42005402U) /**< \brief (AC1) Event Control */
#define REG_AC1_INTENCLR           (0x42005404U) /**< \brief (AC1) Interrupt Enable Clear */
#define REG_AC1_INTENSET           (0x42005405U) /**< \brief (AC1) Interrupt Enable Set */
#define REG_AC1_INTFLAG            (0x42005406U) /**< \brief (AC1) Interrupt Flag Status and Clear */
#define REG_AC1_STATUSA            (0x42005408U) /**< \brief (AC1) Status A */
#define REG_AC1_STATUSB            (0x42005409U) /**< \brief (AC1) Status B */
#define REG_AC1_STATUSC            (0x4200540AU) /**< \brief (AC1) Status C */
#define REG_AC1_WINCTRL            (0x4200540CU) /**< \brief (AC1) Window Control */
#define REG_AC1_COMPCTRL0          (0x42005410U) /**< \brief (AC1) Comparator Control 0 */
#define REG_AC1_COMPCTRL1          (0x42005414U) /**< \brief (AC1) Comparator Control 1 */
#define REG_AC1_SCALER0            (0x42005420U) /**< \brief (AC1) Scaler 0 */
#define REG_AC1_SCALER1            (0x42005421U) /**< \brief (AC1) Scaler 1 */
#else
#define REG_AC1_CTRLA              (*(RwReg8 *)0x42005400U) /**< \brief (AC1) Control A */
#define REG_AC1_CTRLB              (*(WoReg8 *)0x42005401U) /**< \brief (AC1) Control B */
#define REG_AC1_EVCTRL             (*(RwReg16*)0x42005402U) /**< \brief (AC1) Event Control */
#define REG_AC1_INTENCLR           (*(RwReg8 *)0x42005404U) /**< \brief (AC1) Interrupt Enable Clear */
#define REG_AC1_INTENSET           (*(RwReg8 *)0x42005405U) /**< \brief (AC1) Interrupt Enable Set */
#define REG_AC1_INTFLAG            (*(RwReg8 *)0x42005406U) /**< \brief (AC1) Interrupt Flag Status and Clear */
#define REG_AC1_STATUSA            (*(RoReg8 *)0x42005408U) /**< \brief (AC1) Status A */
#define REG_AC1_STATUSB            (*(RoReg8 *)0x42005409U) /**< \brief (AC1) Status B */
#define REG_AC1_STATUSC            (*(RoReg8 *)0x4200540AU) /**< \brief (AC1) Status C */
#define REG_AC1_WINCTRL            (*(RwReg8 *)0x4200540CU) /**< \brief (AC1) Window Control */
#define REG_AC1_COMPCTRL0          (*(RwReg  *)0x42005410U) /**< \brief (AC1) Comparator Control 0 */
#define REG_AC1_COMPCTRL1          (*(RwReg  *)0x42005414U) /**< \brief (AC1) Comparator Control 1 */
#define REG_AC1_SCALER0            (*(RwReg8 *)0x42005420U) /**< \brief (AC1) Scaler 0 */
#define REG_AC1_SCALER1            (*(RwReg8 *)0x42005421U) /**< \brief (AC1) Scaler 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AC1 peripheral ========== */
#define AC1_CMP_NUM                 2        // Number of comparators
#define AC1_GCLK_ID_ANA             32       // Index of Generic Clock for analog
#define AC1_GCLK_ID_DIG             31       // Index of Generic Clock for digital
#define AC1_NUM_CMP                 2       
#define AC1_PAIRS                   1        // Number of pairs of comparators

#endif /* _SAMD21_AC1_INSTANCE_ */
