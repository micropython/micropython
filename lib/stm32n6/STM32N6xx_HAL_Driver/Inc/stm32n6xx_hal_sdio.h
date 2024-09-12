/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_sdio.h
  * @author  MCD Application Team
  * @brief   Header file of SDIO HAL module.
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
#ifndef STM32N6xx_HAL_SDIO_H
#define STM32N6xx_HAL_SDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_ll_sdmmc.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)

/** @defgroup SDIO SDIO
  * @brief SDIO HAL module driver
  * @{
  */

/* Exported types ----------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Exported_Types SDIO Exported Types
  * @{
  */

/** @defgroup SDIO_Exported_Types_Group1 SDIO State enumeration structure
  * @{

  */
typedef enum
{
  HAL_SDIO_STATE_RESET = 0x00U, /*!< SDIO not yet initialized or disabled */
  HAL_SDIO_STATE_READY = 0x01U, /*!< SDIO initialized and ready for us    */
  HAL_SDIO_STATE_BUSY  = 0x02U, /*!< SDIO process ongoing                 */
} HAL_SDIO_StateTypeDef;

/**
  * @}
  */

/** @defgroup SDIO_Exported_Types_Group2 SDIO Handle and Structure definition
  * @{
  */
/**
  * @brief  SDIO Card Common Control Register Structure definition
  */
typedef struct
{
  uint8_t sdio_revision;     /*!< SDIO revision                */
  uint8_t cccr_revision;     /*!< CCCR version                 */
  uint8_t sd_spec_revision;  /*!< SD revision                  */
  uint8_t bus_width_8Bit;    /*!< SDIO bus width 8 bit support */
  uint32_t card_capability;  /*!< SDIO card capability         */
  uint32_t commonCISPointer; /*!< point to common CIS          */
} HAL_SDIO_CCCR_TypeDef;

/**
  * @brief sdio card FBR register(Function Basic Register)
  */
typedef struct
{
  uint8_t flags;             /*!< SDIO current IO flags                  */
  uint8_t ioStdFunctionCode; /*!< SDIO current IO standard function code */
  uint8_t ioExtFunctionCode; /*!< SDIO current IO extended function code */
  uint32_t ioPointerToCIS;   /*!< SDIO current IO pointer to CIS         */
  uint32_t ioPointerToCSA;   /*!< SDIO current IO pointer to CSA         */
} HAL_SDIO_FBR_t;

/**
  * @brief  SDIO CMD52 Structure definition
  */
typedef struct
{
  uint32_t Reg_Addr;       /*!< This is the address of the byte of data inside of the selected function to read or write */
  uint8_t  ReadAfterWrite; /*!< This is the read after write flag, it is used for write access only.                     */
  uint8_t  IOFunctionNbr;  /*!< The number of the function within the IO card you wish to read or write                  */
} HAL_SDIO_DirectCmd_TypeDef;

/**
  * @brief  SDIO CMD53 Structure definition
  */
typedef struct
{
  uint32_t Reg_Addr;      /*!< This is the address of the byte of data inside of the selected function to read or write */
  uint32_t OpCode;        /*!< Read/Write operation mode                                                                */
  uint32_t Block_Mode;    /*!< Bytes or Blocks mode                                                                     */
  uint32_t IOFunctionNbr; /*!< The number of the function within the IO card you wish to read or write                  */
} HAL_SDIO_ExtendedCmd_TypeDef;

#define SDIO_InitTypeDef      SDMMC_InitTypeDef
#define SDIO_TypeDef          SDMMC_TypeDef

/**
  * @brief  SDIO handle Structure definition
  */
