/**
  ******************************************************************************
  * @file    stm32n6xx_ll_utils.c
  * @author  MCD Application Team
  * @brief   UTILS LL module driver.
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
#include "stm32n6xx_ll_utils.h"
#include "stm32n6xx_ll_pwr.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

/** @addtogroup UTILS_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup UTILS_LL_Private_Constants
  * @{
  */
#define UTILS_MAX_FREQUENCY_SCALE0  800000000U      /*!< Maximum frequency for system clock at power scale 0, in Hz */
#define UTILS_MAX_FREQUENCY_SCALE1  600000000U      /*!< Maximum frequency for system clock at power scale 1, in Hz */

#define UTILS_PLLVCO_INPUT_MAX       50000000U      /*!< Frequency max for the PLLVCO input, in Hz  */
#define UTILS_PLLVCO_OUTPUT_MAX    3200000000U      /*!< Frequency max for the PLLVCO output, in Hz */
/* Defines used for HSE range */
#define UTILS_HSE_FREQUENCY_MIN         16000000U   /*!< Frequency min for HSE frequency, in Hz   */
#define UTILS_HSE_FREQUENCY_MAX         48000000U   /*!< Frequency max for HSE frequency, in Hz   */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup UTILS_LL_Private_Macros
  * @{
  */
#define IS_LL_UTILS_AHB_DIV(__VALUE__)    (((__VALUE__) == LL_RCC_AHB_DIV_1) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_2) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_4) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_8) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_16) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_64) \
                                           || ((__VALUE__) == LL_RCC_AHB_DIV_128))

#define IS_LL_UTILS_APB1_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB1_DIV_1) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_2) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_4) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_8) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_16) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_32) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_64) \
                                         || ((__VALUE__) == LL_RCC_APB1_DIV_128))

#define IS_LL_UTILS_APB2_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB2_DIV_1) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_2) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_4) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_8) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_16) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_32) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_64) \
                                         || ((__VALUE__) == LL_RCC_APB2_DIV_128))

#define IS_LL_UTILS_APB4_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB4_DIV_1) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_2) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_4) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_8) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_16) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_32) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_64) \
                                         || ((__VALUE__) == LL_RCC_APB4_DIV_128))

#define IS_LL_UTILS_APB5_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB5_DIV_1) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_2) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_4) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_8) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_16) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_32) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_64) \
                                         || ((__VALUE__) == LL_RCC_APB5_DIV_128))

/* Values expected to use PLL in Integer mode */
#define IS_LL_UTILS_PLLM_VALUE(__VALUE__) ((1U <= (__VALUE__)) && ((__VALUE__) <= 63U))

#define IS_LL_UTILS_PLLN_VALUE(__VALUE__) ((16U <= (__VALUE__)) && ((__VALUE__) <= 640U))

#define IS_LL_UTILS_PLLP_VALUE(__VALUE__) ((1U <= (__VALUE__)) && ((__VALUE__) <= 7U))

#define IS_LL_UTILS_FRACN_VALUE(__VALUE__) ((__VALUE__) == 0U)

#define IS_LL_UTILS_PLLVCO_INPUT(__VALUE__) ((__VALUE__) <= UTILS_PLLVCO_INPUT_MAX)

#define IS_LL_UTILS_PLLVCO_OUTPUT(__VALUE__) ((__VALUE__) <= UTILS_PLLVCO_OUTPUT_MAX)

#define IS_LL_UTILS_HSE_BYPASS(__STATE__) \
  (((__STATE__) == LL_UTILS_HSEBYPASS_ON) || ((__STATE__) == LL_UTILS_HSEBYPASS_OFF))

