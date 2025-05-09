/**
  ******************************************************************************
  * @file    stm32n6xx_ll_fmc.c
  * @author  MCD Application Team
  * @brief   FMC Low Layer HAL module driver.
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the Flexible Memory Controller (FMC) peripheral memories:
  *           + Initialization/de-initialization functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### FMC peripheral features #####
  ==============================================================================
  [..] The Flexible memory controller (FMC) includes following memory controllers:
       (+) The NOR/PSRAM memory controller
     (+) The NAND memory controller
       (+) The Synchronous DRAM (SDRAM) controller

  [..] The FMC functional block makes the interface with synchronous and asynchronous static
       memories and SDRAM memories. Its main purposes are:
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
           (++) Two banks of NAND Flash memory with ECC hardware to check up to 8 Kbytes of
                data
        (+) Interface with synchronous DRAM (SDRAM) memories
        (+) Independent Chip Select control for each memory bank
        (+) Independent configuration for each memory bank

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined(HAL_NOR_MODULE_ENABLED) || defined(HAL_NAND_MODULE_ENABLED) || defined(HAL_SDRAM_MODULE_ENABLED)\
    || defined(HAL_SRAM_MODULE_ENABLED)

/** @defgroup FMC_LL  FMC Low Layer
  * @brief FMC driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup FMC_LL_Private_Constants FMC Low Layer Private Constants
  * @{
  */

/* ----------------------- FMC registers bit mask --------------------------- */

/* --- BCR Register ---*/
/* BCR register clear mask */

/* --- BTR Register ---*/
/* BTR register clear mask */
#define BTR_CLEAR_MASK    ((uint32_t)(FMC_BTRx_ADDSET | FMC_BTRx_ADDHLD  |\
                                      FMC_BTRx_DATAST | FMC_BTRx_BUSTURN |\
                                      FMC_BTRx_CLKDIV | FMC_BTRx_DATLAT  |\
                                      FMC_BTRx_ACCMOD | FMC_BTRx_DATAHLD))

/* --- BWTR Register ---*/
/* BWTR register clear mask */
#define BWTR_CLEAR_MASK   ((uint32_t)(FMC_BWTRx_ADDSET | FMC_BWTRx_ADDHLD  |\
                                      FMC_BWTRx_DATAST | FMC_BWTRx_BUSTURN |\
                                      FMC_BWTRx_ACCMOD | FMC_BWTRx_DATAHLD))

/* --- PCR Register ---*/
/* PCR register clear mask */
#define PCR_CLEAR_MASK    ((uint32_t)(FMC_PCR_PWAITEN | FMC_PCR_PBKEN  | \
                                      FMC_PCR_PWID    | FMC_PCR_ECCEN  | \
                                      FMC_PCR_TCLR    | FMC_PCR_TAR    | \
                                      FMC_PCR_ECCSS))
/* --- PMEM Register ---*/
/* PMEM register clear mask */
#define PMEM_CLEAR_MASK   ((uint32_t)(FMC_PMEM_MEMSET  | FMC_PMEM_MEMWAIT |\
                                      FMC_PMEM_MEMHOLD | FMC_PMEM_MEMHIZ))

/* --- PATT Register ---*/
/* PATT register clear mask */
#define PATT_CLEAR_MASK   ((uint32_t)(FMC_PATT_ATTSET  | FMC_PATT_ATTWAIT |\
                                      FMC_PATT_ATTHOLD | FMC_PATT_ATTHIZ))


/* --- SDCR Register ---*/
/* SDCR register clear mask */
#define SDCR_CLEAR_MASK   ((uint32_t)(FMC_SDCRx_NC    | FMC_SDCRx_NR     | \
                                      FMC_SDCRx_MWID  | FMC_SDCRx_NB     | \
                                      FMC_SDCRx_CAS   | FMC_SDCRx_WP     | \
                                      FMC_SDCRx_SDCLK | FMC_SDCRx_RPIPE))

