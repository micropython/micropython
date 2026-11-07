/*!
    \file    gd32f4xx_spi.c
    \brief   SPI driver

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


#include "gd32f4xx_spi.h"
#include "gd32f4xx_rcu.h"

/* SPI/I2S parameter initialization mask */
#define SPI_INIT_MASK                   ((uint32_t)0x00003040U)  /*!< SPI parameter initialization mask */
#define I2S_INIT_MASK                   ((uint32_t)0x0000F047U)  /*!< I2S parameter initialization mask */
#define I2S_FULL_DUPLEX_MASK            ((uint32_t)0x00000480U)  /*!< I2S full duples mode configure parameter initialization mask */

/* default value */
#define SPI_I2SPSC_DEFAULT_VALUE        ((uint32_t)0x00000002U)  /*!< default value of SPI_I2SPSC register */

/*!
    \brief      deinitialize SPI and I2S
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5),include I2S1_ADD and I2S2_ADD
    \param[out] none
    \retval     none
*/
void spi_i2s_deinit(uint32_t spi_periph)
{
    switch(spi_periph) {
    case SPI0:
        /* reset SPI0 */
        rcu_periph_reset_enable(RCU_SPI0RST);
        rcu_periph_reset_disable(RCU_SPI0RST);
        break;
    case SPI1:
        /* reset SPI1,I2S1 and I2S1_ADD */
        rcu_periph_reset_enable(RCU_SPI1RST);
        rcu_periph_reset_disable(RCU_SPI1RST);
        break;
    case SPI2:
        /* reset SPI2,I2S2 and I2S2_ADD */
        rcu_periph_reset_enable(RCU_SPI2RST);
        rcu_periph_reset_disable(RCU_SPI2RST);
        break;
    case SPI3:
        /* reset SPI3 */
        rcu_periph_reset_enable(RCU_SPI3RST);
        rcu_periph_reset_disable(RCU_SPI3RST);
        break;
    case SPI4:
        /* reset SPI4 */
        rcu_periph_reset_enable(RCU_SPI4RST);
        rcu_periph_reset_disable(RCU_SPI4RST);
        break;
    case SPI5:
        /* reset SPI5 */
        rcu_periph_reset_enable(RCU_SPI5RST);
        rcu_periph_reset_disable(RCU_SPI5RST);
        break;
    default :
        break;
    }
}

/*!
    \brief      initialize the parameters of SPI struct with default values
    \param[in]  none
    \param[out] spi_parameter_struct: the initialized struct spi_parameter_struct pointer
    \retval     none
*/
void spi_struct_para_init(spi_parameter_struct *spi_struct)
{
    /* configure the structure with default value */
    spi_struct->device_mode          = SPI_SLAVE;
    spi_struct->trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_struct->frame_size           = SPI_FRAMESIZE_8BIT;
    spi_struct->nss                  = SPI_NSS_HARD;
    spi_struct->clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_struct->prescale             = SPI_PSC_2;
    spi_struct->endian               = SPI_ENDIAN_MSB;
}
/*!
    \brief      initialize SPI parameter
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_struct: SPI parameter initialization stuct members of the structure
                and the member values are shown as below:
                  device_mode: SPI_MASTER, SPI_SLAVE.
                  trans_mode: SPI_TRANSMODE_FULLDUPLEX, SPI_TRANSMODE_RECEIVEONLY,
                              SPI_TRANSMODE_BDRECEIVE, SPI_TRANSMODE_BDTRANSMIT
                  frame_size: SPI_FRAMESIZE_16BIT, SPI_FRAMESIZE_8BIT
                  nss: SPI_NSS_SOFT, SPI_NSS_HARD
                  endian: SPI_ENDIAN_MSB, SPI_ENDIAN_LSB
                  clock_polarity_phase: SPI_CK_PL_LOW_PH_1EDGE, SPI_CK_PL_HIGH_PH_1EDGE
                                        SPI_CK_PL_LOW_PH_2EDGE, SPI_CK_PL_HIGH_PH_2EDGE
                  prescale: SPI_PSC_n (n=2,4,8,16,32,64,128,256)
    \param[out] none
    \retval     none
*/
void spi_init(uint32_t spi_periph, spi_parameter_struct *spi_struct)
{
    uint32_t reg = 0U;
    reg = SPI_CTL0(spi_periph);
    reg &= SPI_INIT_MASK;

    /* select SPI as master or slave */
    reg |= spi_struct->device_mode;
    /* select SPI transfer mode */
    reg |= spi_struct->trans_mode;
    /* select SPI frame size */
    reg |= spi_struct->frame_size;
    /* select SPI nss use hardware or software */
    reg |= spi_struct->nss;
    /* select SPI LSB or MSB */
    reg |= spi_struct->endian;
    /* select SPI polarity and phase */
    reg |= spi_struct->clock_polarity_phase;
    /* select SPI prescale to adjust transmit speed */
    reg |= spi_struct->prescale;

    /* write to SPI_CTL0 register */
    SPI_CTL0(spi_periph) = (uint32_t)reg;

    SPI_I2SCTL(spi_periph) &= (uint32_t)(~SPI_I2SCTL_I2SSEL);
}

