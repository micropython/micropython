/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_i3c.h
  * @author  MCD Application Team
  * @brief   Header file of I3C HAL module.
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef STM32N6xx_HAL_I3C_H
#define STM32N6xx_HAL_I3C_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"
#include "stm32n6xx_ll_i3c.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup I3C
  * @{
  */

/* Exported types ----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Exported_Types I3C Exported Types
  * @{
  */
/** @defgroup I3C_Init_Structure_definition I3C Init Structure definition
  * @brief    I3C Init Structure definition
  * @{
  */
typedef struct
{
  LL_I3C_CtrlBusConfTypeDef CtrlBusCharacteristic; /*!< Specifies the I3C controller bus characteristic configuration
                                                        when Controller mode                                          */

  LL_I3C_TgtBusConfTypeDef  TgtBusCharacteristic;  /*!< Specifies the I3C target bus characteristic configuration
                                                        when Target mode                                              */

} I3C_InitTypeDef;
/**
  * @}
  */

/** @defgroup I3C_FIFO_Config_Structure_definition I3C FIFO Configuration Structure definition
  * @brief    I3C FIFO configuration structure definition
  * @{
  */
typedef struct
{
  uint32_t RxFifoThreshold; /*!< Specifies the I3C Rx FIFO threshold level.
                                 This parameter must be a value of @ref I3C_RX_FIFO_THRESHOLD                         */

  uint32_t TxFifoThreshold; /*!< Specifies the I3C Tx FIFO threshold level.
                                 This parameter must be a value of @ref I3C_TX_FIFO_THRESHOLD                         */

  uint32_t ControlFifo;     /*!< Specifies the I3C control FIFO enable/disable state.
                                 This parameter is configured only with controller mode and it
                                 must be a value of @ref I3C_CONTROL_FIFO_STATE                                       */

  uint32_t StatusFifo;      /*!< Specifies the I3C status FIFO enable/disable state.
                                 This parameter is configured only with controller mode and it
                                 must be a value of @ref I3C_STATUS_FIFO_STATE                                        */
} I3C_FifoConfTypeDef;
/**
  * @}
  */

/** @defgroup I3C_Controller_Config_Structure_definition I3C Controller Configuration Structure definition
  * @brief    I3C controller configuration structure definition
  * @{
  */
typedef struct
{
  uint8_t DynamicAddr;            /*!< Specifies the dynamic address of the controller when goes in target mode.
                                       This parameter must be a number between Min_Data=0x00 and Max_Data=0x7F        */

  uint8_t StallTime;              /*!< Specifies the controller clock stall time in number of kernel clock cycles.
                                       This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF        */

  FunctionalState HotJoinAllowed; /*!< Specifies the Enable/Disable state of the controller Hot Join acknowledgement
                                       when receiving a hot join request from target.
                                       This parameter can be set to ENABLE or DISABLE                                 */

  FunctionalState ACKStallState;  /*!< Specifies the Enable/Disable state of the controller clock stall
                                       on the ACK phase.
                                       This parameter can be set to ENABLE or DISABLE                                 */

  FunctionalState CCCStallState;  /*!< Specifies the Enable/Disable state of the controller clock stall on the
                                       T bit phase of a CCC communication to allow the target to decode command.
                                       This parameter can be set to ENABLE or DISABLE                                 */

  FunctionalState TxStallState;   /*!< Specifies the Enable/Disable state of the controller clock stall on
                                       parity phase of data to allow the target to read received data.
                                       This parameter can be set to ENABLE or DISABLE                                 */

  FunctionalState RxStallState;   /*!< Specifies the Enable/Disable state of the controller clock stall on the T bit
                                       phase of data enable to allow the target to prepare data to be sent.
                                       This parameter can be set to ENABLE or DISABLE                                 */

  FunctionalState HighKeeperSDA;  /*!< Specifies the Enable/Disable state of the controller SDA high keeper.
                                       This parameter can be set to ENABLE or DISABLE                                 */
} I3C_CtrlConfTypeDef;
/**
  * @}
  */

/** @defgroup I3C_Target_Config_Structure_definition I3C Target Configuration Structure definition
  * @brief    I3C target configuration structure definition
  * @{
  */
typedef struct
{
  uint8_t Identifier;                  /*!< Specifies the target characteristic ID (MIPI named reference DCR).
                                            This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF   */

  uint8_t MIPIIdentifier;              /*!< Specifies the bits [12-15] of the 48-provisioned ID
                                            (MIPI named reference PID), other 48-provisioned ID are hardcoded.
                                            This parameter must be a number between Min_Data=0x00 and Max_Data=0x0F   */

  FunctionalState CtrlRoleRequest;     /*!< Specifies the Enable/Disable state of the target authorization request
                                            for a second master Chip.
                                            This parameter can be set to ENABLE or DISABLE                            */

  FunctionalState HotJoinRequest;      /*!< Specifies the Enable/Disable state of the target hot join
                                            authorization request.
                                            This parameter can be set to ENABLE or DISABLE                            */

  FunctionalState IBIRequest;          /*!< Specifies the Enable/Disable state of the target in Band Interrupt
                                            authorization request.
                                            This parameter can be set to ENABLE or DISABLE                            */

  FunctionalState IBIPayload;          /*!< Specifies the Enable/Disable state of sending data payload after
                                            an accepted IBI.
                                            This parameter can be set to ENABLE or DISABLE                            */

  uint32_t IBIPayloadSize;             /*!< Specifies the I3C target payload data size.
                                            This parameter must be a value of @ref I3C_PAYLOAD_SIZE                   */

  uint16_t MaxReadDataSize;            /*!< Specifies the numbers of data bytes that the target can read at maximum.
                                            This parameter must be a number between Min_Data=0x00 and Max_Data=0xFFFF */

  uint16_t MaxWriteDataSize;           /*!< Specifies the numbers of data bytes that the target can write at maximum.
                                            This parameter must be a number between Min_Data=0x00 and Max_Data=0xFFFF */

  FunctionalState CtrlCapability;      /*!< Specifies the Enable/Disable state of the target controller capability.
                                            This parameter can be set to ENABLE or DISABLE                            */

  FunctionalState GroupAddrCapability; /*!< Specifies the Enable/Disable state of the target support of group address
                                            after a controller role hand-off.
                                            This parameter can be set to ENABLE or DISABLE                            */

  uint32_t DataTurnAroundDuration;     /*!< Specifies the I3C target clock-to-data turnaround time.
                                            This parameter must be a value of @ref I3C_TURNAROUND_TIME_TSCO           */

  uint8_t MaxReadTurnAround;           /*!< Specifies the target maximum read turnaround byte.
                                            This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF   */

  uint32_t MaxDataSpeed;               /*!< Specifies the I3C target returned GETMXDS CCC format.
                                            This parameter must be a value of @ref I3C_GETMXDS_FORMAT                 */

  FunctionalState MaxSpeedLimitation;  /*!< Specifies the Enable/Disable state of the target max data speed limitation.
                                            This parameter can be set to ENABLE or DISABLE                            */

  uint32_t HandOffActivityState;       /*!< Specifies the I3C target activity state when becoming controller.
                                            This parameter must be a value of @ref I3C_HANDOFF_ACTIVITY_STATE         */

  FunctionalState HandOffDelay;        /*!< Specifies the Enable/Disable state of the target need of delay to process
                                            the controller role hand-off.
                                            This parameter can be set to ENABLE or DISABLE                            */

  FunctionalState PendingReadMDB;      /*!< Specifies the Enable/Disable state of the transmission of a mandatory
                                            data bytes indicating a pending read notification for GETCAPR CCC command.
                                            This parameter can be set to ENABLE or DISABLE                            */
} I3C_TgtConfTypeDef;
/**
  * @}
  */

/** @defgroup I3C_Device_Config_Structure_definition I3C Device Configuration Structure definition
  * @brief    I3C device configuration structure definition
  * @{
  */
typedef struct
{
  uint8_t         DeviceIndex;       /*!< Specifies the index value of the device in the DEVRx register.
                                          This parameter must be a number between Min_Data=1 and Max_Data=4           */

  uint8_t         TargetDynamicAddr; /*!< Specifies the dynamic address of the target x (1 to 4) connected on the bus.
                                          This parameter must be a number between Min_Data=0x00 and Max_Data=0x7F     */

  FunctionalState IBIAck;            /*!< Specifies the Enable/Disable state of the controller's ACK when receiving
                                          an IBI from a target x (1 to 4) connected on the bus.
                                          This parameter can be set to ENABLE or DISABLE                              */

  FunctionalState IBIPayload;        /*!< Specifies the Enable/Disable state of the controller's receiving IBI payload
                                          after acknowledging an IBI requested from a target x (1 to 4) connected
                                          on the bus.
                                          This parameter can be set to ENABLE or DISABLE                              */

  FunctionalState CtrlRoleReqAck;    /*!< Specifies the Enable/Disable state of the controller's ACK when receiving
                                          a control request from a target x (1 to 4) connected on the bus.
                                          This parameter can be set to ENABLE or DISABLE                              */

  FunctionalState CtrlStopTransfer;  /*!< Specifies the Enable/Disable state of the controller's stop transfer after
                                          receiving an IBI request from a target x (1 to 4) connected on the bus.
                                          This parameter can be set to ENABLE or DISABLE                              */

} I3C_DeviceConfTypeDef;
/**
  * @}
  */

/** @defgroup I3C_mode_structure_definition I3C mode structure definition
  * @brief    I3C Mode structure definition
  * @{
  */
typedef enum
{
  HAL_I3C_MODE_NONE       = 0x00U,  /*!< No I3C communication on going             */
  HAL_I3C_MODE_CONTROLLER = 0x01U,  /*!< I3C communication is in controller Mode   */
  HAL_I3C_MODE_TARGET     = 0x02U,  /*!< I3C communication is in target Mode       */

} HAL_I3C_ModeTypeDef;
/**
  * @}
  */

/** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief    HAL State structure definition
  * @{
  */
typedef enum
{
  HAL_I3C_STATE_RESET       = 0x00U,   /*!< Peripheral is not yet Initialized                   */
  HAL_I3C_STATE_READY       = 0x10U,   /*!< Peripheral Initialized and ready for use            */
  HAL_I3C_STATE_BUSY        = 0x20U,   /*!< An internal process is ongoing                      */
  HAL_I3C_STATE_BUSY_TX     = 0x21U,   /*!< Data Transmission process is ongoing                */
  HAL_I3C_STATE_BUSY_RX     = 0x22U,   /*!< Data Reception process is ongoing                   */
  HAL_I3C_STATE_BUSY_TX_RX  = 0x23U,   /*!< Data Multiple Transfer process is ongoing           */
  HAL_I3C_STATE_BUSY_DAA    = 0x24U,   /*!< Dynamic address assignment process is ongoing       */
  HAL_I3C_STATE_LISTEN      = 0x30U,   /*!< Listen process is ongoing                           */
  HAL_I3C_STATE_ABORT       = 0x60U,   /*!< Abort user request ongoing                          */
  HAL_I3C_STATE_ERROR       = 0xE0U,   /*!< Error                                               */

} HAL_I3C_StateTypeDef;
/**
  * @}
  */

/** @defgroup I3C_CCCInfoTypeDef_Structure_definition I3C CCCInfoTypeDef Structure definition
  * @brief    I3C CCCInfoTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint32_t DynamicAddrValid;    /*!< I3C target Dynamic Address Valid (updated during ENTDAA/RSTDAA/SETNEWDA CCC)
                                     This parameter can be Valid=1U or Not Valid=0U                                   */
  uint32_t DynamicAddr;         /*!< I3C target Dynamic Address (updated during ENTDAA/RSTDAA/SETNEWDA CCC)           */
  uint32_t MaxWriteLength;      /*!< I3C target Maximum Write Length (updated during SETMWL CCC)                      */
  uint32_t MaxReadLength;       /*!< I3C target Maximum Read Length (updated during SETMRL CCC)                       */
  uint32_t ResetAction;         /*!< I3C target Reset Action level (updated during RSTACT CCC)                        */
  uint32_t ActivityState;       /*!< I3C target Activity State (updated during ENTASx CCC)                            */
  uint32_t HotJoinAllowed;      /*!< I3C target Hot Join (updated during ENEC/DISEC CCC)
                                     This parameter can be Allowed=1U or Not Allowed=0U                               */
  uint32_t InBandAllowed;       /*!< I3C target In Band Interrupt (updated during ENEC/DISEC CCC)
                                     This parameter can be Allowed=1U or Not Allowed=0U                               */
  uint32_t CtrlRoleAllowed;     /*!< I3C target Controller Role Request (updated during ENEC/DISEC CCC)
                                     This parameter can be Allowed=1U or Not Allowed=0U                               */
  uint32_t IBICRTgtAddr;        /*!< I3C controller receive Target Address during IBI or Controller Role Request event*/
  uint32_t IBITgtNbPayload;     /*!< I3C controller get Number of Data Payload after an IBI event                     */
  uint32_t IBITgtPayload;       /*!< I3C controller receive IBI Payload after an IBI event                            */

} I3C_CCCInfoTypeDef;
/**
  * @}
  */

