/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rif.c
  * @author  MCD Application Team
  * @brief   RIF HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Resource Isolation Framework (RIF) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                ##### RIF main features #####
  ==============================================================================
  [..]
    (+) Resource Isolation framework (RIF) is a comprehensive set of hardware
    blocks designed to enforce and manage isolation of STM32 hardware resources
    like memory and peripherals.

    (+) Resource Isolation Framework (RIF) composed of the following sub-blocks:
      (++) RIMC:  Resource Isolation Master Controller
                  This sub-block assigns all non RIF-aware bus master to one
                  security domain by setting secure, privileged and compartment
                  information on the system bus.
                  Note that if non-secure software is permitted to configure one of
                  these peripherals, then this setting is ignored and all accesses
                  initiated by the peripheral are forced to be non-secure.
      (++) RISC:  Resource Isolation Slave Controller
                  This sub-block assigns all non-RIF aware peripherals to zero,
                  one or any security domains (secure, privilege, compartment).
      (++) RISAF: Resource Isolation Slave unit for Address space (Full version)
                  This sub-block assigns memory regions and subregions to one or
                  more security domains (secure, privilege, compartment).
      (++) IAC:   Illegal Access Controller
                  This sub-block centralizes detection of RIF-related accesses,
                  managed by a secure application. Supported illegal accesses are:
                  secure, privileged.

  [..]
    (+) All the setter functions are protected under the CPU_AS_TRUSTED_DOMAIN
        directive.
        It ensures that only the trusted domain can manage the critical
        configurations of the RIF.
        This directive is defined in the CMSIS file.

 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
  [..]
    *** Locking RIF components configuration ***
    ============================================================
    [..]
    (#) Lock and get the lock configuration of RIMC registers until next reset
        with HAL_RIF_RIMC_Lock / HAL_RIF_RIMC_GetLock.
    (#) Lock and get the lock configuration of RISC registers until next reset
        with HAL_RIF_RISC_Lock / HAL_RIF_RISC_GetLock.
    (#) Lock and get the lock configuration of slave resources registers until
        next reset with HAL_RIF_RISC_SlaveConfigLock / HAL_RIF_RISC_GetSlaveConfigLock.
    (#) Lock and get the lock configuration of RISAF registers until next reset
        with HAL_RIF_RISAF_Lock / HAL_RIF_RISAF_GetLock.

    *** Context-related functions ***
    ============================================================
    [..]
    (#) Set or get Debugger Access Port Compartment ID with HAL_RIF_RIMC_SetDebugAccessPortCID /
        HAL_RIF_RIMC_GetDebugAccessPortCID functions.
    (#) Check the Debug Domain and Global Debug Domain status with
        HAL_RIF_RIMC_GetDebugStatus.

#if defined(GENERATOR_RIF_FEATURE_ENCRYPTION_IMPLEMENTED)
    *** Encryption-related functions ***
    ============================================================
    [..]
    (#) Read of write RISAF encryption keys and related flags.

#endif
     *** Configuration of master-related attributes ***
    ============================================================
    [..]
    (#) Set or get the secure, privilege and compartment configuration of a
        bus master thanks to the HAL_RIF_RIMC_ConfigMasterAttributes /
        HAL_RIF_RIMC_GetConfigMasterAttributes functions.
    (#) Lock and get lock status the bus master attributes configuration until
        next system reset using HAL_RIF_RIMC_Lock.

     *** Configuration of slave-related attributes ***
    ============================================================
    [..]
    (#) Set or get the secure and privilege configuration of a bus slave
        thanks to the HAL_RIF_RISC_SetSlaveSecureAttributes /
        HAL_RIF_RISC_GetSlaveSecureAttributes functions.
    (#) Lock and get lock status each bus slave attributes configuration using
        HAL_RIF_RISC_SlaveConfigLock or lock the whole RISC configuration with
        HAL_RIF_RISC_Lock until next system reset.

     *** Configuration of RISAF attributes ***
    ============================================================
    [..]
    (#) Set or get a base region configuration with HAL_RIF_RISAF_ConfigBaseRegion /
        HAL_RIF_RISAF_GetConfigBaseRegion.
    (#) Set or get a subregion configuration with HAL_RIF_RISAF_ConfigSubRegion /
        HAL_RIF_RISAF_GetConfigSubRegion.
    (#) Delegate a subregion configuration to another CID with HAL_RIF_RISAF_ConfigSubRegionDelegation.
    (#) Get the data from an illegal access to a protected memory region with
        HAL_RIF_RISAF_GetIllegalAccess.

     *** IAC related functions ***
    ============================================================
    [..]
    (#) Enable or disable peripheral illegal access detection with HAL_RIF_IAC_EnableIT /
        HAL_RIF_IAC_DisableIT.
    (#) Retrieve an illegal access flag from a designated peripheral with HAL_RIF_IAC_GetFlag
        or clear it with HAL_RIF_IAC_ClearFlag.
    (#) Illegal access interrupt service routines are served by HAL_RIF_IRQHandler()
        and user can add his own code using HAL_RIF_ILA_Callback().

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup RIF RIF
  * @brief RIF HAL module driver.
  * @{
  */

#ifdef HAL_RIF_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/** @addtogroup RIF_Private_Defines RIF Private Defines
  * @{
  */
#define RIF_PERIPH_IAC_REG_MAX             4U
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup RIF_Exported_Functions RIF Exported Functions
  *  @{
  */

/** @defgroup RIF_Exported_Functions_Group1  RIMC Configuration functions
  *  @brief    RIMC Configuration functions
  *
@verbatim
  ==============================================================================
            ##### RIMC Configuration functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to configure RIMC
    RIMC is Resource Isolation Master Controller
@endverbatim
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Lock overall configuration of RIMC.
  * @note   Once set, the overall configuration of RIMC cannot be modified
  *         upon next system reset.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @retval None
  */
void HAL_RIF_RIMC_Lock(void)
{
  /* Set RIMC Lock bit */
  RIFSC->RIMC_CR |= RIFSC_RIMC_CR_GLOCK;
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE) */

/**
  * @brief  Get RIMC global lock status.
  * @retval RIF_LOCK_ENABLE if RIMC configuration is locked, else RIF_LOCK_DISABLE
  */
uint32_t HAL_RIF_RIMC_GetLock(void)
{
  uint32_t lock_status;

  /* Read RIMC lock bit */
  lock_status = (RIFSC->RIMC_CR & RIF_LOCK_ENABLE);

  return lock_status;
}

#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Change the Debugger Access Port ID
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @param  CID  specifies the Debugger Access Port CID
  *         This parameter can be a combination of following values:
  *           @arg @ref RIF_CID_0    Compartment 0 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_1    Compartment 1 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_2    Compartment 2 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_3    Compartment 3 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_4    Compartment 4 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_5    Compartment 5 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_6    Compartment 6 ID selected as Debug access port ID
  *           @arg @ref RIF_CID_7    Compartment 7 ID selected as Debug access port ID
  * @retval None
  */
void HAL_RIF_RIMC_SetDebugAccessPortCID(uint32_t CID)
{
  uint32_t dap_cid;
  uint32_t rimc_cr_reg;

  /* Check the parameter */
  assert_param(IS_RIF_SINGLE_CID(CID));

  /* Set Debug Access Port CID bits */
  dap_cid = POSITION_VAL(CID);
  rimc_cr_reg = RIFSC->RIMC_CR;
  rimc_cr_reg &= (~RIFSC_RIMC_CR_DAPCID);
  rimc_cr_reg |= (dap_cid << RIFSC_RIMC_CR_DAPCID_Pos);
  RIFSC->RIMC_CR = rimc_cr_reg;
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE) */

/**
  * @brief  Get the Debugger Access Port ID
  * @retval Debugger Access Port ID
  */
uint32_t HAL_RIF_RIMC_GetDebugAccessPortCID(void)
{
  uint32_t dap_cid;

  /* Read Debug Access Port CID bits */
  dap_cid = (RIFSC->RIMC_CR & RIFSC_RIMC_CR_DAPCID) >> RIFSC_RIMC_CR_DAPCID_Pos;
  dap_cid = (1UL << dap_cid);
  return dap_cid;
}

#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Configure CID, Secure and Privilege attributes for the designated master.
  * @note   RIF_CID_7 is not an authorized value for the MasterCID field.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @param  MasterId specifies the index of the bus master.
  *         This parameter can be one of @ref RIF_MASTER_INDEX
  * @param  pConfig Pointer on Master Isolation configuration structure
  * @retval None
  */
void HAL_RIF_RIMC_ConfigMasterAttributes(uint32_t MasterId, const RIMC_MasterConfig_t *pConfig)
{
  uint32_t master_cid;
  uint32_t rimc_attr_val;

  /* Check the parameter */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RIF_MASTER_INDEX(MasterId));
  assert_param(IS_RIF_SINGLE_CID(pConfig->MasterCID));
  assert_param(IS_RIF_MASTER_CID(pConfig->MasterCID));
  assert_param(IS_RIF_SEC_PRIV_ATTRIBUTE(pConfig->SecPriv));

  master_cid = POSITION_VAL(pConfig->MasterCID);
  rimc_attr_val = RIFSC->RIMC_ATTRx[MasterId];
  rimc_attr_val &= (~(RIFSC_RIMC_ATTRx_MCID | RIFSC_RIMC_ATTRx_MPRIV | RIFSC_RIMC_ATTRx_MSEC));
  rimc_attr_val |= ((master_cid << RIFSC_RIMC_ATTRx_MCID_Pos) | (pConfig->SecPriv << RIFSC_RIMC_ATTRx_MSEC_Pos));
  RIFSC->RIMC_ATTRx[MasterId] = rimc_attr_val;
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE) */

/**
  * @brief  Get the CID, Secure and Privilege attributes for the designated master.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  MasterId specifies the index of the bus master.
  *         This parameter can be one of @ref RIF_MASTER_INDEX
  * @param  pConfig Pointer on Master Isolation configuration structure
  * @retval None
  */
void HAL_RIF_RIMC_GetConfigMasterAttributes(uint32_t MasterId, RIMC_MasterConfig_t *pConfig)
{
  uint32_t master_cid;
  uint32_t rimc_attr_val;

  /* Check the parameter */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RIF_MASTER_INDEX(MasterId));

  rimc_attr_val = RIFSC->RIMC_ATTRx[MasterId];
  pConfig->SecPriv = ((rimc_attr_val & (RIFSC_RIMC_ATTRx_MPRIV | RIFSC_RIMC_ATTRx_MSEC))
                      >> RIFSC_RIMC_ATTRx_MSEC_Pos);
  master_cid = ((rimc_attr_val & RIFSC_RIMC_ATTRx_MCID) >> RIFSC_RIMC_ATTRx_MCID_Pos);
  pConfig->MasterCID = (1UL << master_cid);
}
/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group2  RIFSC Configuration functions
  *  @brief    RIFSC Configuration functions
  *
@verbatim
  ==============================================================================
            ##### RIFSC Configuration functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to configure RIFSC
    RIFSC is Resource Isolation Framework Secure Controller
@endverbatim
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Lock overall configuration of RISC.
  * @note   Once set, the overall configuration of RIFSC cannot be modified
  *         upon next system reset.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @retval None
  */
void HAL_RIF_RISC_Lock(void)
{
  /* Set RISC Lock bit */
  RIFSC->RISC_CR |= RIFSC_RISC_CR_GLOCK;
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE) */

/**
  * @brief  Get the RIFSC global lock status.
  * @retval 1 if RISC configuration is locked, else 0
  */
uint32_t HAL_RIF_RISC_GetLock(void)
{
  uint32_t lock_status;

  /* Read RISC lock bit */
  lock_status = (RIFSC->RISC_CR & RIF_LOCK_ENABLE);

  return lock_status;
}

/**
  * @brief  Configure the Security and Privilege of a designated slave peripheral.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @param  SecPriv specifies the security and privilege attributes of the peripheral.
  *         This parameter can be one or a combination of @ref RIF_SEC_PRIV
  * @retval None
  */
void HAL_RIF_RISC_SetSlaveSecureAttributes(uint32_t PeriphId, uint32_t SecPriv)
{
  __IO uint32_t sec_reg_val;

  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_RCC_PERIPH_INDEX(PeriphId));
  assert_param(IS_RIF_SEC_PRIV_ATTRIBUTE(SecPriv));

  sec_reg_val = RIFSC->RISC_SECCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT];
  sec_reg_val &= (~(1U << (PeriphId & RIF_PERIPH_BIT_POSITION)));
  sec_reg_val |= ((SecPriv & RIF_ATTRIBUTE_SEC) << (PeriphId & RIF_PERIPH_BIT_POSITION));
  RIFSC->RISC_SECCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT] = sec_reg_val;

  sec_reg_val = RIFSC->RISC_PRIVCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT];
  sec_reg_val &= (~(1U << (PeriphId & RIF_PERIPH_BIT_POSITION)));
  sec_reg_val |= (((SecPriv & RIF_ATTRIBUTE_PRIV) >> 1U) << (PeriphId & RIF_PERIPH_BIT_POSITION));
  RIFSC->RISC_PRIVCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT] = sec_reg_val;
}

/**
  * @brief  Get the Security and Privilege configuration of a designated slave peripheral.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval can be a combination of @ref RIF_SEC_PRIV
  */
uint32_t HAL_RIF_RISC_GetSlaveSecureAttributes(uint32_t PeriphId)
{
  uint32_t sec_attr;
  const __IO uint32_t *p_sec_reg;

  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_RCC_PERIPH_INDEX(PeriphId));

  p_sec_reg = &(RIFSC->RISC_SECCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  sec_attr = ((*p_sec_reg & ((uint32_t)RIF_ATTRIBUTE_SEC << (PeriphId & RIF_PERIPH_BIT_POSITION))) >> (PeriphId & RIF_PERIPH_BIT_POSITION));

  p_sec_reg = &(RIFSC->RISC_PRIVCFGRx[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  sec_attr |= (((*p_sec_reg & ((uint32_t)RIF_ATTRIBUTE_SEC << (PeriphId & RIF_PERIPH_BIT_POSITION))) >> (PeriphId & RIF_PERIPH_BIT_POSITION)) << 1U);

  return sec_attr;
}

#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Lock the Isolation and Security configuration of the designated slave peripheral.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval None
  */
void HAL_RIF_RISC_SlaveConfigLock(uint32_t PeriphId)
{
  __IO uint32_t *p_lock_reg;

  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_RCC_PERIPH_INDEX(PeriphId));

  /* Set RISC Peripheral Lock bit */
  p_lock_reg = &(RIFSC->RISC_RCFGLOCKRx[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  *p_lock_reg |= (RIF_LOCK_ENABLE << (PeriphId & RIF_PERIPH_BIT_POSITION));
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */

/**
  * @brief  Get the Isolation and Security configuration lock status.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval 1 if the Isolation and Security configuration is locked, else 0
  */
uint32_t HAL_RIF_RISC_GetSlaveConfigLock(uint32_t PeriphId)
{
  uint32_t lock_status;
  const __IO uint32_t *p_lock_reg;

  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_RCC_PERIPH_INDEX(PeriphId));

  p_lock_reg = &(RIFSC->RISC_RCFGLOCKRx[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  lock_status = ((*p_lock_reg & (RIF_LOCK_ENABLE << (PeriphId & RIF_PERIPH_BIT_POSITION))) >> (PeriphId & RIF_PERIPH_BIT_POSITION));

  return lock_status;
}

/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group4  RISAF Configuration functions
  *  @brief    RISAF Configuration functions
  *
@verbatim
  ==============================================================================
            ##### RISAF Configuration functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to configure RISAF
    RISAL is Resource Isolation Slave unit for Address space protection (Full version)
@endverbatim
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Lock overall configuration of RISAF.
  * @note   Once set, the overall configuration of RISAF cannot be modified
  *         upon next system reset.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @retval None
  */
void HAL_RIF_RISAF_Lock(RISAF_TypeDef *RISAFx)
{
  /* Check the parameters */
  assert_param(IS_RISAF_INSTANCE(RISAFx));

  /* Set RISAF Lock bit */
  RISAFx->CR |= RISAF_CR_GLOCK;
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */

/**
  * @brief  Get RISAF global lock status.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @retval 1 if RISAF configuration is locked, else 0
  */
uint32_t HAL_RIF_RISAF_GetLock(const RISAF_TypeDef *RISAFx)
{
  uint32_t lock_status;

  /* Read RIMC lock bit */
  lock_status = (RISAFx->CR & RIF_LOCK_ENABLE);

  return lock_status;
}

#if defined(GENERATOR_RIF_FEATURE_ENCRYPTION_IMPLEMENTED)
/**
  * @brief  Get encryption and key related flags.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @retval A combination of the three encryption related flags
  */
uint32_t HAL_RIF_RISAF_GetEncryptionStatus(RISAF_TypeDef *RISAFx)
{
  uint32_t tmp = 0;

  return tmp;
}

#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Write the encryption keys used by the DDRMCE.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Keys Pointer on the Encryption keys structure
  * @retval None
  */
void HAL_RIF_RISAF_WriteEncryptionKeys(RISAF_TypeDef *RISAFx, RISAF_EncryptionKeys_t *Keys)
{
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */

#endif /* defined(GENERATOR_RIF_FEATURE_ENCRYPTION_IMPLEMENTED) */
#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Configure a RISAF Base region.
  * @note   If pConfig->Filtering is equal to RISAF_FILTER_DISABLE, this API only disable the filtering without modifying
  *         its configuration.
  * @note   It is not possible to modify the start and end address of the region if the filtering is already enabled.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  pConfig Pointer on RISAF Base Region configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_ConfigBaseRegion(RISAF_TypeDef *RISAFx, uint32_t Region, const RISAF_BaseRegionConfig_t *pConfig)
{
  /* Check the parameters */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));

  /* Only disable the section */
  if (pConfig->Filtering == RISAF_FILTER_DISABLE)
  {
    RISAFx->REG[Region].CFGR &= (~(RISAF_FILTER_ENABLE));
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_RISAF_FILTERING(pConfig->Filtering));
    assert_param(IS_RIF_CID(pConfig->PrivWhitelist));
    assert_param(IS_RIF_CID(pConfig->ReadWhitelist));
    assert_param(IS_RIF_CID(pConfig->WriteWhitelist));
    assert_param(IS_RIF_SEC_PRIV_ATTRIBUTE(pConfig->Secure));
    assert_param(IS_RISAF_GRANULARITY(RISAFx, pConfig->StartAddress));
    assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->StartAddress));
    assert_param(IS_RISAF_GRANULARITY(RISAFx, (pConfig->EndAddress + 1U)));
    assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->EndAddress));
    assert_param(pConfig->StartAddress < pConfig->EndAddress);

    /* Update region start and end addresses */
    RISAFx->REG[Region].STARTR = pConfig->StartAddress;
    RISAFx->REG[Region].ENDR = pConfig->EndAddress;

    /* Update filtering mode, security and privilege attributes and whitelists */
    RISAFx->REG[Region].CIDCFGR = (pConfig->ReadWhitelist | (pConfig->WriteWhitelist << RISAF_REGx_CIDCFGR_WRENC0_Pos));
    RISAFx->REG[Region].CFGR = (pConfig->Filtering | (pConfig->Secure << RISAF_REGx_CFGR_SEC_Pos)
                                | (pConfig->PrivWhitelist << RISAF_REGx_CFGR_PRIVC0_Pos));
  }
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */

/**
  * @brief  Get the configuration of a RISAF Base region.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  pConfig Pointer on RISAF Base Region configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_GetConfigBaseRegion(const RISAF_TypeDef *RISAFx, uint32_t Region, RISAF_BaseRegionConfig_t *pConfig)
{
  uint32_t cfgr_reg;
  uint32_t cidcfgr_reg;

  /* Check the parameters */
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));
  assert_param(pConfig != (void *)NULL);

  cfgr_reg = RISAFx->REG[Region].CFGR;
  cidcfgr_reg = RISAFx->REG[Region].CIDCFGR;

  pConfig->Filtering = (cfgr_reg & RISAF_REGx_CFGR_BREN);
  pConfig->Secure = ((cfgr_reg & RISAF_REGx_CFGR_SEC) >> RISAF_REGx_CFGR_SEC_Pos);
  pConfig->PrivWhitelist = ((cfgr_reg & (RIF_CID_MASK << RISAF_REGx_CFGR_PRIVC0_Pos)) >> RISAF_REGx_CFGR_PRIVC0_Pos);
  pConfig->ReadWhitelist = (cidcfgr_reg & RIF_CID_MASK);
  pConfig->WriteWhitelist = ((cidcfgr_reg & (RIF_CID_MASK << RISAF_REGx_CIDCFGR_WRENC0_Pos)) >> RISAF_REGx_CIDCFGR_WRENC0_Pos);
  pConfig->StartAddress = RISAFx->REG[Region].STARTR;
  pConfig->EndAddress = RISAFx->REG[Region].ENDR;
}

#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Delegate a Subregion configuration to another CID.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  SubRegion specifies the RISAB Subregion index
  *         This parameter can be one of the following values:
  *           @arg @ref RISAF_SUBREGION_A
  *           @arg @ref RISAF_SUBREGION_B
  * @param  pConfig Pointer on RISAF Subregion configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_ConfigSubRegionDelegation(RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                             const RISAF_DelegationConfig_t *pConfig)
{
  uint32_t cid;
  uint32_t nestr_reg;

  /* Check the parameters */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));
  assert_param(IS_RISAF_SUBREGION(SubRegion));
  assert_param(IS_RISAF_DELEGATION(pConfig->Delegation));
  assert_param(IS_RIF_SINGLE_CID(pConfig->DelegatedCID));

  /* Convert mask into bit position */
  cid = POSITION_VAL(pConfig->DelegatedCID);

  if (SubRegion == RISAF_SUBREGION_A)
  {
    nestr_reg = RISAFx->REG[Region].ANESTR;
    nestr_reg &= (~(RISAF_REGx_zNESTR_DCCID | RISAF_REGx_zNESTR_DCEN));
    nestr_reg |= ((cid << RISAF_REGx_zNESTR_DCCID_Pos) | pConfig->Delegation);
    RISAFx->REG[Region].ANESTR = nestr_reg;
  }
  else
  {
    nestr_reg = RISAFx->REG[Region].BNESTR;
    nestr_reg &= (~(RISAF_REGx_zNESTR_DCCID | RISAF_REGx_zNESTR_DCEN));
    nestr_reg |= ((cid << RISAF_REGx_zNESTR_DCCID_Pos) | pConfig->Delegation);
    RISAFx->REG[Region].BNESTR = nestr_reg;
  }
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */

/**
  * @brief  Get the delegation configuration of a RISAF Subregion.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  SubRegion specifies the RISAB Subregion index
  *         This parameter can be one of the following values:
  *           @arg @ref RISAF_SUBREGION_A
  *           @arg @ref RISAF_SUBREGION_B
  * @param  pConfig Pointer on RISAF Subregion configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_GetConfigSubRegionDelegation(const RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                                RISAF_DelegationConfig_t *pConfig)
{
  uint32_t cid;

  /* Check the parameters */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));
  assert_param(IS_RISAF_SUBREGION(SubRegion));

  if (SubRegion == RISAF_SUBREGION_A)
  {
    pConfig->Delegation = (RISAFx->REG[Region].ANESTR & RISAF_REGx_zNESTR_DCEN);
    cid = ((RISAFx->REG[Region].ANESTR & RISAF_REGx_zNESTR_DCCID) >> RISAF_REGx_zNESTR_DCCID_Pos);
  }
  else
  {
    pConfig->Delegation = (RISAFx->REG[Region].BNESTR & RISAF_REGx_zNESTR_DCEN);
    cid = ((RISAFx->REG[Region].BNESTR & RISAF_REGx_zNESTR_DCCID) >> RISAF_REGx_zNESTR_DCCID_Pos);
  }

  pConfig->DelegatedCID = (1UL << cid);
}

/**
  * @brief  Configure a RISAF subregion.
  * @note   If pConfig->Filtering is equal to RISAF_FILTER_DISABLE, this API only disable the filtering without modifying
  *         its configuration. It also can lock the current subregion configuration.
  * @note   It is not possible to modify the start and end address of a subregion if the filtering is already enabled.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  SubRegion specifies the RISAB Subregion index
  *         This parameter can be one of the following values:
  *           @arg @ref RISAF_SUBREGION_A
  *           @arg @ref RISAF_SUBREGION_B
  * @param  pConfig Pointer on RISAF Base Region configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_ConfigSubRegion(RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                   const RISAF_SubRegionConfig_t *pConfig)
{
  uint32_t cid;

  /* Check the parameters */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));
  assert_param(IS_RISAF_SUBREGION(SubRegion));
  assert_param(IS_RISAF_FILTERING(pConfig->Filtering));
  assert_param(IS_RIF_LOCK_STATE(pConfig->Lock));

  /* Convert mask into bit position */
  cid = POSITION_VAL(pConfig->CID);

  if (SubRegion == RISAF_SUBREGION_A)
  {
    if (pConfig->Filtering == RISAF_FILTER_DISABLE)
    {
      RISAFx->REG[Region].ACFGR &= (~(RISAF_FILTER_ENABLE | RISAF_REGx_zCFGR_RLOCK));
      RISAFx->REG[Region].ACFGR |= (pConfig->Lock << RISAF_REGx_zCFGR_RLOCK_Pos);
    }
    else
    {
      /* Check the parameters */
      assert_param(IS_RISAF_READ_WRITE(pConfig->ReadWrite));
      assert_param(IS_RIF_SEC_PRIV_ATTRIBUTE(pConfig->SecPriv));
      assert_param(IS_RISAF_GRANULARITY(RISAFx, pConfig->StartAddress));
      assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->StartAddress));
      assert_param(IS_RISAF_GRANULARITY(RISAFx, (pConfig->EndAddress + 1U)));
      assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->EndAddress));
      assert_param(pConfig->StartAddress < pConfig->EndAddress);

      RISAFx->REG[Region].ASTARTR = pConfig->StartAddress;
      RISAFx->REG[Region].AENDR = pConfig->EndAddress;
      RISAFx->REG[Region].ACFGR = (pConfig->Filtering  | (pConfig->Lock << RISAF_REGx_zCFGR_RLOCK_Pos)
                                   | (cid << RISAF_REGx_zCFGR_SRCID_Pos) | (pConfig->SecPriv << RISAF_REGx_zCFGR_SEC_Pos)
                                   | (pConfig->ReadWrite));
    }
  }
  else
  {
    if (pConfig->Filtering == RISAF_FILTER_DISABLE)
    {
      RISAFx->REG[Region].BCFGR &= (~(RISAF_FILTER_ENABLE | RISAF_REGx_zCFGR_RLOCK));
      RISAFx->REG[Region].BCFGR |= (pConfig->Lock << RISAF_REGx_zCFGR_RLOCK_Pos);
    }
    else
    {
      /* Check the parameters */
      assert_param(IS_RISAF_READ_WRITE(pConfig->ReadWrite));
      assert_param(IS_RIF_SEC_PRIV_ATTRIBUTE(pConfig->SecPriv));
      assert_param(IS_RISAF_GRANULARITY(RISAFx, pConfig->StartAddress));
      assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->StartAddress));
      assert_param(IS_RISAF_GRANULARITY(RISAFx, (pConfig->EndAddress + 1U)));
      assert_param(IS_RISAF_LIMIT_ADDRESS_SPACE_SIZE(RISAFx, pConfig->EndAddress));
      assert_param(pConfig->StartAddress < pConfig->EndAddress);

      RISAFx->REG[Region].BSTARTR = pConfig->StartAddress;
      RISAFx->REG[Region].BENDR = pConfig->EndAddress;
      RISAFx->REG[Region].BCFGR = (pConfig->Filtering  | (pConfig->Lock << RISAF_REGx_zCFGR_RLOCK_Pos)
                                   | (cid << RISAF_REGx_zCFGR_SRCID_Pos) | (pConfig->SecPriv << RISAF_REGx_zCFGR_SEC_Pos)
                                   | (pConfig->ReadWrite));
    }
  }
}

