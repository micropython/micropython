/**
  ******************************************************************************
  * @file    system_stm32n6xx.h
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M55 Device System Source File for STM32N6xx devices.
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

/** @addtogroup STM32N6xx_system
  * @{
  */

#ifndef SYSTEM_STM32N6XX_H
#define SYSTEM_STM32N6XX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup STM32N6xx_System_Includes
  * @{
  */
#include <stdint.h>
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Exported_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
extern uint32_t SystemCoreClock;        /*!< System Clock Frequency (Core Clock)  */
/**
  * @}
  */

/** @addtogroup STM32N6xx_System_Exported_Functions
  * @{
  */

/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);


/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);


/**
  \brief  Update SystemCoreClock variable from secure application and return its value
          when security is implemented in the system (Non-secure callable function).

   Returns the SystemCoreClock value with current core Clock retrieved from cpu registers.
 */
extern uint32_t SECURE_SystemCoreClockUpdate(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTEM_STM32N6XX_H */

/**
  * @}
  */

/**
  * @}
  */

