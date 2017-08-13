/**
  ******************************************************************************
  * @file    stm32l4xx_hal_flash_ex.c
  * @author  MCD Application Team
  * @version V1.3.0 
  * @date    29-January-2016
  * @brief   Extended FLASH HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the FLASH extended peripheral:
  *           + Extended programming operations functions
  *  
 @verbatim    
 ==============================================================================
                   ##### Flash Extended features #####
  ==============================================================================
           
  [..] Comparing to other previous devices, the FLASH interface for STM32L4xx 
       devices contains the following additional features 
       
       (+) Capacity up to 2 Mbyte with dual bank architecture supporting read-while-write
           capability (RWW)
       (+) Dual bank memory organization       
       (+) PCROP protection for all banks
   
                        ##### How to use this driver #####
 ==============================================================================
  [..] This driver provides functions to configure and program the FLASH memory 
       of all STM32L4xx devices. It includes
      (#) Flash Memory Erase functions: 
           (++) Lock and Unlock the FLASH interface using HAL_FLASH_Unlock() and 
                HAL_FLASH_Lock() functions
           (++) Erase function: Erase page, erase all sectors
           (++) There are two modes of erase :
             (+++) Polling Mode using HAL_FLASHEx_Erase()
             (+++) Interrupt Mode using HAL_FLASHEx_Erase_IT()
                         
      (#) Option Bytes Programming function: Use HAL_FLASHEx_OBProgram() to :
        (++) Set/Reset the write protection
        (++) Set the Read protection Level
        (++) Program the user Option Bytes
        (++) Configure the PCROP protection
        
      (#) Get Option Bytes Configuration function: Use HAL_FLASHEx_OBGetConfig() to :
        (++) Get the value of a write protection area
        (++) Know if the read protection is activated
        (++) Get the value of the user Option Bytes
        (++) Get the value of a PCROP area
    
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

/** @defgroup FLASHEx FLASHEx
  * @brief FALSH Extended HAL module driver
  * @{
  */

