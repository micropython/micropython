/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sd.c
  * @author  MCD Application Team
  * @version V1.5.1
  * @date    01-July-2016
  * @brief   SD card HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Secure Digital (SD) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
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
            (+++) SDIO interrupts are managed using the macros __HAL_SD_SDIO_ENABLE_IT() 
                  and __HAL_SD_SDIO_DISABLE_IT() inside the communication process.
            (+++) SDIO interrupts pending bits are managed using the macros __HAL_SD_SDIO_GET_IT()
                  and __HAL_SD_SDIO_CLEAR_IT()
    (#) At this stage, you can perform SD read/write/erase operations after SD card initialization  

         
  *** SD Card Initialization and configuration ***
  ================================================    
  [..]
    To initialize the SD Card, use the HAL_SD_Init() function.  It Initializes 
    the SD Card and put it into Standby State (Ready for data transfer). 
    This function provide the following operations:
  
    (#) Apply the SD Card initialization process at 400KHz and check the SD Card 
        type (Standard Capacity or High Capacity). You can change or adapt this 
        frequency by adjusting the "ClockDiv" field. 
        The SD Card frequency (SDIO_CK) is computed as follows:
  
           SDIO_CK = SDIOCLK / (ClockDiv + 2)
  
        In initialization mode and according to the SD Card standard, 
        make sure that the SDIO_CK frequency doesn't exceed 400KHz.
  
    (#) Get the SD CID and CSD data. All these information are managed by the SDCardInfo 
        structure. This structure provide also ready computed SD Card capacity 
        and Block size.
        
        -@- These information are stored in SD handle structure in case of future use.  
  
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
        This function support only 512-bytes block length (the block size should be 
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.

    (+) You can read from SD card in DMA mode by using function HAL_SD_ReadBlocks_DMA().
        This function support only 512-bytes block length (the block size should be 
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to call the function HAL_SD_CheckReadOperation(), to insure
        that the read transfer is done correctly in both DMA and SD sides.
  
  *** SD Card Write operation ***
  =============================== 
  [..] 
    (+) You can write to SD card in polling mode by using function HAL_SD_WriteBlocks(). 
        This function support only 512-bytes block length (the block size should be 
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.

    (+) You can write to SD card in DMA mode by using function HAL_SD_WriteBlocks_DMA().
        This function support only 512-bytes block length (the block size should be 
        chosen as 512 byte).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to call the function HAL_SD_CheckWriteOperation(), to insure
        that the write transfer is done correctly in both DMA and SD sides.  
  
  *** SD card status ***
  ====================== 
  [..]
    (+) At any time, you can check the SD Card status and get the SD card state 
        by using the HAL_SD_GetStatus() function. This function checks first if the 
        SD card is still connected and then get the internal SD Card transfer state.     
    (+) You can also get the SD card SD Status register by using the HAL_SD_SendSDStatus() 
        function.    

  *** SD HAL driver macros list ***
  ==================================
  [..]
    Below the list of most used macros in SD HAL driver.

    (+) __HAL_SD_SDIO_ENABLE : Enable the SD device
    (+) __HAL_SD_SDIO_DISABLE : Disable the SD device
    (+) __HAL_SD_SDIO_DMA_ENABLE: Enable the SDIO DMA transfer
    (+) __HAL_SD_SDIO_DMA_DISABLE: Disable the SDIO DMA transfer
    (+) __HAL_SD_SDIO_ENABLE_IT: Enable the SD device interrupt
    (+) __HAL_SD_SDIO_DISABLE_IT: Disable the SD device interrupt
    (+) __HAL_SD_SDIO_GET_FLAG:Check whether the specified SD flag is set or not
    (+) __HAL_SD_SDIO_CLEAR_FLAG: Clear the SD's pending flags
      
    (@) You can refer to the SD HAL driver header file for more useful macros 
      
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

#ifdef HAL_SD_MODULE_ENABLED
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx) || \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)
/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup SD 
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup SD_Private_Defines
  * @{
  */
/** 
  * @brief  SDIO Data block size 
  */ 
#define DATA_BLOCK_SIZE                  ((uint32_t)(9U << 4U))
/** 
  * @brief  SDIO Static flags, Timeout, FIFO Address  
  */
#define SDIO_STATIC_FLAGS               ((uint32_t)(SDIO_FLAG_CCRCFAIL | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_CTIMEOUT |\
                                                    SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR | SDIO_FLAG_RXOVERR  |\
                                                    SDIO_FLAG_CMDREND  | SDIO_FLAG_CMDSENT  | SDIO_FLAG_DATAEND  |\
                                                    SDIO_FLAG_DBCKEND))  

#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000U)

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000U)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000U)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000U)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000U)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000U)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000U)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000U)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000U)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000U)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000U)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000U)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000U)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000U)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000U)
#define SD_OCR_CID_CSD_OVERWRITE        ((uint32_t)0x00010000U)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000U)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000U)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000U)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008U)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008U)

/** 
  * @brief  Masks for R6 Response 
  */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000U)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000U)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000U)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000U)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000U)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000U)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AAU)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFFU)
#define SD_ALLZERO                      ((uint32_t)0x00000000U)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000U)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000U)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000U)

#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFFU)
#define SD_0TO7BITS                     ((uint32_t)0x000000FFU)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00U)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000U)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000U)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFFU)

#define SD_HALFFIFO                     ((uint32_t)0x00000008U)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020U)

/** 
  * @brief  Command Class Supported 
  */
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080U)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040U)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020U)

