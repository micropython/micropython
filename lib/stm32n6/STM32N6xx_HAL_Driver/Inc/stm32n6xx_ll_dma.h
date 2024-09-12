/**
  ******************************************************************************
  * @file    stm32n6xx_ll_dma.h
  * @author  MCD Application Team
  * @brief   Header file of DMA LL module.
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
 @verbatim
  ==============================================================================
                        ##### LL DMA driver acronyms #####
  ==============================================================================
  [..]  Acronyms table :
                   =========================================
                   || Acronym ||                          ||
                   =========================================
                   || SRC     ||  Source                  ||
                   || DEST    ||  Destination             ||
                   || ADDR    ||  Address                 ||
                   || ADDRS   ||  Addresses               ||
                   || INC     ||  Increment / Incremented ||
                   || DEC     ||  Decrement / Decremented ||
                   || BLK     ||  Block                   ||
                   || RPT     ||  Repeat / Repeated       ||
                   || TRIG    ||  Trigger                 ||
                   =========================================
 @endverbatim
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_DMA_H
#define STM32N6xx_LL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if (defined (GPDMA1) || defined (HPDMA1))

/** @defgroup DMA_LL DMA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @defgroup DMA_LL_Private_Variables DMA Private Variables
  * @{
  */
#define DMA_CHANNEL0_OFFSET  (0x00000050UL)
#define DMA_CHANNEL1_OFFSET  (0x000000D0UL)
#define DMA_CHANNEL2_OFFSET  (0x00000150UL)
#define DMA_CHANNEL3_OFFSET  (0x000001D0UL)
#define DMA_CHANNEL4_OFFSET  (0x00000250UL)
#define DMA_CHANNEL5_OFFSET  (0x000002D0UL)
#define DMA_CHANNEL6_OFFSET  (0x00000350UL)
#define DMA_CHANNEL7_OFFSET  (0x000003D0UL)
#define DMA_CHANNEL8_OFFSET  (0x00000450UL)
#define DMA_CHANNEL9_OFFSET  (0x000004D0UL)
#define DMA_CHANNEL10_OFFSET (0x00000550UL)
#define DMA_CHANNEL11_OFFSET (0x000005D0UL)
#define DMA_CHANNEL12_OFFSET (0x00000650UL)
#define DMA_CHANNEL13_OFFSET (0x000006D0UL)
#define DMA_CHANNEL14_OFFSET (0x00000750UL)
#define DMA_CHANNEL15_OFFSET (0x000007D0UL)

/* Array used to get the DMA Channel register offset versus Channel index LL_DMA_CHANNEL_x */
static const uint32_t LL_DMA_CH_OFFSET_TAB[] =
{
  DMA_CHANNEL0_OFFSET,  DMA_CHANNEL1_OFFSET,  DMA_CHANNEL2_OFFSET,  DMA_CHANNEL3_OFFSET,
  DMA_CHANNEL4_OFFSET,  DMA_CHANNEL5_OFFSET,  DMA_CHANNEL6_OFFSET,  DMA_CHANNEL7_OFFSET,
  DMA_CHANNEL8_OFFSET,  DMA_CHANNEL9_OFFSET,  DMA_CHANNEL10_OFFSET, DMA_CHANNEL11_OFFSET,
  DMA_CHANNEL12_OFFSET, DMA_CHANNEL13_OFFSET, DMA_CHANNEL14_OFFSET, DMA_CHANNEL15_OFFSET,
};

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

#if defined (USE_FULL_LL_DRIVER)
/** @defgroup DMA_LL_ES_INIT DMA Exported Init structure
  * @{
  */

/**
  * @brief  LL DMA init structure definition.
  */
typedef struct
{
  uint32_t SrcAddress;               /*!< This field specify the data transfer source address.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcAddress().                                                */

  uint32_t DestAddress;              /*!< This field specify the data transfer destination address.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter must be a value between Min_Data = 0 and Max_Data = 0xFFFFFFFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestAddress().                                               */

  uint32_t Direction;                /*!< This field specify the data transfer direction.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_TRANSFER_DIRECTION.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDataTransferDirection().                                     */

  uint32_t BlkHWRequest;             /*!< This field specify the hardware request unity.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_BLKHW_REQUEST.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkHWRequest().                                              */

  uint32_t DataAlignment;            /*!< This field specify the transfer data alignment.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DATA_ALIGNMENT.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDataAlignment().                                             */

  uint32_t SrcBurstLength;           /*!< This field specify the source burst length of transfer in bytes.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter must be a value between Min_Data = 1 and Max_Data = 64.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcBurstLength().                                            */

  uint32_t DestBurstLength;          /*!< This field specify the destination burst length of transfer in bytes.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter must be a value between Min_Data = 1 and Max_Data = 64.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestBurstLength().                                           */

  uint32_t SrcDataWidth;             /*!< This field specify the source data width.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_SOURCE_DATA_WIDTH.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcDataWidth().                                              */

  uint32_t DestDataWidth;            /*!< This field specify the destination data width.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DESTINATION_DATA_WIDTH.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestDataWidth().                                             */

  uint32_t SrcIncMode;               /*!< This field specify the source burst increment mode.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_SOURCE_INCREMENT_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcIncMode().                                                */

  uint32_t DestIncMode;              /*!< This field specify the destination burst increment mode.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DESTINATION_INCREMENT_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestIncMode().                                               */

  uint32_t Priority;                 /*!< This field specify the channel priority level.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_PRIORITY_LEVEL.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetChannelPriorityLevel().                                      */

  uint32_t BlkDataLength;            /*!< This field specify the length of a block transfer in bytes.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter must be a value between Min_Data = 0 and Max_Data = 0x0000FFFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkDataLength().                                             */

  uint32_t BlkRptCount;              /*!< This field specify the number of repetitions of the current block.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value between 1 and 2048 Min_Data = 0
                                          and Max_Data = 0x000007FF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkRptCount().                                               */

  uint32_t TriggerMode;              /*!< This field specify the trigger mode.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_TRIGGER_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetTriggerMode().                                               */

  uint32_t TriggerPolarity;          /*!< This field specify the trigger event polarity.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_TRIGGER_POLARITY.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetTriggerPolarity().                                           */

  uint32_t TriggerSelection;         /*!< This field specify the trigger event selection.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_TRIGGER_SELECTION.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetHWTrigger().                                                 */

  uint32_t Request;                  /*!< This field specify the peripheral request selection.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_REQUEST_SELECTION.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetPeriphRequest().                                             */

  uint32_t TransferEventMode;        /*!< This field specify the transfer event mode.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_TRANSFER_EVENT_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetTransferEventMode().                                         */

  uint32_t DestWordExchange;         /*!< This field specify the destination  word exchange.
                                          Programming this field is mandatory for all available HPDMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DEST_WORD_EXCHANGE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestWordExchange().                                          */

  uint32_t DestHWordExchange;        /*!< This field specify the destination half word exchange.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DEST_HALFWORD_EXCHANGE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestHWordExchange().                                         */

  uint32_t DestByteExchange;         /*!< This field specify the destination byte exchange.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DEST_BYTE_EXCHANGE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestByteExchange().                                          */

  uint32_t SrcByteExchange;          /*!< This field specify the source byte exchange.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_SRC_BYTE_EXCHANGE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcByteExchange().                                           */

  uint32_t SrcAllocatedPort;         /*!< This field specify the source allocated port.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_SOURCE_ALLOCATED_PORT.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcAllocatedPort().                                          */

  uint32_t DestAllocatedPort;        /*!< This field specify the destination allocated port.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DESTINATION_ALLOCATED_PORT.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestAllocatedPort().                                         */

  uint32_t LinkAllocatedPort;        /*!< This field specify the linked-list allocated port.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_LINKED_LIST_ALLOCATED_PORT.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetLinkAllocatedPort().                                         */

  uint32_t LinkStepMode;             /*!< This field specify the link step mode.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value of @ref DMA_LL_EC_LINK_STEP_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetLinkStepMode().                                              */

  uint32_t SrcAddrUpdateMode;        /*!< This field specify the source address update mode.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value of @ref DMA_LL_EC_SRC_ADDR_UPDATE_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcAddrUpdate().                                             */

  uint32_t DestAddrUpdateMode;       /*!< This field specify the destination address update mode.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value of @ref DMA_LL_EC_DEST_ADDR_UPDATE_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestAddrUpdate().                                            */

  uint32_t SrcAddrOffset;            /*!< This field specifies the source address offset.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value Between 0 to 0x00001FFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetSrcAddrUpdateValue().                                        */

  uint32_t DestAddrOffset;           /*!< This field specifies the destination address offset.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value Between 0 to 0x00001FFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetDestAddrUpdateValue().                                       */

  uint32_t BlkRptSrcAddrUpdateMode;  /*!< This field specifies the block repeat source address update mode.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value of @ref DMA_LL_EC_BLK_RPT_SRC_ADDR_UPDATE_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkRptSrcAddrUpdate().                                       */

  uint32_t BlkRptDestAddrUpdateMode; /*!< This field specifies the block repeat destination address update mode.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value of @ref DMA_LL_EC_BLK_RPT_DEST_ADDR_UPDATE_MODE.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkRptDestAddrUpdate().                                      */

  uint32_t BlkRptSrcAddrOffset;      /*!< This field specifies the block repeat source address offset.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value Between 0 to 0x0000FFFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkRptSrcAddrUpdateValue().                                  */

  uint32_t BlkRptDestAddrOffset;     /*!< This field specifies the block repeat destination address offset.
                                          Programming this field is mandatory only for 2D addressing channels.
                                          This parameter can be a value Between 0 to 0x0000FFFF.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetBlkRptDestAddrUpdateValue().                                 */

  uint32_t LinkedListBaseAddr;       /*!< This field specify the linked list base address.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value Between 0 to 0xFFFF0000 (where the 4 first
                                          bytes are always forced to 0).
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetLinkedListBaseAddr().                                        */

  uint32_t LinkedListAddrOffset;     /*!< Specifies the linked list address offset.
                                          Programming this field is mandatory for all available DMA channels.
                                          This parameter can be a value Between 0 to 0x0000FFFC.
                                          This feature can be modified afterwards using unitary function
                                          @ref LL_DMA_SetLinkedListAddrOffset().                                      */

  uint32_t Mode;                     /*!< Specifies the transfer mode for the DMA channel.
                                          This parameter can be a value of @ref DMA_LL_TRANSFER_MODE                  */
} LL_DMA_InitTypeDef;


/**
  * @brief  LL DMA init linked list structure definition.
  */
typedef struct
{
  uint32_t Priority;             /*!< This field specify the channel priority level.
                                      Programming this field is mandatory for all available DMA channels.
                                      This parameter can be a value of @ref DMA_LL_EC_PRIORITY_LEVEL.
                                      This feature can be modified afterwards using unitary function
                                      @ref LL_DMA_SetChannelPriorityLevel().                                          */

  uint32_t LinkStepMode;         /*!< This field specify the link step mode.
                                      Programming this field is mandatory for all available DMA channels.
                                      This parameter can be a value of @ref DMA_LL_EC_LINK_STEP_MODE.
                                      This feature can be modified afterwards using unitary function
                                      @ref LL_DMA_SetLinkStepMode().                                                  */

  uint32_t LinkAllocatedPort;    /*!< This field specify the linked-list allocated port.
                                      Programming this field is mandatory for all available DMA channels.
                                      This parameter can be a value of @ref DMA_LL_EC_LINKED_LIST_ALLOCATED_PORT.
                                      This feature can be modified afterwards using unitary function
                                      @ref LL_DMA_SetLinkAllocatedPort().                                             */

  uint32_t TransferEventMode;    /*!< This field specify the transfer event mode.
                                      Programming this field is mandatory for all available DMA channels.
                                      This parameter can be a value of @ref DMA_LL_EC_TRANSFER_EVENT_MODE.
                                      This feature can be modified afterwards using unitary function
                                      @ref LL_DMA_SetTransferEventMode().                                             */
} LL_DMA_InitLinkedListTypeDef;


/**
  * @brief  LL DMA node init structure definition.
  */
typedef struct
{
  /* CTR1 register fields ******************************************************
     If any CTR1 fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CTR1 register fields and enable update
     CTR1 register in UpdateRegisters fields if it is not enabled in the
     previous node.

  */
#if defined (CPU_IN_SECURE_STATE)
  uint32_t DestSecure;           /*!< This field specify the destination secure.
                                      This parameter can be a value of @ref DMA_LL_EC_DESTINATION_SECURITY_ATTRIBUTE. */
#endif /* CPU_IN_SECURE_STATE */

  uint32_t DestAllocatedPort;    /*!< This field specify the destination allocated port.
                                      This parameter can be a value of @ref DMA_LL_EC_DESTINATION_ALLOCATED_PORT.     */

  uint32_t DestWordExchange;     /*!< This field specify the destination word exchange.
                                      This parameter can be a value of @ref DMA_LL_EC_DEST_WORD_EXCHANGE.             */

  uint32_t DestHWordExchange;    /*!< This field specify the destination half word exchange.
                                      This parameter can be a value of @ref DMA_LL_EC_DEST_HALFWORD_EXCHANGE.         */

  uint32_t DestByteExchange;     /*!< This field specify the destination byte exchange.
                                      This parameter can be a value of @ref DMA_LL_EC_DEST_BYTE_EXCHANGE.             */

  uint32_t DestBurstLength;      /*!< This field specify the destination burst length of transfer in bytes.
                                      This parameter must be a value between Min_Data = 1 and Max_Data = 64.          */

  uint32_t DestIncMode;          /*!< This field specify the destination increment mode.
                                      This parameter can be a value of @ref DMA_LL_EC_DESTINATION_INCREMENT_MODE.     */

  uint32_t DestDataWidth;        /*!< This field specify the destination data width.
                                      This parameter can be a value of @ref DMA_LL_EC_DESTINATION_DATA_WIDTH.         */

#if defined (CPU_IN_SECURE_STATE)
  uint32_t SrcSecure;            /*!< This field specify the source secure.
                                      This parameter can be a value of @ref DMA_LL_EC_SOURCE_SECURITY_ATTRIBUTE.      */
#endif /* CPU_IN_SECURE_STATE */

  uint32_t SrcAllocatedPort;     /*!< This field specify the source allocated port.
                                      This parameter can be a value of @ref DMA_LL_EC_SOURCE_ALLOCATED_PORT.          */

  uint32_t SrcByteExchange;      /*!< This field specify the source byte exchange.
                                      This parameter can be a value of @ref DMA_LL_EC_SRC_BYTE_EXCHANGE.              */

  uint32_t DataAlignment;        /*!< This field specify the transfer data alignment.
                                      This parameter can be a value of @ref DMA_LL_EC_DATA_ALIGNMENT.                 */

  uint32_t SrcBurstLength;       /*!< This field specify the source burst length of transfer in bytes.
                                      This parameter must be a value between Min_Data = 1 and Max_Data = 64.          */

  uint32_t SrcIncMode;           /*!< This field specify the source increment mode.
                                      This parameter can be a value of @ref DMA_LL_EC_SOURCE_INCREMENT_MODE.          */

  uint32_t SrcDataWidth;         /*!< This field specify the source data width.
                                      This parameter can be a value of @ref DMA_LL_EC_SOURCE_DATA_WIDTH.              */


  /* CTR2 register fields ******************************************************
     If any CTR2 fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CTR2 register fields and enable update
     CTR2 register in UpdateRegisters fields if it is not enabled in the
     previous node.

     For all node created, filling all fields is mandatory.
  */
  uint32_t TransferEventMode;    /*!< This field specify the transfer event mode.
                                      This parameter can be a value of @ref DMA_LL_EC_TRANSFER_EVENT_MODE.            */

  uint32_t TriggerPolarity;      /*!< This field specify the trigger event polarity.
                                      This parameter can be a value of @ref DMA_LL_EC_TRIGGER_POLARITY.               */

  uint32_t TriggerSelection;     /*!< This field specify the trigger event selection.
                                      This parameter can be a value of @ref DMA_LL_EC_TRIGGER_SELECTION.              */

  uint32_t TriggerMode;          /*!< This field specify the trigger mode.
                                      This parameter can be a value of @ref DMA_LL_EC_TRIGGER_MODE.                   */

  uint32_t BlkHWRequest;         /*!< This field specify the hardware request unity.
                                      This parameter can be a value of @ref DMA_LL_EC_BLKHW_REQUEST.                  */

  uint32_t Direction;            /*!< This field specify the transfer direction.
                                      This parameter can be a value of @ref DMA_LL_EC_TRANSFER_DIRECTION.             */

  uint32_t Request;              /*!< This field specify the peripheral request selection.
                                      This parameter can be a value of @ref DMA_LL_EC_REQUEST_SELECTION.              */

  uint32_t Mode;                  /*!< This field DMA Transfer Mode.
                                      This parameter can be a value of @ref DMA_LL_TRANSFER_MODE.                     */

  /* CBR1 register fields ******************************************************
     If any CBR1 fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CBR1 register fields and enable update
     CBR1 register in UpdateRegisters fields if it is not enabled in the
     previous node.

     If the node to be created is not for 2D addressing channels, there is no
     need to fill the following fields for CBR1 register :
     - BlkReptDestAddrUpdate.
     - BlkRptSrcAddrUpdate.
     - DestAddrUpdate.
     - SrcAddrUpdate.
     - BlkRptCount.
  */
  uint32_t BlkRptDestAddrUpdateMode; /*!< This field specifies the block repeat destination address update mode.
                                          This parameter can be a value of
                                          @ref DMA_LL_EC_BLK_RPT_DEST_ADDR_UPDATE_MODE.                               */

  uint32_t BlkRptSrcAddrUpdateMode;  /*!< This field specifies the block repeat source address update mode.
                                          This parameter can be a value of
                                          @ref DMA_LL_EC_BLK_RPT_SRC_ADDR_UPDATE_MODE.                                */

  uint32_t DestAddrUpdateMode;       /*!< This field specify the Destination address update mode.
                                          This parameter can be a value of @ref DMA_LL_EC_DEST_ADDR_UPDATE_MODE.      */

  uint32_t SrcAddrUpdateMode;        /*!< This field specify the Source address update mode.
                                          This parameter can be a value of @ref DMA_LL_EC_SRC_ADDR_UPDATE_MODE.       */

  uint32_t BlkRptCount;              /*!< This field specify the number of repetitions of the current block.
                                          This parameter can be a value between 1 and 2048 Min_Data = 0
                                          and Max_Data = 0x000007FF.                                                  */

  uint32_t BlkDataLength;            /*!< This field specify the length of a block transfer in bytes.
                                          This parameter must be a value between Min_Data = 0
                                          and Max_Data = 0x0000FFFF.                                                  */

  /* CSAR register fields ******************************************************
     If any CSAR fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CSAR register fields and enable update
     CSAR register in UpdateRegisters fields if it is not enabled in the
     previous node.

     For all node created, filling all fields is mandatory.
  */
  uint32_t SrcAddress;              /*!< This field specify the transfer source address.
                                         This parameter must be a value between Min_Data = 0
                                         and Max_Data = 0xFFFFFFFF.                                                   */


  /* CDAR register fields ******************************************************
     If any CDAR fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CDAR register fields and enable update
     CDAR register in UpdateRegisters fields if it is not enabled in the
     previous node.

     For all node created, filling all fields is mandatory.
  */
  uint32_t DestAddress;             /*!< This field specify the transfer destination address.
                                         This parameter must be a value between Min_Data = 0
                                         and Max_Data = 0xFFFFFFFF.                                                   */

  /* CTR3 register fields ******************************************************
     If any CTR3 fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CTR3 register fields and enable update
     CTR3 register in UpdateRegisters fields if it is not enabled in the
     previous node.

     This register is used only for 2D addressing channels.
     If used channel is linear addressing, this register will be overwritten by
     CLLR register in memory.
     When this register is enabled on UpdateRegisters and the selected channel
     is linear addressing, LL APIs will discard this register update in memory.
  */
  uint32_t DestAddrOffset;       /*!< This field specifies the destination address offset.
                                      This parameter can be a value Between 0 to 0x00001FFF.                          */

  uint32_t SrcAddrOffset;        /*!< This field specifies the source address offset.
                                      This parameter can be a value Between 0 to 0x00001FFF.                          */


  /* CBR2 register fields ******************************************************
     If any CBR2 fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CBR2 register fields and enable update
     CBR2 register in UpdateRegisters fields if it is not enabled in the
     previous node.

     This register is used only for 2D addressing channels.
     If used channel is linear addressing, this register will be discarded in
     memory. When this register is enabled on UpdateRegisters and the selected
     channel is linear addressing, LL APIs will discard this register update in
     memory.
  */
  uint32_t BlkRptDestAddrOffset; /*!< This field specifies the block repeat destination address offset.
                                      This parameter can be a value Between 0 to 0x0000FFFF.                          */

  uint32_t BlkRptSrcAddrOffset;  /*!< This field specifies the block repeat source address offset.
                                      This parameter can be a value Between 0 to 0x0000FFFF.                          */

  /* CLLR register fields ******************************************************
     If any CLLR fields need to be updated comparing to previous node, it is
     mandatory to update the new value in CLLR register fields and enable update
     CLLR register in UpdateRegisters fields if it is not enabled in the
     previous node.

     If used channel is linear addressing, there is no need to enable/disable
     CTR3 and CBR2 register in UpdateRegisters fields as they will be discarded
     by LL APIs.
  */
  uint32_t UpdateRegisters;      /*!< Specifies the linked list register update.
                                      This parameter can be a value of @ref DMA_LL_EC_LINKEDLIST_REGISTER_UPDATE.     */

  /* DMA Node type field *******************************************************
     This parameter defines node types as node size and node content varies
     between channels.
     Thanks to this fields, linked list queue could be created independently
     from channel selection. So, one queue could be executed by all DMA channels.
  */
  uint32_t NodeType;             /*!< Specifies the node type to be created.
                                      This parameter can be a value of @ref DMA_LL_EC_LINKEDLIST_NODE_TYPE.           */
} LL_DMA_InitNodeTypeDef;

/**
  * @brief  LL DMA linked list node structure definition.
  * @note   For 2D addressing channels, the maximum node size is :
  *         (4 Bytes * 8 registers = 32 Bytes).
  *         For GPDMA & HPDMA linear addressing channels, the maximum node size is :
  *         (4 Bytes * 6 registers = 24 Bytes).
  */
