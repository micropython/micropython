/**
  ******************************************************************************
  * @file    stm32f4xx_hal_uart.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   UART HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Universal Asynchronous Receiver Transmitter (UART) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions  
  *           + Peripheral State and Errors functions  
  *           
  @verbatim       
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The UART HAL driver can be used as follows:
    
    (#) Declare a UART_HandleTypeDef handle structure.
  
    (#) Initialize the UART low level resources by implement the HAL_UART_MspInit() API:
        (##) Enable the USARTx interface clock.
        (##) UART pins configuration:
            (+++) Enable the clock for the UART GPIOs.
            (+++) Configure these UART pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_UART_Transmit_IT()
             and HAL_UART_Receive_IT() APIs):
            (+++) Configure the USARTx interrupt priority.
            (+++) Enable the NVIC USART IRQ handle.
        (##) DMA Configuration if you need to use DMA process (HAL_UART_Transmit_DMA()
             and HAL_UART_Receive_DMA() APIs):
            (+++) Declare a DMA handle structure for the Tx/Rx stream.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required 
                  Tx/Rx parameters.                
            (+++) Configure the DMA Tx/Rx Stream.
            (+++) Associate the initialized DMA handle to the UART DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete 
                  interrupt on the DMA Tx/Rx Stream.

    (#) Program the Baud Rate, Word Length, Stop Bit, Parity, Hardware 
        flow control and Mode(Receiver/Transmitter) in the huart Init structure.

    (#) For the UART asynchronous mode, initialize the UART registers by calling
        the HAL_UART_Init() API.
    
    (#) For the UART Half duplex mode, initialize the UART registers by calling 
        the HAL_HalfDuplex_Init() API.
    
    (#) For the LIN mode, initialize the UART registers by calling the HAL_LIN_Init() API.
    
    (#) For the Multi-Processor mode, initialize the UART registers by calling 
        the HAL_MultiProcessor_Init() API.
        
        -@- The specific UART interrupts (Transmission complete interrupt, 
            RXNE interrupt and Error Interrupts) will be managed using the macros
            __HAL_UART_ENABLE_IT() and __HAL_UART_DISABLE_IT() inside the transmit 
            and receive process.
          
        -@- These API's(HAL_UART_Init() and HAL_HalfDuplex_Init()) configures also the 
            low level Hardware GPIO, CLOCK, CORTEX...etc) by calling the customed 
            HAL_UART_MspInit() API.
          
    (#) Three mode of operations are available within this driver :     
  
     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Send an amount of data in blocking mode using HAL_UART_Transmit() 
       (+) Receive an amount of data in blocking mode using HAL_UART_Receive()
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Send an amount of data in non blocking mode using HAL_UART_Transmit_IT() 
       (+) At transmission end of half transfer HAL_UART_TxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxHalfCpltCallback 
       (+) At transmission end of transfer HAL_UART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode using HAL_UART_Receive_IT() 
       (+) At reception end of half transfer HAL_UART_RxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxHalfCpltCallback 
       (+) At reception end of transfer HAL_UART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxCpltCallback                                      
       (+) In case of transfer Error, HAL_UART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_UART_ErrorCallback

     *** DMA mode IO operation ***    
     ==============================
     [..] 
       (+) Send an amount of data in non blocking mode (DMA) using HAL_UART_Transmit_DMA() 
       (+) At transmission end of half transfer HAL_UART_TxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxHalfCpltCallback 
       (+) At transmission end of transfer HAL_UART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode (DMA) using HAL_UART_Receive_DMA() 
       (+) At reception end of half transfer HAL_UART_RxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxHalfCpltCallback 
       (+) At reception end of transfer HAL_UART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxCpltCallback                                      
       (+) In case of transfer Error, HAL_UART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_UART_ErrorCallback
       (+) Pause the DMA Transfer using HAL_UART_DMAPause()      
       (+) Resume the DMA Transfer using HAL_UART_DMAResume()  
       (+) Stop the DMA Transfer using HAL_UART_DMAStop()      
    
     *** UART HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in UART HAL driver.
       
      (+) __HAL_UART_ENABLE: Enable the UART peripheral 
      (+) __HAL_UART_DISABLE: Disable the UART peripheral     
      (+) __HAL_UART_GET_FLAG : Checks whether the specified UART flag is set or not
      (+) __HAL_UART_CLEAR_FLAG : Clears the specified UART pending flag
      (+) __HAL_UART_ENABLE_IT: Enables the specified UART interrupt
      (+) __HAL_UART_DISABLE_IT: Disables the specified UART interrupt
      
     [..] 
       (@) You can refer to the UART HAL driver header file for more useful macros 
      
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

/** @defgroup UART 
  * @brief HAL UART module driver
  * @{
  */
