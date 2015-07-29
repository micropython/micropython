/**
  ******************************************************************************
  * @file    stm32f7xx_hal_spi.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   SPI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Serial Peripheral Interface (SPI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
      The SPI HAL driver can be used as follows:

      (#) Declare a SPI_HandleTypeDef handle structure, for example:
          SPI_HandleTypeDef  hspi;

      (#)Initialize the SPI low level resources by implementing the HAL_SPI_MspInit ()API:
          (##) Enable the SPIx interface clock
          (##) SPI pins configuration
              (+++) Enable the clock for the SPI GPIOs
              (+++) Configure these SPI pins as alternate function push-pull
          (##) NVIC configuration if you need to use interrupt process
              (+++) Configure the SPIx interrupt priority
              (+++) Enable the NVIC SPI IRQ handle
          (##) DMA Configuration if you need to use DMA process
              (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive channel
              (+++) Enable the DMAx clock
              (+++) Configure the DMA handle parameters
              (+++) Configure the DMA Tx or Rx channel
              (+++) Associate the initialized hdma_tx handle to the hspi DMA Tx or Rx handle
              (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx or Rx channel

      (#) Program the Mode, BidirectionalMode , Data size, Baudrate Prescaler, NSS
          management, Clock polarity and phase, FirstBit and CRC configuration in the hspi Init structure.

      (#) Initialize the SPI registers by calling the HAL_SPI_Init() API:
          (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
              by calling the customised HAL_SPI_MspInit() API.
     [..]
       Circular mode restriction:
      (#) The DMA circular mode cannot be used when the SPI is configured in these modes:
          (##) Master 2Lines RxOnly
          (##) Master 1Line Rx
      (#) The CRC feature is not managed when the DMA circular mode is enabled
      (#) When the SPI DMA Pause/Stop features are used, we must use the following APIs
          the HAL_SPI_DMAPause()/ HAL_SPI_DMAStop() only under the SPI callbacks

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup SPI SPI
  * @brief SPI HAL module driver
  * @{
  */
#ifdef HAL_SPI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup SPI_Private_Constants SPI Private Constants
  * @{
  */
#define SPI_DEFAULT_TIMEOUT 50
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SPI_Private_Functions
  * @{
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAError(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef SPI_WaitFlagStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, uint32_t State, uint32_t Timeout);
static HAL_StatusTypeDef SPI_WaitFifoStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Fifo, uint32_t State, uint32_t Timeout);
static void SPI_TxISR_8BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_TxISR_16BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_RxISR_8BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_RxISR_8BITCRC(struct __SPI_HandleTypeDef *hspi);
static void SPI_RxISR_16BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_RxISR_16BITCRC(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesRxISR_8BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesRxISR_8BITCRC(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesTxISR_8BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesTxISR_16BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesRxISR_16BIT(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesRxISR_16BITCRC(struct __SPI_HandleTypeDef *hspi);
static void SPI_CloseRxTx_ISR(SPI_HandleTypeDef *hspi);
static void SPI_CloseRx_ISR(SPI_HandleTypeDef *hspi);
static void SPI_CloseTx_ISR(SPI_HandleTypeDef *hspi);
static HAL_StatusTypeDef SPI_EndRxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout);
static HAL_StatusTypeDef SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup SPI_Exported_Functions SPI Exported Functions
  * @{
  */

/** @defgroup SPI_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          de-initialize the SPIx peripheral:

      (+) User must implement HAL_SPI_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_SPI_Init() to configure the selected device with
          the selected configuration:
        (++) Mode
        (++) Direction
        (++) Data Size
        (++) Clock Polarity and Phase
        (++) NSS Management
        (++) BaudRate Prescaler
        (++) FirstBit
        (++) TIMode
        (++) CRC Calculation
        (++) CRC Polynomial if CRC enabled
        (++) CRC Length, used only with Data8 and Data16
        (++) FIFO reception threshold

      (+) Call the function HAL_SPI_DeInit() to restore the default configuration
          of the selected SPIx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the SPI according to the specified parameters
  *         in the SPI_InitTypeDef and create the associated handle.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
  uint32_t frxth;

  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
  assert_param(IS_SPI_MODE(hspi->Init.Mode));
  assert_param(IS_SPI_DIRECTION(hspi->Init.Direction));
  assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
  assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
  assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
  assert_param(IS_SPI_NSS(hspi->Init.NSS));
  assert_param(IS_SPI_NSSP(hspi->Init.NSSPMode));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
  assert_param(IS_SPI_TIMODE(hspi->Init.TIMode));
  assert_param(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
  assert_param(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));
  assert_param(IS_SPI_CRC_LENGTH(hspi->Init.CRCLength));

  if(hspi->State == HAL_SPI_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hspi->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_SPI_MspInit(hspi);
  }

  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disable the selected SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /* Align by default the rs fifo threshold on the data size */
  if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    frxth = SPI_RXFIFO_THRESHOLD_HF;
  }
  else
  {
    frxth = SPI_RXFIFO_THRESHOLD_QF;
  }

  /* CRC calculation is valid only for 16Bit and 8 Bit */
  if(( hspi->Init.DataSize != SPI_DATASIZE_16BIT ) && ( hspi->Init.DataSize != SPI_DATASIZE_8BIT ))
  {
    /* CRC must be disabled */
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  }

  /* Align the CRC Length on the data size */
  if( hspi->Init.CRCLength == SPI_CRC_LENGTH_DATASIZE)
  {
    /* CRC Length aligned on the data size : value set by default */
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
    {
      hspi->Init.CRCLength = SPI_CRC_LENGTH_16BIT;
    }
    else
    {
      hspi->Init.CRCLength = SPI_CRC_LENGTH_8BIT;
    }
  }

  /*---------------------------- SPIx CR1 & CR2 Configuration ------------------------*/
  /* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
  Communication speed, First bit, CRC calculation state, CRC Length */
  hspi->Instance->CR1 = (hspi->Init.Mode | hspi->Init.Direction |
                         hspi->Init.CLKPolarity | hspi->Init.CLKPhase | (hspi->Init.NSS & SPI_CR1_SSM) |
                         hspi->Init.BaudRatePrescaler | hspi->Init.FirstBit  | hspi->Init.CRCCalculation);

  if( hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT)
  {
    hspi->Instance->CR1|= SPI_CR1_CRCL;
  }

  /* Configure : NSS management */
  /* Configure : Rx Fifo Threshold */
  hspi->Instance->CR2 = (((hspi->Init.NSS >> 16) & SPI_CR2_SSOE) | hspi->Init.TIMode | hspi->Init.NSSPMode |
                         hspi->Init.DataSize ) | frxth;

  /*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Configure : CRC Polynomial */
  hspi->Instance->CRCPR = hspi->Init.CRCPolynomial;

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State= HAL_SPI_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the SPI peripheral
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));

  hspi->State = HAL_SPI_STATE_BUSY;

  /* check flag before the SPI disable */
  SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FTLVL, SPI_FTLVL_EMPTY, SPI_DEFAULT_TIMEOUT);
  SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, SPI_DEFAULT_TIMEOUT);
  SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_EMPTY, SPI_DEFAULT_TIMEOUT);

  /* Disable the SPI Peripheral Clock */
  __HAL_SPI_DISABLE(hspi);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  HAL_SPI_MspDeInit(hspi);

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_RESET;

  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief SPI MSP Init
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
 {
   /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_MspInit should be implemented in the user file
   */
}

