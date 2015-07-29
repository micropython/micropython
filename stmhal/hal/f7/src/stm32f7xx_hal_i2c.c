/**
  ******************************************************************************
  * @file    stm32f7xx_hal_i2c.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   I2C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Inter Integrated Circuit (I2C) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
    The I2C HAL driver can be used as follows:

    (#) Declare a I2C_HandleTypeDef handle structure, for example:
        I2C_HandleTypeDef  hi2c;

    (#)Initialize the I2C low level resources by implement the HAL_I2C_MspInit ()API:
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
            (+++) Associate the initialized DMA handle to the hi2c DMA Tx or Rx handle
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx or Rx Stream

    (#) Configure the Communication Clock Timing, Own Address1, Master Addressing Mode, Dual Addressing mode,
        Own Address2, Own Address2 Mask, General call and Nostretch mode in the hi2c Init structure.

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

/** @defgroup I2C I2C
  * @brief I2C HAL module driver
  * @{
  */

#ifdef HAL_I2C_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup I2C_Private_Constants I2C Private Constants
  * @{
  */
#define TIMING_CLEAR_MASK   ((uint32_t)0xF0FFFFFF)  /*<! I2C TIMING clear register Mask */
#define I2C_TIMEOUT_ADDR    ((uint32_t)10000)  /* 10 s  */
#define I2C_TIMEOUT_BUSY    ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_DIR     ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_RXNE    ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_STOPF   ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_TC      ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_TCR     ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_TXIS    ((uint32_t)25)     /* 25 ms */
#define I2C_TIMEOUT_FLAG    ((uint32_t)25)     /* 25 ms */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup I2C_Private_Functions I2C Private Functions
  * @{
  */
static void I2C_DMAMasterTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMASlaveReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMemTransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAMemReceiveCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAError(DMA_HandleTypeDef *hdma);

static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout);
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c, uint32_t Timeout);

