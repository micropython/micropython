/**
 * \file
 *
 * \brief Instance description for SERCOM4
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

#ifndef _SAMD21_SERCOM4_INSTANCE_
#define _SAMD21_SERCOM4_INSTANCE_

/* ========== Register definition for SERCOM4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM4_I2CM_CTRLA     (0x42001800) /**< \brief (SERCOM4) I2CM Control A */
#define REG_SERCOM4_I2CM_CTRLB     (0x42001804) /**< \brief (SERCOM4) I2CM Control B */
#define REG_SERCOM4_I2CM_BAUD      (0x4200180C) /**< \brief (SERCOM4) I2CM Baud Rate */
#define REG_SERCOM4_I2CM_INTENCLR  (0x42001814) /**< \brief (SERCOM4) I2CM Interrupt Enable Clear */
#define REG_SERCOM4_I2CM_INTENSET  (0x42001816) /**< \brief (SERCOM4) I2CM Interrupt Enable Set */
#define REG_SERCOM4_I2CM_INTFLAG   (0x42001818) /**< \brief (SERCOM4) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CM_STATUS    (0x4200181A) /**< \brief (SERCOM4) I2CM Status */
#define REG_SERCOM4_I2CM_SYNCBUSY  (0x4200181C) /**< \brief (SERCOM4) I2CM Syncbusy */
#define REG_SERCOM4_I2CM_ADDR      (0x42001824) /**< \brief (SERCOM4) I2CM Address */
#define REG_SERCOM4_I2CM_DATA      (0x42001828) /**< \brief (SERCOM4) I2CM Data */
#define REG_SERCOM4_I2CM_DBGCTRL   (0x42001830) /**< \brief (SERCOM4) I2CM Debug Control */
#define REG_SERCOM4_I2CS_CTRLA     (0x42001800) /**< \brief (SERCOM4) I2CS Control A */
#define REG_SERCOM4_I2CS_CTRLB     (0x42001804) /**< \brief (SERCOM4) I2CS Control B */
#define REG_SERCOM4_I2CS_INTENCLR  (0x42001814) /**< \brief (SERCOM4) I2CS Interrupt Enable Clear */
#define REG_SERCOM4_I2CS_INTENSET  (0x42001816) /**< \brief (SERCOM4) I2CS Interrupt Enable Set */
#define REG_SERCOM4_I2CS_INTFLAG   (0x42001818) /**< \brief (SERCOM4) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CS_STATUS    (0x4200181A) /**< \brief (SERCOM4) I2CS Status */
#define REG_SERCOM4_I2CS_SYNCBUSY  (0x4200181C) /**< \brief (SERCOM4) I2CS Syncbusy */
#define REG_SERCOM4_I2CS_ADDR      (0x42001824) /**< \brief (SERCOM4) I2CS Address */
#define REG_SERCOM4_I2CS_DATA      (0x42001828) /**< \brief (SERCOM4) I2CS Data */
#define REG_SERCOM4_SPI_CTRLA      (0x42001800) /**< \brief (SERCOM4) SPI Control A */
#define REG_SERCOM4_SPI_CTRLB      (0x42001804) /**< \brief (SERCOM4) SPI Control B */
#define REG_SERCOM4_SPI_BAUD       (0x4200180C) /**< \brief (SERCOM4) SPI Baud Rate */
#define REG_SERCOM4_SPI_INTENCLR   (0x42001814) /**< \brief (SERCOM4) SPI Interrupt Enable Clear */
#define REG_SERCOM4_SPI_INTENSET   (0x42001816) /**< \brief (SERCOM4) SPI Interrupt Enable Set */
#define REG_SERCOM4_SPI_INTFLAG    (0x42001818) /**< \brief (SERCOM4) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM4_SPI_STATUS     (0x4200181A) /**< \brief (SERCOM4) SPI Status */
#define REG_SERCOM4_SPI_SYNCBUSY   (0x4200181C) /**< \brief (SERCOM4) SPI Syncbusy */
#define REG_SERCOM4_SPI_ADDR       (0x42001824) /**< \brief (SERCOM4) SPI Address */
#define REG_SERCOM4_SPI_DATA       (0x42001828) /**< \brief (SERCOM4) SPI Data */
#define REG_SERCOM4_SPI_DBGCTRL    (0x42001830) /**< \brief (SERCOM4) SPI Debug Control */
#define REG_SERCOM4_USART_CTRLA    (0x42001800) /**< \brief (SERCOM4) USART Control A */
#define REG_SERCOM4_USART_CTRLB    (0x42001804) /**< \brief (SERCOM4) USART Control B */
#define REG_SERCOM4_USART_BAUD     (0x4200180C) /**< \brief (SERCOM4) USART Baud Rate */
#define REG_SERCOM4_USART_RXPL     (0x4200180E) /**< \brief (SERCOM4) USART Receive Pulse Length */
#define REG_SERCOM4_USART_INTENCLR (0x42001814) /**< \brief (SERCOM4) USART Interrupt Enable Clear */
#define REG_SERCOM4_USART_INTENSET (0x42001816) /**< \brief (SERCOM4) USART Interrupt Enable Set */
#define REG_SERCOM4_USART_INTFLAG  (0x42001818) /**< \brief (SERCOM4) USART Interrupt Flag Status and Clear */
#define REG_SERCOM4_USART_STATUS   (0x4200181A) /**< \brief (SERCOM4) USART Status */
#define REG_SERCOM4_USART_SYNCBUSY (0x4200181C) /**< \brief (SERCOM4) USART Syncbusy */
#define REG_SERCOM4_USART_DATA     (0x42001828) /**< \brief (SERCOM4) USART Data */
#define REG_SERCOM4_USART_DBGCTRL  (0x42001830) /**< \brief (SERCOM4) USART Debug Control */
#else
#define REG_SERCOM4_I2CM_CTRLA     (*(RwReg  *)0x42001800UL) /**< \brief (SERCOM4) I2CM Control A */
#define REG_SERCOM4_I2CM_CTRLB     (*(RwReg  *)0x42001804UL) /**< \brief (SERCOM4) I2CM Control B */
#define REG_SERCOM4_I2CM_BAUD      (*(RwReg  *)0x4200180CUL) /**< \brief (SERCOM4) I2CM Baud Rate */
#define REG_SERCOM4_I2CM_INTENCLR  (*(RwReg8 *)0x42001814UL) /**< \brief (SERCOM4) I2CM Interrupt Enable Clear */
#define REG_SERCOM4_I2CM_INTENSET  (*(RwReg8 *)0x42001816UL) /**< \brief (SERCOM4) I2CM Interrupt Enable Set */
#define REG_SERCOM4_I2CM_INTFLAG   (*(RwReg8 *)0x42001818UL) /**< \brief (SERCOM4) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CM_STATUS    (*(RwReg16*)0x4200181AUL) /**< \brief (SERCOM4) I2CM Status */
#define REG_SERCOM4_I2CM_SYNCBUSY  (*(RoReg  *)0x4200181CUL) /**< \brief (SERCOM4) I2CM Syncbusy */
#define REG_SERCOM4_I2CM_ADDR      (*(RwReg  *)0x42001824UL) /**< \brief (SERCOM4) I2CM Address */
#define REG_SERCOM4_I2CM_DATA      (*(RwReg8 *)0x42001828UL) /**< \brief (SERCOM4) I2CM Data */
#define REG_SERCOM4_I2CM_DBGCTRL   (*(RwReg8 *)0x42001830UL) /**< \brief (SERCOM4) I2CM Debug Control */
#define REG_SERCOM4_I2CS_CTRLA     (*(RwReg  *)0x42001800UL) /**< \brief (SERCOM4) I2CS Control A */
#define REG_SERCOM4_I2CS_CTRLB     (*(RwReg  *)0x42001804UL) /**< \brief (SERCOM4) I2CS Control B */
#define REG_SERCOM4_I2CS_INTENCLR  (*(RwReg8 *)0x42001814UL) /**< \brief (SERCOM4) I2CS Interrupt Enable Clear */
#define REG_SERCOM4_I2CS_INTENSET  (*(RwReg8 *)0x42001816UL) /**< \brief (SERCOM4) I2CS Interrupt Enable Set */
#define REG_SERCOM4_I2CS_INTFLAG   (*(RwReg8 *)0x42001818UL) /**< \brief (SERCOM4) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CS_STATUS    (*(RwReg16*)0x4200181AUL) /**< \brief (SERCOM4) I2CS Status */
#define REG_SERCOM4_I2CS_SYNCBUSY  (*(RoReg  *)0x4200181CUL) /**< \brief (SERCOM4) I2CS Syncbusy */
#define REG_SERCOM4_I2CS_ADDR      (*(RwReg  *)0x42001824UL) /**< \brief (SERCOM4) I2CS Address */
#define REG_SERCOM4_I2CS_DATA      (*(RwReg8 *)0x42001828UL) /**< \brief (SERCOM4) I2CS Data */
#define REG_SERCOM4_SPI_CTRLA      (*(RwReg  *)0x42001800UL) /**< \brief (SERCOM4) SPI Control A */
#define REG_SERCOM4_SPI_CTRLB      (*(RwReg  *)0x42001804UL) /**< \brief (SERCOM4) SPI Control B */
#define REG_SERCOM4_SPI_BAUD       (*(RwReg8 *)0x4200180CUL) /**< \brief (SERCOM4) SPI Baud Rate */
#define REG_SERCOM4_SPI_INTENCLR   (*(RwReg8 *)0x42001814UL) /**< \brief (SERCOM4) SPI Interrupt Enable Clear */
#define REG_SERCOM4_SPI_INTENSET   (*(RwReg8 *)0x42001816UL) /**< \brief (SERCOM4) SPI Interrupt Enable Set */
#define REG_SERCOM4_SPI_INTFLAG    (*(RwReg8 *)0x42001818UL) /**< \brief (SERCOM4) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM4_SPI_STATUS     (*(RwReg16*)0x4200181AUL) /**< \brief (SERCOM4) SPI Status */
#define REG_SERCOM4_SPI_SYNCBUSY   (*(RoReg  *)0x4200181CUL) /**< \brief (SERCOM4) SPI Syncbusy */
#define REG_SERCOM4_SPI_ADDR       (*(RwReg  *)0x42001824UL) /**< \brief (SERCOM4) SPI Address */
#define REG_SERCOM4_SPI_DATA       (*(RwReg  *)0x42001828UL) /**< \brief (SERCOM4) SPI Data */
#define REG_SERCOM4_SPI_DBGCTRL    (*(RwReg8 *)0x42001830UL) /**< \brief (SERCOM4) SPI Debug Control */
#define REG_SERCOM4_USART_CTRLA    (*(RwReg  *)0x42001800UL) /**< \brief (SERCOM4) USART Control A */
#define REG_SERCOM4_USART_CTRLB    (*(RwReg  *)0x42001804UL) /**< \brief (SERCOM4) USART Control B */
#define REG_SERCOM4_USART_BAUD     (*(RwReg16*)0x4200180CUL) /**< \brief (SERCOM4) USART Baud Rate */
#define REG_SERCOM4_USART_RXPL     (*(RwReg8 *)0x4200180EUL) /**< \brief (SERCOM4) USART Receive Pulse Length */
#define REG_SERCOM4_USART_INTENCLR (*(RwReg8 *)0x42001814UL) /**< \brief (SERCOM4) USART Interrupt Enable Clear */
#define REG_SERCOM4_USART_INTENSET (*(RwReg8 *)0x42001816UL) /**< \brief (SERCOM4) USART Interrupt Enable Set */
#define REG_SERCOM4_USART_INTFLAG  (*(RwReg8 *)0x42001818UL) /**< \brief (SERCOM4) USART Interrupt Flag Status and Clear */
#define REG_SERCOM4_USART_STATUS   (*(RwReg16*)0x4200181AUL) /**< \brief (SERCOM4) USART Status */
#define REG_SERCOM4_USART_SYNCBUSY (*(RoReg  *)0x4200181CUL) /**< \brief (SERCOM4) USART Syncbusy */
#define REG_SERCOM4_USART_DATA     (*(RwReg16*)0x42001828UL) /**< \brief (SERCOM4) USART Data */
#define REG_SERCOM4_USART_DBGCTRL  (*(RwReg8 *)0x42001830UL) /**< \brief (SERCOM4) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM4 peripheral ========== */
#define SERCOM4_DMAC_ID_RX          9        // Index of DMA RX trigger
#define SERCOM4_DMAC_ID_TX          10       // Index of DMA TX trigger
#define SERCOM4_GCLK_ID_CORE        24       // Index of Generic Clock for Core
#define SERCOM4_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM4_INT_MSB             6       

#endif /* _SAMD21_SERCOM4_INSTANCE_ */