/**
  * @brief SPI MSP DeInit
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_MspDeInit should be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SPI_Exported_Functions_Group2 IO operation functions
 *  @brief   Data transfers functions
 *
@verbatim
  ==============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    This subsection provides a set of functions allowing to manage the SPI
    data transfers.

    [..] The SPI supports master and slave mode :

    (#) There are two modes of transfer:
       (++) Blocking mode: The communication is performed in polling mode.
            The HAL status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode: The communication is performed using Interrupts
           or DMA, These APIs return the HAL status.
           The end of the data processing will be indicated through the
           dedicated SPI IRQ when using Interrupt mode or the DMA IRQ when
           using DMA mode.
           The HAL_SPI_TxCpltCallback(), HAL_SPI_RxCpltCallback() and HAL_SPI_TxRxCpltCallback() user callbacks
           will be executed respectively at the end of the transmit or Receive process
           The HAL_SPI_ErrorCallback()user callback will be executed when a communication error is detected

    (#) APIs provided for these 2 transfer modes (Blocking mode or Non blocking mode using either Interrupt or DMA)
        exist for 1Line (simplex) and 2Lines (full duplex) modes.

@endverbatim
  * @{
  */

/**
  * @brief  Transmit an amount of data in blocking mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

  /* Process Locked */
  __HAL_LOCK(hspi);

  if(hspi->State != HAL_SPI_STATE_READY)
  {
    hspi->State = HAL_SPI_STATE_READY;
   /* Process Unlocked */
   __HAL_UNLOCK(hspi);
   return HAL_BUSY;
  }

  if((pData == NULL ) || (Size == 0))
  {
    hspi->State = HAL_SPI_STATE_READY;
   /* Process Unlocked */
   __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)NULL;
  hspi->RxXferSize  = 0;
  hspi->RxXferCount = 0;

  /* Configure communication direction : 1Line */
  if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }

  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }

  /* Check if the SPI is already enabled */
  if((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Transmit data in 16 Bit mode */
  if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* Transmit data in 16 Bit mode */
    while (hspi->TxXferCount > 0)
    {
      /* Wait until TXE flag is set to send data */
      if(SPI_WaitFlagStateUntilTimeout(hspi,SPI_FLAG_TXE,SPI_FLAG_TXE,Timeout) != HAL_OK)
      {
        hspi->State = HAL_SPI_STATE_READY;
        /* Process Unlocked */
       __HAL_UNLOCK(hspi);
        return HAL_TIMEOUT;
      }
      hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
      hspi->pTxBuffPtr += sizeof(uint16_t);
      hspi->TxXferCount--;
    }
  }
  /* Transmit data in 8 Bit mode */
  else
  {
    while (hspi->TxXferCount > 0)
    {
      if(hspi->TxXferCount != 0x1)
      {
        /* Wait until TXE flag is set to send data */
        if(SPI_WaitFlagStateUntilTimeout(hspi,SPI_FLAG_TXE,SPI_FLAG_TXE,Timeout) != HAL_OK)
        {
          hspi->State = HAL_SPI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hspi);
          return HAL_TIMEOUT;
        }
        hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint16_t);
        hspi->TxXferCount -= 2;
      }
      else
      {
        /* Wait until TXE flag is set to send data */
        if(SPI_WaitFlagStateUntilTimeout(hspi,SPI_FLAG_TXE,SPI_FLAG_TXE,Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        *((__IO uint8_t*)&hspi->Instance->DR) = (*hspi->pTxBuffPtr++);
        hspi->TxXferCount--;
      }
    }
  }

  /* Enable CRC Transmission */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
     hspi->Instance->CR1|= SPI_CR1_CRCNEXT;
  }

  /* Check the end of the transaction */
  if(SPI_EndRxTxTransaction(hspi,Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Clear OVERUN flag in 2 Lines communication mode because received is not read */
  if(hspi->Init.Direction == SPI_DIRECTION_2LINES)
  {
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
  }

  hspi->State = HAL_SPI_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Receive an amount of data in blocking mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  __IO uint16_t tmpreg;

  if(hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if((pData == NULL ) || (Size == 0))
  {
    return HAL_ERROR;
  }

  if((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES))
  {
    /* the receive process is not supported in 2Lines direction master mode */
    /* in this case we call the transmitReceive process                     */
    return HAL_SPI_TransmitReceive(hspi,pData,pData,Size,Timeout);
  }

  /* Process Locked */
  __HAL_LOCK(hspi);

  hspi->State       = HAL_SPI_STATE_BUSY_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = pData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;
  hspi->pTxBuffPtr  = (uint8_t *)NULL;
  hspi->TxXferSize  = 0;
  hspi->TxXferCount = 0;

  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
    /* this is done to handle the CRCNEXT before the latest data */
    hspi->RxXferCount--;
  }

  /* Set the Rx Fido threshold */
  if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* set fiforxthreshold according the reception data length: 16bit */
    CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }
  else
  {
    /* set fiforxthreshold according the reception data length: 8bit */
    SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }

  /* Configure communication direction 1Line and enabled SPI if needed */
  if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_RX(hspi);
  }

  /* Check if the SPI is already enabled */
  if((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Receive data in 8 Bit mode */
  if(hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    while(hspi->RxXferCount > 1)
    {
      /* Wait until the RXNE flag */
      if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      (*hspi->pRxBuffPtr++)= *(__IO uint8_t *)&hspi->Instance->DR;
      hspi->RxXferCount--;
    }
  }
  else /* Receive data in 16 Bit mode */
  {
    while(hspi->RxXferCount > 1 )
    {
      /* Wait until RXNE flag is reset to read data */
      if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
      hspi->pRxBuffPtr += sizeof(uint16_t);
      hspi->RxXferCount--;
    }
  }

  /* Enable CRC Transmission */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
  }

  /* Wait until RXNE flag is set */
  if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Receive last data in 16 Bit mode */
  if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
    hspi->pRxBuffPtr += sizeof(uint16_t);
  }
  /* Receive last data in 8 Bit mode */
  else
  {
    (*hspi->pRxBuffPtr++) = *(__IO uint8_t *)&hspi->Instance->DR;
  }
  hspi->RxXferCount--;

  /* Read CRC from DR to close CRC calculation process */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait until TXE flag */
    if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
    {
      /* Error on the CRC reception */
      hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
    }
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
    {
      tmpreg = hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */
    }
    else
    {
      tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */

      if((hspi->Init.DataSize == SPI_DATASIZE_8BIT) && (hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT))
      {
        if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
        {
          /* Error on the CRC reception */
          hspi->ErrorCode|= HAL_SPI_ERROR_FLAG;
        }
        tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
        UNUSED(tmpreg); /* To avoid GCC warning */
      }
    }
  }

  /* Check the end of the transaction */
  if(SPI_EndRxTransaction(hspi,Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  hspi->State = HAL_SPI_STATE_READY;

  /* Check if CRC error occurred */
  if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
  {
    hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Transmit and Receive an amount of data in blocking mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size: amount of data to be sent and received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  __IO uint16_t tmpreg = 0;
  uint32_t tickstart = HAL_GetTick();

  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  if(hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
  {
    return HAL_ERROR;
  }


  /* Process Locked */
  __HAL_LOCK(hspi);

  hspi->State       = HAL_SPI_STATE_BUSY_TX_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = pRxData;
  hspi->RxXferCount = Size;
  hspi->RxXferSize  = Size;
  hspi->pTxBuffPtr  = pTxData;
  hspi->TxXferCount = Size;
  hspi->TxXferSize  = Size;

  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }

  /* Set the Rx Fido threshold */
  if((hspi->Init.DataSize > SPI_DATASIZE_8BIT) || (hspi->RxXferCount > 1))
  {
    /* set fiforxthreshold according the reception data length: 16bit */
    CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }
  else
  {
    /* set fiforxthreshold according the reception data length: 8bit */
    SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }

  /* Check if the SPI is already enabled */
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Transmit and Receive data in 16 Bit mode */
  if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    while ((hspi->TxXferCount > 0 ) || (hspi->RxXferCount > 0))
    {
      /* Check TXE flag */
      if((hspi->TxXferCount > 0) && ((hspi->Instance->SR & SPI_FLAG_TXE) == SPI_FLAG_TXE))
      {
        hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint16_t);
        hspi->TxXferCount--;

        /* Enable CRC Transmission */
        if((hspi->TxXferCount == 0) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
        }
      }

      /* Check RXNE flag */
      if((hspi->RxXferCount > 0) && ((hspi->Instance->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE))
      {
        *((uint16_t *)hspi->pRxBuffPtr) = hspi->Instance->DR;
        hspi->pRxBuffPtr += sizeof(uint16_t);
        hspi->RxXferCount--;
      }
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          hspi->State = HAL_SPI_STATE_READY;
          __HAL_UNLOCK(hspi);
          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Transmit and Receive data in 8 Bit mode */
  else
  {
    while((hspi->TxXferCount > 0) || (hspi->RxXferCount > 0))
    {
      /* check TXE flag */
      if((hspi->TxXferCount > 0) && ((hspi->Instance->SR & SPI_FLAG_TXE) == SPI_FLAG_TXE))
      {
        if(hspi->TxXferCount > 1)
        {
          hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr += sizeof(uint16_t);
          hspi->TxXferCount -= 2;
        }
        else
        {
          *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr++);
          hspi->TxXferCount--;
        }

        /* Enable CRC Transmission */
        if((hspi->TxXferCount == 0) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
        }
      }

      /* Wait until RXNE flag is reset */
      if((hspi->RxXferCount > 0) && ((hspi->Instance->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE))
      {
        if(hspi->RxXferCount > 1)
        {
          *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
          hspi->pRxBuffPtr += sizeof(uint16_t);
          hspi->RxXferCount -= 2;
          if(hspi->RxXferCount <= 1)
          {
            /* set fiforxthreshold before to switch on 8 bit data size */
            SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
          }
        }
        else
        {
          (*hspi->pRxBuffPtr++) =  *(__IO uint8_t *)&hspi->Instance->DR;
          hspi->RxXferCount--;
        }
      }
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          hspi->State = HAL_SPI_STATE_READY;
          __HAL_UNLOCK(hspi);
          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* Read CRC from DR to close CRC calculation process */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait until TXE flag */
    if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
    {
      /* Error on the CRC reception */
      hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
    }

    if(hspi->Init.DataSize == SPI_DATASIZE_16BIT)
    {
      tmpreg = hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */
    }
    else
    {
      tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */

      if(hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT)
      {
        if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, Timeout) != HAL_OK)
        {
          /* Error on the CRC reception */
          hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
        }
        tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
        UNUSED(tmpreg); /* To avoid GCC warning */
      }
    }
  }

  /* Check the end of the transaction */
  if(SPI_EndRxTxTransaction(hspi,Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  hspi->State = HAL_SPI_STATE_READY;

  /* Check if CRC error occurred */
  if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
  {
    hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
    /* Clear CRC Flag */
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    return HAL_ERROR;
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Transmit an amount of data in no-blocking mode with Interrupt
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hspi);

    hspi->State       = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    hspi->pTxBuffPtr  = pData;
    hspi->TxXferSize  = Size;
    hspi->TxXferCount = Size;
    hspi->pRxBuffPtr  = NULL;
    hspi->RxXferSize  = 0;
    hspi->RxXferCount = 0;

    /* Set the function for IT treatement */
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT )
    {
      hspi->RxISR = NULL;
      hspi->TxISR = SPI_TxISR_16BIT;
    }
    else
    {
      hspi->RxISR = NULL;
      hspi->TxISR = SPI_TxISR_8BIT;
    }

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      SPI_1LINE_TX(hspi);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      SPI_RESET_CRC(hspi);
    }

    /* Enable TXE and ERR interrupt */
    __HAL_SPI_ENABLE_IT(hspi,(SPI_IT_TXE));

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    /* Note : The SPI must be enabled after unlocking current process
              to avoid the risk of SPI interrupt handle execution before current
              process unlock */

    /* Check if the SPI is already enabled */
    if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive an amount of data in no-blocking mode with Interrupt
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Configure communication */
    hspi->State       = HAL_SPI_STATE_BUSY_RX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    hspi->pRxBuffPtr  = pData;
    hspi->RxXferSize  = Size;
    hspi->RxXferCount = Size;
    hspi->pTxBuffPtr  = NULL;
    hspi->TxXferSize  = 0;
    hspi->TxXferCount = 0;

    if((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES))
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hspi);
      /* the receive process is not supported in 2Lines direction master mode */
      /* in this we call the transmitReceive process          */
      return HAL_SPI_TransmitReceive_IT(hspi,pData,pData,Size);
    }

    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->CRCSize = 1;
      if((hspi->Init.DataSize <= SPI_DATASIZE_8BIT) && (hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT))
      {
        hspi->CRCSize = 2;
      }
    }
    else
    {
      hspi->CRCSize = 0;
    }

    /* check the data size to adapt Rx threshold and the set the function for IT treatment */
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT )
    {
      /* set fiforxthreshold according the reception data length: 16 bit */
      CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
      hspi->RxISR = SPI_RxISR_16BIT;
      hspi->TxISR = NULL;
    }
    else
    {
      /* set fiforxthreshold according the reception data length: 8 bit */
      SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
      hspi->RxISR = SPI_RxISR_8BIT;
      hspi->TxISR = NULL;
    }

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      SPI_1LINE_RX(hspi);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      SPI_RESET_CRC(hspi);
    }

    /* Enable TXE and ERR interrupt */
    __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    /* Note : The SPI must be enabled after unlocking current process
    to avoid the risk of SPI interrupt handle execution before current
    process unlock */

    /* Check if the SPI is already enabled */
    if((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit and Receive an amount of data in no-blocking mode with Interrupt
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size: amount of data to be sent and received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  if((hspi->State == HAL_SPI_STATE_READY) || \
     ((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (hspi->State == HAL_SPI_STATE_BUSY_RX)))
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process locked */
    __HAL_LOCK(hspi);

    hspi->CRCSize = 0;
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->CRCSize = 1;
      if((hspi->Init.DataSize <= SPI_DATASIZE_8BIT) && (hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT))
      {
        hspi->CRCSize = 2;
      }
    }

    if(hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    hspi->pTxBuffPtr  = pTxData;
    hspi->TxXferSize  = Size;
    hspi->TxXferCount = Size;
    hspi->pRxBuffPtr  = pRxData;
    hspi->RxXferSize  = Size;
    hspi->RxXferCount = Size;

    /* Set the function for IT treatement */
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT )
    {
      hspi->RxISR = SPI_2linesRxISR_16BIT;
      hspi->TxISR = SPI_2linesTxISR_16BIT;
    }
    else
    {
      hspi->RxISR = SPI_2linesRxISR_8BIT;
      hspi->TxISR = SPI_2linesTxISR_8BIT;
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      SPI_RESET_CRC(hspi);
    }

    /* check if packing mode is enabled and if there is more than 2 data to receive */
    if((hspi->Init.DataSize > SPI_DATASIZE_8BIT) || (hspi->RxXferCount >= 2))
    {
      /* set fiforxthreshold according the reception data length: 16 bit */
      CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
    }
    else
    {
      /* set fiforxthreshold according the reception data length: 8 bit */
      SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
    }

    /* Enable TXE, RXNE and ERR interrupt */
    __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    /* Check if the SPI is already enabled */
    if((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit an amount of data in no-blocking mode with DMA
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

  if(hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if((pData == NULL) || (Size == 0))
  {
    return HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hspi);

  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)NULL;
  hspi->RxXferSize  = 0;
  hspi->RxXferCount = 0;

  /* Configure communication direction : 1Line */
  if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }

  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }

  /* Set the SPI TxDMA Half transfer complete callback */
  hspi->hdmatx->XferHalfCpltCallback = SPI_DMAHalfTransmitCplt;

  /* Set the SPI TxDMA transfer complete callback */
  hspi->hdmatx->XferCpltCallback = SPI_DMATransmitCplt;

  /* Set the DMA error callback */
  hspi->hdmatx->XferErrorCallback = SPI_DMAError;

  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
  /* packing mode is enabled only if the DMA setting is HALWORD */
  if((hspi->Init.DataSize <= SPI_DATASIZE_8BIT) && (hspi->hdmatx->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD))
  {
    /* Check the even/odd of the data size + crc if enabled */
    if((hspi->TxXferCount & 0x1) == 0)
    {
      CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
      hspi->TxXferCount = (hspi->TxXferCount >> 1);
    }
    else
    {
      SET_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
      hspi->TxXferCount = (hspi->TxXferCount >> 1) + 1;
    }
  }

  /* Enable the Tx DMA channel */
  HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->DR, hspi->TxXferCount);

  /* Check if the SPI is already enabled */
  if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Enable Tx DMA Request */
  hspi->Instance->CR2 |= SPI_CR2_TXDMAEN;

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
* @brief  Receive an amount of data in no-blocking mode with DMA
* @param  hspi: SPI handle
* @param  pData: pointer to data buffer
* @param  Size: amount of data to be sent
* @retval HAL status
*/
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  if(hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if((pData == NULL) || (Size == 0))
  {
    return HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hspi);

  hspi->State       = HAL_SPI_STATE_BUSY_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = pData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;
  hspi->pTxBuffPtr  = (uint8_t *)NULL;
  hspi->TxXferSize  = 0;
  hspi->TxXferCount = 0;

  if((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES))
  {
    /* Process Unlocked */
    __HAL_UNLOCK(hspi);
    /* the receive process is not supported in 2Lines direction master mode */
    /* in this case we call the transmitReceive process                     */
    return HAL_SPI_TransmitReceive_DMA(hspi,pData,pData,Size);
  }

  /* Configure communication direction : 1Line */
  if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_RX(hspi);
  }

  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }

  /* packing mode management is enabled by the DMA settings */
  if((hspi->Init.DataSize <= SPI_DATASIZE_8BIT) && (hspi->hdmarx->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD))
  {
    /* Process Locked */
    __HAL_UNLOCK(hspi);
    /* Restriction the DMA data received is not allowed in this mode */
    return HAL_ERROR;
  }

  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMARX);
  if( hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* set fiforxthreshold according the reception data length: 16bit */
    CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }
  else
  {
    /* set fiforxthreshold according the reception data length: 8bit */
    SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  }

  /* Set the SPI RxDMA Half transfer complete callback */
  hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;

  /* Set the SPI Rx DMA transfer complete callback */
  hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;

  /* Set the DMA error callback */
  hspi->hdmarx->XferErrorCallback = SPI_DMAError;

  /* Enable Rx DMA Request */
  hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;

  /* Enable the Rx DMA channel */
  HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->DR, (uint32_t)hspi->pRxBuffPtr, hspi->RxXferCount);

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  /* Check if the SPI is already enabled */
  if((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  return HAL_OK;
}

/**
  * @brief  Transmit and Receive an amount of data in no-blocking mode with DMA
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @note  When the CRC feature is enabled the pRxData Length must be Size + 1
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  if((hspi->State == HAL_SPI_STATE_READY) ||
     ((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (hspi->State == HAL_SPI_STATE_BUSY_RX)))
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process locked */
    __HAL_LOCK(hspi);

    /* check if the transmit Receive function is not called by a receive master */
    if(hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    hspi->pTxBuffPtr  = (uint8_t *)pTxData;
    hspi->TxXferSize  = Size;
    hspi->TxXferCount = Size;
    hspi->pRxBuffPtr  = (uint8_t *)pRxData;
    hspi->RxXferSize  = Size;
    hspi->RxXferCount = Size;

    /* Reset CRC Calculation + increase the rxsize */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      SPI_RESET_CRC(hspi);
    }

    /* Reset the threshold bit */
    CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
    CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMARX);

    /* the packing mode management is enabled by the DMA settings according the spi data size */
    if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
    {
      /* set fiforxthreshold according the reception data length: 16bit */
      CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
    }
    else
    {
      /* set fiforxthreshold according the reception data length: 8bit */
      SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);

      if(hspi->hdmatx->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD)
      {
        if((hspi->TxXferSize & 0x1) == 0x0 )
        {
          CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
          hspi->TxXferCount = hspi->TxXferCount >> 1;
        }
        else
        {
          SET_BIT(hspi->Instance->CR2, SPI_CR2_LDMATX);
          hspi->TxXferCount = (hspi->TxXferCount >> 1) + 1;
        }
      }

      if(hspi->hdmarx->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD)
      {
        /* set fiforxthreshold according the reception data length: 16bit */
        CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);

        /* Size must include the CRC length */
        if((hspi->RxXferCount & 0x1) == 0x0 )
        {
          CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_LDMARX);
          hspi->RxXferCount = hspi->RxXferCount >> 1;
        }
        else
        {
          SET_BIT(hspi->Instance->CR2, SPI_CR2_LDMARX);
          hspi->RxXferCount = (hspi->RxXferCount >> 1) + 1;
        }
      }
    }

    /* Set the SPI Rx DMA transfer complete callback because the last generated transfer request is
    the reception request (RXNE) */
    if(hspi->State == HAL_SPI_STATE_BUSY_RX)
    {
      hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;
      hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;
    }
    else
    {
       hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfTransmitReceiveCplt;
      hspi->hdmarx->XferCpltCallback = SPI_DMATransmitReceiveCplt;
    }
    /* Set the DMA error callback */
    hspi->hdmarx->XferErrorCallback = SPI_DMAError;

    /* Enable Rx DMA Request */
    hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;

    /* Enable the Rx DMA channel */
    HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->DR, (uint32_t) hspi->pRxBuffPtr, hspi->RxXferCount);

    /* Set the SPI Tx DMA transfer complete callback as NULL because the communication closing
    is performed in DMA reception complete callback  */
    hspi->hdmatx->XferHalfCpltCallback = NULL;
    hspi->hdmatx->XferCpltCallback = NULL;

    if(hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
    {
      /* Set the DMA error callback */
      hspi->hdmatx->XferErrorCallback = SPI_DMAError;
    }
    else
    {
      hspi->hdmatx->XferErrorCallback = NULL;
    }

    /* Enable the Tx DMA channel */
    HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->DR, hspi->TxXferCount);

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    /* Check if the SPI is already enabled */
    if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    /* Enable Tx DMA Request */
    hspi->Instance->CR2 |= SPI_CR2_TXDMAEN;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Pauses the DMA Transfer.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
  /* Process Locked */
  __HAL_LOCK(hspi);

  /* Disable the SPI DMA Tx & Rx requests */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief Resumes the DMA Transfer.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
  /* Process Locked */
  __HAL_LOCK(hspi);

  /* Enable the SPI DMA Tx & Rx requests */
  SET_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief Stops the DMA Transfer.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
  /* The Lock is not implemented on this API to allow the user application
     to call the HAL SPI API under callbacks HAL_SPI_TxCpltCallback() or HAL_SPI_RxCpltCallback() or HAL_SPI_TxRxCpltCallback():
     when calling HAL_DMA_Abort() API the DMA TX/RX Transfer complete interrupt is generated
     and the correspond call back is executed HAL_SPI_TxCpltCallback() or HAL_SPI_RxCpltCallback() or HAL_SPI_TxRxCpltCallback()
     */

  /* Abort the SPI DMA tx Stream */
  if(hspi->hdmatx != NULL)
  {
    HAL_DMA_Abort(hspi->hdmatx);
  }
  /* Abort the SPI DMA rx Stream */
  if(hspi->hdmarx != NULL)
  {
    HAL_DMA_Abort(hspi->hdmarx);
  }

  /* Disable the SPI DMA Tx & Rx requests */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
  hspi->State = HAL_SPI_STATE_READY;
  return HAL_OK;
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval None
  */
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
  /* SPI in mode Receiver ----------------------------------------------------*/
  if((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR) == RESET) &&
     (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE) != RESET) && (__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_RXNE) != RESET))
  {
    hspi->RxISR(hspi);
    return;
  }

  /* SPI in mode Transmitter ---------------------------------------------------*/
  if((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE) != RESET) && (__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_TXE) != RESET))
  {
    hspi->TxISR(hspi);
    return;
  }

  /* SPI in ERROR Treatment ---------------------------------------------------*/
  if((hspi->Instance->SR & (SPI_FLAG_MODF | SPI_FLAG_OVR | SPI_FLAG_FRE)) != RESET)
  {
    /* SPI Overrun error interrupt occurred -------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR) != RESET)
    {
      if(hspi->State != HAL_SPI_STATE_BUSY_TX)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_OVR;
        __HAL_SPI_CLEAR_OVRFLAG(hspi);
      }
      else
      {
        return;
      }
    }

    /* SPI Mode Fault error interrupt occurred -------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_MODF) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_MODF;
      __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }

    /* SPI Frame error interrupt occurred ----------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_FRE) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FRE;
      __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE | SPI_IT_TXE | SPI_IT_ERR);
    hspi->State = HAL_SPI_STATE_READY;
    HAL_SPI_ErrorCallback(hspi);

    return;
  }
}

/**
  * @brief Tx Transfer completed callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_RxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Tx and Rx Transfer completed callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Tx Half Transfer completed callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxHalfCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Rx Half Transfer completed callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_RxHalfCpltCallback() should be implemented in the user file
   */
}