/* --- SDTR Register ---*/
/* SDTR register clear mask */
#define SDTR_CLEAR_MASK   ((uint32_t)(FMC_SDTRx_TMRD  | FMC_SDTRx_TXSR   | \
                                      FMC_SDTRx_TRAS  | FMC_SDTRx_TRC    | \
                                      FMC_SDTRx_TWR   | FMC_SDTRx_TRP    | \
                                      FMC_SDTRx_TRCD))

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup FMC_LL_Exported_Functions FMC Low Layer Exported Functions
  * @{
  */


/** @defgroup FMC_LL_Exported_Functions_NORSRAM FMC Low Layer NOR SRAM Exported Functions
  * @brief  NORSRAM Controller functions
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

/** @defgroup FMC_LL_NORSRAM_Exported_Functions_Group1 Initialization and de-initialization functions
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
  * @param  Device Pointer to NORSRAM device instance
  * @param  Init Pointer to NORSRAM Initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device,
                                    const FMC_NORSRAM_InitTypeDef *Init)
{
  uint32_t flashaccess;
  uint32_t btcr_reg;
  uint32_t mask;

  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Init->NSBank));
  assert_param(IS_FMC_MUX(Init->DataAddressMux));
  assert_param(IS_FMC_MEMORY(Init->MemoryType));
  assert_param(IS_FMC_NORSRAM_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FMC_BURSTMODE(Init->BurstAccessMode));
  assert_param(IS_FMC_WAIT_POLARITY(Init->WaitSignalPolarity));
  assert_param(IS_FMC_WAIT_SIGNAL_ACTIVE(Init->WaitSignalActive));
  assert_param(IS_FMC_WRITE_OPERATION(Init->WriteOperation));
  assert_param(IS_FMC_WAITE_SIGNAL(Init->WaitSignal));
  assert_param(IS_FMC_EXTENDED_MODE(Init->ExtendedMode));
  assert_param(IS_FMC_ASYNWAIT(Init->AsynchronousWait));
  assert_param(IS_FMC_WRITE_BURST(Init->WriteBurst));
  assert_param(IS_FMC_CONTINOUS_CLOCK(Init->ContinuousClock));
  assert_param(IS_FMC_PAGESIZE(Init->PageSize));
  assert_param(IS_FMC_NBL_SETUPTIME(Init->NBLSetupTime));

  /* Disable NORSRAM Device */
  __FMC_NORSRAM_DISABLE(Device, Init->NSBank);

  /* Set NORSRAM device control parameters */
  if (Init->MemoryType == FMC_MEMORY_TYPE_NOR)
  {
    flashaccess = FMC_NORSRAM_FLASH_ACCESS_ENABLE;
  }
  else
  {
    flashaccess = FMC_NORSRAM_FLASH_ACCESS_DISABLE;
  }

  btcr_reg = (flashaccess                   | \
              Init->DataAddressMux          | \
              Init->MemoryType              | \
              Init->MemoryDataWidth         | \
              Init->BurstAccessMode         | \
              Init->WaitSignalPolarity      | \
              Init->WaitSignalActive        | \
              Init->WriteOperation          | \
              Init->WaitSignal              | \
              Init->ExtendedMode            | \
              Init->AsynchronousWait        | \
              Init->WriteBurst);

  btcr_reg |= Init->ContinuousClock;
  btcr_reg |= Init->NBLSetupTime;
  btcr_reg |= Init->PageSize;

  mask = (FMC_BCRx_MBKEN                |
          FMC_BCRx_MUXEN                |
          FMC_BCRx_MTYP                 |
          FMC_BCRx_MWID                 |
          FMC_BCRx_FACCEN               |
          FMC_BCRx_BURSTEN              |
          FMC_BCRx_WAITPOL              |
          FMC_BCRx_WAITCFG              |
          FMC_BCRx_WREN                 |
          FMC_BCRx_WAITEN               |
          FMC_BCRx_EXTMOD               |
          FMC_BCRx_ASYNCWAIT            |
          FMC_BCRx_CBURSTRW);

  mask |= FMC_CFGR_CCLKEN;
  mask |= FMC_BCRx_NBLSET;
  mask |= FMC_BCRx_CPSIZE;

  MODIFY_REG(Device->BTCR[Init->NSBank], mask, btcr_reg);

  /* Configure synchronous mode when Continuous clock is enabled for bank2..4 */
  if ((Init->ContinuousClock == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC) && (Init->NSBank != FMC_NORSRAM_BANK1))
  {
    MODIFY_REG(Device->BTCR[FMC_NORSRAM_BANK1], FMC_CFGR_CCLKEN, Init->ContinuousClock);
  }

  return HAL_OK;
}

