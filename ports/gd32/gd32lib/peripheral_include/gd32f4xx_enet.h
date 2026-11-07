/*!
    \file    gd32f4xx_enet.h
    \brief   definitions for the ENET

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

#ifndef GD32F4XX_ENET_H
#define GD32F4XX_ENET_H

#include "gd32f4xx.h"
#include <stdlib.h>

#define IF_USE_EXTERNPHY_LIB             0
#if (1 == IF_USE_EXTERNPHY_LIB)
#include "phy.h"
#endif

#ifndef ENET_RXBUF_NUM
#define ENET_RXBUF_NUM                   5U                                     /*!< ethernet Rx DMA descriptor number */
#endif

#ifndef ENET_TXBUF_NUM
#define ENET_TXBUF_NUM                   5U                                     /*!< ethernet Tx DMA descriptor number */
#endif

#ifndef ENET_RXBUF_SIZE
#define ENET_RXBUF_SIZE                  ENET_MAX_FRAME_SIZE                    /*!< ethernet receive buffer size */
#endif

#ifndef ENET_TXBUF_SIZE
#define ENET_TXBUF_SIZE                  ENET_MAX_FRAME_SIZE                    /*!< ethernet transmit buffer size */
#endif

//#define SELECT_DESCRIPTORS_ENHANCED_MODE 

//#define USE_DELAY

#ifndef _PHY_H_
#define DP83848                          0
#define LAN8700                          1
#define SR8201F                          2
#define RTL8201F                         2
#define YT8512                           3
#define PHY_TYPE                         DP83848

#define PHY_ADDRESS                      ((uint16_t)1U)                         /*!< phy address determined by the hardware */

/* PHY read write timeouts */ 
#define PHY_READ_TO                      ((uint32_t)0x0004FFFFU)                /*!< PHY read timeout */
#define PHY_WRITE_TO                     ((uint32_t)0x0004FFFFU)                /*!< PHY write timeout */

/* PHY delay */
#define PHY_RESETDELAY                   ((uint32_t)0x008FFFFFU)                /*!< PHY reset delay */
#define PHY_CONFIGDELAY                  ((uint32_t)0x00FFFFFFU)                /*!< PHY configure delay */

/* PHY register address */ 
#define PHY_REG_BCR                      0U                                     /*!< tranceiver basic control register */
#define PHY_REG_BSR                      1U                                     /*!< tranceiver basic status register */

/* PHY basic control register */
#define PHY_RESET                        ((uint16_t)0x8000)                     /*!< PHY reset */
#define PHY_LOOPBACK                     ((uint16_t)0x4000)                     /*!< enable phy loop-back mode */
#define PHY_FULLDUPLEX_100M              ((uint16_t)0x2100)                     /*!< configure speed to 100 Mbit/s and the full-duplex mode */
#define PHY_HALFDUPLEX_100M              ((uint16_t)0x2000)                     /*!< configure speed to 100 Mbit/s and the half-duplex mode */
#define PHY_FULLDUPLEX_10M               ((uint16_t)0x0100)                     /*!< configure speed to 10 Mbit/s and the full-duplex mode */
#define PHY_HALFDUPLEX_10M               ((uint16_t)0x0000)                     /*!< configure speed to 10 Mbit/s and the half-duplex mode */
#define PHY_AUTONEGOTIATION              ((uint16_t)0x1000)                     /*!< enable auto-negotiation function */
#define PHY_RESTART_AUTONEGOTIATION      ((uint16_t)0x0200)                     /*!< restart auto-negotiation function */
#define PHY_POWERDOWN                    ((uint16_t)0x0800)                     /*!< enable the power down mode */
#define PHY_ISOLATE                      ((uint16_t)0x0400)                     /*!< isolate PHY from MII */

/* PHY basic status register */
#define PHY_AUTONEGO_COMPLETE            ((uint16_t)0x0020)                     /*!< auto-negotioation process completed */
#define PHY_LINKED_STATUS                ((uint16_t)0x0004)                     /*!< valid link established */
#define PHY_JABBER_DETECTION             ((uint16_t)0x0002)                     /*!< jabber condition detected */

#if(PHY_TYPE == LAN8700) 
#define PHY_SR                           31U                                    /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x0004)                     /*!< configured information of speed: 10Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0010)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == DP83848)
#define PHY_SR                           16U                                    /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x0002)                     /*!< configured information of speed: 10Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0004)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == SR8201F)
#define PHY_SR                           PHY_REG_BCR                            /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x2000)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0100)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == YT8512)
#define PHY_SR                           17U                                    /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x4000)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x2000)                     /*!< configured information of duplex: full-duplex */
#endif /* PHY_TYPE */

#endif /* _PHY_H_ */


/* ENET definitions */
#define ENET                             ENET_BASE

/* registers definitions */
#define ENET_MAC_CFG                     REG32((ENET) + 0x0000U)                  /*!< ethernet MAC configuration register */
#define ENET_MAC_FRMF                    REG32((ENET) + 0x0004U)                  /*!< ethernet MAC frame filter register */
#define ENET_MAC_HLH                     REG32((ENET) + 0x0008U)                  /*!< ethernet MAC hash list high register */
#define ENET_MAC_HLL                     REG32((ENET) + 0x000CU)                  /*!< ethernet MAC hash list low register */
#define ENET_MAC_PHY_CTL                 REG32((ENET) + 0x0010U)                  /*!< ethernet MAC PHY control register */
#define ENET_MAC_PHY_DATA                REG32((ENET) + 0x0014U)                  /*!< ethernet MAC PHY data register */
#define ENET_MAC_FCTL                    REG32((ENET) + 0x0018U)                  /*!< ethernet MAC flow control register */
#define ENET_MAC_VLT                     REG32((ENET) + 0x001CU)                  /*!< ethernet MAC VLAN tag register */
#define ENET_MAC_RWFF                    REG32((ENET) + 0x0028U)                  /*!< ethernet MAC remote wakeup frame filter register */
#define ENET_MAC_WUM                     REG32((ENET) + 0x002CU)                  /*!< ethernet MAC wakeup management register */
#define ENET_MAC_DBG                     REG32((ENET) + 0x0034U)                  /*!< ethernet MAC debug register */
#define ENET_MAC_INTF                    REG32((ENET) + 0x0038U)                  /*!< ethernet MAC interrupt flag register */
#define ENET_MAC_INTMSK                  REG32((ENET) + 0x003CU)                  /*!< ethernet MAC interrupt mask register */
#define ENET_MAC_ADDR0H                  REG32((ENET) + 0x0040U)                  /*!< ethernet MAC address 0 high register */
#define ENET_MAC_ADDR0L                  REG32((ENET) + 0x0044U)                  /*!< ethernet MAC address 0 low register */
#define ENET_MAC_ADDR1H                  REG32((ENET) + 0x0048U)                  /*!< ethernet MAC address 1 high register */
#define ENET_MAC_ADDR1L                  REG32((ENET) + 0x004CU)                  /*!< ethernet MAC address 1 low register */
#define ENET_MAC_ADDT2H                  REG32((ENET) + 0x0050U)                  /*!< ethernet MAC address 2 high register */
#define ENET_MAC_ADDR2L                  REG32((ENET) + 0x0054U)                  /*!< ethernet MAC address 2 low register */
#define ENET_MAC_ADDR3H                  REG32((ENET) + 0x0058U)                  /*!< ethernet MAC address 3 high register */
#define ENET_MAC_ADDR3L                  REG32((ENET) + 0x005CU)                  /*!< ethernet MAC address 3 low register */
#define ENET_MAC_FCTH                    REG32((ENET) + 0x1080U)                /*!< ethernet MAC flow control threshold register */

#define ENET_MSC_CTL                     REG32((ENET) + 0x0100U)                 /*!< ethernet MSC control register */
#define ENET_MSC_RINTF                   REG32((ENET) + 0x0104U)                 /*!< ethernet MSC receive interrupt flag register */
#define ENET_MSC_TINTF                   REG32((ENET) + 0x0108U)                 /*!< ethernet MSC transmit interrupt flag register */
#define ENET_MSC_RINTMSK                 REG32((ENET) + 0x010CU)                 /*!< ethernet MSC receive interrupt mask register */
#define ENET_MSC_TINTMSK                 REG32((ENET) + 0x0110U)                 /*!< ethernet MSC transmit interrupt mask register */
#define ENET_MSC_SCCNT                   REG32((ENET) + 0x014CU)                 /*!< ethernet MSC transmitted good frames after a single collision counter register */
#define ENET_MSC_MSCCNT                  REG32((ENET) + 0x0150U)                 /*!< ethernet MSC transmitted good frames after more than a single collision counter register */
#define ENET_MSC_TGFCNT                  REG32((ENET) + 0x0168U)                 /*!< ethernet MSC transmitted good frames counter register */
#define ENET_MSC_RFCECNT                 REG32((ENET) + 0x0194U)                 /*!< ethernet MSC received frames with CRC error counter register */
#define ENET_MSC_RFAECNT                 REG32((ENET) + 0x0198U)                 /*!< ethernet MSC received frames with alignment error counter register */
#define ENET_MSC_RGUFCNT                 REG32((ENET) + 0x01C4U)                 /*!< ethernet MSC received good unicast frames counter register */

#define ENET_PTP_TSCTL                   REG32((ENET) + 0x0700U)                 /*!< ethernet PTP time stamp control register */
#define ENET_PTP_SSINC                   REG32((ENET) + 0x0704U)                 /*!< ethernet PTP subsecond increment register */ 
#define ENET_PTP_TSH                     REG32((ENET) + 0x0708U)                 /*!< ethernet PTP time stamp high register */
#define ENET_PTP_TSL                     REG32((ENET) + 0x070CU)                 /*!< ethernet PTP time stamp low register */
#define ENET_PTP_TSUH                    REG32((ENET) + 0x0710U)                 /*!< ethernet PTP time stamp update high register */
#define ENET_PTP_TSUL                    REG32((ENET) + 0x0714U)                 /*!< ethernet PTP time stamp update low register */
#define ENET_PTP_TSADDEND                REG32((ENET) + 0x0718U)                 /*!< ethernet PTP time stamp addend register */
#define ENET_PTP_ETH                     REG32((ENET) + 0x071CU)                 /*!< ethernet PTP expected time high register */
#define ENET_PTP_ETL                     REG32((ENET) + 0x0720U)                 /*!< ethernet PTP expected time low register */
#define ENET_PTP_TSF                     REG32((ENET) + 0x0728U)                 /*!< ethernet PTP time stamp flag register */
#define ENET_PTP_PPSCTL                  REG32((ENET) + 0x072CU)                 /*!< ethernet PTP PPS control register */

#define ENET_DMA_BCTL                    REG32((ENET) + 0x1000U)                /*!< ethernet DMA bus control register */
#define ENET_DMA_TPEN                    REG32((ENET) + 0x1004U)                /*!< ethernet DMA transmit poll enable register */ 
#define ENET_DMA_RPEN                    REG32((ENET) + 0x1008U)                /*!< ethernet DMA receive poll enable register */
#define ENET_DMA_RDTADDR                 REG32((ENET) + 0x100CU)                /*!< ethernet DMA receive descriptor table address register */
#define ENET_DMA_TDTADDR                 REG32((ENET) + 0x1010U)                /*!< ethernet DMA transmit descriptor table address register */
#define ENET_DMA_STAT                    REG32((ENET) + 0x1014U)                /*!< ethernet DMA status register */
#define ENET_DMA_CTL                     REG32((ENET) + 0x1018U)                /*!< ethernet DMA control register */
#define ENET_DMA_INTEN                   REG32((ENET) + 0x101CU)                /*!< ethernet DMA interrupt enable register */
#define ENET_DMA_MFBOCNT                 REG32((ENET) + 0x1020U)                /*!< ethernet DMA missed frame and buffer overflow counter register */
#define ENET_DMA_RSWDC                   REG32((ENET) + 0x1024U)                /*!< ethernet DMA receive state watchdog counter register */
#define ENET_DMA_CTDADDR                 REG32((ENET) + 0x1048U)                /*!< ethernet DMA current transmit descriptor address register */ 
#define ENET_DMA_CRDADDR                 REG32((ENET) + 0x104CU)                /*!< ethernet DMA current receive descriptor address register */
#define ENET_DMA_CTBADDR                 REG32((ENET) + 0x1050U)                /*!< ethernet DMA current transmit buffer address register */
#define ENET_DMA_CRBADDR                 REG32((ENET) + 0x1054U)                /*!< ethernet DMA current receive buffer address register */

/* bits definitions */
/* ENET_MAC_CFG */
#define ENET_MAC_CFG_REN                 BIT(2)                                 /*!< receiver enable */
#define ENET_MAC_CFG_TEN                 BIT(3)                                 /*!< transmitter enable */
#define ENET_MAC_CFG_DFC                 BIT(4)                                 /*!< defferal check */
#define ENET_MAC_CFG_BOL                 BITS(5,6)                              /*!< back-off limit */
#define ENET_MAC_CFG_APCD                BIT(7)                                 /*!< automatic pad/CRC drop */
#define ENET_MAC_CFG_RTD                 BIT(9)                                 /*!< retry disable */
#define ENET_MAC_CFG_IPFCO               BIT(10)                                /*!< IP frame checksum offload */
#define ENET_MAC_CFG_DPM                 BIT(11)                                /*!< duplex mode */
#define ENET_MAC_CFG_LBM                 BIT(12)                                /*!< loopback mode */
#define ENET_MAC_CFG_ROD                 BIT(13)                                /*!< receive own disable */
#define ENET_MAC_CFG_SPD                 BIT(14)                                /*!< fast eneternet speed */
#define ENET_MAC_CFG_CSD                 BIT(16)                                /*!< carrier sense disable */
#define ENET_MAC_CFG_IGBS                BITS(17,19)                            /*!< inter-frame gap bit selection */            
#define ENET_MAC_CFG_JBD                 BIT(22)                                /*!< jabber disable */
#define ENET_MAC_CFG_WDD                 BIT(23)                                /*!< watchdog disable */
#define ENET_MAC_CFG_TFCD                BIT(25)                                /*!< type frame CRC dropping */

/* ENET_MAC_FRMF */
#define ENET_MAC_FRMF_PM                 BIT(0)                                 /*!< promiscuous mode */
#define ENET_MAC_FRMF_HUF                BIT(1)                                 /*!< hash unicast filter */
#define ENET_MAC_FRMF_HMF                BIT(2)                                 /*!< hash multicast filter */ 
#define ENET_MAC_FRMF_DAIFLT             BIT(3)                                 /*!< destination address inverse filtering enable */ 
#define ENET_MAC_FRMF_MFD                BIT(4)                                 /*!< multicast filter disable */ 
#define ENET_MAC_FRMF_BFRMD              BIT(5)                                 /*!< broadcast frame disable */ 
#define ENET_MAC_FRMF_PCFRM              BITS(6,7)                              /*!< pass control frames */ 
#define ENET_MAC_FRMF_SAIFLT             BIT(8)                                 /*!< source address inverse filtering */ 
#define ENET_MAC_FRMF_SAFLT              BIT(9)                                 /*!< source address filter */ 
#define ENET_MAC_FRMF_HPFLT              BIT(10)                                /*!< hash or perfect filter */ 
#define ENET_MAC_FRMF_FAR                BIT(31)                                /*!< frames all receive */ 
  
/* ENET_MAC_HLH */
#define ENET_MAC_HLH_HLH                 BITS(0,31)                             /*!< hash list high */
  
/* ENET_MAC_HLL */
#define ENET_MAC_HLL_HLL                 BITS(0,31)                             /*!< hash list low */
  
/* ENET_MAC_PHY_CTL */
#define ENET_MAC_PHY_CTL_PB              BIT(0)                                 /*!< PHY busy */ 
#define ENET_MAC_PHY_CTL_PW              BIT(1)                                 /*!< PHY write */ 
#define ENET_MAC_PHY_CTL_CLR             BITS(2,4)                              /*!< clock range */ 
#define ENET_MAC_PHY_CTL_PR              BITS(6,10)                             /*!< PHY register */ 
#define ENET_MAC_PHY_CTL_PA              BITS(11,15)                            /*!< PHY address */ 
    
/* ENET_MAC_PHY_DATA */
#define ENET_MAC_PHY_DATA_PD             BITS(0,15)                             /*!< PHY data */
  
/* ENET_MAC_FCTL */
#define ENET_MAC_FCTL_FLCBBKPA           BIT(0)                                 /*!< flow control busy(in full duplex mode)/backpressure activate(in half duplex mode) */
#define ENET_MAC_FCTL_TFCEN              BIT(1)                                 /*!< transmit flow control enable */
#define ENET_MAC_FCTL_RFCEN              BIT(2)                                 /*!< receive flow control enable */
#define ENET_MAC_FCTL_UPFDT              BIT(3)                                 /*!< unicast pause frame detect */
#define ENET_MAC_FCTL_PLTS               BITS(4,5)                              /*!< pause low threshold */     
#define ENET_MAC_FCTL_DZQP               BIT(7)                                 /*!< disable zero-quanta pause */
#define ENET_MAC_FCTL_PTM                BITS(16,31)                            /*!< pause time */
  
/* ENET_MAC_VLT */
#define ENET_MAC_VLT_VLTI                BITS(0,15)                             /*!< VLAN tag identifier(for receive frames) */
#define ENET_MAC_VLT_VLTC                BIT(16)                                /*!< 12-bit VLAN tag comparison */
  
/* ENET_MAC_RWFF */
#define ENET_MAC_RWFF_DATA               BITS(0,31)                             /*!< wakeup frame filter register data */
  
