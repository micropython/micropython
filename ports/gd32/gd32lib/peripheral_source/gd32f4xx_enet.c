/*!
    \file    gd32f4xx_enet.c
    \brief   ENET driver

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

#include "gd32f4xx_enet.h"

#if defined   (__CC_ARM)                                    /*!< ARM compiler */
__align(4)
enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM];        /*!< ENET RxDMA descriptor */
__align(4)
enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM];        /*!< ENET TxDMA descriptor */
__align(4)
uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];           /*!< ENET receive buffer */
__align(4)
uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];           /*!< ENET transmit buffer */

#elif defined ( __ICCARM__ )                                /*!< IAR compiler */
#pragma data_alignment=4
enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM];        /*!< ENET RxDMA descriptor */
#pragma data_alignment=4
enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM];        /*!< ENET TxDMA descriptor */
#pragma data_alignment=4
uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE];           /*!< ENET receive buffer */
#pragma data_alignment=4
uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE];           /*!< ENET transmit buffer */

#elif defined (__GNUC__)        /* GNU Compiler */
enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM] __attribute__((aligned(4)));          /*!< ENET RxDMA descriptor */
enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM] __attribute__((aligned(4)));          /*!< ENET TxDMA descriptor */
uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE] __attribute__((aligned(4)));             /*!< ENET receive buffer */
uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE] __attribute__((aligned(4)));             /*!< ENET transmit buffer */

#endif /* __CC_ARM */

/* global transmit and receive descriptors pointers */
enet_descriptors_struct  *dma_current_txdesc;
enet_descriptors_struct  *dma_current_rxdesc;

/* structure pointer of ptp descriptor for normal mode */
enet_descriptors_struct  *dma_current_ptp_txdesc = NULL;
enet_descriptors_struct  *dma_current_ptp_rxdesc = NULL;

/* init structure parameters for ENET initialization */
static enet_initpara_struct enet_initpara = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t enet_unknow_err = 0U;
/* array of register offset for debug information get */
static const uint16_t enet_reg_tab[] = {
    0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C, 0x0028, 0x002C, 0x0034,
    0x0038, 0x003C, 0x0040, 0x0044, 0x0048, 0x004C, 0x0050, 0x0054, 0x0058, 0x005C, 0x1080,

    0x0100, 0x0104, 0x0108, 0x010C, 0x0110, 0x014C, 0x0150, 0x0168, 0x0194, 0x0198, 0x01C4,

    0x0700, 0x0704, 0x0708, 0x070C, 0x0710, 0x0714, 0x0718, 0x071C, 0x0720, 0x0728, 0x072C,

    0x1000, 0x1004, 0x1008, 0x100C, 0x1010, 0x1014, 0x1018, 0x101C, 0x1020, 0x1024, 0x1048,
    0x104C, 0x1050, 0x1054
};

/* initialize ENET peripheral with generally concerned parameters, call it by enet_init() */
static void enet_default_init(void);
#ifdef USE_DELAY
/* user can provide more timing precise _ENET_DELAY_ function */
#define _ENET_DELAY_                              delay_ms
#else
/* insert a delay time */
static void enet_delay(uint32_t ncount);
/* default _ENET_DELAY_ function with less precise timing */
#define _ENET_DELAY_                              enet_delay
#endif


/*!
    \brief    deinitialize the ENET, and reset structure parameters for ENET initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_deinit(void)
{
    rcu_periph_reset_enable(RCU_ENETRST);
    rcu_periph_reset_disable(RCU_ENETRST);
    enet_initpara_reset();
}

/*!
    \brief    configure the parameters which are usually less cared for initialization
                note -- this function must be called before enet_init(), otherwise
                configuration will be no effect
    \param[in]  option: different function option, which is related to several parameters, refer to enet_option_enum
                only one parameter can be selected which is shown as below
      \arg        FORWARD_OPTION: choose to configure the frame forward related parameters
      \arg        DMABUS_OPTION: choose to configure the DMA bus mode related parameters
      \arg        DMA_MAXBURST_OPTION: choose to configure the DMA max burst related parameters
      \arg        DMA_ARBITRATION_OPTION: choose to configure the DMA arbitration related parameters
      \arg        STORE_OPTION: choose to configure the store forward mode related parameters
      \arg        DMA_OPTION: choose to configure the DMA descriptor related parameters
      \arg        VLAN_OPTION: choose to configure vlan related parameters
      \arg        FLOWCTL_OPTION: choose to configure flow control related parameters
      \arg        HASHH_OPTION: choose to configure hash high
      \arg        HASHL_OPTION: choose to configure hash low
      \arg        FILTER_OPTION: choose to configure frame filter related parameters
      \arg        HALFDUPLEX_OPTION: choose to configure halfduplex mode related parameters
      \arg        TIMER_OPTION: choose to configure time counter related parameters
      \arg        INTERFRAMEGAP_OPTION: choose to configure the inter frame gap related parameters
    \param[in]  para: the related parameters according to the option
                all the related parameters should be configured which are shown as below
                      FORWARD_OPTION related parameters:
                      -  ENET_AUTO_PADCRC_DROP_ENABLE/ ENET_AUTO_PADCRC_DROP_DISABLE ;
                      -  ENET_TYPEFRAME_CRC_DROP_ENABLE/ ENET_TYPEFRAME_CRC_DROP_DISABLE ;
                      -  ENET_FORWARD_ERRFRAMES_ENABLE/ ENET_FORWARD_ERRFRAMES_DISABLE ;
                      -  ENET_FORWARD_UNDERSZ_GOODFRAMES_ENABLE/ ENET_FORWARD_UNDERSZ_GOODFRAMES_DISABLE .
                      DMABUS_OPTION related parameters:
                      -  ENET_ADDRESS_ALIGN_ENABLE/ ENET_ADDRESS_ALIGN_DISABLE ;
                      -  ENET_FIXED_BURST_ENABLE/ ENET_FIXED_BURST_DISABLE ;
                      -  ENET_MIXED_BURST_ENABLE/ ENET_MIXED_BURST_DISABLE ;
                      DMA_MAXBURST_OPTION related parameters:
                      -  ENET_RXDP_1BEAT/ ENET_RXDP_2BEAT/ ENET_RXDP_4BEAT/
                         ENET_RXDP_8BEAT/ ENET_RXDP_16BEAT/ ENET_RXDP_32BEAT/
                         ENET_RXDP_4xPGBL_4BEAT/ ENET_RXDP_4xPGBL_8BEAT/
                         ENET_RXDP_4xPGBL_16BEAT/ ENET_RXDP_4xPGBL_32BEAT/
                         ENET_RXDP_4xPGBL_64BEAT/ ENET_RXDP_4xPGBL_128BEAT ;
                      -  ENET_PGBL_1BEAT/ ENET_PGBL_2BEAT/ ENET_PGBL_4BEAT/
                         ENET_PGBL_8BEAT/ ENET_PGBL_16BEAT/ ENET_PGBL_32BEAT/
                         ENET_PGBL_4xPGBL_4BEAT/ ENET_PGBL_4xPGBL_8BEAT/
                         ENET_PGBL_4xPGBL_16BEAT/ ENET_PGBL_4xPGBL_32BEAT/
                         ENET_PGBL_4xPGBL_64BEAT/ ENET_PGBL_4xPGBL_128BEAT ;
                      -  ENET_RXTX_DIFFERENT_PGBL/ ENET_RXTX_SAME_PGBL ;
                      DMA_ARBITRATION_OPTION related parameters:
                      -  ENET_ARBITRATION_RXPRIORTX
                      -  ENET_ARBITRATION_RXTX_1_1/ ENET_ARBITRATION_RXTX_2_1/
                         ENET_ARBITRATION_RXTX_3_1/ ENET_ARBITRATION_RXTX_4_1/.
                      STORE_OPTION related parameters:
                      -  ENET_RX_MODE_STOREFORWARD/ ENET_RX_MODE_CUTTHROUGH ;
                      -  ENET_TX_MODE_STOREFORWARD/ ENET_TX_MODE_CUTTHROUGH ;
                      -  ENET_RX_THRESHOLD_64BYTES/ ENET_RX_THRESHOLD_32BYTES/
                         ENET_RX_THRESHOLD_96BYTES/ ENET_RX_THRESHOLD_128BYTES ;
                      -  ENET_TX_THRESHOLD_64BYTES/ ENET_TX_THRESHOLD_128BYTES/
                         ENET_TX_THRESHOLD_192BYTES/ ENET_TX_THRESHOLD_256BYTES/
                         ENET_TX_THRESHOLD_40BYTES/ ENET_TX_THRESHOLD_32BYTES/
                         ENET_TX_THRESHOLD_24BYTES/ ENET_TX_THRESHOLD_16BYTES .
                      DMA_OPTION related parameters:
                      -  ENET_FLUSH_RXFRAME_ENABLE/ ENET_FLUSH_RXFRAME_DISABLE ;
                      -  ENET_SECONDFRAME_OPT_ENABLE/ ENET_SECONDFRAME_OPT_DISABLE ;
                      -  ENET_ENHANCED_DESCRIPTOR/ ENET_NORMAL_DESCRIPTOR .
                      VLAN_OPTION related parameters:
                      -  ENET_VLANTAGCOMPARISON_12BIT/ ENET_VLANTAGCOMPARISON_16BIT ;
                      -  MAC_VLT_VLTI(regval) .
                      FLOWCTL_OPTION related parameters:
                      -  MAC_FCTL_PTM(regval) ;
                      -  ENET_ZERO_QUANTA_PAUSE_ENABLE/ ENET_ZERO_QUANTA_PAUSE_DISABLE ;
                      -  ENET_PAUSETIME_MINUS4/ ENET_PAUSETIME_MINUS28/
                         ENET_PAUSETIME_MINUS144/ENET_PAUSETIME_MINUS256 ;
                      -  ENET_MAC0_AND_UNIQUE_ADDRESS_PAUSEDETECT/ ENET_UNIQUE_PAUSEDETECT ;
                      -  ENET_RX_FLOWCONTROL_ENABLE/ ENET_RX_FLOWCONTROL_DISABLE ;
                      -  ENET_TX_FLOWCONTROL_ENABLE/ ENET_TX_FLOWCONTROL_DISABLE ;
                      -  ENET_ACTIVE_THRESHOLD_256BYTES/ ENET_ACTIVE_THRESHOLD_512BYTES ;
                      -  ENET_ACTIVE_THRESHOLD_768BYTES/ ENET_ACTIVE_THRESHOLD_1024BYTES ;
                      -  ENET_ACTIVE_THRESHOLD_1280BYTES/ ENET_ACTIVE_THRESHOLD_1536BYTES ;
                      -  ENET_ACTIVE_THRESHOLD_1792BYTES ;
                      -  ENET_DEACTIVE_THRESHOLD_256BYTES/ ENET_DEACTIVE_THRESHOLD_512BYTES ;
                      -  ENET_DEACTIVE_THRESHOLD_768BYTES/ ENET_DEACTIVE_THRESHOLD_1024BYTES ;
                      -  ENET_DEACTIVE_THRESHOLD_1280BYTES/ ENET_DEACTIVE_THRESHOLD_1536BYTES ;
                      -  ENET_DEACTIVE_THRESHOLD_1792BYTES .
                      HASHH_OPTION related parameters:
                      -  0x0~0xFFFF FFFFU
                      HASHL_OPTION related parameters:
                      -  0x0~0xFFFF FFFFU
                      FILTER_OPTION related parameters:
                      -  ENET_SRC_FILTER_NORMAL_ENABLE/ ENET_SRC_FILTER_INVERSE_ENABLE/
                         ENET_SRC_FILTER_DISABLE ;
                      -  ENET_DEST_FILTER_INVERSE_ENABLE/ ENET_DEST_FILTER_INVERSE_DISABLE ;
                      -  ENET_MULTICAST_FILTER_HASH_OR_PERFECT/ ENET_MULTICAST_FILTER_HASH/
                         ENET_MULTICAST_FILTER_PERFECT/ ENET_MULTICAST_FILTER_NONE ;
                      -  ENET_UNICAST_FILTER_EITHER/ ENET_UNICAST_FILTER_HASH/
                         ENET_UNICAST_FILTER_PERFECT ;
                      -  ENET_PCFRM_PREVENT_ALL/ ENET_PCFRM_PREVENT_PAUSEFRAME/
                         ENET_PCFRM_FORWARD_ALL/ ENET_PCFRM_FORWARD_FILTERED .
                      HALFDUPLEX_OPTION related parameters:
                      -  ENET_CARRIERSENSE_ENABLE/ ENET_CARRIERSENSE_DISABLE ;
                      -  ENET_RECEIVEOWN_ENABLE/ ENET_RECEIVEOWN_DISABLE ;
                      -  ENET_RETRYTRANSMISSION_ENABLE/ ENET_RETRYTRANSMISSION_DISABLE ;
                      -  ENET_BACKOFFLIMIT_10/ ENET_BACKOFFLIMIT_8/
                         ENET_BACKOFFLIMIT_4/ ENET_BACKOFFLIMIT_1 ;
                      -  ENET_DEFERRALCHECK_ENABLE/ ENET_DEFERRALCHECK_DISABLE .
                      TIMER_OPTION related parameters:
                      -  ENET_WATCHDOG_ENABLE/ ENET_WATCHDOG_DISABLE ;
                      -  ENET_JABBER_ENABLE/ ENET_JABBER_DISABLE ;
                      INTERFRAMEGAP_OPTION related parameters:
                      -  ENET_INTERFRAMEGAP_96BIT/ ENET_INTERFRAMEGAP_88BIT/
                         ENET_INTERFRAMEGAP_80BIT/ ENET_INTERFRAMEGAP_72BIT/
                         ENET_INTERFRAMEGAP_64BIT/ ENET_INTERFRAMEGAP_56BIT/
                         ENET_INTERFRAMEGAP_48BIT/ ENET_INTERFRAMEGAP_40BIT .
    \param[out] none
    \retval     none
*/
void enet_initpara_config(enet_option_enum option, uint32_t para)
{
    switch(option) {
    case FORWARD_OPTION:
        /* choose to configure forward_frame, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)FORWARD_OPTION;
        enet_initpara.forward_frame = para;
        break;
    case DMABUS_OPTION:
        /* choose to configure dmabus_mode, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)DMABUS_OPTION;
        enet_initpara.dmabus_mode = para;
        break;
    case DMA_MAXBURST_OPTION:
        /* choose to configure dma_maxburst, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)DMA_MAXBURST_OPTION;
        enet_initpara.dma_maxburst = para;
        break;
    case DMA_ARBITRATION_OPTION:
        /* choose to configure dma_arbitration, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)DMA_ARBITRATION_OPTION;
        enet_initpara.dma_arbitration = para;
        break;
    case STORE_OPTION:
        /* choose to configure store_forward_mode, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)STORE_OPTION;
        enet_initpara.store_forward_mode = para;
        break;
    case DMA_OPTION:
        /* choose to configure dma_function, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)DMA_OPTION;

#ifndef SELECT_DESCRIPTORS_ENHANCED_MODE
        para &= ~ENET_ENHANCED_DESCRIPTOR;
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

        enet_initpara.dma_function = para;
        break;
    case VLAN_OPTION:
        /* choose to configure vlan_config, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)VLAN_OPTION;
        enet_initpara.vlan_config = para;
        break;
    case FLOWCTL_OPTION:
        /* choose to configure flow_control, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)FLOWCTL_OPTION;
        enet_initpara.flow_control = para;
        break;
    case HASHH_OPTION:
        /* choose to configure hashtable_high, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)HASHH_OPTION;
        enet_initpara.hashtable_high = para;
        break;
    case HASHL_OPTION:
        /* choose to configure hashtable_low, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)HASHL_OPTION;
        enet_initpara.hashtable_low = para;
        break;
    case FILTER_OPTION:
        /* choose to configure framesfilter_mode, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)FILTER_OPTION;
        enet_initpara.framesfilter_mode = para;
        break;
    case HALFDUPLEX_OPTION:
        /* choose to configure halfduplex_param, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)HALFDUPLEX_OPTION;
        enet_initpara.halfduplex_param = para;
        break;
    case TIMER_OPTION:
        /* choose to configure timer_config, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)TIMER_OPTION;
        enet_initpara.timer_config = para;
        break;
    case INTERFRAMEGAP_OPTION:
        /* choose to configure interframegap, and save the configuration parameters */
        enet_initpara.option_enable |= (uint32_t)INTERFRAMEGAP_OPTION;
        enet_initpara.interframegap = para;
        break;
    default:
        break;
    }
}

