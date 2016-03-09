/**
  ******************************************************************************
  * @file    stm32l4xx_hal_sai.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
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
    
    (#) Declare a SAI_HandleTypeDef handle structure (eg. SAI_HandleTypeDef hsai).
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
            (+++) Declare a DMA handle structure for the Tx/Rx channel.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
            (+++) Configure the DMA Tx/Rx Channel.
            (+++) Associate the initialized DMA handle to the SAI DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the 
                DMA Tx/Rx Channel.
  
    (#) The initialization can be done by two ways 
        (##) Expert mode : Initialize the structures Init, FrameInit and SlotInit and call HAL_SAI_Init().
        (##) Simplified mode : Initialize the high part of Init Structure and call HAL_SAI_InitProtocol().
   
  [..]           
   (@) The specific SAI interrupts (FIFO request and Overrun underrun interrupt)
       will be managed using the macros __HAL_SAI_ENABLE_IT() and __HAL_SAI_DISABLE_IT()
       inside the transmit and receive process.   
       
  [..]           
   (@) Make sure that either:
       (+@) PLLSAI1CLK output is configured or 
       (+@) PLLSAI2CLK output is configured or 
       (+@) PLLSAI3CLK output is configured or 
       (+@) External clock source is configured after setting correctly 
            the define constant EXTERNAL_SAI1_CLOCK_VALUE or EXTERNAL_SAI2_CLOCK_VALUE in the stm32l4xx_hal_conf.h file. 

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
     (+) Send an amount of data in non-blocking mode using HAL_SAI_Transmit_IT() 
     (+) At transmission end of transfer HAL_SAI_TxCpltCallback() is executed and user can 
         add his own code by customization of function pointer HAL_SAI_TxCpltCallback()
     (+) Receive an amount of data in non-blocking mode using HAL_SAI_Receive_IT() 
     (+) At reception end of transfer HAL_SAI_RxCpltCallback() is executed and user can 
         add his own code by customization of function pointer HAL_SAI_RxCpltCallback()
     (+) In case of flag error, HAL_SAI_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_SAI_ErrorCallback()

   *** DMA mode IO operation ***
   ==============================
   [..] 
     (+) Send an amount of data in non-blocking mode (DMA) using HAL_SAI_Transmit_DMA() 
     (+) At transmission end of transfer HAL_SAI_TxCpltCallback() is executed and user can 
         add his own code by customization of function pointer HAL_SAI_TxCpltCallback()
     (+) Receive an amount of data in non-blocking mode (DMA) using HAL_SAI_Receive_DMA() 
     (+) At reception end of transfer HAL_SAI_RxCpltCallback() is executed and user can 
         add his own code by customization of function pointer HAL_SAI_RxCpltCallback()
     (+) In case of flag error, HAL_SAI_ErrorCallback() function is executed and user can 
         add his own code by customization of function pointer HAL_SAI_ErrorCallback()
     (+) Pause the DMA Transfer using HAL_SAI_DMAPause()
     (+) Resume the DMA Transfer using HAL_SAI_DMAResume()
     (+) Stop the DMA Transfer using HAL_SAI_DMAStop()
   
   *** SAI HAL driver additional function list ***
   ================================================= 
   [..]
     Below the list the others API available SAI HAL driver :
       
      (+) HAL_SAI_EnableTxMuteMode(): Enable the mute in tx mode 
      (+) HAL_SAI_DisableTxMuteMode(): Disable the mute in tx mode 
      (+) HAL_SAI_EnableRxMuteMode(): Enable the mute in Rx mode
      (+) HAL_SAI_DisableRxMuteMode(): Disable the mute in Rx mode
      (+) HAL_SAI_FlushRxFifo(): Flush the rx fifo.
      (+) HAL_SAI_Abort(): Abort the current transfer

   *** SAI HAL driver macros list ***
   ============================================= 
   [..]
     Below the list of most used macros in SAI HAL driver :
       
      (+) __HAL_SAI_ENABLE(): Enable the SAI peripheral
      (+) __HAL_SAI_DISABLE(): Disable the SAI peripheral
      (+) __HAL_SAI_ENABLE_IT(): Enable the specified SAI interrupts
      (+) __HAL_SAI_DISABLE_IT(): Disable the specified SAI interrupts
      (+) __HAL_SAI_GET_IT_SOURCE(): Check if the specified SAI interrupt source is 
          enabled or disabled
      (+) __HAL_SAI_GET_FLAG(): Check whether the specified SAI flag is set or not
  
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

/** @defgroup SAI SAI
  * @brief SAI HAL module driver
  * @{
  */