/**
  * @brief Tx and Rx Half Transfer callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxHalfCpltCallback() should be implemented in the user file
   */
}

/**
  * @brief SPI error callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_ErrorCallback should be implemented in the user file
   */
  /* NOTE : The ErrorCode parameter in the hspi handle is updated by the SPI processes
            and user can use HAL_SPI_GetError() API to check the latest error occurred
   */
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SPI_Exported_Functions_Group3 Peripheral State and Errors functions
  *  @brief   SPI control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the SPI.
     (+) HAL_SPI_GetState() API can be helpful to check in run-time the state of the SPI peripheral
     (+) HAL_SPI_GetError() check in run-time Errors occurring during communication
@endverbatim
  * @{
  */

/**
  * @brief  Return the SPI state
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI state
  */
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi)
{
  return hspi->State;
}

/**
  * @brief  Return the SPI error code
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI error code in bitmap format
  */
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi)
{
  return hspi->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SPI_Private_Functions SPI Private Functions
  * @{
  */

/**
  * @brief DMA SPI transmit process complete callback
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* DMA Normal Mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* Disable Tx DMA Request */
    CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN);

    /* Clear OVERUN flag in 2 Lines communication mode because received data is not read */
    if(hspi->Init.Direction == SPI_DIRECTION_2LINES)
    {
      __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }

    hspi->TxXferCount = 0;
    hspi->State = HAL_SPI_STATE_READY;

    if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
      HAL_SPI_ErrorCallback(hspi);
      return;
    }
  }
  HAL_SPI_TxCpltCallback(hspi);
}