/*!
    \brief      enable SPI
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_enable(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_SPIEN;
}

/*!
    \brief      disable SPI
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_disable(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_SPIEN);
}

/*!
    \brief      initialize I2S parameter
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[in]  i2s_mode: I2S operation mode
                only one parameter can be selected which is shown as below:
      \arg        I2S_MODE_SLAVETX : I2S slave transmit mode
      \arg        I2S_MODE_SLAVERX : I2S slave receive mode
      \arg        I2S_MODE_MASTERTX : I2S master transmit mode
      \arg        I2S_MODE_MASTERRX : I2S master receive mode
    \param[in]  i2s_standard: I2S standard
                only one parameter can be selected which is shown as below:
      \arg        I2S_STD_PHILLIPS : I2S phillips standard
      \arg        I2S_STD_MSB : I2S MSB standard
      \arg        I2S_STD_LSB : I2S LSB standard
      \arg        I2S_STD_PCMSHORT : I2S PCM short standard
      \arg        I2S_STD_PCMLONG : I2S PCM long standard
    \param[in]  i2s_ckpl: I2S idle state clock polarity
                only one parameter can be selected which is shown as below:
      \arg        I2S_CKPL_LOW : I2S clock polarity low level
      \arg        I2S_CKPL_HIGH : I2S clock polarity high level
    \param[out] none
    \retval     none
*/
void i2s_init(uint32_t spi_periph, uint32_t i2s_mode, uint32_t i2s_standard, uint32_t i2s_ckpl)
{
    uint32_t reg = 0U;
    reg = SPI_I2SCTL(spi_periph);
    reg &= I2S_INIT_MASK;

    /* enable I2S mode */
    reg |= (uint32_t)SPI_I2SCTL_I2SSEL;
    /* select I2S mode */
    reg |= (uint32_t)i2s_mode;
    /* select I2S standard */
    reg |= (uint32_t)i2s_standard;
    /* select I2S polarity */
    reg |= (uint32_t)i2s_ckpl;

    /* write to SPI_I2SCTL register */
    SPI_I2SCTL(spi_periph) = (uint32_t)reg;
}

