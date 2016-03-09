/**
  ******************************************************************************
  * @file    stm32l4xx_hal_nand.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   NAND HAL module driver.
  *          This file provides a generic firmware to drive NAND memories mounted 
  *          as external device.
  *         
  @verbatim
  ==============================================================================
                         ##### How to use this driver #####
  ==============================================================================    
    [..]
      This driver is a generic layered driver which contains a set of APIs used to 
      control NAND flash memories. It uses the FMC layer functions to interface 
      with NAND devices. This driver is used as follows:
    
      (+) NAND flash memory configuration sequence using the function HAL_NAND_Init() 
          with control and timing parameters for both common and attribute spaces.
            
      (+) Read NAND flash memory maker and device IDs using the function
          HAL_NAND_Read_ID(). The read information is stored in the NAND_ID_TypeDef 
          structure declared by the function caller. 
        
      (+) Access NAND flash memory by read/write operations using the functions
          HAL_NAND_Read_Page()/HAL_NAND_Read_SpareArea(), HAL_NAND_Write_Page()/HAL_NAND_Write_SpareArea()
          to read/write page(s)/spare area(s). These functions use specific device 
          information (Block, page size..) predefined by the user in the HAL_NAND_Info_TypeDef 
          structure. The read/write address information is contained by the Nand_Address_Typedef
          structure passed as parameter.
        
      (+) Perform NAND flash Reset chip operation using the function HAL_NAND_Reset().
        
      (+) Perform NAND flash erase block operation using the function HAL_NAND_Erase_Block().
          The erase block address information is contained in the Nand_Address_Typedef 
          structure passed as parameter.
    
      (+) Read the NAND flash status operation using the function HAL_NAND_Read_Status().
        
      (+) You can also control the NAND device by calling the control APIs HAL_NAND_ECC_Enable()/
          HAL_NAND_ECC_Disable() to respectively enable/disable the ECC code correction
          feature or the function HAL_NAND_GetECC() to get the ECC correction code. 
       
      (+) You can monitor the NAND device HAL state by calling the function
          HAL_NAND_GetState()  

    [..]
      (@) This driver is a set of generic APIs which handle standard NAND flash operations.
          If a NAND flash device contains different operations and/or implementations, 
          it should be implemented separately.

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

#ifdef HAL_NAND_MODULE_ENABLED

/** @defgroup NAND NAND
  * @brief NAND HAL module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup NAND_Private_Constants NAND Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/    
/** @defgroup NAND_Private_Macros NAND Private Macros
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup NAND_Private_Functions NAND Private Functions
  * @{
  */
static uint32_t NAND_AddressIncrement(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef* Address);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup NAND_Exported_Functions NAND Exported Functions
  * @{
  */
    
/** @defgroup NAND_Exported_Functions_Group1 Initialization and de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
            ##### NAND Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize
    the NAND memory
  
@endverbatim
  * @{
  */
    
/**
  * @brief  Perform NAND memory Initialization sequence.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  ComSpace_Timing: pointer to Common space timing structure
  * @param  AttSpace_Timing: pointer to Attribute space timing structure
  * @retval HAL status
  */
HAL_StatusTypeDef  HAL_NAND_Init(NAND_HandleTypeDef *hnand, FMC_NAND_PCC_TimingTypeDef *ComSpace_Timing, FMC_NAND_PCC_TimingTypeDef *AttSpace_Timing)
{
  /* Check the NAND handle state */
  if(hnand == NULL)
  {
     return HAL_ERROR;
  }

  if(hnand->State == HAL_NAND_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hnand->Lock = HAL_UNLOCKED;

    /* Initialize the low level hardware (MSP) */
    HAL_NAND_MspInit(hnand);
  } 

  /* Initialize NAND control Interface */
  FMC_NAND_Init(hnand->Instance, &(hnand->Init));
  
  /* Initialize NAND common space timing Interface */  
  FMC_NAND_CommonSpace_Timing_Init(hnand->Instance, ComSpace_Timing, hnand->Init.NandBank);
  
  /* Initialize NAND attribute space timing Interface */  
  FMC_NAND_AttributeSpace_Timing_Init(hnand->Instance, AttSpace_Timing, hnand->Init.NandBank);
  
  /* Enable the NAND device */
  __FMC_NAND_ENABLE(hnand->Instance, hnand->Init.NandBank);
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Perform NAND memory De-Initialization sequence.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_DeInit(NAND_HandleTypeDef *hnand)  
{
  /* Initialize the low level hardware (MSP) */
  HAL_NAND_MspDeInit(hnand);

  /* Configure the NAND registers with their reset values */
  FMC_NAND_DeInit(hnand->Instance, hnand->Init.NandBank);

  /* Reset the NAND controller state */
  hnand->State = HAL_NAND_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hnand);

  return HAL_OK;
}

/**
  * @brief  Initialize the NAND MSP.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval None
  */
__weak void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnand);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NAND_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitialize the NAND MSP.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval None
  */
