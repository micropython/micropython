/**
  ******************************************************************************
  * @file    stm32n6xx_hal_i2s.h
  * @author  MCD Application Team
  * @brief   Header file of I2S HAL module.
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
#ifndef STM32N6xx_HAL_I2S_H
#define STM32N6xx_HAL_I2S_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup I2S
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup I2S_Exported_Types I2S Exported Types
  * @{
  */

/**
  * @brief I2S Init structure definition
  */
typedef struct
{
  uint32_t Mode;                /*!< Specifies the I2S operating mode.
                                     This parameter can be a value of @ref I2S_Mode */

  uint32_t Standard;            /*!< Specifies the standard used for the I2S communication.
                                     This parameter can be a value of @ref I2S_Standard */

  uint32_t DataFormat;          /*!< Specifies the data format for the I2S communication.
                                     This parameter can be a value of @ref I2S_Data_Format */

  uint32_t MCLKOutput;          /*!< Specifies whether the I2S MCLK output is enabled or not.
                                     This parameter can be a value of @ref I2S_MCLK_Output */

  uint32_t AudioFreq;           /*!< Specifies the frequency selected for the I2S communication.
                                     This parameter can be a value of @ref I2S_Audio_Frequency */

  uint32_t CPOL;                /*!< Specifies the idle state of the I2S clock.
                                     This parameter can be a value of @ref I2S_Clock_Polarity */

  uint32_t FirstBit;            /*!< Specifies whether data transfers start from MSB or LSB bit.
                                     This parameter can be a value of @ref I2S_MSB_LSB_Transmission       */

  uint32_t WSInversion;         /*!< Control the Word Select Inversion.
                                     This parameter can be a value of @ref I2S_WSInversion                */

  uint32_t Data24BitAlignment;  /*!< Specifies the Data Padding for 24 bits data length
                                      This parameter can be a value of @ref I2S_Data_24Bit_Alignment       */

  uint32_t MasterKeepIOState;   /*!< Control of Alternate function GPIOs state
                                     This parameter can be a value of @ref I2S_Master_Keep_IO_State */

} I2S_InitTypeDef;

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_I2S_STATE_RESET      = 0x00UL,  /*!< I2S not yet initialized or disabled                */
  HAL_I2S_STATE_READY      = 0x01UL,  /*!< I2S initialized and ready for use                  */
  HAL_I2S_STATE_BUSY       = 0x02UL,  /*!< I2S internal process is ongoing                    */
  HAL_I2S_STATE_BUSY_TX    = 0x03UL,  /*!< Data Transmission process is ongoing               */
  HAL_I2S_STATE_BUSY_RX    = 0x04UL,  /*!< Data Reception process is ongoing                  */
  HAL_I2S_STATE_BUSY_TX_RX = 0x05UL,  /*!< Data Transmission and Reception process is ongoing */
  HAL_I2S_STATE_TIMEOUT    = 0x06UL,  /*!< I2S timeout state                                  */
  HAL_I2S_STATE_ERROR      = 0x07UL   /*!< I2S error state                                    */
} HAL_I2S_StateTypeDef;

/**
  * @brief I2S handle Structure definition
  */