/*!
    \brief    initialize ENET peripheral with generally concerned parameters and the less cared
                parameters
    \param[in]  mediamode: PHY mode and mac loopback configurations, refer to enet_mediamode_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_AUTO_NEGOTIATION: PHY auto negotiation
      \arg        ENET_100M_FULLDUPLEX: 100Mbit/s, full-duplex
      \arg        ENET_100M_HALFDUPLEX: 100Mbit/s, half-duplex
      \arg        ENET_10M_FULLDUPLEX: 10Mbit/s, full-duplex
      \arg        ENET_10M_HALFDUPLEX: 10Mbit/s, half-duplex
      \arg        ENET_LOOPBACKMODE: MAC in loopback mode at the MII
    \param[in]  checksum: IP frame checksum offload function, refer to enet_mediamode_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_NO_AUTOCHECKSUM: disable IP frame checksum function
      \arg        ENET_AUTOCHECKSUM_DROP_FAILFRAMES: enable IP frame checksum function
      \arg        ENET_AUTOCHECKSUM_ACCEPT_FAILFRAMES: enable IP frame checksum function, and the received frame
                                                       with only payload error but no other errors will not be dropped
    \param[in]  recept: frame filter function, refer to enet_frmrecept_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_PROMISCUOUS_MODE: promiscuous mode enabled
      \arg        ENET_RECEIVEALL: all received frame are forwarded to application
      \arg        ENET_BROADCAST_FRAMES_PASS: the address filters pass all received broadcast frames
      \arg        ENET_BROADCAST_FRAMES_DROP: the address filters filter all incoming broadcast frames
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_init(enet_mediamode_enum mediamode, enet_chksumconf_enum checksum, enet_frmrecept_enum recept)
{
    uint32_t reg_value = 0U, reg_temp = 0U, temp = 0U;
    uint32_t media_temp = 0U;
    uint32_t timeout = 0U;
    uint16_t phy_value = 0U;
    ErrStatus phy_state = ERROR, enet_state = ERROR;

    /* PHY interface configuration, configure SMI clock and reset PHY chip */
    if(ERROR == enet_phy_config()) {
        _ENET_DELAY_(PHY_RESETDELAY);
        if(ERROR == enet_phy_config()) {
            return enet_state;
        }
    }
    /* initialize ENET peripheral with generally concerned parameters */
    enet_default_init();

    /* 1st, configure mediamode */
    media_temp = (uint32_t)mediamode;
    /* if is PHY auto negotiation */
    if((uint32_t)ENET_AUTO_NEGOTIATION == media_temp) {
        /* wait for PHY_LINKED_STATUS bit be set */
        do {
            enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
            phy_value &= PHY_LINKED_STATUS;
            timeout++;
        } while((RESET == phy_value) && (timeout < PHY_READ_TO));
        /* return ERROR due to timeout */
        if(PHY_READ_TO == timeout) {
            return enet_state;
        }
        /* reset timeout counter */
        timeout = 0U;

        /* enable auto-negotiation */
        phy_value = PHY_AUTONEGOTIATION;
        phy_state = enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
        if(!phy_state) {
            /* return ERROR due to write timeout */
            return enet_state;
        }

        /* wait for the PHY_AUTONEGO_COMPLETE bit be set */
        do {
            enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
            phy_value &= PHY_AUTONEGO_COMPLETE;
            timeout++;
        } while((RESET == phy_value) && (timeout < (uint32_t)PHY_READ_TO));
        /* return ERROR due to timeout */
        if(PHY_READ_TO == timeout) {
            return enet_state;
        }
        /* reset timeout counter */
        timeout = 0U;

        /* read the result of the auto-negotiation */
        enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_SR, &phy_value);

#if(PHY_TYPE == SR8201F || PHY_TYPE == YT8512)
        /* configure the duplex mode of MAC following the auto-negotiation result */
        if((uint16_t)RESET != (phy_value & PHY_DUPLEX_STATUS)) {
            media_temp = ENET_MODE_FULLDUPLEX;
        } else {
            media_temp = ENET_MODE_HALFDUPLEX;
        }
        /* configure the communication speed of MAC following the auto-negotiation result */
        if((uint16_t)RESET !=(phy_value & PHY_SPEED_STATUS)){
            media_temp |= ENET_SPEEDMODE_100M;
        }else{
            media_temp |= ENET_SPEEDMODE_10M;
        }
#elif (PHY_TYPE == DP83848 || PHY_TYPE == LAN8700)
        /* configure the duplex mode of MAC following the auto-negotiation result */
        if((uint16_t)RESET != (phy_value & PHY_DUPLEX_STATUS)) {
            media_temp = ENET_MODE_FULLDUPLEX;
        } else {
            media_temp = ENET_MODE_HALFDUPLEX;
        }
        /* configure the communication speed of MAC following the auto-negotiation result */
        if((uint16_t)RESET !=(phy_value & PHY_SPEED_STATUS)){
            media_temp |= ENET_SPEEDMODE_10M;
        }else{
            media_temp |= ENET_SPEEDMODE_100M;
        }
#endif /* PHY_TYPE == SR8201F || PHY_TYPE == YT8512 */
    } else {
        phy_value = (uint16_t)((media_temp & ENET_MAC_CFG_DPM) >> 3);
        phy_value |= (uint16_t)((media_temp & ENET_MAC_CFG_SPD) >> 1);
        phy_state = enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
        if(!phy_state) {
            /* return ERROR due to write timeout */
            return enet_state;
        }
        /* PHY configuration need some time */
        _ENET_DELAY_(PHY_CONFIGDELAY);
    }
    /* after configuring the PHY, use mediamode to configure registers */
    reg_value = ENET_MAC_CFG;
    /* configure ENET_MAC_CFG register */
    reg_value &= (~(ENET_MAC_CFG_SPD | ENET_MAC_CFG_DPM | ENET_MAC_CFG_LBM));
    reg_value |= media_temp;
    ENET_MAC_CFG = reg_value;


    /* 2st, configure checksum */
    if(RESET != ((uint32_t)checksum & ENET_CHECKSUMOFFLOAD_ENABLE)) {
        ENET_MAC_CFG |= ENET_CHECKSUMOFFLOAD_ENABLE;

        reg_value = ENET_DMA_CTL;
        /* configure ENET_DMA_CTL register */
        reg_value &= ~ENET_DMA_CTL_DTCERFD;
        reg_value |= ((uint32_t)checksum & ENET_DMA_CTL_DTCERFD);
        ENET_DMA_CTL = reg_value;
    }

    /* 3rd, configure recept */
    ENET_MAC_FRMF |= (uint32_t)recept;

    /* 4th, configure different function options */
    /* configure forward_frame related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)FORWARD_OPTION)) {
        reg_temp = enet_initpara.forward_frame;

        reg_value = ENET_MAC_CFG;
        temp = reg_temp;
        /* configure ENET_MAC_CFG register */
        reg_value &= (~(ENET_MAC_CFG_TFCD | ENET_MAC_CFG_APCD));
        temp &= (ENET_MAC_CFG_TFCD | ENET_MAC_CFG_APCD);
        reg_value |= temp;
        ENET_MAC_CFG = reg_value;

        reg_value = ENET_DMA_CTL;
        temp = reg_temp;
        /* configure ENET_DMA_CTL register */
        reg_value &= (~(ENET_DMA_CTL_FERF | ENET_DMA_CTL_FUF));
        temp &= ((ENET_DMA_CTL_FERF | ENET_DMA_CTL_FUF) << 2);
        reg_value |= (temp >> 2);
        ENET_DMA_CTL = reg_value;
    }

    /* configure dmabus_mode related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)DMABUS_OPTION)) {
        temp = enet_initpara.dmabus_mode;

        reg_value = ENET_DMA_BCTL;
        /* configure ENET_DMA_BCTL register */
        reg_value &= ~(ENET_DMA_BCTL_AA | ENET_DMA_BCTL_FB \
                       | ENET_DMA_BCTL_FPBL | ENET_DMA_BCTL_MB);
        reg_value |= temp;
        ENET_DMA_BCTL = reg_value;
    }

    /* configure dma_maxburst related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)DMA_MAXBURST_OPTION)) {
        temp = enet_initpara.dma_maxburst;

        reg_value = ENET_DMA_BCTL;
        /* configure ENET_DMA_BCTL register */
        reg_value &= ~(ENET_DMA_BCTL_RXDP | ENET_DMA_BCTL_PGBL | ENET_DMA_BCTL_UIP);
        reg_value |= temp;
        ENET_DMA_BCTL = reg_value;
    }

    /* configure dma_arbitration related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)DMA_ARBITRATION_OPTION)) {
        temp = enet_initpara.dma_arbitration;

        reg_value = ENET_DMA_BCTL;
        /* configure ENET_DMA_BCTL register */
        reg_value &= ~(ENET_DMA_BCTL_RTPR | ENET_DMA_BCTL_DAB);
        reg_value |= temp;
        ENET_DMA_BCTL = reg_value;
    }

    /* configure store_forward_mode related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)STORE_OPTION)) {
        temp = enet_initpara.store_forward_mode;

        reg_value = ENET_DMA_CTL;
        /* configure ENET_DMA_CTL register */
        reg_value &= ~(ENET_DMA_CTL_RSFD | ENET_DMA_CTL_TSFD | ENET_DMA_CTL_RTHC | ENET_DMA_CTL_TTHC);
        reg_value |= temp;
        ENET_DMA_CTL = reg_value;
    }

    /* configure dma_function related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)DMA_OPTION)) {
        reg_temp = enet_initpara.dma_function;

        reg_value = ENET_DMA_CTL;
        temp = reg_temp;
        /* configure ENET_DMA_CTL register */
        reg_value &= (~(ENET_DMA_CTL_DAFRF | ENET_DMA_CTL_OSF));
        temp &= (ENET_DMA_CTL_DAFRF | ENET_DMA_CTL_OSF);
        reg_value |= temp;
        ENET_DMA_CTL = reg_value;

        reg_value = ENET_DMA_BCTL;
        temp = reg_temp;
        /* configure ENET_DMA_BCTL register */
        reg_value &= (~ENET_DMA_BCTL_DFM);
        temp &= ENET_DMA_BCTL_DFM;
        reg_value |= temp;
        ENET_DMA_BCTL = reg_value;
    }

    /* configure vlan_config related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)VLAN_OPTION)) {
        reg_temp = enet_initpara.vlan_config;

        reg_value = ENET_MAC_VLT;
        /* configure ENET_MAC_VLT register */
        reg_value &= ~(ENET_MAC_VLT_VLTI | ENET_MAC_VLT_VLTC);
        reg_value |= reg_temp;
        ENET_MAC_VLT = reg_value;
    }

    /* configure flow_control related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)FLOWCTL_OPTION)) {
        reg_temp = enet_initpara.flow_control;

        reg_value = ENET_MAC_FCTL;
        temp = reg_temp;
        /* configure ENET_MAC_FCTL register */
        reg_value &= ~(ENET_MAC_FCTL_PTM | ENET_MAC_FCTL_DZQP | ENET_MAC_FCTL_PLTS \
                       | ENET_MAC_FCTL_UPFDT | ENET_MAC_FCTL_RFCEN | ENET_MAC_FCTL_TFCEN);
        temp &= (ENET_MAC_FCTL_PTM | ENET_MAC_FCTL_DZQP | ENET_MAC_FCTL_PLTS \
                 | ENET_MAC_FCTL_UPFDT | ENET_MAC_FCTL_RFCEN | ENET_MAC_FCTL_TFCEN);
        reg_value |= temp;
        ENET_MAC_FCTL = reg_value;

        reg_value = ENET_MAC_FCTH;
        temp = reg_temp;
        /* configure ENET_MAC_FCTH register */
        reg_value &= ~(ENET_MAC_FCTH_RFA | ENET_MAC_FCTH_RFD);
        temp &= ((ENET_MAC_FCTH_RFA | ENET_MAC_FCTH_RFD) << 8);
        reg_value |= (temp >> 8);
        ENET_MAC_FCTH = reg_value;
    }

    /* configure hashtable_high related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)HASHH_OPTION)) {
        ENET_MAC_HLH = enet_initpara.hashtable_high;
    }

    /* configure hashtable_low related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)HASHL_OPTION)) {
        ENET_MAC_HLL = enet_initpara.hashtable_low;
    }

    /* configure framesfilter_mode related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)FILTER_OPTION)) {
        reg_temp = enet_initpara.framesfilter_mode;

        reg_value = ENET_MAC_FRMF;
        /* configure ENET_MAC_FRMF register */
        reg_value &= ~(ENET_MAC_FRMF_SAFLT | ENET_MAC_FRMF_SAIFLT | ENET_MAC_FRMF_DAIFLT \
                       | ENET_MAC_FRMF_HMF | ENET_MAC_FRMF_HPFLT | ENET_MAC_FRMF_MFD \
                       | ENET_MAC_FRMF_HUF | ENET_MAC_FRMF_PCFRM);
        reg_value |= reg_temp;
        ENET_MAC_FRMF = reg_value;
    }

    /* configure halfduplex_param related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)HALFDUPLEX_OPTION)) {
        reg_temp = enet_initpara.halfduplex_param;

        reg_value = ENET_MAC_CFG;
        /* configure ENET_MAC_CFG register */
        reg_value &= ~(ENET_MAC_CFG_CSD | ENET_MAC_CFG_ROD | ENET_MAC_CFG_RTD \
                       | ENET_MAC_CFG_BOL | ENET_MAC_CFG_DFC);
        reg_value |= reg_temp;
        ENET_MAC_CFG = reg_value;
    }

    /* configure timer_config related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)TIMER_OPTION)) {
        reg_temp = enet_initpara.timer_config;

        reg_value = ENET_MAC_CFG;
        /* configure ENET_MAC_CFG register */
        reg_value &= ~(ENET_MAC_CFG_WDD | ENET_MAC_CFG_JBD);
        reg_value |= reg_temp;
        ENET_MAC_CFG = reg_value;
    }

    /* configure interframegap related registers */
    if(RESET != (enet_initpara.option_enable & (uint32_t)INTERFRAMEGAP_OPTION)) {
        reg_temp = enet_initpara.interframegap;

        reg_value = ENET_MAC_CFG;
        /* configure ENET_MAC_CFG register */
        reg_value &= ~ENET_MAC_CFG_IGBS;
        reg_value |= reg_temp;
        ENET_MAC_CFG = reg_value;
    }

    enet_state = SUCCESS;
    return enet_state;
}

/*!
    \brief    reset all core internal registers located in CLK_TX and CLK_RX
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_software_reset(void)
{
    uint32_t timeout = 0U;
    ErrStatus enet_state = ERROR;
    uint32_t dma_flag;

    /* reset all core internal registers located in CLK_TX and CLK_RX */
    ENET_DMA_BCTL |= ENET_DMA_BCTL_SWR;

    /* wait for reset operation complete */
    do {
        dma_flag = (ENET_DMA_BCTL & ENET_DMA_BCTL_SWR);
        timeout++;
    } while((RESET != dma_flag) && (ENET_DELAY_TO != timeout));

    /* reset operation complete */
    if(RESET == (ENET_DMA_BCTL & ENET_DMA_BCTL_SWR)) {
        enet_state = SUCCESS;
    }

    return enet_state;
}

/*!
    \brief    check receive frame valid and return frame size
    \param[in]  none
    \param[out] none
    \retval     size of received frame: 0x0 - 0x3FFF
*/
uint32_t enet_rxframe_size_get(void)
{
    uint32_t size = 0U;
    uint32_t status;

    /* get rdes0 information of current RxDMA descriptor */
    status = dma_current_rxdesc->status;

    /* if the desciptor is owned by DMA */
    if((uint32_t)RESET != (status & ENET_RDES0_DAV)) {
        return 0U;
    }

    /* if has any error, or the frame uses two or more descriptors */
    if((((uint32_t)RESET) != (status & ENET_RDES0_ERRS)) ||
            (((uint32_t)RESET) == (status & ENET_RDES0_LDES)) ||
            (((uint32_t)RESET) == (status & ENET_RDES0_FDES))) {
        /* drop current receive frame */
        enet_rxframe_drop();

        return 1U;
    }
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    /* if is an ethernet-type frame, and IP frame payload error occurred */
    if(((uint32_t)RESET) != (dma_current_rxdesc->status & ENET_RDES0_FRMT) &&
            ((uint32_t)RESET) != (dma_current_rxdesc->extended_status & ENET_RDES4_IPPLDERR)) {
        /* drop current receive frame */
        enet_rxframe_drop();

        return 1U;
    }
#else
    /* if is an ethernet-type frame, and IP frame payload error occurred */
    if((((uint32_t)RESET) != (status & ENET_RDES0_FRMT)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_PCERR))) {
        /* drop current receive frame */
        enet_rxframe_drop();

        return 1U;
    }
#endif
    /* if CPU owns current descriptor, no error occured, the frame uses only one descriptor */
    if((((uint32_t)RESET) == (status & ENET_RDES0_DAV)) &&
            (((uint32_t)RESET) == (status & ENET_RDES0_ERRS)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_LDES)) &&
            (((uint32_t)RESET) != (status & ENET_RDES0_FDES))) {
        /* get the size of the received data including CRC */
        size = GET_RDES0_FRML(status);
        /* substract the CRC size */
        size = size - 4U;

        /* if is a type frame, and CRC is not included in forwarding frame */
        if((RESET != (ENET_MAC_CFG & ENET_MAC_CFG_TFCD)) && (RESET != (status & ENET_RDES0_FRMT))) {
            size = size + 4U;
        }
    } else {
        enet_unknow_err++;
        enet_rxframe_drop();

        return 1U;
    }

    /* return packet size */
    return size;
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in chain mode
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_descriptors_chain_init(enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc, *desc_tab;
    uint8_t *buf;

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* select chain mode */
        desc_status = ENET_TDES0_TCHM;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* select receive chained mode and set buffer1 size */
        desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
    }
    dma_current_ptp_rxdesc = NULL;
    dma_current_ptp_txdesc = NULL;

    /* configure each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* if is not the last descriptor */
        if(num < (count - 1U)) {
            /* configure the next descriptor address */
            desc->buffer2_next_desc_addr = (uint32_t)(desc_tab + num + 1U);
        } else {
            /* when it is the last descriptor, the next descriptor address
            equals to first descriptor address in descriptor table */
            desc->buffer2_next_desc_addr = (uint32_t) desc_tab;
        }
    }
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in ring mode
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_descriptors_ring_init(enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc;
    enet_descriptors_struct *desc_tab;
    uint8_t *buf;

    /* configure descriptor skip length */
    ENET_DMA_BCTL &= ~ENET_DMA_BCTL_DPSL;
    ENET_DMA_BCTL |= DMA_BCTL_DPSL(0);

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* set buffer1 size */
        desc_bufsize = ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
    }
    dma_current_ptp_rxdesc = NULL;
    dma_current_ptp_txdesc = NULL;

    /* configure each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* when it is the last descriptor */
        if(num == (count - 1U)) {
            if(ENET_DMA_TX == direction) {
                /* configure transmit end of ring mode */
                desc->status |= ENET_TDES0_TERM;
            } else {
                /* configure receive end of ring mode */
                desc->control_buffer_size |= ENET_RDES1_RERM;
            }
        }
    }
}

