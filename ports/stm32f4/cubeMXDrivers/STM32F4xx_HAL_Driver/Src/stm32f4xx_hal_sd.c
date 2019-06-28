/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sd.c
  * @author  MCD Application Team
  * @brief   SD card HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Secure Digital (SD) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + SD card Control functions
  *         
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    This driver implements a high level communication layer for read and write from/to 
    this memory. The needed STM32 hardware resources (SDIO and GPIO) are performed by 
    the user in HAL_SD_MspInit() function (MSP layer).                             
    Basically, the MSP layer configuration should be the same as we provide in the 
    examples.
    You can easily tailor this configuration according to hardware resources.

  [..]
    This driver is a generic layered driver for SDIO memories which uses the HAL 
    SDIO driver functions to interface with SD and uSD cards devices. 
    It is used as follows:
 
    (#)Initialize the SDIO low level resources by implement the HAL_SD_MspInit() API:
        (##) Enable the SDIO interface clock using __HAL_RCC_SDIO_CLK_ENABLE(); 
        (##) SDIO pins configuration for SD card
            (+++) Enable the clock for the SDIO GPIOs using the functions __HAL_RCC_GPIOx_CLK_ENABLE();   
            (+++) Configure these SDIO pins as alternate function pull-up using HAL_GPIO_Init()
                  and according to your pin assignment;
        (##) DMA Configuration if you need to use DMA process (HAL_SD_ReadBlocks_DMA()
             and HAL_SD_WriteBlocks_DMA() APIs).
            (+++) Enable the DMAx interface clock using __HAL_RCC_DMAx_CLK_ENABLE(); 
            (+++) Configure the DMA using the function HAL_DMA_Init() with predeclared and filled. 
        (##) NVIC configuration if you need to use interrupt process when using DMA transfer.
            (+++) Configure the SDIO and DMA interrupt priorities using functions
                  HAL_NVIC_SetPriority(); DMA priority is superior to SDIO's priority
            (+++) Enable the NVIC DMA and SDIO IRQs using function HAL_NVIC_EnableIRQ()
            (+++) SDIO interrupts are managed using the macros __HAL_SD_ENABLE_IT() 
                  and __HAL_SD_DISABLE_IT() inside the communication process.
            (+++) SDIO interrupts pending bits are managed using the macros __HAL_SD_GET_IT()
                  and __HAL_SD_CLEAR_IT()
        (##) NVIC configuration if you need to use interrupt process (HAL_SD_ReadBlocks_IT()
             and HAL_SD_WriteBlocks_IT() APIs).
            (+++) Configure the SDIO interrupt priorities using function
                  HAL_NVIC_SetPriority();
            (+++) Enable the NVIC SDIO IRQs using function HAL_NVIC_EnableIRQ()
            (+++) SDIO interrupts are managed using the macros __HAL_SD_ENABLE_IT() 
                  and __HAL_SD_DISABLE_IT() inside the communication process.
            (+++) SDIO interrupts pending bits are managed using the macros __HAL_SD_GET_IT()
                  and __HAL_SD_CLEAR_IT()
    (#) At this stage, you can perform SD read/write/erase operations after SD card initialization  

         
  *** SD Card Initialization and configuration ***
  ================================================    
  [..]
    To initialize the SD Card, use the HAL_SD_Init() function. It Initializes 
    SDIO IP(STM32 side) and the SD Card, and put it into StandBy State (Ready for data transfer). 
    This function provide the following operations:

    (#) Initialize the SDIO peripheral interface with defaullt configuration.
        The initialization process is done at 400KHz. You can change or adapt 
        this frequency by adjusting the "ClockDiv" field. 
        The SD Card frequency (SDIO_CK) is computed as follows:
  
           SDIO_CK = SDIOCLK / (ClockDiv + 2)
  
        In initialization mode and according to the SD Card standard, 
        make sure that the SDIO_CK frequency doesn't exceed 400KHz.

        This phase of initialization is done through SDIO_Init() and 
        SDIO_PowerState_ON() SDIO low level APIs.

    (#) Initialize the SD card. The API used is HAL_SD_InitCard().
        This phase allows the card initialization and identification 
        and check the SD Card type (Standard Capacity or High Capacity)
        The initialization flow is compatible with SD standard.

        This API (HAL_SD_InitCard()) could be used also to reinitialize the card in case 
        of plug-off plug-in.
  
    (#) Configure the SD Card Data transfer frequency. By Default, the card transfer 
        frequency is set to 24MHz. You can change or adapt this frequency by adjusting 
        the "ClockDiv" field.
        In transfer mode and according to the SD Card standard, make sure that the 
        SDIO_CK frequency doesn't exceed 25MHz and 50MHz in High-speed mode switch.
        To be able to use a frequency higher than 24MHz, you should use the SDIO 
        peripheral in bypass mode. Refer to the corresponding reference manual 
        for more details.
  
    (#) Select the corresponding SD Card according to the address read with the step 2.
    
    (#) Configure the SD Card in wide bus mode: 4-bits data.
  
  *** SD Card Read operation ***
  ==============================
  [..] 
    (+) You can read from SD card in polling mode by using function HAL_SD_ReadBlocks(). 
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.

    (+) You can read from SD card in DMA mode by using function HAL_SD_ReadBlocks_DMA().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.
        You could also check the DMA transfer process through the SD Rx interrupt event.

    (+) You can read from SD card in Interrupt mode by using function HAL_SD_ReadBlocks_IT().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.
        You could also check the IT transfer process through the SD Rx interrupt event.
  
  *** SD Card Write operation ***
  =============================== 
  [..] 
    (+) You can write to SD card in polling mode by using function HAL_SD_WriteBlocks(). 
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.

    (+) You can write to SD card in DMA mode by using function HAL_SD_WriteBlocks_DMA().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.
        You could also check the DMA transfer process through the SD Tx interrupt event.  

    (+) You can write to SD card in Interrupt mode by using function HAL_SD_WriteBlocks_IT().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_SD_GetCardState() function for SD card state.
        You could also check the IT transfer process through the SD Tx interrupt event.
  
  *** SD card status ***
  ====================== 
  [..]
    (+) The SD Status contains status bits that are related to the SD Memory 
        Card proprietary features. To get SD card status use the HAL_SD_GetCardStatus().

  *** SD card information ***
  =========================== 
  [..]
    (+) To get SD card information, you can use the function HAL_SD_GetCardInfo().
        It returns useful information about the SD card such as block size, card type,
        block number ...

  *** SD card CSD register ***
  ============================
  [..]
    (+) The HAL_SD_GetCardCSD() API allows to get the parameters of the CSD register.
        Some of the CSD parameters are useful for card initialization and identification.

  *** SD card CID register ***
  ============================
  [..]
    (+) The HAL_SD_GetCardCID() API allows to get the parameters of the CID register.
        Some of the CSD parameters are useful for card initialization and identification.

  *** SD HAL driver macros list ***
  ==================================
  [..]
    Below the list of most used macros in SD HAL driver.
       
    (+) __HAL_SD_ENABLE : Enable the SD device
    (+) __HAL_SD_DISABLE : Disable the SD device
    (+) __HAL_SD_DMA_ENABLE: Enable the SDIO DMA transfer
    (+) __HAL_SD_DMA_DISABLE: Disable the SDIO DMA transfer
    (+) __HAL_SD_ENABLE_IT: Enable the SD device interrupt
    (+) __HAL_SD_DISABLE_IT: Disable the SD device interrupt
    (+) __HAL_SD_GET_FLAG:Check whether the specified SD flag is set or not
    (+) __HAL_SD_CLEAR_FLAG: Clear the SD's pending flags

   [..]
    (@) You can refer to the SD HAL driver header file for more useful macros 
      
  *** Callback registration ***
  =============================================
  [..]
    The compilation define USE_HAL_SD_REGISTER_CALLBACKS when set to 1
    allows the user to configure dynamically the driver callbacks.

    Use Functions @ref HAL_SD_RegisterCallback() to register a user callback,
    it allows to register following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) AbortCpltCallback : callback when abort is completed.
      (+) MspInitCallback    : SD MspInit.
      (+) MspDeInitCallback  : SD MspDeInit.
    This function takes as parameters the HAL peripheral handle, the Callback ID
    and a pointer to the user callback function.

    Use function @ref HAL_SD_UnRegisterCallback() to reset a callback to the default
    weak (surcharged) function. It allows to reset following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) AbortCpltCallback : callback when abort is completed.
      (+) MspInitCallback    : SD MspInit.
      (+) MspDeInitCallback  : SD MspDeInit.
    This function) takes as parameters the HAL peripheral handle and the Callback ID.

    By default, after the @ref HAL_SD_Init and if the state is HAL_SD_STATE_RESET
    all callbacks are reset to the corresponding legacy weak (surcharged) functions.
    Exception done for MspInit and MspDeInit callbacks that are respectively
    reset to the legacy weak (surcharged) functions in the @ref HAL_SD_Init 
    and @ref  HAL_SD_DeInit only when these callbacks are null (not registered beforehand).
    If not, MspInit or MspDeInit are not null, the @ref HAL_SD_Init and @ref HAL_SD_DeInit
    keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

    Callbacks can be registered/unregistered in READY state only.
    Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
    in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
    during the Init/DeInit.
    In that case first register the MspInit/MspDeInit user callbacks
    using @ref HAL_SD_RegisterCallback before calling @ref HAL_SD_DeInit 
    or @ref HAL_SD_Init function.

    When The compilation define USE_HAL_SD_REGISTER_CALLBACKS is set to 0 or
    not defined, the callback registering feature is not available 
    and weak (surcharged) callbacks are used.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup SD 
  * @{
  */

#ifdef HAL_SD_MODULE_ENABLED

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx) || \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) || defined(STM32F423xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup SD_Private_Defines
  * @{
  */
    
/**
  * @}
  */
  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup SD_Private_Functions SD Private Functions
  * @{
  */
static uint32_t SD_InitCard(SD_HandleTypeDef *hsd);
static uint32_t SD_PowerON(SD_HandleTypeDef *hsd);                      
static uint32_t SD_SendSDStatus(SD_HandleTypeDef *hsd, uint32_t *pSDstatus);
static uint32_t SD_SendStatus(SD_HandleTypeDef *hsd, uint32_t *pCardStatus);
static uint32_t SD_WideBus_Enable(SD_HandleTypeDef *hsd);
static uint32_t SD_WideBus_Disable(SD_HandleTypeDef *hsd);
static uint32_t SD_FindSCR(SD_HandleTypeDef *hsd, uint32_t *pSCR);
static HAL_StatusTypeDef SD_PowerOFF(SD_HandleTypeDef *hsd);
static HAL_StatusTypeDef SD_Write_IT(SD_HandleTypeDef *hsd);
static HAL_StatusTypeDef SD_Read_IT(SD_HandleTypeDef *hsd);
static void SD_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SD_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SD_DMAError(DMA_HandleTypeDef *hdma);
static void SD_DMATxAbort(DMA_HandleTypeDef *hdma);
static void SD_DMARxAbort(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SD_Exported_Functions
  * @{
  */

/** @addtogroup SD_Exported_Functions_Group1
 *  @brief   Initialization and de-initialization functions 
 *
@verbatim    
  ==============================================================================
          ##### Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize the SD
    card device to be ready for use.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the SD according to the specified parameters in the 
            SD_HandleTypeDef and create the associated handle.
  * @param  hsd Pointer to the SD handle  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_Init(SD_HandleTypeDef *hsd)
{
  /* Check the SD handle allocation */
  if(hsd == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SDIO_ALL_INSTANCE(hsd->Instance));
  assert_param(IS_SDIO_CLOCK_EDGE(hsd->Init.ClockEdge));
  assert_param(IS_SDIO_CLOCK_BYPASS(hsd->Init.ClockBypass));
  assert_param(IS_SDIO_CLOCK_POWER_SAVE(hsd->Init.ClockPowerSave));
  assert_param(IS_SDIO_BUS_WIDE(hsd->Init.BusWide));
  assert_param(IS_SDIO_HARDWARE_FLOW_CONTROL(hsd->Init.HardwareFlowControl));
  assert_param(IS_SDIO_CLKDIV(hsd->Init.ClockDiv));

  if(hsd->State == HAL_SD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hsd->Lock = HAL_UNLOCKED;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
    /* Reset Callback pointers in HAL_SD_STATE_RESET only */
    hsd->TxCpltCallback    = HAL_SD_TxCpltCallback;
    hsd->RxCpltCallback    = HAL_SD_RxCpltCallback;
    hsd->ErrorCallback     = HAL_SD_ErrorCallback;
    hsd->AbortCpltCallback = HAL_SD_AbortCallback;

    if(hsd->MspInitCallback == NULL)
    {
      hsd->MspInitCallback = HAL_SD_MspInit;
    }

    /* Init the low level hardware */
    hsd->MspInitCallback(hsd);
#else
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_SD_MspInit(hsd);
#endif
  }

  hsd->State = HAL_SD_STATE_BUSY;

  /* Initialize the Card parameters */
  HAL_SD_InitCard(hsd);

  /* Initialize the error code */
  hsd->ErrorCode = HAL_DMA_ERROR_NONE;
  
  /* Initialize the SD operation */
  hsd->Context = SD_CONTEXT_NONE;
                                                                                     
  /* Initialize the SD state */
  hsd->State = HAL_SD_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Initializes the SD Card.
  * @param  hsd Pointer to SD handle
  * @note   This function initializes the SD card. It could be used when a card 
            re-initialization is needed.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_InitCard(SD_HandleTypeDef *hsd)
{
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  SD_InitTypeDef Init;
  
  /* Default SDIO peripheral configuration for SD card initialization */
  Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
  Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
  Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
  Init.BusWide             = SDIO_BUS_WIDE_1B;
  Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  Init.ClockDiv            = SDIO_INIT_CLK_DIV;

  /* Initialize SDIO peripheral interface with default configuration */
  SDIO_Init(hsd->Instance, Init);

  /* Disable SDIO Clock */
  __HAL_SD_DISABLE(hsd); 
  
  /* Set Power State to ON */
  SDIO_PowerState_ON(hsd->Instance);
  
  /* Enable SDIO Clock */
  __HAL_SD_ENABLE(hsd);
  
  /* Required power up waiting time before starting the SD initialization 
  sequence */
  HAL_Delay(2U);
  
  /* Identify card operating voltage */
  errorstate = SD_PowerON(hsd);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->State = HAL_SD_STATE_READY;
    hsd->ErrorCode |= errorstate;
    return HAL_ERROR;
  }

  /* Card initialization */
  errorstate = SD_InitCard(hsd);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->State = HAL_SD_STATE_READY;
    hsd->ErrorCode |= errorstate;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  De-Initializes the SD card.
  * @param  hsd Pointer to SD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_DeInit(SD_HandleTypeDef *hsd)
{
  /* Check the SD handle allocation */
  if(hsd == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_SDIO_ALL_INSTANCE(hsd->Instance));

  hsd->State = HAL_SD_STATE_BUSY;
  
  /* Set SD power state to off */ 
  SD_PowerOFF(hsd);
  
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
  if(hsd->MspDeInitCallback == NULL)
  {
    hsd->MspDeInitCallback = HAL_SD_MspDeInit;
  }

  /* DeInit the low level hardware */
  hsd->MspDeInitCallback(hsd);
#else
  /* De-Initialize the MSP layer */
  HAL_SD_MspDeInit(hsd);
#endif
  
  hsd->ErrorCode = HAL_SD_ERROR_NONE;
  hsd->State = HAL_SD_STATE_RESET;
  
  return HAL_OK;
}


/**
  * @brief  Initializes the SD MSP.
  * @param  hsd Pointer to SD handle
  * @retval None
  */
__weak void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
 
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_MspInit could be implemented in the user file
   */
}

/**
  * @brief  De-Initialize SD MSP.
  * @param  hsd Pointer to SD handle
  * @retval None
  */
__weak void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
 
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @addtogroup SD_Exported_Functions_Group2
 *  @brief   Data transfer functions 
 *
@verbatim   
  ==============================================================================
                        ##### IO operation functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to manage the data 
    transfer from/to SD card.

@endverbatim
  * @{
  */

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer 
  *         is managed by polling mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @param  hsd Pointer to SD handle
  * @param  pData pointer to the buffer that will contain the received data
  * @param  BlockAdd Block Address from where data is to be read 
  * @param  NumberOfBlocks Number of SD blocks to read
  * @param  Timeout Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_ReadBlocks(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks, uint32_t Timeout)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count = 0U, *tempbuff = (uint32_t *)pData;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }
 
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
    
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
      
    /* Set Block Size for Card */
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);      
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * BLOCKSIZE;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);
    
    /* Read block(s) in polling mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = SD_CONTEXT_READ_MULTIPLE_BLOCK;
      
      /* Read Multi Block command */ 
      errorstate = SDMMC_CmdReadMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = SD_CONTEXT_READ_SINGLE_BLOCK;
      
      /* Read Single Block command */
      errorstate = SDMMC_CmdReadSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
      
    /* Poll on SDIO flags */
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_STA_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND))
#endif /* SDIO_STA_STBITERR */
    {
      if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        /* Read data from SDIO Rx FIFO */
        for(count = 0U; count < 8U; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        tempbuff += 8U;
      }
      
      if((Timeout == 0U)||((HAL_GetTick()-tickstart) >=  Timeout))
      {
        /* Clear all the static flags */
        __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
        hsd->ErrorCode |= HAL_SD_ERROR_TIMEOUT;
        hsd->State= HAL_SD_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    
    /* Send stop transmission command in case of multiblock read */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1U))
    {    
      if(hsd->SdCard.CardType != CARD_SECURED)
      {
        /* Send stop transmission command */
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if(errorstate != HAL_SD_ERROR_NONE)
        {
          /* Clear all the static flags */
          __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
          hsd->ErrorCode |= errorstate;
          hsd->State = HAL_SD_STATE_READY;
          return HAL_ERROR;
        }
      }
    }
    
    /* Get error state */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_CRC_FAIL;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_RX_OVERRUN;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Empty FIFO if there is still any data */
    while ((__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)))
    {
      *tempbuff = SDIO_ReadFIFO(hsd->Instance);
      tempbuff++;
      
      if((Timeout == 0U)||((HAL_GetTick()-tickstart) >=  Timeout))
      {
        /* Clear all the static flags */
        __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);        
        hsd->ErrorCode |= HAL_SD_ERROR_TIMEOUT;
        hsd->State= HAL_SD_STATE_READY;
        return HAL_ERROR;
      }
    }
    
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    hsd->State = HAL_SD_STATE_READY;
    
    return HAL_OK;
  }
  else
  {
    hsd->ErrorCode |= HAL_SD_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Allows to write block(s) to a specified address in a card. The Data
  *         transfer is managed by polling mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @param  hsd Pointer to SD handle
  * @param  pData pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd Block Address where data will be written  
  * @param  NumberOfBlocks Number of SD blocks to write
  * @param  Timeout Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_WriteBlocks(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks, uint32_t Timeout)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count = 0U;
  uint32_t *tempbuff = (uint32_t *)pData;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }

  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
     
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
    
    /* Set Block Size for Card */ 
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);  
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Write Blocks in Polling mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = SD_CONTEXT_WRITE_MULTIPLE_BLOCK;
      
      /* Write Multi Block command */ 
      errorstate = SDMMC_CmdWriteMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = SD_CONTEXT_WRITE_SINGLE_BLOCK;
      
      /* Write Single Block command */
      errorstate = SDMMC_CmdWriteSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);  
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */ 
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * BLOCKSIZE;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);
    
    /* Write block(s) in polling mode */
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND))
#endif /* SDIO_STA_STBITERR */
    {
      if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_TXFIFOHE))
      {
        /* Write data to SDIO Tx FIFO */
        for(count = 0U; count < 8U; count++)
        {
          SDIO_WriteFIFO(hsd->Instance, (tempbuff + count));
        }
        tempbuff += 8U;
      }
      
      if((Timeout == 0U)||((HAL_GetTick()-tickstart) >=  Timeout))
      {
        /* Clear all the static flags */
        __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);  
        hsd->ErrorCode |= errorstate;
        hsd->State = HAL_SD_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    
    /* Send stop transmission command in case of multiblock write */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1U))
    { 
      if(hsd->SdCard.CardType != CARD_SECURED)
      {
        /* Send stop transmission command */
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if(errorstate != HAL_SD_ERROR_NONE)
        {
          /* Clear all the static flags */
          __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);  
          hsd->ErrorCode |= errorstate;
          hsd->State = HAL_SD_STATE_READY;
          return HAL_ERROR;
        }
      }
    }
    
    /* Get error state */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_CRC_FAIL;      
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_TX_UNDERRUN;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    hsd->State = HAL_SD_STATE_READY;
    
    return HAL_OK;
  }
  else
  {
    hsd->ErrorCode |= HAL_SD_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer 
  *         is managed in interrupt mode. 
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @note   You could also check the IT transfer process through the SD Rx 
  *         interrupt event.
  * @param  hsd Pointer to SD handle                 
  * @param  pData Pointer to the buffer that will contain the received data
  * @param  BlockAdd Block Address from where data is to be read 
  * @param  NumberOfBlocks Number of blocks to read.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_ReadBlocks_IT(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
    
    hsd->pRxBuffPtr = (uint32_t *)pData;
    hsd->RxXferSize = BLOCKSIZE * NumberOfBlocks;
    
#ifdef SDIO_STA_STBITERR
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_DATAEND | SDIO_FLAG_RXFIFOHF | SDIO_IT_STBITERR));
#else /* SDIO_STA_STBITERR not defined */
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_DATAEND | SDIO_FLAG_RXFIFOHF));
#endif /* SDIO_STA_STBITERR */
    
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */ 
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);
    
    /* Set Block Size for Card */ 
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }

    /* Read Blocks in IT mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_IT);
      
      /* Read Multi Block command */
      errorstate = SDMMC_CmdReadMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = (SD_CONTEXT_READ_SINGLE_BLOCK | SD_CONTEXT_IT);
      
      /* Read Single Block command */
      errorstate = SDMMC_CmdReadSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer 
  *         is managed in interrupt mode. 
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @note   You could also check the IT transfer process through the SD Tx 
  *         interrupt event. 
  * @param  hsd Pointer to SD handle
  * @param  pData Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd Block Address where data will be written    
  * @param  NumberOfBlocks Number of blocks to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_WriteBlocks_IT(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
    
    hsd->pTxBuffPtr = (uint32_t *)pData;
    hsd->TxXferSize = BLOCKSIZE * NumberOfBlocks;
    
    /* Enable transfer interrupts */
#ifdef SDIO_STA_STBITERR
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_TXUNDERR | SDIO_IT_DATAEND | SDIO_FLAG_TXFIFOHE | SDIO_IT_STBITERR));    
#else /* SDIO_STA_STBITERR not defined */
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_TXUNDERR | SDIO_IT_DATAEND | SDIO_FLAG_TXFIFOHE));    
#endif /* SDIO_STA_STBITERR */
    
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
    
    /* Set Block Size for Card */ 
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Write Blocks in Polling mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK| SD_CONTEXT_IT);
      
      /* Write Multi Block command */ 
      errorstate = SDMMC_CmdWriteMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = (SD_CONTEXT_WRITE_SINGLE_BLOCK | SD_CONTEXT_IT);
      
      /* Write Single Block command */ 
      errorstate = SDMMC_CmdWriteSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */ 
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer 
  *         is managed by DMA mode. 
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @note   You could also check the DMA transfer process through the SD Rx 
  *         interrupt event.
  * @param  hsd Pointer SD handle                 
  * @param  pData Pointer to the buffer that will contain the received data
  * @param  BlockAdd Block Address from where data is to be read  
  * @param  NumberOfBlocks Number of blocks to read.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_ReadBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
    