/*!
    \brief      configure I2S prescale
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[in]  i2s_audiosample: I2S audio sample rate
                only one parameter can be selected which is shown as below:
      \arg        I2S_AUDIOSAMPLE_8K: audio sample rate is 8KHz
      \arg        I2S_AUDIOSAMPLE_11K: audio sample rate is 11KHz
      \arg        I2S_AUDIOSAMPLE_16K: audio sample rate is 16KHz
      \arg        I2S_AUDIOSAMPLE_22K: audio sample rate is 22KHz
      \arg        I2S_AUDIOSAMPLE_32K: audio sample rate is 32KHz
      \arg        I2S_AUDIOSAMPLE_44K: audio sample rate is 44KHz
      \arg        I2S_AUDIOSAMPLE_48K: audio sample rate is 48KHz
      \arg        I2S_AUDIOSAMPLE_96K: audio sample rate is 96KHz
      \arg        I2S_AUDIOSAMPLE_192K: audio sample rate is 192KHz
    \param[in]  i2s_frameformat: I2S data length and channel length
                only one parameter can be selected which is shown as below:
      \arg        I2S_FRAMEFORMAT_DT16B_CH16B: I2S data length is 16 bit and channel length is 16 bit
      \arg        I2S_FRAMEFORMAT_DT16B_CH32B: I2S data length is 16 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT24B_CH32B: I2S data length is 24 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT32B_CH32B: I2S data length is 32 bit and channel length is 32 bit
    \param[in]  i2s_mckout: I2S master clock output
                only one parameter can be selected which is shown as below:
      \arg        I2S_MCKOUT_ENABLE: I2S master clock output enable
      \arg        I2S_MCKOUT_DISABLE: I2S master clock output disable
    \param[out] none
    \retval     none
*/
void i2s_psc_config(uint32_t spi_periph, uint32_t i2s_audiosample, uint32_t i2s_frameformat, uint32_t i2s_mckout)
{
    uint32_t i2sdiv = 2U, i2sof = 0U;
    uint32_t clks = 0U;
    uint32_t i2sclock = 0U;

#ifndef I2S_EXTERNAL_CLOCK_IN
    uint32_t plli2sm = 0U, plli2sn = 0U, plli2sr = 0U;
#endif /* I2S_EXTERNAL_CLOCK_IN */

    /* deinit SPI_I2SPSC register */
    SPI_I2SPSC(spi_periph) = SPI_I2SPSC_DEFAULT_VALUE;

#ifdef I2S_EXTERNAL_CLOCK_IN
    rcu_i2s_clock_config(RCU_I2SSRC_I2S_CKIN);

    /* set the I2S clock to the external clock input value */
    i2sclock = I2S_EXTERNAL_CLOCK_IN;
#else

    /* turn on the oscillator HXTAL */
    rcu_osci_on(RCU_HXTAL);
    /* wait for oscillator stabilization flags is SET */
    rcu_osci_stab_wait(RCU_HXTAL);
    /* turn on the PLLI2S */
    rcu_osci_on(RCU_PLLI2S_CK);
    /* wait for PLLI2S flags is SET */
    rcu_osci_stab_wait(RCU_PLLI2S_CK);
    /* configure the I2S clock source selection */
    rcu_i2s_clock_config(RCU_I2SSRC_PLLI2S);

    /* get the RCU_PLL_PLLPSC value */
    plli2sm = (uint32_t)(RCU_PLL & RCU_PLL_PLLPSC);
    /* get the RCU_PLLI2S_PLLI2SN value */
    plli2sn = (uint32_t)((RCU_PLLI2S & RCU_PLLI2S_PLLI2SN) >> 6);
    /* get the RCU_PLLI2S_PLLI2SR value */
    plli2sr = (uint32_t)((RCU_PLLI2S & RCU_PLLI2S_PLLI2SR) >> 28);

    if((RCU_PLL & RCU_PLL_PLLSEL) == RCU_PLLSRC_HXTAL) {
        /* get the I2S source clock value */
        i2sclock = (uint32_t)(((HXTAL_VALUE / plli2sm) * plli2sn) / plli2sr);
    } else {
        /* get the I2S source clock value */
        i2sclock = (uint32_t)(((IRC16M_VALUE / plli2sm) * plli2sn) / plli2sr);
    }
#endif /* I2S_EXTERNAL_CLOCK_IN */

    /* config the prescaler depending on the mclk output state, the frame format and audio sample rate */
    if(I2S_MCKOUT_ENABLE == i2s_mckout) {
        clks = (uint32_t)(((i2sclock / 256U) * 10U) / i2s_audiosample);
    } else {
        if(I2S_FRAMEFORMAT_DT16B_CH16B == i2s_frameformat) {
            clks = (uint32_t)(((i2sclock / 32U) * 10U) / i2s_audiosample);
        } else {
            clks = (uint32_t)(((i2sclock / 64U) * 10U) / i2s_audiosample);
        }
    }
    /* remove the floating point */
    clks = (clks + 5U) / 10U;
    i2sof = (clks & 0x00000001U);
    i2sdiv = ((clks - i2sof) / 2U);
    i2sof = (i2sof << 8U);

    /* set the default values */
    if((i2sdiv < 2U) || (i2sdiv > 255U)) {
        i2sdiv = 2U;
        i2sof = 0U;
    }

    /* configure SPI_I2SPSC */
    SPI_I2SPSC(spi_periph) = (uint32_t)(i2sdiv | i2sof | i2s_mckout);

    /* clear SPI_I2SCTL_DTLEN and SPI_I2SCTL_CHLEN bits */
    SPI_I2SCTL(spi_periph) &= (uint32_t)(~(SPI_I2SCTL_DTLEN | SPI_I2SCTL_CHLEN));
    /* configure data frame format */
    SPI_I2SCTL(spi_periph) |= (uint32_t)i2s_frameformat;
}

