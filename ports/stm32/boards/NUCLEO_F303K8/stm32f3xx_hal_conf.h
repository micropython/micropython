/**
  ******************************************************************************
  * @file    stm32f3xx_hal_conf.h
  * @author  MCD Application Team
  * @brief   HAL configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MICROPY_INCLUDED_STM32F3xx_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32F3xx_HAL_CONF_BASE_H

#include "boards/stm32f3xx_hal_conf_base.h"
/* ########################## HSE/HSI Values adaptation ##################### */
/**
  * @brief Adjust the value of External High Speed oscillator (HSE) used in your application.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSE is used as system clock source, directly or through the PLL).
  */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    (8000000U) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

/**
  * @brief In the following line adjust the External High Speed oscillator (HSE) Startup
  *        Timeout value
  */
#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    (100U)   /*!< Time out for HSE start up, in ms */
#endif /* HSE_STARTUP_TIMEOUT */

/**
  * @brief External Low Speed oscillator (LSE) value.
  */
#if !defined  (LSE_VALUE)
 #define LSE_VALUE  (32768U)    /*!< Value of the External Low Speed oscillator in Hz */
#endif /* LSE_VALUE */

/**
  * @brief Time out for LSE start up value in ms.
  */
#if !defined  (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    (5000U)   /*!< Time out for LSE start up, in ms */
#endif /* LSE_STARTUP_TIMEOUT */

/**
  * @brief External clock source for I2S peripheral
  *        This value is used by the I2S HAL module to compute the I2S clock source
  *        frequency, this source is inserted directly through I2S_CKIN pad.
  *        - External clock generated through external PLL component on EVAL 303 (based on MCO or crystal)
  *        - External clock not generated on EVAL 373
  */
#if !defined  (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    (8000000U) /*!< Value of the External oscillator in Hz*/
#endif /* EXTERNAL_CLOCK_VALUE */

/* Tip: To avoid modifying this file each time you need to use different HSE,
   ===  you can define the HSE value in your toolchain compiler preprocessor. */

#endif /* MICROPY_INCLUDED_STM32F3xx_HAL_CONF_BASE_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
