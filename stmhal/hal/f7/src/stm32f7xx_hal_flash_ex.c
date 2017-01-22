/**
  ******************************************************************************
  * @file    stm32f7xx_hal_flash_ex.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Extended FLASH HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the FLASH extension peripheral:
  *           + Extended programming operations functions
  *
  @verbatim
  ==============================================================================
                   ##### Flash Extension features #####
  ==============================================================================

  [..] Comparing to other previous devices, the FLASH interface for STM32F76xx/STM32F77xx
       devices contains the following additional features

       (+) Capacity up to 2 Mbyte with dual bank architecture supporting read-while-write
           capability (RWW)
       (+) Dual bank memory organization
       (+) Dual boot mode

                      ##### How to use this driver #####
  ==============================================================================
  [..] This driver provides functions to configure and program the FLASH memory
       of all STM32F7xx devices. It includes
      (#) FLASH Memory Erase functions:
           (++) Lock and Unlock the FLASH interface using HAL_FLASH_Unlock() and
                HAL_FLASH_Lock() functions
           (++) Erase function: Erase sector, erase all sectors
           (++) There are two modes of erase :
             (+++) Polling Mode using HAL_FLASHEx_Erase()
             (+++) Interrupt Mode using HAL_FLASHEx_Erase_IT()

      (#) Option Bytes Programming functions: Use HAL_FLASHEx_OBProgram() to :
           (++) Set/Reset the write protection
           (++) Set the Read protection Level
           (++) Set the BOR level
           (++) Program the user Option Bytes

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
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup FLASHEx FLASHEx
  * @brief FLASH HAL Extension module driver
  * @{
  */