#ifdef SDIO_STA_STBITERR
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_DATAEND | SDIO_IT_STBITERR));
#else /* SDIO_STA_STBITERR not defined */
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_DATAEND));
#endif /* SDIO_STA_STBITERR */
    
    /* Set the DMA transfer complete callback */
    hsd->hdmarx->XferCpltCallback = SD_DMAReceiveCplt;
    
    /* Set the DMA error callback */
    hsd->hdmarx->XferErrorCallback = SD_DMAError;
    
    /* Set the DMA Abort callback */
    hsd->hdmarx->XferAbortCallback = NULL;
    
    /* Enable the DMA Channel */
    HAL_DMA_Start_IT(hsd->hdmarx, (uint32_t)&hsd->Instance->FIFO, (uint32_t)pData, (uint32_t)(BLOCKSIZE * NumberOfBlocks)/4);
    
    /* Enable SD DMA transfer */
    __HAL_SD_DMA_ENABLE(hsd);
    
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */ 
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);

    /* Set Block Size for Card */ 
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
        
    /* Read Blocks in DMA mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_DMA);
      
      /* Read Multi Block command */ 
      errorstate = SDMMC_CmdReadMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = (SD_CONTEXT_READ_SINGLE_BLOCK | SD_CONTEXT_DMA);
      
      /* Read Single Block command */ 
      errorstate = SDMMC_CmdReadSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer 
  *         is managed by DMA mode. 
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @note   You could also check the DMA transfer process through the SD Tx 
  *         interrupt event.
  * @param  hsd Pointer to SD handle
  * @param  pData Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd Block Address where data will be written  
  * @param  NumberOfBlocks Number of blocks to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_WriteBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(NULL == pData)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    return HAL_ERROR;
  }
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if((BlockAdd + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Initialize data control register */
    hsd->Instance->DCTRL = 0U;
    
    /* Enable SD Error interrupts */  
#ifdef SDIO_STA_STBITERR
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR));    
#else /* SDIO_STA_STBITERR not defined */
    __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_TXUNDERR));    
