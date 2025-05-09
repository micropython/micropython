/**
  ******************************************************************************
  * @file    system_stm32n6xx_s.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M55 Device Peripheral Access Layer System Source File
  *          to be used in secure application.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at secure startup just after reset
  *                      and before branch to secure main program.
  *                      This call is made inside the "startup_stm32n6xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the CPU core clock, it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *      - SECURE_SystemCoreClockUpdate(): Non-secure callable function to update
  *                                        the variable SystemCoreClock and return
  *                                        its value to the non-secure calling
  *                                        application. It must be called whenever
  *                                        the core clock is changed during program
  *                                        execution.
  *
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32n6xx.s" file, to
  *   configure the system before to branch to main program.
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
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32N6xx_System
  * @{
  */

/** @addtogroup STM32N6xx_System_Private_Includes
  * @{
  */

#include "stm32n6xx.h"
#include "partition_stm32n6xx.h"  /* Trustzone-M core secure attributes */
#include <math.h>

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_TypesDefinitions
  * @{
  */

#if defined ( __ICCARM__ )
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE)
#define HSE_VALUE      48000000UL /*!< Value of the High-Speed External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE      64000000UL /*!< Value of the High-Speed Internal oscillator in Hz */
#endif /* HSI_VALUE */

#if !defined  (MSI_VALUE)
  #define MSI_VALUE       4000000UL /*!< Minimum value of the Low-power Internal oscillator in Hz */
#endif /* MSI_VALUE */

#if !defined  (EXTERNAL_I2S_CLOCK_VALUE)
  #define EXTERNAL_I2S_CLOCK_VALUE  12288000UL /*!< Value of the External clock for I2S_CKIN in Hz */
#endif /* EXTERNAL_I2S_CLOCK_VALUE */


/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in memory, else the vector table is kept at the automatic
     selected boot address */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
#if !defined(VECT_TAB_BASE_ADDRESS)
#define VECT_TAB_BASE_ADDRESS   SRAM1_AXI_BASE_S /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x400. */
#endif

#if !defined(VECT_TAB_OFFSET)
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x400. */
#endif
#endif /* USER_VECT_TAB_ADDRESS */

