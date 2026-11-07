/*!
    \file    gd32f4xx_i2c.h
    \brief   definitions for the I2C

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef GD32F4XX_I2C_H
#define GD32F4XX_I2C_H

#include "gd32f4xx.h"

/* I2Cx(x=0,1,2) definitions */
#define I2C0                          I2C_BASE                          /*!< I2C0 base address */
#define I2C1                          (I2C_BASE + 0x00000400U)          /*!< I2C1 base address */
#define I2C2                          (I2C_BASE + 0x00000800U)          /*!< I2C2 base address */

/* registers definitions */
#define I2C_CTL0(i2cx)                REG32((i2cx) + 0x00000000U)       /*!< I2C control register 0 */
#define I2C_CTL1(i2cx)                REG32((i2cx) + 0x00000004U)       /*!< I2C control register 1 */
#define I2C_SADDR0(i2cx)              REG32((i2cx) + 0x00000008U)       /*!< I2C slave address register 0 */
#define I2C_SADDR1(i2cx)              REG32((i2cx) + 0x0000000CU)       /*!< I2C slave address register 1 */
#define I2C_DATA(i2cx)                REG32((i2cx) + 0x00000010U)       /*!< I2C transfer buffer register */
#define I2C_STAT0(i2cx)               REG32((i2cx) + 0x00000014U)       /*!< I2C transfer status register 0 */
#define I2C_STAT1(i2cx)               REG32((i2cx) + 0x00000018U)       /*!< I2C transfer status register */
#define I2C_CKCFG(i2cx)               REG32((i2cx) + 0x0000001CU)       /*!< I2C clock configure register */
#define I2C_RT(i2cx)                  REG32((i2cx) + 0x00000020U)       /*!< I2C rise time register */
#define I2C_FCTL(i2cx)                REG32((i2cx) + 0x00000024U)       /*!< I2C filter control register */
#define I2C_SAMCS(i2cx)               REG32((i2cx) + 0x00000080U)       /*!< I2C SAM control and status register */

/* bits definitions */
/* I2Cx_CTL0 */
#define I2C_CTL0_I2CEN                BIT(0)                            /*!< peripheral enable */
#define I2C_CTL0_SMBEN                BIT(1)                            /*!< SMBus mode */
#define I2C_CTL0_SMBSEL               BIT(3)                            /*!< SMBus type */
#define I2C_CTL0_ARPEN                BIT(4)                            /*!< ARP enable */
#define I2C_CTL0_PECEN                BIT(5)                            /*!< PEC enable */
#define I2C_CTL0_GCEN                 BIT(6)                            /*!< general call enable */
#define I2C_CTL0_SS                   BIT(7)                            /*!< clock stretching disable (slave mode) */
#define I2C_CTL0_START                BIT(8)                            /*!< start generation */
#define I2C_CTL0_STOP                 BIT(9)                            /*!< stop generation */
#define I2C_CTL0_ACKEN                BIT(10)                           /*!< acknowledge enable */
#define I2C_CTL0_POAP                 BIT(11)                           /*!< acknowledge/PEC position (for data reception) */
#define I2C_CTL0_PECTRANS             BIT(12)                           /*!< packet error checking */
#define I2C_CTL0_SALT                 BIT(13)                           /*!< SMBus alert */
#define I2C_CTL0_SRESET               BIT(15)                           /*!< software reset */

/* I2Cx_CTL1 */
#define I2C_CTL1_I2CCLK               BITS(0,5)                         /*!< I2CCLK[5:0] bits (peripheral clock frequency) */
#define I2C_CTL1_ERRIE                BIT(8)                            /*!< error interrupt enable */
#define I2C_CTL1_EVIE                 BIT(9)                            /*!< event interrupt enable */
#define I2C_CTL1_BUFIE                BIT(10)                           /*!< buffer interrupt enable */
#define I2C_CTL1_DMAON                BIT(11)                           /*!< DMA requests enable */
#define I2C_CTL1_DMALST               BIT(12)                           /*!< DMA last transfer */