/**
  * @brief DMA SPI receive process complete callback
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  __IO uint16_t tmpreg;
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* DMA Normal mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* CRC handling */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      /* Wait until TXE flag */
      if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, SPI_DEFAULT_TIMEOUT) != HAL_OK)
      {
        /* Error on the CRC reception */
        hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
      }
      if(hspi->Init.DataSize > SPI_DATASIZE_8BIT)
      {
        tmpreg = hspi->Instance->DR;
        UNUSED(tmpreg); /* To avoid GCC warning */
      }
      else
      {
        tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
        UNUSED(tmpreg); /* To avoid GCC warning */

        if(hspi->Init.CRCLength == SPI_CRC_LENGTH_16BIT)
        {
          if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SPI_FLAG_RXNE, SPI_DEFAULT_TIMEOUT) != HAL_OK)
          {
            /* Error on the CRC reception */
            hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
          }
          tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
          UNUSED(tmpreg); /* To avoid GCC warning */
        }
      }
    }

    /* Disable Rx DMA Request */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    /* Disable Tx DMA Request (done by default to handle the case master rx direction 2 lines) */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);

    /* Check the end of the transaction */
    SPI_EndRxTransaction(hspi,SPI_DEFAULT_TIMEOUT);

    hspi->RxXferCount = 0;
    hspi->State = HAL_SPI_STATE_READY;

    /* Check if CRC error occurred */
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
      __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      HAL_SPI_RxCpltCallback(hspi);
    }
    else
    {
      if(hspi->ErrorCode == HAL_SPI_ERROR_NONE)
      {
        HAL_SPI_RxCpltCallback(hspi);
      }
      else
      {
        HAL_SPI_ErrorCallback(hspi);
      }
    }
  }
  else
  {
    HAL_SPI_RxCpltCallback(hspi);
  }
}