typedef struct
{
  __IO uint32_t LinkRegisters[8U];

} LL_DMA_LinkNodeTypeDef;
/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_LL_Exported_Constants DMA Exported Constants
  * @{
  */

/** @defgroup DMA_LL_EC_CHANNEL Channel
  * @{
  */
#define LL_DMA_CHANNEL_0   (0x00U)
#define LL_DMA_CHANNEL_1   (0x01U)
#define LL_DMA_CHANNEL_2   (0x02U)
#define LL_DMA_CHANNEL_3   (0x03U)
#define LL_DMA_CHANNEL_4   (0x04U)
#define LL_DMA_CHANNEL_5   (0x05U)
#define LL_DMA_CHANNEL_6   (0x06U)
#define LL_DMA_CHANNEL_7   (0x07U)
#define LL_DMA_CHANNEL_8   (0x08U)
#define LL_DMA_CHANNEL_9   (0x09U)
#define LL_DMA_CHANNEL_10  (0x0AU)
#define LL_DMA_CHANNEL_11  (0x0BU)
#define LL_DMA_CHANNEL_12  (0x0CU)
#define LL_DMA_CHANNEL_13  (0x0DU)
#define LL_DMA_CHANNEL_14  (0x0EU)
#define LL_DMA_CHANNEL_15  (0x0FU)
#if defined (USE_FULL_LL_DRIVER)
#define LL_DMA_CHANNEL_ALL (0x10U)
#endif /* USE_FULL_LL_DRIVER */
/**
  * @}
  */

#if defined (USE_FULL_LL_DRIVER)
/** @defgroup DMA_LL_EC_CLLR_OFFSET CLLR offset
  * @{
  */
#define LL_DMA_CLLR_OFFSET0 (0x00U)
#define LL_DMA_CLLR_OFFSET1 (0x01U)
#define LL_DMA_CLLR_OFFSET2 (0x02U)
#define LL_DMA_CLLR_OFFSET3 (0x03U)
#define LL_DMA_CLLR_OFFSET4 (0x04U)
#define LL_DMA_CLLR_OFFSET5 (0x05U)
#define LL_DMA_CLLR_OFFSET6 (0x06U)
#define LL_DMA_CLLR_OFFSET7 (0x07U)
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/** @defgroup DMA_LL_EC_CID Static Isolation CID
  * @{
  */
#define LL_DMA_CHANNEL_STATIC_CID_0             (0U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID0 */
#define LL_DMA_CHANNEL_STATIC_CID_1             (1U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID1 */
#define LL_DMA_CHANNEL_STATIC_CID_2             (2U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID2 */
#define LL_DMA_CHANNEL_STATIC_CID_3             (3U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID3 */
#define LL_DMA_CHANNEL_STATIC_CID_4             (4U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID4 */
#define LL_DMA_CHANNEL_STATIC_CID_5             (5U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID5 */
#define LL_DMA_CHANNEL_STATIC_CID_6             (6U<<DMA_CCIDCFGR_SCID_Pos)         /*!< Channel is assigned to CID6 */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_PRIORITY_LEVEL Priority Level
  * @{
  */
#define LL_DMA_LOW_PRIORITY_LOW_WEIGHT  0x00000000U    /*!< Priority level : Low Priority, Low Weight  */
#define LL_DMA_LOW_PRIORITY_MID_WEIGHT  DMA_CCR_PRIO_0 /*!< Priority level : Low Priority, Mid Weight  */
#define LL_DMA_LOW_PRIORITY_HIGH_WEIGHT DMA_CCR_PRIO_1 /*!< Priority level : Low Priority, High Weight */
#define LL_DMA_HIGH_PRIORITY            DMA_CCR_PRIO   /*!< Priority level : High Priority             */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_LINKED_LIST_ALLOCATED_PORT Linked List Allocated Port
  * @{
  */
#define LL_DMA_LINK_ALLOCATED_PORT0 0x00000000U /*!< Linked List Allocated Port 0 */
#define LL_DMA_LINK_ALLOCATED_PORT1 DMA_CCR_LAP /*!< Linked List Allocated Port 1 */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_LINK_STEP_MODE Link Step Mode
  * @{
  */
#define LL_DMA_LSM_FULL_EXECUTION  0x00000000U /*!< Channel execute the full linked list        */
#define LL_DMA_LSM_1LINK_EXECUTION DMA_CCR_LSM /*!< Channel execute one node of the linked list */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DEST_WORD_EXCHANGE Destination Word Exchange
  * @{
  */
#define LL_DMA_DEST_WORD_PRESERVE     0x00000000U  /*!< No destination Word exchange when destination data width
                                                        is double-word                                                */
#define LL_DMA_DEST_WORD_EXCHANGE     DMA_CTR1_DWX /*!< Destination Word exchange when destination data width
                                                        is double-word                                                */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DEST_HALFWORD_EXCHANGE Destination Half-Word Exchange
  * @{
  */
#define LL_DMA_DEST_HALFWORD_PRESERVE 0x00000000U  /*!< No destination Half-Word exchange when destination data width
                                                        is word                                                       */
#define LL_DMA_DEST_HALFWORD_EXCHANGE DMA_CTR1_DHX /*!< Destination Half-Word exchange when destination data width
                                                        is word                                                       */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DEST_BYTE_EXCHANGE Destination Byte Exchange
  * @{
  */
#define LL_DMA_DEST_BYTE_PRESERVE 0x00000000U  /*!< No destination Byte exchange when destination data width > Byte */
#define LL_DMA_DEST_BYTE_EXCHANGE DMA_CTR1_DBX /*!< Destination Byte exchange when destination data width > Byte    */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SRC_BYTE_EXCHANGE Source Byte Exchange
  * @{
  */
#define LL_DMA_SRC_BYTE_PRESERVE 0x00000000U  /*!< No source Byte exchange when source data width is word */
#define LL_DMA_SRC_BYTE_EXCHANGE DMA_CTR1_SBX /*!< Source Byte exchange when source data width is word    */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SOURCE_ALLOCATED_PORT Source Allocated Port
  * @{
  */
#define LL_DMA_SRC_ALLOCATED_PORT0 0x00000000U  /*!< Source Allocated Port 0 */
#define LL_DMA_SRC_ALLOCATED_PORT1 DMA_CTR1_SAP /*!< Source Allocated Port 1 */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DESTINATION_ALLOCATED_PORT Destination Allocated Port
  * @{
  */
#define LL_DMA_DEST_ALLOCATED_PORT0 0x00000000U  /*!< Destination Allocated Port 0 */
#define LL_DMA_DEST_ALLOCATED_PORT1 DMA_CTR1_DAP /*!< Destination Allocated Port 1 */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DESTINATION_INCREMENT_MODE Destination Increment Mode
  * @{
  */
#define LL_DMA_DEST_FIXED       0x00000000U   /*!< Destination fixed single/burst       */
#define LL_DMA_DEST_INCREMENT   DMA_CTR1_DINC /*!< Destination incremented single/burst */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DESTINATION_DATA_WIDTH Destination Data Width
  * @{
  */
#define LL_DMA_DEST_DATAWIDTH_BYTE       0x00000000U         /*!< Destination Data Width : Byte       */
#define LL_DMA_DEST_DATAWIDTH_HALFWORD   DMA_CTR1_DDW_LOG2_0 /*!< Destination Data Width : HalfWord   */
#define LL_DMA_DEST_DATAWIDTH_WORD       DMA_CTR1_DDW_LOG2_1 /*!< Destination Data Width : Word       */
#define LL_DMA_DEST_DATAWIDTH_DOUBLEWORD DMA_CTR1_DDW_LOG2   /*!< Destination Data Width : DoubleWord */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DATA_ALIGNMENT Data Alignment
  * @{
  */
#define LL_DMA_DATA_ALIGN_ZEROPADD    0x00000000U    /*!< If src data width < dest data width :
                                                          => Right Aligned padded with 0 up to destination
                                                             data width.
                                                          If src data width > dest data width :
                                                          => Right Aligned Left Truncated down to destination
                                                             data width.                                              */
#define LL_DMA_DATA_ALIGN_SIGNEXTPADD DMA_CTR1_PAM_0 /*!< If src data width < dest data width :
                                                          => Right Aligned padded with sign extended up to destination
                                                             data width.
                                                          If src data width > dest data width :
                                                          => Left Aligned Right Truncated down to the destination
                                                             data width                                               */
#define LL_DMA_DATA_PACK_UNPACK       DMA_CTR1_PAM_1 /*!< If src data width < dest data width :
                                                          => Packed at the destination data width
                                                          If src data width > dest data width :
                                                          => Unpacked at the destination data width                   */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SOURCE_INCREMENT_MODE Source Increment Mode
  * @{
  */
#define LL_DMA_SRC_FIXED     0x00000000U   /*!< Source fixed single/burst */
#define LL_DMA_SRC_INCREMENT DMA_CTR1_SINC /*!< Source incremented single/burst    */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SOURCE_DATA_WIDTH Source Data Width
  * @{
  */
#define LL_DMA_SRC_DATAWIDTH_BYTE        0x00000000U         /*!< Source Data Width : Byte       */
#define LL_DMA_SRC_DATAWIDTH_HALFWORD    DMA_CTR1_SDW_LOG2_0 /*!< Source Data Width : HalfWord   */
#define LL_DMA_SRC_DATAWIDTH_WORD        DMA_CTR1_SDW_LOG2_1 /*!< Source Data Width : Word       */
#define LL_DMA_SRC_DATAWIDTH_DOUBLEWORD  DMA_CTR1_SDW_LOG2   /*!< Source Data Width : DoubleWord */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_BLKHW_REQUEST Block Hardware Request
  * @{
  */
#define LL_DMA_HWREQUEST_SINGLEBURST 0x00000000U   /*!< Hardware request is driven by a peripheral with a hardware
                                                        request/acknowledge protocol at a burst level                 */
#define LL_DMA_HWREQUEST_BLK         DMA_CTR2_BREQ /*!< Hardware request is driven by a peripheral with a hardware
                                                        request/acknowledge protocol at a block level                 */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_TRANSFER_EVENT_MODE Transfer Event Mode
  * @{
  */
#define LL_DMA_TCEM_BLK_TRANSFER         0x00000000U       /*!< The TC (and the HT) event is generated at the
                                                                (respectively half) end of each block                 */
#define LL_DMA_TCEM_RPT_BLK_TRANSFER     DMA_CTR2_TCEM_0   /*!< The TC (and the HT) event is generated at the
                                                                (respectively half) end of the repeated block         */
#define LL_DMA_TCEM_EACH_LLITEM_TRANSFER DMA_CTR2_TCEM_1   /*!< The TC (and the HT) event is generated at the
                                                                (respectively half) end of each linked-list item      */
#define LL_DMA_TCEM_LAST_LLITEM_TRANSFER DMA_CTR2_TCEM     /*!< The TC (and the HT) event is generated at the
                                                                (respectively half) end of the last linked-list item  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_TRIGGER_POLARITY Trigger Polarity
  * @{
  */
#define LL_DMA_TRIG_POLARITY_MASKED    0x00000000U         /*!< No trigger of the selected DMA request.
                                                                Masked trigger event                                  */
#define LL_DMA_TRIG_POLARITY_RISING    DMA_CTR2_TRIGPOL_0  /*!< Trigger of the selected DMA request on the rising
                                                                edge of the selected trigger event input              */
#define LL_DMA_TRIG_POLARITY_FALLING   DMA_CTR2_TRIGPOL_1  /*!< Trigger of the selected DMA request on the falling
                                                                edge of the selected trigger event input              */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_TRIGGER_MODE Transfer Trigger Mode
  * @{
  */
#define LL_DMA_TRIGM_BLK_TRANSFER        0x00000000U      /*!< A block transfer is conditioned by (at least)
                                                               one hit trigger                                        */
#define LL_DMA_TRIGM_RPT_BLK_TRANSFER    DMA_CTR2_TRIGM_0 /*!< A repeated block transfer is conditioned by (at least)
                                                               one hit trigger                                        */
#define LL_DMA_TRIGM_LLI_LINK_TRANSFER   DMA_CTR2_TRIGM_1 /*!< A LLI link transfer is conditioned by (at least)
                                                               one hit trigger                                        */
#define LL_DMA_TRIGM_SINGLBURST_TRANSFER DMA_CTR2_TRIGM   /*!< A Single/Burst transfer is conditioned by (at least)
                                                               one hit trigger                                        */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_TRANSFER_DIRECTION Transfer Direction
  * @{
  */
#define LL_DMA_DIRECTION_MEMORY_TO_MEMORY DMA_CTR2_SWREQ /*!< Memory to memory direction     */
#define LL_DMA_DIRECTION_PERIPH_TO_MEMORY 0x00000000U    /*!< Peripheral to memory direction */
#define LL_DMA_DIRECTION_MEMORY_TO_PERIPH DMA_CTR2_DREQ  /*!< Memory to peripheral direction */
/**
  * @}
  */

/** @defgroup DMA_LL_TRANSFER_MODE Transfer Mode
  * @{
  */
#define LL_DMA_NORMAL          0x00000000U       /*!< Normal DMA transfer                     */
#define LL_DMA_PFCTRL       DMA_CTR2_PFREQ       /*!< HW request peripheral flow control mode */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_BLK_RPT_SRC_ADDR_UPDATE_MODE Block Repeat Source Address Update Mode
  * @{
  */
#define LL_DMA_BLKRPT_SRC_ADDR_INCREMENT 0x00000000U     /*!< Source address pointer is incremented after each block
                                                              transfer by source update value                         */
#define LL_DMA_BLKRPT_SRC_ADDR_DECREMENT DMA_CBR1_BRSDEC /*!< Source address pointer is decremented after each block
                                                              transfer by source update value                         */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_BLK_RPT_DEST_ADDR_UPDATE_MODE Block Repeat Destination Address Update Mode
  * @{
  */
#define LL_DMA_BLKRPT_DEST_ADDR_INCREMENT 0x00000000U     /*!< Destination address is incremented after each block
                                                               transfer by destination update value                   */
#define LL_DMA_BLKRPT_DEST_ADDR_DECREMENT DMA_CBR1_BRDDEC /*!< Destination address is decremented after each block
                                                               transfer by destination update value                   */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SRC_ADDR_UPDATE_MODE Burst Source Address Update Mode
  * @{
  */
#define LL_DMA_BURST_SRC_ADDR_INCREMENT 0x00000000U   /*!< Source address pointer is incremented after each burst
                                                           transfer by source update value                            */
#define LL_DMA_BURST_SRC_ADDR_DECREMENT DMA_CBR1_SDEC /*!< Source address pointer is decremented after each burst
                                                           transfer by source update value                            */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DEST_ADDR_UPDATE_MODE Burst Destination Address Update Mode
  * @{
  */
#define LL_DMA_BURST_DEST_ADDR_INCREMENT 0x00000000U   /*!< Destination address pointer is incremented after each
                                                            burst transfer by destination update value                */
#define LL_DMA_BURST_DEST_ADDR_DECREMENT DMA_CBR1_DDEC /*!< Destination address pointer is decremented after each
                                                            burst transfer by destination update value                */
/**
  * @}
  */

#if defined (CPU_IN_SECURE_STATE)
/** @defgroup DMA_LL_EC_SOURCE_SECURITY_ATTRIBUTE Source Security Attribute
  * @{
  */
#define LL_DMA_CHANNEL_NSEC 0x00000000U /*!< NSecure channel */
#define LL_DMA_CHANNEL_SEC  0x00000001U /*!< Secure channel  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_SOURCE_SECURITY_ATTRIBUTE Source Security Attribute
  * @{
  */
#define LL_DMA_CHANNEL_SRC_NSEC 0x00000000U   /*!< NSecure transfer from the source */
#define LL_DMA_CHANNEL_SRC_SEC  DMA_CTR1_SSEC /*!< Secure transfer from the source  */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_DESTINATION_SECURITY_ATTRIBUTE Destination Security Attribute
  * @{
  */
#define LL_DMA_CHANNEL_DEST_NSEC 0x00000000U   /*!< NSecure transfer from the destination */
#define LL_DMA_CHANNEL_DEST_SEC  DMA_CTR1_DSEC /*!< Secure transfer from the destination  */
/**
  * @}
  */
#endif /* CPU_IN_SECURE_STATE */

/** @defgroup DMA_LL_EC_LINKEDLIST_NODE_TYPE Linked list node type
  * @{
  */
#define LL_DMA_GPDMA_LINEAR_NODE 0x01U /*!< GPDMA node : linear addressing node      */
#define LL_DMA_GPDMA_2D_NODE     0x02U /*!< GPDMA node : 2 dimension addressing node */
#define LL_DMA_HPDMA_LINEAR_NODE 0x04U /*!< HPDMA node : linear addressing node      */
#define LL_DMA_HPDMA_2D_NODE     0x08U /*!< HPDMA node : 2 dimension addressing node */

/**
  * @}
  */

/** @defgroup DMA_LL_EC_LINKEDLIST_REGISTER_UPDATE Linked list register update
  * @{
  */
#define LL_DMA_UPDATE_CTR1 DMA_CLLR_UT1 /*!< Update CTR1 register from memory :
                                             available for all DMA channels                */
#define LL_DMA_UPDATE_CTR2 DMA_CLLR_UT2 /*!< Update CTR2 register from memory :
                                             available for all DMA channels                */
#define LL_DMA_UPDATE_CBR1 DMA_CLLR_UB1 /*!< Update CBR1 register from memory :
                                             available for all DMA channels                */
#define LL_DMA_UPDATE_CSAR DMA_CLLR_USA /*!< Update CSAR register from memory :
                                             available for all DMA channels                */
#define LL_DMA_UPDATE_CDAR DMA_CLLR_UDA /*!< Update CDAR register from memory :
                                             available for all DMA channels                */
#define LL_DMA_UPDATE_CTR3 DMA_CLLR_UT3 /*!< Update CTR3 register from memory :
                                             available only for 2D addressing DMA channels */
#define LL_DMA_UPDATE_CBR2 DMA_CLLR_UB2 /*!< Update CBR2 register from memory :
                                             available only for 2D addressing DMA channels */
#define LL_DMA_UPDATE_CLLR DMA_CLLR_ULL /*!< Update CLLR register from memory :
                                             available for all DMA channels                */
/**
  * @}
  */

/** @defgroup DMA_LL_EC_REQUEST_SELECTION Request Selection
  * @{
  */
/* HPDMA1 requests */
#define LL_HPDMA1_REQUEST_JPEG_RX       0U       /*!< HPDMA1 HW request is JPEG_RX        */
#define LL_HPDMA1_REQUEST_JPEG_TX       1U       /*!< HPDMA1 HW request is JPEG_TX        */
#define LL_HPDMA1_REQUEST_OCTOSPI1      2U       /*!< HPDMA1 HW request is OCTOSPI1       */
#define LL_HPDMA1_REQUEST_OCTOSPI2      3U       /*!< HPDMA1 HW request is OCTOSPI2       */
#define LL_HPDMA1_REQUEST_OCTOSPI3      4U       /*!< HPDMA1 HW request is OCTOSPI3       */
#define LL_HPDMA1_REQUEST_FMC2_TXRX     5U       /*!< HPDMA1 HW request is FMC2_TXRX      */
#define LL_HPDMA1_REQUEST_FMC2_BCH      6U       /*!< HPDMA1 HW request is FMC2_BCH       */
#define LL_HPDMA1_REQUEST_ADC1          7U       /*!< HPDMA1 HW request is ADC1           */
#define LL_HPDMA1_REQUEST_ADC2          8U       /*!< HPDMA1 HW request is ADC2           */
#define LL_HPDMA1_REQUEST_CRYP_IN       9U       /*!< HPDMA1 HW request is CRYP_IN        */
#define LL_HPDMA1_REQUEST_CRYP_OUT      10U      /*!< HPDMA1 HW request is CRYP_OUT       */
#define LL_HPDMA1_REQUEST_SAES_OUT      11U      /*!< HPDMA1 HW request is SAES_OUT       */
#define LL_HPDMA1_REQUEST_SAES_IN       12U      /*!< HPDMA1 HW request is SAES_IN        */
#define LL_HPDMA1_REQUEST_HASH_IN       13U      /*!< HPDMA1 HW request is HASH_IN        */

#define LL_HPDMA1_REQUEST_TIM1_CH1      14U      /*!< HPDMA1 HW request is TIM1_CH1       */
#define LL_HPDMA1_REQUEST_TIM1_CH2      15U      /*!< HPDMA1 HW request is TIM1_CH2       */
#define LL_HPDMA1_REQUEST_TIM1_CH3      16U      /*!< HPDMA1 HW request is TIM1_CH3       */
#define LL_HPDMA1_REQUEST_TIM1_CH4      17U      /*!< HPDMA1 HW request is TIM1_CH4       */
#define LL_HPDMA1_REQUEST_TIM1_UP       18U      /*!< HPDMA1 HW request is TIM1_UP        */
#define LL_HPDMA1_REQUEST_TIM1_TRIG     19U      /*!< HPDMA1 HW request is TIM1_TRIG      */
#define LL_HPDMA1_REQUEST_TIM1_COM      20U      /*!< HPDMA1 HW request is TIM1_COM       */

#define LL_HPDMA1_REQUEST_TIM2_CH1      21U      /*!< HPDMA1 HW request is TIM2_CH1       */
#define LL_HPDMA1_REQUEST_TIM2_CH2      22U      /*!< HPDMA1 HW request is TIM2_CH2       */
#define LL_HPDMA1_REQUEST_TIM2_CH3      23U      /*!< HPDMA1 HW request is TIM2_CH3       */
#define LL_HPDMA1_REQUEST_TIM2_CH4      24U      /*!< HPDMA1 HW request is TIM2_CH4       */
#define LL_HPDMA1_REQUEST_TIM2_UP       25U      /*!< HPDMA1 HW request is TIM2_UP        */
#define LL_HPDMA1_REQUEST_TIM2_TRIG     26U      /*!< HPDMA1 HW request is TIM2_TRIG      */

#define LL_HPDMA1_REQUEST_TIM3_CH1      27U      /*!< HPDMA1 HW request is TIM3_CH1       */
#define LL_HPDMA1_REQUEST_TIM3_CH2      28U      /*!< HPDMA1 HW request is TIM3_CH2       */
#define LL_HPDMA1_REQUEST_TIM3_CH3      29U      /*!< HPDMA1 HW request is TIM3_CH3       */
#define LL_HPDMA1_REQUEST_TIM3_CH4      30U      /*!< HPDMA1 HW request is TIM3_CH4       */
#define LL_HPDMA1_REQUEST_TIM3_UP       31U      /*!< HPDMA1 HW request is TIM3_UP        */
#define LL_HPDMA1_REQUEST_TIM3_TRIG     32U      /*!< HPDMA1 HW request is TIM3_TRIG      */

#define LL_HPDMA1_REQUEST_TIM4_CH1      33U      /*!< HPDMA1 HW request is TIM4_CH1       */
#define LL_HPDMA1_REQUEST_TIM4_CH2      34U      /*!< HPDMA1 HW request is TIM4_CH2       */
#define LL_HPDMA1_REQUEST_TIM4_CH3      35U      /*!< HPDMA1 HW request is TIM4_CH3       */
#define LL_HPDMA1_REQUEST_TIM4_CH4      36U      /*!< HPDMA1 HW request is TIM4_CH4       */
#define LL_HPDMA1_REQUEST_TIM4_UP       37U      /*!< HPDMA1 HW request is TIM4_UP        */
#define LL_HPDMA1_REQUEST_TIM4_TRIG     38U      /*!< HPDMA1 HW request is TIM4_TRIG      */

#define LL_HPDMA1_REQUEST_TIM5_CH1      39U      /*!< HPDMA1 HW request is TIM5_CH1       */
#define LL_HPDMA1_REQUEST_TIM5_CH2      40U      /*!< HPDMA1 HW request is TIM5_CH2       */
#define LL_HPDMA1_REQUEST_TIM5_CH3      41U      /*!< HPDMA1 HW request is TIM5_CH3       */
#define LL_HPDMA1_REQUEST_TIM5_CH4      42U      /*!< HPDMA1 HW request is TIM5_CH4       */
#define LL_HPDMA1_REQUEST_TIM5_UP       43U      /*!< HPDMA1 HW request is TIM5_UP        */
#define LL_HPDMA1_REQUEST_TIM5_TRIG     44U      /*!< HPDMA1 HW request is TIM5_TRIG      */

#define LL_HPDMA1_REQUEST_TIM6_UP       45U      /*!< HPDMA1 HW request is TIM6_UP        */
#define LL_HPDMA1_REQUEST_TIM7_UP       46U      /*!< HPDMA1 HW request is TIM7_UP        */

#define LL_HPDMA1_REQUEST_TIM8_CH1      47U      /*!< HPDMA1 HW request is TIM8_CH1       */
#define LL_HPDMA1_REQUEST_TIM8_CH2      48U      /*!< HPDMA1 HW request is TIM8_CH2       */
#define LL_HPDMA1_REQUEST_TIM8_CH3      49U      /*!< HPDMA1 HW request is TIM8_CH3       */
#define LL_HPDMA1_REQUEST_TIM8_CH4      50U      /*!< HPDMA1 HW request is TIM8_CH4       */
#define LL_HPDMA1_REQUEST_TIM8_UP       51U      /*!< HPDMA1 HW request is TIM8_UP        */
#define LL_HPDMA1_REQUEST_TIM8_TRIG     52U      /*!< HPDMA1 HW request is TIM8_TRIG      */
#define LL_HPDMA1_REQUEST_TIM8_COM      53U      /*!< HPDMA1 HW request is TIM8_COM       */

/* reserved                             54U                                               */
/* reserved                             55U                                               */

#define LL_HPDMA1_REQUEST_TIM15_CH1     56U      /*!< HPDMA1 HW request is TIM15_CH1      */
#define LL_HPDMA1_REQUEST_TIM15_CH2     57U      /*!< HPDMA1 HW request is TIM15_CH2      */
#define LL_HPDMA1_REQUEST_TIM15_UP      58U      /*!< HPDMA1 HW request is TIM15_UP       */
#define LL_HPDMA1_REQUEST_TIM15_TRIG    59U      /*!< HPDMA1 HW request is TIM15_TRIG     */
#define LL_HPDMA1_REQUEST_TIM15_COM     60U      /*!< HPDMA1 HW request is TIM15_COM      */

#define LL_HPDMA1_REQUEST_TIM16_CH1     61U      /*!< HPDMA1 HW request is TIM16_CH1      */
#define LL_HPDMA1_REQUEST_TIM16_UP      62U      /*!< HPDMA1 HW request is TIM16_UP       */
#define LL_HPDMA1_REQUEST_TIM16_COM     63U      /*!< HPDMA1 HW request is TIM16_COM      */

#define LL_HPDMA1_REQUEST_TIM17_CH1     64U      /*!< HPDMA1 HW request is TIM17_CH1      */
#define LL_HPDMA1_REQUEST_TIM17_UP      65U      /*!< HPDMA1 HW request is TIM17_UP       */
#define LL_HPDMA1_REQUEST_TIM17_COM     66U      /*!< HPDMA1 HW request is TIM17_COM      */

#define LL_HPDMA1_REQUEST_TIM18_CH1     67U      /*!< HPDMA1 HW request is TIM18_CH1      */
#define LL_HPDMA1_REQUEST_TIM18_UP      68U      /*!< HPDMA1 HW request is TIM18_UP       */
#define LL_HPDMA1_REQUEST_TIM18_COM     69U      /*!< HPDMA1 HW request is TIM18_COM      */

#define LL_HPDMA1_REQUEST_LPTIM1_IC1    70U      /*!< HPDMA1 HW request is LPTIM1_IC1     */
#define LL_HPDMA1_REQUEST_LPTIM1_IC2    71U      /*!< HPDMA1 HW request is LPTIM1_IC2     */
#define LL_HPDMA1_REQUEST_LPTIM1_UE     72U      /*!< HPDMA1 HW request is LPTIM1_UE      */
#define LL_HPDMA1_REQUEST_LPTIM2_IC1    73U      /*!< HPDMA1 HW request is LPTIM2_IC1     */
#define LL_HPDMA1_REQUEST_LPTIM2_IC2    74U      /*!< HPDMA1 HW request is LPTIM2_IC2     */
#define LL_HPDMA1_REQUEST_LPTIM2_UE     75U      /*!< HPDMA1 HW request is LPTIM2_UE      */
#define LL_HPDMA1_REQUEST_LPTIM3_IC1    76U      /*!< HPDMA1 HW request is LPTIM3_IC1     */
#define LL_HPDMA1_REQUEST_LPTIM3_IC2    77U      /*!< HPDMA1 HW request is LPTIM3_IC2     */
#define LL_HPDMA1_REQUEST_LPTIM3_UE     78U      /*!< HPDMA1 HW request is LPTIM3_UE      */

#define LL_HPDMA1_REQUEST_SPI1_RX       79U      /*!< HPDMA1 HW request is SPI1_RX        */
#define LL_HPDMA1_REQUEST_SPI1_TX       80U      /*!< HPDMA1 HW request is SPI1_TX        */
#define LL_HPDMA1_REQUEST_SPI2_RX       81U      /*!< HPDMA1 HW request is SPI2_RX        */
#define LL_HPDMA1_REQUEST_SPI2_TX       82U      /*!< HPDMA1 HW request is SPI2_TX        */
#define LL_HPDMA1_REQUEST_SPI3_RX       83U      /*!< HPDMA1 HW request is SPI3_RX        */
#define LL_HPDMA1_REQUEST_SPI3_TX       84U      /*!< HPDMA1 HW request is SPI3_TX        */
#define LL_HPDMA1_REQUEST_SPI4_RX       85U      /*!< HPDMA1 HW request is SPI4_RX        */
#define LL_HPDMA1_REQUEST_SPI4_TX       86U      /*!< HPDMA1 HW request is SPI4_TX        */
#define LL_HPDMA1_REQUEST_SPI5_RX       87U      /*!< HPDMA1 HW request is SPI5_RX        */
#define LL_HPDMA1_REQUEST_SPI5_TX       88U      /*!< HPDMA1 HW request is SPI5_TX        */
#define LL_HPDMA1_REQUEST_SPI6_RX       89U      /*!< HPDMA1 HW request is SPI6_RX        */
#define LL_HPDMA1_REQUEST_SPI6_TX       90U      /*!< HPDMA1 HW request is SPI6_TX        */

#define LL_HPDMA1_REQUEST_SAI1_A        91U      /*!< HPDMA1 HW request is SAI1_A         */
#define LL_HPDMA1_REQUEST_SAI1_B        92U      /*!< HPDMA1 HW request is SAI1_B         */
#define LL_HPDMA1_REQUEST_SAI2_A        93U      /*!< HPDMA1 HW request is SAI2_A         */
#define LL_HPDMA1_REQUEST_SAI2_B        94U      /*!< HPDMA1 HW request is SAI2_B         */

#define LL_HPDMA1_REQUEST_I2C1_RX       95U      /*!< HPDMA1 HW request is I2C1_RX        */
#define LL_HPDMA1_REQUEST_I2C1_TX       96U      /*!< HPDMA1 HW request is I2C1_TX        */
#define LL_HPDMA1_REQUEST_I2C2_RX       97U      /*!< HPDMA1 HW request is I2C2_RX        */
#define LL_HPDMA1_REQUEST_I2C2_TX       98U      /*!< HPDMA1 HW request is I2C2_TX        */
#define LL_HPDMA1_REQUEST_I2C3_RX       99U      /*!< HPDMA1 HW request is I2C3_RX        */
#define LL_HPDMA1_REQUEST_I2C3_TX       100U     /*!< HPDMA1 HW request is I2C3_TX        */

#define LL_HPDMA1_REQUEST_I2C4_RX       101U     /*!< HPDMA1 HW request is I2C4_RX        */
#define LL_HPDMA1_REQUEST_I2C4_TX       102U     /*!< HPDMA1 HW request is I2C4_TX        */

#define LL_HPDMA1_REQUEST_I3C1_RX       103U     /*!< HPDMA1 HW request is I3C1_RX        */
#define LL_HPDMA1_REQUEST_I3C1_TX       104U     /*!< HPDMA1 HW request is I3C1_TX        */
#define LL_HPDMA1_REQUEST_I3C2_RX       105U     /*!< HPDMA1 HW request is I3C2_RX        */
#define LL_HPDMA1_REQUEST_I3C2_TX       106U     /*!< HPDMA1 HW request is I3C2_TX        */

#define LL_HPDMA1_REQUEST_USART1_RX     107U     /*!< HPDMA1 HW request is USART1_RX      */
#define LL_HPDMA1_REQUEST_USART1_TX     108U     /*!< HPDMA1 HW request is USART1_TX      */
#define LL_HPDMA1_REQUEST_USART2_RX     109U     /*!< HPDMA1 HW request is USART2_RX      */
#define LL_HPDMA1_REQUEST_USART2_TX     110U     /*!< HPDMA1 HW request is USART2_TX      */
#define LL_HPDMA1_REQUEST_USART3_RX     111U     /*!< HPDMA1 HW request is USART3_RX      */
#define LL_HPDMA1_REQUEST_USART3_TX     112U     /*!< HPDMA1 HW request is USART3_TX      */
#define LL_HPDMA1_REQUEST_UART4_RX      113U     /*!< HPDMA1 HW request is UART4_RX       */
#define LL_HPDMA1_REQUEST_UART4_TX      114U     /*!< HPDMA1 HW request is UART4_TX       */
#define LL_HPDMA1_REQUEST_UART5_RX      115U     /*!< HPDMA1 HW request is UART5_RX       */
#define LL_HPDMA1_REQUEST_UART5_TX      116U     /*!< HPDMA1 HW request is UART5_TX       */
#define LL_HPDMA1_REQUEST_USART6_RX     117U     /*!< HPDMA1 HW request is USART6_RX      */
#define LL_HPDMA1_REQUEST_USART6_TX     118U     /*!< HPDMA1 HW request is USART6_TX      */
#define LL_HPDMA1_REQUEST_UART7_RX      119U     /*!< HPDMA1 HW request is UART7_RX       */
#define LL_HPDMA1_REQUEST_UART7_TX      120U     /*!< HPDMA1 HW request is UART7_TX       */
#define LL_HPDMA1_REQUEST_UART8_RX      121U     /*!< HPDMA1 HW request is UART8_RX       */
#define LL_HPDMA1_REQUEST_UART8_TX      122U     /*!< HPDMA1 HW request is UART8_TX       */
#define LL_HPDMA1_REQUEST_UART9_RX      123U     /*!< HPDMA1 HW request is UART9_RX       */
#define LL_HPDMA1_REQUEST_UART9_TX      124U     /*!< HPDMA1 HW request is UART9_TX       */
#define LL_HPDMA1_REQUEST_USART10_RX    125U     /*!< HPDMA1 HW request is USART10_RX     */
#define LL_HPDMA1_REQUEST_USART10_TX    126U     /*!< HPDMA1 HW request is USART10_TX     */

#define LL_HPDMA1_REQUEST_LPUART1_RX    127U     /*!< HPDMA1 HW request is LPUART1_RX     */
#define LL_HPDMA1_REQUEST_LPUART1_TX    128U     /*!< HPDMA1 HW request is LPUART1_TX     */

#define LL_HPDMA1_REQUEST_SPDIFRX_CS    129U     /*!< HPDMA1 HW request is SPDIFRX_CS     */
#define LL_HPDMA1_REQUEST_SPDIFRX_DT    130U     /*!< HPDMA1 HW request is SPDIFRX_DT     */

#define LL_HPDMA1_REQUEST_ADF1_FLT0     131U     /*!< HPDMA1 HW request is ADF1_FLT0      */

#define LL_HPDMA1_REQUEST_MDF1_FLT0     132U     /*!< HPDMA1 HW request is MDF1_FLT0      */
#define LL_HPDMA1_REQUEST_MDF1_FLT1     133U     /*!< HPDMA1 HW request is MDF1_FLT1      */
#define LL_HPDMA1_REQUEST_MDF1_FLT2     134U     /*!< HPDMA1 HW request is MDF1_FLT2      */
#define LL_HPDMA1_REQUEST_MDF1_FLT3     135U     /*!< HPDMA1 HW request is MDF1_FLT3      */
#define LL_HPDMA1_REQUEST_MDF1_FLT4     136U     /*!< HPDMA1 HW request is MDF1_FLT4      */
#define LL_HPDMA1_REQUEST_MDF1_FLT5     137U     /*!< HPDMA1 HW request is MDF1_FLT5      */

#define LL_HPDMA1_REQUEST_UCPD1_TX      138U     /*!< HPDMA1 HW request is UCPD1_TX       */
#define LL_HPDMA1_REQUEST_UCPD1_RX      139U     /*!< HPDMA1 HW request is UCPD1_RX       */

#define LL_HPDMA1_REQUEST_DCMI_PSSI     140U     /*!< HPDMA1 HW request is DCMI_PSSI      */

#define LL_HPDMA1_REQUEST_I3C1_TC       141U     /*!< HPDMA1 HW request is I3C1_TC        */
#define LL_HPDMA1_REQUEST_I3C1_RS       142U     /*!< HPDMA1 HW request is I3C1_RS        */

#define LL_HPDMA1_REQUEST_I3C2_TC       143U     /*!< HPDMA1 HW request is I3C2_TC        */
#define LL_HPDMA1_REQUEST_I3C2_RS       144U     /*!< HPDMA1 HW request is I3C2_RS        */


/* GPDMA1 requests */
#define LL_GPDMA1_REQUEST_JPEG_RX       0U       /*!< GPDMA1 HW request is JPEG_DMA_RX    */
#define LL_GPDMA1_REQUEST_JPEG_TX       1U       /*!< GPDMA1 HW request is JPEG_DMA_TX    */
#define LL_GPDMA1_REQUEST_OCTOSPI1      2U       /*!< GPDMA1 HW request is OCTOSPI1       */
#define LL_GPDMA1_REQUEST_OCTOSPI2      3U       /*!< GPDMA1 HW request is OCTOSPI2       */
#define LL_GPDMA1_REQUEST_OCTOSPI3      4U       /*!< GPDMA1 HW request is OCTOSPI3       */
#define LL_GPDMA1_REQUEST_FMC2_TXRX     5U       /*!< GPDMA1 HW request is FMC2_TXRX      */
#define LL_GPDMA1_REQUEST_FMC2_BCH      6U       /*!< GPDMA1 HW request is FMC2_BCH       */
#define LL_GPDMA1_REQUEST_ADC1          7U       /*!< GPDMA1 HW request is ADC1           */
#define LL_GPDMA1_REQUEST_ADC2          8U       /*!< GPDMA1 HW request is ADC2           */
#define LL_GPDMA1_REQUEST_CRYP_IN       9U       /*!< GPDMA1 HW request is CRYP_IN        */
#define LL_GPDMA1_REQUEST_CRYP_OUT      10U      /*!< GPDMA1 HW request is CRYP_OUT       */
#define LL_GPDMA1_REQUEST_SAES_OUT      11U      /*!< GPDMA1 HW request is SAES_OUT       */
#define LL_GPDMA1_REQUEST_SAES_IN       12U      /*!< GPDMA1 HW request is SAES_IN        */
#define LL_GPDMA1_REQUEST_HASH_IN       13U      /*!< GPDMA1 HW request is HASH_IN        */

#define LL_GPDMA1_REQUEST_TIM1_CH1      14U      /*!< GPDMA1 HW request is TIM1_CH1       */
#define LL_GPDMA1_REQUEST_TIM1_CH2      15U      /*!< GPDMA1 HW request is TIM1_CH2       */
#define LL_GPDMA1_REQUEST_TIM1_CH3      16U      /*!< GPDMA1 HW request is TIM1_CH3       */
#define LL_GPDMA1_REQUEST_TIM1_CH4      17U      /*!< GPDMA1 HW request is TIM1_CH4       */
#define LL_GPDMA1_REQUEST_TIM1_UP       18U      /*!< GPDMA1 HW request is TIM1_UP        */
#define LL_GPDMA1_REQUEST_TIM1_TRIG     19U      /*!< GPDMA1 HW request is TIM1_TRIG      */
#define LL_GPDMA1_REQUEST_TIM1_COM      20U      /*!< GPDMA1 HW request is TIM1_COM       */

#define LL_GPDMA1_REQUEST_TIM2_CH1      21U      /*!< GPDMA1 HW request is TIM2_CH1       */
#define LL_GPDMA1_REQUEST_TIM2_CH2      22U      /*!< GPDMA1 HW request is TIM2_CH2       */
#define LL_GPDMA1_REQUEST_TIM2_CH3      23U      /*!< GPDMA1 HW request is TIM2_CH3       */
#define LL_GPDMA1_REQUEST_TIM2_CH4      24U      /*!< GPDMA1 HW request is TIM2_CH4       */
#define LL_GPDMA1_REQUEST_TIM2_UP       25U      /*!< GPDMA1 HW request is TIM2_UP        */
#define LL_GPDMA1_REQUEST_TIM2_TRIG     26U      /*!< GPDMA1 HW request is TIM2_TRIG      */

#define LL_GPDMA1_REQUEST_TIM3_CH1      27U      /*!< GPDMA1 HW request is TIM3_CH1       */
#define LL_GPDMA1_REQUEST_TIM3_CH2      28U      /*!< GPDMA1 HW request is TIM3_CH2       */
#define LL_GPDMA1_REQUEST_TIM3_CH3      29U      /*!< GPDMA1 HW request is TIM3_CH3       */
#define LL_GPDMA1_REQUEST_TIM3_CH4      30U      /*!< GPDMA1 HW request is TIM3_CH4       */
#define LL_GPDMA1_REQUEST_TIM3_UP       31U      /*!< GPDMA1 HW request is TIM3_UP        */
#define LL_GPDMA1_REQUEST_TIM3_TRIG     32U      /*!< GPDMA1 HW request is TIM3_TRIG      */

#define LL_GPDMA1_REQUEST_TIM4_CH1      33U      /*!< GPDMA1 HW request is TIM4_CH1       */
#define LL_GPDMA1_REQUEST_TIM4_CH2      34U      /*!< GPDMA1 HW request is TIM4_CH2       */
#define LL_GPDMA1_REQUEST_TIM4_CH3      35U      /*!< GPDMA1 HW request is TIM4_CH3       */
#define LL_GPDMA1_REQUEST_TIM4_CH4      36U      /*!< GPDMA1 HW request is TIM4_CH4       */
#define LL_GPDMA1_REQUEST_TIM4_UP       37U      /*!< GPDMA1 HW request is TIM4_UP        */
#define LL_GPDMA1_REQUEST_TIM4_TRIG     38U      /*!< GPDMA1 HW request is TIM4_TRIG      */

#define LL_GPDMA1_REQUEST_TIM5_CH1      39U      /*!< GPDMA1 HW request is TIM5_CH1       */
#define LL_GPDMA1_REQUEST_TIM5_CH2      40U      /*!< GPDMA1 HW request is TIM5_CH2       */
#define LL_GPDMA1_REQUEST_TIM5_CH3      41U      /*!< GPDMA1 HW request is TIM5_CH3       */
#define LL_GPDMA1_REQUEST_TIM5_CH4      42U      /*!< GPDMA1 HW request is TIM5_CH4       */
#define LL_GPDMA1_REQUEST_TIM5_UP       43U      /*!< GPDMA1 HW request is TIM5_UP        */
#define LL_GPDMA1_REQUEST_TIM5_TRIG     44U      /*!< GPDMA1 HW request is TIM5_TRIG      */

#define LL_GPDMA1_REQUEST_TIM6_UP       45U      /*!< GPDMA1 HW request is TIM6_UP        */
#define LL_GPDMA1_REQUEST_TIM7_UP       46U      /*!< GPDMA1 HW request is TIM6_UP        */

#define LL_GPDMA1_REQUEST_TIM8_CH1      47U      /*!< GPDMA1 HW request is TIM8_CH1       */
#define LL_GPDMA1_REQUEST_TIM8_CH2      48U      /*!< GPDMA1 HW request is TIM8_CH2       */
#define LL_GPDMA1_REQUEST_TIM8_CH3      49U      /*!< GPDMA1 HW request is TIM8_CH3       */
#define LL_GPDMA1_REQUEST_TIM8_CH4      50U      /*!< GPDMA1 HW request is TIM8_CH4       */
#define LL_GPDMA1_REQUEST_TIM8_UP       51U      /*!< GPDMA1 HW request is TIM8_UP        */
#define LL_GPDMA1_REQUEST_TIM8_TRIG     52U      /*!< GPDMA1 HW request is TIM8_TRIG      */
#define LL_GPDMA1_REQUEST_TIM8_COM      53U      /*!< GPDMA1 HW request is TIM8_COM       */

/* reserved                             54U                                               */
/* reserved                             55U                                               */

#define LL_GPDMA1_REQUEST_TIM15_CH1     56U      /*!< GPDMA1 HW request is TIM15_CH1      */
#define LL_GPDMA1_REQUEST_TIM15_CH2     57U      /*!< GPDMA1 HW request is TIM15_CH2      */
#define LL_GPDMA1_REQUEST_TIM15_UP      58U      /*!< GPDMA1 HW request is TIM15_UP       */
#define LL_GPDMA1_REQUEST_TIM15_TRIG    59U      /*!< GPDMA1 HW request is TIM15_TRIG     */
#define LL_GPDMA1_REQUEST_TIM15_COM     60U      /*!< GPDMA1 HW request is TIM15_COM      */

#define LL_GPDMA1_REQUEST_TIM16_CH1     61U      /*!< GPDMA1 HW request is TIM16_CH1      */
#define LL_GPDMA1_REQUEST_TIM16_UP      62U      /*!< GPDMA1 HW request is TIM16_UP       */
#define LL_GPDMA1_REQUEST_TIM16_COM     63U      /*!< GPDMA1 HW request is TIM16_COM      */

#define LL_GPDMA1_REQUEST_TIM17_CH1     64U      /*!< GPDMA1 HW request is TIM17_CH1      */
#define LL_GPDMA1_REQUEST_TIM17_UP      65U      /*!< GPDMA1 HW request is TIM17_UP       */
#define LL_GPDMA1_REQUEST_TIM17_COM     66U      /*!< GPDMA1 HW request is TIM17_COM      */

#define LL_GPDMA1_REQUEST_TIM18_CH1     67U      /*!< GPDMA1 HW request is TIM18_CH1      */
#define LL_GPDMA1_REQUEST_TIM18_UP      68U      /*!< GPDMA1 HW request is TIM18_UP       */
#define LL_GPDMA1_REQUEST_TIM18_COM     69U      /*!< GPDMA1 HW request is TIM18_COM      */

#define LL_GPDMA1_REQUEST_LPTIM1_IC1    70U      /*!< GPDMA1 HW request is LPTIM1_IC1     */
#define LL_GPDMA1_REQUEST_LPTIM1_IC2    71U      /*!< GPDMA1 HW request is LPTIM1_IC2     */
#define LL_GPDMA1_REQUEST_LPTIM1_UE     72U      /*!< GPDMA1 HW request is LPTIM1_UE      */
#define LL_GPDMA1_REQUEST_LPTIM2_IC1    73U      /*!< GPDMA1 HW request is LPTIM2_IC1     */
#define LL_GPDMA1_REQUEST_LPTIM2_IC2    74U      /*!< GPDMA1 HW request is LPTIM2_IC2     */
#define LL_GPDMA1_REQUEST_LPTIM2_UE     75U      /*!< GPDMA1 HW request is LPTIM2_UE      */
#define LL_GPDMA1_REQUEST_LPTIM3_IC1    76U      /*!< GPDMA1 HW request is LPTIM3_IC1     */
#define LL_GPDMA1_REQUEST_LPTIM3_IC2    77U      /*!< GPDMA1 HW request is LPTIM3_IC2     */
#define LL_GPDMA1_REQUEST_LPTIM3_UE     78U      /*!< GPDMA1 HW request is LPTIM3_UE      */

#define LL_GPDMA1_REQUEST_SPI1_RX       79U      /*!< GPDMA1 HW request is SPI1_RX        */
#define LL_GPDMA1_REQUEST_SPI1_TX       80U      /*!< GPDMA1 HW request is SPI1_TX        */
#define LL_GPDMA1_REQUEST_SPI2_RX       81U      /*!< GPDMA1 HW request is SPI2_RX        */
#define LL_GPDMA1_REQUEST_SPI2_TX       82U      /*!< GPDMA1 HW request is SPI2_TX        */
#define LL_GPDMA1_REQUEST_SPI3_RX       83U      /*!< GPDMA1 HW request is SPI3_RX        */
#define LL_GPDMA1_REQUEST_SPI3_TX       84U      /*!< GPDMA1 HW request is SPI3_TX        */
#define LL_GPDMA1_REQUEST_SPI4_RX       85U      /*!< GPDMA1 HW request is SPI4_RX        */
#define LL_GPDMA1_REQUEST_SPI4_TX       86U      /*!< GPDMA1 HW request is SPI4_TX        */
#define LL_GPDMA1_REQUEST_SPI5_RX       87U      /*!< GPDMA1 HW request is SPI5_RX        */
#define LL_GPDMA1_REQUEST_SPI5_TX       88U      /*!< GPDMA1 HW request is SPI5_TX        */
#define LL_GPDMA1_REQUEST_SPI6_RX       89U      /*!< GPDMA1 HW request is SPI6_RX        */
#define LL_GPDMA1_REQUEST_SPI6_TX       90U      /*!< GPDMA1 HW request is SPI6_TX        */

#define LL_GPDMA1_REQUEST_SAI1_A        91U      /*!< GPDMA1 HW request is SAI1_A         */
#define LL_GPDMA1_REQUEST_SAI1_B        92U      /*!< GPDMA1 HW request is SAI1_B         */
#define LL_GPDMA1_REQUEST_SAI2_A        93U      /*!< GPDMA1 HW request is SAI2_A         */
#define LL_GPDMA1_REQUEST_SAI2_B        94U      /*!< GPDMA1 HW request is SAI2_B         */

#define LL_GPDMA1_REQUEST_I2C1_RX       95U      /*!< GPDMA1 HW request is I2C1_RX        */
#define LL_GPDMA1_REQUEST_I2C1_TX       96U      /*!< GPDMA1 HW request is I2C1_TX        */
#define LL_GPDMA1_REQUEST_I2C2_RX       97U      /*!< GPDMA1 HW request is I2C2_RX        */
#define LL_GPDMA1_REQUEST_I2C2_TX       98U      /*!< GPDMA1 HW request is I2C2_TX        */
#define LL_GPDMA1_REQUEST_I2C3_RX       99U      /*!< GPDMA1 HW request is I2C3_RX        */
#define LL_GPDMA1_REQUEST_I2C3_TX       100U     /*!< GPDMA1 HW request is I2C3_TX        */

#define LL_GPDMA1_REQUEST_I2C4_RX       101U     /*!< GPDMA1 HW request is I2C4_RX        */
#define LL_GPDMA1_REQUEST_I2C4_TX       102U     /*!< GPDMA1 HW request is I2C4_TX        */

#define LL_GPDMA1_REQUEST_I3C1_RX       103U     /*!< GPDMA1 HW request is I3C1_RX        */
#define LL_GPDMA1_REQUEST_I3C1_TX       104U     /*!< GPDMA1 HW request is I3C1_TX        */
#define LL_GPDMA1_REQUEST_I3C2_RX       105U     /*!< GPDMA1 HW request is I3C2_RX        */
#define LL_GPDMA1_REQUEST_I3C2_TX       106U     /*!< GPDMA1 HW request is I3C2_TX        */

#define LL_GPDMA1_REQUEST_USART1_RX     107U     /*!< GPDMA1 HW request is USART1_RX      */
#define LL_GPDMA1_REQUEST_USART1_TX     108U     /*!< GPDMA1 HW request is USART1_TX      */
#define LL_GPDMA1_REQUEST_USART2_RX     109U     /*!< GPDMA1 HW request is USART2_RX      */
#define LL_GPDMA1_REQUEST_USART2_TX     110U     /*!< GPDMA1 HW request is USART2_TX      */
#define LL_GPDMA1_REQUEST_USART3_RX     111U     /*!< GPDMA1 HW request is USART3_RX      */
#define LL_GPDMA1_REQUEST_USART3_TX     112U     /*!< GPDMA1 HW request is USART3_TX      */
#define LL_GPDMA1_REQUEST_UART4_RX      113U     /*!< GPDMA1 HW request is UART4_RX       */
#define LL_GPDMA1_REQUEST_UART4_TX      114U     /*!< GPDMA1 HW request is UART4_TX       */
#define LL_GPDMA1_REQUEST_UART5_RX      115U     /*!< GPDMA1 HW request is UART5_RX       */
#define LL_GPDMA1_REQUEST_UART5_TX      116U     /*!< GPDMA1 HW request is UART5_TX       */
#define LL_GPDMA1_REQUEST_USART6_RX     117U     /*!< GPDMA1 HW request is USART6_RX      */
#define LL_GPDMA1_REQUEST_USART6_TX     118U     /*!< GPDMA1 HW request is USART6_TX      */
#define LL_GPDMA1_REQUEST_UART7_RX      119U     /*!< GPDMA1 HW request is UART7_RX       */
#define LL_GPDMA1_REQUEST_UART7_TX      120U     /*!< GPDMA1 HW request is UART7_TX       */
#define LL_GPDMA1_REQUEST_UART8_RX      121U     /*!< GPDMA1 HW request is UART8_RX       */
#define LL_GPDMA1_REQUEST_UART8_TX      122U     /*!< GPDMA1 HW request is UART8_TX       */
#define LL_GPDMA1_REQUEST_UART9_RX      123U     /*!< GPDMA1 HW request is UART9_RX       */
#define LL_GPDMA1_REQUEST_UART9_TX      124U     /*!< GPDMA1 HW request is UART9_TX       */
#define LL_GPDMA1_REQUEST_USART10_RX    125U     /*!< GPDMA1 HW request is USART10_RX     */
#define LL_GPDMA1_REQUEST_USART10_TX    126U     /*!< GPDMA1 HW request is USART10_TX     */

#define LL_GPDMA1_REQUEST_LPUART1_RX    127U     /*!< GPDMA1 HW request is LPUART1_RX     */
#define LL_GPDMA1_REQUEST_LPUART1_TX    128U     /*!< GPDMA1 HW request is LPUART1_TX     */

#define LL_GPDMA1_REQUEST_SPDIFRX_CS    129U     /*!< GPDMA1 HW request is SPDIFRX_CS     */
#define LL_GPDMA1_REQUEST_SPDIFRX_DT    130U     /*!< GPDMA1 HW request is SPDIFRX_DT     */

#define LL_GPDMA1_REQUEST_ADF1_FLT0     131U     /*!< GPDMA1 HW request is ADF1_FLT0      */

#define LL_GPDMA1_REQUEST_MDF1_FLT0     132U     /*!< GPDMA1 HW request is MDF1_FLT0      */
#define LL_GPDMA1_REQUEST_MDF1_FLT1     133U     /*!< GPDMA1 HW request is MDF1_FLT1      */
#define LL_GPDMA1_REQUEST_MDF1_FLT2     134U     /*!< GPDMA1 HW request is MDF1_FLT2      */
#define LL_GPDMA1_REQUEST_MDF1_FLT3     135U     /*!< GPDMA1 HW request is MDF1_FLT3      */
#define LL_GPDMA1_REQUEST_MDF1_FLT4     136U     /*!< GPDMA1 HW request is MDF1_FLT4      */
#define LL_GPDMA1_REQUEST_MDF1_FLT5     137U     /*!< GPDMA1 HW request is MDF1_FLT5      */

#define LL_GPDMA1_REQUEST_UCPD1_TX      138U     /*!< GPDMA1 HW request is UCPD1_TX       */
#define LL_GPDMA1_REQUEST_UCPD1_RX      139U     /*!< GPDMA1 HW request is UCPD1_RX       */

#define LL_GPDMA1_REQUEST_DCMI_PSSI     140U     /*!< GPDMA1 HW request is DCMI_PSSI      */

#define LL_GPDMA1_REQUEST_I3C1_TC       141U     /*!< GPDMA1 HW request is I3C1_TC        */
#define LL_GPDMA1_REQUEST_I3C1_RS       142U     /*!< GPDMA1 HW request is I3C1_RS        */

#define LL_GPDMA1_REQUEST_I3C2_TC       143U     /*!< GPDMA1 HW request is I3C2_TC        */
#define LL_GPDMA1_REQUEST_I3C2_RS       144U     /*!< GPDMA1 HW request is I3C2_RS        */

/**
  * @}
  */

/** @defgroup DMA_LL_EC_TRIGGER_SELECTION Trigger Selection
  * @{
  */
/* HPDMA1 triggers */
#define LL_HPDMA1_TRIGGER_DCMIPP_P0_FEND           0U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P0_FEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P0_LEND           1U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P0_LEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P0_HSYNC          2U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P0_HSYNC          */
#define LL_HPDMA1_TRIGGER_DCMIPP_P0_VSYNC          3U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P0_VSYNC          */

#define LL_HPDMA1_TRIGGER_DCMIPP_P1_FEND           4U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P1_FEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P1_LEND           5U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P1_LEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P1_HSYNC          6U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P1_HSYNC          */
#define LL_HPDMA1_TRIGGER_DCMIPP_P1_VSYNC          7U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P1_VSYNC          */

#define LL_HPDMA1_TRIGGER_DCMIPP_P2_FEND           8U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P2_FEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P2_LEND           9U   /*!< HPDMA1 HW Trigger signal is DCMIPP_P2_LEND           */
#define LL_HPDMA1_TRIGGER_DCMIPP_P2_HSYNC          10U  /*!< HPDMA1 HW Trigger signal is DCMIPP_P2_HSYNC          */
#define LL_HPDMA1_TRIGGER_DCMIPP_P2_VSYNC          11U  /*!< HPDMA1 HW Trigger signal is DCMIPP_P2_VSYNC          */

#define LL_HPDMA1_TRIGGER_DMA2D_CTC                12U  /*!< HPDMA1 HW Trigger signal is DMA2D_CTC                */
#define LL_HPDMA1_TRIGGER_DMA2D_TC                 13U  /*!< HPDMA1 HW Trigger signal is DMA2D_TC                 */
#define LL_HPDMA1_TRIGGER_DMA2D_TW                 14U  /*!< HPDMA1 HW Trigger signal is DMA2D_TW                 */

#define LL_HPDMA1_TRIGGER_JPEG_EOC                 15U  /*!< HPDMA1 HW Trigger signal is JPEG_EOC                 */
#define LL_HPDMA1_TRIGGER_JPEG_IFNF                16U  /*!< HPDMA1 HW Trigger signal is JPEG_IFNF                */
#define LL_HPDMA1_TRIGGER_JPEG_IFT                 17U  /*!< HPDMA1 HW Trigger signal is JPEG_IFT                 */
#define LL_HPDMA1_TRIGGER_JPEG_OFNE                18U  /*!< HPDMA1 HW Trigger signal is JPEG_OFNE                */
#define LL_HPDMA1_TRIGGER_JPEG_OFT                 19U  /*!< HPDMA1 HW Trigger signal is JPEG_OFT                 */

#define LL_HPDMA1_TRIGGER_LCD_LI                   20U  /*!< HPDMA1 HW Trigger signal is LCD_LI                   */

#define LL_HPDMA1_TRIGGER_GPU2D1_GP_0              21U  /*!< HPDMA1 HW Trigger signal is GPU2D1_GP_0              */
#define LL_HPDMA1_TRIGGER_GPU2D1_GP_1              22U  /*!< HPDMA1 HW Trigger signal is GPU2D1_GP_1              */
#define LL_HPDMA1_TRIGGER_GPU2D1_GP_2              23U  /*!< HPDMA1 HW Trigger signal is GPU2D1_GP_2              */
#define LL_HPDMA1_TRIGGER_GPU2D1_GP_3              24U  /*!< HPDMA1 HW Trigger signal is GPU2D1_GP_3              */

#define LL_HPDMA1_TRIGGER_GFXTIM_3                 25U  /*!< HPDMA1 HW Trigger signal is GFXTIM_3                 */
#define LL_HPDMA1_TRIGGER_GFXTIM_2                 26U  /*!< HPDMA1 HW Trigger signal is GFXTIM_2                 */
#define LL_HPDMA1_TRIGGER_GFXTIM_1                 27U  /*!< HPDMA1 HW Trigger signal is GFXTIM_1                 */
#define LL_HPDMA1_TRIGGER_GFXTIM_0                 28U  /*!< HPDMA1 HW Trigger signal is GFXTIM_0                 */

/* reserved                                        29U                                                            */

#define LL_HPDMA1_TRIGGER_LPTIM1_CH1               30U  /*!< HPDMA1 HW Trigger signal is LPTIM1_CH1               */
#define LL_HPDMA1_TRIGGER_LPTIM1_CH2               31U  /*!< HPDMA1 HW Trigger signal is LPTIM1_CH2               */
#define LL_HPDMA1_TRIGGER_LPTIM2_CH1               32U  /*!< HPDMA1 HW Trigger signal is LPTIM2_CH1               */
#define LL_HPDMA1_TRIGGER_LPTIM2_CH2               33U  /*!< HPDMA1 HW Trigger signal is LPTIM2_CH2               */
#define LL_HPDMA1_TRIGGER_LPTIM3_CH1               34U  /*!< HPDMA1 HW Trigger signal is LPTIM3_CH1               */
#define LL_HPDMA1_TRIGGER_LPTIM3_CH2               35U  /*!< HPDMA1 HW Trigger signal is LPTIM3_CH2               */
#define LL_HPDMA1_TRIGGER_LPTIM4_OUT               36U  /*!< HPDMA1 HW Trigger signal is LPTIM4_OUT               */
#define LL_HPDMA1_TRIGGER_LPTIM5_OUT               37U  /*!< HPDMA1 HW Trigger signal is LPTIM5_OUT               */

/* reserved                                        38U                                                            */

#define LL_HPDMA1_TRIGGER_RTC_WKUP                 39U  /*!< HPDMA1 HW Trigger signal is RTC_WKUP                 */

#define LL_HPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC   40U  /*!< HPDMA1 HW Trigger signal is IT_R_WUP_ASYNC           */
#define LL_HPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC   41U  /*!< HPDMA1 HW Trigger signal is IT_T_WUP_ASYNC           */

#define LL_HPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC 42U  /*!< HPDMA1 HW Trigger signal is SPI6_IT_OR_SPI6_AIT_SYNC */

/* reserved                                        43U                                                            */

#define LL_HPDMA1_TRIGGER_TIM1_TRGO                44U  /*!< HPDMA1 HW Trigger signal is TIM1_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM1_TRGO2               45U  /*!< HPDMA1 HW Trigger signal is TIM1_TRGO2               */
#define LL_HPDMA1_TRIGGER_TIM2_TRGO                46U  /*!< HPDMA1 HW Trigger signal is TIM2_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM3_TRGO                47U  /*!< HPDMA1 HW Trigger signal is TIM3_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM4_TRGO                48U  /*!< HPDMA1 HW Trigger signal is TIM4_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM5_TRGO                49U  /*!< HPDMA1 HW Trigger signal is TIM5_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM6_TRGO                50U  /*!< HPDMA1 HW Trigger signal is TIM6_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM7_TRGO                51U  /*!< HPDMA1 HW Trigger signal is TIM7_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM8_TRGO                52U  /*!< HPDMA1 HW Trigger signal is TIM8_TRGO                */
#define LL_HPDMA1_TRIGGER_TIM8_TRGO2               53U  /*!< HPDMA1 HW Trigger signal is TIM8_TRGO2               */

/* reserved                                        54U                                                            */
/* reserved                                        55U                                                            */
/* reserved                                        56U                                                            */

#define LL_HPDMA1_TRIGGER_TIM12_TRGO               57U  /*!< HPDMA1 HW Trigger signal is TIM12_TRGO               */
#define LL_HPDMA1_TRIGGER_TIM15_TRGO               58U  /*!< HPDMA1 HW Trigger signal is TIM15_TRGO               */

/* reserved                                        59U                                                            */

#define LL_HPDMA1_TRIGGER_HPDMA1_CH0_TCF           60U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH0_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH1_TCF           61U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH1_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH2_TCF           62U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH2_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH3_TCF           63U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH3_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH4_TCF           64U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH4_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH5_TCF           65U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH5_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH6_TCF           66U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH6_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH7_TCF           67U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH7_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH8_TCF           68U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH8_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH9_TCF           69U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH9_TCF           */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH10_TCF          70U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH10_TCF          */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH11_TCF          71U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH11_TCF          */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH12_TCF          72U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH12_TCF          */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH13_TCF          73U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH13_TCF          */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH14_TCF          74U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH14_TCF          */
#define LL_HPDMA1_TRIGGER_HPDMA1_CH15_TCF          75U  /*!< HPDMA1 HW Trigger signal is HPDMA1_CH15_TCF          */

#define LL_HPDMA1_TRIGGER_GPDMA1_CH0_TCF           76U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH0_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH1_TCF           77U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH1_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH2_TCF           78U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH2_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH3_TCF           79U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH3_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH4_TCF           80U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH4_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH5_TCF           81U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH5_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH6_TCF           82U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH6_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH7_TCF           83U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH7_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH8_TCF           84U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH8_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH9_TCF           85U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH9_TCF           */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH10_TCF          86U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH10_TCF          */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH11_TCF          87U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH11_TCF          */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH12_TCF          88U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH12_TCF          */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH13_TCF          89U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH13_TCF          */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH14_TCF          90U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH14_TCF          */
#define LL_HPDMA1_TRIGGER_GPDMA1_CH15_TCF          91U  /*!< HPDMA1 HW Trigger signal is GPDMA1_CH15_TCF          */

/* reserved                                        92U                                                            */

#define LL_HPDMA1_TRIGGER_EXTIT0_SYNC              93U  /*!< HPDMA1 HW Trigger signal is EXTIT0_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT1_SYNC              94U  /*!< HPDMA1 HW Trigger signal is EXTIT1_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT2_SYNC              95U  /*!< HPDMA1 HW Trigger signal is EXTIT2_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT3_SYNC              96U  /*!< HPDMA1 HW Trigger signal is EXTIT3_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT4_SYNC              97U  /*!< HPDMA1 HW Trigger signal is EXTIT4_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT5_SYNC              98U  /*!< HPDMA1 HW Trigger signal is EXTIT5_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT6_SYNC              99U  /*!< HPDMA1 HW Trigger signal is EXTIT6_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT7_SYNC              100U /*!< HPDMA1 HW Trigger signal is EXTIT7_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT8_SYNC              101U /*!< HPDMA1 HW Trigger signal is EXTIT8_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT9_SYNC              102U /*!< HPDMA1 HW Trigger signal is EXTIT9_SYNC              */
#define LL_HPDMA1_TRIGGER_EXTIT10_SYNC             103U /*!< HPDMA1 HW Trigger signal is EXTIT10_SYNC             */
#define LL_HPDMA1_TRIGGER_EXTIT11_SYNC             104U /*!< HPDMA1 HW Trigger signal is EXTIT11_SYNC             */
#define LL_HPDMA1_TRIGGER_EXTIT12_SYNC             105U /*!< HPDMA1 HW Trigger signal is EXTIT12_SYNC             */
#define LL_HPDMA1_TRIGGER_EXTIT13_SYNC             106U /*!< HPDMA1 HW Trigger signal is EXTIT13_SYNC             */
#define LL_HPDMA1_TRIGGER_EXTIT14_SYNC             107U /*!< HPDMA1 HW Trigger signal is EXTIT14_SYNC             */
#define LL_HPDMA1_TRIGGER_EXTIT15_SYNC             108U /*!< HPDMA1 HW Trigger signal is EXTIT15_SYNC             */


/* GPDMA1 triggers */
#define LL_GPDMA1_TRIGGER_DCMIPP_P0_FEND           0U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P0_FEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P0_LEND           1U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P0_LEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P0_HSYNC          2U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P0_HSYNC          */
#define LL_GPDMA1_TRIGGER_DCMIPP_P0_VSYNC          3U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P0_VSYNC          */

#define LL_GPDMA1_TRIGGER_DCMIPP_P1_FEND           4U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P1_FEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P1_LEND           5U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P1_LEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P1_HSYNC          6U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P1_HSYNC          */
#define LL_GPDMA1_TRIGGER_DCMIPP_P1_VSYNC          7U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P1_VSYNC          */

#define LL_GPDMA1_TRIGGER_DCMIPP_P2_FEND           8U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P2_FEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P2_LEND           9U   /*!< GPDMA1 HW Trigger signal is DCMIPP_P2_LEND           */
#define LL_GPDMA1_TRIGGER_DCMIPP_P2_HSYNC          10U  /*!< GPDMA1 HW Trigger signal is DCMIPP_P2_HSYNC          */
#define LL_GPDMA1_TRIGGER_DCMIPP_P2_VSYNC          11U  /*!< GPDMA1 HW Trigger signal is DCMIPP_P2_VSYNC          */

#define LL_GPDMA1_TRIGGER_DMA2D_CTC                12U  /*!< GPDMA1 HW Trigger signal is DMA2D_CTC                */
#define LL_GPDMA1_TRIGGER_DMA2D_TC                 13U  /*!< GPDMA1 HW Trigger signal is DMA2D_TC                 */
#define LL_GPDMA1_TRIGGER_DMA2D_TW                 14U  /*!< GPDMA1 HW Trigger signal is DMA2D_TW                 */

#define LL_GPDMA1_TRIGGER_JPEG_EOC                 15U  /*!< GPDMA1 HW Trigger signal is JPEG_EOC                 */
#define LL_GPDMA1_TRIGGER_JPEG_IFNF                16U  /*!< GPDMA1 HW Trigger signal is JPEG_IFNF                */
#define LL_GPDMA1_TRIGGER_JPEG_IFT                 17U  /*!< GPDMA1 HW Trigger signal is JPEG_IFT                 */
#define LL_GPDMA1_TRIGGER_JPEG_OFNE                18U  /*!< GPDMA1 HW Trigger signal is JPEG_OFNE                */
#define LL_GPDMA1_TRIGGER_JPEG_OFT                 19U  /*!< GPDMA1 HW Trigger signal is JPEG_OFT                 */

#define LL_GPDMA1_TRIGGER_LCD_LI                   20U  /*!< GPDMA1 HW Trigger signal is LCD_LI                   */

#define LL_GPDMA1_TRIGGER_GPU2D1_GP_0              21U  /*!< GPDMA1 HW Trigger signal is GPU2D1_GP_0              */
#define LL_GPDMA1_TRIGGER_GPU2D1_GP_1              22U  /*!< GPDMA1 HW Trigger signal is GPU2D1_GP_1              */
#define LL_GPDMA1_TRIGGER_GPU2D1_GP_2              23U  /*!< GPDMA1 HW Trigger signal is GPU2D1_GP_2              */
#define LL_GPDMA1_TRIGGER_GPU2D1_GP_3              24U  /*!< GPDMA1 HW Trigger signal is GPU2D1_GP_3              */

#define LL_GPDMA1_TRIGGER_GFXTIM_3                 25U  /*!< GPDMA1 HW Trigger signal is GFXTIM_3                 */
#define LL_GPDMA1_TRIGGER_GFXTIM_2                 26U  /*!< GPDMA1 HW Trigger signal is GFXTIM_2                 */
#define LL_GPDMA1_TRIGGER_GFXTIM_1                 27U  /*!< GPDMA1 HW Trigger signal is GFXTIM_1                 */
#define LL_GPDMA1_TRIGGER_GFXTIM_0                 28U  /*!< GPDMA1 HW Trigger signal is GFXTIM_0                 */

/* reserved                                        29U                                                            */

#define LL_GPDMA1_TRIGGER_LPTIM1_CH1               30U  /*!< GPDMA1 HW Trigger signal is LPTIM1_CH1               */
#define LL_GPDMA1_TRIGGER_LPTIM1_CH2               31U  /*!< GPDMA1 HW Trigger signal is LPTIM1_CH2               */
#define LL_GPDMA1_TRIGGER_LPTIM2_CH1               32U  /*!< GPDMA1 HW Trigger signal is LPTIM2_CH1               */
#define LL_GPDMA1_TRIGGER_LPTIM2_CH2               33U  /*!< GPDMA1 HW Trigger signal is LPTIM2_CH2               */
#define LL_GPDMA1_TRIGGER_LPTIM3_CH1               34U  /*!< GPDMA1 HW Trigger signal is LPTIM3_CH1               */
#define LL_GPDMA1_TRIGGER_LPTIM3_CH2               35U  /*!< GPDMA1 HW Trigger signal is LPTIM3_CH2               */
#define LL_GPDMA1_TRIGGER_LPTIM4_OUT               36U  /*!< GPDMA1 HW Trigger signal is LPTIM4_OUT               */
#define LL_GPDMA1_TRIGGER_LPTIM5_OUT               37U  /*!< GPDMA1 HW Trigger signal is LPTIM5_OUT               */

/* reserved                                        38U                                                            */

#define LL_GPDMA1_TRIGGER_RTC_WKUP                 39U  /*!< GPDMA1 HW Trigger signal is RTC_WKUP                 */

#define LL_GPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC   40U  /*!< GPDMA1 HW Trigger signal is IT_R_WUP_ASYNC           */
#define LL_GPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC   41U  /*!< GPDMA1 HW Trigger signal is IT_T_WUP_ASYNC           */

#define LL_GPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC 42U  /*!< GPDMA1 HW Trigger signal is SPI6_IT_OR_SPI6_AIT_SYNC */

/* reserved                                        43U                                                            */

#define LL_GPDMA1_TRIGGER_TIM1_TRGO                44U  /*!< GPDMA1 HW Trigger signal is TIM1_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM1_TRGO2               45U  /*!< GPDMA1 HW Trigger signal is TIM1_TRGO2               */
#define LL_GPDMA1_TRIGGER_TIM2_TRGO                46U  /*!< GPDMA1 HW Trigger signal is TIM2_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM3_TRGO                47U  /*!< GPDMA1 HW Trigger signal is TIM3_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM4_TRGO                48U  /*!< GPDMA1 HW Trigger signal is TIM4_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM5_TRGO                49U  /*!< GPDMA1 HW Trigger signal is TIM5_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM6_TRGO                50U  /*!< GPDMA1 HW Trigger signal is TIM6_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM7_TRGO                51U  /*!< GPDMA1 HW Trigger signal is TIM7_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM8_TRGO                52U  /*!< GPDMA1 HW Trigger signal is TIM8_TRGO                */
#define LL_GPDMA1_TRIGGER_TIM8_TRGO2               53U  /*!< GPDMA1 HW Trigger signal is TIM8_TRGO2               */

/* reserved                                        54U                                                            */
/* reserved                                        55U                                                            */
/* reserved                                        56U                                                            */

#define LL_GPDMA1_TRIGGER_TIM12_TRGO               57U  /*!< GPDMA1 HW Trigger signal is TIM12_TRGO               */
#define LL_GPDMA1_TRIGGER_TIM15_TRGO               58U  /*!< GPDMA1 HW Trigger signal is TIM15_TRGO               */

/* reserved                                        59U                                                            */

#define LL_GPDMA1_TRIGGER_HPDMA1_CH0_TCF           60U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH0_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH1_TCF           61U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH1_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH2_TCF           62U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH2_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH3_TCF           63U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH3_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH4_TCF           64U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH4_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH5_TCF           65U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH5_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH6_TCF           66U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH6_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH7_TCF           67U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH7_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH8_TCF           68U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH8_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH9_TCF           69U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH9_TCF           */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH10_TCF          70U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH10_TCF          */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH11_TCF          71U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH11_TCF          */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH12_TCF          72U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH12_TCF          */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH13_TCF          73U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH13_TCF          */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH14_TCF          74U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH14_TCF          */
#define LL_GPDMA1_TRIGGER_HPDMA1_CH15_TCF          75U  /*!< GPDMA1 HW Trigger signal is HPDMA1_CH15_TCF          */

#define LL_GPDMA1_TRIGGER_GPDMA1_CH0_TCF           76U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH0_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH1_TCF           77U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH1_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH2_TCF           78U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH2_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH3_TCF           79U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH3_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH4_TCF           80U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH4_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH5_TCF           81U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH5_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH6_TCF           82U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH6_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH7_TCF           83U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH7_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH8_TCF           84U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH8_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH9_TCF           85U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH9_TCF           */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH10_TCF          86U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH10_TCF          */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH11_TCF          87U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH11_TCF          */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH12_TCF          88U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH12_TCF          */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH13_TCF          89U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH13_TCF          */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH14_TCF          90U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH14_TCF          */
#define LL_GPDMA1_TRIGGER_GPDMA1_CH15_TCF          91U  /*!< GPDMA1 HW Trigger signal is GPDMA1_CH15_TCF          */

/* reserved                                        92U                                                            */

#define LL_GPDMA1_TRIGGER_EXTIT0_SYNC              93U  /*!< GPDMA1 HW Trigger signal is EXTIT0_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT1_SYNC              94U  /*!< GPDMA1 HW Trigger signal is EXTIT1_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT2_SYNC              95U  /*!< GPDMA1 HW Trigger signal is EXTIT2_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT3_SYNC              96U  /*!< GPDMA1 HW Trigger signal is EXTIT3_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT4_SYNC              97U  /*!< GPDMA1 HW Trigger signal is EXTIT4_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT5_SYNC              98U  /*!< GPDMA1 HW Trigger signal is EXTIT5_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT6_SYNC              99U  /*!< GPDMA1 HW Trigger signal is EXTIT6_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT7_SYNC              100U /*!< GPDMA1 HW Trigger signal is EXTIT7_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT8_SYNC              101U /*!< GPDMA1 HW Trigger signal is EXTIT8_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT9_SYNC              102U /*!< GPDMA1 HW Trigger signal is EXTIT9_SYNC              */
#define LL_GPDMA1_TRIGGER_EXTIT10_SYNC             103U /*!< GPDMA1 HW Trigger signal is EXTIT10_SYNC             */
#define LL_GPDMA1_TRIGGER_EXTIT11_SYNC             104U /*!< GPDMA1 HW Trigger signal is EXTIT11_SYNC             */
#define LL_GPDMA1_TRIGGER_EXTIT12_SYNC             105U /*!< GPDMA1 HW Trigger signal is EXTIT12_SYNC             */
#define LL_GPDMA1_TRIGGER_EXTIT13_SYNC             106U /*!< GPDMA1 HW Trigger signal is EXTIT13_SYNC             */
#define LL_GPDMA1_TRIGGER_EXTIT14_SYNC             107U /*!< GPDMA1 HW Trigger signal is EXTIT14_SYNC             */
#define LL_GPDMA1_TRIGGER_EXTIT15_SYNC             108U /*!< GPDMA1 HW Trigger signal is EXTIT15_SYNC             */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup DMA_LL_Exported_Macros DMA Exported Macros
  * @{
  */

/** @defgroup DMA_LL_EM_COMMON_WRITE_READ_REGISTERS Common Write and Read Registers macros
  * @{
  */
/**
  * @brief  Write a value in DMA register.
  * @param  __INSTANCE__ DMA Instance.
  * @param  __REG__      Register to be written.
  * @param  __VALUE__    Value to be written in the register.
  * @retval None.
  */
#define LL_DMA_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in DMA register.
  * @param  __INSTANCE__ DMA Instance.
  * @param  __REG__      Register to be read.
  * @retval Register value.
  */
#define LL_DMA_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/** @defgroup DMA_LL_EM_CONVERT_DMAxCHANNELy Convert DMAxChannely
  * @{
  */
/**
  * @brief  Convert DMAx_Channely into DMAx.
  * @param  __CHANNEL_INSTANCE__ DMAx_Channely.
  * @retval DMAx.
  */
#define LL_DMA_GET_INSTANCE(__CHANNEL_INSTANCE__) \
  (((uint32_t)(__CHANNEL_INSTANCE__) > ((uint32_t)GPDMA1_Channel15)) ?  HPDMA1 : GPDMA1)

/**
  * @brief  Convert DMAx_Channely into LL_DMA_CHANNEL_y.
  * @param  __CHANNEL_INSTANCE__ DMAx_Channely.
  * @retval LL_DMA_CHANNEL_y.
  */
#define LL_DMA_GET_CHANNEL(__CHANNEL_INSTANCE__) \
  (((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel0))  ? LL_DMA_CHANNEL_0  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel0))  ? LL_DMA_CHANNEL_0  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel1))  ? LL_DMA_CHANNEL_1  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel1))  ? LL_DMA_CHANNEL_1  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel2))  ? LL_DMA_CHANNEL_2  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel2))  ? LL_DMA_CHANNEL_2  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel3))  ? LL_DMA_CHANNEL_3  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel3))  ? LL_DMA_CHANNEL_3  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel4))  ? LL_DMA_CHANNEL_4  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel4))  ? LL_DMA_CHANNEL_4  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel5))  ? LL_DMA_CHANNEL_5  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel5))  ? LL_DMA_CHANNEL_5  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel6))  ? LL_DMA_CHANNEL_6  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel6))  ? LL_DMA_CHANNEL_6  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel7))  ? LL_DMA_CHANNEL_7  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel7))  ? LL_DMA_CHANNEL_7  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel8))  ? LL_DMA_CHANNEL_8  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel8))  ? LL_DMA_CHANNEL_8  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel9))  ? LL_DMA_CHANNEL_9  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel9))  ? LL_DMA_CHANNEL_9  : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel10)) ? LL_DMA_CHANNEL_10 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel10)) ? LL_DMA_CHANNEL_10 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel11)) ? LL_DMA_CHANNEL_11 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel11)) ? LL_DMA_CHANNEL_11 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel12)) ? LL_DMA_CHANNEL_12 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel12)) ? LL_DMA_CHANNEL_12 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel13)) ? LL_DMA_CHANNEL_13 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel13)) ? LL_DMA_CHANNEL_13 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)HPDMA1_Channel14)) ? LL_DMA_CHANNEL_14 : \
   ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)GPDMA1_Channel14)) ? LL_DMA_CHANNEL_14 : \
   LL_DMA_CHANNEL_15)

