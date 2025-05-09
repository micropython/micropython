/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pssi.h
  * @author  MCD Application Team
  * @brief   Header file of PSSI HAL module.
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
#ifndef STM32N6xx_HAL_PSSI_H
#define STM32N6xx_HAL_PSSI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined(PSSI)

#ifndef USE_HAL_PSSI_REGISTER_CALLBACKS
/* For backward compatibility, if USE_HAL_PSSI_REGISTER_CALLBACKS not defined, define it to 1*/
#define USE_HAL_PSSI_REGISTER_CALLBACKS 0U
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */

/** @addtogroup PSSI PSSI
  * @brief PSSI HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PSSI_Exported_Types PSSI Exported Types
  * @{
  */


/**
  * @brief PSSI Init structure definition
  */
typedef struct
{
  uint32_t  DataWidth;          /* !< Configures the data width.
                                      This parameter can be a value of @ref PSSI_DATA_WIDTH. */
  uint32_t  BusWidth;           /* !< Configures the parallel bus width.
                                      This parameter can be a value of @ref PSSI_BUS_WIDTH. */
  uint32_t  ControlSignal;      /* !< Configures Data enable and Data ready.
                                      This parameter can be a value of @ref ControlSignal_Configuration. */
  uint32_t  ClockPolarity;      /* !< Configures the PSSI Input Clock polarity.
                                      This parameter can be a value of @ref Clock_Polarity. */
  uint32_t  DataEnablePolarity; /* !< Configures the PSSI Data Enable polarity.
                                      This parameter can be a value of @ref Data_Enable_Polarity. */
  uint32_t  ReadyPolarity;      /* !< Configures the PSSI Ready polarity.
                                      This parameter can be a value of @ref Ready_Polarity. */

} PSSI_InitTypeDef;


/**
  * @brief  HAL PSSI State structures definition
  */
typedef enum
{
  HAL_PSSI_STATE_RESET   = 0x00U, /* !< PSSI not yet initialized or disabled     */
  HAL_PSSI_STATE_READY   = 0x01U, /* !< Peripheral initialized and ready for use */
  HAL_PSSI_STATE_BUSY    = 0x02U, /* !< An internal process is ongoing           */
  HAL_PSSI_STATE_BUSY_TX = 0x03U, /* !< Transmit process is ongoing              */
  HAL_PSSI_STATE_BUSY_RX = 0x04U, /* !< Receive process is ongoing               */
  HAL_PSSI_STATE_TIMEOUT = 0x05U, /* !< Timeout state                            */
  HAL_PSSI_STATE_ERROR   = 0x06U, /* !< PSSI state error                         */
  HAL_PSSI_STATE_ABORT   = 0x07U, /* !< PSSI process is aborted                  */

} HAL_PSSI_StateTypeDef;

/**
  * @brief  PSSI handle Structure definition
  */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
typedef struct __PSSI_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
{
  PSSI_TypeDef         *Instance;    /*!< PSSI register base address.    */
  PSSI_InitTypeDef      Init;        /*!< PSSI Initialization Structure. */
  uint32_t             *pBuffPtr;    /*!< PSSI Data buffer.              */
  uint32_t              XferCount;   /*!< PSSI transfer count            */
  uint32_t              XferSize;    /*!< PSSI  transfer size            */
#if defined(HAL_DMA_MODULE_ENABLED)
  DMA_HandleTypeDef    *hdmatx;      /*!< PSSI Tx DMA Handle parameters  */
  DMA_HandleTypeDef    *hdmarx;      /*!< PSSI Rx DMA Handle parameters  */
#endif /*HAL_DMA_MODULE_ENABLED*/

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
  void (* TxCpltCallback)(struct __PSSI_HandleTypeDef *hpssi);    /*!< PSSI transfer complete callback. */
  void (* RxCpltCallback)(struct __PSSI_HandleTypeDef *hpssi);    /*!< PSSI transfer complete callback. */
  void (* ErrorCallback)(struct __PSSI_HandleTypeDef *hpssi);     /*!< PSSI transfer complete callback. */
  void (* AbortCpltCallback)(struct __PSSI_HandleTypeDef *hpssi); /*!< PSSI transfer error callback.    */

  void (* MspInitCallback)(struct __PSSI_HandleTypeDef *hpssi);   /*!< PSSI Msp Init callback.          */
  void (* MspDeInitCallback)(struct __PSSI_HandleTypeDef *hpssi); /*!< PSSI Msp DeInit callback.        */
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */

  HAL_LockTypeDef             Lock;                               /*!< PSSI lock.                       */
  __IO HAL_PSSI_StateTypeDef State;                               /*!< PSSI transfer state.             */
  __IO uint32_t               ErrorCode;                          /*!< PSSI error code.                 */

} PSSI_HandleTypeDef;

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL PSSI Callback pointer definition
  */