/*!
    \brief      enable I2S
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[out] none
    \retval     none
*/
void i2s_enable(uint32_t spi_periph)
{
    SPI_I2SCTL(spi_periph) |= (uint32_t)SPI_I2SCTL_I2SEN;
}

/*!
    \brief      disable I2S
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[out] none
    \retval     none
*/
void i2s_disable(uint32_t spi_periph)
{
    SPI_I2SCTL(spi_periph) &= (uint32_t)(~SPI_I2SCTL_I2SEN);
}

/*!
    \brief      enable SPI nss output
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_nss_output_enable(uint32_t spi_periph)
{
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_NSSDRV;
}

/*!
    \brief      disable SPI nss output
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_nss_output_disable(uint32_t spi_periph)
{
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_NSSDRV);
}

/*!
    \brief      SPI nss pin high level in software mode
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_nss_internal_high(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_SWNSS;
}

/*!
    \brief      SPI nss pin low level in software mode
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_nss_internal_low(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_SWNSS);
}

/*!
    \brief      enable SPI DMA send or receive
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_dma: SPI DMA mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_DMA_TRANSMIT: SPI transmit data use DMA
      \arg        SPI_DMA_RECEIVE: SPI receive data use DMA
    \param[out] none
    \retval     none
*/
void spi_dma_enable(uint32_t spi_periph, uint8_t spi_dma)
{
    if(SPI_DMA_TRANSMIT == spi_dma) {
        SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_DMATEN;
    } else {
        SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_DMAREN;
    }
}

/*!
    \brief      diable SPI DMA send or receive
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_dma: SPI DMA mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_DMA_TRANSMIT: SPI transmit data use DMA
      \arg        SPI_DMA_RECEIVE: SPI receive data use DMA
    \param[out] none
    \retval     none
*/
void spi_dma_disable(uint32_t spi_periph, uint8_t spi_dma)
{
    if(SPI_DMA_TRANSMIT == spi_dma) {
        SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_DMATEN);
    } else {
        SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_DMAREN);
    }
}

/*!
    \brief      configure SPI/I2S data frame format
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  frame_format: SPI frame size
                only one parameter can be selected which is shown as below:
      \arg        SPI_FRAMESIZE_16BIT: SPI frame size is 16 bits
      \arg        SPI_FRAMESIZE_8BIT: SPI frame size is 8 bits
    \param[out] none
    \retval     none
*/
void spi_i2s_data_frame_format_config(uint32_t spi_periph, uint16_t frame_format)
{
    /* clear SPI_CTL0_FF16 bit */
    SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_FF16);
    /* configure SPI_CTL0_FF16 bit */
    SPI_CTL0(spi_periph) |= (uint32_t)frame_format;
}

/*!
    \brief      SPI transmit data
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  data: 16-bit data
    \param[out] none
    \retval     none
*/
void spi_i2s_data_transmit(uint32_t spi_periph, uint16_t data)
{
    SPI_DATA(spi_periph) = (uint32_t)data;
}

/*!
    \brief      SPI receive data
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     16-bit data
*/
uint16_t spi_i2s_data_receive(uint32_t spi_periph)
{
    return ((uint16_t)SPI_DATA(spi_periph));
}

/*!
    \brief      configure SPI bidirectional transfer direction
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  transfer_direction: SPI transfer direction
                only one parameter can be selected which is shown as below:
      \arg        SPI_BIDIRECTIONAL_TRANSMIT: SPI work in transmit-only mode
      \arg        SPI_BIDIRECTIONAL_RECEIVE: SPI work in receive-only mode
    \retval     none
*/
void spi_bidirectional_transfer_config(uint32_t spi_periph, uint32_t transfer_direction)
{
    if(SPI_BIDIRECTIONAL_TRANSMIT == transfer_direction) {
        /* set the transmit only mode */
        SPI_CTL0(spi_periph) |= (uint32_t)SPI_BIDIRECTIONAL_TRANSMIT;
    } else {
        /* set the receive only mode */
        SPI_CTL0(spi_periph) &= SPI_BIDIRECTIONAL_RECEIVE;
    }
}