static HAL_StatusTypeDef I2C_MasterTransmit_ISR(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_MasterReceive_ISR(I2C_HandleTypeDef *hi2c);

static HAL_StatusTypeDef I2C_SlaveTransmit_ISR(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef I2C_SlaveReceive_ISR(I2C_HandleTypeDef *hi2c);

static void I2C_TransferConfig(I2C_HandleTypeDef *hi2c,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup I2C_Exported_Functions I2C Exported Functions
  * @{
  */

/** @defgroup I2C_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          de-initialize the I2Cx peripheral:

      (+) User must Implement HAL_I2C_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_I2C_Init() to configure the selected device with
          the selected configuration:
        (++) Clock Timing
        (++) Own Address 1
        (++) Addressing mode (Master, Slave)
        (++) Dual Addressing mode
        (++) Own Address 2
        (++) Own Address 2 Mask
        (++) General call mode
        (++) Nostretch mode

      (+) Call the function HAL_I2C_DeInit() to restore the default configuration
          of the selected I2Cx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and create the associated handle.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
  /* Check the I2C handle allocation */
  if(hi2c == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
  assert_param(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
  assert_param(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
  assert_param(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
  assert_param(IS_I2C_OWN_ADDRESS2_MASK(hi2c->Init.OwnAddress2Masks));
  assert_param(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
  assert_param(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

  if(hi2c->State == HAL_I2C_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hi2c->Lock = HAL_UNLOCKED;
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_I2C_MspInit(hi2c);
  }

  hi2c->State = HAL_I2C_STATE_BUSY;

  /* Disable the selected I2C peripheral */
  __HAL_I2C_DISABLE(hi2c);

  /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
  /* Configure I2Cx: Frequency range */
  hi2c->Instance->TIMINGR = hi2c->Init.Timing & TIMING_CLEAR_MASK;

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  hi2c->Instance->OAR1 &= ~I2C_OAR1_OA1EN;
  if(hi2c->Init.OwnAddress1 != 0)
  {
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
    {
      hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | hi2c->Init.OwnAddress1);
    }
    else /* I2C_ADDRESSINGMODE_10BIT */
    {
      hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | hi2c->Init.OwnAddress1);
    }
  }

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Addressing Master mode */
  if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
  {
    hi2c->Instance->CR2 = (I2C_CR2_ADD10);
  }
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  hi2c->Instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Configure I2Cx: Dual mode and Own Address2 */
  hi2c->Instance->OAR2 = (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2 | (hi2c->Init.OwnAddress2Masks << 8));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  hi2c->Instance->CR1 = (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode);

  /* Enable the selected I2C peripheral */
  __HAL_I2C_ENABLE(hi2c);

  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
  hi2c->State = HAL_I2C_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the I2C peripheral.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

/** @defgroup I2C_Exported_Functions_Group2 Input and Output operation functions
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t sizetmp = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_TX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    /* Size > 255, need to set RELOAD bit */
    if(Size > 255)
    {
      I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
      sizetmp = 255;
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_GENERATE_START_WRITE);
      sizetmp = Size;
    }

    do
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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
      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*pData++);
      sizetmp--;
      Size--;

      if((sizetmp == 0)&&(Size!=0))
      {
        /* Wait until TXE flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(Size > 255)
        {
          I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
          sizetmp = 255;
        }
        else
        {
          I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
          sizetmp = Size;
        }
      }

    }while(Size > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t sizetmp = 0;

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) == SET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    /* Size > 255, need to set RELOAD bit */
    if(Size > 255)
    {
      I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
      sizetmp = 255;
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
      sizetmp = Size;
    }

    do
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Write data to RXDR */
      (*pData++) =hi2c->Instance->RXDR;
      sizetmp--;
      Size--;

      if((sizetmp == 0)&&(Size!=0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(Size > 255)
        {
          I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
          sizetmp = 255;
        }
        else
        {
          I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
          sizetmp = Size;
        }
      }

    }while(Size > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
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

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* If 10bit addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
      /* Wait until ADDR flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;
        return HAL_TIMEOUT;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);
    }

    /* Wait until DIR flag is set Transmitter mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, RESET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    do
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;

        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Read data from TXDR */
      hi2c->Instance->TXDR = (*pData++);
      Size--;
    }while(Size > 0);

    /* Wait until STOP flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;

      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
	/* Normal use case for Transmitter mode */
	/* A NACK is generated to confirm the end of transfer */
	hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_STOPF);

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* Wait until DIR flag is reset Receiver mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, SET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    while(Size > 0)
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;
        if(hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
        {
          return HAL_TIMEOUT;
        }
        else
        {
          return HAL_ERROR;
        }
      }

      /* Read data from RXDR */
      (*pData++) = hi2c->Instance->RXDR;
      Size--;
    }

    /* Wait until STOP flag is set */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;

      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Clear STOP flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_STOPF);

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }


    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_TX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_WRITE);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */


    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_TXI );

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in master mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_RXI );

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in slave mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_TX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_TXI );

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive in slave mode an amount of data in no-blocking mode with Interrupt
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmit in master mode an amount of data in no-blocking mode with DMA
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_TX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMasterTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_WRITE);
    }

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, I2C_TIMEOUT_TXIS) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;

      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }


    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->State = HAL_I2C_STATE_MASTER_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMAMasterReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, hi2c->XferSize);

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    /* Wait until RXNE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, I2C_TIMEOUT_RXNE) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }


    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_TX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    hi2c->XferSize = Size;

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMASlaveTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* If 10bits addressing mode is selected */
    if(hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
    {
      /* Wait until ADDR flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR) != HAL_OK)
      {
        /* Disable Address Acknowledge */
        hi2c->Instance->CR2 |= I2C_CR2_NACK;
        return HAL_TIMEOUT;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);
    }

    /* Wait until DIR flag is set Transmitter mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, RESET, I2C_TIMEOUT_BUSY) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
    /* Process Locked */
    __HAL_LOCK(hi2c);

    hi2c->State = HAL_I2C_STATE_SLAVE_BUSY_RX;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMASlaveReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, Size);

    /* Enable Address Acknowledge */
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Wait until ADDR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, I2C_TIMEOUT_ADDR) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_ADDR);

    /* Wait until DIR flag is set Receiver mode */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_DIR, SET, I2C_TIMEOUT_DIR) != HAL_OK)
    {
      /* Disable Address Acknowledge */
      hi2c->Instance->CR2 |= I2C_CR2_NACK;
      return HAL_TIMEOUT;
    }

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  uint32_t Sizetmp = 0;

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

    /* Set NBYTES to write and reload if size > 255 */
    /* Size > 255, need to set RELOAD bit */
    if(Size > 255)
    {
      I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      Sizetmp = 255;
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
      Sizetmp = Size;
    }

    do
    {
      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

      /* Write data to DR */
      hi2c->Instance->TXDR = (*pData++);
      Sizetmp--;
      Size--;

      if((Sizetmp == 0)&&(Size!=0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }


        if(Size > 255)
        {
          I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
          Sizetmp = 255;
        }
        else
        {
          I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
          Sizetmp = Size;
        }
      }

    }while(Size > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
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

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  uint32_t Sizetmp = 0;

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

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    /* Size > 255, need to set RELOAD bit */
    if(Size > 255)
    {
      I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
      Sizetmp = 255;
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
      Sizetmp = Size;
    }

    do
    {
      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Read data from RXDR */
      (*pData++) = hi2c->Instance->RXDR;

      /* Decrement the Size counter */
      Sizetmp--;
      Size--;

      if((Sizetmp == 0)&&(Size!=0))
      {
        /* Wait until TCR flag is set */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        if(Size > 255)
        {
          I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
          Sizetmp = 255;
        }
        else
        {
          I2C_TransferConfig(hi2c,DevAddress,Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
          Sizetmp = Size;
        }
      }

    }while(Size > 0);

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
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

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

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

    /* Set NBYTES to write and reload if size > 255 */
    /* Size > 255, need to set RELOAD bit */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_TXI );

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Read an amount of data in no-blocking mode with Interrupt from a specific memory address
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

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

    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    /* Size > 255, need to set RELOAD bit */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    /* possible to enable all of these */
    /* I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_RXI );

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  Write an amount of data in no-blocking mode with DMA to a specific memory address
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmatx->XferCpltCallback = I2C_DMAMemTransmitCplt;

    /* Set the DMA error callback */
    hi2c->hdmatx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)pData, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

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

    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > 255 */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
    }

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, I2C_TIMEOUT_TXIS) != HAL_OK)
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

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    if(Size > 255)
    {
      hi2c->XferSize = 255;
    }
    else
    {
      hi2c->XferSize = Size;
    }

    /* Set the I2C DMA transfer complete callback */
    hi2c->hdmarx->XferCpltCallback = I2C_DMAMemReceiveCplt;

    /* Set the DMA error callback */
    hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)pData, hi2c->XferSize);

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

    /* Set NBYTES to write and reload if size > 255 and generate RESTART */
    if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
    }
    else
    {
      I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
    }

    /* Wait until RXNE flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, I2C_TIMEOUT_RXNE) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Enable DMA Request */
    hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;

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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  __IO uint32_t I2C_Trials = 0;

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
      hi2c->Instance->CR2 = I2C_GENERATE_START(hi2c->Init.AddressingMode,DevAddress);

      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is set or a NACK flag is set*/
      tickstart = HAL_GetTick();
      while((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET) && (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == RESET) && (hi2c->State != HAL_I2C_STATE_TIMEOUT))
      {
      	if(Timeout != HAL_MAX_DELAY)
      	{
          if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
          {
            /* Device is ready */
            hi2c->State = HAL_I2C_STATE_READY;
            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);
            return HAL_TIMEOUT;
          }
        }
      }

      /* Check if the NACKF flag has not been set */
      if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == RESET)
      {
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Device is ready */
        hi2c->State = HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
      }
      else
      {
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear NACK Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

        /* Clear STOP Flag, auto generated with autoend*/
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
      }

      /* Check if the maximum allowed number of trials has been reached */
      if (I2C_Trials++ == Trials)
      {
        /* Generate Stop */
        hi2c->Instance->CR2 |= I2C_CR2_STOP;

        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_STOPF, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
      }
    }while(I2C_Trials < Trials);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_TIMEOUT;
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @}
  */

