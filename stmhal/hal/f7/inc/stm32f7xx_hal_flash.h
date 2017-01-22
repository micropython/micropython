/**
  ******************************************************************************
  * @file    stm32f7xx_hal_flash.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of FLASH HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_HAL_FLASH_H
#define __STM32F7xx_HAL_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup FLASH_Exported_Types FLASH Exported Types
  * @{
  */

/**
  * @brief  FLASH Procedure structure definition
  */
typedef enum
{
  FLASH_PROC_NONE = 0U,
  FLASH_PROC_SECTERASE,
  FLASH_PROC_MASSERASE,
  FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;


/**
  * @brief  FLASH handle Structure definition
  */
typedef struct
{
  __IO FLASH_ProcedureTypeDef ProcedureOnGoing;   /* Internal variable to indicate which procedure is ongoing or not in IT context */

  __IO uint32_t               NbSectorsToErase;   /* Internal variable to save the remaining sectors to erase in IT context        */

  __IO uint8_t                VoltageForErase;    /* Internal variable to provide voltage range selected by user in IT context     */

  __IO uint32_t               Sector;             /* Internal variable to define the current sector which is erasing               */

  __IO uint32_t               Address;            /* Internal variable to save address selected for program                        */

  HAL_LockTypeDef             Lock;               /* FLASH locking object                                                          */

  __IO uint32_t               ErrorCode;          /* FLASH error code                                                              */

}FLASH_ProcessTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */

/** @defgroup FLASH_Error_Code FLASH Error Code
  * @brief    FLASH Error Code
  * @{
  */
#define HAL_FLASH_ERROR_NONE         ((uint32_t)0x00000000U)    /*!< No error                      */
#define HAL_FLASH_ERROR_ERS          ((uint32_t)0x00000002U)    /*!< Programming Sequence error    */
#define HAL_FLASH_ERROR_PGP          ((uint32_t)0x00000004U)    /*!< Programming Parallelism error */
#define HAL_FLASH_ERROR_PGA          ((uint32_t)0x00000008U)    /*!< Programming Alignment error   */
#define HAL_FLASH_ERROR_WRP          ((uint32_t)0x00000010U)    /*!< Write protection error        */
#define HAL_FLASH_ERROR_OPERATION    ((uint32_t)0x00000020U)    /*!< Operation Error               */
/**
  * @}
  */

/** @defgroup FLASH_Type_Program FLASH Type Program
  * @{
  */
#define FLASH_TYPEPROGRAM_BYTE        ((uint32_t)0x00U)  /*!< Program byte (8-bit) at a specified address           */
#define FLASH_TYPEPROGRAM_HALFWORD    ((uint32_t)0x01U)  /*!< Program a half-word (16-bit) at a specified address   */
#define FLASH_TYPEPROGRAM_WORD        ((uint32_t)0x02U)  /*!< Program a word (32-bit) at a specified address        */
#define FLASH_TYPEPROGRAM_DOUBLEWORD  ((uint32_t)0x03U)  /*!< Program a double word (64-bit) at a specified address */
/**
  * @}
  */

/** @defgroup FLASH_Flag_definition FLASH Flag definition
  * @brief Flag definition
  * @{
  */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP            /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               FLASH_SR_OPERR          /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR         /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              FLASH_SR_PGPERR         /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_ERSERR              FLASH_SR_ERSERR         /*!< FLASH Erasing Sequence error flag         */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY            /*!< FLASH Busy flag                           */


#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_OPERR   | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                                   FLASH_FLAG_PGPERR  | FLASH_FLAG_ERSERR)
/**
  * @}
  */

/** @defgroup FLASH_Interrupt_definition FLASH Interrupt definition
  * @brief FLASH Interrupt definition
  * @{
  */
#define FLASH_IT_EOP                   FLASH_CR_EOPIE          /*!< End of FLASH Operation Interrupt source */
#define FLASH_IT_ERR                   ((uint32_t)0x02000000U)  /*!< Error Interrupt source                  */
/**
  * @}
  */

/** @defgroup FLASH_Program_Parallelism FLASH Program Parallelism
  * @{
  */
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000U)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)FLASH_CR_PSIZE_0)
#define FLASH_PSIZE_WORD           ((uint32_t)FLASH_CR_PSIZE_1)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)FLASH_CR_PSIZE)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFFU)
/**
  * @}
  */

/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */
#define FLASH_KEY1               ((uint32_t)0x45670123U)
#define FLASH_KEY2               ((uint32_t)0xCDEF89ABU)
#define FLASH_OPT_KEY1           ((uint32_t)0x08192A3BU)
#define FLASH_OPT_KEY2           ((uint32_t)0x4C5D6E7FU)
/**
  * @}
  */

/** @defgroup FLASH_Sectors FLASH Sectors
  * @{
  */
#define FLASH_SECTOR_0           ((uint32_t)0U) /*!< Sector Number 0   */
#define FLASH_SECTOR_1           ((uint32_t)1U) /*!< Sector Number 1   */
#define FLASH_SECTOR_2           ((uint32_t)2U) /*!< Sector Number 2   */
#define FLASH_SECTOR_3           ((uint32_t)3U) /*!< Sector Number 3   */
#define FLASH_SECTOR_4           ((uint32_t)4U) /*!< Sector Number 4   */
#define FLASH_SECTOR_5           ((uint32_t)5U) /*!< Sector Number 5   */
#define FLASH_SECTOR_6           ((uint32_t)6U) /*!< Sector Number 6   */
#define FLASH_SECTOR_7           ((uint32_t)7U) /*!< Sector Number 7   */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup FLASH_Exported_Macros FLASH Exported Macros
  * @{
  */
/**
  * @brief  Set the FLASH Latency.
  * @param  __LATENCY__: FLASH Latency
  *         The value of this parameter depend on device used within the same series
  * @retval none
  */
#define __HAL_FLASH_SET_LATENCY(__LATENCY__) \
                  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, (uint32_t)(__LATENCY__))

