/*!
    \file    gd32f4xx_i2c.c
    \brief   I2C driver

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

#include "gd32f4xx_i2c.h"

/* I2C register bit mask */
#define I2CCLK_MAX                    ((uint32_t)0x0000003CU)             /*!< i2cclk maximum value */
#define I2CCLK_MIN                    ((uint32_t)0x00000002U)             /*!< i2cclk minimum value */
#define I2C_FLAG_MASK                 ((uint32_t)0x0000FFFFU)             /*!< i2c flag mask */
#define I2C_ADDRESS_MASK              ((uint32_t)0x000003FFU)             /*!< i2c address mask */
#define I2C_ADDRESS2_MASK             ((uint32_t)0x000000FEU)             /*!< the second i2c address mask */

/* I2C register bit offset */
#define STAT1_PECV_OFFSET             ((uint32_t)0x00000008U)             /* bit offset of PECV in I2C_STAT1 */

/*!
    \brief      reset I2C
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_deinit(uint32_t i2c_periph)
{
    switch(i2c_periph) {
    case I2C0:
        /* reset I2C0 */
        rcu_periph_reset_enable(RCU_I2C0RST);
        rcu_periph_reset_disable(RCU_I2C0RST);
        break;
    case I2C1:
        /* reset I2C1 */
        rcu_periph_reset_enable(RCU_I2C1RST);
        rcu_periph_reset_disable(RCU_I2C1RST);
        break;
    case I2C2:
        /* reset I2C2 */
        rcu_periph_reset_enable(RCU_I2C2RST);
        rcu_periph_reset_disable(RCU_I2C2RST);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure I2C clock
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  clkspeed: I2C clock speed, supports standard mode (up to 100 kHz), fast mode (up to 400 kHz)
    \param[in]  dutycyc: duty cycle in fast mode
                only one parameter can be selected which is shown as below:
      \arg        I2C_DTCY_2: T_low/T_high = 2 in fast mode
      \arg        I2C_DTCY_16_9: T_low/T_high = 16/9 in fast mode
    \param[out] none
    \retval     none
*/
void i2c_clock_config(uint32_t i2c_periph, uint32_t clkspeed, uint32_t dutycyc)
{
    uint32_t pclk1, clkc, freq, risetime;
    uint32_t temp;

    pclk1 = rcu_clock_freq_get(CK_APB1);
    /* I2C peripheral clock frequency */
    freq = (uint32_t)(pclk1 / 1000000U);
    if(freq >= I2CCLK_MAX) {
        freq = I2CCLK_MAX;
    }
    temp = I2C_CTL1(i2c_periph);
    temp &= ~I2C_CTL1_I2CCLK;
    temp |= freq;

    I2C_CTL1(i2c_periph) = temp;

    if(100000U >= clkspeed) {
        /* the maximum SCL rise time is 1000ns in standard mode */
        risetime = (uint32_t)((pclk1 / 1000000U) + 1U);
        if(risetime >= I2CCLK_MAX) {
            I2C_RT(i2c_periph) = I2CCLK_MAX;
        } else if(risetime <= I2CCLK_MIN) {
            I2C_RT(i2c_periph) = I2CCLK_MIN;
        } else {
            I2C_RT(i2c_periph) = risetime;
        }
        clkc = (uint32_t)(pclk1 / (clkspeed * 2U));
        if(clkc < 0x04U) {
            /* the CLKC in standard mode minmum value is 4 */
            clkc = 0x04U;
        }

        I2C_CKCFG(i2c_periph) |= (I2C_CKCFG_CLKC & clkc);

    } else if(400000U >= clkspeed) {
        /* the maximum SCL rise time is 300ns in fast mode */
        I2C_RT(i2c_periph) = (uint32_t)(((freq * (uint32_t)300U) / (uint32_t)1000U) + (uint32_t)1U);
        if(I2C_DTCY_2 == dutycyc) {
            /* I2C duty cycle is 2 */
            clkc = (uint32_t)(pclk1 / (clkspeed * 3U));
            I2C_CKCFG(i2c_periph) &= ~I2C_CKCFG_DTCY;
        } else {
            /* I2C duty cycle is 16/9 */
            clkc = (uint32_t)(pclk1 / (clkspeed * 25U));
            I2C_CKCFG(i2c_periph) |= I2C_CKCFG_DTCY;
        }
        if(0U == (clkc & I2C_CKCFG_CLKC)) {
            /* the CLKC in fast mode minmum value is 1 */
            clkc |= 0x0001U;
        }
        I2C_CKCFG(i2c_periph) |= I2C_CKCFG_FAST;
        I2C_CKCFG(i2c_periph) |= clkc;
    } else {
    }
}

