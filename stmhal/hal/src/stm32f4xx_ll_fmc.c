/**
  ******************************************************************************
  * @file    stm32f4xx_ll_fmc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   FMC Low Layer HAL module driver.
  *    
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Flexible Memory Controller (FMC) peripheral memories:
  *           + Initialization/de-initialization functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                        ##### FMC peripheral features #####
  ==============================================================================
  [..] The Flexible memory controller (FMC) includes three memory controllers:
       (+) The NOR/PSRAM memory controller
       (+) The NAND/PC Card memory controller
       (+) The Synchronous DRAM (SDRAM) controller 
       
  [..] The FMC functional block makes the interface with synchronous and asynchronous static
       memories, SDRAM memories, and 16-bit PC memory cards. Its main purposes are:
       (+) to translate AHB transactions into the appropriate external device protocol
       (+) to meet the access time requirements of the external memory devices
   
  [..] All external memories share the addresses, data and control signals with the controller.
       Each external device is accessed by means of a unique Chip Select. The FMC performs
       only one access at a time to an external device.
       The main features of the FMC controller are the following:
        (+) Interface with static-memory mapped devices including:
           (++) Static random access memory (SRAM)
           (++) Read-only memory (ROM)
           (++) NOR Flash memory/OneNAND Flash memory
           (++) PSRAM (4 memory banks)
           (++) 16-bit PC Card compatible devices
           (++) Two banks of NAND Flash memory with ECC hardware to check up to 8 Kbytes of
                data
        (+) Interface with synchronous DRAM (SDRAM) memories
        (+) Independent Chip Select control for each memory bank
        (+) Independent configuration for each memory bank
                    
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

/** @defgroup FMC 
  * @brief FMC driver modules
  * @{
  */

#if defined (HAL_SRAM_MODULE_ENABLED) || defined(HAL_NOR_MODULE_ENABLED) || defined(HAL_NAND_MODULE_ENABLED) || defined(HAL_PCCARD_MODULE_ENABLED) || defined(HAL_SDRAM_MODULE_ENABLED)

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FMC_Private_Functions
  * @{
  */

/** @defgroup FMC_NORSRAM Controller functions
  * @brief    NORSRAM Controller functions 
  *
  @verbatim 
  ==============================================================================   
                   ##### How to use NORSRAM device driver #####
  ==============================================================================
 
  [..] 
    This driver contains a set of APIs to interface with the FMC NORSRAM banks in order
    to run the NORSRAM external devices.
      
    (+) FMC NORSRAM bank reset using the function FMC_NORSRAM_DeInit() 
    (+) FMC NORSRAM bank control configuration using the function FMC_NORSRAM_Init()
    (+) FMC NORSRAM bank timing configuration using the function FMC_NORSRAM_Timing_Init()
    (+) FMC NORSRAM bank extended timing configuration using the function 
        FMC_NORSRAM_Extended_Timing_Init()
    (+) FMC NORSRAM bank enable/disable write operation using the functions
        FMC_NORSRAM_WriteOperation_Enable()/FMC_NORSRAM_WriteOperation_Disable()
        

@endverbatim
  * @{
  */
       
/** @defgroup HAL_FMC_NORSRAM_Group1 Initialization/de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FMC NORSRAM interface
    (+) De-initialize the FMC NORSRAM interface 
    (+) Configure the FMC clock and associated GPIOs    
 
@endverbatim
  * @{
  */
  
/**
  * @brief  Initialize the FMC_NORSRAM device according to the specified
  *         control parameters in the FMC_NORSRAM_InitTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Init: Pointer to NORSRAM Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef* Init)
{ 
  uint32_t tmpr = 0;
    
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Init->NSBank));
  assert_param(IS_FMC_MUX(Init->DataAddressMux));
  assert_param(IS_FMC_MEMORY(Init->MemoryType));
  assert_param(IS_FMC_NORSRAM_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FMC_BURSTMODE(Init->BurstAccessMode));
  assert_param(IS_FMC_WAIT_POLARITY(Init->WaitSignalPolarity));
  assert_param(IS_FMC_WRAP_MODE(Init->WrapMode));
  assert_param(IS_FMC_WAIT_SIGNAL_ACTIVE(Init->WaitSignalActive));
  assert_param(IS_FMC_WRITE_OPERATION(Init->WriteOperation));
  assert_param(IS_FMC_WAITE_SIGNAL(Init->WaitSignal));
  assert_param(IS_FMC_EXTENDED_MODE(Init->ExtendedMode));
  assert_param(IS_FMC_ASYNWAIT(Init->AsynchronousWait));
  assert_param(IS_FMC_WRITE_BURST(Init->WriteBurst));
  assert_param(IS_FMC_CONTINOUS_CLOCK(Init->ContinuousClock)); 
  
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
                    Init->WriteBurst           |\
                    Init->ContinuousClock
                    );
                    
  if(Init->MemoryType == FMC_MEMORY_TYPE_NOR)
  {
    tmpr |= (uint32_t)FMC_NORSRAM_FLASH_ACCESS_ENABLE;
  }
  
  Device->BTCR[Init->NSBank] = tmpr;

  /* Configure synchronous mode when Continuous clock is enabled for bank2..4 */
  if((Init->ContinuousClock == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC) && (Init->NSBank != FMC_NORSRAM_BANK1))
  { 
    Init->BurstAccessMode = FMC_BURST_ACCESS_MODE_ENABLE; 
    Device->BTCR[FMC_NORSRAM_BANK1] |= (uint32_t)(Init->BurstAccessMode  |\
                                                  Init->ContinuousClock);                    
  }                       
  
  return HAL_OK;
}


