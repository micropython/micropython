/**
  ******************************************************************************
  * @file    stm32f4xx_ll_fsmc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   FSMC Low Layer HAL module driver.
  *    
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Flexible Static Memory Controller (FSMC) peripheral memories:
  *           + Initialization/de-initialization functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                        ##### FSMC peripheral features #####
  ==============================================================================                  
    [..] The Flexible static memory controller (FSMC) includes two memory controllers:
         (+) The NOR/PSRAM memory controller
         (+) The NAND/PC Card memory controller
       
    [..] The FSMC functional block makes the interface with synchronous and asynchronous static
         memories, SDRAM memories, and 16-bit PC memory cards. Its main purposes are:
         (+) to translate AHB transactions into the appropriate external device protocol.
         (+) to meet the access time requirements of the external memory devices.
   
    [..] All external memories share the addresses, data and control signals with the controller.
         Each external device is accessed by means of a unique Chip Select. The FSMC performs
         only one access at a time to an external device.
         The main features of the FSMC controller are the following:
          (+) Interface with static-memory mapped devices including:
             (++) Static random access memory (SRAM).
             (++) Read-only memory (ROM).
             (++) NOR Flash memory/OneNAND Flash memory.
             (++) PSRAM (4 memory banks).
             (++) 16-bit PC Card compatible devices.
             (++) Two banks of NAND Flash memory with ECC hardware to check up to 8 Kbytes of
                  data.
          (+) Independent Chip Select control for each memory bank.
          (+) Independent configuration for each memory bank.          
        
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

/** @defgroup FSMC 
  * @brief FSMC driver modules
  * @{
  */

#if defined (HAL_SRAM_MODULE_ENABLED) || defined(HAL_NOR_MODULE_ENABLED) || defined(HAL_NAND_MODULE_ENABLED) || defined(HAL_PCCARD_MODULE_ENABLED)

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/    
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FSMC_Private_Functions
  * @{
  */

/** @defgroup FSMC_NORSRAM Controller functions
  * @brief    NORSRAM Controller functions 
  *
  @verbatim 
  ==============================================================================   
                   ##### How to use NORSRAM device driver #####
  ==============================================================================
 
  [..] 
    This driver contains a set of APIs to interface with the FSMC NORSRAM banks in order
    to run the NORSRAM external devices.
      
    (+) FSMC NORSRAM bank reset using the function FSMC_NORSRAM_DeInit() 
    (+) FSMC NORSRAM bank control configuration using the function FSMC_NORSRAM_Init()
    (+) FSMC NORSRAM bank timing configuration using the function FSMC_NORSRAM_Timing_Init()
    (+) FSMC NORSRAM bank extended timing configuration using the function 
        FSMC_NORSRAM_Extended_Timing_Init()
    (+) FSMC NORSRAM bank enable/disable write operation using the functions
        FSMC_NORSRAM_WriteOperation_Enable()/FSMC_NORSRAM_WriteOperation_Disable()

@endverbatim
  * @{
  */
       
/** @defgroup HAL_FSMC_NORSRAM_Group1 Initialization/de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FSMC NORSRAM interface
    (+) De-initialize the FSMC NORSRAM interface 
    (+) Configure the FSMC clock and associated GPIOs    
 
@endverbatim
  * @{
  */
  
/**
  * @brief  Initialize the FSMC_NORSRAM device according to the specified
  *         control parameters in the FSMC_NORSRAM_InitTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Init: Pointer to NORSRAM Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef  FSMC_NORSRAM_Init(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_InitTypeDef* Init)
{ 
  uint32_t tmpr = 0;
    
  /* Check the parameters */
  assert_param(IS_FSMC_NORSRAM_BANK(Init->NSBank));
  assert_param(IS_FSMC_MUX(Init->DataAddressMux));
  assert_param(IS_FSMC_MEMORY(Init->MemoryType));
  assert_param(IS_FSMC_NORSRAM_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FSMC_BURSTMODE(Init->BurstAccessMode));
  assert_param(IS_FSMC_WAIT_POLARITY(Init->WaitSignalPolarity));
  assert_param(IS_FSMC_WRAP_MODE(Init->WrapMode));
  assert_param(IS_FSMC_WAIT_SIGNAL_ACTIVE(Init->WaitSignalActive));
  assert_param(IS_FSMC_WRITE_OPERATION(Init->WriteOperation));
  assert_param(IS_FSMC_WAITE_SIGNAL(Init->WaitSignal));
  assert_param(IS_FSMC_EXTENDED_MODE(Init->ExtendedMode));
  assert_param(IS_FSMC_ASYNWAIT(Init->AsynchronousWait));
  assert_param(IS_FSMC_WRITE_BURST(Init->WriteBurst));
  
  /* Set NORSRAM device control parameters */
  tmpr = (uint32_t)(Init->DataAddressMux       |\
                    Init->MemoryType           |\
                    Init->MemoryDataWidth      |\
                    Init->BurstAccessMode      |\
                    Init->WaitSignalPolarity   |\
                    Init->WrapMode             |\
                    Init->WaitSignalActive     |\
                    Init->WriteOperation       |\
                    Init->WaitSignal           |\
                    Init->ExtendedMode         |\
                    Init->AsynchronousWait     |\
                    Init->WriteBurst
                    );
                    
  if(Init->MemoryType == FSMC_MEMORY_TYPE_NOR)
  {
    tmpr |= (uint32_t)FSMC_NORSRAM_FLASH_ACCESS_ENABLE;
  }
  
  Device->BTCR[Init->NSBank] = tmpr;                   
  
  return HAL_OK;
}


