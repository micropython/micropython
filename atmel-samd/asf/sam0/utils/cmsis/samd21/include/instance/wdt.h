/**
 * \file
 *
 * \brief Instance description for WDT
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

#ifndef _SAMD21_WDT_INSTANCE_
#define _SAMD21_WDT_INSTANCE_

/* ========== Register definition for WDT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_WDT_CTRL               (0x40001000U) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (0x40001001U) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (0x40001002U) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (0x40001004U) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (0x40001005U) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (0x40001006U) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_STATUS             (0x40001007U) /**< \brief (WDT) Status */
#define REG_WDT_CLEAR              (0x40001008U) /**< \brief (WDT) Clear */
#else
#define REG_WDT_CTRL               (*(RwReg8 *)0x40001000U) /**< \brief (WDT) Control */
#define REG_WDT_CONFIG             (*(RwReg8 *)0x40001001U) /**< \brief (WDT) Configuration */
#define REG_WDT_EWCTRL             (*(RwReg8 *)0x40001002U) /**< \brief (WDT) Early Warning Interrupt Control */
#define REG_WDT_INTENCLR           (*(RwReg8 *)0x40001004U) /**< \brief (WDT) Interrupt Enable Clear */
#define REG_WDT_INTENSET           (*(RwReg8 *)0x40001005U) /**< \brief (WDT) Interrupt Enable Set */
#define REG_WDT_INTFLAG            (*(RwReg8 *)0x40001006U) /**< \brief (WDT) Interrupt Flag Status and Clear */
#define REG_WDT_STATUS             (*(RoReg8 *)0x40001007U) /**< \brief (WDT) Status */
#define REG_WDT_CLEAR              (*(WoReg8 *)0x40001008U) /**< \brief (WDT) Clear */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for WDT peripheral ========== */
#define WDT_GCLK_ID                 3        // Index of Generic Clock

#endif /* _SAMD21_WDT_INSTANCE_ */