#ifdef HAL_UART_MODULE_ENABLED
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UART_TIMEOUT_VALUE  22000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void UART_SetConfig (UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAError(DMA_HandleTypeDef *hdma); 
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Timeout);

/* Private functions ---------------------------------------------------------*/

/** @defgroup UART_Private_Functions
  * @{
  */

/** @defgroup UART_Group1 Initialization and de-initialization functions 
  *  @brief    Initialization and Configuration functions 
  *
@verbatim    
===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to initialize the USARTx or the UARTy 
    in asynchronous mode.
      (+) For the asynchronous mode only these parameters can be configured: 
        (++) Baud Rate
        (++) Word Length 
        (++) Stop Bit
        (++) Parity: If the parity is enabled, then the MSB bit of the data written
             in the data register is transmitted but is changed by the parity bit.
             Depending on the frame length defined by the M bit (8-bits or 9-bits),
             the possible UART frame formats are as listed in the following table:
   +-------------------------------------------------------------+     
   |   M bit |  PCE bit  |            UART frame                 |
   |---------------------|---------------------------------------|             
   |    0    |    0      |    | SB | 8 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
   |---------|-----------|---------------------------------------|  
   |    1    |    0      |    | SB | 9 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
   +-------------------------------------------------------------+            
        (++) Hardware flow control
        (++) Receiver/transmitter modes
        (++) Over Sampling Methode
    [..]
    The HAL_UART_Init(), HAL_HalfDuplex_Init(), HAL_LIN_Init() and HAL_MultiProcessor_Init() APIs 
    follow respectively the UART asynchronous, UART Half duplex, LIN and Multi-Processor
    configuration procedures (details for the procedures are available in reference manual (RM0329)).

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }
  
  if(huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* Check the parameters */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_UART_INSTANCE(huart->Instance));
  }
  
  if(huart->State == HAL_UART_STATE_RESET)
  {  
    /* Init the low level hardware */
    HAL_UART_MspInit(huart);
  }
  
  huart->State = HAL_UART_STATE_BUSY;  
  
  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);
  
  /* Set the UART Communication parameters */
  UART_SetConfig(huart);
  
  /* In asynchronous mode, the following bits must be kept cleared: 
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  huart->Instance->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
  huart->Instance->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
  
  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);
  
  /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->State= HAL_UART_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Initializes the half-duplex mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }
  
  if(huart->State == HAL_UART_STATE_RESET)
  {   
    /* Init the low level hardware */
    HAL_UART_MspInit(huart);
  }
  
  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);
  
  /* Set the UART Communication parameters */
  UART_SetConfig(huart);
  
  /* In half-duplex mode, the following bits must be kept cleared: 
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN and IREN bits in the USART_CR3 register.*/
  huart->Instance->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
  huart->Instance->CR3 &= ~(USART_CR3_IREN | USART_CR3_SCEN);
  
  /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
  huart->Instance->CR3 |= USART_CR3_HDSEL;
 
  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);
  
  /* Initialize the UART state*/
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->State= HAL_UART_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Initializes the LIN mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart: UART handle
  * @param  BreakDetectLength: Specifies the LIN break detection length.
  *         This parameter can be one of the following values:
  *            @arg UART_LINBREAKDETECTLENGTH_10B: 10-bit break detection
  *            @arg UART_LINBREAKDETECTLENGTH_11B: 11-bit break detection
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }
  /* Check the Break detection length parameter */
  assert_param(IS_UART_LIN_BREAK_DETECT_LENGTH(BreakDetectLength));

  if(huart->State == HAL_UART_STATE_RESET)
  {   
    /* Init the low level hardware */
    HAL_UART_MspInit(huart);
  }
  
  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);
  
  /* Set the UART Communication parameters */
  UART_SetConfig(huart);
  
  /* In LIN mode, the following bits must be kept cleared: 
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN and IREN bits in the USART_CR3 register.*/
  huart->Instance->CR2 &= ~(USART_CR2_CLKEN);
  huart->Instance->CR3 &= ~(USART_CR3_HDSEL | USART_CR3_IREN | USART_CR3_SCEN);
  
  /* Enable the LIN mode by setting the LINEN bit in the CR2 register */
  huart->Instance->CR2 |= USART_CR2_LINEN;
  
  /* Set the USART LIN Break detection length. */
  huart->Instance->CR2 &= ~(USART_CR2_LBDL);
  huart->Instance->CR2 |= BreakDetectLength; 
  
  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);
  
  /* Initialize the UART state*/
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->State= HAL_UART_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Initializes the Multi-Processor mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart: UART handle
  * @param  Address: USART address
  * @param  WakeUpMethode: specifies the USART wakeup method.
  *          This parameter can be one of the following values:
  *            @arg UART_WAKEUPMETHODE_IDLELINE: Wakeup by an idle line detection
  *            @arg UART_WAKEUPMETHODE_ADDRESSMARK: Wakeup by an address mark
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethode)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Address & wake up method parameters */
  assert_param(IS_UART_WAKEUPMETHODE(WakeUpMethode));
  assert_param(IS_UART_ADDRESS(Address));
  
  if(huart->State == HAL_UART_STATE_RESET)
  {   
    /* Init the low level hardware */
    HAL_UART_MspInit(huart);
  }
  
  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);
  
  /* Set the UART Communication parameters */
  UART_SetConfig(huart);
  
  /* In Multi-Processor mode, the following bits must be kept cleared: 
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register */
  huart->Instance->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
  huart->Instance->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
  
  /* Clear the USART address */
  huart->Instance->CR2 &= ~(USART_CR2_ADD);
  /* Set the USART address node */
  huart->Instance->CR2 |= Address;
  
  /* Set the wake up methode by setting the WAKE bit in the CR1 register */
  huart->Instance->CR1 &= ~(USART_CR1_WAKE);
  huart->Instance->CR1 |= WakeUpMethode;
  
  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);
  
  /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->State= HAL_UART_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  DeInitializes the UART peripheral. 
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if(huart == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  huart->State = HAL_UART_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_UART_MspDeInit(huart);
  
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->State = HAL_UART_STATE_RESET;

  /* Process Lock */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  UART MSP Init.
  * @param  huart: UART handle
  * @retval None
  */
 __weak void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  UART MSP DeInit.
  * @param  huart: UART handle
  * @retval None
  */
 __weak void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup UART_Group2 IO operation functions 
  *  @brief UART Transmit and Receive functions 
  *
@verbatim   
  ==============================================================================
                      ##### IO operation functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to manage the UART asynchronous
    and Half duplex data transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode: The communication is performed in polling mode. 
            The HAL status of all data processing is returned by the same function 
            after finishing transfer.  
       (++) Non-Blocking mode: The communication is performed using Interrupts 
            or DMA, these APIs return the HAL status.
            The end of the data processing will be indicated through the 
            dedicated UART IRQ when using Interrupt mode or the DMA IRQ when 
            using DMA mode.
            The HAL_UART_TxCpltCallback(), HAL_UART_RxCpltCallback() user callbacks 
            will be executed respectivelly at the end of the transmit or receive process.
            The HAL_UART_ErrorCallback() user callback will be executed when 
            a communication error is detected.

    (#) Blocking mode API's are:
        (++) HAL_UART_Transmit()
        (++) HAL_UART_Receive() 
        
    (#) Non-Blocking mode API's with Interrupt are:
        (++) HAL_UART_Transmit_IT()
        (++) HAL_UART_Receive_IT()
        (++) HAL_UART_IRQHandler()

    (#) No-Blocking mode functions with DMA are:
        (++) HAL_UART_Transmit_DMA()
        (++) HAL_UART_Receive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in Non-Blocking mode:
        (++) HAL_UART_TxCpltCallback()
        (++) HAL_UART_RxCpltCallback()
        (++) HAL_UART_ErrorCallback()

    [..] 
      (@) In the Half duplex communication, it is forbidden to run the transmit 
          and receive process in parallel, the UART state HAL_UART_STATE_BUSY_TX_RX 
          can't be useful.
      
@endverbatim
  * @{
  */

/**
  * @brief  Sends an amount of data in blocking mode. 
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State;
  if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_RX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a non-blocking receive process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_RX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_TX;
    }

    huart->TxXferSize = Size;
    huart->TxXferCount = Size;
    while(huart->TxXferCount > 0)
    {
      huart->TxXferCount--;
      if(huart->Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pData;
        huart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          pData +=2;
        }
        else
        { 
          pData +=1;
        }
      } 
      else
      {
        if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        huart->Instance->DR = (*pData++ & (uint8_t)0xFF);
      } 
    }
    
    if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, Timeout) != HAL_OK)
    { 
      return HAL_TIMEOUT;
    }
    
    /* Check if a non-blocking receive process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_READY;
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}

/**
  * @brief  Receives an amount of data in blocking mode. 
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{ 
  uint16_t* tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State;
  if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_TX))
  { 
    if((pData == NULL ) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a non-blocking transmit process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    
    huart->RxXferSize = Size; 
    huart->RxXferCount = Size;
    
    /* Check the remain data to be received */
    while(huart->RxXferCount > 0)
    {
      huart->RxXferCount--;
      if(huart->Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pData ;
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
          pData +=2;
        }
        else
        {
          *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
          pData +=1;
        }

      } 
      else
      {
        if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        { 
          return HAL_TIMEOUT;
        }
        if(huart->Init.Parity == UART_PARITY_NONE)
        {
          *pData++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          *pData++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
        }
        
      }
    }
    
    /* Check if a non-blocking transmit process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX;
    }
    else
    {
      huart->State = HAL_UART_STATE_READY;
    } 
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}

/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t tmp = 0;
  
  tmp = huart->State;
  if((tmp == HAL_UART_STATE_READY) || (tmp == HAL_UART_STATE_BUSY_RX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a receive process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_RX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_TX;
    }

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Transmit Complete Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}

/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t tmp = 0;
  
  tmp = huart->State;  
  if((tmp == HAL_UART_STATE_READY) || (tmp == HAL_UART_STATE_BUSY_TX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    huart->RxXferCount = Size;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a transmit process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    
    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);
    
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @brief  Sends an amount of data in non blocking mode. 
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State;  
  if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_RX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a receive process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_RX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_TX;
    }
    
    /* Set the UART DMA transfer complete callback */
    huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;
    
    /* Set the UART DMA Half transfer complete callback */
    huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt;
    
    /* Set the DMA error callback */
    huart->hdmatx->XferErrorCallback = UART_DMAError;

    /* Enable the UART transmit DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(huart->hdmatx, *(uint32_t*)tmp, (uint32_t)&huart->Instance->DR, Size);
    
    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    huart->Instance->CR3 |= USART_CR3_DMAT;
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}

/**
  * @brief  Receives an amount of data in non blocking mode. 
  * @param  huart: UART handle
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @note   When the UART parity is enabled (PCE = 1) the data received contain the parity bit.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State;    
  if((tmp1 == HAL_UART_STATE_READY) || (tmp1 == HAL_UART_STATE_BUSY_TX))
  {
    if((pData == NULL ) || (Size == 0)) 
    {
      return HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a transmit rocess is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX) 
    {
      huart->State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    
    /* Set the UART DMA transfer complete callback */
    huart->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;
    
    /* Set the UART DMA Half transfer complete callback */
    huart->hdmarx->XferHalfCpltCallback = UART_DMARxHalfCplt;
    
    /* Set the DMA error callback */
    huart->hdmarx->XferErrorCallback = UART_DMAError;

    /* Enable the DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t*)tmp, Size);
    
    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
    in the UART CR3 register */
    huart->Instance->CR3 |= USART_CR3_DMAR;
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}
    
/**
  * @brief Pauses the DMA Transfer.
  * @param huart: UART handle
  * @retval None
  */
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
  /* Process Locked */
  __HAL_LOCK(huart);
  
  if(huart->State == HAL_UART_STATE_BUSY_TX)
  {
    /* Disable the UART DMA Tx request */
    huart->Instance->CR3 &= (uint32_t)(~USART_CR3_DMAT);
  }
  else if(huart->State == HAL_UART_STATE_BUSY_RX)
  {
    /* Disable the UART DMA Rx request */
    huart->Instance->CR3 &= (uint32_t)(~USART_CR3_DMAR);
  }
  else if (huart->State == HAL_UART_STATE_BUSY_TX_RX)
  {
    /* Disable the UART DMA Tx & Rx requests */
    huart->Instance->CR3 &= (uint32_t)(~USART_CR3_DMAT);
    huart->Instance->CR3 &= (uint32_t)(~USART_CR3_DMAR);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @brief Resumes the DMA Transfer.
  * @param huart: UART handle
  * @retval None
  */
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{
  /* Process Locked */
  __HAL_LOCK(huart);
  
  if(huart->State == HAL_UART_STATE_BUSY_TX)
  {
    /* Enable the UART DMA Tx request */
    huart->Instance->CR3 |= USART_CR3_DMAT;
  }
  else if(huart->State == HAL_UART_STATE_BUSY_RX)
  {
    /* Enable the UART DMA Rx request */
    huart->Instance->CR3 |= USART_CR3_DMAR;
  }
  else if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
  {
    /* Enable the UART DMA Tx & Rx request */
    huart->Instance->CR3 |= USART_CR3_DMAT;
    huart->Instance->CR3 |= USART_CR3_DMAR;
  }

  /* If the UART peripheral is still not enabled, enable it */
  if ((huart->Instance->CR1 & USART_CR1_UE) == 0)
  {
    /* Enable UART peripheral */    
    __HAL_UART_ENABLE(huart);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK;
}

/**
  * @brief Stops the DMA Transfer.
  * @param huart: UART handle
  * @retval None
  */
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
  /* Process Locked */
  __HAL_LOCK(huart);
  
  /* Disable the UART Tx/Rx DMA requests */
  huart->Instance->CR3 &= ~USART_CR3_DMAT;
  huart->Instance->CR3 &= ~USART_CR3_DMAR;
  
  /* Abort the UART DMA tx Stream */
  if(huart->hdmatx != NULL)
  {
    HAL_DMA_Abort(huart->hdmatx);
  }
  /* Abort the UART DMA rx Stream */
  if(huart->hdmarx != NULL)
  {
    HAL_DMA_Abort(huart->hdmarx);
  }
  /* Disable UART peripheral */
  __HAL_UART_DISABLE(huart);
  
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK;
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t tmp1 = 0, tmp2 = 0;

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);
    
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
    
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
    
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    UART_Receive_IT(huart);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    UART_Transmit_IT(huart);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
  }
  
  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  }  
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart: UART handle
  * @retval None
  */
 __weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  huart: UART handle
  * @retval None
  */
 __weak void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: UART handle
  * @retval None
  */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Half Transfer completed callbacks.
  * @param  huart: UART handle
  * @retval None
  */
__weak void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  UART error callbacks.
  * @param  huart: UART handle
  * @retval None
  */
 __weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup UART_Group3 Peripheral Control functions 
  *  @brief   UART control functions 
  *
@verbatim   
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control the UART:
    (+) HAL_LIN_SendBreak() API can be helpful to transmit the break character.
    (+) HAL_MultiProcessor_EnterMuteMode() API can be helpful to enter the UART in mute mode. 
    (+) HAL_MultiProcessor_ExitMuteMode() API can be helpful to exit the UART mute mode by software.
    
@endverbatim
  * @{
  */

/**
  * @brief  Transmits break characters.
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));
  
  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->State = HAL_UART_STATE_BUSY;
  
  /* Send break characters */
  huart->Instance->CR1 |= USART_CR1_SBK;
 
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @brief  Enters the UART in mute mode. 
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));
  
  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->State = HAL_UART_STATE_BUSY;
  
  /* Enable the USART mute mode  by setting the RWU bit in the CR1 register */
  huart->Instance->CR1 |= USART_CR1_RWU;
  
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @brief  Exits the UART mute mode: wake up software. 
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));
  
  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->State = HAL_UART_STATE_BUSY;
  
  /* Disable the USART mute mode by clearing the RWU bit in the CR1 register */
  huart->Instance->CR1 &= (uint32_t)~((uint32_t)USART_CR1_RWU);
  
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @brief  Enables the UART transmitter and disables the UART receiver.
  * @param  huart: UART handle
  * @retval HAL status
  * @retval None
  */
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00;

  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->State = HAL_UART_STATE_BUSY;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;
  
  /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));
  
  /* Enable the USART's transmit interface by setting the TE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_TE;
  
  /* Write to USART CR1 */
  huart->Instance->CR1 = (uint32_t)tmpreg;
 
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @brief  Enables the UART receiver and disables the UART transmitter.
  * @param  huart: UART handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00;

  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->State = HAL_UART_STATE_BUSY;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;
  
  /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));
  
  /* Enable the USART's receive interface by setting the RE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_RE;
  
  /* Write to USART CR1 */
  huart->Instance->CR1 = (uint32_t)tmpreg;
  
  huart->State = HAL_UART_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
  
  return HAL_OK; 
}

