/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_eth.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the
*                      ETH firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32V30x_ETH_H
#define __CH32V30x_ETH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


#define PHY_10BASE_T_LINKED   1
#define PHY_10BASE_T_NOT_LINKED   0

#define DMA_TPS_Mask      ((uint32_t)0x00700000) 	
#define DMA_RPS_Mask      ((uint32_t)0x000E0000) 	

/* ETH Init structure definition */
typedef struct {
    uint32_t             ETH_AutoNegotiation;             /* Selects or not the AutoNegotiation mode for the external PHY
                                                             The AutoNegotiation allows an automatic setting of the Speed (10/100Mbps)
                                                             and the mode (half/full-duplex).
                                                             This parameter can be a value of @ref ETH_AutoNegotiation */

    uint32_t             ETH_Watchdog;                    /* Selects or not the Watchdog timer
                                                             When enabled, the MAC allows no more then 2048 bytes to be received.
                                                             When disabled, the MAC can receive up to 16384 bytes.
                                                             This parameter can be a value of @ref ETH_watchdog */

    uint32_t             ETH_Jabber;                      /* Selects or not Jabber timer
                                                             When enabled, the MAC allows no more then 2048 bytes to be sent.
                                                             When disabled, the MAC can send up to 16384 bytes.
                                                             This parameter can be a value of @ref ETH_Jabber */

    uint32_t             ETH_InterFrameGap;               /* Selects the minimum IFG between frames during transmission
                                                             This parameter can be a value of @ref ETH_Inter_Frame_Gap */

    uint32_t             ETH_CarrierSense;                /* Selects or not the Carrier Sense
                                                             This parameter can be a value of @ref ETH_Carrier_Sense */

    uint32_t             ETH_Speed;                       /* Sets the Ethernet speed: 10/100 Mbps
                                                             This parameter can be a value of @ref ETH_Speed */

    uint32_t             ETH_ReceiveOwn;                  /* Selects or not the ReceiveOwn
                                                             ReceiveOwn allows the reception of frames when the TX_EN signal is asserted
                                                             in Half-Duplex mode
                                                             This parameter can be a value of @ref ETH_Receive_Own */

    uint32_t             ETH_LoopbackMode;                /* Selects or not the internal MAC MII Loopback mode
                                                             This parameter can be a value of @ref ETH_Loop_Back_Mode */

    uint32_t             ETH_Mode;                        /* Selects the MAC duplex mode: Half-Duplex or Full-Duplex mode
                                                             This parameter can be a value of @ref ETH_Duplex_Mode */

    uint32_t             ETH_ChecksumOffload;             /* Selects or not the IPv4 checksum checking for received frame payloads' TCP/UDP/ICMP headers.
                                                             This parameter can be a value of @ref ETH_Checksum_Offload */

    uint32_t             ETH_RetryTransmission;           /* Selects or not the MAC attempt retries transmission, based on the settings of BL,
                                                             when a colision occurs (Half-Duplex mode)
                                                             This parameter can be a value of @ref ETH_Retry_Transmission */

    uint32_t             ETH_AutomaticPadCRCStrip;        /* Selects or not the Automatic MAC Pad/CRC Stripping
                                                             This parameter can be a value of @ref ETH_Automatic_Pad_CRC_Strip */

    uint32_t             ETH_BackOffLimit;                /* Selects the BackOff limit value
                                                             This parameter can be a value of @ref ETH_Back_Off_Limit */

    uint32_t             ETH_DeferralCheck;               /* Selects or not the deferral check function (Half-Duplex mode)
                                                             This parameter can be a value of @ref ETH_Deferral_Check */

    uint32_t             ETH_ReceiveAll;                  /* Selects or not all frames reception by the MAC (No fitering)
                                                             This parameter can be a value of @ref ETH_Receive_All */

    uint32_t             ETH_SourceAddrFilter;            /* Selects the Source Address Filter mode
                                                             This parameter can be a value of @ref ETH_Source_Addr_Filter */

    uint32_t             ETH_PassControlFrames;           /* Sets the forwarding mode of the control frames (including unicast and multicast PAUSE frames)
                                                             This parameter can be a value of @ref ETH_Pass_Control_Frames */

    uint32_t             ETH_BroadcastFramesReception;    /* Selects or not the reception of Broadcast Frames
                                                             This parameter can be a value of @ref ETH_Broadcast_Frames_Reception */

    uint32_t             ETH_DestinationAddrFilter;       /* Sets the destination filter mode for both unicast and multicast frames
                                                             This parameter can be a value of @ref ETH_Destination_Addr_Filter */

    uint32_t             ETH_PromiscuousMode;             /* Selects or not the Promiscuous Mode
                                                             This parameter can be a value of @ref ETH_Promiscuous_Mode */

    uint32_t             ETH_MulticastFramesFilter;       /* Selects the Multicast Frames filter mode: None/HashTableFilter/PerfectFilter/PerfectHashTableFilter
                                                             This parameter can be a value of @ref ETH_Multicast_Frames_Filter */

    uint32_t             ETH_UnicastFramesFilter;         /* Selects the Unicast Frames filter mode: HashTableFilter/PerfectFilter/PerfectHashTableFilter
                                                             This parameter can be a value of @ref ETH_Unicast_Frames_Filter */

    uint32_t             ETH_HashTableHigh;               /* This field holds the higher 32 bits of Hash table.  */

    uint32_t             ETH_HashTableLow;                /* This field holds the lower 32 bits of Hash table.  */

    uint32_t             ETH_PauseTime;                   /* This field holds the value to be used in the Pause Time field in the
                                                             transmit control frame */

    uint32_t             ETH_ZeroQuantaPause;             /* Selects or not the automatic generation of Zero-Quanta Pause Control frames
                                                             This parameter can be a value of @ref ETH_Zero_Quanta_Pause */

    uint32_t             ETH_PauseLowThreshold;           /* This field configures the threshold of the PAUSE to be checked for
                                                             automatic retransmission of PAUSE Frame
                                                             This parameter can be a value of @ref ETH_Pause_Low_Threshold */
                                                           
    uint32_t             ETH_UnicastPauseFrameDetect;     /* Selects or not the MAC detection of the Pause frames (with MAC Address0
                                                             unicast address and unique multicast address)
                                                             This parameter can be a value of @ref ETH_Unicast_Pause_Frame_Detect */

    uint32_t             ETH_ReceiveFlowControl;          /* Enables or disables the MAC to decode the received Pause frame and
                                                             disable its transmitter for a specified time (Pause Time)
                                                             This parameter can be a value of @ref ETH_Receive_Flow_Control */

    uint32_t             ETH_TransmitFlowControl;         /* Enables or disables the MAC to transmit Pause frames (Full-Duplex mode)
                                                             or the MAC back-pressure operation (Half-Duplex mode)
                                                             This parameter can be a value of @ref ETH_Transmit_Flow_Control */

    uint32_t             ETH_VLANTagComparison;           /* Selects the 12-bit VLAN identifier or the complete 16-bit VLAN tag for
                                                             comparison and filtering
                                                             This parameter can be a value of @ref ETH_VLAN_Tag_Comparison */

    uint32_t             ETH_VLANTagIdentifier;           /* Holds the VLAN tag identifier for receive frames */

    uint32_t             ETH_DropTCPIPChecksumErrorFrame; /* Selects or not the Dropping of TCP/IP Checksum Error Frames
                                                             This parameter can be a value of @ref ETH_Drop_TCP_IP_Checksum_Error_Frame */ 

    uint32_t             ETH_ReceiveStoreForward;         /* Enables or disables the Receive store and forward mode
                                                             This parameter can be a value of @ref ETH_Receive_Store_Forward */ 

    uint32_t             ETH_FlushReceivedFrame;          /* Enables or disables the flushing of received frames
                                                             This parameter can be a value of @ref ETH_Flush_Received_Frame */ 

    uint32_t             ETH_TransmitStoreForward;        /* Enables or disables Transmit store and forward mode
                                                             This parameter can be a value of @ref ETH_Transmit_Store_Forward */ 

    uint32_t             ETH_TransmitThresholdControl;    /* Selects or not the Transmit Threshold Control
                                                             This parameter can be a value of @ref ETH_Transmit_Threshold_Control */

    uint32_t             ETH_ForwardErrorFrames;          /* Selects or not the forward to the DMA of erroneous frames
                                                             This parameter can be a value of @ref ETH_Forward_Error_Frames */

    uint32_t             ETH_ForwardUndersizedGoodFrames; /* Enables or disables the Rx FIFO to forward Undersized frames (frames with no Error
                                                             and length less than 64 bytes) including pad-bytes and CRC)
                                                             This parameter can be a value of @ref ETH_Forward_Undersized_Good_Frames */

    uint32_t             ETH_ReceiveThresholdControl;     /* Selects the threshold level of the Receive FIFO
                                                             This parameter can be a value of @ref ETH_Receive_Threshold_Control */

    uint32_t             ETH_SecondFrameOperate;          /* Selects or not the Operate on second frame mode, which allows the DMA to process a second
                                                             frame of Transmit data even before obtaining the status for the first frame.
                                                             This parameter can be a value of @ref ETH_Second_Frame_Operate */

    uint32_t             ETH_AddressAlignedBeats;         /* Enables or disables the Address Aligned Beats
                                                             This parameter can be a value of @ref ETH_Address_Aligned_Beats */

    uint32_t             ETH_FixedBurst;                  /* Enables or disables the AHB Master interface fixed burst transfers
                                                             This parameter can be a value of @ref ETH_Fixed_Burst */
                       
    uint32_t             ETH_RxDMABurstLength;            /* Indicates the maximum number of beats to be transferred in one Rx DMA transaction
                                                             This parameter can be a value of @ref ETH_Rx_DMA_Burst_Length */ 

    uint32_t             ETH_TxDMABurstLength;            /* Indicates sthe maximum number of beats to be transferred in one Tx DMA transaction
                                                             This parameter can be a value of @ref ETH_Tx_DMA_Burst_Length */                                                   

    uint32_t             ETH_DescriptorSkipLength;        /* Specifies the number of word to skip between two unchained descriptors (Ring mode) */

    uint32_t             ETH_DMAArbitration;              /* Selects the DMA Tx/Rx arbitration
                                                             This parameter can be a value of @ref ETH_DMA_Arbitration */  
}ETH_InitTypeDef;



/* ETH delay.Just for Ethernet */
#define _eth_delay_    ETH_Delay       /* Default _eth_delay_ function with less precise timing */

/* definition for Ethernet frame */
#define ETH_MAX_PACKET_SIZE    1520    /* ETH_HEADER + ETH_EXTRA + MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER               14    /* 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC                   4    /* Ethernet CRC */
#define ETH_EXTRA                 2    /* Extra bytes in some cases */
#define VLAN_TAG                  4    /* optional 802.1q VLAN Tag */
#define MIN_ETH_PAYLOAD          46    /* Minimum Ethernet payload size */
#define MAX_ETH_PAYLOAD        1500    /* Maximum Ethernet payload size */
#define JUMBO_FRAME_PAYLOAD    9000    /* Jumbo frame payload size */

/* ETH DMA structure definition */
typedef struct
{
  uint32_t   Status;                /* Status */
  uint32_t   ControlBufferSize;     /* Control and Buffer1, Buffer2 lengths */
  uint32_t   Buffer1Addr;           /* Buffer1 address pointer */
  uint32_t   Buffer2NextDescAddr;   /* Buffer2 or next descriptor address pointer */
} ETH_DMADESCTypeDef;

