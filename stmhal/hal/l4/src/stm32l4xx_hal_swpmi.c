/**
  ******************************************************************************
  * @file    stm32l4xx_hal_swpmi.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   SWPMI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Single Wire Protocol Master Interface (SWPMI).
  *           + Initialization and Configuration
  *           + Data transfers functions
  *           + DMA transfers management
  *           + Interrupts and flags management
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  [..]
     The SWPMI HAL driver can be used as follows:

    (#) Declare a SWPMI_HandleTypeDef handle structure (eg. SWPMI_HandleTypeDef hswpmi).

    (#) Initialize the SWPMI low level resources by implementing the HAL_SWPMI_MspInit() API:
        (##) Enable the SWPMIx interface clock with __HAL_RCC_SWPMIx_CLK_ENABLE().
        (##) SWPMI IO configuration:
            (+++) Enable the clock for the SWPMI GPIO.
            (+++) Configure these SWPMI pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_SWPMI_Transmit_IT()
             and HAL_SWPMI_Receive_IT() APIs):
            (+++) Configure the SWPMIx interrupt priority with HAL_NVIC_SetPriority().
            (+++) Enable the NVIC SWPMI IRQ handle with HAL_NVIC_EnableIRQ().

        (##) DMA Configuration if you need to use DMA process (HAL_SWPMI_Transmit_DMA()
             and HAL_SWPMI_Receive_DMA() APIs):
            (+++) Declare a DMA handle structure for the Tx/Rx channels.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required
                  Tx/Rx parameters.
            (+++) Configure the DMA Tx/Rx channels and requests.
            (+++) Associate the initialized DMA handle to the SWPMI DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete
                  interrupt on the DMA Tx/Rx channels.

    (#) Program the Bite Rate, Tx Buffering mode, Rx Buffering mode in the Init structure.

    (#) Enable the SWPMI peripheral by calling the HAL_SWPMI_Init() function.
    
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

/** @defgroup SWPMI SWPMI
  * @brief HAL SWPMI module driver
  * @{
  */
#ifdef HAL_SWPMI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup SWPMI_Private_Constants SWPMI Private Constants
  * @{
  */
#define SWPMI_TIMEOUT_VALUE       ((uint32_t) 22000)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SWPMI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SWPMI_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void SWPMI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SWPMI_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void SWPMI_DMAError(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef SWPMI_Transmit_IT(SWPMI_HandleTypeDef *hswpmi);
static HAL_StatusTypeDef SWPMI_EndTransmit_IT(SWPMI_HandleTypeDef *hswpmi);
static HAL_StatusTypeDef SWPMI_Receive_IT(SWPMI_HandleTypeDef *hswpmi);
static HAL_StatusTypeDef SWPMI_EndReceive_IT(SWPMI_HandleTypeDef *hswpmi);
static HAL_StatusTypeDef SWPMI_EndTransmitReceive_IT(SWPMI_HandleTypeDef *hswpmi);
static HAL_StatusTypeDef SWPMI_WaitOnFlagSetUntilTimeout(SWPMI_HandleTypeDef *hswpmi, uint32_t Flag, uint32_t Timeout);

/* Exported functions --------------------------------------------------------*/

/** @defgroup SWPMI_Exported_Functions SWPMI Exported Functions
  * @{
  */

/** @defgroup SWPMI_Exported_Group1 Initialization/de-initialization methods
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the SWPMI peripheral.
      (+) De-initialize the SWPMI peripheral.

@endverbatim
  * @{
  */

/**
  * @brief Initialize the SWPMI peripheral according to the specified parameters in the SWPMI_InitTypeDef.
  * @param hswpmi: SWPMI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Init(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the SWPMI handle allocation */
  if(hswpmi == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_SWPMI_VOLTAGE_CLASS(hswpmi->Init.VoltageClass));
    assert_param(IS_SWPMI_BITRATE_VALUE(hswpmi->Init.BitRate));
    assert_param(IS_SWPMI_TX_BUFFERING_MODE(hswpmi->Init.TxBufferingMode));
    assert_param(IS_SWPMI_RX_BUFFERING_MODE(hswpmi->Init.RxBufferingMode));

    if(hswpmi->State == HAL_SWPMI_STATE_RESET)
    {
      /* Allocate lock resource and initialize it */
      hswpmi->Lock = HAL_UNLOCKED;

      /* Init the low level hardware : GPIO, CLOCK, CORTEX */
      HAL_SWPMI_MspInit(hswpmi);
    }

    hswpmi->State = HAL_SWPMI_STATE_BUSY;

    /* Disable SWPMI interface */
    CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);

    /* Clear all SWPMI interface flags */
    WRITE_REG(hswpmi->Instance->ICR, 0x019F);

    /* Apply Voltage class selection */
    MODIFY_REG(hswpmi->Instance->OR, SWPMI_OR_CLASS, hswpmi->Init.VoltageClass);

    /* Configure the BRR register (Bitrate) */
    WRITE_REG(hswpmi->Instance->BRR, hswpmi->Init.BitRate);

    /* Apply SWPMI CR configuration */
    MODIFY_REG(hswpmi->Instance->CR, \
               SWPMI_CR_RXDMA | SWPMI_CR_TXDMA  | SWPMI_CR_RXMODE | SWPMI_CR_TXMODE, \
               hswpmi->Init.TxBufferingMode | hswpmi->Init.RxBufferingMode);

    hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;
    hswpmi->State = HAL_SWPMI_STATE_READY;

    /* Enable SWPMI peripheral if not */
    SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
  }

  return status;
}