#ifdef HAL_FLASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Variables FLASHEx Private Variables
 * @{
 */
extern FLASH_ProcessTypeDef pFlash;
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup FLASHEx_Private_Functions FLASHEx Private Functions
 * @{
 */
extern HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
void                     FLASH_PageErase(uint32_t Page, uint32_t Banks);
static void              FLASH_MassErase(uint32_t Banks);
void                     FLASH_FlushCaches(void);
static HAL_StatusTypeDef FLASH_OB_WRPConfig(uint32_t WRPArea, uint32_t WRPStartOffset, uint32_t WRDPEndOffset);
static HAL_StatusTypeDef FLASH_OB_RDPConfig(uint32_t RDPLevel);
static HAL_StatusTypeDef FLASH_OB_UserConfig(uint32_t UserType, uint32_t UserConfig);
static HAL_StatusTypeDef FLASH_OB_PCROPConfig(uint32_t PCROPConfig, uint32_t PCROPStartAddr, uint32_t PCROPEndAddr);
static void              FLASH_OB_GetWRP(uint32_t WRPArea, uint32_t * WRPStartOffset, uint32_t * WRDPEndOffset);
static uint32_t          FLASH_OB_GetRDP(void);
static uint32_t          FLASH_OB_GetUser(void);
static void              FLASH_OB_GetPCROP(uint32_t * PCROPConfig, uint32_t * PCROPStartAddr, uint32_t * PCROPEndAddr);
/**
  * @}
  */

/* Exported functions -------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Functions FLASH Extended Exported Functions
  * @{
  */ 

/** @defgroup FLASHEx_Exported_Functions_Group1 Extended IO operation functions
 *  @brief   Extended IO operation functions 
 *
@verbatim   
 ===============================================================================
                ##### Extended programming operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the Extended FLASH 
    programming operations Operations.
 
@endverbatim
  * @{
  */
/**
  * @brief  Perform a mass erase or erase the specified FLASH memory pages.
  * @param[in]  pEraseInit: pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * 
  * @param[out]  PageError  : pointer to variable that contains the configuration 
  *         information on faulty page in case of error (0xFFFFFFFF means that all 
  *         the pages have been correctly erased)
  * 
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  uint32_t page_index = 0;

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

    if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /* Mass erase to be done */
      FLASH_MassErase(pEraseInit->Banks);

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

      /* If the erase operation is completed, disable the MER1 and MER2 Bits */
      CLEAR_BIT(FLASH->CR, (FLASH_CR_MER1 | FLASH_CR_MER2));
    }
    else
    {
      /*Initialization of PageError variable*/
      *PageError = 0xFFFFFFFF;
      
      for(page_index = pEraseInit->Page; page_index < (pEraseInit->Page + pEraseInit->NbPages); page_index++)
      {
        FLASH_PageErase(page_index, pEraseInit->Banks);

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

        /* If the erase operation is completed, disable the PER Bit */
        CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

        if (status != HAL_OK)
        {
          /* In case of error, stop erase procedure and return the faulty address */
          *PageError = page_index;
          break;
        }
      }
    }
    
    /* Flush the caches to be sure of the data consistency */
    FLASH_FlushCaches();
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory pages with interrupt enabled.
  * @param  pEraseInit: pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * 
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Enable End of Operation and Error interrupts */
  __HAL_FLASH_ENABLE_IT(FLASH_IT_EOP | FLASH_IT_OPERR);

  pFlash.Bank = pEraseInit->Banks;

  if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
  {
    /* Mass erase to be done */
    pFlash.ProcedureOnGoing = FLASH_PROC_MASS_ERASE;
    FLASH_MassErase(pEraseInit->Banks);
  }
  else
  {
    /* Erase by page to be done */
    pFlash.ProcedureOnGoing = FLASH_PROC_PAGE_ERASE;
    pFlash.NbPagesToErase = pEraseInit->NbPages;
    pFlash.Page = pEraseInit->Page;

    /*Erase 1st page and wait for IT */
    FLASH_PageErase(pEraseInit->Page, pEraseInit->Banks);
  }
  
  return status;
}

