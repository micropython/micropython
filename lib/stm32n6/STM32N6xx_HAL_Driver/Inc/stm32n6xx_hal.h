/**
  ******************************************************************************
  * @file    stm32n6xx_hal.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the HAL
  *          module driver.
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
#ifndef __STM32N6xx_HAL_H
#define __STM32N6xx_HAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_conf.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup HAL_Exported_Types HAL Exported Types
  * @{
  */

/** @defgroup HAL_TICK_FREQ Tick Frequency
  * @{
  */
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @defgroup HAL_Exported_Variables HAL Exported Variables
  * @{
  */
extern __IO uint32_t            uwTick;
extern uint32_t                 uwTickPrio;
extern HAL_TickFreqTypeDef      uwTickFreq;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup HAL_Exported_Constants HAL Exported Constants
  * @{
  */

/**
  * @brief STM32N6xx HAL Driver version number
  */
#define __STM32N6xx_HAL_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define __STM32N6xx_HAL_VERSION_SUB1   (0x01U) /*!< [23:16] sub1 version */
#define __STM32N6xx_HAL_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define __STM32N6xx_HAL_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define __STM32N6xx_HAL_VERSION         ((__STM32N6xx_HAL_VERSION_MAIN << 24U)  \
                                         |(__STM32N6xx_HAL_VERSION_SUB1 << 16U) \
                                         |(__STM32N6xx_HAL_VERSION_SUB2 << 8U ) \
                                         |(__STM32N6xx_HAL_VERSION_RC))

/**
  * @}
  */

/** @defgroup SYSCFG_Exported_Constants SYSCFG Exported Constants
  * @{
  */

/** @defgroup SYSCFG_BootId Boot Id
  * @{
  */
#define SYSCFG_BOOT_0              SYSCFG_BOOTCR_BOOT0_PD  /*!< Boot 0 selection */
#define SYSCFG_BOOT_1              SYSCFG_BOOTCR_BOOT1_PD  /*!< Boot 1 selection */
/**
  * @}
  */

/** @defgroup SYSCFG_SDMMCId SDMMC Id
  * @{
  */
#define SYSCFG_SDMMC1              SYSCFG_ICNEWRCR_SDMMC1_EARLY_WR_RSP_ENABLE  /*!< SDMMC 1 selection */
#define SYSCFG_SDMMC2              SYSCFG_ICNEWRCR_SDMMC2_EARLY_WR_RSP_ENABLE  /*!< SDMMC 2 selection */
#define SYSCFG_SDMMC_ALL           (SYSCFG_SDMMC1 | SYSCFG_SDMMC2)             /*!< SDMMC 1 & 2 selection */
/**
  * @}
  */

/** @defgroup  SYSCFG_USBId USB Id
  * @{
  */
#define SYSCFG_USB1                SYSCFG_ICNEWRCR_USB1_EARLY_WR_RSP_ENABLE    /*!< USB 1 selection */
#define SYSCFG_USB2                SYSCFG_ICNEWRCR_USB2_EARLY_WR_RSP_ENABLE    /*!< USB 2 selection */
#define SYSCFG_USB_ALL             (SYSCFG_USB1 | SYSCFG_USB2)                 /*!< USB 1 & 2 selection */
/**
  * @}
  */

/** @defgroup SYSCFG_XPUId XPU Id
  * @{
  */
#if defined(SYSCFG_ICNCGCR_NPU_NIC_CG_DISABLE)
#define SYSCFG_NPU_NIC             SYSCFG_ICNCGCR_NPU_NIC_CG_DISABLE           /*!< NPU_NIC clock gating disable selection */
#endif /* defined(SYSCFG_ICNCGCR_NPU_NOCSYSCFG_ICNCGCR_NPU_NIC_CG_DISABLE_CG_DISABLE) */
#define SYSCFG_CPU_NIC             SYSCFG_ICNCGCR_CPU_NIC_CG_DISABLE           /*!< CPU_NIC clock gating disable selection */
#define SYSCFG_CPU_NOC             SYSCFG_ICNCGCR_CPU_NOC_CG_DISABLE           /*!< CPU_NOC clock gating disable selection */
#if defined(SYSCFG_ICNCGCR_NPU_NIC_CG_DISABLE)
#define SYSCFG_CPU_ALL             (SYSCFG_NPU_NIC | SYSCFG_CPU_NIC | SYSCFG_CPU_NOC)
#else
#define SYSCFG_CPU_ALL             (SYSCFG_CPU_NIC | SYSCFG_CPU_NOC)
#endif /* defined(SYSCFG_ICNCGCR_NPU_NIC_CG_DISABLE) */
/**
  * @}
  */

/** @defgroup SYSCFG_Compensation_Cell_Selection Compensation Cell Selection
  * @{
  */
#define SYSCFG_IO_VDDIO2_CELL       0U         /*!< I/O Compensation cell for VDDIO2 */
#define SYSCFG_IO_VDDIO3_CELL       1U         /*!< I/O Compensation cell for VDDIO3 */
#define SYSCFG_IO_VDDIO4_CELL       2U         /*!< I/O Compensation cell for VDDIO4 */
#define SYSCFG_IO_VDDIO5_CELL       3U         /*!< I/O Compensation cell for VDDIO5 */
/**
  * @}
  */

/** @defgroup SYSCFG_IO_Compensation_Code_Config IO Compensation Code config
  * @{
  */
#define SYSCFG_IO_CELL_CODE               0UL                       /*!< Code from the cell */
#define SYSCFG_IO_REGISTER_CODE           1UL                       /*!< Code from the values in the cell code register */
/**
  * @}
  */

/** @defgroup SYSCFG_DELAY_Feedback_Clock       Delay on feedback clock
  * @{
  */
#define SYSCFG_DELAY_FEEDBACK_NONE              0U                                /*!< No delay on the feedback clock */
#define SYSCFG_DELAY_FEEDBACK_HALF_CYCLE        SYSCFG_FMC_RETIMECR_SDFBCLK_180   /*!< Half a cycle delay on the feedback clock */
/**
  * @}
  */


/** @defgroup SYSCFG_FPU_Interrupts FPU Interrupts
  * @{
  */
#define SYSCFG_IT_FPU_IOC              SYSCFG_CM55CR_FPU_IT_EN_0  /*!< Floating Point Unit Invalid operation Interrupt */
#define SYSCFG_IT_FPU_DZC              SYSCFG_CM55CR_FPU_IT_EN_1  /*!< Floating Point Unit Divide-by-zero Interrupt */
#define SYSCFG_IT_FPU_UFC              SYSCFG_CM55CR_FPU_IT_EN_2  /*!< Floating Point Unit Underflow Interrupt */
#define SYSCFG_IT_FPU_OFC              SYSCFG_CM55CR_FPU_IT_EN_3  /*!< Floating Point Unit Overflow Interrupt */
#define SYSCFG_IT_FPU_IDC              SYSCFG_CM55CR_FPU_IT_EN_4  /*!< Floating Point Unit Input denormal Interrupt */
#define SYSCFG_IT_FPU_IXC              SYSCFG_CM55CR_FPU_IT_EN_5  /*!< Floating Point Unit Inexact Interrupt */
#define SYSCFG_IT_FPU_ALL              (SYSCFG_CM55CR_FPU_IT_EN_0 |\
                                        SYSCFG_CM55CR_FPU_IT_EN_1 |\
                                        SYSCFG_CM55CR_FPU_IT_EN_2 |\
                                        SYSCFG_CM55CR_FPU_IT_EN_3 |\
                                        SYSCFG_CM55CR_FPU_IT_EN_4 |\
                                        SYSCFG_CM55CR_FPU_IT_EN_5)