/* ENET_MAC_WUM */ 
#define ENET_MAC_WUM_PWD                 BIT(0)                                 /*!< power down */
#define ENET_MAC_WUM_MPEN                BIT(1)                                 /*!< magic packet enable */
#define ENET_MAC_WUM_WFEN                BIT(2)                                 /*!< wakeup frame enable */
#define ENET_MAC_WUM_MPKR                BIT(5)                                 /*!< magic packet received */
#define ENET_MAC_WUM_WUFR                BIT(6)                                 /*!< wakeup frame received */
#define ENET_MAC_WUM_GU                  BIT(9)                                 /*!< global unicast */
#define ENET_MAC_WUM_WUFFRPR             BIT(31)                                /*!< wakeup frame filter register pointer reset */

/* ENET_MAC_DBG */ 
#define ENET_MAC_DBG_MRNI                BIT(0)                                 /*!< MAC receive state not idle */
#define ENET_MAC_DBG_RXAFS               BITS(1,2)                              /*!< Rx asynchronous FIFO status */
#define ENET_MAC_DBG_RXFW                BIT(4)                                 /*!< RxFIFO is writing */
#define ENET_MAC_DBG_RXFRS               BITS(5,6)                              /*!< RxFIFO read operation status */
#define ENET_MAC_DBG_RXFS                BITS(8,9)                              /*!< RxFIFO state */
#define ENET_MAC_DBG_MTNI                BIT(16)                                /*!< MAC transmit state not idle */
#define ENET_MAC_DBG_SOMT                BITS(17,18)                            /*!< status of mac transmitter */
#define ENET_MAC_DBG_PCS                 BIT(19)                                /*!< pause condition status */
#define ENET_MAC_DBG_TXFRS               BITS(20,21)                            /*!< TxFIFO read operation status */
#define ENET_MAC_DBG_TXFW                BIT(22)                                /*!< TxFIFO is writing */
#define ENET_MAC_DBG_TXFNE               BIT(24)                                /*!< TxFIFO not empty flag */
#define ENET_MAC_DBG_TXFF                BIT(25)                                /*!< TxFIFO full flag */

/* ENET_MAC_INTF */ 
#define ENET_MAC_INTF_WUM                BIT(3)                                 /*!< WUM status */
#define ENET_MAC_INTF_MSC                BIT(4)                                 /*!< MSC status */
#define ENET_MAC_INTF_MSCR               BIT(5)                                 /*!< MSC receive status */
#define ENET_MAC_INTF_MSCT               BIT(6)                                 /*!< MSC transmit status */
#define ENET_MAC_INTF_TMST               BIT(9)                                 /*!< timestamp trigger status */

/* ENET_MAC_INTMSK */
#define ENET_MAC_INTMSK_WUMIM            BIT(3)                                 /*!< WUM interrupt mask */
#define ENET_MAC_INTMSK_TMSTIM           BIT(9)                                 /*!< timestamp trigger interrupt mask */

/* ENET_MAC_ADDR0H */
#define ENET_MAC_ADDR0H_ADDR0H           BITS(0,15)                             /*!< MAC address0 high */
#define ENET_MAC_ADDR0H_MO               BIT(31)                                /*!< always read 1 and must be kept */
  
/* ENET_MAC_ADDR0L */
#define ENET_MAC_ADDR0L_ADDR0L           BITS(0,31)                             /*!< MAC address0 low */
  
/* ENET_MAC_ADDR1H */
#define ENET_MAC_ADDR1H_ADDR1H           BITS(0,15)                             /*!< MAC address1 high */
#define ENET_MAC_ADDR1H_MB               BITS(24,29)                            /*!< mask byte */ 
#define ENET_MAC_ADDR1H_SAF              BIT(30)                                /*!< source address filter */
#define ENET_MAC_ADDR1H_AFE              BIT(31)                                /*!< address filter enable */
  
/* ENET_MAC_ADDR1L */
#define ENET_MAC_ADDR1L_ADDR1L           BITS(0,31)                             /*!< MAC address1 low */
  
/* ENET_MAC_ADDR2H */
#define ENET_MAC_ADDR2H_ADDR2H           BITS(0,15)                             /*!< MAC address2 high */
#define ENET_MAC_ADDR2H_MB               BITS(24,29)                            /*!< mask byte */
#define ENET_MAC_ADDR2H_SAF              BIT(30)                                /*!< source address filter */
#define ENET_MAC_ADDR2H_AFE              BIT(31)                                /*!< address filter enable */
  
/* ENET_MAC_ADDR2L */
#define ENET_MAC_ADDR2L_ADDR2L           BITS(0,31)                             /*!< MAC address2 low */
  
/* ENET_MAC_ADDR3H */
#define ENET_MAC_ADDR3H_ADDR3H           BITS(0,15)                             /*!< MAC address3 high */
#define ENET_MAC_ADDR3H_MB               BITS(24,29)                            /*!< mask byte */
#define ENET_MAC_ADDR3H_SAF              BIT(30)                                /*!< source address filter */
#define ENET_MAC_ADDR3H_AFE              BIT(31)                                /*!< address filter enable */

/* ENET_MAC_ADDR3L */
#define ENET_MAC_ADDR3L_ADDR3L           BITS(0,31)                             /*!< MAC address3 low */
  
/* ENET_MAC_FCTH */
#define ENET_MAC_FCTH_RFA                BITS(0,2)                              /*!< threshold of active flow control */
#define ENET_MAC_FCTH_RFD                BITS(4,6)                              /*!< threshold of deactive flow control */
 
/* ENET_MSC_CTL */
#define ENET_MSC_CTL_CTR                 BIT(0)                                 /*!< counter reset */
#define ENET_MSC_CTL_CTSR                BIT(1)                                 /*!< counter stop rollover */
#define ENET_MSC_CTL_RTOR                BIT(2)                                 /*!< reset on read */
#define ENET_MSC_CTL_MCFZ                BIT(3)                                 /*!< MSC counter freeze */
#define ENET_MSC_CTL_PMC                 BIT(4)                                 /*!< preset MSC counter */
#define ENET_MSC_CTL_AFHPM               BIT(5)                                 /*!< almost full or half preset mode */

/* ENET_MSC_RINTF */
#define ENET_MSC_RINTF_RFCE              BIT(5)                                 /*!< received frames CRC error */
#define ENET_MSC_RINTF_RFAE              BIT(6)                                 /*!< received frames alignment error */
#define ENET_MSC_RINTF_RGUF              BIT(17)                                /*!< receive good unicast frames */
  
/* ENET_MSC_TINTF */
#define ENET_MSC_TINTF_TGFSC             BIT(14)                                /*!< transmitted good frames single collision */
#define ENET_MSC_TINTF_TGFMSC            BIT(15)                                /*!< transmitted good frames more single collision */
#define ENET_MSC_TINTF_TGF               BIT(21)                                /*!< transmitted good frames */

/* ENET_MSC_RINTMSK */
#define ENET_MSC_RINTMSK_RFCEIM          BIT(5)                                 /*!< received frame CRC error interrupt mask */
#define ENET_MSC_RINTMSK_RFAEIM          BIT(6)                                 /*!< received frames alignment error interrupt mask */
#define ENET_MSC_RINTMSK_RGUFIM          BIT(17)                                /*!< received good unicast frames interrupt mask */
  
/* ENET_MSC_TINTMSK */
#define ENET_MSC_TINTMSK_TGFSCIM         BIT(14)                                /*!< transmitted good frames single collision interrupt mask */
#define ENET_MSC_TINTMSK_TGFMSCIM        BIT(15)                                /*!< transmitted good frames more single collision interrupt mask */
#define ENET_MSC_TINTMSK_TGFIM           BIT(21)                                /*!< transmitted good frames interrupt mask */
  
/* ENET_MSC_SCCNT */
#define ENET_MSC_SCCNT_SCC               BITS(0,31)                             /*!< transmitted good frames single collision counter */
  
/* ENET_MSC_MSCCNT */
#define ENET_MSC_MSCCNT_MSCC             BITS(0,31)                             /*!< transmitted good frames more one single collision counter */
  
/* ENET_MSC_TGFCNT */
#define ENET_MSC_TGFCNT_TGF              BITS(0,31)                             /*!< transmitted good frames counter */
  
/* ENET_MSC_RFCECNT */
#define ENET_MSC_RFCECNT_RFCER           BITS(0,31)                             /*!< received frames with CRC error counter */
  
/* ENET_MSC_RFAECNT */
#define ENET_MSC_RFAECNT_RFAER           BITS(0,31)                             /*!< received frames alignment error counter */
  
/* ENET_MSC_RGUFCNT */
#define ENET_MSC_RGUFCNT_RGUF            BITS(0,31)                             /*!< received good unicast frames counter */
   
/* ENET_PTP_TSCTL */
#define PTP_TSCTL_CKNT(regval)           (BITS(16,17) & ((uint32_t)(regval) << 16))    /*!< write value to ENET_PTP_TSCTL_CKNT bit field */

#define ENET_PTP_TSCTL_TMSEN             BIT(0)                                 /*!< timestamp enable */
#define ENET_PTP_TSCTL_TMSFCU            BIT(1)                                 /*!< timestamp fine or coarse update */
#define ENET_PTP_TSCTL_TMSSTI            BIT(2)                                 /*!< timestamp system time initialize */
#define ENET_PTP_TSCTL_TMSSTU            BIT(3)                                 /*!< timestamp system time update */
#define ENET_PTP_TSCTL_TMSITEN           BIT(4)                                 /*!< timestamp interrupt trigger enable */
#define ENET_PTP_TSCTL_TMSARU            BIT(5)                                 /*!< timestamp addend register update */
#define ENET_PTP_TSCTL_ARFSEN            BIT(8)                                 /*!< all received frames snapshot enable */
#define ENET_PTP_TSCTL_SCROM             BIT(9)                                 /*!< subsecond counter rollover mode */
#define ENET_PTP_TSCTL_PFSV              BIT(10)                                /*!< PTP frame snooping version */
#define ENET_PTP_TSCTL_ESEN              BIT(11)                                /*!< received Ethernet snapshot enable */
#define ENET_PTP_TSCTL_IP6SEN            BIT(12)                                /*!< received IPv6 snapshot enable */
#define ENET_PTP_TSCTL_IP4SEN            BIT(13)                                /*!< received IPv4 snapshot enable */
#define ENET_PTP_TSCTL_ETMSEN            BIT(14)                                /*!< received event type message snapshot enable */
#define ENET_PTP_TSCTL_MNMSEN            BIT(15)                                /*!< received master node message snapshot enable */
#define ENET_PTP_TSCTL_CKNT              BITS(16,17)                            /*!< clock node type for time stamp */
#define ENET_PTP_TSCTL_MAFEN             BIT(18)                                /*!< MAC address filter enable for PTP frame */
  
/* ENET_PTP_SSINC */
#define ENET_PTP_SSINC_STMSSI            BITS(0,7)                              /*!< system time subsecond increment */
  
/* ENET_PTP_TSH */
#define ENET_PTP_TSH_STMS                BITS(0,31)                             /*!< system time second */
  
/* ENET_PTP_TSL */
#define ENET_PTP_TSL_STMSS               BITS(0,30)                             /*!< system time subseconds */
#define ENET_PTP_TSL_STS                 BIT(31)                                /*!< system time sign */
  
/* ENET_PTP_TSUH */
#define ENET_PTP_TSUH_TMSUS              BITS(0,31)                             /*!< timestamp update seconds */
  
/* ENET_PTP_TSUL */
#define ENET_PTP_TSUL_TMSUSS             BITS(0,30)                             /*!< timestamp update subseconds */
#define ENET_PTP_TSUL_TMSUPNS            BIT(31)                                /*!< timestamp update positive or negative sign */

/* ENET_PTP_TSADDAND */
#define ENET_PTP_TSADDAND_TMSA           BITS(0,31)                             /*!< timestamp addend */
  
/* ENET_PTP_ETH */
#define ENET_PTP_ETH_ETSH                BITS(0,31)                             /*!< expected time high */
  
/* ENET_PTP_ETL */
#define ENET_PTP_ETL_ETSL                BITS(0,31)                             /*!< expected time low */
  
/* ENET_PTP_TSF */
#define ENET_PTP_TSF_TSSCO               BIT(0)                                 /*!< timestamp second counter overflow */
#define ENET_PTP_TSF_TTM                 BIT(1)                                 /*!< target time match */
  
/* ENET_PTP_PPSCTL */
#define ENET_PTP_PPSCTL_PPSOFC           BITS(0,3)                              /*!< PPS output frequency configure */

/* ENET_DMA_BCTL */
#define ENET_DMA_BCTL_SWR                BIT(0)                                 /*!< software reset */
#define ENET_DMA_BCTL_DAB                BIT(1)                                 /*!< DMA arbitration */
#define ENET_DMA_BCTL_DPSL               BITS(2,6)                              /*!< descriptor skip length */
#define ENET_DMA_BCTL_DFM                BIT(7)                                 /*!< descriptor format mode */
#define ENET_DMA_BCTL_PGBL               BITS(8,13)                             /*!< programmable burst length */
#define ENET_DMA_BCTL_RTPR               BITS(14,15)                            /*!< RxDMA and TxDMA transfer priority ratio */
#define ENET_DMA_BCTL_FB                 BIT(16)                                /*!< fixed Burst */
#define ENET_DMA_BCTL_RXDP               BITS(17,22)                            /*!< RxDMA PGBL */
#define ENET_DMA_BCTL_UIP                BIT(23)                                /*!< use independent PGBL */
#define ENET_DMA_BCTL_FPBL               BIT(24)                                /*!< four times PGBL mode */
#define ENET_DMA_BCTL_AA                 BIT(25)                                /*!< address-aligned */
#define ENET_DMA_BCTL_MB                 BIT(26)                                /*!< mixed burst */
  
/* ENET_DMA_TPEN */
#define ENET_DMA_TPEN_TPE                BITS(0,31)                             /*!< transmit poll enable */
  
/* ENET_DMA_RPEN */
#define ENET_DMA_RPEN_RPE                BITS(0,31)                             /*!< receive poll enable  */

/* ENET_DMA_RDTADDR */
#define ENET_DMA_RDTADDR_SRT             BITS(0,31)                             /*!< start address of receive table */
  
/* ENET_DMA_TDTADDR */
#define ENET_DMA_TDTADDR_STT             BITS(0,31)                             /*!< start address of transmit table */
  
/* ENET_DMA_STAT */
#define ENET_DMA_STAT_TS                 BIT(0)                                 /*!< transmit status */
#define ENET_DMA_STAT_TPS                BIT(1)                                 /*!< transmit process stopped status */
#define ENET_DMA_STAT_TBU                BIT(2)                                 /*!< transmit buffer unavailable status */
#define ENET_DMA_STAT_TJT                BIT(3)                                 /*!< transmit jabber timeout status */
#define ENET_DMA_STAT_RO                 BIT(4)                                 /*!< receive overflow status */
#define ENET_DMA_STAT_TU                 BIT(5)                                 /*!< transmit underflow status */
#define ENET_DMA_STAT_RS                 BIT(6)                                 /*!< receive status */
#define ENET_DMA_STAT_RBU                BIT(7)                                 /*!< receive buffer unavailable status */
#define ENET_DMA_STAT_RPS                BIT(8)                                 /*!< receive process stopped status */
#define ENET_DMA_STAT_RWT                BIT(9)                                 /*!< receive watchdog timeout status */
#define ENET_DMA_STAT_ET                 BIT(10)                                /*!< early transmit status */
#define ENET_DMA_STAT_FBE                BIT(13)                                /*!< fatal bus error status */
#define ENET_DMA_STAT_ER                 BIT(14)                                /*!< early receive status */
#define ENET_DMA_STAT_AI                 BIT(15)                                /*!< abnormal interrupt summary */
#define ENET_DMA_STAT_NI                 BIT(16)                                /*!< normal interrupt summary */
#define ENET_DMA_STAT_RP                 BITS(17,19)                            /*!< receive process state */
#define ENET_DMA_STAT_TP                 BITS(20,22)                            /*!< transmit process state */
#define ENET_DMA_STAT_EB                 BITS(23,25)                            /*!< error bits status */
#define ENET_DMA_STAT_MSC                BIT(27)                                /*!< MSC status */
#define ENET_DMA_STAT_WUM                BIT(28)                                /*!< WUM status */
#define ENET_DMA_STAT_TST                BIT(29)                                /*!< timestamp trigger status */
 
/* ENET_DMA_CTL */
#define ENET_DMA_CTL_SRE                 BIT(1)                                 /*!< start/stop receive enable */
#define ENET_DMA_CTL_OSF                 BIT(2)                                 /*!< operate on second frame */
#define ENET_DMA_CTL_RTHC                BITS(3,4)                              /*!< receive threshold control */
#define ENET_DMA_CTL_FUF                 BIT(6)                                 /*!< forward undersized good frames */
#define ENET_DMA_CTL_FERF                BIT(7)                                 /*!< forward error frames */
#define ENET_DMA_CTL_STE                 BIT(13)                                /*!< start/stop transmission enable */
#define ENET_DMA_CTL_TTHC                BITS(14,16)                            /*!< transmit threshold control */
#define ENET_DMA_CTL_FTF                 BIT(20)                                /*!< flush transmit FIFO */
#define ENET_DMA_CTL_TSFD                BIT(21)                                /*!< transmit store-and-forward */
#define ENET_DMA_CTL_DAFRF               BIT(24)                                /*!< disable flushing of received frames */
#define ENET_DMA_CTL_RSFD                BIT(25)                                /*!< receive store-and-forward */
#define ENET_DMA_CTL_DTCERFD             BIT(26)                                /*!< dropping of TCP/IP checksum error frames disable */
  
