/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2s_ex.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   I2S HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of I2S extension peripheral:
  *           + Extension features Functions
  *         
  @verbatim
  ==============================================================================
                    ##### I2S Extension features #####
  ============================================================================== 
  [..]
     (#) In I2S full duplex mode, each SPI peripheral is able to manage sending and receiving 
         data simultaneously using two data lines. Each SPI peripheral has an extended block 
         called I2Sxext (i.e I2S2ext for SPI2 and I2S3ext for SPI3).
     (#) The extension block is not a full SPI IP, it is used only as I2S slave to
         implement full duplex mode. The extension block uses the same clock sources
         as its master.

     (#) Both I2Sx and I2Sx_ext can be configured as transmitters or receivers.

     [..]
       (@) Only I2Sx can deliver SCK and WS to I2Sx_ext in full duplex mode, where 
         I2Sx can be I2S2 or I2S3.

                  ##### How to use this driver #####
 ===============================================================================
 [..]    
   Three operation modes are available within this driver :     
  
   *** Polling mode IO operation ***
   =================================
   [..]    
     (+) Send and receive in the same time an amount of data in blocking mode using HAL_I2S_TransmitReceive() 
   
   *** Interrupt mode IO operation ***    
   ===================================
   [..]    
     (+) Send and receive in the same time an amount of data in non blocking mode using HAL_I2S_TransmitReceive_IT() 
     (+) At transmission end of half transfer HAL_I2S_TxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxHalfCpltCallback 
     (+) At transmission end of transfer HAL_I2S_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxCpltCallback
     (+) At reception end of half transfer HAL_I2S_RxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxHalfCpltCallback 
     (+) At reception end of transfer HAL_I2S_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxCpltCallback                                      
     (+) In case of transfer Error, HAL_I2S_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_I2S_ErrorCallback

   *** DMA mode IO operation ***    
   ==============================
   [..] 
     (+) Send and receive an amount of data in non blocking mode (DMA) using HAL_I2S_TransmitReceive_DMA() 
     (+) At transmission end of half transfer HAL_I2S_TxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxHalfCpltCallback 
     (+) At transmission end of transfer HAL_I2S_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxCpltCallback
     (+) At reception end of half transfer HAL_I2S_RxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxHalfCpltCallback 
     (+) At reception end of transfer HAL_I2S_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxCpltCallback                                     
     (+) In case of transfer Error, HAL_I2S_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_I2S_ErrorCallback
     (+) Pause the DMA Transfer using HAL_I2S_DMAPause()      
     (+) Resume the DMA Transfer using HAL_I2S_DMAResume()  
     (+) Stop the DMA Transfer using HAL_I2S_DMAStop()  

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

/** @defgroup I2SEx 
  * @brief I2S HAL module driver
  * @{
  */

#ifdef HAL_I2S_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup I2SEx_Private_Functions
  * @{
  */

/** @defgroup I2SEx_Group1 Extension features functions 
  *  @brief   Extension features functions
  *
@verbatim    
 ===============================================================================
                       ##### Extension features Functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the I2S data 
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode. 
            The status of all data processing is returned by the same function 
            after finishing transfer.  
       (++) No-Blocking mode : The communication is performed using Interrupts 
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the 
            dedicated I2S IRQ when using Interrupt mode or the DMA IRQ when 
            using DMA mode.

    (#) Blocking mode functions are :
        (++) HAL_I2S_TransmitReceive()
        
    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2S_TransmitReceive_IT()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_I2S_TransmitReceive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2S_TxCpltCallback()
        (++) HAL_I2S_RxCpltCallback()
        (++) HAL_I2S_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief Full-Duplex Transmit/Receive data in blocking mode.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pTxData: a 16-bit pointer to the Transmit data buffer.
  * @param pRxData: a 16-bit pointer to the Receive data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @param Timeout: Timeout duration
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  uint32_t tmp1 = 0, tmp2 = 0;
 
  if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }

  /* Check the I2S State */
  if(hi2s->State == HAL_I2S_STATE_READY)
  {  
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN); 
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
       (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }
    
    /* Process Locked */
    __HAL_LOCK(hi2s);
    
    /* Set the I2S State busy TX/RX */
    hi2s->State = HAL_I2S_STATE_BUSY_TX_RX;
    
    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
    { 
      /* Check if the I2S is already enabled: The I2S is kept enabled at the end of transaction
      to avoid the clock de-synchronization between Master and Slave. */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Enable I2Sext(receiver) before enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;

        /* Enable I2Sx peripheral */
        __HAL_I2S_ENABLE(hi2s);
      }
      
      while(hi2s->TxXferCount > 0)
      {
        /* Wait until TXE flag is set */
        if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        hi2s->Instance->DR = (*pTxData++);

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait until RXNE flag is set */
        while((I2SxEXT(hi2s->Instance)->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
        {
          if(Timeout != HAL_MAX_DELAY)
          {
            if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
            {
              /* Process Unlocked */
              __HAL_UNLOCK(hi2s);

              return HAL_TIMEOUT;
            }
          }
        }
        (*pRxData++) = I2SxEXT(hi2s->Instance)->DR;
        
        hi2s->TxXferCount--;
        hi2s->RxXferCount--;
      }
    }
    /* The I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX Mode is selected */
    else
    {
      /* Check if the I2S is already enabled */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Enable I2S peripheral before the I2Sext*/
        __HAL_I2S_ENABLE(hi2s);

        /* Enable I2Sext(transmitter) after enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;
      }
      else
      {
        /* Check if Master Receiver mode is selected */
        if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX)
        {
          /* Clear the Overrun Flag by a read operation on the SPI_DR register followed by a read
          access to the SPI_SR register. */ 
          __HAL_I2S_CLEAR_OVRFLAG(hi2s);
        }
      }
      while(hi2s->TxXferCount > 0)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait until TXE flag is set */
        while((I2SxEXT(hi2s->Instance)->SR & SPI_SR_TXE) != SPI_SR_TXE)
        {
          if(Timeout != HAL_MAX_DELAY)
          {
            if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
            {
              /* Process Unlocked */
              __HAL_UNLOCK(hi2s);

              return HAL_TIMEOUT;
            }
          }
        }
        I2SxEXT(hi2s->Instance)->DR = (*pTxData++);
        
        /* Wait until RXNE flag is set */
        if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_RXNE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        (*pRxData++) = hi2s->Instance->DR;

        hi2s->TxXferCount--;
        hi2s->RxXferCount--;
      }
    }

    /* Set the I2S State ready */
    hi2s->State = HAL_I2S_STATE_READY; 

    /* Process Unlocked */
    __HAL_UNLOCK(hi2s);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Full-Duplex Transmit/Receive data in non-blocking mode using Interrupt 
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pTxData: a 16-bit pointer to the Transmit data buffer.
  * @param pRxData: a 16-bit pointer to the Receive data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size)
{
  uint32_t tmp1 = 0, tmp2 = 0;
  
  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    hi2s->pTxBuffPtr = pTxData;
    hi2s->pRxBuffPtr = pRxData;

    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B)||\
       (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }  
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }
    
    /* Process Locked */
    __HAL_LOCK(hi2s);
    
    hi2s->State = HAL_I2S_STATE_BUSY_TX_RX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;

    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
    { 
      /* Enable I2Sext RXNE and ERR interrupts */
      I2SxEXT(hi2s->Instance)->CR2 |= (I2S_IT_RXNE | I2S_IT_ERR);

      /* Enable I2Sx TXE and ERR interrupts */
      __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

      /* Check if the I2S is already enabled */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Enable I2Sext(receiver) before enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;

        /* Enable I2Sx peripheral */
        __HAL_I2S_ENABLE(hi2s);
      }
    }
    /* The I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX Mode is selected */
    else
    {
      /* Enable I2Sext TXE and ERR interrupts */
      I2SxEXT(hi2s->Instance)->CR2 |= (I2S_IT_TXE |I2S_IT_ERR);

      /* Enable I2Sext RXNE and ERR interrupts */
      __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_RXNE | I2S_IT_ERR));

      /* Check if the I2S is already enabled */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Check if the I2S_MODE_MASTER_RX is selected */
        if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX) 
        {
          /* Prepare the First Data before enabling the I2S */
          if(hi2s->TxXferCount != 0)
          {
            /* Transmit First data */
            I2SxEXT(hi2s->Instance)->DR = (*hi2s->pTxBuffPtr++);
            hi2s->TxXferCount--;	

            if(hi2s->TxXferCount == 0)
            {
              /* Disable I2Sext TXE interrupt */
              I2SxEXT(hi2s->Instance)->CR2 &= ~I2S_IT_TXE;
            }
          }
        }
        /* Enable I2S peripheral */
        __HAL_I2S_ENABLE(hi2s);
        
        /* Enable I2Sext(transmitter) after enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;
      }
    }
    /* Process Unlocked */
    __HAL_UNLOCK(hi2s);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief Full-Duplex Transmit/Receive data in non-blocking mode using DMA  
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pTxData: a 16-bit pointer to the Transmit data buffer.
  * @param pRxData: a 16-bit pointer to the Receive data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0, tmp2 = 0;
    
  if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }

  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    hi2s->pTxBuffPtr = pTxData;
    hi2s->pRxBuffPtr = pRxData;

    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B)||\
       (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }

    /* Process Locked */
    __HAL_LOCK(hi2s);

    hi2s->State = HAL_I2S_STATE_BUSY_TX_RX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;

    /* Set the I2S Rx DMA Half transfert complete callback */
    hi2s->hdmarx->XferHalfCpltCallback = I2S_DMARxHalfCplt;

    /* Set the I2S Rx DMA transfert complete callback */
    hi2s->hdmarx->XferCpltCallback = I2S_DMARxCplt;

    /* Set the I2S Rx DMA error callback */
    hi2s->hdmarx->XferErrorCallback = I2S_DMAError;

    /* Set the I2S Tx DMA Half transfert complete callback */
    hi2s->hdmatx->XferHalfCpltCallback = I2S_DMATxHalfCplt;

    /* Set the I2S Tx DMA transfert complete callback */
    hi2s->hdmatx->XferCpltCallback = I2S_DMATxCplt;

    /* Set the I2S Tx DMA error callback */
    hi2s->hdmatx->XferErrorCallback = I2S_DMAError;

    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
    {
      /* Enable the Rx DMA Stream */
      tmp = (uint32_t*)&pRxData;
      HAL_DMA_Start_IT(hi2s->hdmarx, (uint32_t)&I2SxEXT(hi2s->Instance)->DR, *(uint32_t*)tmp, hi2s->RxXferSize);

      /* Enable Rx DMA Request */  
      I2SxEXT(hi2s->Instance)->CR2 |= SPI_CR2_RXDMAEN;

      /* Enable the Tx DMA Stream */
      tmp = (uint32_t*)&pTxData;
      HAL_DMA_Start_IT(hi2s->hdmatx, *(uint32_t*)tmp, (uint32_t)&hi2s->Instance->DR, hi2s->TxXferSize);

      /* Enable Tx DMA Request */  
      hi2s->Instance->CR2 |= SPI_CR2_TXDMAEN;

      /* Check if the I2S is already enabled */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Enable I2Sext(receiver) before enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;

        /* Enable I2S peripheral after the I2Sext */
        __HAL_I2S_ENABLE(hi2s);
      }
    }
    else
    {
      /* Enable the Tx DMA Stream */
      tmp = (uint32_t*)&pTxData;
      HAL_DMA_Start_IT(hi2s->hdmatx, *(uint32_t*)tmp, (uint32_t)&I2SxEXT(hi2s->Instance)->DR, hi2s->TxXferSize);

      /* Enable Tx DMA Request */  
      I2SxEXT(hi2s->Instance)->CR2 |= SPI_CR2_TXDMAEN;

      /* Enable the Rx DMA Stream */
      tmp = (uint32_t*)&pRxData;
      HAL_DMA_Start_IT(hi2s->hdmarx, (uint32_t)&hi2s->Instance->DR, *(uint32_t*)tmp, hi2s->RxXferSize);

      /* Enable Rx DMA Request */  
      hi2s->Instance->CR2 |= SPI_CR2_RXDMAEN;

      /* Check if the I2S is already enabled */ 
      if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
      {
        /* Enable I2S peripheral before the I2Sext */
        __HAL_I2S_ENABLE(hi2s);

        /* Enable I2Sext(transmitter) after enabling I2Sx peripheral */
        I2SxEXT(hi2s->Instance)->I2SCFGR |= SPI_I2SCFGR_I2SE;
      }
      else
      {
        /* Check if Master Receiver mode is selected */
        if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX)
        {
          /* Clear the Overrun Flag by a read operation on the SPI_DR register followed by a read
          access to the SPI_SR register. */ 
          __HAL_I2S_CLEAR_OVRFLAG(hi2s);
        }
      }
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2s);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @}
  */

