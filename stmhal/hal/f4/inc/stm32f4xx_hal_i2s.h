/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2s.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of I2S HAL module.
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
#ifndef __STM32F4xx_HAL_I2S_H
#define __STM32F4xx_HAL_I2S_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"  

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup I2S
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** 
  * @brief I2S Init structure definition  
  */
typedef struct
{
  uint32_t Mode;         /*!< Specifies the I2S operating mode.
                              This parameter can be a value of @ref I2S_Mode */

  uint32_t Standard;     /*!< Specifies the standard used for the I2S communication.
                              This parameter can be a value of @ref I2S_Standard */

  uint32_t DataFormat;   /*!< Specifies the data format for the I2S communication.
                              This parameter can be a value of @ref I2S_Data_Format */

  uint32_t MCLKOutput;   /*!< Specifies whether the I2S MCLK output is enabled or not.
                              This parameter can be a value of @ref I2S_MCLK_Output */

  uint32_t AudioFreq;    /*!< Specifies the frequency selected for the I2S communication.
                              This parameter can be a value of @ref I2S_Audio_Frequency */

  uint32_t CPOL;         /*!< Specifies the idle state of the I2S clock.
                              This parameter can be a value of @ref I2S_Clock_Polarity */

  uint32_t ClockSource;  /*!< Specifies the I2S Clock Source.
                              This parameter can be a value of @ref I2S_Clock_Source */

  uint32_t FullDuplexMode;  /*!< Specifies the I2S FullDuplex mode.
                                 This parameter can be a value of @ref I2S_FullDuplex_Mode */

}I2S_InitTypeDef;

/** 
  * @brief  HAL State structures definition
  */ 
typedef enum
{
  HAL_I2S_STATE_RESET      = 0x00,  /*!< I2S not yet initialized or disabled                */
  HAL_I2S_STATE_READY      = 0x01,  /*!< I2S initialized and ready for use                  */
  HAL_I2S_STATE_BUSY       = 0x02,  /*!< I2S internal process is ongoing                    */
  HAL_I2S_STATE_BUSY_TX    = 0x12,  /*!< Data Transmission process is ongoing               */
  HAL_I2S_STATE_BUSY_RX    = 0x22,  /*!< Data Reception process is ongoing                  */
  HAL_I2S_STATE_BUSY_TX_RX = 0x32,  /*!< Data Transmission and Reception process is ongoing */
  HAL_I2S_STATE_TIMEOUT    = 0x03,  /*!< I2S timeout state                                  */
  HAL_I2S_STATE_ERROR      = 0x04   /*!< I2S error state                                    */

}HAL_I2S_StateTypeDef;

/** 
  * @brief  HAL I2S Error Code structure definition  
  */ 
typedef enum
{
  HAL_I2S_ERROR_NONE      = 0x00,    /*!< No error                    */
  HAL_I2S_ERROR_UDR       = 0x01,    /*!< I2S Underrun error          */
  HAL_I2S_ERROR_OVR       = 0x02,    /*!< I2S Overrun error           */
  HAL_I2SEX_ERROR_UDR     = 0x04,    /*!< I2S extended Underrun error */
  HAL_I2SEX_ERROR_OVR     = 0x08,    /*!< I2S extended Overrun error  */
  HAL_I2S_ERROR_FRE       = 0x10,    /*!< I2S Frame format error      */
  HAL_I2S_ERROR_DMA       = 0x20     /*!< DMA transfer error          */
}HAL_I2S_ErrorTypeDef;

/** 
  * @brief I2S handle Structure definition  
  */
typedef struct
{
  SPI_TypeDef                *Instance;    /* I2S registers base address        */

  I2S_InitTypeDef            Init;         /* I2S communication parameters      */
  
  uint16_t                   *pTxBuffPtr;  /* Pointer to I2S Tx transfer buffer */
  
  __IO uint16_t              TxXferSize;   /* I2S Tx transfer size              */
  
  __IO uint16_t              TxXferCount;  /* I2S Tx transfer Counter           */
  
  uint16_t                   *pRxBuffPtr;  /* Pointer to I2S Rx transfer buffer */
  
  __IO uint16_t              RxXferSize;   /* I2S Rx transfer size              */
  
  __IO uint16_t              RxXferCount;  /* I2S Rx transfer counter           */

  DMA_HandleTypeDef          *hdmatx;      /* I2S Tx DMA handle parameters      */

  DMA_HandleTypeDef          *hdmarx;      /* I2S Rx DMA handle parameters      */
  
  __IO HAL_LockTypeDef       Lock;         /* I2S locking object                */
  
  __IO HAL_I2S_StateTypeDef  State;        /* I2S communication state           */
  
  __IO HAL_I2S_ErrorTypeDef  ErrorCode;    /* I2S Error code                    */

}I2S_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup I2S_Exported_Constants
  * @{
  */
#define I2SxEXT(__INSTANCE__) ((__INSTANCE__) == (SPI2)? (SPI_TypeDef *)(I2S2ext_BASE): (SPI_TypeDef *)(I2S3ext_BASE)) 

/** @defgroup I2S_Clock_Source 
  * @{
  */
#define I2S_CLOCK_PLL                     ((uint32_t)0x00000000)
#define I2S_CLOCK_EXTERNAL                ((uint32_t)0x00000001)

#define IS_I2S_CLOCKSOURCE(CLOCK) (((CLOCK) == I2S_CLOCK_EXTERNAL) || \
                                   ((CLOCK) == I2S_CLOCK_PLL))
