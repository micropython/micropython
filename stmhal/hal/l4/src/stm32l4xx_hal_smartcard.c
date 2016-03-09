/**
  ******************************************************************************
  * @file    stm32l4xx_hal_smartcard.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   SMARTCARD HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the SMARTCARD peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The SMARTCARD HAL driver can be used as follows:

    (#) Declare a SMARTCARD_HandleTypeDef handle structure (eg. SMARTCARD_HandleTypeDef hsmartcard).
    (#) Associate a USART to the SMARTCARD handle hsmartcard.
    (#) Initialize the SMARTCARD low level resources by implementing the HAL_SMARTCARD_MspInit() API:
        (++) Enable the USARTx interface clock.
        (++) USART pins configuration:
             (+++) Enable the clock for the USART GPIOs.
             (+++) Configure the USART pins (TX as alternate function pull-up, RX as alternate function Input).
        (++) NVIC configuration if you need to use interrupt process (HAL_SMARTCARD_Transmit_IT()
             and HAL_SMARTCARD_Receive_IT() APIs):
             (++) Configure the USARTx interrupt priority.
             (++) Enable the NVIC USART IRQ handle.
        (++) DMA Configuration if you need to use DMA process (HAL_SMARTCARD_Transmit_DMA()
             and HAL_SMARTCARD_Receive_DMA() APIs):
             (+++) Declare a DMA handle structure for the Tx/Rx channel.
             (+++) Enable the DMAx interface clock.
             (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
             (+++) Configure the DMA Tx/Rx channel.
             (+++) Associate the initialized DMA handle to the SMARTCARD DMA Tx/Rx handle.
             (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx/Rx channel.

    (#) Program the Baud Rate, Parity, Mode(Receiver/Transmitter), clock enabling/disabling and accordingly,
        the clock parameters (parity, phase, last bit), prescaler value, guard time and NACK on transmission
        error enabling or disabling in the hsmartcard handle Init structure.

    (#) If required, program SMARTCARD advanced features (TX/RX pins swap, TimeOut, auto-retry counter,...)
        in the hsmartcard handle AdvancedInit structure.

    (#) Initialize the SMARTCARD registers by calling the HAL_SMARTCARD_Init() API:
        (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
             by calling the customized HAL_SMARTCARD_MspInit() API.
        [..]
        (@) The specific SMARTCARD interrupts (Transmission complete interrupt,
             RXNE interrupt and Error Interrupts) will be managed using the macros
             __HAL_SMARTCARD_ENABLE_IT() and __HAL_SMARTCARD_DISABLE_IT() inside the transmit and receive process.

    [..]
    [..] Three operation modes are available within this driver :

     *** Polling mode IO operation ***
     =================================
     [..]
       (+) Send an amount of data in blocking mode using HAL_SMARTCARD_Transmit()
       (+) Receive an amount of data in blocking mode using HAL_SMARTCARD_Receive()

     *** Interrupt mode IO operation ***
     ===================================
     [..]
       (+) Send an amount of data in non-blocking mode using HAL_SMARTCARD_Transmit_IT()
       (+) At transmission end of transfer HAL_SMARTCARD_TxCpltCallback() is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_TxCpltCallback()
       (+) Receive an amount of data in non-blocking mode using HAL_SMARTCARD_Receive_IT()
       (+) At reception end of transfer HAL_SMARTCARD_RxCpltCallback() is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_RxCpltCallback()
       (+) In case of transfer Error, HAL_SMARTCARD_ErrorCallback() function is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_ErrorCallback()

     *** DMA mode IO operation ***
     ==============================
     [..]
       (+) Send an amount of data in non-blocking mode (DMA) using HAL_SMARTCARD_Transmit_DMA()
       (+) At transmission end of transfer HAL_SMARTCARD_TxCpltCallback() is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_TxCpltCallback()
       (+) Receive an amount of data in non-blocking mode (DMA) using HAL_SMARTCARD_Receive_DMA()
       (+) At reception end of transfer HAL_SMARTCARD_RxCpltCallback() is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_RxCpltCallback()
       (+) In case of transfer Error, HAL_SMARTCARD_ErrorCallback() function is executed and user can
            add his own code by customization of function pointer HAL_SMARTCARD_ErrorCallback()

     *** SMARTCARD HAL driver macros list ***
     ========================================
     [..]
       Below the list of most used macros in SMARTCARD HAL driver.

       (+) __HAL_SMARTCARD_GET_FLAG : Check whether or not the specified SMARTCARD flag is set
       (+) __HAL_SMARTCARD_CLEAR_FLAG : Clear the specified SMARTCARD pending flag
       (+) __HAL_SMARTCARD_ENABLE_IT: Enable the specified SMARTCARD interrupt
       (+) __HAL_SMARTCARD_DISABLE_IT: Disable the specified SMARTCARD interrupt
       (+) __HAL_SMARTCARD_GET_IT_SOURCE: Check whether or not the specified SMARTCARD interrupt is enabled

     [..]
       (@) You can refer to the SMARTCARD HAL driver header file for more useful macros

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

/** @defgroup SMARTCARD SMARTCARD
  * @brief HAL SMARTCARD module driver
  * @{
  */

