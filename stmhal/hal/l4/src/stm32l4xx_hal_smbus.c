/**
  ******************************************************************************
  * @file    stm32l4xx_hal_smbus.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   SMBUS HAL module driver. 
  *          This file provides firmware functions to manage the following 
  *          functionalities of the System Management Bus (SMBus) peripheral,
  *          based on I2C principles of operation :
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
  *         
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
    The SMBUS HAL driver can be used as follows:
    
    (#) Declare a SMBUS_HandleTypeDef handle structure, for example:
        SMBUS_HandleTypeDef  hsmbus; 

    (#)Initialize the SMBUS low level resources by implementing the HAL_SMBUS_MspInit() API:
        (++) Enable the SMBUSx interface clock with __HAL_RCC_I2Cx_CLK_ENABLE()
        (++) SMBUS pins configuration
            (+++) Enable the clock for the SMBUS GPIOs
            (+++) Configure SMBUS pins as alternate function open-drain
        (++) NVIC configuration if you need to use interrupt process
            (+++) Configure the SMBUSx interrupt priority 
            (+++) Enable the NVIC SMBUS IRQ Channel

    (#) Configure the Communication Clock Timing, Bus Timeout, Own Address1, Master Addressing Mode,
        Dual Addressing mode, Own Address2, Own Address2 Mask, General call, Nostretch mode,
        Peripheral mode and Packet Error Check mode in the hsmbus Init structure.

    (#) Initialize the SMBUS registers by calling the HAL_SMBUS_Init() API:
        (++) These API's configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
             by calling the customized HAL_SMBUS_MspInit(&hsmbus) API.

    (#) To check if target device is ready for communication, use the function HAL_SMBUS_IsDeviceReady()

    (#) For SMBUS IO operations, only one mode of operations is available within this driver :
            
    *** Interrupt mode IO operation ***
    ===================================
    [..]
      (+) Transmit in master/host SMBUS mode an amount of data in non-blocking mode using HAL_SMBUS_Master_Transmit_IT()
      (++) At transmission end of transfer HAL_SMBUS_MasterTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_MasterTxCpltCallback()
      (+) Receive in master/host SMBUS mode an amount of data in non-blocking mode using HAL_SMBUS_Master_Receive_IT()
      (++) At reception end of transfer HAL_SMBUS_MasterRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_MasterRxCpltCallback()
      (+) Abort a master/host SMBUS process communication with Interrupt using HAL_SMBUS_Master_Abort_IT()
      (++) The associated previous transfer callback is called at the end of abort process
      (++) mean HAL_SMBUS_MasterTxCpltCallback() in case of previous state was master transmit
      (++) mean HAL_SMBUS_MasterRxCpltCallback() in case of previous state was master receive
      (+) Enable/disable the Address listen mode in slave/device or host/slave SMBUS mode
           using HAL_SMBUS_EnableListen_IT() HAL_SMBUS_DisableListen_IT()
      (++) When address slave/device SMBUS match, HAL_SMBUS_AddrCallback() is executed and user can
           add his own code to check the Address Match Code and the transmission direction request by master/host (Write/Read).
      (++) At Listen mode end HAL_SMBUS_ListenCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_ListenCpltCallback()
      (+) Transmit in slave/device SMBUS mode an amount of data in non-blocking mode using HAL_SMBUS_Slave_Transmit_IT()
      (++) At transmission end of transfer HAL_SMBUS_SlaveTxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_SlaveTxCpltCallback()
      (+) Receive in slave/device SMBUS mode an amount of data in non-blocking mode using HAL_SMBUS_Slave_Receive_IT()
      (++) At reception end of transfer HAL_SMBUS_SlaveRxCpltCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_SlaveRxCpltCallback()
      (+) Enable/Disable the SMBUS alert mode using HAL_SMBUS_EnableAlert_IT() HAL_SMBUS_DisableAlert_IT()
      (++) When SMBUS Alert is generated HAL_SMBUS_ErrorCallback() is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_ErrorCallback()
           to check the Alert Error Code using function HAL_SMBUS_GetError()
      (+) Get HAL state machine or error values using HAL_SMBUS_GetState() or HAL_SMBUS_GetError()
      (+) In case of transfer Error, HAL_SMBUS_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer HAL_SMBUS_ErrorCallback()
           to check the Error Code using function HAL_SMBUS_GetError()

     *** SMBUS HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in SMBUS HAL driver.

      (+) __HAL_SMBUS_ENABLE: Enable the SMBUS peripheral
      (+) __HAL_SMBUS_DISABLE: Disable the SMBUS peripheral
      (+) __HAL_SMBUS_GET_FLAG : Checks whether the specified SMBUS flag is set or not
      (+) __HAL_SMBUS_CLEAR_FLAG : Clears the specified SMBUS pending flag
      (+) __HAL_SMBUS_ENABLE_IT: Enables the specified SMBUS interrupt
      (+) __HAL_SMBUS_DISABLE_IT: Disables the specified SMBUS interrupt

     [..]
       (@) You can refer to the SMBUS HAL driver header file for more useful macros

            
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup SMBUS SMBUS
  * @brief SMBUS HAL module driver
  * @{
  */