/**
  * @brief  Convert DMA Instance DMAx and LL_DMA_CHANNEL_y into DMAx_Channely.
  * @param  __DMA_INSTANCE__ DMAx.
  * @param  __CHANNEL__      LL_DMA_CHANNEL_y.
  * @retval DMAx_Channely.
  */
#define LL_DMA_GET_CHANNEL_INSTANCE(__DMA_INSTANCE__, __CHANNEL__) \
  ((((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_0))) \
   ? HPDMA1_Channel0  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_0))) \
   ? GPDMA1_Channel0  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_1))) \
   ? HPDMA1_Channel1  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_1))) \
   ? GPDMA1_Channel1  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_2))) \
   ? HPDMA1_Channel2  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_2))) \
   ? GPDMA1_Channel2  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_3))) \
   ? HPDMA1_Channel3  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_3))) \
   ? GPDMA1_Channel3  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_4))) \
   ? HPDMA1_Channel4  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_4))) \
   ? GPDMA1_Channel4  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_5))) \
   ? HPDMA1_Channel5  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_5))) \
   ? GPDMA1_Channel5  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_6))) \
   ? HPDMA1_Channel6  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_6))) \
   ? GPDMA1_Channel6  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_7))) \
   ? HPDMA1_Channel7  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_7))) \
   ? GPDMA1_Channel7  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_8))) \
   ? HPDMA1_Channel8  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_8))) \
   ? GPDMA1_Channel8  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_9))) \
   ? HPDMA1_Channel9  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_9))) \
   ? GPDMA1_Channel9  :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_10)))\
   ? HPDMA1_Channel10 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_10)))\
   ? GPDMA1_Channel10 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_11)))\
   ? HPDMA1_Channel11 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_11)))\
   ? GPDMA1_Channel11 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_12)))\
   ? HPDMA1_Channel12 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_12)))\
   ? GPDMA1_Channel12 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_13)))\
   ? HPDMA1_Channel13 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_13)))\
   ? GPDMA1_Channel13 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_14)))\
   ? HPDMA1_Channel14 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)GPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_14)))\
   ? GPDMA1_Channel14 :                                                                                                \
   (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)HPDMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_15)))\
   ? HPDMA1_Channel15 :  GPDMA1_Channel15)

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup DMA_LL_Exported_Functions DMA Exported Functions
  * @{
  */

/** @defgroup DMA_LL_EF_Configuration Configuration
  * @{
  */
/**
  * @brief Enable channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          EN            LL_DMA_EnableChannel
  * @param  DMAx DMAx Instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_EN);
}

/**
  * @brief Disable channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          EN            LL_DMA_DisableChannel
  * @param  DMAx DMAx Instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR,
          (DMA_CCR_SUSP | DMA_CCR_RESET));
}

/**
  * @brief Check if channel is enabled or disabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          EN            LL_DMA_IsEnabledChannel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_EN)
           == (DMA_CCR_EN)) ? 1UL : 0UL);
}

/**
  * @brief Reset channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          RESET            LL_DMA_ResetChannel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ResetChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_RESET);
}

/**
  * @brief Suspend channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          SUSP            LL_DMA_SuspendChannel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SuspendChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSP);
}

/**
  * @brief Resume channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          SUSP            LL_DMA_ResumeChannel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ResumeChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSP);
}

/**
  * @brief Check if channel is suspended.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          SUSP            LL_DMA_IsSuspendedChannel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsSuspendedChannel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSP)
           == (DMA_CCR_SUSP)) ? 1UL : 0UL);
}

/**
  * @brief Set linked-list base address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLBAR          LBA           LL_DMA_SetLinkedListBaseAddr
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  LinkedListBaseAddr Between 0 to 0xFFFF0000 (where the 4 LSB bytes
  *         are always 0)
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetLinkedListBaseAddr(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                  uint32_t LinkedListBaseAddr)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLBAR, DMA_CLBAR_LBA,
             (LinkedListBaseAddr & DMA_CLBAR_LBA));
}

/**
  * @brief Get linked-list base address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLBAR          LBA           LL_DMA_GetLinkedListBaseAddr
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Value between 0 to 0xFFFF0000 (where the 4 LSB bytes are always 0)
  */
__STATIC_INLINE uint32_t LL_DMA_GetLinkedListBaseAddr(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLBAR, DMA_CLBAR_LBA));
}