#ifdef HAL_FLASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup FLASHEx_Private_Constants
  * @{
  */
#define SECTOR_MASK               0xFFFFFF07U
#define FLASH_TIMEOUT_VALUE       50000U/* 50 s */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup FLASHEx_Private_Variables
  * @{
  */
extern FLASH_ProcessTypeDef pFlash;
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup FLASHEx_Private_Functions
  * @{
  */
/* Option bytes control */
static HAL_StatusTypeDef  FLASH_OB_EnableWRP(uint32_t WRPSector);
static HAL_StatusTypeDef  FLASH_OB_DisableWRP(uint32_t WRPSector);
static HAL_StatusTypeDef  FLASH_OB_RDP_LevelConfig(uint8_t Level);
static HAL_StatusTypeDef  FLASH_OB_BOR_LevelConfig(uint8_t Level);
static HAL_StatusTypeDef  FLASH_OB_BootAddressConfig(uint32_t BootOption, uint32_t Address);
static uint32_t           FLASH_OB_GetUser(void);
static uint32_t           FLASH_OB_GetWRP(void);
static uint8_t            FLASH_OB_GetRDP(void);
static uint32_t           FLASH_OB_GetBOR(void);
static uint32_t           FLASH_OB_GetBootAddress(uint32_t BootOption);

#if defined (FLASH_OPTCR_nDBANK)
static void               FLASH_MassErase(uint8_t VoltageRange, uint32_t Banks);
static HAL_StatusTypeDef  FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, \
                                              uint32_t Iwdgstdby, uint32_t NDBank, uint32_t NDBoot);
#else
static void               FLASH_MassErase(uint8_t VoltageRange);
static HAL_StatusTypeDef  FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, uint32_t Iwdgstdby);
#endif /* FLASH_OPTCR_nDBANK */

extern HAL_StatusTypeDef  FLASH_WaitForLastOperation(uint32_t Timeout);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Functions FLASHEx Exported Functions
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
    This subsection provides a set of functions allowing to manage the Extension FLASH
    programming operations Operations.

@endverbatim
  * @{
  */
/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors
  * @param[in]  pEraseInit: pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  *
  * @param[out]  SectorError: pointer to variable  that
  *         contains the configuration information on faulty sector in case of error
  *         (0xFFFFFFFF means that all the sectors have been correctly erased)
  *
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  uint32_t index = 0;

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /*Initialization of SectorError variable*/
    *SectorError = 0xFFFFFFFFU;

    if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /*Mass erase to be done*/
#if defined (FLASH_OPTCR_nDBANK)
      FLASH_MassErase((uint8_t) pEraseInit->VoltageRange, pEraseInit->Banks);
#else
      FLASH_MassErase((uint8_t) pEraseInit->VoltageRange);
#endif /* FLASH_OPTCR_nDBANK */

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

      /* if the erase operation is completed, disable the MER Bit */
      FLASH->CR &= (~FLASH_MER_BIT);
    }
    else
    {
      /* Check the parameters */
      assert_param(IS_FLASH_NBSECTORS(pEraseInit->NbSectors + pEraseInit->Sector));

      /* Erase by sector by sector to be done*/
      for(index = pEraseInit->Sector; index < (pEraseInit->NbSectors + pEraseInit->Sector); index++)
      {
        FLASH_Erase_Sector(index, (uint8_t) pEraseInit->VoltageRange);

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

        /* If the erase operation is completed, disable the SER Bit and SNB Bits */
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB));

        if(status != HAL_OK)
        {
          /* In case of error, stop erase procedure and return the faulty sector*/
          *SectorError = index;
          break;
        }
      }
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors  with interrupt enabled
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

  /* Enable End of FLASH Operation interrupt */
  __HAL_FLASH_ENABLE_IT(FLASH_IT_EOP);

  /* Enable Error source interrupt */
  __HAL_FLASH_ENABLE_IT(FLASH_IT_ERR);

  /* Clear pending flags (if any) */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |\
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR| FLASH_FLAG_ERSERR);

  if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
  {
    /*Mass erase to be done*/
    pFlash.ProcedureOnGoing = FLASH_PROC_MASSERASE;
#if defined (FLASH_OPTCR_nDBANK)
    FLASH_MassErase((uint8_t) pEraseInit->VoltageRange, pEraseInit->Banks);
#else
    FLASH_MassErase((uint8_t) pEraseInit->VoltageRange);
#endif /* FLASH_OPTCR_nDBANK */
  }
  else
  {
    /* Erase by sector to be done*/

    /* Check the parameters */
    assert_param(IS_FLASH_NBSECTORS(pEraseInit->NbSectors + pEraseInit->Sector));

    pFlash.ProcedureOnGoing = FLASH_PROC_SECTERASE;
    pFlash.NbSectorsToErase = pEraseInit->NbSectors;
    pFlash.Sector = pEraseInit->Sector;
    pFlash.VoltageForErase = (uint8_t)pEraseInit->VoltageRange;

    /*Erase 1st sector and wait for IT*/
    FLASH_Erase_Sector(pEraseInit->Sector, pEraseInit->VoltageRange);
  }

  return status;
}