/**
  * @brief DMA SPI transmit receive process complete callback
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
  __IO int16_t tmpreg;
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* CRC handling */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    if((hspi->Init.DataSize == SPI_DATASIZE_8BIT) && (hspi->Init.CRCLength == SPI_CRC_LENGTH_8BIT))
    {
      if(SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_QUARTER_FULL, SPI_DEFAULT_TIMEOUT) != HAL_OK)
      {
        /* Error on the CRC reception */
        hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
      }
      tmpreg = *(__IO uint8_t *)&hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */
    }
    else
    {
      if(SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_HALF_FULL, SPI_DEFAULT_TIMEOUT) != HAL_OK)
      {
        /* Error on the CRC reception */
        hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
      }
      tmpreg = hspi->Instance->DR;
      UNUSED(tmpreg); /* To avoid GCC warning */
    }
  }

  /* Check the end of the transaction */
  SPI_EndRxTxTransaction(hspi,SPI_DEFAULT_TIMEOUT);

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN);

  /* Disable Rx DMA Request */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_RXDMAEN);

  hspi->TxXferCount = 0;
  hspi->RxXferCount = 0;
  hspi->State = HAL_SPI_STATE_READY;

  /* Check if CRC error occurred */
  if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
  {
    hspi->ErrorCode = HAL_SPI_ERROR_CRC;
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
    HAL_SPI_ErrorCallback(hspi);
  }
  else
  {
    if(hspi->ErrorCode == HAL_SPI_ERROR_NONE)
    {
      HAL_SPI_TxRxCpltCallback(hspi);
    }
    else
    {
      HAL_SPI_ErrorCallback(hspi);
    }
  }
}