#ifdef HAL_SAI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/** @defgroup SAI_Private_Typedefs  SAI Private Typedefs
  * @{
  */
typedef enum {
  SAI_MODE_DMA,
  SAI_MODE_IT
}SAI_ModeTypedef;
/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
/** @defgroup SAI_Private_Constants  SAI Private Constants
  * @{
  */
#define SAI_FIFO_SIZE       8
#define SAI_DEFAULT_TIMEOUT 4
#define SAI_xCR2_MUTECNT_OFFSET POSITION_VAL(SAI_xCR2_MUTECNT)
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup SAI_Private_Functions  SAI Private Functions
  * @{
  */
static void SAI_FillFifo(SAI_HandleTypeDef *hsai);
static int32_t SAI_InterruptFlag(SAI_HandleTypeDef *hsai, uint32_t mode);
static HAL_StatusTypeDef SAI_InitI2S(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot);
static HAL_StatusTypeDef SAI_InitPCM(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot);

static HAL_StatusTypeDef SAI_Disable(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT8Bit(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT16Bit(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT32Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT8Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT16Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT32Bit(SAI_HandleTypeDef *hsai);
static void SAI_DMATxCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMARxCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void SAI_DMAError(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup SAI_Exported_Functions SAI Exported Functions
  * @{
  */

/** @defgroup SAI_Exported_Functions_Group1 Initialization and de-initialization functions 
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
  * @brief  Initialize the structure FrameInit, SlotInit and the low part of 
  *         Init according to the specified parameters and call the function
  *         HAL_SAI_Init to initialize the SAI block.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains 
  *               the configuration information for SAI module.
  * @param  protocol: one of the supported protocol @ref SAI_Protocol
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  *                   the configuration information for SAI module.
  * @param  nbslot: Number of slot.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_InitProtocol(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot)
{
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  switch(protocol)
  {
  case SAI_I2S_STANDARD :
  case SAI_I2S_MSBJUSTIFIED :
  case SAI_I2S_LSBJUSTIFIED :
    errorcode = SAI_InitI2S(hsai, protocol, datasize, nbslot);
    break;  
  case SAI_PCM_LONG :
  case SAI_PCM_SHORT :
    errorcode = SAI_InitPCM(hsai, protocol, datasize, nbslot);
    break;
  default :
    errorcode = HAL_ERROR;
    break;
  }
  
  if(errorcode == HAL_OK)
  {
    errorcode = HAL_SAI_Init(hsai);
  }

  return errorcode;
}

/**
  * @brief  Initialize the SAI according to the specified parameters 
  *         in the SAI_InitTypeDef structure and initialize the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Init(SAI_HandleTypeDef *hsai)
{ 
  uint32_t tmpregisterGCR = 0;
  uint32_t ckstrBits  = 0;
  uint32_t syncenBits = 0;

  /* Check the SAI handle allocation */
  if(hsai == NULL)
  {
    return HAL_ERROR;
  }
  
  /* check the instance */
  assert_param(IS_SAI_ALL_INSTANCE(hsai->Instance));
  
  /* Check the SAI Block parameters */
  assert_param(IS_SAI_AUDIO_FREQUENCY(hsai->Init.AudioFrequency));
  assert_param(IS_SAI_BLOCK_PROTOCOL(hsai->Init.Protocol));
  assert_param(IS_SAI_BLOCK_MODE(hsai->Init.AudioMode));
  assert_param(IS_SAI_BLOCK_DATASIZE(hsai->Init.DataSize));
  assert_param(IS_SAI_BLOCK_FIRST_BIT(hsai->Init.FirstBit));
  assert_param(IS_SAI_BLOCK_CLOCK_STROBING(hsai->Init.ClockStrobing));
  assert_param(IS_SAI_BLOCK_SYNCHRO(hsai->Init.Synchro));
  assert_param(IS_SAI_BLOCK_OUTPUT_DRIVE(hsai->Init.OutputDrive));
  assert_param(IS_SAI_BLOCK_NODIVIDER(hsai->Init.NoDivider));
  assert_param(IS_SAI_BLOCK_FIFO_THRESHOLD(hsai->Init.FIFOThreshold));
  assert_param(IS_SAI_MONOSTEREO_MODE(hsai->Init.MonoStereoMode));
  assert_param(IS_SAI_BLOCK_COMPANDING_MODE(hsai->Init.CompandingMode));
  assert_param(IS_SAI_BLOCK_TRISTATE_MANAGEMENT(hsai->Init.TriState));
  assert_param(IS_SAI_BLOCK_SYNCEXT(hsai->Init.SynchroExt));
  
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
    /* Allocate lock resource and initialize it */
    hsai->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
    HAL_SAI_MspInit(hsai);
  }
  
  hsai->State = HAL_SAI_STATE_BUSY;
  
  /* Disable the selected SAI peripheral */
  SAI_Disable(hsai);

  /* SAI Block Synchro Configuration ---------------------------------------------------*/
  /* This setting must be done with both audio block (A & B) disabled                   */
  switch(hsai->Init.SynchroExt)
  {
    case SAI_SYNCEXT_DISABLE :
      tmpregisterGCR = 0;
      break;
    case SAI_SYNCEXT_OUTBLOCKA_ENABLE :
      tmpregisterGCR = SAI_GCR_SYNCOUT_0;
      break;
    case SAI_SYNCEXT_OUTBLOCKB_ENABLE :
      tmpregisterGCR = SAI_GCR_SYNCOUT_1;
      break;
  }
    
  switch(hsai->Init.Synchro)
  {
    case SAI_ASYNCHRONOUS :
      {
        syncenBits = 0;
      }
      break;
    case SAI_SYNCHRONOUS :
      {
        syncenBits = SAI_xCR1_SYNCEN_0;
      }
      break;
    case SAI_SYNCHRONOUS_EXT_SAI1 :
      {
        syncenBits = SAI_xCR1_SYNCEN_1;
      }
      break;
    case SAI_SYNCHRONOUS_EXT_SAI2 :
      {
        syncenBits = SAI_xCR1_SYNCEN_1;
        tmpregisterGCR |= SAI_GCR_SYNCIN_0;
      }
      break;
  }

  if((hsai->Instance == SAI1_Block_A) || (hsai->Instance == SAI1_Block_B))
  {
    SAI1->GCR = tmpregisterGCR;
  }
  else 
  {
    SAI2->GCR = tmpregisterGCR;
  }

  if(hsai->Init.AudioFrequency != SAI_AUDIO_FREQUENCY_MCKDIV)
  {
    uint32_t freq = 0;
    uint32_t tmpval;

    /* In this case, the MCKDIV value is calculated to get AudioFrequency */
    if((hsai->Instance == SAI1_Block_A ) || (hsai->Instance == SAI1_Block_B ))
    {
      freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SAI1);  
    }
    if((hsai->Instance == SAI2_Block_A ) || (hsai->Instance == SAI2_Block_B )) 
    {
      freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SAI2);  
    }

    /* Configure Master Clock using the following formula :
       MCLK_x = SAI_CK_x / (MCKDIV[3:0] * 2) with MCLK_x = 256 * FS
       FS = SAI_CK_x / (MCKDIV[3:0] * 2) * 256
        MCKDIV[3:0] = SAI_CK_x / FS * 512 */
    /* (freq x 10) to keep Significant digits */
    tmpval = (freq * 10) / (hsai->Init.AudioFrequency * 2 * 256);
    hsai->Init.Mckdiv = tmpval / 10;
    
    /* Round result to the nearest integer */
    if((tmpval % 10) > 8) 
    {
      hsai->Init.Mckdiv+= 1;
    }
  }

  /* Compute CKSTR bits of SAI CR1 according ClockStrobing and AudioMode */
  if((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
  { /* Transmit */
    ckstrBits = (hsai->Init.ClockStrobing == SAI_CLOCKSTROBING_RISINGEDGE) ? 0 : SAI_xCR1_CKSTR;
  }
  else
  { /* Receive */
    ckstrBits = (hsai->Init.ClockStrobing == SAI_CLOCKSTROBING_RISINGEDGE) ? SAI_xCR1_CKSTR : 0;
  }
  
  /* SAI Block Configuration ------------------------------------------------------------*/
  /* SAI CR1 Configuration */
  hsai->Instance->CR1&=~(SAI_xCR1_MODE | SAI_xCR1_PRTCFG |  SAI_xCR1_DS |      \
                         SAI_xCR1_LSBFIRST | SAI_xCR1_CKSTR | SAI_xCR1_SYNCEN |\
                         SAI_xCR1_MONO | SAI_xCR1_OUTDRIV  | SAI_xCR1_DMAEN |  \
                         SAI_xCR1_NODIV | SAI_xCR1_MCKDIV);
    
  hsai->Instance->CR1|=(hsai->Init.AudioMode | hsai->Init.Protocol |           \
                        hsai->Init.DataSize | hsai->Init.FirstBit  |           \
                        ckstrBits | syncenBits |                               \
                        hsai->Init.MonoStereoMode | hsai->Init.OutputDrive |   \
                        hsai->Init.NoDivider | (hsai->Init.Mckdiv << 20));
  
  /* SAI CR2 Configuration */
  hsai->Instance->CR2&= ~(SAI_xCR2_FTH | SAI_xCR2_FFLUSH | SAI_xCR2_COMP | SAI_xCR2_CPL);
  hsai->Instance->CR2|=  (hsai->Init.FIFOThreshold | hsai->Init.CompandingMode | hsai->Init.TriState);


  /* SAI Frame Configuration -----------------------------------------*/
  hsai->Instance->FRCR&=(~(SAI_xFRCR_FRL | SAI_xFRCR_FSALL | SAI_xFRCR_FSDEF | \
                           SAI_xFRCR_FSPO | SAI_xFRCR_FSOFF));
  hsai->Instance->FRCR|=((hsai->FrameInit.FrameLength - 1)  | 
                            hsai->FrameInit.FSOffset | 
                            hsai->FrameInit.FSDefinition | 
                            hsai->FrameInit.FSPolarity   | 
                            ((hsai->FrameInit.ActiveFrameLength - 1) << 8));  
  
  /* SAI Block_x SLOT Configuration ------------------------------------------*/
  /* This register has no meaning in AC 97 and SPDIF audio protocol */
  hsai->Instance->SLOTR&= (~(SAI_xSLOTR_FBOFF | SAI_xSLOTR_SLOTSZ |            \
                             SAI_xSLOTR_NBSLOT | SAI_xSLOTR_SLOTEN ));
  
  hsai->Instance->SLOTR|=  hsai->SlotInit.FirstBitOffset |  hsai->SlotInit.SlotSize
                          | (hsai->SlotInit.SlotActive << 16) | ((hsai->SlotInit.SlotNumber - 1) <<  8);           
  
  /* Initialize the error code */
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  
  /* Initialize the SAI state */
  hsai->State= HAL_SAI_STATE_READY;
  
  /* Release Lock */
  __HAL_UNLOCK(hsai);
  
  return HAL_OK;
}

/**
  * @brief  DeInitialize the SAI peripheral. 
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
  
  /* Disabled All interrupt and clear all the flag */
  hsai->Instance->IMR = 0;
  hsai->Instance->CLRFR = 0xFFFFFFFF;
  
  /* Disable the SAI */
  SAI_Disable(hsai);

  /* Flush the fifo */
  SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC and DMA */
  HAL_SAI_MspDeInit(hsai);

  /* Initialize the error code */
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  
  /* Initialize the SAI state */
  hsai->State = HAL_SAI_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief Initialize the SAI MSP.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief DeInitialize the SAI MSP.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SAI_Exported_Functions_Group2 IO operation functions 
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
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit(SAI_HandleTypeDef *hsai, uint8_t* pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  if((pData == NULL ) || (Size == 0)) 
  {
    errorcode = HAL_ERROR;
    goto error;
  }
  
  if(hsai->State != HAL_SAI_STATE_READY)
  {
    errorcode = HAL_BUSY;
    goto error;
  }
  
  hsai->State = HAL_SAI_STATE_BUSY_TX;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->pBuffPtr = pData;

  /* Check if the SAI is already enabled */ 
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* fill the fifo with data before to enabled the SAI */
    SAI_FillFifo(hsai);
    /* Enable SAI peripheral */    
    __HAL_SAI_ENABLE(hsai);
  }

  while(hsai->XferCount > 0)
  { 
    /* Write data if the FIFO is not full */
    if((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_FULL)
    {
      if((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
      {
        hsai->Instance->DR = (*hsai->pBuffPtr++);
      }
      else if(hsai->Init.DataSize <= SAI_DATASIZE_16)
      {
        *(uint16_t *)&hsai->Instance->DR = *((uint16_t *)hsai->pBuffPtr);
        hsai->pBuffPtr+= 2;
      }
      else
      {
        hsai->Instance->DR = *((uint32_t *)hsai->pBuffPtr);
        hsai->pBuffPtr+= 4;
      }
      hsai->XferCount--;
    }
    else
    {
      /* Check for the Timeout */
      if((Timeout != HAL_MAX_DELAY) && ((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout)))
      {
        errorcode = HAL_TIMEOUT;
        goto error;
      }
    }
  }

  hsai->State = HAL_SAI_STATE_READY;
    
error :
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}
  
/**
  * @brief  Receive an amount of data in blocking mode. 
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive(SAI_HandleTypeDef *hsai, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  if((pData == NULL ) || (Size == 0)) 
  {
    errorcode = HAL_ERROR;
    goto error;
  }
  
  if(hsai->State != HAL_SAI_STATE_READY)
  { 
    errorcode = HAL_BUSY;
    goto error;
  }
  
  hsai->pBuffPtr = pData;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->State = HAL_SAI_STATE_BUSY_RX;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  
  /* Check if the SAI is already enabled */ 
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */    
    __HAL_SAI_ENABLE(hsai);
  }
  
   /* Receive data */
  do
  { 
    /* Wait until RXNE flag is set */
    if((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_EMPTY)
    {
      if((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
      {
        (*hsai->pBuffPtr++) = hsai->Instance->DR;
      }
      else if(hsai->Init.DataSize <= SAI_DATASIZE_16)
      {
        *((uint16_t*)hsai->pBuffPtr) = hsai->Instance->DR;
        hsai->pBuffPtr+= 2;
      }
      else
      {
        *((uint32_t*)hsai->pBuffPtr) = hsai->Instance->DR;
        hsai->pBuffPtr+= 4;
      }  
      hsai->XferCount--; 
    }
    else
    {
      /* Check for the Timeout */
      if((Timeout != HAL_MAX_DELAY) && ((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout)))
      {
        errorcode = HAL_TIMEOUT;
        goto error;
      }
    }
  }
  while(0 != hsai->XferCount);
  hsai->State = HAL_SAI_STATE_READY;
    
error : 
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit_IT(SAI_HandleTypeDef *hsai, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);  
  
  if((pData == NULL) || (Size == 0)) 
  {
    errorcode = HAL_ERROR;
    goto error;
  }
  
  if(hsai->State != HAL_SAI_STATE_READY)
  {
    errorcode = HAL_BUSY;
    goto error;
  }
  
  hsai->pBuffPtr = pData;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  hsai->State = HAL_SAI_STATE_BUSY_TX;
  if((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
  {
    hsai->InterruptServiceRoutine = SAI_Transmit_IT8Bit;
  }
  else if(hsai->Init.DataSize <= SAI_DATASIZE_16)
  {
    hsai->InterruptServiceRoutine = SAI_Transmit_IT16Bit;
  }
  else
  {
    hsai->InterruptServiceRoutine = SAI_Transmit_IT32Bit;
  }
  
  /* Fill the fifo before starting the communication */
  SAI_FillFifo(hsai);

  /* Enable FRQ and OVRUDR interrupts */
  __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
  
  /* Check if the SAI is already enabled */ 
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */    
    __HAL_SAI_ENABLE(hsai);
  }
  
error :
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive_IT(SAI_HandleTypeDef *hsai, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  if((pData == NULL) || (Size == 0)) 
  {
    errorcode =  HAL_ERROR;
    goto error;
  }
  
  if(hsai->State != HAL_SAI_STATE_READY)
  {
    errorcode = HAL_BUSY;
    goto error;
  }
  
  hsai->pBuffPtr = pData;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  hsai->State = HAL_SAI_STATE_BUSY_RX;
  if((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
  {
    hsai->InterruptServiceRoutine = SAI_Receive_IT8Bit;
  }
  else if(hsai->Init.DataSize <= SAI_DATASIZE_16)
  {
    hsai->InterruptServiceRoutine = SAI_Receive_IT16Bit;
  }
  else
  {
    hsai->InterruptServiceRoutine = SAI_Receive_IT32Bit;
  }
  
  /* Enable the interrupts */
  __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
  
  /* Check if the SAI is already enabled */ 
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */
    __HAL_SAI_ENABLE(hsai);
  }
  
error :
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}

/**
  * @brief Pause the audio stream playing from the Media.
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
  * @brief Resume the audio stream playing from the Media.
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
  if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */    
    __HAL_SAI_ENABLE(hsai);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return HAL_OK;
}

/**
  * @brief Stop the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DMAStop(SAI_HandleTypeDef *hsai)
{
  HAL_StatusTypeDef errorcode = HAL_OK;

  /* Process Locked */
  __HAL_LOCK(hsai);
  
  /* Disable the SAI DMA request */
  hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;
  
  /* Abort the SAI DMA Channel */
  if(hsai->hdmatx != NULL)
  {
    if(HAL_DMA_Abort(hsai->hdmatx) != HAL_OK)
    {
      errorcode = HAL_ERROR;
      goto error;
    }
  }

  if(hsai->hdmarx != NULL)
  {
    if(HAL_DMA_Abort(hsai->hdmarx) != HAL_OK)
    {
      errorcode = HAL_ERROR;
      goto error;
    }
  }
  
  /* Disable SAI peripheral */
  SAI_Disable(hsai);
  
  hsai->State = HAL_SAI_STATE_READY;
  
error :
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return errorcode;
}

/**
  * @brief Abort the current transfer and disable the SAI.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Abort(SAI_HandleTypeDef *hsai)
{
  HAL_StatusTypeDef errorcode = HAL_OK;

  /* Disable the SAI DMA request */
  hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;
  
  /* Abort the SAI DMA Channel */
  if(hsai->hdmatx != NULL)
  {
    if(HAL_DMA_Abort(hsai->hdmatx) != HAL_OK)
    {
      errorcode = HAL_ERROR;
      goto error;
    }
  }
  
  if(hsai->hdmarx != NULL)
  {
    if(HAL_DMA_Abort(hsai->hdmarx) != HAL_OK)
    {
      errorcode = HAL_ERROR;
      goto error;
    }
  }
  
  /* Disabled All interrupt and clear all the flag */
  hsai->Instance->IMR = 0;
  hsai->Instance->CLRFR = 0xFFFFFFFF;
  
  /* Disable SAI peripheral */
  SAI_Disable(hsai);
  
  /* Flush the fifo */
  SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);
  
  hsai->State = HAL_SAI_STATE_READY;
  
error :
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  
  return errorcode;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Transmit_DMA(SAI_HandleTypeDef *hsai, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  if((pData == NULL) || (Size == 0)) 
  {
    errorcode = HAL_ERROR;
    goto error;
  }
  
  if(hsai->State != HAL_SAI_STATE_READY)
  {
    errorcode = HAL_BUSY;
    goto error;  
  }
  
  hsai->pBuffPtr = pData;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE;
  hsai->State = HAL_SAI_STATE_BUSY_TX;
  
  /* Set the SAI Tx DMA Half transfer complete callback */
  hsai->hdmatx->XferHalfCpltCallback = SAI_DMATxHalfCplt;
  
  /* Set the SAI TxDMA transfer complete callback */
  hsai->hdmatx->XferCpltCallback = SAI_DMATxCplt;
  
  /* Set the DMA error callback */
  hsai->hdmatx->XferErrorCallback = SAI_DMAError;
  
  /* Enable the Tx DMA Channel */
  if(HAL_DMA_Start_IT(hsai->hdmatx, (uint32_t)hsai->pBuffPtr, (uint32_t)&hsai->Instance->DR, hsai->XferSize) != HAL_OK)
  {
    errorcode = HAL_ERROR;
    goto error;  
  }
  
  /* Check if the SAI is already enabled */ 
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */
    __HAL_SAI_ENABLE(hsai);
  }
  
  /* Enable the interrupts for error handling */
  __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_DMA));
  
  /* Enable SAI Tx DMA Request */  
  hsai->Instance->CR1 |= SAI_xCR1_DMAEN;
  
error :     
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA. 
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_Receive_DMA(SAI_HandleTypeDef *hsai, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  /* Process Locked */
  __HAL_LOCK(hsai);
  
  if((pData == NULL) || (Size == 0))
  {
    errorcode = HAL_ERROR;
    goto error;
  } 
  
  if(hsai->State != HAL_SAI_STATE_READY)
  {    
    errorcode = HAL_BUSY;
    goto error;
  }
  hsai->pBuffPtr = pData;
  hsai->XferSize = Size;
  hsai->XferCount = Size;
  hsai->ErrorCode = HAL_SAI_ERROR_NONE; 
  hsai->State = HAL_SAI_STATE_BUSY_RX;
  
  /* Set the SAI Rx DMA Half transfer complete callback */
  hsai->hdmarx->XferHalfCpltCallback = SAI_DMARxHalfCplt;
  
  /* Set the SAI Rx DMA transfer complete callback */
  hsai->hdmarx->XferCpltCallback = SAI_DMARxCplt;
  
  /* Set the DMA error callback */
  hsai->hdmarx->XferErrorCallback = SAI_DMAError;
  
  /* Enable the Rx DMA Channel */
  if(HAL_DMA_Start_IT(hsai->hdmarx, (uint32_t)&hsai->Instance->DR, (uint32_t)hsai->pBuffPtr, hsai->XferSize) != HAL_OK)
  {    
    errorcode = HAL_ERROR;
    goto error;
  }
  
  /* Check if the SAI is already enabled */
  if((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
  {
    /* Enable SAI peripheral */
    __HAL_SAI_ENABLE(hsai);
  }
  
  /* Enable the interrupts for error handling */
  __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_DMA));
    
  /* Enable SAI Rx DMA Request */
  hsai->Instance->CR1 |= SAI_xCR1_DMAEN;
  
error :     
  /* Process Unlocked */
  __HAL_UNLOCK(hsai);
  return errorcode;
}

/**
  * @brief  Enable the Tx mute mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  val:  value sent during the mute @ref SAI_Block_Mute_Value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_EnableTxMuteMode(SAI_HandleTypeDef *hsai, uint16_t val)
{
  assert_param(IS_SAI_BLOCK_MUTE_VALUE(val));
  
  if(hsai->State != HAL_SAI_STATE_RESET)
  {
    CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTEVAL | SAI_xCR2_MUTE);
    SET_BIT(hsai->Instance->CR2, SAI_xCR2_MUTE | val);
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Disable the Tx mute mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DisableTxMuteMode(SAI_HandleTypeDef *hsai)
{
  if(hsai->State != HAL_SAI_STATE_RESET)
  {
    CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTEVAL | SAI_xCR2_MUTE);
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Enable the Rx mute detection.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  callback: function called when the mute is detected
  * @param  counter: number a data before mute detection max 63.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_EnableRxMuteMode(SAI_HandleTypeDef *hsai, SAIcallback callback, uint16_t counter)
{
  assert_param(IS_SAI_BLOCK_MUTE_COUNTER(counter));
  
  if(hsai->State != HAL_SAI_STATE_RESET)
  {
    /* set the mute counter */
    CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTECNT);
    SET_BIT(hsai->Instance->CR2, (counter << SAI_xCR2_MUTECNT_OFFSET));
    hsai->mutecallback = callback;
    /* enable the IT interrupt */
    __HAL_SAI_ENABLE_IT(hsai, SAI_IT_MUTEDET);
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Disable the Rx mute detection.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SAI_DisableRxMuteMode(SAI_HandleTypeDef *hsai)
{
  if(hsai->State != HAL_SAI_STATE_RESET)
  {
    /* set the mutecallback to NULL */
    hsai->mutecallback = (SAIcallback)NULL;
    /* enable the IT interrupt */
    __HAL_SAI_DISABLE_IT(hsai, SAI_IT_MUTEDET);
    return HAL_OK;
  }
  return HAL_ERROR;
}

/**
  * @brief  Handle SAI interrupt request.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai)
{ 
  if(hsai->State != HAL_SAI_STATE_RESET)
  {
    __IO uint32_t tmpFlag = hsai->Instance->SR;
    __IO uint32_t tmpItSource = hsai->Instance->IMR; 
    
    if(((tmpFlag & SAI_xSR_FREQ) == SAI_xSR_FREQ) && ((tmpItSource & SAI_IT_FREQ) == SAI_IT_FREQ))
    {
      hsai->InterruptServiceRoutine(hsai);
      goto exit;
    }
    
    /* check the flag only if one of them is set */
    if(tmpFlag != 0x00000000)
    {
      /* SAI Overrun error interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_OVRUDR) == SAI_FLAG_OVRUDR) && ((tmpItSource & SAI_IT_OVRUDR) == SAI_IT_OVRUDR))
      {
        /* Clear the SAI Overrun flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
        /* Change the SAI error code */
        hsai->ErrorCode = ((hsai->State == HAL_SAI_STATE_BUSY_RX) ? HAL_SAI_ERROR_OVR : HAL_SAI_ERROR_UDR);
        /* the transfer is not stopped, we will forward the information to the user and we let the user decide what needs to be done */
        HAL_SAI_ErrorCallback(hsai);
        goto exit;
      }
      
      /* SAI mutedet interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_MUTEDET) == SAI_FLAG_MUTEDET) && ((tmpItSource & SAI_IT_MUTEDET) == SAI_IT_MUTEDET))
      {
        /* Clear the SAI mutedet flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_MUTEDET);
        /* call the call back function */
        if(hsai->mutecallback != (SAIcallback)NULL)
        {
          /* inform the user that an RX mute event has been detected */
          hsai->mutecallback();
        }
        goto exit;
      }
      
      /* SAI AFSDET interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_AFSDET) == SAI_FLAG_AFSDET) && ((tmpItSource & SAI_IT_AFSDET) == SAI_IT_AFSDET))
      {
        /* Change the SAI error code */
        hsai->ErrorCode = HAL_SAI_ERROR_AFSDET;
        goto error;
      }
      
      /* SAI LFSDET interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_LFSDET) == SAI_FLAG_LFSDET) && ((tmpItSource & SAI_IT_LFSDET) == SAI_IT_LFSDET))
      {
        /* Change the SAI error code */
        hsai->ErrorCode = HAL_SAI_ERROR_LFSDET;
        goto error;
      }

      /* SAI WCKCFG interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_WCKCFG) == SAI_FLAG_WCKCFG) && ((tmpItSource & SAI_IT_WCKCFG) == SAI_IT_WCKCFG))
      {
        /* Change the SAI error code */
        hsai->ErrorCode = HAL_SAI_ERROR_WCKCFG;
        goto error;
      }

      /* SAI CNRDY interrupt occurred ----------------------------------*/
      if(((tmpFlag & SAI_FLAG_CNRDY) == SAI_FLAG_CNRDY) && ((tmpItSource & SAI_IT_CNRDY) == SAI_IT_CNRDY))
      {
        /* Clear the SAI CNRDY flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_CNRDY);
        /* Change the SAI error code */
        hsai->ErrorCode = HAL_SAI_ERROR_CNREADY;
        /* the transfer is not stopped, we will forward the information to the user and we let the user decide what needs to be done */
        HAL_SAI_ErrorCallback(hsai);
        goto exit;
      }
    }
  }
  
