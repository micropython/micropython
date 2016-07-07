/**
  ******************************************************************************
  * @file    stm32f4xx_fmc.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    19-September-2013
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the FMC peripheral:           
  *           + Interface with SRAM, PSRAM, NOR and OneNAND memories
  *           + Interface with NAND memories
  *           + Interface with 16-bit PC Card compatible memories 
  *           + Interface with SDRAM memories    
  *           + Interrupts and flags management   
  *           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "stm32f4xx_fmc.h"
//#include "stm32f4xx_rcc.h"

#if CH_KERNEL_MAJOR == 3
	#define assert_param(expr) chDbgAssert(expr,"STPeriph FMC")
#else
	#define assert_param(expr) chDbgAssert(expr,"STPeriph FMC","")
#endif

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FMC 
  * @brief FMC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* --------------------- FMC registers bit mask ---------------------------- */
/* FMC BCRx Mask */
#define BCR_MBKEN_SET              ((uint32_t)0x00000001)
#define BCR_MBKEN_RESET            ((uint32_t)0x000FFFFE)
#define BCR_FACCEN_SET             ((uint32_t)0x00000040)

/* FMC PCRx Mask */
#define PCR_PBKEN_SET              ((uint32_t)0x00000004)
#define PCR_PBKEN_RESET            ((uint32_t)0x000FFFFB)
#define PCR_ECCEN_SET              ((uint32_t)0x00000040)
#define PCR_ECCEN_RESET            ((uint32_t)0x000FFFBF)
#define PCR_MEMORYTYPE_NAND        ((uint32_t)0x00000008)

/* FMC SDCRx write protection Mask*/
#define SDCR_WriteProtection_RESET ((uint32_t)0x00007DFF) 