__weak void HAL_NAND_MspDeInit(NAND_HandleTypeDef *hnand)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnand);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NAND_MspDeInit could be implemented in the user file
   */ 
}


/**
  * @brief  This function handles NAND device interrupt request.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
*/
void HAL_NAND_IRQHandler(NAND_HandleTypeDef *hnand)
{
  /* Check NAND interrupt Rising edge flag */
  if(__FMC_NAND_GET_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_RISING_EDGE))
  {
    /* NAND interrupt callback*/
    HAL_NAND_ITCallback(hnand);
  
    /* Clear NAND interrupt Rising edge pending bit */
    __FMC_NAND_CLEAR_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_RISING_EDGE);
  }
  
  /* Check NAND interrupt Level flag */
  if(__FMC_NAND_GET_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_LEVEL))
  {
    /* NAND interrupt callback*/
    HAL_NAND_ITCallback(hnand);

    /* Clear NAND interrupt Level pending bit */
    __FMC_NAND_CLEAR_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_LEVEL);
  }

  /* Check NAND interrupt Falling edge flag */
  if(__FMC_NAND_GET_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_FALLING_EDGE))
  {
    /* NAND interrupt callback*/
    HAL_NAND_ITCallback(hnand);
  
    /* Clear NAND interrupt Falling edge pending bit */
    __FMC_NAND_CLEAR_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_FALLING_EDGE);
  }
  
  /* Check NAND interrupt FIFO empty flag */
  if(__FMC_NAND_GET_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_FEMPT))
  {
    /* NAND interrupt callback*/
    HAL_NAND_ITCallback(hnand);
  
    /* Clear NAND interrupt FIFO empty pending bit */
    __FMC_NAND_CLEAR_FLAG(hnand->Instance, hnand->Init.NandBank, FMC_FLAG_FEMPT);
  }  
}

/**
  * @brief  NAND interrupt feature callback.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval None
  */
__weak void HAL_NAND_ITCallback(NAND_HandleTypeDef *hnand)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnand);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_NAND_ITCallback could be implemented in the user file
   */
}
 
/**
  * @}
  */
  
/** @defgroup NAND_Exported_Functions_Group2 Input and Output functions 
  * @brief    Input Output and memory control functions 
  *
  @verbatim    
  ==============================================================================
                    ##### NAND Input and Output functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to use and control the NAND 
    memory
  
@endverbatim
  * @{
  */

/**
  * @brief  Read the NAND memory electronic signature.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pNAND_ID: NAND ID structure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Read_ID(NAND_HandleTypeDef *hnand, NAND_IDTypeDef *pNAND_ID)
{
  __IO uint32_t data = 0;
  uint32_t deviceaddress = 0;

  /* Process Locked */
  __HAL_LOCK(hnand);  
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Send Read ID command sequence */ 	
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_READID;
  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;

  /* Read the electronic signature from NAND flash */	
  data = *(__IO uint32_t *)deviceaddress;
  
  /* Return the data read */
  pNAND_ID->Maker_Id   = ADDR_1ST_CYCLE(data);
  pNAND_ID->Device_Id  = ADDR_2ND_CYCLE(data);
  pNAND_ID->Third_Id   = ADDR_3RD_CYCLE(data);
  pNAND_ID->Fourth_Id  = ADDR_4TH_CYCLE(data);
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);   
   
  return HAL_OK;
}