/**
  * @brief  DeInitialize the FMC_NORSRAM peripheral
  * @param  Device Pointer to NORSRAM device instance
  * @param  ExDevice Pointer to NORSRAM extended mode device instance
  * @param  Bank NORSRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device,
                                     FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_EXTENDED_DEVICE(ExDevice));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));

  /* Disable the FMC_NORSRAM device */
  __FMC_NORSRAM_DISABLE(Device, Bank);

  /* De-initialize the FMC_NORSRAM device */
  /* FMC_NORSRAM_BANK1 */
  if (Bank == FMC_NORSRAM_BANK1)
  {
    Device->BTCR[Bank] = 0x000030DBU;
  }
  /* FMC_NORSRAM_BANK2, FMC_NORSRAM_BANK3 or FMC_NORSRAM_BANK4 */
  else
  {
    Device->BTCR[Bank] = 0x000030D2U;
  }

  Device->BTCR[Bank + 1U] = 0x0FFFFFFFU;
  ExDevice->BWTR[Bank]   = 0x0FFFFFFFU;

  return HAL_OK;
}

/**
  * @brief  Initialize the FMC_NORSRAM Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device Pointer to NORSRAM device instance
  * @param  Timing Pointer to NORSRAM Timing structure
  * @param  Bank NORSRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device,
                                          const FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmpr;

  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
  assert_param(IS_FMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
  assert_param(IS_FMC_DATAHOLD_DURATION(Timing->DataHoldTime));
  assert_param(IS_FMC_DATASETUP_TIME(Timing->DataSetupTime));
  assert_param(IS_FMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
  assert_param(IS_FMC_CLK_DIV(Timing->CLKDivision));
  assert_param(IS_FMC_DATA_LATENCY(Timing->DataLatency));
  assert_param(IS_FMC_ACCESS_MODE(Timing->AccessMode));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));

  /* Set FMC_NORSRAM device timing parameters */
  Device->BTCR[Bank + 1U] =
    (Timing->AddressSetupTime << FMC_BTRx_ADDSET_Pos) |
    (Timing->AddressHoldTime << FMC_BTRx_ADDHLD_Pos) |
    (Timing->DataSetupTime << FMC_BTRx_DATAST_Pos) |
    (Timing->DataHoldTime << FMC_BTRx_DATAHLD_Pos) |
    (Timing->BusTurnAroundDuration << FMC_BTRx_BUSTURN_Pos) |
    ((Timing->CLKDivision - 1U) << FMC_BTRx_CLKDIV_Pos) |
    ((Timing->DataLatency - 2U) << FMC_BTRx_DATLAT_Pos) |
    Timing->AccessMode;

  /* Configure Clock division value (in NORSRAM bank 1) when continuous clock is enabled */
  if (HAL_IS_BIT_SET(Device->BTCR[FMC_NORSRAM_BANK1], FMC_CFGR_CCLKEN))
  {
    tmpr = (uint32_t)(Device->BTCR[FMC_NORSRAM_BANK1 + 1U] & ~((0x0FU) << FMC_BTRx_CLKDIV_Pos));
    tmpr |= (uint32_t)(((Timing->CLKDivision) - 1U) << FMC_BTRx_CLKDIV_Pos);
    MODIFY_REG(Device->BTCR[FMC_NORSRAM_BANK1 + 1U], FMC_BTRx_CLKDIV, tmpr);
  }

  return HAL_OK;
}