/**
   DMA Tx Desciptor
  -----------------------------------------------------------------------------------------------
  TDES0 | OWN(31) | CTRL[30:26] | Reserved[25:24] | CTRL[23:20] | Reserved[19:17] | Status[16:0] |
  -----------------------------------------------------------------------------------------------
  TDES1 | Reserved[31:29] | Buffer2 ByteCount[28:16] | Reserved[15:13] | Buffer1 ByteCount[12:0] |
  -----------------------------------------------------------------------------------------------
  TDES2 |                         Buffer1 Address [31:0]                                         |
  -----------------------------------------------------------------------------------------------
  TDES3 |                   Buffer2 Address [31:0] / Next Desciptor Address [31:0]               |
  ------------------------------------------------------------------------------------------------
*/


/* Bit or field definition of TDES0 register (DMA Tx descriptor status register)*/
#define ETH_DMATxDesc_OWN                     ((uint32_t)0x80000000)  /* OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATxDesc_IC                      ((uint32_t)0x40000000)  /* Interrupt on Completion */
#define ETH_DMATxDesc_LS                      ((uint32_t)0x20000000)  /* Last Segment */
#define ETH_DMATxDesc_FS                      ((uint32_t)0x10000000)  /* First Segment */
#define ETH_DMATxDesc_DC                      ((uint32_t)0x08000000)  /* Disable CRC */
#define ETH_DMATxDesc_DP                      ((uint32_t)0x04000000)  /* Disable Padding */
#define ETH_DMATxDesc_TTSE                    ((uint32_t)0x02000000)  /* Transmit Time Stamp Enable */
#define ETH_DMATxDesc_CIC                     ((uint32_t)0x00C00000)  /* Checksum Insertion Control: 4 cases */
#define ETH_DMATxDesc_CIC_ByPass              ((uint32_t)0x00000000)  /* Do Nothing: Checksum Engine is bypassed */
#define ETH_DMATxDesc_CIC_IPV4Header          ((uint32_t)0x00400000)  /* IPV4 header Checksum Insertion */
#define ETH_DMATxDesc_CIC_TCPUDPICMP_Segment  ((uint32_t)0x00800000)  /* TCP/UDP/ICMP Checksum Insertion calculated over segment only */
#define ETH_DMATxDesc_CIC_TCPUDPICMP_Full     ((uint32_t)0x00C00000)  /* TCP/UDP/ICMP Checksum Insertion fully calculated */
#define ETH_DMATxDesc_TER                     ((uint32_t)0x00200000)  /* Transmit End of Ring */
#define ETH_DMATxDesc_TCH                     ((uint32_t)0x00100000)  /* Second Address Chained */
#define ETH_DMATxDesc_TTSS                    ((uint32_t)0x00020000)  /* Tx Time Stamp Status */
#define ETH_DMATxDesc_IHE                     ((uint32_t)0x00010000)  /* IP Header Error */
#define ETH_DMATxDesc_ES                      ((uint32_t)0x00008000)  /* Error summary: OR of the following bits: UE || ED || EC || LCO || NC || LCA || FF || JT */
#define ETH_DMATxDesc_JT                      ((uint32_t)0x00004000)  /* Jabber Timeout */
#define ETH_DMATxDesc_FF                      ((uint32_t)0x00002000)  /* Frame Flushed: DMA/MTL flushed the frame due to SW flush */
#define ETH_DMATxDesc_PCE                     ((uint32_t)0x00001000)  /* Payload Checksum Error */
#define ETH_DMATxDesc_LCA                     ((uint32_t)0x00000800)  /* Loss of Carrier: carrier lost during tramsmission */
#define ETH_DMATxDesc_NC                      ((uint32_t)0x00000400)  /* No Carrier: no carrier signal from the tranceiver */
#define ETH_DMATxDesc_LCO                     ((uint32_t)0x00000200)  /* Late Collision: transmission aborted due to collision */
#define ETH_DMATxDesc_EC                      ((uint32_t)0x00000100)  /* Excessive Collision: transmission aborted after 16 collisions */
#define ETH_DMATxDesc_VF                      ((uint32_t)0x00000080)  /* VLAN Frame */
#define ETH_DMATxDesc_CC                      ((uint32_t)0x00000078)  /* Collision Count */
#define ETH_DMATxDesc_ED                      ((uint32_t)0x00000004)  /* Excessive Deferral */
#define ETH_DMATxDesc_UF                      ((uint32_t)0x00000002)  /* Underflow Error: late data arrival from the memory */
#define ETH_DMATxDesc_DB                      ((uint32_t)0x00000001)  /* Deferred Bit */

/* Field definition of TDES1 register */
#define ETH_DMATxDesc_TBS2  ((uint32_t)0x1FFF0000)  /* Transmit Buffer2 Size */
#define ETH_DMATxDesc_TBS1  ((uint32_t)0x00001FFF)  /* Transmit Buffer1 Size */

/* Field definition of TDES2 register */
#define ETH_DMATxDesc_B1AP  ((uint32_t)0xFFFFFFFF)  /* Buffer1 Address Pointer */

/* Field definition of TDES3 register */
#define ETH_DMATxDesc_B2AP  ((uint32_t)0xFFFFFFFF)  /* Buffer2 Address Pointer */

/**
  DMA Rx Desciptor
  ---------------------------------------------------------------------------------------------------------------------
  RDES0 | OWN(31) |                                             Status [30:0]                                          |
  ---------------------------------------------------------------------------------------------------------------------
  RDES1 | CTRL(31) | Reserved[30:29] | Buffer2 ByteCount[28:16] | CTRL[15:14] | Reserved(13) | Buffer1 ByteCount[12:0] |
  ---------------------------------------------------------------------------------------------------------------------
  RDES2 |                                       Buffer1 Address [31:0]                                                 |
  ---------------------------------------------------------------------------------------------------------------------
  RDES3 |                          Buffer2 Address [31:0] / Next Desciptor Address [31:0]                              |
  ----------------------------------------------------------------------------------------------------------------------
*/

/* Bit or field definition of RDES0 register (DMA Rx descriptor status register) */
#define ETH_DMARxDesc_OWN         ((uint32_t)0x80000000)  /* OWN bit: descriptor is owned by DMA engine  */
#define ETH_DMARxDesc_AFM         ((uint32_t)0x40000000)  /* DA Filter Fail for the rx frame  */
#define ETH_DMARxDesc_FL          ((uint32_t)0x3FFF0000)  /* Receive descriptor frame length  */
#define ETH_DMARxDesc_ES          ((uint32_t)0x00008000)  /* Error summary: OR of the following bits: DE || OE || IPC || LC || RWT || RE || CE */
#define ETH_DMARxDesc_DE          ((uint32_t)0x00004000)  /* Desciptor error: no more descriptors for receive frame  */
#define ETH_DMARxDesc_SAF         ((uint32_t)0x00002000)  /* SA Filter Fail for the received frame */
#define ETH_DMARxDesc_LE          ((uint32_t)0x00001000)  /* Frame size not matching with length field */
#define ETH_DMARxDesc_OE          ((uint32_t)0x00000800)  /* Overflow Error: Frame was damaged due to buffer overflow */
#define ETH_DMARxDesc_VLAN        ((uint32_t)0x00000400)  /* VLAN Tag: received frame is a VLAN frame */
#define ETH_DMARxDesc_FS          ((uint32_t)0x00000200)  /* First descriptor of the frame  */
#define ETH_DMARxDesc_LS          ((uint32_t)0x00000100)  /* Last descriptor of the frame  */
#define ETH_DMARxDesc_IPV4HCE     ((uint32_t)0x00000080)  /* IPC Checksum Error: Rx Ipv4 header checksum error   */
#define ETH_DMARxDesc_LC          ((uint32_t)0x00000040)  /* Late collision occurred during reception   */
#define ETH_DMARxDesc_FT          ((uint32_t)0x00000020)  /* Frame type - Ethernet, otherwise 802.3    */
#define ETH_DMARxDesc_RWT         ((uint32_t)0x00000010)  /* Receive Watchdog Timeout: watchdog timer expired during reception    */
#define ETH_DMARxDesc_RE          ((uint32_t)0x00000008)  /* Receive error: error reported by MII interface  */
#define ETH_DMARxDesc_DBE         ((uint32_t)0x00000004)  /* Dribble bit error: frame contains non int multiple of 8 bits  */
#define ETH_DMARxDesc_CE          ((uint32_t)0x00000002)  /* CRC error */
#define ETH_DMARxDesc_MAMPCE      ((uint32_t)0x00000001)  /* Rx MAC Address/Payload Checksum Error: Rx MAC address matched/ Rx Payload Checksum Error */

/* Bit or field definition of RDES1 register */
#define ETH_DMARxDesc_DIC   ((uint32_t)0x80000000)  /* Disable Interrupt on Completion */
#define ETH_DMARxDesc_RBS2  ((uint32_t)0x1FFF0000)  /* Receive Buffer2 Size */
#define ETH_DMARxDesc_RER   ((uint32_t)0x00008000)  /* Receive End of Ring */
#define ETH_DMARxDesc_RCH   ((uint32_t)0x00004000)  /* Second Address Chained */
#define ETH_DMARxDesc_RBS1  ((uint32_t)0x00001FFF)  /* Receive Buffer1 Size */

/* Field definition of RDES2 register */
#define ETH_DMARxDesc_B1AP  ((uint32_t)0xFFFFFFFF)  /* Buffer1 Address Pointer */

/* Field definition of RDES3 register */
#define ETH_DMARxDesc_B2AP  ((uint32_t)0xFFFFFFFF)  /* Buffer2 Address Pointer */

/* Timeout threshold of Reading or writing PHY registers */
#define PHY_READ_TO                     ((uint32_t)0x004FFFFF)
#define PHY_WRITE_TO                    ((uint32_t)0x0004FFFF)

/* Delay time after reset PHY */
#define PHY_ResetDelay                  ((uint32_t)0x000FFFFF) 

/* Delay time after configure PHY */
#define PHY_ConfigDelay                 ((uint32_t)0x00FFFFFF)

/* PHY basic register */
#define PHY_BCR                          0          /*PHY tranceiver Basic Control Register */
#define PHY_BSR                          1          /*PHY tranceiver Basic Status Register */
#define PHY_BMCR                         PHY_BCR
#define PHY_BMSR                         PHY_BSR

/* Bit or field definition for PHY basic control register */
#define PHY_Reset                       ((uint16_t)0x8000)      /* PHY Reset */
#define PHY_Loopback                    ((uint16_t)0x4000)      /* Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)      /* Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000)      /* Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100)      /* Set the full-duplex mode at 10 Mb/s */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000)      /* Set the half-duplex mode at 10 Mb/s */
#define PHY_AutoNegotiation             ((uint16_t)0x1000)      /* Enable auto-negotiation function */
#define PHY_Restart_AutoNegotiation     ((uint16_t)0x0200)      /* Restart auto-negotiation function */
#define PHY_Powerdown                   ((uint16_t)0x0800)      /* Select the power down mode */
#define PHY_Isolate                     ((uint16_t)0x0400)      /* Isolate PHY from MII */

/* Bit or field definition for PHY basic status register */
#define PHY_AutoNego_Complete           ((uint16_t)0x0020)      /* Auto-Negotioation process completed */
#define PHY_Linked_Status               ((uint16_t)0x0004)      /* Valid link established */
#define PHY_Jabber_detection            ((uint16_t)0x0002)      /* Jabber condition detected */
#define PHY_RMII_Mode                   ((uint16_t)0x0020)      /* RMII */


/* Internal 10BASE-T PHY 50R*4 pull-up resistance enable or disable */
#define ETH_Internal_Pull_Up_Res_Enable     ((uint32_t)0x00100000)
#define ETH_Internal_Pull_Up_Res_Disable    ((uint32_t)0x00000000)

/* MAC autoNegotiation enable or disable */
#define ETH_AutoNegotiation_Enable     ((uint32_t)0x00000001)
#define ETH_AutoNegotiation_Disable    ((uint32_t)0x00000000)