/**
  * @}
  */

/** @defgroup I2S_Mode 
  * @{
  */
#define I2S_MODE_SLAVE_TX                ((uint32_t)0x00000000)
#define I2S_MODE_SLAVE_RX                ((uint32_t)0x00000100)
#define I2S_MODE_MASTER_TX               ((uint32_t)0x00000200)
#define I2S_MODE_MASTER_RX               ((uint32_t)0x00000300)

#define IS_I2S_MODE(MODE) (((MODE) == I2S_MODE_SLAVE_TX)  || \
                           ((MODE) == I2S_MODE_SLAVE_RX)  || \
                           ((MODE) == I2S_MODE_MASTER_TX) || \
                           ((MODE) == I2S_MODE_MASTER_RX))
/**
  * @}
  */
  
/** @defgroup I2S_Standard 
  * @{
  */
#define I2S_STANDARD_PHILIPS             ((uint32_t)0x00000000)
#define I2S_STANDARD_MSB                 ((uint32_t)0x00000010)
#define I2S_STANDARD_LSB                 ((uint32_t)0x00000020)
#define I2S_STANDARD_PCM_SHORT           ((uint32_t)0x00000030)
#define I2S_STANDARD_PCM_LONG            ((uint32_t)0x000000B0)

#define IS_I2S_STANDARD(STANDARD) (((STANDARD) == I2S_STANDARD_PHILIPS)  || \
                                   ((STANDARD) == I2S_STANDARD_MSB)       || \
                                   ((STANDARD) == I2S_STANDARD_LSB)       || \
                                   ((STANDARD) == I2S_STANDARD_PCM_SHORT) || \
                                   ((STANDARD) == I2S_STANDARD_PCM_LONG))
/** @defgroup I2S_Legacy 
  * @{
  */
#define I2S_STANDARD_PHILLIPS      I2S_STANDARD_PHILIPS
/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup I2S_Data_Format 
  * @{
  */
#define I2S_DATAFORMAT_16B               ((uint32_t)0x00000000)
#define I2S_DATAFORMAT_16B_EXTENDED      ((uint32_t)0x00000001)
#define I2S_DATAFORMAT_24B               ((uint32_t)0x00000003)
#define I2S_DATAFORMAT_32B               ((uint32_t)0x00000005)

#define IS_I2S_DATA_FORMAT(FORMAT) (((FORMAT) == I2S_DATAFORMAT_16B)          || \
                                    ((FORMAT) == I2S_DATAFORMAT_16B_EXTENDED) || \
                                    ((FORMAT) == I2S_DATAFORMAT_24B)          || \
                                    ((FORMAT) == I2S_DATAFORMAT_32B))
/**
  * @}
  */

/** @defgroup I2S_MCLK_Output 
  * @{
  */
#define I2S_MCLKOUTPUT_ENABLE           ((uint32_t)SPI_I2SPR_MCKOE)
#define I2S_MCLKOUTPUT_DISABLE          ((uint32_t)0x00000000)

#define IS_I2S_MCLK_OUTPUT(OUTPUT) (((OUTPUT) == I2S_MCLKOUTPUT_ENABLE) || \
                                    ((OUTPUT) == I2S_MCLKOUTPUT_DISABLE))
/**
  * @}
  */

/** @defgroup I2S_Audio_Frequency 
  * @{
  */
#define I2S_AUDIOFREQ_192K               ((uint32_t)192000)
#define I2S_AUDIOFREQ_96K                ((uint32_t)96000)
#define I2S_AUDIOFREQ_48K                ((uint32_t)48000)
#define I2S_AUDIOFREQ_44K                ((uint32_t)44100)
#define I2S_AUDIOFREQ_32K                ((uint32_t)32000)
#define I2S_AUDIOFREQ_22K                ((uint32_t)22050)
#define I2S_AUDIOFREQ_16K                ((uint32_t)16000)
#define I2S_AUDIOFREQ_11K                ((uint32_t)11025)
#define I2S_AUDIOFREQ_8K                 ((uint32_t)8000)
#define I2S_AUDIOFREQ_DEFAULT            ((uint32_t)2)

#define IS_I2S_AUDIO_FREQ(FREQ) ((((FREQ) >= I2S_AUDIOFREQ_8K) && \
                                  ((FREQ) <= I2S_AUDIOFREQ_192K)) || \
                                  ((FREQ) == I2S_AUDIOFREQ_DEFAULT))
/**
  * @}
  */

/** @defgroup I2S_FullDuplex_Mode 
  * @{
  */
#define I2S_FULLDUPLEXMODE_DISABLE                   ((uint32_t)0x00000000)
#define I2S_FULLDUPLEXMODE_ENABLE                    ((uint32_t)0x00000001)