/*!
    \brief      configure I2C address
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  mode:
                only one parameter can be selected which is shown as below:
      \arg        I2C_I2CMODE_ENABLE: I2C mode
      \arg        I2C_SMBUSMODE_ENABLE: SMBus mode
    \param[in]  addformat: 7bits or 10bits
                only one parameter can be selected which is shown as below:
      \arg        I2C_ADDFORMAT_7BITS: address format is 7 bits
      \arg        I2C_ADDFORMAT_10BITS: address format is 10 bits
    \param[in]  addr: I2C address
    \param[out] none
    \retval     none
*/
void i2c_mode_addr_config(uint32_t i2c_periph, uint32_t mode, uint32_t addformat, uint32_t addr)
{
    /* SMBus/I2C mode selected */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_SMBEN);
    ctl |= mode;
    I2C_CTL0(i2c_periph) = ctl;
    /* configure address */
    addr = addr & I2C_ADDRESS_MASK;
    I2C_SADDR0(i2c_periph) = (addformat | addr);
}

/*!
    \brief      select SMBus type
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  type:
                only one parameter can be selected which is shown as below:
      \arg        I2C_SMBUS_DEVICE: SMBus mode device type
      \arg        I2C_SMBUS_HOST: SMBus mode host type
    \param[out] none
    \retval     none
*/
void i2c_smbus_type_config(uint32_t i2c_periph, uint32_t type)
{
    if(I2C_SMBUS_HOST == type) {
        I2C_CTL0(i2c_periph) |= I2C_CTL0_SMBSEL;
    } else {
        I2C_CTL0(i2c_periph) &= ~(I2C_CTL0_SMBSEL);
    }
}

/*!
    \brief      whether or not to send an ACK
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  ack:
                only one parameter can be selected which is shown as below:
      \arg        I2C_ACK_ENABLE: ACK will be sent
      \arg        I2C_ACK_DISABLE: ACK will not be sent
    \param[out] none
    \retval     none
*/
void i2c_ack_config(uint32_t i2c_periph, uint32_t ack)
{
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_ACKEN);
    ctl |= ack;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      configure I2C POAP position
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  pos:
                only one parameter can be selected which is shown as below:
      \arg        I2C_ACKPOS_CURRENT: ACKEN bit decides whether or not to send ACK or not for the current byte
      \arg        I2C_ACKPOS_NEXT: ACKEN bit decides whether or not to send ACK for the next byte
    \param[out] none
    \retval     none
*/
void i2c_ackpos_config(uint32_t i2c_periph, uint32_t pos)
{
    uint32_t ctl = 0U;
    /* configure I2C POAP position */
    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_POAP);
    ctl |= pos;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      master sends slave address
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  addr: slave address
    \param[in]  trandirection: transmitter or receiver
                only one parameter can be selected which is shown as below:
      \arg        I2C_TRANSMITTER: transmitter
      \arg        I2C_RECEIVER: receiver
    \param[out] none
    \retval     none
*/
void i2c_master_addressing(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection)
{
    /* master is a transmitter or a receiver */
    if(I2C_TRANSMITTER == trandirection) {
        addr = addr & I2C_TRANSMITTER;
    } else {
        addr = addr | I2C_RECEIVER;
    }
    /* send slave address */
    I2C_DATA(i2c_periph) = addr;
}

/*!
    \brief      enable dual-address mode
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  addr: the second address in dual-address mode
    \param[out] none
    \retval     none
*/
void i2c_dualaddr_enable(uint32_t i2c_periph, uint32_t addr)
{
    /* configure address */
    addr = addr & I2C_ADDRESS2_MASK;
    I2C_SADDR1(i2c_periph) = (I2C_SADDR1_DUADEN | addr);
}

