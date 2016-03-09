/**
  ******************************************************************************
  * @file    stm32l4xx_hal_nor.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   NOR HAL module driver.
  *          This file provides a generic firmware to drive NOR memories mounted 
  *          as external device.
  *         
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================       
    [..]
      This driver is a generic layered driver which contains a set of APIs used to 
      control NOR flash memories. It uses the FMC layer functions to interface 
      with NOR devices. This driver is used as follows:
    
      (+) NOR flash memory configuration sequence using the function HAL_NOR_Init() 
          with control and timing parameters for both normal and extended mode.
            
      (+) Read NOR flash memory manufacturer code and device IDs using the function
          HAL_NOR_Read_ID(). The read information is stored in the NOR_ID_TypeDef 
          structure declared by the function caller. 
        
      (+) Access NOR flash memory by read/write data unit operations using the functions
          HAL_NOR_Read(), HAL_NOR_Program().
        
      (+) Perform NOR flash erase block/chip operations using the functions 
          HAL_NOR_Erase_Block() and HAL_NOR_Erase_Chip().
        
      (+) Read the NOR flash CFI (common flash interface) IDs using the function
          HAL_NOR_Read_CFI(). The read information is stored in the NOR_CFI_TypeDef
          structure declared by the function caller.
        
      (+) You can also control the NOR device by calling the control APIs HAL_NOR_WriteOperation_Enable()/
          HAL_NOR_WriteOperation_Disable() to respectively enable/disable the NOR write operation  
       
      (+) You can monitor the NOR device HAL state by calling the function
          HAL_NOR_GetState() 
    [..]
     (@) This driver is a set of generic APIs which handle standard NOR flash operations.
         If a NOR flash device contains different operations and/or implementations, 
         it should be implemented separately.

     *** NOR HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in NOR HAL driver.
       
      (+) NOR_WRITE : NOR memory write data to specified address

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

#ifdef HAL_NOR_MODULE_ENABLED

/** @defgroup NOR NOR
  * @brief NOR HAL module driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup NOR_Private_Constants NOR Private Constants
  * @{
  */

/* Constants to define address to set to write a command */
#define NOR_CMD_ADDRESS_FIRST                 (uint16_t)0x0555
#define NOR_CMD_ADDRESS_FIRST_CFI             (uint16_t)0x0055
#define NOR_CMD_ADDRESS_SECOND                (uint16_t)0x02AA
#define NOR_CMD_ADDRESS_THIRD                 (uint16_t)0x0555
#define NOR_CMD_ADDRESS_FOURTH                (uint16_t)0x0555
#define NOR_CMD_ADDRESS_FIFTH                 (uint16_t)0x02AA
#define NOR_CMD_ADDRESS_SIXTH                 (uint16_t)0x0555

/* Constants to define data to program a command */
#define NOR_CMD_DATA_READ_RESET               (uint16_t)0x00F0
#define NOR_CMD_DATA_FIRST                    (uint16_t)0x00AA
#define NOR_CMD_DATA_SECOND                   (uint16_t)0x0055
#define NOR_CMD_DATA_AUTO_SELECT              (uint16_t)0x0090
#define NOR_CMD_DATA_PROGRAM                  (uint16_t)0x00A0
#define NOR_CMD_DATA_CHIP_BLOCK_ERASE_THIRD   (uint16_t)0x0080
#define NOR_CMD_DATA_CHIP_BLOCK_ERASE_FOURTH  (uint16_t)0x00AA
#define NOR_CMD_DATA_CHIP_BLOCK_ERASE_FIFTH   (uint16_t)0x0055
#define NOR_CMD_DATA_CHIP_ERASE               (uint16_t)0x0010
#define NOR_CMD_DATA_CFI                      (uint16_t)0x0098

#define NOR_CMD_DATA_BUFFER_AND_PROG          (uint8_t)0x25
#define NOR_CMD_DATA_BUFFER_AND_PROG_CONFIRM  (uint8_t)0x29
#define NOR_CMD_DATA_BLOCK_ERASE              (uint8_t)0x30

