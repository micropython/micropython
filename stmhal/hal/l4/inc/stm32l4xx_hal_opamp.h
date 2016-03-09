/**
  ******************************************************************************
  * @file    stm32l4xx_hal_opamp.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of OPAMP HAL module.
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
#ifndef __STM32L4xx_HAL_OPAMP_H
#define __STM32L4xx_HAL_OPAMP_H

#ifdef __cplusplus
 extern "C" {
#endif
                  
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup OPAMP
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** @defgroup OPAMP_Exported_Types OPAMP Exported Types
  * @{
  */

/** 
  * @brief  OPAMP Init structure definition  
  */
  
typedef struct
{
  uint32_t PowerSupplyRange;            /*!< Specifies the power supply range: above or under 2.4V.
                                             This parameter must be a value of @ref OPAMP_PowerSupplyRange
                                             Caution: This parameter is common to all OPAMP instances: a modification of this parameter for the selected OPAMP impacts the other OPAMP instances. */

  uint32_t PowerMode;                   /*!< Specifies the power mode Normal or Low-Power.
                                             This parameter must be a value of @ref OPAMP_PowerMode */
  
  uint32_t Mode;                        /*!< Specifies the OPAMP mode
                                             This parameter must be a value of @ref OPAMP_Mode 
                                             mode is either Standalone, - Follower or PGA */
                                    
  uint32_t InvertingInput;              /*!< Specifies the inverting input in Standalone & PGA modes
                                               - In Standalone mode:   i.e. when mode is OPAMP_STANDALONE_MODE
                                                 & PGA mode:           i.e. when mode is OPAMP_PGA_MODE
                                                 This parameter must be a value of @ref OPAMP_InvertingInput 
                                               - In Follower mode    i.e. when mode is OPAMP_FOLLOWER_MODE
                                                 This parameter is Not Applicable */ 

  uint32_t NonInvertingInput;           /*!< Specifies the non inverting input of the opamp: 
                                             This parameter must be a value of @ref OPAMP_NonInvertingInput */                                   
  
  uint32_t PgaGain;                     /*!< Specifies the gain in PGA mode 
                                             i.e. when mode is OPAMP_PGA_MODE. 
                                             This parameter must be a value of @ref OPAMP_PgaGain (2, 4, 8 or 16 ) */
 
  uint32_t UserTrimming;                /*!< Specifies the trimming mode 
                                             This parameter must be a value of @ref OPAMP_UserTrimming 
                                             UserTrimming is either factory or user trimming.*/
                                                                                                                            
  uint32_t TrimmingValueP;              /*!< Specifies the offset trimming value (PMOS)
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31 
                                             16 is typical default value */
                                        
  uint32_t TrimmingValueN;              /*!< Specifies the offset trimming value (NMOS)
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31 
                                             16 is typical default value */
  
  uint32_t TrimmingValuePLowPower;      /*!< Specifies the offset trimming value (PMOS)
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31  
                                             16 is typical default value */

  uint32_t TrimmingValueNLowPower;      /*!< Specifies the offset trimming value (NMOS)
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31  
                                             16 is typical default value */

}OPAMP_InitTypeDef;

/** 
  * @brief  HAL State structures definition  
  */ 

typedef enum
{
  HAL_OPAMP_STATE_RESET               = 0x00000000, /*!< OPAMP is not yet Initialized          */
  
  HAL_OPAMP_STATE_READY               = 0x00000001, /*!< OPAMP is initialized and ready for use */
  HAL_OPAMP_STATE_CALIBBUSY           = 0x00000002, /*!< OPAMP is enabled in auto calibration mode */
 
  HAL_OPAMP_STATE_BUSY                = 0x00000004, /*!< OPAMP is enabled and running in normal mode */                                                                           
  HAL_OPAMP_STATE_BUSYLOCKED          = 0x00000005  /*!< OPAMP is locked
                                                         only system reset allows reconfiguring the opamp. */
    
}HAL_OPAMP_StateTypeDef;

/** 
  * @brief OPAMP Handle Structure definition
  */ 
typedef struct
{
  OPAMP_TypeDef       *Instance;                    /*!< OPAMP instance's registers base address   */
  OPAMP_InitTypeDef   Init;                         /*!< OPAMP required parameters */
  HAL_StatusTypeDef Status;                         /*!< OPAMP peripheral status   */
  HAL_LockTypeDef   Lock;                           /*!< Locking object          */
  __IO HAL_OPAMP_StateTypeDef  State;               /*!< OPAMP communication state */
  
} OPAMP_HandleTypeDef;

/** 
  * @brief HAl_OPAMP_TrimmingValueTypeDef definition 
  */ 

