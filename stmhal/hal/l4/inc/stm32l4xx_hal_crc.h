/**
  ******************************************************************************
  * @file    stm32l4xx_hal_crc.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of CRC HAL module.
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
#ifndef __STM32L4xx_HAL_CRC_H
#define __STM32L4xx_HAL_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup CRC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** @defgroup CRC_Exported_Types CRC Exported Types
  * @{
  */

/** 
  * @brief  CRC HAL State Structure definition  
  */ 
typedef enum
{                                            
  HAL_CRC_STATE_RESET     = 0x00,  /*!< CRC not yet initialized or disabled */
  HAL_CRC_STATE_READY     = 0x01,  /*!< CRC initialized and ready for use   */
  HAL_CRC_STATE_BUSY      = 0x02,  /*!< CRC internal process is ongoing     */
  HAL_CRC_STATE_TIMEOUT   = 0x03,  /*!< CRC timeout state                   */
  HAL_CRC_STATE_ERROR     = 0x04   /*!< CRC error state                     */
}HAL_CRC_StateTypeDef;



/** 
  * @brief CRC Init Structure definition  
  */ 
typedef struct
{
  uint8_t DefaultPolynomialUse;       /*!< This parameter is a value of @ref CRC_Default_Polynomial and indicates if default polynomial is used.  
                                            If set to DEFAULT_POLYNOMIAL_ENABLE, resort to default 
                                            X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2+ X +1. 
                                            In that case, there is no need to set GeneratingPolynomial field.
                                            If otherwise set to DEFAULT_POLYNOMIAL_DISABLE, GeneratingPolynomial and CRCLength fields must be set. */

  uint8_t DefaultInitValueUse;        /*!< This parameter is a value of @ref CRC_Default_InitValue_Use and indicates if default init value is used. 
                                           If set to DEFAULT_INIT_VALUE_ENABLE, resort to default
                                           0xFFFFFFFF value. In that case, there is no need to set InitValue field.   
                                           If otherwise set to DEFAULT_INIT_VALUE_DISABLE,  InitValue field must be set. */

  uint32_t GeneratingPolynomial;      /*!< Set CRC generating polynomial as a 7, 8, 16 or 32-bit long value for a polynomial degree
                                           respectively equal to 7, 8, 16 or 32. This field is written in normal representation, 
                                           e.g., for a polynomial of degree 7, X^7 + X^6 + X^5 + X^2 + 1 is written 0x65.
                                           No need to specify it if DefaultPolynomialUse is set to DEFAULT_POLYNOMIAL_ENABLE.   */                                                

  uint32_t CRCLength;                 /*!< This parameter is a value of @ref CRC_Polynomial_Sizes and indicates CRC length.
                                           Value can be either one of
                                           @arg @ref CRC_POLYLENGTH_32B                  (32-bit CRC),
                                           @arg @ref CRC_POLYLENGTH_16B                  (16-bit CRC),
                                           @arg @ref CRC_POLYLENGTH_8B                   (8-bit CRC),
                                           @arg @ref CRC_POLYLENGTH_7B                   (7-bit CRC). */
                                              
  uint32_t InitValue;                 /*!< Init value to initiate CRC computation. No need to specify it if DefaultInitValueUse 
                                           is set to DEFAULT_INIT_VALUE_ENABLE.   */                                                
  
  uint32_t InputDataInversionMode;    /*!< This parameter is a value of @ref CRCEx_Input_Data_Inversion and specifies input data inversion mode. 
                                           Can be either one of the following values 
                                           @arg @ref CRC_INPUTDATA_INVERSION_NONE       no input data inversion
                                           @arg @ref CRC_INPUTDATA_INVERSION_BYTE       byte-wise inversion, 0x1A2B3C4D becomes 0x58D43CB2
                                           @arg @ref CRC_INPUTDATA_INVERSION_HALFWORD   halfword-wise inversion, 0x1A2B3C4D becomes 0xD458B23C
                                           @arg @ref CRC_INPUTDATA_INVERSION_WORD       word-wise inversion, 0x1A2B3C4D becomes 0xB23CD458 */  
                                              
  uint32_t OutputDataInversionMode;   /*!< This parameter is a value of @ref CRCEx_Output_Data_Inversion and specifies output data (i.e. CRC) inversion mode.
                                            Can be either 
                                            @arg @ref CRC_OUTPUTDATA_INVERSION_DISABLE   no CRC inversion, 
                                            @arg @ref CRC_OUTPUTDATA_INVERSION_ENABLE    CRC 0x11223344 is converted into 0x22CC4488 */                                           
}CRC_InitTypeDef;



/** 
  * @brief  CRC Handle Structure definition  
  */ 