/** @defgroup I3C_ControlTypeDef_Structure_definition I3C ControlTypeDef Structure definition
  * @brief    I3C ControlTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint32_t *pBuffer;  /*!< Pointer to the buffer containing the control or status register values */
  uint32_t  Size;     /*!< The size of pBuffer in words                                           */

} I3C_ControlTypeDef;
/**
  * @}
  */

/** @defgroup I3C_DataTypeDef_Structure_definition I3C DataTypeDef Structure definition
  * @brief    I3C DataTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint8_t   *pBuffer;  /*!< Pointer to the buffer containing all data values to transfer */
  uint32_t  Size;      /*!< The size of pBuffer in bytes                                 */

} I3C_DataTypeDef;

/**
  * @}
  */

/** @defgroup I3C_CCCTypeDef_Structure_definition I3C CCCTypeDef Structure definition
  * @brief    I3C CCCTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint8_t            TargetAddr; /*!< Dynamic or Static target Address                                      */
  uint8_t            CCC;        /*!< CCC value code                                                        */
  I3C_DataTypeDef    CCCBuf;     /*!< Contain size of associated data and size of defining byte if any.
                                      Contain pointer to CCC associated data                                */
  uint32_t           Direction;  /*!< CCC read and/or write direction message                               */

} I3C_CCCTypeDef;
/**
  * @}
  */

/** @defgroup I3C_BCRTypeDef_Structure_definition I3C BCRTypeDef Structure definition
  * @brief    I3C BCRTypeDef Structure definition
  * @{
  */
typedef struct
{
  FunctionalState         MaxDataSpeedLimitation;  /*!< Max data speed limitation */
  FunctionalState         IBIRequestCapable;       /*!< IBI request capable */
  FunctionalState         IBIPayload;              /*!< IBI payload data */
  FunctionalState         OfflineCapable;          /*!< Offline capable */
  FunctionalState         VirtualTargetSupport;    /*!< Virtual target support */
  FunctionalState         AdvancedCapabilities;    /*!< Advanced capabilities */
  FunctionalState         DeviceRole;              /*!< Device role */

} I3C_BCRTypeDef;
/**
  * @}
  */

/** @defgroup I3C_PIDTypeDef_Structure_definition I3C PIDTypeDef Structure definition
  * @brief    I3C_PIDTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint16_t  MIPIMID;         /*!< MIPI Manufacturer ID */
  uint8_t   IDTSEL;          /*!< Provisioned ID Type Selector */
  uint16_t  PartID;          /*!< Part ID device vendor to define */
  uint8_t   MIPIID;          /*!< Instance ID */

} I3C_PIDTypeDef;
/**
  * @}
  */

/** @defgroup I3C_ENTDAAPayloadTypeDef_Structure_definition I3C ENTDAAPayloadTypeDef Structure definition
  * @brief    I3C ENTDAAPayloadTypeDef Structure definition
  * @{
  */
typedef struct
{
  I3C_BCRTypeDef   BCR;             /*!< Bus Characteristics Register */
  uint32_t         DCR;             /*!< Device Characteristics Register */
  I3C_PIDTypeDef   PID;             /*!< Provisioned ID */

} I3C_ENTDAAPayloadTypeDef;
/**
  * @}
  */

/** @defgroup I3C_PrivateTypeDef_Structure_definition I3C PrivateTypeDef Structure definition
  * @brief    I3C PrivateTypeDef Structure definition
  * @{
  */
typedef struct
{
  uint8_t            TargetAddr;  /*!< Dynamic or Static target Address                                 */
  I3C_DataTypeDef    TxBuf;       /*!< Buffer structure containing the data to transmit (little endian) */
  I3C_DataTypeDef    RxBuf;       /*!< Buffer structure containing the data to receive (little endian)  */
  uint32_t           Direction;   /*!< Read and/or write message                                        */

} I3C_PrivateTypeDef;
/**
  * @}
  */

/** @defgroup I3C_XferTypeDef_Structure_definition I3C XferTypeDef Structure definition
  * @brief    I3C XferTypeDef Structure definition
  * @{
  */
typedef struct
{
  I3C_ControlTypeDef CtrlBuf;      /*!< Buffer structure containing the control register values  */
  I3C_ControlTypeDef StatusBuf;    /*!< Buffer structure containing the status register values   */
  I3C_DataTypeDef    TxBuf;        /*!< Buffer structure containing the data to transmit         */
  I3C_DataTypeDef    RxBuf;        /*!< Buffer structure containing the data to receive          */

} I3C_XferTypeDef;
/**
  * @}
  */

/** @defgroup I3C_handle_Structure_definition I3C handle Structure definition
  * @brief    I3C handle Structure definition
  * @{
  */
