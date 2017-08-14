/**
  ******************************************************************************
  * @file    stm32l4xx_hal_sd.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
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
    this memory. The needed STM32 hardware resources (SDMMC1 and GPIO) are performed by 
    the user in HAL_SD_MspInit() function (MSP layer).                             
    Basically, the MSP layer configuration should be the same as we provide in the 
    examples.
    You can easily tailor this configuration according to hardware resources.

  [..]
    This driver is a generic layered driver for SDMMC memories which uses the HAL 
    SDMMC driver functions to interface with SD and uSD cards devices. 
    It is used as follows:
 
    (#)Initialize the SDMMC1 low level resources by implementing the HAL_SD_MspInit() API:
        (##) Call the function HAL_RCCEx_PeriphCLKConfig with RCC_PERIPHCLK_SDMMC1 for
        PeriphClockSelection and select SDMMC1 clock source (MSI, main PLL or PLLSAI1)
        (##) Enable the SDMMC1 interface clock using __HAL_RCC_SDMMC1_CLK_ENABLE(); 
        (##) SDMMC pins configuration for SD card
            (+++) Enable the clock for the SDMMC GPIOs using the functions __HAL_RCC_GPIOx_CLK_ENABLE();   
            (+++) Configure these SDMMC pins as alternate function pull-up using HAL_GPIO_Init()
                  and according to your pin assignment;
        (##) DMA Configuration if you need to use DMA process (HAL_SD_ReadBlocks_DMA()
             and HAL_SD_WriteBlocks_DMA() APIs).
            (+++) Enable the DMAx interface clock using __HAL_RCC_DMAx_CLK_ENABLE(); 
            (+++) Configure the DMA using the function HAL_DMA_Init() with predeclared and filled. 
        (##) NVIC configuration if you need to use interrupt process when using DMA transfer.
            (+++) Configure the SDMMC and DMA interrupt priorities using functions
                  HAL_NVIC_SetPriority(); DMA priority is superior to SDMMC's priority
            (+++) Enable the NVIC DMA and SDMMC IRQs using function HAL_NVIC_EnableIRQ()
            (+++) SDMMC interrupts are managed using the macros __HAL_SD_SDMMC_ENABLE_IT() 
                  and __HAL_SD_SDMMC_DISABLE_IT() inside the communication process.
            (+++) SDMMC interrupts pending bits are managed using the macros __HAL_SD_SDMMC_GET_IT()
                  and __HAL_SD_SDMMC_CLEAR_IT()
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
        The SD Card frequency (SDMMC_CK) is computed as follows:
     (++)

           SDMMC_CK = SDMMCCLK / (ClockDiv + 2)

     -@@-  In initialization mode and according to the SD Card standard, 
          make sure that the SDMMC_CK frequency doesn't exceed 400KHz.
  
    (#) Get the SD CID and CSD data. All these information are managed by the SDCardInfo 
        structure. This structure provide also ready computed SD Card capacity 
        and Block size.
        
        -@- These information are stored in SD handle structure in case of future use.  
  
    (#) Configure the SD Card Data transfer frequency. By Default, the card transfer 
        frequency is set to 24MHz. You can change or adapt this frequency by adjusting 
        the "ClockDiv" field.
        In transfer mode and according to the SD Card standard, make sure that the 
        SDMMC_CK frequency doesn't exceed 25MHz and 50MHz in High-speed mode switch.
        To be able to use a frequency higher than 24MHz, you should use the SDMMC 
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
       
    (+) __HAL_SD_SDMMC_ENABLE : Enable the SD device
    (+) __HAL_SD_SDMMC_DISABLE : Disable the SD device
    (+) __HAL_SD_SDMMC_DMA_ENABLE: Enable the SDMMC DMA transfer
    (+) __HAL_SD_SDMMC_DMA_DISABLE: Disable the SDMMC DMA transfer
    (+) __HAL_SD_SDMMC_ENABLE_IT: Enable the SD device interrupt
    (+) __HAL_SD_SDMMC_DISABLE_IT: Disable the SD device interrupt
    (+) __HAL_SD_SDMMC_GET_FLAG:Check whether the specified SD flag is set or not
    (+) __HAL_SD_SDMMC_CLEAR_FLAG: Clear the SD's pending flags
    [..]  
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
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup SD 
  * @{
  */

#ifdef HAL_SD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup SD_Private_Defines
  * @{
  */
/** 
  * @brief  SDMMC Data block size 
  */ 
#define DATA_BLOCK_SIZE                  ((uint32_t)(9 << 4))
/** 
  * @brief  SDMMC Static flags, Timeout, FIFO Address  
  */
#define SDMMC_STATIC_FLAGS               ((uint32_t)(SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_CTIMEOUT |\
                                                    SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_RXOVERR  |\
                                                    SDMMC_FLAG_CMDREND  | SDMMC_FLAG_CMDSENT  | SDMMC_FLAG_DATAEND  |\
                                                    SDMMC_FLAG_DBCKEND))  

#define SDMMC_CMD0TIMEOUT                ((uint32_t)0x00010000)

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
#define SD_OCR_CID_CSD_OVERWRITE        ((uint32_t)0x00010000)
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
  *         SDMMC_APP_CMD should be sent before sending these commands. 
  */
