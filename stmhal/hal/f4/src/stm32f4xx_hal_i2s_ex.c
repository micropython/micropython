/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2s_ex.c
  * @author  MCD Application Team
  * @version V1.5.1
  * @date    01-July-2016
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
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup I2SEx I2SEx
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
/** @addtogroup I2SEx_Private_Functions
  * @{
  */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup I2SEx_Exported_Functions I2S Exported Functions
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
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F469xx) ||\
    defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) ||\
    defined(STM32F412Cx)
/**
  * @brief Initializes the I2S according to the specified parameters 
  *         in the I2S_InitTypeDef and create the associated handle.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s)
{
  uint32_t tmpreg = 0U, i2sdiv = 2U, i2sodd = 0U, packetlength = 1U;
  uint32_t tmp = 0U, i2sclk = 0U;
  
  /* Check the I2S handle allocation */
  if(hi2s == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the I2S parameters */
  assert_param(IS_I2S_MODE(hi2s->Init.Mode));
  assert_param(IS_I2S_STANDARD(hi2s->Init.Standard));
  assert_param(IS_I2S_DATA_FORMAT(hi2s->Init.DataFormat));
  assert_param(IS_I2S_MCLK_OUTPUT(hi2s->Init.MCLKOutput));
  assert_param(IS_I2S_AUDIO_FREQ(hi2s->Init.AudioFreq));
  assert_param(IS_I2S_CPOL(hi2s->Init.CPOL));  
  assert_param(IS_I2S_CLOCKSOURCE(hi2s->Init.ClockSource));
  
  if(hi2s->State == HAL_I2S_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hi2s->Lock = HAL_UNLOCKED;
    /* Init the low level hardware : GPIO, CLOCK, CORTEX */
    HAL_I2S_MspInit(hi2s);
  }
  
  hi2s->State = HAL_I2S_STATE_BUSY;
  
  /*----------------------- SPIx I2SCFGR & I2SPR Configuration ---------------*/
  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
  hi2s->Instance->I2SCFGR &= ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | \
                               SPI_I2SCFGR_I2SSTD | SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | \
                               SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD); 
  hi2s->Instance->I2SPR = 0x0002U;

  /* Get the I2SCFGR register value */
  tmpreg = hi2s->Instance->I2SCFGR;

  /* If the default frequency value has to be written, reinitialize i2sdiv and i2sodd */
  /* If the requested audio frequency is not the default, compute the prescaler */
  if(hi2s->Init.AudioFreq != I2S_AUDIOFREQ_DEFAULT)
  {
    /* Check the frame length (For the Prescaler computing) *******************/
    if(hi2s->Init.DataFormat != I2S_DATAFORMAT_16B)
    {
      /* Packet length is 32 bits */
      packetlength = 2U;
    }

    /* Get I2S source Clock frequency  ****************************************/
    i2sclk = I2S_GetInputClock(hi2s);

    /* Compute the Real divider depending on the MCLK output state, with a floating point */
    if(hi2s->Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE)
    {
      /* MCLK output is enabled */
      tmp = (uint32_t)(((((i2sclk / 256U) * 10U) / hi2s->Init.AudioFreq)) + 5U);
    }
    else
    {
      /* MCLK output is disabled */
      tmp = (uint32_t)(((((i2sclk / (32U * packetlength)) * 10U) / hi2s->Init.AudioFreq)) + 5U);
    }

    /* Remove the flatting point */
    tmp = tmp / 10U;  

    /* Check the parity of the divider */
    i2sodd = (uint32_t)(tmp & (uint32_t)1U);

    /* Compute the i2sdiv prescaler */
    i2sdiv = (uint32_t)((tmp - i2sodd) / 2U);

    /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
    i2sodd = (uint32_t) (i2sodd << 8U);
  }

  /* Test if the divider is 1 or 0 or greater than 0xFF */
  if((i2sdiv < 2U) || (i2sdiv > 0xFFU))
  {
    /* Set the default values */
    i2sdiv = 2U;
    i2sodd = 0U;
  }
  
  /* Write to SPIx I2SPR register the computed value */
  hi2s->Instance->I2SPR = (uint32_t)((uint32_t)i2sdiv | (uint32_t)(i2sodd | (uint32_t)hi2s->Init.MCLKOutput));
  
  /* Configure the I2S with the I2S_InitStruct values */
  tmpreg |= (uint32_t)(SPI_I2SCFGR_I2SMOD | hi2s->Init.Mode | hi2s->Init.Standard | hi2s->Init.DataFormat | hi2s->Init.CPOL);
  