/* I2Cx_SADDR0 */
#define I2C_SADDR0_ADDRESS0           BIT(0)                            /*!< bit 0 of a 10-bit address */
#define I2C_SADDR0_ADDRESS            BITS(1,7)                         /*!< 7-bit address or bits 7:1 of a 10-bit address */
#define I2C_SADDR0_ADDRESS_H          BITS(8,9)                         /*!< highest two bits of a 10-bit address */
#define I2C_SADDR0_ADDFORMAT          BIT(15)                           /*!< address mode for the I2C slave */

/* I2Cx_SADDR1 */
#define I2C_SADDR1_DUADEN             BIT(0)                            /*!< aual-address mode switch */
#define I2C_SADDR1_ADDRESS2           BITS(1,7)                         /*!< second I2C address for the slave in dual-address mode */

/* I2Cx_DATA */
#define I2C_DATA_TRB                  BITS(0,7)                         /*!< 8-bit data register */

/* I2Cx_STAT0 */
#define I2C_STAT0_SBSEND              BIT(0)                            /*!< start bit (master mode) */
#define I2C_STAT0_ADDSEND             BIT(1)                            /*!< address sent (master mode)/matched (slave mode) */
#define I2C_STAT0_BTC                 BIT(2)                            /*!< byte transfer finished */
#define I2C_STAT0_ADD10SEND           BIT(3)                            /*!< 10-bit header sent (master mode) */
#define I2C_STAT0_STPDET              BIT(4)                            /*!< stop detection (slave mode) */
#define I2C_STAT0_RBNE                BIT(6)                            /*!< data register not empty (receivers) */
#define I2C_STAT0_TBE                 BIT(7)                            /*!< data register empty (transmitters) */
#define I2C_STAT0_BERR                BIT(8)                            /*!< bus error */
#define I2C_STAT0_LOSTARB             BIT(9)                            /*!< arbitration lost (master mode) */
#define I2C_STAT0_AERR                BIT(10)                           /*!< acknowledge failure */
#define I2C_STAT0_OUERR               BIT(11)                           /*!< overrun/underrun */
#define I2C_STAT0_PECERR              BIT(12)                           /*!< PEC error in reception */
#define I2C_STAT0_SMBTO               BIT(14)                           /*!< timeout signal in SMBus mode */
#define I2C_STAT0_SMBALT              BIT(15)                           /*!< SMBus alert status */

/* I2Cx_STAT1 */
#define I2C_STAT1_MASTER              BIT(0)                            /*!< master/slave */
#define I2C_STAT1_I2CBSY              BIT(1)                            /*!< bus busy */
#define I2C_STAT1_TR                  BIT(2)                            /*!< transmitter/receiver */
#define I2C_STAT1_RXGC                BIT(4)                            /*!< general call address (slave mode) */
#define I2C_STAT1_DEFSMB              BIT(5)                            /*!< SMBus device default address (slave mode) */
#define I2C_STAT1_HSTSMB              BIT(6)                            /*!< SMBus host header (slave mode) */
#define I2C_STAT1_DUMODF              BIT(7)                            /*!< dual flag (slave mode) */
#define I2C_STAT1_PECV                BITS(8,15)                        /*!< packet error checking value */

/* I2Cx_CKCFG */
#define I2C_CKCFG_CLKC                BITS(0,11)                        /*!< clock control register in fast/standard mode (master mode) */
#define I2C_CKCFG_DTCY                BIT(14)                           /*!< fast mode duty cycle */
#define I2C_CKCFG_FAST                BIT(15)                           /*!< I2C speed selection in master mode */

/* I2Cx_RT */
#define I2C_RT_RISETIME               BITS(0,5)                         /*!< maximum rise time in fast/standard mode (Master mode) */

/* I2Cx_FCTL */
#define I2C_FCTL_DF                   BITS(0,3)                         /*!< digital noise filter */
#define I2C_FCTL_AFD                  BIT(4)                            /*!< analog noise filter disable */

