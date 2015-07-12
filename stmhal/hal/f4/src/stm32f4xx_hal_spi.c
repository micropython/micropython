/**
  ******************************************************************************
  * @file    stm32f4xx_hal_spi.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   SPI HAL module driver.
  *    
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
              (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive stream
              (+++) Enable the DMAx interface clock using 
              (+++) Configure the DMA handle parameters 
              (+++) Configure the DMA Tx or Rx Stream
              (+++) Associate the initilalized hdma_tx handle to the hspi DMA Tx or Rx handle
              (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx or Rx Stream

      (#) Program the Mode, Direction , Data size, Baudrate Prescaler, NSS 
          management, Clock polarity and phase, FirstBit and CRC configuration in the hspi Init structure.

      (#) Initialize the SPI registers by calling the HAL_SPI_Init() API:
          (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
              by calling the customed HAL_SPI_MspInit() API.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup SPI 
  * @brief SPI HAL module driver
  * @{
  */

#ifdef HAL_SPI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_TIMEOUT_VALUE  10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SPI_TxCloseIRQHandler(SPI_HandleTypeDef *hspi);
static void SPI_TxISR(SPI_HandleTypeDef *hspi);
static void SPI_RxCloseIRQHandler(SPI_HandleTypeDef *hspi);
static void SPI_2LinesRxISR(SPI_HandleTypeDef *hspi);
static void SPI_RxISR(SPI_HandleTypeDef *hspi);
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAError(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status, uint32_t Timeout);

/* Private functions ---------------------------------------------------------*/

/** @defgroup SPI_Private_Functions
  * @{
  */

/** @defgroup SPI_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and 
          de-initialiaze the SPIx peripheral:

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

      (+) Call the function HAL_SPI_DeInit() to restore the default configuration 
          of the selected SPIx periperal.       

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the SPI according to the specified parameters 
  *         in the SPI_InitTypeDef and create the associated handle.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_MODE(hspi->Init.Mode));
  assert_param(IS_SPI_DIRECTION_MODE(hspi->Init.Direction));
  assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
  assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
  assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
  assert_param(IS_SPI_NSS(hspi->Init.NSS));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
  assert_param(IS_SPI_TIMODE(hspi->Init.TIMode));
  assert_param(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
  assert_param(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));

  if(hspi->State == HAL_SPI_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_SPI_MspInit(hspi);
  }
  
  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disble the selected SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
  Communication speed, First bit and CRC calculation state */
  hspi->Instance->CR1 = (hspi->Init.Mode | hspi->Init.Direction | hspi->Init.DataSize |
                         hspi->Init.CLKPolarity | hspi->Init.CLKPhase | (hspi->Init.NSS & SPI_CR1_SSM) |
                         hspi->Init.BaudRatePrescaler | hspi->Init.FirstBit  | hspi->Init.CRCCalculation);

  /* Configure : NSS management */
  hspi->Instance->CR2 = (((hspi->Init.NSS >> 16) & SPI_CR2_SSOE) | hspi->Init.TIMode);

  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  hspi->Instance->CRCPR = hspi->Init.CRCPolynomial;

  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  hspi->Instance->I2SCFGR &= (uint32_t)(~SPI_I2SCFGR_I2SMOD);

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  DeInitializes the SPI peripheral 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if(hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Disable the SPI Peripheral Clock */
  __HAL_SPI_DISABLE(hspi);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  HAL_SPI_MspDeInit(hspi);

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief SPI MSP Init
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
 {
   /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_MspInit could be implenetd in the user file
   */
}

/**
  * @brief SPI MSP DeInit
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_MspDeInit could be implenetd in the user file
   */
}

/**
  * @}
  */