typedef  void (*pPSSI_CallbackTypeDef)(PSSI_HandleTypeDef *hpssi);  /*!< Pointer to a PSSI common callback function */

/**
  * @brief  HAL PSSI Callback ID enumeration definition
  */
typedef enum
{
  HAL_PSSI_TX_COMPLETE_CB_ID = 0x00U, /*!< PSSI Tx Transfer completed callback ID  */
  HAL_PSSI_RX_COMPLETE_CB_ID = 0x01U, /*!< PSSI Rx Transfer completed callback ID  */
  HAL_PSSI_ERROR_CB_ID       = 0x03U, /*!< PSSI Error callback ID                  */
  HAL_PSSI_ABORT_CB_ID       = 0x04U, /*!< PSSI Abort callback ID                  */

  HAL_PSSI_MSPINIT_CB_ID     = 0x05U, /*!< PSSI Msp Init callback ID               */
  HAL_PSSI_MSPDEINIT_CB_ID   = 0x06U  /*!< PSSI Msp DeInit callback ID             */

} HAL_PSSI_CallbackIDTypeDef;
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PSSI_Exported_Constants PSSI Exported Constants
  * @{
  */

/** @defgroup PSSI_Error_Code PSSI Error Code
  * @{
  */
#define HAL_PSSI_ERROR_NONE             0x00000000U /*!< No error                */
#define HAL_PSSI_ERROR_NOT_SUPPORTED    0x00000001U /*!< Not supported operation */
#define HAL_PSSI_ERROR_UNDER_RUN        0x00000002U /*!< FIFO Under-run error    */
#define HAL_PSSI_ERROR_OVER_RUN         0x00000004U /*!< FIFO Over-run  error    */
#define HAL_PSSI_ERROR_DMA              0x00000008U /*!< Dma     error           */
#define HAL_PSSI_ERROR_TIMEOUT          0x00000010U /*!< Timeout error           */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
#define HAL_PSSI_ERROR_INVALID_CALLBACK 0x00000020U /*!< Invalid callback error  */
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup PSSI_DATA_WIDTH PSSI Data Width
  * @{
  */

#define HAL_PSSI_8BITS                  0x00000000U   /*!<  8 Bits  */
#define HAL_PSSI_16BITS                 0x00000001U   /*!< 16 Bits  */
#define HAL_PSSI_32BITS                 0x00000002U   /*!< 32 Bits  */
/**
  * @}
  */

/** @defgroup PSSI_BUS_WIDTH PSSI Bus Width
  * @{
  */

#define HAL_PSSI_8LINES                 0x00000000U   /*!< 8 data lines  */
#define HAL_PSSI_16LINES                PSSI_CR_EDM   /*!< 16 data lines */
/**
  * @}
  */
/** @defgroup PSSI_MODE PSSI mode
  * @{
  */
#define HAL_PSSI_UNIDIRECTIONAL         0x00000000U /*!< Uni-directional mode */
#define HAL_PSSI_BIDIRECTIONAL          0x00000001U /*!< Bi-directional mode  */
/**
  * @}
  */

/** @defgroup ControlSignal_Configuration ControlSignal Configuration
  * @{
  */
#define HAL_PSSI_DE_RDY_DISABLE           (0x0U << PSSI_CR_DERDYCFG_Pos) /*!< Neither DE nor RDY are enabled */
#define HAL_PSSI_RDY_ENABLE               (0x1U << PSSI_CR_DERDYCFG_Pos) /*!< Only RDY enabled */
#define HAL_PSSI_DE_ENABLE                (0x2U << PSSI_CR_DERDYCFG_Pos) /*!< Only DE enabled */
#define HAL_PSSI_DE_RDY_ALT_ENABLE        (0x3U << PSSI_CR_DERDYCFG_Pos) /*!< Both RDY and DE alternate functions enabled */
#define HAL_PSSI_MAP_RDY_BIDIR_ENABLE     (0x4U << PSSI_CR_DERDYCFG_Pos) /*!< Bi-directional on RDY pin */
#define HAL_PSSI_RDY_MAP_ENABLE           (0x5U << PSSI_CR_DERDYCFG_Pos) /*!< Only RDY enabled, mapped to DE pin */
#define HAL_PSSI_DE_MAP_ENABLE            (0x6U << PSSI_CR_DERDYCFG_Pos) /*!< Only DE enabled, mapped to RDY pin */
#define HAL_PSSI_MAP_DE_BIDIR_ENABLE      (0x7U << PSSI_CR_DERDYCFG_Pos) /*!< Bi-directional on DE pin */

/**
  * @}
  */


/** @defgroup Data_Enable_Polarity Data Enable Polarity
  * @{
  */
#define HAL_PSSI_DEPOL_ACTIVE_LOW         0x0U            /*!< Active Low */
#define HAL_PSSI_DEPOL_ACTIVE_HIGH        PSSI_CR_DEPOL   /*!< Active High */
/**
  * @}
  */
/** @defgroup Ready_Polarity Ready Polarity
  * @{
  */
#define HAL_PSSI_RDYPOL_ACTIVE_LOW        0x0U            /*!< Active Low */
#define HAL_PSSI_RDYPOL_ACTIVE_HIGH       PSSI_CR_RDYPOL  /*!< Active High */
/**
  * @}
  */

/** @defgroup Clock_Polarity Clock Polarity
  * @{
  */
#define HAL_PSSI_FALLING_EDGE             0x0U            /*!< Fallling Edge */
#define HAL_PSSI_RISING_EDGE              0x1U            /*!< Rising Edge */
/**
  * @}
  */

/** @defgroup Clock_Source Clock Source
  * @{
  */
#define HAL_PSSI_CLOCK_EXT                0x0U                     /*!< External Clock */
#define HAL_PSSI_CLOCK_INT                PSSI_CR_CKSRC            /*!< Internal Clock */
/**
  * @}
  */


/** @defgroup PSSI_DEFINITION PSSI definitions
  * @{
  */

#define PSSI_MAX_NBYTE_SIZE         0x10000U         /* 64 KB */
#define PSSI_TIMEOUT_TRANSMIT       0x0000FFFFU      /*!< Timeout Value   */

#define PSSI_CR_OUTEN_INPUT         0x00000000U      /*!< Input Mode      */
#define PSSI_CR_OUTEN_OUTPUT        PSSI_CR_OUTEN    /*!< Output Mode     */

#define PSSI_CR_DMA_ENABLE          PSSI_CR_DMAEN    /*!< DMA Mode Enable */
#define PSSI_CR_DMA_DISABLE         (~PSSI_CR_DMAEN) /*!< DMA Mode Disable*/

#define PSSI_CR_16BITS              PSSI_CR_EDM      /*!< 16 Lines Mode   */
#define PSSI_CR_8BITS               (~PSSI_CR_EDM)   /*!< 8 Lines Mode    */

#define PSSI_FLAG_RTT1B             PSSI_SR_RTT1B    /*!< 1 Byte Fifo Flag */
#define PSSI_FLAG_RTT4B             PSSI_SR_RTT4B    /*!< 4 Bytes Fifo Flag*/


/**
  * @}
  */

/** @defgroup PSSI_Interrupts PSSI Interrupts
  * @{
  */

#define PSSI_FLAG_OVR_RIS            PSSI_RIS_OVR_RIS     /*!< Overrun, Underrun errors flag */
#define PSSI_FLAG_MASK               PSSI_RIS_OVR_RIS_Msk /*!< Overrun, Underrun errors Mask */
#define PSSI_FLAG_OVR_MIS            PSSI_MIS_OVR_MIS     /*!< Overrun, Underrun masked errors flag */
/**
  * @}
  */


/**
  * @}
  */
/* Exported macros ------------------------------------------------------------*/
/** @defgroup PSSI_Exported_Macros PSSI Exported Macros
  * @{
  */

/** @brief Reset PSSI handle state
  * @param  __HANDLE__ specifies the PSSI handle.
  * @retval None
  */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
#define HAL_PSSI_RESET_HANDLE_STATE(__HANDLE__) do{                                            \
                                                      (__HANDLE__)->State = HAL_PSSI_STATE_RESET;\
                                                      (__HANDLE__)->MspInitCallback = NULL;       \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;     \
                                                     }while(0)
#else
#define HAL_PSSI_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_PSSI_STATE_RESET)
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */


/**
  * @brief  Enable the PSSI.
  * @param  __HANDLE__ PSSI handle
  * @retval None.
  */
#define HAL_PSSI_ENABLE(__HANDLE__)        ((__HANDLE__)->Instance->CR |= PSSI_CR_ENABLE)
/**
  * @brief  Disable the PSSI.
  * @param  __HANDLE__ PSSI handle
  * @retval None.
  */
#define HAL_PSSI_DISABLE(__HANDLE__)        ((__HANDLE__)->Instance->CR &= (~PSSI_CR_ENABLE))

/* PSSI pripheral STATUS */
/**
  * @brief  Get the PSSI pending flags.
  * @param  __HANDLE__ PSSI handle
  * @param  __FLAG__ flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg PSSI_FLAG_RTT1B:  FIFO is ready to transfer one byte
  *            @arg PSSI_FLAG_RTT4B: FIFO is ready to transfer four bytes
  * @retval The state of FLAG.
  */

#define HAL_PSSI_GET_STATUS(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR & (__FLAG__))


/* Interrupt & Flag management */
/**
  * @brief  Get the PSSI pending flags.
  * @param  __HANDLE__ PSSI handle
  * @param  __FLAG__ flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg PSSI_FLAG_OVR_RIS: Data Buffer overrun/underrun error flag
  * @retval The state of FLAG.
  */
#define HAL_PSSI_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->RIS & (__FLAG__))

/**
  * @brief  Clear the PSSI pending flags.
  * @param  __HANDLE__ PSSI handle
  * @param  __FLAG__ specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg PSSI_FLAG_OVR_RIS: Data Buffer overrun/underrun error flag
  * @retval None
  */
#define HAL_PSSI_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR = (__FLAG__))

/**
  * @brief  Enable the specified PSSI interrupts.
  * @param  __HANDLE__ PSSI handle
  * @param __INTERRUPT__ specifies the PSSI interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *            @arg PSSI_FLAG_OVR_RIS: Configuration error mask
  * @retval None
  */
#define HAL_PSSI_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/**
  * @brief  Disable the specified PSSI interrupts.
  * @param  __HANDLE__ PSSI handle
  * @param __INTERRUPT__ specifies the PSSI interrupt sources to be disabled.
  *          This parameter can be any combination of the following values:
  *            @arg PSSI_IT_OVR_IE: Configuration error mask
  * @retval None
  */
#define HAL_PSSI_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified PSSI interrupt source is enabled or not.
  * @param  __HANDLE__ PSSI handle
  * @param  __INTERRUPT__ specifies the PSSI interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg PSSI_IT_OVR_IE: Data Buffer overrun/underrun error interrupt mask
  * @retval The state of INTERRUPT source.
  */
#define HAL_PSSI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER & (__INTERRUPT__))


/**
  * @brief  Check whether the PSSI Control signal is valid.
  * @param  __CONTROL__ Control signals configuration
  * @retval Valid or not.
  */

#define IS_PSSI_CONTROL_SIGNAL(__CONTROL__) (((__CONTROL__) == HAL_PSSI_DE_RDY_DISABLE        ) || \
                                             ((__CONTROL__) == HAL_PSSI_RDY_ENABLE            ) || \
                                             ((__CONTROL__) == HAL_PSSI_DE_ENABLE             ) || \
                                             ((__CONTROL__) == HAL_PSSI_DE_RDY_ALT_ENABLE     ) || \
                                             ((__CONTROL__) == HAL_PSSI_MAP_RDY_BIDIR_ENABLE  ) || \
                                             ((__CONTROL__) == HAL_PSSI_RDY_MAP_ENABLE        ) || \
                                             ((__CONTROL__) == HAL_PSSI_DE_MAP_ENABLE         ) || \
                                             ((__CONTROL__) == HAL_PSSI_MAP_DE_BIDIR_ENABLE   ))