typedef struct __I3C_HandleTypeDef
{
  I3C_TypeDef                *Instance;                           /*!< I3C registers base address                */

  I3C_InitTypeDef            Init;                                /*!< I3C communication parameters              */

  HAL_I3C_ModeTypeDef        Mode;                                /*!< I3C communication mode.
                                                                       This parameter must be a value of
                                                                       @ref I3C_mode_structure_definition        */

  I3C_XferTypeDef            *pXferData;                          /*!< I3C transfer buffers pointer              */

  const I3C_CCCTypeDef       *pCCCDesc;                           /*!< I3C CCC descriptor pointer                */

  const I3C_PrivateTypeDef   *pPrivateDesc;                       /*!< I3C private transfer descriptor pointer   */

  uint32_t                   ControlXferCount;                    /*!< I3C counter indicating the remaining
                                                                       control data bytes to write in
                                                                       the control register                      */

  uint32_t                   RxXferCount;                         /*!< I3C counter indicating the remaining
                                                                       data bytes to receive                     */

  uint32_t                   TxXferCount;                         /*!< I3C counter indicating the remaining
                                                                       data bytes to transmit                    */

#if defined(HAL_DMA_MODULE_ENABLED)
  DMA_HandleTypeDef          *hdmacr;                             /*!< I3C control DMA handle parameters         */

  DMA_HandleTypeDef          *hdmatx;                             /*!< I3C Tx DMA handle parameters              */

  DMA_HandleTypeDef          *hdmarx;                             /*!< I3C Rx DMA handle parameters              */

  DMA_HandleTypeDef          *hdmasr;                             /*!< I3C status DMA handle parameters          */
#endif /* HAL_DMA_MODULE_ENABLED */

  HAL_LockTypeDef            Lock;                                /*!< I3C locking object                        */

  __IO HAL_I3C_StateTypeDef  State;                               /*!< I3C communication state                   */

  __IO HAL_I3C_StateTypeDef  PreviousState;                       /*!< I3C communication previous state          */

  __IO uint32_t              ErrorCode;                           /*!< I3C Error code                            */

  HAL_StatusTypeDef(*XferISR)(struct __I3C_HandleTypeDef *hi3c,
                              uint32_t itMasks);                  /*!< I3C transfer IRQ handler function pointer */

  void(*ptrTxFunc)(struct __I3C_HandleTypeDef *hi3c);             /*!< I3C transmit function pointer             */

  void(*ptrRxFunc)(struct __I3C_HandleTypeDef *hi3c);             /*!< I3C receive function pointer              */

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)

  void (* CtrlTxCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Controller private data and CCC Tx Transfer complete callback                           */

  void (* CtrlRxCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Controller private data and CCC Rx Transfer completed callback                          */

  void (* CtrlMultipleXferCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Controller multiple Direct CCC, I3C private or I2C Transfer completed callback          */

  void (* CtrlDAACpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Controller Dynamic Address Assignment completed callback                                */

  void (* TgtReqDynamicAddrCallback)(struct __I3C_HandleTypeDef *hi3c, uint64_t targetPayload);
  /*!< I3C Controller request dynamic address callback during Dynamic Address Assignment processus */

  void (* TgtTxCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Target private data Tx Transfer completed callback                                      */

  void (* TgtRxCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Target private data Rx Transfer completed callback                                      */

  void (* TgtHotJoinCallback)(struct __I3C_HandleTypeDef *hi3c, uint8_t dynamicAddress);
  /*!< I3C Target Hot-Join callback                                                                */

  void (* NotifyCallback)(struct __I3C_HandleTypeDef *hi3c, uint32_t eventId);
  /*!< I3C Target or Controller asynchronous events callback                                       */

  void (* ErrorCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Error callback                                                                          */

  void (* AbortCpltCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Abort complete callback                                                                 */

  void (* MspInitCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Msp Init callback                                                                       */

  void (* MspDeInitCallback)(struct __I3C_HandleTypeDef *hi3c);
  /*!< I3C Msp DeInit callback                                                                     */

#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

} I3C_HandleTypeDef;
/**
  * @}
  */

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
/** @defgroup HAL_I3C_Callback_ID_definition I3C callback ID definition
  * @brief    HAL I3C callback ID definition
  * @{
  */
typedef enum
{
  /*!< I3C Controller Tx Transfer completed callback ID                  */
  HAL_I3C_CTRL_TX_COMPLETE_CB_ID               = 0x00U,
  /*!< I3C Controller Rx Transfer completed callback ID                  */
  HAL_I3C_CTRL_RX_COMPLETE_CB_ID               = 0x01U,
  /*!< I3C Controller Multiple Transfer completed callback ID            */
  HAL_I3C_CTRL_MULTIPLE_XFER_COMPLETE_CB_ID    = 0x02U,
  /*!< I3C Controller Dynamic Address Assignment completed callback ID   */
  HAL_I3C_CTRL_DAA_COMPLETE_CB_ID              = 0x03U,
  /*!< I3C Controller request dynamic address completed callback ID      */
  HAL_I3C_TGT_REQ_DYNAMIC_ADDR_CB_ID           = 0x04U,
  /*!< I3C Target Tx Transfer completed callback ID                      */
  HAL_I3C_TGT_TX_COMPLETE_CB_ID                = 0x05U,
  /*!< I3C Target Rx Transfer completed callback ID                      */
  HAL_I3C_TGT_RX_COMPLETE_CB_ID                = 0x06U,
  /*!< I3C Target Hot-join notification callback ID                      */
  HAL_I3C_TGT_HOTJOIN_CB_ID                    = 0x07U,
  /*!< I3C Target or Controller receive notification callback ID         */
  HAL_I3C_NOTIFY_CB_ID                         = 0x08U,
  /*!< I3C Error callback ID                                             */
  HAL_I3C_ERROR_CB_ID                          = 0x09U,
  /*!< I3C Abort callback ID                                             */
  HAL_I3C_ABORT_CB_ID                          = 0x0AU,
  /*!< I3C Msp Init callback ID                                          */
  HAL_I3C_MSPINIT_CB_ID                        = 0x0BU,
  /*!< I3C Msp DeInit callback ID                                        */
  HAL_I3C_MSPDEINIT_CB_ID                      = 0x0CU

} HAL_I3C_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_I3C_Callback_Pointer_definition I3C callback Pointer definition
  * @brief    HAL I3C callback pointer definition
  * @{
  */
typedef  void (*pI3C_CallbackTypeDef)(I3C_HandleTypeDef *hi3c);
typedef  void (*pI3C_NotifyCallbackTypeDef)(I3C_HandleTypeDef *hi3c, uint32_t notifyId);
typedef  void (*pI3C_TgtHotJoinCallbackTypeDef)(I3C_HandleTypeDef *hi3c, uint8_t dynamicAddress);
typedef  void (*pI3C_TgtReqDynamicAddrCallbackTypeDef)(I3C_HandleTypeDef *hi3c, uint64_t targetPayload);
/**
  * @}
  */
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

/**
  * @}
  */

/* Exported constants ------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Exported_Constants I3C Exported Constants
  * @{
  */

/** @defgroup HAL_I3C_Notification_ID_definition I3C Notification ID definition
  * @brief    HAL I3C Notification ID definition
  * @{
  */

#define EVENT_ID_GETACCCR   (0x00000001U)
/*!< I3C target complete controller-role hand-off (direct GETACCR CCC) event         */
#define EVENT_ID_IBIEND     (0x00000002U)
/*!< I3C target IBI end process event                                                */
#define EVENT_ID_DAU        (0x00000004U)
/*!< I3C target receive a dynamic address update (ENTDAA/RSTDAA/SETNEWDA CCC) event  */
#define EVENT_ID_GETx       (0x00000008U)
/*!< I3C target receive any direct GETxxx CCC event                                  */
#define EVENT_ID_GETSTATUS  (0x00000010U)
/*!< I3C target receive get status command (direct GETSTATUS CCC) event              */
#define EVENT_ID_SETMWL     (0x00000020U)
/*!< I3C target receive maximum write length update (direct SETMWL CCC) event        */
#define EVENT_ID_SETMRL     (0x00000040U)
/*!< I3C target receive maximum read length update(direct SETMRL CCC) event          */
#define EVENT_ID_RSTACT     (0x00000080U)
/*!< I3C target detect reset pattern (broadcast or direct RSTACT CCC) event          */
#define EVENT_ID_ENTASx     (0x00000100U)
/*!< I3C target receive activity state update (direct or broadcast ENTASx) event     */
#define EVENT_ID_ENEC_DISEC (0x00000200U)
/*!< I3C target receive a direct or broadcast ENEC/DISEC CCC event                   */
#define EVENT_ID_DEFTGTS    (0x00000400U)
/*!< I3C target receive a broadcast DEFTGTS CCC event                                */
#define EVENT_ID_DEFGRPA    (0x00000800U)
/*!< I3C target receive a group addressing (broadcast DEFGRPA CCC) event             */
#define EVENT_ID_WKP        (0x00001000U)
/*!< I3C target wakeup event                                                         */
#define EVENT_ID_IBI        (0x00002000U)
/*!< I3C controller receive IBI event                                                */
#define EVENT_ID_CR         (0x00004000U)
/*!< I3C controller controller-role request event                                    */
#define EVENT_ID_HJ         (0x00008000U)
/*!< I3C controller hot-join event                                                   */
/**
  * @}
  */

/** @defgroup I3C_OPTION_DEFINITION OPTION DEFINITION
  * @note     HAL I3C option value coding follow below described bitmap:
  *           b31
  *                0  : message end type restart
  *                1  : message end type stop
  *           b30-b29-b28-b27
  *                0010  : I3C private message
  *                0011  : direct CCC message
  *                0110  : broadcast CCC message
  *                0100  : I2C private message
  *           b4
  *                0  : message without arbitration header
  *                1  : message with arbitration header
  *           b0
  *                0  : message without defining byte
  *                1  : message with defining byte
  *
  *           other bits (not used)
  * @{
  */
#define I3C_DIRECT_WITH_DEFBYTE_RESTART       (0x18000001U) /*!< Restart between each Direct Command then Stop
                                                                 request for last command.
                                                                 Each Command have an associated defining byte        */
#define I3C_DIRECT_WITH_DEFBYTE_STOP          (0x98000001U) /*!< Stop between each Direct Command.
                                                                 Each Command have an associated defining byte        */
#define I3C_DIRECT_WITHOUT_DEFBYTE_RESTART    (0x18000000U) /*!< Restart between each Direct Command then Stop
                                                                 request for last command.
                                                                 Each Command have not an associated defining byte    */
#define I3C_DIRECT_WITHOUT_DEFBYTE_STOP       (0x98000000U) /*!< Stop between each Direct Command.
                                                                 Each Command have not an associated defining byte    */
#define I3C_BROADCAST_WITH_DEFBYTE_RESTART    (0x30000001U) /*!< Restart between each Broadcast Command then Stop
                                                                 request for last command.
                                                                 Each Command have an associated defining byte        */
#define I3C_BROADCAST_WITH_DEFBYTE_STOP       (0xB0000001U) /*!< Stop between each Broadcast Command.
                                                                 Each Command have an associated defining byte        */
#define I3C_BROADCAST_WITHOUT_DEFBYTE_RESTART (0x30000000U) /*!< Restart between each Broadcast Command then Stop
                                                                 request for last command.
                                                                 Each Command have not an associated defining byte    */
#define I3C_BROADCAST_WITHOUT_DEFBYTE_STOP    (0xB0000000U) /*!< Stop between each Broadcast Command.
                                                                 Each Command have not an associated defining byte    */
#define I3C_PRIVATE_WITH_ARB_RESTART          (0x10000000U) /*!< Restart between each I3C Private message then Stop
                                                                 request for last message.
                                                                 Each Message start with an arbitration header after
                                                                 start bit condition                                  */
#define I3C_PRIVATE_WITH_ARB_STOP             (0x90000000U) /*!< Stop between each I3C Private message.
                                                                 Each Message start with an arbitration header after
                                                                 start bit condition                                  */
#define I3C_PRIVATE_WITHOUT_ARB_RESTART       (0x10000004U) /*!< Restart between each I3C message then Stop request
                                                                 for last message.
                                                                 Each Message start with Target address after start
                                                                 bit condition                                        */
#define I3C_PRIVATE_WITHOUT_ARB_STOP          (0x90000004U) /*!< Stop between each I3C Private message.
                                                                 Each Message start with Target address after
                                                                 start bit condition                                  */
#define I2C_PRIVATE_WITH_ARB_RESTART          (0x20000000U) /*!< Restart between each I2C Private message then Stop
                                                                 request for last message.
                                                                 Each Message start with an arbitration header after
                                                                 start bit condition                                  */
#define I2C_PRIVATE_WITH_ARB_STOP             (0xA0000000U) /*!< Stop between each I2C Private message.
                                                                 Each Message start with an arbitration header after
                                                                 start bit condition                                  */
#define I2C_PRIVATE_WITHOUT_ARB_RESTART       (0x20000004U) /*!< Restart between each I2C message then Stop request
                                                                 for last message.
                                                                 Each Message start with Target address after start
                                                                 bit condition                                        */
#define I2C_PRIVATE_WITHOUT_ARB_STOP          (0xA0000004U) /*!< Stop between each I2C Private message.
                                                                 Each Message start with Target address after start
                                                                 bit condition                                        */
/**
  * @}
  */

/** @defgroup I3C_DYNAMIC_ADDRESS_OPTION_DEFINITION I3C DYNAMIC ADDRESS OPTION DEFINITION
  * @{
  */
#define I3C_RSTDAA_THEN_ENTDAA  (0x00000001U) /*!< Initiate a RSTDAA before a ENTDAA procedure */
#define I3C_ONLY_ENTDAA         (0x00000002U) /*!< Initiate a ENTDAA without RSTDAA            */
/**
  * @}
  */

/** @defgroup I3C_ERROR_CODE_DEFINITION ERROR CODE DEFINITION
  * @{
  */
#define HAL_I3C_ERROR_NONE (0x00000000U)                                /*!< No error                                 */

#define HAL_I3C_ERROR_CE0  (I3C_SER_PERR | LL_I3C_CONTROLLER_ERROR_CE0) /*!< Controller detected an illegally
                                                                             formatted CCC                            */
#define HAL_I3C_ERROR_CE1  (I3C_SER_PERR | LL_I3C_CONTROLLER_ERROR_CE1) /*!< Controller detected that transmitted data
                                                                             on the bus is different than expected    */
#define HAL_I3C_ERROR_CE2  (I3C_SER_PERR | LL_I3C_CONTROLLER_ERROR_CE2) /*!< Controller detected that broadcast address
                                                                             7'h7E has been nacked                    */
#define HAL_I3C_ERROR_CE3  (I3C_SER_PERR | LL_I3C_CONTROLLER_ERROR_CE3) /*!< Controller detected that new Controller
                                                                             did not drive the bus after
                                                                             Controller-role handoff                  */
#define HAL_I3C_ERROR_TE0  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE0)     /*!< Target detected an invalid broadcast
                                                                             address                                  */
#define HAL_I3C_ERROR_TE1  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE1)     /*!< Target detected an invalid CCC Code      */
#define HAL_I3C_ERROR_TE2  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE2)     /*!< Target detected a parity error during
                                                                             a write data                             */
#define HAL_I3C_ERROR_TE3  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE3)     /*!< Target detected a parity error on assigned
                                                                             address during dynamic address
                                                                             arbitration                              */
#define HAL_I3C_ERROR_TE4  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE4)     /*!< Target detected 7'h7E missing after Restart
                                                                             during Dynamic Address Assignment
                                                                             procedure                                */
#define HAL_I3C_ERROR_TE5  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE5)     /*!< Target detected an illegally
                                                                             formatted CCC                            */
#define HAL_I3C_ERROR_TE6  (I3C_SER_PERR | LL_I3C_TARGET_ERROR_TE6)     /*!< Target detected that transmitted data on
                                                                             the bus is different than expected       */
#define HAL_I3C_ERROR_DATA_HAND_OFF (I3C_SER_DERR)  /*!< I3C data error during controller-role hand-off process       */
#define HAL_I3C_ERROR_DATA_NACK     (I3C_SER_DNACK) /*!< I3C data not acknowledged error                              */
#define HAL_I3C_ERROR_ADDRESS_NACK  (I3C_SER_ANACK) /*!< I3C address not acknowledged error                           */
#define HAL_I3C_ERROR_COVR          (I3C_SER_COVR)  /*!< I3C S FIFO Over-Run or C FIFO Under-Run error                */
#define HAL_I3C_ERROR_DOVR          (I3C_SER_DOVR)  /*!< I3C Rx FIFO Over-Run or Tx FIFO Under-Run error              */
#define HAL_I3C_ERROR_STALL         (I3C_SER_STALL) /*!< I3C SCL stall error                                          */
#define HAL_I3C_ERROR_DMA           (0x00010000U)   /*!< DMA transfer error                                           */
#define HAL_I3C_ERROR_TIMEOUT       (0x00020000U)   /*!< Timeout error                                                */
#define HAL_I3C_ERROR_DMA_PARAM     (0x00040000U)   /*!< DMA Parameter Error                                          */
#define HAL_I3C_ERROR_INVALID_PARAM (0x00080000U)   /*!< Invalid Parameters error                                     */
#define HAL_I3C_ERROR_SIZE          (0x00100000U)   /*!< I3C size management error                                    */
#define HAL_I3C_ERROR_NOT_ALLOWED   (0x00200000U)   /*!< I3C operation is not allowed                                 */
#define HAL_I3C_ERROR_DYNAMIC_ADDR  (0x00400000U)   /*!< I3C dynamic address error                                    */

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
#define HAL_I3C_ERROR_INVALID_CALLBACK (0x00800000U) /*!< Invalid Callback error                                      */
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
/**
  * @}
  */

/** @defgroup I3C_SDA_HOLD_TIME SDA HOLD TIME
  * @{
  */
#define HAL_I3C_SDA_HOLD_TIME_0_5  LL_I3C_SDA_HOLD_TIME_0_5 /*!< SDA hold time equal to 0.5 x ti3cclk */
#define HAL_I3C_SDA_HOLD_TIME_1_5  LL_I3C_SDA_HOLD_TIME_1_5 /*!< SDA hold time equal to 1.5 x ti3cclk */
/**
  * @}
  */

/** @defgroup I3C_OWN_ACTIVITY_STATE OWN ACTIVITY STATE
  * @{
  */
#define HAL_I3C_OWN_ACTIVITY_STATE_0  LL_I3C_OWN_ACTIVITY_STATE_0 /*!< Own Controller Activity state 0 */
#define HAL_I3C_OWN_ACTIVITY_STATE_1  LL_I3C_OWN_ACTIVITY_STATE_1 /*!< Own Controller Activity state 1 */
#define HAL_I3C_OWN_ACTIVITY_STATE_2  LL_I3C_OWN_ACTIVITY_STATE_2 /*!< Own Controller Activity state 2 */
#define HAL_I3C_OWN_ACTIVITY_STATE_3  LL_I3C_OWN_ACTIVITY_STATE_3 /*!< Own Controller Activity state 3 */
/**
  * @}
  */

/** @defgroup I3C_RX_FIFO_THRESHOLD RX FIFO THRESHOLD
  * @{
  */
#define HAL_I3C_RXFIFO_THRESHOLD_1_4  LL_I3C_RXFIFO_THRESHOLD_1_4  /*!< Rx Fifo Threshold is 1 byte  */
#define HAL_I3C_RXFIFO_THRESHOLD_4_4  LL_I3C_RXFIFO_THRESHOLD_4_4  /*!< Rx Fifo Threshold is 4 bytes */
/**
  * @}
  */

/** @defgroup I3C_TX_FIFO_THRESHOLD TX FIFO THRESHOLD
  * @{
  */
#define HAL_I3C_TXFIFO_THRESHOLD_1_4  LL_I3C_TXFIFO_THRESHOLD_1_4  /*!< Tx Fifo Threshold is 1 byte  */
#define HAL_I3C_TXFIFO_THRESHOLD_4_4  LL_I3C_TXFIFO_THRESHOLD_4_4  /*!< Tx Fifo Threshold is 4 bytes */
/**
  * @}
  */

/** @defgroup I3C_CONTROL_FIFO_STATE CONTROL FIFO STATE
  * @{
  */
#define HAL_I3C_CONTROLFIFO_DISABLE  0x00000000U      /*!< Control FIFO mode disable */
#define HAL_I3C_CONTROLFIFO_ENABLE   I3C_CFGR_TMODE   /*!< Control FIFO mode enable  */
/**
  * @}
  */

/** @defgroup I3C_STATUS_FIFO_STATE STATUS FIFO STATE
  * @{
  */
#define HAL_I3C_STATUSFIFO_DISABLE  0x00000000U      /*!< Status FIFO mode disable */
#define HAL_I3C_STATUSFIFO_ENABLE   I3C_CFGR_SMODE   /*!< Status FIFO mode enable  */
/**
  * @}
  */

/** @defgroup I3C_DIRECTION DIRECTION
  * @{
  */
#define HAL_I3C_DIRECTION_WRITE     LL_I3C_DIRECTION_WRITE       /*!< Write transfer          */
#define HAL_I3C_DIRECTION_READ      LL_I3C_DIRECTION_READ        /*!< Read transfer           */
#define HAL_I3C_DIRECTION_BOTH      (LL_I3C_DIRECTION_READ | 1U) /*!< Read and Write transfer */
/**
  * @}
  */

/** @defgroup I3C_PAYLOAD_SIZE PAYLOAD SIZE
  * @{
  */
#define HAL_I3C_PAYLOAD_EMPTY    LL_I3C_PAYLOAD_EMPTY   /*!< Empty payload, no additional data after IBI acknowledge */
#define HAL_I3C_PAYLOAD_1_BYTE   LL_I3C_PAYLOAD_1_BYTE  /*!< One additional data byte after IBI acknowledge          */
#define HAL_I3C_PAYLOAD_2_BYTES  LL_I3C_PAYLOAD_2_BYTES /*!< Two additional data bytes after IBI acknowledge         */
#define HAL_I3C_PAYLOAD_3_BYTES  LL_I3C_PAYLOAD_3_BYTES /*!< Three additional data bytes after IBI acknowledge       */
#define HAL_I3C_PAYLOAD_4_BYTES  LL_I3C_PAYLOAD_4_BYTES /*!< Four additional data bytes after IBI acknowledge        */
/**
  * @}
  */

/** @defgroup I3C_HANDOFF_ACTIVITY_STATE HANDOFF ACTIVITY STATE
  * @{
  */
#define HAL_I3C_HANDOFF_ACTIVITY_STATE_0  LL_I3C_HANDOFF_ACTIVITY_STATE_0  /*!< Activity state 0 after handoff */
#define HAL_I3C_HANDOFF_ACTIVITY_STATE_1  LL_I3C_HANDOFF_ACTIVITY_STATE_1  /*!< Activity state 1 after handoff */
#define HAL_I3C_HANDOFF_ACTIVITY_STATE_2  LL_I3C_HANDOFF_ACTIVITY_STATE_2  /*!< Activity state 2 after handoff */
#define HAL_I3C_HANDOFF_ACTIVITY_STATE_3  LL_I3C_HANDOFF_ACTIVITY_STATE_3  /*!< Activity state 3 after handoff */
/**
  * @}
  */

/** @defgroup I3C_GETMXDS_FORMAT GETMXDS FORMAT
  * @{
  */
#define HAL_I3C_GETMXDS_FORMAT_1     LL_I3C_GETMXDS_FORMAT_1      /*!< GETMXDS CCC Format 1 is used, no MaxRdTurn
                                                                       field in response                              */
#define HAL_I3C_GETMXDS_FORMAT_2_LSB LL_I3C_GETMXDS_FORMAT_2_LSB  /*!< GETMXDS CCC Format 2 is used, MaxRdTurn field
                                                                       in response, LSB = RDTURN[7:0]                 */
#define HAL_I3C_GETMXDS_FORMAT_2_MID LL_I3C_GETMXDS_FORMAT_2_MID  /*!< GETMXDS CCC Format 2 is used, MaxRdTurn field
                                                                       in response, Middle byte = RDTURN[7:0]         */
#define HAL_I3C_GETMXDS_FORMAT_2_MSB LL_I3C_GETMXDS_FORMAT_2_MSB  /*!< GETMXDS CCC Format 2 is used, MaxRdTurn field
                                                                       in response, MSB = RDTURN[7:0]                 */
/**
  * @}
  */

/** @defgroup I3C_TURNAROUND_TIME_TSCO TURNAROUND TIME TSCO
  * @{
  */
#define HAL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS    LL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS
/*!< clock-to-data turnaround time tSCO <= 12ns */
#define HAL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS  LL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS
/*!< clock-to-data turnaround time tSCO > 12ns  */
/**
  * @}
  */

/** @defgroup I3C_COMMON_INTERRUPT I3C COMMON INTERRUPT
  * @{
  */
#define HAL_I3C_IT_TXFNFIE      LL_I3C_IER_TXFNFIE /*!< Tx FIFO not full interrupt enable      */
#define HAL_I3C_IT_RXFNEIE      LL_I3C_IER_RXFNEIE /*!< Rx FIFO not empty interrupt enable     */
#define HAL_I3C_IT_FCIE         LL_I3C_IER_FCIE    /*!< Frame complete interrupt enable        */
#define HAL_I3C_IT_ERRIE        LL_I3C_IER_ERRIE   /*!< Error interrupt enable                 */
#define HAL_I3C_ALL_COMMON_ITS  (uint32_t)(LL_I3C_IER_TXFNFIE | LL_I3C_IER_RXFNEIE | \
                                           LL_I3C_IER_FCIE    | LL_I3C_IER_ERRIE)
/**
  * @}
  */

/** @defgroup I3C_TARGET_INTERRUPT I3C TARGET INTERRUPT
  * @{
  */
#define HAL_I3C_IT_IBIENDIE     LL_I3C_IER_IBIENDIE /*!< IBI end interrupt enable                    */
#define HAL_I3C_IT_CRUPDIE      LL_I3C_IER_CRUPDIE  /*!< controller-role update interrupt enable     */
#define HAL_I3C_IT_WKPIE        LL_I3C_IER_WKPIE    /*!< wakeup interrupt enable                     */
#define HAL_I3C_IT_GETIE        LL_I3C_IER_GETIE    /*!< GETxxx CCC interrupt enable                 */
#define HAL_I3C_IT_STAIE        LL_I3C_IER_STAIE    /*!< GETSTATUS CCC interrupt enable              */
#define HAL_I3C_IT_DAUPDIE      LL_I3C_IER_DAUPDIE  /*!< ENTDAA/RSTDAA/SETNEWDA CCC interrupt enable */
#define HAL_I3C_IT_MWLUPDIE     LL_I3C_IER_MWLUPDIE /*!< SETMWL CCC interrupt enable                 */
#define HAL_I3C_IT_MRLUPDIE     LL_I3C_IER_MRLUPDIE /*!< SETMRL CCC interrupt enable                 */
#define HAL_I3C_IT_RSTIE        LL_I3C_IER_RSTIE    /*!< reset pattern interrupt enable              */
#define HAL_I3C_IT_ASUPDIE      LL_I3C_IER_ASUPDIE  /*!< ENTASx CCC interrupt enable                 */
#define HAL_I3C_IT_INTUPDIE     LL_I3C_IER_INTUPDIE /*!< ENEC/DISEC CCC interrupt enable             */
#define HAL_I3C_IT_DEFIE        (LL_I3C_IER_DEFIE | LL_I3C_IER_RXFNEIE)
/*!< DEFTGTS CCC interrupt enable                */
#define HAL_I3C_IT_GRPIE        (LL_I3C_IER_GRPIE | LL_I3C_IER_RXFNEIE)
/*!< DEFGRPA CCC interrupt enable                */
#define HAL_I3C_ALL_TGT_ITS     (uint32_t)(LL_I3C_IER_IBIENDIE | LL_I3C_IER_CRUPDIE  | LL_I3C_IER_WKPIE   | \
                                           LL_I3C_IER_GETIE    | LL_I3C_IER_STAIE    | LL_I3C_IER_DAUPDIE | \
                                           LL_I3C_IER_MWLUPDIE | LL_I3C_IER_MRLUPDIE | LL_I3C_IER_RSTIE   | \
                                           LL_I3C_IER_ASUPDIE  | LL_I3C_IER_INTUPDIE | LL_I3C_IER_DEFIE   | \
                                           LL_I3C_IER_GRPIE)
/**
  * @}
  */

/** @defgroup I3C_CONTROLLER_INTERRUPT I3C CONTROLLER INTERRUPT
  * @{
  */
#define HAL_I3C_IT_CFNFIE      LL_I3C_IER_CFNFIE     /*!< Control FIFO not full interrupt enable     */
#define HAL_I3C_IT_SFNEIE      LL_I3C_IER_SFNEIE     /*!< Status FIFO not empty interrupt enable     */
#define HAL_I3C_IT_HJIE        LL_I3C_IER_HJIE       /*!< Hot-join interrupt enable                  */
#define HAL_I3C_IT_CRIE        LL_I3C_IER_CRIE       /*!< Controller-role request interrupt enable   */
#define HAL_I3C_IT_IBIIE       LL_I3C_IER_IBIIE      /*!< IBI request interrupt enable               */
#define HAL_I3C_IT_RXTGTENDIE  LL_I3C_IER_RXTGTENDIE /*!< Target-initiated read end interrupt enable */
#define HAL_I3C_ALL_CTRL_ITS   (uint32_t)(LL_I3C_IER_CFNFIE | LL_I3C_IER_SFNEIE | LL_I3C_IER_HJIE | \
                                          LL_I3C_IER_CRIE  | LL_I3C_IER_IBIIE  | LL_I3C_IER_RXTGTENDIE)
/**
  * @}
  */

/** @defgroup I3C_FLAGS I3C FLAGS
  * @{
  */
#define HAL_I3C_FLAG_CFEF       LL_I3C_EVR_CFEF      /*!< Control FIFO not empty flag      */
#define HAL_I3C_FLAG_TXFEF      LL_I3C_EVR_TXFEF     /*!< Tx FIFO empty flag               */
#define HAL_I3C_FLAG_CFNFF      LL_I3C_EVR_CFNFF     /*!< Control FIFO not full flag       */
#define HAL_I3C_FLAG_SFNEF      LL_I3C_EVR_SFNEF     /*!< Status FIFO not empty flag       */
#define HAL_I3C_FLAG_TXFNFF     LL_I3C_EVR_TXFNFF    /*!< Tx FIFO not full flag            */
#define HAL_I3C_FLAG_RXFNEF     LL_I3C_EVR_RXFNEF    /*!< Rx FIFO not empty flag           */
#define HAL_I3C_FLAG_RXLASTF    LL_I3C_EVR_RXLASTF   /*!< Last read data byte/word flag    */
#define HAL_I3C_FLAG_TXLASTF    LL_I3C_EVR_TXLASTF   /*!< Last written data byte/word flag */
#define HAL_I3C_FLAG_FCF        LL_I3C_EVR_FCF       /*!< Frame complete flag              */
#define HAL_I3C_FLAG_RXTGTENDF  LL_I3C_EVR_RXTGTENDF /*!< Target-initiated read end flag   */
#define HAL_I3C_FLAG_ERRF       LL_I3C_EVR_ERRF      /*!< Error flag                       */
#define HAL_I3C_FLAG_IBIF       LL_I3C_EVR_IBIF      /*!< IBI request flag                 */
#define HAL_I3C_FLAG_IBIENDF    LL_I3C_EVR_IBIENDF   /*!< IBI end flag                     */
#define HAL_I3C_FLAG_CRF        LL_I3C_EVR_CRF       /*!< Controller-role request flag     */
#define HAL_I3C_FLAG_CRUPDF     LL_I3C_EVR_CRUPDF    /*!< Controller-role update flag      */
#define HAL_I3C_FLAG_HJF        LL_I3C_EVR_HJF       /*!< Hot-join flag                    */
#define HAL_I3C_FLAG_WKPF       LL_I3C_EVR_WKPF      /*!< Wakeup flag                      */
#define HAL_I3C_FLAG_GETF       LL_I3C_EVR_GETF      /*!< GETxxx CCC flag                  */
#define HAL_I3C_FLAG_STAF       LL_I3C_EVR_STAF      /*!< Format 1 GETSTATUS CCC flag      */
#define HAL_I3C_FLAG_DAUPDF     LL_I3C_EVR_DAUPDF    /*!< ENTDAA/RSTDAA/SETNEWDA CCC flag  */
#define HAL_I3C_FLAG_MWLUPDF    LL_I3C_EVR_MWLUPDF   /*!< SETMWL CCC flag                  */
#define HAL_I3C_FLAG_MRLUPDF    LL_I3C_EVR_MRLUPDF   /*!< SETMRL CCC flag                  */
#define HAL_I3C_FLAG_RSTF       LL_I3C_EVR_RSTF      /*!< Reset pattern flag               */
#define HAL_I3C_FLAG_ASUPDF     LL_I3C_EVR_ASUPDF    /*!< ENTASx CCC flag                  */
#define HAL_I3C_FLAG_INTUPDF    LL_I3C_EVR_INTUPDF   /*!< ENEC/DISEC CCC flag              */
#define HAL_I3C_FLAG_DEFF       LL_I3C_EVR_DEFF      /*!< DEFTGTS CCC flag                 */
#define HAL_I3C_FLAG_GRPF       LL_I3C_EVR_GRPF      /*!< DEFGRPA CCC flag                 */
/**
  * @}
  */

/** @defgroup I3C_BCR_IN_PAYLOAD I3C BCR IN PAYLOAD
  * @{
  */
#define HAL_I3C_BCR_IN_PAYLOAD_SHIFT             48                  /*!< BCR field in target payload */
/**
  * @}
  */

/** @defgroup I3C_PATTERN_CONFIGURATION I3C PATTERN CONFIGURATION
  * @{
  */
#define HAL_I3C_TARGET_RESET_PATTERN             0x00000001U        /*!< Target reset pattern */
#define HAL_I3C_HDR_EXIT_PATTERN                 0x00000002U        /*!< HDR exit pattern */
/**
  * @}
  */

/** @defgroup I3C_RESET_PATTERN RESET PATTERN
  * @{
  */
#define HAL_I3C_RESET_PATTERN_DISABLE            0x00000000U
/*!< Standard STOP condition emitted at the end of a frame */
#define HAL_I3C_RESET_PATTERN_ENABLE             I3C_CFGR_RSTPTRN
/*!< Reset pattern is inserted before the STOP condition of any emitted frame */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros ---------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Exported_Macros I3C Exported Macros
  * @{
  */

/** @brief Reset I3C handle state.
  * @param  __HANDLE__ specifies the I3C Handle.
  * @retval None
  */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1)
#define __HAL_I3C_RESET_HANDLE_STATE(__HANDLE__)                do{                                             \
                                                                    (__HANDLE__)->State = HAL_I3C_STATE_RESET;  \
                                                                    (__HANDLE__)->MspInitCallback = NULL;       \
                                                                    (__HANDLE__)->MspDeInitCallback = NULL;     \
                                                                  } while(0)
#else
#define __HAL_I3C_RESET_HANDLE_STATE(__HANDLE__)                ((__HANDLE__)->State = HAL_I3C_STATE_RESET)
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */

/** @brief  Enable the specified I3C interrupt.
  * @param  __HANDLE__ specifies the I3C Handle.
  * @param  __INTERRUPT__ specifies the interrupt source to enable.
  *         This parameter can be one value or a combination of the following group's values:
  *            @arg @ref I3C_CONTROLLER_INTERRUPT
  *            @arg @ref I3C_TARGET_INTERRUPT
  *            @arg @ref I3C_COMMON_INTERRUPT
  * @retval None
  */
#define __HAL_I3C_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/** @brief  Disable the specified I3C interrupt.
  * @param  __HANDLE__ specifies the I3C Handle.
  * @param  __INTERRUPT__ specifies the interrupt source to disable.
  *         This parameter can be one value or a combination of the following group's values:
  *            @arg @ref I3C_CONTROLLER_INTERRUPT
  *            @arg @ref I3C_TARGET_INTERRUPT
  *            @arg @ref I3C_COMMON_INTERRUPT
  * @retval None
  */
#define __HAL_I3C_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->IER &= (~(__INTERRUPT__)))

/** @brief  Check whether the specified I3C flag is set or not.
  * @param  __HANDLE__ specifies the I3C Handle.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one value of the group @arg @ref I3C_FLAGS values.
  * @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_I3C_GET_FLAG(__HANDLE__, __FLAG__) (((((__HANDLE__)->Instance->EVR) &\
                                                    (__FLAG__)) == (__FLAG__)) ? SET : RESET)

/** @brief  Get Bus Characterics in payload (64bits) receive during ENTDAA procedure.
  * @param  __PAYLOAD__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFFFFFFFF.
  * @retval The value of BCR Return value between Min_Data=0x00 and Max_Data=0xFF.
  */
#define __HAL_I3C_GET_BCR(__PAYLOAD__) (((uint32_t)((uint64_t)(__PAYLOAD__) >> HAL_I3C_BCR_IN_PAYLOAD_SHIFT)) & \
                                        I3C_BCR_BCR)

/** @brief  Check IBI request capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The state of IBI request capabilities (ENABLE or DISABLE).
  */
#define __HAL_I3C_GET_IBI_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR1_Msk) >> I3C_BCR_BCR1_Pos) == 1U) \
                                            ? ENABLE : DISABLE)

/** @brief  Check IBI additional data byte capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The state of IBI additional data byte capabilities (ENABLE or DISABLE).
  */
#define __HAL_I3C_GET_IBI_PAYLOAD(__BCR__) (((((__BCR__) & I3C_BCR_BCR2_Msk) >> I3C_BCR_BCR2_Pos) == 1U) \
                                            ? ENABLE : DISABLE)

/** @brief  Check Controller role request capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The state of Controller role request capabilities (ENABLE or DISABLE).
  */
#define __HAL_I3C_GET_CR_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR6_Msk) >> I3C_BCR_BCR6_Pos) == 1U) \
                                           ? ENABLE : DISABLE)

/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/
/** @addtogroup I3C_Exported_Functions
  * @{
  */

/** @addtogroup I3C_Exported_Functions_Group1 Initialization and de-initialization functions.
  * @{
  */
HAL_StatusTypeDef HAL_I3C_Init(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_DeInit(I3C_HandleTypeDef *hi3c);
void HAL_I3C_MspInit(I3C_HandleTypeDef *hi3c);
void HAL_I3C_MspDeInit(I3C_HandleTypeDef *hi3c);
/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group2 Interrupt and callback functions.
  * @{
  */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_I3C_RegisterCallback(I3C_HandleTypeDef *hi3c,
                                           HAL_I3C_CallbackIDTypeDef callbackID,
                                           pI3C_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_I3C_RegisterNotifyCallback(I3C_HandleTypeDef *hi3c,
                                                 pI3C_NotifyCallbackTypeDef pNotifyCallback);
HAL_StatusTypeDef HAL_I3C_RegisterTgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c,
                                                            pI3C_TgtReqDynamicAddrCallbackTypeDef pTgtReqAddrCallback);
HAL_StatusTypeDef HAL_I3C_RegisterTgtHotJoinCallback(I3C_HandleTypeDef *hi3c,
                                                     pI3C_TgtHotJoinCallbackTypeDef pTgtHotJoinCallback);
HAL_StatusTypeDef HAL_I3C_UnRegisterCallback(I3C_HandleTypeDef *hi3c, HAL_I3C_CallbackIDTypeDef callbackID);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

HAL_StatusTypeDef HAL_I3C_ActivateNotification(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData,
                                               uint32_t interruptMask);
HAL_StatusTypeDef HAL_I3C_DeactivateNotification(I3C_HandleTypeDef *hi3c, uint32_t interruptMask);
void HAL_I3C_CtrlTxCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_CtrlRxCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_CtrlMultipleXferCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_CtrlDAACpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c, uint64_t targetPayload);
void HAL_I3C_TgtTxCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_TgtRxCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_TgtHotJoinCallback(I3C_HandleTypeDef *hi3c, uint8_t dynamicAddress);
void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t eventId);
void HAL_I3C_AbortCpltCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_ErrorCallback(I3C_HandleTypeDef *hi3c);
void HAL_I3C_ER_IRQHandler(I3C_HandleTypeDef *hi3c);
void HAL_I3C_EV_IRQHandler(I3C_HandleTypeDef *hi3c);
/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group3 Configuration functions.
  * @{
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_BusCharacteristicConfig(I3C_HandleTypeDef *hi3c,
                                                       const LL_I3C_CtrlBusConfTypeDef *pConfig);
HAL_StatusTypeDef HAL_I3C_Tgt_BusCharacteristicConfig(I3C_HandleTypeDef *hi3c,
                                                      const LL_I3C_TgtBusConfTypeDef *pConfig);
HAL_StatusTypeDef HAL_I3C_SetConfigFifo(I3C_HandleTypeDef *hi3c, const I3C_FifoConfTypeDef *pConfig);
HAL_StatusTypeDef HAL_I3C_Ctrl_Config(I3C_HandleTypeDef *hi3c, const I3C_CtrlConfTypeDef *pConfig);
HAL_StatusTypeDef HAL_I3C_Tgt_Config(I3C_HandleTypeDef *hi3c, const I3C_TgtConfTypeDef *pConfig);
HAL_StatusTypeDef HAL_I3C_Ctrl_ConfigBusDevices(I3C_HandleTypeDef           *hi3c,
                                                const I3C_DeviceConfTypeDef *pDesc,
                                                uint8_t                      nbDevice);
HAL_StatusTypeDef HAL_I3C_AddDescToFrame(I3C_HandleTypeDef         *hi3c,
                                         const I3C_CCCTypeDef      *pCCCDesc,
                                         const I3C_PrivateTypeDef  *pPrivateDesc,
                                         I3C_XferTypeDef           *pXferData,
                                         uint8_t                   nbFrame,
                                         uint32_t                  option);
HAL_StatusTypeDef HAL_I3C_Ctrl_SetConfigResetPattern(I3C_HandleTypeDef *hi3c, uint32_t resetPattern);
HAL_StatusTypeDef HAL_I3C_Ctrl_GetConfigResetPattern(I3C_HandleTypeDef *hi3c, uint32_t *pResetPattern);
/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group4 FIFO Management functions.
  * @{
  */
HAL_StatusTypeDef HAL_I3C_FlushAllFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_FlushTxFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_FlushRxFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_FlushControlFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_FlushStatusFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_ClearConfigFifo(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_GetConfigFifo(I3C_HandleTypeDef *hi3c, I3C_FifoConfTypeDef *pConfig);
/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group5 Controller operational functions.
  * @{
  */
/* Controller transmit direct write or a broadcast CCC command APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC(I3C_HandleTypeDef *hi3c,
                                           I3C_XferTypeDef   *pXferData,
                                           uint32_t           timeout);
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC_IT(I3C_HandleTypeDef *hi3c,
                                              I3C_XferTypeDef   *pXferData);
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC_DMA(I3C_HandleTypeDef *hi3c,
                                               I3C_XferTypeDef   *pXferData);

/* Controller transmit direct read CCC command APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC(I3C_HandleTypeDef *hi3c,
                                          I3C_XferTypeDef   *pXferData,
                                          uint32_t           timeout);
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC_IT(I3C_HandleTypeDef *hi3c,
                                             I3C_XferTypeDef   *pXferData);
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC_DMA(I3C_HandleTypeDef *hi3c,
                                              I3C_XferTypeDef   *pXferData);

/* Controller private write APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit(I3C_HandleTypeDef   *hi3c,
                                        I3C_XferTypeDef     *pXferData,
                                        uint32_t             timeout);
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit_IT(I3C_HandleTypeDef   *hi3c,
                                           I3C_XferTypeDef     *pXferData);
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit_DMA(I3C_HandleTypeDef   *hi3c,
                                            I3C_XferTypeDef     *pXferData);

/* Controller private read APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive(I3C_HandleTypeDef   *hi3c,
                                       I3C_XferTypeDef     *pXferData,
                                       uint32_t             timeout);
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive_IT(I3C_HandleTypeDef   *hi3c,
                                          I3C_XferTypeDef     *pXferData);
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive_DMA(I3C_HandleTypeDef   *hi3c,
                                           I3C_XferTypeDef     *pXferData);

/* Controller multiple Direct CCC Command, I3C private or I2C transfer APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_MultipleTransfer_IT(I3C_HandleTypeDef   *hi3c,
                                                   I3C_XferTypeDef     *pXferData);
HAL_StatusTypeDef HAL_I3C_Ctrl_MultipleTransfer_DMA(I3C_HandleTypeDef   *hi3c,
                                                    I3C_XferTypeDef     *pXferData);

/* Controller assign dynamic address APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_SetDynAddr(I3C_HandleTypeDef *hi3c, uint8_t devAddress);
HAL_StatusTypeDef HAL_I3C_Ctrl_DynAddrAssign_IT(I3C_HandleTypeDef *hi3c, uint32_t dynOption);
HAL_StatusTypeDef HAL_I3C_Ctrl_DynAddrAssign(I3C_HandleTypeDef *hi3c,
                                             uint64_t          *target_payload,
                                             uint32_t           dynOption,
                                             uint32_t           timeout);
/* Controller check device ready APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_IsDeviceI3C_Ready(I3C_HandleTypeDef *hi3c,
                                                 uint8_t            devAddress,
                                                 uint32_t           trials,
                                                 uint32_t           timeout);
HAL_StatusTypeDef HAL_I3C_Ctrl_IsDeviceI2C_Ready(I3C_HandleTypeDef *hi3c,
                                                 uint8_t            devAddress,
                                                 uint32_t           trials,
                                                 uint32_t           timeout);
/* Controller arbitration APIs */
HAL_StatusTypeDef HAL_I3C_Ctrl_GenerateArbitration(I3C_HandleTypeDef *hi3c, uint32_t timeout);

/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group6 Target operational functions.
  * @{
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData, uint32_t timeout);
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit_IT(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData);
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit_DMA(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData);
HAL_StatusTypeDef HAL_I3C_Tgt_Receive(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData, uint32_t timeout);
HAL_StatusTypeDef HAL_I3C_Tgt_Receive_IT(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData);
HAL_StatusTypeDef HAL_I3C_Tgt_Receive_DMA(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData);
HAL_StatusTypeDef HAL_I3C_Tgt_ControlRoleReq(I3C_HandleTypeDef *hi3c, uint32_t timeout);
HAL_StatusTypeDef HAL_I3C_Tgt_ControlRoleReq_IT(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_Tgt_HotJoinReq(I3C_HandleTypeDef *hi3c, uint8_t *pAddress, uint32_t timeout);
HAL_StatusTypeDef HAL_I3C_Tgt_HotJoinReq_IT(I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_Tgt_IBIReq(I3C_HandleTypeDef *hi3c, const uint8_t *pPayload,
                                     uint8_t payloadSize, uint32_t timeout);
HAL_StatusTypeDef HAL_I3C_Tgt_IBIReq_IT(I3C_HandleTypeDef *hi3c, const uint8_t *pPayload, uint8_t payloadSize);
/**
  * @}
  */

/** @addtogroup I3C_Exported_Functions_Group7 Generic and Common functions.
  * @{
  */
HAL_StatusTypeDef HAL_I3C_Abort_IT(I3C_HandleTypeDef *hi3c);
HAL_I3C_StateTypeDef HAL_I3C_GetState(const I3C_HandleTypeDef *hi3c);
HAL_I3C_ModeTypeDef HAL_I3C_GetMode(const I3C_HandleTypeDef *hi3c);
uint32_t HAL_I3C_GetError(const I3C_HandleTypeDef *hi3c);
HAL_StatusTypeDef HAL_I3C_GetCCCInfo(I3C_HandleTypeDef *hi3c,
                                     uint32_t notifyId,
                                     I3C_CCCInfoTypeDef *pCCCInfo);
HAL_StatusTypeDef HAL_I3C_Get_ENTDAA_Payload_Info(I3C_HandleTypeDef *hi3c,
                                                  uint64_t ENTDAA_payload,
                                                  I3C_ENTDAAPayloadTypeDef *pENTDAA_payload);
/**
  * @}
  */

/**
  * @}
  */

/* Private constants -------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Private_Constants I3C Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Private_Macro I3C Private Macros
  * @{
  */
#define IS_I3C_MODE(__MODE__) (((__MODE__) == HAL_I3C_MODE_NONE)       || \
                               ((__MODE__) == HAL_I3C_MODE_CONTROLLER) || \
                               ((__MODE__) == HAL_I3C_MODE_TARGET))

#define IS_I3C_INTERRUPTMASK(__MODE__, __ITMASK__) (((__MODE__) == HAL_I3C_MODE_CONTROLLER)             ? \
                                                    ((((__ITMASK__) & HAL_I3C_ALL_CTRL_ITS) != 0x0U)   || \
                                                     (((__ITMASK__) & HAL_I3C_ALL_COMMON_ITS) != 0x0U)) : \
                                                    ((((__ITMASK__) & HAL_I3C_ALL_TGT_ITS) != 0x0U)    || \
                                                     (((__ITMASK__) & HAL_I3C_ALL_COMMON_ITS) != 0x0U)))

#define IS_I3C_ENTDAA_OPTION(__OPTION__) (((__OPTION__) == I3C_RSTDAA_THEN_ENTDAA) || \
                                          ((__OPTION__) == I3C_ONLY_ENTDAA))

#define IS_I3C_SDAHOLDTIME_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_SDA_HOLD_TIME_0_5) || \
                                             ((__VALUE__) == HAL_I3C_SDA_HOLD_TIME_1_5))

#define IS_I3C_WAITTIME_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_OWN_ACTIVITY_STATE_0) || \
                                          ((__VALUE__) == HAL_I3C_OWN_ACTIVITY_STATE_1) || \
                                          ((__VALUE__) == HAL_I3C_OWN_ACTIVITY_STATE_2) || \
                                          ((__VALUE__) == HAL_I3C_OWN_ACTIVITY_STATE_3))

#define IS_I3C_TXFIFOTHRESHOLD_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_TXFIFO_THRESHOLD_1_4) || \
                                                 ((__VALUE__) == HAL_I3C_TXFIFO_THRESHOLD_4_4))

#define IS_I3C_RXFIFOTHRESHOLD_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_RXFIFO_THRESHOLD_1_4) || \
                                                 ((__VALUE__) == HAL_I3C_RXFIFO_THRESHOLD_4_4))