/**
  * @brief Configure all parameters linked to channel control.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR         PRIO           LL_DMA_ConfigControl\n
  *         CCR         LAP            LL_DMA_ConfigControl\n
  *         CCR         LSM            LL_DMA_ConfigControl
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_LOW_PRIORITY_LOW_WEIGHT  or @ref LL_DMA_LOW_PRIORITY_MID_WEIGHT or
  *              @ref LL_DMA_LOW_PRIORITY_HIGH_WEIGHT or @ref LL_DMA_HIGH_PRIORITY
  *         @arg @ref LL_DMA_LINK_ALLOCATED_PORT0     or @ref LL_DMA_LINK_ALLOCATED_PORT1
  *         @arg @ref LL_DMA_LSM_FULL_EXECUTION       or @ref LL_DMA_LSM_1LINK_EXECUTION
  *@retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigControl(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR,
             (DMA_CCR_PRIO | DMA_CCR_LAP | DMA_CCR_LSM), Configuration);
}

/**
  * @brief Set priority level.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          PRIO           LL_DMA_SetChannelPriorityLevel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Priority This parameter can be one of the following values:
  *         @arg @ref LL_DMA_LOW_PRIORITY_LOW_WEIGHT
  *         @arg @ref LL_DMA_LOW_PRIORITY_MID_WEIGHT
  *         @arg @ref LL_DMA_LOW_PRIORITY_HIGH_WEIGHT
  *         @arg @ref LL_DMA_HIGH_PRIORITY
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetChannelPriorityLevel(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t  Priority)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_PRIO, Priority);
}

/**
  * @brief Get Channel priority level.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          PRIO           LL_DMA_GetChannelPriorityLevel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_LOW_PRIORITY_LOW_WEIGHT
  *         @arg @ref LL_DMA_LOW_PRIORITY_MID_WEIGHT
  *         @arg @ref LL_DMA_LOW_PRIORITY_HIGH_WEIGHT
  *         @arg @ref LL_DMA_HIGH_PRIORITY
  */