/**
  * @brief  Initialize the FMC_NORSRAM Extended mode Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device Pointer to NORSRAM device instance
  * @param  Timing Pointer to NORSRAM Timing structure
  * @param  Bank NORSRAM bank number
  * @param  ExtendedMode FMC Extended Mode
  *          This parameter can be one of the following values:
  *            @arg FMC_EXTENDED_MODE_DISABLE
  *            @arg FMC_EXTENDED_MODE_ENABLE
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device,
                                                   const FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank,
                                                   uint32_t ExtendedMode)
{
  /* Check the parameters */
  assert_param(IS_FMC_EXTENDED_MODE(ExtendedMode));

  /* Set NORSRAM device timing register for write configuration, if extended mode is used */
  if (ExtendedMode == FMC_EXTENDED_MODE_ENABLE)
  {
    /* Check the parameters */
    assert_param(IS_FMC_NORSRAM_EXTENDED_DEVICE(Device));
    assert_param(IS_FMC_ADDRESS_SETUP_TIME(Timing->AddressSetupTime));
    assert_param(IS_FMC_ADDRESS_HOLD_TIME(Timing->AddressHoldTime));
    assert_param(IS_FMC_DATASETUP_TIME(Timing->DataSetupTime));
    assert_param(IS_FMC_DATAHOLD_DURATION(Timing->DataHoldTime));
    assert_param(IS_FMC_TURNAROUND_TIME(Timing->BusTurnAroundDuration));
    assert_param(IS_FMC_ACCESS_MODE(Timing->AccessMode));
    assert_param(IS_FMC_NORSRAM_BANK(Bank));

    /* Set NORSRAM device timing register for write configuration, if extended mode is used */
    MODIFY_REG(Device->BWTR[Bank], BWTR_CLEAR_MASK, (Timing->AddressSetupTime                                    |
                                                     ((Timing->AddressHoldTime)        << FMC_BWTRx_ADDHLD_Pos)  |
                                                     ((Timing->DataSetupTime)          << FMC_BWTRx_DATAST_Pos)  |
                                                     ((Timing->DataHoldTime)           << FMC_BWTRx_DATAHLD_Pos) |
                                                     Timing->AccessMode                                          |
                                                     ((Timing->BusTurnAroundDuration)  << FMC_BWTRx_BUSTURN_Pos)));
  }
  else
  {
    Device->BWTR[Bank] = 0x0FFFFFFFU;
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup FMC_LL_NORSRAM_Private_Functions_Group2
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
  * @param  Device Pointer to NORSRAM device instance
  * @param  Bank NORSRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));

  /* Enable write operation */
  SET_BIT(Device->BTCR[Bank], FMC_WRITE_OPERATION_ENABLE);

  return HAL_OK;
}

/**
  * @brief  Disables dynamically FMC_NORSRAM write operation.
  * @param  Device Pointer to NORSRAM device instance
  * @param  Bank NORSRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NORSRAM_DEVICE(Device));
  assert_param(IS_FMC_NORSRAM_BANK(Bank));

  /* Disable write operation */
  CLEAR_BIT(Device->BTCR[Bank], FMC_WRITE_OPERATION_ENABLE);

  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */


/** @defgroup FMC_LL_Exported_Functions_NAND FMC Low Layer NAND Exported Functions
  * @brief    NAND Controller functions
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

/** @defgroup FMC_LL_NAND_Exported_Functions_Group1 Initialization and de-initialization functions
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
  * @param  Device Pointer to NAND device instance
  * @param  Init Pointer to NAND Initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_Init(FMC_NAND_TypeDef *Device, const FMC_NAND_InitTypeDef *Init)
{
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_NAND_BANK(Init->NandBank));
  assert_param(IS_FMC_WAIT_FEATURE(Init->Waitfeature));
  assert_param(IS_FMC_NAND_MEMORY_WIDTH(Init->MemoryDataWidth));
  assert_param(IS_FMC_ECC_STATE(Init->EccComputation));
  assert_param(IS_FMC_ECCPAGE_SIZE(Init->ECCPageSize));
  assert_param(IS_FMC_TCLR_TIME(Init->TCLRSetupTime));
  assert_param(IS_FMC_TAR_TIME(Init->TARSetupTime));

  /* NAND bank 3 registers configuration */
  MODIFY_REG(Device->PCR, PCR_CLEAR_MASK, (Init->Waitfeature                            |
                                           FMC_PCR_MEMORY_TYPE_NAND                     |
                                           Init->MemoryDataWidth                        |
                                           Init->EccComputation                         |
                                           Init->ECCPageSize                            |
                                           ((Init->TCLRSetupTime) << FMC_PCR_TCLR_Pos)  |
                                           ((Init->TARSetupTime)  << FMC_PCR_TAR_Pos)));

  return HAL_OK;
}