/**
  * @brief  DeInitialize the FMC_NORSRAM peripheral 
  * @param  Device: Pointer to NORSRAM device instance
  * @param  ExDevice: Pointer to NORSRAM extended mode device instance  
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_EXTENDED_DEVICE(ExDevice));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));
  
  /* Disable the FMC_NORSRAM device */
  __FMC_NORSRAM_DISABLE(Device, Bank);
  
  /* De-initialize the FMC_NORSRAM device */
  /* FMC_NORSRAM_BANK1 */
  if(Bank == FMC_NORSRAM_BANK1)
  {
    Device->BTCR[Bank] = 0x000030DB;    
  }
  /* FMC_NORSRAM_BANK2, FMC_NORSRAM_BANK3 or FMC_NORSRAM_BANK4 */
  else
  {   
    Device->BTCR[Bank] = 0x000030D2; 
  }
  
  Device->BTCR[Bank + 1] = 0x0FFFFFFF;
  ExDevice->BWTR[Bank]   = 0x0FFFFFFF;
   
  return HAL_OK;
}


/**
  * @brief  Initialize the FMC_NORSRAM Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmpr = 0;
  
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
  assert_param(IS_FMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
  assert_param(IS_FMC_DATASETUP_TIME(Timing->DataSetupTime));
  assert_param(IS_FMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
  assert_param(IS_FMC_CLK_DIV(Timing->CLKDivision));
  assert_param(IS_FMC_DATA_LATENCY(Timing->DataLatency));
  assert_param(IS_FMC_ACCESS_MODE(Timing->AccessMode));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));
  
  /* Set FMC_NORSRAM device timing parameters */  
  tmpr = (uint32_t)(Timing->AddressSetupTime                  |\
                   ((Timing->AddressHoldTime) << 4)          |\
                   ((Timing->DataSetupTime) << 8)            |\
                   ((Timing->BusTurnAroundDuration) << 16)   |\
                   (((Timing->CLKDivision)-1) << 20)         |\
                   (((Timing->DataLatency)-2) << 24)         |\
                    (Timing->AccessMode)
                    );
  
  Device->BTCR[Bank + 1] = tmpr;
  
  /* Configure Clock division value (in NORSRAM bank 1) when continuous clock is enabled */
  if(HAL_IS_BIT_SET(Device->BTCR[FMC_NORSRAM_BANK1], FMC_BCR1_CCLKEN))
  {
    tmpr = (uint32_t)(Device->BTCR[FMC_NORSRAM_BANK1 + 1] & ~(((uint32_t)0x0F) << 20)); 
    tmpr |= (uint32_t)(((Timing->CLKDivision)-1) << 20);
    Device->BTCR[FMC_NORSRAM_BANK1 + 1] = tmpr;
  }  
  
  return HAL_OK;   
}