/* MAC watchdog enable or disable */
#define ETH_Watchdog_Enable       ((uint32_t)0x00000000)
#define ETH_Watchdog_Disable      ((uint32_t)0x00800000)

/* Bit description - MAC jabber enable or disable */
#define ETH_Jabber_Enable    ((uint32_t)0x00000000)
#define ETH_Jabber_Disable   ((uint32_t)0x00400000)

/* Value of minimum IFG between frames during transmission */
#define ETH_InterFrameGap_96Bit   ((uint32_t)0x00000000)  /* minimum IFG between frames during transmission is 96Bit */
#define ETH_InterFrameGap_88Bit   ((uint32_t)0x00020000)  /* minimum IFG between frames during transmission is 88Bit */
#define ETH_InterFrameGap_80Bit   ((uint32_t)0x00040000)  /* minimum IFG between frames during transmission is 80Bit */
#define ETH_InterFrameGap_72Bit   ((uint32_t)0x00060000)  /* minimum IFG between frames during transmission is 72Bit */
#define ETH_InterFrameGap_64Bit   ((uint32_t)0x00080000)  /* minimum IFG between frames during transmission is 64Bit */
#define ETH_InterFrameGap_56Bit   ((uint32_t)0x000A0000)  /* minimum IFG between frames during transmission is 56Bit */
#define ETH_InterFrameGap_48Bit   ((uint32_t)0x000C0000)  /* minimum IFG between frames during transmission is 48Bit */
#define ETH_InterFrameGap_40Bit   ((uint32_t)0x000E0000)  /* minimum IFG between frames during transmission is 40Bit */

/* MAC carrier sense enable or disable */
#define ETH_CarrierSense_Enable   ((uint32_t)0x00000000)
#define ETH_CarrierSense_Disable  ((uint32_t)0x00010000)

/* MAC speed */
#define ETH_Speed_10M        ((uint32_t)0x00000000)
#define ETH_Speed_100M       ((uint32_t)0x00004000)
#define ETH_Speed_1000M      ((uint32_t)0x00008000)

/* MAC receive own enable or disable */
#define ETH_ReceiveOwn_Enable     ((uint32_t)0x00000000)
#define ETH_ReceiveOwn_Disable    ((uint32_t)0x00002000)

/* MAC Loopback mode enable or disable */
#define ETH_LoopbackMode_Enable        ((uint32_t)0x00001000)
#define ETH_LoopbackMode_Disable       ((uint32_t)0x00000000)

/* MAC fullDuplex or halfDuplex */
#define ETH_Mode_FullDuplex       ((uint32_t)0x00000800)
#define ETH_Mode_HalfDuplex       ((uint32_t)0x00000000)

/* MAC offload checksum enable or disable */
#define ETH_ChecksumOffload_Enable     ((uint32_t)0x00000400)
#define ETH_ChecksumOffload_Disable    ((uint32_t)0x00000000)

/* MAC transmission retry enable or disable */
#define ETH_RetryTransmission_Enable   ((uint32_t)0x00000000)
#define ETH_RetryTransmission_Disable  ((uint32_t)0x00000200)

/* MAC automatic pad CRC strip enable or disable */
#define ETH_AutomaticPadCRCStrip_Enable     ((uint32_t)0x00000080)
#define ETH_AutomaticPadCRCStrip_Disable    ((uint32_t)0x00000000)

/* MAC backoff limitation */
#define ETH_BackOffLimit_10  ((uint32_t)0x00000000)
#define ETH_BackOffLimit_8   ((uint32_t)0x00000020)
#define ETH_BackOffLimit_4   ((uint32_t)0x00000040)
#define ETH_BackOffLimit_1   ((uint32_t)0x00000060)

/* MAC deferral check enable or disable */
#define ETH_DeferralCheck_Enable       ((uint32_t)0x00000010)
#define ETH_DeferralCheck_Disable      ((uint32_t)0x00000000)

/* Bit description  : MAC receive all frame enable or disable */
#define ETH_ReceiveAll_Enable     ((uint32_t)0x80000000)
#define ETH_ReceiveAll_Disable    ((uint32_t)0x00000000)

/* MAC backoff limitation */
#define ETH_SourceAddrFilter_Normal_Enable       ((uint32_t)0x00000200)
#define ETH_SourceAddrFilter_Inverse_Enable      ((uint32_t)0x00000300)
#define ETH_SourceAddrFilter_Disable             ((uint32_t)0x00000000)

/* MAC Pass control frames */
#define ETH_PassControlFrames_BlockAll                ((uint32_t)0x00000040)  /* MAC filters all control frames from reaching the application */
#define ETH_PassControlFrames_ForwardAll              ((uint32_t)0x00000080)  /* MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_PassControlFrames_ForwardPassedAddrFilter ((uint32_t)0x000000C0)  /* MAC forwards control frames that pass the Address Filter. */

/* MAC broadcast frames reception */
#define ETH_BroadcastFramesReception_Enable      ((uint32_t)0x00000000)
#define ETH_BroadcastFramesReception_Disable     ((uint32_t)0x00000020)

/* MAC destination address filter */
#define ETH_DestinationAddrFilter_Normal    ((uint32_t)0x00000000)
#define ETH_DestinationAddrFilter_Inverse   ((uint32_t)0x00000008)

/* MAC Promiscuous mode enable or disable */
#define ETH_PromiscuousMode_Enable     ((uint32_t)0x00000001)
#define ETH_PromiscuousMode_Disable    ((uint32_t)0x00000000)

/* MAC multicast frames filter */
#define ETH_MulticastFramesFilter_PerfectHashTable    ((uint32_t)0x00000404)
#define ETH_MulticastFramesFilter_HashTable           ((uint32_t)0x00000004)
#define ETH_MulticastFramesFilter_Perfect             ((uint32_t)0x00000000)
#define ETH_MulticastFramesFilter_None                ((uint32_t)0x00000010)

/* MAC unicast frames filter */
#define ETH_UnicastFramesFilter_PerfectHashTable ((uint32_t)0x00000402)
#define ETH_UnicastFramesFilter_HashTable        ((uint32_t)0x00000002)
#define ETH_UnicastFramesFilter_Perfect          ((uint32_t)0x00000000)

/* Bit description  : MAC zero quanta pause */
#define ETH_ZeroQuantaPause_Enable     ((uint32_t)0x00000000)
#define ETH_ZeroQuantaPause_Disable    ((uint32_t)0x00000080)

/* Field description  : MAC pause low threshold */
#define ETH_PauseLowThreshold_Minus4        ((uint32_t)0x00000000)  /* Pause time minus 4 slot times */
#define ETH_PauseLowThreshold_Minus28       ((uint32_t)0x00000010)  /* Pause time minus 28 slot times */
#define ETH_PauseLowThreshold_Minus144      ((uint32_t)0x00000020)  /* Pause time minus 144 slot times */
#define ETH_PauseLowThreshold_Minus256      ((uint32_t)0x00000030)  /* Pause time minus 256 slot times */

/* MAC unicast pause frame detect enable or disable*/
#define ETH_UnicastPauseFrameDetect_Enable  ((uint32_t)0x00000008)
#define ETH_UnicastPauseFrameDetect_Disable ((uint32_t)0x00000000)

/* MAC receive flow control frame enable or disable */
#define ETH_ReceiveFlowControl_Enable       ((uint32_t)0x00000004)
#define ETH_ReceiveFlowControl_Disable      ((uint32_t)0x00000000)

/* MAC transmit flow control enable or disable */
#define ETH_TransmitFlowControl_Enable      ((uint32_t)0x00000002)
#define ETH_TransmitFlowControl_Disable     ((uint32_t)0x00000000)

/* MAC VLAN tag comparison */
#define ETH_VLANTagComparison_12Bit    ((uint32_t)0x00010000)
#define ETH_VLANTagComparison_16Bit    ((uint32_t)0x00000000)

/* MAC flag */
#define ETH_MAC_FLAG_TST     ((uint32_t)0x00000200)  /* Time stamp trigger flag (on MAC) */
#define ETH_MAC_FLAG_MMCT    ((uint32_t)0x00000040)  /* MMC transmit flag  */
#define ETH_MAC_FLAG_MMCR    ((uint32_t)0x00000020)  /* MMC receive flag */
#define ETH_MAC_FLAG_MMC     ((uint32_t)0x00000010)  /* MMC flag (on MAC) */
#define ETH_MAC_FLAG_PMT     ((uint32_t)0x00000008)  /* PMT flag (on MAC) */

/* MAC interrupt */
#define ETH_MAC_IT_TST       ((uint32_t)0x00000200)  /* Time stamp trigger interrupt (on MAC) */
#define ETH_MAC_IT_MMCT      ((uint32_t)0x00000040)  /* MMC transmit interrupt */
#define ETH_MAC_IT_MMCR      ((uint32_t)0x00000020)  /* MMC receive interrupt */
#define ETH_MAC_IT_MMC       ((uint32_t)0x00000010)  /* MMC interrupt (on MAC) */
#define ETH_MAC_IT_PMT       ((uint32_t)0x00000008)  /* PMT interrupt (on MAC) */

/* MAC address */
#define ETH_MAC_Address0     ((uint32_t)0x00000000)
#define ETH_MAC_Address1     ((uint32_t)0x00000008)
#define ETH_MAC_Address2     ((uint32_t)0x00000010)
#define ETH_MAC_Address3     ((uint32_t)0x00000018)

/* MAC address filter select */
#define ETH_MAC_AddressFilter_SA       ((uint32_t)0x00000000)
#define ETH_MAC_AddressFilter_DA       ((uint32_t)0x00000008)

/* MAC address mask */
#define ETH_MAC_AddressMask_Byte6      ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
#define ETH_MAC_AddressMask_Byte5      ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
#define ETH_MAC_AddressMask_Byte4      ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
#define ETH_MAC_AddressMask_Byte3      ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
#define ETH_MAC_AddressMask_Byte2      ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
#define ETH_MAC_AddressMask_Byte1      ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [70] */


/******************************************************************************/
/*                                                                            */
/*                          MAC Descriptor Register                                                                                                                                     */
/*                                                                            */
/******************************************************************************/

/* DMA descriptor segment */
#define ETH_DMATxDesc_LastSegment      ((uint32_t)0x40000000)  /* Last Segment */
#define ETH_DMATxDesc_FirstSegment     ((uint32_t)0x20000000)  /* First Segment */

/* DMA descriptor checksum setting */
#define ETH_DMATxDesc_ChecksumByPass             ((uint32_t)0x00000000)   /* Checksum engine bypass */
#define ETH_DMATxDesc_ChecksumIPV4Header         ((uint32_t)0x00400000)   /* IPv4 header checksum insertion  */
#define ETH_DMATxDesc_ChecksumTCPUDPICMPSegment  ((uint32_t)0x00800000)   /* TCP/UDP/ICMP checksum insertion. Pseudo header checksum is assumed to be present */
#define ETH_DMATxDesc_ChecksumTCPUDPICMPFull     ((uint32_t)0x00C00000)   /* TCP/UDP/ICMP checksum fully in hardware including pseudo header */

/* DMA RX & TX buffer */
#define ETH_DMARxDesc_Buffer1     ((uint32_t)0x00000000)  /* DMA Rx Desc Buffer1 */
#define ETH_DMARxDesc_Buffer2     ((uint32_t)0x00000001)  /* DMA Rx Desc Buffer2 */


/******************************************************************************/
/*                                                                            */
/*                          ETH DMA Register                                                                                                                                    */
/*                                                                            */
/******************************************************************************/

/* DMA drop TCPIP checksum error frame enable or disable */
#define ETH_DropTCPIPChecksumErrorFrame_Enable   ((uint32_t)0x00000000)
#define ETH_DropTCPIPChecksumErrorFrame_Disable  ((uint32_t)0x04000000)

