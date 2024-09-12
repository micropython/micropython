/**
  ******************************************************************************
  * @file    stm32n6xx_hal_eth_ex.h
  * @author  MCD Application Team
  * @brief   Header file of ETH HAL Extended module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_ETH_EX_H
#define STM32N6xx_HAL_ETH_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ETH1)

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup ETHEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Types ETHEx Exported Types
  * @{
  */

/**
  * @brief  ETH RX VLAN structure definition
  */
typedef struct
{
  FunctionalState InnerVLANTagInStatus;      /*!< Enables or disables Inner VLAN Tag in Rx Status  */

  uint32_t StripInnerVLANTag;                /*!< Sets the Inner VLAN Tag Stripping on Receive
                                                  This parameter can be a value of
                                                  @ref ETHEx_Rx_Inner_VLAN_Tag_Stripping */

  FunctionalState InnerVLANTag;              /*!< Enables or disables Inner VLAN Tag */

  FunctionalState DoubleVLANProcessing;      /*!< Enable or Disable double VLAN processing */

  FunctionalState VLANTagHashTableMatch;     /*!< Enable or Disable VLAN Tag Hash Table Match */

  FunctionalState VLANTagInStatus;           /*!< Enable or Disable VLAN Tag in Rx status */

  uint32_t StripVLANTag;                     /*!< Set the VLAN Tag Stripping on Receive
                                                  This parameter can be a value of @ref ETHEx_Rx_VLAN_Tag_Stripping */

  uint32_t VLANTypeCheck;                    /*!< Enable or Disable VLAN Type Check
                                                  This parameter can be a value of @ref ETHEx_VLAN_Type_Check */

  FunctionalState VLANTagInverseMatch;       /*!< Enable or disable VLAN Tag Inverse Match */

  FunctionalState BitVLANcomparison;         /*!< Enable 12-Bit VLAN Tag Comparison */

  uint32_t        VLANcomparison;            /*!< 12-bit or 16-bit VLAN comparison */

  FunctionalState VLANTagEnable;             /*!< VLAN Tag Enable */

  uint32_t        VLANTagID;                 /*!< VLAN Tag ID */

  uint32_t        RxDMAChannelNumber;          /*!< Rx DMA Channel Number  */

  FunctionalState DMAChannelNumberEnable;    /*!< Enable Rx DMA Channel Number */
} ETH_RxVLANConfigTypeDef;
/**
  *
  */

/**
  * @brief  ETH MAC TMRQR INDIRECT REG enum definition
  */
typedef enum
{
  ETH_IND_REG_0    = 0x00000000U,   /*!<  Indirect register 0 */
  ETH_IND_REG_1    = 0x00000001U,   /*!<  Indirect register 1 */
  ETH_IND_REG_2    = 0x00000002U,   /*!<  Indirect register 2 */
  ETH_IND_REG_3    = 0x00000003U,   /*!<  Indirect register 3 */
  ETH_IND_REG_4    = 0x00000004U,   /*!<  Indirect register 4 */
  ETH_IND_REG_5    = 0x00000005U,   /*!<  Indirect register 5 */
  ETH_IND_REG_6    = 0x00000006U,   /*!<  Indirect register 6 */
  ETH_IND_REG_7    = 0x00000007U    /*!<  Indirect register 7 */
} ETH_IDRegTypeDef;
/**
  *
  */
/**
  * @brief  ETH MAC_MTL Mapping Configuration Structure definition
  */