#define SD_SDMMC_SEND_IF_COND            ((uint32_t)SD_CMD_HS_SEND_EXT_CSD)
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
            SD_HandleTypeDef and initialize the associated handle.
  * @param  hsd: SD handle
  * @param  SDCardInfo: HAL_SD_CardInfoTypedef structure for SD card information   
  * @retval HAL SD error state
  */
HAL_SD_ErrorTypedef HAL_SD_Init(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypedef *SDCardInfo)
{ 
  __IO HAL_SD_ErrorTypedef errorstate = SD_OK;
  SD_InitTypeDef tmpinit;
  
  /* Initialize the low level hardware (MSP) */
  HAL_SD_MspInit(hsd);
  
  /* Default SDMMC peripheral configuration for SD card initialization */
  tmpinit.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  tmpinit.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  tmpinit.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  tmpinit.BusWide             = SDMMC_BUS_WIDE_1B;
  tmpinit.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  tmpinit.ClockDiv            = SDMMC_INIT_CLK_DIV;
  
  /* Initialize SDMMC peripheral interface with default configuration */
  SDMMC_Init(hsd->Instance, tmpinit);
  
  /* Identify card operating voltage */
  errorstate = SD_PowerON(hsd); 
  
  if(errorstate != SD_OK)     
  {
    return errorstate;
  }
  
  /* Initialize the present SDMMC card(s) and put them in idle state */
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
    errorstate = SD_Select_Deselect(hsd, (uint32_t)(((uint32_t)SDCardInfo->RCA) << 16));
  }
  
  /* Configure SDMMC peripheral interface */
  SDMMC_Init(hsd->Instance, hsd->Init);   
  
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

  /* NOTE : This function should not be modified, when the callback is needed,
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

  /* NOTE : This function should not be modified, when the callback is needed,
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
  * @param  BlockNumber: Block number from where data is to be read (byte address = BlockNumber * BlockSize)
  * @param  BlockSize: SD card Data block size 
  *   @note BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of SD blocks to read   
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_BlockNumber(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDMMC_CmdInitTypeDef  sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t count = 0, *tempbuff = (uint32_t *)pReadBuffer;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  uint32_t ReadAddr;
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr = BlockNumber;
  }
  else
  {
    // should not overflow for standard-capacity cards
    ReadAddr = BlockNumber * BlockSize;
  }
  
  /* Set Block Size for Card */ 
  sdmmc_cmdinitstructure.Argument         = (uint32_t) BlockSize;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = NumberOfBlocks * BlockSize;
  sdmmc_datainitstructure.DataBlockSize = DATA_BLOCK_SIZE;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
  if(NumberOfBlocks > 1)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;    
  }
  
  sdmmc_cmdinitstructure.Argument         = ReadAddr;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Read block(s) in polling mode */
  if(NumberOfBlocks > 1)
  {
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_READ_MULT_BLOCK);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Poll on SDMMC flags */
    while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXFIFOHF))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDMMC_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
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
    while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
    {
      if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXFIFOHF))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDMMC_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
      }
    }   
  }
  
  /* Send stop transmission command in case of multiblock read */
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DATAEND) && (NumberOfBlocks > 1))
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
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  count = SD_DATATIMEOUT;
  
  /* Empty FIFO if there is still any data */
  while ((__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXDAVL)) && (count > 0))
  {
    *tempbuff = SDMMC_ReadFIFO(hsd->Instance);
    tempbuff++;
    count--;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
  return errorstate;
}