/**
  * @}
  */

/** @defgroup UART_Group4 Peripheral State and Errors functions 
  *  @brief   UART State and Errors functions 
  *
@verbatim   
  ==============================================================================
                 ##### Peripheral State and Errors functions #####
  ==============================================================================  
 [..]
   This subsection provides a set of functions allowing to return the State of 
   UART communication process, return Peripheral Errors occured during communication 
   process
   (+) HAL_UART_GetState() API can be helpful to check in run-time the state of the UART peripheral.
   (+) HAL_UART_GetError() check in run-time errors that could be occured durung communication. 

@endverbatim
  * @{
  */
  
/**
  * @brief  Returns the UART state.
  * @param  huart: UART handle
  * @retval HAL state
  */
HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart)
{
  return huart->State;
}

/**
* @brief  Return the UART error code
* @param  huart : pointer to a UART_HandleTypeDef structure that contains
  *              the configuration information for the specified UART.
* @retval UART Error Code
*/
uint32_t HAL_UART_GetError(UART_HandleTypeDef *huart)
{
  return huart->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  DMA UART transmit process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)     
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  huart->TxXferCount = 0;
  
  /* Disable the DMA transfer for transmit request by setting the DMAT bit
     in the UART CR3 register */
  huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);

  /* Wait for UART TC Flag */
  if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, UART_TIMEOUT_VALUE) != HAL_OK)
  {
    /* Timeout Occured */ 
    huart->State = HAL_UART_STATE_TIMEOUT;
    HAL_UART_ErrorCallback(huart);
  }
  else
  {
    /* No Timeout */
    /* Check if a receive process is ongoing or not */
    if(huart->State == HAL_UART_STATE_BUSY_TX_RX)
    {
      huart->State = HAL_UART_STATE_BUSY_RX;
    }
    else
    {
      huart->State = HAL_UART_STATE_READY;
    }
    HAL_UART_TxCpltCallback(huart);
  }
}

