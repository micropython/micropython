/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2c.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   I2C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Inter Integrated Circuit (I2C) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The I2C HAL driver can be used as follows:

    (#) Declare a I2C_HandleTypeDef handle structure, for example:
        I2C_HandleTypeDef  hi2c;

    (#)Initialize the I2C low level resources by implement the HAL_I2C_MspInit() API:
        (##) Enable the I2Cx interface clock
        (##) I2C pins configuration
            (+++) Enable the clock for the I2C GPIOs
            (+++) Configure I2C pins as alternate function open-drain
        (##) NVIC configuration if you need to use interrupt process
            (+++) Configure the I2Cx interrupt priority
            (+++) Enable the NVIC I2C IRQ Channel
        (##) DMA Configuration if you need to use DMA process
            (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive stream
            (+++) Enable the DMAx interface clock using
            (+++) Configure the DMA handle parameters
            (+++) Configure the DMA Tx or Rx Stream
            (+++) Associate the initilalized DMA handle to the hi2c DMA Tx or Rx handle
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on 
                  the DMA Tx or Rx Stream

    (#) Configure the Communication Speed, Duty cycle, Addressing mode, Own Address1,
        Dual Addressing mode, Own Address2, General call and Nostretch mode in the hi2c Init structure.

    (#) Initialize the I2C registers by calling the HAL_I2C_Init(), configures also the low level Hardware 
        (GPIO, CLOCK, NVIC...etc) by calling the customed HAL_I2C_MspInit(&hi2c) API.

    (#) To check if target device is ready for communication, use the function HAL_I2C_IsDeviceReady()

    (#) For I2C IO and IO MEM operations, three operation modes are available within this driver :

    *** Polling mode IO operation ***
    =================================
    [..]
      (+) Transmit in master mode an amount of data in blocking mode using HAL_I2C_Master_Transmit()
      (+) Receive in master mode an amount of data in blocking mode using HAL_I2C_Master_Receive()
      (+) Transmit in slave mode an amount of data in blocking mode using HAL_I2C_Slave_Transmit()
      (+) Receive in slave mode an amount of data in blocking mode using HAL_I2C_Slave_Receive()

    *** Polling mode IO MEM operation ***
    =====================================
    [..]
      (+) Write an amount of data in blocking mode to a specific memory address using HAL_I2C_Mem_Write()
      (+) Read an amount of data in blocking mode from a specific memory address using HAL_I2C_Mem_Read()


    *** Interrupt mode IO operation ***
    ===================================
    [..]
      (+) Transmit in master mode an amount of data in non blocking mode using HAL_I2C_Master_Transmit_IT()
      (+) At transmission end of transfer HAL_I2C_MasterTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterTxCpltCallback
      (+) Receive in master mode an amount of data in non blocking mode using HAL_I2C_Master_Receive_IT()
      (+) At reception end of transfer HAL_I2C_MasterRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback
      (+) Transmit in slave mode an amount of data in non blocking mode using HAL_I2C_Slave_Transmit_IT()
      (+) At transmission end of transfer HAL_I2C_SlaveTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveTxCpltCallback
      (+) Receive in slave mode an amount of data in non blocking mode using HAL_I2C_Slave_Receive_IT()
      (+) At reception end of transfer HAL_I2C_SlaveRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveRxCpltCallback
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback

    *** Interrupt mode IO MEM operation ***
    =======================================
    [..]
      (+) Write an amount of data in no-blocking mode with Interrupt to a specific memory address using
          HAL_I2C_Mem_Write_IT()
      (+) At MEM end of write transfer HAL_I2C_MemTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemTxCpltCallback
      (+) Read an amount of data in no-blocking mode with Interrupt from a specific memory address using
          HAL_I2C_Mem_Read_IT()
      (+) At MEM end of read transfer HAL_I2C_MemRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemRxCpltCallback
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback

    *** DMA mode IO operation ***
    ==============================
    [..]
      (+) Transmit in master mode an amount of data in non blocking mode (DMA) using
          HAL_I2C_Master_Transmit_DMA()
      (+) At transmission end of transfer HAL_I2C_MasterTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterTxCpltCallback
      (+) Receive in master mode an amount of data in non blocking mode (DMA) using
          HAL_I2C_Master_Receive_DMA()
      (+) At reception end of transfer HAL_I2C_MasterRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MasterRxCpltCallback
      (+) Transmit in slave mode an amount of data in non blocking mode (DMA) using
          HAL_I2C_Slave_Transmit_DMA()
      (+) At transmission end of transfer HAL_I2C_SlaveTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveTxCpltCallback
      (+) Receive in slave mode an amount of data in non blocking mode (DMA) using
          HAL_I2C_Slave_Receive_DMA()
      (+) At reception end of transfer HAL_I2C_SlaveRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_SlaveRxCpltCallback
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback

    *** DMA mode IO MEM operation ***
    =================================
    [..]
      (+) Write an amount of data in no-blocking mode with DMA to a specific memory address using
          HAL_I2C_Mem_Write_DMA()
      (+) At MEM end of write transfer HAL_I2C_MemTxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemTxCpltCallback
      (+) Read an amount of data in no-blocking mode with DMA from a specific memory address using
          HAL_I2C_Mem_Read_DMA()
      (+) At MEM end of read transfer HAL_I2C_MemRxCpltCallback is executed and user can
           add his own code by customization of function pointer HAL_I2C_MemRxCpltCallback
      (+) In case of transfer Error, HAL_I2C_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_I2C_ErrorCallback


     *** I2C HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in I2C HAL driver.

      (+) __HAL_I2C_ENABLE: Enable the I2C peripheral
      (+) __HAL_I2C_DISABLE: Disable the I2C peripheral
      (+) __HAL_I2C_GET_FLAG : Checks whether the specified I2C flag is set or not
      (+) __HAL_I2C_CLEAR_FLAG : Clear the specified I2C pending flag
      (+) __HAL_I2C_ENABLE_IT: Enable the specified I2C interrupt
      (+) __HAL_I2C_DISABLE_IT: Disable the specified I2C interrupt

     [..]
       (@) You can refer to the I2C HAL driver header file for more useful macros


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

/** @defgroup I2C
  * @brief I2C HAL module driver
  * @{
  */

#ifdef HAL_I2C_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_TIMEOUT_FLAG          ((uint32_t)35)     /* 35 ms */
#define I2C_TIMEOUT_ADDR_SLAVE    ((uint32_t)10000)  /* 10 s  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void I2C_DMAMasterTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMemTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMemReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAError(DMA_HandleTypeDef *hdma);

static HAL_StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout);
static HAL_StatusTypeDef I2C_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout);
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout);

static HAL_StatusTypeDef I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_MasterTransmit_BTF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_MasterReceive_BTF(I2C_HandleTypeDef *hi2c);

static HAL_StatusTypeDef I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_Slave_AF(I2C_HandleTypeDef *hi2c);

/* Private functions ---------------------------------------------------------*/

/** @defgroup I2C_Private_Functions
  * @{
  */

/** @defgroup I2C_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          de-initialiaze the I2Cx peripheral:

      (+) User must Implement HAL_I2C_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC).

      (+) Call the function HAL_I2C_Init() to configure the selected device with
          the selected configuration:
        (++) Communication Speed
        (++) Duty cycle
        (++) Addressing mode
        (++) Own Address 1
        (++) Dual Addressing mode
        (++) Own Address 2
        (++) General call mode
        (++) Nostretch mode

      (+) Call the function HAL_I2C_DeInit() to restore the default configuration
          of the selected I2Cx periperal.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and create the associated handle.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
  uint32_t freqrange = 0;
  uint32_t pclk1 = 0;

  /* Check the I2C handle allocation */
  if(hi2c == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_I2C_CLOCK_SPEED(hi2c->Init.ClockSpeed));
  assert_param(IS_I2C_DUTY_CYCLE(hi2c->Init.DutyCycle));
  assert_param(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
  assert_param(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
  assert_param(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
  assert_param(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
  assert_param(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
  assert_param(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

  if(hi2c->State == HAL_I2C_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_I2C_MspInit(hi2c);
  }

  hi2c->State = HAL_I2C_STATE_BUSY;

  /* Disble the selected I2C peripheral */
  __HAL_I2C_DISABLE(hi2c);

  /* Get PCLK1 frequency */
  pclk1 = HAL_RCC_GetPCLK1Freq();

  /* Calculate frequency range */
  freqrange = __HAL_I2C_FREQRANGE(pclk1);

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Frequency range */
  hi2c->Instance->CR2 = freqrange;

  /*---------------------------- I2Cx TRISE Configuration --------------------*/
  /* Configure I2Cx: Rise Time */
  hi2c->Instance->TRISE = __HAL_I2C_RISE_TIME(freqrange, hi2c->Init.ClockSpeed);

  /*---------------------------- I2Cx CCR Configuration ----------------------*/
  /* Configure I2Cx: Speed */
  hi2c->Instance->CCR = __HAL_I2C_SPEED(pclk1, hi2c->Init.ClockSpeed, hi2c->Init.DutyCycle);

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  hi2c->Instance->CR1 = (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode);

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Configure I2Cx: Own Address1 and addressing mode */
  hi2c->Instance->OAR1 = (hi2c->Init.AddressingMode | hi2c->Init.OwnAddress1);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Configure I2Cx: Dual mode and Own Address2 */
  hi2c->Instance->OAR2 = (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2);

  /* Enable the selected I2C peripheral */
  __HAL_I2C_ENABLE(hi2c);

  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
  hi2c->State = HAL_I2C_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the I2C peripheral.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
  /* Check the I2C handle allocation */
  if(hi2c == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));

  hi2c->State = HAL_I2C_STATE_BUSY;

  /* Disable the I2C Peripheral Clock */
  __HAL_I2C_DISABLE(hi2c);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_I2C_MspDeInit(hi2c);

  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

  hi2c->State = HAL_I2C_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief I2C MSP Init.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_MspInit could be implemented in the user file
   */
}

/**
  * @brief I2C MSP DeInit
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2C_Group2 IO operation functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the I2C data
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode.
            The status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode : The communication is performed using Interrupts
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the
            dedicated I2C IRQ when using Interrupt mode or the DMA IRQ when
            using DMA mode.

    (#) Blocking mode functions are :
        (++) HAL_I2C_Master_Transmit()
        (++) HAL_I2C_Master_Receive()
        (++) HAL_I2C_Slave_Transmit()
        (++) HAL_I2C_Slave_Receive()
        (++) HAL_I2C_Mem_Write()
        (++) HAL_I2C_Mem_Read()
        (++) HAL_I2C_IsDeviceReady()

    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2C_Master_Transmit_IT()
        (++) HAL_I2C_Master_Receive_IT()
        (++) HAL_I2C_Slave_Transmit_IT()
        (++) HAL_I2C_Slave_Receive_IT()
        (++) HAL_I2C_Mem_Write_IT()
        (++) HAL_I2C_Mem_Read_IT()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_I2C_Master_Transmit_DMA()
        (++) HAL_I2C_Master_Receive_DMA()
        (++) HAL_I2C_Slave_Transmit_DMA()
        (++) HAL_I2C_Slave_Receive_DMA()
        (++) HAL_I2C_Mem_Write_DMA()
        (++) HAL_I2C_Mem_Read_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2C_MemTxCpltCallback()
        (++) HAL_I2C_MemRxCpltCallback()
        (++) HAL_I2C_MasterTxCpltCallback()
        (++) HAL_I2C_MasterRxCpltCallback()
        (++) HAL_I2C_SlaveTxCpltCallback()
        (++) HAL_I2C_SlaveRxCpltCallback()
        (++) HAL_I2C_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(hi2c, DevAddress, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    while(Size > 0)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Write data to DR */
      hi2c->Instance->DR = (*pData++);
      Size--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (Size != 0))
      {
        /* Write data to DR */
        hi2c->Instance->DR = (*pData++);
        Size--;
      }
    }

    /* Wait until TXE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives in master mode an amount of data in blocking mode.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    if(I2C_MasterRequestRead(hi2c, DevAddress, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(Size == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }
    else if(Size == 2)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Enable Pos */
      hi2c->Instance->CR1 |= I2C_CR1_POS;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Enable Acknowledge */
      hi2c->Instance->CR1 |= I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while(Size > 0)
    {
      if(Size <= 3)
      {
        /* One byte */
        if(Size == 1)
        {
          /* Wait until RXNE flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
        /* Two bytes */
        else if(Size == 2)
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable Acknowledge */
          hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Read data from DR */
        (*pData++) = hi2c->Instance->DR;
        Size--;

        if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
      }
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmits in slave mode an amount of data in blocking mode.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* If 10bit addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
      /* Wait until ADDR flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while(Size > 0)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Write data to DR */
      hi2c->Instance->DR = (*pData++);
      Size--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (Size != 0))
      {
        /* Write data to DR */
        hi2c->Instance->DR = (*pData++);
        Size--;
      }
    }

    /* Wait until AF flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_AF, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Clear AF flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Disable Address Acknowledge */
    hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in blocking mode
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    while(Size > 0)
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Read data from DR */
      (*pData++) = hi2c->Instance->DR;
      Size--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (Size != 0))
      {
        /* Read data from DR */
        (*pData++) = hi2c->Instance->DR;
        Size--;
      }
    }

    /* Wait until STOP flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Clear STOP flag */
    __HAL_I2C_CLEAR_STOPFLAG(hi2c);

    /* Disable Address Acknowledge */
    hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in master mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(hi2c, DevAddress, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in master mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Send Slave Address */
    if(I2C_MasterRequestRead(hi2c, DevAddress, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(hi2c->XferCount == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }
    else if(hi2c->XferCount == 2)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Enable Pos */
      hi2c->Instance->CR1 |= I2C_CR1_POS;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Enable Acknowledge */
      hi2c->Instance->CR1 |= I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in slave mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in master mode an amount of data in no-blocking mode with DMA
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMasterTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->DR, Size);

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(hi2c, DevAddress, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in master mode an amount of data in no-blocking mode with DMA
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMAMasterReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->DR, (uint32_t)pData, Size);

    /* Send Slave Address */
    if(I2C_MasterRequestRead(hi2c, DevAddress, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(Size == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
    }
    else
    {
      /* Enable Last DMA bit */
      hi2c->Instance->CR2 |= I2C_CR2_LAST;
    }

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in slave mode an amount of data in no-blocking mode with DMA
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMASlaveTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->DR, Size);

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR_SLAVE) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* If 7bit addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
    {
      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Wait until ADDR flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR_SLAVE) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in no-blocking mode with DMA
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMASlaveReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->DR, (uint32_t)pData, Size);

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR1 |= I2C_CR1_ACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR_SLAVE) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in blocking mode to a specific memory address
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    while(Size > 0)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Write data to DR */
      hi2c->Instance->DR = (*pData++);
      Size--;

      if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (Size != 0))
      {
        /* Write data to DR */
        hi2c->Instance->DR = (*pData++);
        Size--;
      }
    }

    /* Wait until TXE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(Size == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }
    else if(Size == 2)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Enable Pos */
      hi2c->Instance->CR1 |= I2C_CR1_POS;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while(Size > 0)
    {
      if(Size <= 3)
      {
        /* One byte */
        if(Size== 1)
        {
          /* Wait until RXNE flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
        /* Two bytes */
        else if(Size == 2)
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable Acknowledge */
          hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          hi2c->Instance->CR1 |= I2C_CR1_STOP;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;

          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Read data from DR */
        (*pData++) = hi2c->Instance->DR;
        Size--;

        if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          (*pData++) = hi2c->Instance->DR;
          Size--;
        }
      }
    }

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in no-blocking mode with Interrupt to a specific memory address
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Read an amount of data in no-blocking mode with Interrupt from a specific memory address
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(hi2c->XferCount == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
    }
    else if(hi2c->XferCount == 2)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

      /* Enable Pos */
      hi2c->Instance->CR1 |= I2C_CR1_POS;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Enable Acknowledge */
      hi2c->Instance->CR1 |= I2C_CR1_ACK;

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in no-blocking mode with DMA to a specific memory address
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_TX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMemTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->DR, Size);

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Reads an amount of data in no-blocking mode with DMA from a specific memory address.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MEM_BUSY_RX;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfert complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMAMemReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->DR, (uint32_t)pData, Size);

    /* Send Slave Address and Memory Address */
    if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_TIMEOUT;
      }
    }

    if(Size == 1)
    {
      /* Disable Acknowledge */
      hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
    }
    else
    {
      /* Enable Last DMA bit */
      hi2c->Instance->CR2 |= I2C_CR2_LAST;
    }

    /* Enable DMA Request */
    hi2c->Instance->CR2 |= I2C_CR2_DMAEN;

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
  uint32_t tickstart = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, I2C_Trials = 1;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    do
    {
      /* Generate Start */
      hi2c->Instance->CR1 |= I2C_CR1_START;

      /* Wait until SB flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Send slave address */
      hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(DevAddress);

      /* Wait until ADDR or AF flag are set */
      /* Get tick */
      tickstart = HAL_GetTick();

      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
      tmp2 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
      tmp3 = hi2c->State;
      while((tmp1 == RESET) && (tmp2 == RESET) && (tmp3 != HAL_I2C_STATE_TIMEOUT))
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          hi2c->State = HAL_I2C_STATE_TIMEOUT;
        }
        tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
        tmp2 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
        tmp3 = hi2c->State;
      }

      hi2c->State = HAL_I2C_STATE_READY;

      /* Check if the ADDR flag has been set */
      if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)
      {
        /* Generate Stop */
        hi2c->Instance->CR1 |= I2C_CR1_STOP;

        /* Clear ADDR Flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

        /* Wait until BUSY flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        hi2c->State = HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
      }
      else
      {
        /* Generate Stop */
        hi2c->Instance->CR1 |= I2C_CR1_STOP;

        /* Clear AF Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

        /* Wait until BUSY flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
      }
    }while(I2C_Trials++ < Trials);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
  /* Master mode selected */
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_MSL) == SET)
  {
    /* I2C in mode Transmitter -----------------------------------------------*/
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* TXE set and BTF reset -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        I2C_MasterTransmit_TXE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        I2C_MasterTransmit_BTF(hi2c);
      }
    }
    /* I2C in mode Receiver --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* RXNE set and BTF reset -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        I2C_MasterReceive_RXNE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        I2C_MasterReceive_BTF(hi2c);
      }
    }
  }
  /* Slave mode selected */
  else
  {
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
    tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_EVT));
    tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF);
    tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
    /* ADDR set --------------------------------------------------------------*/
    if((tmp1 == SET) && (tmp2 == SET))
    {
      I2C_Slave_ADDR(hi2c);
    }
    /* STOPF set --------------------------------------------------------------*/
    else if((tmp3 == SET) && (tmp2 == SET))
    {
      I2C_Slave_STOPF(hi2c);
    }
    /* I2C in mode Transmitter -----------------------------------------------*/
    else if(tmp4 == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* TXE set and BTF reset -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        I2C_SlaveTransmit_TXE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        I2C_SlaveTransmit_BTF(hi2c);
      }
    }
    /* I2C in mode Receiver --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* RXNE set and BTF reset ----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        I2C_SlaveReceive_RXNE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        I2C_SlaveReceive_BTF(hi2c);
      }
    }
  }
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Bus error interrupt occurred ----------------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* Clear BERR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Arbitration Loss error interrupt occurred ---------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Acknowledge failure error interrupt occurred ------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_MSL);
    tmp2 = hi2c->XferCount;
    tmp3 = hi2c->State;
    if((tmp1 == RESET) && (tmp2 == 0) && (tmp3 == HAL_I2C_STATE_BUSY_TX))
    {
      I2C_Slave_AF(hi2c);
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
      /* Clear AF flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    }
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_OVR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;
    /* Clear OVR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->State = HAL_I2C_STATE_READY;
    
    HAL_I2C_ErrorCallback(hi2c);
  }
}

/**
  * @brief  Master Tx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Master Rx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
__weak void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/** @brief  Slave Tx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Slave Rx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
__weak void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Memory Tx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Memory Rx Transfer completed callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
__weak void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  I2C error callbacks.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
 __weak void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2C_Group3 Peripheral State and Errors functions
 *  @brief   Peripheral State and Errors functions
 *
@verbatim
 ===============================================================================
            ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the I2C state.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL state
  */
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
  return hi2c->State;
}

