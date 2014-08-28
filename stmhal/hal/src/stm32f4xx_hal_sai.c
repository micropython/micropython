/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sai.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   SAI HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Serial Audio Interface (SAI) peripheral:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
 ==============================================================================
                  ##### How to use this driver #####
  ==============================================================================
           
  [..]
    The SAI HAL driver can be used as follows:
    
    (#) Declare a SAI_HandleTypeDef handle structure.
    (#) Initialize the SAI low level resources by implementing the HAL_SAI_MspInit() API:
        (##) Enable the SAI interface clock.                      
        (##) SAI pins configuration:
            (+++) Enable the clock for the SAI GPIOs.
            (+++) Configure these SAI pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_SAI_Transmit_IT()
             and HAL_SAI_Receive_IT() APIs):
            (+++) Configure the SAI interrupt priority.
            (+++) Enable the NVIC SAI IRQ handle.

        (##) DMA Configuration if you need to use DMA process (HAL_SAI_Transmit_DMA()
             and HAL_SAI_Receive_DMA() APIs):
            (+++) Declare a DMA handle structure for the Tx/Rx stream.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.                
            (+++) Configure the DMA Tx/Rx Stream.
            (+++) Associate the initialized DMA handle to the SAI DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the 
                DMA Tx/Rx Stream.
  
   (#) Program the SAI Mode, Standard, Data Format, MCLK Output, Audio frequency and Polarity
       using HAL_SAI_Init() function.
   
   -@- The specific SAI interrupts (FIFO request and Overrun underrun interrupt)
       will be managed using the macros __SAI_ENABLE_IT() and __SAI_DISABLE_IT()
       inside the transmit and receive process.   
       
  [..]           
   (@) Make sure that either:
       (+@) I2S PLL is configured or 
       (+@) SAI PLL is configured or 
       (+@) External clock source is configured after setting correctly 
            the define constant EXTERNAL_CLOCK_VALUE in the stm32f4xx_hal_conf.h file. 
                        
  [..]           
    (@) In master Tx mode: enabling the audio block immediately generates the bit clock 
        for the external slaves even if there is no data in the FIFO, However FS signal 
        generation is conditioned by the presence of data in the FIFO.
                 
  [..]           
    (@) In master Rx mode: enabling the audio block immediately generates the bit clock 
        and FS signal for the external slaves. 
                
  [..]           
    (@) It is mandatory to respect the following conditions in order to avoid bad SAI behavior: 
        (+@)  First bit Offset <= (SLOT size - Data size)
        (+@)  Data size <= SLOT size
        (+@)  Number of SLOT x SLOT size = Frame length
        (+@)  The number of slots should be even when SAI_FS_CHANNEL_IDENTIFICATION is selected.  

  [..]         
     Three operation modes are available within this driver :     
  
   *** Polling mode IO operation ***
   =================================
   [..]    
     (+) Send an amount of data in blocking mode using HAL_SAI_Transmit() 
     (+) Receive an amount of data in blocking mode using HAL_SAI_Receive()
   
   *** Interrupt mode IO operation ***    
   ===================================
   [..]    
     (+) Send an amount of data in non blocking mode using HAL_SAI_Transmit_IT() 
     (+) At transmission end of transfer HAL_SAI_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_SAI_TxCpltCallback
     (+) Receive an amount of data in non blocking mode using HAL_SAI_Receive_IT() 
     (+) At reception end of transfer HAL_SAI_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_SAI_RxCpltCallback                                      
     (+) In case of transfer Error, HAL_SAI_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_SAI_ErrorCallback

   *** DMA mode IO operation ***    
   ==============================
   [..] 
     (+) Send an amount of data in non blocking mode (DMA) using HAL_SAI_Transmit_DMA() 
     (+) At transmission end of transfer HAL_SAI_TxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_SAI_TxCpltCallback
     (+) Receive an amount of data in non blocking mode (DMA) using HAL_SAI_Receive_DMA() 
     (+) At reception end of transfer HAL_SAI_RxCpltCallback is executed and user can 
         add his own code by customization of function pointer HAL_SAI_RxCpltCallback                                  
     (+) In case of transfer Error, HAL_SAI_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_SAI_ErrorCallback
     (+) Pause the DMA Transfer using HAL_SAI_DMAPause()      
     (+) Resume the DMA Transfer using HAL_SAI_DMAResume()  
     (+) Stop the DMA Transfer using HAL_SAI_DMAStop()      
   
   *** SAI HAL driver macros list ***
   ============================================= 
   [..]
     Below the list of most used macros in USART HAL driver :
       
      (+) __HAL_SAI_ENABLE: Enable the SAI peripheral
      (+) __HAL_SAI_DISABLE: Disable the SAI peripheral
      (+) __HAL_SAI_ENABLE_IT : Enable the specified SAI interrupts
      (+) __HAL_SAI_DISABLE_IT : Disable the specified SAI interrupts
      (+) __HAL_SAI_GET_IT_SOURCE: Check if the specified SAI interrupt source is 
          enabled or disabled
      (+) __HAL_SAI_GET_FLAG: Check whether the specified SAI flag is set or not
  
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

/** @defgroup SAI 
  * @brief SAI HAL module driver
  * @{
  */

#ifdef HAL_SAI_MODULE_ENABLED

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* SAI registers Masks */
#define CR1_CLEAR_MASK            ((uint32_t)0xFF07C010)
#define FRCR_CLEAR_MASK           ((uint32_t)0xFFF88000)
#define SLOTR_CLEAR_MASK          ((uint32_t)0x0000F020)

#define SAI_TIMEOUT_VALUE         10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SAI_DMATxCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMARxCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMAError(DMA_HandleTypeDef *hdma);

/* Private functions ---------------------------------------------------------*/

/** @defgroup SAI_Private_Functions
  * @{
  */

/** @defgroup SAI_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and 
          de-initialize the SAIx peripheral:

      (+) User must implement HAL_SAI_MspInit() function in which he configures 
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_SAI_Init() to configure the selected device with 
          the selected configuration:
        (++) Mode (Master/slave TX/RX)
        (++) Protocol 
        (++) Data Size
        (++) MCLK Output
        (++) Audio frequency
        (++) FIFO Threshold
        (++) Frame Config
        (++) Slot Config

      (+) Call the function HAL_SAI_DeInit() to restore the default configuration 
          of the selected SAI peripheral.     

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the SAI according to the specified parameters 
  *         in the SAI_InitTypeDef and create the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Init(SAI_HandleTypeDef *hsai)
{ 
  uint32_t tmpreg = 0;
  uint32_t tmpclock = 0, tmp2clock = 0;
  /* This variable used to store the VCO Input (value in Hz) */
  uint32_t vcoinput = 0;
  /* This variable used to store the SAI_CK_x (value in Hz) */
  uint32_t saiclocksource = 0;
  
  /* Check the SAI handle allocation */
  if(hsai == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the SAI Block parameters */
  assert_param(IS_SAI_BLOCK_PROTOCOL(hsai->Init.Protocol));
  assert_param(IS_SAI_BLOCK_MODE(hsai->Init.AudioMode));
  assert_param(IS_SAI_BLOCK_DATASIZE(hsai->Init.DataSize));
  assert_param(IS_SAI_BLOCK_FIRST_BIT(hsai->Init.FirstBit));
  assert_param(IS_SAI_BLOCK_CLOCK_STROBING(hsai->Init.ClockStrobing));
  assert_param(IS_SAI_BLOCK_SYNCHRO(hsai->Init.Synchro));
  assert_param(IS_SAI_BLOCK_OUTPUT_DRIVE(hsai->Init.OutputDrive));
  assert_param(IS_SAI_BLOCK_NODIVIDER(hsai->Init.NoDivider));
  assert_param(IS_SAI_BLOCK_FIFO_THRESHOLD(hsai->Init.FIFOThreshold));
  assert_param(IS_SAI_AUDIO_FREQUENCY(hsai->Init.AudioFrequency));
  
  /* Check the SAI Block Frame parameters */
  assert_param(IS_SAI_BLOCK_FRAME_LENGTH(hsai->FrameInit.FrameLength));
  assert_param(IS_SAI_BLOCK_ACTIVE_FRAME(hsai->FrameInit.ActiveFrameLength));
  assert_param(IS_SAI_BLOCK_FS_DEFINITION(hsai->FrameInit.FSDefinition));
  assert_param(IS_SAI_BLOCK_FS_POLARITY(hsai->FrameInit.FSPolarity));
  assert_param(IS_SAI_BLOCK_FS_OFFSET(hsai->FrameInit.FSOffset));
  
  /* Check the SAI Block Slot parameters */
  assert_param(IS_SAI_BLOCK_FIRSTBIT_OFFSET(hsai->SlotInit.FirstBitOffset));
  assert_param(IS_SAI_BLOCK_SLOT_SIZE(hsai->SlotInit.SlotSize));
  assert_param(IS_SAI_BLOCK_SLOT_NUMBER(hsai->SlotInit.SlotNumber));
  assert_param(IS_SAI_SLOT_ACTIVE(hsai->SlotInit.SlotActive));
  
  if(hsai->State == HAL_SAI_STATE_RESET)
  {
    /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
    HAL_SAI_MspInit(hsai);
  }
  
  hsai->State = HAL_SAI_STATE_BUSY;
  
  /* Disable the selected SAI peripheral */
  __HAL_SAI_DISABLE(hsai);
    
  /* SAI Block Configuration ------------------------------------------------------------*/
  /* SAI Block_x CR1 Configuration */
  /* Get the SAI Block_x CR1 value */
  tmpreg = hsai->Instance->CR1;
  /* Clear MODE, PRTCFG, DS, LSBFIRST, CKSTR, SYNCEN, OUTDRIV, NODIV, and MCKDIV bits */
  tmpreg &= CR1_CLEAR_MASK;
  /* Configure SAI_Block_x: Audio Protocol, Data Size, first transmitted bit, Clock strobing 
     edge, Synchronization mode, Output drive, Master Divider and FIFO level */  
  /* Set PRTCFG bits according to Protocol value      */
  /* Set DS bits according to DataSize value          */
  /* Set LSBFIRST bit according to FirstBit value     */
  /* Set CKSTR bit according to ClockStrobing value   */
  /* Set SYNCEN bit according to Synchro value        */
  /* Set OUTDRIV bit according to OutputDrive value   */
  /* Set NODIV bit according to NoDivider value       */
  tmpreg |= (uint32_t)(hsai->Init.Protocol      |
                       hsai->Init.AudioMode     |
                       hsai->Init.DataSize      | 
                       hsai->Init.FirstBit      |  
                       hsai->Init.ClockStrobing | 
                       hsai->Init.Synchro       |  
                       hsai->Init.OutputDrive   | 
                       hsai->Init.NoDivider);      
  /* Write to SAI_Block_x CR1 */
  hsai->Instance->CR1 = tmpreg;
  
  /* SAI Block_x CR2 Configuration */
  /* Get the SAIBlock_x CR2 value */
  tmpreg = hsai->Instance->CR2;
  /* Clear FTH bits */
  tmpreg &= ~(SAI_xCR2_FTH);
  /* Configure the FIFO Level */
  /* Set FTH bits according to SAI_FIFOThreshold value */ 
  tmpreg |= (uint32_t)(hsai->Init.FIFOThreshold);
  /* Write to SAI_Block_x CR2 */
  hsai->Instance->CR2 = tmpreg;

  /* SAI Block_x Frame Configuration -----------------------------------------*/
  /* Get the SAI Block_x FRCR value */
  tmpreg = hsai->Instance->FRCR;
  /* Clear FRL, FSALL, FSDEF, FSPOL, FSOFF bits */
  tmpreg &= FRCR_CLEAR_MASK;
  /* Configure SAI_Block_x Frame: Frame Length, Active Frame Length, Frame Synchronization
     Definition, Frame Synchronization Polarity and Frame Synchronization Polarity */
  /* Set FRL bits according to SAI_FrameLength value         */
  /* Set FSALL bits according to SAI_ActiveFrameLength value */
  /* Set FSDEF bit according to SAI_FSDefinition value       */
  /* Set FSPOL bit according to SAI_FSPolarity value         */
  /* Set FSOFF bit according to SAI_FSOffset value           */
  tmpreg |= (uint32_t)((uint32_t)(hsai->FrameInit.FrameLength - 1)  | 
                        hsai->FrameInit.FSOffset     | 
                        hsai->FrameInit.FSDefinition | 
                        hsai->FrameInit.FSPolarity   | 
                        (uint32_t)((hsai->FrameInit.ActiveFrameLength - 1) << 8));

  /* Write to SAI_Block_x FRCR */
  hsai->Instance->FRCR = tmpreg;

  /* SAI Block_x SLOT Configuration ------------------------------------------*/
  /* Get the SAI Block_x SLOTR value */
  tmpreg = hsai->Instance->SLOTR;
  /* Clear FBOFF, SLOTSZ, NBSLOT, SLOTEN bits */
  tmpreg &= SLOTR_CLEAR_MASK;
  /* Configure SAI_Block_x Slot: First bit offset, Slot size, Number of Slot in  
     audio frame and slots activated in audio frame */
  /* Set FBOFF bits according to SAI_FirstBitOffset value  */
  /* Set SLOTSZ bits according to SAI_SlotSize value       */
  /* Set NBSLOT bits according to SAI_SlotNumber value     */
  /* Set SLOTEN bits according to SAI_SlotActive value     */
  tmpreg |= (uint32_t)(hsai->SlotInit.FirstBitOffset | 
                       hsai->SlotInit.SlotSize       | 
                       hsai->SlotInit.SlotActive     |    
                       (uint32_t)((hsai->SlotInit.SlotNumber - 1) <<  8));

  /* Write to SAI_Block_x SLOTR */
  hsai->Instance->SLOTR = tmpreg;
  
  /* SAI Block_x Clock Configuration -----------------------------------------*/
  /* Check the Clock parameters */
  assert_param(IS_SAI_CLK_SOURCE(hsai->Init.ClockSource));

  /* SAI Block clock source selection */
  if(hsai->Instance == SAI1_Block_A)
  {
     __HAL_RCC_SAI_BLOCKACLKSOURCE_CONFIG(hsai->Init.ClockSource);
  }
  else
  {
     __HAL_RCC_SAI_BLOCKBCLKSOURCE_CONFIG((uint32_t)(hsai->Init.ClockSource << 2));
  }
  
  /* VCO Input Clock value calculation */
  if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLSOURCE_HSI)
  {
    /* In Case the PLL Source is HSI (Internal Clock) */
    vcoinput = (HSI_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM));
  }
  else
  {
    /* In Case the PLL Source is HSE (External Clock) */
    vcoinput = ((HSE_VALUE / (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM)));
  }
  
  /* SAI_CLK_x : SAI Block Clock configuration for different clock sources selected */
  if(hsai->Init.ClockSource == SAI_CLKSOURCE_PLLSAI)
  {    
    /* Configure the PLLI2S division factor */
    /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
    /* SAI_CLK(first level) = PLLSAI_VCO Output/PLLSAIQ */
    tmpreg = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> 24;
    saiclocksource = (vcoinput * ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN) >> 6))/(tmpreg);
    
    /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ */
    tmpreg = (((RCC->DCKCFGR & RCC_DCKCFGR_PLLSAIDIVQ) >> 8) + 1);
    saiclocksource = saiclocksource/(tmpreg); 

  }
  else if(hsai->Init.ClockSource == SAI_CLKSOURCE_PLLI2S)
  {        
    /* Configure the PLLI2S division factor */
    /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
    /* SAI_CLK(first level) = PLLI2S_VCO Output/PLLI2SQ */
    tmpreg = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> 24;
    saiclocksource = (vcoinput * ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6))/(tmpreg);
    
    /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ */
    tmpreg = ((RCC->DCKCFGR & RCC_DCKCFGR_PLLI2SDIVQ) + 1); 
    saiclocksource = saiclocksource/(tmpreg);
  }
  else /* sConfig->ClockSource == SAI_CLKSource_Ext */
  {
    /* Enable the External Clock selection */
    __HAL_RCC_I2SCLK(RCC_I2SCLKSOURCE_EXT);
    
    saiclocksource = EXTERNAL_CLOCK_VALUE;
  }
  
  /* Configure Master Clock using the following formula :
     MCLK_x = SAI_CK_x / (MCKDIV[3:0] * 2) with MCLK_x = 256 * FS
     FS = SAI_CK_x / (MCKDIV[3:0] * 2) * 256
     MCKDIV[3:0] = SAI_CK_x / FS * 512 */
  if(hsai->Init.NoDivider == SAI_MASTERDIVIDER_ENABLED) 
  { 
    /* (saiclocksource x 10) to keep Significant digits */
    tmpclock = (((saiclocksource * 10) / ((hsai->Init.AudioFrequency) * 512)));
    
    /* Get the result of modulo division */
    tmp2clock = (tmpclock % 10);
    
    /* Round result to the nearest integer*/
    if (tmp2clock > 8) 
    {
      tmpclock = ((tmpclock / 10) + 1);  
    }
    else 
    {
      tmpclock = (tmpclock / 10); 
    }
    /*Set MCKDIV value in CR1 register*/
    hsai->Instance->CR1 |= (tmpclock << 20);

  }

  /* Initialise the error code */
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;

  /* Initialize the SAI state */
  hsai->State= HAL_SAI_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the SAI peripheral. 
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DeInit(SAI_HandleTypeDef *hsai)
{
  /* Check the SAI handle allocation */
  if(hsai == NULL)
  {
    return HAL_ERROR;
  }

  hsai->State = HAL_SAI_STATE_BUSY;

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC and DMA */
  HAL_SAI_MspDeInit(hsai);

  /* Initialize the error code */
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  
  /* Initialize the SAI state */
  hsai->State = HAL_SAI_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hsai);

  return HAL_OK;
}