/* ENET_DMA_INTEN */
#define ENET_DMA_INTEN_TIE               BIT(0)                                 /*!< transmit interrupt enable */
#define ENET_DMA_INTEN_TPSIE             BIT(1)                                 /*!< transmit process stopped interrupt enable */
#define ENET_DMA_INTEN_TBUIE             BIT(2)                                 /*!< transmit buffer unavailable interrupt enable */
#define ENET_DMA_INTEN_TJTIE             BIT(3)                                 /*!< transmit jabber timeout interrupt enable */
#define ENET_DMA_INTEN_ROIE              BIT(4)                                 /*!< receive overflow interrupt enable */
#define ENET_DMA_INTEN_TUIE              BIT(5)                                 /*!< transmit underflow interrupt enable */
#define ENET_DMA_INTEN_RIE               BIT(6)                                 /*!< receive interrupt enable */
#define ENET_DMA_INTEN_RBUIE             BIT(7)                                 /*!< receive buffer unavailable interrupt enable */
#define ENET_DMA_INTEN_RPSIE             BIT(8)                                 /*!< receive process stopped interrupt enable */
#define ENET_DMA_INTEN_RWTIE             BIT(9)                                 /*!< receive watchdog timeout interrupt enable */
#define ENET_DMA_INTEN_ETIE              BIT(10)                                /*!< early transmit interrupt enable */
#define ENET_DMA_INTEN_FBEIE             BIT(13)                                /*!< fatal bus error interrupt enable */
#define ENET_DMA_INTEN_ERIE              BIT(14)                                /*!< early receive interrupt enable */
#define ENET_DMA_INTEN_AIE               BIT(15)                                /*!< abnormal interrupt summary enable */
#define ENET_DMA_INTEN_NIE               BIT(16)                                /*!< normal interrupt summary enable */
  
/* ENET_DMA_MFBOCNT */
#define ENET_DMA_MFBOCNT_MSFC            BITS(0,15)                             /*!< missed frames by the controller */
#define ENET_DMA_MFBOCNT_MSFA            BITS(17,27)                            /*!< missed frames by the application */

/* ENET_DMA_RSWDC */
#define ENET_DMA_RSWDC_WDCFRS            BITS(0,7)                              /*!< watchdog counter for receive status (RS) */

/* ENET_DMA_CTDADDR */
#define ENET_DMA_CTDADDR_TDAP            BITS(0,31)                             /*!< transmit descriptor address pointer */

/* ENET_DMA_CRDADDR */
#define ENET_DMA_CRDADDR_RDAP            BITS(0,31)                             /*!< receive descriptor address pointer */
  
/* ENET_DMA_CTBADDR */
#define ENET_DMA_CTBADDR_TBAP            BITS(0,31)                             /*!< transmit buffer address pointer */
  
/* ENET_DMA_CRBADDR */
#define ENET_DMA_CRBADDR_RBAP            BITS(0,31)                             /*!< receive buffer address pointer */

/* ENET DMA Tx descriptor TDES0 */
#define ENET_TDES0_DB                    BIT(0)                                 /*!< deferred */
#define ENET_TDES0_UFE                   BIT(1)                                 /*!< underflow error */
#define ENET_TDES0_EXD                   BIT(2)                                 /*!< excessive deferral */
#define ENET_TDES0_COCNT                 BITS(3,6)                              /*!< collision count */
#define ENET_TDES0_VFRM                  BIT(7)                                 /*!< VLAN frame */
#define ENET_TDES0_ECO                   BIT(8)                                 /*!< excessive collision */
#define ENET_TDES0_LCO                   BIT(9)                                 /*!< late collision */
#define ENET_TDES0_NCA                   BIT(10)                                /*!< no carrier */
#define ENET_TDES0_LCA                   BIT(11)                                /*!< loss of carrier */
#define ENET_TDES0_IPPE                  BIT(12)                                /*!< IP payload error */
#define ENET_TDES0_FRMF                  BIT(13)                                /*!< frame flushed */
#define ENET_TDES0_JT                    BIT(14)                                /*!< jabber timeout */
#define ENET_TDES0_ES                    BIT(15)                                /*!< error summary */
#define ENET_TDES0_IPHE                  BIT(16)                                /*!< IP header error */
#define ENET_TDES0_TTMSS                 BIT(17)                                /*!< transmit timestamp status */
#define ENET_TDES0_TCHM                  BIT(20)                                /*!< the second address chained mode */
#define ENET_TDES0_TERM                  BIT(21)                                /*!< transmit end of ring mode*/
#define ENET_TDES0_CM                    BITS(22,23)                            /*!< checksum mode */
#define ENET_TDES0_TTSEN                 BIT(25)                                /*!< transmit timestamp function enable */
#define ENET_TDES0_DPAD                  BIT(26)                                /*!< disable adding pad */
#define ENET_TDES0_DCRC                  BIT(27)                                /*!< disable CRC */
#define ENET_TDES0_FSG                   BIT(28)                                /*!< first segment */
#define ENET_TDES0_LSG                   BIT(29)                                /*!< last segment */
#define ENET_TDES0_INTC                  BIT(30)                                /*!< interrupt on completion */
#define ENET_TDES0_DAV                   BIT(31)                                /*!< DAV bit */

/* ENET DMA Tx descriptor TDES1 */
#define ENET_TDES1_TB1S                  BITS(0,12)                             /*!< transmit buffer 1 size */
#define ENET_TDES1_TB2S                  BITS(16,28)                            /*!< transmit buffer 2 size */

/* ENET DMA Tx descriptor TDES2 */
#define ENET_TDES2_TB1AP                 BITS(0,31)                             /*!< transmit buffer 1 address pointer/transmit frame timestamp low 32-bit value */

/* ENET DMA Tx descriptor TDES3 */
#define ENET_TDES3_TB2AP                 BITS(0,31)                             /*!< transmit buffer 2 address pointer (or next descriptor address) / transmit frame timestamp high 32-bit value */

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
/* ENET DMA Tx descriptor TDES6 */
#define ENET_TDES6_TTSL                  BITS(0,31)                             /*!< transmit frame timestamp low 32-bit value */

/* ENET DMA Tx descriptor TDES7 */
#define ENET_TDES7_TTSH                  BITS(0,31)                             /*!< transmit frame timestamp high 32-bit value */
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

/* ENET DMA Rx descriptor RDES0 */
#define ENET_RDES0_PCERR                 BIT(0)                                 /*!< payload checksum error */
#define ENET_RDES0_EXSV                  BIT(0)                                 /*!< extended status valid */
#define ENET_RDES0_CERR                  BIT(1)                                 /*!< CRC error */
#define ENET_RDES0_DBERR                 BIT(2)                                 /*!< dribble bit error */
#define ENET_RDES0_RERR                  BIT(3)                                 /*!< receive error */
#define ENET_RDES0_RWDT                  BIT(4)                                 /*!< receive watchdog timeout */
#define ENET_RDES0_FRMT                  BIT(5)                                 /*!< frame type */
#define ENET_RDES0_LCO                   BIT(6)                                 /*!< late collision */
#define ENET_RDES0_IPHERR                BIT(7)                                 /*!< IP frame header error */
#define ENET_RDES0_TSV                   BIT(7)                                 /*!< timestamp valid */
#define ENET_RDES0_LDES                  BIT(8)                                 /*!< last descriptor */ 
#define ENET_RDES0_FDES                  BIT(9)                                 /*!< first descriptor */
#define ENET_RDES0_VTAG                  BIT(10)                                /*!< VLAN tag */
#define ENET_RDES0_OERR                  BIT(11)                                /*!< overflow Error */
#define ENET_RDES0_LERR                  BIT(12)                                /*!< length error */
#define ENET_RDES0_SAFF                  BIT(13)                                /*!< SA filter fail */
#define ENET_RDES0_DERR                  BIT(14)                                /*!< descriptor error */
#define ENET_RDES0_ERRS                  BIT(15)                                /*!< error summary */
#define ENET_RDES0_FRML                  BITS(16,29)                            /*!< frame length */
#define ENET_RDES0_DAFF                  BIT(30)                                /*!< destination address filter fail */
#define ENET_RDES0_DAV                   BIT(31)                                /*!< descriptor available */

/* ENET DMA Rx descriptor RDES1 */ 
#define ENET_RDES1_RB1S                  BITS(0,12)                             /*!< receive buffer 1 size */
#define ENET_RDES1_RCHM                  BIT(14)                                /*!< receive chained mode for second address */
#define ENET_RDES1_RERM                  BIT(15)                                /*!< receive end of ring mode*/
#define ENET_RDES1_RB2S                  BITS(16,28)                            /*!< receive buffer 2 size */
#define ENET_RDES1_DINTC                 BIT(31)                                /*!< disable interrupt on completion */

/* ENET DMA Rx descriptor RDES2 */
#define ENET_RDES2_RB1AP                 BITS(0,31)                             /*!< receive buffer 1 address pointer / receive frame timestamp low 32-bit */

/* ENET DMA Rx descriptor RDES3 */
#define ENET_RDES3_RB2AP                 BITS(0,31)                             /*!< receive buffer 2 address pointer (next descriptor address)/receive frame timestamp high 32-bit value */

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
/* ENET DMA Rx descriptor RDES4 */
#define ENET_RDES4_IPPLDT                BITS(0,2)                              /*!< IP frame payload type */
#define ENET_RDES4_IPHERR                BIT(3)                                 /*!< IP frame header error */
#define ENET_RDES4_IPPLDERR              BIT(4)                                 /*!< IP frame payload error */
#define ENET_RDES4_IPCKSB                BIT(5)                                 /*!< IP frame checksum bypassed */
#define ENET_RDES4_IPF4                  BIT(6)                                 /*!< IP frame in version 4 */
#define ENET_RDES4_IPF6                  BIT(7)                                 /*!< IP frame in version 6 */
#define ENET_RDES4_PTPMT                 BITS(8,11)                             /*!< PTP message type */
#define ENET_RDES4_PTPOEF                BIT(12)                                /*!< PTP on ethernet frame */
#define ENET_RDES4_PTPVF                 BIT(13)                                /*!< PTP version format */

/* ENET DMA Rx descriptor RDES6 */
#define ENET_RDES6_RTSL                  BITS(0,31)                             /*!< receive frame timestamp low 32-bit value */

/* ENET DMA Rx descriptor RDES7 */
#define ENET_RDES7_RTSH                  BITS(0,31)                             /*!< receive frame timestamp high 32-bit value */
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

/* constants definitions */
/* define bit position and its register index offset */
#define ENET_REGIDX_BIT(regidx, bitpos)  (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define ENET_REG_VAL(periph)             (REG32(ENET + ((uint32_t)(periph) >> 6)))
#define ENET_BIT_POS(val)                ((uint32_t)(val) & 0x1FU)

/* ENET clock range judgement */
#define ENET_RANGE(hclk, n, m)           (((hclk) >= (n))&&((hclk) < (m)))

/* define MAC address configuration and reference address */
#define ENET_SET_MACADDRH(p)             (((uint32_t)(p)[5] << 8) | (uint32_t)(p)[4])         
#define ENET_SET_MACADDRL(p)             (((uint32_t)(p)[3] << 24) | ((uint32_t)(p)[2] << 16) | ((uint32_t)(p)[1] << 8) | (uint32_t)(p)[0])
#define ENET_ADDRH_BASE                  ((ENET) + 0x40U)
#define ENET_ADDRL_BASE                  ((ENET) + 0x44U)
#define ENET_GET_MACADDR(offset, n)      ((uint8_t)((REG32((ENET_ADDRL_BASE + (offset)) - (((n) / 4U) * 4U)) >> (8U * ((n) % 4U))) & 0xFFU))

/* register offset */
#define MAC_FCTL_REG_OFFSET              ((uint16_t)0x0018U)                                /*!< MAC flow control register offset */
#define MAC_WUM_REG_OFFSET               ((uint16_t)0x002CU)                                /*!< MAC wakeup management register offset */
#define MAC_INTF_REG_OFFSET              ((uint16_t)0x0038U)                                /*!< MAC interrupt flag register offset */
#define MAC_INTMSK_REG_OFFSET            ((uint16_t)0x003CU)                                /*!< MAC interrupt mask register offset */

#define MSC_RINTF_REG_OFFSET             ((uint16_t)0x0104U)                                /*!< MSC receive interrupt flag register offset */
#define MSC_TINTF_REG_OFFSET             ((uint16_t)0x0108U)                                /*!< MSC transmit interrupt flag register offset */
#define MSC_RINTMSK_REG_OFFSET           ((uint16_t)0x010CU)                                /*!< MSC receive interrupt mask register offset */
#define MSC_TINTMSK_REG_OFFSET           ((uint16_t)0x0110U)                                /*!< MSC transmit interrupt mask register offset */
#define MSC_SCCNT_REG_OFFSET             ((uint16_t)0x014CU)                                /*!< MSC transmitted good frames after a single collision counter register offset */
#define MSC_MSCCNT_REG_OFFSET            ((uint16_t)0x0150U)                                /*!< MSC transmitted good frames after more than a single collision counter register offset */
#define MSC_TGFCNT_REG_OFFSET            ((uint16_t)0x0168U)                                /*!< MSC transmitted good frames counter register offset */
#define MSC_RFCECNT_REG_OFFSET           ((uint16_t)0x0194U)                                /*!< MSC received frames with CRC error counter register offset */
#define MSC_RFAECNT_REG_OFFSET           ((uint16_t)0x0198U)                                /*!< MSC received frames with alignment error counter register offset */
#define MSC_RGUFCNT_REG_OFFSET           ((uint16_t)0x01C4U)                                /*!< MSC received good unicast frames counter register offset */
                                                                           
#define PTP_TSF_REG_OFFSET               ((uint16_t)0x0728U)                                /*!< PTP time stamp flag register offset */

#define DMA_STAT_REG_OFFSET              ((uint16_t)0x1014U)                                /*!< DMA status register offset */
#define DMA_INTEN_REG_OFFSET             ((uint16_t)0x101CU)                                /*!< DMA interrupt enable register offset */
#define DMA_TDTADDR_REG_OFFSET           ((uint16_t)0x1010U)                                /*!< DMA transmit descriptor table address register offset */
#define DMA_CTDADDR_REG_OFFSET           ((uint16_t)0x1048U)                                /*!< DMA current transmit descriptor address register */
#define DMA_CTBADDR_REG_OFFSET           ((uint16_t)0x1050U)                                /*!< DMA current transmit buffer address register */
#define DMA_RDTADDR_REG_OFFSET           ((uint16_t)0x100CU)                                /*!< DMA receive descriptor table address register */
#define DMA_CRDADDR_REG_OFFSET           ((uint16_t)0x104CU)                                /*!< DMA current receive descriptor address register */
#define DMA_CRBADDR_REG_OFFSET           ((uint16_t)0x1054U)                                /*!< DMA current receive buffer address register */

/* ENET status flag get */
typedef enum
{
    /* ENET_MAC_WUM register */
    ENET_MAC_FLAG_MPKR              = ENET_REGIDX_BIT(MAC_WUM_REG_OFFSET, 5U),      /*!< magic packet received flag */
    ENET_MAC_FLAG_WUFR              = ENET_REGIDX_BIT(MAC_WUM_REG_OFFSET, 6U),      /*!< wakeup frame received flag */ 
    /* ENET_MAC_FCTL register */
    ENET_MAC_FLAG_FLOWCONTROL       = ENET_REGIDX_BIT(MAC_FCTL_REG_OFFSET, 0U),     /*!< flow control status flag */
    /* ENET_MAC_INTF register */
    ENET_MAC_FLAG_WUM               = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 3U),     /*!< WUM status flag */
    ENET_MAC_FLAG_MSC               = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 4U),     /*!< MSC status flag */
    ENET_MAC_FLAG_MSCR              = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 5U),     /*!< MSC receive status flag */
    ENET_MAC_FLAG_MSCT              = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 6U),     /*!< MSC transmit status flag */
    ENET_MAC_FLAG_TMST              = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 9U),     /*!< timestamp trigger status flag */
    /* ENET_PTP_TSF register */
    ENET_PTP_FLAG_TSSCO             = ENET_REGIDX_BIT(PTP_TSF_REG_OFFSET, 0U),      /*!< timestamp second counter overflow flag */  
    ENET_PTP_FLAG_TTM               = ENET_REGIDX_BIT(PTP_TSF_REG_OFFSET, 1U),      /*!< target time match flag */
    /* ENET_MSC_RINTF register */
    ENET_MSC_FLAG_RFCE              = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 5U),    /*!< received frames CRC error flag */
    ENET_MSC_FLAG_RFAE              = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 6U),    /*!< received frames alignment error flag */
    ENET_MSC_FLAG_RGUF              = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 17U),   /*!< received good unicast frames flag */
    /* ENET_MSC_TINTF register */ 
    ENET_MSC_FLAG_TGFSC             = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 14U),   /*!< transmitted good frames single collision flag */
    ENET_MSC_FLAG_TGFMSC            = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 15U),   /*!< transmitted good frames more single collision flag */
    ENET_MSC_FLAG_TGF               = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 21U),   /*!< transmitted good frames flag */
    /* ENET_DMA_STAT register */
    ENET_DMA_FLAG_TS                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 0U),     /*!< transmit status flag */
    ENET_DMA_FLAG_TPS               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 1U),     /*!< transmit process stopped status flag */
    ENET_DMA_FLAG_TBU               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 2U),     /*!< transmit buffer unavailable status flag */
    ENET_DMA_FLAG_TJT               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 3U),     /*!< transmit jabber timeout status flag */
    ENET_DMA_FLAG_RO                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 4U),     /*!< receive overflow status flag */
    ENET_DMA_FLAG_TU                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 5U),     /*!< transmit underflow status flag */
    ENET_DMA_FLAG_RS                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 6U),     /*!< receive status flag */
    ENET_DMA_FLAG_RBU               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 7U),     /*!< receive buffer unavailable status flag */
    ENET_DMA_FLAG_RPS               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 8U),     /*!< receive process stopped status flag */
    ENET_DMA_FLAG_RWT               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 9U),     /*!< receive watchdog timeout status flag */
    ENET_DMA_FLAG_ET                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 10U),    /*!< early transmit status flag */
    ENET_DMA_FLAG_FBE               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 13U),    /*!< fatal bus error status flag */
    ENET_DMA_FLAG_ER                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 14U),    /*!< early receive status flag */
    ENET_DMA_FLAG_AI                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 15U),    /*!< abnormal interrupt summary flag */
    ENET_DMA_FLAG_NI                = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 16U),    /*!< normal interrupt summary flag */
    ENET_DMA_FLAG_EB_DMA_ERROR      = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 23U),    /*!< error during data transfer by RxDMA/TxDMA flag */
    ENET_DMA_FLAG_EB_TRANSFER_ERROR = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 24U),    /*!< error during write/read transfer flag */
    ENET_DMA_FLAG_EB_ACCESS_ERROR   = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 25U),    /*!< error during data buffer/descriptor access flag */
    ENET_DMA_FLAG_MSC               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 27U),    /*!< MSC status flag */
    ENET_DMA_FLAG_WUM               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 28U),    /*!< WUM status flag */
    ENET_DMA_FLAG_TST               = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 29U),    /*!< timestamp trigger status flag */                        
}enet_flag_enum;