/* FMC SDCMR Mask*/
#define SDCMR_CTB1_RESET           ((uint32_t)0x003FFFEF)
#define SDCMR_CTB2_RESET           ((uint32_t)0x003FFFF7)
#define SDCMR_CTB1_2_RESET         ((uint32_t)0x003FFFE7)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FMC_Private_Functions
  * @{
  */

/** @defgroup FMC_Group1 NOR/SRAM Controller functions
  * @brief    NOR/SRAM Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### NOR and SRAM Controller functions #####
 ===============================================================================  

 [..] The following sequence should be followed to configure the FMC to interface
      with SRAM, PSRAM, NOR or OneNAND memory connected to the NOR/SRAM Bank:
 
   (#) Enable the clock for the FMC and associated GPIOs using the following functions:
          RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

   (#) FMC pins configuration 
       (++) Connect the involved FMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
       (++) Configure these FMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
   (#) Declare a FMC_NORSRAMInitTypeDef structure, for example:
          FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
      and fill the FMC_NORSRAMInitStructure variable with the allowed values of
      the structure member.
      
   (#) Initialize the NOR/SRAM Controller by calling the function
          FMC_NORSRAMInit(&FMC_NORSRAMInitStructure); 

   (#) Then enable the NOR/SRAM Bank, for example:
          FMC_NORSRAMCmd(FMC_Bank1_NORSRAM2, ENABLE);  

   (#) At this stage you can read/write from/to the memory connected to the NOR/SRAM Bank. 
   
@endverbatim
  * @{
  */

/**
  * @brief  De-initializes the FMC NOR/SRAM Banks registers to their default 
  *   reset values.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank1_NORSRAM1: FMC Bank1 NOR/SRAM1  
  *            @arg FMC_Bank1_NORSRAM2: FMC Bank1 NOR/SRAM2 
  *            @arg FMC_Bank1_NORSRAM3: FMC Bank1 NOR/SRAM3 
  *            @arg FMC_Bank1_NORSRAM4: FMC Bank1 NOR/SRAM4 
  * @retval None
  */
void FMC_NORSRAMDeInit(uint32_t FMC_Bank)
{
  /* Check the parameter */
  assert_param(IS_FMC_NORSRAM_BANK(FMC_Bank));
  
  /* FMC_Bank1_NORSRAM1 */
  if(FMC_Bank == FMC_Bank1_NORSRAM1)
  {
    FMC_Bank1->BTCR[FMC_Bank] = 0x000030DB;    
  }
  /* FMC_Bank1_NORSRAM2,  FMC_Bank1_NORSRAM3 or FMC_Bank1_NORSRAM4 */
  else
  {   
    FMC_Bank1->BTCR[FMC_Bank] = 0x000030D2; 
  }
  FMC_Bank1->BTCR[FMC_Bank + 1] = 0x0FFFFFFF;
  FMC_Bank1E->BWTR[FMC_Bank] = 0x0FFFFFFF;  
}

/**
  * @brief  Initializes the FMC NOR/SRAM Banks according to the specified
  *         parameters in the FMC_NORSRAMInitStruct.
  * @param  FMC_NORSRAMInitStruct : pointer to a FMC_NORSRAMInitTypeDef structure
  *         that contains the configuration information for the FMC NOR/SRAM 
  *         specified Banks.                       
  * @retval None
  */
void FMC_NORSRAMInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct)
{
  uint32_t tmpr = 0;
  
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_BANK(FMC_NORSRAMInitStruct->FMC_Bank));
  assert_param(IS_FMC_MUX(FMC_NORSRAMInitStruct->FMC_DataAddressMux));
  assert_param(IS_FMC_MEMORY(FMC_NORSRAMInitStruct->FMC_MemoryType));
  assert_param(IS_FMC_NORSRAM_MEMORY_WIDTH(FMC_NORSRAMInitStruct->FMC_MemoryDataWidth));
  assert_param(IS_FMC_BURSTMODE(FMC_NORSRAMInitStruct->FMC_BurstAccessMode));
  assert_param(IS_FMC_WAIT_POLARITY(FMC_NORSRAMInitStruct->FMC_WaitSignalPolarity));
  assert_param(IS_FMC_WRAP_MODE(FMC_NORSRAMInitStruct->FMC_WrapMode));
  assert_param(IS_FMC_WAIT_SIGNAL_ACTIVE(FMC_NORSRAMInitStruct->FMC_WaitSignalActive));
  assert_param(IS_FMC_WRITE_OPERATION(FMC_NORSRAMInitStruct->FMC_WriteOperation));
  assert_param(IS_FMC_WAITE_SIGNAL(FMC_NORSRAMInitStruct->FMC_WaitSignal));
  assert_param(IS_FMC_EXTENDED_MODE(FMC_NORSRAMInitStruct->FMC_ExtendedMode));
  assert_param(IS_FMC_ASYNWAIT(FMC_NORSRAMInitStruct->FMC_AsynchronousWait));
  assert_param(IS_FMC_WRITE_BURST(FMC_NORSRAMInitStruct->FMC_WriteBurst));
  assert_param(IS_FMC_CONTINOUS_CLOCK(FMC_NORSRAMInitStruct->FMC_ContinousClock));  
  assert_param(IS_FMC_ADDRESS_SETUP_TIME(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressSetupTime));
  assert_param(IS_FMC_ADDRESS_HOLD_TIME(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressHoldTime));
  assert_param(IS_FMC_DATASETUP_TIME(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataSetupTime));
  assert_param(IS_FMC_TURNAROUND_TIME(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_BusTurnAroundDuration));
  assert_param(IS_FMC_CLK_DIV(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision));
  assert_param(IS_FMC_DATA_LATENCY(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataLatency));
  assert_param(IS_FMC_ACCESS_MODE(FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AccessMode)); 
  
  /* NOR/SRAM Bank control register configuration */ 
  FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank] =   
            (uint32_t)FMC_NORSRAMInitStruct->FMC_DataAddressMux |
            FMC_NORSRAMInitStruct->FMC_MemoryType |
            FMC_NORSRAMInitStruct->FMC_MemoryDataWidth |
            FMC_NORSRAMInitStruct->FMC_BurstAccessMode |
            FMC_NORSRAMInitStruct->FMC_WaitSignalPolarity |
            FMC_NORSRAMInitStruct->FMC_WrapMode |
            FMC_NORSRAMInitStruct->FMC_WaitSignalActive |
            FMC_NORSRAMInitStruct->FMC_WriteOperation |
            FMC_NORSRAMInitStruct->FMC_WaitSignal |
            FMC_NORSRAMInitStruct->FMC_ExtendedMode |
            FMC_NORSRAMInitStruct->FMC_AsynchronousWait |
            FMC_NORSRAMInitStruct->FMC_WriteBurst |
            FMC_NORSRAMInitStruct->FMC_ContinousClock;

            
  if(FMC_NORSRAMInitStruct->FMC_MemoryType == FMC_MemoryType_NOR)
  {
    FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank] |= (uint32_t)BCR_FACCEN_SET;
  }

  /* Configure Continuous clock feature when bank2..4 is used */
  if((FMC_NORSRAMInitStruct->FMC_ContinousClock == FMC_CClock_SyncAsync) && (FMC_NORSRAMInitStruct->FMC_Bank != FMC_Bank1_NORSRAM1))
  {
    tmpr = (uint32_t)((FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1+1]) & ~(((uint32_t)0x0F) << 20));    
    
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1]  |= FMC_NORSRAMInitStruct->FMC_ContinousClock;
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1]  |= FMC_BurstAccessMode_Enable;
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1+1] = (uint32_t)(tmpr | (((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision)-1) << 20));
  }
  
  /* NOR/SRAM Bank timing register configuration */
  FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank+1] =   
            (uint32_t)FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressSetupTime |
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressHoldTime << 4) |
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataSetupTime << 8) |
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_BusTurnAroundDuration << 16) |
            ((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision) << 20) |
            ((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataLatency) << 24) |
             FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AccessMode;
     
  /* NOR/SRAM Bank timing register for write configuration, if extended mode is used */
  if(FMC_NORSRAMInitStruct->FMC_ExtendedMode == FMC_ExtendedMode_Enable)
  {
    assert_param(IS_FMC_ADDRESS_SETUP_TIME(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressSetupTime));
    assert_param(IS_FMC_ADDRESS_HOLD_TIME(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressHoldTime));
    assert_param(IS_FMC_DATASETUP_TIME(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataSetupTime));
    assert_param(IS_FMC_CLK_DIV(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_CLKDivision));
    assert_param(IS_FMC_DATA_LATENCY(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataLatency));
    assert_param(IS_FMC_ACCESS_MODE(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AccessMode));
    
    FMC_Bank1E->BWTR[FMC_NORSRAMInitStruct->FMC_Bank] =   
               (uint32_t)FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressSetupTime |
               (FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressHoldTime << 4 )|
               (FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataSetupTime << 8) |
               ((FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_CLKDivision) << 20) |
               ((FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataLatency) << 24) |
               FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AccessMode;
  }
  else
  {
    FMC_Bank1E->BWTR[FMC_NORSRAMInitStruct->FMC_Bank] = 0x0FFFFFFF;
  }
  
}

/**
  * @brief  Fills each FMC_NORSRAMInitStruct member with its default value.
  * @param  FMC_NORSRAMInitStruct: pointer to a FMC_NORSRAMInitTypeDef structure 
  *         which will be initialized.
  * @retval None
  */