/**
  * @brief  Check whether the PSSI Bus Width is valid.
  * @param  __BUSWIDTH__ PSSI Bush width
  * @retval Valid or not.
  */

#define IS_PSSI_BUSWIDTH(__BUSWIDTH__) (((__BUSWIDTH__) == HAL_PSSI_8LINES    ) || \
                                        ((__BUSWIDTH__) == HAL_PSSI_16LINES   ))

/**

  * @brief  Check whether the PSSI Clock Polarity is valid.
  * @param  __CLOCKPOL__ PSSI Clock Polarity
  * @retval Valid or not.
  */

#define IS_PSSI_CLOCK_POLARITY(__CLOCKPOL__) (((__CLOCKPOL__) == HAL_PSSI_FALLING_EDGE   ) || \
                                              ((__CLOCKPOL__) == HAL_PSSI_RISING_EDGE    ))


/**
  * @brief  Check whether the PSSI Data Enable Polarity is valid.
  * @param  __DEPOL__ PSSI DE Polarity
  * @retval Valid or not.
  */

#define IS_PSSI_DE_POLARITY(__DEPOL__) (((__DEPOL__) == HAL_PSSI_DEPOL_ACTIVE_LOW    ) || \
                                        ((__DEPOL__) == HAL_PSSI_DEPOL_ACTIVE_HIGH   ))