/**
  * @brief SAI MSP Init.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief SAI MSP DeInit.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SAI_Group2 IO operation functions 
 *  @brief   Data transfers functions 
 *
@verbatim   
  ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the SAI data 
    transfers.

    (+) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode. 
            The status of all data processing is returned by the same function 
            after finishing transfer.  
       (++) No-Blocking mode : The communication is performed using Interrupts 
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the 
            dedicated SAI IRQ when using Interrupt mode or the DMA IRQ when 
            using DMA mode.

    (+) Blocking mode functions are :
        (++) HAL_SAI_Transmit()
        (++) HAL_SAI_Receive()
        (++) HAL_SAI_TransmitReceive()
        
    (+) Non Blocking mode functions with Interrupt are :
        (++) HAL_SAI_Transmit_IT()
        (++) HAL_SAI_Receive_IT()
        (++) HAL_SAI_TransmitReceive_IT()

    (+) Non Blocking mode functions with DMA are :
        (++) HAL_SAI_Transmit_DMA()
        (++) HAL_SAI_Receive_DMA()
        (++) HAL_SAI_TransmitReceive_DMA()

    (+) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_SAI_TxCpltCallback()
        (++) HAL_SAI_RxCpltCallback()
        (++) HAL_SAI_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmits an amount of data in blocking mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit(SAI_HandleTypeDef *hsai, uint16_t* pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  if((pData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }
  
  if(hsai->State == HAL_SAI_STATE_READY)
  {  
    /* Process Locked */
    __HAL_LOCK(hsai);
    
    hsai->State = HAL_SAI_STATE_BUSY_TX;
   
    /* Check if the SAI is already enabled */ 
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */    
      __HAL_SAI_ENABLE(hsai);
    }
    
    while(Size > 0)
    { 
       /* Get tick */
       tickstart = HAL_GetTick();

      /* Wait the FIFO to be empty */
      while(__HAL_SAI_GET_FLAG(hsai, SAI_xSR_FREQ) == RESET)
      {
        /* Check for the Timeout */
        if(Timeout != HAL_MAX_DELAY)
        {
          if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
          {
            /* Update error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;
            
            /* Process Unlocked */
            __HAL_UNLOCK(hsai);
            
            /* Change the SAI state */
            hsai->State = HAL_SAI_STATE_TIMEOUT;
            
            return HAL_TIMEOUT;
          }
        } 
      } 
      hsai->Instance->DR = (*pData++);
      Size--; 
    }      
      
    hsai->State = HAL_SAI_STATE_READY; 
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives an amount of data in blocking mode. 
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = 0;
 
  if((pData == NULL ) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }
  
  if(hsai->State == HAL_SAI_STATE_READY)
  { 
    /* Process Locked */
    __HAL_LOCK(hsai);
    
    hsai->State = HAL_SAI_STATE_BUSY_RX;
        
    /* Check if the SAI is already enabled */ 
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */    
      __HAL_SAI_ENABLE(hsai);
    }
       
    /* Receive data */
    while(Size > 0)
    {
      /* Get tick */
      tickstart = HAL_GetTick();

      /* Wait until RXNE flag is set */
      while(__HAL_SAI_GET_FLAG(hsai, SAI_xSR_FREQ) == RESET)
      {
        /* Check for the Timeout */
        if(Timeout != HAL_MAX_DELAY)
        {
          if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
          {
            /* Update error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;
            
            /* Process Unlocked */
            __HAL_UNLOCK(hsai);
            
            /* Change the SAI state */
            hsai->State = HAL_SAI_STATE_TIMEOUT;
            
            return HAL_TIMEOUT;
          }
        }
      }
      
      (*pData++) = hsai->Instance->DR;
      Size--; 
    }      

    hsai->State = HAL_SAI_STATE_READY; 
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Transmits an amount of data in no-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit_IT(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size)
{
 if(hsai->State == HAL_SAI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    
    hsai->pTxBuffPtr = pData;
    hsai->TxXferSize = Size;
    hsai->TxXferCount = Size;
    
    /* Process Locked */
    __HAL_LOCK(hsai);
    
    hsai->State = HAL_SAI_STATE_BUSY_TX;
    
    /* Transmit data */
    hsai->Instance->DR = (*hsai->pTxBuffPtr++);
    hsai->TxXferCount--;
      
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    /* Enable FRQ and OVRUDR interrupts */
    __HAL_SAI_ENABLE_IT(hsai, (SAI_IT_FREQ | SAI_IT_OVRUDR));
      
    /* Check if the SAI is already enabled */ 
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */    
      __HAL_SAI_ENABLE(hsai);
    }

   
    return HAL_OK;
  }
  else if(hsai->State == HAL_SAI_STATE_BUSY_TX)
  {   
    /* Process Locked */
    __HAL_LOCK(hsai);
    
    /* Transmit data */
    hsai->Instance->DR = (*hsai->pTxBuffPtr++);
    
    hsai->TxXferCount--;	
    
    if(hsai->TxXferCount == 0)
    {
      /* Disable FREQ and OVRUDR interrupts */
      __HAL_SAI_DISABLE_IT(hsai, (SAI_IT_FREQ | SAI_IT_OVRUDR));
      
      hsai->State = HAL_SAI_STATE_READY;
      
      HAL_SAI_TxCpltCallback(hsai);
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    return HAL_OK;
  }
  
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives an amount of data in no-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive_IT(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size)
{
  if(hsai->State == HAL_SAI_STATE_READY)
  {
    if((pData == NULL) || (Size == 0)) 
    {
      return  HAL_ERROR;
    }
    
    hsai->pRxBuffPtr = pData;
    hsai->RxXferSize = Size;
    hsai->RxXferCount = Size;
    
    /* Process Locked */
    __HAL_LOCK(hsai);
    
    hsai->State = HAL_SAI_STATE_BUSY_RX;

    /* Enable TXE and OVRUDR interrupts */
    __HAL_SAI_ENABLE_IT(hsai, (SAI_IT_FREQ | SAI_IT_OVRUDR));
    
    /* Check if the SAI is already enabled */ 
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */    
      __HAL_SAI_ENABLE(hsai);
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    return HAL_OK;
  }
  else if(hsai->State == HAL_SAI_STATE_BUSY_RX)
  {
     /* Process Locked */
    __HAL_LOCK(hsai);
    
    /* Receive data */    
    (*hsai->pRxBuffPtr++) = hsai->Instance->DR;
    
    hsai->RxXferCount--;
    
    if(hsai->RxXferCount == 0)
    {    
      /* Disable TXE and OVRUDR interrupts */
      __HAL_SAI_DISABLE_IT(hsai, (SAI_IT_FREQ | SAI_IT_OVRUDR));
      
      hsai->State = HAL_SAI_STATE_READY;     
      HAL_SAI_RxCpltCallback(hsai); 
    }
    
    /* Process Unlocked */
    __HAL_UNLOCK(hsai);
    
    return HAL_OK;
  }

  else
  {
    return HAL_BUSY; 
  } 
}

/**
  * @brief Pauses the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DMAPause(SAI_HandleTypeDef *hsai)
{
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  /* Pause the audio file playing by disabling the SAI DMA requests */
  hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;
  
 
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return HAL_OK; 
}

