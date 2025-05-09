/**
  ******************************************************************************
  * @file    stm32n6xx_ll_pwr.c
  * @author  MCD Application Team
  * @brief   PWR LL module driver.
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
  */

#if defined (USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_pwr.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (PWR)

/** @defgroup PWR_LL PWR
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup PWR_LL_Exported_Functions
  * @{
  */

/** @addtogroup PWR_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the PWR registers to their default reset values.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS : PWR registers are de-initialized.
  *          - ERROR   : not applicable.
  */
ErrorStatus LL_PWR_DeInit(void)
{
  WRITE_REG(PWR->CR1, 0x00000024U);
  WRITE_REG(PWR->CR2, 0x00000000U);
  WRITE_REG(PWR->CR3, 0x00000000U);
  WRITE_REG(PWR->CR4, 0x00000000U);
  WRITE_REG(PWR->VOSCR, 0x00020002U);
  WRITE_REG(PWR->BDCR1, 0x00000000U);
  WRITE_REG(PWR->BDCR2, 0x00000000U);
  WRITE_REG(PWR->DBPCR, 0x00000000U);
  WRITE_REG(PWR->CPUCR, 0x00010000U);
  WRITE_REG(PWR->SVMCR1, 0x00000000U);
  WRITE_REG(PWR->SVMCR2, 0x00000000U);
  WRITE_REG(PWR->SVMCR3, 0x00000000U);
  WRITE_REG(PWR->WKUPCR, 0x00000000U);
  WRITE_REG(PWR->WKUPSR, 0x00000000U);
  WRITE_REG(PWR->WKUPEPR, 0x00000000U);

  /* Clear PWR low power flags */
  LL_PWR_ClearFlag_STOP_SB();

  /* Clear PWR wake up flags */
  LL_PWR_ClearFlag_WU();

  /* Reset privilege attribute */
  LL_PWR_ConfigPrivilege(0);

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  /* Reset secure attribute */
  LL_PWR_ConfigSecure(0);
#endif /* defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U) */

  return SUCCESS;
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

#endif /* defined(PWR) */
/**
  * @}
  */

#endif /* defined (USE_FULL_LL_DRIVER) */