/* ENET stutus flag clear */
typedef enum
{
    /* ENET_DMA_STAT register */
    ENET_DMA_FLAG_TS_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 0U),     /*!< transmit status flag */
    ENET_DMA_FLAG_TPS_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 1U),     /*!< transmit process stopped status flag */
    ENET_DMA_FLAG_TBU_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 2U),     /*!< transmit buffer unavailable status flag */
    ENET_DMA_FLAG_TJT_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 3U),     /*!< transmit jabber timeout status flag */
    ENET_DMA_FLAG_RO_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 4U),     /*!< receive overflow status flag */
    ENET_DMA_FLAG_TU_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 5U),     /*!< transmit underflow status flag */
    ENET_DMA_FLAG_RS_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 6U),     /*!< receive status flag */
    ENET_DMA_FLAG_RBU_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 7U),     /*!< receive buffer unavailable status flag */
    ENET_DMA_FLAG_RPS_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 8U),     /*!< receive process stopped status flag */
    ENET_DMA_FLAG_RWT_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 9U),     /*!< receive watchdog timeout status flag */
    ENET_DMA_FLAG_ET_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 10U),    /*!< early transmit status flag */
    ENET_DMA_FLAG_FBE_CLR           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 13U),    /*!< fatal bus error status flag */
    ENET_DMA_FLAG_ER_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 14U),    /*!< early receive status flag */
    ENET_DMA_FLAG_AI_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 15U),    /*!< abnormal interrupt summary flag */
    ENET_DMA_FLAG_NI_CLR            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 16U),    /*!< normal interrupt summary flag */                       
}enet_flag_clear_enum;

/* ENET interrupt enable/disable */
typedef enum
{
    /* ENET_MAC_INTMSK register */
    ENET_MAC_INT_WUMIM              = ENET_REGIDX_BIT(MAC_INTMSK_REG_OFFSET, 3U),   /*!< WUM interrupt mask */
    ENET_MAC_INT_TMSTIM             = ENET_REGIDX_BIT(MAC_INTMSK_REG_OFFSET, 9U),   /*!< timestamp trigger interrupt mask */
    /* ENET_MSC_RINTMSK register */ 
    ENET_MSC_INT_RFCEIM             = ENET_REGIDX_BIT(MSC_RINTMSK_REG_OFFSET, 5U),  /*!< received frame CRC error interrupt mask */
    ENET_MSC_INT_RFAEIM             = ENET_REGIDX_BIT(MSC_RINTMSK_REG_OFFSET, 6U),  /*!< received frames alignment error interrupt mask */
    ENET_MSC_INT_RGUFIM             = ENET_REGIDX_BIT(MSC_RINTMSK_REG_OFFSET, 17U), /*!< received good unicast frames interrupt mask */
    /* ENET_MSC_TINTMSK register */ 
    ENET_MSC_INT_TGFSCIM            = ENET_REGIDX_BIT(MSC_TINTMSK_REG_OFFSET, 14U), /*!< transmitted good frames single collision interrupt mask */
    ENET_MSC_INT_TGFMSCIM           = ENET_REGIDX_BIT(MSC_TINTMSK_REG_OFFSET, 15U), /*!< transmitted good frames more single collision interrupt mask */
    ENET_MSC_INT_TGFIM              = ENET_REGIDX_BIT(MSC_TINTMSK_REG_OFFSET, 21U), /*!< transmitted good frames interrupt mask */
    /* ENET_DMA_INTEN register */ 
    ENET_DMA_INT_TIE                = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 0U),    /*!< transmit interrupt enable */
    ENET_DMA_INT_TPSIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 1U),    /*!< transmit process stopped interrupt enable */
    ENET_DMA_INT_TBUIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 2U),    /*!< transmit buffer unavailable interrupt enable */
    ENET_DMA_INT_TJTIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 3U),    /*!< transmit jabber timeout interrupt enable */
    ENET_DMA_INT_ROIE               = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 4U),    /*!< receive overflow interrupt enable */
    ENET_DMA_INT_TUIE               = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 5U),    /*!< transmit underflow interrupt enable */
    ENET_DMA_INT_RIE                = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 6U),    /*!< receive interrupt enable */
    ENET_DMA_INT_RBUIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 7U),    /*!< receive buffer unavailable interrupt enable */
    ENET_DMA_INT_RPSIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 8U),    /*!< receive process stopped interrupt enable */
    ENET_DMA_INT_RWTIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 9U),    /*!< receive watchdog timeout interrupt enable */
    ENET_DMA_INT_ETIE               = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 10U),   /*!< early transmit interrupt enable */
    ENET_DMA_INT_FBEIE              = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 13U),   /*!< fatal bus error interrupt enable */
    ENET_DMA_INT_ERIE               = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 14U),   /*!< early receive interrupt enable */
    ENET_DMA_INT_AIE                = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 15U),   /*!< abnormal interrupt summary enable */
    ENET_DMA_INT_NIE                = ENET_REGIDX_BIT(DMA_INTEN_REG_OFFSET, 16U),   /*!< normal interrupt summary enable */
}enet_int_enum;
 
/* ENET interrupt flag get */
typedef enum
{
    /* ENET_MAC_INTF register */
    ENET_MAC_INT_FLAG_WUM           = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 3U),     /*!< WUM status flag */
    ENET_MAC_INT_FLAG_MSC           = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 4U),     /*!< MSC status flag */
    ENET_MAC_INT_FLAG_MSCR          = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 5U),     /*!< MSC receive status flag */
    ENET_MAC_INT_FLAG_MSCT          = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 6U),     /*!< MSC transmit status flag */
    ENET_MAC_INT_FLAG_TMST          = ENET_REGIDX_BIT(MAC_INTF_REG_OFFSET, 9U),     /*!< timestamp trigger status flag */
    /* ENET_MSC_RINTF register */
    ENET_MSC_INT_FLAG_RFCE          = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 5U),    /*!< received frames CRC error flag */
    ENET_MSC_INT_FLAG_RFAE          = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 6U),    /*!< received frames alignment error flag */
    ENET_MSC_INT_FLAG_RGUF          = ENET_REGIDX_BIT(MSC_RINTF_REG_OFFSET, 17U),   /*!< received good unicast frames flag */
    /* ENET_MSC_TINTF register */
    ENET_MSC_INT_FLAG_TGFSC         = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 14U),   /*!< transmitted good frames single collision flag */
    ENET_MSC_INT_FLAG_TGFMSC        = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 15U),   /*!< transmitted good frames more single collision flag */
    ENET_MSC_INT_FLAG_TGF           = ENET_REGIDX_BIT(MSC_TINTF_REG_OFFSET, 21U),   /*!< transmitted good frames flag */
    /* ENET_DMA_STAT register */
    ENET_DMA_INT_FLAG_TS            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 0U),     /*!< transmit status flag */
    ENET_DMA_INT_FLAG_TPS           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 1U),     /*!< transmit process stopped status flag */
    ENET_DMA_INT_FLAG_TBU           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 2U),     /*!< transmit buffer unavailable status flag */
    ENET_DMA_INT_FLAG_TJT           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 3U),     /*!< transmit jabber timeout status flag */
    ENET_DMA_INT_FLAG_RO            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 4U),     /*!< receive overflow status flag */
    ENET_DMA_INT_FLAG_TU            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 5U),     /*!< transmit underflow status flag */
    ENET_DMA_INT_FLAG_RS            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 6U),     /*!< receive status flag */
    ENET_DMA_INT_FLAG_RBU           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 7U),     /*!< receive buffer unavailable status flag */
    ENET_DMA_INT_FLAG_RPS           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 8U),     /*!< receive process stopped status flag */
    ENET_DMA_INT_FLAG_RWT           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 9U),     /*!< receive watchdog timeout status flag */
    ENET_DMA_INT_FLAG_ET            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 10U),    /*!< early transmit status flag */
    ENET_DMA_INT_FLAG_FBE           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 13U),    /*!< fatal bus error status flag */
    ENET_DMA_INT_FLAG_ER            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 14U),    /*!< early receive status flag */
    ENET_DMA_INT_FLAG_AI            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 15U),    /*!< abnormal interrupt summary flag */
    ENET_DMA_INT_FLAG_NI            = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 16U),    /*!< normal interrupt summary flag */
    ENET_DMA_INT_FLAG_MSC           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 27U),    /*!< MSC status flag */
    ENET_DMA_INT_FLAG_WUM           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 28U),    /*!< WUM status flag */
    ENET_DMA_INT_FLAG_TST           = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 29U),    /*!< timestamp trigger status flag */ 
}enet_int_flag_enum;

/* ENET interrupt flag clear */
typedef enum
{
    /* ENET_DMA_STAT register */
    ENET_DMA_INT_FLAG_TS_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 0U),     /*!< transmit status flag */
    ENET_DMA_INT_FLAG_TPS_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 1U),     /*!< transmit process stopped status flag */
    ENET_DMA_INT_FLAG_TBU_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 2U),     /*!< transmit buffer unavailable status flag */
    ENET_DMA_INT_FLAG_TJT_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 3U),     /*!< transmit jabber timeout status flag */
    ENET_DMA_INT_FLAG_RO_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 4U),     /*!< receive overflow status flag */
    ENET_DMA_INT_FLAG_TU_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 5U),     /*!< transmit underflow status flag */
    ENET_DMA_INT_FLAG_RS_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 6U),     /*!< receive status flag */
    ENET_DMA_INT_FLAG_RBU_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 7U),     /*!< receive buffer unavailable status flag */
    ENET_DMA_INT_FLAG_RPS_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 8U),     /*!< receive process stopped status flag */
    ENET_DMA_INT_FLAG_RWT_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 9U),     /*!< receive watchdog timeout status flag */
    ENET_DMA_INT_FLAG_ET_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 10U),    /*!< early transmit status flag */
    ENET_DMA_INT_FLAG_FBE_CLR       = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 13U),    /*!< fatal bus error status flag */
    ENET_DMA_INT_FLAG_ER_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 14U),    /*!< early receive status flag */
    ENET_DMA_INT_FLAG_AI_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 15U),    /*!< abnormal interrupt summary flag */
    ENET_DMA_INT_FLAG_NI_CLR        = ENET_REGIDX_BIT(DMA_STAT_REG_OFFSET, 16U),    /*!< normal interrupt summary flag */
}enet_int_flag_clear_enum;

/* current RX/TX descriptor/buffer/descriptor table address get */
typedef enum
{
    ENET_RX_DESC_TABLE              = DMA_RDTADDR_REG_OFFSET,                       /*!< RX descriptor table */
    ENET_RX_CURRENT_DESC            = DMA_CRDADDR_REG_OFFSET,                       /*!< current RX descriptor */
    ENET_RX_CURRENT_BUFFER          = DMA_CRBADDR_REG_OFFSET,                       /*!< current RX buffer */
    ENET_TX_DESC_TABLE              = DMA_TDTADDR_REG_OFFSET,                       /*!< TX descriptor table */
    ENET_TX_CURRENT_DESC            = DMA_CTDADDR_REG_OFFSET,                       /*!< current TX descriptor */
    ENET_TX_CURRENT_BUFFER          = DMA_CTBADDR_REG_OFFSET                        /*!< current TX buffer */
}enet_desc_reg_enum;

/* MAC statistics counter get */
typedef enum
{
    ENET_MSC_TX_SCCNT               = MSC_SCCNT_REG_OFFSET,                         /*!< MSC transmitted good frames after a single collision counter */
    ENET_MSC_TX_MSCCNT              = MSC_MSCCNT_REG_OFFSET,                        /*!< MSC transmitted good frames after more than a single collision counter */
    ENET_MSC_TX_TGFCNT              = MSC_TGFCNT_REG_OFFSET,                        /*!< MSC transmitted good frames counter */
    ENET_MSC_RX_RFCECNT             = MSC_RFCECNT_REG_OFFSET,                       /*!< MSC received frames with CRC error counter */
    ENET_MSC_RX_RFAECNT             = MSC_RFAECNT_REG_OFFSET,                       /*!< MSC received frames with alignment error counter */
    ENET_MSC_RX_RGUFCNT             = MSC_RGUFCNT_REG_OFFSET                        /*!< MSC received good unicast frames counter */
}enet_msc_counter_enum; 

/* function option, used for ENET initialization */
typedef enum
{
    FORWARD_OPTION                  = BIT(0),                                       /*!< configure the frame forward related parameters */
    DMABUS_OPTION                   = BIT(1),                                       /*!< configure the DMA bus mode related parameters */
    DMA_MAXBURST_OPTION             = BIT(2),                                       /*!< configure the DMA max burst related parameters */
    DMA_ARBITRATION_OPTION          = BIT(3),                                       /*!< configure the DMA arbitration related parameters */
    STORE_OPTION                    = BIT(4),                                       /*!< configure the store forward mode related parameters */
    DMA_OPTION                      = BIT(5),                                       /*!< configure the DMA control related parameters */
    VLAN_OPTION                     = BIT(6),                                       /*!< configure the VLAN tag related parameters */
    FLOWCTL_OPTION                  = BIT(7),                                       /*!< configure the flow control related parameters */
    HASHH_OPTION                    = BIT(8),                                       /*!< configure the hash list high 32-bit related parameters */
    HASHL_OPTION                    = BIT(9),                                       /*!< configure the hash list low 32-bit related parameters */
    FILTER_OPTION                   = BIT(10),                                      /*!< configure the frame filter control related parameters */
    HALFDUPLEX_OPTION               = BIT(11),                                      /*!< configure the halfduplex related parameters */
    TIMER_OPTION                    = BIT(12),                                      /*!< configure the frame timer related parameters */
    INTERFRAMEGAP_OPTION            = BIT(13),                                      /*!< configure the inter frame gap related parameters */
}enet_option_enum;

/* phy mode and mac loopback configurations */
typedef enum
{
    ENET_AUTO_NEGOTIATION           = 0x01U,                                        /*!< PHY auto negotiation */
    ENET_100M_FULLDUPLEX            = (ENET_MAC_CFG_SPD | ENET_MAC_CFG_DPM),        /*!< 100Mbit/s, full-duplex */
    ENET_100M_HALFDUPLEX            = ENET_MAC_CFG_SPD ,                            /*!< 100Mbit/s, half-duplex */
    ENET_10M_FULLDUPLEX             = ENET_MAC_CFG_DPM,                             /*!< 10Mbit/s, full-duplex */
    ENET_10M_HALFDUPLEX             = (uint32_t)0x00000000U,                        /*!< 10Mbit/s, half-duplex */
    ENET_LOOPBACKMODE               = (ENET_MAC_CFG_LBM | ENET_MAC_CFG_DPM)         /*!< MAC in loopback mode at the MII */
}enet_mediamode_enum;

/* IP frame checksum function */
typedef enum
{
    ENET_NO_AUTOCHECKSUM                = (uint32_t)0x00000000U,                    /*!< disable IP frame checksum function */
    ENET_AUTOCHECKSUM_DROP_FAILFRAMES   = ENET_MAC_CFG_IPFCO,                       /*!< enable IP frame checksum function */
    ENET_AUTOCHECKSUM_ACCEPT_FAILFRAMES = (ENET_MAC_CFG_IPFCO|ENET_DMA_CTL_DTCERFD) /*!< enable IP frame checksum function, and the received frame
                                                                                         with only payload error but no other errors will not be dropped */
}enet_chksumconf_enum;

/* received frame filter function */
typedef enum
{
    ENET_PROMISCUOUS_MODE           = ENET_MAC_FRMF_PM,                             /*!< promiscuous mode enabled */
    ENET_RECEIVEALL                 = (int32_t)ENET_MAC_FRMF_FAR,                   /*!< all received frame are forwarded to application */
    ENET_BROADCAST_FRAMES_PASS      = (uint32_t)0x00000000U,                        /*!< the address filters pass all received broadcast frames */
    ENET_BROADCAST_FRAMES_DROP      = ENET_MAC_FRMF_BFRMD                           /*!< the address filters filter all incoming broadcast frames */
}enet_frmrecept_enum;

