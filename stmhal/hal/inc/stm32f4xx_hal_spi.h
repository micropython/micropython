/**
  ******************************************************************************
  * @file    stm32f4xx_hal_spi.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of SPI HAL module.
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
#ifndef __STM32F4xx_HAL_SPI_H
#define __STM32F4xx_HAL_SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"  

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup SPI
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  SPI Configuration Structure definition  
  */
typedef struct
{
  uint32_t Mode;               /*!< Specifies the SPI operating mode.
                                    This parameter can be a value of @ref SPI_mode */

  uint32_t Direction;          /*!< Specifies the SPI Directional mode state.
                                    This parameter can be a value of @ref SPI_Direction_mode */

  uint32_t DataSize;           /*!< Specifies the SPI data size.
                                    This parameter can be a value of @ref SPI_data_size */

  uint32_t CLKPolarity;        /*!< Specifies the serial clock steady state.
                                    This parameter can be a value of @ref SPI_Clock_Polarity */

  uint32_t CLKPhase;           /*!< Specifies the clock active edge for the bit capture.
                                    This parameter can be a value of @ref SPI_Clock_Phase */

  uint32_t NSS;                /*!< Specifies whether the NSS signal is managed by
                                    hardware (NSS pin) or by software using the SSI bit.
                                    This parameter can be a value of @ref SPI_Slave_Select_management */

  uint32_t BaudRatePrescaler;  /*!< Specifies the Baud Rate prescaler value which will be
                                    used to configure the transmit and receive SCK clock.
                                    This parameter can be a value of @ref SPI_BaudRate_Prescaler
                                    @note The communication clock is derived from the master
                                    clock. The slave clock does not need to be set */

  uint32_t FirstBit;           /*!< Specifies whether data transfers start from MSB or LSB bit.
                                    This parameter can be a value of @ref SPI_MSB_LSB_transmission */

  uint32_t TIMode;             /*!< Specifies if the TI mode is enabled or not.
                                    This parameter can be a value of @ref SPI_TI_mode */

  uint32_t CRCCalculation;     /*!< Specifies if the CRC calculation is enabled or not.
                                    This parameter can be a value of @ref SPI_CRC_Calculation */

  uint32_t CRCPolynomial;      /*!< Specifies the polynomial used for the CRC calculation.
                                    This parameter must be a number between Min_Data = 0 and Max_Data = 65535 */

}SPI_InitTypeDef;

/**
  * @brief  HAL SPI State structure definition
  */
typedef enum
{
  HAL_SPI_STATE_RESET      = 0x00,  /*!< SPI not yet initialized or disabled                */
  HAL_SPI_STATE_READY      = 0x01,  /*!< SPI initialized and ready for use                  */
  HAL_SPI_STATE_BUSY       = 0x02,  /*!< SPI process is ongoing                             */
  HAL_SPI_STATE_BUSY_TX    = 0x12,  /*!< Data Transmission process is ongoing               */
  HAL_SPI_STATE_BUSY_RX    = 0x22,  /*!< Data Reception process is ongoing                  */
  HAL_SPI_STATE_BUSY_TX_RX = 0x32,  /*!< Data Transmission and Reception process is ongoing */
  HAL_SPI_STATE_ERROR      = 0x03   /*!< SPI error state                                    */
    
}HAL_SPI_StateTypeDef;

/** 
  * @brief  HAL SPI Error Code structure definition  
  */ 
typedef enum
{
  HAL_SPI_ERROR_NONE      = 0x00,    /*!< No error             */
  HAL_SPI_ERROR_MODF      = 0x01,    /*!< MODF error           */
  HAL_SPI_ERROR_CRC       = 0x02,    /*!< CRC error            */
  HAL_SPI_ERROR_OVR       = 0x04,    /*!< OVR error            */
  HAL_SPI_ERROR_FRE       = 0x08,    /*!< FRE error            */
  HAL_SPI_ERROR_DMA       = 0x10,    /*!< DMA transfer error   */
  HAL_SPI_ERROR_FLAG      = 0x20     /*!< Flag: RXNE,TXE, BSY  */

}HAL_SPI_ErrorTypeDef;

