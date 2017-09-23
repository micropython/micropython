/**
 * \file
 *
 * \brief Instance description for SERCOM5
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

#ifndef _SAMD21_SERCOM5_INSTANCE_
#define _SAMD21_SERCOM5_INSTANCE_

/* ========== Register definition for SERCOM5 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM5_I2CM_CTRLA     (0x42001C00) /**< \brief (SERCOM5) I2CM Control A */
#define REG_SERCOM5_I2CM_CTRLB     (0x42001C04) /**< \brief (SERCOM5) I2CM Control B */
#define REG_SERCOM5_I2CM_BAUD      (0x42001C0C) /**< \brief (SERCOM5) I2CM Baud Rate */
#define REG_SERCOM5_I2CM_INTENCLR  (0x42001C14) /**< \brief (SERCOM5) I2CM Interrupt Enable Clear */
#define REG_SERCOM5_I2CM_INTENSET  (0x42001C16) /**< \brief (SERCOM5) I2CM Interrupt Enable Set */
#define REG_SERCOM5_I2CM_INTFLAG   (0x42001C18) /**< \brief (SERCOM5) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CM_STATUS    (0x42001C1A) /**< \brief (SERCOM5) I2CM Status */
#define REG_SERCOM5_I2CM_SYNCBUSY  (0x42001C1C) /**< \brief (SERCOM5) I2CM Syncbusy */
#define REG_SERCOM5_I2CM_ADDR      (0x42001C24) /**< \brief (SERCOM5) I2CM Address */
#define REG_SERCOM5_I2CM_DATA      (0x42001C28) /**< \brief (SERCOM5) I2CM Data */
#define REG_SERCOM5_I2CM_DBGCTRL   (0x42001C30) /**< \brief (SERCOM5) I2CM Debug Control */
#define REG_SERCOM5_I2CS_CTRLA     (0x42001C00) /**< \brief (SERCOM5) I2CS Control A */
#define REG_SERCOM5_I2CS_CTRLB     (0x42001C04) /**< \brief (SERCOM5) I2CS Control B */
#define REG_SERCOM5_I2CS_INTENCLR  (0x42001C14) /**< \brief (SERCOM5) I2CS Interrupt Enable Clear */
#define REG_SERCOM5_I2CS_INTENSET  (0x42001C16) /**< \brief (SERCOM5) I2CS Interrupt Enable Set */
#define REG_SERCOM5_I2CS_INTFLAG   (0x42001C18) /**< \brief (SERCOM5) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CS_STATUS    (0x42001C1A) /**< \brief (SERCOM5) I2CS Status */
#define REG_SERCOM5_I2CS_SYNCBUSY  (0x42001C1C) /**< \brief (SERCOM5) I2CS Syncbusy */
#define REG_SERCOM5_I2CS_ADDR      (0x42001C24) /**< \brief (SERCOM5) I2CS Address */
#define REG_SERCOM5_I2CS_DATA      (0x42001C28) /**< \brief (SERCOM5) I2CS Data */
#define REG_SERCOM5_SPI_CTRLA      (0x42001C00) /**< \brief (SERCOM5) SPI Control A */
#define REG_SERCOM5_SPI_CTRLB      (0x42001C04) /**< \brief (SERCOM5) SPI Control B */
#define REG_SERCOM5_SPI_BAUD       (0x42001C0C) /**< \brief (SERCOM5) SPI Baud Rate */
#define REG_SERCOM5_SPI_INTENCLR   (0x42001C14) /**< \brief (SERCOM5) SPI Interrupt Enable Clear */
#define REG_SERCOM5_SPI_INTENSET   (0x42001C16) /**< \brief (SERCOM5) SPI Interrupt Enable Set */
#define REG_SERCOM5_SPI_INTFLAG    (0x42001C18) /**< \brief (SERCOM5) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM5_SPI_STATUS     (0x42001C1A) /**< \brief (SERCOM5) SPI Status */
#define REG_SERCOM5_SPI_SYNCBUSY   (0x42001C1C) /**< \brief (SERCOM5) SPI Syncbusy */
#define REG_SERCOM5_SPI_ADDR       (0x42001C24) /**< \brief (SERCOM5) SPI Address */
#define REG_SERCOM5_SPI_DATA       (0x42001C28) /**< \brief (SERCOM5) SPI Data */
#define REG_SERCOM5_SPI_DBGCTRL    (0x42001C30) /**< \brief (SERCOM5) SPI Debug Control */
#define REG_SERCOM5_USART_CTRLA    (0x42001C00) /**< \brief (SERCOM5) USART Control A */
#define REG_SERCOM5_USART_CTRLB    (0x42001C04) /**< \brief (SERCOM5) USART Control B */
#define REG_SERCOM5_USART_BAUD     (0x42001C0C) /**< \brief (SERCOM5) USART Baud Rate */
#define REG_SERCOM5_USART_RXPL     (0x42001C0E) /**< \brief (SERCOM5) USART Receive Pulse Length */
#define REG_SERCOM5_USART_INTENCLR (0x42001C14) /**< \brief (SERCOM5) USART Interrupt Enable Clear */
#define REG_SERCOM5_USART_INTENSET (0x42001C16) /**< \brief (SERCOM5) USART Interrupt Enable Set */
#define REG_SERCOM5_USART_INTFLAG  (0x42001C18) /**< \brief (SERCOM5) USART Interrupt Flag Status and Clear */
#define REG_SERCOM5_USART_STATUS   (0x42001C1A) /**< \brief (SERCOM5) USART Status */
#define REG_SERCOM5_USART_SYNCBUSY (0x42001C1C) /**< \brief (SERCOM5) USART Syncbusy */
#define REG_SERCOM5_USART_DATA     (0x42001C28) /**< \brief (SERCOM5) USART Data */
#define REG_SERCOM5_USART_DBGCTRL  (0x42001C30) /**< \brief (SERCOM5) USART Debug Control */
#else
#define REG_SERCOM5_I2CM_CTRLA     (*(RwReg  *)0x42001C00UL) /**< \brief (SERCOM5) I2CM Control A */
#define REG_SERCOM5_I2CM_CTRLB     (*(RwReg  *)0x42001C04UL) /**< \brief (SERCOM5) I2CM Control B */
#define REG_SERCOM5_I2CM_BAUD      (*(RwReg  *)0x42001C0CUL) /**< \brief (SERCOM5) I2CM Baud Rate */
#define REG_SERCOM5_I2CM_INTENCLR  (*(RwReg8 *)0x42001C14UL) /**< \brief (SERCOM5) I2CM Interrupt Enable Clear */
#define REG_SERCOM5_I2CM_INTENSET  (*(RwReg8 *)0x42001C16UL) /**< \brief (SERCOM5) I2CM Interrupt Enable Set */
#define REG_SERCOM5_I2CM_INTFLAG   (*(RwReg8 *)0x42001C18UL) /**< \brief (SERCOM5) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CM_STATUS    (*(RwReg16*)0x42001C1AUL) /**< \brief (SERCOM5) I2CM Status */
#define REG_SERCOM5_I2CM_SYNCBUSY  (*(RoReg  *)0x42001C1CUL) /**< \brief (SERCOM5) I2CM Syncbusy */
#define REG_SERCOM5_I2CM_ADDR      (*(RwReg  *)0x42001C24UL) /**< \brief (SERCOM5) I2CM Address */
#define REG_SERCOM5_I2CM_DATA      (*(RwReg8 *)0x42001C28UL) /**< \brief (SERCOM5) I2CM Data */
#define REG_SERCOM5_I2CM_DBGCTRL   (*(RwReg8 *)0x42001C30UL) /**< \brief (SERCOM5) I2CM Debug Control */
#define REG_SERCOM5_I2CS_CTRLA     (*(RwReg  *)0x42001C00UL) /**< \brief (SERCOM5) I2CS Control A */
#define REG_SERCOM5_I2CS_CTRLB     (*(RwReg  *)0x42001C04UL) /**< \brief (SERCOM5) I2CS Control B */
#define REG_SERCOM5_I2CS_INTENCLR  (*(RwReg8 *)0x42001C14UL) /**< \brief (SERCOM5) I2CS Interrupt Enable Clear */
#define REG_SERCOM5_I2CS_INTENSET  (*(RwReg8 *)0x42001C16UL) /**< \brief (SERCOM5) I2CS Interrupt Enable Set */
#define REG_SERCOM5_I2CS_INTFLAG   (*(RwReg8 *)0x42001C18UL) /**< \brief (SERCOM5) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CS_STATUS    (*(RwReg16*)0x42001C1AUL) /**< \brief (SERCOM5) I2CS Status */
#define REG_SERCOM5_I2CS_SYNCBUSY  (*(RoReg  *)0x42001C1CUL) /**< \brief (SERCOM5) I2CS Syncbusy */
#define REG_SERCOM5_I2CS_ADDR      (*(RwReg  *)0x42001C24UL) /**< \brief (SERCOM5) I2CS Address */
#define REG_SERCOM5_I2CS_DATA      (*(RwReg8 *)0x42001C28UL) /**< \brief (SERCOM5) I2CS Data */
#define REG_SERCOM5_SPI_CTRLA      (*(RwReg  *)0x42001C00UL) /**< \brief (SERCOM5) SPI Control A */
#define REG_SERCOM5_SPI_CTRLB      (*(RwReg  *)0x42001C04UL) /**< \brief (SERCOM5) SPI Control B */
#define REG_SERCOM5_SPI_BAUD       (*(RwReg8 *)0x42001C0CUL) /**< \brief (SERCOM5) SPI Baud Rate */
#define REG_SERCOM5_SPI_INTENCLR   (*(RwReg8 *)0x42001C14UL) /**< \brief (SERCOM5) SPI Interrupt Enable Clear */
#define REG_SERCOM5_SPI_INTENSET   (*(RwReg8 *)0x42001C16UL) /**< \brief (SERCOM5) SPI Interrupt Enable Set */
#define REG_SERCOM5_SPI_INTFLAG    (*(RwReg8 *)0x42001C18UL) /**< \brief (SERCOM5) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM5_SPI_STATUS     (*(RwReg16*)0x42001C1AUL) /**< \brief (SERCOM5) SPI Status */
#define REG_SERCOM5_SPI_SYNCBUSY   (*(RoReg  *)0x42001C1CUL) /**< \brief (SERCOM5) SPI Syncbusy */
#define REG_SERCOM5_SPI_ADDR       (*(RwReg  *)0x42001C24UL) /**< \brief (SERCOM5) SPI Address */
#define REG_SERCOM5_SPI_DATA       (*(RwReg  *)0x42001C28UL) /**< \brief (SERCOM5) SPI Data */
#define REG_SERCOM5_SPI_DBGCTRL    (*(RwReg8 *)0x42001C30UL) /**< \brief (SERCOM5) SPI Debug Control */
#define REG_SERCOM5_USART_CTRLA    (*(RwReg  *)0x42001C00UL) /**< \brief (SERCOM5) USART Control A */
#define REG_SERCOM5_USART_CTRLB    (*(RwReg  *)0x42001C04UL) /**< \brief (SERCOM5) USART Control B */
#define REG_SERCOM5_USART_BAUD     (*(RwReg16*)0x42001C0CUL) /**< \brief (SERCOM5) USART Baud Rate */
#define REG_SERCOM5_USART_RXPL     (*(RwReg8 *)0x42001C0EUL) /**< \brief (SERCOM5) USART Receive Pulse Length */
#define REG_SERCOM5_USART_INTENCLR (*(RwReg8 *)0x42001C14UL) /**< \brief (SERCOM5) USART Interrupt Enable Clear */
#define REG_SERCOM5_USART_INTENSET (*(RwReg8 *)0x42001C16UL) /**< \brief (SERCOM5) USART Interrupt Enable Set */
#define REG_SERCOM5_USART_INTFLAG  (*(RwReg8 *)0x42001C18UL) /**< \brief (SERCOM5) USART Interrupt Flag Status and Clear */
#define REG_SERCOM5_USART_STATUS   (*(RwReg16*)0x42001C1AUL) /**< \brief (SERCOM5) USART Status */
#define REG_SERCOM5_USART_SYNCBUSY (*(RoReg  *)0x42001C1CUL) /**< \brief (SERCOM5) USART Syncbusy */
#define REG_SERCOM5_USART_DATA     (*(RwReg16*)0x42001C28UL) /**< \brief (SERCOM5) USART Data */
#define REG_SERCOM5_USART_DBGCTRL  (*(RwReg8 *)0x42001C30UL) /**< \brief (SERCOM5) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM5 peripheral ========== */
#define SERCOM5_DMAC_ID_RX          11       // Index of DMA RX trigger
#define SERCOM5_DMAC_ID_TX          12       // Index of DMA TX trigger
#define SERCOM5_GCLK_ID_CORE        25       // Index of Generic Clock for Core
#define SERCOM5_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM5_INT_MSB             6       

#endif /* _SAMD21_SERCOM5_INSTANCE_ */