/* DMA receive store forward enable or disable */
#define ETH_ReceiveStoreForward_Enable      ((uint32_t)0x02000000)
#define ETH_ReceiveStoreForward_Disable     ((uint32_t)0x00000000)

/* DMA flush received frame enable or disable */
#define ETH_FlushReceivedFrame_Enable       ((uint32_t)0x00000000)
#define ETH_FlushReceivedFrame_Disable      ((uint32_t)0x01000000)

/* DMA transmit store forward enable or disable */
#define ETH_TransmitStoreForward_Enable     ((uint32_t)0x00200000)
#define ETH_TransmitStoreForward_Disable    ((uint32_t)0x00000000)

/* DMA transmit threshold control */
#define ETH_TransmitThresholdControl_64Bytes     ((uint32_t)0x00000000)  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
#define ETH_TransmitThresholdControl_128Bytes    ((uint32_t)0x00004000)  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
#define ETH_TransmitThresholdControl_192Bytes    ((uint32_t)0x00008000)  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
#define ETH_TransmitThresholdControl_256Bytes    ((uint32_t)0x0000C000)  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
#define ETH_TransmitThresholdControl_40Bytes     ((uint32_t)0x00010000)  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
#define ETH_TransmitThresholdControl_32Bytes     ((uint32_t)0x00014000)  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
#define ETH_TransmitThresholdControl_24Bytes     ((uint32_t)0x00018000)  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
#define ETH_TransmitThresholdControl_16Bytes     ((uint32_t)0x0001C000)  /* threshold level of the MTL Transmit FIFO is 16 Bytes */

/* DMA forward error frames */
#define ETH_ForwardErrorFrames_Enable       ((uint32_t)0x00000080)
#define ETH_ForwardErrorFrames_Disable      ((uint32_t)0x00000000)

/* DMA forward undersized good frames enable or disable */
#define ETH_ForwardUndersizedGoodFrames_Enable   ((uint32_t)0x00000040)
#define ETH_ForwardUndersizedGoodFrames_Disable  ((uint32_t)0x00000000)     

/* DMA receive threshold control */
#define ETH_ReceiveThresholdControl_64Bytes      ((uint32_t)0x00000000)  /* threshold level of the MTL Receive FIFO is 64 Bytes */
#define ETH_ReceiveThresholdControl_32Bytes      ((uint32_t)0x00000008)  /* threshold level of the MTL Receive FIFO is 32 Bytes */
#define ETH_ReceiveThresholdControl_96Bytes      ((uint32_t)0x00000010)  /* threshold level of the MTL Receive FIFO is 96 Bytes */
#define ETH_ReceiveThresholdControl_128Bytes     ((uint32_t)0x00000018)  /* threshold level of the MTL Receive FIFO is 128 Bytes */

/* DMA second frame operate enable or disable */
#define ETH_SecondFrameOperate_Enable       ((uint32_t)0x00000004)
#define ETH_SecondFrameOperate_Disable      ((uint32_t)0x00000000)  

/* Address aligned beats enable or disable */
#define ETH_AddressAlignedBeats_Enable      ((uint32_t)0x02000000)
#define ETH_AddressAlignedBeats_Disable     ((uint32_t)0x00000000) 

/* DMA Fixed burst enable or disable */
#define ETH_FixedBurst_Enable     ((uint32_t)0x00010000)
#define ETH_FixedBurst_Disable    ((uint32_t)0x00000000) 


/* RX DMA burst length */
#define ETH_RxDMABurstLength_1Beat          ((uint32_t)0x00020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
#define ETH_RxDMABurstLength_2Beat          ((uint32_t)0x00040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
#define ETH_RxDMABurstLength_4Beat          ((uint32_t)0x00080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RxDMABurstLength_8Beat          ((uint32_t)0x00100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RxDMABurstLength_16Beat         ((uint32_t)0x00200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RxDMABurstLength_32Beat         ((uint32_t)0x00400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_RxDMABurstLength_4xPBL_4Beat    ((uint32_t)0x01020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RxDMABurstLength_4xPBL_8Beat    ((uint32_t)0x01040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RxDMABurstLength_4xPBL_16Beat   ((uint32_t)0x01080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RxDMABurstLength_4xPBL_32Beat   ((uint32_t)0x01100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_RxDMABurstLength_4xPBL_64Beat   ((uint32_t)0x01200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
#define ETH_RxDMABurstLength_4xPBL_128Beat  ((uint32_t)0x01400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */

 
/* TX DMA burst length */
#define ETH_TxDMABurstLength_1Beat          ((uint32_t)0x00000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
#define ETH_TxDMABurstLength_2Beat          ((uint32_t)0x00000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
#define ETH_TxDMABurstLength_4Beat          ((uint32_t)0x00000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TxDMABurstLength_8Beat          ((uint32_t)0x00000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TxDMABurstLength_16Beat         ((uint32_t)0x00001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TxDMABurstLength_32Beat         ((uint32_t)0x00002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_TxDMABurstLength_4xPBL_4Beat    ((uint32_t)0x01000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TxDMABurstLength_4xPBL_8Beat    ((uint32_t)0x01000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TxDMABurstLength_4xPBL_16Beat   ((uint32_t)0x01000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TxDMABurstLength_4xPBL_32Beat   ((uint32_t)0x01000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_TxDMABurstLength_4xPBL_64Beat   ((uint32_t)0x01001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
#define ETH_TxDMABurstLength_4xPBL_128Beat  ((uint32_t)0x01002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */

/* DMA arbitration_round robin */
#define ETH_DMAArbitration_RoundRobin_RxTx_1_1   ((uint32_t)0x00000000)
#define ETH_DMAArbitration_RoundRobin_RxTx_2_1   ((uint32_t)0x00004000)
#define ETH_DMAArbitration_RoundRobin_RxTx_3_1   ((uint32_t)0x00008000)
#define ETH_DMAArbitration_RoundRobin_RxTx_4_1   ((uint32_t)0x0000C000)
#define ETH_DMAArbitration_RxPriorTx             ((uint32_t)0x00000002)

/* DMA interrupt FALG */
#define ETH_DMA_FLAG_TST               ((uint32_t)0x20000000)  /* Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_FLAG_PMT               ((uint32_t)0x10000000)  /* PMT interrupt (on DMA) */
#define ETH_DMA_FLAG_MMC               ((uint32_t)0x08000000)  /* MMC interrupt (on DMA) */
#define ETH_DMA_FLAG_DataTransferError ((uint32_t)0x00800000)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMA_FLAG_ReadWriteError    ((uint32_t)0x01000000)  /* Error bits 0-write trnsf, 1-read transfr */
#define ETH_DMA_FLAG_AccessError       ((uint32_t)0x02000000)  /* Error bits 0-data buffer, 1-desc. access */
#define ETH_DMA_FLAG_NIS               ((uint32_t)0x00010000)  /* Normal interrupt summary flag */
#define ETH_DMA_FLAG_AIS               ((uint32_t)0x00008000)  /* Abnormal interrupt summary flag */
#define ETH_DMA_FLAG_ER                ((uint32_t)0x00004000)  /* Early receive flag */
#define ETH_DMA_FLAG_FBE               ((uint32_t)0x00002000)  /* Fatal bus error flag */
#define ETH_DMA_FLAG_ET                ((uint32_t)0x00000400)  /* Early transmit flag */
#define ETH_DMA_FLAG_RWT               ((uint32_t)0x00000200)  /* Receive watchdog timeout flag */
#define ETH_DMA_FLAG_RPS               ((uint32_t)0x00000100)  /* Receive process stopped flag */
#define ETH_DMA_FLAG_RBU               ((uint32_t)0x00000080)  /* Receive buffer unavailable flag */
#define ETH_DMA_FLAG_R                 ((uint32_t)0x00000040)  /* Receive flag */
#define ETH_DMA_FLAG_TU                ((uint32_t)0x00000020)  /* Underflow flag */
#define ETH_DMA_FLAG_RO                ((uint32_t)0x00000010)  /* Overflow flag */
#define ETH_DMA_FLAG_TJT               ((uint32_t)0x00000008)  /* Transmit jabber timeout flag */
#define ETH_DMA_FLAG_TBU               ((uint32_t)0x00000004)  /* Transmit buffer unavailable flag */
#define ETH_DMA_FLAG_TPS               ((uint32_t)0x00000002)  /* Transmit process stopped flag */
#define ETH_DMA_FLAG_T                 ((uint32_t)0x00000001)  /* Transmit flag */

/* DMA interrupt */
#define ETH_DMA_IT_PHYLINK   ((uint32_t)0x80000000)  /* Internal PHY link status change interrupt */
#define ETH_DMA_IT_TST       ((uint32_t)0x20000000)  /* Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_IT_PMT       ((uint32_t)0x10000000)  /* PMT interrupt (on DMA) */
#define ETH_DMA_IT_MMC       ((uint32_t)0x08000000)  /* MMC interrupt (on DMA) */
#define ETH_DMA_IT_NIS       ((uint32_t)0x00010000)  /* Normal interrupt summary */
#define ETH_DMA_IT_AIS       ((uint32_t)0x00008000)  /* Abnormal interrupt summary */
#define ETH_DMA_IT_ER        ((uint32_t)0x00004000)  /* Early receive interrupt */
#define ETH_DMA_IT_FBE       ((uint32_t)0x00002000)  /* Fatal bus error interrupt */
#define ETH_DMA_IT_ET        ((uint32_t)0x00000400)  /* Early transmit interrupt */
#define ETH_DMA_IT_RWT       ((uint32_t)0x00000200)  /* Receive watchdog timeout interrupt */
#define ETH_DMA_IT_RPS       ((uint32_t)0x00000100)  /* Receive process stopped interrupt */
#define ETH_DMA_IT_RBU       ((uint32_t)0x00000080)  /* Receive buffer unavailable interrupt */
#define ETH_DMA_IT_R         ((uint32_t)0x00000040)  /* Receive interrupt */
#define ETH_DMA_IT_TU        ((uint32_t)0x00000020)  /* Underflow interrupt */
#define ETH_DMA_IT_RO        ((uint32_t)0x00000010)  /* Overflow interrupt */
#define ETH_DMA_IT_TJT       ((uint32_t)0x00000008)  /* Transmit jabber timeout interrupt */
#define ETH_DMA_IT_TBU       ((uint32_t)0x00000004)  /* Transmit buffer unavailable interrupt */
#define ETH_DMA_IT_TPS       ((uint32_t)0x00000002)  /* Transmit process stopped interrupt */
#define ETH_DMA_IT_T         ((uint32_t)0x00000001)  /* Transmit interrupt */

/* DMA transmit process */
#define ETH_DMA_TransmitProcess_Stopped     ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Tx Command issued */
#define ETH_DMA_TransmitProcess_Fetching    ((uint32_t)0x00100000)  /* Running - fetching the Tx descriptor */
#define ETH_DMA_TransmitProcess_Waiting     ((uint32_t)0x00200000)  /* Running - waiting for status */
#define ETH_DMA_TransmitProcess_Reading     ((uint32_t)0x00300000)  /* Running - reading the data from host memory */
#define ETH_DMA_TransmitProcess_Suspended   ((uint32_t)0x00600000)  /* Suspended - Tx Desciptor unavailabe */
#define ETH_DMA_TransmitProcess_Closing     ((uint32_t)0x00700000)  /* Running - closing Rx descriptor */

/* DMA receive Process */
#define ETH_DMA_ReceiveProcess_Stopped      ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Rx Command issued */
#define ETH_DMA_ReceiveProcess_Fetching     ((uint32_t)0x00020000)  /* Running - fetching the Rx descriptor */
#define ETH_DMA_ReceiveProcess_Waiting      ((uint32_t)0x00060000)  /* Running - waiting for packet */
#define ETH_DMA_ReceiveProcess_Suspended    ((uint32_t)0x00080000)  /* Suspended - Rx Desciptor unavailable */
#define ETH_DMA_ReceiveProcess_Closing      ((uint32_t)0x000A0000)  /* Running - closing descriptor */
#define ETH_DMA_ReceiveProcess_Queuing      ((uint32_t)0x000E0000)  /* Running - queuing the recieve frame into host memory */