#endif /* SDIO_STA_STBITERR */
    
    /* Set the DMA transfer complete callback */
    hsd->hdmatx->XferCpltCallback = SD_DMATransmitCplt;
    
    /* Set the DMA error callback */
    hsd->hdmatx->XferErrorCallback = SD_DMAError;
    
    /* Set the DMA Abort callback */
    hsd->hdmatx->XferAbortCallback = NULL;
    
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockAdd *= 512U;
    }
    
    /* Set Block Size for Card */ 
    errorstate = SDMMC_CmdBlockLength(hsd->Instance, BLOCKSIZE);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Write Blocks in Polling mode */
    if(NumberOfBlocks > 1U)
    {
      hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK | SD_CONTEXT_DMA);
      
      /* Write Multi Block command */ 
      errorstate = SDMMC_CmdWriteMultiBlock(hsd->Instance, BlockAdd);
    }
    else
    {
      hsd->Context = (SD_CONTEXT_WRITE_SINGLE_BLOCK | SD_CONTEXT_DMA);
      
      /* Write Single Block command */
      errorstate = SDMMC_CmdWriteSingleBlock(hsd->Instance, BlockAdd);
    }
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Enable SDIO DMA transfer */
    __HAL_SD_DMA_ENABLE(hsd);
    
    /* Enable the DMA Channel */
    HAL_DMA_Start_IT(hsd->hdmatx, (uint32_t)pData, (uint32_t)&hsd->Instance->FIFO, (uint32_t)(BLOCKSIZE * NumberOfBlocks)/4);
    
    /* Configure the SD DPSM (Data Path State Machine) */ 
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(hsd->Instance, &config);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @note   This API should be followed by a check on the card state through
  *         HAL_SD_GetCardState().
  * @param  hsd Pointer to SD handle 
  * @param  BlockStartAdd Start Block address
  * @param  BlockEndAdd End Block address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_Erase(SD_HandleTypeDef *hsd, uint32_t BlockStartAdd, uint32_t BlockEndAdd)
{
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    hsd->ErrorCode = HAL_DMA_ERROR_NONE;
    
    if(BlockEndAdd < BlockStartAdd)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
      return HAL_ERROR;
    }
    
    if(BlockEndAdd > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_BUSY;
    
    /* Check if the card command class supports erase command */
    if(((hsd->SdCard.Class) & SDIO_CCCC_ERASE) == 0U)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      hsd->ErrorCode |= HAL_SD_ERROR_REQUEST_NOT_APPLICABLE;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    if((SDIO_GetResponse(hsd->Instance, SDIO_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);  
      hsd->ErrorCode |= HAL_SD_ERROR_LOCK_UNLOCK_FAILED;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    /* Get start and end block for high capacity cards */
    if(hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      BlockStartAdd *= 512U;
      BlockEndAdd   *= 512U;
    }
    
    /* According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
    if(hsd->SdCard.CardType != CARD_SECURED)
    {
      /* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
      errorstate = SDMMC_CmdSDEraseStartAdd(hsd->Instance, BlockStartAdd);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        /* Clear all the static flags */
        __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
        hsd->ErrorCode |= errorstate;
        hsd->State = HAL_SD_STATE_READY;
        return HAL_ERROR;
      }
      
      /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
      errorstate = SDMMC_CmdSDEraseEndAdd(hsd->Instance, BlockEndAdd);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        /* Clear all the static flags */
        __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
        hsd->ErrorCode |= errorstate;
        hsd->State = HAL_SD_STATE_READY;
        return HAL_ERROR;
      }
    }
    
    /* Send CMD38 ERASE */
    errorstate = SDMMC_CmdErase(hsd->Instance);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS); 
      hsd->ErrorCode |= errorstate;
      hsd->State = HAL_SD_STATE_READY;
      return HAL_ERROR;
    }
    
    hsd->State = HAL_SD_STATE_READY;
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  This function handles SD card interrupt request.
  * @param  hsd Pointer to SD handle
  * @retval None
  */
void HAL_SD_IRQHandler(SD_HandleTypeDef *hsd)
{
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  /* Check for SDIO interrupt flags */
  if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DATAEND) != RESET)
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_DATAEND); 
    
#ifdef SDIO_STA_STBITERR
    __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                             SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR | SDIO_IT_STBITERR);
#else /* SDIO_STA_STBITERR not defined */
    __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                             SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR);
#endif
    
    if((hsd->Context & SD_CONTEXT_IT) != RESET)
    {
      if(((hsd->Context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != RESET) || ((hsd->Context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != RESET))
      {
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if(errorstate != HAL_SD_ERROR_NONE)
        {
          hsd->ErrorCode |= errorstate;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
          hsd->ErrorCallback(hsd);
#else
          HAL_SD_ErrorCallback(hsd);
#endif
        }
      }
      
      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
      
      hsd->State = HAL_SD_STATE_READY;
      if(((hsd->Context & SD_CONTEXT_READ_SINGLE_BLOCK) != RESET) || ((hsd->Context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != RESET))
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->RxCpltCallback(hsd);
#else
        HAL_SD_RxCpltCallback(hsd);
#endif
      }
      else
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->TxCpltCallback(hsd);
#else      
        HAL_SD_TxCpltCallback(hsd);
#endif
      }
    }
    else if((hsd->Context & SD_CONTEXT_DMA) != RESET)
    {
      if((hsd->Context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != RESET)
      {
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if(errorstate != HAL_SD_ERROR_NONE)
        {
          hsd->ErrorCode |= errorstate;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
          hsd->ErrorCallback(hsd);
#else
          HAL_SD_ErrorCallback(hsd);
#endif
        }
      }
      if(((hsd->Context & SD_CONTEXT_READ_SINGLE_BLOCK) == RESET) && ((hsd->Context & SD_CONTEXT_READ_MULTIPLE_BLOCK) == RESET))
      {
        /* Disable the DMA transfer for transmit request by setting the DMAEN bit
        in the SD DCTRL register */
        hsd->Instance->DCTRL &= (uint32_t)~((uint32_t)SDIO_DCTRL_DMAEN);
        
        hsd->State = HAL_SD_STATE_READY;
        
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->TxCpltCallback(hsd);
#else      
        HAL_SD_TxCpltCallback(hsd);
#endif
      }
    }
  }
  
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_TXFIFOHE) != RESET)
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_TXFIFOHE);
    
    SD_Write_IT(hsd);
  }
  
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_RXFIFOHF) != RESET)
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_RXFIFOHF);
    
    SD_Read_IT(hsd);
  }
  