typedef struct
{
  uint32_t                VLANTagFilterFailPacketsQueue;                  /*!< Specifies the Rx queue to which the tagged packets fail DA/SA/VLANtag filter must be routed to */

  FunctionalState         VLANTagFilterFailPacketsQueuingEnable;          /*!< Enables routing the tagged packets fail DA/SA/VLANtag filter to Rx queue */

  uint32_t                MulticastAddFilterFailPacketsQueue;             /*!< Specifies the Rx queue to which the Multicast packets fail the DA/SA filter are routed to */

  FunctionalState         MulticastAddrFilterFailPacketsQueuingEnable;    /*!< Enables routing Multicast Packets fail DA/SA filter to Rx Queue programmed in MFFQ */

  uint32_t                UnicastAddrFilterFailPacketsQueue;              /*!< Specifies the Rx queue to which the Unicast packets fail the DA/SA filter are routed to */

  FunctionalState         UnicastAddrFilterFailPacketsQueuingEnable;      /*!< Enables routing Unicast Packets fail DA/SA filter to Rx Queue programmed in UFFQ */

  FunctionalState         TypeFieldBasedRxQueuingEnable;                  /*!< Enables Type field based Rx queuing */

  uint32_t                OverridingMCBCQueuePrioritySelect;              /*!< Select the Overriding MC-BC queue priority  */

  uint32_t                FramePreemptionResidueQueue;                    /*!< Specifies the Rx queue to which the residual preemption frames must be forwarded */

  uint32_t                TaggedPTPoEPacketsQueuingControl;               /*!< Specifies the routing of the VLAN tagged PTPoE packets */

  FunctionalState         TaggedAVControlPacketsQueuingEnable;            /*!< Enable MAC routes the received Tagged AV control packets to Rx queue specified by AVCPQ field */

  FunctionalState         MulticastBroadcastQueueEnable;                  /*!< Specifies that Multicast or Broadcast packets routing to the Rx queue is enabled */

  uint32_t                MulticastBroadcastQueue;                        /*!< Specifies the Rx queue onto which Multicast or Broadcast packets are routed */

  uint32_t                UntaggedPacketQueue;                            /*!< Specifies the Rx queue to which Untagged Packets are to be routed */

  uint32_t                PTPPacketsQueue;                                /*!< Specifies the Rx queue on which the PTP packets sent over the Ethernet payload */

  uint32_t                AVUntaggedControlPacketsQueue;                  /*!< Specifies the Receive queue to receive AV untagged control packets */

  uint32_t                PrioritiesSelectedRxQ0;                         /*!< Specifies the Priorities Selected in the Receive Queue 0 @ref ETHEx_Rx_VLAN_PRIO */

  uint32_t                PrioritiesSelectedRxQ1;                         /*!< Specifies the Priorities Selected in the Receive Queue 1 @ref ETHEx_Rx_VLAN_PRIO */


} ETH_MACMTLMappingTypeDef;
/**
  *
  */

/**
  * @brief  ETH MTL Queue Configuration Structure definition
  */
typedef struct
{
  uint32_t               QueueOpMode;                     /*!< Queue Disabled, Enabled or AV Mode. */

  uint32_t               AVAlgorithm;                     /*!< Queue Disabled, Enabled or AV Mode. */

  uint32_t               TxQueueSize;                     /*!< Specifies the Tx Queue Size */

  uint32_t               TransmitQueueMode;               /*!< Specifies the Transmit Queue operating mode.*/
} MTL_TxQueueInstance_t;
/**
  *
  */

/**
  * @brief  ETH MTL Queue Configuration Structure definition
  */
typedef struct
{
  uint32_t               QueueOpMode;                     /*!< Queue Disabled, Enabled or AV Mode. */

  uint32_t               RxQueueSize;                     /*!< Specifies the Rx Queue Size */

  FunctionalState        DropTCPIPChecksumErrorPacket;    /*!< Enables or disables Dropping of TCPIP Checksum Error Packets.*/

  FunctionalState        ForwardRxErrorPacket;            /*!< Enables or disables  forwarding Error Packets. */

  FunctionalState        ForwardRxUndersizedGoodPacket;   /*!< Enables or disables  forwarding Undersized Good Packets.*/

  uint32_t               ReceiveQueueMode;                /*!< Specifies the Receive Queue operating mode */

  uint32_t               MappedToDMACh;                   /*!< Specifies the DMA Channel to which MTL Q is mapped */

} MTL_RxQueueInstance_t;
/**
  *
  */

/**
  * @brief  ETH MTL Configuration Structure definition
  */
