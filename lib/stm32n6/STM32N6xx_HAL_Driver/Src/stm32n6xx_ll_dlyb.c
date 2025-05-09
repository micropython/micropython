/**
  ******************************************************************************
  * @file    stm32n6xx_ll_dlyb.c
  * @author  MCD Application Team
  * @brief   DelayBlock Low Layer HAL module driver.
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the DelayBlock peripheral:
  *           + input clock frequency
  *           + up to 12 oversampling phases
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
                       ##### DelayBlock peripheral features #####
  ==============================================================================
    [..] The DelayBlock is used to generate an Output clock which is de-phased from the Input
          clock. The phase of the Output clock is programmed by FW. The Output clock is then used
          to clock the receive data in i.e. a SDMMC, OSPI or QSPI interface.
         The delay is Voltage and Temperature dependent, which may require FW to do re-tuning
          and recenter the Output clock phase to the receive data.

    [..] The DelayBlock features include the following:
         (+) Input clock frequency.
         (+) Up to 12 oversampling phases.

                           ##### How to use this driver #####
  ==============================================================================
    [..]
      This driver is a considered as a driver of service for external devices drivers
      that interfaces with the DELAY peripheral.
      The LL_DLYB_SetDelay() function, configure the Delay value configured on SEL and UNIT.
      The LL_DLYB_GetDelay() function, return the Delay value configured on SEL and UNIT.
      The LL_DLYB_GetClockPeriod()function, get the clock period.


  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

/** @defgroup DLYB_LL DLYB
  * @brief DLYB LL module driver.
  * @{
  */

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_OSPI_MODULE_ENABLED) || defined(HAL_XSPI_MODULE_ENABLED)
#if defined (DLYB_SDMMC1) || defined (DLYB_SDMMC2) || defined (DLYB_OCTOSPI1) || defined (DLYB_OCTOSPI2)

/**
  @cond 0
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DLYB_TIMEOUT 0xFFU
#define DLYB_LNG_10_0_MASK   0x07FF0000U
#define DLYB_LNG_11_10_MASK  0x0C000000U
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  @endcond
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup DLYB_LL_Exported_Functions
  *  @brief    Configuration and control functions
  *
@verbatim
 ===============================================================================
              ##### Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to
      (+) Control the DLYB.
@endverbatim
  * @{
  */

/** @addtogroup DLYB_Control_Functions DLYB Control functions
  * @{
  */

/**
  * @brief  Set the Delay value configured on SEL and UNIT.
  * @param  DLYBx: Pointer to DLYB instance.
  * @param  pdlyb_cfg: Pointer to DLYB configuration structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: the Delay value is set.
  *          - ERROR: the Delay value is not set.
  */
void LL_DLYB_SetDelay(DLYB_TypeDef *DLYBx, const LL_DLYB_CfgTypeDef  *pdlyb_cfg)
{
  /* Check the DelayBlock instance */
  assert_param(IS_DLYB_ALL_INSTANCE(DLYBx));

  /* Enable the length sampling */
  SET_BIT(DLYBx->CR, DLYB_CR_SEN);

  /* Update the UNIT and SEL field */
  DLYBx->CFGR = (pdlyb_cfg->PhaseSel) | ((pdlyb_cfg->Units) << DLYB_CFGR_UNIT_Pos);

  /* Disable the length sampling */
  CLEAR_BIT(DLYBx->CR, DLYB_CR_SEN);
}

/**
  * @brief  Get the Delay value configured on SEL and UNIT.
  * @param  DLYBx: Pointer to DLYB instance.
  * @param  pdlyb_cfg: Pointer to DLYB configuration structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: the Delay value is received.
  *          - ERROR: the Delay value is not received.
  */
void LL_DLYB_GetDelay(const DLYB_TypeDef *DLYBx, LL_DLYB_CfgTypeDef *pdlyb_cfg)
{
  /* Check the DelayBlock instance */
  assert_param(IS_DLYB_ALL_INSTANCE(DLYBx));

  /* Fill the DelayBlock configuration structure with SEL and UNIT value */
  pdlyb_cfg->Units = ((DLYBx->CFGR & DLYB_CFGR_UNIT) >> DLYB_CFGR_UNIT_Pos);
  pdlyb_cfg->PhaseSel = (DLYBx->CFGR & DLYB_CFGR_SEL);
}

/**
  * @brief  Get the clock period.
  * @param  DLYBx: Pointer to DLYB instance.
  * @param  pdlyb_cfg: Pointer to DLYB configuration structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: there is a valid period detected and stored in pdlyb_cfg.
  *          - ERROR: there is no valid period detected.
  */
uint32_t LL_DLYB_GetClockPeriod(DLYB_TypeDef *DLYBx, LL_DLYB_CfgTypeDef *pdlyb_cfg)
{
  uint32_t i = 0U;
  uint32_t nb ;
  uint32_t lng ;
  uint32_t tickstart;

  /* Check the DelayBlock instance */
  assert_param(IS_DLYB_ALL_INSTANCE(DLYBx));

  /* Enable the length sampling */
  SET_BIT(DLYBx->CR, DLYB_CR_SEN);

  /* Delay line length detection */
  while (i < DLYB_MAX_UNIT)
  {
    /* Set the Delay of the UNIT(s)*/
    DLYBx->CFGR = DLYB_MAX_SELECT | (i << DLYB_CFGR_UNIT_Pos);

    /* Waiting for a LNG valid value */
    tickstart =  HAL_GetTick();
    while ((DLYBx->CFGR & DLYB_CFGR_LNGF) == 0U)
    {
      if ((HAL_GetTick() - tickstart) >=  DLYB_TIMEOUT)
      {
        /* New check to avoid false timeout detection in case of preemption */
        if ((DLYBx->CFGR & DLYB_CFGR_LNGF) == 0U)
        {
          return (uint32_t) HAL_TIMEOUT;
        }
      }
    }

    if ((DLYBx->CFGR & DLYB_LNG_10_0_MASK) != 0U)
    {
      if ((DLYBx->CFGR & (DLYB_CFGR_LNG_11 | DLYB_CFGR_LNG_10)) != DLYB_LNG_11_10_MASK)
      {
        /* Delay line length is configured to one input clock period*/
        break;
      }
    }
    i++;
  }

  if (DLYB_MAX_UNIT != i)
  {
    /* Determine how many unit delays (nb) span one input clock period */
    lng = (DLYBx->CFGR & DLYB_CFGR_LNG) >> 16U;
    nb = 10U;
    while ((nb > 0U) && ((lng >> nb) == 0U))
    {
      nb--;
    }
    if (nb != 0U)
    {
      pdlyb_cfg->PhaseSel = nb ;
      pdlyb_cfg->Units = i ;

      /* Disable the length sampling */
      CLEAR_BIT(DLYBx->CR, DLYB_CR_SEN);

      return (uint32_t)SUCCESS;
    }
  }

  /* Disable the length sampling */
  CLEAR_BIT(DLYBx->CR, DLYB_CR_SEN);

  return (uint32_t)ERROR;

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
#endif /* DLYB_SDMMC1 || DLYB_SDMMC2 || DLYB_OCTOSPI1 || DLYB_OCTOSPI2 */
#endif /* HAL_SD_MODULE_ENABLED || HAL_OSPI_MODULE_ENABLED || HAL_XSPI_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