/* I2Cx_SAMCS */
#define I2C_SAMCS_SAMEN               BIT(0)                            /*!< SAM_V interface enable */
#define I2C_SAMCS_STOEN               BIT(1)                            /*!< SAM_V interface timeout detect enable */
#define I2C_SAMCS_TFFIE               BIT(4)                            /*!< txframe fall interrupt enable */
#define I2C_SAMCS_TFRIE               BIT(5)                            /*!< txframe rise interrupt enable */
#define I2C_SAMCS_RFFIE               BIT(6)                            /*!< rxframe fall interrupt enable */
#define I2C_SAMCS_RFRIE               BIT(7)                            /*!< rxframe rise interrupt enable */
#define I2C_SAMCS_TXF                 BIT(8)                            /*!< level of txframe signal */
#define I2C_SAMCS_RXF                 BIT(9)                            /*!< level of rxframe signal */
#define I2C_SAMCS_TFF                 BIT(12)                           /*!< txframe fall flag */
#define I2C_SAMCS_TFR                 BIT(13)                           /*!< txframe rise flag */
#define I2C_SAMCS_RFF                 BIT(14)                           /*!< rxframe fall flag */
#define I2C_SAMCS_RFR                 BIT(15)                           /*!< rxframe rise flag */

/* constants definitions */
/* define the I2C bit position and its register index offset */
#define I2C_REGIDX_BIT(regidx, bitpos)  (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define I2C_REG_VAL(i2cx, offset)       (REG32((i2cx) + (((uint32_t)(offset) & 0x0000FFFFU) >> 6)))
#define I2C_BIT_POS(val)                ((uint32_t)(val) & 0x0000001FU)
#define I2C_REGIDX_BIT2(regidx, bitpos, regidx2, bitpos2)   (((uint32_t)(regidx2) << 22) | (uint32_t)((bitpos2) << 16)\
                                                            | (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos)))
#define I2C_REG_VAL2(i2cx, offset)      (REG32((i2cx) + ((uint32_t)(offset) >> 22)))
#define I2C_BIT_POS2(val)               (((uint32_t)(val) & 0x001F0000U) >> 16)

/* register offset */
#define I2C_CTL1_REG_OFFSET           ((uint32_t)0x00000004U)           /*!< CTL1 register offset */
#define I2C_STAT0_REG_OFFSET          ((uint32_t)0x00000014U)           /*!< STAT0 register offset */
#define I2C_STAT1_REG_OFFSET          ((uint32_t)0x00000018U)           /*!< STAT1 register offset */
#define I2C_SAMCS_REG_OFFSET          ((uint32_t)0x00000080U)           /*!< SAMCS register offset */

/* I2C flags */
typedef enum {
    /* flags in STAT0 register */
    I2C_FLAG_SBSEND = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 0U),         /*!< start condition sent out in master mode */
    I2C_FLAG_ADDSEND = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 1U),        /*!< address is sent in master mode or received and matches in slave mode */
    I2C_FLAG_BTC = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 2U),            /*!< byte transmission finishes */
    I2C_FLAG_ADD10SEND = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 3U),      /*!< header of 10-bit address is sent in master mode */
    I2C_FLAG_STPDET = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 4U),         /*!< stop condition detected in slave mode */
    I2C_FLAG_RBNE = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 6U),           /*!< I2C_DATA is not empty during receiving */
    I2C_FLAG_TBE = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 7U),            /*!< I2C_DATA is empty during transmitting */
    I2C_FLAG_BERR = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 8U),           /*!< a bus error occurs indication a unexpected start or stop condition on I2C bus */
    I2C_FLAG_LOSTARB = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 9U),        /*!< arbitration lost in master mode */
    I2C_FLAG_AERR = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 10U),          /*!< acknowledge error */
    I2C_FLAG_OUERR = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 11U),         /*!< over-run or under-run situation occurs in slave mode */
    I2C_FLAG_PECERR = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 12U),        /*!< PEC error when receiving data */
    I2C_FLAG_SMBTO = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 14U),         /*!< timeout signal in SMBus mode */
    I2C_FLAG_SMBALT = I2C_REGIDX_BIT(I2C_STAT0_REG_OFFSET, 15U),        /*!< SMBus alert status */
    /* flags in STAT1 register */
    I2C_FLAG_MASTER = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 0U),         /*!< a flag indicating whether I2C block is in master or slave mode */
    I2C_FLAG_I2CBSY = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 1U),         /*!< busy flag */
    I2C_FLAG_TR = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 2U),             /*!< whether the I2C is a transmitter or a receiver */
    I2C_FLAG_RXGC = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 4U),           /*!< general call address (00h) received */
    I2C_FLAG_DEFSMB = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 5U),         /*!< default address of SMBus device */
    I2C_FLAG_HSTSMB = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 6U),         /*!< SMBus host header detected in slave mode */
    I2C_FLAG_DUMOD = I2C_REGIDX_BIT(I2C_STAT1_REG_OFFSET, 7U),          /*!< dual flag in slave mode indicating which address is matched in dual-address mode */
    /* flags in SAMCS register */
    I2C_FLAG_TFF = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 12U),           /*!< txframe fall flag */
    I2C_FLAG_TFR = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 13U),           /*!< txframe rise flag */
    I2C_FLAG_RFF = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 14U),           /*!< rxframe fall flag */
    I2C_FLAG_RFR = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 15U)            /*!< rxframe rise flag */
} i2c_flag_enum;