typedef struct
{
  uint32_t                TxSchedulingAlgorithm;        /*!< Specifies the algorithm for Tx scheduling */

  uint32_t                ReceiveArbitrationAlgorithm;  /*!< Specifies the arbitration algorithm for the Rx side */

  MTL_TxQueueInstance_t   TxQ[ETH_MTL_TX_Q_CNT];        /*!< MTL Tx Queue Configuration */

  MTL_RxQueueInstance_t   RxQ[ETH_MTL_RX_Q_CNT];        /*!< MTL Rx Queue Configuration */

  FunctionalState         TransmitStatus;               /*!< Enables or disables forwarding Tx Packet Status to the application. */

} ETH_MTLConfigTypeDef;
/**
  *
  */

/**
  * @brief  ETH Packet TYPE Queue Configuration structure definition
  */
typedef struct
{
  ETH_IDRegTypeDef Address; /*!< Sets Address Offset for indirect accesses to ETH_MAC_TMRQR */

  uint32_t Type;          /*!< Indicates the type value of packet that needs to be compared with the received packet. */

  uint32_t Queue;         /*!< Indicates the receive queue number to which the packet needs to be forwarded */

  uint32_t Preemption;    /*!< Preemption or Express Packet
                                This parameter can be a value of @ref ETHEx_Preemption_Packet */
} ETH_PacketTypeQueueConfigTypeDef;
/**
  *
  */

/**
  * @brief  ETH TX VLAN structure definition
  */
typedef struct
{
  FunctionalState SourceTxDesc;   /*!< Enable or Disable VLAN tag source from DMA tx descriptors */

  FunctionalState SVLANType;      /*!< Enable or Disable insertion of SVLAN type */

  uint32_t VLANTagControl;        /*!< Sets the VLAN tag control in tx packets
                                      This parameter can be a value of @ref ETHEx_VLAN_Tag_Control */
} ETH_TxVLANConfigTypeDef;
/**
  *
  */

/**
  * @brief  ETH L3 filter structure definition
  */
typedef struct
{
  uint32_t Protocol;                /*!< Sets the L3 filter protocol to IPv4 or IPv6
                                         This parameter can be a value of @ref ETHEx_L3_Protocol */

  uint32_t SrcAddrFilterMatch;      /*!< Sets the L3 filter source address match
                                         This parameter can be a value of @ref ETHEx_L3_Source_Match */

  uint32_t DestAddrFilterMatch;     /*!< Sets the L3 filter destination address match
                                         This parameter can be a value of @ref ETHEx_L3_Destination_Match */

  uint32_t SrcAddrHigherBitsMatch;  /*!< Sets the L3 filter source address higher bits match
                                         This parameter can be a value from 0 to 31 */

  uint32_t DestAddrHigherBitsMatch; /*!< Sets the L3 filter destination address higher bits match
                                         This parameter can be a value from 0 to 31 */

  uint32_t Ip4SrcAddr;              /*!< Sets the L3 filter IPv4 source address if IPv4 protocol is used
                                         This parameter can be a value from 0x0 to 0xFFFFFFFF */

  uint32_t Ip4DestAddr;             /*!< Sets the L3 filter IPv4 destination  address if IPv4 protocol is used
                                         This parameter can be a value from 0 to 0xFFFFFFFF  */

  uint32_t Ip6Addr[4];                 /*!< Sets the L3 filter IPv6 address if IPv6 protocol is used
                                          This parameter must be a table of 4 words (4* 32 bits) */
} ETH_L3FilterConfigTypeDef;
/**
  *
  */

/**
  * @brief  ETH L4 filter structure definition
  */
typedef struct
{
  uint32_t Protocol;               /*!< Sets the L4 filter protocol to TCP or UDP
                                        This parameter can be a value of @ref ETHEx_L4_Protocol */

  uint32_t SrcPortFilterMatch;     /*!< Sets the L4 filter source port match
                                        This parameter can be a value of @ref ETHEx_L4_Source_Match */

  uint32_t DestPortFilterMatch;    /*!< Sets the L4 filter destination port match
                                        This parameter can be a value of @ref ETHEx_L4_Destination_Match */

  uint32_t SourcePort;             /*!< Sets the L4 filter source port
                                        This parameter must be a value from 0x0 to 0xFFFF */

  uint32_t DestinationPort;        /*!< Sets the L4 filter destination port
                                        This parameter must be a value from 0x0 to 0xFFFF */
} ETH_L4FilterConfigTypeDef;
/**
  *
  */

