/**
  ******************************************************************************
  * @file    system_stm32n6xx_ns.c
  * @author  GPM Application Team
  * @brief   CMSIS Cortex-M55 Device Peripheral Access Layer System Source File
  *          to be used in non-secure application when the system implements
  *          the TrustZone-M security.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at non-secure startup before
  *                      branch to non-secure main program.
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
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32n6xx.s" file, to
  *   configure the system before to branch to main secure program.
  *   Later, when non-secure SystemInit() function is called, in "startup_stm32n6xx.s"
  *   file, the system clock may have been updated from reset value by the main
  *   secure program.
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

/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Private_TypesDefinitions
  * @{
  */

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
#define VECT_TAB_BASE_ADDRESS   SRAM2_AXI_BASE_NS /*!< Vector Table base address field.
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
  /* Vector table location and FPU setup done by secure application */

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#else
  SCB->VTOR = INTVECT_START;
#endif  /* USER_VECT_TAB_ADDRESS */

  /* Non-secure main application shall call SystemCoreClockUpdate() to update */
  /* the SystemCoreClock variable to insure non-secure application relies on  */
  /* the initial clock reference set by secure application.                   */
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   From the non-secure application, the SystemCoreClock value is
  *         retrieved from the secure domain via a Non-Secure Callable function
  *         since the RCC peripheral may be protected with security attributes
  *         that prevent to compute the SystemCoreClock variable from the RCC
  *         peripheral registers.
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
  /* Get the SystemCoreClock value from the secure domain */
  SystemCoreClock = SECURE_SystemCoreClockUpdate();
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
