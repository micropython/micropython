/**
  ******************************************************************************
  * @file    stm32f7xx_hal_gpio_ex.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of GPIO HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_HAL_GPIO_EX_H
#define __STM32F7xx_HAL_GPIO_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup GPIOEx GPIOEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIOEx_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup GPIO_Alternate_function_selection GPIO Alternate Function Selection
  * @{
  */

/**
  * @brief   AF 0 selection
  */
#define GPIO_AF0_RTC_50Hz      ((uint8_t)0x00)  /* RTC_50Hz Alternate Function mapping                       */
#define GPIO_AF0_MCO           ((uint8_t)0x00)  /* MCO (MCO1 and MCO2) Alternate Function mapping            */
#define GPIO_AF0_SWJ           ((uint8_t)0x00)  /* SWJ (SWD and JTAG) Alternate Function mapping             */
#define GPIO_AF0_TRACE         ((uint8_t)0x00)  /* TRACE Alternate Function mapping                          */

/**
  * @brief   AF 1 selection
  */
#define GPIO_AF1_TIM1          ((uint8_t)0x01)  /* TIM1 Alternate Function mapping */
#define GPIO_AF1_TIM2          ((uint8_t)0x01)  /* TIM2 Alternate Function mapping */

/**
  * @brief   AF 2 selection
  */
#define GPIO_AF2_TIM3          ((uint8_t)0x02)  /* TIM3 Alternate Function mapping */
#define GPIO_AF2_TIM4          ((uint8_t)0x02)  /* TIM4 Alternate Function mapping */
#define GPIO_AF2_TIM5          ((uint8_t)0x02)  /* TIM5 Alternate Function mapping */

/**
  * @brief   AF 3 selection
  */
#define GPIO_AF3_TIM8          ((uint8_t)0x03)  /* TIM8 Alternate Function mapping  */
#define GPIO_AF3_TIM9          ((uint8_t)0x03)  /* TIM9 Alternate Function mapping  */
#define GPIO_AF3_TIM10         ((uint8_t)0x03)  /* TIM10 Alternate Function mapping */
#define GPIO_AF3_TIM11         ((uint8_t)0x03)  /* TIM11 Alternate Function mapping */
#define GPIO_AF3_LPTIM1        ((uint8_t)0x03)  /* LPTIM1 Alternate Function mapping */
#define GPIO_AF3_CEC           ((uint8_t)0x03)  /* CEC Alternate Function mapping */


/**
  * @brief   AF 4 selection
  */
#define GPIO_AF4_I2C1          ((uint8_t)0x04)  /* I2C1 Alternate Function mapping */
#define GPIO_AF4_I2C2          ((uint8_t)0x04)  /* I2C2 Alternate Function mapping */
#define GPIO_AF4_I2C3          ((uint8_t)0x04)  /* I2C3 Alternate Function mapping */
#define GPIO_AF4_I2C4          ((uint8_t)0x04)  /* I2C4 Alternate Function mapping */
#define GPIO_AF4_CEC           ((uint8_t)0x04)  /* CEC Alternate Function mapping */

/**
  * @brief   AF 5 selection
  */
#define GPIO_AF5_SPI1          ((uint8_t)0x05)  /* SPI1 Alternate Function mapping        */
#define GPIO_AF5_SPI2          ((uint8_t)0x05)  /* SPI2/I2S2 Alternate Function mapping   */
#define GPIO_AF5_SPI3          ((uint8_t)0x05)  /* SPI3/I2S3 Alternate Function mapping   */
#define GPIO_AF5_SPI4          ((uint8_t)0x05)  /* SPI4 Alternate Function mapping        */
#define GPIO_AF5_SPI5          ((uint8_t)0x05)  /* SPI5 Alternate Function mapping        */
#define GPIO_AF5_SPI6          ((uint8_t)0x05)  /* SPI6 Alternate Function mapping        */

/**
  * @brief   AF 6 selection
  */