/*!
    \brief      disable dual-address mode
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_dualaddr_disable(uint32_t i2c_periph)
{
    I2C_SADDR1(i2c_periph) &= ~(I2C_SADDR1_DUADEN);
}

/*!
    \brief      enable I2C
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_enable(uint32_t i2c_periph)
{
    I2C_CTL0(i2c_periph) |= I2C_CTL0_I2CEN;
}

/*!
    \brief      disable I2C
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_disable(uint32_t i2c_periph)
{
    I2C_CTL0(i2c_periph) &= ~(I2C_CTL0_I2CEN);
}

/*!
    \brief      generate a START condition on I2C bus
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_start_on_bus(uint32_t i2c_periph)
{
    I2C_CTL0(i2c_periph) |= I2C_CTL0_START;
}

/*!
    \brief      generate a STOP condition on I2C bus
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_stop_on_bus(uint32_t i2c_periph)
{
    I2C_CTL0(i2c_periph) |= I2C_CTL0_STOP;
}

/*!
    \brief      I2C transmit data function
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  data: data of transmission
    \param[out] none
    \retval     none
*/
void i2c_data_transmit(uint32_t i2c_periph, uint8_t data)
{
    I2C_DATA(i2c_periph) = DATA_TRANS(data);
}

/*!
    \brief      I2C receive data function
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     data of received
*/
uint8_t i2c_data_receive(uint32_t i2c_periph)
{
    return (uint8_t)DATA_RECV(I2C_DATA(i2c_periph));
}

/*!
    \brief      configure I2C DMA mode
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  dmastate:
                only one parameter can be selected which is shown as below:
      \arg        I2C_DMA_ON: enable DMA mode
      \arg        I2C_DMA_OFF: disable DMA mode
    \param[out] none
    \retval     none
*/
void i2c_dma_config(uint32_t i2c_periph, uint32_t dmastate)
{
    /* configure I2C DMA function */
    uint32_t ctl = 0U;

    ctl = I2C_CTL1(i2c_periph);
    ctl &= ~(I2C_CTL1_DMAON);
    ctl |= dmastate;
    I2C_CTL1(i2c_periph) = ctl;
}

/*!
    \brief      configure whether next DMA EOT is DMA last transfer or not
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  dmalast:
                only one parameter can be selected which is shown as below:
      \arg        I2C_DMALST_ON: next DMA EOT is the last transfer
      \arg        I2C_DMALST_OFF: next DMA EOT is not the last transfer
    \param[out] none
    \retval     none
*/
void i2c_dma_last_transfer_config(uint32_t i2c_periph, uint32_t dmalast)
{
    /* configure DMA last transfer */
    uint32_t ctl = 0U;

    ctl = I2C_CTL1(i2c_periph);
    ctl &= ~(I2C_CTL1_DMALST);
    ctl |= dmalast;
    I2C_CTL1(i2c_periph) = ctl;
}

/*!
    \brief      whether to stretch SCL low when data is not ready in slave mode
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  stretchpara:
                only one parameter can be selected which is shown as below:
      \arg        I2C_SCLSTRETCH_ENABLE: enable SCL stretching
      \arg        I2C_SCLSTRETCH_DISABLE: disable SCL stretching
    \param[out] none
    \retval     none
*/
void i2c_stretch_scl_low_config(uint32_t i2c_periph, uint32_t stretchpara)
{
    /* configure I2C SCL strerching */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_SS);
    ctl |= stretchpara;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      whether or not to response to a general call
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  gcallpara:
                only one parameter can be selected which is shown as below:
      \arg        I2C_GCEN_ENABLE: slave will response to a general call
      \arg        I2C_GCEN_DISABLE: slave will not response to a general call
    \param[out] none
    \retval     none
*/
void i2c_slave_response_to_gcall_config(uint32_t i2c_periph, uint32_t gcallpara)
{
    /* configure slave response to a general call enable or disable */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_GCEN);
    ctl |= gcallpara;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      configure software reset of I2C
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  sreset:
                only one parameter can be selected which is shown as below:
      \arg        I2C_SRESET_SET: I2C is under reset
      \arg        I2C_SRESET_RESET: I2C is not under reset
    \param[out] none
    \retval     none