/**
  * @brief  Program Option bytes.
  * @param  pOBInit: pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  * 
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  
  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Check the parameters */
  assert_param(IS_OPTIONBYTE(pOBInit->OptionType));
  
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Write protection configuration */
  if((pOBInit->OptionType & OPTIONBYTE_WRP) != RESET)
  {
    /* Configure of Write protection on the selected area */
    status = FLASH_OB_WRPConfig(pOBInit->WRPArea, pOBInit->WRPStartOffset, pOBInit->WRPEndOffset);
  }
  
  /* Read protection configuration */
  if((pOBInit->OptionType & OPTIONBYTE_RDP) != RESET)
  {
    /* Configure the Read protection level */
    status = FLASH_OB_RDPConfig(pOBInit->RDPLevel);
  }
  
  /* User Configuration */
  if((pOBInit->OptionType & OPTIONBYTE_USER) != RESET)
  {
    /* Configure the user option bytes */
    status = FLASH_OB_UserConfig(pOBInit->USERType, pOBInit->USERConfig);
  }
  
  /* PCROP Configuration */
  if((pOBInit->OptionType & OPTIONBYTE_PCROP) != RESET)
  {
    /* Configure the Proprietary code readout protection */
    status = FLASH_OB_PCROPConfig(pOBInit->PCROPConfig, pOBInit->PCROPStartAddr, pOBInit->PCROPEndAddr);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Get the Option bytes configuration.
  * @param  pOBInit: pointer to an FLASH_OBInitStruct structure that contains the 
  *                  configuration information. The fields pOBInit->WRPArea and 
  *                  pOBInit->PCROPConfig should indicate which area is requested
  *                  for the WRP and PCROP 
  * 
  * @retval None
  */
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit)
{
  pOBInit->OptionType = (OPTIONBYTE_WRP | OPTIONBYTE_RDP | OPTIONBYTE_USER | OPTIONBYTE_PCROP);

  /* Get write protection on the selected area */
  FLASH_OB_GetWRP(pOBInit->WRPArea, &(pOBInit->WRPStartOffset), &(pOBInit->WRPEndOffset));
  
  /* Get Read protection level */
  pOBInit->RDPLevel = FLASH_OB_GetRDP();
  
  /* Get the user option bytes */
  pOBInit->USERConfig = FLASH_OB_GetUser();
  
  /* Get the Proprietary code readout protection */
  FLASH_OB_GetPCROP(&(pOBInit->PCROPConfig), &(pOBInit->PCROPStartAddr), &(pOBInit->PCROPEndAddr));
  
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/* Private functions ---------------------------------------------------------*/

/** @addtogroup FLASHEx_Private_Functions
  * @{
  */
/**
  * @brief  Mass erase of FLASH memory.
  * @param  Banks: Banks to be erased
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: Bank1 to be erased
  *            @arg FLASH_BANK_2: Bank2 to be erased
  *            @arg FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  * @retval None
  */
static void FLASH_MassErase(uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_FLASH_BANK(Banks));

  /* Set the Mass Erase Bit for the bank 1 if requested */
  if((Banks & FLASH_BANK_1) != RESET)
  {
    SET_BIT(FLASH->CR, FLASH_CR_MER1);
  }
  
  /* Set the Mass Erase Bit for the bank 2 if requested */
  if((Banks & FLASH_BANK_2) != RESET)
  {
    SET_BIT(FLASH->CR, FLASH_CR_MER2);
  }

  /* Proceed to erase all sectors */
  SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

/**
  * @brief  Erase the specified FLASH memory page.
  * @param  Page: FLASH page to erase
  *         This parameter must be a value between 0 and (max number of pages in the bank - 1)      
  * @param  Banks: Bank(s) where the page will be erased
  *          This parameter can be one or a combination of the following values:
  *            @arg FLASH_BANK_1: Page in bank 1 to be erased
  *            @arg FLASH_BANK_2: Page in bank 2 to be erased
  * @retval None
  */
void FLASH_PageErase(uint32_t Page, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_FLASH_PAGE(Page));
  assert_param(IS_FLASH_BANK_EXCLUSIVE(Banks));

  if((Banks & FLASH_BANK_1) != RESET)
  {
    CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
  }
  else
  {
    SET_BIT(FLASH->CR, FLASH_CR_BKER);
  }

  /* Proceed to erase the page */
  MODIFY_REG(FLASH->CR, FLASH_CR_PNB, (Page << 3));
  SET_BIT(FLASH->CR, FLASH_CR_PER);
  SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

/**
  * @brief  Flush the instruction and data caches.
  * @retval None
  */
void FLASH_FlushCaches(void)
{
  /* Flush instruction cache  */
  if(READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) != RESET)
  {
    /* Disable instruction cache  */
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
    /* Reset instruction cache */
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
    /* Enable instruction cache */
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  }
  
  /* Flush data cache */
  if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != RESET)
  {
    /* Disable data cache  */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    /* Reset data cache */
    __HAL_FLASH_DATA_CACHE_RESET();
    /* Enable data cache */
    __HAL_FLASH_DATA_CACHE_ENABLE();
  }
}

