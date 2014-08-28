/**
  ******************************************************************************
  * @file    stm32f4xx_hal_eth.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of ETH HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_HAL_ETH_H
#define __STM32F4xx_HAL_ETH_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup ETH
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief  HAL State structures definition  
  */ 
typedef enum
{
  HAL_ETH_STATE_RESET             = 0x00,    /*!< Peripheral not yet Initialized or disabled         */
  HAL_ETH_STATE_READY             = 0x01,    /*!< Peripheral Initialized and ready for use           */
  HAL_ETH_STATE_BUSY              = 0x02,    /*!< an internal process is ongoing                     */
  HAL_ETH_STATE_BUSY_TX           = 0x12,    /*!< Data Transmission process is ongoing               */
  HAL_ETH_STATE_BUSY_RX           = 0x22,    /*!< Data Reception process is ongoing                  */
  HAL_ETH_STATE_BUSY_TX_RX        = 0x32,    /*!< Data Transmission and Reception process is ongoing */
  HAL_ETH_STATE_BUSY_WR           = 0x42,    /*!< Write process is ongoing                           */
  HAL_ETH_STATE_BUSY_RD           = 0x82,    /*!< Read process is ongoing                            */
  HAL_ETH_STATE_TIMEOUT           = 0x03,    /*!< Timeout state                                      */
  HAL_ETH_STATE_ERROR             = 0x04     /*!< Reception process is ongoing                       */
}HAL_ETH_StateTypeDef;

/** 
  * @brief  ETH Init Structure definition  
  */

typedef struct
{
  uint32_t             AutoNegotiation;           /*!< Selects or not the AutoNegotiation mode for the external PHY
                                                           The AutoNegotiation allows an automatic setting of the Speed (10/100Mbps)
                                                           and the mode (half/full-duplex).
                                                           This parameter can be a value of @ref ETH_AutoNegotiation */

  uint32_t             Speed;                     /*!< Sets the Ethernet speed: 10/100 Mbps.
                                                           This parameter can be a value of @ref ETH_Speed */

  uint32_t             DuplexMode;                /*!< Selects the MAC duplex mode: Half-Duplex or Full-Duplex mode
                                                           This parameter can be a value of @ref ETH_Duplex_Mode */
  
  uint16_t             PhyAddress;                /*!< Ethernet PHY address.
                                                           This parameter must be a number between Min_Data = 0 and Max_Data = 32 */
  
  uint8_t             *MACAddr;                   /*!< MAC Address of used Hardware: must be pointer on an array of 6 bytes */
  
  uint32_t             RxMode;                    /*!< Selects the Ethernet Rx mode: Polling mode, Interrupt mode.
                                                           This parameter can be a value of @ref ETH_Rx_Mode */
  
  uint32_t             ChecksumMode;              /*!< Selects if the checksum is check by hardware or by software. 
                                                         This parameter can be a value of @ref ETH_Checksum_Mode */
  
  uint32_t             MediaInterface    ;               /*!< Selects the media-independent interface or the reduced media-independent interface. 
                                                         This parameter can be a value of @ref ETH_Media_Interface */

} ETH_InitTypeDef;


 /** 
  * @brief  ETH MAC Configuration Structure definition  
  */

typedef struct
{
  uint32_t             Watchdog;                  /*!< Selects or not the Watchdog timer
                                                           When enabled, the MAC allows no more then 2048 bytes to be received.
                                                           When disabled, the MAC can receive up to 16384 bytes.
                                                           This parameter can be a value of @ref ETH_watchdog */  

  uint32_t             Jabber;                    /*!< Selects or not Jabber timer
                                                           When enabled, the MAC allows no more then 2048 bytes to be sent.
                                                           When disabled, the MAC can send up to 16384 bytes.
                                                           This parameter can be a value of @ref ETH_Jabber */

  uint32_t             InterFrameGap;             /*!< Selects the minimum IFG between frames during transmission.
                                                           This parameter can be a value of @ref ETH_Inter_Frame_Gap */   

  uint32_t             CarrierSense;              /*!< Selects or not the Carrier Sense.
                                                           This parameter can be a value of @ref ETH_Carrier_Sense */

  uint32_t             ReceiveOwn;                /*!< Selects or not the ReceiveOwn,
                                                           ReceiveOwn allows the reception of frames when the TX_EN signal is asserted
                                                           in Half-Duplex mode.
                                                           This parameter can be a value of @ref ETH_Receive_Own */  

  uint32_t             LoopbackMode;              /*!< Selects or not the internal MAC MII Loopback mode.
                                                           This parameter can be a value of @ref ETH_Loop_Back_Mode */  

  uint32_t             ChecksumOffload;           /*!< Selects or not the IPv4 checksum checking for received frame payloads' TCP/UDP/ICMP headers.
                                                           This parameter can be a value of @ref ETH_Checksum_Offload */    

  uint32_t             RetryTransmission;         /*!< Selects or not the MAC attempt retries transmission, based on the settings of BL,
                                                           when a collision occurs (Half-Duplex mode).
                                                           This parameter can be a value of @ref ETH_Retry_Transmission */

  uint32_t             AutomaticPadCRCStrip;      /*!< Selects or not the Automatic MAC Pad/CRC Stripping.
                                                           This parameter can be a value of @ref ETH_Automatic_Pad_CRC_Strip */ 

  uint32_t             BackOffLimit;              /*!< Selects the BackOff limit value.
                                                           This parameter can be a value of @ref ETH_Back_Off_Limit */

  uint32_t             DeferralCheck;             /*!< Selects or not the deferral check function (Half-Duplex mode).
                                                           This parameter can be a value of @ref ETH_Deferral_Check */                                                                                                        

  uint32_t             ReceiveAll;                /*!< Selects or not all frames reception by the MAC (No filtering).
                                                           This parameter can be a value of @ref ETH_Receive_All */   

  uint32_t             SourceAddrFilter;          /*!< Selects the Source Address Filter mode.                                                           
                                                           This parameter can be a value of @ref ETH_Source_Addr_Filter */                  

  uint32_t             PassControlFrames;         /*!< Sets the forwarding mode of the control frames (including unicast and multicast PAUSE frames)                                                          
                                                           This parameter can be a value of @ref ETH_Pass_Control_Frames */ 

  uint32_t             BroadcastFramesReception;  /*!< Selects or not the reception of Broadcast Frames.
                                                           This parameter can be a value of @ref ETH_Broadcast_Frames_Reception */

  uint32_t             DestinationAddrFilter;     /*!< Sets the destination filter mode for both unicast and multicast frames.
                                                           This parameter can be a value of @ref ETH_Destination_Addr_Filter */ 

  uint32_t             PromiscuousMode;           /*!< Selects or not the Promiscuous Mode
                                                           This parameter can be a value of @ref ETH_Promiscuous_Mode */

  uint32_t             MulticastFramesFilter;     /*!< Selects the Multicast Frames filter mode: None/HashTableFilter/PerfectFilter/PerfectHashTableFilter.
                                                           This parameter can be a value of @ref ETH_Multicast_Frames_Filter */ 

  uint32_t             UnicastFramesFilter;       /*!< Selects the Unicast Frames filter mode: HashTableFilter/PerfectFilter/PerfectHashTableFilter.
                                                           This parameter can be a value of @ref ETH_Unicast_Frames_Filter */ 

  uint32_t             HashTableHigh;             /*!< This field holds the higher 32 bits of Hash table.
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFFFFFF */

  uint32_t             HashTableLow;              /*!< This field holds the lower 32 bits of Hash table.
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFFFFFF  */    

  uint32_t             PauseTime;                 /*!< This field holds the value to be used in the Pause Time field in the transmit control frame. 
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFF */

  uint32_t             ZeroQuantaPause;           /*!< Selects or not the automatic generation of Zero-Quanta Pause Control frames.
                                                           This parameter can be a value of @ref ETH_Zero_Quanta_Pause */  

  uint32_t             PauseLowThreshold;         /*!< This field configures the threshold of the PAUSE to be checked for
                                                           automatic retransmission of PAUSE Frame.
                                                           This parameter can be a value of @ref ETH_Pause_Low_Threshold */
                                                           
  uint32_t             UnicastPauseFrameDetect;   /*!< Selects or not the MAC detection of the Pause frames (with MAC Address0
                                                           unicast address and unique multicast address).
                                                           This parameter can be a value of @ref ETH_Unicast_Pause_Frame_Detect */  

  uint32_t             ReceiveFlowControl;        /*!< Enables or disables the MAC to decode the received Pause frame and
                                                           disable its transmitter for a specified time (Pause Time)
                                                           This parameter can be a value of @ref ETH_Receive_Flow_Control */

  uint32_t             TransmitFlowControl;       /*!< Enables or disables the MAC to transmit Pause frames (Full-Duplex mode)
                                                           or the MAC back-pressure operation (Half-Duplex mode)
                                                           This parameter can be a value of @ref ETH_Transmit_Flow_Control */     

  uint32_t             VLANTagComparison;         /*!< Selects the 12-bit VLAN identifier or the complete 16-bit VLAN tag for
                                                           comparison and filtering.
                                                           This parameter can be a value of @ref ETH_VLAN_Tag_Comparison */ 

  uint32_t             VLANTagIdentifier;         /*!< Holds the VLAN tag identifier for receive frames */

} ETH_MACInitTypeDef;


/** 
  * @brief  ETH DMA Configuration Structure definition  
  */

typedef struct
{
 uint32_t              DropTCPIPChecksumErrorFrame; /*!< Selects or not the Dropping of TCP/IP Checksum Error Frames.
                                                             This parameter can be a value of @ref ETH_Drop_TCP_IP_Checksum_Error_Frame */ 

  uint32_t             ReceiveStoreForward;         /*!< Enables or disables the Receive store and forward mode.
                                                             This parameter can be a value of @ref ETH_Receive_Store_Forward */ 

  uint32_t             FlushReceivedFrame;          /*!< Enables or disables the flushing of received frames.
                                                             This parameter can be a value of @ref ETH_Flush_Received_Frame */ 

  uint32_t             TransmitStoreForward;        /*!< Enables or disables Transmit store and forward mode.
                                                             This parameter can be a value of @ref ETH_Transmit_Store_Forward */ 

  uint32_t             TransmitThresholdControl;    /*!< Selects or not the Transmit Threshold Control.
                                                             This parameter can be a value of @ref ETH_Transmit_Threshold_Control */

  uint32_t             ForwardErrorFrames;          /*!< Selects or not the forward to the DMA of erroneous frames.
                                                             This parameter can be a value of @ref ETH_Forward_Error_Frames */

  uint32_t             ForwardUndersizedGoodFrames; /*!< Enables or disables the Rx FIFO to forward Undersized frames (frames with no Error
                                                             and length less than 64 bytes) including pad-bytes and CRC)
                                                             This parameter can be a value of @ref ETH_Forward_Undersized_Good_Frames */

  uint32_t             ReceiveThresholdControl;     /*!< Selects the threshold level of the Receive FIFO.
                                                             This parameter can be a value of @ref ETH_Receive_Threshold_Control */

  uint32_t             SecondFrameOperate;          /*!< Selects or not the Operate on second frame mode, which allows the DMA to process a second
                                                             frame of Transmit data even before obtaining the status for the first frame.
                                                             This parameter can be a value of @ref ETH_Second_Frame_Operate */

  uint32_t             AddressAlignedBeats;         /*!< Enables or disables the Address Aligned Beats.
                                                             This parameter can be a value of @ref ETH_Address_Aligned_Beats */

  uint32_t             FixedBurst;                  /*!< Enables or disables the AHB Master interface fixed burst transfers.
                                                             This parameter can be a value of @ref ETH_Fixed_Burst */
                       
  uint32_t             RxDMABurstLength;            /*!< Indicates the maximum number of beats to be transferred in one Rx DMA transaction.
                                                             This parameter can be a value of @ref ETH_Rx_DMA_Burst_Length */ 

  uint32_t             TxDMABurstLength;            /*!< Indicates the maximum number of beats to be transferred in one Tx DMA transaction.
                                                             This parameter can be a value of @ref ETH_Tx_DMA_Burst_Length */
  
  uint32_t             EnhancedDescriptorFormat;    /*!< Enables the enhanced descriptor format.
                                                             This parameter can be a value of @ref ETH_DMA_Enhanced_descriptor_format */

  uint32_t             DescriptorSkipLength;        /*!< Specifies the number of word to skip between two unchained descriptors (Ring mode)
                                                             This parameter must be a number between Min_Data = 0 and Max_Data = 32 */                                                             

  uint32_t             DMAArbitration;              /*!< Selects the DMA Tx/Rx arbitration.
                                                             This parameter can be a value of @ref ETH_DMA_Arbitration */  
} ETH_DMAInitTypeDef;


/** 
  * @brief  ETH DMA Descriptors data structure definition
  */ 

typedef struct  
{
  __IO uint32_t   Status;           /*!< Status */
  
  uint32_t   ControlBufferSize;     /*!< Control and Buffer1, Buffer2 lengths */
  
  uint32_t   Buffer1Addr;           /*!< Buffer1 address pointer */
  
  uint32_t   Buffer2NextDescAddr;   /*!< Buffer2 or next descriptor address pointer */
  
  /*!< Enhanced ETHERNET DMA PTP Descriptors */
  uint32_t   ExtendedStatus;        /*!< Extended status for PTP receive descriptor */
  
  uint32_t   Reserved1;             /*!< Reserved */
  
  uint32_t   TimeStampLow;          /*!< Time Stamp Low value for transmit and receive */
  
  uint32_t   TimeStampHigh;         /*!< Time Stamp High value for transmit and receive */

} ETH_DMADescTypeDef;


/** 
  * @brief  Received Frame Informations structure definition
  */ 
typedef struct  
{
  ETH_DMADescTypeDef *FSRxDesc;          /*!< First Segment Rx Desc */
  
  ETH_DMADescTypeDef *LSRxDesc;          /*!< Last Segment Rx Desc */
  
  uint32_t  SegCount;                    /*!< Segment count */
  
  uint32_t length;                       /*!< Frame length */
  
  uint32_t buffer;                       /*!< Frame buffer */

} ETH_DMARxFrameInfos;


/** 
  * @brief  ETH Handle Structure definition  
  */
  
typedef struct
{
  ETH_TypeDef                *Instance;     /*!< Register base address       */
  
  ETH_InitTypeDef            Init;          /*!< Ethernet Init Configuration */
  
  uint32_t                   LinkStatus;    /*!< Ethernet link status        */
  
  ETH_DMADescTypeDef         *RxDesc;       /*!< Rx descriptor to Get        */
  
  ETH_DMADescTypeDef         *TxDesc;       /*!< Tx descriptor to Set        */
  
  ETH_DMARxFrameInfos        RxFrameInfos;  /*!< last Rx frame infos         */
  
  __IO HAL_ETH_StateTypeDef  State;         /*!< ETH communication state     */
  
  HAL_LockTypeDef            Lock;          /*!< ETH Lock                    */

} ETH_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

#define IS_ETH_PHY_ADDRESS(ADDRESS) ((ADDRESS) <= 0x20)