#ifdef SDIO_STA_STBITERR
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR) != RESET)
  {
    /* Set Error code */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DCRCFAIL) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_CRC_FAIL; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DTIMEOUT) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_RXOVERR) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_RX_OVERRUN; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_TXUNDERR) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_TX_UNDERRUN; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_STBITERR) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT;
    }

    /* Clear All flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS | SDIO_FLAG_STBITERR);
    
    /* Disable all interrupts */
    __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                             SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR |SDIO_IT_STBITERR);
    
    if((hsd->Context & SD_CONTEXT_DMA) != RESET)
    {
      /* Abort the SD DMA Streams */
      if(hsd->hdmatx != NULL)
      {
        /* Set the DMA Tx abort callback */
        hsd->hdmatx->XferAbortCallback = SD_DMATxAbort;
        /* Abort DMA in IT mode */
        if(HAL_DMA_Abort_IT(hsd->hdmatx) != HAL_OK)
        {
          SD_DMATxAbort(hsd->hdmatx);
        }
      }
      else if(hsd->hdmarx != NULL)
      {
        /* Set the DMA Rx abort callback */
        hsd->hdmarx->XferAbortCallback = SD_DMARxAbort;
        /* Abort DMA in IT mode */
        if(HAL_DMA_Abort_IT(hsd->hdmarx) != HAL_OK)
        {
          SD_DMARxAbort(hsd->hdmarx);
        }
      }
      else
      {
        hsd->ErrorCode = HAL_SD_ERROR_NONE;
        hsd->State = HAL_SD_STATE_READY;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->AbortCpltCallback(hsd);
#else
        HAL_SD_AbortCallback(hsd);
#endif
      }
    }
    else if((hsd->Context & SD_CONTEXT_IT) != RESET)
    {
      /* Set the SD state to ready to be able to start again the process */
      hsd->State = HAL_SD_STATE_READY;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
      hsd->ErrorCallback(hsd);
#else
      HAL_SD_ErrorCallback(hsd);
#endif
    }
  }
#else /* SDIO_STA_STBITERR not defined */
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_RXOVERR | SDIO_IT_TXUNDERR) != RESET)
  {
    /* Set Error code */
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DCRCFAIL) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_CRC_FAIL; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_DTIMEOUT) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_RXOVERR) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_RX_OVERRUN; 
    }
    if(__HAL_SD_GET_FLAG(hsd, SDIO_IT_TXUNDERR) != RESET)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_TX_UNDERRUN; 
    }

    /* Clear All flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    /* Disable all interrupts */
    __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                             SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR);
    
    if((hsd->Context & SD_CONTEXT_DMA) != RESET)
    {
      /* Abort the SD DMA Streams */
      if(hsd->hdmatx != NULL)
      {
        /* Set the DMA Tx abort callback */
        hsd->hdmatx->XferAbortCallback = SD_DMATxAbort;
        /* Abort DMA in IT mode */
        if(HAL_DMA_Abort_IT(hsd->hdmatx) != HAL_OK)
        {
          SD_DMATxAbort(hsd->hdmatx);
        }
      }
      else if(hsd->hdmarx != NULL)
      {
        /* Set the DMA Rx abort callback */
        hsd->hdmarx->XferAbortCallback = SD_DMARxAbort;
        /* Abort DMA in IT mode */
        if(HAL_DMA_Abort_IT(hsd->hdmarx) != HAL_OK)
        {
          SD_DMARxAbort(hsd->hdmarx);
        }
      }
      else
      {
        hsd->ErrorCode = HAL_SD_ERROR_NONE;
        hsd->State = HAL_SD_STATE_READY;
        HAL_SD_AbortCallback(hsd);
      }
    }
    else if((hsd->Context & SD_CONTEXT_IT) != RESET)
    {
      /* Set the SD state to ready to be able to start again the process */
      hsd->State = HAL_SD_STATE_READY;
      HAL_SD_ErrorCallback(hsd);
    }
  }
#endif
}

/**
  * @brief return the SD state
  * @param hsd Pointer to sd handle
  * @retval HAL state
  */
HAL_SD_StateTypeDef HAL_SD_GetState(SD_HandleTypeDef *hsd)
{
  return hsd->State;
}

/**
* @brief  Return the SD error code
* @param  hsd  Pointer to a SD_HandleTypeDef structure that contains
  *              the configuration information.
* @retval SD Error Code
*/
uint32_t HAL_SD_GetError(SD_HandleTypeDef *hsd)
{
  return hsd->ErrorCode;
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd Pointer to SD handle
  * @retval None
  */
 __weak void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_TxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd Pointer SD handle
  * @retval None
  */
__weak void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
 
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_RxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief SD error callbacks
  * @param hsd Pointer SD handle
  * @retval None
  */
__weak void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
 
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_ErrorCallback can be implemented in the user file
   */ 
}

/**
  * @brief SD Abort callbacks
  * @param hsd Pointer SD handle
  * @retval None
  */
__weak void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
 
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_ErrorCallback can be implemented in the user file
   */ 
}

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User SD Callback
  *         To be used instead of the weak (surcharged) predefined callback 
  * @param hsd : SD handle
  * @param CallbackId : Id of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SD_TX_CPLT_CB_ID    SD Tx Complete Callback ID
  *          @arg @ref HAL_SD_RX_CPLT_CB_ID    SD Rx Complete Callback ID
  *          @arg @ref HAL_SD_ERROR_CB_ID      SD Error Callback ID
  *          @arg @ref HAL_SD_ABORT_CB_ID      SD Abort Callback ID
  *          @arg @ref HAL_SD_MSP_INIT_CB_ID   SD MspInit Callback ID 
  *          @arg @ref HAL_SD_MSP_DEINIT_CB_ID SD MspDeInit Callback ID  
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SD_RegisterCallback(SD_HandleTypeDef *hsd, HAL_SD_CallbackIDTypeDef CallbackId, pSD_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if(pCallback == NULL)
  {
    /* Update the error code */
    hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hsd);
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    switch (CallbackId)
    {
    case HAL_SD_TX_CPLT_CB_ID :
      hsd->TxCpltCallback = pCallback;
      break;
    case HAL_SD_RX_CPLT_CB_ID :
      hsd->RxCpltCallback = pCallback;
      break;
    case HAL_SD_ERROR_CB_ID :
      hsd->ErrorCallback = pCallback;
      break;
    case HAL_SD_ABORT_CB_ID :
      hsd->AbortCpltCallback = pCallback;
      break;
    case HAL_SD_MSP_INIT_CB_ID :
      hsd->MspInitCallback = pCallback;
      break;
    case HAL_SD_MSP_DEINIT_CB_ID :
      hsd->MspDeInitCallback = pCallback;
      break;
    default :
      /* Update the error code */
      hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (hsd->State == HAL_SD_STATE_RESET)
  {
    switch (CallbackId)
    {
    case HAL_SD_MSP_INIT_CB_ID :
      hsd->MspInitCallback = pCallback;
      break;
    case HAL_SD_MSP_DEINIT_CB_ID :
      hsd->MspDeInitCallback = pCallback;
      break;
    default :
      /* Update the error code */
      hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
    /* Update the error code */
    hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
    /* update return status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hsd);
  return status;
}

/**
  * @brief  Unregister a User SD Callback
  *         SD Callback is redirected to the weak (surcharged) predefined callback 
  * @param hsd : SD handle
  * @param CallbackId : Id of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SD_TX_CPLT_CB_ID    SD Tx Complete Callback ID
  *          @arg @ref HAL_SD_RX_CPLT_CB_ID    SD Rx Complete Callback ID
  *          @arg @ref HAL_SD_ERROR_CB_ID      SD Error Callback ID
  *          @arg @ref HAL_SD_ABORT_CB_ID      SD Abort Callback ID
  *          @arg @ref HAL_SD_MSP_INIT_CB_ID   SD MspInit Callback ID 
  *          @arg @ref HAL_SD_MSP_DEINIT_CB_ID SD MspDeInit Callback ID  
  * @retval status
  */
HAL_StatusTypeDef HAL_SD_UnRegisterCallback(SD_HandleTypeDef *hsd, HAL_SD_CallbackIDTypeDef CallbackId)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hsd);
  
  if(hsd->State == HAL_SD_STATE_READY)
  {
    switch (CallbackId)
    {
    case HAL_SD_TX_CPLT_CB_ID :
      hsd->TxCpltCallback = HAL_SD_TxCpltCallback;
      break;
    case HAL_SD_RX_CPLT_CB_ID :
      hsd->RxCpltCallback = HAL_SD_RxCpltCallback;
      break;
    case HAL_SD_ERROR_CB_ID :
      hsd->ErrorCallback = HAL_SD_ErrorCallback;
      break;
    case HAL_SD_ABORT_CB_ID :
      hsd->AbortCpltCallback = HAL_SD_AbortCallback;
      break;
    case HAL_SD_MSP_INIT_CB_ID :
      hsd->MspInitCallback = HAL_SD_MspInit;
      break;
    case HAL_SD_MSP_DEINIT_CB_ID :
      hsd->MspDeInitCallback = HAL_SD_MspDeInit;
      break;
    default :
      /* Update the error code */
      hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (hsd->State == HAL_SD_STATE_RESET)
  {
    switch (CallbackId)
    {
    case HAL_SD_MSP_INIT_CB_ID :
      hsd->MspInitCallback = HAL_SD_MspInit;
      break;
    case HAL_SD_MSP_DEINIT_CB_ID :
      hsd->MspDeInitCallback = HAL_SD_MspDeInit;
      break;
    default :
      /* Update the error code */
      hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
    /* Update the error code */
    hsd->ErrorCode |= HAL_SD_ERROR_INVALID_CALLBACK; 
    /* update return status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hsd);
  return status;
}
#endif

/**
  * @}
  */

/** @addtogroup SD_Exported_Functions_Group3
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control the SD card 
    operations and get the related information

@endverbatim
  * @{
  */

/**
  * @brief  Returns information the information of the card which are stored on
  *         the CID register.
  * @param  hsd Pointer to SD handle
  * @param  pCID Pointer to a HAL_SD_CIDTypeDef structure that  
  *         contains all CID register parameters 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_GetCardCID(SD_HandleTypeDef *hsd, HAL_SD_CardCIDTypeDef *pCID)
{
  uint32_t tmp = 0U;
  
  /* Byte 0 */
  tmp = (uint8_t)((hsd->CID[0U] & 0xFF000000U) >> 24U);
  pCID->ManufacturerID = tmp;
  
  /* Byte 1 */
  tmp = (uint8_t)((hsd->CID[0U] & 0x00FF0000U) >> 16U);
  pCID->OEM_AppliID = tmp << 8U;
  
  /* Byte 2 */
  tmp = (uint8_t)((hsd->CID[0U] & 0x000000FF00U) >> 8U);
  pCID->OEM_AppliID |= tmp;
  
  /* Byte 3 */
  tmp = (uint8_t)(hsd->CID[0U] & 0x000000FFU);
  pCID->ProdName1 = tmp << 24U;
  
  /* Byte 4 */
  tmp = (uint8_t)((hsd->CID[1U] & 0xFF000000U) >> 24U);
  pCID->ProdName1 |= tmp << 16;
  
  /* Byte 5 */
  tmp = (uint8_t)((hsd->CID[1U] & 0x00FF0000U) >> 16U);
  pCID->ProdName1 |= tmp << 8U;
  
  /* Byte 6 */
  tmp = (uint8_t)((hsd->CID[1U] & 0x0000FF00U) >> 8U);
  pCID->ProdName1 |= tmp;
  
  /* Byte 7 */
  tmp = (uint8_t)(hsd->CID[1U] & 0x000000FFU);
  pCID->ProdName2 = tmp;
  
  /* Byte 8 */
  tmp = (uint8_t)((hsd->CID[2U] & 0xFF000000U) >> 24U);
  pCID->ProdRev = tmp;
  
  /* Byte 9 */
  tmp = (uint8_t)((hsd->CID[2U] & 0x00FF0000U) >> 16U);
  pCID->ProdSN = tmp << 24U;
  
  /* Byte 10 */
  tmp = (uint8_t)((hsd->CID[2U] & 0x0000FF00U) >> 8U);
  pCID->ProdSN |= tmp << 16U;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CID[2U] & 0x000000FFU);
  pCID->ProdSN |= tmp << 8U;
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CID[3U] & 0xFF000000U) >> 24U);
  pCID->ProdSN |= tmp;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CID[3U] & 0x00FF0000U) >> 16U);
  pCID->Reserved1   |= (tmp & 0xF0U) >> 4U;
  pCID->ManufactDate = (tmp & 0x0FU) << 8U;
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CID[3U] & 0x0000FF00U) >> 8U);
  pCID->ManufactDate |= tmp;
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CID[3U] & 0x000000FFU);
  pCID->CID_CRC   = (tmp & 0xFEU) >> 1U;
  pCID->Reserved2 = 1U;

  return HAL_OK;
}