/**
  * @brief  Configure the write protection of the desired pages.
  *
  * @note   When the memory read protection level is selected (RDP level = 1), 
  *         it is not possible to program or erase Flash memory if the CPU debug 
  *         features are connected (JTAG or single wire) or boot code is being 
  *         executed from RAM or System flash, even if WRP is not activated. 
  * @note   To configure the WRP options, the option lock bit OPTLOCK must be 
  *         cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the WRP options, the option bytes must be reloaded 
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *
  * @param  WRPArea: specifies the area to be configured.
  *          This parameter can be one of the following values:
  *            @arg OB_WRPAREA_BANK1_AREAA: Flash Bank 1 Area A                      
  *            @arg OB_WRPAREA_BANK1_AREAB: Flash Bank 1 Area B                      
  *            @arg OB_WRPAREA_BANK2_AREAA: Flash Bank 2 Area A                      
  *            @arg OB_WRPAREA_BANK2_AREAB: Flash Bank 2 Area B                      
  *
  * @param  WRPStartOffset: specifies the start page of the write protected area
  *          This parameter can be page number between 0 and (max number of pages in the bank - 1) 
  *
  * @param  WRDPEndOffset: specifies the end page of the write protected area
  *          This parameter can be page number between WRPStartOffset and (max number of pages in the bank - 1) 
  *
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_WRPConfig(uint32_t WRPArea, uint32_t WRPStartOffset, uint32_t WRDPEndOffset)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_WRPAREA(WRPArea));
  assert_param(IS_FLASH_PAGE(WRPStartOffset));
  assert_param(IS_FLASH_PAGE(WRDPEndOffset));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /* Configure the write protected area */
    if(WRPArea == OB_WRPAREA_BANK1_AREAA)
    {
      MODIFY_REG(FLASH->WRP1AR, (FLASH_WRP1AR_WRP1A_STRT | FLASH_WRP1AR_WRP1A_END), 
                 (WRPStartOffset | (WRDPEndOffset << 16)));
    }
    else if(WRPArea == OB_WRPAREA_BANK1_AREAB)
    {
      MODIFY_REG(FLASH->WRP1BR, (FLASH_WRP1BR_WRP1B_STRT | FLASH_WRP1BR_WRP1B_END), 
                 (WRPStartOffset | (WRDPEndOffset << 16)));
    }
    else if(WRPArea == OB_WRPAREA_BANK2_AREAA)
    {
      MODIFY_REG(FLASH->WRP2AR, (FLASH_WRP2AR_WRP2A_STRT | FLASH_WRP2AR_WRP2A_END), 
                 (WRPStartOffset | (WRDPEndOffset << 16)));
    }
    else if(WRPArea == OB_WRPAREA_BANK2_AREAB)
    {
      MODIFY_REG(FLASH->WRP2BR, (FLASH_WRP2BR_WRP2B_STRT | FLASH_WRP2BR_WRP2B_END), 
                 (WRPStartOffset | (WRDPEndOffset << 16)));
    }
    
    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }
  
  return status;
}

/**
  * @brief  Set the read protection level.
  *    
  * @note   To configure the RDP level, the option lock bit OPTLOCK must be 
  *         cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the RDP level, the option bytes must be reloaded 
  *         through the call of the HAL_FLASH_OB_Launch() function.
  * @note   !!! Warning : When enabling OB_RDP level 2 it's no more possible 
  *         to go back to level 1 or 0 !!!
  *    
  * @param  RDPLevel: specifies the read protection level.
  *         This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  *   
  * @retval HAL status
  */
static HAL_StatusTypeDef FLASH_OB_RDPConfig(uint32_t RDPLevel)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_RDP_LEVEL(RDPLevel));
    
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  { 
    /* Configure the RDP level in the option bytes register */
    MODIFY_REG(FLASH->OPTR, FLASH_OPTR_RDP, RDPLevel);
    
    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }

  return status;            
}

/**
  * @brief  Program the FLASH User Option Byte.    
  *   
  * @note   To configure the user option bytes, the option lock bit OPTLOCK must
  *         be cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the user option bytes, the option bytes must be reloaded 
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *   
  * @param  UserType: The FLASH User Option Bytes to be modified 
  * @param  UserConfig: The FLASH User Option Bytes values: 
  *         BOR_LEV(Bit8-10), nRST_STOP(Bit12), nRST_STDBY(Bit13), IWDG_SW(Bit16),
  *         IWDG_STOP(Bit17), IWDG_STDBY(Bit18), WWDG_SW(Bit19), BFB2(Bit20), 
  *         DUALBANK(Bit21), nBOOT1(Bit23), SRAM2_PE(Bit24) and SRAM2_RST(Bit25). 
  *   
  * @retval HAL status
  */
