/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_i3c.c
  * @author  MCD Application Team
  * @brief   I3C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Improvement Inter Integrated Circuit (I3C) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
  *
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
  @verbatim
  ======================================================================================================================
                                           ##### How to use this driver #####
  ======================================================================================================================
    [..]
    The I3C HAL driver can be used as follows:

    (#) Declare a I3C_HandleTypeDef handle structure, for example:
        I3C_HandleTypeDef  hi3c;

    (#) Declare a I3C_XferTypeDef transfer descriptor structure, for example:
        I3C_XferTypeDef  ContextBuffers;

    (#)Initialize the I3C low level resources by implementing the HAL_I3C_MspInit() API:
        (##) Enable the I3Cx interface clock
        (##) I3C pins configuration
            (+++) Enable the clock for the I3C GPIOs
            (+++) Configure I3C pins as alternate function push-pull with no-pull
        (##) NVIC configuration if you need to use interrupt process
            (+++) Configure the I3Cx interrupt priority
            (+++) Enable the NVIC I3C IRQ Channel
        (##) DMA Configuration if you need to use DMA process
            (+++) Declare a DMA_HandleTypeDef handle structure for
                  the Command Common Code (CCC) management channel
            (+++) Declare a DMA_HandleTypeDef handle structure for
                  the transmit channel
            (+++) Declare a DMA_HandleTypeDef handle structure for
                  the receive channel
            (+++) Declare a DMA_HandleTypeDef handle structure for
                  the status channel
            (+++) Enable the DMAx interface clock
            (+++) Configure the DMA handle parameters
            (+++) Configure the DMA Command Common Code (CCC) channel
            (+++) Configure the DMA Tx channel
            (+++) Configure the DMA Rx channel
            (+++) Configure the DMA Status channel
            (+++) Associate the initialized DMA handle to the hi3c DMA CCC, Tx, Rx or Status handle as necessary
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on
                  the DMA CCC, Tx, Rx or Status instance

    (#) Configure the HAL I3C Communication Mode as Controller or Target in the hi3c Init structure.

    (#) Configure the Controller Communication Bus characterics for Controller mode.
        This mean, configure the parameters SDAHoldTime, WaitTime, SCLPPLowDuration,
        SCLI3CHighDuration, SCLODLowDuration, SCLI2CHighDuration, BusFreeDuration,
        BusIdleDuration in the LL_I3C_CtrlBusConfTypeDef structure through h3c Init structure.

    (#) Configure the Target Communication Bus characterics for Target mode.
        This mean, configure the parameter BusAvailableDuration in the LL_I3C_TgtBusConfTypeDef structure
        through h3c Init structure.

        All these parameters for Controller or Target can be configured directly in user code or
        by using CubeMx generation.
        To help the computation of the different parameters, the recommendation is to use CubeMx.

        Those parameters can be modified after the hi3c initialization by using
        HAL_I3C_Ctrl_BusCharacteristicConfig() for controller and
        HAL_I3C_Tgt_BusCharacteristicConfig() for target.

    (#) Initialize the I3C registers by calling the HAL_I3C_Init(), configures also the low level Hardware
        (GPIO, CLOCK, NVIC...etc) by calling the customized HAL_I3C_MspInit(&hi3c) API.

    (#) Configure the different FIFO parameters in I3C_FifoConfTypeDef structure as RxFifoThreshold, TxFifoThreshold
        for Controller or Target mode.
        And enable/disable the Control or Status FIFO only for Controller Mode.
        Use HAL_I3C_SetConfigFifo() function to finalize the configuration, and HAL_I3C_GetConfigFifo() to retrieve
        FIFO configuration.
        Possibility to clear the FIFO configuration by using HAL_I3C_ClearConfigFifo() which reset the configuration
        FIFO to their default hardware value

    (#) Configure the different additional Controller configuration in I3C_CtrlConfTypeDef structure as DynamicAddr,
        StallTime, HotJoinAllowed, ACKStallState, CCCStallState, TxStallState, RxStallState, HighKeeperSDA.
        Use HAL_I3C_Ctrl_Config() function to finalize the Controller configuration.

    (#) Configure the different additional Target configuration in I3C_TgtConfTypeDef structure as Identifier,
        MIPIIdentifier, CtrlRoleRequest, HotJoinRequest, IBIRequest, IBIPayload, IBIPayloadSize, MaxReadDataSize,
        MaxWriteDataSize, CtrlCapability, GroupAddrCapability, DataTurnAroundDuration, MaxReadTurnAround,
        MaxDataSpeed, MaxSpeedLimitation, HandOffActivityState, HandOffDelay, PendingReadMDB.
        Use HAL_I3C_Tgt_Config() function to finalize the Target configuration.

    (#) Before initiate any IO operation, the application must launch an assignment of the different
        Target dynamic address by using HAL_I3C_Ctrl_DynAddrAssign() in polling mode or
        HAL_I3C_Ctrl_DynAddrAssign_IT() in interrupt mode.
        This procedure is named Enter Dynamic Address Assignment (ENTDAA CCC command).
        For the initiation of ENTDAA procedure from the controller, each target connected and powered on the I3C bus
        must repond to this particular Command Common Code by sending its proper Payload (a amount of 48bits which
        contain the target characteristics)
        Each time a target responds to ENTDAA sequence, the application is informed through
        HAL_I3C_TgtReqDynamicAddrCallback() of the reception of the target paylaod.
        And then application must send a associated dynamic address through HAL_I3C_Ctrl_SetDynAddr().
        This procedure in loop automatically in hardware side until a target respond to repeated ENTDAA sequence.
        The application is informed of the end of the procedure at reception of HAL_I3C_CtrlDAACpltCallback().
        Then application can easily retrieve ENTDAA payload information through HAL_I3C_Get_ENTDAA_Payload_Info()
        function.
        At the end of procedure, the function HAL_I3C_Ctrl_ConfigBusDevices() must be called to store in hardware
        register part the target capabilities as Dynamic address, IBI support with or without additional data byte,
        Controller role request support, Controller stop transfer after IBI through I3C_DeviceConfTypeDef structure.

    (#) Other action to be done, before initiate any IO operation, the application must prepare the different frame
        descriptor with its associated buffer allocation in their side.
        Configure the different information related to CCC transfer through I3C_CCCTypeDef structure
        Configure the different information related to Private or I2C transfer through I3C_PrivateTypeDef structure
        Configure the different buffer pointers and associated size needed for the driver communication
        through I3C_XferTypeDef structure
        The I3C_XferTypeDef structure contains different parameters about Control, Status buffer,
        and Transmit and Receive buffer.
        Use HAL_I3C_AddDescToFrame() function each time application add a descriptor in the frame before call
        a Controller IO operation interface
        One element of the frame descriptor correspond to one frame to manage through IO operation.

    (#) To check if I3C target device is ready for communication, use the function HAL_I3C_Ctrl_IsDeviceI3C_Ready()

    (#) To check if I2C target device is ready for communication, use the function HAL_I3C_Ctrl_IsDeviceI2C_Ready()

    (#) To send a message header {S + 0x7E + W + STOP}, use the function HAL_I3C_Ctrl_GenerateArbitration().
    (#) To insert a target reset pattern before the STOP of a transmitted frame containing a RSTACT CCC command,
        the application must enable the reset pattern configuration using HAL_I3C_Ctrl_SetConfigResetPattern()
        before calling HAL_I3C_Ctrl_TransmitCCC() or HAL_I3C_Ctrl_ReceiveCCC() interfaces.

        To have a standard STOP emitted at the end of a frame containing a RSTACT CCC command, the application must
        disable the reset pattern configuration using HAL_I3C_Ctrl_SetConfigResetPattern() before calling
        HAL_I3C_Ctrl_TransmitCCC() or HAL_I3C_Ctrl_ReceiveCCC() interfaces.

        Use HAL_I3C_Ctrl_SetConfigResetPattern() function to configure the insertion of the reset pattern at
        the end of a Frame, and HAL_I3C_Ctrl_GetConfigResetPattern() to retrieve reset pattern configuration.

    (#) For I3C IO operations, three operation modes are available within this driver :

    *** Polling mode IO operation ***
    =================================
    [..]
      (+) Activate asynchronous event in controller or target mode a Common Command Code in a broadcast
          or a direct communication in blocking mode using HAL_I3C_Ctrl_TransmitCCC()
      (+) Transmit in controller mode a Common Command Code in a broadcast or a direct communication in blocking mode
          using HAL_I3C_Ctrl_TransmitCCC()
      (+) Receive in controller mode a Common Command Code in a direct communication in blocking mode
          using HAL_I3C_Ctrl_ReceiveCCC()
      (+) Transmit in controller mode an amount of private data in an I3C or an I2C communication in blocking mode
          using HAL_I3C_Ctrl_Transmit()
      (+) Receive in controller mode an amount of private data in an I3C or an I2C communication in blocking mode
          using HAL_I3C_Ctrl_Receive()
      (+) Transmit in target mode an amount of private data in an I3C communication in blocking mode
          using HAL_I3C_Tgt_Transmit()
      (+) Receive in target mode an amount of private data in an I3C communication in blocking mode
          using HAL_I3C_Tgt_Receive()
      (+) At the end of a transfer, the different FIFO can be flushed if necessary by using HAL_I3C_FlushAllFifo() for
          flush all the FIFO, or flush individually y using HAL_I3C_FlushTxFifo(), HAL_I3C_FlushRxFifo(),
          HAL_I3C_FlushControlFifo(), HAL_I3C_FlushStatusFifo().
      (+) Request a HotJoin in target mode in blocking mode using HAL_I3C_Tgt_HotJoinReq()
      (+) Request a In Band Interrupt in target mode in blocking mode using HAL_I3C_Tgt_IBIReq()
      (+) Request a Controller Role in target mode in blocking mode using HAL_I3C_Tgt_ControlRoleReq()


    *** DMA and Interrupt mode IO operation ***
    ===================================
    [..]
      (+) Transmit in controller mode a Common Command Code in a broadcast or a direct communication in non-blocking
          mode using HAL_I3C_Ctrl_TransmitCCC_IT() or HAL_I3C_Ctrl_TransmitCCC_DMA()
      (+) At transmission end of transfer, HAL_I3C_CtrlTxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_CtrlTxCpltCallback()
      (+) Receive in controller mode a Common Command Code in a direct communication in non-blocking
          mode using HAL_I3C_Ctrl_ReceiveCCC_IT() or HAL_I3C_Ctrl_ReceiveCCC_DMA()
      (+) At reception end of transfer, HAL_I3C_CtrlRxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_CtrlRxCpltCallback()
      (+) Transmit in controller mode an amount of private data in an I3C or an I2C communication in non-blocking mode
          using HAL_I3C_Ctrl_Transmit_IT() or HAL_I3C_Ctrl_Transmit_DMA()
      (+) At transmission end of transfer, HAL_I3C_CtrlTxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_CtrlTxCpltCallback()
      (+) Receive in controller mode an amount of private data in an I3C or an I2C communication in non-blocking mode
          using HAL_I3C_Ctrl_Receive_IT() or HAL_I3C_Ctrl_Receive_DMA()
      (+) At reception end of transfer, HAL_I3C_CtrlRxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_CtrlRxCpltCallback()
      (+) Transfer in multiple direction (transmit/receive) in controller mode a Common Command Code in a direct
          communication or an amount of private data in an I2C or I3C communication in non-blocking mode using
          HAL_I3C_Ctrl_MultipleTransfer_IT() or HAL_I3C_Ctrl_MultipleTransfer_DMA()
      (+) At the end of transfer, HAL_I3C_CtrlMultipleXferCpltCallback() is executed and users can
          add their own code by customization of function pointer HAL_I3C_CtrlMultipleXferCpltCallback()
      (+) Transmit in target mode an amount of private data in an I3C communication in non-blocking mode
          using HAL_I3C_Tgt_Transmit_IT() or HAL_I3C_Tgt_Transmit_DMA()
      (+) At transmission end of transfer, HAL_I3C_TgtTxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_TgtTxCpltCallback()
      (+) Receive in target mode an amount of private data in an I3C communication in non-blocking mode
          using HAL_I3C_Tgt_Receive_IT() or HAL_I3C_Tgt_Receive_DMA()
      (+) At reception end of transfer, HAL_I3C_TgtRxCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_TgtRxCpltCallback()
      (+) To treat asynchronous event, HAL_I3C_ActivateNotification() or HAL_I3C_DeactivateNotification() function is
          used for enable or disable one or more notification related to specific asynchronous event.
          Each time one or more event detected by hardware the associated HAL_I3C_NotifyCallback() is executed
           and users can add their own code by customization of function pointer HAL_I3C_NotifyCallback().
          Then application can easily retrieve some specific associated event data through HAL_I3C_GetCCCInfo() function
      (+) At the end of a transfer, the different FIFO can be flushed if necessary by using HAL_I3C_FlushAllFifo() for
          flush all the FIFO, or flush individually y using HAL_I3C_FlushTxFifo(), HAL_I3C_FlushRxFifo(),
          HAL_I3C_FlushControlFifo(), HAL_I3C_FlushStatusFifo().
      (+) Request a HotJoin in target mode in non-blocking mode using HAL_I3C_Tgt_HotJoinReq_IT
      (+) At completion, HAL_I3C_TgtHotJoinCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_TgtHotJoinCallback()
      (+) Request an In Band Interrupt in target mode in non-blocking mode using HAL_I3C_Tgt_IBIReq_IT()
      (+) At completion, HAL_I3C_NotifyCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_NotifyCallback()
      (+) Request a Controller Role in target mode in non-blocking mode using HAL_I3C_Tgt_ControlRoleReq_IT()
      (+) At completion, HAL_I3C_NotifyCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_NotifyCallback()
      (+) To manage the wakeup capability, HAL_I3C_ActivateNotification() or HAL_I3C_DeactivateNotification() function
          is used for enable or disable Wake Up interrupt.
          At wakeup detection the associated HAL_I3C_NotifyCallback() is executed.
      (+) In case of transfer Error, HAL_I3C_ErrorCallback() function is executed and users can
           add their own code by customization of function pointer HAL_I3C_ErrorCallback()
      (+) Abort an I3C process communication with Interrupt using HAL_I3C_Abort_IT()
      (+) End of abort process, HAL_I3C_AbortCpltCallback() is executed and users can
           add their own code by customization of function pointer HAL_I3C_AbortCpltCallback()


     *** I3C HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in I3C HAL driver.

      (+) __HAL_I3C_ENABLE: Enable the I3C peripheral
      (+) __HAL_I3C_DISABLE: Disable the I3C peripheral
      (+) __HAL_I3C_RESET_HANDLE_STATE: Reset the I3C handle state
      (+) __HAL_I3C_GET_FLAG: Check whether the specified I3C flag is set or not

     *** Callback registration ***
     =============================================
    [..]
     The compilation flag USE_HAL_I3C_REGISTER_CALLBACKS when set to 1
     allows the user to configure dynamically the driver callbacks.
     Use Functions HAL_I3C_RegisterCallback() or HAL_I3C_RegisterNotifyCallback()
     or HAL_I3C_RegisterTgtReqDynamicAddrCallback() or HAL_I3C_RegisterTgtHotJoinCallback()
     to register an interrupt callback.
    [..]
     Function HAL_I3C_RegisterCallback() allows to register following callbacks:
       (+) CtrlTxCpltCallback   : callback for Controller transmission CCC, I3C private or I2C end of transfer.
       (+) CtrlRxCpltCallback   : callback for Controller reception CCC, I3C private or I2C end of transfer.
       (+) CtrlMultipleXferCpltCallback : callback for Controller multiple Direct CCC, I3C private or I2C
           end of transfer.
       (+) CtrlDAACpltCallback  : callback for Controller Enter Dynamic Address Assignment end of transfer.
       (+) TgtTxCpltCallback    : callback for Target transmission I3C private end of transfer.
       (+) TgtRxCpltCallback    : callback for Target reception I3C private end of transfer.
       (+) ErrorCallback        : callback for error detection.
       (+) AbortCpltCallback    : callback for abort completion process.
       (+) MspInitCallback      : callback for Msp Init.
       (+) MspDeInitCallback    : callback for Msp DeInit.
     This function takes as parameters the HAL peripheral handle, the Callback ID
     and a pointer to the user callback function.
    [..]
     For specific callback NotifyCallback
     use dedicated register callbacks : HAL_I3C_RegisterNotifyCallback().
    [..]
     For specific callback TgtReqDynamicAddrCallback
     use dedicated register callbacks : HAL_I3C_RegisterTgtReqDynamicAddrCallback().
    [..]
     For specific callback TgtHotJoinCallback
     use dedicated register callbacks : HAL_I3C_RegisterTgtHotJoinCallback().
    [..]
     Use function HAL_I3C_UnRegisterCallback to reset a callback to the default
     weak function.
     HAL_I3C_UnRegisterCallback takes as parameters the HAL peripheral handle,
     and the Callback ID.
     This function allows to reset following callbacks:
       (+) CtrlTxCpltCallback   : callback for Controller transmission CCC, I3C private or I2C end of transfer.
       (+) CtrlRxCpltCallback   : callback for Controller reception CCC, I3C private or I2C end of transfer.
       (+) CtrlMultipleXferCpltCallback : callback for Controller multiple Direct CCC, I3C private or I2C
           end of transfer.
       (+) CtrlDAACpltCallback  : callback for Controller Enter Dynamic Address Assignment end of transfer.
       (+) TgtTxCpltCallback    : callback for Target transmission I3C private end of transfer.
       (+) TgtRxCpltCallback    : callback for Target reception I3C private end of transfer.
       (+) ErrorCallback        : callback for error detection.
       (+) AbortCpltCallback    : callback for abort completion process.
       (+) MspInitCallback      : callback for Msp Init.
       (+) MspDeInitCallback    : callback for Msp DeInit.
       (+) NotifyCallback       : callback for Controller and Target notification process.
       (+) TgtReqDynamicAddrCallback  : callback for controller application
            when a target sent its payload to the controller during Dynamic Address Assignment process.
       (+) TgtHotJoinCallback   : callback for Target Hotjoin completion process.
    [..]
     By default, after the HAL_I3C_Init() and when the state is HAL_I3C_STATE_RESET
     all callbacks are set to the corresponding weak functions:
     examples HAL_I3C_CtrlTxCpltCallback(), HAL_I3C_CtrlRxCpltCallback().
     Exception done for MspInit and MspDeInit functions that are
     reset to the legacy weak functions in the HAL_I3C_Init()/ HAL_I3C_DeInit() only when
     these callbacks are null (not registered beforehand).
     If MspInit or MspDeInit are not null, the HAL_I3C_Init()/ HAL_I3C_DeInit()
     keep and use the user MspInit/MspDeInit callbacks (registered beforehand) whatever the state.
    [..]
     Callbacks can be registered/unregistered in HAL_I3C_STATE_READY state only.
     Exception done MspInit/MspDeInit functions that can be registered/unregistered
     in HAL_I3C_STATE_READY or HAL_I3C_STATE_RESET state,
     thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
     Then, the user first registers the MspInit/MspDeInit user callbacks
     using HAL_I3C_RegisterCallback() before calling HAL_I3C_DeInit()
     or HAL_I3C_Init() function.
    [..]
     When the compilation flag USE_HAL_I3C_REGISTER_CALLBACKS is set to 0 or
     not defined, the callback registration feature is not available and all callbacks
     are set to the corresponding weak functions.

     [..]
       (@) You can refer to the I3C HAL driver header file for more useful macros

  @endverbatim
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup I3C I3C
  * @brief I3C HAL module driver
  * @{
  */

#ifdef HAL_I3C_MODULE_ENABLED

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Private_Define I3C Private Define
  * @{
  */

/* Private define to centralize the enable/disable of Interrupts */
#define I3C_XFER_LISTEN_IT            (0x00000001U)
#define I3C_XFER_TARGET_TX_IT         (0x00000002U)
#define I3C_XFER_TARGET_RX_IT         (0x00000004U)
#define I3C_XFER_DMA                  (0x00000008U)
#define I3C_XFER_TARGET_CTRLROLE      (0x00000010U)
#define I3C_XFER_TARGET_HOTJOIN       (0x00000020U)
#define I3C_XFER_TARGET_IBI           (0x00000040U)
#define I3C_XFER_CONTROLLER_TX_IT     (0x00000080U)
#define I3C_XFER_CONTROLLER_RX_IT     (0x00000100U)
#define I3C_XFER_CONTROLLER_RX_CCC_IT (0x00000400U)
#define I3C_XFER_CONTROLLER_DAA_IT    (0x00001000U)

/* Private defines for control buffer prior preparation */
#define I3C_OPERATION_TYPE_MASK       (0x78000000U)
#define I3C_RESTART_STOP_MASK         (0x80000000U)
#define I3C_ARBITRATION_HEADER_MASK   (0x00000004U)
#define I3C_DEFINE_BYTE_MASK          (0x00000001U)

/* Private define for CCC command */
#define I3C_BROADCAST_RSTDAA          (0x00000006U)
#define I3C_BROADCAST_ENTDAA          (0x00000007U)

/* Private define to split ENTDAA payload */
#define I3C_DCR_IN_PAYLOAD_SHIFT       56
#define I3C_PID_IN_PAYLOAD_MASK        0xFFFFFFFFFFFFU

/* Private define to split PID */
/* Bits[47:33]: MIPI Manufacturer ID */
#define I3C_MIPIMID_PID_SHIFT          33
#define I3C_MIPIMID_PID_MASK           0x7FFFU

/* Bit[32]: Provisioned ID Type Selector */
#define I3C_IDTSEL_PID_SHIFT           32
#define I3C_IDTSEL_PID_MASK            0x01U

/* Bits[31:16]: Part ID */
#define I3C_PART_ID_PID_SHIFT          16
#define I3C_PART_ID_PID_MASK           0xFFFFU

/* Bits[15:12]: MIPI Instance ID */
#define I3C_MIPIID_PID_SHIFT           12
#define I3C_MIPIID_PID_MASK            0xFU
/**
  * @}
  */

/* Private macro -----------------------------------------------------------------------------------------------------*/

/** @brief  Get Provisioned ID in payload (64bits) receive during ENTDAA procedure.
  * @param  __PAYLOAD__ specifies the Device Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFFFFFF.
  * @retval The value of PID Return value between Min_Data=0x00 and Max_Data=0xFFFFFFFFFFFF.
  */
#define I3C_GET_PID(__PAYLOAD__) ((uint64_t)(__PAYLOAD__) & I3C_PID_IN_PAYLOAD_MASK)

/** @brief  Get MIPI Manufacturer ID in PID (48bits).
  * @param  __PID__ specifies the Provisioned ID retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFF.
  * @retval The value of MIPI ID Return value between Min_Data=0x00 and Max_Data=0x7FFF.
  */
#define I3C_GET_MIPIMID(__PID__) ((uint16_t)((uint64_t)(__PID__) >> I3C_MIPIMID_PID_SHIFT) & \
                                  I3C_MIPIMID_PID_MASK)

/** @brief  Get Type Selector in PID (48bits).
  * @param  __PID__ specifies the Provisioned ID retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFF.
  * @retval The value of Type Selector Return 0 or 1.
  */
#define I3C_GET_IDTSEL(__PID__) ((uint8_t)((uint64_t)(__PID__) >> I3C_IDTSEL_PID_SHIFT) & \
                                 I3C_IDTSEL_PID_MASK)

/** @brief  Get Part ID in PID (48bits).
  * @param  __PID__ specifies the Provisioned ID retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFF.
  * @retval The value of Part ID Return value between Min_Data=0x00 and Max_Data=0xFFFF.
  */
#define I3C_GET_PART_ID(__PID__) ((uint16_t)((uint64_t)(__PID__) >> I3C_PART_ID_PID_SHIFT) & \
                                  I3C_PART_ID_PID_MASK)

/** @brief  Get Instance ID in PID (48bits).
  * @param  __PID__ specifies the Provisioned ID retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFF.
  * @retval The value of Instance ID Return value between Min_Data=0x00 and Max_Data=0xF.
  */
#define I3C_GET_MIPIID(__PID__) ((uint8_t)((uint64_t)(__PID__) >> I3C_MIPIID_PID_SHIFT) & \
                                 I3C_MIPIID_PID_MASK)

/** @brief  Get Device Characterics in payload (64bits) receive during ENTDAA procedure.
  * @param  __PAYLOAD__ specifies the Device Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFFFFFFFF.
  * @retval The value of BCR Return value between Min_Data=0x00 and Max_Data=0xFF.
  */
#define I3C_GET_DCR(__PAYLOAD__) (((uint32_t)((uint64_t)(__PAYLOAD__) >> I3C_DCR_IN_PAYLOAD_SHIFT)) & \
                                  I3C_DCR_DCR)

/** @brief  Get Advanced Capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The value of advanced capabilities:
  *           ENABLE: supports optional advanced capabilities.
  *           DISABLE: not supports optional advanced capabilities.
  */
#define I3C_GET_ADVANCED_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR5_Msk) >> \
                                             I3C_BCR_BCR5_Pos) == 1U) ? ENABLE : DISABLE)

/** @brief  Get virtual target support.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The value of offline capable:
  *           ENABLE: is a Virtual Target
  *           DISABLE: is not a Virtual Target
  */
#define I3C_GET_VIRTUAL_TGT(__BCR__) (((((__BCR__) & I3C_BCR_BCR4_Msk) >> \
                                        I3C_BCR_BCR4_Pos) == 1U) ? ENABLE : DISABLE)

/** @brief  Get offline capable.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The value of offline capable
  *           ENABLE: Device will not always respond to I3C Bus commands
  *           DISABLE: Device will always respond to I3C Bus commands
  */
#define I3C_GET_OFFLINE_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR3_Msk) >> \
                                            I3C_BCR_BCR3_Pos) == 1U) ? ENABLE : DISABLE)

/** @brief  Get Max data speed limitation.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval The value of offline capable:
  *           ENABLE: Limitation
  *           DISABLE: No Limitation
  */
#define I3C_GET_MAX_DATA_SPEED_LIMIT(__BCR__) (((((__BCR__) & I3C_BCR_BCR0_Msk) >> \
                                                 I3C_BCR_BCR0_Pos) == 1U) ? ENABLE : DISABLE)

/** @brief  Change uint32_t variable form big endian to little endian.
  * @param  __DATA__ .uint32_t variable in big endian.
  *         This parameter must be a number between Min_Data=0x00(uint32_t) and Max_Data=0xFFFFFFFF.
  * @retval uint32_t variable in little endian.
  */
#define I3C_BIG_TO_LITTLE_ENDIAN(__DATA__) ((uint32_t)((((__DATA__) & 0xff000000U) >> 24) | \
                                                       (((__DATA__) & 0x00ff0000U) >> 8)  | \
                                                       (((__DATA__) & 0x0000ff00U) << 8)  | \
                                                       (((__DATA__) & 0x000000ffU) << 24)))

/* Private variables -------------------------------------------------------------------------------------------------*/
/** @addtogroup  I3C_Private_Variables
  * @{
  */
/* Structure containing address device and message type used for the private function I3C_Ctrl_IsDevice_Ready() */
typedef struct
{
  uint8_t   Address;            /* Dynamic or Static target Address */
  uint32_t  MessageType;        /* Message Type */

} I3C_DeviceTypeDef;
/**
  * @}
  */

/* Private function prototypes ---------------------------------------------------------------------------------------*/
/** @addtogroup I3C_Private_Functions
  * @{
  */
static HAL_StatusTypeDef I3C_Tgt_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Tgt_Tx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Tgt_Rx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
#if defined(HAL_DMA_MODULE_ENABLED)
static HAL_StatusTypeDef I3C_Tgt_Tx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Tgt_Rx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
#endif /* HAL_DMA_MODULE_ENABLED */
static HAL_StatusTypeDef I3C_Tgt_HotJoin_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Tgt_CtrlRole_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Tgt_IBI_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Tx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Rx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Tx_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Rx_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
#if defined(HAL_DMA_MODULE_ENABLED)
static HAL_StatusTypeDef I3C_Ctrl_Tx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Rx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
#endif /* HAL_DMA_MODULE_ENABLED */
static HAL_StatusTypeDef I3C_Ctrl_DAA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);
static HAL_StatusTypeDef I3C_Abort_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks);

static HAL_StatusTypeDef I3C_WaitOnDAAUntilTimeout(I3C_HandleTypeDef *hi3c, uint32_t timeout, uint32_t tickstart);
static HAL_StatusTypeDef I3C_WaitOnFlagUntilTimeout(I3C_HandleTypeDef *hi3c, uint32_t flag, FlagStatus flagstatus,
                                                    uint32_t timeout, uint32_t tickstart);
static void I3C_TransmitByteTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_TransmitWordTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_ReceiveByteTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_ReceiveWordTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_ControlDataTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_ErrorTreatment(I3C_HandleTypeDef *hi3c);
static void I3C_GetErrorSources(I3C_HandleTypeDef *hi3c);
static void I3C_StateUpdate(I3C_HandleTypeDef *hi3c);
#if defined(HAL_DMA_MODULE_ENABLED)
static void I3C_DMAAbort(DMA_HandleTypeDef *hdma);
static void I3C_DMAControlTransmitCplt(DMA_HandleTypeDef *hdma);
static void I3C_DMADataTransmitCplt(DMA_HandleTypeDef *hdma);
static void I3C_DMADataReceiveCplt(DMA_HandleTypeDef *hdma);
static void I3C_DMAError(DMA_HandleTypeDef *hdma);
#endif /* HAL_DMA_MODULE_ENABLED */
static void I3C_Enable_IRQ(I3C_HandleTypeDef *hi3c, uint32_t InterruptRequest);
static void I3C_Disable_IRQ(I3C_HandleTypeDef *hi3c, uint32_t InterruptRequest);
static HAL_StatusTypeDef I3C_Xfer_PriorPreparation(I3C_HandleTypeDef *hi3c, uint8_t counter, uint32_t option);
static uint32_t I3C_FillTxBuffer_CCC(I3C_HandleTypeDef *hi3c,
                                     uint32_t           indexDesc,
                                     uint32_t           txSize,
                                     uint32_t           txCurrentIndex);
static uint32_t I3C_FillTxBuffer_Private(I3C_HandleTypeDef *hi3c,
                                         uint32_t           indexDesc,
                                         uint32_t           txSize,
                                         uint32_t           txCurrentIndex);
static HAL_StatusTypeDef I3C_ControlBuffer_PriorPreparation(I3C_HandleTypeDef *hi3c,
                                                            uint8_t            counter,
                                                            uint32_t           option);
static HAL_StatusTypeDef I3C_Ctrl_IsDevice_Ready(I3C_HandleTypeDef *hi3c,
                                                 const I3C_DeviceTypeDef *pDevice,
                                                 uint32_t           trials,
                                                 uint32_t           timeout);
static void I3C_TreatErrorCallback(I3C_HandleTypeDef *hi3c);
/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/
/** @addtogroup I3C_Exported_Functions I3C Exported Functions
  * @{
  */

/** @defgroup I3C_Exported_Functions_Group1 Initialization and de-initialization functions.
  * @brief    I3C initialization and de-initialization functions
  *
@verbatim
 =======================================================================================================================
                              ##### Initialization and de-initialization functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to initialize and deinitialize the I3Cx peripheral:

         (+) Users must implement HAL_I3C_MspInit() function in which they configure all related peripherals
             resources (APB and Kernel CLOCK, GPIO, DMA, IT and NVIC).

         (+) Call the function HAL_I3C_Init() to configure the bus characteristic depends on the device mode
             with the selected configuration below:

             (++) Controller mode, Serial source clock wave form configuration:
                  (+++) SCL push pull low duration
                  (+++) SCL I3C high duration
                  (+++) SCL open drain low duration
                  (+++) SCL I2C high duration

             (++) Controller mode, Bus timing configuration:
                  (+++) SDA hold time
                  (+++) Wait time
                  (+++) Bus free duration
                  (+++) Bus available duration

             (++) Target mode, Bus timing configuration:
                  (+++) Bus available duration

         (+) Call the function HAL_I3C_DeInit() to restore the default configuration of the selected I3Cx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the I3C instance by activating the low-level hardware and configuring the bus
  *         characteristic according to the specified parameters in the I3C_InitTypeDef.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Init(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t waveform_value;
  uint32_t timing_value;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Init the I3C Callback settings */
      /* Legacy weak CtrlTxCpltCallback            */
      hi3c->CtrlTxCpltCallback            = HAL_I3C_CtrlTxCpltCallback;
      /* Legacy weak CtrlRxCpltCallback            */
      hi3c->CtrlRxCpltCallback            = HAL_I3C_CtrlRxCpltCallback;
      /* Legacy weak CtrlMultipleXferCpltCallback  */
      hi3c->CtrlMultipleXferCpltCallback  = HAL_I3C_CtrlMultipleXferCpltCallback;
      /* Legacy weak CtrlDAACpltCallback           */
      hi3c->CtrlDAACpltCallback           = HAL_I3C_CtrlDAACpltCallback;
      /* Legacy weak TgtReqDynamicAddrCallback     */
      hi3c->TgtReqDynamicAddrCallback     = HAL_I3C_TgtReqDynamicAddrCallback;
      /* Legacy weak TgtTxCpltCallback             */
      hi3c->TgtTxCpltCallback             = HAL_I3C_TgtTxCpltCallback;
      /* Legacy weak TgtRxCpltCallback             */
      hi3c->TgtRxCpltCallback             = HAL_I3C_TgtRxCpltCallback;
      /* Legacy weak TgtHotJoinCallback            */
      hi3c->TgtHotJoinCallback            = HAL_I3C_TgtHotJoinCallback;
      /* Legacy weak NotifyCallback                */
      hi3c->NotifyCallback                = HAL_I3C_NotifyCallback;
      /* Legacy weak ErrorCallback                 */
      hi3c->ErrorCallback                 = HAL_I3C_ErrorCallback;
      /* Legacy weak AbortCpltCallback             */
      hi3c->AbortCpltCallback             = HAL_I3C_AbortCpltCallback;

      /* Check on the MSP init callback */
      if (hi3c->MspInitCallback == NULL)
      {
        /* Legacy weak MspInit  */
        hi3c->MspInitCallback = HAL_I3C_MspInit;
      }

      /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
      hi3c->MspInitCallback(hi3c);
#else
      /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
      HAL_I3C_MspInit(hi3c);

#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */
    }

    /* Update the I3C state to busy */
    hi3c->State = HAL_I3C_STATE_BUSY;

    /* Disable the selected I3C peripheral */
    LL_I3C_Disable(hi3c->Instance);

    /* Check on the I3C mode: initialization depends on the mode */
    if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
    {
      /* Check the parameters */
      assert_param(IS_I3C_SDAHOLDTIME_VALUE(hi3c->Init.CtrlBusCharacteristic.SDAHoldTime));
      assert_param(IS_I3C_WAITTIME_VALUE(hi3c->Init.CtrlBusCharacteristic.WaitTime));

      /* Set Controller mode */
      LL_I3C_SetMode(hi3c->Instance, LL_I3C_MODE_CONTROLLER);

      /*----------------- SCL signal waveform configuration : I3C timing register 0 (I3C_TIMINGR0) ------------------ */
      /* Set the controller SCL waveform */
      waveform_value = ((uint32_t)hi3c->Init.CtrlBusCharacteristic.SCLPPLowDuration                                   |
                        ((uint32_t)hi3c->Init.CtrlBusCharacteristic.SCLI3CHighDuration << I3C_TIMINGR0_SCLH_I3C_Pos)  |
                        ((uint32_t)hi3c->Init.CtrlBusCharacteristic.SCLODLowDuration << I3C_TIMINGR0_SCLL_OD_Pos)     |
                        ((uint32_t)hi3c->Init.CtrlBusCharacteristic.SCLI2CHighDuration << I3C_TIMINGR0_SCLH_I2C_Pos));

      LL_I3C_ConfigClockWaveForm(hi3c->Instance, waveform_value);

      /*------------------ Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ------------------------------ */
      /* Set SDA hold time, activity state, bus free duration and bus available duration */
      timing_value = ((uint32_t)hi3c->Init.CtrlBusCharacteristic.SDAHoldTime                                 |
                      (uint32_t)hi3c->Init.CtrlBusCharacteristic.WaitTime                                    |
                      ((uint32_t)hi3c->Init.CtrlBusCharacteristic.BusFreeDuration <<  I3C_TIMINGR1_FREE_Pos) |
                      (uint32_t)hi3c->Init.CtrlBusCharacteristic.BusIdleDuration);

      LL_I3C_SetCtrlBusCharacteristic(hi3c->Instance, timing_value);
    }
    else
    {
      /* Set target mode */
      LL_I3C_SetMode(hi3c->Instance, LL_I3C_MODE_TARGET);

      /*------------------ Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ------------------------------ */
      /* Set the number of kernel clocks cycles for the bus available condition time */
      LL_I3C_SetAvalTiming(hi3c->Instance, hi3c->Init.TgtBusCharacteristic.BusAvailableDuration);
    }

    /* Enable the selected I3C peripheral */
    LL_I3C_Enable(hi3c->Instance);

    hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

    /* Update I3C state */
    hi3c->State = HAL_I3C_STATE_READY;
    hi3c->PreviousState = HAL_I3C_STATE_READY;
  }

  return status;
}

