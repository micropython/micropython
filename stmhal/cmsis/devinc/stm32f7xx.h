/**
  ******************************************************************************
  * @file    stm32f7xx.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   CMSIS STM32F7xx Device Peripheral Access Layer Header File.
  *
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c. This file contains:
  *           - Configuration section that allows to select:
  *              - The STM32F7xx device used in the target application
  *              - To use or not the peripheral's drivers in application code(i.e.
  *                code will be based on direct access to peripheral's registers
  *                rather than drivers API), this option is controlled by
  *                "#define USE_HAL_DRIVER"
  *
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

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f7xx
  * @{
  */

#ifndef __STM32F7xx_H
#define __STM32F7xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/** @addtogroup Library_configuration_section
  * @{
  */

/**
  * @brief STM32 Family
  */
#if !defined  (STM32F7)
#define STM32F7
#endif /* STM32F7 */

/* Uncomment the line below according to the target STM32 device used in your
   application
  */
#if !defined (STM32F756xx) && !defined (STM32F746xx) && !defined (STM32F745xx)
  /* #define STM32F756xx */   /*!< STM32F756VG, STM32F756ZG, STM32F756ZG, STM32F756IG, STM32F756BG,
                                   STM32F756NG Devices */
  /* #define STM32F746xx */   /*!< STM32F746VE, STM32F746VG, STM32F746ZE, STM32F746ZG, STM32F746IE, STM32F746IG,
                                   STM32F746BE, STM32F746BG, STM32F746NE, STM32F746NG Devices */
  /* #define STM32F745xx */   /*!< STM32F745VE, STM32F745VG, STM32F745ZG, STM32F745ZE, STM32F745IE, STM32F745IG Devices */
#endif

/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.
  */

#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will
   be based on direct access to peripherals registers
   */
  /*#define USE_HAL_DRIVER */
#endif /* USE_HAL_DRIVER */

/**
  * @brief CMSIS Device version number V1.0.1
  */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_SUB2   (0x01) /*!< [15:8]  sub2 version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32F7xx_CMSIS_DEVICE_VERSION        ((__STM32F7xx_CMSIS_DEVICE_VERSION_MAIN << 24)\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION_SUB1 << 16)\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION_SUB2 << 8 )\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION))
/**
  * @}
  */

/** @addtogroup Device_Included
  * @{
  */
#if defined(STM32F756xx)
  #include "stm32f756xx.h"
#elif defined(STM32F746xx)
  #include "stm32f746xx.h"
#elif defined(STM32F745xx)
  #include "stm32f745xx.h"
#else
 #error "Please select first the target STM32F7xx device used in your application (in stm32f7xx.h file)"
#endif

/**
  * @}
  */

/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */

/** @addtogroup Exported_macro
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

/**
  * @}
  */

#ifdef USE_HAL_DRIVER
  #include "stm32f7xx_hal_conf.h"
#endif /* USE_HAL_DRIVER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F7xx_H */

/**
  * @}
  */

  /**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
