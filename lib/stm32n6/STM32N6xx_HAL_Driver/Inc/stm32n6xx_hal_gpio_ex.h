/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gpio_ex.h
  * @author  GPM Application Team
  * @brief   Header file of GPIO HAL Extended module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_GPIO_EX_H
#define STM32N6xx_HAL_GPIO_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup GPIOEx GPIOEx
  * @brief GPIO Extended HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIOEx_Exported_Constants GPIOEx Exported Constants
  * @{
  */

/** @defgroup GPIOEx_Alternate_function_selection GPIOEx Alternate function selection
  * @{
  */

/**
  * @brief   AF 0 selection
  */
#define GPIO_AF0_PWR           ((uint8_t)0x00)  /*!< PWR Alternate Function mapping */
#define GPIO_AF0_MCO           ((uint8_t)0x00)  /*!< MCO Alternate Function mapping */
#define GPIO_AF0_RTC           ((uint8_t)0x00)  /*!< RTC Alternate Function mapping */
#define GPIO_AF0_SWJ           ((uint8_t)0x00)  /*!< SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF0_TRACE         ((uint8_t)0x00)  /*!< TRACE Alternate Function mapping */
#define GPIO_AF0_SYS           ((uint8_t)0x00)  /*!< BOOT1 Alternate Function mapping */

/**
  * @brief   AF 1 selection
  */
#define GPIO_AF1_LPTIM1        ((uint8_t)0x01)  /*!< LPTIM1 Alternate Function mapping */
#define GPIO_AF1_LPTIM5        ((uint8_t)0x01)  /*!< LPTIM5 Alternate Function mapping */
#define GPIO_AF1_TIM1          ((uint8_t)0x01)  /*!< TIM1 Alternate Function mapping */
#define GPIO_AF1_TIM2          ((uint8_t)0x01)  /*!< TIM2 Alternate Function mapping */
#define GPIO_AF1_TIM16         ((uint8_t)0x01)  /*!< TIM16 Alternate Function mapping */
#define GPIO_AF1_TIM17         ((uint8_t)0x01)  /*!< TIM17 Alternate Function mapping */

/**
  * @brief   AF 2 selection
  */
#define GPIO_AF2_GFXTIM        ((uint8_t)0x02)  /*!< GFXTIM Alternate Function mapping */
#define GPIO_AF2_I3C2          ((uint8_t)0x02)  /*!< I3C2 Alternate Function mapping */
#define GPIO_AF2_SAI1          ((uint8_t)0x02)  /*!< SAI1 Alternate Function mapping */
#define GPIO_AF2_TIM2          ((uint8_t)0x02)  /*!< TIM2 Alternate Function mapping */
#define GPIO_AF2_TIM3          ((uint8_t)0x02)  /*!< TIM3 Alternate Function mapping */
#define GPIO_AF2_TIM4          ((uint8_t)0x02)  /*!< TIM4 Alternate Function mapping */
#define GPIO_AF2_TIM5          ((uint8_t)0x02)  /*!< TIM5 Alternate Function mapping */
#define GPIO_AF2_TIM8          ((uint8_t)0x02)  /*!< TIM8 Alternate Function mapping */
#define GPIO_AF2_TIM12         ((uint8_t)0x02)  /*!< TIM12 Alternate Function mapping */

/**
  * @brief   AF 3 selection
  */
#define GPIO_AF3_ADF1          ((uint8_t)0x03)  /*!< ADF1 Alternate Function mapping */
#define GPIO_AF3_LPTIM2        ((uint8_t)0x03)  /*!< LPTIM2 Alternate Function mapping */
#define GPIO_AF3_LPTIM3        ((uint8_t)0x03)  /*!< LPTIM3 Alternate Function mapping */
#define GPIO_AF3_LPTIM4        ((uint8_t)0x03)  /*!< LPTIM4 Alternate Function mapping */
#define GPIO_AF3_LPUART1       ((uint8_t)0x03)  /*!< LPUART1 Alternate Function mapping */
#define GPIO_AF3_TIM9          ((uint8_t)0x03)  /*!< TIM9 Alternate Function mapping */
#define GPIO_AF3_USART3        ((uint8_t)0x03)  /*!< USART3 Alternate Function mapping */

/**
  * @brief   AF 4 selection
  */
#define GPIO_AF4_I2C1          ((uint8_t)0x04)  /*!< I2C1 Alternate Function mapping */
#define GPIO_AF4_I2C2          ((uint8_t)0x04)  /*!< I2C2 Alternate Function mapping */
#define GPIO_AF4_I2C3          ((uint8_t)0x04)  /*!< I2C3 Alternate Function mapping */
#define GPIO_AF4_I2C4          ((uint8_t)0x04)  /*!< I2C4 Alternate Function mapping */
#define GPIO_AF4_I3C1          ((uint8_t)0x04)  /*!< I3C1 Alternate Function mapping */
#define GPIO_AF4_MDF1          ((uint8_t)0x04)  /*!< MDF1 Alternate Function mapping */
#define GPIO_AF4_TIM15         ((uint8_t)0x04)  /*!< TIM15 Alternate Function mapping */
#define GPIO_AF4_USART1        ((uint8_t)0x04)  /*!< UASRT1 Alternate Function mapping */

/**
  * @brief   AF 5 selection
  */
#define GPIO_AF5_I3C1          ((uint8_t)0x05)  /*!< I3C1 Alternate Function mapping */
#define GPIO_AF5_SPI1          ((uint8_t)0x05)  /*!< SPI1 Alternate Function mapping */
#define GPIO_AF5_SPI2          ((uint8_t)0x05)  /*!< SPI2 Alternate Function mapping */
#define GPIO_AF5_SPI3          ((uint8_t)0x05)  /*!< SPI3 Alternate Function mapping */
#define GPIO_AF5_SPI4          ((uint8_t)0x05)  /*!< SPI4 Alternate Function mapping */
#define GPIO_AF5_SPI5          ((uint8_t)0x05)  /*!< SPI5 Alternate Function mapping */
#define GPIO_AF5_SPI6          ((uint8_t)0x05)  /*!< SPI6 Alternate Function mapping */

/**
  * @brief   AF 6 selection
  */
#define GPIO_AF6_FDCAN1        ((uint8_t)0x06)  /*!< FDCAN1 Alternate Function mapping */
#define GPIO_AF6_FDCAN2        ((uint8_t)0x06)  /*!< FDCAN2 Alternate Function mapping */
#define GPIO_AF6_FDCAN3        ((uint8_t)0x06)  /*!< FDCAN3 Alternate Function mapping */
#define GPIO_AF6_SAI1          ((uint8_t)0x06)  /*!< SAI1 Alternate Function mapping */
#define GPIO_AF6_SPI3          ((uint8_t)0x06)  /*!< SPI3 Alternate Function mapping */
#define GPIO_AF6_SPI4          ((uint8_t)0x06)  /*!< SPI4 Alternate Function mapping */
#define GPIO_AF6_UCPD1         ((uint8_t)0x06)  /*!< UCPD1 Alternate Function mapping */
#define GPIO_AF6_USART10       ((uint8_t)0x06)  /*!< USART10 Alternate Function mapping */

/**
  * @brief   AF 7 selection
  */
#define GPIO_AF7_I3C1          ((uint8_t)0x07)  /*!< I3C1 Alternate Function mapping */
#define GPIO_AF7_SPI2          ((uint8_t)0x07)  /*!< SPI2 Alternate Function mapping */
#define GPIO_AF7_SPI3          ((uint8_t)0x07)  /*!< SPI3 Alternate Function mapping */
#define GPIO_AF7_SPI6          ((uint8_t)0x07)  /*!< SPI6 Alternate Function mapping */
#define GPIO_AF7_TIM15         ((uint8_t)0x07)  /*!< TIM15 Alternate Function mapping */
#define GPIO_AF7_UART9         ((uint8_t)0x07)  /*!< UART9 Alternate Function mapping */
#define GPIO_AF7_UCPD1         ((uint8_t)0x07)  /*!< UCPD1 Alternate Function mapping */
#define GPIO_AF7_USART1        ((uint8_t)0x07)  /*!< USART1 Alternate Function mapping */
#define GPIO_AF7_USART2        ((uint8_t)0x07)  /*!< USART2 Alternate Function mapping */
#define GPIO_AF7_USART3        ((uint8_t)0x07)  /*!< USART3 Alternate Function mapping */
#define GPIO_AF7_USART6        ((uint8_t)0x07)  /*!< USART6 Alternate Function mapping */

/**
  * @brief   AF 8 selection
  */
#define GPIO_AF8_GFXTIM        ((uint8_t)0x08)  /*!< GFXTIM Alternate Function mapping */
#define GPIO_AF8_SPDIFRX       ((uint8_t)0x08)  /*!< SPDIFRX Alternate Function mapping */
#define GPIO_AF8_SPI6          ((uint8_t)0x08)  /*!< SPI6 Alternate Function mapping */
#define GPIO_AF8_UART4         ((uint8_t)0x08)  /*!< UART4 Alternate Function mapping */
#define GPIO_AF8_UART7         ((uint8_t)0x08)  /*!< UART7 Alternate Function mapping */
#define GPIO_AF8_UART8         ((uint8_t)0x08)  /*!< UART8 Alternate Function mapping */
#define GPIO_AF8_USART2        ((uint8_t)0x08)  /*!< USART2 Alternate Function mapping */

/**
  * @brief   AF 9 selection
  */
#define GPIO_AF9_XSPIM_P1      ((uint8_t)0x09)  /*!< XSPIM Manager Port 1 Alternate Function mapping  */
#define GPIO_AF9_XSPIM_P2      ((uint8_t)0x09)  /*!< XSPIM Manager Port 2 Alternate Function mapping  */
#define GPIO_AF9_PSSI          ((uint8_t)0x09)  /*!< PSSI Alternate Function mapping */
#define GPIO_AF9_SPI5          ((uint8_t)0x09)  /*!< SPI5 Alternate Function mapping */
#define GPIO_AF9_TIM11         ((uint8_t)0x09)  /*!< TIM11 Alternate Function mapping */
#define GPIO_AF9_DCMIPP        ((uint8_t)0x09)  /*!< DCMIPP Alternate Function mapping */
#define GPIO_AF9_DCMI          ((uint8_t)0x09)  /*!< DCMI Alternate Function mapping */

/**
  * @brief   AF 10 selection
  */
#define GPIO_AF10_LCD          ((uint8_t)0x0A)  /*!< LCD Alternate Function mapping */
#define GPIO_AF10_MDIOS        ((uint8_t)0x0A)  /*!< MDIOS Alternate Function mapping */
#define GPIO_AF10_PSSI         ((uint8_t)0x0A)  /*!< PSSI Alternate Function mapping */
#define GPIO_AF10_SAI2         ((uint8_t)0x0A)  /*!< SAI2 Alternate Function mapping */
#define GPIO_AF10_SDMMC1       ((uint8_t)0x0A)  /*!< SDMMC1 Alternate Function mapping */
#define GPIO_AF10_SPI1         ((uint8_t)0x0A)  /*!< SPI1 Alternate Function mapping */
#define GPIO_AF10_TIM10        ((uint8_t)0x0A)  /*!< TIM10 Alternate Function mapping */
#define GPIO_AF10_TIM13        ((uint8_t)0x0A)  /*!< TIM13 Alternate Function mapping */
#define GPIO_AF10_UART7        ((uint8_t)0x0A)  /*!< UART7 Alternate Function mapping */
#define GPIO_AF10_FMC          ((uint8_t)0x0A)  /*!< FMC Alternate Function mapping */
#define GPIO_AF10_DCMIPP       ((uint8_t)0x0A)  /*!< DCMIPP Alternate Function mapping */
#define GPIO_AF10_DCMI         ((uint8_t)0x0A)  /*!< DCMI Alternate Function mapping */

/**
  * @brief   AF 11 selection
  */
#define GPIO_AF11_ETH1         ((uint8_t)0x0B)  /*!< ETH1 Alternate Function mapping */
#define GPIO_AF11_MDIOS        ((uint8_t)0x0B)  /*!< MDIOS Alternate Function mapping */
#define GPIO_AF11_SDMMC2       ((uint8_t)0x0B)  /*!< SDMMC2 Alternate Function mapping */
#define GPIO_AF11_UART5        ((uint8_t)0x0B)  /*!< UART5 Alternate Function mapping */
#define GPIO_AF11_TIM14        ((uint8_t)0x0B)  /*!< TIM14 Alternate Function mapping */

/**
  * @brief   AF 12 selection
  */
#define GPIO_AF12_ETH1         ((uint8_t)0x0C)  /*!< ETH1 Alternate Function mapping */
#define GPIO_AF12_FMC          ((uint8_t)0x0C)  /*!< FMC Alternate Function mapping */
#define GPIO_AF12_GFXTIM       ((uint8_t)0x0C)  /*!< GFXTIM Alternate Function mapping */
#define GPIO_AF12_LCD          ((uint8_t)0x0C)  /*!< LCD Alternate Function mapping */
#define GPIO_AF12_SDMMC1       ((uint8_t)0x0C)  /*!< SDMMC1 Alternate Function mapping */

/**
  * @brief   AF 13 selection
  */
#define GPIO_AF13_GFXTIM       ((uint8_t)0x0D)  /*!< GFXTIM Alternate Function mapping */
#define GPIO_AF13_TIM1         ((uint8_t)0x0D)  /*!< TIM1 Alternate Function mapping */

/**
  * @brief   AF 14 selection
  */
#define GPIO_AF14_FMC          ((uint8_t)0x0E)   /*!< FMC Alternate Function mapping */
#define GPIO_AF14_LCD          ((uint8_t)0x0E)   /*!< LCD Alternate Function mapping */

/**
  * @brief   AF 15 selection
  */
#define GPIO_AF15_EVENTOUT     ((uint8_t)0x0F)    /*!< EVENTOUT Alternate Function mapping */
#define GPIO_AF15_HDP          ((uint8_t)0x0F)    /*!< HDP Alternate Function mapping */

/**
  * @brief   check if AF is valid
  */
#define IS_GPIO_AF(AF)   ((AF) <= (uint8_t)0x0F)

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIOEx_Exported_Macros GPIOEx Exported Macros
  * @{
  */

/** @defgroup GPIOEx_Get_Port_Index GPIOEx Get Port Index
  * @{
  */
#define GPIO_GET_INDEX(__GPIOx__)    (((__GPIOx__) == (GPIOA))? 0uL :\
                                      ((__GPIOx__) == (GPIOB))? 1uL :\
                                      ((__GPIOx__) == (GPIOC))? 2uL :\
                                      ((__GPIOx__) == (GPIOD))? 3uL :\
                                      ((__GPIOx__) == (GPIOE))? 4uL :\
                                      ((__GPIOx__) == (GPIOF))? 5uL :\
                                      ((__GPIOx__) == (GPIOG))? 6uL :\
                                      ((__GPIOx__) == (GPIOH))? 7uL :\
                                      ((__GPIOx__) == (GPION))? 8uL :\
                                      ((__GPIOx__) == (GPIOO))? 9uL :\
                                      ((__GPIOx__) == (GPIOP))? 10uL :\
                                      ((__GPIOx__) == (GPIOQ))? 11uL : 16uL)

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_GPIO_EX_H */