#ifdef HAL_SMARTCARD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup SMARTCARD_Private_Constants SMARTCARD Private Constants
 * @{
 */
#define SMARTCARD_TEACK_REACK_TIMEOUT               1000      /*!< SMARTCARD TX or RX enable acknowledge time-out value  */

#define USART_CR1_FIELDS      ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                     USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8))       /*!< USART CR1 fields of parameters set by SMARTCARD_SetConfig API */
#define USART_CR2_CLK_FIELDS  ((uint32_t)(USART_CR2_CLKEN|USART_CR2_CPOL|USART_CR2_CPHA|USART_CR2_LBCL)) /*!< SMARTCARD clock-related USART CR2 fields of parameters */
#define USART_CR2_FIELDS      ((uint32_t)(USART_CR2_RTOEN|USART_CR2_CLK_FIELDS|USART_CR2_STOP)) /*!< USART CR2 fields of parameters set by SMARTCARD_SetConfig API */
#define USART_CR3_FIELDS      ((uint32_t)(USART_CR3_ONEBIT|USART_CR3_NACK|USART_CR3_SCARCNT))   /*!< USART CR3 fields of parameters set by SMARTCARD_SetConfig API */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SMARTCARD_Private_Functions
  * @{
  */
static void SMARTCARD_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SMARTCARD_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SMARTCARD_DMAError(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef SMARTCARD_SetConfig(SMARTCARD_HandleTypeDef *hsmartcard);
static void SMARTCARD_AdvFeatureConfig(SMARTCARD_HandleTypeDef *hsmartcard);
static HAL_StatusTypeDef SMARTCARD_WaitOnFlagUntilTimeout(SMARTCARD_HandleTypeDef *hsmartcard, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
static HAL_StatusTypeDef SMARTCARD_CheckIdleState(SMARTCARD_HandleTypeDef *hsmartcard);
static HAL_StatusTypeDef SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsmartcard);
static HAL_StatusTypeDef SMARTCARD_EndTransmit_IT(SMARTCARD_HandleTypeDef *hsmartcard);
static HAL_StatusTypeDef SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsmartcard);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup SMARTCARD_Exported_Functions SMARTCARD Exported Functions
  * @{
  */

/** @defgroup SMARTCARD_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
  ===============================================================================
              ##### Initialization and Configuration functions #####
  ===============================================================================
  [..]
  This subsection provides a set of functions allowing to initialize the USARTx
  associated to the SmartCard.
  (+) These parameters can be configured:
      (++) Baud Rate
      (++) Parity: parity should be enabled,
           Frame Length is fixed to 8 bits plus parity.
      (++) Receiver/transmitter modes
      (++) Synchronous mode (and if enabled, phase, polarity and last bit parameters)
      (++) Prescaler value
      (++) Guard bit time
      (++) NACK enabling or disabling on transmission error

  (+) The following advanced features can be configured as well:
      (++) TX and/or RX pin level inversion
      (++) data logical level inversion
      (++) RX and TX pins swap
      (++) RX overrun detection disabling
      (++) DMA disabling on RX error
      (++) MSB first on communication line
      (++) Time out enabling (and if activated, timeout value)
      (++) Block length
      (++) Auto-retry counter
  [..]
  The HAL_SMARTCARD_Init() API follows the USART synchronous configuration procedures
  (details for the procedures are available in reference manual).

@endverbatim

  The USART frame format is given in the following table:

    Table 1. USART frame format.
    +---------------------------------------------------------------+        
    | M1M0 bits |  PCE bit  |            USART frame                |        
    |-----------------------|---------------------------------------|        
    |     01    |    1      |    | SB | 8 bit data | PB | STB |     |        
    +---------------------------------------------------------------+        


  * @{
  */

/**
  * @brief Initialize the SMARTCARD mode according to the specified
  *         parameters in the SMARTCARD_HandleTypeDef and initialize the associated handle.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Check the SMARTCARD handle allocation */
  if(hsmartcard == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the USART associated to the SmartCard */
  assert_param(IS_SMARTCARD_INSTANCE(hsmartcard->Instance));

  if(hsmartcard->State == HAL_SMARTCARD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hsmartcard->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK */
    HAL_SMARTCARD_MspInit(hsmartcard);
  }

  hsmartcard->State = HAL_SMARTCARD_STATE_BUSY;

  /* Disable the Peripheral to set smartcard mode */
  CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

  /* In SmartCard mode, the following bits must be kept cleared:
  - LINEN in the USART_CR2 register,
  - HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(hsmartcard->Instance->CR2, USART_CR2_LINEN);
  CLEAR_BIT(hsmartcard->Instance->CR3, (USART_CR3_HDSEL | USART_CR3_IREN));

  /* set the USART in SMARTCARD mode */
  SET_BIT(hsmartcard->Instance->CR3, USART_CR3_SCEN);

  /* Set the SMARTCARD Communication parameters */
  if (SMARTCARD_SetConfig(hsmartcard) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  if (hsmartcard->AdvancedInit.AdvFeatureInit != SMARTCARD_ADVFEATURE_NO_INIT)
  {
    SMARTCARD_AdvFeatureConfig(hsmartcard);
  }

  /* Enable the Peripheral */
  SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

  /* TEACK and/or REACK to check before moving hsmartcard->State to Ready */
  return (SMARTCARD_CheckIdleState(hsmartcard));
}


/**
  * @brief DeInitialize the SMARTCARD peripheral.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Check the SMARTCARD handle allocation */
  if(hsmartcard == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SMARTCARD_INSTANCE(hsmartcard->Instance));

  hsmartcard->State = HAL_SMARTCARD_STATE_BUSY;

  /* Disable the Peripheral */
  CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

  WRITE_REG(hsmartcard->Instance->CR1, 0x0);
  WRITE_REG(hsmartcard->Instance->CR2, 0x0);
  WRITE_REG(hsmartcard->Instance->CR3, 0x0);
  WRITE_REG(hsmartcard->Instance->RTOR, 0x0);
  WRITE_REG(hsmartcard->Instance->GTPR, 0x0);

  /* DeInit the low level hardware */
  HAL_SMARTCARD_MspDeInit(hsmartcard);

  hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
  hsmartcard->State = HAL_SMARTCARD_STATE_RESET;

  /* Process Unlock */
  __HAL_UNLOCK(hsmartcard);

  return HAL_OK;
}

/**
  * @brief Initialize the SMARTCARD MSP.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmartcard);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_MspInit can be implemented in the user file
   */
}

/**
  * @brief DeInitialize the SMARTCARD MSP.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmartcard);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_MspDeInit can be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SMARTCARD_Exported_Functions_Group2 IO operation functions
  *  @brief   SMARTCARD Transmit and Receive functions
  *
@verbatim
  ==============================================================================
                         ##### IO operation functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to manage the SMARTCARD data transfers.

  [..]
    Smartcard is a single wire half duplex communication protocol.
    The Smartcard interface is designed to support asynchronous protocol Smartcards as
    defined in the ISO 7816-3 standard. The USART should be configured as:
    (+) 8 bits plus parity: where M=1 and PCE=1 in the USART_CR1 register
    (+) 1.5 stop bits when transmitting and receiving: where STOP=11 in the USART_CR2 register.

  [..]
    (+) There are two modes of transfer:
        (++) Blocking mode: The communication is performed in polling mode.
             The HAL status of all data processing is returned by the same function
             after finishing transfer.
        (++) No-Blocking mode: The communication is performed using Interrupts
             or DMA, the relevant API's return the HAL status.
             The end of the data processing will be indicated through the
             dedicated SMARTCARD IRQ when using Interrupt mode or the DMA IRQ when
             using DMA mode.
        (++) The HAL_SMARTCARD_TxCpltCallback(), HAL_SMARTCARD_RxCpltCallback() user callbacks
             will be executed respectively at the end of the Transmit or Receive process
             The HAL_SMARTCARD_ErrorCallback() user callback will be executed when a communication
             error is detected.

    (+) Blocking mode APIs are :
        (++) HAL_SMARTCARD_Transmit()
        (++) HAL_SMARTCARD_Receive()

    (+) Non Blocking mode APIs with Interrupt are :
        (++) HAL_SMARTCARD_Transmit_IT()
        (++) HAL_SMARTCARD_Receive_IT()
        (++) HAL_SMARTCARD_IRQHandler()

    (+) Non Blocking mode functions with DMA are :
        (++) HAL_SMARTCARD_Transmit_DMA()
        (++) HAL_SMARTCARD_Receive_DMA()

    (+) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_SMARTCARD_TxCpltCallback()
        (++) HAL_SMARTCARD_RxCpltCallback()
        (++) HAL_SMARTCARD_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief Send an amount of data in blocking mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be sent.
  * @param Timeout : Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a receive process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
    }

    /* Disable the Peripheral first to update mode for TX master */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    /* Disable Rx, enable Tx */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_RE);
    SET_BIT(hsmartcard->Instance->RQR, SMARTCARD_RXDATA_FLUSH_REQUEST);
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_TE);
    
    /* Enable the Peripheral */
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->TxXferSize = Size;
    hsmartcard->TxXferCount = Size;

    while(hsmartcard->TxXferCount > 0)
    {
      hsmartcard->TxXferCount--;
      if(SMARTCARD_WaitOnFlagUntilTimeout(hsmartcard, SMARTCARD_FLAG_TXE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      hsmartcard->Instance->TDR = (*pData++ & (uint8_t)0xFF);
    }
    if(SMARTCARD_WaitOnFlagUntilTimeout(hsmartcard, SMARTCARD_FLAG_TC, RESET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
    /* Re-enable Rx at end of transmission if initial mode is Rx/Tx */
    if(hsmartcard->Init.Mode == SMARTCARD_MODE_TX_RX)
    {
      /* Disable the Peripheral first to update modes */
      CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);
      SET_BIT(hsmartcard->Instance->CR1, USART_CR1_RE);
      /* Enable the Peripheral */
      SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);
    }
    
    /* Check if a receive process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in blocking mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be received.
  * @param Timeout : Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a non-blocking transmit process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_RX;
    }

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->RxXferSize = Size;
    hsmartcard->RxXferCount = Size;

    /* Check the remain data to be received */
    while(hsmartcard->RxXferCount > 0)
    {
      hsmartcard->RxXferCount--;
      if(SMARTCARD_WaitOnFlagUntilTimeout(hsmartcard, SMARTCARD_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      *pData++ = (uint8_t)(hsmartcard->Instance->RDR & (uint8_t)0x00FF);
    }

    /* Check if a non-blocking transmit process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
    }
    else
    {
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Send an amount of data in interrupt mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a receive process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
    }

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->pTxBuffPtr = pData;
    hsmartcard->TxXferSize = Size;
    hsmartcard->TxXferCount = Size;

    /* Disable the Peripheral first to update mode for TX master */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    /* Disable Rx, enable Tx */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_RE);
    SET_BIT(hsmartcard->Instance->RQR, SMARTCARD_RXDATA_FLUSH_REQUEST);
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_TE);
    
    /* Enable the Peripheral */
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    /* Enable the SMARTCARD Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    /* Enable the SMARTCARD Transmit Data Register Empty Interrupt */
    __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in interrupt mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a transmit process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_RX;
    }

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->pRxBuffPtr = pData;
    hsmartcard->RxXferSize = Size;
    hsmartcard->RxXferCount = Size;

    /* Enable the SMARTCARD Parity Error Interrupt */
    __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_PE);

    /* Enable the SMARTCARD Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    /* Enable the SMARTCARD Data Register not empty Interrupt */
    __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_RXNE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Send an amount of data in DMA mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_DMA(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a receive process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
    }

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->pTxBuffPtr = pData;
    hsmartcard->TxXferSize = Size;
    hsmartcard->TxXferCount = Size;

    /* Disable the Peripheral first to update mode for TX master */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    /* Disable Rx, enable Tx */
    CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_RE);
    SET_BIT(hsmartcard->Instance->RQR, SMARTCARD_RXDATA_FLUSH_REQUEST);
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_TE);
    
    /* Enable the Peripheral */
    SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);

    /* Set the SMARTCARD DMA transfer complete callback */
    hsmartcard->hdmatx->XferCpltCallback = SMARTCARD_DMATransmitCplt;

    /* Set the SMARTCARD error callback */
    hsmartcard->hdmatx->XferErrorCallback = SMARTCARD_DMAError;

    /* Enable the SMARTCARD transmit DMA channel */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hsmartcard->hdmatx, *(uint32_t*)tmp, (uint32_t)&hsmartcard->Instance->TDR, Size);

    /* Clear the TC flag in the ICR register */
    __HAL_SMARTCARD_CLEAR_FLAG(hsmartcard, SMARTCARD_CLEAR_TCF);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the SMARTCARD associated USART CR3 register */
    SET_BIT(hsmartcard->Instance->CR3, USART_CR3_DMAT);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in DMA mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param pData: pointer to data buffer.
  * @param Size: amount of data to be received.
  * @note   The SMARTCARD-associated USART parity is enabled (PCE = 1),
  *         the received data contain the parity bit (MSB position).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Receive_DMA(SMARTCARD_HandleTypeDef *hsmartcard, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

  if ((hsmartcard->State == HAL_SMARTCARD_STATE_READY) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX))
  {
    if((pData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hsmartcard);

    /* Check if a transmit process is ongoing or not */
    if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX)
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX_RX;
    }
    else
    {
      hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_RX;
    }

    hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsmartcard->pRxBuffPtr = pData;
    hsmartcard->RxXferSize = Size;

    /* Set the SMARTCARD DMA transfer complete callback */
    hsmartcard->hdmarx->XferCpltCallback = SMARTCARD_DMAReceiveCplt;

    /* Set the SMARTCARD DMA error callback */
    hsmartcard->hdmarx->XferErrorCallback = SMARTCARD_DMAError;

    /* Enable the DMA channel */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hsmartcard->hdmarx, (uint32_t)&hsmartcard->Instance->RDR, *(uint32_t*)tmp, Size);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit
       in the SMARTCARD associated USART CR3 register */
    SET_BIT(hsmartcard->Instance->CR3, USART_CR3_DMAR);

    /* Process Unlocked */
    __HAL_UNLOCK(hsmartcard);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Handle SMARTCARD interrupt requests.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* SMARTCARD parity error interrupt occurred -------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_PE) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_PE) != RESET))
  {
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_PEF);
    hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_PE;
    /* Set the SMARTCARD state ready to be able to start again the process */
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  /* SMARTCARD frame error interrupt occurred --------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_FE) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_ERR) != RESET))
  {
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_FEF);
    hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_FE;
    /* Set the SMARTCARD state ready to be able to start again the process */
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  /* SMARTCARD noise error interrupt occurred --------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_NE) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_ERR) != RESET))
  {
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_NEF);
    hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_NE;
    /* Set the SMARTCARD state ready to be able to start again the process */
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  /* SMARTCARD Over-Run interrupt occurred -----------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_ORE) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_ERR) != RESET))
  {
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_OREF);
    hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_ORE;
    /* Set the SMARTCARD state ready to be able to start again the process */
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  /* SMARTCARD receiver timeout interrupt occurred -----------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_RTO) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_RTO) != RESET))
  {
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_RTOF);
    hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_RTO;
    /* Set the SMARTCARD state ready to be able to start again the process */
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  /* Call SMARTCARD Error Call back function if need be --------------------------*/
  if(hsmartcard->ErrorCode != HAL_SMARTCARD_ERROR_NONE)
  {
    HAL_SMARTCARD_ErrorCallback(hsmartcard);
  }

  /* SMARTCARD in mode Receiver ---------------------------------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_RXNE) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_RXNE) != RESET))
  {
    SMARTCARD_Receive_IT(hsmartcard);
    /* Clear RXNE interrupt flag done by reading RDR in SMARTCARD_Receive_IT() */
  }

  /* SMARTCARD in mode Receiver, end of block interruption ------------------------*/
  if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_EOB) != RESET) && (__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_EOB) != RESET))
  {
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
    __HAL_UNLOCK(hsmartcard);
    HAL_SMARTCARD_RxCpltCallback(hsmartcard);
    /* Clear EOBF interrupt after HAL_SMARTCARD_RxCpltCallback() call for the End of Block information
     * to be available during HAL_SMARTCARD_RxCpltCallback() processing */
    __HAL_SMARTCARD_CLEAR_IT(hsmartcard, SMARTCARD_CLEAR_EOBF);
  }

  /* SMARTCARD in mode Transmitter ------------------------------------------------*/
 if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_TXE) != RESET) &&(__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_TXE) != RESET))
  {
    SMARTCARD_Transmit_IT(hsmartcard);
  }

  /* SMARTCARD in mode Transmitter (transmission end) ------------------------*/
 if((__HAL_SMARTCARD_GET_IT(hsmartcard, SMARTCARD_IT_TC) != RESET) &&(__HAL_SMARTCARD_GET_IT_SOURCE(hsmartcard, SMARTCARD_IT_TC) != RESET))
  {
    SMARTCARD_EndTransmit_IT(hsmartcard);
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmartcard);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_TxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief Rx Transfer completed callback.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmartcard);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_RxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief SMARTCARD error callback.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsmartcard);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_ErrorCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup SMARTCARD_Exported_Functions_Group4 Peripheral State and Errors functions
  *  @brief   SMARTCARD State and Errors functions
  *