/**
  * @brief  Program option bytes
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

  /* Write protection configuration */
  if((pOBInit->OptionType & OPTIONBYTE_WRP) == OPTIONBYTE_WRP)
  {
    assert_param(IS_WRPSTATE(pOBInit->WRPState));
    if(pOBInit->WRPState == OB_WRPSTATE_ENABLE)
    {
      /*Enable of Write protection on the selected Sector*/
      status = FLASH_OB_EnableWRP(pOBInit->WRPSector);
    }
    else
    {
      /*Disable of Write protection on the selected Sector*/
      status = FLASH_OB_DisableWRP(pOBInit->WRPSector);
    }
  }

  /* Read protection configuration */
  if((pOBInit->OptionType & OPTIONBYTE_RDP) == OPTIONBYTE_RDP)
  {
    status = FLASH_OB_RDP_LevelConfig(pOBInit->RDPLevel);
  }

  /* USER  configuration */
  if((pOBInit->OptionType & OPTIONBYTE_USER) == OPTIONBYTE_USER)
  {
#if defined (FLASH_OPTCR_nDBANK)
    status = FLASH_OB_UserConfig(pOBInit->USERConfig & OB_WWDG_SW,
                                 pOBInit->USERConfig & OB_IWDG_SW,
                                 pOBInit->USERConfig & OB_STOP_NO_RST,
                                 pOBInit->USERConfig & OB_STDBY_NO_RST,
                                 pOBInit->USERConfig & OB_IWDG_STOP_ACTIVE,
                                 pOBInit->USERConfig & OB_IWDG_STDBY_ACTIVE,
                                 pOBInit->USERConfig & OB_NDBANK_SINGLE_BANK,
                                 pOBInit->USERConfig & OB_DUAL_BOOT_DISABLE);
#else
    status = FLASH_OB_UserConfig(pOBInit->USERConfig & OB_WWDG_SW,
                                 pOBInit->USERConfig & OB_IWDG_SW,
                                 pOBInit->USERConfig & OB_STOP_NO_RST,
                                 pOBInit->USERConfig & OB_STDBY_NO_RST,
                                 pOBInit->USERConfig & OB_IWDG_STOP_ACTIVE,
                                 pOBInit->USERConfig & OB_IWDG_STDBY_ACTIVE);
#endif /* FLASH_OPTCR_nDBANK */
  }

  /* BOR Level  configuration */
  if((pOBInit->OptionType & OPTIONBYTE_BOR) == OPTIONBYTE_BOR)
  {
    status = FLASH_OB_BOR_LevelConfig(pOBInit->BORLevel);
  }

  /* Boot 0 Address configuration */
  if((pOBInit->OptionType & OPTIONBYTE_BOOTADDR_0) == OPTIONBYTE_BOOTADDR_0)
  {
    status = FLASH_OB_BootAddressConfig(OPTIONBYTE_BOOTADDR_0, pOBInit->BootAddr0);
  }

  /* Boot 1 Address configuration */
  if((pOBInit->OptionType & OPTIONBYTE_BOOTADDR_1) == OPTIONBYTE_BOOTADDR_1)
  {
    status = FLASH_OB_BootAddressConfig(OPTIONBYTE_BOOTADDR_1, pOBInit->BootAddr1);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

/**
  * @brief  Get the Option byte configuration
  * @param  pOBInit: pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  *
  * @retval None
  */
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit)
{
  pOBInit->OptionType = OPTIONBYTE_WRP | OPTIONBYTE_RDP | OPTIONBYTE_USER |\
	                OPTIONBYTE_BOR | OPTIONBYTE_BOOTADDR_0 | OPTIONBYTE_BOOTADDR_1;

  /*Get WRP*/
  pOBInit->WRPSector = FLASH_OB_GetWRP();

  /*Get RDP Level*/
  pOBInit->RDPLevel = FLASH_OB_GetRDP();

  /*Get USER*/
  pOBInit->USERConfig = FLASH_OB_GetUser();

  /*Get BOR Level*/
  pOBInit->BORLevel = FLASH_OB_GetBOR();

  /*Get Boot Address when Boot pin = 0 */
  pOBInit->BootAddr0 = FLASH_OB_GetBootAddress(OPTIONBYTE_BOOTADDR_0);

  /*Get Boot Address when Boot pin = 1 */
  pOBInit->BootAddr1 = FLASH_OB_GetBootAddress(OPTIONBYTE_BOOTADDR_1);
}
/**
  * @}
  */

#if defined (FLASH_OPTCR_nDBANK)
/**
  * @brief  Full erase of FLASH memory sectors
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V,
  *                                  the operation will be done by byte (8-bit)
  *            @arg VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp,
  *                                  the operation will be done by double word (64-bit)
  * @param  Banks: Banks to be erased
  *          This parameter can be one of the following values:
  *            @arg FLASH_BANK_1: Bank1 to be erased
  *            @arg FLASH_BANK_2: Bank2 to be erased
  *            @arg FLASH_BANK_BOTH: Bank1 and Bank2 to be erased
  *
  * @retval HAL Status
  */