void FMC_NORSRAMStructInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct)
{  
  /* Reset NOR/SRAM Init structure parameters values */
  FMC_NORSRAMInitStruct->FMC_Bank = FMC_Bank1_NORSRAM1;
  FMC_NORSRAMInitStruct->FMC_DataAddressMux = FMC_DataAddressMux_Enable;
  FMC_NORSRAMInitStruct->FMC_MemoryType = FMC_MemoryType_SRAM;
  FMC_NORSRAMInitStruct->FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
  FMC_NORSRAMInitStruct->FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
  FMC_NORSRAMInitStruct->FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;
  FMC_NORSRAMInitStruct->FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
  FMC_NORSRAMInitStruct->FMC_WrapMode = FMC_WrapMode_Disable;
  FMC_NORSRAMInitStruct->FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
  FMC_NORSRAMInitStruct->FMC_WriteOperation = FMC_WriteOperation_Enable;
  FMC_NORSRAMInitStruct->FMC_WaitSignal = FMC_WaitSignal_Enable;
  FMC_NORSRAMInitStruct->FMC_ExtendedMode = FMC_ExtendedMode_Disable;
  FMC_NORSRAMInitStruct->FMC_WriteBurst = FMC_WriteBurst_Disable;
  FMC_NORSRAMInitStruct->FMC_ContinousClock = FMC_CClock_SyncOnly;
  
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressSetupTime = 15;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressHoldTime = 15;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataSetupTime = 255;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_BusTurnAroundDuration = 15;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision = 15;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataLatency = 15;
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AccessMode = FMC_AccessMode_A; 
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressSetupTime = 15;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressHoldTime = 15;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataSetupTime = 255;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_BusTurnAroundDuration = 15;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_CLKDivision = 15;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataLatency = 15;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AccessMode = FMC_AccessMode_A;
}

/**
  * @brief  Enables or disables the specified NOR/SRAM Memory Bank.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank1_NORSRAM1: FMC Bank1 NOR/SRAM1  
  *            @arg FMC_Bank1_NORSRAM2: FMC Bank1 NOR/SRAM2 
  *            @arg FMC_Bank1_NORSRAM3: FMC Bank1 NOR/SRAM3 
  *            @arg FMC_Bank1_NORSRAM4: FMC Bank1 NOR/SRAM4 
  * @param  NewState: new state of the FMC_Bank. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FMC_NORSRAM_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected NOR/SRAM Bank by setting the PBKEN bit in the BCRx register */
    FMC_Bank1->BTCR[FMC_Bank] |= BCR_MBKEN_SET;
  }
  else
  {
    /* Disable the selected NOR/SRAM Bank by clearing the PBKEN bit in the BCRx register */
    FMC_Bank1->BTCR[FMC_Bank] &= BCR_MBKEN_RESET;
  }
}
/**
  * @}
  */

/** @defgroup FMC_Group2 NAND Controller functions
  * @brief    NAND Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### NAND Controller functions #####
 ===============================================================================  

 [..]  The following sequence should be followed to configure the FMC to interface 
       with 8-bit or 16-bit NAND memory connected to the NAND Bank:
 
  (#) Enable the clock for the FMC and associated GPIOs using the following functions:
      (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
      (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
      (++) Connect the involved FMC pins to AF12 using the following function 
           GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
      (++) Configure these FMC pins in alternate function mode by calling the function
           GPIO_Init();    
       
  (#) Declare a FMC_NANDInitTypeDef structure, for example:
      FMC_NANDInitTypeDef  FMC_NANDInitStructure;
      and fill the FMC_NANDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the NAND Controller by calling the function
      FMC_NANDInit(&FMC_NANDInitStructure); 

  (#) Then enable the NAND Bank, for example:
      FMC_NANDCmd(FMC_Bank3_NAND, ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the NAND Bank. 
   
 [..]
  (@) To enable the Error Correction Code (ECC), you have to use the function
      FMC_NANDECCCmd(FMC_Bank3_NAND, ENABLE);  
 [..]
  (@) and to get the current ECC value you have to use the function
      ECCval = FMC_GetECC(FMC_Bank3_NAND); 

@endverbatim
  * @{
  */
  
/**
  * @brief  De-initializes the FMC NAND Banks registers to their default reset values.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND 
  * @retval None
  */
void FMC_NANDDeInit(uint32_t FMC_Bank)
{
  /* Check the parameter */
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    /* Set the FMC_Bank2 registers to their reset values */
    FMC_Bank2->PCR2 = 0x00000018;
    FMC_Bank2->SR2 = 0x00000040;
    FMC_Bank2->PMEM2 = 0xFCFCFCFC;
    FMC_Bank2->PATT2 = 0xFCFCFCFC;  
  }
  /* FMC_Bank3_NAND */  
  else
  {
    /* Set the FMC_Bank3 registers to their reset values */
    FMC_Bank3->PCR3 = 0x00000018;
    FMC_Bank3->SR3 = 0x00000040;
    FMC_Bank3->PMEM3 = 0xFCFCFCFC;
    FMC_Bank3->PATT3 = 0xFCFCFCFC; 
  }  
}

/**
  * @brief  Initializes the FMC NAND Banks according to the specified parameters
  *         in the FMC_NANDInitStruct.
  * @param  FMC_NANDInitStruct : pointer to a FMC_NANDInitTypeDef structure that
  *         contains the configuration information for the FMC NAND specified Banks.                       
  * @retval None
  */