/**
  * @brief  DeInitialize the I3C peripheral.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_DeInit(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));

    /* Update the I3C state to busy */
    hi3c->State = HAL_I3C_STATE_BUSY;

    /* Disable the selected I3C peripheral */
    LL_I3C_Disable(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Check on the MSP init callback */
    if (hi3c->MspDeInitCallback == NULL)
    {
      /* Legacy weak MspDeInit  */
      hi3c->MspDeInitCallback = HAL_I3C_MspDeInit;
    }

    /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
    hi3c->MspDeInitCallback(hi3c);
#else
    /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
    HAL_I3C_MspDeInit(hi3c);

#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */

    /* Update the I3C Error code, state and mode */
    hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
    hi3c->State         = HAL_I3C_STATE_RESET;
    hi3c->PreviousState = HAL_I3C_STATE_RESET;
    hi3c->Mode          = HAL_I3C_MODE_NONE;
  }

  return status;
}

/**
  * @brief Initialize the I3C MSP.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_MspInit(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I3C_MspInit could be implemented in the user file */
}

/**
  * @brief DeInitialize the I3C MSP.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_MspDeInit(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I3C_MspDeInit could be implemented in the user file */
}
/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group2 Interrupt and callback functions.
  * @brief    I3C interrupt and callback functions.
  *
@verbatim
 =======================================================================================================================
                                    ##### Interrupt and callback functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to manage callbacks and interrupts request:

         (+) Register/Unregister callback function:
             (++) Call the function HAL_I3C_RegisterCallback() to register an I3C user callback.
             (++) Call the function HAL_I3C_RegisterNotifyCallback() to register an I3C user notification callback.
             (++) Call the function HAL_I3C_RegisterDynamicAddrCallback() to register an I3C user address callback.
             (++) Call the function HAL_I3C_RegisterHotJoinCallback() to register an I3C user hot join callback.
             (++) Call the function HAL_I3C_UnRegisterCallback() to unregister an I3C user callback.

         (+) Notification management function:
             (++) Call the function HAL_I3C_ActivateNotification() to activate the I3C notifications.
             (++) Call the function HAL_I3C_DeactivateNotification() to deactivate the I3C notifications.

         (+) Controller callback functions:
             (++) Users must implement HAL_I3C_CtrlTxCpltCallback() function when the transmission of private data or
                  Tx CCC transfer is completed.
             (++) Users must implement HAL_I3C_CtrlRxCpltCallback() function when the reception of private data or
                  Rx CCC transfer is completed.
             (++) Users must implement HAL_I3C_CtrlMultipleXferCpltCallback() function when the multiple
                  transfer of CCC, I3C private or I2C transfer is completed.
             (++) Users must implement HAL_I3C_CtrlDAACpltCallback() function when Dynamic Address Assignment
                  procedure is completed.
             (++) Users must implement HAL_I3C_TgtReqDynamicAddrCallback() function in the controller application
                  when a target sent its payload to the controller during Dynamic Address Assignment procedure.

         (+) Target callback functions:
             (++) Users must implement HAL_I3C_TgtTxCpltCallback() function when the transmission of private
                  data is completed.
             (++) Users must implement HAL_I3C_TgtRxCpltCallback() function when the reception of private
                  data is completed.
             (++) Users must implement HAL_I3C_TgtHotJoinCallback() function when a target hot join process
                  is completed.

         (+) Common callback functions:
             (++) Users must implement HAL_I3C_NotifyCallback() function when the device receives
                  an asynchronous event like IBI, a Hot-join, CCC command for target...
             (++) Users must implement HAL_I3C_AbortCpltCallback() function when an abort process is completed.
             (++) Users must implement HAL_I3C_ErrorCallback() function when an error is occurred.

         (+) Interrupt and event function:
             (++) Call the function HAL_I3C_ER_IRQHandler() in the ISR file to handle I3C error interrupts request.
             (++) Call the function HAL_I3C_EV_IRQHandler() in the ISR file to handle I3C event interrupts request.
@endverbatim
  * @{
  */

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
/**
  * @brief  Register a User I3C Callback to be used instead of the weak predefined callback.
  * @note   The HAL_I3C_RegisterCallback() may be called before HAL_I3C_Init() in HAL_I3C_STATE_RESET
  *         to register callbacks for HAL_I3C_MSPINIT_CB_ID and HAL_I3C_MSPDEINIT_CB_ID
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  callbackID : [IN]  ID of the callback to be registered.
  *                            This parameter can be one of the following values:
  *                            @arg @ref HAL_I3C_CTRL_TX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_RX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_MULTIPLE_XFER_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_DAA_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_TGT_TX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_TGT_RX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_ERROR_CB_ID
  *                            @arg @ref HAL_I3C_ABORT_CB_ID
  *                            @arg @ref HAL_I3C_MSPINIT_CB_ID
  *                            @arg @ref HAL_I3C_MSPDEINIT_CB_ID
  * @param  pCallback  : [IN]  Pointer to the Callback function.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_RegisterCallback(I3C_HandleTypeDef *hi3c,
                                           HAL_I3C_CallbackIDTypeDef callbackID,
                                           pI3C_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the user callback allocation */
    if (pCallback == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else if (HAL_I3C_STATE_READY == hi3c->State)
    {
      switch (callbackID)
      {
        case HAL_I3C_CTRL_TX_COMPLETE_CB_ID :
          hi3c->CtrlTxCpltCallback = pCallback;
          break;

        case HAL_I3C_CTRL_RX_COMPLETE_CB_ID :
          hi3c->CtrlRxCpltCallback = pCallback;
          break;

        case HAL_I3C_CTRL_MULTIPLE_XFER_COMPLETE_CB_ID :
          hi3c->CtrlMultipleXferCpltCallback = pCallback;
          break;

        case HAL_I3C_CTRL_DAA_COMPLETE_CB_ID :
          hi3c->CtrlDAACpltCallback = pCallback;
          break;

        case HAL_I3C_TGT_TX_COMPLETE_CB_ID :
          hi3c->TgtTxCpltCallback = pCallback;
          break;

        case HAL_I3C_TGT_RX_COMPLETE_CB_ID :
          hi3c->TgtRxCpltCallback = pCallback;
          break;

        case HAL_I3C_ERROR_CB_ID :
          hi3c->ErrorCallback = pCallback;
          break;

        case HAL_I3C_ABORT_CB_ID :
          hi3c->AbortCpltCallback = pCallback;
          break;

        case HAL_I3C_MSPINIT_CB_ID :
          hi3c->MspInitCallback = pCallback;
          break;

        case HAL_I3C_MSPDEINIT_CB_ID :
          hi3c->MspDeInitCallback = pCallback;
          break;

        default :
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_CALLBACK;
          status =  HAL_ERROR;
          break;
      }
    }
    else if (HAL_I3C_STATE_RESET == hi3c->State)
    {
      switch (callbackID)
      {
        case HAL_I3C_MSPINIT_CB_ID :
          hi3c->MspInitCallback = pCallback;
          break;

        case HAL_I3C_MSPDEINIT_CB_ID :
          hi3c->MspDeInitCallback = pCallback;
          break;

        default :
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_CALLBACK;
          status =  HAL_ERROR;
          break;
      }
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status =  HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Register a User I3C Notify Callback to be used instead of the weak predefined callback.
  * @param  hi3c            : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                 information for the specified I3C.
  * @param  pNotifyCallback : [IN]  Pointer to the Callback function.
  * @retval HAL Status      :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_RegisterNotifyCallback(I3C_HandleTypeDef *hi3c, pI3C_NotifyCallbackTypeDef pNotifyCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the user callback allocation */
    if (pNotifyCallback == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else if (HAL_I3C_STATE_READY == hi3c->State)
    {
      hi3c->NotifyCallback = pNotifyCallback;
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Register a User I3C dynamic address Callback to be used instead of the weak predefined callback.
  * @param  hi3c                : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                     information for the specified I3C.
  * @param  pTgtReqAddrCallback : [IN]  Pointer to the Callback function.
  * @retval HAL Status          :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_RegisterTgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c,
                                                            pI3C_TgtReqDynamicAddrCallbackTypeDef pTgtReqAddrCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the user callback allocation */
    if (pTgtReqAddrCallback == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else if (HAL_I3C_STATE_READY == hi3c->State)
    {
      hi3c->TgtReqDynamicAddrCallback = pTgtReqAddrCallback;
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Register a User I3C hot join Callback to be used instead of the weak predefined callback.
  * @param  hi3c                : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                     information for the specified I3C.
  * @param  pTgtHotJoinCallback : [IN]  Pointer to the Callback function.
  * @retval HAL Status          :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_RegisterTgtHotJoinCallback(I3C_HandleTypeDef *hi3c,
                                                     pI3C_TgtHotJoinCallbackTypeDef pTgtHotJoinCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the user callback allocation */
    if (pTgtHotJoinCallback == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else if (HAL_I3C_STATE_READY == hi3c->State)
    {
      hi3c->TgtHotJoinCallback = pTgtHotJoinCallback;
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Unregister a user I3C Callback.
  *         The I3C callback is redirected to the weak predefined callback
  * @note   The HAL_I3C_UnRegisterCallback() may be called before HAL_I3C_Init() in HAL_I3C_STATE_RESET
  *         to un-register callbacks for HAL_I3C_MSPINIT_CB_ID and HAL_I3C_MSPDEINIT_CB_ID
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  callbackID : [IN]  ID of the callback to be unregistered.
  *                            This parameter can be one of the following values:
  *                            @arg @ref HAL_I3C_CTRL_TX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_RX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_MULTIPLE_XFER_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_CTRL_DAA_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_TGT_REQ_DYNAMIC_ADDR_CB_ID
  *                            @arg @ref HAL_I3C_TGT_TX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_TGT_RX_COMPLETE_CB_ID
  *                            @arg @ref HAL_I3C_TGT_HOTJOIN_CB_ID
  *                            @arg @ref HAL_I3C_NOTIFY_CB_ID
  *                            @arg @ref HAL_I3C_ERROR_CB_ID
  *                            @arg @ref HAL_I3C_ABORT_CB_ID
  *                            @arg @ref HAL_I3C_MSPINIT_CB_ID
  *                            @arg @ref HAL_I3C_MSPDEINIT_CB_ID
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_UnRegisterCallback(I3C_HandleTypeDef *hi3c, HAL_I3C_CallbackIDTypeDef callbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (HAL_I3C_STATE_READY == hi3c->State)
    {
      switch (callbackID)
      {
        case HAL_I3C_CTRL_TX_COMPLETE_CB_ID :
          /* Legacy weak CtrlTxCpltCallback               */
          hi3c->CtrlTxCpltCallback = HAL_I3C_CtrlTxCpltCallback;
          break;

        case HAL_I3C_CTRL_RX_COMPLETE_CB_ID :
          /* Legacy weak CtrlRxCpltCallback               */
          hi3c->CtrlRxCpltCallback = HAL_I3C_CtrlRxCpltCallback;
          break;

        case HAL_I3C_CTRL_MULTIPLE_XFER_COMPLETE_CB_ID :
          /* Legacy weak CtrlMultipleXferCpltCallback     */
          hi3c->CtrlMultipleXferCpltCallback = HAL_I3C_CtrlMultipleXferCpltCallback;
          break;

        case HAL_I3C_CTRL_DAA_COMPLETE_CB_ID :
          /* Legacy weak CtrlDAACpltCallback              */
          hi3c->CtrlDAACpltCallback = HAL_I3C_CtrlDAACpltCallback;
          break;

        case HAL_I3C_TGT_REQ_DYNAMIC_ADDR_CB_ID :
          /*Legacy weak TgtReqDynamicAddrCallback          */
          hi3c->TgtReqDynamicAddrCallback = HAL_I3C_TgtReqDynamicAddrCallback;
          break;

        case HAL_I3C_TGT_TX_COMPLETE_CB_ID :
          /* Legacy weak TgtTxCpltCallback                 */
          hi3c->TgtTxCpltCallback = HAL_I3C_TgtTxCpltCallback;
          break;

        case HAL_I3C_TGT_RX_COMPLETE_CB_ID :
          /* Legacy weak TgtRxCpltCallback                 */
          hi3c->TgtRxCpltCallback = HAL_I3C_TgtRxCpltCallback;
          break;

        case HAL_I3C_TGT_HOTJOIN_CB_ID :
          /* Legacy weak TgtHotJoinCallback                */
          hi3c->TgtHotJoinCallback = HAL_I3C_TgtHotJoinCallback;
          break;

        case HAL_I3C_NOTIFY_CB_ID :
          /* Legacy weak NotifyCallback                    */
          hi3c->NotifyCallback = HAL_I3C_NotifyCallback;
          break;

        case HAL_I3C_ERROR_CB_ID :
          /* Legacy weak ErrorCallback                     */
          hi3c->ErrorCallback = HAL_I3C_ErrorCallback;
          break;

        case HAL_I3C_ABORT_CB_ID :
          /* Legacy weak AbortCpltCallback                 */
          hi3c->AbortCpltCallback = HAL_I3C_AbortCpltCallback;
          break;

        case HAL_I3C_MSPINIT_CB_ID :
          /* Legacy weak MspInit                           */
          hi3c->MspInitCallback = HAL_I3C_MspInit;
          break;

        case HAL_I3C_MSPDEINIT_CB_ID :
          /* Legacy weak MspDeInit                         */
          hi3c->MspDeInitCallback = HAL_I3C_MspDeInit;
          break;

        default :
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_CALLBACK;
          status =  HAL_ERROR;
          break;
      }
    }
    else if (HAL_I3C_STATE_RESET == hi3c->State)
    {
      switch (callbackID)
      {
        case HAL_I3C_MSPINIT_CB_ID :
          /* Legacy weak MspInit                           */
          hi3c->MspInitCallback = HAL_I3C_MspInit;
          break;

        case HAL_I3C_MSPDEINIT_CB_ID :
          /* Legacy weak MspDeInit                         */
          hi3c->MspDeInitCallback = HAL_I3C_MspDeInit;
          break;

        default :
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_CALLBACK;
          status =  HAL_ERROR;
          break;
      }
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status =  HAL_ERROR;
    }
  }

  return status;
}
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

/**
  * @brief  This function permits to activate the I3C notifications.
  * @param  hi3c          : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                               information for the specified I3C.
  * @param  pXferData     : [IN/OUT] Pointer to an I3C_XferTypeDef structure that contains the reception buffer to
  *                               retrieve data during broadcast CCC DEFTGTS and DEFGRPA when Target mode only.
  * @param  interruptMask : [IN]  Parameter indicates which interrupts will be enabled.
  *                               This parameter can be any combination of @arg I3C_TARGET_INTERRUPT when
  *                               the I3C is in target mode or a combination of @arg I3C_CONTROLLER_INTERRUPT
  *                               when it is in controller mode.
  * @retval HAL Status    :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_ActivateNotification(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData,
                                               uint32_t interruptMask)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));
    assert_param(IS_I3C_INTERRUPTMASK(hi3c->Mode, interruptMask));

    /* Check the I3C state and mode */
    if ((hi3c->State == HAL_I3C_STATE_RESET) ||
        ((hi3c->Mode != HAL_I3C_MODE_CONTROLLER) && (hi3c->Mode != HAL_I3C_MODE_TARGET)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Check the I3C mode */
    else if ((hi3c->Mode == HAL_I3C_MODE_TARGET) &&
             ((interruptMask & (HAL_I3C_IT_DEFIE | HAL_I3C_IT_GRPIE)) != 0U) &&
             (pXferData == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else
    {
      /* Check the I3C mode */
      if (hi3c->Mode == HAL_I3C_MODE_TARGET)
      {
        if ((interruptMask & (HAL_I3C_IT_DEFIE | HAL_I3C_IT_GRPIE)) != 0U)
        {
          hi3c->pXferData = pXferData;
          hi3c->RxXferCount = hi3c->pXferData->RxBuf.Size;

          /* Check on the Rx threshold to know the Rx treatment process : byte or word */
          if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
          {
            /* Set byte treatment function pointer */
            hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
          }
          else
          {
            /* Set word treatment function pointer */
            hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
          }
        }
        /* Store the target event treatment function */
        hi3c->XferISR = I3C_Tgt_Event_ISR;
      }
      else
      {
        /* Store the controller event treatment function */
        hi3c->XferISR = I3C_Ctrl_Event_ISR;
      }

      /* Update handle parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->State         = HAL_I3C_STATE_LISTEN;
      hi3c->PreviousState = HAL_I3C_STATE_LISTEN;

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */
      /* Enable selected notifications */
      I3C_Enable_IRQ(hi3c, (interruptMask | I3C_XFER_LISTEN_IT));
    }
  }

  return status;
}

/**
  * @brief  This function permits to deactivate the I3C notifications.
  * @param  hi3c          : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                               information for the specified I3C.
  * @param  interruptMask : [IN]  Parameter indicates which interrupts will be disabled.
  *                               This parameter can be any combination of @arg I3C_TARGET_INTERRUPT when
  *                               the I3C is in target mode or a combination of @arg I3C_CONTROLLER_INTERRUPT
  *                               when it is in controller mode.
  * @retval HAL Status    :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_DeactivateNotification(I3C_HandleTypeDef *hi3c, uint32_t interruptMask)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance parameter */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));

    /* Check on the State */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Disable selected notifications */
      I3C_Disable_IRQ(hi3c, (interruptMask | I3C_XFER_LISTEN_IT));

      if (READ_REG(hi3c->Instance->IER) == 0U)
      {
        /* Update the XferISR pointer */
        hi3c->XferISR = NULL;

        /* Update I3C state */
        hi3c->State         = HAL_I3C_STATE_READY;
        hi3c->PreviousState = HAL_I3C_STATE_READY;
      }
    }
  }

  return status;
}

/**
  * @brief  Controller Transmission Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_CtrlTxCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_CtrlTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Controller Reception Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_CtrlRxCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_CtrlRxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Controller multiple Direct CCC Command, I3C private or I2C transfer Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_CtrlMultipleXferCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_CtrlMultipleXferCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Controller dynamic address assignment Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_CtrlDAACpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_CtrlDAACpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Target Request Dynamic Address callback.
  * @param  hi3c          : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                              for the specified I3C.
  * @param  targetPayload : [IN] Parameter indicates the target payload.
  * @retval None
  */
__weak void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c, uint64_t targetPayload)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);
  UNUSED(targetPayload);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_TgtReqDynamicAddrCallback could be implemented in the user file
   */
}

/**
  * @brief  Target Transmission Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_TgtTxCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_TgtTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Target Reception Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_TgtRxCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_TgtRxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Target Hot join process Complete callback.
  * @param  hi3c           : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                               information for the specified I3C.
  * @param  dynamicAddress : [IN] The returned dynamic address value after the hot join process.
  * @retval None
  */
__weak void HAL_I3C_TgtHotJoinCallback(I3C_HandleTypeDef *hi3c, uint8_t dynamicAddress)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);
  UNUSED(dynamicAddress);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_TgtHotJoinCallback could be implemented in the user file
   */
}

/**
  * @brief  Target/Controller Notification event callback.
  * @param  hi3c     : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                         for the specified I3C.
  * @param  eventId  : [IN] Parameter indicates which notification is signaled.
  *                         It can be a combination value of @ref HAL_I3C_Notification_ID_definition.
  * @retval None
  */
__weak void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t eventId)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);
  UNUSED(eventId);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_NotifyCallback could be implemented in the user file
   */
}