/**
  * @brief  NAND memory reset.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Reset(NAND_HandleTypeDef *hnand)
{
  uint32_t deviceaddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);
    
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  /* Identify the device address */  
  deviceaddress = NAND_DEVICE;
  
  /* Update the NAND controller state */   
  hnand->State = HAL_NAND_STATE_BUSY; 
  
  /* Send NAND reset command */  
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = 0xFF;
    
  
  /* Update the NAND controller state */   
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);    
  
  return HAL_OK;
  
}

/**
  * @brief  Read Page(s) from NAND memory block.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress: pointer to NAND address structure
  * @param  pBuffer: pointer to destination read buffer
  * @param  NumPageToRead: number of pages to read from block 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Read_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead)
{   
  __IO uint32_t index  = 0;
  uint32_t deviceaddress = 0, size = 0, numpagesread = 0, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  uint32_t addressoffset = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;

  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Page(s) read loop */
  while((NumPageToRead != 0) && (addressstatus == NAND_VALID_ADDRESS))  
  {	   
    /* update the buffer size */
    size = hnand->Info.PageSize + ((hnand->Info.PageSize) * numpagesread);
    
    /* Get the address offset */
    addressoffset = ARRAY_ADDRESS(&nandaddress, hnand);
    
    /* Send read page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;  
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00; 
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(addressoffset); 
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(addressoffset); 
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(addressoffset);
  
    /* for 512 and 1 GB devices, 4th cycle is required */    
    if(hnand->Info.BlockNbr >= 1024)
    {
      *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_4TH_CYCLE(addressoffset);
    }
  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;
      
    /* Get Data into Buffer */    
    for(; index < size; index++)
    {
      *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
    }
    
    /* Increment read pages number */
    numpagesread++;
    
    /* Decrement pages to read */
    NumPageToRead--;
    
    /* Increment the NAND address */
    addressstatus = NAND_AddressIncrement(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);  
    
  return HAL_OK;

}

/**
  * @brief  Write Page(s) to NAND memory block.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress: pointer to NAND address structure
  * @param  pBuffer: pointer to source buffer to write  
  * @param  NumPageToWrite: number of pages to write to block 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Write_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToWrite)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0;
  uint32_t deviceaddress = 0 , size = 0, numpageswritten = 0, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  uint32_t addressoffset = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);  

  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
    
  /* Page(s) write loop */
  while((NumPageToWrite != 0) && (addressstatus == NAND_VALID_ADDRESS))
  {  
    /* update the buffer size */
    size = hnand->Info.PageSize + ((hnand->Info.PageSize) * numpageswritten);
    
    /* Get the address offset */
    addressoffset = ARRAY_ADDRESS(&nandaddress, hnand);
 
    /* Send write page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(addressoffset);  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(addressoffset);  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(addressoffset);
  
    /* for 512 and 1 GB devices, 4th cycle is required */     
    if(hnand->Info.BlockNbr >= 1024)
    {
      *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_4TH_CYCLE(addressoffset);
    }
  
    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      } 
    }    
 
    /* Increment written pages number */
    numpageswritten++;
    
    /* Decrement pages to write */
    NumPageToWrite--;
    
    /* Increment the NAND address */
    addressstatus = NAND_AddressIncrement(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);      
  
  return HAL_OK;
}

/**
  * @brief  Read Spare area(s) from NAND memory. 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress: pointer to NAND address structure
  * @param  pBuffer: pointer to source buffer to write  
  * @param  NumSpareAreaToRead: Number of spare area to read  
  * @retval HAL status
*/
HAL_StatusTypeDef HAL_NAND_Read_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead)
{
  __IO uint32_t index   = 0; 
  uint32_t deviceaddress = 0, size = 0, num_spare_area_read = 0, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  uint32_t addressoffset = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);  
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;    
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Spare area(s) read loop */ 
  while((NumSpareAreaToRead != 0) && (addressstatus == NAND_VALID_ADDRESS))
  { 
    /* update the buffer size */
    size = (hnand->Info.SpareAreaSize) + ((hnand->Info.SpareAreaSize) * num_spare_area_read);   

    /* Get the address offset */
    addressoffset = ARRAY_ADDRESS(&nandaddress, hnand);

    /* Send read spare area command sequence */     
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_C;
    
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00; 
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(addressoffset);     
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(addressoffset);     
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(addressoffset);
  
    /* for 512 and 1 GB devices, 4th cycle is required */    
    if(hnand->Info.BlockNbr >= 1024)
    {
      *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_4TH_CYCLE(addressoffset);
    } 

    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_TRUE1;    
    
    /* Get Data into Buffer */
    for ( ;index < size; index++)
    {
      *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
    }
    
    /* Increment read spare areas number */
    num_spare_area_read++;
    
    /* Decrement spare areas to read */
    NumSpareAreaToRead--;
    
    /* Increment the NAND address */
    addressstatus = NAND_AddressIncrement(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);     

  return HAL_OK;  
}