/** @defgroup IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
 * @{
 */

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  /* I2C in mode Transmitter ---------------------------------------------------*/
  if (((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)) && (__HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_TXI | I2C_IT_ADDRI)) == SET))
  {
    /* Slave mode selected */
    if (hi2c->State == HAL_I2C_STATE_SLAVE_BUSY_TX)
    {
      I2C_SlaveTransmit_ISR(hi2c);
    }
  }

  if (((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)) && (__HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_TXI)) == SET))
  {
    /* Master mode selected */
    if ((hi2c->State == HAL_I2C_STATE_MASTER_BUSY_TX) || (hi2c->State == HAL_I2C_STATE_MEM_BUSY_TX))
    {
      I2C_MasterTransmit_ISR(hi2c);
    }
  }

  /* I2C in mode Receiver ----------------------------------------------------*/
  if (((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)) && (__HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_RXI | I2C_IT_ADDRI)) == SET))
  {
    /* Slave mode selected */
    if (hi2c->State == HAL_I2C_STATE_SLAVE_BUSY_RX)
    {
      I2C_SlaveReceive_ISR(hi2c);
    }
  }
  if (((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) || (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)) && (__HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_RXI)) == SET))
  {
    /* Master mode selected */
    if ((hi2c->State == HAL_I2C_STATE_MASTER_BUSY_RX) || (hi2c->State == HAL_I2C_STATE_MEM_BUSY_RX))
    {
      I2C_MasterReceive_ISR(hi2c);
    }
  }
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  /* I2C Bus error interrupt occurred ------------------------------------*/
  if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR) == SET) && (__HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERRI) == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* Clear BERR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
  }

  /* I2C Over-Run/Under-Run interrupt occurred ----------------------------------------*/
  if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_OVR) == SET) && (__HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERRI) == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;

    /* Clear OVR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  /* I2C Arbitration Loss error interrupt occurred -------------------------------------*/
  if((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO) == SET) && (__HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERRI) == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  /* Call the Error Callback in case of Error detected */
  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->State = HAL_I2C_STATE_READY;

    HAL_I2C_ErrorCallback(hi2c);
  }
}