/** 
  * @brief  Following commands are SD Card Specific commands.
  *         SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_SDIO_SEND_IF_COND            ((uint32_t)SD_CMD_HS_SEND_EXT_CSD)

/**
  * @}
  */
  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SD_Private_Functions_Prototypes
  * @{
  */
static HAL_SD_ErrorTypedef SD_Initialize_Cards(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_Select_Deselect(SD_HandleTypeDef *hsd, uint64_t addr);
static HAL_SD_ErrorTypedef SD_PowerON(SD_HandleTypeDef *hsd); 
static HAL_SD_ErrorTypedef SD_PowerOFF(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_SendStatus(SD_HandleTypeDef *hsd, uint32_t *pCardStatus);
static HAL_SD_CardStateTypedef SD_GetState(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_IsCardProgramming(SD_HandleTypeDef *hsd, uint8_t *pStatus);
static HAL_SD_ErrorTypedef SD_CmdError(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_CmdResp1Error(SD_HandleTypeDef *hsd, uint8_t SD_CMD);
static HAL_SD_ErrorTypedef SD_CmdResp7Error(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_CmdResp3Error(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_CmdResp2Error(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_CmdResp6Error(SD_HandleTypeDef *hsd, uint8_t SD_CMD, uint16_t *pRCA);
static HAL_SD_ErrorTypedef SD_WideBus_Enable(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_WideBus_Disable(SD_HandleTypeDef *hsd);
static HAL_SD_ErrorTypedef SD_FindSCR(SD_HandleTypeDef *hsd, uint32_t *pSCR);  
static void SD_DMA_RxCplt(DMA_HandleTypeDef *hdma);
static void SD_DMA_RxError(DMA_HandleTypeDef *hdma);
static void SD_DMA_TxCplt(DMA_HandleTypeDef *hdma);
static void SD_DMA_TxError(DMA_HandleTypeDef *hdma);
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
  * @brief  Initializes the SD card according to the specified parameters in the 
            SD_HandleTypeDef and create the associated handle.
  * @param  hsd: SD handle
  * @param  SDCardInfo: HAL_SD_CardInfoTypedef structure for SD card information   
  * @retval HAL SD error state
  */
HAL_SD_ErrorTypedef HAL_SD_Init(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypedef *SDCardInfo)
{ 
  __IO HAL_SD_ErrorTypedef errorstate = SD_OK;
  SD_InitTypeDef tmpinit;
  
  /* Allocate lock resource and initialize it */
  hsd->Lock = HAL_UNLOCKED;
  /* Initialize the low level hardware (MSP) */
  HAL_SD_MspInit(hsd);
  
  /* Default SDIO peripheral configuration for SD card initialization */
  tmpinit.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
  tmpinit.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
  tmpinit.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
  tmpinit.BusWide             = SDIO_BUS_WIDE_1B;
  tmpinit.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  tmpinit.ClockDiv            = SDIO_INIT_CLK_DIV;
  
  /* Initialize SDIO peripheral interface with default configuration */
  SDIO_Init(hsd->Instance, tmpinit);
  
  /* Identify card operating voltage */
  errorstate = SD_PowerON(hsd); 
  
  if(errorstate != SD_OK)     
  {
    return errorstate;
  }
  
  /* Initialize the present SDIO card(s) and put them in idle state */
  errorstate = SD_Initialize_Cards(hsd);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Read CSD/CID MSD registers */
  errorstate = HAL_SD_Get_CardInfo(hsd, SDCardInfo);
  
  if (errorstate == SD_OK)
  {
    /* Select the Card */
    errorstate = SD_Select_Deselect(hsd, (uint32_t)(((uint32_t)SDCardInfo->RCA) << 16U));
  }
  
  /* Configure SDIO peripheral interface */
  SDIO_Init(hsd->Instance, hsd->Init);   
  
  return errorstate;
}

/**
  * @brief  De-Initializes the SD card.
  * @param  hsd: SD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SD_DeInit(SD_HandleTypeDef *hsd)
{
  
  /* Set SD power state to off */ 
  SD_PowerOFF(hsd);
  
  /* De-Initialize the MSP layer */
  HAL_SD_MspDeInit(hsd);
  
  return HAL_OK;
}


/**
  * @brief  Initializes the SD MSP.
  * @param  hsd: SD handle
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
  * @param  hsd: SD handle
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
  * @param  hsd: SD handle
  * @param  pReadBuffer: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of SD blocks to read   
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef  sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t count = 0U, *tempbuff = (uint32_t *)pReadBuffer;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0U;
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512U;
    ReadAddr /= 512U;
  }
  
  /* Set Block Size for Card */ 
  sdio_cmdinitstructure.Argument         = (uint32_t) BlockSize;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = NumberOfBlocks * BlockSize;
  sdio_datainitstructure.DataBlockSize = DATA_BLOCK_SIZE;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  if(NumberOfBlocks > 1U)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;    
  }
  
  sdio_cmdinitstructure.Argument         = (uint32_t)ReadAddr;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Read block(s) in polling mode */
  if(NumberOfBlocks > 1U)
  {
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_READ_MULT_BLOCK);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Poll on SDIO flags */
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND))
#endif /* SDIO_STA_STBITERR */
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        /* Read data from SDIO Rx FIFO */
        for (count = 0U; count < 8U; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8U;
      }
    }      
  }
  else
  {
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_READ_SINGLE_BLOCK); 
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }    
    
    /* In case of single block transfer, no need of stop transfer at all */
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))      
#endif /* SDIO_STA_STBITERR */
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        /* Read data from SDIO Rx FIFO */
        for (count = 0U; count < 8U; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8U;
      }
    }
  }
  
  /* Send stop transmission command in case of multiblock read */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1U))
  {    
    if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) ||\
      (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
        (hsd->CardType == HIGH_CAPACITY_SD_CARD))
    {
      /* Send stop transmission command */
      errorstate = HAL_SD_StopTransfer(hsd);
    }
  }
  
  /* Get error state */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
#ifdef SDIO_STA_STBITERR
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorstate = SD_START_BIT_ERR;
    
    return errorstate;
  }
#endif /* SDIO_STA_STBITERR */ 
  else
  {
    /* No error flag set */
  }
  
  count = SD_DATATIMEOUT;
  
  /* Empty FIFO if there is still any data */
  while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0U))
  {
    *tempbuff = SDIO_ReadFIFO(hsd->Instance);
    tempbuff++;
    count--;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Allows to write block(s) to a specified address in a card. The Data
  *         transfer is managed by polling mode.  
  * @param  hsd: SD handle
  * @param  pWriteBuffer: pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written 
  * @param  BlockSize: SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of SD blocks to write 
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t totalnumberofbytes = 0U, bytestransferred = 0U, count = 0U, restwords = 0U;
  uint32_t *tempbuff = (uint32_t *)pWriteBuffer;
  uint8_t cardstate  = 0U;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0U;
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512U;
    WriteAddr /= 512U;
  }
  
  /* Set Block Size for Card */ 
  sdio_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  if(NumberOfBlocks > 1U)
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  else
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  
  sdio_cmdinitstructure.Argument         = (uint32_t)WriteAddr;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1U)
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_WRITE_MULT_BLOCK);
  }
  else
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_WRITE_SINGLE_BLOCK);
  }  
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Set total number of bytes to write */
  totalnumberofbytes = NumberOfBlocks * BlockSize;
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = NumberOfBlocks * BlockSize;
  sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  /* Write block(s) in polling mode */
  if(NumberOfBlocks > 1U)
  {
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND))
#endif /* SDIO_STA_STBITERR */     
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXFIFOHE))
      {
        if ((totalnumberofbytes - bytestransferred) < 32U)
        {
          restwords = ((totalnumberofbytes - bytestransferred) % 4U == 0U) ? ((totalnumberofbytes - bytestransferred) / 4U) : (( totalnumberofbytes -  bytestransferred) / 4U + 1U);
          
          /* Write data to SDIO Tx FIFO */
          for (count = 0U; count < restwords; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++;
            bytestransferred += 4U;
          }
        }
        else
        {
          /* Write data to SDIO Tx FIFO */
          for (count = 0U; count < 8U; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, (tempbuff + count));
          }
          
          tempbuff += 8U;
          bytestransferred += 32U;
        }
      }
    }   
  }
  else
  {
    /* In case of single data block transfer no need of stop command at all */
#ifdef SDIO_STA_STBITERR
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))
#endif /* SDIO_STA_STBITERR */
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXFIFOHE))
      {
        if ((totalnumberofbytes - bytestransferred) < 32U)
        {
          restwords = ((totalnumberofbytes - bytestransferred) % 4U == 0U) ? ((totalnumberofbytes - bytestransferred) / 4U) : (( totalnumberofbytes -  bytestransferred) / 4U + 1U);
          
          /* Write data to SDIO Tx FIFO */
          for (count = 0U; count < restwords; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++; 
            bytestransferred += 4U;
          }
        }
        else
        {
          /* Write data to SDIO Tx FIFO */
          for (count = 0U; count < 8U; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, (tempbuff + count));
          }
          
          tempbuff += 8U;
          bytestransferred += 32U;
        }
      }
    }  
  }
  
  /* Send stop transmission command in case of multiblock write */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1U))
  {    
    if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
      (hsd->CardType == HIGH_CAPACITY_SD_CARD))
    {
      /* Send stop transmission command */
      errorstate = HAL_SD_StopTransfer(hsd);
    }
  }
  
  /* Get error state */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_TXUNDERR);
    
    errorstate = SD_TX_UNDERRUN;
    
    return errorstate;
  }
#ifdef SDIO_STA_STBITERR
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorstate = SD_START_BIT_ERR;
    
    return errorstate;
  }
#endif /* SDIO_STA_STBITERR */
  else
  {
    /* No error flag set */
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* Wait till the card is in programming state */
  errorstate = SD_IsCardProgramming(hsd, &cardstate);
  
  while ((errorstate == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstate = SD_IsCardProgramming(hsd, &cardstate);
  }
  
  return errorstate;
}

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer 
  *         is managed by DMA mode. 
  * @note   This API should be followed by the function HAL_SD_CheckReadOperation()
  *         to check the completion of the read process   
  * @param  hsd: SD handle                 
  * @param  pReadBuffer: Pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_DMA(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0U;
  
  /* Initialize handle flags */
  hsd->SdTransferCplt  = 0U;
  hsd->DmaTransferCplt = 0U;
  hsd->SdTransferErr   = SD_OK; 
  
  /* Initialize SD Read operation */
  if(NumberOfBlocks > 1U)
  {
    hsd->SdOperation = SD_READ_MULTIPLE_BLOCK;
  }
  else
  {
    hsd->SdOperation = SD_READ_SINGLE_BLOCK;
  }
  
  /* Enable transfer interrupts */
#ifdef SDIO_STA_STBITERR
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_RXOVERR  |\
                                SDIO_IT_STBITERR));
