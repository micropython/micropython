/**
 * \file
 *
 * \brief Instance description for SERCOM6
 *
 * Copyright (c) 2017 Atmel Corporation,
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

#ifndef _SAMD51_SERCOM6_INSTANCE_
#define _SAMD51_SERCOM6_INSTANCE_

/* ========== Register definition for SERCOM6 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM6_I2CM_CTRLA     (0x43000800) /**< \brief (SERCOM6) I2CM Control A */
#define REG_SERCOM6_I2CM_CTRLB     (0x43000804) /**< \brief (SERCOM6) I2CM Control B */
#define REG_SERCOM6_I2CM_CTRLC     (0x43000808) /**< \brief (SERCOM6) I2CM Control C */
#define REG_SERCOM6_I2CM_BAUD      (0x4300080C) /**< \brief (SERCOM6) I2CM Baud Rate */
#define REG_SERCOM6_I2CM_INTENCLR  (0x43000814) /**< \brief (SERCOM6) I2CM Interrupt Enable Clear */
#define REG_SERCOM6_I2CM_INTENSET  (0x43000816) /**< \brief (SERCOM6) I2CM Interrupt Enable Set */
#define REG_SERCOM6_I2CM_INTFLAG   (0x43000818) /**< \brief (SERCOM6) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM6_I2CM_STATUS    (0x4300081A) /**< \brief (SERCOM6) I2CM Status */
#define REG_SERCOM6_I2CM_SYNCBUSY  (0x4300081C) /**< \brief (SERCOM6) I2CM Synchronization Busy */
#define REG_SERCOM6_I2CM_ADDR      (0x43000824) /**< \brief (SERCOM6) I2CM Address */
#define REG_SERCOM6_I2CM_DATA      (0x43000828) /**< \brief (SERCOM6) I2CM Data */
#define REG_SERCOM6_I2CM_DBGCTRL   (0x43000830) /**< \brief (SERCOM6) I2CM Debug Control */
#define REG_SERCOM6_I2CS_CTRLA     (0x43000800) /**< \brief (SERCOM6) I2CS Control A */
#define REG_SERCOM6_I2CS_CTRLB     (0x43000804) /**< \brief (SERCOM6) I2CS Control B */
#define REG_SERCOM6_I2CS_CTRLC     (0x43000808) /**< \brief (SERCOM6) I2CS Control C */
#define REG_SERCOM6_I2CS_INTENCLR  (0x43000814) /**< \brief (SERCOM6) I2CS Interrupt Enable Clear */
#define REG_SERCOM6_I2CS_INTENSET  (0x43000816) /**< \brief (SERCOM6) I2CS Interrupt Enable Set */
#define REG_SERCOM6_I2CS_INTFLAG   (0x43000818) /**< \brief (SERCOM6) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM6_I2CS_STATUS    (0x4300081A) /**< \brief (SERCOM6) I2CS Status */
#define REG_SERCOM6_I2CS_SYNCBUSY  (0x4300081C) /**< \brief (SERCOM6) I2CS Synchronization Busy */
#define REG_SERCOM6_I2CS_LENGTH    (0x43000822) /**< \brief (SERCOM6) I2CS Length */
#define REG_SERCOM6_I2CS_ADDR      (0x43000824) /**< \brief (SERCOM6) I2CS Address */
#define REG_SERCOM6_I2CS_DATA      (0x43000828) /**< \brief (SERCOM6) I2CS Data */
#define REG_SERCOM6_SPI_CTRLA      (0x43000800) /**< \brief (SERCOM6) SPI Control A */
#define REG_SERCOM6_SPI_CTRLB      (0x43000804) /**< \brief (SERCOM6) SPI Control B */
#define REG_SERCOM6_SPI_CTRLC      (0x43000808) /**< \brief (SERCOM6) SPI Control C */
#define REG_SERCOM6_SPI_BAUD       (0x4300080C) /**< \brief (SERCOM6) SPI Baud Rate */
#define REG_SERCOM6_SPI_INTENCLR   (0x43000814) /**< \brief (SERCOM6) SPI Interrupt Enable Clear */
#define REG_SERCOM6_SPI_INTENSET   (0x43000816) /**< \brief (SERCOM6) SPI Interrupt Enable Set */
#define REG_SERCOM6_SPI_INTFLAG    (0x43000818) /**< \brief (SERCOM6) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM6_SPI_STATUS     (0x4300081A) /**< \brief (SERCOM6) SPI Status */
#define REG_SERCOM6_SPI_SYNCBUSY   (0x4300081C) /**< \brief (SERCOM6) SPI Synchronization Busy */
#define REG_SERCOM6_SPI_LENGTH     (0x43000822) /**< \brief (SERCOM6) SPI Length */
#define REG_SERCOM6_SPI_ADDR       (0x43000824) /**< \brief (SERCOM6) SPI Address */
#define REG_SERCOM6_SPI_DATA       (0x43000828) /**< \brief (SERCOM6) SPI Data */
#define REG_SERCOM6_SPI_DBGCTRL    (0x43000830) /**< \brief (SERCOM6) SPI Debug Control */
#define REG_SERCOM6_USART_CTRLA    (0x43000800) /**< \brief (SERCOM6) USART Control A */
#define REG_SERCOM6_USART_CTRLB    (0x43000804) /**< \brief (SERCOM6) USART Control B */
#define REG_SERCOM6_USART_CTRLC    (0x43000808) /**< \brief (SERCOM6) USART Control C */
#define REG_SERCOM6_USART_BAUD     (0x4300080C) /**< \brief (SERCOM6) USART Baud Rate */
#define REG_SERCOM6_USART_RXPL     (0x4300080E) /**< \brief (SERCOM6) USART Receive Pulse Length */
#define REG_SERCOM6_USART_INTENCLR (0x43000814) /**< \brief (SERCOM6) USART Interrupt Enable Clear */
#define REG_SERCOM6_USART_INTENSET (0x43000816) /**< \brief (SERCOM6) USART Interrupt Enable Set */
#define REG_SERCOM6_USART_INTFLAG  (0x43000818) /**< \brief (SERCOM6) USART Interrupt Flag Status and Clear */
#define REG_SERCOM6_USART_STATUS   (0x4300081A) /**< \brief (SERCOM6) USART Status */
#define REG_SERCOM6_USART_SYNCBUSY (0x4300081C) /**< \brief (SERCOM6) USART Synchronization Busy */
#define REG_SERCOM6_USART_RXERRCNT (0x43000820) /**< \brief (SERCOM6) USART Receive Error Count */
#define REG_SERCOM6_USART_LENGTH   (0x43000822) /**< \brief (SERCOM6) USART Length */
#define REG_SERCOM6_USART_DATA     (0x43000828) /**< \brief (SERCOM6) USART Data */
#define REG_SERCOM6_USART_DBGCTRL  (0x43000830) /**< \brief (SERCOM6) USART Debug Control */
#else
#define REG_SERCOM6_I2CM_CTRLA     (*(RwReg  *)0x43000800UL) /**< \brief (SERCOM6) I2CM Control A */
#define REG_SERCOM6_I2CM_CTRLB     (*(RwReg  *)0x43000804UL) /**< \brief (SERCOM6) I2CM Control B */
#define REG_SERCOM6_I2CM_CTRLC     (*(RwReg  *)0x43000808UL) /**< \brief (SERCOM6) I2CM Control C */
#define REG_SERCOM6_I2CM_BAUD      (*(RwReg  *)0x4300080CUL) /**< \brief (SERCOM6) I2CM Baud Rate */
#define REG_SERCOM6_I2CM_INTENCLR  (*(RwReg8 *)0x43000814UL) /**< \brief (SERCOM6) I2CM Interrupt Enable Clear */
#define REG_SERCOM6_I2CM_INTENSET  (*(RwReg8 *)0x43000816UL) /**< \brief (SERCOM6) I2CM Interrupt Enable Set */
#define REG_SERCOM6_I2CM_INTFLAG   (*(RwReg8 *)0x43000818UL) /**< \brief (SERCOM6) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM6_I2CM_STATUS    (*(RwReg16*)0x4300081AUL) /**< \brief (SERCOM6) I2CM Status */
#define REG_SERCOM6_I2CM_SYNCBUSY  (*(RoReg  *)0x4300081CUL) /**< \brief (SERCOM6) I2CM Synchronization Busy */
#define REG_SERCOM6_I2CM_ADDR      (*(RwReg  *)0x43000824UL) /**< \brief (SERCOM6) I2CM Address */
#define REG_SERCOM6_I2CM_DATA      (*(RwReg  *)0x43000828UL) /**< \brief (SERCOM6) I2CM Data */
#define REG_SERCOM6_I2CM_DBGCTRL   (*(RwReg8 *)0x43000830UL) /**< \brief (SERCOM6) I2CM Debug Control */
#define REG_SERCOM6_I2CS_CTRLA     (*(RwReg  *)0x43000800UL) /**< \brief (SERCOM6) I2CS Control A */
#define REG_SERCOM6_I2CS_CTRLB     (*(RwReg  *)0x43000804UL) /**< \brief (SERCOM6) I2CS Control B */
#define REG_SERCOM6_I2CS_CTRLC     (*(RwReg  *)0x43000808UL) /**< \brief (SERCOM6) I2CS Control C */
#define REG_SERCOM6_I2CS_INTENCLR  (*(RwReg8 *)0x43000814UL) /**< \brief (SERCOM6) I2CS Interrupt Enable Clear */
#define REG_SERCOM6_I2CS_INTENSET  (*(RwReg8 *)0x43000816UL) /**< \brief (SERCOM6) I2CS Interrupt Enable Set */
#define REG_SERCOM6_I2CS_INTFLAG   (*(RwReg8 *)0x43000818UL) /**< \brief (SERCOM6) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM6_I2CS_STATUS    (*(RwReg16*)0x4300081AUL) /**< \brief (SERCOM6) I2CS Status */
#define REG_SERCOM6_I2CS_SYNCBUSY  (*(RoReg  *)0x4300081CUL) /**< \brief (SERCOM6) I2CS Synchronization Busy */
#define REG_SERCOM6_I2CS_LENGTH    (*(RwReg16*)0x43000822UL) /**< \brief (SERCOM6) I2CS Length */
#define REG_SERCOM6_I2CS_ADDR      (*(RwReg  *)0x43000824UL) /**< \brief (SERCOM6) I2CS Address */
#define REG_SERCOM6_I2CS_DATA      (*(RwReg  *)0x43000828UL) /**< \brief (SERCOM6) I2CS Data */
#define REG_SERCOM6_SPI_CTRLA      (*(RwReg  *)0x43000800UL) /**< \brief (SERCOM6) SPI Control A */
#define REG_SERCOM6_SPI_CTRLB      (*(RwReg  *)0x43000804UL) /**< \brief (SERCOM6) SPI Control B */
#define REG_SERCOM6_SPI_CTRLC      (*(RwReg  *)0x43000808UL) /**< \brief (SERCOM6) SPI Control C */
#define REG_SERCOM6_SPI_BAUD       (*(RwReg8 *)0x4300080CUL) /**< \brief (SERCOM6) SPI Baud Rate */
#define REG_SERCOM6_SPI_INTENCLR   (*(RwReg8 *)0x43000814UL) /**< \brief (SERCOM6) SPI Interrupt Enable Clear */
#define REG_SERCOM6_SPI_INTENSET   (*(RwReg8 *)0x43000816UL) /**< \brief (SERCOM6) SPI Interrupt Enable Set */
#define REG_SERCOM6_SPI_INTFLAG    (*(RwReg8 *)0x43000818UL) /**< \brief (SERCOM6) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM6_SPI_STATUS     (*(RwReg16*)0x4300081AUL) /**< \brief (SERCOM6) SPI Status */
#define REG_SERCOM6_SPI_SYNCBUSY   (*(RoReg  *)0x4300081CUL) /**< \brief (SERCOM6) SPI Synchronization Busy */
#define REG_SERCOM6_SPI_LENGTH     (*(RwReg16*)0x43000822UL) /**< \brief (SERCOM6) SPI Length */
#define REG_SERCOM6_SPI_ADDR       (*(RwReg  *)0x43000824UL) /**< \brief (SERCOM6) SPI Address */
#define REG_SERCOM6_SPI_DATA       (*(RwReg  *)0x43000828UL) /**< \brief (SERCOM6) SPI Data */
#define REG_SERCOM6_SPI_DBGCTRL    (*(RwReg8 *)0x43000830UL) /**< \brief (SERCOM6) SPI Debug Control */
#define REG_SERCOM6_USART_CTRLA    (*(RwReg  *)0x43000800UL) /**< \brief (SERCOM6) USART Control A */
#define REG_SERCOM6_USART_CTRLB    (*(RwReg  *)0x43000804UL) /**< \brief (SERCOM6) USART Control B */
#define REG_SERCOM6_USART_CTRLC    (*(RwReg  *)0x43000808UL) /**< \brief (SERCOM6) USART Control C */
#define REG_SERCOM6_USART_BAUD     (*(RwReg16*)0x4300080CUL) /**< \brief (SERCOM6) USART Baud Rate */
#define REG_SERCOM6_USART_RXPL     (*(RwReg8 *)0x4300080EUL) /**< \brief (SERCOM6) USART Receive Pulse Length */
#define REG_SERCOM6_USART_INTENCLR (*(RwReg8 *)0x43000814UL) /**< \brief (SERCOM6) USART Interrupt Enable Clear */
#define REG_SERCOM6_USART_INTENSET (*(RwReg8 *)0x43000816UL) /**< \brief (SERCOM6) USART Interrupt Enable Set */
#define REG_SERCOM6_USART_INTFLAG  (*(RwReg8 *)0x43000818UL) /**< \brief (SERCOM6) USART Interrupt Flag Status and Clear */
#define REG_SERCOM6_USART_STATUS   (*(RwReg16*)0x4300081AUL) /**< \brief (SERCOM6) USART Status */
#define REG_SERCOM6_USART_SYNCBUSY (*(RoReg  *)0x4300081CUL) /**< \brief (SERCOM6) USART Synchronization Busy */
#define REG_SERCOM6_USART_RXERRCNT (*(RoReg8 *)0x43000820UL) /**< \brief (SERCOM6) USART Receive Error Count */
#define REG_SERCOM6_USART_LENGTH   (*(RwReg16*)0x43000822UL) /**< \brief (SERCOM6) USART Length */
#define REG_SERCOM6_USART_DATA     (*(RwReg  *)0x43000828UL) /**< \brief (SERCOM6) USART Data */
#define REG_SERCOM6_USART_DBGCTRL  (*(RwReg8 *)0x43000830UL) /**< \brief (SERCOM6) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM6 peripheral ========== */
#define SERCOM6_CLK_REDUCTION       1        // Reduce clock options to pin 1 for SPI and USART
#define SERCOM6_DLY_COMPENSATION    1        // Compensates for a fast DLY50 element. Assuming 20ns
#define SERCOM6_DMA                 1        // DMA support implemented?
#define SERCOM6_DMAC_ID_RX          16       // Index of DMA RX trigger
#define SERCOM6_DMAC_ID_TX          17       // Index of DMA TX trigger
#define SERCOM6_FIFO_DEPTH_POWER    1        // 2^FIFO_DEPTH_POWER gives rx FIFO depth.
#define SERCOM6_GCLK_ID_CORE        36      
#define SERCOM6_GCLK_ID_SLOW        3       
#define SERCOM6_INT_MSB             6       
#define SERCOM6_PMSB                3       
#define SERCOM6_RETENTION_SUPPORT   0        // Retention supported?
#define SERCOM6_SE_CNT              1        // SE counter included?
#define SERCOM6_SPI                 1        // SPI mode implemented?
#define SERCOM6_SPI_HW_SS_CTRL      1        // Master _SS hardware control implemented?
#define SERCOM6_SPI_ICSPACE_EXT     1        // SPI inter character space implemented?
#define SERCOM6_SPI_OZMO            0        // OZMO features implemented?
#define SERCOM6_SPI_WAKE_ON_SSL     1        // _SS low detect implemented?
#define SERCOM6_TTBIT_EXTENSION     1        // 32-bit extension implemented?
#define SERCOM6_TWIM                1        // TWI Master mode implemented?
#define SERCOM6_TWIS                1        // TWI Slave mode implemented?
#define SERCOM6_TWIS_AUTO_ACK       1        // TWI slave automatic acknowledge implemented?
#define SERCOM6_TWIS_GROUP_CMD      1        // TWI slave group command implemented?
#define SERCOM6_TWIS_SDASETUP_CNT_SIZE 8        // TWIS sda setup count size
#define SERCOM6_TWIS_SDASETUP_SIZE  4        // TWIS sda setup size
#define SERCOM6_TWIS_SUDAT          1        // TWI slave SDA setup implemented?
#define SERCOM6_TWI_FASTMP          1        // TWI fast mode plus implemented?
#define SERCOM6_TWI_HSMODE          1        // USART mode implemented?
#define SERCOM6_TWI_SCLSM_MODE      1        // TWI SCL clock stretch mode implemented?
#define SERCOM6_TWI_SMB_TIMEOUTS    1        // TWI SMBus timeouts implemented?
#define SERCOM6_TWI_TENBIT_ADR      1        // TWI ten bit enabled?
#define SERCOM6_USART               1        // USART mode implemented?
#define SERCOM6_USART_AUTOBAUD      1        // USART autobaud implemented?
#define SERCOM6_USART_COLDET        1        // USART collision detection implemented?
#define SERCOM6_USART_FLOW_CTRL     1        // USART flow control implemented?
#define SERCOM6_USART_FRAC_BAUD     1        // USART fractional BAUD implemented?
#define SERCOM6_USART_IRDA          1        // USART IrDA implemented?
#define SERCOM6_USART_ISO7816       1        // USART ISO7816 mode implemented?
#define SERCOM6_USART_LIN_MASTER    1        // USART LIN Master mode implemented?
#define SERCOM6_USART_RS485         1        // USART RS485 mode implemented?
#define SERCOM6_USART_SAMPA_EXT     1        // USART sample adjust implemented?
#define SERCOM6_USART_SAMPR_EXT     1        // USART oversampling adjustment implemented?

#endif /* _SAMD51_SERCOM6_INSTANCE_ */