/* Delay to wait when writing to some Ethernet registers */
#define ETH_REG_WRITE_DELAY ((uint32_t)0x00000001)


/* ETHERNET Errors */
#define  ETH_SUCCESS            ((uint32_t)0)
#define  ETH_ERROR              ((uint32_t)1)

/** @defgroup ETH_Buffers_setting 
  * @{
  */ 
#define ETH_MAX_PACKET_SIZE    ((uint32_t)1524)    /*!< ETH_HEADER + ETH_EXTRA + VLAN_TAG + MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER               ((uint32_t)14)    /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC                   ((uint32_t)4)    /*!< Ethernet CRC */
#define ETH_EXTRA                 ((uint32_t)2)    /*!< Extra bytes in some cases */   
#define VLAN_TAG                  ((uint32_t)4)    /*!< optional 802.1q VLAN Tag */
#define MIN_ETH_PAYLOAD          ((uint32_t)46)    /*!< Minimum Ethernet payload size */
#define MAX_ETH_PAYLOAD        ((uint32_t)1500)    /*!< Maximum Ethernet payload size */
#define JUMBO_FRAME_PAYLOAD    ((uint32_t)9000)    /*!< Jumbo frame payload size */      

 /* Ethernet driver receive buffers are organized in a chained linked-list, when
    an ethernet packet is received, the Rx-DMA will transfer the packet from RxFIFO
    to the driver receive buffers memory.

    Depending on the size of the received ethernet packet and the size of 
    each ethernet driver receive buffer, the received packet can take one or more
    ethernet driver receive buffer. 

    In below are defined the size of one ethernet driver receive buffer ETH_RX_BUF_SIZE 
    and the total count of the driver receive buffers ETH_RXBUFNB.

    The configured value for ETH_RX_BUF_SIZE and ETH_RXBUFNB are only provided as 
    example, they can be reconfigured in the application layer to fit the application 
    needs */ 

/* Here we configure each Ethernet driver receive buffer to fit the Max size Ethernet
   packet */
#ifndef ETH_RX_BUF_SIZE
 #define ETH_RX_BUF_SIZE         ETH_MAX_PACKET_SIZE 
#endif

/* 5 Ethernet driver receive buffers are used (in a chained linked list)*/ 
#ifndef ETH_RXBUFNB
 #define ETH_RXBUFNB             ((uint32_t)5     /*  5 Rx buffers of size ETH_RX_BUF_SIZE */
#endif


 /* Ethernet driver transmit buffers are organized in a chained linked-list, when
    an ethernet packet is transmitted, Tx-DMA will transfer the packet from the 
    driver transmit buffers memory to the TxFIFO.

    Depending on the size of the Ethernet packet to be transmitted and the size of 
    each ethernet driver transmit buffer, the packet to be transmitted can take 
    one or more ethernet driver transmit buffer. 

    In below are defined the size of one ethernet driver transmit buffer ETH_TX_BUF_SIZE 
    and the total count of the driver transmit buffers ETH_TXBUFNB.

    The configured value for ETH_TX_BUF_SIZE and ETH_TXBUFNB are only provided as 
    example, they can be reconfigured in the application layer to fit the application 
    needs */ 

/* Here we configure each Ethernet driver transmit buffer to fit the Max size Ethernet
   packet */
#ifndef ETH_TX_BUF_SIZE 
 #define ETH_TX_BUF_SIZE         ETH_MAX_PACKET_SIZE
#endif

/* 5 ethernet driver transmit buffers are used (in a chained linked list)*/ 
#ifndef ETH_TXBUFNB
 #define ETH_TXBUFNB             ((uint32_t)5      /* 5  Tx buffers of size ETH_TX_BUF_SIZE */
#endif


/*
   DMA Tx Desciptor
  -----------------------------------------------------------------------------------------------
  TDES0 | OWN(31) | CTRL[30:26] | Reserved[25:24] | CTRL[23:20] | Reserved[19:17] | Status[16:0] |
  -----------------------------------------------------------------------------------------------
  TDES1 | Reserved[31:29] | Buffer2 ByteCount[28:16] | Reserved[15:13] | Buffer1 ByteCount[12:0] |
  -----------------------------------------------------------------------------------------------
  TDES2 |                         Buffer1 Address [31:0]                                         |
  -----------------------------------------------------------------------------------------------
  TDES3 |                   Buffer2 Address [31:0] / Next Descriptor Address [31:0]              |
  -----------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of TDES0 register: DMA Tx descriptor status register
  */ 
#define ETH_DMATXDESC_OWN                     ((uint32_t)0x80000000)  /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXDESC_IC                      ((uint32_t)0x40000000)  /*!< Interrupt on Completion */
#define ETH_DMATXDESC_LS                      ((uint32_t)0x20000000)  /*!< Last Segment */
#define ETH_DMATXDESC_FS                      ((uint32_t)0x10000000)  /*!< First Segment */
#define ETH_DMATXDESC_DC                      ((uint32_t)0x08000000)  /*!< Disable CRC */
#define ETH_DMATXDESC_DP                      ((uint32_t)0x04000000)  /*!< Disable Padding */
#define ETH_DMATXDESC_TTSE                    ((uint32_t)0x02000000)  /*!< Transmit Time Stamp Enable */
#define ETH_DMATXDESC_CIC                     ((uint32_t)0x00C00000)  /*!< Checksum Insertion Control: 4 cases */
#define ETH_DMATXDESC_CIC_BYPASS              ((uint32_t)0x00000000)  /*!< Do Nothing: Checksum Engine is bypassed */ 
#define ETH_DMATXDESC_CIC_IPV4HEADER          ((uint32_t)0x00400000)  /*!< IPV4 header Checksum Insertion */ 
#define ETH_DMATXDESC_CIC_TCPUDPICMP_SEGMENT  ((uint32_t)0x00800000)  /*!< TCP/UDP/ICMP Checksum Insertion calculated over segment only */ 
#define ETH_DMATXDESC_CIC_TCPUDPICMP_FULL     ((uint32_t)0x00C00000)  /*!< TCP/UDP/ICMP Checksum Insertion fully calculated */ 
#define ETH_DMATXDESC_TER                     ((uint32_t)0x00200000)  /*!< Transmit End of Ring */
#define ETH_DMATXDESC_TCH                     ((uint32_t)0x00100000)  /*!< Second Address Chained */
#define ETH_DMATXDESC_TTSS                    ((uint32_t)0x00020000)  /*!< Tx Time Stamp Status */
#define ETH_DMATXDESC_IHE                     ((uint32_t)0x00010000)  /*!< IP Header Error */
#define ETH_DMATXDESC_ES                      ((uint32_t)0x00008000)  /*!< Error summary: OR of the following bits: UE || ED || EC || LCO || NC || LCA || FF || JT */
#define ETH_DMATXDESC_JT                      ((uint32_t)0x00004000)  /*!< Jabber Timeout */
#define ETH_DMATXDESC_FF                      ((uint32_t)0x00002000)  /*!< Frame Flushed: DMA/MTL flushed the frame due to SW flush */
#define ETH_DMATXDESC_PCE                     ((uint32_t)0x00001000)  /*!< Payload Checksum Error */
#define ETH_DMATXDESC_LCA                     ((uint32_t)0x00000800)  /*!< Loss of Carrier: carrier lost during transmission */
#define ETH_DMATXDESC_NC                      ((uint32_t)0x00000400)  /*!< No Carrier: no carrier signal from the transceiver */
#define ETH_DMATXDESC_LCO                     ((uint32_t)0x00000200)  /*!< Late Collision: transmission aborted due to collision */
#define ETH_DMATXDESC_EC                      ((uint32_t)0x00000100)  /*!< Excessive Collision: transmission aborted after 16 collisions */
#define ETH_DMATXDESC_VF                      ((uint32_t)0x00000080)  /*!< VLAN Frame */
#define ETH_DMATXDESC_CC                      ((uint32_t)0x00000078)  /*!< Collision Count */
#define ETH_DMATXDESC_ED                      ((uint32_t)0x00000004)  /*!< Excessive Deferral */
#define ETH_DMATXDESC_UF                      ((uint32_t)0x00000002)  /*!< Underflow Error: late data arrival from the memory */
#define ETH_DMATXDESC_DB                      ((uint32_t)0x00000001)  /*!< Deferred Bit */

/** 
  * @brief  Bit definition of TDES1 register
  */ 
#define ETH_DMATXDESC_TBS2  ((uint32_t)0x1FFF0000)  /*!< Transmit Buffer2 Size */
#define ETH_DMATXDESC_TBS1  ((uint32_t)0x00001FFF)  /*!< Transmit Buffer1 Size */

/** 
  * @brief  Bit definition of TDES2 register
  */ 
#define ETH_DMATXDESC_B1AP  ((uint32_t)0xFFFFFFFF)  /*!< Buffer1 Address Pointer */

/** 
  * @brief  Bit definition of TDES3 register
  */ 
#define ETH_DMATXDESC_B2AP  ((uint32_t)0xFFFFFFFF)  /*!< Buffer2 Address Pointer */

  /*---------------------------------------------------------------------------------------------
  TDES6 |                         Transmit Time Stamp Low [31:0]                                 |
  -----------------------------------------------------------------------------------------------
  TDES7 |                         Transmit Time Stamp High [31:0]                                |
  ----------------------------------------------------------------------------------------------*/

/* Bit definition of TDES6 register */
 #define ETH_DMAPTPTXDESC_TTSL  ((uint32_t)0xFFFFFFFF)  /* Transmit Time Stamp Low */

/* Bit definition of TDES7 register */
 #define ETH_DMAPTPTXDESC_TTSH  ((uint32_t)0xFFFFFFFF)  /* Transmit Time Stamp High */

/**
  * @}
  */ 


/** @defgroup ETH_DMA_Rx_descriptor 
  * @{
  */

/*
  DMA Rx Descriptor
  --------------------------------------------------------------------------------------------------------------------
  RDES0 | OWN(31) |                                             Status [30:0]                                          |
  ---------------------------------------------------------------------------------------------------------------------
  RDES1 | CTRL(31) | Reserved[30:29] | Buffer2 ByteCount[28:16] | CTRL[15:14] | Reserved(13) | Buffer1 ByteCount[12:0] |
  ---------------------------------------------------------------------------------------------------------------------
  RDES2 |                                       Buffer1 Address [31:0]                                                 |
  ---------------------------------------------------------------------------------------------------------------------
  RDES3 |                          Buffer2 Address [31:0] / Next Descriptor Address [31:0]                             |
  ---------------------------------------------------------------------------------------------------------------------
*/

/** 
  * @brief  Bit definition of RDES0 register: DMA Rx descriptor status register
  */ 
#define ETH_DMARXDESC_OWN         ((uint32_t)0x80000000)  /*!< OWN bit: descriptor is owned by DMA engine  */
#define ETH_DMARXDESC_AFM         ((uint32_t)0x40000000)  /*!< DA Filter Fail for the rx frame  */
#define ETH_DMARXDESC_FL          ((uint32_t)0x3FFF0000)  /*!< Receive descriptor frame length  */
#define ETH_DMARXDESC_ES          ((uint32_t)0x00008000)  /*!< Error summary: OR of the following bits: DE || OE || IPC || LC || RWT || RE || CE */
#define ETH_DMARXDESC_DE          ((uint32_t)0x00004000)  /*!< Descriptor error: no more descriptors for receive frame  */
#define ETH_DMARXDESC_SAF         ((uint32_t)0x00002000)  /*!< SA Filter Fail for the received frame */
#define ETH_DMARXDESC_LE          ((uint32_t)0x00001000)  /*!< Frame size not matching with length field */
#define ETH_DMARXDESC_OE          ((uint32_t)0x00000800)  /*!< Overflow Error: Frame was damaged due to buffer overflow */
#define ETH_DMARXDESC_VLAN        ((uint32_t)0x00000400)  /*!< VLAN Tag: received frame is a VLAN frame */
#define ETH_DMARXDESC_FS          ((uint32_t)0x00000200)  /*!< First descriptor of the frame  */
#define ETH_DMARXDESC_LS          ((uint32_t)0x00000100)  /*!< Last descriptor of the frame  */ 
#define ETH_DMARXDESC_IPV4HCE     ((uint32_t)0x00000080)  /*!< IPC Checksum Error: Rx Ipv4 header checksum error   */    
#define ETH_DMARXDESC_LC          ((uint32_t)0x00000040)  /*!< Late collision occurred during reception   */
#define ETH_DMARXDESC_FT          ((uint32_t)0x00000020)  /*!< Frame type - Ethernet, otherwise 802.3    */
#define ETH_DMARXDESC_RWT         ((uint32_t)0x00000010)  /*!< Receive Watchdog Timeout: watchdog timer expired during reception    */
#define ETH_DMARXDESC_RE          ((uint32_t)0x00000008)  /*!< Receive error: error reported by MII interface  */
#define ETH_DMARXDESC_DBE         ((uint32_t)0x00000004)  /*!< Dribble bit error: frame contains non int multiple of 8 bits  */
#define ETH_DMARXDESC_CE          ((uint32_t)0x00000002)  /*!< CRC error */
#define ETH_DMARXDESC_MAMPCE      ((uint32_t)0x00000001)  /*!< Rx MAC Address/Payload Checksum Error: Rx MAC address matched/ Rx Payload Checksum Error */

/** 
  * @brief  Bit definition of RDES1 register
  */ 
#define ETH_DMARXDESC_DIC   ((uint32_t)0x80000000)  /*!< Disable Interrupt on Completion */
#define ETH_DMARXDESC_RBS2  ((uint32_t)0x1FFF0000)  /*!< Receive Buffer2 Size */
#define ETH_DMARXDESC_RER   ((uint32_t)0x00008000)  /*!< Receive End of Ring */
#define ETH_DMARXDESC_RCH   ((uint32_t)0x00004000)  /*!< Second Address Chained */
#define ETH_DMARXDESC_RBS1  ((uint32_t)0x00001FFF)  /*!< Receive Buffer1 Size */

/** 
  * @brief  Bit definition of RDES2 register  
  */ 
#define ETH_DMARXDESC_B1AP  ((uint32_t)0xFFFFFFFF)  /*!< Buffer1 Address Pointer */

/** 
  * @brief  Bit definition of RDES3 register  
  */ 
#define ETH_DMARXDESC_B2AP  ((uint32_t)0xFFFFFFFF)  /*!< Buffer2 Address Pointer */

/*---------------------------------------------------------------------------------------------------------------------
  RDES4 |                   Reserved[31:15]              |             Extended Status [14:0]                          |
  ---------------------------------------------------------------------------------------------------------------------
  RDES5 |                                            Reserved[31:0]                                                    |
  ---------------------------------------------------------------------------------------------------------------------
  RDES6 |                                       Receive Time Stamp Low [31:0]                                          |
  ---------------------------------------------------------------------------------------------------------------------
  RDES7 |                                       Receive Time Stamp High [31:0]                                         |
  --------------------------------------------------------------------------------------------------------------------*/