static void FLASH_MassErase(uint8_t VoltageRange, uint32_t Banks)
{
  /* Check the parameters */
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  assert_param(IS_FLASH_BANK(Banks));

  /* if the previous operation is completed, proceed to erase all sectors */
  FLASH->CR &= CR_PSIZE_MASK;
  if(Banks == FLASH_BANK_BOTH)
  {
    /* bank1 & bank2 will be erased*/
    FLASH->CR |= FLASH_MER_BIT;
  }
  else if(Banks == FLASH_BANK_2)
  {
    /*Only bank2 will be erased*/
    FLASH->CR |= FLASH_CR_MER2;
  }
  else
  {
    /*Only bank1 will be erased*/
    FLASH->CR |= FLASH_CR_MER1;
  }
  FLASH->CR |= FLASH_CR_STRT | ((uint32_t)VoltageRange <<8);
  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}

/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector: FLASH sector to erase
  *         The value of this parameter depend on device used within the same series
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V,
  *                                  the operation will be done by byte (8-bit)
  *            @arg FLASH_VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg FLASH_VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg FLASH_VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp,
  *                                  the operation will be done by double word (64-bit)
  *
  * @retval None
  */
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0;

  /* Check the parameters */
  assert_param(IS_FLASH_SECTOR(Sector));
  assert_param(IS_VOLTAGERANGE(VoltageRange));

  if(VoltageRange == FLASH_VOLTAGE_RANGE_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;
  }
  else if(VoltageRange == FLASH_VOLTAGE_RANGE_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;
  }
  else if(VoltageRange == FLASH_VOLTAGE_RANGE_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
  }

  /* Need to add offset of 4 when sector higher than FLASH_SECTOR_11 */
  if(Sector > FLASH_SECTOR_11)
  {
    Sector += 4;
  }

  /* If the previous operation is completed, proceed to erase the sector */
  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= tmp_psize;
  CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);
  FLASH->CR |= FLASH_CR_SER | (Sector << POSITION_VAL(FLASH_CR_SNB));
  FLASH->CR |= FLASH_CR_STRT;

  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}

/**
  * @brief  Return the FLASH Write Protection Option Bytes value.
  * @retval uint32_t FLASH Write Protection Option Bytes value
  */
static uint32_t FLASH_OB_GetWRP(void)
{
  /* Return the FLASH write protection Register value */
  return ((uint32_t)(FLASH->OPTCR & 0x0FFF0000));
}

/**
  * @brief  Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.
  * @param  Wwdg: Selects the IWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_WWDG_SW: Software WWDG selected
  *            @arg OB_WWDG_HW: Hardware WWDG selected
  * @param  Iwdg: Selects the WWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_IWDG_SW: Software IWDG selected
  *            @arg OB_IWDG_HW: Hardware IWDG selected
  * @param  Stop: Reset event when entering STOP mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STOP_NO_RST: No reset generated when entering in STOP
  *            @arg OB_STOP_RST: Reset generated when entering in STOP
  * @param  Stdby: Reset event when entering Standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STDBY_NO_RST: No reset generated when entering in STANDBY
  *            @arg OB_STDBY_RST: Reset generated when entering in STANDBY
  * @param  Iwdgstop: Independent watchdog counter freeze in Stop mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STOP_FREEZE: Freeze IWDG counter in STOP
  *            @arg OB_IWDG_STOP_ACTIVE: IWDG counter active in STOP
  * @param  Iwdgstdby: Independent watchdog counter freeze in standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STDBY_FREEZE: Freeze IWDG counter in STANDBY
  *            @arg OB_IWDG_STDBY_ACTIVE: IWDG counter active in STANDBY
  * @param  NDBank: Flash Single Bank mode enabled.
  *          This parameter  can be one of the following values:
  *            @arg OB_NDBANK_SINGLE_BANK: enable 256 bits mode (Flash is a single bank)
  *            @arg OB_NDBANK_DUAL_BANK: disable 256 bits mode (Flash is a dual bank in 128 bits mode)
  * @param  NDBoot: Flash Dual boot mode disable.
  *          This parameter  can be one of the following values:
  *            @arg OB_DUAL_BOOT_DISABLE: Disable Dual Boot
  *            @arg OB_DUAL_BOOT_ENABLE: Enable Dual Boot

  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, \
                                             uint32_t Iwdgstdby, uint32_t NDBank, uint32_t NDBoot)
{
  uint32_t useroptionmask = 0x00;
  uint32_t useroptionvalue = 0x00;

  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_WWDG_SOURCE(Wwdg));
  assert_param(IS_OB_IWDG_SOURCE(Iwdg));
  assert_param(IS_OB_STOP_SOURCE(Stop));
  assert_param(IS_OB_STDBY_SOURCE(Stdby));
  assert_param(IS_OB_IWDG_STOP_FREEZE(Iwdgstop));
  assert_param(IS_OB_IWDG_STDBY_FREEZE(Iwdgstdby));
  assert_param(IS_OB_NDBANK(NDBank));
  assert_param(IS_OB_NDBOOT(NDBoot));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    useroptionmask = (FLASH_OPTCR_WWDG_SW | FLASH_OPTCR_IWDG_SW | FLASH_OPTCR_nRST_STOP | \
                      FLASH_OPTCR_nRST_STDBY | FLASH_OPTCR_IWDG_STOP | FLASH_OPTCR_IWDG_STDBY | \
                      FLASH_OPTCR_nDBOOT | FLASH_OPTCR_nDBANK);

    useroptionvalue = (Iwdg | Wwdg | Stop | Stdby | Iwdgstop | Iwdgstdby | NDBoot | NDBank);

    /* Update User Option Byte */
    MODIFY_REG(FLASH->OPTCR, useroptionmask, useroptionvalue);
  }

  return status;
}

