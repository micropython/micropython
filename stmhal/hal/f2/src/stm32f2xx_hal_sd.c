/**
  ******************************************************************************
  * @file    stm32f2xx_hal_sd.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
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
        (##) Enable the SDIO interface clock using __SDIO_CLK_ENABLE(); 
        (##) SDIO pins configuration for SD card
            (+++) Enable the clock for the SDIO GPIOs using the functions __GPIOx_CLK_ENABLE();   
            (+++) Configure these SDIO pins as alternate function pull-up using HAL_GPIO_Init()
                  and according to your pin assignment;
        (##) DMA Configuration if you need to use DMA process (HAL_SD_ReadBlocks_DMA()
             and HAL_SD_WriteBlocks_DMA() APIs).
            (+++) Enable the DMAx interface clock using __DMAx_CLK_ENABLE(); 
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
    the SD Card and put it into StandBy State (Ready for data transfer). 
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
        The SD Card frequency (SDIO_CK) is computed as follows:

           SDIO_CK = SDIOCLK / (ClockDiv + 2) 

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
        This function support only 512-byte block length (the block size should be 
        chosen as 512 byte).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.

    (+) You can read from SD card in DMA mode by using function HAL_SD_ReadBlocks_DMA().
        This function support only 512-byte block length (the block size should be 
        chosen as 512 byte).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to call the function HAL_SD_CheckReadOperation(), to insure
        that the read transfer is done correctly in both DMA and SD sides.
  
  *** SD Card Write operation ***
  =============================== 
  [..] 
    (+) You can write to SD card in polling mode by using function HAL_SD_WriteBlocks(). 
        This function support only 512-byte block length (the block size should be 
        chosen as 512 byte).
        You can choose either one block read operation or multiple block read operation 
        by adjusting the "NumberOfBlocks" parameter.

    (+) You can write to SD card in DMA mode by using function HAL_SD_WriteBlocks_DMA().
        This function support only 512-byte block length (the block size should be 
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
#include "stm32f2xx_hal.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @defgroup SD
  * @brief SD HAL module driver
  * @{
  */

#ifdef HAL_SD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** 
  * @brief  SDIO Static flags, TimeOut, FIFO Address  
  */
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000)

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

/** 
  * @brief  Masks for R6 Response 
  */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)

#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

/** 
  * @brief  Command Class Supported 
  */
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020)

/** 
  * @brief  Following commands are SD Card Specific commands.
  *         SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_SDIO_SEND_IF_COND            ((uint32_t)SD_CMD_HS_SEND_EXT_CSD)

  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
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

/** @defgroup SD_Private_Functions
  * @{
  */

/** @defgroup SD_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
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
  __IO HAL_SD_ErrorTypedef errorState = SD_OK;
  SD_InitTypeDef tmpInit;
  
  /* Initialize the low level hardware (MSP) */
  HAL_SD_MspInit(hsd);
  
  /* Default SDIO peripheral configuration for SD card initialization */
  tmpInit.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
  tmpInit.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
  tmpInit.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
  tmpInit.BusWide             = SDIO_BUS_WIDE_1B;
  tmpInit.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  tmpInit.ClockDiv            = SDIO_INIT_CLK_DIV;
  
  /* Initialize SDIO peripheral interface with default configuration */
  SDIO_Init(hsd->Instance, tmpInit);
  
  /* Identify card operating voltage */
  errorState = SD_PowerON(hsd); 
  
  if(errorState != SD_OK)     
  {
    return errorState;
  }
  
  /* Initialize the present SDIO card(s) and put them in idle state */
  errorState = SD_Initialize_Cards(hsd);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Read CSD/CID MSD registers */
  errorState = HAL_SD_Get_CardInfo(hsd, SDCardInfo);
  
  if (errorState == SD_OK)
  {
    /* Select the Card */
    errorState = SD_Select_Deselect(hsd, (uint32_t)(((uint32_t)SDCardInfo->RCA) << 16));
  }
  
  /* Configure SDIO peripheral interface */
  SDIO_Init(hsd->Instance, hsd->Init);   
  
  return errorState;
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
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SD_Group2 IO operation functions 
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
  *          This parameter should be 512
  * @param  NumberOfBlocks: Number of SD blocks to read   
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef  SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t count = 0, *tempbuff = (uint32_t *)pReadBuffer;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }
  
  /* Set Block Size for Card */ 
  SDIO_CmdInitStructure.Argument         = (uint32_t) BlockSize;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = NumberOfBlocks * BlockSize;
  SDIO_DataInitStructure.DataBlockSize = (uint32_t)(9 << 4);
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  if(NumberOfBlocks > 1)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;    
  }
  
  SDIO_CmdInitStructure.Argument         = (uint32_t)ReadAddr;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Read block(s) in polling mode */
  if(NumberOfBlocks > 1)
  {
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_READ_MULT_BLOCK);
    
    if (errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Poll on SDIO flags */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_FLAG_STBITERR))
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        /* Read data from SDIO Rx FIFO */
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
      }
    }      
  }
  else
  {
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_READ_SINGLE_BLOCK); 
    
    if (errorState != SD_OK)
    {
      return errorState;
    }    
    
    /* In case of single block transfer, no need of stop transfer at all */
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        /* Read data from SDIO Rx FIFO */
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
      }
    }   
  }
  
  /* Send stop transmission command in case of multiblock read */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1))
  {    
    if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) ||\
      (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
        (hsd->CardType == HIGH_CAPACITY_SD_CARD))
    {
      /* Send stop transmission command */
      errorState = HAL_SD_StopTransfer(hsd);
    }
  }
  
  /* Get error state */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorState = SD_DATA_TIMEOUT;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorState = SD_DATA_CRC_FAIL;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorState = SD_RX_OVERRUN;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorState = SD_START_BIT_ERR;
    
    return errorState;
  }
  
  count = SD_DATATIMEOUT;
  
  /* Empty FIFO if there is still any data */
  while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0))
  {
    *tempbuff = SDIO_ReadFIFO(hsd->Instance);
    tempbuff++;
    count--;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorState;
}