/**
  * @brief  Initialize the FMC_NORSRAM Extended mode Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef  FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode)
{  
  /* Check the parameters */
  assert_param(IS_FMC_EXTENDED_MODE(ExtendedMode));
  
  /* Set NORSRAM device timing register for write configuration, if extended mode is used */
  if(ExtendedMode == FMC_EXTENDED_MODE_ENABLE)
  {
    /* Check the parameters */  
    assert_param(IS_FMC_NORSRAM_EXTENDED_DEVICE(Device));  
    assert_param(IS_FMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
    assert_param(IS_FMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
    assert_param(IS_FMC_DATASETUP_TIME(Timing->DataSetupTime));
    assert_param(IS_FMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
    assert_param(IS_FMC_CLK_DIV(Timing->CLKDivision));
    assert_param(IS_FMC_DATA_LATENCY(Timing->DataLatency));
    assert_param(IS_FMC_ACCESS_MODE(Timing->AccessMode));
    assert_param(IS_FMC_NORSRAM_BANK(Bank));  
    
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
  
  
/** @defgroup HAL_FMC_NORSRAM_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                      ##### FMC_NORSRAM Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the FMC NORSRAM interface.

@endverbatim
  * @{
  */
    
/**
  * @brief  Enables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));
  
  /* Enable write operation */
  Device->BTCR[Bank] |= FMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{ 
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));
    
  /* Disable write operation */
  Device->BTCR[Bank] &= ~FMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}

/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup FMC_PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
  @verbatim 
  ==============================================================================
                    ##### How to use NAND device driver #####
  ==============================================================================
  [..]
    This driver contains a set of APIs to interface with the FMC NAND banks in order
    to run the NAND external devices.
  
    (+) FMC NAND bank reset using the function FMC_NAND_DeInit() 
    (+) FMC NAND bank control configuration using the function FMC_NAND_Init()
    (+) FMC NAND bank common space timing configuration using the function 
        FMC_NAND_CommonSpace_Timing_Init()
    (+) FMC NAND bank attribute space timing configuration using the function 
        FMC_NAND_AttributeSpace_Timing_Init()
    (+) FMC NAND bank enable/disable ECC correction feature using the functions
        FMC_NAND_ECC_Enable()/FMC_NAND_ECC_Disable()
    (+) FMC NAND bank get ECC correction code using the function FMC_NAND_GetECC()    

@endverbatim
  * @{
  */
    
/** @defgroup HAL_FMC_NAND_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FMC NAND interface
    (+) De-initialize the FMC NAND interface 
    (+) Configure the FMC clock and associated GPIOs
        
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the FMC_NAND device according to the specified
  *         control parameters in the FMC_NAND_HandleTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Init: Pointer to NAND Initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_Init(FMC_NAND_TypeDef *Device, FMC_NAND_InitTypeDef *Init)
{
  uint32_t tmppcr  = 0; 
    
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_NAND_BANK(Init->NandBank));
  assert_param(IS_FMC_WAIT_FEATURE(Init->Waitfeature));
  assert_param(IS_FMC_NAND_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FMC_ECC_STATE(Init->EccComputation));
  assert_param(IS_FMC_ECCPAGE_SIZE(Init->ECCPageSize));
  assert_param(IS_FMC_TCLR_TIME(Init->TCLRSetupTime));
  assert_param(IS_FMC_TAR_TIME(Init->TARSetupTime));   

  /* Set NAND device control parameters */
  tmppcr = (uint32_t)(Init->Waitfeature                |\
                      FMC_PCR_MEMORY_TYPE_NAND         |\
                      Init->MemoryDataWidth            |\
                      Init->EccComputation             |\
                      Init->ECCPageSize                |\
                      ((Init->TCLRSetupTime) << 9)     |\
                      ((Init->TARSetupTime) << 13)
                      );   
  
  if(Init->NandBank == FMC_NAND_BANK2)
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
  * @brief  Initializes the FMC_NAND Common space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_CommonSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmppmem = 0;  
  
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));
  assert_param(IS_FMC_NAND_BANK(Bank));
  
  /* Set FMC_NAND device timing parameters */
  tmppmem = (uint32_t)(Timing->SetupTime                  |\
                       ((Timing->WaitSetupTime) << 8)     |\
                       ((Timing->HoldSetupTime) << 16)    |\
                       ((Timing->HiZSetupTime) << 24)
                       );
                            
  if(Bank == FMC_NAND_BANK2)
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
  * @brief  Initializes the FMC_NAND Attribute space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number 
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_AttributeSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmppatt = 0;  
  
  /* Check the parameters */ 
  assert_param(IS_FMC_NAND_DEVICE(Device)); 
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));
  assert_param(IS_FMC_NAND_BANK(Bank));
  
  /* Set FMC_NAND device timing parameters */
  tmppatt = (uint32_t)(Timing->SetupTime                  |\
                       ((Timing->WaitSetupTime) << 8)     |\
                       ((Timing->HoldSetupTime) << 16)    |\
                       ((Timing->HiZSetupTime) << 24)
                       );
                       
  if(Bank == FMC_NAND_BANK2)
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
  * @brief  DeInitializes the FMC_NAND device 
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_DeInit(FMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */ 
  assert_param(IS_FMC_NAND_DEVICE(Device)); 
  assert_param(IS_FMC_NAND_BANK(Bank));
      
  /* Disable the NAND Bank */
  __FMC_NAND_DISABLE(Device, Bank);
 
  /* De-initialize the NAND Bank */
  if(Bank == FMC_NAND_BANK2)
  {
    /* Set the FMC_NAND_BANK2 registers to their reset values */
    Device->PCR2  = 0x00000018;
    Device->SR2   = 0x00000040;
    Device->PMEM2 = 0xFCFCFCFC;
    Device->PATT2 = 0xFCFCFCFC;  
  }
  /* FMC_Bank3_NAND */  
  else
  {
    /* Set the FMC_NAND_BANK3 registers to their reset values */
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
  
  
/** @defgroup HAL_FMC_NAND_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                       ##### FMC_NAND Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the FMC NAND interface.

@endverbatim
  * @{
  */ 

    
/**
  * @brief  Enables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */    
HAL_StatusTypeDef FMC_NAND_ECC_Enable(FMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */ 
  assert_param(IS_FMC_NAND_DEVICE(Device)); 
  assert_param(IS_FMC_NAND_BANK(Bank));
    
  /* Enable ECC feature */
  if(Bank == FMC_NAND_BANK2)
  {
    Device->PCR2 |= FMC_PCR2_ECCEN;
  }
  else
  {
    Device->PCR3 |= FMC_PCR3_ECCEN;
  } 
  
  return HAL_OK;  
}


/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */  
HAL_StatusTypeDef FMC_NAND_ECC_Disable(FMC_NAND_TypeDef *Device, uint32_t Bank)  
{  
  /* Check the parameters */ 
  assert_param(IS_FMC_NAND_DEVICE(Device)); 
  assert_param(IS_FMC_NAND_BANK(Bank));
    
  /* Disable ECC feature */
  if(Bank == FMC_NAND_BANK2)
  {
    Device->PCR2 &= ~FMC_PCR2_ECCEN;
  }
  else
  {
    Device->PCR3 &= ~FMC_PCR3_ECCEN;
  } 

  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  ECCval: Pointer to ECC value
  * @param  Bank: NAND bank number
  * @param  Timeout: Timeout wait value  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_GetECC(FMC_NAND_TypeDef *Device, uint32_t *ECCval, uint32_t Bank, uint32_t Timeout)
{
  uint32_t timeout = 0;

  /* Check the parameters */ 
  assert_param(IS_FMC_NAND_DEVICE(Device)); 
  assert_param(IS_FMC_NAND_BANK(Bank));
      
  timeout = HAL_GetTick() + Timeout;
  
  /* Wait untill FIFO is empty */
  while(__FMC_NAND_GET_FLAG(Device, Bank, FMC_FLAG_FEMPT))
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
     
  if(Bank == FMC_NAND_BANK2)
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
    
/** @defgroup FMC_PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
  @verbatim 
  ==============================================================================  
                    ##### How to use PCCARD device driver #####
  ==============================================================================
  [..]
    This driver contains a set of APIs to interface with the FMC PCCARD bank in order
    to run the PCCARD/compact flash external devices.
  
    (+) FMC PCCARD bank reset using the function FMC_PCCARD_DeInit() 
    (+) FMC PCCARD bank control configuration using the function FMC_PCCARD_Init()
    (+) FMC PCCARD bank common space timing configuration using the function 
        FMC_PCCARD_CommonSpace_Timing_Init()
    (+) FMC PCCARD bank attribute space timing configuration using the function 
        FMC_PCCARD_AttributeSpace_Timing_Init()
    (+) FMC PCCARD bank IO space timing configuration using the function 
        FMC_PCCARD_IOSpace_Timing_Init()

       
@endverbatim
  * @{
  */
  
/** @defgroup HAL_FMC_PCCARD_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FMC PCCARD interface
    (+) De-initialize the FMC PCCARD interface 
    (+) Configure the FMC clock and associated GPIOs
        
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the FMC_PCCARD device according to the specified
  *         control parameters in the FMC_PCCARD_HandleTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Init: Pointer to PCCARD Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_PCCARD_Init(FMC_PCCARD_TypeDef *Device, FMC_PCCARD_InitTypeDef *Init)
{
  /* Check the parameters */ 
  assert_param(IS_FMC_PCCARD_DEVICE(Device));
  assert_param(IS_FMC_WAIT_FEATURE(Init->Waitfeature));
  assert_param(IS_FMC_TCLR_TIME(Init->TCLRSetupTime));
  assert_param(IS_FMC_TAR_TIME(Init->TARSetupTime));     
  
  /* Set FMC_PCCARD device control parameters */
  Device->PCR4 = (uint32_t)(Init->Waitfeature               |\
                            FMC_NAND_PCC_MEM_BUS_WIDTH_16   |\
                            (Init->TCLRSetupTime << 9)      |\
                            (Init->TARSetupTime << 13));
  
  return HAL_OK;

}

/**
  * @brief  Initializes the FMC_PCCARD Common space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure 
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_PCCARD_CommonSpace_Timing_Init(FMC_PCCARD_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */
  assert_param(IS_FMC_PCCARD_DEVICE(Device));
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  Device->PMEM4 = (uint32_t)((Timing->SetupTime                 |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             ); 

  return HAL_OK;  
}

/**
  * @brief  Initializes the FMC_PCCARD Attribute space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_PCCARD_AttributeSpace_Timing_Init(FMC_PCCARD_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */ 
  assert_param(IS_FMC_PCCARD_DEVICE(Device)); 
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set PCCARD timing parameters */
  Device->PATT4 = (uint32_t)((Timing->SetupTime                 |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             );  
                                        
  return HAL_OK;
}

/**
  * @brief  Initializes the FMC_PCCARD IO space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to PCCARD device instance
  * @param  Timing: Pointer to PCCARD timing structure  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_PCCARD_IOSpace_Timing_Init(FMC_PCCARD_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing)
{
  /* Check the parameters */  
  assert_param(IS_FMC_PCCARD_DEVICE(Device));
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));

  /* Set FMC_PCCARD device timing parameters */
  Device->PIO4 = (uint32_t)((Timing->SetupTime                  |\
                             ((Timing->WaitSetupTime) << 8)     |\
                              (Timing->HoldSetupTime) << 16)    |\
                              ((Timing->HiZSetupTime) << 24)
                             );   
  
  return HAL_OK;
}
                                           