/**
  * @brief  Write Spare area(s) to NAND memory.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress: pointer to NAND address structure
  * @param  pBuffer: pointer to source buffer to write  
  * @param  NumSpareAreaTowrite: number of spare areas to write to block
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Write_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0;
  uint32_t deviceaddress = 0, size = 0, num_spare_area_written = 0, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  uint32_t addressoffset = 0;

  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;
  
  /* Update the FMC_NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Spare area(s) write loop */
  while((NumSpareAreaTowrite != 0) && (addressstatus == NAND_VALID_ADDRESS))
  {  
    /* update the buffer size */
    size = (hnand->Info.SpareAreaSize) + ((hnand->Info.SpareAreaSize) * num_spare_area_written);

    /* Get the address offset */
    addressoffset = ARRAY_ADDRESS(&nandaddress, hnand);
    
    /* Send write Spare area command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_C;
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = 0x00;  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(addressoffset);  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(addressoffset);  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(addressoffset); 
  
    /* for 512 and 1 GB devices, 4th cycle is required */     
    if(hnand->Info.BlockNbr >= 1024)
    {
      *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_4TH_CYCLE(addressoffset);
    }
  
    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    
    /* Get tick */
    tickstart = HAL_GetTick();
   
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      }   
    }

    /* Increment written spare areas number */
    num_spare_area_written++;
    
    /* Decrement spare areas to write */
    NumSpareAreaTowrite--;
    
    /* Increment the NAND address */
    addressstatus = NAND_AddressIncrement(hnand, &nandaddress);
  }

  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hnand);
    
  return HAL_OK;  
}

/**
  * @brief  NAND memory Block erase.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress: pointer to NAND address structure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress)
{
  uint32_t deviceaddress = 0;
  uint32_t tickstart = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceaddress = NAND_DEVICE;
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* Send Erase block command sequence */
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE0;

  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_1ST_CYCLE(ARRAY_ADDRESS(pAddress, hnand));
  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_2ND_CYCLE(ARRAY_ADDRESS(pAddress, hnand));
  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_3RD_CYCLE(ARRAY_ADDRESS(pAddress, hnand));
  
  /* for 512 and 1 GB devices, 4th cycle is required */     
  if(hnand->Info.BlockNbr >= 1024)
  {
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = ADDR_4TH_CYCLE(ARRAY_ADDRESS(pAddress, hnand));
  }  
		
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE1; 
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Read status until NAND is ready */
  while(HAL_NAND_Read_Status(hnand) != NAND_READY)
  {
    if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
    {
      /* Process unlocked */
      __HAL_UNLOCK(hnand);    
  
      return HAL_TIMEOUT; 
    } 
  }    
 
  /* Process unlocked */
  __HAL_UNLOCK(hnand);    
  
  return HAL_OK;  
}

/**
  * @brief  NAND memory read status. 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval NAND status
  */
uint32_t HAL_NAND_Read_Status(NAND_HandleTypeDef *hnand)
{
  uint32_t data = 0;
  uint32_t deviceaddress = 0;
  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hnand);

  /* Identify the device address */
   deviceaddress = NAND_DEVICE;

  /* Send Read status operation command */
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_STATUS;
  
  /* Read status register data */
  data = *(__IO uint8_t *)deviceaddress;

  /* Return the status */
  if((data & NAND_ERROR) == NAND_ERROR)
  {
    return NAND_ERROR;
  } 
  else if((data & NAND_READY) == NAND_READY)
  {
    return NAND_READY;
  }

  return NAND_BUSY; 
}

