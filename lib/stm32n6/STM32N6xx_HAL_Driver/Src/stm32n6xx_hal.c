/**
  ******************************************************************************
  * @file    stm32n6xx_hal.c
  * @author  MCD Application Team
  * @brief   HAL module driver.
  *          This is the common part of the HAL initialization
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
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The common HAL driver contains a set of generic and common APIs that can be
    used by the PPP peripheral drivers and the user to start using the HAL.
    [..]
    The HAL contains two APIs' categories:
         (+) Common HAL APIs
         (+) Services HAL APIs
    [..]
    All the SYSCFG functions have the same privilege and security attributes.
         (+) Functions can operate in non-privileged code should
             privileged and unprivileged data access are granted to the
             peripheral (see HAL_RIF_RISC_SetSlaveSecureAttributes)

         (+) Functions  can operate in non-secure code should
             secure and non-secure data access are granted to the peripheral
             (see HAL_RIF_RISC_SetSlaveSecureAttributes).

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup HAL HAL
  * @brief HAL module driver
  * @{
  */

#ifdef HAL_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HAL_TIMEOUT_SYSCFG_ABORT        5U    /* 5 ms */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/** @defgroup HAL_Exported_Variables HAL Exported Variables
  * @{
  */
__IO uint32_t uwTick;
uint32_t uwTickPrio   = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup HAL_Exported_Functions HAL Exported Functions
  * @{
  */

/** @defgroup HAL_Exported_Functions_Group1 Initialization and de-initialization Functions
  * @brief    Initialization and de-initialization functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initializes the NVIC allocation and initial clock configuration.
          It initializes the systick also when timeout is needed
          and the backup domain when enabled.
      (+) De-Initializes common part of the HAL.
      (+) Configure The time base source to have 1ms time base with a dedicated
          Tick interrupt priority.
        (++) SysTick timer is used by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
        (++) Time base configuration function (HAL_InitTick ()) is called automatically
             at the beginning of the program after reset by HAL_Init() or at any time
             when clock is configured, by HAL_RCC_ClockConfig().
        (++) Source of time base is configured  to generate interrupts at regular
             time intervals. Care must be taken if HAL_Delay() is called from a
             peripheral ISR process, the Tick interrupt line must have higher priority
            (numerically lower) than the peripheral interrupt. Otherwise the caller
            ISR process will be blocked.
       (++) functions affecting time base configurations are declared as __weak
             to make  override possible  in case of other  implementations in user file.
@endverbatim
  * @{
  */

/**
  * @brief  Configure the time base source, NVIC and any required global low
  *         level hardware by calling the HAL_MspInit() callback function to be optionally defined
  *         in user file stm32n6xx_hal_msp.c.
  *
  * @note   HAL_Init() function is called at the beginning of program after reset and before
  *         the clock configuration.
  *
  * @note   In the default implementation the System Timer (Systick) is used as source of time base.
  *         The Systick configuration is based on HSI clock, as HSI is the clock
  *         used after a system Reset and the NVIC configuration is set to Priority group 4.
  *         Once done, time base tick starts incrementing: the tick variable counter is incremented
  *         each 1ms in the SysTick_Handler() interrupt handler.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_Init(void)
{
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Ensure time base clock coherency in SystemCoreClock global variable */
  SystemCoreClockUpdate();

  /* Initialize 1ms tick time base (default SysTick based on HSI clock after Reset) */
  if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Init the low level hardware */
  HAL_MspInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  This function de-initializes all peripherals and low level hardware resources.
  * @note   This function should only be called from code running in internal RAM
  *         since it resets the external peripheral memory interfaces.
  * @note   This function is optional.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DeInit(void)
{
  /* Reset of all peripherals */
  __HAL_RCC_APB1_FORCE_RESET();
  __HAL_RCC_APB1_RELEASE_RESET();

  __HAL_RCC_APB2_FORCE_RESET();
  __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_APB4_FORCE_RESET();
  __HAL_RCC_APB4_RELEASE_RESET();

  __HAL_RCC_APB5_FORCE_RESET();
  __HAL_RCC_APB5_RELEASE_RESET();

  __HAL_RCC_AHB1_FORCE_RESET();
  __HAL_RCC_AHB1_RELEASE_RESET();

  __HAL_RCC_AHB2_FORCE_RESET();
  __HAL_RCC_AHB2_RELEASE_RESET();

  __HAL_RCC_AHB3_FORCE_RESET();
  __HAL_RCC_AHB3_RELEASE_RESET();

  __HAL_RCC_AHB4_FORCE_RESET();
  __HAL_RCC_AHB4_RELEASE_RESET();

  __HAL_RCC_AHB5_FORCE_RESET();
  __HAL_RCC_AHB5_RELEASE_RESET();

  /* De-Init the low level hardware */
  HAL_MspDeInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the MSP.
  * @retval None
  */
__weak void HAL_MspInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the MSP.
  * @retval None
  */
__weak void HAL_MspDeInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief This function configures the source of the time base.
  *        The time source is configured to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function is called  automatically at the beginning of program after
  *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals.
  *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
  *       The SysTick interrupt must have higher priority (numerically lower)
  *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  *       The function is declared as __weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that don't take the value zero)*/
  if ((uint32_t)uwTickFreq == 0UL)
  {
    return HAL_ERROR;
  }

  /* Configure the SysTick to have interrupt in 1ms time basis*/
  if (HAL_SYSTICK_Config(SystemCoreClock / (1000UL / (uint32_t)uwTickFreq)) > 0U)
  {
    return HAL_ERROR;
  }

  /* Configure the SysTick IRQ priority */
  if (TickPriority < (1UL << __NVIC_PRIO_BITS))
  {
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
    uwTickPrio = TickPriority;
  }
  else
  {
    return HAL_ERROR;
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HAL_Group2 HAL Control functions
  *  @brief    HAL Control functions
  *
@verbatim
 ===============================================================================
                      ##### HAL Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Provide a tick value in millisecond
      (+) Provide a blocking delay in millisecond
      (+) Suspend the time base source interrupt
      (+) Resume the time base source interrupt
      (+) Get the HAL API driver version
      (+) Get the device identifier
      (+) Get the device revision identifier
      (+) Enable/Disable Debug module during SLEEP mode
      (+) Enable/Disable Debug module during STOP mode
      (+) Enable/Disable Debug module during STANDBY mode

@endverbatim
  * @{
  */

/**
  * @brief This function is called to increment a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_IncTick(void)
{
  uwTick += (uint32_t)uwTickFreq;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval tick value
  */
__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/**
  * @brief This function returns a tick priority.
  * @retval tick priority
  */
uint32_t HAL_GetTickPrio(void)
{
  return uwTickPrio;
}

/**
  * @brief Set new tick Freq.
  * @param Freq Tick frequency, value of @ref HAL_TickFreqTypeDef.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq)
{
  HAL_StatusTypeDef status  = HAL_OK;
  HAL_TickFreqTypeDef prevTickFreq;

  assert_param(IS_TICKFREQ(Freq));

  if (uwTickFreq != Freq)
  {
    /* Back up uwTickFreq frequency */
    prevTickFreq = uwTickFreq;

    /* Update uwTickFreq global variable used by HAL_InitTick() */
    uwTickFreq = Freq;

    /* Apply the new tick Freq  */
    status = HAL_InitTick(uwTickPrio);
    if (status != HAL_OK)
    {
      /* Restore previous tick frequency */
      uwTickFreq = prevTickFreq;
    }
  }

  return status;
}

/**
  * @brief Return tick frequency.
  * @retval Tick frequency.
  *         Value of @ref HAL_TickFreqTypeDef.
  */
HAL_TickFreqTypeDef HAL_GetTickFreq(void)
{
  return uwTickFreq;
}

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay Specifies the delay time length, in milliseconds.
  * @retval None
  */
__weak void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}

/**
  * @brief Suspend Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_SuspendTick()
  *       is called, the SysTick interrupt will be disabled and so Tick increment
  *       is suspended.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief Resume Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the SysTick interrupt will be enabled and so Tick increment
  *       is resumed.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief  Returns the HAL revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
  return __STM32N6xx_HAL_VERSION;
}

#if defined(CPU_IN_SECURE_STATE)
/**
  * @brief  Returns the device revision identifier
  * @note Returned Revision ID can be
  *       0x00000100 for Cut1.0
  *       0x00000101 for Cut1.1
  *       0x00000200 for Cut2.0
  * @retval Device revision identifier
  */
uint32_t HAL_GetREVID(void)
{
  return *(uint32_t *)(REVID_BASE);
}

/**
  * @brief  Returns the device identifier
  * @note This function can only be used if the HAL BSEC module is enabled in the hal configuration file.
  *       The BSEC clock must be enabled before calling this function
  *       Returned Device ID can be
  *       0x00006200 for STM32N645xx
  *       0x00006000 for STM32N655xx
  *       0x00002200 for STM32N647xx
  *       0x00002000 for STM32N657xx
  *       0xFFFFFFFF if an error occurs
  * @retval Device identifier
  */
uint32_t HAL_GetDEVID(void)
{
#if defined(HAL_BSEC_MODULE_ENABLED)
#define BSEC_FUSE_ADDRESS               9U
  uint32_t data;
  if(__HAL_RCC_BSEC_IS_CLK_ENABLED() == 0UL){
    data = 0xFFFFFFFFU;
  }
  else
  {
    BSEC_HandleTypeDef sBsecHandler = {0};
    sBsecHandler.Instance = BSEC;

    if(HAL_BSEC_OTP_Read(&sBsecHandler, BSEC_FUSE_ADDRESS, &data) != HAL_OK){
      data = 0xFFFFFFFFU;
    }
  }
  return data;
#else
  return 0xFFFFFFFFU;
#endif /* HAL_BSEC_MODULE_ENABLED */
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief  Return the first word of the unique device identifier (UID based on 96 bits)
  * @note   The application must ensures that SYSCFG clock is enabled.
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw0(void)
{
  return (READ_REG(*((uint32_t *)UID_BASE)));
}

/**
  * @brief  Return the second word of the unique device identifier (UID based on 96 bits)
  * @note   The application must ensures that SYSCFG clock is enabled.
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw1(void)
{
  return (READ_REG(*((uint32_t *)(UID_BASE + 4U))));
}

/**
  * @brief  Return the third word of the unique device identifier (UID based on 96 bits)
  * @note   The application must ensures that SYSCFG clock is enabled.
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw2(void)
{
  return (READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

/**
  * @}
  */

/** @defgroup HAL_Exported_Functions_Group3 HAL Debug functions
  *  @brief    HAL Debug functions
  *
@verbatim
 ===============================================================================
                      ##### HAL Debug functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Enable/Disable Debug module during Sleep mode
      (+) Enable/Disable Debug module during Stop modes
      (+) Enable/Disable Debug module during Stanby mode

@endverbatim
  * @{
  */

/**
  * @brief  Enable the Debug Module during Sleep mode.
  * @retval None
  */
void HAL_DBGMCU_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Disable the Debug Module during Sleep mode.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Enable the Debug Module during Stop mode.
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Disable the Debug Module during Stop mode.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Enable the Debug Module during Stanby mode.
  * @retval None
  */
void HAL_DBGMCU_EnableDBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @brief  Disable the Debug Module during Stanby mode.
  * @retval None
  */
void HAL_DBGMCU_DisableDBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);
}

/**
  * @}
  */

/** @defgroup HAL_Exported_Functions_Group4 HAL SYSCFG configuration functions
  *  @brief    HAL SYSCFG configuration functions
  *
@verbatim
 ===============================================================================
                      ##### HAL SYSCFG configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Enable or disable BOOT pins pull-down.
      (+) Lock and get locks status of system items.
      (+) Set (and get) TCM size and DTCM size.
      (+) Lock (and get) write accesses.
      (+) Enable or disable wait state on extended ITCM and DTCM.
      (+) Enable or disable external RW margin input for TCM memories.
      (+) Set and get the TCM biasing level.
      (+) Enable or disable the external RW for caches memories.
      (+) Set and get the cache biasing level.
      (+) Enable or disable the Power-on reset upon SW system request.
      (+) Enable or disable that Lockup requests a warm reset to the RCC
      (+) Enable or disable that Lockup generates a NMI on the core
      (+) Clear Error capture in write posting buffer
      (+) Reserve VENCRAM for VENC or release it (any use allowed)
      (+) Enable or disable PKA, SAES, CRYP1/2 and HASH reset in case of potential tamper
      (+) Set/Get write/read Qos for NP1, NP2 or CPU.
      (+) Enable or disable SDMMC early-write response.
      (+) Enable or disable USB early-write response.
      (+) Enable or disable the NPU NIC, NPU NOC or CPU NOC clock gating.
      (+) Enable or disable the VDDIOx compensation cell.
      (+) Configure or get VDDIOx compensation cells.
      (+) Get VDDIOx compensation ready status.
      (+) Enable or disable the VDD compensation cell.
      (+) Configure or get VDD compensation cells.
      (+) Configure or get the Timer Break input for error flag
      (+) Set and get secure OS allocation of specific CID to the DMA channel
      (+) Set and get non-secure OS allocation of specific CID to the DMA channel
      (+) Enable or disable Retiming on RX path.
      (+) Enable or disable Retiming on TX path.
      (+) Set and get Delay on feedback clock.
      (+) Enable or disable the interleaving on NPU RAM.
      (+) Get BOOT pin connection status
      (+) Get the address of first error in P-AHB write-posting buffer

@endverbatim
  * @{
  */

/**
  * @brief  Enable the Pull Down on BOOT pin.
  * @param  BootId Specifies the boot pins.
  *         This parameter can one of the following values:
  *            @arg SYSCFG_BOOT_0 boot 0 pin
  *            @arg SYSCFG_BOOT_1 boot 1 pin
  * @retval None
  */
void HAL_SYSCFG_EnablePullDown(uint32_t BootId)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_ID(BootId));

  SET_BIT(SYSCFG->BOOTCR, BootId);
}