void FMC_NANDInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct)
{
  uint32_t tmppcr = 0x00000000, tmppmem = 0x00000000, tmppatt = 0x00000000; 
    
  /* Check the parameters */
  assert_param(IS_FMC_NAND_BANK(FMC_NANDInitStruct->FMC_Bank));
  assert_param(IS_FMC_WAIT_FEATURE(FMC_NANDInitStruct->FMC_Waitfeature));
  assert_param(IS_FMC_NAND_MEMORY_WIDTH(FMC_NANDInitStruct->FMC_MemoryDataWidth));
  assert_param(IS_FMC_ECC_STATE(FMC_NANDInitStruct->FMC_ECC));
  assert_param(IS_FMC_ECCPAGE_SIZE(FMC_NANDInitStruct->FMC_ECCPageSize));
  assert_param(IS_FMC_TCLR_TIME(FMC_NANDInitStruct->FMC_TCLRSetupTime));
  assert_param(IS_FMC_TAR_TIME(FMC_NANDInitStruct->FMC_TARSetupTime));
  assert_param(IS_FMC_SETUP_TIME(FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime));
  assert_param(IS_FMC_WAIT_TIME(FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime));
  assert_param(IS_FMC_SETUP_TIME(FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime));
  assert_param(IS_FMC_WAIT_TIME(FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime));
  
  /* Set the tmppcr value according to FMC_NANDInitStruct parameters */
  tmppcr = (uint32_t)FMC_NANDInitStruct->FMC_Waitfeature |
            PCR_MEMORYTYPE_NAND |
            FMC_NANDInitStruct->FMC_MemoryDataWidth |
            FMC_NANDInitStruct->FMC_ECC |
            FMC_NANDInitStruct->FMC_ECCPageSize |
            (FMC_NANDInitStruct->FMC_TCLRSetupTime << 9 )|
            (FMC_NANDInitStruct->FMC_TARSetupTime << 13);
            
  /* Set tmppmem value according to FMC_CommonSpaceTimingStructure parameters */
  tmppmem = (uint32_t)FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime |
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime << 8) |
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime << 16)|
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime << 24); 
            
  /* Set tmppatt value according to FMC_AttributeSpaceTimingStructure parameters */
  tmppatt = (uint32_t)FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime |
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime << 8) |
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime << 16)|
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime << 24);
  
  if(FMC_NANDInitStruct->FMC_Bank == FMC_Bank2_NAND)
  {
    /* FMC_Bank2_NAND registers configuration */
    FMC_Bank2->PCR2 = tmppcr;
    FMC_Bank2->PMEM2 = tmppmem;
    FMC_Bank2->PATT2 = tmppatt;
  }
  else
  {
    /* FMC_Bank3_NAND registers configuration */
    FMC_Bank3->PCR3 = tmppcr;
    FMC_Bank3->PMEM3 = tmppmem;
    FMC_Bank3->PATT3 = tmppatt;
  }
}


/**
  * @brief  Fills each FMC_NANDInitStruct member with its default value.
  * @param  FMC_NANDInitStruct: pointer to a FMC_NANDInitTypeDef structure which
  *         will be initialized.
  * @retval None
  */
void FMC_NANDStructInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct)
{ 
  /* Reset NAND Init structure parameters values */
  FMC_NANDInitStruct->FMC_Bank = FMC_Bank2_NAND;
  FMC_NANDInitStruct->FMC_Waitfeature = FMC_Waitfeature_Disable;
  FMC_NANDInitStruct->FMC_MemoryDataWidth = FMC_NAND_MemoryDataWidth_16b;
  FMC_NANDInitStruct->FMC_ECC = FMC_ECC_Disable;
  FMC_NANDInitStruct->FMC_ECCPageSize = FMC_ECCPageSize_256Bytes;
  FMC_NANDInitStruct->FMC_TCLRSetupTime = 0x0;
  FMC_NANDInitStruct->FMC_TARSetupTime = 0x0;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime = 252;	  
}

/**
  * @brief  Enables or disables the specified NAND Memory Bank.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  * @param  NewState: new state of the FMC_Bank. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_NANDCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected NAND Bank by setting the PBKEN bit in the PCRx register */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 |= PCR_PBKEN_SET;
    }
    else
    {
      FMC_Bank3->PCR3 |= PCR_PBKEN_SET;
    }
  }
  else
  {
    /* Disable the selected NAND Bank by clearing the PBKEN bit in the PCRx register */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 &= PCR_PBKEN_RESET;
    }
    else
    {
      FMC_Bank3->PCR3 &= PCR_PBKEN_RESET;
    }
  }
}
/**
  * @brief  Enables or disables the FMC NAND ECC feature.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  * @param  NewState: new state of the FMC NAND ECC feature.  
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_NANDECCCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected NAND Bank ECC function by setting the ECCEN bit in the PCRx register */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 |= PCR_ECCEN_SET;
    }
    else
    {
      FMC_Bank3->PCR3 |= PCR_ECCEN_SET;
    }
  }
  else
  {
    /* Disable the selected NAND Bank ECC function by clearing the ECCEN bit in the PCRx register */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 &= PCR_ECCEN_RESET;
    }
    else
    {
      FMC_Bank3->PCR3 &= PCR_ECCEN_RESET;
    }
  }
}

/**
  * @brief  Returns the error correction code register value.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  * @retval The Error Correction Code (ECC) value.
  */
uint32_t FMC_GetECC(uint32_t FMC_Bank)
{
  uint32_t eccval = 0x00000000;
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    /* Get the ECCR2 register value */
    eccval = FMC_Bank2->ECCR2;
  }
  else
  {
    /* Get the ECCR3 register value */
    eccval = FMC_Bank3->ECCR3;
  }
  /* Return the error correction code value */
  return(eccval);
}
/**
  * @}
  */

/** @defgroup FMC_Group3 PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### PCCARD Controller functions #####
 ===============================================================================  

 [..]  he following sequence should be followed to configure the FMC to interface 
       with 16-bit PC Card compatible memory connected to the PCCARD Bank:
 
  (#)  Enable the clock for the FMC and associated GPIOs using the following functions:
       (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
       (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
       (++) Connect the involved FMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
       (++) Configure these FMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
  (#) Declare a FMC_PCCARDInitTypeDef structure, for example:
      FMC_PCCARDInitTypeDef  FMC_PCCARDInitStructure;
      and fill the FMC_PCCARDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the PCCARD Controller by calling the function
      FMC_PCCARDInit(&FMC_PCCARDInitStructure); 

  (#) Then enable the PCCARD Bank:
      FMC_PCCARDCmd(ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the PCCARD Bank. 
 
@endverbatim
  * @{
  */

/**
  * @brief  De-initializes the FMC PCCARD Bank registers to their default reset values.
  * @param  None                       
  * @retval None
  */
void FMC_PCCARDDeInit(void)
{
  /* Set the FMC_Bank4 registers to their reset values */
  FMC_Bank4->PCR4 = 0x00000018; 
  FMC_Bank4->SR4 = 0x00000000;	
  FMC_Bank4->PMEM4 = 0xFCFCFCFC;
  FMC_Bank4->PATT4 = 0xFCFCFCFC;
  FMC_Bank4->PIO4 = 0xFCFCFCFC;
}