/* DMA overflow */
#define ETH_DMA_Overflow_RxFIFOCounter      ((uint32_t)0x10000000)  /* Overflow bit for FIFO overflow counter */
#define ETH_DMA_Overflow_MissedFrameCounter ((uint32_t)0x00010000)  /* Overflow bit for missed frame counter */


/*********************************************************************************
*                            Ethernet PMT defines
**********************************************************************************/

/* PMT flag */
#define ETH_PMT_FLAG_WUFFRPR      ((uint32_t)0x80000000)  /* Wake-Up Frame Filter Register Poniter Reset */
#define ETH_PMT_FLAG_WUFR         ((uint32_t)0x00000040)  /* Wake-Up Frame Received */
#define ETH_PMT_FLAG_MPR          ((uint32_t)0x00000020)  /* Magic Packet Received */

/*********************************************************************************
*                            Ethernet MMC defines
**********************************************************************************/

/* MMC TX interrupt flag */
#define ETH_MMC_IT_TGF       ((uint32_t)0x00200000)  /* When Tx good frame counter reaches half the maximum value */
#define ETH_MMC_IT_TGFMSC    ((uint32_t)0x00008000)  /* When Tx good multi col counter reaches half the maximum value */
#define ETH_MMC_IT_TGFSC     ((uint32_t)0x00004000)  /* When Tx good single col counter reaches half the maximum value */

/* MMC RX interrupt flag */
#define ETH_MMC_IT_RGUF      ((uint32_t)0x10020000)  /* When Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMC_IT_RFAE      ((uint32_t)0x10000040)  /* When Rx alignment error counter reaches half the maximum value */
#define ETH_MMC_IT_RFCE      ((uint32_t)0x10000020)  /* When Rx crc error counter reaches half the maximum value */


/* MMC description */
#define ETH_MMCCR            ((uint32_t)0x00000100)  /* MMC CR register */
#define ETH_MMCRIR           ((uint32_t)0x00000104)  /* MMC RIR register */
#define ETH_MMCTIR           ((uint32_t)0x00000108)  /* MMC TIR register */
#define ETH_MMCRIMR          ((uint32_t)0x0000010C)  /* MMC RIMR register */
#define ETH_MMCTIMR          ((uint32_t)0x00000110)  /* MMC TIMR register */
#define ETH_MMCTGFSCCR       ((uint32_t)0x0000014C)  /* MMC TGFSCCR register */
#define ETH_MMCTGFMSCCR      ((uint32_t)0x00000150)  /* MMC TGFMSCCR register */
#define ETH_MMCTGFCR         ((uint32_t)0x00000168)  /* MMC TGFCR register */
#define ETH_MMCRFCECR        ((uint32_t)0x00000194)  /* MMC RFCECR register */
#define ETH_MMCRFAECR        ((uint32_t)0x00000198)  /* MMC RFAECR register */
#define ETH_MMCRGUFCR        ((uint32_t)0x000001C4)  /* MMC RGUFCR register */


/*********************************************************************************
*                            Ethernet PTP defines
**********************************************************************************/

/* PTP fine update method or coarse Update method */
#define ETH_PTP_FineUpdate        ((uint32_t)0x00000001)  /* Fine Update method */
#define ETH_PTP_CoarseUpdate      ((uint32_t)0x00000000)  /* Coarse Update method */


/* PTP time stamp control */
#define ETH_PTP_FLAG_TSARU        ((uint32_t)0x00000020)  /* Addend Register Update */
#define ETH_PTP_FLAG_TSITE        ((uint32_t)0x00000010)  /* Time Stamp Interrupt Trigger */
#define ETH_PTP_FLAG_TSSTU        ((uint32_t)0x00000008)  /* Time Stamp Update */
#define ETH_PTP_FLAG_TSSTI        ((uint32_t)0x00000004)  /* Time Stamp Initialize */

/* PTP positive/negative time value */
#define ETH_PTP_PositiveTime      ((uint32_t)0x00000000)  /* Positive time value */
#define ETH_PTP_NegativeTime      ((uint32_t)0x80000000)  /* Negative time value */


/******************************************************************************/
/*                                                                            */
/*                                PTP Register                                                                                                                           */
/*                                                                            */
/******************************************************************************/
#define ETH_PTPTSCR     ((uint32_t)0x00000700)  /* PTP TSCR register */
#define ETH_PTPSSIR     ((uint32_t)0x00000704)  /* PTP SSIR register */
#define ETH_PTPTSHR     ((uint32_t)0x00000708)  /* PTP TSHR register */
#define ETH_PTPTSLR     ((uint32_t)0x0000070C)  /* PTP TSLR register */
#define ETH_PTPTSHUR    ((uint32_t)0x00000710)  /* PTP TSHUR register */
#define ETH_PTPTSLUR    ((uint32_t)0x00000714)  /* PTP TSLUR register */
#define ETH_PTPTSAR     ((uint32_t)0x00000718)  /* PTP TSAR register */
#define ETH_PTPTTHR     ((uint32_t)0x0000071C)  /* PTP TTHR register */
#define ETH_PTPTTLR     ((uint32_t)0x00000720)  /* PTP TTLR register */

#define ETH_DMASR_TSTS       ((unsigned int)0x20000000)  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       ((unsigned int)0x10000000)  /* PMT status */
#define ETH_DMASR_MMCS       ((unsigned int)0x08000000)  /* MMC status */
#define ETH_DMASR_EBS        ((unsigned int)0x03800000)  /* Error bits status */
  #define ETH_DMASR_EBS_DescAccess      ((unsigned int)0x02000000)  /* Error bits 0-data buffer, 1-desc. access */
  #define ETH_DMASR_EBS_ReadTransf      ((unsigned int)0x01000000)  /* Error bits 0-write trnsf, 1-read transfr */
  #define ETH_DMASR_EBS_DataTransfTx    ((unsigned int)0x00800000)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((unsigned int)0x00700000)  /* Transmit process state */
  #define ETH_DMASR_TPS_Stopped         ((unsigned int)0x00000000)  /* Stopped - Reset or Stop Tx Command issued  */
  #define ETH_DMASR_TPS_Fetching        ((unsigned int)0x00100000)  /* Running - fetching the Tx descriptor */
  #define ETH_DMASR_TPS_Waiting         ((unsigned int)0x00200000)  /* Running - waiting for status */
  #define ETH_DMASR_TPS_Reading         ((unsigned int)0x00300000)  /* Running - reading the data from host memory */
  #define ETH_DMASR_TPS_Suspended       ((unsigned int)0x00600000)  /* Suspended - Tx Descriptor unavailabe */
  #define ETH_DMASR_TPS_Closing         ((unsigned int)0x00700000)  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         ((unsigned int)0x000E0000)  /* Receive process state */
  #define ETH_DMASR_RPS_Stopped         ((unsigned int)0x00000000)  /* Stopped - Reset or Stop Rx Command issued */
  #define ETH_DMASR_RPS_Fetching        ((unsigned int)0x00020000)  /* Running - fetching the Rx descriptor */
  #define ETH_DMASR_RPS_Waiting         ((unsigned int)0x00060000)  /* Running - waiting for packet */
  #define ETH_DMASR_RPS_Suspended       ((unsigned int)0x00080000)  /* Suspended - Rx Descriptor unavailable */
  #define ETH_DMASR_RPS_Closing         ((unsigned int)0x000A0000)  /* Running - closing descriptor */
  #define ETH_DMASR_RPS_Queuing         ((unsigned int)0x000E0000)  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        ((unsigned int)0x00010000)  /* Normal interrupt summary */
#define ETH_DMASR_AIS        ((unsigned int)0x00008000)  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        ((unsigned int)0x00004000)  /* Early receive status */
#define ETH_DMASR_FBES       ((unsigned int)0x00002000)  /* Fatal bus error status */
#define ETH_DMASR_ETS        ((unsigned int)0x00000400)  /* Early transmit status */
#define ETH_DMASR_RWTS       ((unsigned int)0x00000200)  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       ((unsigned int)0x00000100)  /* Receive process stopped status */
#define ETH_DMASR_RBUS       ((unsigned int)0x00000080)  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         ((unsigned int)0x00000040)  /* Receive status */
#define ETH_DMASR_TUS        ((unsigned int)0x00000020)  /* Transmit underflow status */
#define ETH_DMASR_ROS        ((unsigned int)0x00000010)  /* Receive overflow status */
#define ETH_DMASR_TJTS       ((unsigned int)0x00000008)  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       ((unsigned int)0x00000004)  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       ((unsigned int)0x00000002)  /* Transmit process stopped status */
#define ETH_DMASR_TS         ((unsigned int)0x00000001)  /* Transmit status */


/******************************************************************************/
/*                                                                            */
/*                          ETH MAC Register                                                                                                                               */
/*                                                                            */
/******************************************************************************/
#define ETH_MACCR_WD      ((unsigned int)0x00800000)  /* Watchdog disable */
#define ETH_MACCR_JD      ((unsigned int)0x00400000)  /* Jabber disable */
#define ETH_MACCR_IFG     ((unsigned int)0x000E0000)  /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit     ((unsigned int)0x00000000)  /* Minimum IFG between frames during transmission is 96Bit */
   #define ETH_MACCR_IFG_88Bit     ((unsigned int)0x00020000)  /* Minimum IFG between frames during transmission is 88Bit */
   #define ETH_MACCR_IFG_80Bit     ((unsigned int)0x00040000)  /* Minimum IFG between frames during transmission is 80Bit */
   #define ETH_MACCR_IFG_72Bit     ((unsigned int)0x00060000)  /* Minimum IFG between frames during transmission is 72Bit */
   #define ETH_MACCR_IFG_64Bit     ((unsigned int)0x00080000)  /* Minimum IFG between frames during transmission is 64Bit */
   #define ETH_MACCR_IFG_56Bit     ((unsigned int)0x000A0000)  /* Minimum IFG between frames during transmission is 56Bit */
   #define ETH_MACCR_IFG_48Bit     ((unsigned int)0x000C0000)  /* Minimum IFG between frames during transmission is 48Bit */
   #define ETH_MACCR_IFG_40Bit     ((unsigned int)0x000E0000)  /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_CSD     ((unsigned int)0x00010000)  /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES     ((unsigned int)0x00004000)  /* Fast ethernet speed */
#define ETH_MACCR_ROD     ((unsigned int)0x00002000)  /* Receive own disable */
#define ETH_MACCR_LM      ((unsigned int)0x00001000)  /* loopback mode */
#define ETH_MACCR_DM      ((unsigned int)0x00000800)  /* Duplex mode */
#define ETH_MACCR_IPCO    ((unsigned int)0x00000400)  /* IP Checksum offload */
#define ETH_MACCR_RD      ((unsigned int)0x00000200)  /* Retry disable */
#define ETH_MACCR_APCS    ((unsigned int)0x00000080)  /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL      ((unsigned int)0x00000060)  /* Back-off limit: random integer number (r) of slot time delays before reschedulinga transmission attempt during retries after a collision: 0 =< r <2^k */
   #define ETH_MACCR_BL_10    ((unsigned int)0x00000000)  /* k = min (n, 10) */
   #define ETH_MACCR_BL_8     ((unsigned int)0x00000020)  /* k = min (n, 8) */
   #define ETH_MACCR_BL_4     ((unsigned int)0x00000040)  /* k = min (n, 4) */
   #define ETH_MACCR_BL_1     ((unsigned int)0x00000060)  /* k = min (n, 1) */