/**
  * @brief  Disable the Pull Down on BOOT pin.
  * @param  BootId Specifies the boot pins.
  *         This parameter can one of the following values:
  *            @arg SYSCFG_BOOT_0 boot 0 pin
  *            @arg SYSCFG_BOOT_1 boot 1 pin
  * @retval None
  */
void HAL_SYSCFG_DisablePullDown(uint32_t BootId)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_ID(BootId));

  CLEAR_BIT(SYSCFG->BOOTCR, BootId);
}

/**
  * @brief  Lock the SYSCFG item(s).
  * @note   Lock(s) cleared only at system reset
  * @param  Item Item(s) to set lock on.
  *         This parameter can be a combination of @ref SYSCFG_Lock_items
  * @retval None
  */
void HAL_SYSCFG_Lock(uint32_t Item)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_LOCK_ITEMS(Item));

  MODIFY_REG(SYSCFG->CM55CR, SYSCFG_LOCK_ALL, Item);
}

/**
  * @brief  Get the lock state of SYSCFG item.
  * @param  pItem Pointer to return locked items
  *         the return value can be a combination of @ref SYSCFG_Lock_items
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetLock(uint32_t *pItem)
{
  /* Check null pointer */
  if (pItem == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure lock state */
  *pItem = READ_BIT(SYSCFG->CM55CR, SYSCFG_LOCK_ALL);

  return HAL_OK;
}