*/
void i2c_software_reset_config(uint32_t i2c_periph, uint32_t sreset)
{
    /* modify CTL0 and configure software reset I2C state */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_SRESET);
    ctl |= sreset;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      configure I2C PEC calculation
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  pecstate:
                only one parameter can be selected which is shown as below:
      \arg        I2C_PEC_ENABLE: PEC calculation on
      \arg        I2C_PEC_DISABLE: PEC calculation off
    \param[out] none
    \retval     none
*/
void i2c_pec_config(uint32_t i2c_periph, uint32_t pecstate)
{
    /* on/off PEC calculation */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_PECEN);
    ctl |= pecstate;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      configure whether to transfer PEC value
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  pecpara:
                only one parameter can be selected which is shown as below:
      \arg        I2C_PECTRANS_ENABLE: transfer PEC value
      \arg        I2C_PECTRANS_DISABLE: not transfer PEC value
    \param[out] none
    \retval     none
*/
void i2c_pec_transfer_config(uint32_t i2c_periph, uint32_t pecpara)
{
    /* whether to transfer PEC */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_PECTRANS);
    ctl |= pecpara;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      get packet error checking value
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     PEC value
*/
uint8_t i2c_pec_value_get(uint32_t i2c_periph)
{
    return (uint8_t)((I2C_STAT1(i2c_periph) & I2C_STAT1_PECV) >> STAT1_PECV_OFFSET);
}

/*!
    \brief      configure I2C alert through SMBA pin
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  smbuspara:
                only one parameter can be selected which is shown as below:
      \arg        I2C_SALTSEND_ENABLE: issue alert through SMBA pin
      \arg        I2C_SALTSEND_DISABLE: not issue alert through SMBA pin
    \param[out] none
    \retval     none
*/
void i2c_smbus_alert_config(uint32_t i2c_periph, uint32_t smbuspara)
{
    /* configure smubus alert through SMBA pin */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_SALT);
    ctl |= smbuspara;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      configure I2C ARP protocol in SMBus
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  arpstate:
                only one parameter can be selected which is shown as below:
      \arg        I2C_ARP_ENABLE: enable ARP
      \arg        I2C_ARP_DISABLE: disable ARP
    \param[out] none
    \retval     none
*/
void i2c_smbus_arp_config(uint32_t i2c_periph, uint32_t arpstate)
{
    /* enable or disable I2C ARP protocol*/
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(i2c_periph);
    ctl &= ~(I2C_CTL0_ARPEN);
    ctl |= arpstate;
    I2C_CTL0(i2c_periph) = ctl;
}

/*!
    \brief      disable analog noise filter
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_analog_noise_filter_disable(uint32_t i2c_periph)
{
    I2C_FCTL(i2c_periph) |= I2C_FCTL_AFD;
}

/*!
    \brief      enable analog noise filter
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_analog_noise_filter_enable(uint32_t i2c_periph)
{
    I2C_FCTL(i2c_periph) &= ~(I2C_FCTL_AFD);
}

/*!
    \brief      configure digital noise filter
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  dfilterpara: refer to i2c_digital_filter_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_DF_DISABLE: disable digital noise filter
      \arg        I2C_DF_1PCLK: enable digital noise filter and the maximum filtered spiker's length 1 PCLK1
      \arg        I2C_DF_2PCLK: enable digital noise filter and the maximum filtered spiker's length 2 PCLK1
      \arg        I2C_DF_3PCLK: enable digital noise filter and the maximum filtered spiker's length 3 PCLK1
      \arg        I2C_DF_4PCLK: enable digital noise filter and the maximum filtered spiker's length 4 PCLK1
      \arg        I2C_DF_5PCLK: enable digital noise filter and the maximum filtered spiker's length 5 PCLK1
      \arg        I2C_DF_6PCLK: enable digital noise filter and the maximum filtered spiker's length 6 PCLK1
      \arg        I2C_DF_7PCLK: enable digital noise filter and the maximum filtered spiker's length 7 PCLK1
      \arg        I2C_DF_8PCLK: enable digital noise filter and the maximum filtered spiker's length 8 PCLK1
      \arg        I2C_DF_9PCLK: enable digital noise filter and the maximum filtered spiker's length 9 PCLK1
      \arg        I2C_DF_10PCLK: enable digital noise filter and the maximum filtered spiker's length 10 PCLK1
      \arg        I2C_DF_11CLK: enable digital noise filter and the maximum filtered spiker's length 11 PCLK1
      \arg        I2C_DF_12CLK: enable digital noise filter and the maximum filtered spiker's length 12 PCLK1
      \arg        I2C_DF_13PCLK: enable digital noise filter and the maximum filtered spiker's length 13 PCLK1
      \arg        I2C_DF_14PCLK: enable digital noise filter and the maximum filtered spiker's length 14 PCLK1
      \arg        I2C_DF_15PCLK: enable digital noise filter and the maximum filtered spiker's length 15 PCLK1
    \param[out] none
    \retval     none
*/
void i2c_digital_noise_filter_config(uint32_t i2c_periph, i2c_digital_filter_enum dfilterpara)
{
    I2C_FCTL(i2c_periph) |= dfilterpara;
}