/**
  * @brief  Initializes the FMC PCCARD Bank according to the specified parameters
  *         in the FMC_PCCARDInitStruct.
  * @param  FMC_PCCARDInitStruct : pointer to a FMC_PCCARDInitTypeDef structure
  *         that contains the configuration information for the FMC PCCARD Bank.                       
  * @retval None
  */
void FMC_PCCARDInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct)
{
  /* Check the parameters */
  assert_param(IS_FMC_WAIT_FEATURE(FMC_PCCARDInitStruct->FMC_Waitfeature));
  assert_param(IS_FMC_TCLR_TIME(FMC_PCCARDInitStruct->FMC_TCLRSetupTime));
  assert_param(IS_FMC_TAR_TIME(FMC_PCCARDInitStruct->FMC_TARSetupTime));
 
  assert_param(IS_FMC_SETUP_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime));
  assert_param(IS_FMC_WAIT_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime));
  
  assert_param(IS_FMC_SETUP_TIME(FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime));
  assert_param(IS_FMC_WAIT_TIME(FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime));
  assert_param(IS_FMC_SETUP_TIME(FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_SetupTime));
  assert_param(IS_FMC_WAIT_TIME(FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HiZSetupTime));
  
  /* Set the PCR4 register value according to FMC_PCCARDInitStruct parameters */
  FMC_Bank4->PCR4 = (uint32_t)FMC_PCCARDInitStruct->FMC_Waitfeature |
                     FMC_NAND_MemoryDataWidth_16b |  
                     (FMC_PCCARDInitStruct->FMC_TCLRSetupTime << 9) |
                     (FMC_PCCARDInitStruct->FMC_TARSetupTime << 13);
            
  /* Set PMEM4 register value according to FMC_CommonSpaceTimingStructure parameters */
  FMC_Bank4->PMEM4 = (uint32_t)FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime |
                      (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime << 8) |
                      (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime << 16)|
                      (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime << 24); 
            
  /* Set PATT4 register value according to FMC_AttributeSpaceTimingStructure parameters */
  FMC_Bank4->PATT4 = (uint32_t)FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime |
                      (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime << 8) |
                      (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime << 16)|
                      (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime << 24);	
            
  /* Set PIO4 register value according to FMC_IOSpaceTimingStructure parameters */
  FMC_Bank4->PIO4 = (uint32_t)FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_SetupTime |
                     (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_WaitSetupTime << 8) |
                     (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HoldSetupTime << 16)|
                     (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HiZSetupTime << 24);             
}

/**
  * @brief  Fills each FMC_PCCARDInitStruct member with its default value.
  * @param  FMC_PCCARDInitStruct: pointer to a FMC_PCCARDInitTypeDef structure
  *         which will be initialized.
  * @retval None
  */
void FMC_PCCARDStructInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct)
{
  /* Reset PCCARD Init structure parameters values */
  FMC_PCCARDInitStruct->FMC_Waitfeature = FMC_Waitfeature_Disable;
  FMC_PCCARDInitStruct->FMC_TCLRSetupTime = 0;
  FMC_PCCARDInitStruct->FMC_TARSetupTime = 0;
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime = 252;	
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HiZSetupTime = 252;
}

/**
  * @brief  Enables or disables the PCCARD Memory Bank.
  * @param  NewState: new state of the PCCARD Memory Bank.  
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_PCCARDCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the PCCARD Bank by setting the PBKEN bit in the PCR4 register */
    FMC_Bank4->PCR4 |= PCR_PBKEN_SET;
  }
  else
  {
    /* Disable the PCCARD Bank by clearing the PBKEN bit in the PCR4 register */
    FMC_Bank4->PCR4 &= PCR_PBKEN_RESET;
  }
}

/**
  * @}
  */

/** @defgroup FMC_Group4  SDRAM Controller functions
  * @brief    SDRAM Controller functions
  *
@verbatim   
 ===============================================================================
                     ##### SDRAM Controller functions ##### 
 ===============================================================================  
  
 [..]  The following sequence should be followed to configure the FMC to interface
       with SDRAM memory connected to the SDRAM Bank 1 or SDRAM bank 2:
 
  (#) Enable the clock for the FMC and associated GPIOs using the following functions:
      (++) RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
      (++) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
      (++) Connect the involved FMC pins to AF12 using the following function 
           GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
      (++) Configure these FMC pins in alternate function mode by calling the function
           GPIO_Init();    
       
  (#) Declare a FMC_SDRAMInitTypeDef structure, for example:
       FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
      and fill the FMC_SDRAMInitStructure variable with the allowed values of
      the structure member.  
      
  (#) Initialize the SDRAM Controller by calling the function
          FMC_SDRAMInit(&FMC_SDRAMInitStructure);
          
  (#) Declare a FMC_SDRAMCommandTypeDef structure, for example:
        FMC_SDRAMCommandTypeDef  FMC_SDRAMCommandStructure;
      and fill the FMC_SDRAMCommandStructure variable with the allowed values of
      the structure member.        

  (#) Configure the SDCMR register with the desired command parameters by calling 
      the function FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  

  (#) At this stage, the SDRAM memory is ready for any valid command.
   
@endverbatim
  * @{
  */

/**
  * @brief  De-initializes the FMC SDRAM Banks registers to their default 
  *         reset values.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM 
  * @retval None
  */
void FMC_SDRAMDeInit(uint32_t FMC_Bank)
{
  /* Check the parameter */
  assert_param(IS_FMC_SDRAM_BANK(FMC_Bank));
  
  FMC_Bank5_6->SDCR[FMC_Bank] = 0x000002D0;
  FMC_Bank5_6->SDTR[FMC_Bank] = 0x0FFFFFFF;    
  FMC_Bank5_6->SDCMR = 0x00000000;
  FMC_Bank5_6->SDRTR = 0x00000000;
  FMC_Bank5_6->SDSR = 0x00000000; 
}  