/*!
    \brief    handle current received frame data to application buffer
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] buffer: pointer to the received frame data
                note -- if the input is NULL, user should copy data in application by himself
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_frame_receive(uint8_t *buffer, uint32_t bufsize)
{
    uint32_t offset = 0U, size = 0U;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_DAV)) {
        return ERROR;
    }


    /* if buffer pointer is null, indicates that users has copied data in application */
    if(NULL != buffer) {
        /* if no error occurs, and the frame uses only one descriptor */
        if((((uint32_t)RESET) == (dma_current_rxdesc->status & ENET_RDES0_ERRS)) &&
                (((uint32_t)RESET) != (dma_current_rxdesc->status & ENET_RDES0_LDES)) &&
                (((uint32_t)RESET) != (dma_current_rxdesc->status & ENET_RDES0_FDES))) {
            /* get the frame length except CRC */
            size = GET_RDES0_FRML(dma_current_rxdesc->status);
            size = size - 4U;

            /* if is a type frame, and CRC is not included in forwarding frame */
            if((RESET != (ENET_MAC_CFG & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc->status & ENET_RDES0_FRMT))) {
                size = size + 4U;
            }

            /* to avoid situation that the frame size exceeds the buffer length */
            if(size > bufsize) {
                return ERROR;
            }

            /* copy data from Rx buffer to application buffer */
            for(offset = 0U; offset < size; offset++) {
                (*(buffer + offset)) = (*(__IO uint8_t *)(uint32_t)((dma_current_rxdesc->buffer1_addr) + offset));
            }

        } else {
            /* return ERROR */
            return ERROR;
        }
    }
    /* enable reception, descriptor is owned by DMA */
    dma_current_rxdesc->status = ENET_RDES0_DAV;

    /* check Rx buffer unavailable flag status */
    if((uint32_t)RESET != (ENET_DMA_STAT & ENET_DMA_STAT_RBU)) {
        /* clear RBU flag */
        ENET_DMA_STAT = ENET_DMA_STAT_RBU;
        /* resume DMA reception by writing to the RPEN register*/
        ENET_DMA_RPEN = 0U;
    }

    /* update the current RxDMA descriptor pointer to the next decriptor in RxDMA decriptor table */
    /* chained mode */
    if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RCHM)) {
        dma_current_rxdesc = (enet_descriptors_struct *)(dma_current_rxdesc->buffer2_next_desc_addr);
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_rxdesc = (enet_descriptors_struct *)(ENET_DMA_RDTADDR);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_rxdesc = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc + ETH_DMARXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL)));
        }
    }

    return SUCCESS;
}

/*!
    \brief    handle application buffer data to transmit it
    \param[in]  buffer: pointer to the frame data to be transmitted,
                note -- if the input is NULL, user should handle the data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_frame_transmit(uint8_t *buffer, uint32_t length)
{
    uint32_t offset = 0U;
    uint32_t dma_tbu_flag, dma_tu_flag;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_DAV)) {
        return ERROR;
    }

    /* only frame length no more than ENET_MAX_FRAME_SIZE is allowed */
    if(length > ENET_MAX_FRAME_SIZE) {
        return ERROR;
    }

    /* if buffer pointer is null, indicates that users has handled data in application */
    if(NULL != buffer) {
        /* copy frame data from application buffer to Tx buffer */
        for(offset = 0U; offset < length; offset++) {
            (*(__IO uint8_t *)(uint32_t)((dma_current_txdesc->buffer1_addr) + offset)) = (*(buffer + offset));
        }
    }

    /* set the frame length */
    dma_current_txdesc->control_buffer_size = length;
    /* set the segment of frame, frame is transmitted in one descriptor */
    dma_current_txdesc->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
    /* enable the DMA transmission */
    dma_current_txdesc->status |= ENET_TDES0_DAV;

    /* check Tx buffer unavailable flag status */
    dma_tbu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TBU);
    dma_tu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TU);

    if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
        /* clear TBU and TU flag */
        ENET_DMA_STAT = (dma_tbu_flag | dma_tu_flag);
        /* resume DMA transmission by writing to the TPEN register*/
        ENET_DMA_TPEN = 0U;
    }

    /* update the current TxDMA descriptor pointer to the next decriptor in TxDMA decriptor table*/
    /* chained mode */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TCHM)) {
        dma_current_txdesc = (enet_descriptors_struct *)(dma_current_txdesc->buffer2_next_desc_addr);
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_txdesc = (enet_descriptors_struct *)(ENET_DMA_TDTADDR);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_txdesc = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc + ETH_DMATXDESC_SIZE + (GET_DMA_BCTL_DPSL(ENET_DMA_BCTL)));
        }
    }

    return SUCCESS;
}

/*!
    \brief    configure the transmit IP frame checksum offload calculation and insertion
    \param[in]  desc: the descriptor pointer which users want to configure, refer to enet_descriptors_struct
    \param[in]  checksum: IP frame checksum configuration
                only one parameter can be selected which is shown as below
      \arg        ENET_CHECKSUM_DISABLE: checksum insertion disabled
      \arg        ENET_CHECKSUM_IPV4HEADER: only IP header checksum calculation and insertion are enabled
      \arg        ENET_CHECKSUM_TCPUDPICMP_SEGMENT: TCP/UDP/ICMP checksum insertion calculated but pseudo-header
      \arg        ENET_CHECKSUM_TCPUDPICMP_FULL: TCP/UDP/ICMP checksum insertion fully calculated
    \param[out] none
    \retval     none
*/
void enet_transmit_checksum_config(enet_descriptors_struct *desc, uint32_t checksum)
{
    desc->status &= ~ENET_TDES0_CM;
    desc->status |= checksum;
}

/*!
    \brief    ENET Tx and Rx function enable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_enable(void)
{
    enet_tx_enable();
    enet_rx_enable();
}

/*!
    \brief    ENET Tx and Rx function disable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_disable(void)
{
    enet_tx_disable();
    enet_rx_disable();
}

/*!
    \brief    configure MAC address
    \param[in]  mac_addr: select which MAC address will be set, refer to enet_macaddress_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_ADDRESS0: set MAC address 0 filter
      \arg        ENET_MAC_ADDRESS1: set MAC address 1 filter
      \arg        ENET_MAC_ADDRESS2: set MAC address 2 filter
      \arg        ENET_MAC_ADDRESS3: set MAC address 3 filter
    \param[in]  paddr: the buffer pointer which stores the MAC address
                  (little-ending store, such as MAC address is aa:bb:cc:dd:ee:22, the buffer is {22, ee, dd, cc, bb, aa})
    \param[out] none
    \retval     none
*/
void enet_mac_address_set(enet_macaddress_enum mac_addr, uint8_t paddr[])
{
    REG32(ENET_ADDRH_BASE + (uint32_t)mac_addr) = ENET_SET_MACADDRH(paddr);
    REG32(ENET_ADDRL_BASE + (uint32_t)mac_addr) = ENET_SET_MACADDRL(paddr);
}

/*!
    \brief    get MAC address
    \param[in]  mac_addr: select which MAC address will be get, refer to enet_macaddress_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_ADDRESS0: get MAC address 0 filter
      \arg        ENET_MAC_ADDRESS1: get MAC address 1 filter
      \arg        ENET_MAC_ADDRESS2: get MAC address 2 filter
      \arg        ENET_MAC_ADDRESS3: get MAC address 3 filter
    \param[out] paddr: the buffer pointer which is stored the MAC address
                  (little-ending store, such as mac address is aa:bb:cc:dd:ee:22, the buffer is {22, ee, dd, cc, bb, aa})
    \retval     none
*/
void enet_mac_address_get(enet_macaddress_enum mac_addr, uint8_t paddr[])
{
    paddr[0] = ENET_GET_MACADDR(mac_addr, 0U);
    paddr[1] = ENET_GET_MACADDR(mac_addr, 1U);
    paddr[2] = ENET_GET_MACADDR(mac_addr, 2U);
    paddr[3] = ENET_GET_MACADDR(mac_addr, 3U);
    paddr[4] = ENET_GET_MACADDR(mac_addr, 4U);
    paddr[5] = ENET_GET_MACADDR(mac_addr, 5U);
}

/*!
    \brief    get the ENET MAC/MSC/PTP/DMA status flag
    \param[in]  enet_flag: ENET status flag, refer to enet_flag_enum,
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_FLAG_MPKR: magic packet received flag
      \arg        ENET_MAC_FLAG_WUFR: wakeup frame received flag
      \arg        ENET_MAC_FLAG_FLOWCONTROL: flow control status flag
      \arg        ENET_MAC_FLAG_WUM: WUM status flag
      \arg        ENET_MAC_FLAG_MSC: MSC status flag
      \arg        ENET_MAC_FLAG_MSCR: MSC receive status flag
      \arg        ENET_MAC_FLAG_MSCT: MSC transmit status flag
      \arg        ENET_MAC_FLAG_TMST: time stamp trigger status flag
      \arg        ENET_PTP_FLAG_TSSCO: timestamp second counter overflow flag
      \arg        ENET_PTP_FLAG_TTM: target time match flag
      \arg        ENET_MSC_FLAG_RFCE: received frames CRC error flag
      \arg        ENET_MSC_FLAG_RFAE: received frames alignment error flag
      \arg        ENET_MSC_FLAG_RGUF: received good unicast frames flag
      \arg        ENET_MSC_FLAG_TGFSC: transmitted good frames single collision flag
      \arg        ENET_MSC_FLAG_TGFMSC: transmitted good frames more single collision flag
      \arg        ENET_MSC_FLAG_TGF: transmitted good frames flag
      \arg        ENET_DMA_FLAG_TS: transmit status flag
      \arg        ENET_DMA_FLAG_TPS: transmit process stopped status flag
      \arg        ENET_DMA_FLAG_TBU: transmit buffer unavailable status flag
      \arg        ENET_DMA_FLAG_TJT: transmit jabber timeout status flag
      \arg        ENET_DMA_FLAG_RO: receive overflow status flag
      \arg        ENET_DMA_FLAG_TU: transmit underflow status flag
      \arg        ENET_DMA_FLAG_RS: receive status flag
      \arg        ENET_DMA_FLAG_RBU: receive buffer unavailable status flag
      \arg        ENET_DMA_FLAG_RPS: receive process stopped status flag
      \arg        ENET_DMA_FLAG_RWT: receive watchdog timeout status flag
      \arg        ENET_DMA_FLAG_ET: early transmit status flag
      \arg        ENET_DMA_FLAG_FBE: fatal bus error status flag
      \arg        ENET_DMA_FLAG_ER: early receive status flag
      \arg        ENET_DMA_FLAG_AI: abnormal interrupt summary flag
      \arg        ENET_DMA_FLAG_NI: normal interrupt summary flag
      \arg        ENET_DMA_FLAG_EB_DMA_ERROR: DMA error flag
      \arg        ENET_DMA_FLAG_EB_TRANSFER_ERROR: transfer error flag
      \arg        ENET_DMA_FLAG_EB_ACCESS_ERROR: access error flag
      \arg        ENET_DMA_FLAG_MSC: MSC status flag
      \arg        ENET_DMA_FLAG_WUM: WUM status flag
      \arg        ENET_DMA_FLAG_TST: timestamp trigger status flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus enet_flag_get(enet_flag_enum enet_flag)
{
    if(RESET != (ENET_REG_VAL(enet_flag) & BIT(ENET_BIT_POS(enet_flag)))) {
        return SET;
    } else {
        return RESET;
    }
}

/*!
    \brief    clear the ENET DMA status flag
    \param[in]  enet_flag: ENET DMA flag clear, refer to enet_flag_clear_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_FLAG_TS_CLR: transmit status flag clear
      \arg        ENET_DMA_FLAG_TPS_CLR: transmit process stopped status flag clear
      \arg        ENET_DMA_FLAG_TBU_CLR: transmit buffer unavailable status flag clear
      \arg        ENET_DMA_FLAG_TJT_CLR: transmit jabber timeout status flag clear
      \arg        ENET_DMA_FLAG_RO_CLR: receive overflow status flag clear
      \arg        ENET_DMA_FLAG_TU_CLR: transmit underflow status flag clear
      \arg        ENET_DMA_FLAG_RS_CLR: receive status flag clear
      \arg        ENET_DMA_FLAG_RBU_CLR: receive buffer unavailable status flag clear
      \arg        ENET_DMA_FLAG_RPS_CLR: receive process stopped status flag clear
      \arg        ENET_DMA_FLAG_RWT_CLR: receive watchdog timeout status flag clear
      \arg        ENET_DMA_FLAG_ET_CLR: early transmit status flag clear
      \arg        ENET_DMA_FLAG_FBE_CLR: fatal bus error status flag clear
      \arg        ENET_DMA_FLAG_ER_CLR: early receive status flag clear
      \arg        ENET_DMA_FLAG_AI_CLR: abnormal interrupt summary flag clear
      \arg        ENET_DMA_FLAG_NI_CLR: normal interrupt summary flag clear
    \param[out] none
    \retval     none
*/
void enet_flag_clear(enet_flag_clear_enum enet_flag)
{
    /* write 1 to the corresponding bit in ENET_DMA_STAT, to clear it */
    ENET_REG_VAL(enet_flag) = BIT(ENET_BIT_POS(enet_flag));
}

/*!
    \brief    enable ENET MAC/MSC/DMA interrupt
    \param[in]  enet_int: ENET interrupt,, refer to enet_int_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_INT_WUMIM: WUM interrupt mask
      \arg        ENET_MAC_INT_TMSTIM: timestamp trigger interrupt mask
      \arg        ENET_MSC_INT_RFCEIM: received frame CRC error interrupt mask
      \arg        ENET_MSC_INT_RFAEIM: received frames alignment error interrupt mask
      \arg        ENET_MSC_INT_RGUFIM: received good unicast frames interrupt mask
      \arg        ENET_MSC_INT_TGFSCIM: transmitted good frames single collision interrupt mask
      \arg        ENET_MSC_INT_TGFMSCIM: transmitted good frames more single collision interrupt mask
      \arg        ENET_MSC_INT_TGFIM: transmitted good frames interrupt mask
      \arg        ENET_DMA_INT_TIE: transmit interrupt enable
      \arg        ENET_DMA_INT_TPSIE: transmit process stopped interrupt enable
      \arg        ENET_DMA_INT_TBUIE: transmit buffer unavailable interrupt enable
      \arg        ENET_DMA_INT_TJTIE: transmit jabber timeout interrupt enable
      \arg        ENET_DMA_INT_ROIE: receive overflow interrupt enable
      \arg        ENET_DMA_INT_TUIE: transmit underflow interrupt enable
      \arg        ENET_DMA_INT_RIE: receive interrupt enable
      \arg        ENET_DMA_INT_RBUIE: receive buffer unavailable interrupt enable
      \arg        ENET_DMA_INT_RPSIE: receive process stopped interrupt enable
      \arg        ENET_DMA_INT_RWTIE: receive watchdog timeout interrupt enable
      \arg        ENET_DMA_INT_ETIE: early transmit interrupt enable
      \arg        ENET_DMA_INT_FBEIE: fatal bus error interrupt enable
      \arg        ENET_DMA_INT_ERIE: early receive interrupt enable
      \arg        ENET_DMA_INT_AIE: abnormal interrupt summary enable
      \arg        ENET_DMA_INT_NIE: normal interrupt summary enable
    \param[out] none
    \retval     none
*/
void enet_interrupt_enable(enet_int_enum enet_int)
{
    if(DMA_INTEN_REG_OFFSET == ((uint32_t)enet_int >> 6)) {
        /* ENET_DMA_INTEN register interrupt */
        ENET_REG_VAL(enet_int) |= BIT(ENET_BIT_POS(enet_int));
    } else {
        /* other INTMSK register interrupt */
        ENET_REG_VAL(enet_int) &= ~BIT(ENET_BIT_POS(enet_int));
    }
}