/* I2C interrupt flags */
typedef enum {
    /* interrupt flags in CTL1 register */
    I2C_INT_FLAG_SBSEND = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 0U),        /*!< start condition sent out in master mode interrupt flag */
    I2C_INT_FLAG_ADDSEND = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 1U),       /*!< address is sent in master mode or received and matches in slave mode interrupt flag */
    I2C_INT_FLAG_BTC =  I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 2U),          /*!< byte transmission finishes interrupt flag */
    I2C_INT_FLAG_ADD10SEND =  I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 3U),    /*!< header of 10-bit address is sent in master mode interrupt flag */
    I2C_INT_FLAG_STPDET = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 4U),        /*!< stop condition detected in slave mode interrupt flag */
    I2C_INT_FLAG_RBNE = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 6U),          /*!< I2C_DATA is not Empty during receiving interrupt flag */
    I2C_INT_FLAG_TBE = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 9U, I2C_STAT0_REG_OFFSET, 7U),           /*!< I2C_DATA is empty during transmitting interrupt flag */
    I2C_INT_FLAG_BERR = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 8U),          /*!< a bus error occurs indication a unexpected start or stop condition on I2C bus interrupt flag */
    I2C_INT_FLAG_LOSTARB = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 9U),       /*!< arbitration lost in master mode interrupt flag */
    I2C_INT_FLAG_AERR = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 10U),         /*!< acknowledge error interrupt flag */
    I2C_INT_FLAG_OUERR = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 11U),        /*!< over-run or under-run situation occurs in slave mode interrupt flag */
    I2C_INT_FLAG_PECERR = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 12U),       /*!< PEC error when receiving data interrupt flag */
    I2C_INT_FLAG_SMBTO = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 14U),        /*!< timeout signal in SMBus mode interrupt flag */
    I2C_INT_FLAG_SMBALT = I2C_REGIDX_BIT2(I2C_CTL1_REG_OFFSET, 8U, I2C_STAT0_REG_OFFSET, 15U),       /*!< SMBus alert status interrupt flag */
    /* interrupt flags in SAMCS register */
    I2C_INT_FLAG_TFF = I2C_REGIDX_BIT2(I2C_SAMCS_REG_OFFSET, 4U, I2C_SAMCS_REG_OFFSET, 12U),         /*!< txframe fall interrupt flag */
    I2C_INT_FLAG_TFR = I2C_REGIDX_BIT2(I2C_SAMCS_REG_OFFSET, 5U, I2C_SAMCS_REG_OFFSET, 13U),         /*!< txframe rise interrupt flag */
    I2C_INT_FLAG_RFF = I2C_REGIDX_BIT2(I2C_SAMCS_REG_OFFSET, 6U, I2C_SAMCS_REG_OFFSET, 14U),         /*!< rxframe fall interrupt flag */
    I2C_INT_FLAG_RFR = I2C_REGIDX_BIT2(I2C_SAMCS_REG_OFFSET, 7U, I2C_SAMCS_REG_OFFSET, 15U)          /*!< rxframe rise interrupt flag */
} i2c_interrupt_flag_enum;