/**
  * @brief  Initializes the FMC SDRAM Banks according to the specified
  *         parameters in the FMC_SDRAMInitStruct.
  * @param  FMC_SDRAMInitStruct : pointer to a FMC_SDRAMInitTypeDef structure
  *         that contains the configuration information for the FMC SDRAM 
  *         specified Banks.                       
  * @retval None
  */
void FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)
{ 
  /* temporary registers */
  uint32_t tmpr1 = 0;
  uint32_t tmpr2 = 0;
  uint32_t tmpr3 = 0;
  uint32_t tmpr4 = 0;
  
  /* Check the parameters */
  
  /* Control parameters */
  assert_param(IS_FMC_SDRAM_BANK(FMC_SDRAMInitStruct->FMC_Bank));
  assert_param(IS_FMC_COLUMNBITS_NUMBER(FMC_SDRAMInitStruct->FMC_ColumnBitsNumber)); 
  assert_param(IS_FMC_ROWBITS_NUMBER(FMC_SDRAMInitStruct->FMC_RowBitsNumber));
  assert_param(IS_FMC_SDMEMORY_WIDTH(FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth));
  assert_param(IS_FMC_INTERNALBANK_NUMBER(FMC_SDRAMInitStruct->FMC_InternalBankNumber)); 
  assert_param(IS_FMC_CAS_LATENCY(FMC_SDRAMInitStruct->FMC_CASLatency));
  assert_param(IS_FMC_WRITE_PROTECTION(FMC_SDRAMInitStruct->FMC_WriteProtection));
  assert_param(IS_FMC_SDCLOCK_PERIOD(FMC_SDRAMInitStruct->FMC_SDClockPeriod));
  assert_param(IS_FMC_READ_BURST(FMC_SDRAMInitStruct->FMC_ReadBurst));
  assert_param(IS_FMC_READPIPE_DELAY(FMC_SDRAMInitStruct->FMC_ReadPipeDelay));   
  
  /* Timing parameters */
  assert_param(IS_FMC_LOADTOACTIVE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)); 
  assert_param(IS_FMC_EXITSELFREFRESH_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay));
  assert_param(IS_FMC_SELFREFRESH_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime));
  assert_param(IS_FMC_ROWCYCLE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay));
  assert_param(IS_FMC_WRITE_RECOVERY_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)); 
  assert_param(IS_FMC_RP_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)); 
  assert_param(IS_FMC_RCD_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay));    
  
  /* SDRAM bank control register configuration */ 
  tmpr1 =   (uint32_t)FMC_SDRAMInitStruct->FMC_ColumnBitsNumber |
             FMC_SDRAMInitStruct->FMC_RowBitsNumber |
             FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth |
             FMC_SDRAMInitStruct->FMC_InternalBankNumber |           
             FMC_SDRAMInitStruct->FMC_CASLatency |
             FMC_SDRAMInitStruct->FMC_WriteProtection |
             FMC_SDRAMInitStruct->FMC_SDClockPeriod |
             FMC_SDRAMInitStruct->FMC_ReadBurst | 
             FMC_SDRAMInitStruct->FMC_ReadPipeDelay;
            
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1;
  }
  else   /* SDCR2 "don't care" bits configuration */
  {
    tmpr3 = (uint32_t)FMC_SDRAMInitStruct->FMC_SDClockPeriod |
             FMC_SDRAMInitStruct->FMC_ReadBurst | 
             FMC_SDRAMInitStruct->FMC_ReadPipeDelay;
    
    FMC_Bank5_6->SDCR[FMC_Bank1_SDRAM] = tmpr3;
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1;
  }
  /* SDRAM bank timing register configuration */
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    tmpr2 =   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay)-1) << 24);
            
            FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2;
  }
  else   /* SDTR "don't care bits configuration */
  {
    tmpr2 =   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16);
            
    tmpr4 =   (uint32_t)(((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20);
            
            FMC_Bank5_6->SDTR[FMC_Bank1_SDRAM] = tmpr4;
            FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2;
  }
  
}

/**
  * @brief  Fills each FMC_SDRAMInitStruct member with its default value.
  * @param  FMC_SDRAMInitStruct: pointer to a FMC_SDRAMInitTypeDef structure 
  *         which will be initialized.
  * @retval None
  */
void FMC_SDRAMStructInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)  
{  
  /* Reset SDRAM Init structure parameters values */
  FMC_SDRAMInitStruct->FMC_Bank = FMC_Bank1_SDRAM;
  FMC_SDRAMInitStruct->FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  FMC_SDRAMInitStruct->FMC_RowBitsNumber = FMC_RowBits_Number_11b; 
  FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
  FMC_SDRAMInitStruct->FMC_InternalBankNumber = FMC_InternalBank_Number_4; 
  FMC_SDRAMInitStruct->FMC_CASLatency = FMC_CAS_Latency_1;  
  FMC_SDRAMInitStruct->FMC_WriteProtection = FMC_Write_Protection_Enable;
  FMC_SDRAMInitStruct->FMC_SDClockPeriod = FMC_SDClock_Disable;
  FMC_SDRAMInitStruct->FMC_ReadBurst = FMC_Read_Burst_Disable;
  FMC_SDRAMInitStruct->FMC_ReadPipeDelay = FMC_ReadPipe_Delay_0; 
   
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay = 16;
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay = 16;
  
}

/**
  * @brief  Configures the SDRAM memory command issued when the device is accessed.   
  * @param  FMC_SDRAMCommandStruct: pointer to a FMC_SDRAMCommandTypeDef structure 
  *         which will be configured.
  * @retval None
  */