/*!
    \brief    disable ENET MAC/MSC/DMA interrupt
    \param[in]  enet_int: ENET interrupt, refer to enet_int_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_INT_WUMIM: WUM interrupt mask
      \arg        ENET_MAC_INT_TMSTIM: timestamp trigger interrupt mask
      \arg        ENET_MSC_INT_RFCEIM: received frame CRC error interrupt mask
      \arg        ENET_MSC_INT_RFAEIM: received frames alignment error interrupt mask
      \arg        ENET_MSC_INT_RGUFIM: received good unicast frames interrupt mask
      \arg        ENET_MSC_INT_TGFSCIM: transmitted good frames single collision interrupt mask
      \arg        ENET_MSC_INT_TGFMSCIM: transmitted good frames more single collision interrupt mask
      \arg        ENET_MSC_INT_TGFIM: transmitted good frames interrupt mask
      \arg        ENET_DMA_INT_TIE: transmit interrupt enable
      \arg        ENET_DMA_INT_TPSIE: transmit process stopped interrupt enable
      \arg        ENET_DMA_INT_TBUIE: transmit buffer unavailable interrupt enable
      \arg        ENET_DMA_INT_TJTIE: transmit jabber timeout interrupt enable
      \arg        ENET_DMA_INT_ROIE: receive overflow interrupt enable
      \arg        ENET_DMA_INT_TUIE: transmit underflow interrupt enable
      \arg        ENET_DMA_INT_RIE: receive interrupt enable
      \arg        ENET_DMA_INT_RBUIE: receive buffer unavailable interrupt enable
      \arg        ENET_DMA_INT_RPSIE: receive process stopped interrupt enable
      \arg        ENET_DMA_INT_RWTIE: receive watchdog timeout interrupt enable
      \arg        ENET_DMA_INT_ETIE: early transmit interrupt enable
      \arg        ENET_DMA_INT_FBEIE: fatal bus error interrupt enable
      \arg        ENET_DMA_INT_ERIE: early receive interrupt enable
      \arg        ENET_DMA_INT_AIE: abnormal interrupt summary enable
      \arg        ENET_DMA_INT_NIE: normal interrupt summary enable
    \param[out] none
    \retval     none
*/
void enet_interrupt_disable(enet_int_enum enet_int)
{
    if(DMA_INTEN_REG_OFFSET == ((uint32_t)enet_int >> 6)) {
        /* ENET_DMA_INTEN register interrupt */
        ENET_REG_VAL(enet_int) &= ~BIT(ENET_BIT_POS(enet_int));
    } else {
        /* other INTMSK register interrupt */
        ENET_REG_VAL(enet_int) |= BIT(ENET_BIT_POS(enet_int));
    }
}

/*!
    \brief    get ENET MAC/MSC/DMA interrupt flag
    \param[in]  int_flag: ENET interrupt flag, refer to enet_int_flag_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_INT_FLAG_WUM: WUM status flag
      \arg        ENET_MAC_INT_FLAG_MSC: MSC status flag
      \arg        ENET_MAC_INT_FLAG_MSCR: MSC receive status flag
      \arg        ENET_MAC_INT_FLAG_MSCT: MSC transmit status flag
      \arg        ENET_MAC_INT_FLAG_TMST: time stamp trigger status flag
      \arg        ENET_MSC_INT_FLAG_RFCE: received frames CRC error flag
      \arg        ENET_MSC_INT_FLAG_RFAE: received frames alignment error flag
      \arg        ENET_MSC_INT_FLAG_RGUF: received good unicast frames flag
      \arg        ENET_MSC_INT_FLAG_TGFSC: transmitted good frames single collision flag
      \arg        ENET_MSC_INT_FLAG_TGFMSC: transmitted good frames more single collision flag
      \arg        ENET_MSC_INT_FLAG_TGF: transmitted good frames flag
      \arg        ENET_DMA_INT_FLAG_TS: transmit status flag
      \arg        ENET_DMA_INT_FLAG_TPS: transmit process stopped status flag
      \arg        ENET_DMA_INT_FLAG_TBU: transmit buffer unavailable status flag
      \arg        ENET_DMA_INT_FLAG_TJT: transmit jabber timeout status flag
      \arg        ENET_DMA_INT_FLAG_RO: receive overflow status flag
      \arg        ENET_DMA_INT_FLAG_TU: transmit underflow status flag
      \arg        ENET_DMA_INT_FLAG_RS: receive status flag
      \arg        ENET_DMA_INT_FLAG_RBU: receive buffer unavailable status flag
      \arg        ENET_DMA_INT_FLAG_RPS: receive process stopped status flag
      \arg        ENET_DMA_INT_FLAG_RWT: receive watchdog timeout status flag
      \arg        ENET_DMA_INT_FLAG_ET: early transmit status flag
      \arg        ENET_DMA_INT_FLAG_FBE: fatal bus error status flag
      \arg        ENET_DMA_INT_FLAG_ER: early receive status flag
      \arg        ENET_DMA_INT_FLAG_AI: abnormal interrupt summary flag
      \arg        ENET_DMA_INT_FLAG_NI: normal interrupt summary flag
      \arg        ENET_DMA_INT_FLAG_MSC: MSC status flag
      \arg        ENET_DMA_INT_FLAG_WUM: WUM status flag
      \arg        ENET_DMA_INT_FLAG_TST: timestamp trigger status flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus enet_interrupt_flag_get(enet_int_flag_enum int_flag)
{
    if(RESET != (ENET_REG_VAL(int_flag) & BIT(ENET_BIT_POS(int_flag)))) {
        return SET;
    } else {
        return RESET;
    }
}

/*!
    \brief    clear ENET DMA interrupt flag
    \param[in]  int_flag_clear: clear ENET interrupt flag, refer to enet_int_flag_clear_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_INT_FLAG_TS_CLR: transmit status flag
      \arg        ENET_DMA_INT_FLAG_TPS_CLR: transmit process stopped status flag
      \arg        ENET_DMA_INT_FLAG_TBU_CLR: transmit buffer unavailable status flag
      \arg        ENET_DMA_INT_FLAG_TJT_CLR: transmit jabber timeout status flag
      \arg        ENET_DMA_INT_FLAG_RO_CLR: receive overflow status flag
      \arg        ENET_DMA_INT_FLAG_TU_CLR: transmit underflow status flag
      \arg        ENET_DMA_INT_FLAG_RS_CLR: receive status flag
      \arg        ENET_DMA_INT_FLAG_RBU_CLR: receive buffer unavailable status flag
      \arg        ENET_DMA_INT_FLAG_RPS_CLR: receive process stopped status flag
      \arg        ENET_DMA_INT_FLAG_RWT_CLR: receive watchdog timeout status flag
      \arg        ENET_DMA_INT_FLAG_ET_CLR: early transmit status flag
      \arg        ENET_DMA_INT_FLAG_FBE_CLR: fatal bus error status flag
      \arg        ENET_DMA_INT_FLAG_ER_CLR: early receive status flag
      \arg        ENET_DMA_INT_FLAG_AI_CLR: abnormal interrupt summary flag
      \arg        ENET_DMA_INT_FLAG_NI_CLR: normal interrupt summary flag
    \param[out] none
    \retval     none
*/
void enet_interrupt_flag_clear(enet_int_flag_clear_enum int_flag_clear)
{
    /* write 1 to the corresponding bit in ENET_DMA_STAT, to clear it */
    ENET_REG_VAL(int_flag_clear) = BIT(ENET_BIT_POS(int_flag_clear));
}

/*!
    \brief    ENET Tx function enable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_tx_enable(void)
{
    ENET_MAC_CFG |= ENET_MAC_CFG_TEN;
    enet_txfifo_flush();
    ENET_DMA_CTL |= ENET_DMA_CTL_STE;
}

/*!
    \brief    ENET Tx function disable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_tx_disable(void)
{
    ENET_DMA_CTL &= ~ENET_DMA_CTL_STE;
    enet_txfifo_flush();
    ENET_MAC_CFG &= ~ENET_MAC_CFG_TEN;
}

/*!
    \brief    ENET Rx function enable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_rx_enable(void)
{
    ENET_MAC_CFG |= ENET_MAC_CFG_REN;
    ENET_DMA_CTL |= ENET_DMA_CTL_SRE;
}

/*!
    \brief    ENET Rx function disable (include MAC and DMA module)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_rx_disable(void)
{
    ENET_DMA_CTL &= ~ENET_DMA_CTL_SRE;
    ENET_MAC_CFG &= ~ENET_MAC_CFG_REN;
}

/*!
    \brief    put registers value into the application buffer
    \param[in]  type: register type which will be get, refer to enet_registers_type_enum,
                only one parameter can be selected which is shown as below
      \arg        ALL_MAC_REG: get the registers within the offset scope between ENET_MAC_CFG and ENET_MAC_FCTH
      \arg        ALL_MSC_REG: get the registers within the offset scope between ENET_MSC_CTL and ENET_MSC_RGUFCNT
      \arg        ALL_PTP_REG: get the registers within the offset scope between ENET_PTP_TSCTL and ENET_PTP_PPSCTL
      \arg        ALL_DMA_REG: get the registers within the offset scope between ENET_DMA_BCTL and ENET_DMA_CRBADDR
    \param[in]  num: the number of registers that the user want to get
    \param[out] preg: the application buffer pointer for storing the register value
    \retval     none
*/
void enet_registers_get(enet_registers_type_enum type, uint32_t *preg, uint32_t num)
{
    uint32_t offset = 0U, max = 0U, limit = 0U;

    offset = (uint32_t)type;
    max = (uint32_t)type + num;
    limit = sizeof(enet_reg_tab) / sizeof(uint16_t);

    /* prevent element in this array is out of range */
    if(max > limit) {
        max = limit;
    }

    for(; offset < max; offset++) {
        /* get value of the corresponding register */
        *preg = REG32((ENET) + enet_reg_tab[offset]);
        preg++;
    }
}

/*!
    \brief    get the enet debug status from the debug register
    \param[in]  mac_debug: enet debug status
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_RECEIVER_NOT_IDLE: MAC receiver is not in idle state
      \arg        ENET_RX_ASYNCHRONOUS_FIFO_STATE: Rx asynchronous FIFO status
      \arg        ENET_RXFIFO_WRITING: RxFIFO is doing write operation
      \arg        ENET_RXFIFO_READ_STATUS: RxFIFO read operation status
      \arg        ENET_RXFIFO_STATE: RxFIFO state
      \arg        ENET_MAC_TRANSMITTER_NOT_IDLE: MAC transmitter is not in idle state
      \arg        ENET_MAC_TRANSMITTER_STATUS: status of MAC transmitter
      \arg        ENET_PAUSE_CONDITION_STATUS: pause condition status
      \arg        ENET_TXFIFO_READ_STATUS: TxFIFO read operation status
      \arg        ENET_TXFIFO_WRITING: TxFIFO is doing write operation
      \arg        ENET_TXFIFO_NOT_EMPTY: TxFIFO is not empty
      \arg        ENET_TXFIFO_FULL: TxFIFO is full
    \param[out] none
    \retval     value of the status users want to get
*/
uint32_t enet_debug_status_get(uint32_t mac_debug)
{
    uint32_t temp_state = 0U;

    switch(mac_debug) {
    case ENET_RX_ASYNCHRONOUS_FIFO_STATE:
        temp_state = GET_MAC_DBG_RXAFS(ENET_MAC_DBG);
        break;
    case ENET_RXFIFO_READ_STATUS:
        temp_state = GET_MAC_DBG_RXFRS(ENET_MAC_DBG);
        break;
    case ENET_RXFIFO_STATE:
        temp_state = GET_MAC_DBG_RXFS(ENET_MAC_DBG);
        break;
    case ENET_MAC_TRANSMITTER_STATUS:
        temp_state = GET_MAC_DBG_SOMT(ENET_MAC_DBG);
        break;
    case ENET_TXFIFO_READ_STATUS:
        temp_state = GET_MAC_DBG_TXFRS(ENET_MAC_DBG);
        break;
    default:
        if(RESET != (ENET_MAC_DBG & mac_debug)) {
            temp_state = 0x1U;
        }
        break;
    }
    return temp_state;
}

/*!
    \brief    enable the MAC address filter
    \param[in]  mac_addr: select which MAC address will be enable, refer to enet_macaddress_enum
      \arg        ENET_MAC_ADDRESS1: enable MAC address 1 filter
      \arg        ENET_MAC_ADDRESS2: enable MAC address 2 filter
      \arg        ENET_MAC_ADDRESS3: enable MAC address 3 filter
    \param[out] none
    \retval     none
*/
void enet_address_filter_enable(enet_macaddress_enum mac_addr)
{
    REG32(ENET_ADDRH_BASE + mac_addr) |= ENET_MAC_ADDR1H_AFE;
}

/*!
    \brief    disable the MAC address filter
    \param[in]  mac_addr: select which MAC address will be disable, refer to enet_macaddress_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_ADDRESS1: disable MAC address 1 filter
      \arg        ENET_MAC_ADDRESS2: disable MAC address 2 filter
      \arg        ENET_MAC_ADDRESS3: disable MAC address 3 filter
    \param[out] none
    \retval     none
*/
void enet_address_filter_disable(enet_macaddress_enum mac_addr)
{
    REG32(ENET_ADDRH_BASE + mac_addr) &= ~ENET_MAC_ADDR1H_AFE;
}

/*!
    \brief    configure the MAC address filter
    \param[in]  mac_addr: select which MAC address will be configured, refer to enet_macaddress_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC_ADDRESS1: configure MAC address 1 filter
      \arg        ENET_MAC_ADDRESS2: configure MAC address 2 filter
      \arg        ENET_MAC_ADDRESS3: configure MAC address 3 filter
    \param[in]  addr_mask: select which MAC address bytes will be mask
                one or more parameters can be selected which are shown as below
      \arg        ENET_ADDRESS_MASK_BYTE0: mask ENET_MAC_ADDR1L[7:0] bits
      \arg        ENET_ADDRESS_MASK_BYTE1: mask ENET_MAC_ADDR1L[15:8] bits
      \arg        ENET_ADDRESS_MASK_BYTE2: mask ENET_MAC_ADDR1L[23:16] bits
      \arg        ENET_ADDRESS_MASK_BYTE3: mask ENET_MAC_ADDR1L [31:24] bits
      \arg        ENET_ADDRESS_MASK_BYTE4: mask ENET_MAC_ADDR1H [7:0] bits
      \arg        ENET_ADDRESS_MASK_BYTE5: mask ENET_MAC_ADDR1H [15:8] bits
    \param[in]  filter_type: select which MAC address filter type will be selected
                only one parameter can be selected which is shown as below
      \arg        ENET_ADDRESS_FILTER_SA: The MAC address is used to compared with the SA field of the received frame
      \arg        ENET_ADDRESS_FILTER_DA: The MAC address is used to compared with the DA field of the received frame
    \param[out] none
    \retval     none
*/
void enet_address_filter_config(enet_macaddress_enum mac_addr, uint32_t addr_mask, uint32_t filter_type)
{
    uint32_t reg;

    /* get the address filter register value which is to be configured */
    reg = REG32(ENET_ADDRH_BASE + mac_addr);

    /* clear and configure the address filter register */
    reg &= ~(ENET_MAC_ADDR1H_MB | ENET_MAC_ADDR1H_SAF);
    reg |= (addr_mask | filter_type);
    REG32(ENET_ADDRH_BASE + mac_addr) = reg;
}

/*!
    \brief    PHY interface configuration (configure SMI clock and reset PHY chip)
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_phy_config(void)
{
    uint32_t ahbclk;
    uint32_t reg;
    uint16_t phy_value;
    ErrStatus enet_state = ERROR;

    /* clear the previous MDC clock */
    reg = ENET_MAC_PHY_CTL;
    reg &= ~ENET_MAC_PHY_CTL_CLR;

    /* get the HCLK frequency */
    ahbclk = rcu_clock_freq_get(CK_AHB);

    /* configure MDC clock according to HCLK frequency range */
    if(ENET_RANGE(ahbclk, 20000000U, 35000000U)) {
        reg |= ENET_MDC_HCLK_DIV16;
    } else if(ENET_RANGE(ahbclk, 35000000U, 60000000U)) {
        reg |= ENET_MDC_HCLK_DIV26;
    } else if(ENET_RANGE(ahbclk, 60000000U, 100000000U)) {
        reg |= ENET_MDC_HCLK_DIV42;
    } else if(ENET_RANGE(ahbclk, 100000000U, 150000000U)) {
        reg |= ENET_MDC_HCLK_DIV62;
    } else if((ENET_RANGE(ahbclk, 150000000U, 240000000U)) || (240000000U == ahbclk)) {
        reg |= ENET_MDC_HCLK_DIV102;
    } else {
        return enet_state;
    }
    ENET_MAC_PHY_CTL = reg;

    /* reset PHY */
    phy_value = PHY_RESET;
    if(ERROR == (enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &phy_value))) {
        return enet_state;
    }
    /* PHY reset need some time */
    _ENET_DELAY_(ENET_DELAY_TO);

    /* check whether PHY reset is complete */
    if(ERROR == (enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &phy_value))) {
        return enet_state;
    }

    /* PHY reset complete */
    if(RESET == (phy_value & PHY_RESET)) {
        enet_state = SUCCESS;
    }

    return enet_state;
}