/**
  * @brief  Allows to write block(s) to a specified address in a card. The Data
  *         transfer is managed by polling mode.  
  * @param  hsd: SD handle
  * @param  pWriteBuffer: pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written 
  * @param  BlockSize: SD card Data block size 
  *          This parameter should be 512.
  * @param  NumberOfBlocks: Number of SD blocks to write 
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t TotalNumberOfBytes = 0, bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)pWriteBuffer;
  uint8_t cardstate  = 0;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }
  
  /* Set Block Size for Card */ 
  SDIO_CmdInitStructure.Argument         = (uint32_t)BlockSize;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  if(NumberOfBlocks > 1)
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  else
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  
  SDIO_CmdInitStructure.Argument         = (uint32_t)WriteAddr;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1)
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_WRITE_MULT_BLOCK);
  }
  else
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_WRITE_SINGLE_BLOCK);
  }  
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Set total number of bytes to write */
  TotalNumberOfBytes = NumberOfBlocks * BlockSize;
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = NumberOfBlocks * BlockSize;
  SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  /* Write block(s) in polling mode */
  if(NumberOfBlocks > 1)
  {
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DATAEND | SDIO_FLAG_STBITERR))
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXFIFOHE))
      {
        if ((TotalNumberOfBytes - bytestransferred) < 32)
        {
          restwords = ((TotalNumberOfBytes - bytestransferred) % 4 == 0) ? ((TotalNumberOfBytes - bytestransferred) / 4) : (( TotalNumberOfBytes -  bytestransferred) / 4 + 1);
          
          /* Write data to SDIO Tx FIFO */
          for (count = 0; count < restwords; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++;
            bytestransferred += 4;
          }
        }
        else
        {
          /* Write data to SDIO Tx FIFO */
          for (count = 0; count < 8; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, (tempbuff + count));
          }
          
          tempbuff += 8;
          bytestransferred += 32;
        }
      }
    }   
  }
  else
  {
    /* In case of single data block transfer no need of stop command at all */ 
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXFIFOHE))
      {
        if ((TotalNumberOfBytes - bytestransferred) < 32)
        {
          restwords = ((TotalNumberOfBytes - bytestransferred) % 4 == 0) ? ((TotalNumberOfBytes - bytestransferred) / 4) : (( TotalNumberOfBytes -  bytestransferred) / 4 + 1);
          
          /* Write data to SDIO Tx FIFO */
          for (count = 0; count < restwords; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++; 
            bytestransferred += 4;
          }
        }
        else
        {
          /* Write data to SDIO Tx FIFO */
          for (count = 0; count < 8; count++)
          {
            SDIO_WriteFIFO(hsd->Instance, (tempbuff + count));
          }
          
          tempbuff += 8;
          bytestransferred += 32;
        }
      }
    }  
  }
  
  /* Send stop transmission command in case of multiblock write */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DATAEND) && (NumberOfBlocks > 1))
  {    
    if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
      (hsd->CardType == HIGH_CAPACITY_SD_CARD))
    {
      /* Send stop transmission command */
      errorState = HAL_SD_StopTransfer(hsd);
    }
  }
  
  /* Get error state */
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorState = SD_DATA_TIMEOUT;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorState = SD_DATA_CRC_FAIL;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXUNDERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_TXUNDERR);
    
    errorState = SD_TX_UNDERRUN;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorState = SD_START_BIT_ERR;
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* Wait till the card is in programming state */
  errorState = SD_IsCardProgramming(hsd, &cardstate);
  
  while ((errorState == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorState = SD_IsCardProgramming(hsd, &cardstate);
  }
  
  return errorState;
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
  *         This paramater should be 512.
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_DMA(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  /* Initialize handle flags */
  hsd->SdTransferCplt  = 0;
  hsd->DmaTransferCplt = 0;
  hsd->SdTransferErr   = SD_OK; 
  
  /* Initialize SD Read operation */
  if(NumberOfBlocks > 1)
  {
    hsd->SdOperation = SD_READ_MULTIPLE_BLOCK;
  }
  else
  {
    hsd->SdOperation = SD_READ_SINGLE_BLOCK;
  }
  
  /* Enable transfer interrupts */
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_RXOVERR  |\
                                SDIO_IT_STBITERR));
  
  /* Enable SDIO DMA transfer */
  __HAL_SD_SDIO_DMA_ENABLE();
  
  /* Configure DMA user callbacks */
  hsd->hdmarx->XferCpltCallback  = SD_DMA_RxCplt;
  hsd->hdmarx->XferErrorCallback = SD_DMA_RxError;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsd->hdmarx, (uint32_t)SDIO_FIFO_ADDRESS, (uint32_t)pReadBuffer, (uint32_t)(BlockSize * NumberOfBlocks));
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }
  
  /* Set Block Size for Card */ 
  SDIO_CmdInitStructure.Argument         = (uint32_t)BlockSize;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = BlockSize * NumberOfBlocks;
  SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  /* Check number of blocks command */
  if(NumberOfBlocks > 1)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
  }
  
  SDIO_CmdInitStructure.Argument         = (uint32_t)ReadAddr;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1)
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_READ_MULT_BLOCK);
  }
  else
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_READ_SINGLE_BLOCK);
  }
  
  /* Update the SD transfer error in SD handle */
  hsd->SdTransferErr = errorState;
  
  return errorState;
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
  *          This parameter should be 512.
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_DMA(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  /* Initialize handle flags */
  hsd->SdTransferCplt  = 0;
  hsd->DmaTransferCplt = 0;
  hsd->SdTransferErr   = SD_OK;
  
  /* Initialize SD Write operation */
  if(NumberOfBlocks > 1)
  {
    hsd->SdOperation = SD_WRITE_MULTIPLE_BLOCK;
  }
  else
  {
    hsd->SdOperation = SD_WRITE_SINGLE_BLOCK;
  }  
  
  /* Enable transfer interrupts */
  __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DCRCFAIL |\
                                SDIO_IT_DTIMEOUT |\
                                SDIO_IT_DATAEND  |\
                                SDIO_IT_TXUNDERR |\
                                SDIO_IT_STBITERR)); 
  
  /* Configure DMA user callbacks */
  hsd->hdmatx->XferCpltCallback  = SD_DMA_TxCplt;
  hsd->hdmatx->XferErrorCallback = SD_DMA_TxError;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsd->hdmatx, (uint32_t)pWriteBuffer, (uint32_t)SDIO_FIFO_ADDRESS, (uint32_t)(BlockSize * NumberOfBlocks));

  /* Enable SDIO DMA transfer */
  __HAL_SD_SDIO_DMA_ENABLE();
  
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }

  /* Set Block Size for Card */ 
  SDIO_CmdInitStructure.Argument         = (uint32_t)BlockSize;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);

  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);

  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Check number of blocks command */
  if(NumberOfBlocks <= 1)
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  else
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  
  SDIO_CmdInitStructure.Argument         = (uint32_t)WriteAddr;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);

  /* Check for error conditions */
  if(NumberOfBlocks > 1)
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_WRITE_MULT_BLOCK);
  }
  else
  {
    errorState = SD_CmdResp1Error(hsd, SD_CMD_WRITE_SINGLE_BLOCK);
  }
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = BlockSize * NumberOfBlocks;
  SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_512B;
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_CARD;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  hsd->SdTransferErr = errorState;
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t timeout = Timeout;
  uint32_t tmp1, tmp2;
  HAL_SD_ErrorTypedef tmp3;
  
  /* Wait for DMA/SD transfer end or SD error variables to be in SD handle */
  tmp1 = hsd->DmaTransferCplt; 
  tmp2 = hsd->SdTransferCplt;
  tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    
  while ((tmp1 == 0) && (tmp2 == 0) && (tmp3 == SD_OK) && (timeout > 0))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;    
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Wait until the Rx transfer is no longer active */
  while((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXACT)) && (timeout > 0))
  {
    timeout--;  
  }
  
  /* Send stop command in multiblock read */
  if (hsd->SdOperation == SD_READ_MULTIPLE_BLOCK)
  {
    errorState = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0) && (errorState == SD_OK))
  {
    errorState = SD_DATA_TIMEOUT;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* Return error state */
  if (hsd->SdTransferErr != SD_OK)
  {
    return (HAL_SD_ErrorTypedef)(hsd->SdTransferErr);
  }
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t timeout = Timeout;
  uint32_t tmp1, tmp2;
  HAL_SD_ErrorTypedef tmp3;

  /* Wait for DMA/SD transfer end or SD error variables to be in SD handle */
  tmp1 = hsd->DmaTransferCplt; 
  tmp2 = hsd->SdTransferCplt;
  tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    
  while ((tmp1 == 0) && (tmp2 == 0) && (tmp3 == SD_OK) && (timeout > 0))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Wait until the Tx transfer is no longer active */
  while((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_TXACT))  && (timeout > 0))
  {
    timeout--;  
  }

  /* Send stop command in multiblock write */
  if (hsd->SdOperation == SD_WRITE_MULTIPLE_BLOCK)
  {
    errorState = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0) && (errorState == SD_OK))
  {
    errorState = SD_DATA_TIMEOUT;
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

  return errorState; 
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  
  uint32_t delay         = 0;
  __IO uint32_t maxdelay = 0;
  uint8_t cardstate      = 0;
  
  /* Check if the card command class supports erase command */
  if (((hsd->CSD[1] >> 20) & SD_CCCC_ERASE) == 0)
  {
    errorState = SD_REQUEST_NOT_APPLICABLE;
    
    return errorState;
  }
  
  /* Get max delay value */
  maxdelay = 120000 / (((hsd->Instance->CLKCR) & 0xFF) + 2);
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorState = SD_LOCK_UNLOCK_FAILED;
    
    return errorState;
  }
  
  /* Get start and end block for high capacity cards */
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    startaddr /= 512;
    endaddr   /= 512;
  }
  
  /* According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
  if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
    (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    /* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
    SDIO_CmdInitStructure.Argument         =(uint32_t)startaddr;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_START;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
    SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_START);
    
    if (errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    SDIO_CmdInitStructure.Argument         = (uint32_t)endaddr;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_END;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_END);
    
    if (errorState != SD_OK)
    {
      return errorState;
    }
  }
  
  /* Send CMD38 ERASE */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_ERASE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_ERASE);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  for (; delay < maxdelay; delay++)
  {
  }
  
  /* Wait untill the card is in programming state */
  errorState = SD_IsCardProgramming(hsd, &cardstate);
  
  delay = SD_DATATIMEOUT;
  
  while ((delay > 0) && (errorState == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorState = SD_IsCardProgramming(hsd, &cardstate);
    delay--;
  }
  
  return errorState;
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
    hsd->SdTransferCplt = 1;

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
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_IT_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    hsd->SdTransferErr = SD_START_BIT_ERR;
    
    HAL_SD_XferErrorCallback(hsd);
  }

  /* Disable all SDIO peripheral interrupt sources */
  __HAL_SD_SDIO_DISABLE_IT(hsd, SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND  |\
                                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |\
                                SDIO_IT_RXOVERR  | SDIO_IT_STBITERR);                               
}


