/**
  ******************************************************************************
  * @file    stm32n6xx_ll_ucpd.c
  * @author  MCD Application Team
  * @brief   UCPD LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_ucpd.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */
#if defined (UCPD1)
/** @addtogroup UCPD_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup UCPD_LL_Private_Constants UCPD Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup UCPD_LL_Private_Macros UCPD Private Macros
  * @{
  */


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup UCPD_LL_Exported_Functions
  * @{
  */

/** @addtogroup UCPD_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the UCPD registers to their default reset values.
  * @param  UCPDx ucpd Instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ucpd registers are de-initialized
  *          - ERROR: ucpd registers are not de-initialized
  */
ErrorStatus LL_UCPD_DeInit(UCPD_TypeDef *UCPDx)
{
  ErrorStatus status = ERROR;

  /* Check the parameters */
  assert_param(IS_UCPD_ALL_INSTANCE(UCPDx));

  LL_UCPD_Disable(UCPDx);

  if (UCPD1 == UCPDx)
  {
    /* Force reset of ucpd clock */
    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_UCPD1);

    /* Release reset of ucpd clock */
    LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_UCPD1);

    /* Disable ucpd clock */
    LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_UCPD1);

    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  Initialize the ucpd registers according to the specified parameters in UCPD_InitStruct.
  * @note   As some bits in ucpd configuration registers can only be written when the ucpd is disabled
  *         (ucpd_CR1_SPE bit =0), UCPD peripheral should be in disabled state prior calling this function.
  *         Otherwise, ERROR result will be returned.
  * @param  UCPDx UCPD Instance
  * @param  UCPD_InitStruct pointer to a @ref LL_UCPD_InitTypeDef structure that contains
  *         the configuration information for the UCPD peripheral.
  * @retval An ErrorStatus enumeration value. (Return always SUCCESS)
  */
ErrorStatus LL_UCPD_Init(UCPD_TypeDef *UCPDx, const LL_UCPD_InitTypeDef *UCPD_InitStruct)
{
  /* Check the ucpd Instance UCPDx*/
  assert_param(IS_UCPD_ALL_INSTANCE(UCPDx));

  if (UCPD1 == UCPDx)
  {
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_UCPD1);
  }


  LL_UCPD_Disable(UCPDx);

  /*---------------------------- UCPDx CFG1 Configuration ------------------------*/
  MODIFY_REG(UCPDx->CFG1,
             UCPD_CFG1_PSC_UCPDCLK | UCPD_CFG1_TRANSWIN | UCPD_CFG1_IFRGAP | UCPD_CFG1_HBITCLKDIV,
             UCPD_InitStruct->psc_ucpdclk | (UCPD_InitStruct->transwin  << UCPD_CFG1_TRANSWIN_Pos) |
             (UCPD_InitStruct->IfrGap << UCPD_CFG1_IFRGAP_Pos) | UCPD_InitStruct->HbitClockDiv);

  return SUCCESS;
}

/**
  * @brief  Set each @ref LL_UCPD_InitTypeDef field to default value.
  * @param  UCPD_InitStruct pointer to a @ref LL_UCPD_InitTypeDef structure
  *         whose fields will be set to default values.
  * @retval None
  */
void LL_UCPD_StructInit(LL_UCPD_InitTypeDef *UCPD_InitStruct)
{
  /* Set UCPD_InitStruct fields to default values */
  UCPD_InitStruct->psc_ucpdclk  = LL_UCPD_PSC_DIV2;
  UCPD_InitStruct->transwin     = 0x7;   /* Divide by 8                     */
  UCPD_InitStruct->IfrGap       = 0x10;  /* Divide by 17                    */
  UCPD_InitStruct->HbitClockDiv = 0x0D;  /* Divide by 14 to produce HBITCLK */
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
#endif /* defined (UCPD1) */
/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