/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval uint32_t FLASH User Option Bytes values: WWDG_SW(Bit4), IWDG_SW(Bit5), nRST_STOP(Bit6),
  *         nRST_STDBY(Bit7), nDBOOT(Bit28), nDBANK(Bit29), IWDG_STDBY(Bit30) and IWDG_STOP(Bit31).
  */
static uint32_t FLASH_OB_GetUser(void)
{
  /* Return the User Option Byte */
  return ((uint32_t)(FLASH->OPTCR & 0xF00000F0U));
}
#else

/**
  * @brief  Full erase of FLASH memory sectors
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V,
  *                                  the operation will be done by byte (8-bit)
  *            @arg VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp,
  *                                  the operation will be done by double word (64-bit)
  *
  * @retval HAL Status
  */
static void FLASH_MassErase(uint8_t VoltageRange)
{
  /* Check the parameters */
  assert_param(IS_VOLTAGERANGE(VoltageRange));

  /* if the previous operation is completed, proceed to erase all sectors */
  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= FLASH_CR_MER;
  FLASH->CR |= FLASH_CR_STRT | ((uint32_t)VoltageRange <<8);
  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}

/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector: FLASH sector to erase
  *         The value of this parameter depend on device used within the same series
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V,
  *                                  the operation will be done by byte (8-bit)
  *            @arg FLASH_VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg FLASH_VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg FLASH_VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp,
  *                                  the operation will be done by double word (64-bit)
  *
  * @retval None
  */
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0;

  /* Check the parameters */
  assert_param(IS_FLASH_SECTOR(Sector));
  assert_param(IS_VOLTAGERANGE(VoltageRange));

  if(VoltageRange == FLASH_VOLTAGE_RANGE_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;
  }
  else if(VoltageRange == FLASH_VOLTAGE_RANGE_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;
  }
  else if(VoltageRange == FLASH_VOLTAGE_RANGE_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
  }

  /* If the previous operation is completed, proceed to erase the sector */
  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= tmp_psize;
  FLASH->CR &= SECTOR_MASK;
  FLASH->CR |= FLASH_CR_SER | (Sector << POSITION_VAL(FLASH_CR_SNB));
  FLASH->CR |= FLASH_CR_STRT;

  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}

/**
  * @brief  Return the FLASH Write Protection Option Bytes value.
  * @retval uint32_t FLASH Write Protection Option Bytes value
  */
static uint32_t FLASH_OB_GetWRP(void)
{
  /* Return the FLASH write protection Register value */
  return ((uint32_t)(FLASH->OPTCR & 0x00FF0000));
}