#if defined(SPI_I2SCFGR_ASTRTEN)
  if (hi2s->Init.Standard == I2S_STANDARD_PCM_SHORT) 
  {
  /* Write to SPIx I2SCFGR */  
  hi2s->Instance->I2SCFGR = tmpreg | SPI_I2SCFGR_ASTRTEN;
  }
  else
  {
  /* Write to SPIx I2SCFGR */  
  hi2s->Instance->I2SCFGR = tmpreg;    
  }
#else
  /* Write to SPIx I2SCFGR */  
  hi2s->Instance->I2SCFGR = tmpreg;
#endif
      
  /* Configure the I2S extended if the full duplex mode is enabled */
  assert_param(IS_I2S_FULLDUPLEX_MODE(hi2s->Init.FullDuplexMode));
  if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
  {    
    /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
    I2SxEXT(hi2s->Instance)->I2SCFGR &= ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | \
                                          SPI_I2SCFGR_I2SSTD | SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | \
                                          SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD);
    I2SxEXT(hi2s->Instance)->I2SPR = 2U;
    
    /* Get the I2SCFGR register value */
    tmpreg = I2SxEXT(hi2s->Instance)->I2SCFGR;
    
    /* Get the mode to be configured for the extended I2S */
    if((hi2s->Init.Mode == I2S_MODE_MASTER_TX) || (hi2s->Init.Mode == I2S_MODE_SLAVE_TX))
    {
      tmp = I2S_MODE_SLAVE_RX;
    }
    else
    {
      if((hi2s->Init.Mode == I2S_MODE_MASTER_RX) || (hi2s->Init.Mode == I2S_MODE_SLAVE_RX))
      {
        tmp = I2S_MODE_SLAVE_TX;
      }
    }
    
    /* Configure the I2S Slave with the I2S Master parameter values */
    tmpreg |= (uint32_t)(SPI_I2SCFGR_I2SMOD | tmp | hi2s->Init.Standard | hi2s->Init.DataFormat | hi2s->Init.CPOL);
    
    /* Write to SPIx I2SCFGR */  
    I2SxEXT(hi2s->Instance)->I2SCFGR = tmpreg;
  }
  
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->State= HAL_I2S_STATE_READY;
  
  return HAL_OK;
}

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
  uint32_t tickstart = 0U;
  uint32_t tmp1 = 0U;
 
  if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0U)) 
  {
    return  HAL_ERROR;
  }

  /* Check the I2S State */
  if(hi2s->State == HAL_I2S_STATE_READY)
  {  
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B) || (tmp1 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2U;
      hi2s->TxXferCount = Size*2U;
      hi2s->RxXferSize = Size*2U;
      hi2s->RxXferCount = Size*2U;
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
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp1 == I2S_MODE_SLAVE_TX))
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
      
      while(hi2s->TxXferCount > 0U)
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
            if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
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
      while(hi2s->TxXferCount > 0U)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait until TXE flag is set */
        while((I2SxEXT(hi2s->Instance)->SR & SPI_SR_TXE) != SPI_SR_TXE)
        {
          if(Timeout != HAL_MAX_DELAY)
          {
            if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
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
  uint32_t tmp1 = 0U;
  
  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0U)) 
    {
      return  HAL_ERROR;
    }

    hi2s->pTxBuffPtr = pTxData;
    hi2s->pRxBuffPtr = pRxData;

    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B) || (tmp1 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2U;
      hi2s->TxXferCount = Size*2U;
      hi2s->RxXferSize = Size*2U;
      hi2s->RxXferCount = Size*2U;
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
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp1 == I2S_MODE_SLAVE_TX))
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
          if(hi2s->TxXferCount != 0U)
          {
            /* Transmit First data */
            I2SxEXT(hi2s->Instance)->DR = (*hi2s->pTxBuffPtr++);
            hi2s->TxXferCount--;	

            if(hi2s->TxXferCount == 0U)
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
  uint32_t tmp1 = 0U;
    
  if((pTxData == NULL ) || (pRxData == NULL ) || (Size == 0U)) 
  {
    return  HAL_ERROR;
  }

  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    hi2s->pTxBuffPtr = pTxData;
    hi2s->pRxBuffPtr = pRxData;

    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    /* Check the Data format: When a 16-bit data frame or a 16-bit data frame extended 
       is selected during the I2S configuration phase, the Size parameter means the number
       of 16-bit data length in the transaction and when a 24-bit data frame or a 32-bit data 
       frame is selected the Size parameter means the number of 16-bit data length. */
    if((tmp1 == I2S_DATAFORMAT_24B) || (tmp1 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2U;
      hi2s->TxXferCount = Size*2U;
      hi2s->RxXferSize = Size*2U;
      hi2s->RxXferCount = Size*2U;
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

    /* Set the I2S Rx DMA Half transfer complete callback */
    hi2s->hdmarx->XferHalfCpltCallback = I2S_DMARxHalfCplt;

    /* Set the I2S Rx DMA transfer complete callback */
    hi2s->hdmarx->XferCpltCallback = I2S_DMARxCplt;

    /* Set the I2S Rx DMA error callback */
    hi2s->hdmarx->XferErrorCallback = I2S_DMAError;

    /* Set the I2S Tx DMA Half transfer complete callback */
    hi2s->hdmatx->XferHalfCpltCallback = I2S_DMATxHalfCplt;

    /* Set the I2S Tx DMA transfer complete callback */
    hi2s->hdmatx->XferCpltCallback = I2S_DMATxCplt;

    /* Set the I2S Tx DMA error callback */
    hi2s->hdmatx->XferErrorCallback = I2S_DMAError;

    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp1 == I2S_MODE_SLAVE_TX))
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
  * @brief Pauses the audio stream playing from the Media.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s)
{
  /* Process Locked */
  __HAL_LOCK(hi2s);
  
  if(hi2s->State == HAL_I2S_STATE_BUSY_TX)
  {
    /* Disable the I2S DMA Tx request */
    hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
  }
  else if(hi2s->State == HAL_I2S_STATE_BUSY_RX)
  {
    /* Disable the I2S DMA Rx request */
    hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
  }
  else if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
  {
    if((hi2s->Init.Mode == I2S_MODE_SLAVE_TX)||(hi2s->Init.Mode == I2S_MODE_MASTER_TX))
    {
      /* Disable the I2S DMA Tx request */
      hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
      /* Disable the I2SEx Rx DMA Request */
      I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    }
    else
    {
      /* Disable the I2S DMA Rx request */
      hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
      /* Disable the I2SEx Tx DMA Request */
      I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);      
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hi2s);
  
  return HAL_OK; 
}