/**
  * @brief  SD end of transfer callback.
  * @param  hsd: SD handle 
  * @retval None
  */
__weak void HAL_SD_XferCpltCallback(SD_HandleTypeDef *hsd)
{
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
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_XferErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Rx callback in non blocking mode.
  * @param  hdma: DMA handle 
  * @retval None
  */
__weak void HAL_SD_DMA_RxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_RxCpltCallback could be implemented in the user file
   */ 
}  

/**
  * @brief  SD DMA transfer complete Rx error callback.
  * @param  hdma: DMA handle 
  * @retval None
  */
__weak void HAL_SD_DMA_RxErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_RxErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Tx callback in non blocking mode.
  * @param  hdma: DMA handle 
  * @retval None
  */
__weak void HAL_SD_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_TxCpltCallback could be implemented in the user file
   */ 
}  

/**
  * @brief  SD DMA transfer complete error Tx callback.
  * @param  hdma: DMA handle 
  * @retval None
  */
__weak void HAL_SD_DMA_TxErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SD_DMA_TxErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SD_Group3 Peripheral Control functions 
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t tmp = 0;
  
  pCardInfo->CardType = (uint8_t)(hsd->CardType);
  pCardInfo->RCA      = (uint16_t)(hsd->RCA);
  
  /* Byte 0 */
  tmp = (hsd->CSD[0] & 0xFF000000) >> 24;
  pCardInfo->SD_csd.CSDStruct      = (uint8_t)((tmp & 0xC0) >> 6);
  pCardInfo->SD_csd.SysSpecVersion = (uint8_t)((tmp & 0x3C) >> 2);
  pCardInfo->SD_csd.Reserved1      = tmp & 0x03;
  
  /* Byte 1 */
  tmp = (hsd->CSD[0] & 0x00FF0000) >> 16;
  pCardInfo->SD_csd.TAAC = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (hsd->CSD[0] & 0x0000FF00) >> 8;
  pCardInfo->SD_csd.NSAC = (uint8_t)tmp;
  
  /* Byte 3 */
  tmp = hsd->CSD[0] & 0x000000FF;
  pCardInfo->SD_csd.MaxBusClkFrec = (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (hsd->CSD[1] & 0xFF000000) >> 24;
  pCardInfo->SD_csd.CardComdClasses = (uint16_t)(tmp << 4);
  
  /* Byte 5 */
  tmp = (hsd->CSD[1] & 0x00FF0000) >> 16;
  pCardInfo->SD_csd.CardComdClasses |= (uint16_t)((tmp & 0xF0) >> 4);
  pCardInfo->SD_csd.RdBlockLen       = (uint8_t)(tmp & 0x0F);
  
  /* Byte 6 */
  tmp = (hsd->CSD[1] & 0x0000FF00) >> 8;
  pCardInfo->SD_csd.PartBlockRead   = (uint8_t)((tmp & 0x80) >> 7);
  pCardInfo->SD_csd.WrBlockMisalign = (uint8_t)((tmp & 0x40) >> 6);
  pCardInfo->SD_csd.RdBlockMisalign = (uint8_t)((tmp & 0x20) >> 5);
  pCardInfo->SD_csd.DSRImpl         = (uint8_t)((tmp & 0x10) >> 4);
  pCardInfo->SD_csd.Reserved2       = 0; /*!< Reserved */
  
  if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0))
  {
    pCardInfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;
    
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1] & 0x000000FF);
    pCardInfo->SD_csd.DeviceSize |= (tmp) << 2;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2] & 0xFF000000) >> 24);
    pCardInfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;
    
    pCardInfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    pCardInfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2] & 0x00FF0000) >> 16);
    pCardInfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    pCardInfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    pCardInfo->SD_csd.DeviceSizeMul      = (tmp & 0x03) << 1;
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2] & 0x0000FF00) >> 8);
    pCardInfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    
    pCardInfo->CardCapacity  = (pCardInfo->SD_csd.DeviceSize + 1) ;
    pCardInfo->CardCapacity *= (1 << (pCardInfo->SD_csd.DeviceSizeMul + 2));
    pCardInfo->CardBlockSize = 1 << (pCardInfo->SD_csd.RdBlockLen);
    pCardInfo->CardCapacity *= pCardInfo->CardBlockSize;
  }
  else if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    /* Byte 7 */
    tmp = (uint8_t)(hsd->CSD[1] & 0x000000FF);
    pCardInfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;
    
    /* Byte 8 */
    tmp = (uint8_t)((hsd->CSD[2] & 0xFF000000) >> 24);
    
    pCardInfo->SD_csd.DeviceSize |= (tmp << 8);
    
    /* Byte 9 */
    tmp = (uint8_t)((hsd->CSD[2] & 0x00FF0000) >> 16);
    
    pCardInfo->SD_csd.DeviceSize |= (tmp);
    
    /* Byte 10 */
    tmp = (uint8_t)((hsd->CSD[2] & 0x0000FF00) >> 8);
    
    pCardInfo->CardCapacity  = ((pCardInfo->SD_csd.DeviceSize + 1)) * 512 * 1024;
    pCardInfo->CardBlockSize = 512;    
  }
    
  pCardInfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
  pCardInfo->SD_csd.EraseGrMul  = (tmp & 0x3F) << 1;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CSD[2] & 0x000000FF);
  pCardInfo->SD_csd.EraseGrMul     |= (tmp & 0x80) >> 7;
  pCardInfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CSD[3] & 0xFF000000) >> 24);
  pCardInfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
  pCardInfo->SD_csd.ManDeflECC        = (tmp & 0x60) >> 5;
  pCardInfo->SD_csd.WrSpeedFact       = (tmp & 0x1C) >> 2;
  pCardInfo->SD_csd.MaxWrBlockLen     = (tmp & 0x03) << 2;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CSD[3] & 0x00FF0000) >> 16);
  pCardInfo->SD_csd.MaxWrBlockLen      |= (tmp & 0xC0) >> 6;
  pCardInfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
  pCardInfo->SD_csd.Reserved3           = 0;
  pCardInfo->SD_csd.ContentProtectAppli = (tmp & 0x01);
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CSD[3] & 0x0000FF00) >> 8);
  pCardInfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
  pCardInfo->SD_csd.CopyFlag         = (tmp & 0x40) >> 6;
  pCardInfo->SD_csd.PermWrProtect    = (tmp & 0x20) >> 5;
  pCardInfo->SD_csd.TempWrProtect    = (tmp & 0x10) >> 4;
  pCardInfo->SD_csd.FileFormat       = (tmp & 0x0C) >> 2;
  pCardInfo->SD_csd.ECC              = (tmp & 0x03);
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CSD[3] & 0x000000FF);
  pCardInfo->SD_csd.CSD_CRC   = (tmp & 0xFE) >> 1;
  pCardInfo->SD_csd.Reserved4 = 1;
  
  /* Byte 0 */
  tmp = (uint8_t)((hsd->CID[0] & 0xFF000000) >> 24);
  pCardInfo->SD_cid.ManufacturerID = tmp;
  
  /* Byte 1 */
  tmp = (uint8_t)((hsd->CID[0] & 0x00FF0000) >> 16);
  pCardInfo->SD_cid.OEM_AppliID = tmp << 8;
  
  /* Byte 2 */
  tmp = (uint8_t)((hsd->CID[0] & 0x000000FF00) >> 8);
  pCardInfo->SD_cid.OEM_AppliID |= tmp;
  
  /* Byte 3 */
  tmp = (uint8_t)(hsd->CID[0] & 0x000000FF);
  pCardInfo->SD_cid.ProdName1 = tmp << 24;
  
  /* Byte 4 */
  tmp = (uint8_t)((hsd->CID[1] & 0xFF000000) >> 24);
  pCardInfo->SD_cid.ProdName1 |= tmp << 16;
  
  /* Byte 5 */
  tmp = (uint8_t)((hsd->CID[1] & 0x00FF0000) >> 16);
  pCardInfo->SD_cid.ProdName1 |= tmp << 8;
  
  /* Byte 6 */
  tmp = (uint8_t)((hsd->CID[1] & 0x0000FF00) >> 8);
  pCardInfo->SD_cid.ProdName1 |= tmp;
  
  /* Byte 7 */
  tmp = (uint8_t)(hsd->CID[1] & 0x000000FF);
  pCardInfo->SD_cid.ProdName2 = tmp;
  
  /* Byte 8 */
  tmp = (uint8_t)((hsd->CID[2] & 0xFF000000) >> 24);
  pCardInfo->SD_cid.ProdRev = tmp;
  
  /* Byte 9 */
  tmp = (uint8_t)((hsd->CID[2] & 0x00FF0000) >> 16);
  pCardInfo->SD_cid.ProdSN = tmp << 24;
  
  /* Byte 10 */
  tmp = (uint8_t)((hsd->CID[2] & 0x0000FF00) >> 8);
  pCardInfo->SD_cid.ProdSN |= tmp << 16;
  
  /* Byte 11 */
  tmp = (uint8_t)(hsd->CID[2] & 0x000000FF);
  pCardInfo->SD_cid.ProdSN |= tmp << 8;
  
  /* Byte 12 */
  tmp = (uint8_t)((hsd->CID[3] & 0xFF000000) >> 24);
  pCardInfo->SD_cid.ProdSN |= tmp;
  
  /* Byte 13 */
  tmp = (uint8_t)((hsd->CID[3] & 0x00FF0000) >> 16);
  pCardInfo->SD_cid.Reserved1   |= (tmp & 0xF0) >> 4;
  pCardInfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;
  
  /* Byte 14 */
  tmp = (uint8_t)((hsd->CID[3] & 0x0000FF00) >> 8);
  pCardInfo->SD_cid.ManufactDate |= tmp;
  
  /* Byte 15 */
  tmp = (uint8_t)(hsd->CID[3] & 0x000000FF);
  pCardInfo->SD_cid.CID_CRC   = (tmp & 0xFE) >> 1;
  pCardInfo->SD_cid.Reserved2 = 1;
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  SDIO_InitTypeDef Init;
  
  /* MMC Card does not support this feature */
  if (hsd->CardType == MULTIMEDIA_CARD)
  {
    errorState = SD_UNSUPPORTED_FEATURE;
    
    return errorState;
  }
  else if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
    (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    if (WideMode == SDIO_BUS_WIDE_8B)
    {
      errorState = SD_UNSUPPORTED_FEATURE;
      
      return errorState;
    }
    else if (WideMode == SDIO_BUS_WIDE_4B)
    {
      errorState = SD_WideBus_Enable(hsd);
      
      if (errorState == SD_OK)
      {
        /* Configure the SDIO peripheral */
        Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
        Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
        Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
        Init.BusWide             = SDIO_BUS_WIDE_4B;
        Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
        Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;
        
        /* Configure SDIO peripheral interface */
        SDIO_Init(hsd->Instance, Init);
      }
    }
    else
    {
      errorState = SD_WideBus_Disable(hsd);
      
      if (errorState == SD_OK)
      {
        /* Configure the SDIO peripheral */
        Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
        Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
        Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
        Init.BusWide             = SDIO_BUS_WIDE_1B;
        Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
        Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;
        
        /* Configure SDIO peripheral interface */
        SDIO_Init(hsd->Instance, Init);
      }
    }
  }
  
  return errorState;
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_StopTransfer(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  /* Send CMD12 STOP_TRANSMISSION  */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_STOP_TRANSMISSION;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_STOP_TRANSMISSION);
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  
  uint8_t SD_hs[64]  = {0};
  uint32_t SD_scr[2] = {0, 0};
  uint32_t SD_SPEC   = 0 ;
  uint32_t count = 0, *tempbuff = (uint32_t *)SD_hs;
  
  /* Initialize the Data control register */
  hsd->Instance->DCTRL = 0;
  
  /* Get SCR Register */
  errorState = SD_FindSCR(hsd, SD_scr);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Test the Version supported by the card*/ 
  SD_SPEC = (SD_scr[1]  & 0x01000000) | (SD_scr[1]  & 0x02000000);
  
  if (SD_SPEC != SD_ALLZERO)
  {
    /* Set Block Size for Card */
    SDIO_CmdInitStructure.Argument         = (uint32_t)64;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
    SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
    
    if (errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */
    SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.DataLength    = 64;
    SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_64B ;
    SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
    SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
    SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
    SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
    
    /* Send CMD6 switch mode */
    SDIO_CmdInitStructure.Argument         = 0x80FFFF01;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_HS_SWITCH;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure); 
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_HS_SWITCH);
    
    if (errorState != SD_OK)
    {
      return errorState;
    }
        
    while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
    {
      if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
      {
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDIO_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
      }
    }
    
    if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
      
      errorState = SD_DATA_TIMEOUT;
      
      return errorState;
    }
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
      
      errorState = SD_DATA_CRC_FAIL;
      
      return errorState;
    }
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
      
      errorState = SD_RX_OVERRUN;
      
      return errorState;
    }
    else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
    {
      __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
      
      errorState = SD_START_BIT_ERR;
      
      return errorState;
    }
    
    count = SD_DATATIMEOUT;
    
    while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0))
    {
      *tempbuff = SDIO_ReadFIFO(hsd->Instance);
      tempbuff++;
      count--;
    }
    
    /* Clear all the static flags */
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
    
    /* Test if the switch mode HS is ok */
    if ((SD_hs[13]& 2) != 2)
    {
      errorState = SD_UNSUPPORTED_FEATURE;
    } 
  }
  
  return errorState;
}