/**
  * @brief  Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.
  * @param  Wwdg: Selects the IWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_WWDG_SW: Software WWDG selected
  *            @arg OB_WWDG_HW: Hardware WWDG selected
  * @param  Iwdg: Selects the WWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_IWDG_SW: Software IWDG selected
  *            @arg OB_IWDG_HW: Hardware IWDG selected
  * @param  Stop: Reset event when entering STOP mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STOP_NO_RST: No reset generated when entering in STOP
  *            @arg OB_STOP_RST: Reset generated when entering in STOP
  * @param  Stdby: Reset event when entering Standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STDBY_NO_RST: No reset generated when entering in STANDBY
  *            @arg OB_STDBY_RST: Reset generated when entering in STANDBY
  * @param  Iwdgstop: Independent watchdog counter freeze in Stop mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STOP_FREEZE: Freeze IWDG counter in STOP
  *            @arg OB_IWDG_STOP_ACTIVE: IWDG counter active in STOP
  * @param  Iwdgstdby: Independent watchdog counter freeze in standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_IWDG_STDBY_FREEZE: Freeze IWDG counter in STANDBY
  *            @arg OB_IWDG_STDBY_ACTIVE: IWDG counter active in STANDBY
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_UserConfig(uint32_t Wwdg, uint32_t Iwdg, uint32_t Stop, uint32_t Stdby, uint32_t Iwdgstop, uint32_t Iwdgstdby)
{
  uint32_t useroptionmask = 0x00;
  uint32_t useroptionvalue = 0x00;

  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_WWDG_SOURCE(Wwdg));
  assert_param(IS_OB_IWDG_SOURCE(Iwdg));
  assert_param(IS_OB_STOP_SOURCE(Stop));
  assert_param(IS_OB_STDBY_SOURCE(Stdby));
  assert_param(IS_OB_IWDG_STOP_FREEZE(Iwdgstop));
  assert_param(IS_OB_IWDG_STDBY_FREEZE(Iwdgstdby));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    useroptionmask = (FLASH_OPTCR_WWDG_SW | FLASH_OPTCR_IWDG_SW | FLASH_OPTCR_nRST_STOP | \
                      FLASH_OPTCR_nRST_STDBY | FLASH_OPTCR_IWDG_STOP | FLASH_OPTCR_IWDG_STDBY);

    useroptionvalue = (Iwdg | Wwdg | Stop | Stdby | Iwdgstop | Iwdgstdby);

    /* Update User Option Byte */
    MODIFY_REG(FLASH->OPTCR, useroptionmask, useroptionvalue);
  }

  return status;

}

/**
  * @brief  Return the FLASH User Option Byte value.
  * @retval uint32_t FLASH User Option Bytes values: WWDG_SW(Bit4), IWDG_SW(Bit5), nRST_STOP(Bit6),
  *         nRST_STDBY(Bit7), IWDG_STDBY(Bit30) and IWDG_STOP(Bit31).
  */
static uint32_t FLASH_OB_GetUser(void)
{
  /* Return the User Option Byte */
  return ((uint32_t)(FLASH->OPTCR & 0xC00000F0U));
}
#endif /* FLASH_OPTCR_nDBANK */

/**
  * @brief  Enable the write protection of the desired bank1 or bank2 sectors
  *
  * @note   When the memory read protection level is selected (RDP level = 1),
  *         it is not possible to program or erase the flash sector i if CortexM7
  *         debug features are connected or boot code is executed in RAM, even if nWRPi = 1
  *
  * @param  WRPSector: specifies the sector(s) to be write protected.
  *          This parameter can be one of the following values:
  *            @arg WRPSector: A value between OB_WRP_SECTOR_0 and OB_WRP_SECTOR_7 (for STM32F74xxx/STM32F75xxx devices)
  *              or a value between OB_WRP_SECTOR_0 and OB_WRP_SECTOR_11 (in Single Bank mode for STM32F76xxx/STM32F77xxx devices)
  *              or a value between OB_WRP_DB_SECTOR_0 and OB_WRP_DB_SECTOR_23 (in Dual Bank mode for STM32F76xxx/STM32F77xxx devices)
  *            @arg OB_WRP_SECTOR_All
  *
  * @retval HAL FLASH State
  */