/**
  * @brief  DeInitialize the FSMC_NORSRAM peripheral 
  * @param  Device: Pointer to NORSRAM device instance
  * @param  ExDevice: Pointer to NORSRAM extended mode device instance  
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NORSRAM_DeInit(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FSMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FSMC_NORSRAM_EXTENDED_DEVICE(ExDevice));

  /* Disable the FSMC_NORSRAM device */
  __FSMC_NORSRAM_DISABLE(Device, Bank);
  
  /* De-initialize the FSMC_NORSRAM device */
  /* FSMC_NORSRAM_BANK1 */
  if(Bank == FSMC_NORSRAM_BANK1)
  {
    Device->BTCR[Bank] = 0x000030DB;    
  }
  /* FSMC_NORSRAM_BANK2, FSMC_NORSRAM_BANK3 or FSMC_NORSRAM_BANK4 */
  else
  {   
    Device->BTCR[Bank] = 0x000030D2; 
  }
  
  Device->BTCR[Bank + 1] = 0x0FFFFFFF;
  ExDevice->BWTR[Bank]   = 0x0FFFFFFF;
   
  return HAL_OK;
}


/**
  * @brief  Initialize the FSMC_NORSRAM Timing according to the specified
  *         parameters in the FSMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NORSRAM_Timing_Init(FSMC_NORSRAM_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmpr = 0;
  
  /* Check the parameters */
  assert_param(IS_FSMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
  assert_param(IS_FSMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
  assert_param(IS_FSMC_DATASETUP_TIME(Timing->DataSetupTime));
  assert_param(IS_FSMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
  assert_param(IS_FSMC_CLK_DIV(Timing->CLKDivision));
  assert_param(IS_FSMC_DATA_LATENCY(Timing->DataLatency));
  assert_param(IS_FSMC_ACCESS_MODE(Timing->AccessMode));
  
  /* Set FSMC_NORSRAM device timing parameters */  
  tmpr = (uint32_t)(Timing->AddressSetupTime                  |\
                   ((Timing->AddressHoldTime) << 4)          |\
                   ((Timing->DataSetupTime) << 8)            |\
                   ((Timing->BusTurnAroundDuration) << 16)   |\
                   (((Timing->CLKDivision)-1) << 20)         |\
                   (((Timing->DataLatency)-2) << 24)         |\
                    (Timing->AccessMode)
                    );
  
  Device->BTCR[Bank + 1] = tmpr; 
  
  return HAL_OK;   
}

/**
  * @brief  Initialize the FSMC_NORSRAM Extended mode Timing according to the specified
  *         parameters in the FSMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef  FSMC_NORSRAM_Extended_Timing_Init(FSMC_NORSRAM_EXTENDED_TypeDef *Device, FSMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode)
{
  /* Set NORSRAM device timing register for write configuration, if extended mode is used */
  if(ExtendedMode == FSMC_EXTENDED_MODE_ENABLE)
  {
    /* Check the parameters */  
    assert_param(IS_FSMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
    assert_param(IS_FSMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
    assert_param(IS_FSMC_DATASETUP_TIME(Timing->DataSetupTime));
    assert_param(IS_FSMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
    assert_param(IS_FSMC_CLK_DIV(Timing->CLKDivision));
    assert_param(IS_FSMC_DATA_LATENCY(Timing->DataLatency));
    assert_param(IS_FSMC_ACCESS_MODE(Timing->AccessMode));
  
    Device->BWTR[Bank] = (uint32_t)(Timing->AddressSetupTime                 |\
                                   ((Timing->AddressHoldTime) << 4)          |\
                                   ((Timing->DataSetupTime) << 8)            |\
                                   ((Timing->BusTurnAroundDuration) << 16)   |\
                                   (((Timing->CLKDivision)-1) << 20)         |\
                                   (((Timing->DataLatency)-2) << 24)         |\
                                   (Timing->AccessMode));
  }
  else                                        
  {
    Device->BWTR[Bank] = 0x0FFFFFFF;
  }   
  
  return HAL_OK;  
}


/**
  * @}
  */
  
  
/** @defgroup HAL_FSMC_NORSRAM_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                      ##### FSMC_NORSRAM Control functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the FSMC NORSRAM interface.

@endverbatim
  * @{
  */
    
/**
  * @brief  Enables dynamically FSMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NORSRAM_WriteOperation_Enable(FSMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{
  /* Enable write operation */
  Device->BTCR[Bank] |= FSMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FSMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NORSRAM_WriteOperation_Disable(FSMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{ 
  /* Disable write operation */
  Device->BTCR[Bank] &= ~FSMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}

/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup FSMC_PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
  @verbatim 
  ==============================================================================   
                    ##### How to use NAND device driver #####
  ==============================================================================
  [..]
    This driver contains a set of APIs to interface with the FSMC NAND banks in order
    to run the NAND external devices.
  
    (+) FSMC NAND bank reset using the function FSMC_NAND_DeInit() 
    (+) FSMC NAND bank control configuration using the function FSMC_NAND_Init()
    (+) FSMC NAND bank common space timing configuration using the function 
        FSMC_NAND_CommonSpace_Timing_Init()
    (+) FSMC NAND bank attribute space timing configuration using the function 
        FSMC_NAND_AttributeSpace_Timing_Init()
    (+) FSMC NAND bank enable/disable ECC correction feature using the functions
        FSMC_NAND_ECC_Enable()/FSMC_NAND_ECC_Disable()
    (+) FSMC NAND bank get ECC correction code using the function FSMC_NAND_GetECC()  

@endverbatim
  * @{
  */
    
/** @defgroup HAL_FSMC_NAND_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FSMC NAND interface
    (+) De-initialize the FSMC NAND interface 
    (+) Configure the FSMC clock and associated GPIOs
        
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the FSMC_NAND device according to the specified
  *         control parameters in the FSMC_NAND_HandleTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Init: Pointer to NAND Initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NAND_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_InitTypeDef *Init)
{
  uint32_t tmppcr  = 0; 
    
  /* Check the parameters */
  assert_param(IS_FSMC_NAND_BANK(Init->NandBank));
  assert_param(IS_FSMC_WAIT_FEATURE(Init->Waitfeature));
  assert_param(IS_FSMC_NAND_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FSMC_ECC_STATE(Init->EccComputation));
  assert_param(IS_FSMC_ECCPAGE_SIZE(Init->ECCPageSize));
  assert_param(IS_FSMC_TCLR_TIME(Init->TCLRSetupTime));
  assert_param(IS_FSMC_TAR_TIME(Init->TARSetupTime));   

  /* Set NAND device control parameters */
  tmppcr = (uint32_t)(Init->Waitfeature                |\
                      FSMC_PCR_MEMORY_TYPE_NAND         |\
                      Init->MemoryDataWidth            |\
                      Init->EccComputation             |\
                      Init->ECCPageSize                |\
                      ((Init->TCLRSetupTime) << 9)     |\
                      ((Init->TARSetupTime) << 13)
                      );   
  
  if(Init->NandBank == FSMC_NAND_BANK2)
  {
    /* NAND bank 2 registers configuration */
    Device->PCR2  = tmppcr;
  }
  else
  {
    /* NAND bank 3 registers configuration */
    Device->PCR3  = tmppcr;
  }
  
  return HAL_OK;

}

/**
  * @brief  Initializes the FSMC_NAND Common space Timing according to the specified
  *         parameters in the FSMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NAND_CommonSpace_Timing_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmppmem = 0;  
  
  /* Check the parameters */
  assert_param(IS_FSMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(Timing->HiZSetupTime));
  
  /* Set FSMC_NAND device timing parameters */
  tmppmem = (uint32_t)(Timing->SetupTime                  |\
                       ((Timing->WaitSetupTime) << 8)     |\
                       ((Timing->HoldSetupTime) << 16)    |\
                       ((Timing->HiZSetupTime) << 24)
                       );
                            
  if(Bank == FSMC_NAND_BANK2)
  {
    /* NAND bank 2 registers configuration */
    Device->PMEM2 = tmppmem;
  }
  else
  {
    /* NAND bank 3 registers configuration */
    Device->PMEM3 = tmppmem;
  }  
  
  return HAL_OK;  
}

/**
  * @brief  Initializes the FSMC_NAND Attribute space Timing according to the specified
  *         parameters in the FSMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number 
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NAND_AttributeSpace_Timing_Init(FSMC_NAND_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmppatt = 0;  
  
  /* Check the parameters */  
  assert_param(IS_FSMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(Timing->HiZSetupTime));
  
  /* Set FSMC_NAND device timing parameters */
  tmppatt = (uint32_t)(Timing->SetupTime                  |\
                       ((Timing->WaitSetupTime) << 8)     |\
                       ((Timing->HoldSetupTime) << 16)    |\
                       ((Timing->HiZSetupTime) << 24)
                       );
                       
  if(Bank == FSMC_NAND_BANK2)
  {
    /* NAND bank 2 registers configuration */
    Device->PATT2 = tmppatt;
  }
  else
  {
    /* NAND bank 3 registers configuration */
    Device->PATT3 = tmppatt;
  }   
  
  return HAL_OK;
}


/**
  * @brief  DeInitializes the FSMC_NAND device 
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NAND_DeInit(FSMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Disable the NAND Bank */
  __FSMC_NAND_DISABLE(Device, Bank);
 
  /* De-initialize the NAND Bank */
  if(Bank == FSMC_NAND_BANK2)
  {
    /* Set the FSMC_NAND_BANK2 registers to their reset values */
    Device->PCR2  = 0x00000018;
    Device->SR2   = 0x00000040;
    Device->PMEM2 = 0xFCFCFCFC;
    Device->PATT2 = 0xFCFCFCFC;  
  }
  /* FSMC_Bank3_NAND */  
  else
  {
    /* Set the FSMC_NAND_BANK3 registers to their reset values */
    Device->PCR3  = 0x00000018;
    Device->SR3   = 0x00000040;
    Device->PMEM3 = 0xFCFCFCFC;
    Device->PATT3 = 0xFCFCFCFC; 
  }
  
  return HAL_OK;
}

/**
  * @}
  */
  
  
/** @defgroup HAL_FSMC_NAND_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                       ##### FSMC_NAND Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the FSMC NAND interface.

@endverbatim
  * @{
  */ 

    
/**
  * @brief  Enables dynamically FSMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */    
HAL_StatusTypeDef  FSMC_NAND_ECC_Enable(FSMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Enable ECC feature */
  if(Bank == FSMC_NAND_BANK2)
  {
    Device->PCR2 |= FSMC_PCR2_ECCEN;
  }
  else
  {
    Device->PCR3 |= FSMC_PCR3_ECCEN;
  } 
  
  return HAL_OK;  
}


/**
  * @brief  Disables dynamically FSMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */  
HAL_StatusTypeDef FSMC_NAND_ECC_Disable(FSMC_NAND_TypeDef *Device, uint32_t Bank)  
{  
  /* Disable ECC feature */
  if(Bank == FSMC_NAND_BANK2)
  {
    Device->PCR2 &= ~FSMC_PCR2_ECCEN;
  }
  else
  {
    Device->PCR3 &= ~FSMC_PCR3_ECCEN;
  } 

  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FSMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  ECCval: Pointer to ECC value
  * @param  Bank: NAND bank number
  * @param  Timeout: Timeout wait value  
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_NAND_GetECC(FSMC_NAND_TypeDef *Device, uint32_t *ECCval, uint32_t Bank, uint32_t Timeout)
{
  uint32_t timeout = 0;
  
  /* Check the parameters */ 
  assert_param(IS_FSMC_NAND_DEVICE(Device)); 
  assert_param(IS_FSMC_NAND_BANK(Bank));
    
  timeout = HAL_GetTick() + Timeout;
  
  /* Wait untill FIFO is empty */
  while(__FSMC_NAND_GET_FLAG(Device, Bank, FSMC_FLAG_FEMPT))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        return HAL_TIMEOUT;
      }
    }   
  }
     
  if(Bank == FSMC_NAND_BANK2)
  {    
    /* Get the ECCR2 register value */
    *ECCval = (uint32_t)Device->ECCR2;
  }
  else
  {    
    /* Get the ECCR3 register value */
    *ECCval = (uint32_t)Device->ECCR3;
  }

  return HAL_OK;  
}