static HAL_StatusTypeDef FLASH_OB_UserConfig(uint32_t UserType, uint32_t UserConfig)
{
  uint32_t optr_reg_val = 0;
  uint32_t optr_reg_mask = 0;
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_USER_TYPE(UserType));
  
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  { 
    if((UserType & OB_USER_BOR_LEV) != RESET)
    {
      /* BOR level option byte should be modified */
      assert_param(IS_OB_USER_BOR_LEVEL(UserConfig & FLASH_OPTR_BOR_LEV));
    
      /* Set value and mask for BOR level option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_BOR_LEV);
      optr_reg_mask |= FLASH_OPTR_BOR_LEV;
    }

    if((UserType & OB_USER_nRST_STOP) != RESET)
    {
      /* nRST_STOP option byte should be modified */
      assert_param(IS_OB_USER_STOP(UserConfig & FLASH_OPTR_nRST_STOP));
    
      /* Set value and mask for nRST_STOP option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_STOP);
      optr_reg_mask |= FLASH_OPTR_nRST_STOP;
    }

    if((UserType & OB_USER_nRST_STDBY) != RESET)
    {
      /* nRST_STDBY option byte should be modified */
      assert_param(IS_OB_USER_STANDBY(UserConfig & FLASH_OPTR_nRST_STDBY));
    
      /* Set value and mask for nRST_STDBY option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_STDBY);
      optr_reg_mask |= FLASH_OPTR_nRST_STDBY;
    }

    if((UserType & OB_USER_nRST_SHDW) != RESET)
    {
      /* nRST_SHDW option byte should be modified */
      assert_param(IS_OB_USER_SHUTDOWN(UserConfig & FLASH_OPTR_nRST_SHDW));
    
      /* Set value and mask for nRST_SHDW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_SHDW);
      optr_reg_mask |= FLASH_OPTR_nRST_SHDW;
    }

    if((UserType & OB_USER_IWDG_SW) != RESET)
    {
      /* IWDG_SW option byte should be modified */
      assert_param(IS_OB_USER_IWDG(UserConfig & FLASH_OPTR_IWDG_SW));
    
      /* Set value and mask for IWDG_SW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_SW);
      optr_reg_mask |= FLASH_OPTR_IWDG_SW;
    }

    if((UserType & OB_USER_IWDG_STOP) != RESET)
    {
      /* IWDG_STOP option byte should be modified */
      assert_param(IS_OB_USER_IWDG_STOP(UserConfig & FLASH_OPTR_IWDG_STOP));
    
      /* Set value and mask for IWDG_STOP option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_STOP);
      optr_reg_mask |= FLASH_OPTR_IWDG_STOP;
    }

    if((UserType & OB_USER_IWDG_STDBY) != RESET)
    {
      /* IWDG_STDBY option byte should be modified */
      assert_param(IS_OB_USER_IWDG_STDBY(UserConfig & FLASH_OPTR_IWDG_STDBY));
    
      /* Set value and mask for IWDG_STDBY option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_STDBY);
      optr_reg_mask |= FLASH_OPTR_IWDG_STDBY;
    }

    if((UserType & OB_USER_WWDG_SW) != RESET)
    {
      /* WWDG_SW option byte should be modified */
      assert_param(IS_OB_USER_WWDG(UserConfig & FLASH_OPTR_WWDG_SW));
    
      /* Set value and mask for WWDG_SW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_WWDG_SW);
      optr_reg_mask |= FLASH_OPTR_WWDG_SW;
    }

    if((UserType & OB_USER_BFB2) != RESET)
    {
      /* BFB2 option byte should be modified */
      assert_param(IS_OB_USER_BFB2(UserConfig & FLASH_OPTR_BFB2));
    
      /* Set value and mask for BFB2 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_BFB2);
      optr_reg_mask |= FLASH_OPTR_BFB2;
    }

    if((UserType & OB_USER_DUALBANK) != RESET)
    {
      /* DUALBANK option byte should be modified */
      assert_param(IS_OB_USER_DUALBANK(UserConfig & FLASH_OPTR_DUALBANK));
    
      /* Set value and mask for DUALBANK option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_DUALBANK);
      optr_reg_mask |= FLASH_OPTR_DUALBANK;
    }

    if((UserType & OB_USER_nBOOT1) != RESET)
    {
      /* nBOOT1 option byte should be modified */
      assert_param(IS_OB_USER_BOOT1(UserConfig & FLASH_OPTR_nBOOT1));
    
      /* Set value and mask for nBOOT1 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nBOOT1);
      optr_reg_mask |= FLASH_OPTR_nBOOT1;
    }

    if((UserType & OB_USER_SRAM2_PE) != RESET)
    {
      /* SRAM2_PE option byte should be modified */
      assert_param(IS_OB_USER_SRAM2_PARITY(UserConfig & FLASH_OPTR_SRAM2_PE));
    
      /* Set value and mask for SRAM2_PE option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_SRAM2_PE);
      optr_reg_mask |= FLASH_OPTR_SRAM2_PE;
    }

    if((UserType & OB_USER_SRAM2_RST) != RESET)
    {
      /* SRAM2_RST option byte should be modified */
      assert_param(IS_OB_USER_SRAM2_RST(UserConfig & FLASH_OPTR_SRAM2_RST));
    
      /* Set value and mask for SRAM2_RST option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_SRAM2_RST);
      optr_reg_mask |= FLASH_OPTR_SRAM2_RST;
    }

    /* Configure the option bytes register */
    MODIFY_REG(FLASH->OPTR, optr_reg_mask, optr_reg_val);
    
    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }

  return status;            
}