/**
  * @brief  Initializes the FMC_NAND Common space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device Pointer to NAND device instance
  * @param  Timing Pointer to NAND timing structure
  * @param  Bank NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_CommonSpace_Timing_Init(FMC_NAND_TypeDef *Device,
                                                   const FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));
  assert_param(IS_FMC_NAND_BANK(Bank));

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  /* NAND bank 3 registers configuration */
  Device->PMEM = (Timing->SetupTime |
                  ((Timing->WaitSetupTime) << FMC_PMEM_MEMWAIT_Pos) |
                  ((Timing->HoldSetupTime) << FMC_PMEM_MEMHOLD_Pos) |
                  ((Timing->HiZSetupTime) << FMC_PMEM_MEMHIZ_Pos));

  return HAL_OK;
}

/**
  * @brief  Initializes the FMC_NAND Attribute space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device Pointer to NAND device instance
  * @param  Timing Pointer to NAND timing structure
  * @param  Bank NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_AttributeSpace_Timing_Init(FMC_NAND_TypeDef *Device,
                                                      const FMC_NAND_PCC_TimingTypeDef *Timing, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_SETUP_TIME(Timing->SetupTime));
  assert_param(IS_FMC_WAIT_TIME(Timing->WaitSetupTime));
  assert_param(IS_FMC_HOLD_TIME(Timing->HoldSetupTime));
  assert_param(IS_FMC_HIZ_TIME(Timing->HiZSetupTime));
  assert_param(IS_FMC_NAND_BANK(Bank));

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  /* NAND bank 3 registers configuration */
  Device->PATT = (Timing->SetupTime |
                  ((Timing->WaitSetupTime) << FMC_PATT_ATTWAIT_Pos) |
                  ((Timing->HoldSetupTime) << FMC_PATT_ATTHOLD_Pos) |
                  ((Timing->HiZSetupTime)  << FMC_PATT_ATTHIZ_Pos));

  return HAL_OK;
}

/**
  * @brief  DeInitializes the FMC_NAND device
  * @param  Device Pointer to NAND device instance
  * @param  Bank NAND bank number
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
  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  /* Set the FMC_NAND_BANK3 registers to their reset values */
  WRITE_REG(Device->PCR,  0x0007FE08U);
  WRITE_REG(Device->SR,   0x00015753U);
  WRITE_REG(Device->PMEM, 0x0A0A0A0AU);
  WRITE_REG(Device->PATT, 0x0A0A0A0AU);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HAL_FMC_NAND_Group2 Peripheral Control functions
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
  * @param  Device Pointer to NAND device instance
  * @param  Bank NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_ECC_Enable(FMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_NAND_BANK(Bank));

  /* Enable ECC feature */
  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  SET_BIT(Device->PCR, FMC_PCR_ECCEN);

  return HAL_OK;
}


/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device Pointer to NAND device instance
  * @param  Bank NAND bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_ECC_Disable(FMC_NAND_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_NAND_BANK(Bank));

  /* Disable ECC feature */
  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  CLEAR_BIT(Device->PCR, FMC_PCR_ECCEN);

  return HAL_OK;
}

/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device Pointer to NAND device instance
  * @param  ECCval Pointer to ECC value
  * @param  Bank NAND bank number
  * @param  Timeout Timeout wait value
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NAND_GetECC(const FMC_NAND_TypeDef *Device, uint32_t *ECCval, uint32_t Bank,
                                  uint32_t Timeout)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_FMC_NAND_DEVICE(Device));
  assert_param(IS_FMC_NAND_BANK(Bank));

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until FIFO is empty */
  while (__FMC_NAND_GET_FLAG(Device, Bank, FMC_FLAG_FEMPT) == RESET)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(Bank);

  /* Get the ECCR register value */
  *ECCval = (uint32_t)Device->HECCR;

  return HAL_OK;
}