typedef  uint32_t HAL_OPAMP_TrimmingValueTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup OPAMP_Exported_Constants OPAMP Exported Constants
  * @{
  */      

/** @defgroup OPAMP_Mode OPAMP Mode
  * @{
  */
#define OPAMP_STANDALONE_MODE            ((uint32_t)0x00000000) /*!< standalone mode */
#define OPAMP_PGA_MODE                   OPAMP_CSR_OPAMODE_1    /*!< PGA mode */
#define OPAMP_FOLLOWER_MODE              OPAMP_CSR_OPAMODE      /*!< follower mode */
    
/**
  * @}
  */                                        
                                                                             
/** @defgroup OPAMP_NonInvertingInput OPAMP Non Inverting Input
  * @{
  */

#define OPAMP_NONINVERTINGINPUT_IO0        ((uint32_t)0x00000000)  /*!< OPAMP non-inverting input connected to dedicated IO pin */
#define OPAMP_NONINVERTINGINPUT_DAC_CH      OPAMP_CSR_VPSEL        /*!< OPAMP non-inverting input connected internally to DAC channel */

/**
  * @}
  */

/** @defgroup OPAMP_InvertingInput OPAMP Inverting Input
  * @{
  */

#define OPAMP_INVERTINGINPUT_IO0              ((uint32_t)0x00000000)  /*!< OPAMP inverting input connected to dedicated IO pin low-leakage */
#define OPAMP_INVERTINGINPUT_IO1              OPAMP_CSR_VMSEL_0       /*!< OPAMP inverting input connected to alternative IO pin available on some device packages */
#define OPAMP_INVERTINGINPUT_CONNECT_NO       OPAMP_CSR_VMSEL_1       /*!< OPAMP inverting input not connected externally (PGA mode only) */

/**
  * @}
  */

/** @defgroup OPAMP_PgaGain OPAMP Pga Gain
  * @{
  */

#define OPAMP_PGA_GAIN_2                ((uint32_t)0x00000000)                        /*!< PGA gain =  2 */
#define OPAMP_PGA_GAIN_4                OPAMP_CSR_PGGAIN_0                            /*!< PGA gain =  4 */
#define OPAMP_PGA_GAIN_8                OPAMP_CSR_PGGAIN_1                            /*!< PGA gain =  8 */
#define OPAMP_PGA_GAIN_16              (OPAMP_CSR_PGGAIN_0 | OPAMP_CSR_PGGAIN_1)      /*!< PGA gain = 16 */

/**
  * @}
  */

/** @defgroup OPAMP_PowerMode OPAMP PowerMode
  * @{
  */
#define OPAMP_POWERMODE_NORMAL        ((uint32_t)0x00000000)
#define OPAMP_POWERMODE_LOWPOWER      OPAMP_CSR_OPALPM

/**
  * @}
  */

/** @defgroup OPAMP_PowerSupplyRange OPAMP PowerSupplyRange
  * @{
  */
#define OPAMP_POWERSUPPLY_LOW          ((uint32_t)0x00000000)  /*!< Power supply range low (VDDA lower than 2.4V) */
#define OPAMP_POWERSUPPLY_HIGH         OPAMP1_CSR_OPARANGE     /*!< Power supply range high (VDDA higher than 2.4V) */

/**
  * @}
  */ 

/** @defgroup OPAMP_UserTrimming OPAMP User Trimming
  * @{
  */
#define OPAMP_TRIMMING_FACTORY        ((uint32_t)0x00000000)                          /*!< Factory trimming */
#define OPAMP_TRIMMING_USER           OPAMP_CSR_USERTRIM                              /*!< User trimming */

#define IS_OPAMP_TRIMMING(TRIMMING) (((TRIMMING) == OPAMP_TRIMMING_FACTORY) || \
                                     ((TRIMMING) == OPAMP_TRIMMING_USER))
/**
  * @}
  */

/** @defgroup OPAMP_FactoryTrimming OPAMP Factory Trimming
  * @{
  */
#define OPAMP_FACTORYTRIMMING_DUMMY    ((uint32_t)0xFFFFFFFF)                           /*!< Dummy value if trimming value could not be retrieved */

#define OPAMP_FACTORYTRIMMING_N        ((uint32_t)0x00000000)                          /*!< Offset trimming N */
#define OPAMP_FACTORYTRIMMING_P        ((uint32_t)0x00000001)                          /*!< Offset trimming P */

/**
  * @}
  */           

 /**
  * @}
  */ 

/* Private constants ---------------------------------------------------------*/
/** @defgroup OPAMP_Private_Constants OPAMP Private Constants
  * @brief   OPAMP Private constants and defines
  * @{
  */