void FMC_SDRAMCmdConfig(FMC_SDRAMCommandTypeDef* FMC_SDRAMCommandStruct)
{
  uint32_t tmpr = 0x0;
    
  /* check parameters */
  assert_param(IS_FMC_COMMAND_MODE(FMC_SDRAMCommandStruct->FMC_CommandMode));
  assert_param(IS_FMC_COMMAND_TARGET(FMC_SDRAMCommandStruct->FMC_CommandTarget));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber));
  assert_param(IS_FMC_MODE_REGISTER(FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition));
  
  tmpr =   (uint32_t)(FMC_SDRAMCommandStruct->FMC_CommandMode |
                      FMC_SDRAMCommandStruct->FMC_CommandTarget |
                     (((FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber)-1)<<5) |
                     ((FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition)<<9));
  
  FMC_Bank5_6->SDCMR = tmpr;

}


/**
  * @brief  Returns the indicated FMC SDRAM bank mode status.
  * @param  SDRAM_Bank: Defines the FMC SDRAM bank. This parameter can be 
  *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM. 
  * @retval The FMC SDRAM bank mode status         
  */
uint32_t FMC_GetModeStatus(uint32_t SDRAM_Bank)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameter */
  assert_param(IS_FMC_SDRAM_BANK(SDRAM_Bank));

  /* Get the busy flag status */
  if(SDRAM_Bank == FMC_Bank1_SDRAM)
  {
    tmpreg = (uint32_t)(FMC_Bank5_6->SDSR & FMC_SDSR_MODES1); 
  }
  else
  {
    tmpreg = ((uint32_t)(FMC_Bank5_6->SDSR & FMC_SDSR_MODES2) >> 2);
  }
  
  /* Return the mode status */
  return tmpreg;
}

/**
  * @brief  defines the SDRAM Memory Refresh rate.
  * @param  FMC_Count: specifies the Refresh timer count.       
  * @retval None
  */
void FMC_SetRefreshCount(uint32_t FMC_Count)
{
  /* check the parameters */
  assert_param(IS_FMC_REFRESH_COUNT(FMC_Count));
  
  FMC_Bank5_6->SDRTR |= (FMC_Count<<1);
   
}

/**
  * @brief  Sets the Number of consecutive SDRAM Memory auto Refresh commands.
  * @param  FMC_Number: specifies the auto Refresh number.       
  * @retval None
  */
void FMC_SetAutoRefresh_Number(uint32_t FMC_Number)
{
  /* check the parameters */
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(FMC_Number));
  
  FMC_Bank5_6->SDCMR |= (FMC_Number << 5);   
}

/**
  * @brief  Enables or disables write protection to the specified FMC SDRAM Bank.
  * @param  SDRAM_Bank: Defines the FMC SDRAM bank. This parameter can be 
  *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM.   
  * @param  NewState: new state of the write protection flag.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_SDRAMWriteProtectionConfig(uint32_t SDRAM_Bank, FunctionalState NewState)
{
  /* Check the parameter */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_FMC_SDRAM_BANK(SDRAM_Bank));
  
  if (NewState != DISABLE)
  {
    FMC_Bank5_6->SDCR[SDRAM_Bank] |= FMC_Write_Protection_Enable;    
  }
  else
  {
    FMC_Bank5_6->SDCR[SDRAM_Bank] &= SDCR_WriteProtection_RESET;
  } 
  
}

/**
  * @}
  */

/** @defgroup FMC_Group5  Interrupts and flags management functions
  * @brief    Interrupts and flags management functions
  *
@verbatim   
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified FMC interrupts.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  *            @arg FMC_Bank4_PCCARD: FMC Bank4 PCCARD
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM   
  * @param  FMC_IT: specifies the FMC interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg FMC_IT_RisingEdge: Rising edge detection interrupt. 
  *            @arg FMC_IT_Level: Level edge detection interrupt.
  *            @arg FMC_IT_FallingEdge: Falling edge detection interrupt.
  *            @arg FMC_IT_Refresh: Refresh error detection interrupt.  
  * @param  NewState: new state of the specified FMC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FMC_ITConfig(uint32_t FMC_Bank, uint32_t FMC_IT, FunctionalState NewState)
{
  assert_param(IS_FMC_IT_BANK(FMC_Bank));
  assert_param(IS_FMC_IT(FMC_IT));	
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected FMC_Bank2 interrupts */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->SR2 |= FMC_IT;
    }
    /* Enable the selected FMC_Bank3 interrupts */
    else if (FMC_Bank == FMC_Bank3_NAND)
    {
      FMC_Bank3->SR3 |= FMC_IT;
    }
    /* Enable the selected FMC_Bank4 interrupts */
    else if (FMC_Bank == FMC_Bank4_PCCARD)
    {
      FMC_Bank4->SR4 |= FMC_IT;    
    }
    /* Enable the selected FMC_Bank5_6 interrupt */
    else
    {
      /* Enables the interrupt if the refresh error flag is set */
      FMC_Bank5_6->SDRTR |= FMC_IT; 
    }
  }
  else
  {
    /* Disable the selected FMC_Bank2 interrupts */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      
      FMC_Bank2->SR2 &= (uint32_t)~FMC_IT;
    }
    /* Disable the selected FMC_Bank3 interrupts */
    else if (FMC_Bank == FMC_Bank3_NAND)
    {
      FMC_Bank3->SR3 &= (uint32_t)~FMC_IT;
    }
    /* Disable the selected FMC_Bank4 interrupts */
    else if(FMC_Bank == FMC_Bank4_PCCARD)
    {
      FMC_Bank4->SR4 &= (uint32_t)~FMC_IT;    
    }
    /* Disable the selected FMC_Bank5_6 interrupt */
    else
    {
      /* Disables the interrupt if the refresh error flag is not set */
      FMC_Bank5_6->SDRTR &= (uint32_t)~FMC_IT; 
    }
  }
}