/**
  * @brief  Increment the NAND memory address.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param pAddress: pointer to NAND address structure
  * @retval The new status of the increment address operation. It can be:
  *           - NAND_VALID_ADDRESS: When the new address is valid address
  *           - NAND_INVALID_ADDRESS: When the new address is invalid address
  */
uint32_t HAL_NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress)
{
  uint32_t status = NAND_VALID_ADDRESS;
 
  /* Increment page address */
  pAddress->Page++;

  /* Check NAND address is valid */
  if(pAddress->Page == hnand->Info.BlockSize)
  {
    pAddress->Page = 0;
    pAddress->Block++;
    
    if(pAddress->Block == hnand->Info.ZoneSize)
    {
      pAddress->Block = 0;
      pAddress->Zone++;

      if(pAddress->Zone == (hnand->Info.ZoneSize/ hnand->Info.BlockNbr))
      {
        status = NAND_INVALID_ADDRESS;
      }
    }
  } 
  
  return (status);
}
/**
  * @}
  */

/** @defgroup NAND_Exported_Functions_Group3 Peripheral Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                         ##### NAND Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the NAND interface.

@endverbatim
  * @{
  */ 

    
/**
  * @brief  Enable dynamically NAND ECC feature.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */    
HAL_StatusTypeDef  HAL_NAND_ECC_Enable(NAND_HandleTypeDef *hnand)
{
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_BUSY;
   
  /* Enable ECC feature */
  FMC_NAND_ECC_Enable(hnand->Instance, hnand->Init.NandBank);
  
  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_READY;
  
  return HAL_OK;  
}

/**
  * @brief  Disable dynamically NAND ECC feature.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */  
HAL_StatusTypeDef  HAL_NAND_ECC_Disable(NAND_HandleTypeDef *hnand)  
{
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_BUSY;
    
  /* Disable ECC feature */
  FMC_NAND_ECC_Disable(hnand->Instance, hnand->Init.NandBank);
  
  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_READY;
  
  return HAL_OK;  
}

/**
  * @brief  Disable dynamically NAND ECC feature.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  ECCval: pointer to ECC value 
  * @param  Timeout: maximum timeout to wait    
  * @retval HAL status
  */
HAL_StatusTypeDef  HAL_NAND_GetECC(NAND_HandleTypeDef *hnand, uint32_t *ECCval, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_BUSY;  
   
  /* Get NAND ECC value */
  status = FMC_NAND_GetECC(hnand->Instance, ECCval, hnand->Init.NandBank, Timeout);
  
  /* Update the NAND state */
  hnand->State = HAL_NAND_STATE_READY;

  return status;  
}
                      
/**
  * @}
  */
  
    
/** @defgroup NAND_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                         ##### NAND State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in run-time the status of the NAND controller 
    and the data flow.

@endverbatim
  * @{
  */
  
/**
  * @brief  Return the NAND handle state.
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL state
  */
HAL_NAND_StateTypeDef HAL_NAND_GetState(NAND_HandleTypeDef *hnand)
{
  /* Return NAND handle state */
  return hnand->State;
}

/**
  * @}
  */  

/**
  * @}
  */

/** @addtogroup NAND_Private_Functions
  * @{
  */

/**
  * @brief  Increment the NAND memory address. 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  Address: address to be incremented.
  * @retval The new status of the increment address operation. It can be:
  *              - NAND_VALID_ADDRESS: When the new address is valid address
  *              - NAND_INVALID_ADDRESS: When the new address is invalid address   
  */
static uint32_t NAND_AddressIncrement(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef* Address)
{
  uint32_t status = NAND_VALID_ADDRESS;
 
  Address->Page++;

  if(Address->Page == hnand->Info.BlockSize)
  {
    Address->Page = 0;
    Address->Block++;
    
    if(Address->Block == hnand->Info.ZoneSize)
    {
      Address->Block = 0;
      Address->Zone++;

      if(Address->Zone == hnand->Info.BlockNbr)
      {
        status = NAND_INVALID_ADDRESS;
      }
    }
  } 
  
  return (status);
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_NAND_MODULE_ENABLED  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