/* NONINVERTING bit position in OTR & LPOTR */ 
#define OPAMP_INPUT_NONINVERTING              ((uint32_t) 8) /*!< Non inverting input */  

/* Offset trimming time: during calibration, minimum time needed between two  */
/* steps to have 1 mV accuracy.                                               */
/* Refer to datasheet, electrical characteristics: parameter tOFFTRIM Typ=1ms.*/
/* Unit: ms.                                                                  */
#define OPAMP_TRIMMING_DELAY               ((uint32_t) 1)

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup OPAMP_Exported_Macros OPAMP Exported Macros
  * @{
  */

/** @brief Reset OPAMP handle state.
  * @param  __HANDLE__: OPAMP handle.
  * @retval None
  */
#define __HAL_OPAMP_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_OPAMP_STATE_RESET)

/**
  * @}
  */ 

/* Private macro -------------------------------------------------------------*/

/** @defgroup OPAMP_Private_Macros OPAMP Private Macros
  * @{
  */

#define IS_OPAMP_FUNCTIONAL_NORMALMODE(INPUT) (((INPUT) == OPAMP_STANDALONE_MODE) || \
                                               ((INPUT) == OPAMP_PGA_MODE) || \
                                               ((INPUT) == OPAMP_FOLLOWER_MODE))

#define IS_OPAMP_INVERTING_INPUT_STANDALONE(INPUT) (((INPUT) == OPAMP_INVERTINGINPUT_IO0) || \
                                                    ((INPUT) == OPAMP_INVERTINGINPUT_IO1))

#define IS_OPAMP_NONINVERTING_INPUT(INPUT) (((INPUT) == OPAMP_NONINVERTINGINPUT_IO0) || \
                                            ((INPUT) == OPAMP_NONINVERTINGINPUT_DAC_CH))

#define IS_OPAMP_INVERTING_INPUT_PGA(INPUT) (((INPUT) == OPAMP_INVERTINGINPUT_IO0) || \
                                             ((INPUT) == OPAMP_INVERTINGINPUT_IO1)|| \
                                             ((INPUT) == OPAMP_INVERTINGINPUT_CONNECT_NO))

#define IS_OPAMP_PGA_GAIN(GAIN) (((GAIN) == OPAMP_PGA_GAIN_2) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_4) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_8) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_16))

#define IS_OPAMP_POWERMODE(TRIMMING) (((TRIMMING) == OPAMP_POWERMODE_NORMAL) || \
                                      ((TRIMMING) == OPAMP_POWERMODE_LOWPOWER) )

#define IS_OPAMP_POWER_SUPPLY_RANGE(RANGE) (((RANGE) == OPAMP_POWERSUPPLY_LOW) || \
                                            ((RANGE) == OPAMP_POWERSUPPLY_HIGH)  )

#define IS_OPAMP_TRIMMING(TRIMMING) (((TRIMMING) == OPAMP_TRIMMING_FACTORY) || \
                                     ((TRIMMING) == OPAMP_TRIMMING_USER))


#define IS_OPAMP_TRIMMINGVALUE(TRIMMINGVALUE) ((TRIMMINGVALUE) <= 0x1F)

#define IS_OPAMP_FACTORYTRIMMING(TRIMMING) (((TRIMMING) == OPAMP_FACTORYTRIMMING_N) || \
                                             ((TRIMMING) == OPAMP_FACTORYTRIMMING_P))

/**
  * @}
  */ 

/* Include OPAMP HAL Extended module */
#include "stm32l4xx_hal_opamp_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup OPAMP_Exported_Functions
  * @{
  */

/** @addtogroup OPAMP_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_OPAMP_Init(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_DeInit (OPAMP_HandleTypeDef *hopamp);
void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef *hopamp);
void HAL_OPAMP_MspDeInit(OPAMP_HandleTypeDef *hopamp);
/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group2
  * @{
  */

/* I/O operation functions  *****************************************************/
HAL_StatusTypeDef HAL_OPAMP_Start(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_Stop(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_SelfCalibrate(OPAMP_HandleTypeDef *hopamp); 

/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group3
  * @{
  */

/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef HAL_OPAMP_Lock(OPAMP_HandleTypeDef *hopamp); 
HAL_OPAMP_TrimmingValueTypeDef HAL_OPAMP_GetTrimOffset (OPAMP_HandleTypeDef *hopamp, uint32_t trimmingoffset);

/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group4
  * @{
  */

/* Peripheral State functions  **************************************************/
HAL_OPAMP_StateTypeDef HAL_OPAMP_GetState(OPAMP_HandleTypeDef *hopamp);

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

#endif /* __STM32L4xx_HAL_OPAMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