/**
  * @brief  Returns information the information of the card which are stored on
  *         the CSD register.
  * @param  hsd Pointer to SD handle
  * @param  pCSD Pointer to a HAL_SD_CardCSDTypeDef structure that  
  *         contains all CSD register parameters  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_GetCardCSD(SD_HandleTypeDef *hsd, HAL_SD_CardCSDTypeDef *pCSD)
{
  uint32_t tmp = 0U;
  
  /* Byte 0 */
  tmp = (hsd->CSD[0U] & 0xFF000000U) >> 24U;
  pCSD->CSDStruct      = (uint8_t)((tmp & 0xC0U) >> 6U);
  pCSD->SysSpecVersion = (uint8_t)((tmp & 0x3CU) >> 2U);
  pCSD->Reserved1      = tmp & 0x03U;
  
  /* Byte 1 */
  tmp = (hsd->CSD[0U] & 0x00FF0000U) >> 16U;
  pCSD->TAAC = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (hsd->CSD[0U] & 0x0000FF00U) >> 8U;
  pCSD->NSAC = (uint8_t)tmp;
  
  /* Byte 3 */
  tmp = hsd->CSD[0U] & 0x000000FFU;
  pCSD->MaxBusClkFrec = (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (hsd->CSD[1U] & 0xFF000000U) >> 24U;
  pCSD->CardComdClasses = (uint16_t)(tmp << 4U);
  
  /* Byte 5 */
  tmp = (hsd->CSD[1U] & 0x00FF0000U) >> 16U;
  pCSD->CardComdClasses |= (uint16_t)((tmp & 0xF0U) >> 4U);
  pCSD->RdBlockLen       = (uint8_t)(tmp & 0x0FU);
  
  /* Byte 6 */
  tmp = (hsd->CSD[1U] & 0x0000FF00U) >> 8U;
  pCSD->PartBlockRead   = (uint8_t)((tmp & 0x80U) >> 7U);
  pCSD->WrBlockMisalign = (uint8_t)((tmp & 0x40U) >> 6U);
  pCSD->RdBlockMisalign = (uint8_t)((tmp & 0x20U) >> 5U);
  pCSD->DSRImpl         = (uint8_t)((tmp & 0x10U) >> 4U);
  pCSD->Reserved2       = 0U; /*!< Reserved */
       
  if(hsd->SdCard.CardType == CARD_SDSC)
  {
    pCSD->DeviceSize = (tmp & 0x03U) << 10U;
    
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1U] & 0x000000FFU);
    pCSD->DeviceSize |= (tmp) << 2U;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0xFF000000U) >> 24U);
    pCSD->DeviceSize |= (tmp & 0xC0U) >> 6U;
    
    pCSD->MaxRdCurrentVDDMin = (tmp & 0x38U) >> 3U;
    pCSD->MaxRdCurrentVDDMax = (tmp & 0x07U);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x00FF0000U) >> 16U);
    pCSD->MaxWrCurrentVDDMin = (tmp & 0xE0U) >> 5U;
    pCSD->MaxWrCurrentVDDMax = (tmp & 0x1CU) >> 2U;
    pCSD->DeviceSizeMul      = (tmp & 0x03U) << 1U;
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x0000FF00U) >> 8U);
    pCSD->DeviceSizeMul |= (tmp & 0x80U) >> 7U;
    
    hsd->SdCard.BlockNbr  = (pCSD->DeviceSize + 1U) ;
    hsd->SdCard.BlockNbr *= (1U << (pCSD->DeviceSizeMul + 2U));
    hsd->SdCard.BlockSize = 1U << (pCSD->RdBlockLen);

    hsd->SdCard.LogBlockNbr =  (hsd->SdCard.BlockNbr) * ((hsd->SdCard.BlockSize) / 512U); 
    hsd->SdCard.LogBlockSize = 512U;
  }
  else if(hsd->SdCard.CardType == CARD_SDHC_SDXC)
  {
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1U] & 0x000000FFU);
    pCSD->DeviceSize = (tmp & 0x3FU) << 16U;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0xFF000000U) >> 24U);
    
    pCSD->DeviceSize |= (tmp << 8U);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x00FF0000U) >> 16U);
    
    pCSD->DeviceSize |= (tmp);
    
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x0000FF00U) >> 8U);
    
    hsd->SdCard.LogBlockNbr = hsd->SdCard.BlockNbr = (((uint64_t)pCSD->DeviceSize + 1U) * 1024U);
    hsd->SdCard.LogBlockSize = hsd->SdCard.BlockSize = 512U;
  }
  else
  {
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);   
    hsd->ErrorCode |= HAL_SD_ERROR_UNSUPPORTED_FEATURE;
    hsd->State = HAL_SD_STATE_READY;
    return HAL_ERROR;
  }
  
  pCSD->EraseGrSize = (tmp & 0x40U) >> 6U;
  pCSD->EraseGrMul  = (tmp & 0x3FU) << 1U;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CSD[2U] & 0x000000FFU);
  pCSD->EraseGrMul     |= (tmp & 0x80U) >> 7U;
  pCSD->WrProtectGrSize = (tmp & 0x7FU);
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0xFF000000U) >> 24U);
  pCSD->WrProtectGrEnable = (tmp & 0x80U) >> 7U;
  pCSD->ManDeflECC        = (tmp & 0x60U) >> 5U;
  pCSD->WrSpeedFact       = (tmp & 0x1CU) >> 2U;
  pCSD->MaxWrBlockLen     = (tmp & 0x03U) << 2U;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0x00FF0000U) >> 16U);
  pCSD->MaxWrBlockLen      |= (tmp & 0xC0U) >> 6U;
  pCSD->WriteBlockPaPartial = (tmp & 0x20U) >> 5U;
  pCSD->Reserved3           = 0U;
  pCSD->ContentProtectAppli = (tmp & 0x01U);
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0x0000FF00U) >> 8U);
  pCSD->FileFormatGrouop = (tmp & 0x80U) >> 7U;
  pCSD->CopyFlag         = (tmp & 0x40U) >> 6U;
  pCSD->PermWrProtect    = (tmp & 0x20U) >> 5U;
  pCSD->TempWrProtect    = (tmp & 0x10U) >> 4U;
  pCSD->FileFormat       = (tmp & 0x0CU) >> 2U;
  pCSD->ECC              = (tmp & 0x03U);
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CSD[3U] & 0x000000FFU);
  pCSD->CSD_CRC   = (tmp & 0xFEU) >> 1U;
  pCSD->Reserved4 = 1U;
  
  return HAL_OK;
}