/**
  * @brief Resumes the audio stream playing from the Media.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s)
{
  /* Process Locked */
  __HAL_LOCK(hi2s);
  
  if(hi2s->State == HAL_I2S_STATE_BUSY_TX)
  {
    /* Enable the I2S DMA Tx request */
    hi2s->Instance->CR2 |= SPI_CR2_TXDMAEN;
  }
  else if(hi2s->State == HAL_I2S_STATE_BUSY_RX)
  {
    /* Enable the I2S DMA Rx request */
    hi2s->Instance->CR2 |= SPI_CR2_RXDMAEN;
  }
  else if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
  {
    if((hi2s->Init.Mode == I2S_MODE_SLAVE_TX)||(hi2s->Init.Mode == I2S_MODE_MASTER_TX))
    {
      /* Enable the I2S DMA Tx request */
      hi2s->Instance->CR2 |= SPI_CR2_TXDMAEN;
      /* Disable the I2SEx Rx DMA Request */  
      I2SxEXT(hi2s->Instance)->CR2 |= SPI_CR2_RXDMAEN;
    }
    else
    {
      /* Enable the I2S DMA Rx request */
      hi2s->Instance->CR2 |= SPI_CR2_RXDMAEN;
      /* Enable the I2SEx Tx DMA Request */  
      I2SxEXT(hi2s->Instance)->CR2 |= SPI_CR2_TXDMAEN;
    }
  }

  /* If the I2S peripheral is still not enabled, enable it */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE) == 0U)
  {
    /* Enable I2S peripheral */    
    __HAL_I2S_ENABLE(hi2s);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hi2s);
  
  return HAL_OK;
}

