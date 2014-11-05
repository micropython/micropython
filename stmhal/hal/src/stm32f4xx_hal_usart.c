/**
  ******************************************************************************
  * @file    stm32f4xx_hal_usart.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   USART HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Universal Synchronous Asynchronous Receiver Transmitter (USART) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The USART HAL driver can be used as follows:

    (#) Declare a USART_HandleTypeDef handle structure.
    (#) Initialize the USART low level resources by implementing the HAL_USART_MspInit () API:
        (##) Enable the USARTx interface clock.
        (##) USART pins configuration:
             (+++) Enable the clock for the USART GPIOs.
             (+++) Configure these USART pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_USART_Transmit_IT(),
             HAL_USART_Receive_IT() and HAL_USART_TransmitReceive_IT() APIs):
             (+++) Configure the USARTx interrupt priority.
             (+++) Enable the NVIC USART IRQ handle.
        (##) DMA Configuration if you need to use DMA process (HAL_USART_Transmit_DMA()
             HAL_USART_Receive_IT() and HAL_USART_TransmitReceive_IT() APIs):
             (+++) Declare a DMA handle structure for the Tx/Rx stream.
             (+++) Enable the DMAx interface clock.
             (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
             (+++) Configure the DMA Tx/Rx Stream.
             (+++) Associate the initilalized DMA handle to the USART DMA Tx/Rx handle.
             (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx/Rx Stream.

    (#) Program the Baud Rate, Word Length, Stop Bit, Parity, Hardware 
        flow control and Mode(Receiver/Transmitter) in the husart Init structure.

    (#) Initialize the USART registers by calling the HAL_USART_Init() API:
        (++) These APIs configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
             by calling the customed HAL_USART_MspInit(&husart) API.
                    
        -@@- The specific USART interrupts (Transmission complete interrupt, 
             RXNE interrupt and Error Interrupts) will be managed using the macros
             __USART_ENABLE_IT() and __USART_DISABLE_IT() inside the transmit and receive process.
          
    (#) Three operation modes are available within this driver :     
  
     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Send an amount of data in blocking mode using HAL_USART_Transmit() 
       (+) Receive an amount of data in blocking mode using HAL_USART_Receive()
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Send an amount of data in non blocking mode using HAL_USART_Transmit_IT() 
       (+) At transmission end of half transfer HAL_USART_TxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_TxHalfCpltCallback 
       (+) At transmission end of transfer HAL_USART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode using HAL_USART_Receive_IT() 
       (+) At reception end of half transfer HAL_USART_RxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_RxHalfCpltCallback 
       (+) At reception end of transfer HAL_USART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_RxCpltCallback                                      
       (+) In case of transfer Error, HAL_USART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_USART_ErrorCallback
    
     *** DMA mode IO operation ***    
     ==============================
     [..] 
       (+) Send an amount of data in non blocking mode (DMA) using HAL_USART_Transmit_DMA() 
       (+) At transmission end of half transfer HAL_USART_TxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_TxHalfCpltCallback 
       (+) At transmission end of transfer HAL_USART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode (DMA) using HAL_USART_Receive_DMA() 
       (+) At reception end of half transfer HAL_USART_RxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_RxHalfCpltCallback 
       (+) At reception end of transfer HAL_USART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_USART_RxCpltCallback                                      
       (+) In case of transfer Error, HAL_USART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_USART_ErrorCallback
       (+) Pause the DMA Transfer using HAL_USART_DMAPause()      
       (+) Resume the DMA Transfer using HAL_USART_DMAResume()  
       (+) Stop the DMA Transfer using HAL_USART_DMAStop()      
     
     *** USART HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in USART HAL driver.
       
       (+) __HAL_USART_ENABLE: Enable the USART peripheral 
       (+) __HAL_USART_DISABLE: Disable the USART peripheral     
       (+) __HAL_USART_GET_FLAG : Check whether the specified USART flag is set or not
       (+) __HAL_USART_CLEAR_FLAG : Clear the specified USART pending flag
       (+) __HAL_USART_ENABLE_IT: Enable the specified USART interrupt
       (+) __HAL_USART_DISABLE_IT: Disable the specified USART interrupt
      
     [..] 
       (@) You can refer to the USART HAL driver header file for more useful macros

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

/** @defgroup USART 
  * @brief HAL USART Synchronous module driver
  * @{
  */