@verbatim
  ==============================================================================
                  ##### Peripheral State and Errors functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to return the State of SmartCard
    handle and also return Peripheral Errors occurred during communication process
     (+) HAL_SMARTCARD_GetState() API can be helpful to check in run-time the state
         of the SMARTCARD peripheral.
     (+) HAL_SMARTCARD_GetError() checks in run-time errors that could occur during
         communication.

@endverbatim
  * @{
  */


/**
  * @brief  Return the SMARTCARD handle state.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval SMARTCARD handle state
  */
HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(SMARTCARD_HandleTypeDef *hsmartcard)
{
  return hsmartcard->State;
}

/**
  * @brief  Return the SMARTCARD handle error code.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
* @retval SMARTCARD handle Error Code
*/
uint32_t HAL_SMARTCARD_GetError(SMARTCARD_HandleTypeDef *hsmartcard)
{
  return hsmartcard->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SMARTCARD_Private_Functions   SMARTCARD Private Functions
  * @{
  */

/**
  * @brief Send an amount of data in non-blocking mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for the specified SMARTCARD module.
  *         Function called under interruption only, once
  *         interruptions have been enabled by HAL_SMARTCARD_Transmit_IT()
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsmartcard)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX))
  {

    if(hsmartcard->TxXferCount == 0)
    {
      /* Disable the SMARTCARD Transmit Data Register Empty Interrupt */
      __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_TXE);

      /* Enable the SMARTCARD Transmit Complete Interrupt */
      __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_TC);

      return HAL_OK;
    }
    else
    {
      hsmartcard->Instance->TDR = (*hsmartcard->pTxBuffPtr++ & (uint8_t)0xFF);
      hsmartcard->TxXferCount--;

      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Wrap up transmission in non-blocking mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for the specified SMARTCARD module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_EndTransmit_IT(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Disable the SMARTCARD Transmit Complete Interrupt */
  __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_TC);

  /* Check if a receive process is ongoing or not */
  if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX)
  {
    hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_RX;

    /* Re-enable Rx at end of transmission if initial mode is Rx/Tx */
    if(hsmartcard->Init.Mode == SMARTCARD_MODE_TX_RX)
    {
      /* Disable the Peripheral first to update modes */
      CLEAR_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);
      SET_BIT(hsmartcard->Instance->CR1, USART_CR1_RE);
      /* Enable the Peripheral */
      SET_BIT(hsmartcard->Instance->CR1, USART_CR1_UE);
    }
  }
  else
  {
    /* Disable the SMARTCARD Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  HAL_SMARTCARD_TxCpltCallback(hsmartcard);

  return HAL_OK;
}


/**
  * @brief Receive an amount of data in non-blocking mode.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for the specified SMARTCARD module.
  *         Function called under interruption only, once
  *         interruptions have been enabled by HAL_SMARTCARD_Receive_IT().
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsmartcard)
{
  if ((hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_RX) || (hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX))
  {

    *hsmartcard->pRxBuffPtr++ = (uint8_t)(hsmartcard->Instance->RDR & (uint8_t)0xFF);

    if(--hsmartcard->RxXferCount == 0)
    {
      __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_RXNE);

      /* Check if a transmit process is ongoing or not */
      if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX)
      {
        hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
      }
      else
      {
        /* Disable the SMARTCARD Parity Error Interrupt */
        __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_PE);

        /* Disable the SMARTCARD Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

        hsmartcard->State = HAL_SMARTCARD_STATE_READY;
      }

      HAL_SMARTCARD_RxCpltCallback(hsmartcard);

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
  * @brief  Handle SMARTCARD Communication Timeout.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @param  Flag: specifies the SMARTCARD flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Timeout: Timeout duration.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_WaitOnFlagUntilTimeout(SMARTCARD_HandleTypeDef *hsmartcard, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_SMARTCARD_GET_FLAG(hsmartcard, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_TXE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_RXNE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_PE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

          hsmartcard->State= HAL_SMARTCARD_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmartcard);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_SMARTCARD_GET_FLAG(hsmartcard, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
        {
          /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_TXE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_RXNE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_PE);
          __HAL_SMARTCARD_DISABLE_IT(hsmartcard, SMARTCARD_IT_ERR);

          hsmartcard->State= HAL_SMARTCARD_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hsmartcard);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief DMA SMARTCARD transmit process complete callback.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SMARTCARD_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  SMARTCARD_HandleTypeDef* hsmartcard = ( SMARTCARD_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hsmartcard->TxXferCount = 0;

  /* Disable the DMA transfer for transmit request by resetting the DMAT bit
  in the SMARTCARD associated USART CR3 register */
  CLEAR_BIT(hsmartcard->Instance->CR3, USART_CR3_DMAT);

  /* Enable the SMARTCARD Transmit Complete Interrupt */
  __HAL_SMARTCARD_ENABLE_IT(hsmartcard, SMARTCARD_IT_TC);
}