typedef struct __I2S_HandleTypeDef
{
  SPI_TypeDef                *Instance;            /*!< I2S registers base address */

  I2S_InitTypeDef            Init;                 /*!< I2S communication parameters */

  const uint16_t             *pTxBuffPtr;          /*!< Pointer to I2S Tx transfer buffer */

  __IO uint16_t              TxXferSize;           /*!< I2S Tx transfer size */

  __IO uint16_t              TxXferCount;          /*!< I2S Tx transfer Counter */

  uint16_t                   *pRxBuffPtr;          /*!< Pointer to I2S Rx transfer buffer */

  __IO uint16_t              RxXferSize;           /*!< I2S Rx transfer size */

  __IO uint16_t              RxXferCount;          /*!< I2S Rx transfer counter
                                                      (This field is initialized at the
                                                       same value as transfer size at the
                                                       beginning of the transfer and
                                                       decremented when a sample is received
                                                       NbSamplesReceived = RxBufferSize-RxBufferCount) */

  void (*RxISR)(struct __I2S_HandleTypeDef *hi2s); /*!< function pointer on Rx ISR */

  void (*TxISR)(struct __I2S_HandleTypeDef *hi2s); /*!< function pointer on Tx ISR */

  DMA_HandleTypeDef          *hdmatx;              /*!< I2S Tx DMA handle parameters */

  DMA_HandleTypeDef          *hdmarx;              /*!< I2S Rx DMA handle parameters */

  __IO HAL_LockTypeDef       Lock;                 /*!< I2S locking object */

  __IO HAL_I2S_StateTypeDef  State;                /*!< I2S communication state */

  __IO uint32_t              ErrorCode;            /*!< I2S Error code
                                                        This parameter can be a value of @ref I2S_Error */

#if (USE_HAL_I2S_REGISTER_CALLBACKS == 1U)
  void (* TxCpltCallback)(struct __I2S_HandleTypeDef *hi2s);             /*!< I2S Tx Completed callback          */
  void (* RxCpltCallback)(struct __I2S_HandleTypeDef *hi2s);             /*!< I2S Rx Completed callback          */
  void (* TxRxCpltCallback)(struct __I2S_HandleTypeDef *hi2s);           /*!< I2S TxRx Completed callback        */
  void (* TxHalfCpltCallback)(struct __I2S_HandleTypeDef *hi2s);         /*!< I2S Tx Half Completed callback     */
  void (* RxHalfCpltCallback)(struct __I2S_HandleTypeDef *hi2s);         /*!< I2S Rx Half Completed callback     */
  void (* TxRxHalfCpltCallback)(struct __I2S_HandleTypeDef *hi2s);       /*!< I2S TxRx Half Completed callback   */
  void (* ErrorCallback)(struct __I2S_HandleTypeDef *hi2s);              /*!< I2S Error callback                 */
  void (* MspInitCallback)(struct __I2S_HandleTypeDef *hi2s);            /*!< I2S Msp Init callback              */
  void (* MspDeInitCallback)(struct __I2S_HandleTypeDef *hi2s);          /*!< I2S Msp DeInit callback            */

#endif  /* USE_HAL_I2S_REGISTER_CALLBACKS */
} I2S_HandleTypeDef;

#if (USE_HAL_I2S_REGISTER_CALLBACKS == 1UL)
/**

  * @brief  HAL I2S Callback ID enumeration definition
  */
typedef enum
{
  HAL_I2S_TX_COMPLETE_CB_ID             = 0x00UL,    /*!< I2S Tx Completed callback ID         */
  HAL_I2S_RX_COMPLETE_CB_ID             = 0x01UL,    /*!< I2S Rx Completed callback ID         */
  HAL_I2S_TX_RX_COMPLETE_CB_ID          = 0x02UL,    /*!< I2S TxRx Completed callback ID       */
  HAL_I2S_TX_HALF_COMPLETE_CB_ID        = 0x03UL,    /*!< I2S Tx Half Completed callback ID    */
  HAL_I2S_RX_HALF_COMPLETE_CB_ID        = 0x04UL,    /*!< I2S Rx Half Completed callback ID    */
  HAL_I2S_TX_RX_HALF_COMPLETE_CB_ID     = 0x05UL,    /*!< I2S TxRx Half Completed callback ID  */
  HAL_I2S_ERROR_CB_ID                   = 0x06UL,    /*!< I2S Error callback ID                */
  HAL_I2S_MSPINIT_CB_ID                 = 0x07UL,    /*!< I2S Msp Init callback ID             */
  HAL_I2S_MSPDEINIT_CB_ID               = 0x08UL     /*!< I2S Msp DeInit callback ID           */

} HAL_I2S_CallbackIDTypeDef;

/**
  * @brief  HAL I2S Callback pointer definition
  */
typedef  void (*pI2S_CallbackTypeDef)(I2S_HandleTypeDef *hi2s); /*!< pointer to an I2S callback function */