#ifdef HAL_USART_MODULE_ENABLED
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DUMMY_DATA      0xFFFF
#define USART_TIMEOUT_VALUE  22000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef USART_Transmit_IT(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_Receive_IT(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_TransmitReceive_IT(USART_HandleTypeDef *husart);
static void USART_SetConfig (USART_HandleTypeDef *husart);
static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAError(DMA_HandleTypeDef *hdma); 
static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
/* Private functions ---------------------------------------------------------*/


/** @defgroup USART_Private_Functions
  * @{
  */

/** @defgroup USART_Group1 USART Initialization and de-initialization functions 
  *  @brief    Initialization and Configuration functions 
  *
@verbatim
  ==============================================================================
              ##### Initialization and Configuration functions #####
  ==============================================================================
  [..]
  This subsection provides a set of functions allowing to initialize the USART 
  in asynchronous and in synchronous modes.
  (+) For the asynchronous mode only these parameters can be configured: 
      (++) Baud Rate
      (++) Word Length 
      (++) Stop Bit
      (++) Parity: If the parity is enabled, then the MSB bit of the data written
           in the data register is transmitted but is changed by the parity bit.
           Depending on the frame length defined by the M bit (8-bits or 9-bits),
           please refer to Reference manual for possible USART frame formats.   
      (++) USART polarity
      (++) USART phase
      (++) USART LastBit
      (++) Receiver/transmitter modes

  [..]
    The HAL_USART_Init() function follows the USART  synchronous configuration 
    procedure (details for the procedure are available in reference manual (RM0329)).

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the USART mode according to the specified
  *         parameters in the USART_InitTypeDef and create the associated handle.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart)
{
  /* Check the USART handle allocation */
  if(husart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  if(husart->State == HAL_USART_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_USART_MspInit(husart);
  }
  
  husart->State = HAL_USART_STATE_BUSY;  

  /* Set the USART Communication parameters */
  USART_SetConfig(husart);

  /* In USART mode, the following bits must be kept cleared: 
     - LINEN bit in the USART_CR2 register
     - HDSEL, SCEN and IREN bits in the USART_CR3 register */
  husart->Instance->CR2 &= ~USART_CR2_LINEN;
  husart->Instance->CR3 &= ~(USART_CR3_IREN | USART_CR3_SCEN | USART_CR3_HDSEL);

  /* Enable the Peripheral */
  __USART_ENABLE(husart);

  /* Initialize the USART state */
  husart->ErrorCode = HAL_USART_ERROR_NONE;
  husart->State= HAL_USART_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the USART peripheral.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart)
{
   /* Check the USART handle allocation */
  if(husart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  husart->State = HAL_USART_STATE_BUSY;

  /* DeInit the low level hardware */
  HAL_USART_MspDeInit(husart);

  husart->ErrorCode = HAL_USART_ERROR_NONE;
  husart->State = HAL_USART_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/**
  * @brief  USART MSP Init.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
 __weak void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_MspInit could be implenetd in the user file
   */ 
}

/**
  * @brief  USART MSP DeInit.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
 __weak void HAL_USART_MspDeInit(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_MspDeInit could be implenetd in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup USART_Group2 IO operation functions 
  *  @brief   USART Transmit and Receive functions 
  *
@verbatim
  ==============================================================================
                         ##### IO operation functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to manage the USART synchronous
    data transfers.
      
  [..] 
    The USART supports master mode only: it cannot receive or send data related to an input
    clock (SCLK is always an output).

    (#) There are two modes of transfer:
        (++) Blocking mode: The communication is performed in polling mode. 
             The HAL status of all data processing is returned by the same function 
             after finishing transfer.  
        (++) No-Blocking mode: The communication is performed using Interrupts 
             or DMA, These API's return the HAL status.
             The end of the data processing will be indicated through the 
             dedicated USART IRQ when using Interrupt mode or the DMA IRQ when 
             using DMA mode.
             The HAL_USART_TxCpltCallback(), HAL_USART_RxCpltCallback() and HAL_USART_TxRxCpltCallback() 
              user callbacks 
             will be executed respectivelly at the end of the transmit or Receive process
             The HAL_USART_ErrorCallback() user callback will be executed when a communication 
             error is detected

    (#) Blocking mode APIs are :
        (++) HAL_USART_Transmit() in simplex mode
        (++) HAL_USART_Receive() in full duplex receive only
        (++) HAL_USART_TransmitReceive() in full duplex mode
        
    (#) Non Blocking mode APIs with Interrupt are :
        (++) HAL_USART_Transmit_IT()in simplex mode
        (++) HAL_USART_Receive_IT() in full duplex receive only
        (++) HAL_USART_TransmitReceive_IT() in full duplex mode
        (++) HAL_USART_IRQHandler()

    (#) Non Blocking mode functions with DMA are :
        (++) HAL_USART_Transmit_DMA()in simplex mode
        (++) HAL_USART_Receive_DMA() in full duplex receive only
        (++) HAL_USART_TransmitReceie_DMA() in full duplex mode
          
    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_USART_TxCpltCallback()
        (++) HAL_USART_RxCpltCallback()
        (++) HAL_USART_ErrorCallback()
        (++) HAL_USART_TxRxCpltCallback()
      
@endverbatim
  * @{
  */

/**
  * @brief  Simplex Send an amount of data in blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    while(husart->TxXferCount > 0)
    {
      husart->TxXferCount--;
      if(husart->Init.WordLength == USART_WORDLENGTH_9B)
      {
        /* Wait for TC flag in order to write data in DR */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pTxData;
        husart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          pTxData += 2;
        }
        else
        {
          pTxData += 1;
        }
      }
      else
      {
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        husart->Instance->DR = (*pTxData++ & (uint8_t)0xFF);
      }
    }

    if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TC, RESET, Timeout) != HAL_OK)
    { 
      return HAL_TIMEOUT;
    }

    husart->State = HAL_USART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Full-Duplex Receive an amount of data in blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pRxData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
    /* Check the remain data to be received */
    while(husart->RxXferCount > 0)
    {
      husart->RxXferCount--;
      if(husart->Init.WordLength == USART_WORDLENGTH_9B)
      {
        /* Wait until TXE flag is set to send dummy byte in order to generate the clock for the slave to send data */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        /* Send dummy byte in order to generate clock */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF);
        
        /* Wait for RXNE Flag */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pRxData ;
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
          pRxData +=2;
        }
        else
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
          pRxData +=1;
        }
      }
      else
      {
        /* Wait until TXE flag is set to send dummy byte in order to generate the clock for the slave to send data */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }

        /* Send Dummy Byte in order to generate clock */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x00FF);

        /* Wait until RXNE flag is set to receive the byte */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
        }
        
      }
    }

    husart->State = HAL_USART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Full-Duplex Send receive an amount of data in full-duplex mode (blocking mode). 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data transmitted buffer
  * @param  pRxData: Pointer to data received buffer  
  * @param  Size: Amount of data to be sent
   * @param  Timeout: Timeout duration    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    husart->RxXferSize = Size;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    husart->RxXferCount = Size;

    /* Check the remain data to be received */
    while(husart->TxXferCount > 0)
    {
      husart->TxXferCount--;
      husart->RxXferCount--;
      if(husart->Init.WordLength == USART_WORDLENGTH_9B)
      {
        /* Wait for TC flag in order to write data in DR */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pTxData;
        husart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          pTxData += 2;
        }
        else
        {
          pTxData += 1;
        }
        
        /* Wait for RXNE Flag */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pRxData ;
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
          pRxData += 2;
        }
        else
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
          pRxData += 1;
        }
      } 
      else
      {
        /* Wait for TC flag in order to write data in DR */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        husart->Instance->DR = (*pTxData++ & (uint8_t)0x00FF);

        /* Wait for RXNE Flag */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if(husart->Init.Parity == USART_PARITY_NONE)
        {
          /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
        }
      }
    }

    husart->State = HAL_USART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Simplex Send an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  * @note   The USART errors are not managed to avoid the overrun error.
  */
HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

    /* The USART Error Interrupts: (Frame error, Noise error, Overrun error) 
       are not managed by the USART transmit process to avoid the overrun interrupt
       when the USART mode is configured for transmit and receive "USART_MODE_TX_RX"
       to benefit for the frame error and noise interrupts the USART mode should be
       configured only for transmit "USART_MODE_TX"
       The __USART_ENABLE_IT(husart, USART_IT_ERR) can be used to enable the Frame error,
       Noise error interrupt */

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Enable the USART Transmit Data Register Empty Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Simplex Receive an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pRxData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    /* Enable the USART Data Register not empty Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_RXNE); 

    /* Enable the USART Parity Error Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_PE);

    /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    __USART_ENABLE_IT(husart, USART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Send dummy byte in order to generate the clock for the slave to send data */
    husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF);    

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Full-Duplex Send receive an amount of data in full-duplex mode (non-blocking). 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data transmitted buffer
  * @param  pRxData: Pointer to data received buffer 
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData,  uint16_t Size)
{
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;

    /* Enable the USART Data Register not empty Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_RXNE); 

    /* Enable the USART Parity Error Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_PE);

    /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    __USART_ENABLE_IT(husart, USART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Enable the USART Transmit Data Register Empty Interrupt */
    __USART_ENABLE_IT(husart, USART_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @brief  Simplex Send an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  uint32_t *tmp;
  
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);  

    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

    /* Set the USART DMA transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

    /* Set the DMA error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

    /* Enable the USART transmit DMA Stream */
    tmp = (uint32_t*)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->DR, Size);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    husart->Instance->CR3 |= USART_CR3_DMAT;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Full-Duplex Receive an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pRxData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  * @note   The USART DMA transmit stream must be configured in order to generate the clock for the slave.
  * @note   When the USART parity is enabled (PCE = 1) the data received contain the parity bit.
  */
HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;
  
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pRxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

    /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

    /* Enable the USART receive DMA Stream */
    tmp = (uint32_t*)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->DR, *(uint32_t*)tmp, Size);

    /* Enable the USART transmit DMA Stream: the transmit stream is used in order
       to generate in the non-blocking mode the clock to the slave device, 
       this mode isn't a simplex receive mode but a full-duplex receive one */
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->DR, Size);

    /* Clear the Overrun flag just before enabling the DMA Rx request: mandatory for the second transfer
       when using the USART in circular mode */
    __HAL_USART_CLEAR_OREFLAG(husart);
    
    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
       in the USART CR3 register */
    husart->Instance->CR3 |= USART_CR3_DMAR;

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    husart->Instance->CR3 |= USART_CR3_DMAT;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Full-Duplex Transmit Receive an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  pTxData: Pointer to data transmitted buffer
  * @param  pRxData: Pointer to data received buffer 
  * @param  Size: Amount of data to be received
  * @note   When the USART parity is enabled (PCE = 1) the data received contain the parity bit.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;
  
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;

    /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

    /* Set the USART DMA Tx transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

    /* Set the USART DMA Tx transfer error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

    /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

    /* Enable the USART receive DMA Stream */
    tmp = (uint32_t*)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->DR, *(uint32_t*)tmp, Size);

    /* Enable the USART transmit DMA Stream */
    tmp = (uint32_t*)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->DR, Size);
    
    /* Clear the Overrun flag: mandatory for the second transfer in circular mode */
    __HAL_USART_CLEAR_OREFLAG(husart);
    
    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
       in the USART CR3 register */
    husart->Instance->CR3 |= USART_CR3_DMAR;

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    husart->Instance->CR3 |= USART_CR3_DMAT;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Pauses the DMA Transfer.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart)
{
  /* Process Locked */
  __HAL_LOCK(husart);
  
  /* Disable the USART DMA Tx request */
  husart->Instance->CR3 &= (uint32_t)(~USART_CR3_DMAT);
  
  /* Process Unlocked */
  __HAL_UNLOCK(husart);
  
  return HAL_OK; 
}