/** @defgroup SPI_Group2 IO operation functions
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
           will be executed respectivelly at the end of the transmit or Receive process
           The HAL_SPI_ErrorCallback()user callback will be executed when a communication error is detected

    (#) Blocking mode APIs are :
        (++) HAL_SPI_Transmit()in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_Receive() in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_TransmitReceive() in full duplex mode

    (#) Non Blocking mode API's with Interrupt are :
        (++) HAL_SPI_Transmit_IT()in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_Receive_IT() in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_TransmitReceive_IT()in full duplex mode
        (++) HAL_SPI_IRQHandler()

    (#) Non Blocking mode functions with DMA are :
        (++) HAL_SPI_Transmit_DMA()in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_Receive_DMA() in 1Line (simplex) and 2Lines (full duplex) mode
        (++) HAL_SPI_TransmitReceie_DMA() in full duplex mode

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_SPI_TxCpltCallback()
        (++) HAL_SPI_RxCpltCallback()
        (++) HAL_SPI_ErrorCallback()
        (++) HAL_SPI_TxRxCpltCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmit an amount of data in blocking mode
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{

  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Configure communication */
    hspi->State = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

    hspi->pTxBuffPtr = pData;
    hspi->TxXferSize = Size;
    hspi->TxXferCount = Size;

    /*Init field not used in handle to zero */
    hspi->TxISR = 0;
    hspi->RxISR = 0;
    hspi->RxXferSize   = 0;
    hspi->RxXferCount  = 0;

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      /* Configure communication direction : 1Line */
      __HAL_SPI_1LINE_TX(hspi);
    }

    /* Check if the SPI is already enabled */ 
    if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    /* Transmit data in 8 Bit mode */
    if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
    {
      if((hspi->Init.Mode == SPI_MODE_SLAVE)|| (hspi->TxXferCount == 0x01))
      {
        hspi->Instance->DR = (*hspi->pTxBuffPtr++);
        hspi->TxXferCount--;
      }
      while(hspi->TxXferCount > 0)
      {
        /* Wait until TXE flag is set to send data */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        hspi->Instance->DR = (*hspi->pTxBuffPtr++);
        hspi->TxXferCount--;
      }
      /* Enable CRC Transmission */
      if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) 
      {
        hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
      }
    }
    /* Transmit data in 16 Bit mode */
    else
    {
      if((hspi->Init.Mode == SPI_MODE_SLAVE) || (hspi->TxXferCount == 0x01))
      {
        hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr+=2;
        hspi->TxXferCount--;
      }
      while(hspi->TxXferCount > 0)
      {
        /* Wait until TXE flag is set to send data */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr+=2;
        hspi->TxXferCount--;
      }
      /* Enable CRC Transmission */
      if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) 
      {
        hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
      }
    }

    /* Wait until TXE flag is set to send data */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, Timeout) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
      return HAL_TIMEOUT;
    }

    /* Wait until Busy flag is reset before disabling SPI */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_BSY, SET, Timeout) != HAL_OK)
    { 
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
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

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive an amount of data in blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  __IO uint16_t tmpreg;
  uint32_t tmp = 0;

  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0)) 
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

    /*Init field not used in handle to zero */
    hspi->RxISR = 0;
    hspi->TxISR = 0;
    hspi->TxXferSize   = 0;
    hspi->TxXferCount  = 0;

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      __HAL_SPI_1LINE_RX(hspi);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }
    
    if((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES))
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hspi);

      /* Call transmit-receive function to send Dummy data on Tx line and generate clock on CLK line */
      return HAL_SPI_TransmitReceive(hspi, pData, pData, Size, Timeout);
    }

    /* Check if the SPI is already enabled */ 
    if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    /* Receive data in 8 Bit mode */
    if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
    {
      while(hspi->RxXferCount > 1)
      {
        /* Wait until RXNE flag is set */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }

        (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
        hspi->RxXferCount--;
      }
      /* Enable CRC Transmission */
      if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) 
      {
        hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
      }
    }
    /* Receive data in 16 Bit mode */
    else
    {
      while(hspi->RxXferCount > 1)
      {
        /* Wait until RXNE flag is set to read data */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }

        *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
        hspi->pRxBuffPtr+=2;
        hspi->RxXferCount--;
      }
      /* Enable CRC Transmission */
      if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) 
      {
        hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
      }
    }

    /* Wait until RXNE flag is set */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
    { 
      return HAL_TIMEOUT;
    }

    /* Receive last data in 8 Bit mode */
    if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
    {
      (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
    }
    /* Receive last data in 16 Bit mode */
    else
    {
      *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
      hspi->pRxBuffPtr+=2;
    }
    hspi->RxXferCount--;

    /* Wait until RXNE flag is set: CRC Received */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
        return HAL_TIMEOUT;
      }

      /* Read CRC to Flush RXNE flag */
      tmpreg = hspi->Instance->DR;
      (void)tmpreg; // suppress compiler warning
    }
    
    if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
    {
      /* Disable SPI peripheral */
      __HAL_SPI_DISABLE(hspi);
    }

    hspi->State = HAL_SPI_STATE_READY;

    tmp = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR);
    /* Check if CRC error occurred */
    if((hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) && (tmp != RESET))
    {  
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;

      /* Reset CRC Calculation */
      __HAL_SPI_RESET_CRC(hspi);

      /* Process Unlocked */
      __HAL_UNLOCK(hspi);

      return HAL_ERROR; 
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit and Receive an amount of data in blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer to be
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  __IO uint16_t tmpreg;
  uint32_t tmpstate = 0, tmp = 0;
  
  tmpstate = hspi->State; 
  if((tmpstate == HAL_SPI_STATE_READY) || (tmpstate == HAL_SPI_STATE_BUSY_RX))
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);
 
    /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
    if(hspi->State == HAL_SPI_STATE_READY)
    {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

     /* Configure communication */   
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

    hspi->pRxBuffPtr  = pRxData;
    hspi->RxXferSize  = Size;
    hspi->RxXferCount = Size;  
    
    hspi->pTxBuffPtr  = pTxData;
    hspi->TxXferSize  = Size; 
    hspi->TxXferCount = Size;

    /*Init field not used in handle to zero */
    hspi->RxISR = 0;
    hspi->TxISR = 0;

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Check if the SPI is already enabled */ 
    if((hspi->Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
      /* Enable SPI peripheral */
      __HAL_SPI_ENABLE(hspi);
    }

    /* Transmit and Receive data in 16 Bit mode */
    if(hspi->Init.DataSize == SPI_DATASIZE_16BIT)
    {
      if((hspi->Init.Mode == SPI_MODE_SLAVE) || ((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->TxXferCount == 0x01)))
      {
        hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr+=2;
        hspi->TxXferCount--;
      }
      if(hspi->TxXferCount == 0)
      {
        /* Enable CRC Transmission */
        if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
        {
          hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
        }

        /* Wait until RXNE flag is set */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }

        *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
        hspi->pRxBuffPtr+=2;
        hspi->RxXferCount--;
      }
      else
      {
        while(hspi->TxXferCount > 0)
        {
          /* Wait until TXE flag is set to send data */
          if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, Timeout) != HAL_OK)
          { 
            return HAL_TIMEOUT;
          }

          hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr+=2;
          hspi->TxXferCount--;

          /* Enable CRC Transmission */
          if((hspi->TxXferCount == 0) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED))
          {
            hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
          }

          /* Wait until RXNE flag is set */
          if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
          { 
            return HAL_TIMEOUT;
          }
          
          *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
          hspi->pRxBuffPtr+=2;
          hspi->RxXferCount--;
        }
        /* Receive the last byte */
        if(hspi->Init.Mode == SPI_MODE_SLAVE)
        {
          /* Wait until RXNE flag is set */
          if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }
          
          *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
          hspi->pRxBuffPtr+=2;
          hspi->RxXferCount--;
        }
      }
    }
    /* Transmit and Receive data in 8 Bit mode */
    else
    {
      if((hspi->Init.Mode == SPI_MODE_SLAVE) || ((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->TxXferCount == 0x01)))
      {
        hspi->Instance->DR = (*hspi->pTxBuffPtr++);
        hspi->TxXferCount--;
      }
      if(hspi->TxXferCount == 0)
      {
        /* Enable CRC Transmission */
        if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
        {
          hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
        }

        /* Wait until RXNE flag is set */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        (*hspi->pRxBuffPtr) = hspi->Instance->DR;
        hspi->RxXferCount--;
      }
      else
      {
        while(hspi->TxXferCount > 0)
        {
          /* Wait until TXE flag is set to send data */
          if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          hspi->Instance->DR = (*hspi->pTxBuffPtr++);
          hspi->TxXferCount--;

          /* Enable CRC Transmission */
          if((hspi->TxXferCount == 0) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED))
          {
            hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
          }

            /* Wait until RXNE flag is set */
            if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
            {
              return HAL_TIMEOUT;
            }
            
            (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
            hspi->RxXferCount--;
        }
        if(hspi->Init.Mode == SPI_MODE_SLAVE)
        {
          /* Wait until RXNE flag is set */
          if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }
          
          (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
          hspi->RxXferCount--;
        }
      }
    }

    /* Read CRC from DR to close CRC calculation process */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      /* Wait until RXNE flag is set */
      if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
        return HAL_TIMEOUT;
      }
      /* Read CRC */
      tmpreg = hspi->Instance->DR;
      (void)tmpreg; // suppress compiler warning
    }

    /* Wait until Busy flag is reset before disabling SPI */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_BSY, SET, Timeout) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
      return HAL_TIMEOUT;
    }
    
    hspi->State = HAL_SPI_STATE_READY;

    tmp = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR);
    /* Check if CRC error occurred */
    if((hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED) && (tmp != RESET))
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;

      /* Reset CRC Calculation */
      if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
      {
        __HAL_SPI_RESET_CRC(hspi);
      }

      /* Process Unlocked */
      __HAL_UNLOCK(hspi);
      
      return HAL_ERROR; 
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit an amount of data in no-blocking mode with Interrupt
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Configure communication */
    hspi->State        = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode    = HAL_SPI_ERROR_NONE;

    hspi->TxISR = &SPI_TxISR;
    hspi->pTxBuffPtr   = pData;
    hspi->TxXferSize   = Size;
    hspi->TxXferCount  = Size;

    /*Init field not used in handle to zero */
    hspi->RxISR = 0;
    hspi->RxXferSize   = 0;
    hspi->RxXferCount  = 0;

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      __HAL_SPI_1LINE_TX(hspi);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    if (hspi->Init.Direction == SPI_DIRECTION_2LINES)
    {
      __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE));
    }else
    {
      /* Enable TXE and ERR interrupt */
      __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_ERR));
    }
    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

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
  *                the configuration information for SPI module.
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
    hspi->State        = HAL_SPI_STATE_BUSY_RX;
    hspi->ErrorCode    = HAL_SPI_ERROR_NONE;

    hspi->RxISR = &SPI_RxISR;
    hspi->pRxBuffPtr   = pData;
    hspi->RxXferSize   = Size;
    hspi->RxXferCount  = Size ; 

   /*Init field not used in handle to zero */
    hspi->TxISR = 0;
    hspi->TxXferSize   = 0;
    hspi->TxXferCount  = 0;

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
       __HAL_SPI_1LINE_RX(hspi);
    }
    else if((hspi->Init.Direction == SPI_DIRECTION_2LINES) && (hspi->Init.Mode == SPI_MODE_MASTER))
    {
       /* Process Unlocked */
       __HAL_UNLOCK(hspi);

       /* Call transmit-receive function to send Dummy data on Tx line and generate clock on CLK line */
       return HAL_SPI_TransmitReceive_IT(hspi, pData, pData, Size);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Enable TXE and ERR interrupt */
    __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

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
  * @brief  Transmit and Receive an amount of data in no-blocking mode with Interrupt 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer to be
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
 uint32_t tmpstate = 0;

 tmpstate = hspi->State;
  if((tmpstate == HAL_SPI_STATE_READY) || \
     ((hspi->Init.Mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmpstate == HAL_SPI_STATE_BUSY_RX)))
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

    /* Process locked */
    __HAL_LOCK(hspi);

    /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
    if(hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    /* Configure communication */
    hspi->ErrorCode    = HAL_SPI_ERROR_NONE;

    hspi->TxISR = &SPI_TxISR;
    hspi->pTxBuffPtr   = pTxData;
    hspi->TxXferSize   = Size;
    hspi->TxXferCount  = Size;

    hspi->RxISR = &SPI_2LinesRxISR;
    hspi->pRxBuffPtr   = pRxData;
    hspi->RxXferSize   = Size;
    hspi->RxXferCount  = Size;

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Enable TXE, RXNE and ERR interrupt */
    __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

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
  * @brief  Transmit an amount of data in no-blocking mode with DMA
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  if(hspi->State == HAL_SPI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Configure communication */
    hspi->State       = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

    hspi->pTxBuffPtr  = pData;
    hspi->TxXferSize  = Size;
    hspi->TxXferCount = Size;

    /*Init field not used in handle to zero */
    hspi->TxISR = 0;
    hspi->RxISR = 0;
    hspi->RxXferSize   = 0;
    hspi->RxXferCount  = 0;

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
      __HAL_SPI_1LINE_TX(hspi);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Set the SPI TxDMA Half transfer complete callback */
    hspi->hdmatx->XferHalfCpltCallback = SPI_DMAHalfTransmitCplt;
    
    /* Set the SPI TxDMA transfer complete callback */
    hspi->hdmatx->XferCpltCallback = SPI_DMATransmitCplt;

    /* Set the DMA error callback */
    hspi->hdmatx->XferErrorCallback = SPI_DMAError;

    /* Enable the Tx DMA Stream */
    HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->DR, hspi->TxXferCount);

    /* Enable Tx DMA Request */
    hspi->Instance->CR2 |= SPI_CR2_TXDMAEN;

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

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
  * @brief  Receive an amount of data in no-blocking mode with DMA 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @note  When the CRC feature is enabled the pData Length must be Size + 1. 
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
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

    /*Init field not used in handle to zero */
    hspi->RxISR = 0;
    hspi->TxISR = 0;
    hspi->TxXferSize   = 0;
    hspi->TxXferCount  = 0;

    /* Configure communication direction : 1Line */
    if(hspi->Init.Direction == SPI_DIRECTION_1LINE)
    {
       __HAL_SPI_1LINE_RX(hspi);
    }
    else if((hspi->Init.Direction == SPI_DIRECTION_2LINES)&&(hspi->Init.Mode == SPI_MODE_MASTER))
    {
       /* Process Unlocked */
       __HAL_UNLOCK(hspi);

       /* Call transmit-receive function to send Dummy data on Tx line and generate clock on CLK line */
       return HAL_SPI_TransmitReceive_DMA(hspi, pData, pData, Size);
    }

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Set the SPI RxDMA Half transfer complete callback */
    hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;
    
    /* Set the SPI Rx DMA transfer complete callback */
    hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;

    /* Set the DMA error callback */
    hspi->hdmarx->XferErrorCallback = SPI_DMAError;

    /* Enable the Rx DMA Stream */
    HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->DR, (uint32_t)hspi->pRxBuffPtr, hspi->RxXferCount);

    /* Enable Rx DMA Request */  
    hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;  

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

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
  * @brief  Transmit and Receive an amount of data in no-blocking mode with DMA 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @note  When the CRC feature is enabled the pRxData Length must be Size + 1 
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  uint32_t tmpstate = 0;
  tmpstate = hspi->State;
  if((tmpstate == HAL_SPI_STATE_READY) || ((hspi->Init.Mode == SPI_MODE_MASTER) && \
     (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmpstate == HAL_SPI_STATE_BUSY_RX)))
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));
    
    /* Process locked */
    __HAL_LOCK(hspi);

    /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
    if(hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    /* Configure communication */
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

    hspi->pTxBuffPtr  = (uint8_t*)pTxData;
    hspi->TxXferSize  = Size;
    hspi->TxXferCount = Size;

    hspi->pRxBuffPtr  = (uint8_t*)pRxData;
    hspi->RxXferSize  = Size;
    hspi->RxXferCount = Size;

    /*Init field not used in handle to zero */
    hspi->RxISR = 0;
    hspi->TxISR = 0;

    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      __HAL_SPI_RESET_CRC(hspi);
    }

    /* Check if we are in Rx only or in Rx/Tx Mode and configure the DMA transfer complete callback */
    if(hspi->State == HAL_SPI_STATE_BUSY_RX)
    {
      /* Set the SPI Rx DMA Half transfer complete callback */
      hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;
      
      hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;
    }
    else
    {
      /* Set the SPI Tx/Rx DMA Half transfer complete callback */
      hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfTransmitReceiveCplt;
  
      hspi->hdmarx->XferCpltCallback = SPI_DMATransmitReceiveCplt;
    }

    /* Set the DMA error callback */
    hspi->hdmarx->XferErrorCallback = SPI_DMAError;

    /* Enable the Rx DMA Stream */
    HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->DR, (uint32_t)hspi->pRxBuffPtr, hspi->RxXferCount);

    /* Enable Rx DMA Request */  
    hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;

    /* Set the SPI Tx DMA transfer complete callback as NULL because the communication closing
    is performed in DMA reception complete callback  */
    hspi->hdmatx->XferCpltCallback = NULL;

    /* Set the DMA error callback */
    hspi->hdmatx->XferErrorCallback = SPI_DMAError;

    /* Enable the Tx DMA Stream */
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
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief Pauses the DMA Transfer.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
  /* Process Locked */
  __HAL_LOCK(hspi);
  
  /* Disable the SPI DMA Tx & Rx requests */
  hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
  hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
  
  /* Process Unlocked */
  __HAL_UNLOCK(hspi);
  
  return HAL_OK; 
}