#define ETH_MACCR_DC      ((unsigned int)0x00000010)  /* Defferal check */
#define ETH_MACCR_TE      ((unsigned int)0x00000008)  /* Transmitter enable */
#define ETH_MACCR_RE      ((unsigned int)0x00000004)  /* Receiver enable */

#define ETH_MACFFR_RA     ((unsigned int)0x80000000)  /* Receive all */
#define ETH_MACFFR_HPF    ((unsigned int)0x00000400)  /* Hash or perfect filter */
#define ETH_MACFFR_SAF    ((unsigned int)0x00000200)  /* Source address filter enable */
#define ETH_MACFFR_SAIF   ((unsigned int)0x00000100)  /* SA inverse filtering */
#define ETH_MACFFR_PCF    ((unsigned int)0x000000C0)  /* Pass control frames: 3 cases */
   #define ETH_MACFFR_PCF_BlockAll                ((unsigned int)0x00000040)  /* MAC filters all control frames from reaching the application */
   #define ETH_MACFFR_PCF_ForwardAll              ((unsigned int)0x00000080)  /* MAC forwards all control frames to application even if they fail the Address Filter */
   #define ETH_MACFFR_PCF_ForwardPassedAddrFilter ((unsigned int)0x000000C0)  /* MAC forwards control frames that pass the Address Filter. */
#define ETH_MACFFR_BFD    ((unsigned int)0x00000020)  /* Broadcast frame disable */
#define ETH_MACFFR_PAM    ((unsigned int)0x00000010)  /* Pass all mutlicast */
#define ETH_MACFFR_DAIF   ((unsigned int)0x00000008)  /* DA Inverse filtering */
#define ETH_MACFFR_HM     ((unsigned int)0x00000004)  /* Hash multicast */
#define ETH_MACFFR_HU     ((unsigned int)0x00000002)  /* Hash unicast */
#define ETH_MACFFR_PM     ((unsigned int)0x00000001)  /* Promiscuous mode */

#define ETH_MACHTHR_HTH   ((unsigned int)0xFFFFFFFF)  /* Hash table high */
#define ETH_MACHTLR_HTL   ((unsigned int)0xFFFFFFFF)  /* Hash table low */

#define ETH_MACMIIAR_PA   ((unsigned int)0x0000F800)  /* Physical layer address */
#define ETH_MACMIIAR_MR   ((unsigned int)0x000007C0)  /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR   ((unsigned int)0x0000001C)  /* CR clock range: 6 cases */
   #define ETH_MACMIIAR_CR_Div42   ((unsigned int)0x00000000)  /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
   #define ETH_MACMIIAR_CR_Div16   ((unsigned int)0x00000008)  /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
   #define ETH_MACMIIAR_CR_Div26   ((unsigned int)0x0000000C)  /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
#define ETH_MACMIIAR_MW   ((unsigned int)0x00000002)  /* MII write */
#define ETH_MACMIIAR_MB   ((unsigned int)0x00000001)  /* MII busy */
#define ETH_MACMIIDR_MD   ((unsigned int)0x0000FFFF)  /* MII data: read/write data from/to PHY */
#define ETH_MACFCR_PT     ((unsigned int)0xFFFF0000)  /* Pause time */
#define ETH_MACFCR_ZQPD   ((unsigned int)0x00000080)  /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT    ((unsigned int)0x00000030)  /* Pause low threshold: 4 cases */
   #define ETH_MACFCR_PLT_Minus4   ((unsigned int)0x00000000)  /* Pause time minus 4 slot times */
   #define ETH_MACFCR_PLT_Minus28  ((unsigned int)0x00000010)  /* Pause time minus 28 slot times */
   #define ETH_MACFCR_PLT_Minus144 ((unsigned int)0x00000020)  /* Pause time minus 144 slot times */
   #define ETH_MACFCR_PLT_Minus256 ((unsigned int)0x00000030)  /* Pause time minus 256 slot times */
#define ETH_MACFCR_UPFD   ((unsigned int)0x00000008)  /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE   ((unsigned int)0x00000004)  /* Receive flow control enable */
#define ETH_MACFCR_TFCE   ((unsigned int)0x00000002)  /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA ((unsigned int)0x00000001)  /* Flow control busy/backpressure activate */

#define ETH_MACVLANTR_VLANTC ((unsigned int)0x00010000)  /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI ((unsigned int)0x0000FFFF)  /* VLAN tag identifier (for receive frames) */

#define ETH_MACRWUFFR_D   ((unsigned int)0xFFFFFFFF)  /* Wake-up frame filter register data */
/* Eight sequential Writes to this address (offset 0x28) will write all Wake-UpFrame Filter Registers.
Eight sequential Reads from this address (offset 0x28) will read all Wake-UpFrame Filter Registers. */

/*
Wake-UpFrame Filter Reg0 : Filter 0 Byte Mask
Wake-UpFrame Filter Reg1 : Filter 1 Byte Mask
Wake-UpFrame Filter Reg2 : Filter 2 Byte Mask
Wake-UpFrame Filter Reg3 : Filter 3 Byte Mask
Wake-UpFrame Filter Reg4 : RSVD - Filter3 Command - RSVD - Filter2 Command -
                           RSVD - Filter1 Command - RSVD - Filter0 Command
Wake-UpFrame Filter Re5 : Filter3 Offset - Filter2 Offset - Filter1 Offset - Filter0 Offset
Wake-UpFrame Filter Re6 : Filter1 CRC16 - Filter0 CRC16
Wake-UpFrame Filter Re7 : Filter3 CRC16 - Filter2 CRC16 */

#define ETH_MACPMTCSR_WFFRPR ((unsigned int)0x80000000)  /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU     ((unsigned int)0x00000200)  /* Global Unicast */
#define ETH_MACPMTCSR_WFR    ((unsigned int)0x00000040)  /* Wake-Up Frame Received */
#define ETH_MACPMTCSR_MPR    ((unsigned int)0x00000020)  /* Magic Packet Received */
#define ETH_MACPMTCSR_WFE    ((unsigned int)0x00000004)  /* Wake-Up Frame Enable */
#define ETH_MACPMTCSR_MPE    ((unsigned int)0x00000002)  /* Magic Packet Enable */
#define ETH_MACPMTCSR_PD     ((unsigned int)0x00000001)  /* Power Down */

#define ETH_MACSR_TSTS      ((unsigned int)0x00000200)  /* Time stamp trigger status */
#define ETH_MACSR_MMCTS     ((unsigned int)0x00000040)  /* MMC transmit status */
#define ETH_MACSR_MMMCRS    ((unsigned int)0x00000020)  /* MMC receive status */
#define ETH_MACSR_MMCS      ((unsigned int)0x00000010)  /* MMC status */
#define ETH_MACSR_PMTS      ((unsigned int)0x00000008)  /* PMT status */

#define ETH_MACIMR_TSTIM     ((unsigned int)0x00000200)  /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM     ((unsigned int)0x00000008)  /* PMT interrupt mask */

#define ETH_MACA0HR_MACA0H   ((unsigned int)0x0000FFFF)  /* MAC address0 high */

#define ETH_MACA0LR_MACA0L   ((unsigned int)0xFFFFFFFF)  /* MAC address0 low */

#define ETH_MACA1HR_AE       ((unsigned int)0x80000000)  /* Address enable */
#define ETH_MACA1HR_SA       ((unsigned int)0x40000000)  /* Source address */
#define ETH_MACA1HR_MBC      ((unsigned int)0x3F000000)  /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
   #define ETH_MACA1HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
   #define ETH_MACA1HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
   #define ETH_MACA1HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
   #define ETH_MACA1HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
   #define ETH_MACA1HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
   #define ETH_MACA1HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* Mask MAC Address low reg bits [7:0] */
#define ETH_MACA1HR_MACA1H   ((unsigned int)0x0000FFFF)  /* MAC address1 high */

#define ETH_MACA1LR_MACA1L   ((unsigned int)0xFFFFFFFF)  /* MAC address1 low */

#define ETH_MACA2HR_AE       ((unsigned int)0x80000000)  /* Address enable */
#define ETH_MACA2HR_SA       ((unsigned int)0x40000000)  /* Source address */
#define ETH_MACA2HR_MBC      ((unsigned int)0x3F000000)  /* Mask byte control */
   #define ETH_MACA2HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
   #define ETH_MACA2HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
   #define ETH_MACA2HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
   #define ETH_MACA2HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
   #define ETH_MACA2HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
   #define ETH_MACA2HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* Mask MAC Address low reg bits [70] */

#define ETH_MACA2HR_MACA2H   ((unsigned int)0x0000FFFF)  /* MAC address1 high */
#define ETH_MACA2LR_MACA2L   ((unsigned int)0xFFFFFFFF)  /* MAC address2 low */

#define ETH_MACA3HR_AE       ((unsigned int)0x80000000)  /* Address enable */
#define ETH_MACA3HR_SA       ((unsigned int)0x40000000)  /* Source address */
#define ETH_MACA3HR_MBC      ((unsigned int)0x3F000000)  /* Mask byte control */
   #define ETH_MACA3HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
   #define ETH_MACA3HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
   #define ETH_MACA3HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
   #define ETH_MACA3HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
   #define ETH_MACA3HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
   #define ETH_MACA3HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA3HR_MACA3H   ((unsigned int)0x0000FFFF)  /* MAC address3 high */
#define ETH_MACA3LR_MACA3L   ((unsigned int)0xFFFFFFFF)  /* MAC address3 low */

/******************************************************************************/
/*
/*                          ETH MMC Register
/*
/******************************************************************************/
#define ETH_MMCCR_MCFHP      ((unsigned int)0x00000020)  /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP        ((unsigned int)0x00000010)  /* MMC counter preset */
#define ETH_MMCCR_MCF        ((unsigned int)0x00000008)  /* MMC Counter Freeze */
#define ETH_MMCCR_ROR        ((unsigned int)0x00000004)  /* Reset on Read */
#define ETH_MMCCR_CSR        ((unsigned int)0x00000002)  /* Counter Stop Rollover */
#define ETH_MMCCR_CR         ((unsigned int)0x00000001)  /* Counters Reset */

#define ETH_MMCRIR_RGUFS     ((unsigned int)0x00020000)  /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES     ((unsigned int)0x00000040)  /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES     ((unsigned int)0x00000020)  /* Set when Rx crc error counter reaches half the maximum value */

#define ETH_MMCTIR_TGFS      ((unsigned int)0x00200000)  /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS   ((unsigned int)0x00008000)  /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS    ((unsigned int)0x00004000)  /* Set when Tx good single col counter reaches half the maximum value */

#define ETH_MMCRIMR_RGUFM    ((unsigned int)0x00020000)  /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM    ((unsigned int)0x00000040)  /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM    ((unsigned int)0x00000020)  /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

#define ETH_MMCTIMR_TGFM     ((unsigned int)0x00200000)  /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM  ((unsigned int)0x00008000)  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM   ((unsigned int)0x00004000)  /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

#define ETH_MMCTGFSCCR_TGFSCC     ((unsigned int)0xFFFFFFFF)  /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

#define ETH_MMCTGFMSCCR_TGFMSCC   ((unsigned int)0xFFFFFFFF)  /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

#define ETH_MMCTGFCR_TGFC    ((unsigned int)0xFFFFFFFF)  /* Number of good frames transmitted. */

#define ETH_MMCRFCECR_RFCEC  ((unsigned int)0xFFFFFFFF)  /* Number of frames received with CRC error. */

#define ETH_MMCRFAECR_RFAEC  ((unsigned int)0xFFFFFFFF)  /* Number of frames received with alignment (dribble) error */

#define ETH_MMCRGUFCR_RGUFC  ((unsigned int)0xFFFFFFFF)  /* Number of good unicast frames received. */