#else /* SDIO_STA_STBITERR not defined */
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_RXOVERR));
#endif /* SDIO_STA_STBITERR */
  
  /* Enable SDIO DMA transfer */
  __HAL_SD_SDIO_DMA_ENABLE();
  
  /* Configure DMA user callbacks */
  hsd->hdmarx->XferCpltCallback  = SD_DMA_RxCplt;
  hsd->hdmarx->XferErrorCallback = SD_DMA_RxError;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsd->hdmarx, (uint32_t)&hsd->Instance->FIFO, (uint32_t)pReadBuffer, (uint32_t)(BlockSize * NumberOfBlocks)/4);
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512U;
    ReadAddr /= 512U;
  }
  
  /* Set Block Size for Card */ 
  sdio_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = BlockSize * NumberOfBlocks;
  sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  /* Check number of blocks command */
  if(NumberOfBlocks > 1U)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
  }
  
  sdio_cmdinitstructure.Argument = (uint32_t)ReadAddr;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1U)
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_READ_MULT_BLOCK);
  }
  else
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_READ_SINGLE_BLOCK);
  }
  
  /* Update the SD transfer error in SD handle */
  hsd->SdTransferErr = errorstate;
  
  return errorstate;
}


/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer 
  *         is managed by DMA mode. 
  * @note   This API should be followed by the function HAL_SD_CheckWriteOperation()
  *         to check the completion of the write process (by SD current status polling).  
  * @param  hsd: SD handle
  * @param  pWriteBuffer: pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be read   
  * @param  BlockSize: the SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_DMA(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0U;
  
  /* Initialize handle flags */
  hsd->SdTransferCplt  = 0U;
  hsd->DmaTransferCplt = 0U;
  hsd->SdTransferErr   = SD_OK;
  
  /* Initialize SD Write operation */
  if(NumberOfBlocks > 1U)
  {
    hsd->SdOperation = SD_WRITE_MULTIPLE_BLOCK;
  }
  else
  {
    hsd->SdOperation = SD_WRITE_SINGLE_BLOCK;
  }  
  
  /* Enable transfer interrupts */
#ifdef SDIO_STA_STBITERR
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_TXUNDERR |\
                                SDIO_IT_STBITERR));
#else /* SDIO_STA_STBITERR not defined */
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_TXUNDERR));
#endif /* SDIO_STA_STBITERR */
  
  /* Configure DMA user callbacks */
  hsd->hdmatx->XferCpltCallback  = SD_DMA_TxCplt;
  hsd->hdmatx->XferErrorCallback = SD_DMA_TxError;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsd->hdmatx, (uint32_t)pWriteBuffer, (uint32_t)&hsd->Instance->FIFO, (uint32_t)(BlockSize * NumberOfBlocks)/4);

  /* Enable SDIO DMA transfer */
  __HAL_SD_SDIO_DMA_ENABLE();
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512U;
    WriteAddr /= 512U;
  }

  /* Set Block Size for Card */ 
  sdio_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);

  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);

  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Check number of blocks command */
  if(NumberOfBlocks <= 1U)
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  else
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    sdio_cmdinitstructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  
  sdio_cmdinitstructure.Argument         = (uint32_t)WriteAddr;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);

  /* Check for error conditions */
  if(NumberOfBlocks > 1U)
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_WRITE_MULT_BLOCK);
  }
  else
  {
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_WRITE_SINGLE_BLOCK);
  }
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = BlockSize * NumberOfBlocks;
  sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  hsd->SdTransferErr = errorstate;
  
  return errorstate;
}

/**
  * @brief  This function waits until the SD DMA data read transfer is finished. 
  *         This API should be called after HAL_SD_ReadBlocks_DMA() function
  *         to insure that all data sent by the card is already transferred by the 
  *         DMA controller.
  * @param  hsd: SD handle
  * @param  Timeout: Timeout duration  
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_CheckReadOperation(SD_HandleTypeDef *hsd, uint32_t Timeout)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t timeout = Timeout;
  uint32_t tmp1, tmp2;
  HAL_SD_ErrorTypedef tmp3;
  
  /* Wait for DMA/SD transfer end or SD error variables to be in SD handle */
  tmp1 = hsd->DmaTransferCplt; 
  tmp2 = hsd->SdTransferCplt;
  tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    
  while ((tmp1 == 0U) && (tmp2 == 0U) && (tmp3 == SD_OK) && (timeout > 0U))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;    
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Wait until the Rx transfer is no longer active */
  while((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXACT)) && (timeout > 0U))
  {
    timeout--;  
  }
  
  /* Send stop command in multiblock read */
  if (hsd->SdOperation == SD_READ_MULTIPLE_BLOCK)
  {
    errorstate = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0U) && (errorstate == SD_OK))
  {
    errorstate = SD_DATA_TIMEOUT;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* Return error state */
  if (hsd->SdTransferErr != SD_OK)
  {
    return (HAL_SD_ErrorTypedef)(hsd->SdTransferErr);
  }
  
  return errorstate;
}

/**
  * @brief  This function waits until the SD DMA data write transfer is finished. 
  *         This API should be called after HAL_SD_WriteBlocks_DMA() function
  *         to insure that all data sent by the card is already transferred by the 
  *         DMA controller.
  * @param  hsd: SD handle
  * @param  Timeout: Timeout duration  
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_CheckWriteOperation(SD_HandleTypeDef *hsd, uint32_t Timeout)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t timeout = Timeout;
  uint32_t tmp1, tmp2;
  HAL_SD_ErrorTypedef tmp3;

  /* Wait for DMA/SD transfer end or SD error variables to be in SD handle */
  tmp1 = hsd->DmaTransferCplt; 
  tmp2 = hsd->SdTransferCplt;
  tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    
  while ((tmp1 == 0U) && (tmp2 == 0U) && (tmp3 == SD_OK) && (timeout > 0U))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Wait until the Tx transfer is no longer active */
  while((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXACT))  && (timeout > 0U))
  {
    timeout--;  
  }

  /* Send stop command in multiblock write */
  if (hsd->SdOperation == SD_WRITE_MULTIPLE_BLOCK)
  {
    errorstate = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0U) && (errorstate == SD_OK))
  {
    errorstate = SD_DATA_TIMEOUT;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* Return error state */
  if (hsd->SdTransferErr != SD_OK)
  {
    return (HAL_SD_ErrorTypedef)(hsd->SdTransferErr);
  }
  
  /* Wait until write is complete */
  while(HAL_SD_GetStatus(hsd) != SD_TRANSFER_OK)
  {    
  }

  return errorstate; 
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  hsd: SD handle 
  * @param  startaddr: Start byte address
  * @param  endaddr: End byte address
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_Erase(SD_HandleTypeDef *hsd, uint64_t startaddr, uint64_t endaddr)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  
  uint32_t delay         = 0U;
  __IO uint32_t maxdelay = 0U;
  uint8_t cardstate      = 0U;
  
  /* Check if the card command class supports erase command */
  if (((hsd->CSD[1U] >> 20U) & SD_CCCC_ERASE) == 0U)
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
  
  /* Get max delay value */
  maxdelay = 120000U / (((hsd->Instance->CLKCR) & 0xFFU) + 2U);
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
  }
  
  /* Get start and end block for high capacity cards */
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    startaddr /= 512U;
    endaddr   /= 512U;
  }
  
  /* According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
  if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
    (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    /* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
    sdio_cmdinitstructure.Argument         =(uint32_t)startaddr;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_START;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
    sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
    sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_START);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    sdio_cmdinitstructure.Argument         = (uint32_t)endaddr;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_END;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_END);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
  }
  
  /* Send CMD38 ERASE */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_ERASE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_ERASE);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  for (; delay < maxdelay; delay++)
  {
  }
  
  /* Wait until the card is in programming state */
  errorstate = SD_IsCardProgramming(hsd, &cardstate);
  
  delay = SD_DATATIMEOUT;
  
  while ((delay > 0U) && (errorstate == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstate = SD_IsCardProgramming(hsd, &cardstate);
    delay--;
  }
  
  return errorstate;
}

/**
  * @brief  This function handles SD card interrupt request.
  * @param  hsd: SD handle
  * @retval None
  */