/*!
    \brief      configure i2s full duplex mode
    \param[in]  i2s_add_periph: I2Sx_ADD(x=1,2)
    \param[in]  i2s_mode:
      \arg        I2S_MODE_SLAVETX : I2S slave transmit mode
      \arg        I2S_MODE_SLAVERX : I2S slave receive mode
      \arg        I2S_MODE_MASTERTX : I2S master transmit mode
      \arg        I2S_MODE_MASTERRX : I2S master receive mode
    \param[in]  i2s_standard:
      \arg        I2S_STD_PHILLIPS : I2S phillips standard
      \arg        I2S_STD_MSB : I2S MSB standard
      \arg        I2S_STD_LSB : I2S LSB standard
      \arg        I2S_STD_PCMSHORT : I2S PCM short standard
      \arg        I2S_STD_PCMLONG : I2S PCM long standard
    \param[in]  i2s_ckpl:
      \arg        I2S_CKPL_LOW : I2S clock polarity low level
      \arg        I2S_CKPL_HIGH : I2S clock polarity high level
    \param[in]  i2s_frameformat:
      \arg        I2S_FRAMEFORMAT_DT16B_CH16B: I2S data length is 16 bit and channel length is 16 bit
      \arg        I2S_FRAMEFORMAT_DT16B_CH32B: I2S data length is 16 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT24B_CH32B: I2S data length is 24 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT32B_CH32B: I2S data length is 32 bit and channel length is 32 bit
    \param[out] none
    \retval     none
*/
void i2s_full_duplex_mode_config(uint32_t i2s_add_periph, uint32_t i2s_mode, uint32_t i2s_standard,
                                 uint32_t i2s_ckpl, uint32_t i2s_frameformat)
{
    uint32_t reg = 0U, tmp = 0U;

    reg = I2S_ADD_I2SCTL(i2s_add_periph);
    reg &= I2S_FULL_DUPLEX_MASK;

    /* get the mode of the extra I2S module I2Sx_ADD */
    if((I2S_MODE_MASTERTX == i2s_mode) || (I2S_MODE_SLAVETX == i2s_mode)) {
        tmp = I2S_MODE_SLAVERX;
    } else {
        tmp = I2S_MODE_SLAVETX;
    }

    /* enable I2S mode */
    reg |= (uint32_t)SPI_I2SCTL_I2SSEL;
    /* select I2S mode */
    reg |= (uint32_t)tmp;
    /* select I2S standard */
    reg |= (uint32_t)i2s_standard;
    /* select I2S polarity */
    reg |= (uint32_t)i2s_ckpl;
    /* configure data frame format */
    reg |= (uint32_t)i2s_frameformat;

    /* write to SPI_I2SCTL register */
    I2S_ADD_I2SCTL(i2s_add_periph) = (uint32_t)reg;
}

/*!
    \brief      clear SPI/I2S format error flag status
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  flag: SPI/I2S frame format error flag 
      \arg        SPI_FLAG_FERR: only for SPI work in TI mode
      \arg        I2S_FLAG_FERR: for I2S
    \param[out] none
    \retval     none
*/
void spi_i2s_format_error_clear(uint32_t spi_periph, uint32_t flag)
{
    SPI_STAT(spi_periph) = (uint32_t)(~flag);
}

/*!
    \brief      set SPI CRC polynomial
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  crc_poly: CRC polynomial value
    \param[out] none
    \retval     none
*/
void spi_crc_polynomial_set(uint32_t spi_periph, uint16_t crc_poly)
{
    /* set SPI CRC polynomial */
    SPI_CRCPOLY(spi_periph) = (uint32_t)crc_poly;
}

/*!
    \brief      get SPI CRC polynomial
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     16-bit CRC polynomial
*/
uint16_t spi_crc_polynomial_get(uint32_t spi_periph)
{
    return ((uint16_t)SPI_CRCPOLY(spi_periph));
}

/*!
    \brief      turn on SPI CRC function
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_crc_on(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_CRCEN;
}

/*!
    \brief      turn off SPI CRC function
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_crc_off(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_CRCEN);
}

/*!
    \brief      SPI next data is CRC value
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_crc_next(uint32_t spi_periph)
{
    SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_CRCNT;
}

/*!
    \brief      get SPI CRC send value or receive value
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_crc: SPI crc value
                only one parameter can be selected which is shown as below:
      \arg        SPI_CRC_TX: get transmit crc value
      \arg        SPI_CRC_RX: get receive crc value
    \param[out] none
    \retval     16-bit CRC value
*/
uint16_t spi_crc_get(uint32_t spi_periph, uint8_t spi_crc)
{
    uint16_t ret = 0U;

    if(SPI_CRC_TX == spi_crc) {
        ret = ((uint16_t)(SPI_TCRC(spi_periph)));
    } else {
        ret = ((uint16_t)(SPI_RCRC(spi_periph)));
    }
    return ret;
}