/* Bit definition of RDES4 register */
#define ETH_DMAPTPRXDESC_PTPV     ((uint32_t)0x00002000)  /* PTP Version */
#define ETH_DMAPTPRXDESC_PTPFT    ((uint32_t)0x00001000)  /* PTP Frame Type */
#define ETH_DMAPTPRXDESC_PTPMT    ((uint32_t)0x00000F00)  /* PTP Message Type */
  #define ETH_DMAPTPRXDESC_PTPMT_SYNC                      ((uint32_t)0x00000100)  /* SYNC message (all clock types) */
  #define ETH_DMAPTPRXDESC_PTPMT_FOLLOWUP                  ((uint32_t)0x00000200)  /* FollowUp message (all clock types) */ 
  #define ETH_DMAPTPRXDESC_PTPMT_DELAYREQ                  ((uint32_t)0x00000300)  /* DelayReq message (all clock types) */ 
  #define ETH_DMAPTPRXDESC_PTPMT_DELAYRESP                 ((uint32_t)0x00000400)  /* DelayResp message (all clock types) */ 
  #define ETH_DMAPTPRXDESC_PTPMT_PDELAYREQ_ANNOUNCE        ((uint32_t)0x00000500)  /* PdelayReq message (peer-to-peer transparent clock) or Announce message (Ordinary or Boundary clock) */ 
  #define ETH_DMAPTPRXDESC_PTPMT_PDELAYRESP_MANAG          ((uint32_t)0x00000600)  /* PdelayResp message (peer-to-peer transparent clock) or Management message (Ordinary or Boundary clock)  */ 
  #define ETH_DMAPTPRXDESC_PTPMT_PDELAYRESPFOLLOWUP_SIGNAL ((uint32_t)0x00000700)  /* PdelayRespFollowUp message (peer-to-peer transparent clock) or Signaling message (Ordinary or Boundary clock) */           
#define ETH_DMAPTPRXDESC_IPV6PR   ((uint32_t)0x00000080)  /* IPv6 Packet Received */
#define ETH_DMAPTPRXDESC_IPV4PR   ((uint32_t)0x00000040)  /* IPv4 Packet Received */
#define ETH_DMAPTPRXDESC_IPCB  ((uint32_t)0x00000020)  /* IP Checksum Bypassed */
#define ETH_DMAPTPRXDESC_IPPE  ((uint32_t)0x00000010)  /* IP Payload Error */
#define ETH_DMAPTPRXDESC_IPHE  ((uint32_t)0x00000008)  /* IP Header Error */
#define ETH_DMAPTPRXDESC_IPPT  ((uint32_t)0x00000007)  /* IP Payload Type */
  #define ETH_DMAPTPRXDESC_IPPT_UDP                 ((uint32_t)0x00000001)  /* UDP payload encapsulated in the IP datagram */
  #define ETH_DMAPTPRXDESC_IPPT_TCP                 ((uint32_t)0x00000002)  /* TCP payload encapsulated in the IP datagram */ 
  #define ETH_DMAPTPRXDESC_IPPT_ICMP                ((uint32_t)0x00000003)  /* ICMP payload encapsulated in the IP datagram */

/* Bit definition of RDES6 register */
#define ETH_DMAPTPRXDESC_RTSL  ((uint32_t)0xFFFFFFFF)  /* Receive Time Stamp Low */

/* Bit definition of RDES7 register */
#define ETH_DMAPTPRXDESC_RTSH  ((uint32_t)0xFFFFFFFF)  /* Receive Time Stamp High */


 /** @defgroup ETH_AutoNegotiation 
  * @{
  */ 
#define ETH_AUTONEGOTIATION_ENABLE     ((uint32_t)0x00000001)
#define ETH_AUTONEGOTIATION_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_AUTONEGOTIATION(CMD) (((CMD) == ETH_AUTONEGOTIATION_ENABLE) || \
                                     ((CMD) == ETH_AUTONEGOTIATION_DISABLE))
/**
  * @}
  */
/** @defgroup ETH_Speed 
  * @{
  */ 
#define ETH_SPEED_10M        ((uint32_t)0x00000000)
#define ETH_SPEED_100M       ((uint32_t)0x00004000)
#define IS_ETH_SPEED(SPEED) (((SPEED) == ETH_SPEED_10M) || \
                             ((SPEED) == ETH_SPEED_100M))
/**
  * @}
  */
/** @defgroup ETH_Duplex_Mode 
  * @{
  */ 
#define ETH_MODE_FULLDUPLEX       ((uint32_t)0x00000800)
#define ETH_MODE_HALFDUPLEX       ((uint32_t)0x00000000)
#define IS_ETH_DUPLEX_MODE(MODE)  (((MODE) == ETH_MODE_FULLDUPLEX) || \
                                  ((MODE) == ETH_MODE_HALFDUPLEX))
/**
  * @}
  */
/** @defgroup ETH_Rx_Mode 
  * @{
  */ 
#define ETH_RXPOLLING_MODE      ((uint32_t)0x00000000)
#define ETH_RXINTERRUPT_MODE    ((uint32_t)0x00000001)
#define IS_ETH_RX_MODE(MODE)    (((MODE) == ETH_RXPOLLING_MODE) || \
                                 ((MODE) == ETH_RXINTERRUPT_MODE))
/**
  * @}
  */

/** @defgroup ETH_Checksum_Mode
  * @{
  */ 
#define ETH_CHECKSUM_BY_HARDWARE      ((uint32_t)0x00000000)
#define ETH_CHECKSUM_BY_SOFTWARE      ((uint32_t)0x00000001)
#define IS_ETH_CHECKSUM_MODE(MODE)    (((MODE) == ETH_CHECKSUM_BY_HARDWARE) || \
                                      ((MODE) == ETH_CHECKSUM_BY_SOFTWARE))
/**
  * @}
  */

/** @defgroup ETH_Media_Interface
  * @{
  */ 
#define ETH_MEDIA_INTERFACE_MII       ((uint32_t)0x00000000)
#define ETH_MEDIA_INTERFACE_RMII      ((uint32_t)SYSCFG_PMC_MII_RMII_SEL)
#define IS_ETH_MEDIA_INTERFACE(MODE)         (((MODE) == ETH_MEDIA_INTERFACE_MII) || \
                                              ((MODE) == ETH_MEDIA_INTERFACE_RMII))

/**
  * @}
  */

/** @defgroup ETH_watchdog 
  * @{
  */ 