void HAL_SD_IRQHandler(SD_HandleTypeDef *hsd)
{  
  /* Check for SDIO interrupt flags */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_DATAEND))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_IT_DATAEND);  
      
    /* SD transfer is complete */
    hsd->SdTransferCplt = 1U;

    /* No transfer error */ 
    hsd->SdTransferErr  = SD_OK;

    HAL_SD_XferCpltCallback(hsd);  
  }  
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    hsd->SdTransferErr = SD_DATA_CRC_FAIL;
    
    HAL_SD_XferErrorCallback(hsd);
    
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    hsd->SdTransferErr = SD_DATA_TIMEOUT;
    
    HAL_SD_XferErrorCallback(hsd);
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    hsd->SdTransferErr = SD_RX_OVERRUN;
    
    HAL_SD_XferErrorCallback(hsd);
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_TXUNDERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_TXUNDERR);
    
    hsd->SdTransferErr = SD_TX_UNDERRUN;
    
    HAL_SD_XferErrorCallback(hsd);
  }
#ifdef SDIO_STA_STBITERR
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    hsd->SdTransferErr = SD_START_BIT_ERR;
    
    HAL_SD_XferErrorCallback(hsd);
  }
#endif /* SDIO_STA_STBITERR */
  else
  {
    /* No error flag set */
  }

  /* Disable all SDIO peripheral interrupt sources */
#ifdef SDIO_STA_STBITERR
  __HAL_SD_SDIO_DISABLE_IT(hsd, SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND  |\
                                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |\
                                SDIO_IT_RXOVERR  | SDIO_IT_STBITERR);
#else /* SDIO_STA_STBITERR not defined */
  __HAL_SD_SDIO_DISABLE_IT(hsd, SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND  |\
                                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |\
                                SDIO_IT_RXOVERR);
#endif /* SDIO_STA_STBITERR */
}


/**
  * @brief  SD end of transfer callback.
  * @param  hsd: SD handle 
  * @retval None
  */
__weak void HAL_SD_XferCpltCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_XferCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer Error callback.
  * @param  hsd: SD handle
  * @retval None
  */
__weak void HAL_SD_XferErrorCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_XferErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Rx callback in non blocking mode.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_RxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_RxCpltCallback could be implemented in the user file
   */ 
}  

/**
  * @brief  SD DMA transfer complete Rx error callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_RxErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_RxErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Tx callback in non blocking mode.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_TxCpltCallback could be implemented in the user file
   */ 
}  

/**
  * @brief  SD DMA transfer complete error Tx callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_TxErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_TxErrorCallback could be implemented in the user file
   */ 
}

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
    operations.

@endverbatim
  * @{
  */

/**
  * @brief  Returns information about specific card.
  * @param  hsd: SD handle
  * @param  pCardInfo: Pointer to a HAL_SD_CardInfoTypedef structure that  
  *         contains all SD cardinformation  
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_Get_CardInfo(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypedef *pCardInfo)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t tmp = 0U;
  
  pCardInfo->CardType = (uint8_t)(hsd->CardType);
  pCardInfo->RCA      = (uint16_t)(hsd->RCA);
  
  /* Byte 0 */
  tmp = (hsd->CSD[0U] & 0xFF000000U) >> 24U;
  pCardInfo->SD_csd.CSDStruct      = (uint8_t)((tmp & 0xC0U) >> 6U);
  pCardInfo->SD_csd.SysSpecVersion = (uint8_t)((tmp & 0x3CU) >> 2U);
  pCardInfo->SD_csd.Reserved1      = tmp & 0x03U;
  
  /* Byte 1 */
  tmp = (hsd->CSD[0U] & 0x00FF0000U) >> 16U;
  pCardInfo->SD_csd.TAAC = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (hsd->CSD[0U] & 0x0000FF00U) >> 8U;
  pCardInfo->SD_csd.NSAC = (uint8_t)tmp;
  
  /* Byte 3 */
  tmp = hsd->CSD[0U] & 0x000000FFU;
  pCardInfo->SD_csd.MaxBusClkFrec = (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (hsd->CSD[1U] & 0xFF000000U) >> 24U;
  pCardInfo->SD_csd.CardComdClasses = (uint16_t)(tmp << 4U);
  
  /* Byte 5 */
  tmp = (hsd->CSD[1U] & 0x00FF0000U) >> 16U;
  pCardInfo->SD_csd.CardComdClasses |= (uint16_t)((tmp & 0xF0) >> 4U);
  pCardInfo->SD_csd.RdBlockLen       = (uint8_t)(tmp & 0x0FU);
  
  /* Byte 6 */
  tmp = (hsd->CSD[1U] & 0x0000FF00U) >> 8U;
  pCardInfo->SD_csd.PartBlockRead   = (uint8_t)((tmp & 0x80U) >> 7U);
  pCardInfo->SD_csd.WrBlockMisalign = (uint8_t)((tmp & 0x40U) >> 6U);
  pCardInfo->SD_csd.RdBlockMisalign = (uint8_t)((tmp & 0x20U) >> 5U);
  pCardInfo->SD_csd.DSRImpl         = (uint8_t)((tmp & 0x10U) >> 4U);
  pCardInfo->SD_csd.Reserved2       = 0U; /*!< Reserved */
  
  if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0))
  {
    pCardInfo->SD_csd.DeviceSize = (tmp & 0x03U) << 10U;
    
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1U] & 0x000000FFU);
    pCardInfo->SD_csd.DeviceSize |= (tmp) << 2U;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0xFF000000U) >> 24U);
    pCardInfo->SD_csd.DeviceSize |= (tmp & 0xC0U) >> 6U;
    
    pCardInfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38U) >> 3U;
    pCardInfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07U);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x00FF0000U) >> 16U);
    pCardInfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0U) >> 5U;
    pCardInfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1CU) >> 2U;
    pCardInfo->SD_csd.DeviceSizeMul      = (tmp & 0x03U) << 1U;
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x0000FF00U) >> 8U);
    pCardInfo->SD_csd.DeviceSizeMul |= (tmp & 0x80U) >> 7U;
    
    pCardInfo->CardCapacity  = (pCardInfo->SD_csd.DeviceSize + 1U) ;
    pCardInfo->CardCapacity *= (1U << (pCardInfo->SD_csd.DeviceSizeMul + 2U));
    pCardInfo->CardBlockSize = 1U << (pCardInfo->SD_csd.RdBlockLen);
    pCardInfo->CardCapacity *= pCardInfo->CardBlockSize;
  }
  else if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1U] & 0x000000FFU);
    pCardInfo->SD_csd.DeviceSize = (tmp & 0x3FU) << 16U;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0xFF000000U) >> 24U);
    
    pCardInfo->SD_csd.DeviceSize |= (tmp << 8U);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x00FF0000U) >> 16U);
    
    pCardInfo->SD_csd.DeviceSize |= (tmp);
    
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2U] & 0x0000FF00U) >> 8U);
    
    pCardInfo->CardCapacity = (uint64_t)((((uint64_t)pCardInfo->SD_csd.DeviceSize + 1U)) * 512U * 1024U);
    pCardInfo->CardBlockSize = 512U;    
  }
  else
  {
    /* Not supported card type */
    errorstate = SD_ERROR;
  }
    
  pCardInfo->SD_csd.EraseGrSize = (tmp & 0x40U) >> 6U;
  pCardInfo->SD_csd.EraseGrMul  = (tmp & 0x3FU) << 1U;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CSD[2U] & 0x000000FFU);
  pCardInfo->SD_csd.EraseGrMul     |= (tmp & 0x80U) >> 7U;
  pCardInfo->SD_csd.WrProtectGrSize = (tmp & 0x7FU);
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0xFF000000U) >> 24U);
  pCardInfo->SD_csd.WrProtectGrEnable = (tmp & 0x80U) >> 7U;
  pCardInfo->SD_csd.ManDeflECC        = (tmp & 0x60U) >> 5U;
  pCardInfo->SD_csd.WrSpeedFact       = (tmp & 0x1CU) >> 2U;
  pCardInfo->SD_csd.MaxWrBlockLen     = (tmp & 0x03U) << 2U;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0x00FF0000U) >> 16U);
  pCardInfo->SD_csd.MaxWrBlockLen      |= (tmp & 0xC0U) >> 6U;
  pCardInfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20U) >> 5U;
  pCardInfo->SD_csd.Reserved3           = 0U;
  pCardInfo->SD_csd.ContentProtectAppli = (tmp & 0x01U);
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CSD[3U] & 0x0000FF00U) >> 8U);
  pCardInfo->SD_csd.FileFormatGrouop = (tmp & 0x80U) >> 7U;
  pCardInfo->SD_csd.CopyFlag         = (tmp & 0x40U) >> 6U;
  pCardInfo->SD_csd.PermWrProtect    = (tmp & 0x20U) >> 5U;
  pCardInfo->SD_csd.TempWrProtect    = (tmp & 0x10U) >> 4U;
  pCardInfo->SD_csd.FileFormat       = (tmp & 0x0CU) >> 2U;
  pCardInfo->SD_csd.ECC              = (tmp & 0x03U);
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CSD[3U] & 0x000000FFU);
  pCardInfo->SD_csd.CSD_CRC   = (tmp & 0xFEU) >> 1U;
  pCardInfo->SD_csd.Reserved4 = 1U;
  
  /* Byte 0 */
  tmp = (uint8_t)((hsd->CID[0U] & 0xFF000000U) >> 24U);
  pCardInfo->SD_cid.ManufacturerID = tmp;
  
  /* Byte 1 */
  tmp = (uint8_t)((hsd->CID[0U] & 0x00FF0000U) >> 16U);
  pCardInfo->SD_cid.OEM_AppliID = tmp << 8U;
  
  /* Byte 2 */
  tmp = (uint8_t)((hsd->CID[0U] & 0x0000FF00U) >> 8U);
  pCardInfo->SD_cid.OEM_AppliID |= tmp;
  
  /* Byte 3 */
  tmp = (uint8_t)(hsd->CID[0U] & 0x000000FFU);
  pCardInfo->SD_cid.ProdName1 = tmp << 24U;
  
  /* Byte 4 */
  tmp = (uint8_t)((hsd->CID[1U] & 0xFF000000U) >> 24U);
  pCardInfo->SD_cid.ProdName1 |= tmp << 16U;
  
  /* Byte 5 */
  tmp = (uint8_t)((hsd->CID[1U] & 0x00FF0000U) >> 16U);
  pCardInfo->SD_cid.ProdName1 |= tmp << 8U;
  
  /* Byte 6 */
  tmp = (uint8_t)((hsd->CID[1U] & 0x0000FF00U) >> 8U);
  pCardInfo->SD_cid.ProdName1 |= tmp;
  
  /* Byte 7 */
  tmp = (uint8_t)(hsd->CID[1U] & 0x000000FFU);
  pCardInfo->SD_cid.ProdName2 = tmp;
  
  /* Byte 8 */
  tmp = (uint8_t)((hsd->CID[2U] & 0xFF000000U) >> 24U);
  pCardInfo->SD_cid.ProdRev = tmp;
  
  /* Byte 9 */
  tmp = (uint8_t)((hsd->CID[2U] & 0x00FF0000U) >> 16U);
  pCardInfo->SD_cid.ProdSN = tmp << 24U;
  
  /* Byte 10 */
  tmp = (uint8_t)((hsd->CID[2U] & 0x0000FF00U) >> 8U);
  pCardInfo->SD_cid.ProdSN |= tmp << 16U;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CID[2U] & 0x000000FFU);
  pCardInfo->SD_cid.ProdSN |= tmp << 8U;
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CID[3U] & 0xFF000000U) >> 24U);
  pCardInfo->SD_cid.ProdSN |= tmp;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CID[3U] & 0x00FF0000U) >> 16U);
  pCardInfo->SD_cid.Reserved1   |= (tmp & 0xF0U) >> 4U;
  pCardInfo->SD_cid.ManufactDate = (tmp & 0x0FU) << 8U;
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CID[3U] & 0x0000FF00U) >> 8U);
  pCardInfo->SD_cid.ManufactDate |= tmp;
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CID[3U] & 0x000000FFU);
  pCardInfo->SD_cid.CID_CRC   = (tmp & 0xFEU) >> 1U;
  pCardInfo->SD_cid.Reserved2 = 1U;
  
  return errorstate;
}

