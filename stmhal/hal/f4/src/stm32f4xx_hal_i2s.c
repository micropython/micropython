/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2s.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   I2S HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Integrated Interchip Sound (I2S) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
  @verbatim
 ===============================================================================
                  ##### How to use this driver #####
 ===============================================================================
 [..]
    The I2S HAL driver can be used as follow:
    
    (#) Declare a I2S_HandleTypeDef handle structure.
    (#) Initialize the I2S low level resources by implement the HAL_I2S_MspInit() API:
        (##) Enable the SPIx interface clock.                      
        (##) I2S pins configuration:
            (+++) Enable the clock for the I2S GPIOs.
            (+++) Configure these I2S pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_I2S_Transmit_IT()
             and HAL_I2S_Receive_IT() APIs).
            (+++) Configure the I2Sx interrupt priority.
            (+++) Enable the NVIC I2S IRQ handle.
        (##) DMA Configuration if you need to use DMA process (HAL_I2S_Transmit_DMA()
             and HAL_I2S_Receive_DMA() APIs:
            (+++) Declare a DMA handle structure for the Tx/Rx stream.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
            (+++) Configure the DMA Tx/Rx Stream.
            (+++) Associate the initilalized DMA handle to the I2S DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the 
                DMA Tx/Rx Stream.
  
   (#) Program the Mode, Standard, Data Format, MCLK Output, Audio frequency and Polarity
       using HAL_I2S_Init() function.

   -@- The specific I2S interrupts (Transmission complete interrupt, 
       RXNE interrupt and Error Interrupts) will be managed using the macros
       __I2S_ENABLE_IT() and __I2S_DISABLE_IT() inside the transmit and receive process.
   -@- Make sure that either:
        (+@) I2S PLL is configured or 
        (+@) External clock source is configured after setting correctly 
             the define constant EXTERNAL_CLOCK_VALUE in the stm32f4xx_hal_conf.h file. 
             
   (#) Three operation modes are available within this driver :
  
   *** Polling mode IO operation ***
   =================================
   [..]    
     (+) Send an amount of data in blocking mode using HAL_I2S_Transmit() 
     (+) Receive an amount of data in blocking mode using HAL_I2S_Receive()
   
   *** Interrupt mode IO operation ***
   ===================================
   [..]    
     (+) Send an amount of data in non blocking mode using HAL_I2S_Transmit_IT() 
     (+) At transmission end of half transfer HAL_I2S_TxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxHalfCpltCallback 
     (+) At transmission end of transfer HAL_I2S_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxCpltCallback
     (+) Receive an amount of data in non blocking mode using HAL_I2S_Receive_IT() 
     (+) At reception end of half transfer HAL_I2S_RxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxHalfCpltCallback 
     (+) At reception end of transfer HAL_I2S_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxCpltCallback
     (+) In case of transfer Error, HAL_I2S_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_I2S_ErrorCallback

   *** DMA mode IO operation ***
   ==============================
   [..] 
     (+) Send an amount of data in non blocking mode (DMA) using HAL_I2S_Transmit_DMA() 
     (+) At transmission end of half transfer HAL_I2S_TxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxHalfCpltCallback 
     (+) At transmission end of transfer HAL_I2S_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_TxCpltCallback
     (+) Receive an amount of data in non blocking mode (DMA) using HAL_I2S_Receive_DMA() 
     (+) At reception end of half transfer HAL_I2S_RxHalfCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxHalfCpltCallback 
     (+) At reception end of transfer HAL_I2S_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_I2S_RxCpltCallback
     (+) In case of transfer Error, HAL_I2S_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_I2S_ErrorCallback
     (+) Pause the DMA Transfer using HAL_I2S_DMAPause()
     (+) Resume the DMA Transfer using HAL_I2S_DMAResume()
     (+) Stop the DMA Transfer using HAL_I2S_DMAStop()

   *** I2S HAL driver macros list ***
   =============================================
   [..]
     Below the list of most used macros in USART HAL driver.
       
      (+) __HAL_I2S_ENABLE: Enable the specified SPI peripheral (in I2S mode) 
      (+) __HAL_I2S_DISABLE: Disable the specified SPI peripheral (in I2S mode)
      (+) __HAL_I2S_ENABLE_IT : Enable the specified I2S interrupts
      (+) __HAL_I2S_DISABLE_IT : Disable the specified I2S interrupts
      (+) __HAL_I2S_GET_FLAG: Check whether the specified I2S flag is set or not

    [..]
      (@) You can refer to the I2S HAL driver header file for more useful macros

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

/** @defgroup I2S 
  * @brief I2S HAL module driver
  * @{
  */

#ifdef HAL_I2S_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef I2S_Transmit_IT(I2S_HandleTypeDef *hi2s);
static HAL_StatusTypeDef I2S_Receive_IT(I2S_HandleTypeDef *hi2s);
/* Private functions ---------------------------------------------------------*/

/** @defgroup I2S_Private_Functions
  * @{
  */

/** @defgroup  I2S_Group1 Initialization and de-initialization functions 
  *  @brief    Initialization and Configuration functions 
  *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and 
          de-initialiaze the I2Sx peripheral in simplex mode:

      (+) User must Implement HAL_I2S_MspInit() function in which he configures 
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_I2S_Init() to configure the selected device with 
          the selected configuration:
        (++) Mode
        (++) Standard 
        (++) Data Format
        (++) MCLK Output
        (++) Audio frequency
        (++) Polarity
        (++) Full duplex mode

      (+) Call the function HAL_I2S_DeInit() to restore the default configuration 
          of the selected I2Sx periperal. 
@endverbatim
  * @{
  */

/**
  * @brief Initializes the I2S according to the specified parameters 
  *         in the I2S_InitTypeDef and create the associated handle.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s)
{
  uint32_t tmpreg = 0, i2sdiv = 2, i2sodd = 0, packetlength = 1;
  uint32_t tmp = 0, i2sclk = 0;
  
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
  assert_param(IS_I2S_FULLDUPLEX_MODE(hi2s->Init.FullDuplexMode));
  
  if(hi2s->State == HAL_I2S_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_I2S_MspInit(hi2s);
  }
  
  hi2s->State = HAL_I2S_STATE_BUSY;
  
  /*----------------------- SPIx I2SCFGR & I2SPR Configuration ---------------*/
  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
  hi2s->Instance->I2SCFGR &= ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | \
                               SPI_I2SCFGR_I2SSTD | SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | \
                               SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD); 
  hi2s->Instance->I2SPR = 0x0002;

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
      packetlength = 2;
    }

    /* Get I2S source Clock frequency  ****************************************/
    /* If an external I2S clock has to be used, the specific define should be set  
    in the project configuration or in the stm32f4xx_conf.h file */
    if(hi2s->Init.ClockSource == I2S_CLOCK_EXTERNAL)
    {
      /* Set external clock as I2S clock source */
      if((RCC->CFGR & RCC_CFGR_I2SSRC) == 0)
      {
        RCC->CFGR |= (uint32_t)RCC_CFGR_I2SSRC;
      }

      /* Set the I2S clock to the external clock  value */
      i2sclk = EXTERNAL_CLOCK_VALUE;
    }
    else
    { 
      /* Check if PLLI2S is enabled or Not */
      if((RCC->CR & RCC_CR_PLLI2SON) != RCC_CR_PLLI2SON)
      {
        hi2s->State= HAL_I2S_STATE_READY;
        
        return HAL_ERROR;
      }

      /* Set PLLI2S as I2S clock source */
      if((RCC->CFGR & RCC_CFGR_I2SSRC) != 0)
      {
        RCC->CFGR &= ~(uint32_t)RCC_CFGR_I2SSRC;
      }

      /* Get the PLLM value */
      if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
      {
        /* Get the I2S source clock value */
        i2sclk = (uint32_t)(HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
      }
      else
      {
        /* Get the I2S source clock value */
        i2sclk = (uint32_t)(HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
      }
      i2sclk *= (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6) & (RCC_PLLI2SCFGR_PLLI2SN >> 6));
      i2sclk /= (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28) & (RCC_PLLI2SCFGR_PLLI2SR >> 28));
    }

    /* Compute the Real divider depending on the MCLK output state, with a floating point */
    if(hi2s->Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE)
    {
      /* MCLK output is enabled */
      tmp = (uint32_t)(((((i2sclk / 256) * 10) / hi2s->Init.AudioFreq)) + 5);
    }
    else
    {
      /* MCLK output is disabled */
      tmp = (uint32_t)(((((i2sclk / (32 * packetlength)) *10 ) / hi2s->Init.AudioFreq)) + 5);
    }

    /* Remove the flatting point */
    tmp = tmp / 10;  

    /* Check the parity of the divider */
    i2sodd = (uint32_t)(tmp & (uint32_t)1);

    /* Compute the i2sdiv prescaler */
    i2sdiv = (uint32_t)((tmp - i2sodd) / 2);

    /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
    i2sodd = (uint32_t) (i2sodd << 8);
  }

  /* Test if the divider is 1 or 0 or greater than 0xFF */
  if((i2sdiv < 2) || (i2sdiv > 0xFF))
  {
    /* Set the default values */
    i2sdiv = 2;
    i2sodd = 0;
  }
  
  /* Write to SPIx I2SPR register the computed value */
  hi2s->Instance->I2SPR = (uint32_t)((uint32_t)i2sdiv | (uint32_t)(i2sodd | (uint32_t)hi2s->Init.MCLKOutput));
  
  /* Configure the I2S with the I2S_InitStruct values */
  tmpreg |= (uint32_t)(SPI_I2SCFGR_I2SMOD | hi2s->Init.Mode | hi2s->Init.Standard | hi2s->Init.DataFormat | hi2s->Init.CPOL);
  
  /* Write to SPIx I2SCFGR */  
  hi2s->Instance->I2SCFGR = tmpreg;
  
  /* Configure the I2S extended if the full duplex mode is enabled */
  if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
  {    
    /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
    I2SxEXT(hi2s->Instance)->I2SCFGR &= ~(SPI_I2SCFGR_CHLEN | SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CKPOL | \
                                          SPI_I2SCFGR_I2SSTD | SPI_I2SCFGR_PCMSYNC | SPI_I2SCFGR_I2SCFG | \
                                          SPI_I2SCFGR_I2SE | SPI_I2SCFGR_I2SMOD);
    I2SxEXT(hi2s->Instance)->I2SPR = 2;
    
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
  * @brief DeInitializes the I2S peripheral 
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s)
{
  /* Check the I2S handle allocation */
  if(hi2s == NULL)
  {
    return HAL_ERROR;
  }
  
  hi2s->State = HAL_I2S_STATE_BUSY;
  
  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  HAL_I2S_MspDeInit(hi2s);

  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->State = HAL_I2S_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hi2s);

  return HAL_OK;
}

/**
  * @brief I2S MSP Init
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
 __weak void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_MspInit could be implenetd in the user file
   */ 
}

/**
  * @brief I2S MSP DeInit
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
 __weak void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_MspDeInit could be implenetd in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup I2S_Group2 IO operation functions 
  *  @brief Data transfers functions 
  *
@verbatim   
 ===============================================================================
                      ##### IO operation functions #####
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
        (++) HAL_I2S_Transmit()
        (++) HAL_I2S_Receive()
        
    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2S_Transmit_IT()
        (++) HAL_I2S_Receive_IT()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_I2S_Transmit_DMA()
        (++) HAL_I2S_Receive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2S_TxCpltCallback()
        (++) HAL_I2S_RxCpltCallback()
        (++) HAL_I2S_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief Transmit an amount of data in blocking mode
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @param  Timeout: Timeout duration
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tmp1 = 0, tmp2 = 0;  
  if((pData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }
  
  if(hi2s->State == HAL_I2S_STATE_READY)
  { 
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
       (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
    }
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
    }
    
    /* Process Locked */
    __HAL_LOCK(hi2s);
    
    hi2s->State = HAL_I2S_STATE_BUSY_TX;
   
    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
    }
    
    while(hi2s->TxXferCount > 0)
    {
      hi2s->Instance->DR = (*pData++);
      hi2s->TxXferCount--;   
      /* Wait until TXE flag is set */
      if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_TXE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
    } 
    /* Wait until Busy flag is reset */
    if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_BSY, SET, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

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
  * @brief Receive an amount of data in blocking mode 
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @param Timeout: Timeout duration
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @note In I2S Master Receiver mode, just after enabling the peripheral the clock will be generate
  *       in continouse way and as the I2S is not disabled at the end of the I2S transaction.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tmp1 = 0, tmp2 = 0;   
  if((pData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }
  
  if(hi2s->State == HAL_I2S_STATE_READY)
  { 
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
       (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }
    else
    {
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }
    /* Process Locked */
    __HAL_LOCK(hi2s);

    hi2s->State = HAL_I2S_STATE_BUSY_RX;

    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
    }

    /* Check if Master Receiver mode is selected */
    if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX)
    {
      /* Clear the Overrun Flag by a read operation on the SPI_DR register followed by a read
      access to the SPI_SR register. */ 
      __HAL_I2S_CLEAR_OVRFLAG(hi2s);
    }

    /* Receive data */
    while(hi2s->RxXferCount > 0)
    {
      /* Wait until RXNE flag is set */
      if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_RXNE, RESET, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      (*pData++) = hi2s->Instance->DR;
      hi2s->RxXferCount--;
    }

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
  * @brief Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t tmp1 = 0, tmp2 = 0;     
  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    hi2s->pTxBuffPtr = pData;
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
      (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
    }
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
    }

    /* Process Locked */
    __HAL_LOCK(hi2s);

    hi2s->State = HAL_I2S_STATE_BUSY_TX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;

    /* Enable TXE and ERR interrupt */
    __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
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
  * @brief Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to the Receive data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @note It is recommended to use DMA for the I2S receiver to avoid de-synchronisation 
  * between Master and Slave otherwise the I2S interrupt should be optimized. 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t tmp1 = 0, tmp2 = 0;     
  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }

    hi2s->pRxBuffPtr = pData;
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)||\
      (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }
    else
    {
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }
    /* Process Locked */
    __HAL_LOCK(hi2s);
    
    hi2s->State = HAL_I2S_STATE_BUSY_RX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
    
    /* Enable TXE and ERR interrupt */
    __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_RXNE | I2S_IT_ERR));
    
    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
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
  * @brief Transmit an amount of data in non-blocking mode with DMA
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to the Transmit data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0, tmp2 = 0;     
  
  if((pData == NULL) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }

  if(hi2s->State == HAL_I2S_STATE_READY)
  {  
    hi2s->pTxBuffPtr = pData;
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
      (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->TxXferSize = Size*2;
      hi2s->TxXferCount = Size*2;
    }
    else
    {
      hi2s->TxXferSize = Size;
      hi2s->TxXferCount = Size;
    }

    /* Process Locked */
    __HAL_LOCK(hi2s);

    hi2s->State = HAL_I2S_STATE_BUSY_TX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;

    /* Set the I2S Tx DMA Half transfert complete callback */
    hi2s->hdmatx->XferHalfCpltCallback = I2S_DMATxHalfCplt;

    /* Set the I2S Tx DMA transfert complete callback */
    hi2s->hdmatx->XferCpltCallback = I2S_DMATxCplt;

    /* Set the DMA error callback */
    hi2s->hdmatx->XferErrorCallback = I2S_DMAError;

    /* Enable the Tx DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hi2s->hdmatx, *(uint32_t*)tmp, (uint32_t)&hi2s->Instance->DR, hi2s->TxXferSize);

    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
    }

     /* Check if the I2S Tx request is already enabled */ 
    if((hi2s->Instance->CR2 & SPI_CR2_TXDMAEN) != SPI_CR2_TXDMAEN)
    {
      /* Enable Tx DMA Request */  
      hi2s->Instance->CR2 |= SPI_CR2_TXDMAEN;
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
  * @brief Receive an amount of data in non-blocking mode with DMA 
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param pData: a 16-bit pointer to the Receive data buffer.
  * @param Size: number of data sample to be sent:
  * @note When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *       configuration phase, the Size parameter means the number of 16-bit data length 
  *       in the transaction and when a 24-bit data frame or a 32-bit data frame is selected 
  *       the Size parameter means the number of 16-bit data length. 
  * @note The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization 
  *       between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  uint32_t tmp1 = 0, tmp2 = 0;  
  
  if((pData == NULL) || (Size == 0))
  {
    return  HAL_ERROR;
  }

  if(hi2s->State == HAL_I2S_STATE_READY)
  {
    hi2s->pRxBuffPtr = pData;
    tmp1 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    tmp2 = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN | SPI_I2SCFGR_CHLEN);
    if((tmp1 == I2S_DATAFORMAT_24B)|| \
      (tmp2 == I2S_DATAFORMAT_32B))
    {
      hi2s->RxXferSize = Size*2;
      hi2s->RxXferCount = Size*2;
    }
    else
    {
      hi2s->RxXferSize = Size;
      hi2s->RxXferCount = Size;
    }
    /* Process Locked */
    __HAL_LOCK(hi2s);
    
    hi2s->State = HAL_I2S_STATE_BUSY_RX;
    hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
    
    /* Set the I2S Rx DMA Half transfert complete callback */
    hi2s->hdmarx->XferHalfCpltCallback = I2S_DMARxHalfCplt;
    
    /* Set the I2S Rx DMA transfert complete callback */
    hi2s->hdmarx->XferCpltCallback = I2S_DMARxCplt;
    
    /* Set the DMA error callback */
    hi2s->hdmarx->XferErrorCallback = I2S_DMAError;
    
    /* Check if Master Receiver mode is selected */
    if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX)
    {
      /* Clear the Overrun Flag by a read operation to the SPI_DR register followed by a read
      access to the SPI_SR register. */ 
      __HAL_I2S_CLEAR_OVRFLAG(hi2s);
    }
    
    /* Enable the Rx DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hi2s->hdmarx, (uint32_t)&hi2s->Instance->DR, *(uint32_t*)tmp, hi2s->RxXferSize);
    
    /* Check if the I2S is already enabled */ 
    if((hi2s->Instance->I2SCFGR &SPI_I2SCFGR_I2SE) != SPI_I2SCFGR_I2SE)
    {
      /* Enable I2S peripheral */
      __HAL_I2S_ENABLE(hi2s);
    }

     /* Check if the I2S Rx request is already enabled */ 
    if((hi2s->Instance->CR2 &SPI_CR2_RXDMAEN) != SPI_CR2_RXDMAEN)
    {
      /* Enable Rx DMA Request */  
      hi2s->Instance->CR2 |= SPI_CR2_RXDMAEN;
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
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE) == 0)
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
  uint32_t tmp1 = 0, tmp2 = 0;    
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
    tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
    /* Check if the I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX Mode is selected */
    if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
    { 
      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_RXNE; 
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_RXNE;  
      /* I2Sext in mode Receiver ---------------------------------------------*/
      if((tmp1 == SPI_SR_RXNE) && (tmp2 == I2S_IT_RXNE))
      {
        tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        /* When the I2S mode is configured as I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX,
        the I2Sext RXNE interrupt will be generated to manage the full-duplex receive phase. */
        if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
        {
          I2SEx_TransmitReceive_IT(hi2s);
        }
      }

      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_OVR;
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_ERR;
      /* I2Sext Overrun error interrupt occurred ------------------------------*/
      if((tmp1 == SPI_SR_OVR) && (tmp2 == I2S_IT_ERR))
      {
        /* Clear I2Sext OVR Flag */ 
        I2SxEXT(hi2s->Instance)->DR;
        I2SxEXT(hi2s->Instance)->SR;
        hi2s->ErrorCode |= HAL_I2SEX_ERROR_OVR;
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_TXE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_TXE);
      /* I2S in mode Tramitter -----------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        /* When the I2S mode is configured as I2S_MODE_MASTER_TX or I2S_MODE_SLAVE_TX,
        the I2S TXE interrupt will be generated to manage the full-duplex transmit phase. */
        if((tmp1 == I2S_MODE_MASTER_TX) || (tmp2 == I2S_MODE_SLAVE_TX))
        {
          I2SEx_TransmitReceive_IT(hi2s);
        }
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
    /* The I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX Mode is selected */
    else
    {
      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_RXNE);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_RXNE);
      /* I2S in mode Receiver ------------------------------------------------*/
      if((tmp1 != RESET) && (tmp2 != RESET))
      {
        tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        /* When the I2S mode is configured as I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX,
        the I2S RXNE interrupt will be generated to manage the full-duplex receive phase. */
        if((tmp1 == I2S_MODE_MASTER_RX) || (tmp2 == I2S_MODE_SLAVE_RX))
        {
          I2SEx_TransmitReceive_IT(hi2s);
        }
      }

      tmp1 = __HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_OVR);
      tmp2 = __HAL_I2S_GET_IT_SOURCE(hi2s, I2S_IT_ERR);
      /* I2S Overrun error interrupt occurred ---------------------------------*/
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
        tmp1 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        tmp2 = hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG;
        /* When the I2S mode is configured as I2S_MODE_MASTER_RX or I2S_MODE_SLAVE_RX,
        the I2Sext TXE interrupt will be generated to manage the full-duplex transmit phase. */
        if((tmp1 == I2S_MODE_MASTER_RX) || (tmp2 == I2S_MODE_SLAVE_RX))
        {
          I2SEx_TransmitReceive_IT(hi2s);
        }
      }

      tmp1 = I2SxEXT(hi2s->Instance)->SR & SPI_SR_UDR;
      tmp2 = I2SxEXT(hi2s->Instance)->CR2 & I2S_IT_ERR;
      /* I2Sext Underrun error interrupt occurred -----------------------------*/
      if((tmp1 == SPI_SR_UDR) && (tmp2 == I2S_IT_ERR))
      {
        /* Clear I2Sext UDR Flag */ 
        I2SxEXT(hi2s->Instance)->SR;
        hi2s->ErrorCode |= HAL_I2SEX_ERROR_UDR;
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
  * @brief Tx Transfer Half completed callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
 __weak void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxHalfCpltCallback could be implenetd in the user file
   */ 
}