/**
  * @brief Resumes the DMA Transfer.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
  /* Process Locked */
  __HAL_LOCK(hspi);
  
  /* Enable the SPI DMA Tx & Rx requests */
  hspi->Instance->CR2 |= SPI_CR2_TXDMAEN;
  hspi->Instance->CR2 |= SPI_CR2_RXDMAEN;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hspi);
  
  return HAL_OK;
}

/**
  * @brief Stops the DMA Transfer.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
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
  hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
  hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
  
  hspi->State = HAL_SPI_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_RXNE);
  tmp3 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR);
  /* SPI in mode Receiver and Overrun not occurred ---------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET) && (tmp3 == RESET))
  {
    hspi->RxISR(hspi);
    return;
  } 

  tmp1 = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE);
  tmp2 = __HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_TXE);
  /* SPI in mode Tramitter ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    hspi->TxISR(hspi);
    return;
  }

  if(__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_ERR) != RESET)
  {
    /* SPI CRC error interrupt occurred ---------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
      __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
    }
    /* SPI Mode Fault error interrupt occurred --------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_MODF) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_MODF;
      __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }
    
    /* SPI Overrun error interrupt occurred -----------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR) != RESET)
    {
      if(hspi->State != HAL_SPI_STATE_BUSY_TX)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_OVR;
        __HAL_SPI_CLEAR_OVRFLAG(hspi);      
      }
    }

    /* SPI Frame error interrupt occurred -------------------------------------*/
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_FRE) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FRE;
      __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    /* Call the Error call Back in case of Errors */
    if(hspi->ErrorCode!=HAL_SPI_ERROR_NONE)
    {
      hspi->State = HAL_SPI_STATE_READY;
      HAL_SPI_ErrorCallback(hspi);
    }
  }
}