/**
  * @brief  Configure the Proprietary code readout protection of the desired addresses.
  *
  * @note   To configure the PCROP options, the option lock bit OPTLOCK must be 
  *         cleared with the call of the HAL_FLASH_OB_Unlock() function.
  * @note   To validate the PCROP options, the option bytes must be reloaded 
  *         through the call of the HAL_FLASH_OB_Launch() function.
  *
  * @param  PCROPConfig: specifies the configuration (Bank to be configured and PCROP_RDP option).
  *          This parameter must be a combination of FLASH_BANK_1 or FLASH_BANK_2 
  *          with OB_PCROP_RDP_NOT_ERASE or OB_PCROP_RDP_ERASE
  *
  * @param  PCROPStartAddr: specifies the start address of the Proprietary code readout protection
  *          This parameter can be an address between begin and end of the bank 
  *
  * @param  PCROPEndAddr: specifies the end address of the Proprietary code readout protection
  *          This parameter can be an address between PCROPStartAddr and end of the bank 
  *
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_PCROPConfig(uint32_t PCROPConfig, uint32_t PCROPStartAddr, uint32_t PCROPEndAddr)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t reg_value = 0;
  uint32_t bank1_addr, bank2_addr;

  /* Check the parameters */
  assert_param(IS_FLASH_BANK_EXCLUSIVE(PCROPConfig & FLASH_BANK_BOTH));
  assert_param(IS_OB_PCROP_RDP(PCROPConfig & FLASH_PCROP1ER_PCROP_RDP));
  assert_param(IS_FLASH_MAIN_MEM_ADDRESS(PCROPStartAddr));
  assert_param(IS_FLASH_MAIN_MEM_ADDRESS(PCROPEndAddr));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /* Get the information about the bank swapping */
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
      bank1_addr = FLASH_BASE;
      bank2_addr = FLASH_BASE + FLASH_BANK_SIZE;
    }
    else
    {
      bank1_addr = FLASH_BASE + FLASH_BANK_SIZE;
      bank2_addr = FLASH_BASE;
    }
  
    /* Configure the Proprietary code readout protection */
    if((PCROPConfig & FLASH_BANK_BOTH) == FLASH_BANK_1)
    {
      reg_value = ((PCROPStartAddr - bank1_addr) >> 3);
      MODIFY_REG(FLASH->PCROP1SR, FLASH_PCROP1SR_PCROP1_STRT, reg_value);

      reg_value = ((PCROPEndAddr - bank1_addr) >> 3);
      MODIFY_REG(FLASH->PCROP1ER, FLASH_PCROP1ER_PCROP1_END, reg_value);
    }
    else if((PCROPConfig & FLASH_BANK_BOTH) == FLASH_BANK_2)
    {
      reg_value = ((PCROPStartAddr - bank2_addr) >> 3);
      MODIFY_REG(FLASH->PCROP2SR, FLASH_PCROP2SR_PCROP2_STRT, reg_value);

      reg_value = ((PCROPEndAddr - bank2_addr) >> 3);
      MODIFY_REG(FLASH->PCROP2ER, FLASH_PCROP2ER_PCROP2_END, reg_value);
    }

    MODIFY_REG(FLASH->PCROP1ER, FLASH_PCROP1ER_PCROP_RDP, (PCROPConfig & FLASH_PCROP1ER_PCROP_RDP));
    
    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }
  
  return status;
}