/**
  * @brief  Master Tx Transfer completed callbacks.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2C_TxCpltCallback could be implemented in the user file
   */
}

/** @brief  Slave Tx Transfer completed callbacks.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
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

/** @defgroup I2C_Exported_Functions_Group3 Peripheral State and Errors functions
 *  @brief   Peripheral State and Errors functions
 *
@verbatim
 ===============================================================================
            ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the I2C state.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL state
  */
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
  return hi2c->State;
}

/**
  * @brief  Return the I2C error code
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
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
  * @}
  */

/** @addtogroup I2C_Private_Functions
  * @{
  */

/**
  * @brief  Handle Interrupt Flags Master Transmit Mode
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterTransmit_ISR(I2C_HandleTypeDef *hi2c)
{
  uint16_t DevAddress;

  /* Process Locked */
  __HAL_LOCK(hi2c);

  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == SET)
  {
    /* Write data to TXDR */
    hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
    hi2c->XferSize--;
    hi2c->XferCount--;
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET)
  {
    if((hi2c->XferSize == 0)&&(hi2c->XferCount!=0))
    {
      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      if(hi2c->XferCount > 255)
      {
        I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        hi2c->XferSize = 255;
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferCount, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        hi2c->XferSize = hi2c->XferCount;
      }
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Wrong size Status regarding TCR flag event */
      hi2c->ErrorCode |= HAL_I2C_ERROR_SIZE;
      HAL_I2C_ErrorCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET)
  {
    if(hi2c->XferCount == 0)
    {
      /* Generate Stop */
      hi2c->Instance->CR2 |= I2C_CR2_STOP;
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Wrong size Status regarding TCR flag event */
      hi2c->ErrorCode |= HAL_I2C_ERROR_SIZE;
      HAL_I2C_ErrorCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
  {
    /* Disable ERR, TC, STOP, NACK, TXI interrupt */
    __HAL_I2C_DISABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_TXI );

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    if(hi2c->State == HAL_I2C_STATE_MEM_BUSY_TX)
    {
      HAL_I2C_MemTxCpltCallback(hi2c);
    }
    else
    {
      HAL_I2C_MasterTxCpltCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    HAL_I2C_ErrorCallback(hi2c);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Handle Interrupt Flags Master Receive Mode
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterReceive_ISR(I2C_HandleTypeDef *hi2c)
{
  uint16_t DevAddress;

  /* Process Locked */
  __HAL_LOCK(hi2c);

  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
  {
    /* Read data from RXDR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
    hi2c->XferSize--;
    hi2c->XferCount--;
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET)
  {
    if((hi2c->XferSize == 0)&&(hi2c->XferCount!=0))
    {
      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      if(hi2c->XferCount > 255)
      {
        I2C_TransferConfig(hi2c,DevAddress,255, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
        hi2c->XferSize = 255;
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferCount, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
        hi2c->XferSize = hi2c->XferCount;
      }
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Wrong size Status regarding TCR flag event */
      hi2c->ErrorCode |= HAL_I2C_ERROR_SIZE;
      HAL_I2C_ErrorCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TC) == SET)
  {
    if(hi2c->XferCount == 0)
    {
      /* Generate Stop */
      hi2c->Instance->CR2 |= I2C_CR2_STOP;
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Wrong size Status regarding TCR flag event */
      hi2c->ErrorCode |= HAL_I2C_ERROR_SIZE;
      HAL_I2C_ErrorCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
  {
    /* Disable ERR, TC, STOP, NACK, TXI interrupt */
    __HAL_I2C_DISABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_RXI );

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    if(hi2c->State == HAL_I2C_STATE_MEM_BUSY_RX)
    {
      HAL_I2C_MemRxCpltCallback(hi2c);
    }
    else
    {
      HAL_I2C_MasterRxCpltCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    HAL_I2C_ErrorCallback(hi2c);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;

}

/**
  * @brief  Handle Interrupt Flags Slave Transmit Mode
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveTransmit_ISR(I2C_HandleTypeDef *hi2c)
{
  /* Process locked */
  __HAL_LOCK(hi2c);

  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) != RESET)
  {
    /* Check that I2C transfer finished */
    /* if yes, normal usecase, a NACK is sent by the MASTER when Transfer is finished */
    /* Mean XferCount == 0*/
    /* So clear Flag NACKF only */
    if(hi2c->XferCount == 0)
    {
      /* Clear NACK Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);
    }
    else
    {
      /* if no, error usecase, a Non-Acknowledge of last Data is generated by the MASTER*/
      /* Clear NACK Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Call the Error callback to prevent upper layer */
      HAL_I2C_ErrorCallback(hi2c);
    }
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)
  {
    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);
  }
  /* Check first if STOPF is set          */
  /* to prevent a Write Data in TX buffer */
  /* which is stuck in TXDR until next    */
  /* communication with Master            */
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
  {
    /* Disable ERRI, TCI, STOPI, NACKI, ADDRI, RXI, TXI interrupt */
    __HAL_I2C_DISABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI );

    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    HAL_I2C_SlaveTxCpltCallback(hi2c);
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == SET)
  {
    /* Write data to TXDR only if XferCount not reach "0" */
    /* A TXIS flag can be set, during STOP treatment      */
    if(hi2c->XferCount > 0)
    {
      /* Write data to TXDR */
      hi2c->Instance->TXDR = (*hi2c->pBuffPtr++);
      hi2c->XferCount--;
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Handle Interrupt Flags Slave Receive Mode
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_SlaveReceive_ISR(I2C_HandleTypeDef *hi2c)
{
  /* Process Locked */
  __HAL_LOCK(hi2c);

  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) != RESET)
  {
    /* Clear NACK Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    HAL_I2C_ErrorCallback(hi2c);
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)
  {
    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
  {
    /* Read data from RXDR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->RXDR;
    hi2c->XferSize--;
    hi2c->XferCount--;
  }
  else if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
  {
    /* Disable ERRI, TCI, STOPI, NACKI, ADDRI, RXI, TXI interrupt */
    __HAL_I2C_DISABLE_IT(hi2c,I2C_IT_ERRI | I2C_IT_TCI| I2C_IT_STOPI| I2C_IT_NACKI | I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_RXI );

    /* Disable Address Acknowledge */
    hi2c->Instance->CR2 |= I2C_CR2_NACK;

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    hi2c->State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    HAL_I2C_SlaveRxCpltCallback(hi2c);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2c);

  return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for write request.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
  I2C_TransferConfig(hi2c,DevAddress,MemAddSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

    /* Send LSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TCR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

return HAL_OK;
}

/**
  * @brief  Master sends target device address followed by internal memory address for read request.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  MemAddSize: Size of internal memory address
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
  I2C_TransferConfig(hi2c,DevAddress,MemAddSize, I2C_SOFTEND_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout) != HAL_OK)
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

    /* Send LSB of Memory Address */
    hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TC flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TC, RESET, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
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
  uint16_t DevAddress;
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Check if last DMA request was done with RELOAD */
  /* Set NBYTES to write and reload if size > 255 */
  if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
  {
    /* Wait until TCR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, I2C_TIMEOUT_TCR) != HAL_OK)
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

    /* Check if Errors has been detected during transfer */
    if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is reset */
      if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
        }
        else
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        }
      }

      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->XferCount = 0;

      hi2c->State = HAL_I2C_STATE_READY;
      HAL_I2C_ErrorCallback(hi2c);
    }
    else
    {
      hi2c->pBuffPtr += hi2c->XferSize;
      hi2c->XferCount -= hi2c->XferSize;
      if(hi2c->XferCount > 255)
      {
        hi2c->XferSize = 255;
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
      }

      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)hi2c->pBuffPtr, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

      /* Send Slave Address */
      /* Set NBYTES to write and reload if size > 255 */
      if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
      }

      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, I2C_TIMEOUT_TXIS) != HAL_OK)
      {
        /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
        {
          if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
          }
          else
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          }
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Clear Configuration Register 2 */
        I2C_RESET_CR2(hi2c);

        hi2c->XferCount = 0;

        hi2c->State = HAL_I2C_STATE_READY;
        HAL_I2C_ErrorCallback(hi2c);
      }
      else
      {
        /* Enable DMA Request */
        hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;
      }
    }
  }
  else
  {
    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
      }
      else
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

    hi2c->XferCount = 0;

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
}