/******************************************************************************/
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
uint32_t SystemCoreClock = HSI_VALUE;
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_Functions
  * @{
  */

#if defined(__ICCARM__)
extern uint32_t __vector_table;
#define INTVECT_START ((uint32_t)&__vector_table)
#elif defined(__ARMCC_VERSION)
extern void *__Vectors;
#define INTVECT_START ((uint32_t)&__Vectors)
#elif defined(__GNUC__)
extern void *g_pfnVectors;
#define INTVECT_START ((uint32_t)&g_pfnVectors)
#endif

/**
  * @brief  Setup the microcontroller system.
  * @retval None
  */

void SystemInit(void)
{
  /* SAU/IDAU, FPU and Interrupts secure/non-secure allocation settings */
  TZ_SAU_Setup();

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#else
  SCB->VTOR = INTVECT_START;
#endif  /* USER_VECT_TAB_ADDRESS */

  /* System configuration setup */
  RCC->APB4ENSR2 = RCC_APB4ENSR2_SYSCFGENS;
  /* Delay after an RCC peripheral clock enabling */
  (void)RCC->APB4ENR2;

  /* Set default Vector Table location after system reset or return from Standby */
  SYSCFG->INITSVTORCR = SCB->VTOR;
  /* Read back the value to make sure it is written before deactivating SYSCFG */
  (void) SYSCFG->INITSVTORCR;
  /* Deactivate SYSCFG clock */
  RCC->APB4ENCR2 = RCC_APB4ENCR2_SYSCFGENC;
  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */

  SCB_NS->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif /* __FPU_PRESENT && __FPU_USED */

}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Depending on secure or non-secure compilation, the adequate RCC peripheral
  *         memory are is accessed thanks to RCC alias defined in stm32n6xxxx.h device file
  *         so either from RCC_S peripheral register mapped memory in secure or from
  *         RCC_NS peripheral register mapped memory in non-secure.
  *
  * @note   Each time the CPU core clock changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If CPUCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *
  *           - If CPUCLK source is MSI, SystemCoreClock will contain the MSI_VALUE(**)
  *
  *           - If CPUCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If CPUCLK source is IC1, SystemCoreClock will contain the HSI_VALUE(*)
  *             or MSI_VALUE(**) or HSE_VALUE(***) or EXTERNAL_I2S_CLOCK_VALUE (****)
  *             multiplied/divided by the PLL factors.
  *
  *         (*) HSI_VALUE default value is 64 MHz.
  *             With the HAL, HSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file
  *             but the real value may vary depending on the variations in voltage and temperature.
  *
  *        (**) MSI_VALUE default value is 4 MHz.
  *             With the HAL, MSI_VALUE is a constant defined in stm32n6xx_hal_conf.h file
  *             but the real value may vary depending on the variations in voltage and temperature.
  *
  *       (***) HSE_VALUE default value is 30 MHz.
  *             With the HAL, HSE_VALUE is a constant defined in stm32n6xx_hal_conf.h file.
  *             User has to ensure that HSE_VALUE is same as the real frequency of the crystal used.
  *             Otherwise, this function may have wrong result.
  *
  *      (****) EXTERNAL_I2S_CLOCK_VALUE default value is 12.288 MHz.
  *             With the HAL, EXTERNAL_I2S_CLOCK_VALUE is a constant defined in stm32n6xx_hal_conf.h file.
  *             User has to ensure that EXTERNAL_I2S_CLOCK_VALUE is same as the real I2S_CKIN
  *             pin frequency. Otherwise, this function may have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t sysclk = 0;
  uint32_t pllm = 0;
  uint32_t plln = 0;
  uint32_t pllfracn = 0;
  uint32_t pllp1 = 0;
  uint32_t pllp2 = 0;
  uint32_t pllcfgr, pllsource, pllbypass, ic_divider;
  float_t pllvco;

  /* Get CPUCLK source -------------------------------------------------------*/
  switch (RCC->CFGR1 & RCC_CFGR1_CPUSWS)
  {
  case 0:  /* HSI used as system clock source (default after reset) */
    sysclk = HSI_VALUE >> ((RCC->HSICFGR & RCC_HSICFGR_HSIDIV) >> RCC_HSICFGR_HSIDIV_Pos);
    break;

  case RCC_CFGR1_CPUSWS_0:  /* MSI used as system clock source */
    if (READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL) == 0UL)
    {
      sysclk = MSI_VALUE;
    }
    else
    {
      sysclk = 16000000UL;
    }
    break;

  case RCC_CFGR1_CPUSWS_1:  /* HSE used as system clock source */
    sysclk = HSE_VALUE;
    break;

  case (RCC_CFGR1_CPUSWS_1 | RCC_CFGR1_CPUSWS_0):  /* IC1 used as system clock  source */
    /* Get IC1 clock source parameters */
    switch (READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1SEL))
    {
    case 0:  /* PLL1 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL1CFGR1);
      pllsource = pllcfgr & RCC_PLL1CFGR1_PLL1SEL;
      pllbypass = pllcfgr & RCC_PLL1CFGR1_PLL1BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL1CFGR1_PLL1DIVM) >>  RCC_PLL1CFGR1_PLL1DIVM_Pos;
        plln = (pllcfgr & RCC_PLL1CFGR1_PLL1DIVN) >>  RCC_PLL1CFGR1_PLL1DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL1CFGR2, RCC_PLL1CFGR2_PLL1DIVNFRAC) >>  RCC_PLL1CFGR2_PLL1DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL1CFGR3);
        pllp1 = (pllcfgr & RCC_PLL1CFGR3_PLL1PDIV1) >>  RCC_PLL1CFGR3_PLL1PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL1CFGR3_PLL1PDIV2) >>  RCC_PLL1CFGR3_PLL1PDIV2_Pos;
      }
      break;
    case RCC_IC1CFGR_IC1SEL_0:  /* PLL2 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL2CFGR1);
      pllsource = pllcfgr & RCC_PLL2CFGR1_PLL2SEL;
      pllbypass = pllcfgr & RCC_PLL2CFGR1_PLL2BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL2CFGR1_PLL2DIVM) >>  RCC_PLL2CFGR1_PLL2DIVM_Pos;
        plln = (pllcfgr & RCC_PLL2CFGR1_PLL2DIVN) >>  RCC_PLL2CFGR1_PLL2DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL2CFGR2, RCC_PLL2CFGR2_PLL2DIVNFRAC) >>  RCC_PLL2CFGR2_PLL2DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL2CFGR3);
        pllp1 = (pllcfgr & RCC_PLL2CFGR3_PLL2PDIV1) >>  RCC_PLL2CFGR3_PLL2PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL2CFGR3_PLL2PDIV2) >>  RCC_PLL2CFGR3_PLL2PDIV2_Pos;
      }
      break;

    case RCC_IC1CFGR_IC1SEL_1:  /* PLL3 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL3CFGR1);
      pllsource = pllcfgr & RCC_PLL3CFGR1_PLL3SEL;
      pllbypass = pllcfgr & RCC_PLL3CFGR1_PLL3BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL3CFGR1_PLL3DIVM) >>  RCC_PLL3CFGR1_PLL3DIVM_Pos;
        plln = (pllcfgr & RCC_PLL3CFGR1_PLL3DIVN) >>  RCC_PLL3CFGR1_PLL3DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL3CFGR2, RCC_PLL3CFGR2_PLL3DIVNFRAC) >>  RCC_PLL3CFGR2_PLL3DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL3CFGR3);
        pllp1 = (pllcfgr & RCC_PLL3CFGR3_PLL3PDIV1) >>  RCC_PLL3CFGR3_PLL3PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL3CFGR3_PLL3PDIV2) >>  RCC_PLL3CFGR3_PLL3PDIV2_Pos;
      }
      break;

    default: /* RCC_IC1CFGR_IC1SEL_1 | RCC_IC1CFGR_IC1SEL_0 */  /* PLL4 selected at IC1 clock source */
      pllcfgr = READ_REG(RCC->PLL4CFGR1);
      pllsource = pllcfgr & RCC_PLL4CFGR1_PLL4SEL;
      pllbypass = pllcfgr & RCC_PLL4CFGR1_PLL4BYP;
      if (pllbypass == 0U)
      {
        pllm = (pllcfgr & RCC_PLL4CFGR1_PLL4DIVM) >>  RCC_PLL4CFGR1_PLL4DIVM_Pos;
        plln = (pllcfgr & RCC_PLL4CFGR1_PLL4DIVN) >>  RCC_PLL4CFGR1_PLL4DIVN_Pos;
        pllfracn = READ_BIT(RCC->PLL4CFGR2, RCC_PLL4CFGR2_PLL4DIVNFRAC) >>  RCC_PLL4CFGR2_PLL4DIVNFRAC_Pos;
        pllcfgr = READ_REG(RCC->PLL4CFGR3);
        pllp1 = (pllcfgr & RCC_PLL4CFGR3_PLL4PDIV1) >>  RCC_PLL4CFGR3_PLL4PDIV1_Pos;
        pllp2 = (pllcfgr & RCC_PLL4CFGR3_PLL4PDIV2) >>  RCC_PLL4CFGR3_PLL4PDIV2_Pos;
      }
      break;
    }

    /* Get oscillator frequency used as PLL clock source */
    switch (pllsource)
    {
    case 0:  /* HSI selected as PLL clock source */
      sysclk = HSI_VALUE >> ((RCC->HSICFGR & RCC_HSICFGR_HSIDIV) >> RCC_HSICFGR_HSIDIV_Pos);
      break;
    case RCC_PLL1CFGR1_PLL1SEL_0: /* MSI selected as PLL clock source */
      if (READ_BIT(RCC->MSICFGR, RCC_MSICFGR_MSIFREQSEL) == 0UL)
      {
        sysclk = MSI_VALUE;
      }
      else
      {
        sysclk = 16000000UL;
      }
      break;
    case RCC_PLL1CFGR1_PLL1SEL_1: /* HSE selected as PLL clock source */
      sysclk = HSE_VALUE;
      break;
    case (RCC_PLL1CFGR1_PLL1SEL_1 | RCC_PLL1CFGR1_PLL1SEL_0):  /* I2S_CKIN selected as PLL clock source */
      sysclk = EXTERNAL_I2S_CLOCK_VALUE;
      break;
    default:
      /* Nothing to do, should not occur */
      break;
    }

    /* Check whether PLL is in bypass mode or not */
    if (pllbypass == 0U)
    {
      /* Compte PLL output frequency (Integer and fractional modes) */
      /* PLLVCO = (Freq * (DIVN + (FRACN / 0x1000000) / DIVM) / (DIVP1 * DIVP2)) */
      pllvco = ((float_t)sysclk * ((float_t)plln + ((float_t)pllfracn/(float_t)0x1000000UL))) / (float_t)pllm;
      sysclk = (uint32_t)((float_t)(pllvco/(((float_t)pllp1) * ((float_t)pllp2))));
    }
    /* Apply IC1 divider */
    ic_divider = (READ_BIT(RCC->IC1CFGR, RCC_IC1CFGR_IC1INT) >> RCC_IC1CFGR_IC1INT_Pos) + 1UL;
    sysclk = sysclk / ic_divider;
    break;
  default:
    /* Nothing to do, should not occur */
    break;
  }

  /* Return system clock frequency (CPU frequency) */
  SystemCoreClock = sysclk;
}

/**
  * @brief  Secure Non-Secure-Callable function to return the current
  *         SystemCoreClock value after SystemCoreClock update.
  *         The SystemCoreClock variable contains the CPU core clock, it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  * @retval SystemCoreClock value
  */
CMSE_NS_ENTRY uint32_t SECURE_SystemCoreClockUpdate(void)
{
  SystemCoreClockUpdate();

  return SystemCoreClock;
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