/**
  * @brief De-initialize the SWPMI peripheral.
  * @param hswpmi: SWPMI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_DeInit(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the SWPMI handle allocation */
  if(hswpmi == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_SWPMI_INSTANCE(hswpmi->Instance));

    hswpmi->State = HAL_SWPMI_STATE_BUSY;

    /* Disable SWPMI interface */
    CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);

    /* DeInit the low level hardware */
    HAL_SWPMI_MspDeInit(hswpmi);

    hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;

    hswpmi->State = HAL_SWPMI_STATE_RESET;

    /* Release Lock */
    __HAL_UNLOCK(hswpmi);
  }

  return status;
}

/**
  * @brief Initialize the SWPMI MSP.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_MspInit(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SWPMI_MspInit can be implemented in the user file
   */
}

/**
  * @brief DeInitialize the SWPMI MSP.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_MspDeInit(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SWPMI_MspDeInit can be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SWPMI_Exported_Group2 IO operation methods
  *  @brief SWPMI Transmit/Receive functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation methods #####
 ===============================================================================
 [..]
    This subsection provides a set of functions allowing to manage the SWPMI
     data transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode: The communication is performed in polling mode.
            The HAL status of all data processing is returned by the same function
            after finishing transfer.
       (++) Non-Blocking mode: The communication is performed using Interrupts
           or DMA. The end of the data processing will be indicated through the
           dedicated SWPMI Interrupt handler (HAL_SWPMI_IRQHandler()) when using Interrupt mode or
           the selected DMA channel interrupt handler when using DMA mode.
           The HAL_SWPMI_TxCpltCallback(), HAL_SWPMI_RxCpltCallback() user callbacks
           will be executed respectively at the end of the transmit or receive process.
           The HAL_SWPMI_ErrorCallback() user callback will be executed when a communication error is detected.

    (#) Blocking mode API's are:
        (++) HAL_SWPMI_Transmit()
        (++) HAL_SWPMI_Receive()

    (#) Non-Blocking mode API's with Interrupt are:
        (++) HAL_SWPMI_Transmit_IT()
        (++) HAL_SWPMI_Receive_IT()
        (++) HAL_SWPMI_IRQHandler()

    (#) Non-Blocking mode API's with DMA are:
        (++) HAL_SWPMI_Transmit_DMA()
        (++) HAL_SWPMI_Receive_DMA()
        (++) HAL_SWPMI_DMAPause()
        (++) HAL_SWPMI_DMAResume()
        (++) HAL_SWPMI_DMAStop()

    (#) A set of Transfer Complete Callbacks are provided in Non-Blocking mode:
        (++) HAL_SWPMI_TxHalfCpltCallback()
        (++) HAL_SWPMI_TxCpltCallback()
        (++) HAL_SWPMI_RxHalfCpltCallback()
        (++) HAL_SWPMI_RxCpltCallback()
        (++) HAL_SWPMI_ErrorCallback()

    (#) The capability to launch the above IO operations in loopback mode for 
        user application verification:    
        (++) HAL_SWPMI_EnableLoopback()
        (++) HAL_SWPMI_DisableLoopback()
        
@endverbatim
  * @{
  */