/**
  * @brief  Gets the SD status info.
  * @param  hsd Pointer to SD handle      
  * @param  pStatus Pointer to the HAL_SD_CardStatusTypeDef structure that 
  *         will contain the SD card status information 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_GetCardStatus(SD_HandleTypeDef *hsd, HAL_SD_CardStatusTypeDef *pStatus)
{
  uint32_t tmp = 0U;
  uint32_t sd_status[16U];
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  errorstate = SD_SendSDStatus(hsd, sd_status);
  if(errorstate != HAL_OK)
  {
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);   
    hsd->ErrorCode |= errorstate;
    hsd->State = HAL_SD_STATE_READY;
    return HAL_ERROR;
  }
  else
  {
    /* Byte 0 */
    tmp = (sd_status[0U] & 0xC0U) >> 6U;
    pStatus->DataBusWidth = (uint8_t)tmp;
    
    /* Byte 0 */
    tmp = (sd_status[0U] & 0x20U) >> 5U;
    pStatus->SecuredMode = (uint8_t)tmp;
    
    /* Byte 2 */
    tmp = (sd_status[0U] & 0x00FF0000U) >> 16U;
    pStatus->CardType = (uint16_t)(tmp << 8U);
    
    /* Byte 3 */
    tmp = (sd_status[0U] & 0xFF000000U) >> 24U;
    pStatus->CardType |= (uint16_t)tmp;
    
    /* Byte 4 */
    tmp = (sd_status[1U] & 0xFFU);
    pStatus->ProtectedAreaSize = (uint32_t)(tmp << 24U);
    
    /* Byte 5 */
    tmp = (sd_status[1U] & 0xFF00U) >> 8U;
    pStatus->ProtectedAreaSize |= (uint32_t)(tmp << 16U);
    
    /* Byte 6 */
    tmp = (sd_status[1U] & 0xFF0000U) >> 16U;
    pStatus->ProtectedAreaSize |= (uint32_t)(tmp << 8U);
    
    /* Byte 7 */
    tmp = (sd_status[1U] & 0xFF000000U) >> 24U;
    pStatus->ProtectedAreaSize |= (uint32_t)tmp;
    
    /* Byte 8 */
    tmp = (sd_status[2U] & 0xFFU);
    pStatus->SpeedClass = (uint8_t)tmp;
    
    /* Byte 9 */
    tmp = (sd_status[2U] & 0xFF00U) >> 8U;
    pStatus->PerformanceMove = (uint8_t)tmp;
    
    /* Byte 10 */
    tmp = (sd_status[2U] & 0xF00000U) >> 20U;
    pStatus->AllocationUnitSize = (uint8_t)tmp;
    
    /* Byte 11 */
    tmp = (sd_status[2U] & 0xFF000000U) >> 24U;
    pStatus->EraseSize = (uint16_t)(tmp << 8U);
    
    /* Byte 12 */
    tmp = (sd_status[3U] & 0xFFU);
    pStatus->EraseSize |= (uint16_t)tmp;
    
    /* Byte 13 */
    tmp = (sd_status[3U] & 0xFC00U) >> 10U;
    pStatus->EraseTimeout = (uint8_t)tmp;
    
    /* Byte 13 */
    tmp = (sd_status[3U] & 0x0300U) >> 8U;
    pStatus->EraseOffset = (uint8_t)tmp;
  }
  
  return HAL_OK;
}

/**
  * @brief  Gets the SD card info.
  * @param  hsd Pointer to SD handle      
  * @param  pCardInfo Pointer to the HAL_SD_CardInfoTypeDef structure that 
  *         will contain the SD card status information 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_GetCardInfo(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypeDef *pCardInfo)
{
  pCardInfo->CardType     = (uint32_t)(hsd->SdCard.CardType);
  pCardInfo->CardVersion  = (uint32_t)(hsd->SdCard.CardVersion);
  pCardInfo->Class        = (uint32_t)(hsd->SdCard.Class);
  pCardInfo->RelCardAdd   = (uint32_t)(hsd->SdCard.RelCardAdd);
  pCardInfo->BlockNbr     = (uint32_t)(hsd->SdCard.BlockNbr);
  pCardInfo->BlockSize    = (uint32_t)(hsd->SdCard.BlockSize);
  pCardInfo->LogBlockNbr  = (uint32_t)(hsd->SdCard.LogBlockNbr);
  pCardInfo->LogBlockSize = (uint32_t)(hsd->SdCard.LogBlockSize);
  
  return HAL_OK;
}

/**
  * @brief  Enables wide bus operation for the requested card if supported by 
  *         card.
  * @param  hsd Pointer to SD handle       
  * @param  WideMode Specifies the SD card wide bus mode 
  *          This parameter can be one of the following values:
  *            @arg SDIO_BUS_WIDE_8B: 8-bit data transfer
  *            @arg SDIO_BUS_WIDE_4B: 4-bit data transfer
  *            @arg SDIO_BUS_WIDE_1B: 1-bit data transfer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_ConfigWideBusOperation(SD_HandleTypeDef *hsd, uint32_t WideMode)
{
  SDIO_InitTypeDef Init;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  /* Check the parameters */
  assert_param(IS_SDIO_BUS_WIDE(WideMode));
  
  /* Chnage Satte */
  hsd->State = HAL_SD_STATE_BUSY;
  
  if(hsd->SdCard.CardType != CARD_SECURED) 
  {
    if(WideMode == SDIO_BUS_WIDE_8B)
    {
      hsd->ErrorCode |= HAL_SD_ERROR_UNSUPPORTED_FEATURE;
    }
    else if(WideMode == SDIO_BUS_WIDE_4B)
    {
      errorstate = SD_WideBus_Enable(hsd);
      
      hsd->ErrorCode |= errorstate;
    }
    else if(WideMode == SDIO_BUS_WIDE_1B)
    {
      errorstate = SD_WideBus_Disable(hsd);
      
      hsd->ErrorCode |= errorstate;
    }
    else
    {
      /* WideMode is not a valid argument*/
      hsd->ErrorCode |= HAL_SD_ERROR_PARAM;
    }
  }  
  else
  {
    /* MMC Card does not support this feature */
    hsd->ErrorCode |= HAL_SD_ERROR_UNSUPPORTED_FEATURE;
  }
  
  if(hsd->ErrorCode != HAL_SD_ERROR_NONE)
  {
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    hsd->State = HAL_SD_STATE_READY;
    return HAL_ERROR;
  }
  else
  {
    /* Configure the SDIO peripheral */
    Init.ClockEdge           = hsd->Init.ClockEdge;
    Init.ClockBypass         = hsd->Init.ClockBypass;
    Init.ClockPowerSave      = hsd->Init.ClockPowerSave;
    Init.BusWide             = WideMode;
    Init.HardwareFlowControl = hsd->Init.HardwareFlowControl;
    Init.ClockDiv            = hsd->Init.ClockDiv;
    SDIO_Init(hsd->Instance, Init);
  }

  /* Change State */
  hsd->State = HAL_SD_STATE_READY;
  
  return HAL_OK;
}


/**
  * @brief  Gets the current sd card data state.
  * @param  hsd pointer to SD handle
  * @retval Card state
  */
HAL_SD_CardStateTypeDef HAL_SD_GetCardState(SD_HandleTypeDef *hsd)
{
  HAL_SD_CardStateTypeDef cardstate =  HAL_SD_CARD_TRANSFER;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint32_t resp1 = 0;
  
  errorstate = SD_SendStatus(hsd, &resp1);
  if(errorstate != HAL_OK)
  {
    hsd->ErrorCode |= errorstate;
  }

  cardstate = (HAL_SD_CardStateTypeDef)((resp1 >> 9U) & 0x0FU);
  
  return cardstate;
}

/**
  * @brief  Abort the current transfer and disable the SD.
  * @param  hsd pointer to a SD_HandleTypeDef structure that contains
  *                the configuration information for SD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_Abort(SD_HandleTypeDef *hsd)
{
  HAL_SD_CardStateTypeDef CardState;
  
  /* DIsable All interrupts */
  __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                           SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR);
  
  /* Clear All flags */
  __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  if((hsd->hdmatx != NULL) || (hsd->hdmarx != NULL))
  {
    /* Disable the SD DMA request */
    hsd->Instance->DCTRL &= (uint32_t)~((uint32_t)SDIO_DCTRL_DMAEN);
    
    /* Abort the SD DMA Tx Stream */
    if(hsd->hdmatx != NULL)
    {
      HAL_DMA_Abort(hsd->hdmatx);
    }
    /* Abort the SD DMA Rx Stream */
    if(hsd->hdmarx != NULL)
    {
      HAL_DMA_Abort(hsd->hdmarx);
    }
  }
  
  hsd->State = HAL_SD_STATE_READY;
  CardState = HAL_SD_GetCardState(hsd);
  if((CardState == HAL_SD_CARD_RECEIVING) || (CardState == HAL_SD_CARD_SENDING))
  {
    hsd->ErrorCode = SDMMC_CmdStopTransfer(hsd->Instance);
  }
  if(hsd->ErrorCode != HAL_SD_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  Abort the current transfer and disable the SD (IT mode).
  * @param  hsd pointer to a SD_HandleTypeDef structure that contains
  *                the configuration information for SD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_Abort_IT(SD_HandleTypeDef *hsd)
{
  HAL_SD_CardStateTypeDef CardState;
    
  /* DIsable All interrupts */
  __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
                           SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR);
  
  /* Clear All flags */
  __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  if((hsd->hdmatx != NULL) || (hsd->hdmarx != NULL))
  {
    /* Disable the SD DMA request */
    hsd->Instance->DCTRL &= (uint32_t)~((uint32_t)SDIO_DCTRL_DMAEN);
    
    /* Abort the SD DMA Tx Stream */
    if(hsd->hdmatx != NULL)
    {
      hsd->hdmatx->XferAbortCallback =  SD_DMATxAbort;
      if(HAL_DMA_Abort_IT(hsd->hdmatx) != HAL_OK)
      {
        hsd->hdmatx = NULL;
      }
    }
    /* Abort the SD DMA Rx Stream */
    if(hsd->hdmarx != NULL)
    {
      hsd->hdmarx->XferAbortCallback =  SD_DMARxAbort;
      if(HAL_DMA_Abort_IT(hsd->hdmarx) != HAL_OK)
      {
        hsd->hdmarx = NULL;
      }
    }
  }
  
  /* No transfer ongoing on both DMA channels*/
  if((hsd->hdmatx == NULL) && (hsd->hdmarx == NULL))
  {
    CardState = HAL_SD_GetCardState(hsd);
    hsd->State = HAL_SD_STATE_READY;
    if((CardState == HAL_SD_CARD_RECEIVING) || (CardState == HAL_SD_CARD_SENDING))
    {
      hsd->ErrorCode = SDMMC_CmdStopTransfer(hsd->Instance);
    }
    if(hsd->ErrorCode != HAL_SD_ERROR_NONE)
    {
      return HAL_ERROR;
    }
    else
    {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
      hsd->AbortCpltCallback(hsd);
#else
      HAL_SD_AbortCallback(hsd);
#endif
    }
  }
  
  return HAL_OK;
}

/**
  * @}
  */
  
/**
  * @}
  */
  
/* Private function ----------------------------------------------------------*/  
/** @addtogroup SD_Private_Functions
  * @{
  */

/**
  * @brief  DMA SD transmit process complete callback 
  * @param  hdma DMA handle
  * @retval None
  */