static HAL_StatusTypeDef FLASH_OB_EnableWRP(uint32_t WRPSector)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_WRP_SECTOR(WRPSector));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /*Write protection enabled on sectors */
    FLASH->OPTCR &= (~WRPSector);
  }

  return status;
}

/**
  * @brief  Disable the write protection of the desired bank1 or bank 2 sectors
  *
  * @note   When the memory read protection level is selected (RDP level = 1),
  *         it is not possible to program or erase the flash sector i if CortexM4
  *         debug features are connected or boot code is executed in RAM, even if nWRPi = 1
  *
  * @param  WRPSector: specifies the sector(s) to be write protected.
  *          This parameter can be one of the following values:
  *            @arg WRPSector: A value between OB_WRP_SECTOR_0 and OB_WRP_SECTOR_7 (for STM32F74xxx/STM32F75xxx devices)
  *              or a value between OB_WRP_SECTOR_0 and OB_WRP_SECTOR_11 (in Single Bank mode for STM32F76xxx/STM32F77xxx devices)
  *              or a value between OB_WRP_DB_SECTOR_0 and OB_WRP_DB_SECTOR_23 (in Dual Bank mode for STM32F76xxx/STM32F77xxx devices)
  *            @arg OB_WRP_Sector_All
  *
  *
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_DisableWRP(uint32_t WRPSector)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_WRP_SECTOR(WRPSector));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /* Write protection disabled on sectors */
    FLASH->OPTCR |= (WRPSector);
  }

  return status;
}

/**
  * @brief  Set the read protection level.
  * @param  Level: specifies the read protection level.
  *          This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  *
  * @note WARNING: When enabling OB_RDP level 2 it's no more possible to go back to level 1 or 0
  *
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_RDP_LevelConfig(uint8_t Level)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_RDP_LEVEL(Level));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    *(__IO uint8_t*)OPTCR_BYTE1_ADDRESS = Level;
  }

  return status;
}

/**
  * @brief  Set the BOR Level.
  * @param  Level: specifies the Option Bytes BOR Reset Level.
  *          This parameter can be one of the following values:
  *            @arg OB_BOR_LEVEL3: Supply voltage ranges from 2.7 to 3.6 V
  *            @arg OB_BOR_LEVEL2: Supply voltage ranges from 2.4 to 2.7 V
  *            @arg OB_BOR_LEVEL1: Supply voltage ranges from 2.1 to 2.4 V
  *            @arg OB_BOR_OFF: Supply voltage ranges from 1.62 to 2.1 V
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_BOR_LevelConfig(uint8_t Level)
{
  /* Check the parameters */
  assert_param(IS_OB_BOR_LEVEL(Level));

  /* Set the BOR Level */
  MODIFY_REG(FLASH->OPTCR, FLASH_OPTCR_BOR_LEV, Level);

  return HAL_OK;

}

/**
  * @brief  Configure Boot base address.
  *
  * @param   BootOption : specifies Boot base address depending from Boot pin = 0 or pin = 1
  *          This parameter can be one of the following values:
  *            @arg OPTIONBYTE_BOOTADDR_0 : Boot address based when Boot pin = 0
  *            @arg OPTIONBYTE_BOOTADDR_1 : Boot address based when Boot pin = 1
  * @param   Address: specifies Boot base address
  *          This parameter can be one of the following values:
  *            @arg OB_BOOTADDR_ITCM_RAM : Boot from ITCM RAM (0x00000000)
  *            @arg OB_BOOTADDR_SYSTEM : Boot from System memory bootloader (0x00100000)
  *            @arg OB_BOOTADDR_ITCM_FLASH : Boot from Flash on ITCM interface (0x00200000)
  *            @arg OB_BOOTADDR_AXIM_FLASH : Boot from Flash on AXIM interface (0x08000000)
  *            @arg OB_BOOTADDR_DTCM_RAM : Boot from DTCM RAM (0x20000000)
  *            @arg OB_BOOTADDR_SRAM1 : Boot from SRAM1 (0x20010000)
  *            @arg OB_BOOTADDR_SRAM2 : Boot from SRAM2 (0x2004C000)
  *
  * @retval HAL Status
  */