#ifdef HAL_ETH_USE_CBS
/**
  * @brief  ETH CBS Algorithm components
  */
typedef struct
{
  uint32_t               QueueIdx;            /*!< Specifies the queue Index to be configured  */

  uint32_t               SlotCount;           /*!< Specifies number of slots  */

  uint32_t               CreditControl;       /*!< Specifies Credit Control mode  */

  uint32_t               IdleSlope;           /*!< idleSlopeCredit value required for the CBS algorithm */

  uint32_t               SendSlope;           /*!< sendSlope value required for the CBS algorithm */

  uint32_t               HiCredit;            /*!< hiCredit value required for the CBS algorithm */

  uint32_t               LoCredit;            /*!< loCredit value required for the CBS algorithm */

} ETH_CBSConfigTypeDef;
/**
  *
  */

#endif /* HAL_ETH_USE_CBS */

#ifdef HAL_ETH_USE_TAS
/**
  * @brief  ETH TAS Operation components
  */
typedef struct
{
  uint32_t Gate;                         /*! Gate status Open or Closed */

  uint32_t Interval;                     /*! Time interval valid gate control */

} ETH_TASOperationConfigTypeDef;

/**
  * @brief  ETH TAS Operation components
  */
typedef struct
{
  uint64_t         BaseTimeRegister;                         /*! Base Time 32 bits seconds 32 bits nanoseconds */

  uint64_t         CycleTimeRegister;                        /*! Cycle Time 32 bits seconds 32 bits nanoseconds */

  uint32_t         TimeExtensionRegister;                    /*! Time Extension 32 bits seconds 32 bits nanoseconds */

  uint32_t         ListLengthRegister;                       /*! GCL list Length */

  ETH_TASOperationConfigTypeDef *opList;                     /*! Pointer to GCL list size */

} ETH_GCLConfigTypeDef;

/**
  * @brief  ETH Enhancements to Scheduled Traffic components
  */
typedef struct
{
  uint32_t                  SwitchToSWOL;                      /*! Switch to S/W owned list */

  uint32_t                  PTPTimeOffset;                     /*! PTP Time Offset Value */

  uint32_t                  CurrentTimeOffset;                 /*! Current Time Offset Value */

  uint32_t                  TimeIntervalLeftShift;             /*! Time Interval Left Shift Amount */

  uint32_t                  LoopCountSchedulingError;          /*! Loop Count to report Scheduling Error */

  FunctionalState           DropFramesCausingError;            /*! Drop Frames causing Scheduling Error */

  FunctionalState           NotDropFramesDuringFrameSizeError; /*! Drop Frames causing Scheduling Error */

  uint32_t                  OverheadBytesValue;                /*! Overhead Bytes Value */

  ETH_GCLConfigTypeDef      GCLRegisters;                     /*! Pointer to GCL Registers */

} ETH_ESTConfigTypeDef;
/**
  *
  */
#endif /* HAL_ETH_USE_TAS */

#ifdef HAL_ETH_USE_FPE
/**
  * @brief  ETH Frame Preemption components
  */
typedef struct
{
  uint32_t            AdditionalFragmentSize;   /*! Additional Fragment Size */

  uint32_t            PreemptionClassification; /*! Preemption Classification
                                      This parameter can be a combination of @ref ETHEx_FPE_Preemption_Classification*/

  uint32_t            HoldReleaseStatus;        /*! Hold/Release Status */

  FunctionalState     SendVerifymPacket;        /*! Send Verify mPacket */

  FunctionalState     SendRespondmPacket;       /*! Send Respond mPacket */

  uint32_t            HoldAdvance;              /*! The maximum time in nanoseconds that can elapse between issuing a
                                               HOLD to the MAC and the MAC ceasing to transmit any preemptible frame */
  uint32_t            ReleaseAdvance;           /*! The maximum time in nanoseconds that can elapse between issuing a
                             RELEASE to the MAC and the MAC being ready to resume transmission of preemptible frames */
} ETH_FPEConfigTypeDef;
/**
  *
  */