/**
  * @}
  */


/** @defgroup SYSCFG_Lock_items SYSCFG Lock items
  * @brief SYSCFG items to set lock on
  * @{
  */
#define SYSCFG_MPU_NSEC                SYSCFG_CM55CR_LOCKNSMPU            /*!< Non-secure MPU lock (privileged secure or non-secure only) */
#define SYSCFG_VTOR_NSEC               SYSCFG_CM55CR_LOCKNSVTOR           /*!< Non-secure VTOR lock (privileged secure or non-secure only) */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define SYSCFG_DCAIC                   (SYSCFG_CM55CR_LOCKDCAIC)          /*!< Disable access to the instruction cache direct cache access registers DCAICLR
and DCAICRR */
#define SYSCFG_SAU                     (SYSCFG_CM55CR_LOCKSAU)            /*!< SAU lock (privileged secure code only) */
#define SYSCFG_MPU_SEC                 (SYSCFG_CM55CR_LOCKSMPU)           /*!< Secure MPU lock (privileged secure code only) */
#define SYSCFG_VTOR_AIRCR_SEC          (SYSCFG_CM55CR_LOCKSVTAIRCR)       /*!< VTOR_S and AIRCR lock (privileged secure code only) */
#define SYSCFG_LOCK_ALL                (SYSCFG_MPU_NSEC | SYSCFG_VTOR_NSEC | SYSCFG_DCAIC | SYSCFG_SAU | SYSCFG_MPU_SEC | SYSCFG_VTOR_AIRCR_SEC)  /*!< All */
#else
#define SYSCFG_LOCK_ALL                (SYSCFG_MPU_NSEC | SYSCFG_VTOR_NSEC)  /*!< All (privileged secure or non-secure only) */
#endif /* __ARM_FEATURE_CMSE */
/**
  * @}
  */

/** @defgroup SYSCFG_DTCM_size SYSCFG DTCM size
  * @brief DTCM size to be configured
  * @{
  */
#define SYSCFG_DTCM_128K (SYSCFG_CM55TCMCR_CFGDTCMSZ_3)                                /*!< DTCM memory size is 128K */
#define SYSCFG_DTCM_256K (SYSCFG_CM55TCMCR_CFGDTCMSZ_0 | SYSCFG_CM55TCMCR_CFGDTCMSZ_3) /*!< DTCM memory size is 256K */

/**
  * @}
  */

/** @defgroup SYSCFG_ITCM_size SYSCFG ITCM size
  * @brief ITCM size to be configured
  * @{
  */
#define SYSCFG_ITCM_64K  (SYSCFG_CM55TCMCR_CFGITCMSZ_0 | SYSCFG_CM55TCMCR_CFGITCMSZ_1 | SYSCFG_CM55TCMCR_CFGITCMSZ_2) /*!< ITCM memory size is 64K  */
#define SYSCFG_ITCM_128K (SYSCFG_CM55TCMCR_CFGITCMSZ_3)                                /*!< ITCM memory size is 128K */
#define SYSCFG_ITCM_256K (SYSCFG_CM55TCMCR_CFGITCMSZ_0 | SYSCFG_CM55TCMCR_CFGITCMSZ_3) /*!< ITCM memory size is 256K */

/**
  * @}
  */

/** @defgroup SYSCFG_WRITE_access  WRITE access selection
  * @brief WRITE assess to be configured
  * @{
  */