#endif /* USE_HAL_I2S_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup I2S_Exported_Constants I2S Exported Constants
  * @{
  */
/** @defgroup I2S_Error I2S Error
  * @{
  */
#define HAL_I2S_ERROR_NONE               (0x00000000UL)  /*!< No error                          */
#define HAL_I2S_ERROR_TIMEOUT            (0x00000001UL)  /*!< Timeout error                     */
#define HAL_I2S_ERROR_OVR                (0x00000002UL)  /*!< OVR error                         */
#define HAL_I2S_ERROR_UDR                (0x00000004UL)  /*!< UDR error                         */
#define HAL_I2S_ERROR_DMA                (0x00000008UL)  /*!< DMA transfer error                */
#define HAL_I2S_ERROR_PRESCALER          (0x00000010UL)  /*!< Prescaler Calculation error       */
#define HAL_I2S_ERROR_FRE                (0x00000020UL)  /*!< FRE error                         */
#define HAL_I2S_ERROR_NO_OGT             (0x00000040UL)  /*!< No On Going Transfer error        */
#define HAL_I2S_ERROR_NOT_SUPPORTED      (0x00000080UL)  /*!< Requested operation not supported */
#if (USE_HAL_I2S_REGISTER_CALLBACKS == 1UL)
#define HAL_I2S_ERROR_INVALID_CALLBACK   (0x00000100UL)  /*!< Invalid Callback error      */
#endif /* USE_HAL_I2S_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup I2S_Mode I2S Mode
  * @{
  */
#define I2S_MODE_SLAVE_TX                (0x00000000UL)
#define I2S_MODE_SLAVE_RX                (SPI_I2SCFGR_I2SCFG_0)
#define I2S_MODE_MASTER_TX               (SPI_I2SCFGR_I2SCFG_1)
#define I2S_MODE_MASTER_RX               (SPI_I2SCFGR_I2SCFG_0 | SPI_I2SCFGR_I2SCFG_1)
#define I2S_MODE_SLAVE_FULLDUPLEX        (SPI_I2SCFGR_I2SCFG_2)
#define I2S_MODE_MASTER_FULLDUPLEX       (SPI_I2SCFGR_I2SCFG_2 | SPI_I2SCFGR_I2SCFG_0)
/**
  * @}
  */

/** @defgroup I2S_Standard I2S Standard
  * @{
  */
#define I2S_STANDARD_PHILIPS             (0x00000000UL)
#define I2S_STANDARD_MSB                 (SPI_I2SCFGR_I2SSTD_0)
#define I2S_STANDARD_LSB                 (SPI_I2SCFGR_I2SSTD_1)
#define I2S_STANDARD_PCM_SHORT           (SPI_I2SCFGR_I2SSTD_0 | SPI_I2SCFGR_I2SSTD_1)
#define I2S_STANDARD_PCM_LONG            (SPI_I2SCFGR_I2SSTD_0 | SPI_I2SCFGR_I2SSTD_1 | SPI_I2SCFGR_PCMSYNC)
/**
  * @}
  */

/** @defgroup I2S_Data_Format I2S Data Format
  * @{
  */
#define I2S_DATAFORMAT_16B               (0x00000000UL)
#define I2S_DATAFORMAT_16B_EXTENDED      (SPI_I2SCFGR_CHLEN)
#define I2S_DATAFORMAT_24B               (SPI_I2SCFGR_DATLEN_0)
#define I2S_DATAFORMAT_32B               (SPI_I2SCFGR_DATLEN_1)
/**
  * @}
  */

/** @defgroup I2S_MCLK_Output I2S MCLK Output
  * @{
  */
#define I2S_MCLKOUTPUT_ENABLE            (SPI_I2SCFGR_MCKOE)
#define I2S_MCLKOUTPUT_DISABLE           (0x00000000UL)
/**
  * @}
  */

/** @defgroup I2S_Audio_Frequency I2S Audio Frequency
  * @{
  */
#define I2S_AUDIOFREQ_192K               (192000UL)
#define I2S_AUDIOFREQ_96K                (96000UL)
#define I2S_AUDIOFREQ_48K                (48000UL)
#define I2S_AUDIOFREQ_44K                (44100UL)
#define I2S_AUDIOFREQ_32K                (32000UL)
#define I2S_AUDIOFREQ_22K                (22050UL)
#define I2S_AUDIOFREQ_16K                (16000UL)
#define I2S_AUDIOFREQ_11K                (11025UL)
#define I2S_AUDIOFREQ_8K                 (8000UL)
#define I2S_AUDIOFREQ_DEFAULT            (2UL)
/**
  * @}
  */

/** @defgroup I2S_Clock_Polarity I2S FullDuplex Mode
  * @{
  */
#define I2S_CPOL_LOW                     (0x00000000UL)
#define I2S_CPOL_HIGH                    (SPI_I2SCFGR_CKPOL)
/**
  * @}
  */

/** @defgroup I2S_MSB_LSB_Transmission I2S MSB LSB Transmission
  * @{
  */
#define I2S_FIRSTBIT_MSB                 (0x00000000UL)
#define I2S_FIRSTBIT_LSB                 SPI_CFG2_LSBFRST
/**
  * @}
  */

/** @defgroup I2S_WSInversion I2S Word Select Inversion
  * @{
  */
#define I2S_WS_INVERSION_DISABLE         (0x00000000UL)
#define I2S_WS_INVERSION_ENABLE          SPI_I2SCFGR_WSINV
/**
  * @}
  */

/** @defgroup I2S_Data_24Bit_Alignment Data Padding 24Bit
  * @{
  */
#define I2S_DATA_24BIT_ALIGNMENT_RIGHT   (0x00000000UL)
#define I2S_DATA_24BIT_ALIGNMENT_LEFT    SPI_I2SCFGR_DATFMT
/**
  * @}
  */

/** @defgroup I2S_Master_Keep_IO_State Keep IO State
  * @{
  */
#define I2S_MASTER_KEEP_IO_STATE_DISABLE (0x00000000U)
#define I2S_MASTER_KEEP_IO_STATE_ENABLE  SPI_CFG2_AFCNTR
/**
  * @}
  */

/** @defgroup I2S_Interrupts_Definition I2S Interrupts Definition
  * @{
  */
#define I2S_IT_RXP                       SPI_IER_RXPIE
#define I2S_IT_TXP                       SPI_IER_TXPIE
#define I2S_IT_DXP                       SPI_IER_DXPIE
#define I2S_IT_UDR                       SPI_IER_UDRIE
#define I2S_IT_OVR                       SPI_IER_OVRIE
#define I2S_IT_FRE                       SPI_IER_TIFREIE
#define I2S_IT_ERR                       (SPI_IER_UDRIE | SPI_IER_OVRIE | SPI_IER_TIFREIE)
/**
  * @}
  */

/** @defgroup I2S_Flags_Definition I2S Flags Definition
  * @{
  */
#define I2S_FLAG_RXP                     SPI_SR_RXP       /* I2S status flag : Rx-Packet available flag              */
#define I2S_FLAG_TXP                     SPI_SR_TXP       /* I2S status flag : Tx-Packet space available flag        */
#define I2S_FLAG_DXP                     SPI_SR_DXP       /* I2S status flag : Dx-Packet space available flag        */
#define I2S_FLAG_UDR                     SPI_SR_UDR       /* I2S Error flag  : Underrun flag                         */
#define I2S_FLAG_OVR                     SPI_SR_OVR       /* I2S Error flag  : Overrun flag                          */
#define I2S_FLAG_FRE                     SPI_SR_TIFRE     /* I2S Error flag  : TI mode frame format error flag       */

#define I2S_FLAG_MASK                    (SPI_SR_RXP | SPI_SR_TXP | SPI_SR_DXP |SPI_SR_UDR | SPI_SR_OVR | SPI_SR_TIFRE)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup I2S_Exported_macros I2S Exported Macros
  * @{
  */

/** @brief  Reset I2S handle state
  * @param  __HANDLE__ specifies the I2S Handle.
  * @retval None
  */
#if (USE_HAL_I2S_REGISTER_CALLBACKS == 1UL)
#define __HAL_I2S_RESET_HANDLE_STATE(__HANDLE__)                do{                                                  \
                                                                    (__HANDLE__)->State = HAL_I2S_STATE_RESET;       \
                                                                    (__HANDLE__)->MspInitCallback = NULL;            \
                                                                    (__HANDLE__)->MspDeInitCallback = NULL;          \
                                                                  } while(0)
#else
#define __HAL_I2S_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_I2S_STATE_RESET)
#endif /* USE_HAL_I2S_REGISTER_CALLBACKS */

/** @brief  Enable the specified SPI peripheral (in I2S mode).
  * @param  __HANDLE__ specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_ENABLE(__HANDLE__)    (SET_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_SPE))

/** @brief  Disable the specified SPI peripheral (in I2S mode).
  * @param  __HANDLE__ specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_DISABLE(__HANDLE__) (CLEAR_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_SPE))

/** @brief  Enable the specified I2S interrupts.
  * @param  __HANDLE__ specifies the I2S Handle.
  *        This parameter can be I2S where x: 1, 2 or 3 to select the I2S peripheral.
  * @param  __INTERRUPT__ specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg I2S_IT_RXP : Rx-Packet available interrupt
  *            @arg I2S_IT_TXP : Tx-Packet space available interrupt
  *            @arg I2S_IT_UDR : Underrun interrupt
  *            @arg I2S_IT_OVR : Overrun interrupt
  *            @arg I2S_IT_FRE : TI mode frame format error interrupt
  *            @arg I2S_IT_ERR : Error interrupt enable
  * @retval None
  */
#define __HAL_I2S_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/** @brief  Disable the specified I2S interrupts.
  * @param  __HANDLE__ specifies the I2S Handle.
  *         This parameter can be I2S where x: 1, 2 or 3 to select the I2S peripheral.
  * @param  __INTERRUPT__ specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg I2S_IT_RXP : Rx-Packet available interrupt
  *            @arg I2S_IT_TXP : Tx-Packet space available interrupt
  *            @arg I2S_IT_UDR : Underrun interrupt
  *            @arg I2S_IT_OVR : Overrun interrupt
  *            @arg I2S_IT_FRE : TI mode frame format error interrupt
  *            @arg I2S_IT_ERR : Error interrupt enable
  * @retval None
  */
#define __HAL_I2S_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= (~(__INTERRUPT__)))

/** @brief  Check if the specified I2S interrupt source is enabled or disabled.
  * @param  __HANDLE__ specifies the I2S Handle.
  *         This parameter can be I2S where x: 1, 2 or 3 to select the I2S peripheral.
  * @param  __INTERRUPT__ specifies the I2S interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg I2S_IT_RXP : Rx-Packet available interrupt
  *            @arg I2S_IT_TXP : Tx-Packet space available interrupt
  *            @arg I2S_IT_DXP : Tx-Packet space available interrupt
  *            @arg I2S_IT_UDR : Underrun interrupt
  *            @arg I2S_IT_OVR : Overrun interrupt
  *            @arg I2S_IT_FRE : TI mode frame format error interrupt
  *            @arg I2S_IT_ERR : Error interrupt enable
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_I2S_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IER\
                                                              & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified I2S flag is set or not.
  * @param  __HANDLE__ specifies the I2S Handle.
  *         This parameter can be I2S where x: 1, 2 or 3 to select the I2S peripheral.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg I2S_FLAG_RXP : Rx-Packet available flag
  *            @arg I2S_FLAG_TXP : Tx-Packet space available flag
  *            @arg I2S_FLAG_UDR : Underrun flag
  *            @arg I2S_FLAG_OVR : Overrun flag
  *            @arg I2S_FLAG_FRE : TI mode frame format error flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_I2S_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief Clear the I2S OVR pending flag.
  * @param  __HANDLE__ specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_CLEAR_OVRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_OVRC)

/** @brief Clear the I2S UDR pending flag.
  * @param  __HANDLE__ specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_CLEAR_UDRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_UDRC)

/** @brief  Clear the I2S FRE pending flag.
  * @param  __HANDLE__: specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_CLEAR_TIFREFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_TIFREC)
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup I2S_Exported_Functions
  * @{
  */

/** @addtogroup I2S_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_I2S_REGISTER_CALLBACKS == 1UL)
HAL_StatusTypeDef HAL_I2S_RegisterCallback(I2S_HandleTypeDef *hi2s, HAL_I2S_CallbackIDTypeDef CallbackID,
                                           pI2S_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_I2S_UnRegisterCallback(I2S_HandleTypeDef *hi2s, HAL_I2S_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_I2S_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @addtogroup I2S_Exported_Functions_Group2
  * @{
  */
/* I/O operation functions  ***************************************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, const uint16_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive(I2S_HandleTypeDef *hi2s, const uint16_t *pTxData, uint16_t *pRxData,
                                            uint16_t Size, uint32_t Timeout);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, const uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, const uint16_t *pTxData, uint16_t *pRxData,
                                               uint16_t Size);

void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, const uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, const uint16_t *pTxData, uint16_t *pRxData,
                                                uint16_t Size);

HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s);

/* Callbacks used in non blocking modes (Interrupt and DMA) *******************/
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);
/**
  * @}
  */

/** @addtogroup I2S_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control and State functions  ************************************/
HAL_I2S_StateTypeDef HAL_I2S_GetState(const I2S_HandleTypeDef *hi2s);
uint32_t HAL_I2S_GetError(const I2S_HandleTypeDef *hi2s);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup I2S_Private_Constants I2S Private Constants
  * @{
  */

/**
  * @}
  */

/* Private Functions ---------------------------------------------------------*/
/** @defgroup I2S_Private_Functions I2S Private Functions
  * @{
  */
/* Private functions are defined in stm32h7xx_hal_i2S.c file */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup I2S_Private_Macros I2S Private Macros
  * @{
  */

/** @brief  Check whether the specified SPI flag is set or not.
  * @param  __SR__  copy of I2S SR register.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg I2S_FLAG_RXP : Rx-Packet available flag
  *            @arg I2S_FLAG_TXP : Tx-Packet space available flag
  *            @arg I2S_FLAG_UDR : Underrun flag
  *            @arg I2S_FLAG_OVR : Overrun flag
  *            @arg I2S_FLAG_FRE : TI mode frame format error flag
  * @retval SET or RESET.
  */
#define I2S_CHECK_FLAG(__SR__, __FLAG__)         ((((__SR__)\
                                                    & ((__FLAG__) & I2S_FLAG_MASK)) == ((__FLAG__) & I2S_FLAG_MASK))\
                                                  ? SET : RESET)