/******************************************************************************/
/*
/*                          ETH Precise Clock Protocol Register
/*
/******************************************************************************/
#define ETH_PTPTSCR_TSCNT       ((unsigned int)0x00030000)  /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME     ((unsigned int)0x00008000)  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME      ((unsigned int)0x00004000)  /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE   ((unsigned int)0x00002000)  /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE   ((unsigned int)0x00001000)  /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE  ((unsigned int)0x00000800)  /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E  ((unsigned int)0x00000400)  /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR       ((unsigned int)0x00000200)  /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE     ((unsigned int)0x00000100)  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU    ((unsigned int)0x00000020)  /* Addend register update */
#define ETH_PTPTSCR_TSITE    ((unsigned int)0x00000010)  /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU    ((unsigned int)0x00000008)  /* Time stamp update */
#define ETH_PTPTSCR_TSSTI    ((unsigned int)0x00000004)  /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU    ((unsigned int)0x00000002)  /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE      ((unsigned int)0x00000001)  /* Time stamp enable */

#define ETH_PTPSSIR_STSSI    ((unsigned int)0x000000FF)  /* System time Sub-second increment value */

#define ETH_PTPTSHR_STS      ((unsigned int)0xFFFFFFFF)  /* System Time second */

#define ETH_PTPTSLR_STPNS    ((unsigned int)0x80000000)  /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS     ((unsigned int)0x7FFFFFFF)  /* System Time sub-seconds */

#define ETH_PTPTSHUR_TSUS    ((unsigned int)0xFFFFFFFF)  /* Time stamp update seconds */

#define ETH_PTPTSLUR_TSUPNS  ((unsigned int)0x80000000)  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS   ((unsigned int)0x7FFFFFFF)  /* Time stamp update sub-seconds */

#define ETH_PTPTSAR_TSA      ((unsigned int)0xFFFFFFFF)  /* Time stamp addend */

#define ETH_PTPTTHR_TTSH     ((unsigned int)0xFFFFFFFF)  /* Target time stamp high */

#define ETH_PTPTTLR_TTSL     ((unsigned int)0xFFFFFFFF)  /* Target time stamp low */

#define ETH_PTPTSSR_TSTTR    ((unsigned int)0x00000020)  /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO     ((unsigned int)0x00000010)  /* Time stamp seconds overflow */

/******************************************************************************/
/*
/*                       ETH DMA Register
/*
/******************************************************************************/
#define ETH_DMABMR_AAB       ((unsigned int)0x02000000)  /* Address-Aligned beats */
#define ETH_DMABMR_FPM        ((unsigned int)0x01000000)  /* 4xPBL mode */
#define ETH_DMABMR_USP       ((unsigned int)0x00800000)  /* Use separate PBL */
#define ETH_DMABMR_RDP       ((unsigned int)0x007E0000)  /* RxDMA PBL */
   #define ETH_DMABMR_RDP_1Beat    ((unsigned int)0x00020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
   #define ETH_DMABMR_RDP_2Beat    ((unsigned int)0x00040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
   #define ETH_DMABMR_RDP_4Beat    ((unsigned int)0x00080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
   #define ETH_DMABMR_RDP_8Beat    ((unsigned int)0x00100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
   #define ETH_DMABMR_RDP_16Beat   ((unsigned int)0x00200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
   #define ETH_DMABMR_RDP_32Beat   ((unsigned int)0x00400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
   #define ETH_DMABMR_RDP_4xPBL_4Beat   ((unsigned int)0x01020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
   #define ETH_DMABMR_RDP_4xPBL_8Beat   ((unsigned int)0x01040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
   #define ETH_DMABMR_RDP_4xPBL_16Beat  ((unsigned int)0x01080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
   #define ETH_DMABMR_RDP_4xPBL_32Beat  ((unsigned int)0x01100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
   #define ETH_DMABMR_RDP_4xPBL_64Beat  ((unsigned int)0x01200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
   #define ETH_DMABMR_RDP_4xPBL_128Beat ((unsigned int)0x01400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */
#define ETH_DMABMR_FB        ((unsigned int)0x00010000)  /* Fixed Burst */
#define ETH_DMABMR_RTPR      ((unsigned int)0x0000C000)  /* Rx Tx priority ratio */
   #define ETH_DMABMR_RTPR_1_1     ((unsigned int)0x00000000)  /* Rx Tx priority ratio */
   #define ETH_DMABMR_RTPR_2_1     ((unsigned int)0x00004000)  /* Rx Tx priority ratio */
   #define ETH_DMABMR_RTPR_3_1     ((unsigned int)0x00008000)  /* Rx Tx priority ratio */
   #define ETH_DMABMR_RTPR_4_1     ((unsigned int)0x0000C000)  /* Rx Tx priority ratio */
#define ETH_DMABMR_PBL    ((unsigned int)0x00003F00)  /* Programmable burst length */
   #define ETH_DMABMR_PBL_1Beat    ((unsigned int)0x00000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
   #define ETH_DMABMR_PBL_2Beat    ((unsigned int)0x00000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
   #define ETH_DMABMR_PBL_4Beat    ((unsigned int)0x00000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
   #define ETH_DMABMR_PBL_8Beat    ((unsigned int)0x00000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
   #define ETH_DMABMR_PBL_16Beat   ((unsigned int)0x00001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
   #define ETH_DMABMR_PBL_32Beat   ((unsigned int)0x00002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
   #define ETH_DMABMR_PBL_4xPBL_4Beat   ((unsigned int)0x01000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
   #define ETH_DMABMR_PBL_4xPBL_8Beat   ((unsigned int)0x01000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
   #define ETH_DMABMR_PBL_4xPBL_16Beat  ((unsigned int)0x01000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
   #define ETH_DMABMR_PBL_4xPBL_32Beat  ((unsigned int)0x01000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
   #define ETH_DMABMR_PBL_4xPBL_64Beat  ((unsigned int)0x01001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
   #define ETH_DMABMR_PBL_4xPBL_128Beat ((unsigned int)0x01002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE       ((unsigned int)0x00000080)  /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL       ((unsigned int)0x0000007C)  /* Descriptor Skip Length */
#define ETH_DMABMR_DA        ((unsigned int)0x00000002)  /* DMA arbitration scheme */
#define ETH_DMABMR_SR        ((unsigned int)0x00000001)  /* Software reset */

#define ETH_DMATPDR_TPD      ((unsigned int)0xFFFFFFFF)  /* Transmit poll demand */

#define ETH_DMARPDR_RPD      ((unsigned int)0xFFFFFFFF)  /* Receive poll demand  */

#define ETH_DMARDLAR_SRL     ((unsigned int)0xFFFFFFFF)  /* Start of receive list */

#define ETH_DMATDLAR_STL     ((unsigned int)0xFFFFFFFF)  /* Start of transmit list */

#define ETH_DMASR_TSTS       ((unsigned int)0x20000000)  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       ((unsigned int)0x10000000)  /* PMT status */
#define ETH_DMASR_MMCS       ((unsigned int)0x08000000)  /* MMC status */
#define ETH_DMASR_EBS        ((unsigned int)0x03800000)  /* Error bits status */
   #define ETH_DMASR_EBS_DescAccess      ((unsigned int)0x02000000)  /* Error bits 0-data buffer, 1-desc. access */
   #define ETH_DMASR_EBS_ReadTransf      ((unsigned int)0x01000000)  /* Error bits 0-write trnsf, 1-read transfr */
   #define ETH_DMASR_EBS_DataTransfTx    ((unsigned int)0x00800000)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((unsigned int)0x00700000)  /* Transmit process state */
   #define ETH_DMASR_TPS_Stopped         ((unsigned int)0x00000000)  /* Stopped - Reset or Stop Tx Command issued  */
   #define ETH_DMASR_TPS_Fetching        ((unsigned int)0x00100000)  /* Running - fetching the Tx descriptor */
   #define ETH_DMASR_TPS_Waiting         ((unsigned int)0x00200000)  /* Running - waiting for status */
   #define ETH_DMASR_TPS_Reading         ((unsigned int)0x00300000)  /* Running - reading the data from host memory */
   #define ETH_DMASR_TPS_Suspended       ((unsigned int)0x00600000)  /* Suspended - Tx Descriptor unavailabe */
   #define ETH_DMASR_TPS_Closing         ((unsigned int)0x00700000)  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         ((unsigned int)0x000E0000)  /* Receive process state */
   #define ETH_DMASR_RPS_Stopped         ((unsigned int)0x00000000)  /* Stopped - Reset or Stop Rx Command issued */
   #define ETH_DMASR_RPS_Fetching        ((unsigned int)0x00020000)  /* Running - fetching the Rx descriptor */
   #define ETH_DMASR_RPS_Waiting         ((unsigned int)0x00060000)  /* Running - waiting for packet */
   #define ETH_DMASR_RPS_Suspended       ((unsigned int)0x00080000)  /* Suspended - Rx Descriptor unavailable */
   #define ETH_DMASR_RPS_Closing         ((unsigned int)0x000A0000)  /* Running - closing descriptor */
   #define ETH_DMASR_RPS_Queuing         ((unsigned int)0x000E0000)  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        ((unsigned int)0x00010000)  /* Normal interrupt summary */
#define ETH_DMASR_AIS        ((unsigned int)0x00008000)  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        ((unsigned int)0x00004000)  /* Early receive status */
#define ETH_DMASR_FBES       ((unsigned int)0x00002000)  /* Fatal bus error status */
#define ETH_DMASR_ETS        ((unsigned int)0x00000400)  /* Early transmit status */
#define ETH_DMASR_RWTS       ((unsigned int)0x00000200)  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       ((unsigned int)0x00000100)  /* Receive process stopped status */
#define ETH_DMASR_RBUS       ((unsigned int)0x00000080)  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         ((unsigned int)0x00000040)  /* Receive status */
#define ETH_DMASR_TUS        ((unsigned int)0x00000020)  /* Transmit underflow status */
#define ETH_DMASR_ROS        ((unsigned int)0x00000010)  /* Receive overflow status */
#define ETH_DMASR_TJTS       ((unsigned int)0x00000008)  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       ((unsigned int)0x00000004)  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       ((unsigned int)0x00000002)  /* Transmit process stopped status */
#define ETH_DMASR_TS         ((unsigned int)0x00000001)  /* Transmit status */

#define ETH_DMAOMR_DTCEFD    ((unsigned int)0x04000000)  /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF       ((unsigned int)0x02000000)  /* Receive store and forward */
#define ETH_DMAOMR_DFRF      ((unsigned int)0x01000000)  /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF       ((unsigned int)0x00200000)  /* Transmit store and forward */
#define ETH_DMAOMR_FTF       ((unsigned int)0x00100000)  /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC       ((unsigned int)0x0001C000)  /* Transmit threshold control */
   #define ETH_DMAOMR_TTC_64Bytes       ((unsigned int)0x00000000)  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
   #define ETH_DMAOMR_TTC_128Bytes      ((unsigned int)0x00004000)  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
   #define ETH_DMAOMR_TTC_192Bytes      ((unsigned int)0x00008000)  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
   #define ETH_DMAOMR_TTC_256Bytes      ((unsigned int)0x0000C000)  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
   #define ETH_DMAOMR_TTC_40Bytes       ((unsigned int)0x00010000)  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
   #define ETH_DMAOMR_TTC_32Bytes       ((unsigned int)0x00014000)  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
   #define ETH_DMAOMR_TTC_24Bytes       ((unsigned int)0x00018000)  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
   #define ETH_DMAOMR_TTC_16Bytes       ((unsigned int)0x0001C000)  /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST        ((unsigned int)0x00002000)  /* Start/stop transmission command */