#define SYSCFG_LOCK_WR_TCM  (SYSCFG_CM55TCMCR_LOCKTCM)  /*!< ITCM memory size is 64K  */
#define SYSCFG_LOCK_WR_ITGU (SYSCFG_CM55TCMCR_LOCKITGU) /*!< Disable writes to registers associated with the ITCM interface security/gating. */
#define SYSCFG_LOCK_WR_DTGU (SYSCFG_CM55TCMCR_LOCKDTGU) /*!< Disable writes to registers associated with the DTCM interface security/gating */
#define SYSCFG_LOCK_WR_ALL  (SYSCFG_CM55TCMCR_LOCKTCM | SYSCFG_CM55TCMCR_LOCKITGU | SYSCFG_CM55TCMCR_LOCKDTGU)  /*!< All writes accesses */
/**
  * @}
  */
/** @defgroup SYSCFG_CACHE_biasing Cache biasing level adjust input selection
  * @brief CACHE biasing level adjust input selection
  * @{
  */
#define SYSCFG_CACHE_BIAS_VNOM            SYSCFG_CM55RWMCR_BC1_CACHE
#define SYSCFG_CACHE_BIAS_VNOM_10_PERCENT SYSCFG_CM55RWMCR_BC2_CACHE
/**
  * @}
  */

/** @defgroup SYSCFG_TCM_biasing biasing level selection
  * @brief TCM biasing level adjust input selection
  * @{
  */
#define SYSCFG_TCM_BIAS_VNOM            SYSCFG_CM55RWMCR_BC1_TCM
#define SYSCFG_TCM_BIAS_VNOM_10_PERCENT SYSCFG_CM55RWMCR_BC2_TCM
/**
  * @}
  */

/** @defgroup SBS_Timer_Break_Inputs Timer Break Inputs
  * @{
  */
#define SYSCFG_CBR_BREAK_LOCK_CORE        SYSCFG_CBR_CM55L         /*!< Cortex-CM55 lockup break lock */
#define SYSCFG_CBR_BREAK_LOCK_PVD         SYSCFG_CBR_PVDL_LOCK     /*!< PVD lock */
#define SYSCFG_CBR_BREAK_LOCK_BKPRAM      SYSCFG_CBR_BKPRAML       /*!< Backup SRAM ECC error break lock */
#define SYSCFG_CBR_BREAK_LOCK_CM55_CACHE  SYSCFG_CBR_CM55CACHEL    /*!< CM55 cache double ECC error lock */
#define SYSCFG_CBR_BREAK_LOCK_CM55_TCM    SYSCFG_CBR_CM55TCML      /*!< CM55 TCM double ECC error lock */
#define SYSCFG_CBR_BREAK_LOCK_ALL         (SYSCFG_CBR_CM55L      |\
                                           SYSCFG_CBR_PVDL_LOCK  |\
                                           SYSCFG_CBR_BKPRAML    |\
                                           SYSCFG_CBR_CM55CACHEL |\
                                           SYSCFG_CBR_CM55TCML)

/**
  * @}
  */

/**
  * @}
  */



/* Exported macros -----------------------------------------------------------*/
/** @defgroup DBGMCU_Exported_Macros DBGMCU Exported Macros
  * @{
  */

/** @brief  Freeze/Unfreeze Peripherals in Debug mode
  */
#if defined(DBGMCU_APB1LFZ1_DBG_TIM2_STOP)
#define __HAL_DBGMCU_FREEZE_TIM2()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM2_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM2()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM2_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM2_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM3_STOP)
#define __HAL_DBGMCU_FREEZE_TIM3()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM3_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM3()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM3_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM3_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM4_STOP)
#define __HAL_DBGMCU_FREEZE_TIM4()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM4_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM4()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM4_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM4_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM5_STOP)
#define __HAL_DBGMCU_FREEZE_TIM5()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM5_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM5()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM5_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM5_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM6_STOP)
#define __HAL_DBGMCU_FREEZE_TIM6()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM6_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM6()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM6_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM6_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM7_STOP)
#define __HAL_DBGMCU_FREEZE_TIM7()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM7_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM7()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM7_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM7_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM10_STOP)
#define __HAL_DBGMCU_FREEZE_TIM10()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM10_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM10()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM10_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM10_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM11_STOP)
#define __HAL_DBGMCU_FREEZE_TIM11()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM11_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM11()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM11_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM11_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM12_STOP)
#define __HAL_DBGMCU_FREEZE_TIM12()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM12_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM12()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM12_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM12_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM13_STOP)
#define __HAL_DBGMCU_FREEZE_TIM13()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM13_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM13()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM13_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM13_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_TIM14_STOP)
#define __HAL_DBGMCU_FREEZE_TIM14()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM14_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM14()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_TIM14_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_TIM14_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_LPTIM1_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM1()            SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_LPTIM1_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM1()          CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_LPTIM1_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_LPTIM1_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_WWDG1_STOP)
#define __HAL_DBGMCU_FREEZE_WWDG()             SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_WWDG1_STOP)
#define __HAL_DBGMCU_UNFREEZE_WWDG()           CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_WWDG1_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_WWDG1_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_I2C1_STOP)
#define __HAL_DBGMCU_FREEZE_I2C1()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C1_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C1()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C1_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_I2C1_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_I2C2_STOP)
#define __HAL_DBGMCU_FREEZE_I2C2()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C2_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C2()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C2_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_I2C2_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_I2C3_STOP)
#define __HAL_DBGMCU_FREEZE_I2C3()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C3_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C3()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I2C3_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_I2C3_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_I3C1_STOP)
#define __HAL_DBGMCU_FREEZE_I3C1()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I3C1_STOP)
#define __HAL_DBGMCU_UNFREEZE_I3C1()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I3C1_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_I3C1_STOP) */