/**
  * @brief  DeInitializes the FMC_PCCARD device 
  * @param  Device: Pointer to PCCARD device instance
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_PCCARD_DeInit(FMC_PCCARD_TypeDef *Device)
{
  /* Check the parameters */  
  assert_param(IS_FMC_PCCARD_DEVICE(Device));
    
  /* Disable the FMC_PCCARD device */
  __FMC_PCCARD_DISABLE(Device);
  
  /* De-initialize the FMC_PCCARD device */
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


/** @defgroup FMC_SDRAM Controller functions
  * @brief    SDRAM Controller functions 
  *
  @verbatim 
  ==============================================================================
                     ##### How to use SDRAM device driver #####
  ==============================================================================
  [..] 
    This driver contains a set of APIs to interface with the FMC SDRAM banks in order
    to run the SDRAM external devices.
    
    (+) FMC SDRAM bank reset using the function FMC_SDRAM_DeInit() 
    (+) FMC SDRAM bank control configuration using the function FMC_SDRAM_Init()
    (+) FMC SDRAM bank timing configuration using the function FMC_SDRAM_Timing_Init()
    (+) FMC SDRAM bank enable/disable write operation using the functions
        FMC_SDRAM_WriteOperation_Enable()/FMC_SDRAM_WriteOperation_Disable()   
    (+) FMC SDRAM bank send command using the function FMC_SDRAM_SendCommand()      
       
@endverbatim
  * @{
  */
         