/**
  * @brief DMA UART transmit process half complete callback 
  * @param hdma : DMA handle
  * @retval None
  */
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef* huart = (UART_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_UART_TxHalfCpltCallback(huart);
}

/**
  * @brief  DMA UART receive process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)  
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  huart->RxXferCount = 0;
  
  /* Disable the DMA transfer for the receiver request by setting the DMAR bit 
     in the UART CR3 register */
  huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAR);

  /* Check if a transmit process is ongoing or not */
  if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
  {
    huart->State = HAL_UART_STATE_BUSY_TX;
  }
  else
  {
    huart->State = HAL_UART_STATE_READY;
  }
  HAL_UART_RxCpltCallback(huart);
}

/**
  * @brief DMA UART receive process half complete callback 
  * @param hdma : DMA handle
  * @retval None
  */
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef* huart = (UART_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_UART_RxHalfCpltCallback(huart); 
}

/**
  * @brief  DMA UART communication error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void UART_DMAError(DMA_HandleTypeDef *hdma)   
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  huart->RxXferCount = 0;
  huart->TxXferCount = 0;
  huart->State= HAL_UART_STATE_READY;
  huart->ErrorCode |= HAL_UART_ERROR_DMA;
  HAL_UART_ErrorCallback(huart);
}

/**
  * @brief  This function handles UART Communication Timeout.
  * @param  huart: UART handle
  * @param  Flag: specifies the UART flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t timeout = 0;
  
  timeout = HAL_GetTick() + Timeout;
  
  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_UART_GET_FLAG(huart, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

          huart->State= HAL_UART_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(huart);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_UART_GET_FLAG(huart, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
          __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

          huart->State= HAL_UART_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(huart);
        
          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;      
}

/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart: UART handle
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  uint16_t* tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State;
  if((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
  {
    /* Process Locked */
    __HAL_LOCK(huart);
    
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        huart->pTxBuffPtr += 2;
      }
      else
      {
        huart->pTxBuffPtr += 1;
      }
    } 
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if(--huart->TxXferCount == 0)
    {
      /* Disable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
      
      /* Check if a receive process is ongoing or not */
      if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
      {
        huart->State = HAL_UART_STATE_BUSY_RX;
      }
      else
      {
        /* Disable the UART Parity Error Interrupt */
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
        
        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
        
        huart->State = HAL_UART_STATE_READY;
      }
      /* Call the Process Unlocked before calling the Tx callback API to give the possibility to
         start again the Transmission under the Tx callback API */
      __HAL_UNLOCK(huart);
      
      HAL_UART_TxCpltCallback(huart);
      
      return HAL_OK;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

      return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}