/**
  * @brief  Enables wide bus operation for the requested card if supported by 
  *         card.
  * @param  hsd: SD handle       
  * @param  WideMode: Specifies the SD card wide bus mode 
  *          This parameter can be one of the following values:
  *            @arg SDIO_BUS_WIDE_8B: 8-bit data transfer (Only for MMC)
  *            @arg SDIO_BUS_WIDE_4B: 4-bit data transfer
  *            @arg SDIO_BUS_WIDE_1B: 1-bit data transfer
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WideBusOperation_Config(SD_HandleTypeDef *hsd, uint32_t WideMode)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  SDIO_InitTypeDef tmpinit;
  
  /* MMC Card does not support this feature */
  if (hsd->CardType == MULTIMEDIA_CARD)
  {
    errorstate = SD_UNSUPPORTED_FEATURE;
    
    return errorstate;
  }
  else if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
    (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    if (WideMode == SDIO_BUS_WIDE_8B)
    {
      errorstate = SD_UNSUPPORTED_FEATURE;
    }
    else if (WideMode == SDIO_BUS_WIDE_4B)
    {
      errorstate = SD_WideBus_Enable(hsd);
    }
    else if (WideMode == SDIO_BUS_WIDE_1B)
    {
      errorstate = SD_WideBus_Disable(hsd);
    }
    else
    {
      /* WideMode is not a valid argument*/
      errorstate = SD_INVALID_PARAMETER;
    }
      
    if (errorstate == SD_OK)
    {
      /* Configure the SDIO peripheral */
      tmpinit.ClockEdge           = hsd->Init.ClockEdge;
      tmpinit.ClockBypass         = hsd->Init.ClockBypass;
      tmpinit.ClockPowerSave      = hsd->Init.ClockPowerSave;
      tmpinit.BusWide             = WideMode;
      tmpinit.HardwareFlowControl = hsd->Init.HardwareFlowControl;
      tmpinit.ClockDiv            = hsd->Init.ClockDiv;
      SDIO_Init(hsd->Instance, tmpinit);
    }
  }
  
  return errorstate;
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_StopTransfer(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Send CMD12 STOP_TRANSMISSION  */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_STOP_TRANSMISSION;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_STOP_TRANSMISSION);
  
  return errorstate;
}

/**
  * @brief  Switches the SD card to High Speed mode.
  *         This API must be used after "Transfer State"
  * @note   This operation should be followed by the configuration 
  *         of PLL to have SDIOCK clock between 67 and 75 MHz
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_HighSpeed (SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  
  uint8_t SD_hs[64U]  = {0U};
  uint32_t SD_scr[2U] = {0U, 0U};
  uint32_t SD_SPEC    = 0U;
  uint32_t count = 0U, *tempbuff = (uint32_t *)SD_hs;
  
  /* Initialize the Data control register */
  hsd->Instance->DCTRL = 0U;
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, SD_scr);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Test the Version supported by the card*/ 
  SD_SPEC = (SD_scr[1U]  & 0x01000000U) | (SD_scr[1U]  & 0x02000000U);
  
  if (SD_SPEC != SD_ALLZERO)
  {
    /* Set Block Size for Card */
    sdio_cmdinitstructure.Argument         = (uint32_t)64U;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
    sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
    sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */
    sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
    sdio_datainitstructure.DataLength    = 64U;
    sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_64B ;
    sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
    sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
    
    /* Send CMD6 switch mode */
    sdio_cmdinitstructure.Argument         = 0x80FFFF01U;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_HS_SWITCH;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure); 
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_HS_SWITCH);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
#ifdef SDIO_STA_STBITERR        
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))      
#endif /* SDIO_STA_STBITERR */
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        for (count = 0U; count < 8U; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8U;
      }
    }
    
    if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
      
      errorstate = SD_DATA_TIMEOUT;
      
      return errorstate;
    }
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
      
      errorstate = SD_DATA_CRC_FAIL;
      
      return errorstate;
    }
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
      
      errorstate = SD_RX_OVERRUN;
      
      return errorstate;
    }