/**
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hswpmi: pointer to a SWPMI_HandleTypeDef structure that contains
  *                the configuration information for SWPMI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Transmit(SWPMI_HandleTypeDef *hswpmi, uint32_t* pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_RX))
    {
      /* Check if a non-blocking receive process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;

        /* Disable any transmitter interrupts */
        __HAL_SWPMI_DISABLE_IT(hswpmi, SWPMI_IT_TCIE | SWPMI_IT_TIE | SWPMI_IT_TXUNRIE | SWPMI_IT_TXBEIE);

        /* Disable any transmitter flags */
        __HAL_SWPMI_CLEAR_FLAG(hswpmi, SWPMI_FLAG_TXBEF | SWPMI_FLAG_TXUNRF | SWPMI_FLAG_TCF);

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      do
      {
        /* Wait the TXE to write data */
        if(HAL_IS_BIT_SET(hswpmi->Instance->ISR, SWPMI_FLAG_TXE))
        {
          hswpmi->Instance->TDR = (*pData++);
          Size--;
        }
        else
        {
          /* Check for the Timeout */
          if(Timeout != HAL_MAX_DELAY)
          {
            if((Timeout == 0) || ((HAL_GetTick() - tickstart) > Timeout))
            {
              status = HAL_TIMEOUT;
              break;
            }
          }
        }
      } while(Size != 0);

      /* Wait on TXBEF flag to be able to start a second transfer */
      if(SWPMI_WaitOnFlagSetUntilTimeout(hswpmi, SWPMI_FLAG_TXBEF, Timeout) != HAL_OK)
      {
        status = HAL_TIMEOUT;
      }

      if(status == HAL_OK)
      {
        /* Check if a non-blocking receive Process is ongoing or not */
        if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
        {
          hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;
        }
        else
        {
          hswpmi->State = HAL_SWPMI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_BUSY;
    }
  }

  if((status != HAL_OK) && (status != HAL_BUSY))
  {
    hswpmi->State = HAL_SWPMI_STATE_READY;
  }
  /* Process Unlocked */
  __HAL_UNLOCK(hswpmi);

  return status;
}

/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  hswpmi: pointer to a SWPMI_HandleTypeDef structure that contains
  *                the configuration information for SWPMI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Receive(SWPMI_HandleTypeDef *hswpmi, uint32_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_TX))
    {
      /* Check if a non-blocking transmit process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;

        /* Disable any receiver interrupts */
        CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_SRIE | SWPMI_IT_RIE | SWPMI_IT_RXBERIE | SWPMI_IT_RXOVRIE | SWPMI_IT_RXBFIE);

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      do
      {
        /* Wait the RXNE to read data */
        if(HAL_IS_BIT_SET(hswpmi->Instance->ISR, SWPMI_FLAG_RXNE))
        {
          (*pData++) = hswpmi->Instance->RDR;
          Size--;
        }
        else
        {
          /* Check for the Timeout */
          if(Timeout != HAL_MAX_DELAY)
          {
            if((Timeout == 0) || ((HAL_GetTick() - tickstart) > Timeout))
            {
              status = HAL_TIMEOUT;
              break;
            }
          }
        }
      } while(Size != 0);
      
      if(status == HAL_OK)
      {
        if(HAL_IS_BIT_SET(hswpmi->Instance->ISR, SWPMI_FLAG_RXBFF))
        {
          /* Clear RXBFF at end of reception */
          WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_RXBFF);
        }

        /* Check if a non-blocking transmit Process is ongoing or not */
        if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
        {
          hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;
        }
        else
        {
          hswpmi->State = HAL_SWPMI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_BUSY;
    }
  }

  if((status != HAL_OK) && (status != HAL_BUSY))
  {
    hswpmi->State = HAL_SWPMI_STATE_READY;
  }
  /* Process Unlocked */
  __HAL_UNLOCK(hswpmi);

  return status;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with interrupt.
  * @param  hswpmi: pointer to a SWPMI_HandleTypeDef structure that contains
  *                the configuration information for SWPMI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Transmit_IT(SWPMI_HandleTypeDef *hswpmi, uint32_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status =  HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_RX))
    {
      /* Update handle */
      hswpmi->pTxBuffPtr = pData;
      hswpmi->TxXferSize = Size;
      hswpmi->TxXferCount = Size;
      hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;

      /* Check if a receive process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      /* Enable the SWPMI transmit underrun error */
      __HAL_SWPMI_ENABLE_IT(hswpmi, SWPMI_IT_TXUNRIE);

      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);

      /* Enable the SWPMI interrupts:      */
      /* - Transmit data register empty    */
      /* - Transmit buffer empty           */
      /* - Transmit/Reception completion   */
      __HAL_SWPMI_ENABLE_IT(hswpmi, SWPMI_IT_TIE | SWPMI_IT_TXBEIE | SWPMI_IT_TCIE);
    }
    else
    {
      status =  HAL_BUSY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);
    }
  }

  return status;
}