#define GPIO_AF6_SPI3          ((uint8_t)0x06)  /* SPI3/I2S3 Alternate Function mapping  */
#define GPIO_AF6_SAI1          ((uint8_t)0x06)  /* SAI1 Alternate Function mapping       */

/**
  * @brief   AF 7 selection
  */
#define GPIO_AF7_USART1        ((uint8_t)0x07)  /* USART1 Alternate Function mapping     */
#define GPIO_AF7_USART2        ((uint8_t)0x07)  /* USART2 Alternate Function mapping     */
#define GPIO_AF7_USART3        ((uint8_t)0x07)  /* USART3 Alternate Function mapping     */
#define GPIO_AF7_UART5         ((uint8_t)0x07)  /* UART5 Alternate Function mapping      */
#define GPIO_AF7_SPDIFRX       ((uint8_t)0x07)  /* SPDIF-RX Alternate Function mapping   */
#define GPIO_AF7_SPI2          ((uint8_t)0x07)  /* SPI2 Alternate Function mapping       */
#define GPIO_AF7_SPI3          ((uint8_t)0x07)  /* SPI3 Alternate Function mapping       */

/**
  * @brief   AF 8 selection
  */
#define GPIO_AF8_UART4         ((uint8_t)0x08)  /* UART4 Alternate Function mapping  */
#define GPIO_AF8_UART5         ((uint8_t)0x08)  /* UART5 Alternate Function mapping  */
#define GPIO_AF8_USART6        ((uint8_t)0x08)  /* USART6 Alternate Function mapping */
#define GPIO_AF8_UART7         ((uint8_t)0x08)  /* UART7 Alternate Function mapping  */
#define GPIO_AF8_UART8         ((uint8_t)0x08)  /* UART8 Alternate Function mapping  */
#define GPIO_AF8_SPDIFRX       ((uint8_t)0x08)  /* SPIDIF-RX Alternate Function mapping  */
#define GPIO_AF8_SAI2          ((uint8_t)0x08)  /* SAI2 Alternate Function mapping  */


/**
  * @brief   AF 9 selection
  */
#define GPIO_AF9_CAN1          ((uint8_t)0x09)  /* CAN1 Alternate Function mapping    */
#define GPIO_AF9_CAN2          ((uint8_t)0x09)  /* CAN2 Alternate Function mapping    */
#define GPIO_AF9_TIM12         ((uint8_t)0x09)  /* TIM12 Alternate Function mapping   */
#define GPIO_AF9_TIM13         ((uint8_t)0x09)  /* TIM13 Alternate Function mapping   */
#define GPIO_AF9_TIM14         ((uint8_t)0x09)  /* TIM14 Alternate Function mapping   */
#define GPIO_AF9_QUADSPI       ((uint8_t)0x09)  /* QUADSPI Alternate Function mapping */
#if defined(STM32F756xx) || defined(STM32F746xx)
#define GPIO_AF9_LTDC          ((uint8_t)0x09)  /* LCD-TFT Alternate Function mapping */
#endif /* STM32F756xx || STM32F746xx */
/**
  * @brief   AF 10 selection
  */
#define GPIO_AF10_OTG_FS        ((uint8_t)0xA)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF10_OTG_HS        ((uint8_t)0xA)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF10_QUADSPI       ((uint8_t)0xA)  /* QUADSPI Alternate Function mapping */
#define GPIO_AF10_SAI2          ((uint8_t)0xA)  /* SAI2 Alternate Function mapping */

/**
  * @brief   AF 11 selection
  */
#define GPIO_AF11_ETH           ((uint8_t)0x0B)  /* ETHERNET Alternate Function mapping */

/**
  * @brief   AF 12 selection
  */
#define GPIO_AF12_FMC           ((uint8_t)0xC)  /* FMC Alternate Function mapping                      */
#define GPIO_AF12_OTG_HS_FS     ((uint8_t)0xC)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF12_SDMMC1        ((uint8_t)0xC)  /* SDMMC1 Alternate Function mapping                     */

/**
  * @brief   AF 13 selection
  */