__STATIC_INLINE uint32_t LL_DMA_GetChannelPriorityLevel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_PRIO));
}

/**
  * @brief Set linked-list allocated port.
  * @rmtoll CCR          LAP           LL_DMA_SetLinkAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  LinkAllocatedPort This parameter can be one of the following values:
  *         @arg @ref LL_DMA_LINK_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_LINK_ALLOCATED_PORT1
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetLinkAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t  LinkAllocatedPort)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR,
             DMA_CCR_LAP, LinkAllocatedPort);
}

/**
  * @brief Get linked-list allocated port.
  * @rmtoll CCR          LAP           LL_DMA_GetLinkAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_LINK_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_LINK_ALLOCATED_PORT1
  */
__STATIC_INLINE uint32_t LL_DMA_GetLinkAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_LAP));
}

/**
  * @brief Set link step mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          LSM           LL_DMA_SetLinkStepMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
    *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  LinkStepMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_LSM_FULL_EXECUTION
  *         @arg @ref LL_DMA_LSM_1LINK_EXECUTION
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetLinkStepMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t LinkStepMode)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_LSM, LinkStepMode);
}

/**
  * @brief Get Link step mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR          LSM           LL_DMA_GetLinkStepMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_LSM_FULL_EXECUTION
  *         @arg @ref LL_DMA_LSM_1LINK_EXECUTION
  */
__STATIC_INLINE uint32_t LL_DMA_GetLinkStepMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_LSM));
}