/*!
    \brief      clear SPI CRC error flag status
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_crc_error_clear(uint32_t spi_periph)
{
    SPI_STAT(spi_periph) = (uint32_t)(~SPI_FLAG_CRCERR);
}

/*!
    \brief      enable SPI TI mode
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_ti_mode_enable(uint32_t spi_periph)
{
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_TMOD;
}

/*!
    \brief      disable SPI TI mode
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[out] none
    \retval     none
*/
void spi_ti_mode_disable(uint32_t spi_periph)
{
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_TMOD);
}

/*!
    \brief      enable quad wire SPI
    \param[in]  spi_periph: SPIx(only x=5)
    \param[out] none
    \retval     none
*/
void spi_quad_enable(uint32_t spi_periph)
{
    SPI_QCTL(spi_periph) |= (uint32_t)SPI_QCTL_QMOD;
}

/*!
    \brief      disable quad wire SPI
    \param[in]  spi_periph: SPIx(only x=5)
    \param[out] none
    \retval     none
*/
void spi_quad_disable(uint32_t spi_periph)
{
    SPI_QCTL(spi_periph) &= (uint32_t)(~SPI_QCTL_QMOD);
}

/*!
    \brief      enable quad wire SPI write
    \param[in]  spi_periph: SPIx(only x=5)
    \param[out] none
    \retval     none
*/
void spi_quad_write_enable(uint32_t spi_periph)
{
    SPI_QCTL(spi_periph) &= (uint32_t)(~SPI_QCTL_QRD);
}

/*!
    \brief      enable quad wire SPI read
    \param[in]  spi_periph: SPIx(only x=5)
    \param[out] none
    \retval     none
*/
void spi_quad_read_enable(uint32_t spi_periph)
{
    SPI_QCTL(spi_periph) |= (uint32_t)SPI_QCTL_QRD;
}

/*!
    \brief      get SPI and I2S flag status
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_i2s_flag: SPI/I2S flag status
                only one parameter can be selected which are shown as below:
      \arg        SPI_FLAG_TBE: transmit buffer empty flag
      \arg        SPI_FLAG_RBNE: receive buffer not empty flag
      \arg        SPI_FLAG_TRANS: transmit on-going flag
      \arg        SPI_FLAG_RXORERR: receive overrun error flag
      \arg        SPI_FLAG_CONFERR: mode config error flag
      \arg        SPI_FLAG_CRCERR: CRC error flag
      \arg        SPI_FLAG_FERR: format error flag
      \arg        I2S_FLAG_TBE: transmit buffer empty flag
      \arg        I2S_FLAG_RBNE: receive buffer not empty flag
      \arg        I2S_FLAG_TRANS: transmit on-going flag
      \arg        I2S_FLAG_RXORERR: overrun error flag
      \arg        I2S_FLAG_TXURERR: underrun error flag
      \arg        I2S_FLAG_CH: channel side flag
      \arg        I2S_FLAG_FERR: format error flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus spi_i2s_flag_get(uint32_t spi_periph, uint32_t flag)
{
    FlagStatus ret = RESET;

    if(SPI_STAT(spi_periph) & flag) {
        ret = SET;
    } else {
        ret = RESET;
    }
    return ret;
}

/*!
    \brief      enable SPI and I2S interrupt
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_i2s_int: SPI/I2S interrupt
                only one parameter can be selected which is shown as below:
      \arg        SPI_I2S_INT_TBE: transmit buffer empty interrupt
      \arg        SPI_I2S_INT_RBNE: receive buffer not empty interrupt
      \arg        SPI_I2S_INT_ERR: CRC error,configuration error,reception overrun error,
                                   transmission underrun error and format error interrupt
    \param[out] none
    \retval     none
*/
void spi_i2s_interrupt_enable(uint32_t spi_periph, uint8_t interrupt)
{
    switch(interrupt) {
    /* SPI/I2S transmit buffer empty interrupt */
    case SPI_I2S_INT_TBE:
        SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_TBEIE;
        break;
    /* SPI/I2S receive buffer not empty interrupt */
    case SPI_I2S_INT_RBNE:
        SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_RBNEIE;
        break;
    /* SPI/I2S error */
    case SPI_I2S_INT_ERR:
        SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_ERRIE;
        break;
    default:
        break;
    }
}