/* register group value get */
typedef enum
{
    ALL_MAC_REG                     = 0U,                                            /*!< MAC register group */
    ALL_MSC_REG                     = 22U,                                           /*!< MSC register group */
    ALL_PTP_REG                     = 33U,                                           /*!< PTP register group */
    ALL_DMA_REG                     = 44U,                                           /*!< DMA register group */
}enet_registers_type_enum;

/* dma direction select */
typedef enum
{
    ENET_DMA_TX                     = ENET_DMA_STAT_TP,                             /*!< DMA transmit direction */
    ENET_DMA_RX                     = ENET_DMA_STAT_RP                              /*!< DMA receive direction */
}enet_dmadirection_enum;

/* PHY operation direction select */
typedef enum
{
    ENET_PHY_READ                   = (uint32_t)0x00000000,                         /*!< read PHY */
    ENET_PHY_WRITE                  = ENET_MAC_PHY_CTL_PW                           /*!< write PHY */
}enet_phydirection_enum;

/* register operation direction select */
typedef enum
{
    ENET_REG_READ,                                                                  /*!< read register */
    ENET_REG_WRITE                                                                  /*!< write register */
}enet_regdirection_enum;

/* ENET MAC addresses */ 
typedef enum
{
    ENET_MAC_ADDRESS0               = ((uint32_t)0x00000000),                       /*!< MAC address0 */
    ENET_MAC_ADDRESS1               = ((uint32_t)0x00000008),                       /*!< MAC address1 */
    ENET_MAC_ADDRESS2               = ((uint32_t)0x00000010),                       /*!< MAC address2 */
    ENET_MAC_ADDRESS3               = ((uint32_t)0x00000018)                        /*!< MAC address3 */
}enet_macaddress_enum;

/* descriptor information */
typedef enum
{
    TXDESC_COLLISION_COUNT,                                                         /*!< the number of collisions occurred before the frame was transmitted */
    TXDESC_BUFFER_1_ADDR,                                                           /*!< transmit frame buffer 1 address */
    RXDESC_FRAME_LENGTH,                                                            /*!< the byte length of the received frame that was transferred to the buffer */
    RXDESC_BUFFER_1_SIZE,                                                           /*!< receive buffer 1 size */
    RXDESC_BUFFER_2_SIZE,                                                           /*!< receive buffer 2 size */
    RXDESC_BUFFER_1_ADDR                                                            /*!< receive frame buffer 1 address */
}enet_descstate_enum;

/* MSC counters preset mode */
typedef enum
{
    ENET_MSC_PRESET_NONE            = 0U,                                           /*!< do not preset MSC counter */
    ENET_MSC_PRESET_HALF            = ENET_MSC_CTL_PMC,                             /*!< preset all MSC counters to almost-half(0x7FFF FFF0) value */
    ENET_MSC_PRESET_FULL            = ENET_MSC_CTL_PMC | ENET_MSC_CTL_AFHPM         /*!< preset all MSC counters to almost-full(0xFFFF FFF0) value */
}enet_msc_preset_enum;

typedef enum{
    ENET_CKNT_ORDINARY                = PTP_TSCTL_CKNT(0),                          /*!< type of ordinary clock node type for timestamp */
    ENET_CKNT_BOUNDARY                = PTP_TSCTL_CKNT(1),                          /*!< type of boundary clock node type for timestamp */
    ENET_CKNT_END_TO_END              = PTP_TSCTL_CKNT(2),                          /*!< type of end-to-end transparent clock node type for timestamp */
    ENET_CKNT_PEER_TO_PEER            = PTP_TSCTL_CKNT(3),                          /*!< type of peer-to-peer transparent clock node type for timestamp */
    ENET_PTP_SYSTIME_INIT             = ENET_PTP_TSCTL_TMSSTI,                      /*!< timestamp initialize */
    ENET_PTP_SYSTIME_UPDATE           = ENET_PTP_TSCTL_TMSSTU,                      /*!< timestamp update */ 
    ENET_PTP_ADDEND_UPDATE            = ENET_PTP_TSCTL_TMSARU,                      /*!< addend register update */
    ENET_PTP_FINEMODE                 = (int32_t)(ENET_PTP_TSCTL_TMSFCU| BIT(31)),  /*!< the system timestamp uses the fine method for updating */
    ENET_PTP_COARSEMODE               = ENET_PTP_TSCTL_TMSFCU,                      /*!< the system timestamp uses the coarse method for updating */
    ENET_SUBSECOND_DIGITAL_ROLLOVER   = (int32_t)(ENET_PTP_TSCTL_SCROM | BIT(31)),  /*!< digital rollover mode */
    ENET_SUBSECOND_BINARY_ROLLOVER    = ENET_PTP_TSCTL_SCROM,                       /*!< binary rollover mode */
    ENET_SNOOPING_PTP_VERSION_2       = (int32_t)(ENET_PTP_TSCTL_PFSV| BIT(31)),    /*!< version 2 */
    ENET_SNOOPING_PTP_VERSION_1       = ENET_PTP_TSCTL_PFSV,                        /*!< version 1 */
    ENET_EVENT_TYPE_MESSAGES_SNAPSHOT = (int32_t)(ENET_PTP_TSCTL_ETMSEN| BIT(31)),  /*!< only event type messages are taken snapshot */
    ENET_ALL_TYPE_MESSAGES_SNAPSHOT   = ENET_PTP_TSCTL_ETMSEN,                      /*!< all type messages are taken snapshot except announce, management and signaling message */
    ENET_MASTER_NODE_MESSAGE_SNAPSHOT = (int32_t)(ENET_PTP_TSCTL_MNMSEN| BIT(31)),  /*!< snapshot is only take for master node message */
    ENET_SLAVE_NODE_MESSAGE_SNAPSHOT  = ENET_PTP_TSCTL_MNMSEN,                      /*!< snapshot is only taken for slave node message */
}enet_ptp_function_enum;

/* structure for initialization of the ENET  */
typedef struct
{
    uint32_t option_enable;                                                         /*!< select which function to configure */
    uint32_t forward_frame;                                                         /*!< frame forward related parameters */ 
    uint32_t dmabus_mode;                                                           /*!< DMA bus mode related parameters */
    uint32_t dma_maxburst;                                                          /*!< DMA max burst related parameters */
    uint32_t dma_arbitration;                                                       /*!< DMA Tx and Rx arbitration related parameters */
    uint32_t store_forward_mode;                                                    /*!< store forward mode related parameters */
    uint32_t dma_function;                                                          /*!< DMA control related parameters */
    uint32_t vlan_config;                                                           /*!< VLAN tag related parameters */   
    uint32_t flow_control;                                                          /*!< flow control related parameters */
    uint32_t hashtable_high;                                                        /*!< hash list high 32-bit related parameters */
    uint32_t hashtable_low;                                                         /*!< hash list low 32-bit related parameters */
    uint32_t framesfilter_mode;                                                     /*!< frame filter control related parameters */
    uint32_t halfduplex_param;                                                      /*!< halfduplex related parameters */            
    uint32_t timer_config;                                                          /*!< frame timer related parameters */
    uint32_t interframegap;                                                         /*!< inter frame gap related parameters */
}enet_initpara_struct;

/* structure for ENET DMA desciptors */ 
typedef struct  
{
    uint32_t status;                                                                /*!< status */
    uint32_t control_buffer_size;                                                   /*!< control and buffer1, buffer2 lengths */
    uint32_t buffer1_addr;                                                          /*!< buffer1 address pointer/timestamp low */
    uint32_t buffer2_next_desc_addr;                                                /*!< buffer2 or next descriptor address pointer/timestamp high */

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    uint32_t extended_status;                                                       /*!< extended status */
    uint32_t reserved;                                                              /*!< reserved */
    uint32_t timestamp_low;                                                         /*!< timestamp low */
    uint32_t timestamp_high;                                                        /*!< timestamp high */ 
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */ 
  
} enet_descriptors_struct;

/* structure of PTP system time */ 
typedef struct
{
    uint32_t second;                                                                /*!< second of system time */
    uint32_t subsecond;                                                             /*!< subsecond of system time */
    uint32_t sign;                                                                  /*!< sign of system time */
}enet_ptp_systime_struct;

/* mac_cfg register value */
#define MAC_CFG_BOL(regval)                       (BITS(5,6) & ((uint32_t)(regval) << 5))       /*!< write value to ENET_MAC_CFG_BOL bit field */
#define ENET_BACKOFFLIMIT_10                      MAC_CFG_BOL(0)                                /*!< min (n, 10) */
#define ENET_BACKOFFLIMIT_8                       MAC_CFG_BOL(1)                                /*!< min (n, 8) */
#define ENET_BACKOFFLIMIT_4                       MAC_CFG_BOL(2)                                /*!< min (n, 4) */
#define ENET_BACKOFFLIMIT_1                       MAC_CFG_BOL(3)                                /*!< min (n, 1) */ 

#define MAC_CFG_IGBS(regval)                      (BITS(17,19) & ((uint32_t)(regval) << 17))    /*!< write value to ENET_MAC_CFG_IGBS bit field */
#define ENET_INTERFRAMEGAP_96BIT                  MAC_CFG_IGBS(0)                               /*!< minimum 96 bit times */ 
#define ENET_INTERFRAMEGAP_88BIT                  MAC_CFG_IGBS(1)                               /*!< minimum 88 bit times */
#define ENET_INTERFRAMEGAP_80BIT                  MAC_CFG_IGBS(2)                               /*!< minimum 80 bit times */
#define ENET_INTERFRAMEGAP_72BIT                  MAC_CFG_IGBS(3)                               /*!< minimum 72 bit times */
#define ENET_INTERFRAMEGAP_64BIT                  MAC_CFG_IGBS(4)                               /*!< minimum 64 bit times */
#define ENET_INTERFRAMEGAP_56BIT                  MAC_CFG_IGBS(5)                               /*!< minimum 56 bit times */
#define ENET_INTERFRAMEGAP_48BIT                  MAC_CFG_IGBS(6)                               /*!< minimum 48 bit times */
#define ENET_INTERFRAMEGAP_40BIT                  MAC_CFG_IGBS(7)                               /*!< minimum 40 bit times */

#define ENET_TYPEFRAME_CRC_DROP_ENABLE            ENET_MAC_CFG_TFCD                             /*!< FCS field(last 4 bytes) of frame will be dropped before forwarding */
#define ENET_TYPEFRAME_CRC_DROP_DISABLE           ((uint32_t)0x00000000U)                       /*!< FCS field(last 4 bytes) of frame will not be dropped before forwarding */
#define ENET_TYPEFRAME_CRC_DROP                   ENET_MAC_CFG_TFCD                             /*!< the function that FCS field(last 4 bytes) of frame will be dropped before forwarding */

#define ENET_WATCHDOG_ENABLE                      ((uint32_t)0x00000000U)                       /*!< the MAC allows no more than 2048 bytes of the frame being received */
#define ENET_WATCHDOG_DISABLE                     ENET_MAC_CFG_WDD                              /*!< the MAC disables the watchdog timer on the receiver, and can receive frames of up to 16384 bytes */
 
#define ENET_JABBER_ENABLE                        ((uint32_t)0x00000000U)                       /*!< the maximum transmission byte is 2048 */
#define ENET_JABBER_DISABLE                       ENET_MAC_CFG_JBD                              /*!< the maximum transmission byte can be 16384 */

#define ENET_CARRIERSENSE_ENABLE                  ((uint32_t)0x00000000U)                       /*!< the MAC transmitter generates carrier sense error and aborts the transmission */
#define ENET_CARRIERSENSE_DISABLE                 ENET_MAC_CFG_CSD                              /*!< the MAC transmitter ignores the MII CRS signal during frame transmission in half-duplex mode */
 
#define ENET_SPEEDMODE_10M                        ((uint32_t)0x00000000U)                       /*!< 10 Mbit/s */
#define ENET_SPEEDMODE_100M                       ENET_MAC_CFG_SPD                              /*!< 100 Mbit/s */

#define ENET_RECEIVEOWN_ENABLE                    ((uint32_t)0x00000000U)                       /*!< the MAC receives all packets that are given by the PHY while transmitting */
#define ENET_RECEIVEOWN_DISABLE                   ENET_MAC_CFG_ROD                              /*!< the MAC disables the reception of frames in half-duplex mode */

#define ENET_LOOPBACKMODE_ENABLE                  ENET_MAC_CFG_LBM                              /*!< the MAC operates in loopback mode at the MII */
#define ENET_LOOPBACKMODE_DISABLE                 ((uint32_t)0x00000000U)                       /*!< the MAC operates in normal mode */

#define ENET_MODE_FULLDUPLEX                      ENET_MAC_CFG_DPM                              /*!< full-duplex mode enable */
#define ENET_MODE_HALFDUPLEX                      ((uint32_t)0x00000000U)                       /*!< half-duplex mode enable */

#define ENET_CHECKSUMOFFLOAD_ENABLE               ENET_MAC_CFG_IPFCO                            /*!< IP frame checksum offload function enabled for received IP frame */
#define ENET_CHECKSUMOFFLOAD_DISABLE              ((uint32_t)0x00000000U)                       /*!< the checksum offload function in the receiver is disabled */

#define ENET_RETRYTRANSMISSION_ENABLE             ((uint32_t)0x00000000U)                       /*!< the MAC attempts retries up to 16 times based on the settings of BOL*/
#define ENET_RETRYTRANSMISSION_DISABLE            ENET_MAC_CFG_RTD                              /*!< the MAC attempts only 1 transmission */

#define ENET_AUTO_PADCRC_DROP_ENABLE              ENET_MAC_CFG_APCD                             /*!< the MAC strips the Pad/FCS field on received frames */
#define ENET_AUTO_PADCRC_DROP_DISABLE             ((uint32_t)0x00000000U)                       /*!< the MAC forwards all received frames without modify it */
#define ENET_AUTO_PADCRC_DROP                     ENET_MAC_CFG_APCD                             /*!< the function of the MAC strips the Pad/FCS field on received frames */

#define ENET_DEFERRALCHECK_ENABLE                 ENET_MAC_CFG_DFC                              /*!< the deferral check function is enabled in the MAC */
#define ENET_DEFERRALCHECK_DISABLE                ((uint32_t)0x00000000U)                       /*!< the deferral check function is disabled */

/* mac_frmf register value */
#define MAC_FRMF_PCFRM(regval)                    (BITS(6,7) & ((uint32_t)(regval) << 6))       /*!< write value to ENET_MAC_FRMF_PCFRM bit field */
#define ENET_PCFRM_PREVENT_ALL                    MAC_FRMF_PCFRM(0)                             /*!< MAC prevents all control frames from reaching the application */
#define ENET_PCFRM_PREVENT_PAUSEFRAME             MAC_FRMF_PCFRM(1)                             /*!< MAC only forwards all other control frames except pause control frame */
#define ENET_PCFRM_FORWARD_ALL                    MAC_FRMF_PCFRM(2)                             /*!< MAC forwards all control frames to application even if they fail the address filter */
#define ENET_PCFRM_FORWARD_FILTERED               MAC_FRMF_PCFRM(3)                             /*!< MAC forwards control frames that only pass the address filter */
 
#define ENET_RX_FILTER_DISABLE                    ENET_MAC_FRMF_FAR                             /*!< all received frame are forwarded to application */
#define ENET_RX_FILTER_ENABLE                     ((uint32_t)0x00000000U)                       /*!< only the frame passed the filter can be forwarded to application */
 
#define ENET_SRC_FILTER_NORMAL_ENABLE             ENET_MAC_FRMF_SAFLT                           /*!< filter source address */
#define ENET_SRC_FILTER_INVERSE_ENABLE            (ENET_MAC_FRMF_SAFLT | ENET_MAC_FRMF_SAIFLT)  /*!< inverse source address filtering result */
#define ENET_SRC_FILTER_DISABLE                   ((uint32_t)0x00000000U)                       /*!< source address function in filter disable */
#define ENET_SRC_FILTER                           ENET_MAC_FRMF_SAFLT                           /*!< filter source address function */
#define ENET_SRC_FILTER_INVERSE                   ENET_MAC_FRMF_SAIFLT                          /*!< inverse source address filtering result function */

#define ENET_BROADCASTFRAMES_ENABLE               ((uint32_t)0x00000000U)                       /*!< the address filters pass all received broadcast frames */
#define ENET_BROADCASTFRAMES_DISABLE              ENET_MAC_FRMF_BFRMD                           /*!< the address filters filter all incoming broadcast frames */
 
#define ENET_DEST_FILTER_INVERSE_ENABLE           ENET_MAC_FRMF_DAIFLT                          /*!< inverse DA filtering result */
#define ENET_DEST_FILTER_INVERSE_DISABLE          ((uint32_t)0x00000000U)                       /*!< not inverse DA filtering result */
#define ENET_DEST_FILTER_INVERSE                  ENET_MAC_FRMF_DAIFLT                          /*!< inverse DA filtering result function */

#define ENET_PROMISCUOUS_ENABLE                   ENET_MAC_FRMF_PM                              /*!< promiscuous mode enabled */
#define ENET_PROMISCUOUS_DISABLE                  ((uint32_t)0x00000000U)                       /*!< promiscuous mode disabled */
          