#ifdef HAL_SMBUS_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup SMBUS_Private_Define SMBUS Private Constants
 * @{
 */
#define TIMING_CLEAR_MASK   ((uint32_t)0xF0FFFFFF)      /*<! SMBUS TIMING clear register Mask */
#define HAL_TIMEOUT_ADDR    ((uint32_t)10000)           /* 10 s  */
#define HAL_TIMEOUT_BUSY    ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_DIR     ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_RXNE    ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_STOPF   ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_TC      ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_TCR     ((uint32_t)25)              /* 25 ms */
#define HAL_TIMEOUT_TXIS    ((uint32_t)25)              /* 25 ms */
#define MAX_NBYTE_SIZE      255
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SMBUS_Private_Functions SMBUS Private Functions
  * @{
  */
static HAL_StatusTypeDef SMBUS_WaitOnFlagUntilTimeout(SMBUS_HandleTypeDef *hsmbus, uint32_t Flag, FlagStatus Status, uint32_t Timeout);

static HAL_StatusTypeDef SMBUS_Enable_IRQ(SMBUS_HandleTypeDef *hsmbus, uint16_t InterruptRequest);
static HAL_StatusTypeDef SMBUS_Disable_IRQ(SMBUS_HandleTypeDef *hsmbus, uint16_t InterruptRequest);
static HAL_StatusTypeDef SMBUS_Master_ISR(SMBUS_HandleTypeDef *hsmbus);
static HAL_StatusTypeDef SMBUS_Slave_ISR(SMBUS_HandleTypeDef *hsmbus);

static void SMBUS_TransferConfig(SMBUS_HandleTypeDef *hsmbus,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup SMBUS_Exported_Functions SMBUS Exported Functions
  * @{
  */

/** @defgroup SMBUS_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and 
          de-initialize the SMBUSx peripheral:

      (+) User must Implement HAL_SMBUS_MspInit() function in which he configures 
          all related peripherals resources (CLOCK, GPIO, IT and NVIC ).

      (+) Call the function HAL_SMBUS_Init() to configure the selected device with 
          the selected configuration:
        (++) Clock Timing
        (++) Bus Timeout
        (++) Analog Filer mode
        (++) Own Address 1
        (++) Addressing mode (Master, Slave)
        (++) Dual Addressing mode
        (++) Own Address 2
        (++) Own Address 2 Mask
        (++) General call mode
        (++) Nostretch mode
        (++) Packet Error Check mode
        (++) Peripheral mode


      (+) Call the function HAL_SMBUS_DeInit() to restore the default configuration 
          of the selected SMBUSx peripheral.       

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the SMBUS according to the specified parameters 
  *         in the SMBUS_InitTypeDef and initialize the associated handle.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Init(SMBUS_HandleTypeDef *hsmbus)
{ 
  /* Check the SMBUS handle allocation */
  if(hsmbus == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_SMBUS_ALL_INSTANCE(hsmbus->Instance));
  assert_param(IS_SMBUS_ANALOG_FILTER(hsmbus->Init.AnalogFilter));
  assert_param(IS_SMBUS_OWN_ADDRESS1(hsmbus->Init.OwnAddress1));
  assert_param(IS_SMBUS_ADDRESSING_MODE(hsmbus->Init.AddressingMode));
  assert_param(IS_SMBUS_DUAL_ADDRESS(hsmbus->Init.DualAddressMode));
  assert_param(IS_SMBUS_OWN_ADDRESS2(hsmbus->Init.OwnAddress2));
  assert_param(IS_SMBUS_OWN_ADDRESS2_MASK(hsmbus->Init.OwnAddress2Masks));
  assert_param(IS_SMBUS_GENERAL_CALL(hsmbus->Init.GeneralCallMode));
  assert_param(IS_SMBUS_NO_STRETCH(hsmbus->Init.NoStretchMode));
  assert_param(IS_SMBUS_PEC(hsmbus->Init.PacketErrorCheckMode));
  assert_param(IS_SMBUS_PERIPHERAL_MODE(hsmbus->Init.PeripheralMode));

  if(hsmbus->State == HAL_SMBUS_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hsmbus->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_SMBUS_MspInit(hsmbus);
  }
  
  hsmbus->State = HAL_SMBUS_STATE_BUSY;
  
  /* Disable the selected SMBUS peripheral */
  __HAL_SMBUS_DISABLE(hsmbus);
  
  /*---------------------------- SMBUSx TIMINGR Configuration ------------------------*/  
  /* Configure SMBUSx: Frequency range */
  hsmbus->Instance->TIMINGR = hsmbus->Init.Timing & TIMING_CLEAR_MASK;
  
  /*---------------------------- SMBUSx TIMEOUTR Configuration ------------------------*/  
  /* Configure SMBUSx: Bus Timeout  */
  hsmbus->Instance->TIMEOUTR &= ~I2C_TIMEOUTR_TIMOUTEN;
  hsmbus->Instance->TIMEOUTR &= ~I2C_TIMEOUTR_TEXTEN;
  hsmbus->Instance->TIMEOUTR = hsmbus->Init.SMBusTimeout;

  /*---------------------------- SMBUSx OAR1 Configuration -----------------------*/
  /* Configure SMBUSx: Own Address1 and ack own address1 mode */
  hsmbus->Instance->OAR1 &= ~I2C_OAR1_OA1EN;
  
  if(hsmbus->Init.OwnAddress1 != 0)
  {
    if(hsmbus->Init.AddressingMode == SMBUS_ADDRESSINGMODE_7BIT)
  {
    hsmbus->Instance->OAR1 = (I2C_OAR1_OA1EN | hsmbus->Init.OwnAddress1);
  }
    else /* SMBUS_ADDRESSINGMODE_10BIT */
  {
    hsmbus->Instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | hsmbus->Init.OwnAddress1);
  }
  }

  /*---------------------------- SMBUSx CR2 Configuration ------------------------*/
  /* Configure SMBUSx: Addressing Master mode */
  if(hsmbus->Init.AddressingMode == SMBUS_ADDRESSINGMODE_10BIT)
  {
    hsmbus->Instance->CR2 = (I2C_CR2_ADD10);
  }
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process) */
  /* AUTOEND and NACK bit will be manage during Transfer process */
  hsmbus->Instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
  
  /*---------------------------- SMBUSx OAR2 Configuration -----------------------*/  
  /* Configure SMBUSx: Dual mode and Own Address2 */
  hsmbus->Instance->OAR2 = (hsmbus->Init.DualAddressMode | hsmbus->Init.OwnAddress2 | (hsmbus->Init.OwnAddress2Masks << 8));

  /*---------------------------- SMBUSx CR1 Configuration ------------------------*/
  /* Configure SMBUSx: Generalcall and NoStretch mode */
  hsmbus->Instance->CR1 = (hsmbus->Init.GeneralCallMode | hsmbus->Init.NoStretchMode | hsmbus->Init.PacketErrorCheckMode | hsmbus->Init.PeripheralMode | hsmbus->Init.AnalogFilter);
  
  /* Enable Slave Byte Control only in case of Packet Error Check is enabled and SMBUS Peripheral is set in Slave mode */
  if( (hsmbus->Init.PacketErrorCheckMode == SMBUS_PEC_ENABLE)
     && ( (hsmbus->Init.PeripheralMode == SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE) || (hsmbus->Init.PeripheralMode == SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE_ARP) ) )
  {
    hsmbus->Instance->CR1 |= I2C_CR1_SBC;
  }

  /* Enable the selected SMBUS peripheral */
  __HAL_SMBUS_ENABLE(hsmbus);
  
  hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
  hsmbus->PreviousState = HAL_SMBUS_STATE_READY;
  hsmbus->State = HAL_SMBUS_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  DeInitialize the SMBUS peripheral. 
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_DeInit(SMBUS_HandleTypeDef *hsmbus)
{
  /* Check the SMBUS handle allocation */
  if(hsmbus == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_SMBUS_ALL_INSTANCE(hsmbus->Instance));
  
  hsmbus->State = HAL_SMBUS_STATE_BUSY;
  
  /* Disable the SMBUS Peripheral Clock */
  __HAL_SMBUS_DISABLE(hsmbus);
  
  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_SMBUS_MspDeInit(hsmbus);
  
  hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
  hsmbus->PreviousState =  HAL_SMBUS_STATE_RESET;
  hsmbus->State = HAL_SMBUS_STATE_RESET;
  
   /* Release Lock */
  __HAL_UNLOCK(hsmbus);
  
  return HAL_OK;
}

/**
  * @brief Initialize the SMBUS MSP.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_MspInit(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief DeInitialize the SMBUS MSP.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_MspDeInit(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SMBUS_Exported_Functions_Group2 Input and Output operation functions
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the SMBUS data 
    transfers.

    (#) Blocking mode function to check if device is ready for usage is :
        (++) HAL_SMBUS_IsDeviceReady()

    (#) There is only one mode of transfer:
       (++) No-Blocking mode : The communication is performed using Interrupts.
            These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the 
            dedicated SMBUS IRQ when using Interrupt mode.

    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_SMBUS_Master_Transmit_IT()
        (++) HAL_SMBUS_Master_Receive_IT()
        (++) HAL_SMBUS_Slave_Transmit_IT()
        (++) HAL_SMBUS_Slave_Receive_IT()
        (++) HAL_SMBUS_EnableListen_IT() or alias HAL_SMBUS_EnableListen_IT()
        (++) HAL_SMBUS_DisableListen_IT()
        (++) HAL_SMBUS_EnableAlert_IT()
        (++) HAL_SMBUS_DisableAlert_IT()

    (#) A set of Transfer Complete Callbacks are provided in No_Blocking mode:
        (++) HAL_SMBUS_MasterTxCpltCallback()
        (++) HAL_SMBUS_MasterRxCpltCallback()
        (++) HAL_SMBUS_SlaveTxCpltCallback()
        (++) HAL_SMBUS_SlaveRxCpltCallback()
        (++) HAL_SMBUS_AddrCallback()
        (++) HAL_SMBUS_ListenCpltCallback()
        (++) HAL_SMBUS_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmit in master/host SMBUS mode an amount of data in non-blocking mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref SMBUS_XferOptions_definition
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Master_Transmit_IT(SMBUS_HandleTypeDef *hsmbus, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{   
  /* Check the parameters */
  assert_param(IS_SMBUS_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hsmbus->State == HAL_SMBUS_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    hsmbus->State = HAL_SMBUS_STATE_MASTER_BUSY_TX;
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    /* Prepare transfer parameters */
    hsmbus->pBuffPtr = pData;
    hsmbus->XferCount = Size;
    hsmbus->XferOptions = XferOptions;

    /* In case of Quick command, remove autoend mode */
    /* Manage the stop generation by software */
    if(hsmbus->pBuffPtr == NULL)
    {
      hsmbus->XferOptions &= ~SMBUS_AUTOEND_MODE;
    }

    if(Size > MAX_NBYTE_SIZE)
    {
      hsmbus->XferSize = MAX_NBYTE_SIZE;
    }
    else
    {
      hsmbus->XferSize = Size;
    }
    
    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > MAX_NBYTE_SIZE and generate RESTART */
    if( (hsmbus->XferSize == MAX_NBYTE_SIZE) && (hsmbus->XferSize < hsmbus->XferCount) )
    {
      SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, SMBUS_RELOAD_MODE | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE), SMBUS_GENERATE_START_WRITE);
    }
    else
    {
      /* If transfer direction not change, do not generate Restart Condition */
      /* Mean Previous state is same as current state */
      if(hsmbus->PreviousState == HAL_SMBUS_STATE_MASTER_BUSY_TX)
      {
        SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
      }
      /* Else transfer direction change, so generate Restart with new transfer direction */
      else
      {
        SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_GENERATE_START_WRITE);
      }

      /* If PEC mode is enable, size to transmit manage by SW part should be Size-1 byte, corresponding to PEC byte */
      /* PEC byte is automatically sent by HW block, no need to manage it in Transmit process */
      if(SMBUS_GET_PEC_MODE(hsmbus) != RESET)
      {
        hsmbus->XferSize--;
        hsmbus->XferCount--;
      }
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus); 

    /* Note : The SMBUS interrupts must be enabled after unlocking current process 
              to avoid the risk of SMBUS interrupt handle execution before current
              process unlock */
    SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_TX);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  } 
}