/** @brief  Check whether the specified SPI Interrupt is set or not.
  * @param  __IER__  copy of I2S IER register.
  * @param  __INTERRUPT__ specifies the SPI interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg I2S_IT_RXP : Rx-Packet available interrupt
  *            @arg I2S_IT_TXP : Tx-Packet space available interrupt
  *            @arg I2S_IT_UDR : Underrun interrupt
  *            @arg I2S_IT_OVR : Overrun interrupt
  *            @arg I2S_IT_FRE : TI mode frame format error interrupt
  *            @arg I2S_IT_ERR : Error interrupt enable
  * @retval SET or RESET.
  */
#define I2S_CHECK_IT_SOURCE(__IER__, __INTERRUPT__)      ((((__IER__)\
                                                            & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Checks if I2S Mode parameter is in allowed range.
  * @param  __MODE__ specifies the I2S Mode.
  *         This parameter can be a value of @ref I2S_Mode
  * @retval None
  */
#define IS_I2S_MODE(__MODE__)                       (((__MODE__) == I2S_MODE_SLAVE_TX)                   || \
                                                     ((__MODE__) == I2S_MODE_SLAVE_RX)                   || \
                                                     ((__MODE__) == I2S_MODE_MASTER_TX)                  || \
                                                     ((__MODE__) == I2S_MODE_MASTER_RX)                  || \
                                                     ((__MODE__) == I2S_MODE_SLAVE_FULLDUPLEX)           || \
                                                     ((__MODE__) == I2S_MODE_MASTER_FULLDUPLEX))