/**
  * @brief Tx Transfer completed callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
 __weak void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxCpltCallback could be implenetd in the user file
   */ 
}

/**
  * @brief Rx Transfer half completed callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxCpltCallback could be implenetd in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxCpltCallback could be implenetd in the user file
   */
}

/**
  * @brief I2S error callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
 __weak void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_ErrorCallback could be implenetd in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup I2S_Group3 Peripheral State and Errors functions 
  *  @brief   Peripheral State functions 
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
  * @brief  Return the I2S state
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL state
  */
HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s)
{
  return hi2s->State;
}

/**
  * @brief  Return the I2S error code
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval I2S Error Code
  */
HAL_I2S_ErrorTypeDef HAL_I2S_GetError(I2S_HandleTypeDef *hi2s)
{
  return hi2s->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief DMA I2S transmit process complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMATxCplt(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    hi2s->TxXferCount = 0;

    /* Disable Tx DMA Request */
    hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    
    if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
    {
      /* Disable Rx DMA Request for the slave*/  
      I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);
    }

    if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
    {
      if(hi2s->RxXferCount == 0)
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
  * @brief DMA I2S transmit process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_I2S_TxHalfCpltCallback(hi2s);
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

  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* Disable Rx DMA Request */
    hi2s->Instance->CR2 &= (uint32_t)(~SPI_CR2_RXDMAEN);

    if(hi2s->Init.FullDuplexMode == I2S_FULLDUPLEXMODE_ENABLE)
    {
      /* Disable Tx DMA Request for the slave*/  
      I2SxEXT(hi2s->Instance)->CR2 &= (uint32_t)(~SPI_CR2_TXDMAEN);
    }

    hi2s->RxXferCount = 0;
    if(hi2s->State == HAL_I2S_STATE_BUSY_TX_RX)
    {
      if(hi2s->TxXferCount == 0)
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
  * @brief DMA I2S receive process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_I2S_RxHalfCpltCallback(hi2s); 
}

/**
  * @brief DMA I2S communication error callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void I2S_DMAError(DMA_HandleTypeDef *hdma)
{
  I2S_HandleTypeDef* hi2s = (I2S_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  hi2s->TxXferCount = 0;
  hi2s->RxXferCount = 0;

  hi2s->State= HAL_I2S_STATE_READY;

  hi2s->ErrorCode |= HAL_I2S_ERROR_DMA;
  HAL_I2S_ErrorCallback(hi2s);
}

/**
  * @brief Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2S_Transmit_IT(I2S_HandleTypeDef *hi2s)
{
 if(hi2s->State == HAL_I2S_STATE_BUSY_TX)
  {
    /* Process Locked */
    __HAL_LOCK(hi2s);

    /* Transmit data */
    hi2s->Instance->DR = (*hi2s->pTxBuffPtr++);

    hi2s->TxXferCount--;	
    
    if(hi2s->TxXferCount == 0)
    {
      /* Disable TXE and ERR interrupt */
      __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));
      
      hi2s->State = HAL_I2S_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2s);
      HAL_I2S_TxCpltCallback(hi2s);
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2s);
    }

    return HAL_OK;
  }
  
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
static HAL_StatusTypeDef I2S_Receive_IT(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->State == HAL_I2S_STATE_BUSY_RX)
  {
    /* Process Locked */
    __HAL_LOCK(hi2s);

    /* Receive data */
    (*hi2s->pRxBuffPtr++) = hi2s->Instance->DR;

    hi2s->RxXferCount--;

    /* Check if Master Receiver mode is selected */
    if((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG) == I2S_MODE_MASTER_RX)
    {
      /* Clear the Overrun Flag by a read operation on the SPI_DR register followed by a read
      access to the SPI_SR register. */ 
      __HAL_I2S_CLEAR_OVRFLAG(hi2s);
    }

    if(hi2s->RxXferCount == 0)
    {
      /* Disable RXNE and ERR interrupt */
      __HAL_I2S_DISABLE_IT(hi2s, I2S_IT_RXNE | I2S_IT_ERR);

      hi2s->State = HAL_I2S_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2s);

      HAL_I2S_RxCpltCallback(hi2s);
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hi2s);
    }

    return HAL_OK; 
  }
  else
  {
    return HAL_BUSY; 
  } 
}

/**
  * @brief This function handles I2S Communication Timeout.
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param Flag: Flag checked
  * @param State: Value of the flag expected
  * @param Timeout: Duration of the timeout
  * @retval HAL status
  */
HAL_StatusTypeDef I2S_WaitFlagStateUntilTimeout(I2S_HandleTypeDef *hi2s, uint32_t Flag, uint32_t Status, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_I2S_GET_FLAG(hi2s, Flag) == RESET)
    {
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Set the I2S State ready */
          hi2s->State= HAL_I2S_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2s);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_I2S_GET_FLAG(hi2s, Flag) != RESET)
    {
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Set the I2S State ready */
          hi2s->State= HAL_I2S_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2s);

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
  
#endif /* HAL_I2S_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