/*!
    \brief    write to / read from a PHY register
    \param[in]  direction: only one parameter can be selected which is shown as below, refer to enet_phydirection_enum
      \arg        ENET_PHY_WRITE: write data to phy register
      \arg        ENET_PHY_READ:  read data from phy register
    \param[in]  phy_address: 0x0000 - 0x001F
    \param[in]  phy_reg: 0x0000 - 0x001F
    \param[in]  pvalue: the value will be written to the PHY register in ENET_PHY_WRITE direction
    \param[out] pvalue: the value will be read from the PHY register in ENET_PHY_READ direction
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_phy_write_read(enet_phydirection_enum direction, uint16_t phy_address, uint16_t phy_reg, uint16_t *pvalue)
{
    uint32_t reg, phy_flag;
    uint32_t timeout = 0U;
    ErrStatus enet_state = ERROR;

    /* configure ENET_MAC_PHY_CTL with write/read operation */
    reg = ENET_MAC_PHY_CTL;
    reg &= ~(ENET_MAC_PHY_CTL_PB | ENET_MAC_PHY_CTL_PW | ENET_MAC_PHY_CTL_PR | ENET_MAC_PHY_CTL_PA);
    reg |= (direction | MAC_PHY_CTL_PR(phy_reg) | MAC_PHY_CTL_PA(phy_address) | ENET_MAC_PHY_CTL_PB);

    /* if do the write operation, write value to the register */
    if(ENET_PHY_WRITE == direction) {
        ENET_MAC_PHY_DATA = *pvalue;
    }

    /* do PHY write/read operation, and wait the operation complete  */
    ENET_MAC_PHY_CTL = reg;
    do {
        phy_flag = (ENET_MAC_PHY_CTL & ENET_MAC_PHY_CTL_PB);
        timeout++;
    } while((RESET != phy_flag) && (ENET_DELAY_TO != timeout));

    /* write/read operation complete */
    if(RESET == (ENET_MAC_PHY_CTL & ENET_MAC_PHY_CTL_PB)) {
        enet_state = SUCCESS;
    }

    /* if do the read operation, get value from the register */
    if(ENET_PHY_READ == direction) {
        *pvalue = (uint16_t)ENET_MAC_PHY_DATA;
    }

    return enet_state;
}

/*!
    \brief    enable the loopback function of PHY chip
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_phyloopback_enable(void)
{
    uint16_t temp_phy = 0U;
    ErrStatus phy_state = ERROR;

    /* get the PHY configuration to update it */
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &temp_phy);

    /* enable the PHY loopback mode */
    temp_phy |= PHY_LOOPBACK;

    /* update the PHY control register with the new configuration */
    phy_state = enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &temp_phy);

    return phy_state;
}

/*!
    \brief    disable the loopback function of PHY chip
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_phyloopback_disable(void)
{
    uint16_t temp_phy = 0U;
    ErrStatus phy_state = ERROR;

    /* get the PHY configuration to update it */
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &temp_phy);

    /* disable the PHY loopback mode */
    temp_phy &= (uint16_t)~PHY_LOOPBACK;

    /* update the PHY control register with the new configuration */
    phy_state = enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &temp_phy);

    return phy_state;
}

/*!
    \brief    enable ENET forward feature
    \param[in]  feature: the feature of ENET forward mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_AUTO_PADCRC_DROP: the function of the MAC strips the Pad/FCS field on received frames
      \arg        ENET_TYPEFRAME_CRC_DROP: the function that FCS field(last 4 bytes) of frame will be dropped before forwarding
      \arg        ENET_FORWARD_ERRFRAMES: the function that all frame received with error except runt error are forwarded to memory
      \arg        ENET_FORWARD_UNDERSZ_GOODFRAMES: the function that forwarding undersized good frames
    \param[out] none
    \retval     none
*/
void enet_forward_feature_enable(uint32_t feature)
{
    uint32_t mask;

    mask = (feature & (~(ENET_FORWARD_ERRFRAMES | ENET_FORWARD_UNDERSZ_GOODFRAMES)));
    ENET_MAC_CFG |= mask;

    mask = (feature & (~(ENET_AUTO_PADCRC_DROP | ENET_TYPEFRAME_CRC_DROP)));
    ENET_DMA_CTL |= (mask >> 2);
}

/*!
    \brief    disable ENET forward feature
    \param[in]  feature: the feature of ENET forward mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_AUTO_PADCRC_DROP: the function of the MAC strips the Pad/FCS field on received frames
      \arg        ENET_TYPEFRAME_CRC_DROP: the function that FCS field(last 4 bytes) of frame will be dropped before forwarding
      \arg        ENET_FORWARD_ERRFRAMES: the function that all frame received with error except runt error are forwarded to memory
      \arg        ENET_FORWARD_UNDERSZ_GOODFRAMES: the function that forwarding undersized good frames
    \param[out] none
    \retval     none
*/
void enet_forward_feature_disable(uint32_t feature)
{
    uint32_t mask;

    mask = (feature & (~(ENET_FORWARD_ERRFRAMES | ENET_FORWARD_UNDERSZ_GOODFRAMES)));
    ENET_MAC_CFG &= ~mask;

    mask = (feature & (~(ENET_AUTO_PADCRC_DROP | ENET_TYPEFRAME_CRC_DROP)));
    ENET_DMA_CTL &= ~(mask >> 2);
}

/*!
    \brief      enable ENET fliter feature
    \param[in]  feature: the feature of ENET fliter mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_SRC_FILTER: filter source address function
      \arg        ENET_SRC_FILTER_INVERSE: inverse source address filtering result function
      \arg        ENET_DEST_FILTER_INVERSE: inverse DA filtering result function
      \arg        ENET_MULTICAST_FILTER_PASS: pass all multicast frames function
      \arg        ENET_MULTICAST_FILTER_HASH_MODE: HASH multicast filter function
      \arg        ENET_UNICAST_FILTER_HASH_MODE: HASH unicast filter function
      \arg        ENET_FILTER_MODE_EITHER: HASH or perfect filter function
    \param[out] none
    \retval     none
*/
void enet_fliter_feature_enable(uint32_t feature)
{
    ENET_MAC_FRMF |= feature;
}

/*!
    \brief    disable ENET fliter feature
    \param[in]  feature: the feature of ENET fliter mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_SRC_FILTER: filter source address function
      \arg        ENET_SRC_FILTER_INVERSE: inverse source address filtering result function
      \arg        ENET_DEST_FILTER_INVERSE: inverse DA filtering result function
      \arg        ENET_MULTICAST_FILTER_PASS: pass all multicast frames function
      \arg        ENET_MULTICAST_FILTER_HASH_MODE: HASH multicast filter function
      \arg        ENET_UNICAST_FILTER_HASH_MODE: HASH unicast filter function
      \arg        ENET_FILTER_MODE_EITHER: HASH or perfect filter function
    \param[out] none
    \retval     none
*/
void enet_fliter_feature_disable(uint32_t feature)
{
    ENET_MAC_FRMF &= ~feature;
}

/*!
    \brief    generate the pause frame, ENET will send pause frame after enable transmit flow control
                this function only use in full-dulex mode
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_pauseframe_generate(void)
{
    ErrStatus enet_state = ERROR;
    uint32_t temp = 0U;

    /* in full-duplex mode, must make sure this bit is 0 before writing register */
    temp = ENET_MAC_FCTL & ENET_MAC_FCTL_FLCBBKPA;
    if(RESET == temp) {
        ENET_MAC_FCTL |= ENET_MAC_FCTL_FLCBBKPA;
        enet_state = SUCCESS;
    }
    return enet_state;
}

/*!
    \brief    configure the pause frame detect type
    \param[in]  detect: pause frame detect type
                only one parameter can be selected which is shown as below
      \arg        ENET_MAC0_AND_UNIQUE_ADDRESS_PAUSEDETECT: besides the unique multicast address, MAC can also
                                                            use the MAC0 address to detecting pause frame
      \arg        ENET_UNIQUE_PAUSEDETECT: only the unique multicast address for pause frame which is specified
                                           in IEEE802.3 can be detected
    \param[out] none
    \retval     none
*/
void enet_pauseframe_detect_config(uint32_t detect)
{
    ENET_MAC_FCTL &= ~ENET_MAC_FCTL_UPFDT;
    ENET_MAC_FCTL |= detect;
}

/*!
    \brief    configure the pause frame parameters
    \param[in]  pausetime: pause time in transmit pause control frame
    \param[in]  pause_threshold: the threshold of the pause timer for retransmitting frames automatically
                this value must make sure to be less than configured pause time
                only one parameter can be selected which is shown as below
      \arg        ENET_PAUSETIME_MINUS4: pause time minus 4 slot times
      \arg        ENET_PAUSETIME_MINUS28: pause time minus 28 slot times
      \arg        ENET_PAUSETIME_MINUS144: pause time minus 144 slot times
      \arg        ENET_PAUSETIME_MINUS256: pause time minus 256 slot times
    \param[out] none
    \retval     none
*/
void enet_pauseframe_config(uint32_t pausetime, uint32_t pause_threshold)
{
    ENET_MAC_FCTL &= ~(ENET_MAC_FCTL_PTM | ENET_MAC_FCTL_PLTS);
    ENET_MAC_FCTL |= (MAC_FCTL_PTM(pausetime) | pause_threshold);
}

/*!
    \brief    configure the threshold of the flow control(deactive and active threshold)
    \param[in]  deactive: the threshold of the deactive flow control
                this value should always be less than active flow control value
                only one parameter can be selected which is shown as below
      \arg        ENET_DEACTIVE_THRESHOLD_256BYTES: threshold level is 256 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_512BYTES: threshold level is 512 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_768BYTES: threshold level is 768 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_1024BYTES: threshold level is 1024 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_1280BYTES: threshold level is 1280 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_1536BYTES: threshold level is 1536 bytes
      \arg        ENET_DEACTIVE_THRESHOLD_1792BYTES: threshold level is 1792 bytes
    \param[in]  active: the threshold of the active flow control
                only one parameter can be selected which is shown as below
      \arg        ENET_ACTIVE_THRESHOLD_256BYTES: threshold level is 256 bytes
      \arg        ENET_ACTIVE_THRESHOLD_512BYTES: threshold level is 512 bytes
      \arg        ENET_ACTIVE_THRESHOLD_768BYTES: threshold level is 768 bytes
      \arg        ENET_ACTIVE_THRESHOLD_1024BYTES: threshold level is 1024 bytes
      \arg        ENET_ACTIVE_THRESHOLD_1280BYTES: threshold level is 1280 bytes
      \arg        ENET_ACTIVE_THRESHOLD_1536BYTES: threshold level is 1536 bytes
      \arg        ENET_ACTIVE_THRESHOLD_1792BYTES: threshold level is 1792 bytes
    \param[out] none
    \retval     none
*/
void enet_flowcontrol_threshold_config(uint32_t deactive, uint32_t active)
{
    ENET_MAC_FCTH = ((deactive | active) >> 8);
}

/*!
    \brief    enable ENET flow control feature
    \param[in]  feature: the feature of ENET flow control mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_ZERO_QUANTA_PAUSE: the automatic zero-quanta generation function
      \arg        ENET_TX_FLOWCONTROL: the flow control operation in the MAC
      \arg        ENET_RX_FLOWCONTROL: decoding function for the received pause frame and process it
      \arg        ENET_BACK_PRESSURE: back pressure operation in the MAC(only use in half-dulex mode)
    \param[out] none
    \retval     none
*/
void enet_flowcontrol_feature_enable(uint32_t feature)
{
    if(RESET != (feature & ENET_ZERO_QUANTA_PAUSE)) {
        ENET_MAC_FCTL &= ~ENET_ZERO_QUANTA_PAUSE;
    }
    feature &= ~ENET_ZERO_QUANTA_PAUSE;
    ENET_MAC_FCTL |= feature;
}

/*!
    \brief    disable ENET flow control feature
    \param[in]  feature: the feature of ENET flow control mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_ZERO_QUANTA_PAUSE: the automatic zero-quanta generation function
      \arg        ENET_TX_FLOWCONTROL: the flow control operation in the MAC
      \arg        ENET_RX_FLOWCONTROL: decoding function for the received pause frame and process it
      \arg        ENET_BACK_PRESSURE: back pressure operation in the MAC(only use in half-dulex mode)
    \param[out] none
    \retval     none
*/
void enet_flowcontrol_feature_disable(uint32_t feature)
{
    if(RESET != (feature & ENET_ZERO_QUANTA_PAUSE)) {
        ENET_MAC_FCTL |= ENET_ZERO_QUANTA_PAUSE;
    }
    feature &= ~ENET_ZERO_QUANTA_PAUSE;
    ENET_MAC_FCTL &= ~feature;
}

/*!
    \brief    get the dma transmit/receive process state
    \param[in]  direction: choose the direction of dma process which users want to check, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: dma transmit process
      \arg        ENET_DMA_RX: dma receive process
    \param[out] none
    \retval     state of dma process, the value range shows below:
                  ENET_RX_STATE_STOPPED, ENET_RX_STATE_FETCHING, ENET_RX_STATE_WAITING,
                  ENET_RX_STATE_SUSPENDED, ENET_RX_STATE_CLOSING, ENET_RX_STATE_QUEUING,
                  ENET_TX_STATE_STOPPED, ENET_TX_STATE_FETCHING, ENET_TX_STATE_WAITING,
                  ENET_TX_STATE_READING, ENET_TX_STATE_SUSPENDED, ENET_TX_STATE_CLOSING
*/
uint32_t enet_dmaprocess_state_get(enet_dmadirection_enum direction)
{
    uint32_t reval;
    reval = (uint32_t)(ENET_DMA_STAT & (uint32_t)direction);
    return reval;
}

/*!
    \brief    poll the DMA transmission/reception enable by writing any value to the
                ENET_DMA_TPEN/ENET_DMA_RPEN register, this will make the DMA to resume transmission/reception
    \param[in]  direction: choose the direction of DMA process which users want to resume, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA transmit process
      \arg        ENET_DMA_RX: DMA receive process
    \param[out] none
    \retval     none
*/
void enet_dmaprocess_resume(enet_dmadirection_enum direction)
{
    if(ENET_DMA_TX == direction) {
        ENET_DMA_TPEN = 0U;
    } else {
        ENET_DMA_RPEN = 0U;
    }
}

/*!
    \brief    check and recover the Rx process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_rxprocess_check_recovery(void)
{
    uint32_t status;

    /* get DAV information of current RxDMA descriptor */
    status = dma_current_rxdesc->status;
    status &= ENET_RDES0_DAV;

    /* if current descriptor is owned by DMA, but the descriptor address mismatches with
    receive descriptor address pointer updated by RxDMA controller */
    if((ENET_DMA_CRDADDR != ((uint32_t)dma_current_rxdesc)) &&
            (ENET_RDES0_DAV == status)) {
        dma_current_rxdesc = (enet_descriptors_struct *)ENET_DMA_CRDADDR;
    }
}

/*!
    \brief    flush the ENET transmit FIFO, and wait until the flush operation completes
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus enet_txfifo_flush(void)
{
    uint32_t flush_state;
    uint32_t timeout = 0U;
    ErrStatus enet_state = ERROR;

    /* set the FTF bit for flushing transmit FIFO */
    ENET_DMA_CTL |= ENET_DMA_CTL_FTF;
    /* wait until the flush operation completes */
    do {
        flush_state = ENET_DMA_CTL & ENET_DMA_CTL_FTF;
        timeout++;
    } while((RESET != flush_state) && (timeout < ENET_DELAY_TO));
    /* return ERROR due to timeout */
    if(RESET == flush_state) {
        enet_state = SUCCESS;
    }

    return  enet_state;
}

/*!
    \brief    get the transmit/receive address of current descriptor, or current buffer, or descriptor table
    \param[in]  addr_get: choose the address which users want to get, refer to enet_desc_reg_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_RX_DESC_TABLE: the start address of the receive descriptor table
      \arg        ENET_RX_CURRENT_DESC: the start descriptor address of the current receive descriptor read by
                                        the RxDMA controller
      \arg        ENET_RX_CURRENT_BUFFER: the current receive buffer address being read by the RxDMA controller
      \arg        ENET_TX_DESC_TABLE: the start address of the transmit descriptor table
      \arg        ENET_TX_CURRENT_DESC: the start descriptor address of the current transmit descriptor read by
                                        the TxDMA controller
      \arg        ENET_TX_CURRENT_BUFFER: the current transmit buffer address being read by the TxDMA controller
    \param[out] none
    \retval     address value
*/
uint32_t enet_current_desc_address_get(enet_desc_reg_enum addr_get)
{
    uint32_t reval = 0U;

    reval = REG32((ENET) + (uint32_t)addr_get);
    return reval;
}

/*!
    \brief    get the Tx or Rx descriptor information
    \param[in]  desc: the descriptor pointer which users want to get information
    \param[in]  info_get: the descriptor information type which is selected, refer to enet_descstate_enum
                only one parameter can be selected which is shown as below
      \arg        RXDESC_BUFFER_1_SIZE: receive buffer 1 size
      \arg        RXDESC_BUFFER_2_SIZE: receive buffer 2 size
      \arg        RXDESC_FRAME_LENGTH: the byte length of the received frame that was transferred to the buffer
      \arg        TXDESC_COLLISION_COUNT: the number of collisions occurred before the frame was transmitted
      \arg        RXDESC_BUFFER_1_ADDR: the buffer1 address of the Rx frame
      \arg        TXDESC_BUFFER_1_ADDR: the buffer1 address of the Tx frame
    \param[out] none
    \retval     descriptor information, if value is 0xFFFFFFFFU, means the false input parameter
*/
uint32_t enet_desc_information_get(enet_descriptors_struct *desc, enet_descstate_enum info_get)
{
    uint32_t reval = 0xFFFFFFFFU;

    switch(info_get) {
    case RXDESC_BUFFER_1_SIZE:
        reval = GET_RDES1_RB1S(desc->control_buffer_size);
        break;
    case RXDESC_BUFFER_2_SIZE:
        reval = GET_RDES1_RB2S(desc->control_buffer_size);
        break;
    case RXDESC_FRAME_LENGTH:
        reval = GET_RDES0_FRML(desc->status);
        if(reval > 4U) {
            reval = reval - 4U;

            /* if is a type frame, and CRC is not included in forwarding frame */
            if((RESET != (ENET_MAC_CFG & ENET_MAC_CFG_TFCD)) && (RESET != (desc->status & ENET_RDES0_FRMT))) {
                reval = reval + 4U;
            }
        } else {
            reval = 0U;
        }

        break;
    case RXDESC_BUFFER_1_ADDR:
        reval = desc->buffer1_addr;
        break;
    case TXDESC_BUFFER_1_ADDR:
        reval = desc->buffer1_addr;
        break;
    case TXDESC_COLLISION_COUNT:
        reval = GET_TDES0_COCNT(desc->status);
        break;
    default:
        break;
    }
    return reval;
}