/**
  * @}
  */

/** @defgroup SD_Group4 Peripheral State functions 
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
  SDIO_CmdInitTypeDef  SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t count = 0;
  
  /* Check SD response */
  if ((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorState = SD_LOCK_UNLOCK_FAILED;
    
    return errorState;
  }
  
  /* Set block size for card if it is not equal to current block size for card */
  SDIO_CmdInitStructure.Argument         = 64;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Send CMD55 */
  SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = 64;
  SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_64B;
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  /* Send ACMD13 (SD_APP_STAUS)  with argument as card's RCA */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SD_APP_STAUS;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_STAUS);
  
  if (errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Get status data */
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
  {
    if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXFIFOHF))
    {
      for (count = 0; count < 8; count++)
      {
        *(pSDstatus + count) = SDIO_ReadFIFO(hsd->Instance);
      }
      
      pSDstatus += 8;
    }
  }
  
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DTIMEOUT);
    
    errorState = SD_DATA_TIMEOUT;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorState = SD_DATA_CRC_FAIL;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorState = SD_RX_OVERRUN;
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorState = SD_START_BIT_ERR;
    
    return errorState;
  }
  
  count = SD_DATATIMEOUT;
  while ((__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXDAVL)) && (count > 0))
  {
    *pSDstatus = SDIO_ReadFIFO(hsd->Instance);
    pSDstatus++;
    count--;
  }
  
  /* Clear all the static status flags*/
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t tmp = 0;
  uint32_t SD_STATUS[16];
  
  errorState = HAL_SD_SendSDStatus(hsd, SD_STATUS);
  
  if (errorState  != SD_OK)
  {
    return errorState;
  }
  
  /* Byte 0 */
  tmp = (SD_STATUS[0] & 0xC0) >> 6;
  pCardStatus->DAT_BUS_WIDTH = (uint8_t)tmp;
  
  /* Byte 0 */
  tmp = (SD_STATUS[0] & 0x20) >> 5;
  pCardStatus->SECURED_MODE = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (SD_STATUS[2] & 0xFF);
  pCardStatus->SD_CARD_TYPE = (uint8_t)(tmp << 8);
  
  /* Byte 3 */
  tmp = (SD_STATUS[3] & 0xFF);
  pCardStatus->SD_CARD_TYPE |= (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (SD_STATUS[4] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA = (uint8_t)(tmp << 24);
  
  /* Byte 5 */
  tmp = (SD_STATUS[5] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 16);
  
  /* Byte 6 */
  tmp = (SD_STATUS[6] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 8);
  
  /* Byte 7 */
  tmp = (SD_STATUS[7] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)tmp;
  
  /* Byte 8 */
  tmp = (SD_STATUS[8] & 0xFF);
  pCardStatus->SPEED_CLASS = (uint8_t)tmp;
  
  /* Byte 9 */
  tmp = (SD_STATUS[9] & 0xFF);
  pCardStatus->PERFORMANCE_MOVE = (uint8_t)tmp;
  
  /* Byte 10 */
  tmp = (SD_STATUS[10] & 0xF0) >> 4;
  pCardStatus->AU_SIZE = (uint8_t)tmp;
  
  /* Byte 11 */
  tmp = (SD_STATUS[11] & 0xFF);
  pCardStatus->ERASE_SIZE = (uint8_t)(tmp << 8);
  
  /* Byte 12 */
  tmp = (SD_STATUS[12] & 0xFF);
  pCardStatus->ERASE_SIZE |= (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (SD_STATUS[13] & 0xFC) >> 2;
  pCardStatus->ERASE_TIMEOUT = (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (SD_STATUS[13] & 0x3);
  pCardStatus->ERASE_OFFSET = (uint8_t)tmp;
  
  return errorState;
}
         
/**
  * @}
  */

/**
  * @brief  SD DMA transfer complete Rx callback.
  * @param  hdma: DMA handle 
  * @retval None
  */
static void SD_DMA_RxCplt(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = (SD_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* DMA transfer is complete */
  hsd->DmaTransferCplt = 1;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0)
  {
  }
  
  /* Transfer complete user callback */
  HAL_SD_DMA_RxCpltCallback(hsd->hdmarx);   
}

/**
  * @brief  SD DMA transfer Error Rx callback.
  * @param  hdma: DMA handle 
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
  * @param  hdma: DMA handle 
  * @retval None
  */
static void SD_DMA_TxCplt(DMA_HandleTypeDef *hdma)
{
  SD_HandleTypeDef *hsd = (SD_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* DMA transfer is complete */
  hsd->DmaTransferCplt = 1;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0)
  {
  }
  
  /* Transfer complete user callback */
  HAL_SD_DMA_TxCpltCallback(hsd->hdmatx);  
}

/**
  * @brief  SD DMA transfer Error Tx callback.
  * @param  hdma: DMA handle 
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
  uint32_t resp1 = 0;
  
  if (SD_SendStatus(hsd, &resp1) != SD_OK)
  {
    return SD_CARD_ERROR;
  }
  else
  {
    return (HAL_SD_CardStateTypedef)((resp1 >> 9) & 0x0F);
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
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure; 
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint16_t sd_rca = 1;
  
  if(SDIO_GetPowerState(hsd->Instance) == 0) /* Power off */
  {
    errorState = SD_REQUEST_NOT_APPLICABLE;
    
    return errorState;
  }
  
  if(hsd->CardType != SECURE_DIGITAL_IO_CARD)
  {
    /* Send CMD2 ALL_SEND_CID */
    SDIO_CmdInitStructure.Argument         = 0;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_LONG;
    SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
    SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp2Error(hsd);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Get Card identification number data */
    hsd->CID[0] = SDIO_GetResponse(SDIO_RESP1);
    hsd->CID[1] = SDIO_GetResponse(SDIO_RESP2);
    hsd->CID[2] = SDIO_GetResponse(SDIO_RESP3);
    hsd->CID[3] = SDIO_GetResponse(SDIO_RESP4);
  }
  
  if((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1)    || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
     (hsd->CardType == SECURE_DIGITAL_IO_COMBO_CARD) || (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 */
    /* SD Card publishes its RCA. */
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_REL_ADDR;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp6Error(hsd, SD_CMD_SET_REL_ADDR, &sd_rca);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
  }
  
  if (hsd->CardType != SECURE_DIGITAL_IO_CARD)
  {
    /* Get the SD card RCA */
    hsd->RCA = sd_rca;
    
    /* Send CMD9 SEND_CSD with argument as card's RCA */
    SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_LONG;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp2Error(hsd);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Get Card Specific Data */
    hsd->CSD[0] = SDIO_GetResponse(SDIO_RESP1);
    hsd->CSD[1] = SDIO_GetResponse(SDIO_RESP2);
    hsd->CSD[2] = SDIO_GetResponse(SDIO_RESP3);
    hsd->CSD[3] = SDIO_GetResponse(SDIO_RESP4);
  }
  
  /* All cards are initialized */
  return errorState;
}

/**
  * @brief  Selects od Deselects the corresponding card.
  * @param  hsd: SD handle
  * @param  addr: Address of the card to be selected  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_Select_Deselect(SD_HandleTypeDef *hsd, uint64_t addr)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  /* Send CMD7 SDIO_SEL_DESEL_CARD */
  SDIO_CmdInitStructure.Argument         = (uint32_t)addr;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SEL_DESEL_CARD;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SEL_DESEL_CARD);
  
  return errorState;
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
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure; 
  __IO HAL_SD_ErrorTypedef errorState = SD_OK; 
  uint32_t response = 0, count = 0, validvoltage = 0;
  uint32_t SDType = SD_STD_CAPACITY;
  
  /* Power ON Sequence -------------------------------------------------------*/
  /* Disable SDIO Clock */
  __HAL_SD_SDIO_DISABLE(); 
  
  /* Set Power State to ON */
  SDIO_PowerState_ON(hsd->Instance);
  
  /* Enable SDIO Clock */
  __HAL_SD_SDIO_ENABLE();
  
  /* CMD0: GO_IDLE_STATE -----------------------------------------------------*/
  /* No CMD response required */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_GO_IDLE_STATE;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_NO;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdError(hsd);
  
  if(errorState != SD_OK)
  {
    /* CMD Response TimeOut (wait for CMDSENT flag) */
    return errorState;
  }
  
  /* CMD8: SEND_IF_COND ------------------------------------------------------*/
  /* Send CMD8 to verify SD card interface operating condition */
  /* Argument: - [31:12]: Reserved (shall be set to '0')
  - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
  - [7:0]: Check Pattern (recommended 0xAA) */
  /* CMD Response: R7 */
  SDIO_CmdInitStructure.Argument         = SD_CHECK_PATTERN;
  SDIO_CmdInitStructure.CmdIndex         = SD_SDIO_SEND_IF_COND;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */ 
  errorState = SD_CmdResp7Error(hsd);
  
  if (errorState == SD_OK)
  {
    /* SD Card 2.0 */
    hsd->CardType = STD_CAPACITY_SD_CARD_V2_0; 
    SDType        = SD_HIGH_CAPACITY;
  }
  
  /* Send CMD55 */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  /* If errorState is Command TimeOut, it is a MMC card */
  /* If errorState is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  if(errorState == SD_OK)
  {
    /* SD CARD */
    /* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {
      
      /* SEND CMD55 APP_CMD with RCA as 0 */
      SDIO_CmdInitStructure.Argument         = 0;
      SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
      SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
      SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
      SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
      SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
      
      /* Check for error conditions */
      errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
      
      if(errorState != SD_OK)
      {
        return errorState;
      }
      
      /* Send CMD41 */
      SDIO_CmdInitStructure.Argument         = SD_VOLTAGE_WINDOW_SD | SDType;
      SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SD_APP_OP_COND;
      SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
      SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
      SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
      SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
      
      /* Check for error conditions */
      errorState = SD_CmdResp3Error(hsd);
      
      if(errorState != SD_OK)
      {
        return errorState;
      }
      
      /* Get command response */
      response = SDIO_GetResponse(SDIO_RESP1);
      
      /* Get operating voltage*/
      validvoltage = (((response >> 31) == 1) ? 1 : 0);
      
      count++;
    }
    
    if(count >= SD_MAX_VOLT_TRIAL)
    {
      errorState = SD_INVALID_VOLTRANGE;
      
      return errorState;
    }
    
    if((response & SD_HIGH_CAPACITY) == SD_HIGH_CAPACITY) /* (response &= SD_HIGH_CAPACITY) */
    {
      hsd->CardType = HIGH_CAPACITY_SD_CARD;
    }
    
  } /* else MMC Card */
  
  return errorState;
}