/**
  * @brief  Abort complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_AbortCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_AbortCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Error callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
__weak void HAL_I3C_ErrorCallback(I3C_HandleTypeDef *hi3c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi3c);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I3C_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  This function handles I3C error interrupt request.
  * @param  hi3c : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                      for the specified I3C.
  * @retval None
  */
void HAL_I3C_ER_IRQHandler(I3C_HandleTypeDef *hi3c)
{
  uint32_t it_flag   = READ_REG(hi3c->Instance->EVR);
  uint32_t it_source = READ_REG(hi3c->Instance->IER);

  /* Check on the error interrupt flag and source */
  if ((I3C_CHECK_FLAG(it_flag, HAL_I3C_FLAG_ERRF) != RESET) &&
      (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_ERRIE) != RESET))
  {
    /* Clear the error flag */
    LL_I3C_ClearFlag_ERR(hi3c->Instance);

    if (hi3c->State != HAL_I3C_STATE_ABORT)
    {
      /* Get error sources */
      I3C_GetErrorSources(hi3c);
    }

    /* Errors treatment */
    I3C_ErrorTreatment(hi3c);
  }
}

/**
  * @brief  This function handles I3C event interrupt request.
  * @param  hi3c : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                      for the specified I3C.
  * @retval None
  */
void HAL_I3C_EV_IRQHandler(I3C_HandleTypeDef *hi3c) /* Derogation MISRAC2012-Rule-8.13 */
{
  uint32_t it_flags   = READ_REG(hi3c->Instance->EVR);
  uint32_t it_sources = READ_REG(hi3c->Instance->IER);

  uint32_t it_masks   = (uint32_t)(it_flags & it_sources);

  /* I3C events treatment */
  if (hi3c->XferISR != NULL)
  {
    hi3c->XferISR(hi3c, it_masks);
  }
}
/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group3 Configuration functions.
  * @brief    I3C configuration functions.
  *
@verbatim
 =======================================================================================================================
                                       ##### Configuration functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to configure the I3C instances.

         (+) Call the function HAL_I3C_Ctrl_BusCharacteristicConfig() to modify the controller Bus Characteristics
             after initialize the bus through HAL_I3C_Init.

         (+) Call the function HAL_I3C_Tgt_BusCharacteristicConfig() to modify the target Bus Characteristics
             after initialize the bus through HAL_I3C_Init.

         (+) Call the function HAL_I3C_SetConfigFifo() to set FIFOs configuration (enabled FIFOs and
             threshold level) with the selected parameters in the configuration structure I3C_FifoConfTypeDef.

         (+) Call the function HAL_I3C_Ctrl_Config() to configure the I3C Controller instances with the selected
             parameters in the configuration structure I3C_CtrlConfTypeDef.
             This function is called only when mode is Controller.

         (+) Call the function HAL_I3C_Tgt_Config() to configure the I3C Target instances with the selected
             parameters in the configuration structure I3C_TgtConfTypeDef.
             This function is called only when mode is Target.

         (+) Call the function HAL_I3C_Ctrl_ConfigBusDevices() to configure Hardware device characteristics register
             with Devices capabilities present on the Bus.
             All different characteristics must be fill through structure I3C_DeviceConfTypeDef.
             This function is called only when mode is Controller.

         (+) Call the function HAL_I3C_AddDescToFrame() to prepare the full transfer usecase in a Controller transfer
             descriptor which contained different buffer pointers and their associated size through I3C_XferTypeDef.
             This function must be called before initiate any communication transfer.
         (+) Call the function HAL_I3C_Ctrl_SetConfigResetPattern() to configure the insertion of the reset pattern
             at the end of a Frame.
         (+) Call the function HAL_I3C_Ctrl_GetConfigResetPattern() to get the current reset pattern configuration

     [..]
       (@) Users must call all above functions after I3C initialization.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the Controller Bus characterics.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pConfig    : [IN]  Pointer to an LL_I3C_CtrlBusConfTypeDef structure contains controller bus configuration.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_BusCharacteristicConfig(I3C_HandleTypeDef *hi3c,
                                                       const LL_I3C_CtrlBusConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t waveform_value;
  uint32_t timing_value;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state and mode */
    else if ((hi3c->State != HAL_I3C_STATE_READY) || (hi3c->Mode != HAL_I3C_MODE_CONTROLLER))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Check the parameters */
      assert_param(IS_I3C_SDAHOLDTIME_VALUE(pConfig->SDAHoldTime));
      assert_param(IS_I3C_WAITTIME_VALUE(pConfig->WaitTime));

      /* Disable the selected I3C peripheral */
      LL_I3C_Disable(hi3c->Instance);

      /*----------------- SCL signal waveform configuration : I3C timing register 0 (I3C_TIMINGR0) ------------------ */
      /* Set the controller SCL waveform */
      waveform_value = ((uint32_t)pConfig->SCLPPLowDuration                                    |
                        ((uint32_t)pConfig->SCLI3CHighDuration << I3C_TIMINGR0_SCLH_I3C_Pos)   |
                        ((uint32_t)pConfig->SCLODLowDuration << I3C_TIMINGR0_SCLL_OD_Pos)      |
                        ((uint32_t)pConfig->SCLI2CHighDuration << I3C_TIMINGR0_SCLH_I2C_Pos));

      LL_I3C_ConfigClockWaveForm(hi3c->Instance, waveform_value);

      /*------------------ Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ------------------------------ */
      /* Set SDA hold time, activity state, bus free duration and bus available duration */
      timing_value = ((uint32_t)pConfig->SDAHoldTime                                 |
                      (uint32_t)pConfig->WaitTime                                    |
                      ((uint32_t)pConfig->BusFreeDuration <<  I3C_TIMINGR1_FREE_Pos) |
                      (uint32_t)pConfig->BusIdleDuration);

      LL_I3C_SetCtrlBusCharacteristic(hi3c->Instance, timing_value);

      /* Enable the selected I3C peripheral */
      LL_I3C_Enable(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Configure the target Bus characterics.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pConfig    : [IN]  Pointer to an LL_I3C_TgtBusConfTypeDef structure contains target bus configuration.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_BusCharacteristicConfig(I3C_HandleTypeDef *hi3c,
                                                      const LL_I3C_TgtBusConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else
    {
      /* Check the instance and the mode parameters */
      assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
      assert_param(IS_I3C_MODE(hi3c->Mode));

      /* Check the I3C state and mode */
      if ((hi3c->State != HAL_I3C_STATE_READY) || (hi3c->Mode != HAL_I3C_MODE_TARGET))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
      else
      {
        /* Disable the selected I3C peripheral */
        LL_I3C_Disable(hi3c->Instance);

        /*------------------ Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ---------------------------- */
        /* Set the number of kernel clocks cycles for the bus available condition time */
        LL_I3C_SetAvalTiming(hi3c->Instance, pConfig->BusAvailableDuration);

        /* Enable the selected I3C peripheral */
        LL_I3C_Enable(hi3c->Instance);
      }
    }
  }

  return status;
}

/**
  * @brief  Set I3C FIFOs configuration.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pConfig    : [IN]  Pointer to an I3C_FifoConfTypeDef structure contains FIFOs configuration.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_SetConfigFifo(I3C_HandleTypeDef *hi3c, const I3C_FifoConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t cfgr_value;
  uint32_t cfgr_mask;

  /* Check the I3C handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state */
    else if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Check the instance and the mode parameters */
      assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
      assert_param(IS_I3C_MODE(hi3c->Mode));

      /* Check configuration parameters */
      assert_param(IS_I3C_TXFIFOTHRESHOLD_VALUE(pConfig->TxFifoThreshold));
      assert_param(IS_I3C_RXFIFOTHRESHOLD_VALUE(pConfig->RxFifoThreshold));

      /* Set Tx and Rx Fifo threshold */
      cfgr_value = (pConfig->TxFifoThreshold | pConfig->RxFifoThreshold);
      cfgr_mask  = (I3C_CFGR_TXTHRES | I3C_CFGR_RXTHRES);

      /* Check on the I3C mode: Control and status FIFOs available only with controller mode */
      if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
      {
        /* Check configuration parameters */
        assert_param(IS_I3C_CONTROLFIFOSTATE_VALUE(pConfig->ControlFifo));
        assert_param(IS_I3C_STATUSFIFOSTATE_VALUE(pConfig->StatusFifo));

        /* Set Control and Status Fifo states */
        cfgr_value |= (pConfig->StatusFifo | pConfig->ControlFifo);
        cfgr_mask  |= (I3C_CFGR_TMODE | I3C_CFGR_SMODE);
      }

      /* Set configuration in the CFGR register */
      MODIFY_REG(hi3c->Instance->CFGR, cfgr_mask, cfgr_value);
    }
  }

  return status;
}

/**
  * @brief  Set I3C controller configuration.
  * @note   This function is called only when the I3C instance is initialized as controller.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pConfig    : [IN]  Pointer to an I3C_CtrlConfTypeDef structure that contains controller configuration.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Config(I3C_HandleTypeDef *hi3c, const I3C_CtrlConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t timing2_value;
  uint32_t cfgr_value;

  /* Check the I3C handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state and mode */
    else if ((hi3c->State == HAL_I3C_STATE_RESET) || (hi3c->Mode != HAL_I3C_MODE_CONTROLLER))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Check configuration parameters values */
      assert_param(IS_I3C_DYNAMICADDRESS_VALUE(pConfig->DynamicAddr));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->HighKeeperSDA));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->HotJoinAllowed));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->ACKStallState));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->CCCStallState));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->TxStallState));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->RxStallState));

      /* Disable the selected I3C peripheral */
      LL_I3C_Disable(hi3c->Instance);

      /* Calculate value to be written in timing register 2 */
      timing2_value = (((uint32_t)pConfig->StallTime << I3C_TIMINGR2_STALL_Pos)      |
                       ((uint32_t)pConfig->ACKStallState << I3C_TIMINGR2_STALLA_Pos) |
                       ((uint32_t)pConfig->CCCStallState << I3C_TIMINGR2_STALLC_Pos) |
                       ((uint32_t)pConfig->TxStallState << I3C_TIMINGR2_STALLD_Pos)  |
                       ((uint32_t)pConfig->RxStallState << I3C_TIMINGR2_STALLT_Pos));

      /* Set value in timing 2 register */
      WRITE_REG(hi3c->Instance->TIMINGR2, timing2_value);

      /* Calculate value to be written in CFGR register */
      cfgr_value = (((uint32_t)pConfig->HighKeeperSDA << I3C_CFGR_HKSDAEN_Pos) |
                    ((uint32_t)pConfig->HotJoinAllowed << I3C_CFGR_HJACK_Pos));

      /* Set hot join acknowledge and high keeper values */
      MODIFY_REG(hi3c->Instance->CFGR, I3C_CFGR_HKSDAEN | I3C_CFGR_HJACK, cfgr_value);

      /* Set dynamic address value */
      LL_I3C_SetOwnDynamicAddress(hi3c->Instance, pConfig->DynamicAddr);

      /* Validate the controller dynamic address */
      LL_I3C_EnableOwnDynAddress(hi3c->Instance);

      /* Enable the selected I3C peripheral */
      LL_I3C_Enable(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Set I3C target configuration.
  * @note   This function is called only when the I3C instance is initialized as target.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pConfig    : [IN]  Pointer to an I3C_TgtConfTypeDef structure that contains target configuration.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Config(I3C_HandleTypeDef *hi3c, const I3C_TgtConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t getmxdsr_value;
  uint32_t maxrlr_value;
  uint32_t crccapr_value;
  uint32_t bcr_value;
  uint32_t devr0_value;

  /* Check the I3C handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state and mode */
    else if ((hi3c->State == HAL_I3C_STATE_RESET) || (hi3c->Mode != HAL_I3C_MODE_TARGET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Check configuration parameters values */
      assert_param(IS_I3C_HANDOFFACTIVITYSTATE_VALUE(pConfig->HandOffActivityState));
      assert_param(IS_I3C_TSCOTIME_VALUE(pConfig->DataTurnAroundDuration));
      assert_param(IS_I3C_MAXSPEEDDATA_VALUE(pConfig->MaxDataSpeed));
      assert_param(IS_I3C_IBIPAYLOADSIZE_VALUE(pConfig->IBIPayloadSize));
      assert_param(IS_I3C_MIPIIDENTIFIER_VALUE(pConfig->MIPIIdentifier));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->HandOffDelay));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->GroupAddrCapability));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->PendingReadMDB));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->IBIPayload));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->MaxSpeedLimitation));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->CtrlCapability));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->IBIRequest));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->CtrlRoleRequest));
      assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pConfig->HotJoinRequest));

      /* Disable the selected I3C peripheral */
      LL_I3C_Disable(hi3c->Instance);

      /* Calculate value to be written in the GETMXDSR register */
      getmxdsr_value = (pConfig->HandOffActivityState | pConfig->MaxDataSpeed | pConfig->DataTurnAroundDuration |
                        ((uint32_t)pConfig->MaxReadTurnAround << I3C_GETMXDSR_RDTURN_Pos));

      /* Set value in GETMXDSR register */
      WRITE_REG(hi3c->Instance->GETMXDSR, getmxdsr_value);

      /* Calculate value to be written in MAXRLR register */
      maxrlr_value = (pConfig->IBIPayloadSize | (pConfig->MaxReadDataSize & I3C_MAXRLR_MRL));

      /* Set payload size and max read data size in MAXRLR register */
      WRITE_REG(hi3c->Instance->MAXRLR, maxrlr_value);

      /* Set max write data size in MAXWLR register */
      LL_I3C_SetMaxWriteLength(hi3c->Instance, pConfig->MaxWriteDataSize);

      /* Set MIPI identifier value in EPIDR register */
      LL_I3C_SetMIPIInstanceID(hi3c->Instance, pConfig->MIPIIdentifier);

      /* Set identifier value in DCR register */
      LL_I3C_SetDeviceCharacteristics(hi3c->Instance, pConfig->Identifier);

      /* Calculate value to be written in CRCCAPR register */
      crccapr_value = (((uint32_t)pConfig->HandOffDelay << I3C_CRCAPR_CAPDHOFF_Pos) |
                       ((uint32_t)pConfig->GroupAddrCapability << I3C_CRCAPR_CAPGRP_Pos));

      /* Set hand off dealy and group address capability in CRCCAPR register */
      WRITE_REG(hi3c->Instance->CRCAPR, crccapr_value);

      /* Set pending read MDB notification value in GETCAPR register */
      LL_I3C_SetPendingReadMDB(hi3c->Instance, ((uint32_t)pConfig->PendingReadMDB << I3C_GETCAPR_CAPPEND_Pos));

      /* Calculate value to be written in BCR register */
      bcr_value = (((uint32_t)pConfig->MaxSpeedLimitation << I3C_BCR_BCR0_Pos) |
                   ((uint32_t)pConfig->IBIPayload << I3C_BCR_BCR2_Pos)         |
                   ((uint32_t)pConfig->CtrlCapability << I3C_BCR_BCR6_Pos));

      /* Set control capability, IBI payload support and max speed limitation in BCR register */
      WRITE_REG(hi3c->Instance->BCR, bcr_value);

      /* Calculate value to be written in CFGR register */
      devr0_value = (((uint32_t)pConfig->IBIRequest << I3C_DEVR0_IBIEN_Pos)     |
                     ((uint32_t)pConfig->CtrlRoleRequest << I3C_DEVR0_CREN_Pos) |
                     ((uint32_t)pConfig->HotJoinRequest << I3C_DEVR0_HJEN_Pos));

      /* Set IBI request, control role request and hot join request in DEVR0 register */
      MODIFY_REG(hi3c->Instance->DEVR0, (I3C_DEVR0_HJEN | I3C_DEVR0_IBIEN | I3C_DEVR0_CREN), devr0_value);

      /* Enable the selected I3C peripheral */
      LL_I3C_Enable(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Set I3C bus devices configuration.
  * @note   This function is called only when the I3C instance is initialized as controller.
  *         This function can be called by the controller application to help the automatic treatment when target have
  *         capability of IBI and/or Control-Role.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pDesc      : [IN]  Pointer to an I3C_DeviceConfTypeDef descriptor that contains the bus devices
  *                            configurations.
  * @param  nbDevice   : [IN]  Value specifies the number of devices to be treated.
  *                            This parameter must be a number between Min_Data=1U and Max_Data=4U.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_ConfigBusDevices(I3C_HandleTypeDef           *hi3c,
                                                const I3C_DeviceConfTypeDef *pDesc,
                                                uint8_t                      nbDevice)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t write_value;

  /* Check the I3C handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check on user parameters */
    if (pDesc == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state and mode */
    else if ((hi3c->State == HAL_I3C_STATE_RESET) || (hi3c->Mode != HAL_I3C_MODE_CONTROLLER))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Check the instance and the mode parameters */
      assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
      assert_param(IS_I3C_MODE(hi3c->Mode));
      assert_param(IS_I3C_DEVICE_VALUE(nbDevice));

      /* Loop on the nbDevice to be treated */
      for (uint32_t index = 0U; index < nbDevice; index++)
      {
        /* Check configuration parameters values */
        assert_param(IS_I3C_DEVICE_VALUE(pDesc[index].DeviceIndex));
        assert_param(IS_I3C_DYNAMICADDRESS_VALUE(pDesc[index].TargetDynamicAddr));
        assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pDesc[index].IBIAck));
        assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pDesc[index].CtrlRoleReqAck));
        assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pDesc[index].CtrlStopTransfer));
        assert_param(IS_I3C_FUNCTIONALSTATE_VALUE(pDesc[index].IBIPayload));

        /* Set value to be written */
        write_value = (((uint32_t)pDesc[index].TargetDynamicAddr << I3C_DEVRX_DA_Pos)     |
                       ((uint32_t)pDesc[index].IBIAck            << I3C_DEVRX_IBIACK_Pos) |
                       ((uint32_t)pDesc[index].CtrlRoleReqAck    << I3C_DEVRX_CRACK_Pos)  |
                       ((uint32_t)pDesc[index].CtrlStopTransfer  << I3C_DEVRX_SUSP_Pos)   |
                       ((uint32_t)pDesc[index].IBIPayload        << I3C_DEVRX_IBIDEN_Pos));

        /* Write configuration in the DEVRx register */
        WRITE_REG(hi3c->Instance->DEVRX[(pDesc[index].DeviceIndex - 1U)], write_value);
      }
    }
  }

  return status;
}

/**
  * @brief  Add Private or CCC descriptor in the user data transfer controller descriptor.
  * @note   This function must be called before initiate initiate any controller communication transfer. This function
  *         help the preparation of the full transfer usecase in a transfer descriptor which contained different buffer
  *         pointers and their associated size through I3C_XferTypeDef.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @param  hi3c          : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                               for the specified I3C.
  * @param  pCCCDesc      : [IN]  Pointer to an I3C_CCCTypeDef structure that contains the CCC descriptor information.
  * @param  pPrivateDesc  : [IN]  Pointer to an I3C_PrivateTypeDef structure that contains the transfer descriptor.
  * @param  pXferData     : [IN/OUT] Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                                  (control buffer, data buffer and status buffer).
  * @param  nbFrame       : [IN]  The number of CCC commands or the number of device to treat.
  * @param  option        : [IN]  Value indicates the transfer option. It can be one value of @ref I3C_OPTION_DEFINITION
  * @retval HAL Status    :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_AddDescToFrame(I3C_HandleTypeDef         *hi3c,
                                         const I3C_CCCTypeDef      *pCCCDesc,
                                         const I3C_PrivateTypeDef  *pPrivateDesc,
                                         I3C_XferTypeDef           *pXferData,
                                         uint8_t                    nbFrame,
                                         uint32_t                   option)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Set handle transfer parameters */
    hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
    hi3c->pCCCDesc      = pCCCDesc;
    hi3c->pPrivateDesc  = pPrivateDesc;
    hi3c->pXferData     = pXferData;
    hi3c->RxXferCount   = 0;
    hi3c->TxXferCount   = 0;

    /* Prepare Direction, and Check on user parameters */
    if (((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_CCC) ||
        ((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_DIRECT))
    {
      /* Check on user parameters */
      if ((pCCCDesc == NULL)  ||
          (pXferData == NULL) ||
          (nbFrame < 1U)      ||
          (((option & (I3C_OPERATION_TYPE_MASK        | I3C_DEFINE_BYTE_MASK)) == \
            (LL_I3C_CONTROLLER_MTYPE_DIRECT | I3C_DEFINE_BYTE_MASK)) && (pCCCDesc->CCCBuf.Size == 0U)))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
        status = HAL_ERROR;
      }
    }
    else
    {
      /* Check on user parameters */
      if ((pPrivateDesc == NULL) || (pXferData == NULL) || (nbFrame <= 0U))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* check on the State */
      if ((handle_state == HAL_I3C_STATE_READY) || (handle_state == HAL_I3C_STATE_LISTEN))
      {
        /* I3C control buffer prior preparation */
        if (I3C_ControlBuffer_PriorPreparation(hi3c, nbFrame, option) != HAL_OK)
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
          status = HAL_ERROR;
        }

        /* I3C Tx Buffer prior preparation, set and check RxCount */
        if (I3C_Xfer_PriorPreparation(hi3c, nbFrame, option) != HAL_OK)
        {
          status = HAL_ERROR;
        }
      }
      else
      {
        status = HAL_BUSY;
      }
    }
  }

  return status;
}

/**
  * @brief Set the configuration of the inserted reset pattern at the end of a Frame.
  * @note  When the transfer descriptor contains multiple frames with RESTART option, the reset pattern at the end of
  *        RSTACT CCC frame is not possible.
  * @param  hi3c         : [IN] Pointer to an I3C_HandleTypeDef structure that contains
  *                             the configuration information for the specified I3C.
  * @param  resetPattern : [IN] Specifies the reset pattern configuration.
  *                             It can be a value of @ref I3C_RESET_PATTERN
  * @retval HAL Status   :      Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_SetConfigResetPattern(I3C_HandleTypeDef *hi3c, uint32_t resetPattern)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));
    assert_param(IS_I3C_RESET_PATTERN(resetPattern));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      hi3c->State = HAL_I3C_STATE_BUSY;

      if (resetPattern == HAL_I3C_RESET_PATTERN_ENABLE)
      {
        LL_I3C_EnableResetPattern(hi3c->Instance);
      }
      else
      {
        LL_I3C_DisableResetPattern(hi3c->Instance);
      }

      /* At the end of process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief Get the configuration of the inserted reset pattern at the end of a Frame.
  * @param  hi3c          : [IN] Pointer to an I3C_HandleTypeDef structure that contains
  *                              the configuration information for the specified I3C.
  * @param  pResetPattern : [OUT] Pointer to the current reset pattern configuration.
  *                               It can be a value of @ref I3C_RESET_PATTERN
  * @retval HAL Status    :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_GetConfigResetPattern(I3C_HandleTypeDef *hi3c, uint32_t *pResetPattern)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  /* Check on user parameters */
  else if (pResetPattern == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check if the reset pattern configuration is enabled */
    if (LL_I3C_IsEnabledResetPattern(hi3c->Instance) == 1U)
    {
      *pResetPattern     = HAL_I3C_RESET_PATTERN_ENABLE;
    }
    else
    {
      *pResetPattern     = HAL_I3C_RESET_PATTERN_DISABLE;
    }
  }

  return status;
}

/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group4 FIFO Management functions.
  * @brief    I3C FIFO management functions.
  *
@verbatim
 =======================================================================================================================
                                      ##### FIFO Management functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to manage I3C FIFOs.

         (+) Call the function HAL_I3C_FlushAllFifo() to flush the content of all used FIFOs (Control, Status,
             Tx and Rx FIFO).
         (+) Call the function HAL_I3C_FlushTxFifo() to flush only the content of Tx FIFO.
         (+) Call the function HAL_I3C_FlushRxFifo() to flush only the content of Rx FIFO.
         (+) Call the function HAL_I3C_FlushControlFifo() to flush only the content of Control FIFO.
             This function is called only when mode is controller.
         (+) Call the function HAL_I3C_FlushStatusFifo() to flush only the content of Status FIFO.
             This function is called only when mode is controller.
         (+) Call the function HAL_I3C_ClearConfigFifo() to clear the FIFOs configuration and set it to default values.
         (+) Call the function HAL_I3C_GetConfigFifo() to get the current FIFOs configuration (enabled FIFOs and
             threshold level).

         (+) Users must not call all above functions before I3C initialization.

         (+) Users should call Flush APIs after an end of process, before starting a transfer or in case of bus
             failure and error detection.

@endverbatim
  * @{
  */

/**
  * @brief  Flush all I3C FIFOs content.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_FlushAllFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t cfgr_value;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Flush the content of Tx and Rx Fifo */
      cfgr_value = (I3C_CFGR_TXFLUSH | I3C_CFGR_RXFLUSH);

      /* Check on the I3C mode: Control and status FIFOs available only with controller mode */
      if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
      {
        /* Flush the content of Control and Status Fifo */
        cfgr_value = (I3C_CFGR_SFLUSH | I3C_CFGR_CFLUSH);
      }

      /* Set configuration in the CFGR register */
      MODIFY_REG(hi3c->Instance->CFGR, cfgr_value, cfgr_value);
    }
  }

  return status;
}