/**
  * @brief  DMA I2C slave transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMASlaveTransmitCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Wait until STOP flag is set */
  if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      /* Normal Use case, a AF is generated by master */
      /* to inform slave the end of transfer */
      hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }
  }

  /* Clear STOP flag */
  __HAL_I2C_CLEAR_FLAG(hi2c,I2C_FLAG_STOPF);

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Disable DMA Request */
  hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

  hi2c->XferCount = 0;

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
  * @brief DMA I2C master receive process complete callback
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMasterReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  uint16_t DevAddress;

  /* Check if last DMA request was done with RELOAD */
  /* Set NBYTES to write and reload if size > 255 */
  if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
  {
    /* Wait until TCR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, I2C_TIMEOUT_TCR) != HAL_OK)
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

    /* Check if Errors has been detected during transfer */
    if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is reset */
      if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
        }
        else
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        }
      }

      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->XferCount = 0;

      hi2c->State = HAL_I2C_STATE_READY;
      HAL_I2C_ErrorCallback(hi2c);
    }
    else
    {
      hi2c->pBuffPtr += hi2c->XferSize;
      hi2c->XferCount -= hi2c->XferSize;
      if(hi2c->XferCount > 255)
      {
        hi2c->XferSize = 255;
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
      }

      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)hi2c->pBuffPtr, hi2c->XferSize);

      /* Send Slave Address */
      /* Set NBYTES to write and reload if size > 255 */
      if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
      }

      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, I2C_TIMEOUT_RXNE) != HAL_OK)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }

      /* Check if Errors has been detected during transfer */
      if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
      {
        /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
        {
          if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
          }
          else
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          }
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Clear Configuration Register 2 */
        I2C_RESET_CR2(hi2c);

        hi2c->XferCount = 0;

        hi2c->State = HAL_I2C_STATE_READY;

        HAL_I2C_ErrorCallback(hi2c);
      }
      else
      {
        /* Enable DMA Request */
        hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;
      }
    }
  }
  else
  {
    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
      }
      else
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

    hi2c->XferCount = 0;

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
  if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }
  }

  /* Clear STOPF flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY) != HAL_OK)
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  }

  /* Disable DMA Request */
  hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

  /* Disable Address Acknowledge */
  hi2c->Instance->CR2 |= I2C_CR2_NACK;

  hi2c->XferCount = 0;

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
  * @brief DMA I2C Memory Write process complete callback
  * @param hdma : DMA handle
  * @retval None
  */