/**
  * @brief DMA SMARTCARD receive process complete callback.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SMARTCARD_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SMARTCARD_HandleTypeDef* hsmartcard = ( SMARTCARD_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hsmartcard->RxXferCount = 0;

  /* Disable the DMA transfer for the receiver request by resetting the DMAR bit
     in the SMARTCARD associated USART CR3 register */
  CLEAR_BIT(hsmartcard->Instance->CR3, USART_CR3_DMAR);

  /* Check if a transmit process is ongoing or not */
  if(hsmartcard->State == HAL_SMARTCARD_STATE_BUSY_TX_RX)
  {
    hsmartcard->State = HAL_SMARTCARD_STATE_BUSY_TX;
  }
  else
  {
    hsmartcard->State = HAL_SMARTCARD_STATE_READY;
  }

  HAL_SMARTCARD_RxCpltCallback(hsmartcard);
}

/**
  * @brief DMA SMARTCARD communication error callback.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SMARTCARD_DMAError(DMA_HandleTypeDef *hdma)
{
  SMARTCARD_HandleTypeDef* hsmartcard = ( SMARTCARD_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hsmartcard->RxXferCount = 0;
  hsmartcard->TxXferCount = 0;
  hsmartcard->State= HAL_SMARTCARD_STATE_READY;
  hsmartcard->ErrorCode |= HAL_SMARTCARD_ERROR_DMA;
  HAL_SMARTCARD_ErrorCallback(hsmartcard);
}

/**
  * @brief Configure the SMARTCARD associated USART peripheral.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
static HAL_StatusTypeDef SMARTCARD_SetConfig(SMARTCARD_HandleTypeDef *hsmartcard)
{
  uint32_t tmpreg                          = 0x00000000;
  SMARTCARD_ClockSourceTypeDef clocksource = SMARTCARD_CLOCKSOURCE_UNDEFINED;
  HAL_StatusTypeDef ret                    = HAL_OK;

  /* Check the parameters */
  assert_param(IS_SMARTCARD_INSTANCE(hsmartcard->Instance));
  assert_param(IS_SMARTCARD_BAUDRATE(hsmartcard->Init.BaudRate));
  assert_param(IS_SMARTCARD_WORD_LENGTH(hsmartcard->Init.WordLength));
  assert_param(IS_SMARTCARD_STOPBITS(hsmartcard->Init.StopBits));
  assert_param(IS_SMARTCARD_PARITY(hsmartcard->Init.Parity));
  assert_param(IS_SMARTCARD_MODE(hsmartcard->Init.Mode));
  assert_param(IS_SMARTCARD_POLARITY(hsmartcard->Init.CLKPolarity));
  assert_param(IS_SMARTCARD_PHASE(hsmartcard->Init.CLKPhase));
  assert_param(IS_SMARTCARD_LASTBIT(hsmartcard->Init.CLKLastBit));
  assert_param(IS_SMARTCARD_ONE_BIT_SAMPLE(hsmartcard->Init.OneBitSampling));
  assert_param(IS_SMARTCARD_NACK(hsmartcard->Init.NACKEnable));
  assert_param(IS_SMARTCARD_TIMEOUT(hsmartcard->Init.TimeOutEnable));
  assert_param(IS_SMARTCARD_AUTORETRY_COUNT(hsmartcard->Init.AutoRetryCount));

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* In SmartCard mode, M and PCE are forced to 1 (8 bits + parity).
   * Oversampling is forced to 16 (OVER8 = 0).
   * Configure the Parity and Mode:
   *  set PS bit according to hsmartcard->Init.Parity value
   *  set TE and RE bits according to hsmartcard->Init.Mode value */
  tmpreg = (uint32_t) hsmartcard->Init.Parity | hsmartcard->Init.Mode;
  tmpreg |= (uint32_t) hsmartcard->Init.WordLength;
  MODIFY_REG(hsmartcard->Instance->CR1, USART_CR1_FIELDS, tmpreg);

  /*-------------------------- USART CR2 Configuration -----------------------*/
  tmpreg = hsmartcard->Init.StopBits;
  /* Synchronous mode is activated by default */
  tmpreg |= (uint32_t) USART_CR2_CLKEN | hsmartcard->Init.CLKPolarity;
  tmpreg |= (uint32_t) hsmartcard->Init.CLKPhase | hsmartcard->Init.CLKLastBit;
  tmpreg |= (uint32_t) hsmartcard->Init.TimeOutEnable;
  MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_FIELDS, tmpreg);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure
   * - one-bit sampling method versus three samples' majority rule
   *   according to hsmartcard->Init.OneBitSampling
   * - NACK transmission in case of parity error according
   *   to hsmartcard->Init.NACKEnable
   * - autoretry counter according to hsmartcard->Init.AutoRetryCount     */
  tmpreg =  (uint32_t) hsmartcard->Init.OneBitSampling | hsmartcard->Init.NACKEnable;
  tmpreg |= ((uint32_t)hsmartcard->Init.AutoRetryCount << SMARTCARD_CR3_SCARCNT_LSB_POS);
  MODIFY_REG(hsmartcard->Instance-> CR3,USART_CR3_FIELDS, tmpreg);

  /*-------------------------- USART GTPR Configuration ----------------------*/
  tmpreg = (hsmartcard->Init.Prescaler | ((uint32_t)hsmartcard->Init.GuardTime << SMARTCARD_GTPR_GT_LSB_POS));
  MODIFY_REG(hsmartcard->Instance->GTPR, (USART_GTPR_GT|USART_GTPR_PSC), tmpreg);

  /*-------------------------- USART RTOR Configuration ----------------------*/
  tmpreg =   ((uint32_t)hsmartcard->Init.BlockLength << SMARTCARD_RTOR_BLEN_LSB_POS);
  if (hsmartcard->Init.TimeOutEnable == SMARTCARD_TIMEOUT_ENABLE)
  {
    assert_param(IS_SMARTCARD_TIMEOUT_VALUE(hsmartcard->Init.TimeOutValue));
    tmpreg |=  (uint32_t) hsmartcard->Init.TimeOutValue;
  }
  MODIFY_REG(hsmartcard->Instance->RTOR, (USART_RTOR_RTO|USART_RTOR_BLEN), tmpreg);

  /*-------------------------- USART BRR Configuration -----------------------*/
  SMARTCARD_GETCLOCKSOURCE(hsmartcard, clocksource);
  switch (clocksource)
  {
    case SMARTCARD_CLOCKSOURCE_PCLK1:
      hsmartcard->Instance->BRR = (uint16_t)(HAL_RCC_GetPCLK1Freq() / hsmartcard->Init.BaudRate);
      break;
    case SMARTCARD_CLOCKSOURCE_PCLK2:
      hsmartcard->Instance->BRR = (uint16_t)(HAL_RCC_GetPCLK2Freq() / hsmartcard->Init.BaudRate);
      break;
    case SMARTCARD_CLOCKSOURCE_HSI:
      hsmartcard->Instance->BRR = (uint16_t)(HSI_VALUE / hsmartcard->Init.BaudRate);
      break;
    case SMARTCARD_CLOCKSOURCE_SYSCLK:
      hsmartcard->Instance->BRR = (uint16_t)(HAL_RCC_GetSysClockFreq() / hsmartcard->Init.BaudRate);
      break;
    case SMARTCARD_CLOCKSOURCE_LSE:
      hsmartcard->Instance->BRR = (uint16_t)(LSE_VALUE / hsmartcard->Init.BaudRate);
      break;
    case SMARTCARD_CLOCKSOURCE_UNDEFINED:
    default:
      ret = HAL_ERROR;
      break;
  }

  return ret;
}