#if defined(DBGMCU_APB1LFZ1_DBG_I3C2_STOP)
#define __HAL_DBGMCU_FREEZE_I3C2()              SET_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I3C2_STOP)
#define __HAL_DBGMCU_UNFREEZE_I3C2()            CLEAR_BIT(DBGMCU->APB1LFZ1, DBGMCU_APB1LFZ1_DBG_I3C2_STOP)
#endif /* defined(DBGMCU_APB1LFZ1_DBG_I3C2_STOP) */

#if defined(DBGMCU_APB1HFZ1_DBG_FDCAN_STOP)
#define __HAL_DBGMCU_FREEZE_FDCAN()             SET_BIT(DBGMCU->APB1HFZ1, DBGMCU_APB1HFZ1_DBG_FDCAN_STOP)
#define __HAL_DBGMCU_UNFREEZE_FDCAN()           CLEAR_BIT(DBGMCU->APB1HFZ1, DBGMCU_APB1HFZ1_DBG_FDCAN_STOP)
#endif /* defined(DBGMCU_APB1HFZ1_DBG_FDCAN_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM1_STOP)
#define __HAL_DBGMCU_FREEZE_TIM1()              SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM1_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM1()            CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM1_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM1_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM8_STOP)
#define __HAL_DBGMCU_FREEZE_TIM8()              SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM8_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM8()            CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM8_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM8_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM18_STOP)
#define __HAL_DBGMCU_FREEZE_TIM18()             SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM18_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM18()           CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM18_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM18_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM15_STOP)
#define __HAL_DBGMCU_FREEZE_TIM15()             SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM15_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM15()           CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM15_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM15_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM16_STOP)
#define __HAL_DBGMCU_FREEZE_TIM16()             SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM16_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM16()           CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM16_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM16_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM17_STOP)
#define __HAL_DBGMCU_FREEZE_TIM17()             SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM17_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM17()           CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM17_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM17_STOP) */

#if defined(DBGMCU_APB2FZ1_DBG_TIM9_STOP)
#define __HAL_DBGMCU_FREEZE_TIM9()              SET_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM9_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM9()            CLEAR_BIT(DBGMCU->APB2FZ1, DBGMCU_APB2FZ1_DBG_TIM9_STOP)
#endif /* defined(DBGMCU_APB2FZ1_DBG_TIM9_STOP) */

#if defined(DBGMCU_APB4FZ1_DBG_I2C4_STOP)
#define __HAL_DBGMCU_FREEZE_I2C4()              SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_I2C4_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C4()            CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_I2C4_STOP)
#endif /* defined(DBGMCU_APB4FZ1_DBG_I2C4_STOP) */

#if defined(DBGMCU_APB4FZ1_DBG_LPTIM2_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM2()            SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM2_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM2()          CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM2_STOP)
#endif /* defined(DBGMCU_APB4FZ1_DBG_LPTIM2_STOP) */

#if defined(DBGMCU_APB4FZ1_DBG_LPTIM3_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM3()            SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM3_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM3()          CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM3_STOP)
#endif /* DBGMCU_APB4FZ1_DBG_LPTIM3_STOP */

#if defined(DBGMCU_APB4FZ1_DBG_LPTIM4_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM4()            SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM4_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM4()          CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM4_STOP)
#endif /* DBGMCU_APB4FZ1_DBG_LPTIM4_STOP */

#if defined(DBGMCU_APB4FZ1_DBG_LPTIM5_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM5()            SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM5_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM5()          CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_LPTIM5_STOP)
#endif /* DBGMCU_APB4FZ1_DBG_LPTIM5_STOP */

#if defined(DBGMCU_APB4FZ1_DBG_RTC_STOP)
#define __HAL_DBGMCU_FREEZE_RTC()               SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_RTC_STOP)
#define __HAL_DBGMCU_UNFREEZE_RTC()             CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_RTC_STOP)
#endif /* DBGMCU_APB4FZ1_DBG_RTC_STOP */

#if defined(DBGMCU_APB4FZ1_DBG_IWDG_STOP)
#define __HAL_DBGMCU_FREEZE_IWDG()              SET_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_IWDG_STOP)
#define __HAL_DBGMCU_UNFREEZE_IWDG()            CLEAR_BIT(DBGMCU->APB4FZ1, DBGMCU_APB4FZ1_DBG_IWDG_STOP)
#endif /* DBGMCU_APB4FZ1_DBG_IWDG_STOP */

#if defined(DBGMCU_APB5FZ1_DBG_GFXTIM_STOP)
#define __HAL_DBGMCU_FREEZE_GFXTIM()            SET_BIT(DBGMCU->APB5FZ1, DBGMCU_APB5FZ1_DBG_GFXTIM_STOP)
#define __HAL_DBGMCU_UNFREEZE_GFXTIM()          CLEAR_BIT(DBGMCU->APB5FZ1, DBGMCU_APB5FZ1_DBG_GFXTIM_STOP)
#endif /* DBGMCU_APB5FZ1_DBG_GFXTIM_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH0_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH0()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH0_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH0()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH0_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH0_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH1_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH1()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH1_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH1()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH1_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH1_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH2_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH2()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH2_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH2()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH2_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH2_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH3_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH3()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH3_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH3()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH3_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH3_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH4_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH4()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH4_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH4()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH4_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH4_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH5_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH5()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH5_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH5()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH5_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH5_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH6_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH6()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH6_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH6()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH6_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH6_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH7_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH7()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH7_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH7()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH7_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH7_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH8_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH8()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH8_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH8()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH8_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH8_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH9_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH9()        SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH9_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH9()      CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH9_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH9_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH10_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH10()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH10_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH10()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH10_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH10_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH11_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH11()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH11_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH11()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH11_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH11_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH12_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH12()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH12_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH12()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH12_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH12_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH13_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH13()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH13_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH13()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH13_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH13_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH14_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH14()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH14_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH14()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH14_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH14_STOP */