/**
  * @brief  Set the DTCM and ITCM memory sizes.
  * @note   Write-once TCM configuration change.
  * @note   Effective change applied upon new power-on reset by calling
  *         HAL_SYSCFG_EnablePowerOnReset() and NVIC_SystemReset()
  * @param  DtcmSize DTCM memory size
  *         This parameter can one of the following values:
  *            @arg SYSCFG_DTCM_128K 128K
  *            @arg SYSCFG_DTCM_256K 256K
  * @param  ItcmSize ITCM memory size
  *         This parameter can one of the following values:
  *            @arg SYSCFG_ITCM_64K  64K
  *            @arg SYSCFG_ITCM_128K 128K
  *            @arg SYSCFG_ITCM_256K 256K
  * @retval None
  */
void HAL_SYSCFG_SetTCMSize(uint32_t DtcmSize, uint32_t ItcmSize)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_DTCM_SIZE(DtcmSize));
  assert_param(IS_SYSCFG_ITCM_SIZE(ItcmSize));

  MODIFY_REG(SYSCFG->CM55TCMCR, SYSCFG_CM55TCMCR_CFGDTCMSZ | SYSCFG_CM55TCMCR_CFGITCMSZ, DtcmSize | ItcmSize);
}

/**
  * @brief  Get the DTCM and ITCM memory sizes.
  * @param  pDtcmSize Pointer to return DTCM memory size
  *         This return value can be one of
  *            @arg SYSCFG_DTCM_128K 128K
  *            @arg SYSCFG_DTCM_256K 256K
  * @param  pItcmSize Pointer to return ITCM memory size
  *         This return value can be one of
  *            @arg SYSCFG_ITCM_64K  64K
  *            @arg SYSCFG_ITCM_128K 128K
  *            @arg SYSCFG_ITCM_256K 256K
  * @retval HAL status
   */
HAL_StatusTypeDef HAL_SYSCFG_GetTCMSize(uint32_t *pDtcmSize, uint32_t *pItcmSize)
{
  uint32_t values;

  /* Check null pointer */
  if ((pDtcmSize == NULL) || (pItcmSize == NULL))
  {
    return HAL_ERROR;
  }

  values = READ_REG(SYSCFG->CM55TCMCR);
  *pDtcmSize = (values & 0xF0U);
  *pItcmSize = (values & 0xFU);

  return HAL_OK;
}

/**
  * @brief  Lock the SYSCFG write access item(s).
  * @note   Lock(s) cleared only at system reset
  * @param  Item Item(s) to set lock on.
  *         This parameter can be a combination of @ref SYSCFG_WRITE_access
  * @retval None
  */
void HAL_SYSCFG_LockWriteAccess(uint32_t Item)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_LOCK_WRACCESS(Item));

  MODIFY_REG(SYSCFG->CM55TCMCR, SYSCFG_LOCK_WR_ALL, Item);
}

/**
  * @brief  Get the lock state of SYSCFG  write access item.
  * @param  pItem pointer to return locked items
  *         the return value can be a combination of @ref SYSCFG_WRITE_access
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetLockWriteAccess(uint32_t *pItem)
{
  /* Check null pointer */
  if (pItem == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure lock state */
  *pItem = READ_BIT(SYSCFG->CM55TCMCR, SYSCFG_LOCK_WR_ALL);

  return HAL_OK;
}

/**
  * @brief  Enable the ITCM Wait State.
  * @retval None
  */
void HAL_SYSCFG_EnableITCMWaiteState(void)
{
  SET_BIT(SYSCFG->CM55TCMCR, SYSCFG_CM55TCMCR_ITCMWSDISABLE);
}

/**
  * @brief  Disable the ITCM Wait State.
  * @retval None
  */
void HAL_SYSCFG_DisableITCMWaiteState(void)
{
  CLEAR_BIT(SYSCFG->CM55TCMCR, SYSCFG_CM55TCMCR_ITCMWSDISABLE);
}

/**
  * @brief  Enable the DTCM Wait State.
  * @retval None
  */
void HAL_SYSCFG_EnableDTCMWaiteState(void)
{
  SET_BIT(SYSCFG->CM55TCMCR, SYSCFG_CM55TCMCR_DTCMWSDISABLE);
}

/**
  * @brief  Disable the DTCM Wait State.
  * @retval None
  */
void HAL_SYSCFG_DisableDTCMWaiteState(void)
{
  CLEAR_BIT(SYSCFG->CM55TCMCR, SYSCFG_CM55TCMCR_DTCMWSDISABLE);
}

/**
  * @brief  Enable external RW margin inputs for TCM memories
  * @retval None
  */
void HAL_SYSCFG_EnableTCMExternalMargin(void)
{
  SET_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RME_TCM);
}

/**
  * @brief  Disable external RW margin inputs for TCM memories
  * @retval None
  */
void HAL_SYSCFG_DisableTCMExternalMargin(void)
{
  CLEAR_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RME_TCM);
}

/**
  * @brief  Set external RW margin inputs for TCM memories.
  * @param  TcmRwMarginInput Input Margin.
  * @retval None
  */
void HAL_SYSCFG_SetTCMRWMarginInput(uint32_t TcmRwMarginInput)
{
  assert_param(IS_TCM_MARGIN_INPUT(TcmRwMarginInput));

  MODIFY_REG(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RM_TCM, (TcmRwMarginInput << 1U));
}

/**
  * @brief  Get the External RW margin inputs for TCM memories.
  * @param  pTcmRwMarginInput pointer to return Input Margin.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_SYSCFG_GetTCMRWMarginInput(uint32_t *pTcmRwMarginInput)
{
  /* Check null pointer */
  if (pTcmRwMarginInput == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure lock state */
  *pTcmRwMarginInput = (READ_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RM_TCM) >> 1U);

  return HAL_OK;
}

/**
  * @brief  Set the TCM biasing level adjust input.
  * @param  Level Biaising level adjust input
  *         This parameter can one of the following values:
  *            @arg SYSCFG_TCM_BIAS_VNOM : Biasing level adjust input recommended for Vnom
  *            @arg SYSCFG_TCM_BIAS_VNOM_10_PERCENT : Biasing level adjust input recommended for Vnom + 10%
  * @retval None
  */
void HAL_SYSCFG_SetTCMBiasingLevel(uint32_t Level)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_TCM_BIASING_LEVEL(Level));

  MODIFY_REG(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_BC2_TCM | SYSCFG_CM55RWMCR_BC1_TCM, Level);
}

/**
  * @brief  Get the TCM biasing level adjust input.
  * @param  pLevel Pointer to return biasing level adjust input.
  *         The return value can be one of the following values:
  *            @arg SYSCFG_CACHE_BIAS_VNOM boot 0 pin: Biasing level adjust input recommended for Vnom
  *            @arg SYSCFG_CACHE_BIAS_VNOM_10_PERCENT: Biasing level adjust input recommended for Vnom + 10%
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetTCMBiasingLevel(uint32_t *pLevel)
{
  /* Check null pointer */
  if (pLevel == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure lock state */
  *pLevel = READ_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_BC2_TCM | SYSCFG_CM55RWMCR_BC1_TCM);

  return HAL_OK;
}