/**
  * @}
  */


/** @defgroup FMC_LL_SDRAM
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

/** @addtogroup FMC_LL_SDRAM_Private_Functions_Group1
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
  * @param  Device Pointer to SDRAM device instance
  * @param  Init Pointer to SDRAM Initialization structure
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_Init(FMC_SDRAM_TypeDef *Device, const FMC_SDRAM_InitTypeDef *Init)
{
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
  assert_param(IS_FMC_READPIPE_DELAY(Init->ReadPipeDelay));

  /* Set SDRAM bank configuration parameters */
  if (Init->SDBank == FMC_SDRAM_BANK1)
  {
    MODIFY_REG(Device->SDCR[FMC_SDRAM_BANK1],
               SDCR_CLEAR_MASK,
               (Init->ColumnBitsNumber   |
                Init->RowBitsNumber      |
                Init->MemoryDataWidth    |
                Init->InternalBankNumber |
                Init->CASLatency         |
                Init->WriteProtection    |
                Init->SDClockPeriod      |
                Init->ReadPipeDelay));
  }
  else /* FMC_Bank2_SDRAM */
  {
    MODIFY_REG(Device->SDCR[FMC_SDRAM_BANK1],
               FMC_SDCRx_SDCLK           |
               FMC_SDCRx_RPIPE,
               (Init->SDClockPeriod      |
                Init->ReadPipeDelay));

    MODIFY_REG(Device->SDCR[FMC_SDRAM_BANK2],
               SDCR_CLEAR_MASK,
               (Init->ColumnBitsNumber   |
                Init->RowBitsNumber      |
                Init->MemoryDataWidth    |
                Init->InternalBankNumber |
                Init->CASLatency         |
                Init->WriteProtection));
  }

  return HAL_OK;
}


/**
  * @brief  Initializes the FMC_SDRAM device timing according to the specified
  *         parameters in the FMC_SDRAM_TimingTypeDef
  * @param  Device Pointer to SDRAM device instance
  * @param  Timing Pointer to SDRAM Timing structure
  * @param  Bank SDRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_Timing_Init(FMC_SDRAM_TypeDef *Device,
                                        const FMC_SDRAM_TimingTypeDef *Timing, uint32_t Bank)
{
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
  MODIFY_REG(Device->SDTR,
             SDTR_CLEAR_MASK,
             (((Timing->LoadToActiveDelay) - 1U)                                      |
              (((Timing->ExitSelfRefreshDelay) - 1U) << FMC_SDTRx_TXSR_Pos) |
              (((Timing->SelfRefreshTime) - 1U)      << FMC_SDTRx_TRAS_Pos) |
              (((Timing->RowCycleDelay) - 1U)        << FMC_SDTRx_TRC_Pos)  |
              (((Timing->WriteRecoveryTime) - 1U)    << FMC_SDTRx_TWR_Pos)  |
              (((Timing->RPDelay) - 1U)              << FMC_SDTRx_TRP_Pos)  |
              (((Timing->RCDDelay) - 1U)             << FMC_SDTRx_TRCD_Pos)));

  return HAL_OK;
}

/**
  * @brief  DeInitializes the FMC_SDRAM peripheral
  * @param  Device Pointer to SDRAM device instance
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_DeInit(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));

  /* De-initialize the SDRAM device */
  Device->SDCR[Bank] = 0x000002D0U;
  Device->SDTR       = 0x0FFFFFFFU;
  Device->SDCMR      = 0x00000000U;
  Device->SDRTR      = 0x00000000U;
  Device->SDSR       = 0x00000000U;

  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup FMC_LL_SDRAMPrivate_Functions_Group2
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
  * @param  Device Pointer to SDRAM device instance
  * @param  Bank SDRAM bank number
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_WriteProtection_Enable(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));

  /* Enable write protection */
  SET_BIT(Device->SDCR[Bank], FMC_SDRAM_WRITE_PROTECTION_ENABLE);

  return HAL_OK;
}