/**
  * @brief Resumes the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DMAResume(SAI_HandleTypeDef *hsai)
{
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  /* Enable the SAI DMA requests */
  hsai->Instance->CR1 |= SAI_xCR1_DMAEN;
  
  
  /* If the SAI peripheral is still not enabled, enable it */
  if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == 0)
  {
    /* Enable SAI peripheral */    
    __HAL_SAI_ENABLE(hsai);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return HAL_OK;
}

/**
  * @brief Stops the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DMAStop(SAI_HandleTypeDef *hsai)
{
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  /* Disable the SAI DMA request */
  hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;
  
  /* Abort the SAI DMA Tx Stream */
  if(hsai->hdmatx != NULL)
  {
    HAL_DMA_Abort(hsai->hdmatx);
  }
  /* Abort the SAI DMA Rx Stream */
  if(hsai->hdmarx != NULL)
  {  
    HAL_DMA_Abort(hsai->hdmarx);
  }

  /* Disable SAI peripheral */
  __HAL_SAI_DISABLE(hsai);
  
  hsai->State = HAL_SAI_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return HAL_OK;
}
/**
  * @brief  Transmits an amount of data in no-blocking mode with DMA.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit_DMA(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size)
{
  uint32_t *tmp;

  if((pData == NULL) || (Size == 0)) 
  {
    return  HAL_ERROR;
  }

  if(hsai->State == HAL_SAI_STATE_READY)
  {  
    hsai->pTxBuffPtr = pData;
    hsai->TxXferSize = Size;
    hsai->TxXferCount = Size;

    /* Process Locked */
    __HAL_LOCK(hsai);

    hsai->State = HAL_SAI_STATE_BUSY_TX;

    /* Set the SAI Tx DMA Half transfert complete callback */
    hsai->hdmatx->XferHalfCpltCallback = SAI_DMATxHalfCplt;

    /* Set the SAI TxDMA transfer complete callback */
    hsai->hdmatx->XferCpltCallback = SAI_DMATxCplt;

    /* Set the DMA error callback */
    hsai->hdmatx->XferErrorCallback = SAI_DMAError;

    /* Enable the Tx DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hsai->hdmatx, *(uint32_t*)tmp, (uint32_t)&hsai->Instance->DR, hsai->TxXferSize);

    /* Check if the SAI is already enabled */ 
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */
      __HAL_SAI_ENABLE(hsai);
    }

    /* Enable SAI Tx DMA Request */  
    hsai->Instance->CR1 |= SAI_xCR1_DMAEN;

    /* Process Unlocked */
    __HAL_UNLOCK(hsai);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives an amount of data in no-blocking mode with DMA. 
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive_DMA(SAI_HandleTypeDef *hsai, uint16_t *pData, uint16_t Size)
{
  uint32_t *tmp;
  
  if((pData == NULL) || (Size == 0))
  {
    return  HAL_ERROR;
  } 
    
  if(hsai->State == HAL_SAI_STATE_READY)
  {    
    hsai->pRxBuffPtr = pData;
    hsai->RxXferSize = Size;
    hsai->RxXferCount = Size;

    /* Process Locked */
    __HAL_LOCK(hsai);
    
    hsai->State = HAL_SAI_STATE_BUSY_RX;

    /* Set the SAI Rx DMA Half transfert complete callback */
    hsai->hdmarx->XferHalfCpltCallback = SAI_DMARxHalfCplt;

    /* Set the SAI Rx DMA transfert complete callback */
    hsai->hdmarx->XferCpltCallback = SAI_DMARxCplt;

    /* Set the DMA error callback */
    hsai->hdmarx->XferErrorCallback = SAI_DMAError;

    /* Enable the Rx DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(hsai->hdmarx, (uint32_t)&hsai->Instance->DR, *(uint32_t*)tmp, hsai->RxXferSize);

    /* Check if the SAI is already enabled */
    if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != SAI_xCR1_SAIEN)
    {
      /* Enable SAI peripheral */
      __HAL_SAI_ENABLE(hsai);
    }

    /* Enable SAI Rx DMA Request */
    hsai->Instance->CR1 |= SAI_xCR1_DMAEN;

    /* Process Unlocked */
    __HAL_UNLOCK(hsai);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  This function handles SAI interrupt request.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai)
{
  uint32_t tmp1 = 0, tmp2 = 0; 
  
  if(hsai->State == HAL_SAI_STATE_BUSY_RX)
  {
    tmp1 = __HAL_SAI_GET_FLAG(hsai, SAI_xSR_FREQ);
    tmp2 = __HAL_SAI_GET_IT_SOURCE(hsai, SAI_IT_FREQ);
    /* SAI in mode Receiver --------------------------------------------------*/
    if((tmp1  != RESET) && (tmp2 != RESET))
    {
      HAL_SAI_Receive_IT(hsai, NULL, 0);
    }

    tmp1 = __HAL_SAI_GET_FLAG(hsai, SAI_FLAG_OVRUDR);
    tmp2 = __HAL_SAI_GET_IT_SOURCE(hsai, SAI_IT_OVRUDR);
    /* SAI Overrun error interrupt occurred ----------------------------------*/
    if((tmp1 != RESET) && (tmp2 != RESET))
    {
      /* Change the SAI error code */
      hsai->ErrorCode = HAL_SAI_ERROR_OVR;

      /* Clear the SAI Overrun flag */
      __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
      /* Set the SAI state ready to be able to start again the process */
      hsai->State = HAL_SAI_STATE_READY;
      HAL_SAI_ErrorCallback(hsai);
    }
  }
  
  if(hsai->State == HAL_SAI_STATE_BUSY_TX)
  {  
    tmp1 = __HAL_SAI_GET_FLAG(hsai, SAI_xSR_FREQ);
    tmp2 = __HAL_SAI_GET_IT_SOURCE(hsai, SAI_IT_FREQ);
    /* SAI in mode Transmitter -----------------------------------------------*/
    if((tmp1 != RESET) && (tmp2 != RESET))
    {     
      HAL_SAI_Transmit_IT(hsai, NULL, 0);
    } 
    
    tmp1 = __HAL_SAI_GET_FLAG(hsai, SAI_FLAG_OVRUDR);
    tmp2 = __HAL_SAI_GET_IT_SOURCE(hsai, SAI_IT_OVRUDR);
    /* SAI Underrun error interrupt occurred ---------------------------------*/
    if((tmp1 != RESET) && (tmp2 != RESET))
    {
      /* Change the SAI error code */
      hsai->ErrorCode = HAL_SAI_ERROR_UDR;

      /* Clear the SAI Underrun flag */
      __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
      /* Set the SAI state ready to be able to start again the process */
      hsai->State = HAL_SAI_STATE_READY;
      HAL_SAI_ErrorCallback(hsai);
    }
  } 
}