#ifdef SDIO_STA_STBITERR
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
      
      errorstate = SD_START_BIT_ERR;
      
      return errorstate;
    }
#endif /* SDIO_STA_STBITERR */
    else
    {
      /* No error flag set */
    }
    
    count = SD_DATATIMEOUT;
    
    while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0U))
    {
      *tempbuff = SDIO_ReadFIFO(hsd->Instance);
      tempbuff++;
      count--;
    }
    
    /* Clear all the static flags */
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    /* Test if the switch mode HS is ok */
    if ((SD_hs[13U]& 2U) != 2U)
    {
      errorstate = SD_UNSUPPORTED_FEATURE;
    } 
  }
  
  return errorstate;
}

/**
  * @}
  */

/** @addtogroup SD_Exported_Functions_Group4
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in runtime the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the current SD card's status.
  * @param  hsd: SD handle
  * @param  pSDstatus: Pointer to the buffer that will contain the SD card status 
  *         SD Status register)
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_SendSDStatus(SD_HandleTypeDef *hsd, uint32_t *pSDstatus)
{
  SDIO_CmdInitTypeDef  sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t count = 0U;
  
  /* Check SD response */
  if ((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
  }
  
  /* Set block size for card if it is not equal to current block size for card */
  sdio_cmdinitstructure.Argument         = 64U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Send CMD55 */
  sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = 64U;
  sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_64B;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  /* Send ACMD13 (SD_APP_STATUS)  with argument as card's RCA */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_STATUS;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_STATUS);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Get status data */
#ifdef SDIO_STA_STBITERR 
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))    
#endif /* SDIO_STA_STBITERR */
  {
    if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
    {
      for (count = 0U; count < 8U; count++)
      {
        *(pSDstatus + count) = SDIO_ReadFIFO(hsd->Instance);
      }
      
      pSDstatus += 8U;
    }
  }
  
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
#ifdef SDIO_STA_STBITERR
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorstate = SD_START_BIT_ERR;
    
    return errorstate;
  }
#endif /* SDIO_STA_STBITERR */
  else
  {
    /* No error flag set */
  }  
  
  count = SD_DATATIMEOUT;
  while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0U))
  {
    *pSDstatus = SDIO_ReadFIFO(hsd->Instance);
    pSDstatus++;
    count--;
  }
  
  /* Clear all the static status flags*/
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Gets the current sd card data status.
  * @param  hsd: SD handle
  * @retval Data Transfer state
  */
HAL_SD_TransferStateTypedef HAL_SD_GetStatus(SD_HandleTypeDef *hsd)
{
  HAL_SD_CardStateTypedef cardstate =  SD_CARD_TRANSFER;

  /* Get SD card state */
  cardstate = SD_GetState(hsd);
  
  /* Find SD status according to card state*/
  if (cardstate == SD_CARD_TRANSFER)
  {
    return SD_TRANSFER_OK;
  }
  else if(cardstate == SD_CARD_ERROR)
  {
    return SD_TRANSFER_ERROR;
  }
  else
  {
    return SD_TRANSFER_BUSY;
  }
}

/**
  * @brief  Gets the SD card status.
  * @param  hsd: SD handle      
  * @param  pCardStatus: Pointer to the HAL_SD_CardStatusTypedef structure that 
  *         will contain the SD card status information 
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_GetCardStatus(SD_HandleTypeDef *hsd, HAL_SD_CardStatusTypedef *pCardStatus)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t tmp = 0U;
  uint32_t sd_status[16U];
  
  errorstate = HAL_SD_SendSDStatus(hsd, sd_status);
  
  if (errorstate  != SD_OK)
  {
    return errorstate;
  }
  
  /* Byte 0 */
  tmp = (sd_status[0U] & 0xC0U) >> 6U;
  pCardStatus->DAT_BUS_WIDTH = (uint8_t)tmp;
  
  /* Byte 0 */
  tmp = (sd_status[0U] & 0x20U) >> 5U;
  pCardStatus->SECURED_MODE = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (sd_status[2U] & 0xFFU);
  pCardStatus->SD_CARD_TYPE = (uint8_t)(tmp << 8U);
  
  /* Byte 3 */
  tmp = (sd_status[3U] & 0xFFU);
  pCardStatus->SD_CARD_TYPE |= (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (sd_status[4U] & 0xFFU);
  pCardStatus->SIZE_OF_PROTECTED_AREA = (uint8_t)(tmp << 24U);
  
  /* Byte 5 */
  tmp = (sd_status[5U] & 0xFFU);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 16U);
  
  /* Byte 6 */
  tmp = (sd_status[6U] & 0xFFU);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 8U);
  
  /* Byte 7 */
  tmp = (sd_status[7U] & 0xFFU);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)tmp;
  
  /* Byte 8 */
  tmp = (sd_status[8U] & 0xFFU);
  pCardStatus->SPEED_CLASS = (uint8_t)tmp;
  
  /* Byte 9 */
  tmp = (sd_status[9U] & 0xFFU);
  pCardStatus->PERFORMANCE_MOVE = (uint8_t)tmp;
  
  /* Byte 10 */
  tmp = (sd_status[10U] & 0xF0U) >> 4U;
  pCardStatus->AU_SIZE = (uint8_t)tmp;
  
  /* Byte 11 */
  tmp = (sd_status[11U] & 0xFFU);
  pCardStatus->ERASE_SIZE = (uint8_t)(tmp << 8U);
  
  /* Byte 12 */
  tmp = (sd_status[12U] & 0xFFU);
  pCardStatus->ERASE_SIZE |= (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (sd_status[13U] & 0xFCU) >> 2U;
  pCardStatus->ERASE_TIMEOUT = (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (sd_status[13U] & 0x3U);
  pCardStatus->ERASE_OFFSET = (uint8_t)tmp;
  
  return errorstate;
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
  * @brief  SD DMA transfer complete Rx callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SD_DMA_RxCplt(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = (SD_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* DMA transfer is complete */
  hsd->DmaTransferCplt = 1U;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0U)
  {
  }
  
  /* Disable the DMA channel */
  HAL_DMA_Abort(hdma);

  /* Transfer complete user callback */
  HAL_SD_DMA_RxCpltCallback(hsd->hdmarx);   
}

/**
  * @brief  SD DMA transfer Error Rx callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SD_DMA_RxError(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = (SD_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Transfer complete user callback */
  HAL_SD_DMA_RxErrorCallback(hsd->hdmarx);
}

/**
  * @brief  SD DMA transfer complete Tx callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SD_DMA_TxCplt(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = (SD_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* DMA transfer is complete */
  hsd->DmaTransferCplt = 1U;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0U)
  {
  }
 
  /* Disable the DMA channel */
  HAL_DMA_Abort(hdma);

  /* Transfer complete user callback */
  HAL_SD_DMA_TxCpltCallback(hsd->hdmatx);  
}

/**
  * @brief  SD DMA transfer Error Tx callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void SD_DMA_TxError(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = ( SD_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  /* Transfer complete user callback */
  HAL_SD_DMA_TxErrorCallback(hsd->hdmatx);
}

/**
  * @brief  Returns the SD current state.
  * @param  hsd: SD handle
  * @retval SD card current state
  */
static HAL_SD_CardStateTypedef SD_GetState(SD_HandleTypeDef *hsd)
{
  uint32_t resp1 = 0U;
  
  if (SD_SendStatus(hsd, &resp1) != SD_OK)
  {
    return SD_CARD_ERROR;
  }
  else
  {
    return (HAL_SD_CardStateTypedef)((resp1 >> 9U) & 0x0FU);
  }
}