/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart: UART handle
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint16_t* tmp;
  uint32_t tmp1 = 0;
  
  tmp1 = huart->State; 
  if((tmp1 == HAL_UART_STATE_BUSY_RX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
  {
    /* Process Locked */
    __HAL_LOCK(huart);
    
    if(huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t*) huart->pRxBuffPtr;
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
        huart->pRxBuffPtr += 2;
      }
      else
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
        huart->pRxBuffPtr += 1;
      }
    }
    else
    {
      if(huart->Init.Parity == UART_PARITY_NONE)
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
    }
    
    if(--huart->RxXferCount == 0)
    {
      while(HAL_IS_BIT_SET(huart->Instance->SR, UART_FLAG_RXNE))
      {
      }
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
      
      /* Check if a transmit process is ongoing or not */
      if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
      {
        huart->State = HAL_UART_STATE_BUSY_TX;
      }
      else
      {
        /* Disable the UART Parity Error Interrupt */
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
        
        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
        
        huart->State = HAL_UART_STATE_READY;
      }
      /* Call the Process Unlocked before calling the Rx callback API to give the possibility to
         start again the reception under the Rx callback API */
      __HAL_UNLOCK(huart);
      
      HAL_UART_RxCpltCallback(huart);
      
      return HAL_OK;
    }
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @brief  Configures the UART peripheral. 
  * @param  huart: UART handle
  * @retval None
  */