/**
  * @brief Tx Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback could be implenetd in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_RxCpltCallback() could be implenetd in the user file
   */
}

/**
  * @brief Tx and Rx Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_TxRxCpltCallback() could be implenetd in the user file
   */
}

/**
  * @brief Tx Half Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_TxHalfCpltCallback could be implenetd in the user file
   */
}

/**
  * @brief Rx Half Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_RxHalfCpltCallback() could be implenetd in the user file
   */
}

/**
  * @brief Tx and Rx Transfer completed callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SPI_TxRxHalfCpltCallback() could be implenetd in the user file
   */
}

/**
  * @brief SPI error callbacks
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval None
  */
 __weak void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* NOTE : - This function Should not be modified, when the callback is needed,
            the HAL_SPI_ErrorCallback() could be implenetd in the user file.
            - The ErrorCode parameter in the hspi handle is updated by the SPI processes
            and user can use HAL_SPI_GetError() API to check the latest error occurred.
   */
}

/**
  * @}
  */

/** @defgroup SPI_Group3 Peripheral State and Errors functions 
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
  *                the configuration information for SPI module.
  * @retval HAL state
  */
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi)
{
  return hspi->State;
}

/**
  * @brief  Return the SPI error code
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval SPI Error Code
  */
HAL_SPI_ErrorTypeDef HAL_SPI_GetError(SPI_HandleTypeDef *hspi)
{
  return hspi->ErrorCode;
}