/**
  * @}
  */
  
/**
  * @}
  */
    
/** @defgroup FSMC_PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
  @verbatim 
  ==============================================================================  
                    ##### How to use PCCARD device driver #####
  ==============================================================================
  [..]
    This driver contains a set of APIs to interface with the FSMC PCCARD bank in order
    to run the PCCARD/compact flash external devices.
  
    (+) FSMC PCCARD bank reset using the function FSMC_PCCARD_DeInit() 
    (+) FSMC PCCARD bank control configuration using the function FSMC_PCCARD_Init()
    (+) FSMC PCCARD bank common space timing configuration using the function 
        FSMC_PCCARD_CommonSpace_Timing_Init()
    (+) FSMC PCCARD bank attribute space timing configuration using the function 
        FSMC_PCCARD_AttributeSpace_Timing_Init()
    (+) FSMC PCCARD bank IO space timing configuration using the function 
        FSMC_PCCARD_IOSpace_Timing_Init()
       
@endverbatim
  * @{
  */
  
/** @defgroup HAL_FSMC_PCCARD_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FSMC PCCARD interface
    (+) De-initialize the FSMC PCCARD interface 
    (+) Configure the FSMC clock and associated GPIOs
        
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the FSMC_PCCARD device according to the specified
  *         control parameters in the FSMC_PCCARD_HandleTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Init: Pointer to PCCARD Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_PCCARD_Init(FSMC_PCCARD_TypeDef *Device, FSMC_PCCARD_InitTypeDef *Init)
{
  /* Check the parameters */ 
  assert_param(IS_FSMC_WAIT_FEATURE(Init->Waitfeature));
  assert_param(IS_FSMC_TCLR_TIME(Init->TCLRSetupTime));
  assert_param(IS_FSMC_TAR_TIME(Init->TARSetupTime));     
  
  /* Set FSMC_PCCARD device control parameters */
  Device->PCR4 = (uint32_t)(Init->Waitfeature               |\
                            FSMC_NAND_PCC_MEM_BUS_WIDTH_16   |\
                            (Init->TCLRSetupTime << 9)      |\
                            (Init->TARSetupTime << 13));
  
  return HAL_OK;

}