/**
  * @brief  Allows to write block(s) to a specified address in a card. The Data
  *         transfer is managed by polling mode.  
  * @param  hsd: SD handle
  * @param  pWriteBuffer: pointer to the buffer that will contain the data to transmit
  * @param  BlockNumber: Block number to where data is to be written (byte address = BlockNumber * BlockSize) 
  * @param  BlockSize: SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of SD blocks to write 
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_BlockNumber(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t totalnumberofbytes = 0, bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)pWriteBuffer;
  uint8_t cardstate  = 0;
  
  /* Initialize data control register */
  hsd->Instance->DCTRL = 0;
  
  uint32_t WriteAddr;
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr = BlockNumber;
  }
  else
  {
    // should not overflow for standard-capacity cards
    WriteAddr = BlockNumber * BlockSize;
  }
  
  /* Set Block Size for Card */ 
  sdmmc_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  if(NumberOfBlocks > 1)
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  else
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  
  sdmmc_cmdinitstructure.Argument         = WriteAddr;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1)
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
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = NumberOfBlocks * BlockSize;
  sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
  /* Write block(s) in polling mode */
  if(NumberOfBlocks > 1)
  {
    while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXFIFOHE))
      {
        if ((totalnumberofbytes - bytestransferred) < 32)
        {
          restwords = ((totalnumberofbytes - bytestransferred) % 4 == 0) ? ((totalnumberofbytes - bytestransferred) / 4) : (( totalnumberofbytes -  bytestransferred) / 4 + 1);
          
          /* Write data to SDMMC Tx FIFO */
          for (count = 0; count < restwords; count++)
          {
            SDMMC_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++;
            bytestransferred += 4;
          }
        }
        else
        {
          /* Write data to SDMMC Tx FIFO */
          for (count = 0; count < 8; count++)
          {
            SDMMC_WriteFIFO(hsd->Instance, (tempbuff + count));
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
    while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
    {
      if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXFIFOHE))
      {
        if ((totalnumberofbytes - bytestransferred) < 32)
        {
          restwords = ((totalnumberofbytes - bytestransferred) % 4 == 0) ? ((totalnumberofbytes - bytestransferred) / 4) : (( totalnumberofbytes -  bytestransferred) / 4 + 1);
          
          /* Write data to SDMMC Tx FIFO */
          for (count = 0; count < restwords; count++)
          {
            SDMMC_WriteFIFO(hsd->Instance, tempbuff);
            tempbuff++; 
            bytestransferred += 4;
          }
        }
        else
        {
          /* Write data to SDMMC Tx FIFO */
          for (count = 0; count < 8; count++)
          {
            SDMMC_WriteFIFO(hsd->Instance, (tempbuff + count));
          }
          
          tempbuff += 8;
          bytestransferred += 32;
        }
      }
    }  
  }
  
  /* Send stop transmission command in case of multiblock write */
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DATAEND) && (NumberOfBlocks > 1))
  {    
    if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
      (hsd->CardType == HIGH_CAPACITY_SD_CARD))
    {
      /* Send stop transmission command */
      errorstate = HAL_SD_StopTransfer(hsd);
    }
  }
  
  /* Get error state */
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXUNDERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_TXUNDERR);
    
    errorstate = SD_TX_UNDERRUN;
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  * @param  BlockNumber: Block number from where data is to be read (byte address = BlockNumber * BlockSize)
  * @param  BlockSize: SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_BlockNumber_DMA(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
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
  __HAL_SD_SDMMC_ENABLE_IT(hsd, (SDMMC_IT_DCRCFAIL |\
                                 SDMMC_IT_DTIMEOUT |\
                                 SDMMC_IT_DATAEND  |\
                                 SDMMC_IT_RXOVERR));
  
  /* Enable SDMMC DMA transfer */
  __HAL_SD_SDMMC_DMA_ENABLE(hsd);
  
  /* Configure DMA user callbacks */
  hsd->hdmarx->XferCpltCallback  = SD_DMA_RxCplt;
  hsd->hdmarx->XferErrorCallback = SD_DMA_RxError;

  /* Change DMA direction Periph to Memory */
  hsd->hdmarx->Init.Direction = DMA_PERIPH_TO_MEMORY;
  hsd->hdmarx->Instance->CCR &= ~DMA_MEMORY_TO_PERIPH;
  
  /* Enable the DMA Channel */
  HAL_DMA_Start_IT(hsd->hdmarx, (uint32_t)&hsd->Instance->FIFO, (uint32_t)pReadBuffer, (uint32_t)(BlockSize * NumberOfBlocks)/4);
  
  uint32_t ReadAddr;
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr = BlockNumber;
  }
  else
  {
    // should not overflow for standard-capacity cards
    ReadAddr = BlockNumber * BlockSize;
  }
  
  /* Set Block Size for Card */ 
  sdmmc_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = BlockSize * NumberOfBlocks;
  sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
  /* Check number of blocks command */
  if(NumberOfBlocks > 1)
  {
    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_READ_MULT_BLOCK;
  }
  else
  {
    /* Send CMD17 READ_SINGLE_BLOCK */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
  }
  
  sdmmc_cmdinitstructure.Argument         = ReadAddr;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  if(NumberOfBlocks > 1)
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
  * @param  @param  BlockNumber: Block number to where data is to be written (byte address = BlockNumber * BlockSize)
  * @param  BlockSize: the SD card Data block size 
  * @note   BlockSize must be 512 bytes.
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_BlockNumber_DMA(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
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
  __HAL_SD_SDMMC_ENABLE_IT(hsd, (SDMMC_IT_DCRCFAIL |\
                                 SDMMC_IT_DTIMEOUT |\
                                 SDMMC_IT_DATAEND  |\
                                 SDMMC_IT_TXUNDERR)); 
  
  /* Configure DMA user callbacks */
  hsd->hdmatx->XferCpltCallback  = SD_DMA_TxCplt;
  hsd->hdmatx->XferErrorCallback = SD_DMA_TxError;

  /* Change DMA direction Memory to Periph */
  hsd->hdmatx->Init.Direction = DMA_MEMORY_TO_PERIPH;
  hsd->hdmatx->Instance->CCR |= DMA_MEMORY_TO_PERIPH;
  
  /* Enable the DMA Channel */
  HAL_DMA_Start_IT(hsd->hdmatx, (uint32_t)pWriteBuffer, (uint32_t)&hsd->Instance->FIFO, (uint32_t)(BlockSize * NumberOfBlocks)/4);

  /* Enable SDMMC DMA transfer */
  __HAL_SD_SDMMC_DMA_ENABLE(hsd);
  
  uint32_t WriteAddr;
  if (hsd->CardType == HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr = BlockNumber;
  }
  else
  {
    // should not overflow for standard-capacity cards
    WriteAddr = BlockNumber * BlockSize;
  }

  /* Set Block Size for Card */ 
  sdmmc_cmdinitstructure.Argument         = (uint32_t)BlockSize;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);

  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);

  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Check number of blocks command */
  if(NumberOfBlocks <= 1)
  {
    /* Send CMD24 WRITE_SINGLE_BLOCK */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  }
  else
  {
    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    sdmmc_cmdinitstructure.CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  }
  
  sdmmc_cmdinitstructure.Argument         = (uint32_t)WriteAddr;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);

  /* Check for error conditions */
  if(NumberOfBlocks > 1)
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
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = BlockSize * NumberOfBlocks;
  sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
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
    
  while (((tmp1 & tmp2) == 0) && (tmp3 == SD_OK) && (timeout > 0))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;    
    timeout--;
  }

  timeout = Timeout;
  
  /* Wait until the Rx transfer is no longer active */
  while((__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXACT)) && (timeout > 0))
  {
    timeout--;  
  }
  
  /* Send stop command in multiblock read */
  if (hsd->SdOperation == SD_READ_MULTIPLE_BLOCK)
  {
    errorstate = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0) && (errorstate == SD_OK))
  {
    errorstate = SD_DATA_TIMEOUT;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
    
  while (((tmp1 & tmp2) == 0) && (tmp3 == SD_OK) && (timeout > 0))
  {
    tmp1 = hsd->DmaTransferCplt; 
    tmp2 = hsd->SdTransferCplt;
    tmp3 = (HAL_SD_ErrorTypedef)hsd->SdTransferErr;
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Wait until the Tx transfer is no longer active */
  while((__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_TXACT))  && (timeout > 0))
  {
    timeout--;  
  }

  /* Send stop command in multiblock write */
  if (hsd->SdOperation == SD_WRITE_MULTIPLE_BLOCK)
  {
    errorstate = HAL_SD_StopTransfer(hsd);
  }
  
  if ((timeout == 0) && (errorstate == SD_OK))
  {
    errorstate = SD_DATA_TIMEOUT;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  
  uint32_t delay         = 0;
  __IO uint32_t maxdelay = 0;
  uint8_t cardstate      = 0;
  
  /* Check if the card command class supports erase command */
  if (((hsd->CSD[1] >> 20) & SD_CCCC_ERASE) == 0)
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
  
  /* Get max delay value */
  maxdelay = 120000 / (((hsd->Instance->CLKCR) & 0xFF) + 2);
  
  if((SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
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
    sdmmc_cmdinitstructure.Argument         =(uint32_t)startaddr;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_START;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
    sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
    sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_START);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    sdmmc_cmdinitstructure.Argument         = (uint32_t)endaddr;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SD_ERASE_GRP_END;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_ERASE_GRP_END);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
  }
  
  /* Send CMD38 ERASE */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_ERASE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
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
  
  while ((delay > 0) && (errorstate == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
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
  /* Check for SDMMC interrupt flags */
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_IT_DATAEND))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_IT_DATAEND);  
      
    /* SD transfer is complete */
    hsd->SdTransferCplt = 1;

    /* No transfer error */ 
    hsd->SdTransferErr  = SD_OK;

    HAL_SD_XferCpltCallback(hsd);  
  }  
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_IT_DCRCFAIL))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
    
    hsd->SdTransferErr = SD_DATA_CRC_FAIL;
    
    HAL_SD_XferErrorCallback(hsd);
    
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_IT_DTIMEOUT))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
    
    hsd->SdTransferErr = SD_DATA_TIMEOUT;
    
    HAL_SD_XferErrorCallback(hsd);
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_IT_RXOVERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_RXOVERR);
    
    hsd->SdTransferErr = SD_RX_OVERRUN;
    
    HAL_SD_XferErrorCallback(hsd);
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_IT_TXUNDERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_TXUNDERR);
    
    hsd->SdTransferErr = SD_TX_UNDERRUN;
    
    HAL_SD_XferErrorCallback(hsd);
  }
  else
  {
    /* No error flag set */
  }  

  /* Disable all SDMMC peripheral interrupt sources */
  __HAL_SD_SDMMC_DISABLE_IT(hsd, SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_DATAEND  |\
                                 SDMMC_IT_TXFIFOHE | SDMMC_IT_RXFIFOHF | SDMMC_IT_TXUNDERR |\
                                 SDMMC_IT_RXOVERR);                               
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

  /* NOTE : This function should not be modified, when the callback is needed,
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

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_XferErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Rx callback in non-blocking mode.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_RxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);

  /* NOTE : This function should not be modified, when the callback is needed,
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

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SD_DMA_RxErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  SD Transfer complete Tx callback in non-blocking mode.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SD_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);

  /* NOTE : This function should not be modified, when the callback is needed,
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

  /* NOTE : This function should not be modified, when the callback is needed,
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
    
    pCardInfo->CardCapacity = (uint64_t)(((uint64_t)pCardInfo->SD_csd.DeviceSize + 1) * 512 * 1024);
    pCardInfo->CardBlockSize = 512;    
  }
  else
  {
    /* Not supported card type */
    errorstate = SD_ERROR;
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
  
  return errorstate;
}