typedef struct
{
  CRC_TypeDef                 *Instance;   /*!< Register base address        */ 
  
  CRC_InitTypeDef             Init;        /*!< CRC configuration parameters */
  
  HAL_LockTypeDef             Lock;        /*!< CRC Locking object           */
    
  __IO HAL_CRC_StateTypeDef   State;       /*!< CRC communication state      */
  
  uint32_t InputDataFormat;                /*!< This parameter is a value of @ref CRC_Input_Buffer_Format and specifies input data format. 
                                            Can be either 
                                            @arg @ref CRC_INPUTDATA_FORMAT_BYTES       input data is a stream of bytes (8-bit data)
                                            @arg @ref CRC_INPUTDATA_FORMAT_HALFWORDS   input data is a stream of half-words (16-bit data)
                                            @arg @ref CRC_INPUTDATA_FORMAT_WORDS       input data is a stream of words (32-bit data)
                                                                                                                                    
                                           Note that constant CRC_INPUT_FORMAT_UNDEFINED is defined but an initialization error
                                           must occur if InputBufferFormat is not one of the three values listed above  */ 
}CRC_HandleTypeDef;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CRC_Exported_Constants CRC Exported Constants
  * @{
  */

/** @defgroup CRC_Default_Polynomial_Value    Default CRC generating polynomial
  * @{
  */
#define DEFAULT_CRC32_POLY      0x04C11DB7  /*!<  X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2+ X +1 */
/**
  * @}
  */

/** @defgroup CRC_Default_InitValue    Default CRC computation initialization value
  * @{
  */
#define DEFAULT_CRC_INITVALUE   0xFFFFFFFF  /*!< Initial CRC default value */
/**
  * @}
  */

/** @defgroup CRC_Default_Polynomial    Indicates whether or not default polynomial is used
  * @{
  */
#define DEFAULT_POLYNOMIAL_ENABLE       ((uint8_t)0x00)  /*!< Enable default generating polynomial 0x04C11DB7  */
#define DEFAULT_POLYNOMIAL_DISABLE      ((uint8_t)0x01)  /*!< Disable default generating polynomial 0x04C11DB7 */
/**
  * @}
  */
 
/** @defgroup CRC_Default_InitValue_Use    Indicates whether or not default init value is used
  * @{
  */                                      
#define DEFAULT_INIT_VALUE_ENABLE      ((uint8_t)0x00) /*!< Enable initial CRC default value  */
#define DEFAULT_INIT_VALUE_DISABLE     ((uint8_t)0x01) /*!< Disable initial CRC default value */
/**
  * @}
  */

/** @defgroup CRC_Polynomial_Sizes Polynomial sizes to configure the IP
  * @{
  */
#define CRC_POLYLENGTH_32B                  ((uint32_t)0x00000000)        /*!< Resort to a 32-bit long generating polynomial */
#define CRC_POLYLENGTH_16B                  ((uint32_t)CRC_CR_POLYSIZE_0) /*!< Resort to a 16-bit long generating polynomial */
#define CRC_POLYLENGTH_8B                   ((uint32_t)CRC_CR_POLYSIZE_1) /*!< Resort to a 8-bit long generating polynomial  */
#define CRC_POLYLENGTH_7B                   ((uint32_t)CRC_CR_POLYSIZE)   /*!< Resort to a 7-bit long generating polynomial  */
/**
  * @}
  */

/** @defgroup CRC_Polynomial_Size_Definitions CRC polynomial possible sizes actual definitions
  * @{
  */
#define HAL_CRC_LENGTH_32B     32          /*!< 32-bit long CRC */
#define HAL_CRC_LENGTH_16B     16          /*!< 16-bit long CRC */
#define HAL_CRC_LENGTH_8B       8          /*!< 8-bit long CRC  */
#define HAL_CRC_LENGTH_7B       7          /*!< 7-bit long CRC  */
/**
  * @}
  */  

/** @defgroup CRC_Input_Buffer_Format Input Buffer Format
  * @{
  */
/* WARNING: CRC_INPUT_FORMAT_UNDEFINED is created for reference purposes but
 * an error is triggered in HAL_CRC_Init() if InputDataFormat field is set 
 * to CRC_INPUT_FORMAT_UNDEFINED: the format MUST be defined by the user for 
 * the CRC APIs to provide a correct result */ 
#define CRC_INPUTDATA_FORMAT_UNDEFINED             ((uint32_t)0x00000000)  /*!< Undefined input data format    */
#define CRC_INPUTDATA_FORMAT_BYTES                 ((uint32_t)0x00000001)  /*!< Input data in byte format      */
#define CRC_INPUTDATA_FORMAT_HALFWORDS             ((uint32_t)0x00000002)  /*!< Input data in half-word format */
#define CRC_INPUTDATA_FORMAT_WORDS                 ((uint32_t)0x00000003)  /*!< Input data in word format      */                                               
/**                                               
  * @}
  */
  
/** @defgroup CRC_Aliases CRC API aliases
  * @{
  */
#define HAL_CRC_Input_Data_Reverse   HAL_CRCEx_Input_Data_Reverse    /*!< Aliased to HAL_CRCEx_Input_Data_Reverse for inter STM32 series compatibility  */
#define HAL_CRC_Output_Data_Reverse  HAL_CRCEx_Output_Data_Reverse   /*!< Aliased to HAL_CRCEx_Output_Data_Reverse for inter STM32 series compatibility */
/**
  * @}
  */

/**
  * @}
  */


/* Exported macros -----------------------------------------------------------*/
/** @defgroup CRC_Exported_Macros CRC Exported Macros
  * @{
  */

/** @brief Reset CRC handle state.
  * @param  __HANDLE__: CRC handle.
  * @retval None
  */
#define __HAL_CRC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_CRC_STATE_RESET)

/**
  * @brief  Reset CRC Data Register.
  * @param  __HANDLE__: CRC handle
  * @retval None
  */
#define __HAL_CRC_DR_RESET(__HANDLE__) ((__HANDLE__)->Instance->CR |= CRC_CR_RESET)

/**
  * @brief  Set CRC INIT non-default value
  * @param  __HANDLE__: CRC handle
  * @param  __INIT__: 32-bit initial value  
  * @retval None
  */
#define __HAL_CRC_INITIALCRCVALUE_CONFIG(__HANDLE__, __INIT__) ((__HANDLE__)->Instance->INIT = (__INIT__))    

/**
  * @brief Store a 8-bit data in the Independent Data(ID) register.
  * @param __HANDLE__: CRC handle
  * @param __VALUE__: 8-bit value to be stored in the ID register
  * @retval None
  */
#define __HAL_CRC_SET_IDR(__HANDLE__, __VALUE__) (WRITE_REG((__HANDLE__)->Instance->IDR, (__VALUE__)))

/**
  * @brief Return the 8-bit data stored in the Independent Data(ID) register.
  * @param __HANDLE__: CRC handle
  * @retval 8-bit value of the ID register 
  */
#define __HAL_CRC_GET_IDR(__HANDLE__) (((__HANDLE__)->Instance->IDR) & CRC_IDR_IDR)
/**
  * @}
  */


/* Private macros --------------------------------------------------------*/
/** @addtogroup  CRC_Private_Macros   CRC Private Macros
  * @{
  */

#define IS_DEFAULT_POLYNOMIAL(DEFAULT) (((DEFAULT) == DEFAULT_POLYNOMIAL_ENABLE) || \
                                        ((DEFAULT) == DEFAULT_POLYNOMIAL_DISABLE))
                                        