/*!
    \brief      disable SPI and I2S interrupt
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_i2s_int: SPI/I2S interrupt
                only one parameter can be selected which is shown as below:
      \arg        SPI_I2S_INT_TBE: transmit buffer empty interrupt
      \arg        SPI_I2S_INT_RBNE: receive buffer not empty interrupt
      \arg        SPI_I2S_INT_ERR: CRC error,configuration error,reception overrun error,
                                   transmission underrun error and format error interrupt
    \param[out] none
    \retval     none
*/
void spi_i2s_interrupt_disable(uint32_t spi_periph, uint8_t interrupt)
{
    switch(interrupt) {
    /* SPI/I2S transmit buffer empty interrupt */
    case SPI_I2S_INT_TBE :
        SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_TBEIE);
        break;
    /* SPI/I2S receive buffer not empty interrupt */
    case SPI_I2S_INT_RBNE :
        SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_RBNEIE);
        break;
    /* SPI/I2S error */
    case SPI_I2S_INT_ERR :
        SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_ERRIE);
        break;
    default :
        break;
    }
}

/*!
    \brief      get SPI and I2S interrupt flag status
    \param[in]  spi_periph: SPIx(x=0,1,2,3,4,5)
    \param[in]  spi_i2s_int: SPI/I2S interrupt flag status
                only one parameter can be selected which are shown as below:
      \arg        SPI_I2S_INT_FLAG_TBE: transmit buffer empty interrupt flag
      \arg        SPI_I2S_INT_FLAG_RBNE: receive buffer not empty interrupt flag
      \arg        SPI_I2S_INT_FLAG_RXORERR: overrun interrupt flag
      \arg        SPI_INT_FLAG_CONFERR: config error interrupt flag
      \arg        SPI_INT_FLAG_CRCERR: CRC error interrupt flag
      \arg        I2S_INT_FLAG_TXURERR: underrun error interrupt flag
      \arg        SPI_I2S_INT_FLAG_FERR: format error interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus spi_i2s_interrupt_flag_get(uint32_t spi_periph, uint8_t interrupt)
{
    FlagStatus ret = RESET;
    uint32_t reg1 = SPI_STAT(spi_periph);
    uint32_t reg2 = SPI_CTL1(spi_periph);

    switch(interrupt) {
    /* SPI/I2S transmit buffer empty interrupt */
    case SPI_I2S_INT_FLAG_TBE :
        reg1 = reg1 & SPI_STAT_TBE;
        reg2 = reg2 & SPI_CTL1_TBEIE;
        break;
    /* SPI/I2S receive buffer not empty interrupt */
    case SPI_I2S_INT_FLAG_RBNE :
        reg1 = reg1 & SPI_STAT_RBNE;
        reg2 = reg2 & SPI_CTL1_RBNEIE;
        break;
    /* SPI/I2S overrun interrupt */
    case SPI_I2S_INT_FLAG_RXORERR :
        reg1 = reg1 & SPI_STAT_RXORERR;
        reg2 = reg2 & SPI_CTL1_ERRIE;
        break;
    /* SPI config error interrupt */
    case SPI_INT_FLAG_CONFERR :
        reg1 = reg1 & SPI_STAT_CONFERR;
        reg2 = reg2 & SPI_CTL1_ERRIE;
        break;
    /* SPI CRC error interrupt */
    case SPI_INT_FLAG_CRCERR :
        reg1 = reg1 & SPI_STAT_CRCERR;
        reg2 = reg2 & SPI_CTL1_ERRIE;
        break;
    /* I2S underrun error interrupt */
    case I2S_INT_FLAG_TXURERR :
        reg1 = reg1 & SPI_STAT_TXURERR;
        reg2 = reg2 & SPI_CTL1_ERRIE;
        break;
    /* SPI/I2S format error interrupt */
    case SPI_I2S_INT_FLAG_FERR :
        reg1 = reg1 & SPI_STAT_FERR;
        reg2 = reg2 & SPI_CTL1_ERRIE;
        break;
    default :
        break;
    }
    /*get SPI/I2S interrupt flag status */
    if(reg1 && reg2) {
        ret = SET;
    } else {
        ret = RESET;
    }
    return ret;
}