/**
  * @brief  Enable external RW margin inputs for Cache memories
  * @retval None
  */
void HAL_SYSCFG_EnableCacheExternalMargin(void)
{
  SET_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RME_CACHE);
}

/**
  * @brief  Disable external RW margin inputs for Cache memories
  * @retval None
  */
void HAL_SYSCFG_DisableCacheExternalMargin(void)
{
  CLEAR_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RME_CACHE);
}

/**
  * @brief  Set External RW margin inputs for Cache memories.
  * @param  CacheRWMarginInput Input Margin.
  * @retval None
  */
void HAL_SYSCFG_SetCacheRWMarginInput(uint32_t CacheRWMarginInput)
{
  assert_param(IS_TCM_MARGIN_INPUT(CacheRWMarginInput));

  MODIFY_REG(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RM_CACHE, (CacheRWMarginInput << 8U));
}

/**
  * @brief  Get the External RW margin inputs for Cache memories.
  * @param  pCacheRWMarginInput Pointer to return Input Margin.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetCacheRWMarginInput(uint32_t *pCacheRWMarginInput)
{
  /* Check null pointer */
  if (pCacheRWMarginInput == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure lock state */
  *pCacheRWMarginInput = (READ_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_RM_CACHE) >> 8U);

  return HAL_OK;
}

/**
  * @brief  Set the Cache biasing level adjust input.
  * @param  Level the biaising level adjust input
  *         This parameter can one of the following values:
  *            @arg SYSCFG_CACHE_BIAS_VNOM: Biasing level adjust input recommended for Vnom
  *            @arg SYSCFG_CACHE_BIAS_VNOM_10_PERCENT: Biasing level adjust input recommended for Vnom + 10%
  * @retval None
  */
void HAL_SYSCFG_SetCacheBiasingLevel(uint32_t Level)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_CACHE_BIASING_LEVEL(Level));

  MODIFY_REG(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_BC2_CACHE | SYSCFG_CM55RWMCR_BC1_CACHE, Level);
}

/**
  * @brief  Get the Cache biasing level adjust input.
  * @param  pLevel Pointer to return Cache biasing level adjust input.
  *         The return value can be onr of the following values:
  *            @arg SYSCFG_CACHE_BIAS_VNOM boot: Biasing level adjust input recommended for Vnom
  *            @arg SYSCFG_CACHE_BIAS_VNOM_10_PERCENT: Biasing level adjust input recommended for Vnom + 10%
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetCacheBiasingLevel(uint32_t *pLevel)
{
  /* Check null pointer */
  if (pLevel == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the cache biaising */
  *pLevel = READ_BIT(SYSCFG->CM55RWMCR, SYSCFG_CM55RWMCR_BC2_CACHE | SYSCFG_CM55RWMCR_BC1_CACHE);

  return HAL_OK;
}

/**
  * @brief  Set the secure vector table (VTOR) address.
  * @note   Secure vector table address must be set to ensure return from STANDBY state.
  * @param  Address Secure VTOR address
  * @retval None
  */
void HAL_SYSCFG_SetSVTORAddress(uint32_t Address)
{
  /* Check the parameters */
  assert_param(IS_VTOR_ADDRESS(Address));

  WRITE_REG(SYSCFG->INITSVTORCR, Address);
}

/**
  * @brief  Get the secure vector table (VTOR) address.
  * @param  pAddress  Pointer to return the secure vector table address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetSVTORAddress(uint32_t *pAddress)
{
  /* Check null pointer */
  if (pAddress == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the secure VTOR address */
  *pAddress = READ_REG(SYSCFG->INITSVTORCR);

  return HAL_OK;
}

/**
  * @brief  Set the non secure vector table (VTOR) address.
  * @param  Address Non-secure VTOR address
  * @retval None
  */
void HAL_SYSCFG_SetNSVTORAddress(uint32_t Address)
{
  /* Check the parameters */
  assert_param(IS_VTOR_ADDRESS(Address));

  WRITE_REG(SYSCFG->INITNSVTORCR, Address);
}

/**
  * @brief  Get the non secure VTOR address.
  * @param  pAddress  Pointer to return the non-secure vector table address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetNSVTORAddress(uint32_t *pAddress)
{
  /* Check null pointer */
  if (pAddress == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the non-secure VTOR address */
  *pAddress = READ_REG(SYSCFG->INITNSVTORCR);

  return HAL_OK;
}

/**
  * @brief  Enable the Power-on reset.
  * @note   Select power-on reset to apply on core upon SYSRESETREQ (NVIC_SystemReset())
  * @retval None
  */
void HAL_SYSCFG_EnablePowerOnReset(void)
{
  SET_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_CORE_RESET_TYPE);
}

/**
  * @brief  Disable the Power-on reset.
  * @note   Select warm reset to apply on core upon SYSRESETREQ (NVIC_SystemReset())
  * @retval None
  */
void HAL_SYSCFG_DisablePowerOnReset(void)
{
  CLEAR_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_CORE_RESET_TYPE);
}

/**
  * @brief  Enable Lockup requests a warm reset to the RCC.
  * @note   Select action to perform on a lockup state on the core
  * @retval None
  */
void HAL_SYSCFG_EnableLockupWarmResetonRCC(void)
{
  SET_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_LOCKUP_RST_EN);
}

/**
  * @brief  Disable Lockup requests a warm reset to the RCC.
  * @note   Select action to perform on a lockup state on the core
  * @retval None
  */
void HAL_SYSCFG_DisableLockupWarmResetonRCC(void)
{
  CLEAR_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_LOCKUP_RST_EN);
}

/**
  * @brief  Enable Lockup generates a NMI on the core.
  * @note   Select action to perform on a lockup state on the core
  * @retval None
  */
void HAL_SYSCFG_EnableLockupGenerateNMI(void)
{
  SET_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_LOCKUP_NMI_EN);
}

/**
  * @brief  Disable Lockup generates a NMI on the core.
  * @note   Select action to perform on a lockup state on the core
  * @retval None
  */
void HAL_SYSCFG_DisableLockupGenerateNMI(void)
{
  CLEAR_BIT(SYSCFG->CM55RSTCR, SYSCFG_CM55RSTCR_LOCKUP_NMI_EN);
}

/**
  * @brief   Enable the capture mechanism upon write posting buffer error.
  * @note    Upon error detection, the erroneous address is logged on and
  *          the capture mechanism is on hold.
  *          The address error can be read thanks to
  *          HAL_SYSCFG_GetAddressWritePostingBuffer.
  *  @note   This function enables the error capture mechanism again.
  *  @retval HAL status.
  */
HAL_StatusTypeDef HAL_SYSCFG_ReEnableWritePostingErrorCapture(void)
{
  /* Re-enable HW write posting detection mechanism */
  SET_BIT(SYSCFG->CM55PAHBWPR, SYSCFG_CM55PAHBWPR_PAHB_ERROR_ACK);

  CLEAR_BIT(SYSCFG->CM55PAHBWPR, SYSCFG_CM55PAHBWPR_PAHB_ERROR_ACK);

  return HAL_OK;
}

#if defined(VENC)
/**
  * @brief  Reserve the VENCRAM allocation to VENC.
  * @retval None
  */
void HAL_SYSCFG_EnableVENCRAMReserved(void)
{
  CLEAR_BIT(SYSCFG->VENCRAMCR, SYSCFG_VENCRAMCR_VENCRAM_EN);
}