#define ENET_MULTICAST_FILTER_HASH_OR_PERFECT     (ENET_MAC_FRMF_HMF | ENET_MAC_FRMF_HPFLT)     /*!< pass multicast frames that match either the perfect or the hash filtering */
#define ENET_MULTICAST_FILTER_HASH                ENET_MAC_FRMF_HMF                             /*!< pass multicast frames that match the hash filtering */
#define ENET_MULTICAST_FILTER_PERFECT             ((uint32_t)0x00000000U)                       /*!< pass multicast frames that match the perfect filtering */
#define ENET_MULTICAST_FILTER_NONE                ENET_MAC_FRMF_MFD                             /*!< all multicast frames are passed */
#define ENET_MULTICAST_FILTER_PASS                ENET_MAC_FRMF_MFD                             /*!< pass all multicast frames function */
#define ENET_MULTICAST_FILTER_HASH_MODE           ENET_MAC_FRMF_HMF                             /*!< HASH multicast filter function */
#define ENET_FILTER_MODE_EITHER                   ENET_MAC_FRMF_HPFLT                           /*!< HASH or perfect filter function */

#define ENET_UNICAST_FILTER_EITHER                (ENET_MAC_FRMF_HUF | ENET_MAC_FRMF_HPFLT)     /*!< pass unicast frames that match either the perfect or the hash filtering */
#define ENET_UNICAST_FILTER_HASH                  ENET_MAC_FRMF_HUF                             /*!< pass unicast frames that match the hash filtering */
#define ENET_UNICAST_FILTER_PERFECT               ((uint32_t)0x00000000U)                       /*!< pass unicast frames that match the perfect filtering */
#define ENET_UNICAST_FILTER_HASH_MODE             ENET_MAC_FRMF_HUF                             /*!< HASH unicast filter function */

/* mac_phy_ctl register value */
#define MAC_PHY_CTL_CLR(regval)                   (BITS(2,4) & ((uint32_t)(regval) << 2))       /*!< write value to ENET_MAC_PHY_CTL_CLR bit field */
#define ENET_MDC_HCLK_DIV42                       MAC_PHY_CTL_CLR(0)                            /*!< HCLK:60-100 MHz; MDC clock= HCLK/42 */
#define ENET_MDC_HCLK_DIV62                       MAC_PHY_CTL_CLR(1)                            /*!< HCLK:100-150 MHz; MDC clock= HCLK/62 */
#define ENET_MDC_HCLK_DIV16                       MAC_PHY_CTL_CLR(2)                            /*!< HCLK:20-35 MHz; MDC clock= HCLK/16 */
#define ENET_MDC_HCLK_DIV26                       MAC_PHY_CTL_CLR(3)                            /*!< HCLK:35-60 MHz; MDC clock= HCLK/26 */
#define ENET_MDC_HCLK_DIV102                      MAC_PHY_CTL_CLR(4)                            /*!< HCLK:150-240 MHz; MDC clock= HCLK/102 */

#define MAC_PHY_CTL_PR(regval)                    (BITS(6,10) & ((uint32_t)(regval) << 6))      /*!< write value to ENET_MAC_PHY_CTL_PR bit field */

#define MAC_PHY_CTL_PA(regval)                    (BITS(11,15) & ((uint32_t)(regval) << 11))    /*!< write value to ENET_MAC_PHY_CTL_PA bit field */

/* mac_phy_data register value */
#define MAC_PHY_DATA_PD(regval)                   (BITS(0,15) & ((uint32_t)(regval) << 0))      /*!< write value to ENET_MAC_PHY_DATA_PD bit field */

/* mac_fctl register value */
#define MAC_FCTL_PLTS(regval)                     (BITS(4,5) & ((uint32_t)(regval) << 4))       /*!< write value to ENET_MAC_FCTL_PLTS bit field */
#define ENET_PAUSETIME_MINUS4                     MAC_FCTL_PLTS(0)                              /*!< pause time minus 4 slot times */
#define ENET_PAUSETIME_MINUS28                    MAC_FCTL_PLTS(1)                              /*!< pause time minus 28 slot times */
#define ENET_PAUSETIME_MINUS144                   MAC_FCTL_PLTS(2)                              /*!< pause time minus 144 slot times */
#define ENET_PAUSETIME_MINUS256                   MAC_FCTL_PLTS(3)                              /*!< pause time minus 256 slot times */ 

#define ENET_ZERO_QUANTA_PAUSE_ENABLE             ((uint32_t)0x00000000U)                       /*!< enable the automatic zero-quanta generation function */
#define ENET_ZERO_QUANTA_PAUSE_DISABLE            ENET_MAC_FCTL_DZQP                            /*!< disable the automatic zero-quanta generation function */
#define ENET_ZERO_QUANTA_PAUSE                    ENET_MAC_FCTL_DZQP                            /*!< the automatic zero-quanta generation function */

#define ENET_MAC0_AND_UNIQUE_ADDRESS_PAUSEDETECT  ENET_MAC_FCTL_UPFDT                           /*!< besides the unique multicast address, MAC also use the MAC0 address to detect pause frame */
#define ENET_UNIQUE_PAUSEDETECT                   ((uint32_t)0x00000000U)                       /*!< only the unique multicast address for pause frame which is specified in IEEE802.3 can be detected */
 
#define ENET_RX_FLOWCONTROL_ENABLE                ENET_MAC_FCTL_RFCEN                           /*!< enable decoding function for the received pause frame and process it */
#define ENET_RX_FLOWCONTROL_DISABLE               ((uint32_t)0x00000000U)                       /*!< decode function for pause frame is disabled */
#define ENET_RX_FLOWCONTROL                       ENET_MAC_FCTL_RFCEN                           /*!< decoding function for the received pause frame and process it */

#define ENET_TX_FLOWCONTROL_ENABLE                ENET_MAC_FCTL_TFCEN                           /*!< enable the flow control operation in the MAC */
#define ENET_TX_FLOWCONTROL_DISABLE               ((uint32_t)0x00000000U)                       /*!< disable the flow control operation in the MAC */
#define ENET_TX_FLOWCONTROL                       ENET_MAC_FCTL_TFCEN                           /*!< the flow control operation in the MAC */

#define ENET_BACK_PRESSURE_ENABLE                 ENET_MAC_FCTL_FLCBBKPA                        /*!< enable the back pressure operation in the MAC */
#define ENET_BACK_PRESSURE_DISABLE                ((uint32_t)0x00000000U)                       /*!< disable the back pressure operation in the MAC */
#define ENET_BACK_PRESSURE                        ENET_MAC_FCTL_FLCBBKPA                        /*!< the back pressure operation in the MAC */
                                                                                      
#define MAC_FCTL_PTM(regval)                      (BITS(16,31) & ((uint32_t)(regval) << 16))    /*!< write value to ENET_MAC_FCTL_PTM bit field */
/* mac_vlt register value */
#define MAC_VLT_VLTI(regval)                      (BITS(0,15) & ((uint32_t)(regval) << 0))      /*!< write value to ENET_MAC_VLT_VLTI bit field */
 
#define ENET_VLANTAGCOMPARISON_12BIT              ENET_MAC_VLT_VLTC                             /*!< only low 12 bits of the VLAN tag are used for comparison */
#define ENET_VLANTAGCOMPARISON_16BIT              ((uint32_t)0x00000000U)                       /*!< all 16 bits of the VLAN tag are used for comparison */

/* mac_wum register value */ 
#define ENET_WUM_FLAG_WUFFRPR                     ENET_MAC_WUM_WUFFRPR                          /*!< wakeup frame filter register poniter reset */
#define ENET_WUM_FLAG_WUFR                        ENET_MAC_WUM_WUFR                             /*!< wakeup frame received */
#define ENET_WUM_FLAG_MPKR                        ENET_MAC_WUM_MPKR                             /*!< magic packet received */
#define ENET_WUM_POWER_DOWN                       ENET_MAC_WUM_PWD                              /*!< power down mode */    
#define ENET_WUM_MAGIC_PACKET_FRAME               ENET_MAC_WUM_MPEN                             /*!< enable a wakeup event due to magic packet reception */   
#define ENET_WUM_WAKE_UP_FRAME                    ENET_MAC_WUM_WFEN                             /*!< enable a wakeup event due to wakeup frame reception */     
#define ENET_WUM_GLOBAL_UNICAST                   ENET_MAC_WUM_GU                               /*!< any received unicast frame passed filter is considered to be a wakeup frame */

/* mac_dbg register value */
#define ENET_MAC_RECEIVER_NOT_IDLE                ENET_MAC_DBG_MRNI                             /*!< MAC receiver is not in idle state */
#define ENET_RX_ASYNCHRONOUS_FIFO_STATE           ENET_MAC_DBG_RXAFS                            /*!< Rx asynchronous FIFO status */
#define ENET_RXFIFO_WRITING                       ENET_MAC_DBG_RXFW                             /*!< RxFIFO is doing write operation */
#define ENET_RXFIFO_READ_STATUS                   ENET_MAC_DBG_RXFRS                            /*!< RxFIFO read operation status */
#define ENET_RXFIFO_STATE                         ENET_MAC_DBG_RXFS                             /*!< RxFIFO state */
#define ENET_MAC_TRANSMITTER_NOT_IDLE             ENET_MAC_DBG_MTNI                             /*!< MAC transmitter is not in idle state */
#define ENET_MAC_TRANSMITTER_STATUS               ENET_MAC_DBG_SOMT                             /*!< status of MAC transmitter */
#define ENET_PAUSE_CONDITION_STATUS               ENET_MAC_DBG_PCS                              /*!< pause condition status */
#define ENET_TXFIFO_READ_STATUS                   ENET_MAC_DBG_TXFRS                            /*!< TxFIFO read operation status */
#define ENET_TXFIFO_WRITING                       ENET_MAC_DBG_TXFW                             /*!< TxFIFO is doing write operation */
#define ENET_TXFIFO_NOT_EMPTY                     ENET_MAC_DBG_TXFNE                            /*!< TxFIFO is not empty */
#define ENET_TXFIFO_FULL                          ENET_MAC_DBG_TXFF                             /*!< TxFIFO is full */

#define GET_MAC_DBG_RXAFS(regval)                 GET_BITS((regval),1,2)                        /*!< get value of ENET_MAC_DBG_RXAFS bit field */

#define GET_MAC_DBG_RXFRS(regval)                 GET_BITS((regval),5,6)                        /*!< get value of ENET_MAC_DBG_RXFRS bit field */

#define GET_MAC_DBG_RXFS(regval)                  GET_BITS((regval),8,9)                        /*!< get value of ENET_MAC_DBG_RXFS bit field */

#define GET_MAC_DBG_SOMT(regval)                  GET_BITS((regval),17,18)                      /*!< get value of ENET_MAC_DBG_SOMT bit field */

#define GET_MAC_DBG_TXFRS(regval)                 GET_BITS((regval),20,21)                      /*!< get value of ENET_MAC_DBG_TXFRS bit field */

/* mac_addr0h register value */
#define MAC_ADDR0H_ADDR0H(regval)                 (BITS(0,15) & ((uint32_t)(regval) << 0))      /*!< write value to ENET_MAC_ADDR0H_ADDR0H bit field */

/* mac_addrxh register value, x = 1,2,3 */
#define MAC_ADDR123H_ADDR123H(regval)             (BITS(0,15) & ((uint32_t)(regval) << 0))      /*!< write value to ENET_MAC_ADDRxH_ADDRxH(x=1,2,3) bit field */

#define ENET_ADDRESS_MASK_BYTE0                   BIT(24)                                       /*!< low register bits [7:0] */
#define ENET_ADDRESS_MASK_BYTE1                   BIT(25)                                       /*!< low register bits [15:8] */
#define ENET_ADDRESS_MASK_BYTE2                   BIT(26)                                       /*!< low register bits [23:16] */
#define ENET_ADDRESS_MASK_BYTE3                   BIT(27)                                       /*!< low register bits [31:24] */
#define ENET_ADDRESS_MASK_BYTE4                   BIT(28)                                       /*!< high register bits [7:0] */
#define ENET_ADDRESS_MASK_BYTE5                   BIT(29)                                       /*!< high register bits [15:8] */

#define ENET_ADDRESS_FILTER_SA                    BIT(30)                                       /*!< use MAC address[47:0] is to compare with the SA fields of the received frame */
#define ENET_ADDRESS_FILTER_DA                    ((uint32_t)0x00000000)                        /*!< use MAC address[47:0] is to compare with the DA fields of the received frame */
 
/* mac_fcth register value */
#define MAC_FCTH_RFA(regval)                      ((BITS(0,2) & ((uint32_t)(regval) << 0)) << 8)  /*!< write value to ENET_MAC_FCTH_RFA bit field */
#define ENET_ACTIVE_THRESHOLD_256BYTES            MAC_FCTH_RFA(0)                               /*!< threshold level is 256 bytes */
#define ENET_ACTIVE_THRESHOLD_512BYTES            MAC_FCTH_RFA(1)                               /*!< threshold level is 512 bytes */
#define ENET_ACTIVE_THRESHOLD_768BYTES            MAC_FCTH_RFA(2)                               /*!< threshold level is 768 bytes */
#define ENET_ACTIVE_THRESHOLD_1024BYTES           MAC_FCTH_RFA(3)                               /*!< threshold level is 1024 bytes */
#define ENET_ACTIVE_THRESHOLD_1280BYTES           MAC_FCTH_RFA(4)                               /*!< threshold level is 1280 bytes */
#define ENET_ACTIVE_THRESHOLD_1536BYTES           MAC_FCTH_RFA(5)                               /*!< threshold level is 1536 bytes */
#define ENET_ACTIVE_THRESHOLD_1792BYTES           MAC_FCTH_RFA(6)                               /*!< threshold level is 1792 bytes */

#define MAC_FCTH_RFD(regval)                      ((BITS(4,6) & ((uint32_t)(regval) << 4)) << 8)  /*!< write value to ENET_MAC_FCTH_RFD bit field */
#define ENET_DEACTIVE_THRESHOLD_256BYTES          MAC_FCTH_RFD(0)                               /*!< threshold level is 256 bytes */
#define ENET_DEACTIVE_THRESHOLD_512BYTES          MAC_FCTH_RFD(1)                               /*!< threshold level is 512 bytes */
#define ENET_DEACTIVE_THRESHOLD_768BYTES          MAC_FCTH_RFD(2)                               /*!< threshold level is 768 bytes */
#define ENET_DEACTIVE_THRESHOLD_1024BYTES         MAC_FCTH_RFD(3)                               /*!< threshold level is 1024 bytes */
#define ENET_DEACTIVE_THRESHOLD_1280BYTES         MAC_FCTH_RFD(4)                               /*!< threshold level is 1280 bytes */
#define ENET_DEACTIVE_THRESHOLD_1536BYTES         MAC_FCTH_RFD(5)                               /*!< threshold level is 1536 bytes */
#define ENET_DEACTIVE_THRESHOLD_1792BYTES         MAC_FCTH_RFD(6)                               /*!< threshold level is 1792 bytes */

/* msc_ctl register value */
#define ENET_MSC_COUNTER_STOP_ROLLOVER            ENET_MSC_CTL_CTSR                             /*!< counter stop rollover */
#define ENET_MSC_RESET_ON_READ                    ENET_MSC_CTL_RTOR                             /*!< reset on read */
#define ENET_MSC_COUNTERS_FREEZE                  ENET_MSC_CTL_MCFZ                             /*!< MSC counter freeze */

/* ptp_tsctl register value */
#define ENET_RXTX_TIMESTAMP                       ENET_PTP_TSCTL_TMSEN                          /*!< enable timestamp function for transmit and receive frames */
#define ENET_PTP_TIMESTAMP_INT                    ENET_PTP_TSCTL_TMSITEN                        /*!< timestamp interrupt trigger enable */
#define ENET_ALL_RX_TIMESTAMP                     ENET_PTP_TSCTL_ARFSEN                         /*!< all received frames are taken snapshot */
#define ENET_NONTYPE_FRAME_SNAPSHOT               ENET_PTP_TSCTL_ESEN                           /*!< take snapshot when received non type frame */
#define ENET_IPV6_FRAME_SNAPSHOT                  ENET_PTP_TSCTL_IP6SEN                         /*!< take snapshot for IPv6 frame */
#define ENET_IPV4_FRAME_SNAPSHOT                  ENET_PTP_TSCTL_IP4SEN                         /*!< take snapshot for IPv4 frame */
#define ENET_PTP_FRAME_USE_MACADDRESS_FILTER      ENET_PTP_TSCTL_MAFEN                          /*!< enable MAC address1-3 to filter the PTP frame */

/* ptp_ssinc register value */
#define PTP_SSINC_STMSSI(regval)                  (BITS(0,7) & ((uint32_t)(regval) << 0))       /*!< write value to ENET_PTP_SSINC_STMSSI bit field */

/* ptp_tsl register value */
#define GET_PTP_TSL_STMSS(regval)                 GET_BITS((uint32_t)(regval),0,30)             /*!< get value of ENET_PTP_TSL_STMSS bit field */
 
#define ENET_PTP_TIME_POSITIVE                    ((uint32_t)0x00000000)                        /*!< time value is positive */
#define ENET_PTP_TIME_NEGATIVE                    ENET_PTP_TSL_STS                              /*!< time value is negative */

#define GET_PTP_TSL_STS(regval)                   (((regval) & BIT(31)) >> (31U))               /*!< get value of ENET_PTP_TSL_STS bit field */

/* ptp_tsul register value */
#define PTP_TSUL_TMSUSS(regval)                   (BITS(0,30) & ((uint32_t)(regval) << 0))      /*!< write value to ENET_PTP_TSUL_TMSUSS bit field */

#define ENET_PTP_ADD_TO_TIME                      ((uint32_t)0x00000000)                        /*!< timestamp update value is added to system time */
#define ENET_PTP_SUBSTRACT_FROM_TIME              ENET_PTP_TSUL_TMSUPNS                         /*!< timestamp update value is subtracted from system time */