/**
  * @brief Tx Transfer completed callbacks.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
 __weak void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief Tx Transfer Half completed callbacks
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
 __weak void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxHalfCpltCallback could be implenetd in the user file
   */ 
}

/**
  * @brief Rx Transfer completed callbacks.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief Rx Transfer half completed callbacks
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_RxCpltCallback could be implenetd in the user file
   */
}

/**
  * @brief SAI error callbacks.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */


/** @defgroup SAI_Group3 Peripheral State functions 
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
  * @brief  Returns the SAI state.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL state
  */
HAL_SAI_StateTypeDef HAL_SAI_GetState(SAI_HandleTypeDef *hsai)
{
  return hsai->State;
}

/**
* @brief  Return the SAI error code
* @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *              the configuration information for the specified SAI Block.
* @retval SAI Error Code
*/
uint32_t HAL_SAI_GetError(SAI_HandleTypeDef *hsai)
{
  return hsai->ErrorCode;
}
/**
  * @}
  */

/**
  * @brief DMA SAI transmit process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMATxCplt(DMA_HandleTypeDef *hdma)   
{
  uint32_t tickstart = 0;
  
  SAI_HandleTypeDef* hsai = (SAI_HandleTypeDef*)((DMA_HandleTypeDef* )hdma)->Parent;

  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  { 
    hsai->TxXferCount = 0;
    hsai->RxXferCount = 0;
    
    /* Disable SAI Tx DMA Request */  
    hsai->Instance->CR1 &= (uint32_t)(~SAI_xCR1_DMAEN);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Set timeout: 10 is the max delay to send the remaining data in the SAI FIFO */
    /* Wait until FIFO is empty */    
    while(__HAL_SAI_GET_FLAG(hsai, SAI_xSR_FLVL) != RESET)
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > SAI_TIMEOUT_VALUE)
      {         
        /* Update error code */
        hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;
        
        /* Change the SAI state */
        HAL_SAI_ErrorCallback(hsai);
      }
    } 
    
    hsai->State= HAL_SAI_STATE_READY;
  }
  HAL_SAI_TxCpltCallback(hsai);
}