/*!
    \brief      enable SAM_V interface
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_sam_enable(uint32_t i2c_periph)
{
    I2C_SAMCS(i2c_periph) |= I2C_SAMCS_SAMEN;
}

/*!
    \brief      disable SAM_V interface
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_sam_disable(uint32_t i2c_periph)
{
    I2C_SAMCS(i2c_periph) &= ~(I2C_SAMCS_SAMEN);
}

/*!
    \brief      enable SAM_V interface timeout detect
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_sam_timeout_enable(uint32_t i2c_periph)
{
    I2C_SAMCS(i2c_periph) |= I2C_SAMCS_STOEN;
}

/*!
    \brief      disable SAM_V interface timeout detect
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void i2c_sam_timeout_disable(uint32_t i2c_periph)
{
    I2C_SAMCS(i2c_periph) &= ~(I2C_SAMCS_STOEN);
}

/*!
    \brief      get I2C flag status
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  flag: I2C flags, refer to i2c_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_FLAG_SBSEND: start condition sent out in master mode
      \arg        I2C_FLAG_ADDSEND: address is sent in master mode or received and matches in slave mode
      \arg        I2C_FLAG_BTC: byte transmission finishes
      \arg        I2C_FLAG_ADD10SEND: header of 10-bit address is sent in master mode
      \arg        I2C_FLAG_STPDET: stop condition detected in slave mode
      \arg        I2C_FLAG_RBNE: I2C_DATA is not empty during receiving
      \arg        I2C_FLAG_TBE: I2C_DATA is empty during transmitting
      \arg        I2C_FLAG_BERR: a bus error occurs indication a unexpected start or stop condition on I2C bus
      \arg        I2C_FLAG_LOSTARB: arbitration lost in master mode
      \arg        I2C_FLAG_AERR: acknowledge error
      \arg        I2C_FLAG_OUERR: over-run or under-run situation occurs in slave mode
      \arg        I2C_FLAG_PECERR: PEC error when receiving data
      \arg        I2C_FLAG_SMBTO: timeout signal in SMBus mode
      \arg        I2C_FLAG_SMBALT: SMBus alert status
      \arg        I2C_FLAG_MASTER: a flag indicating whether I2C block is in master or slave mode
      \arg        I2C_FLAG_I2CBSY: busy flag
      \arg        I2C_FLAG_TR: whether the I2C is a transmitter or a receiver
      \arg        I2C_FLAG_RXGC: general call address (00h) received
      \arg        I2C_FLAG_DEFSMB: default address of SMBus device
      \arg        I2C_FLAG_HSTSMB: SMBus host header detected in slave mode
      \arg        I2C_FLAG_DUMOD: dual flag in slave mode indicating which address is matched in dual-address mode
      \arg        I2C_FLAG_TFF: txframe fall flag
      \arg        I2C_FLAG_TFR: txframe rise flag
      \arg        I2C_FLAG_RFF: rxframe fall flag
      \arg        I2C_FLAG_RFR: rxframe rise flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus i2c_flag_get(uint32_t i2c_periph, i2c_flag_enum flag)
{
    FlagStatus status;
    
    if(RESET != (I2C_REG_VAL(i2c_periph, flag) & BIT(I2C_BIT_POS(flag)))) {
        status = SET;
    } else {
        status = RESET;
    }
    
    return status;
}

/*!
    \brief      clear I2C flag status
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  flag: I2C flags, refer to i2c_flag_enum
                only one parameter can be selected which is shown as below:
      \arg       I2C_FLAG_SMBALT: SMBus alert status
      \arg       I2C_FLAG_SMBTO: timeout signal in SMBus mode
      \arg       I2C_FLAG_PECERR: PEC error when receiving data
      \arg       I2C_FLAG_OUERR: over-run or under-run situation occurs in slave mode
      \arg       I2C_FLAG_AERR: acknowledge error
      \arg       I2C_FLAG_LOSTARB: arbitration lost in master mode
      \arg       I2C_FLAG_BERR: a bus error occurs indication a unexpected start or stop condition on I2C bus
      \arg       I2C_FLAG_ADDSEND: address is sent in master mode or received and matches in slave mode
      \arg       I2C_FLAG_TFF: txframe fall flag
      \arg       I2C_FLAG_TFR: txframe rise flag
      \arg       I2C_FLAG_RFF: rxframe fall flag
      \arg       I2C_FLAG_RFR: rxframe rise flag
    \param[out] none
    \retval     none
*/
void i2c_flag_clear(uint32_t i2c_periph, i2c_flag_enum flag)
{
    if(I2C_FLAG_ADDSEND == flag) {
        /* read I2C_STAT0 and then read I2C_STAT1 to clear ADDSEND */
        I2C_STAT0(i2c_periph);
        I2C_STAT1(i2c_periph);
    } else {
        I2C_REG_VAL(i2c_periph, flag) &= ~BIT(I2C_BIT_POS(flag));
    }
}