/**
  * @brief  Enables wide bus operation for the requested card if supported by 
  *         card.
  * @param  hsd: SD handle       
  * @param  WideMode: Specifies the SD card wide bus mode 
  *          This parameter can be one of the following values:
  *            @arg SDMMC_BUS_WIDE_8B: 8-bit data transfer (Only for MMC)
  *            @arg SDMMC_BUS_WIDE_4B: 4-bit data transfer
  *            @arg SDMMC_BUS_WIDE_1B: 1-bit data transfer
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_WideBusOperation_Config(SD_HandleTypeDef *hsd, uint32_t WideMode)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  SDMMC_InitTypeDef tmpinit;
  
  /* MMC Card does not support this feature */
  if (hsd->CardType == MULTIMEDIA_CARD)
  {
    errorstate = SD_UNSUPPORTED_FEATURE;
    
    return errorstate;
  }
  else if ((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1) || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
    (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    if (WideMode == SDMMC_BUS_WIDE_8B)
    {
      errorstate = SD_UNSUPPORTED_FEATURE;
    }
    else if (WideMode == SDMMC_BUS_WIDE_4B)
    {
      errorstate = SD_WideBus_Enable(hsd);
    }
    else if (WideMode == SDMMC_BUS_WIDE_1B)
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
      /* Configure the SDMMC peripheral */
      tmpinit.ClockEdge           = hsd->Init.ClockEdge;
      tmpinit.ClockBypass         = hsd->Init.ClockBypass;
      tmpinit.ClockPowerSave      = hsd->Init.ClockPowerSave;
      tmpinit.BusWide             = WideMode;
      tmpinit.HardwareFlowControl = hsd->Init.HardwareFlowControl;
      tmpinit.ClockDiv            = hsd->Init.ClockDiv;
      SDMMC_Init(hsd->Instance, tmpinit);
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Send CMD12 STOP_TRANSMISSION  */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_STOP_TRANSMISSION;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_STOP_TRANSMISSION);
  
  return errorstate;
}