/** 
  * @brief  SPI handle Structure definition
  */
typedef struct __SPI_HandleTypeDef
{
  SPI_TypeDef                *Instance;    /* SPI registers base address */

  SPI_InitTypeDef            Init;         /* SPI communication parameters */

  uint8_t                    *pTxBuffPtr;  /* Pointer to SPI Tx transfer Buffer */

  uint16_t                   TxXferSize;   /* SPI Tx transfer size */
  
  uint16_t                   TxXferCount;  /* SPI Tx Transfer Counter */

  uint8_t                    *pRxBuffPtr;  /* Pointer to SPI Rx transfer Buffer */

  uint16_t                   RxXferSize;   /* SPI Rx transfer size */

  uint16_t                   RxXferCount;  /* SPI Rx Transfer Counter */

  DMA_HandleTypeDef          *hdmatx;      /* SPI Tx DMA handle parameters */

  DMA_HandleTypeDef          *hdmarx;      /* SPI Rx DMA handle parameters */

  void                       (*RxISR)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Rx ISR */

  void                       (*TxISR)(struct __SPI_HandleTypeDef * hspi); /* function pointer on Tx ISR */

  HAL_LockTypeDef            Lock;         /* SPI locking object */

  __IO HAL_SPI_StateTypeDef  State;        /* SPI communication state */

  __IO HAL_SPI_ErrorTypeDef  ErrorCode;         /* SPI Error code */

}SPI_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup SPI_Exported_Constants
  * @{
  */

/** @defgroup SPI_mode 
  * @{
  */
#define SPI_MODE_SLAVE                  ((uint32_t)0x00000000)
#define SPI_MODE_MASTER                 (SPI_CR1_MSTR | SPI_CR1_SSI)

#define IS_SPI_MODE(MODE) (((MODE) == SPI_MODE_SLAVE) || \
                           ((MODE) == SPI_MODE_MASTER))
/**
  * @}
  */

/** @defgroup SPI_Direction_mode 
  * @{
  */
#define SPI_DIRECTION_2LINES             ((uint32_t)0x00000000)
#define SPI_DIRECTION_2LINES_RXONLY      SPI_CR1_RXONLY
#define SPI_DIRECTION_1LINE              SPI_CR1_BIDIMODE

#define IS_SPI_DIRECTION_MODE(MODE) (((MODE) == SPI_DIRECTION_2LINES)        || \
                                     ((MODE) == SPI_DIRECTION_2LINES_RXONLY) || \
                                     ((MODE) == SPI_DIRECTION_1LINE))

#define IS_SPI_DIRECTION_2LINES_OR_1LINE(MODE) (((MODE) == SPI_DIRECTION_2LINES)  || \
                                                ((MODE) == SPI_DIRECTION_1LINE))

#define IS_SPI_DIRECTION_2LINES(MODE) ((MODE) == SPI_DIRECTION_2LINES)

/**
  * @}
  */

/** @defgroup SPI_data_size 
  * @{
  */
#define SPI_DATASIZE_8BIT               ((uint32_t)0x00000000)
#define SPI_DATASIZE_16BIT              SPI_CR1_DFF

#define IS_SPI_DATASIZE(DATASIZE) (((DATASIZE) == SPI_DATASIZE_16BIT) || \
                                   ((DATASIZE) == SPI_DATASIZE_8BIT))
/**
  * @}
  */ 

/** @defgroup SPI_Clock_Polarity 
  * @{
  */
#define SPI_POLARITY_LOW                ((uint32_t)0x00000000)
#define SPI_POLARITY_HIGH               SPI_CR1_CPOL