/* Mask on NOR STATUS REGISTER */
#define NOR_MASK_STATUS_DQ5                   (uint16_t)0x0020
#define NOR_MASK_STATUS_DQ6                   (uint16_t)0x0040

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup NOR_Private_Macros NOR Private Macros
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/

/** @defgroup NOR_Private_Variables NOR Private Variables
  * @{
  */

static uint32_t uwNORMemoryDataWidth  = NOR_MEMORY_8B;

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup NOR_Exported_Functions NOR Exported Functions
  * @{
  */

/** @defgroup NOR_Exported_Functions_Group1 Initialization and de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
           ##### NOR Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize
    the NOR memory
  
@endverbatim
  * @{
  */
    
/**
  * @brief  Perform the NOR memory Initialization sequence.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  Timing: pointer to NOR control timing structure 
  * @param  ExtTiming: pointer to NOR extended mode timing structure    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Init(NOR_HandleTypeDef *hnor, FMC_NORSRAM_TimingTypeDef *Timing, FMC_NORSRAM_TimingTypeDef *ExtTiming)
{
  /* Check the NOR handle parameter */
  if(hnor == NULL)
  {
     return HAL_ERROR;
  }
  
  if(hnor->State == HAL_NOR_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hnor->Lock = HAL_UNLOCKED;

    /* Initialize the low level hardware (MSP) */
    HAL_NOR_MspInit(hnor);
  }

  /* Initialize NOR control Interface */
  FMC_NORSRAM_Init(hnor->Instance, &(hnor->Init));

  /* Initialize NOR timing Interface */
  FMC_NORSRAM_Timing_Init(hnor->Instance, Timing, hnor->Init.NSBank); 

  /* Initialize NOR extended mode timing Interface */
  FMC_NORSRAM_Extended_Timing_Init(hnor->Extended, ExtTiming, hnor->Init.NSBank, hnor->Init.ExtendedMode);

  /* Enable the NORSRAM device */
  __FMC_NORSRAM_ENABLE(hnor->Instance, hnor->Init.NSBank);  

  /* Initialize NOR Memory Data Width*/
  if (hnor->Init.MemoryDataWidth == FMC_NORSRAM_MEM_BUS_WIDTH_8)
  {
    uwNORMemoryDataWidth = NOR_MEMORY_8B;
  }
  else
  {
    uwNORMemoryDataWidth = NOR_MEMORY_16B;
  }

  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY; 
  
  return HAL_OK;
}

/**
  * @brief  Perform NOR memory De-Initialization sequence.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_DeInit(NOR_HandleTypeDef *hnor)  
{
  /* De-Initialize the low level hardware (MSP) */
  HAL_NOR_MspDeInit(hnor);
 
  /* Configure the NOR registers with their reset values */
  FMC_NORSRAM_DeInit(hnor->Instance, hnor->Extended, hnor->Init.NSBank);
  
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hnor);

  return HAL_OK;
}

/**
  * @brief  Initialize the NOR MSP.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval None
  */
__weak void HAL_NOR_MspInit(NOR_HandleTypeDef *hnor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnor);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NOR_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitialize the NOR MSP.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval None
  */
__weak void HAL_NOR_MspDeInit(NOR_HandleTypeDef *hnor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnor);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NOR_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @brief  NOR MSP Wait for Ready/Busy signal.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  Timeout: Maximum timeout value
  * @retval None
  */
__weak void HAL_NOR_MspWait(NOR_HandleTypeDef *hnor, uint32_t Timeout)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnor);
  UNUSED(Timeout);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NOR_MspWait could be implemented in the user file
   */ 
}
  
/**
  * @}
  */

/** @defgroup NOR_Exported_Functions_Group2 Input and Output functions 
  * @brief    Input Output and memory control functions 
  *
  @verbatim    
  ==============================================================================
                ##### NOR Input and Output functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to use and control the NOR memory
  
@endverbatim
  * @{
  */
  
