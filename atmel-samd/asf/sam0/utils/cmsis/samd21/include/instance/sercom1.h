/**
 * \file
 *
 * \brief Instance description for SERCOM1
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

#ifndef _SAMD21_SERCOM1_INSTANCE_
#define _SAMD21_SERCOM1_INSTANCE_

/* ========== Register definition for SERCOM1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM1_I2CM_CTRLA     (0x42000C00U) /**< \brief (SERCOM1) I2CM Control A */
#define REG_SERCOM1_I2CM_CTRLB     (0x42000C04U) /**< \brief (SERCOM1) I2CM Control B */
#define REG_SERCOM1_I2CM_BAUD      (0x42000C0CU) /**< \brief (SERCOM1) I2CM Baud Rate */
#define REG_SERCOM1_I2CM_INTENCLR  (0x42000C14U) /**< \brief (SERCOM1) I2CM Interrupt Enable Clear */
#define REG_SERCOM1_I2CM_INTENSET  (0x42000C16U) /**< \brief (SERCOM1) I2CM Interrupt Enable Set */
#define REG_SERCOM1_I2CM_INTFLAG   (0x42000C18U) /**< \brief (SERCOM1) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CM_STATUS    (0x42000C1AU) /**< \brief (SERCOM1) I2CM Status */
#define REG_SERCOM1_I2CM_SYNCBUSY  (0x42000C1CU) /**< \brief (SERCOM1) I2CM Synchronization Busy */
#define REG_SERCOM1_I2CM_ADDR      (0x42000C24U) /**< \brief (SERCOM1) I2CM Address */
#define REG_SERCOM1_I2CM_DATA      (0x42000C28U) /**< \brief (SERCOM1) I2CM Data */
#define REG_SERCOM1_I2CM_DBGCTRL   (0x42000C30U) /**< \brief (SERCOM1) I2CM Debug Control */
#define REG_SERCOM1_I2CS_CTRLA     (0x42000C00U) /**< \brief (SERCOM1) I2CS Control A */
#define REG_SERCOM1_I2CS_CTRLB     (0x42000C04U) /**< \brief (SERCOM1) I2CS Control B */
#define REG_SERCOM1_I2CS_INTENCLR  (0x42000C14U) /**< \brief (SERCOM1) I2CS Interrupt Enable Clear */
#define REG_SERCOM1_I2CS_INTENSET  (0x42000C16U) /**< \brief (SERCOM1) I2CS Interrupt Enable Set */
#define REG_SERCOM1_I2CS_INTFLAG   (0x42000C18U) /**< \brief (SERCOM1) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CS_STATUS    (0x42000C1AU) /**< \brief (SERCOM1) I2CS Status */
#define REG_SERCOM1_I2CS_SYNCBUSY  (0x42000C1CU) /**< \brief (SERCOM1) I2CS Synchronization Busy */
#define REG_SERCOM1_I2CS_ADDR      (0x42000C24U) /**< \brief (SERCOM1) I2CS Address */
#define REG_SERCOM1_I2CS_DATA      (0x42000C28U) /**< \brief (SERCOM1) I2CS Data */
#define REG_SERCOM1_SPI_CTRLA      (0x42000C00U) /**< \brief (SERCOM1) SPI Control A */
#define REG_SERCOM1_SPI_CTRLB      (0x42000C04U) /**< \brief (SERCOM1) SPI Control B */
#define REG_SERCOM1_SPI_BAUD       (0x42000C0CU) /**< \brief (SERCOM1) SPI Baud Rate */
#define REG_SERCOM1_SPI_INTENCLR   (0x42000C14U) /**< \brief (SERCOM1) SPI Interrupt Enable Clear */
#define REG_SERCOM1_SPI_INTENSET   (0x42000C16U) /**< \brief (SERCOM1) SPI Interrupt Enable Set */
#define REG_SERCOM1_SPI_INTFLAG    (0x42000C18U) /**< \brief (SERCOM1) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM1_SPI_STATUS     (0x42000C1AU) /**< \brief (SERCOM1) SPI Status */
#define REG_SERCOM1_SPI_SYNCBUSY   (0x42000C1CU) /**< \brief (SERCOM1) SPI Synchronization Busy */
#define REG_SERCOM1_SPI_ADDR       (0x42000C24U) /**< \brief (SERCOM1) SPI Address */
#define REG_SERCOM1_SPI_DATA       (0x42000C28U) /**< \brief (SERCOM1) SPI Data */
#define REG_SERCOM1_SPI_DBGCTRL    (0x42000C30U) /**< \brief (SERCOM1) SPI Debug Control */
#define REG_SERCOM1_USART_CTRLA    (0x42000C00U) /**< \brief (SERCOM1) USART Control A */
#define REG_SERCOM1_USART_CTRLB    (0x42000C04U) /**< \brief (SERCOM1) USART Control B */
#define REG_SERCOM1_USART_BAUD     (0x42000C0CU) /**< \brief (SERCOM1) USART Baud Rate */
#define REG_SERCOM1_USART_RXPL     (0x42000C0EU) /**< \brief (SERCOM1) USART Receive Pulse Length */
#define REG_SERCOM1_USART_INTENCLR (0x42000C14U) /**< \brief (SERCOM1) USART Interrupt Enable Clear */
#define REG_SERCOM1_USART_INTENSET (0x42000C16U) /**< \brief (SERCOM1) USART Interrupt Enable Set */
#define REG_SERCOM1_USART_INTFLAG  (0x42000C18U) /**< \brief (SERCOM1) USART Interrupt Flag Status and Clear */
#define REG_SERCOM1_USART_STATUS   (0x42000C1AU) /**< \brief (SERCOM1) USART Status */
#define REG_SERCOM1_USART_SYNCBUSY (0x42000C1CU) /**< \brief (SERCOM1) USART Synchronization Busy */
#define REG_SERCOM1_USART_DATA     (0x42000C28U) /**< \brief (SERCOM1) USART Data */
#define REG_SERCOM1_USART_DBGCTRL  (0x42000C30U) /**< \brief (SERCOM1) USART Debug Control */
#else
#define REG_SERCOM1_I2CM_CTRLA     (*(RwReg  *)0x42000C00U) /**< \brief (SERCOM1) I2CM Control A */
#define REG_SERCOM1_I2CM_CTRLB     (*(RwReg  *)0x42000C04U) /**< \brief (SERCOM1) I2CM Control B */
#define REG_SERCOM1_I2CM_BAUD      (*(RwReg  *)0x42000C0CU) /**< \brief (SERCOM1) I2CM Baud Rate */
#define REG_SERCOM1_I2CM_INTENCLR  (*(RwReg8 *)0x42000C14U) /**< \brief (SERCOM1) I2CM Interrupt Enable Clear */
#define REG_SERCOM1_I2CM_INTENSET  (*(RwReg8 *)0x42000C16U) /**< \brief (SERCOM1) I2CM Interrupt Enable Set */
#define REG_SERCOM1_I2CM_INTFLAG   (*(RwReg8 *)0x42000C18U) /**< \brief (SERCOM1) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CM_STATUS    (*(RwReg16*)0x42000C1AU) /**< \brief (SERCOM1) I2CM Status */
#define REG_SERCOM1_I2CM_SYNCBUSY  (*(RoReg  *)0x42000C1CU) /**< \brief (SERCOM1) I2CM Synchronization Busy */
#define REG_SERCOM1_I2CM_ADDR      (*(RwReg  *)0x42000C24U) /**< \brief (SERCOM1) I2CM Address */
#define REG_SERCOM1_I2CM_DATA      (*(RwReg8 *)0x42000C28U) /**< \brief (SERCOM1) I2CM Data */
#define REG_SERCOM1_I2CM_DBGCTRL   (*(RwReg8 *)0x42000C30U) /**< \brief (SERCOM1) I2CM Debug Control */
#define REG_SERCOM1_I2CS_CTRLA     (*(RwReg  *)0x42000C00U) /**< \brief (SERCOM1) I2CS Control A */
#define REG_SERCOM1_I2CS_CTRLB     (*(RwReg  *)0x42000C04U) /**< \brief (SERCOM1) I2CS Control B */
#define REG_SERCOM1_I2CS_INTENCLR  (*(RwReg8 *)0x42000C14U) /**< \brief (SERCOM1) I2CS Interrupt Enable Clear */
#define REG_SERCOM1_I2CS_INTENSET  (*(RwReg8 *)0x42000C16U) /**< \brief (SERCOM1) I2CS Interrupt Enable Set */
#define REG_SERCOM1_I2CS_INTFLAG   (*(RwReg8 *)0x42000C18U) /**< \brief (SERCOM1) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CS_STATUS    (*(RwReg16*)0x42000C1AU) /**< \brief (SERCOM1) I2CS Status */
#define REG_SERCOM1_I2CS_SYNCBUSY  (*(RoReg  *)0x42000C1CU) /**< \brief (SERCOM1) I2CS Synchronization Busy */
#define REG_SERCOM1_I2CS_ADDR      (*(RwReg  *)0x42000C24U) /**< \brief (SERCOM1) I2CS Address */
#define REG_SERCOM1_I2CS_DATA      (*(RwReg8 *)0x42000C28U) /**< \brief (SERCOM1) I2CS Data */
#define REG_SERCOM1_SPI_CTRLA      (*(RwReg  *)0x42000C00U) /**< \brief (SERCOM1) SPI Control A */
#define REG_SERCOM1_SPI_CTRLB      (*(RwReg  *)0x42000C04U) /**< \brief (SERCOM1) SPI Control B */
#define REG_SERCOM1_SPI_BAUD       (*(RwReg8 *)0x42000C0CU) /**< \brief (SERCOM1) SPI Baud Rate */
#define REG_SERCOM1_SPI_INTENCLR   (*(RwReg8 *)0x42000C14U) /**< \brief (SERCOM1) SPI Interrupt Enable Clear */
#define REG_SERCOM1_SPI_INTENSET   (*(RwReg8 *)0x42000C16U) /**< \brief (SERCOM1) SPI Interrupt Enable Set */
#define REG_SERCOM1_SPI_INTFLAG    (*(RwReg8 *)0x42000C18U) /**< \brief (SERCOM1) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM1_SPI_STATUS     (*(RwReg16*)0x42000C1AU) /**< \brief (SERCOM1) SPI Status */
#define REG_SERCOM1_SPI_SYNCBUSY   (*(RoReg  *)0x42000C1CU) /**< \brief (SERCOM1) SPI Synchronization Busy */
#define REG_SERCOM1_SPI_ADDR       (*(RwReg  *)0x42000C24U) /**< \brief (SERCOM1) SPI Address */
#define REG_SERCOM1_SPI_DATA       (*(RwReg  *)0x42000C28U) /**< \brief (SERCOM1) SPI Data */
#define REG_SERCOM1_SPI_DBGCTRL    (*(RwReg8 *)0x42000C30U) /**< \brief (SERCOM1) SPI Debug Control */
#define REG_SERCOM1_USART_CTRLA    (*(RwReg  *)0x42000C00U) /**< \brief (SERCOM1) USART Control A */
#define REG_SERCOM1_USART_CTRLB    (*(RwReg  *)0x42000C04U) /**< \brief (SERCOM1) USART Control B */
#define REG_SERCOM1_USART_BAUD     (*(RwReg16*)0x42000C0CU) /**< \brief (SERCOM1) USART Baud Rate */
#define REG_SERCOM1_USART_RXPL     (*(RwReg8 *)0x42000C0EU) /**< \brief (SERCOM1) USART Receive Pulse Length */
#define REG_SERCOM1_USART_INTENCLR (*(RwReg8 *)0x42000C14U) /**< \brief (SERCOM1) USART Interrupt Enable Clear */
#define REG_SERCOM1_USART_INTENSET (*(RwReg8 *)0x42000C16U) /**< \brief (SERCOM1) USART Interrupt Enable Set */
#define REG_SERCOM1_USART_INTFLAG  (*(RwReg8 *)0x42000C18U) /**< \brief (SERCOM1) USART Interrupt Flag Status and Clear */
#define REG_SERCOM1_USART_STATUS   (*(RwReg16*)0x42000C1AU) /**< \brief (SERCOM1) USART Status */
#define REG_SERCOM1_USART_SYNCBUSY (*(RoReg  *)0x42000C1CU) /**< \brief (SERCOM1) USART Synchronization Busy */
#define REG_SERCOM1_USART_DATA     (*(RwReg16*)0x42000C28U) /**< \brief (SERCOM1) USART Data */
#define REG_SERCOM1_USART_DBGCTRL  (*(RwReg8 *)0x42000C30U) /**< \brief (SERCOM1) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM1 peripheral ========== */
#define SERCOM1_DMAC_ID_RX          3        // Index of DMA RX trigger
#define SERCOM1_DMAC_ID_TX          4        // Index of DMA TX trigger
#define SERCOM1_GCLK_ID_CORE        21       // Index of Generic Clock for Core
#define SERCOM1_GCLK_ID_SLOW        19       // Index of Generic Clock for SMbus timeout
#define SERCOM1_INT_MSB             6       

#endif /* _SAMD21_SERCOM1_INSTANCE_ */