#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_POLARITY_LOW) || \
                           ((CPOL) == SPI_POLARITY_HIGH))
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase 
  * @{
  */
#define SPI_PHASE_1EDGE                 ((uint32_t)0x00000000)
#define SPI_PHASE_2EDGE                 SPI_CR1_CPHA

#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_PHASE_1EDGE) || \
                           ((CPHA) == SPI_PHASE_2EDGE))
/**
  * @}
  */

/** @defgroup SPI_Slave_Select_management 
  * @{
  */
#define SPI_NSS_SOFT                    SPI_CR1_SSM
#define SPI_NSS_HARD_INPUT              ((uint32_t)0x00000000)
#define SPI_NSS_HARD_OUTPUT             ((uint32_t)0x00040000)

#define IS_SPI_NSS(NSS) (((NSS) == SPI_NSS_SOFT)       || \
                         ((NSS) == SPI_NSS_HARD_INPUT) || \
                         ((NSS) == SPI_NSS_HARD_OUTPUT))
/**
  * @}
  */ 

/** @defgroup SPI_BaudRate_Prescaler 
  * @{
  */
#define SPI_BAUDRATEPRESCALER_2         ((uint32_t)0x00000000)
#define SPI_BAUDRATEPRESCALER_4         ((uint32_t)0x00000008)
#define SPI_BAUDRATEPRESCALER_8         ((uint32_t)0x00000010)
#define SPI_BAUDRATEPRESCALER_16        ((uint32_t)0x00000018)
#define SPI_BAUDRATEPRESCALER_32        ((uint32_t)0x00000020)
#define SPI_BAUDRATEPRESCALER_64        ((uint32_t)0x00000028)
#define SPI_BAUDRATEPRESCALER_128       ((uint32_t)0x00000030)
#define SPI_BAUDRATEPRESCALER_256       ((uint32_t)0x00000038)

#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER) (((PRESCALER) == SPI_BAUDRATEPRESCALER_2)   || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_4)   || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_8)   || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_16)  || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_32)  || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_64)  || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_128) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_256))
/**
  * @}
  */ 

/** @defgroup SPI_MSB_LSB_transmission 
  * @{
  */
#define SPI_FIRSTBIT_MSB                ((uint32_t)0x00000000)
#define SPI_FIRSTBIT_LSB                SPI_CR1_LSBFIRST

#define IS_SPI_FIRST_BIT(BIT) (((BIT) == SPI_FIRSTBIT_MSB) || \
                               ((BIT) == SPI_FIRSTBIT_LSB))
/**
  * @}
  */

/** @defgroup SPI_TI_mode 
  * @{
  */
#define SPI_TIMODE_DISABLED             ((uint32_t)0x00000000)
#define SPI_TIMODE_ENABLED              SPI_CR2_FRF

#define IS_SPI_TIMODE(MODE) (((MODE) == SPI_TIMODE_DISABLED) || \
                             ((MODE) == SPI_TIMODE_ENABLED))
/**
  * @}
  */

/** @defgroup SPI_CRC_Calculation 
  * @{
  */
#define SPI_CRCCALCULATION_DISABLED     ((uint32_t)0x00000000)
#define SPI_CRCCALCULATION_ENABLED      SPI_CR1_CRCEN

#define IS_SPI_CRC_CALCULATION(CALCULATION) (((CALCULATION) == SPI_CRCCALCULATION_DISABLED) || \
                                             ((CALCULATION) == SPI_CRCCALCULATION_ENABLED))
/**
  * @}
  */

/** @defgroup SPI_Interrupt_configuration_definition
  * @{
  */
#define SPI_IT_TXE                      SPI_CR2_TXEIE
#define SPI_IT_RXNE                     SPI_CR2_RXNEIE
#define SPI_IT_ERR                      SPI_CR2_ERRIE
/**
  * @}
  */

/** @defgroup SPI_Flag_definition 
  * @{
  */