/**
  * @brief  Initializes all cards or single card as the case may be Card(s) come 
  *         into standby state.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_Initialize_Cards(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure; 
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint16_t sd_rca = 1U;
  
  if(SDIO_GetPowerState(hsd->Instance) == 0U) /* Power off */
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
  
  if(hsd->CardType != SECURE_DIGITAL_IO_CARD)
  {
    /* Send CMD2 ALL_SEND_CID */
    sdio_cmdinitstructure.Argument         = 0U;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_ALL_SEND_CID;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_LONG;
    sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
    sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp2Error(hsd);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Get Card identification number data */
    hsd->CID[0U] = SDIO_GetResponse(SDIO_RESP1);
    hsd->CID[1U] = SDIO_GetResponse(SDIO_RESP2);
    hsd->CID[2U] = SDIO_GetResponse(SDIO_RESP3);
    hsd->CID[3U] = SDIO_GetResponse(SDIO_RESP4);
  }
  
  if((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1)    || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
     (hsd->CardType == SECURE_DIGITAL_IO_COMBO_CARD) || (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 */
    /* SD Card publishes its RCA. */
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_REL_ADDR;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp6Error(hsd, SD_CMD_SET_REL_ADDR, &sd_rca);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
  }
  
  if (hsd->CardType != SECURE_DIGITAL_IO_CARD)
  {
    /* Get the SD card RCA */
    hsd->RCA = sd_rca;
    
    /* Send CMD9 SEND_CSD with argument as card's RCA */
    sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_SEND_CSD;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_LONG;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp2Error(hsd);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Get Card Specific Data */
    hsd->CSD[0U] = SDIO_GetResponse(SDIO_RESP1);
    hsd->CSD[1U] = SDIO_GetResponse(SDIO_RESP2);
    hsd->CSD[2U] = SDIO_GetResponse(SDIO_RESP3);
    hsd->CSD[3U] = SDIO_GetResponse(SDIO_RESP4);
  }
  
  /* All cards are initialized */
  return errorstate;
}

/**
  * @brief  Selects of Deselects the corresponding card.
  * @param  hsd: SD handle
  * @param  addr: Address of the card to be selected  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_Select_Deselect(SD_HandleTypeDef *hsd, uint64_t addr)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Send CMD7 SDIO_SEL_DESEL_CARD */
  sdio_cmdinitstructure.Argument         = (uint32_t)addr;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SEL_DESEL_CARD;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SEL_DESEL_CARD);
  
  return errorstate;
}

/**
  * @brief  Enquires cards about their operating voltage and configures clock
  *         controls and stores SD information that will be needed in future
  *         in the SD handle.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_PowerON(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure; 
  __IO HAL_SD_ErrorTypedef errorstate = SD_OK; 
  uint32_t response = 0U, count = 0U, validvoltage = 0U;
  uint32_t sdtype = SD_STD_CAPACITY;
  
  /* Power ON Sequence -------------------------------------------------------*/
  /* Disable SDIO Clock */
  __HAL_SD_SDIO_DISABLE(); 
  
  /* Set Power State to ON */
  SDIO_PowerState_ON(hsd->Instance);
  
  /* 1ms: required power up waiting time before starting the SD initialization 
     sequence */
  HAL_Delay(1);
  
  /* Enable SDIO Clock */
  __HAL_SD_SDIO_ENABLE();
  
  /* CMD0: GO_IDLE_STATE -----------------------------------------------------*/
  /* No CMD response required */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_GO_IDLE_STATE;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_NO;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdError(hsd);
  
  if(errorstate != SD_OK)
  {
    /* CMD Response Timeout (wait for CMDSENT flag) */
    return errorstate;
  }
  
  /* CMD8: SEND_IF_COND ------------------------------------------------------*/
  /* Send CMD8 to verify SD card interface operating condition */
  /* Argument: - [31:12]: Reserved (shall be set to '0')
  - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
  - [7:0]: Check Pattern (recommended 0xAA) */
  /* CMD Response: R7 */
  sdio_cmdinitstructure.Argument         = SD_CHECK_PATTERN;
  sdio_cmdinitstructure.CmdIndex         = SD_SDIO_SEND_IF_COND;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */ 
  errorstate = SD_CmdResp7Error(hsd);
  
  if (errorstate == SD_OK)
  {
    /* SD Card 2.0 */
    hsd->CardType = STD_CAPACITY_SD_CARD_V2_0; 
    sdtype        = SD_HIGH_CAPACITY;
  }
  
  /* Send CMD55 */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  /* If errorstate is Command Timeout, it is a MMC card */
  /* If errorstate is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  if(errorstate == SD_OK)
  {
    /* SD CARD */
    /* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {
      
      /* SEND CMD55 APP_CMD with RCA as 0 */
      sdio_cmdinitstructure.Argument         = 0U;
      sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
      sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
      sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
      sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
      SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
      
      /* Check for error conditions */
      errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
      
      if(errorstate != SD_OK)
      {
        return errorstate;
      }
      
      /* Send CMD41 */
      sdio_cmdinitstructure.Argument         = SD_VOLTAGE_WINDOW_SD | sdtype;
      sdio_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_OP_COND;
      sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
      sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
      sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
      SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
      
      /* Check for error conditions */
      errorstate = SD_CmdResp3Error(hsd);
      
      if(errorstate != SD_OK)
      {
        return errorstate;
      }
      
      /* Get command response */
      response = SDIO_GetResponse(SDIO_RESP1);
      
      /* Get operating voltage*/
      validvoltage = (((response >> 31U) == 1U) ? 1U : 0U);
      
      count++;
    }
    
    if(count >= SD_MAX_VOLT_TRIAL)
    {
      errorstate = SD_INVALID_VOLTRANGE;
      
      return errorstate;
    }
    
    if((response & SD_HIGH_CAPACITY) == SD_HIGH_CAPACITY) /* (response &= SD_HIGH_CAPACITY) */
    {
      hsd->CardType = HIGH_CAPACITY_SD_CARD;
    }
    
  } /* else MMC Card */
  
  return errorstate;
}

/**
  * @brief  Turns the SDIO output signals off.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_PowerOFF(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Set Power State to OFF */
  SDIO_PowerState_OFF(hsd->Instance);
  
  return errorstate;
}