#if defined(DBGMCU_AHB1FZ1_DBG_GPDMA1_CH15_STOP)
#define __HAL_DBGMCU_FREEZE_GPDMA1_CH15()       SET_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH15_STOP)
#define __HAL_DBGMCU_UNFREEZE_GPDMA1_CH15()     CLEAR_BIT(DBGMCU->AHB1FZ1, DBGMCU_AHB1FZ1_DBG_GPDMA1_CH15_STOP)
#endif /* DBGMCU_AHB1FZ1_DBG_GPDMA1_CH15_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH0_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH0()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH0_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH0()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH0_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH0_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH1_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH1()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH1_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH1()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH1_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH1_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH2_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH2()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH2_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH2()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH2_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH2_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH3_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH3()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH3_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH3()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH3_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH3_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH4_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH4()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH4_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH4()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH4_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH4_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH5_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH5()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH5_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH5()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH5_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH5_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH6_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH6()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH6_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH6()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH6_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH6_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH7_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH7()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH7_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH7()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH7_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH7_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH8_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH8()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH8_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH8()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH8_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH8_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH9_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH9()        SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH9_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH9()      CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH9_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH9_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH10_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH10()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH10_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH10()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH10_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH10_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH11_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH11()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH11_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH11()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH11_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH11_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH12_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH12()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH12_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH12()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH12_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH12_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH13_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH13()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH13_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH13()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH13_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH13_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH14_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH14()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH14_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH14()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH14_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH14_STOP */

#if defined(DBGMCU_AHB5FZ1_DBG_HPDMA1_CH15_STOP)
#define __HAL_DBGMCU_FREEZE_HPDMA1_CH15()       SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH15_STOP)
#define __HAL_DBGMCU_UNFREEZE_HPDMA1_CH15()     CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_DBG_HPDMA1_CH15_STOP)
#endif /* DBGMCU_AHB5FZ1_DBG_HPDMA1_CH15_STOP */

#if defined(DBGMCU_AHB5FZ1_NPU_DBG_FREEZE)
#define __HAL_DBGMCU_FREEZE_NPU()               SET_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_NPU_DBG_FREEZE)
#define __HAL_DBGMCU_UNFREEZE_NPU()             CLEAR_BIT(DBGMCU->AHB5FZ1, DBGMCU_AHB5FZ1_NPU_DBG_FREEZE)
#endif /* DBGMCU_AHB5FZ1_NPU_DBG_FREEZE */


/**
  * @}
  */

/** @defgroup SYSCFG_Exported_Macros SYSCFG Exported Macros
  * @{
  */

/** @brief  Floating Point Unit interrupt enable/disable macros
  * @param __INTERRUPT__: This parameter can be a value of @ref SYSCFG_FPU_Interrupts
  */
#define __HAL_SYSCFG_FPU_INTERRUPT_ENABLE(__INTERRUPT__)    do {assert_param(IS_SYSCFG_FPU_INTERRUPT((__INTERRUPT__)));\
                                                                 SET_BIT(SYSCFG->CM55CR, (__INTERRUPT__));\
                                                               }while(0)

#define __HAL_SYSCFG_FPU_INTERRUPT_DISABLE(__INTERRUPT__)   do {assert_param(IS_SYSCFG_FPU_INTERRUPT((__INTERRUPT__)));\
                                                                 CLEAR_BIT(SYSCFG->CM55CR, (__INTERRUPT__));\
                                                               }while(0)

/* Private macros ------------------------------------------------------------*/

/** @defgroup SYSCFG_Private_Macros SYSCFG Private Macros
  * @{
  */
#define IS_SYSCFG_BOOT_ID(__BOOTID__)  ((((__BOOTID__) & SYSCFG_BOOT_0) == SYSCFG_BOOT_0) || \
                                        (((__BOOTID__) & SYSCFG_BOOT_1) == SYSCFG_BOOT_1))

#define IS_SYSCFG_FPU_INTERRUPT(__INTERRUPT__) (((((__INTERRUPT__) & SYSCFG_IT_FPU_IOC) == SYSCFG_IT_FPU_IOC)  || \
                                                 (((__INTERRUPT__) & SYSCFG_IT_FPU_DZC) == SYSCFG_IT_FPU_DZC)  || \
                                                 (((__INTERRUPT__) & SYSCFG_IT_FPU_UFC) == SYSCFG_IT_FPU_UFC)  || \
                                                 (((__INTERRUPT__) & SYSCFG_IT_FPU_OFC) == SYSCFG_IT_FPU_OFC)  || \
                                                 (((__INTERRUPT__) & SYSCFG_IT_FPU_IDC) == SYSCFG_IT_FPU_IDC)  || \
                                                 (((__INTERRUPT__) & SYSCFG_IT_FPU_IXC) == SYSCFG_IT_FPU_IXC)) && \
                                                (((__INTERRUPT__) & ~(SYSCFG_IT_FPU_ALL)) == 0U))

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define IS_SYSCFG_LOCK_ITEMS(__ITEM__) (((((__ITEM__) & SYSCFG_MPU_NSEC)       == SYSCFG_MPU_NSEC)  || \
                                         (((__ITEM__) & SYSCFG_VTOR_NSEC)      == SYSCFG_VTOR_NSEC) || \
                                         (((__ITEM__) & SYSCFG_DCAIC)          == SYSCFG_DCAIC)     || \
                                         (((__ITEM__) & SYSCFG_SAU)            == SYSCFG_SAU)       || \
                                         (((__ITEM__) & SYSCFG_MPU_SEC)        == SYSCFG_MPU_SEC)   || \
                                         (((__ITEM__) & SYSCFG_VTOR_AIRCR_SEC) == SYSCFG_VTOR_AIRCR_SEC)) && \
                                        (((__ITEM__) & ~(SYSCFG_LOCK_ALL)) == 0U))