/**
  * @brief  Check whether the PSSI Ready Polarity is valid.
  * @param  __RDYPOL__ PSSI RDY Polarity
  * @retval Valid or not.
  */

#define IS_PSSI_RDY_POLARITY(__RDYPOL__) (((__RDYPOL__) == HAL_PSSI_RDYPOL_ACTIVE_LOW   ) || \
                                          ((__RDYPOL__) == HAL_PSSI_RDYPOL_ACTIVE_HIGH   ))

/**
  * @brief  Check whether the PSSI Clock source is valid.
  * @param  __CLOCKSRC__ PSSI Clock Source
  * @retval Valid or not.
  */

#define IS_PSSI_CLOCK_SOURCE(__CLOCKSRC__) (((__CLOCKSRC__) == HAL_PSSI_CLOCK_EXT   ) || \
                                            ((__CLOCKSRC__) == HAL_PSSI_CLOCK_INT   ))
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup PSSI_Exported_Functions PSSI Exported Functions
  * @{
  */

/** @addtogroup PSSI_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */

/* Initialization and de-initialization functions *******************************/
HAL_StatusTypeDef HAL_PSSI_Init(PSSI_HandleTypeDef *hpssi);
HAL_StatusTypeDef HAL_PSSI_DeInit(PSSI_HandleTypeDef *hpssi);
void              HAL_PSSI_MspInit(PSSI_HandleTypeDef *hpssi);
void              HAL_PSSI_MspDeInit(PSSI_HandleTypeDef *hpssi);
/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_PSSI_RegisterCallback(PSSI_HandleTypeDef *hpssi, HAL_PSSI_CallbackIDTypeDef CallbackID,
                                            pPSSI_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_PSSI_UnRegisterCallback(PSSI_HandleTypeDef *hpssi, HAL_PSSI_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */

/**
  * @}
  */


/** @addtogroup PSSI_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_PSSI_Transmit(PSSI_HandleTypeDef *hpssi, uint8_t *pData, uint32_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_PSSI_Receive(PSSI_HandleTypeDef *hpssi, uint8_t *pData, uint32_t Size, uint32_t Timeout);
#if defined(HAL_DMA_MODULE_ENABLED)
HAL_StatusTypeDef HAL_PSSI_Transmit_DMA(PSSI_HandleTypeDef *hpssi, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_PSSI_Receive_DMA(PSSI_HandleTypeDef *hpssi, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_PSSI_Abort_DMA(PSSI_HandleTypeDef *hpssi);
#endif /*HAL_DMA_MODULE_ENABLED*/

/**
  * @}
  */

/** @addtogroup PSSI_Exported_Functions_Group3 Peripheral State and Error functions
  * @{
  */

/* Peripheral State functions ***************************************************/
HAL_PSSI_StateTypeDef HAL_PSSI_GetState(const PSSI_HandleTypeDef *hpssi);
uint32_t              HAL_PSSI_GetError(const PSSI_HandleTypeDef *hpssi);

/**
  * @}
  */

/** @addtogroup PSSI_Exported_Functions_Group4 Clock Source Selection function
  * @{
  */
/* Clock source selection function  *******************************************/
HAL_StatusTypeDef HAL_PSSI_ClockConfig(PSSI_HandleTypeDef *hpssi, uint32_t ClockSource);

/**
  * @}
  */

/** @addtogroup PSSI_IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
  * @{
  */

void HAL_PSSI_IRQHandler(PSSI_HandleTypeDef *hpssi);
void HAL_PSSI_TxCpltCallback(PSSI_HandleTypeDef *hpssi);
void HAL_PSSI_RxCpltCallback(PSSI_HandleTypeDef *hpssi);
void HAL_PSSI_ErrorCallback(PSSI_HandleTypeDef *hpssi);
void HAL_PSSI_AbortCpltCallback(PSSI_HandleTypeDef *hpssi);

/**
  * @}
  */


/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/


/* Private macros ------------------------------------------------------------*/


/**
  * @}
  */
#endif /* PSSI */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_PSSI_H */