static void UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00;
  
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));
  assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));  
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
  assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));
  assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));

  /* The hardware flow control is available only for USART1, USART2, USART3 and USART6 */
  if(huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
  }

  /*-------------------------- USART CR2 Configuration -----------------------*/
  tmpreg = huart->Instance->CR2;

  /* Clear STOP[13:12] bits */
  tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP);

  /* Configure the UART Stop Bits: Set STOP[13:12] bits according to huart->Init.StopBits value */
  tmpreg |= (uint32_t)huart->Init.StopBits;
  
  /* Write to USART CR2 */
  huart->Instance->CR2 = (uint32_t)tmpreg;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;

  /* Clear M, PCE, PS, TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | \
                                   USART_CR1_RE | USART_CR1_OVER8));

  /* Configure the UART Word Length, Parity and mode: 
     Set the M bits according to huart->Init.WordLength value 
     Set PCE and PS bits according to huart->Init.Parity value
     Set TE and RE bits according to huart->Init.Mode value
     Set OVER8 bit according to huart->Init.OverSampling value */
  tmpreg |= (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling;
  
  /* Write to USART CR1 */
  huart->Instance->CR1 = (uint32_t)tmpreg;
  
  /*-------------------------- USART CR3 Configuration -----------------------*/  
  tmpreg = huart->Instance->CR3;
  
  /* Clear CTSE and RTSE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE));
  
  /* Configure the UART HFC: Set CTSE and RTSE bits according to huart->Init.HwFlowCtl value */
  tmpreg |= huart->Init.HwFlowCtl;
  
  /* Write to USART CR3 */
  huart->Instance->CR3 = (uint32_t)tmpreg;
  
  /* Check the Over Sampling */
  if(huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
    /*-------------------------- USART BRR Configuration ---------------------*/
    if((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = __UART_BRR_SAMPLING8(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
    else
    {
      huart->Instance->BRR = __UART_BRR_SAMPLING8(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
  else
  {
    /*-------------------------- USART BRR Configuration ---------------------*/
    if((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = __UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
    else
    {
      huart->Instance->BRR = __UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
}

/**
  * @}
  */

#endif /* HAL_UART_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