/* I2C interrupt */
typedef enum {
    /* interrupt in CTL1 register */
    I2C_INT_ERR = I2C_REGIDX_BIT(I2C_CTL1_REG_OFFSET, 8U),              /*!< error interrupt */
    I2C_INT_EV = I2C_REGIDX_BIT(I2C_CTL1_REG_OFFSET, 9U),               /*!< event interrupt */
    I2C_INT_BUF = I2C_REGIDX_BIT(I2C_CTL1_REG_OFFSET, 10U),             /*!< buffer interrupt */
    /* interrupt in SAMCS register */
    I2C_INT_TFF = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 4U),             /*!< txframe fall interrupt */
    I2C_INT_TFR = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 5U),             /*!< txframe rise interrupt */
    I2C_INT_RFF = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 6U),             /*!< rxframe fall interrupt */
    I2C_INT_RFR = I2C_REGIDX_BIT(I2C_SAMCS_REG_OFFSET, 7U)              /*!< rxframe rise interrupt */
} i2c_interrupt_enum;

/* the digital noise filter can filter spikes's length */
typedef enum {
    I2C_DF_DISABLE = 0,                                                 /*!< disable digital noise filter */
    I2C_DF_1PCLK,                                                       /*!< enable digital noise filter and the maximum filtered spiker's length 1 PCLK1 */
    I2C_DF_2PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 2 PCLK1 */
    I2C_DF_3PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 3 PCLK1 */
    I2C_DF_4PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 4 PCLK1 */
    I2C_DF_5PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 5 PCLK1 */
    I2C_DF_6PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 6 PCLK1 */
    I2C_DF_7PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 7 PCLK1 */
    I2C_DF_8PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 8 PCLK1 */
    I2C_DF_9PCLKS,                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 9 PCLK1 */
    I2C_DF_10PCLKS,                                                     /*!< enable digital noise filter and the maximum filtered spiker's length 10 PCLK1 */
    I2C_DF_11PCLKS,                                                     /*!< enable digital noise filter and the maximum filtered spiker's length 11 PCLK1 */
    I2C_DF_12PCLKS,                                                     /*!< enable digital noise filter and the maximum filtered spiker's length 12 PCLK1 */
    I2C_DF_13PCLKS,                                                     /*!< enable digital noise filter and the maximum filtered spiker's length 13 PCLK1 */
    I2C_DF_14PCLKS,                                                     /*!< enable digital noise filter and the maximum filtered spiker's length 14 PCLK1 */
    I2C_DF_15PCLKS                                                      /*!< enable digital noise filter and the maximum filtered spiker's length 15 PCLK1 */
} i2c_digital_filter_enum;

/* SMBus/I2C mode switch and SMBus type selection */
#define I2C_I2CMODE_ENABLE            ((uint32_t)0x00000000U)           /*!< I2C mode */
#define I2C_SMBUSMODE_ENABLE          I2C_CTL0_SMBEN                    /*!< SMBus mode */

/* SMBus/I2C mode switch and SMBus type selection */
#define I2C_SMBUS_DEVICE              ((uint32_t)0x00000000U)           /*!< SMBus mode device type */
#define I2C_SMBUS_HOST                I2C_CTL0_SMBSEL                   /*!< SMBus mode host type */

/* I2C transfer direction */
#define I2C_RECEIVER                  ((uint32_t)0x00000001U)           /*!< receiver */
#define I2C_TRANSMITTER               ((uint32_t)0xFFFFFFFEU)           /*!< transmitter */

/* whether or not to send an ACK */
#define I2C_ACK_DISABLE               ((uint32_t)0x00000000U)           /*!< ACK will be not sent */
#define I2C_ACK_ENABLE                I2C_CTL0_ACKEN                    /*!< ACK will be sent */

/* I2C POAP position*/
#define I2C_ACKPOS_CURRENT            ((uint32_t)0x00000000U)           /*!< ACKEN bit decides whether or not to send ACK or not for the current byte */
#define I2C_ACKPOS_NEXT               I2C_CTL0_POAP                     /*!< ACKEN bit decides whether or not to send ACK for the next byte */