/**
  * @}
  */

  /**
  * @brief  Interrupt Handler to close Tx transfer 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval void
  */
static void SPI_TxCloseIRQHandler(SPI_HandleTypeDef *hspi)
{
  /* Wait until TXE flag is set to send data */
  if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
  {
    hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
  }

  /* Disable TXE interrupt */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE ));

  /* Disable ERR interrupt if Receive process is finished */
  if(__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_RXNE) == RESET)
  {
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_ERR));

    /* Wait until Busy flag is reset before disabling SPI */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_BSY, SET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }

    /* Clear OVERUN flag in 2 Lines communication mode because received is not read */
    if(hspi->Init.Direction == SPI_DIRECTION_2LINES)
    {
      __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }
    
    /* Check if Errors has been detected during transfer */
    if(hspi->ErrorCode ==  HAL_SPI_ERROR_NONE)
    {
      /* Check if we are in Tx or in Rx/Tx Mode */
      if(hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
      {
        /* Set state to READY before run the Callback Complete */
        hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_TxRxCpltCallback(hspi);
      }
      else
      {
        /* Set state to READY before run the Callback Complete */
        hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_TxCpltCallback(hspi);
      }
    }
    else
    {
      /* Set state to READY before run the Callback Complete */
      hspi->State = HAL_SPI_STATE_READY;
      /* Call Error call back in case of Error */
      HAL_SPI_ErrorCallback(hspi);
    }
  }
}