/** @defgroup HAL_FMC_SDRAM_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize and configure the FMC SDRAM interface
    (+) De-initialize the FMC SDRAM interface 
    (+) Configure the FMC clock and associated GPIOs
        
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the FMC_SDRAM device according to the specified
  *         control parameters in the FMC_SDRAM_InitTypeDef
  * @param  Device: Pointer to SDRAM device instance
  * @param  Init: Pointer to SDRAM Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_Init(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_InitTypeDef *Init)
{
  uint32_t tmpr1 = 0;
  uint32_t tmpr2 = 0;
    
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Init->SDBank));
  assert_param(IS_FMC_COLUMNBITS_NUMBER(Init->ColumnBitsNumber));
  assert_param(IS_FMC_ROWBITS_NUMBER(Init->RowBitsNumber));
  assert_param(IS_FMC_SDMEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FMC_INTERNALBANK_NUMBER(Init->InternalBankNumber));
  assert_param(IS_FMC_CAS_LATENCY(Init->CASLatency));
  assert_param(IS_FMC_WRITE_PROTECTION(Init->WriteProtection));
  assert_param(IS_FMC_SDCLOCK_PERIOD(Init->SDClockPeriod));
  assert_param(IS_FMC_READ_BURST(Init->ReadBurst));
  assert_param(IS_FMC_READPIPE_DELAY(Init->ReadPipeDelay));   

  /* Set SDRAM bank configuration parameters */
  if (Init->SDBank != FMC_SDRAM_BANK2) 
  {                                      
    Device->SDCR[FMC_SDRAM_BANK1] = (uint32_t)(Init->ColumnBitsNumber   |\
                                               Init->RowBitsNumber      |\
                                               Init->MemoryDataWidth    |\
                                               Init->InternalBankNumber |\
                                               Init->CASLatency         |\
                                               Init->WriteProtection    |\
                                               Init->SDClockPeriod      |\
                                               Init->ReadBurst          |\
                                               Init->ReadPipeDelay
                                               );                                      
  }
  else /* FMC_Bank2_SDRAM */                      
  {
    tmpr1 = (uint32_t)(Init->SDClockPeriod      |\
                       Init->ReadBurst          |\
                       Init->ReadPipeDelay
                       );  

    tmpr2 = (uint32_t)(Init->ColumnBitsNumber   |\
                       Init->RowBitsNumber      |\
                       Init->MemoryDataWidth    |\
                       Init->InternalBankNumber |\
                       Init->CASLatency         |\
                       Init->WriteProtection
                       ); 
  
    Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
    Device->SDCR[FMC_SDRAM_BANK2] = tmpr2;
  }  
  
  return HAL_OK;
}

