/**
 * \file
 *
 * \brief Instance description for SERCOM4
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

#ifndef _SAMD21_SERCOM4_INSTANCE_
#define _SAMD21_SERCOM4_INSTANCE_

/* ========== Register definition for SERCOM4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM4_I2CM_CTRLA     (0x42001800U) /**< \brief (SERCOM4) I2CM Control A */
#define REG_SERCOM4_I2CM_CTRLB     (0x42001804U) /**< \brief (SERCOM4) I2CM Control B */
#define REG_SERCOM4_I2CM_BAUD      (0x4200180CU) /**< \brief (SERCOM4) I2CM Baud Rate */
#define REG_SERCOM4_I2CM_INTENCLR  (0x42001814U) /**< \brief (SERCOM4) I2CM Interrupt Enable Clear */
#define REG_SERCOM4_I2CM_INTENSET  (0x42001816U) /**< \brief (SERCOM4) I2CM Interrupt Enable Set */
#define REG_SERCOM4_I2CM_INTFLAG   (0x42001818U) /**< \brief (SERCOM4) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CM_STATUS    (0x4200181AU) /**< \brief (SERCOM4) I2CM Status */
#define REG_SERCOM4_I2CM_SYNCBUSY  (0x4200181CU) /**< \brief (SERCOM4) I2CM Synchronization Busy */
#define REG_SERCOM4_I2CM_ADDR      (0x42001824U) /**< \brief (SERCOM4) I2CM Address */
#define REG_SERCOM4_I2CM_DATA      (0x42001828U) /**< \brief (SERCOM4) I2CM Data */
#define REG_SERCOM4_I2CM_DBGCTRL   (0x42001830U) /**< \brief (SERCOM4) I2CM Debug Control */
#define REG_SERCOM4_I2CS_CTRLA     (0x42001800U) /**< \brief (SERCOM4) I2CS Control A */
#define REG_SERCOM4_I2CS_CTRLB     (0x42001804U) /**< \brief (SERCOM4) I2CS Control B */
#define REG_SERCOM4_I2CS_INTENCLR  (0x42001814U) /**< \brief (SERCOM4) I2CS Interrupt Enable Clear */
#define REG_SERCOM4_I2CS_INTENSET  (0x42001816U) /**< \brief (SERCOM4) I2CS Interrupt Enable Set */
#define REG_SERCOM4_I2CS_INTFLAG   (0x42001818U) /**< \brief (SERCOM4) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CS_STATUS    (0x4200181AU) /**< \brief (SERCOM4) I2CS Status */
#define REG_SERCOM4_I2CS_SYNCBUSY  (0x4200181CU) /**< \brief (SERCOM4) I2CS Synchronization Busy */
#define REG_SERCOM4_I2CS_ADDR      (0x42001824U) /**< \brief (SERCOM4) I2CS Address */
#define REG_SERCOM4_I2CS_DATA      (0x42001828U) /**< \brief (SERCOM4) I2CS Data */
#define REG_SERCOM4_SPI_CTRLA      (0x42001800U) /**< \brief (SERCOM4) SPI Control A */
#define REG_SERCOM4_SPI_CTRLB      (0x42001804U) /**< \brief (SERCOM4) SPI Control B */
#define REG_SERCOM4_SPI_BAUD       (0x4200180CU) /**< \brief (SERCOM4) SPI Baud Rate */
#define REG_SERCOM4_SPI_INTENCLR   (0x42001814U) /**< \brief (SERCOM4) SPI Interrupt Enable Clear */
#define REG_SERCOM4_SPI_INTENSET   (0x42001816U) /**< \brief (SERCOM4) SPI Interrupt Enable Set */
#define REG_SERCOM4_SPI_INTFLAG    (0x42001818U) /**< \brief (SERCOM4) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM4_SPI_STATUS     (0x4200181AU) /**< \brief (SERCOM4) SPI Status */
#define REG_SERCOM4_SPI_SYNCBUSY   (0x4200181CU) /**< \brief (SERCOM4) SPI Synchronization Busy */
#define REG_SERCOM4_SPI_ADDR       (0x42001824U) /**< \brief (SERCOM4) SPI Address */
#define REG_SERCOM4_SPI_DATA       (0x42001828U) /**< \brief (SERCOM4) SPI Data */
#define REG_SERCOM4_SPI_DBGCTRL    (0x42001830U) /**< \brief (SERCOM4) SPI Debug Control */
#define REG_SERCOM4_USART_CTRLA    (0x42001800U) /**< \brief (SERCOM4) USART Control A */
#define REG_SERCOM4_USART_CTRLB    (0x42001804U) /**< \brief (SERCOM4) USART Control B */
#define REG_SERCOM4_USART_BAUD     (0x4200180CU) /**< \brief (SERCOM4) USART Baud Rate */
#define REG_SERCOM4_USART_RXPL     (0x4200180EU) /**< \brief (SERCOM4) USART Receive Pulse Length */
#define REG_SERCOM4_USART_INTENCLR (0x42001814U) /**< \brief (SERCOM4) USART Interrupt Enable Clear */
#define REG_SERCOM4_USART_INTENSET (0x42001816U) /**< \brief (SERCOM4) USART Interrupt Enable Set */
#define REG_SERCOM4_USART_INTFLAG  (0x42001818U) /**< \brief (SERCOM4) USART Interrupt Flag Status and Clear */
#define REG_SERCOM4_USART_STATUS   (0x4200181AU) /**< \brief (SERCOM4) USART Status */
#define REG_SERCOM4_USART_SYNCBUSY (0x4200181CU) /**< \brief (SERCOM4) USART Synchronization Busy */
#define REG_SERCOM4_USART_DATA     (0x42001828U) /**< \brief (SERCOM4) USART Data */
#define REG_SERCOM4_USART_DBGCTRL  (0x42001830U) /**< \brief (SERCOM4) USART Debug Control */
#else
#define REG_SERCOM4_I2CM_CTRLA     (*(RwReg  *)0x42001800U) /**< \brief (SERCOM4) I2CM Control A */
#define REG_SERCOM4_I2CM_CTRLB     (*(RwReg  *)0x42001804U) /**< \brief (SERCOM4) I2CM Control B */
#define REG_SERCOM4_I2CM_BAUD      (*(RwReg  *)0x4200180CU) /**< \brief (SERCOM4) I2CM Baud Rate */
#define REG_SERCOM4_I2CM_INTENCLR  (*(RwReg8 *)0x42001814U) /**< \brief (SERCOM4) I2CM Interrupt Enable Clear */
#define REG_SERCOM4_I2CM_INTENSET  (*(RwReg8 *)0x42001816U) /**< \brief (SERCOM4) I2CM Interrupt Enable Set */
#define REG_SERCOM4_I2CM_INTFLAG   (*(RwReg8 *)0x42001818U) /**< \brief (SERCOM4) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CM_STATUS    (*(RwReg16*)0x4200181AU) /**< \brief (SERCOM4) I2CM Status */
#define REG_SERCOM4_I2CM_SYNCBUSY  (*(RoReg  *)0x4200181CU) /**< \brief (SERCOM4) I2CM Synchronization Busy */
#define REG_SERCOM4_I2CM_ADDR      (*(RwReg  *)0x42001824U) /**< \brief (SERCOM4) I2CM Address */
#define REG_SERCOM4_I2CM_DATA      (*(RwReg8 *)0x42001828U) /**< \brief (SERCOM4) I2CM Data */
#define REG_SERCOM4_I2CM_DBGCTRL   (*(RwReg8 *)0x42001830U) /**< \brief (SERCOM4) I2CM Debug Control */
#define REG_SERCOM4_I2CS_CTRLA     (*(RwReg  *)0x42001800U) /**< \brief (SERCOM4) I2CS Control A */
#define REG_SERCOM4_I2CS_CTRLB     (*(RwReg  *)0x42001804U) /**< \brief (SERCOM4) I2CS Control B */
#define REG_SERCOM4_I2CS_INTENCLR  (*(RwReg8 *)0x42001814U) /**< \brief (SERCOM4) I2CS Interrupt Enable Clear */
#define REG_SERCOM4_I2CS_INTENSET  (*(RwReg8 *)0x42001816U) /**< \brief (SERCOM4) I2CS Interrupt Enable Set */
#define REG_SERCOM4_I2CS_INTFLAG   (*(RwReg8 *)0x42001818U) /**< \brief (SERCOM4) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM4_I2CS_STATUS    (*(RwReg16*)0x4200181AU) /**< \brief (SERCOM4) I2CS Status */
#define REG_SERCOM4_I2CS_SYNCBUSY  (*(RoReg  *)0x4200181CU) /**< \brief (SERCOM4) I2CS Synchronization Busy */
#define REG_SERCOM4_I2CS_ADDR      (*(RwReg  *)0x42001824U) /**< \brief (SERCOM4) I2CS Address */
#define REG_SERCOM4_I2CS_DATA      (*(RwReg8 *)0x42001828U) /**< \brief (SERCOM4) I2CS Data */
#define REG_SERCOM4_SPI_CTRLA      (*(RwReg  *)0x42001800U) /**< \brief (SERCOM4) SPI Control A */
#define REG_SERCOM4_SPI_CTRLB      (*(RwReg  *)0x42001804U) /**< \brief (SERCOM4) SPI Control B */
#define REG_SERCOM4_SPI_BAUD       (*(RwReg8 *)0x4200180CU) /**< \brief (SERCOM4) SPI Baud Rate */
#define REG_SERCOM4_SPI_INTENCLR   (*(RwReg8 *)0x42001814U) /**< \brief (SERCOM4) SPI Interrupt Enable Clear */
#define REG_SERCOM4_SPI_INTENSET   (*(RwReg8 *)0x42001816U) /**< \brief (SERCOM4) SPI Interrupt Enable Set */
#define REG_SERCOM4_SPI_INTFLAG    (*(RwReg8 *)0x42001818U) /**< \brief (SERCOM4) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM4_SPI_STATUS     (*(RwReg16*)0x4200181AU) /**< \brief (SERCOM4) SPI Status */
#define REG_SERCOM4_SPI_SYNCBUSY   (*(RoReg  *)0x4200181CU) /**< \brief (SERCOM4) SPI Synchronization Busy */
#define REG_SERCOM4_SPI_ADDR       (*(RwReg  *)0x42001824U) /**< \brief (SERCOM4) SPI Address */
#define REG_SERCOM4_SPI_DATA       (*(RwReg  *)0x42001828U) /**< \brief (SERCOM4) SPI Data */
#define REG_SERCOM4_SPI_DBGCTRL    (*(RwReg8 *)0x42001830U) /**< \brief (SERCOM4) SPI Debug Control */
#define REG_SERCOM4_USART_CTRLA    (*(RwReg  *)0x42001800U) /**< \brief (SERCOM4) USART Control A */
#define REG_SERCOM4_USART_CTRLB    (*(RwReg  *)0x42001804U) /**< \brief (SERCOM4) USART Control B */
#define REG_SERCOM4_USART_BAUD     (*(RwReg16*)0x4200180CU) /**< \brief (SERCOM4) USART Baud Rate */
#define REG_SERCOM4_USART_RXPL     (*(RwReg8 *)0x4200180EU) /**< \brief (SERCOM4) USART Receive Pulse Length */
#define REG_SERCOM4_USART_INTENCLR (*(RwReg8 *)0x42001814U) /**< \brief (SERCOM4) USART Interrupt Enable Clear */
#define REG_SERCOM4_USART_INTENSET (*(RwReg8 *)0x42001816U) /**< \brief (SERCOM4) USART Interrupt Enable Set */
#define REG_SERCOM4_USART_INTFLAG  (*(RwReg8 *)0x42001818U) /**< \brief (SERCOM4) USART Interrupt Flag Status and Clear */
#define REG_SERCOM4_USART_STATUS   (*(RwReg16*)0x4200181AU) /**< \brief (SERCOM4) USART Status */
#define REG_SERCOM4_USART_SYNCBUSY (*(RoReg  *)0x4200181CU) /**< \brief (SERCOM4) USART Synchronization Busy */
#define REG_SERCOM4_USART_DATA     (*(RwReg16*)0x42001828U) /**< \brief (SERCOM4) USART Data */
#define REG_SERCOM4_USART_DBGCTRL  (*(RwReg8 *)0x42001830U) /**< \brief (SERCOM4) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM4 peripheral ========== */
#define SERCOM4_DMAC_ID_RX          9        // Index of DMA RX trigger
#define SERCOM4_DMAC_ID_TX          10       // Index of DMA TX trigger
#define SERCOM4_GCLK_ID_CORE        24       // Index of Generic Clock for Core
#define SERCOM4_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM4_INT_MSB             6       

#endif /* _SAMD21_SERCOM4_INSTANCE_ */
