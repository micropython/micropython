/**
 * \file
 *
 * \brief Instance description for SERCOM7
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

#ifndef _SAMD51_SERCOM7_INSTANCE_
#define _SAMD51_SERCOM7_INSTANCE_

/* ========== Register definition for SERCOM7 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM7_I2CM_CTRLA     (0x43000C00) /**< \brief (SERCOM7) I2CM Control A */
#define REG_SERCOM7_I2CM_CTRLB     (0x43000C04) /**< \brief (SERCOM7) I2CM Control B */
#define REG_SERCOM7_I2CM_CTRLC     (0x43000C08) /**< \brief (SERCOM7) I2CM Control C */
#define REG_SERCOM7_I2CM_BAUD      (0x43000C0C) /**< \brief (SERCOM7) I2CM Baud Rate */
#define REG_SERCOM7_I2CM_INTENCLR  (0x43000C14) /**< \brief (SERCOM7) I2CM Interrupt Enable Clear */
#define REG_SERCOM7_I2CM_INTENSET  (0x43000C16) /**< \brief (SERCOM7) I2CM Interrupt Enable Set */
#define REG_SERCOM7_I2CM_INTFLAG   (0x43000C18) /**< \brief (SERCOM7) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM7_I2CM_STATUS    (0x43000C1A) /**< \brief (SERCOM7) I2CM Status */
#define REG_SERCOM7_I2CM_SYNCBUSY  (0x43000C1C) /**< \brief (SERCOM7) I2CM Synchronization Busy */
#define REG_SERCOM7_I2CM_ADDR      (0x43000C24) /**< \brief (SERCOM7) I2CM Address */
#define REG_SERCOM7_I2CM_DATA      (0x43000C28) /**< \brief (SERCOM7) I2CM Data */
#define REG_SERCOM7_I2CM_DBGCTRL   (0x43000C30) /**< \brief (SERCOM7) I2CM Debug Control */
#define REG_SERCOM7_I2CS_CTRLA     (0x43000C00) /**< \brief (SERCOM7) I2CS Control A */
#define REG_SERCOM7_I2CS_CTRLB     (0x43000C04) /**< \brief (SERCOM7) I2CS Control B */
#define REG_SERCOM7_I2CS_CTRLC     (0x43000C08) /**< \brief (SERCOM7) I2CS Control C */
#define REG_SERCOM7_I2CS_INTENCLR  (0x43000C14) /**< \brief (SERCOM7) I2CS Interrupt Enable Clear */
#define REG_SERCOM7_I2CS_INTENSET  (0x43000C16) /**< \brief (SERCOM7) I2CS Interrupt Enable Set */
#define REG_SERCOM7_I2CS_INTFLAG   (0x43000C18) /**< \brief (SERCOM7) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM7_I2CS_STATUS    (0x43000C1A) /**< \brief (SERCOM7) I2CS Status */
#define REG_SERCOM7_I2CS_SYNCBUSY  (0x43000C1C) /**< \brief (SERCOM7) I2CS Synchronization Busy */
#define REG_SERCOM7_I2CS_LENGTH    (0x43000C22) /**< \brief (SERCOM7) I2CS Length */
#define REG_SERCOM7_I2CS_ADDR      (0x43000C24) /**< \brief (SERCOM7) I2CS Address */
#define REG_SERCOM7_I2CS_DATA      (0x43000C28) /**< \brief (SERCOM7) I2CS Data */
#define REG_SERCOM7_SPI_CTRLA      (0x43000C00) /**< \brief (SERCOM7) SPI Control A */
#define REG_SERCOM7_SPI_CTRLB      (0x43000C04) /**< \brief (SERCOM7) SPI Control B */
#define REG_SERCOM7_SPI_CTRLC      (0x43000C08) /**< \brief (SERCOM7) SPI Control C */
#define REG_SERCOM7_SPI_BAUD       (0x43000C0C) /**< \brief (SERCOM7) SPI Baud Rate */
#define REG_SERCOM7_SPI_INTENCLR   (0x43000C14) /**< \brief (SERCOM7) SPI Interrupt Enable Clear */
#define REG_SERCOM7_SPI_INTENSET   (0x43000C16) /**< \brief (SERCOM7) SPI Interrupt Enable Set */
#define REG_SERCOM7_SPI_INTFLAG    (0x43000C18) /**< \brief (SERCOM7) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM7_SPI_STATUS     (0x43000C1A) /**< \brief (SERCOM7) SPI Status */
#define REG_SERCOM7_SPI_SYNCBUSY   (0x43000C1C) /**< \brief (SERCOM7) SPI Synchronization Busy */
#define REG_SERCOM7_SPI_LENGTH     (0x43000C22) /**< \brief (SERCOM7) SPI Length */
#define REG_SERCOM7_SPI_ADDR       (0x43000C24) /**< \brief (SERCOM7) SPI Address */
#define REG_SERCOM7_SPI_DATA       (0x43000C28) /**< \brief (SERCOM7) SPI Data */
#define REG_SERCOM7_SPI_DBGCTRL    (0x43000C30) /**< \brief (SERCOM7) SPI Debug Control */
#define REG_SERCOM7_USART_CTRLA    (0x43000C00) /**< \brief (SERCOM7) USART Control A */
#define REG_SERCOM7_USART_CTRLB    (0x43000C04) /**< \brief (SERCOM7) USART Control B */
#define REG_SERCOM7_USART_CTRLC    (0x43000C08) /**< \brief (SERCOM7) USART Control C */
#define REG_SERCOM7_USART_BAUD     (0x43000C0C) /**< \brief (SERCOM7) USART Baud Rate */
#define REG_SERCOM7_USART_RXPL     (0x43000C0E) /**< \brief (SERCOM7) USART Receive Pulse Length */
#define REG_SERCOM7_USART_INTENCLR (0x43000C14) /**< \brief (SERCOM7) USART Interrupt Enable Clear */
#define REG_SERCOM7_USART_INTENSET (0x43000C16) /**< \brief (SERCOM7) USART Interrupt Enable Set */
#define REG_SERCOM7_USART_INTFLAG  (0x43000C18) /**< \brief (SERCOM7) USART Interrupt Flag Status and Clear */
#define REG_SERCOM7_USART_STATUS   (0x43000C1A) /**< \brief (SERCOM7) USART Status */
#define REG_SERCOM7_USART_SYNCBUSY (0x43000C1C) /**< \brief (SERCOM7) USART Synchronization Busy */
#define REG_SERCOM7_USART_RXERRCNT (0x43000C20) /**< \brief (SERCOM7) USART Receive Error Count */
#define REG_SERCOM7_USART_LENGTH   (0x43000C22) /**< \brief (SERCOM7) USART Length */
#define REG_SERCOM7_USART_DATA     (0x43000C28) /**< \brief (SERCOM7) USART Data */
#define REG_SERCOM7_USART_DBGCTRL  (0x43000C30) /**< \brief (SERCOM7) USART Debug Control */
#else
#define REG_SERCOM7_I2CM_CTRLA     (*(RwReg  *)0x43000C00UL) /**< \brief (SERCOM7) I2CM Control A */
#define REG_SERCOM7_I2CM_CTRLB     (*(RwReg  *)0x43000C04UL) /**< \brief (SERCOM7) I2CM Control B */
#define REG_SERCOM7_I2CM_CTRLC     (*(RwReg  *)0x43000C08UL) /**< \brief (SERCOM7) I2CM Control C */
#define REG_SERCOM7_I2CM_BAUD      (*(RwReg  *)0x43000C0CUL) /**< \brief (SERCOM7) I2CM Baud Rate */
#define REG_SERCOM7_I2CM_INTENCLR  (*(RwReg8 *)0x43000C14UL) /**< \brief (SERCOM7) I2CM Interrupt Enable Clear */
#define REG_SERCOM7_I2CM_INTENSET  (*(RwReg8 *)0x43000C16UL) /**< \brief (SERCOM7) I2CM Interrupt Enable Set */
#define REG_SERCOM7_I2CM_INTFLAG   (*(RwReg8 *)0x43000C18UL) /**< \brief (SERCOM7) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM7_I2CM_STATUS    (*(RwReg16*)0x43000C1AUL) /**< \brief (SERCOM7) I2CM Status */
#define REG_SERCOM7_I2CM_SYNCBUSY  (*(RoReg  *)0x43000C1CUL) /**< \brief (SERCOM7) I2CM Synchronization Busy */
#define REG_SERCOM7_I2CM_ADDR      (*(RwReg  *)0x43000C24UL) /**< \brief (SERCOM7) I2CM Address */
#define REG_SERCOM7_I2CM_DATA      (*(RwReg  *)0x43000C28UL) /**< \brief (SERCOM7) I2CM Data */
#define REG_SERCOM7_I2CM_DBGCTRL   (*(RwReg8 *)0x43000C30UL) /**< \brief (SERCOM7) I2CM Debug Control */
#define REG_SERCOM7_I2CS_CTRLA     (*(RwReg  *)0x43000C00UL) /**< \brief (SERCOM7) I2CS Control A */
#define REG_SERCOM7_I2CS_CTRLB     (*(RwReg  *)0x43000C04UL) /**< \brief (SERCOM7) I2CS Control B */
#define REG_SERCOM7_I2CS_CTRLC     (*(RwReg  *)0x43000C08UL) /**< \brief (SERCOM7) I2CS Control C */
#define REG_SERCOM7_I2CS_INTENCLR  (*(RwReg8 *)0x43000C14UL) /**< \brief (SERCOM7) I2CS Interrupt Enable Clear */
#define REG_SERCOM7_I2CS_INTENSET  (*(RwReg8 *)0x43000C16UL) /**< \brief (SERCOM7) I2CS Interrupt Enable Set */
#define REG_SERCOM7_I2CS_INTFLAG   (*(RwReg8 *)0x43000C18UL) /**< \brief (SERCOM7) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM7_I2CS_STATUS    (*(RwReg16*)0x43000C1AUL) /**< \brief (SERCOM7) I2CS Status */
#define REG_SERCOM7_I2CS_SYNCBUSY  (*(RoReg  *)0x43000C1CUL) /**< \brief (SERCOM7) I2CS Synchronization Busy */
#define REG_SERCOM7_I2CS_LENGTH    (*(RwReg16*)0x43000C22UL) /**< \brief (SERCOM7) I2CS Length */
#define REG_SERCOM7_I2CS_ADDR      (*(RwReg  *)0x43000C24UL) /**< \brief (SERCOM7) I2CS Address */
#define REG_SERCOM7_I2CS_DATA      (*(RwReg  *)0x43000C28UL) /**< \brief (SERCOM7) I2CS Data */
#define REG_SERCOM7_SPI_CTRLA      (*(RwReg  *)0x43000C00UL) /**< \brief (SERCOM7) SPI Control A */
#define REG_SERCOM7_SPI_CTRLB      (*(RwReg  *)0x43000C04UL) /**< \brief (SERCOM7) SPI Control B */
#define REG_SERCOM7_SPI_CTRLC      (*(RwReg  *)0x43000C08UL) /**< \brief (SERCOM7) SPI Control C */
#define REG_SERCOM7_SPI_BAUD       (*(RwReg8 *)0x43000C0CUL) /**< \brief (SERCOM7) SPI Baud Rate */
#define REG_SERCOM7_SPI_INTENCLR   (*(RwReg8 *)0x43000C14UL) /**< \brief (SERCOM7) SPI Interrupt Enable Clear */
#define REG_SERCOM7_SPI_INTENSET   (*(RwReg8 *)0x43000C16UL) /**< \brief (SERCOM7) SPI Interrupt Enable Set */
#define REG_SERCOM7_SPI_INTFLAG    (*(RwReg8 *)0x43000C18UL) /**< \brief (SERCOM7) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM7_SPI_STATUS     (*(RwReg16*)0x43000C1AUL) /**< \brief (SERCOM7) SPI Status */
#define REG_SERCOM7_SPI_SYNCBUSY   (*(RoReg  *)0x43000C1CUL) /**< \brief (SERCOM7) SPI Synchronization Busy */
#define REG_SERCOM7_SPI_LENGTH     (*(RwReg16*)0x43000C22UL) /**< \brief (SERCOM7) SPI Length */
#define REG_SERCOM7_SPI_ADDR       (*(RwReg  *)0x43000C24UL) /**< \brief (SERCOM7) SPI Address */
#define REG_SERCOM7_SPI_DATA       (*(RwReg  *)0x43000C28UL) /**< \brief (SERCOM7) SPI Data */
#define REG_SERCOM7_SPI_DBGCTRL    (*(RwReg8 *)0x43000C30UL) /**< \brief (SERCOM7) SPI Debug Control */
#define REG_SERCOM7_USART_CTRLA    (*(RwReg  *)0x43000C00UL) /**< \brief (SERCOM7) USART Control A */
#define REG_SERCOM7_USART_CTRLB    (*(RwReg  *)0x43000C04UL) /**< \brief (SERCOM7) USART Control B */
#define REG_SERCOM7_USART_CTRLC    (*(RwReg  *)0x43000C08UL) /**< \brief (SERCOM7) USART Control C */
#define REG_SERCOM7_USART_BAUD     (*(RwReg16*)0x43000C0CUL) /**< \brief (SERCOM7) USART Baud Rate */
#define REG_SERCOM7_USART_RXPL     (*(RwReg8 *)0x43000C0EUL) /**< \brief (SERCOM7) USART Receive Pulse Length */
#define REG_SERCOM7_USART_INTENCLR (*(RwReg8 *)0x43000C14UL) /**< \brief (SERCOM7) USART Interrupt Enable Clear */
#define REG_SERCOM7_USART_INTENSET (*(RwReg8 *)0x43000C16UL) /**< \brief (SERCOM7) USART Interrupt Enable Set */
#define REG_SERCOM7_USART_INTFLAG  (*(RwReg8 *)0x43000C18UL) /**< \brief (SERCOM7) USART Interrupt Flag Status and Clear */
#define REG_SERCOM7_USART_STATUS   (*(RwReg16*)0x43000C1AUL) /**< \brief (SERCOM7) USART Status */
#define REG_SERCOM7_USART_SYNCBUSY (*(RoReg  *)0x43000C1CUL) /**< \brief (SERCOM7) USART Synchronization Busy */
#define REG_SERCOM7_USART_RXERRCNT (*(RoReg8 *)0x43000C20UL) /**< \brief (SERCOM7) USART Receive Error Count */
#define REG_SERCOM7_USART_LENGTH   (*(RwReg16*)0x43000C22UL) /**< \brief (SERCOM7) USART Length */
#define REG_SERCOM7_USART_DATA     (*(RwReg  *)0x43000C28UL) /**< \brief (SERCOM7) USART Data */
#define REG_SERCOM7_USART_DBGCTRL  (*(RwReg8 *)0x43000C30UL) /**< \brief (SERCOM7) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM7 peripheral ========== */
#define SERCOM7_CLK_REDUCTION       1        // Reduce clock options to pin 1 for SPI and USART
#define SERCOM7_DLY_COMPENSATION    1        // Compensates for a fast DLY50 element. Assuming 20ns
#define SERCOM7_DMA                 1        // DMA support implemented?
#define SERCOM7_DMAC_ID_RX          18       // Index of DMA RX trigger
#define SERCOM7_DMAC_ID_TX          19       // Index of DMA TX trigger
#define SERCOM7_FIFO_DEPTH_POWER    1        // 2^FIFO_DEPTH_POWER gives rx FIFO depth.
#define SERCOM7_GCLK_ID_CORE        37      
#define SERCOM7_GCLK_ID_SLOW        3       
#define SERCOM7_INT_MSB             6       
#define SERCOM7_PMSB                3       
#define SERCOM7_RETENTION_SUPPORT   0        // Retention supported?
#define SERCOM7_SE_CNT              1        // SE counter included?
#define SERCOM7_SPI                 1        // SPI mode implemented?
#define SERCOM7_SPI_HW_SS_CTRL      1        // Master _SS hardware control implemented?
#define SERCOM7_SPI_ICSPACE_EXT     1        // SPI inter character space implemented?
#define SERCOM7_SPI_OZMO            0        // OZMO features implemented?
#define SERCOM7_SPI_WAKE_ON_SSL     1        // _SS low detect implemented?
#define SERCOM7_TTBIT_EXTENSION     1        // 32-bit extension implemented?
#define SERCOM7_TWIM                1        // TWI Master mode implemented?
#define SERCOM7_TWIS                1        // TWI Slave mode implemented?
#define SERCOM7_TWIS_AUTO_ACK       1        // TWI slave automatic acknowledge implemented?
#define SERCOM7_TWIS_GROUP_CMD      1        // TWI slave group command implemented?
#define SERCOM7_TWIS_SDASETUP_CNT_SIZE 8        // TWIS sda setup count size
#define SERCOM7_TWIS_SDASETUP_SIZE  4        // TWIS sda setup size
#define SERCOM7_TWIS_SUDAT          1        // TWI slave SDA setup implemented?
#define SERCOM7_TWI_FASTMP          1        // TWI fast mode plus implemented?
#define SERCOM7_TWI_HSMODE          1        // USART mode implemented?
#define SERCOM7_TWI_SCLSM_MODE      1        // TWI SCL clock stretch mode implemented?
#define SERCOM7_TWI_SMB_TIMEOUTS    1        // TWI SMBus timeouts implemented?
#define SERCOM7_TWI_TENBIT_ADR      1        // TWI ten bit enabled?
#define SERCOM7_USART               1        // USART mode implemented?
#define SERCOM7_USART_AUTOBAUD      1        // USART autobaud implemented?
#define SERCOM7_USART_COLDET        1        // USART collision detection implemented?
#define SERCOM7_USART_FLOW_CTRL     1        // USART flow control implemented?
#define SERCOM7_USART_FRAC_BAUD     1        // USART fractional BAUD implemented?
#define SERCOM7_USART_IRDA          1        // USART IrDA implemented?
#define SERCOM7_USART_ISO7816       1        // USART ISO7816 mode implemented?
#define SERCOM7_USART_LIN_MASTER    1        // USART LIN Master mode implemented?
#define SERCOM7_USART_RS485         1        // USART RS485 mode implemented?
#define SERCOM7_USART_SAMPA_EXT     1        // USART sample adjust implemented?
#define SERCOM7_USART_SAMPR_EXT     1        // USART oversampling adjustment implemented?

#endif /* _SAMD51_SERCOM7_INSTANCE_ */