/**
  * @brief  Receive in master/host SMBUS mode an amount of data in non-blocking mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  DevAddress: Target device address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref SMBUS_XferOptions_definition
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Master_Receive_IT(SMBUS_HandleTypeDef *hsmbus, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  /* Check the parameters */
  assert_param(IS_SMBUS_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hsmbus->State == HAL_SMBUS_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    hsmbus->State = HAL_SMBUS_STATE_MASTER_BUSY_RX;
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    
    /* Prepare transfer parameters */
    hsmbus->pBuffPtr = pData;
    hsmbus->XferCount = Size;
    hsmbus->XferOptions = XferOptions;
    
    /* In case of Quick command, remove autoend mode */
    /* Manage the stop generation by software */
    if(hsmbus->pBuffPtr == NULL)
    {
      hsmbus->XferOptions &= ~SMBUS_AUTOEND_MODE;
    }
    
    if(Size > MAX_NBYTE_SIZE)
    {
      hsmbus->XferSize = MAX_NBYTE_SIZE;
    }
    else
    {
      hsmbus->XferSize = Size;
    }
    
    /* Send Slave Address */
    /* Set NBYTES to write and reload if size > MAX_NBYTE_SIZE and generate RESTART */
    if( (hsmbus->XferSize == MAX_NBYTE_SIZE) && (hsmbus->XferSize < hsmbus->XferCount) )
    {
      SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, SMBUS_RELOAD_MODE  | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE), SMBUS_GENERATE_START_READ);
    }
    else
    {
      /* If transfer direction not change, do not generate Restart Condition */
      /* Mean Previous state is same as current state */
      if(hsmbus->PreviousState == HAL_SMBUS_STATE_MASTER_BUSY_RX)
      {
        SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
      }
      /* Else transfer direction change, so generate Restart with new transfer direction */
      else
      {
        SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_GENERATE_START_READ);
      }
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus); 

    /* Note : The SMBUS interrupts must be enabled after unlocking current process 
              to avoid the risk of SMBUS interrupt handle execution before current
              process unlock */
    SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_RX);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  } 
}

/**
  * @brief  Abort a master/host SMBUS process communication with Interrupt.
  * @note   This abort can be called only if state is ready
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  DevAddress: Target device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Master_Abort_IT(SMBUS_HandleTypeDef *hsmbus, uint16_t DevAddress)
{
  if(hsmbus->State == HAL_SMBUS_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    /* Keep the same state as previous */
    /* to perform as well the call of the corresponding end of transfer callback */
    if(hsmbus->PreviousState == HAL_SMBUS_STATE_MASTER_BUSY_TX)
    {
      hsmbus->State = HAL_SMBUS_STATE_MASTER_BUSY_TX;
    }
    else if(hsmbus->PreviousState == HAL_SMBUS_STATE_MASTER_BUSY_RX)
    {
      hsmbus->State = HAL_SMBUS_STATE_MASTER_BUSY_RX;
    }
    else
    {
      /* Wrong usage of abort function */
      /* This function should be used only in case of abort monitored by master device */
      return HAL_ERROR;
    }
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    
    /* Set NBYTES to 1 to generate a dummy read on SMBUS peripheral */
    /* Set AUTOEND mode, this will generate a NACK then STOP condition to abort the current transfer */
    SMBUS_TransferConfig(hsmbus, DevAddress, 1, SMBUS_AUTOEND_MODE, SMBUS_NO_STARTSTOP);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus); 

    /* Note : The SMBUS interrupts must be enabled after unlocking current process 
              to avoid the risk of SMBUS interrupt handle execution before current
              process unlock */
    if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_TX)
    {
      SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_TX);
    }
    else if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_RX)
    {
      SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_RX);
    }
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  } 
}