/**
  * @brief  Checks whether the specified FMC flag is set or not.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  *            @arg FMC_Bank4_PCCARD: FMC Bank4 PCCARD
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM 
  *            @arg FMC_Bank1_SDRAM | FMC_Bank2_SDRAM: FMC Bank1 or Bank2 SDRAM    
  * @param  FMC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg FMC_FLAG_RisingEdge: Rising edge detection Flag.
  *            @arg FMC_FLAG_Level: Level detection Flag.
  *            @arg FMC_FLAG_FallingEdge: Falling edge detection Flag.
  *            @arg FMC_FLAG_FEMPT: Fifo empty Flag.
  *            @arg FMC_FLAG_Refresh: Refresh error Flag.
  *            @arg FMC_FLAG_Busy: Busy status Flag.     
  * @retval The new state of FMC_FLAG (SET or RESET).
  */
FlagStatus FMC_GetFlagStatus(uint32_t FMC_Bank, uint32_t FMC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpsr = 0x00000000;
  
  /* Check the parameters */
  assert_param(IS_FMC_GETFLAG_BANK(FMC_Bank));
  assert_param(IS_FMC_GET_FLAG(FMC_FLAG));
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    tmpsr = FMC_Bank2->SR2;
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    tmpsr = FMC_Bank3->SR3;
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    tmpsr = FMC_Bank4->SR4;
  }
  else 
  {
    tmpsr = FMC_Bank5_6->SDSR;
  }
  
  /* Get the flag status */
  if ((tmpsr & FMC_FLAG) != FMC_FLAG )
  {
    bitstatus = RESET;
  }
  else
  {
    bitstatus = SET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the FMC's pending flags.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  *            @arg FMC_Bank4_PCCARD: FMC Bank4 PCCARD
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM  
  * @param  FMC_FLAG: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg FMC_FLAG_RisingEdge: Rising edge detection Flag.
  *            @arg FMC_FLAG_Level: Level detection Flag.
  *            @arg FMC_FLAG_FallingEdge: Falling edge detection Flag.
  *            @arg FMC_FLAG_Refresh: Refresh error Flag.  
  * @retval None
  */
void FMC_ClearFlag(uint32_t FMC_Bank, uint32_t FMC_FLAG)
{
 /* Check the parameters */
  assert_param(IS_FMC_GETFLAG_BANK(FMC_Bank));
  assert_param(IS_FMC_CLEAR_FLAG(FMC_FLAG)) ;
    
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    FMC_Bank2->SR2 &= (~FMC_FLAG); 
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    FMC_Bank3->SR3 &= (~FMC_FLAG);
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    FMC_Bank4->SR4 &= (~FMC_FLAG);
  }
  /* FMC_Bank5_6 SDRAM*/
  else
  {
    FMC_Bank5_6->SDRTR &= (~FMC_FLAG);
  }
  
}

/**
  * @brief  Checks whether the specified FMC interrupt has occurred or not.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  *            @arg FMC_Bank4_PCCARD: FMC Bank4 PCCARD
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM   
  * @param  FMC_IT: specifies the FMC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg FMC_IT_RisingEdge: Rising edge detection interrupt. 
  *            @arg FMC_IT_Level: Level edge detection interrupt.
  *            @arg FMC_IT_FallingEdge: Falling edge detection interrupt.
  *            @arg FMC_IT_Refresh: Refresh error detection interrupt.    
  * @retval The new state of FMC_IT (SET or RESET).
  */
ITStatus FMC_GetITStatus(uint32_t FMC_Bank, uint32_t FMC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpsr = 0x0;
  uint32_t tmpsr2 = 0x0;
  uint32_t itstatus = 0x0;
  uint32_t itenable = 0x0; 
  
  /* Check the parameters */
  assert_param(IS_FMC_IT_BANK(FMC_Bank));
  assert_param(IS_FMC_GET_IT(FMC_IT));
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    tmpsr = FMC_Bank2->SR2;
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    tmpsr = FMC_Bank3->SR3;
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    tmpsr = FMC_Bank4->SR4;
  }
  /* FMC_Bank5_6 SDRAM*/
  else
  {
    tmpsr = FMC_Bank5_6->SDRTR;
    tmpsr2 = FMC_Bank5_6->SDSR;
  } 
  
  /* get the IT enable bit status*/
  itenable = tmpsr & FMC_IT;
  
  /* get the corresponding IT Flag status*/
  if((FMC_Bank == FMC_Bank1_SDRAM) || (FMC_Bank == FMC_Bank2_SDRAM))
  {
    itstatus = tmpsr2 & FMC_SDSR_RE;  
  }           
  else
  {
    itstatus = tmpsr & (FMC_IT >> 3);  
  }  
  
  if ((itstatus != (uint32_t)RESET)  && (itenable != (uint32_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus; 
}

/**
  * @brief  Clears the FMC's interrupt pending bits.
  * @param  FMC_Bank: specifies the FMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FMC_Bank2_NAND: FMC Bank2 NAND 
  *            @arg FMC_Bank3_NAND: FMC Bank3 NAND
  *            @arg FMC_Bank4_PCCARD: FMC Bank4 PCCARD
  *            @arg FMC_Bank1_SDRAM: FMC Bank1 SDRAM 
  *            @arg FMC_Bank2_SDRAM: FMC Bank2 SDRAM   
  * @param  FMC_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg FMC_IT_RisingEdge: Rising edge detection interrupt. 
  *            @arg FMC_IT_Level: Level edge detection interrupt.
  *            @arg FMC_IT_FallingEdge: Falling edge detection interrupt.
  *            @arg FMC_IT_Refresh: Refresh error detection interrupt.  
  * @retval None
  */
void FMC_ClearITPendingBit(uint32_t FMC_Bank, uint32_t FMC_IT)
{
  /* Check the parameters */
  assert_param(IS_FMC_IT_BANK(FMC_Bank));
  assert_param(IS_FMC_IT(FMC_IT));
    
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    FMC_Bank2->SR2 &= ~(FMC_IT >> 3); 
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    FMC_Bank3->SR3 &= ~(FMC_IT >> 3);
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    FMC_Bank4->SR4 &= ~(FMC_IT >> 3);
  }
  /* FMC_Bank5_6 SDRAM*/
  else
  {
    FMC_Bank5_6->SDRTR |= FMC_SDRTR_CRE;
  }
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