/**
  * @brief  Return the FLASH Write Protection Option Bytes value.
  *
  * @param[in]  WRPArea: specifies the area to be returned.
  *          This parameter can be one of the following values:
  *            @arg OB_WRPAREA_BANK1_AREAA: Flash Bank 1 Area A                      
  *            @arg OB_WRPAREA_BANK1_AREAB: Flash Bank 1 Area B                      
  *            @arg OB_WRPAREA_BANK2_AREAA: Flash Bank 2 Area A                      
  *            @arg OB_WRPAREA_BANK2_AREAB: Flash Bank 2 Area B                      
  *
  * @param[out]  WRPStartOffset: specifies the address where to copied the start page 
  *                         of the write protected area
  *
  * @param[out]  WRDPEndOffset: specifies the address where to copied the end page of 
  *                        the write protected area
  *
  * @retval None
  */
static void FLASH_OB_GetWRP(uint32_t WRPArea, uint32_t * WRPStartOffset, uint32_t * WRDPEndOffset)
{
  /* Check the parameters */
  assert_param(IS_OB_WRPAREA(WRPArea));

  /* Get the configuration of the write protected area */
  if(WRPArea == OB_WRPAREA_BANK1_AREAA)
  {
    *WRPStartOffset = READ_BIT(FLASH->WRP1AR, FLASH_WRP1AR_WRP1A_STRT);
    *WRDPEndOffset = (READ_BIT(FLASH->WRP1AR, FLASH_WRP1AR_WRP1A_END) >> 16);
  }
  else if(WRPArea == OB_WRPAREA_BANK1_AREAB)
  {
    *WRPStartOffset = READ_BIT(FLASH->WRP1BR, FLASH_WRP1BR_WRP1B_STRT);
    *WRDPEndOffset = (READ_BIT(FLASH->WRP1BR, FLASH_WRP1BR_WRP1B_END) >> 16);
  }
  else if(WRPArea == OB_WRPAREA_BANK2_AREAA)
  {
    *WRPStartOffset = READ_BIT(FLASH->WRP2AR, FLASH_WRP2AR_WRP2A_STRT);
    *WRDPEndOffset = (READ_BIT(FLASH->WRP2AR, FLASH_WRP2AR_WRP2A_END) >> 16);
  }
  else if(WRPArea == OB_WRPAREA_BANK2_AREAB)
  {
    *WRPStartOffset = READ_BIT(FLASH->WRP2BR, FLASH_WRP2BR_WRP2B_STRT);
    *WRDPEndOffset = (READ_BIT(FLASH->WRP2BR, FLASH_WRP2BR_WRP2B_END) >> 16);
  }
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval FLASH ReadOut Protection Status:
  *         This return value can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  */
static uint32_t FLASH_OB_GetRDP(void)
{
  if ((READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP) != OB_RDP_LEVEL_0) && 
      (READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP) != OB_RDP_LEVEL_2))
  {
    return (OB_RDP_LEVEL_1);
  }
  else
  {
    return (READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP));
  }
}