/**
  * @brief  Turns the SDIO output signals off.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_PowerOFF(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  /* Set Power State to OFF */
  SDIO_PowerState_OFF(hsd->Instance);
  
  return errorState;
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
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  if(pCardStatus == NULL)
  {
    errorState = SD_INVALID_PARAMETER;
    
    return errorState;
  }
  
  /* Send Status command */
  SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SEND_STATUS);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Get SD card status */
  *pCardStatus = SDIO_GetResponse(SDIO_RESP1);
  
  return errorState;
}

/**
  * @brief  Checks for error conditions for CMD0.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdError(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t timeout, tmp;
  
  timeout = SDIO_CMD0TIMEOUT;
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDSENT);
    
  while((timeout > 0) && (!tmp))
  {
    tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDSENT);
    timeout--;
  }
  
  if(timeout == 0)
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorState;
}

/**
  * @brief  Checks for error conditions for R7 response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp7Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorState = SD_ERROR;
  uint32_t timeout = SDIO_CMD0TIMEOUT, tmp;
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT); 
  
  while((!tmp) && (timeout > 0))
  {
    tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT);
    timeout--;
  }
  
  tmp = __HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT); 
  
  if((timeout == 0) || tmp)
  {
    /* Card is not V2.0 compliant or card does not support the set voltage range */
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CMDREND))
  {
    /* Card is SD V2.0 compliant */
    errorState = SD_OK;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CMDREND);
    
    return errorState;
  }
  
  return errorState;
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  hsd: SD handle
  * @param  SD_CMD: The sent command index  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp1Error(SD_HandleTypeDef *hsd, uint8_t SD_CMD)
{
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t response_R1;
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorState = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorState;
  }
  
  /* Check response received is of desired command */
  if(SDIO_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorState = SD_ILLEGAL_CMD;
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* We have received response, retrieve it for analysis  */
  response_R1 = SDIO_GetResponse(SDIO_RESP1);
  
  if((response_R1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return errorState;
  }
  
  if((response_R1 & SD_OCR_ADDR_OUT_OF_RANGE) == SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }
  
  if((response_R1 & SD_OCR_ADDR_MISALIGNED) == SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }
  
  if((response_R1 & SD_OCR_BLOCK_LEN_ERR) == SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }
  
  if((response_R1 & SD_OCR_ERASE_SEQ_ERR) == SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }
  
  if((response_R1 & SD_OCR_BAD_ERASE_PARAM) == SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }
  
  if((response_R1 & SD_OCR_WRITE_PROT_VIOLATION) == SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }
  
  if((response_R1 & SD_OCR_LOCK_UNLOCK_FAILED) == SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }
  
  if((response_R1 & SD_OCR_COM_CRC_FAILED) == SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  if((response_R1 & SD_OCR_ILLEGAL_CMD) == SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }
  
  if((response_R1 & SD_OCR_CARD_ECC_FAILED) == SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }
  
  if((response_R1 & SD_OCR_CC_ERROR) == SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }
  
  if((response_R1 & SD_OCR_GENERAL_UNKNOWN_ERROR) == SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }
  
  if((response_R1 & SD_OCR_STREAM_READ_UNDERRUN) == SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }
  
  if((response_R1 & SD_OCR_STREAM_WRITE_OVERRUN) == SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }
  
  if((response_R1 & SD_OCR_CID_CSD_OVERWRIETE) == SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }
  
  if((response_R1 & SD_OCR_WP_ERASE_SKIP) == SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }
  
  if((response_R1 & SD_OCR_CARD_ECC_DISABLED) == SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }
  
  if((response_R1 & SD_OCR_ERASE_RESET) == SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }
  
  if((response_R1 & SD_OCR_AKE_SEQ_ERROR) == SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }
  
  return errorState;
}

/**
  * @brief  Checks for error conditions for R3 (OCR) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp3Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  while (!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorState;
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_CmdResp2Error(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  while (!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
    
  if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  else if (__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorState = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  return errorState;
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
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t response_R1;
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorState = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorState;
  }
  
  /* Check response received is of desired command */
  if(SDIO_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorState = SD_ILLEGAL_CMD;
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  /* We have received response, retrieve it.  */
  response_R1 = SDIO_GetResponse(SDIO_RESP1);
  
  if((response_R1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)) == SD_ALLZERO)
  {
    *pRCA = (uint16_t) (response_R1 >> 16);
    
    return errorState;
  }
  
  if((response_R1 & SD_R6_GENERAL_UNKNOWN_ERROR) == SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }
  
  if((response_R1 & SD_R6_ILLEGAL_CMD) == SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }
  
  if((response_R1 & SD_R6_COM_CRC_FAILED) == SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  return errorState;
}

/**
  * @brief  Enables the SDIO wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Enable(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  uint32_t scr[2] = {0, 0};
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorState = SD_LOCK_UNLOCK_FAILED;
    
    return errorState;
  }
  
  /* Get SCR Register */
  errorState = SD_FindSCR(hsd, scr);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  
  /* If requested card supports wide bus operation */
  if((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
    SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    SDIO_CmdInitStructure.Argument         = 2;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_SD_SET_BUSWIDTH);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    return errorState;
  }
  else
  {
    errorState = SD_REQUEST_NOT_APPLICABLE;
    
    return errorState;
  }
}   

/**
  * @brief  Disables the SDIO wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Disable(SD_HandleTypeDef *hsd)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  
  uint32_t scr[2] = {0, 0};
  
  if((SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorState = SD_LOCK_UNLOCK_FAILED;
    
    return errorState;
  }
  
  /* Get SCR Register */
  errorState = SD_FindSCR(hsd, scr);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  
  /* If requested card supports 1 bit mode operation */
  if((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA */
    SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
    SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
    SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    /* Send ACMD6 APP_CMD with argument as 0 for single bus mode */
    SDIO_CmdInitStructure.Argument         = 0;
    SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
    
    /* Check for error conditions */
    errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_SD_SET_BUSWIDTH);
    
    if(errorState != SD_OK)
    {
      return errorState;
    }
    
    return errorState;
  }
  else
  {
    errorState = SD_REQUEST_NOT_APPLICABLE;
    
    return errorState;
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
  SDIO_CmdInitTypeDef  SDIO_CmdInitStructure;
  SDIO_DataInitTypeDef SDIO_DataInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  uint32_t index = 0;
  uint32_t tempscr[2] = {0, 0};
  
  /* Set Block Size To 8 Bytes */
  /* Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.Argument         = (uint32_t)8;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  
  /* Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.Argument         = (uint32_t)((hsd->RCA) << 16);
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_APP_CMD;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  SDIO_DataInitStructure.DataTimeOut   = SD_DATATIMEOUT;
  SDIO_DataInitStructure.DataLength    = 8;
  SDIO_DataInitStructure.DataBlockSize = SDIO_DATABLOCK_SIZE_8B;
  SDIO_DataInitStructure.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
  SDIO_DataInitStructure.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
  SDIO_DataInitStructure.DPSM          = SDIO_DPSM_ENABLE;
  SDIO_DataConfig(hsd->Instance, &SDIO_DataInitStructure);
  
  /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  SDIO_CmdInitStructure.Argument         = 0;
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SD_APP_SEND_SCR;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  /* Check for error conditions */
  errorState = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_SEND_SCR);
  
  if(errorState != SD_OK)
  {
    return errorState;
  }
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR))
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
    
    errorState = SD_DATA_TIMEOUT;
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_DCRCFAIL);
    
    errorState = SD_DATA_CRC_FAIL;
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_RXOVERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_RXOVERR);
    
    errorState = SD_RX_OVERRUN;
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_STBITERR))
  {
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_STBITERR);
    
    errorState = SD_START_BIT_ERR;
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  *(pSCR + 1) = ((tempscr[0] & SD_0TO7BITS) << 24)  | ((tempscr[0] & SD_8TO15BITS) << 8) |\
    ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
  
  *(pSCR) = ((tempscr[1] & SD_0TO7BITS) << 24)  | ((tempscr[1] & SD_8TO15BITS) << 8) |\
    ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
  
  return errorState;
}