#define IS_I3C_CONTROLFIFOSTATE_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_CONTROLFIFO_DISABLE) || \
                                                  ((__VALUE__) == HAL_I3C_CONTROLFIFO_ENABLE))

#define IS_I3C_STATUSFIFOSTATE_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_STATUSFIFO_DISABLE) || \
                                                 ((__VALUE__) == HAL_I3C_STATUSFIFO_ENABLE))

#define IS_I3C_DEVICE_VALUE(__VALUE__) (((__VALUE__) >= 1U) && ((__VALUE__) <= 4U))

#define IS_I3C_DYNAMICADDRESS_VALUE(__VALUE__) ((__VALUE__) <= 0x7FU)

#define IS_I3C_FUNCTIONALSTATE_VALUE(__VALUE__) (((__VALUE__) == DISABLE) || \
                                                 ((__VALUE__) == ENABLE))

#define IS_I3C_HANDOFFACTIVITYSTATE_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_HANDOFF_ACTIVITY_STATE_0) || \
                                                      ((__VALUE__) == HAL_I3C_HANDOFF_ACTIVITY_STATE_1) || \
                                                      ((__VALUE__) == HAL_I3C_HANDOFF_ACTIVITY_STATE_2) || \
                                                      ((__VALUE__) == HAL_I3C_HANDOFF_ACTIVITY_STATE_3))