/**
  * @brief Receive an amount of data in non-blocking mode with interrupt.
  * @param hswpmi: SWPMI handle
  * @param pData: pointer to data buffer
  * @param Size: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Receive_IT(SWPMI_HandleTypeDef *hswpmi, uint32_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status =  HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_TX))
    {
      /* Update handle */
      hswpmi->pRxBuffPtr = pData;
      hswpmi->RxXferSize = Size;
      hswpmi->RxXferCount = Size;
      hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;

      /* Check if a transmit process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);

      /* Enable the SWPMI slave resume */
      /* Enable the SWPMI Data Register not empty Interrupt, receive CRC Error, receive overrun and RxBuf Interrupt */
      /*  Enable the SWPMI Transmit/Reception completion   */
      __HAL_SWPMI_ENABLE_IT(hswpmi, SWPMI_IT_RIE | SWPMI_IT_RXBERIE | SWPMI_IT_RXOVRIE | SWPMI_IT_RXBFIE);
    }
    else
    {
      status = HAL_BUSY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);
    }
  }

  return status;
}

/**
  * @brief Transmit an amount of data in non-blocking mode with DMA interrupt.
  * @param hswpmi: SWPMI handle
  * @param pData: pointer to data buffer
  * @param Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Transmit_DMA(SWPMI_HandleTypeDef *hswpmi, uint32_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status =  HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_RX))
    {
      /* Update handle */
      hswpmi->pTxBuffPtr = pData;
      hswpmi->TxXferSize = Size;
      hswpmi->TxXferCount = Size;
      hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;

      /* Check if a receive process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      /* Set the SWPMI DMA transfer complete callback */
      hswpmi->hdmatx->XferCpltCallback = SWPMI_DMATransmitCplt;

      /* Set the SWPMI DMA Half transfer complete callback */
      hswpmi->hdmatx->XferHalfCpltCallback = SWPMI_DMATxHalfCplt;

      /* Set the DMA error callback */
      hswpmi->hdmatx->XferErrorCallback = SWPMI_DMAError;

      /* Enable the SWPMI transmit DMA Stream */
      HAL_DMA_Start_IT(hswpmi->hdmatx, (uint32_t)hswpmi->pTxBuffPtr, (uint32_t)&hswpmi->Instance->TDR, Size);

      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);

      /* Enable the DMA transfer for transmit request by setting the TXDMA bit
         in the SWPMI CR register */
      SET_BIT(hswpmi->Instance->CR, SWPMI_CR_TXDMA);
    }
    else
    {
      status = HAL_BUSY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);
    }
  }

  return status;
}

/**
  * @brief Receive an amount of data in non-blocking mode with DMA interrupt.
  * @param hswpmi: SWPMI handle
  * @param pData: pointer to data buffer
  * @param Size: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SWPMI_Receive_DMA(SWPMI_HandleTypeDef *hswpmi, uint32_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  if((pData == NULL ) || (Size == 0))
  {
    status =  HAL_ERROR;
  }
  else
  {
    /* Process Locked */
    __HAL_LOCK(hswpmi);

    if((hswpmi->State == HAL_SWPMI_STATE_READY) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_TX))
    {
      /* Update handle */
      hswpmi->pRxBuffPtr = pData;
      hswpmi->RxXferSize = Size;
      hswpmi->ErrorCode = HAL_SWPMI_ERROR_NONE;

      /* Check if a transmit process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_READY)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;

        /* Enable SWPMI peripheral if not */
        SET_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_TX_RX;
      }

      /* Set the SWPMI DMA transfer complete callback */
      hswpmi->hdmarx->XferCpltCallback = SWPMI_DMAReceiveCplt;

      /* Set the SWPMI DMA Half transfer complete callback */
      hswpmi->hdmarx->XferHalfCpltCallback = SWPMI_DMARxHalfCplt;

      /* Set the DMA error callback */
      hswpmi->hdmarx->XferErrorCallback = SWPMI_DMAError;

      /* Enable the DMA request */
      HAL_DMA_Start_IT(hswpmi->hdmarx, (uint32_t)&hswpmi->Instance->RDR, (uint32_t)hswpmi->pRxBuffPtr, Size);

      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);

      /* Enable the DMA transfer for the receiver request by setting the RXDMA bit
         in the SWPMI CR register */
      SET_BIT(hswpmi->Instance->CR, SWPMI_CR_RXDMA);
    }
    else
    {
      status = HAL_BUSY;

      /* Process Unlocked */
      __HAL_UNLOCK(hswpmi);
    }
  }

  return status;
}