/**
  * @brief DMA SPI half transmit process complete callback
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  HAL_SPI_TxHalfCpltCallback(hspi);
}

/**
  * @brief DMA SPI half receive process complete callback
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  HAL_SPI_RxHalfCpltCallback(hspi);
}

/**
  * @brief DMA SPI Half transmit receive process complete callback
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  HAL_SPI_TxRxHalfCpltCallback(hspi);
}

/**
  * @brief DMA SPI communication error callback
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAError(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Stop the disable DMA transfer on SPI side */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

  hspi->ErrorCode|= HAL_SPI_ERROR_DMA;
  hspi->State = HAL_SPI_STATE_READY;
  HAL_SPI_ErrorCallback(hspi);
}

/**
  * @brief  Rx Handler for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesRxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
  /* Receive data in packing mode */
  if(hspi->RxXferCount > 1)
  {
    *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
    hspi->pRxBuffPtr += sizeof(uint16_t);
    hspi->RxXferCount -= 2;
    if(hspi->RxXferCount == 1)
    {
      /* set fiforxthreshold according the reception data length: 8bit */
      SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
    }
  }
  /* Receive data in 8 Bit mode */
  else
  {
    *hspi->pRxBuffPtr++ = *((__IO uint8_t *)&hspi->Instance->DR);
    hspi->RxXferCount--;
  }

  /* check end of the reception */
  if(hspi->RxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
      hspi->RxISR =  SPI_2linesRxISR_8BITCRC;
      return;
    }

    /* Disable RXNE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE);

    if(hspi->TxXferCount == 0)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
}

/**
  * @brief  Rx Handler for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesRxISR_8BITCRC(struct __SPI_HandleTypeDef *hspi)
{
  __IO uint8_t tmpreg;

  tmpreg = *((__IO uint8_t *)&hspi->Instance->DR);
  UNUSED(tmpreg); /* To avoid GCC warning */

  hspi->CRCSize--;

  /* check end of the reception */
  if(hspi->CRCSize == 0)
  {
    /* Disable RXNE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE);

    if(hspi->TxXferCount == 0)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
}

/**
  * @brief  Tx Handler for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesTxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
  /* Transmit data in packing Bit mode */
  if(hspi->TxXferCount >= 2)
  {
    hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
    hspi->pTxBuffPtr += sizeof(uint16_t);
    hspi->TxXferCount -= 2;
  }
  /* Transmit data in 8 Bit mode */
  else
  {
    *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr++);
    hspi->TxXferCount--;
  }

  /* check the end of the transmission */
  if(hspi->TxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
    }
    /* Disable TXE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);

    if(hspi->RxXferCount == 0)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
}

/**
  * @brief  Rx 16Bit Handler for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesRxISR_16BIT(struct __SPI_HandleTypeDef *hspi)
{
  /* Receive data in 16 Bit mode */
  *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
  hspi->pRxBuffPtr += sizeof(uint16_t);
  hspi->RxXferCount--;

  if(hspi->RxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->RxISR =  SPI_2linesRxISR_16BITCRC;
      return;
    }

    /* Disable RXNE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE);

    if(hspi->TxXferCount == 0)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
}

/**
  * @brief  Manage the CRC 16bit receive for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesRxISR_16BITCRC(struct __SPI_HandleTypeDef *hspi)
{
  /* Receive data in 16 Bit mode */
  __IO uint16_t tmpreg = hspi->Instance->DR;
  UNUSED(tmpreg); /* To avoid GCC warning */

  /* Disable RXNE interrupt */
  __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE);

  SPI_CloseRxTx_ISR(hspi);
}