#define IS_I3C_TSCOTIME_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS) || \
                                          ((__VALUE__) == HAL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS))

#define IS_I3C_MAXSPEEDDATA_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_GETMXDS_FORMAT_1    ) || \
                                              ((__VALUE__) == HAL_I3C_GETMXDS_FORMAT_2_LSB) || \
                                              ((__VALUE__) == HAL_I3C_GETMXDS_FORMAT_2_MID) || \
                                              ((__VALUE__) == HAL_I3C_GETMXDS_FORMAT_2_MSB))

#define IS_I3C_IBIPAYLOADSIZE_VALUE(__VALUE__) (((__VALUE__) == HAL_I3C_PAYLOAD_EMPTY  ) || \
                                                ((__VALUE__) == HAL_I3C_PAYLOAD_1_BYTE ) || \
                                                ((__VALUE__) == HAL_I3C_PAYLOAD_2_BYTES) || \
                                                ((__VALUE__) == HAL_I3C_PAYLOAD_3_BYTES) || \
                                                ((__VALUE__) == HAL_I3C_PAYLOAD_4_BYTES))

#define IS_I3C_MIPIIDENTIFIER_VALUE(__VALUE__) ((__VALUE__) <= 0x0FU)

#define IS_I3C_MAXREADTURNARROUND_VALUE(__VALUE__) ((__VALUE__) <= 0xFFU)