/**
  * @brief Resumes the DMA Transfer.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart)
{
  /* Process Locked */
  __HAL_LOCK(husart);
  
  /* Enable the USART DMA Tx request */
  husart->Instance->CR3 |= USART_CR3_DMAT;
  
  /* Process Unlocked */
  __HAL_UNLOCK(husart);
  
  return HAL_OK;
}

/**
  * @brief Stops the DMA Transfer.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart)
{
  /* The Lock is not implemented on this API to allow the user application
     to call the HAL USART API under callbacks HAL_USART_TxCpltCallback() / HAL_USART_RxCpltCallback():
     when calling HAL_DMA_Abort() API the DMA TX/RX Transfer complete interrupt is generated
     and the correspond call back is executed HAL_USART_TxCpltCallback() / HAL_USART_RxCpltCallback()
     */

  /* Abort the USART DMA Tx Stream */
  if(husart->hdmatx != NULL)
  {
    HAL_DMA_Abort(husart->hdmatx);
  }
  /* Abort the USART DMA Rx Stream */
  if(husart->hdmarx != NULL)
  {  
    HAL_DMA_Abort(husart->hdmarx);
  }
  
  /* Disable the USART Tx/Rx DMA requests */
  husart->Instance->CR3 &= ~USART_CR3_DMAT;
  husart->Instance->CR3 &= ~USART_CR3_DMAR;
  
  husart->State = HAL_USART_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  This function handles USART interrupt request.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
void HAL_USART_IRQHandler(USART_HandleTypeDef *husart)
{
  uint32_t tmp1 = 0, tmp2 = 0;
  
  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_PE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_PE);
  /* USART parity error interrupt occurred -----------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_USART_CLEAR_PEFLAG(husart);
    husart->ErrorCode |= HAL_USART_ERROR_PE;
  }

  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_FE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_ERR);
  /* USART frame error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_USART_CLEAR_FEFLAG(husart);
    husart->ErrorCode |= HAL_USART_ERROR_FE;
  }

  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_NE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_ERR);
  /* USART noise error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_USART_CLEAR_NEFLAG(husart);
    husart->ErrorCode |= HAL_USART_ERROR_NE;
  }

  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_ORE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_ERR);
  /* USART Over-Run interrupt occurred ---------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    __HAL_USART_CLEAR_OREFLAG(husart);
    husart->ErrorCode |= HAL_USART_ERROR_ORE;
  }

  if(husart->ErrorCode != HAL_USART_ERROR_NONE)
  {
    /* Set the USART state ready to be able to start again the process */
    husart->State = HAL_USART_STATE_READY;
    
    HAL_USART_ErrorCallback(husart);
  }

  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_RXNE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_RXNE);
  /* USART in mode Receiver --------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    if(husart->State == HAL_USART_STATE_BUSY_RX)
    {
      USART_Receive_IT(husart);
    }
    else
    {
      USART_TransmitReceive_IT(husart);
    }
  }

  tmp1 = __HAL_USART_GET_FLAG(husart, USART_FLAG_TXE);
  tmp2 = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_TXE);
  /* USART in mode Transmitter -----------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    if(husart->State == HAL_USART_STATE_BUSY_TX)
    {
      USART_Transmit_IT(husart);
    }
    else
    {
      USART_TransmitReceive_IT(husart);
    }
  }
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
 __weak void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
 __weak void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
__weak void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Half Transfer completed callbacks.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
__weak void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx/Rx Transfers completed callback for the non-blocking process.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
__weak void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  USART error callbacks.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
 __weak void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_USART_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup USART_Group3 Peripheral State and Errors functions 
  *  @brief   USART State and Errors functions 
  *
@verbatim   
  ==============================================================================
                  ##### Peripheral State and Errors functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to return the State of 
    USART communication
    process, return Peripheral Errors occurred during communication process
     (+) HAL_USART_GetState() API can be helpful to check in run-time the state 
         of the USART peripheral.
     (+) HAL_USART_GetError() check in run-time errors that could be occurred during 
         communication. 
@endverbatim
  * @{
  */

/**
  * @brief  Returns the USART state.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL state
  */