/**
  * @brief  Transmit in slave/device SMBUS mode an amount of data in non-blocking mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref SMBUS_XferOptions_definition
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Slave_Transmit_IT(SMBUS_HandleTypeDef *hsmbus, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  /* Check the parameters */
  assert_param(IS_SMBUS_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hsmbus->State == HAL_SMBUS_STATE_LISTEN)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;                                    
    }

    /* Disable Interrupts, to prevent preemption during treatment in case of multicall */
    SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_ADDR | SMBUS_IT_TX);

    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    hsmbus->State |= HAL_SMBUS_STATE_SLAVE_BUSY_TX;
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    
    /* Set SBC bit to manage Acknowledge at each bit */
    hsmbus->Instance->CR1 |= I2C_CR1_SBC;

    /* Enable Address Acknowledge */
    hsmbus->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hsmbus->pBuffPtr = pData;
    hsmbus->XferSize = Size;
    hsmbus->XferCount = Size;
    hsmbus->XferOptions = XferOptions;

    if(Size > MAX_NBYTE_SIZE)
    {
      hsmbus->XferSize = MAX_NBYTE_SIZE;
    }
    else
    {
      hsmbus->XferSize = Size;
    }

    /* Set NBYTES to write and reload if size > MAX_NBYTE_SIZE and generate RESTART */
    if( (hsmbus->XferSize == MAX_NBYTE_SIZE) && (hsmbus->XferSize < hsmbus->XferCount) )
    {
      SMBUS_TransferConfig(hsmbus,0,hsmbus->XferSize, SMBUS_RELOAD_MODE | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE), SMBUS_NO_STARTSTOP);
    }
    else
    {
      /* Set NBYTE to transmit */
      SMBUS_TransferConfig(hsmbus,0,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);

      /* If PEC mode is enable, size to transmit should be Size-1 byte, corresponding to PEC byte */
      /* PEC byte is automatically sent by HW block, no need to manage it in Transmit process */
      if(SMBUS_GET_PEC_MODE(hsmbus) != RESET)
      {
        hsmbus->XferSize--;
        hsmbus->XferCount--;
      }
    }
    
    /* Clear ADDR flag after prepare the transfer parameters */
    /* This action will generate an acknowledge to the HOST */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus,SMBUS_FLAG_ADDR);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus); 

    /* Note : The SMBUS interrupts must be enabled after unlocking current process 
              to avoid the risk of SMBUS interrupt handle execution before current
              process unlock */
    /* REnable ADDR interrupt */
    SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_TX | SMBUS_IT_ADDR);

    return HAL_OK;
  }
  else
  {
    return HAL_ERROR; 
  } 
}

/**
  * @brief  Receive in slave/device SMBUS mode an amount of data in non-blocking mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  XferOptions: Options of Transfer, value of @ref SMBUS_XferOptions_definition
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_Slave_Receive_IT(SMBUS_HandleTypeDef *hsmbus, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
  /* Check the parameters */
  assert_param(IS_SMBUS_TRANSFER_OPTIONS_REQUEST(XferOptions));

  if(hsmbus->State == HAL_SMBUS_STATE_LISTEN)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;                                    
    }
    
    /* Disable Interrupts, to prevent preemption during treatment in case of multicall */
    SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_ADDR | SMBUS_IT_RX);

    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    hsmbus->State |= HAL_SMBUS_STATE_SLAVE_BUSY_RX;
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    
    /* Set SBC bit to manage Acknowledge at each bit */
    hsmbus->Instance->CR1 |= I2C_CR1_SBC;

    /* Enable Address Acknowledge */
    hsmbus->Instance->CR2 &= ~I2C_CR2_NACK;

    /* Prepare transfer parameters */
    hsmbus->pBuffPtr = pData;
    hsmbus->XferSize = Size;
    hsmbus->XferCount = Size;
    hsmbus->XferOptions = XferOptions;
    
    /* Set NBYTE to receive */
    /* If XferSize equal "1", or XferSize equal "2" with PEC requested (mean 1 data byte + 1 PEC byte */
    /* no need to set RELOAD bit mode, a ACK will be automatically generated in that case */
    /* else need to set RELOAD bit mode to generate an automatic ACK at each byte Received */
    /* This RELOAD bit will be reset for last BYTE to be receive in SMBUS_Slave_ISR */
    if((hsmbus->XferSize == 1) || ((hsmbus->XferSize == 2) && (SMBUS_GET_PEC_MODE(hsmbus) != RESET)))
    {
      SMBUS_TransferConfig(hsmbus,0,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
    }
    else
    {
      SMBUS_TransferConfig(hsmbus,0, 1, hsmbus->XferOptions | SMBUS_RELOAD_MODE, SMBUS_NO_STARTSTOP);
    }

    /* Clear ADDR flag after prepare the transfer parameters */
    /* This action will generate an acknowledge to the HOST */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus,SMBUS_FLAG_ADDR);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus); 

    /* Note : The SMBUS interrupts must be enabled after unlocking current process 
              to avoid the risk of SMBUS interrupt handle execution before current
              process unlock */
    /* REnable ADDR interrupt */
    SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_RX | SMBUS_IT_ADDR);

    return HAL_OK;
  }
  else
  {
    return HAL_ERROR; 
  }
}

/**
  * @brief  Enable the Address listen mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_EnableListen_IT(SMBUS_HandleTypeDef *hsmbus)
{
  hsmbus->State = HAL_SMBUS_STATE_LISTEN;
  
  /* Enable the Address Match interrupt */
  SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_ADDR);
  
  return HAL_OK;
}

/**
  * @brief  Disable the Address listen mode with Interrupt.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_DisableListen_IT(SMBUS_HandleTypeDef *hsmbus)
{
  /* Disable Address listen mode only if a transfer is not ongoing */
  if(hsmbus->State == HAL_SMBUS_STATE_LISTEN)
  {
    hsmbus->State = HAL_SMBUS_STATE_READY;
  
    /* Disable the Address Match interrupt */
    SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_ADDR);
  
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Enable the SMBUS alert mode with Interrupt.
  * @param  hsmbus : pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUSx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_EnableAlert_IT(SMBUS_HandleTypeDef *hsmbus)
{
  /* Enable SMBus alert */
  hsmbus->Instance->CR1 |= I2C_CR1_ALERTEN;   

  /* Clear ALERT flag */
  __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_ALERT);

  /* Enable Alert Interrupt */
  SMBUS_Enable_IRQ(hsmbus, SMBUS_IT_ALERT);

  return HAL_OK; 
}
/**
  * @brief  Disable the SMBUS alert mode with Interrupt.
  * @param  hsmbus : pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUSx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_DisableAlert_IT(SMBUS_HandleTypeDef *hsmbus)
{
  /* Enable SMBus alert */
  hsmbus->Instance->CR1 &= ~I2C_CR1_ALERTEN;   
  
  /* Disable Alert Interrupt */
  SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_ALERT);

  return HAL_OK; 
}

/**
  * @brief  Check if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMBUS_IsDeviceReady(SMBUS_HandleTypeDef *hsmbus, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{  
  uint32_t tickstart = 0;
  
  __IO uint32_t SMBUS_Trials = 0;
 
  if(hsmbus->State == HAL_SMBUS_STATE_READY)
  {
    if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_BUSY) != RESET)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hsmbus);
    
    hsmbus->State = HAL_SMBUS_STATE_BUSY;
    hsmbus->ErrorCode = HAL_SMBUS_ERROR_NONE;
    
    do
    {
      /* Generate Start */
      hsmbus->Instance->CR2 = SMBUS_GENERATE_START(hsmbus->Init.AddressingMode,DevAddress);
      
      /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
      /* Wait until STOPF flag is set or a NACK flag is set*/
      tickstart = HAL_GetTick();
      while((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_STOPF) == RESET) && (__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_AF) == RESET) && (hsmbus->State != HAL_SMBUS_STATE_TIMEOUT))
      {
        if(Timeout != HAL_MAX_DELAY)
        {    
          if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
          {
            /* Device is ready */
            hsmbus->State = HAL_SMBUS_STATE_READY;
        
            /* Process Unlocked */
            __HAL_UNLOCK(hsmbus);
            return HAL_TIMEOUT;
          }
        } 
      }
      
      /* Check if the NACKF flag has not been set */
      if (__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_AF) == RESET)
      {
        /* Wait until STOPF flag is reset */ 
        if(SMBUS_WaitOnFlagUntilTimeout(hsmbus, SMBUS_FLAG_STOPF, RESET, Timeout) != HAL_OK)      
        {
          return HAL_TIMEOUT;
        }
        
        /* Clear STOP Flag */
        __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);

        /* Device is ready */
        hsmbus->State = HAL_SMBUS_STATE_READY;
        
        /* Process Unlocked */
        __HAL_UNLOCK(hsmbus);
        
        return HAL_OK;
      }
      else
      {
        /* Wait until STOPF flag is reset */ 
        if(SMBUS_WaitOnFlagUntilTimeout(hsmbus, SMBUS_FLAG_STOPF, RESET, Timeout) != HAL_OK)      
        {
          return HAL_TIMEOUT;
        }

        /* Clear NACK Flag */
        __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_AF);

        /* Clear STOP Flag, auto generated with autoend*/
        __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);
      }
      
      /* Check if the maximum allowed number of trials has been reached */
      if (SMBUS_Trials++ == Trials)
      {
        /* Generate Stop */
        hsmbus->Instance->CR2 |= I2C_CR2_STOP;
        
        /* Wait until STOPF flag is reset */ 
        if(SMBUS_WaitOnFlagUntilTimeout(hsmbus, SMBUS_FLAG_STOPF, RESET, Timeout) != HAL_OK)      
        {
          return HAL_TIMEOUT;
        }
        
        /* Clear STOP Flag */
        __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);
      }      
    }while(SMBUS_Trials < Trials);

    hsmbus->State = HAL_SMBUS_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus);
        
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