/**
  * @brief  Get the configuration of a RISAF Subregion.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  Region specifies the RISAF region index
  *         This parameter can be one one of the following values:
  *           @arg @ref RISAF_REGION_1
  *           @arg @ref RISAF_REGION_2
  *           @arg @ref RISAF_REGION_3
  *           @arg @ref RISAF_REGION_4
  *           @arg @ref RISAF_REGION_5
  *           @arg @ref RISAF_REGION_6
  *           @arg @ref RISAF_REGION_7
  *           @arg @ref RISAF_REGION_8
  *           @arg @ref RISAF_REGION_9
  *           @arg @ref RISAF_REGION_10
  *           @arg @ref RISAF_REGION_11
  *           @arg @ref RISAF_REGION_12
  *           @arg @ref RISAF_REGION_13
  *           @arg @ref RISAF_REGION_14
  *           @arg @ref RISAF_REGION_15
  * @param  SubRegion specifies the RISAB Subregion index
  *         This parameter can be one of the following values:
  *           @arg @ref RISAF_SUBREGION_A
  *           @arg @ref RISAF_SUBREGION_B
  * @param  pConfig Pointer on RISAF Base Region configuration structure
  * @retval None
  */
void HAL_RIF_RISAF_GetConfigSubRegion(const RISAF_TypeDef *RISAFx, uint32_t Region, uint32_t SubRegion,
                                      RISAF_SubRegionConfig_t *pConfig)
{
  uint32_t cid;
  uint32_t cfgr_reg;

  /* Check the parameters */
  assert_param(pConfig != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));
  assert_param(IS_RISAF_REGION(Region));
  assert_param(IS_RISAF_MAX_REGION(RISAFx, Region));
  assert_param(IS_RISAF_SUBREGION(SubRegion));

  if (SubRegion == RISAF_SUBREGION_A)
  {
    cfgr_reg = RISAFx->REG[Region].ACFGR;
    pConfig->StartAddress = RISAFx->REG[Region].ASTARTR;
    pConfig->EndAddress = RISAFx->REG[Region].AENDR;
  }
  else
  {
    cfgr_reg = RISAFx->REG[Region].BCFGR;
    pConfig->StartAddress = RISAFx->REG[Region].BSTARTR;
    pConfig->EndAddress = RISAFx->REG[Region].BENDR;
  }

  pConfig->Filtering = (cfgr_reg & RISAF_REGx_zCFGR_SREN);
  cid = ((cfgr_reg & RISAF_REGx_zCFGR_SRCID) >> RISAF_REGx_zCFGR_SRCID_Pos);
  pConfig->CID = (1UL << cid);
  pConfig->SecPriv = ((cfgr_reg & (RISAF_REGx_zCFGR_PRIV | RISAF_REGx_zCFGR_SEC)) >> RISAF_REGx_zCFGR_SEC_Pos);
  pConfig->ReadWrite = (cfgr_reg & (RISAF_REGx_zCFGR_WREN | RISAF_REGx_zCFGR_RDEN));
  pConfig->Lock = ((cfgr_reg & RISAF_REGx_zCFGR_RLOCK) >> RISAF_REGx_zCFGR_RLOCK_Pos);
}