/**
  * @brief  Read NOR flash IDs.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  pNOR_ID : pointer to NOR ID structure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Read_ID(NOR_HandleTypeDef *hnor, NOR_IDTypeDef *pNOR_ID)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }  
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send read ID command */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_THIRD), NOR_CMD_DATA_AUTO_SELECT);

  /* Read the NOR IDs */
  pNOR_ID->Manufacturer_Code = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, MC_ADDRESS);
  pNOR_ID->Device_Code1      = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, DEVICE_CODE1_ADDR);
  pNOR_ID->Device_Code2      = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, DEVICE_CODE2_ADDR);
  pNOR_ID->Device_Code3      = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, DEVICE_CODE3_ADDR);
  
  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);   
  
  return HAL_OK;
}

/**
  * @brief  Return the NOR memory to Read mode.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_ReturnToReadMode(NOR_HandleTypeDef *hnor)
{
  uint32_t deviceaddress = 0;  
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }  
  
  NOR_WRITE(deviceaddress, NOR_CMD_DATA_READ_RESET);

  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);   
  
  return HAL_OK;
}

/**
  * @brief  Read data from NOR memory.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  pAddress: pointer to Device address
  * @param  pData : pointer to read data  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Read(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  } 
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send read data command */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST); 
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);  
  NOR_WRITE((uint32_t)pAddress, NOR_CMD_DATA_READ_RESET);

  /* Read the data */
  *pData = *(__IO uint32_t *)(uint32_t)pAddress;
  
  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;  
}

/**
  * @brief  Program data to NOR memory.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  pAddress: Device address
  * @param  pData : pointer to the data to write   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Program(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  } 
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send program data command */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_THIRD), NOR_CMD_DATA_PROGRAM);

  /* Write the data */
  NOR_WRITE(pAddress, *pData);
  
  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;  
}

/**
  * @brief  Read a block of data from the FMC NOR memory.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  uwAddress: NOR memory internal address to read from.
  * @param  pData: pointer to the buffer that receives the data read from the 
  *         NOR memory.
  * @param  uwBufferSize : number of Half word to read.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_ReadBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }  
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send read data command */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST); 
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);  
  NOR_WRITE(uwAddress, NOR_CMD_DATA_READ_RESET);
  
  /* Read buffer */
  while( uwBufferSize > 0) 
  {
    *pData++ = *(__IO uint16_t *)uwAddress;
    uwAddress += 2;
    uwBufferSize--;
  } 
  
  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;  
}

/**
  * @brief  Write a half-word buffer to the FMC NOR memory. This function 
  *         must be used only with S29GL128P NOR memory. 
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  uwAddress: NOR memory internal address from which the data 
  * @note   Some NOR memory need Address aligned to xx bytes (can be aligned to 
  *          64 bytes boundary for example).
  * @param  pData: pointer to source data buffer. 
  * @param  uwBufferSize: number of Half words to write. 
  * @note   The maximum buffer size allowed is NOR memory dependent
  *         (can be 64 Bytes max for example).
  * @retval HAL status
  */ 
HAL_StatusTypeDef HAL_NOR_ProgramBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize)
{
  uint16_t * p_currentaddress = (uint16_t *)NULL;
  uint16_t * p_endaddress = (uint16_t *)NULL;
  uint32_t lastloadedaddress = 0, deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }  
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Initialize variables */
  p_currentaddress  = (uint16_t*)((uint32_t)(uwAddress));
  p_endaddress      = p_currentaddress + (uwBufferSize-1);
  lastloadedaddress = (uint32_t)(uwAddress);

  /* Issue unlock command sequence */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND); 

  /* Write Buffer Load Command */
  NOR_WRITE((uint32_t)(p_currentaddress), NOR_CMD_DATA_BUFFER_AND_PROG); 
  NOR_WRITE((uint32_t)(p_currentaddress), (uwBufferSize-1)); 

  /* Load Data into NOR Buffer */
  while(p_currentaddress <= p_endaddress)
  {
    /* Store last loaded address & data value (for polling) */
    lastloadedaddress = (uint32_t)p_currentaddress;
 
    NOR_WRITE(p_currentaddress, *pData++);
    
    p_currentaddress++;
  }

  NOR_WRITE((uint32_t)(lastloadedaddress), NOR_CMD_DATA_BUFFER_AND_PROG_CONFIRM); 
  
  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK; 
  
}