#endif /* HAL_ETH_USE_FPE */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ETHEx_Exported_Constants ETHEx Exported Constants
  * @{
  */

/** @defgroup ETHEx_LPI_Event ETHEx LPI Event
  * @{
  */
#define ETH_TX_LPI_ENTRY    ETH_MACLCSR_TLPIEN
#define ETH_TX_LPI_EXIT     ETH_MACLCSR_TLPIEX
#define ETH_RX_LPI_ENTRY    ETH_MACLCSR_RLPIEN
#define ETH_RX_LPI_EXIT     ETH_MACLCSR_RLPIEX
/**
  * @}
  */

/** @defgroup ETHEx_L3_Filter ETHEx L3 Filter
  * @{
  */
#define ETH_L3_FILTER_0                 0x00000000U
#define ETH_L3_FILTER_1                 0x0000000CU
/**
  * @}
  */

/** @defgroup ETHEx_L4_Filter ETHEx L4 Filter
  * @{
  */
#define ETH_L4_FILTER_0                 0x00000000U
#define ETH_L4_FILTER_1                 0x0000000CU
/**
  * @}
  */

/** @defgroup ETHEx_L3_Protocol ETHEx L3 Protocol
  * @{
  */
#define ETH_L3_IPV6_MATCH                       ETH_MACL3L4C0R_L3PEN0
#define ETH_L3_IPV4_MATCH                       0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_L3_Source_Match ETHEx L3 Source Match
  * @{
  */
#define ETH_L3_SRC_ADDR_PERFECT_MATCH_ENABLE    ETH_MACL3L4C0R_L3SAM0
#define ETH_L3_SRC_ADDR_INVERSE_MATCH_ENABLE    (ETH_MACL3L4C0R_L3SAM0 | ETH_MACL3L4C0R_L3SAIM0)
#define ETH_L3_SRC_ADDR_MATCH_DISABLE           0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_L3_Destination_Match ETHEx L3 Destination Match
  * @{
  */
#define ETH_L3_DEST_ADDR_PERFECT_MATCH_ENABLE   ETH_MACL3L4C0R_L3DAM0
#define ETH_L3_DEST_ADDR_INVERSE_MATCH_ENABLE   (ETH_MACL3L4C0R_L3DAM0 | ETH_MACL3L4C0R_L3DAIM0)
#define ETH_L3_DEST_ADDR_MATCH_DISABLE          0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_L4_Protocol ETHEx L4 Protocol
  * @{
  */
#define ETH_L4_UDP_MATCH                        ETH_MACL3L4C0R_L4PEN0
#define ETH_L4_TCP_MATCH                        0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_L4_Source_Match ETHEx L4 Source Match
  * @{
  */
#define ETH_L4_SRC_PORT_PERFECT_MATCH_ENABLE    ETH_MACL3L4C0R_L4SPM0
#define ETH_L4_SRC_PORT_INVERSE_MATCH_ENABLE    (ETH_MACL3L4C0R_L4SPM0 |ETH_MACL3L4C0R_L4SPIM0)
#define ETH_L4_SRC_PORT_MATCH_DISABLE           0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_L4_Destination_Match ETHEx L4 Destination Match
  * @{
  */
#define ETH_L4_DEST_PORT_PERFECT_MATCH_ENABLE   ETH_MACL3L4C0R_L4DPM0
#define ETH_L4_DEST_PORT_INVERSE_MATCH_ENABLE   (ETH_MACL3L4C0R_L4DPM0 | ETH_MACL3L4C0R_L4DPIM0)
#define ETH_L4_DEST_PORT_MATCH_DISABLE          0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_Rx_Inner_VLAN_Tag_Stripping ETHEx Rx Inner VLAN Tag Stripping
  * @{
  */
#define ETH_INNERVLANTAGRXSTRIPPING_NONE      ETH_MACVTCR_EIVLS_DONOTSTRIP
#define ETH_INNERVLANTAGRXSTRIPPING_IFPASS    ETH_MACVTCR_EIVLS_STRIPIFPASS
#define ETH_INNERVLANTAGRXSTRIPPING_IFFAILS   ETH_MACVTCR_EIVLS_STRIPIFFAILS
#define ETH_INNERVLANTAGRXSTRIPPING_ALWAYS    ETH_MACVTCR_EIVLS_ALWAYSSTRIP
/**
  * @}
  */

/** @defgroup ETHEx_Rx_VLAN_Tag_Stripping ETHEx Rx VLAN Tag Stripping
  * @{
  */
#define ETH_VLANTAGRXSTRIPPING_NONE      ETH_MACVTCR_EVLS_DONOTSTRIP
#define ETH_VLANTAGRXSTRIPPING_IFPASS    ETH_MACVTCR_EVLS_STRIPIFPASS
#define ETH_VLANTAGRXSTRIPPING_IFFAILS   ETH_MACVTCR_EVLS_STRIPIFFAILS
#define ETH_VLANTAGRXSTRIPPING_ALWAYS    ETH_MACVTCR_EVLS_ALWAYSSTRIP
/**
  * @}
  */

/** @defgroup ETHEx_VLAN_Type_Check ETHEx VLAN Type Check
  * @{
  */
#define ETH_VLANTYPECHECK_DISABLE    ETH_MACVTCR_DOVLTC
#define ETH_VLANTYPECHECK_SVLAN      (ETH_MACVTCR_ERSVLM | ETH_MACVTCR_ESVL)
#define ETH_VLANTYPECHECK_CVLAN      0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_VLAN_Tag_Control ETHEx_VLAN_Tag_Control
  * @{
  */
#define ETH_VLANTAGCONTROL_NONE       (ETH_MACVIR_VLP | ETH_MACVIR_VLC_NOVLANTAG)
#define ETH_VLANTAGCONTROL_DELETE     (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGDELETE)
#define ETH_VLANTAGCONTROL_INSERT     (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGINSERT)
#define ETH_VLANTAGCONTROL_REPLACE    (ETH_MACVIR_VLP | ETH_MACVIR_VLC_VLANTAGREPLACE)
/**
  * @}
  */

/** @defgroup ETHEx_Tx_VLAN_Tag ETHEx Tx VLAN Tag
  * @{
  */
#define ETH_INNER_TX_VLANTAG    0x00000001U
#define ETH_OUTER_TX_VLANTAG    0x00000000U
/**
  * @}
  */

/** @defgroup ETHEx_Rx_VLAN_PRIO ETHEx Rx VLAN PRIO
  * @{
  */
#define  ETH_RX_QUEUE_PRIO_0   0x00000001U    /*!<  Rx VLAN User Tag Priority 0 */
#define  ETH_RX_QUEUE_PRIO_1   0x00000002U    /*!<  Rx VLAN User Tag Priority 1 */
#define  ETH_RX_QUEUE_PRIO_2   0x00000004U    /*!<  Rx VLAN User Tag Priority 2 */
#define  ETH_RX_QUEUE_PRIO_3   0x00000008U    /*!<  Rx VLAN User Tag Priority 3 */
#define  ETH_RX_QUEUE_PRIO_4   0x00000010U    /*!<  Rx VLAN User Tag Priority 4 */
#define  ETH_RX_QUEUE_PRIO_5   0x00000020U    /*!<  Rx VLAN User Tag Priority 5 */
#define  ETH_RX_QUEUE_PRIO_6   0x00000040U    /*!<  Rx VLAN User Tag Priority 6 */
#define  ETH_RX_QUEUE_PRIO_7   0x00000080U    /*!<  Rx VLAN User Tag Priority 7 */
/**
  * @}
  */

/** @defgroup ETHEx_Preemption_Packet ETHEx Preemption Packet
  * @{
  */
#define ETH_EXPRESS_PACKET           0x00000000U
#define ETH_PREEMPTION_PACKET        ETH_MACTMRQR_PFEX
/**
  * @}
  */

/** @defgroup ETHEx_Command_Type ETHEx Command Type
  * @{
  */
#define ETH_WRITE_OPERATION          0x00000000U
#define ETH_READ_OPERATION           ETH_MACIACR_COM
/**
  * @}
  */

#ifdef HAL_ETH_USE_CBS
/** @defgroup ETHEx_CBS_Credit_Control ETHEx CBS Credit Control
  * @{
  */
#define ETH_ENABLE_CBS_CREDIT_CONTROL    ETH_MTLTXQ1ECR_CC
#define ETH_DISABLE_CBS_CREDIT_CONTROL   0x00000000U
/**
  * @}
  */
#endif /* HAL_ETH_USE_CBS */

#ifdef HAL_ETH_USE_TAS
#ifndef ETH_HWRESET_TIMEOUT
#define ETH_HWRESET_TIMEOUT     1000U
#endif /* ETH_SWRESET_TIMEOUT */
#endif /* HAL_ETH_USE_TAS */

#ifdef HAL_ETH_USE_FPE
/** @defgroup ETHEx_FPE_Preemption_Classification ETHEx FPE Preemption Classification
  * @{
  */
#define ETH_QUEUE0_EXPRESS         0x00000000U
#define ETH_QUEUE0_PREEMPTABLE     0x00000100U
#define ETH_QUEUE1_EXPRESS         0x00000000U
#define ETH_QUEUE1_PREEMPTABLE     0x00000200U
/**
  * @}
  */
#endif /* HAL_ETH_USE_FPE */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ETHEx_Exported_Functions
  * @{
  */

/** @addtogroup ETHEx_Exported_Functions_Group1
  * @{
  */
/* MAC ARP Offloading APIs  ***************************************************/
void              HAL_ETHEx_EnableARPOffload(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableARPOffload(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_SetARPAddressMatch(ETH_HandleTypeDef *heth, uint32_t IpAddress);

/* MAC L3 L4 Filtering APIs ***************************************************/
void              HAL_ETHEx_EnableL3L4Filtering(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableL3L4Filtering(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetL3FilterConfig(const ETH_HandleTypeDef *heth, uint32_t Filter,
                                              ETH_L3FilterConfigTypeDef *pL3FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_GetL4FilterConfig(const ETH_HandleTypeDef *heth, uint32_t Filter,
                                              ETH_L4FilterConfigTypeDef *pL4FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_SetL3FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter,
                                              const ETH_L3FilterConfigTypeDef *pL3FilterConfig);
HAL_StatusTypeDef HAL_ETHEx_SetL4FilterConfig(ETH_HandleTypeDef *heth, uint32_t Filter,
                                              const ETH_L4FilterConfigTypeDef *pL4FilterConfig);

/* MAC VLAN Processing APIs    ************************************************/
void              HAL_ETHEx_EnableVLANProcessing(ETH_HandleTypeDef *heth);
void              HAL_ETHEx_DisableVLANProcessing(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetRxVLANConfig(const ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig);
HAL_StatusTypeDef HAL_ETHEx_SetRxVLANConfig(ETH_HandleTypeDef *heth, ETH_RxVLANConfigTypeDef *pVlanConfig);
void              HAL_ETHEx_SetVLANHashTable(ETH_HandleTypeDef *heth, uint32_t VLANHashTable);
HAL_StatusTypeDef HAL_ETHEx_GetTxVLANConfig(const ETH_HandleTypeDef *heth, uint32_t VLANTag,
                                            ETH_TxVLANConfigTypeDef *pVlanConfig);
HAL_StatusTypeDef HAL_ETHEx_SetTxVLANConfig(ETH_HandleTypeDef *heth, uint32_t VLANTag,
                                            const ETH_TxVLANConfigTypeDef *pVlanConfig);
void              HAL_ETHEx_SetTxVLANIdentifier(ETH_HandleTypeDef *heth, uint32_t VLANTag, uint32_t VLANIdentifier);

/* Energy Efficient Ethernet APIs *********************************************/
void              HAL_ETHEx_EnterLPIMode(ETH_HandleTypeDef *heth, FunctionalState TxAutomate,
                                         FunctionalState TxClockStop);
void              HAL_ETHEx_ExitLPIMode(ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetMACLPIEvent(const ETH_HandleTypeDef *heth);
/* Multi-Queue Ethernet APIs *********************************************/
uint32_t          HAL_ETHEx_GetRxDMAChNumber(const ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetTxDMAChNumber(const ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetRxMTLQNumber(const ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetTxMTLQNumber(const ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetMTLConfig(const ETH_HandleTypeDef *heth, ETH_MTLConfigTypeDef *mtlconf);
HAL_StatusTypeDef HAL_ETHEx_SetMTLConfig(ETH_HandleTypeDef *heth, ETH_MTLConfigTypeDef *mtlconf);
HAL_StatusTypeDef HAL_ETHEx_SetMACMTLMappingConfig(ETH_HandleTypeDef *heth, const ETH_MACMTLMappingTypeDef *macmtlconf);
HAL_StatusTypeDef HAL_ETHEx_GetMACMTLMappingConfig(const ETH_HandleTypeDef *heth, ETH_MACMTLMappingTypeDef *macmtlconf);
void              ETHEx_SetMTLConfig(ETH_HandleTypeDef *heth, const ETH_MTLConfigTypeDef *mtlconf);
HAL_StatusTypeDef ETHEx_SetMACMTLMappingConfig(ETH_HandleTypeDef *heth, const ETH_MACMTLMappingTypeDef *macmtlconf);
HAL_StatusTypeDef HAL_ETHEx_SetUserTagPriorityQueue(ETH_HandleTypeDef *heth, uint32_t psrq, uint32_t queue);
HAL_StatusTypeDef HAL_ETHEx_GetUserTagPriorityQueue(const ETH_HandleTypeDef *heth, uint32_t *psrq,
                                                    uint32_t queue);
HAL_StatusTypeDef HAL_ETHEx_SetPacketTypeQueue(ETH_HandleTypeDef *heth,
                                               const ETH_PacketTypeQueueConfigTypeDef *typequeueconf);
HAL_StatusTypeDef HAL_ETHEx_GetPacketTypeQueue(ETH_HandleTypeDef *heth,
                                               ETH_PacketTypeQueueConfigTypeDef *typequeueconf);
#ifdef HAL_ETH_USE_CBS
HAL_StatusTypeDef HAL_ETHEx_SetCBSConfig(ETH_HandleTypeDef *heth, ETH_CBSConfigTypeDef *cbsconf);
HAL_StatusTypeDef HAL_ETHEx_GetCBSConfig(const ETH_HandleTypeDef *heth,
                                         ETH_CBSConfigTypeDef *pCBSConfig, uint8_t queueIndex);
HAL_StatusTypeDef HAL_ETHEx_EnableCBS(ETH_HandleTypeDef *heth, uint8_t queueIndex);
#endif /* HAL_ETH_USE_CBS */
#ifdef HAL_ETH_USE_TAS
uint32_t          HAL_ETHEx_GetGCLDepth(const ETH_HandleTypeDef *heth);
uint32_t          HAL_ETHEx_GetGCLWidthTimeInterval(const ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_EnableEST(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_DisableEST(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_SetESTConfig(ETH_HandleTypeDef *heth,  ETH_ESTConfigTypeDef *estconf);
HAL_StatusTypeDef HAL_ETHEx_SetGCLRegisters(ETH_HandleTypeDef *heth, const ETH_GCLConfigTypeDef *gclconf);
HAL_StatusTypeDef HAL_ETHEx_SetGCLConfig(ETH_HandleTypeDef *heth, ETH_GCLConfigTypeDef *gclconf);
HAL_StatusTypeDef HAL_ETHEx_GetGCLRegisters(ETH_HandleTypeDef *heth, ETH_GCLConfigTypeDef *gclconf);
#endif /* HAL_ETH_USE_TAS */
#ifdef HAL_ETH_USE_FPE
HAL_StatusTypeDef HAL_ETHEx_EnableFPE(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_DisableFPE(ETH_HandleTypeDef *heth);
HAL_StatusTypeDef HAL_ETHEx_GetFPEConfig(ETH_HandleTypeDef *heth,  ETH_FPEConfigTypeDef *fpeconf);
HAL_StatusTypeDef HAL_ETHEx_SetFPEConfig(ETH_HandleTypeDef *heth,  ETH_FPEConfigTypeDef *fpeconf);
#endif /* HAL_ETH_USE_FPE */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* ETH1 */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_ETH_EX_H */