static HAL_StatusTypeDef FLASH_OB_BootAddressConfig(uint32_t BootOption, uint32_t Address)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OB_BOOT_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    if(BootOption == OPTIONBYTE_BOOTADDR_0)
    {
      MODIFY_REG(FLASH->OPTCR1, FLASH_OPTCR1_BOOT_ADD0, Address);
    }
    else
    {
      MODIFY_REG(FLASH->OPTCR1, FLASH_OPTCR1_BOOT_ADD1, (Address << 16));
    }
  }

  return status;
}

/**
  * @brief  Returns the FLASH Read Protection level.
  * @retval FlagStatus FLASH ReadOut Protection Status:
  *         This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Read protection of the memory
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  */
static uint8_t FLASH_OB_GetRDP(void)
{
  uint8_t readstatus = OB_RDP_LEVEL_0;

  if ((*(__IO uint8_t*)(OPTCR_BYTE1_ADDRESS)) == OB_RDP_LEVEL_0)
  {
    readstatus = OB_RDP_LEVEL_0;
  }
  else if ((*(__IO uint8_t*)(OPTCR_BYTE1_ADDRESS)) == OB_RDP_LEVEL_2)
  {
    readstatus = OB_RDP_LEVEL_2;
  }
  else
  {
    readstatus = OB_RDP_LEVEL_1;
  }

  return readstatus;
}

/**
  * @brief  Returns the FLASH BOR level.
  * @retval uint32_t The FLASH BOR level:
  *           - OB_BOR_LEVEL3: Supply voltage ranges from 2.7 to 3.6 V
  *           - OB_BOR_LEVEL2: Supply voltage ranges from 2.4 to 2.7 V
  *           - OB_BOR_LEVEL1: Supply voltage ranges from 2.1 to 2.4 V
  *           - OB_BOR_OFF   : Supply voltage ranges from 1.62 to 2.1 V
  */
static uint32_t FLASH_OB_GetBOR(void)
{
  /* Return the FLASH BOR level */
  return ((uint32_t)(FLASH->OPTCR & 0x0C));
}

/**
  * @brief  Configure Boot base address.
  *
  * @param   BootOption : specifies Boot base address depending from Boot pin = 0 or pin = 1
  *          This parameter can be one of the following values:
  *            @arg OPTIONBYTE_BOOTADDR_0 : Boot address based when Boot pin = 0
  *            @arg OPTIONBYTE_BOOTADDR_1 : Boot address based when Boot pin = 1
  *
  * @retval uint32_t Boot Base Address:
  *            - OB_BOOTADDR_ITCM_RAM : Boot from ITCM RAM (0x00000000)
  *            - OB_BOOTADDR_SYSTEM : Boot from System memory bootloader (0x00100000)
  *            - OB_BOOTADDR_ITCM_FLASH : Boot from Flash on ITCM interface (0x00200000)
  *            - OB_BOOTADDR_AXIM_FLASH : Boot from Flash on AXIM interface (0x08000000)
  *            - OB_BOOTADDR_DTCM_RAM : Boot from DTCM RAM (0x20000000)
  *            - OB_BOOTADDR_SRAM1 : Boot from SRAM1 (0x20010000)
  *            - OB_BOOTADDR_SRAM2 : Boot from SRAM2 (0x2004C000)
  */
static uint32_t FLASH_OB_GetBootAddress(uint32_t BootOption)
{
  uint32_t Address = 0;

	/* Return the Boot base Address */
  if(BootOption == OPTIONBYTE_BOOTADDR_0)
  {
    Address = FLASH->OPTCR1 & FLASH_OPTCR1_BOOT_ADD0;
	}
  else
	{
		Address = ((FLASH->OPTCR1 & FLASH_OPTCR1_BOOT_ADD1) >> 16);
	}

  return Address;
}

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