/**
  * @brief  Interrupt Handler to transmit amount of data in no-blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval void
  */
static void SPI_TxISR(SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 8 Bit mode */
  if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
  {
    hspi->Instance->DR = (*hspi->pTxBuffPtr++);
  }
  /* Transmit data in 16 Bit mode */
  else
  {
    hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
    hspi->pTxBuffPtr+=2;
  }
  hspi->TxXferCount--;

  if(hspi->TxXferCount == 0)
  {
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      /* calculate and transfer CRC on Tx line */
      hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;
    }
    SPI_TxCloseIRQHandler(hspi);
  }
}

/**
  * @brief  Interrupt Handler to close Rx transfer 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval void
  */
static void SPI_RxCloseIRQHandler(SPI_HandleTypeDef *hspi)
{
  __IO uint16_t tmpreg;

  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
  {
    /* Wait until RXNE flag is set to send data */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }

    /* Read CRC to reset RXNE flag */
    tmpreg = hspi->Instance->DR;
    (void)tmpreg; // suppress compiler warning

    /* Wait until RXNE flag is set to send data */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, SET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }

    /* Check if CRC error occurred */
    if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_CRC;

      /* Reset CRC Calculation */
      __HAL_SPI_RESET_CRC(hspi);
    }
  }

  /* Disable RXNE and ERR interrupt */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE));

  /* if Transmit process is finished */
  if(__HAL_SPI_GET_IT_SOURCE(hspi, SPI_IT_TXE) == RESET)
  {
    /* Disable ERR interrupt */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_ERR));

    if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
    {
      /* Disable SPI peripheral */
      __HAL_SPI_DISABLE(hspi);
    }
    
    /* Check if Errors has been detected during transfer */
    if(hspi->ErrorCode ==  HAL_SPI_ERROR_NONE)
    {
      /* Check if we are in Rx or in Rx/Tx Mode */
      if(hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
      {
        /* Set state to READY before run the Callback Complete */
        hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_TxRxCpltCallback(hspi);
      }
      else
      {
        /* Set state to READY before run the Callback Complete */
        hspi->State = HAL_SPI_STATE_READY;
        HAL_SPI_RxCpltCallback(hspi);
      }
    }
    else
    {
      /* Set state to READY before run the Callback Complete */
      hspi->State = HAL_SPI_STATE_READY;
      /* Call Error call back in case of Error */
      HAL_SPI_ErrorCallback(hspi);
    }
  }
}

