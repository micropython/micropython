/**
  ******************************************************************************
  * @file    stm32f2xx_hal_flash.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of FLASH HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F2xx_HAL_FLASH_H
#define __STM32F2xx_HAL_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/**
  * @brief  FLASH Error structure definition
  */
typedef enum
{ 
  FLASH_ERROR_RD =  0x01,
  FLASH_ERROR_PGS = 0x02,
  FLASH_ERROR_PGP = 0x04,
  FLASH_ERROR_PGA = 0x08,
  FLASH_ERROR_WRP = 0x10,
  FLASH_ERROR_OPERATION = 0x20
}FLASH_ErrorTypeDef;

/**
  * @brief  FLASH Procedure structure definition
  */
typedef enum 
{
  FLASH_PROC_NONE = 0, 
  FLASH_PROC_SECTERASE,
  FLASH_PROC_MASSERASE,
  FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;


/** 
  * @brief  FLASH handle Structure definition  
  */
typedef struct
{
  __IO FLASH_ProcedureTypeDef ProcedureOnGoing;   /*Internal variable to indicate which procedure is ongoing or not in IT context*/
  
  __IO uint32_t               NbSectorsToErase;   /*Internal variable to save the remaining sectors to erase in IT context*/
  
  __IO uint8_t                VoltageForErase;    /*Internal variable to provide voltange range selected by user in IT context*/
  
  __IO uint32_t               Sector;             /*Internal variable to define the current sector which is erasing*/
  
  __IO uint32_t               Address;            /*Internal variable to save address selected for program*/
  
  HAL_LockTypeDef             Lock;               /* FLASH locking object                */

  __IO FLASH_ErrorTypeDef     ErrorCode;          /* FLASH error code                    */

}FLASH_ProcessTypeDef;

/** 
  * @brief FLASH Error source  
  */ 
/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */  



/** @defgroup FLASH_Type_Program FLASH Type Program
  * @{
  */ 
#define TYPEPROGRAM_BYTE        ((uint32_t)0x00)  /*!< Program byte (8-bit) at a specified address           */
#define TYPEPROGRAM_HALFWORD    ((uint32_t)0x01)  /*!< Program a half-word (16-bit) at a specified address   */
#define TYPEPROGRAM_WORD        ((uint32_t)0x02)  /*!< Program a word (32-bit) at a specified address        */
#define TYPEPROGRAM_DOUBLEWORD  ((uint32_t)0x03)  /*!< Program a double word (64-bit) at a specified address */

#define IS_TYPEPROGRAM(VALUE)(((VALUE) == TYPEPROGRAM_BYTE) || \
                              ((VALUE) == TYPEPROGRAM_HALFWORD) || \
                              ((VALUE) == TYPEPROGRAM_WORD) || \
                              ((VALUE) == TYPEPROGRAM_DOUBLEWORD))  

/**
  * @}
  */

/** @defgroup FLASH_Flag_definition FLASH Flag definition
  * @brief Flag definition
  * @{
  */ 
#define FLASH_FLAG_EOP                 FLASH_SR_EOP            /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               FLASH_SR_SOP            /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR         /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              FLASH_SR_PGPERR         /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_PGSERR              FLASH_SR_PGSERR         /*!< FLASH Programming Sequence error flag     */
#define FLASH_FLAG_RDERR               ((uint32_t)0x00000100)  /*!< Read Protection error flag (PCROP)        */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY            /*!< FLASH Busy flag                           */ 

/**
  * @}
  */
  
/** @defgroup FLASH_Interrupt_definition FLASH Interrupt definition
  * @brief FLASH Interrupt definition
  * @{
  */ 
#define FLASH_IT_EOP                   FLASH_CR_EOPIE          /*!< End of FLASH Operation Interrupt source */
#define FLASH_IT_ERR                   ((uint32_t)0x02000000)  /*!< Error Interrupt source                  */

/**
  * @}
  */  

/** @defgroup FLASH_Program_Parallelism FLASH Program Parallelism
  * @{
  */
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)0x00000100)
#define FLASH_PSIZE_WORD           ((uint32_t)0x00000200)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)0x00000300)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFF)
/**
  * @}
  */ 

/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */ 
#define RDP_KEY                  ((uint16_t)0x00A5)
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
#define FLASH_OPT_KEY1           ((uint32_t)0x08192A3B)
#define FLASH_OPT_KEY2           ((uint32_t)0x4C5D6E7F)
/**
  * @}
  */ 