#define IS_LL_UTILS_HSE_FREQUENCY(__FREQUENCY__) \
  (((__FREQUENCY__) >= UTILS_HSE_FREQUENCY_MIN) && ((__FREQUENCY__) <= UTILS_HSE_FREQUENCY_MAX))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup UTILS_LL_Private_Functions UTILS Private functions
  * @{
  */
static void        UTILS_ConfigurePLL1InIntegerMode(const LL_UTILS_PLLInitTypeDef *pUTILS_ClkInitStruct);
static void        UTILS_ConfigureIC(const LL_UTILS_ICInitTypeDef *pUTILS_ICInitStruct);
static ErrorStatus UTILS_EnablePLL1AndSwitchSystem(uint32_t CPU_Frequency,
                                                   const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup UTILS_LL_Exported_Functions
  * @{
  */

/** @addtogroup UTILS_LL_EF_DELAY
  * @{
  */
/**
  * @brief  This function configures the Cortex-M SysTick source to have 1ms time base.
  * @note   When a RTOS is used, it is recommended to avoid changing the Systick
  *         configuration by calling this function, for a delay use rather osDelay RTOS service.
  * @param  CPU_Frequency Core frequency in Hz
  * @note   CPU_Frequency can be calculated thanks to RCC helper macro or function
  *         @ref LL_RCC_GetSystemClocksFreq
  * @retval None
  */
void LL_Init1msTick(uint32_t CPU_Frequency)
{
  /* Use frequency provided in argument */
  LL_InitTick(CPU_Frequency, 1000U);
}


/**
  * @brief  This function provides accurate delay (in milliseconds) based
  *         on SysTick counter flag
  * @note   When a RTOS is used, it is recommended to avoid using blocking delay
  *         and use rather osDelay service.
  * @note   To respect 1ms timebase, user should call @ref LL_Init1msTick function which
  *         will configure Systick to 1ms
  * @param  Delay specifies the delay time length, in milliseconds.
  * @retval None
  */
void LL_mDelay(uint32_t Delay)
{
  uint32_t count = Delay;
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if (count < LL_MAX_DELAY)
  {
    count++;
  }

  while (count != 0U)
  {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      count--;
    }
  }
}

/**
  * @}
  */

/** @addtogroup UTILS_EF_SYSTEM
  *  @brief    System Configuration functions
  *
  @verbatim
 ===============================================================================
           ##### System Configuration functions #####
 ===============================================================================
    [..]
         System, AXI, AHB and APB buses clocks configuration

         (+) The maximum frequency of the CPU is 800 MHz and AXI is 400 MHz.
         (+) The maximum frequency of the HCLK, PCLK1, PCLK2, PCLK4 and PCLK5 is 200 MHz.
  @endverbatim
  * @{
  */

/**
  * @brief  This function sets directly SystemCoreClock CMSIS variable.
  * @note   Variable can be calculated also through SystemCoreClockUpdate function.
  * @param  CPU_Frequency Core frequency in Hz
  * @note   CPU_Frequency can be calculated thanks to RCC helper macro or function
  *         @ref LL_RCC_GetSystemClocksFreq
  * @retval None
  */
void LL_SetSystemCoreClock(uint32_t CPU_Frequency)
{
  /* CPU clock frequency */
  SystemCoreClock = CPU_Frequency;
}

/**
  * @brief  This function configures the CPU system clock with HSI as clock source of the PLL1
  *         used in integer mode.
  * @note   The application needs to ensure that PLL1 is disabled.
  * @note   Function is based on the following formula:
  *         - PLL output frequency = (((HSI frequency / PLLM) * PLLN) / PLLP1 / PLLP2)
  *         - PLLM: ensure that the VCO input frequency ranges from 1 to 50 MHz (PLLVCO_input = HSI frequency / PLLM)
  *         - PLLN: ensure that the VCO output frequency is maximum 3200 MHz (PLLVCO_output = PLLVCO_input * PLLN)
  *         - PLLP1, PLLP2: ensure that max frequency at 800 MHz is reached (PLLVCO_output / PLLP1 / PLLP2)
  * @param  pUTILS_PLLInitStruct pointer to a @ref LL_UTILS_PLLInitTypeDef structure that contains
  *                             the configuration information for the PLL.
  * @param  pUTILS_ICInitStruct pointer to a @ref LL_UTILS_ICInitTypeDef structure that contains
  *                             the configuration information for the IC.
  * @param  pUTILS_ClkInitStruct pointer to a @ref LL_UTILS_ClkInitTypeDef structure that contains
  *                             the configuration information for the BUS prescalers.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Max frequency configuration done
  *          - ERROR: Max frequency configuration not done
  *
  */
ErrorStatus LL_PLL_ConfigSystemClock_HSI(const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct)
{
  ErrorStatus status;
  uint32_t pllfreq;
  uint32_t hsi_clk;

  /* Check the PLL parameters */
  assert_param(IS_LL_UTILS_PLLM_VALUE(pUTILS_PLLInitStruct->PLLM));
  assert_param(IS_LL_UTILS_PLLN_VALUE(pUTILS_PLLInitStruct->PLLN));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP1));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP2));
  assert_param(IS_LL_UTILS_FRACN_VALUE(pUTILS_PLLInitStruct->FRACN));

  hsi_clk = (HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_HSICFGR_HSIDIV_Pos));

  /* Check VCO Input frequency */
  assert_param(IS_LL_UTILS_PLLVCO_INPUT(hsi_clk / pUTILS_PLLInitStruct->PLLM));

  /* Check that PLL1 is not enabled and thus ready for configuration */
  if (LL_RCC_PLL1_IsReady() != 1U)
  {
    /* Integer mode only */
    if (pUTILS_PLLInitStruct->FRACN == 0U)
    {
      /* Calculate the new PLL output frequency */
      pllfreq = LL_RCC_CalcPLLClockFreq(hsi_clk, pUTILS_PLLInitStruct->PLLM,
                                        pUTILS_PLLInitStruct->PLLN, pUTILS_PLLInitStruct->FRACN,
                                        pUTILS_PLLInitStruct->PLLP1, pUTILS_PLLInitStruct->PLLP2);
      /* Check VCO Output frequency */
      assert_param(IS_LL_UTILS_PLLVCO_OUTPUT(pllfreq * pUTILS_PLLInitStruct->PLLP1 * pUTILS_PLLInitStruct->PLLP2));

      /* Enable HSI if not enabled */
      if (LL_RCC_HSI_IsReady() != 1U)
      {
        LL_RCC_HSI_Enable();
        while (LL_RCC_HSI_IsReady() != 1U)
        {
          /* Wait for HSI ready */
        }
      }

      /* Configure PLL1 */
      LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSI);

      UTILS_ConfigurePLL1InIntegerMode(pUTILS_PLLInitStruct);

      UTILS_ConfigureIC(pUTILS_ICInitStruct);

      /* Enable PLL and switch CPU/system clock to PLL */
      status = UTILS_EnablePLL1AndSwitchSystem(pllfreq / pUTILS_ICInitStruct->IC1Divider, pUTILS_ClkInitStruct);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    /* Current PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function configures the CPU system clock with MSI as clock source of the PLL1
  *         used in integer mode.
  * @note   The application needs to ensure that PLL1 is disabled.
  * @note   Function is based on the following formula:
  *         - PLL output frequency = (((MSI frequency / PLLM) * PLLN) / PLLP1 / PLLP2)
  *         - PLLM: ensure that the VCO input frequency ranges from 1 to 50 MHz (PLLVCO_input = MSI frequency / PLLM)
  *         - PLLN: ensure that the VCO output frequency is maximum 3200 MHz (PLLVCO_output = PLLVCO_input * PLLN)
  *         - PLLP1, PLLP2: ensure that max frequency at 800 MHz is reached (PLLVCO_output / PLLP1 / PLLP2)
  * @param  pUTILS_PLLInitStruct pointer to a @ref LL_UTILS_PLLInitTypeDef structure that contains
  *                             the configuration information for the PLL.
  * @param  pUTILS_ICInitStruct pointer to a @ref LL_UTILS_ICInitTypeDef structure that contains
  *                             the configuration information for the IC.
  * @param  pUTILS_ClkInitStruct pointer to a @ref LL_UTILS_ClkInitTypeDef structure that contains
  *                             the configuration information for the BUS prescalers.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Max frequency configuration done
  *          - ERROR: Max frequency configuration not done
  *
  */
ErrorStatus LL_PLL_ConfigSystemClock_MSI(const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct)
{
  ErrorStatus status;
  uint32_t pllfreq;

  /* Check the PLL parameters */
  assert_param(IS_LL_UTILS_PLLM_VALUE(pUTILS_PLLInitStruct->PLLM));
  assert_param(IS_LL_UTILS_PLLN_VALUE(pUTILS_PLLInitStruct->PLLN));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP1));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP2));
  assert_param(IS_LL_UTILS_FRACN_VALUE(pUTILS_PLLInitStruct->FRACN));

  /* Check VCO Input frequency */
  assert_param(IS_LL_UTILS_PLLVCO_INPUT(MSI_VALUE / pUTILS_PLLInitStruct->PLLM));

  /* Check that PLL1 is not enabled and thus ready for configuration */
  if (LL_RCC_PLL1_IsReady() != 1U)
  {
    /* Integer mode only */
    if (pUTILS_PLLInitStruct->FRACN == 0U)
    {
      /* Calculate the new PLL output frequency */
      pllfreq = LL_RCC_CalcPLLClockFreq(MSI_VALUE, pUTILS_PLLInitStruct->PLLM,
                                        pUTILS_PLLInitStruct->PLLN, pUTILS_PLLInitStruct->FRACN,
                                        pUTILS_PLLInitStruct->PLLP1, pUTILS_PLLInitStruct->PLLP2);

      /* Check VCO Output frequency */
      assert_param(IS_LL_UTILS_PLLVCO_OUTPUT(pllfreq * pUTILS_PLLInitStruct->PLLP1 * pUTILS_PLLInitStruct->PLLP2));

      /* Enable MSI if not enabled */
      if (LL_RCC_MSI_IsReady() != 1U)
      {
        LL_RCC_MSI_Enable();
        while (LL_RCC_MSI_IsReady() != 1U)
        {
          /* Wait for MSI ready */
        }
      }

      /* Configure PLL1 */
      LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_MSI);

      UTILS_ConfigurePLL1InIntegerMode(pUTILS_PLLInitStruct);

      UTILS_ConfigureIC(pUTILS_ICInitStruct);

      /* Enable PLL and switch system clock to PLL */
      status = UTILS_EnablePLL1AndSwitchSystem(pllfreq / pUTILS_ICInitStruct->IC1Divider, pUTILS_ClkInitStruct);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    /* Current PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function configures the CPU system clock with HSE as clock source of the PLL1
  *         used in integer mode.
  * @note   The application needs to ensure that PLL1 is disabled.
  * @note   Function is based on the following formula:
  *         - PLL output frequency = (((HSE frequency / PLLM) * PLLN) / PLLP1 / PLLP2)
  *         - PLLM: ensure that the VCO input frequency ranges from 1 to 50 MHz (PLLVCO_input = HSE frequency / PLLM)
  *         - PLLN: ensure that the VCO output frequency is maximum 3200 MHz (PLLVCO_output = PLLVCO_input * PLLN)
  *         - PLLP1, PLLP2: ensure that max frequency at 800 MHz is reached (PLLVCO_output / PLLP1 / PLLP2)
  * @param  HSEFrequency Value between Min_Data = 4000000 and Max_Data = 50000000
  * @param  HSEBypass This parameter can be one of the following values:
  *         @arg @ref LL_UTILS_HSEBYPASS_ON
  *         @arg @ref LL_UTILS_HSEBYPASS_OFF
  * @param  pUTILS_PLLInitStruct pointer to a @ref LL_UTILS_PLLInitTypeDef structure that contains
  *                             the configuration information for the PLL.
  * @param  pUTILS_ICInitStruct pointer to a @ref LL_UTILS_ICInitTypeDef structure that contains
  *                             the configuration information for the IC.
  * @param  pUTILS_ClkInitStruct pointer to a @ref LL_UTILS_ClkInitTypeDef structure that contains
  *                             the configuration information for the BUS prescalers.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Max frequency configuration done
  *          - ERROR: Max frequency configuration not done
  *
  */
ErrorStatus LL_PLL_ConfigSystemClock_HSE(uint32_t HSEFrequency, uint32_t HSEBypass,
                                         const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct)
{
  ErrorStatus status;
  uint32_t pllfreq;

  /* Check the HSE parameters */
  assert_param(IS_LL_UTILS_HSE_FREQUENCY(HSEFrequency));
  assert_param(IS_LL_UTILS_HSE_BYPASS(HSEBypass));
  /* Check the PLL parameters */
  assert_param(IS_LL_UTILS_PLLM_VALUE(pUTILS_PLLInitStruct->PLLM));
  assert_param(IS_LL_UTILS_PLLN_VALUE(pUTILS_PLLInitStruct->PLLN));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP1));
  assert_param(IS_LL_UTILS_PLLP_VALUE(pUTILS_PLLInitStruct->PLLP2));
  assert_param(IS_LL_UTILS_FRACN_VALUE(pUTILS_PLLInitStruct->FRACN));

  /* Check VCO Input frequency */
  assert_param(IS_LL_UTILS_PLLVCO_INPUT(HSEFrequency / pUTILS_PLLInitStruct->PLLM));

  /* Check that PLL1 is not enabled and thus ready for configuration */
  if (LL_RCC_PLL1_IsReady() != 1U)
  {
    /* Integer mode only */
    if (pUTILS_PLLInitStruct->FRACN == 0U)
    {
      /* Calculate the new PLL output frequency */
      pllfreq = LL_RCC_CalcPLLClockFreq(HSEFrequency, pUTILS_PLLInitStruct->PLLM,
                                        pUTILS_PLLInitStruct->PLLN, pUTILS_PLLInitStruct->FRACN,
                                        pUTILS_PLLInitStruct->PLLP1, pUTILS_PLLInitStruct->PLLP2);

      /* Check VCO Output frequency */
      assert_param(IS_LL_UTILS_PLLVCO_OUTPUT(pllfreq * pUTILS_PLLInitStruct->PLLP1 * pUTILS_PLLInitStruct->PLLP2));

      /* Enable HSE if not enabled */
      if (LL_RCC_HSE_IsReady() != 1U)
      {
        /* Check if need to enable HSE bypass feature or not */
        if (HSEBypass == LL_UTILS_HSEBYPASS_ON)
        {
          LL_RCC_HSE_EnableBypass();
        }
        else
        {
          LL_RCC_HSE_DisableBypass();
        }

        /* Enable HSE */
        LL_RCC_HSE_Enable();
        while (LL_RCC_HSE_IsReady() != 1U)
        {
          /* Wait for HSE ready */
        }
      }

      /* Configure PLL1 */
      LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSE);

      UTILS_ConfigurePLL1InIntegerMode(pUTILS_PLLInitStruct);

      UTILS_ConfigureIC(pUTILS_ICInitStruct);

      /* Enable PLL and switch system clock to PLL */
      status = UTILS_EnablePLL1AndSwitchSystem(pllfreq / pUTILS_ICInitStruct->IC1Divider, pUTILS_ClkInitStruct);
    }
    else
    {
      status = ERROR;
    }
  }
  else
  {
    /* Current PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup UTILS_LL_Private_Functions
  * @{
  */


/**
  * @brief  Function to configure PLL1 in Integer mode
  * @param  pUTILS_PLLInitStruct pointer to a @ref LL_UTILS_PLLInitTypeDef structure that contains
  *                             the configuration information for the PLL.
  * @retval None
  */
static void UTILS_ConfigurePLL1InIntegerMode(const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct)
{
  LL_RCC_PLL1_DisableModulationSpreadSpectrum();
  LL_RCC_PLL1_SetM(pUTILS_PLLInitStruct->PLLM);
  LL_RCC_PLL1_SetN(pUTILS_PLLInitStruct->PLLN);
  LL_RCC_PLL1_SetFRACN(pUTILS_PLLInitStruct->FRACN);
  LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
  LL_RCC_PLL1_AssertModulationSpreadSpectrumReset();
  LL_RCC_PLL1_SetP1(pUTILS_PLLInitStruct->PLLP1);
  LL_RCC_PLL1_SetP2(pUTILS_PLLInitStruct->PLLP2);
}

/**
  * @brief  Function to configure IC for CPU/System buses clocks
  * @param  pUTILS_ICInitStruct pointer to a @ref LL_UTILS_ICInitTypeDef structure that contains
  *                             the configuration information for the IC (IC1, IC2, IC6 and IC11).
  * @retval None
  */
static void UTILS_ConfigureIC(const LL_UTILS_ICInitTypeDef *pUTILS_ICInitStruct)
{
  /* Configure and enable each IC used for CPU/System buses clocks */
  LL_RCC_IC1_SetSource(pUTILS_ICInitStruct->IC1Source);
  LL_RCC_IC1_SetDivider(pUTILS_ICInitStruct->IC1Divider);
  LL_RCC_IC1_Enable();
  LL_RCC_IC2_SetSource(pUTILS_ICInitStruct->IC2Source);
  LL_RCC_IC2_SetDivider(pUTILS_ICInitStruct->IC2Divider);
  LL_RCC_IC2_Enable();
  LL_RCC_IC6_SetSource(pUTILS_ICInitStruct->IC6Source);
  LL_RCC_IC6_SetDivider(pUTILS_ICInitStruct->IC6Divider);
  LL_RCC_IC6_Enable();
  LL_RCC_IC11_SetSource(pUTILS_ICInitStruct->IC11Source);
  LL_RCC_IC11_SetDivider(pUTILS_ICInitStruct->IC11Divider);
  LL_RCC_IC11_Enable();
}

/**
  * @brief  Function to enable PLL1 and switch CPU/system clock to PLL
  * @param  CPU_Frequency CPU frequency
  * @param  pUTILS_ClkInitStruct pointer to a @ref LL_UTILS_ClkInitTypeDef structure that contains
  *                             the configuration information for the BUS prescalers.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: No problem to switch system to PLL
  *          - ERROR: Problem to switch system to PLL
  */
static ErrorStatus UTILS_EnablePLL1AndSwitchSystem(uint32_t CPU_Frequency,
                                                   const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct)
{
  ErrorStatus status = SUCCESS;

  assert_param(IS_LL_UTILS_AHB_DIV(pUTILS_ClkInitStruct->AHBCLKDivider));
  assert_param(IS_LL_UTILS_APB1_DIV(pUTILS_ClkInitStruct->APB1CLKDivider));
  assert_param(IS_LL_UTILS_APB2_DIV(pUTILS_ClkInitStruct->APB2CLKDivider));
  assert_param(IS_LL_UTILS_APB4_DIV(pUTILS_ClkInitStruct->APB4CLKDivider));
  assert_param(IS_LL_UTILS_APB5_DIV(pUTILS_ClkInitStruct->APB5CLKDivider));

  /* Update system clock configuration */
  /* Enable PLL1 */
  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1U)
  {
    /* Wait for PLL1 ready */
  }

  LL_RCC_PLL1P_Enable();

  /* Set APBx prescalers to the highest divider */
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_128);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_128);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_128);
  LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_128);

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(pUTILS_ClkInitStruct->AHBCLKDivider);

  /* CPU clock switch on the IC1 */
  LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
  while (LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1)
  {
    /* Wait for CPU clock switch to IC1 */
  }

  /* System buses clock switch on the IC2, IC6 and IC11 */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11)
  {
    /* Wait for System buses clock switch to IC2, IC6 and IC11 */
  }

  LL_SetSystemCoreClock(CPU_Frequency);

  /* Set APBx prescalers */
  LL_RCC_SetAPB1Prescaler(pUTILS_ClkInitStruct->APB1CLKDivider);
  LL_RCC_SetAPB2Prescaler(pUTILS_ClkInitStruct->APB2CLKDivider);
  LL_RCC_SetAPB4Prescaler(pUTILS_ClkInitStruct->APB4CLKDivider);
  LL_RCC_SetAPB5Prescaler(pUTILS_ClkInitStruct->APB5CLKDivider);

  return status;
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