#define ETH_WATCHDOG_ENABLE       ((uint32_t)0x00000000)
#define ETH_WATCHDOG_DISABLE      ((uint32_t)0x00800000)
#define IS_ETH_WATCHDOG(CMD) (((CMD) == ETH_WATCHDOG_ENABLE) || \
                              ((CMD) == ETH_WATCHDOG_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Jabber 
  * @{
  */ 
#define ETH_JABBER_ENABLE    ((uint32_t)0x00000000)
#define ETH_JABBER_DISABLE   ((uint32_t)0x00400000)
#define IS_ETH_JABBER(CMD) (((CMD) == ETH_JABBER_ENABLE) || \
                            ((CMD) == ETH_JABBER_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Inter_Frame_Gap 
  * @{
  */ 
#define ETH_INTERFRAMEGAP_96BIT   ((uint32_t)0x00000000)  /*!< minimum IFG between frames during transmission is 96Bit */
#define ETH_INTERFRAMEGAP_88BIT   ((uint32_t)0x00020000)  /*!< minimum IFG between frames during transmission is 88Bit */
#define ETH_INTERFRAMEGAP_80BIT   ((uint32_t)0x00040000)  /*!< minimum IFG between frames during transmission is 80Bit */
#define ETH_INTERFRAMEGAP_72BIT   ((uint32_t)0x00060000)  /*!< minimum IFG between frames during transmission is 72Bit */
#define ETH_INTERFRAMEGAP_64BIT   ((uint32_t)0x00080000)  /*!< minimum IFG between frames during transmission is 64Bit */
#define ETH_INTERFRAMEGAP_56BIT   ((uint32_t)0x000A0000)  /*!< minimum IFG between frames during transmission is 56Bit */
#define ETH_INTERFRAMEGAP_48BIT   ((uint32_t)0x000C0000)  /*!< minimum IFG between frames during transmission is 48Bit */
#define ETH_INTERFRAMEGAP_40BIT   ((uint32_t)0x000E0000)  /*!< minimum IFG between frames during transmission is 40Bit */
#define IS_ETH_INTER_FRAME_GAP(GAP) (((GAP) == ETH_INTERFRAMEGAP_96BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_88BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_80BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_72BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_64BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_56BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_48BIT) || \
                                     ((GAP) == ETH_INTERFRAMEGAP_40BIT))

/**
  * @}
  */

/** @defgroup ETH_Carrier_Sense 
  * @{
  */ 
#define ETH_CARRIERSENCE_ENABLE   ((uint32_t)0x00000000)
#define ETH_CARRIERSENCE_DISABLE ((uint32_t)0x00010000)
#define IS_ETH_CARRIER_SENSE(CMD) (((CMD) == ETH_CARRIERSENCE_ENABLE) || \
                                   ((CMD) == ETH_CARRIERSENCE_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Receive_Own 
  * @{
  */ 
#define ETH_RECEIVEOWN_ENABLE     ((uint32_t)0x00000000)
#define ETH_RECEIVEOWN_DISABLE   ((uint32_t)0x00002000)
#define IS_ETH_RECEIVE_OWN(CMD) (((CMD) == ETH_RECEIVEOWN_ENABLE) || \
                                 ((CMD) == ETH_RECEIVEOWN_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Loop_Back_Mode 
  * @{
  */ 
#define ETH_LOOPBACKMODE_ENABLE        ((uint32_t)0x00001000)
#define ETH_LOOPBACKMODE_DISABLE       ((uint32_t)0x00000000)
#define IS_ETH_LOOPBACK_MODE(CMD) (((CMD) == ETH_LOOPBACKMODE_ENABLE) || \
                                   ((CMD) == ETH_LOOPBACKMODE_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Checksum_Offload 
  * @{
  */ 
#define ETH_CHECKSUMOFFLAOD_ENABLE     ((uint32_t)0x00000400)
#define ETH_CHECKSUMOFFLAOD_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_CHECKSUM_OFFLOAD(CMD) (((CMD) == ETH_CHECKSUMOFFLAOD_ENABLE) || \
                                      ((CMD) == ETH_CHECKSUMOFFLAOD_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Retry_Transmission 
  * @{
  */ 
#define ETH_RETRYTRANSMISSION_ENABLE   ((uint32_t)0x00000000)
#define ETH_RETRYTRANSMISSION_DISABLE  ((uint32_t)0x00000200)
#define IS_ETH_RETRY_TRANSMISSION(CMD) (((CMD) == ETH_RETRYTRANSMISSION_ENABLE) || \
                                        ((CMD) == ETH_RETRYTRANSMISSION_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Automatic_Pad_CRC_Strip 
  * @{
  */ 
#define ETH_AUTOMATICPADCRCSTRIP_ENABLE     ((uint32_t)0x00000080)
#define ETH_AUTOMATICPADCRCSTRIP_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_AUTOMATIC_PADCRC_STRIP(CMD) (((CMD) == ETH_AUTOMATICPADCRCSTRIP_ENABLE) || \
                                            ((CMD) == ETH_AUTOMATICPADCRCSTRIP_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Back_Off_Limit 
  * @{
  */ 
#define ETH_BACKOFFLIMIT_10  ((uint32_t)0x00000000)
#define ETH_BACKOFFLIMIT_8   ((uint32_t)0x00000020)
#define ETH_BACKOFFLIMIT_4   ((uint32_t)0x00000040)
#define ETH_BACKOFFLIMIT_1   ((uint32_t)0x00000060)
#define IS_ETH_BACKOFF_LIMIT(LIMIT) (((LIMIT) == ETH_BACKOFFLIMIT_10) || \
                                     ((LIMIT) == ETH_BACKOFFLIMIT_8) || \
                                     ((LIMIT) == ETH_BACKOFFLIMIT_4) || \
                                     ((LIMIT) == ETH_BACKOFFLIMIT_1))

/**
  * @}
  */

/** @defgroup ETH_Deferral_Check 
  * @{
  */
#define ETH_DEFFERRALCHECK_ENABLE       ((uint32_t)0x00000010)
#define ETH_DEFFERRALCHECK_DISABLE      ((uint32_t)0x00000000)
#define IS_ETH_DEFERRAL_CHECK(CMD) (((CMD) == ETH_DEFFERRALCHECK_ENABLE) || \
                                    ((CMD) == ETH_DEFFERRALCHECK_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Receive_All 
  * @{
  */ 
#define ETH_RECEIVEALL_ENABLE     ((uint32_t)0x80000000)
#define ETH_RECEIVEAll_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_RECEIVE_ALL(CMD) (((CMD) == ETH_RECEIVEALL_ENABLE) || \
                                 ((CMD) == ETH_RECEIVEAll_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Source_Addr_Filter 
  * @{
  */ 
#define ETH_SOURCEADDRFILTER_NORMAL_ENABLE       ((uint32_t)0x00000200)
#define ETH_SOURCEADDRFILTER_INVERSE_ENABLE      ((uint32_t)0x00000300)
#define ETH_SOURCEADDRFILTER_DISABLE             ((uint32_t)0x00000000)
#define IS_ETH_SOURCE_ADDR_FILTER(CMD) (((CMD) == ETH_SOURCEADDRFILTER_NORMAL_ENABLE) || \
                                        ((CMD) == ETH_SOURCEADDRFILTER_INVERSE_ENABLE) || \
                                        ((CMD) == ETH_SOURCEADDRFILTER_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Pass_Control_Frames 
  * @{
  */ 
#define ETH_PASSCONTROLFRAMES_BLOCKALL                ((uint32_t)0x00000040)  /*!< MAC filters all control frames from reaching the application */
#define ETH_PASSCONTROLFRAMES_FORWARDALL              ((uint32_t)0x00000080)  /*!< MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_PASSCONTROLFRAMES_FORWARDPASSEDADDRFILTER ((uint32_t)0x000000C0)  /*!< MAC forwards control frames that pass the Address Filter. */ 
#define IS_ETH_CONTROL_FRAMES(PASS) (((PASS) == ETH_PASSCONTROLFRAMES_BLOCKALL) || \
                                     ((PASS) == ETH_PASSCONTROLFRAMES_FORWARDALL) || \
                                     ((PASS) == ETH_PASSCONTROLFRAMES_FORWARDPASSEDADDRFILTER))

/**
  * @}
  */

/** @defgroup ETH_Broadcast_Frames_Reception 
  * @{
  */ 
#define ETH_BROADCASTFRAMESRECEPTION_ENABLE     ((uint32_t)0x00000000)
#define ETH_BROADCASTFRAMESRECEPTION_DISABLE    ((uint32_t)0x00000020)
#define IS_ETH_BROADCAST_FRAMES_RECEPTION(CMD) (((CMD) == ETH_BROADCASTFRAMESRECEPTION_ENABLE) || \
                                                ((CMD) == ETH_BROADCASTFRAMESRECEPTION_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Destination_Addr_Filter 
  * @{
  */ 
#define ETH_DESTINATIONADDRFILTER_NORMAL    ((uint32_t)0x00000000)
#define ETH_DESTINATIONADDRFILTER_INVERSE   ((uint32_t)0x00000008)
#define IS_ETH_DESTINATION_ADDR_FILTER(FILTER) (((FILTER) == ETH_DESTINATIONADDRFILTER_NORMAL) || \
                                                ((FILTER) == ETH_DESTINATIONADDRFILTER_INVERSE))

/**
  * @}
  */

/** @defgroup ETH_Promiscuous_Mode 
  * @{
  */ 
#define ETH_PROMISCIOUSMODE_ENABLE     ((uint32_t)0x00000001)
#define ETH_PROMISCIOUSMODE_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_PROMISCIOUS_MODE(CMD) (((CMD) == ETH_PROMISCIOUSMODE_ENABLE) || \
                                      ((CMD) == ETH_PROMISCIOUSMODE_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Multicast_Frames_Filter 
  * @{
  */ 
#define ETH_MULTICASTFRAMESFILTER_PERFECTHASHTABLE    ((uint32_t)0x00000404)
#define ETH_MULTICASTFRAMESFILTER_HASHTABLE           ((uint32_t)0x00000004)
#define ETH_MULTICASTFRAMESFILTER_PERFECT             ((uint32_t)0x00000000)
#define ETH_MULTICASTFRAMESFILTER_NONE                ((uint32_t)0x00000010)
#define IS_ETH_MULTICAST_FRAMES_FILTER(FILTER) (((FILTER) == ETH_MULTICASTFRAMESFILTER_PERFECTHASHTABLE) || \
                                                ((FILTER) == ETH_MULTICASTFRAMESFILTER_HASHTABLE) || \
                                                ((FILTER) == ETH_MULTICASTFRAMESFILTER_PERFECT) || \
                                                ((FILTER) == ETH_MULTICASTFRAMESFILTER_NONE))
/**
  * @}
  */

/** @defgroup ETH_Unicast_Frames_Filter 
  * @{
  */ 
#define ETH_UNICASTFRAMESFILTER_PERFECTHASHTABLE ((uint32_t)0x00000402)
#define ETH_UNICASTFRAMESFILTER_HASHTABLE        ((uint32_t)0x00000002)
#define ETH_UNICASTFRAMESFILTER_PERFECT          ((uint32_t)0x00000000)
#define IS_ETH_UNICAST_FRAMES_FILTER(FILTER) (((FILTER) == ETH_UNICASTFRAMESFILTER_PERFECTHASHTABLE) || \
                                              ((FILTER) == ETH_UNICASTFRAMESFILTER_HASHTABLE) || \
                                              ((FILTER) == ETH_UNICASTFRAMESFILTER_PERFECT))
/**
  * @}
  */

/** @defgroup ETH_Pause_Time 
  * @{
  */ 
#define IS_ETH_PAUSE_TIME(TIME) ((TIME) <= 0xFFFF)

/**
  * @}
  */

/** @defgroup ETH_Zero_Quanta_Pause 
  * @{
  */ 
#define ETH_ZEROQUANTAPAUSE_ENABLE     ((uint32_t)0x00000000)
#define ETH_ZEROQUANTAPAUSE_DISABLE    ((uint32_t)0x00000080)
#define IS_ETH_ZEROQUANTA_PAUSE(CMD)   (((CMD) == ETH_ZEROQUANTAPAUSE_ENABLE) || \
                                        ((CMD) == ETH_ZEROQUANTAPAUSE_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Pause_Low_Threshold 
  * @{
  */ 
#define ETH_PAUSELOWTHRESHOLD_MINUS4        ((uint32_t)0x00000000)  /*!< Pause time minus 4 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS28       ((uint32_t)0x00000010)  /*!< Pause time minus 28 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS144      ((uint32_t)0x00000020)  /*!< Pause time minus 144 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS256      ((uint32_t)0x00000030)  /*!< Pause time minus 256 slot times */
#define IS_ETH_PAUSE_LOW_THRESHOLD(THRESHOLD) (((THRESHOLD) == ETH_PAUSELOWTHRESHOLD_MINUS4) || \
                                               ((THRESHOLD) == ETH_PAUSELOWTHRESHOLD_MINUS28) || \
                                               ((THRESHOLD) == ETH_PAUSELOWTHRESHOLD_MINUS144) || \
                                               ((THRESHOLD) == ETH_PAUSELOWTHRESHOLD_MINUS256))
/**
  * @}
  */

/** @defgroup ETH_Unicast_Pause_Frame_Detect 
  * @{
  */ 
#define ETH_UNICASTPAUSEFRAMEDETECT_ENABLE  ((uint32_t)0x00000008)
#define ETH_UNICASTPAUSEFRAMEDETECT_DISABLE ((uint32_t)0x00000000)
#define IS_ETH_UNICAST_PAUSE_FRAME_DETECT(CMD) (((CMD) == ETH_UNICASTPAUSEFRAMEDETECT_ENABLE) || \
                                                ((CMD) == ETH_UNICASTPAUSEFRAMEDETECT_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Receive_Flow_Control 
  * @{
  */ 
#define ETH_RECEIVEFLOWCONTROL_ENABLE       ((uint32_t)0x00000004)
#define ETH_RECEIVEFLOWCONTROL_DISABLE      ((uint32_t)0x00000000)
#define IS_ETH_RECEIVE_FLOWCONTROL(CMD) (((CMD) == ETH_RECEIVEFLOWCONTROL_ENABLE) || \
                                         ((CMD) == ETH_RECEIVEFLOWCONTROL_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Transmit_Flow_Control 
  * @{
  */ 
#define ETH_TRANSMITFLOWCONTROL_ENABLE      ((uint32_t)0x00000002)
#define ETH_TRANSMITFLOWCONTROL_DISABLE     ((uint32_t)0x00000000)
#define IS_ETH_TRANSMIT_FLOWCONTROL(CMD) (((CMD) == ETH_TRANSMITFLOWCONTROL_ENABLE) || \
                                          ((CMD) == ETH_TRANSMITFLOWCONTROL_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_VLAN_Tag_Comparison 
  * @{
  */ 
#define ETH_VLANTAGCOMPARISON_12BIT    ((uint32_t)0x00010000)
#define ETH_VLANTAGCOMPARISON_16BIT    ((uint32_t)0x00000000)
#define IS_ETH_VLAN_TAG_COMPARISON(COMPARISON) (((COMPARISON) == ETH_VLANTAGCOMPARISON_12BIT) || \
                                                ((COMPARISON) == ETH_VLANTAGCOMPARISON_16BIT))
#define IS_ETH_VLAN_TAG_IDENTIFIER(IDENTIFIER) ((IDENTIFIER) <= 0xFFFF)

/**
  * @}
  */

/** @defgroup ETH_MAC_addresses 
  * @{
  */ 
#define ETH_MAC_ADDRESS0     ((uint32_t)0x00000000)
#define ETH_MAC_ADDRESS1     ((uint32_t)0x00000008)
#define ETH_MAC_ADDRESS2     ((uint32_t)0x00000010)
#define ETH_MAC_ADDRESS3     ((uint32_t)0x00000018)
#define IS_ETH_MAC_ADDRESS0123(ADDRESS) (((ADDRESS) == ETH_MAC_ADDRESS0) || \
                                         ((ADDRESS) == ETH_MAC_ADDRESS1) || \
                                         ((ADDRESS) == ETH_MAC_ADDRESS2) || \
                                         ((ADDRESS) == ETH_MAC_ADDRESS3))
#define IS_ETH_MAC_ADDRESS123(ADDRESS) (((ADDRESS) == ETH_MAC_ADDRESS1) || \
                                        ((ADDRESS) == ETH_MAC_ADDRESS2) || \
                                        ((ADDRESS) == ETH_MAC_ADDRESS3))
/**
  * @}
  */

/** @defgroup ETH_MAC_addresses_filter_SA_DA_filed_of_received_frames 
  * @{
  */ 
#define ETH_MAC_ADDRESSFILTER_SA       ((uint32_t)0x00000000)
#define ETH_MAC_ADDRESSFILTER_DA       ((uint32_t)0x00000008)
#define IS_ETH_MAC_ADDRESS_FILTER(FILTER) (((FILTER) == ETH_MAC_ADDRESSFILTER_SA) || \
                                           ((FILTER) == ETH_MAC_ADDRESSFILTER_DA))
/**
  * @}
  */

/** @defgroup ETH_MAC_addresses_filter_Mask_bytes 
  * @{
  */ 
#define ETH_MAC_ADDRESSMASK_BYTE6      ((uint32_t)0x20000000)  /*!< Mask MAC Address high reg bits [15:8] */
#define ETH_MAC_ADDRESSMASK_BYTE5      ((uint32_t)0x10000000)  /*!< Mask MAC Address high reg bits [7:0] */
#define ETH_MAC_ADDRESSMASK_BYTE4      ((uint32_t)0x08000000)  /*!< Mask MAC Address low reg bits [31:24] */
#define ETH_MAC_ADDRESSMASK_BYTE3      ((uint32_t)0x04000000)  /*!< Mask MAC Address low reg bits [23:16] */
#define ETH_MAC_ADDRESSMASK_BYTE2      ((uint32_t)0x02000000)  /*!< Mask MAC Address low reg bits [15:8] */
#define ETH_MAC_ADDRESSMASK_BYTE1      ((uint32_t)0x01000000)  /*!< Mask MAC Address low reg bits [70] */
#define IS_ETH_MAC_ADDRESS_MASK(MASK) (((MASK) == ETH_MAC_ADDRESSMASK_BYTE6) || \
                                       ((MASK) == ETH_MAC_ADDRESSMASK_BYTE5) || \
                                       ((MASK) == ETH_MAC_ADDRESSMASK_BYTE4) || \
                                       ((MASK) == ETH_MAC_ADDRESSMASK_BYTE3) || \
                                       ((MASK) == ETH_MAC_ADDRESSMASK_BYTE2) || \
                                       ((MASK) == ETH_MAC_ADDRESSMASK_BYTE1))

/**
  * @}
  */

/** @defgroup ETH_MAC_Debug_flags
  * @{
  */ 
#define ETH_MAC_TXFIFO_FULL          ((uint32_t)0x02000000)  /* Tx FIFO full */

#define ETH_MAC_TXFIFONOT_EMPTY      ((uint32_t)0x01000000)  /* Tx FIFO not empty */

#define ETH_MAC_TXFIFO_WRITE_ACTIVE  ((uint32_t)0x00400000)  /* Tx FIFO write active */

#define ETH_MAC_TXFIFO_IDLE     ((uint32_t)0x00000000)  /* Tx FIFO read status: Idle */
#define ETH_MAC_TXFIFO_READ     ((uint32_t)0x00100000)  /* Tx FIFO read status: Read (transferring data to the MAC transmitter) */
#define ETH_MAC_TXFIFO_WAITING  ((uint32_t)0x00200000)  /* Tx FIFO read status: Waiting for TxStatus from MAC transmitter */
#define ETH_MAC_TXFIFO_WRITING  ((uint32_t)0x00300000)  /* Tx FIFO read status: Writing the received TxStatus or flushing the TxFIFO */

#define ETH_MAC_TRANSMISSION_PAUSE     ((uint32_t)0x00080000)  /* MAC transmitter in pause */

#define ETH_MAC_TRANSMITFRAMECONTROLLER_IDLE            ((uint32_t)0x00000000)  /* MAC transmit frame controller: Idle */
#define ETH_MAC_TRANSMITFRAMECONTROLLER_WAITING         ((uint32_t)0x00020000)  /* MAC transmit frame controller: Waiting for Status of previous frame or IFG/backoff period to be over */
#define ETH_MAC_TRANSMITFRAMECONTROLLER_GENRATING_PCF   ((uint32_t)0x00040000)  /* MAC transmit frame controller: Generating and transmitting a Pause control frame (in full duplex mode) */
#define ETH_MAC_TRANSMITFRAMECONTROLLER_TRANSFERRING    ((uint32_t)0x00060000)  /* MAC transmit frame controller: Transferring input frame for transmission */

#define ETH_MAC_MII_TRANSMIT_ACTIVE      ((uint32_t)0x00010000)  /* MAC MII transmit engine active */

#define ETH_MAC_RXFIFO_EMPTY             ((uint32_t)0x00000000)  /* Rx FIFO fill level: empty */
#define ETH_MAC_RXFIFO_BELOW_THRESHOLD   ((uint32_t)0x00000100)  /* Rx FIFO fill level: fill-level below flow-control de-activate threshold */
#define ETH_MAC_RXFIFO_ABOVE_THRESHOLD   ((uint32_t)0x00000200)  /* Rx FIFO fill level: fill-level above flow-control activate threshold */
#define ETH_MAC_RXFIFO_FULL              ((uint32_t)0x00000300)  /* Rx FIFO fill level: full */

#define ETH_MAC_READCONTROLLER_IDLE            ((uint32_t)0x00000060)  /* Rx FIFO read controller IDLE state */
#define ETH_MAC_READCONTROLLER_READING_DATA    ((uint32_t)0x00000060)  /* Rx FIFO read controller Reading frame data */
#define ETH_MAC_READCONTROLLER_READING_STATUS  ((uint32_t)0x00000060)  /* Rx FIFO read controller Reading frame status (or time-stamp) */
#define ETH_MAC_READCONTROLLER_ FLUSHING       ((uint32_t)0x00000060)  /* Rx FIFO read controller Flushing the frame data and status */

#define ETH_MAC_RXFIFO_WRITE_ACTIVE     ((uint32_t)0x00000010)  /* Rx FIFO write controller active */

#define ETH_MAC_SMALL_FIFO_NOTACTIVE    ((uint32_t)0x00000000)  /* MAC small FIFO read / write controllers not active */
#define ETH_MAC_SMALL_FIFO_READ_ACTIVE  ((uint32_t)0x00000002)  /* MAC small FIFO read controller active */
#define ETH_MAC_SMALL_FIFO_WRITE_ACTIVE ((uint32_t)0x00000004)  /* MAC small FIFO write controller active */
#define ETH_MAC_SMALL_FIFO_RW_ACTIVE    ((uint32_t)0x00000006)  /* MAC small FIFO read / write controllers active */

#define ETH_MAC_MII_RECEIVE_PROTOCOL_AVTIVE   ((uint32_t)0x00000001)  /* MAC MII receive protocol engine active */

/**
  * @}
  */

/** @defgroup ETH_Drop_TCP_IP_Checksum_Error_Frame 
  * @{
  */ 
#define ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE   ((uint32_t)0x00000000)
#define ETH_DROPTCPIPCHECKSUMERRORFRAME_DISABLE  ((uint32_t)0x04000000)
#define IS_ETH_DROP_TCPIP_CHECKSUM_FRAME(CMD) (((CMD) == ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE) || \
                                               ((CMD) == ETH_DROPTCPIPCHECKSUMERRORFRAME_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Receive_Store_Forward 
  * @{
  */ 
#define ETH_RECEIVESTOREFORWARD_ENABLE      ((uint32_t)0x02000000)
#define ETH_RECEIVESTOREFORWARD_DISABLE     ((uint32_t)0x00000000)
#define IS_ETH_RECEIVE_STORE_FORWARD(CMD) (((CMD) == ETH_RECEIVESTOREFORWARD_ENABLE) || \
                                           ((CMD) == ETH_RECEIVESTOREFORWARD_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Flush_Received_Frame 
  * @{
  */ 
#define ETH_FLUSHRECEIVEDFRAME_ENABLE       ((uint32_t)0x00000000)
#define ETH_FLUSHRECEIVEDFRAME_DISABLE      ((uint32_t)0x01000000)
#define IS_ETH_FLUSH_RECEIVE_FRAME(CMD) (((CMD) == ETH_FLUSHRECEIVEDFRAME_ENABLE) || \
                                         ((CMD) == ETH_FLUSHRECEIVEDFRAME_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Transmit_Store_Forward 
  * @{
  */ 
#define ETH_TRANSMITSTOREFORWARD_ENABLE     ((uint32_t)0x00200000)
#define ETH_TRANSMITSTOREFORWARD_DISABLE    ((uint32_t)0x00000000)
#define IS_ETH_TRANSMIT_STORE_FORWARD(CMD) (((CMD) == ETH_TRANSMITSTOREFORWARD_ENABLE) || \
                                            ((CMD) == ETH_TRANSMITSTOREFORWARD_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Transmit_Threshold_Control 
  * @{
  */ 
#define ETH_TRANSMITTHRESHOLDCONTROL_64BYTES     ((uint32_t)0x00000000)  /*!< threshold level of the MTL Transmit FIFO is 64 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_128BYTES    ((uint32_t)0x00004000)  /*!< threshold level of the MTL Transmit FIFO is 128 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_192BYTES    ((uint32_t)0x00008000)  /*!< threshold level of the MTL Transmit FIFO is 192 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_256BYTES    ((uint32_t)0x0000C000)  /*!< threshold level of the MTL Transmit FIFO is 256 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_40BYTES     ((uint32_t)0x00010000)  /*!< threshold level of the MTL Transmit FIFO is 40 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_32BYTES     ((uint32_t)0x00014000)  /*!< threshold level of the MTL Transmit FIFO is 32 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_24BYTES     ((uint32_t)0x00018000)  /*!< threshold level of the MTL Transmit FIFO is 24 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_16BYTES     ((uint32_t)0x0001C000)  /*!< threshold level of the MTL Transmit FIFO is 16 Bytes */
#define IS_ETH_TRANSMIT_THRESHOLD_CONTROL(THRESHOLD) (((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_64BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_128BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_192BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_256BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_40BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_32BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_24BYTES) || \
                                                      ((THRESHOLD) == ETH_TRANSMITTHRESHOLDCONTROL_16BYTES))
/**
  * @}
  */

/** @defgroup ETH_Forward_Error_Frames 
  * @{
  */ 
#define ETH_FORWARDERRORFRAMES_ENABLE       ((uint32_t)0x00000080)
#define ETH_FORWARDERRORFRAMES_DISABLE      ((uint32_t)0x00000000)
#define IS_ETH_FORWARD_ERROR_FRAMES(CMD) (((CMD) == ETH_FORWARDERRORFRAMES_ENABLE) || \
                                          ((CMD) == ETH_FORWARDERRORFRAMES_DISABLE))
/**
  * @}
  */

/** @defgroup ETH_Forward_Undersized_Good_Frames 
  * @{
  */ 
#define ETH_FORWARDUNDERSIZEDGOODFRAMES_ENABLE   ((uint32_t)0x00000040)
#define ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE  ((uint32_t)0x00000000)     
#define IS_ETH_FORWARD_UNDERSIZED_GOOD_FRAMES(CMD) (((CMD) == ETH_FORWARDUNDERSIZEDGOODFRAMES_ENABLE) || \
                                                    ((CMD) == ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Receive_Threshold_Control 
  * @{
  */ 
#define ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES      ((uint32_t)0x00000000)  /*!< threshold level of the MTL Receive FIFO is 64 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_32BYTES      ((uint32_t)0x00000008)  /*!< threshold level of the MTL Receive FIFO is 32 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_96BYTES      ((uint32_t)0x00000010)  /*!< threshold level of the MTL Receive FIFO is 96 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_128BYTES     ((uint32_t)0x00000018)  /*!< threshold level of the MTL Receive FIFO is 128 Bytes */
#define IS_ETH_RECEIVE_THRESHOLD_CONTROL(THRESHOLD) (((THRESHOLD) == ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES) || \
                                                     ((THRESHOLD) == ETH_RECEIVEDTHRESHOLDCONTROL_32BYTES) || \
                                                     ((THRESHOLD) == ETH_RECEIVEDTHRESHOLDCONTROL_96BYTES) || \
                                                     ((THRESHOLD) == ETH_RECEIVEDTHRESHOLDCONTROL_128BYTES))
/**
  * @}
  */

/** @defgroup ETH_Second_Frame_Operate 
  * @{
  */ 
#define ETH_SECONDFRAMEOPERARTE_ENABLE       ((uint32_t)0x00000004)
#define ETH_SECONDFRAMEOPERARTE_DISABLE      ((uint32_t)0x00000000)  
#define IS_ETH_SECOND_FRAME_OPERATE(CMD) (((CMD) == ETH_SECONDFRAMEOPERARTE_ENABLE) || \
                                          ((CMD) == ETH_SECONDFRAMEOPERARTE_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Address_Aligned_Beats 
  * @{
  */ 
#define ETH_ADDRESSALIGNEDBEATS_ENABLE      ((uint32_t)0x02000000)
#define ETH_ADDRESSALIGNEDBEATS_DISABLE     ((uint32_t)0x00000000) 
#define IS_ETH_ADDRESS_ALIGNED_BEATS(CMD) (((CMD) == ETH_ADDRESSALIGNEDBEATS_ENABLE) || \
                                           ((CMD) == ETH_ADDRESSALIGNEDBEATS_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Fixed_Burst 
  * @{
  */ 
#define ETH_FIXEDBURST_ENABLE     ((uint32_t)0x00010000)
#define ETH_FIXEDBURST_DISABLE    ((uint32_t)0x00000000) 
#define IS_ETH_FIXED_BURST(CMD) (((CMD) == ETH_FIXEDBURST_ENABLE) || \
                                 ((CMD) == ETH_FIXEDBURST_DISABLE))

/**
  * @}
  */

/** @defgroup ETH_Rx_DMA_Burst_Length 
  * @{
  */ 
#define ETH_RXDMABURSTLENGTH_1BEAT          ((uint32_t)0x00020000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 1 */
#define ETH_RXDMABURSTLENGTH_2BEAT          ((uint32_t)0x00040000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 2 */
#define ETH_RXDMABURSTLENGTH_4BEAT          ((uint32_t)0x00080000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RXDMABURSTLENGTH_8BEAT          ((uint32_t)0x00100000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RXDMABURSTLENGTH_16BEAT         ((uint32_t)0x00200000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RXDMABURSTLENGTH_32BEAT         ((uint32_t)0x00400000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 32 */                
#define ETH_RXDMABURSTLENGTH_4XPBL_4BEAT    ((uint32_t)0x01020000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RXDMABURSTLENGTH_4XPBL_8BEAT    ((uint32_t)0x01040000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RXDMABURSTLENGTH_4XPBL_16BEAT   ((uint32_t)0x01080000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RXDMABURSTLENGTH_4XPBL_32BEAT   ((uint32_t)0x01100000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_RXDMABURSTLENGTH_4XPBL_64BEAT   ((uint32_t)0x01200000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 64 */
#define ETH_RXDMABURSTLENGTH_4XPBL_128BEAT  ((uint32_t)0x01400000)  /*!< maximum number of beats to be transferred in one RxDMA transaction is 128 */

#define IS_ETH_RXDMA_BURST_LENGTH(LENGTH) (((LENGTH) == ETH_RXDMABURSTLENGTH_1BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_2BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_8BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_16BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_32BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_4BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_8BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_16BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_32BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_64BEAT) || \
                                           ((LENGTH) == ETH_RXDMABURSTLENGTH_4XPBL_128BEAT))
 
/**
  * @}
  */

/** @defgroup ETH_Tx_DMA_Burst_Length 
  * @{
  */ 
#define ETH_TXDMABURSTLENGTH_1BEAT          ((uint32_t)0x00000100)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
#define ETH_TXDMABURSTLENGTH_2BEAT          ((uint32_t)0x00000200)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
#define ETH_TXDMABURSTLENGTH_4BEAT          ((uint32_t)0x00000400)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TXDMABURSTLENGTH_8BEAT          ((uint32_t)0x00000800)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TXDMABURSTLENGTH_16BEAT         ((uint32_t)0x00001000)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TXDMABURSTLENGTH_32BEAT         ((uint32_t)0x00002000)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */                
#define ETH_TXDMABURSTLENGTH_4XPBL_4BEAT    ((uint32_t)0x01000100)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TXDMABURSTLENGTH_4XPBL_8BEAT    ((uint32_t)0x01000200)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TXDMABURSTLENGTH_4XPBL_16BEAT   ((uint32_t)0x01000400)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TXDMABURSTLENGTH_4XPBL_32BEAT   ((uint32_t)0x01000800)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_TXDMABURSTLENGTH_4XPBL_64BEAT   ((uint32_t)0x01001000)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
#define ETH_TXDMABURSTLENGTH_4XPBL_128BEAT  ((uint32_t)0x01002000)  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */

#define IS_ETH_TXDMA_BURST_LENGTH(LENGTH) (((LENGTH) == ETH_TXDMABURSTLENGTH_1BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_2BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_8BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_16BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_32BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_4BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_8BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_16BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_32BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_64BEAT) || \
                                           ((LENGTH) == ETH_TXDMABURSTLENGTH_4XPBL_128BEAT))

/** @defgroup ETH_DMA_Enhanced_descriptor_format 
  * @{
  */  
#define ETH_DMAENHANCEDDESCRIPTOR_ENABLE              ((uint32_t)0x00000080)
#define ETH_DMAENHANCEDDESCRIPTOR_DISABLE             ((uint32_t)0x00000000)

#define IS_ETH_ENHANCED_DESCRIPTOR_FORMAT(CMD) (((CMD) == ETH_DMAENHANCEDDESCRIPTOR_ENABLE) || \
                                                ((CMD) == ETH_DMAENHANCEDDESCRIPTOR_DISABLE))

/**
  * @}
  */

/** 
  * @brief  ETH DMA Descriptor SkipLength  
  */ 
#define IS_ETH_DMA_DESC_SKIP_LENGTH(LENGTH) ((LENGTH) <= 0x1F)


/** @defgroup ETH_DMA_Arbitration 
  * @{
  */ 
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1   ((uint32_t)0x00000000)
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_2_1   ((uint32_t)0x00004000)
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_3_1   ((uint32_t)0x00008000)
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_4_1   ((uint32_t)0x0000C000)
#define ETH_DMAARBITRATION_RXPRIORTX             ((uint32_t)0x00000002)
#define IS_ETH_DMA_ARBITRATION_ROUNDROBIN_RXTX(RATIO) (((RATIO) == ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1) || \
                                                       ((RATIO) == ETH_DMAARBITRATION_ROUNDROBIN_RXTX_2_1) || \
                                                       ((RATIO) == ETH_DMAARBITRATION_ROUNDROBIN_RXTX_3_1) || \
                                                       ((RATIO) == ETH_DMAARBITRATION_ROUNDROBIN_RXTX_4_1) || \
                                                       ((RATIO) == ETH_DMAARBITRATION_RXPRIORTX))
/**
  * @}
  */

/** @defgroup ETH_DMA_Tx_descriptor_flags
  * @{
  */ 
#define IS_ETH_DMATXDESC_GET_FLAG(FLAG) (((FLAG) == ETH_DMATXDESC_OWN) || \
                                         ((FLAG) == ETH_DMATXDESC_IC) || \
                                         ((FLAG) == ETH_DMATXDESC_LS) || \
                                         ((FLAG) == ETH_DMATXDESC_FS) || \
                                         ((FLAG) == ETH_DMATXDESC_DC) || \
                                         ((FLAG) == ETH_DMATXDESC_DP) || \
                                         ((FLAG) == ETH_DMATXDESC_TTSE) || \
                                         ((FLAG) == ETH_DMATXDESC_TER) || \
                                         ((FLAG) == ETH_DMATXDESC_TCH) || \
                                         ((FLAG) == ETH_DMATXDESC_TTSS) || \
                                         ((FLAG) == ETH_DMATXDESC_IHE) || \
                                         ((FLAG) == ETH_DMATXDESC_ES) || \
                                         ((FLAG) == ETH_DMATXDESC_JT) || \
                                         ((FLAG) == ETH_DMATXDESC_FF) || \
                                         ((FLAG) == ETH_DMATXDESC_PCE) || \
                                         ((FLAG) == ETH_DMATXDESC_LCA) || \
                                         ((FLAG) == ETH_DMATXDESC_NC) || \
                                         ((FLAG) == ETH_DMATXDESC_LCO) || \
                                         ((FLAG) == ETH_DMATXDESC_EC) || \
                                         ((FLAG) == ETH_DMATXDESC_VF) || \
                                         ((FLAG) == ETH_DMATXDESC_CC) || \
                                         ((FLAG) == ETH_DMATXDESC_ED) || \
                                         ((FLAG) == ETH_DMATXDESC_UF) || \
                                         ((FLAG) == ETH_DMATXDESC_DB))

/**
  * @}
  */

/** @defgroup ETH_DMA_Tx_descriptor_segment 
  * @{
  */ 
#define ETH_DMATXDESC_LASTSEGMENTS      ((uint32_t)0x40000000)  /*!< Last Segment */
#define ETH_DMATXDESC_FIRSTSEGMENT      ((uint32_t)0x20000000)  /*!< First Segment */
#define IS_ETH_DMA_TXDESC_SEGMENT(SEGMENT) (((SEGMENT) == ETH_DMATXDESC_LASTSEGMENTS) || \
                                            ((SEGMENT) == ETH_DMATXDESC_FIRSTSEGMENT))

/**
  * @}
  */

/** @defgroup ETH_DMA_Tx_descriptor_Checksum_Insertion_Control
  * @{
  */ 
#define ETH_DMATXDESC_CHECKSUMBYPASS             ((uint32_t)0x00000000)   /*!< Checksum engine bypass */
#define ETH_DMATXDESC_CHECKSUMIPV4HEADER         ((uint32_t)0x00400000)   /*!< IPv4 header checksum insertion  */
#define ETH_DMATXDESC_CHECKSUMTCPUDPICMPSEGMENT  ((uint32_t)0x00800000)   /*!< TCP/UDP/ICMP checksum insertion. Pseudo header checksum is assumed to be present */
#define ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL     ((uint32_t)0x00C00000)   /*!< TCP/UDP/ICMP checksum fully in hardware including pseudo header */
#define IS_ETH_DMA_TXDESC_CHECKSUM(CHECKSUM) (((CHECKSUM) == ETH_DMATXDESC_CHECKSUMBYPASS) || \
                                              ((CHECKSUM) == ETH_DMATXDESC_CHECKSUMIPV4HEADER) || \
                                              ((CHECKSUM) == ETH_DMATXDESC_CHECKSUMTCPUDPICMPSEGMENT) || \
                                              ((CHECKSUM) == ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL))
/** 
  * @brief  ETH DMA Tx Desciptor buffer size
  */ 
#define IS_ETH_DMATXDESC_BUFFER_SIZE(SIZE) ((SIZE) <= 0x1FFF)

/**
  * @}
  */

/** @defgroup ETH_DMA_Rx_descriptor_flags
  * @{
  */ 
#define IS_ETH_DMARXDESC_GET_FLAG(FLAG) (((FLAG) == ETH_DMARXDESC_OWN) || \
                                         ((FLAG) == ETH_DMARXDESC_AFM) || \
                                         ((FLAG) == ETH_DMARXDESC_ES) || \
                                         ((FLAG) == ETH_DMARXDESC_DE) || \
                                         ((FLAG) == ETH_DMARXDESC_SAF) || \
                                         ((FLAG) == ETH_DMARXDESC_LE) || \
                                         ((FLAG) == ETH_DMARXDESC_OE) || \
                                         ((FLAG) == ETH_DMARXDESC_VLAN) || \
                                         ((FLAG) == ETH_DMARXDESC_FS) || \
                                         ((FLAG) == ETH_DMARXDESC_LS) || \
                                         ((FLAG) == ETH_DMARXDESC_IPV4HCE) || \
                                         ((FLAG) == ETH_DMARXDESC_LC) || \
                                         ((FLAG) == ETH_DMARXDESC_FT) || \
                                         ((FLAG) == ETH_DMARXDESC_RWT) || \
                                         ((FLAG) == ETH_DMARXDESC_RE) || \
                                         ((FLAG) == ETH_DMARXDESC_DBE) || \
                                         ((FLAG) == ETH_DMARXDESC_CE) || \
                                         ((FLAG) == ETH_DMARXDESC_MAMPCE))

/* ETHERNET DMA PTP Rx descriptor extended flags  --------------------------------*/
#define IS_ETH_DMAPTPRXDESC_GET_EXTENDED_FLAG(FLAG) (((FLAG) == ETH_DMAPTPRXDESC_PTPV) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_PTPFT) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_PTPMT) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPV6PR) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPV4PR) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPCB) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPPE) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPHE) || \
                                                     ((FLAG) == ETH_DMAPTPRXDESC_IPPT))

/**
  * @}
  */

/** @defgroup ETH_DMA_Rx_descriptor_buffers_ 
  * @{
  */ 
#define ETH_DMARXDESC_BUFFER1     ((uint32_t)0x00000000)  /*!< DMA Rx Desc Buffer1 */
#define ETH_DMARXDESC_BUFFER2     ((uint32_t)0x00000001)  /*!< DMA Rx Desc Buffer2 */
#define IS_ETH_DMA_RXDESC_BUFFER(BUFFER) (((BUFFER) == ETH_DMARXDESC_BUFFER1) || \
                                          ((BUFFER) == ETH_DMARXDESC_BUFFER2))


/* ETHERNET DMA Tx descriptors Collision Count Shift */
#define  ETH_DMATXDESC_COLLISION_COUNTSHIFT         ((uint32_t)3)

/* ETHERNET DMA Tx descriptors Buffer2 Size Shift */
#define  ETH_DMATXDESC_BUFFER2_SIZESHIFT           ((uint32_t)16)

/* ETHERNET DMA Rx descriptors Frame Length Shift */
#define  ETH_DMARXDESC_FRAME_LENGTHSHIFT           ((uint32_t)16)

/* ETHERNET DMA Rx descriptors Buffer2 Size Shift */
#define  ETH_DMARXDESC_BUFFER2_SIZESHIFT           ((uint32_t)16)

/* ETHERNET DMA Rx descriptors Frame length Shift */
#define  ETH_DMARXDESC_FRAMELENGTHSHIFT            ((uint32_t)16)
 
/**
  * @}
  */

/** @defgroup ETH_PMT_Flags 
  * @{
  */ 
#define ETH_PMT_FLAG_WUFFRPR      ((uint32_t)0x80000000)  /*!< Wake-Up Frame Filter Register Pointer Reset */
#define ETH_PMT_FLAG_WUFR         ((uint32_t)0x00000040)  /*!< Wake-Up Frame Received */
#define ETH_PMT_FLAG_MPR          ((uint32_t)0x00000020)  /*!< Magic Packet Received */
#define IS_ETH_PMT_GET_FLAG(FLAG) (((FLAG) == ETH_PMT_FLAG_WUFR) || \
                                   ((FLAG) == ETH_PMT_FLAG_MPR))
/**
  * @}
  */

/** @defgroup ETH_MMC_Tx_Interrupts 
  * @{
  */ 
#define ETH_MMC_IT_TGF       ((uint32_t)0x00200000)  /*!< When Tx good frame counter reaches half the maximum value */
#define ETH_MMC_IT_TGFMSC    ((uint32_t)0x00008000)  /*!< When Tx good multi col counter reaches half the maximum value */
#define ETH_MMC_IT_TGFSC     ((uint32_t)0x00004000)  /*!< When Tx good single col counter reaches half the maximum value */

/**
  * @}
  */

/** @defgroup ETH_MMC_Rx_Interrupts 
  * @{
  */
#define ETH_MMC_IT_RGUF      ((uint32_t)0x10020000)  /*!< When Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMC_IT_RFAE      ((uint32_t)0x10000040)  /*!< When Rx alignment error counter reaches half the maximum value */
#define ETH_MMC_IT_RFCE      ((uint32_t)0x10000020)  /*!< When Rx crc error counter reaches half the maximum value */
#define IS_ETH_MMC_IT(IT) (((((IT) & (uint32_t)0xFFDF3FFF) == 0x00) || (((IT) & (uint32_t)0xEFFDFF9F) == 0x00)) && \
                           ((IT) != 0x00))
#define IS_ETH_MMC_GET_IT(IT) (((IT) == ETH_MMC_IT_TGF) || ((IT) == ETH_MMC_IT_TGFMSC) || \
                               ((IT) == ETH_MMC_IT_TGFSC) || ((IT) == ETH_MMC_IT_RGUF) || \
                               ((IT) == ETH_MMC_IT_RFAE) || ((IT) == ETH_MMC_IT_RFCE))
/**
  * @}
  */

/** @defgroup ETH_MMC_Registers 
  * @{
  */ 
#define ETH_MMCCR            ((uint32_t)0x00000100)  /*!< MMC CR register */
#define ETH_MMCRIR           ((uint32_t)0x00000104)  /*!< MMC RIR register */
#define ETH_MMCTIR           ((uint32_t)0x00000108)  /*!< MMC TIR register */
#define ETH_MMCRIMR          ((uint32_t)0x0000010C)  /*!< MMC RIMR register */
#define ETH_MMCTIMR          ((uint32_t)0x00000110)  /*!< MMC TIMR register */ 
#define ETH_MMCTGFSCCR       ((uint32_t)0x0000014C)  /*!< MMC TGFSCCR register */
#define ETH_MMCTGFMSCCR      ((uint32_t)0x00000150)  /*!< MMC TGFMSCCR register */ 
#define ETH_MMCTGFCR         ((uint32_t)0x00000168)  /*!< MMC TGFCR register */
#define ETH_MMCRFCECR        ((uint32_t)0x00000194)  /*!< MMC RFCECR register */
#define ETH_MMCRFAECR        ((uint32_t)0x00000198)  /*!< MMC RFAECR register */
#define ETH_MMCRGUFCR        ((uint32_t)0x000001C4)  /*!< MMC RGUFCR register */

/** 
  * @brief  ETH MMC registers  
  */ 
#define IS_ETH_MMC_REGISTER(REG) (((REG) == ETH_MMCCR)  || ((REG) == ETH_MMCRIR) || \
                                  ((REG) == ETH_MMCTIR)  || ((REG) == ETH_MMCRIMR) || \
                                  ((REG) == ETH_MMCTIMR) || ((REG) == ETH_MMCTGFSCCR) || \
                                  ((REG) == ETH_MMCTGFMSCCR) || ((REG) == ETH_MMCTGFCR) || \
                                  ((REG) == ETH_MMCRFCECR) || ((REG) == ETH_MMCRFAECR) || \
                                  ((REG) == ETH_MMCRGUFCR)) 
/**
  * @}
  */

/** @defgroup ETH_MAC_Flags 
  * @{
  */ 
#define ETH_MAC_FLAG_TST     ((uint32_t)0x00000200)  /*!< Time stamp trigger flag (on MAC) */
#define ETH_MAC_FLAG_MMCT    ((uint32_t)0x00000040)  /*!< MMC transmit flag  */
#define ETH_MAC_FLAG_MMCR    ((uint32_t)0x00000020)  /*!< MMC receive flag */
#define ETH_MAC_FLAG_MMC     ((uint32_t)0x00000010)  /*!< MMC flag (on MAC) */
#define ETH_MAC_FLAG_PMT     ((uint32_t)0x00000008)  /*!< PMT flag (on MAC) */
#define IS_ETH_MAC_GET_FLAG(FLAG) (((FLAG) == ETH_MAC_FLAG_TST) || ((FLAG) == ETH_MAC_FLAG_MMCT) || \
                                   ((FLAG) == ETH_MAC_FLAG_MMCR) || ((FLAG) == ETH_MAC_FLAG_MMC) || \
                                   ((FLAG) == ETH_MAC_FLAG_PMT))
/**
  * @}
  */

/** @defgroup ETH_DMA_Flags 
  * @{
  */ 
#define ETH_DMA_FLAG_TST               ((uint32_t)0x20000000)  /*!< Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_FLAG_PMT               ((uint32_t)0x10000000)  /*!< PMT interrupt (on DMA) */
#define ETH_DMA_FLAG_MMC               ((uint32_t)0x08000000)  /*!< MMC interrupt (on DMA) */
#define ETH_DMA_FLAG_DATATRANSFERERROR ((uint32_t)0x00800000)  /*!< Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMA_FLAG_READWRITEERROR    ((uint32_t)0x01000000)  /*!< Error bits 0-write trnsf, 1-read transfr */
#define ETH_DMA_FLAG_ACCESSERROR       ((uint32_t)0x02000000)  /*!< Error bits 0-data buffer, 1-desc. access */
#define ETH_DMA_FLAG_NIS               ((uint32_t)0x00010000)  /*!< Normal interrupt summary flag */
#define ETH_DMA_FLAG_AIS               ((uint32_t)0x00008000)  /*!< Abnormal interrupt summary flag */
#define ETH_DMA_FLAG_ER                ((uint32_t)0x00004000)  /*!< Early receive flag */
#define ETH_DMA_FLAG_FBE               ((uint32_t)0x00002000)  /*!< Fatal bus error flag */
#define ETH_DMA_FLAG_ET                ((uint32_t)0x00000400)  /*!< Early transmit flag */
#define ETH_DMA_FLAG_RWT               ((uint32_t)0x00000200)  /*!< Receive watchdog timeout flag */
#define ETH_DMA_FLAG_RPS               ((uint32_t)0x00000100)  /*!< Receive process stopped flag */
#define ETH_DMA_FLAG_RBU               ((uint32_t)0x00000080)  /*!< Receive buffer unavailable flag */
#define ETH_DMA_FLAG_R                 ((uint32_t)0x00000040)  /*!< Receive flag */
#define ETH_DMA_FLAG_TU                ((uint32_t)0x00000020)  /*!< Underflow flag */
#define ETH_DMA_FLAG_RO                ((uint32_t)0x00000010)  /*!< Overflow flag */
#define ETH_DMA_FLAG_TJT               ((uint32_t)0x00000008)  /*!< Transmit jabber timeout flag */
#define ETH_DMA_FLAG_TBU               ((uint32_t)0x00000004)  /*!< Transmit buffer unavailable flag */
#define ETH_DMA_FLAG_TPS               ((uint32_t)0x00000002)  /*!< Transmit process stopped flag */
#define ETH_DMA_FLAG_T                 ((uint32_t)0x00000001)  /*!< Transmit flag */

#define IS_ETH_DMA_FLAG(FLAG) ((((FLAG) & (uint32_t)0xC7FE1800) == 0x00) && ((FLAG) != 0x00)) 
#define IS_ETH_DMA_GET_FLAG(FLAG) (((FLAG) == ETH_DMA_FLAG_TST) || ((FLAG) == ETH_DMA_FLAG_PMT) || \
                                   ((FLAG) == ETH_DMA_FLAG_MMC) || ((FLAG) == ETH_DMA_FLAG_DATATRANSFERERROR) || \
                                   ((FLAG) == ETH_DMA_FLAG_READWRITEERROR) || ((FLAG) == ETH_DMA_FLAG_ACCESSERROR) || \
                                   ((FLAG) == ETH_DMA_FLAG_NIS) || ((FLAG) == ETH_DMA_FLAG_AIS) || \
                                   ((FLAG) == ETH_DMA_FLAG_ER) || ((FLAG) == ETH_DMA_FLAG_FBE) || \
                                   ((FLAG) == ETH_DMA_FLAG_ET) || ((FLAG) == ETH_DMA_FLAG_RWT) || \
                                   ((FLAG) == ETH_DMA_FLAG_RPS) || ((FLAG) == ETH_DMA_FLAG_RBU) || \
                                   ((FLAG) == ETH_DMA_FLAG_R) || ((FLAG) == ETH_DMA_FLAG_TU) || \
                                   ((FLAG) == ETH_DMA_FLAG_RO) || ((FLAG) == ETH_DMA_FLAG_TJT) || \
                                   ((FLAG) == ETH_DMA_FLAG_TBU) || ((FLAG) == ETH_DMA_FLAG_TPS) || \
                                   ((FLAG) == ETH_DMA_FLAG_T))
/**
  * @}
  */

/** @defgroup ETH_MAC_Interrupts 
  * @{
  */ 
#define ETH_MAC_IT_TST       ((uint32_t)0x00000200)  /*!< Time stamp trigger interrupt (on MAC) */
#define ETH_MAC_IT_MMCT      ((uint32_t)0x00000040)  /*!< MMC transmit interrupt */
#define ETH_MAC_IT_MMCR      ((uint32_t)0x00000020)  /*!< MMC receive interrupt */
#define ETH_MAC_IT_MMC       ((uint32_t)0x00000010)  /*!< MMC interrupt (on MAC) */
#define ETH_MAC_IT_PMT       ((uint32_t)0x00000008)  /*!< PMT interrupt (on MAC) */
#define IS_ETH_MAC_IT(IT) ((((IT) & (uint32_t)0xFFFFFDF7) == 0x00) && ((IT) != 0x00))
#define IS_ETH_MAC_GET_IT(IT) (((IT) == ETH_MAC_IT_TST) || ((IT) == ETH_MAC_IT_MMCT) || \
                               ((IT) == ETH_MAC_IT_MMCR) || ((IT) == ETH_MAC_IT_MMC) || \
                               ((IT) == ETH_MAC_IT_PMT))
/**
  * @}
  */

/** @defgroup ETH_DMA_Interrupts 
  * @{
  */ 
#define ETH_DMA_IT_TST       ((uint32_t)0x20000000)  /*!< Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_IT_PMT       ((uint32_t)0x10000000)  /*!< PMT interrupt (on DMA) */
#define ETH_DMA_IT_MMC       ((uint32_t)0x08000000)  /*!< MMC interrupt (on DMA) */
#define ETH_DMA_IT_NIS       ((uint32_t)0x00010000)  /*!< Normal interrupt summary */
#define ETH_DMA_IT_AIS       ((uint32_t)0x00008000)  /*!< Abnormal interrupt summary */
#define ETH_DMA_IT_ER        ((uint32_t)0x00004000)  /*!< Early receive interrupt */
#define ETH_DMA_IT_FBE       ((uint32_t)0x00002000)  /*!< Fatal bus error interrupt */
#define ETH_DMA_IT_ET        ((uint32_t)0x00000400)  /*!< Early transmit interrupt */
#define ETH_DMA_IT_RWT       ((uint32_t)0x00000200)  /*!< Receive watchdog timeout interrupt */
#define ETH_DMA_IT_RPS       ((uint32_t)0x00000100)  /*!< Receive process stopped interrupt */
#define ETH_DMA_IT_RBU       ((uint32_t)0x00000080)  /*!< Receive buffer unavailable interrupt */
#define ETH_DMA_IT_R         ((uint32_t)0x00000040)  /*!< Receive interrupt */
#define ETH_DMA_IT_TU        ((uint32_t)0x00000020)  /*!< Underflow interrupt */
#define ETH_DMA_IT_RO        ((uint32_t)0x00000010)  /*!< Overflow interrupt */
#define ETH_DMA_IT_TJT       ((uint32_t)0x00000008)  /*!< Transmit jabber timeout interrupt */
#define ETH_DMA_IT_TBU       ((uint32_t)0x00000004)  /*!< Transmit buffer unavailable interrupt */
#define ETH_DMA_IT_TPS       ((uint32_t)0x00000002)  /*!< Transmit process stopped interrupt */
#define ETH_DMA_IT_T         ((uint32_t)0x00000001)  /*!< Transmit interrupt */

#define IS_ETH_DMA_IT(IT) ((((IT) & (uint32_t)0xC7FE1800) == 0x00) && ((IT) != 0x00))
#define IS_ETH_DMA_GET_IT(IT) (((IT) == ETH_DMA_IT_TST) || ((IT) == ETH_DMA_IT_PMT) || \
                               ((IT) == ETH_DMA_IT_MMC) || ((IT) == ETH_DMA_IT_NIS) || \
                               ((IT) == ETH_DMA_IT_AIS) || ((IT) == ETH_DMA_IT_ER) || \
                               ((IT) == ETH_DMA_IT_FBE) || ((IT) == ETH_DMA_IT_ET) || \
                               ((IT) == ETH_DMA_IT_RWT) || ((IT) == ETH_DMA_IT_RPS) || \
                               ((IT) == ETH_DMA_IT_RBU) || ((IT) == ETH_DMA_IT_R) || \
                               ((IT) == ETH_DMA_IT_TU) || ((IT) == ETH_DMA_IT_RO) || \
                               ((IT) == ETH_DMA_IT_TJT) || ((IT) == ETH_DMA_IT_TBU) || \
                               ((IT) == ETH_DMA_IT_TPS) || ((IT) == ETH_DMA_IT_T))

/**
  * @}
  */

/** @defgroup ETH_DMA_transmit_process_state_ 
  * @{
  */ 
#define ETH_DMA_TRANSMITPROCESS_STOPPED     ((uint32_t)0x00000000)  /*!< Stopped - Reset or Stop Tx Command issued */
#define ETH_DMA_TRANSMITPROCESS_FETCHING    ((uint32_t)0x00100000)  /*!< Running - fetching the Tx descriptor */
#define ETH_DMA_TRANSMITPROCESS_WAITING     ((uint32_t)0x00200000)  /*!< Running - waiting for status */
#define ETH_DMA_TRANSMITPROCESS_READING     ((uint32_t)0x00300000)  /*!< Running - reading the data from host memory */
#define ETH_DMA_TRANSMITPROCESS_SUSPENDED   ((uint32_t)0x00600000)  /*!< Suspended - Tx Descriptor unavailable */
#define ETH_DMA_TRANSMITPROCESS_CLOSING     ((uint32_t)0x00700000)  /*!< Running - closing Rx descriptor */

/**
  * @}
  */ 


/** @defgroup ETH_DMA_receive_process_state_ 
  * @{
  */ 
#define ETH_DMA_RECEIVEPROCESS_STOPPED      ((uint32_t)0x00000000)  /*!< Stopped - Reset or Stop Rx Command issued */
#define ETH_DMA_RECEIVEPROCESS_FETCHING     ((uint32_t)0x00020000)  /*!< Running - fetching the Rx descriptor */
#define ETH_DMA_RECEIVEPROCESS_WAITING      ((uint32_t)0x00060000)  /*!< Running - waiting for packet */
#define ETH_DMA_RECEIVEPROCESS_SUSPENDED    ((uint32_t)0x00080000)  /*!< Suspended - Rx Descriptor unavailable */
#define ETH_DMA_RECEIVEPROCESS_CLOSING      ((uint32_t)0x000A0000)  /*!< Running - closing descriptor */
#define ETH_DMA_RECEIVEPROCESS_QUEUING      ((uint32_t)0x000E0000)  /*!< Running - queuing the receive frame into host memory */

/**
  * @}
  */

/** @defgroup ETH_DMA_overflow_ 
  * @{
  */ 
#define ETH_DMA_OVERFLOW_RXFIFOCOUNTER      ((uint32_t)0x10000000)  /*!< Overflow bit for FIFO overflow counter */
#define ETH_DMA_OVERFLOW_MISSEDFRAMECOUNTER ((uint32_t)0x00010000)  /*!< Overflow bit for missed frame counter */
#define IS_ETH_DMA_GET_OVERFLOW(OVERFLOW) (((OVERFLOW) == ETH_DMA_OVERFLOW_RXFIFOCOUNTER) || \
                                           ((OVERFLOW) == ETH_DMA_OVERFLOW_MISSEDFRAMECOUNTER))
/**
  * @}
  */ 

/* ETHERNET MAC address offsets */
#define ETH_MAC_ADDR_HBASE    (uint32_t)(ETH_MAC_BASE + (uint32_t)0x40)  /* ETHERNET MAC address high offset */
#define ETH_MAC_ADDR_LBASE    (uint32_t)(ETH_MAC_BASE + (uint32_t)0x44)  /* ETHERNET MAC address low offset */

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

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Reset ETH handle state
  * @param  __HANDLE__: specifies the ETH handle.
  * @retval None
  */
#define __HAL_ETH_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_ETH_STATE_RESET)

/** 
  * @brief  Checks whether the specified ETHERNET DMA Tx Desc flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check.
  * @retval the ETH_DMATxDescFlag (SET or RESET).
  */
#define __HAL_ETH_DMATXDESC_GET_FLAG(__HANDLE__, __FLAG__)             ((__HANDLE__)->TxDesc->Status & (__FLAG__) == (__FLAG__))

/**
  * @brief  Checks whether the specified ETHERNET DMA Rx Desc flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check.
  * @retval the ETH_DMATxDescFlag (SET or RESET).
  */
#define __HAL_ETH_DMARXDESC_GET_FLAG(__HANDLE__, __FLAG__)             ((__HANDLE__)->RxDesc->Status & (__FLAG__) == (__FLAG__))

/**
  * @brief  Enables the specified DMA Rx Desc receive interrupt.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_DMARXDESC_ENABLE_IT(__HANDLE__)                          ((__HANDLE__)->RxDesc->ControlBufferSize &=(~(uint32_t)ETH_DMARXDESC_DIC))

/**
  * @brief  Disables the specified DMA Rx Desc receive interrupt.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_DMARXDESC_DISABLE_IT(__HANDLE__)                         ((__HANDLE__)->RxDesc->ControlBufferSize |= ETH_DMARXDESC_DIC)

/**
  * @brief  Set the specified DMA Rx Desc Own bit.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_DMARXDESC_SET_OWN_BIT(__HANDLE__)                           ((__HANDLE__)->RxDesc->Status |= ETH_DMARXDESC_OWN)

/**
  * @brief  Returns the specified ETHERNET DMA Tx Desc collision count.
  * @param  __HANDLE__: ETH Handle                     
  * @retval The Transmit descriptor collision counter value.
  */
#define __HAL_ETH_DMATXDESC_GET_COLLISION_COUNT(__HANDLE__)                   (((__HANDLE__)->TxDesc->Status & ETH_DMATXDESC_CC) >> ETH_DMATXDESC_COLLISION_COUNTSHIFT)

/**
  * @brief  Set the specified DMA Tx Desc Own bit.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_SET_OWN_BIT(__HANDLE__)                       ((__HANDLE__)->TxDesc->Status |= ETH_DMATXDESC_OWN)

/**
  * @brief  Enables the specified DMA Tx Desc Transmit interrupt.
  * @param  __HANDLE__: ETH Handle                   
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_ENABLE_IT(__HANDLE__)                          ((__HANDLE__)->TxDesc->Status |= ETH_DMATXDESC_IC)

/**
  * @brief  Disables the specified DMA Tx Desc Transmit interrupt.
  * @param  __HANDLE__: ETH Handle             
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_DISABLE_IT(__HANDLE__)                          ((__HANDLE__)->TxDesc->Status &= ~ETH_DMATXDESC_IC)

/**
  * @brief  Selects the specified ETHERNET DMA Tx Desc Checksum Insertion.
  * @param  __HANDLE__: ETH Handle  
  * @param  __CHECKSUM__: specifies is the DMA Tx desc checksum insertion.
  *   This parameter can be one of the following values:
  *     @arg ETH_DMATXDESC_CHECKSUMBYPASS : Checksum bypass
  *     @arg ETH_DMATXDESC_CHECKSUMIPV4HEADER : IPv4 header checksum
  *     @arg ETH_DMATXDESC_CHECKSUMTCPUDPICMPSEGMENT : TCP/UDP/ICMP checksum. Pseudo header checksum is assumed to be present
  *     @arg ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL : TCP/UDP/ICMP checksum fully in hardware including pseudo header                                                                
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_CHECKSUM_INSERTION(__HANDLE__, __CHECKSUM__)     ((__HANDLE__)->TxDesc->Status |= (__CHECKSUM__))

/**
  * @brief  Enables the DMA Tx Desc CRC.
  * @param  __HANDLE__: ETH Handle 
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_CRC_ENABLE(__HANDLE__)                          ((__HANDLE__)->TxDesc->Status &= ~ETH_DMATXDESC_DC)

/**
  * @brief  Disables the DMA Tx Desc CRC.
  * @param  __HANDLE__: ETH Handle 
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_CRC_DISABLE(__HANDLE__)                         ((__HANDLE__)->TxDesc->Status |= ETH_DMATXDESC_DC)

/**
  * @brief  Enables the DMA Tx Desc padding for frame shorter than 64 bytes.
  * @param  __HANDLE__: ETH Handle 
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_SHORT_FRAME_PADDING_ENABLE(__HANDLE__)            ((__HANDLE__)->TxDesc->Status &= ~ETH_DMATXDESC_DP)

/**
  * @brief  Disables the DMA Tx Desc padding for frame shorter than 64 bytes.
  * @param  __HANDLE__: ETH Handle 
  * @retval None
  */
#define __HAL_ETH_DMATXDESC_SHORT_FRAME_PADDING_DISABLE(__HANDLE__)           ((__HANDLE__)->TxDesc->Status |= ETH_DMATXDESC_DP)

/** 
 * @brief  Enables the specified ETHERNET MAC interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET MAC interrupt sources to be
  *   enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ETH_MAC_IT_TST : Time stamp trigger interrupt 
  *     @arg ETH_MAC_IT_PMT : PMT interrupt 
  * @retval None
  */
#define __HAL_ETH_MAC_ENABLE_IT(__HANDLE__, __INTERRUPT__)                 ((__HANDLE__)->Instance->MACIMR |= (__INTERRUPT__))

/**
  * @brief  Disables the specified ETHERNET MAC interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET MAC interrupt sources to be
  *   enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ETH_MAC_IT_TST : Time stamp trigger interrupt 
  *     @arg ETH_MAC_IT_PMT : PMT interrupt
  * @retval None
  */
#define __HAL_ETH_MAC_DISABLE_IT(__HANDLE__, __INTERRUPT__)                ((__HANDLE__)->Instance->MACIMR &= ~(__INTERRUPT__))

/**
  * @brief  Initiate a Pause Control Frame (Full-duplex only).
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_INITIATE_PAUSE_CONTROL_FRAME(__HANDLE__)              ((__HANDLE__)->Instance->MACFCR |= ETH_MACFCR_FCBBPA)

/**
  * @brief  Checks whether the ETHERNET flow control busy bit is set or not.
  * @param  __HANDLE__: ETH Handle
  * @retval The new state of flow control busy status bit (SET or RESET).
  */
#define __HAL_ETH_GET_FLOW_CONTROL_BUSY_STATUS(__HANDLE__)               (((__HANDLE__)->Instance->MACFCR & ETH_MACFCR_FCBBPA) == ETH_MACFCR_FCBBPA)

/**
  * @brief  Enables the MAC Back Pressure operation activation (Half-duplex only).
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_BACK_PRESSURE_ACTIVATION_ENABLE(__HANDLE__)          ((__HANDLE__)->Instance->MACFCR |= ETH_MACFCR_FCBBPA)

/**
  * @brief  Disables the MAC BackPressure operation activation (Half-duplex only).
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_BACK_PRESSURE_ACTIVATION_DISABLE(__HANDLE__)         ((__HANDLE__)->Instance->MACFCR &= ~ETH_MACFCR_FCBBPA)

/**
  * @brief  Checks whether the specified ETHERNET MAC flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg ETH_MAC_FLAG_TST  : Time stamp trigger flag   
  *     @arg ETH_MAC_FLAG_MMCT : MMC transmit flag  
  *     @arg ETH_MAC_FLAG_MMCR : MMC receive flag   
  *     @arg ETH_MAC_FLAG_MMC  : MMC flag  
  *     @arg ETH_MAC_FLAG_PMT  : PMT flag  
  * @retval The state of ETHERNET MAC flag.
  */
#define __HAL_ETH_MAC_GET_FLAG(__HANDLE__, __FLAG__)                   (((__HANDLE__)->Instance->MACSR &( __FLAG__)) == ( __FLAG__))

/** 
  * @brief  Enables the specified ETHERNET DMA interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET DMA interrupt sources to be
  *   enabled @defgroup ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_ENABLE_IT(__HANDLE__, __INTERRUPT__)                 ((__HANDLE__)->Instance->DMAIER |= (__INTERRUPT__))

/**
  * @brief  Disables the specified ETHERNET DMA interrupts.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the ETHERNET DMA interrupt sources to be
  *   disabled. @defgroup ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)                ((__HANDLE__)->Instance->DMAIER &= ~(__INTERRUPT__))

/**
  * @brief  Clears the ETHERNET DMA IT pending bit.
  * @param  __HANDLE__   : ETH Handle
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear. @defgroup ETH_DMA_Interrupts
  * @retval None
  */
#define __HAL_ETH_DMA_CLEAR_IT(__HANDLE__, __INTERRUPT__)      ((__HANDLE__)->Instance->DMASR =(__INTERRUPT__))

/**
  * @brief  Checks whether the specified ETHERNET DMA flag is set or not.
* @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to check. @defgroup ETH_DMA_Flags
  * @retval The new state of ETH_DMA_FLAG (SET or RESET).
  */
#define __HAL_ETH_DMA_GET_FLAG(__HANDLE__, __FLAG__)                   (((__HANDLE__)->Instance->DMASR &( __FLAG__)) == ( __FLAG__))

/**
  * @brief  Checks whether the specified ETHERNET DMA flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __FLAG__: specifies the flag to clear. @defgroup ETH_DMA_Flags
  * @retval The new state of ETH_DMA_FLAG (SET or RESET).
  */
#define __HAL_ETH_DMA_CLEAR_FLAG(__HANDLE__, __FLAG__)                 ((__HANDLE__)->Instance->DMASR = (__FLAG__))

/**
  * @brief  Checks whether the specified ETHERNET DMA overflow flag is set or not.
  * @param  __HANDLE__: ETH Handle
  * @param  __OVERFLOW__: specifies the DMA overflow flag to check.
  *   This parameter can be one of the following values:
  *     @arg ETH_DMA_OVERFLOW_RXFIFOCOUNTER : Overflow for FIFO Overflows Counter
  *     @arg ETH_DMA_OVERFLOW_MISSEDFRAMECOUNTER : Overflow for Buffer Unavailable Missed Frame Counter
  * @retval The state of ETHERNET DMA overflow Flag (SET or RESET).
  */
#define __HAL_ETH_GET_DMA_OVERFLOW_STATUS(__HANDLE__, __OVERFLOW__)       (((__HANDLE__)->Instance->DMAMFBOCR & (__OVERFLOW__)) == (__OVERFLOW__))

/**
  * @brief  Set the DMA Receive status watchdog timer register value
  * @param  __HANDLE__: ETH Handle
  * @param  __VALUE__: DMA Receive status watchdog timer register value   
  * @retval None
  */
#define __HAL_ETH_SET_RECEIVE_WATCHDOG_TIMER(__HANDLE__, __VALUE__)       ((__HANDLE__)->Instance->DMARSWTR = (__VALUE__))

/** 
  * @brief  Enables any unicast packet filtered by the MAC address
  *   recognition to be a wake-up frame.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_GLOBAL_UNICAST_WAKEUP_ENABLE(__HANDLE__)               ((__HANDLE__)->Instance->MACPMTCSR |= ETH_MACPMTCSR_GU)

/**
  * @brief  Disables any unicast packet filtered by the MAC address
  *   recognition to be a wake-up frame.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_GLOBAL_UNICAST_WAKEUP_DISABLE(__HANDLE__)              ((__HANDLE__)->Instance->MACPMTCSR &= ~ETH_MACPMTCSR_GU)

/**
  * @brief  Enables the MAC Wake-Up Frame Detection.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_WAKEUP_FRAME_DETECTION_ENABLE(__HANDLE__)              ((__HANDLE__)->Instance->MACPMTCSR |= ETH_MACPMTCSR_WFE)

/**
  * @brief  Disables the MAC Wake-Up Frame Detection.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_WAKEUP_FRAME_DETECTION_DISABLE(__HANDLE__)             ((__HANDLE__)->Instance->MACPMTCSR &= ~ETH_MACPMTCSR_WFE)

/**
  * @brief  Enables the MAC Magic Packet Detection.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MAGIC_PACKET_DETECTION_ENABLE(__HANDLE__)              ((__HANDLE__)->Instance->MACPMTCSR |= ETH_MACPMTCSR_MPE)

/**
  * @brief  Disables the MAC Magic Packet Detection.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MAGIC_PACKET_DETECTION_DISABLE(__HANDLE__)             ((__HANDLE__)->Instance->MACPMTCSR &= ~ETH_MACPMTCSR_WFE)

/**
  * @brief  Enables the MAC Power Down.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_POWER_DOWN_ENABLE(__HANDLE__)                         ((__HANDLE__)->Instance->MACPMTCSR |= ETH_MACPMTCSR_PD)

/**
  * @brief  Disables the MAC Power Down.
  * @param  __HANDLE__: ETH Handle
  * @retval None
  */
#define __HAL_ETH_POWER_DOWN_DISABLE(__HANDLE__)                        ((__HANDLE__)->Instance->MACPMTCSR &= ~ETH_MACPMTCSR_PD)

/**
  * @brief  Checks whether the specified ETHERNET PMT flag is set or not.
  * @param  __HANDLE__: ETH Handle.
  * @param  __FLAG__: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg ETH_PMT_FLAG_WUFFRPR : Wake-Up Frame Filter Register Pointer Reset 
  *     @arg ETH_PMT_FLAG_WUFR    : Wake-Up Frame Received 
  *     @arg ETH_PMT_FLAG_MPR     : Magic Packet Received
  * @retval The new state of ETHERNET PMT Flag (SET or RESET).
  */
#define __HAL_ETH_GET_PMT_FLAG_STATUS(__HANDLE__, __FLAG__)               (((__HANDLE__)->Instance->MACPMTCSR &( __FLAG__)) == ( __FLAG__))

/** 
  * @brief  Preset and Initialize the MMC counters to almost-full value: 0xFFFF_FFF0 (full - 16)
  * @param   __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MMC_COUNTER_FULL_PRESET(__HANDLE__)                     ((__HANDLE__)->Instance->MMCCR |= (ETH_MMCCR_MCFHP | ETH_MMCCR_MCP))

/**
  * @brief  Preset and Initialize the MMC counters to almost-half value: 0x7FFF_FFF0 (half - 16)
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MMC_COUNTER_HALF_PRESET(__HANDLE__)                     do{(__HANDLE__)->Instance->MMCCR &= ~ETH_MMCCR_MCFHP;\
                                                                          (__HANDLE__)->Instance->MMCCR |= ETH_MMCCR_MCP;} while (0)

/**
  * @brief  Enables the MMC Counter Freeze.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MMC_COUNTER_FREEZE_ENABLE(__HANDLE__)                  ((__HANDLE__)->Instance->MMCCR |= ETH_MMCCR_MCF)

/**
  * @brief  Disables the MMC Counter Freeze.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MMC_COUNTER_FREEZE_DISABLE(__HANDLE__)                 ((__HANDLE__)->Instance->MMCCR &= ~ETH_MMCCR_MCF)

/**
  * @brief  Enables the MMC Reset On Read.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_ETH_MMC_RESET_ONREAD_ENABLE(__HANDLE__)                ((__HANDLE__)->Instance->MMCCR |= ETH_MMCCR_ROR)

/**
  * @brief  Disables the MMC Reset On Read.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_ETH_MMC_RESET_ONREAD_DISABLE(__HANDLE__)               ((__HANDLE__)->Instance->MMCCR &= ~ETH_MMCCR_ROR)

/**
  * @brief  Enables the MMC Counter Stop Rollover.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_ETH_MMC_COUNTER_ROLLOVER_ENABLE(__HANDLE__)            ((__HANDLE__)->Instance->MMCCR &= ~ETH_MMCCR_CSR)

/**
  * @brief  Disables the MMC Counter Stop Rollover.
  * @param  __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_ETH_MMC_COUNTER_ROLLOVER_DISABLE(__HANDLE__)           ((__HANDLE__)->Instance->MMCCR |= ETH_MMCCR_CSR)

/**
  * @brief  Resets the MMC Counters.
  * @param   __HANDLE__: ETH Handle.
  * @retval None
  */
#define __HAL_ETH_MMC_COUNTERS_RESET(__HANDLE__)                         ((__HANDLE__)->Instance->MMCCR |= ETH_MMCCR_CR)

/**
  * @brief  Enables the specified ETHERNET MMC Rx interrupts.
  * @param   __HANDLE__: ETH Handle.
  * @param  __INTERRUPT__: specifies the ETHERNET MMC interrupt sources to be enabled or disabled.
  *   This parameter can be one of the following values:  
  *     @arg ETH_MMC_IT_RGUF  : When Rx good unicast frames counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_RFAE  : When Rx alignment error counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_RFCE  : When Rx crc error counter reaches half the maximum value
  * @retval None
  */
#define __HAL_ETH_MMC_RX_IT_ENABLE(__HANDLE__, __INTERRUPT__)               (__HANDLE__)->Instance->MMCRIMR &= ~((__INTERRUPT__) & 0xEFFFFFFF)
/**
  * @brief  Disables the specified ETHERNET MMC Rx interrupts.
  * @param   __HANDLE__: ETH Handle.
  * @param  __INTERRUPT__: specifies the ETHERNET MMC interrupt sources to be enabled or disabled.
  *   This parameter can be one of the following values: 
  *     @arg ETH_MMC_IT_RGUF  : When Rx good unicast frames counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_RFAE  : When Rx alignment error counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_RFCE  : When Rx crc error counter reaches half the maximum value
  * @retval None
  */
#define __HAL_ETH_MMC_RX_IT_DISABLE(__HANDLE__, __INTERRUPT__)              (__HANDLE__)->Instance->MMCRIMR |= ((__INTERRUPT__) & 0xEFFFFFFF)
/**
  * @brief  Enables the specified ETHERNET MMC Tx interrupts.
  * @param   __HANDLE__: ETH Handle.
  * @param  __INTERRUPT__: specifies the ETHERNET MMC interrupt sources to be enabled or disabled.
  *   This parameter can be one of the following values:  
  *     @arg ETH_MMC_IT_TGF   : When Tx good frame counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_TGFMSC: When Tx good multi col counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_TGFSC : When Tx good single col counter reaches half the maximum value 
  * @retval None
  */
#define __HAL_ETH_MMC_TX_IT_ENABLE(__HANDLE__, __INTERRUPT__)            ((__HANDLE__)->Instance->MMCRIMR &= ~ (__INTERRUPT__))

/**
  * @brief  Disables the specified ETHERNET MMC Tx interrupts.
  * @param   __HANDLE__: ETH Handle.
  * @param  __INTERRUPT__: specifies the ETHERNET MMC interrupt sources to be enabled or disabled.
  *   This parameter can be one of the following values:  
  *     @arg ETH_MMC_IT_TGF   : When Tx good frame counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_TGFMSC: When Tx good multi col counter reaches half the maximum value 
  *     @arg ETH_MMC_IT_TGFSC : When Tx good single col counter reaches half the maximum value 
  * @retval None
  */
#define __HAL_ETH_MMC_TX_IT_DISABLE(__HANDLE__, __INTERRUPT__)           ((__HANDLE__)->Instance->MMCRIMR |= (__INTERRUPT__))                                             

/** @defgroup ETH_EXTI_LINE_WAKEUP 
  * @{
  */ 
#define ETH_EXTI_LINE_WAKEUP              ((uint32_t)0x00080000)  /*!< External interrupt line 19 Connected to the ETH EXTI Line */

/**
  * @}
  */

/**
  * @brief  Enables the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_ENABLE_IT()    EXTI->IMR |= (ETH_EXTI_LINE_WAKEUP)

/**
  * @brief  Disables the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_DISABLE_IT()   EXTI->IMR &= ~(ETH_EXTI_LINE_WAKEUP)

/**
  * @brief  Get flag of the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_GET_FLAG()     EXTI->PR & (ETH_EXTI_LINE_WAKEUP)

/**
  * @brief  Clear flag of the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_CLEAR_FLAG()   EXTI->PR = (ETH_EXTI_LINE_WAKEUP)

/**
  * @brief  Sets rising edge trigger to the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_SET_RISING_EGDE_TRIGGER()  EXTI->FTSR &= ~(ETH_EXTI_LINE_WAKEUP);\
                                                  EXTI->RTSR |= ETH_EXTI_LINE_WAKEUP

/**
  * @brief  Sets falling edge trigger to the ETH External interrupt line.
  * @param  None
  * @retval None
  */                                                      
#define __HAL_ETH_EXTI_SET_FALLING_EGDE_TRIGGER()  EXTI->FTSR |= (ETH_EXTI_LINE_WAKEUP);\
                                                   EXTI->RTSR &= ~(ETH_EXTI_LINE_WAKEUP)

/**
  * @brief  Sets rising/falling edge trigger to the ETH External interrupt line.
  * @param  None
  * @retval None
  */
#define __HAL_ETH_EXTI_SET_FALLINGRISING_TRIGGER()   EXTI->RTSR &= ~(ETH_EXTI_LINE_WAKEUP);\
                                                     EXTI->FTSR &= ~(ETH_EXTI_LINE_WAKEUP);\
                                                     EXTI->RTSR |= ETH_EXTI_LINE_WAKEUP;\
                                                     EXTI->FTSR |= ETH_EXTI_LINE_WAKEUP

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  ****************************/
HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_DeInit(ETH_HandleTypeDef *heth);
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth);
void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_DMATxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMATxDescTab, uint8_t* TxBuff, uint32_t TxBuffCount);
HAL_StatusTypeDef HAL_ETH_DMARxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);

/* IO operation functions  ****************************************************/
HAL_StatusTypeDef HAL_ETH_TransmitFrame(ETH_HandleTypeDef *heth, uint32_t FrameLength);
HAL_StatusTypeDef HAL_ETH_GetReceivedFrame(ETH_HandleTypeDef *heth);

 /* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_ETH_GetReceivedFrame_IT(ETH_HandleTypeDef *heth);
void HAL_ETH_IRQHandler(ETH_HandleTypeDef *heth);

 /* Callback in non blocking modes (Interrupt) */
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth);
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth);
void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth);

/* Cmmunication with PHY functions*/
HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t *RegValue);
HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t RegValue);

/* Peripheral Control functions  **********************************************/
HAL_StatusTypeDef HAL_ETH_Start(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETH_Stop(ETH_HandleTypeDef *heth);

HAL_StatusTypeDef HAL_ETH_ConfigMAC(ETH_HandleTypeDef *heth, ETH_MACInitTypeDef *macconf);
HAL_StatusTypeDef HAL_ETH_ConfigDMA(ETH_HandleTypeDef *heth, ETH_DMAInitTypeDef *dmaconf);

/* Peripheral State functions  ************************************************/
HAL_ETH_StateTypeDef HAL_ETH_GetState(ETH_HandleTypeDef *heth);

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_ETH_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