#define SPI_FLAG_RXNE                   SPI_SR_RXNE
#define SPI_FLAG_TXE                    SPI_SR_TXE
#define SPI_FLAG_CRCERR                 SPI_SR_CRCERR
#define SPI_FLAG_MODF                   SPI_SR_MODF
#define SPI_FLAG_OVR                    SPI_SR_OVR
#define SPI_FLAG_BSY                    SPI_SR_BSY
#define SPI_FLAG_FRE                    SPI_SR_FRE

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief  Enable or disable the specified SPI interrupts.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *            @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *            @arg SPI_IT_ERR: Error interrupt enable
  * @retval None
  */
#define __HAL_SPI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CR2 |= (__INTERRUPT__))
#define __HAL_SPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CR2 &= (~(__INTERRUPT__)))

/** @brief  Check if the specified SPI interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the SPI interrupt source to check.
  *          This parameter can be one of the following values:
  *             @arg SPI_IT_TXE: Tx buffer empty interrupt enable
  *             @arg SPI_IT_RXNE: RX buffer not empty interrupt enable
  *             @arg SPI_IT_ERR: Error interrupt enable
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR2 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SPI flag is set or not.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SPI_FLAG_RXNE: Receive buffer not empty flag
  *            @arg SPI_FLAG_TXE: Transmit buffer empty flag
  *            @arg SPI_FLAG_CRCERR: CRC error flag
  *            @arg SPI_FLAG_MODF: Mode fault flag
  *            @arg SPI_FLAG_OVR: Overrun flag
  *            @arg SPI_FLAG_BSY: Busy flag
  *            @arg SPI_FLAG_FRE: Frame format error flag  
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the SPI CRCERR pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_CLEAR_CRCERRFLAG(__HANDLE__) ((__HANDLE__)->Instance->SR &= ~(SPI_FLAG_CRCERR))

/** @brief  Clear the SPI MODF pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral. 
  * @retval None
  */
#define __HAL_SPI_CLEAR_MODFFLAG(__HANDLE__) do{(__HANDLE__)->Instance->SR;\
                                                (__HANDLE__)->Instance->CR1 &= (~SPI_CR1_SPE);}while(0) 

/** @brief  Clear the SPI OVR pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral. 
  * @retval None
  */
#define __HAL_SPI_CLEAR_OVRFLAG(__HANDLE__) do{(__HANDLE__)->Instance->DR;\
                                               (__HANDLE__)->Instance->SR;}while(0) 

/** @brief  Clear the SPI FRE pending flag.
  * @param  __HANDLE__: specifies the SPI handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define __HAL_SPI_CLEAR_FREFLAG(__HANDLE__) ((__HANDLE__)->Instance->SR)

#define __HAL_SPI_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 |=  SPI_CR1_SPE)
#define __HAL_SPI_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 &=  ~SPI_CR1_SPE)

#define IS_SPI_CRC_POLYNOMIAL(POLYNOMIAL) (((POLYNOMIAL) >= 0x1) && ((POLYNOMIAL) <= 0xFFFF))

#define __HAL_SPI_1LINE_TX(__HANDLE__) ((__HANDLE__)->Instance->CR1 |= SPI_CR1_BIDIOE)

#define __HAL_SPI_1LINE_RX(__HANDLE__) ((__HANDLE__)->Instance->CR1 &= ~SPI_CR1_BIDIOE) 

#define __HAL_SPI_RESET_CRC(__HANDLE__) do{(__HANDLE__)->Instance->CR1 &= (~SPI_CR1_CRCEN);\
                                           (__HANDLE__)->Instance->CR1 |= SPI_CR1_CRCEN;}while(0)

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit (SPI_HandleTypeDef *hspi);
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);

/* I/O operation functions  *****************************************************/
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);

/* Peripheral State and Control functions  **************************************/
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi);
HAL_SPI_ErrorTypeDef HAL_SPI_GetError(SPI_HandleTypeDef *hspi);

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_SPI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