/* ptp_ppsctl register value */
#define PTP_PPSCTL_PPSOFC(regval)                 (BITS(0,3) & ((uint32_t)(regval) << 0))       /*!< write value to ENET_PTP_PPSCTL_PPSOFC bit field */
#define ENET_PPSOFC_1HZ                           PTP_PPSCTL_PPSOFC(0)                          /*!< PPS output 1Hz frequency */
#define ENET_PPSOFC_2HZ                           PTP_PPSCTL_PPSOFC(1)                          /*!< PPS output 2Hz frequency */
#define ENET_PPSOFC_4HZ                           PTP_PPSCTL_PPSOFC(2)                          /*!< PPS output 4Hz frequency */
#define ENET_PPSOFC_8HZ                           PTP_PPSCTL_PPSOFC(3)                          /*!< PPS output 8Hz frequency */
#define ENET_PPSOFC_16HZ                          PTP_PPSCTL_PPSOFC(4)                          /*!< PPS output 16Hz frequency */
#define ENET_PPSOFC_32HZ                          PTP_PPSCTL_PPSOFC(5)                          /*!< PPS output 32Hz frequency */
#define ENET_PPSOFC_64HZ                          PTP_PPSCTL_PPSOFC(6)                          /*!< PPS output 64Hz frequency */
#define ENET_PPSOFC_128HZ                         PTP_PPSCTL_PPSOFC(7)                          /*!< PPS output 128Hz frequency */
#define ENET_PPSOFC_256HZ                         PTP_PPSCTL_PPSOFC(8)                          /*!< PPS output 256Hz frequency */
#define ENET_PPSOFC_512HZ                         PTP_PPSCTL_PPSOFC(9)                          /*!< PPS output 512Hz frequency */
#define ENET_PPSOFC_1024HZ                        PTP_PPSCTL_PPSOFC(10)                         /*!< PPS output 1024Hz frequency */
#define ENET_PPSOFC_2048HZ                        PTP_PPSCTL_PPSOFC(11)                         /*!< PPS output 2048Hz frequency */
#define ENET_PPSOFC_4096HZ                        PTP_PPSCTL_PPSOFC(12)                         /*!< PPS output 4096Hz frequency */
#define ENET_PPSOFC_8192HZ                        PTP_PPSCTL_PPSOFC(13)                         /*!< PPS output 8192Hz frequency */
#define ENET_PPSOFC_16384HZ                       PTP_PPSCTL_PPSOFC(14)                         /*!< PPS output 16384Hz frequency */
#define ENET_PPSOFC_32768HZ                       PTP_PPSCTL_PPSOFC(15)                         /*!< PPS output 32768Hz frequency */

/* dma_bctl register value */
#define DMA_BCTL_DPSL(regval)                     (BITS(2,6) & ((uint32_t)(regval) << 2))       /*!< write value to ENET_DMA_BCTL_DPSL bit field */
#define GET_DMA_BCTL_DPSL(regval)                 GET_BITS((regval),2,6)                        /*!< get value of ENET_DMA_BCTL_DPSL bit field */

#define ENET_ENHANCED_DESCRIPTOR                  ENET_DMA_BCTL_DFM                             /*!< enhanced mode descriptor */
#define ENET_NORMAL_DESCRIPTOR                    ((uint32_t)0x00000000)                        /*!< normal mode descriptor */

#define DMA_BCTL_PGBL(regval)                     (BITS(8,13) & ((uint32_t)(regval) << 8))      /*!< write value to ENET_DMA_BCTL_PGBL bit field */
#define ENET_PGBL_1BEAT                           DMA_BCTL_PGBL(1)                              /*!< maximum number of beats is 1 */
#define ENET_PGBL_2BEAT                           DMA_BCTL_PGBL(2)                              /*!< maximum number of beats is 2 */
#define ENET_PGBL_4BEAT                           DMA_BCTL_PGBL(4)                              /*!< maximum number of beats is 4 */
#define ENET_PGBL_8BEAT                           DMA_BCTL_PGBL(8)                              /*!< maximum number of beats is 8 */
#define ENET_PGBL_16BEAT                          DMA_BCTL_PGBL(16)                             /*!< maximum number of beats is 16 */
#define ENET_PGBL_32BEAT                          DMA_BCTL_PGBL(32)                             /*!< maximum number of beats is 32 */                
#define ENET_PGBL_4xPGBL_4BEAT                    (DMA_BCTL_PGBL(1)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats is 4 */
#define ENET_PGBL_4xPGBL_8BEAT                    (DMA_BCTL_PGBL(2)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats is 8 */
#define ENET_PGBL_4xPGBL_16BEAT                   (DMA_BCTL_PGBL(4)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats is 16 */
#define ENET_PGBL_4xPGBL_32BEAT                   (DMA_BCTL_PGBL(8)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats is 32 */
#define ENET_PGBL_4xPGBL_64BEAT                   (DMA_BCTL_PGBL(16)|ENET_DMA_BCTL_FPBL)        /*!< maximum number of beats is 64 */
#define ENET_PGBL_4xPGBL_128BEAT                  (DMA_BCTL_PGBL(32)|ENET_DMA_BCTL_FPBL)        /*!< maximum number of beats is 128 */

#define DMA_BCTL_RTPR(regval)                     (BITS(14,15) & ((uint32_t)(regval) << 14))    /*!< write value to ENET_DMA_BCTL_RTPR bit field */
#define ENET_ARBITRATION_RXTX_1_1                 DMA_BCTL_RTPR(0)                              /*!< receive and transmit priority ratio is 1:1*/
#define ENET_ARBITRATION_RXTX_2_1                 DMA_BCTL_RTPR(1)                              /*!< receive and transmit priority ratio is 2:1*/
#define ENET_ARBITRATION_RXTX_3_1                 DMA_BCTL_RTPR(2)                              /*!< receive and transmit priority ratio is 3:1 */
#define ENET_ARBITRATION_RXTX_4_1                 DMA_BCTL_RTPR(3)                              /*!< receive and transmit priority ratio is 4:1 */  
#define ENET_ARBITRATION_RXPRIORTX                ENET_DMA_BCTL_DAB                             /*!< RxDMA has higher priority than TxDMA */

#define ENET_FIXED_BURST_ENABLE                   ENET_DMA_BCTL_FB                              /*!< AHB can only use SINGLE/INCR4/INCR8/INCR16 during start of normal burst transfers */
#define ENET_FIXED_BURST_DISABLE                  ((uint32_t)0x00000000)                        /*!< AHB can use SINGLE/INCR burst transfer operations */

#define DMA_BCTL_RXDP(regval)                     (BITS(17,22) & ((uint32_t)(regval) << 17))    /*!< write value to ENET_DMA_BCTL_RXDP bit field */
#define ENET_RXDP_1BEAT                           DMA_BCTL_RXDP(1)                              /*!< maximum number of beats 1 */
#define ENET_RXDP_2BEAT                           DMA_BCTL_RXDP(2)                              /*!< maximum number of beats 2 */
#define ENET_RXDP_4BEAT                           DMA_BCTL_RXDP(4)                              /*!< maximum number of beats 4 */
#define ENET_RXDP_8BEAT                           DMA_BCTL_RXDP(8)                              /*!< maximum number of beats 8 */
#define ENET_RXDP_16BEAT                          DMA_BCTL_RXDP(16)                             /*!< maximum number of beats 16 */
#define ENET_RXDP_32BEAT                          DMA_BCTL_RXDP(32)                             /*!< maximum number of beats 32 */                
#define ENET_RXDP_4xPGBL_4BEAT                    (DMA_BCTL_RXDP(1)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats 4 */
#define ENET_RXDP_4xPGBL_8BEAT                    (DMA_BCTL_RXDP(2)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats 8 */
#define ENET_RXDP_4xPGBL_16BEAT                   (DMA_BCTL_RXDP(4)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats 16 */
#define ENET_RXDP_4xPGBL_32BEAT                   (DMA_BCTL_RXDP(8)|ENET_DMA_BCTL_FPBL)         /*!< maximum number of beats 32 */
#define ENET_RXDP_4xPGBL_64BEAT                   (DMA_BCTL_RXDP(16)|ENET_DMA_BCTL_FPBL)        /*!< maximum number of beats 64 */
#define ENET_RXDP_4xPGBL_128BEAT                  (DMA_BCTL_RXDP(32)|ENET_DMA_BCTL_FPBL)        /*!< maximum number of beats 128 */  

#define ENET_RXTX_DIFFERENT_PGBL                  ENET_DMA_BCTL_UIP                             /*!< RxDMA uses the RXDP[5:0], while TxDMA uses the PGBL[5:0] */
#define ENET_RXTX_SAME_PGBL                       ((uint32_t)0x00000000)                        /*!< RxDMA/TxDMA uses PGBL[5:0] */

#define ENET_ADDRESS_ALIGN_ENABLE                 ENET_DMA_BCTL_AA                              /*!< enabled address-aligned */
#define ENET_ADDRESS_ALIGN_DISABLE                ((uint32_t)0x00000000)                        /*!< disable address-aligned */

#define ENET_MIXED_BURST_ENABLE                   ENET_DMA_BCTL_MB                              /*!< AHB master interface transfer burst length greater than 16 with INCR */
#define ENET_MIXED_BURST_DISABLE                  ((uint32_t)0x00000000)                        /*!< AHB master interface only transfer fixed burst length with 16 and below */

/* dma_stat register value */
#define GET_DMA_STAT_RP(regval)                   GET_BITS((uint32_t)(regval),17,19)            /*!< get value of ENET_DMA_STAT_RP bit field */
#define ENET_RX_STATE_STOPPED                     ((uint32_t)0x00000000)                        /*!< reset or stop rx command issued */
#define ENET_RX_STATE_FETCHING                    BIT(17)                                       /*!< fetching the Rx descriptor */
#define ENET_RX_STATE_WAITING                     (BIT(17)|BIT(18))                             /*!< waiting for receive packet */
#define ENET_RX_STATE_SUSPENDED                   BIT(19)                                       /*!< Rx descriptor unavailable */
#define ENET_RX_STATE_CLOSING                     (BIT(17)|BIT(19))                             /*!< closing receive descriptor */
#define ENET_RX_STATE_QUEUING                     ENET_DMA_STAT_RP                              /*!< transferring the receive packet data from recevie buffer to host memory */

#define GET_DMA_STAT_TP(regval)                   GET_BITS((uint32_t)(regval),20,22)            /*!< get value of ENET_DMA_STAT_TP bit field */
#define ENET_TX_STATE_STOPPED                     ((uint32_t)0x00000000)                        /*!< reset or stop Tx Command issued  */
#define ENET_TX_STATE_FETCHING                    BIT(20)                                       /*!< fetching the Tx descriptor */
#define ENET_TX_STATE_WAITING                     BIT(21)                                       /*!< waiting for status */
#define ENET_TX_STATE_READING                     (BIT(20)|BIT(21))                             /*!< reading the data from host memory buffer and queuing it to transmit buffer */
#define ENET_TX_STATE_SUSPENDED                   (BIT(21)|BIT(22))                             /*!< Tx descriptor unavailabe or transmit buffer underflow */
#define ENET_TX_STATE_CLOSING                     ENET_DMA_STAT_TP                              /*!< closing Tx descriptor */

#define GET_DMA_STAT_EB(regval)                   GET_BITS((uint32_t)(regval),23,25)            /*!< get value of ENET_DMA_STAT_EB bit field */
#define ENET_ERROR_TXDATA_TRANSFER                BIT(23)                                       /*!< error during data transfer by TxDMA or RxDMA */
#define ENET_ERROR_READ_TRANSFER                  BIT(24)                                       /*!< error during write transfer or read transfer */
#define ENET_ERROR_DESC_ACCESS                    BIT(25)                                       /*!< error during descriptor or buffer access */

/* dma_ctl register value */
#define DMA_CTL_RTHC(regval)                      (BITS(3,4) & ((uint32_t)(regval) << 3))       /*!< write value to ENET_DMA_CTL_RTHC bit field */
#define ENET_RX_THRESHOLD_64BYTES                 DMA_CTL_RTHC(0)                               /*!< threshold level is 64 Bytes */
#define ENET_RX_THRESHOLD_32BYTES                 DMA_CTL_RTHC(1)                               /*!< threshold level is 32 Bytes */
#define ENET_RX_THRESHOLD_96BYTES                 DMA_CTL_RTHC(2)                               /*!< threshold level is 96 Bytes */
#define ENET_RX_THRESHOLD_128BYTES                DMA_CTL_RTHC(3)                               /*!< threshold level is 128 Bytes */

#define DMA_CTL_TTHC(regval)                      (BITS(14,16) & ((uint32_t)(regval) << 14))    /*!< write value to ENET_DMA_CTL_TTHC bit field */
#define ENET_TX_THRESHOLD_64BYTES                 DMA_CTL_TTHC(0)                               /*!< threshold level is 64 Bytes */
#define ENET_TX_THRESHOLD_128BYTES                DMA_CTL_TTHC(1)                               /*!< threshold level is 128 Bytes */
#define ENET_TX_THRESHOLD_192BYTES                DMA_CTL_TTHC(2)                               /*!< threshold level is 192 Bytes */
#define ENET_TX_THRESHOLD_256BYTES                DMA_CTL_TTHC(3)                               /*!< threshold level is 256 Bytes */
#define ENET_TX_THRESHOLD_40BYTES                 DMA_CTL_TTHC(4)                               /*!< threshold level is 40 Bytes */
#define ENET_TX_THRESHOLD_32BYTES                 DMA_CTL_TTHC(5)                               /*!< threshold level is 32 Bytes */
#define ENET_TX_THRESHOLD_24BYTES                 DMA_CTL_TTHC(6)                               /*!< threshold level is 24 Bytes */
#define ENET_TX_THRESHOLD_16BYTES                 DMA_CTL_TTHC(7)                               /*!< threshold level is 16 Bytes */

#define ENET_TCPIP_CKSUMERROR_ACCEPT              ENET_DMA_CTL_DTCERFD                          /*!< Rx frame with only payload error but no other errors will not be dropped */
#define ENET_TCPIP_CKSUMERROR_DROP                ((uint32_t)0x00000000)                        /*!< all error frames will be dropped when FERF = 0 */

#define ENET_RX_MODE_STOREFORWARD                 ENET_DMA_CTL_RSFD                             /*!< RxFIFO operates in store-and-forward mode */
#define ENET_RX_MODE_CUTTHROUGH                   ((uint32_t)0x00000000)                        /*!< RxFIFO operates in cut-through mode */

#define ENET_FLUSH_RXFRAME_ENABLE                 ((uint32_t)0x00000000)                        /*!< RxDMA flushes all frames */
#define ENET_FLUSH_RXFRAME_DISABLE                ENET_DMA_CTL_DAFRF                            /*!< RxDMA does not flush any frames */
#define ENET_NO_FLUSH_RXFRAME                     ENET_DMA_CTL_DAFRF                            /*!< RxDMA flushes frames function */

#define ENET_TX_MODE_STOREFORWARD                 ENET_DMA_CTL_TSFD                             /*!< TxFIFO operates in store-and-forward mode */
#define ENET_TX_MODE_CUTTHROUGH                   ((uint32_t)0x00000000)                        /*!< TxFIFO operates in cut-through mode */

#define ENET_FORWARD_ERRFRAMES_ENABLE             (ENET_DMA_CTL_FERF << 2)                      /*!< all frame received with error except runt error are forwarded to memory */
#define ENET_FORWARD_ERRFRAMES_DISABLE            ((uint32_t)0x00000000)                        /*!< RxFIFO drop error frame */
#define ENET_FORWARD_ERRFRAMES                    (ENET_DMA_CTL_FERF << 2)                      /*!< the function that all frame received with error except runt error are forwarded to memory */

#define ENET_FORWARD_UNDERSZ_GOODFRAMES_ENABLE    (ENET_DMA_CTL_FUF << 2)                       /*!< forward undersized good frames */
#define ENET_FORWARD_UNDERSZ_GOODFRAMES_DISABLE   ((uint32_t)0x00000000)                        /*!< RxFIFO drops all frames whose length is less than 64 bytes */  
#define ENET_FORWARD_UNDERSZ_GOODFRAMES           (ENET_DMA_CTL_FUF << 2)                       /*!< the function that forwarding undersized good frames */

#define ENET_SECONDFRAME_OPT_ENABLE               ENET_DMA_CTL_OSF                              /*!< TxDMA controller operate on second frame mode enable*/
#define ENET_SECONDFRAME_OPT_DISABLE              ((uint32_t)0x00000000)                        /*!< TxDMA controller operate on second frame mode disable */
#define ENET_SECONDFRAME_OPT                      ENET_DMA_CTL_OSF                              /*!< TxDMA controller operate on second frame function */
/* dma_mfbocnt register value */
#define GET_DMA_MFBOCNT_MSFC(regval)              GET_BITS((regval),0,15)                       /*!< get value of ENET_DMA_MFBOCNT_MSFC bit field */

#define GET_DMA_MFBOCNT_MSFA(regval)              GET_BITS((regval),17,27)                      /*!< get value of ENET_DMA_MFBOCNT_MSFA bit field */

/* dma_rswdc register value */
#define DMA_RSWDC_WDCFRS(regval)                  (BITS(0,7) & ((uint32_t)(regval) << 0))       /*!< write value to ENET_DMA_RSWDC_WDCFRS bit field */

/* dma tx descriptor tdes0 register value */
#define TDES0_CONT(regval)                        (BITS(3,6) & ((uint32_t)(regval) << 3))       /*!< write value to ENET DMA TDES0 CONT bit field */
#define GET_TDES0_COCNT(regval)                   GET_BITS((regval),3,6)                        /*!< get value of ENET DMA TDES0 CONT bit field */