/**
  * @brief  Flush I3C Tx FIFO content.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_FlushTxFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Flush the content of Tx Fifo */
      LL_I3C_RequestTxFIFOFlush(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Flush I3C Rx FIFO content.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_FlushRxFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {

    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Flush the content of Rx Fifo */
      LL_I3C_RequestRxFIFOFlush(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Flush I3C control FIFO content.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_FlushControlFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state and mode */
    if ((hi3c->State == HAL_I3C_STATE_RESET) || (hi3c->Mode != HAL_I3C_MODE_CONTROLLER))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Flush the content of Control Fifo */
      LL_I3C_RequestControlFIFOFlush(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Flush I3C status FIFO content.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_FlushStatusFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state and mode */
    if ((hi3c->State == HAL_I3C_STATE_RESET) || (hi3c->Mode != HAL_I3C_MODE_CONTROLLER))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Flush the content of Control Fifo */
      LL_I3C_RequestStatusFIFOFlush(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Clear I3C FIFOs configuration and set it to default values.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_ClearConfigFifo(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t cfgr_value;
  uint32_t cfgr_mask;

  /* Check the I3C handle allocation */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check the I3C state */
    if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Clear Tx Fifo and Rx Fifo threshold and set it to default value */
      cfgr_value = (LL_I3C_TXFIFO_THRESHOLD_1_4 | LL_I3C_RXFIFO_THRESHOLD_1_4);
      cfgr_mask  = (I3C_CFGR_TXTHRES | I3C_CFGR_RXTHRES);

      /* Check on the I3C mode: Control and status FIFOs available only with controller mode */
      if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
      {
        /* Disable the status and Control Fifo state */
        cfgr_value |= (HAL_I3C_STATUSFIFO_DISABLE | HAL_I3C_CONTROLFIFO_DISABLE);
        cfgr_mask  |= (I3C_CFGR_TMODE | I3C_CFGR_SMODE);
      }

      /* Set configuration in the CFGR register */
      MODIFY_REG(hi3c->Instance->CFGR, cfgr_mask, cfgr_value);
    }
  }

  return status;
}

/**
  * @brief  Get I3C FIFOs current configuration.
  * @param  hi3c       : [IN]     Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                               for the specified I3C.
  * @param  pConfig    : [IN/OUT] Pointer to an I3C_FifoConfTypeDef structure that returns current FIFOs configuration.
  * @retval HAL Status :          Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_GetConfigFifo(I3C_HandleTypeDef *hi3c, I3C_FifoConfTypeDef *pConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the I3C handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Check on user parameters */
    if (pConfig == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state */
    else if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Get Tx Fifo threshold */
      pConfig->TxFifoThreshold = LL_I3C_GetTxFIFOThreshold(hi3c->Instance);

      /* Get Rx Fifo threshold */
      pConfig->RxFifoThreshold = LL_I3C_GetRxFIFOThreshold(hi3c->Instance);

      /* Get the Control Fifo state */
      pConfig->ControlFifo = (uint32_t)(LL_I3C_IsEnabledControlFIFO(hi3c->Instance) << I3C_CFGR_TMODE_Pos);

      /* Get the status Fifo state */
      pConfig->StatusFifo = (uint32_t)(LL_I3C_IsEnabledStatusFIFO(hi3c->Instance) << I3C_CFGR_SMODE_Pos);
    }
  }

  return status;
}
/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group5 Controller operational functions.
  * @brief    I3C controller operational functions.
  *
@verbatim
 =======================================================================================================================
                                  ##### Controller operational functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to manage controller I3C operation.

         (+) Call the function HAL_I3C_Ctrl_TransmitCCC() to transmit direct write or a broadcast
             CCC command in polling mode.
         (+) Call the function HAL_I3C_Ctrl_TransmitCCC_IT() to transmit direct write or a broadcast
             CCC command in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_TransmitCCC_DMA() to transmit direct write or a broadcast
             CCC command in DMA mode.
         (+) Call the function HAL_I3C_Ctrl_ReceiveCCC() to transmit direct read CCC command in polling mode.
         (+) Call the function HAL_I3C_Ctrl_ReceiveCCC_IT() to transmit direct read CCC command in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_ReceiveCCC_DMA() to transmit direct read CCC command in DMA mode.
         (+) Call the function HAL_I3C_Ctrl_Transmit() to transmit private data in polling mode.
         (+) Call the function HAL_I3C_Ctrl_Transmit_IT() to transmit private data in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_Transmit_DMA() to transmit private data in DMA mode.
         (+) Call the function HAL_I3C_Ctrl_Receive() to receive private data in polling mode.
         (+) Call the function HAL_I3C_Ctrl_Receive_IT() to receive private data in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_Receive_DMA() to receive private data in DMA mode.
         (+) Call the function HAL_I3C_Ctrl_MultipleTransfer_IT() to transfer I3C or I2C private data or CCC command
             in multiple direction in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_MultipleTransfer_DMA() to transfer I3C or I2C private data or CCC command
             in multiple direction in DMA mode.
         (+) Call the function HAL_I3C_Ctrl_DynAddrAssign() to send a broadcast ENTDAA CCC
             command in polling mode.
         (+) Call the function HAL_I3C_Ctrl_DynAddrAssign_IT() to send a broadcast ENTDAA CCC
             command in interrupt mode.
         (+) Call the function HAL_I3C_Ctrl_SetDynAddr() to set, asscociate the target dynamic address
             during the Dynamic Address Assignment processus.
         (+) Call the function HAL_I3C_Ctrl_IsDeviceI3C_Ready() to check if I3C target device is ready.
         (+) Call the function HAL_I3C_Ctrl_IsDeviceI2C_Ready() to check if I2C target device is ready.
         (+) Call the function HAL_I3C_Ctrl_GenerateArbitration to send arbitration
            (message header {S + 0x7E + W + STOP}) in polling mode

         (+) Those functions are called only when mode is Controller.

@endverbatim
  * @{
  */

/**
  * @brief  Controller transmit direct write or a broadcast CCC command in polling mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC(I3C_HandleTypeDef *hi3c,
                                           I3C_XferTypeDef   *pXferData,
                                           uint32_t           timeout)
{
  uint32_t tickstart;
  uint32_t exit_condition;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      /* Update returned status value */
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData = pXferData;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Check on control FIFO enable/disable state */
      if (LL_I3C_IsEnabledControlFIFO(hi3c->Instance) == 1U)
      {
        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Decrement remaining control buffer data counter */
        hi3c->ControlXferCount--;

        /* Initiate a start condition by writing in the CR register */
        WRITE_REG(hi3c->Instance->CR, *hi3c->pXferData->CtrlBuf.pBuffer);

        /* Increment Buffer pointer */
        hi3c->pXferData->CtrlBuf.pBuffer++;
      }

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        /* Check if hardware asks for control data */
        if (hi3c->ControlXferCount > 0U)
        {
          /* Call control data treatment function */
          I3C_ControlDataTreatment(hi3c);
        }

        /* Check if hardware asks for Tx data */
        if (hi3c->TxXferCount > 0U)
        {
          /* Call transmit treatment function */
          hi3c->ptrTxFunc(hi3c);
        }

        /* Check for the timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;
            break;
          }
        }

        if ((__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET) && (hi3c->ControlXferCount > 0U))
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);

          /* Then Initiate a Start condition */
          LL_I3C_RequestTransfer(hi3c->Instance);
        }

        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      } while ((exit_condition == 0U) ||
               ((exit_condition == I3C_EVR_FCF) && (hi3c->ControlXferCount > 0U)));

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update returned status value */
        status = HAL_ERROR;
      }

      /* At the end of Tx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Controller transmit direct write or a broadcast CCC command in interrupt mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC_IT(I3C_HandleTypeDef *hi3c,
                                              I3C_XferTypeDef   *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

      if (handle_state == HAL_I3C_STATE_LISTEN)
      {
        hi3c->XferISR = I3C_Ctrl_Tx_Listen_Event_ISR;
      }
      else
      {
        hi3c->XferISR = I3C_Ctrl_Tx_ISR;
      }

      hi3c->pXferData = pXferData;
      hi3c->State     = HAL_I3C_STATE_BUSY_TX;


      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Tx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_TX_IT);

      /* Initiate a Start condition */
      LL_I3C_RequestTransfer(hi3c->Instance);
    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Controller transmit direct write or a broadcast CCC command in DMA mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_TransmitCCC_DMA(I3C_HandleTypeDef *hi3c,
                                               I3C_XferTypeDef   *pXferData)
{
  HAL_StatusTypeDef control_dma_status;
  HAL_StatusTypeDef tx_dma_status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmatx and hdmacr handle */
    else if ((hi3c->hdmatx == NULL) || (hi3c->hdmacr == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->XferISR       = I3C_Ctrl_Tx_DMA_ISR;
      hi3c->pXferData     = pXferData;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX;

      /*------------------------------------ I3C DMA channel for Control Data ----------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmacr->XferCpltCallback = I3C_DMAControlTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmacr->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmacr->XferHalfCpltCallback = NULL;
      hi3c->hdmacr->XferAbortCallback    = NULL;

      /* assert that DMA source and destination width are configured in word */
      assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmacr->Init.SrcDataWidth));
      assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmacr->Init.DestDataWidth));

      /* Enable the control data DMA channel */
      control_dma_status = HAL_DMA_Start_IT(hi3c->hdmacr, (uint32_t)hi3c->pXferData->CtrlBuf.pBuffer,
                                            (uint32_t)&hi3c->Instance->CR, (hi3c->ControlXferCount * 4U));

      /*------------------------------------ I3C DMA channel for the Tx Data -----------------------------------------*/
      /* Check if Tx counter different from zero */
      if (hi3c->TxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmatx->XferCpltCallback = I3C_DMADataTransmitCplt;

        /* Set the DMA error callback */
        hi3c->hdmatx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmatx->XferHalfCpltCallback = NULL;
        hi3c->hdmatx->XferAbortCallback    = NULL;

        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDR, hi3c->pXferData->TxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->TxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->TxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->TxBuf.Size + 4U) - (hi3c->pXferData->TxBuf.Size % 4U));
          }

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDWR, size_align_word);
        }
      }

      /* Check if DMA process is well started */
      if ((control_dma_status == HAL_OK) && (tx_dma_status == HAL_OK))
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Tx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->ControlXferCount = 0U;

        /* Enable control DMA Request */
        LL_I3C_EnableDMAReq_Control(hi3c->Instance);

        /* Check if Tx counter different from zero */
        if (hi3c->TxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->TxXferCount = 0U;

          /* Enable Tx data DMA Request */
          LL_I3C_EnableDMAReq_TX(hi3c->Instance);
        }

        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmacr) == HAL_OK)
        {
          hi3c->hdmacr->XferCpltCallback = NULL;
          hi3c->hdmacr->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmatx) == HAL_OK)
        {
          hi3c->hdmatx->XferCpltCallback = NULL;
          hi3c->hdmatx->XferErrorCallback = NULL;
        }

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Controller transmit direct read CCC command in polling mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC(I3C_HandleTypeDef *hi3c,
                                          I3C_XferTypeDef   *pXferData,
                                          uint32_t           timeout)
{
  uint32_t tickstart;
  uint32_t exit_condition;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL)                     ||
        (pXferData->RxBuf.pBuffer == NULL)      ||
        ((pXferData->TxBuf.pBuffer == NULL) && (hi3c->TxXferCount != 0U)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = hi3c->pXferData->RxBuf.Size;

      /* Check on CCC defining byte */
      if (hi3c->TxXferCount != 0U)
      {
        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* Set byte treatment function pointer */
          hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
        }
        else
        {
          /* Set word treatment function pointer */
          hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
        }
      }

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Check on control FIFO enable/disable state */
      if (LL_I3C_IsEnabledControlFIFO(hi3c->Instance) == 1U)
      {
        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Decrement remaining control buffer data counter */
        hi3c->ControlXferCount--;

        /* Initiate a start condition by writing in the CR register */
        WRITE_REG(hi3c->Instance->CR, *hi3c->pXferData->CtrlBuf.pBuffer);

        /* Increment Buffer pointer */
        hi3c->pXferData->CtrlBuf.pBuffer++;
      }

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);

        if (hi3c->TxXferCount != 0U)
        {
          /* Call transmit treatment function */
          hi3c->ptrTxFunc(hi3c);
        }

        /* Call receive treatment function */
        hi3c->ptrRxFunc(hi3c);

        /* Check for the timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }

        if ((__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET) && (hi3c->ControlXferCount > 0U))
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);

          /* Then Initiate a Start condition */
          LL_I3C_RequestTransfer(hi3c->Instance);
        }

        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      } while ((exit_condition == 0U) ||
               ((exit_condition == I3C_EVR_FCF) && (hi3c->ControlXferCount > 0U)));

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check if all data bytes are received */
      if ((hi3c->RxXferCount != 0U) && (status == HAL_OK))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;
        status = HAL_ERROR;
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update returned status value */
        status = HAL_ERROR;
      }

      /* At the end of Rx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Controller transmit direct read CCC command in interrupt mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC_IT(I3C_HandleTypeDef *hi3c,
                                             I3C_XferTypeDef   *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL)                     ||
        (pXferData->RxBuf.pBuffer == NULL)      ||
        ((pXferData->TxBuf.pBuffer == NULL) && (hi3c->TxXferCount != 0U)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      if (handle_state == HAL_I3C_STATE_LISTEN)
      {
        hi3c->XferISR   = I3C_Ctrl_Rx_Listen_Event_ISR;
      }
      else
      {
        hi3c->XferISR   = I3C_Ctrl_Rx_ISR;
      }
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = pXferData->RxBuf.Size;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;

      /* Check on CCC defining byte */
      if (hi3c->TxXferCount != 0U)
      {
        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* Set byte treatment function pointer */
          hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
        }
        else
        {
          /* Set word treatment function pointer */
          hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
        }
      }

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Rx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_CCC_IT);

      /* Initiate a Start condition */
      LL_I3C_RequestTransfer(hi3c->Instance);

    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Controller transmit direct read CCC command in DMA mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_ReceiveCCC_DMA(I3C_HandleTypeDef *hi3c,
                                              I3C_XferTypeDef   *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_StatusTypeDef control_dma_status;
  HAL_StatusTypeDef tx_dma_status = HAL_OK;
  HAL_StatusTypeDef rx_dma_status = HAL_OK;
  uint32_t size_align_word;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL)                     ||
        (pXferData->RxBuf.pBuffer == NULL)      ||
        ((pXferData->TxBuf.pBuffer == NULL) && (hi3c->TxXferCount != 0U)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmarx and hdmacr handle */
    else if ((hi3c->hdmarx == NULL) || (hi3c->hdmacr == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    else if ((hi3c->TxXferCount != 0U) && (hi3c->hdmatx == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->XferISR     = I3C_Ctrl_Rx_DMA_ISR;
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = hi3c->pXferData->RxBuf.Size;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;

      /*------------------------------------ I3C DMA channel for Control Data ----------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmacr->XferCpltCallback = I3C_DMAControlTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmacr->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmacr->XferHalfCpltCallback = NULL;
      hi3c->hdmacr->XferAbortCallback    = NULL;

      /* assert that DMA source and destination width are configured in word */
      assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmacr->Init.SrcDataWidth));
      assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmacr->Init.DestDataWidth));

      /* Enable the control data DMA channel */
      control_dma_status = HAL_DMA_Start_IT(hi3c->hdmacr, (uint32_t)hi3c->pXferData->CtrlBuf.pBuffer,
                                            (uint32_t)&hi3c->Instance->CR, (hi3c->ControlXferCount * 4U));

      /*------------------------------------ I3C DMA channel for defining byte ---------------------------------------*/
      /* Check if Tx counter different from zero */
      if (hi3c->TxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmatx->XferCpltCallback = I3C_DMADataTransmitCplt;

        /* Set the DMA error callback */
        hi3c->hdmatx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmatx->XferHalfCpltCallback = NULL;
        hi3c->hdmatx->XferAbortCallback    = NULL;

        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDR, hi3c->pXferData->TxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Modify size to be multiple of 4 */
          size_align_word = ((hi3c->pXferData->TxBuf.Size + 4U) - (hi3c->pXferData->TxBuf.Size % 4U));

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDWR, size_align_word);
        }
      }
      /*------------------------------------ I3C DMA channel for the Rx Data -----------------------------------------*/
      /* Check if Rx counter different from zero */
      if (hi3c->RxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmarx->XferCpltCallback = I3C_DMADataReceiveCplt;

        /* Set the DMA error callback */
        hi3c->hdmarx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmarx->XferHalfCpltCallback = NULL;
        hi3c->hdmarx->XferAbortCallback    = NULL;

        /* Check on the Rx threshold to know the Rx treatment process : byte or word */
        if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, hi3c->pXferData->RxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->RxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->RxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->RxBuf.Size + 4U) - (hi3c->pXferData->RxBuf.Size % 4U));
          }

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDWR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, size_align_word);
        }
      }

      /* Check if DMA process is well started */
      if ((control_dma_status == HAL_OK) && (tx_dma_status == HAL_OK) && (rx_dma_status == HAL_OK))
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Rx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->ControlXferCount = 0U;

        /* Enable control DMA Request */
        LL_I3C_EnableDMAReq_Control(hi3c->Instance);

        /* Check if Tx counter different from zero */
        if (hi3c->TxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->TxXferCount = 0U;

          /* Enable Tx data DMA Request */
          LL_I3C_EnableDMAReq_TX(hi3c->Instance);
        }

        /* Check if Rx counter different from zero */
        if (hi3c->RxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->RxXferCount = 0U;

          /* Enable Rx data DMA Request */
          LL_I3C_EnableDMAReq_RX(hi3c->Instance);
        }

        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmacr) == HAL_OK)
        {
          hi3c->hdmacr->XferCpltCallback = NULL;
          hi3c->hdmacr->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmatx) == HAL_OK)
        {
          hi3c->hdmatx->XferCpltCallback = NULL;
          hi3c->hdmatx->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmarx) == HAL_OK)
        {
          hi3c->hdmarx->XferCpltCallback = NULL;
          hi3c->hdmarx->XferErrorCallback = NULL;
        }

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Controller private write in polling mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @note   The TxBuf.Size must be equal to the sum of all TxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit(I3C_HandleTypeDef   *hi3c,
                                        I3C_XferTypeDef     *pXferData,
                                        uint32_t             timeout)
{
  uint32_t tickstart;
  uint32_t exit_condition;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData     = pXferData;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Check on control FIFO enable/disable state */
      if (LL_I3C_IsEnabledControlFIFO(hi3c->Instance) == 1U)
      {
        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Decrement remaining control buffer data counter */
        hi3c->ControlXferCount--;

        /* Initiate a start condition by writing in the CR register */
        WRITE_REG(hi3c->Instance->CR, *hi3c->pXferData->CtrlBuf.pBuffer);

        /* Increment Buffer pointer */
        hi3c->pXferData->CtrlBuf.pBuffer++;
      }

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);

        /* Call transmit treatment function */
        hi3c->ptrTxFunc(hi3c);

        /* Check for the timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }

        if ((__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET) && (hi3c->ControlXferCount > 0U))
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);

          /* Then Initiate a Start condition */
          LL_I3C_RequestTransfer(hi3c->Instance);
        }

        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      } while ((exit_condition == 0U) ||
               ((exit_condition == I3C_EVR_FCF) && (hi3c->ControlXferCount > 0U)));

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check if all data bytes are transmitted */
      if ((hi3c->TxXferCount != 0U) && (status == HAL_OK))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;
        status = HAL_ERROR;
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update returned status value */
        status = HAL_ERROR;
      }

      /* At the end of Tx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Controller private write in interrupt mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @note   The TxBuf.Size must be equal to the sum of all TxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit_IT(I3C_HandleTypeDef   *hi3c,
                                           I3C_XferTypeDef     *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      if (handle_state == HAL_I3C_STATE_LISTEN)
      {
        hi3c->XferISR     = I3C_Ctrl_Tx_Listen_Event_ISR;
      }
      else
      {
        hi3c->XferISR     = I3C_Ctrl_Tx_ISR;
      }
      hi3c->pXferData     = pXferData;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Tx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_TX_IT);

      /* Initiate a Start condition */
      LL_I3C_RequestTransfer(hi3c->Instance);
    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Controller private write in DMA mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @note   The TxBuf.Size must be equal to the sum of all TxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Transmit_DMA(I3C_HandleTypeDef   *hi3c,
                                            I3C_XferTypeDef     *pXferData)
{
  HAL_StatusTypeDef control_dma_status;
  HAL_StatusTypeDef tx_dma_status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) ||
        ((hi3c->TxXferCount != 0U) && (pXferData->TxBuf.pBuffer == NULL)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmatx and hdmacr handle */
    else if ((hi3c->hdmatx == NULL) || (hi3c->hdmacr == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->XferISR       = I3C_Ctrl_Tx_DMA_ISR;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData     = pXferData;

      /*------------------------------------ I3C DMA channel for Control Data ----------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmacr->XferCpltCallback = I3C_DMAControlTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmacr->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmacr->XferHalfCpltCallback = NULL;
      hi3c->hdmacr->XferAbortCallback    = NULL;

      /* assert that DMA source and destination width are configured in word */
      assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmacr->Init.SrcDataWidth));
      assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmacr->Init.DestDataWidth));

      /* Enable the control data DMA channel */
      control_dma_status = HAL_DMA_Start_IT(hi3c->hdmacr, (uint32_t)hi3c->pXferData->CtrlBuf.pBuffer,
                                            (uint32_t)&hi3c->Instance->CR, (hi3c->ControlXferCount * 4U));

      /*------------------------------------ I3C DMA channel for the Tx Data -----------------------------------------*/
      /* Check if Tx counter different from zero */
      if (hi3c->TxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmatx->XferCpltCallback = I3C_DMADataTransmitCplt;

        /* Set the DMA error callback */
        hi3c->hdmatx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmatx->XferHalfCpltCallback = NULL;
        hi3c->hdmatx->XferAbortCallback    = NULL;

        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDR, hi3c->pXferData->TxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->TxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->TxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->TxBuf.Size + 4U) - (hi3c->pXferData->TxBuf.Size % 4U));
          }

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDWR, size_align_word);
        }
      }

      /* Check if DMA process is well started */
      if ((control_dma_status == HAL_OK) && (tx_dma_status == HAL_OK))
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Tx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->ControlXferCount = 0U;

        /* Enable control DMA Request */
        LL_I3C_EnableDMAReq_Control(hi3c->Instance);

        /* Check if Tx counter different from zero */
        if (hi3c->TxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->TxXferCount = 0U;

          /* Enable Tx data DMA Request */
          LL_I3C_EnableDMAReq_TX(hi3c->Instance);
        }

        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmacr) == HAL_OK)
        {
          hi3c->hdmacr->XferCpltCallback = NULL;
          hi3c->hdmacr->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmatx) == HAL_OK)
        {
          hi3c->hdmatx->XferCpltCallback = NULL;
          hi3c->hdmatx->XferErrorCallback = NULL;
        }

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Controller private read in polling mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive(I3C_HandleTypeDef   *hi3c,
                                       I3C_XferTypeDef     *pXferData,
                                       uint32_t             timeout)
{
  uint32_t tickstart;
  uint32_t exit_condition;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->State         = HAL_I3C_STATE_BUSY_RX;
      hi3c->pXferData     = pXferData;
      hi3c->RxXferCount   = hi3c->pXferData->RxBuf.Size;

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Check on control FIFO enable/disable state */
      if (LL_I3C_IsEnabledControlFIFO(hi3c->Instance) == 1U)
      {
        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Decrement remaining control buffer data counter */
        hi3c->ControlXferCount--;

        /* Initiate a start condition by writing in the CR register */
        WRITE_REG(hi3c->Instance->CR, *hi3c->pXferData->CtrlBuf.pBuffer);

        /* Increment Buffer pointer */
        hi3c->pXferData->CtrlBuf.pBuffer++;
      }

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);

        /* Call receive treatment function */
        hi3c->ptrRxFunc(hi3c);

        /* Check for the timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }

        if ((__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET) && (hi3c->ControlXferCount > 0U))
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);

          /* Then Initiate a Start condition */
          LL_I3C_RequestTransfer(hi3c->Instance);
        }

        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      } while ((exit_condition == 0U) ||
               ((exit_condition == I3C_EVR_FCF) && (hi3c->ControlXferCount > 0U)));

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check if all data bytes are received */
      if ((hi3c->RxXferCount != 0U) && (status == HAL_OK))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;
        status = HAL_ERROR;
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update returned status value */
        status = HAL_ERROR;
      }

      /* At the end of Rx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Controller private read in interrupt mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive_IT(I3C_HandleTypeDef   *hi3c,
                                          I3C_XferTypeDef     *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      if (handle_state == HAL_I3C_STATE_LISTEN)
      {
        hi3c->XferISR     = I3C_Ctrl_Rx_Listen_Event_ISR;
      }
      else
      {
        hi3c->XferISR     = I3C_Ctrl_Rx_ISR;
      }
      hi3c->pXferData     = pXferData;
      hi3c->RxXferCount   = hi3c->pXferData->RxBuf.Size;
      hi3c->State         = HAL_I3C_STATE_BUSY_RX;

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Rx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_IT);

      /* Initiate a Start condition */
      LL_I3C_RequestTransfer(hi3c->Instance);
    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Controller private read in DMA mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            (control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_Receive_DMA(I3C_HandleTypeDef   *hi3c,
                                           I3C_XferTypeDef     *pXferData)
{
  HAL_StatusTypeDef control_dma_status;
  HAL_StatusTypeDef rx_dma_status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmarx and hdmacr handle */
    else if ((hi3c->hdmarx == NULL) || (hi3c->hdmacr == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->XferISR       = I3C_Ctrl_Rx_DMA_ISR;
      hi3c->pXferData     = pXferData;
      hi3c->RxXferCount   = hi3c->pXferData->RxBuf.Size;
      hi3c->State         = HAL_I3C_STATE_BUSY_RX;

      /*------------------------------------ I3C DMA channel for Control Data ----------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmacr->XferCpltCallback = I3C_DMAControlTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmacr->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmacr->XferHalfCpltCallback = NULL;
      hi3c->hdmacr->XferAbortCallback    = NULL;

      /* assert that DMA source and destination width are configured in word */
      assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmacr->Init.SrcDataWidth));
      assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmacr->Init.DestDataWidth));

      /* Enable the control data DMA channel */
      control_dma_status = HAL_DMA_Start_IT(hi3c->hdmacr, (uint32_t)hi3c->pXferData->CtrlBuf.pBuffer,
                                            (uint32_t)&hi3c->Instance->CR, (hi3c->ControlXferCount * 4U));

      /*------------------------------------ I3C DMA channel for the Rx Data -----------------------------------------*/
      /* Check if Rx counter different from zero */
      if (hi3c->RxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmarx->XferCpltCallback = I3C_DMADataReceiveCplt;

        /* Set the DMA error callback */
        hi3c->hdmarx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmarx->XferHalfCpltCallback = NULL;
        hi3c->hdmarx->XferAbortCallback    = NULL;

        /* Check on the Rx threshold to know the Rx treatment process : byte or word */
        if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, hi3c->pXferData->RxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->RxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->RxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->RxBuf.Size + 4U) - (hi3c->pXferData->RxBuf.Size % 4U));
          }

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDWR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, size_align_word);
        }
      }

      /* Check if DMA process is well started */
      if ((control_dma_status == HAL_OK) && (rx_dma_status == HAL_OK))
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Rx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->ControlXferCount = 0U;

        /* Enable control DMA Request */
        LL_I3C_EnableDMAReq_Control(hi3c->Instance);

        /* Check if Rx counter different from zero */
        if (hi3c->RxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->RxXferCount = 0U;

          /* Enable Rx data DMA Request */
          LL_I3C_EnableDMAReq_RX(hi3c->Instance);
        }

        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmacr) == HAL_OK)
        {
          hi3c->hdmacr->XferCpltCallback = NULL;
          hi3c->hdmacr->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmarx) == HAL_OK)
        {
          hi3c->hdmarx->XferCpltCallback = NULL;
          hi3c->hdmarx->XferErrorCallback = NULL;
        }

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }

    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Controller multiple Direct CCC Command, I3C private or I2C transfer in interrupt mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @note   This function must be called to transfer read/write I3C or I2C private data or a direct read/write CCC.
  * @note   The TxBuf.Size must be equal to the sum of all TxBuf.Size exist in the descriptor.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmit and receive
  *                            buffers (control buffer, data buffers and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_MultipleTransfer_IT(I3C_HandleTypeDef   *hi3c,
                                                   I3C_XferTypeDef     *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL)                                               ||
        ((pXferData->RxBuf.pBuffer == NULL) && (hi3c->RxXferCount != 0U)) ||
        ((pXferData->TxBuf.pBuffer == NULL) && (hi3c->TxXferCount != 0U)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      if (handle_state == HAL_I3C_STATE_LISTEN)
      {
        hi3c->XferISR     = I3C_Ctrl_Multiple_Xfer_Listen_Event_ISR;
      }
      else
      {
        hi3c->XferISR     = I3C_Ctrl_Multiple_Xfer_ISR;
      }
      hi3c->pXferData     = pXferData;
      hi3c->TxXferCount   = hi3c->pXferData->TxBuf.Size;
      hi3c->RxXferCount   = hi3c->pXferData->RxBuf.Size;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX_RX;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Tx and Rx process interrupts */
      I3C_Enable_IRQ(hi3c, (I3C_XFER_CONTROLLER_TX_IT | I3C_XFER_CONTROLLER_RX_IT));

      /* Initiate a Start condition */
      LL_I3C_RequestTransfer(hi3c->Instance);
    }
  }
  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Controller multiple Direct CCC Command, I3C private or I2C transfer in DMA mode.
  * @note   The function @ref HAL_I3C_AddDescToFrame() must be called before initiate a transfer.
  * @note   The Tx FIFO threshold @ref HAL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
  *         multiple transmission frames.
  * @note   The TxBuf.Size must be equal to the sum of all TxBuf.Size exist in the descriptor.
  * @note   The RxBuf.Size must be equal to the sum of all RxBuf.Size exist in the descriptor.
  * @note   This function must be called to transfer read/write private data or a direct read/write CCC command.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmit and receive
  *                            buffers(control buffer, data buffer and status buffer).
  *                            This value contain transfer data after called @ref HAL_I3C_AddDescToFrame().
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_MultipleTransfer_DMA(I3C_HandleTypeDef   *hi3c, I3C_XferTypeDef     *pXferData)
{
  HAL_StatusTypeDef control_dma_status;
  HAL_StatusTypeDef tx_dma_status = HAL_OK;
  HAL_StatusTypeDef rx_dma_status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL)                                               ||
        ((pXferData->RxBuf.pBuffer == NULL) && (hi3c->RxXferCount != 0U)) ||
        ((pXferData->TxBuf.pBuffer == NULL) && (hi3c->TxXferCount != 0U)))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmatx, hdmarx and hdmacr handle */
    else if ((hi3c->hdmatx == NULL) || (hi3c->hdmacr == NULL) || (hi3c->hdmarx == NULL))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
      hi3c->XferISR       = I3C_Ctrl_Multiple_Xfer_DMA_ISR;
      hi3c->pXferData     = pXferData;
      hi3c->RxXferCount   = hi3c->pXferData->RxBuf.Size;
      hi3c->TxXferCount   = hi3c->pXferData->TxBuf.Size;
      hi3c->State         = HAL_I3C_STATE_BUSY_TX_RX;

      /*------------------------------------ I3C DMA channel for Control Data -------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmacr->XferCpltCallback = I3C_DMAControlTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmacr->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmacr->XferHalfCpltCallback = NULL;
      hi3c->hdmacr->XferAbortCallback    = NULL;

      /* assert that DMA source and destination width are configured in word */
      assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmacr->Init.SrcDataWidth));
      assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmacr->Init.DestDataWidth));

      /* Enable the control data DMA channel */
      control_dma_status = HAL_DMA_Start_IT(hi3c->hdmacr, (uint32_t)hi3c->pXferData->CtrlBuf.pBuffer,
                                            (uint32_t)&hi3c->Instance->CR, (hi3c->ControlXferCount * 4U));

      /*------------------------------------ I3C DMA channel for the Rx Data --------------------------------*/
      /* Check if Rx counter different from zero */
      if (hi3c->RxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmarx->XferCpltCallback = I3C_DMADataReceiveCplt;

        /* Set the DMA error callback */
        hi3c->hdmarx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmarx->XferHalfCpltCallback = NULL;
        hi3c->hdmarx->XferAbortCallback    = NULL;

        /* Check on the Rx threshold to know the Rx treatment process : byte or word */
        if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, hi3c->pXferData->RxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmarx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->RxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->RxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->RxBuf.Size + 4U) - (hi3c->pXferData->RxBuf.Size % 4U));
          }

          /* Enable the Rx data DMA channel */
          rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDWR,
                                           (uint32_t)hi3c->pXferData->RxBuf.pBuffer, size_align_word);
        }
      }

      /*------------------------------------ I3C DMA channel for the Tx Data --------------------------------*/
      /* Check if Tx counter different from zero */
      if (hi3c->TxXferCount != 0U)
      {
        /* Set the I3C DMA transfer complete callback */
        hi3c->hdmatx->XferCpltCallback = I3C_DMADataTransmitCplt;

        /* Set the DMA error callback */
        hi3c->hdmatx->XferErrorCallback = I3C_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hi3c->hdmatx->XferHalfCpltCallback = NULL;
        hi3c->hdmatx->XferAbortCallback    = NULL;

        /* Check on the Tx threshold to know the Tx treatment process : byte or word */
        if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
        {
          /* assert that DMA source and destination width are configured in byte */
          assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDR, hi3c->pXferData->TxBuf.Size);
        }
        else
        {
          /* assert that DMA source and destination width are configured in word */
          assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
          assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmatx->Init.DestDataWidth));

          /* Check to align data size in words */
          if ((hi3c->pXferData->TxBuf.Size % 4U) == 0U)
          {
            /* Keep the same size */
            size_align_word = hi3c->pXferData->TxBuf.Size;
          }
          else
          {
            /* Modify size to be multiple of 4 */
            size_align_word = ((hi3c->pXferData->TxBuf.Size + 4U) - (hi3c->pXferData->TxBuf.Size % 4U));
          }

          /* Enable the Tx data DMA channel */
          tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                           (uint32_t)&hi3c->Instance->TDWR, size_align_word);
        }
      }

      /* Check if DMA process is well started */
      if ((control_dma_status == HAL_OK) && (tx_dma_status == HAL_OK) && (rx_dma_status == HAL_OK))
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Tx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->ControlXferCount = 0U;

        /* Enable control DMA Request */
        LL_I3C_EnableDMAReq_Control(hi3c->Instance);

        /* Check if Rx counter different from zero */
        if (hi3c->RxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->RxXferCount = 0U;

          /* Enable Rx data DMA Request */
          LL_I3C_EnableDMAReq_RX(hi3c->Instance);
        }

        /* Check if Tx counter different from zero */
        if (hi3c->TxXferCount != 0U)
        {
          /* Update the number of remaining data bytes */
          hi3c->TxXferCount = 0U;

          /* Enable Tx data DMA Request */
          LL_I3C_EnableDMAReq_TX(hi3c->Instance);
        }

        /* Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmacr) == HAL_OK)
        {
          hi3c->hdmacr->XferCpltCallback = NULL;
          hi3c->hdmacr->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmatx) == HAL_OK)
        {
          hi3c->hdmatx->XferCpltCallback = NULL;
          hi3c->hdmatx->XferErrorCallback = NULL;
        }

        /* Set callback to NULL if DMA started */
        if (HAL_DMA_Abort(hi3c->hdmarx) == HAL_OK)
        {
          hi3c->hdmarx->XferCpltCallback = NULL;
          hi3c->hdmarx->XferErrorCallback = NULL;
        }

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }
  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Controller assign dynamic address (send a broadcast ENTDAA CCC command) in polling mode.
  * @param  hi3c           : [IN]     Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                   information for the specified I3C.
  * @param  target_payload : [IN/OUT] Pointer to the returned target payload value.
  * @param  dynOption      : [IN]     Parameter indicates the Dynamic address assignment option.
  *                                   It can be one value of @ref I3C_DYNAMIC_ADDRESS_OPTION_DEFINITION.
  * @param  timeout        : [IN]     Timeout duration in millisecond.
  * @retval HAL Status     :          Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_DynAddrAssign(I3C_HandleTypeDef  *hi3c,
                                             uint64_t           *target_payload,
                                             uint32_t            dynOption,
                                             uint32_t            timeout)
{
  uint32_t tickstart;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on parameters */
  assert_param(IS_I3C_ENTDAA_OPTION(dynOption));

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    if (target_payload == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Launch a RSTDAA procedure before launch ENTDAA */
      if ((dynOption == I3C_RSTDAA_THEN_ENTDAA) &&
          ((handle_state == HAL_I3C_STATE_READY) || (handle_state == HAL_I3C_STATE_LISTEN)))
      {
        /* Set handle transfer parameters */
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
        hi3c->State     = HAL_I3C_STATE_BUSY_DAA;

        /* Init tickstart for timeout management */
        tickstart = HAL_GetTick();

        /* Enable arbitration header */
        LL_I3C_EnableArbitrationHeader(hi3c->Instance);

        /* Write CCC information in the control register */
        LL_I3C_ControllerHandleCCC(hi3c->Instance, I3C_BROADCAST_RSTDAA, 0U, LL_I3C_GENERATE_STOP);

        /* Wait Frame completion flag */
        status = I3C_WaitOnFlagUntilTimeout(hi3c, HAL_I3C_FLAG_FCF, RESET, timeout, tickstart);

        /* Clear frame complete flag */
        if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
        {
          LL_I3C_ClearFlag_FC(hi3c->Instance);
        }

        /* Check on error flag */
        if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
        {
          /* Clear error flag */
          LL_I3C_ClearFlag_ERR(hi3c->Instance);

          /* Update handle error code parameter */
          I3C_GetErrorSources(hi3c);

          status = HAL_ERROR;
        }

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }

      if (status == HAL_OK)
      {
        /* check on the State */
        if ((handle_state == HAL_I3C_STATE_READY) || (handle_state == HAL_I3C_STATE_LISTEN) ||
            (handle_state == HAL_I3C_STATE_BUSY_DAA))
        {
          /* Check on the state */
          if (handle_state != HAL_I3C_STATE_BUSY_DAA)
          {
            /* Set handle transfer parameters */
            hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
            hi3c->State     = HAL_I3C_STATE_BUSY_DAA;

            /* Init tickstart for timeout management */
            tickstart = HAL_GetTick();

            /* Enable arbitration header */
            LL_I3C_EnableArbitrationHeader(hi3c->Instance);

            /* Write CCC information in the control register */
            LL_I3C_ControllerHandleCCC(hi3c->Instance, I3C_BROADCAST_ENTDAA, 0U, LL_I3C_GENERATE_STOP);
          }
          else
          {
            /* Init tickstart for timeout management */
            tickstart = HAL_GetTick();
          }

          /* Wait frame complete flag or TX FIFO not full flag until timeout */
          status = I3C_WaitOnDAAUntilTimeout(hi3c, timeout, tickstart);

          /* Check TX FIFO not full flag */
          if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_TXFNFF) == SET)
          {
            /* Check on the Rx FIFO threshold to know the Rx treatment process : byte or word */
            if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
            {
              /* For loop to get target payload */
              for (uint32_t index = 0U; index < 8U; index++)
              {
                /* Retrieve payload byte by byte */
                *target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData8(hi3c->Instance) << (index * 8U));
              }
            }
            else
            {
              /* Retrieve first 32 bits payload */
              *target_payload = (uint64_t)LL_I3C_ReceiveData32(hi3c->Instance);

              /* Retrieve second 32 bits payload */
              *target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData32(hi3c->Instance) << 32U);
            }

            status = HAL_BUSY;
          }
          /* Check on frame complete flag */
          else
          {
            /* Clear frame complete flag */
            if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
            {
              /* Clear frame complete flag */
              LL_I3C_ClearFlag_FC(hi3c->Instance);
            }
            /* Update handle state parameter */
            I3C_StateUpdate(hi3c);
          }
        }
        else
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
          status = HAL_ERROR;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Controller assign dynamic address (send a broadcast ENTDAA CCC command) in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  dynOption  : [IN]  Parameter indicates the Dynamic address assignment option.
  *                            It can be one value of @ref I3C_DYNAMIC_ADDRESS_OPTION_DEFINITION.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_DynAddrAssign_IT(I3C_HandleTypeDef *hi3c, uint32_t dynOption)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on parameters */
  assert_param(IS_I3C_ENTDAA_OPTION(dynOption));

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY_DAA;
      hi3c->XferISR   = I3C_Ctrl_DAA_ISR;

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Dynamic Address Assignment process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_DAA_IT);

      /* Enable arbitration header */
      LL_I3C_EnableArbitrationHeader(hi3c->Instance);

      /* Launch a RSTDAA procedure before launch ENTDAA */
      if (dynOption == I3C_RSTDAA_THEN_ENTDAA)
      {
        /* Write RSTDAA CCC information in the control register */
        LL_I3C_ControllerHandleCCC(hi3c->Instance, I3C_BROADCAST_RSTDAA, 0U, LL_I3C_GENERATE_RESTART);
      }
      else
      {
        /* Write ENTDAA CCC information in the control register */
        LL_I3C_ControllerHandleCCC(hi3c->Instance, I3C_BROADCAST_ENTDAA, 0U, LL_I3C_GENERATE_STOP);
      }
    }
  }

  return status;
}