/** @defgroup SMBUS_IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
 * @{
 */

/**
  * @brief  Handle SMBUS event interrupt request.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
void HAL_SMBUS_EV_IRQHandler(SMBUS_HandleTypeDef *hsmbus)
{
  uint32_t tmpisrvalue = 0;
  
  /* Use a local variable to store the current ISR flags */
  /* This action will avoid a wrong treatment due to ISR flags change during interrupt handler */
  tmpisrvalue = SMBUS_GET_ISR_REG(hsmbus);
    
  /* SMBUS in mode Transmitter ---------------------------------------------------*/
  if (((SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_TXIS) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_TCR) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_TC) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_STOPF) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_AF) != RESET)) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, (SMBUS_IT_TCI| SMBUS_IT_STOPI| SMBUS_IT_NACKI | SMBUS_IT_TXI)) != RESET))
  {     
    /* Slave mode selected */
    if ((hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_TX) == HAL_SMBUS_STATE_SLAVE_BUSY_TX)
    {
      SMBUS_Slave_ISR(hsmbus);
    }
    /* Master mode selected */
    else if((hsmbus->State & HAL_SMBUS_STATE_MASTER_BUSY_TX) == HAL_SMBUS_STATE_MASTER_BUSY_TX)
    {
      SMBUS_Master_ISR(hsmbus);
    }
  }
    
  /* SMBUS in mode Receiver ----------------------------------------------------*/
  if (((SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_RXNE) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_TCR) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_TC) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_STOPF) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_AF) != RESET)) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, (SMBUS_IT_TCI| SMBUS_IT_STOPI| SMBUS_IT_NACKI | SMBUS_IT_RXI)) != RESET))
  {
    /* Slave mode selected */
    if ((hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_RX) == HAL_SMBUS_STATE_SLAVE_BUSY_RX)
    {
      SMBUS_Slave_ISR(hsmbus);
    }
    /* Master mode selected */
    else if((hsmbus->State & HAL_SMBUS_STATE_MASTER_BUSY_RX) == HAL_SMBUS_STATE_MASTER_BUSY_RX)
    {
      SMBUS_Master_ISR(hsmbus);
    }
  } 
      
   /* SMBUS in mode Listener Only --------------------------------------------------*/
  if (((SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_ADDR) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_STOPF) != RESET) || (SMBUS_CHECK_FLAG(tmpisrvalue, SMBUS_FLAG_AF) != RESET))
     && ((__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ADDRI) != RESET) || (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_STOPI) != RESET) || (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_NACKI) != RESET)))
  {
    if (hsmbus->State == HAL_SMBUS_STATE_LISTEN)
    {
      SMBUS_Slave_ISR(hsmbus);
    }
  }
}

/**
  * @brief  Handle SMBUS error interrupt request.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
void HAL_SMBUS_ER_IRQHandler(SMBUS_HandleTypeDef *hsmbus)
{
  /* SMBUS Bus error interrupt occurred ------------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_BERR) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_BERR;
   
    /* Clear BERR flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_BERR);
  }
  
  /* SMBUS Over-Run/Under-Run interrupt occurred ----------------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_OVR) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_OVR;

    /* Clear OVR flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_OVR);
  }

  /* SMBUS Arbitration Loss error interrupt occurred ------------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_ARLO) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_ARLO);
  }

  /* SMBUS Timeout error interrupt occurred ---------------------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TIMEOUT) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_BUSTIMEOUT;

    /* Clear TIMEOUT flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_TIMEOUT);
  }

  /* SMBUS Alert error interrupt occurred -----------------------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_ALERT) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_ALERT;

    /* Clear ALERT flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_ALERT);
  }

  /* SMBUS Packet Error Check error interrupt occurred ----------------------------------*/
  if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_PECERR) != RESET) && (__HAL_SMBUS_GET_IT_SOURCE(hsmbus, SMBUS_IT_ERRI) != RESET))
  { 
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_PECERR;

    /* Clear PEC error flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_PECERR);
  }
  
  /* Call the Error Callback() in case of Error detected */
  if((hsmbus->ErrorCode != HAL_SMBUS_ERROR_NONE)&&(hsmbus->ErrorCode != HAL_SMBUS_ERROR_ACKF))
  {
    /* Do not Reset the HAL state in case of ALERT error */
    if((hsmbus->ErrorCode & HAL_SMBUS_ERROR_ALERT) != HAL_SMBUS_ERROR_ALERT)
    {
      if(((hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_TX) == HAL_SMBUS_STATE_SLAVE_BUSY_TX)
         || ((hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_RX) == HAL_SMBUS_STATE_SLAVE_BUSY_RX))
      {
        /* Reset only HAL_SMBUS_STATE_SLAVE_BUSY_XX */
        /* keep HAL_SMBUS_STATE_LISTEN if set */
        hsmbus->PreviousState = HAL_SMBUS_STATE_READY;
        hsmbus->State = HAL_SMBUS_STATE_LISTEN;
      }
    }
    
    /* Call the Error callback to prevent upper layer */
    HAL_SMBUS_ErrorCallback(hsmbus);
  }
}

/**
  * @brief  Master Tx Transfer completed callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_MasterTxCpltCallback() could be implemented in the user file
   */ 
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_MasterRxCpltCallback() could be implemented in the user file
   */
}

/** @brief  Slave Tx Transfer completed callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_SlaveTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_SlaveTxCpltCallback() could be implemented in the user file
   */ 
}

/**
  * @brief  Slave Rx Transfer completed callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_SlaveRxCpltCallback() could be implemented in the user file
   */
}

/**
  * @brief  Slave Address Match callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  TransferDirection: Master request Transfer Direction (Write/Read)
  * @param  AddrMatchCode: Address Match Code
  * @retval None
  */
__weak void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);
  UNUSED(TransferDirection);
  UNUSED(AddrMatchCode);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_AddrCallback() could be implemented in the user file
   */
}

/**
  * @brief  Listen Complete callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_ListenCpltCallback() could be implemented in the user file
   */
}