/**
  * @brief  Return the I2C error code
  * @param  hi2c : pointer to a I2C_HandleTypeDef structure that contains
  *              the configuration information for the specified I2C.
* @retval I2C Error Code
*/
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c)
{
  return hi2c->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  Handle TXE flag for Master
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
  /* Write data to DR */
  hi2c->Instance->DR = (*hi2c->pBuffPtr++);
  hi2c->XferCount--;

  if(hi2c->XferCount == 0)
  {
    /* Disable BUF interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_BUF);
  }

  return HAL_OK;
}

/**
  * @brief  Handle BTF flag for Master transmitter
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterTransmit_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  else
  {
    /* Disable EVT, BUF and ERR interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    if(hi2c->State == HAL_I2C_STATE_MEM_BUSY_TX)
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MemTxCpltCallback(hi2c);
    }
    else
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MasterTxCpltCallback(hi2c);
    }
  }
  return HAL_OK;
}

/**
  * @brief  Handle RXNE flag for Master
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
  uint32_t tmp = 0;

  tmp = hi2c->XferCount;
  if(tmp > 3)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  else if((tmp == 2) || (tmp == 3))
  {
    /* Disable BUF interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_BUF);
  }
  else
  {
    /* Disable EVT, BUF and ERR interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    if(hi2c->State == HAL_I2C_STATE_MEM_BUSY_RX)
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MemRxCpltCallback(hi2c);
    }
    else
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MasterRxCpltCallback(hi2c);
    }
  }
  return HAL_OK;
}

/**
  * @brief  Handle BTF flag for Master receiver
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterReceive_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount == 3)
  {
    /* Disable Acknowledge */
    hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  else if(hi2c->XferCount == 2)
  {
    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;

    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;

    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;

    /* Disable EVT and ERR interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    if(hi2c->State == HAL_I2C_STATE_MEM_BUSY_RX)
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MemRxCpltCallback(hi2c);
    }
    else
    {
      hi2c->State = HAL_I2C_STATE_READY;

      HAL_I2C_MasterRxCpltCallback(hi2c);
    }
  }
  else
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return HAL_OK;
}

/**
  * @brief  Handle TXE flag for Slave
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return HAL_OK;
}

/**
  * @brief  Handle BTF flag for Slave transmitter
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return HAL_OK;
}

/**
  * @brief  Handle RXNE flag for Slave
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return HAL_OK;
}

/**
  * @brief  Handle BTF flag for Slave receiver
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return HAL_OK;
}

/**
  * @brief  Handle ADD flag for Slave
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)
{
  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  return HAL_OK;
}

/**
  * @brief  Handle STOPF flag for Slave
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)
{
  /* Disable EVT, BUF and ERR interrupt */
  __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

  /* Clear STOPF flag */
  __HAL_I2C_CLEAR_STOPFLAG(hi2c);

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  HAL_I2C_SlaveRxCpltCallback(hi2c);

  return HAL_OK;
}

/**
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Slave_AF(I2C_HandleTypeDef *hi2c)
{
  /* Disable EVT, BUF and ERR interrupt */
  __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

  /* Clear AF flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  HAL_I2C_SlaveTxCpltCallback(hi2c);

  return HAL_OK;
}

/**
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout)
{
  /* Generate Start */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = __HAL_I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send slave address */
    hi2c->Instance->DR = __HAL_I2C_10BIT_ADDRESS(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Master sends target device address for read request.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout)
{
  /* Enable Acknowledge */
  hi2c->Instance->CR1 |= I2C_CR1_ACK;

  /* Generate Start */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_READ(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = __HAL_I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send slave address */
    hi2c->Instance->DR = __HAL_I2C_10BIT_ADDRESS(DevAddress);

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Generate Restart */
    hi2c->Instance->CR1 |= I2C_CR1_START;

    /* Wait until SB flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Send header of slave address */
    hi2c->Instance->DR = __HAL_I2C_10BIT_HEADER_READ(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for write request.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
  /* Generate Start */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_LSB(MemAddress);
  }

  return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for read request.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
  /* Enable Acknowledge */
  hi2c->Instance->CR1 |= I2C_CR1_ACK;

  /* Generate Start */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->DR = __HAL_I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TXE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Generate Restart */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = __HAL_I2C_7BIT_ADD_READ(DevAddress);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  DMA I2C master transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMasterTransmitCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Wait until BTF flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Generate Stop */
  hi2c->Instance->CR1 |= I2C_CR1_STOP;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_MasterTxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C slave transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Wait until AF flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_AF, RESET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Clear AF flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

  /* Disable Address Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_SlaveTxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C master receive process complete callback
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Generate Stop */
  hi2c->Instance->CR1 |= I2C_CR1_STOP;

  /* Disable Last DMA */
  hi2c->Instance->CR2 &= ~I2C_CR2_LAST;

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_MasterRxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C slave receive process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMASlaveReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Wait until STOPF flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Clear STOPF flag */
  __HAL_I2C_CLEAR_STOPFLAG(hi2c);

  /* Disable Address Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_SlaveRxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C Memory Write process complete callback
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMemTransmitCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Wait until BTF flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Generate Stop */
  hi2c->Instance->CR1 |= I2C_CR1_STOP;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_MemTxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C Memory Read process complete callback
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMemReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Generate Stop */
  hi2c->Instance->CR1 |= I2C_CR1_STOP;

  /* Disable Last DMA */
  hi2c->Instance->CR2 &= ~I2C_CR2_LAST;

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  /* Disable DMA Request */
  hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

  hi2c->XferCount = 0;

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_FLAG) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  hi2c->State = HAL_I2C_STATE_READY;

  /* Check if Errors has been detected during transfer */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    HAL_I2C_ErrorCallback(hi2c);
  }
  else
  {
    HAL_I2C_MemRxCpltCallback(hi2c);
  }
}

/**
  * @brief  DMA I2C communication error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAError(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;

  hi2c->XferCount = 0;

  hi2c->State = HAL_I2C_STATE_READY;

  hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;

  HAL_I2C_ErrorCallback(hi2c);
}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag: specifies the I2C flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          hi2c->State= HAL_I2C_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_I2C_GET_FLAG(hi2c, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          hi2c->State= HAL_I2C_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for Master addressing phase.
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag: specifies the I2C flag to check.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Get tick */
  tickstart = HAL_GetTick();

  while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      /* Clear AF Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->ErrorCode = HAL_I2C_ERROR_AF;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @}
  */

#endif /* HAL_I2C_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