/**
  * @brief  Disables dynamically FMC_SDRAM write protection.
  * @param  hsdram FMC_SDRAM handle
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_SDRAM_WriteProtection_Disable(FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));

  /* Disable write protection */
  CLEAR_BIT(Device->SDCR[Bank], FMC_SDRAM_WRITE_PROTECTION_ENABLE);

  return HAL_OK;
}

/**
  * @brief  Send Command to the FMC SDRAM bank
  * @param  Device Pointer to SDRAM device instance
  * @param  Command Pointer to SDRAM command structure
  * @param  Timing Pointer to SDRAM Timing structure
  * @param  Timeout Timeout wait value
  * @retval HAL state
  */
HAL_StatusTypeDef FMC_SDRAM_SendCommand(FMC_SDRAM_TypeDef *Device,
                                        const FMC_SDRAM_CommandTypeDef *Command, uint32_t Timeout)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_COMMAND_MODE(Command->CommandMode));
  assert_param(IS_FMC_COMMAND_TARGET(Command->CommandTarget));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(Command->AutoRefreshNumber));
  assert_param(IS_FMC_MODE_REGISTER(Command->ModeRegisterDefinition));

  /* Set command register */
  MODIFY_REG(Device->SDCMR, (FMC_SDCMR_MODE | FMC_SDCMR_DS2 | FMC_SDCMR_DS1 | FMC_SDCMR_NRFS | FMC_SDCMR_MRD),
             ((Command->CommandMode) | (Command->CommandTarget) |
              (((Command->AutoRefreshNumber) - 1U) << FMC_SDCMR_NRFS_Pos) |
              ((Command->ModeRegisterDefinition) << FMC_SDCMR_MRD_Pos)));
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Timeout);
  return HAL_OK;
}

/**
  * @brief  Program the SDRAM Memory Refresh rate.
  * @param  Device Pointer to SDRAM device instance
  * @param  RefreshRate The SDRAM refresh rate value.
  * @retval HAL state
  */
HAL_StatusTypeDef FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_TypeDef *Device, uint32_t RefreshRate)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_REFRESH_RATE(RefreshRate));

  /* Set the refresh rate in command register */
  MODIFY_REG(Device->SDRTR, FMC_SDRTR_RFSCNT, (RefreshRate << FMC_SDRTR_RFSCNT_Pos));

  return HAL_OK;
}

/**
  * @brief  Set the Number of consecutive SDRAM Memory auto Refresh commands.
  * @param  Device Pointer to SDRAM device instance
  * @param  AutoRefreshNumber Specifies the auto Refresh number.
  * @retval None
  */
HAL_StatusTypeDef FMC_SDRAM_SetAutoRefreshNumber(FMC_SDRAM_TypeDef *Device,
                                                 uint32_t AutoRefreshNumber)
{
  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(AutoRefreshNumber));

  /* Set the Auto-refresh number in command register */
  MODIFY_REG(Device->SDCMR, FMC_SDCMR_NRFS, ((AutoRefreshNumber - 1U) << FMC_SDCMR_NRFS_Pos));

  return HAL_OK;
}

/**
  * @brief  Returns the indicated FMC SDRAM bank mode status.
  * @param  Device Pointer to SDRAM device instance
  * @param  Bank Defines the FMC SDRAM bank. This parameter can be
  *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM.
  * @retval The FMC SDRAM bank mode status, could be on of the following values:
  *         FMC_SDRAM_NORMAL_MODE, FMC_SDRAM_SELF_REFRESH_MODE or
  *         FMC_SDRAM_POWER_DOWN_MODE.
  */
uint32_t FMC_SDRAM_GetModeStatus(const FMC_SDRAM_TypeDef *Device, uint32_t Bank)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_FMC_SDRAM_DEVICE(Device));
  assert_param(IS_FMC_SDRAM_BANK(Bank));

  /* Get the corresponding bank mode */
  if (Bank == FMC_SDRAM_BANK1)
  {
    tmpreg = (uint32_t)(Device->SDSR & FMC_SDSR_MODES1);
  }
  else
  {
    tmpreg = ((uint32_t)(Device->SDSR & FMC_SDSR_MODES2) >> 2U);
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
/**
  * @}
  */