/**
  * @brief Configure data transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DAP           LL_DMA_ConfigTransfer\n
  *         CTR1          DWX           LL_DMA_ConfigTransfer\n
  *         CTR1          DHX           LL_DMA_ConfigTransfer\n
  *         CTR1          DBX           LL_DMA_ConfigTransfer\n
  *         CTR1          DINC          LL_DMA_ConfigTransfer\n
  *         CTR1          SAP           LL_DMA_ConfigTransfer\n
  *         CTR1          SBX           LL_DMA_ConfigTransfer\n
  *         CTR1          PAM           LL_DMA_ConfigTransfer\n
  *         CTR1          SINC          LL_DMA_ConfigTransfer
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_DEST_ALLOCATED_PORT0     or @ref LL_DMA_DEST_ALLOCATED_PORT1
  *         @arg @ref LL_DMA_DEST_WORD_PRESERVE       or @ref LL_DMA_DEST_WORD_EXCHANGE
  *         @arg @ref LL_DMA_DEST_HALFWORD_PRESERVE   or @ref LL_DMA_DEST_HALFWORD_EXCHANGE
  *         @arg @ref LL_DMA_DEST_BYTE_PRESERVE       or @ref LL_DMA_DEST_BYTE_EXCHANGE
  *         @arg @ref LL_DMA_SRC_BYTE_PRESERVE        or @ref LL_DMA_SRC_BYTE_EXCHANGE
  *         @arg @ref LL_DMA_DEST_FIXED               or @ref LL_DMA_DEST_INCREMENT
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_BYTE      or @ref LL_DMA_DEST_DATAWIDTH_HALFWORD or
  *              @ref LL_DMA_DEST_DATAWIDTH_WORD      or @ref LL_DMA_DEST_DATAWIDTH_DOUBLEWORD
  *         @arg @ref LL_DMA_SRC_ALLOCATED_PORT0      or @ref LL_DMA_SRC_ALLOCATED_PORT1
  *         @arg @ref LL_DMA_DATA_ALIGN_ZEROPADD      or @ref LL_DMA_DATA_ALIGN_SIGNEXTPADD  or
  *              @ref LL_DMA_DATA_PACK_UNPACK
  *         @arg @ref LL_DMA_SRC_FIXED                or @ref LL_DMA_SRC_INCREMENT
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_BYTE       or @ref LL_DMA_SRC_DATAWIDTH_HALFWORD  or
  *              @ref LL_DMA_SRC_DATAWIDTH_WORD       or @ref LL_DMA_SRC_DATAWIDTH_DOUBLEWORD
  *@retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigTransfer(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1,
             DMA_CTR1_DAP | DMA_CTR1_DWX | DMA_CTR1_DHX | DMA_CTR1_DBX | DMA_CTR1_SBX | DMA_CTR1_DINC | \
             DMA_CTR1_SINC | DMA_CTR1_SAP | DMA_CTR1_PAM | DMA_CTR1_DDW_LOG2 | DMA_CTR1_SDW_LOG2, Configuration);
}

/**
  * @brief Configure source and destination burst length.
  * @rmtoll CTR1          DBL_1           LL_DMA_SetDestBurstLength\n
  * @rmtoll CTR1          SBL_1           LL_DMA_SetDestBurstLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcBurstLength Between 1 to 64
  * @param  DestBurstLength Between 1 to 64
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigBurstLength(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcBurstLength,
                                              uint32_t DestBurstLength)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1,
             (DMA_CTR1_SBL_1 | DMA_CTR1_DBL_1), (((SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1) | \
             (((DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1));
}

#if defined(CPU_IN_SECURE_STATE)
/**
  * @brief Set the static isolation attribute (CID) of the DMA channel.
  * @note  This API is used for HPDMA channels.
  * @rmtoll CCIDCFGR      SCID           LL_DMA_SetStaticIsolation\n
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Cid This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_0
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_1
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_2
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_3
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_4
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_5
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_6
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetStaticIsolation(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Cid)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCIDCFGR,
             DMA_CCIDCFGR_SCID, Cid);
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief Get the static isolation attribute (CID) of the DMA channel.
  * @note  This API is used for HPDMA channels.
  * @rmtoll CCIDCFGR      SCID           LL_DMA_GetStaticIsolation\n
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_0
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_1
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_2
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_3
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_4
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_5
  *         @arg @ref LL_DMA_CHANNEL_STATIC_CID_6
  */
__STATIC_INLINE uint32_t LL_DMA_GetStaticIsolation(DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCIDCFGR,
                   DMA_CCIDCFGR_SCID));
}

/**
  * @brief Enable the isolation (CID filtering) of the DMA channel.
  * @note  This API is used for HPDMA channels.
  * @rmtoll CCIDCFGR          CFEN       LL_DMA_EnableIsolation
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIsolation(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCIDCFGR, DMA_CCIDCFGR_CFEN);
}

/**
  * @brief Disable the isolation (CID filtering) of the DMA channel.
  * @note  This API is used for HPDMA channels.
  * @rmtoll CCIDCFGR          CFEN       LL_DMA_DisableIsolation
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIsolation(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCIDCFGR, DMA_CCIDCFGR_CFEN);
}

/**
  * @brief Check isolation (CID filtering) of the DMA channel.
  * @note  This API is used for HPDMA channels.
  * @rmtoll CCIDCFGR       CFEN           LL_DMA_IsEnabledIsolation
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIsolation(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCIDCFGR,
                    DMA_CCIDCFGR_CFEN) == (DMA_CCIDCFGR_CFEN)) ? 1UL : 0UL);
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief Configure all secure parameters linked to DMA channel.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SECCFGR       SEC            LL_DMA_ConfigChannelSecure\n
  * @rmtoll CTR1          SSEC           LL_DMA_ConfigChannelSecure\n
  * @rmtoll CTR1          DSEC           LL_DMA_ConfigChannelSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_CHANNEL_NSEC      or @ref LL_DMA_CHANNEL_SEC
  *         @arg @ref LL_DMA_CHANNEL_SRC_NSEC  or @ref LL_DMA_CHANNEL_SRC_SEC
  *         @arg @ref LL_DMA_CHANNEL_DEST_NSEC or @ref LL_DMA_CHANNEL_DEST_SEC
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigChannelSecure(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(DMAx->SECCFGR, (DMA_SECCFGR_SEC0 << Channel), ((Configuration & LL_DMA_CHANNEL_SEC) << Channel));
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1,
             (DMA_CTR1_SSEC | DMA_CTR1_DSEC), (Configuration & (~LL_DMA_CHANNEL_SEC)));
}

/**
  * @brief Enable security attribute of the DMA transfer to the destination.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DSEC           LL_DMA_EnableChannelDestSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannelDestSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DSEC);
}

/**
  * @brief Disable security attribute of the DMA transfer to the destination.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DSEC           LL_DMA_DisableChannelDestSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableChannelDestSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DSEC);
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief Check security attribute of the DMA transfer to the destination.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DSEC           LL_DMA_IsEnabledChannelDestSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannelDestSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DSEC)
           == (DMA_CTR1_DSEC)) ? 1UL : 0UL);
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief Enable security attribute of the DMA transfer from the source.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SSEC           LL_DMA_EnableChannelSrcSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannelSrcSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SSEC);
}

/**
  * @brief Disable security attribute of the DMA transfer from the source.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SSEC           LL_DMA_DisableChannelSrcSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableChannelSrcSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SSEC);
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief Check security attribute of the DMA transfer from the source.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SSEC           LL_DMA_IsEnabledChannelSrcSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannelSrcSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SSEC)
           == (DMA_CTR1_SSEC)) ? 1UL : 0UL);
}

/**
  * @brief Set destination allocated port.
  * @rmtoll CTR1          DAP           LL_DMA_SetDestAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestAllocatedPort This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_DEST_ALLOCATED_PORT1
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestAllocatedPort)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DAP,
             DestAllocatedPort);
}

/**
  * @brief Get destination allocated port.
  * @rmtoll CTR1          DAP           LL_DMA_GetDestAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_DEST_ALLOCATED_PORT1
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DAP));
}

/**
  * @brief Set destination word exchange.
  * @rmtoll CTR1          DWX            LL_DMA_SetDestWordExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestWordExchange This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_WORD_PRESERVE
  *         @arg @ref LL_DMA_DEST_WORD_EXCHANGE
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestWordExchange(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestWordExchange)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DWX,
             DestWordExchange);
}

/**
  * @brief Get destination word exchange.
  * @rmtoll CTR1          DWX            LL_DMA_GetDestWordExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_WORD_PRESERVE
  *         @arg @ref LL_DMA_DEST_WORD_EXCHANGE
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestWordExchange(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DWX));
}

/**
  * @brief Set destination half-word exchange.
  * @rmtoll CTR1          DHX            LL_DMA_SetDestHWordExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestHWordExchange This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_HALFWORD_PRESERVE
  *         @arg @ref LL_DMA_DEST_HALFWORD_EXCHANGE
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestHWordExchange(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestHWordExchange)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DHX,
             DestHWordExchange);
}

/**
  * @brief Get destination half-word exchange.
  * @rmtoll CTR1          DHX            LL_DMA_GetDestHWordExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_HALFWORD_PRESERVE
  *         @arg @ref LL_DMA_DEST_HALFWORD_EXCHANGE
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestHWordExchange(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DHX));
}

/**
  * @brief Set destination byte exchange.
  * @rmtoll CTR1          DBX            LL_DMA_SetDestByteExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestByteExchange This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_BYTE_PRESERVE
  *         @arg @ref LL_DMA_DEST_BYTE_EXCHANGE
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestByteExchange(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestByteExchange)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DBX,
             DestByteExchange);
}

/**
  * @brief Get destination byte exchange.
  * @rmtoll CTR1          DBX            LL_DMA_GetDestByteExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_BYTE_PRESERVE
  *         @arg @ref LL_DMA_DEST_BYTE_EXCHANGE
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestByteExchange(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DBX));
}

/**
  * @brief Set source byte exchange.
  * @rmtoll CTR1          SBX            LL_DMA_SetSrcByteExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcByteExchange This parameter can be one of the following values:
  *         @arg @ref LL_DMA_SRC_BYTE_PRESERVE
  *         @arg @ref LL_DMA_SRC_BYTE_EXCHANGE
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcByteExchange(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcByteExchange)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SBX,
             SrcByteExchange);
}

/**
  * @brief Get source byte exchange.
  * @rmtoll CTR1          SBX            LL_DMA_GetSrcByteExchange
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_SRC_BYTE_PRESERVE
  *         @arg @ref LL_DMA_SRC_BYTE_EXCHANGE
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcByteExchange(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SBX));
}

/**
  * @brief Set destination burst length.
  * @rmtoll CTR1          DBL_1           LL_DMA_SetDestBurstLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestBurstLength Between 1 to 64
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestBurstLength(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestBurstLength)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DBL_1,
             ((DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1);
}

/**
  * @brief Get destination burst length.
  * @rmtoll CTR1          DBL_1           LL_DMA_GetDestBurstLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 1 to 64.
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestBurstLength(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1,
                    DMA_CTR1_DBL_1) >> DMA_CTR1_DBL_1_Pos) + 1U);
}

/**
  * @brief Set destination increment mode.
  * @rmtoll CTR1          DINC           LL_DMA_SetDestIncMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestInc This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_FIXED
  *         @arg @ref LL_DMA_DEST_INCREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestIncMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestInc)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DINC, DestInc);
}

/**
  * @brief Get destination increment mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DINC           LL_DMA_GetDestIncMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_FIXED
  *         @arg @ref LL_DMA_DEST_INCREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestIncMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DINC));
}

/**
  * @brief Set destination data width.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DDW_LOG2           LL_DMA_SetDestDataWidth
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestDataWidth This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_BYTE
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_HALFWORD
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_WORD
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_DOUBLEWORD
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestDataWidth(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t  DestDataWidth)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DDW_LOG2,
             DestDataWidth);
}

/**
  * @brief Get destination data width.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          DDW_LOG2           LL_DMA_GetDestDataWidth
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_BYTE
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_HALFWORD
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_WORD
  *         @arg @ref LL_DMA_DEST_DATAWIDTH_DOUBLEWORD
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestDataWidth(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_DDW_LOG2));
}

/**
  * @brief Set source allocated port.
  * @rmtoll CTR1          SAP           LL_DMA_SetSrcAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcAllocatedPort This parameter can be one of the following values:
  *         @arg @ref LL_DMA_SRC_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_SRC_ALLOCATED_PORT1
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAllocatedPort)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SAP,
             SrcAllocatedPort);
}

/**
  * @brief Get source allocated port.
  * @rmtoll CTR1          SAP           LL_DMA_GetSrcAllocatedPort
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_SRC_ALLOCATED_PORT0
  *         @arg @ref LL_DMA_SRC_ALLOCATED_PORT1
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcAllocatedPort(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SAP));
}

/**
  * @brief Set data alignment mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          PAM           LL_DMA_SetDataAlignment
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DataAlignment This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DATA_ALIGN_ZEROPADD
  *         @arg @ref LL_DMA_DATA_ALIGN_SIGNEXTPADD
  *         @arg @ref LL_DMA_DATA_PACK_UNPACK
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDataAlignment(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DataAlignment)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_PAM,
             DataAlignment);
}

/**
  * @brief Get data alignment mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          PAM           LL_DMA_GetDataAlignment
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DATA_ALIGN_ZEROPADD
  *         @arg @ref LL_DMA_DATA_ALIGN_SIGNEXTPADD
  *         @arg @ref LL_DMA_DATA_PACK_UNPACK
  */
__STATIC_INLINE uint32_t LL_DMA_GetDataAlignment(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_PAM));
}

/**
  * @brief Set source burst length.
  * @rmtoll CTR1          SBL_1           LL_DMA_SetSrcBurstLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcBurstLength Between 1 to 64
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcBurstLength(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcBurstLength)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SBL_1,
             ((SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1);
}

/**
  * @brief Get source burst length.
  * @rmtoll CTR1          SBL_1           LL_DMA_GetSrcBurstLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 1 to 64
  * @retval None.
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcBurstLength(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1,
                    DMA_CTR1_SBL_1) >> DMA_CTR1_SBL_1_Pos) + 1U);
}

/**
  * @brief Set source increment mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SINC           LL_DMA_SetSrcIncMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcInc This parameter can be one of the following values:
  *         @arg @ref LL_DMA_SRC_FIXED
  *         @arg @ref LL_DMA_SRC_INCREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcIncMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcInc)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SINC, SrcInc);
}

/**
  * @brief Get source increment mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SINC           LL_DMA_GetSrcIncMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_SRC_FIXED
  *         @arg @ref LL_DMA_SRC_INCREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcIncMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SINC));
}

/**
  * @brief Set source data width.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SDW_LOG2           LL_DMA_SetSrcDataWidth
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcDataWidth This parameter can be one of the following values:
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_BYTE
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_HALFWORD
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_WORD
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_DOUBLEWORD
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcDataWidth(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t  SrcDataWidth)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SDW_LOG2,
             SrcDataWidth);
}

/**
  * @brief Get Source Data width.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR1          SDW_LOG2           LL_DMA_GetSrcDataWidth
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_BYTE
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_HALFWORD
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_WORD
  *         @arg @ref LL_DMA_SRC_DATAWIDTH_DOUBLEWORD
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcDataWidth(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR1, DMA_CTR1_SDW_LOG2));
}

/**
  * @brief Configure channel transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2        TCEM               LL_DMA_ConfigChannelTransfer\n
  *         CTR2        TRIGPOL            LL_DMA_ConfigChannelTransfer\n
  *         CTR2        TRIGM              LL_DMA_ConfigChannelTransfer\n
  *         CTR2        BREQ               LL_DMA_ConfigChannelTransfer\n
  *         CTR2        DREQ               LL_DMA_ConfigChannelTransfer\n
  *         CTR2        SWREQ              LL_DMA_ConfigChannelTransfer\n
  *         CTR2        PFREQ              LL_DMA_ConfigChannelTransfer
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_TCEM_BLK_TRANSFER          or @ref LL_DMA_TCEM_RPT_BLK_TRANSFER      or
  *              @ref LL_DMA_TCEM_EACH_LLITEM_TRANSFER  or @ref LL_DMA_TCEM_LAST_LLITEM_TRANSFER
  *         @arg @ref LL_DMA_HWREQUEST_SINGLEBURST      or @ref LL_DMA_HWREQUEST_BLK
  *         @arg @ref LL_DMA_TRIG_POLARITY_MASKED       or @ref LL_DMA_TRIG_POLARITY_RISING       or
  *              @ref LL_DMA_TRIG_POLARITY_FALLING
  *         @arg @ref LL_DMA_TRIGM_BLK_TRANSFER         or @ref LL_DMA_TRIGM_RPT_BLK_TRANSFER     or
  *              @ref LL_DMA_TRIGM_LLI_LINK_TRANSFER    or @ref LL_DMA_TRIGM_SINGLBURST_TRANSFER
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY or @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH or
  *              @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  *         @arg @ref LL_DMA_NORMAL                     or @ref LL_DMA_PFCTRL
  *@retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigChannelTransfer(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2,
             (DMA_CTR2_TCEM | DMA_CTR2_TRIGPOL | DMA_CTR2_TRIGM | DMA_CTR2_DREQ | DMA_CTR2_SWREQ | DMA_CTR2_BREQ |
              DMA_CTR2_PFREQ), Configuration);
}

/**
  * @brief Set transfer event mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TCEM           LL_DMA_SetTransferEventMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  TransferEventMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_TCEM_BLK_TRANSFER
  *         @arg @ref LL_DMA_TCEM_RPT_BLK_TRANSFER
  *         @arg @ref LL_DMA_TCEM_EACH_LLITEM_TRANSFER
  *         @arg @ref LL_DMA_TCEM_LAST_LLITEM_TRANSFER
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetTransferEventMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t  TransferEventMode)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TCEM,
             TransferEventMode);
}

/**
  * @brief Get transfer event mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TCEM           LL_DMA_GetTransferEventMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_TCEM_BLK_TRANSFER
  *         @arg @ref LL_DMA_TCEM_RPT_BLK_TRANSFER
  *         @arg @ref LL_DMA_TCEM_EACH_LLITEM_TRANSFER
  *         @arg @ref LL_DMA_TCEM_LAST_LLITEM_TRANSFER
  */
__STATIC_INLINE uint32_t LL_DMA_GetTransferEventMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TCEM));
}

/**
  * @brief Set trigger polarity.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TRIGPOL            LL_DMA_SetTriggerPolarity
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  TriggerPolarity This parameter can be one of the following values:
  *         @arg @ref LL_DMA_TRIG_POLARITY_MASKED
  *         @arg @ref LL_DMA_TRIG_POLARITY_RISING
  *         @arg @ref LL_DMA_TRIG_POLARITY_FALLING
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetTriggerPolarity(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t TriggerPolarity)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TRIGPOL,
             TriggerPolarity);
}

/**
  * @brief Get trigger polarity.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TRIGPOL            LL_DMA_GetTriggerPolarity
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_TRIG_POLARITY_MASKED
  *         @arg @ref LL_DMA_TRIG_POLARITY_RISING
  *         @arg @ref LL_DMA_TRIG_POLARITY_FALLING
  */
__STATIC_INLINE uint32_t LL_DMA_GetTriggerPolarity(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TRIGPOL));
}

/**
  * @brief Set trigger Mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TRIGM            LL_DMA_SetTriggerMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  TriggerMode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_TRIGM_BLK_TRANSFER
  *         @arg @ref LL_DMA_TRIGM_RPT_BLK_TRANSFER (This value is allowed only for 2D addressing channels)
  *         @arg @ref LL_DMA_TRIGM_LLI_LINK_TRANSFER
  *         @arg @ref LL_DMA_TRIGM_SINGLBURST_TRANSFER
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetTriggerMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t TriggerMode)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TRIGM,
             TriggerMode);
}

/**
  * @brief Get trigger Mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          TRIGM            LL_DMA_GetTriggerMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_TRIGM_BLK_TRANSFER
  *         @arg @ref LL_DMA_TRIGM_RPT_BLK_TRANSFER (This value is allowed only for 2D addressing channels)
  *         @arg @ref LL_DMA_TRIGM_LLI_LINK_TRANSFER
  *         @arg @ref LL_DMA_TRIGM_SINGLBURST_TRANSFER
  */
__STATIC_INLINE uint32_t LL_DMA_GetTriggerMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TRIGM));
}

/**
  * @brief Set destination hardware and software transfer request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          DREQ           LL_DMA_SetDataTransferDirection\n
  * @rmtoll CTR2          SWREQ          LL_DMA_SetDataTransferDirection
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Direction This parameter can be one of the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDataTransferDirection(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Direction)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2,
             DMA_CTR2_DREQ | DMA_CTR2_SWREQ, Direction);
}

/**
  * @brief Get destination hardware and software transfer request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          DREQ           LL_DMA_GetDataTransferDirection\n
  * @rmtoll CTR2          SWREQ          LL_DMA_GetDataTransferDirection
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_DIRECTION_PERIPH_TO_MEMORY
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_PERIPH
  *         @arg @ref LL_DMA_DIRECTION_MEMORY_TO_MEMORY
  */
__STATIC_INLINE uint32_t LL_DMA_GetDataTransferDirection(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2,
                   DMA_CTR2_DREQ | DMA_CTR2_SWREQ));
}