/**
  * @brief  Release the VENCRAM allocation to VENC (any use allowed).
  * @retval None
  */
void HAL_SYSCFG_DisableVENCRAMReserved(void)
{
  SET_BIT(SYSCFG->VENCRAMCR, SYSCFG_VENCRAMCR_VENCRAM_EN);
}

#endif /* VENC */

/**
  * @brief  Enable PKA, SAES, CRYP1/2, and HASH reset, in case of potential tamper
  * @retval None
  */
void HAL_SYSCFG_EnableCRYPPotentialTamper(void)
{
  CLEAR_BIT(SYSCFG->POTTAMPRSTCR, SYSCFG_POTTAMPRSTCR_POTTAMPERSETMASK);
}

/**
  * @brief  Disable PKA, SAES, CRYP1/2, and HASH reset, in case of potential tamper
  * @retval None
  */
void HAL_SYSCFG_DisableCRYPPotentialTamper(void)
{
  SET_BIT(SYSCFG->POTTAMPRSTCR, SYSCFG_POTTAMPRSTCR_POTTAMPERSETMASK);
}

#if defined(SYSCFG_NPUNICQOSCR_NPU1_ARQOSR)
/**
  * @brief  Set write QoS information for master port from NP1 NPUNIC
  * @param  QosValue Write QoS value (0 to 15).
  * @retval None
  */
void HAL_SYSCFG_SetWriteQosNP1(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU1_ARQOSW, QosValue << 4U);
}

/**
  * @brief  Get write QoS information for master port from NP1 NPUNIC
  * @param  pQosValue Pointer to return write QOS value (0 to 15)
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_SYSCFG_GetWriteQosNP1(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = (READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU1_ARQOSW) >> 4U);

  return HAL_OK;
}

/**
  * @brief  Set read QoS information for master port from NP1 NPUNIC
  * @param  QosValue Read QoS value (0 to 15)
  * @retval None
  */
void HAL_SYSCFG_SetReadQosNP1(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU1_ARQOSR, QosValue);
}

/**
  * @brief  Get read QoS information for master port from NP1 NPUNIC
  * @param  pQosValue Pointer to return read QOS value (0 to 15)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosNP1(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU1_ARQOSR);

  return HAL_OK;
}

/**
  * @brief  Set write QoS information for master port from NP2 NPUNIC
  * @param  QosValue Write Qos value (0 to 15)
  * @retval None
  */
void HAL_SYSCFG_SetWriteQosNP2(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU2_ARQOSW, QosValue << 12U);
}

/**
  * @brief  Get write QoS information for master port from NP2 NPUNIC
  * @param  pQosValue Pointer to return write QOS value (0 to 15)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetWriteQosNP2(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = (READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU2_ARQOSW) >> 12U);

  return HAL_OK;
}

/**
  * @brief  Set read QoS information for master port from NP2 NPUNIC
  * @param  QosValue Read Qos value (0 to 15)
  * @retval None
  */
void HAL_SYSCFG_SetReadQosNP2(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU2_ARQOSR, QosValue << 8U);
}

/**
  * @brief  Get read QoS information for master port from NP2 NPUNIC
  * @param  pQosValue Pointer to return read QOS value (0 to 15)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosNP2(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = (READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_NPU2_ARQOSR) >> 8U);

  return HAL_OK;
}

/**
  * @brief  Set write QoS information for master port from CPUSS NPUNIC
  * @param  QosValue Write QoS value (0 to 15)
  * @retval None
  */
void HAL_SYSCFG_SetWriteQosCPUSS(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_CPUSS_ARQOSW, QosValue << 20U);
}

/**
  * @brief  Get write QoS information for master port from CPUSS NPUNIC
  * @param  pQosValue Pointer to return write QOS value (0 to 15)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetwriteQosCPUSS(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = (READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_CPUSS_ARQOSW) >> 20U);

  return HAL_OK;
}

/**
  * @brief  Set read QoS information for master port from CPUSS NPUNIC
  * @param  QosValue Read QoS value (0 to 15)
  * @retval None
  */
void HAL_SYSCFG_SetReadQosCPUSS(uint32_t QosValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_QOS_CPU(QosValue));

  MODIFY_REG(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_CPUSS_ARQOSR, QosValue << 16U);
}

/**
  * @brief  Get read QoS information for master port from CPUSS NPUNIC
  * @param  pQosValue Pointer to return read QOS value (0 to 15)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosCPUSS(uint32_t *pQosValue)
{
  /* Check null pointer */
  if (pQosValue == NULL)
  {
    return HAL_ERROR;
  }

  /* Get the QOS */
  *pQosValue = (READ_BIT(SYSCFG->NPUNICQOSCR, SYSCFG_NPUNICQOSCR_CPUSS_ARQOSR) >> 16U);

  return HAL_OK;
}
#endif /* defined(SYSCFG_NPUNICQOSCR_NPU1_ARQOSR) */

/**
  * @brief  Enable SDMMC early-write response.
  * @param  Sdmmc Selected SDMMC instance(s).
  *               This parameter can be a one of @ref SYSCFG_SDMMCId
  * @retval None
  */
void HAL_SYSCFG_EnableSDMMCEarlyWRRESP(uint32_t Sdmmc)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_SDMMC(Sdmmc));

  SET_BIT(SYSCFG->ICNEWRCR, Sdmmc);
}

/**
  * @brief  Disable SDMMC early-write response.
  * @param  Sdmmc Selected SDMMC instance(s).
  *               This parameter can be a one of @ref SYSCFG_SDMMCId
  * @retval None
  */
void HAL_SYSCFG_DisableSDMMCEarlyWRRSP(uint32_t Sdmmc)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_SDMMC(Sdmmc));

  CLEAR_BIT(SYSCFG->ICNEWRCR, Sdmmc);
}

/**
  * @brief  Enable USB early-write response.
  * @param  Usb Selected USB instance(s).
  *             This parameter can be a one of @ref SYSCFG_USBId
  * @retval None
  */
void HAL_SYSCFG_EnableUSBEarlyEarlyWRRESP(uint32_t Usb)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_USB(Usb));

  SET_BIT(SYSCFG->ICNEWRCR, Usb);
}

/**
  * @brief  Disable USB early-write response.
  * @param  Usb Selected USB instance(s).
  *             This parameter can be a one of @ref SYSCFG_USBId
  * @retval None
  */
void HAL_SYSCFG_DisableUSBEarlyEarlyWRRESP(uint32_t Usb)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_USB(Usb));

  CLEAR_BIT(SYSCFG->ICNEWRCR, Usb);
}

/**
  * @brief  Enable xPU clock gating.
  * @param  Xpu Selected CPU(s).
  *             This parameter can be a one of @ref SYSCFG_XPUId
  * @retval None
  */
void HAL_SYSCFG_EnablexPUClockGating(uint32_t Xpu)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_CPU_CLK_GATING(Xpu));

  SET_BIT(SYSCFG->ICNCGCR, Xpu);
}

/**
  * @brief  Disable xPU clock gating.
  * @param  Xpu Selected CPU(s).
  *             This parameter can be a one of @ref SYSCFG_XPUId
  * @retval None
  */
void HAL_SYSCFG_DisablexPUClockGating(uint32_t Xpu)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_CPU_CLK_GATING(Xpu));

  CLEAR_BIT(SYSCFG->ICNCGCR, Xpu);
}