/*!
    \brief    get the number of missed frames during receiving
    \param[in]  none
    \param[out] rxfifo_drop: pointer to the number of frames dropped by RxFIFO
    \param[out] rxdma_drop: pointer to the number of frames missed by the RxDMA controller
    \retval     none
*/
void enet_missed_frame_counter_get(uint32_t *rxfifo_drop, uint32_t *rxdma_drop)
{
    uint32_t temp_counter = 0U;

    temp_counter = ENET_DMA_MFBOCNT;
    *rxfifo_drop = GET_DMA_MFBOCNT_MSFA(temp_counter);
    *rxdma_drop = GET_DMA_MFBOCNT_MSFC(temp_counter);
}

/*!
    \brief    get the bit flag of ENET DMA descriptor
    \param[in]  desc: the descriptor pointer which users want to get flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_DB: deferred
      \arg        ENET_TDES0_UFE: underflow error
      \arg        ENET_TDES0_EXD: excessive deferral
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_ECO: excessive collision
      \arg        ENET_TDES0_LCO: late collision
      \arg        ENET_TDES0_NCA: no carrier
      \arg        ENET_TDES0_LCA: loss of carrier
      \arg        ENET_TDES0_IPPE: IP payload error
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_JT: jabber timeout
      \arg        ENET_TDES0_ES: error summary
      \arg        ENET_TDES0_IPHE: IP header error
      \arg        ENET_TDES0_TTMSS: transmit timestamp status
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit

      \arg        ENET_RDES0_PCERR: payload checksum error
      \arg        ENET_RDES0_EXSV: extended status valid
      \arg        ENET_RDES0_CERR: CRC error
      \arg        ENET_RDES0_DBERR: dribble bit error
      \arg        ENET_RDES0_RERR: receive error
      \arg        ENET_RDES0_RWDT: receive watchdog timeout
      \arg        ENET_RDES0_FRMT: frame type
      \arg        ENET_RDES0_LCO: late collision
      \arg        ENET_RDES0_IPHERR: IP frame header error
      \arg        ENET_RDES0_TSV: timestamp valid
      \arg        ENET_RDES0_LDES: last descriptor
      \arg        ENET_RDES0_FDES: first descriptor
      \arg        ENET_RDES0_VTAG: VLAN tag
      \arg        ENET_RDES0_OERR: overflow error
      \arg        ENET_RDES0_LERR: length error
      \arg        ENET_RDES0_SAFF: SA filter fail
      \arg        ENET_RDES0_DERR: descriptor error
      \arg        ENET_RDES0_ERRS: error summary
      \arg        ENET_RDES0_DAFF: destination address filter fail
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus enet_desc_flag_get(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    FlagStatus enet_flag = RESET;

    if((uint32_t)RESET != (desc->status & desc_flag)) {
        enet_flag = SET;
    }

    return enet_flag;
}

/*!
    \brief    set the bit flag of ENET DMA descriptor
    \param[in]  desc: the descriptor pointer which users want to set flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     none
*/
void enet_desc_flag_set(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    desc->status |= desc_flag;
}

/*!
    \brief    clear the bit flag of ENET DMA descriptor
    \param[in]  desc: the descriptor pointer which users want to clear flag
    \param[in]  desc_flag: the bit flag of ENET DMA descriptor
                only one parameter can be selected which is shown as below
      \arg        ENET_TDES0_VFRM: VLAN frame
      \arg        ENET_TDES0_FRMF: frame flushed
      \arg        ENET_TDES0_TCHM: the second address chained mode
      \arg        ENET_TDES0_TERM: transmit end of ring mode
      \arg        ENET_TDES0_TTSEN: transmit timestamp function enable
      \arg        ENET_TDES0_DPAD: disable adding pad
      \arg        ENET_TDES0_DCRC: disable CRC
      \arg        ENET_TDES0_FSG: first segment
      \arg        ENET_TDES0_LSG: last segment
      \arg        ENET_TDES0_INTC: interrupt on completion
      \arg        ENET_TDES0_DAV: DAV bit
      \arg        ENET_RDES0_DAV: descriptor available
    \param[out] none
    \retval     none
*/
void enet_desc_flag_clear(enet_descriptors_struct *desc, uint32_t desc_flag)
{
    desc->status &= ~desc_flag;
}

/*!
    \brief    when receiving completed, set RS bit in ENET_DMA_STAT register will immediately set
    \param[in]  desc: the descriptor pointer which users want to configure
    \param[out] none
    \retval     none
*/
void enet_rx_desc_immediate_receive_complete_interrupt(enet_descriptors_struct *desc)
{
    desc->control_buffer_size &= ~ENET_RDES1_DINTC;
}

/*!
    \brief    when receiving completed, set RS bit in ENET_DMA_STAT register will is set after a configurable delay time
    \param[in]  desc: the descriptor pointer which users want to configure
    \param[in]  delay_time: delay a time of 256*delay_time HCLK(0x00000000 - 0x000000FF)
    \param[out] none
    \retval     none
*/
void enet_rx_desc_delay_receive_complete_interrupt(enet_descriptors_struct *desc, uint32_t delay_time)
{
    desc->control_buffer_size |= ENET_RDES1_DINTC;
    ENET_DMA_RSWDC = DMA_RSWDC_WDCFRS(delay_time);
}

/*!
    \brief    drop current receive frame
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_rxframe_drop(void)
{
    /* enable reception, descriptor is owned by DMA */
    dma_current_rxdesc->status = ENET_RDES0_DAV;

    /* chained mode */
    if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RCHM)) {
        if(NULL != dma_current_ptp_rxdesc) {
            dma_current_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->buffer2_next_desc_addr);
            /* if it is the last ptp descriptor */
            if(0U != dma_current_ptp_rxdesc->status) {
                /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
                dma_current_ptp_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->status);
            } else {
                /* ponter to the next ptp descriptor */
                dma_current_ptp_rxdesc++;
            }
        } else {
            dma_current_rxdesc = (enet_descriptors_struct *)(dma_current_rxdesc->buffer2_next_desc_addr);
        }

    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_rxdesc = (enet_descriptors_struct *)(ENET_DMA_RDTADDR);
            if(NULL != dma_current_ptp_rxdesc) {
                dma_current_ptp_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->status);
            }
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_rxdesc = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL));
            if(NULL != dma_current_ptp_rxdesc) {
                dma_current_ptp_rxdesc++;
            }
        }
    }
}

/*!
    \brief    enable DMA feature
    \param[in]  feature: the feature of DMA mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_NO_FLUSH_RXFRAME: RxDMA does not flushes frames function
      \arg        ENET_SECONDFRAME_OPT: TxDMA controller operate on second frame function
    \param[out] none
    \retval     none
*/
void enet_dma_feature_enable(uint32_t feature)
{
    ENET_DMA_CTL |= feature;
}

/*!
    \brief    disable DMA feature
    \param[in]  feature: the feature of DMA mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_NO_FLUSH_RXFRAME: RxDMA does not flushes frames function
      \arg        ENET_SECONDFRAME_OPT: TxDMA controller operate on second frame function
    \param[out] none
    \retval     none
*/
void enet_dma_feature_disable(uint32_t feature)
{
    ENET_DMA_CTL &= ~feature;
}

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
/*!
    \brief    get the bit of extended status flag in ENET DMA descriptor
    \param[in]  desc: the descriptor pointer which users want to get the extended status flag
    \param[in]  desc_status: the extended status want to get
                only one parameter can be selected which is shown as below
      \arg        ENET_RDES4_IPPLDT: IP frame payload type
      \arg        ENET_RDES4_IPHERR: IP frame header error
      \arg        ENET_RDES4_IPPLDERR: IP frame payload error
      \arg        ENET_RDES4_IPCKSB: IP frame checksum bypassed
      \arg        ENET_RDES4_IPF4: IP frame in version 4
      \arg        ENET_RDES4_IPF6: IP frame in version 6
      \arg        ENET_RDES4_PTPMT: PTP message type
      \arg        ENET_RDES4_PTPOEF: PTP on ethernet frame
      \arg        ENET_RDES4_PTPVF: PTP version format
    \param[out] none
    \retval     value of extended status
*/
uint32_t enet_rx_desc_enhanced_status_get(enet_descriptors_struct *desc, uint32_t desc_status)
{
    uint32_t reval = 0xFFFFFFFFU;

    switch(desc_status) {
    case ENET_RDES4_IPPLDT:
        reval = GET_RDES4_IPPLDT(desc->extended_status);
        break;
    case ENET_RDES4_PTPMT:
        reval = GET_RDES4_PTPMT(desc->extended_status);
        break;
    default:
        if((uint32_t)RESET != (desc->extended_status & desc_status)) {
            reval = 1U;
        } else {
            reval = 0U;
        }
    }

    return reval;
}

/*!
    \brief    configure descriptor to work in enhanced mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_desc_select_enhanced_mode(void)
{
    ENET_DMA_BCTL |= ENET_DMA_BCTL_DFM;
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in enhanced chain mode with ptp function
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_ptp_enhanced_descriptors_chain_init(enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc, *desc_tab;
    uint8_t *buf;

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* select chain mode, and enable transmit timestamp function */
        desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* select receive chained mode and set buffer1 size */
        desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
    }

    /* configuration each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* if is not the last descriptor */
        if(num < (count - 1U)) {
            /* configure the next descriptor address */
            desc->buffer2_next_desc_addr = (uint32_t)(desc_tab + num + 1U);
        } else {
            /* when it is the last descriptor, the next descriptor address
            equals to first descriptor address in descriptor table */
            desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
        }
    }
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in enhanced ring mode with ptp function
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[out] none
    \retval     none
*/
void enet_ptp_enhanced_descriptors_ring_init(enet_dmadirection_enum direction)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc;
    enet_descriptors_struct *desc_tab;
    uint8_t *buf;

    /* configure descriptor skip length */
    ENET_DMA_BCTL &= ~ENET_DMA_BCTL_DPSL;
    ENET_DMA_BCTL |= DMA_BCTL_DPSL(0);

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* select ring mode, and enable transmit timestamp function */
        desc_status = ENET_TDES0_TTSEN;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* set buffer1 size */
        desc_bufsize = ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
    }

    /* configure each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* when it is the last descriptor */
        if(num == (count - 1U)) {
            if(ENET_DMA_TX == direction) {
                /* configure transmit end of ring mode */
                desc->status |= ENET_TDES0_TERM;
            } else {
                /* configure receive end of ring mode */
                desc->control_buffer_size |= ENET_RDES1_RERM;
            }
        }
    }
}

/*!
    \brief    receive a packet data with timestamp values to application buffer, when the DMA is in enhanced mode
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] buffer: pointer to the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_receive_enhanced_mode(uint8_t *buffer, uint32_t bufsize, uint32_t timestamp[])
{
    uint32_t offset = 0U, size = 0U;
    uint32_t timeout = 0U;
    uint32_t rdes0_tsv_flag;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_DAV)) {
        return ERROR;
    }

    /* if buffer pointer is null, indicates that users has copied data in application */
    if(NULL != buffer) {
        /* if no error occurs, and the frame uses only one descriptor */
        if(((uint32_t)RESET == (dma_current_rxdesc->status & ENET_RDES0_ERRS)) &&
                ((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_LDES)) &&
                ((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_FDES))) {
            /* get the frame length except CRC */
            size = GET_RDES0_FRML(dma_current_rxdesc->status) - 4U;

            /* if is a type frame, and CRC is not included in forwarding frame */
            if((RESET != (ENET_MAC_CFG & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc->status & ENET_RDES0_FRMT))) {
                size = size + 4U;
            }

            /* to avoid situation that the frame size exceeds the buffer length */
            if(size > bufsize) {
                return ERROR;
            }

            /* copy data from Rx buffer to application buffer */
            for(offset = 0; offset < size; offset++) {
                (*(buffer + offset)) = (*(__IO uint8_t *)((dma_current_rxdesc->buffer1_addr) + offset));
            }
        } else {
            return ERROR;
        }
    }

    /* if timestamp pointer is null, indicates that users don't care timestamp in application */
    if(NULL != timestamp) {
        /* wait for ENET_RDES0_TSV flag to be set, the timestamp value is taken and
        write to the RDES6 and RDES7 */
        do {
            rdes0_tsv_flag = (dma_current_rxdesc->status & ENET_RDES0_TSV);
            timeout++;
        } while((RESET == rdes0_tsv_flag) && (timeout < ENET_DELAY_TO));

        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            return ERROR;
        }

        /* clear the ENET_RDES0_TSV flag */
        dma_current_rxdesc->status &= ~ENET_RDES0_TSV;
        /* get the timestamp value of the received frame */
        timestamp[0] = dma_current_rxdesc->timestamp_low;
        timestamp[1] = dma_current_rxdesc->timestamp_high;
    }

    /* enable reception, descriptor is owned by DMA */
    dma_current_rxdesc->status = ENET_RDES0_DAV;

    /* check Rx buffer unavailable flag status */
    if((uint32_t)RESET != (ENET_DMA_STAT & ENET_DMA_STAT_RBU)) {
        /* Clear RBU flag */
        ENET_DMA_STAT = ENET_DMA_STAT_RBU;
        /* resume DMA reception by writing to the RPEN register*/
        ENET_DMA_RPEN = 0;
    }

    /* update the current RxDMA descriptor pointer to the next decriptor in RxDMA decriptor table */
    /* chained mode */
    if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RCHM)) {
        dma_current_rxdesc = (enet_descriptors_struct *)(dma_current_rxdesc->buffer2_next_desc_addr);
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_rxdesc = (enet_descriptors_struct *)(ENET_DMA_RDTADDR);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_rxdesc = (enet_descriptors_struct *)((uint32_t)dma_current_rxdesc + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL));
        }
    }

    return SUCCESS;
}

/*!
    \brief    send data with timestamp values in application buffer as a transmit packet, when the DMA is in enhanced mode
    \param[in]  buffer: pointer on the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_transmit_enhanced_mode(uint8_t *buffer, uint32_t length, uint32_t timestamp[])
{
    uint32_t offset = 0;
    uint32_t dma_tbu_flag, dma_tu_flag;
    uint32_t tdes0_ttmss_flag;
    uint32_t timeout = 0;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_DAV)) {
        return ERROR;
    }

    /* only frame length no more than ENET_MAX_FRAME_SIZE is allowed */
    if(length > ENET_MAX_FRAME_SIZE) {
        return ERROR;
    }

    /* if buffer pointer is null, indicates that users has handled data in application */
    if(NULL != buffer) {
        /* copy frame data from application buffer to Tx buffer */
        for(offset = 0; offset < length; offset++) {
            (*(__IO uint8_t *)((dma_current_txdesc->buffer1_addr) + offset)) = (*(buffer + offset));
        }
    }
    /* set the frame length */
    dma_current_txdesc->control_buffer_size = length;
    /* set the segment of frame, frame is transmitted in one descriptor */
    dma_current_txdesc->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
    /* enable the DMA transmission */
    dma_current_txdesc->status |= ENET_TDES0_DAV;

    /* check Tx buffer unavailable flag status */
    dma_tbu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TBU);
    dma_tu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TU);

    if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
        /* Clear TBU and TU flag */
        ENET_DMA_STAT = (dma_tbu_flag | dma_tu_flag);
        /* resume DMA transmission by writing to the TPEN register*/
        ENET_DMA_TPEN = 0;
    }

    /* if timestamp pointer is null, indicates that users don't care timestamp in application */
    if(NULL != timestamp) {
        /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
        do {
            tdes0_ttmss_flag = (dma_current_txdesc->status & ENET_TDES0_TTMSS);
            timeout++;
        } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            return ERROR;
        }

        /* clear the ENET_TDES0_TTMSS flag */
        dma_current_txdesc->status &= ~ENET_TDES0_TTMSS;
        /* get the timestamp value of the transmit frame */
        timestamp[0] = dma_current_txdesc->timestamp_low;
        timestamp[1] = dma_current_txdesc->timestamp_high;
    }

    /* update the current TxDMA descriptor pointer to the next decriptor in TxDMA decriptor table*/
    /* chained mode */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TCHM)) {
        dma_current_txdesc = (enet_descriptors_struct *)(dma_current_txdesc->buffer2_next_desc_addr);
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_txdesc = (enet_descriptors_struct *)(ENET_DMA_TDTADDR);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_txdesc = (enet_descriptors_struct *)((uint32_t)dma_current_txdesc + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL));
        }
    }

    return SUCCESS;
}

#else