/**
  * @brief  Returns the current card's status.
  * @param  hsd: SD handle
  * @param  pCardStatus: pointer to the buffer that will contain the SD card 
  *         status (Card Status register)  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_SendStatus(SD_HandleTypeDef *hsd, uint32_t *pCardStatus)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  if(pCardStatus == NULL)
  {
    errorstate = SD_INVALID_PARAMETER;
    
    return errorstate;
  }
  
  /* Send Status command */
  sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SEND_STATUS;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SEND_STATUS);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Get SD card status */
  *pCardStatus = SDIO_GetResponse(SDIO_RESP1);
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for CMD0.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdError(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t timeout, tmp;
  
  timeout = SDIO_CMD0TIMEOUT;
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDSENT);
    
  while((timeout > 0U) && (!tmp))
  {
    tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDSENT);
    timeout--;
  }
  
  if(timeout == 0U)
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for R7 response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp7Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_ERROR;
  uint32_t timeout = SDIO_CMD0TIMEOUT, tmp;
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT); 
  
  while((!tmp) && (timeout > 0U))
  {
    tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT);
    timeout--;
  }
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT); 
  
  if((timeout == 0U) || tmp)
  {
    /* Card is not V2.0 compliant or card does not support the set voltage range */
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDREND))
  {
    /* Card is SD V2.0 compliant */
    errorstate = SD_OK;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CMDREND);
    
    return errorstate;
  }
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  hsd: SD handle
  * @param  SD_CMD: The sent command index  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp1Error(SD_HandleTypeDef *hsd, uint8_t SD_CMD)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t response_r1;
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  
  /* Check response received is of desired command */
  if(SDIO_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* We have received response, retrieve it for analysis  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);
  
  if((response_r1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return errorstate;
  }
  
  if((response_r1 & SD_OCR_ADDR_OUT_OF_RANGE) == SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }
  
  if((response_r1 & SD_OCR_ADDR_MISALIGNED) == SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }
  
  if((response_r1 & SD_OCR_BLOCK_LEN_ERR) == SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }
  
  if((response_r1 & SD_OCR_ERASE_SEQ_ERR) == SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }
  
  if((response_r1 & SD_OCR_BAD_ERASE_PARAM) == SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }
  
  if((response_r1 & SD_OCR_WRITE_PROT_VIOLATION) == SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }
  
  if((response_r1 & SD_OCR_LOCK_UNLOCK_FAILED) == SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }
  
  if((response_r1 & SD_OCR_COM_CRC_FAILED) == SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  if((response_r1 & SD_OCR_ILLEGAL_CMD) == SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }
  
  if((response_r1 & SD_OCR_CARD_ECC_FAILED) == SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }
  
  if((response_r1 & SD_OCR_CC_ERROR) == SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }
  
  if((response_r1 & SD_OCR_GENERAL_UNKNOWN_ERROR) == SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }
  
  if((response_r1 & SD_OCR_STREAM_READ_UNDERRUN) == SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }
  
  if((response_r1 & SD_OCR_STREAM_WRITE_OVERRUN) == SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }
  
  if((response_r1 & SD_OCR_CID_CSD_OVERWRITE) == SD_OCR_CID_CSD_OVERWRITE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }
  
  if((response_r1 & SD_OCR_WP_ERASE_SKIP) == SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }
  
  if((response_r1 & SD_OCR_CARD_ECC_DISABLED) == SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }
  
  if((response_r1 & SD_OCR_ERASE_RESET) == SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }
  
  if((response_r1 & SD_OCR_AKE_SEQ_ERROR) == SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for R3 (OCR) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp3Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  while (!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp2Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  while (!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
    
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Checks for error conditions for R6 (RCA) response.
  * @param  hsd: SD handle
  * @param  SD_CMD: The sent command index
  * @param  pRCA: Pointer to the variable that will contain the SD card relative 
  *         address RCA   
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp6Error(SD_HandleTypeDef *hsd, uint8_t SD_CMD, uint16_t *pRCA)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t response_r1;
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Check response received is of desired command */
  if(SDIO_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);
  
  if((response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)) == SD_ALLZERO)
  {
    *pRCA = (uint16_t) (response_r1 >> 16U);
    
    return errorstate;
  }
  
  if((response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR) == SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }
  
  if((response_r1 & SD_R6_ILLEGAL_CMD) == SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }
  
  if((response_r1 & SD_R6_COM_CRC_FAILED) == SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  return errorstate;
}

/**
  * @brief  Enables the SDIO wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Enable(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  uint32_t scr[2U] = {0U, 0U};
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
  }
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, scr);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* If requested card supports wide bus operation */
  if((scr[1U] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
    sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
    sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    sdio_cmdinitstructure.Argument         = 2U;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_SD_SET_BUSWIDTH);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    return errorstate;
  }
  else
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
}   

/**
  * @brief  Disables the SDIO wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Disable(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  uint32_t scr[2U] = {0U, 0U};
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
  }
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, scr);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* If requested card supports 1 bit mode operation */
  if((scr[1U] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA */
    sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
    sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
    sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
    sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 0 for single bus mode */
    sdio_cmdinitstructure.Argument         = 0U;
    sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_SD_SET_BUSWIDTH);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    return errorstate;
  }
  else
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
}
  
  
/**
  * @brief  Finds the SD card SCR register value.
  * @param  hsd: SD handle
  * @param  pSCR: pointer to the buffer that will contain the SCR value  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_FindSCR(SD_HandleTypeDef *hsd, uint32_t *pSCR)
{
  SDIO_CmdInitTypeDef  sdio_cmdinitstructure;
  SDIO_DataInitTypeDef sdio_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t index = 0U;
  uint32_t tempscr[2U] = {0U, 0U};
  
  /* Set Block Size To 8 Bytes */
  /* Send CMD55 APP_CMD with argument as card's RCA */
  sdio_cmdinitstructure.Argument         = (uint32_t)8U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Send CMD55 APP_CMD with argument as card's RCA */
  sdio_cmdinitstructure.Argument         = (uint32_t)((hsd->RCA) << 16U);
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  sdio_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdio_datainitstructure.DataLength    = 8U;
  sdio_datainitstructure.DataBlockSize = SDIO_DATABLOCK_SIZE_8B;
  sdio_datainitstructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  sdio_datainitstructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  sdio_datainitstructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &sdio_datainitstructure);
  
  /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  sdio_cmdinitstructure.Argument         = 0U;
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_SEND_SCR;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_SEND_SCR);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
#ifdef SDIO_STA_STBITERR  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
#else /* SDIO_STA_STBITERR not defined */
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND))
#endif /* SDIO_STA_STBITERR */
  {
    if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL))
    {
      *(tempscr + index) = SDIO_ReadFIFO(hsd->Instance);
      index++;
    }
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
#ifdef SDIO_STA_STBITERR
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorstate = SD_START_BIT_ERR;
    
    return errorstate;
  }
#endif /* SDIO_STA_STBITERR */
  else
  {
    /* No error flag set */
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  *(pSCR + 1U) = ((tempscr[0U] & SD_0TO7BITS) << 24U)  | ((tempscr[0U] & SD_8TO15BITS) << 8U) |\
    ((tempscr[0U] & SD_16TO23BITS) >> 8U) | ((tempscr[0U] & SD_24TO31BITS) >> 24U);
  
  *(pSCR) = ((tempscr[1U] & SD_0TO7BITS) << 24U)  | ((tempscr[1U] & SD_8TO15BITS) << 8U) |\
    ((tempscr[1U] & SD_16TO23BITS) >> 8U) | ((tempscr[1U] & SD_24TO31BITS) >> 24U);
  
  return errorstate;
}

/**
  * @brief  Checks if the SD card is in programming state.
  * @param  hsd: SD handle
  * @param  pStatus: pointer to the variable that will contain the SD card state  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_IsCardProgramming(SD_HandleTypeDef *hsd, uint8_t *pStatus)
{
  SDIO_CmdInitTypeDef sdio_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  __IO uint32_t responseR1 = 0U;
  
  sdio_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16U);
  sdio_cmdinitstructure.CmdIndex         = SD_CMD_SEND_STATUS;
  sdio_cmdinitstructure.Response         = SDIO_RESPONSE_SHORT;
  sdio_cmdinitstructure.WaitForInterrupt = SDIO_WAIT_NO;
  sdio_cmdinitstructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &sdio_cmdinitstructure);
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Check response received is of desired command */
  if((uint32_t)SDIO_GetCommandResponse(hsd->Instance) != SD_CMD_SEND_STATUS)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  
  /* We have received response, retrieve it for analysis */
  responseR1 = SDIO_GetResponse(SDIO_RESP1);
  
  /* Find out card status */
  *pStatus = (uint8_t)((responseR1 >> 9U) & 0x0000000FU);
  
  if((responseR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return errorstate;
  }
  
  if((responseR1 & SD_OCR_ADDR_OUT_OF_RANGE) == SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }
  
  if((responseR1 & SD_OCR_ADDR_MISALIGNED) == SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }
  
  if((responseR1 & SD_OCR_BLOCK_LEN_ERR) == SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }
  
  if((responseR1 & SD_OCR_ERASE_SEQ_ERR) == SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }
  
  if((responseR1 & SD_OCR_BAD_ERASE_PARAM) == SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }
  
  if((responseR1 & SD_OCR_WRITE_PROT_VIOLATION) == SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }
  
  if((responseR1 & SD_OCR_LOCK_UNLOCK_FAILED) == SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }
  
  if((responseR1 & SD_OCR_COM_CRC_FAILED) == SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  if((responseR1 & SD_OCR_ILLEGAL_CMD) == SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }
  
  if((responseR1 & SD_OCR_CARD_ECC_FAILED) == SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }
  
  if((responseR1 & SD_OCR_CC_ERROR) == SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }
  
  if((responseR1 & SD_OCR_GENERAL_UNKNOWN_ERROR) == SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }
  
  if((responseR1 & SD_OCR_STREAM_READ_UNDERRUN) == SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }
  
  if((responseR1 & SD_OCR_STREAM_WRITE_OVERRUN) == SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }
  
  if((responseR1 & SD_OCR_CID_CSD_OVERWRITE) == SD_OCR_CID_CSD_OVERWRITE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }
  
  if((responseR1 & SD_OCR_WP_ERASE_SKIP) == SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }
  
  if((responseR1 & SD_OCR_CARD_ECC_DISABLED) == SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }
  
  if((responseR1 & SD_OCR_ERASE_RESET) == SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }
  
  if((responseR1 & SD_OCR_AKE_SEQ_ERROR) == SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }
  
  return errorstate;
}   

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || 
          STM32F412Rx || STM32F412Cx */
#endif /* HAL_SD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