/**
  * @brief  Enable the VDDIO2 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_EnableVDDIO2CompensationCell(void)
{
  SET_BIT(SYSCFG->VDDIO2CCCR, SYSCFG_VDDIO2CCCR_EN);
}

/**
  * @brief  Disable the VDDIO2 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_DisableVDDIO2CompensationCell(void)
{
  CLEAR_BIT(SYSCFG->VDDIO2CCCR, SYSCFG_VDDIO2CCCR_EN);
}

/**
  * @brief  Enable the VDDIO3 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_EnableVDDIO3CompensationCell(void)
{
  SET_BIT(SYSCFG->VDDIO3CCCR, SYSCFG_VDDIO3CCCR_EN);
}

/**
  * @brief  Disable the VDDIO3 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_DisableVDDIO3CompensationCell(void)
{
  CLEAR_BIT(SYSCFG->VDDIO3CCCR, SYSCFG_VDDIO3CCCR_EN);
}

/**
  * @brief  Enable the VDDIO4 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_EnableVDDIO4CompensationCell(void)
{
  SET_BIT(SYSCFG->VDDIO4CCCR, SYSCFG_VDDIO4CCCR_EN);
}

/**
  * @brief  Disable the VDDIO4 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_DisableVDDIO4CompensationCell(void)
{
  CLEAR_BIT(SYSCFG->VDDIO4CCCR, SYSCFG_VDDIO4CCCR_EN);
}

/**
  * @brief  Enable the VDDIO5 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_EnableVDDIO5CompensationCell(void)
{
  SET_BIT(SYSCFG->VDDIO5CCCR, SYSCFG_VDDIO5CCCR_EN);
}

/**
  * @brief  Disable the VDDIO5 compensation cell.
  * @retval None
  */
void HAL_SYSCFG_DisableVDDIO5CompensationCell(void)
{
  CLEAR_BIT(SYSCFG->VDDIO5CCCR, SYSCFG_VDDIO5CCCR_EN);
}

/**
  * @brief  Configure the code selection for the compensation cell
  * @param  Selection specifies the concerned compensation cell
  *         This parameter can one of the following values:
  *            @arg SYSCFG_IO_VDDIO2_CELL Compensation cell for the VDDIO2
  *            @arg SYSCFG_IO_VDDIO3_CELL Compensation cell for the VDDIO3
  *            @arg SYSCFG_IO_VDDIO4_CELL Compensation cell for the VDDIO4
  *            @arg SYSCFG_IO_VDDIO5_CELL Compensation cell for the VDDIO5
  * @param  Code code selection to be applied for the I/O compensation cell
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_IO_CELL_CODE  Code from the cell (available in the SYSCFG_VDDIOxCCSR)
  *            @arg SYSCFG_IO_REGISTER_CODE Code from the compensation cell code register ((SYSCFG_VDDIOxCCCR)
  * @param  NmosValue In case SYSCFG_IO_REGISTER_CODE is selected, it  provides the Nmos value
  *                   to apply in range 0 to 15 else this parameter is not used
  * @param  PmosValue In case SYSCFG_IO_REGISTER_CODE is selected, it  provides the Pmos value
  *                   to apply in range 0 to 15 else this parameter is not used
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SYSCFG_ConfigVDDIOCompensationCell(uint32_t Selection, uint32_t Code, uint32_t NmosValue,
                                                         uint32_t PmosValue)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_SYSCFG_COMPENSATION_CELL(Selection));
  assert_param(IS_SYSCFG_IO_COMPENSATION_CODE(Code));

  if (Code == SYSCFG_IO_REGISTER_CODE)
  {
    /* Check the parameters */
    assert_param(IS_SYSCFG_IO_COMPENSATION_CELL_NMOS_VALUE(NmosValue));
    assert_param(IS_SYSCFG_IO_COMPENSATION_CELL_PMOS_VALUE(PmosValue));

    /* Reject forbidden Nmos = 0 and Pmos = 15 */
    if ((NmosValue == 0U) || (PmosValue == 0xFU))
    {
      status = HAL_ERROR;
    }
    else
    {
      switch (Selection)
      {
        case SYSCFG_IO_VDDIO2_CELL:
        {
          MODIFY_REG(SYSCFG->VDDIO2CCCR, 0x2FFU, ((NmosValue | (PmosValue << (4U))) | SYSCFG_VDDIO2CCCR_CS));
          break;
        }
        case SYSCFG_IO_VDDIO3_CELL:
        {
          MODIFY_REG(SYSCFG->VDDIO3CCCR, 0x2FFU, ((NmosValue | (PmosValue << (4U))) | SYSCFG_VDDIO3CCCR_CS));
          break;
        }
        case SYSCFG_IO_VDDIO4_CELL:
        {
          MODIFY_REG(SYSCFG->VDDIO4CCCR, 0x2FFU, ((NmosValue) | (PmosValue << (4U)) | SYSCFG_VDDIO4CCCR_CS));
          break;
        }
        case SYSCFG_IO_VDDIO5_CELL:
        {
          MODIFY_REG(SYSCFG->VDDIO5CCCR, 0x2FFU, ((NmosValue | (PmosValue << (4U))) | SYSCFG_VDDIO5CCCR_CS));
          break;
        }
        default:
        {
          /* Update error status */
          status = HAL_ERROR;
          break;
        }
      }
    }
  }
  else
  {
    switch (Selection)
    {

      case SYSCFG_IO_VDDIO2_CELL:
      {
        MODIFY_REG(SYSCFG->VDDIO2CCCR, SYSCFG_VDDIO2CCCR_CS, 0U);
        break;
      }
      case SYSCFG_IO_VDDIO3_CELL:
      {
        MODIFY_REG(SYSCFG->VDDIO3CCCR, SYSCFG_VDDIO3CCCR_CS, 0U);
        break;
      }
      case SYSCFG_IO_VDDIO4_CELL:
      {
        MODIFY_REG(SYSCFG->VDDIO4CCCR, SYSCFG_VDDIO4CCCR_CS, 0U);
        break;
      }
      case SYSCFG_IO_VDDIO5_CELL:
      {
        MODIFY_REG(SYSCFG->VDDIO5CCCR, SYSCFG_VDDIO5CCCR_CS, 0U);
        break;
      }
      default:
      {
        /* Update error status */
        status = HAL_ERROR;
        break;
      }
    }
  }
  return status;
}

/**
  * @brief  Get the code selection for the compensation cell
  * @param  Selection specifies the concerned compensation cell
  *         This parameter can one of the following values:
  *            @arg SYSCFG_IO_VDDIO2_CELL Compensation cell for the VDDIO2
  *            @arg SYSCFG_IO_VDDIO3_CELL Compensation cell for the VDDIO3
  *            @arg SYSCFG_IO_VDDIO4_CELL Compensation cell for the VDDIO4
  *            @arg SYSCFG_IO_VDDIO5_CELL Compensation cell for the VDDIO5
  * @param  pCode pointer to code selection
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_IO_CELL_CODE  Code from the cell (available in the SYSCFG_VDDIOxCCSR)
  *            @arg SYSCFG_IO_REGISTER_CODE Code from the compensation cell code register ((SYSCFG_VDDIOxCCCR)
  * @param  pNmosValue pointer to the Nmos value in range 0 to 15
  * @param  pPmosValue pointer to the Pmos value in range 0 to 15
  * @retval  HAL_OK (all values available) or HAL_ERROR (check parameters)
  */