/** 
  * @brief   ACR register byte 0 (Bits[7:0]) base address  
  */ 
#define ACR_BYTE0_ADDRESS           ((uint32_t)0x40023C00) 
/** 
  * @brief   OPTCR register byte 0 (Bits[7:0]) base address  
  */ 
#define OPTCR_BYTE0_ADDRESS         ((uint32_t)0x40023C14)
/** 
  * @brief   OPTCR register byte 1 (Bits[15:8]) base address  
  */ 
#define OPTCR_BYTE1_ADDRESS         ((uint32_t)0x40023C15)
/** 
  * @brief   OPTCR register byte 2 (Bits[23:16]) base address  
  */ 
#define OPTCR_BYTE2_ADDRESS         ((uint32_t)0x40023C16)
/** 
  * @brief   OPTCR register byte 3 (Bits[31:24]) base address  
  */ 
#define OPTCR_BYTE3_ADDRESS         ((uint32_t)0x40023C17)

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Set the FLASH Latency.
  * @param  __LATENCY__: FLASH Latency                   
  *         The value of this parameter depend on device used within the same series
  * @retval none
  */ 
#define __HAL_FLASH_SET_LATENCY(__LATENCY__) (*(__IO uint8_t *)ACR_BYTE0_ADDRESS = (uint8_t)(__LATENCY__))

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
  * @brief  Enable the FLASH instruction cache.
  * @retval none
  */ 
#define __HAL_FLASH_INSTRUCTION_CACHE_ENABLE()  (FLASH->ACR |= FLASH_ACR_ICEN)

/**
  * @brief  Disable the FLASH instruction cache.
  * @retval none
  */ 
#define __HAL_FLASH_INSTRUCTION_CACHE_DISABLE()   (FLASH->ACR &= (~FLASH_ACR_ICEN))

/**
  * @brief  Enable the FLASH data cache.
  * @retval none
  */ 
#define __HAL_FLASH_DATA_CACHE_ENABLE()  (FLASH->ACR |= FLASH_ACR_DCEN)

/**
  * @brief  Disable the FLASH data cache.
  * @retval none
  */ 
#define __HAL_FLASH_DATA_CACHE_DISABLE()   (FLASH->ACR &= (~FLASH_ACR_DCEN))

/**
  * @brief  Resets the FLASH instruction Cache.
  * @note   This function must be used only when the Instruction Cache is disabled.  
  * @retval None
  */
#define __HAL_FLASH_INSTRUCTION_CACHE_RESET()  (FLASH->ACR |= FLASH_ACR_ICRST)

/**
  * @brief  Resets the FLASH data Cache.
  * @note   This function must be used only when the data Cache is disabled.  
  * @retval None
  */
#define __HAL_FLASH_DATA_CACHE_RESET()  (FLASH->ACR |= FLASH_ACR_DCRST)

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
  *            @arg FLASH_FLAG_PGSERR: FLASH Programming Sequence error flag
  *            @arg FLASH_FLAG_RDERR : FLASH Read Protection error flag (PCROP) 
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
  *            @arg FLASH_FLAG_PGSERR: FLASH Programming Sequence error flag
  *            @arg FLASH_FLAG_RDERR : FLASH Read Protection error flag (PCROP)   
  * @retval none
  */
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)   (FLASH->SR = (__FLAG__))

/* Include FLASH HAL Extension module */
#include "stm32f2xx_hal_flash_ex.h"

/* Exported functions --------------------------------------------------------*/
/* Program operation functions  ***********************************************/
HAL_StatusTypeDef   HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef   HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
/* FLASH IRQ handler method */
void                HAL_FLASH_IRQHandler(void);
/* Callbacks in non blocking modes */ 
void         HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void         HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);

/* Peripheral Control functions  **********************************************/
HAL_StatusTypeDef   HAL_FLASH_Unlock(void);
HAL_StatusTypeDef   HAL_FLASH_Lock(void);
HAL_StatusTypeDef   HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef   HAL_FLASH_OB_Lock(void);
/* Option bytes control */
HAL_StatusTypeDef   HAL_FLASH_OB_Launch(void);

/* Peripheral State functions  ************************************************/
FLASH_ErrorTypeDef  HAL_FLASH_GetError(void);

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);

/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2xx_HAL_FLASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