static void SD_DMATransmitCplt(DMA_HandleTypeDef *hdma)     
{
  SD_HandleTypeDef* hsd = (SD_HandleTypeDef* )(hdma->Parent);
  
  /* Enable DATAEND Interrupt */
  __HAL_SD_ENABLE_IT(hsd, (SDIO_IT_DATAEND));
}

/**
  * @brief  DMA SD receive process complete callback 
  * @param  hdma DMA handle
  * @retval None
  */
static void SD_DMAReceiveCplt(DMA_HandleTypeDef *hdma)  
{
  SD_HandleTypeDef* hsd = (SD_HandleTypeDef* )(hdma->Parent);
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  /* Send stop command in multiblock write */
  if(hsd->Context == (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_DMA))
  {
    errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      hsd->ErrorCode |= errorstate;
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
      hsd->ErrorCallback(hsd);
#else
      HAL_SD_ErrorCallback(hsd);
#endif
    }
  }
  
  /* Disable the DMA transfer for transmit request by setting the DMAEN bit
  in the SD DCTRL register */
  hsd->Instance->DCTRL &= (uint32_t)~((uint32_t)SDIO_DCTRL_DMAEN);
  
  /* Clear all the static flags */
  __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  hsd->State = HAL_SD_STATE_READY;

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
  hsd->RxCpltCallback(hsd);
#else
  HAL_SD_RxCpltCallback(hsd);
#endif
}

/**
  * @brief  DMA SD communication error callback 
  * @param  hdma DMA handle
  * @retval None
  */
static void SD_DMAError(DMA_HandleTypeDef *hdma)   
{
  SD_HandleTypeDef* hsd = (SD_HandleTypeDef* )(hdma->Parent);
  HAL_SD_CardStateTypeDef CardState;
  
  if((hsd->hdmarx->ErrorCode == HAL_DMA_ERROR_TE) || (hsd->hdmatx->ErrorCode == HAL_DMA_ERROR_TE))
  {
    /* Clear All flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    /* Disable All interrupts */
    __HAL_SD_DISABLE_IT(hsd, SDIO_IT_DATAEND | SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT|\
      SDIO_IT_TXUNDERR| SDIO_IT_RXOVERR);
    
    hsd->ErrorCode |= HAL_SD_ERROR_DMA;
    CardState = HAL_SD_GetCardState(hsd);
    if((CardState == HAL_SD_CARD_RECEIVING) || (CardState == HAL_SD_CARD_SENDING))
    {
      hsd->ErrorCode |= SDMMC_CmdStopTransfer(hsd->Instance);
    }
    
    hsd->State= HAL_SD_STATE_READY;
  }

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
    hsd->ErrorCallback(hsd);
#else
  HAL_SD_ErrorCallback(hsd);
#endif
}

/**
  * @brief  DMA SD Tx Abort callback 
  * @param  hdma DMA handle
  * @retval None
  */
static void SD_DMATxAbort(DMA_HandleTypeDef *hdma)   
{
  SD_HandleTypeDef* hsd = (SD_HandleTypeDef* )(hdma->Parent);
  HAL_SD_CardStateTypeDef CardState;
  
  if(hsd->hdmatx != NULL)
  {
    hsd->hdmatx = NULL;
  }
  
  /* All DMA channels are aborted */
  if(hsd->hdmarx == NULL)
  {
    CardState = HAL_SD_GetCardState(hsd);
    hsd->ErrorCode = HAL_SD_ERROR_NONE;
    hsd->State = HAL_SD_STATE_READY;
    if((CardState == HAL_SD_CARD_RECEIVING) || (CardState == HAL_SD_CARD_SENDING))
    {
      hsd->ErrorCode |= SDMMC_CmdStopTransfer(hsd->Instance);
      
      if(hsd->ErrorCode != HAL_SD_ERROR_NONE)
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->AbortCpltCallback(hsd);
#else
        HAL_SD_AbortCallback(hsd);
#endif
      }
      else
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->ErrorCallback(hsd);
#else
        HAL_SD_ErrorCallback(hsd);
#endif
      }
    }
  }
}

/**
  * @brief  DMA SD Rx Abort callback 
  * @param  hdma DMA handle
  * @retval None
  */
static void SD_DMARxAbort(DMA_HandleTypeDef *hdma)   
{
  SD_HandleTypeDef* hsd = (SD_HandleTypeDef* )(hdma->Parent);
  HAL_SD_CardStateTypeDef CardState;
  
  if(hsd->hdmarx != NULL)
  {
    hsd->hdmarx = NULL;
  }
  
  /* All DMA channels are aborted */
  if(hsd->hdmatx == NULL)
  {
    CardState = HAL_SD_GetCardState(hsd);
    hsd->ErrorCode = HAL_SD_ERROR_NONE;
    hsd->State = HAL_SD_STATE_READY;
    if((CardState == HAL_SD_CARD_RECEIVING) || (CardState == HAL_SD_CARD_SENDING))
    {
      hsd->ErrorCode |= SDMMC_CmdStopTransfer(hsd->Instance);
      
      if(hsd->ErrorCode != HAL_SD_ERROR_NONE)
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->AbortCpltCallback(hsd);
#else
        HAL_SD_AbortCallback(hsd);
#endif
      }
      else
      {
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
        hsd->ErrorCallback(hsd);
#else
        HAL_SD_ErrorCallback(hsd);
#endif
      }
    }
  }
}


/**
  * @brief  Initializes the sd card.
  * @param  hsd Pointer to SD handle
  * @retval SD Card error state
  */
static uint32_t SD_InitCard(SD_HandleTypeDef *hsd)
{
  HAL_SD_CardCSDTypeDef CSD;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint16_t sd_rca = 1U;
  
  /* Check the power State */
  if(SDIO_GetPowerState(hsd->Instance) == 0U) 
  {
    /* Power off */
    return HAL_SD_ERROR_REQUEST_NOT_APPLICABLE;
  }
  
  if(hsd->SdCard.CardType != CARD_SECURED) 
  {
    /* Send CMD2 ALL_SEND_CID */
    errorstate = SDMMC_CmdSendCID(hsd->Instance);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      return errorstate;
    }
    else
    {
      /* Get Card identification number data */
      hsd->CID[0U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP1);
      hsd->CID[1U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP2);
      hsd->CID[2U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP3);
      hsd->CID[3U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP4);
    }
  }
  
  if(hsd->SdCard.CardType != CARD_SECURED) 
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 */
    /* SD Card publishes its RCA. */
    errorstate = SDMMC_CmdSetRelAdd(hsd->Instance, &sd_rca);
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      return errorstate;
    }
  }
  if(hsd->SdCard.CardType != CARD_SECURED) 
  {
    /* Get the SD card RCA */
    hsd->SdCard.RelCardAdd = sd_rca;
    
    /* Send CMD9 SEND_CSD with argument as card's RCA */
    errorstate = SDMMC_CmdSendCSD(hsd->Instance, (uint32_t)(hsd->SdCard.RelCardAdd << 16U));
    if(errorstate != HAL_SD_ERROR_NONE)
    {
      return errorstate;
    }
    else
    {
      /* Get Card Specific Data */
      hsd->CSD[0U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP1);
      hsd->CSD[1U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP2);
      hsd->CSD[2U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP3);
      hsd->CSD[3U] = SDIO_GetResponse(hsd->Instance, SDIO_RESP4);
    }
  }
  
  /* Get the Card Class */
  hsd->SdCard.Class = (SDIO_GetResponse(hsd->Instance, SDIO_RESP2) >> 20U);
  
  /* Get CSD parameters */
  HAL_SD_GetCardCSD(hsd, &CSD);

  /* Select the Card */
  errorstate = SDMMC_CmdSelDesel(hsd->Instance, (uint32_t)(((uint32_t)hsd->SdCard.RelCardAdd) << 16U));
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    return errorstate;
  }

  /* Configure SDIO peripheral interface */     
  SDIO_Init(hsd->Instance, hsd->Init);

  /* All cards are initialized */
  return HAL_SD_ERROR_NONE;
}

/**
  * @brief  Enquires cards about their operating voltage and configures clock
  *         controls and stores SD information that will be needed in future
  *         in the SD handle.
  * @param  hsd Pointer to SD handle
  * @retval error state
  */
static uint32_t SD_PowerON(SD_HandleTypeDef *hsd)
{
  __IO uint32_t count = 0U;
  uint32_t response = 0U, validvoltage = 0U;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  /* CMD0: GO_IDLE_STATE */
  errorstate = SDMMC_CmdGoIdleState(hsd->Instance);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    return errorstate;
  }
  
  /* CMD8: SEND_IF_COND: Command available only on V2.0 cards */
  errorstate = SDMMC_CmdOperCond(hsd->Instance);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->SdCard.CardVersion = CARD_V1_X;
      
    /* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while(validvoltage == 0U)
    {
      if(count++ == SDMMC_MAX_VOLT_TRIAL)
      {
        return HAL_SD_ERROR_INVALID_VOLTRANGE;
      }
      
      /* SEND CMD55 APP_CMD with RCA as 0 */
      errorstate = SDMMC_CmdAppCommand(hsd->Instance, 0U);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        return HAL_SD_ERROR_UNSUPPORTED_FEATURE;
      }
      
      /* Send CMD41 */
      errorstate = SDMMC_CmdAppOperCommand(hsd->Instance, SDMMC_STD_CAPACITY);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        return HAL_SD_ERROR_UNSUPPORTED_FEATURE;
      }
      
      /* Get command response */
      response = SDIO_GetResponse(hsd->Instance, SDIO_RESP1);
      
      /* Get operating voltage*/
      validvoltage = (((response >> 31U) == 1U) ? 1U : 0U);
    }
    /* Card type is SDSC */
    hsd->SdCard.CardType = CARD_SDSC;
  }
  else
  {
    hsd->SdCard.CardVersion = CARD_V2_X;
        
    /* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while(validvoltage == 0U)
    {
      if(count++ == SDMMC_MAX_VOLT_TRIAL)
      {
        return HAL_SD_ERROR_INVALID_VOLTRANGE;
      }
      
      /* SEND CMD55 APP_CMD with RCA as 0 */
      errorstate = SDMMC_CmdAppCommand(hsd->Instance, 0U);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        return errorstate;
      }
      
      /* Send CMD41 */
      errorstate = SDMMC_CmdAppOperCommand(hsd->Instance, SDMMC_HIGH_CAPACITY);
      if(errorstate != HAL_SD_ERROR_NONE)
      {
        return errorstate;
      }
      
      /* Get command response */
      response = SDIO_GetResponse(hsd->Instance, SDIO_RESP1);
      
      /* Get operating voltage*/
      validvoltage = (((response >> 31U) == 1U) ? 1U : 0U);
    }
    
    if((response & SDMMC_HIGH_CAPACITY) == SDMMC_HIGH_CAPACITY) /* (response &= SD_HIGH_CAPACITY) */
    {
      hsd->SdCard.CardType = CARD_SDHC_SDXC;
    }
    else
    {
      hsd->SdCard.CardType = CARD_SDSC;
    }
  }
  
  return HAL_SD_ERROR_NONE;
}