#define IS_I2S_FULLDUPLEX_MODE(MODE) (((MODE) == I2S_FULLDUPLEXMODE_DISABLE) || \
                                      ((MODE) == I2S_FULLDUPLEXMODE_ENABLE))
/**
  * @}
  */

/** @defgroup I2S_Clock_Polarity 
  * @{
  */
#define I2S_CPOL_LOW                    ((uint32_t)0x00000000)
#define I2S_CPOL_HIGH                   ((uint32_t)SPI_I2SCFGR_CKPOL)

#define IS_I2S_CPOL(CPOL) (((CPOL) == I2S_CPOL_LOW) || \
                           ((CPOL) == I2S_CPOL_HIGH))
/**
  * @}
  */

/** @defgroup I2S_Interrupt_configuration_definition
  * @{
  */
#define I2S_IT_TXE                      SPI_CR2_TXEIE
#define I2S_IT_RXNE                     SPI_CR2_RXNEIE
#define I2S_IT_ERR                      SPI_CR2_ERRIE
/**
  * @}
  */

/** @defgroup I2S_Flag_definition 
  * @{
  */
#define I2S_FLAG_TXE                    SPI_SR_TXE
#define I2S_FLAG_RXNE                   SPI_SR_RXNE

#define I2S_FLAG_UDR                    SPI_SR_UDR
#define I2S_FLAG_OVR                    SPI_SR_OVR
#define I2S_FLAG_FRE                    SPI_SR_FRE

#define I2S_FLAG_CHSIDE                 SPI_SR_CHSIDE
#define I2S_FLAG_BSY                    SPI_SR_BSY
/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/


/** @brief Reset I2S handle state
  * @param  __HANDLE__: specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_I2S_STATE_RESET)

/** @brief  Enable or disable the specified SPI peripheral (in I2S mode).
  * @param  __HANDLE__: specifies the I2S Handle. 
  * @retval None
  */
#define __HAL_I2S_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->I2SCFGR |= SPI_I2SCFGR_I2SE)
#define __HAL_I2S_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->I2SCFGR &= ~SPI_I2SCFGR_I2SE)

/** @brief  Enable or disable the specified I2S interrupts.
  * @param  __HANDLE__: specifies the I2S Handle.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg I2S_IT_TXE: Tx buffer empty interrupt enable
  *            @arg I2S_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg I2S_IT_ERR: Error interrupt enable
  * @retval None
  */  
#define __HAL_I2S_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR2 |= (__INTERRUPT__))
#define __HAL_I2S_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR2 &= ~(__INTERRUPT__))
 
/** @brief  Checks if the specified I2S interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the I2S Handle.
  *         This parameter can be I2S where x: 1, 2, or 3 to select the I2S peripheral.
  * @param  __INTERRUPT__: specifies the I2S interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg I2S_IT_TXE: Tx buffer empty interrupt enable
  *            @arg I2S_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg I2S_IT_ERR: Error interrupt enable
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_I2S_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR2 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Checks whether the specified I2S flag is set or not.
  * @param  __HANDLE__: specifies the I2S Handle.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg I2S_FLAG_RXNE: Receive buffer not empty flag
  *            @arg I2S_FLAG_TXE: Transmit buffer empty flag
  *            @arg I2S_FLAG_UDR: Underrun flag
  *            @arg I2S_FLAG_OVR: Overrun flag
  *            @arg I2S_FLAG_FRE: Frame error flag
  *            @arg I2S_FLAG_CHSIDE: Channel Side flag
  *            @arg I2S_FLAG_BSY: Busy flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_I2S_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief Clears the I2S OVR pending flag.
  * @param  __HANDLE__: specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_CLEAR_OVRFLAG(__HANDLE__) do{(__HANDLE__)->Instance->DR;\
                                               (__HANDLE__)->Instance->SR;}while(0)
/** @brief Clears the I2S UDR pending flag.
  * @param  __HANDLE__: specifies the I2S Handle.
  * @retval None
  */
#define __HAL_I2S_CLEAR_UDRFLAG(__HANDLE__)((__HANDLE__)->Instance->SR)

/* Include I2S Extension module */
#include "stm32f4xx_hal_i2s_ex.h"

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DeInit (I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s);

/* I/O operation functions  *****************************************************/
 /* Blocking mode: Polling */
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout);

 /* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s);

/* Peripheral Control and State functions  **************************************/
HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s);
HAL_I2S_ErrorTypeDef HAL_I2S_GetError(I2S_HandleTypeDef *hi2s);

/* Callbacks used in non blocking modes (Interrupt and DMA) *******************/
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);

void              I2S_DMATxCplt(DMA_HandleTypeDef *hdma);
void              I2S_DMATxHalfCplt(DMA_HandleTypeDef *hdma); 
void              I2S_DMARxCplt(DMA_HandleTypeDef *hdma);
void              I2S_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
void              I2S_DMAError(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef I2S_WaitFlagStateUntilTimeout(I2S_HandleTypeDef *hi2s, uint32_t Flag, uint32_t Status, uint32_t Timeout);

/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_HAL_I2S_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