#define GPIO_AF13_DCMI          ((uint8_t)0x0D)  /* DCMI Alternate Function mapping */

#if defined(STM32F756xx) || defined(STM32F746xx)
/**
  * @brief   AF 14 selection
  */
#define GPIO_AF14_LTDC          ((uint8_t)0x0E)  /* LCD-TFT Alternate Function mapping */
#endif /* STM32F756xx || STM32F746xx */
/**
  * @brief   AF 15 selection
  */
#define GPIO_AF15_EVENTOUT      ((uint8_t)0x0F)  /* EVENTOUT Alternate Function mapping */


/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIOEx_Exported_Macros GPIO Exported Macros
  * @{
  */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPIOEx_Exported_Functions GPIO Exported Functions
  * @{
  */
/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup GPIOEx_Private_Constants GPIO Private Constants
  * @{
  */

/**
  * @brief   GPIO pin available on the platform
  */
/* Defines the available pins per GPIOs */
#define GPIOA_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOB_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOC_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOD_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOE_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOF_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOG_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOI_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOJ_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOH_PIN_AVAILABLE  GPIO_PIN_All
#define GPIOK_PIN_AVAILABLE  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | \
                              GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup GPIOEx_Private_Macros GPIO Private Macros
  * @{
  */
/** @defgroup GPIOEx_Get_Port_Index GPIO Get Port Index
  * @{
  */
#define GPIO_GET_INDEX(__GPIOx__)   (uint8_t)(((__GPIOx__) == (GPIOA))? 0U :\
                                              ((__GPIOx__) == (GPIOB))? 1U :\
                                              ((__GPIOx__) == (GPIOC))? 2U :\
                                              ((__GPIOx__) == (GPIOD))? 3U :\
                                              ((__GPIOx__) == (GPIOE))? 4U :\
                                              ((__GPIOx__) == (GPIOF))? 5U :\
                                              ((__GPIOx__) == (GPIOG))? 6U :\
                                              ((__GPIOx__) == (GPIOH))? 7U :\
                                              ((__GPIOx__) == (GPIOI))? 8U :\
                                              ((__GPIOx__) == (GPIOJ))? 9U : 10U)
/**
  * @}
  */

#define IS_GPIO_PIN_AVAILABLE(__INSTANCE__,__PIN__)  \
           ((((__INSTANCE__) == GPIOA) && (((__PIN__) & (GPIOA_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOA_PIN_AVAILABLE)) == (GPIOA_PIN_AVAILABLE))) || \
            (((__INSTANCE__) == GPIOB) && (((__PIN__) & (GPIOB_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOB_PIN_AVAILABLE)) == (GPIOB_PIN_AVAILABLE))) || \
            (((__INSTANCE__) == GPIOC) && (((__PIN__) & (GPIOC_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOC_PIN_AVAILABLE)) == (GPIOC_PIN_AVAILABLE))) || \
            (((__INSTANCE__) == GPIOD) && (((__PIN__) & (GPIOD_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOD_PIN_AVAILABLE)) == (GPIOD_PIN_AVAILABLE))) || \
            (((__INSTANCE__) == GPIOE) && (((__PIN__) & (GPIOE_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOE_PIN_AVAILABLE)) == (GPIOE_PIN_AVAILABLE))) || \
            (((__INSTANCE__) == GPIOF) && (((__PIN__) & (GPIOF_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOF_PIN_AVAILABLE)) == (GPIOF_PIN_AVAILABLE))) || \
			(((__INSTANCE__) == GPIOG) && (((__PIN__) & (GPIOG_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOG_PIN_AVAILABLE)) == (GPIOG_PIN_AVAILABLE))) || \
			(((__INSTANCE__) == GPIOI) && (((__PIN__) & (GPIOI_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOI_PIN_AVAILABLE)) == (GPIOI_PIN_AVAILABLE))) || \
			(((__INSTANCE__) == GPIOJ) && (((__PIN__) & (GPIOJ_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOJ_PIN_AVAILABLE)) == (GPIOJ_PIN_AVAILABLE))) || \
			(((__INSTANCE__) == GPIOK) && (((__PIN__) & (GPIOK_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOK_PIN_AVAILABLE)) == (GPIOK_PIN_AVAILABLE))) || \
			(((__INSTANCE__) == GPIOH) && (((__PIN__) & (GPIOH_PIN_AVAILABLE)) != 0) && (((__PIN__) | (GPIOH_PIN_AVAILABLE)) == (GPIOH_PIN_AVAILABLE))))
/** @defgroup GPIOEx_IS_Alternat_function_selection GPIO Check Alternate Function
  * @{
  */
#if defined(STM32F756xx) || defined(STM32F746xx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF0_RTC_50Hz)   || ((AF) == GPIO_AF1_TIM1)        || \
                          ((AF) == GPIO_AF0_SWJ)        || ((AF) == GPIO_AF0_TRACE)      || \
                          ((AF) == GPIO_AF0_MCO)       || ((AF) == GPIO_AF1_TIM2)       || \
                          ((AF) == GPIO_AF2_TIM3)       || ((AF) == GPIO_AF2_TIM4)       || \
                          ((AF) == GPIO_AF2_TIM5)       || ((AF) == GPIO_AF3_TIM8)       || \
                          ((AF) == GPIO_AF3_TIM9)       || ((AF) == GPIO_AF3_TIM10)      || \
                          ((AF) == GPIO_AF3_TIM11)      || ((AF) == GPIO_AF3_LPTIM1)     || \
                          ((AF) == GPIO_AF3_CEC)        || ((AF) == GPIO_AF4_CEC)        || \
                          ((AF) == GPIO_AF4_I2C1)       || ((AF) == GPIO_AF4_I2C2)       || \
                          ((AF) == GPIO_AF4_I2C3)       || ((AF) == GPIO_AF4_I2C4)       || \
                          ((AF) == GPIO_AF5_SPI1)       || ((AF) == GPIO_AF5_SPI2)       || \
                          ((AF) == GPIO_AF5_SPI3)       || ((AF) == GPIO_AF5_SPI4)       || \
                          ((AF) == GPIO_AF5_SPI5)       || ((AF) == GPIO_AF5_SPI6)       || \
                          ((AF) == GPIO_AF6_SPI3)       || ((AF) == GPIO_AF6_SAI1)       || \
                          ((AF) == GPIO_AF7_SPI3)       || ((AF) == GPIO_AF7_SPI2)        || \
                          ((AF) == GPIO_AF7_USART1)     || ((AF) == GPIO_AF7_USART2)      || \
                          ((AF) == GPIO_AF7_USART3)     || ((AF) == GPIO_AF7_UART5)       || \
                          ((AF) == GPIO_AF7_SPDIFRX)    || ((AF) == GPIO_AF8_SPDIFRX)     || \
                          ((AF) == GPIO_AF8_SAI2)       || ((AF) == GPIO_AF8_USART6)      || \
                          ((AF) == GPIO_AF8_UART4)      || ((AF) == GPIO_AF8_UART5)       || \
                          ((AF) == GPIO_AF8_UART7)      || ((AF) == GPIO_AF8_UART8)       || \
                          ((AF) == GPIO_AF9_CAN1)       || ((AF) == GPIO_AF9_CAN2)        || \
                          ((AF) == GPIO_AF9_TIM12)      || ((AF) == GPIO_AF9_TIM12)      || \
                          ((AF) == GPIO_AF9_TIM14)      || ((AF) == GPIO_AF9_QUADSPI)    || \
                          ((AF) == GPIO_AF9_LTDC)       || ((AF) == GPIO_AF10_OTG_FS)    || \
                          ((AF) == GPIO_AF10_OTG_HS)    || ((AF) == GPIO_AF10_SAI2)      || \
                          ((AF) == GPIO_AF10_QUADSPI)   || ((AF) == GPIO_AF11_ETH)       || \
                          ((AF) == GPIO_AF12_OTG_HS_FS) || ((AF) == GPIO_AF12_SDMMC1)     || \
                          ((AF) == GPIO_AF12_FMC)       || ((AF) == GPIO_AF15_EVENTOUT)  || \
                          ((AF) == GPIO_AF13_DCMI)      || ((AF) == GPIO_AF14_LTDC))
#elif defined(STM32F745xx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF0_RTC_50Hz)   || ((AF) == GPIO_AF1_TIM1)        || \
                          ((AF) == GPIO_AF0_SWJ)        || ((AF) == GPIO_AF0_TRACE)      || \
                          ((AF) == GPIO_AF0_MCO)       || ((AF) == GPIO_AF1_TIM2)       || \
                          ((AF) == GPIO_AF2_TIM3)       || ((AF) == GPIO_AF2_TIM4)       || \
                          ((AF) == GPIO_AF2_TIM5)       || ((AF) == GPIO_AF3_TIM8)       || \
                          ((AF) == GPIO_AF3_TIM9)       || ((AF) == GPIO_AF3_TIM10)      || \
                          ((AF) == GPIO_AF3_TIM11)      || ((AF) == GPIO_AF3_LPTIM1)     || \
                          ((AF) == GPIO_AF3_CEC)        || ((AF) == GPIO_AF4_CEC)        || \
                          ((AF) == GPIO_AF4_I2C1)       || ((AF) == GPIO_AF4_I2C2)       || \
                          ((AF) == GPIO_AF4_I2C3)       || ((AF) == GPIO_AF4_I2C4)       || \
                          ((AF) == GPIO_AF5_SPI1)       || ((AF) == GPIO_AF5_SPI2)       || \
                          ((AF) == GPIO_AF5_SPI3)       || ((AF) == GPIO_AF5_SPI4)       || \
                          ((AF) == GPIO_AF5_SPI5)       || ((AF) == GPIO_AF5_SPI6)       || \
                          ((AF) == GPIO_AF6_SPI3)       || ((AF) == GPIO_AF6_SAI1)       || \
                          ((AF) == GPIO_AF7_SPI3)       || ((AF) == GPIO_AF7_SPI2)        || \
                          ((AF) == GPIO_AF7_USART1)     || ((AF) == GPIO_AF7_USART2)      || \
                          ((AF) == GPIO_AF7_USART3)     || ((AF) == GPIO_AF7_UART5)       || \
                          ((AF) == GPIO_AF7_SPDIFRX)    || ((AF) == GPIO_AF8_SPDIFRX)     || \
                          ((AF) == GPIO_AF8_SAI2)       || ((AF) == GPIO_AF8_USART6)      || \
                          ((AF) == GPIO_AF8_UART4)      || ((AF) == GPIO_AF8_UART5)       || \
                          ((AF) == GPIO_AF8_UART7)      || ((AF) == GPIO_AF8_UART8)       || \
                          ((AF) == GPIO_AF9_CAN1)       || ((AF) == GPIO_AF9_CAN2)        || \
                          ((AF) == GPIO_AF9_TIM12)      || ((AF) == GPIO_AF9_TIM12)      || \
                          ((AF) == GPIO_AF9_TIM14)      || ((AF) == GPIO_AF9_QUADSPI)    || \
                          ((AF) == GPIO_AF13_DCMI)      || ((AF) == GPIO_AF10_OTG_FS)    || \
                          ((AF) == GPIO_AF10_OTG_HS)    || ((AF) == GPIO_AF10_SAI2)      || \
                          ((AF) == GPIO_AF10_QUADSPI)   || ((AF) == GPIO_AF11_ETH)       || \
                          ((AF) == GPIO_AF12_OTG_HS_FS) || ((AF) == GPIO_AF12_SDMMC1)     || \
                          ((AF) == GPIO_AF12_FMC)       || ((AF) == GPIO_AF15_EVENTOUT))
#endif /* STM32F756xx || STM32F746xx */
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup GPIOEx_Private_Functions GPIO Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_HAL_GPIO_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