/* whether or not to stretch SCL low */
#define I2C_SCLSTRETCH_ENABLE         ((uint32_t)0x00000000U)           /*!< enable SCL stretching */
#define I2C_SCLSTRETCH_DISABLE        I2C_CTL0_SS                       /*!< disable SCL stretching */

/* whether or not to response to a general call */
#define I2C_GCEN_ENABLE               I2C_CTL0_GCEN                     /*!< slave will response to a general call */
#define I2C_GCEN_DISABLE              ((uint32_t)0x00000000U)           /*!< slave will not response to a general call */

/* software reset I2C */
#define I2C_SRESET_RESET              ((uint32_t)0x00000000U)           /*!< I2C is not under reset */
#define I2C_SRESET_SET                I2C_CTL0_SRESET                   /*!< I2C is under reset */

/* I2C DMA mode configure */
/* DMA mode switch */
#define I2C_DMA_OFF                   ((uint32_t)0x00000000U)           /*!< disable DMA mode */
#define I2C_DMA_ON                    I2C_CTL1_DMAON                    /*!< enable DMA mode */

/* flag indicating DMA last transfer */
#define I2C_DMALST_OFF                ((uint32_t)0x00000000U)           /*!< next DMA EOT is not the last transfer */
#define I2C_DMALST_ON                 I2C_CTL1_DMALST                   /*!< next DMA EOT is the last transfer */

/* I2C PEC configure */
/* PEC enable */
#define I2C_PEC_DISABLE              ((uint32_t)0x00000000U)            /*!< PEC calculation off */
#define I2C_PEC_ENABLE                I2C_CTL0_PECEN                    /*!< PEC calculation on */

/* PEC transfer */
#define I2C_PECTRANS_DISABLE          ((uint32_t)0x00000000U)           /*!< not transfer PEC value */
#define I2C_PECTRANS_ENABLE           I2C_CTL0_PECTRANS                 /*!< transfer PEC value */

/* I2C SMBus configure */
/* issue or not alert through SMBA pin */
#define I2C_SALTSEND_DISABLE          ((uint32_t)0x00000000U)           /*!< not issue alert through SMBA */
#define I2C_SALTSEND_ENABLE           I2C_CTL0_SALT                     /*!< issue alert through SMBA pin */

/* ARP protocol in SMBus switch */
#define I2C_ARP_DISABLE               ((uint32_t)0x00000000U)           /*!< disable ARP */
#define I2C_ARP_ENABLE                I2C_CTL0_ARPEN                    /*!< enable ARP */

/* transmit I2C data */
#define DATA_TRANS(regval)            (BITS(0,7) & ((uint32_t)(regval) << 0))

/* receive I2C data */
#define DATA_RECV(regval)             GET_BITS((uint32_t)(regval), 0, 7)

/* I2C duty cycle in fast mode */
#define I2C_DTCY_2                    ((uint32_t)0x00000000U)           /*!< T_low/T_high = 2 in fast mode */
#define I2C_DTCY_16_9                 I2C_CKCFG_DTCY                    /*!< T_low/T_high = 16/9 in fast mode */

/* address mode for the I2C slave */
#define I2C_ADDFORMAT_7BITS           ((uint32_t)0x00000000U)           /*!< address format is 7 bits */
#define I2C_ADDFORMAT_10BITS          I2C_SADDR0_ADDFORMAT              /*!< address format is 10 bits */

/* function declarations */
/* initialization functions */
/* reset I2C */
void i2c_deinit(uint32_t i2c_periph);
/* configure I2C clock */
void i2c_clock_config(uint32_t i2c_periph, uint32_t clkspeed, uint32_t dutycyc);
/* configure I2C address */
void i2c_mode_addr_config(uint32_t i2c_periph, uint32_t mode, uint32_t addformat, uint32_t addr);