/**
  * @brief DMA SAI transmit process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  SAI_HandleTypeDef* hsai = (SAI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_SAI_TxHalfCpltCallback(hsai);
}

/**
  * @brief DMA SAI receive process complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMARxCplt(DMA_HandleTypeDef *hdma)   
{
  SAI_HandleTypeDef* hsai = ( SAI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    /* Disable Rx DMA Request */
    hsai->Instance->CR1 &= (uint32_t)(~SAI_xCR1_DMAEN);
    hsai->RxXferCount = 0;
    
    hsai->State = HAL_SAI_STATE_READY;
  }
  HAL_SAI_RxCpltCallback(hsai); 
}

/**
  * @brief DMA SAI receive process half complete callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  SAI_HandleTypeDef* hsai = (SAI_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  HAL_SAI_RxHalfCpltCallback(hsai); 
}
/**
  * @brief DMA SAI communication error callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMAError(DMA_HandleTypeDef *hdma)   
{
  SAI_HandleTypeDef* hsai = ( SAI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* Set the SAI state ready to be able to start again the process */
  hsai->State= HAL_SAI_STATE_READY;
  HAL_SAI_ErrorCallback(hsai);
  
  hsai->TxXferCount = 0;
  hsai->RxXferCount = 0;
}

/**
  * @}
  */

#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_SAI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