HAL_USART_StateTypeDef HAL_USART_GetState(USART_HandleTypeDef *husart)
{
  return husart->State;
}

/**
  * @brief  Return the USART error code
  * @param  husart : pointer to a USART_HandleTypeDef structure that contains
  *              the configuration information for the specified USART.
  * @retval USART Error Code
  */
uint32_t HAL_USART_GetError(USART_HandleTypeDef *husart)
{
  return husart->ErrorCode;
}

/**
  * @}
  */
  

/**
  * @brief  DMA USART transmit process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = ( USART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* DMA Normal mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    husart->TxXferCount = 0;
    if(husart->State == HAL_USART_STATE_BUSY_TX)
    {
      /* Wait for USART TC Flag */
      if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TC, RESET, USART_TIMEOUT_VALUE) != HAL_OK)
      {
        /* Timeout occurred */ 
        husart->State = HAL_USART_STATE_TIMEOUT;
        HAL_USART_ErrorCallback(husart);
      }
      else
      {
        /* No Timeout */
        /* Disable the DMA transfer for transmit request by setting the DMAT bit
         in the USART CR3 register */
        husart->Instance->CR3 &= ~(USART_CR3_DMAT);
        husart->State= HAL_USART_STATE_READY;
        HAL_USART_TxCpltCallback(husart);
      }
    }
  }
  /* DMA Circular mode */
  else
  {
    if(husart->State == HAL_USART_STATE_BUSY_TX)
    {
      HAL_USART_TxCpltCallback(husart);
    }
  }
}

/**
  * @brief DMA USART transmit process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_USART_TxHalfCpltCallback(husart);
}

/**
  * @brief  DMA USART receive process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = ( USART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* DMA Normal mode */
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    husart->RxXferCount = 0;
    husart->State= HAL_USART_STATE_READY;
    if(husart->State == HAL_USART_STATE_BUSY_RX)
    {
      /* Disable the DMA transfer for the Transmit/receiver requests by setting the DMAT/DMAR bit 
         in the USART CR3 register */
      husart->Instance->CR3 &= ~(USART_CR3_DMAR);

      HAL_USART_RxCpltCallback(husart);
    }
    /* the usart state is HAL_USART_STATE_BUSY_TX_RX*/
    else
    {
      /* Disable the DMA transfer for the Transmit/receiver requests by setting the DMAT/DMAR bit 
         in the USART CR3 register */
      husart->Instance->CR3 &= ~(USART_CR3_DMAR);
      husart->Instance->CR3 &= ~(USART_CR3_DMAT);

      HAL_USART_TxRxCpltCallback(husart);
    }
  }
  /* DMA circular mode */
  else
  {
    if(husart->State == HAL_USART_STATE_BUSY_RX)
    {
      HAL_USART_RxCpltCallback(husart);
    }
    /* the usart state is HAL_USART_STATE_BUSY_TX_RX*/
    else
    {
      HAL_USART_TxRxCpltCallback(husart);
    }
  }
}