#define ETH_DMAOMR_FEF       ((unsigned int)0x00000080)  /* Forward error frames */
#define ETH_DMAOMR_FUGF      ((unsigned int)0x00000040)  /* Forward undersized good frames */
#define ETH_DMAOMR_RTC       ((unsigned int)0x00000018)  /* receive threshold control */
   #define ETH_DMAOMR_RTC_64Bytes       ((unsigned int)0x00000000)  /* threshold level of the MTL Receive FIFO is 64 Bytes */
   #define ETH_DMAOMR_RTC_32Bytes       ((unsigned int)0x00000008)  /* threshold level of the MTL Receive FIFO is 32 Bytes */
   #define ETH_DMAOMR_RTC_96Bytes       ((unsigned int)0x00000010)  /* threshold level of the MTL Receive FIFO is 96 Bytes */
   #define ETH_DMAOMR_RTC_128Bytes      ((unsigned int)0x00000018)  /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF       ((unsigned int)0x00000004)  /* operate on second frame */
#define ETH_DMAOMR_SR        ((unsigned int)0x00000002)  /* Start/stop receive */

#define ETH_DMAIER_NISE      ((unsigned int)0x00010000)  /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE      ((unsigned int)0x00008000)  /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE      ((unsigned int)0x00004000)  /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE     ((unsigned int)0x00002000)  /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE      ((unsigned int)0x00000400)  /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE     ((unsigned int)0x00000200)  /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE     ((unsigned int)0x00000100)  /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE     ((unsigned int)0x00000080)  /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE       ((unsigned int)0x00000040)  /* Receive interrupt enable */
#define ETH_DMAIER_TUIE      ((unsigned int)0x00000020)  /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE      ((unsigned int)0x00000010)  /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE     ((unsigned int)0x00000008)  /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE     ((unsigned int)0x00000004)  /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE     ((unsigned int)0x00000002)  /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE       ((unsigned int)0x00000001)  /* Transmit interrupt enable */

#define ETH_DMAMFBOCR_OFOC   ((unsigned int)0x10000000)  /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA    ((unsigned int)0x0FFE0000)  /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC   ((unsigned int)0x00010000)  /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC    ((unsigned int)0x0000FFFF)  /* Number of frames missed by the controller */

#define ETH_DMACHTDR_HTDAP   ((unsigned int)0xFFFFFFFF)  /* Host transmit descriptor address pointer */
#define ETH_DMACHRDR_HRDAP   ((unsigned int)0xFFFFFFFF)  /* Host receive descriptor address pointer */
#define ETH_DMACHTBAR_HTBAP  ((unsigned int)0xFFFFFFFF)  /* Host transmit buffer address pointer */
#define ETH_DMACHRBAR_HRBAP  ((unsigned int)0xFFFFFFFF)  /* Host receive buffer address pointer */


#define ETH_MAC_ADDR_HBASE   (ETH_MAC_BASE + 0x40)   /* ETHERNET MAC address high offset */
#define ETH_MAC_ADDR_LBASE    (ETH_MAC_BASE + 0x44)  /* ETHERNET MAC address low offset */

/* ETHERNET MACMIIAR register Mask */
#define MACMIIAR_CR_MASK    ((uint32_t)0xFFFFFFE3)

/* ETHERNET MACCR register Mask */
#define MACCR_CLEAR_MASK    ((uint32_t)0xFF20810F)

/* ETHERNET MACFCR register Mask */
#define MACFCR_CLEAR_MASK   ((uint32_t)0x0000FF41)

/* ETHERNET DMAOMR register Mask */
#define DMAOMR_CLEAR_MASK   ((uint32_t)0xF8DE3F23)

/* ETHERNET Remote Wake-up frame register length */
#define ETH_WAKEUP_REGISTER_LENGTH      8

/* ETHERNET Missed frames counter Shift */
#define  ETH_DMA_RX_OVERFLOW_MISSEDFRAMES_COUNTERSHIFT     17

/* ETHERNET DMA Tx descriptors Collision Count Shift */
#define  ETH_DMATXDESC_COLLISION_COUNTSHIFT        3

/* ETHERNET DMA Tx descriptors Buffer2 Size Shift */
#define  ETH_DMATXDESC_BUFFER2_SIZESHIFT           16

/* ETHERNET DMA Rx descriptors Frame Length Shift */
#define  ETH_DMARXDESC_FRAME_LENGTHSHIFT           16

/* ETHERNET DMA Rx descriptors Buffer2 Size Shift */
#define  ETH_DMARXDESC_BUFFER2_SIZESHIFT           16

/* ETHERNET errors */
#define  ETH_ERROR              ((uint32_t)0)
#define  ETH_SUCCESS            ((uint32_t)1)


void ETH_DeInit(void);
void ETH_StructInit(ETH_InitTypeDef* ETH_InitStruct);
void ETH_SoftwareReset(void);
FlagStatus ETH_GetSoftwareResetStatus(void);
FlagStatus ETH_GetlinkStaus (void);
void  ETH_Start(void);
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength);
void delay_clk (uint32_t nCount);
void printf_dmasr (void);
void print_dmasr_tbus(void);
void print_dmasr_rps(void);
void print_dmasr_tps(void);
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);
uint32_t ETH_GetRxPktSize(void);
void ETH_DropRxPkt(void);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
uint32_t ETH_PHYLoopBackCmd(uint16_t PHYAddress, FunctionalState NewState);

void ETH_MACTransmissionCmd(FunctionalState NewState);
void ETH_MACReceptionCmd(FunctionalState NewState);
FlagStatus ETH_GetFlowControlBusyStatus(void);
void ETH_InitiatePauseControlFrame(void);  
void ETH_BackPressureActivationCmd(FunctionalState NewState); 
FlagStatus ETH_GetMACFlagStatus(uint32_t ETH_MAC_FLAG);  
ITStatus ETH_GetMACITStatus(uint32_t ETH_MAC_IT);
void ETH_MACITConfig(uint32_t ETH_MAC_IT, FunctionalState NewState);
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);
void ETH_GetMACAddress(uint32_t MacAddr, uint8_t *Addr);
void ETH_MACAddressPerfectFilterCmd(uint32_t MacAddr, FunctionalState NewState);
void ETH_MACAddressFilterConfig(uint32_t MacAddr, uint32_t Filter);
void ETH_MACAddressMaskBytesFilterConfig(uint32_t MacAddr, uint32_t MaskByte);

void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount);
void ETH_DMATxDescRingInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff1, uint8_t *TxBuff2, uint32_t TxBuffCount);
FlagStatus ETH_GetDMATxDescFlagStatus(ETH_DMADESCTypeDef *DMATxDesc, uint32_t ETH_DMATxDescFlag);
uint32_t ETH_GetDMATxDescCollisionCount(ETH_DMADESCTypeDef *DMATxDesc);
void ETH_SetDMATxDescOwnBit(ETH_DMADESCTypeDef *DMATxDesc);
void ETH_DMATxDescTransmitITConfig(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescFrameSegmentConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_FrameSegment);
void ETH_DMATxDescChecksumInsertionConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_Checksum);
void ETH_DMATxDescCRCCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescShortFramePaddingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescTimeStampCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState);
void ETH_DMATxDescBufferSizeConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t BufferSize1, uint32_t BufferSize2);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_DMARxDescRingInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff1, uint8_t *RxBuff2, uint32_t RxBuffCount);
FlagStatus ETH_GetDMARxDescFlagStatus(ETH_DMADESCTypeDef *DMARxDesc, uint32_t ETH_DMARxDescFlag);
void ETH_SetDMARxDescOwnBit(ETH_DMADESCTypeDef *DMARxDesc);
uint32_t ETH_GetDMARxDescFrameLength(ETH_DMADESCTypeDef *DMARxDesc);
void ETH_DMARxDescReceiveITConfig(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState);
void ETH_DMARxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState);
void ETH_DMARxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState);
uint32_t ETH_GetDMARxDescBufferSize(ETH_DMADESCTypeDef *DMARxDesc, uint32_t DMARxDesc_Buffer);

FlagStatus ETH_GetDMAFlagStatus(uint32_t ETH_DMA_FLAG);
void ETH_DMAClearFlag(uint32_t ETH_DMA_FLAG);
ITStatus ETH_GetDMAITStatus(uint32_t ETH_DMA_IT);
void ETH_DMAClearITPendingBit(uint32_t ETH_DMA_IT);
uint32_t ETH_GetTransmitProcessState(void);
uint32_t ETH_GetReceiveProcessState(void);
void ETH_FlushTransmitFIFO(void);
FlagStatus ETH_GetFlushTransmitFIFOStatus(void);
void ETH_DMATransmissionCmd(FunctionalState NewState);
void ETH_DMAReceptionCmd(FunctionalState NewState);
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState);
FlagStatus ETH_GetDMAOverflowStatus(uint32_t ETH_DMA_Overflow);
uint32_t ETH_GetRxOverflowMissedFrameCounter(void);
uint32_t ETH_GetBufferUnavailableMissedFrameCounter(void);
uint32_t ETH_GetCurrentTxDescStartAddress(void);
uint32_t ETH_GetCurrentRxDescStartAddress(void);
uint32_t ETH_GetCurrentTxBufferAddress(void);
uint32_t ETH_GetCurrentRxBufferAddress(void);
void ETH_ResumeDMATransmission(void);
void ETH_ResumeDMAReception(void);

void ETH_ResetWakeUpFrameFilterRegisterPointer(void);
void ETH_SetWakeUpFrameFilterRegister(uint32_t *Buffer);
void ETH_GlobalUnicastWakeUpCmd(FunctionalState NewState);
FlagStatus ETH_GetPMTFlagStatus(uint32_t ETH_PMT_FLAG);
void ETH_WakeUpFrameDetectionCmd(FunctionalState NewState);
void ETH_MagicPacketDetectionCmd(FunctionalState NewState);
void ETH_PowerDownCmd(FunctionalState NewState);

void ETH_MMCCounterFreezeCmd(FunctionalState NewState);
void ETH_MMCResetOnReadCmd(FunctionalState NewState);
void ETH_MMCCounterRolloverCmd(FunctionalState NewState);
void ETH_MMCCountersReset(void);
void ETH_MMCITConfig(uint32_t ETH_MMC_IT, FunctionalState NewState);
ITStatus ETH_GetMMCITStatus(uint32_t ETH_MMC_IT);
uint32_t ETH_GetMMCRegister(uint32_t ETH_MMCReg);

uint32_t ETH_HandlePTPTxPkt(uint8_t *ppkt, uint16_t FrameLength, uint32_t *PTPTxTab);
uint32_t ETH_HandlePTPRxPkt(uint8_t *ppkt, uint32_t *PTPRxTab);
void ETH_DMAPTPTxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, ETH_DMADESCTypeDef *DMAPTPTxDescTab, uint8_t* TxBuff, uint32_t TxBuffCount);
void ETH_DMAPTPRxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, ETH_DMADESCTypeDef *DMAPTPRxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_EnablePTPTimeStampAddend(void);
void ETH_EnablePTPTimeStampInterruptTrigger(void);
void ETH_EnablePTPTimeStampUpdate(void);
void ETH_InitializePTPTimeStamp(void);
void ETH_PTPUpdateMethodConfig(uint32_t UpdateMethod);
void ETH_PTPTimeStampCmd(FunctionalState NewState);
FlagStatus ETH_GetPTPFlagStatus(uint32_t ETH_PTP_FLAG);
void ETH_SetPTPSubSecondIncrement(uint32_t SubSecondValue);
void ETH_SetPTPTimeStampUpdate(uint32_t Sign, uint32_t SecondValue, uint32_t SubSecondValue);
void ETH_SetPTPTimeStampAddend(uint32_t Value);
void ETH_SetPTPTargetTime(uint32_t HighValue, uint32_t LowValue);
uint32_t ETH_GetPTPRegister(uint32_t ETH_PTPReg);
void RGMII_TXC_Delay(uint8_t clock_polarity,uint8_t delay_time);

#ifdef __cplusplus
}
#endif

#endif