/**
  * @brief  Switches the SD card to High Speed mode.
  *         This API must be used after "Transfer State"
  * @note   This operation should be followed by the configuration 
  *         of PLL to have SDMMCCK clock between 67 and 75 MHz
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
HAL_SD_ErrorTypedef HAL_SD_HighSpeed (SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  
  uint8_t SD_hs[64]  = {0};
  uint32_t SD_scr[2] = {0, 0};
  uint32_t SD_SPEC   = 0 ;
  uint32_t count = 0, *tempbuff = (uint32_t *)SD_hs;
  
  /* Initialize the Data control register */
  hsd->Instance->DCTRL = 0;
  
  /* Get SCR Register */
  errorstate = SD_FindSCR(hsd, SD_scr);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Test the Version supported by the card*/ 
  SD_SPEC = (SD_scr[1]  & 0x01000000) | (SD_scr[1]  & 0x02000000);
  
  if (SD_SPEC != SD_ALLZERO)
  {
    /* Set Block Size for Card */
    sdmmc_cmdinitstructure.Argument         = (uint32_t)64;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
    sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
    sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Configure the SD DPSM (Data Path State Machine) */
    sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
    sdmmc_datainitstructure.DataLength    = 64;
    sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_64B ;
    sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
    SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
    
    /* Send CMD6 switch mode */
    sdmmc_cmdinitstructure.Argument         = 0x80FFFF01;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_HS_SWITCH;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure); 
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_HS_SWITCH);
    
    if (errorstate != SD_OK)
    {
      return errorstate;
    }
        
    while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
    {
      if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXFIFOHF))
      {
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDMMC_ReadFIFO(hsd->Instance);
        }
        
        tempbuff += 8;
      }
    }
    
    if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DTIMEOUT))
    {
      __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
      
      errorstate = SD_DATA_TIMEOUT;
      
      return errorstate;
    }
    else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DCRCFAIL))
    {
      __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
      
      errorstate = SD_DATA_CRC_FAIL;
      
      return errorstate;
    }
    else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR))
    {
      __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_RXOVERR);
      
      errorstate = SD_RX_OVERRUN;
      
      return errorstate;
    }
    else
    {
      /* No error flag set */
    }
        
    count = SD_DATATIMEOUT;
    
    while ((__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXDAVL)) && (count > 0))
    {
      *tempbuff = SDMMC_ReadFIFO(hsd->Instance);
      tempbuff++;
      count--;
    }
    
    /* Clear all the static flags */
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
    
    /* Test if the switch mode HS is ok */
    if ((SD_hs[13]& 2) != 2)
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
  SDMMC_CmdInitTypeDef  sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t count = 0;
  
  /* Check SD response */
  if ((SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
  {
    errorstate = SD_LOCK_UNLOCK_FAILED;
    
    return errorstate;
  }
  
  /* Set block size for card if it is not equal to current block size for card */
  sdmmc_cmdinitstructure.Argument         = 64;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Send CMD55 */
  sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Configure the SD DPSM (Data Path State Machine) */ 
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = 64;
  sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_64B;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
  /* Send ACMD13 (SD_APP_STAUS)  with argument as card's RCA */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_STATUS;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_STATUS);
  
  if (errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Get status data */
  while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
  {
    if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXFIFOHF))
    {
      for (count = 0; count < 8; count++)
      {
        *(pSDstatus + count) = SDMMC_ReadFIFO(hsd->Instance);
      }
      
      pSDstatus += 8;
    }
  }
  
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }  
  
  count = SD_DATATIMEOUT;
  while ((__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXDAVL)) && (count > 0))
  {
    *pSDstatus = SDMMC_ReadFIFO(hsd->Instance);
    pSDstatus++;
    count--;
  }
  
  /* Clear all the static status flags*/
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  uint32_t tmp = 0;
  uint32_t sd_status[16];
  
  errorstate = HAL_SD_SendSDStatus(hsd, sd_status);
  
  if (errorstate  != SD_OK)
  {
    return errorstate;
  }
  
  /* Byte 0 */
  tmp = (sd_status[0] & 0xC0) >> 6;
  pCardStatus->DAT_BUS_WIDTH = (uint8_t)tmp;
  
  /* Byte 0 */
  tmp = (sd_status[0] & 0x20) >> 5;
  pCardStatus->SECURED_MODE = (uint8_t)tmp;
  
  /* Byte 2 */
  tmp = (sd_status[2] & 0xFF);
  pCardStatus->SD_CARD_TYPE = (uint8_t)(tmp << 8);
  
  /* Byte 3 */
  tmp = (sd_status[3] & 0xFF);
  pCardStatus->SD_CARD_TYPE |= (uint8_t)tmp;
  
  /* Byte 4 */
  tmp = (sd_status[4] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA = (uint8_t)(tmp << 24);
  
  /* Byte 5 */
  tmp = (sd_status[5] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 16);
  
  /* Byte 6 */
  tmp = (sd_status[6] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)(tmp << 8);
  
  /* Byte 7 */
  tmp = (sd_status[7] & 0xFF);
  pCardStatus->SIZE_OF_PROTECTED_AREA |= (uint8_t)tmp;
  
  /* Byte 8 */
  tmp = (sd_status[8] & 0xFF);
  pCardStatus->SPEED_CLASS = (uint8_t)tmp;
  
  /* Byte 9 */
  tmp = (sd_status[9] & 0xFF);
  pCardStatus->PERFORMANCE_MOVE = (uint8_t)tmp;
  
  /* Byte 10 */
  tmp = (sd_status[10] & 0xF0) >> 4;
  pCardStatus->AU_SIZE = (uint8_t)tmp;
  
  /* Byte 11 */
  tmp = (sd_status[11] & 0xFF);
  pCardStatus->ERASE_SIZE = (uint8_t)(tmp << 8);
  
  /* Byte 12 */
  tmp = (sd_status[12] & 0xFF);
  pCardStatus->ERASE_SIZE |= (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (sd_status[13] & 0xFC) >> 2;
  pCardStatus->ERASE_TIMEOUT = (uint8_t)tmp;
  
  /* Byte 13 */
  tmp = (sd_status[13] & 0x3);
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
  hsd->DmaTransferCplt = 1;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0)
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
  hsd->DmaTransferCplt = 1;
  
  /* Wait until SD transfer is complete */
  while(hsd->SdTransferCplt == 0)
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure; 
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint16_t sd_rca = 1;
  
  if(SDMMC_GetPowerState(hsd->Instance) == 0) /* Power off */
  {
    errorstate = SD_REQUEST_NOT_APPLICABLE;
    
    return errorstate;
  }
  
  if(hsd->CardType != SECURE_DIGITAL_IO_CARD)
  {
    /* Send CMD2 ALL_SEND_CID */
    sdmmc_cmdinitstructure.Argument         = 0;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_ALL_SEND_CID;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_LONG;
    sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
    sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp2Error(hsd);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Get Card identification number data */
    hsd->CID[0] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
    hsd->CID[1] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP2);
    hsd->CID[2] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP3);
    hsd->CID[3] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP4);
  }
  
  if((hsd->CardType == STD_CAPACITY_SD_CARD_V1_1)    || (hsd->CardType == STD_CAPACITY_SD_CARD_V2_0) ||\
     (hsd->CardType == SECURE_DIGITAL_IO_COMBO_CARD) || (hsd->CardType == HIGH_CAPACITY_SD_CARD))
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 */
    /* SD Card publishes its RCA. */
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_REL_ADDR;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
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
    sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SEND_CSD;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_LONG;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp2Error(hsd);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Get Card Specific Data */
    hsd->CSD[0] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
    hsd->CSD[1] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP2);
    hsd->CSD[2] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP3);
    hsd->CSD[3] = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP4);
  }
  
  /* All cards are initialized */
  return errorstate;
}