/**
  * @brief  Interrupt Handler to receive amount of data in 2Lines mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval void
  */
static void SPI_2LinesRxISR(SPI_HandleTypeDef *hspi)
{
  /* Receive data in 8 Bit mode */
  if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
  {
    (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
  }
  /* Receive data in 16 Bit mode */
  else
  {
    *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
    hspi->pRxBuffPtr+=2;
  }
  hspi->RxXferCount--;

  if(hspi->RxXferCount==0)
  {
    SPI_RxCloseIRQHandler(hspi);
  }
}

/**
  * @brief  Interrupt Handler to receive amount of data in no-blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval void
  */
static void SPI_RxISR(SPI_HandleTypeDef *hspi)
{
  /* Receive data in 8 Bit mode */
  if(hspi->Init.DataSize == SPI_DATASIZE_8BIT)
  {
    (*hspi->pRxBuffPtr++) = hspi->Instance->DR;
  }
  /* Receive data in 16 Bit mode */
  else
  {
    *((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
    hspi->pRxBuffPtr+=2;
  }
    hspi->RxXferCount--;

  /* Enable CRC Transmission */
  if((hspi->RxXferCount == 1) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED))
  {
    /* Set CRC Next to calculate CRC on Rx side */
    hspi->Instance->CR1 |= SPI_CR1_CRCNEXT;  
  }

  if(hspi->RxXferCount == 0)
  {
    SPI_RxCloseIRQHandler(hspi);
  }
}

/**
  * @brief DMA SPI transmit process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* DMA Normal Mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* Wait until TXE flag is set to send data */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }
    /* Disable Tx DMA Request */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);

    /* Wait until Busy flag is reset before disabling SPI */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_BSY, SET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }

    hspi->TxXferCount = 0;

    hspi->State = HAL_SPI_STATE_READY;
  }

  /* Clear OVERUN flag in 2 Lines communication mode because received is not read */
  if(hspi->Init.Direction == SPI_DIRECTION_2LINES)
  {
   __HAL_SPI_CLEAR_OVRFLAG(hspi);
  }

  /* Check if Errors has been detected during transfer */
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
  * @brief DMA SPI receive process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  __IO uint16_t tmpreg;
  
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* DMA Normal mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    if((hspi->Init.Mode == SPI_MODE_MASTER)&&((hspi->Init.Direction == SPI_DIRECTION_1LINE)||(hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
    {
      /* Disable SPI peripheral */
      __HAL_SPI_DISABLE(hspi);
    }
    
    /* Disable Rx DMA Request */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    /* Disable Tx DMA Request (done by default to handle the case Master RX direction 2 lines) */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    
    hspi->RxXferCount = 0;
    hspi->State = HAL_SPI_STATE_READY;
    
    
    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      /* Wait until RXNE flag is set to send data */
      if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
      }
      
      /* Read CRC */
      tmpreg = hspi->Instance->DR;
      (void)tmpreg; // suppress compiler warning
      
      /* Wait until RXNE flag is set */
      if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, SET, SPI_TIMEOUT_VALUE) != HAL_OK)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
      }
      
      /* Check if CRC error occurred */
      if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
        __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      }
    }
    
    /* Check if Errors has been detected during transfer */
    if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
      HAL_SPI_ErrorCallback(hspi);
    }
    else
    {
      HAL_SPI_RxCpltCallback(hspi);
    } 
  }
  else
  {
    HAL_SPI_RxCpltCallback(hspi);
  }
}