/**
  * @brief  Tx Handler for Transmit and Receive in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_2linesTxISR_16BIT(struct __SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 16 Bit mode */
  hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
  hspi->pTxBuffPtr += sizeof(uint16_t);
  hspi->TxXferCount--;

  /* Enable CRC Transmission */
  if(hspi->TxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
    }
    /* Disable TXE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);

    if(hspi->RxXferCount == 0)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
}

/**
  * @brief  Manage the CRC receive in Interrupt context
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_8BITCRC(struct __SPI_HandleTypeDef *hspi)
{
  __IO uint8_t tmpreg;
  tmpreg = *((__IO uint8_t*)&hspi->Instance->DR);
  UNUSED(tmpreg); /* To avoid GCC warning */

  hspi->CRCSize--;

  if(hspi->CRCSize == 0)
  {
    SPI_CloseRx_ISR(hspi);
  }
}

/**
  * @brief  Manage the receive in Interrupt context
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
  *hspi->pRxBuffPtr++ = (*(__IO uint8_t *)&hspi->Instance->DR);
  hspi->RxXferCount--;

  /* Enable CRC Transmission */
  if((hspi->RxXferCount == 1) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
  {
    hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
  }

  if(hspi->RxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->RxISR =  SPI_RxISR_8BITCRC;
      return;
    }
    SPI_CloseRx_ISR(hspi);
  }
}

/**
  * @brief  Manage the CRC 16bit receive in Interrupt context
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_16BITCRC(struct __SPI_HandleTypeDef *hspi)
{
  __IO uint16_t tmpreg;

  tmpreg = hspi->Instance->DR;
  UNUSED(tmpreg); /* To avoid GCC warning */

  /* Disable RXNE and ERR interrupt */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

  SPI_CloseRx_ISR(hspi);
}

/**
  * @brief  Manage the 16Bit receive in Interrupt context
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_16BIT(struct __SPI_HandleTypeDef *hspi)
{
  *((uint16_t *)hspi->pRxBuffPtr) = hspi->Instance->DR;
  hspi->pRxBuffPtr += sizeof(uint16_t);
  hspi->RxXferCount--;

  /* Enable CRC Transmission */
  if((hspi->RxXferCount == 1) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
  {
    hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
  }

  if(hspi->RxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->RxISR = SPI_RxISR_16BITCRC;
      return;
    }
    SPI_CloseRx_ISR(hspi);
  }
}