/**
  * @brief Stop all DMA transfers.
  * @param hswpmi: SWPMI handle
  * @retval HAL_OK
  */
HAL_StatusTypeDef HAL_SWPMI_DMAStop(SWPMI_HandleTypeDef *hswpmi)
{
  /* Process Locked */
  __HAL_LOCK(hswpmi);

  /* Disable the SWPMI Tx/Rx DMA requests */
  CLEAR_BIT(hswpmi->Instance->CR, (SWPMI_CR_TXDMA | SWPMI_CR_RXDMA));

  /* Abort the SWPMI DMA tx channel */
  if(hswpmi->hdmatx != NULL)
  {
    HAL_DMA_Abort(hswpmi->hdmatx);
  }
  /* Abort the SWPMI DMA rx channel */
  if(hswpmi->hdmarx != NULL)
  {
    HAL_DMA_Abort(hswpmi->hdmarx);
  }

  /* Disable SWPMI interface */
  CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT);

  hswpmi->State = HAL_SWPMI_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hswpmi);

  return HAL_OK;
}


/**
  * @brief Enable the Loopback mode.
  * @param hswpmi: SWPMI handle
  * @note  Loopback mode is to be used only for test purposes
  * @retval HAL_OK / HAL_BUSY
  */
HAL_StatusTypeDef HAL_SWPMI_EnableLoopback(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef  status = HAL_OK;

  /* Process Locked */
  __HAL_LOCK(hswpmi);

  /* Check SWPMI not enabled */
  if(READ_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT) != RESET)
  {
    status = HAL_BUSY;
  }
  else
  {
    /* Set Loopback */
    SET_BIT(hswpmi->Instance->CR, SWPMI_CR_LPBK);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hswpmi);

  return status;
}

/**
  * @brief Disable the Loopback mode.
  * @param hswpmi: SWPMI handle
  * @note  Loopback mode is to be used only for test purposes
  * @retval HAL_OK / HAL_BUSY
  */
HAL_StatusTypeDef HAL_SWPMI_DisableLoopback(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef  status = HAL_OK;

  /* Process Locked */
  __HAL_LOCK(hswpmi);

  /* Check SWPMI not enabled */
  if(READ_BIT(hswpmi->Instance->CR, SWPMI_CR_SWPACT) != RESET)
  {
    status = HAL_BUSY;
  }
  else
  {
    /* Reset Loopback */
    CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_LPBK);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hswpmi);

  return status;
}

/**
  * @}
  */

/** @defgroup SWPMI_Exported_Group3 SWPMI IRQ handler and callbacks
 *  @brief  SWPMI  IRQ handler.
 *
@verbatim   
  ==============================================================================
                      ##### SWPMI IRQ handler and callbacks  #####
  ==============================================================================  
[..]  This section provides SWPMI IRQ handler and callback functions called within 
      the IRQ handler.

@endverbatim
  * @{
  */