/*!
    \brief      enable I2C interrupt
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  interrupt: I2C interrupts, refer to i2c_interrupt_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_INT_ERR: error interrupt
      \arg        I2C_INT_EV: event interrupt
      \arg        I2C_INT_BUF: buffer interrupt
      \arg        I2C_INT_TFF: txframe fall interrupt
      \arg        I2C_INT_TFR: txframe rise interrupt
      \arg        I2C_INT_RFF: rxframe fall interrupt
      \arg        I2C_INT_RFR: rxframe rise interrupt
    \param[out] none
    \retval     none
*/
void i2c_interrupt_enable(uint32_t i2c_periph, i2c_interrupt_enum interrupt)
{
    I2C_REG_VAL(i2c_periph, interrupt) |= BIT(I2C_BIT_POS(interrupt));
}

/*!
    \brief      disable I2C interrupt
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  interrupt: I2C interrupts, refer to i2c_interrupt_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_INT_ERR: error interrupt
      \arg        I2C_INT_EV: event interrupt
      \arg        I2C_INT_BUF: buffer interrupt
      \arg        I2C_INT_TFF: txframe fall interrupt
      \arg        I2C_INT_TFR: txframe rise interrupt
      \arg        I2C_INT_RFF: rxframe fall interrupt
      \arg        I2C_INT_RFR: rxframe rise interrupt
    \param[out] none
    \retval     none
*/
void i2c_interrupt_disable(uint32_t i2c_periph, i2c_interrupt_enum interrupt)
{
    I2C_REG_VAL(i2c_periph, interrupt) &= ~BIT(I2C_BIT_POS(interrupt));
}