/**
  * @brief  Erase the specified block of the NOR memory.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  BlockAddress : Block to erase address 
  * @param  Address: Device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Erase_Block(NOR_HandleTypeDef *hnor, uint32_t BlockAddress, uint32_t Address)
{
  uint32_t deviceaddress = 0;

  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send block erase command sequence */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_THIRD), NOR_CMD_DATA_CHIP_BLOCK_ERASE_THIRD);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FOURTH), NOR_CMD_DATA_CHIP_BLOCK_ERASE_FOURTH);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIFTH), NOR_CMD_DATA_CHIP_BLOCK_ERASE_FIFTH);
  NOR_WRITE((uint32_t)(BlockAddress + Address), NOR_CMD_DATA_BLOCK_ERASE);

  /* Check the NOR memory status and update the controller state */
  hnor->State = HAL_NOR_STATE_READY;
    
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;
 
}

/**
  * @brief  Erase the entire NOR chip.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  Address : Device address  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Erase_Chip(NOR_HandleTypeDef *hnor, uint32_t Address)
{
  uint32_t deviceaddress = 0;
  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Address);

  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;  
    
  /* Send NOR chip erase command sequence */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST), NOR_CMD_DATA_FIRST);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SECOND), NOR_CMD_DATA_SECOND);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_THIRD), NOR_CMD_DATA_CHIP_BLOCK_ERASE_THIRD);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FOURTH), NOR_CMD_DATA_CHIP_BLOCK_ERASE_FOURTH);
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIFTH), NOR_CMD_DATA_CHIP_BLOCK_ERASE_FIFTH);  
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_SIXTH), NOR_CMD_DATA_CHIP_ERASE);
  
  /* Check the NOR memory status and update the controller state */
  hnor->State = HAL_NOR_STATE_READY;
    
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;  
}

/**
  * @brief  Read NOR flash CFI IDs.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @param  pNOR_CFI : pointer to NOR CFI IDs structure  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_Read_CFI(NOR_HandleTypeDef *hnor, NOR_CFITypeDef *pNOR_CFI)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnor);
  
  /* Check the NOR controller state */
  if(hnor->State == HAL_NOR_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Select the NOR device address */
  if (hnor->Init.NSBank == FMC_NORSRAM_BANK1)
  {
    deviceaddress = NOR_MEMORY_ADRESS1;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK2)
  {
    deviceaddress = NOR_MEMORY_ADRESS2;
  }
  else if (hnor->Init.NSBank == FMC_NORSRAM_BANK3)
  {
    deviceaddress = NOR_MEMORY_ADRESS3;
  }
  else /* FMC_NORSRAM_BANK4 */
  {
    deviceaddress = NOR_MEMORY_ADRESS4;
  }  
    
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
  
  /* Send read CFI query command */
  NOR_WRITE(NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, NOR_CMD_ADDRESS_FIRST_CFI), NOR_CMD_DATA_CFI);

  /* read the NOR CFI information */
  pNOR_CFI->CFI_1 = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, CFI1_ADDRESS);
  pNOR_CFI->CFI_2 = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, CFI2_ADDRESS);
  pNOR_CFI->CFI_3 = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, CFI3_ADDRESS);
  pNOR_CFI->CFI_4 = *(__IO uint16_t *) NOR_ADDR_SHIFT(deviceaddress, uwNORMemoryDataWidth, CFI4_ADDRESS);

  /* Check the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor);
  
  return HAL_OK;
}

/**
  * @}
  */
  
/** @defgroup NOR_Exported_Functions_Group3 Peripheral Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                        ##### NOR Control functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the NOR interface.

@endverbatim
  * @{
  */
    
/**
  * @brief  Enable dynamically NOR write operation.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_WriteOperation_Enable(NOR_HandleTypeDef *hnor)
{
  /* Process Locked */
  __HAL_LOCK(hnor);

  /* Enable write operation */
  FMC_NORSRAM_WriteOperation_Enable(hnor->Instance, hnor->Init.NSBank); 
  
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor); 
  
  return HAL_OK;  
}