/**
  * @brief  Initializes the FMC_SDRAM device timing according to the specified
  *         parameters in the FMC_SDRAM_TimingTypeDef
  * @param  Device: Pointer to SDRAM device instance
  * @param  Timing: Pointer to SDRAM Timing structure
  * @param  Bank: SDRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_Timing_Init(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmpr1 = 0;
  uint32_t tmpr2 = 0;
    
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_LOADTOACTIVE_DELAY(Timing->LoadToActiveDelay));
  assert_param(IS_FMC_EXITSELFREFRESH_DELAY(Timing->ExitSelfRefreshDelay));
  assert_param(IS_FMC_SELFREFRESH_TIME(Timing->SelfRefreshTime));
  assert_param(IS_FMC_ROWCYCLE_DELAY(Timing->RowCycleDelay));
  assert_param(IS_FMC_WRITE_RECOVERY_TIME(Timing->WriteRecoveryTime));
  assert_param(IS_FMC_RP_DELAY(Timing->RPDelay));
  assert_param(IS_FMC_RCD_DELAY(Timing->RCDDelay));
  assert_param(IS_FMC_SDRAM_BANK(Bank));
  
  /* Set SDRAM device timing parameters */ 
  if (Bank != FMC_SDRAM_BANK2) 
  {                                         
    Device->SDTR[FMC_SDRAM_BANK1] = (uint32_t)(((Timing->LoadToActiveDelay)-1)           |\
                                               (((Timing->ExitSelfRefreshDelay)-1) << 4) |\
                                               (((Timing->SelfRefreshTime)-1) << 8)      |\
                                               (((Timing->RowCycleDelay)-1) << 12)       |\
                                               (((Timing->WriteRecoveryTime)-1) <<16)    |\
                                               (((Timing->RPDelay)-1) << 20)             |\
                                               (((Timing->RCDDelay)-1) << 24)
                                               );
  }
  else /* FMC_Bank2_SDRAM */
  {  

    tmpr1 = (uint32_t)(((Timing->LoadToActiveDelay)-1)           |\
                       (((Timing->ExitSelfRefreshDelay)-1) << 4) |\
                       (((Timing->SelfRefreshTime)-1) << 8)      |\
                       (((Timing->WriteRecoveryTime)-1) <<16)    |\
                       (((Timing->RCDDelay)-1) << 24)
                       );   
                                                         
    tmpr2  = (uint32_t)((((Timing->RowCycleDelay)-1) << 12)       |\
                        (((Timing->RPDelay)-1) << 20)
                        ); 

    Device->SDTR[FMC_SDRAM_BANK2] = tmpr1;
    Device->SDTR[FMC_SDRAM_BANK1] = tmpr2;
  }   
  
  return HAL_OK;
}