/**
  * @brief  Initializes the FSMC_PCCARD Common space Timing according to the specified
  *         parameters in the FSMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure 
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_PCCARD_CommonSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */
  assert_param(IS_FSMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  Device->PMEM4 = (uint32_t)((Timing->SetupTime                 |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             ); 

  return HAL_OK;  
}

/**
  * @brief  Initializes the FSMC_PCCARD Attribute space Timing according to the specified
  *         parameters in the FSMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure  
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_PCCARD_AttributeSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */  
  assert_param(IS_FSMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  Device->PATT4 = (uint32_t)((Timing->SetupTime                 |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             );  
                                        
  return HAL_OK;
}

/**
  * @brief  Initializes the FSMC_PCCARD IO space Timing according to the specified
  *         parameters in the FSMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure  
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_PCCARD_IOSpace_Timing_Init(FSMC_PCCARD_TypeDef *Device, FSMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */  
  assert_param(IS_FSMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set FSMC_PCCARD device timing parameters */
  Device->PIO4 = (uint32_t)((Timing->SetupTime                  |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             );   
  
  return HAL_OK;
}
                                           
/**
  * @brief  DeInitializes the FSMC_PCCARD device 
  * @param  Device: Pointer to PCCARD device instance
  * @retval HAL status
  */
HAL_StatusTypeDef FSMC_PCCARD_DeInit(FSMC_PCCARD_TypeDef *Device)
{
  /* Disable the FSMC_PCCARD device */
  __FSMC_PCCARD_DISABLE(Device);
  
  /* De-initialize the FSMC_PCCARD device */
  Device->PCR4    = 0x00000018; 
  Device->SR4     = 0x00000000;	
  Device->PMEM4   = 0xFCFCFCFC;
  Device->PATT4   = 0xFCFCFCFC;
  Device->PIO4    = 0xFCFCFCFC;
  
  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx */

#endif /* HAL_FSMC_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