/*!
    \brief    configure descriptor to work in normal mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_desc_select_normal_mode(void)
{
    ENET_DMA_BCTL &= ~ENET_DMA_BCTL_DFM;
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in normal chain mode with PTP function
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[in]  desc_ptptab: pointer to the first descriptor address of PTP Rx descriptor table
    \param[out] none
    \retval     none
*/
void enet_ptp_normal_descriptors_chain_init(enet_dmadirection_enum direction, enet_descriptors_struct *desc_ptptab)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc, *desc_tab;
    uint8_t *buf;

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* select chain mode, and enable transmit timestamp function */
        desc_status = ENET_TDES0_TCHM | ENET_TDES0_TTSEN;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
        dma_current_ptp_txdesc = desc_ptptab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* select receive chained mode and set buffer1 size */
        desc_bufsize = ENET_RDES1_RCHM | (uint32_t)ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
        dma_current_ptp_rxdesc = desc_ptptab;
    }

    /* configure each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* if is not the last descriptor */
        if(num < (count - 1U)) {
            /* configure the next descriptor address */
            desc->buffer2_next_desc_addr = (uint32_t)(desc_tab + num + 1U);
        } else {
            /* when it is the last descriptor, the next descriptor address
            equals to first descriptor address in descriptor table */
            desc->buffer2_next_desc_addr = (uint32_t)desc_tab;
        }
        /* set desc_ptptab equal to desc_tab */
        (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
        (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
    }
    /* when it is the last ptp descriptor, preserve the first descriptor
    address of desc_ptptab in ptp descriptor status */
    (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
}

/*!
    \brief    initialize the DMA Tx/Rx descriptors's parameters in normal ring mode with PTP function
    \param[in]  direction: the descriptors which users want to init, refer to enet_dmadirection_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_DMA_TX: DMA Tx descriptors
      \arg        ENET_DMA_RX: DMA Rx descriptors
    \param[in]  desc_ptptab: pointer to the first descriptor address of PTP Rx descriptor table
    \param[out] none
    \retval     none
*/
void enet_ptp_normal_descriptors_ring_init(enet_dmadirection_enum direction, enet_descriptors_struct *desc_ptptab)
{
    uint32_t num = 0U, count = 0U, maxsize = 0U;
    uint32_t desc_status = 0U, desc_bufsize = 0U;
    enet_descriptors_struct *desc, *desc_tab;
    uint8_t *buf;

    /* configure descriptor skip length */
    ENET_DMA_BCTL &= ~ENET_DMA_BCTL_DPSL;
    ENET_DMA_BCTL |= DMA_BCTL_DPSL(0);

    /* if want to initialize DMA Tx descriptors */
    if(ENET_DMA_TX == direction) {
        /* save a copy of the DMA Tx descriptors */
        desc_tab = txdesc_tab;
        buf = &tx_buff[0][0];
        count = ENET_TXBUF_NUM;
        maxsize = ENET_TXBUF_SIZE;

        /* select ring mode, and enable transmit timestamp function */
        desc_status = ENET_TDES0_TTSEN;

        /* configure DMA Tx descriptor table address register */
        ENET_DMA_TDTADDR = (uint32_t)desc_tab;
        dma_current_txdesc = desc_tab;
        dma_current_ptp_txdesc = desc_ptptab;
    } else {
        /* if want to initialize DMA Rx descriptors */
        /* save a copy of the DMA Rx descriptors */
        desc_tab = rxdesc_tab;
        buf = &rx_buff[0][0];
        count = ENET_RXBUF_NUM;
        maxsize = ENET_RXBUF_SIZE;

        /* enable receiving */
        desc_status = ENET_RDES0_DAV;
        /* select receive ring mode and set buffer1 size */
        desc_bufsize = (uint32_t)ENET_RXBUF_SIZE;

        /* configure DMA Rx descriptor table address register */
        ENET_DMA_RDTADDR = (uint32_t)desc_tab;
        dma_current_rxdesc = desc_tab;
        dma_current_ptp_rxdesc = desc_ptptab;
    }

    /* configure each descriptor */
    for(num = 0U; num < count; num++) {
        /* get the pointer to the next descriptor of the descriptor table */
        desc = desc_tab + num;

        /* configure descriptors */
        desc->status = desc_status;
        desc->control_buffer_size = desc_bufsize;
        desc->buffer1_addr = (uint32_t)(&buf[num * maxsize]);

        /* when it is the last descriptor */
        if(num == (count - 1U)) {
            if(ENET_DMA_TX == direction) {
                /* configure transmit end of ring mode */
                desc->status |= ENET_TDES0_TERM;
            } else {
                /* configure receive end of ring mode */
                desc->control_buffer_size |= ENET_RDES1_RERM;
            }
        }
        /* set desc_ptptab equal to desc_tab */
        (&desc_ptptab[num])->buffer1_addr = desc->buffer1_addr;
        (&desc_ptptab[num])->buffer2_next_desc_addr = desc->buffer2_next_desc_addr;
    }
    /* when it is the last ptp descriptor, preserve the first descriptor
    address of desc_ptptab in ptp descriptor status */
    (&desc_ptptab[num - 1U])->status = (uint32_t)desc_ptptab;
}

/*!
    \brief    receive a packet data with timestamp values to application buffer, when the DMA is in normal mode
    \param[in]  bufsize: the size of buffer which is the parameter in function
    \param[out] buffer: pointer to the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_receive_normal_mode(uint8_t *buffer, uint32_t bufsize, uint32_t timestamp[])
{
    uint32_t offset = 0U, size = 0U;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_DAV)) {
        return ERROR;
    }

    /* if buffer pointer is null, indicates that users has copied data in application */
    if(NULL != buffer) {
        /* if no error occurs, and the frame uses only one descriptor */
        if(((uint32_t)RESET == (dma_current_rxdesc->status & ENET_RDES0_ERRS)) &&
                ((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_LDES)) &&
                ((uint32_t)RESET != (dma_current_rxdesc->status & ENET_RDES0_FDES))) {

            /* get the frame length except CRC */
            size = GET_RDES0_FRML(dma_current_rxdesc->status) - 4U;
            /* if is a type frame, and CRC is not included in forwarding frame */
            if((RESET != (ENET_MAC_CFG & ENET_MAC_CFG_TFCD)) && (RESET != (dma_current_rxdesc->status & ENET_RDES0_FRMT))) {
                size = size + 4U;
            }

            /* to avoid situation that the frame size exceeds the buffer length */
            if(size > bufsize) {
                return ERROR;
            }

            /* copy data from Rx buffer to application buffer */
            for(offset = 0U; offset < size; offset++) {
                (*(buffer + offset)) = (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_rxdesc->buffer1_addr) + offset));
            }

        } else {
            return ERROR;
        }
    }
    /* copy timestamp value from Rx descriptor to application array */
    timestamp[0] = dma_current_rxdesc->buffer1_addr;
    timestamp[1] = dma_current_rxdesc->buffer2_next_desc_addr;

    dma_current_rxdesc->buffer1_addr = dma_current_ptp_rxdesc ->buffer1_addr ;
    dma_current_rxdesc->buffer2_next_desc_addr = dma_current_ptp_rxdesc ->buffer2_next_desc_addr;

    /* enable reception, descriptor is owned by DMA */
    dma_current_rxdesc->status = ENET_RDES0_DAV;

    /* check Rx buffer unavailable flag status */
    if((uint32_t)RESET != (ENET_DMA_STAT & ENET_DMA_STAT_RBU)) {
        /* clear RBU flag */
        ENET_DMA_STAT = ENET_DMA_STAT_RBU;
        /* resume DMA reception by writing to the RPEN register*/
        ENET_DMA_RPEN = 0U;
    }


    /* update the current RxDMA descriptor pointer to the next decriptor in RxDMA decriptor table */
    /* chained mode */
    if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RCHM)) {
        dma_current_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->buffer2_next_desc_addr);
        /* if it is the last ptp descriptor */
        if(0U != dma_current_ptp_rxdesc->status) {
            /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
            dma_current_ptp_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->status);
        } else {
            /* ponter to the next ptp descriptor */
            dma_current_ptp_rxdesc++;
        }
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_rxdesc->control_buffer_size & ENET_RDES1_RERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_rxdesc = (enet_descriptors_struct *)(ENET_DMA_RDTADDR);
            /* RDES2 and RDES3 will not be covered by buffer address, so do not need to preserve a new table,
            use the same table with RxDMA descriptor */
            dma_current_ptp_rxdesc = (enet_descriptors_struct *)(dma_current_ptp_rxdesc->status);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_rxdesc = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_rxdesc + ETH_DMARXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL));
            dma_current_ptp_rxdesc ++;
        }
    }

    return SUCCESS;
}

/*!
    \brief    send data with timestamp values in application buffer as a transmit packet, when the DMA is in normal mode
    \param[in]  buffer: pointer on the application buffer
                note -- if the input is NULL, user should copy data in application by himself
    \param[in]  length: the length of frame data to be transmitted
    \param[out] timestamp: pointer to the table which stores the timestamp high and low
                note -- if the input is NULL, timestamp is ignored
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptpframe_transmit_normal_mode(uint8_t *buffer, uint32_t length, uint32_t timestamp[])
{
    uint32_t offset = 0U, timeout = 0U;
    uint32_t dma_tbu_flag, dma_tu_flag, tdes0_ttmss_flag;

    /* the descriptor is busy due to own by the DMA */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_DAV)) {
        return ERROR;
    }

    /* only frame length no more than ENET_MAX_FRAME_SIZE is allowed */
    if(length > ENET_MAX_FRAME_SIZE) {
        return ERROR;
    }

    /* if buffer pointer is null, indicates that users has handled data in application */
    if(NULL != buffer) {
        /* copy frame data from application buffer to Tx buffer */
        for(offset = 0U; offset < length; offset++) {
            (*(__IO uint8_t *)(uint32_t)((dma_current_ptp_txdesc->buffer1_addr) + offset)) = (*(buffer + offset));
        }
    }
    /* set the frame length */
    dma_current_txdesc->control_buffer_size = (length & (uint32_t)0x1FFF);
    /* set the segment of frame, frame is transmitted in one descriptor */
    dma_current_txdesc->status |= ENET_TDES0_LSG | ENET_TDES0_FSG;
    /* enable the DMA transmission */
    dma_current_txdesc->status |= ENET_TDES0_DAV;

    /* check Tx buffer unavailable flag status */
    dma_tbu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TBU);
    dma_tu_flag = (ENET_DMA_STAT & ENET_DMA_STAT_TU);

    if((RESET != dma_tbu_flag) || (RESET != dma_tu_flag)) {
        /* clear TBU and TU flag */
        ENET_DMA_STAT = (dma_tbu_flag | dma_tu_flag);
        /* resume DMA transmission by writing to the TPEN register*/
        ENET_DMA_TPEN = 0U;
    }

    /* if timestamp pointer is null, indicates that users don't care timestamp in application */
    if(NULL != timestamp) {
        /* wait for ENET_TDES0_TTMSS flag to be set, a timestamp was captured */
        do {
            tdes0_ttmss_flag = (dma_current_txdesc->status & ENET_TDES0_TTMSS);
            timeout++;
        } while((RESET == tdes0_ttmss_flag) && (timeout < ENET_DELAY_TO));

        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            return ERROR;
        }

        /* clear the ENET_TDES0_TTMSS flag */
        dma_current_txdesc->status &= ~ENET_TDES0_TTMSS;
        /* get the timestamp value of the transmit frame */
        timestamp[0] = dma_current_txdesc->buffer1_addr;
        timestamp[1] = dma_current_txdesc->buffer2_next_desc_addr;
    }
    dma_current_txdesc->buffer1_addr = dma_current_ptp_txdesc ->buffer1_addr ;
    dma_current_txdesc->buffer2_next_desc_addr = dma_current_ptp_txdesc ->buffer2_next_desc_addr;

    /* update the current TxDMA descriptor pointer to the next decriptor in TxDMA decriptor table */
    /* chained mode */
    if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TCHM)) {
        dma_current_txdesc = (enet_descriptors_struct *)(dma_current_ptp_txdesc->buffer2_next_desc_addr);
        /* if it is the last ptp descriptor */
        if(0U != dma_current_ptp_txdesc->status) {
            /* pointer back to the first ptp descriptor address in the desc_ptptab list address */
            dma_current_ptp_txdesc = (enet_descriptors_struct *)(dma_current_ptp_txdesc->status);
        } else {
            /* ponter to the next ptp descriptor */
            dma_current_ptp_txdesc++;
        }
    } else {
        /* ring mode */
        if((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_TERM)) {
            /* if is the last descriptor in table, the next descriptor is the table header */
            dma_current_txdesc = (enet_descriptors_struct *)(ENET_DMA_TDTADDR);
            /* TDES2 and TDES3 will not be covered by buffer address, so do not need to preserve a new table,
            use the same table with TxDMA descriptor */
            dma_current_ptp_txdesc = (enet_descriptors_struct *)(dma_current_ptp_txdesc->status);
        } else {
            /* the next descriptor is the current address, add the descriptor size, and descriptor skip length */
            dma_current_txdesc = (enet_descriptors_struct *)(uint32_t)((uint32_t)dma_current_txdesc + ETH_DMATXDESC_SIZE + GET_DMA_BCTL_DPSL(ENET_DMA_BCTL));
            dma_current_ptp_txdesc ++;
        }
    }
    return SUCCESS;
}

#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

/*!
    \brief    wakeup frame filter register pointer reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_wum_filter_register_pointer_reset(void)
{
    ENET_MAC_WUM |= ENET_MAC_WUM_WUFFRPR;
}

/*!
    \brief    set the remote wakeup frame registers
    \param[in]  pdata: pointer to buffer data which is written to remote wakeup frame registers (8 words total)
    \param[out] none
    \retval     none
*/
void enet_wum_filter_config(uint32_t pdata[])
{
    uint32_t num = 0U;

    /* configure ENET_MAC_RWFF register */
    for(num = 0U; num < ETH_WAKEUP_REGISTER_LENGTH; num++) {
        ENET_MAC_RWFF = pdata[num];
    }
}

/*!
    \brief    enable wakeup management features
    \param[in]  feature: the wake up type which is selected
                one or more parameters can be selected which are shown as below
      \arg        ENET_WUM_POWER_DOWN: power down mode
      \arg        ENET_WUM_MAGIC_PACKET_FRAME: enable a wakeup event due to magic packet reception
      \arg        ENET_WUM_WAKE_UP_FRAME: enable a wakeup event due to wakeup frame reception
      \arg        ENET_WUM_GLOBAL_UNICAST: any received unicast frame passed filter is considered to be a wakeup frame
    \param[out] none
    \retval     none
*/
void enet_wum_feature_enable(uint32_t feature)
{
    ENET_MAC_WUM |= feature;
}

/*!
    \brief    disable wakeup management features
    \param[in]  feature: the wake up type which is selected
                one or more parameters can be selected which are shown as below
      \arg        ENET_WUM_MAGIC_PACKET_FRAME: enable a wakeup event due to magic packet reception
      \arg        ENET_WUM_WAKE_UP_FRAME: enable a wakeup event due to wakeup frame reception
      \arg        ENET_WUM_GLOBAL_UNICAST: any received unicast frame passed filter is considered to be a wakeup frame
    \param[out] none
    \retval     none
*/
void enet_wum_feature_disable(uint32_t feature)
{
    ENET_MAC_WUM &= (~feature);
}

/*!
    \brief    reset the MAC statistics counters
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_msc_counters_reset(void)
{
    /* reset all counters */
    ENET_MSC_CTL |= ENET_MSC_CTL_CTR;
}

/*!
    \brief    enable the MAC statistics counter features
    \param[in]  feature: the feature of MAC statistics counter
                one or more parameters can be selected which are shown as below
      \arg        ENET_MSC_COUNTER_STOP_ROLLOVER: counter stop rollover
      \arg        ENET_MSC_RESET_ON_READ: reset on read
      \arg        ENET_MSC_COUNTERS_FREEZE: MSC counter freeze
    \param[out] none
    \retval     none
*/
void enet_msc_feature_enable(uint32_t feature)
{
    ENET_MSC_CTL |= feature;
}

/*!
    \brief    disable the MAC statistics counter features
    \param[in]  feature: the feature of MAC statistics counter
                one or more parameters can be selected which are shown as below
      \arg        ENET_MSC_COUNTER_STOP_ROLLOVER: counter stop rollover
      \arg        ENET_MSC_RESET_ON_READ: reset on read
      \arg        ENET_MSC_COUNTERS_FREEZE: MSC counter freeze
    \param[out] none
    \retval     none
*/
void enet_msc_feature_disable(uint32_t feature)
{
    ENET_MSC_CTL &= (~feature);
}

/*!
    \brief    configure MAC statistics counters preset mode
    \param[in]  mode: MSC counters preset mode, refer to enet_msc_preset_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MSC_PRESET_NONE: do not preset MSC counter
      \arg        ENET_MSC_PRESET_HALF: preset all MSC counters to almost-half(0x7FFF FFF0) value
      \arg        ENET_MSC_PRESET_FULL: preset all MSC counters to almost-full(0xFFFF FFF0) value
    \param[out] none
    \retval     none
*/
void enet_msc_counters_preset_config(enet_msc_preset_enum mode)
{
    ENET_MSC_CTL &= ENET_MSC_PRESET_MASK;
    ENET_MSC_CTL |= (uint32_t)mode;
}

