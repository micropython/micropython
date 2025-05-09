/**
  ******************************************************************************
  * @file    stm32n6xx_ll_venc.c
  * @author  GPM Application Team
  * @brief   VENC LL module driver
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

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_venc.h"
#include "stm32n6xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */


/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */
#if defined(VENC)

/** @addtogroup VENC_LL
  * @{
  */


/* Global variables ----------------------------------------------------------*/
/** @addtogroup VENC_Global_Variables
  * @{
  */



/**
  * @}
  */

/* Private typedef -----------------------------------------------------------*/
/** @defgroup VENC_Private_Types VENC Private Types
  * @{
  */

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup VENC_Private_Constants VENC Private Constants
  * @{

  */


/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup VENC_Private_Macros VENC Private Macros
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup VENC_Private_Variables VENC Private Variables
  * @{

 */



/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/
/** @addtogroup VENC_Exported_Functions
  * @{
  */

/**
  * @brief  Initialize the VENC hardware
  * @note   to check that the initialization is correct, ASIC ID should be checked
  * @retval None
  */
void LL_VENC_Init(void)
{
  /* make sure VENCRAM is allocated to VENC and not the system*/
  assert_param(!LL_VENC_IS_VENCRAM_SYSTEM_ACCESSIBLE());

  /* enable APB5 bus clock*/
  /* cf. errata : SHOULD ADD REFERENCE TO THE ERRATA */
  /* using CMSIS access because ll_bus does not contain APB5 enable macros*/
  WRITE_REG(RCC->BUSENSR, RCC_BUSENSR_APB5ENS);

  /* enable VENCRAM */
  LL_MEM_EnableClock(LL_MEM_VENCRAM);

  /* enable VENC clock */
  LL_APB5_GRP1_EnableClock(LL_APB5_GRP1_PERIPH_VENC);
}

/**
  * @brief  De-Initialize the VENC hardware
  * @retval None
  */
void LL_VENC_DeInit(void)
{
  /* don't turn off APB5 in DeInit because other peripherals may be using it */

  /* disable VENCRAM */
  LL_MEM_DisableClock(LL_MEM_VENCRAM);

  /* disable VENC clock */
  LL_APB5_GRP1_DisableClock(LL_APB5_GRP1_PERIPH_VENC);
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* USE_VENC_MODULE */

/**
  * @}
  */