typedef struct __SDIO_HandleTypeDef
{
  SDIO_TypeDef                   *Instance;        /*!< SDIO registers base address                             */

  SDIO_InitTypeDef               Init;              /*!< SDIO required parameters                               */

  HAL_LockTypeDef                Lock;              /*!< SDIO locking object                                    */

  uint8_t                        *pTxBuffPtr;       /*!< Pointer to SDIO Tx transfer Buffer                     */

  uint32_t                       TxXferSize;        /*!< SDIO Tx Transfer size                                  */

  uint8_t                        *pRxBuffPtr;       /*!< Pointer to SDIO Rx transfer Buffer                     */

  uint32_t                       RxXferSize;        /*!< SDIO Rx Transfer size                                  */

  uint32_t                       remaining_data;    /*!< Remaining data to transfer                             */

  uint32_t                       next_data_addr;    /*!< SDIO Next data address                                 */

  __IO uint32_t                  next_reg_addr;     /*!< SDIO Next register address                             */

  uint16_t                       block_size;        /*!< SDIO Block size                                        */

  __IO uint32_t                  Context;           /*!< SDIO transfer context                                  */

  __IO HAL_SDIO_StateTypeDef     State;             /*!< SDIO card State                                        */

  __IO uint32_t                  ErrorCode;         /*!< SDIO Card Error codes                                  */

  uint8_t                        IOFunctionMask;    /*!< SDIO used to record current enabled io interrupt       */

  volatile uint8_t               IOInterruptNbr;    /*!< SDIO used to record total enabled io interrupt numbers */

  void (* SDIO_IOFunction_Callback[SDIO_MAX_IO_NUMBER])(struct __SDIO_HandleTypeDef *hsdio, uint32_t func);

#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
  void (* TxCpltCallback)(struct __SDIO_HandleTypeDef *hsdio);
  void (* RxCpltCallback)(struct __SDIO_HandleTypeDef *hsdio);
  void (* ErrorCallback)(struct __SDIO_HandleTypeDef *hsdio);
  void (* MspInitCallback)(struct __SDIO_HandleTypeDef *hsdio);
  void (* MspDeInitCallback)(struct __SDIO_HandleTypeDef *hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */

#if (USE_SDIO_TRANSCEIVER != 0U)
  void (* DriveTransceiver_1_8V_Callback)(struct __SDIO_HandleTypeDef *hsdio, FlagStatus status);
#endif /* USE_SDIO_TRANSCEIVER */

  HAL_StatusTypeDef(* SDIO_IdentifyCard)(struct __SDIO_HandleTypeDef *hsdio);

} SDIO_HandleTypeDef;

/**
  * @}
  */
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
/** @defgroup SDIO_Exported_Types_Group3 SDIO Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_SDIO_TX_CPLT_CB_ID    = 0x00U,  /*!< SDIO Tx Complete Callback ID */
  HAL_SDIO_RX_CPLT_CB_ID    = 0x01U,  /*!< SDIO Rx Complete Callback ID */
  HAL_SDIO_ERROR_CB_ID      = 0x02U,  /*!< SDIO Error Callback ID       */
  HAL_SDIO_MSP_INIT_CB_ID   = 0x10U,  /*!< SDIO MspInit Callback ID     */
  HAL_SDIO_MSP_DEINIT_CB_ID = 0x11U   /*!< SDIO MspDeInit Callback ID   */
} HAL_SDIO_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup SDIO_Exported_Types_Group4 SDIO Callback pointer definition
  * @{
  */
typedef void (*pSDIO_CallbackTypeDef)(SDIO_HandleTypeDef *hsdio);
/**
  * @}
  */
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */

#if (USE_SDIO_TRANSCEIVER != 0U)
typedef void (*pSDIO_TransceiverCallbackTypeDef)(SDIO_HandleTypeDef *hsdio, FlagStatus status);
#endif /* USE_SDIO_TRANSCEIVER */

typedef HAL_StatusTypeDef(*pSDIO_IdentifyCardCallbackTypeDef)(SDIO_HandleTypeDef *hsdio);
typedef void (*HAL_SDIO_IOFunction_CallbackTypeDef)(SDIO_HandleTypeDef *hsdio, uint32_t func);
/**
  * @}
  */

/* Exported constants ------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Exported_Constants SDIO Exported Constants
  * @{
  */

/** @defgroup SDIO_Exported_Constansts_Group1 SDIO Error status Structure definition
  * @{
  */
#define HAL_SDIO_ERROR_NONE             SDMMC_ERROR_NONE              /*!< No error                                    */
#define HAL_SDIO_ERROR_DATA_CRC_FAIL    SDMMC_ERROR_DATA_CRC_FAIL     /*!< Data block sent/received (CRC check failed) */
#define HAL_SDIO_ERROR_DATA_TIMEOUT     SDMMC_ERROR_DATA_TIMEOUT      /*!< Data timeout                                */
#define HAL_SDIO_ERROR_TX_UNDERRUN      SDMMC_ERROR_TX_UNDERRUN       /*!< Transmit FIFO underrun                      */
#define HAL_SDIO_ERROR_RX_OVERRUN       SDMMC_ERROR_RX_OVERRUN        /*!< Receive FIFO overrun                        */
#define HAL_SDIO_ERROR_TIMEOUT          SDMMC_ERROR_TIMEOUT           /*!< Timeout error                               */
#define HAL_SDIO_ERROR_INVALID_CALLBACK SDMMC_ERROR_INVALID_PARAMETER /*!< Invalid callback error                      */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group2 SDIO context enumeration
  * @{
  */
#define SDIO_CONTEXT_NONE                 0x00U /*!< None                             */
#define SDIO_CONTEXT_READ_SINGLE_BLOCK    0x01U /*!< Read single block operation      */
#define SDIO_CONTEXT_READ_MULTIPLE_BLOCK  0x02U /*!< Read multiple blocks operation   */
#define SDIO_CONTEXT_WRITE_SINGLE_BLOCK   0x10U /*!< Write single block operation     */
#define SDIO_CONTEXT_WRITE_MULTIPLE_BLOCK 0x20U /*!< Write multiple blocks operation  */
#define SDIO_CONTEXT_IT                   0x08U /*!< Process in Interrupt mode        */
#define SDIO_CONTEXT_DMA                  0x80U /*!< Process in DMA mode              */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group3 SDIO Block mode enumeration
  * @{
  */
#define HAL_SDIO_MODE_BYTE   SDMMC_SDIO_MODE_BYTE
#define HAL_SDIO_MODE_BLOCK  SDMMC_SDIO_MODE_BLOCK
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group4 SDIO operation code enumeration
  * @{
  */
#define HAL_SDIO_OP_CODE_NO_INC    SDMMC_SDIO_NO_INC
#define HAL_SDIO_OP_CODE_AUTO_INC  SDMMC_SDIO_AUTO_INC
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group5 SDIO Read After Write(RAW) enumeration
  * @{
  */
#define HAL_SDIO_WRITE_ONLY       SDMMC_SDIO_WO  /*!< SDIO Write only       */
#define HAL_SDIO_READ_AFTER_WRITE SDMMC_SDIO_RAW /*!< SDIO Read after write */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group6 SDIO wire mode enumeration
  * @{
  */
#define HAL_SDIO_1_WIRE_MODE   0U /*!< SDIO wire support 1 wire  */
#define HAL_SDIO_4_WIRES_MODE  1U /*!< SDIO wire support 4 wires */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group7 SDIO Data block size enumeration
  * @{
  */
#define HAL_SDIO_DATA_BLOCK_SIZE_1BYTE     1U    /*!< SDIO data block size 1 byte    */
#define HAL_SDIO_DATA_BLOCK_SIZE_2BYTE     2U    /*!< SDIO data block size 2 byte    */
#define HAL_SDIO_DATA_BLOCK_SIZE_4BYTE     4U    /*!< SDIO data block size 4 byte    */
#define HAL_SDIO_DATA_BLOCK_SIZE_8BYTE     8U    /*!< SDIO data block size 8 byte    */
#define HAL_SDIO_DATA_BLOCK_SIZE_16BYTE    16U   /*!< SDIO data block size 16 byte   */
#define HAL_SDIO_DATA_BLOCK_SIZE_32BYTE    32U   /*!< SDIO data block size 32 byte   */
#define HAL_SDIO_DATA_BLOCK_SIZE_64BYTE    64U   /*!< SDIO data block size 64 byte   */
#define HAL_SDIO_DATA_BLOCK_SIZE_128BYTE   128U  /*!< SDIO data block size 128 byte  */
#define HAL_SDIO_DATA_BLOCK_SIZE_256BYTE   256U  /*!< SDIO data block size 256 byte  */
#define HAL_SDIO_DATA_BLOCK_SIZE_512BYTE   512U  /*!< SDIO data block size 512 byte  */
#define HAL_SDIO_DATA_BLOCK_SIZE_1024BYTE  1024U /*!< SDIO data block size 1024 byte */
#define HAL_SDIO_DATA_BLOCK_SIZE_2048BYTE  2048U /*!< SDIO data block size 2048 byte */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group8 SDIO Bus Width enumeration
  * @{
  */
#define HAL_SDIO_BUS_WIDTH_8BIT_NOT_SUPPORTED 0U /*!< SDIO bus width 8 bit is not supported */
#define HAL_SDIO_BUS_WIDTH_8BIT_SUPPORTED     1U /*!< SDIO bus width 8 bit is supported     */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group9 SDIO Data rate definitions
  * @{
  */
#define HAL_SDIOS_DATA_RATE_SDR12  0U /*!< SDIO Data rate SDR12 */
#define HAL_SDIOS_DATA_RATE_SDR25  1U /*!< SDIO Data rate SDR25 */
#define HAL_SDIOS_DATA_RATE_SDR50  2U /*!< SDIO Data rate SDR50 */
#define HAL_SDIOS_DATA_RATE_DDR50  3U /*!< SDIO Data rate DDR50 */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group10 SDIO Functions definitions
  * @{
  */
#define HAL_SDIO_FUNCTION_0  0U /*!< SDIO function 0 */
#define HAL_SDIO_FUNCTION_1  1U /*!< SDIO function 1 */
#define HAL_SDIO_FUNCTION_2  2U /*!< SDIO function 2 */
#define HAL_SDIO_FUNCTION_3  3U /*!< SDIO function 3 */
#define HAL_SDIO_FUNCTION_4  4U /*!< SDIO function 4 */
#define HAL_SDIO_FUNCTION_5  5U /*!< SDIO function 5 */
#define HAL_SDIO_FUNCTION_6  6U /*!< SDIO function 6 */
#define HAL_SDIO_FUNCTION_7  7U /*!< SDIO function 7 */
/**
  * @}
  */

/** @defgroup SDIO_Exported_Constansts_Group11 SDIO FBR definitions
  * @{
  */
#define HAL_SDIO_FBR_SUPPORT_CSA             1U /*!< SDIO function support CSA             */
#define HAL_SDIO_FBR_SUPPORT_POWER_SELECTION 1U /*!< SDIO function support power selection */
/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Exported_macros SDIO Exported Macros
  *  @brief macros to handle interrupts and specific clock configurations
  * @{
  */
/**
  * @brief  Enable the SDIO device interrupt.
  * @param  __HANDLE__ SDIO Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt sources to be enabled.
  * @retval None
  */
#define __HAL_SDIO_ENABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_ENABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Disable the SDIO device interrupt.
  * @param  __HANDLE__ SDIO Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt sources to be disabled.
  * @retval None
  */
#define __HAL_SDIO_DISABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_DISABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Check whether the specified SDIO flag is set or not.
  * @param  __HANDLE__ SDIO Handle.
  * @param  __FLAG__ specifies the flag to check.
  * @retval The new state of SDIO FLAG (SET or RESET).
  */
#define __HAL_SDIO_GET_FLAG(__HANDLE__, __FLAG__) __SDMMC_GET_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Clear the SDIO's pending flags.
  * @param  __HANDLE__ SDIO Handle.
  * @param  __FLAG__ specifies the flag to clear.
  * @retval None
  */
#define __HAL_SDIO_CLEAR_FLAG(__HANDLE__, __FLAG__) __SDMMC_CLEAR_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Check whether the specified SDIO interrupt has occurred or not.
  * @param  __HANDLE__ SDIO Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt source to check.
  * @retval The new state of SDIO IT (SET or RESET).
  */
#define __HAL_SDIO_GET_IT(__HANDLE__, __INTERRUPT__) __SDMMC_GET_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Checks whether the specified SDIO interrupt is enabled or not.
  * @param  __HANDLE__    : SDIO handle.
  * @param  __INTERRUPT__ : specifies the SDMMC interrupt source to check.
  * @retval The state of SDIO IT (SET or RESET).
  */
#define __HAL_SDIO_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) \
  __SDMMC_GET_IT_SOURCE((__HANDLE__)->Instance, (__INTERRUPT__))
/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */
/** @defgroup SDIO_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_SDIO_Init(SDIO_HandleTypeDef *hsdio);
HAL_StatusTypeDef HAL_SDIO_DeInit(SDIO_HandleTypeDef *hsdio);

void HAL_SDIO_MspInit(SDIO_HandleTypeDef *hsdio);
void HAL_SDIO_MspDeInit(SDIO_HandleTypeDef *hsdio);
/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions_Group2 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_SDIO_SetDataBusWidth(SDIO_HandleTypeDef *hsdio, uint32_t BusWide);
HAL_StatusTypeDef HAL_SDIO_ConfigFrequency(SDIO_HandleTypeDef *hsdio, uint32_t ClockSpeed);

HAL_StatusTypeDef HAL_SDIO_SetBlockSize(SDIO_HandleTypeDef *hsdio, uint8_t function_nbr, uint16_t BlockSize);
HAL_StatusTypeDef HAL_SDIO_SetSpeedMode(SDIO_HandleTypeDef *hsdio, uint32_t DataRate);

HAL_StatusTypeDef HAL_SDIO_CardReset(SDIO_HandleTypeDef *hsdio);
HAL_StatusTypeDef HAL_SDIO_GetCardCommonControlRegister(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CCCR_TypeDef *pCccr);
HAL_StatusTypeDef HAL_SDIO_GetCardFBRRegister(SDIO_HandleTypeDef *hsdio, HAL_SDIO_FBR_t *pFbr);
/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions_Group3 Process functions
  * @{
  */
HAL_StatusTypeDef HAL_SDIO_ReadDirect(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_DirectCmd_TypeDef *Argument,
                                      uint8_t *pData);
HAL_StatusTypeDef HAL_SDIO_WriteDirect(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_DirectCmd_TypeDef *Argument,
                                       uint8_t Data);

HAL_StatusTypeDef HAL_SDIO_ReadExtended(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                        uint8_t *pData, uint32_t Size_byte, uint32_t Timeout_Ms);

HAL_StatusTypeDef HAL_SDIO_WriteExtended(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                         uint8_t *pData, uint32_t Size_byte, uint32_t Timeout_Ms);

HAL_StatusTypeDef HAL_SDIO_ReadExtended_DMA(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                            uint8_t *pData, uint32_t Size_byte);

HAL_StatusTypeDef HAL_SDIO_WriteExtended_DMA(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                             uint8_t *pData, uint32_t Size_byte);
/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions_Group4 IRQHandler and callback functions
  * @{
  */
void HAL_SDIO_IRQHandler(SDIO_HandleTypeDef *hsdio);

void HAL_SDIO_TxCpltCallback(SDIO_HandleTypeDef *hsdio);
void HAL_SDIO_RxCpltCallback(SDIO_HandleTypeDef *hsdio);
void HAL_SDIO_ErrorCallback(SDIO_HandleTypeDef *hsdio);
void HAL_SDIO_IOFunctionCallback(SDIO_HandleTypeDef *hsdio, uint32_t func);
#if (USE_SDIO_TRANSCEIVER != 0U)
/* Callback to switch in 1.8V mode */
void HAL_SDIO_DriveTransceiver_1_8V_Callback(SDIO_HandleTypeDef *hsdio, FlagStatus status);
#endif /* USE_SDIO_TRANSCEIVER */

#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_SDIO_RegisterCallback(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CallbackIDTypeDef CallbackID,
                                            pSDIO_CallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_SDIO_UnRegisterCallback(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */

#if (USE_SDIO_TRANSCEIVER != 0U)
HAL_StatusTypeDef HAL_SDIO_RegisterTransceiverCallback(SDIO_HandleTypeDef *hsdio,
                                                       pSDIO_TransceiverCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_SDIO_UnRegisterTransceiverCallback(SDIO_HandleTypeDef *hsdio);
#endif /* USE_SDIO_TRANSCEIVER */

HAL_StatusTypeDef HAL_SDIO_RegisterIOFunctionCallback(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction,
                                                      HAL_SDIO_IOFunction_CallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_SDIO_RegisterIdentifyCardCallback(SDIO_HandleTypeDef *hsdio,
                                                        pSDIO_IdentifyCardCallbackTypeDef pCallback);
/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions_Group5 Peripheral State and Errors functions
  * @{
  */
HAL_SDIO_StateTypeDef HAL_SDIO_GetState(const SDIO_HandleTypeDef *hsdio);
uint32_t              HAL_SDIO_GetError(const SDIO_HandleTypeDef *hsdio);
/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions_Group6 Peripheral IO interrupt
  * @{
  */
HAL_StatusTypeDef HAL_SDIO_EnableIOFunctionInterrupt(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);
HAL_StatusTypeDef HAL_SDIO_DisableIOFunctionInterrupt(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);

HAL_StatusTypeDef HAL_SDIO_EnableIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);
HAL_StatusTypeDef HAL_SDIO_DisableIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);

HAL_StatusTypeDef HAL_SDIO_SelectIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);
HAL_StatusTypeDef HAL_SDIO_AbortIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction);

HAL_StatusTypeDef HAL_SDIO_EnableIOAsynInterrupt(SDIO_HandleTypeDef *hsdio);
HAL_StatusTypeDef HAL_SDIO_DisableIOAsynInterrupt(SDIO_HandleTypeDef *hsdio);

/**
  * @}
  */

/* Private types -----------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Types SDIO Private Types
  * @{
  */

/**
  * @}
  */

/* Private defines ---------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Defines SDIO Private Defines
  * @{
  */

/**
  * @}
  */

/* Private variables -------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Variables SDIO Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants -------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Constants SDIO Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ----------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Macros SDIO Private Macros
  * @{
  */

/**
  * @}
  */

/* Private functions prototypes --------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Functions_Prototypes SDIO Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Functions SDIO Private Functions
  * @{
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
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* STM32N6xx_HAL_SDIO_H */