#define IS_I2S_MASTER(__MODE__)                     (((__MODE__) == I2S_MODE_MASTER_TX)                  || \
                                                     ((__MODE__) == I2S_MODE_MASTER_RX)                  || \
                                                     ((__MODE__) == I2S_MODE_MASTER_FULLDUPLEX))

#define IS_I2S_SLAVE(__MODE__)                      (((__MODE__) == I2S_MODE_SLAVE_TX)                  || \
                                                     ((__MODE__) == I2S_MODE_SLAVE_RX)                  || \
                                                     ((__MODE__) == I2S_MODE_SLAVE_FULLDUPLEX))

#define IS_I2S_FULLDUPLEX(__MODE__)                 (((__MODE__) == I2S_MODE_MASTER_FULLDUPLEX)         || \
                                                     ((__MODE__) == I2S_MODE_SLAVE_FULLDUPLEX))

#define IS_I2S_STANDARD(__STANDARD__)               (((__STANDARD__) == I2S_STANDARD_PHILIPS)            || \
                                                     ((__STANDARD__) == I2S_STANDARD_MSB)                || \
                                                     ((__STANDARD__) == I2S_STANDARD_LSB)                || \
                                                     ((__STANDARD__) == I2S_STANDARD_PCM_SHORT)          || \
                                                     ((__STANDARD__) == I2S_STANDARD_PCM_LONG))