/**
  * @brief  Disable dynamically NOR write operation.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NOR_WriteOperation_Disable(NOR_HandleTypeDef *hnor)
{
  /* Process Locked */
  __HAL_LOCK(hnor);

  /* Update the SRAM controller state */
  hnor->State = HAL_NOR_STATE_BUSY;
    
  /* Disable write operation */
  FMC_NORSRAM_WriteOperation_Disable(hnor->Instance, hnor->Init.NSBank); 
  
  /* Update the NOR controller state */
  hnor->State = HAL_NOR_STATE_PROTECTED;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnor); 
  
  return HAL_OK;  
}

/**
  * @}
  */  
  
/** @defgroup NOR_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                      ##### NOR State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in run-time the status of the NOR controller 
    and the data flow.

@endverbatim
  * @{
  */
  
/**
  * @brief  Return the NOR controller handle state.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.
  * @retval NOR controller state
  */
HAL_NOR_StateTypeDef HAL_NOR_GetState(NOR_HandleTypeDef *hnor)
{
  /* Return NOR handle state */
  return hnor->State;
}

/**
  * @brief  Return the NOR operation status.
  * @param  hnor: pointer to a NOR_HandleTypeDef structure that contains
  *                the configuration information for NOR module.   
  * @param  Address: Device address
  * @param  Timeout: NOR programming Timeout
  * @retval NOR_Status: The returned value can be: HAL_NOR_STATUS_SUCCESS, HAL_NOR_STATUS_ERROR
  *         or HAL_NOR_STATUS_TIMEOUT
  */
HAL_NOR_StatusTypeDef HAL_NOR_GetStatus(NOR_HandleTypeDef *hnor, uint32_t Address, uint32_t Timeout)
{ 
  HAL_NOR_StatusTypeDef status = HAL_NOR_STATUS_ONGOING;
  uint16_t tmp_sr1 = 0, tmp_sr2 = 0;
  uint32_t tickstart = 0;

  /* Poll on NOR memory Ready/Busy signal ------------------------------------*/
  HAL_NOR_MspWait(hnor, Timeout);
  
  /* Get tick */
  tickstart = HAL_GetTick();
  while((status != HAL_NOR_STATUS_SUCCESS) && (status != HAL_NOR_STATUS_TIMEOUT))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        status = HAL_NOR_STATUS_TIMEOUT; 
      } 
    } 

    /* Read NOR status register (DQ6 and DQ5) */
    tmp_sr1 = *(__IO uint16_t *)Address;
    tmp_sr2 = *(__IO uint16_t *)Address;

    /* If DQ6 did not toggle between the two reads then return NOR_Success */
    if((tmp_sr1 & NOR_MASK_STATUS_DQ6) == (tmp_sr2 & NOR_MASK_STATUS_DQ6)) 
    {
      return HAL_NOR_STATUS_SUCCESS;
    }
    
    if((tmp_sr1 & NOR_MASK_STATUS_DQ5) != NOR_MASK_STATUS_DQ5)
    {
      status = HAL_NOR_STATUS_ONGOING;
    }
    
    tmp_sr1 = *(__IO uint16_t *)Address;
    tmp_sr2 = *(__IO uint16_t *)Address;

    /* If DQ6 did not toggle between the two reads then return NOR_Success */
    if((tmp_sr1 & NOR_MASK_STATUS_DQ6) == (tmp_sr2 & NOR_MASK_STATUS_DQ6)) 
    {
      return HAL_NOR_STATUS_SUCCESS;
    }
    else if((tmp_sr1 & NOR_MASK_STATUS_DQ5) == NOR_MASK_STATUS_DQ5)
    {
      return HAL_NOR_STATUS_ERROR;
    } 
  }

  /* Return the operation status */
  return status;
}

/**
  * @}
  */

/**
  * @}
  */
/**
  * @}
  */
#endif /* HAL_NOR_MODULE_ENABLED */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