#else
#define IS_SYSCFG_LOCK_ITEMS(__ITEM__) (((((__ITEM__) & SYSCFG_MPU_NSEC)  == SYSCFG_MPU_NSEC)  || \
                                         (((__ITEM__) & SYSCFG_VTOR_NSEC) == SYSCFG_VTOR_NSEC)) && \
                                        (((__ITEM__) & ~(SYSCFG_LOCK_ALL)) == 0U))
#endif /* __ARM_FEATURE_CMSE */

#define IS_SYSCFG_DTCM_SIZE(__SIZE__) ((((__SIZE__) & SYSCFG_CM55TCMCR_CFGDTCMSZ) == SYSCFG_DTCM_128K)  || \
                                       (((__SIZE__) & SYSCFG_CM55TCMCR_CFGDTCMSZ) == SYSCFG_DTCM_256K))

#define IS_SYSCFG_ITCM_SIZE(__SIZE__) ((((__SIZE__) & SYSCFG_CM55TCMCR_CFGITCMSZ) == SYSCFG_ITCM_64K)   || \
                                       (((__SIZE__) & SYSCFG_CM55TCMCR_CFGITCMSZ) == SYSCFG_ITCM_128K)  || \
                                       (((__SIZE__) & SYSCFG_CM55TCMCR_CFGITCMSZ) == SYSCFG_ITCM_256K))

#define IS_SYSCFG_LOCK_WRACCESS(__WRACCESS__) (((((__WRACCESS__) & SYSCFG_LOCK_WR_TCM)   == SYSCFG_LOCK_WR_TCM)  || \
                                                (((__WRACCESS__) & SYSCFG_LOCK_WR_ITGU)  == SYSCFG_LOCK_WR_ITGU) || \
                                                (((__WRACCESS__) & SYSCFG_LOCK_WR_DTGU)  == SYSCFG_LOCK_WR_DTGU)) && \
                                               (((__WRACCESS__) & ~(SYSCFG_LOCK_WR_ALL)) == 0U))

#define IS_TCM_MARGIN_INPUT(__MARGIN__)  ((__MARGIN__) < 16U)

#define IS_SYSCFG_CACHE_BIASING_LEVEL(__LEVEL__) ((((__LEVEL__) & (SYSCFG_CM55RWMCR_BC1_CACHE | SYSCFG_CM55RWMCR_BC2_CACHE)) == SYSCFG_CACHE_BIAS_VNOM)             || \
                                                  (((__LEVEL__) & (SYSCFG_CM55RWMCR_BC1_CACHE | SYSCFG_CM55RWMCR_BC2_CACHE)) == SYSCFG_CACHE_BIAS_VNOM_10_PERCENT))

#define IS_SYSCFG_TCM_BIASING_LEVEL(__LEVEL__) ((((__LEVEL__) & (SYSCFG_CM55RWMCR_BC1_TCM | SYSCFG_CM55RWMCR_BC2_TCM)) == SYSCFG_TCM_BIAS_VNOM)  || \
                                                (((__LEVEL__) & (SYSCFG_CM55RWMCR_BC1_TCM | SYSCFG_CM55RWMCR_BC2_TCM)) == SYSCFG_TCM_BIAS_VNOM_10_PERCENT))

#define IS_VTOR_ADDRESS(__ADDRESS__) (((__ADDRESS__) & 0x7FU) == 0U)

#define IS_SYSCFG_QOS_CPU(__QOS__) ((__QOS__) < 16U)

#define IS_SYSCFG_SDMMC(__SDMMC__) (((((__SDMMC__) & SYSCFG_SDMMC1) == SYSCFG_SDMMC1) || \
                                     (((__SDMMC__) & SYSCFG_SDMMC2) == SYSCFG_SDMMC2)) && \
                                    (((__SDMMC__) & ~(SYSCFG_SDMMC_ALL)) == 0U))

#define IS_SYSCFG_USB(__USB__) (((((__USB__) & SYSCFG_USB1) == SYSCFG_USB1) || \
                                 (((__USB__) & SYSCFG_USB2) == SYSCFG_USB2)) && \
                                (((__USB__) & ~(SYSCFG_USB_ALL)) == 0U))

#if defined(SYSCFG_ICNCGCR_NPU_NOC_CG_DISABLE)
#define IS_SYSCFG_CPU_CLK_GATING(__CPU__) (((((__CPU__) & SYSCFG_NPU_NOC) == SYSCFG_NPU_NOC) || \
                                            (((__CPU__) & SYSCFG_CPU_NIC) == SYSCFG_CPU_NIC) || \
                                            (((__CPU__) & SYSCFG_CPU_NOC) == SYSCFG_CPU_NOC)) && \
                                           (((__CPU__) & ~(SYSCFG_CPU_ALL)) == 0U))