#define IS_DEFAULT_INIT_VALUE(VALUE)  (((VALUE) == DEFAULT_INIT_VALUE_ENABLE) || \
                                       ((VALUE) == DEFAULT_INIT_VALUE_DISABLE))   
                                       
#define IS_CRC_POL_LENGTH(LENGTH)     (((LENGTH) == CRC_POLYLENGTH_32B) || \
                                       ((LENGTH) == CRC_POLYLENGTH_16B) || \
                                       ((LENGTH) == CRC_POLYLENGTH_8B)  || \
                                       ((LENGTH) == CRC_POLYLENGTH_7B))  

#define IS_CRC_INPUTDATA_FORMAT(FORMAT)           (((FORMAT) == CRC_INPUTDATA_FORMAT_BYTES) || \
                                                   ((FORMAT) == CRC_INPUTDATA_FORMAT_HALFWORDS) || \
                                                   ((FORMAT) == CRC_INPUTDATA_FORMAT_WORDS))                                                     

/**
  * @}
  */

/* Include CRC HAL Extended module */
#include "stm32l4xx_hal_crc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup CRC_Exported_Functions CRC Exported Functions
  * @{
  */

/* Initialization and de-initialization functions  ****************************/
/** @defgroup CRC_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */  
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
HAL_StatusTypeDef HAL_CRC_DeInit (CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);
/**
  * @}
  */

/* Peripheral Control functions ***********************************************/
/** @defgroup CRC_Exported_Functions_Group2 Peripheral Control functions
  * @{
  */
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
/**
  * @}
  */

/* Peripheral State and Error functions ***************************************/
/** @defgroup CRC_Exported_Functions_Group3 Peripheral State functions
  * @{
  */
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc);
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
#endif

#endif /* __STM32L4xx_HAL_CRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