/**
  * @brief  Handle the data 8Bit transmit in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_TxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
  *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr++);
  hspi->TxXferCount--;

  if(hspi->TxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      /* Enable CRC Transmission */
      hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
    }
    SPI_CloseTx_ISR(hspi);
  }
}

/**
  * @brief  Handle the data 16Bit transmit in Interrupt mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_TxISR_16BIT(struct __SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 16 Bit mode */
  hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
  hspi->pTxBuffPtr += sizeof(uint16_t);
  hspi->TxXferCount--;

  if(hspi->TxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      /* Enable CRC Transmission */
      hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
    }
    SPI_CloseTx_ISR(hspi);
  }
}

/**
  * @brief This function handles SPI Communication Timeout.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param Flag : SPI flag to check
  * @param State : flag state to check
  * @param Timeout : Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_WaitFlagStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, uint32_t State, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while((hspi->Instance->SR & Flag) != State)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0) || ((HAL_GetTick()-tickstart) >= Timeout))
      {
        /* Disable the SPI and reset the CRC: the CRC value should be cleared
        on both master and slave sides in order to resynchronize the master
        and slave for their respective CRC calculation */

        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

        if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
        {
          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);
        }

        /* Reset CRC Calculation */
        if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
        {
          SPI_RESET_CRC(hspi);
        }

        hspi->State= HAL_SPI_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hspi);

        return HAL_TIMEOUT;
      }
    }
  }

  return HAL_OK;
}

/**
  * @brief This function handles SPI Communication Timeout.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param Fifo : Fifo to check
  * @param State : Fifo state to check
  * @param Timeout : Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_WaitFifoStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Fifo, uint32_t State, uint32_t Timeout)
{
  __IO uint8_t tmpreg;
  uint32_t tickstart = HAL_GetTick();

  while((hspi->Instance->SR & Fifo) != State)
  {
    if((Fifo == SPI_SR_FRLVL) && (State == SPI_FRLVL_EMPTY))
    {
      tmpreg = *((__IO uint8_t*)&hspi->Instance->DR);
      UNUSED(tmpreg); /* To avoid GCC warning */
    }

    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0) || ((HAL_GetTick()-tickstart) >= Timeout))
      {
        /* Disable the SPI and reset the CRC: the CRC value should be cleared
                  on both master and slave sides in order to resynchronize the master
                 and slave for their respective CRC calculation */

        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

        if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
        {
          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);
        }

        /* Reset CRC Calculation */
        if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
        {
          SPI_RESET_CRC(hspi);
        }

        hspi->State = HAL_SPI_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hspi);

        return HAL_TIMEOUT;
      }
    }
  }

  return HAL_OK;
}

/**
  * @brief This function handles the check of the RX transaction complete.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param Timeout : Timeout duration
  * @retval None
  */
static HAL_StatusTypeDef SPI_EndRxTransaction(SPI_HandleTypeDef *hspi,  uint32_t Timeout)
{
  if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
  {
    /* Disable SPI peripheral */
    __HAL_SPI_DISABLE(hspi);
  }
  if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    return HAL_TIMEOUT;
  }
  if(SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_EMPTY, Timeout) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    return HAL_TIMEOUT;
  }

  return HAL_OK;
}

/**
  * @brief This function handles the check of the RXTX or TX transaction complete.
  * @param hspi: SPI handle
  * @param Timeout : Timeout duration
  */
static HAL_StatusTypeDef SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout)
{
  /* Procedure to check the transaction complete */
  if(SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FTLVL, SPI_FTLVL_EMPTY, Timeout) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    return HAL_TIMEOUT;
  }
  if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    return HAL_TIMEOUT;
  }
  if(SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_EMPTY, Timeout) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    return HAL_TIMEOUT;
  }
  return HAL_OK;
}

/**
  * @brief This function handles the close of the RXTX transaction.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_CloseRxTx_ISR(SPI_HandleTypeDef *hspi)
{
  /* Disable ERR interrupt */
  __HAL_SPI_DISABLE_IT(hspi, SPI_IT_ERR);

  /* Check if CRC error occurred */
  if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
  {
    hspi->State = HAL_SPI_STATE_READY;
    hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
    HAL_SPI_ErrorCallback(hspi);
  }
  else
  {
    if(hspi->ErrorCode == HAL_SPI_ERROR_NONE)
    {
      if(hspi->State == HAL_SPI_STATE_BUSY_RX)
      {
      	hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_RxCpltCallback(hspi);
      }
      else
      {
      	hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_TxRxCpltCallback(hspi);
      }
    }
    else
    {
      hspi->State = HAL_SPI_STATE_READY;
      HAL_SPI_ErrorCallback(hspi);
    }
  }
}

/**
  * @brief This function handles the close of the RX transaction.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_CloseRx_ISR(SPI_HandleTypeDef *hspi)
{
    /* Disable RXNE and ERR interrupt */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

    /* Check the end of the transaction */
    SPI_EndRxTransaction(hspi,SPI_DEFAULT_TIMEOUT);

    hspi->State = HAL_SPI_STATE_READY;

    /* Check if CRC error occurred */
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode|= HAL_SPI_ERROR_CRC;
      __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      HAL_SPI_ErrorCallback(hspi);
    }
    else
    {
      if(hspi->ErrorCode == HAL_SPI_ERROR_NONE)
      {
        HAL_SPI_RxCpltCallback(hspi);
      }
      else
      {
        HAL_SPI_ErrorCallback(hspi);
      }
    }
}

/**
  * @brief This function handles the close of the TX transaction.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_CloseTx_ISR(SPI_HandleTypeDef *hspi)
{
  /* Disable TXE and ERR interrupt */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_ERR));

  /* Clear OVERUN flag in 2 Lines communication mode because received is not read */
  if(hspi->Init.Direction == SPI_DIRECTION_2LINES)
  {
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
  }

  hspi->State = HAL_SPI_STATE_READY;
  if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    HAL_SPI_ErrorCallback(hspi);
  }
  else
  {
    HAL_SPI_TxCpltCallback(hspi);
  }
}

/**
  * @}
  */

#endif /* HAL_SPI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