#define TDES0_CM(regval)                          (BITS(22,23) & ((uint32_t)(regval) << 22))    /*!< write value to ENET DMA TDES0 CM bit field */
#define ENET_CHECKSUM_DISABLE                     TDES0_CM(0)                                   /*!< checksum insertion disabled */ 
#define ENET_CHECKSUM_IPV4HEADER                  TDES0_CM(1)                                   /*!< only IP header checksum calculation and insertion are enabled */ 
#define ENET_CHECKSUM_TCPUDPICMP_SEGMENT          TDES0_CM(2)                                   /*!< TCP/UDP/ICMP checksum insertion calculated but pseudo-header  */ 
#define ENET_CHECKSUM_TCPUDPICMP_FULL             TDES0_CM(3)                                   /*!< TCP/UDP/ICMP checksum insertion fully calculated */ 

/* dma tx descriptor tdes1 register value */
#define TDES1_TB1S(regval)                        (BITS(0,12) & ((uint32_t)(regval) << 0))      /*!< write value to ENET DMA TDES1 TB1S bit field */

#define TDES1_TB2S(regval)                        (BITS(16,28) & ((uint32_t)(regval) << 16))    /*!< write value to ENET DMA TDES1 TB2S bit field */

/* dma rx descriptor rdes0 register value */
#define RDES0_FRML(regval)                        (BITS(16,29) & ((uint32_t)(regval) << 16))    /*!< write value to ENET DMA RDES0 FRML bit field */
#define GET_RDES0_FRML(regval)                    GET_BITS((regval),16,29)                      /*!< get value of ENET DMA RDES0 FRML bit field */

/* dma rx descriptor rdes1 register value */
#define ENET_RECEIVE_COMPLETE_INT_ENABLE          ((uint32_t)0x00000000U)                       /*!< RS bit immediately set after Rx completed */
#define ENET_RECEIVE_COMPLETE_INT_DISABLE         ENET_RDES1_DINTC                              /*!< RS bit not immediately set after Rx completed */

#define GET_RDES1_RB1S(regval)                    GET_BITS((regval),0,12)                       /*!< get value of ENET DMA RDES1 RB1S bit field */

#define GET_RDES1_RB2S(regval)                    GET_BITS((regval),16,28)                      /*!< get value of ENET DMA RDES1 RB2S bit field */

/* dma rx descriptor rdes4 register value */
#define RDES4_IPPLDT(regval)                      (BITS(0,2) & ((uint32_t)(regval) << 0))       /*!< write value to ENET DMA RDES4 IPPLDT bit field */
#define GET_RDES4_IPPLDT(regval)                  GET_BITS((regval),0,2)                        /*!< get value of ENET DMA RDES4 IPPLDT bit field */

#define RDES4_PTPMT(regval)                       (BITS(8,11) & ((uint32_t)(regval) << 8))      /*!< write value to ENET DMA RDES4 PTPMT bit field */
#define GET_RDES4_PTPMT(regval)                   GET_BITS((regval),8,11)                       /*!< get value of ENET DMA RDES4 PTPMT bit field */

/* ENET register mask value */
#define MAC_CFG_MASK                              ((uint32_t)0xFD30810FU)                       /*!< ENET_MAC_CFG register mask */
#define MAC_FCTL_MASK                             ((uint32_t)0x0000FF41U)                       /*!< ENET_MAC_FCTL register mask */
#define DMA_CTL_MASK                              ((uint32_t)0xF8DE3F23U)                       /*!< ENET_DMA_CTL register mask */
#define DMA_BCTL_MASK                             ((uint32_t)0xF800007DU)                       /*!< ENET_DMA_BCTL register mask */
#define ENET_MSC_PRESET_MASK                      (~(ENET_MSC_CTL_PMC | ENET_MSC_CTL_AFHPM))    /*!< ENET_MSC_CTL preset mask */

#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
#define ETH_DMATXDESC_SIZE                        ((uint32_t)0x00000020U)                       /*!< TxDMA enhanced descriptor size */
#define ETH_DMARXDESC_SIZE                        ((uint32_t)0x00000020U)                       /*!< RxDMA enhanced descriptor size */
#else
#define ETH_DMATXDESC_SIZE                        ((uint32_t)0x00000010U)                       /*!< TxDMA descriptor size */
#define ETH_DMARXDESC_SIZE                        ((uint32_t)0x00000010U)                       /*!< RxDMA descriptor size */
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */ 

/* ENET remote wake-up frame register length */
#define ETH_WAKEUP_REGISTER_LENGTH                8U                                            /*!< remote wake-up frame register length */

/* ENET frame size */ 
#define ENET_MAX_FRAME_SIZE                       1524U                                         /*!< header + frame_extra + payload + CRC */    

/* ENET delay timeout */
#define ENET_DELAY_TO                             ((uint32_t)0x0004FFFFU)                       /*!< ENET delay timeout */
#define ENET_RESET_TO                             ((uint32_t)0x000004FFU)                       /*!< ENET reset timeout */



/* function declarations */
/* main function */
/* deinitialize the ENET, and reset structure parameters for ENET initialization */
void enet_deinit(void);
/* configure the parameters which are usually less cared for initialization */
void enet_initpara_config(enet_option_enum option, uint32_t para);
/* initialize ENET peripheral with generally concerned parameters and the less cared parameters */
ErrStatus enet_init(enet_mediamode_enum mediamode, enet_chksumconf_enum checksum, enet_frmrecept_enum recept);
/* reset all core internal registers located in CLK_TX and CLK_RX */
ErrStatus enet_software_reset(void);
/* check receive frame valid and return frame size */
uint32_t enet_rxframe_size_get(void);
/* initialize the dma tx/rx descriptors's parameters in chain mode */
void enet_descriptors_chain_init(enet_dmadirection_enum direction);
/* initialize the dma tx/rx descriptors's parameters in ring mode */
void enet_descriptors_ring_init(enet_dmadirection_enum direction);
/* handle current received frame data to application buffer */
ErrStatus enet_frame_receive(uint8_t *buffer, uint32_t bufsize);
/* handle current received frame but without data copy to application buffer */
#define ENET_NOCOPY_FRAME_RECEIVE()         enet_frame_receive(NULL, 0U)
/* handle application buffer data to transmit it */
ErrStatus enet_frame_transmit(uint8_t *buffer, uint32_t length);
/* handle current transmit frame but without data copy from application buffer */
#define ENET_NOCOPY_FRAME_TRANSMIT(len)     enet_frame_transmit(NULL, (len))
/* configure the transmit IP frame checksum offload calculation and insertion */
void enet_transmit_checksum_config(enet_descriptors_struct *desc, uint32_t checksum);
/* ENET Tx and Rx function enable (include MAC and DMA module) */
void enet_enable(void);   
/* ENET Tx and Rx function disable (include MAC and DMA module) */
void enet_disable(void);
/* configure MAC address */
void enet_mac_address_set(enet_macaddress_enum mac_addr, uint8_t paddr[]);
/* get MAC address */   
void enet_mac_address_get(enet_macaddress_enum mac_addr, uint8_t paddr[]);

/* get the ENET MAC/MSC/PTP/DMA status flag */
FlagStatus enet_flag_get(enet_flag_enum enet_flag);
/* clear the ENET DMA status flag */
void enet_flag_clear(enet_flag_clear_enum enet_flag);
/* enable ENET MAC/MSC/DMA interrupt */
void enet_interrupt_enable(enet_int_enum enet_int);
/* disable ENET MAC/MSC/DMA interrupt */
void enet_interrupt_disable(enet_int_enum enet_int);
/* get ENET MAC/MSC/DMA interrupt flag */
FlagStatus enet_interrupt_flag_get(enet_int_flag_enum int_flag);
/* clear ENET DMA interrupt flag */
void enet_interrupt_flag_clear(enet_int_flag_clear_enum int_flag_clear);

/* MAC function */
/* ENET Tx function enable (include MAC and DMA module) */
void enet_tx_enable(void);
/* ENET Tx function disable (include MAC and DMA module) */
void enet_tx_disable(void);
/* ENET Rx function enable (include MAC and DMA module) */
void enet_rx_enable(void);
/* ENET Rx function disable (include MAC and DMA module) */
void enet_rx_disable(void);
/* put registers value into the application buffer */
void enet_registers_get(enet_registers_type_enum type, uint32_t *preg, uint32_t num);
/* get the enet debug status from the debug register */
uint32_t enet_debug_status_get(uint32_t mac_debug);
/* enable the MAC address filter */
void enet_address_filter_enable(enet_macaddress_enum mac_addr);
/* disable the MAC address filter */
void enet_address_filter_disable(enet_macaddress_enum mac_addr);
/* configure the MAC address filter */
void enet_address_filter_config(enet_macaddress_enum mac_addr, uint32_t addr_mask, uint32_t filter_type);
/* PHY interface configuration (configure SMI clock and reset PHY chip) */
ErrStatus enet_phy_config(void);
/* write to/read from a PHY register */
ErrStatus enet_phy_write_read(enet_phydirection_enum direction, uint16_t phy_address, uint16_t phy_reg, uint16_t *pvalue);
/* enable the loopback function of phy chip */
ErrStatus enet_phyloopback_enable(void);
/* disable the loopback function of phy chip */
ErrStatus enet_phyloopback_disable(void);
/* enable ENET forward feature */
void enet_forward_feature_enable(uint32_t feature);
/* disable ENET forward feature */
void enet_forward_feature_disable(uint32_t feature);
/* enable ENET fliter feature */
void enet_fliter_feature_enable(uint32_t feature);
/* disable ENET fliter feature */
void enet_fliter_feature_disable(uint32_t feature);

/* flow control function */
/* generate the pause frame, ENET will send pause frame after enable transmit flow control */
ErrStatus enet_pauseframe_generate(void);
/* configure the pause frame detect type */
void enet_pauseframe_detect_config(uint32_t detect);
/* configure the pause frame parameters */
void enet_pauseframe_config(uint32_t pausetime, uint32_t pause_threshold);
/* configure the threshold of the flow control(deactive and active threshold) */
void enet_flowcontrol_threshold_config(uint32_t deactive, uint32_t active);
/* enable ENET flow control feature */
void enet_flowcontrol_feature_enable(uint32_t feature);
/* disable ENET flow control feature */
void enet_flowcontrol_feature_disable(uint32_t feature);

/* DMA function */
/* get the dma transmit/receive process state */
uint32_t enet_dmaprocess_state_get(enet_dmadirection_enum direction); 
/* poll the dma transmission/reception enable */
void enet_dmaprocess_resume(enet_dmadirection_enum direction);
/* check and recover the Rx process */
void enet_rxprocess_check_recovery(void);
/* flush the ENET transmit fifo, and wait until the flush operation completes */
ErrStatus enet_txfifo_flush(void);
/* get the transmit/receive address of current descriptor, or current buffer, or descriptor table */
uint32_t enet_current_desc_address_get(enet_desc_reg_enum addr_get);
/* get the Tx or Rx descriptor information */
uint32_t enet_desc_information_get(enet_descriptors_struct *desc, enet_descstate_enum info_get);
/* get the number of missed frames during receiving */
void enet_missed_frame_counter_get(uint32_t *rxfifo_drop, uint32_t *rxdma_drop);

/* descriptor function */
/* get the bit flag of ENET dma descriptor */
FlagStatus enet_desc_flag_get(enet_descriptors_struct *desc, uint32_t desc_flag);
/* set the bit flag of ENET dma tx descriptor */
void enet_desc_flag_set(enet_descriptors_struct *desc, uint32_t desc_flag);
/* clear the bit flag of ENET dma tx descriptor */
void enet_desc_flag_clear(enet_descriptors_struct *desc, uint32_t desc_flag); 
/* when receiving the completed, set RS bit in ENET_DMA_STAT register will immediately set */
void enet_rx_desc_immediate_receive_complete_interrupt(enet_descriptors_struct *desc);
/* when receiving the completed, set RS bit in ENET_DMA_STAT register will is set after a configurable delay time */
void enet_rx_desc_delay_receive_complete_interrupt(enet_descriptors_struct *desc, uint32_t delay_time);
/* drop current receive frame */
void enet_rxframe_drop(void);
/* enable DMA feature */
void enet_dma_feature_enable(uint32_t feature);
/* disable DMA feature */
void enet_dma_feature_disable(uint32_t feature);


/* special enhanced mode function */
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
/* get the bit of extended status flag in ENET DMA descriptor */
uint32_t enet_rx_desc_enhanced_status_get(enet_descriptors_struct *desc, uint32_t desc_status);
/* configure descriptor to work in enhanced mode */
void enet_desc_select_enhanced_mode(void);
/* initialize the dma Tx/Rx descriptors's parameters in enhanced chain mode with ptp function */
void enet_ptp_enhanced_descriptors_chain_init(enet_dmadirection_enum direction);
/* initialize the dma Tx/Rx descriptors's parameters in enhanced ring mode with ptp function */
void enet_ptp_enhanced_descriptors_ring_init(enet_dmadirection_enum direction);
/* receive a packet data with timestamp values to application buffer, when the DMA is in enhanced mode */
ErrStatus enet_ptpframe_receive_enhanced_mode(uint8_t *buffer, uint32_t bufsize, uint32_t timestamp[]);
/* handle current received frame but without data copy to application buffer in PTP enhanced mode */
#define ENET_NOCOPY_PTPFRAME_RECEIVE_ENHANCED_MODE(ptr)           enet_ptpframe_receive_enhanced_mode(NULL, 0U, (ptr))
/* send data with timestamp values in application buffer as a transmit packet, when the DMA is in enhanced mode */
ErrStatus enet_ptpframe_transmit_enhanced_mode(uint8_t *buffer, uint32_t length, uint32_t timestamp[]);
/* handle current transmit frame but without data copy from application buffer in PTP enhanced mode */
#define ENET_NOCOPY_PTPFRAME_TRANSMIT_ENHANCED_MODE(len, ptr)     enet_ptpframe_transmit_enhanced_mode(NULL, (len), (ptr))

#else

/* configure descriptor to work in normal mode */
void enet_desc_select_normal_mode(void);
/* initialize the dma Tx/Rx descriptors's parameters in normal chain mode with ptp function */
void enet_ptp_normal_descriptors_chain_init(enet_dmadirection_enum direction, enet_descriptors_struct *desc_ptptab);
/* initialize the dma Tx/Rx descriptors's parameters in normal ring mode with ptp function */
void enet_ptp_normal_descriptors_ring_init(enet_dmadirection_enum direction, enet_descriptors_struct *desc_ptptab);
/* receive a packet data with timestamp values to application buffer, when the DMA is in normal mode */
ErrStatus enet_ptpframe_receive_normal_mode(uint8_t *buffer, uint32_t bufsize, uint32_t timestamp[]);
/* handle current received frame but without data copy to application buffer in PTP normal mode */
#define ENET_NOCOPY_PTPFRAME_RECEIVE_NORMAL_MODE(ptr)             enet_ptpframe_receive_normal_mode(NULL, 0U, (ptr))
/* send data with timestamp values in application buffer as a transmit packet, when the DMA is in normal mode */
ErrStatus enet_ptpframe_transmit_normal_mode(uint8_t *buffer, uint32_t length, uint32_t timestamp[]);
/* handle current transmit frame but without data copy from application buffer in PTP normal mode */
#define ENET_NOCOPY_PTPFRAME_TRANSMIT_NORMAL_MODE(len, ptr)       enet_ptpframe_transmit_normal_mode(NULL, (len), (ptr))

#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

/* WUM function */
/* wakeup frame filter register pointer reset */
void enet_wum_filter_register_pointer_reset(void);
/* set the remote wakeup frame registers */
void enet_wum_filter_config(uint32_t pdata[]);
/* enable wakeup management features */
void enet_wum_feature_enable(uint32_t feature);
/* disable wakeup management features */
void enet_wum_feature_disable(uint32_t feature);

/* MSC function */
/* reset the MAC statistics counters */
void enet_msc_counters_reset(void);
/* enable the MAC statistics counter features */ 
void enet_msc_feature_enable(uint32_t feature);
/* disable the MAC statistics counter features */ 
void enet_msc_feature_disable(uint32_t feature);
/* configure MAC statistics counters preset mode */
void enet_msc_counters_preset_config(enet_msc_preset_enum mode);
/* get MAC statistics counter */                   
uint32_t enet_msc_counters_get(enet_msc_counter_enum counter);

/* PTP function */
/* enable the PTP features */
void enet_ptp_feature_enable(uint32_t feature);
/* disable the PTP features */
void enet_ptp_feature_disable(uint32_t feature);
/* configure the PTP timestamp function */
ErrStatus enet_ptp_timestamp_function_config(enet_ptp_function_enum func);
/* configure the PTP system time subsecond increment value */
void enet_ptp_subsecond_increment_config(uint32_t subsecond);
/* adjusting the PTP clock frequency only in fine update mode */
void enet_ptp_timestamp_addend_config(uint32_t add);
/* initializing or adding/subtracting to second of the PTP system time */
void enet_ptp_timestamp_update_config(uint32_t sign, uint32_t second, uint32_t subsecond);
/* configure the PTP expected target time */
void enet_ptp_expected_time_config(uint32_t second, uint32_t nanosecond);
/* get the PTP current system time */
void enet_ptp_system_time_get(enet_ptp_systime_struct *systime_struct);
/* configure the PPS output frequency */
void enet_ptp_pps_output_frequency_config(uint32_t freq);


/* internal function */
/* reset the ENET initpara struct, call it before using enet_initpara_config() */
void enet_initpara_reset(void);


#endif /* GD32F4XX_ENET_H */