/**
  * @brief  Turns the SDIO output signals off.
  * @param  hsd Pointer to SD handle
  * @retval HAL status
  */
static HAL_StatusTypeDef SD_PowerOFF(SD_HandleTypeDef *hsd)
{
  /* Set Power State to OFF */
  SDIO_PowerState_OFF(hsd->Instance);
  
  return HAL_OK;
}

/**
  * @brief  Send Status info command.
  * @param  hsd pointer to SD handle
  * @param  pSDstatus Pointer to the buffer that will contain the SD card status 
  *         SD Status register)
  * @retval error state
  */
static uint32_t SD_SendSDStatus(SD_HandleTypeDef *hsd, uint32_t *pSDstatus)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count = 0U;
  
  /* Check SD response */
  if((SDIO_GetResponse(hsd->Instance, SDIO_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
  {
    return HAL_SD_ERROR_LOCK_UNLOCK_FAILED;
  }
  
  /* Set block size for card if it is not equal to current block size for card */
  errorstate = SDMMC_CmdBlockLength(hsd->Instance, 64U);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_NONE;
    return errorstate;
  }
  
  /* Send CMD55 */
  errorstate = SDMMC_CmdAppCommand(hsd->Instance, (uint32_t)(hsd->SdCard.RelCardAdd << 16U));
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_NONE;
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  config.DataTimeOut   = SDMMC_DATATIMEOUT;
  config.DataLength    = 64U;
  config.DataBlockSize = SDIO_DATABLOCK_SIZE_64B;
  config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  config.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_ConfigData(hsd->Instance, &config);
  
  /* Send ACMD13 (SD_APP_STAUS)  with argument as card's RCA */
  errorstate = SDMMC_CmdStatusRegister(hsd->Instance);
  if(errorstate != HAL_SD_ERROR_NONE)
  {
    hsd->ErrorCode |= HAL_SD_ERROR_NONE;
    return errorstate;
  }
  
  /* Get status data */
  while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))
  {
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
    {
      for(count = 0U; count < 8U; count++)
      {
        *(pSDstatus + count) = SDIO_ReadFIFO(hsd->Instance);
      }
      
      pSDstatus += 8U;
    }
    
    if((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
    {
      return HAL_SD_ERROR_TIMEOUT;
    }
  }
  
  if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    return HAL_SD_ERROR_DATA_TIMEOUT;
  }
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    return HAL_SD_ERROR_DATA_CRC_FAIL;
  }
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    return HAL_SD_ERROR_RX_OVERRUN;
  }

  while ((__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)))
  {
    *pSDstatus = SDIO_ReadFIFO(hsd->Instance);
    pSDstatus++;
    
    if((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
    {
      return HAL_SD_ERROR_TIMEOUT;
    }
  }
  
  /* Clear all the static status flags*/
  __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return HAL_SD_ERROR_NONE;
}

/**
  * @brief  Returns the current card's status.
  * @param  hsd Pointer to SD handle
  * @param  pCardStatus pointer to the buffer that will contain the SD card 
  *         status (Card Status register)  
  * @retval error state
  */
static uint32_t SD_SendStatus(SD_HandleTypeDef *hsd, uint32_t *pCardStatus)
{
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if(pCardStatus == NULL)
  {
    return HAL_SD_ERROR_PARAM;
  }
  
  /* Send Status command */
  errorstate = SDMMC_CmdSendStatus(hsd->Instance, (uint32_t)(hsd->SdCard.RelCardAdd << 16U));
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }
  
  /* Get SD card status */
  *pCardStatus = SDIO_GetResponse(hsd->Instance, SDIO_RESP1);
  
  return HAL_SD_ERROR_NONE;
}

/**
  * @brief  Enables the SDIO wide bus mode.
  * @param  hsd pointer to SD handle
  * @retval error state
  */
static uint32_t SD_WideBus_Enable(SD_HandleTypeDef *hsd)
{
  uint32_t scr[2U] = {0U, 0U};
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if((SDIO_GetResponse(hsd->Instance, SDIO_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
  {
    return HAL_SD_ERROR_LOCK_UNLOCK_FAILED;
  }
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, scr);
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }
  
  /* If requested card supports wide bus operation */
  if((scr[1U] & SDMMC_WIDE_BUS_SUPPORT) != SDMMC_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    errorstate = SDMMC_CmdAppCommand(hsd->Instance, (uint32_t)(hsd->SdCard.RelCardAdd << 16U));
    if(errorstate != HAL_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    errorstate = SDMMC_CmdBusWidth(hsd->Instance, 2U);
    if(errorstate != HAL_OK)
    {
      return errorstate;
    }

    return HAL_SD_ERROR_NONE;
  }
  else
  {
    return HAL_SD_ERROR_REQUEST_NOT_APPLICABLE;
  }
}

/**
  * @brief  Disables the SDIO wide bus mode.
  * @param  hsd Pointer to SD handle
  * @retval error state
  */
static uint32_t SD_WideBus_Disable(SD_HandleTypeDef *hsd)
{
  uint32_t scr[2U] = {0U, 0U};
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  
  if((SDIO_GetResponse(hsd->Instance, SDIO_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
  {
    return HAL_SD_ERROR_LOCK_UNLOCK_FAILED;
  }
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, scr);
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }
  
  /* If requested card supports 1 bit mode operation */
  if((scr[1U] & SDMMC_SINGLE_BUS_SUPPORT) != SDMMC_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA */
    errorstate = SDMMC_CmdAppCommand(hsd->Instance, (uint32_t)(hsd->SdCard.RelCardAdd << 16U));
    if(errorstate != HAL_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 0 for single bus mode */
    errorstate = SDMMC_CmdBusWidth(hsd->Instance, 0U);
    if(errorstate != HAL_OK)
    {
      return errorstate;
    }
    
    return HAL_SD_ERROR_NONE;
  }
  else
  {
    return HAL_SD_ERROR_REQUEST_NOT_APPLICABLE;
  }
}
  
  
/**
  * @brief  Finds the SD card SCR register value.
  * @param  hsd Pointer to SD handle
  * @param  pSCR pointer to the buffer that will contain the SCR value  
  * @retval error state
  */
static uint32_t SD_FindSCR(SD_HandleTypeDef *hsd, uint32_t *pSCR)
{
  SDIO_DataInitTypeDef config;
  uint32_t errorstate = HAL_SD_ERROR_NONE;
  uint32_t tickstart = HAL_GetTick();
  uint32_t index = 0U;
  uint32_t tempscr[2U] = {0U, 0U};
  
  /* Set Block Size To 8 Bytes */
  errorstate = SDMMC_CmdBlockLength(hsd->Instance, 8U);
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }

  /* Send CMD55 APP_CMD with argument as card's RCA */
  errorstate = SDMMC_CmdAppCommand(hsd->Instance, (uint32_t)((hsd->SdCard.RelCardAdd) << 16U));
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }

  config.DataTimeOut   = SDMMC_DATATIMEOUT;
  config.DataLength    = 8U;
  config.DataBlockSize = SDIO_DATABLOCK_SIZE_8B;
  config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  config.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_ConfigData(hsd->Instance, &config);
  
  /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  errorstate = SDMMC_CmdSendSCR(hsd->Instance);
  if(errorstate != HAL_OK)
  {
    return errorstate;
  }
  
  while(!__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))
  {
    if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXDAVL))
    {
      *(tempscr + index) = SDIO_ReadFIFO(hsd->Instance);
      index++;
    }
    
    if((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
    {
      return HAL_SD_ERROR_TIMEOUT;
    }
  }
  
  if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    return HAL_SD_ERROR_DATA_TIMEOUT;
  }
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    return HAL_SD_ERROR_DATA_CRC_FAIL;
  }
  else if(__HAL_SD_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    return HAL_SD_ERROR_RX_OVERRUN;
  }
  else
  {
    /* No error flag set */
    /* Clear all the static flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    *(pSCR + 1U) = ((tempscr[0U] & SDMMC_0TO7BITS) << 24U)  | ((tempscr[0U] & SDMMC_8TO15BITS) << 8U) |\
      ((tempscr[0U] & SDMMC_16TO23BITS) >> 8U) | ((tempscr[0U] & SDMMC_24TO31BITS) >> 24U);
    
    *(pSCR) = ((tempscr[1U] & SDMMC_0TO7BITS) << 24U)  | ((tempscr[1U] & SDMMC_8TO15BITS) << 8U) |\
      ((tempscr[1U] & SDMMC_16TO23BITS) >> 8U) | ((tempscr[1U] & SDMMC_24TO31BITS) >> 24U);
  }

  return HAL_SD_ERROR_NONE;
}

/**
  * @brief  Wrap up reading in non-blocking mode.
  * @param  hsd pointer to a SD_HandleTypeDef structure that contains
  *              the configuration information.
  * @retval HAL status
  */
static HAL_StatusTypeDef SD_Read_IT(SD_HandleTypeDef *hsd)
{
  uint32_t count = 0U;
  uint32_t* tmp;

  tmp = (uint32_t*)hsd->pRxBuffPtr;
  
  /* Read data from SDIO Rx FIFO */
  for(count = 0U; count < 8U; count++)
  {
    *(tmp + count) = SDIO_ReadFIFO(hsd->Instance);
  }
  
  hsd->pRxBuffPtr += 8U;
  
  return HAL_OK;
}

/**
  * @brief  Wrap up writing in non-blocking mode.
  * @param  hsd pointer to a SD_HandleTypeDef structure that contains
  *              the configuration information.
  * @retval HAL status
  */
static HAL_StatusTypeDef SD_Write_IT(SD_HandleTypeDef *hsd)
{
  uint32_t count = 0U;
  uint32_t* tmp;
  
  tmp = (uint32_t*)hsd->pTxBuffPtr;
  
  /* Write data to SDIO Tx FIFO */
  for(count = 0U; count < 8U; count++)
  {
    SDIO_WriteFIFO(hsd->Instance, (tmp + count));
  }
  
  hsd->pTxBuffPtr += 8U;
  
  return HAL_OK;
}

/**
  * @}
  */

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || 
          STM32F412Rx || STM32F412Cx || STM32F413xx || STM32F423xx */

#endif /* HAL_SD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