/*!
    \brief    get MAC statistics counter
    \param[in]  counter: MSC counters which is selected, refer to enet_msc_counter_enum
                only one parameter can be selected which is shown as below
      \arg        ENET_MSC_TX_SCCNT: MSC transmitted good frames after a single collision counter
      \arg        ENET_MSC_TX_MSCCNT: MSC transmitted good frames after more than a single collision counter
      \arg        ENET_MSC_TX_TGFCNT: MSC transmitted good frames counter
      \arg        ENET_MSC_RX_RFCECNT: MSC received frames with CRC error counter
      \arg        ENET_MSC_RX_RFAECNT: MSC received frames with alignment error counter
      \arg        ENET_MSC_RX_RGUFCNT: MSC received good unicast frames counter
    \param[out] none
    \retval     the MSC counter value
*/
uint32_t enet_msc_counters_get(enet_msc_counter_enum counter)
{
    uint32_t reval;

    reval = REG32((ENET + (uint32_t)counter));

    return reval;
}

/*!
    \brief    enable the PTP features
    \param[in]  feature: the feature of ENET PTP mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_RXTX_TIMESTAMP: timestamp function for transmit and receive frames
      \arg        ENET_PTP_TIMESTAMP_INT: timestamp interrupt trigger
      \arg        ENET_ALL_RX_TIMESTAMP: all received frames are taken snapshot
      \arg        ENET_NONTYPE_FRAME_SNAPSHOT: take snapshot when received non type frame
      \arg        ENET_IPV6_FRAME_SNAPSHOT: take snapshot for IPv6 frame
      \arg        ENET_IPV4_FRAME_SNAPSHOT: take snapshot for IPv4 frame
      \arg        ENET_PTP_FRAME_USE_MACADDRESS_FILTER: use MAC address1-3 to filter the PTP frame
    \param[out] none
    \retval     none
*/
void enet_ptp_feature_enable(uint32_t feature)
{
    ENET_PTP_TSCTL |= feature;
}

/*!
    \brief    disable the PTP features
    \param[in]  feature: the feature of ENET PTP mode
                one or more parameters can be selected which are shown as below
      \arg        ENET_RXTX_TIMESTAMP: timestamp function for transmit and receive frames
      \arg        ENET_PTP_TIMESTAMP_INT: timestamp interrupt trigger
      \arg        ENET_ALL_RX_TIMESTAMP: all received frames are taken snapshot
      \arg        ENET_NONTYPE_FRAME_SNAPSHOT: take snapshot when received non type frame
      \arg        ENET_IPV6_FRAME_SNAPSHOT: take snapshot for IPv6 frame
      \arg        ENET_IPV4_FRAME_SNAPSHOT: take snapshot for IPv4 frame
      \arg        ENET_PTP_FRAME_USE_MACADDRESS_FILTER: use MAC address1-3 to filter the PTP frame
    \param[out] none
    \retval     none
*/
void enet_ptp_feature_disable(uint32_t feature)
{
    ENET_PTP_TSCTL &= ~feature;
}

/*!
    \brief    configure the PTP timestamp function
    \param[in]  func: the function of PTP timestamp
                only one parameter can be selected which is shown as below
      \arg        ENET_CKNT_ORDINARY: type of ordinary clock node type for timestamp
      \arg        ENET_CKNT_BOUNDARY: type of boundary clock node type for timestamp
      \arg        ENET_CKNT_END_TO_END: type of end-to-end transparent clock node type for timestamp
      \arg        ENET_CKNT_PEER_TO_PEER: type of peer-to-peer transparent clock node type for timestamp
      \arg        ENET_PTP_ADDEND_UPDATE: addend register update
      \arg        ENET_PTP_SYSTIME_UPDATE: timestamp update
      \arg        ENET_PTP_SYSTIME_INIT: timestamp initialize
      \arg        ENET_PTP_FINEMODE: the system timestamp uses the fine method for updating
      \arg        ENET_PTP_COARSEMODE: the system timestamp uses the coarse method for updating
      \arg        ENET_SUBSECOND_DIGITAL_ROLLOVER: digital rollover mode
      \arg        ENET_SUBSECOND_BINARY_ROLLOVER: binary rollover mode
      \arg        ENET_SNOOPING_PTP_VERSION_2: version 2
      \arg        ENET_SNOOPING_PTP_VERSION_1: version 1
      \arg        ENET_EVENT_TYPE_MESSAGES_SNAPSHOT: only event type messages are taken snapshot
      \arg        ENET_ALL_TYPE_MESSAGES_SNAPSHOT: all type messages are taken snapshot except announce,
                                                   management and signaling message
      \arg        ENET_MASTER_NODE_MESSAGE_SNAPSHOT: snapshot is only take for master node message
      \arg        ENET_SLAVE_NODE_MESSAGE_SNAPSHOT: snapshot is only taken for slave node message
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus enet_ptp_timestamp_function_config(enet_ptp_function_enum func)
{
    uint32_t temp_config = 0U, temp_state = 0U;
    uint32_t timeout = 0U;
    ErrStatus enet_state = SUCCESS;

    switch(func) {
    case ENET_CKNT_ORDINARY:
    case ENET_CKNT_BOUNDARY:
    case ENET_CKNT_END_TO_END:
    case ENET_CKNT_PEER_TO_PEER:
        ENET_PTP_TSCTL &= ~ENET_PTP_TSCTL_CKNT;
        ENET_PTP_TSCTL |= (uint32_t)func;
        break;
    case ENET_PTP_ADDEND_UPDATE:
        /* this bit must be read as zero before application set it */
        do {
            temp_state = ENET_PTP_TSCTL & ENET_PTP_TSCTL_TMSARU;
            timeout++;
        } while((RESET != temp_state) && (timeout < ENET_DELAY_TO));
        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            enet_state = ERROR;
        } else {
            ENET_PTP_TSCTL |= ENET_PTP_TSCTL_TMSARU;
        }
        break;
    case ENET_PTP_SYSTIME_UPDATE:
        /* both the TMSSTU and TMSSTI bits must be read as zero before application set this bit */
        do {
            temp_state = ENET_PTP_TSCTL & (ENET_PTP_TSCTL_TMSSTU | ENET_PTP_TSCTL_TMSSTI);
            timeout++;
        } while((RESET != temp_state) && (timeout < ENET_DELAY_TO));
        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            enet_state = ERROR;
        } else {
            ENET_PTP_TSCTL |= ENET_PTP_TSCTL_TMSSTU;
        }
        break;
    case ENET_PTP_SYSTIME_INIT:
        /* this bit must be read as zero before application set it */
        do {
            temp_state = ENET_PTP_TSCTL & ENET_PTP_TSCTL_TMSSTI;
            timeout++;
        } while((RESET != temp_state) && (timeout < ENET_DELAY_TO));
        /* return ERROR due to timeout */
        if(ENET_DELAY_TO == timeout) {
            enet_state = ERROR;
        } else {
            ENET_PTP_TSCTL |= ENET_PTP_TSCTL_TMSSTI;
        }
        break;
    default:
        temp_config = (uint32_t)func & (~BIT(31));
        if(RESET != ((uint32_t)func & BIT(31))) {
            ENET_PTP_TSCTL |= temp_config;
        } else {
            ENET_PTP_TSCTL &= ~temp_config;
        }
        break;
    }

    return enet_state;
}

/*!
    \brief    configure system time subsecond increment value
    \param[in]  subsecond: the value will be added to the subsecond value of system time(0x00000000 - 0x000000FF)
    \param[out] none
    \retval     none
*/
void enet_ptp_subsecond_increment_config(uint32_t subsecond)
{
    ENET_PTP_SSINC = PTP_SSINC_STMSSI(subsecond);
}

/*!
    \brief    adjusting the clock frequency only in fine update mode
    \param[in]  add: the value will be added to the accumulator register to achieve time synchronization
    \param[out] none
    \retval     none
*/
void enet_ptp_timestamp_addend_config(uint32_t add)
{
    ENET_PTP_TSADDEND = add;
}

/*!
    \brief    initialize or add/subtract to second of the system time
    \param[in]  sign: timestamp update positive or negative sign
                only one parameter can be selected which is shown as below
      \arg        ENET_PTP_ADD_TO_TIME: timestamp update value is added to system time
      \arg        ENET_PTP_SUBSTRACT_FROM_TIME: timestamp update value is subtracted from system time
    \param[in]  second: initializing or adding/subtracting to second of the system time
    \param[in]  subsecond: the current subsecond of the system time
                with 0.46 ns accuracy if required accuracy is 20 ns
    \param[out] none
    \retval     none
*/
void enet_ptp_timestamp_update_config(uint32_t sign, uint32_t second, uint32_t subsecond)
{
    ENET_PTP_TSUH = second;
    ENET_PTP_TSUL = sign | PTP_TSUL_TMSUSS(subsecond);
}

/*!
    \brief    configure the expected target time
    \param[in]  second: the expected target second time
    \param[in]  nanosecond: the expected target nanosecond time (signed)
    \param[out] none
    \retval     none
*/
void enet_ptp_expected_time_config(uint32_t second, uint32_t nanosecond)
{
    ENET_PTP_ETH = second;
    ENET_PTP_ETL = nanosecond;
}

/*!
    \brief    get the current system time
    \param[in]  none
    \param[out] systime_struct: pointer to a enet_ptp_systime_struct structure which contains
                parameters of PTP system time
                members of the structure and the member values are shown as below:
                  second: 0x0 - 0xFFFF FFFF
                  subsecond: 0x0 - 0x7FFF FFFF
                  sign: ENET_PTP_TIME_POSITIVE, ENET_PTP_TIME_NEGATIVE
    \retval     none
*/
void enet_ptp_system_time_get(enet_ptp_systime_struct *systime_struct)
{
    uint32_t temp_sec = 0U, temp_subs = 0U;

    /* get the value of sysytem time registers */
    temp_sec = (uint32_t)ENET_PTP_TSH;
    temp_subs = (uint32_t)ENET_PTP_TSL;

    /* get sysytem time and construct the enet_ptp_systime_struct structure */
    systime_struct->second = temp_sec;
    systime_struct->subsecond = GET_PTP_TSL_STMSS(temp_subs);
    systime_struct->sign = GET_PTP_TSL_STS(temp_subs);
}

/*!
    \brief    configure the PPS output frequency
    \param[in]  freq: PPS output frequency
                only one parameter can be selected which is shown as below
      \arg        ENET_PPSOFC_1HZ: PPS output 1Hz frequency
      \arg        ENET_PPSOFC_2HZ: PPS output 2Hz frequency
      \arg        ENET_PPSOFC_4HZ: PPS output 4Hz frequency
      \arg        ENET_PPSOFC_8HZ: PPS output 8Hz frequency
      \arg        ENET_PPSOFC_16HZ: PPS output 16Hz frequency
      \arg        ENET_PPSOFC_32HZ: PPS output 32Hz frequency
      \arg        ENET_PPSOFC_64HZ: PPS output 64Hz frequency
      \arg        ENET_PPSOFC_128HZ: PPS output 128Hz frequency
      \arg        ENET_PPSOFC_256HZ: PPS output 256Hz frequency
      \arg        ENET_PPSOFC_512HZ: PPS output 512Hz frequency
      \arg        ENET_PPSOFC_1024HZ: PPS output 1024Hz frequency
      \arg        ENET_PPSOFC_2048HZ: PPS output 2048Hz frequency
      \arg        ENET_PPSOFC_4096HZ: PPS output 4096Hz frequency
      \arg        ENET_PPSOFC_8192HZ: PPS output 8192Hz frequency
      \arg        ENET_PPSOFC_16384HZ: PPS output 16384Hz frequency
      \arg        ENET_PPSOFC_32768HZ: PPS output 32768Hz frequency
    \param[out] none
    \retval     none
*/
void enet_ptp_pps_output_frequency_config(uint32_t freq)
{
    ENET_PTP_PPSCTL = freq;
}

/*!
    \brief    reset the ENET initpara struct, call it before using enet_initpara_config()
    \param[in]  none
    \param[out] none
    \retval     none
*/
void enet_initpara_reset(void)
{
    enet_initpara.option_enable = 0U;
    enet_initpara.forward_frame = 0U;
    enet_initpara.dmabus_mode = 0U;
    enet_initpara.dma_maxburst = 0U;
    enet_initpara.dma_arbitration = 0U;
    enet_initpara.store_forward_mode = 0U;
    enet_initpara.dma_function = 0U;
    enet_initpara.vlan_config = 0U;
    enet_initpara.flow_control = 0U;
    enet_initpara.hashtable_high = 0U;
    enet_initpara.hashtable_low = 0U;
    enet_initpara.framesfilter_mode = 0U;
    enet_initpara.halfduplex_param = 0U;
    enet_initpara.timer_config = 0U;
    enet_initpara.interframegap = 0U;
}

/*!
    \brief    initialize ENET peripheral with generally concerned parameters, call it by enet_init()
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_default_init(void)
{
    uint32_t reg_value = 0U;

    /* MAC */
    /* configure ENET_MAC_CFG register */
    reg_value = ENET_MAC_CFG;
    reg_value &= MAC_CFG_MASK;
    reg_value |= ENET_WATCHDOG_ENABLE | ENET_JABBER_ENABLE | ENET_INTERFRAMEGAP_96BIT \
                 | ENET_SPEEDMODE_10M | ENET_MODE_HALFDUPLEX | ENET_LOOPBACKMODE_DISABLE \
                 | ENET_CARRIERSENSE_DISABLE | ENET_RECEIVEOWN_ENABLE \
                 | ENET_RETRYTRANSMISSION_ENABLE | ENET_BACKOFFLIMIT_10 \
                 | ENET_DEFERRALCHECK_DISABLE \
                 | ENET_TYPEFRAME_CRC_DROP_DISABLE \
                 | ENET_AUTO_PADCRC_DROP_DISABLE \
                 | ENET_CHECKSUMOFFLOAD_DISABLE;
    ENET_MAC_CFG = reg_value;

    /* configure ENET_MAC_FRMF register */
    ENET_MAC_FRMF = ENET_SRC_FILTER_DISABLE | ENET_DEST_FILTER_INVERSE_DISABLE \
                    | ENET_MULTICAST_FILTER_PERFECT | ENET_UNICAST_FILTER_PERFECT \
                    | ENET_PCFRM_PREVENT_ALL | ENET_BROADCASTFRAMES_ENABLE \
                    | ENET_PROMISCUOUS_DISABLE | ENET_RX_FILTER_ENABLE;

    /* configure ENET_MAC_HLH, ENET_MAC_HLL register */
    ENET_MAC_HLH = 0x0U;

    ENET_MAC_HLL = 0x0U;

    /* configure ENET_MAC_FCTL, ENET_MAC_FCTH register */
    reg_value = ENET_MAC_FCTL;
    reg_value &= MAC_FCTL_MASK;
    reg_value |= MAC_FCTL_PTM(0) | ENET_ZERO_QUANTA_PAUSE_DISABLE \
                 | ENET_PAUSETIME_MINUS4 | ENET_UNIQUE_PAUSEDETECT \
                 | ENET_RX_FLOWCONTROL_DISABLE | ENET_TX_FLOWCONTROL_DISABLE;
    ENET_MAC_FCTL = reg_value;

    /* configure ENET_MAC_VLT register */
    ENET_MAC_VLT = ENET_VLANTAGCOMPARISON_16BIT | MAC_VLT_VLTI(0);

    /* disable MAC interrupt */
    ENET_MAC_INTMSK |= ENET_MAC_INTMSK_TMSTIM | ENET_MAC_INTMSK_WUMIM;

    /* MSC */
    /* disable MSC Rx interrupt */
    ENET_MSC_RINTMSK |= ENET_MSC_RINTMSK_RFAEIM | ENET_MSC_RINTMSK_RFCEIM \
                        | ENET_MSC_RINTMSK_RGUFIM;

    /* disable MSC Tx interrupt */
    ENET_MSC_TINTMSK |= ENET_MSC_TINTMSK_TGFIM | ENET_MSC_TINTMSK_TGFMSCIM \
                        | ENET_MSC_TINTMSK_TGFSCIM;

    /* DMA */
    /* configure ENET_DMA_CTL register */
    reg_value = ENET_DMA_CTL;
    reg_value &= DMA_CTL_MASK;
    reg_value |= ENET_TCPIP_CKSUMERROR_DROP | ENET_RX_MODE_STOREFORWARD \
                 | ENET_FLUSH_RXFRAME_ENABLE | ENET_TX_MODE_STOREFORWARD \
                 | ENET_TX_THRESHOLD_64BYTES | ENET_RX_THRESHOLD_64BYTES \
                 | ENET_SECONDFRAME_OPT_DISABLE;
    ENET_DMA_CTL = reg_value;

    /* configure ENET_DMA_BCTL register */
    reg_value = ENET_DMA_BCTL;
    reg_value &= DMA_BCTL_MASK;
    reg_value = ENET_ADDRESS_ALIGN_ENABLE | ENET_ARBITRATION_RXTX_2_1 \
                | ENET_RXDP_32BEAT | ENET_PGBL_32BEAT | ENET_RXTX_DIFFERENT_PGBL \
                | ENET_FIXED_BURST_ENABLE | ENET_MIXED_BURST_DISABLE \
                | ENET_NORMAL_DESCRIPTOR;
    ENET_DMA_BCTL = reg_value;
}

#ifndef USE_DELAY
/*!
    \brief    insert a delay time
    \param[in]  ncount: specifies the delay time length
    \param[out] none
    \param[out] none
*/
static void enet_delay(uint32_t ncount)
{
    __IO uint32_t delay_time = 0U;

    for(delay_time = ncount; delay_time != 0U; delay_time--) {
    }
}
#endif /* USE_DELAY */