/**
  * @brief  Get the FLASH Latency.
  * @retval FLASH Latency
  *          The value of this parameter depend on device used within the same series
  */
#define __HAL_FLASH_GET_LATENCY()     (READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY))

/**
  * @brief  Enable the FLASH prefetch buffer.
  * @retval none
  */
#define __HAL_FLASH_PREFETCH_BUFFER_ENABLE()  (FLASH->ACR |= FLASH_ACR_PRFTEN)

/**
  * @brief  Disable the FLASH prefetch buffer.
  * @retval none
  */
#define __HAL_FLASH_PREFETCH_BUFFER_DISABLE()   (FLASH->ACR &= (~FLASH_ACR_PRFTEN))

/**
  * @brief  Enable the FLASH Adaptive Real-Time memory accelerator.
  * @note   The ART accelerator is available only for flash access on ITCM interface.
  * @retval none
  */
#define __HAL_FLASH_ART_ENABLE()  SET_BIT(FLASH->ACR, FLASH_ACR_ARTEN)

/**
  * @brief  Disable the FLASH Adaptive Real-Time memory accelerator.
  * @retval none
  */
#define __HAL_FLASH_ART_DISABLE()   CLEAR_BIT(FLASH->ACR, FLASH_ACR_ARTEN)

/**
  * @brief  Resets the FLASH Adaptive Real-Time memory accelerator.
  * @note   This function must be used only when the Adaptive Real-Time memory accelerator
  *         is disabled.
  * @retval None
  */
#define __HAL_FLASH_ART_RESET()  (FLASH->ACR |= FLASH_ACR_ARTRST)

/**
  * @brief  Enable the specified FLASH interrupt.
  * @param  __INTERRUPT__ : FLASH interrupt
  *         This parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP: End of FLASH Operation Interrupt
  *     @arg FLASH_IT_ERR: Error Interrupt
  * @retval none
  */
#define __HAL_FLASH_ENABLE_IT(__INTERRUPT__)  (FLASH->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the specified FLASH interrupt.
  * @param  __INTERRUPT__ : FLASH interrupt
  *         This parameter can be any combination of the following values:
  *     @arg FLASH_IT_EOP: End of FLASH Operation Interrupt
  *     @arg FLASH_IT_ERR: Error Interrupt
  * @retval none
  */
#define __HAL_FLASH_DISABLE_IT(__INTERRUPT__)  (FLASH->CR &= ~(uint32_t)(__INTERRUPT__))

/**
  * @brief  Get the specified FLASH flag status.
  * @param  __FLAG__: specifies the FLASH flag to check.
  *          This parameter can be one of the following values:
  *            @arg FLASH_FLAG_EOP   : FLASH End of Operation flag
  *            @arg FLASH_FLAG_OPERR : FLASH operation Error flag
  *            @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
  *            @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag
  *            @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *            @arg FLASH_FLAG_ERSERR : FLASH Erasing Sequence error flag
  *            @arg FLASH_FLAG_BSY   : FLASH Busy flag
  * @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_FLASH_GET_FLAG(__FLAG__)   ((FLASH->SR & (__FLAG__)))

/**
  * @brief  Clear the specified FLASH flag.
  * @param  __FLAG__: specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg FLASH_FLAG_EOP   : FLASH End of Operation flag
  *            @arg FLASH_FLAG_OPERR : FLASH operation Error flag
  *            @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
  *            @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag
  *            @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *            @arg FLASH_FLAG_ERSERR : FLASH Erasing Sequence error flag
  * @retval none
  */
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)   (FLASH->SR = (__FLAG__))
/**
  * @}
  */

/* Include FLASH HAL Extension module */
#include "stm32f7xx_hal_flash_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup FLASH_Exported_Functions
  * @{
  */
/** @addtogroup FLASH_Exported_Functions_Group1
  * @{
  */
/* Program operation functions  ***********************************************/
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
/* FLASH IRQ handler method */
void HAL_FLASH_IRQHandler(void);
/* Callbacks in non blocking modes */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
/**
  * @}
  */

/** @addtogroup FLASH_Exported_Functions_Group2
  * @{
  */
/* Peripheral Control functions  **********************************************/
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
/* Option bytes control */
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);
/**
  * @}
  */

/** @addtogroup FLASH_Exported_Functions_Group3
  * @{
  */
/* Peripheral State functions  ************************************************/
uint32_t HAL_FLASH_GetError(void);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Variables FLASH Private Variables
  * @{
  */

/**
  * @}
  */
/* Private constants ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Constants FLASH Private Constants
  * @{
  */

/**
  * @brief   OPTCR register byte 1 (Bits[15:8]) base address
  */
#define OPTCR_BYTE1_ADDRESS         ((uint32_t)0x40023C15)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup FLASH_Private_Macros FLASH Private Macros
  * @{
  */

/** @defgroup FLASH_IS_FLASH_Definitions FLASH Private macros to check input parameters
  * @{
  */
#define IS_FLASH_TYPEPROGRAM(VALUE)(((VALUE) == FLASH_TYPEPROGRAM_BYTE) || \
                                    ((VALUE) == FLASH_TYPEPROGRAM_HALFWORD) || \
                                    ((VALUE) == FLASH_TYPEPROGRAM_WORD) || \
                                    ((VALUE) == FLASH_TYPEPROGRAM_DOUBLEWORD))
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup FLASH_Private_Functions FLASH Private Functions
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

#endif /* __STM32F7xx_HAL_FLASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
