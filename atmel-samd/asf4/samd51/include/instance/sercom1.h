/**
 * \file
 *
 * \brief Instance description for SERCOM1
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

#ifndef _SAMD51_SERCOM1_INSTANCE_
#define _SAMD51_SERCOM1_INSTANCE_

/* ========== Register definition for SERCOM1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM1_I2CM_CTRLA     (0x40003400) /**< \brief (SERCOM1) I2CM Control A */
#define REG_SERCOM1_I2CM_CTRLB     (0x40003404) /**< \brief (SERCOM1) I2CM Control B */
#define REG_SERCOM1_I2CM_CTRLC     (0x40003408) /**< \brief (SERCOM1) I2CM Control C */
#define REG_SERCOM1_I2CM_BAUD      (0x4000340C) /**< \brief (SERCOM1) I2CM Baud Rate */
#define REG_SERCOM1_I2CM_INTENCLR  (0x40003414) /**< \brief (SERCOM1) I2CM Interrupt Enable Clear */
#define REG_SERCOM1_I2CM_INTENSET  (0x40003416) /**< \brief (SERCOM1) I2CM Interrupt Enable Set */
#define REG_SERCOM1_I2CM_INTFLAG   (0x40003418) /**< \brief (SERCOM1) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CM_STATUS    (0x4000341A) /**< \brief (SERCOM1) I2CM Status */
#define REG_SERCOM1_I2CM_SYNCBUSY  (0x4000341C) /**< \brief (SERCOM1) I2CM Synchronization Busy */
#define REG_SERCOM1_I2CM_ADDR      (0x40003424) /**< \brief (SERCOM1) I2CM Address */
#define REG_SERCOM1_I2CM_DATA      (0x40003428) /**< \brief (SERCOM1) I2CM Data */
#define REG_SERCOM1_I2CM_DBGCTRL   (0x40003430) /**< \brief (SERCOM1) I2CM Debug Control */
#define REG_SERCOM1_I2CS_CTRLA     (0x40003400) /**< \brief (SERCOM1) I2CS Control A */
#define REG_SERCOM1_I2CS_CTRLB     (0x40003404) /**< \brief (SERCOM1) I2CS Control B */
#define REG_SERCOM1_I2CS_CTRLC     (0x40003408) /**< \brief (SERCOM1) I2CS Control C */
#define REG_SERCOM1_I2CS_INTENCLR  (0x40003414) /**< \brief (SERCOM1) I2CS Interrupt Enable Clear */
#define REG_SERCOM1_I2CS_INTENSET  (0x40003416) /**< \brief (SERCOM1) I2CS Interrupt Enable Set */
#define REG_SERCOM1_I2CS_INTFLAG   (0x40003418) /**< \brief (SERCOM1) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CS_STATUS    (0x4000341A) /**< \brief (SERCOM1) I2CS Status */
#define REG_SERCOM1_I2CS_SYNCBUSY  (0x4000341C) /**< \brief (SERCOM1) I2CS Synchronization Busy */
#define REG_SERCOM1_I2CS_LENGTH    (0x40003422) /**< \brief (SERCOM1) I2CS Length */
#define REG_SERCOM1_I2CS_ADDR      (0x40003424) /**< \brief (SERCOM1) I2CS Address */
#define REG_SERCOM1_I2CS_DATA      (0x40003428) /**< \brief (SERCOM1) I2CS Data */
#define REG_SERCOM1_SPI_CTRLA      (0x40003400) /**< \brief (SERCOM1) SPI Control A */
#define REG_SERCOM1_SPI_CTRLB      (0x40003404) /**< \brief (SERCOM1) SPI Control B */
#define REG_SERCOM1_SPI_CTRLC      (0x40003408) /**< \brief (SERCOM1) SPI Control C */
#define REG_SERCOM1_SPI_BAUD       (0x4000340C) /**< \brief (SERCOM1) SPI Baud Rate */
#define REG_SERCOM1_SPI_INTENCLR   (0x40003414) /**< \brief (SERCOM1) SPI Interrupt Enable Clear */
#define REG_SERCOM1_SPI_INTENSET   (0x40003416) /**< \brief (SERCOM1) SPI Interrupt Enable Set */
#define REG_SERCOM1_SPI_INTFLAG    (0x40003418) /**< \brief (SERCOM1) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM1_SPI_STATUS     (0x4000341A) /**< \brief (SERCOM1) SPI Status */
#define REG_SERCOM1_SPI_SYNCBUSY   (0x4000341C) /**< \brief (SERCOM1) SPI Synchronization Busy */
#define REG_SERCOM1_SPI_LENGTH     (0x40003422) /**< \brief (SERCOM1) SPI Length */
#define REG_SERCOM1_SPI_ADDR       (0x40003424) /**< \brief (SERCOM1) SPI Address */
#define REG_SERCOM1_SPI_DATA       (0x40003428) /**< \brief (SERCOM1) SPI Data */
#define REG_SERCOM1_SPI_DBGCTRL    (0x40003430) /**< \brief (SERCOM1) SPI Debug Control */
#define REG_SERCOM1_USART_CTRLA    (0x40003400) /**< \brief (SERCOM1) USART Control A */
#define REG_SERCOM1_USART_CTRLB    (0x40003404) /**< \brief (SERCOM1) USART Control B */
#define REG_SERCOM1_USART_CTRLC    (0x40003408) /**< \brief (SERCOM1) USART Control C */
#define REG_SERCOM1_USART_BAUD     (0x4000340C) /**< \brief (SERCOM1) USART Baud Rate */
#define REG_SERCOM1_USART_RXPL     (0x4000340E) /**< \brief (SERCOM1) USART Receive Pulse Length */
#define REG_SERCOM1_USART_INTENCLR (0x40003414) /**< \brief (SERCOM1) USART Interrupt Enable Clear */
#define REG_SERCOM1_USART_INTENSET (0x40003416) /**< \brief (SERCOM1) USART Interrupt Enable Set */
#define REG_SERCOM1_USART_INTFLAG  (0x40003418) /**< \brief (SERCOM1) USART Interrupt Flag Status and Clear */
#define REG_SERCOM1_USART_STATUS   (0x4000341A) /**< \brief (SERCOM1) USART Status */
#define REG_SERCOM1_USART_SYNCBUSY (0x4000341C) /**< \brief (SERCOM1) USART Synchronization Busy */
#define REG_SERCOM1_USART_RXERRCNT (0x40003420) /**< \brief (SERCOM1) USART Receive Error Count */
#define REG_SERCOM1_USART_LENGTH   (0x40003422) /**< \brief (SERCOM1) USART Length */
#define REG_SERCOM1_USART_DATA     (0x40003428) /**< \brief (SERCOM1) USART Data */
#define REG_SERCOM1_USART_DBGCTRL  (0x40003430) /**< \brief (SERCOM1) USART Debug Control */
#else
#define REG_SERCOM1_I2CM_CTRLA     (*(RwReg  *)0x40003400UL) /**< \brief (SERCOM1) I2CM Control A */
#define REG_SERCOM1_I2CM_CTRLB     (*(RwReg  *)0x40003404UL) /**< \brief (SERCOM1) I2CM Control B */
#define REG_SERCOM1_I2CM_CTRLC     (*(RwReg  *)0x40003408UL) /**< \brief (SERCOM1) I2CM Control C */
#define REG_SERCOM1_I2CM_BAUD      (*(RwReg  *)0x4000340CUL) /**< \brief (SERCOM1) I2CM Baud Rate */
#define REG_SERCOM1_I2CM_INTENCLR  (*(RwReg8 *)0x40003414UL) /**< \brief (SERCOM1) I2CM Interrupt Enable Clear */
#define REG_SERCOM1_I2CM_INTENSET  (*(RwReg8 *)0x40003416UL) /**< \brief (SERCOM1) I2CM Interrupt Enable Set */
#define REG_SERCOM1_I2CM_INTFLAG   (*(RwReg8 *)0x40003418UL) /**< \brief (SERCOM1) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CM_STATUS    (*(RwReg16*)0x4000341AUL) /**< \brief (SERCOM1) I2CM Status */
#define REG_SERCOM1_I2CM_SYNCBUSY  (*(RoReg  *)0x4000341CUL) /**< \brief (SERCOM1) I2CM Synchronization Busy */
#define REG_SERCOM1_I2CM_ADDR      (*(RwReg  *)0x40003424UL) /**< \brief (SERCOM1) I2CM Address */
#define REG_SERCOM1_I2CM_DATA      (*(RwReg  *)0x40003428UL) /**< \brief (SERCOM1) I2CM Data */
#define REG_SERCOM1_I2CM_DBGCTRL   (*(RwReg8 *)0x40003430UL) /**< \brief (SERCOM1) I2CM Debug Control */
#define REG_SERCOM1_I2CS_CTRLA     (*(RwReg  *)0x40003400UL) /**< \brief (SERCOM1) I2CS Control A */
#define REG_SERCOM1_I2CS_CTRLB     (*(RwReg  *)0x40003404UL) /**< \brief (SERCOM1) I2CS Control B */
#define REG_SERCOM1_I2CS_CTRLC     (*(RwReg  *)0x40003408UL) /**< \brief (SERCOM1) I2CS Control C */
#define REG_SERCOM1_I2CS_INTENCLR  (*(RwReg8 *)0x40003414UL) /**< \brief (SERCOM1) I2CS Interrupt Enable Clear */
#define REG_SERCOM1_I2CS_INTENSET  (*(RwReg8 *)0x40003416UL) /**< \brief (SERCOM1) I2CS Interrupt Enable Set */
#define REG_SERCOM1_I2CS_INTFLAG   (*(RwReg8 *)0x40003418UL) /**< \brief (SERCOM1) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM1_I2CS_STATUS    (*(RwReg16*)0x4000341AUL) /**< \brief (SERCOM1) I2CS Status */
#define REG_SERCOM1_I2CS_SYNCBUSY  (*(RoReg  *)0x4000341CUL) /**< \brief (SERCOM1) I2CS Synchronization Busy */
#define REG_SERCOM1_I2CS_LENGTH    (*(RwReg16*)0x40003422UL) /**< \brief (SERCOM1) I2CS Length */
#define REG_SERCOM1_I2CS_ADDR      (*(RwReg  *)0x40003424UL) /**< \brief (SERCOM1) I2CS Address */
#define REG_SERCOM1_I2CS_DATA      (*(RwReg  *)0x40003428UL) /**< \brief (SERCOM1) I2CS Data */
#define REG_SERCOM1_SPI_CTRLA      (*(RwReg  *)0x40003400UL) /**< \brief (SERCOM1) SPI Control A */
#define REG_SERCOM1_SPI_CTRLB      (*(RwReg  *)0x40003404UL) /**< \brief (SERCOM1) SPI Control B */
#define REG_SERCOM1_SPI_CTRLC      (*(RwReg  *)0x40003408UL) /**< \brief (SERCOM1) SPI Control C */
#define REG_SERCOM1_SPI_BAUD       (*(RwReg8 *)0x4000340CUL) /**< \brief (SERCOM1) SPI Baud Rate */
#define REG_SERCOM1_SPI_INTENCLR   (*(RwReg8 *)0x40003414UL) /**< \brief (SERCOM1) SPI Interrupt Enable Clear */
#define REG_SERCOM1_SPI_INTENSET   (*(RwReg8 *)0x40003416UL) /**< \brief (SERCOM1) SPI Interrupt Enable Set */
#define REG_SERCOM1_SPI_INTFLAG    (*(RwReg8 *)0x40003418UL) /**< \brief (SERCOM1) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM1_SPI_STATUS     (*(RwReg16*)0x4000341AUL) /**< \brief (SERCOM1) SPI Status */
#define REG_SERCOM1_SPI_SYNCBUSY   (*(RoReg  *)0x4000341CUL) /**< \brief (SERCOM1) SPI Synchronization Busy */
#define REG_SERCOM1_SPI_LENGTH     (*(RwReg16*)0x40003422UL) /**< \brief (SERCOM1) SPI Length */
#define REG_SERCOM1_SPI_ADDR       (*(RwReg  *)0x40003424UL) /**< \brief (SERCOM1) SPI Address */
#define REG_SERCOM1_SPI_DATA       (*(RwReg  *)0x40003428UL) /**< \brief (SERCOM1) SPI Data */
#define REG_SERCOM1_SPI_DBGCTRL    (*(RwReg8 *)0x40003430UL) /**< \brief (SERCOM1) SPI Debug Control */
#define REG_SERCOM1_USART_CTRLA    (*(RwReg  *)0x40003400UL) /**< \brief (SERCOM1) USART Control A */
#define REG_SERCOM1_USART_CTRLB    (*(RwReg  *)0x40003404UL) /**< \brief (SERCOM1) USART Control B */
#define REG_SERCOM1_USART_CTRLC    (*(RwReg  *)0x40003408UL) /**< \brief (SERCOM1) USART Control C */
#define REG_SERCOM1_USART_BAUD     (*(RwReg16*)0x4000340CUL) /**< \brief (SERCOM1) USART Baud Rate */
#define REG_SERCOM1_USART_RXPL     (*(RwReg8 *)0x4000340EUL) /**< \brief (SERCOM1) USART Receive Pulse Length */
#define REG_SERCOM1_USART_INTENCLR (*(RwReg8 *)0x40003414UL) /**< \brief (SERCOM1) USART Interrupt Enable Clear */
#define REG_SERCOM1_USART_INTENSET (*(RwReg8 *)0x40003416UL) /**< \brief (SERCOM1) USART Interrupt Enable Set */
#define REG_SERCOM1_USART_INTFLAG  (*(RwReg8 *)0x40003418UL) /**< \brief (SERCOM1) USART Interrupt Flag Status and Clear */
#define REG_SERCOM1_USART_STATUS   (*(RwReg16*)0x4000341AUL) /**< \brief (SERCOM1) USART Status */
#define REG_SERCOM1_USART_SYNCBUSY (*(RoReg  *)0x4000341CUL) /**< \brief (SERCOM1) USART Synchronization Busy */
#define REG_SERCOM1_USART_RXERRCNT (*(RoReg8 *)0x40003420UL) /**< \brief (SERCOM1) USART Receive Error Count */
#define REG_SERCOM1_USART_LENGTH   (*(RwReg16*)0x40003422UL) /**< \brief (SERCOM1) USART Length */
#define REG_SERCOM1_USART_DATA     (*(RwReg  *)0x40003428UL) /**< \brief (SERCOM1) USART Data */
#define REG_SERCOM1_USART_DBGCTRL  (*(RwReg8 *)0x40003430UL) /**< \brief (SERCOM1) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM1 peripheral ========== */
#define SERCOM1_CLK_REDUCTION       1        // Reduce clock options to pin 1 for SPI and USART
#define SERCOM1_DLY_COMPENSATION    1        // Compensates for a fast DLY50 element. Assuming 20ns
#define SERCOM1_DMA                 1        // DMA support implemented?
#define SERCOM1_DMAC_ID_RX          6        // Index of DMA RX trigger
#define SERCOM1_DMAC_ID_TX          7        // Index of DMA TX trigger
#define SERCOM1_FIFO_DEPTH_POWER    1        // 2^FIFO_DEPTH_POWER gives rx FIFO depth.
#define SERCOM1_GCLK_ID_CORE        8       
#define SERCOM1_GCLK_ID_SLOW        3       
#define SERCOM1_INT_MSB             6       
#define SERCOM1_PMSB                3       
#define SERCOM1_RETENTION_SUPPORT   0        // Retention supported?
#define SERCOM1_SE_CNT              1        // SE counter included?
#define SERCOM1_SPI                 1        // SPI mode implemented?
#define SERCOM1_SPI_HW_SS_CTRL      1        // Master _SS hardware control implemented?
#define SERCOM1_SPI_ICSPACE_EXT     1        // SPI inter character space implemented?
#define SERCOM1_SPI_OZMO            0        // OZMO features implemented?
#define SERCOM1_SPI_WAKE_ON_SSL     1        // _SS low detect implemented?
#define SERCOM1_TTBIT_EXTENSION     1        // 32-bit extension implemented?
#define SERCOM1_TWIM                1        // TWI Master mode implemented?
#define SERCOM1_TWIS                1        // TWI Slave mode implemented?
#define SERCOM1_TWIS_AUTO_ACK       1        // TWI slave automatic acknowledge implemented?
#define SERCOM1_TWIS_GROUP_CMD      1        // TWI slave group command implemented?
#define SERCOM1_TWIS_SDASETUP_CNT_SIZE 8        // TWIS sda setup count size
#define SERCOM1_TWIS_SDASETUP_SIZE  4        // TWIS sda setup size
#define SERCOM1_TWIS_SUDAT          1        // TWI slave SDA setup implemented?
#define SERCOM1_TWI_FASTMP          1        // TWI fast mode plus implemented?
#define SERCOM1_TWI_HSMODE          1        // USART mode implemented?
#define SERCOM1_TWI_SCLSM_MODE      1        // TWI SCL clock stretch mode implemented?
#define SERCOM1_TWI_SMB_TIMEOUTS    1        // TWI SMBus timeouts implemented?
#define SERCOM1_TWI_TENBIT_ADR      1        // TWI ten bit enabled?
#define SERCOM1_USART               1        // USART mode implemented?
#define SERCOM1_USART_AUTOBAUD      1        // USART autobaud implemented?
#define SERCOM1_USART_COLDET        1        // USART collision detection implemented?
#define SERCOM1_USART_FLOW_CTRL     1        // USART flow control implemented?
#define SERCOM1_USART_FRAC_BAUD     1        // USART fractional BAUD implemented?
#define SERCOM1_USART_IRDA          1        // USART IrDA implemented?
#define SERCOM1_USART_ISO7816       1        // USART ISO7816 mode implemented?
#define SERCOM1_USART_LIN_MASTER    1        // USART LIN Master mode implemented?
#define SERCOM1_USART_RS485         1        // USART RS485 mode implemented?
#define SERCOM1_USART_SAMPA_EXT     1        // USART sample adjust implemented?
#define SERCOM1_USART_SAMPR_EXT     1        // USART oversampling adjustment implemented?

#endif /* _SAMD51_SERCOM1_INSTANCE_ */