/**
  * @brief  Controller set dynamic address.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  devAddress : [IN]  Value of the dynamic address to be assigned.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_SetDynAddr(I3C_HandleTypeDef *hi3c, uint8_t devAddress)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check if Tx FIFO requests data */
    if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_TXFNFF) == SET)
    {
      /* Write device address in the TDR register */
      LL_I3C_TransmitData8(hi3c->Instance, devAddress);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Check if I3C target device is ready for communication.
  * @param  hi3c       : [IN] Pointer to a I3C_HandleTypeDef structure that contains
  *                           the configuration information for the specified I3C.
  * @param  devAddress : [IN] Value of the device dynamic address.
  * @param  trials     : [IN] Number of trials
  * @param  timeout    : [IN] Timeout duration
  * @retval HAL Status :      Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_IsDeviceI3C_Ready(I3C_HandleTypeDef *hi3c,
                                                 uint8_t            devAddress,
                                                 uint32_t           trials,
                                                 uint32_t           timeout)
{
  I3C_DeviceTypeDef device;
  HAL_StatusTypeDef status;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Initiate a device address */
    device.Address = devAddress;

    /* Initiate a message type */
    device.MessageType = LL_I3C_CONTROLLER_MTYPE_PRIVATE;

    /* Check if the device is ready*/
    status = I3C_Ctrl_IsDevice_Ready(hi3c, &device, trials, timeout);
  }

  return status;
}

/**
  * @brief  Check if I2C target device is ready for communication.
  * @param  hi3c       : [IN] Pointer to a I3C_HandleTypeDef structure that contains
  *                           the configuration information for the specified I3C.
  * @param  devAddress : [IN] Value of the device dynamic address.
  * @param  trials     : [IN] Number of trials
  * @param  timeout    : [IN] Timeout duration
  * @retval HAL Status :      Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_IsDeviceI2C_Ready(I3C_HandleTypeDef *hi3c,
                                                 uint8_t            devAddress,
                                                 uint32_t           trials,
                                                 uint32_t           timeout)
{
  I3C_DeviceTypeDef device;
  HAL_StatusTypeDef status;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Initiate a device address */
    device.Address = devAddress;

    /* Initiate a message type */
    device.MessageType = LL_I3C_CONTROLLER_MTYPE_LEGACY_I2C;

    /* Check if the device is ready*/
    status = I3C_Ctrl_IsDevice_Ready(hi3c, &device, trials, timeout);
  }

  return status;
}

/**
  * @brief Controller generates arbitration (message header {S/Sr + 0x7E addr + W}) in polling mode.
  * @param  hi3c       : [IN] Pointer to an I3C_HandleTypeDef structure that contains
  *                           the configuration information for the specified I3C.
  * @param  timeout    : [IN] Timeout duration
  * @retval HAL Status :      Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Ctrl_GenerateArbitration(I3C_HandleTypeDef *hi3c, uint32_t timeout)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  __IO uint32_t exit_condition;
  uint32_t tickstart;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      hi3c->State = HAL_I3C_STATE_BUSY;

      /* Disable exit pattern */
      LL_I3C_DisableExitPattern(hi3c->Instance);
      /* Disable reset pattern */
      LL_I3C_DisableResetPattern(hi3c->Instance);

      /* Write message control register */
      WRITE_REG(hi3c->Instance->CR, LL_I3C_CONTROLLER_MTYPE_HEADER | LL_I3C_GENERATE_STOP);

      /* Calculate exit_condition value based on Frame complete and error flags */
      exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));

      tickstart = HAL_GetTick();

      while (exit_condition == 0U)
      {
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            /* Update I3C error code */
            hi3c->ErrorCode |= HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }
        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      }

      if (status == HAL_OK)
      {
        /* Check if the FCF flag has been set */
        if (__HAL_I3C_GET_FLAG(hi3c, I3C_EVR_FCF) == SET)
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);
        }
        else
        {
          /* Clear error flag */
          LL_I3C_ClearFlag_ERR(hi3c->Instance);

          /* Update handle error code parameter */
          I3C_GetErrorSources(hi3c);

          /* Update returned status value */
          status = HAL_ERROR;
        }
      }

      /* At the end of Rx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group6 Target operational functions.
  * @brief    I3C target operational functions.
  *
@verbatim
 =======================================================================================================================
                                    ##### Target operational functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to manage target I3C operation.

         (+) Call the function HAL_I3C_Tgt_Transmit() to transmit private data in polling mode.
         (+) Call the function HAL_I3C_Tgt_Transmit_IT() to transmit private data in interrupt mode.
         (+) Call the function HAL_I3C_Tgt_Transmit_DMA() to transmit private data in DMA mode.
         (+) Call the function HAL_I3C_Tgt_Receive() to receive private data in polling mode.
         (+) Call the function HAL_I3C_Tgt_Receive_IT() to receive private data in interrupt mode.
         (+) Call the function HAL_I3C_Tgt_Receive_DMA() to receive private data in DMA mode.
         (+) Call the function HAL_I3C_Tgt_ControlRoleReq() to send a control-role request in polling mode.
         (+) Call the function HAL_I3C_Tgt_ControlRoleReq_IT() to send a control-role request in interrupt mode.
         (+) Call the function HAL_I3C_Tgt_HotJoinReq() to send a Hot-Join request in polling mode.
         (+) Call the function HAL_I3C_Tgt_HotJoinReq_IT() to send a Hot-Join request in interrupt mode.
         (+) Call the function HAL_I3C_Tgt_IBIReq() to send an IBI request in polling mode.
         (+) Call the function HAL_I3C_Tgt_IBIReq_IT() to send an IBI request in interrupt mode.

         (+) Those functions are called only when mode is Target.

@endverbatim
  * @{
  */

/**
  * @brief  Target transmit private data in polling mode.
  * @note   Target FIFO preload data is forced within this API for timing purpose.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            information (Pointer to the Tx buffer (TxBuf.pBuffer) and size of data
  *                            to transmit in bytes (TxBuf.Size)).
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData, uint32_t timeout)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->TxBuf.pBuffer == NULL) || (pXferData->TxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF or GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData   = pXferData;
      hi3c->TxXferCount = pXferData->TxBuf.Size;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Set Preload information */
      LL_I3C_ConfigTxPreload(hi3c->Instance, (uint16_t)hi3c->pXferData->TxBuf.Size);

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        /* Call transmit treatment function */
        hi3c->ptrTxFunc(hi3c);

        /* Check for the Timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }
        /* Exit loop on Frame complete or error flags */
      } while ((READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF)) == 0U);

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check if all data bytes are transmitted */
      if ((LL_I3C_GetXferDataCount(hi3c->Instance) != hi3c->pXferData->TxBuf.Size) && (status == HAL_OK))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;
        status = HAL_ERROR;
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update returned status value */
        status = HAL_ERROR;
      }

      /* At the end of Tx process update state to Previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Target transmit private data in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            information (Pointer to the Tx buffer (TxBuf.pBuffer) and size of data
  *                            to transmit in bytes (TxBuf.Size)).
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit_IT(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->TxBuf.pBuffer == NULL) || (pXferData->TxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF and GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;

      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData   = pXferData;
      hi3c->TxXferCount = pXferData->TxBuf.Size;
      hi3c->XferISR     = I3C_Tgt_Tx_ISR;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrTxFunc = &I3C_TransmitWordTreatment;
      }

      /* Set Preload information */
      LL_I3C_ConfigTxPreload(hi3c->Instance, (uint16_t)hi3c->pXferData->TxBuf.Size);

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Tx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_TX_IT);
    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Target transmit private data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required transmission buffers
  *                            information (Pointer to the Tx buffer (TxBuf.pBuffer) and size of data
  *                            to transmit in bytes (TxBuf.Size)).
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Transmit_DMA(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData)
{
  HAL_StatusTypeDef tx_dma_status;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->TxBuf.pBuffer == NULL) || (pXferData->TxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmatx handle */
    else if (hi3c->hdmatx == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF and GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_TX;
      hi3c->pXferData   = pXferData;
      hi3c->TxXferCount = pXferData->TxBuf.Size;
      hi3c->XferISR     = I3C_Tgt_Tx_DMA_ISR;

      /* Set Preload information */
      LL_I3C_ConfigTxPreload(hi3c->Instance, (uint16_t)hi3c->pXferData->TxBuf.Size);

      /*------------------------------------ I3C DMA channel for the Tx Data -----------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmatx->XferCpltCallback = I3C_DMADataTransmitCplt;

      /* Set the DMA error callback */
      hi3c->hdmatx->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmatx->XferHalfCpltCallback = NULL;
      hi3c->hdmatx->XferAbortCallback    = NULL;

      /* Check on the Tx threshold to know the Tx treatment process : byte or word */
      if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
      {
        /* assert that DMA source and destination width are configured in byte */
        assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
        assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmatx->Init.DestDataWidth));

        /* Enable the Tx data DMA channel */
        tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                         (uint32_t)&hi3c->Instance->TDR, hi3c->pXferData->TxBuf.Size);
      }
      else
      {
        /* assert that DMA source and destination width are configured in word */
        assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmatx->Init.SrcDataWidth));
        assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmatx->Init.DestDataWidth));

        /* Check to align data size in words */
        if ((hi3c->pXferData->TxBuf.Size % 4U) == 0U)
        {
          /* Keep the same size */
          size_align_word = hi3c->pXferData->TxBuf.Size;
        }
        else
        {
          /* Modify size to be multiple of 4 */
          size_align_word = ((hi3c->pXferData->TxBuf.Size + 4U) - (hi3c->pXferData->TxBuf.Size % 4U));
        }

        /* Enable the Tx data DMA channel */
        tx_dma_status = HAL_DMA_Start_IT(hi3c->hdmatx, (uint32_t)hi3c->pXferData->TxBuf.pBuffer,
                                         (uint32_t)&hi3c->Instance->TDWR, size_align_word);
      }

      /* Check if DMA process is well started */
      if (tx_dma_status == HAL_OK)
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Tx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->TxXferCount = 0U;

        /* Enable Tx data DMA Request */
        LL_I3C_EnableDMAReq_TX(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        hi3c->hdmatx->XferCpltCallback = NULL;
        hi3c->hdmatx->XferErrorCallback = NULL;

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Target receive private data in polling mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            information (Pointer to the Rx buffer (RxBuf.pBuffer) and size of data
  *                            to be received in bytes (RxBuf.Size)).
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Receive(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData, uint32_t timeout)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL) || (pXferData->RxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF and GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = pXferData->RxBuf.Size;

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Do while until FC (Frame Complete) is set or timeout */
      do
      {
        if (hi3c->RxXferCount > 0U)
        {
          /* Call receive treatment function */
          hi3c->ptrRxFunc(hi3c);
        }

        /* Check for the Timeout */
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }
        /* Exit loop on Frame complete or error flags */
      } while ((READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF)) == 0U);

      /* Clear frame complete flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_FCF) == SET)
      {
        LL_I3C_ClearFlag_FC(hi3c->Instance);
      }

      /* Check if all data bytes are received */
      if ((LL_I3C_GetXferDataCount(hi3c->Instance) != hi3c->pXferData->RxBuf.Size) && (status == HAL_OK))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;
        status = HAL_ERROR;
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        status = HAL_ERROR;
      }

      /* At the end of Rx process update state to previous state */
      I3C_StateUpdate(hi3c);
    }
  }

  return status;
}

/**
  * @brief  Target receive private data in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            information (Pointer to the Rx buffer (RxBuf.pBuffer) and size of data
  *                            to be received in bytes (RxBuf.Size)).
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Receive_IT(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL) || (pXferData->RxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF and GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = pXferData->RxBuf.Size;
      hi3c->XferISR     = I3C_Tgt_Rx_ISR;

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* Set byte treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveByteTreatment;
      }
      else
      {
        /* Set word treatment function pointer */
        hi3c->ptrRxFunc = &I3C_ReceiveWordTreatment;
      }

      /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
      of I3C interrupt handle execution before current process unlock */

      /* Enable Rx process interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_RX_IT);
    }
  }

  return status;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Target receive private data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  pXferData  : [IN]  Pointer to an I3C_XferTypeDef structure that contains required reception buffers
  *                            information (Pointer to the Rx buffer (RxBuf.pBuffer) and size of data
  *                            to be received in bytes (RxBuf.Size)).
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_Receive_DMA(I3C_HandleTypeDef *hi3c, I3C_XferTypeDef *pXferData)
{
  HAL_StatusTypeDef rx_dma_status;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t size_align_word;
  uint32_t it_source;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    it_source = READ_REG(hi3c->Instance->IER);

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on user parameters */
    if ((pXferData == NULL) || (pXferData->RxBuf.pBuffer == NULL) || (pXferData->RxBuf.Size == 0U))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on hdmarx handle */
    else if (hi3c->hdmarx == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DMA_PARAM;
      status = HAL_ERROR;

      /* Update handle state parameter */
      I3C_StateUpdate(hi3c);
    }
    /* check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* check if DEF and GRP CCC notifications are enabled */
    else if ((I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_DEFIE) != RESET) ||
             (I3C_CHECK_IT_SOURCE(it_source, HAL_I3C_IT_GRPIE) != RESET))
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Set handle transfer parameters */
      hi3c->ErrorCode   = HAL_I3C_ERROR_NONE;
      hi3c->State       = HAL_I3C_STATE_BUSY_RX;
      hi3c->pXferData   = pXferData;
      hi3c->RxXferCount = pXferData->RxBuf.Size;
      hi3c->XferISR     = I3C_Tgt_Rx_DMA_ISR;

      /*------------------------------------ I3C DMA channel for the Rx Data ---------------------------------------*/
      /* Set the I3C DMA transfer complete callback */
      hi3c->hdmarx->XferCpltCallback = I3C_DMADataReceiveCplt;

      /* Set the DMA error callback */
      hi3c->hdmarx->XferErrorCallback = I3C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi3c->hdmarx->XferHalfCpltCallback = NULL;
      hi3c->hdmarx->XferAbortCallback    = NULL;

      /* Check on the Rx threshold to know the Rx treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* assert that DMA source and destination width are configured in byte */
        assert_param(IS_I3C_DMASOURCEBYTE_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
        assert_param(IS_I3C_DMADESTINATIONBYTE_VALUE(hi3c->hdmarx->Init.DestDataWidth));

        /* Enable the Rx data DMA channel */
        rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDR,
                                         (uint32_t)hi3c->pXferData->RxBuf.pBuffer, hi3c->pXferData->RxBuf.Size);
      }
      else
      {
        /* assert that DMA source and destination width are configured in word */
        assert_param(IS_I3C_DMASOURCEWORD_VALUE(hi3c->hdmarx->Init.SrcDataWidth));
        assert_param(IS_I3C_DMADESTINATIONWORD_VALUE(hi3c->hdmarx->Init.DestDataWidth));

        /* Check to align data size in words */
        if ((hi3c->pXferData->RxBuf.Size % 4U) == 0U)
        {
          /* Keep the same size */
          size_align_word = hi3c->pXferData->RxBuf.Size;
        }
        else
        {
          /* Modify size to be multiple of 4 */
          size_align_word = ((hi3c->pXferData->RxBuf.Size + 4U) - (hi3c->pXferData->RxBuf.Size % 4U));
        }

        /* Enable the Rx data DMA channel */
        rx_dma_status = HAL_DMA_Start_IT(hi3c->hdmarx, (uint32_t)&hi3c->Instance->RDWR,
                                         (uint32_t)hi3c->pXferData->RxBuf.pBuffer, size_align_word);
      }

      if (rx_dma_status == HAL_OK)
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process to avoid the risk
        of I3C interrupt handle execution before current process unlock */

        /* Enable Rx process interrupts */
        I3C_Enable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update the number of remaining data bytes */
        hi3c->RxXferCount = 0U;

        /* Enable Rx data DMA Request */
        LL_I3C_EnableDMAReq_RX(hi3c->Instance);
      }
      else
      {
        /* Set callback to NULL if DMA started */
        hi3c->hdmarx->XferCpltCallback = NULL;
        hi3c->hdmarx->XferErrorCallback = NULL;

        hi3c->ErrorCode = HAL_I3C_ERROR_DMA;
        status = HAL_ERROR;

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Target send control role request in polling mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_ControlRoleReq(I3C_HandleTypeDef *hi3c, uint32_t timeout)
{
  uint32_t tickstart;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Verify if control role request feature is enabled */
      if (LL_I3C_IsEnabledControllerRoleReq(hi3c->Instance) != 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Request Controllership */
      LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_CONTROLLER_ROLE_REQ, 0U);

      /* Wait Controllership completion confirmation flag */
      status = I3C_WaitOnFlagUntilTimeout(hi3c, HAL_I3C_FLAG_CRUPDF, RESET, timeout, tickstart);

      /* Clear Control role request flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_CRUPDF) == SET)
      {
        LL_I3C_ClearFlag_CRUPD(hi3c->Instance);
      }

      /* Check on error flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update handle state parameter to previous state */
        I3C_StateUpdate(hi3c);

        status = HAL_ERROR;
      }
      else
      {
        /* Update handle state parameter to previous state */
        I3C_StateUpdate(hi3c);
      }
    }
  }

  return status;
}