static void I2C_DMAMemTransmitCplt(DMA_HandleTypeDef *hdma)
{
  uint16_t DevAddress;
  I2C_HandleTypeDef* hi2c = ( I2C_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Check if last DMA request was done with RELOAD */
  /* Set NBYTES to write and reload if size > 255 */
  if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
  {
    /* Wait until TCR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, I2C_TIMEOUT_TCR) != HAL_OK)
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

    /* Check if Errors has been detected during transfer */
    if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is reset */
      if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
        }
        else
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        }
      }

      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->XferCount = 0;

      hi2c->State = HAL_I2C_STATE_READY;
      HAL_I2C_ErrorCallback(hi2c);
    }
    else
    {
      hi2c->pBuffPtr += hi2c->XferSize;
      hi2c->XferCount -= hi2c->XferSize;
      if(hi2c->XferCount > 255)
      {
        hi2c->XferSize = 255;
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
      }

      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(hi2c->hdmatx, (uint32_t)hi2c->pBuffPtr, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize);

      /* Send Slave Address */
      /* Set NBYTES to write and reload if size > 255 */
      if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
      }

      /* Wait until TXIS flag is set */
      if(I2C_WaitOnTXISFlagUntilTimeout(hi2c, I2C_TIMEOUT_TXIS) != HAL_OK)
      {
        /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
        {
          if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
          }
          else
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          }
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Clear Configuration Register 2 */
        I2C_RESET_CR2(hi2c);

        hi2c->XferCount = 0;

        hi2c->State = HAL_I2C_STATE_READY;
        HAL_I2C_ErrorCallback(hi2c);
      }
      else
      {
        /* Enable DMA Request */
        hi2c->Instance->CR1 |= I2C_CR1_TXDMAEN;
      }
    }
  }
  else
  {
    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
      }
      else
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_TXDMAEN;

    hi2c->XferCount = 0;

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
}