#define IS_I2S_DATA_FORMAT(__FORMAT__)              (((__FORMAT__) == I2S_DATAFORMAT_16B)                || \
                                                     ((__FORMAT__) == I2S_DATAFORMAT_16B_EXTENDED)       || \
                                                     ((__FORMAT__) == I2S_DATAFORMAT_24B)                || \
                                                     ((__FORMAT__) == I2S_DATAFORMAT_32B))

#define IS_I2S_MCLK_OUTPUT(__OUTPUT__)              (((__OUTPUT__) == I2S_MCLKOUTPUT_ENABLE)             || \
                                                     ((__OUTPUT__) == I2S_MCLKOUTPUT_DISABLE))

#define IS_I2S_AUDIO_FREQ(__FREQ__)                ((((__FREQ__) >= I2S_AUDIOFREQ_8K)                    && \
                                                     ((__FREQ__) <= I2S_AUDIOFREQ_192K))                 || \
                                                    ((__FREQ__) == I2S_AUDIOFREQ_DEFAULT))

#define IS_I2S_CPOL(__CPOL__)                       (((__CPOL__) == I2S_CPOL_LOW)                        || \
                                                     ((__CPOL__) == I2S_CPOL_HIGH))

#define IS_I2S_FIRST_BIT(__BIT__)                   (((__BIT__) == I2S_FIRSTBIT_MSB)                     || \
                                                     ((__BIT__) == I2S_FIRSTBIT_LSB))

#define IS_I2S_WS_INVERSION(__WSINV__)              (((__WSINV__) == I2S_WS_INVERSION_DISABLE)           || \
                                                     ((__WSINV__) == I2S_WS_INVERSION_ENABLE))

#define IS_I2S_DATA_24BIT_ALIGNMENT(__ALIGNMENT__)  (((__ALIGNMENT__) == I2S_DATA_24BIT_ALIGNMENT_RIGHT) || \
                                                     ((__ALIGNMENT__) == I2S_DATA_24BIT_ALIGNMENT_LEFT))

#define IS_I2S_MASTER_KEEP_IO_STATE(__AFCNTR__)     (((__AFCNTR__) == I2S_MASTER_KEEP_IO_STATE_DISABLE)  || \
                                                     ((__AFCNTR__) == I2S_MASTER_KEEP_IO_STATE_ENABLE))


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

#endif /* STM32N6xx_HAL_I2S_H */