/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval The FLASH User Option Bytes values: 
  *         BOR_LEV(Bit8-10), nRST_STOP(Bit12), nRST_STDBY(Bit13), nRST_SHDW(Bit14), 
  *         IWDG_SW(Bit16), IWDG_STOP(Bit17), IWDG_STDBY(Bit18), WWDG_SW(Bit19),  
  *         BFB2(Bit20), DUALBANK(Bit21), nBOOT1(Bit23), SRAM2_PE(Bit24) and SRAM2_RST(Bit25). 
  */
static uint32_t FLASH_OB_GetUser(void)
{
  uint32_t user_config = READ_REG(FLASH->OPTR);
  CLEAR_BIT(user_config, FLASH_OPTR_RDP);
  
  return user_config;
}

/**
  * @brief  Return the FLASH Write Protection Option Bytes value.
  *
  * @param PCROPConfig [inout]: specifies the configuration (Bank to be configured and PCROP_RDP option).
  *          This parameter must be a combination of FLASH_BANK_1 or FLASH_BANK_2 
  *          with OB_PCROP_RDP_NOT_ERASE or OB_PCROP_RDP_ERASE
  *
  * @param PCROPStartAddr [out]: specifies the address where to copied the start address 
  *                         of the Proprietary code readout protection
  *
  * @param PCROPEndAddr [out]: specifies the address where to copied the end address of 
  *                       the Proprietary code readout protection
  *
  * @retval None
  */
static void FLASH_OB_GetPCROP(uint32_t * PCROPConfig, uint32_t * PCROPStartAddr, uint32_t * PCROPEndAddr)
{
  uint32_t reg_value = 0;
  uint32_t bank1_addr, bank2_addr;
  
  /* Check the parameters */
  assert_param(IS_FLASH_BANK_EXCLUSIVE((*PCROPConfig) & FLASH_BANK_BOTH));

  /* Get the information about the bank swapping */
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
    bank1_addr = FLASH_BASE;
    bank2_addr = FLASH_BASE + FLASH_BANK_SIZE;
  }
  else
  {
    bank1_addr = FLASH_BASE + FLASH_BANK_SIZE;
    bank2_addr = FLASH_BASE;
  }

  if(((*PCROPConfig) & FLASH_BANK_BOTH) == FLASH_BANK_1)
  {
    reg_value       = (READ_REG(FLASH->PCROP1SR) & FLASH_PCROP1SR_PCROP1_STRT);
    *PCROPStartAddr = (reg_value << 3) + bank1_addr;
    
    reg_value     = (READ_REG(FLASH->PCROP1ER) & FLASH_PCROP1ER_PCROP1_END);
    *PCROPEndAddr = (reg_value << 3) + bank1_addr;
  }
  else if(((*PCROPConfig) & FLASH_BANK_BOTH) == FLASH_BANK_2)
  {
    reg_value       = (READ_REG(FLASH->PCROP2SR) & FLASH_PCROP2SR_PCROP2_STRT);
    *PCROPStartAddr = (reg_value << 3) + bank2_addr;

    reg_value     = (READ_REG(FLASH->PCROP2ER) & FLASH_PCROP2ER_PCROP2_END);
    *PCROPEndAddr = (reg_value << 3) + bank2_addr;
  }

  *PCROPConfig |= (READ_REG(FLASH->PCROP1ER) & FLASH_PCROP1ER_PCROP_RDP);
}
/**
  * @}
  */ 

/**
  * @}
  */ 

#endif /* HAL_FLASH_MODULE_ENABLED */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