/**
  * @brief  DMA I2C Memory Read process complete callback
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAMemReceiveCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = ( I2C_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  uint16_t DevAddress;

  /* Check if last DMA request was done with RELOAD */
  /* Set NBYTES to write and reload if size > 255 */
  if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
  {
    /* Wait until TCR flag is set */
    if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, I2C_TIMEOUT_TCR) != HAL_OK)
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
    }

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

    /* Check if Errors has been detected during transfer */
    if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is reset */
      if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
        }
        else
        {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        }
      }

      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->XferCount = 0;

      hi2c->State = HAL_I2C_STATE_READY;
      HAL_I2C_ErrorCallback(hi2c);
    }
    else
    {
      hi2c->pBuffPtr += hi2c->XferSize;
      hi2c->XferCount -= hi2c->XferSize;
      if(hi2c->XferCount > 255)
      {
        hi2c->XferSize = 255;
      }
      else
      {
        hi2c->XferSize = hi2c->XferCount;
      }

      DevAddress = (hi2c->Instance->CR2 & I2C_CR2_SADD);

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->RXDR, (uint32_t)hi2c->pBuffPtr, hi2c->XferSize);

      /* Send Slave Address */
      /* Set NBYTES to write and reload if size > 255 */
      if( (hi2c->XferSize == 255) && (hi2c->XferSize < hi2c->XferCount) )
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
      else
      {
        I2C_TransferConfig(hi2c,DevAddress,hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
      }

      /* Wait until RXNE flag is set */
      if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, I2C_TIMEOUT_RXNE) != HAL_OK)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }

      /* Check if Errors has been detected during transfer */
      if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
      {
        /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
        /* Wait until STOPF flag is reset */
        if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
        {
          if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
          }
          else
          {
            hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          }
        }

        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Clear Configuration Register 2 */
        I2C_RESET_CR2(hi2c);

        hi2c->XferCount = 0;

        hi2c->State = HAL_I2C_STATE_READY;
        HAL_I2C_ErrorCallback(hi2c);
      }
      else
      {
        /* Enable DMA Request */
        hi2c->Instance->CR1 |= I2C_CR1_RXDMAEN;
      }
    }
  }
  else
  {
    /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
    /* Wait until STOPF flag is reset */
    if(I2C_WaitOnSTOPFlagUntilTimeout(hi2c, I2C_TIMEOUT_STOPF) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
      }
      else
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      }
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    /* Disable DMA Request */
    hi2c->Instance->CR1 &= ~I2C_CR1_RXDMAEN;

    hi2c->XferCount = 0;

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
}