/**
  * @brief Configure the SMARTCARD associated USART peripheral advanced features.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval None
  */
static void SMARTCARD_AdvFeatureConfig(SMARTCARD_HandleTypeDef *hsmartcard)
{
  /* Check whether the set of advanced features to configure is properly set */
  assert_param(IS_SMARTCARD_ADVFEATURE_INIT(hsmartcard->AdvancedInit.AdvFeatureInit));

  /* if required, configure TX pin active level inversion */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_TXINVERT_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_TXINV(hsmartcard->AdvancedInit.TxPinLevelInvert));
    MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_TXINV, hsmartcard->AdvancedInit.TxPinLevelInvert);
  }

  /* if required, configure RX pin active level inversion */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_RXINVERT_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_RXINV(hsmartcard->AdvancedInit.RxPinLevelInvert));
    MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_RXINV, hsmartcard->AdvancedInit.RxPinLevelInvert);
  }

  /* if required, configure data inversion */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_DATAINVERT_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_DATAINV(hsmartcard->AdvancedInit.DataInvert));
    MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_DATAINV, hsmartcard->AdvancedInit.DataInvert);
  }

  /* if required, configure RX/TX pins swap */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_SWAP_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_SWAP(hsmartcard->AdvancedInit.Swap));
    MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_SWAP, hsmartcard->AdvancedInit.Swap);
  }

  /* if required, configure RX overrun detection disabling */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_RXOVERRUNDISABLE_INIT))
  {
    assert_param(IS_SMARTCARD_OVERRUN(hsmartcard->AdvancedInit.OverrunDisable));
    MODIFY_REG(hsmartcard->Instance->CR3, USART_CR3_OVRDIS, hsmartcard->AdvancedInit.OverrunDisable);
  }

  /* if required, configure DMA disabling on reception error */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_DMADISABLEONERROR_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_DMAONRXERROR(hsmartcard->AdvancedInit.DMADisableonRxError));
    MODIFY_REG(hsmartcard->Instance->CR3, USART_CR3_DDRE, hsmartcard->AdvancedInit.DMADisableonRxError);
  }

  /* if required, configure MSB first on communication line */
  if (HAL_IS_BIT_SET(hsmartcard->AdvancedInit.AdvFeatureInit, SMARTCARD_ADVFEATURE_MSBFIRST_INIT))
  {
    assert_param(IS_SMARTCARD_ADVFEATURE_MSBFIRST(hsmartcard->AdvancedInit.MSBFirst));
    MODIFY_REG(hsmartcard->Instance->CR2, USART_CR2_MSBFIRST, hsmartcard->AdvancedInit.MSBFirst);
  }

}