/**
  * @brief  DeInitializes the FMC_SDRAM peripheral 
  * @param  Device: Pointer to SDRAM device instance
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_DeInit(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));
  
  /* De-initialize the SDRAM device */
  Device->SDCR[Bank] = 0x000002D0;
  Device->SDTR[Bank] = 0x0FFFFFFF;    
  Device->SDCMR      = 0x00000000;
  Device->SDRTR      = 0x00000000;
  Device->SDSR       = 0x00000000;

  return HAL_OK;
}

/**
  * @}
  */

  
/** @defgroup HAL_FMC_SDRAM_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                      ##### FMC_SDRAM Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the FMC SDRAM interface.

@endverbatim
  * @{
  */

/**
  * @brief  Enables dynamically FMC_SDRAM write protection.
  * @param  Device: Pointer to SDRAM device instance
  * @param  Bank: SDRAM bank number 
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_WriteProtection_Enable(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{ 
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));
  
  /* Enable write protection */
  Device->SDCR[Bank] |= FMC_SDRAM_WRITE_PROTECTION_ENABLE;
  
  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FMC_SDRAM write protection.
  * @param  hsdram: FMC_SDRAM handle
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_WriteProtection_Disable(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));
  
  /* Disable write protection */
  Device->SDCR[Bank] &= ~FMC_SDRAM_WRITE_PROTECTION_ENABLE;
  
  return HAL_OK;
}
  