/**
  * @brief Handle SWPMI interrupt request.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
void HAL_SWPMI_IRQHandler(SWPMI_HandleTypeDef *hswpmi)
{
  uint32_t regisr = READ_REG(hswpmi->Instance->ISR);
  uint32_t regier = READ_REG(hswpmi->Instance->IER);

  /* SWPMI CRC error interrupt occurred --------------------------------------*/
  if(((regisr & SWPMI_FLAG_RXBERF) != RESET) && ((regier & SWPMI_IT_RXBERIE) != RESET))
  {
    /* Disable Receive CRC interrupt */
    CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_RXBERIE | SWPMI_IT_RXBFIE);
    /* Clear Receive CRC and Receive buffer full flag */
    WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_RXBERF | SWPMI_FLAG_RXBFF);

    hswpmi->ErrorCode |= HAL_SWPMI_ERROR_CRC;
  }

  /* SWPMI Over-Run interrupt occurred -----------------------------------------*/
  if(((regisr & SWPMI_FLAG_RXOVRF) != RESET) && ((regier & SWPMI_IT_RXOVRIE) != RESET))
  {
    /* Disable Receive overrun interrupt */
    CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_RXOVRIE);
    /* Clear Receive overrun flag */
    WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_RXOVRF);

    hswpmi->ErrorCode |= HAL_SWPMI_ERROR_OVR;
  }

  /* SWPMI Under-Run interrupt occurred -----------------------------------------*/
  if(((regisr & SWPMI_FLAG_TXUNRF) != RESET) && ((regier & SWPMI_IT_TXUNRIE) != RESET))
  {
    /* Disable Transmit under run interrupt */
    CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_TXUNRIE);
    /* Clear Transmit under run flag */
    WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_TXUNRF);

    hswpmi->ErrorCode |= HAL_SWPMI_ERROR_UDR;
  }

   /* Call SWPMI Error Call back function if need be --------------------------*/
  if(hswpmi->ErrorCode != HAL_SWPMI_ERROR_NONE)
  {
    /* Set the SWPMI state ready to be able to start again the process */
    hswpmi->State = HAL_SWPMI_STATE_READY;

    HAL_SWPMI_ErrorCallback(hswpmi);
  }

  /* SWPMI in mode Receiver ---------------------------------------------------*/
  if(((regisr & SWPMI_FLAG_RXNE) != RESET) && ((regier & SWPMI_IT_RIE)  != RESET))
  {
    SWPMI_Receive_IT(hswpmi);
  }

  /* SWPMI in mode Transmitter ------------------------------------------------*/
  if(((regisr & SWPMI_FLAG_TXE) != RESET) && ((regier & SWPMI_IT_TIE) != RESET))
  {
    SWPMI_Transmit_IT(hswpmi);
  }

  /* SWPMI in mode Transmitter (Transmit buffer empty) ------------------------*/
  if(((regisr & SWPMI_FLAG_TXBEF) != RESET) && ((regier & SWPMI_IT_TXBEIE) != RESET))
  {
    SWPMI_EndTransmit_IT(hswpmi);
  }

  /* SWPMI in mode Receiver (Receive buffer full) -----------------------------*/
  if(((regisr & SWPMI_FLAG_RXBFF) != RESET) && ((regier & SWPMI_IT_RXBFIE) != RESET))
  {
    SWPMI_EndReceive_IT(hswpmi);
  }

  /* Both Transmission and reception complete ---------------------------------*/
  if(((regisr & SWPMI_FLAG_TCF) != RESET) && ((regier & SWPMI_IT_TCIE) != RESET))
  {
    SWPMI_EndTransmitReceive_IT(hswpmi);
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_TxCpltCallback(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SWPMI_TxCpltCallback is to be implemented in the user file
   */
}

/**
  * @brief  Tx Half Transfer completed callback.
  * @param  hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_TxHalfCpltCallback(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_SWPMI_TxHalfCpltCallback is to be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callback.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_RxCpltCallback(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SWPMI_RxCpltCallback is to be implemented in the user file
   */
}

/**
  * @brief  Rx Half Transfer completed callback.
  * @param  hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_RxHalfCpltCallback(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_SWPMI_RxHalfCpltCallback is to be implemented in the user file
   */
}

/**
  * @brief SWPMI error callback.
  * @param hswpmi: SWPMI handle
  * @retval None
  */