HAL_StatusTypeDef HAL_SYSCFG_GetVDDIOCompensationCell(uint32_t Selection, const uint32_t *pCode, uint32_t *pNmosValue,
                                                      uint32_t *pPmosValue)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  uint32_t values = 0;

  /* Check the parameters */
  assert_param(IS_SYSCFG_COMPENSATION_CELL(Selection));
  assert_param(IS_SYSCFG_IO_COMPENSATION_CODE(*pCode));

  if ((pCode != NULL) && (pNmosValue != NULL) && (pPmosValue != NULL))
  {
    status = HAL_OK;

    if (*pCode == SYSCFG_IO_REGISTER_CODE)
    {
      switch (Selection)
      {
        case SYSCFG_IO_VDDIO2_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO2CCCR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO3_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO3CCCR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO4_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO4CCCR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO5_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO5CCCR) & 0xFFU;
          break;
        }
        default:
        {
          /* Update error status */
          status = HAL_ERROR;
          break;
        }
      }
    }
    else
    {
      switch (Selection)
      {
        case SYSCFG_IO_VDDIO2_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO2CCSR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO3_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO3CCSR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO4_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO4CCSR) & 0xFFU;
          break;
        }
        case SYSCFG_IO_VDDIO5_CELL:
        {
          values = READ_REG(SYSCFG->VDDIO5CCSR) & 0xFFU;
          break;
        }
        default:
        {
          /* Update error status */
          status = HAL_ERROR;
          break;
        }
      }
    }
    *pNmosValue = (values & 0xFU);
    *pPmosValue = ((values & 0xF0U) >> 4U);
  }
  return status;
}

/**
  * @brief  Get the compensation cell ready status of VDDIO2.
  * @retval Ready status (0 (not ready) or !=0 (ready))
  */
uint32_t HAL_SYSCFG_GetCompVDDIO2CellReadyStatus(void)
{
  return READ_BIT(SYSCFG->VDDIO2CCSR, SYSCFG_VDDIO2CCSR_READY);
}

/**
  * @brief  Get the compensation cell ready status of VDDIO3.
  * @retval Ready status (0 (not ready) or !=0 (ready))
  */
uint32_t HAL_SYSCFG_GetCompVDDIO3CellReadyStatus(void)
{
  return READ_BIT(SYSCFG->VDDIO3CCSR, SYSCFG_VDDIO3CCSR_READY);
}

/**
  * @brief Get the compensation cell ready status of VDDIO4.
  * @retval Ready status (0 (not ready) or !=0 (ready))
  */
uint32_t HAL_SYSCFG_GetCompVDDIO4CellReadyStatus(void)
{
  return READ_BIT(SYSCFG->VDDIO4CCSR, SYSCFG_VDDIO4CCSR_READY);
}

/**
  * @brief Get the compensation cell ready status of VDDIO5.
  * @retval Ready status (0 (not ready) or !=0 (ready))
  */
uint32_t HAL_SYSCFG_GetCompVDDIO5CellReadyStatus(void)
{
  return READ_BIT(SYSCFG->VDDIO5CCSR, SYSCFG_VDDIO5CCSR_READY);
}

/**
  * @brief  Configure the code selection for the compensation cell of VDD
  * @param  Code code selection to be applied for the I/O compensation cell
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_IO_CELL_CODE  Code from the cell (available in the SYSCFG_VDDIOxCCSR)
  *            @arg SYSCFG_IO_REGISTER_CODE Code from the compensation cell code register ((SYSCFG_VDDIOxCCCR)
  * @param  NmosValue In case SYSCFG_IO_REGISTER_CODE is selected, it  provides the Nmos value
  *                   to apply in range 0 to 15 else this parameter is not used
  * @param  PmosValue In case SYSCFG_IO_REGISTER_CODE is selected, it  provides the Pmos value
  *                   to apply in range 0 to 15 else this parameter is not used
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_SYSCFG_ConfigVDDCompensationCell(uint32_t Code, uint32_t NmosValue, uint32_t PmosValue)
{
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_SYSCFG_IO_COMPENSATION_CODE(Code));

  if (Code == SYSCFG_IO_REGISTER_CODE)
  {
    /* Check the parameters */
    assert_param(IS_SYSCFG_IO_COMPENSATION_CELL_NMOS_VALUE(NmosValue));
    assert_param(IS_SYSCFG_IO_COMPENSATION_CELL_PMOS_VALUE(PmosValue));

    MODIFY_REG(SYSCFG->VDDCCCR, 0xFFU, ((NmosValue | (PmosValue << (4U))) | SYSCFG_VDDCCCR_CS));

    status = HAL_OK;
  }
  else
  {
    MODIFY_REG(SYSCFG->VDDCCCR, 0x200U, 0U);
    status = HAL_OK;
  }
  return status;
}

/**
  * @brief  Enable the VDD compensation cell.
  * @retval None
  */
void HAL_SYSCFG_EnableVDDCompensationCell(void)
{
  SET_BIT(SYSCFG->VDDCCCR, SYSCFG_VDDCCCR_EN);
}

/**
  * @brief  Disable the VDD compensation cell.
  * @retval None
  */
void HAL_SYSCFG_DisableVDDCompensationCell(void)
{
  CLEAR_BIT(SYSCFG->VDDCCCR, SYSCFG_VDDCCCR_EN);
}

/**
  * @brief  Get the code selection for the compensation cell of VDD
  * @param  Code code selection
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_IO_CELL_CODE  Code from the cell (available in the SYSCFG_VDDIOxCCSR)
  *            @arg SYSCFG_IO_REGISTER_CODE Code from the compensation cell code register ((SYSCFG_VDDIOxCCCR)
  * @param  pNmosValue pointer to the Nmos value in range 0 to 15
  * @param  pPmosValue pointer to the Pmos value in range 0 to 15
  * @retval HAL_OK (all values available) or HAL_ERROR (check parameters)
  */
HAL_StatusTypeDef HAL_SYSCFG_GetVDDCompensationCell(uint32_t Code, uint32_t *pNmosValue,
                                                    uint32_t *pPmosValue)
{
  uint32_t values;
  HAL_StatusTypeDef status = HAL_ERROR;

  /* Check the parameters */
  assert_param(IS_SYSCFG_IO_COMPENSATION_CODE(Code));

  if ((pNmosValue != NULL) && (pPmosValue != NULL))
  {
    if (Code == SYSCFG_IO_REGISTER_CODE)
    {
      values = READ_REG(SYSCFG->VDDCCCR) & 0xFFU;
    }
    else
    {
      values = READ_REG(SYSCFG->VDDCCSR) & 0xFFU;
    }

    *pNmosValue = (values & 0xFU);
    *pPmosValue = ((values >> 4U) & 0xFU);

    status = HAL_OK;
  }
  return status;
}

/**
  * @brief  Get the compensation cell ready status of VDD.
  * @retval Ready status (0 (not ready) or !=0 (ready))
  */
uint32_t HAL_SYSCFG_GetCompensationVDDCellReadyStatus(void)
{
  return READ_BIT(SYSCFG->VDDCCSR, SYSCFG_VDDCCSR_READY);
}

/**
  * @brief  Configure the Timer Break input for error flag(s).
  * @note   When a configuration is set, only a system reset can reset it.
  * @param  Input Input configuration
  *              This parameter can be one or a combination of the following values:
  *                @arg SYSCFG_CBR_BREAK_LOCK_CORE       Cortex-CM55 lockup
  *                @arg SYSCFG_CBR_BREAK_LOCK_PVD        PVD lock
  *                @arg SYSCFG_CBR_BREAK_LOCK_BKPRAM     Backup SRAM ECC error
  *                @arg SYSCFG_CBR_BREAK_LOCK_CM55_CACHE CM55 cache double ECC error
  *                @arg SYSCFG_CBR_BREAK_LOCK_CM55_TCM   DTCM double ECC error
  * @retval None
  */