/**
  * @brief Set block hardware request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          BREQ           LL_DMA_SetBlkHWRequest\n
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkHWRequest This parameter can be one of the following values:
  *         @arg @ref LL_DMA_HWREQUEST_SINGLEBURST
  *         @arg @ref LL_DMA_HWREQUEST_BLK
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkHWRequest(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t BlkHWRequest)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_BREQ,
             BlkHWRequest);
}

/**
  * @brief Get block hardware request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2          BREQ           LL_DMA_GetBlkHWRequest\n
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_HWREQUEST_SINGLEBURST
  *         @arg @ref LL_DMA_HWREQUEST_BLK
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkHWRequest(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_BREQ));
}

/**
  * @brief Set hardware request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         REQSEL     LL_DMA_SetPeriphRequest
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Request This parameter can be one of the following values:
  *         @arg @ref LL_HPDMA1_REQUEST_JPEG_RX
  *         @arg @ref LL_HPDMA1_REQUEST_JPEG_TX
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI1
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI2
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI3
  *         @arg @ref LL_HPDMA1_REQUEST_FMC2_TXRX
  *         @arg @ref LL_HPDMA1_REQUEST_FMC2_BCH
  *         @arg @ref LL_HPDMA1_REQUEST_ADC1
  *         @arg @ref LL_HPDMA1_REQUEST_ADC2
  *         @arg @ref LL_HPDMA1_REQUEST_CRYP_IN
  *         @arg @ref LL_HPDMA1_REQUEST_CRYP_OUT
  *         @arg @ref LL_HPDMA1_REQUEST_SAES_OUT
  *         @arg @ref LL_HPDMA1_REQUEST_SAES_IN
  *         @arg @ref LL_HPDMA1_REQUEST_HASH_IN
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM6_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM7_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_COM
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_UE
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_UE
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_UE
  *         @arg @ref LL_HPDMA1_REQUEST_SPI1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI5_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI5_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI6_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI6_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_HPDMA1_REQUEST_I2C1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART5_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART5_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART6_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART6_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART7_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART7_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART8_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART8_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART9_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART9_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART10_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART10_TX
  *         @arg @ref LL_HPDMA1_REQUEST_LPUART1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_LPUART1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPDIFRX_CS
  *         @arg @ref LL_HPDMA1_REQUEST_SPDIFRX_DT
  *         @arg @ref LL_HPDMA1_REQUEST_ADF1_FLT0
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT0
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT1
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT2
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT3
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT4
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT5
  *         @arg @ref LL_HPDMA1_REQUEST_UCPD1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UCPD1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_DCMI_PSSI
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_TC
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_RS
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_TC
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_RS
  *         @arg @ref LL_GPDMA1_REQUEST_JPEG_RX
  *         @arg @ref LL_GPDMA1_REQUEST_JPEG_TX
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI1
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI2
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI3
  *         @arg @ref LL_GPDMA1_REQUEST_FMC2_TXRX
  *         @arg @ref LL_GPDMA1_REQUEST_FMC2_BCH
  *         @arg @ref LL_GPDMA1_REQUEST_ADC1
  *         @arg @ref LL_GPDMA1_REQUEST_ADC2
  *         @arg @ref LL_GPDMA1_REQUEST_CRYP_IN
  *         @arg @ref LL_GPDMA1_REQUEST_CRYP_OUT
  *         @arg @ref LL_GPDMA1_REQUEST_SAES_OUT
  *         @arg @ref LL_GPDMA1_REQUEST_SAES_IN
  *         @arg @ref LL_GPDMA1_REQUEST_HASH_IN
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM6_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM7_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_COM
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_UE
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_UE
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_UE
  *         @arg @ref LL_GPDMA1_REQUEST_SPI1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI5_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI5_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI6_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI6_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_GPDMA1_REQUEST_I2C1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART5_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART5_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART6_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART6_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART7_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART7_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART8_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART8_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART9_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART9_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART10_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART10_TX
  *         @arg @ref LL_GPDMA1_REQUEST_LPUART1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_LPUART1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPDIFRX_DT
  *         @arg @ref LL_GPDMA1_REQUEST_SPDIFRX_CS
  *         @arg @ref LL_GPDMA1_REQUEST_ADF1_FLT0
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT0
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT1
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT2
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT3
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT4
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT5
  *         @arg @ref LL_GPDMA1_REQUEST_UCPD1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UCPD1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_DCMI_PSSI
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_TC
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_RS
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_TC
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_RS
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetPeriphRequest(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Request)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_REQSEL, Request);
}

/**
  * @brief  Get hardware request.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         REQSEL     LL_DMA_GetPeriphRequest
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_HPDMA1_REQUEST_JPEG_RX
  *         @arg @ref LL_HPDMA1_REQUEST_JPEG_TX
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI1
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI2
  *         @arg @ref LL_HPDMA1_REQUEST_OCTOSPI3
  *         @arg @ref LL_HPDMA1_REQUEST_FMC2_TXRX
  *         @arg @ref LL_HPDMA1_REQUEST_FMC2_BCH
  *         @arg @ref LL_HPDMA1_REQUEST_ADC1
  *         @arg @ref LL_HPDMA1_REQUEST_ADC2
  *         @arg @ref LL_HPDMA1_REQUEST_CRYP_IN
  *         @arg @ref LL_HPDMA1_REQUEST_CRYP_OUT
  *         @arg @ref LL_HPDMA1_REQUEST_SAES_OUT
  *         @arg @ref LL_HPDMA1_REQUEST_SAES_IN
  *         @arg @ref LL_HPDMA1_REQUEST_HASH_IN
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM1_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM2_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM3_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM4_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM5_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM6_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM7_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH3
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_CH4
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM8_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_CH2
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_TRIG
  *         @arg @ref LL_HPDMA1_REQUEST_TIM15_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM16_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM17_COM
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_CH1
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_UP
  *         @arg @ref LL_HPDMA1_REQUEST_TIM18_COM
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM1_UE
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM2_UE
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_IC1
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_IC2
  *         @arg @ref LL_HPDMA1_REQUEST_LPTIM3_UE
  *         @arg @ref LL_HPDMA1_REQUEST_SPI1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI5_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI5_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI6_RX
  *         @arg @ref LL_HPDMA1_REQUEST_SPI6_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_HPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_HPDMA1_REQUEST_I2C1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I2C4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART2_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART2_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART3_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART3_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART4_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART4_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART5_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART5_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART6_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART6_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART7_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART7_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART8_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART8_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UART9_RX
  *         @arg @ref LL_HPDMA1_REQUEST_UART9_TX
  *         @arg @ref LL_HPDMA1_REQUEST_USART10_RX
  *         @arg @ref LL_HPDMA1_REQUEST_USART10_TX
  *         @arg @ref LL_HPDMA1_REQUEST_LPUART1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_LPUART1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_SPDIFRX_CS
  *         @arg @ref LL_HPDMA1_REQUEST_SPDIFRX_DT
  *         @arg @ref LL_HPDMA1_REQUEST_ADF1_FLT0
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT0
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT1
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT2
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT3
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT4
  *         @arg @ref LL_HPDMA1_REQUEST_MDF1_FLT5
  *         @arg @ref LL_HPDMA1_REQUEST_UCPD1_TX
  *         @arg @ref LL_HPDMA1_REQUEST_UCPD1_RX
  *         @arg @ref LL_HPDMA1_REQUEST_DCMI_PSSI
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_TC
  *         @arg @ref LL_HPDMA1_REQUEST_I3C1_RS
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_TC
  *         @arg @ref LL_HPDMA1_REQUEST_I3C2_RS
  *         @arg @ref LL_GPDMA1_REQUEST_JPEG_RX
  *         @arg @ref LL_GPDMA1_REQUEST_JPEG_TX
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI1
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI2
  *         @arg @ref LL_GPDMA1_REQUEST_OCTOSPI3
  *         @arg @ref LL_GPDMA1_REQUEST_FMC2_TXRX
  *         @arg @ref LL_GPDMA1_REQUEST_FMC2_BCH
  *         @arg @ref LL_GPDMA1_REQUEST_ADC1
  *         @arg @ref LL_GPDMA1_REQUEST_ADC2
  *         @arg @ref LL_GPDMA1_REQUEST_CRYP_IN
  *         @arg @ref LL_GPDMA1_REQUEST_CRYP_OUT
  *         @arg @ref LL_GPDMA1_REQUEST_SAES_OUT
  *         @arg @ref LL_GPDMA1_REQUEST_SAES_IN
  *         @arg @ref LL_GPDMA1_REQUEST_HASH_IN
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM1_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM2_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM3_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM4_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM5_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM6_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM7_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH3
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_CH4
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM8_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_CH2
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_TRIG
  *         @arg @ref LL_GPDMA1_REQUEST_TIM15_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM16_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM17_COM
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_CH1
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_UP
  *         @arg @ref LL_GPDMA1_REQUEST_TIM18_COM
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM1_UE
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM2_UE
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_IC1
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_IC2
  *         @arg @ref LL_GPDMA1_REQUEST_LPTIM3_UE
  *         @arg @ref LL_GPDMA1_REQUEST_SPI1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI5_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI5_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI6_RX
  *         @arg @ref LL_GPDMA1_REQUEST_SPI6_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_A
  *         @arg @ref LL_GPDMA1_REQUEST_SAI1_B
  *         @arg @ref LL_GPDMA1_REQUEST_I2C1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I2C4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART2_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART2_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART3_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART3_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART4_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART4_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART5_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART5_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART6_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART6_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART7_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART7_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART8_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART8_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UART9_RX
  *         @arg @ref LL_GPDMA1_REQUEST_UART9_TX
  *         @arg @ref LL_GPDMA1_REQUEST_USART10_RX
  *         @arg @ref LL_GPDMA1_REQUEST_USART10_TX
  *         @arg @ref LL_GPDMA1_REQUEST_LPUART1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_LPUART1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_SPDIFRX_DT
  *         @arg @ref LL_GPDMA1_REQUEST_SPDIFRX_CS
  *         @arg @ref LL_GPDMA1_REQUEST_ADF1_FLT0
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT0
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT1
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT2
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT3
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT4
  *         @arg @ref LL_GPDMA1_REQUEST_MDF1_FLT5
  *         @arg @ref LL_GPDMA1_REQUEST_UCPD1_TX
  *         @arg @ref LL_GPDMA1_REQUEST_UCPD1_RX
  *         @arg @ref LL_GPDMA1_REQUEST_DCMI_PSSI
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_TC
  *         @arg @ref LL_GPDMA1_REQUEST_I3C1_RS
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_TC
  *         @arg @ref LL_GPDMA1_REQUEST_I3C2_RS
  */
__STATIC_INLINE  uint32_t LL_DMA_GetPeriphRequest(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_REQSEL));
}

/**
  * @brief Set hardware trigger.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         TRIGSEL     LL_DMA_SetHWTrigger
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Trigger This parameter can be one of the following values:
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_CTC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_TC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_TW
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_EOC
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_IFNF
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_IFT
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_OFNE
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_OFT
  *         @arg @ref LL_HPDMA1_TRIGGER_LCD_LI
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_0
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_1
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_2
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_3
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_3
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_2
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_1
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_0
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM1_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM1_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM2_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM2_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM3_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM3_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM4_OUT
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM5_OUT
  *         @arg @ref LL_HPDMA1_TRIGGER_RTC_WKUP
  *         @arg @ref LL_HPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM1_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM1_TRGO2
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM2_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM3_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM4_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM5_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM6_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM7_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM8_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM8_TRGO2
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM12_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM15_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH0_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH1_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH2_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH3_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH4_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH5_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH6_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH7_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH8_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH9_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH10_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH11_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH12_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH13_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH14_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH15_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH0_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH1_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH2_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH3_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH4_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH5_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH6_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH7_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH8_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH9_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH10_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH11_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH12_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH13_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH14_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH15_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT0_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT1_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT2_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT3_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT4_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT5_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT6_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT7_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT8_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT9_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT10_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT11_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT12_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT13_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT14_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT15_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_CTC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_TC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_TW
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_EOC
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_IFNF
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_IFT
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_OFNE
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_OFT
  *         @arg @ref LL_GPDMA1_TRIGGER_LCD_LI
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_0
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_1
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_2
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_3
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_3
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_2
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_1
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_0
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM1_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM1_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM2_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM2_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM3_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM3_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM4_OUT
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM5_OUT
  *         @arg @ref LL_GPDMA1_TRIGGER_RTC_WKUP
  *         @arg @ref LL_GPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM1_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM1_TRGO2
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM2_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM3_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM4_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM5_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM6_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM7_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM8_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM8_TRGO2
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM12_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM15_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH0_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH1_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH2_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH3_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH4_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH5_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH6_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH7_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH8_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH9_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH10_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH11_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH12_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH13_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH14_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH15_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH0_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH1_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH2_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH3_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH4_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH5_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH6_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH7_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH8_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH9_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH10_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH11_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH12_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH13_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH14_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH15_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT0_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT1_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT2_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT3_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT4_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT5_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT6_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT7_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT8_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT9_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT10_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT11_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT12_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT13_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT14_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT15_SYNC
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetHWTrigger(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Trigger)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_TRIGSEL,
             (Trigger << DMA_CTR2_TRIGSEL_Pos) & DMA_CTR2_TRIGSEL);
}

/**
  * @brief Get hardware triggers.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         TRIGSEL     LL_DMA_GetHWTrigger
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P0_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P1_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_FEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_LEND
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_HSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DCMIPP_P2_VSYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_CTC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_TC
  *         @arg @ref LL_HPDMA1_TRIGGER_DMA2D_TW
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_EOC
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_IFNF
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_IFT
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_OFNE
  *         @arg @ref LL_HPDMA1_TRIGGER_JPEG_OFT
  *         @arg @ref LL_HPDMA1_TRIGGER_LCD_LI
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_0
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_1
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_2
  *         @arg @ref LL_HPDMA1_TRIGGER_GPU2D1_GP_3
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_3
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_2
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_1
  *         @arg @ref LL_HPDMA1_TRIGGER_GFXTIM_0
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM1_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM1_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM2_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM2_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM3_CH1
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM3_CH2
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM4_OUT
  *         @arg @ref LL_HPDMA1_TRIGGER_LPTIM5_OUT
  *         @arg @ref LL_HPDMA1_TRIGGER_RTC_WKUP
  *         @arg @ref LL_HPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM1_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM1_TRGO2
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM2_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM3_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM4_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM5_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM6_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM7_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM8_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM8_TRGO2
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM12_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_TIM15_TRGO
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH0_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH1_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH2_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH3_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH4_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH5_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH6_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH7_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH8_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH9_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH10_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH11_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH12_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH13_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH14_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_HPDMA1_CH15_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH0_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH1_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH2_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH3_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH4_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH5_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH6_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH7_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH8_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH9_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH10_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH11_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH12_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH13_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH14_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_GPDMA1_CH15_TCF
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT0_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT1_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT2_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT3_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT4_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT5_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT6_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT7_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT8_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT9_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT10_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT11_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT12_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT13_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT14_SYNC
  *         @arg @ref LL_HPDMA1_TRIGGER_EXTIT15_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P0_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P1_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_FEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_LEND
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_HSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DCMIPP_P2_VSYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_CTC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_TC
  *         @arg @ref LL_GPDMA1_TRIGGER_DMA2D_TW
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_EOC
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_IFNF
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_IFT
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_OFNE
  *         @arg @ref LL_GPDMA1_TRIGGER_JPEG_OFT
  *         @arg @ref LL_GPDMA1_TRIGGER_LCD_LI
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_0
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_1
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_2
  *         @arg @ref LL_GPDMA1_TRIGGER_GPU2D1_GP_3
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_3
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_2
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_1
  *         @arg @ref LL_GPDMA1_TRIGGER_GFXTIM_0
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM1_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM1_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM2_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM2_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM3_CH1
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM3_CH2
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM4_OUT
  *         @arg @ref LL_GPDMA1_TRIGGER_LPTIM5_OUT
  *         @arg @ref LL_GPDMA1_TRIGGER_RTC_WKUP
  *         @arg @ref LL_GPDMA1_TRIGGER_LPUART1_IT_R_WUP_ASYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_LPUART1_IT_T_WUP_ASYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_SPI6_IT_OR_SPI6_AIT_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM1_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM1_TRGO2
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM2_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM3_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM4_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM5_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM6_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM7_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM8_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM8_TRGO2
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM12_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_TIM15_TRGO
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH0_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH1_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH2_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH3_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH4_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH5_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH6_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH7_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH8_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH9_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH10_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH11_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH12_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH13_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH14_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_HPDMA1_CH15_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH0_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH1_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH2_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH3_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH4_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH5_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH6_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH7_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH8_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH9_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH10_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH11_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH12_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH13_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH14_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_GPDMA1_CH15_TCF
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT0_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT1_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT2_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT3_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT4_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT5_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT6_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT7_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT8_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT9_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT10_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT11_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT12_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT13_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT14_SYNC
  *         @arg @ref LL_GPDMA1_TRIGGER_EXTIT15_SYNC
  */
__STATIC_INLINE  uint32_t LL_DMA_GetHWTrigger(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2,
                   DMA_CTR2_TRIGSEL) >> DMA_CTR2_TRIGSEL_Pos);
}

/**
  * @brief Set DMA transfer mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         PFREQ     LL_DMA_SetTransferMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_DMA_NORMAL
  *         @arg @ref LL_DMA_PFCTRL
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetTransferMode(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Mode)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2, DMA_CTR2_PFREQ,
             Mode & DMA_CTR2_PFREQ);
}

/**
  * @brief Get DMA transfer mode.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CTR2         TRIGSEL     LL_DMA_GetTransferMode
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_NORMAL
  *         @arg @ref LL_DMA_PFCTRL
  */
__STATIC_INLINE  uint32_t LL_DMA_GetTransferMode(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR2,
                   DMA_CTR2_PFREQ));
}

/**
  * @brief Configure addresses update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1         BRDDEC            LL_DMA_ConfigBlkRptAddrUpdate\n
  *         CBR1         BRSDEC            LL_DMA_ConfigBlkRptAddrUpdate\n
  *         CBR1         DDEC              LL_DMA_ConfigBlkRptAddrUpdate\n
  *         CBR1         SDEC              LL_DMA_ConfigBlkRptAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_BLKRPT_DEST_ADDR_INCREMENT or @ref LL_DMA_BLKRPT_DEST_ADDR_DECREMENT
  *         @arg @ref LL_DMA_BLKRPT_SRC_ADDR_INCREMENT  or @ref LL_DMA_BLKRPT_SRC_ADDR_DECREMENT
  *         @arg @ref LL_DMA_BURST_DEST_ADDR_INCREMENT  or @ref LL_DMA_BURST_DEST_ADDR_DECREMENT
  *         @arg @ref LL_DMA_BURST_SRC_ADDR_INCREMENT   or @ref LL_DMA_BURST_SRC_ADDR_DECREMENT
  *@retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigBlkRptAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1,
             DMA_CBR1_BRDDEC | DMA_CBR1_BRSDEC | DMA_CBR1_DDEC | DMA_CBR1_SDEC, Configuration);
}

/**
  * @brief Configure DMA Block number of data and repeat Count.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1         BNDT                     LL_DMA_ConfigBlkCounters\n
  *         CBR1         BRC                      LL_DMA_ConfigBlkCounters
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkDataLength Block transfer length
                          Value between 0 to 0x0000FFFF
  * @param  BlkRptCount Block repeat counter
  *                     Value between 0 to 0x000007FF
  *@retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigBlkCounters(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t BlkDataLength,
                                              uint32_t BlkRptCount)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1,
             (DMA_CBR1_BNDT | DMA_CBR1_BRC), (BlkDataLength | (BlkRptCount << DMA_CBR1_BRC_Pos)));
}

/**
  * @brief Set block repeat destination address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          BRDDEC           LL_DMA_SetBlkRptDestAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptDestAddrUpdate This parameter can be one of the following values:
  *         @arg @ref LL_DMA_BLKRPT_DEST_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BLKRPT_DEST_ADDR_DECREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkRptDestAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                    uint32_t BlkRptDestAddrUpdate)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BRDDEC,
             BlkRptDestAddrUpdate);
}

/**
  * @brief Get block repeat destination address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          BRDDEC           LL_DMA_GetBlkRptDestAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_BLKRPT_DEST_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BLKRPT_DEST_ADDR_DECREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkRptDestAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BRDDEC));
}

/**
  * @brief Set block repeat source address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          BRSDEC           LL_DMA_SetBlkRptSrcAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptSrcAddrUpdate This parameter can be one of the following values:
  *         @arg @ref LL_DMA_BLKRPT_SRC_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BLKRPT_SRC_ADDR_DECREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkRptSrcAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                   uint32_t BlkRptSrcAddrUpdate)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BRSDEC,
             BlkRptSrcAddrUpdate);
}

/**
  * @brief Get block repeat source address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          BRSDEC           LL_DMA_GetBlkRptSrcAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_BLKRPT_SRC_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BLKRPT_SRC_ADDR_DECREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkRptSrcAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BRSDEC));
}

/**
  * @brief Set destination address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          DDEC           LL_DMA_SetDestAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestAddrUpdate This parameter can be one of the following values:
  *         @arg @ref LL_DMA_BURST_DEST_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BURST_DEST_ADDR_DECREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestAddrUpdate)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_DDEC,
             DestAddrUpdate);
}

/**
  * @brief Get destination address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          DDEC           LL_DMA_GetDestAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_BURST_DEST_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BURST_DEST_ADDR_DECREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_DDEC));
}

/**
  * @brief Set source address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          SDEC           LL_DMA_SetSrcAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcAddrUpdate This parameter can be one of the following values:
  *         @arg @ref LL_DMA_BURST_SRC_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BURST_SRC_ADDR_DECREMENT
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddrUpdate)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_SDEC,
             SrcAddrUpdate);
}

/**
  * @brief Get source address update.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1          SDEC           LL_DMA_GetSrcAddrUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DMA_BURST_SRC_ADDR_INCREMENT
  *         @arg @ref LL_DMA_BURST_SRC_ADDR_DECREMENT
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcAddrUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_SDEC));
}

/**
  * @brief Set block repeat count.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1        BRC            LL_DMA_SetBlkRptCount
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptCount Block repeat counter
  *                     Value between 0 to 0x000007FF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkRptCount(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t BlkRptCount)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BRC,
             (BlkRptCount << DMA_CBR1_BRC_Pos) & DMA_CBR1_BRC);
}

/**
  * @brief Get block repeat count.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR1        BRC            LL_DMA_GetBlkRptCount
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x000007FF
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkRptCount(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1,
                   DMA_CBR1_BRC) >> DMA_CBR1_BRC_Pos);
}

/**
  * @brief  Set block data length in bytes to transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CBR1        BNDT         LL_DMA_SetBlkDataLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkDataLength Between 0 to 0x0000FFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkDataLength(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t BlkDataLength)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BNDT,
             BlkDataLength);
}

/**
  * @brief  Get block data length in bytes to transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CBR1        BNDT         LL_DMA_GetBlkDataLength
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x0000FFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkDataLength(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR1, DMA_CBR1_BNDT));
}

/**
  * @brief Configure the source and destination addresses.
  * @note  This API is used for all available DMA channels.
  * @note   This API must not be called when the DMA Channel is enabled.
  * @rmtoll CSAR        SA          LL_DMA_ConfigAddresses\n
  *         CDAR        DA          LL_DMA_ConfigAddresses
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcAddress Between 0 to 0xFFFFFFFF
  * @param  DestAddress Between 0 to 0xFFFFFFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigAddresses(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddress, uint32_t
                                            DestAddress)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSAR, SrcAddress);
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CDAR, DestAddress);
}

/**
  * @brief Set source address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSAR        SA         LL_DMA_SetSrcAddress
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcAddress Between 0 to 0xFFFFFFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcAddress(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddress)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSAR, SrcAddress);
}

/**
  * @brief  Get source address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSAR        SA         LL_DMA_GetSrcAddress
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcAddress(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSAR));
}

/**
  * @brief  Set destination address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CDAR        DA         LL_DMA_SetDestAddress
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestAddress Between 0 to 0xFFFFFFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestAddress(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestAddress)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CDAR, DestAddress);
}

/**
  * @brief  Get destination address.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CDAR        DA         LL_DMA_GetDestAddress
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestAddress(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CDAR));
}

/**
  * @brief Configure source and destination addresses offset.
  * @note  This API is used only for 2D addressing channels.
  * @note   This API must not be called when the DMA Channel is enabled.
  * @rmtoll CTR3        DAO          LL_DMA_ConfigAddrUpdateValue\n
  *         CTR3        SAO          LL_DMA_ConfigAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestAddrOffset Between 0 to 0x00001FFF
  * @param  SrcAddrOffset Between 0 to 0x00001FFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddrOffset,
                                                  uint32_t DestAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR3,
            (SrcAddrOffset  & DMA_CTR3_SAO) | ((DestAddrOffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO));
}

/**
  * @brief Set destination address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CTR3       DAO         LL_DMA_SetDestAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  DestAddrOffset Between 0 to 0x00001FFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetDestAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t DestAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR3, DMA_CTR3_DAO,
             ((DestAddrOffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO));
}

/**
  * @brief Get destination address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CDAR        DAO         LL_DMA_GetDestAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x00001FFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetDestAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR3,
                   DMA_CTR3_DAO) >> DMA_CTR3_DAO_Pos);
}

/**
  * @brief Set source address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CTR3        SAO         LL_DMA_SetSrcAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  SrcAddrOffset Between 0 to 0x00001FFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetSrcAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR3, DMA_CTR3_SAO,
             SrcAddrOffset & DMA_CTR3_SAO);
}

/**
  * @brief Get source address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CTR3        SAO         LL_DMA_GetSrcAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x00001FFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetSrcAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CTR3, DMA_CTR3_SAO));
}

/**
  * @brief Configure the block repeated source and destination addresses offset.
  * @note  This API is used only for 2D addressing channels.
  * @note   This API must not be called when the DMA Channel is enabled.
  * @rmtoll CBR2        BRDAO          LL_DMA_ConfigBlkRptAddrUpdateValue\n
  *         CBR2        BRSAO          LL_DMA_ConfigBlkRptAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptDestAddrOffset Between 0 to 0x0000FFFF
  * @param  BlkRptSrcAddrOffset Between 0 to 0x0000FFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigBlkRptAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                        uint32_t BlkRptSrcAddrOffset, uint32_t BlkRptDestAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR2,
            ((BlkRptDestAddrOffset << DMA_CBR2_BRDAO_Pos) & DMA_CBR2_BRDAO) | (BlkRptSrcAddrOffset  & DMA_CBR2_BRSAO));
}

/**
  * @brief Set block repeated destination address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR2       BRDAO         LL_DMA_SetBlkRptDestAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptDestAddrOffset Between 0 to 0x0000FFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkRptDestAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                         uint32_t BlkRptDestAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR2, DMA_CBR2_BRDAO,
             ((BlkRptDestAddrOffset << DMA_CBR2_BRDAO_Pos) & DMA_CBR2_BRDAO));
}

/**
  * @brief Get block repeated destination address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR2        BRDAO         LL_DMA_GetBlkRptDestAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x0000FFFF.
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkRptDestAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR2,
                   DMA_CBR2_BRDAO) >> DMA_CBR2_BRDAO_Pos);
}

/**
  * @brief Set block repeated source address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR2        BRSAO         LL_DMA_SetBlkRptSrcAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  BlkRptSrcAddrOffset Between 0 to 0x0000FFFF
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetBlkRptSrcAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                        uint32_t BlkRptSrcAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR2, DMA_CBR2_BRSAO,
             BlkRptSrcAddrOffset);
}

/**
  * @brief Get block repeated source address offset.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CBR2        BRSAO         LL_DMA_GetBlkRptSrcAddrUpdateValue
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x0000FFFF
  */