__weak void HAL_SWPMI_ErrorCallback(SWPMI_HandleTypeDef *hswpmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hswpmi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SWPMI_ErrorCallback is to be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SWPMI_Exported_Group4 Peripheral Control methods
  *  @brief   SWPMI control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control methods #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the SWPMI.
     (+) HAL_SWPMI_GetState() API is helpful to check in run-time the state of the SWPMI peripheral
     (+) HAL_SWPMI_GetError() API is helpful to check in run-time the error state of the SWPMI peripheral
@endverbatim
  * @{
  */

/**
  * @brief Return the SWPMI handle state.
  * @param hswpmi: SWPMI handle
  * @retval HAL state
  */
HAL_SWPMI_StateTypeDef HAL_SWPMI_GetState(SWPMI_HandleTypeDef *hswpmi)
{
  /* Return SWPMI handle state */
  return hswpmi->State;
}

/**
* @brief  Return the SWPMI error code.
* @param  hswpmi : pointer to a SWPMI_HandleTypeDef structure that contains
  *              the configuration information for the specified SWPMI.
* @retval SWPMI Error Code
*/
uint32_t HAL_SWPMI_GetError(SWPMI_HandleTypeDef *hswpmi)
{
  return hswpmi->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @defgroup SWPMI_Private_Functions SWPMI Private Functions
  * @{
  */

/**
  * @brief Transmit an amount of data in interrupt mode.
  * @note  Function called under interruption only, once interruptions have been enabled by HAL_SWPMI_Transmit_IT()
  * @param  hswpmi: SWPMI handle
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_Transmit_IT(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hswpmi->State == HAL_SWPMI_STATE_BUSY_TX) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX))
  {
    if(hswpmi->TxXferCount == 0)
    {
      /* Disable the SWPMI TXE and Underrun Interrupts */
      CLEAR_BIT(hswpmi->Instance->IER, (SWPMI_IT_TIE | SWPMI_IT_TXUNRIE));
    }
    else
    {
      hswpmi->Instance->TDR = (uint32_t)(*hswpmi->pTxBuffPtr++);
      hswpmi->TxXferCount--;
    }
  }
  else
  {
    status = HAL_BUSY;
  }

  return status;
}

/**
  * @brief  Wraps up transmission in non-blocking mode.
  * @param  hswpmi: SWPMI handle
  * @retval HAL status
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_EndTransmit_IT(SWPMI_HandleTypeDef *hswpmi)
{
  /* Clear the SWPMI Transmit buffer empty Flag */
  WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_TXBEF);
  /* Disable the all SWPMI Transmit Interrupts  */
  CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_TIE | SWPMI_IT_TXUNRIE | SWPMI_IT_TXBEIE);

  /* Check if a receive Process is ongoing or not */
  if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
  {
    hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;
  }
  else
  {
    hswpmi->State = HAL_SWPMI_STATE_READY;
  }
  
  HAL_SWPMI_TxCpltCallback(hswpmi);

  return HAL_OK;
}

/**
  * @brief Receive an amount of data in interrupt mode.
  * @note  Function called under interruption only, once interruptions have been enabled by HAL_SWPMI_Receive_IT()
  * @param  hswpmi: SWPMI handle
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_Receive_IT(SWPMI_HandleTypeDef *hswpmi)
{
  HAL_StatusTypeDef status = HAL_OK;

  if((hswpmi->State == HAL_SWPMI_STATE_BUSY_RX) || (hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX))
  {
    *hswpmi->pRxBuffPtr++ = (uint32_t)(hswpmi->Instance->RDR);

    if(--hswpmi->RxXferCount == 0)
    {
      /* Wait for RXBFF flag to update state */
      HAL_SWPMI_RxCpltCallback(hswpmi);
    }
  }
  else
  {
    status = HAL_BUSY;
  }

  return status;
}

/**
  * @brief  Wraps up reception in non-blocking mode.
  * @param  hswpmi: SWPMI handle
  * @retval HAL status
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_EndReceive_IT(SWPMI_HandleTypeDef *hswpmi)
{
  /* Clear the SWPMI Receive buffer full Flag */
  WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_RXBFF);
  /* Disable the all SWPMI Receive Interrupts  */
  CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_RIE | SWPMI_IT_RXBERIE | SWPMI_IT_RXOVRIE | SWPMI_IT_RXBFIE);

  /* Check if a transmit Process is ongoing or not */
  if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
  {
    hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;
  }
  else
  {
    hswpmi->State = HAL_SWPMI_STATE_READY;
  }

  return HAL_OK;
}

/**
  * @brief  Wraps up transmission and reception in non-blocking mode.
  * @param  hswpmi: SWPMI handle
  * @retval HAL status
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_EndTransmitReceive_IT(SWPMI_HandleTypeDef *hswpmi)
{
  /* Clear the SWPMI Transmission Complete Flag */
  WRITE_REG(hswpmi->Instance->ICR, SWPMI_FLAG_TCF);
  /* Disable the SWPMI Transmission  Complete Interrupt */
  CLEAR_BIT(hswpmi->Instance->IER, SWPMI_IT_TCIE);

  /* Check if a receive Process is ongoing or not */
  if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
  {
    hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;
  }
  else if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX)
  {
    hswpmi->State = HAL_SWPMI_STATE_READY;
  }

  return HAL_OK;
}