#else
#define IS_SYSCFG_CPU_CLK_GATING(__CPU__) (((((__CPU__) & SYSCFG_CPU_NIC) == SYSCFG_CPU_NIC) ||  \
                                            (((__CPU__) & SYSCFG_CPU_NOC) == SYSCFG_CPU_NOC)) && \
                                           (((__CPU__) & ~(SYSCFG_CPU_ALL)) == 0U))
#endif /* defined(SYSCFG_ICNCGCR_NPU_NOC_CG_DISABLE) */

#define IS_SYSCFG_IO_COMPENSATION_CELL_NMOS_VALUE(__VALUE__) (((__VALUE__) < 16U))
#define IS_SYSCFG_IO_COMPENSATION_CELL_PMOS_VALUE(__VALUE__) (((__VALUE__) < 16U))

#define IS_SYSCFG_COMPENSATION_CELL(__VALUE__) (((__VALUE__) == SYSCFG_IO_VDDIO2_CELL) || \
                                                ((__VALUE__) == SYSCFG_IO_VDDIO3_CELL) || \
                                                ((__VALUE__) == SYSCFG_IO_VDDIO4_CELL) || \
                                                ((__VALUE__) == SYSCFG_IO_VDDIO5_CELL))

#define IS_SYSCFG_IO_COMPENSATION_CODE(__CELL__) (((__CELL__) == SYSCFG_IO_CELL_CODE)      || \
                                                  ((__CELL__) == SYSCFG_IO_REGISTER_CODE))

#define IS_SYSCFG_CBR_BREAK_INPUT(__VALUE__) \
  ((((__VALUE__) & SYSCFG_CBR_BREAK_LOCK_ALL) != 0U) && \
   (((__VALUE__) & ~SYSCFG_CBR_BREAK_LOCK_ALL) == 0U))

#define IS_SYSCFG_DMA_CID_SEC(__CID__) (((__CID__) < 8U))

#define IS_SYSCFG_DMA_CID_NON_SEC(__CID__) (((__CID__) < 8U))

#define IS_SYSCFG_DMA_DELAY_FEEDBACK_CLOCK(__DELAY__) (((__DELAY__) == SYSCFG_DELAY_FEEDBACK_NONE)  || \
                                                       ((__DELAY__) == SYSCFG_DELAY_FEEDBACK_HALF_CYCLE))

/**
  * @}
  */


/** @defgroup HAL_Private_Macros HAL Private Macros
  * @{
  */
#define IS_TICKFREQ(FREQ) (((FREQ) == HAL_TICK_FREQ_10HZ)  || \
                           ((FREQ) == HAL_TICK_FREQ_100HZ) || \
                           ((FREQ) == HAL_TICK_FREQ_1KHZ))
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup HAL_Exported_Functions
  * @{
  */

/** @addtogroup HAL_Exported_Functions_Group1
  * @{
  */

/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);

/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group2
  * @{
  */

/* Peripheral Control functions  ************************************************/
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);

/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group3
  * @{
  */

/* DBGMCU Peripheral Control functions  *****************************************/
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);

/** @addtogroup HAL_Exported_Functions_Group4
  * @{
  */

/* SYSCFG Peripheral Control functions  *****************************************/
void HAL_SYSCFG_EnablePullDown(uint32_t BootId);
void HAL_SYSCFG_DisablePullDown(uint32_t BootId);


void HAL_SYSCFG_Lock(uint32_t Item);
HAL_StatusTypeDef HAL_SYSCFG_GetLock(uint32_t *pItem);

void HAL_SYSCFG_SetTCMSize(uint32_t DtcmSize, uint32_t ItcmSize);
HAL_StatusTypeDef HAL_SYSCFG_GetTCMSize(uint32_t *pDtcmSize, uint32_t *pItcmSize);

void HAL_SYSCFG_LockWriteAccess(uint32_t Item);
HAL_StatusTypeDef HAL_SYSCFG_GetLockWriteAccess(uint32_t *pItem);

void HAL_SYSCFG_EnableITCMWaiteState(void);
void HAL_SYSCFG_DisableITCMWaiteState(void);
void HAL_SYSCFG_EnableDTCMWaiteState(void);
void HAL_SYSCFG_DisableDTCMWaiteState(void);

void HAL_SYSCFG_EnableTCMExternalMargin(void);
void HAL_SYSCFG_DisableTCMExternalMargin(void);

void HAL_SYSCFG_SetTCMRWMarginInput(uint32_t TcmRwMarginInput);
HAL_StatusTypeDef HAL_SYSCFG_GetTCMRWMarginInput(uint32_t *pTcmRwMarginInput);

void HAL_SYSCFG_SetTCMBiasingLevel(uint32_t Level);
HAL_StatusTypeDef HAL_SYSCFG_GetTCMBiasingLevel(uint32_t *pLevel);

void HAL_SYSCFG_EnableCacheExternalMargin(void);
void HAL_SYSCFG_DisableCacheExternalMargin(void);

void HAL_SYSCFG_SetCacheRWMarginInput(uint32_t CacheRWMarginInput);
HAL_StatusTypeDef HAL_SYSCFG_GetCacheRWMarginInput(uint32_t *pCacheRWMarginInput);

void HAL_SYSCFG_SetCacheBiasingLevel(uint32_t Level);
HAL_StatusTypeDef HAL_SYSCFG_GetCacheBiasingLevel(uint32_t *pLevel);


void HAL_SYSCFG_SetSVTORAddress(uint32_t Address);
HAL_StatusTypeDef HAL_SYSCFG_GetSVTORAddress(uint32_t *pAddress);

void HAL_SYSCFG_SetNSVTORAddress(uint32_t Address);
HAL_StatusTypeDef HAL_SYSCFG_GetNSVTORAddress(uint32_t *pAddress);


