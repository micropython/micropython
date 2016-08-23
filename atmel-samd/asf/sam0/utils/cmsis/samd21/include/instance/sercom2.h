/**
 * \file
 *
 * \brief Instance description for SERCOM2
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

#ifndef _SAMD21_SERCOM2_INSTANCE_
#define _SAMD21_SERCOM2_INSTANCE_

/* ========== Register definition for SERCOM2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM2_I2CM_CTRLA     (0x42001000U) /**< \brief (SERCOM2) I2CM Control A */
#define REG_SERCOM2_I2CM_CTRLB     (0x42001004U) /**< \brief (SERCOM2) I2CM Control B */
#define REG_SERCOM2_I2CM_BAUD      (0x4200100CU) /**< \brief (SERCOM2) I2CM Baud Rate */
#define REG_SERCOM2_I2CM_INTENCLR  (0x42001014U) /**< \brief (SERCOM2) I2CM Interrupt Enable Clear */
#define REG_SERCOM2_I2CM_INTENSET  (0x42001016U) /**< \brief (SERCOM2) I2CM Interrupt Enable Set */
#define REG_SERCOM2_I2CM_INTFLAG   (0x42001018U) /**< \brief (SERCOM2) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM2_I2CM_STATUS    (0x4200101AU) /**< \brief (SERCOM2) I2CM Status */
#define REG_SERCOM2_I2CM_SYNCBUSY  (0x4200101CU) /**< \brief (SERCOM2) I2CM Synchronization Busy */
#define REG_SERCOM2_I2CM_ADDR      (0x42001024U) /**< \brief (SERCOM2) I2CM Address */
#define REG_SERCOM2_I2CM_DATA      (0x42001028U) /**< \brief (SERCOM2) I2CM Data */
#define REG_SERCOM2_I2CM_DBGCTRL   (0x42001030U) /**< \brief (SERCOM2) I2CM Debug Control */
#define REG_SERCOM2_I2CS_CTRLA     (0x42001000U) /**< \brief (SERCOM2) I2CS Control A */
#define REG_SERCOM2_I2CS_CTRLB     (0x42001004U) /**< \brief (SERCOM2) I2CS Control B */
#define REG_SERCOM2_I2CS_INTENCLR  (0x42001014U) /**< \brief (SERCOM2) I2CS Interrupt Enable Clear */
#define REG_SERCOM2_I2CS_INTENSET  (0x42001016U) /**< \brief (SERCOM2) I2CS Interrupt Enable Set */
#define REG_SERCOM2_I2CS_INTFLAG   (0x42001018U) /**< \brief (SERCOM2) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM2_I2CS_STATUS    (0x4200101AU) /**< \brief (SERCOM2) I2CS Status */
#define REG_SERCOM2_I2CS_SYNCBUSY  (0x4200101CU) /**< \brief (SERCOM2) I2CS Synchronization Busy */
#define REG_SERCOM2_I2CS_ADDR      (0x42001024U) /**< \brief (SERCOM2) I2CS Address */
#define REG_SERCOM2_I2CS_DATA      (0x42001028U) /**< \brief (SERCOM2) I2CS Data */
#define REG_SERCOM2_SPI_CTRLA      (0x42001000U) /**< \brief (SERCOM2) SPI Control A */
#define REG_SERCOM2_SPI_CTRLB      (0x42001004U) /**< \brief (SERCOM2) SPI Control B */
#define REG_SERCOM2_SPI_BAUD       (0x4200100CU) /**< \brief (SERCOM2) SPI Baud Rate */
#define REG_SERCOM2_SPI_INTENCLR   (0x42001014U) /**< \brief (SERCOM2) SPI Interrupt Enable Clear */
#define REG_SERCOM2_SPI_INTENSET   (0x42001016U) /**< \brief (SERCOM2) SPI Interrupt Enable Set */
#define REG_SERCOM2_SPI_INTFLAG    (0x42001018U) /**< \brief (SERCOM2) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM2_SPI_STATUS     (0x4200101AU) /**< \brief (SERCOM2) SPI Status */
#define REG_SERCOM2_SPI_SYNCBUSY   (0x4200101CU) /**< \brief (SERCOM2) SPI Synchronization Busy */
#define REG_SERCOM2_SPI_ADDR       (0x42001024U) /**< \brief (SERCOM2) SPI Address */
#define REG_SERCOM2_SPI_DATA       (0x42001028U) /**< \brief (SERCOM2) SPI Data */
#define REG_SERCOM2_SPI_DBGCTRL    (0x42001030U) /**< \brief (SERCOM2) SPI Debug Control */
#define REG_SERCOM2_USART_CTRLA    (0x42001000U) /**< \brief (SERCOM2) USART Control A */
#define REG_SERCOM2_USART_CTRLB    (0x42001004U) /**< \brief (SERCOM2) USART Control B */
#define REG_SERCOM2_USART_BAUD     (0x4200100CU) /**< \brief (SERCOM2) USART Baud Rate */
#define REG_SERCOM2_USART_RXPL     (0x4200100EU) /**< \brief (SERCOM2) USART Receive Pulse Length */
#define REG_SERCOM2_USART_INTENCLR (0x42001014U) /**< \brief (SERCOM2) USART Interrupt Enable Clear */
#define REG_SERCOM2_USART_INTENSET (0x42001016U) /**< \brief (SERCOM2) USART Interrupt Enable Set */
#define REG_SERCOM2_USART_INTFLAG  (0x42001018U) /**< \brief (SERCOM2) USART Interrupt Flag Status and Clear */
#define REG_SERCOM2_USART_STATUS   (0x4200101AU) /**< \brief (SERCOM2) USART Status */
#define REG_SERCOM2_USART_SYNCBUSY (0x4200101CU) /**< \brief (SERCOM2) USART Synchronization Busy */
#define REG_SERCOM2_USART_DATA     (0x42001028U) /**< \brief (SERCOM2) USART Data */
#define REG_SERCOM2_USART_DBGCTRL  (0x42001030U) /**< \brief (SERCOM2) USART Debug Control */
#else
#define REG_SERCOM2_I2CM_CTRLA     (*(RwReg  *)0x42001000U) /**< \brief (SERCOM2) I2CM Control A */
#define REG_SERCOM2_I2CM_CTRLB     (*(RwReg  *)0x42001004U) /**< \brief (SERCOM2) I2CM Control B */
#define REG_SERCOM2_I2CM_BAUD      (*(RwReg  *)0x4200100CU) /**< \brief (SERCOM2) I2CM Baud Rate */
#define REG_SERCOM2_I2CM_INTENCLR  (*(RwReg8 *)0x42001014U) /**< \brief (SERCOM2) I2CM Interrupt Enable Clear */
#define REG_SERCOM2_I2CM_INTENSET  (*(RwReg8 *)0x42001016U) /**< \brief (SERCOM2) I2CM Interrupt Enable Set */
#define REG_SERCOM2_I2CM_INTFLAG   (*(RwReg8 *)0x42001018U) /**< \brief (SERCOM2) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM2_I2CM_STATUS    (*(RwReg16*)0x4200101AU) /**< \brief (SERCOM2) I2CM Status */
#define REG_SERCOM2_I2CM_SYNCBUSY  (*(RoReg  *)0x4200101CU) /**< \brief (SERCOM2) I2CM Synchronization Busy */
#define REG_SERCOM2_I2CM_ADDR      (*(RwReg  *)0x42001024U) /**< \brief (SERCOM2) I2CM Address */
#define REG_SERCOM2_I2CM_DATA      (*(RwReg8 *)0x42001028U) /**< \brief (SERCOM2) I2CM Data */
#define REG_SERCOM2_I2CM_DBGCTRL   (*(RwReg8 *)0x42001030U) /**< \brief (SERCOM2) I2CM Debug Control */
#define REG_SERCOM2_I2CS_CTRLA     (*(RwReg  *)0x42001000U) /**< \brief (SERCOM2) I2CS Control A */
#define REG_SERCOM2_I2CS_CTRLB     (*(RwReg  *)0x42001004U) /**< \brief (SERCOM2) I2CS Control B */
#define REG_SERCOM2_I2CS_INTENCLR  (*(RwReg8 *)0x42001014U) /**< \brief (SERCOM2) I2CS Interrupt Enable Clear */
#define REG_SERCOM2_I2CS_INTENSET  (*(RwReg8 *)0x42001016U) /**< \brief (SERCOM2) I2CS Interrupt Enable Set */
#define REG_SERCOM2_I2CS_INTFLAG   (*(RwReg8 *)0x42001018U) /**< \brief (SERCOM2) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM2_I2CS_STATUS    (*(RwReg16*)0x4200101AU) /**< \brief (SERCOM2) I2CS Status */
#define REG_SERCOM2_I2CS_SYNCBUSY  (*(RoReg  *)0x4200101CU) /**< \brief (SERCOM2) I2CS Synchronization Busy */
#define REG_SERCOM2_I2CS_ADDR      (*(RwReg  *)0x42001024U) /**< \brief (SERCOM2) I2CS Address */
#define REG_SERCOM2_I2CS_DATA      (*(RwReg8 *)0x42001028U) /**< \brief (SERCOM2) I2CS Data */
#define REG_SERCOM2_SPI_CTRLA      (*(RwReg  *)0x42001000U) /**< \brief (SERCOM2) SPI Control A */
#define REG_SERCOM2_SPI_CTRLB      (*(RwReg  *)0x42001004U) /**< \brief (SERCOM2) SPI Control B */
#define REG_SERCOM2_SPI_BAUD       (*(RwReg8 *)0x4200100CU) /**< \brief (SERCOM2) SPI Baud Rate */
#define REG_SERCOM2_SPI_INTENCLR   (*(RwReg8 *)0x42001014U) /**< \brief (SERCOM2) SPI Interrupt Enable Clear */
#define REG_SERCOM2_SPI_INTENSET   (*(RwReg8 *)0x42001016U) /**< \brief (SERCOM2) SPI Interrupt Enable Set */
#define REG_SERCOM2_SPI_INTFLAG    (*(RwReg8 *)0x42001018U) /**< \brief (SERCOM2) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM2_SPI_STATUS     (*(RwReg16*)0x4200101AU) /**< \brief (SERCOM2) SPI Status */
#define REG_SERCOM2_SPI_SYNCBUSY   (*(RoReg  *)0x4200101CU) /**< \brief (SERCOM2) SPI Synchronization Busy */
#define REG_SERCOM2_SPI_ADDR       (*(RwReg  *)0x42001024U) /**< \brief (SERCOM2) SPI Address */
#define REG_SERCOM2_SPI_DATA       (*(RwReg  *)0x42001028U) /**< \brief (SERCOM2) SPI Data */
#define REG_SERCOM2_SPI_DBGCTRL    (*(RwReg8 *)0x42001030U) /**< \brief (SERCOM2) SPI Debug Control */
#define REG_SERCOM2_USART_CTRLA    (*(RwReg  *)0x42001000U) /**< \brief (SERCOM2) USART Control A */
#define REG_SERCOM2_USART_CTRLB    (*(RwReg  *)0x42001004U) /**< \brief (SERCOM2) USART Control B */
#define REG_SERCOM2_USART_BAUD     (*(RwReg16*)0x4200100CU) /**< \brief (SERCOM2) USART Baud Rate */
#define REG_SERCOM2_USART_RXPL     (*(RwReg8 *)0x4200100EU) /**< \brief (SERCOM2) USART Receive Pulse Length */
#define REG_SERCOM2_USART_INTENCLR (*(RwReg8 *)0x42001014U) /**< \brief (SERCOM2) USART Interrupt Enable Clear */
#define REG_SERCOM2_USART_INTENSET (*(RwReg8 *)0x42001016U) /**< \brief (SERCOM2) USART Interrupt Enable Set */
#define REG_SERCOM2_USART_INTFLAG  (*(RwReg8 *)0x42001018U) /**< \brief (SERCOM2) USART Interrupt Flag Status and Clear */
#define REG_SERCOM2_USART_STATUS   (*(RwReg16*)0x4200101AU) /**< \brief (SERCOM2) USART Status */
#define REG_SERCOM2_USART_SYNCBUSY (*(RoReg  *)0x4200101CU) /**< \brief (SERCOM2) USART Synchronization Busy */
#define REG_SERCOM2_USART_DATA     (*(RwReg16*)0x42001028U) /**< \brief (SERCOM2) USART Data */
#define REG_SERCOM2_USART_DBGCTRL  (*(RwReg8 *)0x42001030U) /**< \brief (SERCOM2) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM2 peripheral ========== */
#define SERCOM2_DMAC_ID_RX          5        // Index of DMA RX trigger
#define SERCOM2_DMAC_ID_TX          6        // Index of DMA TX trigger
#define SERCOM2_GCLK_ID_CORE        22       // Index of Generic Clock for Core
#define SERCOM2_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM2_INT_MSB             6       

#endif /* _SAMD21_SERCOM2_INSTANCE_ */