#if defined(CPU_AS_TRUSTED_DOMAIN)
/**
  * @brief  Get the error detected by the RISAF control block,
  *         the illegal access data and clear related flags.
  *         Error information can be illegal access or control access type. In case of an illegal access detection, the
  *         error information is:
  *           - responsible CID;
  *           - security access type;
  *           - privilege access type;
  *           - read (fetch) or write access type;
  *           - targeted address.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  RISAFx specifies the RISAF instance.
  *         This parameter is one of the RISAF instances defined in the CMSIS.
  * @param  IllegalAccess Pointer on illegal access structure
  * @retval None
  */
void HAL_RIF_RISAF_GetIllegalAccess(RISAF_TypeDef *RISAFx, RISAF_IllegalAccess_t *IllegalAccess)
{
  uint32_t cid;
  uint32_t secpriv;

  /* Check the parameters */
  assert_param(IllegalAccess != (void *)NULL);
  assert_param(IS_RISAF_INSTANCE(RISAFx));

  /* Get illegal access type */
  IllegalAccess->ErrorType = (RISAFx->IASR & (RISAF_IASR_IAEF | RISAF_IASR_CAEF));

  if (IllegalAccess->ErrorType != RISAF_ILLEGAL_ACCESS_NONE)
  {
    /* Get illegal access data */
    cid = (RISAFx->IAR->IAESR & RISAF_IAESR_IACID);
    IllegalAccess->Data.CID = (1UL << cid);
    secpriv = ((RISAFx->IAR->IAESR & (RISAF_IAESR_IASEC | RISAF_IAESR_IAPRIV)) >> RISAF_IAESR_IAPRIV_Pos);
    IllegalAccess->Data.SecPriv = ((secpriv >> 1UL) | (secpriv << 1UL)) & 3UL;
    IllegalAccess->Data.AccessType = ((RISAFx->IAR->IAESR & RISAF_IAESR_IANRW) >> RISAF_IAESR_IANRW_Pos);

    if ((IllegalAccess->ErrorType & RISAF_ILLEGAL_ACCESS) != 0U)
    {
      IllegalAccess->Data.Address = RISAFx->IAR->IADDR;
    }
    else
    {
      IllegalAccess->Data.Address = 0U;
    }

    /* Clear flags */
    RISAFx->IACR = (RISAF_IACR_IAEF | RISAF_IACR_CAEF);
  }
}
#endif /* defined(CPU_AS_TRUSTED_DOMAIN) */
/**
  * @}
  */