exit :
  return;
error :
  /* Abort the current communication, disable the SAI block, and clear all the flags */
  HAL_SAI_Abort(hsai);
  HAL_SAI_ErrorCallback(hsai);
  return;
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_TxCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief Tx Transfer Half completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_TxHalfCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief Rx Transfer half completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_RxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief SAI error callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */


/** @defgroup SAI_Exported_Functions_Group3 Peripheral State functions 
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
  * @brief  Return the SAI handle state.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval HAL state
  */
HAL_SAI_StateTypeDef HAL_SAI_GetState(SAI_HandleTypeDef *hsai)
{
  /* Return SAI handle state */
  return hsai->State;
}

/**
* @brief  Return the SAI error code.
* @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
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
  * @}
  */

/** @addtogroup SAI_Private_Functions 
 *  @brief   Private functions 
  * @{
  */
/**
  * @brief  Initialize the SAI I2S protocol according to the specified parameters 
  *         in the SAI_InitTypeDef and create the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  protocol: one of the supported protocol
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  *                the configuration information for SAI module.
  * @param  nbslot: number of slot minimum value is 2 and max is 16. 
  *                    the value must be a multiple of 2.
  * @retval HAL status
  */
static HAL_StatusTypeDef SAI_InitI2S(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot)
{
  HAL_StatusTypeDef errorcode = HAL_OK;

  hsai->Init.Protocol            = SAI_FREE_PROTOCOL;
  hsai->Init.FirstBit            = SAI_FIRSTBIT_MSB;
  /* Compute ClockStrobing according AudioMode */
  if((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
  { /* Transmit */
    hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
  }
  else
  { /* Receive */
    hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_RISINGEDGE;
  }
  hsai->FrameInit.FSDefinition   = SAI_FS_CHANNEL_IDENTIFICATION;
  hsai->SlotInit.SlotActive      = SAI_SLOTACTIVE_ALL;
  hsai->SlotInit.FirstBitOffset  = 0;
  hsai->SlotInit.SlotNumber      = nbslot;
  
  /* in IS2 the number of slot must be even */
  if((nbslot & 0x1) != 0 )
  {
    return HAL_ERROR;
  }
    
  switch(protocol)
  {
  case SAI_I2S_STANDARD :
    hsai->FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
    hsai->FrameInit.FSOffset   = SAI_FS_BEFOREFIRSTBIT;
    break;
  case SAI_I2S_MSBJUSTIFIED :
  case SAI_I2S_LSBJUSTIFIED :
    hsai->FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
    hsai->FrameInit.FSOffset   = SAI_FS_FIRSTBIT;
    break;
  default :
    return HAL_ERROR;
  }
   
  /* Frame definition */
  if(datasize == SAI_PROTOCOL_DATASIZE_16BIT)
  {
    hsai->Init.DataSize = SAI_DATASIZE_16;
    hsai->FrameInit.FrameLength = 32*(nbslot/2);
    hsai->FrameInit.ActiveFrameLength = 16*(nbslot/2);
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_16B;
  }
  else if(datasize == SAI_PROTOCOL_DATASIZE_16BITEXTENDED)
  {
    hsai->Init.DataSize = SAI_DATASIZE_16;
    hsai->FrameInit.FrameLength = 64*(nbslot/2);
    hsai->FrameInit.ActiveFrameLength = 32*(nbslot/2);
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  }
  else if(datasize == SAI_PROTOCOL_DATASIZE_24BIT)
  { 
    hsai->Init.DataSize = SAI_DATASIZE_24;
    hsai->FrameInit.FrameLength = 64*(nbslot/2);
    hsai->FrameInit.ActiveFrameLength = 32*(nbslot/2);
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  }
  else if(datasize == SAI_PROTOCOL_DATASIZE_32BIT)
  {
    hsai->Init.DataSize = SAI_DATASIZE_32;
    hsai->FrameInit.FrameLength = 64*(nbslot/2);
    hsai->FrameInit.ActiveFrameLength = 32*(nbslot/2);
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  }
  else
  {
    errorcode = HAL_ERROR;
  }
    
  if(errorcode == HAL_OK)
  {
    if(protocol == SAI_I2S_LSBJUSTIFIED)
    {
      if (datasize == SAI_PROTOCOL_DATASIZE_16BITEXTENDED)
      {
        hsai->SlotInit.FirstBitOffset = 16;
      }
      else if (datasize == SAI_PROTOCOL_DATASIZE_24BIT)
      {
        hsai->SlotInit.FirstBitOffset = 8;
      }
    }
  }

  return errorcode;
}

/**
  * @brief  Initialize the SAI PCM protocol according to the specified parameters 
  *         in the SAI_InitTypeDef and create the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  protocol: one of the supported protocol
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  * @param  nbslot: number of slot minimum value is 1 and the max is 16.
  * @retval HAL status
  */
static HAL_StatusTypeDef SAI_InitPCM(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot)
{
  hsai->Init.Protocol            = SAI_FREE_PROTOCOL;
  hsai->Init.FirstBit            = SAI_FIRSTBIT_MSB;
  /* Compute ClockStrobing according AudioMode */
  if((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
  { /* Transmit */
    hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_RISINGEDGE;
  }
  else
  { /* Receive */
    hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
  }
  hsai->FrameInit.FSDefinition   = SAI_FS_STARTFRAME;
  hsai->FrameInit.FSPolarity     = SAI_FS_ACTIVE_HIGH;
  hsai->FrameInit.FSOffset       = SAI_FS_BEFOREFIRSTBIT;
  hsai->SlotInit.FirstBitOffset  = 0;
  hsai->SlotInit.SlotNumber      = nbslot;
  hsai->SlotInit.SlotActive      = SAI_SLOTACTIVE_ALL;
  
  switch(protocol)
  {
  case SAI_PCM_SHORT :
    hsai->FrameInit.ActiveFrameLength = 1;
    break;
  case SAI_PCM_LONG :
    hsai->FrameInit.ActiveFrameLength = 13;
    break;
  default :
    return HAL_ERROR;
  }
 
  switch(datasize)
  {
  case SAI_PROTOCOL_DATASIZE_16BIT:
    hsai->Init.DataSize = SAI_DATASIZE_16;
    hsai->FrameInit.FrameLength = 16 * nbslot;
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_16B;
    break; 
  case SAI_PROTOCOL_DATASIZE_16BITEXTENDED :
    hsai->Init.DataSize = SAI_DATASIZE_16;
    hsai->FrameInit.FrameLength = 32 * nbslot;
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
    break;
  case SAI_PROTOCOL_DATASIZE_24BIT :
    hsai->Init.DataSize = SAI_DATASIZE_24;
    hsai->FrameInit.FrameLength = 32 * nbslot;
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
    break;
  case SAI_PROTOCOL_DATASIZE_32BIT: 
    hsai->Init.DataSize = SAI_DATASIZE_32;
    hsai->FrameInit.FrameLength = 32 * nbslot;
    hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
    break;
  default :
    return HAL_ERROR;
  }
 
  return HAL_OK;
}

/**
  * @brief  Fill the fifo.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_FillFifo(SAI_HandleTypeDef *hsai)
{
  /* fill the fifo with data before to enabled the SAI */
  while(((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_FULL) && (hsai->XferCount > 0))
  {
    if((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
    {
      hsai->Instance->DR = (*hsai->pBuffPtr++);
    }
    else if(hsai->Init.DataSize <= SAI_DATASIZE_16)
    {
      hsai->Instance->DR = *((uint32_t *)hsai->pBuffPtr);
      hsai->pBuffPtr+= 2;
    }
    else
    {
      hsai->Instance->DR = *((uint32_t *)hsai->pBuffPtr);
      hsai->pBuffPtr+= 4;
    }
    hsai->XferCount--;
  }
}

/**
  * @brief  Return the interrupt flag to set according the SAI setup.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @param  mode: SAI_MODE_DMA or SAI_MODE_IT
  * @retval the list of the IT flag to enable
 */
static int32_t SAI_InterruptFlag(SAI_HandleTypeDef *hsai, uint32_t mode)
{
  int32_t tmpIT = SAI_IT_OVRUDR; 
  
  if(mode == SAI_MODE_IT)
  {
    tmpIT|= SAI_IT_FREQ;
  }
  
  if((hsai->Init.Protocol == SAI_AC97_PROTOCOL) && 
    ((hsai->Init.AudioMode == SAI_MODESLAVE_RX) || (hsai->Init.AudioMode == SAI_MODEMASTER_RX)))
  {
    tmpIT|= SAI_IT_CNRDY;
  }
  
  if((hsai->Init.AudioMode == SAI_MODESLAVE_RX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
  {
    tmpIT|= SAI_IT_AFSDET | SAI_IT_LFSDET;
  }
  else
  {
    /* hsai has been configured in master mode */
    tmpIT|= SAI_IT_WCKCFG;
  }
  return tmpIT;
}

/**
  * @brief  Disable the SAI and wait for the disabling.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static HAL_StatusTypeDef SAI_Disable(SAI_HandleTypeDef *hsai)
{
  uint32_t tickstart = HAL_GetTick();
  HAL_StatusTypeDef errorcode = HAL_OK;
  
  __HAL_SAI_DISABLE(hsai);
  while((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != RESET)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > SAI_DEFAULT_TIMEOUT)
    {
      errorcode = HAL_TIMEOUT;
      goto error;
    }
  }
error:
  return errorcode;
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode 8-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT8Bit(SAI_HandleTypeDef *hsai)
{
  if(hsai->XferCount == 0)
  {
    /* Handle the end of the transmission */
    /* Disable FREQ and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT)); 
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_TxCpltCallback(hsai);
  }
  else
  {
    /* Write data on DR register */
    hsai->Instance->DR = (*hsai->pBuffPtr++);
    hsai->XferCount--;
  }
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode for 16-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT16Bit(SAI_HandleTypeDef *hsai)
{
  if(hsai->XferCount == 0)
  {
    /* Handle the end of the transmission */
    /* Disable FREQ and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT)); 
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_TxCpltCallback(hsai);
  }
  else
  {
    /* Write data on DR register */
    hsai->Instance->DR = *(uint16_t *)hsai->pBuffPtr;
    hsai->pBuffPtr+=2;
    hsai->XferCount--;
  }
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode for 32-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT32Bit(SAI_HandleTypeDef *hsai)
{
  if(hsai->XferCount == 0)
  {
    /* Handle the end of the transmission */
    /* Disable FREQ and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT)); 
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_TxCpltCallback(hsai);
  }
  else
  {
    /* Write data on DR register */
    hsai->Instance->DR = *(uint32_t *)hsai->pBuffPtr;
    hsai->pBuffPtr+=4;
    hsai->XferCount--;
  }
}

/**
  * @brief  Rx Handler for Receive in Interrupt mode 8-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT8Bit(SAI_HandleTypeDef *hsai)
{
  /* Receive data */    
  (*hsai->pBuffPtr++) = hsai->Instance->DR;
  hsai->XferCount--;
  
  /* Check end of the transfer */  
  if(hsai->XferCount == 0)
  {    
    /* Disable TXE and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
    
    /* Clear the SAI Overrun flag */
    __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
    
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_RxCpltCallback(hsai); 
  }
}

/**
  * @brief  Rx Handler for Receive in Interrupt mode for 16-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT16Bit(SAI_HandleTypeDef *hsai)
{
  /* Receive data */    
  *(uint16_t*)hsai->pBuffPtr = hsai->Instance->DR;
  hsai->pBuffPtr+=2;
  hsai->XferCount--;
  
  /* Check end of the transfer */  
  if(hsai->XferCount == 0)
  {    
    /* Disable TXE and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
    
    /* Clear the SAI Overrun flag */
    __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
    
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_RxCpltCallback(hsai); 
  }
}
/**
  * @brief  Rx Handler for Receive in Interrupt mode for 32-bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT32Bit(SAI_HandleTypeDef *hsai)
{
  /* Receive data */    
  *(uint32_t*)hsai->pBuffPtr = hsai->Instance->DR;
  hsai->pBuffPtr+=4;
  hsai->XferCount--;
  
  /* Check end of the transfer */  
  if(hsai->XferCount == 0)
  {    
    /* Disable TXE and OVRUDR interrupts */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
    
    /* Clear the SAI Overrun flag */
    __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);
    
    hsai->State = HAL_SAI_STATE_READY;
    HAL_SAI_RxCpltCallback(hsai); 
  }
}
/**
  * @brief DMA SAI transmit process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMATxCplt(DMA_HandleTypeDef *hdma)   
{
  SAI_HandleTypeDef* hsai = (SAI_HandleTypeDef*)((DMA_HandleTypeDef* )hdma)->Parent;

  if((hdma->Instance->CCR & DMA_CCR_CIRC) == 0)
  { 
    hsai->XferCount = 0;
    
    /* Disable SAI Tx DMA Request */  
    hsai->Instance->CR1 &= (uint32_t)(~SAI_xCR1_DMAEN);
    
    /* Stop the interrupts error handling */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_DMA));
    
    hsai->State= HAL_SAI_STATE_READY;
  }
  HAL_SAI_TxCpltCallback(hsai);
}

/**
  * @brief DMA SAI transmit process half complete callback.
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
  if((hdma->Instance->CCR & DMA_CCR_CIRC) == 0)
  {
    /* Disable Rx DMA Request */
    hsai->Instance->CR1 &= (uint32_t)(~SAI_xCR1_DMAEN);
    hsai->XferCount = 0;
    
    /* Stop the interrupts error handling */
    __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_DMA));
    
    hsai->State = HAL_SAI_STATE_READY;
  }
  HAL_SAI_RxCpltCallback(hsai); 
}

/**
  * @brief DMA SAI receive process half complete callback. 
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
  
  /* Stop the DMA transfer */
  HAL_SAI_DMAStop(hsai);
  
  /* Set the SAI state ready to be able to start again the process */
  hsai->State= HAL_SAI_STATE_READY;
  HAL_SAI_ErrorCallback(hsai);
  hsai->XferCount = 0;
}

/**
  * @}
  */

#endif /* HAL_SAI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