/**
  * @brief Full-Duplex Transmit/Receive data in non-blocking mode using Interrupt 
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s)
{
  uint32_t tmp1 = 0, tmp2 = 0;
  
  if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
  {
    /* Process Locked */
    __HAL_LOCK(hi2s);

    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
    {
      if(hi2s->TxXferCount != 0)
      {
        if(__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_TXE) != RESET)
        {        
          /* Transmit data */
          hi2s->Instance->DR = (*hi2s->pTxBuffPtr++);
          hi2s->TxXferCount--;

          if(hi2s->TxXferCount == 0)
          {
            /* Disable TXE interrupt */
            __HAL_I2S_DISABLE_IT(hi2s, I2S_IT_TXE);
          }
        }
      }

      if(hi2s->RxXferCount != 0)
      {
        if((I2SxEXT(hi2s->Instance)->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
        {
          /* Receive data */
          (*hi2s->pRxBuffPtr++) = I2SxEXT(hi2s->Instance)->DR;
          hi2s->RxXferCount--;

          if(hi2s->RxXferCount == 0)
          {
            /* Disable I2Sext RXNE interrupt */
            I2SxEXT(hi2s->Instance)->CR2 &= ~I2S_IT_RXNE;
          }
        }
      }
    }
    /* The I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX Mode is selected */ 
    else
    {
      if(hi2s->TxXferCount != 0)
      {
        if((I2SxEXT(hi2s->Instance)->SR & SPI_SR_TXE) == SPI_SR_TXE)
        {        
          /* Transmit data */
          I2SxEXT(hi2s->Instance)->DR = (*hi2s->pTxBuffPtr++);
          hi2s->TxXferCount--;

          if(hi2s->TxXferCount == 0)
          {
            /* Disable I2Sext TXE interrupt */
            I2SxEXT(hi2s->Instance)->CR2 &= ~I2S_IT_TXE;

            HAL_I2S_TxCpltCallback(hi2s);
          }
        }
      }
      if(hi2s->RxXferCount != 0)
      {
        if(__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_RXNE) != RESET)
        {
          /* Receive data */
          (*hi2s->pRxBuffPtr++) = hi2s->Instance->DR;
          hi2s->RxXferCount--;

          if(hi2s->RxXferCount == 0)
          {
            /* Disable RXNE interrupt */
            __HAL_I2S_DISABLE_IT(hi2s, I2S_IT_RXNE);

            HAL_I2S_RxCpltCallback(hi2s);
          }
        }
      }
    }

    tmp1 = hi2s->RxXferCount;
    tmp2 = hi2s->TxXferCount;
    if((tmp1 == 0) && (tmp2 == 0))
    {
      /* Disable I2Sx ERR interrupt */
      __HAL_I2S_DISABLE_IT(hi2s, I2S_IT_ERR);
      /* Disable I2Sext ERR interrupt */
      I2SxEXT(hi2s->Instance)->CR2 &= ~I2S_IT_ERR;
      
      hi2s->State = HAL_I2S_STATE_READY; 
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hi2s);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}

/**
  * @}
  */
  
#endif /* HAL_I2S_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