/**
  * @brief  Target send control role request in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_ControlRoleReq_IT(I3C_HandleTypeDef *hi3c)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Verify if control role request feature is enabled */
      if (LL_I3C_IsEnabledControllerRoleReq(hi3c->Instance) != 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* Update handle parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;
      hi3c->XferISR   = I3C_Tgt_CtrlRole_ISR;

      /* Enable controller-role update and error interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_CTRLROLE);

      /* Request Controllership */
      LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_CONTROLLER_ROLE_REQ, 0U);
    }
  }

  return status;
}

/**
  * @brief  Target send hot join request in polling mode.
  * @param  hi3c       : [IN]     Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                               information for the specified I3C.
  * @param  pAddress   : [IN/OUT] Pointer to the target own dynamic address assigned by the controller.
  * @param  timeout    : [IN]     Timeout duration in millisecond.
  * @retval HAL Status :          Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_HotJoinReq(I3C_HandleTypeDef *hi3c, uint8_t *pAddress, uint32_t timeout)
{
  uint32_t tickstart;
  HAL_I3C_StateTypeDef handle_state;
  uint32_t valid_dynamic_address;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on the pAddress value */
    if (pAddress == NULL)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check on the Mode */
    else if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    else
    {
      /* Check on the hot join request feature */
      if (LL_I3C_IsEnabledHotJoin(hi3c->Instance) != 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;

      /* Init tickstart for timeout management */
      tickstart = HAL_GetTick();

      /* Request hot join */
      LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_HOT_JOIN, 0U);

      /* Wait hot join completion confirmation flag */
      status = I3C_WaitOnFlagUntilTimeout(hi3c, HAL_I3C_FLAG_DAUPDF, RESET, timeout, tickstart);

      /* Clear dynamic address update flag */
      if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_DAUPDF) == SET)
      {
        LL_I3C_ClearFlag_DAUPD(hi3c->Instance);
      }

      /* Get dynamic address validity flag */
      valid_dynamic_address = LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance);

      /* Check the validity of the own dynamic address */
      if (valid_dynamic_address == 0U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_DYNAMIC_ADDR;
        status = HAL_ERROR;

        /* Update handle state parameter to previous state */
        I3C_StateUpdate(hi3c);
      }
      /* Check on error flag */
      else if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
      {
        /* Clear error flag */
        LL_I3C_ClearFlag_ERR(hi3c->Instance);

        /* Update handle error code parameter */
        I3C_GetErrorSources(hi3c);

        /* Update handle state parameter to previous state */
        I3C_StateUpdate(hi3c);

        status = HAL_ERROR;
      }
      else
      {
        /* Update handle state parameter to previous state */
        I3C_StateUpdate(hi3c);

        /* Get assigned dynamic address */
        *pAddress = LL_I3C_GetOwnDynamicAddress(hi3c->Instance);
      }
    }
  }

  return status;
}

/**
  * @brief  Target send hot join request in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_HotJoinReq_IT(I3C_HandleTypeDef *hi3c)
{
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* Check on the hot join request feature */
    else if (LL_I3C_IsEnabledHotJoin(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Update handle parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;
      hi3c->XferISR   = I3C_Tgt_HotJoin_ISR;

      /* Enable dynamic address update and error interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_HOTJOIN);

      /* Request hot join */
      LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_HOT_JOIN, 0U);
    }
  }

  return status;
}

/**
  * @brief  Target send IBI request in polling mode.
  * @param  hi3c        : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                             information for the specified I3C.
  * @param  pPayload    : [IN]  Pointer to the buffer contains the payload data.
  * @param  payloadSize : [IN]  Payload buffer size in bytes.
  * @param  timeout     : [IN]  Timeout duration in millisecond.
  * @retval HAL Status  :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_IBIReq(I3C_HandleTypeDef *hi3c, const uint8_t *pPayload,
                                     uint8_t payloadSize, uint32_t timeout)
{
  uint32_t tickstart;
  uint32_t payload_value = 0U;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Verify if IBI request feature is enabled*/
      if (LL_I3C_IsEnabledIBI(hi3c->Instance) != 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* Update handle parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;

      /* Check on the IBI additional data */
      if (LL_I3C_GetDeviceIBIPayload(hi3c->Instance) == LL_I3C_IBI_ADDITIONAL_DATA)
      {
        /* Check on the pPayload and payloadSize values */
        if ((pPayload == NULL) || (payloadSize == 0U))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
          status = HAL_ERROR;

          /* Update handle state parameter */
          I3C_StateUpdate(hi3c);
        }
        else
        {
          /* For loop to calculate the payload value */
          for (uint32_t index = 0U; index < payloadSize; index++)
          {
            payload_value |= ((uint32_t)pPayload[index] << (index * 8U));
          }

          /* Load IBI payload data */
          LL_I3C_SetIBIPayload(hi3c->Instance, payload_value);
        }
      }

      if (status == HAL_OK)
      {
        /* Init tickstart for timeout management */
        tickstart = HAL_GetTick();

        /* Request IBI */
        LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_IBI, payloadSize);

        /* Wait IBI completion confirmation flag */
        status = I3C_WaitOnFlagUntilTimeout(hi3c, HAL_I3C_FLAG_IBIENDF, RESET, timeout, tickstart);

        if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_IBIENDF) == SET)
        {
          /* Clear IBI end process flag */
          LL_I3C_ClearFlag_IBIEND(hi3c->Instance);
        }

        /* Check on error flag value */
        if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
        {
          /* Clear error flag */
          LL_I3C_ClearFlag_ERR(hi3c->Instance);

          /* Update handle error code parameter */
          I3C_GetErrorSources(hi3c);

          /* Update handle state parameter to previous state */
          I3C_StateUpdate(hi3c);

          status = HAL_ERROR;
        }
        else
        {
          /* Update handle state parameter to previous state */
          I3C_StateUpdate(hi3c);
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Target send IBI request in interrupt mode.
  * @param  hi3c        : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                             information for the specified I3C.
  * @param  pPayload    : [IN]  Pointer to the buffer contains the payload data.
  * @param  payloadSize : [IN]  Payload buffer size in bytes.
  * @retval HAL Status  :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Tgt_IBIReq_IT(I3C_HandleTypeDef *hi3c, const uint8_t *pPayload, uint8_t payloadSize)
{
  uint32_t payload_value = 0U;
  HAL_I3C_StateTypeDef handle_state;
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the instance and the mode parameters */
    assert_param(IS_I3C_ALL_INSTANCE(hi3c->Instance));
    assert_param(IS_I3C_MODE(hi3c->Mode));

    /* Get I3C handle state */
    handle_state = hi3c->State;

    /* Check on the Mode */
    if (hi3c->Mode != HAL_I3C_MODE_TARGET)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    /* check on the State */
    else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
    {
      status = HAL_BUSY;
    }
    /* Verify the dynamic address validity */
    else if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) != 1U)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Verify if IBI request feature is enabled */
      if (LL_I3C_IsEnabledIBI(hi3c->Instance) != 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* Update handle parameters */
      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;
      hi3c->State     = HAL_I3C_STATE_BUSY;
      hi3c->XferISR   = I3C_Tgt_IBI_ISR;

      /* Check on the IBI additional data */
      if (LL_I3C_GetDeviceIBIPayload(hi3c->Instance) == LL_I3C_IBI_ADDITIONAL_DATA)
      {
        /* Check on the pPayload and payloadSize values */
        if ((pPayload == NULL) || (payloadSize == 0U))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
          status = HAL_ERROR;

          /* Update handle state parameter */
          I3C_StateUpdate(hi3c);
        }
        else
        {
          /* For loop to calculate the payload value */
          for (uint32_t index = 0U; index < payloadSize; index++)
          {
            payload_value |= ((uint32_t)pPayload[index] << (index * 8U));
          }

          /* Load IBI payload data */
          LL_I3C_SetIBIPayload(hi3c->Instance, payload_value);
        }
      }

      /* Enable IBI end and error interrupts */
      I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_IBI);

      /* Request IBI */
      LL_I3C_TargetHandleMessage(hi3c->Instance, LL_I3C_TARGET_MTYPE_IBI, payloadSize);
    }
  }

  return status;
}
/**
  * @}
  */

/** @defgroup I3C_Exported_Functions_Group7 Generic and Common functions.
  * @brief    I3C generic and common functions.
  *
@verbatim
 =======================================================================================================================
                                        ##### Generic and Common functions #####
 =======================================================================================================================
    [..]  This subsection provides a set of functions allowing to Abort transfer or to get in run-time the status
          of the peripheral.

         (+) Call the function HAL_I3C_Abort_IT() to abort the current transfer either in DMA or IT.
         (+) Call the function HAL_I3C_GetState() to get the I3C handle state.
         (+) Call the function HAL_I3C_GetMode() to get the I3C handle mode.
         (+) Call the function HAL_I3C_GetError() to get the error code.
         (+) Call the function HAL_I3C_Get_ENTDAA_Payload_Info() to get BCR, DCR and PID information after ENTDAA.

@endverbatim
  * @{
  */

/**
  * @brief  Abort an I3C IT or DMA process communication with Interrupt.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Abort_IT(I3C_HandleTypeDef *hi3c)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hi3c->State != HAL_I3C_STATE_ABORT)
    {
      /* Set State at HAL_I3C_STATE_ABORT */
      hi3c->State     = HAL_I3C_STATE_ABORT;

      /* Disable Error Interrupts */
      __HAL_I3C_DISABLE_IT(hi3c, HAL_I3C_IT_ERRIE);

      hi3c->XferISR   = I3C_Abort_ISR;

      /* Flush the different Fifos to generate an automatic stop mode link to underflow or overflow detection timeout */
      /* Flush the content of Tx Fifo */
      LL_I3C_RequestTxFIFOFlush(hi3c->Instance);

      /* Flush the content of Rx Fifo */
      LL_I3C_RequestRxFIFOFlush(hi3c->Instance);

      /* Check on the I3C mode: Control and status FIFOs available only with controller mode */
      if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
      {
        /* Flush the content of Control Fifo */
        LL_I3C_RequestControlFIFOFlush(hi3c->Instance);

        /* Flush the content of Status Fifo */
        LL_I3C_RequestStatusFIFOFlush(hi3c->Instance);
      }

      /* Disable all DMA Requests */
      LL_I3C_DisableDMAReq_Control(hi3c->Instance);
      LL_I3C_DisableDMAReq_RX(hi3c->Instance);
      LL_I3C_DisableDMAReq_TX(hi3c->Instance);
      LL_I3C_DisableDMAReq_Status(hi3c->Instance);

      if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process
        to avoid the risk of I3C interrupt handle execution before current
        process unlock */
        I3C_Enable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_CCC_IT);
      }
      else
      {
        /* Note : The I3C interrupts must be enabled after unlocking current process
        to avoid the risk of I3C interrupt handle execution before current
        process unlock */
        I3C_Enable_IRQ(hi3c, I3C_XFER_TARGET_RX_IT);
      }
    }
    else
    {
      return HAL_BUSY;
    }
  }

  return status;
}

/**
  * @brief  Return the I3C handle state.
  * @param  hi3c      : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                           information for the specified I3C.
  * @retval HAL State : [OUT] Value from HAL_I3C_StateTypeDef enumeration.
  */
HAL_I3C_StateTypeDef HAL_I3C_GetState(const I3C_HandleTypeDef *hi3c)
{
  return hi3c->State;
}

/**
  * @brief  Returns the I3C handle mode.
  * @param  hi3c     : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                          information for the specified I3C.
  * @retval HAL Mode : [OUT] Value from HAL_I3C_ModeTypeDef enumeration.
  */
HAL_I3C_ModeTypeDef HAL_I3C_GetMode(const I3C_HandleTypeDef *hi3c)
{
  return hi3c->Mode;
}

/**
  * @brief  Return the I3C error code.
  * @param  hi3c           : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                information for the specified I3C.
  * @retval I3C Error Code : [OUT] Value from @ref I3C_ERROR_CODE_DEFINITION.
  */
uint32_t HAL_I3C_GetError(const I3C_HandleTypeDef *hi3c)
{
  return hi3c->ErrorCode;
}

/**
  * @brief  Target/Controller Get Common Command Code Information updated after event.
  * @param  hi3c     : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                         for the specified I3C.
  * @param  notifyId : [IN] Parameter indicates which notification is signaled.
  *                         It can be a combination of value of @ref HAL_I3C_Notification_ID_definition.
  * @param  pCCCInfo : [IN/OUT] Pointer to an I3C_CCCInfoTypeDef structure that contains the CCC information
  *                             updated after CCC event.
  * @retval HAL Status : Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_GetCCCInfo(I3C_HandleTypeDef *hi3c,
                                     uint32_t notifyId,
                                     I3C_CCCInfoTypeDef *pCCCInfo)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check on user parameters */
    if (pCCCInfo == NULL)
    {
      /* Update handle error code parameter */
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    /* Check the I3C state */
    else if (hi3c->State == HAL_I3C_STATE_RESET)
    {
      /* Update handle error code parameter */
      hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
      status = HAL_ERROR;
    }
    else
    {
      /* Retrieve Target Dynamic Address value and Validity (target/controller) */
      if ((notifyId & EVENT_ID_DAU) == EVENT_ID_DAU)
      {
        pCCCInfo->DynamicAddrValid = LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance);
        pCCCInfo->DynamicAddr = LL_I3C_GetOwnDynamicAddress(hi3c->Instance);
      }

      /* Retrieve Maximum Write Data Length (target) */
      if ((notifyId & EVENT_ID_SETMWL) == EVENT_ID_SETMWL)
      {
        pCCCInfo->MaxWriteLength = LL_I3C_GetMaxWriteLength(hi3c->Instance);
      }

      /* Retrieve Maximum Read Data Length (target) */
      if ((notifyId & EVENT_ID_SETMRL) == EVENT_ID_SETMRL)
      {
        pCCCInfo->MaxReadLength = LL_I3C_GetMaxReadLength(hi3c->Instance);
      }

      /* Retrieve Reset Action/Level on received reset pattern (target) */
      if ((notifyId & EVENT_ID_RSTACT) == EVENT_ID_RSTACT)
      {
        pCCCInfo->ResetAction = LL_I3C_GetResetAction(hi3c->Instance);
      }

      /* Retrieve Activity State (target) */
      if ((notifyId & EVENT_ID_ENTASx) == EVENT_ID_ENTASx)
      {
        pCCCInfo->ActivityState = LL_I3C_GetActivityState(hi3c->Instance);
      }

      /* Retrieve Interrupt allowed status (target) */
      if ((notifyId & EVENT_ID_ENEC_DISEC) == EVENT_ID_ENEC_DISEC)
      {
        pCCCInfo->HotJoinAllowed = LL_I3C_IsEnabledHotJoin(hi3c->Instance);
        pCCCInfo->InBandAllowed = LL_I3C_IsEnabledIBI(hi3c->Instance);
        pCCCInfo->CtrlRoleAllowed = LL_I3C_IsEnabledControllerRoleReq(hi3c->Instance);
      }

      /* Retrieve In Band Interrupt information (controller) */
      if ((notifyId & EVENT_ID_IBI) == EVENT_ID_IBI)
      {
        pCCCInfo->IBICRTgtAddr = LL_I3C_GetIBITargetAddr(hi3c->Instance);
        pCCCInfo->IBITgtNbPayload = LL_I3C_GetNbIBIAddData(hi3c->Instance);
        pCCCInfo->IBITgtPayload = LL_I3C_GetIBIPayload(hi3c->Instance);
      }

      /* Retrieve Controller role request Interrupt information (controller) */
      if ((notifyId & EVENT_ID_CR) == EVENT_ID_CR)
      {
        pCCCInfo->IBICRTgtAddr = LL_I3C_GetIBITargetAddr(hi3c->Instance);
      }
    }
  }

  return status;
}

/**
  * @brief  Get BCR, DCR and PID information after ENTDAA.
  * @param  hi3c     : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                         for the specified I3C.
  * @param  ENTDAA_payload  :[IN] Payload received after ENTDAA
  * @param  pENTDAA_payload :[OUT] Pointer to an I3C_ENTDAAPayloadTypeDef structure that contains the BCR, DCR and PID
  *                          information.
  * @retval HAL Status : Value from HAL_StatusTypeDef enumeration.
  */
HAL_StatusTypeDef HAL_I3C_Get_ENTDAA_Payload_Info(I3C_HandleTypeDef *hi3c,
                                                  uint64_t ENTDAA_payload,
                                                  I3C_ENTDAAPayloadTypeDef *pENTDAA_payload)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t BCR;
  uint64_t PID;

  /* check on the handle */
  if (hi3c == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check on user parameters */
    if (pENTDAA_payload == NULL)
    {
      /* Update handle error code parameter */
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else
    {
      /* Get Bus Characterics */
      BCR = __HAL_I3C_GET_BCR(ENTDAA_payload);

      /* Retrieve BCR information */
      pENTDAA_payload->BCR.IBIPayload = __HAL_I3C_GET_IBI_PAYLOAD(BCR);
      pENTDAA_payload->BCR.IBIRequestCapable = __HAL_I3C_GET_IBI_CAPABLE(BCR);
      pENTDAA_payload->BCR.DeviceRole = __HAL_I3C_GET_CR_CAPABLE(BCR);
      pENTDAA_payload->BCR.AdvancedCapabilities = I3C_GET_ADVANCED_CAPABLE(BCR);
      pENTDAA_payload->BCR.OfflineCapable = I3C_GET_OFFLINE_CAPABLE(BCR);
      pENTDAA_payload->BCR.VirtualTargetSupport = I3C_GET_VIRTUAL_TGT(BCR);
      pENTDAA_payload->BCR.MaxDataSpeedLimitation = I3C_GET_MAX_DATA_SPEED_LIMIT(BCR);

      /* Get Device Characterics */
      pENTDAA_payload->DCR = I3C_GET_DCR(ENTDAA_payload);

      /* Get Provisioned ID */
      PID = I3C_GET_PID(ENTDAA_payload);

      /* Change PID from BigEndian to litlleEndian */
      PID = (uint64_t)((((uint64_t)I3C_BIG_TO_LITTLE_ENDIAN((uint32_t) PID) << 32)   |
                        ((uint64_t)I3C_BIG_TO_LITTLE_ENDIAN((uint32_t)(PID >> 32)))) >> 16);

      /* Retrieve PID information*/
      pENTDAA_payload->PID.MIPIMID = I3C_GET_MIPIMID(PID);
      pENTDAA_payload->PID.IDTSEL = I3C_GET_IDTSEL(PID);
      pENTDAA_payload->PID.PartID = I3C_GET_PART_ID(PID);
      pENTDAA_payload->PID.MIPIID = I3C_GET_MIPIID(PID);
    }
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_Private_Functions I3C Private Functions
  * @{
  */

/**
  * @brief  Interrupt Sub-Routine which handles target received events.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  uint32_t tmpevent = 0U;

  /* I3C Rx FIFO not empty interrupt Check */
  if (I3C_CHECK_FLAG(itMasks, HAL_I3C_FLAG_RXFNEF) != RESET)
  {
    /* Call receive treatment function */
    hi3c->ptrRxFunc(hi3c);
  }

  /* I3C target complete controller-role hand-off procedure (direct GETACCR CCC) event management --------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRUPDF) != RESET)
  {
    /* Clear controller-role update flag */
    LL_I3C_ClearFlag_CRUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_GETACCCR */
    tmpevent |= EVENT_ID_GETACCCR;
  }

  /* I3C target receive any direct GETxxx CCC event management -------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_GETF) != RESET)
  {
    /* Clear GETxxx CCC flag */
    LL_I3C_ClearFlag_GET(hi3c->Instance);

    /* Set Identifier EVENT_ID_GETx */
    tmpevent |= EVENT_ID_GETx;
  }

  /* I3C target receive get status command (direct GETSTATUS CCC) event management -----------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_STAF) != RESET)
  {
    /* Clear GETSTATUS CCC flag */
    LL_I3C_ClearFlag_STA(hi3c->Instance);

    /* Set Identifier EVENT_ID_GETSTATUS */
    tmpevent |= EVENT_ID_GETSTATUS;
  }

  /* I3C target receive a dynamic address update (ENTDAA/RSTDAA/SETNEWDA CCC) event management -----------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_DAUPDF) != RESET)
  {
    /* Clear dynamic address update flag */
    LL_I3C_ClearFlag_DAUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_DAU */
    tmpevent |= EVENT_ID_DAU;
  }

  /* I3C target receive maximum write length update (direct SETMWL CCC) event management -----------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_MWLUPDF) != RESET)
  {
    /* Clear SETMWL CCC flag */
    LL_I3C_ClearFlag_MWLUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_SETMWL */
    tmpevent |= EVENT_ID_SETMWL;
  }

  /* I3C target receive maximum read length update(direct SETMRL CCC) event management -------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_MRLUPDF) != RESET)
  {
    /* Clear SETMRL CCC flag */
    LL_I3C_ClearFlag_MRLUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_SETMRL */
    tmpevent |= EVENT_ID_SETMRL;
  }

  /* I3C target detect reset pattern (broadcast or direct RSTACT CCC) event management -------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_RSTF) != RESET)
  {
    /* Clear reset pattern flag */
    LL_I3C_ClearFlag_RST(hi3c->Instance);

    /* Set Identifier EVENT_ID_RSTACT */
    tmpevent |= EVENT_ID_RSTACT;
  }

  /* I3C target receive activity state update (direct or broadcast ENTASx) CCC event management ----------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_ASUPDF) != RESET)
  {
    /* Clear ENTASx CCC flag */
    LL_I3C_ClearFlag_ASUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_ENTASx */
    tmpevent |= EVENT_ID_ENTASx;
  }

  /* I3C target receive a direct or broadcast ENEC/DISEC CCC event management ----------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_INTUPDF) != RESET)
  {
    /* Clear ENEC/DISEC CCC flag */
    LL_I3C_ClearFlag_INTUPD(hi3c->Instance);

    /* Set Identifier EVENT_ID_ENEC_DISEC */
    tmpevent |= EVENT_ID_ENEC_DISEC;
  }

  /* I3C target receive a broadcast DEFTGTS CCC event management -----------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_DEFF) != RESET)
  {
    /* Clear DEFTGTS CCC flag */
    LL_I3C_ClearFlag_DEF(hi3c->Instance);

    /* Set Identifier EVENT_ID_DEFTGTS */
    tmpevent |= EVENT_ID_DEFTGTS;
  }

  /* I3C target receive a group addressing (broadcast DEFGRPA CCC) event management ----------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_GRPF) != RESET)
  {
    /* Clear DEFGRPA CCC flag */
    LL_I3C_ClearFlag_GRP(hi3c->Instance);

    /* Set Identifier EVENT_ID_DEFGRPA */
    tmpevent |= EVENT_ID_DEFGRPA;
  }

  /* I3C target wakeup event management ----------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_WKPF) != RESET)
  {
    /* Clear WKP flag */
    LL_I3C_ClearFlag_WKP(hi3c->Instance);

    /* Set Identifier EVENT_ID_WKP */
    tmpevent |= EVENT_ID_WKP;
  }

  if (tmpevent != 0U)
  {
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, tmpevent);
#else
    /* Asynchronous receive CCC event Callback */
    HAL_I3C_NotifyCallback(hi3c, tmpevent);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* Update handle state parameter */
  I3C_StateUpdate(hi3c);

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles Controller received events.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C controller receive IBI event management ---------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_IBIF) != RESET)
  {
    /* Clear IBI request flag */
    LL_I3C_ClearFlag_IBI(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_IBI);
#else
    /* Asynchronous IBI event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_IBI);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller controller-role request event management ---------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRF) != RESET)
  {
    /* Clear controller-role request flag */
    LL_I3C_ClearFlag_CR(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_CR);
#else
    /* Asynchronous controller-role event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_CR);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller hot-join event management ------------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_HJF) != RESET)
  {
    /* Clear hot-join flag */
    LL_I3C_ClearFlag_HJ(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_HJ);
#else
    /* Asynchronous hot-join event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_HJ);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* Update handle state parameter */
  I3C_StateUpdate(hi3c);

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target hot join event.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_HotJoin_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C target receive a dynamic address update event management */
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_DAUPDF) != RESET)
  {
    /* Clear dynamic address update flag */
    LL_I3C_ClearFlag_DAUPD(hi3c->Instance);

    /* Disable dynamic address update and error interrupts */
    I3C_Disable_IRQ(hi3c, I3C_XFER_TARGET_HOTJOIN);

    /* Check the validity of the own dynamic address */
    if (LL_I3C_IsEnabledOwnDynAddress(hi3c->Instance) == 1U)
    {
      /* Update handle state parameter */
      I3C_StateUpdate(hi3c);

      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Call registered callback */
      hi3c->TgtHotJoinCallback(hi3c, (uint8_t)LL_I3C_GetOwnDynamicAddress(hi3c->Instance));
#else
      /* Asynchronous receive ENTDAA/RSTDAA/SETNEWDA CCC event Callback */
      HAL_I3C_TgtHotJoinCallback(hi3c, (uint8_t)LL_I3C_GetOwnDynamicAddress(hi3c->Instance));
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
    else
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_DYNAMIC_ADDR;

      /* Call error treatment function */
      I3C_ErrorTreatment(hi3c);
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target control role event.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_CtrlRole_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C target complete controller-role hand-off procedure (direct GETACCR CCC) event management -------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRUPDF) != RESET)
  {
    /* Clear controller-role update flag */
    LL_I3C_ClearFlag_CRUPD(hi3c->Instance);

    /* Disable controller-role update and error interrupts */
    I3C_Disable_IRQ(hi3c, I3C_XFER_TARGET_CTRLROLE);

    /* Update handle state parameter */
    I3C_StateUpdate(hi3c);

    hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_GETACCCR);
#else
    /* Asynchronous receive GETACCR CCC event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_GETACCCR);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target IBI event.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_IBI_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C target IBI end process event management ---------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_IBIENDF) != RESET)
  {
    /* Clear IBI end flag */
    LL_I3C_ClearFlag_IBIEND(hi3c->Instance);

    /* Disable IBI end and error interrupts */
    I3C_Disable_IRQ(hi3c, I3C_XFER_TARGET_IBI);

    /* Update handle state parameter */
    I3C_StateUpdate(hi3c);

    hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_IBIEND);