/**
  * @brief DMA SPI transmit receive process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma)   
{
  __IO uint16_t tmpreg;
  
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* Reset CRC Calculation */
    if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
    {
      /* Check if CRC is done on going (RXNE flag set) */
      if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, SET, SPI_TIMEOUT_VALUE) == HAL_OK)
      {
        /* Wait until RXNE flag is set to send data */
        if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
        {
          hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
        }
      }
      /* Read CRC */
      tmpreg = hspi->Instance->DR;
      (void)tmpreg; // suppress compiler warning
      
      /* Check if CRC error occurred */
      if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
      {
        hspi->ErrorCode |= HAL_SPI_ERROR_CRC;
        __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      }
    }

    /* Wait until TXE flag is set to send data */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_TXE, RESET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }
    /* Disable Tx DMA Request */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    
    /* Wait until Busy flag is reset before disabling SPI */
    if(SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_BSY, SET, SPI_TIMEOUT_VALUE) != HAL_OK)
    {
      hspi->ErrorCode |= HAL_SPI_ERROR_FLAG;
    }
    
    /* Disable Rx DMA Request */
    hspi->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    
    hspi->TxXferCount = 0;
    hspi->RxXferCount = 0;
    
    hspi->State = HAL_SPI_STATE_READY;
    
    
    /* Check if Errors has been detected during transfer */
    if(hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
      HAL_SPI_ErrorCallback(hspi);
    }
    else
    {
      HAL_SPI_TxRxCpltCallback(hspi);
    }
  }
  else
  {
    HAL_SPI_TxRxCpltCallback(hspi);
  }
}

/**
  * @brief DMA SPI half transmit process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
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
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  HAL_SPI_RxHalfCpltCallback(hspi);
}

/**
  * @brief DMA SPI Half transmit receive process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma)   
{
  SPI_HandleTypeDef* hspi = ( SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  HAL_SPI_TxRxHalfCpltCallback(hspi);
}

/**
  * @brief DMA SPI communication error callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAError(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef* hspi = (SPI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hspi->TxXferCount = 0;
  hspi->RxXferCount = 0;
  hspi->State= HAL_SPI_STATE_READY;
  hspi->ErrorCode |= HAL_SPI_ERROR_DMA;
  HAL_SPI_ErrorCallback(hspi);
}

/**
  * @brief This function handles SPI Communication Timeout.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status, uint32_t Timeout)  
{
  uint32_t tickstart = 0;

  /* Get tick */ 
  tickstart = HAL_GetTick();

  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_SPI_GET_FLAG(hspi, Flag) == RESET)
    {
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Disable the SPI and reset the CRC: the CRC value should be cleared
             on both master and slave sides in order to resynchronize the master
             and slave for their respective CRC calculation */

          /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
          __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);

          /* Reset CRC Calculation */
          if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
          {
            __HAL_SPI_RESET_CRC(hspi);
          }

          hspi->State= HAL_SPI_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_SPI_GET_FLAG(hspi, Flag) != RESET)
    {
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Disable the SPI and reset the CRC: the CRC value should be cleared
             on both master and slave sides in order to resynchronize the master
             and slave for their respective CRC calculation */

          /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
          __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);

          /* Reset CRC Calculation */
          if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLED)
          {
            __HAL_SPI_RESET_CRC(hspi);
          }

          hspi->State= HAL_SPI_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
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