/**
  * @brief DMA SWPMI transmit process complete callback.
  * @param hdma: DMA handle
  * @retval None
  */
static void SWPMI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  SWPMI_HandleTypeDef* hswpmi = ( SWPMI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* DMA Normal mode*/
  if((hdma->Instance->CCR & DMA_CCR_CIRC) != SET)
  {
    hswpmi->TxXferCount = 0;

    /* Disable the DMA transfer for transmit request by setting the TXDMA bit
    in the SWPMI CR register */
    CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_TXDMA);

    /* Wait the TXBEF */
    if(SWPMI_WaitOnFlagSetUntilTimeout(hswpmi, SWPMI_FLAG_TXBEF, SWPMI_TIMEOUT_VALUE) != HAL_OK)
    {
      /* Timeout occurred */
      HAL_SWPMI_ErrorCallback(hswpmi);
    }
    else
    {
      /* No Timeout */
      /* Check if a receive process is ongoing or not */
      if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
      {
        hswpmi->State = HAL_SWPMI_STATE_BUSY_RX;
      }
      else
      {
        hswpmi->State = HAL_SWPMI_STATE_READY;
      }

      HAL_SWPMI_TxCpltCallback(hswpmi);
    }
  }
  /* DMA Circular mode */
  else
  {
    HAL_SWPMI_TxCpltCallback(hswpmi);
  }
}

/**
  * @brief DMA SWPMI transmit process half complete callback.
  * @param hdma : DMA handle
  * @retval None
  */
static void SWPMI_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  SWPMI_HandleTypeDef* hswpmi = (SWPMI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_SWPMI_TxHalfCpltCallback(hswpmi);
}


/**
  * @brief DMA SWPMI receive process complete callback.
  * @param hdma: DMA handle
  * @retval None
  */
static void SWPMI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SWPMI_HandleTypeDef* hswpmi = ( SWPMI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* DMA Normal mode*/
  if((hdma->Instance->CCR & DMA_CCR_CIRC) == RESET)
  {
    hswpmi->RxXferCount = 0;

    /* Disable the DMA transfer for the receiver request by setting the RXDMA bit
    in the SWPMI CR register */
    CLEAR_BIT(hswpmi->Instance->CR, SWPMI_CR_RXDMA);

    /* Check if a transmit Process is ongoing or not */
    if(hswpmi->State == HAL_SWPMI_STATE_BUSY_TX_RX)
    {
      hswpmi->State = HAL_SWPMI_STATE_BUSY_TX;
    }
    else
    {
      hswpmi->State = HAL_SWPMI_STATE_READY;
    }
  }
  HAL_SWPMI_RxCpltCallback(hswpmi);
}

/**
  * @brief DMA SWPMI receive process half complete callback.
  * @param hdma : DMA handle
  * @retval None
  */
static void SWPMI_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  SWPMI_HandleTypeDef* hswpmi = (SWPMI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_SWPMI_RxHalfCpltCallback(hswpmi);
}

/**
  * @brief DMA SWPMI communication error callback.
  * @param hdma: DMA handle
  * @retval None
  */
static void SWPMI_DMAError(DMA_HandleTypeDef *hdma)
{
  SWPMI_HandleTypeDef* hswpmi = ( SWPMI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Update handle */
  hswpmi->RxXferCount = 0;
  hswpmi->TxXferCount = 0;
  hswpmi->State= HAL_SWPMI_STATE_READY;
  hswpmi->ErrorCode |= HAL_SWPMI_ERROR_DMA;

  HAL_SWPMI_ErrorCallback(hswpmi);
}

/**
  * @brief  Handle SWPMI Communication Timeout.
  * @param  hswpmi: SWPMI handle
  * @param  Flag: specifies the SWPMI flag to check.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef SWPMI_WaitOnFlagSetUntilTimeout(SWPMI_HandleTypeDef *hswpmi, uint32_t Flag, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef status = HAL_OK;

  /* Wait until flag is set */
  while(!(HAL_IS_BIT_SET(hswpmi->Instance->ISR, Flag)))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((HAL_GetTick() - tickstart ) > Timeout)
      {
        hswpmi->State = HAL_SWPMI_STATE_TIMEOUT;

        status = HAL_TIMEOUT;
        break;
      }
    }
  }

  return status;
}

/**
  * @}
  */

#endif /* HAL_SWPMI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