/**
  * @brief Resumes the audio stream playing from the Media.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s)
{
  /* Process Locked */
  __HAL_LOCK(hi2s);
  
  /* Disable the I2S Tx/Rx DMA requests */
  hi2s->Instance->CR2 &= ~SPI_CR2_TXDMAEN;
  hi2s->Instance->CR2 &= ~SPI_CR2_RXDMAEN;
  
  if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
  {
    /* Disable the I2S extended Tx/Rx DMA requests */  
    I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
  }
  
  /* Abort the I2S DMA Stream tx */
  if(hi2s->hdmatx != NULL)
  {
    HAL_DMA_Abort(hi2s->hdmatx);
  }
  /* Abort the I2S DMA Stream rx */
  if(hi2s->hdmarx != NULL)
  {
    HAL_DMA_Abort(hi2s->hdmarx);
  }

  /* Disable I2S peripheral */
  __HAL_I2S_DISABLE(hi2s);
 
  if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
  {
    /* Disable the I2Sext peripheral */
    I2SxEXT(hi2s->Instance)->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
  }
  hi2s->State = HAL_I2S_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hi2s);
  
  return HAL_OK;
}

/**
  * @brief  This function handles I2S interrupt request.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s)
{  
  uint32_t tmp1 = 0U, tmp2 = 0U;
  __IO uint32_t tmpreg1 = 0U;    
  if(hi2s->Init.FullDuplexMode != I2S_FULLDUPLEXMODE_ENABLE)
  {
    if(hi2s->State == HAL_I2S_STATE_BUSY_RX)
    {
      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_RXNE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_RXNE);
      /* I2S in mode Receiver ------------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        I2S_Receive_IT(hi2s);
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_OVR);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR);
      /* I2S Overrun error interrupt occurred ---------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        __HAL_I2S_CLEAR_OVRFLAG(hi2s);
        hi2s->ErrorCode |= HAL_I2S_ERROR_OVR;
      }
    }

    if(hi2s->State == HAL_I2S_STATE_BUSY_TX)
    {
      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_TXE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_TXE);
      /* I2S in mode Tramitter -----------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        I2S_Transmit_IT(hi2s);
      } 

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_UDR);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR);
      /* I2S Underrun error interrupt occurred --------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        __HAL_I2S_CLEAR_UDRFLAG(hi2s);
        hi2s->ErrorCode |= HAL_I2S_ERROR_UDR;
      }
    }
  }
  else
  {
    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp1 == I2S_MODE_SLAVE_TX))
    { 
      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_RXNE; 
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_RXNE;  
      /* I2Sext in mode Receiver ---------------------------------------------*/
      if((tmp1 == SPI_SR_RXNE) && (tmp2 == I2S_IT_RXNE))
      {
        /* When the I2S mode is configured as I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX,
        the I2Sext RXNE interrupt will be generated to manage the full-duplex receive phase. */
        I2SEx_TransmitReceive_IT(hi2s);
      }

      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_OVR;
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_ERR;
      /* I2Sext Overrun error interrupt occurred -----------------------------*/
      if((tmp1 == SPI_SR_OVR) && (tmp2 == I2S_IT_ERR))
      {
        /* Clear I2Sext OVR Flag */ 
        tmpreg1 = I2SxEXT(hi2s->Instance)->DR;
        tmpreg1 = I2SxEXT(hi2s->Instance)->SR;
        hi2s->ErrorCode |= HAL_I2SEX_ERROR_OVR;
        UNUSED(tmpreg1);
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_TXE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_TXE);
      /* I2S in mode Tramitter -----------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        /* When the I2S mode is configured as I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX,
        the I2S TXE interrupt will be generated to manage the full-duplex transmit phase. */
        I2SEx_TransmitReceive_IT(hi2s);
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_UDR);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR);
      /* I2S Underrun error interrupt occurred -------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        __HAL_I2S_CLEAR_UDRFLAG(hi2s);
        hi2s->ErrorCode |= HAL_I2S_ERROR_UDR;
      }
    }
    /* The I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX Mode is selected */
    else
    {
      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_RXNE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_RXNE);
      /* I2S in mode Receiver ------------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        /* When the I2S mode is configured as I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX,
        the I2S RXNE interrupt will be generated to manage the full-duplex receive phase. */
        I2SEx_TransmitReceive_IT(hi2s);
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_OVR);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR);
      /* I2S Overrun error interrupt occurred --------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        __HAL_I2S_CLEAR_OVRFLAG(hi2s);
        hi2s->ErrorCode |= HAL_I2S_ERROR_OVR;
      }

      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_TXE;
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_TXE; 
      /* I2Sext in mode Tramitter --------------------------------------------*/
      if((tmp1 == SPI_SR_TXE) && (tmp2 == I2S_IT_TXE))
      {
        /* When the I2S mode is configured as I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX,
        the I2Sext TXE interrupt will be generated to manage the full-duplex transmit phase. */
        I2SEx_TransmitReceive_IT(hi2s);
      }

      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_UDR;
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_ERR;
      /* I2Sext Underrun error interrupt occurred ----------------------------*/
      if((tmp1 == SPI_SR_UDR) && (tmp2 == I2S_IT_ERR))
      {
        /* Clear I2Sext UDR Flag */ 
        tmpreg1 = I2SxEXT(hi2s->Instance)->SR;
        hi2s->ErrorCode |= HAL_I2SEX_ERROR_UDR;
        UNUSED(tmpreg1);
      }
    }
  }

  /* Call the Error call Back in case of Errors */
  if(hi2s->ErrorCode != HAL_I2S_ERROR_NONE)
  {
    /* Set the I2S state ready to be able to start again the process */
    hi2s->State= HAL_I2S_STATE_READY;
    HAL_I2S_ErrorCallback(hi2s);
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
  uint32_t tmp1 = 0U, tmp2 = 0U;
  
  if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
  {
    /* Process Locked */
    __HAL_LOCK(hi2s);

    tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp1 == I2S_MODE_SLAVE_TX))
    {
      if(hi2s->TxXferCount != 0U)
      {
        if(__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_TXE) != RESET)
        {        
          /* Transmit data */
          hi2s->Instance->DR = (*hi2s->pTxBuffPtr++);
          hi2s->TxXferCount--;

          if(hi2s->TxXferCount == 0U)
          {
            /* Disable TXE interrupt */
            __HAL_I2S_DISABLE_IT(hi2s, I2S_IT_TXE);
          }
        }
      }

      if(hi2s->RxXferCount != 0U)
      {
        if((I2SxEXT(hi2s->Instance)->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
        {
          /* Receive data */
          (*hi2s->pRxBuffPtr++) = I2SxEXT(hi2s->Instance)->DR;
          hi2s->RxXferCount--;

          if(hi2s->RxXferCount == 0U)
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
      if(hi2s->TxXferCount != 0U)
      {
        if((I2SxEXT(hi2s->Instance)->SR & SPI_SR_TXE) == SPI_SR_TXE)
        {        
          /* Transmit data */
          I2SxEXT(hi2s->Instance)->DR = (*hi2s->pTxBuffPtr++);
          hi2s->TxXferCount--;

          if(hi2s->TxXferCount == 0U)
          {
            /* Disable I2Sext TXE interrupt */
            I2SxEXT(hi2s->Instance)->CR2 &= ~I2S_IT_TXE;

            HAL_I2S_TxCpltCallback(hi2s);
          }
        }
      }
      if(hi2s->RxXferCount != 0U)
      {
        if(__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_RXNE) != RESET)
        {
          /* Receive data */
          (*hi2s->pRxBuffPtr++) = hi2s->Instance->DR;
          hi2s->RxXferCount--;

          if(hi2s->RxXferCount == 0U)
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
    if((tmp1 == 0U) && (tmp2 == 0U))
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
#endif /* STM32F40xxx || STM32F41xxx || STM32F42xxx || STM32F43xxx || STM32F401xx ||\
          STM32F411xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || \
          STM32F412Rx || STM32F412Cx */
/**
  * @brief DMA I2S transmit process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMATxCplt(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    if(hi2s->Init.FullDuplexMode != I2S_FULLDUPLEXMODE_ENABLE)
    {    
       hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    }
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F469xx) ||\
    defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) ||\
    defined(STM32F412Cx)
    
    /* FullDuplexMode feature enabled */   
    else
    {
      if(((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_TX) || ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_SLAVE_TX))
      {
        /* Disable Tx DMA Request for the I2S Master*/  
        hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
      }
      else
      {
        /* Disable Tx DMA Request for the I2SEx Slave */  
        I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
      }
    }
#endif /* STM32F40xxx || STM32F41xxx || STM32F42xxx || STM32F43xxx || STM32F401xx || STM32F411xx ||\
          STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */
    hi2s->TxXferCount = 0U;
    if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
    {
      if(hi2s->RxXferCount == 0U)
      {
        hi2s->State = HAL_I2S_STATE_READY;
      }
    }
    else
    {
      hi2s->State = HAL_I2S_STATE_READY; 
    }
  }
  HAL_I2S_TxCpltCallback(hi2s);
}

/**
  * @brief DMA I2S receive process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMARxCplt(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    if(hi2s->Init.FullDuplexMode != I2S_FULLDUPLEXMODE_ENABLE)
    {    
       hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    }
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F469xx) ||\
    defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) ||\
    defined(STM32F412Cx)
   /* FullDuplexMode feature enabled */   
    else
    {
      if(((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_TX) || ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_SLAVE_TX))
      {
        /* Disable Rx DMA Request for the I2SEx Slave */  
        I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
      }
      else
      {
        /* Disable Rx DMA Request for the I2S Master*/  
        hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
      }
    }
#endif /* STM32F40xxx || STM32F41xxx || STM32F42xxx || STM32F43xxx || STM32F401xx || STM32F411xx ||\
          STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */
    hi2s->RxXferCount = 0U;
    if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
    {
      if(hi2s->TxXferCount == 0U)
      {
        hi2s->State = HAL_I2S_STATE_READY;
      }
    }
    else
    {
      hi2s->State = HAL_I2S_STATE_READY; 
    }
  }
  HAL_I2S_RxCpltCallback(hi2s); 
}

/**
  * @brief  Get I2S clock Input based on Source clock selection in RCC
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval I2S Clock Input
  */
uint32_t I2S_GetInputClock(I2S_HandleTypeDef *hi2s)
{
  /* This variable used to store the VCO Input (value in Hz) */
  uint32_t vcoinput = 0U;
  /* This variable used to store the VCO Output (value in Hz) */
  uint32_t vcooutput = 0U;
  /* This variable used to store the I2S_CK_x (value in Hz) */
  uint32_t i2ssourceclock = 0U;

  /* Configure 12S Clock based on I2S source clock selection */ 
#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)
  switch(hi2s->Init.ClockSource)
  {
    case I2S_CLOCK_EXTERNAL :
    {
      /* Set the I2S clock to the external clock  value */
      i2ssourceclock = EXTERNAL_CLOCK_VALUE;
      break;
    }
#if defined(STM32F446xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)
    case I2S_CLOCK_PLL :
    { 
      /* Configure the PLLI2S division factor */
      /* PLLI2S_VCO Input  = PLL_SOURCE/PLLI2SM */
      if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
      {
        /* Get the I2S source clock value */
        vcoinput = (uint32_t)(HSE_VALUE / (uint32_t)(RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SM));
      }
      else
      {
        /* Get the I2S source clock value */
        vcoinput = (uint32_t)(HSI_VALUE / (uint32_t)(RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SM));
      }
      
      /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
      vcooutput = (uint32_t)(vcoinput * (((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6U) & (RCC_PLLI2SCFGR_PLLI2SN >> 6U)));
      /* I2S_CLK = PLLI2S_VCO Output/PLLI2SR */
      i2ssourceclock = (uint32_t)(vcooutput /(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28U) & (RCC_PLLI2SCFGR_PLLI2SR >> 28U)));
      break;
    }
#endif /* STM32F446xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */
    case I2S_CLOCK_PLLR :
    { 
      /* Configure the PLLI2S division factor */
      /* PLL_VCO Input  = PLL_SOURCE/PLLM */
      if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
      {
        /* Get the I2S source clock value */
        vcoinput = (uint32_t)(HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
      }
      else
      {
        /* Get the I2S source clock value */
        vcoinput = (uint32_t)(HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
      }
      
      /* PLL_VCO Output = PLL_VCO Input * PLLN */
      vcooutput = (uint32_t)(vcoinput * (((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6U) & (RCC_PLLCFGR_PLLN >> 6U)));
      /* I2S_CLK = PLLI2S_VCO Output/PLLI2SR */
      i2ssourceclock = (uint32_t)(vcooutput /(((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 28U) & (RCC_PLLCFGR_PLLR >> 28U)));
      break;
    }
    case I2S_CLOCK_PLLSRC :
    { 
      /* Configure the PLLI2S division factor */
      /* PLL_VCO Input  = PLL_SOURCE/PLLM */
      if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
      {
        /* Get the I2S source clock value */
        i2ssourceclock = (uint32_t)(HSE_VALUE);
      }
      else
      {
        /* Get the I2S source clock value */
        i2ssourceclock = (uint32_t)(HSI_VALUE);
      }
      break;
    }
    default :
    {
      break;
    }
  }
#endif /* STM32F410xx || STM32F446xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F469xx) || defined(STM32F479xx)
      
  /* If an external I2S clock has to be used, the specific define should be set  
  in the project configuration or in the stm32f4xx_conf.h file */
  if(hi2s->Init.ClockSource == I2S_CLOCK_EXTERNAL)
  {
    /* Set the I2S clock to the external clock  value */
    i2ssourceclock = EXTERNAL_CLOCK_VALUE;
  }
  else
  { 
    /* Configure the PLLI2S division factor */
    /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
    if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
    {
      /* Get the I2S source clock value */
      vcoinput = (uint32_t)(HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
    }
    else
    {
      /* Get the I2S source clock value */
      vcoinput = (uint32_t)(HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
    }
    
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
    vcooutput = (uint32_t)(vcoinput * (((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6U) & (RCC_PLLI2SCFGR_PLLI2SN >> 6U)));
    /* I2S_CLK = PLLI2S_VCO Output/PLLI2SR */
    i2ssourceclock = (uint32_t)(vcooutput /(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28U) & (RCC_PLLI2SCFGR_PLLI2SR >> 28U)));
  }
#endif /* STM32F40xxx || STM32F41xxx || STM32F42xxx || STM32F43xxx || STM32F469xx || STM32F479xx */

#if defined(STM32F411xE)
      
  /* If an external I2S clock has to be used, the specific define should be set  
  in the project configuration or in the stm32f4xx_conf.h file */
  if(hi2s->Init.ClockSource == I2S_CLOCK_EXTERNAL)
  {
    /* Set the I2S clock to the external clock  value */
    i2ssourceclock = EXTERNAL_CLOCK_VALUE;
  }
  else
  { 
    /* Configure the PLLI2S division factor */
    /* PLLI2S_VCO Input  = PLL_SOURCE/PLLI2SM */
    if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
    {
      /* Get the I2S source clock value */
      vcoinput = (uint32_t)(HSE_VALUE / (uint32_t)(RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SM));
    }
    else
    {
      /* Get the I2S source clock value */
      vcoinput = (uint32_t)(HSI_VALUE / (uint32_t)(RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SM));
    }
    
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
    vcooutput = (uint32_t)(vcoinput * (((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6U) & (RCC_PLLI2SCFGR_PLLI2SN >> 6U)));
    /* I2S_CLK = PLLI2S_VCO Output/PLLI2SR */
    i2ssourceclock = (uint32_t)(vcooutput /(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28U) & (RCC_PLLI2SCFGR_PLLI2SR >> 28U)));
  }
#endif /* STM32F411xE */

  /* the return result is the value of I2S clock */
  return i2ssourceclock; 
}
/**
  * @}
  */
  
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