/* application function declarations */
/* select SMBus type */
void i2c_smbus_type_config(uint32_t i2c_periph, uint32_t type);
/* whether or not to send an ACK */
void i2c_ack_config(uint32_t i2c_periph, uint32_t ack);
/* configure I2C POAP position */
void i2c_ackpos_config(uint32_t i2c_periph, uint32_t pos);
/* master sends slave address */
void i2c_master_addressing(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection);
/* enable dual-address mode */
void i2c_dualaddr_enable(uint32_t i2c_periph, uint32_t addr);
/* disable dual-address mode */
void i2c_dualaddr_disable(uint32_t i2c_periph);
/* enable I2C */
void i2c_enable(uint32_t i2c_periph);
/* disable I2C */
void i2c_disable(uint32_t i2c_periph);
/* generate a START condition on I2C bus */
void i2c_start_on_bus(uint32_t i2c_periph);
/* generate a STOP condition on I2C bus */
void i2c_stop_on_bus(uint32_t i2c_periph);
/* I2C transmit data function */
void i2c_data_transmit(uint32_t i2c_periph, uint8_t data);
/* I2C receive data function */
uint8_t i2c_data_receive(uint32_t i2c_periph);
/* configure I2C DMA mode */
void i2c_dma_config(uint32_t i2c_periph, uint32_t dmastate);
/* configure whether next DMA EOT is DMA last transfer or not */
void i2c_dma_last_transfer_config(uint32_t i2c_periph, uint32_t dmalast);
/* whether to stretch SCL low when data is not ready in slave mode */
void i2c_stretch_scl_low_config(uint32_t i2c_periph, uint32_t stretchpara);
/* whether or not to response to a general call */
void i2c_slave_response_to_gcall_config(uint32_t i2c_periph, uint32_t gcallpara);
/* configure software reset of I2C */
void i2c_software_reset_config(uint32_t i2c_periph, uint32_t sreset);
/* configure I2C PEC calculation */
void i2c_pec_config(uint32_t i2c_periph, uint32_t pecstate);
/* configure whether to transfer PEC value */
void i2c_pec_transfer_config(uint32_t i2c_periph, uint32_t pecpara);
/* get packet error checking value */
uint8_t i2c_pec_value_get(uint32_t i2c_periph);
/* configure I2C alert through SMBA pin */
void i2c_smbus_alert_config(uint32_t i2c_periph, uint32_t smbuspara);
/* configure I2C ARP protocol in SMBus */
void i2c_smbus_arp_config(uint32_t i2c_periph, uint32_t arpstate);
/* disable analog noise filter */
void i2c_analog_noise_filter_disable(uint32_t i2c_periph);
/* enable analog noise filter */
void i2c_analog_noise_filter_enable(uint32_t i2c_periph);
/* configure digital noise filter */
void i2c_digital_noise_filter_config(uint32_t i2c_periph, i2c_digital_filter_enum dfilterpara);
/* enable SAM_V interface */
void i2c_sam_enable(uint32_t i2c_periph);
/* disable SAM_V interface */
void i2c_sam_disable(uint32_t i2c_periph);
/* enable SAM_V interface timeout detect */
void i2c_sam_timeout_enable(uint32_t i2c_periph);
/* disable SAM_V interface timeout detect */
void i2c_sam_timeout_disable(uint32_t i2c_periph);

/* interrupt & flag functions */
/* get I2C flag status */
FlagStatus i2c_flag_get(uint32_t i2c_periph, i2c_flag_enum flag);
/* clear I2C flag status */
void i2c_flag_clear(uint32_t i2c_periph, i2c_flag_enum flag);
/* enable I2C interrupt */
void i2c_interrupt_enable(uint32_t i2c_periph, i2c_interrupt_enum interrupt);
/* disable I2C interrupt */
void i2c_interrupt_disable(uint32_t i2c_periph, i2c_interrupt_enum interrupt);
/* get I2C interrupt flag status */
FlagStatus i2c_interrupt_flag_get(uint32_t i2c_periph, i2c_interrupt_flag_enum int_flag);
/* clear I2C interrupt flag status */
void i2c_interrupt_flag_clear(uint32_t i2c_periph, i2c_interrupt_flag_enum int_flag);

#endif /* GD32F4XX_I2C_H */