/**
  * @brief  DMA I2C communication error callback.
  * @param hdma : DMA handle
  * @retval None
  */
static void I2C_DMAError(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = ( I2C_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Disable Acknowledge */
  hi2c->Instance->CR2 |= I2C_CR2_NACK;

  hi2c->XferCount = 0;

  hi2c->State = HAL_I2C_STATE_READY;

  hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;

  HAL_I2C_ErrorCallback(hi2c);
}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Flag: specifies the I2C flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

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
  * @brief  This function handles I2C Communication Timeout for specific usage of TXIS flag.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
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
  * @brief  This function handles I2C Communication Timeout for specific usage of STOP flag.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout)
{
  uint32_t tickstart = 0x00;
  tickstart = HAL_GetTick();

  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of RXNE flag.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout)
{
  uint32_t tickstart = 0x00;
  tickstart = HAL_GetTick();

  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET)
  {
    /* Check if a STOPF is detected */
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
    {
      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      /* Clear Configuration Register 2 */
      I2C_RESET_CR2(hi2c);

      hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c : Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c, uint32_t Timeout)
{
  uint32_t tickstart = 0x00;
  tickstart = HAL_GetTick();

  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Generate stop if necessary only in case of I2C peripheral in MASTER mode */
    if((hi2c->State == HAL_I2C_STATE_MASTER_BUSY_TX) || (hi2c->State == HAL_I2C_STATE_MEM_BUSY_TX)
       || (hi2c->State == HAL_I2C_STATE_MEM_BUSY_RX))
    {
      /* No need to generate the STOP condition if AUTOEND mode is enabled */
      /* Generate the STOP condition only in case of SOFTEND mode is enabled */
      if((hi2c->Instance->CR2 & I2C_AUTOEND_MODE) != I2C_AUTOEND_MODE)
      {
        /* Generate Stop */
        hi2c->Instance->CR2 |= I2C_CR2_STOP;
      }
    }

    /* Wait until STOP Flag is reset */
    /* AutoEnd should be initiate after AF */
    while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
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

    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->ErrorCode = HAL_I2C_ERROR_AF;
    hi2c->State= HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  Handles I2Cx communication when starting transfer or during transfer (TC or TCR flag are set).
  * @param  hi2c: I2C handle.
  * @param  DevAddress: specifies the slave address to be programmed.
  * @param  Size: specifies the number of bytes to be programmed.
  *   This parameter must be a value between 0 and 255.
  * @param  Mode: new state of the I2C START condition generation.
  *   This parameter can be one of the following values:
  *     @arg I2C_RELOAD_MODE: Enable Reload mode .
  *     @arg I2C_AUTOEND_MODE: Enable Automatic end mode.
  *     @arg I2C_SOFTEND_MODE: Enable Software end mode.
  * @param  Request: new state of the I2C START condition generation.
  *   This parameter can be one of the following values:
  *     @arg I2C_NO_STARTSTOP: Don't Generate stop and start condition.
  *     @arg I2C_GENERATE_STOP: Generate stop condition (Size should be set to 0).
  *     @arg I2C_GENERATE_START_READ: Generate Restart for read request.
  *     @arg I2C_GENERATE_START_WRITE: Generate Restart for write request.
  * @retval None
  */
static void I2C_TransferConfig(I2C_HandleTypeDef *hi2c,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_TRANSFER_MODE(Mode));
  assert_param(IS_TRANSFER_REQUEST(Request));

  /* Get the CR2 register value */
  tmpreg = hi2c->Instance->CR2;

  /* clear tmpreg specific bits */
  tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

  /* update tmpreg */
  tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES) | \
            (uint32_t)Mode | (uint32_t)Request);

  /* update CR2 register */
  hi2c->Instance->CR2 = tmpreg;
}

/**
  * @}
  */

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