void HAL_SYSCFG_ConfigTimerBreakInput(uint32_t Input)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CBR_BREAK_INPUT(Input));

  MODIFY_REG(SYSCFG->CBR, Input, Input);
}

/**
  * @brief  Get the Timer Break input configuration.
  * @note   When a configuration is set, only a system reset can reset it.
  * @retval Timer break input configuration
  *              This return value can be one or a combination of the following values:
  *                @arg SYSCFG_CBR_BREAK_LOCK_CORE       Cortex-CM55 lockup
  *                @arg SYSCFG_CBR_BREAK_LOCK_PVD        PVD lock
  *                @arg SYSCFG_CBR_BREAK_LOCK_BKPRAM     Backup SRAM ECC error
  *                @arg SYSCFG_CBR_BREAK_LOCK_CM55_CACHE CM55 cache double ECC error
  *                @arg SYSCFG_CBR_BREAK_LOCK_CM55_TCM   DTCM double ECC error
  */
uint32_t HAL_SYSCFG_GetTimerBreakInputConfig(void)
{
  return (SYSCFG->CBR & SYSCFG_CBR_BREAK_LOCK_ALL);
}

/**
  * @brief  Set the perceived CID of the peripheral.
  * @note   Set the perceived CID so that all accesses which arrive at the peripheral
  *         (here HPDMA) appear to carry that CID.
  *         See Multi-tenancy section of the reference manual for further information.
  * @param  Cid CID in range 0 to 7
  * @retval None
  */
void HAL_SYSCFG_SetPerceivedCID(uint32_t Cid)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_DMA_CID_SEC(Cid));

  MODIFY_REG(SYSCFG->SEC_AIDCR, SYSCFG_SEC_AIDCR_DMACID_SEC, Cid);
}

/**
  * @brief  Get the perceived CID of the peripheral.
  * @retval CID in range 0 to 7
  */
uint32_t HAL_SYSCFG_GetPerceivedCID(void)
{
  return (SYSCFG->SEC_AIDCR & SYSCFG_SEC_AIDCR_DMACID_SEC);
}

/**
  * @brief  Set the perceived CID of the peripheral as privileged.
  * @note   Set the perceived CID so that all accesses which arrive at the peripheral
  *         (here HPDMA) appear to carry that CID.
  *         This function shall be called in case of DMA channel interrupt (privileged) code
  *         to read the registers and clear the interrupt of this DMA channel.
  *         See Multi-tenancy section of the reference manual for forther information.
  * @param  Cid in range 0 to 7
  * @retval None
  */
void HAL_SYSCFG_SetPerceivedPrivCID(uint32_t Cid)
{
  MODIFY_REG(SYSCFG->SECPRIV_AIDCR, SYSCFG_SECPRIV_AIDCR_DMACID_NONSEC, Cid);
}

/**
  * @brief   Get the perceived CID of the peripheral as privileged.
  * @retval  CID in range 0 to 7
  */
uint32_t HAL_SYSCFG_GetPerceivedPrivCID(void)
{
  return (SYSCFG->SECPRIV_AIDCR & SYSCFG_SECPRIV_AIDCR_DMACID_NONSEC);
}

/**
  * @brief  Enable Retiming on RX path.
  * @retval None
  */
void HAL_SYSCFG_EnableReTimingRXpath(void)
{
  SET_BIT(SYSCFG->FMC_RETIMECR, SYSCFG_FMC_RETIMECR_CFG_RETIME_RX);
}

/**
  * @brief  Disable Retiming on RX path.
  * @retval None
  */
void HAL_SYSCFG_DisableReTimingRXpath(void)
{
  CLEAR_BIT(SYSCFG->FMC_RETIMECR, SYSCFG_FMC_RETIMECR_CFG_RETIME_RX);
}

/**
  * @brief  Enable Retiming on TX path.
  * @retval None
  */
void HAL_SYSCFG_EnableReTimingTXpath(void)
{
  SET_BIT(SYSCFG->FMC_RETIMECR, SYSCFG_FMC_RETIMECR_CFG_RETIME_TX);
}

/**
  * @brief  Disable Retiming on TX path.
  * @retval None
  */
void HAL_SYSCFG_DisableReTimingTXpath(void)
{
  CLEAR_BIT(SYSCFG->FMC_RETIMECR, SYSCFG_FMC_RETIMECR_CFG_RETIME_TX);
}

/**
  * @brief  Set delay on feedback clock.
  * @param  Delay Delay on feedback clock
  *                @arg SYSCFG_DELAY_FEEDBACK_NONE             None
  *                @arg SYSCFG_DELAY_FEEDBACK_HALF_CYCLE       Half a cycle delay
  * @retval None
  */
void HAL_SYSCFG_SetDelayOnFeedbackClock(uint32_t Delay)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_DMA_DELAY_FEEDBACK_CLOCK(Delay));

  MODIFY_REG(SYSCFG->FMC_RETIMECR, SYSCFG_FMC_RETIMECR_SDFBCLK_180, Delay);
}

/**
  * @brief  Get delay on feedback clock.
  * @retval Delay on feedback clock
  *                @arg SYSCFG_DELAY_FEEDBACK_NONE             None
  *                @arg SYSCFG_DELAY_FEEDBACK_HALF_CYCLE       Half a cycle delay
  */
uint32_t HAL_SYSCFG_GetDelayOnFeedbackClock(void)
{
  return (SYSCFG->FMC_RETIMECR & SYSCFG_FMC_RETIMECR_SDFBCLK_180);
}

#if defined(SYSCFG_NPUNICQOSCR_NPU1_ARQOSR)
/**
  * @brief  Enable the interleaving on NPU RAM.
  * @retval None
  */
void HAL_SYSCFG_EnableInterleavingCpuRam(void)
{
  SET_BIT(SYSCFG->NPU_ICNCR, SYSCFG_NPU_ICNCR_INTERLEAVING_ACTIVE);
}

/**
  * @brief  Disable the interleaving on NPU RAM.
  * @retval None
  */
void HAL_SYSCFG_DisableInterleavingCpuRam(void)
{
  CLEAR_BIT(SYSCFG->NPU_ICNCR, SYSCFG_NPU_ICNCR_INTERLEAVING_ACTIVE);
}
#endif /* SYSCFG_NPUNICQOSCR_NPU1_ARQOSR */

/**
  * @brief  Get BOOT pin connection status
  * @param  BootId specifies the boot pins.
  * @retval BOOT pin connection configuration
  *              This return value can be one of the following values:
  *                @arg SYSCFG_BOOT_CONNECTION_VSS       pin connected to VSS
  *                @arg SYSCFG_BOOT_CONNECTION_VDD       pin connected to VDD
  */
uint32_t HAL_SYSCFG_GetBootPinConnection(uint32_t BootId)
{
  uint32_t Connection;

  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_ID(BootId));

  Connection = READ_BIT(SYSCFG->BOOTSR, BootId);

  if (BootId == SYSCFG_BOOTCR_BOOT0_PD)
  {
    return Connection;
  }
  else
  {
    return (Connection >> 1U);
  }
}

/**
  * @brief  Get address of first error in P-AHB write-posting buffer
  * @retval Address of the first error in P-AHB write-posting buffer
  */
uint32_t HAL_SYSCFG_GetAddressWritePostingBuffer(void)
{
  return READ_BIT(SYSCFG->AHBWP_ERROR_SR, SYSCFG_AHBWP_ERROR_SR_PAHB_ERROR_ADDR);
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