/**
  * @brief  Checks if the SD card is in programming state.
  * @param  hsd: SD handle
  * @param  pStatus: pointer to the variable that will contain the SD card state  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_IsCardProgramming(SD_HandleTypeDef *hsd, uint8_t *pStatus)
{
  SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
  HAL_SD_ErrorTypedef errorState = SD_OK;
  __IO uint32_t responseR1 = 0;
  
  SDIO_CmdInitStructure.Argument         = (uint32_t)(hsd->RCA << 16);
  SDIO_CmdInitStructure.CmdIndex         = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.Response         = SDIO_RESPONSE_SHORT;
  SDIO_CmdInitStructure.WaitForInterrupt = SDIO_WAIT_NO;
  SDIO_CmdInitStructure.CPSM             = SDIO_CPSM_ENABLE;
  SDIO_SendCommand(hsd->Instance, &SDIO_CmdInitStructure);
  
  while(!__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CTIMEOUT))
  {
    errorState = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CTIMEOUT);
    
    return errorState;
  }
  else if(__HAL_SD_SDIO_GET_FLAG(hsd, SDIO_FLAG_CCRCFAIL))
  {
    errorState = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_FLAG_CCRCFAIL);
    
    return errorState;
  }
  
  /* Check response received is of desired command */
  if((uint32_t)SDIO_GetCommandResponse(hsd->Instance) != SD_CMD_SEND_STATUS)
  {
    errorState = SD_ILLEGAL_CMD;
    
    return errorState;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDIO_CLEAR_FLAG(hsd, SDIO_STATIC_FLAGS);
  
  
  /* We have received response, retrieve it for analysis */
  responseR1 = SDIO_GetResponse(SDIO_RESP1);
  
  /* Find out card status */
  *pStatus = (uint8_t)((responseR1 >> 9) & 0x0000000F);
  
  if((responseR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return errorState;
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
  
  if((responseR1 & SD_OCR_CID_CSD_OVERWRIETE) == SD_OCR_CID_CSD_OVERWRIETE)
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
  
  return errorState;
}   

/**
  * @}
  */

#endif /* HAL_SD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