/**
  * @brief  SMBUS error callback.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
__weak void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmbus);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMBUS_ErrorCallback() could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SMBUS_Exported_Functions_Group3 Peripheral State and Errors functions 
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
  * @brief  Return the SMBUS handle state.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL state
  */
uint32_t HAL_SMBUS_GetState(SMBUS_HandleTypeDef *hsmbus)
{
  /* Return SMBUS handle state */
  return hsmbus->State;
}

/**
* @brief  Return the SMBUS error code.
* @param  hsmbus : pointer to a SMBUS_HandleTypeDef structure that contains
  *              the configuration information for the specified SMBUS.
* @retval SMBUS Error Code
*/
uint32_t HAL_SMBUS_GetError(SMBUS_HandleTypeDef *hsmbus)
{
  return hsmbus->ErrorCode;
}

/**
  * @}
  */  

/**
  * @}
  */  

/** @addtogroup SMBUS_Private_Functions SMBUS Private Functions
 *  @brief   Data transfers Private functions 
  * @{
  */

/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Master Mode.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_Master_ISR(SMBUS_HandleTypeDef *hsmbus) 
{
  uint16_t DevAddress;

  /* Process Locked */
  __HAL_LOCK(hsmbus);
  
  if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_AF) != RESET)
  {
    /* Clear NACK Flag */
    __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_AF);
    
    /* Set corresponding Error Code */
    /* No need to generate STOP, it is automatically done */
    hsmbus->ErrorCode |= HAL_SMBUS_ERROR_ACKF;

    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus);
    
    /* Call the Error callback to prevent upper layer */
    HAL_SMBUS_ErrorCallback(hsmbus);
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_STOPF) != RESET)
  {
      
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_TX)
    {
      /* Disable Interrupt */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_TX);

      /* Clear STOP Flag */
      __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);
      
      /* Clear Configuration Register 2 */
      SMBUS_RESET_CR2(hsmbus);
    
      /* Flush remaining data in Fifo register in case of error occurs before TXEmpty */
      /* Disable the selected SMBUS peripheral */
      __HAL_SMBUS_DISABLE(hsmbus);

      hsmbus->PreviousState = HAL_SMBUS_STATE_READY;
      hsmbus->State = HAL_SMBUS_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);
  
      /* REenable the selected SMBUS peripheral */
      __HAL_SMBUS_ENABLE(hsmbus);

      HAL_SMBUS_MasterTxCpltCallback(hsmbus);
    }
    else if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_RX)
    {
      /* Disable Interrupt */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX);

      /* Clear STOP Flag */
      __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);
      
      /* Clear Configuration Register 2 */
      SMBUS_RESET_CR2(hsmbus);
    
      hsmbus->PreviousState = HAL_SMBUS_STATE_READY;
      hsmbus->State = HAL_SMBUS_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);
  
      HAL_SMBUS_MasterRxCpltCallback(hsmbus);
    }
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_RXNE) != RESET)
  {  
    /* Read data from RXDR */
    (*hsmbus->pBuffPtr++) = hsmbus->Instance->RXDR;
    hsmbus->XferSize--;
    hsmbus->XferCount--;
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TXIS) != RESET)
  {
    /* Write data to TXDR */
    hsmbus->Instance->TXDR = (*hsmbus->pBuffPtr++);
    hsmbus->XferSize--;
    hsmbus->XferCount--;	
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TCR) != RESET)
  {
    if((hsmbus->XferSize == 0)&&(hsmbus->XferCount!=0))
    {
      DevAddress = (hsmbus->Instance->CR2 & I2C_CR2_SADD);
      
      if(hsmbus->XferCount > MAX_NBYTE_SIZE)
      {    
        SMBUS_TransferConfig(hsmbus, DevAddress, MAX_NBYTE_SIZE, (SMBUS_RELOAD_MODE | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE)), SMBUS_NO_STARTSTOP);
        hsmbus->XferSize = MAX_NBYTE_SIZE;
      }
      else
      {
        hsmbus->XferSize = hsmbus->XferCount;
        SMBUS_TransferConfig(hsmbus,DevAddress,hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
        /* If PEC mode is enable, size to transmit should be Size-1 byte, corresponding to PEC byte */
        /* PEC byte is automatically sent by HW block, no need to manage it in Transmit process */
        if(SMBUS_GET_PEC_MODE(hsmbus) != RESET)
        {
          hsmbus->XferSize--;
          hsmbus->XferCount--;
        }
      }
    }
    else if((hsmbus->XferSize == 0)&&(hsmbus->XferCount==0))
    {
      /* Call TxCpltCallback() if no stop mode is set */
      if(SMBUS_GET_STOP_MODE(hsmbus) != SMBUS_AUTOEND_MODE)
      {
        /* Call the corresponding callback to inform upper layer of End of Transfer */
        if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_TX)
        {
          /* Disable Interrupt */
          SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_TX);
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State = HAL_SMBUS_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
      
          HAL_SMBUS_MasterTxCpltCallback(hsmbus);
        }
        else if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_RX)
        {
          SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX);
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State = HAL_SMBUS_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
      
          HAL_SMBUS_MasterRxCpltCallback(hsmbus);
        }
      }
    }
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TC) != RESET)
  {
    if(hsmbus->XferCount == 0)
    {
      /* Specific use case for Quick command */
      if(hsmbus->pBuffPtr == NULL)
      {
        /* Generate a Stop command */
        hsmbus->Instance->CR2 |= I2C_CR2_STOP;
      }
      /* Call TxCpltCallback() if no stop mode is set */
      else if(SMBUS_GET_STOP_MODE(hsmbus) != SMBUS_AUTOEND_MODE)
      {
        /* No Generate Stop, to permit restart mode */
        /* The stop will be done at the end of transfer, when SMBUS_AUTOEND_MODE enable */
        
        /* Call the corresponding callback to inform upper layer of End of Transfer */
        if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_TX)
        {
          /* Disable Interrupt */
          SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_TX);
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State = HAL_SMBUS_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
      
          HAL_SMBUS_MasterTxCpltCallback(hsmbus);
        }
        else if(hsmbus->State == HAL_SMBUS_STATE_MASTER_BUSY_RX)
        {
          SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX);
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State = HAL_SMBUS_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
      
          HAL_SMBUS_MasterRxCpltCallback(hsmbus);
        }
      }
    }
  }
    
  /* Process Unlocked */
  __HAL_UNLOCK(hsmbus); 
  
  return HAL_OK; 
}  
/**
  * @brief  Interrupt Sub-Routine which handle the Interrupt Flags Slave Mode.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_Slave_ISR(SMBUS_HandleTypeDef *hsmbus) 
{
  uint8_t TransferDirection = 0;
  uint16_t SlaveAddrCode = 0;

  /* Process Locked */
  __HAL_LOCK(hsmbus);
  
  if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_AF) != RESET)
  {
    /* Check that SMBUS transfer finished */
    /* if yes, normal use case, a NACK is sent by the HOST when Transfer is finished */
    /* Mean XferCount == 0*/
    /* So clear Flag NACKF only */
    if(hsmbus->XferCount == 0)
    {
      /* Clear NACK Flag */
      __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_AF);

      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);
    }
    else
    {
      /* if no, error use case, a Non-Acknowledge of last Data is generated by the HOST*/
      /* Clear NACK Flag */
      __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_AF);

      /* Set HAL State to "Idle" State, mean to LISTEN state */
      /* So reset Slave Busy state */
      hsmbus->PreviousState = hsmbus->State;
      hsmbus->State &= ~((uint32_t)HAL_SMBUS_STATE_SLAVE_BUSY_TX);
      hsmbus->State &= ~((uint32_t)HAL_SMBUS_STATE_SLAVE_BUSY_RX);

      /* Disable RX/TX Interrupts, keep only ADDR Interrupt */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX | SMBUS_IT_TX);

      /* Set ErrorCode corresponding to a Non-Acknowledge */
      hsmbus->ErrorCode |= HAL_SMBUS_ERROR_ACKF;

      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);
    
      /* Call the Error callback to prevent upper layer */
      HAL_SMBUS_ErrorCallback(hsmbus);
    }
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_ADDR) != RESET)
  {
    TransferDirection = SMBUS_GET_DIR(hsmbus);
    SlaveAddrCode = SMBUS_GET_ADDR_MATCH(hsmbus);
      
    /* Disable ADDR interrupt to prevent multiple ADDRInterrupt*/
    /* Other ADDRInterrupt will be treat in next Listen use case */
    __HAL_SMBUS_DISABLE_IT(hsmbus, SMBUS_IT_ADDRI);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsmbus);

    /* Call Slave Addr callback */
    HAL_SMBUS_AddrCallback(hsmbus, TransferDirection, SlaveAddrCode);
  }
  else if((__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_RXNE) != RESET) || (__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TCR) != RESET))
  {
    if( (hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_RX) == HAL_SMBUS_STATE_SLAVE_BUSY_RX)
    {
      /* Read data from RXDR */
      (*hsmbus->pBuffPtr++) = hsmbus->Instance->RXDR;
      hsmbus->XferSize--;
      hsmbus->XferCount--;

      if(hsmbus->XferCount == 1)
      {
        /* Receive last Byte, can be PEC byte in case of PEC BYTE enabled */
        /* or only the last Byte of Transfer */
        /* So reset the RELOAD bit mode */
        hsmbus->XferOptions &= ~SMBUS_RELOAD_MODE;
        SMBUS_TransferConfig(hsmbus,0 ,1 , hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
      }
      else if(hsmbus->XferCount == 0)
      {
        /* Last Byte is received, disable Interrupt */
        SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX);
        
        /* Remove HAL_SMBUS_STATE_SLAVE_BUSY_RX, keep only HAL_SMBUS_STATE_LISTEN */
        hsmbus->PreviousState = hsmbus->State;
        hsmbus->State &= ~((uint32_t)HAL_SMBUS_STATE_SLAVE_BUSY_RX);
        
        /* Process Unlocked */
        __HAL_UNLOCK(hsmbus);

        /* Call the Rx complete callback to inform upper layer of the end of receive process */
        HAL_SMBUS_SlaveRxCpltCallback(hsmbus);
      }
      else
      {
        /* Set Reload for next Bytes */
        SMBUS_TransferConfig(hsmbus,0, 1, SMBUS_RELOAD_MODE  | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE), SMBUS_NO_STARTSTOP);

        /* Ack last Byte Read */
        hsmbus->Instance->CR2 &= ~I2C_CR2_NACK;
      }
    }    
    else if( (hsmbus->State & HAL_SMBUS_STATE_SLAVE_BUSY_TX) == HAL_SMBUS_STATE_SLAVE_BUSY_TX)
    {
      if((hsmbus->XferSize == 0)&&(hsmbus->XferCount!=0))
      {
        if(hsmbus->XferCount > MAX_NBYTE_SIZE)
        {    
          SMBUS_TransferConfig(hsmbus, 0, MAX_NBYTE_SIZE, (SMBUS_RELOAD_MODE | (hsmbus->XferOptions & SMBUS_SENDPEC_MODE)), SMBUS_NO_STARTSTOP);
          hsmbus->XferSize = MAX_NBYTE_SIZE;
        }
        else
        {
          hsmbus->XferSize = hsmbus->XferCount;
          SMBUS_TransferConfig(hsmbus, 0, hsmbus->XferSize, hsmbus->XferOptions, SMBUS_NO_STARTSTOP);
          /* If PEC mode is enable, size to transmit should be Size-1 byte, corresponding to PEC byte */
          /* PEC byte is automatically sent by HW block, no need to manage it in Transmit process */
          if(SMBUS_GET_PEC_MODE(hsmbus) != RESET)
          {
            hsmbus->XferSize--;
            hsmbus->XferCount--;
          }
        }
      }
    }
  }
  else if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_TXIS) != RESET)
  {
    /* Write data to TXDR only if XferCount not reach "0" */
    /* A TXIS flag can be set, during STOP treatment      */
    /* Check if all Data have already been sent */
    /* If it is the case, this last write in TXDR is not sent, correspond to a dummy TXIS event */
    if(hsmbus->XferCount > 0)
    {
      /* Write data to TXDR */
      hsmbus->Instance->TXDR = (*hsmbus->pBuffPtr++);
      hsmbus->XferCount--;
      hsmbus->XferSize--;
    }
    
    if(hsmbus->XferCount == 0)
    {
      /* Last Byte is Transmitted */
      /* Remove HAL_SMBUS_STATE_SLAVE_BUSY_TX, keep only HAL_SMBUS_STATE_LISTEN */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_TX);
      hsmbus->PreviousState = hsmbus->State;
      hsmbus->State &= ~((uint32_t)HAL_SMBUS_STATE_SLAVE_BUSY_TX);

      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);

      /* Call the Tx complete callback to inform upper layer of the end of transmit process */
      HAL_SMBUS_SlaveTxCpltCallback(hsmbus);
    }
  }

  /* Check if STOPF is set */
  if(__HAL_SMBUS_GET_FLAG(hsmbus, SMBUS_FLAG_STOPF) != RESET)
  {
    if((hsmbus->State & HAL_SMBUS_STATE_LISTEN) == HAL_SMBUS_STATE_LISTEN)
    {
      /* Disable RX and TX Interrupts */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_RX | SMBUS_IT_TX);

      /* Disable ADDR Interrupt */
      SMBUS_Disable_IRQ(hsmbus, SMBUS_IT_ADDR);

      /* Disable Address Acknowledge */
      hsmbus->Instance->CR2 |= I2C_CR2_NACK;

      /* Clear Configuration Register 2 */
      SMBUS_RESET_CR2(hsmbus);
    
      /* Clear STOP Flag */
      __HAL_SMBUS_CLEAR_FLAG(hsmbus, SMBUS_FLAG_STOPF);

     /* Clear ADDR flag */
     __HAL_SMBUS_CLEAR_FLAG(hsmbus,SMBUS_FLAG_ADDR);

      hsmbus->XferOptions = 0;
      hsmbus->PreviousState = hsmbus->State;
      hsmbus->State = HAL_SMBUS_STATE_READY;
    
      /* Process Unlocked */
      __HAL_UNLOCK(hsmbus);

      /* Call the Listen Complete callback, to prevent upper layer of the end of Listen use case */
      HAL_SMBUS_ListenCpltCallback(hsmbus);
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hsmbus);
  
  return HAL_OK;     
}  
/**
  * @brief  Manage the enabling of Interrupts.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  InterruptRequest : Value of @ref SMBUS_Interrupt_configuration_definition.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_Enable_IRQ(SMBUS_HandleTypeDef *hsmbus, uint16_t InterruptRequest) 
{
  uint32_t tmpisr = 0;

  if((InterruptRequest & SMBUS_IT_ALERT) == SMBUS_IT_ALERT)
  {
    /* Enable ERR interrupt */
    tmpisr |= SMBUS_IT_ERRI;
  }
  
  if((InterruptRequest & SMBUS_IT_ADDR) == SMBUS_IT_ADDR)
  {
    /* Enable ADDR, STOP interrupt */
    tmpisr |= SMBUS_IT_ADDRI | SMBUS_IT_STOPI | SMBUS_IT_NACKI | SMBUS_IT_ERRI;
  }
  
  if((InterruptRequest & SMBUS_IT_TX) == SMBUS_IT_TX)
  {
    /* Enable ERR, TC, STOP, NACK, RXI interrupt */
    tmpisr |= SMBUS_IT_ERRI | SMBUS_IT_TCI | SMBUS_IT_STOPI | SMBUS_IT_NACKI | SMBUS_IT_TXI;
  }
  
  if((InterruptRequest & SMBUS_IT_RX) == SMBUS_IT_RX)
  {
    /* Enable ERR, TC, STOP, NACK, TXI interrupt */
    tmpisr |= SMBUS_IT_ERRI | SMBUS_IT_TCI | SMBUS_IT_STOPI | SMBUS_IT_NACKI | SMBUS_IT_RXI;
  }
  
  /* Enable interrupts only at the end */
  /* to avoid the risk of SMBUS interrupt handle execution before */
  /* all interrupts requested done */
  __HAL_SMBUS_ENABLE_IT(hsmbus, tmpisr);

  return HAL_OK;     
}
/**
  * @brief  Manage the disabling of Interrupts.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  InterruptRequest : Value of @ref SMBUS_Interrupt_configuration_definition.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_Disable_IRQ(SMBUS_HandleTypeDef *hsmbus, uint16_t InterruptRequest) 
{
  uint32_t tmpisr = 0;

  if( ((InterruptRequest & SMBUS_IT_ALERT) == SMBUS_IT_ALERT) && (hsmbus->State == HAL_SMBUS_STATE_READY) )
  {
    /* Disable ERR interrupt */
    tmpisr |= SMBUS_IT_ERRI;
  }
  
  if((InterruptRequest & SMBUS_IT_TX) == SMBUS_IT_TX)
  {
    /* Disable TC, STOP, NACK, TXI interrupt */
    tmpisr |= SMBUS_IT_TCI | SMBUS_IT_TXI;
    
    if((SMBUS_GET_ALERT_ENABLED(hsmbus) == RESET)
       && ((hsmbus->State & HAL_SMBUS_STATE_LISTEN) != HAL_SMBUS_STATE_LISTEN))
    {
      /* Disable ERR interrupt */
      tmpisr |= SMBUS_IT_ERRI;
    }
    
    if((hsmbus->State & HAL_SMBUS_STATE_LISTEN) != HAL_SMBUS_STATE_LISTEN)
    {
      /* Disable STOPI, NACKI */
      tmpisr |= SMBUS_IT_STOPI | SMBUS_IT_NACKI;
    }
  }
  
  if((InterruptRequest & SMBUS_IT_RX) == SMBUS_IT_RX)
  {
    /* Disable TC, STOP, NACK, RXI interrupt */
    tmpisr |= SMBUS_IT_TCI | SMBUS_IT_RXI;
    
    if((SMBUS_GET_ALERT_ENABLED(hsmbus) == RESET)
       && ((hsmbus->State & HAL_SMBUS_STATE_LISTEN) != HAL_SMBUS_STATE_LISTEN))
    {
      /* Disable ERR interrupt */
      tmpisr |= SMBUS_IT_ERRI;
    }

    if((hsmbus->State & HAL_SMBUS_STATE_LISTEN) != HAL_SMBUS_STATE_LISTEN)
    {
      /* Disable STOPI, NACKI */
      tmpisr |= SMBUS_IT_STOPI | SMBUS_IT_NACKI;
    }
  }
  
  if((InterruptRequest & SMBUS_IT_ADDR) == SMBUS_IT_ADDR)
  {
    /* Enable ADDR, STOP interrupt */
    tmpisr |= SMBUS_IT_ADDRI | SMBUS_IT_STOPI | SMBUS_IT_NACKI;

    if(SMBUS_GET_ALERT_ENABLED(hsmbus) == RESET) 
    {
      /* Disable ERR interrupt */
      tmpisr |= SMBUS_IT_ERRI;
    }
  }

  /* Disable interrupts only at the end */
  /* to avoid a breaking situation like at "t" time */
  /* all disable interrupts request are not done */
  __HAL_SMBUS_DISABLE_IT(hsmbus, tmpisr);
  
  return HAL_OK;
}
/**
  * @brief  Handle SMBUS Communication Timeout.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @param  Flag: specifies the SMBUS flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnFlagUntilTimeout(SMBUS_HandleTypeDef *hsmbus, uint32_t Flag, FlagStatus Status, uint32_t Timeout)  
{  
  uint32_t tickstart = HAL_GetTick();
  
  /* Wait until flag is set */
  if(Status == RESET)
  {    
    while(__HAL_SMBUS_GET_FLAG(hsmbus, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State= HAL_SMBUS_STATE_READY;
        
          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
        
          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_SMBUS_GET_FLAG(hsmbus, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          hsmbus->PreviousState = hsmbus->State;
          hsmbus->State= HAL_SMBUS_STATE_READY;
        
          /* Process Unlocked */
          __HAL_UNLOCK(hsmbus);
        
          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;      
}

/**
  * @brief  Handle SMBUSx communication when starting transfer or during transfer (TC or TCR flag are set).
  * @param  hsmbus: SMBUS handle.
  * @param  DevAddress: specifies the slave address to be programmed.
  * @param  Size: specifies the number of bytes to be programmed.
  *   This parameter must be a value between 0 and 255.
  * @param  Mode: new state of the SMBUS START condition generation.
  *   This parameter can be one or a combination  of the following values:
  *     @arg SMBUS_NO_MODE: No specific mode enabled.
  *     @arg SMBUS_RELOAD_MODE: Enable Reload mode.
  *     @arg SMBUS_AUTOEND_MODE: Enable Automatic end mode.
  *     @arg SMBUS_SOFTEND_MODE: Enable Software end mode and Reload mode.
  * @param  Request: new state of the SMBUS START condition generation.
  *   This parameter can be one of the following values:
  *     @arg SMBUS_NO_STARTSTOP: Don't Generate stop and start condition.
  *     @arg SMBUS_GENERATE_STOP: Generate stop condition (Size should be set to 0).
  *     @arg SMBUS_GENERATE_START_READ: Generate Restart for read request.
  *     @arg SMBUS_GENERATE_START_WRITE: Generate Restart for write request.
  * @retval None
  */
static void SMBUS_TransferConfig(SMBUS_HandleTypeDef *hsmbus,  uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_SMBUS_ALL_INSTANCE(hsmbus->Instance));
  assert_param(IS_SMBUS_TRANSFER_MODE(Mode));
  assert_param(IS_SMBUS_TRANSFER_REQUEST(Request));
    
  /* Get the CR2 register value */
  tmpreg = hsmbus->Instance->CR2;
  
  /* clear tmpreg specific bits */
  tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP | I2C_CR2_PECBYTE));
  
  /* update tmpreg */
  tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES) | \
              (uint32_t)Mode | (uint32_t)Request);
    
  /* update CR2 register */
  hsmbus->Instance->CR2 = tmpreg;  
}  
/**
  * @}
  */

#endif /* HAL_SMBUS_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