void HAL_SYSCFG_EnablePowerOnReset(void);
void HAL_SYSCFG_DisablePowerOnReset(void);

void HAL_SYSCFG_EnableLockupWarmResetonRCC(void);
void HAL_SYSCFG_DisableLockupWarmResetonRCC(void);

void HAL_SYSCFG_EnableLockupGenerateNMI(void);
void HAL_SYSCFG_DisableLockupGenerateNMI(void);

HAL_StatusTypeDef HAL_SYSCFG_ReEnableWritePostingErrorCapture(void);

#if defined(VENC)
void HAL_SYSCFG_EnableVENCRAMReserved(void);
void HAL_SYSCFG_DisableVENCRAMReserved(void);
#endif /* VENC */

void HAL_SYSCFG_EnableCRYPPotentialTamper(void);
void HAL_SYSCFG_DisableCRYPPotentialTamper(void);


void HAL_SYSCFG_SetWriteQosNP1(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetWriteQosNP1(uint32_t *pQosValue);
void HAL_SYSCFG_SetReadQosNP1(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosNP1(uint32_t *pQosValue);

void HAL_SYSCFG_SetWriteQosNP2(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetWriteQosNP2(uint32_t *pQosValue);
void HAL_SYSCFG_SetReadQosNP2(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosNP2(uint32_t *pQosValue);

void HAL_SYSCFG_SetWriteQosCPUSS(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetwriteQosCPUSS(uint32_t *pQosValue);
void HAL_SYSCFG_SetReadQosCPUSS(uint32_t QosValue);
HAL_StatusTypeDef HAL_SYSCFG_GetReadQosCPUSS(uint32_t *pQosValue);


void HAL_SYSCFG_EnableSDMMCEarlyWRRESP(uint32_t Sdmmc);
void HAL_SYSCFG_DisableSDMMCEarlyWRRSP(uint32_t Sdmmc);

void HAL_SYSCFG_EnableUSBEarlyEarlyWRRESP(uint32_t Usb);
void HAL_SYSCFG_DisableUSBEarlyEarlyWRRESP(uint32_t Usb);


void HAL_SYSCFG_EnablexPUClockGating(uint32_t Xpu);
void HAL_SYSCFG_DisablexPUClockGating(uint32_t Xpu);




void HAL_SYSCFG_EnableVDDIO2CompensationCell(void);
void HAL_SYSCFG_DisableVDDIO2CompensationCell(void);

void HAL_SYSCFG_EnableVDDIO3CompensationCell(void);
void HAL_SYSCFG_DisableVDDIO3CompensationCell(void);

void HAL_SYSCFG_EnableVDDIO4CompensationCell(void);
void HAL_SYSCFG_DisableVDDIO4CompensationCell(void);

void HAL_SYSCFG_EnableVDDIO5CompensationCell(void);
void HAL_SYSCFG_DisableVDDIO5CompensationCell(void);


HAL_StatusTypeDef HAL_SYSCFG_ConfigVDDIOCompensationCell(uint32_t Selection, uint32_t Code,
                                                         uint32_t NmosValue, uint32_t PmosValue);

HAL_StatusTypeDef HAL_SYSCFG_GetVDDIOCompensationCell(uint32_t Selection, const uint32_t *pCode, uint32_t *pNmosValue,
                                                      uint32_t *pPmosValue);


uint32_t HAL_SYSCFG_GetCompVDDIO2CellReadyStatus(void);
uint32_t HAL_SYSCFG_GetCompVDDIO3CellReadyStatus(void);
uint32_t HAL_SYSCFG_GetCompVDDIO4CellReadyStatus(void);
uint32_t HAL_SYSCFG_GetCompVDDIO5CellReadyStatus(void);

HAL_StatusTypeDef HAL_SYSCFG_ConfigVDDCompensationCell(uint32_t Code, uint32_t NmosValue, uint32_t PmosValue);

void HAL_SYSCFG_EnableVDDCompensationCell(void);
void HAL_SYSCFG_DisableVDDCompensationCell(void);

HAL_StatusTypeDef HAL_SYSCFG_GetVDDCompensationCell(uint32_t Code, uint32_t *pNmosValue,
                                                 uint32_t *pPmosValue);
uint32_t HAL_SYSCFG_GetCompensationVDDCellReadyStatus(void);

void HAL_SYSCFG_ConfigTimerBreakInput(uint32_t Input);
uint32_t HAL_SYSCFG_GetTimerBreakInputConfig(void);

void HAL_SYSCFG_SetPerceivedCID(uint32_t Cid);
uint32_t HAL_SYSCFG_GetPerceivedCID(void);

void HAL_SYSCFG_SetPerceivedPrivCID(uint32_t Cid);
uint32_t HAL_SYSCFG_GetPerceivedPrivCID(void);

void HAL_SYSCFG_EnableReTimingRXpath(void);
void HAL_SYSCFG_DisableReTimingRXpath(void);

void HAL_SYSCFG_EnableReTimingTXpath(void);
void HAL_SYSCFG_DisableReTimingTXpath(void);

void HAL_SYSCFG_SetDelayOnFeedbackClock(uint32_t Delay);
uint32_t HAL_SYSCFG_GetDelayOnFeedbackClock(void);

void HAL_SYSCFG_EnableInterleavingCpuRam(void);
void HAL_SYSCFG_DisableInterleavingCpuRam(void);

uint32_t HAL_SYSCFG_GetBootPinConnection(uint32_t BootId);

uint32_t HAL_SYSCFG_GetAddressWritePostingBuffer(void);

/**
  * @}
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

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32N6xx_HAL_H */