/**
  * @brief Check the SMARTCARD Idle State.
  * @param hsmartcard: Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                    the configuration information for the specified SMARTCARD module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_CheckIdleState(SMARTCARD_HandleTypeDef *hsmartcard)
{

  /* Initialize the SMARTCARD ErrorCode */
  hsmartcard->ErrorCode = HAL_SMARTCARD_ERROR_NONE;

  /* Check if the Transmitter is enabled */
  if((hsmartcard->Instance->CR1 & USART_CR1_TE) == USART_CR1_TE)
  {
    /* Wait until TEACK flag is set */
    if(SMARTCARD_WaitOnFlagUntilTimeout(hsmartcard, USART_ISR_TEACK, RESET, SMARTCARD_TEACK_REACK_TIMEOUT) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  }
  /* Check if the Receiver is enabled */
  if((hsmartcard->Instance->CR1 & USART_CR1_RE) == USART_CR1_RE)
  {
    /* Wait until REACK flag is set */
    if(SMARTCARD_WaitOnFlagUntilTimeout(hsmartcard, USART_ISR_REACK, RESET, SMARTCARD_TEACK_REACK_TIMEOUT) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Initialize the SMARTCARD state*/
  hsmartcard->State= HAL_SMARTCARD_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hsmartcard);

  return HAL_OK;
}

/**
  * @}
  */

#endif /* HAL_SMARTCARD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