/**
  * @brief  Selects or Deselects the corresponding card.
  * @param  hsd: SD handle
  * @param  addr: Address of the card to be selected  
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_Select_Deselect(SD_HandleTypeDef *hsd, uint64_t addr)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Send CMD7 SDMMC_SEL_DESEL_CARD */
  sdmmc_cmdinitstructure.Argument         = (uint32_t)addr;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SEL_DESEL_CARD;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure; 
  __IO HAL_SD_ErrorTypedef errorstate = SD_OK; 
  uint32_t response = 0, count = 0, validvoltage = 0;
  uint32_t sdtype = SD_STD_CAPACITY;
  
  /* Power ON Sequence -------------------------------------------------------*/
  /* Disable SDMMC Clock */
  __HAL_SD_SDMMC_DISABLE(hsd); 
  
  /* Set Power State to ON */
  SDMMC_PowerState_ON(hsd->Instance);
  
  /* 1ms: required power up waiting time before starting the SD initialization 
     sequence */
  HAL_Delay(1);
  
  /* Enable SDMMC Clock */
  __HAL_SD_SDMMC_ENABLE(hsd);
  
  /* CMD0: GO_IDLE_STATE -----------------------------------------------------*/
  /* No CMD response required */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_GO_IDLE_STATE;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_NO;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
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
  sdmmc_cmdinitstructure.Argument         = SD_CHECK_PATTERN;
  sdmmc_cmdinitstructure.CmdIndex         = SD_SDMMC_SEND_IF_COND;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */ 
  errorstate = SD_CmdResp7Error(hsd);
  
  if (errorstate == SD_OK)
  {
    /* SD Card 2.0 */
    hsd->CardType = STD_CAPACITY_SD_CARD_V2_0; 
    sdtype        = SD_HIGH_CAPACITY;
  }
  
  /* Send CMD55 */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
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
      sdmmc_cmdinitstructure.Argument         = 0;
      sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
      sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
      sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
      sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
      SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
      
      /* Check for error conditions */
      errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
      
      if(errorstate != SD_OK)
      {
        return errorstate;
      }
      
      /* Send CMD41 */
      sdmmc_cmdinitstructure.Argument         = SD_VOLTAGE_WINDOW_SD | sdtype;
      sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_OP_COND;
      sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
      sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
      sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
      SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
      
      /* Check for error conditions */
      errorstate = SD_CmdResp3Error(hsd);
      
      if(errorstate != SD_OK)
      {
        return errorstate;
      }
      
      /* Get command response */
      response = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
      
      /* Get operating voltage*/
      validvoltage = (((response >> 31) == 1) ? 1 : 0);
      
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
  * @brief  Turns the SDMMC output signals off.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_PowerOFF(SD_HandleTypeDef *hsd)
{
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  /* Set Power State to OFF */
  SDMMC_PowerState_OFF(hsd->Instance);
  
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  if(pCardStatus == NULL)
  {
    errorstate = SD_INVALID_PARAMETER;
    
    return errorstate;
  }
  
  /* Send Status command */
  sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SEND_STATUS;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SEND_STATUS);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Get SD card status */
  *pCardStatus = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
  
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
  
  timeout = SDMMC_CMD0TIMEOUT;
  
  tmp = __HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CMDSENT);
    
  while((timeout > 0) && (!tmp))
  {
    tmp = __HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CMDSENT);
    timeout--;
  }
  
  if(timeout == 0)
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  uint32_t timeout = SDMMC_CMD0TIMEOUT, tmp;
  
  tmp = __HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT); 
  
  while((!tmp) && (timeout > 0))
  {
    tmp = __HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT);
    timeout--;
  }
  
  tmp = __HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT); 
  
  if((timeout == 0) || tmp)
  {
    /* Card is not V2.0 compliant or card does not support the set voltage range */
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  
  if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CMDREND))
  {
    /* Card is SD V2.0 compliant */
    errorstate = SD_OK;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CMDREND);
    
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
  
  while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  
  /* Check response received is of desired command */
  if(SDMMC_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
  /* We have received response, retrieve it for analysis  */
  response_r1 = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
  
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
  
  while (!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT))
  {
  }
  
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  
  while (!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT))
  {
  }
    
  if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if (__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }  
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
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
  
  while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }  
  
  /* Check response received is of desired command */
  if(SDMMC_GetCommandResponse(hsd->Instance) != SD_CMD)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
  /* We have received response, retrieve it.  */
  response_r1 = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
  
  if((response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)) == SD_ALLZERO)
  {
    *pRCA = (uint16_t) (response_r1 >> 16);
    
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
  * @brief  Enables the SDMMC wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Enable(SD_HandleTypeDef *hsd)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  uint32_t scr[2] = {0, 0};
  
  if((SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
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
  if((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
    sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
    sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    sdmmc_cmdinitstructure.Argument         = 2;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
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
  * @brief  Disables the SDMMC wide bus mode.
  * @param  hsd: SD handle
  * @retval SD Card error state
  */
static HAL_SD_ErrorTypedef SD_WideBus_Disable(SD_HandleTypeDef *hsd)
{
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  
  uint32_t scr[2] = {0, 0};
  
  if((SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1) & SD_CARD_LOCKED) == SD_CARD_LOCKED)
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
  if((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
  {
    /* Send CMD55 APP_CMD with argument as card's RCA */
    sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
    sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
    sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
    sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
    /* Check for error conditions */
    errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
    
    if(errorstate != SD_OK)
    {
      return errorstate;
    }
    
    /* Send ACMD6 APP_CMD with argument as 0 for single bus mode */
    sdmmc_cmdinitstructure.Argument         = 0;
    sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
    
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
  SDMMC_CmdInitTypeDef  sdmmc_cmdinitstructure;
  SDMMC_DataInitTypeDef sdmmc_datainitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  uint32_t index = 0;
  uint32_t tempscr[2] = {0, 0};
  
  /* Set Block Size To 8 Bytes */
  /* Send CMD55 APP_CMD with argument as card's RCA */
  sdmmc_cmdinitstructure.Argument         = (uint32_t)8;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SET_BLOCKLEN;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SET_BLOCKLEN);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  /* Send CMD55 APP_CMD with argument as card's RCA */
  sdmmc_cmdinitstructure.Argument         = (uint32_t)((hsd->RCA) << 16);
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_APP_CMD;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_APP_CMD);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  sdmmc_datainitstructure.DataTimeOut   = SD_DATATIMEOUT;
  sdmmc_datainitstructure.DataLength    = 8;
  sdmmc_datainitstructure.DataBlockSize = SDMMC_DATABLOCK_SIZE_8B;
  sdmmc_datainitstructure.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
  sdmmc_datainitstructure.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  sdmmc_datainitstructure.DPSM          = SDMMC_DPSM_ENABLE;
  SDMMC_DataConfig(hsd->Instance, &sdmmc_datainitstructure);
  
  /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  sdmmc_cmdinitstructure.Argument         = 0;
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SD_APP_SEND_SCR;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  /* Check for error conditions */
  errorstate = SD_CmdResp1Error(hsd, SD_CMD_SD_APP_SEND_SCR);
  
  if(errorstate != SD_OK)
  {
    return errorstate;
  }
  
  while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
  {
    if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXDAVL))
    {
      *(tempscr + index) = SDMMC_ReadFIFO(hsd->Instance);
      index++;
    }
  }
  
  if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DTIMEOUT))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DTIMEOUT);
    
    errorstate = SD_DATA_TIMEOUT;
    
    return errorstate;
  }
  else if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_DCRCFAIL))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_DCRCFAIL);
    
    errorstate = SD_DATA_CRC_FAIL;
    
    return errorstate;
  }
  else if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_RXOVERR))
  {
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_RXOVERR);
    
    errorstate = SD_RX_OVERRUN;
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
  *(pSCR + 1) = ((tempscr[0] & SD_0TO7BITS) << 24)  | ((tempscr[0] & SD_8TO15BITS) << 8) |\
    ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
  
  *(pSCR) = ((tempscr[1] & SD_0TO7BITS) << 24)  | ((tempscr[1] & SD_8TO15BITS) << 8) |\
    ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
  
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
  SDMMC_CmdInitTypeDef sdmmc_cmdinitstructure;
  HAL_SD_ErrorTypedef errorstate = SD_OK;
  __IO uint32_t responseR1 = 0;
  
  sdmmc_cmdinitstructure.Argument         = (uint32_t)(hsd->RCA << 16);
  sdmmc_cmdinitstructure.CmdIndex         = SD_CMD_SEND_STATUS;
  sdmmc_cmdinitstructure.Response         = SDMMC_RESPONSE_SHORT;
  sdmmc_cmdinitstructure.WaitForInterrupt = SDMMC_WAIT_NO;
  sdmmc_cmdinitstructure.CPSM             = SDMMC_CPSM_ENABLE;
  SDMMC_SendCommand(hsd->Instance, &sdmmc_cmdinitstructure);
  
  while(!__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CMDREND | SDMMC_FLAG_CTIMEOUT))
  {
  }
  
  if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CTIMEOUT))
  {
    errorstate = SD_CMD_RSP_TIMEOUT;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CTIMEOUT);
    
    return errorstate;
  }
  else if(__HAL_SD_SDMMC_GET_FLAG(hsd, SDMMC_FLAG_CCRCFAIL))
  {
    errorstate = SD_CMD_CRC_FAIL;
    
    __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_FLAG_CCRCFAIL);
    
    return errorstate;
  }
  else
  {
    /* No error flag set */
  }
  
  /* Check response received is of desired command */
  if((uint32_t)SDMMC_GetCommandResponse(hsd->Instance) != SD_CMD_SEND_STATUS)
  {
    errorstate = SD_ILLEGAL_CMD;
    
    return errorstate;
  }
  
  /* Clear all the static flags */
  __HAL_SD_SDMMC_CLEAR_FLAG(hsd, SDMMC_STATIC_FLAGS);
  
  
  /* We have received response, retrieve it for analysis */
  responseR1 = SDMMC_GetResponse(hsd->Instance, SDMMC_RESP1);
  
  /* Find out card status */
  *pStatus = (uint8_t)((responseR1 >> 9) & 0x0000000F);
  
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

#endif /* HAL_SD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