#define I3C_CHECK_IT_SOURCE(__IER__, __IT__)  ((((__IER__) & (__IT__)) == (__IT__)) ? SET : RESET)

#define I3C_CHECK_FLAG(__ISR__, __FLAG__) ((((__ISR__) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)

#define IS_I3C_DMASOURCEBYTE_VALUE(__VALUE__) ((__VALUE__) == DMA_SRC_DATAWIDTH_BYTE)

#define IS_I3C_DMASOURCEWORD_VALUE(__VALUE__) ((__VALUE__) == DMA_SRC_DATAWIDTH_WORD)

#define IS_I3C_DMADESTINATIONBYTE_VALUE(__VALUE__) ((__VALUE__) == DMA_DEST_DATAWIDTH_BYTE)

#define IS_I3C_DMADESTINATIONWORD_VALUE(__VALUE__) ((__VALUE__) == DMA_DEST_DATAWIDTH_WORD)

#define I3C_GET_DMA_REMAIN_DATA(__HANDLE__) (__HAL_DMA_GET_COUNTER(__HANDLE__) + HAL_DMAEx_GetFifoLevel(__HANDLE__))
#define IS_I3C_RESET_PATTERN(__RSTPTRN__) (((__RSTPTRN__) == HAL_I3C_RESET_PATTERN_ENABLE)   || \
                                           ((__RSTPTRN__) == HAL_I3C_RESET_PATTERN_DISABLE))
/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Private_Functions I3C Private Functions
  * @{
  */
/* Private functions are defined in stm32n6xx_hal_i3c.c file */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_I3C_H */