/**
  * @brief DMA USART receive process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_USART_RxHalfCpltCallback(husart); 
}

/**
  * @brief  DMA USART communication error callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void USART_DMAError(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = ( USART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  husart->RxXferCount = 0;
  husart->TxXferCount = 0;
  husart->ErrorCode |= HAL_USART_ERROR_DMA;
  husart->State= HAL_USART_STATE_READY;
  
  HAL_USART_ErrorCallback(husart);
}

/**
  * @brief  This function handles USART Communication Timeout.
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @param  Flag: specifies the USART flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Get tick */ 
  tickstart = HAL_GetTick();

  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_USART_GET_FLAG(husart, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __USART_DISABLE_IT(husart, USART_IT_TXE);
          __USART_DISABLE_IT(husart, USART_IT_RXNE);
          __USART_DISABLE_IT(husart, USART_IT_PE);
          __USART_DISABLE_IT(husart, USART_IT_ERR);

          husart->State= HAL_USART_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(husart);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_USART_GET_FLAG(husart, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __USART_DISABLE_IT(husart, USART_IT_TXE);
          __USART_DISABLE_IT(husart, USART_IT_RXNE);
          __USART_DISABLE_IT(husart, USART_IT_PE);
          __USART_DISABLE_IT(husart, USART_IT_ERR);

          husart->State= HAL_USART_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(husart);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
}

  
/**
  * @brief  Simplex Send an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  * @note   The USART errors are not managed to avoid the overrun error.
  */
static HAL_StatusTypeDef USART_Transmit_IT(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
 
  if(husart->State == HAL_USART_STATE_BUSY_TX)
  {
    if(husart->Init.WordLength == USART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) husart->pTxBuffPtr;
      husart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if(husart->Init.Parity == USART_PARITY_NONE)
      {
        husart->pTxBuffPtr += 2;
      }
      else
      {
        husart->pTxBuffPtr += 1;
      }
    } 
    else
    { 
      husart->Instance->DR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }
    
    if(--husart->TxXferCount == 0)
    {
      /* Disable the USART Transmit data register empty Interrupt */
      __USART_DISABLE_IT(husart, USART_IT_TXE);

      /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      __USART_DISABLE_IT(husart, USART_IT_ERR);


      if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TC, RESET, USART_TIMEOUT_VALUE) != HAL_OK)
      { 
        return HAL_TIMEOUT;
      }
      husart->State = HAL_USART_STATE_READY;
      
      HAL_USART_TxCpltCallback(husart);

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Simplex Receive an amount of data in non-blocking mode. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_Receive_IT(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
  if(husart->State == HAL_USART_STATE_BUSY_RX)
  {
    if(husart->Init.WordLength == USART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) husart->pRxBuffPtr;
      if(husart->Init.Parity == USART_PARITY_NONE)
      {
        *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
        husart->pRxBuffPtr += 2;
      }
      else
      {
        *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
        husart->pRxBuffPtr += 1;
      }
      if(--husart->RxXferCount != 0x00) 
      {
        /* Send dummy byte in order to generate the clock for the slave to send the next data */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF); 
      }
    } 
    else
    {
      if(husart->Init.Parity == USART_PARITY_NONE)
      {
        *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
      }

      if(--husart->RxXferCount != 0x00) 
      {
        /* Send dummy byte in order to generate the clock for the slave to send the next data */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x00FF);  
      }
    }

    if(husart->RxXferCount == 0)
    {
      /* Disable the USART RXNE Interrupt */
      __USART_DISABLE_IT(husart, USART_IT_RXNE);

      /* Disable the USART Parity Error Interrupt */
      __USART_DISABLE_IT(husart, USART_IT_PE);

      /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      __USART_DISABLE_IT(husart, USART_IT_ERR);

      husart->State = HAL_USART_STATE_READY;
      HAL_USART_RxCpltCallback(husart);
      
      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @brief  Full-Duplex Send receive an amount of data in full-duplex mode (non-blocking). 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_TransmitReceive_IT(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;

  if(husart->State == HAL_USART_STATE_BUSY_TX_RX)
  {
    if(husart->TxXferCount != 0x00)
    {
      if(__HAL_USART_GET_FLAG(husart, USART_FLAG_TXE) != RESET)
      {
        if(husart->Init.WordLength == USART_WORDLENGTH_9B)
        {
          tmp = (uint16_t*) husart->pTxBuffPtr;
          husart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
          if(husart->Init.Parity == USART_PARITY_NONE)
          {
            husart->pTxBuffPtr += 2;
          }
          else
          {
            husart->pTxBuffPtr += 1;
          }
        } 
        else
        {
          husart->Instance->DR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0x00FF);
        }
        husart->TxXferCount--;

        /* Check the latest data transmitted */
        if(husart->TxXferCount == 0)
        {
           __USART_DISABLE_IT(husart, USART_IT_TXE);
        }
      }
    }

    if(husart->RxXferCount != 0x00)
    {
      if(__HAL_USART_GET_FLAG(husart, USART_FLAG_RXNE) != RESET)
      {
        if(husart->Init.WordLength == USART_WORDLENGTH_9B)
        {
          tmp = (uint16_t*) husart->pRxBuffPtr;
          if(husart->Init.Parity == USART_PARITY_NONE)
          {
            *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
            husart->pRxBuffPtr += 2;
          }
          else
          {
            *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
            husart->pRxBuffPtr += 1;
          }
        } 
        else
        {
          if(husart->Init.Parity == USART_PARITY_NONE)
          {
            *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
          }
          else
          {
            *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
          }
        }
        husart->RxXferCount--;
      }
    }

    /* Check the latest data received */
    if(husart->RxXferCount == 0)
    {
      __USART_DISABLE_IT(husart, USART_IT_RXNE);

      /* Disable the USART Parity Error Interrupt */
      __USART_DISABLE_IT(husart, USART_IT_PE);

      /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      __USART_DISABLE_IT(husart, USART_IT_ERR);
      
      husart->State = HAL_USART_STATE_READY;

      HAL_USART_TxRxCpltCallback(husart);

      return HAL_OK;
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @brief  Configures the USART peripheral. 
  * @param  husart: pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
static void USART_SetConfig(USART_HandleTypeDef *husart)
{
  uint32_t tmpreg = 0x00;

  /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));
  assert_param(IS_USART_POLARITY(husart->Init.CLKPolarity));
  assert_param(IS_USART_PHASE(husart->Init.CLKPhase));
  assert_param(IS_USART_LASTBIT(husart->Init.CLKLastBit));
  assert_param(IS_USART_BAUDRATE(husart->Init.BaudRate));  
  assert_param(IS_USART_WORD_LENGTH(husart->Init.WordLength));
  assert_param(IS_USART_STOPBITS(husart->Init.StopBits));
  assert_param(IS_USART_PARITY(husart->Init.Parity));
  assert_param(IS_USART_MODE(husart->Init.Mode));

  /* The LBCL, CPOL and CPHA bits have to be selected when both the transmitter and the
     receiver are disabled (TE=RE=0) to ensure that the clock pulses function correctly. */
  husart->Instance->CR1 &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));

  /*---------------------------- USART CR2 Configuration ---------------------*/
  tmpreg = husart->Instance->CR2;
  /* Clear CLKEN, CPOL, CPHA and LBCL bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN | USART_CR2_LBCL | USART_CR2_STOP));
  /* Configure the USART Clock, CPOL, CPHA and LastBit -----------------------*/
  /* Set CPOL bit according to husart->Init.CLKPolarity value */
  /* Set CPHA bit according to husart->Init.CLKPhase value */
  /* Set LBCL bit according to husart->Init.CLKLastBit value */
  /* Set Stop Bits: Set STOP[13:12] bits according to husart->Init.StopBits value */
  tmpreg |= (uint32_t)(USART_CLOCK_ENABLED| husart->Init.CLKPolarity | 
                      husart->Init.CLKPhase| husart->Init.CLKLastBit | husart->Init.StopBits);
  /* Write to USART CR2 */
  husart->Instance->CR2 = (uint32_t)tmpreg;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = husart->Instance->CR1;

  /* Clear M, PCE, PS, TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | \
                                   USART_CR1_RE));

  /* Configure the USART Word Length, Parity and mode: 
     Set the M bits according to husart->Init.WordLength value 
     Set PCE and PS bits according to husart->Init.Parity value
     Set TE and RE bits according to husart->Init.Mode value */
  tmpreg |= (uint32_t)husart->Init.WordLength | husart->Init.Parity | husart->Init.Mode;

  /* Write to USART CR1 */
  husart->Instance->CR1 = (uint32_t)tmpreg;

  /*-------------------------- USART CR3 Configuration -----------------------*/  
  /* Clear CTSE and RTSE bits */
  husart->Instance->CR3 &= (uint32_t)~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE));

  /*-------------------------- USART BRR Configuration -----------------------*/
  if((husart->Instance == USART1) || (husart->Instance == USART6))
  {
    husart->Instance->BRR = __USART_BRR(HAL_RCC_GetPCLK2Freq(), husart->Init.BaudRate);
  }
  else
  {
    husart->Instance->BRR = __USART_BRR(HAL_RCC_GetPCLK1Freq(), husart->Init.BaudRate);
  }
}

/**
  * @}
  */

#endif /* HAL_USART_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