/*!
    \brief      get I2C interrupt flag status
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  int_flag: I2C interrupt flags, refer to i2c_interrupt_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_INT_FLAG_SBSEND: start condition sent out in master mode interrupt flag
      \arg        I2C_INT_FLAG_ADDSEND: address is sent in master mode or received and matches in slave mode interrupt flag
      \arg        I2C_INT_FLAG_BTC: byte transmission finishes interrupt flag
      \arg        I2C_INT_FLAG_ADD10SEND: header of 10-bit address is sent in master mode interrupt flag
      \arg        I2C_INT_FLAG_STPDET: stop condition detected in slave mode interrupt flag
      \arg        I2C_INT_FLAG_RBNE: I2C_DATA is not Empty during receiving interrupt flag
      \arg        I2C_INT_FLAG_TBE: I2C_DATA is empty during transmitting interrupt flag
      \arg        I2C_INT_FLAG_BERR: a bus error occurs indication a unexpected start or stop condition on I2C bus interrupt flag
      \arg        I2C_INT_FLAG_LOSTARB: arbitration lost in master mode interrupt flag
      \arg        I2C_INT_FLAG_AERR: acknowledge error interrupt flag
      \arg        I2C_INT_FLAG_OUERR: over-run or under-run situation occurs in slave mode interrupt flag
      \arg        I2C_INT_FLAG_PECERR: PEC error when receiving data interrupt flag
      \arg        I2C_INT_FLAG_SMBTO: timeout signal in SMBus mode interrupt flag
      \arg        I2C_INT_FLAG_SMBALT: SMBus alert status interrupt flag
      \arg        I2C_INT_FLAG_TFF: txframe fall interrupt flag
      \arg        I2C_INT_FLAG_TFR: txframe rise interrupt flag
      \arg        I2C_INT_FLAG_RFF: rxframe fall interrupt flag
      \arg        I2C_INT_FLAG_RFR: rxframe rise interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus i2c_interrupt_flag_get(uint32_t i2c_periph, i2c_interrupt_flag_enum int_flag)
{
    uint32_t intenable = 0U, flagstatus = 0U, bufie;
    FlagStatus status;

    /* check BUFIE */
    bufie = I2C_CTL1(i2c_periph)&I2C_CTL1_BUFIE;

    /* get the interrupt enable bit status */
    intenable = (I2C_REG_VAL(i2c_periph, int_flag) & BIT(I2C_BIT_POS(int_flag)));
    /* get the corresponding flag bit status */
    flagstatus = (I2C_REG_VAL2(i2c_periph, int_flag) & BIT(I2C_BIT_POS2(int_flag)));

    if((I2C_INT_FLAG_RBNE == int_flag) || (I2C_INT_FLAG_TBE == int_flag)) {
        if(intenable && bufie) {
            intenable = 1U;
        } else {
            intenable = 0U;
        }
    }
    
    if((0U != flagstatus) && (0U != intenable)) {
        status = SET;
    } else {
        status = RESET;
    }
    
    return status;
}

/*!
    \brief      clear I2C interrupt flag status
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  int_flag: I2C interrupt flags, refer to i2c_interrupt_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        I2C_INT_FLAG_ADDSEND: address is sent in master mode or received and matches in slave mode interrupt flag
      \arg        I2C_INT_FLAG_BERR: a bus error occurs indication a unexpected start or stop condition on I2C bus interrupt flag
      \arg        I2C_INT_FLAG_LOSTARB: arbitration lost in master mode interrupt flag
      \arg        I2C_INT_FLAG_AERR: acknowledge error interrupt flag
      \arg        I2C_INT_FLAG_OUERR: over-run or under-run situation occurs in slave mode interrupt flag
      \arg        I2C_INT_FLAG_PECERR: PEC error when receiving data interrupt flag
      \arg        I2C_INT_FLAG_SMBTO: timeout signal in SMBus mode interrupt flag
      \arg        I2C_INT_FLAG_SMBALT: SMBus alert status interrupt flag
      \arg        I2C_INT_FLAG_TFF: txframe fall interrupt flag
      \arg        I2C_INT_FLAG_TFR: txframe rise interrupt flag
      \arg        I2C_INT_FLAG_RFF: rxframe fall interrupt flag
      \arg        I2C_INT_FLAG_RFR: rxframe rise interrupt flag
    \param[out] none
    \retval     none
*/
void i2c_interrupt_flag_clear(uint32_t i2c_periph, i2c_interrupt_flag_enum int_flag)
{
    if(I2C_INT_FLAG_ADDSEND == int_flag) {
        /* read I2C_STAT0 and then read I2C_STAT1 to clear ADDSEND */
        I2C_STAT0(i2c_periph);
        I2C_STAT1(i2c_periph);
    } else {
        I2C_REG_VAL2(i2c_periph, int_flag) &= ~BIT(I2C_BIT_POS2(int_flag));
    }
}