__STATIC_INLINE uint32_t LL_DMA_GetBlkRptSrcAddrUpdateValue(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CBR2, DMA_CBR2_BRSAO));
}

/**
  * @brief Configure registers update and node address offset during the link transfer.
  * @note  This API is used for all available DMA channels.
  *        For linear addressing channels, UT3 and UB2 fields are discarded.
  * @rmtoll CLLR          UT1            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          UT2            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          UB1            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          USA            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          UDA            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          UT3            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          UB2            LL_DMA_ConfigLinkUpdate\n
  * @rmtoll CLLR          ULL            LL_DMA_ConfigLinkUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  RegistersUpdate This parameter must be a combination of all the following values:
  *         @arg @ref LL_DMA_UPDATE_CTR1
  *         @arg @ref LL_DMA_UPDATE_CTR2
  *         @arg @ref LL_DMA_UPDATE_CBR1
  *         @arg @ref LL_DMA_UPDATE_CSAR
  *         @arg @ref LL_DMA_UPDATE_CDAR
  *         @arg @ref LL_DMA_UPDATE_CTR3  (This value is allowed only for 2D addressing channels)
  *         @arg @ref LL_DMA_UPDATE_CBR2  (This value is allowed only for 2D addressing channels)
  *         @arg @ref LL_DMA_UPDATE_CLLR
  * @param  LinkedListAddrOffset Between 0 to 0x0000FFFC by increment of 4 Bytes.
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ConfigLinkUpdate(const DMA_TypeDef *DMAx, uint32_t Channel, uint32_t RegistersUpdate,
                                             uint32_t LinkedListAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR,
             (DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | DMA_CLLR_UDA | DMA_CLLR_UT3 | \
              DMA_CLLR_UB2 | DMA_CLLR_ULL | DMA_CLLR_LA), (RegistersUpdate | (LinkedListAddrOffset & DMA_CLLR_LA)));
}

/**
  * @brief Enable CTR1 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT1            LL_DMA_EnableCTR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCTR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT1);
}

/**
  * @brief Disable CTR1 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT1            LL_DMA_DisableCTR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCTR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT1);
}

/**
  * @brief Check if CTR1 update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT1            LL_DMA_IsEnabledCTR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCTR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT1)
           == (DMA_CLLR_UT1)) ? 1UL : 0UL);
}

/**
  * @brief Enable CTR2 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT2            LL_DMA_EnableCTR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCTR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT2);
}

/**
  * @brief Disable CTR2 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT2            LL_DMA_DisableCTR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCTR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT2);
}

/**
  * @brief Check if CTR2 update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UT2            LL_DMA_IsEnabledCTR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCTR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT2)
           == (DMA_CLLR_UT2)) ? 1UL : 0UL);
}

/**
  * @brief Enable CBR1 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UB1            LL_DMA_EnableCBR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCBR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB1);
}

/**
  * @brief Disable CBR1 update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UB1            LL_DMA_DisableCBR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCBR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB1);
}

/**
  * @brief Check if CBR1 update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UB1            LL_DMA_IsEnabledCBR1Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCBR1Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB1)
           == (DMA_CLLR_UB1)) ? 1UL : 0UL);
}

/**
  * @brief Enable CSAR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          USA            LL_DMA_EnableCSARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCSARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_USA);
}

/**
  * @brief Disable CSAR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          USA            LL_DMA_DisableCSARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCSARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_USA);
}

/**
  * @brief Check if CSAR update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          USA            LL_DMA_IsEnabledCSARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCSARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_USA)
           == (DMA_CLLR_USA)) ? 1UL : 0UL);
}

/**
  * @brief Enable CDAR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UDA            LL_DMA_EnableCDARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCDARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UDA);
}

/**
  * @brief Disable CDAR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UDA            LL_DMA_DisableCDARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCDARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UDA);
}

/**
  * @brief Check if CDAR update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          UDA            LL_DMA_IsEnabledCDARUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCDARUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UDA)
           == (DMA_CLLR_UDA)) ? 1UL : 0UL);
}

/**
  * @brief Enable CTR3 update during the link transfer.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UT3            LL_DMA_EnableCTR3Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCTR3Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT3);
}

/**
  * @brief Disable CTR3 update during the link transfer.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UT3            LL_DMA_DisableCTR3Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCTR3Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT3);
}

/**
  * @brief Check if CTR3 update during the link transfer is enabled.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UT3            LL_DMA_IsEnabledCTR3Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCTR3Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UT3)
           == (DMA_CLLR_UT3)) ? 1UL : 0UL);
}

/**
  * @brief Enable CBR2 update during the link transfer.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UB2            LL_DMA_EnableCBR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCBR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB2);
}

/**
  * @brief Disable CBR2 update during the link transfer.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UB2            LL_DMA_DisableCBR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCBR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB2);
}

/**
  * @brief Check if CBR2 update during the link transfer is enabled.
  * @note  This API is used only for 2D addressing channels.
  * @rmtoll CLLR          UB2            LL_DMA_IsEnabledCBR2Update
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCBR2Update(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_UB2)
           == (DMA_CLLR_UB2)) ? 1UL : 0UL);
}

/**
  * @brief Enable CLLR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          ULL            LL_DMA_EnableCLLRUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableCLLRUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_ULL);
}

/**
  * @brief Disable CLLR update during the link transfer.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          ULL            LL_DMA_DisableCLLRUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableCLLRUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_ULL);
}

/**
  * @brief Check if CLLR update during the link transfer is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          ULL            LL_DMA_IsEnabledCLLRUpdate
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledCLLRUpdate(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_ULL)
           == (DMA_CLLR_ULL)) ? 1UL : 0UL);
}

/**
  * @brief Set linked list address offset.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          LA           LL_DMA_SetLinkedListAddrOffset
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @param  LinkedListAddrOffset Between 0 to 0x0000FFFC by increment of 4 Bytes.
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_SetLinkedListAddrOffset(const DMA_TypeDef *DMAx, uint32_t Channel,
                                                    uint32_t  LinkedListAddrOffset)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  MODIFY_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR, DMA_CLLR_LA,
             (LinkedListAddrOffset & DMA_CLLR_LA));
}

/**
  * @brief Get linked list address offset.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CLLR          LA           LL_DMA_GetLinkedListAddrOffset
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x0000FFFC.
  */
__STATIC_INLINE uint32_t LL_DMA_GetLinkedListAddrOffset(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CLLR,
                   DMA_CLLR_LA) >> DMA_CLLR_LA_Pos);
}

/**
  * @brief Get FIFO level.
  * @rmtoll CSR          FIFOL           LL_DMA_GetFIFOLevel
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval Between 0 to 0x000000FF.
  */
__STATIC_INLINE uint32_t LL_DMA_GetFIFOLevel(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return (READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR,
                   DMA_CSR_FIFOL) >> DMA_CSR_FIFOL_Pos);
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief Enable the DMA channel secure attribute.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SECCFGR  SECx    LL_DMA_EnableChannelSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannelSecure(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(DMAx->SECCFGR, (DMA_SECCFGR_SEC0 << (Channel & 0x0000000FU)));
}

/**
  * @brief Disable the DMA channel secure attribute.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SECCFGR  SECx    LL_DMA_DisableChannelSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableChannelSecure(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(DMAx->SECCFGR, (DMA_SECCFGR_SEC0 << (Channel & 0x0000000FU)));
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief Check if DMA channel secure is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SECCFGR             SECx          LL_DMA_IsEnabledChannelSecure
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannelSecure(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  return ((READ_BIT(DMAx->SECCFGR, (DMA_SECCFGR_SEC0 << (Channel & 0x0000000FU)))
           == (DMA_SECCFGR_SEC0 << (Channel & 0x0000000FU))) ? 1UL : 0UL);
}

/**
  * @brief Enable the DMA channel privilege attribute.
  * @note  This API is used for all available DMA channels.
  * @rmtoll PRIVCFGR          PRIVx      LL_DMA_EnableChannelPrivilege
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannelPrivilege(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(DMAx->PRIVCFGR, (DMA_PRIVCFGR_PRIV0 << (Channel & 0x0000000FU)));
}

/**
  * @brief Disable the DMA channel privilege attribute.
  * @note  This API is used for all available DMA channels.
  * @rmtoll PRIVCFGR          PRIVx      LL_DMA_DisableChannelPrivilege
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableChannelPrivilege(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(DMAx->PRIVCFGR, (DMA_PRIVCFGR_PRIV0 << (Channel & 0x0000000FU)));
}

/**
  * @brief Check if DMA Channel privilege is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll PRIVCFGR          PRIVx      LL_DMA_IsEnabledChannelPrivilege
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannelPrivilege(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  return ((READ_BIT(DMAx->PRIVCFGR, (DMA_PRIVCFGR_PRIV0 << (Channel & 0x0000000FU)))
           == (DMA_PRIVCFGR_PRIV0 << (Channel & 0x0000000FU))) ? 1UL : 0UL);
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief Enable the DMA channel lock attributes.
  * @note  This API is used for all available DMA channels.
  * @rmtoll RCFGLOCKR           LOCKx         LL_DMA_EnableChannelLockAttribute
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableChannelLockAttribute(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(DMAx->RCFGLOCKR, (DMA_RCFGLOCKR_LOCK0 << (Channel & 0x0000000FU)));
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief Check if DMA channel attributes are locked.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SECCFGR            LOCKx       LL_DMA_IsEnabledChannelLockAttribute
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannelLockAttribute(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  return ((READ_BIT(DMAx->RCFGLOCKR, (DMA_RCFGLOCKR_LOCK0 << (Channel & 0x0000000FU)))
           == (DMA_RCFGLOCKR_LOCK0 << (Channel & 0x0000000FU))) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup DMA_LL_EF_FLAG_Management Flag Management
  * @{
  */

/**
  * @brief Clear trigger overrun flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          TOF        LL_DMA_ClearFlag_TO
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TO(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_TOF);
}

/**
  * @brief Clear suspension flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          SUSPF        LL_DMA_ClearFlag_SUSP
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_SUSP(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_SUSPF);
}

/**
  * @brief Clear user setting error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          USEF         LL_DMA_ClearFlag_USE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_USE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_USEF);
}

/**
  * @brief Clear link transfer error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          ULEF         LL_DMA_ClearFlag_ULE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_ULE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_ULEF);
}

/**
  * @brief Clear data transfer error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          DTEF         LL_DMA_ClearFlag_DTE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_DTE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_DTEF);
}

/**
  * @brief Clear half transfer flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          HTF          LL_DMA_ClearFlag_HT
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_HT(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_HTF);
}

/**
  * @brief Clear transfer complete flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CFCR          TCF          LL_DMA_ClearFlag_TC
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_ClearFlag_TC(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  WRITE_REG(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CFCR, DMA_CFCR_TCF);
}

/**
  * @brief Get trigger overrun flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           TOF        LL_DMA_IsActiveFlag_TO
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TO(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_TOF)
           == (DMA_CSR_TOF)) ? 1UL : 0UL);
}

/**
  * @brief Get suspension flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           SUSPF        LL_DMA_IsActiveFlag_SUSP
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_SUSP(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_SUSPF)
           == (DMA_CSR_SUSPF)) ? 1UL : 0UL);
}

/**
  * @brief Get user setting error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           USEF         LL_DMA_IsActiveFlag_USE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_USE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_USEF)
           == (DMA_CSR_USEF)) ? 1UL : 0UL);
}

/**
  * @brief Get user setting error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           ULEF         LL_DMA_IsActiveFlag_ULE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_ULE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_ULEF)
           == (DMA_CSR_ULEF)) ? 1UL : 0UL);
}

/**
  * @brief Get data transfer error flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           DTEF         LL_DMA_IsActiveFlag_DTE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_DTE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_DTEF)
           == (DMA_CSR_DTEF)) ? 1UL : 0UL);
}

/**
  * @brief Get half transfer flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           HTF          LL_DMA_IsActiveFlag_HT
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_HTF)
           == (DMA_CSR_HTF)) ? 1UL : 0UL);
}

/**
  * @brief Get transfer complete flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           TCF          LL_DMA_IsActiveFlag_TC
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_TCF)
           == (DMA_CSR_TCF)) ? 1UL : 0UL);
}

/**
  * @brief Get idle flag.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CSR           IDLEF        LL_DMA_IsActiveFlag_IDLE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_IDLE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CSR, DMA_CSR_IDLEF)
           == (DMA_CSR_IDLEF)) ? 1UL : 0UL);
}

/**
  * @brief Check if nsecure masked interrupt is active.
  * @note  This API is used for all available DMA channels.
  * @rmtoll MISR  MISx    LL_DMA_IsActiveFlag_MIS
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_MIS(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  return ((READ_BIT(DMAx->MISR, (DMA_MISR_MIS0 << (Channel & 0x0FU)))
           == (DMA_MISR_MIS0 << (Channel & 0x0FU))) ? 1UL : 0UL);
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief Check if secure masked interrupt is active.
  * @note  This API is used for all available DMA channels.
  * @rmtoll SMISR  MISx    LL_DMA_IsActiveFlag_SMIS
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_SMIS(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  return ((READ_BIT(DMAx->SMISR, (DMA_SMISR_MIS0 << (Channel & 0x0000000FU)))
           == (DMA_SMISR_MIS0 << (Channel & 0x0000000FU))) ? 1UL : 0UL);
}
#endif /* CPU_IN_SECURE_STATE */
/**
  * @}
  */

/** @defgroup DMA_LL_EF_IT_Management Interrupt Management
  * @{
  */

/**
  * @brief Enable trigger overrun interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           TOIE       LL_DMA_EnableIT_TO
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_TO(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TOIE);
}

/**
  * @brief Enable suspension interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           SUSPIE       LL_DMA_EnableIT_SUSP
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_SUSP(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSPIE);
}

/**
  * @brief Enable user setting error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           USEIE        LL_DMA_EnableIT_USE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_USE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_USEIE);
}

/**
  * @brief Enable update link transfer error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           ULEIE        LL_DMA_EnableIT_ULE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_ULE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_ULEIE);
}

/**
  * @brief Enable data transfer error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           DTEIE        LL_DMA_EnableIT_DTE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_DTE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_DTEIE);
}

/**
  * @brief Enable half transfer complete interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           HTIE         LL_DMA_EnableIT_HT
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_HT(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_HTIE);
}

/**
  * @brief Enable transfer complete interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           TCIE         LL_DMA_EnableIT_TC
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_EnableIT_TC(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  SET_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TCIE);
}

/**
  * @brief Disable trigger overrun interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           TOIE       LL_DMA_DisableIT_TO
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_TO(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TOIE);
}

/**
  * @brief Disable suspension interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           SUSPIE       LL_DMA_DisableIT_SUSP
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_SUSP(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSPIE);
}

/**
  * @brief Disable user setting error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           USEIE        LL_DMA_DisableIT_USE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_USE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_USEIE);
}

/**
  * @brief Disable update link transfer error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           ULEIE        LL_DMA_DisableIT_ULE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_ULE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_ULEIE);
}

/**
  * @brief Disable data transfer error interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           DTEIE        LL_DMA_DisableIT_DTE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_DTE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_DTEIE);
}

/**
  * @brief Disable half transfer complete interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           HTIE         LL_DMA_DisableIT_HT
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_HT(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_HTIE);
}

/**
  * @brief Disable transfer complete interrupt.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           TCIE         LL_DMA_DisableIT_TC
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval None.
  */
__STATIC_INLINE void LL_DMA_DisableIT_TC(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  CLEAR_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TCIE);
}

/**
  * @brief Check if trigger overrun interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           TOIE       LL_DMA_IsEnabledIT_TO
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TO(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TOIE)
           == DMA_CCR_TOIE) ? 1UL : 0UL);
}

/**
  * @brief Check if suspension interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           SUSPIE       LL_DMA_IsEnabledIT_SUSP
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_SUSP(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_SUSPIE)
           == DMA_CCR_SUSPIE) ? 1UL : 0UL);
}

/**
  * @brief Check if user setting error interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR           USEIE        LL_DMA_IsEnabledIT_USE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_USE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_USEIE)
           == DMA_CCR_USEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if update link transfer error interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR        ULEIE         LL_DMA_IsEnabledIT_ULE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_ULE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_ULEIE)
           == DMA_CCR_ULEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if data transfer error interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR        DTEIE         LL_DMA_IsEnabledIT_DTE
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_DTE(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_DTEIE)
           == DMA_CCR_DTEIE) ? 1UL : 0UL);
}

/**
  * @brief Check if half transfer complete interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR        HTIE         LL_DMA_IsEnabledIT_HT
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_HT(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_HTIE)
           == DMA_CCR_HTIE) ? 1UL : 0UL);
}

/**
  * @brief Check if transfer complete interrupt is enabled.
  * @note  This API is used for all available DMA channels.
  * @rmtoll CCR        TCIE         LL_DMA_IsEnabledIT_TC
  * @param  DMAx DMAx Instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_DMA_CHANNEL_0
  *         @arg @ref LL_DMA_CHANNEL_1
  *         @arg @ref LL_DMA_CHANNEL_2
  *         @arg @ref LL_DMA_CHANNEL_3
  *         @arg @ref LL_DMA_CHANNEL_4
  *         @arg @ref LL_DMA_CHANNEL_5
  *         @arg @ref LL_DMA_CHANNEL_6
  *         @arg @ref LL_DMA_CHANNEL_7
  *         @arg @ref LL_DMA_CHANNEL_8
  *         @arg @ref LL_DMA_CHANNEL_9
  *         @arg @ref LL_DMA_CHANNEL_10
  *         @arg @ref LL_DMA_CHANNEL_11
  *         @arg @ref LL_DMA_CHANNEL_12
  *         @arg @ref LL_DMA_CHANNEL_13
  *         @arg @ref LL_DMA_CHANNEL_14
  *         @arg @ref LL_DMA_CHANNEL_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TC(const DMA_TypeDef *DMAx, uint32_t Channel)
{
  uint32_t dma_base_addr = (uint32_t)DMAx;
  return ((READ_BIT(((DMA_Channel_TypeDef *)(dma_base_addr + LL_DMA_CH_OFFSET_TAB[Channel]))->CCR, DMA_CCR_TCIE)
           == DMA_CCR_TCIE) ? 1UL : 0UL);
}
/**
  * @}
  */

#if defined (USE_FULL_LL_DRIVER)
/** @defgroup DMA_LL_EF_Init Initialization and de-initialization functions
  * @{
  */
uint32_t LL_DMA_Init(DMA_TypeDef *DMAx, uint32_t Channel, LL_DMA_InitTypeDef *DMA_InitStruct);
uint32_t LL_DMA_DeInit(DMA_TypeDef *DMAx, uint32_t Channel);

void     LL_DMA_StructInit(LL_DMA_InitTypeDef *DMA_InitStruct);
void     LL_DMA_ListStructInit(LL_DMA_InitLinkedListTypeDef *DMA_InitLinkedListStruct);
void     LL_DMA_NodeStructInit(LL_DMA_InitNodeTypeDef *DMA_InitNodeStruct);

uint32_t LL_DMA_List_Init(DMA_TypeDef *DMAx, uint32_t Channel,
                          LL_DMA_InitLinkedListTypeDef *DMA_InitLinkedListStruct);
uint32_t LL_DMA_List_DeInit(DMA_TypeDef *DMAx, uint32_t Channel);

uint32_t LL_DMA_CreateLinkNode(const LL_DMA_InitNodeTypeDef *DMA_InitNodeStruct, LL_DMA_LinkNodeTypeDef *pNode);
void     LL_DMA_ConnectLinkNode(LL_DMA_LinkNodeTypeDef *pPrevLinkNode, uint32_t PrevNodeCLLRIdx,
                                LL_DMA_LinkNodeTypeDef *pNewLinkNode, uint32_t NewNodeCLLRIdx);
void     LL_DMA_DisconnectNextLinkNode(LL_DMA_LinkNodeTypeDef *pLinkNode, uint32_t LinkNodeCLLRIdx);
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* GPDMA1 || HPDMA1 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32N6xx_LL_DMA_H */