#else
    /* Asynchronous IBI end event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_IBIEND);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target transmit data in Interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_Tx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Tx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX)
  {
    /* I3C Tx FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_TXFNFF) != RESET)
    {
      if (hi3c->TxXferCount > 0U)
      {
        /* Call transmit treatment function */
        hi3c->ptrTxFunc(hi3c);
      }
    }

    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Check if all data bytes are transmitted */
      if (LL_I3C_GetXferDataCount(hi3c->Instance) == hi3c->pXferData->TxBuf.Size)
      {
        /* Disable Tx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_TARGET_TX_IT);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the transmit complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->TgtTxCpltCallback(hi3c);
#else
        HAL_I3C_TgtTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

        /* Call error treatment function */
        I3C_ErrorTreatment(hi3c);
      }
    }

    /* I3C target wakeup event management ----------------------------------*/
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_WKPF) != RESET)
    {
      /* Clear WKP flag */
      LL_I3C_ClearFlag_WKP(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Call registered callback */
      hi3c->NotifyCallback(hi3c, EVENT_ID_WKP);
#else
      /* Asynchronous receive CCC event Callback */
      HAL_I3C_NotifyCallback(hi3c, EVENT_ID_WKP);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
  }

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target receive data in Interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_Rx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that an Rx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_RX)
  {
    /* I3C Rx FIFO not empty interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, HAL_I3C_FLAG_RXFNEF) != RESET)
    {
      if (hi3c->RxXferCount > 0U)
      {
        /* Call receive treatment function */
        hi3c->ptrRxFunc(hi3c);
      }
    }

    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Check if all data bytes are received */
      if (LL_I3C_GetXferDataCount(hi3c->Instance) == hi3c->pXferData->RxBuf.Size)
      {
        /* Disable Rx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_TARGET_RX_IT);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the receive complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->TgtRxCpltCallback(hi3c);
#else
        HAL_I3C_TgtRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

        /* Call error treatment function */
        I3C_ErrorTreatment(hi3c);
      }
    }

    /* I3C target wakeup event management ----------------------------------*/
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_WKPF) != RESET)
    {
      /* Clear WKP flag */
      LL_I3C_ClearFlag_WKP(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Call registered callback */
      hi3c->NotifyCallback(hi3c, EVENT_ID_WKP);
#else
      /* Asynchronous receive CCC event Callback */
      HAL_I3C_NotifyCallback(hi3c, EVENT_ID_WKP);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
  }

  return HAL_OK;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Interrupt Sub-Routine which handles target transmit data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_Tx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Tx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX)
  {
    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Check if all data bytes are transmitted */
      if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmatx) == 0U)
      {
        /* Disable Tx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Update the number of remaining data bytes */
        hi3c->TxXferCount = 0U;

        /* Call target transmit complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->TgtTxCpltCallback(hi3c);
#else
        HAL_I3C_TgtTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

        /* Call error treatment function */
        I3C_ErrorTreatment(hi3c);
      }
    }

    /* I3C target wakeup event management ----------------------------------*/
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_WKPF) != RESET)
    {
      /* Clear WKP flag */
      LL_I3C_ClearFlag_WKP(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Call registered callback */
      hi3c->NotifyCallback(hi3c, EVENT_ID_WKP);
#else
      /* Asynchronous receive CCC event Callback */
      HAL_I3C_NotifyCallback(hi3c, EVENT_ID_WKP);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
  }

  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles target receive data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Tgt_Rx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Rx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_RX)
  {
    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Check if all data bytes are received */
      if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmarx) == 0U)
      {
        /* Disable Rx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_DMA);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Update the number of remaining data bytes */
        hi3c->RxXferCount = 0U;

        /* Call target receive complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->TgtRxCpltCallback(hi3c);
#else
        HAL_I3C_TgtRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

        /* Call error treatment function */
        I3C_ErrorTreatment(hi3c);
      }
    }

    /* I3C target wakeup event management ----------------------------------*/
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_WKPF) != RESET)
    {
      /* Clear WKP flag */
      LL_I3C_ClearFlag_WKP(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      /* Call registered callback */
      hi3c->NotifyCallback(hi3c, EVENT_ID_WKP);
#else
      /* Asynchronous receive CCC event Callback */
      HAL_I3C_NotifyCallback(hi3c, EVENT_ID_WKP);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
  }

  return HAL_OK;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Interrupt Sub-Routine which handles controller transmission in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Tx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Tx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX)
  {
    /* Check if Control FIFO requests data */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CFNFF) != RESET)
    {
      if (hi3c->ControlXferCount > 0U)
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);
      }
    }

    /* I3C Tx FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_TXFNFF) != RESET)
    {
      if (hi3c->TxXferCount > 0U)
      {
        /* Call Transmit treatment function */
        hi3c->ptrTxFunc(hi3c);
      }
    }

    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (hi3c->ControlXferCount == 0U)
      {
        /* Disable Tx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_TX_IT);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the transmit complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlTxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the transmit complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlTxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);

      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles controller reception in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Rx_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that an Rx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_RX)
  {
    /* Check if Control FIFO requests data */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CFNFF) != RESET)
    {
      if (hi3c->ControlXferCount > 0U)
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);
      }
    }

    /* I3C Rx FIFO not empty interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, HAL_I3C_FLAG_RXFNEF) != RESET)
    {
      if (hi3c->RxXferCount > 0U)
      {
        /* Call receive treatment function */
        hi3c->ptrRxFunc(hi3c);
      }
    }

    /* I3C Tx FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_TXFNFF) != RESET)
    {
      if (hi3c->TxXferCount > 0U)
      {
        /* Call Transmit treatment function */
        hi3c->ptrTxFunc(hi3c);
      }
    }

    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (hi3c->ControlXferCount == 0U)
      {
        /* Disable Rx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_CCC_IT);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the receive complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlRxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the receive complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlRxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles controller multiple transmission/reception in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Tx/Rx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX_RX)
  {
    /* Check if Control FIFO requests data */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CFNFF) != RESET)
    {
      if (hi3c->ControlXferCount > 0U)
      {
        /* Call control data treatment function */
        I3C_ControlDataTreatment(hi3c);
      }
    }

    /* I3C Tx FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_TXFNFF) != RESET)
    {
      if (hi3c->TxXferCount > 0U)
      {
        /* Call Transmit treatment function */
        hi3c->ptrTxFunc(hi3c);
      }
    }

    /* I3C Rx FIFO not empty interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, HAL_I3C_FLAG_RXFNEF) != RESET)
    {
      if (hi3c->RxXferCount > 0U)
      {
        /* Call receive treatment function */
        hi3c->ptrRxFunc(hi3c);
      }
    }

    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (hi3c->ControlXferCount == 0U)
      {
        /* Disable Tx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_TX_IT);

        /* Disable Rx process interrupts */
        I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_CCC_IT);

        /* Update handle state parameter */
        I3C_StateUpdate(hi3c);

        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the transmit, receive complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlMultipleXferCpltCallback(hi3c);
#else
        HAL_I3C_CtrlMultipleXferCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles controller transmission and Controller received events
  *         in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Tx_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C controller receive IBI event management ---------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_IBIF) != RESET)
  {
    /* Clear IBI request flag */
    LL_I3C_ClearFlag_IBI(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_IBI);
#else
    /* Asynchronous IBI event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_IBI);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller controller-role request event management ---------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRF) != RESET)
  {
    /* Clear controller-role request flag */
    LL_I3C_ClearFlag_CR(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_CR);
#else
    /* Asynchronous controller-role event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_CR);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller hot-join event management ------------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_HJF) != RESET)
  {
    /* Clear hot-join flag */
    LL_I3C_ClearFlag_HJ(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_HJ);
#else
    /* Asynchronous hot-join event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_HJ);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* ISR controller transmission */
  return (I3C_Ctrl_Tx_ISR(hi3c, itMasks));
}

/**
  * @brief  Interrupt Sub-Routine which handles controller reception and Controller received events in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Rx_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C controller receive IBI event management ---------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_IBIF) != RESET)
  {
    /* Clear IBI request flag */
    LL_I3C_ClearFlag_IBI(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_IBI);
#else
    /* Asynchronous IBI event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_IBI);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller controller-role request event management ---------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRF) != RESET)
  {
    /* Clear controller-role request flag */
    LL_I3C_ClearFlag_CR(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_CR);
#else
    /* Asynchronous controller-role event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_CR);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller hot-join event management ------------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_HJF) != RESET)
  {
    /* Clear hot-join flag */
    LL_I3C_ClearFlag_HJ(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_HJ);
#else
    /* Asynchronous hot-join event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_HJ);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* ISR controller reception */
  return (I3C_Ctrl_Rx_ISR(hi3c, itMasks));
}

/**
  * @brief  Interrupt Sub-Routine which handles controller multiple transmission/reception  and
  *         Controller received eventsin interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_Listen_Event_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* I3C controller receive IBI event management ---------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_IBIF) != RESET)
  {
    /* Clear IBI request flag */
    LL_I3C_ClearFlag_IBI(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_IBI);
#else
    /* Asynchronous IBI event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_IBI);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller controller-role request event management ---------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CRF) != RESET)
  {
    /* Clear controller-role request flag */
    LL_I3C_ClearFlag_CR(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_CR);
#else
    /* Asynchronous controller-role event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_CR);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* I3C controller hot-join event management ------------------------------------------------------------------------*/
  if (I3C_CHECK_FLAG(itMasks, I3C_EVR_HJF) != RESET)
  {
    /* Clear hot-join flag */
    LL_I3C_ClearFlag_HJ(hi3c->Instance);

#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
    /* Call registered callback */
    hi3c->NotifyCallback(hi3c, EVENT_ID_HJ);
#else
    /* Asynchronous hot-join event Callback */
    HAL_I3C_NotifyCallback(hi3c, EVENT_ID_HJ);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
  }

  /* ISR controller transmission/reception */
  return (I3C_Ctrl_Multiple_Xfer_ISR(hi3c, itMasks));
}
/**
  * @brief  Interrupt Sub-Routine which handles controller CCC Dynamic Address Assignment command in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_DAA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  uint64_t target_payload = 0U;

  /* Check that a Dynamic Address Assignment process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_DAA)
  {
    /* I3C Control FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_CFNFF) != RESET)
    {
      /* Write ENTDAA CCC information in the control register */
      LL_I3C_ControllerHandleCCC(hi3c->Instance, I3C_BROADCAST_ENTDAA, 0U, LL_I3C_GENERATE_STOP);
    }

    /* I3C Tx FIFO not full interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_TXFNFF) != RESET)
    {
      /* Check on the Rx FIFO threshold to know the Dynamic Address Assignment treatment process : byte or word */
      if (LL_I3C_GetRxFIFOThreshold(hi3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
      {
        /* For loop to get target payload */
        for (uint32_t index = 0U; index < 8U; index++)
        {
          /* Retrieve payload byte by byte */
          target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData8(hi3c->Instance) << (index * 8U));
        }
      }
      else
      {
        /* Retrieve first 32 bits payload */
        target_payload = (uint64_t)LL_I3C_ReceiveData32(hi3c->Instance);

        /* Retrieve second 32 bits payload */
        target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData32(hi3c->Instance) << 32U);
      }

      /* Call the corresponding callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      hi3c->TgtReqDynamicAddrCallback(hi3c, target_payload);
#else
      HAL_I3C_TgtReqDynamicAddrCallback(hi3c, target_payload);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */
    }

    /* I3C frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Disable Dynamic Address Assignment process interrupts */
      I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_DAA_IT);

      /* Update handle state parameter */
      I3C_StateUpdate(hi3c);

      hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

      /* Call the Dynamic Address Assignment complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
      hi3c->CtrlDAACpltCallback(hi3c);
#else
      HAL_I3C_CtrlDAACpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
    }
  }
  return HAL_OK;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Interrupt Sub-Routine which handles controller transmit data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Tx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that a Tx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX)
  {
    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmacr) == 0U)
      {
        /* Check if all data bytes are transmitted */
        if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmatx) == 0U)
        {
          /* Disable Tx process interrupts */
          I3C_Disable_IRQ(hi3c, I3C_XFER_DMA);

          /* Update handle state parameter */
          I3C_StateUpdate(hi3c);

          hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

          /* Update the number of remaining data bytes */
          hi3c->TxXferCount = 0U;

          /* Call controller transmit complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
          hi3c->CtrlTxCpltCallback(hi3c);
#else
          HAL_I3C_CtrlTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
        }
        else
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

          /* Call error treatment function */
          I3C_ErrorTreatment(hi3c);
        }
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the transmit complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlTxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlTxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles controller receive data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Rx_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that an Rx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_RX)
  {
    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmacr) == 0U)
      {
        /* Check if all data bytes are received */
        if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmarx) == 0U)
        {
          /* Disable Rx process interrupts */
          I3C_Disable_IRQ(hi3c, I3C_XFER_DMA);

          /* Update handle state parameter */
          I3C_StateUpdate(hi3c);

          hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

          /* Update the number of remaining data bytes */
          hi3c->RxXferCount = 0U;

          /* Call controller receive complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
          hi3c->CtrlRxCpltCallback(hi3c);
#else
          HAL_I3C_CtrlRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
        }
        else
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

          /* Call error treatment function */
          I3C_ErrorTreatment(hi3c);
        }
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Call the receive complete callback */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
        hi3c->CtrlRxCpltCallback(hi3c);
#else
        HAL_I3C_CtrlRxCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Interrupt Sub-Routine which handles controller multiple receive and transmit data in DMA mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_Multiple_Xfer_DMA_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that an Rx or Tx process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_BUSY_TX_RX)
  {
    /* I3C target frame complete event Check */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmacr) == 0U)
      {
        /* Check if all data bytes are received or transmitted */
        if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmarx) == 0U)
        {
          if (I3C_GET_DMA_REMAIN_DATA(hi3c->hdmatx) == 0U)
          {
            /* Disable transfer Tx/Rx process interrupts */
            I3C_Disable_IRQ(hi3c, I3C_XFER_DMA);

            /* Update handle state parameter */
            I3C_StateUpdate(hi3c);

            hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

            /* Update the number of remaining data bytes */
            hi3c->RxXferCount = 0U;

            /* Update the number of remaining data bytes */
            hi3c->TxXferCount = 0U;

            /* Call controller transmit, receive complete callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1U)
            hi3c->CtrlMultipleXferCpltCallback(hi3c);
#else
            HAL_I3C_CtrlMultipleXferCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS == 1U */
          }
          else
          {
            hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

            /* Call error treatment function */
            I3C_ErrorTreatment(hi3c);
          }
        }
        else
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_SIZE;

          /* Call error treatment function */
          I3C_ErrorTreatment(hi3c);
        }
      }
      else
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NONE;

        /* Then Initiate a Start condition */
        LL_I3C_RequestTransfer(hi3c->Instance);
      }
    }
  }
  return HAL_OK;
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  Interrupt Sub-Routine which handles abort process in interrupt mode.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                            for the specified I3C.
  * @param  itMasks    : [IN]  Flag Interrupt Masks flags to handle.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Abort_ISR(struct __I3C_HandleTypeDef *hi3c, uint32_t itMasks)
{
  /* Check that an Abort process is ongoing */
  if (hi3c->State == HAL_I3C_STATE_ABORT)
  {
    /* I3C Rx FIFO not empty interrupt Check */
    if (I3C_CHECK_FLAG(itMasks, HAL_I3C_FLAG_RXFNEF) != RESET)
    {
      if (LL_I3C_IsActiveFlag_DOVR(hi3c->Instance) == 1U)
      {
        /* Flush remaining Rx data */
        LL_I3C_RequestRxFIFOFlush(hi3c->Instance);
      }
    }

    /* I3C Abort frame complete event Check */
    /* Evenif abort is called, the Frame completion can arrive if abort is requested at the end of the processus */
    /* Evenif completion occurs, treat this end of processus as abort completion process */
    if (I3C_CHECK_FLAG(itMasks, I3C_EVR_FCF) != RESET)
    {
      /* Clear frame complete flag */
      LL_I3C_ClearFlag_FC(hi3c->Instance);

      /* Call error treatment function */
      I3C_ErrorTreatment(hi3c);
    }
  }
  return HAL_OK;
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  DMA I3C control transmit process complete callback.
  * @param  hdma : [IN] Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                     for the specified DMA channel.
  * @retval None
  */
static void I3C_DMAControlTransmitCplt(DMA_HandleTypeDef *hdma)
{
  /* Get the address of the I3C handle : Derogation MISRAC2012-Rule-11.5 */
  I3C_HandleTypeDef *hi3c = (I3C_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  /* Disable control DMA Request */
  LL_I3C_DisableDMAReq_Control(hi3c->Instance);
}

/**
  * @brief  DMA I3C transmit data process complete callback.
  * @param  hdma : [IN] Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                     for the specified DMA channel.
  * @retval None
  */
static void I3C_DMADataTransmitCplt(DMA_HandleTypeDef *hdma)
{
  /* Get the address of the I3C handle : Derogation MISRAC2012-Rule-11.5 */
  I3C_HandleTypeDef *hi3c = (I3C_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  /* Disable Tx DMA Request */
  LL_I3C_DisableDMAReq_TX(hi3c->Instance);
}

/**
  * @brief  DMA I3C receive data process complete callback.
  * @param  hdma : [IN] Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                     for the specified DMA channel.
  * @retval None
  */
static void I3C_DMADataReceiveCplt(DMA_HandleTypeDef *hdma)
{
  /* Get the address of the I3C handle : Derogation MISRAC2012-Rule-11.5 */
  I3C_HandleTypeDef *hi3c = (I3C_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  /* Disable Rx DMA Request */
  LL_I3C_DisableDMAReq_RX(hi3c->Instance);
}

/**
  * @brief  DMA I3C communication error callback.
  * @param  hdma : [IN] Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                     for the specified DMA channel.
  * @retval None
  */
static void I3C_DMAError(DMA_HandleTypeDef *hdma)
{
  /* Just to solve MisraC error then to be removed */
  /* Derogation MISRAC2012-Rule-11.5 */
  I3C_HandleTypeDef *hi3c = (I3C_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  hi3c->ErrorCode |= HAL_I3C_ERROR_DMA;
}

/**
  * @brief DMA I3C communication abort callback to be called at end of DMA Abort procedure.
  * @param  hdma : [IN] Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                     for the specified DMA channel.
  * @retval None
  */
static void I3C_DMAAbort(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  I3C_HandleTypeDef *hi3c = (I3C_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  /* Reset Tx DMA AbortCpltCallback */
  if (hi3c->hdmatx != NULL)
  {
    hi3c->hdmatx->XferAbortCallback = NULL;
  }

  /* Reset Rx DMA AbortCpltCallback */
  if (hi3c->hdmarx != NULL)
  {
    hi3c->hdmarx->XferAbortCallback = NULL;
  }

  /* Reset control DMA AbortCpltCallback */
  if (hi3c->hdmacr != NULL)
  {
    hi3c->hdmacr->XferAbortCallback = NULL;
  }

  I3C_TreatErrorCallback(hi3c);
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief  This function handles I3C Communication Timeout.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @param  flag       : [IN]  Specifies the I3C flag to check.
  * @param  flagstatus : [IN]  The new Flag status (SET or RESET).
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @param  tickstart  : [IN]  Tick start value
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_WaitOnFlagUntilTimeout(I3C_HandleTypeDef *hi3c, uint32_t flag, FlagStatus flagstatus,
                                                    uint32_t timeout, uint32_t tickstart)
{
  HAL_StatusTypeDef status = HAL_OK;

  while ((__HAL_I3C_GET_FLAG(hi3c, flag) == flagstatus) && (status == HAL_OK))
  {
    /* Check for the Timeout */
    if (timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
      {
        if (__HAL_I3C_GET_FLAG(hi3c, flag) == flagstatus)
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_TIMEOUT;
          status = HAL_TIMEOUT;
        }
      }
    }

    /* Check if an error occurs during Flag waiting */
    if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
    {
      /* Clear error flag */
      LL_I3C_ClearFlag_ERR(hi3c->Instance);

      /* Update handle error code parameter */
      I3C_GetErrorSources(hi3c);

      status = HAL_ERROR;
    }
  }
  return status;
}

/**
  * @brief  This function handles I3C Dynamic Address Assignment timeout.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @param  timeout    : [IN]  Timeout duration in millisecond.
  * @param  tickstart  : [IN]  Tick start value
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_WaitOnDAAUntilTimeout(I3C_HandleTypeDef *hi3c, uint32_t timeout, uint32_t tickstart)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t active_flags = READ_REG(hi3c->Instance->EVR);

  while (((active_flags & (HAL_I3C_FLAG_FCF | HAL_I3C_FLAG_TXFNFF)) == 0U) && (status == HAL_OK))
  {
    /* Check for the Timeout */
    if (timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
      {
        if ((active_flags & (HAL_I3C_FLAG_FCF | HAL_I3C_FLAG_TXFNFF)) == 0U)
        {
          hi3c->ErrorCode |= HAL_I3C_ERROR_TIMEOUT;
          status = HAL_TIMEOUT;
        }
      }
    }

    /* Check if an error occurs during Flag waiting */
    if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_ERRF) == SET)
    {
      /* Clear error flag */
      LL_I3C_ClearFlag_ERR(hi3c->Instance);

      /* Update handle error code parameter */
      I3C_GetErrorSources(hi3c);

      status = HAL_ERROR;
    }

    /* Read active flags from EVR register */
    active_flags = READ_REG(hi3c->Instance->EVR);
  }
  return status;
}

/**
  * @brief  I3C transmit by byte.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_TransmitByteTreatment(I3C_HandleTypeDef *hi3c)
{
  /* Check TX FIFO not full flag */
  while ((__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_TXFNFF) == SET) && (hi3c->TxXferCount > 0U))
  {
    /* Write Tx buffer data to transmit register */
    LL_I3C_TransmitData8(hi3c->Instance, *hi3c->pXferData->TxBuf.pBuffer);

    /* Increment Buffer pointer */
    hi3c->pXferData->TxBuf.pBuffer++;

    /* Decrement remaining bytes counter */
    hi3c->TxXferCount--;
  }
}

/**
  * @brief  I3C transmit by word.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_TransmitWordTreatment(I3C_HandleTypeDef *hi3c)
{
  /* Check TX FIFO not full flag */
  while (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_TXFNFF) == SET)
  {
    /* Write Tx buffer data to transmit register */
    LL_I3C_TransmitData32(hi3c->Instance, *((uint32_t *)hi3c->pXferData->TxBuf.pBuffer));

    /* Increment Buffer pointer */
    hi3c->pXferData->TxBuf.pBuffer += sizeof(uint32_t);

    if (hi3c->TxXferCount < sizeof(uint32_t))
    {
      hi3c->TxXferCount = 0U;
    }
    else
    {
      /* Decrement remaining bytes counter */
      hi3c->TxXferCount -= sizeof(uint32_t);
    }
  }
}

/**
  * @brief  I3C receive by byte.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_ReceiveByteTreatment(I3C_HandleTypeDef *hi3c)
{
  /* Check RX FIFO not empty flag */
  while (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_RXFNEF) == SET)
  {
    /* Store received bytes in the Rx buffer */
    *hi3c->pXferData->RxBuf.pBuffer = LL_I3C_ReceiveData8(hi3c->Instance);

    /* Increment Buffer pointer */
    hi3c->pXferData->RxBuf.pBuffer++;

    /* Decrement remaining bytes counter */
    hi3c->RxXferCount--;
  }
}

/**
  * @brief  I3C receive by word.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_ReceiveWordTreatment(I3C_HandleTypeDef *hi3c)
{
  /* Check RX FIFO not empty flag */
  while (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_RXFNEF) == SET)
  {
    /* Store received bytes in the Rx buffer */
    *((uint32_t *)hi3c->pXferData->RxBuf.pBuffer) = LL_I3C_ReceiveData32(hi3c->Instance);

    /* Increment Buffer pointer */
    hi3c->pXferData->RxBuf.pBuffer += sizeof(uint32_t);

    if (hi3c->RxXferCount > sizeof(uint32_t))
    {
      /* Decrement remaining bytes counter */
      hi3c->RxXferCount -= sizeof(uint32_t);
    }
    else
    {
      /* Reset counter as last modulo word Rx data received */
      hi3c->RxXferCount = 0U;
    }
  }
}

/**
  * @brief  I3C Control data treatment.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_ControlDataTreatment(I3C_HandleTypeDef *hi3c)
{
  /* Check if Control FIFO requests data */
  if (__HAL_I3C_GET_FLAG(hi3c, HAL_I3C_FLAG_CFNFF) == SET)
  {
    /* Decrement remaining control buffer data counter */
    hi3c->ControlXferCount--;

    /* Write Control buffer data to control register */
    WRITE_REG(hi3c->Instance->CR, *hi3c->pXferData->CtrlBuf.pBuffer);

    /* Increment Buffer pointer */
    hi3c->pXferData->CtrlBuf.pBuffer++;
  }
}

/**
  * @brief  I3C state update.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_StateUpdate(I3C_HandleTypeDef *hi3c)
{
  /* Check on previous state */
  if (hi3c->PreviousState == HAL_I3C_STATE_LISTEN)
  {
    /* Set state to listen */
    hi3c->State = HAL_I3C_STATE_LISTEN;

    /* Check the I3C mode */
    if (hi3c->Mode == HAL_I3C_MODE_TARGET)
    {
      /* Store the target event treatment function */
      hi3c->XferISR = I3C_Tgt_Event_ISR;
    }
    else
    {
      /* Store the controller event treatment function */
      hi3c->XferISR = I3C_Ctrl_Event_ISR;
    }
  }
  else
  {
    /* Set state to ready */
    hi3c->State = HAL_I3C_STATE_READY;

    /* Reset XferISR */
    hi3c->XferISR = NULL;
  }
}

/**
  * @brief  I3C get error source.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_GetErrorSources(I3C_HandleTypeDef *hi3c)
{
  /* Check on the I3C mode */
  switch (hi3c->Mode)
  {
    case HAL_I3C_MODE_CONTROLLER:
    {
      /* I3C data error during controller-role hand-off procedure */
      if (LL_I3C_IsActiveFlag_DERR(hi3c->Instance) == 1U)
      {
        hi3c->ErrorCode |= HAL_I3C_ERROR_DATA_HAND_OFF;
      }

      /* I3C data not acknowledged error */
      if (LL_I3C_IsActiveFlag_DNACK(hi3c->Instance) == 1U)
      {
        hi3c->ErrorCode |= HAL_I3C_ERROR_DATA_NACK;
      }

      /* I3C address not acknowledged error */
      if (LL_I3C_IsActiveFlag_ANACK(hi3c->Instance) == 1U)
      {
        hi3c->ErrorCode |= HAL_I3C_ERROR_ADDRESS_NACK;
      }

      /* I3C Status FIFO Over-Run or Control FIFO Under-Run error */
      if (LL_I3C_IsActiveFlag_COVR(hi3c->Instance) == 1U)
      {
        hi3c->ErrorCode |= HAL_I3C_ERROR_COVR;
      }

      break;
    }

    case HAL_I3C_MODE_TARGET:
    {
      /* I3C SCL stall error */
      if (LL_I3C_IsActiveFlag_STALL(hi3c->Instance) == 1U)
      {
        hi3c->ErrorCode |= HAL_I3C_ERROR_STALL;
      }

      break;
    }

    default:
    {
      break;
    }
  }

  /* I3C Rx FIFO Over-Run or Tx FIFO Under-Run error */
  if (LL_I3C_IsActiveFlag_DOVR(hi3c->Instance) == 1U)
  {
    hi3c->ErrorCode |= HAL_I3C_ERROR_DOVR;
  }

  /* I3C Protocol error */
  if (LL_I3C_IsActiveFlag_PERR(hi3c->Instance) == 1U)
  {
    hi3c->ErrorCode |= (I3C_SER_PERR | LL_I3C_GetMessageErrorCode(hi3c->Instance));
  }
}