/** @defgroup RIF_Exported_Functions_Group6  IAC Configuration functions
  * @brief    IAC Configuration functions
  *
@verbatim
  ==============================================================================
            ##### IAC Configuration functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to configure IAC
    IAC is Illegal Access Controller
@endverbatim
  * @{
  */
#if defined(CPU_AS_TRUSTED_DOMAIN) && defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Enable interrupt generation on illegal access detection on peripheral or memory access.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval None
  */
void HAL_RIF_IAC_EnableIT(uint32_t PeriphId)
{
  __IO uint32_t *p_interrupt_reg;

  /* Check parameters */
  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_AWARE_PERIPH_INDEX(PeriphId));

  p_interrupt_reg = &(IAC->IER[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  *p_interrupt_reg |= (1UL << (PeriphId & RIF_PERIPH_BIT_POSITION));

}

/**
  * @brief  Disable interrupt generation on illegal access detection on peripheral or memory access.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval None
  */
void HAL_RIF_IAC_DisableIT(uint32_t PeriphId)
{
  __IO uint32_t *p_interrupt_reg;

  /* Check parameters */
  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_AWARE_PERIPH_INDEX(PeriphId));

  p_interrupt_reg = &(IAC->IER[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  *p_interrupt_reg &= (~(1UL << (PeriphId & RIF_PERIPH_BIT_POSITION)));
}

/**
  * @brief  Get illegal access detection flag.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval Detection flag value
  */
uint32_t HAL_RIF_IAC_GetFlag(uint32_t PeriphId)
{
  uint32_t interrupt_flag;
  const __IO uint32_t *p_interrupt_reg;
  uint32_t shift_var;

  /* Check parameters */
  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_AWARE_PERIPH_INDEX(PeriphId));

  shift_var = (PeriphId & RIF_PERIPH_BIT_POSITION);
  p_interrupt_reg = &(IAC->ISR[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  interrupt_flag = ((*p_interrupt_reg & (1UL << shift_var)) >> shift_var);

  return interrupt_flag;
}

/**
  * @brief  Clear illegal access detection flag.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval None
  */
void HAL_RIF_IAC_ClearFlag(uint32_t PeriphId)
{
  __IO uint32_t *p_interrupt_reg;

  /* Check parameters */
  assert_param(IS_RIF_RISC_PERIPH_INDEX(PeriphId) || IS_RIF_AWARE_PERIPH_INDEX(PeriphId));

  p_interrupt_reg = &(IAC->ICR[PeriphId >> RIF_PERIPH_REG_SHIFT]);
  *p_interrupt_reg = (1UL << (PeriphId & RIF_PERIPH_BIT_POSITION));
}

/**
  * @brief  Parse all pending flags and call callback when necessary.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @retval None
  */
void HAL_RIF_IRQHandler(void)
{
  uint32_t reg_index;
  uint32_t flag_mask;
  uint32_t position;
  uint32_t periph_id;

  for (reg_index = 0 ; reg_index < (RIF_PERIPH_IAC_REG_MAX + 1U) ; reg_index++)
  {
    flag_mask = IAC->ISR[reg_index];

    position = 0U;
    while ((flag_mask >> position) != 0U)
    {
      if ((flag_mask & (1UL << position)) != 0U)
      {
        periph_id = ((reg_index << RIF_PERIPH_REG_SHIFT) | position);
        HAL_RIF_IAC_ClearFlag(periph_id);
        HAL_RIF_ILA_Callback(periph_id);
      }
      position++;
    }
  }
}

/**
  * @brief  RIF illegal access callback.
  * @note   This API is protected by the Trusted Domain compilation directive.
  * @note   This API is protected by the TrustZone Enabled compilation directive.
  * @param  PeriphId specifies the index of the bus slave.
  *         This parameter can be one of @ref RIF_PERIPHERAL_INDEX
  *         bits[31:28] define the register index a peripheral belongs to, value from 0 to 5
  *         bits[4:0]   define the bit position within the register, value from 0 to 31
  * @retval None
  */
__weak void HAL_RIF_ILA_Callback(uint32_t PeriphId)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(PeriphId);

  /* NOTE: This function should not be modified, when the callback is needed,
                 the HAL_RIF_ILA_Callback could be implemented in the user file
  */
}
#endif /* CPU_AS_TRUSTED_DOMAIN && CPU_IN_SECURE_STATE */
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_RIF_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