/**
  * @brief  Send Command to the FMC SDRAM bank
  * @param  Device: Pointer to SDRAM device instance
  * @param  Command: Pointer to SDRAM command structure   
  * @param  Timing: Pointer to SDRAM Timing structure
  * @param  Timeout: Timeout wait value
  * @retval HAL state
  */  
HAL_StatusTypeDef FMC_SDRAM_SendCommand(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_CommandTypeDef *Command, uint32_t Timeout)
{
  __IO uint32_t tmpr = 0;
  uint32_t timeout = 0;
  
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_COMMAND_MODE(Command->CommandMode));
  assert_param(IS_FMC_COMMAND_TARGET(Command->CommandTarget));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(Command->AutoRefreshNumber));
  assert_param(IS_FMC_MODE_REGISTER(Command->ModeRegisterDefinition));  

  /* Set command register */
  tmpr = (uint32_t)((Command->CommandMode)                  |\
                    (Command->CommandTarget)                |\
                    (((Command->AutoRefreshNumber)-1) << 5) |\
                    ((Command->ModeRegisterDefinition) << 9)     
                    );
    
  Device->SDCMR = tmpr;
   
  timeout = HAL_GetTick() + Timeout;

  /* wait until command is send */
  while(HAL_IS_BIT_SET(Device->SDSR, FMC_SDSR_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        return HAL_TIMEOUT;
      }
    }     
    
    return HAL_ERROR;
  }
  
  return HAL_OK;  
}

/**
  * @brief  Program the SDRAM Memory Refresh rate.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  RefreshRate: The SDRAM refresh rate value.       
  * @retval HAL state
  */
HAL_StatusTypeDef FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_TypeDef *Device, uint32_t RefreshRate)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_REFRESH_RATE(RefreshRate));
  
  /* Set the refresh rate in command register */
  Device->SDRTR |= (RefreshRate<<1);
  
  return HAL_OK;   
}

/**
  * @brief  Set the Number of consecutive SDRAM Memory auto Refresh commands.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  AutoRefreshNumber: Specifies the auto Refresh number.       
  * @retval None
  */
HAL_StatusTypeDef FMC_SDRAM_SetAutoRefreshNumber(FMC_SDRAM_TypeDef *Device, uint32_t AutoRefreshNumber)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(AutoRefreshNumber));
  
  /* Set the Auto-refresh number in command register */
  Device->SDCMR |= (AutoRefreshNumber << 5); 

  return HAL_OK;  
}

/**
  * @brief  Returns the indicated FMC SDRAM bank mode status.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  Bank: Defines the FMC SDRAM bank. This parameter can be 
  *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM. 
  * @retval The FMC SDRAM bank mode status, could be on of the following values:
  *         FMC_SDRAM_NORMAL_MODE, FMC_SDRAM_SELF_REFRESH_MODE or 
  *         FMC_SDRAM_POWER_DOWN_MODE.           
  */
uint32_t FMC_SDRAM_GetModeStatus(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));

  /* Get the corresponding bank mode */
  if(Bank == FMC_SDRAM_BANK1)
  {
    tmpreg = (uint32_t)(Device->SDSR & FMC_SDSR_MODES1); 
  }
  else
  {
    tmpreg = ((uint32_t)(Device->SDSR & FMC_SDSR_MODES2) >> 2);
  }
  
  /* Return the mode status */
  return tmpreg;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */

#endif /* HAL_FMC_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