/**
  * @brief  I3C transfer prior preparation.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @param  counter    : [IN]  Number of devices or commands to treat.
  * @param  option     : [IN]  Parameter indicates the transfer option.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Xfer_PriorPreparation(I3C_HandleTypeDef *hi3c, uint8_t counter, uint32_t option)
{
  HAL_StatusTypeDef status  = HAL_OK;
  uint32_t current_tx_index = 0U;
  uint32_t global_tx_size   = 0U;
  uint32_t global_rx_size   = 0U;
  uint32_t nb_tx_frame      = 0U;
  uint32_t direction;

  for (uint32_t descr_index = 0U; descr_index < counter; descr_index++)
  {
    /* Direct CCC command */
    if ((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_DIRECT)
    {
      /* Update direction of frame */
      direction = hi3c->pCCCDesc[descr_index].Direction;

      /* Direction read with Define byte */
      if (((option & I3C_DEFINE_BYTE_MASK) != 0U) && (direction == HAL_I3C_DIRECTION_READ))
      {
        nb_tx_frame += 1U;

        global_tx_size += 1U;

        global_rx_size += hi3c->pCCCDesc[descr_index].CCCBuf.Size - 1U;

        /* Check on the global size and on the Tx buffer pointer */
        if ((global_tx_size > hi3c->pXferData->TxBuf.Size)    || \
            (current_tx_index > hi3c->pXferData->TxBuf.Size)  || \
            (hi3c->pXferData->TxBuf.pBuffer == NULL))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;

          status = HAL_ERROR;
        }
        else
        {
          /* Fill global Tx buffer with data and update the current index of the Tx buffer */
          current_tx_index = I3C_FillTxBuffer_CCC(hi3c, descr_index, 1U, current_tx_index);
        }
      }
      else if (direction == HAL_I3C_DIRECTION_WRITE)
      {
        nb_tx_frame += 1U;

        global_tx_size += hi3c->pCCCDesc[descr_index].CCCBuf.Size;

        /* Check on the global size and on the Tx buffer pointer */
        if ((global_tx_size > hi3c->pXferData->TxBuf.Size)    || \
            (current_tx_index > hi3c->pXferData->TxBuf.Size)  || \
            (hi3c->pXferData->TxBuf.pBuffer == NULL))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;

          status = HAL_ERROR;
        }
        else
        {
          /* Fill global Tx buffer with data and update the current index of the Tx buffer */
          current_tx_index = I3C_FillTxBuffer_CCC(hi3c,
                                                  descr_index,
                                                  hi3c->pCCCDesc[descr_index].CCCBuf.Size,
                                                  current_tx_index);
        }
      }
      /* Direction read without Define byte */
      else
      {
        global_rx_size += hi3c->pCCCDesc[descr_index].CCCBuf.Size;
      }
    }
    /* Broadcast CCC command */
    else if ((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_CCC)
    {
      /* Update direction of frame */
      direction = hi3c->pCCCDesc[descr_index].Direction;

      if (direction == HAL_I3C_DIRECTION_WRITE)
      {
        nb_tx_frame += 1U;

        global_tx_size += hi3c->pCCCDesc[descr_index].CCCBuf.Size;

        /* Check on the global size and on the Tx buffer pointer */
        if ((global_tx_size > hi3c->pXferData->TxBuf.Size)    || \
            (current_tx_index > hi3c->pXferData->TxBuf.Size)  || \
            (hi3c->pXferData->TxBuf.pBuffer == NULL))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;

          status = HAL_ERROR;
        }
        else
        {
          /* Fill global Tx buffer with data and update the current index of the Tx buffer */
          current_tx_index = I3C_FillTxBuffer_CCC(hi3c,
                                                  descr_index,
                                                  hi3c->pCCCDesc[descr_index].CCCBuf.Size,
                                                  current_tx_index);
        }
      }
      else
      {
        status = HAL_ERROR;
      }
    }
    /* Private */
    else
    {
      /* Update direction of frame */
      direction = hi3c->pPrivateDesc[descr_index].Direction;

      if (direction == HAL_I3C_DIRECTION_WRITE)
      {
        nb_tx_frame += 1U;

        global_tx_size += hi3c->pPrivateDesc[descr_index].TxBuf.Size;

        /* Check on the global size and on the Tx buffer pointer */
        if ((global_tx_size > hi3c->pXferData->TxBuf.Size)    || \
            (current_tx_index > hi3c->pXferData->TxBuf.Size)  || \
            (hi3c->pXferData->TxBuf.pBuffer == NULL))
        {
          hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;

          status = HAL_ERROR;
        }
        else
        {
          /* Fill global Tx buffer with data and update the current index of the Tx buffer */
          current_tx_index = I3C_FillTxBuffer_Private(hi3c,
                                                      descr_index,
                                                      hi3c->pPrivateDesc[descr_index].TxBuf.Size,
                                                      current_tx_index);
        }
      }
      else
      {
        global_rx_size += hi3c->pPrivateDesc[descr_index].RxBuf.Size;
      }
    }

    /* Check if there is an error in the Tx Buffer*/
    if (status == HAL_ERROR)
    {
      break;
    }
  }

  if (status == HAL_OK)
  {
    /* Check on the Tx threshold and the number of Tx frame */
    if (LL_I3C_GetTxFIFOThreshold(hi3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_4_4)
    {
      /* LL_I3C_TXFIFO_THRESHOLD_4_4 is not allowed when the transfer descriptor contains
         multiple transmission frames */
      if (nb_tx_frame > 1U)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Check on the size Rx buffer */
    if (global_rx_size > hi3c->pXferData->RxBuf.Size)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
      status = HAL_ERROR;
    }
    else
    {
      hi3c->RxXferCount = global_rx_size;
    }

    /* Set handle transfer parameters */
    hi3c->TxXferCount = global_tx_size;
  }

  return status;
}

/**
  * @brief  I3C fill Tx Buffer with data from CCC Descriptor.
  * @param  hi3c           : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                information for the specified I3C.
  * @param  indexDesc      : [IN]  Index of descriptor.
  * @param  txSize         : [IN]  Size of Tx data.
  * @param  txCurrentIndex : [IN]  Current Index of TxBuffer.
  * @retval index_tx       : [OUT] New current Index of TxBuffer.
  */
static uint32_t I3C_FillTxBuffer_CCC(I3C_HandleTypeDef *hi3c,
                                     uint32_t           indexDesc,
                                     uint32_t           txSize,
                                     uint32_t           txCurrentIndex)
{
  uint32_t index_tx = txCurrentIndex;

  for (uint32_t index = 0U; index < txSize; index++)
  {
    hi3c->pXferData->TxBuf.pBuffer[index_tx] = hi3c->pCCCDesc[indexDesc].CCCBuf.pBuffer[index];

    index_tx++;
  }

  return index_tx;
}

/**
  * @brief  I3C fill Tx Buffer with data from Private Descriptor.
  * @param  hi3c           : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                information for the specified I3C.
  * @param  indexDesc      : [IN]  Index of descriptor.
  * @param  txSize         : [IN]  Size of Tx data.
  * @param  txCurrentIndex : [IN]  Current Index of TxBuffer.
  * @retval index_tx       : [OUT] New current Index of TxBuffer.
  */
static uint32_t I3C_FillTxBuffer_Private(I3C_HandleTypeDef *hi3c,
                                         uint32_t           indexDesc,
                                         uint32_t           txSize,
                                         uint32_t           txCurrentIndex)
{
  uint32_t index_tx = txCurrentIndex;

  for (uint32_t index = 0U; index < txSize; index++)
  {
    hi3c->pXferData->TxBuf.pBuffer[index_tx] = hi3c->pPrivateDesc[indexDesc].TxBuf.pBuffer[index];

    index_tx++;
  }

  return index_tx;
}

/**
  * @brief  I3C Control buffer prior preparation.
  * @param  hi3c       : [IN]  Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                            information for the specified I3C.
  * @param  counter    : [IN]  Number of devices or commands to treat.
  * @param  option     : [IN]  Parameter indicates the transfer option.
  * @retval HAL Status :       Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_ControlBuffer_PriorPreparation(I3C_HandleTypeDef *hi3c,
                                                            uint8_t            counter,
                                                            uint32_t           option)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t nb_define_bytes;
  uint32_t stop_condition;
  uint32_t nb_data_bytes;
  uint32_t index;

  /* Check on the control buffer pointer */
  if (hi3c->pXferData->CtrlBuf.pBuffer == NULL)
  {
    hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Extract from option required information */
    nb_define_bytes = (option & I3C_DEFINE_BYTE_MASK);
    stop_condition  = (option & I3C_RESTART_STOP_MASK);

    /* Check on the deactivation of the arbitration */
    if ((option & I3C_ARBITRATION_HEADER_MASK) == I3C_ARBITRATION_HEADER_MASK)
    {
      /* Disable arbitration header */
      LL_I3C_DisableArbitrationHeader(hi3c->Instance);
    }
    else
    {
      /* Enable arbitration header */
      LL_I3C_EnableArbitrationHeader(hi3c->Instance);
    }

    /* Check on the operation type */
    if ((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_CCC)
    {
      /*------------------------------------------ Broadcast CCC -----------------------------------------------------*/
      /* Check on the control buffer size */
      if (hi3c->pXferData->CtrlBuf.Size < (uint32_t)counter)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
        status = HAL_ERROR;
      }
      else
      {
        /* Set remaining control buffer data counter */
        hi3c->ControlXferCount = (uint32_t)counter;

        /* For loop on the number of commands */
        for (index = 0U; index < ((uint32_t)counter - 1U); index++)
        {
          /* Update control buffer value */
          hi3c->pXferData->CtrlBuf.pBuffer[index] = ((uint32_t)hi3c->pCCCDesc[index].CCCBuf.Size              |
                                                     ((uint32_t)hi3c->pCCCDesc[index].CCC  << I3C_CR_CCC_Pos) |
                                                     LL_I3C_CONTROLLER_MTYPE_CCC | stop_condition);
        }

        /* At the last device we should generate a stop condition */
        hi3c->pXferData->CtrlBuf.pBuffer[index] = ((uint32_t)hi3c->pCCCDesc[index].CCCBuf.Size              |
                                                   ((uint32_t)hi3c->pCCCDesc[index].CCC  << I3C_CR_CCC_Pos) |
                                                   LL_I3C_CONTROLLER_MTYPE_CCC | LL_I3C_GENERATE_STOP);
      }
    }
    else if ((option & I3C_OPERATION_TYPE_MASK) == LL_I3C_CONTROLLER_MTYPE_DIRECT)
    {
      /*------------------------------------------ Direct CCC --------------------------------------------------------*/
      /* Check on the control buffer size */
      if (hi3c->pXferData->CtrlBuf.Size < ((uint32_t)counter * 2U))
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
        status = HAL_ERROR;
      }
      else
      {
        /* Set remaining control buffer data counter */
        hi3c->ControlXferCount = ((uint32_t)counter * 2U);

        /* For loop on the number of (devices or commands) * 2 */
        for (index = 0U; index < (((uint32_t)counter * 2U) - 2U); index += 2U)
        {
          /* Step 1 : update control buffer value for the CCC command */
          hi3c->pXferData->CtrlBuf.pBuffer[index] = (nb_define_bytes                                               |
                                                     ((uint32_t)hi3c->pCCCDesc[index / 2U].CCC  << I3C_CR_CCC_Pos) |
                                                     LL_I3C_CONTROLLER_MTYPE_CCC | LL_I3C_GENERATE_RESTART);

          /* Step 2 : update control buffer value for target address */
          hi3c->pXferData->CtrlBuf.pBuffer[index + 1U] =
            (((uint32_t)hi3c->pCCCDesc[index / 2U].CCCBuf.Size - nb_define_bytes) |
             hi3c->pCCCDesc[index / 2U].Direction                                            |
             ((uint32_t)hi3c->pCCCDesc[index / 2U].TargetAddr << I3C_CR_ADD_Pos)  |
             LL_I3C_CONTROLLER_MTYPE_DIRECT | stop_condition);
        }

        /* Update control buffer value for the last CCC command */
        hi3c->pXferData->CtrlBuf.pBuffer[index] = (nb_define_bytes                                               |
                                                   ((uint32_t)hi3c->pCCCDesc[index / 2U].CCC  << I3C_CR_CCC_Pos) |
                                                   LL_I3C_CONTROLLER_MTYPE_CCC | LL_I3C_GENERATE_RESTART);

        /* At the last device we should generate a stop condition */
        hi3c->pXferData->CtrlBuf.pBuffer[index + 1U] =
          (((uint32_t)hi3c->pCCCDesc[index / 2U].CCCBuf.Size - nb_define_bytes) |
           hi3c->pCCCDesc[index / 2U].Direction                                            |
           ((uint32_t)hi3c->pCCCDesc[index / 2U].TargetAddr << I3C_CR_ADD_Pos)  |
           LL_I3C_CONTROLLER_MTYPE_DIRECT | LL_I3C_GENERATE_STOP);
      }
    }
    else
    {
      /*------------------------------------------ Private I3C/I2C ---------------------------------------------------*/
      /* Check on the control buffer size */
      if (hi3c->pXferData->CtrlBuf.Size < (uint32_t)counter)
      {
        hi3c->ErrorCode = HAL_I3C_ERROR_INVALID_PARAM;
        status = HAL_ERROR;
      }
      else
      {
        /* Set remaining control buffer data counter */
        hi3c->ControlXferCount = (uint32_t)counter;

        /* For loop on the number of devices */
        for (index = 0U; index < ((uint32_t)counter - 1U); index++)
        {
          /* Check on transfer direction */
          if (hi3c->pPrivateDesc[index].Direction == HAL_I3C_DIRECTION_READ)
          {
            nb_data_bytes = hi3c->pPrivateDesc[index].RxBuf.Size;
          }
          else
          {
            nb_data_bytes = hi3c->pPrivateDesc[index].TxBuf.Size;
          }

          /* Update control buffer value */
          hi3c->pXferData->CtrlBuf.pBuffer[index] =
            (nb_data_bytes | hi3c->pPrivateDesc[index].Direction                |
             ((uint32_t)hi3c->pPrivateDesc[index].TargetAddr << I3C_CR_ADD_Pos) |
             (option & I3C_OPERATION_TYPE_MASK) | stop_condition);
        }

        /* Check on transfer direction */
        if (hi3c->pPrivateDesc[index].Direction == HAL_I3C_DIRECTION_READ)
        {
          nb_data_bytes = hi3c->pPrivateDesc[index].RxBuf.Size;
        }
        else
        {
          nb_data_bytes = hi3c->pPrivateDesc[index].TxBuf.Size;
        }

        /* At the last device we should generate a stop condition */
        hi3c->pXferData->CtrlBuf.pBuffer[index] =
          (nb_data_bytes | hi3c->pPrivateDesc[index].Direction                |
           ((uint32_t)hi3c->pPrivateDesc[index].TargetAddr << I3C_CR_ADD_Pos) |
           (option & I3C_OPERATION_TYPE_MASK) | LL_I3C_GENERATE_STOP);
      }
    }
  }

  return status;
}

/**
  * @brief  Check if target device is ready for communication.
  * @param  hi3c    : Pointer to a I3C_HandleTypeDef structure that contains
  *         the configuration information for the specified I3C.
  * @param  pDevice : [IN] Structure to define the device address and the device type.
  * @param  trials  : [IN] Number of trials
  * @param  timeout : [IN] Timeout duration
  * @retval HAL Status :   Value from HAL_StatusTypeDef enumeration.
  */
static HAL_StatusTypeDef I3C_Ctrl_IsDevice_Ready(I3C_HandleTypeDef *hi3c,
                                                 const I3C_DeviceTypeDef *pDevice,
                                                 uint32_t           trials,
                                                 uint32_t           timeout)
{
  __IO uint32_t I3C_Trials = 0UL;
  __IO uint32_t exit_condition;
  uint32_t CR_tmp;
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  HAL_I3C_StateTypeDef handle_state;
  uint32_t arbitration_previous_state;

  /* Get I3C handle state */
  handle_state = hi3c->State;

  /* check on the Mode */
  if (hi3c->Mode != HAL_I3C_MODE_CONTROLLER)
  {
    hi3c->ErrorCode = HAL_I3C_ERROR_NOT_ALLOWED;
    status = HAL_ERROR;
  }
  /* check on the State */
  else if ((handle_state != HAL_I3C_STATE_READY) && (handle_state != HAL_I3C_STATE_LISTEN))
  {
    status = HAL_BUSY;
  }
  else
  {
    /* Set handle transfer parameters */
    hi3c->ErrorCode     = HAL_I3C_ERROR_NONE;
    hi3c->State         = HAL_I3C_STATE_BUSY;

    /* Before modify the arbitration, get the current arbitration state */
    arbitration_previous_state = LL_I3C_IsEnabledArbitrationHeader(hi3c->Instance);

    /* Disable arbitration header */
    LL_I3C_DisableArbitrationHeader(hi3c->Instance);

    CR_tmp = (HAL_I3C_DIRECTION_WRITE                                   |
              ((uint32_t)pDevice->Address << I3C_CR_ADD_Pos)      |
              pDevice->MessageType | LL_I3C_GENERATE_STOP);

    do
    {
      /* Initiate a start condition by writing in the CR register */
      WRITE_REG(hi3c->Instance->CR, CR_tmp);

      /* Calculate exit_condition value based on Frame complete and error flags */
      exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));

      tickstart = HAL_GetTick();

      while (exit_condition == 0U)
      {
        if (timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U))
          {
            /* Update I3C error code */
            hi3c->ErrorCode |= HAL_I3C_ERROR_TIMEOUT;
            status = HAL_TIMEOUT;

            break;
          }
        }
        /* Calculate exit_condition value based on Frame complete and error flags */
        exit_condition = (READ_REG(hi3c->Instance->EVR) & (I3C_EVR_FCF | I3C_EVR_ERRF));
      }

      if (status == HAL_OK)
      {
        /* Check if the FCF flag has been set */
        if (__HAL_I3C_GET_FLAG(hi3c, I3C_EVR_FCF) == SET)
        {
          /* Clear frame complete flag */
          LL_I3C_ClearFlag_FC(hi3c->Instance);

          /* Device is ready */
          break;
        }
        else
        {
          /* Clear ERR flag */
          LL_I3C_ClearFlag_ERR(hi3c->Instance);
        }
      }

      /* Increment Trials */
      I3C_Trials++;

    } while ((I3C_Trials < trials) && (status == HAL_OK));

    /* Device is not ready */
    if (trials == I3C_Trials)
    {
      hi3c->ErrorCode = HAL_I3C_ERROR_ADDRESS_NACK;
      status = HAL_ERROR;
    }

    /* update state to Previous state */
    I3C_StateUpdate(hi3c);

    /* Check if previous arbitration state is enabled */
    if (arbitration_previous_state == 1U)
    {
      LL_I3C_EnableArbitrationHeader(hi3c->Instance);
    }
  }

  return status;
}

/**
  * @brief  Manage the enabling of Interrupts.
  * @param  hi3c             : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                 information for the specified I3C.
  * @param  InterruptRequest : [IN] Value of the interrupt request
  * @retval None
  */
static void I3C_Enable_IRQ(I3C_HandleTypeDef *hi3c, uint32_t InterruptRequest)
{
  uint32_t tmpisr = 0U;

  /* Check if requested interrupts are related to listening mode */
  if ((InterruptRequest & I3C_XFER_LISTEN_IT) != 0U)
  {
    tmpisr |= ((InterruptRequest & (~I3C_XFER_LISTEN_IT)) | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target transmit in IT mode */
  if ((InterruptRequest & I3C_XFER_TARGET_TX_IT) != 0U)
  {
    /* Enable frame complete, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target receive in IT mode */
  if ((InterruptRequest & I3C_XFER_TARGET_RX_IT) != 0U)
  {
    /* Enable frame complete, receive FIFO not empty and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_RXFNEIE | HAL_I3C_IT_ERRIE) ;
  }

  /* Check if requested interrupts are related to transmit/receive in DMA mode */
  if ((InterruptRequest & I3C_XFER_DMA) != 0U)
  {
    /* Enable frame complete and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target hot join */
  if ((InterruptRequest & I3C_XFER_TARGET_HOTJOIN) != 0U)
  {
    /* Enable dynamic address update and error interrupts */
    tmpisr |= (HAL_I3C_IT_DAUPDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target control role */
  if ((InterruptRequest & I3C_XFER_TARGET_CTRLROLE) != 0U)
  {
    /* Enable control role update and error interrupts */
    tmpisr |= (HAL_I3C_IT_CRUPDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target in band interrupt */
  if ((InterruptRequest & I3C_XFER_TARGET_IBI) != 0U)
  {
    /* Enable IBI end and error interrupts */
    tmpisr |= (HAL_I3C_IT_IBIENDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_TX_IT) != 0U)
  {
    /* Enable frame complete, control FIFO not full, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller receive in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_RX_IT) != 0U)
  {
    /* Enable frame complete, control FIFO not full, receive FIFO not empty and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_RXFNEIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit read or a broadcast CCC in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_RX_CCC_IT) != 0U)
  {
    /* Enable frame complete, transmit FIFO not full, control FIFO not full,
       receive FIFO not empty and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_RXFNEIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit broadcast ENTDAA CCC in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_DAA_IT) != 0U)
  {
    /* Enable frame complete, control FIFO not full, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Enable requested interrupts */
  __HAL_I3C_ENABLE_IT(hi3c, tmpisr);
}

/**
  * @brief  Manage the disabling of Interrupts.
  * @param  hi3c             : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                                 information for the specified I3C.
  * @param  InterruptRequest : [IN] Value of the interrupt request
  * @retval None
  */
static void I3C_Disable_IRQ(I3C_HandleTypeDef *hi3c, uint32_t InterruptRequest)
{
  uint32_t tmpisr = 0U;

  /* Check if requested interrupts are related to listening mode */
  if ((InterruptRequest & I3C_XFER_LISTEN_IT) != 0U)
  {
    tmpisr |= ((InterruptRequest & (~I3C_XFER_LISTEN_IT)) | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target transmit mode */
  if ((InterruptRequest & I3C_XFER_TARGET_TX_IT) != 0U)
  {
    /* Disable frame complete, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target receive mode */
  if ((InterruptRequest & I3C_XFER_TARGET_RX_IT) != 0U)
  {
    /* Disable frame complete, receive FIFO not empty and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_RXFNEIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to transmit/receive in DMA mode */
  if ((InterruptRequest & I3C_XFER_DMA) != 0U)
  {
    /* Disable frame complete and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target hot join */
  if ((InterruptRequest & I3C_XFER_TARGET_HOTJOIN) != 0U)
  {
    /* Disable dynamic address update and error interrupts */
    tmpisr |= (HAL_I3C_IT_DAUPDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target control role */
  if ((InterruptRequest & I3C_XFER_TARGET_CTRLROLE) != 0U)
  {
    /* Disable control role update and error interrupts */
    tmpisr |= (HAL_I3C_IT_CRUPDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to target in band interrupt */
  if ((InterruptRequest & I3C_XFER_TARGET_IBI) != 0U)
  {
    /* Disable IBI end and error interrupts */
    tmpisr |= (HAL_I3C_IT_IBIENDIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_TX_IT) != 0U)
  {
    /* Disable frame complete, control FIFO not full, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit read or a broadcast CCC in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_RX_CCC_IT) != 0U)
  {
    /* Disable frame complete, transmit FIFO not full, control FIFO not full,
       receive FIFO not empty and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_RXFNEIE | HAL_I3C_IT_ERRIE);
  }

  /* Check if requested interrupts are related to controller transmit broadcast ENTDAA CCC in IT mode */
  if ((InterruptRequest & I3C_XFER_CONTROLLER_DAA_IT) != 0U)
  {
    /* Disable frame complete, control FIFO not full, transmit FIFO not full and error interrupts */
    tmpisr |= (HAL_I3C_IT_FCIE | HAL_I3C_IT_CFNFIE | HAL_I3C_IT_TXFNFIE | HAL_I3C_IT_ERRIE);
  }

  /* Disable requested interrupts */
  __HAL_I3C_DISABLE_IT(hi3c, tmpisr);
}

/**
  * @brief  I3C error treatment.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_ErrorTreatment(I3C_HandleTypeDef *hi3c)
{
  HAL_I3C_StateTypeDef tmpstate = hi3c->State;
  uint32_t dmaabortongoing = 0U;

  /* Check on the state */
  if (tmpstate == HAL_I3C_STATE_BUSY)
  {
    /* Update handle state parameter */
    I3C_StateUpdate(hi3c);

    /* Disable all interrupts related to busy state */
    I3C_Disable_IRQ(hi3c, (I3C_XFER_TARGET_IBI | I3C_XFER_TARGET_HOTJOIN | I3C_XFER_TARGET_CTRLROLE));
  }
  else
  {
    /* Disable all interrupts related to busy Tx and Rx state */
    I3C_Disable_IRQ(hi3c, I3C_XFER_CONTROLLER_RX_CCC_IT);

    /* Reset Tx counter */
    hi3c->TxXferCount = 0U;

    /* Reset Rx counter */
    hi3c->RxXferCount = 0U;

    /* Reset Control counter */
    hi3c->ControlXferCount = 0U;

    /* Reset Tx function pointer */
    hi3c->ptrTxFunc = NULL;

    /* Reset Rx function pointer */
    hi3c->ptrRxFunc = NULL;

    /* Reset Context pointer */
    hi3c->pXferData = NULL;
    hi3c->pCCCDesc = NULL;
    hi3c->pPrivateDesc = NULL;

    /* Flush all FIFOs */
    /* Flush the content of Tx Fifo */
    LL_I3C_RequestTxFIFOFlush(hi3c->Instance);

    /* Flush the content of Rx Fifo */
    LL_I3C_RequestRxFIFOFlush(hi3c->Instance);

    /* Check on the I3C mode: Control and status FIFOs available only with controller mode */
    if (hi3c->Mode == HAL_I3C_MODE_CONTROLLER)
    {
      /* Flush the content of Control Fifo */
      LL_I3C_RequestControlFIFOFlush(hi3c->Instance);

      /* Flush the content of Status Fifo */
      LL_I3C_RequestStatusFIFOFlush(hi3c->Instance);
    }

#if defined(HAL_DMA_MODULE_ENABLED)
    /* Abort control DMA transfer if any */
    if (hi3c->hdmacr != NULL)
    {
      /* Disable control DMA Request */
      LL_I3C_DisableDMAReq_Control(hi3c->Instance);

      /* Check DMA state */
      if (HAL_DMA_GetState(hi3c->hdmacr) != HAL_DMA_STATE_READY)
      {
        /* Set the I3C DMA Abort callback : will lead to call HAL_I3C_AbortCpltCallback()
           at end of DMA abort procedure */

        /* DMA abort on going */
        dmaabortongoing = 1U;

        /* Abort control DMA */
        if (HAL_DMA_Abort_IT(hi3c->hdmacr) != HAL_OK)
        {
          /* Call Directly XferAbortCallback function in case of error */
          hi3c->hdmacr->XferAbortCallback(hi3c->hdmacr);
        }
      }
    }

    /* Abort RX DMA transfer if any */
    if (hi3c->hdmarx != NULL)
    {
      /* Disable Rx DMA Request */
      LL_I3C_DisableDMAReq_RX(hi3c->Instance);

      /* Check DMA state */
      if (HAL_DMA_GetState(hi3c->hdmarx) != HAL_DMA_STATE_READY)
      {
        /* Set the I3C DMA Abort callback : will lead to call HAL_I3C_AbortCpltCallback()
           at end of DMA abort procedure */
        hi3c->hdmarx->XferAbortCallback = I3C_DMAAbort;

        /* DMA abort on going */
        dmaabortongoing = 1U;

        /* Abort DMA RX */
        if (HAL_DMA_Abort_IT(hi3c->hdmarx) != HAL_OK)
        {
          /* Call Directly XferAbortCallback function in case of error */
          hi3c->hdmarx->XferAbortCallback(hi3c->hdmarx);
        }
      }
    }

    /* Abort TX DMA transfer if any */
    if (hi3c->hdmatx != NULL)
    {
      /* Disable Tx DMA Request */
      LL_I3C_DisableDMAReq_TX(hi3c->Instance);

      /* Check DMA state */
      if (HAL_DMA_GetState(hi3c->hdmatx) != HAL_DMA_STATE_READY)
      {
        /* Set the I3C DMA Abort callback : will lead to call HAL_I3C_AbortCpltCallback()
           at end of DMA abort procedure */
        hi3c->hdmatx->XferAbortCallback = I3C_DMAAbort;

        /* DMA abort on going */
        dmaabortongoing = 1U;

        /* Abort DMA TX */
        if (HAL_DMA_Abort_IT(hi3c->hdmatx) != HAL_OK)
        {
          /* Call Directly XferAbortCallback function in case of error */
          hi3c->hdmatx->XferAbortCallback(hi3c->hdmatx);
        }
      }
    }
#endif /* HAL_DMA_MODULE_ENABLED */
  }

  /* Call Error callback if there is no DMA abort on going */
  if (dmaabortongoing == 0U)
  {
    I3C_TreatErrorCallback(hi3c);
  }
}

/**
  * @brief  I3C Error callback treatment.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration
  *                     information for the specified I3C.
  * @retval None
  */
static void I3C_TreatErrorCallback(I3C_HandleTypeDef *hi3c)
{
  if (hi3c->State == HAL_I3C_STATE_ABORT)
  {
    /* Update handle state parameter */
    I3C_StateUpdate(hi3c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1)
    hi3c->AbortCpltCallback(hi3c);
#else
    HAL_I3C_AbortCpltCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */
  }
  else
  {
    /* Update handle state parameter */
    I3C_StateUpdate(hi3c);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_I3C_REGISTER_CALLBACKS == 1)
    hi3c->ErrorCallback(hi3c);
#else
    HAL_I3C_ErrorCallback(hi3c);
#endif /* USE_HAL_I3C_REGISTER_CALLBACKS */
  }
}

/**
  * @}
  */

#endif /* HAL_I3C_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
