/**
 * \file
 *
 * \brief Instance description for SERCOM0
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

#ifndef _SAMD21_SERCOM0_INSTANCE_
#define _SAMD21_SERCOM0_INSTANCE_

/* ========== Register definition for SERCOM0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM0_I2CM_CTRLA     (0x42000800U) /**< \brief (SERCOM0) I2CM Control A */
#define REG_SERCOM0_I2CM_CTRLB     (0x42000804U) /**< \brief (SERCOM0) I2CM Control B */
#define REG_SERCOM0_I2CM_BAUD      (0x4200080CU) /**< \brief (SERCOM0) I2CM Baud Rate */
#define REG_SERCOM0_I2CM_INTENCLR  (0x42000814U) /**< \brief (SERCOM0) I2CM Interrupt Enable Clear */
#define REG_SERCOM0_I2CM_INTENSET  (0x42000816U) /**< \brief (SERCOM0) I2CM Interrupt Enable Set */
#define REG_SERCOM0_I2CM_INTFLAG   (0x42000818U) /**< \brief (SERCOM0) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CM_STATUS    (0x4200081AU) /**< \brief (SERCOM0) I2CM Status */
#define REG_SERCOM0_I2CM_SYNCBUSY  (0x4200081CU) /**< \brief (SERCOM0) I2CM Synchronization Busy */
#define REG_SERCOM0_I2CM_ADDR      (0x42000824U) /**< \brief (SERCOM0) I2CM Address */
#define REG_SERCOM0_I2CM_DATA      (0x42000828U) /**< \brief (SERCOM0) I2CM Data */
#define REG_SERCOM0_I2CM_DBGCTRL   (0x42000830U) /**< \brief (SERCOM0) I2CM Debug Control */
#define REG_SERCOM0_I2CS_CTRLA     (0x42000800U) /**< \brief (SERCOM0) I2CS Control A */
#define REG_SERCOM0_I2CS_CTRLB     (0x42000804U) /**< \brief (SERCOM0) I2CS Control B */
#define REG_SERCOM0_I2CS_INTENCLR  (0x42000814U) /**< \brief (SERCOM0) I2CS Interrupt Enable Clear */
#define REG_SERCOM0_I2CS_INTENSET  (0x42000816U) /**< \brief (SERCOM0) I2CS Interrupt Enable Set */
#define REG_SERCOM0_I2CS_INTFLAG   (0x42000818U) /**< \brief (SERCOM0) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CS_STATUS    (0x4200081AU) /**< \brief (SERCOM0) I2CS Status */
#define REG_SERCOM0_I2CS_SYNCBUSY  (0x4200081CU) /**< \brief (SERCOM0) I2CS Synchronization Busy */
#define REG_SERCOM0_I2CS_ADDR      (0x42000824U) /**< \brief (SERCOM0) I2CS Address */
#define REG_SERCOM0_I2CS_DATA      (0x42000828U) /**< \brief (SERCOM0) I2CS Data */
#define REG_SERCOM0_SPI_CTRLA      (0x42000800U) /**< \brief (SERCOM0) SPI Control A */
#define REG_SERCOM0_SPI_CTRLB      (0x42000804U) /**< \brief (SERCOM0) SPI Control B */
#define REG_SERCOM0_SPI_BAUD       (0x4200080CU) /**< \brief (SERCOM0) SPI Baud Rate */
#define REG_SERCOM0_SPI_INTENCLR   (0x42000814U) /**< \brief (SERCOM0) SPI Interrupt Enable Clear */
#define REG_SERCOM0_SPI_INTENSET   (0x42000816U) /**< \brief (SERCOM0) SPI Interrupt Enable Set */
#define REG_SERCOM0_SPI_INTFLAG    (0x42000818U) /**< \brief (SERCOM0) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM0_SPI_STATUS     (0x4200081AU) /**< \brief (SERCOM0) SPI Status */
#define REG_SERCOM0_SPI_SYNCBUSY   (0x4200081CU) /**< \brief (SERCOM0) SPI Synchronization Busy */
#define REG_SERCOM0_SPI_ADDR       (0x42000824U) /**< \brief (SERCOM0) SPI Address */
#define REG_SERCOM0_SPI_DATA       (0x42000828U) /**< \brief (SERCOM0) SPI Data */
#define REG_SERCOM0_SPI_DBGCTRL    (0x42000830U) /**< \brief (SERCOM0) SPI Debug Control */
#define REG_SERCOM0_USART_CTRLA    (0x42000800U) /**< \brief (SERCOM0) USART Control A */
#define REG_SERCOM0_USART_CTRLB    (0x42000804U) /**< \brief (SERCOM0) USART Control B */
#define REG_SERCOM0_USART_BAUD     (0x4200080CU) /**< \brief (SERCOM0) USART Baud Rate */
#define REG_SERCOM0_USART_RXPL     (0x4200080EU) /**< \brief (SERCOM0) USART Receive Pulse Length */
#define REG_SERCOM0_USART_INTENCLR (0x42000814U) /**< \brief (SERCOM0) USART Interrupt Enable Clear */
#define REG_SERCOM0_USART_INTENSET (0x42000816U) /**< \brief (SERCOM0) USART Interrupt Enable Set */
#define REG_SERCOM0_USART_INTFLAG  (0x42000818U) /**< \brief (SERCOM0) USART Interrupt Flag Status and Clear */
#define REG_SERCOM0_USART_STATUS   (0x4200081AU) /**< \brief (SERCOM0) USART Status */
#define REG_SERCOM0_USART_SYNCBUSY (0x4200081CU) /**< \brief (SERCOM0) USART Synchronization Busy */
#define REG_SERCOM0_USART_DATA     (0x42000828U) /**< \brief (SERCOM0) USART Data */
#define REG_SERCOM0_USART_DBGCTRL  (0x42000830U) /**< \brief (SERCOM0) USART Debug Control */
#else
#define REG_SERCOM0_I2CM_CTRLA     (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) I2CM Control A */
#define REG_SERCOM0_I2CM_CTRLB     (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) I2CM Control B */
#define REG_SERCOM0_I2CM_BAUD      (*(RwReg  *)0x4200080CU) /**< \brief (SERCOM0) I2CM Baud Rate */
#define REG_SERCOM0_I2CM_INTENCLR  (*(RwReg8 *)0x42000814U) /**< \brief (SERCOM0) I2CM Interrupt Enable Clear */
#define REG_SERCOM0_I2CM_INTENSET  (*(RwReg8 *)0x42000816U) /**< \brief (SERCOM0) I2CM Interrupt Enable Set */
#define REG_SERCOM0_I2CM_INTFLAG   (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CM_STATUS    (*(RwReg16*)0x4200081AU) /**< \brief (SERCOM0) I2CM Status */
#define REG_SERCOM0_I2CM_SYNCBUSY  (*(RoReg  *)0x4200081CU) /**< \brief (SERCOM0) I2CM Synchronization Busy */
#define REG_SERCOM0_I2CM_ADDR      (*(RwReg  *)0x42000824U) /**< \brief (SERCOM0) I2CM Address */
#define REG_SERCOM0_I2CM_DATA      (*(RwReg8 *)0x42000828U) /**< \brief (SERCOM0) I2CM Data */
#define REG_SERCOM0_I2CM_DBGCTRL   (*(RwReg8 *)0x42000830U) /**< \brief (SERCOM0) I2CM Debug Control */
#define REG_SERCOM0_I2CS_CTRLA     (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) I2CS Control A */
#define REG_SERCOM0_I2CS_CTRLB     (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) I2CS Control B */
#define REG_SERCOM0_I2CS_INTENCLR  (*(RwReg8 *)0x42000814U) /**< \brief (SERCOM0) I2CS Interrupt Enable Clear */
#define REG_SERCOM0_I2CS_INTENSET  (*(RwReg8 *)0x42000816U) /**< \brief (SERCOM0) I2CS Interrupt Enable Set */
#define REG_SERCOM0_I2CS_INTFLAG   (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM0_I2CS_STATUS    (*(RwReg16*)0x4200081AU) /**< \brief (SERCOM0) I2CS Status */
#define REG_SERCOM0_I2CS_SYNCBUSY  (*(RoReg  *)0x4200081CU) /**< \brief (SERCOM0) I2CS Synchronization Busy */
#define REG_SERCOM0_I2CS_ADDR      (*(RwReg  *)0x42000824U) /**< \brief (SERCOM0) I2CS Address */
#define REG_SERCOM0_I2CS_DATA      (*(RwReg8 *)0x42000828U) /**< \brief (SERCOM0) I2CS Data */
#define REG_SERCOM0_SPI_CTRLA      (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) SPI Control A */
#define REG_SERCOM0_SPI_CTRLB      (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) SPI Control B */
#define REG_SERCOM0_SPI_BAUD       (*(RwReg8 *)0x4200080CU) /**< \brief (SERCOM0) SPI Baud Rate */
#define REG_SERCOM0_SPI_INTENCLR   (*(RwReg8 *)0x42000814U) /**< \brief (SERCOM0) SPI Interrupt Enable Clear */
#define REG_SERCOM0_SPI_INTENSET   (*(RwReg8 *)0x42000816U) /**< \brief (SERCOM0) SPI Interrupt Enable Set */
#define REG_SERCOM0_SPI_INTFLAG    (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM0_SPI_STATUS     (*(RwReg16*)0x4200081AU) /**< \brief (SERCOM0) SPI Status */
#define REG_SERCOM0_SPI_SYNCBUSY   (*(RoReg  *)0x4200081CU) /**< \brief (SERCOM0) SPI Synchronization Busy */
#define REG_SERCOM0_SPI_ADDR       (*(RwReg  *)0x42000824U) /**< \brief (SERCOM0) SPI Address */
#define REG_SERCOM0_SPI_DATA       (*(RwReg  *)0x42000828U) /**< \brief (SERCOM0) SPI Data */
#define REG_SERCOM0_SPI_DBGCTRL    (*(RwReg8 *)0x42000830U) /**< \brief (SERCOM0) SPI Debug Control */
#define REG_SERCOM0_USART_CTRLA    (*(RwReg  *)0x42000800U) /**< \brief (SERCOM0) USART Control A */
#define REG_SERCOM0_USART_CTRLB    (*(RwReg  *)0x42000804U) /**< \brief (SERCOM0) USART Control B */
#define REG_SERCOM0_USART_BAUD     (*(RwReg16*)0x4200080CU) /**< \brief (SERCOM0) USART Baud Rate */
#define REG_SERCOM0_USART_RXPL     (*(RwReg8 *)0x4200080EU) /**< \brief (SERCOM0) USART Receive Pulse Length */
#define REG_SERCOM0_USART_INTENCLR (*(RwReg8 *)0x42000814U) /**< \brief (SERCOM0) USART Interrupt Enable Clear */
#define REG_SERCOM0_USART_INTENSET (*(RwReg8 *)0x42000816U) /**< \brief (SERCOM0) USART Interrupt Enable Set */
#define REG_SERCOM0_USART_INTFLAG  (*(RwReg8 *)0x42000818U) /**< \brief (SERCOM0) USART Interrupt Flag Status and Clear */
#define REG_SERCOM0_USART_STATUS   (*(RwReg16*)0x4200081AU) /**< \brief (SERCOM0) USART Status */
#define REG_SERCOM0_USART_SYNCBUSY (*(RoReg  *)0x4200081CU) /**< \brief (SERCOM0) USART Synchronization Busy */
#define REG_SERCOM0_USART_DATA     (*(RwReg16*)0x42000828U) /**< \brief (SERCOM0) USART Data */
#define REG_SERCOM0_USART_DBGCTRL  (*(RwReg8 *)0x42000830U) /**< \brief (SERCOM0) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM0 peripheral ========== */
#define SERCOM0_DMAC_ID_RX          1        // Index of DMA RX trigger
#define SERCOM0_DMAC_ID_TX          2        // Index of DMA TX trigger
#define SERCOM0_GCLK_ID_CORE        20       // Index of Generic Clock for Core
#define SERCOM0_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM0_INT_MSB             6       

#endif /* _SAMD21_SERCOM0_INSTANCE_ */
